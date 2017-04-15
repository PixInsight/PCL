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
      "\nPixInsight XISF Utility - "
   << Version::AsString()
   << "\nCopyright (c) 2014-"
   << []{ int year, month, day; double dayf; TimePoint::Now().GetComplexTime( year, month, day, dayf ); return year; }()
   << " Pleiades Astrophoto. All Rights Reserved"
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
      "\n         Extract XISF headers and write them to XML files. Output files"
      "\n         will have the same names as the corresponding input files with"
      "\n         the '-header.xml' suffix."
      "\n"
      "\n-r | --read"
      "\n"
      "\n         Read a set of pixel sample rows and output them in CSV format."
      "\n         See the -y, -n and -c arguments for read coordinates."
      "\n         Output files will have the same names as the corresponding"
      "\n         input files with the '-samples.csv' suffix."
      "\n"
      "\n-y=<row>"
      "\n"
      "\n         <row> is the first pixel row to read (>= 0, 0 by default)."
      "\n"
      "\n-n=<count>"
      "\n"
      "\n         <count> is the number of rows to read (>= 1, 1 by default)."
      "\n"
      "\n-c=<channel>"
      "\n"
      "\n         <channel> is the desired image channel (>= 0, 0 by default)."
      "\n"
      "\n-os | --to-stdout"
      "\n"
      "\n         Send output to stdout instead of disk files."
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
      "\n";
}

// ----------------------------------------------------------------------------

class LogHandler : public XISFLogHandler
{
public:

   LogHandler() = default;

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

// ----------------------------------------------------------------------------

static void ExtractXISFHeaders( const StringList& files, bool toStdout )
{
   if ( !toStdout )
      std::cout << String().Format( "\nExtracting %u XISF header(s):\n\n", files.Length() );

   for ( const String& path : files )
   {
      if ( !toStdout )
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

static void ReadPixelSamples( const StringList& files, int firstRow, int rowCount, int channel, bool toStdout )
{
   if ( !toStdout )
      std::cout << String().Format( "\nReading %u XISF file(s):\n\n", files.Length() );

   for ( const String& path : files )
   {
      if ( !toStdout )
         std::cout << path << '\n';

      XISFReader xisf;
      xisf.SetLogHandler( new LogHandler );
      xisf.Open( path );

      if ( xisf.NumberOfImages() < 1 )
      {
         std::cerr << "The XSIF file contains no readable images.\n";
         continue;
      }

      //xisf.SelectImage( 0 );

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
      bool readSamples = false;
      int firstRow = 0, rowCount = 1, channel = 0;
      bool toStdout = false;
      bool noLogo = false;

      for ( const Argument& arg : arguments )
      {
         if ( arg.IsNumeric() )
         {
            if ( arg.Id() == "y" )
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
            else if ( arg.Id() == "r" || arg.Id() == "--read" )
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

      if ( readSamples )
         ReadPixelSamples( files, firstRow, rowCount, channel, toStdout );

      std::cout << '\n' << std::flush;

      return 0;
   }

   ERROR_HANDLER
   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/xisf.cpp - Released 2017-04-15T23:26:38Z
