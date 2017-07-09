//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 01.00.05.0309
// ----------------------------------------------------------------------------
// RestorationFilterInstance.cpp - Released 2017-07-09T18:07:33Z
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

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/FourierTransform.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/Selection.h>
#include <pcl/StdStatus.h>
#include <pcl/VariableShapeFilter.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RestorationFilterInstance::RestorationFilterInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   psfMode( RFPSFMode::Default ),
   psfSigma( TheRFPSFParametricSigmaParameter->DefaultValue() ),
   psfShape( TheRFPSFParametricShapeParameter->DefaultValue() ),
   psfAspectRatio( TheRFPSFParametricAspectRatioParameter->DefaultValue() ),
   psfRotationAngle( TheRFPSFParametricRotationAngleParameter->DefaultValue() ),
   psfMotionLength( TheRFPSFMotionLengthParameter->DefaultValue() ),
   psfMotionRotationAngle( TheRFPSFMotionRotationAngleParameter->DefaultValue() ),
   K( TheRFWienerKParameter->DefaultValue() ),
   gamma( TheRFLeastSquaresGammaParameter->DefaultValue() ),
   algorithm( RFAlgorithm::Default ),
   amount( TheRFAmountParameter->DefaultValue() ),
   toLuminance( TheRFToLuminanceParameter->DefaultValue() ),
   linear( TheRFLinearParameter->DefaultValue() ),
   deringing( TheRFDeringingParameter->DefaultValue() ),
   deringingDark( TheRFDeringingDarkParameter->DefaultValue() ),
   deringingBright( TheRFDeringingBrightParameter->DefaultValue() ),
   outputDeringingMaps( TheRFOutputDeringingMapsParameter->DefaultValue() ),
   rangeLow( TheRFRangeLowParameter->DefaultValue() ),
   rangeHigh( TheRFRangeHighParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

RestorationFilterInstance::RestorationFilterInstance( const RestorationFilterInstance& x ) : ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void RestorationFilterInstance::Assign( const ProcessImplementation& p )
{
   const RestorationFilterInstance* x = dynamic_cast<const RestorationFilterInstance*>( &p );
   if ( x != nullptr )
   {
      psfMode = x->psfMode;
      psfSigma = x->psfSigma;
      psfShape = x->psfShape;
      psfAspectRatio = x->psfAspectRatio;
      psfRotationAngle = x->psfRotationAngle;
      psfMotionLength = x->psfMotionLength;
      psfMotionRotationAngle = x->psfMotionRotationAngle;
      psfViewId = x->psfViewId;
      K = x->K;
      gamma = x->gamma;
      algorithm = x->algorithm;
      amount = x->amount;
      toLuminance = x->toLuminance;
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

bool RestorationFilterInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.Image().IsComplexSample() )
   {
      whyNot = "RestorationFilter cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static void CreateMotionBlurPSF( Image& image, float length, float thetaDegrees )
{
   float theta = Rad( thetaDegrees );
   int width = Max( 3, RoundI( length*Abs( Cos( theta ) ) ) );
   int height = Max( 3, RoundI( length*Abs( Sin( theta ) ) ) );
   VariableShapeFilter( Max( width, height ), 16, 0.01F, 0.75F/length, theta ).ToImage( image );
}

// ----------------------------------------------------------------------------

class RestorationFilterEngine
{
public:

   RestorationFilterEngine( ImageVariant& image, const RestorationFilterInstance& instance, const View& view ) :
   m_instance( instance ), m_psf()
   {
      if ( m_instance.toLuminance && image.IsColor() )
      {
         ImageVariant L;
         if ( m_instance.linear )
            image.GetLuminance( L );
         else
            image.GetLightness( L );

         RestorationFilterEngine( L, m_instance, view );

         if ( m_instance.linear )
            image.SetLuminance( L );
         else
            image.SetLightness( L );

         return;
      }

      switch ( m_instance.psfMode )
      {
      default:
      case RFPSFMode::Parametric:
         VariableShapeFilter( m_instance.psfSigma, m_instance.psfShape, 0.01F,
                              m_instance.psfAspectRatio, Rad( m_instance.psfRotationAngle ) ).ToImage( m_psf );
         break;

      case RFPSFMode::MotionBlur:
         CreateMotionBlurPSF( m_psf, m_instance.psfMotionLength, m_instance.psfMotionRotationAngle );
         break;

      case RFPSFMode::External:
         {
            if ( m_instance.psfViewId.IsEmpty() )
               throw Error( "No PSF view identifier has been specified." );

            View psfView = View::ViewById( m_instance.psfViewId );
            if ( psfView.IsNull() )
               throw Error( m_instance.psfViewId + ": No such PSF image." );
            if ( psfView.IsColor() )
               throw Error( m_instance.psfViewId + ": Color PSF images are not supported for deconvolution." );

            AutoViewLock lock( psfView, false );
            lock.LockForWrite();
            ImageVariant( &m_psf ).CopyImage( psfView.Image() );
         }
         break;
      }

      if ( m_psf.Width() > view.Width() || m_psf.Height() > view.Height() )
         throw Error( "The PSF kernel is larger than the target image." );

      if ( image.IsFloatSample() )
      {
         switch ( image.BitsPerSample() )
         {
         case 32 : Apply( static_cast<pcl::Image&>( *image ), (ComplexPixelTraits*)0 ); break;
         case 64 : Apply( static_cast<pcl::DImage&>( *image ), (DComplexPixelTraits*)0 ); break;
         }
      }
      else
      {
         ImageVariant floatImage;
         floatImage.CreateFloatImage( (image.BitsPerSample() < 32) ? 32 : 64 );
         floatImage.CopyImage( image );
         switch ( floatImage.BitsPerSample() )
         {
         case 32 : Apply( static_cast<pcl::Image&>( *floatImage ), (ComplexPixelTraits*)0 ); break;
         case 64 : Apply( static_cast<pcl::DImage&>( *floatImage ), (DComplexPixelTraits*)0 ); break;
         }
         image.CopyImage( floatImage );
      }
   }

   ~RestorationFilterEngine()
   {
      m_psf.FreeData();
   }

private:

   const RestorationFilterInstance& m_instance;
         Image                      m_psf;

   template <class P1, class P2>
   void Apply( GenericImage<P1>& image, P2* )
   {
      int w = image.Width();
      int h = image.Height();

      int wh = m_psf.Width();
      int hh = m_psf.Height();

      int w1 = FFT2D::OptimizedLength( w + 2*wh );
      int h1 = FFT2D::OptimizedLength( h + 2*hh );

      int dw = (w1 - w) >> 1;
      int dh = (h1 - h) >> 1;

      m_psf.ResetSelections();
      image.ResetSelections();

      GenericImage<P1> im0;  // deringing working image
      if ( m_instance.deringing )
      {
         image.SelectNominalChannels();
         im0 = image;
         im0.SetStatusCallback( 0 );
      }

      for( int ch = 0; ch < image.NumberOfNominalChannels(); ch++ )
      {
         if ( image.IsColor() )
            Console().WriteLn( "<end><cbr>Processing channel #" + String( ch ) );

         image.SelectChannel( ch );
         m_psf.SelectChannel( Min( ch, m_psf.NumberOfNominalChannels()-1 ) );

         GenericImage<P2> G( w1, h1 );
         G.Zero();
         G.SelectPoint( dw, dh );
         G.Apply( image );

         // Fix boundary artifacts by mirroring the image

         {
#define MIRROR_TOP_OR_BOTTOM                                                                 \
            {                                                                                \
               int i = 0;                                                                    \
               typename GenericImage<P2>::sample* cd = G.PixelAddress( 0, j );             \
               const typename GenericImage<P2>::sample* cs = G.PixelAddress( dw << 1, y ); \
               for ( ; i < dw; ++i )                                                         \
                  *cd++ = *cs--;                                                             \
               cs = G.PixelAddress( i, y );                                                  \
               for ( ; i < x1; ++i )                                                         \
                  *cd++ = *cs++;                                                             \
               cs = G.PixelAddress( x1-1, y );                                               \
               for ( ; i < G.Width(); ++i )                                                  \
                  *cd++ = *cs--;                                                             \
            }

            G.ResetSelections();
            int x1 = dw + w;
            int y1 = dh + h;
            int j = 0;
            for ( int y = dh << 1; j < dh; ++j, --y )
               MIRROR_TOP_OR_BOTTOM
            for ( int y = dh; j < y1; ++j, ++y )
            {
               typename GenericImage<P2>::sample* cd = G.PixelAddress( 0, j );
               const typename GenericImage<P2>::sample* cs = G.PixelAddress( dw << 1, y );
               for ( int i = 0; i < dw; ++i )
                  *cd++ = *cs--;
               cd = G.PixelAddress( x1, j );
               cs = G.PixelAddress( x1-1, y );
               for ( int i = x1; i < G.Width(); ++i )
                  *cd++ = *cs--;
            }
            for ( int y = y1-1; j < G.Height(); ++j, --y )
               MIRROR_TOP_OR_BOTTOM

#undef MIRROR_TOP_OR_BOTTOM
         }

         // Store the PSF in wrap-around order

         ComplexImage H( w1, h1 );
         H.Zero();

         {
            // Normalization factor:
            // the sum of response function elements must be equal to one.
            double k = m_psf.Norm();
            if ( 1 + k == 1 )
               throw Error( "Empty PSF!" );
            k = 1/k;

            int wh2 = wh >> 1;
            int hh2 = hh >> 1;

            // 1st quadrant -> 3rd
            for ( int sy = 0, ty = h1-hh2; sy < hh2; ++sy, ++ty )
               for ( int sx = wh2, tx = 0; sx < wh; ++sx, ++tx )
                  H.Pixel( tx, ty ) = k*m_psf.Pixel( sx, sy );
            // 2nd quadrant -> 4th
            for ( int sy = 0, ty = h1-hh2; sy < hh2; ++sy, ++ty )
               for ( int sx = 0, tx = w1-wh2; sx < wh2; ++sx, ++tx )
                  H.Pixel( tx, ty ) = k*m_psf.Pixel( sx, sy );
            // 3rd quadrant -> 1st
            for ( int sy = hh2, ty = 0; sy < hh; ++sy, ++ty )
               for ( int sx = 0, tx = w1-wh2; sx < wh2; ++sx, ++tx )
                  H.Pixel( tx, ty ) = k*m_psf.Pixel( sx, sy );
            // 4th quadrant -> 2nd
            for ( int sy = hh2, ty = 0; sy < hh; ++sy, ++ty )
               for ( int sx = wh2, tx = 0; sx < wh; ++sx, ++tx )
                  H.Pixel( tx, ty ) = k*m_psf.Pixel( sx, sy );
         }

         const char* algName;
         switch ( m_instance.algorithm )
         {
         case RFAlgorithm::Wiener: algName = "Wiener"; break;
         default:
         case RFAlgorithm::ConstrainedLeastSquares: algName = "Constrained Least Squares"; break;
         }

         G.Status() = image.Status();
         G.Status().Initialize( String( algName ) + " filter",
                              3*(w1 + h1) + G.NumberOfPixels() + image.NumberOfPixels() );
         G.Status().DisableInitialization();

         InPlaceFourierTransform() >> G;  // w1 + h1

         H.Status() = G.Status();
         InPlaceFourierTransform() >> H;  // w1 + h1
         G.Status() = H.Status();

         switch ( m_instance.algorithm )
         {
         case RFAlgorithm::Wiener:
            for ( int v = 0; v < h1; ++v )
               for ( int u = 0; u < w1; ++u, ++G.Status() ) // N
               {
                  ComplexImage::sample Huv = H.Pixel( u, v );
                  ComplexImage::sample Huv2 = Huv.Conj()*Huv;
                  G.Pixel( u, v ) *= (Huv2/(Huv2 + m_instance.K))/Huv;
               }
            break;

         case RFAlgorithm::ConstrainedLeastSquares:
            {
               // FFT of the Laplacian
               ComplexImage P( w1, h1 );
               P.Zero();
               int w2 = w1 >> 1;
               int h2 = h1 >> 1;
               P.Pixel( w2,   h2-1 ).Real() = -1;
               P.Pixel( w2-1, h2   ).Real() = -1;
               P.Pixel( w2,   h2   ).Real() =  4;
               P.Pixel( w2+1, h2   ).Real() = -1;
               P.Pixel( w2,   h2+1 ).Real() = -1;
               InPlaceFourierTransform() >> P;

               for ( int v = 0; v < h1; ++v )
                  for ( int u = 0; u < w1; ++u, ++G.Status() ) // N
                  {
                     ComplexImage::sample Huv = H.Pixel( u, v );
                     ComplexImage::sample Huvc = Huv.Conj();
                     ComplexImage::sample Huv2 = Huvc*Huv;
                     ComplexImage::sample Puv = P.Pixel( u, v );
                     ComplexImage::sample Puv2 = Puv.Conj()*Puv;
                     G.Pixel( u, v ) *= Huvc/(Huv2 + m_instance.gamma*Puv2);
                  }
            }
            break;
         }

         InPlaceInverseFourierTransform() >> G; // w1 + h1
         G /= double( G.NumberOfPixels() );     // N

         image.Status() = G.Status();

         if ( m_instance.amount == 1 )
         {
            G.SelectRectangle( dw, dh, dw+w, dh+h );
            image.Apply( G );                     // N
         }
         else
         {
            double a1 = 1 - m_instance.amount;
            for ( int y = 0; y < image.Height(); ++y )
            {
               typename P1::sample* v = image.ScanLine( y, image.SelectedChannel() );
               const typename P1::sample* vN = v + image.Width();
               const typename P2::sample* w = G.PixelAddress( dw, y+dh );
               do
               {
                  double m = w++->Mag();
                  double f; P1::FromSample( f, *v );
                  *v++ = P1::ToSample( a1*f + m_instance.amount*m );
               }
               while ( v != vN );

               image.Status() += image.Width();
            }
         }

         image.Status().EnableInitialization();
      }

      image.ResetSelections();
      image.SelectNominalChannels();

      if ( m_instance.deringing )
         Dering( image, im0 );

      Normalize( image );
   }

   template <class P>
   void Dering( GenericImage<P>& i1, const GenericImage<P>& i0 )
   {
      i1.Status().Initialize( "Applying deringing routine", 12*i1.NumberOfNominalSamples() );
      i1.Status().DisableInitialization();

      float kd = 10 * m_instance.deringingDark;
      float kb = 10 * m_instance.deringingBright;

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

         if ( m_instance.outputDeringingMaps )
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

         if ( m_instance.outputDeringingMaps )
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
      image.Truncate( -m_instance.rangeLow, 1 + m_instance.rangeHigh ); // N*n
      image.Normalize();                                                // N*n
      image.Status().EnableInitialization();
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool RestorationFilterInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   RestorationFilterEngine( image, *this, view );

   return true;
}

// ----------------------------------------------------------------------------

void* RestorationFilterInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheRFWienerKParameter )
      return &K;
   if ( p == TheRFLeastSquaresGammaParameter )
      return &gamma;
   if ( p == TheRFAlgorithmParameter )
      return &algorithm;
   if ( p == TheRFPSFModeParameter )
      return &psfMode;
   if ( p == TheRFPSFParametricSigmaParameter )
      return &psfSigma;
   if ( p == TheRFPSFParametricShapeParameter )
      return &psfShape;
   if ( p == TheRFPSFParametricAspectRatioParameter )
      return &psfAspectRatio;
   if ( p == TheRFPSFParametricRotationAngleParameter )
      return &psfRotationAngle;
   if ( p == TheRFPSFMotionLengthParameter )
      return &psfMotionLength;
   if ( p == TheRFPSFMotionRotationAngleParameter )
      return &psfMotionRotationAngle;
   if ( p == TheRFPSFViewIdParameter )
      return psfViewId.Begin();
   if ( p == TheRFAmountParameter )
      return &amount;
   if ( p == TheRFToLuminanceParameter )
      return &toLuminance;
   if ( p == TheRFLinearParameter )
      return &linear;
   if ( p == TheRFDeringingParameter )
      return &deringing;
   if ( p == TheRFDeringingDarkParameter )
      return &deringingDark;
   if ( p == TheRFDeringingBrightParameter )
      return &deringingBright;
   if ( p == TheRFOutputDeringingMapsParameter )
      return &outputDeringingMaps;
   if ( p == TheRFRangeLowParameter )
      return &rangeLow;
   if ( p == TheRFRangeHighParameter )
      return &rangeHigh;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool RestorationFilterInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheRFPSFViewIdParameter )
   {
      psfViewId.Clear();
      if ( sizeOrLength > 0 )
         psfViewId.SetLength( sizeOrLength );
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

size_type RestorationFilterInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheRFPSFViewIdParameter )
      return psfViewId.Length();

   return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool RestorationFilterInstance::CreatePSF( Image& psf ) const
{
   switch ( psfMode )
   {
   default:
   case RFPSFMode::Parametric:
      VariableShapeFilter( psfSigma, psfShape, 0.01F, psfAspectRatio, Rad( psfRotationAngle ) ).ToImage( psf );
      break;

   case RFPSFMode::MotionBlur:
      CreateMotionBlurPSF( psf, psfMotionLength, psfMotionRotationAngle );
      break;

   case RFPSFMode::External:
      {
         if ( psfViewId.IsEmpty() )
            return false;
         View psfView = View::ViewById( psfViewId );
         if ( psfView.IsNull() )
            return false;
         ImageVariant( &psf ).CopyImage( psfView.Image() );
      }
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RestorationFilterInstance.cpp - Released 2017-07-09T18:07:33Z
