//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.00.0320
// ----------------------------------------------------------------------------
// DynamicCropInstance.cpp - Released 2016/11/14 19:38:23 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "DynamicCropInstance.h"
#include "GeometryModule.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/Mutex.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

DynamicCropInstance::DynamicCropInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_center( TheDCCenterXParameter->DefaultValue(), TheDCCenterYParameter->DefaultValue() ),
   p_width( TheDCWidthParameter->DefaultValue() ),
   p_height( TheDCHeightParameter->DefaultValue() ),
   p_angle( TheDCRotationAngleParameter->DefaultValue() ),
   p_scaleX( TheDCScaleXParameter->DefaultValue() ),
   p_scaleY( TheDCScaleYParameter->DefaultValue() ),
   p_optimizeFast( TheDCOptimizeFastRotationsParameter->DefaultValue() ),
   p_interpolation( TheDCInterpolationAlgorithmParameter->Default ),
   p_clampingThreshold( TheDCClampingThresholdParameter->DefaultValue() ),
   p_smoothness( TheDCSmoothnessParameter->DefaultValue() ),
   p_resolution( TheDCXResolutionParameter->DefaultValue(), TheDCYResolutionParameter->DefaultValue() ),
   p_metric( TheDCMetricResolutionParameter->DefaultValue() ),
   p_forceResolution( TheDCForceResolutionParameter->DefaultValue() ),
   p_fillColor( TheDCFillRedParameter->DefaultValue(),
                TheDCFillGreenParameter->DefaultValue(),
                TheDCFillBlueParameter->DefaultValue(),
                TheDCFillAlphaParameter->DefaultValue() )
{
}

DynamicCropInstance::DynamicCropInstance( const DynamicCropInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void DynamicCropInstance::Assign( const ProcessImplementation& p )
{
   const DynamicCropInstance* x = dynamic_cast<const DynamicCropInstance*>( &p );
   if ( x != nullptr )
   {
      p_center            = x->p_center;
      p_width             = x->p_width;
      p_height            = x->p_height;
      p_angle             = x->p_angle;
      p_scaleX            = x->p_scaleX;
      p_scaleY            = x->p_scaleY;
      p_optimizeFast      = x->p_optimizeFast;
      p_interpolation     = x->p_interpolation;
      p_clampingThreshold = x->p_clampingThreshold;
      p_smoothness        = x->p_smoothness;
      p_resolution        = x->p_resolution;
      p_metric            = x->p_metric;
      p_forceResolution   = x->p_forceResolution;
      p_fillColor         = x->p_fillColor;
   }
}

bool DynamicCropInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

UndoFlags DynamicCropInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData | UndoFlag::Keywords | (p_forceResolution ? UndoFlag::Resolution : 0);
}

bool DynamicCropInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "DynamicCrop cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool DynamicCropInstance::BeforeExecution( View& view )
{
   return WarnOnAstrometryMetadataOrPreviewsOrMask( view.Window(), Meta()->Id() );
}

// ----------------------------------------------------------------------------

class DynamicCropEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const DynamicCropInstance& C )
   {
      DynamicCropData<P> data;

      // Dimensions of the target image
      data.m_sourceWidth  = image.Width();
      data.m_sourceHeight = image.Height();

      // Dimensions of the cropped and scaled image
      data.m_width  = Max( 1, RoundInt( C.p_scaleX*C.p_width*data.m_sourceWidth ) );
      data.m_height = Max( 1, RoundInt( C.p_scaleY*C.p_height*data.m_sourceHeight ) );

      // On 32-bit systems, make sure the resulting image requires less than 4 GiB.
      if ( sizeof( void* ) == sizeof( uint32 ) )
      {
         uint64 sz = uint64( data.m_width )*uint64( data.m_height )*image.NumberOfChannels()*image.BytesPerSample();
         if ( sz > uint64( uint32_max-256 ) )
            throw Error( "DynamicCrop: Invalid operation: Target image dimensions would exceed four gigabytes" );
      }

      bool crop = data.m_width != data.m_sourceWidth || data.m_height != data.m_sourceHeight || C.p_center != 0.5;
      bool rotate = C.p_angle != 0;
      data.m_scale = C.p_scaleX != 1 || C.p_scaleY != 1;

      if ( !(crop || rotate || data.m_scale) ) // identity transform ?
      {
         Console().WriteLn( "<end><cbr>&lt;* Identity *&gt;" );
         return;
      }

      // Center of cropping rectangle
      data.m_centerX = C.p_center.x*data.m_sourceWidth;
      data.m_centerY = C.p_center.y*data.m_sourceHeight;

      // Scaling factors
      data.m_scaleX = C.p_scaleX;
      data.m_scaleY = C.p_scaleY;

      double deg = Round( Deg( C.p_angle ), 4 );
      data.m_fastDegrees = int( deg );

      // Type of rotation
      data.m_rotateFast = rotate && C.p_optimizeFast && (deg == 90 || deg == -90 || deg == 180 || deg == -180);
      data.m_rotateSlow = rotate && !data.m_rotateFast;

      bool interpolate = data.m_scale || data.m_rotateSlow;

      if ( data.m_rotateSlow )
      {
         data.m_sa = Sin( C.p_angle );
         data.m_ca = Cos( C.p_angle );
      }

      int numberOfChannels = image.NumberOfChannels();
      int fillChannels = Max( image.NumberOfNominalChannels() + (image.HasAlphaChannels() ? 1 : 0), 4 );

      typename GenericImage<P>::color_space colorSpace = image.ColorSpace();

      typename P::sample** f0 = 0;

      typename P::sample fillValues[ 4 ];
      for ( int c = 0; c < 4; ++c )
         fillValues[c] = P::ToSample( C.p_fillColor[c] );
      if ( !image.IsColor() )
         fillValues[1] = fillValues[3];

      StatusMonitor status = image.Status();

      int numberOfThreads = pcl::Thread::NumberOfThreads( data.m_height, 16 );
      int rowsPerThread = data.m_height/numberOfThreads;

      try
      {
         String info;

         AutoPointer<PixelInterpolation> interpolation;

         if ( interpolate )
         {
            interpolation = NewInterpolation( C.p_interpolation,
                                              data.m_width, data.m_height,
                                              Max( 1, RoundInt( C.p_width*data.m_sourceWidth ) ),
                                              Max( 1, RoundInt( C.p_height*data.m_sourceHeight ) ),
                                              data.m_rotateSlow, C.p_clampingThreshold, C.p_smoothness, (P*)0 );

            info = interpolation->Description() + String().Format( ": %dx%d pixels, %d channels",
                                                                   data.m_width, data.m_height, numberOfChannels );
            if ( rotate )
               info.AppendFormat( ", rotate %+.3f deg", deg );
         }
         else
         {
            info = String().Format( "Crop to %dx%d pixels, %d channels", data.m_width, data.m_height, numberOfChannels );
            if ( rotate )
               info.AppendFormat( ", rotate %+d deg", data.m_fastDegrees );
         }

         size_type N = size_type( numberOfChannels )*data.m_width*data.m_height;
         status.Initialize( info, N );

         f0 = image.ReleaseData();

         for ( int c = 0; c < numberOfChannels; ++c )
         {
            data.m_f = image.Allocator().AllocatePixels( data.m_width, data.m_height );
            data.m_f0 = f0[c];
            data.m_fillValue = (c < fillChannels) ? fillValues[c] : P::MinSampleValue();

            if ( rotate || data.m_scale )
            {
               AbstractImage::ThreadData threadData( status, N );
               ReferenceArray<Thread<P> > threads;
               for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
                  threads.Add( new Thread<P>( threadData, data,
                                              interpolate ? interpolation->NewInterpolator<P>(
                                                      data.m_f0, data.m_sourceWidth, data.m_sourceHeight ) : nullptr,
                                              i*rowsPerThread,
                                              (j < numberOfThreads) ? j*rowsPerThread : data.m_height ) );

               AbstractImage::RunThreads( threads, threadData );
               threads.Destroy();

               status = threadData.status;
            }
            else
            {
               // Simple crop

               typename P::sample* fij = data.m_f;

               double w2 = 0.5*data.m_width;
               double h2 = 0.5*data.m_height;

               Rect r( data.m_width, data.m_height );
               r += Point( TruncInt( data.m_centerX-w2 ), TruncInt( data.m_centerY-h2 ) );

               int y = r.y0;

               for ( ; y < 0 && y < r.y1; ++y, status += data.m_width )
                  for ( int j = 0; j < data.m_width; ++j )
                     *fij++ = data.m_fillValue;

               for ( ; y < data.m_sourceHeight && y < r.y1; ++y, status += data.m_width )
               {
                  int x = r.x0;

                  for ( ; x < 0 && x < r.x1; ++x )
                     *fij++ = data.m_fillValue;

                  int dx = Min( data.m_sourceWidth, r.x1 ) - x;
                  if ( dx > 0 )
                  {
                     ::memcpy( fij, data.m_f0 + size_type( y )*data.m_sourceWidth + x, dx*sizeof( *fij ) );
                     fij += dx;
                     x += dx;
                  }

                  for ( ; x < r.x1; ++x )
                     *fij++ = data.m_fillValue;
               }

               for ( ; y < r.y1; ++y, status += data.m_width )
                  for ( int j = 0; j < data.m_width; ++j )
                     *fij++ = data.m_fillValue;
            }

            image.Allocator().Deallocate( f0[c] );
            f0[c] = data.m_f;
            data.m_f = nullptr;
         }

         image.ImportData( f0, data.m_width, data.m_height, numberOfChannels, colorSpace );
         image.Status() = status;
      }
      catch ( ... )
      {
         if ( data.m_f != nullptr )
            image.Allocator().Deallocate( data.m_f );
         if ( f0 != nullptr )
         {
            for ( int c = 0; c < numberOfChannels; ++c )
               if ( f0[c] != nullptr )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
         }

         image.FreeData();
         throw;
      }
   }

private:

   template <class P>
   struct DynamicCropData
   {
      typename P::sample*       m_f;                  // target pixel data
      const typename P::sample* m_f0;                 // source pixel data
      bool                      m_rotateFast;         // rotate 180 or +/-90
      bool                      m_rotateSlow;         // rotate arbitrary
      bool                      m_scale;              // scale image
      int                       m_fastDegrees;        // fast rotation angle, degrees
      double                    m_sa, m_ca;           // sin( angle ), cos( angle )
      double                    m_centerX, m_centerY; // center x, center y
      double                    m_scaleX, m_scaleY;   // scale x, scale y
      int                       m_sourceWidth, m_sourceHeight; // original width, height
      int                       m_width, m_height;    // target width, height
      typename P::sample        m_fillValue;          // unmapped pixel value
   };

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      Thread( AbstractImage::ThreadData& data,
              const DynamicCropData<P>& cropData,
              PixelInterpolation::Interpolator<P>* interpolator, int startRow, int endRow ) :
         pcl::Thread(),
         m_data( data ), m_cropData( cropData ),
         m_interpolator( interpolator ), m_startRow( startRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         typename P::sample* f = m_cropData.m_f + size_type( m_startRow )*m_cropData.m_width;

         double w2 = 0.5*m_cropData.m_width;
         double h2 = 0.5*m_cropData.m_height;

         for ( int i = m_startRow; i < m_endRow; ++i )
         {
            for ( int j = 0; j < m_cropData.m_width; ++j, ++f )
            {
               double dx, dy;

               if ( m_cropData.m_rotateFast )
               {
                  switch ( m_cropData.m_fastDegrees )
                  {
                  case +90:
                     dx = i - h2;
                     dy = w2 - j;
                     break;
                  case -90:
                     dx = h2 - i;
                     dy = j - w2;
                     break;
                  default:
                  case +180:
                  case -180:
                     dx = w2 - j;
                     dy = h2 - i;
                     break;
                  }
               }
               else
               {
                  dx = j - w2;
                  dy = i - h2;
               }

               if ( m_cropData.m_scale )
               {
                  dx /= m_cropData.m_scaleX;
                  dy /= m_cropData.m_scaleY;
               }

               dx += m_cropData.m_centerX;
               dy += m_cropData.m_centerY;

               if ( m_cropData.m_rotateSlow )
                  Rotate( dx, dy, m_cropData.m_sa, m_cropData.m_ca, m_cropData.m_centerX, m_cropData.m_centerY );

               if ( dx >= 0 && dy >= 0 && dx < m_cropData.m_sourceWidth && dy < m_cropData.m_sourceHeight )
                  *f = m_interpolator.IsValid() ? (*m_interpolator)( dx, dy ) :
                                        m_cropData.m_f0[TruncInt( dy )*m_cropData.m_sourceWidth + TruncInt( dx )];
               else
                  *f = m_cropData.m_fillValue;

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      AbstractImage::ThreadData&                        m_data;
      const DynamicCropData<P>&                         m_cropData;
      AutoPointer<PixelInterpolation::Interpolator<P> > m_interpolator;
      int                                               m_startRow, m_endRow;
   };
};

bool DynamicCropInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;  // should not happen!

   AutoViewLock lock( view );

   ImageWindow window = view.Window();
   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   DeleteAstrometryMetadataAndPreviewsAndMask( window );

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: DynamicCropEngine::Apply( static_cast<Image&>( *image ), *this ); break;
      case 64: DynamicCropEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: DynamicCropEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
      case 16: DynamicCropEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
      case 32: DynamicCropEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
      }

   if ( p_forceResolution )
   {
      Console().WriteLn( String().Format( "Setting resolution: h:%.3lf, v:%.3lf, u:px/%s",
                                          p_resolution.x, p_resolution.y, p_metric ? "cm" : "inch" ) );
      window.SetResolution( p_resolution.x, p_resolution.y, p_metric );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* DynamicCropInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheDCCenterXParameter )
      return &p_center.x;
   if ( p == TheDCCenterYParameter )
      return &p_center.y;
   if ( p == TheDCWidthParameter )
      return &p_width;
   if ( p == TheDCHeightParameter )
      return &p_height;
   if ( p == TheDCRotationAngleParameter )
      return &p_angle;
   if ( p == TheDCScaleXParameter )
      return &p_scaleX;
   if ( p == TheDCScaleYParameter )
      return &p_scaleY;
   if ( p == TheDCOptimizeFastRotationsParameter )
      return &p_optimizeFast;
   if ( p == TheDCInterpolationAlgorithmParameter )
      return &p_interpolation;
   if ( p == TheDCClampingThresholdParameter )
      return &p_clampingThreshold;
   if ( p == TheDCSmoothnessParameter )
      return &p_smoothness;
   if ( p == TheDCXResolutionParameter )
      return &p_resolution.x;
   if ( p == TheDCYResolutionParameter )
      return &p_resolution.y;
   if ( p == TheDCMetricResolutionParameter )
      return &p_metric;
   if ( p == TheDCForceResolutionParameter )
      return &p_forceResolution;
   if ( p == TheDCFillRedParameter )
      return p_fillColor.At( 0 );
   if ( p == TheDCFillGreenParameter )
      return p_fillColor.At( 1 );
   if ( p == TheDCFillBlueParameter )
      return p_fillColor.At( 2 );
   if ( p == TheDCFillAlphaParameter )
      return p_fillColor.At( 3 );
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicCropInstance.cpp - Released 2016/11/14 19:38:23 UTC
