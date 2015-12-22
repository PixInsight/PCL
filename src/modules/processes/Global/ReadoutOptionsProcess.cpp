//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0324
// ----------------------------------------------------------------------------
// ReadoutOptionsProcess.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ReadoutOptionsParameters.h"
#include "ReadoutOptionsProcess.h"
#include "ReadoutOptionsInstance.h"
#include "ReadoutOptionsInterface.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptionsProcess* TheReadoutOptionsProcess = 0;

// ----------------------------------------------------------------------------

#include "ReadoutOptionsIcon.xpm"

// ----------------------------------------------------------------------------

ReadoutOptionsProcess::ReadoutOptionsProcess() : MetaProcess()
{
   TheReadoutOptionsProcess = this;

   // Instantiate process parameters
   new ReadoutOptionsData( this );
   new ReadoutOptionsMode( this );
   new ReadoutOptionsProbeSize( this );
   new ReadoutOptionsPreviewSize( this );
   new ReadoutOptionsPreviewZoomFactor( this );
   new ReadoutRealPrecision( this );
   new ReadoutIntegerRange( this );
   new ReadoutAlpha( this );
   new ReadoutMask( this );
   new ReadoutPreview( this );
   new ReadoutPreviewCenter( this );
   new ReadoutBroadcast( this );
   new ReadoutReal( this );
}

// ----------------------------------------------------------------------------

IsoString ReadoutOptionsProcess::Id() const
{
   return "ReadoutOptions";
}

// ----------------------------------------------------------------------------

IsoString ReadoutOptionsProcess::Category() const
{
   return "Global";
}

// ----------------------------------------------------------------------------

uint32 ReadoutOptionsProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ReadoutOptionsProcess::Description() const
{
   return

   "<html>"
   "<p>ReadoutOptions provides access to the parameters that define how PixInsight "
   "calculates and presents numerical pixel data obtained directly from images.</p>"

   "<p>Readouts are generated dynamically when you move the mouse cursor over an image "
   "in PixInsight. Readout data are shown on the Readout toolbar, usually located at the "
   "bottom of the application's main window. Furthermore, when you press the mouse cursor "
   "over an image, readout values are sent to interface windows that request them, as the "
   "standard HistogramTransformation and CurvesTransformation interfaces, for example. "
   "This is called <i>readout broadcasting</i>. When an interface receives readout values, "
   "it usually gives you some visual feedback, or use them to modify its behavior, as "
   "appropriate.</p>"

   "<p>With ReadoutOptions you can specify two sets of parameters: a set that defines "
   "how readout values are calcualted, and a second set that indicates how the obtained "
   "numerical readout values are presented to you.</p>"

   "<p>The first parameter set includes the color space in which data is generated "
   "(e.g. RGB, HSV, CIE L*a*b*, etc.), the calculation mode used (average, median, minimum "
   "or maximum), and the size of the square box used to calculate readouts.</p>"

   "<p>The second set of parameters lets you choose between real and integer readouts. "
   "Real readouts are normalized to the [0,1] interval, where 0=black and 1=white. For "
   "real readouts you may also specify a resolution in terms of the amount of decimal "
   "digits shown (from 0 to 15). Integer readouts are given in the range from zero to an "
   "arbitrary maximum value that you can specify from 1 to 2<sup>32</sup>.</p>"

   "<p>Finally, ReadoutOptions allows you to control generation of <i>readout previews</i>. "
   "A readout preview is a real-time rendition of a small image area at the cursor location, "
   "which is shown when you hold the primary mouse button pressed for more than about 0.4 "
   "seconds in the <i>readout</i> and <i>center</i> navigation modes. With ReadoutOptions "
   "you can define the size, magnification and appearance of readout previews.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** ReadoutOptionsProcess::IconImageXPM() const
{
   return ReadoutOptionsIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* ReadoutOptionsProcess::DefaultInterface() const
{
   return TheReadoutOptionsInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ReadoutOptionsProcess::Create() const
{
   return new ReadoutOptionsInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ReadoutOptionsProcess::Clone( const ProcessImplementation& p ) const
{
   const ReadoutOptionsInstance* instPtr = dynamic_cast<const ReadoutOptionsInstance*>( &p );
   return (instPtr != 0) ? new ReadoutOptionsInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsProcess::PrefersGlobalExecution() const
{
   return true; // this is a global process
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: ReadoutOptions [<arg_list>]"
"\n"
"\n* Arguments controlling the readout output color space and data."
"\n"
"\n-rgb | -RGB"
"\n"
"\n      Display pixel readouts in the RGB or grayscale color spaces."
"\n"
"\n-rgbl | -RGBL"
"\n"
"\n      Display RGB readouts plus the CIE L* component."
"\n"
"\n-rgby | -RGBY"
"\n"
"\n      Display RGB readouts plus the CIE Y component."
"\n"
"\n-xyz | -XYZ"
"\n"
"\n      Display pixel readouts in the CIE XYZ color space."
"\n"
"\n-lab | -Lab"
"\n"
"\n      Display pixel readouts in the CIE L*a*b* color space."
"\n"
"\n-lch | -Lch"
"\n"
"\n      Display pixel readouts in the CIE L*c*h* color space."
"\n"
"\n-hsv | -HSV"
"\n"
"\n      Display pixel readouts in the HSV color ordering system."
"\n"
"\n-hsi | -HSI|-HSL"
"\n"
"\n      Display pixel readouts in the HSI (aka HSL) color ordering system."
"\n"
"\n-alpha[+|-]"
"\n"
"\n      Enables/disables alpha channel readouts."
"\n"
"\n-mask[+|-]"
"\n"
"\n      Enables/disables mask channel readouts."
"\n"
"\n* Arguments controlling the calculation mode for readout generation."
"\n"
"\n-m | -mean"
"\n"
"\n      Generate arithmetic mean pixel readouts."
"\n"
"\n-n | -median"
"\n"
"\n      Generate median pixel readouts."
"\n"
"\n-M | -max"
"\n"
"\n      Generate maximum pixel readouts."
"\n"
"\n-N | -min"
"\n"
"\n      Generate minimum pixel readouts."
"\n"
"\n* Arguments controlling the readout probe and the readout preview."
"\n"
"\n-s=<n> | -size=<n>"
"\n"
"\n      <n> is the size in pixels of the square readout probe."
"\n      Valid sizes are: 1,3,5,7,9,11,13,15."
"\n"
"\n-ps=<n> | -preview-size=<n>"
"\n"
"\n      <n> is the size in pixels of the square readout preview box."
"\n      <n> must be an odd integer 15 <= n <= 127."
"\n"
"\n-pz=<n> | -preview-zoom=<n>"
"\n"
"\n      <n> is the zoom ratio for generation of readout previews."
"\n      <n> must be an integer 1 <= n <= 16."
"\n"
"\n-preview[+|-]"
"\n"
"\n      Enables/disables generation of readout previews."
"\n"
"\n-preview-center[+|-]"
"\n"
"\n      Enables/disables drawing of readout preview center lines."
"\n"
"\n* Arguments controlling readout data types for output."
"\n"
"\n-r=<n> | -real=<n>"
"\n"
"\n      Display normalized real readout values in the [0,1] range."
"\n      <n> is the number of decimal digits to show, 0 <= n <= 15"
"\n"
"\n-i8"
"\n"
"\n      Display 8-bit integer pixel readouts in the [0,255] range"
"\n"
"\n-i10"
"\n"
"\n      Display 10-bit integer pixel readouts in the [0,1023] range."
"\n"
"\n-i12"
"\n"
"\n      Display 12-bit integer pixel readouts in the [0,4095] range."
"\n"
"\n-i14"
"\n"
"\n      Display 14-bit integer pixel readouts in the [0,16383] range."
"\n"
"\n-i16"
"\n"
"\n      Display 16-bit integer pixel readouts in the [0,65535] range."
"\n"
"\n-i32"
"\n"
"\n      Display 32-bit integer pixel readouts in the [0,4294967295] range."
"\n"
"\n-i=<n> | integer=<n>"
"\n"
"\n      Display integer readouts in an arbitrary range from 0 to <n>."
"\n"
"\n* Miscellaneous arguments."
"\n"
"\n-broadcast[+|-]"
"\n"
"\n      Enables/disables broadcasting of generated pixel readouts."
"\n"
"\n--load"
"\n"
"\n      Load current readout options."
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

int ReadoutOptionsProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   ReadoutOptions options;
   bool launchInterface = false;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "s" || arg.Id() == "size" )
         {
            int sz = int( arg.NumericValue() );
            if ( (sz & 1) == 0 || sz < 1 || sz > ReadoutOptions::MaxProbeSize )
               throw Error( "Invalid readout probe size: " + arg.Token() );
            options.SetProbeSize( sz );
         }
         else if ( arg.Id() == "ps" || arg.Id() == "preview-size" )
         {
            int sz = int( arg.NumericValue() );
            if ( (sz & 1) == 0 || sz < ReadoutOptions::MinPreviewSize || sz > ReadoutOptions::MaxPreviewSize )
               throw Error( "Invalid readout preview size: " + arg.Token() );
            options.SetPreviewSize( sz );
         }
         else if ( arg.Id() == "pz" || arg.Id() == "preview-zoom" )
         {
            int sz = int( arg.NumericValue() );
            if ( sz < 1 || sz > ReadoutOptions::MaxPreviewZoomFactor )
               throw Error( "Invalid readout preview zoom factor: " + arg.Token() );
            options.SetPreviewZoomFactor( sz );
         }
         else if ( arg.Id() == "r" || arg.Id() == "real" )
         {
            options.SetReal();
            int n = int( arg.NumericValue() );
            if ( n < 0 || n > ReadoutOptions::MaxPrecision )
               throw Error( "Invalid readout real precision: " + arg.Token() );
            options.SetPrecision( n );
         }
         else if ( arg.Id() == "i" || arg.Id() == "integer" )
         {
            options.SetInteger();
            double n = arg.NumericValue();
            if ( n < 1 || n > uint32_max )
               throw Error( "Invalid integer readout range: " + arg.Token() );
            options.SetIntegerRange( unsigned( n ) );
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "alpha" )
            options.EnableAlphaChannel( arg.SwitchState() );
         else if ( arg.Id() == "mask" )
            options.EnableMaskChannel( arg.SwitchState() );
         else if ( arg.Id() == "preview" )
            options.EnablePreview( arg.SwitchState() );
         else if ( arg.Id() == "preview-center" )
            options.EnablePreviewCenter( arg.SwitchState() );
         else if ( arg.Id() == "broadcast" )
            options.EnableBroadcast( arg.SwitchState() );
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "m" || arg.Id() == "mean" )
            options.SetMode( ReadoutMode::Mean );
         else if ( arg.Id() == "n" || arg.Id() == "median" )
            options.SetMode( ReadoutMode::Median );
         else if ( arg.Id() == "M" || arg.Id() == "max" || arg.Id() == "maximum" )
            options.SetMode( ReadoutMode::Maximum );
         else if ( arg.Id() == "N" || arg.Id() == "min" || arg.Id() == "minimum" )
            options.SetMode( ReadoutMode::Minimum );
         else if ( arg.Id() == "rgb" || arg.Id() == "RGB" )
            options.SetData( ReadoutData::RGBK );
         else if ( arg.Id() == "rgbl" || arg.Id() == "RGBL" )
            options.SetData( ReadoutData::RGBL );
         else if ( arg.Id() == "rgby" || arg.Id() == "RGBY" )
            options.SetData( ReadoutData::RGBY );
         else if ( arg.Id() == "xyz" || arg.Id() == "XYZ" )
            options.SetData( ReadoutData::CIEXYZ );
         else if ( arg.Id() == "lab" || arg.Id() == "Lab" )
            options.SetData( ReadoutData::CIELab );
         else if ( arg.Id() == "lch" || arg.Id() == "Lch" )
            options.SetData( ReadoutData::CIELch );
         else if ( arg.Id() == "hsv" || arg.Id() == "HSV" )
            options.SetData( ReadoutData::HSV );
         else if ( arg.Id() == "hsi" || arg.Id() == "HSI" || arg.Id() == "HSL" )
            options.SetData( ReadoutData::HSI );
         else if ( arg.Id() == "i8" )
         {
            options.SetInteger();
            options.SetIntegerRange( 255 );
         }
         else if ( arg.Id() == "i10" )
         {
            options.SetInteger();
            options.SetIntegerRange( 1023 );
         }
         else if ( arg.Id() == "i12" )
         {
            options.SetInteger();
            options.SetIntegerRange( 4095 );
         }
         else if ( arg.Id() == "i14" )
         {
            options.SetInteger();
            options.SetIntegerRange( 16383 );
         }
         else if ( arg.Id() == "i16" )
         {
            options.SetInteger();
            options.SetIntegerRange( 65535 );
         }
         else if ( arg.Id() == "i32" )
         {
            options.SetInteger();
            options.SetIntegerRange( 4294967295ul );
         }
         else if ( arg.Id() == "alpha" )
            options.EnableAlphaChannel();
         else if ( arg.Id() == "mask" )
            options.EnableMaskChannel();
         else if ( arg.Id() == "preview" )
            options.EnablePreview();
         else if ( arg.Id() == "preview-center" )
            options.EnablePreviewCenter();
         else if ( arg.Id() == "broadcast" )
            options.EnableBroadcast();
         else if ( arg.Id() == "-load" )
            options = ReadoutOptions::GetCurrentOptions();
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
         throw Error( "Invalid non-parametric argument: " + arg.Token() );
   }

   ReadoutOptionsInstance instance( this );

   instance.SetOptions( options );

   if ( launchInterface || arguments.IsEmpty() )
      instance.LaunchInterface();
   else
      instance.LaunchGlobal();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsProcess.cpp - Released 2015/12/18 08:55:08 UTC
