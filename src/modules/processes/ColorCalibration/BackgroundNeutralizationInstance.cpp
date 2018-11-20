//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0319
// ----------------------------------------------------------------------------
// BackgroundNeutralizationInstance.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "BackgroundNeutralizationInstance.h"
#include "BackgroundNeutralizationParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BackgroundNeutralizationInstance::BackgroundNeutralizationInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   backgroundLow( TheBNBackgroundLowParameter->DefaultValue() ),
   backgroundHigh( TheBNBackgroundHighParameter->DefaultValue() ),
   useROI( TheBNUseROIParameter->DefaultValue() ),
   roi( 0 ),
   mode( BNMode::Default ),
   targetBackground( TheBNTargetBackgroundParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

BackgroundNeutralizationInstance::BackgroundNeutralizationInstance( const BackgroundNeutralizationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void BackgroundNeutralizationInstance::Assign( const ProcessImplementation& p )
{
   const BackgroundNeutralizationInstance* x = dynamic_cast<const BackgroundNeutralizationInstance*>( &p );
   if ( x != nullptr )
   {
      backgroundReferenceViewId = x->backgroundReferenceViewId;
      backgroundLow = x->backgroundLow;
      backgroundHigh = x->backgroundHigh;
      useROI = x->useROI;
      roi = x->roi;
      mode = x->mode;
      targetBackground = x->targetBackground;
   }
}

// ----------------------------------------------------------------------------

UndoFlags BackgroundNeutralizationInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool BackgroundNeutralizationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( !view.IsColor() )
   {
      whyNot = "BackgroundNeutralization can only be executed on color images.";
      return false;
   }
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "BackgroundNeutralization cannot be executed on complex images.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

template <class P>
static DVector BackgroundReferenceVector( const GenericImage<P>& image, float low, float high )
{
   Array<float> bR, bG, bB;
   for ( typename GenericImage<P>::const_roi_pixel_iterator i( image ); i; ++i, ++image.Status() )
   {
      float fR; P::FromSample( fR, i[0] );
      if ( fR > low && fR < high )
      {
         float fG; P::FromSample( fG, i[1] );
         if ( fG > low && fG < high )
         {
            float fB; P::FromSample( fB, i[2] );
            if ( fB > low && fB < high )
            {
               bR.Append( fR );
               bG.Append( fG );
               bB.Append( fB );
            }
         }
      }
   }

   DVector b( 0.0, 3 );
   if ( !bR.IsEmpty() )
   {
      b[0] = pcl::Median( bR.Begin(), bR.End() );
      b[1] = pcl::Median( bG.Begin(), bG.End() );
      b[2] = pcl::Median( bB.Begin(), bB.End() );
   }

   return b;
}

static DVector BackgroundReferenceVector( const ImageVariant& image, float low, float high )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return BackgroundReferenceVector( static_cast<const Image&>( *image ), low, high );
      case 64: return BackgroundReferenceVector( static_cast<const DImage&>( *image ), low, high );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return BackgroundReferenceVector( static_cast<const UInt8Image&>( *image ), low, high );
      case 16: return BackgroundReferenceVector( static_cast<const UInt16Image&>( *image ), low, high );
      case 32: return BackgroundReferenceVector( static_cast<const UInt32Image&>( *image ), low, high );
      }

   return 0;
}

// ----------------------------------------------------------------------------

bool BackgroundNeutralizationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   if ( !view.IsColor() )
      throw Error( "BackgroundNeutralization can only work for color images: " + view.FullId() );

   if ( useROI )
   {
      roi.Order();
      if ( !roi.IsRect() )
         throw Error( "Empty ROI defined" );
   }

   StandardStatus status;
   Console console;

   console.EnableAbort();

   DVector bg0;
   {
      View bkgView;

      if ( backgroundReferenceViewId.IsEmpty() )
         bkgView = view;
      else
      {
         bkgView = View::ViewById( backgroundReferenceViewId );
         if ( bkgView.IsNull() )
            throw Error( "No such view (background reference): " + backgroundReferenceViewId );
         if ( !bkgView.IsColor() )
            throw Error( "The background reference view must be a color image: " + backgroundReferenceViewId );
      }

      AutoViewLock bkgLock( bkgView, false/*lock*/ );
      if ( bkgView.CanWrite() )
         bkgLock.LockForWrite();

      ImageVariant v = bkgView.Image();

      if ( useROI )
      {
         v.SelectRectangle( roi );
         Rect r = v.SelectedRectangle();
         if ( !r.IsRect() )
            v.ResetSelection();
         console.WriteLn( String().Format( "<end><cbr>Region of interest : left=%d, top=%d, width=%d, height=%d",
                                           r.x0, r.y0, r.Width(), r.Height() ) );
      }

      v.SetStatusCallback( &status );
      v.Status().Initialize( "Evaluating background", v.NumberOfSelectedPixels() );

      bg0 = BackgroundReferenceVector( v, backgroundLow, backgroundHigh );

      console.NoteLn( String().Format( "<end><cbr>* Background reference:\n"
                                       "B_R : %.5e\n"
                                       "B_G : %.5e\n"
                                       "B_B : %.5e", bg0[0], bg0[1], bg0[2] ) );
   }

   ImageVariant v = view.Image();

   v.SetStatusCallback( &status );
   v.Status().Initialize( "Applying background neutralization", 3*2*v.NumberOfPixels() );
   v.Status().DisableInitialization();

   float bg1 = (mode == BNMode::TargetBackground) ? targetBackground : 0;

   if ( v.IsFloatSample() )
   {
      for ( int c = 0; c < 3; ++c )
         v.Add( bg1 - bg0[c], Rect( 0 ), c, c );

      switch ( mode )
      {
      default:
      case BNMode::TargetBackground:
      case BNMode::Truncate:
         v.Truncate();
         break;
      case BNMode::RescaleAsNeeded:
         v.Normalize();
         break;
      case BNMode::Rescale:
         v.Rescale();
         break;
      }
   }
   else
   {
      ImageVariant v1;
      v1.CreateFloatImage( (v.BitsPerSample() > 16) ? 64 : 32 );
      v1.CopyImage( v );

      for ( int c = 0; c < 3; ++c )
         v1.Add( bg1 - bg0[c], Rect( 0 ), c, c );

      switch ( mode )
      {
      default:
      case BNMode::TargetBackground:
      case BNMode::Truncate:
         v1.Truncate();
         break;
      case BNMode::RescaleAsNeeded:
         v1.Normalize();
         break;
      case BNMode::Rescale:
         v1.Rescale();
         break;
      }

      v.CopyImage( v1 );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* BackgroundNeutralizationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheBNBackgroundReferenceViewIdParameter )
      return backgroundReferenceViewId.Begin();
   if ( p == TheBNBackgroundLowParameter )
      return &backgroundLow;
   if ( p == TheBNBackgroundHighParameter )
      return &backgroundHigh;
   if ( p == TheBNUseROIParameter )
      return &useROI;
   if ( p == TheBNROIX0Parameter )
      return &roi.x0;
   if ( p == TheBNROIY0Parameter )
      return &roi.y0;
   if ( p == TheBNROIX1Parameter )
      return &roi.x1;
   if ( p == TheBNROIY1Parameter )
      return &roi.y1;
   if ( p == TheBNModeParameter )
      return &mode;
   if ( p == TheBNTargetBackgroundParameter )
      return &targetBackground;
   return 0;
}

bool BackgroundNeutralizationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheBNBackgroundReferenceViewIdParameter )
   {
      backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type BackgroundNeutralizationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheBNBackgroundReferenceViewIdParameter )
      return backgroundReferenceViewId.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BackgroundNeutralizationInstance.cpp - Released 2018-11-01T11:07:20Z
