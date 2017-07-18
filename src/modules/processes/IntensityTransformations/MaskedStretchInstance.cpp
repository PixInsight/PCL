//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0397
// ----------------------------------------------------------------------------
// MaskedStretchInstance.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "MaskedStretchInstance.h"
#include "MaskedStretchParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Histogram.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/RGBColorSystem.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MaskedStretchInstance::MaskedStretchInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_targetBackground( TheMSTargetBackgroundParameter->DefaultValue() ),
   p_numberOfIterations( TheMSNumberOfIterationsParameter->DefaultValue() ),
   p_clippingFraction( TheMSClippingFractionParameter->DefaultValue() ),
   p_backgroundReferenceViewId(),
   p_backgroundLow( TheMSBackgroundLowParameter->DefaultValue() ),
   p_backgroundHigh( TheMSBackgroundHighParameter->DefaultValue() ),
   p_useROI( TheMSUseROIParameter->DefaultValue() ),
   p_roi( 0 ),
   p_maskType( MSMaskType::Default )
{
}

// ----------------------------------------------------------------------------

MaskedStretchInstance::MaskedStretchInstance( const MaskedStretchInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void MaskedStretchInstance::Assign( const ProcessImplementation& p )
{
   const MaskedStretchInstance* x = dynamic_cast<const MaskedStretchInstance*>( &p );
   if ( x != nullptr )
   {
      p_targetBackground          = x->p_targetBackground;
      p_numberOfIterations        = x->p_numberOfIterations;
      p_clippingFraction          = x->p_clippingFraction;
      p_backgroundReferenceViewId = x->p_backgroundReferenceViewId;
      p_backgroundLow             = x->p_backgroundLow;
      p_backgroundHigh            = x->p_backgroundHigh;
      p_useROI                    = x->p_useROI;
      p_roi                       = x->p_roi;
      p_maskType                  = x->p_maskType;
   }
}

// ----------------------------------------------------------------------------

bool MaskedStretchInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "MaskedStretch cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template <class P>
static double InitialBackground( const GenericImage<P>& image, double low, double high, double clip )
{
   Array<double> B;
   int n = image.NumberOfNominalChannels();
   bool clipped = clip > 0;
   double c1 = 1 - clip;

   if ( n == 1 )
   {
      for ( typename GenericImage<P>::const_roi_sample_iterator i( image ); i; ++i, ++image.Status() )
      {
         double f; P::FromSample( f, *i );
         if ( clipped )
            f = (f - clip)/c1;
         if ( f > low && f < high )
            B.Append( f );
      }
   }
   else
   {
      for ( typename GenericImage<P>::const_roi_pixel_iterator i( image ); i; ++i, ++image.Status() )
      {
         double f = 0;
         for ( int c = 0; c < n; ++c )
         {
            double v; P::FromSample( v, i[c] );
            if ( clipped )
               v = (v - clip)/c1;
            if ( v <= low || v >= high )
               goto __outOfRange;
            f += v;
         }
         B.Append( f/n );
__outOfRange:
         ;
      }
   }

   return B.IsEmpty() ? 0.0 : pcl::Median( B.Begin(), B.End() );
}

static double InitialBackground( const ImageVariant& image, double low, double high, double clip )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return InitialBackground( static_cast<const Image&>( *image ), low, high, clip );
      case 64: return InitialBackground( static_cast<const DImage&>( *image ), low, high, clip );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return InitialBackground( static_cast<const UInt8Image&>( *image ), low, high, clip );
      case 16: return InitialBackground( static_cast<const UInt16Image&>( *image ), low, high, clip );
      case 32: return InitialBackground( static_cast<const UInt32Image&>( *image ), low, high, clip );
      }

   return 0;
}

// ----------------------------------------------------------------------------

class MaskedStretchEngine
{
public:

   MaskedStretchEngine( ImageVariant& image, const MaskedStretchInstance& instance, double background )
   {
      /*
       * Precompute the sequence of MTF parameters simulating the iterative
       * masked stretch process.
       */
      DVector m( instance.p_numberOfIterations );
      for ( int i = 0; i < instance.p_numberOfIterations; ++i )
      {
         double nextValue = background + (instance.p_targetBackground - background)/(instance.p_numberOfIterations - i);
         m[i] = HistogramTransformation::MTF( nextValue, background );
         background = background*background + (1 - background)*nextValue;
      }

      /*
       * Perform the masked stretch task.
       */
      image.Status().Initialize( "Masked stretch", image.NumberOfPixels()*instance.p_numberOfIterations );

      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<pcl::Image&>( *image ), instance, m ); break;
         case 64: Apply( static_cast<pcl::DImage&>( *image ), instance, m ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: Apply( static_cast<pcl::UInt8Image&>( *image ), instance, m ); break;
         case 16: Apply( static_cast<pcl::UInt16Image&>( *image ), instance, m ); break;
         case 32: Apply( static_cast<pcl::UInt32Image&>( *image ), instance, m ); break;
         }
   }

private:

   template <class P>
   void Apply( GenericImage<P>& image, const MaskedStretchInstance& instance, const DVector& m )
   {
      size_type N = image.NumberOfPixels();
      int numberOfThreads = Thread::NumberOfThreads( N, 16 );
      size_type pixelsPerThread = N/numberOfThreads;

      AbstractImage::ThreadData data( image, image.Status().Total() );

      ReferenceArray<MaskedStretchThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new MaskedStretchThread<P>( instance, data, image, m,
                                                  i*pixelsPerThread,
                                                  (j < numberOfThreads) ? j*pixelsPerThread : N ) );

      AbstractImage::RunThreads( threads, data );

      threads.Destroy();

      image.Status() = data.status;
   }

   template <class P>
   class MaskedStretchThread : public Thread
   {
   public:

      MaskedStretchThread( const MaskedStretchInstance& instance, const AbstractImage::ThreadData& data,
                           GenericImage<P>& image, const DVector& m, size_type start, size_type end ) :
         m_instance( instance ),
         m_data( data ),
         m_image( image ),
         m_m( m ),
         m_start( start ),
         m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         int n = m_image.NumberOfNominalChannels();

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         for ( int it = 0; it < m_instance.p_numberOfIterations; ++it )
         {
            if ( n == 1 )
            {
               typename GenericImage<P>::sample_iterator i( m_image ); i += m_start;
               typename GenericImage<P>::sample_iterator j( m_image ); j += m_end;
               for ( ; i < j; ++i )
               {
                  double f; P::FromSample( f, *i );
                  double f1 = 1 - f;
                  double g = HistogramTransformation::MTF( m_m[it], f );
                  *i = P::ToSample( f*f + f1*g );

                  UPDATE_THREAD_MONITOR( 65536 )
               }
            }
            else
            {
               typename GenericImage<P>::pixel_iterator i( m_image ); i += m_start;
               typename GenericImage<P>::pixel_iterator j( m_image ); j += m_end;
               for ( ; i < j; ++i )
               {
                  DVector v( n );
                  for ( int c = 0; c < n; ++c )
                     P::FromSample( v[c], i[c] );

                  double f;
                  switch ( m_instance.p_maskType )
                  {
                  case MSMaskType::Value:
                     f = rgbws.Value( v[0], v[1], v[2] );
                     break;
                  default:
                  case MSMaskType::Intensity:
                     f = rgbws.Intensity( v[0], v[1], v[2] );
                     break;
                  }

                  double f1 = 1 - f;
                  for ( int c = 0; c < n; ++c )
                  {
                     double g = HistogramTransformation::MTF( m_m[it], v[c] );
                     i[c] = P::ToSample( f*v[c] + f1*g );
                  }

                  UPDATE_THREAD_MONITOR( 65536 )
               }
            }
         }
      }

   private:

      const MaskedStretchInstance&     m_instance;
      const AbstractImage::ThreadData& m_data;
            GenericImage<P>&           m_image;
      const DVector&                   m_m;
            size_type                  m_start;
            size_type                  m_end;
   };

};

bool MaskedStretchInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      throw Error( "Complex images are not supported" );

   if ( p_useROI )
   {
      p_roi.Order();
      if ( !p_roi.IsRect() )
         throw Error( "Empty ROI defined" );
   }

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console console;
   console.EnableAbort();

   double clippingPoint = 0;
   if ( p_clippingFraction > 0 )
   {
      View clipView = view.IsPreview() ? view.Window().MainView() : view;

      AutoViewLock clipLock( clipView, false/*lock*/ );
      if ( clipView.CanWrite() )
         clipLock.LockForWrite();

      ImageVariant clipImage = clipView.Image();
      Histogram H( 0x40000L ); // 256K histogram bins = 2 MiB
      clippingPoint = 1;
      for ( int c = 0; c < clipImage->NumberOfNominalChannels(); ++c )
      {
         clipImage->SelectChannel( c );
         H << clipImage;
         double cc = H.NormalizedClipLow( H.Count( 0 ) + RoundInt64( clipImage->NumberOfPixels()*double( p_clippingFraction ) ) );
         if ( cc < clippingPoint )
            clippingPoint = cc;
      }

      console.WriteLn( String().Format( "<end><cbr>* Clipping point: %.5e", clippingPoint ) );

      HistogramTransformation( 0.5, clippingPoint ) >> image;
   }

   double background;
   {
      View bkgView;

      if ( p_backgroundReferenceViewId.IsEmpty() )
         bkgView = view.Window().MainView();
      else
      {
         bkgView = View::ViewById( p_backgroundReferenceViewId );
         if ( bkgView.IsNull() )
            throw Error( "No such view (background reference): " + p_backgroundReferenceViewId );
      }

      AutoViewLock bkgLock( bkgView, false/*lock*/ );
      if ( bkgView.CanWrite() )
         bkgLock.LockForWrite();

      ImageVariant bkgImage = bkgView.Image();

      if ( p_useROI )
      {
         bkgImage.SelectRectangle( p_roi );
         Rect r = bkgImage.SelectedRectangle();
         if ( !r.IsRect() )
            bkgImage.ResetSelection();
         console.WriteLn( String().Format( "<end><cbr>Region of interest : left=%d, top=%d, width=%d, height=%d",
                                           r.x0, r.y0, r.Width(), r.Height() ) );
      }

      bkgImage.SetStatusCallback( &status );
      bkgImage.Status().Initialize( "Evaluating background", bkgImage.NumberOfSelectedPixels() );

      double bkgClip = 0;
      if ( p_clippingFraction > 0 )
         if ( bkgView != view )
            if ( bkgView.Window() == view.Window() )
               bkgClip = clippingPoint;

      background = InitialBackground( bkgImage, p_backgroundLow, p_backgroundHigh, bkgClip );

      if ( 1 + background <= 1 )
         throw Error( "Black or too dark image - unable to auto stretch" );
      if ( background >= 1 )
         throw Error( "White or too bright image - unable to auto stretch" );
      if ( background >= p_targetBackground )
         throw Error( "The initial background estimate is higher than the target background value" );

      console.WriteLn( String().Format( "<end><cbr>* Initial background: %.5e", background ) );
   }

   MaskedStretchEngine( image, *this, background );

   return true;
}

// ----------------------------------------------------------------------------

void* MaskedStretchInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheMSTargetBackgroundParameter )
      return &p_targetBackground;
   if ( p == TheMSNumberOfIterationsParameter )
      return &p_numberOfIterations;
   if ( p == TheMSClippingFractionParameter )
      return &p_clippingFraction;
   if ( p == TheMSBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Begin();
   if ( p == TheMSBackgroundLowParameter )
      return &p_backgroundLow;
   if ( p == TheMSBackgroundHighParameter )
      return &p_backgroundHigh;
   if ( p == TheMSUseROIParameter )
      return &p_useROI;
   if ( p == TheMSROIX0Parameter )
      return &p_roi.x0;
   if ( p == TheMSROIY0Parameter )
      return &p_roi.y0;
   if ( p == TheMSROIX1Parameter )
      return &p_roi.x1;
   if ( p == TheMSROIY1Parameter )
      return &p_roi.y1;
   if ( p == TheMSMaskTypeParameter )
      return &p_maskType;

   return nullptr;
}

bool MaskedStretchInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheMSBackgroundReferenceViewIdParameter )
   {
      p_backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         p_backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type MaskedStretchInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheMSBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MaskedStretchInstance.cpp - Released 2017-07-18T16:14:18Z
