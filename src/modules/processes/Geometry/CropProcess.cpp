//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0377
// ----------------------------------------------------------------------------
// CropProcess.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "CropProcess.h"
#include "CropParameters.h"
#include "CropInstance.h"
#include "CropInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CropProcess* TheCropProcess = nullptr;

// ----------------------------------------------------------------------------

#include "CropIcon.xpm"

// ----------------------------------------------------------------------------

CropProcess::CropProcess() : MetaProcess()
{
   TheCropProcess = this;

   new CRLeftMargin( this );
   new CRTopMargin( this );
   new CRRightMargin( this );
   new CRBottomMargin( this );
   TheCRModeParameter = new CRMode( this );
   TheCRXResolutionParameter = new XResolution( this );
   TheCRYResolutionParameter = new YResolution( this );
   TheCRMetricResolutionParameter = new MetricResolution( this );
   TheCRForceResolutionParameter = new ForceResolution( this );
   TheCRFillRedParameter = new FillRed( this );
   TheCRFillGreenParameter = new FillGreen( this );
   TheCRFillBlueParameter = new FillBlue( this );
   TheCRFillAlphaParameter = new FillAlpha( this );
   TheCRNoGUIMessagesParameter = new NoGUIMessages( this );
}

IsoString CropProcess::Id() const
{
   return "Crop";
}

IsoString CropProcess::Category() const
{
   return "Geometry";
}

uint32 CropProcess::Version() const
{
   return 0x100;
}

String CropProcess::Description() const
{
   return "";
}

const char** CropProcess::IconImageXPM() const
{
   return CropIcon_XPM;
}

ProcessInterface* CropProcess::DefaultInterface() const
{
   return TheCropInterface;
}

ProcessImplementation* CropProcess::Create() const
{
   return new CropInstance( this );
}

ProcessImplementation* CropProcess::Clone( const ProcessImplementation& p ) const
{
   const CropInstance* instPtr = dynamic_cast<const CropInstance*>( &p );
   return (instPtr != nullptr) ? new CropInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropProcess.cpp - Released 2017-08-01T14:26:58Z
