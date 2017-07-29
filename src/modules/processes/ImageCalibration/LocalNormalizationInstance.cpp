//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0300
// ----------------------------------------------------------------------------
// LocalNormalizationInstance.cpp - Released 2017-05-17T17:41:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "LocalNormalizationInstance.h"
#include "LocalNormalizationParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/MultiscaleLinearTransform.h>
#include <pcl/StdStatus.h>
#include <pcl/Vector.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LocalNormalizationInstance::LocalNormalizationInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_scale( int32( TheLNScaleParameter->DefaultValue() ) )
{
}

// ----------------------------------------------------------------------------

LocalNormalizationInstance::LocalNormalizationInstance( const LocalNormalizationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInstance::Assign( const ProcessImplementation& p )
{
   const LocalNormalizationInstance* x = dynamic_cast<const LocalNormalizationInstance*>( &p );
   if ( x != nullptr )
   {
      p_scale           = x->p_scale;
      p_referenceViewId = x->p_referenceViewId;
   }
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::IsHistoryUpdater( const View& view ) const
{
   return true;
}

// ----------------------------------------------------------------------------

UndoFlags LocalNormalizationInstance::UndoMode( const View& view ) const
{
   return UndoFlag::DefaultMode;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( p_referenceViewId.Trimmed().IsEmpty() )
//   if ( !p_reference.IsDefined() )
   {
      whyNot = "No reference image has been specified.";
      return false;
   }

   if ( view.Image().IsComplexSample() )
   {
      whyNot = "LocalNormalization cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

class SurfacePolynomial
{
public:

   SurfacePolynomial( const Array<double>& X, const Array<double>& Y, const Array<double>& Z, const Rect& rect, int degree = 3 ) :
      m_rect( rect ),
      m_degree( Max( 1, degree ) )
   {
      const int numberOfSamples = int( X.Length() );
      const int size = (degree + 1)*(degree + 2) >> 1;

      GenericVector<DVector> z( numberOfSamples );
      {
         int h1 = rect.Height()-1;
         int w1 = rect.Width()-1;
         for ( int k = 0; k < numberOfSamples; ++k )
         {
            z[k] = DVector( size );
            for ( int i = 0, l = 0; i <= degree; ++i )
               for ( int j = 0; j <= degree-i; ++j, ++l )
                  z[k][l] = PowI( X[k]/w1, i ) * PowI( Y[k]/h1, j );
         }
      }

      DMatrix M( 0.0, size, size );
      DVector R( 0.0, size );
      {
         int N2 = numberOfSamples*numberOfSamples;
         for ( int i = 0; i < size; ++i )
         {
            for ( int j = 0; j < size; ++j )
            {
               for ( int k = 0; k < numberOfSamples; ++k )
                  M[i][j] += z[k][i] * z[k][j];
               M[i][j] /= N2;
            }

            for ( int k = 0; k < numberOfSamples; ++k )
               R[i] += Z[k] * z[k][i];
            R[i] /= N2;
         }
      }

      for ( int i = 0; i < size; ++i )
      {
         double pMi = M[i][i];
         if ( M[i][i] != 0 )
         {
            for ( int j = i; j < size; ++j )
               M[i][j] /= pMi;
            R[i] /= pMi;
         }

         for ( int k = 1; i+k < size; ++k )
         {
            double pMk = M[i+k][i];
            if ( M[i+k][i] != 0 )
            {
               for ( int j = i; j < size; ++j )
               {
                  M[i+k][j] /= pMk;
                  M[i+k][j] -= M[i][j];
               }
               R[i+k] /= pMk;
               R[i+k] -= R[i];
            }
         }
      }

      m_coefficients = DVector( size );
      for ( int i = size; --i >= 0; )
      {
         m_coefficients[i] = R[i];
         for ( int j = i; --j >= 0; )
            R[j] -= M[j][i]*R[i];
      }
   }

   template <typename T>
   double operator ()( T x, T y ) const
   {
      double dx = double( x )/(m_rect.Width() - 1);
      double dy = double( y )/(m_rect.Height() - 1);
      double z = 0;
      double px = 1;
      for ( int i = 0, l = 0; ; px *= dx )
      {
         double py = 1;
         for ( int j = 0; j <= m_degree-i; py *= dy, ++j, ++l )
            z += m_coefficients[l]*px*py;
         if ( ++i > m_degree )
            break;
      }
      return z;
   }

private:

   Rect    m_rect;
   int     m_degree;
   DVector m_coefficients;
};

class LocalImageNormalizationEngine
{
public:

   LocalImageNormalizationEngine( int scale = 7 ) :
      m_scale( scale )
   {
   }

   void Build( const ImageVariant& target, const ImageVariant& reference )
   {
      if ( !reference.IsComplexSample() )
         if ( reference.IsFloatSample() )
            switch ( reference.BitsPerSample() )
            {
            case 32: Build( target, static_cast<const Image&>( *reference ) ); break;
            case 64: Build( target, static_cast<const DImage&>( *reference ) ); break;
            }
         else
            switch ( reference.BitsPerSample() )
            {
            case  8: Build( target, static_cast<const UInt8Image&>( *reference ) ); break;
            case 16: Build( target, static_cast<const UInt16Image&>( *reference ) ); break;
            case 32: Build( target, static_cast<const UInt32Image&>( *reference ) ); break;
            }
   }

   void Apply( ImageVariant& target ) const
   {
      if ( !target.IsComplexSample() )
         if ( target.IsFloatSample() )
            switch ( target.BitsPerSample() )
            {
            case 32: Apply( static_cast<Image&>( *target ) ); break;
            case 64: Apply( static_cast<DImage&>( *target ) ); break;
            }
         else
            switch ( target.BitsPerSample() )
            {
            case  8: Apply( static_cast<UInt8Image&>( *target ) ); break;
            case 16: Apply( static_cast<UInt16Image&>( *target ) ); break;
            case 32: Apply( static_cast<UInt32Image&>( *target ) ); break;
            }
   }

private:

   int    m_scale;
   DImage m_A, m_B;


   static void CreateImageWindow( const Image& image, const IsoString& id )
   {
      ImageWindow window( 1, 1, 1, 32, true, false, true, id );
      window.MainView().Image().CopyImage( image );
      window.Show();
      window.ZoomToFit();
   }

   class FixZeroThread : public Thread
   {
   public:

      FixZeroThread( Image& image, const SurfacePolynomial& P, int channel, int startRow, int endRow ) :
         m_image( image ),
         m_P( P ),
         m_channel( channel ),
         m_startRow( startRow ),
         m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         Image::sample_iterator r( m_image, m_channel );
         r.MoveBy( 0, m_startRow );
         for ( int y = m_startRow; y < m_endRow; ++y )
            for ( int x = 0; x < m_image.Width(); ++x, ++r )
               if ( *r == 0 )
                  *r = m_P( x, y );
      }

   private:

      Image&                   m_image;
      const SurfacePolynomial& m_P;
      int                      m_channel;
      int                      m_startRow;
      int                      m_endRow;
   };

   static void FixZero( Image& image, int delta = 64, int sampleSize = 32 )
   {
      const int w  = image.Width();
      const int h  = image.Height();
      const int d2 = delta >> 1;
      const int s2 = sampleSize >> 1;

      image.SetRangeClipping( 0, 0.92 );

      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         Array<double> X0, Y0, Z0;
         for ( int y = d2; y < h; y += delta )
            for ( int x = d2; x < w; x += delta )
               if ( image( x, y, c ) != 0 )
               {
                  double z = image.Median( Rect( x-s2, y-s2, x+s2+1, y+s2+1 ), c, c, 1 );
                  if ( 1 + z != 1 )
                  {
                     X0 << x;
                     Y0 << y;
                     Z0 << z;
                  }
               }

         double m = NondestructiveMedian( Z0.Begin(), Z0.End() );
         double s = 1.5*MAD( Z0.Begin(), Z0.End(), m );
         Array<double> X, Y, Z;
         for ( size_type i = 0; i < Z0.Length(); ++i )
            if ( Abs( Z0[i] - m ) < 3*s )
            {
               X << X0[i];
               Y << Y0[i];
               Z << Z0[i];
            }

         if ( X.Length() < 16 )
            throw Error( "FixZero(): Insufficient data to sample nonzero image pixels, channel " + String( c ) );

//          SurfaceSpline<double> S;
//          S.SetSmoothing( 0.1 );
//          S.Initialize( X.Begin(), Y.Begin(), Z.Begin(), X.Length() );
//          GridInterpolation G;
//          G.Initialize( image.Bounds(), 8/*delta*/, S, false/*verbose*/ );

         SurfacePolynomial P( X, Y, Z, image.Bounds(), 3/*degree*/ );

         int numberOfThreads = Thread::NumberOfThreads( image.Height(), 4 );
         int rowsPerThread = image.Height()/numberOfThreads;
         ReferenceArray<FixZeroThread> threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads << new FixZeroThread( image, P, c,
                                 i*rowsPerThread,
                                 (j < numberOfThreads) ? j*rowsPerThread : image.Height() );
         if ( numberOfThreads > 1 )
         {
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i].Start( ThreadPriority::DefaultMax, i );
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i].Wait();
         }
         else
            threads[0].Run();
      }

//       CreateImageWindow( image, "Z" );
   }

   template <class P> static
   Image Extended( const GenericImage<P> image, int scale )
   {
      int w0 = image.Width();
      int h0 = image.Height();
      int n = image.NumberOfChannels();
      int delta = 1 << scale;
      int w1 = w0 + 2*delta;
      int h1 = h0 + 2*delta;
      Image result;
      result.AllocateData( w1, h1, n );
      result.Zero();
      result.Apply( image, ImageOp::Mov, Point( delta ), 0/*channel*/, Rect( 0 ), 0/*firstChannel*/, n-1/*lastChannel*/ );
      /*
      for ( int c = 0; c < n; ++c )
      {
         {
            typename GenericImage<P>::const_sample_iterator i( image, c );
            Image::sample_iterator r( result, c );
            r.MoveBy( 0, delta );
            for ( int y = 0; y < h0; ++y )
            {
               r += delta - 1;
               for ( int x = 0; x < delta; ++x, ++i, --r )
                  *r = *i;
               i += w0 - delta - 1;
               r += w0 + delta + 1;
               for ( int x = 0; x < delta; ++x, --i, ++r )
                  *r = *i;
               i += delta + 1;
            }
         }

         {
            Image::const_sample_iterator s( result, c );
            Image::sample_iterator t( result, c );
            s.MoveBy( 0, delta );
            t.MoveBy( 0, delta - 1 );
            for ( int y = 0; y < delta; ++y, s += w1, t -= w1 )
               ::memcpy( t.Position(), s.Position(), w1*P::BytesPerSample() );
         }

         {
            Image::const_sample_iterator s( result, c );
            Image::sample_iterator t( result, c );
            s.MoveBy( 0, h0 + delta - 1 );
            t.MoveBy( 0, h0 + delta );
            for ( int y = 0; y < delta; ++y, s -= w1, t += w1 )
               ::memcpy( t.Position(), s.Position(), w1*P::BytesPerSample() );
         }
      }
      */

      return result;
   }

   template <class P>
   class BuildThread : public Thread
   {
   public:

      BuildThread( AbstractImage::ThreadData& data,
                   DImage& A, DImage& B,
                   const GenericImage<P>& target,
                   const Image& Tm, const Image& T2, const Image& Rm, const Image& R2,
                   size_type begin, size_type end ) :
         m_data( data ),
         m_A( A ),
         m_B( B ),
         m_target( target ),
         m_Tm( Tm ),
         m_T2( T2 ),
         m_Rm( Rm ),
         m_R2( R2 ),
         m_begin( begin ),
         m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         for ( int c = 0; c < m_target.NumberOfChannels(); ++c )
         {
            typename GenericImage<P>::const_sample_iterator t( m_target, c );
            Image::const_sample_iterator tm( m_Tm, c );
            Image::const_sample_iterator t2( m_T2, c );
            Image::const_sample_iterator rm( m_Rm, c );
            Image::const_sample_iterator r2( m_R2, c );
            DImage::sample_iterator a( m_A, c );
            DImage::sample_iterator b( m_B, c );

            t  += m_begin;
            tm += m_begin;
            t2 += m_begin;
            rm += m_begin;
            r2 += m_begin;
            a += m_begin;
            b += m_begin;

            for ( size_type p = m_begin; p < m_end; ++p, ++a, ++b, ++t, ++tm, ++t2, ++rm, ++r2 )
            {
               if ( *t != 0 )
               {
                  double an = Sqrt( *r2 - *rm * *rm );
                  if ( 1 + an != 1 )
                  {
                     double ad = Sqrt( *t2 - *tm * *tm );
                     if ( 1 + ad != 1 )
                     {
                        *a = an/ad;
                        *b = *rm - *a * *tm;
                        goto __next_sample;
                     }
                  }
               }

               *a = 1;
               *b = 0;
__next_sample:
               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      AbstractImage::ThreadData& m_data;
      DImage&                    m_A;
      DImage&                    m_B;
      const GenericImage<P>&     m_target;
      const Image&               m_Tm;
      const Image&               m_T2;
      const Image&               m_Rm;
      const Image&               m_R2;
      size_type                  m_begin;
      size_type                  m_end;
   };

   template <class P1, class P2>
   void Build( const GenericImage<P1>& target, const GenericImage<P2>& reference )
   {
      if ( target.Bounds() != reference.Bounds() || target.NumberOfChannels() != reference.NumberOfChannels() )
         throw Error( "LocalImageNormalizationEngine::Build(): Internal error: Incompatible image geometries." );

      int delta = 1 << m_scale;

      Image T = Extended( target, m_scale );
      FixZero( T );
      Image R = Extended( reference, m_scale );
      FixZero( R );

      MultiscaleLinearTransform L( m_scale, 0/*scalingSequence:dyadic*/, true/*useMeanFilters*/ );
      for ( int j = 0; j < m_scale; ++j )
         L.DisableLayer( j );

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Sampling local normalization data", (2 + 4*m_scale)*T.NumberOfSamples() );

      T.Status() = monitor;
      T.Status().DisableInitialization();
      L << T;
      Image Tm = L[m_scale];
      Tm.CropBy( -delta, -delta, -delta, -delta );

      R.Status() = T.Status();
      L << R;
      Image Rm = L[m_scale];
      Rm.CropBy( -delta, -delta, -delta, -delta );

      T.Status() = R.Status();
      T.Multiply( T );
      L << T;
      Image T2 = L[m_scale];
      T2.CropBy( -delta, -delta, -delta, -delta );

      R.Status() = T.Status();
      R.Multiply( R );
      L << R;
      Image R2 = L[m_scale];
      R2.CropBy( -delta, -delta, -delta, -delta );
      monitor.Initialize( "Building local normalization functions", target.NumberOfSamples() );

      m_A.AllocateData( target.Width(), target.Height(), target.NumberOfChannels() );
      m_B.AllocateData( target.Width(), target.Height(), target.NumberOfChannels() );

      AbstractImage::ThreadData data( monitor, target.NumberOfSamples() );
      ReferenceArray<BuildThread<P1> > threads;
      int numberOfThreads = Thread::NumberOfThreads( target.NumberOfPixels(), 1024 );
      size_type pixelsPerThread = target.NumberOfPixels()/numberOfThreads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads << new BuildThread<P1>( data, m_A, m_B, target, Tm, T2, Rm, R2,
                                 i*pixelsPerThread,
                                 (j < numberOfThreads) ? j*pixelsPerThread : target.NumberOfPixels() );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
   }

   template <class P>
   void Build( const ImageVariant& target, const GenericImage<P>& reference )
   {
      if ( !target.IsComplexSample() )
         if ( target.IsFloatSample() )
            switch ( target.BitsPerSample() )
            {
            case 32: Build( static_cast<const Image&>( *target ), reference ); break;
            case 64: Build( static_cast<const DImage&>( *target ), reference ); break;
            }
         else
            switch ( target.BitsPerSample() )
            {
            case  8: Build( static_cast<const UInt8Image&>( *target ), reference ); break;
            case 16: Build( static_cast<const UInt16Image&>( *target ), reference ); break;
            case 32: Build( static_cast<const UInt32Image&>( *target ), reference ); break;
            }
   }

   template <class P>
   class ApplyThread : public Thread
   {
   public:

      ApplyThread( AbstractImage::ThreadData& data, GenericImage<P>& target, const DImage& A, const DImage& B,
                   size_type begin, size_type end ) :
         m_data( data ),
         m_target( target ),
         m_A( A ),
         m_B( B ),
         m_begin( begin ),
         m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         for ( int c = 0; c < m_target.NumberOfChannels(); ++c )
         {
            typename GenericImage<P>::sample_iterator t( m_target, c );
            DImage::const_sample_iterator a( m_A, c );
            DImage::const_sample_iterator b( m_B, c );

            t += m_begin;
            a += m_begin;
            b += m_begin;

            for ( size_type p = m_begin; p < m_end; ++p, ++t, ++a, ++b )
            {
               double f; P::FromSample( f, *t );
               *t = P::ToSample( Range( *a * f + *b, 0.0, 1.0 ) );

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      AbstractImage::ThreadData& m_data;
      GenericImage<P>&           m_target;
      const DImage&              m_A;
      const DImage&              m_B;
      size_type                  m_begin;
      size_type                  m_end;
   };

   template <class P>
   void Apply( GenericImage<P>& target ) const
   {
      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Applying local normalization", target.NumberOfSamples() );

      AbstractImage::ThreadData data( monitor, target.NumberOfSamples() );
      ReferenceArray<ApplyThread<P> > threads;
      int numberOfThreads = Thread::NumberOfThreads( target.NumberOfPixels(), 1024 );
      size_type pixelsPerThread = target.NumberOfPixels()/numberOfThreads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads << new ApplyThread<P>( data, target, m_A, m_B,
                                 i*pixelsPerThread,
                                 (j < numberOfThreads) ? j*pixelsPerThread : target.NumberOfPixels() );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
   }
};

// ----------------------------------------------------------------------------
/*
class LinearFitEngine
{
public:

   typedef Array<LinearFit>   linear_fits;
   typedef Array<size_type>   fit_counts;

   linear_fits fits;
   fit_counts  counts;

   LinearFitEngine( float rejectLow = 0, float rejectHigh = 0.92F ) :
      m_rejectLow( rejectLow ), m_rejectHigh( rejectHigh )
   {
   }

   void Fit( const ImageVariant& target, const ImageVariant& reference )
   {
      if ( target.IsFloatSample() )
         switch ( target.BitsPerSample() )
         {
         case 32: Fit( static_cast<const Image&>( *target ), reference ); break;
         case 64: Fit( static_cast<const DImage&>( *target ), reference ); break;
         }
      else
         switch ( target.BitsPerSample() )
         {
         case  8: Fit( static_cast<const UInt8Image&>( *target ), reference ); break;
         case 16: Fit( static_cast<const UInt16Image&>( *target ), reference ); break;
         case 32: Fit( static_cast<const UInt32Image&>( *target ), reference ); break;
         }
   }

   void Apply( ImageVariant& target ) const
   {
      if ( !target.IsComplexSample() )
         if ( target.IsFloatSample() )
            switch ( target.BitsPerSample() )
            {
            case 32: Apply( static_cast<Image&>( *target ) ); break;
            case 64: Apply( static_cast<DImage&>( *target ) ); break;
            }
         else
            switch ( target.BitsPerSample() )
            {
            case  8: Apply( static_cast<UInt8Image&>( *target ) ); break;
            case 16: Apply( static_cast<UInt16Image&>( *target ) ); break;
            case 32: Apply( static_cast<UInt32Image&>( *target ) ); break;
            }
   }

private:

   float m_rejectLow, m_rejectHigh;

   template <class P1, class P2>
   void Fit( const GenericImage<P1>& target, const GenericImage<P2>& reference )
   {
      if ( target.Bounds() != reference.Bounds() || target.NumberOfNominalChannels() != reference.NumberOfNominalChannels() )
         throw Error( "LinearFitEngine::Fit(): Internal error: Incompatible image geometries." );

      fits.Clear();
      counts.Clear();

      Console console;
      console.WriteLn( String().Format( "<end><cbr>Sampling interval: (%.6f,%.6f)", m_rejectLow, m_rejectHigh ) );
      SpinStatus spin;
      StatusMonitor monitor;
      monitor.SetCallback( &spin );
      monitor.Initialize( "Fitting images" );

      for ( int c = 0; c < target.NumberOfNominalChannels(); ++c )
      {
         Array<double> F1, F2;
         typename GenericImage<P1>::sample_iterator t( target, c );
         typename GenericImage<P2>::const_sample_iterator r( reference, c );
         for ( ; t; ++t, ++r, ++monitor )
         {
            double f1; P1::FromSample( f1, *t );
            if ( f1 > m_rejectLow && f1 < m_rejectHigh )
            {
               double f2; P2::FromSample( f2, *r );
               if ( f2 > m_rejectLow && f2 < m_rejectHigh )
               {
                  F1 << f1;
                  F2 << f2;
               }
            }
         }

         if ( F1.Length() < 3 )
            throw Error( "Insufficient data (channel " + String( c ) + ')' );

         LinearFit L( F1, F2, &monitor );
         if ( !L.IsValid() )
            throw Error( "Invalid linear fit (channel " + String( c ) + ')' );

         fits << L;
         counts << F1.Length();
      }

      monitor.Complete();

      console.WriteLn( "<end><cbr>Linear fit functions:" );
      for ( int c = 0; c < target.NumberOfNominalChannels(); ++c )
      {
         console.WriteLn( String().Format( "y<sub>%d</sub> = %+.6f %c %.6f&middot;x<sub>%d</sub>",
                                           c, fits[c].a, (fits[c].b < 0) ? '-' : '+', Abs( fits[c].b ), c ) );
         console.WriteLn( String().Format( "&sigma;<sub>%d</sub> = %+.6f",
                                           c, fits[c].adev ) );
         console.WriteLn( String().Format( "N<sub>%d</sub> = %6.3f%% (%u)",
                                           c, 100.0*counts[c]/target.NumberOfPixels(), counts[c] ) );
      }
   }

   template <class P1>
   void Fit( const GenericImage<P1>& target, const ImageVariant& reference )
   {
      if ( reference.IsFloatSample() )
         switch ( reference.BitsPerSample() )
         {
         case 32: Fit( target, static_cast<const Image&>( *reference ) ); break;
         case 64: Fit( target, static_cast<const DImage&>( *reference ) ); break;
         }
      else
         switch ( reference.BitsPerSample() )
         {
         case  8: Fit( target, static_cast<const UInt8Image&>( *reference ) ); break;
         case 16: Fit( target, static_cast<const UInt16Image&>( *reference ) ); break;
         case 32: Fit( target, static_cast<const UInt32Image&>( *reference ) ); break;
         }
   }

   template <class P1>
   void Apply( GenericImage<P1>& target )
   {
      if ( fits.Length() < target.NumberOfNominalChannels() )
         throw Error( "LinearFitEngine::Apply(): Internal error: No fitting performed." );

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Applying linear fit", target.NumberOfNominalSamples() );

      for ( int c = 0; c < target.NumberOfNominalChannels(); ++c )
         for ( typename GenericImage<P1>::sample_iterator t( target, c ); t; ++t, ++monitor )
         {
            double f; P1::FromSample( f, *t );
            *t = P1::ToSample( fits[c]( f ) );
         }

      target.Truncate();
   }
};
*/

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::ExecuteOn( View& view )
{
   p_referenceViewId.Trim();
   if ( p_referenceViewId.IsEmpty() )
      throw Error( "No reference view has been specified" );

   View referenceView = View::ViewById( p_referenceViewId );
   if ( referenceView.IsNull() )
      throw Error( "No such view (reference image): " + p_referenceViewId );
   if ( referenceView.IsPreview() && referenceView.Bounds() != referenceView.Window().MainView().Bounds() )
      throw Error( "Cannot use a partial preview as reference: " + p_referenceViewId );
   if ( referenceView == view )
      throw Error( "The reference and target images must be different" );

   if ( view.Bounds() != referenceView.Bounds() || view.IsColor() != referenceView.IsColor() )
      throw Error( "Incompatible image geometry: " + view.Id() );

   AutoViewLock lock1( referenceView, false/*lock*/ );
   lock1.LockForWrite();

   AutoViewLock lock2( view, false/*lock*/ );
   if ( !view.IsPreview() || view.Window() != referenceView.Window() )
      lock2.Lock();

   ImageVariant target = view.Image();
   ImageVariant reference = referenceView.Image();

   Console().EnableAbort();

   LocalImageNormalizationEngine LN( p_scale );
   LN.Build( target, reference );
   LN.Apply( target );

   return true;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::CanExecuteGlobal( String& whyNot ) const
{
   return false;

//    if ( !p_reference.IsDefined() )
//    {
//       whyNot = "No reference image has been specified.";
//       return false;
//    }
//
//    if ( p_targets.IsEmpty() )
//    {
//       whyNot = "No target images have been specified.";
//       return false;
//    }
//
//    return true;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::ExecuteGlobal()
{
   return false;
}

// ----------------------------------------------------------------------------

void* LocalNormalizationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLNScaleParameter )
      return &p_scale;
   if ( p == TheLNReferenceViewIdParameter )
      return p_referenceViewId.Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLNReferenceViewIdParameter )
   {
      p_referenceViewId.Clear();
      if ( sizeOrLength > 0 )
         p_referenceViewId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type LocalNormalizationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheLNReferenceViewIdParameter )
      return p_referenceViewId.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInstance.cpp - Released 2017-05-17T17:41:56Z
