//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0224
// ----------------------------------------------------------------------------
// GREYCstorationInstance.cpp - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#include "GREYCstorationInstance.h"
#include "GREYCstorationParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/View.h>
#include <pcl/StdStatus.h>
#include <pcl/Console.h>
#include <pcl/Thread.h>
#include <pcl/AutoLock.h>
#include <pcl/GlobalSettings.h>

static pcl::Mutex     GREYCstoration_mutex1;
static pcl::Mutex     GREYCstoration_mutex2;
static pcl::size_type GREYCstoration_count = 0;
static bool           GREYCstoration_abort = false;

#define cimg_debug 0
#define cimg_display 0

#define cimg_pixinsight_module_greycstoration 1

#include "CImg/CImg.h"

#undef Status

namespace pcl
{

// ----------------------------------------------------------------------------

GREYCstorationInstance::GREYCstorationInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
amplitude( TheGREYCsAmplitudeParameter->DefaultValue() ),
numberOfIterations( int32( TheGREYCsIterationsParameter->DefaultValue() ) ),
sharpness( TheGREYCsSharpnessParameter->DefaultValue() ),
anisotropy( TheGREYCsAnisotropyParameter->DefaultValue() ),
alpha( TheGREYCsAlphaParameter->DefaultValue() ),
sigma( TheGREYCsSigmaParameter->DefaultValue() ),
fastApproximation( TheGREYCsFastApproximationParameter->DefaultValue() ),
precision( TheGREYCsPrecisionParameter->DefaultValue() ),
spatialStepSize( TheGREYCsSpatialStepSizeParameter->DefaultValue() ),
angularStepSize( TheGREYCsAngularStepSizeParameter->DefaultValue() ),
interpolation( GREYCsInterpolation::Default ),
coupledChannels( TheGREYCsCoupledChannelsParameter->DefaultValue() )
{
}

GREYCstorationInstance::GREYCstorationInstance( const GREYCstorationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void GREYCstorationInstance::Assign( const ProcessImplementation& p )
{
   const GREYCstorationInstance* x = dynamic_cast<const GREYCstorationInstance*>( &p );
   if ( x != 0 )
   {
      amplitude = x->amplitude;
      numberOfIterations = x->numberOfIterations;
      sharpness = x->sharpness;
      anisotropy = x->anisotropy;
      alpha = x->alpha;
      sigma = x->sigma;
      fastApproximation = x->fastApproximation;
      precision = x->precision;
      spatialStepSize = x->spatialStepSize;
      angularStepSize = x->angularStepSize;
      interpolation = x->interpolation;
      coupledChannels = x->coupledChannels;
   }
}

// ----------------------------------------------------------------------------

bool GREYCstorationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "GREYCstoration cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

template <typename T>
class GREYCstorationThread : public Thread
{
public:

   GREYCstorationThread( cimg_library::CImg<T>* aImage,
                         const GREYCstorationInstance& aInstance,
                         int startRow, int endRow ) :
   cimg( aImage ), instance( aInstance ), y0( startRow ), y1( endRow )
   {
   }

   virtual ~GREYCstorationThread()
   {
      if ( cimg != 0 )
         delete cimg, cimg = 0;
   }

   virtual void Run(); // Defined after GREYCstorationEngine

   const cimg_library::CImg<T>* Image() const
   {
      return cimg;
   }

   int StartRow() const
   {
      return y0;
   }

   int EndRow() const
   {
      return y1;
   }

   int NumberOfRows() const
   {
      return y1 - y0;
   }

private:

   cimg_library::CImg<T>* cimg;
   const GREYCstorationInstance& instance;
   int y0, y1;
};

class GREYCstorationEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& img, const GREYCstorationInstance& instance )
   {
      typedef cimg_library::CImg<typename P::sample>   thread_image;
      typedef GREYCstorationThread<typename P::sample> thread;

      /*
       * Size of overlapping regions.
       * The optimal factor of 0.25 has been found by experimentation.
       */
      int d = Max( 6, RoundI( 0.25*instance.amplitude ) );

      /*
       * Processing threads
       */
      IndirectArray<thread> threads;

      /*
       * Temporary storage for alpha channels
       */
      typename P::sample** alphaChannels = 0;

      /*
       * Image geometry, color space and status monitor.
       */
      int w = img.Width();
      int h = img.Height();
      int n = img.NumberOfNominalChannels();
      int n0 = img.NumberOfChannels();
      ImageColor::color_space cs = img.ColorSpace();
      StatusMonitor status = img.Status();

      /*
       * Thread wait time
       */
      uint32 waitTime = 750; // milliseconds

      /*
       * Reset global thread control variables
       */
      ::GREYCstoration_count = 0;
      ::GREYCstoration_abort = false;

      /*
       * Monitoring count
       */
      size_type lastCount = 0;

      try
      {
         /*
          * Number of threads and number of pixel rows per thread
          */
         int numberOfThreads = Thread::NumberOfThreads( h, 2*d );
         int rowsPerThread = h/numberOfThreads;

         /*
          * Initialize status monitor
          */
         size_type N = size_type( (360 - ((360%int( instance.angularStepSize )) >> 1))/instance.angularStepSize );
         N = instance.numberOfIterations * w * (h + numberOfThreads*2*d) * (2 + N);
         if ( P::IsFloatSample() )
            N += instance.numberOfIterations * w*h; // Normalize() at each iteration
         if ( n > 1 && !instance.coupledChannels )
            N *= n;
         status.Initialize( String( "Processing " ) + ((n == 1) ? "grayscale pixel data" :
                                   (instance.coupledChannels ? "multichannel pixel data" :
                                                               "separate color channels" )), N );
         status.DisableInitialization();

         /*
          * Save alpha channels, which are not processed in our implementation of GREYCstoration.
          * We'll restore them after completing GREYCstoration iterations.
          */
         if ( n0 > n )
         {
            alphaChannels = img.Allocator().AllocateChannelSlots( n0 - n );
            for ( int c = n; c < n0; ++c )
               alphaChannels[c-n] = img.PixelData( c );
            img.ForgetAlphaChannels();
         }

         /*
          * GREYCstoration iterations
          */
         for ( int it = 0; it < instance.numberOfIterations; ++it )
         {
            /*
             * Generate threads
             * To avoid border artifacts completely, we regenerate all threads
             * with fresh subimages and overlapping regions at each iteration.
             */
            for ( int i = 0; i < numberOfThreads; ++i )
            {
               /*
                * Starting and end rows for this thread referred to the
                * original image (excluding overlapping regions).
                */
               int y0 = i*rowsPerThread;
               int y1 = (i < numberOfThreads-1) ? (i + 1)*rowsPerThread : h;

               /*
                * Number of non-overlapping pixel rows in this thread
                */
               int hi = y1 - y0;

               /*
                * New thread image
                */
               thread_image* t = new thread_image( w, hi + 2*d, 1, n );

               /*
                * Transfer pixels from the target image
                */
               for ( int c = 0; c < n; ++c )
               {
                  // Top overlapping region
                  for ( int y = 0; y < d; ++y )
                     P::Copy( t->data( 0, y, 0, c ), img.ScanLine( (i == 0) ? d-y-1 : y0-d+y, c ), w );

                  // Non-overlapping region
                  P::Copy( t->data( 0, d, 0, c ), img.ScanLine( y0, c ), w*hi );

                  // Bottom overlapping region
                  for ( int y = 0; y < d; ++y )
                     P::Copy( t->data( 0, d+hi+y, 0, c ), img.ScanLine( (i == numberOfThreads-1) ? h-y-1 : y1+y, c ), w );
               }

               threads.Add( new thread( t, instance, y0, y1 ) );
            }

            /*
             * Deallocate image data to save space.
             * We have all pixel data stored in thread images at this point.
             */
            img.FreeData();

            /*
             * Fire our threads
             */
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i]->Start( ThreadPriority::DefaultMax, i );

            /*
             * Wait for all threads to terminate
             */
            for ( ;; )
            {
               bool someRunning = false;

               /*
                * Look for a running thread
                */
               for ( int i = 0; i < numberOfThreads; ++i )
                  if ( !threads[i]->Wait( waitTime ) )
                  {
                     someRunning = true;
                     break;
                  }

               /*
                * Break when all threads finished
                */
               if ( !someRunning )
                  break;

               /*
                * Update monitor count
                *
                * If the user requests to abort execution, a ProcessAborted
                * exception is thrown here. If this happens, we signal abortion
                * through the GREYCstoration_abort global flag and wait until
                * all threads terminate.
                */
               if ( ::GREYCstoration_mutex2.TryLock() )
               {
                  try
                  {
                     status += ::GREYCstoration_count - lastCount;
                     lastCount = ::GREYCstoration_count;
                  }
                  catch ( ... )
                  {
                     ::GREYCstoration_abort = true;
                  }
                  ::GREYCstoration_mutex2.Unlock();
               }
            }

            /*
             * When no thread is running it is safe to throw an exception to
             * abort the whole process.
             */
            if ( ::GREYCstoration_abort )
               throw ProcessAborted();

            /*
             * Allocate pixel data
             */
            img.AllocateData( w, h, n, cs );

            /*
             * Copy processed pixels, discarding overlapping regions.
             */
            for ( int i = 0; i < numberOfThreads; ++i )
            {
               const thread* t = threads[i];
               for ( int c = 0; c < n; ++c )
                  P::Copy( img.ScanLine( t->StartRow(), c ), t->Image()->data( 0, d, 0, c ), w*t->NumberOfRows() );
            }

            /*
             * Destroy all threads
             */
            threads.Destroy();

            /*
             * Floating point images need a normalization to constrain their
             * sample values to the [0,1] range. This is because the
             * GREYCstoration algorithm can yield values slightly above one
             * (say 1.00001 for example), which invalidates the rescaling to
             * the [0,255] range performed by CImg<T>::blur_anisotropic().
             * Along with that reason, the core PixInsight application expects
             * all real images normalized to the [0,1] range.
             */
            if ( P::IsFloatSample() )
            {
               img.SelectNominalChannels();
               img.Status() = status;
               img.Normalize();
               status = img.Status();
            }
         }

         if ( !status.IsCompleted() )
            status.Complete();

         /*
          * If the image had alpha channels, restore them.
          */
         if ( alphaChannels != 0 )
         {
            for ( int c = n; c < n0; ++c )
            {
               img.AddAlphaChannel( alphaChannels[c-n] );
               alphaChannels[c-n] = 0;
            }
            img.Allocator().Deallocate( alphaChannels );
            alphaChannels = 0;
         }
      }

      catch ( ... )
      {
         threads.Destroy();

         if ( alphaChannels != 0 )
         {
            for ( int c = 0; c < n; ++c )
               if ( alphaChannels[c] != 0 )
                  img.Allocator().Deallocate( alphaChannels[c] );
            img.Allocator().Deallocate( alphaChannels );
         }

         throw;
      }
   }

#define CALL_CImg() \
   cimg->blur_anisotropic( instance.amplitude, instance.sharpness, instance.anisotropy,                        \
                           instance.alpha, instance.sigma, instance.spatialStepSize, instance.angularStepSize, \
                           instance.precision, instance.interpolation, instance.fastApproximation )
   template <class T>
   static void BlurAnisotropic( cimg_library::CImg<T>* cimg, const GREYCstorationInstance& instance )
   {
      if ( cimg->_spectrum == 1 || instance.coupledChannels )
      {
         /*
          * Normal GREYCstoration working mode: process a RGB (or grayscale)
          * image as a single unit.
          */
         CALL_CImg();
      }
      else
      {
         /*
          * Alternative GREYCstoration working mode: process each color channel
          * as an independent image.
          *
          * Using this mode is not recommended on a regular basis because it
          * lacks GREYCstoration's strongest point for color image denoising:
          * manage the relationships between color components as a single
          * mathematical object.
          */
         int n( cimg->_spectrum );
         T* data = cimg->_data;

         cimg->_spectrum = 1;

         for ( int c = 0; c < n; ++c )
         {
            cimg->_data = data + c*cimg->_width*cimg->_height;

            CALL_CImg();

            if ( ::GREYCstoration_abort )
               break;
         }

         cimg->_spectrum = n;
         cimg->_data = data;
      }
   }

#undef CALL_CImg
};

template <class T>
void GREYCstorationThread<T>::Run()
{
   try
   {
      GREYCstorationEngine::BlurAnisotropic( cimg, instance );
   }
   catch ( ... )
   {
   }
}

bool GREYCstorationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: GREYCstorationEngine::Apply( static_cast<pcl::Image&>( *image ), *this ); break;
      case 64: GREYCstorationEngine::Apply( static_cast<pcl::DImage&>( *image ), *this ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: GREYCstorationEngine::Apply( static_cast<pcl::UInt8Image&>( *image ), *this ); break;
      case 16: GREYCstorationEngine::Apply( static_cast<pcl::UInt16Image&>( *image ), *this ); break;
      case 32:
         {
            /*
             * GREYCstoration (as of CImg.h version 1.46) causes overflow
             * of bright pixels in 32-bit unsigned integer format.
             * To work around this problem we create a temporary 64-bit
             * floating point working image. ### TODO: Fix this issue.
             */
            DImage tmp = static_cast<pcl::UInt32Image&>( *image );
            image.FreeImage();
            GREYCstorationEngine::Apply( tmp, *this );
            image.CopyImage( tmp );
         }
         break;
      }

   return true;
}

// ----------------------------------------------------------------------------

void* GREYCstorationInstance::LockParameter( const MetaParameter* p, size_type/*tableRow*/ )
{
   if ( p == TheGREYCsAmplitudeParameter )
      return &amplitude;
   if ( p == TheGREYCsIterationsParameter )
      return &numberOfIterations;
   if ( p == TheGREYCsSharpnessParameter )
      return &sharpness;
   if ( p == TheGREYCsAnisotropyParameter )
      return &anisotropy;
   if ( p == TheGREYCsAlphaParameter )
      return &alpha;
   if ( p == TheGREYCsSigmaParameter )
      return &sigma;
   if ( p == TheGREYCsFastApproximationParameter )
      return &fastApproximation;
   if ( p == TheGREYCsPrecisionParameter )
      return &precision;
   if ( p == TheGREYCsSpatialStepSizeParameter )
      return &spatialStepSize;
   if ( p == TheGREYCsAngularStepSizeParameter )
      return &angularStepSize;
   if ( p == TheGREYCsInterpolationParameter )
      return &interpolation;
   if ( p == TheGREYCsCoupledChannelsParameter )
      return &coupledChannels;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GREYCstorationInstance.cpp - Released 2015/07/31 11:49:48 UTC
