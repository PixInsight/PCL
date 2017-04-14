//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.02.00.0247
// ----------------------------------------------------------------------------
// LinearFitInstance.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "LinearFitInstance.h"
#include "LinearFitParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Vector.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LinearFitInstance::LinearFitInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
referenceViewId(),
rejectLow( TheLFRejectLowParameter->DefaultValue() ),
rejectHigh( TheLFRejectHighParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

LinearFitInstance::LinearFitInstance( const LinearFitInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void LinearFitInstance::Assign( const ProcessImplementation& p )
{
   const LinearFitInstance* x = dynamic_cast<const LinearFitInstance*>( &p );
   if ( x != 0 )
   {
      referenceViewId = x->referenceViewId;
      rejectLow = x->rejectLow;
      rejectHigh = x->rejectHigh;
   }
}

// ----------------------------------------------------------------------------

bool LinearFitInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( referenceViewId.Trimmed().IsEmpty() )
   {
      whyNot = "This instance does not specify a reference view.";
      return false;
   }

   if ( view.Image().IsComplexSample() )
   {
      whyNot = "LinearFit cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class LinearFitEngine
{
public:

   typedef GenericVector<LinearFit> linear_fit_set;

   LinearFitEngine( const LinearFitInstance* L ) : instance( L )
   {
   }

   linear_fit_set Fit( const ImageVariant& image, const ImageVariant& reference )
   {
      if ( !image.IsComplexSample() )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: return Fit( static_cast<const Image&>( *image ), reference ); break;
            case 64: return Fit( static_cast<const DImage&>( *image ), reference ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: return Fit( static_cast<const UInt8Image&>( *image ), reference ); break;
            case 16: return Fit( static_cast<const UInt16Image&>( *image ), reference ); break;
            case 32: return Fit( static_cast<const UInt32Image&>( *image ), reference ); break;
            }
      return linear_fit_set();
   }

   void Apply( ImageVariant& image, const linear_fit_set& L )
   {
      if ( !image.IsComplexSample() )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: Apply( static_cast<Image&>( *image ), L ); break;
            case 64: Apply( static_cast<DImage&>( *image ), L ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8:
               {
                  UInt8Image& wrk = static_cast<UInt8Image&>( *image );
                  Image tmp( wrk );
                  Apply( tmp, L );
                  wrk.Apply( tmp );
               }
               break;
            case 16:
               {
                  UInt16Image& wrk = static_cast<UInt16Image&>( *image );
                  Image tmp( wrk );
                  Apply( tmp, L );
                  wrk.Apply( tmp );
               }
               break;
            case 32:
               {
                  UInt32Image& wrk = static_cast<UInt32Image&>( *image );
                  DImage tmp( wrk );
                  Apply( tmp, L );
                  wrk.Apply( tmp );
               }
               break;
            }
   }

private:

   const LinearFitInstance* instance;

   template <class P1, class P2>
   linear_fit_set Fit( const GenericImage<P1>& image, const GenericImage<P2>& reference )
   {
      linear_fit_set L( image.NumberOfNominalChannels() );
      GenericVector<size_type> count( image.NumberOfNominalChannels() );
      size_type N = image.NumberOfPixels();

      Console console;
      console.WriteLn( String().Format( "<end><cbr>Sampling interval: ]%.6f,%.6f[",
                                          instance->rejectLow, instance->rejectHigh ) );
      SpinStatus spin;
      StatusMonitor monitor;
      monitor.SetCallback( &spin );
      monitor.Initialize( "Fitting images" );

      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         Array<float> F1, F2;
         F1.Reserve( N );
         F2.Reserve( N );
         const typename P1::sample* v1 = image.PixelData( c );
         const typename P1::sample* vN = v1 + N;
         const typename P2::sample* v2 = reference.PixelData( c );
         for ( ; v1 < vN; ++v1, ++v2, ++monitor )
         {
            float f1; P1::FromSample( f1, *v1 );
            if ( f1 > instance->rejectLow && f1 < instance->rejectHigh )
            {
               float f2; P2::FromSample( f2, *v2 );
               if ( f2 > instance->rejectLow && f2 < instance->rejectHigh )
               {
                  F1.Add( f1 );
                  F2.Add( f2 );
               }
            }
         }

         if ( F1.Length() < 3 )
            throw Error( "Insufficient data (channel " + String( c ) + ')' );

         count[c] = F1.Length();

         L[c] = LinearFit( F1, F2, &monitor );

         if ( !L[c].IsValid() )
            throw Error( "Invalid linear fit (channel " + String( c ) + ')' );
      }

      monitor.Complete();

      console.WriteLn( "<end><cbr>Linear fit functions:" );
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         console.WriteLn( String().Format( "y<sub>%d</sub> = %+.6f %c %.6f&middot;x<sub>%d</sub>",
                                             c, L[c].a, (L[c].b < 0) ? '-' : '+', Abs( L[c].b ), c ) );
         console.WriteLn( String().Format( "&sigma;<sub>%d</sub> = %+.6f",
                                             c, L[c].adev ) );
         console.WriteLn( String().Format( "N<sub>%d</sub> = %6.2f%% (%u)",
                                             c, 100.0*count[c]/N, count[c] ) );
      }

      return L;
   }

   template <class P>
   linear_fit_set Fit( const GenericImage<P>& image, const ImageVariant& reference )
   {
      if ( !reference.IsComplexSample() )
         if ( reference.IsFloatSample() )
            switch ( reference.BitsPerSample() )
            {
            case 32: return Fit( image, static_cast<const Image&>( *reference ) ); break;
            case 64: return Fit( image, static_cast<const DImage&>( *reference ) ); break;
            }
         else
            switch ( reference.BitsPerSample() )
            {
            case  8: return Fit( image, static_cast<const UInt8Image&>( *reference ) ); break;
            case 16: return Fit( image, static_cast<const UInt16Image&>( *reference ) ); break;
            case 32: return Fit( image, static_cast<const UInt32Image&>( *reference ) ); break;
            }
      return linear_fit_set();
   }

   template <class P>
   void Apply( GenericImage<P>& image, const linear_fit_set& L )
   {
      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Applying linear fit", image.NumberOfNominalSamples() );

      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         typename P::sample* v = image.PixelData( c );
         typename P::sample* vN = v + image.NumberOfPixels();
         for ( ; v < vN; ++v, ++monitor )   // N
         {
            double f; P::FromSample( f, *v );
            *v = P::ToSample( L[c]( f ) );
         }
      }

      image.Truncate();
   }
};

// ----------------------------------------------------------------------------

bool LinearFitInstance::ExecuteOn( View& view )
{
   referenceViewId.Trim();
   if ( referenceViewId.IsEmpty() )
      throw Error( "No reference view has been specified" );

   if ( rejectLow >= rejectHigh )
      throw Error( "Empty sampling interval" );

   View referenceView = View::ViewById( referenceViewId );
   if ( referenceView.IsNull() )
      throw Error( "No such view (reference image): " + referenceViewId );
   if ( referenceView.IsPreview() && referenceView.Bounds() != referenceView.Window().MainView().Bounds() )
      throw Error( "Cannot use a partial preview as reference: " + referenceViewId );
   if ( referenceView == view )
      throw Error( "The reference and target images must be different" );

   View targetView;
   if ( view.IsPreview() )
      targetView = view.Window().MainView();
   else
      targetView = view;

   if ( targetView.Bounds() != referenceView.Bounds() || targetView.IsColor() != referenceView.IsColor() )
      throw Error( "Incompatible image geometry: " + targetView.Id() );

   AutoViewLock lock1( referenceView, false/*lock*/ );
   lock1.LockForWrite();

   AutoViewLock lock2( view, false/*lock*/ );
   if ( view.IsPreview() )
   {
      if ( view.Window() != referenceView.Window() )
         lock2.Lock();
   }
   else
      lock2.LockForWrite();

   ImageVariant image = view.Image();
   ImageVariant target = targetView.Image();
   ImageVariant reference = referenceView.Image();

   Console().EnableAbort();

   LinearFitEngine E( this );

   E.Apply( image, E.Fit( target, reference ) );

   return true;
}

// ----------------------------------------------------------------------------

void* LinearFitInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheLFReferenceViewIdParameter )
      return referenceViewId.Begin();
   if ( p == TheLFRejectLowParameter )
      return &rejectLow;
   if ( p == TheLFRejectHighParameter )
      return &rejectHigh;
   return 0;
}

bool LinearFitInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLFReferenceViewIdParameter )
   {
      referenceViewId.Clear();
      if ( sizeOrLength > 0 )
         referenceViewId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type LinearFitInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheLFReferenceViewIdParameter )
      return referenceViewId.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LinearFitInstance.cpp - Released 2017-04-14T23:07:12Z
