//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 01.00.05.0321
// ----------------------------------------------------------------------------
// RestorationFilterProcess.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
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

#include "RestorationFilterInstance.h"
#include "RestorationFilterInterface.h"
#include "RestorationFilterParameters.h"
#include "RestorationFilterProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RestorationFilterProcess* TheRestorationFilterProcess = 0;

// ----------------------------------------------------------------------------

#include "RestorationFilterIcon.xpm"

// ----------------------------------------------------------------------------

RestorationFilterProcess::RestorationFilterProcess() : MetaProcess()
{
   TheRestorationFilterProcess = this;

   // Instantiate process parameters
   new RFWienerK( this );
   new RFLeastSquaresGamma( this );
   new RFAlgorithm( this );
   new RFPSFMode( this );
   new RFPSFParametricSigma( this );
   new RFPSFParametricShape( this );
   new RFPSFParametricAspectRatio( this );
   new RFPSFParametricRotationAngle( this );
   new RFPSFMotionLength( this );
   new RFPSFMotionRotationAngle( this );
   new RFPSFViewId( this );
   new RFAmount( this );
   new RFDeringing( this );
   new RFDeringingDark( this );
   new RFDeringingBright( this );
   new RFOutputDeringingMaps( this );
   new RFToLuminance( this );
   new RFLinear( this );
   new RFRangeLow( this );
   new RFRangeHigh( this );
}

IsoString RestorationFilterProcess::Id() const
{
   return "RestorationFilter";
}

IsoString RestorationFilterProcess::Category() const
{
   return "Deconvolution";
}

uint32 RestorationFilterProcess::Version() const
{
   return 0x100;
}

String RestorationFilterProcess::Description() const
{
   return

   "<html>"
   "<p>RestorationFilter implements some non-iterative image deconvolution algorithms in the frequency domain.</p>"

   "<p>The two algorithms currently available are Wiener filtering (or Minimum Mean Square Error filtering) and "
   "Constrained Least Squares filtering.</p>"
   "</html>";
}

const char** RestorationFilterProcess::IconImageXPM() const
{
   return RestorationFilterIcon_XPM;
}

ProcessInterface* RestorationFilterProcess::DefaultInterface() const
{
   return TheRestorationFilterInterface;
}

ProcessImplementation* RestorationFilterProcess::Create() const
{
   return new RestorationFilterInstance( this );
}

ProcessImplementation* RestorationFilterProcess::Clone( const ProcessImplementation& p ) const
{
   const RestorationFilterInstance* instPtr = dynamic_cast<const RestorationFilterInstance*>( &p );
   return (instPtr != 0) ? new RestorationFilterInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RestorationFilterProcess.cpp - Released 2017-08-01T14:26:58Z
