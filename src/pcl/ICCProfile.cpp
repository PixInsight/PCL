//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/ICCProfile.cpp - Released 2017-07-18T16:14:00Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#include <pcl/AutoLock.h>
#include <pcl/EndianConversions.h>
#include <pcl/File.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfile.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#include <lcms/lcms2.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ICC profile header structure, 4.3.0.0 specification.
 */
struct ICCHeader
{
   uint32   size;
   uint32   CMMType;
   uint32   version;
   uint32   profileClass;
   uint32   dataColorSpace;
   uint32   connectionSpace;
   uint8    creationDateTime[ 12 ];
   uint32   signature;
   uint32   primaryPlatform;
   uint32   flags;
   uint32   deviceManufacturer;
   uint32   deviceModel;
   uint64   deviceAttributes;
   uint32   renderingIntent;
   uint32   illuminantX;
   uint32   illuminantY;
   uint32   illuminantZ;
   uint32   creatorSignature;
   uint8    profileID[ 16 ];
   uint8    reserved[ 28 ];

   size_type Size() const
   {
      return size_type( BigToLittleEndian( size ) );
   }

   uint32 Signature() const
   {
      return BigToLittleEndian( signature );
   }
};

// ----------------------------------------------------------------------------

#define ICC_MAGIC_NUMBER   0x61637370 // 'acsp'
#define ICC_FLAGS_BYTE_IDX 44

static bool CanBeProfile( const ICCHeader* h )
{
   return h != nullptr &&
          h->Size() > sizeof( ICCHeader ) &&
          h->Signature() == ICC_MAGIC_NUMBER;
}

static bool CanBeProfile( const ByteArray& x )
{
   return CanBeProfile( reinterpret_cast<const ICCHeader*>( x.Begin() ) );
}

static bool CanBeProfile( const void* x )
{
   return CanBeProfile( reinterpret_cast<const ICCHeader*>( x ) );
}

// ----------------------------------------------------------------------------

/*
 * Thread-safe LCMS error log.
 */
class LCMSErrorMessage
{
public:

   static void InstallHandlers();
   static void Throw( const String& info );
   static String Get();

   static cmsContext CheckContext()
   {
      return s_checkContext;
   }

   bool operator ==( const LCMSErrorMessage& x ) const
   {
      return thread == x.thread;
   }

   bool operator <( const LCMSErrorMessage& x ) const
   {
      return thread < x.thread;
   }

private:

   typedef SortedArray<LCMSErrorMessage> message_list;

   static Mutex        s_mutex;
   static message_list s_messages;
   static cmsContext   s_checkContext;

   thread_handle thread;
   String        message;

   LCMSErrorMessage();
   LCMSErrorMessage( uint32 errorCode, const char* errorText );

   static void LogError( cmsContext, uint32 errorCode, const char* errorText );
};

Mutex                          LCMSErrorMessage::s_mutex;
LCMSErrorMessage::message_list LCMSErrorMessage::s_messages;
cmsContext                     LCMSErrorMessage::s_checkContext = nullptr;

LCMSErrorMessage::LCMSErrorMessage() :
   thread( (*API->Thread->GetCurrentThread)() )
{
}

LCMSErrorMessage::LCMSErrorMessage( uint32 errorCode, const char* errorText ) :
   thread( (*API->Thread->GetCurrentThread)() ),
   message( String().Format( "LCMS Error (%u): ", errorCode ) + errorText )
{
}

void LCMSErrorMessage::LogError( cmsContext, uint32 errorCode, const char* errorText )
{
   volatile AutoLock lock( s_mutex );
   s_messages.Add( LCMSErrorMessage( errorCode, errorText ) );
}

void LCMSErrorMessage::InstallHandlers()
{
   static AtomicInt done;
   if ( !done )
   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );
      if ( done.Load() == 0 )
      {
         s_checkContext = ::cmsCreateContext( nullptr/*Plugin*/, nullptr/*UserData*/ );
         ::cmsSetLogErrorHandler( LogError );
         done.Store( 1 );
      }
   }
}

void LCMSErrorMessage::Throw( const String& info )
{
   String message = info;
   String lcmsMessage = LCMSErrorMessage::Get();
   if ( !lcmsMessage.IsEmpty() )
      message << '\n' << lcmsMessage;
   throw Error( message );
}

String LCMSErrorMessage::Get()
{
   volatile AutoLock lock( s_mutex );
   String message;
   message_list::const_iterator i = s_messages.Search( LCMSErrorMessage() );
   if ( i != s_messages.End() )
   {
      message = i->message;
      message_list::const_iterator j = i;
      for ( ; ++j != s_messages.End() && *j == *i; )
         message << '\n' << j->message;
      s_messages.Remove( i, j );
   }
   return message;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define thisHeader reinterpret_cast<const ICCHeader*>( m_data.Begin() )

// ----------------------------------------------------------------------------

void ICCProfile::ThrowErrorWithCMSInfo( const String& message )
{
   LCMSErrorMessage::InstallHandlers();
   LCMSErrorMessage::Throw( message );
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsSameProfile( const ICCProfile& other ) const
{
#define otherHeader reinterpret_cast<const ICCHeader*>( other.m_data.Begin() )

   if ( m_data.IsEmpty() )
      return other.m_data.IsEmpty();
   if ( other.m_data.IsEmpty() )
      return false;
   size_type size = thisHeader->Size();
   if ( otherHeader->Size() != size )
      return false;
   return ::memcmp( m_data.Begin(), other.m_data.Begin(), size ) == 0;

#undef otherHeader
}

// ----------------------------------------------------------------------------

size_type ICCProfile::ProfileSize() const
{
   return m_data.IsEmpty() ? size_type( 0 ) : thisHeader->Size();
}

// ----------------------------------------------------------------------------

String ICCProfile::Description( const char* language, const char* country ) const
{
   if ( m_data.IsEmpty() )
      return String();
   handle h = Open();
   String info = Description( h, language, country );
   Close( h );
   return info;
}

// ----------------------------------------------------------------------------

String ICCProfile::Manufacturer( const char* language, const char* country ) const
{
   if ( m_data.IsEmpty() )
      return String();
   handle h = Open();
   String info = Manufacturer( h, language, country );
   Close( h );
   return info;
}

// ----------------------------------------------------------------------------

String ICCProfile::Model( const char* language, const char* country ) const
{
   if ( m_data.IsEmpty() )
      return String();
   handle h = Open();
   String info = Model( h, language, country );
   Close( h );
   return info;
}

// ----------------------------------------------------------------------------

String ICCProfile::Copyright( const char* language, const char* country ) const
{
   if ( m_data.IsEmpty() )
      return String();
   handle h = Open();
   String info = Copyright( h, language, country );
   Close( h );
   return info;
}

// ----------------------------------------------------------------------------

void ICCProfile::GetInformation( String& description, String& manufacturer, String& model, String& copyright,
                                 const char* language, const char* country ) const
{
   if ( !m_data.IsEmpty() )
   {
      handle h = Open();
      description = Description( h, language, country );
      manufacturer = Manufacturer( h, language, country );
      model = Model( h, language, country );
      copyright = Copyright( h, language, country );
      Close( h );
   }
   else
   {
      description.Clear();
      manufacturer.Clear();
      model.Clear();
      copyright.Clear();
   }
}

// ----------------------------------------------------------------------------

ICCProfile::profile_class ICCProfile::Class() const
{
   if ( m_data.IsEmpty() )
      return ICCProfileClass::Unknown;
   handle h = Open();
   ICCProfile::profile_class pc = Class( h );
   Close( h );
   return pc;
}

// ----------------------------------------------------------------------------

ICCProfile::color_space ICCProfile::ColorSpace() const
{
   if ( m_data.IsEmpty() )
      return ICCColorSpace::Unknown;
   handle h = Open();
   ICCProfile::color_space cs = ColorSpace( h );
   Close( h );
   return cs;
}

// ----------------------------------------------------------------------------

bool ICCProfile::SupportsRenderingIntent( ICCProfile::rendering_intent intent, ICCProfile::rendering_direction direction ) const
{
   if ( m_data.IsEmpty() )
      return false;
   handle h = Open();
   bool b = SupportsRenderingIntent( h, intent, direction );
   Close( h );
   return b;
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsEmbedded() const
{
   return !m_data.IsEmpty() && (m_data[ICC_FLAGS_BYTE_IDX] & 1) != 0;
}

// ----------------------------------------------------------------------------

void ICCProfile::SetEmbeddedFlag( bool on )
{
   if ( !m_data.IsEmpty() )
   {
      m_data.EnsureUnique();
      if ( on )
         m_data[ICC_FLAGS_BYTE_IDX] |= uint8( 1 );
      else
         m_data[ICC_FLAGS_BYTE_IDX] &= ~uint8( 1 );
   }
}

// ----------------------------------------------------------------------------

void ICCProfile::Load( const String& path )
{
   try
   {
      Clear();

      m_path = path.Trimmed();
      if ( m_path.IsEmpty() )
         throw Error( "Empty profile file path." );
      if ( !File::Exists( m_path ) )
         throw Error( "The profile file does not exist: " + m_path );

      File file = File::OpenFileForReading( m_path );

      fsize_type size = file.Size();
      if ( size_type( size ) < sizeof( ICCHeader ) )
         throw Error( "Invalid or corrupted ICC profile: " + m_path );

      m_data = ByteArray( size );
      file.Read( m_data.Begin(), size );

      if ( !CanBeProfile( m_data ) )
         throw Error( "Invalid or corrupted ICC profile: " + m_path );

      file.Close();
   }
   catch ( ... )
   {
      Clear();
      throw;
   }
}

// ----------------------------------------------------------------------------

void ICCProfile::Set( const ByteArray& icc )
{
   Clear();

   if ( !icc.IsEmpty() )
   {
      if ( !CanBeProfile( icc ) )
         throw Error( String().Format( "Invalid or corrupted ICC profile data at %p", icc.Begin() ) );
      m_data = icc;
   }
}

// ----------------------------------------------------------------------------

void ICCProfile::Set( const void* data )
{
#define header reinterpret_cast<const ICCHeader*>( data )

   Clear();

   if ( !CanBeProfile( header ) )
      throw Error( String().Format( "Invalid or corrupted ICC profile data at %p", data ) );
   const uint8* begin = reinterpret_cast<const uint8*>( data );
   m_data = ByteArray( begin, begin + header->Size() );

#undef header
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static SortedArray<ICCProfile::handle> s_handles;
static Mutex                           s_mutex;

static void ValidateHandle( ICCProfile::handle h )
{
   if ( !ICCProfile::IsValidHandle( h ) )
      throw Error( String().Format( "Invalid ICC profile handle %p", h ) );
}

// ----------------------------------------------------------------------------

ICCProfile::handle ICCProfile::Open( const String& filePath )
{
   String path = filePath.Trimmed();
   if ( path.IsEmpty() )
      throw Error( "ICCProfile::Open(): Empty file path." );
   if ( !File::Exists( path ) )
      throw Error( "ICCProfile::Open(): No such file: " + path );

   IsoString path8 =
#ifdef __PCL_WINDOWS
      File::UnixPathToWindows( path ).ToMBS();
#else
      path.ToUTF8();
#endif
   LCMSErrorMessage::InstallHandlers();
   handle h = ::cmsOpenProfileFromFile( path8.c_str(), "r" );
   if ( h == nullptr )
      LCMSErrorMessage::Throw( "Cannot open ICC profile: " + path );

   {
      volatile AutoLock lock( s_mutex );
      s_handles.Add( h );
      return h;
   }
}

// ----------------------------------------------------------------------------

ICCProfile::handle ICCProfile::Open( const void* data )
{
   if ( !CanBeProfile( data ) )
      throw Error( "ICCProfile::Open(): Invalid ICC profile structure." );
   LCMSErrorMessage::InstallHandlers();
   handle h = ::cmsOpenProfileFromMem( data, uint32( reinterpret_cast<const ICCHeader*>( data )->Size() ) );
   if ( h == nullptr )
      LCMSErrorMessage::Throw( String().Format( "Cannot open ICC profile from data at %p", data ) );

   {
      volatile AutoLock lock( s_mutex );
      s_handles.Add( h );
      return h;
   }
}

// ----------------------------------------------------------------------------

void ICCProfile::Close( ICCProfile::handle h )
{
   ValidateHandle( h );
   LCMSErrorMessage::InstallHandlers();
   ::cmsCloseProfile( cmsHPROFILE( h ) );
   {
      volatile AutoLock lock( s_mutex );
      s_handles.Remove( h );
   }
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsValidHandle( ICCProfile::handle h )
{
   volatile AutoLock lock( s_mutex );
   return s_handles.Contains( h );
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsValidFile( const String& filePath )
{
   String path = filePath.Trimmed();
   if ( !path.IsEmpty() )
      if ( File::Exists( path ) )
      {
         IsoString path8 =
#ifdef __PCL_WINDOWS
            File::UnixPathToWindows( path ).ToMBS();
#else
            path.ToUTF8();
#endif

         LCMSErrorMessage::InstallHandlers();
         cmsHPROFILE h = ::cmsOpenProfileFromFileTHR( LCMSErrorMessage::CheckContext(), path8.c_str(), "r" );
         if ( h != nullptr )
         {
            ::cmsCloseProfile( h );
            return true;
         }
      }

   return false;
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsValid( const void* data )
{
   if ( CanBeProfile( data ) )
   {
      LCMSErrorMessage::InstallHandlers();
      cmsHPROFILE h = ::cmsOpenProfileFromMemTHR( LCMSErrorMessage::CheckContext(), data,
                                                  uint32( reinterpret_cast<const ICCHeader*>( data )->Size() ) );
      if ( h != nullptr )
      {
         ::cmsCloseProfile( h );
         return true;
      }
   }

   return false;
}

// ----------------------------------------------------------------------------

static String ProfileInfo( cmsInfoType info, ICCProfile::handle h, const char* language, const char* country )
{
   ValidateHandle( h );

   if ( language == nullptr || *language == '\0' )
      language = "en";
   if ( country == nullptr || *country == '\0' )
      country = "US";

   LCMSErrorMessage::InstallHandlers();
   uint32 n = ::cmsGetProfileInfo( h, info, language, country, 0, 0 );
   if ( n == 0 )
      return String();
   Array<wchar_t> ws( n+1 );
   ws[n] = 0;
   (void)::cmsGetProfileInfo( h, info, language, country, ws.Begin(), n );
   return String( ws.Begin() ).Trimmed();
}

String ICCProfile::Description( ICCProfile::handle h, const char* language, const char* country )
{
   return ProfileInfo( cmsInfoDescription, h, language, country );
}

String ICCProfile::Manufacturer( ICCProfile::handle h, const char* language, const char* country )
{
   return ProfileInfo( cmsInfoManufacturer, h, language, country );
}

String ICCProfile::Model( ICCProfile::handle h, const char* language, const char* country )
{
   return ProfileInfo( cmsInfoModel, h, language, country );
}

String ICCProfile::Copyright( ICCProfile::handle h, const char* language, const char* country )
{
   return ProfileInfo( cmsInfoCopyright, h, language, country );
}

// ----------------------------------------------------------------------------

ICCProfile::profile_class ICCProfile::Class( ICCProfile::handle h )
{
   ValidateHandle( h );

   switch ( ::cmsGetDeviceClass( h ) )
   {
   case cmsSigInputClass:      return ICCProfileClass::InputDevice;
   case cmsSigDisplayClass:    return ICCProfileClass::DisplayDevice;
   case cmsSigOutputClass:     return ICCProfileClass::OutputDevice;
   case cmsSigLinkClass:       return ICCProfileClass::DeviceLink;
   case cmsSigAbstractClass:   return ICCProfileClass::AbstractProfile;
   case cmsSigColorSpaceClass: return ICCProfileClass::ColorSpaceConversion;
   case cmsSigNamedColorClass: return ICCProfileClass::NamedColorProfile;
   default:                    return ICCProfileClass::Unknown;
   }
}

// ----------------------------------------------------------------------------

ICCProfile::color_space ICCProfile::ColorSpace( ICCProfile::handle h )
{
   ValidateHandle( h );

   switch ( ::cmsGetColorSpace( h ) )
   {
   case cmsSigXYZData:   return ICCColorSpace::XYZ;
   case cmsSigLabData:   return ICCColorSpace::Lab;
   case cmsSigLuvData:   return ICCColorSpace::Luv;
   case cmsSigYCbCrData: return ICCColorSpace::YCbCr;
   case cmsSigYxyData:   return ICCColorSpace::Yxy;
   case cmsSigRgbData:   return ICCColorSpace::RGB;
   case cmsSigGrayData:  return ICCColorSpace::Gray;
   case cmsSigHsvData:   return ICCColorSpace::HSV;
   case cmsSigHlsData:   return ICCColorSpace::HLS;
   case cmsSigCmykData:  return ICCColorSpace::CMYK;
   case cmsSigCmyData:   return ICCColorSpace::CMY;
   case cmsSigLuvKData:  return ICCColorSpace::LuvK;
   default:              return ICCColorSpace::Unknown;
   }
}

// ----------------------------------------------------------------------------

bool ICCProfile::SupportsRenderingIntent( ICCProfile::handle h,
                                          ICCProfile::rendering_intent intent,
                                          ICCProfile::rendering_direction direction )
{
   ValidateHandle( h );

   uint32 cmsIntent;
   switch ( intent )
   {
   case ICCRenderingIntent::Perceptual:           cmsIntent = INTENT_PERCEPTUAL;
   case ICCRenderingIntent::RelativeColorimetric: cmsIntent = INTENT_RELATIVE_COLORIMETRIC;
   case ICCRenderingIntent::Saturation:           cmsIntent = INTENT_SATURATION;
   case ICCRenderingIntent::AbsoluteColorimetric: cmsIntent = INTENT_ABSOLUTE_COLORIMETRIC;
   default: return false; // ?!
   }

   uint32 cmsDirection;
   switch ( direction )
   {
   case ICCRenderingDirection::Input:    cmsDirection = LCMS_USED_AS_INPUT;
   case ICCRenderingDirection::Output:   cmsDirection = LCMS_USED_AS_OUTPUT;
   case ICCRenderingDirection::Proofing: cmsDirection = LCMS_USED_AS_PROOF;
   default: return false; // ?!
   }

   return ::cmsIsIntentSupported( h, cmsIntent, cmsDirection );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

StringList ICCProfile::ProfileDirectories()
{
   StringList directories;

   for ( int i = 0; ; ++i )
   {
      size_type len = 0;
      (*API->Global->GetProfilesDirectory)( i, 0, &len );
      if ( len == 0 )
         break;

      String path;
      path.SetLength( len );
      if ( (*API->Global->GetProfilesDirectory)( i, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetProfilesDirectory" );
      path.ResizeToNullTerminated();

      directories.Add( path );
   }

   return directories;
}

// ----------------------------------------------------------------------------

static String FindProfileRecursively( const String& description, const String& dirPath, bool exactMatch )
{
   pcl::FindFileInfo info;

   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( !info.IsDirectory() && !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
      {
         /*
          * N.B.: Don't restrict the search to .icc or .icm files because on
          * some platforms ICC profiles can have no file suffixes, or even
          * nonstandard suffixes.
          */
         //String ext = File::ExtractExtension( info.name );
         //if ( !ext.CompareIC( ".icc" ) || !ext.CompareIC( ".icm" ) )

         try
         {
            String filePath = File::FullPath( dirPath + "/" + info.name );
            ICCProfile::handle h = ICCProfile::Open( filePath );
            String desc = ICCProfile::Description( h );
            ICCProfile::Close( h );
            if ( exactMatch ? (desc == description) : desc.ContainsIC( description ) )
               return filePath;
         }
         catch ( ... )
         {
         }
      }

   StringList directories;
   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( info.IsDirectory() && info.name != "." && info.name != ".." )
         directories.Add( info.name );

   for ( const String& dir : directories )
   {
      String filePath = FindProfileRecursively( description, dirPath + '/' + dir, exactMatch );
      if ( !filePath.IsEmpty() )
         return filePath;
   }

   return String();
}

String ICCProfile::FindInstalledProfile( const String& description, bool exactMatch )
{
   StringList directories = ProfileDirectories();
   for ( const String& dir : directories )
   {
      String filePath = FindProfileRecursively( description, dir, exactMatch );
      if ( !filePath.IsEmpty() )
         return filePath;
   }

   return String();
}

// ----------------------------------------------------------------------------

static void FindProfilesRecursively( StringList& list, const String& dirPath )
{
   pcl::FindFileInfo info;

   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( !info.IsDirectory() && !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
      {
         /*
          * N.B.: Don't restrict the search to .icc or .icm files because on
          * some platforms ICC profiles can have no file suffixes, or even
          * nonstandard suffixes.
          */
         //String ext = File::ExtractExtension( info.name );
         //if ( !ext.CompareIC( ".icc" ) || !ext.CompareIC( ".icm" ) )

         try
         {
            String filePath = File::FullPath( dirPath + "/" + info.name );
            ICCHeader header;
            File f = File::OpenFileForReading( filePath );
            f.Read( &header, sizeof( ICCHeader ) );
            f.Close();
            if ( CanBeProfile( &header ) )
               list.Add( filePath );
         }
         catch ( ... )
         {
         }
      }

   StringList directories;
   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( info.IsDirectory() && info.name != "." && info.name != ".." )
         directories.Add( info.name );

   for ( const String& dir : directories )
      FindProfilesRecursively( list, dirPath + '/' + dir );
}

StringList ICCProfile::FindProfiles( const String& dirPath )
{
   StringList list;

#ifdef __PCL_WINDOWS
   String path = File::WindowsPathToUnix( dirPath.Trimmed() );
#else
   String path = dirPath.Trimmed();
#endif
   if ( path.IsEmpty() )
   {
      StringList directories = ProfileDirectories();
      for ( const String& dir : directories )
         FindProfilesRecursively( list, dir );
   }
   else
   {
      // Strip away a trailing slash, except for root directories.
      if ( path.Length() > 1 && path.EndsWith( '/' )
#ifdef __PCL_WINDOWS
        && !(path.Length() == 3 && path[1] == ':')
#endif
         )
         path.Delete( path.UpperBound() );

      FindProfilesRecursively( list, path );
   }

   return list;
}

// ----------------------------------------------------------------------------

/*
 * Extract a subset of profile paths and their descriptions from a previously
 * retrieved list of full paths to ICC profile disk files.
 *
 * selectedDescriptionsList   [output] List of ICC profile descriptions for
 *                            selected ICC profiles.
 *
 * selectedPathsList          [output] List of ICC profile full file paths for
 *                            selected ICC profiles.
 *
 * pathList                   Source list of ICC profile full file paths. This
 *                            can be the result of a previous call to
 *                            ICCProfile::Find().
 *
 * colorSpace                 Color spaces (or'ed ICCColorSpace constants) of
 *                            profiles to extract. Zero means any color space.
 *
 * profileClass               Profile classes (or'ed ICCProfileClass constants)
 *                            of profiles to extract. Zero means any class.
 */
void ICCProfile::ExtractProfileList( StringList& selectedDescriptionsList,
                                     StringList& selectedPathsList,
                                     const StringList& pathList,
                                     ICCColorSpaces colorSpace,
                                     ICCProfileClasses profileClass )
{
   for ( const String& path : pathList )
   {
      try
      {
         handle h = Open( path );
         if ( !profileClass || profileClass.IsFlagSet( Class( h ) ) )
            if ( !colorSpace || colorSpace.IsFlagSet( ColorSpace( h ) ) )
            {
               try
               {
                  String desc = Description( h );
                  if ( !desc.IsEmpty() )
                  {
                     selectedDescriptionsList.Add( desc );
                     selectedPathsList.Add( path );
                  }
               }
               catch ( ... )
               {
               }
            }

         Close( h );
      }
      catch ( ... )
      {
      }
   }
}

// ----------------------------------------------------------------------------

ICCProfile::profile_list ICCProfile::FindProfilesByColorSpace( ICCColorSpaces colorSpaces )
{
   StringList all = FindProfiles();

   StringList descriptions;
   StringList paths;
   ExtractProfileList( descriptions, paths, all, colorSpaces );

   profile_list rawList;
   for ( StringList::const_iterator i = descriptions.Begin(), j = paths.Begin(); i != descriptions.End(); ++i, ++j )
      rawList.Add( Info( *i, *j ) );

   // Remove duplicates
   profile_list profiles;
   for ( profile_list::const_iterator i = rawList.Begin(), j = i; i != rawList.End(); ++i )
      if ( j == i || *i != *j )
      {
         j = i;
         profiles.Add( *i );
      }

   return profiles;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ICCProfile.cpp - Released 2017-07-18T16:14:00Z
