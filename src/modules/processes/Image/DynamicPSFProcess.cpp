//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0412
// ----------------------------------------------------------------------------
// DynamicPSFProcess.cpp - Released 2018-11-13T16:55:32Z
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

#include "DynamicPSFProcess.h"
#include "DynamicPSFParameters.h"
#include "DynamicPSFInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DynamicPSFProcess* TheDynamicPSFProcess = nullptr;

// ----------------------------------------------------------------------------

#include "DynamicPSFIcon.xpm"

// ----------------------------------------------------------------------------

DynamicPSFProcess::DynamicPSFProcess()
{
   TheDynamicPSFProcess = this;

   new DPViewTable( this );
   new DPViewId( TheDPViewTableParameter );

   new DPStarTable( this );
   new DPStarViewIndex( TheDPStarTableParameter );
   new DPStarChannel( TheDPStarTableParameter );
   new DPStarStatus( TheDPStarTableParameter );
   new DPStarRectX0( TheDPStarTableParameter );
   new DPStarRectY0( TheDPStarTableParameter );
   new DPStarRectX1( TheDPStarTableParameter );
   new DPStarRectY1( TheDPStarTableParameter );
   new DPStarPosX( TheDPStarTableParameter );
   new DPStarPosY( TheDPStarTableParameter );

   new DPPSFTable( this );
   new DPPSFStarIndex( TheDPPSFTableParameter );
   new DPPSFFunction( TheDPPSFTableParameter );
   new DPPSFCircular( TheDPPSFTableParameter );
   new DPPSFStatus( TheDPPSFTableParameter );
   new DPPSFBackground( TheDPPSFTableParameter );
   new DPPSFAmplitude( TheDPPSFTableParameter );
   new DPPSFCentroidX( TheDPPSFTableParameter );
   new DPPSFCentroidY( TheDPPSFTableParameter );
   new DPPSFRadiusX( TheDPPSFTableParameter );
   new DPPSFRadiusY( TheDPPSFTableParameter );
   new DPPSFRotationAngle( TheDPPSFTableParameter );
   new DPPSFBeta( TheDPPSFTableParameter );
   new DPPSFMAD( TheDPPSFTableParameter );
   new DPPSFCelestial( TheDPPSFTableParameter );
   new DPPSFCentroidRA( TheDPPSFTableParameter );
   new DPPSFCentroidDec( TheDPPSFTableParameter );

   new DPAutoPSF( this );
   new DPCircularPSF( this );
   new DPGaussianPSF( this );
   new DPMoffatPSF( this );
   new DPMoffat10PSF( this );
   new DPMoffat8PSF( this );
   new DPMoffat6PSF( this );
   new DPMoffat4PSF( this );
   new DPMoffat25PSF( this );
   new DPMoffat15PSF( this );
   new DPLorentzianPSF( this );

   new DPSignedAngles( this );
   new DPRegenerate( this );
   new DPAstrometry( this );

   new DPSearchRadius( this );
   new DPThreshold( this );
   new DPAutoAperture( this );
   new DPScaleMode( this );
   new DPScaleValue( this );
   new DPScaleKeyword( this );

   new DPStarColor( this );
   new DPSelectedStarColor( this );
   new DPSelectedStarFillColor( this );
   new DPBadStarColor( this );
   new DPBadStarFillColor( this );
}

// ----------------------------------------------------------------------------

IsoString DynamicPSFProcess::Id() const
{
   return "DynamicPSF";
}

// ----------------------------------------------------------------------------

IsoString DynamicPSFProcess::Category() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

uint32 DynamicPSFProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String DynamicPSFProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** DynamicPSFProcess::IconImageXPM() const
{
   return DynamicPSFIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* DynamicPSFProcess::DefaultInterface() const
{
   return TheDynamicPSFInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* DynamicPSFProcess::Create() const
{
   return new DynamicPSFInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* DynamicPSFProcess::Clone( const ProcessImplementation& p ) const
{
   const DynamicPSFInstance* instance = dynamic_cast<const DynamicPSFInstance*>( &p );
   return (instance != nullptr) ? new DynamicPSFInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicPSFProcess.cpp - Released 2018-11-13T16:55:32Z
