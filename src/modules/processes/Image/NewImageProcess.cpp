//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0410
// ----------------------------------------------------------------------------
// NewImageProcess.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "NewImageProcess.h"
#include "NewImageParameters.h"
#include "NewImageInstance.h"
#include "NewImageInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

NewImageProcess* TheNewImageProcess = nullptr;

// ----------------------------------------------------------------------------

#include "NewImageIcon.xpm"

// ----------------------------------------------------------------------------

NewImageProcess::NewImageProcess() : MetaProcess()
{
   TheNewImageProcess = this;

   // Instantiate process parameters
   new NewImageIdentifier( this );
   new NewImageWidth( this );
   new NewImageHeight( this );
   new NewImageNumberOfChannels( this );
   new NewImageColorSpace( this );
   new NewImageSampleFormat( this );
   new NewImageV0( this );
   new NewImageV1( this );
   new NewImageV2( this );
}

// ----------------------------------------------------------------------------

IsoString NewImageProcess::Id() const
{
   return "NewImage";
}

// ----------------------------------------------------------------------------

IsoString NewImageProcess::Category() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

uint32 NewImageProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String NewImageProcess::Description() const
{
   return

   "<html>"
   "<p>NewImage is the standard process used to create new images in PixInsight. "
   "A complete set of parameters can be specified for images generated with the "
   "NewImage process: image identifier, sample format (among all supported integer "
   "and floating-point formats), color space (RGB color or grayscale), geometric "
   "properties (width and height in pixels, and number of channels, including an "
   "unlimited amount of alpha channels), and initial sample values.</p>"

   "<p>If the image identifier is left blank, an automatically-generated identifier "
   "will be used, following naming criteria as specified by global preferences.</p>"

   "<p>Alpha channels (those in excess of nominal channels: three for RGB color images "
   "and one for grayscale images) are initialized to black.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** NewImageProcess::IconImageXPM() const
{
   return NewImageIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* NewImageProcess::DefaultInterface() const
{
   return TheNewImageInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* NewImageProcess::Create() const
{
   return new NewImageInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* NewImageProcess::Clone( const ProcessImplementation& p ) const
{
   const NewImageInstance* instPtr = dynamic_cast<const NewImageInstance*>( &p );
   return (instPtr != nullptr) ? new NewImageInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool NewImageProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool NewImageProcess::PrefersGlobalExecution() const
{
   return true; // this is a global process
}

// ----------------------------------------------------------------------------

bool NewImageProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: NewImage [<arg_list>]"
"\n"
"\n-id=<id>"
"\n"
"\n      <id> is the identifier that will be assigned to the new image."
"\n"
"\n-w=<n> | -width=<n>"
"\n"
"\n      <n> is the width of the new image in pixels, n >= 1. (default=256)"
"\n"
"\n-h=<n> | -height=<n>"
"\n"
"\n      <n> is the height of the new image in pixels, n >= 1. (default=256)"
"\n"
"\n-n=<n> | -channels=<n>"
"\n"
"\n      <n> is the total number of channels, including nominal channels (one"
"\n      for grayscale images and three for RGB color images) and optional"
"\n      alpha channels."
"\n"
"\n-c | -rgb | -rgbcolor"
"\n"
"\n      Create a RGB color image. If specified, the value of -n must be >= 3."
"\n      (default=RGB)"
"\n"
"\n-g | -gray | -grayscale"
"\n"
"\n      Create a grayscale image. If specified, the value of -n must be >= 1."
"\n"
"\n-i<n>"
"\n"
"\n      Create an integer image. <n> specifies the number of bits per sample"
"\n      and must be one of: 8,16,32."
"\n"
"\n-f<n>"
"\n"
"\n      Create a floating point image. <n> specifies the number of bits per"
"\n      sample and must be one of: 32,64. (default=-f32)"
"\n"
"\n-v0=<n> | -red=<n> | -k=<n>"
"\n"
"\n      Initial value for the red or gray channel in the normalized [0,1] range."
"\n      (default=0)"
"\n"
"\n-v1=<n> | -green=<n>"
"\n"
"\n      Initial value for the green channel in the normalized [0,1] range."
"\n      (default=0)"
"\n"
"\n-v2=<n> | -blue=<n>"
"\n"
"\n      Initial value for the blue channel in the normalized [0,1] range."
"\n      (default=0)"
"\n"
"\n-va=<n> | -alpha=<n>"
"\n"
"\n      Initial value for the first alpha channel in the normalized [0,1] range."
"\n      (default=0)"
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

static void ValidateInitialValue( const Argument& arg )
{
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 )
      throw Error( "Invalid normalized sample value: " + arg.Token() );
}

int NewImageProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   NewImageInstance instance( this );
   bool launchInterface = false;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "w" || arg.Id() == "width" )
         {
            int w = int( arg.NumericValue() );
            if ( w <= 0 )
               throw Error( "Invalid image width: " + arg.Token() );
            instance.width = w;
         }
         else if ( arg.Id() == "h" || arg.Id() == "height" )
         {
            int h = int( arg.NumericValue() );
            if ( h <= 0 )
               throw Error( "Invalid image height: " + arg.Token() );
            instance.height = h;
         }
         else if ( arg.Id() == "n" || arg.Id() == "channels" )
         {
            int n = int( arg.NumericValue() );
            if ( n < 1 )
               throw Error( "Invalid channel count: " + arg.Token() );
            instance.numberOfChannels = n;
         }
         else if ( arg.Id() == "v0" || arg.Id() == "red" || arg.Id() == "k" )
         {
            ValidateInitialValue( arg );
            instance.v0 = arg.NumericValue();
         }
         else if ( arg.Id() == "v1" || arg.Id() == "green" )
         {
            ValidateInitialValue( arg );
            instance.v1 = arg.NumericValue();
         }
         else if ( arg.Id() == "v2" || arg.Id() == "blue" )
         {
            ValidateInitialValue( arg );
            instance.v2 = arg.NumericValue();
         }
         else if ( arg.Id() == "va" || arg.Id() == "alpha" )
         {
            ValidateInitialValue( arg );
            instance.va = arg.NumericValue();
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "id" || arg.Id() == "identifier" )
         {
            if ( !arg.StringValue().IsValidIdentifier() )
               throw Error( "Invalid image identifier: " + arg.Token() );
            instance.id = arg.StringValue();
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "i8" )
            instance.sampleFormat = NewImageSampleFormat::I8;
         else if ( arg.Id() == "i16" )
            instance.sampleFormat = NewImageSampleFormat::I16;
         else if ( arg.Id() == "i32" )
            instance.sampleFormat = NewImageSampleFormat::I32;
         else if ( arg.Id() == "f32" )
            instance.sampleFormat = NewImageSampleFormat::F32;
         else if ( arg.Id() == "f64" )
            instance.sampleFormat = NewImageSampleFormat::F64;
         else if ( arg.Id() == "c" || arg.Id() == "rgb" || arg.Id() == "rgbcolor" )
            instance.colorSpace = NewImageColorSpace::RGB;
         else if ( arg.Id() == "g" || arg.Id() == "gray" || arg.Id() == "grayscale" )
            instance.colorSpace = NewImageColorSpace::Gray;
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

   if ( instance.colorSpace == NewImageColorSpace::RGB && instance.numberOfChannels < 3 )
      throw Error( "Too few channels specified for a RGB color image" );

   if ( launchInterface || arguments.IsEmpty() )
      instance.LaunchInterface();
   else
      instance.LaunchGlobal();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NewImageProcess.cpp - Released 2018-11-01T11:07:21Z
