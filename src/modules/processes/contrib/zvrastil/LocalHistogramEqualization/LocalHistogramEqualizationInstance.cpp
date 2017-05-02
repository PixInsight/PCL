//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 01.00.00.0180
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationInstance.cpp - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2017 Zbynek Vrastil
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#include "LocalHistogramEqualizationInstance.h"
#include "LocalHistogramEqualizationParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/MuteStatus.h>
#include <pcl/ReferenceArray.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LocalHistogramEqualizationInstance::LocalHistogramEqualizationInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
radius( int32( TheLHERadiusParameter->DefaultValue() ) ),
histogramBins( LHEHistogramBins::Default ),
slopeLimit( TheLHESlopeLimitParameter->DefaultValue() ),
amount( TheLHEAmountParameter->DefaultValue() ),
circularKernel( TheLHECircularKernelParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

LocalHistogramEqualizationInstance::LocalHistogramEqualizationInstance( const LocalHistogramEqualizationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInstance::Assign( const ProcessImplementation& p )
{
   const LocalHistogramEqualizationInstance* x = dynamic_cast<const LocalHistogramEqualizationInstance*>( &p );
   if ( x != 0 )
   {
      radius = x->radius;
      histogramBins = x->histogramBins;
      slopeLimit = x->slopeLimit;
      amount = x->amount;
      circularKernel = x->circularKernel;
   }
}

// ----------------------------------------------------------------------------

bool LocalHistogramEqualizationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "LocalHistogramEqualization cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class LocalHistogramEqualizationEngine
{
public:

   static void Apply( ImageVariant& image, const LocalHistogramEqualizationInstance& instance )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32 : Apply( static_cast<Image&>( *image ), instance ); break;
         case 64 : Apply( static_cast<DImage&>( *image ), instance ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8 : Apply( static_cast<UInt8Image&>( *image ), instance ); break;
         case 16 : Apply( static_cast<UInt16Image&>( *image ), instance ); break;
         case 32 : Apply( static_cast<UInt32Image&>( *image ), instance ); break;
         }
   }

   template <class P>
   static void Apply( GenericImage<P>& image, const LocalHistogramEqualizationInstance& instance )
   {
      if ( image.IsColor() )
      {
         Image L;
         image.GetLightness( L );
         L.Status() = image.Status();
         Apply( L, instance );
         image.Status() = L.Status();
         image.SetLightness( L );
         return;
      }

      // create copy of the luminance to evaluate histogram from
      GenericImage<P> imageCopy( image );
      imageCopy.EnsureUnique(); // really not necessary, but we'll be safer if this is done

      size_type N = image.NumberOfPixels();

      int numberOfThreads = Thread::NumberOfThreads( image.Height(), 1 );
      int rowsPerThread = image.Height()/numberOfThreads;

      image.Status().Initialize( "CLAHE", N );

      AbstractImage::ThreadData data( image, N );

      // create processing threads
      ReferenceArray<LocalHistogramEqualizationThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new LocalHistogramEqualizationThread<P>( data,
                                 instance,
                                 image,
                                 imageCopy,
                                 i*rowsPerThread,
                                 (j < numberOfThreads) ? j*rowsPerThread : image.Height() ) );

      AbstractImage::RunThreads( threads, data );

      threads.Destroy();

      image.Status() = data.status;
   }

private:

   // Thread class, performs CLAHE on given range of lines
   template <class P>
   class LocalHistogramEqualizationThread : public Thread
   {
   public:

      // constructor, accepts process instance, destination and source images and pixel range
      LocalHistogramEqualizationThread( const AbstractImage::ThreadData&          data,
                                        const LocalHistogramEqualizationInstance& instance,
                                        GenericImage<P>&                          imageDst,
                                        const GenericImage<P>&                    imageSrc,
                                        int                                       firstRow,
                                        int                                       endRow ) :
      Thread(),
      m_data( data ), m_instance( instance ),
      m_imageDst( imageDst ), m_imageSrc( imageSrc ), m_firstRow( firstRow ), m_endRow( endRow ),
      kernelMask( 0 ), kernelFront( 0 ), kernelBack( 0 ), histogram( 0 ), clippedHistogram( 0 )
      {
         // extract parameters from instance
         histogramSize = m_instance.GetHistogramSize();
         radius = m_instance.GetRadius();
         limit = m_instance.GetLimit();
         factor = (double)(histogramSize-1);

         // allocate histograms
         histogram = new uint32[histogramSize];
         clippedHistogram = new uint32[histogramSize];

         // allocate and build kernel mask (square or circular)
         BuildKernelMask();
      }

      virtual ~LocalHistogramEqualizationThread()
      {
         if ( kernelMask != 0 )
            delete [] kernelMask, kernelMask = 0;
         if ( kernelFront != 0 )
            delete [] kernelFront, kernelFront = 0;
         if ( kernelBack != 0 )
            delete [] kernelBack, kernelBack = 0;
         if ( histogram != 0 )
            delete [] histogram, histogram = 0;
         if ( clippedHistogram != 0 )
            delete [] clippedHistogram, clippedHistogram = 0;
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         // get pointer to image area which we're about to process
         typename P::sample* pL = m_imageDst.PixelAddress( 0, m_firstRow, 0 );

         // iterate over all lines
         for ( int y = m_firstRow; y < m_endRow; y++ )
         {
            // iterate over pixels in current line
            for ( int x = 0; x < m_imageDst.Width(); x++ )
            {
               // get pixel luminance
               RGBColorSystem::sample L;
               P::FromSample( L, *pL );

               // on the beginning of the line, init histogram of pixel neighborhood
               if ( x == 0 )
                  InitHistogram( y );
               // on next pixels, just shift the histogram, adding and removing required pixels
               else
                  AdvanceHistogram( x, y );

               // clip the histogram according to Contrast Limit parameter
               ClipHistogram();

               // transform pixel luminance using Cumulative Distribution Function of clipped histogram
               RGBColorSystem::sample outL = ComputeCDF( L );

               // blend with original luminance according to Amount parameter
               outL = m_instance.GetAmount()*outL + (1 - m_instance.GetAmount())*L;

               // set new pixel color and go to next pixel
               *pL++ = P::ToSample( outL );

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      // allocates and builds kernel mask - 2D array of bool values whether
      // corresponding pixel belongs to kernel or not
      void BuildKernelMask()
      {
         // compute size of the kernel
         kernelSize = radius * 2 -1;

         // allocated square kernel mask
         kernelMask = new bool[kernelSize*kernelSize];

         // allocate list of pixels on the front (right) and back (left) part of the kernel
         // these lists are used to advance the histogram to next pixels
         kernelFront = new int[kernelSize];
         kernelBack = new int[kernelSize];

         // fill in the mask
         for (int y = 0; y < kernelSize; y++)
         {
            bool firstValid = false;
            for (int x = 0; x < kernelSize; x++)
            {
               // compute index in kernel mask array
               int i = y*kernelSize+x;

               // for circular kernel, compute distance of the pixel from center and compare
               // to kernel radius
               if (m_instance.IsCircular())
               {
                  int dx = x - radius + 1;
                  int dy = y - radius + 1;
                  double dist = Sqrt( (double)( dx * dx + dy * dy ) );
                  kernelMask[i] = dist <= (double)(radius-1)+1e-6;
               }
               // for square kernel, all pixels belong to mask
               else kernelMask[i] = true;

               //update kernel front and back indices for this kernel row
               if (!firstValid && kernelMask[i])
               {
                  firstValid = true;
                  kernelFront[y] = x-radius+1;
                  kernelBack[y] = x-radius+1;
               }
               else if (firstValid && kernelMask[i])
               {
                  kernelFront[y] = x-radius+1;
               }
            }
         }
      }

      // computes the histogram for the neighborhood of the pixel at the beginning of given line
      // kernel mask is used to determine which pixels contribute to the histogram
      // image is mirorred on the boundaries in order to keep histogram size constant
      void InitHistogram(int y)
      {
         // zero histogram
         ::memset( histogram, 0, histogramSize * sizeof(uint32) );
         valuesInHistogram = 0;

         // get pointer to image pixels
         const typename P::sample* pL = m_imageSrc[0];
         int r = radius-1;

         // kernel mask is placed with center on the processed pixel
         // iterate through kernel mask with indices relative to center
         for (int ky = -r; ky <= r; ky++)
         {
            // compute y position of the kernel point in the image
            int yy = y+ky;
            // mirror it on boundaries
            if (yy < 0) yy = -yy;
            if (yy >= m_imageSrc.Height()) yy = 2*m_imageSrc.Height()-yy;

            // make sanity check (for images smaller than kernel size)
            if (yy >= 0 && yy < m_imageSrc.Height())
            {
               // compute offset of the processed line in the image
               int offset = yy*m_imageSrc.Width();

               // iterate through kernel mask row with indices relative to center
               for (int kx = -r; kx <= r; kx++)
               {
                  // compute y position of the kernel point in the image
                  int xx = kx;
                  // mirror it on left boundary (that's where we are in this method)
                  if (xx < 0) xx = -xx;

                  // make sanity check (for images smaller than kernel size)
                  if (xx >= 0 && xx < m_imageSrc.Width())
                  {
                     // if the current pixel is part of the kernel
                     if (kernelMask[(ky+r)*kernelSize+kx+r])
                     {
                        // get pixel luminance
                        RGBColorSystem::sample L;
                        P::FromSample( L, *(pL+offset+xx) );

                        // sample it to current histogram resolution
                        uint32 value = Range((uint32)(L*factor), (uint32)0, (uint32)(histogramSize-1));

                        // add it to the histogram
                        histogram[value]++;
                        valuesInHistogram++;
                     }
                  }
               }
            }
         }
      }

      // shifts the histogram from position (x-1, y) to (x, y)
      // it does it by removing pixels on the left of the kernel and adding pixels on the right of the kernel
      // image is mirorred on the boundaries in order to keep histogram size constant
      void AdvanceHistogram(int x, int y)
      {
         // get pointer to image pixels
         const typename P::sample* pL = m_imageSrc[0];

         int r = radius-1;

         // iterate through kernel mask rows with indices relative to center
         for (int ky = -r; ky <= r; ky++)
         {
            // compute y position of the kernel point in the image
            int yy = y+ky;

            // mirror it on boundaries
            if (yy < 0) yy = -yy;
            if (yy >= m_imageSrc.Height()) yy = 2*m_imageSrc.Height()-yy;

            // make sanity check (for images smaller than kernel size)
            if (yy >= 0 && yy < m_imageSrc.Height())
            {
               // compute offset of the processed line in the image
               int offset = yy*m_imageSrc.Width();

               // remove kernel back from histogram, using stored index on this row
               // get x position of the back (left) pixel in the kernel on this row
               int kx = kernelBack[ky+r];

               // compute x position of the kernel point in the image (shifted one pixel to the left)
               int xx = x+kx-1;

               // mirror it on boundaries
               if (xx < 0) xx = -xx;
               if (xx >= m_imageSrc.Width()) xx = 2*m_imageSrc.Height()-xx;

               // make sanity check (for images smaller than kernel size)
               if (xx >= 0 && xx < m_imageSrc.Width())
               {
                  // get pixel luminance
                  RGBColorSystem::sample L;
                  P::FromSample( L, *(pL+offset+xx) );

                  // sample it to current histogram resolution
                  uint32 value = Range((uint32)(L*factor), (uint32)0, (uint32)(histogramSize-1));

                  // remove it from the histogram
                  histogram[value]--;
                  valuesInHistogram--;
               }

               // add kernel front to the histogram
               // get x position of the front (right) pixel in the kernel on this row
               kx = kernelFront[ky+r];

               // compute x position of the kernel point in the image
               xx = x+kx;

               // mirror it on boundaries
               if (xx < 0) xx = -xx;
               if (xx >= m_imageSrc.Width()) xx = 2*m_imageSrc.Height()-xx;

               // make sanity check (for images smaller than kernel size)
               if (xx >= 0 && xx < m_imageSrc.Width())
               {
                  // get pixel luminance
                  RGBColorSystem::sample L;
                  P::FromSample( L, *(pL+offset+xx) );

                  // sample it to current histogram resolution
                  uint32 value = Range((uint32)(L*factor), (uint32)0, (uint32)(histogramSize-1));

                  // add it to the histogram
                  histogram[value]++;
                  valuesInHistogram++;
               }
            }
         }
      }

      // creates clipped version of the histogram
      // the histogram is clipped to ensure maximal required slope of its cumulative distribution function
      // it is done by clipping each value in histogram and redistributing clipped values uniformly over
      // the histogram
      void ClipHistogram()
      {
         // copy current histogram to clipped histogram
         memcpy( clippedHistogram, histogram, histogramSize * sizeof(uint32) );

         int clippedValues = 0;
         int clippedValuesBefore;

         // compute maximal limit for one histogram value from required slope
         int histLimit = ( int )( limit * valuesInHistogram / (histogramSize-1) + 0.5f );
         if (histLimit == 0) histLimit = 1;

         int iterations = 0;

         // start iterative clipping
         do
         {
            clippedValuesBefore = clippedValues;
            clippedValues = 0;

            // clip all values over limit, accumulate amount of clipped values
            for ( uint32 i = 0; i < histogramSize; i++ )
            {
               int32 d = (int32)clippedHistogram[i] - histLimit;
               if ( d > 0 )
               {
                  clippedValues += d;
                  clippedHistogram[i] = histLimit;
               }
            }

            // number of clipped values should be less then in previous iteration
            if ( iterations == 0 || clippedValues < clippedValuesBefore )
            {
               // compute amount to deliver to each value and rest
               int32 d = clippedValues / histogramSize;
               int32 m = clippedValues % histogramSize;
               if ( d != 0 )
               {
                  // distribute clipped values to whole histogram
                  for ( uint32 i = 0; i < histogramSize; i++ )
                     clippedHistogram[i] += d;
               }

               if ( m != 0 )
               {
                  // distribute uniformly the rest
                  int s = (histogramSize - 1) / m;
                  for ( uint32 i = 0; i < histogramSize; i += s )
                     clippedHistogram[i]++;
               }
            }

            iterations++;
         }
         // continue iterations as long as number of clipped values goes down
         while ( iterations == 1 || clippedValues < clippedValuesBefore );
      }

      // compute new lightness value from old one, using cumulative distribution function
      // of clipped histogram
      RGBColorSystem::sample ComputeCDF( RGBColorSystem::sample L )
      {
         // sample value to current histogram resolution
         uint32 value = Range((uint32)(L*factor), (uint32)0, (uint32)(histogramSize-1));

         // find first nonzero value in histogram
         uint32 cdfMin = 0;
         for (uint32 i = 0; i < histogramSize; i++)
         {
            if (clippedHistogram[i] != 0)
            {
               cdfMin = clippedHistogram[i];
               break;
            }
         }

         // compute new value (value of cumulative distribution function for the original luminance value)
         uint32 cdf = 0;
         for (uint32 i = 0; i <= value; i++)
            cdf += clippedHistogram[i];

         // compute total sum of histogram
         uint32 cdfMax = cdf;
         for (uint32 i = value+1; i < histogramSize; i++)
            cdfMax += clippedHistogram[i];

         // rescale result to range of CDF
         return (RGBColorSystem::sample)(cdf-cdfMin)/(RGBColorSystem::sample)(cdfMax-cdfMin);
      }

      const AbstractImage::ThreadData&          m_data;
      const LocalHistogramEqualizationInstance& m_instance;
            GenericImage<P>&                    m_imageDst;
      const GenericImage<P>&                    m_imageSrc;
            int                                 m_firstRow, m_endRow;

            int     kernelSize;
            bool*   kernelMask;
            int*    kernelFront;
            int*    kernelBack;
            uint32  histogramSize;
            uint32* histogram;
            uint32* clippedHistogram;
            uint32  valuesInHistogram;
            int     radius;
            double  limit;
            double  factor;
   };
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInstance::Preview( UInt16Image& image ) const
{
   MuteStatus status;
   image.SetStatusCallback( &status );
   LocalHistogramEqualizationEngine::Apply( image, *this );
   image.ResetSelections();
}

// ----------------------------------------------------------------------------

bool LocalHistogramEqualizationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image->SetStatusCallback( &status );

   LocalHistogramEqualizationEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* LocalHistogramEqualizationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheLHERadiusParameter )
      return &radius;
   if ( p == TheLHEHistogramBinsParameter )
      return &histogramBins;
   if ( p == TheLHESlopeLimitParameter )
      return &slopeLimit;
   if ( p == TheLHECircularKernelParameter )
      return &circularKernel;
   if ( p == TheLHEAmountParameter )
      return &amount;
   return 0;
}

// ----------------------------------------------------------------------------

int LocalHistogramEqualizationInstance::GetHistogramSize() const
{
   switch (histogramBins)
   {
      default:
      case LHEHistogramBins::Bit8: return 1 << 8;
      case LHEHistogramBins::Bit10: return 1 << 10;
      case LHEHistogramBins::Bit12: return 1 << 12;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationInstance.cpp - Released 2017-05-02T09:43:01Z
