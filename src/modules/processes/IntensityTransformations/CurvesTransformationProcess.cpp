//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0430
// ----------------------------------------------------------------------------
// CurvesTransformationProcess.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "CurvesTransformationProcess.h"
#include "CurvesTransformationParameters.h"
#include "CurvesTransformationInstance.h"
#include "CurvesTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CurvesTransformationProcess* TheCurvesTransformationProcess = nullptr;

// ----------------------------------------------------------------------------

#include "CurvesTransformationIcon.xpm"

// ----------------------------------------------------------------------------

CurvesTransformationProcess::CurvesTransformationProcess()
{
   TheCurvesTransformationProcess = this;

   // Instantiate process parameters

   new RedCurve( this );
   new XR( TheRedCurveParameter );
   new YR( TheRedCurveParameter );
   new RedCurveType( this );

   new GreenCurve( this );
   new XG( TheGreenCurveParameter );
   new YG( TheGreenCurveParameter );
   new GreenCurveType( this );

   new BlueCurve( this );
   new XB( TheBlueCurveParameter );
   new YB( TheBlueCurveParameter );
   new BlueCurveType( this );

   new RGBKCurve( this );
   new XRGBK( TheRGBKCurveParameter );
   new YRGBK( TheRGBKCurveParameter );
   new RGBKCurveType( this );

   new AlphaCurve( this );
   new XA( TheAlphaCurveParameter );
   new YA( TheAlphaCurveParameter );
   new AlphaCurveType( this );

   new CIELCurve( this );
   new XL( TheCIELCurveParameter );
   new YL( TheCIELCurveParameter );
   new CIELCurveType( this );

   new CIEaCurve( this );
   new Xa( TheCIEaCurveParameter );
   new Ya( TheCIEaCurveParameter );
   new CIEaCurveType( this );

   new CIEbCurve( this );
   new Xb( TheCIEbCurveParameter );
   new Yb( TheCIEbCurveParameter );
   new CIEbCurveType( this );

   new CIEcCurve( this );
   new Xc( TheCIEcCurveParameter );
   new Yc( TheCIEcCurveParameter );
   new CIEcCurveType( this );

   new HueCurve( this );
   new XH( TheHueCurveParameter );
   new YH( TheHueCurveParameter );
   new HueCurveType( this );

   new SaturationCurve( this );
   new XS( TheSaturationCurveParameter );
   new YS( TheSaturationCurveParameter );
   new SaturationCurveType( this );
}

// ----------------------------------------------------------------------------

IsoString CurvesTransformationProcess::Id() const
{
   return "CurvesTransformation";
}

// ----------------------------------------------------------------------------

IsoString CurvesTransformationProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 CurvesTransformationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CurvesTransformationProcess::Description() const
{
   return

   "<html>"
   "<p>The CurvesTransformation process defines a set of <i>transfer curves</i> that can be applied to selected channels and/or "
   "color components of images. A transfer curve in PixInsight is an interpolated function applied to replace existing "
   "pixel values.</p>"

   "<p>Each transfer curve is defined by a set of arbitrary <i>interpolation points</i>. For each interpolation point, an "
   "<i>input value</i> and an <i>output value</i> are defined in the normalized [0,1] range. When the transfer curve is "
   "applied to each pixel of an image, the current pixel value is used as input to interpolate a new value from the set "
   "of interpolation points, and the interpolated output value replaces the original.</p>"

   "<p>CurvesTransformation implements a set of eleven independent transfer curves that can be used to process RGB color images:</p>"

   "<ul>"
   "<li><b>Individual red, green, and blue channel curves</b>. Each of these curves are applied to their counterpart channels of "
   "RGB color images<br/></li>"

   "<li><b>Combined RGB/gray channel curve</b>. This curve is applied to the red, green and blue channels of RGB color images, and "
   "to the unique gray channel of grayscale images.<br/></li>"

   "<li><b>Alpha channel curve</b>. This curve is applied to the first alpha channel, which defines pixel transparency in PixInsight.<br/></li>"

   "<li><b>Lightness curve</b>, applied to the L* component of the CIE L*a*b* space.<br/></li>"

   "<li><b>a* and b* curves</b>, applied to their corresponding components in the CIE L*a*b* space. The a* component is the green/red "
   "balance, and the b* component is the blue/yellow balance. By modifying these curves, very accurate and selective color "
   "transforms can be applied. Since the a* and b* components express relations between complementary colors, these curves "
   "can also be used to implement selective color saturation adjustments.<br/></li>"

   "<li><b>c* curve</b>, applied to the c* component of the CIE L*c*h* color space. The c* component stands for <i>colorfulness</i>. "
   "This curve allows for very accurate and adaptive color saturation transforms in a CIE color space.<br/></li>"

   "<li><b>Hue curve</b> in the HSV color ordering system.<br/></li>"

   "<li><b>Saturation (S) curve</b> in a special HSVL* space. For each pixel, two color space transforms are carried out: one to the HSV "
   "color system and another to the CIE L*a*b* color space. The S channel of HSV is interpolated from the curve, and the reverse "
   "transform HSV->RGB is performed. The resulting RGB pixel is again transformed to the CIE L*a*b* space, the new lightness is "
   "discarded and replaced with the original value, and a final reverse CIE L*a*b* -> RGB transform is done. This effectively "
   "applies a saturation transform without altering the luminance of the image, which prevents transferring noise from the "
   "chrominance to the luminance.<br/></li>"
   "</ul>"

   "<p>For grayscale images, only the combined RGB/gray and alpha curves (when applicable) are used.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** CurvesTransformationProcess::IconImageXPM() const
{
   return CurvesTransformationIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* CurvesTransformationProcess::DefaultInterface() const
{
   return TheCurvesTransformationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* CurvesTransformationProcess::Create() const
{
   return new CurvesTransformationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* CurvesTransformationProcess::Clone( const ProcessImplementation& p ) const
{
   const CurvesTransformationInstance* instPtr = dynamic_cast<const CurvesTransformationInstance*>( &p );
   return (instPtr != nullptr) ? new CurvesTransformationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool CurvesTransformationProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CurvesTransformationProcess.cpp - Released 2019-01-21T12:06:41Z
