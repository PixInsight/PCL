//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0364
// ----------------------------------------------------------------------------
// HistogramTransformationProcess.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "HistogramTransformationProcess.h"
#include "HistogramTransformationParameters.h"
#include "HistogramTransformationInstance.h"
#include "HistogramTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramTransformationProcess* TheHistogramTransformationProcess = 0;

// ----------------------------------------------------------------------------

#include "HistogramTransformationIcon.xpm"

// ----------------------------------------------------------------------------

HistogramTransformationProcess::HistogramTransformationProcess() : MetaProcess()
{
   TheHistogramTransformationProcess = this;

   // Instantiate process parameters
   new HistogramTransformationTable( this );
   new ShadowsClipping( TheHistogramTransformationTableParameter );
   new MidtonesBalance( TheHistogramTransformationTableParameter );
   new HighlightsClipping( TheHistogramTransformationTableParameter );
   new LowRange( TheHistogramTransformationTableParameter );
   new HighRange( TheHistogramTransformationTableParameter );
}

// ----------------------------------------------------------------------------

IsoString HistogramTransformationProcess::Id() const
{
   return "HistogramTransformation";
}

// ----------------------------------------------------------------------------

IsoString HistogramTransformationProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 HistogramTransformationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String HistogramTransformationProcess::Description() const
{
   return

   "<html>"
   "<p>HistogramTransformation implements a direct pixel manipulation procedure defined by a set of specific parameters. HistogramTransformation "
   "parameters are real values in the normalized range [0,1]:</p>"

   "<ul>"
   "<li><b>Shadows and Highlights Clipping Bounds</b>. The clipping parameters define a relative reduction of the dynamic range usage "
   "of the image. Any pixel component below the shadows clipping bound will be assigned a zero (black) value, while pixel "
   "components above the highlights clipping bound will be set to one (white). The remaining pixel component values, between both "
   "clipping bounds, will be proportionally rescaled to extend over the full normalized dynamic range.<br/></li>"

   "<li><b>Midtones Balance</b>. The midtones balance parameter can take values in the entire normalized dynamic range (from 0 to 1). "
   "It defines a <i>midtones transfer function</i> (MTF) that is applied to every pixel component after clipping. A midtones balance "
   "value of 0.5 defines a linear, <i>identity</i> function (i.e., no change occurs). Values below 0.5 increase midtones, while values "
   "above 0.5 darken midtones in the image. MTF curves are implemented as rational interpolation functions in the PixInsight/PCL "
   "framework. A common misunderstanding is to assume that MTFs are gamma curves in PixInsight. Actually, rational interpolation "
   "allows for much more flexible, accurate and controllable MTF curves in PixInsight than gamma curves are in other applications.<br/></li>"

   "<li><b>Dynamic Range Expansion Bounds</b>. The <i>dynamic range expansion lower bound</i> parameter of HistogramTransformation can vary "
   "from -10 to 0, while the <i>dynamic range expansion upper bound</i> ranges from 1 to +11. These two parameters actually allow "
   "expansion of the <i>unused</i> dynamic range at both ends of the histogram. This can be probably better understood as a two-step "
   "procedure: first, the dynamic range is expanded to occupy the entire interval defined by the lower and upper bound expansion "
   "parameters, but actual pixel values are not changed. The second step is to rescale both the dynamic range and all pixel values "
   "back to the normalized [0,1] range. The result of this process is that all pixel data are constrained to a smaller effective "
   "interval, and free unused portions appear at the corresponding histogram ends. This is used as a previous step for some image "
   "processing techniques, with the purpose of preserving actual pixel data from losses due to excessive contrast gains.<br/></li>"
   "</ul>"

   "<p>Histograms are internally generated with 16-bit resolution (65536 levels) in PixInsight for all supported pixel data formats. "
   "However, histogram functions can be represented and inspected with interpolated resolutions varying in the range from 6-bit "
   "(64 levels) to the full 16-bit precision, including the standard 8-bit (256 levels), 10-bit (1024 levels), and 12-bit (4096) "
   "binary resolutions, among many others. This allows for the precise adaptation of the histogram graphics and manipulation tools "
   "to the specific contents of each target image.</p>"

   "<p>For grayscale images, a unique set of parameters is used (RGB/K set). For RGB color images, an independent set of parameters is "
   "applicable to each individual color component, plus an additional set that can be applied equally to the three RGB components "
   "(RGB/K set). In addition, HistogramTransfrom provides a set of parameters specific to the first alpha channel, which defines "
   "pixel transparency in PixInsight.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** HistogramTransformationProcess::IconImageXPM() const
{
   return HistogramTransformationIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* HistogramTransformationProcess::DefaultInterface() const
{
   return TheHistogramTransformationInterface;
}

// -------------------------------------------------------------------------

bool HistogramTransformationProcess::CanProcessImages() const
{
   return true;
}

// -------------------------------------------------------------------------

ProcessImplementation* HistogramTransformationProcess::Create() const
{
   return new HistogramTransformationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* HistogramTransformationProcess::Clone( const ProcessImplementation& p ) const
{
   const HistogramTransformationInstance* instPtr = dynamic_cast<const HistogramTransformationInstance*>( &p );
   return (instPtr != 0) ? new HistogramTransformationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HistogramTransformationProcess.cpp - Released 2017-04-14T23:07:12Z
