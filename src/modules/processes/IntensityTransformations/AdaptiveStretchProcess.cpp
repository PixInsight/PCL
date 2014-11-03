// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard IntensityTransformations Process Module Version 01.07.00.0285
// ****************************************************************************
// AdaptiveStretchProcess.cpp - Released 2014/10/29 07:35:24 UTC
// ****************************************************************************
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "AdaptiveStretchProcess.h"
#include "AdaptiveStretchParameters.h"
#include "AdaptiveStretchInstance.h"
#include "AdaptiveStretchInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#include "AdaptiveStretchIcon.xpm"

// ----------------------------------------------------------------------------

AdaptiveStretchProcess* TheAdaptiveStretchProcess = 0;

// ----------------------------------------------------------------------------

AdaptiveStretchProcess::AdaptiveStretchProcess()
{
   TheAdaptiveStretchProcess = this;

   new ASNoiseThreshold( this );
   new ASProtection( this );
   new ASUseProtection( this );
   new ASMaxCurvePoints( this );
   new ASUseROI( this );
   new ASROIX0( this );
   new ASROIY0( this );
   new ASROIX1( this );
   new ASROIY1( this );
}

IsoString AdaptiveStretchProcess::Id() const
{
   return "AdaptiveStretch";
}

IsoString AdaptiveStretchProcess::Category() const
{
   return "IntensityTransformations";
}

uint32 AdaptiveStretchProcess::Version() const
{
   return 0x100;
}

String AdaptiveStretchProcess::Description() const
{
   return "";
}

const char** AdaptiveStretchProcess::IconImageXPM() const
{
   return AdaptiveStretchIcon_XPM;
}

ProcessInterface* AdaptiveStretchProcess::DefaultInterface() const
{
   return TheAdaptiveStretchInterface;
}

ProcessImplementation* AdaptiveStretchProcess::Create() const
{
   return new AdaptiveStretchInstance( this );
}

ProcessImplementation* AdaptiveStretchProcess::Clone( const ProcessImplementation& p ) const
{
   const AdaptiveStretchInstance* instPtr = dynamic_cast<const AdaptiveStretchInstance*>( &p );
   return (instPtr != 0) ? new AdaptiveStretchInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AdaptiveStretchProcess.cpp - Released 2014/10/29 07:35:24 UTC
