// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/File.cpp - Released 2014/10/29 07:34:21 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include <pcl/Defs.h>

#ifdef __PCL_WINDOWS
#  include <windows.h>
#  include <sys/stat.h>
#  include <sys/utime.h>
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/statvfs.h>
#  include <limits.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <stdlib.h>
#  include <stdio.h>
#  include <errno.h>
#  include <utime.h>
#endif

#include <time.h>

#include <pcl/File.h>
#include <pcl/FileInfo.h>
#include <pcl/Arguments.h>
#include <pcl/Math.h>
#include <pcl/Random.h>

#define THROW( x )   throw x( FilePath() )

#define CHECK_OPEN_FILE( fp )                                                                         \
   if ( !IsOpen() )                                                                                   \
      throw File::Error( String(), "File::" + String( fp ) + "(): File must be open." )

#define CHECK_WRITABLE( fp )                                                                          \
   CHECK_OPEN_FILE( fp );                                                                             \
   if ( !CanWrite() )                                                                                 \
      throw File::Error( FilePath(), "File::" + String( fp ) + "(): File is open in read-only mode" )

#define CHECK_READABLE( fp )                                                                          \
   CHECK_OPEN_FILE( fp );                                                                             \
   if ( !CanRead() )                                                                                  \
      throw File::Error( FilePath(), "File::" + String( fp ) + "(): File is open in write-only mode" )

#define COPY_BLOCK_SIZE 4096

namespace pcl
{

// ----------------------------------------------------------------------------

static const File::handle invalid_handle =
#ifdef __PCL_WINDOWS
   File::handle( INVALID_HANDLE_VALUE );
#else
   File::handle( (FILE*)0 );
#endif

// ----------------------------------------------------------------------------

#ifdef __PCL_WINDOWS

static String WinErrorMessage()
{
   DWORD errCode = ::GetLastError();

   LPVOID lpMsgBuf;

   ::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     0,
                     errCode,
                     MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
                     (LPWSTR)&lpMsgBuf,
                     0,
                     0 );

   String msg( reinterpret_cast<char16_type*>( lpMsgBuf ) );

   ::LocalFree( lpMsgBuf );

   return String().Format( "Win32 error (%u): ", errCode ) + msg;
}

static inline void WinSplitPos( DWORD& loPos, LONG& hiPos, fpos_type pos )
{
   loPos = DWORD( pos & 0x00000000FFFFFFFFull );
   hiPos = LONG( pos >> 32 );
}

static inline fpos_type WinMergePos( DWORD loPos, LONG hiPos )
{
   return fpos_type( loPos ) | (fpos_type( hiPos ) << 32);
}

#endif

// ----------------------------------------------------------------------------

double FileTime::ToJD() const
{
   return ComplexTimeToJD( year, month, day, (hour + (minute + (second + milliseconds/1000.0)/60.0)/60.0)/24.0 );
}

double FileTime::ToJDUTC() const
{
#if defined( __PCL_WINDOWS )
   double jd = ToJD();
   long tz;
   if ( ::_get_timezone( &tz ) == 0 )
      jd += tz/86400.0;
   return jd;
#elif defined( __PCL_FREEBSD )
   time_t t = time( 0 );
   struct tm r;
   ::localtime_r( &t, &r );
   return ToJD() + r.tm_gmtoff/86400.0;
#else
   return ToJD() + ::timezone/86400.0;
#endif
}

// ----------------------------------------------------------------------------

String File::SizeAsString( fsize_type bytes, int precision, bool alsoShowBytes )
{
   const fsize_type kb = 1024;
   const fsize_type mb = 1024 * kb;
   const fsize_type gb = 1024 * mb;
   const fsize_type tb = 1024 * gb;
   String text;
   if ( bytes >= tb )
      text = String().Format( "%.*f TiB", precision, double( bytes )/tb );
   else if ( bytes >= gb )
      text = String().Format( "%.*f GiB", precision, double( bytes )/gb );
   else if ( bytes >= mb )
      text = String().Format( "%.*f MiB", precision, double( bytes )/mb );
   else if ( bytes >= kb )
      text = String().Format( "%.*f KiB", precision, double( bytes )/kb );
   else
      return String().Format( "%lld B", bytes );
   if ( alsoShowBytes )
      text.AppendFormat( " (%lld bytes)", bytes );
   return text;
}

// ----------------------------------------------------------------------------

fpos_type File::Position() const
{
   CHECK_OPEN_FILE( "Position" );

#ifdef __PCL_WINDOWS

   LONG hiPos = 0;
   DWORD loPos = ::SetFilePointer( m_fileHandle, 0, &hiPos, FILE_CURRENT );

   if ( loPos == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR )
      throw File::Error( FilePath(), "File seek error: " + WinErrorMessage() );

   return WinMergePos( loPos, hiPos );

#else

   errno = 0;
   fpos_type p = ::ftello( (FILE *)m_fileHandle );
   if ( p < 0 )
   {
      if ( errno != 0 )
         throw File::Error( FilePath(), "File seek error: " + String( ::strerror( errno ) ) );
      THROW( SeekError );
   }

   return p;

#endif
}

// ----------------------------------------------------------------------------

void File::SetPosition( fpos_type pos )
{
   CHECK_OPEN_FILE( "SetPosition" );

#ifdef __PCL_WINDOWS

   DWORD loPos;
   LONG hiPos;
   WinSplitPos( loPos, hiPos, pos );

   loPos = ::SetFilePointer( m_fileHandle, loPos, &hiPos, FILE_BEGIN );

   if ( loPos == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR || WinMergePos( loPos, hiPos ) != pos )
      throw File::Error( FilePath(), "File seek error: " + WinErrorMessage() );

#else

   errno = 0;
   if ( ::fseeko( (FILE *)m_fileHandle, pos, SEEK_SET ) == 0 )
   {
      fpos_type p = ::ftello( (FILE *)m_fileHandle );
      if ( p == pos )
         return;
   }

   if ( errno != 0 )
      throw File::Error( FilePath(), "File seek error: " + String( ::strerror( errno ) ) );
   THROW( SeekError );

#endif
}

// ----------------------------------------------------------------------------

fpos_type File::Seek( fpos_type delta, File::seek_mode origin )
{
   CHECK_OPEN_FILE( "Seek" );

#ifdef __PCL_WINDOWS

   DWORD winWhence;
   switch ( origin )
   {
      default:
      case SeekMode::FromBegin   : winWhence = FILE_BEGIN;   break;
      case SeekMode::FromCurrent : winWhence = FILE_CURRENT; break;
      case SeekMode::FromEnd     : winWhence = FILE_END;     break;
   }

   DWORD loDelta;
   LONG hiDelta;
   WinSplitPos( loDelta, hiDelta, delta );

   loDelta = ::SetFilePointer( m_fileHandle, loDelta, &hiDelta, winWhence );

   if ( loDelta == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR )
      throw File::Error( FilePath(), "File seek error: " + WinErrorMessage() );

   return WinMergePos( loDelta, hiDelta );

#else

   int stdWhence;
   switch ( origin )
   {
      default:
      case SeekMode::FromBegin   : stdWhence = SEEK_SET; break;
      case SeekMode::FromCurrent : stdWhence = SEEK_CUR; break;
      case SeekMode::FromEnd     : stdWhence = SEEK_END; break;
   }

   errno = 0;
   if ( ::fseeko( (FILE *)m_fileHandle, delta, stdWhence ) == 0 )
   {
      fpos_type pos = ::ftello( (FILE *)m_fileHandle );
      if ( pos >= 0 )
         return pos;
   }

   if ( errno != 0 )
      throw File::Error( FilePath(), "File seek error: " + String( ::strerror( errno ) ) );
   THROW( SeekError );

#endif
}

// ----------------------------------------------------------------------------

fsize_type File::Size() const
{
   CHECK_OPEN_FILE( "Size" );

#ifdef __PCL_WINDOWS

   DWORD hiSize;
   DWORD loSize = ::GetFileSize( m_fileHandle, &hiSize );

   if ( loSize == INVALID_FILE_SIZE && ::GetLastError() != NO_ERROR )
      throw File::Error( FilePath(), "File seek error: " + WinErrorMessage() );

   return fsize_type( WinMergePos( loSize, hiSize ) );

#else

   IsoString utf8_fn = FilePath().ToUTF8();
   errno = 0;
   struct stat s;
   if ( ::stat( utf8_fn.c_str(), &s ) != 0 )
   {
      if ( errno != 0 )
         throw File::Error( FilePath(), "File access error: " + String( ::strerror( errno ) ) );
      THROW( AccessError );
   }

   return s.st_size;

#endif
}

// ----------------------------------------------------------------------------

void File::Resize( fsize_type sz )
{
   CHECK_WRITABLE( "Resize" );

#ifdef __PCL_WINDOWS
   SetPosition( sz );
   if ( !::SetEndOfFile( m_fileHandle ) )
      throw File::Error( FilePath(), "Unable to set file size " + WinErrorMessage() );
#else
   errno = 0;
   if ( ::ftruncate( fileno( (FILE *)m_fileHandle ), sz ) != 0 )
   {
      if ( errno != 0 )
         throw File::Error( FilePath(), "Unable to set file size: " + String( ::strerror( errno ) ) );
      THROW( UnableToResizeFile );
   }
#endif
}

// ----------------------------------------------------------------------------

static const size_type ioBlockSz = 0xfffffff0u;

void File::Read( void* b, fsize_type sz )
{
   CHECK_READABLE( "Read" );

   if ( sz != 0 )
   {
#ifndef __PCL_WINDOWS
      if ( sizeof( void* ) == 4 )
      {
#endif
         size_type blockCount  = sz / ioBlockSz;
         size_type lastBLockSz = sz % ioBlockSz;

         for ( size_type i = 0; ; ++i )
         {
            size_type thisBlockSz = (i == blockCount) ? lastBLockSz : ioBlockSz;
            if ( thisBlockSz > 0 )
            {
#ifdef __PCL_WINDOWS
               DWORD nr;
               if ( !::ReadFile( m_fileHandle, b, DWORD( thisBlockSz ), &nr, 0 ) )
                  throw File::Error( FilePath(), "File read error: " + WinErrorMessage() );
#else
               errno = 0;
               size_t nr = ::fread( b, size_t( 1 ), size_t( thisBlockSz ), (FILE *)m_fileHandle );
               if ( errno != 0 )
                  throw File::Error( FilePath(), "File read error: " + String( ::strerror( errno ) ) );
#endif
               if ( size_type( nr ) != thisBlockSz )
                  THROW( UnexpectedEndOfFile );
            }

            if ( i == blockCount )
               break;

            b = (void*)(reinterpret_cast<uint8*>( b ) + thisBlockSz);
         }
#ifndef __PCL_WINDOWS
      }
      else
      {
         errno = 0;
         size_t nr = ::fread( b, size_t( 1 ), size_t( sz ), (FILE *)m_fileHandle );
         if ( errno != 0 )
            throw File::Error( FilePath(), "File read error: " + String( ::strerror( errno ) ) );
         if ( fsize_type( nr ) != sz )
            THROW( UnexpectedEndOfFile );
      }
#endif
   }
}

// ----------------------------------------------------------------------------

void File::Write( const void* b, fsize_type sz )
{
   CHECK_WRITABLE( "Write" );

   if ( sz != 0 )
   {
#ifndef __PCL_WINDOWS
      if ( sizeof( void* ) == 4 )
      {
#endif
         size_type blockCount  = sz / ioBlockSz;
         size_type lastBLockSz = sz % ioBlockSz;

         for ( size_type i = 0; ; ++i )
         {
            size_type thisBlockSz = (i == blockCount) ? lastBLockSz : ioBlockSz;
            if ( thisBlockSz > 0 )
            {
#ifdef __PCL_WINDOWS
               DWORD nw;
               if ( !::WriteFile( m_fileHandle, b, DWORD( thisBlockSz ), &nw, 0 ) )
                  throw File::Error( FilePath(), "File write error: " + WinErrorMessage() );
#else
               errno = 0;
               size_t nw = ::fwrite( b, size_t( 1 ), size_t( thisBlockSz ), (FILE *)m_fileHandle );
               if ( errno != 0 )
                  throw File::Error( FilePath(), "File write error: " + String( ::strerror( errno ) ) );
#endif
               if ( size_type( nw ) != thisBlockSz )
                  THROW( IncompleteWriteOperation );
            }

            if ( i == blockCount )
               break;

            b = (const void*)(reinterpret_cast<const uint8*>( b ) + thisBlockSz);
         }
#ifndef __PCL_WINDOWS
      }
      else
      {
         errno = 0;
         size_t nw = ::fwrite( b, size_t( 1 ), size_t( sz ), (FILE *)m_fileHandle );
         if ( errno != 0 )
            throw File::Error( FilePath(), "File write error: " + String( ::strerror( errno ) ) );
         if ( fsize_type( nw ) != sz )
            THROW( IncompleteWriteOperation );
      }
#endif
   }
}

// ----------------------------------------------------------------------------

void File::Read( IsoString& s )
{
   s.Empty();

   uint32 len;
   Read( len );
   if ( len == 0xffffffffu )
   {
      Read( len );
      if ( len != 0 )
      {
         String u;
         u.Reserve( size_type( len ) );
         Read( (void*)u.c_str(), size_type( len )*u.BytesPerChar() );
         u[len] = CharTraits::Null();
         s = u;
      }
   }
   else
   {
      if ( len != 0 )
      {
         s.Reserve( size_type( len ) );
         Read( (void*)s.c_str(), size_type( len ) );
         s[len] = IsoCharTraits::Null();
      }
   }
}

void File::Read( String& s )
{
   s.Empty();

   uint32 len;
   Read( len );
   if ( len == 0xffffffffu )
   {
      Read( len );
      if ( len != 0 )
      {
         s.Reserve( size_type( len ) );
         Read( (void*)s.c_str(), size_type( len )*s.BytesPerChar() );
         s[len] = CharTraits::Null();
      }
   }
   else
   {
      if ( len != 0 )
      {
         IsoString u;
         u.Reserve( size_type( len ) );
         Read( (void*)u.c_str(), size_type( len ) );
         u[len] = IsoCharTraits::Null();
         s = u;
      }
   }
}

void File::Write( const char16_type* s )
{
   size_type len = CharTraits::Length( s );
   if ( len == 0 )
      Write( uint32( 0 ) );
   else
   {
      Write( uint32( 0xffffffffu ) );
      Write( uint32( len ) );
      Write( (const void*)s, len << 1 );
   }
}

void File::Write( const char* s )
{
   size_type len = ::strlen( s );
   if ( len > 0xfffffff0u )
   {
      String t( s );
      Write( t );
   }
   else
   {
      Write( uint32( len ) );
      if ( len != 0 )
         Write( (const void*)s, len );
   }
}

// ----------------------------------------------------------------------------

void File::OutText( const char16_type* s )
{
   if ( s != 0 )
      for ( ; *s != CharTraits::Null(); )
      {
         const char16_type* r = s;
         for ( ;; )
         {
            if ( *r == '\n' )
               break;

            if ( *++r == CharTraits::Null() )
            {
               Write( (const void*)s, (r - s) << 1 );
               return;
            }
         }

         Write( (const void*)s, (r - s) << 1 );
         Write( char16_type( '\n' ) );

         s = r + 1;
      }
}

void File::OutText( const char* s )
{
   if ( s != 0 )
      for ( ; *s != '\0'; )
      {
         const char* r = ::strchr( s, '\n' );

         if ( r == 0 )
         {
            Write( (const void*)s, ::strlen( s ) );
            break;
         }

         Write( (const void*)s, r-s );
         Write( '\n' );

         s = r+1;
      }
}

// ----------------------------------------------------------------------------

void File::Flush()
{
   CHECK_WRITABLE( "Flush" );

#ifdef __PCL_WINDOWS
   if ( !::FlushFileBuffers( m_fileHandle ) )
      throw File::Error( FilePath(), "File flush failed: " + WinErrorMessage() );
#else
   errno = 0;
   if ( ::fflush( (FILE *)m_fileHandle ) != 0 )
   {
      if ( errno != 0 )
         throw File::Error( FilePath(), "File flush failed: " + String( ::strerror( errno ) ) );
      THROW( FlushFailed );
   }
#endif
}

// ----------------------------------------------------------------------------

void File::Open( const String& filePath, FileModes mode )
{
   if ( IsOpen() )
      Close();

   if ( filePath.IsEmpty() )
      THROW( InvalidFileName );

   if ( !mode.IsFlagSet( FileMode::OpenMode ) ) // no open mode specified ?
      mode |= FileMode::Open;
   else
   {
      if ( mode.IsFlagSet( FileMode::Create ) ) // create a new file, or overwrite an existing one
         mode |= FileMode::Write;
   }

   if ( !mode.IsFlagSet( FileMode::AccessMode ) ) // no access mode specified ?
      mode |= FileMode::Read;

#ifdef __PCL_WINDOWS

   DWORD dwDesiredAccess = 0, dwShareMode = 0, dwCreationDistribution;

   if ( mode.IsFlagSet( FileMode::Open ) )
   {
      if ( mode.IsFlagSet( FileMode::Create ) )
         dwCreationDistribution = OPEN_ALWAYS;
      else
         dwCreationDistribution = OPEN_EXISTING;
   }
   else // FileMode::Create
      dwCreationDistribution = CREATE_ALWAYS;

   if ( mode.IsFlagSet( FileMode::Read ) )
      dwDesiredAccess |= GENERIC_READ;

   if ( mode.IsFlagSet( FileMode::Write ) )
      dwDesiredAccess |= GENERIC_WRITE;

   if ( mode.IsFlagSet( FileMode::ShareRead ) )
      dwShareMode |= FILE_SHARE_READ;

   if ( mode.IsFlagSet( FileMode::ShareWrite ) )
      dwShareMode |= FILE_SHARE_WRITE;

   String winPath = UnixPathToWindows( filePath );

   m_fileHandle = ::CreateFileW(
      (LPCWSTR)winPath.c_str(),  // pointer to name of the file
      dwDesiredAccess,           // access (read-write) mode
      dwShareMode,               // share mode
      0,                         // pointer to security attributes
      dwCreationDistribution,    // how to create
      FILE_ATTRIBUTE_NORMAL,     // file attributes and flags
      0                          // handle to file with attributes to copy
   );

#else // !__PCL_WINDOWS

   IsoString howToOpen;

   if ( mode.IsFlagSet( FileMode::Open ) )
   {
      if ( mode.IsFlagSet( FileMode::Create ) )
      {
         howToOpen = 'a';
         if ( mode.IsFlagSet( FileMode::Read ) )
            howToOpen += '+';
      }
      else
      {
         howToOpen = 'r';
         if ( mode.IsFlagSet( FileMode::Write ) )
            howToOpen += '+';
      }
   }
   else // FileMode::Create
   {
      howToOpen = 'w';
      if ( mode.IsFlagSet( FileMode::Read ) )
         howToOpen += '+';
   }

   howToOpen += 'b'; // always in binary mode

   /*
    * ### TODO: Emulate fsopen() on Linux - for now, sharing modes are ignored.
    *
   int howToShare = mode.IsFlagSet( FileMode::ShareRead ) ?
      (mode.IsFlagSet( FileMode::ShareWrite ) ? SH_DENYNO : SH_DENYWR) :
      (mode.IsFlagSet( FileMode::ShareWrite ) ? SH_DENYRD : SH_DENYRW);

   m_fileHandle = (handle)::fsopen( filePath.c_str(), howToOpen.c_str(), howToShare );
   */

   errno = 0;
   IsoString utf8_fn = filePath.ToUTF8();
   m_fileHandle = (handle)::fopen( utf8_fn.c_str(), howToOpen.c_str() );

#endif // __PCL_WINDOWS

   m_filePath = FullPath( filePath );
   m_fileMode = mode;

   if ( !IsValidHandle( m_fileHandle ) )
   {
#ifdef __PCL_WINDOWS
      throw File::Error( FilePath(), (mode.IsFlagSet( FileMode::Create ) ?
                                     "Unable to create file: " : "Unable to open file: ")
                                     + WinErrorMessage() );
#else
      if ( errno != 0 )
         throw File::Error( FilePath(), (mode.IsFlagSet( FileMode::Create ) ?
                                        "Unable to create file: " : "Unable to open file: ")
                                        + String( ::strerror( errno ) ) );
      if ( mode.IsFlagSet( FileMode::Create ) )
         THROW( UnableToCreateFile );
      THROW( UnableToOpenFile );
#endif
   }
}

// ----------------------------------------------------------------------------

void File::Close()
{
   if ( IsOpen() )
   {
#ifdef __PCL_WINDOWS
      if ( !::CloseHandle( m_fileHandle ) )
         throw File::Error( FilePath(), "Unable to close file: " + WinErrorMessage() );
#else
      errno = 0;
      if ( ::fclose( (FILE *)m_fileHandle ) != 0 )
      {
         if ( errno != 0 )
            throw File::Error( FilePath(), "Unable to close file: " + String( ::strerror( errno ) ) );
         THROW( UnableToCloseFile );
      }
#endif
      m_fileHandle = invalid_handle;
      m_fileMode = FileMode::Zero;
   }
}

// ----------------------------------------------------------------------------

void File::Remove( const String& filePath )
{
   if ( filePath.IsEmpty() )
      throw InvalidFileName( filePath );

#ifdef __PCL_WINDOWS
   String wfn( UnixPathToWindows( filePath ) );
   if ( !::DeleteFileW( (LPCWSTR)wfn.c_str() ) )
      throw File::Error( filePath, "Unable to delete file: " + WinErrorMessage() );
#else
   errno = 0;
   IsoString utf8_fn = filePath.ToUTF8();
   if ( ::remove( utf8_fn.c_str() ) != 0 )
   {
      if ( errno != 0 )
         throw File::Error( filePath, "Unable to delete file: " + String( ::strerror( errno ) ) );
      throw UnableToDeleteFile( filePath );
   }
#endif
}

// ----------------------------------------------------------------------------

static void DoCreateDirectory( const String& dirPath )
{
   if ( File::Exists( dirPath ) )
      throw File::Error( dirPath, "Couldn't create directory - a file already exists with the same name" );

#ifdef __PCL_WINDOWS
   String wdp( File::UnixPathToWindows( dirPath ) );
   if ( !::CreateDirectoryW( (LPCWSTR)wdp.c_str(), 0 ) )
      throw File::Error( dirPath, "Unable to create directory: " + WinErrorMessage() );
#else
   errno = 0;
   IsoString utf8_dp = dirPath.ToUTF8();
   if ( ::mkdir( utf8_dp.c_str(), 0775 ) != 0 )
   {
      if ( errno != 0 )
         throw File::Error( dirPath, "Unable to create directory: " + String( ::strerror( errno ) ) );
      throw File::UnableToCreateDirectory( dirPath );
   }
#endif
}

void File::CreateDirectory( const String& dirPath, bool createIntermediateDirectories )
{
   if ( dirPath.IsEmpty() )
      throw InvalidFileName( dirPath );

#ifdef __PCL_WINDOWS
   String dir = WindowsPathToUnix( dirPath );
#else
   String dir = dirPath;
#endif

   if ( createIntermediateDirectories )
   {
      size_type n = dir.Length();
      size_type p0 = 0;
      if ( dir[0] == '/' )
      {
         if ( n == 1 )
            throw File::Error( dir, "Attempt to create the root directory" );
         p0 = 1;
      }
#ifdef __PCL_WINDOWS
      else
      {
         // Deal with Windows drive letters
         if ( n > 1 && dir[1] == ':' )
         {
            if ( n == 2 )
               throw File::Error( dir, "(Windows-specific) Attempt to \"create a drive\"" );

            if ( dir[2] == '/' )
            {
               if ( n == 3 )
                  throw File::Error( dir, "(Windows-specific) Attempt to create a drive's root directory" );
               p0 = 3;
            }
            else
               p0 = 2;
         }
      }
#endif

      do
      {
         size_type p = dir.Find( '/', p0 );
         if ( p == String::notFound )
            break;
         if ( p > 0 )
         {
            String d = dir.Left( p );
            if ( !File::DirectoryExists( d ) )
               DoCreateDirectory( d );
         }

         p0 = p + 1;
      }
      while ( p0 != n );
   }

   DoCreateDirectory( dir );
}

// ----------------------------------------------------------------------------

void File::RemoveDirectory( const String& dirPath )
{
   if ( dirPath.IsEmpty() )
      throw InvalidFileName( dirPath );

#ifdef __PCL_WINDOWS
   String wdp( UnixPathToWindows( dirPath ) );
   if ( !::RemoveDirectoryW( (LPCWSTR)wdp.c_str() ) )
      throw File::Error( dirPath, "Unable to remove directory: " + WinErrorMessage() );
#else
   errno = 0;
   IsoString utf8_dp = dirPath.ToUTF8();
   if ( ::rmdir( utf8_dp.c_str() ) != 0 )
   {
      if ( errno != 0 )
         throw File::Error( dirPath, "Unable to remove directory: " + String( ::strerror( errno ) ) );
      throw UnableToRemoveDirectory( dirPath );
   }
#endif
}

// ----------------------------------------------------------------------------

void File::Move( const String& filePath, const String& newFilePath )
{
   if ( filePath.IsEmpty() || newFilePath.IsEmpty() )
      throw InvalidFileName( filePath );

#ifdef __PCL_WINDOWS
   String wold( UnixPathToWindows( filePath ) );
   String wnew( UnixPathToWindows( newFilePath ) );
   //if ( !::MoveFileW( (LPCWSTR)wold.c_str(), (LPCWSTR)wnew.c_str() ) )
   // ### NB: MoveFileEx() is not available under Win98/Me
   if ( !::MoveFileExW( (LPCWSTR)wold.c_str(), (LPCWSTR)wnew.c_str(),
                        MOVEFILE_COPY_ALLOWED|MOVEFILE_WRITE_THROUGH ) )
      throw File::Error( filePath, "Unable to rename file: " + WinErrorMessage() );
#else
   // copy *not* allowed for std file i/o
   errno = 0;
   IsoString utf8_old = filePath.ToUTF8();
   IsoString utf8_new = newFilePath.ToUTF8();
   if ( ::rename( utf8_old.c_str(), utf8_new.c_str() ) != 0 )
   {
      if ( errno != 0 )
         throw File::Error( filePath, "Unable to rename file: " + String( ::strerror( errno ) ) );
      throw UnableToMoveFile( filePath );
   }
#endif
}

// ----------------------------------------------------------------------------

bool File::Exists( const String& filePath )
{
   if ( filePath.IsEmpty() )
      throw InvalidFileName( filePath );

#ifdef __PCL_WINDOWS

   String wfn( UnixPathToWindows( filePath ) );
   DWORD attr = ::GetFileAttributesW( (LPCWSTR)wfn.c_str() );
   return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;

#else

   errno = 0;
   IsoString utf8_fn = FullPath( filePath ).ToUTF8();
   if ( ::access( utf8_fn.c_str(), F_OK ) == 0 ) // check for existence only
   {
      struct stat s;
      if ( ::stat( utf8_fn.c_str(), &s ) == 0 )
         return (s.st_mode & S_IFMT) == S_IFREG; // regular file
   }
   if ( errno == ENOENT )
      return false;
   if ( errno != 0 )
      throw File::Error( filePath, "File access error: " + String( ::strerror( errno ) ) );
   throw AccessError( filePath );

#endif
}

// ----------------------------------------------------------------------------

bool File::DirectoryExists( const String& dirPath )
{
   if ( dirPath.IsEmpty() )
      throw InvalidFileName( dirPath );

#ifdef __PCL_WINDOWS

   String wdp( UnixPathToWindows( dirPath ) );
   DWORD attr = ::GetFileAttributesW( (LPCWSTR)wdp.c_str() );
   return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;

#else

   errno = 0;
   IsoString utf8_dp = FullPath( dirPath ).ToUTF8();
   if ( ::access( utf8_dp.c_str(), F_OK ) == 0 ) // check for existence only
   {
      struct stat s;
      if ( ::stat( utf8_dp.c_str(), &s ) == 0 )
         return (s.st_mode & S_IFMT) == S_IFDIR; // directory
   }
   if ( errno == ENOENT )
      return false;
   if ( errno != 0 )
      throw File::Error( dirPath, "File access error: " + String( ::strerror( errno ) ) );
   throw AccessError( dirPath );

#endif
}

// ----------------------------------------------------------------------------

bool File::IsReadOnly( const String& filePath )
{
   if ( filePath.IsEmpty() )
      throw InvalidFileName( filePath );

#ifdef __PCL_WINDOWS

   String wfn( UnixPathToWindows( filePath ) );
   DWORD attr = ::GetFileAttributesW( (LPCWSTR)wfn.c_str() );

   if ( attr == INVALID_FILE_ATTRIBUTES )
      throw FileDoesNotExist( filePath );

   return (attr & FILE_ATTRIBUTE_READONLY) != 0;

#else

   errno = 0;
   IsoString utf8_fn = filePath.ToUTF8();
   if ( ::access( utf8_fn.c_str(), 2 ) == 0 ) // check for write permission
      return false;
   //if ( errno == ENOENT )
   //   throw FileDoesNotExist( filePath );
   if ( errno != 0 )
      throw File::Error( filePath, "File access error: " + String( ::strerror( errno ) ) );
   return true;

#endif
}

// ----------------------------------------------------------------------------

void File::SetReadOnly( const String& filePath, bool rdOnly )
{
   if ( filePath.IsEmpty() )
      throw InvalidFileName( filePath );

#ifdef __PCL_WINDOWS

   String wfn( UnixPathToWindows( filePath ) );
   DWORD attr = ::GetFileAttributesW( (LPCWSTR)wfn.c_str() );

   if ( attr == INVALID_FILE_ATTRIBUTES )
      throw FileDoesNotExist( filePath );

   if ( ((attr & FILE_ATTRIBUTE_READONLY) != 0) != rdOnly )
   {
      if ( rdOnly )
         attr |= FILE_ATTRIBUTE_READONLY;
      else
         attr &= ~FILE_ATTRIBUTE_READONLY;
   }

   if ( !::SetFileAttributesW( (LPCWSTR)wfn.c_str(), attr ) )
      throw File::Error( filePath, "Unable to set file attributes: " + WinErrorMessage() );

#else

   errno = 0;
   IsoString utf8_fn = filePath.ToUTF8();
   if( ::chmod( utf8_fn.c_str(), rdOnly ? S_IREAD : S_IREAD|S_IWRITE ) != 0 )
   {
      //if ( errno == ENOENT )
      //   throw FileDoesNotExist( filePath );
      if ( errno != 0 )
         throw File::Error( filePath, "Unable to set file attributes: " + String( ::strerror( errno ) ) );
      throw UnableToSetFileAttributes( filePath );
   }

#endif
}

// ----------------------------------------------------------------------------

#ifdef __PCL_WINDOWS

static String WinLongPathName( const String& path )
{
   DWORD n = ::GetLongPathNameW( (LPCWSTR)path.c_str(), 0, 0 );
   if ( n != 0 )
   {
      String longPath;
      longPath.Reserve( n );
      DWORD n1 = ::GetLongPathNameW( (LPCWSTR)path.c_str(), (LPWSTR)longPath.c_str(), n );
      if ( n1 != 0 && n1 <= n )
         return longPath;
   }
   return path;
}

static void WinAttrToPCL( pcl::FileAttributes& a, DWORD wa )
{
   a.SetFlag( FileAttribute::Regular,    (wa & FILE_ATTRIBUTE_DIRECTORY) == 0 );
   a.SetFlag( FileAttribute::Directory,  (wa & FILE_ATTRIBUTE_DIRECTORY) != 0 );
   a.SetFlag( FileAttribute::Archive,    (wa & FILE_ATTRIBUTE_ARCHIVE) != 0 );
   a.SetFlag( FileAttribute::Compressed, (wa & FILE_ATTRIBUTE_COMPRESSED) != 0 );
   a.SetFlag( FileAttribute::Encrypted,  (wa & FILE_ATTRIBUTE_ENCRYPTED) != 0 );
   a.SetFlag( FileAttribute::Hidden,     (wa & FILE_ATTRIBUTE_HIDDEN) != 0 );
   a.SetFlag( FileAttribute::ReadOnly,   (wa & FILE_ATTRIBUTE_READONLY) != 0 );
   a.SetFlag( FileAttribute::System,     (wa & FILE_ATTRIBUTE_SYSTEM) != 0 );
   a.SetFlag( FileAttribute::Temporary,  (wa & FILE_ATTRIBUTE_TEMPORARY) != 0 );
}

static void WinFileTimeToPCL( pcl::FileTime& t, FILETIME ft )
{
   SYSTEMTIME stUTC, stLocal;
   ::FileTimeToSystemTime( &ft, &stUTC );
   ::SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );
   t.year         = stLocal.wYear;
   t.month        = stLocal.wMonth;
   t.day          = stLocal.wDay;
   t.hour         = stLocal.wHour;
   t.minute       = stLocal.wMinute;
   t.second       = stLocal.wSecond;
   // Milliseconds are unreliable for file times on Windows
   t.milliseconds = 0; // stLocal.wMilliseconds;
}

static void WinFindDataToPCL( pcl::FindFileInfo& info, const WIN32_FIND_DATAW& data )
{
   info.name = File::WindowsPathToUnix( WinLongPathName( data.cFileName ) );

   info.attributes.Clear();
   WinAttrToPCL( info.attributes, data.dwFileAttributes );

   info.size = fsize_type( data.nFileSizeLow ) | (fsize_type( data.nFileSizeHigh ) << 32);

   WinFileTimeToPCL( info.created, data.ftCreationTime );
   WinFileTimeToPCL( info.lastAccessed, data.ftLastAccessTime );
   WinFileTimeToPCL( info.lastModified, data.ftLastWriteTime );
}

#else // !__PCL_WINDOWS

static void POSIXAttrToPCL( pcl::FileAttributes& a, unsigned mode )
{
   unsigned type = mode & S_IFMT;
   a.SetFlag( FileAttribute::Block,        type == S_IFBLK );
   a.SetFlag( FileAttribute::Character,    type == S_IFCHR );
   a.SetFlag( FileAttribute::FIFO,         type == S_IFIFO );
   a.SetFlag( FileAttribute::Regular,      type == S_IFREG );
   a.SetFlag( FileAttribute::Directory,    type == S_IFDIR );
   a.SetFlag( FileAttribute::SymbolicLink, type == S_IFLNK );
   a.SetFlag( FileAttribute::Socket,       type == S_IFSOCK );

   a.SetFlag( FileAttribute::Read,          (mode & S_IRUSR) != 0 );
   a.SetFlag( FileAttribute::Write,         (mode & S_IWUSR) != 0 );
   a.SetFlag( FileAttribute::Execute,       (mode & S_IXUSR) != 0 );
   a.SetFlag( FileAttribute::ReadGroup,     (mode & S_IRGRP) != 0 );
   a.SetFlag( FileAttribute::WriteGroup,    (mode & S_IWGRP) != 0 );
   a.SetFlag( FileAttribute::ExecuteGroup,  (mode & S_IXGRP) != 0 );
   a.SetFlag( FileAttribute::ReadOthers,    (mode & S_IROTH) != 0 );
   a.SetFlag( FileAttribute::WriteOthers,   (mode & S_IWOTH) != 0 );
   a.SetFlag( FileAttribute::ExecuteOthers, (mode & S_IXOTH) != 0 );
}

static void POSIXFileTimeToPCL( pcl::FileTime& t, time_t ft )
{
   ::tm* lt = ::localtime( &ft );
   t.year         = lt->tm_year + 1900;
   t.month        = lt->tm_mon + 1;
   t.day          = lt->tm_mday;
   t.hour         = lt->tm_hour;
   t.minute       = lt->tm_min;
   t.second       = lt->tm_sec;
   t.milliseconds = 0;
}

static void POSIXFindDataToPCL( pcl::FindFileInfo& info, const ::dirent* data, const String& dir )
{
   info.name = String::UTF8ToUTF16( data->d_name );

   IsoString utf8_path;

   if ( !dir.IsEmpty() )
   {
      utf8_path = dir.ToUTF8();
      if ( !dir.EndsWith( '/' ) )
         utf8_path += '/';
   }

   utf8_path += data->d_name;

   struct stat s;
   if ( ::lstat( utf8_path.c_str(), &s ) != 0 )
      throw File::AccessError( utf8_path );

   info.attributes.Clear();

   POSIXAttrToPCL( info.attributes, s.st_mode );

   // emulated flags
   info.attributes.SetFlag( FileAttribute::Hidden,   info.name.BeginsWith( '.' ) );
   info.attributes.SetFlag( FileAttribute::ReadOnly, info.attributes.IsFlagSet( FileAttribute::Write ) );

   info.size          = s.st_size;
   info.numberOfLinks = s.st_nlink;
   info.userId        = s.st_uid;
   info.groupId       = s.st_gid;

   POSIXFileTimeToPCL( info.created,      s.st_ctime );
   POSIXFileTimeToPCL( info.lastAccessed, s.st_atime );
   POSIXFileTimeToPCL( info.lastModified, s.st_mtime );
}

#endif // __PCL_WINDOWS

void File::Find::Begin( const String& path )
{
   End();

   if ( path.IsEmpty() )
      throw File::InvalidSearchPath( path );

   searchPath = path;

#ifdef __PCL_WINDOWS

   String wpath( UnixPathToWindows( path ) );
   WIN32_FIND_DATAW data;
   HANDLE h = ::FindFirstFileW( (LPCWSTR)wpath.c_str(), &data );

   if ( h != INVALID_HANDLE_VALUE )
   {
      WinFindDataToPCL( info, data );
      handle = (void*)h;
   }

#else

   searchDir = File::ExtractDirectory( path );

   searchName = File::ExtractName( path ) + File::ExtractExtension( path );
   if ( searchName.IsEmpty() )
      searchName = "*";

   IsoString utf8_sd = searchDir.ToUTF8();
   DIR* d = ::opendir( searchDir.IsEmpty() ? "." : utf8_sd.c_str() );

   if ( d != 0 )
      for ( ;; )
      {
         errno = 0;
         dirent* data = ::readdir( d );
         if ( data == 0 )
         {
            if ( errno != 0 && errno != ENOENT )
               throw File::Error( searchPath, "Directory search error: " + String( ::strerror( errno ) ) );
            ::closedir( d );
            break;
         }

         if ( MatchesWildSpecification( String::UTF8ToUTF16( data->d_name ), searchName ) )
         {
            POSIXFindDataToPCL( info, data, searchDir );
            handle = (void*)d;
            break;
         }
      }

#endif
}

bool File::Find::NextItem( FindFileInfo& _info )
{
   if ( handle == 0 )
      return false;

   _info = info;

#ifdef __PCL_WINDOWS

   WIN32_FIND_DATAW data;

   if ( ::FindNextFileW( (HANDLE)handle, &data ) )
      WinFindDataToPCL( info, data );
   else
   {
      DWORD error = ::GetLastError();
      End();
      if ( error != ERROR_NO_MORE_FILES )
         throw File::Error( searchPath, "Directory search error: " + WinErrorMessage() );
   }

#else

   for ( ;; )
   {
      errno = 0;
      dirent* data = ::readdir( (DIR*)handle );
      if ( data == 0 )
      {
         End();
         if ( errno != 0 && errno != ENOENT )
            throw File::Error( searchPath, "Directory search error: " + String( ::strerror( errno ) ) );
         break;
      }

      if ( MatchesWildSpecification( String::UTF8ToUTF16( data->d_name ), searchName ) )
      {
         POSIXFindDataToPCL( info, data, searchDir );
         break;
      }
   }

#endif

   return true;
}

void File::Find::End()
{
   if ( handle != 0 )
   {
#ifdef __PCL_WINDOWS
      ::FindClose( (HANDLE)handle );
#else
      ::closedir( (DIR*)handle );
#endif
      handle = 0;
   }
}

// ----------------------------------------------------------------------------

ByteArray File::ReadFile( const String& filePath )
{
   File f;
   f.OpenForReading( filePath );
   ByteArray contents;
   fsize_type size = f.Size();
   if ( size > 0 )
   {
      contents.Add( uint8( 0 ), size_type( size ) );
      f.Read( reinterpret_cast<void*>( contents.Begin() ), size );
   }
   f.Close();
   return contents;
}

// ----------------------------------------------------------------------------

void File::WriteFile( const String& filePath, const ByteArray& contents )
{
   File f;
   f.CreateForWriting( filePath );
   if ( !contents.IsEmpty() )
      f.Write( reinterpret_cast<const void*>( contents.Begin() ), fsize_type( contents.Length() ) );
   f.Flush();
   f.Close();
}

// ----------------------------------------------------------------------------

void File::SetPermissions( const String& filePath, FileAttributes permissions )
{
#ifdef __PCL_WINDOWS

   int mode = 0;

   if ( permissions & FileAttribute::Read ||
        permissions & FileAttribute::ReadGroup ||
        permissions & FileAttribute::ReadOthers )
      mode |= _S_IREAD;

   if ( !(permissions & FileAttribute::ReadOnly) &&
         (permissions & FileAttribute::Write ||
          permissions & FileAttribute::WriteGroup ||
          permissions & FileAttribute::WriteOthers) )
      mode |= _S_IWRITE;

   if ( mode != 0 ) // silently ignore unsupported file permissions on Windows
      if ( ::_wchmod( (const wchar_t*)filePath.c_str(), mode ) != 0 )
         throw File::Error( filePath, "Unable to set file permissions: " + String( ::_wcserror( errno ) ) );
#else
   mode_t mode = 0;
   if ( permissions & FileAttribute::Read )
      mode |= S_IRUSR;
   if ( permissions & FileAttribute::Write )
      mode |= S_IWUSR;
   if ( permissions & FileAttribute::Execute )
      mode |= S_IXUSR;
   if ( permissions & FileAttribute::ReadGroup )
      mode |= S_IRGRP;
   if ( permissions & FileAttribute::WriteGroup )
      mode |= S_IWGRP;
   if ( permissions & FileAttribute::ExecuteGroup )
      mode |= S_IXGRP;
   if ( permissions & FileAttribute::ReadOthers )
      mode |= S_IROTH;
   if ( permissions & FileAttribute::WriteOthers )
      mode |= S_IWOTH;
   if ( permissions & FileAttribute::ExecuteOthers )
      mode |= S_IXOTH;

   IsoString utf8 = filePath.ToUTF8();
   if ( ::chmod( utf8.c_str(), mode ) != 0 )
      throw File::Error( filePath, "Unable to set file permissions: " + String( ::strerror( errno ) ) );
#endif
}

// ----------------------------------------------------------------------------

void File::CopyTimesAndPermissions( const String& targetPath, const String& sourcePath )
{
   FileInfo sourceInfo( sourcePath );
#ifdef __PCL_WINDOWS
   __utimbuf64 times;
   times.actime = TruncI64( sourceInfo.LastAccessed().SecondsSinceEpoch() );
   times.modtime = TruncI64( sourceInfo.LastModified().SecondsSinceEpoch() );
   if ( _wutime64( (const wchar_t*)targetPath.c_str(), &times ) != 0 )
      throw File::Error( targetPath, "Unable to set file times: " + String( ::_wcserror( errno ) ) );
#else
   utimbuf times;
   if ( sizeof( time_t ) == 8 )
   {
      times.actime = TruncI64( sourceInfo.LastAccessed().SecondsSinceEpoch() );
      times.modtime = TruncI64( sourceInfo.LastModified().SecondsSinceEpoch() );
   }
   else
   {
      times.actime = TruncI( sourceInfo.LastAccessed().SecondsSinceEpoch() );
      times.modtime = TruncI( sourceInfo.LastModified().SecondsSinceEpoch() );
   }

   IsoString utf8 = targetPath.ToUTF8();
   if ( ::utime( utf8.c_str(), &times ) != 0 )
      throw File::Error( targetPath, "Unable to set file times: " + String( ::strerror( errno ) ) );
#endif

   SetPermissions( targetPath, sourceInfo.Attributes() );
}

// ----------------------------------------------------------------------------

#ifndef __PCL_WINDOWS

void File::CopyLink( const String& targetLinkPath, const String& sourceLinkPath )
{
   IsoString sourceLinkPath8 = sourceLinkPath.ToUTF8();
   IsoString targetLinkPath8 = targetLinkPath.ToUTF8();
   char buf[ 4096 ];
   int len = ::readlink( sourceLinkPath8.c_str(), buf, sizeof( buf )-1 );
   if ( len < 0 )
      throw File::Error( sourceLinkPath, "Unable to read symbolic link contents: " + String( ::strerror( errno ) ) );
   buf[len] = '\0';
   if ( ::symlink( buf, targetLinkPath8.c_str() ) < 0 )
      throw File::Error( targetLinkPath, "Unable to create symbolic link: " + String( ::strerror( errno ) ) );
}

#endif

// ----------------------------------------------------------------------------

void File::CopyFile( const String& targetFilePath, const String& sourceFilePath, File::Progress* progress )
{
   if ( FileInfo( targetFilePath ).Exists() )
      throw File::Error( targetFilePath, "The target file already exists" );

   FileInfo sourceInfo( sourceFilePath );
   if ( !sourceInfo.Exists() )
      throw File::Error( sourceFilePath, "The source file does not exist" );
#ifndef __PCL_WINDOWS
   if ( sourceInfo.IsSymbolicLink() )
   {
      File::CopyLink( targetFilePath, sourceFilePath );
      return;
   }
#endif
   if ( sourceInfo.IsDirectory() )
      throw File::Error( sourceFilePath, "Cannot copy a directory with File::CopyFile()" );
   if ( !sourceInfo.IsFile() )
      throw File::Error( sourceFilePath, "Cannot copy a sequential file using block copy" );

   File sourceFile;
   sourceFile.OpenForReading( sourceFilePath );

   String tempFilePath = targetFilePath + ".pi-writing";
   if ( FileInfo( tempFilePath ).Exists() )
      for ( int n = 1; ; ++n )
      {
         String tryPath = tempFilePath + String().Format( "-%04d", n );
         if ( !FileInfo( tryPath ).Exists() )
         {
            tempFilePath = tryPath;
            break;
         }
      }

   File targetFile;

   try
   {
      targetFile.CreateForWriting( tempFilePath );

      fsize_type size = sourceFile.Size();
      if ( progress )
         progress->SetText( targetFilePath + " (" + File::SizeAsString( size ) + ")" );

      if ( size > 0 )
      {
         uint8 block[ COPY_BLOCK_SIZE ];
         fsize_type numberOfBlocks = size/COPY_BLOCK_SIZE;
         fsize_type remainderBytes = size%COPY_BLOCK_SIZE;

         for ( fsize_type i = 0; i < numberOfBlocks; ++i )
         {
            sourceFile.Read( block, COPY_BLOCK_SIZE );
            targetFile.Write( block, COPY_BLOCK_SIZE );
            if ( progress )
               progress->Add( COPY_BLOCK_SIZE );
         }

         if ( remainderBytes > 0 )
         {
            sourceFile.Read( block, remainderBytes );
            targetFile.Write( block, remainderBytes );
            if ( progress )
               progress->Add( remainderBytes );
         }
      }

      sourceFile.Close();

      targetFile.Flush();
      targetFile.Close();

      File::Move( tempFilePath, targetFilePath );
      File::CopyTimesAndPermissions( targetFilePath, sourceFilePath );
   }
   catch ( Exception& )
   {
      try
      {
         sourceFile.Close();
         targetFile.Close();
         if ( File::Exists( tempFilePath ) )
            File::Remove( tempFilePath );
      }
      catch ( ... )
      {
         // Want no additional exceptions here
      }
      throw;
   }
}

// ----------------------------------------------------------------------------

void File::MoveFile( const String& targetFilePath, const String& sourceFilePath, File::Progress* progress )
{
   if ( FileInfo( targetFilePath ).Exists() )
      throw File::Error( targetFilePath, "The target file already exists" );

   FileInfo sourceInfo( sourceFilePath );
   if ( !sourceInfo.Exists() )
      throw File::Error( sourceFilePath, "The source file does not exist" );

   /*
    * Check if source and target are in the same device.
    */
   bool sameDevice = File::SameDevice( sourceFilePath,
                              File::ExtractDrive( targetFilePath ) + File::ExtractDirectory( targetFilePath ) );

   /*
    * Move within the same device: rename source -> target.
    */
   if ( sameDevice )
   {
      if ( progress )
         progress->SetText( targetFilePath );
      File::Move( sourceFilePath, targetFilePath );
      if ( progress )
         progress->Add( FileInfo( targetFilePath ).Size() );
      return;
   }

   /*
    * Move across devices: copy source -> target, delete source.
    */
#ifndef __PCL_WINDOWS
   if ( sourceInfo.IsSymbolicLink() )
   {
      File::CopyLink( targetFilePath, sourceFilePath );
      File::Remove( sourceFilePath );
      return;
   }
#endif
   if ( sourceInfo.IsDirectory() )
      throw File::Error( sourceFilePath, "Cannot move a directory across devices with File::MoveFile()" );
   if ( !sourceInfo.IsFile() )
      throw File::Error( sourceFilePath, "Cannot move a sequential file using block copy" );

   File sourceFile;
   sourceFile.OpenForReading( sourceFilePath );

   String tempFilePath = targetFilePath + ".pi-writing";
   if ( FileInfo( tempFilePath ).Exists() )
      for ( int n = 1; ; ++n )
      {
         String tryPath = tempFilePath + String().Format( "-%04d", n );
         if ( !FileInfo( tryPath ).Exists() )
         {
            tempFilePath = tryPath;
            break;
         }
      }

   File targetFile;

   try
   {
      targetFile.CreateForWriting( tempFilePath );

      fsize_type size = sourceFile.Size();
      if ( progress )
         progress->SetText( targetFilePath + " (" + File::SizeAsString( size ) + ")" );

      if ( size > 0 )
      {
         uint8 block[ COPY_BLOCK_SIZE ];
         fsize_type numberOfBlocks = size/COPY_BLOCK_SIZE;
         fsize_type remainderBytes = size%COPY_BLOCK_SIZE;

         for ( fsize_type i = 0; i < numberOfBlocks; ++i )
         {
            sourceFile.Read( block, COPY_BLOCK_SIZE );
            targetFile.Write( block, COPY_BLOCK_SIZE );
            if ( progress )
               progress->Add( COPY_BLOCK_SIZE );
         }

         if ( remainderBytes > 0 )
         {
            sourceFile.Read( block, remainderBytes );
            targetFile.Write( block, remainderBytes );
            if ( progress )
               progress->Add( remainderBytes );
         }
      }

      sourceFile.Close();

      targetFile.Flush();
      targetFile.Close();

      File::Move( tempFilePath, targetFilePath );
      File::CopyTimesAndPermissions( targetFilePath, sourceFilePath );
      File::Remove( sourceFilePath );
   }
   catch ( Exception& )
   {
      try
      {
         sourceFile.Close();
         targetFile.Close();
         if ( File::Exists( tempFilePath ) )
            File::Remove( tempFilePath );
      }
      catch ( ... )
      {
         // Want no additional exceptions here
      }
      throw;
   }
}

// ----------------------------------------------------------------------------

bool File::SameDevice( const String& path1, const String& path2 )
{
#ifdef __PCL_WINDOWS
   struct __stat64 s1, s2;
   if ( _wstat64( (const wchar_t*)path1.c_str(), &s1 ) != 0 )
      throw File::Error( path1, "Cannot stat file" );
   if ( _wstat64( (const wchar_t*)path2.c_str(), &s2 ) != 0 )
      throw File::Error( path2, "Cannot stat file" );
#else
   struct stat s1, s2;
   IsoString u81 = path1.ToUTF8();
   IsoString u82 = path2.ToUTF8();
   if ( lstat( u81.c_str(), &s1 ) != 0 )
      throw File::Error( path1, "Cannot stat file" );
   if ( lstat( u82.c_str(), &s2 ) != 0 )
      throw File::Error( path2, "Cannot stat file" );
#endif
   return s1.st_dev == s2.st_dev;
}

// ----------------------------------------------------------------------------

bool File::SameFile( const String& path1, const String& path2 )
{
#ifdef __PCL_WINDOWS
   struct __stat64 s1, s2;
   if ( _wstat64( (const wchar_t*)path1.c_str(), &s1 ) != 0 )
      throw File::Error( path1, "Cannot stat file" );
   if ( _wstat64( (const wchar_t*)path2.c_str(), &s2 ) != 0 )
      throw File::Error( path2, "Cannot stat file" );
#else
   struct stat s1, s2;
   IsoString u81 = path1.ToUTF8();
   IsoString u82 = path2.ToUTF8();
   if ( lstat( u81.c_str(), &s1 ) != 0 )
      throw File::Error( path1, "Cannot stat file" );
   if ( lstat( u82.c_str(), &s2 ) != 0 )
      throw File::Error( path2, "Cannot stat file" );
#endif
   if ( s1.st_dev != s2.st_dev )
      return false;
#ifdef __PCL_WINDOWS
   return FullPath( path1 ).CompareIC( FullPath( path2 ), true/*localeAware*/ ) == 0;
#else
   return s1.st_ino == s2.st_ino;
#endif
}

// ----------------------------------------------------------------------------

static void AddLineHelper( IsoStringList& lines, const char* p, const ReadTextOptions& options )
{
   IsoString line( p );

   if ( options.IsFlagSet( ReadTextOption::TrimTrailingSpaces ) )
   {
      if ( options.IsFlagSet( ReadTextOption::TrimLeadingSpaces ) )
         line.Trim();
      else
         line.TrimRight();
   }
   else if ( options.IsFlagSet( ReadTextOption::TrimLeadingSpaces ) )
      line.TrimLeft();

   if ( !line.IsEmpty() || !options.IsFlagSet( ReadTextOption::RemoveEmptyLines ) )
      lines.Add( line );
}

IsoStringList File::ReadLines( const String& filePath, ReadTextOptions options )
{
   File fin;
   fin.OpenForReading( filePath );

   fsize_type fileSize = fin.Size();
   if ( fileSize <= 0 )
   {
      fin.Close();
      return IsoStringList();
   }

   Array<char> buffer( fileSize + 16 );

   fin.Read( reinterpret_cast<void*>( buffer.Begin() ), fileSize );
   fin.Close();

   char* p = buffer.Begin();
   char* p1 = buffer.At( fileSize );
   *p1 = '\0';

   IsoStringList lines;

   for ( ;; )
   {
      char* npos = ::strchr( p, '\n' );
      char* rpos = ::strchr( p, '\r' );
      char* eol = (npos == 0) ? rpos : ((rpos == 0) ? npos : Min( npos, rpos ));
      if ( eol == 0 )
         break;
      *eol = '\0';
      AddLineHelper( lines, p, options );
      p = eol + 1;
      if ( p == p1 || *eol == '\r' && *p == '\n' && ++p == p1 )
         break;
   }

   if ( p != p1 )
      AddLineHelper( lines, p, options );

   return lines;
}

// ----------------------------------------------------------------------------

static int ScanLineHelper( char*& p, char*& p1, bool (*callback)( char*, void* ), void* data, const ReadTextOptions& options )
{
   if ( options.IsFlagSet( ReadTextOption::TrimTrailingSpaces ) )
      for ( ; p1 > p && IsoCharTraits::IsTrimable( p1[-1] ); --p1 ) {}
   if ( options.IsFlagSet( ReadTextOption::TrimLeadingSpaces ) )
      for ( ; p < p1 && IsoCharTraits::IsTrimable( *p ); ++p ) {}
   if ( p == p1 && options.IsFlagSet( ReadTextOption::RemoveEmptyLines ) )
      return 0;
   *p1 = '\0';
   return (*callback)( p, data ) ? 1 : -1;
}

size_type File::ScanLines( const String& filePath, bool (*callback)( char*, void* ), void* data, ReadTextOptions options )
{
   File fin;
   fin.OpenForReading( filePath );

   fsize_type fileSize = fin.Size();
   if ( fileSize <= 0 )
   {
      fin.Close();
      return 0;
   }

   Array<char> buffer( fileSize + 16 );

   fin.Read( reinterpret_cast<void*>( buffer.Begin() ), fileSize );
   fin.Close();

   char* p = buffer.Begin();
   char* p1 = buffer.At( fileSize );
   *p1 = '\0';

   size_type n = 0;

   for ( ;; )
   {
      char* npos = ::strchr( p, '\n' );
      char* rpos = ::strchr( p, '\r' );
      char* eol = (npos == 0) ? rpos : ((rpos == 0) ? npos : Min( npos, rpos ));
      if ( eol == 0 )
         break;
      *eol = '\0';
      int r = ScanLineHelper( p, eol, callback, data, options );
      if ( r < 0 )
         return n;
      n += r;
      p = eol + 1;
      if ( p == p1 || *eol == '\r' && *p == '\n' && ++p == p1 )
         break;
   }

   if ( p != p1 )
   {
      int r = ScanLineHelper( p, p1, callback, data, options );
      if ( r < 0 )
         return n;
      n += r;
   }

   return n;
}

// ----------------------------------------------------------------------------

static String CleanPath( const String& path )
{
   if ( path.IsEmpty() )
      return path;

   String name = File::WindowsPathToUnix( path );

   int used = 0, levels = 0;
   const int len = int( name.Length() );

   String out( name );
   out.SetUnique();

   for ( int i = 0, last = -1, iwrite = 0; i < len; ++i )
   {
      if ( name[i] == '/' )
      {
         while ( i < len-1 && name[i+1] == '/' )
         {
#ifdef __PCL_WINDOWS // allow unc paths
            if ( i == 0 )
               break;
#endif
               ++i;
         }

         bool eaten = false;

         if ( i < len-1 && name[i+1] == '.' )
         {
            int dotcount = 1;

            if ( i < len-2 && name[i+2] == '.' )
               ++dotcount;

            if ( i == len-dotcount-1 )
            {
               if ( dotcount == 1 )
                  break;

               if ( levels != 0 )
               {
                  if ( last == -1 )
                  {
                     for ( int i2 = iwrite-1; i2 >= 0; --i2 )
                     {
                        if ( out[i2] == '/' )
                        {
                           last = i2;
                           break;
                        }
                     }
                  }

                  used -= iwrite - last - 1;
                  break;
               }
            }
            else if ( name[i+dotcount+1] == '/' )
            {
               if ( dotcount == 2 && levels != 0 )
               {
                  if ( last == -1 || iwrite - last == 1 )
                  {
                     for ( int i2 = (last == -1) ? iwrite-1 : last-1; i2 >= 0; --i2 )
                     {
                        if ( out[i2] == '/' )
                        {
                           eaten = true;
                           last = i2;
                           break;
                        }
                     }
                  }
                  else
                     eaten = true;

                  if ( eaten )
                  {
                     levels--;
                     used -= iwrite - last;
                     iwrite = last;
                     last = -1;
                  }
               }
               else if ( dotcount == 1 )
                  eaten = true;

               if ( eaten )
                  i += dotcount;
            }
            else
               ++levels;
         }
         else if ( last != -1 && iwrite - last == 1 )
         {
#ifdef __PCL_WINDOWS
            eaten = iwrite > 2;
#else
            eaten = true;
#endif
            last = -1;
         }
         else if ( last != -1 && i == len-1 )
            eaten = true;
         else
            levels++;

         if ( !eaten )
            last = i - (i - iwrite);
         else
            continue;
      }
      else if ( i == 0 && name[i] == '.' )
      {
         int dotcount = 1;
         if ( len >= 1 && name[1] == '.' )
            ++dotcount;
         if ( len >= dotcount && name[dotcount] == '/' )
         {
            if ( dotcount == 1 )
            {
               ++i;
               while ( i+1 < len-1 && name[i+1] == '/' )
                  ++i;
               continue;
            }
         }
      }

      out[iwrite++] = name[i];
      ++used;
   }

   String ret;
   if ( used == len )
      ret = name;
   else
      ret = out.Left( used );

   // Strip away last slash except for root directories
   if ( ret.Length() > 1 && ret.EndsWith( '/' )
#ifdef __PCL_WINDOWS
        && !(ret.Length() == 3 && ret[1] == ':')
#endif
      )
      ret.Delete( ret.UpperBound() );

   return ret;
}

String File::FullPath( const String& filePath )
{
#ifdef __PCL_WINDOWS

   String winPath = UnixPathToWindows( filePath );

   DWORD n = ::GetFullPathNameW( (LPCWSTR)winPath.c_str(), 0, 0, 0 );

   if ( n == 0 )
      return filePath;

   String fullPath;
   fullPath.Reserve( n += 8 );
   LPWSTR dum;
   DWORD n1 = ::GetFullPathNameW( (LPCWSTR)winPath.c_str(), n, (LPWSTR)fullPath.c_str(), &dum );

   return (n1 != 0 && n1 <= n) ? WindowsPathToUnix( WinLongPathName( fullPath ) ) : filePath;

#else

   if ( filePath.BeginsWith( '/' ) ) // absolute path
      return CleanPath( filePath );

   IsoString curDir;
   curDir.Reserve( PATH_MAX );
   if( ::getcwd( curDir.c_str(), PATH_MAX ) == 0 || curDir.IsEmpty() )  // ???!!! this can't happen
      throw pcl::Error( "Unable to retrieve the current directory" );

   if ( !curDir.EndsWith( '/' ) )
      curDir += '/';

   return CleanPath( curDir.UTF8ToUTF16() + filePath );

#endif
}

// ----------------------------------------------------------------------------

String File::SystemTempDirectory()
{
   String tempDir;

#ifdef __PCL_WINDOWS

   DWORD n = ::GetTempPathW( 0, 0 );
   if ( n != 0 )
   {
      String temp;
      temp.Reserve( n );
      DWORD n1 = ::GetTempPathW( n, (LPWSTR)temp.c_str() );
      if ( n1 != 0 && n1 <= n )
         tempDir = WindowsPathToUnix( WinLongPathName( temp ) );
   }

#endif

#ifdef __PCL_WINDOWS
   if ( tempDir.IsEmpty() )
   {
#endif
      tempDir = ::getenv( "TEMP" );
      if ( tempDir.IsEmpty() )
      {
         tempDir = ::getenv( "TMP" );
         if ( tempDir.IsEmpty() )
         {
#ifdef __PCL_WINDOWS
            tempDir = ".";
#else
            tempDir = ::getenv( "temp" );
            if ( tempDir.IsEmpty() )
            {
               tempDir = ::getenv( "tmp" );
               if ( tempDir.IsEmpty() )
               {
                  tempDir = "/tmp";
                  if ( !DirectoryExists( tempDir ) )
                  {
                     tempDir = ::getenv( "HOME" );
                     if ( tempDir.IsEmpty() )
                     {
                        tempDir = ::getenv( "home" );

                        if ( tempDir.IsEmpty() )
                           return FullPath( "./" );
                     }

                     String homeTempDir = FullPath( tempDir + "/tmp" );
                     if ( DirectoryExists( homeTempDir ) )
                        tempDir = homeTempDir;
                  }
               }
            }
#endif
         }
      }
#ifdef __PCL_WINDOWS
   }
#endif

   // Strip away last slash except for root directories
   if ( tempDir.Length() > 1 && tempDir.EndsWith( '/' )
#ifdef __PCL_WINDOWS
        && !(tempDir.Length() == 3 && tempDir[1] == ':')
#endif
      )
      tempDir.DeleteRight( tempDir.Length()-1 );

   return tempDir;
}

// ----------------------------------------------------------------------------

String File::WindowsPathToUnix( const String& path )
{
   String unixPath( path );

   for ( String::iterator i = unixPath.Begin(); i != unixPath.End(); ++i )
      if ( *i == '\\' )
      {
         size_type n = pcl::Distance( unixPath.Begin(), i );
         unixPath.SetUnique();
         i = unixPath.Begin() + n;
         for ( *i++ = '/'; i != unixPath.End(); ++i )
            if ( *i == '\\' )
               *i = '/';
         break;
      }

   return unixPath;
}

String File::UnixPathToWindows( const String& path )
{
   String unixPath( path );

   for ( String::iterator i = unixPath.Begin(); i != unixPath.End(); ++i )
      if ( *i == '/' )
      {
         size_type n = pcl::Distance( unixPath.Begin(), i );
         unixPath.SetUnique();
         i = unixPath.Begin() + n;
         for ( *i++ = '\\'; i != unixPath.End(); ++i )
            if ( *i == '/' )
               *i = '\\';
         break;
      }

   return unixPath;
}

// ----------------------------------------------------------------------------

String File::UniqueFileName( const String& directory, int n, const String& prefix, const String& postfix )
{
   // Use only uppercase characters to avoid case ambiguity in file names (Windows).
   static const char* C = "A9B0CD8E1F7GH2I6J3K5L4MN4O5P3QR6S2T7U1V8WX0Y9Z";

   String filePath;
   RandomNumberGenerator R( ::strlen( C ) - 1 );

   if ( n < 5 )
      n = 5;

   String baseDir = directory;
   if ( baseDir.IsEmpty() )
      baseDir = "./";
   else if ( !baseDir.EndsWith( '/' ) )
      baseDir += '/';

   do
   {
      filePath = baseDir;
      filePath += prefix;
      for ( int i = 0; i < n; ++i )
         filePath += C[RoundI( R() )];
      filePath += postfix;
   }
   while ( FileInfo( filePath ).Exists() );

   return File::FullPath( filePath );
}

// ----------------------------------------------------------------------------

uint64 File::GetAvailableSpace( const String& dirPath, uint64* argTotalBytes, uint64* argFreeBytes )
{
#ifdef __PCL_WINDOWS

   String dir = UnixPathToWindows( dirPath );
   dir.Trim();
   if ( dir[0] == '\\' && dir[1] == '\\' )
   {
      if ( !dir.EndsWith( '\\' ) )
         dir.Append( '\\' );  // UNC paths must include a trailing backslash
   }
   else
   {
      if ( dir.EndsWith( '\\' ) )
         if ( dir.Length() > 3 || dir[1] != ':' )
            dir.DeleteRight( dir.Length()-1 );
   }

   ULARGE_INTEGER availableBytes, totalBytes, freeBytes;

   if ( ::GetDiskFreeSpaceExW( (LPCWSTR)dir.c_str(), &availableBytes, &totalBytes, &freeBytes ) )
   {
      if ( argTotalBytes != 0 )
         *argTotalBytes = totalBytes.QuadPart;
      if ( argFreeBytes != 0 )
         *argFreeBytes = freeBytes.QuadPart;

      return availableBytes.QuadPart;
   }

   throw File::Error( dirPath, "Unable to get file system information: " + WinErrorMessage() );

#else

   String dir = dirPath;
   dir.Trim();
   if ( dir.Length() > 1 && dir.EndsWith( '/' ) )
      dir.DeleteRight( dir.Length()-1 );

   IsoString utf8_dir = File::FullPath( dir ).ToUTF8();

   errno = 0;
   struct statvfs buf;

   if ( ::statvfs( utf8_dir.c_str(), &buf ) == 0 )
   {
      // f_frsize is not guaranteed to be supported.
      unsigned long blockSize = buf.f_frsize ? buf.f_frsize : buf.f_bsize;

      if ( argTotalBytes != 0 )
         *argTotalBytes = uint64( buf.f_blocks ) * blockSize;
      if ( argFreeBytes != 0 )
         *argFreeBytes = uint64( buf.f_bfree ) * blockSize;

      return uint64( buf.f_bavail ) * blockSize;
   }

   if ( errno != 0 )
      throw File::Error( dirPath, "Unable to get file system information: " + String( ::strerror( errno ) ) );

   if ( argTotalBytes != 0 )
      *argTotalBytes = 0;
   if ( argFreeBytes != 0 )
      *argFreeBytes = 0;

   return 0;

#endif
}

// ----------------------------------------------------------------------------

size_type File::FindDrive( const String& path )
{
#ifdef __PCL_WINDOWS
   if ( path.Length() > 1 && path[1] == ':' )
      return 0;
#endif
   return String::notFound;
}

// ----------------------------------------------------------------------------

static
size_type FindLastDirectorySeparator( const String& path )
{
#ifdef __PCL_WINDOWS
   size_type p = path.FindLast( '/' );
   size_type q = path.FindLast( '\\' );
   if ( q == String::notFound )
      return p;
   if ( p == String::notFound )
      return q;
   return pcl::Max( p, q );
#else
   return path.FindLast( '/' );
#endif
}

size_type File::FindName( const String& path )
{
   size_type p = FindLastDirectorySeparator( path );
   if ( p == String::notFound )
#ifdef __PCL_WINDOWS
   {
      if ( path.Length() > 1 && path[1] == ':' )
         p = 2;
      else
         p = 0;
   }
#else
      p = 0;
#endif
   else
      ++p;

   return (p == path.Length()) ? String::notFound : p;
}

// ----------------------------------------------------------------------------

size_type File::FindExtension( const String& path )
{
   size_type d = path.FindLast( '.' );
   if ( d != String::notFound )
   {
      size_type p = FindLastDirectorySeparator( path );
      if ( p != String::notFound )
         if ( d < p )
            return String::notFound;
   }
   return d;
}

// ----------------------------------------------------------------------------

size_type File::FindCompleteSuffix( const String& path )
{
   size_type p = FindLastDirectorySeparator( path );
   return path.FindFirst( '.', (p == String::notFound) ? 0 : p+1 );
}

// ----------------------------------------------------------------------------

String File::ExtractDrive( const String& path )
{
#ifdef __PCL_WINDOWS
   if ( path.Length() > 1 && path[1] == ':' )
      return path.Left( 2 );
#endif
   return String();
}

// ----------------------------------------------------------------------------

String File::ExtractDirectory( const String& path )
{
   size_type p1 = FindLastDirectorySeparator( path );
   if ( p1 == String::notFound )
      return String();

#ifdef __PCL_WINDOWS
   size_type p0 = 0;
   if ( path.Length() > 1 && path[1] == ':' )
      p0 = 2;
   return path.SubString( p0, (p1 == p0) ? 1 : p1-p0 );
#else
   return path.SubString( 0, (p1 == 0) ? 1 : p1 );
#endif
}

// ----------------------------------------------------------------------------

String File::ExtractName( const String& path )
{
   size_type p = FindName( path );
   if ( p == String::notFound )
      return String();
   return path.SubString( p, FindExtension( path )-p );
}

// ----------------------------------------------------------------------------

String File::ExtractExtension( const String& path )
{
   return path.SubString( FindExtension( path ) );
}

// ----------------------------------------------------------------------------

String File::ExtractCompleteSuffix( const String& path )
{
   return path.SubString( FindCompleteSuffix( path ) );
}

// ----------------------------------------------------------------------------

String File::ExtractNameAndExtension( const String& path )
{
   return path.SubString( FindName( path ) );
}

// ----------------------------------------------------------------------------

String File::ChangeExtension( const String& path, const String& ext )
{
   return path.Left( FindExtension( path ) ) + ext;
}

// ----------------------------------------------------------------------------

String File::AppendToName( const String& path, const String& postfix )
{
   size_type p = FindExtension( path );
   return path.Left( p ) + postfix + path.SubString( p );
}

String File::PrependToName( const String& path, const String& prefix )
{
   size_type p = FindName( path );
   return path.Left( p ) + prefix + path.SubString( p );
}

// ----------------------------------------------------------------------------

void File::Initialize()
{
   m_fileHandle = invalid_handle;
   m_filePath.Empty();
   m_fileMode = FileMode::Zero;
}

bool File::IsValidHandle( handle h ) const
{
   return h != invalid_handle;
}

// ----------------------------------------------------------------------------

}  // pcl

// ****************************************************************************
// EOF pcl/File.cpp - Released 2014/10/29 07:34:21 UTC
