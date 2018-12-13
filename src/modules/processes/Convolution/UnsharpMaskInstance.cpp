//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0276
// ----------------------------------------------------------------------------
// UnsharpMaskInstance.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#include "UnsharpMaskInstance.h"
#include "UnsharpMaskParameters.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/View.h>
#include <pcl/StdStatus.h>
#include <pcl/Console.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/FFTConvolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/MuteStatus.h>
#include <pcl/Selection.h>
#include <pcl/Image.h>
#include <pcl/GlobalSettings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

UnsharpMaskInstance::UnsharpMaskInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   sigma( TheUSMSigmaParameter->DefaultValue() ),
   amount( TheUSMAmountParameter->DefaultValue() ),
   useLuminance( TheUSMUseLuminanceParameter->DefaultValue() ),
   linear( TheUSMLinearParameter->DefaultValue() ),
   deringing( TheUSMDeringingParameter->DefaultValue() ),
   deringingDark( TheUSMDeringingDarkParameter->DefaultValue() ),
   deringingBright( TheUSMDeringingBrightParameter->DefaultValue() ),
   outputDeringingMaps( TheUSMOutputDeringingMapsParameter->DefaultValue() ),
   rangeLow( TheUSMRangeLowParameter->DefaultValue() ),
   rangeHigh( TheUSMRangeHighParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

UnsharpMaskInstance::UnsharpMaskInstance( const UnsharpMaskInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void UnsharpMaskInstance::Assign( const ProcessImplementation& p )
{
   const UnsharpMaskInstance* x = dynamic_cast<const UnsharpMaskInstance*>( &p );
   if ( x != nullptr )
   {
      sigma = x->sigma;
      amount = x->amount;
      useLuminance = x->useLuminance;
      linear = x->linear;
      deringing = x->deringing;
      deringingDark = x->deringingDark;
      deringingBright = x->deringingBright;
      outputDeringingMaps = x->outputDeringingMaps;
      rangeLow = x->rangeLow;
      rangeHigh = x->rangeHigh;
   }
}

// ----------------------------------------------------------------------------

UndoFlags UnsharpMaskInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool UnsharpMaskInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "UnsharpMask cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class UnsharpMaskEngine
{
public:

   UnsharpMaskEngine( ImageVariant& image, const UnsharpMaskInstance& i, bool aUseConsole = true ) :
   instance( i ), useConsole( aUseConsole )
   {
      if ( instance.useLuminance && image->IsColor() )
      {
         ImageVariant L;
         if ( instance.linear )
            image.GetLuminance( L );
         else
            image.GetLightness( L );

         UnsharpMaskEngine( L, instance, useConsole );

         if ( instance.linear )
            image.SetLuminance( L );
         else
            image.SetLightness( L );
      }
      else
      {
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32 : Apply( static_cast<pcl::Image&>( *image ) ); break;
            case 64 : Apply( static_cast<pcl::DImage&>( *image ) ); break;
            }
         else
         {
            ImageVariant floatImage;
            floatImage.CreateFloatImage( (image.BitsPerSample() < 32) ? 32 : 64 );
            floatImage.CopyImage( image );
            switch ( floatImage.BitsPerSample() )
            {
            case 32 : Apply( static_cast<pcl::Image&>( *floatImage ) ); break;
            case 64 : Apply( static_cast<pcl::DImage&>( *floatImage ) ); break;
            }
            image.CopyImage( floatImage );
         }
      }
   }

private:

   const UnsharpMaskInstance& instance;
   bool useConsole;

   template <class P>
   class USMThread : public Thread
   {
   public:

      USMThread( P*, typename P::sample* V, const typename P::sample* VN, const float* M, double A1, double A2 ) :
      Thread(), v( V ), vN( VN ), m( M ), a1( A1 ), a2( A2 )
      {
      }

      virtual void Run()
      {
         do
         {
            double f; P::FromSample( f, *v );
            *v++ = P::ToSample( a1*f - a2 * *m++ );
         }
         while ( v != vN );
      }

   private:

      typename P::sample* v;
      const typename P::sample* vN;
      const float* m;
      double a1, a2;
   };

   template <class P>
   void Apply( GenericImage<P>& image )
   {
      size_type N = image.NumberOfPixels();
      int numberOfThreads = Thread::NumberOfThreads( N, 16 );
      size_type pixelsPerThread = N/numberOfThreads;

      /*
       * For USM filters of size <= 49px, we use separable convolutions in the
       * spatial domain. For larger filters, FFT convolutions are faster. This
       * limit has been determined empirically.
       */

      GaussianFilter G( instance.sigma, 0.05F );
      AutoPointer<ImageTransformation> T;
      if ( G.Size() < PCL_FFT_CONVOLUTION_IS_FASTER_THAN_SEPARABLE_FILTER_SIZE )
         T = new SeparableConvolution( G.AsSeparableFilter() );
      else
         T = new FFTConvolution( G );

      double a = 1 - 0.499*instance.amount;
      double a1 = a/(a+a - 1);
      double a2 = (1 - a)/(a+a - 1);

      image.ResetSelections();

      GenericImage<P> im0;  // deringing working image
      if ( instance.deringing )
      {
         image.SelectNominalChannels();
         im0 = image;
         im0.SetStatusCallback( 0 );
      }

      for ( int ch = 0; ch < image.NumberOfNominalChannels(); ++ch )
      {
         if ( useConsole && image.IsColor() )
            Console().WriteLn( "<end><cbr>Processing channel #" + String( ch ) );

         image.SelectChannel( ch );

         Image mask( image );
         mask.Status().Initialize( useConsole ? String( "Generating USM mask" ) : String(),
                                   mask.NumberOfPixels() );
         mask.Status().DisableInitialization();
         *T >> mask;

         image.Status().Initialize( useConsole ? String( "Applying unsharp mask filter" ) : String(), N );

         IndirectArray<USMThread<P> > threads;
         typename P::sample* v = image[ch];
         const typename P::sample* vN = v + N;
         const float* m = *mask;
         for ( int i = 0; i < numberOfThreads; ++i, v += pixelsPerThread, m += pixelsPerThread )
            threads.Add( new USMThread<P>( (P*)0, v, (i < numberOfThreads-1) ? v+pixelsPerThread : vN, m, a1, a2 ) );

         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i]->Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i]->Wait();

         threads.Destroy();

         image.Status() += N;
      }

      image.ResetSelections();
      image.SelectNominalChannels();

      if ( instance.deringing )
         Dering( image, im0 );

      Normalize( image );

      image.ResetSelections();
   }

   template <class P>
   void Dering( GenericImage<P>& i1, const GenericImage<P>& i0 )
   {
      i1.Status().Initialize( "Applying deringing routine", 12*i1.NumberOfNominalSamples() );
      i1.Status().DisableInitialization();

      float kd = 10 * instance.deringingDark;
      float kb = 10 * instance.deringingBright;

      i1.ResetSelections();
      i1.SelectNominalChannels();

      typename P::sample pmin = i1.MinimumPixelValue();
      if ( pmin < 0 )
         pmin = -pmin;
      pmin += 0.001F;

      i1 += pmin;  // N*n

      Image drm( i1 );

      size_type N = drm.NumberOfPixels();

      for ( int c = 0; c < drm.NumberOfChannels(); ++c )
      {
         float* f = drm[c];
         const float* fN = f + N;
         const typename P::sample* g = i0[c];
         do
            *f++ /= (*g++ + pmin);
         while ( f != fN );

         drm.Status() += N;   // N*n
      }

      drm.Rescale();          // N*n

      float median[ 3 ];
      for ( int c = 0; c < drm.NumberOfChannels(); ++c )
      {
         drm.SelectChannel( c );

         Image tmp( drm );
         size_type N2 = N >> 1;
         if ( N & 1 )
            median[c] = *pcl::Select( *tmp, *tmp + N, N2 );
         else
         {
            Sort( *tmp, *tmp + N );
            median[c] = 0.5*((*tmp)[N2] + (*tmp)[N2-1]);
         }

         drm.Status() += N;   // N*n
      }

      drm.ResetSelections();

      Image drm0;
      if ( kd > 0 && kb > 0 )
         drm0 = drm;

      if ( kd > 0 )
      {
         drm.Invert();           // N*n

         for ( int c = 0; c < drm.NumberOfChannels(); ++c )
         {
            drm.SelectChannel( c );

            drm.Sub( 1 - median[c] );   // N*n
            drm.Truncate();         // N*n

            typename P::sample* f = i1[c];
            const typename P::sample* fN = f + N;
            const float* d = drm[c];
            do
               *f++ += kd * *d++;
            while ( f != fN );

            drm.Status() += N;   // N*n
         }

         drm.ResetSelections();
         drm.SelectNominalChannels();

         if ( instance.outputDeringingMaps )
         {
            ImageWindow window( drm.Width(), drm.Height(), drm.NumberOfChannels(), 32, true, drm.IsColor(), true, "dr_map_dark" );
            window.MainView().Image().CopyImage( drm );
            window.Show();
         }

         if ( kb > 0 )
         {
            drm0.Status() = drm.Status();
            drm = drm0;
            drm0.FreeData();
         }
      }
      else
         drm.Status() += 4*N*drm.NumberOfChannels();

      if ( kb > 0 )
      {
         for ( int c = 0; c < drm.NumberOfChannels(); ++c )
         {
            drm.SelectChannel( c );

            drm.Sub( median[c] );   // N*n
            drm.Truncate();         // N*n

            typename P::sample* f = i1[c];
            const typename P::sample* fN = f + N;
            const float* d = drm[c];
            do
               *f++ -= kb * *d++;
            while ( f != fN );

            drm.Status() += N;   // N*n
         }

         drm.ResetSelections();
         drm.SelectNominalChannels();

         if ( instance.outputDeringingMaps )
         {
            ImageWindow window( drm.Width(), drm.Height(), drm.NumberOfChannels(), 32, true, drm.IsColor(), true, "dr_map_bright" );
            window.MainView().Image().CopyImage( drm );
            window.Show();
         }
      }
      else
         drm.Status() += 3*N*drm.NumberOfChannels();

      i1.Status() = drm.Status();
      i1.ResetSelections();
      i1.SelectNominalChannels();

      i1 -= pmin;  // N*n

      i1.Status().EnableInitialization();
   }

   template <class P>
   void Normalize( GenericImage<P>& image )
   {
      image.Status().Initialize( "Normalizing sample values", 2*image.NumberOfNominalSamples() );
      image.Status().DisableInitialization();
      image.SelectNominalChannels();
      image.Truncate( -instance.rangeLow, 1 + instance.rangeHigh ); // N*n
      image.Normalize();                                            // N*n
      image.Status().EnableInitialization();
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void UnsharpMaskInstance::Preview( UInt16Image& image ) const
{
   pcl_bool saveOutputDeringingMaps = outputDeringingMaps;
   const_cast<UnsharpMaskInstance*>( this )->outputDeringingMaps = false;

   ImageVariant v( &image );
   MuteStatus status;
   v.SetStatusCallback( &status );

   UnsharpMaskEngine( v, *this, false );

   const_cast<UnsharpMaskInstance*>( this )->outputDeringingMaps = saveOutputDeringingMaps;
}

// ----------------------------------------------------------------------------

bool UnsharpMaskInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   UnsharpMaskEngine( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* UnsharpMaskInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheUSMSigmaParameter )
      return &sigma;
   if ( p == TheUSMAmountParameter )
      return &amount;
   if ( p == TheUSMUseLuminanceParameter )
      return &useLuminance;
   if ( p == TheUSMLinearParameter )
      return &linear;
   if ( p == TheUSMDeringingParameter )
      return &deringing;
   if ( p == TheUSMDeringingDarkParameter )
      return &deringingDark;
   if ( p == TheUSMDeringingBrightParameter )
      return &deringingBright;
   if ( p == TheUSMOutputDeringingMapsParameter )
      return &outputDeringingMaps;
   if ( p == TheUSMRangeLowParameter )
      return &rangeLow;
   if ( p == TheUSMRangeHighParameter )
      return &rangeHigh;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF UnsharpMaskInstance.cpp - Released 2018-12-12T09:25:24Z
