//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0437
// ----------------------------------------------------------------------------
// DynamicPSFInstance.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "DynamicPSFInstance.h"
#include "DynamicPSFInterface.h"

#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

DynamicPSFInstance::DynamicPSFInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_signedAngles( TheDPSignedAnglesParameter->DefaultValue() ),
   p_regenerate( TheDPRegenerateParameter->DefaultValue() ),
   p_astrometry( TheDPAstrometryParameter->DefaultValue() ),
   p_searchRadius( TheDPSearchRadiusParameter->DefaultValue() ),
   p_threshold( TheDPThresholdParameter->DefaultValue() ),
   p_autoAperture( TheDPAutoApertureParameter->DefaultValue() ),
   p_scaleMode( DPScaleMode::Default ),
   p_scaleValue( TheDPScaleValueParameter->DefaultValue() ),
   p_starColor( TheDPStarColorParameter->DefaultValue() ),
   p_selectedStarColor( TheDPSelectedStarColorParameter->DefaultValue() ),
   p_selectedStarFillColor( TheDPSelectedStarFillColorParameter->DefaultValue() ),
   p_badStarColor( TheDPBadStarColorParameter->DefaultValue() ),
   p_badStarFillColor( TheDPBadStarFillColorParameter->DefaultValue() )
{
   p_psfOptions.autoPSF = TheDPAutoPSFParameter->DefaultValue();
   p_psfOptions.circular = TheDPCircularPSFParameter->DefaultValue();
   p_psfOptions.gaussian = TheDPGaussianPSFParameter->DefaultValue();
   p_psfOptions.moffat = TheDPMoffatPSFParameter->DefaultValue();
   p_psfOptions.moffatA = TheDPMoffat10PSFParameter->DefaultValue();
   p_psfOptions.moffat8 = TheDPMoffat8PSFParameter->DefaultValue();
   p_psfOptions.moffat6 = TheDPMoffat6PSFParameter->DefaultValue();
   p_psfOptions.moffat4 = TheDPMoffat4PSFParameter->DefaultValue();
   p_psfOptions.moffat25 = TheDPMoffat25PSFParameter->DefaultValue();
   p_psfOptions.moffat15 = TheDPMoffat15PSFParameter->DefaultValue();
   p_psfOptions.lorentzian = TheDPLorentzianPSFParameter->DefaultValue();
}

// ----------------------------------------------------------------------------

DynamicPSFInstance::DynamicPSFInstance( const DynamicPSFInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

DynamicPSFInstance::~DynamicPSFInstance()
{
}

// ----------------------------------------------------------------------------

void DynamicPSFInstance::Assign( const ProcessImplementation& p )
{
   const DynamicPSFInstance* x = dynamic_cast<const DynamicPSFInstance*>( &p );
   if ( x != nullptr )
      if ( x != this )
      {
         p_views = x->p_views;
         p_stars = x->p_stars;
         p_psfs = x->p_psfs;
         p_psfOptions.autoPSF = x->p_psfOptions.autoPSF;
         p_psfOptions.circular = x->p_psfOptions.circular;
         p_psfOptions.gaussian = x->p_psfOptions.gaussian;
         p_psfOptions.moffat = x->p_psfOptions.moffat;
         p_psfOptions.moffatA = x->p_psfOptions.moffatA;
         p_psfOptions.moffat8 = x->p_psfOptions.moffat8;
         p_psfOptions.moffat6 = x->p_psfOptions.moffat6;
         p_psfOptions.moffat4 = x->p_psfOptions.moffat4;
         p_psfOptions.moffat25 = x->p_psfOptions.moffat25;
         p_psfOptions.moffat15 = x->p_psfOptions.moffat15;
         p_psfOptions.lorentzian = x->p_psfOptions.lorentzian;
         p_signedAngles = x->p_signedAngles;
         p_regenerate = x->p_regenerate;
         p_astrometry = x->p_astrometry;
         p_searchRadius = x->p_searchRadius;
         p_threshold = x->p_threshold;
         p_autoAperture = x->p_autoAperture;
         p_scaleMode = x->p_scaleMode;
         p_scaleValue = x->p_scaleValue;
         p_scaleKeyword = x->p_scaleKeyword;
         p_starColor = x->p_starColor;
         p_selectedStarColor = x->p_selectedStarColor;
         p_selectedStarFillColor = x->p_selectedStarFillColor;
         p_badStarColor = x->p_badStarColor;
         p_badStarFillColor = x->p_badStarFillColor;
      }
}

// ----------------------------------------------------------------------------

void DynamicPSFInstance::AssignOptions( const DynamicPSFInstance& x )
{
   p_views.Clear();
   p_stars.Clear();
   p_psfs.Clear();
   p_psfOptions.autoPSF = x.p_psfOptions.autoPSF;
   p_psfOptions.circular = x.p_psfOptions.circular;
   p_psfOptions.gaussian = x.p_psfOptions.gaussian;
   p_psfOptions.moffat = x.p_psfOptions.moffat;
   p_psfOptions.moffatA = x.p_psfOptions.moffatA;
   p_psfOptions.moffat8 = x.p_psfOptions.moffat8;
   p_psfOptions.moffat6 = x.p_psfOptions.moffat6;
   p_psfOptions.moffat4 = x.p_psfOptions.moffat4;
   p_psfOptions.moffat25 = x.p_psfOptions.moffat25;
   p_psfOptions.moffat15 = x.p_psfOptions.moffat15;
   p_psfOptions.lorentzian = x.p_psfOptions.lorentzian;
   p_signedAngles = x.p_signedAngles;
   p_regenerate = x.p_regenerate;
   p_astrometry = x.p_astrometry;
   p_searchRadius = x.p_searchRadius;
   p_threshold = x.p_threshold;
   p_autoAperture = x.p_autoAperture;
   p_scaleMode = x.p_scaleMode;
   p_scaleValue = x.p_scaleValue;
   p_scaleKeyword = x.p_scaleKeyword;
   p_starColor = x.p_starColor;
   p_selectedStarColor = x.p_selectedStarColor;
   p_selectedStarFillColor = x.p_selectedStarFillColor;
   p_badStarColor = x.p_badStarColor;
   p_badStarFillColor = x.p_badStarFillColor;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "DynamicPSF can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInstance::ExecuteGlobal()
{
   DynamicPSFInterface::ExecuteInstance( *this );
   Console console;
   console.WriteLn( "<end><cbr>" + String( p_views.Length() ) + " view(s)" );
   console.WriteLn( String( p_stars.Length() ) + " star(s)" );
   console.WriteLn( String( p_psfs.Length() ) + " PSF fittings" );
   return true;
}

// ----------------------------------------------------------------------------

void* DynamicPSFInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDPViewIdParameter )
      return p_views[tableRow].Begin();

   if ( p == TheDPStarViewIndexParameter )
      return &p_stars[tableRow].view;
   if ( p == TheDPStarChannelParameter )
      return &p_stars[tableRow].channel;
   if ( p == TheDPStarStatusParameter )
      return &p_stars[tableRow].status;
   if ( p == TheDPStarRectX0Parameter )
      return &p_stars[tableRow].rect.x0;
   if ( p == TheDPStarRectY0Parameter )
      return &p_stars[tableRow].rect.y0;
   if ( p == TheDPStarRectX1Parameter )
      return &p_stars[tableRow].rect.x1;
   if ( p == TheDPStarRectY1Parameter )
      return &p_stars[tableRow].rect.y1;
   if ( p == TheDPStarPosXParameter )
      return &p_stars[tableRow].pos.x;
   if ( p == TheDPStarPosYParameter )
      return &p_stars[tableRow].pos.y;

   if ( p == TheDPPSFStarIndexParameter )
      return &p_psfs[tableRow].star;
   if ( p == TheDPPSFFunctionParameter )
      return &p_psfs[tableRow].function;
   if ( p == TheDPPSFCircularParameter )
      return &p_psfs[tableRow].circular;
   if ( p == TheDPPSFStatusParameter )
      return &p_psfs[tableRow].status;
   if ( p == TheDPPSFBackgroundParameter )
      return &p_psfs[tableRow].B;
   if ( p == TheDPPSFAmplitudeParameter )
      return &p_psfs[tableRow].A;
   if ( p == TheDPPSFCentroidXParameter )
      return &p_psfs[tableRow].c0.x;
   if ( p == TheDPPSFCentroidYParameter )
      return &p_psfs[tableRow].c0.y;
   if ( p == TheDPPSFRadiusXParameter )
      return &p_psfs[tableRow].sx;
   if ( p == TheDPPSFRadiusYParameter )
      return &p_psfs[tableRow].sy;
   if ( p == TheDPPSFRotationAngleParameter )
      return &p_psfs[tableRow].theta;
   if ( p == TheDPPSFBetaParameter )
      return &p_psfs[tableRow].beta;
   if ( p == TheDPPSFMADParameter )
      return &p_psfs[tableRow].mad;
   if ( p == TheDPPSFCelestialParameter )
      return &p_psfs[tableRow].celestial;
   if ( p == TheDPPSFCentroidRAParameter )
      return &p_psfs[tableRow].q0.x;
   if ( p == TheDPPSFCentroidDecParameter )
      return &p_psfs[tableRow].q0.y;

   if ( p == TheDPAutoPSFParameter )
      return &p_psfOptions.autoPSF;
   if ( p == TheDPCircularPSFParameter )
      return &p_psfOptions.circular;
   if ( p == TheDPGaussianPSFParameter )
      return &p_psfOptions.gaussian;
   if ( p == TheDPMoffatPSFParameter )
      return &p_psfOptions.moffat;
   if ( p == TheDPMoffat10PSFParameter )
      return &p_psfOptions.moffatA;
   if ( p == TheDPMoffat8PSFParameter )
      return &p_psfOptions.moffat8;
   if ( p == TheDPMoffat6PSFParameter )
      return &p_psfOptions.moffat6;
   if ( p == TheDPMoffat4PSFParameter )
      return &p_psfOptions.moffat4;
   if ( p == TheDPMoffat25PSFParameter )
      return &p_psfOptions.moffat25;
   if ( p == TheDPMoffat15PSFParameter )
      return &p_psfOptions.moffat15;
   if ( p == TheDPLorentzianPSFParameter )
      return &p_psfOptions.lorentzian;

   if ( p == TheDPSignedAnglesParameter )
      return &p_signedAngles;
   if ( p == TheDPRegenerateParameter )
      return &p_regenerate;
   if ( p == TheDPAstrometryParameter )
      return &p_astrometry;

   if ( p == TheDPSearchRadiusParameter )
      return &p_searchRadius;
   if ( p == TheDPThresholdParameter )
      return &p_threshold;
   if ( p == TheDPAutoApertureParameter )
      return &p_autoAperture;
   if ( p == TheDPScaleModeParameter )
      return &p_scaleMode;
   if ( p == TheDPScaleValueParameter )
      return &p_scaleValue;
   if ( p == TheDPScaleKeywordParameter )
      return p_scaleKeyword.Begin();

   if ( p == TheDPStarColorParameter )
      return &p_starColor;
   if ( p == TheDPSelectedStarColorParameter )
      return &p_selectedStarColor;
   if ( p == TheDPSelectedStarFillColorParameter )
      return &p_selectedStarFillColor;
   if ( p == TheDPBadStarColorParameter )
      return &p_badStarColor;
   if ( p == TheDPBadStarFillColorParameter )
      return &p_badStarFillColor;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDPViewTableParameter )
   {
      p_views.Clear();
      if ( sizeOrLength > 0 )
         p_views = StringList( sizeOrLength );
   }
   else if ( p == TheDPViewIdParameter )
   {
      p_views[tableRow].Clear();
      if ( sizeOrLength > 0 )
         p_views[tableRow].SetLength( sizeOrLength );
   }
   else if ( p == TheDPStarTableParameter )
   {
      p_stars.Clear();
      if ( sizeOrLength > 0 )
         p_stars = Array<Star>( sizeOrLength );
   }
   else if ( p == TheDPPSFTableParameter )
   {
      p_psfs.Clear();
      if ( sizeOrLength > 0 )
         p_psfs = Array<PSF>( sizeOrLength );
   }
   else if ( p == TheDPScaleKeywordParameter )
   {
      p_scaleKeyword.Clear();
      if ( sizeOrLength > 0 )
         p_scaleKeyword.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type DynamicPSFInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheDPViewTableParameter )
      return p_views.Length();
   if ( p == TheDPViewIdParameter )
      return p_views[tableRow].Length();
   if ( p == TheDPStarTableParameter )
      return p_stars.Length();
   if ( p == TheDPPSFTableParameter )
      return p_psfs.Length();
   if ( p == TheDPScaleKeywordParameter )
      return p_scaleKeyword.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicPSFInstance.cpp - Released 2019-01-21T12:06:41Z
