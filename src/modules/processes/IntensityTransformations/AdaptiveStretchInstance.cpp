//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.00.0314
// ----------------------------------------------------------------------------
// AdaptiveStretchInstance.cpp - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "AdaptiveStretchInstance.h"
#include "AdaptiveStretchCurveGraphInterface.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/Matrix.h>
#include <pcl/MuteStatus.h>
#include <pcl/Mutex.h>
#include <pcl/ReferenceArray.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AdaptiveStretchInstance::AdaptiveStretchInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_noiseThreshold( TheASNoiseThresholdParameter->DefaultValue() ),
p_protection( TheASProtectionParameter->DefaultValue() ),
p_useProtection( TheASUseProtectionParameter->DefaultValue() ),
p_maxCurvePoints( TheASMaxCurvePointsParameter->DefaultValue() ),
p_useROI( TheASUseROIParameter->DefaultValue() ),
p_roi( 0 )
{
}

AdaptiveStretchInstance::AdaptiveStretchInstance( const AdaptiveStretchInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInstance::Assign( const ProcessImplementation& p )
{
   const AdaptiveStretchInstance* x = dynamic_cast<const AdaptiveStretchInstance*>( &p );
   if ( x != 0 )
   {
      p_noiseThreshold = x->p_noiseThreshold;
      p_protection     = x->p_protection;
      p_useProtection  = x->p_useProtection;
      p_maxCurvePoints = x->p_maxCurvePoints;
      p_useROI         = x->p_useROI;
      p_roi            = x->p_roi;
   }
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "AdaptiveStretch cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class AdaptiveStretchCurve : public StretchCurve
{
public:

   typedef GenericVector<uint32> force_list;
   typedef Array<force_list>     partial_force_list;

   /*
    * Create a curve with n bins for one channel of an image.
    */
   template <class P>
   AdaptiveStretchCurve( const GenericImage<P>& image, int numberOfBins, const AdaptiveStretchInstance& instance, int channel = 0 ) :
   StretchCurve( numberOfBins )
   {
      /*
       * Sampling region.
       */
      Rect rect = image.Bounds();
      if ( instance.p_useROI )
         if ( instance.p_roi.IsRect() )
         {
            Rect r = instance.p_roi;
            if ( image.Clip( r ) && r.Width() >= 16 && r.Height() >= 16 )
               rect = r;
         }
      /*
       * Reduce the sampling region to exclude one pixel at the left, right and
       * bottom borders of the image.
       */
      rect.x0 = Max( 1, rect.x0 );
      rect.x1 = Min( rect.x1, image.Width()-1 );
      rect.y1 = Min( rect.y1, image.Height()-1 );


      int numberOfThreads = pcl::Thread::NumberOfThreads( rect.Height(), 16 );
      int rowsPerThread = rect.Height()/numberOfThreads;

      size_type N = rect.Area() + (instance.p_useProtection ? 2 : 1)*numberOfThreads*numberOfBins;
      image.Status().Initialize( String().Format( "Computing adaptive stretch curve (%.5g points)", double( numberOfBins ) ), N );

      ThreadData data( image, N );

      data.rect = rect;

      /*
       * Create a bin index matrix. This matrix extends one pixel beyond the
       * left, right and bottom borders of the sampling region, in order to
       * provide 8-connectivity.
       */
      data.binIndex = IMatrix( rect.Height()+1, rect.Width()+2 );
      int n1 = Length() - 1;
      int bm = n1, bM = 0;
      for ( int j = rect.y0, * m = data.binIndex.Begin(); j <= rect.y1; ++j )
      {
         const typename P::sample* p  = image.PixelAddress( rect.x0-1, j, channel );
         const typename P::sample* pN = p + rect.Width()+2;
         for ( ; p < pN; ++p, ++m )
         {
            double f; P::FromSample( f, *p );
            int b = RoundI( n1*f );
            *m = b;
            if ( b < bm )
               bm = b;
            if ( b > bM )
               bM = b;
         }
      }

      /*
       * Create and fire curve builder threads.
       */

      data.channel = channel;
      data.numberOfBins = numberOfBins;
      data.instance = &instance;
      data.status = image.Status();
      data.count = 0;

      ReferenceArray<BuilderThread<P> > builderThreads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         builderThreads.Add( new BuilderThread<P>( data, image,
                                          i*rowsPerThread,
                                          (j < numberOfThreads) ? j*rowsPerThread : rect.Height() ) );

      AbstractImage::RunThreads( builderThreads, data );

      /*
       * Accumulate positive and negative forces from all builderThreads.
       */

      partial_force_list partialPositiveForces( numberOfThreads );
      partial_force_list partialNegativeForces( numberOfThreads );
      for ( int i = 0; i < numberOfThreads; ++i )
      {
         partialPositiveForces[i] = builderThreads[i].positiveForces;
         partialNegativeForces[i] = builderThreads[i].negativeForces;
      }

      builderThreads.Destroy();

      force_list positiveForces( uint32( 0 ), numberOfBins );
      force_list negativeForces( uint32( 0 ), numberOfBins );

      numberOfThreads = Thread::NumberOfThreads( numberOfBins, 64 );
      int binsPerThread = numberOfBins/numberOfThreads;

      ReferenceArray<AccumulatorThread> accumulatorThreads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         accumulatorThreads.Add( new AccumulatorThread( data,
                                          positiveForces, negativeForces,
                                          partialPositiveForces, partialNegativeForces,
                                          i*binsPerThread,
                                          (j < numberOfThreads) ? j*binsPerThread : numberOfBins ) );
      AbstractImage::RunThreads( accumulatorThreads, data );

      accumulatorThreads.Destroy();

      image.Status() = data.status;

      /*
       * Cumulative net forces.
       */
      FVector netForces( numberOfBins );
      if ( instance.p_useProtection )
      {
         netForces[0] = positiveForces[0] - instance.p_protection*negativeForces[0];
         for ( int k = 1; k < numberOfBins; ++k )
            netForces[k] = positiveForces[k] - instance.p_protection*negativeForces[k] + netForces[k-1];
      }
      else
      {
         netForces[0] = positiveForces[0];
         for ( int k = 1; k < numberOfBins; ++k )
            netForces[k] = positiveForces[k] + netForces[k-1];
      }

      /*
       * Compute the transformation curve points.
       */
      for ( int k = 0; k < bm; ++k )
         operator[]( k ) = 0;
      for ( int k = bm; k <= bM; ++k )
         operator[]( k ) = netForces[k] - netForces[bm];
      for ( int k = bM+1; k < numberOfBins; ++k )
         operator[]( k ) = 1;

      /*
       * Ensure that the curve is a strictly growing function in [bm,bM].
       * Curve regularization algorithm by Carlos Milovic F.
       */
      double mindiff = 0;
      for ( int k = bm; k < bM; ++k )
      {
         float d = operator[]( k+1 ) - operator[]( k );
         if ( d < mindiff )
            mindiff = d;
      }
      if ( mindiff < 0 )
      {
         mindiff = -mindiff;
         for ( int k = bm; k <= bM; ++k )
            operator[]( k ) += (k - bm)*(mindiff + 1/(bM - bm));
      }

      /*
       * Normalize curve points to [0,1]
       */
      for ( int k = bm; k <= bM; ++k )
         operator[]( k ) /= operator[]( bM );
   }

   AdaptiveStretchCurve( const AdaptiveStretchCurve& x ) : StretchCurve( x )
   {
   }

   AdaptiveStretchCurve& operator =( const AdaptiveStretchCurve& x )
   {
      FVector::operator =( x );
      return *this;
   }

   double operator ()( double f ) const
   {
      return StretchCurve::operator()( f );
   }

   /*
    * Transforms one channel of an image
    */
   template <class P>
   void operator ()( GenericImage<P>& image ) const
   {
      image.Status().Initialize( "Applying adaptive stretch curve", image.NumberOfNominalSamples() );

      int numberOfThreads = Thread::NumberOfThreads( image.NumberOfPixels(), 64 );
      int pixelsPerThread = image.NumberOfPixels()/numberOfThreads;

      ThreadData data( image, image.NumberOfNominalSamples() );

      ReferenceArray<TransformationThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new TransformationThread<P>( data, image, *this,
                                                   i*pixelsPerThread,
                                                   (j < numberOfThreads) ? j*pixelsPerThread : image.NumberOfPixels() ) );

      AbstractImage::RunThreads( threads, data );

      threads.Destroy();

      image.Status() = data.status;
   }

private:

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const AbstractImage& image, size_type count ) :
      AbstractImage::ThreadData( image, count )
      {
      }

            Rect                     rect;
            int                      channel;
            int                      numberOfBins;
            IMatrix                  binIndex;
      const AdaptiveStretchInstance* instance;
   };

   template<class P>
   class BuilderThread : public Thread
   {
   public:

      force_list positiveForces;
      force_list negativeForces;

      BuilderThread( const ThreadData& data,
                     const GenericImage<P>& image,
                     int begin,
                     int end ) :
      Thread(), m_data( data ), m_image( image ), m_begin( begin ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         positiveForces = force_list( uint32( 0 ), m_data.numberOfBins );
         negativeForces = force_list( uint32( 0 ), m_data.numberOfBins );

         /*
          * Accumulate negative and positive forces computed from pairwise
          * pixel relations using 8-connectivity.
          *
          * N.B. The bin index matrix extends one pixel to the left, right and
          * bottom borders of the sampling region.
          */
         for ( int j = m_begin, w = m_image.Width(), c = m_data.binIndex.Cols(); j < m_end; ++j )
         {
            const typename P::sample* p  = m_image.PixelAddress( m_data.rect.x0, m_data.rect.y0 + j, m_data.channel );
            const typename P::sample* pN = p + m_data.rect.Width();
            const int* m = m_data.binIndex[j] + 1; // See N.B. above

            for ( ; p < pN; ++p, ++m )
            {
               // current pixel
               double f1; P::FromSample( f1, *p );
               int b1 = *m;

               // right pixel
               double f2; P::FromSample( f2, p[1] );
               int b2 = Min( b1, m[1] );

               // bottom right pixel
               double f3; P::FromSample( f3, p[w+1] );
               int b3 = Min( b1, m[c+1] );

               // bottom pixel
               double f4; P::FromSample( f4, p[w] );
               int b4 = Min( b1, m[c] );

               // bottom left pixel
               double f5; P::FromSample( f5, p[w-1] );
               int b5 = Min( b1, m[c-1] );

               if ( pcl::Abs( f1 - f2 ) > m_data.instance->p_noiseThreshold )
                  ++positiveForces[b2];
               else
                  ++negativeForces[b2];

               if ( pcl::Abs( f1 - f3 ) > m_data.instance->p_noiseThreshold )
                  ++positiveForces[b3];
               else
                  ++negativeForces[b3];

               if ( pcl::Abs( f1 - f4 ) > m_data.instance->p_noiseThreshold )
                  ++positiveForces[b4];
               else
                  ++negativeForces[b4];

               if ( pcl::Abs( f1 - f5 ) > m_data.instance->p_noiseThreshold )
                  ++positiveForces[b5];
               else
                  ++negativeForces[b5];
            }

            UPDATE_THREAD_MONITOR( 512 )
         }
      }

   private:

      const ThreadData&      m_data;
      const GenericImage<P>& m_image;
            int              m_begin, m_end;
   };

   class AccumulatorThread : public Thread
   {
   public:

      AccumulatorThread( const ThreadData&         data,
                         force_list&               positiveForces,
                         force_list&               negativeForces,
                         const partial_force_list& partialPositiveForces,
                         const partial_force_list& partialNegativeForces,
                         int                       begin,
                         int                       end ) :
      Thread(),
      m_data( data ),
      m_positiveForces( positiveForces ), m_negativeForces( negativeForces ),
      m_partialPositiveForces( partialPositiveForces ), m_partialNegativeForces( partialNegativeForces ),
      m_begin( begin ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         for ( partial_force_list::const_iterator i = m_partialPositiveForces.Begin(); i < m_partialPositiveForces.End(); ++i )
         {
            force_list::const_iterator f0 = i->At( m_begin );
            force_list::const_iterator fN = i->At( m_end );
            force_list::iterator       f1 = m_positiveForces.At( m_begin );
            for ( ; f0 < fN; ++f0, ++f1 )
            {
               *f1 += *f0;
               UPDATE_THREAD_MONITOR( 1024 )
            }
         }

         if ( m_data.instance->p_useProtection )
            for ( partial_force_list::const_iterator i = m_partialNegativeForces.Begin(); i < m_partialNegativeForces.End(); ++i )
            {
               force_list::const_iterator f0 = i->At( m_begin );
               force_list::const_iterator fN = i->At( m_end );
               force_list::iterator       f1 = m_negativeForces.At( m_begin );
               for ( ; f0 < fN; ++f0, ++f1 )
               {
                  *f1 += *f0;
                  UPDATE_THREAD_MONITOR( 1024 )
               }
            }
      }

   private:

      const ThreadData&         m_data;
            force_list&         m_positiveForces;
            force_list&         m_negativeForces;
      const partial_force_list& m_partialPositiveForces;
      const partial_force_list& m_partialNegativeForces;
            int                 m_begin, m_end;
   };

   template <class P>
   class TransformationThread : public Thread
   {
   public:

      TransformationThread( const ThreadData& data,
                            GenericImage<P>& image,
                            const AdaptiveStretchCurve& curve,
                            int begin,
                            int end ) :
      Thread(), m_data( data ), m_image( image ), m_curve( curve ), m_begin( begin ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         for ( int ch = 0; ch < m_image.NumberOfNominalChannels(); ++ch )
         {
            typename P::sample* p =  m_image[ch] + m_begin;
            typename P::sample* pN = m_image[ch] + m_end;
            for ( ; p < pN; ++p )
            {
               double f; P::FromSample( f, *p );
               *p = P::ToSample( m_curve( f ) );
               UPDATE_THREAD_MONITOR( 1024 )
            }
         }
      }

   private:

      const ThreadData&           m_data;
            GenericImage<P>&      m_image;
      const AdaptiveStretchCurve& m_curve;
            int                   m_begin, m_end;
   };
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class AdaptiveStretchEngine
{
public:

   static StretchCurve Apply( ImageVariant& target, const ImageVariant& source, const AdaptiveStretchInstance& instance )
   {
      AdaptiveStretchCurve curve = GetCurve( source, instance );
      ApplyCurve( target, curve );
      return curve;
   }

private:

   template <class P>
   static AdaptiveStretchCurve GetCurve( const GenericImage<P>& image, int numberOfBins, const AdaptiveStretchInstance& instance )
   {
      const GenericImage<P>* workingImage;

      /*
       * For color images, we compute the transformation curve from the HSI
       * intensity component. Then we apply the same curve to the three nominal
       * RGB channels.
       */
      GenericImage<P> intensity;
      if ( image.IsColor() )
      {
         image.GetIntensity( intensity );
         intensity.Status() = image.Status();
         workingImage = &intensity;
      }
      else
         workingImage = &image;

      AdaptiveStretchCurve curve( *workingImage, numberOfBins, instance );

      if ( image.IsColor() )
         image.Status() = intensity.Status();

      return curve;
   }

   static AdaptiveStretchCurve GetCurve( const ImageVariant& image, const AdaptiveStretchInstance& instance )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: return GetCurve( static_cast<const Image&>( *image ),
                                 Min( instance.p_maxCurvePoints,  10000000 ), instance );
         case 64: return GetCurve( static_cast<const DImage&>( *image ),
                                 Min( instance.p_maxCurvePoints, 100000000 ), instance );
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: return GetCurve( static_cast<const UInt8Image&>( *image ),
                                 Min( instance.p_maxCurvePoints,       256 ), instance );
         case 16: return GetCurve( static_cast<const UInt16Image&>( *image ),
                                 Min( instance.p_maxCurvePoints,     65536 ), instance );
         case 32: return GetCurve( static_cast<const UInt32Image&>( *image ),
                                 Min( instance.p_maxCurvePoints, 100000000 ), instance );
         }

      throw Error( "AdaptiveStretch: Internal error" );
   }

   static void ApplyCurve( ImageVariant& image, const AdaptiveStretchCurve& curve )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: curve( static_cast<Image&>( *image ) ); break;
         case 64: curve( static_cast<DImage&>( *image ) ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: curve( static_cast<UInt8Image&>( *image ) ); break;
         case 16: curve( static_cast<UInt16Image&>( *image ) ); break;
         case 32: curve( static_cast<UInt32Image&>( *image ) ); break;
         }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool AdaptiveStretchInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant target = view.Image();

   if ( target.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   target->SetStatusCallback( &status );

   if ( view.IsPreview() && !view.IsStoredPreview() )
   {
      View mainView = view.Window().MainView();
      ImageVariant source = mainView.Image();
      source->SetStatusCallback( &status );
      AdaptiveStretchEngine::Apply( target, source, *this );
   }
   else
      AdaptiveStretchEngine::Apply( target, target, *this );

   return true;
}

void* AdaptiveStretchInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheASNoiseThresholdParameter )
      return &p_noiseThreshold;
   if ( p == TheASProtectionParameter )
      return &p_protection;
   if ( p == TheASUseProtectionParameter )
      return &p_useProtection;
   if ( p == TheASMaxCurvePointsParameter )
      return &p_maxCurvePoints;
   if ( p == TheASUseROIParameter )
      return &p_useROI;
   if ( p == TheASROIX0Parameter )
      return &p_roi.x0;
   if ( p == TheASROIY0Parameter )
      return &p_roi.y0;
   if ( p == TheASROIX1Parameter )
      return &p_roi.x1;
   if ( p == TheASROIY1Parameter )
      return &p_roi.y1;
   return 0;
}

ProcessInterface* AdaptiveStretchInstance::SelectInterface() const
{
   return Meta()->DefaultInterface();
}

StretchCurve AdaptiveStretchInstance::Preview( UInt16Image& image, const View& view ) const
{
   ImageVariant target( &image );
   ImageVariant source;
   View mainView;

   bool viewLocked = false;

   try
   {
      if ( view.CanWrite() )
      {
         view.LockForWrite();
         viewLocked = true;
      }

      if ( view.IsPreview() && !view.IsStoredPreview() )
      {
         mainView = view.Window().MainView();
         source = mainView.Image();
      }
      else
         source = view.Image();

      source->Status() = image.Status();

      MuteStatus status;
      target.SetStatusCallback( &status );
      source.SetStatusCallback( &status );

      StretchCurve curve = AdaptiveStretchEngine::Apply( target, source, *this );

      target.ResetSelections();

      if ( viewLocked )
         view.Unlock();

      return curve;
   }
   catch ( ... )
   {
      if ( viewLocked )
         view.Unlock();
      throw;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchInstance.cpp - Released 2015/10/08 11:24:40 UTC
