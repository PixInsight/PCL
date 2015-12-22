//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0348
// ----------------------------------------------------------------------------
// DynamicPSFInstance.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

DynamicPSFInstance::DynamicPSFInstance( const MetaProcess* P ) : ProcessImplementation( P ),
views(),
stars(),
psfs(),
signedAngles( TheDPSignedAnglesParameter->DefaultValue() ),
regenerate( TheDPRegenerateParameter->DefaultValue() ),
searchRadius( TheDPSearchRadiusParameter->DefaultValue() ),
threshold( TheDPThresholdParameter->DefaultValue() ),
autoAperture( TheDPAutoApertureParameter->DefaultValue() ),
scaleMode( DPScaleMode::Default ),
scaleValue( TheDPScaleValueParameter->DefaultValue() ),
scaleKeyword(),
starColor( TheDPStarColorParameter->DefaultValue() ),
selectedStarColor( TheDPSelectedStarColorParameter->DefaultValue() ),
selectedStarFillColor( TheDPSelectedStarFillColorParameter->DefaultValue() ),
badStarColor( TheDPBadStarColorParameter->DefaultValue() ),
badStarFillColor( TheDPBadStarFillColorParameter->DefaultValue() )
{
   psfOptions.autoPSF = TheDPAutoPSFParameter->DefaultValue();
   psfOptions.circular = TheDPCircularPSFParameter->DefaultValue();
   psfOptions.gaussian = TheDPGaussianPSFParameter->DefaultValue();
   psfOptions.moffat = TheDPMoffatPSFParameter->DefaultValue();
   psfOptions.moffatA = TheDPMoffat10PSFParameter->DefaultValue();
   psfOptions.moffat8 = TheDPMoffat8PSFParameter->DefaultValue();
   psfOptions.moffat6 = TheDPMoffat6PSFParameter->DefaultValue();
   psfOptions.moffat4 = TheDPMoffat4PSFParameter->DefaultValue();
   psfOptions.moffat25 = TheDPMoffat25PSFParameter->DefaultValue();
   psfOptions.moffat15 = TheDPMoffat15PSFParameter->DefaultValue();
   psfOptions.lorentzian = TheDPLorentzianPSFParameter->DefaultValue();
}

DynamicPSFInstance::DynamicPSFInstance( const DynamicPSFInstance& x ) : ProcessImplementation( x )
{
   Assign( x );
}

DynamicPSFInstance::~DynamicPSFInstance()
{
}

void DynamicPSFInstance::Assign( const ProcessImplementation& p )
{
   const DynamicPSFInstance* x = dynamic_cast<const DynamicPSFInstance*>( &p );
   if ( x != 0 && x != this )
   {
      views = x->views;
      stars = x->stars;
      psfs = x->psfs;
      psfOptions.autoPSF = x->psfOptions.autoPSF;
      psfOptions.circular = x->psfOptions.circular;
      psfOptions.gaussian = x->psfOptions.gaussian;
      psfOptions.moffat = x->psfOptions.moffat;
      psfOptions.moffatA = x->psfOptions.moffatA;
      psfOptions.moffat8 = x->psfOptions.moffat8;
      psfOptions.moffat6 = x->psfOptions.moffat6;
      psfOptions.moffat4 = x->psfOptions.moffat4;
      psfOptions.moffat25 = x->psfOptions.moffat25;
      psfOptions.moffat15 = x->psfOptions.moffat15;
      psfOptions.lorentzian = x->psfOptions.lorentzian;
      signedAngles = x->signedAngles;
      regenerate = x->regenerate;
      searchRadius = x->searchRadius;
      threshold = x->threshold;
      autoAperture = x->autoAperture;
      scaleMode = x->scaleMode;
      scaleValue = x->scaleValue;
      scaleKeyword = x->scaleKeyword;
      starColor = x->starColor;
      selectedStarColor = x->selectedStarColor;
      selectedStarFillColor = x->selectedStarFillColor;
      badStarColor = x->badStarColor;
      badStarFillColor = x->badStarFillColor;
   }
}

void DynamicPSFInstance::AssignOptions( const DynamicPSFInstance& x )
{
   views.Clear();
   stars.Clear();
   psfs.Clear();
   psfOptions.autoPSF = x.psfOptions.autoPSF;
   psfOptions.circular = x.psfOptions.circular;
   psfOptions.gaussian = x.psfOptions.gaussian;
   psfOptions.moffat = x.psfOptions.moffat;
   psfOptions.moffatA = x.psfOptions.moffatA;
   psfOptions.moffat8 = x.psfOptions.moffat8;
   psfOptions.moffat6 = x.psfOptions.moffat6;
   psfOptions.moffat4 = x.psfOptions.moffat4;
   psfOptions.moffat25 = x.psfOptions.moffat25;
   psfOptions.moffat15 = x.psfOptions.moffat15;
   psfOptions.lorentzian = x.psfOptions.lorentzian;
   signedAngles = x.signedAngles;
   regenerate = x.regenerate;
   searchRadius = x.searchRadius;
   threshold = x.threshold;
   autoAperture = x.autoAperture;
   scaleMode = x.scaleMode;
   scaleValue = x.scaleValue;
   scaleKeyword = x.scaleKeyword;
   starColor = x.starColor;
   selectedStarColor = x.selectedStarColor;
   selectedStarFillColor = x.selectedStarFillColor;
   badStarColor = x.badStarColor;
   badStarFillColor = x.badStarFillColor;
}

bool DynamicPSFInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "DynamicPSF can only be executed in the global context.";
   return false;
}

bool DynamicPSFInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

bool DynamicPSFInstance::ExecuteGlobal()
{
   DynamicPSFInterface::ExecuteInstance( *this );
   Console console;
   console.WriteLn( "<end><cbr>" + String( views.Length() ) + " view(s)" );
   console.WriteLn( String( stars.Length() ) + " star(s)" );
   console.WriteLn( String( psfs.Length() ) + " PSF fittings" );
   return true;
}

void* DynamicPSFInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDPViewIdParameter )
      return views[tableRow].c_str();

   if ( p == TheDPStarViewIndexParameter )
      return &stars[tableRow].view;
   if ( p == TheDPStarChannelParameter )
      return &stars[tableRow].channel;
   if ( p == TheDPStarStatusParameter )
      return &stars[tableRow].status;
   if ( p == TheDPStarRectX0Parameter )
      return &stars[tableRow].rect.x0;
   if ( p == TheDPStarRectY0Parameter )
      return &stars[tableRow].rect.y0;
   if ( p == TheDPStarRectX1Parameter )
      return &stars[tableRow].rect.x1;
   if ( p == TheDPStarRectY1Parameter )
      return &stars[tableRow].rect.y1;
   if ( p == TheDPStarPosXParameter )
      return &stars[tableRow].pos.x;
   if ( p == TheDPStarPosYParameter )
      return &stars[tableRow].pos.y;

   if ( p == TheDPPSFStarIndexParameter )
      return &psfs[tableRow].star;
   if ( p == TheDPPSFFunctionParameter )
      return &psfs[tableRow].function;
   if ( p == TheDPPSFCircularParameter )
      return &psfs[tableRow].circular;
   if ( p == TheDPPSFStatusParameter )
      return &psfs[tableRow].status;
   if ( p == TheDPPSFBackgroundParameter )
      return &psfs[tableRow].B;
   if ( p == TheDPPSFAmplitudeParameter )
      return &psfs[tableRow].A;
   if ( p == TheDPPSFCentroidXParameter )
      return &psfs[tableRow].c0.x;
   if ( p == TheDPPSFCentroidYParameter )
      return &psfs[tableRow].c0.y;
   if ( p == TheDPPSFRadiusXParameter )
      return &psfs[tableRow].sx;
   if ( p == TheDPPSFRadiusYParameter )
      return &psfs[tableRow].sy;
   if ( p == TheDPPSFRotationAngleParameter )
      return &psfs[tableRow].theta;
   if ( p == TheDPPSFBetaParameter )
      return &psfs[tableRow].beta;
   if ( p == TheDPPSFMADParameter )
      return &psfs[tableRow].mad;

   if ( p == TheDPAutoPSFParameter )
      return &psfOptions.autoPSF;
   if ( p == TheDPCircularPSFParameter )
      return &psfOptions.circular;
   if ( p == TheDPGaussianPSFParameter )
      return &psfOptions.gaussian;
   if ( p == TheDPMoffatPSFParameter )
      return &psfOptions.moffat;
   if ( p == TheDPMoffat10PSFParameter )
      return &psfOptions.moffatA;
   if ( p == TheDPMoffat8PSFParameter )
      return &psfOptions.moffat8;
   if ( p == TheDPMoffat6PSFParameter )
      return &psfOptions.moffat6;
   if ( p == TheDPMoffat4PSFParameter )
      return &psfOptions.moffat4;
   if ( p == TheDPMoffat25PSFParameter )
      return &psfOptions.moffat25;
   if ( p == TheDPMoffat15PSFParameter )
      return &psfOptions.moffat15;
   if ( p == TheDPLorentzianPSFParameter )
      return &psfOptions.lorentzian;

   if ( p == TheDPSignedAnglesParameter )
      return &signedAngles;
   if ( p == TheDPRegenerateParameter )
      return &regenerate;

   if ( p == TheDPSearchRadiusParameter )
      return &searchRadius;
   if ( p == TheDPThresholdParameter )
      return &threshold;
   if ( p == TheDPAutoApertureParameter )
      return &autoAperture;
   if ( p == TheDPScaleModeParameter )
      return &scaleMode;
   if ( p == TheDPScaleValueParameter )
      return &scaleValue;
   if ( p == TheDPScaleKeywordParameter )
      return scaleKeyword.c_str();

   if ( p == TheDPStarColorParameter )
      return &starColor;
   if ( p == TheDPSelectedStarColorParameter )
      return &selectedStarColor;
   if ( p == TheDPSelectedStarFillColorParameter )
      return &selectedStarFillColor;
   if ( p == TheDPBadStarColorParameter )
      return &badStarColor;
   if ( p == TheDPBadStarFillColorParameter )
      return &badStarFillColor;

   return 0;
}

bool DynamicPSFInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDPViewTableParameter )
   {
      views.Clear();
      if ( sizeOrLength > 0 )
         views = StringList( sizeOrLength );
   }
   else if ( p == TheDPViewIdParameter )
   {
      views[tableRow].Clear();
      if ( sizeOrLength > 0 )
         views[tableRow].SetLength( sizeOrLength );
   }
   else if ( p == TheDPStarTableParameter )
   {
      stars.Clear();
      if ( sizeOrLength > 0 )
         stars = Array<Star>( sizeOrLength );
   }
   else if ( p == TheDPPSFTableParameter )
   {
      psfs.Clear();
      if ( sizeOrLength > 0 )
         psfs = Array<PSF>( sizeOrLength );
   }
   else if ( p == TheDPScaleKeywordParameter )
   {
      scaleKeyword.Clear();
      if ( sizeOrLength > 0 )
         scaleKeyword.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type DynamicPSFInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheDPViewTableParameter )
      return views.Length();
   if ( p == TheDPViewIdParameter )
      return views[tableRow].Length();
   if ( p == TheDPStarTableParameter )
      return stars.Length();
   if ( p == TheDPPSFTableParameter )
      return psfs.Length();
   if ( p == TheDPScaleKeywordParameter )
      return scaleKeyword.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicPSFInstance.cpp - Released 2015/12/18 08:55:08 UTC
