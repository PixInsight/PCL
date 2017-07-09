//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0390
// ----------------------------------------------------------------------------
// SampleFormatConversionProcess.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "SampleFormatConversionProcess.h"
#include "SampleFormatConversionParameters.h"
#include "SampleFormatConversionInstance.h"
#include "SampleFormatConversionInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SampleFormatConversionProcess* TheSampleFormatConversionProcess = 0;

// ----------------------------------------------------------------------------

#include "SampleFormatConversionIcon.xpm"

// ----------------------------------------------------------------------------

SampleFormatConversionProcess::SampleFormatConversionProcess() : MetaProcess()
{
   TheSampleFormatConversionProcess = this;

   // Instantiate process parameter
   new SampleFormatConversion( this );
}

// ----------------------------------------------------------------------------

IsoString SampleFormatConversionProcess::Id() const
{
   return "SampleFormatConversion";
}

// ----------------------------------------------------------------------------

IsoString SampleFormatConversionProcess::Category() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

uint32 SampleFormatConversionProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String SampleFormatConversionProcess::Description() const
{
   return

   "<html>"
   "<p>The SampleFormatConversion process converts images between the different sample formats supported by PixInsight: 8, 16 and "
   "32-bit integers, 32 and 64-bit floating point. The sample format defines a numerical type and range used internally to store "
   "and manipulate pixel data in memory.</p>"

   "<p>In general, the 8-bit integer format should be avoided to perform actual image processing, as far as possible, due to its limited "
   "range of just 256 discrete values. It can be useful, however, to manage huge image files, due to its low memory consumption.</p>"

   "<p>The 16-bit integer format uses the native range of most CCD and CMOS imaging devices and is quite appropriate for processing, but "
   "in general a 32-bit format is preferable. The 16-bit format is a good option to store relatively simple masks, since it requires "
   "just two bytes for each gray pixel value.</p>"

   "<p>The 32-bit integer format, along with the 64-bit floating-point format, is an excellent option for processing works requiring "
   "extremely large dynamic ranges, thanks to its wide data range of 2^32 discrete values. The 32-bit integer format can be somewhat "
   "slow with processes that perform intensive calculations.</p>"

   "<p>Floating-point formats provide high numerical accuracy, which is essential to avoid image degradation due to roundoff errors, "
   "especially during complex and extended processing works. The 32-bit floating-point format is probably the best-balanced option "
   "available in PixInsight. It provides an effective data range of 10^6 to 10^7 discrete levels in PixInsight, depending on the "
   "complexity of the applied operations.</p>"

   "<p>The 64-bit floating-point format provides a huge data range of 10^15 discrete levels, but it requires 24 bytes to store each "
   "pixel of a RGB color image, so it can be prohibitive for large images on 32-bit platforms. This format is indicated for complex "
   "processing works that must preserve extremely large dynamic ranges. For example, with the 64-bit floating-point format you can "
   "integrate a set of exposures from 1/1000 of a second to several seconds as a single image without any data loss. Or the whole "
   "brightness range and chromatic richness in the Great Orion Nebula, rendering from the most subtle details of the Trapezium area "
   "to the dimmest nebular tendrils on the regions between M43 and NGC 1977.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** SampleFormatConversionProcess::IconImageXPM() const
{
   return SampleFormatConversionIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* SampleFormatConversionProcess::DefaultInterface() const
{
   return TheSampleFormatConversionInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* SampleFormatConversionProcess::Create() const
{
   return new SampleFormatConversionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SampleFormatConversionProcess::Clone( const ProcessImplementation& p ) const
{
   const SampleFormatConversionInstance* instPtr = dynamic_cast<const SampleFormatConversionInstance*>( &p );
   return (instPtr != 0) ? new SampleFormatConversionInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: SampleFormatConversion [<arg_list>] [<view_list>]"
"\n"
"\n-i<n>"
"\n"
"\n      Convert to an integer format. <n> specifies the number of bits per"
"\n      sample and must be one of: 8,16,32."
"\n"
"\n-f<n>"
"\n"
"\n      Convert to a floating point real format. <n> specifies the number of"
"\n      bits per sample and must be one of: 32,64."
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int SampleFormatConversionProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews,
                     ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   SampleFormatConversionInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
         throw Error( "Unknown numeric argument: " + arg.Token() );
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "i8" )
            instance.sampleFormat = SampleFormatConversion::To8Bit;
         else if ( arg.Id() == "i16" )
            instance.sampleFormat = SampleFormatConversion::To16Bit;
         else if ( arg.Id() == "i32" )
            instance.sampleFormat = SampleFormatConversion::To32Bit;
         else if ( arg.Id() == "f32" )
            instance.sampleFormat = SampleFormatConversion::ToFloat;
         else if ( arg.Id() == "f64" )
            instance.sampleFormat = SampleFormatConversion::ToDouble;
         else if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            continue;
         }

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "No such view: " + *j );
            instance.LaunchOn( v );
         }
      }
   }

   if ( launchInterface )
      instance.LaunchInterface();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentWindow();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionProcess.cpp - Released 2017-07-09T18:07:33Z
