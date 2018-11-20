//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 01.00.04.0249
// ----------------------------------------------------------------------------
// FourierTransformInstance.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
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

#include "FourierTransformInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/FourierTransform.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FourierTransformInstance::FourierTransformInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   radialCoordinates( TheFTRadialCoordinatesParameter->DefaultValue() ),
   centered( TheFTCenteredParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

FourierTransformInstance::FourierTransformInstance( const FourierTransformInstance& x ) :
   ProcessImplementation( x ),
   radialCoordinates( x.radialCoordinates ),
   centered( x.centered )
{
}

// -------------------------------------------------------------------------

void FourierTransformInstance::Assign( const ProcessImplementation& p )
{
   const FourierTransformInstance* x = dynamic_cast<const FourierTransformInstance*>( &p );
   if ( x != nullptr )
   {
      radialCoordinates = x->radialCoordinates;
      centered = x->centered;
   }
}

// ----------------------------------------------------------------------------

bool FourierTransformInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool FourierTransformInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.GetImage().IsComplexSample() )
   {
      whyNot = "FourierTransform cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

class FourierTransformEngine
{
public:

   FourierTransformEngine( const ImageVariant& image, const FourierTransformInstance& i ) : instance( i )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: DoTransform( static_cast<const Image&>( *image ), (ComplexPixelTraits*)0, (FloatPixelTraits*)0 ); break;
         case 64: DoTransform( static_cast<const DImage&>( *image ), (DComplexPixelTraits*)0, (DoublePixelTraits*)0 ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: DoTransform( static_cast<const UInt8Image&>( *image ), (ComplexPixelTraits*)0, (FloatPixelTraits*)0 ); break;
         case 16: DoTransform( static_cast<const UInt16Image&>( *image ), (ComplexPixelTraits*)0, (FloatPixelTraits*)0 ); break;
         case 32: DoTransform( static_cast<const UInt32Image&>( *image ), (DComplexPixelTraits*)0, (DoublePixelTraits*)0 ); break;
         }
   }

private:

   const FourierTransformInstance& instance;

   template <class P1, class P2, class P3>
   void DoTransform( const GenericImage<P1>& img, P2*, P3* )
   {
      int w0 = img.Width();
      int h0 = img.Height();

      int wc = FFT2D::OptimizedLength( w0 );
      int hc = FFT2D::OptimizedLength( h0 );

      GenericImage<P2> C( wc, hc, img.ColorSpace() );
      C.Zero();
      C.SelectPoint( (wc - img.Width()) >> 1, (hc - img.Height()) >> 1 );
      C.Apply( img );

      if ( instance.centered )
         ShiftToCenter( C );

      StandardStatus status;
      C.SetStatusCallback( &status );

      InPlaceFourierTransform() >> C;

      C.SetStatusCallback( 0 );

      C *= 1.0/C.NumberOfPixels();

      const char* id1, * id2;
      if ( instance.radialCoordinates )
      {
         id1 = "magnitude";
         id2 = "phase";
      }
      else
      {
         id1 = "real";
         id2 = "imaginary";
      }

      ImageWindow w1( wc, hc, C.NumberOfChannels(),
                      P3::BitsPerSample(), true/*floatSample*/, C.IsColor(),
                      true/*initialProcessing*/, IsoString( "DFT_" ) + id1 );
      ImageVariant v1 = w1.MainView().Image();
      GenericImage<P3>& img1 = static_cast<GenericImage<P3>&>( *v1 );

      ImageWindow w2( wc, hc, C.NumberOfChannels(),
                      P3::BitsPerSample(), true/*floatSample>*/, C.IsColor(),
                      true/*initialProcessing*/, IsoString( "DFT_" ) + id2 );
      ImageVariant v2 = w2.MainView().Image();
      GenericImage<P3>& img2 = static_cast<GenericImage<P3>&>( *v2 );

      if ( instance.radialCoordinates )
         TransferMagnitudeAndPhaseData( img1, img2, C );
      else
         TransferRealAndImaginaryData( img1, img2, C );

      SetMetadataAndRescaleDFTComponent( img1, w1, id1, w0, h0 );
      SetMetadataAndRescaleDFTComponent( img2, w2, id2, w0, h0 );

      w2.Show();
      w2.ZoomToFit( false/*allowMagnification*/ );

      w1.Show();
      w1.ZoomToFit( false/*allowMagnification*/ );
   }

   template <class P2>
   static void ShiftToCenter( GenericImage<P2>& C )
   {
      /*
       * Multiply by (-1)^(x+y) to translate the origin of the DFT to the center
       * of the transformation matrix.
       */
      for ( int ch = 0; ch < C.NumberOfChannels(); ++ch )
         for ( int i = 0; i < C.Height(); ++i )
         {
            typename P2::sample* c = C.ScanLine( i, ch ) + (i & 1);
            for ( int j = i & 1; j < C.Width(); j += 2, c += 2 )
               *c = -*c;
         }
   }

   template <class P2, class P3>
   static void TransferRealAndImaginaryData( GenericImage<P3>& R, GenericImage<P3>& I, const GenericImage<P2>& C )
   {
      for ( int ch = 0; ch < C.NumberOfChannels(); ++ch )
      {
         const typename P2::sample* c  = C[ch];
         const typename P2::sample* cN = c + C.NumberOfPixels();
               typename P3::sample* r  = R[ch];
               typename P3::sample* i  = I[ch];
         do
         {
            *r++ = c->Real();
            *i++ = c->Imag();
         }
         while ( ++c < cN );
      }
   }

   template <class P2, class P3>
   class TransferMagnitudeAndPhaseDataThread : public Thread
   {
   public:

      TransferMagnitudeAndPhaseDataThread( GenericImage<P3>& _M,
                                           GenericImage<P3>& _P,
                                           const GenericImage<P2>& _C,
                                           size_type _start, size_type _end ) :
         M( _M ),
         P( _P ),
         C( _C ),
         start( _start ),
         end( _end )
      {
      }

      virtual void Run()
      {
         for ( int ch = 0; ch < C.NumberOfChannels(); ++ch )
         {
            const typename P2::sample* c  = C[ch] + start;
            const typename P2::sample* cN = C[ch] + end;
                  typename P3::sample* m  = M[ch] + start;
                  typename P3::sample* p  = P[ch] + start;
            do
            {
               *m++ = c->Mag();
               *p++ = c->Arg();
            }
            while ( ++c < cN );
         }
      }

   private:

      GenericImage<P3>& M;
      GenericImage<P3>& P;
      const GenericImage<P2>& C;
      size_type start, end;
   };

   template <class P2, class P3>
   static void TransferMagnitudeAndPhaseData( GenericImage<P3>& M, GenericImage<P3>& P, const GenericImage<P2>& C )
   {
      size_type N = C.NumberOfPixels();
      int numberOfThreads = Thread::NumberOfThreads( N, 1024 );
      int pixelsPerThread = N/numberOfThreads;

      IndirectArray<TransferMagnitudeAndPhaseDataThread<P2, P3> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new TransferMagnitudeAndPhaseDataThread<P2, P3>( M, P, C,
                                    i*pixelsPerThread, (j < numberOfThreads) ? j*pixelsPerThread : N ) );

      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i]->Start( ThreadPriority::DefaultMax, i );
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i]->Wait();

      threads.Destroy();
   }

   template <class P3>
   void SetMetadataAndRescaleDFTComponent( GenericImage<P3>& img, ImageWindow& w,
                                           const char* component, int width, int height )
   {
      typename P3::sample fmin, fmax;
      img.GetExtremePixelValues( fmin, fmax );

      FITSHeaderKeyword kcmp( "PIDFTCMP",
                              component,
                              "PixInsight: Discrete Fourier transform component" );
      FITSHeaderKeyword kmin( "PIDFTMIN",
                              IsoString().Format( "%+.15e", double( fmin ) ),
                              "PixInsight: Minimum value of DFT component" );
      FITSHeaderKeyword kmax( "PIDFTMAX",
                              IsoString().Format( "%+.15e", double( fmax ) ),
                              "PixInsight: Maximum value of DFT component" );
      FITSHeaderKeyword kcnt( "PIDFTCNT",
                              IsoString( bool( instance.centered ) ),
                              "PixInsight: DFT shifted to center" );
      FITSHeaderKeyword kxsz( "PIDFTXSZ",
                              IsoString().Format( "%d", width ),
                              "PixInsight: Unoptimized width in pixels before DFT" );
      FITSHeaderKeyword kysz( "PIDFTYSZ",
                              IsoString().Format( "%d", height ),
                              "PixInsight: Unoptimized height in pixels before DFT" );

      FITSKeywordArray keywords;
      w.GetKeywords( keywords );
      keywords.Add( kcmp );
      keywords.Add( kmin );
      keywords.Add( kmax );
      keywords.Add( kcnt );
      keywords.Add( kxsz );
      keywords.Add( kysz );
      w.SetKeywords( keywords );

      const double delta = fmax - fmin;
      for ( int ch = 0; ch < img.NumberOfChannels(); ++ch )
      {
         typename P3::sample* f  = img[ch];
         typename P3::sample* fN = f + img.NumberOfPixels();
         do
         {
            *f -= fmin;
            *f /= delta;
         }
         while ( ++f < fN );
      }
      //img.Rescale();
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool FourierTransformInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );
   ImageVariant v = view.GetImage();
   Console().EnableAbort();
   FourierTransformEngine( v, *this );
   return true;
}

// ----------------------------------------------------------------------------

void* FourierTransformInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheFTRadialCoordinatesParameter )
      return &radialCoordinates;
   if ( p == TheFTCenteredParameter )
      return &centered;

   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FourierTransformInstance.cpp - Released 2018-11-01T11:07:20Z
