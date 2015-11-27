//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 01.00.00.0155
// ----------------------------------------------------------------------------
// AssistedColorCalibrationProcess.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2015 Zbynek Vrastil
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "AssistedColorCalibrationProcess.h"
#include "AssistedColorCalibrationParameters.h"
#include "AssistedColorCalibrationInstance.h"
#include "AssistedColorCalibrationInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "AssistedColorCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

AssistedColorCalibrationProcess* TheAssistedColorCalibrationProcess = 0;

// ----------------------------------------------------------------------------

AssistedColorCalibrationProcess::AssistedColorCalibrationProcess() : MetaProcess()
{
   TheAssistedColorCalibrationProcess = this;

   // Instantiate process parameters
   new ACCRedCorrectionFactor( this );
   new ACCGreenCorrectionFactor( this );
   new ACCBlueCorrectionFactor( this );
   new ACCBackgroundReference( this );
   new ACCHistogramShadows( this );
   new ACCHistogramHighlights( this );
   new ACCHistogramMidtones( this );
   new ACCSaturationBoost( this );
}

// ----------------------------------------------------------------------------

IsoString AssistedColorCalibrationProcess::Id() const
{
   return "AssistedColorCalibration";
}

// ----------------------------------------------------------------------------

IsoString AssistedColorCalibrationProcess::Category() const
{
   return "ColorCalibration";
}

// ----------------------------------------------------------------------------

uint32 AssistedColorCalibrationProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationProcess::Description() const
{
   return

   "<html>"
   "<p>AssistedColorCalibration process is used to do <b>manual</b> color calibration (white balance) on the image."
   "The color calibration should be done before further processing, even before the background (sky glow, light pollution) is subtracted. "
   "This makes manual color calibration difficult, because it is not easy to evaluate the effect on not-yet-processed image.</p>"
   "<p>This process helps you to find out (or just check) the correct white balance factors by computing an approximation of final processed image. "
   "It does it by applying selected white balance coefficents, subtracting provided background reference (also calibrated), applying histogram "
   "transform and color saturation enhancement. Although this is definitely very simple processing, it should give you an image reasonably close "
   "to final result - at least good enough to evaluate current white balance coefficents.</p>"
   "<p>Please note that these further steps (background subtraction, histogram transform, saturation enhancement) are only applied to previews. Only white "
   "balance cofficients are applied to the regular image. This process is only designed to do the white balance - not to substitute the post-processing.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** AssistedColorCalibrationProcess::IconImageXPM() const
{
   return AssistedColorCalibrationIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* AssistedColorCalibrationProcess::DefaultInterface() const
{
   return TheAssistedColorCalibrationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* AssistedColorCalibrationProcess::Create() const
{
   return new AssistedColorCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AssistedColorCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const AssistedColorCalibrationInstance* instPtr = dynamic_cast<const AssistedColorCalibrationInstance*>( &p );
   return (instPtr != 0) ? new AssistedColorCalibrationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool AssistedColorCalibrationProcess::CanProcessCommandLines() const
{
   return false;
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationProcess.cpp - Released 2015/11/26 16:00:13 UTC
