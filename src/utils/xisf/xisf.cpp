//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
//
// This file is part of the XISF command-line utility program.
//
// Copyright (c) 2017 Pleiades Astrophoto S.L.
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

#include <pcl/Arguments.h>
#include <pcl/ErrorHandler.h>
#include <pcl/File.h>
#include <pcl/TimePoint.h>
#include <pcl/Version.h>
#include <pcl/XISF.h>

#include <iostream>

using namespace pcl;

// ----------------------------------------------------------------------------

static void ShowLogo()
{
   std::cout
   << "\n-------------------------------------------------------------------------------"
      "\nPixInsight XISF Utility - " << Version::AsString()
   << "\nCopyright (c) 2014-" << TimePoint::Now().Year() << " Pleiades Astrophoto. All Rights Reserved."
      "\n-------------------------------------------------------------------------------"
      "\n";
}

// ----------------------------------------------------------------------------

static void ShowUsage()
{
   std::cout
   << "\nUsage: xisf <arg_list> <file_list>"
      "\n"
      "\n<file_list>       Is a sequence <file> [<file_list>]"
      "\n<file>            Is a path to an existing monolithic XISF file."
      "\n"
      "\n<arg_list>        Is a sequence <arg> [<arg_list>]"
      "\n<arg>             Is a valid argument:"
      "\n"
      "\n-h | --header"
      "\n"
      "\n         Extract XISF headers and write them to XML files."
      "\n         Output suffix = '-header.xml'"
      "\n"
      "\n-ei | --enumerate-images"
      "\n"
      "\n         Enumerate all images stored in the XISF unit."
      "\n         Output suffix = '-images.txt'"
      "\n"
      "\n-ep | --enumerate-properties"
      "\n"
      "\n         Enumerate all XISF properties, including global properties and"
      "\n         image properties for all of the images stored in the XISF unit."
      "\n         Output suffix = '-properties.txt'"
      "\n"
      "\n-rP=<property_id> | --read-property=<property_id>"
      "\n"
      "\n         Read a property with identifier <property_id> from the XISF unit"
      "\n         and generate a Unicode text representation."
      "\n         Output suffix = '-property-<property_id>.txt'"
      "\n"
      "\n-rp=<property_id> | --read-image-property=<property_id>"
      "\n"
      "\n         Read a property with identifier <property_id> from the selected"
      "\n         image and generate a Unicode text representation. See -i for"
      "\n         image selection."
      "\n         Output suffix = '-image-property-<property_id>.txt'"
      "\n"
      "\n-rk | --read-fits-keywords"
      "\n"
      "\n         Read all FITS header keywords for the selected image and output"
      "\n         an ASCII tabular representation. See -i for image selection."
      "\n         Output suffix = '-fits-keywords.txt'"
      "\n"
      "\n-rs | --read-samples"
      "\n"
      "\n         Read a set of pixel sample rows and output them in CSV format."
      "\n         See the -i, -y, -n and -c arguments for image selection and"
      "\n         pixel coordinates."
      "\n         Output suffix = '-samples.csv'"
      "\n"
      "\n-i=<image>"
      "\n"
      "\n         <image> is the index of the image to read (-rp -rk -rs) (>= 0)."
      "\n"
      "\n-y=<row>"
      "\n"
      "\n         <row> is the first pixel row to read (-rs) (>= 0)."
      "\n"
      "\n-n=<count>"
      "\n"
      "\n         <count> is the number of rows to read (-rs) (>= 1)."
      "\n"
      "\n-c=<channel>"
      "\n"
      "\n         <channel> is the desired image channel (-rs) (>= 0)."
      "\n"
      "\n-os | --to-stdout"
      "\n"
      "\n         Send output to stdout instead of disk files, and suppress"
      "\n         superfluous informative messages."
      "\n"
      "\n--no-logo"
      "\n"
      "\n         Suppress the startup banner normally sent to stdout."
      "\n"
      "\n--r[+|-]"
      "\n"
      "\n         Enables/disables recursive directory search."
      "\n"
      "\n--help"
      "\n"
      "\n         Shows this help text and exits."
      "\n"
      "\n"
      "\nOutput files will have the same names as their corresponding input files "
      "\nwith the specified output suffix for each operation."
      "\n"
      "\n";
}

// ----------------------------------------------------------------------------

class VerboseLogHandler : public XISFLogHandler
{
public:

   VerboseLogHandler() = default;

   virtual void Init( const String& filePath, bool writing )
   {
   }

   virtual void Log( const String& text, message_type type )
   {
      switch ( type )
      {
      default:
      case XISFMessageType::Informative:
         std::cout << text;
         break;
      case XISFMessageType::Note:
         std::cout << "* " << text;
         break;
      case XISFMessageType::Warning:
         std::cerr << "** " << text;
         break;
      case XISFMessageType::RecoverableError:
         std::cerr << "*** " << text;
         break;
      }
   }

   virtual void Close()
   {
   }
};

class SilentLogHandler : public XISFLogHandler
{
public:

   SilentLogHandler() = default;

   virtual void Init( const String& filePath, bool writing )
   {
   }

   virtual void Log( const String& text, message_type type )
   {
      switch ( type )
      {
      default:
      case XISFMessageType::Informative:
      case XISFMessageType::Note:
         break;
      case XISFMessageType::Warning:
         std::cerr << "** " << text;
         break;
      case XISFMessageType::RecoverableError:
         std::cerr << "*** " << text;
         break;
      }
   }

   virtual void Close()
   {
   }
};

// ----------------------------------------------------------------------------

static void ExtractXISFHeaders( const StringList& files, bool toStdout )
{
   if ( !toStdout )
      std::cout << "\nExtracting " << files.Length() << " XISF header(s):\n\n";

   for ( const String& path : files )
   {
      std::cout << path << '\n';

      AutoPointer<XMLDocument> xml = XISFReader::ExtractHeader( path );
      xml->EnableAutoFormatting();
      xml->SetIndentSize( 3 );
      if ( toStdout )
         std::cout << xml->Serialize();
      else
         xml->SerializeToFile( File::ChangeExtension( File::AppendToName( path, "-header" ), ".xml" ) );
   }
}

// ----------------------------------------------------------------------------

static void EnumerateImages( const StringList& files, bool toStdout )
{
   if ( !toStdout )
      std::cout << "\nEnumerating images for " << files.Length() << " XISF unit(s):\n\n";

   for ( const String& path : files )
   {
      std::cout << path << '\n';

      XISFReader xisf;
      xisf.SetLogHandler( new SilentLogHandler );
      xisf.Open( path );

      IsoString text;

      for ( int i = 0; i < xisf.NumberOfImages(); ++i )
      {
         xisf.SelectImage( i );
         IsoString s;
         s << "* Image " << IsoString( i+1 ) << " of " << IsoString( xisf.NumberOfImages() ) << ':';
         if ( !xisf.ImageId().IsEmpty() )
            s << " id=" << xisf.ImageId();
         ImageInfo info = xisf.ImageInfo();
         ImageOptions options = xisf.ImageOptions();
         s << ' ' << XISF::SampleFormatId( options.bitsPerSample, options.ieeefpSampleFormat, options.complexSample );
         s << ' ' << XISF::ColorSpaceId( info.colorSpace );
         s.AppendFormat( " w=%d h=%d n=%d p=%u\n", info.width, info.height, info.numberOfChannels, xisf.ImageProperties().Length() );
         std::cout << s;
         if ( !toStdout )
            text << s;
      }

      xisf.Close();

      if ( !toStdout )
         File::WriteTextFile( File::ChangeExtension( File::AppendToName( path, "-images" ), ".txt" ), text );

      std::cout << '\n';
   }
}

// ----------------------------------------------------------------------------

static void EnumerateProperties( const StringList& files, bool toStdout )
{
   if ( !toStdout )
      std::cout << "\nEnumerating properties for " << files.Length() << " XISF unit(s):\n\n";

   for ( const String& path : files )
   {
      std::cout << path << '\n';

      XISFReader xisf;
      xisf.SetLogHandler( new SilentLogHandler );
      xisf.Open( path );

      IsoString text;

      for ( const PropertyDescription& p : xisf.Properties() )
      {
         IsoString s;
         s << p.id << " (" << XISF::PropertyTypeId( p.type ) << ")\n";
         std::cout << s;
         if ( !toStdout )
            text << s;
      }

      for ( int i = 0; i < xisf.NumberOfImages(); ++i )
      {
         xisf.SelectImage( i );
         PropertyDescriptionArray properties = xisf.ImageProperties();
         if ( !properties.IsEmpty() )
         {
            IsoString s;
            s << "* Image " << IsoString( i+1 ) << " of " << IsoString( xisf.NumberOfImages() );
            if ( !xisf.ImageId().IsEmpty() )
               s << " id=" << xisf.ImageId();
            s << '\n';
            std::cout << s;
            if ( !toStdout )
               text << s;

            for ( const PropertyDescription& p : xisf.ImageProperties() )
            {
               IsoString s;
               s << p.id << " (" << XISF::PropertyTypeId( p.type ) << ")\n";
               std::cout << s;
               if ( !toStdout )
                  text << s;
            }
         }
      }

      xisf.Close();

      if ( !toStdout )
         File::WriteTextFile( File::ChangeExtension( File::AppendToName( path, "-properties" ), ".txt" ), text );

      std::cout << '\n';
   }
}

// ----------------------------------------------------------------------------

static void ReadProperty( const StringList& files, const IsoString& propertyId, bool toStdout )
{
   if ( !toStdout )
      std::cout << "\nReading " << propertyId << " property from " << files.Length() << " XISF units(s):\n\n";

   for ( const String& path : files )
   {
      std::cout << path << '\n';

      XISFReader xisf;
      xisf.SetLogHandler( new SilentLogHandler );
      xisf.Open( path );

      Variant value = xisf.ReadProperty( propertyId );
      if ( !value.IsValid() )
      {
         std::cerr << "* Not found.\n";
         continue;
      }

      IsoString text;

      String valueAsString = value.ToString();
      std::wcout << valueAsString;
      if ( !toStdout )
         text << valueAsString.ToUTF8();

      xisf.Close();

      if ( !toStdout )
         File::WriteTextFile( File::ChangeExtension( File::AppendToName( path, "-property-" + propertyId ), ".txt" ), text );

      std::cout << '\n';
      if ( !toStdout )
         std::cout << '\n';
   }
}

// ----------------------------------------------------------------------------

static void ReadImageProperty( const StringList& files, int imageIndex, const IsoString& propertyId, bool toStdout )
{
   if ( !toStdout )
      std::cout << "\nReading " << propertyId << " image property from " << files.Length() << " XISF units(s):\n\n";

   for ( const String& path : files )
   {
      std::cout << path << '\n';

      XISFReader xisf;
      xisf.SetLogHandler( new SilentLogHandler );
      xisf.Open( path );

      if ( xisf.NumberOfImages() < 1 )
      {
         std::cerr << "The XSIF unit contains no readable image.\n";
         continue;
      }

      if ( xisf.NumberOfImages() <= imageIndex )
      {
         std::cerr << "The XSIF unit contains only " << xisf.NumberOfImages() << " images.\n";
         continue;
      }

      xisf.SelectImage( imageIndex );

      Variant value = xisf.ReadImageProperty( propertyId );
      if ( !value.IsValid() )
      {
         std::cerr << "*** Not found.\n";
         continue;
      }

      IsoString text;

      String valueAsString = value.ToString();
      std::wcout << valueAsString;
      if ( !toStdout )
         text << valueAsString.ToUTF8();

      xisf.Close();

      if ( !toStdout )
         File::WriteTextFile( File::ChangeExtension( File::AppendToName( path, "-image-property-" + propertyId ), ".txt" ), text );

      std::cout << '\n';
      if ( !toStdout )
         std::cout << '\n';
   }
}

// ----------------------------------------------------------------------------

static void ReadFITSHeaderKeywords( const StringList& files, int imageIndex, bool toStdout )
{
   if ( !toStdout )
      std::cout << "\nReading FITS header keywords from " << files.Length() << " XISF units(s):\n\n";

   for ( const String& path : files )
   {
      std::cout << path << '\n';

      XISFReader xisf;
      xisf.SetLogHandler( new SilentLogHandler );
      xisf.Open( path );

      if ( xisf.NumberOfImages() < 1 )
      {
         std::cerr << "The XSIF unit contains no readable image.\n";
         continue;
      }

      if ( xisf.NumberOfImages() <= imageIndex )
      {
         std::cerr << "The XSIF unit contains only " << xisf.NumberOfImages() << " images.\n";
         continue;
      }

      xisf.SelectImage( imageIndex );

      FITSKeywordArray keywords = xisf.ReadFITSKeywords();

      xisf.Close();

      size_type maxNameLength = 8;
      size_type maxValueLength = 0;
      for ( const FITSHeaderKeyword& keyword : keywords )
      {
         if ( keyword.name.Length() > maxNameLength )
            maxNameLength = keyword.name.Length();
         if ( keyword.value.Length() > maxValueLength )
            maxValueLength = keyword.value.Length();
      }

      IsoString text;
      for ( const FITSHeaderKeyword& keyword : keywords )
         text << keyword.name.LeftJustified( maxNameLength+1 )
              << keyword.value.LeftJustified( maxValueLength+1 )
              << keyword.comment.Trimmed()
              << '\n';

      std::cout << text;
      if ( !toStdout )
         File::WriteTextFile( File::ChangeExtension( File::AppendToName( path, "-fits-keywords" ), ".txt" ), text );

      if ( !toStdout )
         std::cout << '\n';
   }
}

// ----------------------------------------------------------------------------

static IsoString ReadSamplesAsCSVText( XISFReader& reader, int firstRow, int rowCount, int width, int channel )
{
   DVector samples( rowCount * width );

   reader.ReadSamples( samples.Begin(), firstRow, rowCount, channel );
   IsoString text;
   for ( int i = 0, row = 0; row < rowCount; ++row )
   {
      for ( int col = 0; ; )
      {
         text.AppendFormat( "%.8lg", samples[i++] );
         if ( ++col == width )
            break;
         text << ',';
      }
      text << '\n';
   }
   return text;
}

static void ReadPixelSamples( const StringList& files, int imageIndex, int firstRow, int rowCount, int channel, bool toStdout )
{
   if ( !toStdout )
      std::cout << "\nReading pixel samples from " << files.Length() << " XISF units(s):\n\n";

   for ( const String& path : files )
   {
      if ( !toStdout )
         std::cout << path << '\n';

      XISFReader xisf;
      xisf.SetLogHandler( new VerboseLogHandler );
      xisf.Open( path );

      if ( xisf.NumberOfImages() < 1 )
      {
         std::cerr << "The XSIF unit contains no readable image.\n";
         continue;
      }

      if ( xisf.NumberOfImages() <= imageIndex )
      {
         std::cerr << "The XSIF unit contains only " << xisf.NumberOfImages() << " images.\n";
         continue;
      }

      xisf.SelectImage( imageIndex );

      ImageInfo info = xisf.ImageInfo();

      if ( info.numberOfChannels <= channel )
      {
         std::cerr << "The image has only " << info.numberOfChannels << " channels.\n";
         continue;
      }

      if ( info.height <= firstRow )
      {
         std::cerr << "The image has only " << info.height << " pixel rows.\n";
         continue;
      }

      IsoString csv = ReadSamplesAsCSVText( xisf, firstRow, Range( rowCount, 1, info.height-firstRow ), info.width, channel );

      xisf.Close();

      if ( toStdout )
         std::cout << csv;
      else
         File::WriteTextFile( File::ChangeExtension( File::AppendToName( path, "-samples" ), ".csv" ), csv );

      if ( !toStdout )
         std::cout << '\n';
   }
}

// ----------------------------------------------------------------------------

static bool IsValidPropertyId( const IsoString& id )
{
   IsoStringList tokens;
   id.Break( tokens, ':' );
   for ( const IsoString& token : tokens )
      if ( !token.IsValidIdentifier() )
         return false;
   return true;
}

// ----------------------------------------------------------------------------

int main( int argc, const char** argv )
{
   Exception::DisableGUIOutput();
   Exception::EnableConsoleOutput();

   try
   {
      StringList cmdLineArgs;
      for ( int i = 1; i < argc; ++i )
         cmdLineArgs.Add( argv[i] );

      ArgumentList arguments = ExtractArguments( cmdLineArgs,
                                                 ArgumentItemMode::AsFiles,
                                                 ArgumentOption::AllowWildcards |
                                                 ArgumentOption::RecursiveDirSearch |
                                                 ArgumentOption::RecursiveSearchArgs );
      StringList files;
      bool extractHeaders = false;
      bool enumerateImages = false;
      bool enumerateProperties = false;
      bool readProperty = false;
      IsoString propertyId;
      bool readImageProperty = false;
      IsoString imagePropertyId;
      bool readFITSKeywords = false;
      bool readSamples = false;
      int imageIndex = 0, firstRow = 0, rowCount = 1, channel = 0;
      bool toStdout = false;
      bool noLogo = false;

      for ( const Argument& arg : arguments )
      {
         if ( arg.IsNumeric() )
         {
            if ( arg.Id() == "i" )
            {
               imageIndex = RoundInt( arg.NumericValue() );
               if ( imageIndex < 0 )
                  throw Error( "Invalid image index parameter: " + arg.Token() );
            }
            else if ( arg.Id() == "y" )
            {
               firstRow = RoundInt( arg.NumericValue() );
               if ( firstRow < 0 )
                  throw Error( "Invalid first row parameter: " + arg.Token() );
            }
            else if ( arg.Id() == "n" )
            {
               rowCount = RoundInt( arg.NumericValue() );
               if ( rowCount <= 0 )
                  throw Error( "Invalid row count parameter: " + arg.Token() );
            }
            else if ( arg.Id() == "c" )
            {
               channel = RoundInt( arg.NumericValue() );
               if ( channel < 0 )
                  throw Error( "Invalid channel index parameter: " + arg.Token() );
            }
            else
               throw Error( "Unknown numeric argument: " + arg.Token() );
         }
         else if ( arg.IsString() )
         {
            if ( arg.Id() == "rP" || arg.Id() == "-read-property" )
            {
               readProperty = true;
               propertyId = arg.StringValue();
               if ( !IsValidPropertyId( propertyId ) )
                  throw Error( "Invalid XISF property identifier: " + arg.Token() );
            }
            else if ( arg.Id() == "rp" || arg.Id() == "-read-image-property" )
            {
               readImageProperty = true;
               imagePropertyId = arg.StringValue();
               if ( !IsValidPropertyId( imagePropertyId ) )
                  throw Error( "Invalid XISF property identifier: " + arg.Token() );
            }
            else
               throw Error( "Unknown string argument: " + arg.Token() );
         }
         else if ( arg.IsSwitch() )
         {
            throw Error( "Unknown switch argument: " + arg.Token() );
         }
         else if ( arg.IsLiteral() )
         {
            if ( arg.Id() == "h" || arg.Id() == "-header" )
               extractHeaders = true;
            else if ( arg.Id() == "ei" || arg.Id() == "-enumerate-images" )
               enumerateImages = true;
            else if ( arg.Id() == "ep" || arg.Id() == "-enumerate-properties" )
               enumerateProperties = true;
            else if ( arg.Id() == "rk" || arg.Id() == "-read-fits-keywords" )
               readFITSKeywords = true;
            else if ( arg.Id() == "rs" || arg.Id() == "-read-samples" )
               readSamples = true;
            else if ( arg.Id() == "os" || arg.Id() == "-to-stdout" )
               toStdout = true;
            else if ( arg.Id() == "-no-logo" )
               noLogo = true;
            else if ( arg.Id() == "-help" )
            {
               if ( !noLogo )
                  ShowLogo();
               ShowUsage();
               return 0;
            }
            else
               throw Error( "Unknown argument: " + arg.Token() );
         }
         else if ( arg.IsItemList() )
         {
            for ( const String& file : arg.Items() )
               files << file;
         }
      }

      if ( files.IsEmpty() )
         throw pcl::Error( "No input files have been selected." );

      if ( !noLogo )
         ShowLogo();

      if ( extractHeaders )
         ExtractXISFHeaders( files, toStdout );

      if ( enumerateImages )
         EnumerateImages( files, toStdout );

      if ( enumerateProperties )
         EnumerateProperties( files, toStdout );

      if ( readProperty )
         ReadProperty( files, propertyId, toStdout );

      if ( readFITSKeywords )
         ReadFITSHeaderKeywords( files, imageIndex, toStdout );

      if ( readImageProperty )
         ReadImageProperty( files, imageIndex, imagePropertyId, toStdout );

      if ( readSamples )
         ReadPixelSamples( files, imageIndex, firstRow, rowCount, channel, toStdout );

      std::cout << '\n' << std::flush;

      return 0;
   }

   ERROR_HANDLER
   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/xisf.cpp - Released 2017-04-16T10:37:44Z
