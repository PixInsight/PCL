//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0374
// ----------------------------------------------------------------------------
// RGBWorkingSpaceProcess.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "RGBWorkingSpaceProcess.h"
#include "RGBWorkingSpaceParameters.h"
#include "RGBWorkingSpaceInstance.h"
#include "RGBWorkingSpaceInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RGBWorkingSpaceProcess*  TheRGBWorkingSpaceProcess = 0;

// ----------------------------------------------------------------------------

#include "RGBWorkingSpaceIcon.xpm"

// ----------------------------------------------------------------------------

RGBWorkingSpaceProcess::RGBWorkingSpaceProcess() : MetaProcess()
{
   TheRGBWorkingSpaceProcess = this;

   // Instantiate process parameters

   new RGBWSChannelTable( this );
   new RGBWSLuminanceCoefficient( TheRGBWSChannelTableParameter );
   new RGBWSChromaticityX( TheRGBWSChannelTableParameter );
   new RGBWSChromaticityY( TheRGBWSChannelTableParameter );
   new RGBWSGamma( this );
   new RGBWSsRGBG( this );
   new RGBWSApplyGlobal( this );
}

// ----------------------------------------------------------------------------

IsoString RGBWorkingSpaceProcess::Id() const
{
   return "RGBWorkingSpace";
}

// ----------------------------------------------------------------------------

IsoString RGBWorkingSpaceProcess::Category() const
{
   return "ColorSpaces";
}

// ----------------------------------------------------------------------------

uint32 RGBWorkingSpaceProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String RGBWorkingSpaceProcess::Description() const
{
   return

   "<html>"
   "<p>Separating a color image into its luminance and chrominance components is a fundamental "
   "task of image processing. To calculate luminance and chrominance values, PixInsight uses a "
   "colorimetrically defined <i>RGB working space</i> (RGBWS). Such a RGBWS is defined by the "
   "following parameters:</p>"

   "<ul>"
   "<li><b>Luminance coefficients.</b> These are the relative weights of red, green and blue used "
   "to calculate the luminance of a pixel.<br/></li>"

   "<li><b>Chromaticity coordinates.</b> The x, y coordinates of the red, green and blue primaries. "
   "These colors are the fundamental colorants of the RGB color space.<br/></li>"

   "<li><b>Gamma.</b> A function used to linearize RGB components when converting them into a linear "
   "transformation space such as CIE XYZ. With the exception of the sRGB space, the gamma function is "
   "just the raise function (exponentiation), and the value of gamma is the exponent applied.<br/></li>"
   "</ul>"

   "<p>RGB Working Spaces in PixInsight have nothing to do with color management or ICC profiles. Color "
   "management comprises color spaces and transformations used to achieve consistent color through "
   "different imaging devices. A RGBWS is used strictly for pure image processing tasks. This differs "
   "from what is customary in most imaging applications.</p>"

   "<p>Each image window can use its own, <i>local RGBWS</i>. For images that don't have their own RGBWS "
   "(as happens by default), there exists a <i>global RGBWS</i>. By default, the global RGBWS is the "
   "sRGB space, mainly for compatibility with other applications. Note that sRGB is usually far from "
   "optimal for deep-sky astronomical images.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** RGBWorkingSpaceProcess::IconImageXPM() const
{
   return RGBWorkingSpaceIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* RGBWorkingSpaceProcess::DefaultInterface() const
{
   return TheRGBWorkingSpaceInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* RGBWorkingSpaceProcess::Create() const
{
   return new RGBWorkingSpaceInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* RGBWorkingSpaceProcess::Clone( const ProcessImplementation& p ) const
{
   const RGBWorkingSpaceInstance* instPtr = dynamic_cast<const RGBWorkingSpaceInstance*>( &p );
   return (instPtr != 0) ? new RGBWorkingSpaceInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceProcess.cpp - Released 2019-01-21T12:06:41Z
