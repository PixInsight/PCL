//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0405
// ----------------------------------------------------------------------------
// ColorSaturationProcess.cpp - Released 2017-08-01T14:26:58Z
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

#include "ColorSaturationProcess.h"
#include "ColorSaturationParameters.h"
#include "ColorSaturationInstance.h"
#include "ColorSaturationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSaturationProcess* TheColorSaturationProcess = 0;

// ----------------------------------------------------------------------------

#include "ColorSaturationIcon.xpm"

// ----------------------------------------------------------------------------

ColorSaturationProcess::ColorSaturationProcess() : MetaProcess()
{
   TheColorSaturationProcess = this;

   // Instantiate process parameters

   new HueSaturationCurve( this );
   new XHS( TheHueSaturationCurveParameter );
   new YHS( TheHueSaturationCurveParameter );
   new HueSaturationCurveType( this );

   new HueShift( this );
}

// ----------------------------------------------------------------------------

IsoString ColorSaturationProcess::Id() const
{
   return "ColorSaturation";
}

// ----------------------------------------------------------------------------

IsoString ColorSaturationProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 ColorSaturationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ColorSaturationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** ColorSaturationProcess::IconImageXPM() const
{
   return ColorSaturationIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* ColorSaturationProcess::DefaultInterface() const
{
   return TheColorSaturationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ColorSaturationProcess::Create() const
{
   return new ColorSaturationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ColorSaturationProcess::Clone( const ProcessImplementation& p ) const
{
   const ColorSaturationInstance* instPtr = dynamic_cast<const ColorSaturationInstance*>( &p );
   return (instPtr != 0) ? new ColorSaturationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorSaturationProcess.cpp - Released 2017-08-01T14:26:58Z
