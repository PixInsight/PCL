//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.04.03.0209
// ----------------------------------------------------------------------------
// DebayerInstance.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#include "DebayerInstance.h"
#include "DebayerParameters.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/MuteStatus.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/Version.h>

#define SRC_CHANNEL(c) (m_source.IsColor() ? c : 0)

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * 5x5 B3-spline wavelet scaling function used by the noise estimation routine.
 *
 * Kernel filter coefficients:
 *
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   3.0/128, 3.0/32, 9.0/64,  3.0/32, 3.0/128,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256
 *
 * Note that we use this scaling function as a separable filter (row and column
 * vectors) for performance reasons.
 */
// Separable filter coefficients
const float __5x5B3Spline_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
// Gaussian noise scaling factors
const float __5x5B3Spline_kj[] =
{ 0.8907F, 0.2007F, 0.0856F, 0.0413F, 0.0205F, 0.0103F, 0.0052F, 0.0026F, 0.0013F, 0.0007F };

// ----------------------------------------------------------------------------

static IsoString ValidFullId( const IsoString& id )
{
   IsoString validId( id );
   validId.ReplaceString( "->", "_" );
   return validId;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DebayerInstance::DebayerInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_bayerPattern( DebayerBayerPatternParameter::Default ),
p_debayerMethod( DebayerMethodParameter::Default ),
p_evaluateNoise( TheDebayerEvaluateNoiseParameter->DefaultValue() ),
p_noiseEvaluationAlgorithm( DebayerNoiseEvaluationAlgorithm::Default ),
p_showImages( TheDebayerShowImagesParameter->DefaultValue() ),
o_imageId(),
o_noiseEstimates( 0.0F, 3 ),
o_noiseFractions( 0.0F, 3 ),
o_noiseAlgorithms( 3 )
{
}

DebayerInstance::DebayerInstance( const DebayerInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void DebayerInstance::Assign( const ProcessImplementation& p )
{
   const DebayerInstance* x = dynamic_cast<const DebayerInstance*>( &p );
   if ( x != 0 )
   {
      p_bayerPattern             = x->p_bayerPattern;
      p_debayerMethod            = x->p_debayerMethod;
      p_evaluateNoise            = x->p_evaluateNoise;
      p_noiseEvaluationAlgorithm = x->p_noiseEvaluationAlgorithm;
      p_showImages               = x->p_showImages;

      o_imageId                  = x->o_imageId;
      o_noiseEstimates           = x->o_noiseEstimates;
      o_noiseFractions           = x->o_noiseFractions;
      o_noiseAlgorithms          = x->o_noiseAlgorithms;
   }
}

bool DebayerInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
      whyNot = "Debayer cannot be executed on complex images.";
   else if ( view.Image().Width() < 6 || view.Image().Height() < 6 )
      whyNot = "Debayer needs an image of at least 6 by 6 pixels";
   else
   {
      whyNot.Clear();
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerEngine
{
public:

   template <class P>
   static void SuperPixelThreaded( Image& target, const GenericImage<P>& source, const DebayerInstance& instance )
   {
      int target_w = source.Width() >> 1;
      int target_h = source.Height() >> 1;

      target.AllocateData( target_w, target_h, 3, ColorSpace::RGB );

      target.Status().Initialize( "SuperPixel debayering", target_h );

      int numberOfThreads = Thread::NumberOfThreads( target_h, 1 );
      int rowsPerThread = target_h/numberOfThreads;

      AbstractImage::ThreadData data( target, target_h );

      ReferenceArray<SuperPixelThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SuperPixelThread<P>( data, target, source, instance,
                                               i*rowsPerThread,
                                               (j < numberOfThreads) ? j*rowsPerThread : target_h ) );

      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      target.Status() = data.status;
   }

   template <class P>
   static void BilinearThreaded( Image& target, const GenericImage<P>& source, const DebayerInstance& instance )
   {
      int target_w = source.Width();
      int target_h = source.Height();

      target.AllocateData( target_w, target_h, 3, ColorSpace::RGB );

      target.Status().Initialize( "Bilinear debayering", target_h-2 );

      int numberOfThreads = Thread::NumberOfThreads( target_h-2, 1 );
      int rowsPerThread = (target_h - 2)/numberOfThreads;

      AbstractImage::ThreadData data( target, target_h-2 );

      ReferenceArray<BilinearThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new BilinearThread<P>( data, target, source, instance,
                                             i*rowsPerThread + 1,
                                             (j < numberOfThreads) ? j*rowsPerThread + 1 : target_h-1 ) );

      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      // copy top and bottom rows from the adjecent ones
      for ( int col = 0; col < target_w; col++ )
         for ( int i = 0; i < 3; i++ )
         {
            target.Pixel( col, 0, i ) = target.Pixel( col, 1, i );
            target.Pixel( col, target_h-1, i ) = target.Pixel( col, target_h-2, i );
         }

      target.Status() = data.status;
   }

   template <class P>
   static void VNGThreaded( Image& target, const GenericImage<P>& source, const DebayerInstance& instance )
   {
      int target_w = source.Width();
      int target_h = source.Height();

      target.AllocateData( target_w, target_h, 3, ColorSpace::RGB );

      target.Status().Initialize( "VNG debayering", target_h-4 );

      int numberOfThreads = Thread::NumberOfThreads( target_h-4, 1 );
      int rowsPerThread = (target_h - 4)/numberOfThreads;

      AbstractImage::ThreadData data( target, target_h-4 );

      ReferenceArray<VNGThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new VNGThread<P>( data, target, source, instance,
                                        i*rowsPerThread + 2,
                                        (j < numberOfThreads) ? j*rowsPerThread + 2 : target_h-2 ) );

      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      // copy top and bottom two rows from the adjecent ones
      for ( int col = 0; col < target_w; col++ )
         for ( int i = 0; i < 3; i++ )
         {
            target.Pixel( col, 0, i ) = target.Pixel( col, 1, i ) = target.Pixel( col, 2, i );
            target.Pixel( col, target_h-1, i ) = target.Pixel( col, target_h-2, i ) = target.Pixel( col, target_h-3, i );
         }

      target.Status() = data.status;
   }

   static void EvaluateNoise( FVector& noiseEstimates,
                              FVector& noiseFractions,
                              StringList& noiseAlgorithms,
                              const Image& image, int algorithm )
   {
      SpinStatus spin;
      image.SetStatusCallback( &spin );
      image.Status().Initialize( "Noise evaluation" );
      image.Status().DisableInitialization();

      Console console;

      int numberOfThreads = Thread::NumberOfThreads( image.NumberOfPixels(), 1 );
      if ( numberOfThreads >= 3 )
      {
         int numberOfSubthreads = RoundI( numberOfThreads/3.0 );
         ReferenceArray<NoiseEvaluationThread> threads;
         threads.Add( new NoiseEvaluationThread( image, 0, algorithm, numberOfSubthreads ) );
         threads.Add( new NoiseEvaluationThread( image, 1, algorithm, numberOfSubthreads ) );
         threads.Add( new NoiseEvaluationThread( image, 2, algorithm, numberOfThreads - 2*numberOfSubthreads ) );

         AbstractImage::ThreadData data( image, 0 ); // unbounded
         AbstractImage::RunThreads( threads, data );

         for ( int i = 0; i < 3; ++i )
         {
            noiseEstimates[i]  = threads[i].noiseEstimate;
            noiseFractions[i]  = threads[i].noiseFraction;
            noiseAlgorithms[i] = String( threads[i].noiseAlgorithm );
         }

         threads.Destroy();
      }
      else
      {
         for ( int i = 0; i < 3; ++i )
         {
            NoiseEvaluationThread thread( image, i, algorithm, 1 );
            thread.Run();
            noiseEstimates[i]  = thread.noiseEstimate;
            noiseFractions[i]  = thread.noiseFraction;
            noiseAlgorithms[i] = String( thread.noiseAlgorithm );
         }
      }

      image.ResetSelections();
      image.Status().Complete();

      console.WriteLn( "<end><cbr>Gaussian noise estimates:" );
      for ( int i = 0; i < 3; ++i )
         console.WriteLn( String().Format( "s%d = %.3e, n%d = %.4f ",
                              i, noiseEstimates[i], i, noiseFractions[i] ) + '(' + noiseAlgorithms[i] + ')' );
   }

private:

   template <class P>
   class DebayerThreadBase : public Thread
   {
   public:

      DebayerThreadBase( const AbstractImage::ThreadData& data,
                         Image& target, const GenericImage<P>& source,
                         const DebayerInstance& instance, int start, int end ) :
      Thread(), m_data( data ), m_target( target ), m_source( source ), m_instance( instance ), m_start( start ), m_end( end )
      {
      }

   protected:

      const AbstractImage::ThreadData& m_data;
            Image&                     m_target;
      const GenericImage<P>&           m_source;
      const DebayerInstance&           m_instance;
            int                        m_start, m_end;
   };

#define m_target   this->m_target
#define m_source   this->m_source
#define m_instance this->m_instance
#define m_start    this->m_start
#define m_end      this->m_end

   template <class P>
   class SuperPixelThread : public DebayerThreadBase<P>
   {
   public:

      SuperPixelThread( const AbstractImage::ThreadData& data,
                        Image& target, const GenericImage<P>& source, const DebayerInstance& instance, int start, int end ) :
      DebayerThreadBase<P>( data, target, source, instance, start, end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const int src_w2 = m_source.Width() >> 1;

         for ( int row = m_start; row < m_end; row++ )
         {
            for ( int col = 0; col < src_w2; col++ )
            {
               int red_col, red_row, green_col1, green_col2, green_row1, green_row2, blue_row, blue_col;
               switch( m_instance.p_bayerPattern )
               {
               default:
               case DebayerBayerPatternParameter::RGGB:
                  red_col = (col * 2);
                  red_row = (row * 2);
                  green_col1 = (col * 2) + 1;
                  green_row1 = row * 2;
                  green_col2 = col * 2;
                  green_row2 = (row * 2) + 1;
                  blue_col = col * 2 + 1;
                  blue_row = row * 2 + 1;
                  break;
               case DebayerBayerPatternParameter::BGGR:
                  red_col = (col * 2) + 1;
                  red_row = (row * 2) + 1;
                  green_col1 = (col * 2) + 1;
                  green_row1 = row * 2;
                  green_col2 = col * 2;
                  green_row2 = (row * 2) + 1;
                  blue_col = col * 2;
                  blue_row = row * 2;
                  break;
               case DebayerBayerPatternParameter::GBRG:
                  red_col = (col * 2);
                  red_row = (row * 2) + 1;
                  green_col1 = (col * 2);
                  green_row1 = row * 2;
                  green_col2 = (col * 2) + 1;
                  green_row2 = (row * 2) + 1;
                  blue_col = (col * 2) + 1;
                  blue_row = row * 2;
                  break;
               case DebayerBayerPatternParameter::GRBG:
                  red_col = (col * 2) + 1;
                  red_row = (row * 2);
                  green_col1 = (col * 2);
                  green_row1 = (row * 2);
                  green_col2 = (col * 2) + 1;
                  green_row2 = (row * 2) + 1;
                  blue_col = (col * 2);
                  blue_row = (row * 2) + 1;
                  break;
               }

               // red
               P::FromSample( m_target.Pixel( col, row, 0 ), m_source.Pixel( red_col, red_row, SRC_CHANNEL( 0 ) ) );
               //green
               double v1, v2;
               P::FromSample( v1, m_source.Pixel( green_col1, green_row1, SRC_CHANNEL( 1 ) ) );
               P::FromSample( v2, m_source.Pixel( green_col2, green_row2, SRC_CHANNEL( 1 ) ) );
               m_target.Pixel( col, row, 1 ) = (v1 + v2)/2;
               // blue
               P::FromSample( m_target.Pixel( col, row, 2 ), m_source.Pixel( blue_col, blue_row, SRC_CHANNEL( 2 ) ) );
            }

            UPDATE_THREAD_MONITOR( 16 )
         }
      }
   }; // SuperPixelThread

   template <class P>
   class BilinearThread : public DebayerThreadBase<P>
   {
   public:

      BilinearThread( const AbstractImage::ThreadData& data,
                      Image& target, const GenericImage<P>& source, const DebayerInstance& instance, int start, int end ) :
      DebayerThreadBase<P>( data, target, source, instance, start, end )
      {
      }

      virtual void Run()
      {
         /*
          * http://winfij.homeip.net/maximdl/bilineardebayer.html
          */

         INIT_THREAD_MONITOR()

         const int src_w = m_source.Width();
         int colors[ 2 ][ 2 ]; // [row][col]

         switch ( m_instance.p_bayerPattern )
         {
         default:
         case DebayerBayerPatternParameter::RGGB:
            colors[0][0] = 0;
            colors[0][1] = 1;
            colors[1][0] = 1;
            colors[1][1] = 2;
            break;
         case DebayerBayerPatternParameter::BGGR:
            colors[0][0] = 2;
            colors[0][1] = 1;
            colors[1][0] = 1;
            colors[1][1] = 0;
            break;
         case DebayerBayerPatternParameter::GBRG:
            colors[0][0] = 1;
            colors[0][1] = 2;
            colors[1][0] = 0;
            colors[1][1] = 1;
            break;
         case DebayerBayerPatternParameter::GRBG:
            colors[0][0] = 1;
            colors[0][1] = 0;
            colors[1][0] = 2;
            colors[1][1] = 1;
            break;
         }

         for ( int row = m_start; row < m_end; row++ )
         {
            // skip the first and last column
            for ( int col = 1; col < src_w-1; col++ )
            {
               //color_index = (row % 2) + (col % 2); // modulo division
               int current_color = colors[(row % 2)][(col % 2)];
               int next_color = colors[(row % 2)][((col+1) % 2)];
               double c1, c2, v1, v2, v3, v4, target_colors[ 3 ];
               //straight copy of the current color
               P::FromSample( target_colors[current_color], m_source.Pixel( col, row, SRC_CHANNEL( current_color ) ) );
               switch ( current_color )
               {
               case 0: // red done
               case 2: // blue done
                  // get green samples
                  P::FromSample( v2, m_source.Pixel( col, row + 1, SRC_CHANNEL( 1 ) ) );
                  P::FromSample( v1, m_source.Pixel( col, row - 1, SRC_CHANNEL( 1 ) ) );
                  P::FromSample( v3, m_source.Pixel( col - 1, row, SRC_CHANNEL( 1 ) ) );
                  P::FromSample( v4, m_source.Pixel( col + 1, row, SRC_CHANNEL( 1 ) ) );
                  c1 = (v1 + v2 + v3 + v4)/4;
                  target_colors[1] = c1;
                  // get blue or red samples
                  P::FromSample( v1, m_source.Pixel( col - 1, row - 1, SRC_CHANNEL( 2-current_color ) ) );
                  P::FromSample( v2, m_source.Pixel( col + 1, row + 1, SRC_CHANNEL( 2-current_color ) ) );
                  P::FromSample( v3, m_source.Pixel( col - 1, row + 1, SRC_CHANNEL( 2-current_color ) ) );
                  P::FromSample( v4, m_source.Pixel( col + 1, row - 1, SRC_CHANNEL( 2-current_color ) ) );
                  c1 = (v1 + v2 + v3 + v4)/4;
                  // if the current color is red then we just grabbed blue and vise versa
                  target_colors[2-current_color] = c1;
                  break;

               case 1: // green already done
                  P::FromSample( v1, m_source.Pixel( col, row - 1, SRC_CHANNEL( 2-next_color ) ) );
                  P::FromSample( v2, m_source.Pixel( col, row + 1, SRC_CHANNEL( 2-next_color ) ) );
                  P::FromSample( v3, m_source.Pixel( col - 1, row, SRC_CHANNEL( next_color ) ) ); // next color
                  P::FromSample( v4, m_source.Pixel( col + 1, row, SRC_CHANNEL( next_color ) ) );
                  c1 = (v1 + v2)/2;
                  c2 = (v3 + v4)/2;
                  // if the current color is red then we just grabbed blue and vise versa
                  target_colors[next_color] = c2;
                  target_colors[2-next_color] = c1;
                  break;
               }

               for ( int i = 0; i < 3; i++ )
                  m_target.Pixel( col, row, i ) = target_colors[i];
            }

            // get colors for the inner and outer column
            for ( int i = 0; i < 3; i++ )
            {
               m_target.Pixel( 0, row, i ) = m_target.Pixel( 1, row, i );
               m_target.Pixel( src_w - 1, row, i ) = m_target.Pixel( src_w-2, row, i );
            }

            UPDATE_THREAD_MONITOR( 16 )
         }
      }
   }; // BilinearThread

   template <class P>
   class VNGThread : public DebayerThreadBase<P>
   {
   public:

      VNGThread( const AbstractImage::ThreadData& data,
                 Image& target, const GenericImage<P>& source, const DebayerInstance& instance, int start, int end ) :
      DebayerThreadBase<P>( data, target, source, instance, start, end )
      {
      }

      virtual void Run()
      {
         // http://openfmi.net/plugins/scmsvn/cgi-bin/viewcvs.cgi/*checkout*/books/Chang.pdf?content-type=text%2Fplain&rev=15&root=interpol

         INIT_THREAD_MONITOR()

         const int src_w = m_source.Width();
         int colors[ 2 ][ 2 ]; // [row][col]

         // store channel indices according to selected bayer pattern
         switch ( m_instance.p_bayerPattern )
         {
         default:
         case DebayerBayerPatternParameter::RGGB:
            colors[0][0] = 0;
            colors[0][1] = 1;
            colors[1][0] = 1;
            colors[1][1] = 2;
            break;
         case DebayerBayerPatternParameter::BGGR:
            colors[0][0] = 2;
            colors[0][1] = 1;
            colors[1][0] = 1;
            colors[1][1] = 0;
            break;
         case DebayerBayerPatternParameter::GBRG:
            colors[0][0] = 1;
            colors[0][1] = 2;
            colors[1][0] = 0;
            colors[1][1] = 1;
            break;
         case DebayerBayerPatternParameter::GRBG:
            colors[0][0] = 1;
            colors[0][1] = 0;
            colors[1][0] = 2;
            colors[1][1] = 1;
            break;
         }

         // iterate over all rows assigned to this thread
         for ( int row = m_start; row < m_end; row++ )
         {
            // skip two first and last columns
            for ( int col = 2; col < src_w-2; col++ )
            {
               // cache matrix of 5x5 pixels around current pixel
               double v[ 25 ];     // values of 5x5 pixels of bayered image, centered on current pixel
               int channels[ 25 ]; // channel indices for 5x5 pixels
               if ( col == 2 )
               {
                  // for first column, read whole matrix
                  for ( int y = 0; y < 5; y++ )
                     for ( int x = 0; x < 5; x++ )
                     {
                        int c = col+x-2;
                        int r = row+y-2;
                        channels[y*5+x] = colors[r%2][c%2];
                        P::FromSample( v[y*5+x], m_source.Pixel( c, r, SRC_CHANNEL( channels[y*5+x] ) ) );
                     }
               }
               else
               {
                  // for other columns, shift the matrix to the left and read just last column
                  for ( int y = 0; y < 5; y++ )
                  {
                     for (int x = 0; x < 4; x++)
                     {
                        channels[y*5+x] = channels[y*5+x+1];
                        v[y*5+x] = v[y*5+x+1];
                     }
                     channels[y*5+4] = colors[(row+y-2)%2][(col+2)%2];
                     P::FromSample( v[y*5+4], m_source.Pixel( col+2, row+y-2, SRC_CHANNEL( channels[y*5+4] ) ) );
                  }
               }

               double gradients[ 8 ];     // gradients in N, E, S, W, NE, SW, NW and SE directions
               int valid_gradients[ 8 ];  // list of gradients below computed threshold (valid for color computation)
               int valid_gradients_count; // number of valid gradients
               double value_sums[ 3 ];    // sums of color coefficients for all valid gradients

               // get channel for actual pixel from bayer pattern
               int current_channel = colors[row % 2][col % 2];

               // compute gradients in eight directions
               ComputeGradients( v, current_channel, gradients );

               // compute threshold and list of gradients below the threshold
               ThresholdGradients( gradients, valid_gradients, valid_gradients_count );

               // compute sums of color coefficients for each of valid gradients
               ComputeSums( v, channels, gradients, valid_gradients, valid_gradients_count, value_sums );

               // current_channel hold index of the channel of current pixels
               // get indices of two remaining channels
               int other_channel1 = 0, other_channel2 = 0;
               switch ( current_channel )
               {
               case 0:
                  other_channel1 = 1;
                  other_channel2 = 2;
                  break;
               case 1:
                  other_channel1 = 0;
                  other_channel2 = 2;
                  break;
               case 2:
                  other_channel1 = 0;
                  other_channel2 = 1;
                  break;
               }

               // compute normalized color differences for remaining channels
               double diff1 = (value_sums[other_channel1] - value_sums[current_channel])/double( valid_gradients_count );
               double diff2 = (value_sums[other_channel2] - value_sums[current_channel])/double( valid_gradients_count );

               // compute current pixel color
               // current channel is directly known from bayered image (take the center of the cached matrix)
               m_target.Pixel( col, row, current_channel ) = v[12];
               // two remaining channels are computed using normalized differences
               m_target.Pixel( col, row, other_channel1 ) = Range( v[12]+diff1, 0.0, 1.0 );
               m_target.Pixel( col, row, other_channel2 ) = Range( v[12]+diff2, 0.0, 1.0 );
            }

            // get colors for the inner and outer two columns
            for ( int i = 0; i < 3; i++ )
            {
               m_target.Pixel( 0, row, i ) = m_target.Pixel( 2, row, i );
               m_target.Pixel( 1, row, i ) = m_target.Pixel( 2, row, i );
               m_target.Pixel( src_w - 1, row, i ) = m_target.Pixel( src_w - 3, row, i );
               m_target.Pixel( src_w - 2, row, i ) = m_target.Pixel( src_w - 3, row, i );
            }

            UPDATE_THREAD_MONITOR( 16 )
         }
      }

   private:

      // compute gradients in eight directions around current pixel
      void ComputeGradients( const double* v, int current_channel, double* gradients )
      {
         // compute gradients in eight directions
         // formulas taken directly from VNG method paper
         const int G_N = 0;
         const int G_E = 1;
         const int G_S = 2;
         const int G_W = 3;
         const int G_NE = 4;
         const int G_SE = 5;
         const int G_NW = 6;
         const int G_SW = 7;

         // compute vertical and horizontal gradients
         gradients[G_N]  = Abs( v[7]  - v[17] )   + Abs( v[2]  - v[12] )   + Abs( v[6]  - v[16] )/2 +
                           Abs( v[8]  - v[18] )/2 + Abs( v[1]  - v[11] )/2 + Abs( v[3]  - v[13] )/2;
         gradients[G_E]  = Abs( v[13] - v[11] )   + Abs( v[14] - v[12] )   + Abs( v[8]  - v[6]  )/2 +
                           Abs( v[18] - v[16] )/2 + Abs( v[9]  - v[7]  )/2 + Abs( v[19] - v[17] )/2;
         gradients[G_S]  = Abs( v[17] - v[7]  )   + Abs( v[22] - v[12] )   + Abs( v[16] - v[6]  )/2 +
                           Abs( v[18] - v[8]  )/2 + Abs( v[21] - v[11] )/2 + Abs( v[23] - v[13] )/2;
         gradients[G_W]  = Abs( v[11] - v[13] )   + Abs( v[10] - v[12] )   + Abs( v[6]  - v[8]  )/2 +
                           Abs( v[16] - v[18] )/2 + Abs( v[5]  - v[7]  )/2 + Abs( v[15] - v[17] )/2;

         // diagonal gradients differ for green channel and other channels
         switch ( current_channel )
         {
         // green center
         default: // just to shut down '-Wmaybe-uninitialized' warnings
         case 1:
            gradients[G_NE] = Abs( v[8]  - v[16] ) + Abs( v[4]  - v[12] ) +
                              Abs( v[3]  - v[11] ) + Abs( v[9]  - v[17] );
            gradients[G_SE] = Abs( v[18] - v[6]  ) + Abs( v[24] - v[12] ) +
                              Abs( v[23] - v[11] ) + Abs( v[19] - v[7]  );
            gradients[G_NW] = Abs( v[6]  - v[18] ) + Abs( v[0]  - v[12] ) +
                              Abs( v[1]  - v[13] ) + Abs( v[5]  - v[17] );
            gradients[G_SW] = Abs( v[16] - v[8]  ) + Abs( v[20] - v[12] ) +
                              Abs( v[21] - v[13] ) + Abs( v[15] - v[7]  );
            break;
         // red or blue center
         case 0:
         case 2:
            gradients[G_NE] = Abs( v[8]  - v[16] )   + Abs( v[4]  - v[12] )   + Abs( v[7]  - v[11] )/2 +
                              Abs( v[13] - v[17] )/2 + Abs( v[3]  - v[7]  )/2 + Abs( v[9]  - v[13] )/2;
            gradients[G_SE] = Abs( v[18] - v[6]  )   + Abs( v[24] - v[12] )   + Abs( v[13] - v[7]  )/2 +
                              Abs( v[17] - v[11] )/2 + Abs( v[19] - v[13] )   + Abs( v[23] - v[17] )/2;
            gradients[G_NW] = Abs( v[6]  - v[18] )   + Abs( v[0]  - v[12] )   + Abs( v[7]  - v[13] )/2 +
                              Abs( v[11] - v[17] )/2 + Abs( v[1]  - v[7]  )/2 + Abs( v[5]  - v[11] )/2;
            gradients[G_SW] = Abs( v[16] - v[8]  )   + Abs( v[20] - v[12] )   + Abs( v[11] - v[7]  )/2 +
                              Abs( v[17] - v[13] )/2 + Abs( v[15] - v[11] )   + Abs( v[21] - v[17] )/2;
            break;
         }
      }

      // compute threshold and list of gradients below the threshold
      void ThresholdGradients( const double* gradients, int* valid_gradients, int& count )
      {
         // get minimal and maximal gradient
         double min = gradients[0], max = gradients[0];
         for ( int i = 1; i < 8; i++ )
         {
            if ( gradients[i] < min )
               min = gradients[i];
            if ( gradients[i] > max )
               max = gradients[i];
         }

         // compute threshold
         // k1 and k2 coefficient values are taken from VNG method paper (empirically found to produce best results)
         const double k1 = 1.5;
         const double k2 = 0.5;
         double threshold = k1*min + k2*(max - min);

         // find valid gradients (below threshold)
         count = 0;
         for ( int i = 0; i < 8; i++ )
            if ( gradients[i] <= threshold+1e-10 )
               valid_gradients[count++] = i;
      }

      // compute sums of averaged color coefficients form bayered image for each of valid gradients
      void ComputeSums( const double* v, const int* channels,
                        const double* gradients, const int* valid_gradients, int count,
                        double* sums )
      {
         // list of indices to 5x5 matrix to compute summed colors for respective gradients
         // for green center pixel
         static int green_center_indices[ 8 ][ 8 ] =
         {
            {  1,  2,  3,  7, 11, 12, 13, -1 },
            {  7,  9, 12, 13, 14, 17, 19, -1 },
            { 11, 12, 13, 17, 21, 22, 23, -1 },
            {  5,  7, 10, 11, 12, 15, 17, -1 },
            {  3,  7,  8,  9, 13, -1, -1, -1 },
            { 13, 17, 18, 19, 23, -1, -1, -1 },
            {  1,  5,  6,  7, 11, -1, -1, -1 },
            { 11, 15, 16, 17, 21, -1, -1, -1 }
         };

         // list of indices to 5x5 matrix to compute summed colors for respective gradients
         // for red or blue center pixel
         static int other_center_indices[ 8 ][ 8 ] =
         {
            {  2,  6,  7,  8, 12, -1, -1, -1 },
            {  8, 12, 13, 14, 18, -1, -1, -1 },
            { 12, 16, 17, 18, 22, -1, -1, -1 },
            {  6, 10, 11, 12, 16, -1, -1, -1 },
            {  3,  4,  7,  8,  9, 12, 13, -1 },
            { 12, 13, 17, 18, 19, 23, 24, -1 },
            {  0,  1,  5,  6,  7, 11, 12, -1 },
            { 11, 12, 15, 16, 17, 20, 21, -1 }
         };

         // init sums to zero
         for ( int channel = 0; channel < 3; channel++ )
            sums[channel] = 0;

         // for all valid gradients
         for ( int i = 0; i < count; i++ )
         {
            // get index of the valid gradient
            int grad = valid_gradients[i];
            int j = 0;
            // init partial sums over channels
            double partial_sums[ 3 ] = { 0.0, 0.0, 0.0 };
            int partial_counts[ 3 ] = { 0, 0, 0 };
            int* indices = (channels[ 12 ] == 1) ? green_center_indices[grad] : other_center_indices[grad];

            // compute partial sums for current gradient
            while ( indices[j] >= 0 )
            {
               // get index to 5x5 matrix
               int index = indices[j];
               // get bayer channel for this index
               int channel = channels[index];
               // add to sum for this channel
               partial_sums[channel] += v[index];
               partial_counts[channel]++;
               j++;
            }

            // add averaged partial sums to total sums per each channel
            for ( int channel = 0; channel < 3; channel++ )
               if ( partial_counts[channel] > 0 )
                  sums[channel] += partial_sums[channel]/double( partial_counts[channel] );
         }
      }
   }; // VNGThread

#undef m_target
#undef m_source
#undef m_instance
#undef m_start
#undef m_end

   class NoiseEvaluationThread : public Thread
   {
   public:

      float       noiseEstimate;
      float       noiseFraction;
      const char* noiseAlgorithm;

      NoiseEvaluationThread( const Image& image, int channel, int algorithm, int numberOfSubthreads ) :
      Thread(),
      noiseEstimate( 0 ), noiseFraction( 0 ), noiseAlgorithm( "" ),
      m_image(), m_algorithm( algorithm ), m_numberOfSubthreads( numberOfSubthreads )
      {
         image.SelectChannel( channel );
         m_image = image;
      }

      virtual void Run()
      {
         MuteStatus status;
         m_image.SetStatusCallback( &status );
         m_image.Status().DisableInitialization();

         SeparableFilter H( __5x5B3Spline_hv, __5x5B3Spline_hv, 5 );

         switch ( m_algorithm )
         {
         case DebayerNoiseEvaluationAlgorithm::KSigma:
            {
               ATrousWaveletTransform W( H, 1 );
               W.EnableParallelProcessing( m_numberOfSubthreads > 1, m_numberOfSubthreads );
               W << m_image;
               size_type N;
               noiseEstimate = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/__5x5B3Spline_kj[0];
               noiseFraction = float( double( N )/m_image.NumberOfPixels() );
               noiseAlgorithm = "K-Sigma";
            }
            break;
         default:
         case DebayerNoiseEvaluationAlgorithm::MRS:
            {
               double s0 = 0;
               float f0 = 0;
               for ( int n = 4; ; )
               {
                  ATrousWaveletTransform W( H, n );
                  W.EnableParallelProcessing( m_numberOfSubthreads > 1, m_numberOfSubthreads );
                  W << m_image;

                  size_type N;
                  if ( n == 4 )
                  {
                     s0 = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/__5x5B3Spline_kj[0];
                     f0 = float( double( N )/m_image.NumberOfPixels() );
                  }
                  noiseEstimate = W.NoiseMRS( ImageVariant( &m_image ), __5x5B3Spline_kj, s0, 3, &N );
                  noiseFraction = float( double( N )/m_image.NumberOfPixels() );

                  if ( noiseEstimate > 0 && noiseFraction >= 0.01F )
                  {
                     noiseAlgorithm = "MRS";
                     break;
                  }

                  if ( --n == 1 )
                  {
                     noiseEstimate = s0;
                     noiseFraction = f0;
                     noiseAlgorithm = "K-Sigma";
                     break;
                  }
               }
            }
            break;
         }
      }

   private:

      Image m_image;
      int   m_algorithm;
      int   m_numberOfSubthreads;

   }; // NoiseEvaluationThread

}; // DebayerEngine

// ----------------------------------------------------------------------------

bool DebayerInstance::ExecuteOn( View& view )
{
   o_imageId.Clear();
   o_noiseEstimates = FVector( 0.0F, 3 );
   o_noiseFractions = FVector( 0.0F, 3 );
   o_noiseAlgorithms = StringList( 3 );

   AutoViewLock lock( view, false/*lock*/ );
   lock.LockForWrite();

   ImageVariant source = view.Image();
   if ( source.IsComplexSample() )
      return false;

   IsoString methodId;
   switch( p_debayerMethod )
   {
   case DebayerMethodParameter::SuperPixel: methodId = "SuperPixel"; break;
   case DebayerMethodParameter::Bilinear:   methodId = "Bilinear"; break;
   case DebayerMethodParameter::VNG:        methodId = "VNG"; break;
   default:
      throw Error( "Internal error: Invalid de-Bayer method!" );
   }

   IsoString patternId;
   switch ( p_bayerPattern )
   {
   case DebayerBayerPatternParameter::RGGB: patternId = "RGGB"; break;
   case DebayerBayerPatternParameter::BGGR: patternId = "BGGR"; break;
   case DebayerBayerPatternParameter::GBRG: patternId = "GBRG"; break;
   case DebayerBayerPatternParameter::GRBG: patternId = "GRBG"; break;
   default:
      throw Error( "Internal error: Invalid Bayer pattern!" );
   }

   IsoString baseId = ValidFullId( view.FullId() ) + "_RGB";

   ImageWindow targetWindow(  1,    // width
                              1,    // height
                              3,    // numberOfChannels
                             32,    // bitsPerSample
                             true,  // floatSample
                             true,  // color
                             true,  // initialProcessing
                             baseId + '_' + methodId );  // imageId

   ImageVariant t = targetWindow.MainView().Image();
   Image& target = static_cast<Image&>( *t );

   StandardStatus status;
   target.SetStatusCallback( &status );

   Console().EnableAbort();

   switch( p_debayerMethod )
   {
   case DebayerMethodParameter::SuperPixel:
      DoSuperPixel( target, source );
      break;
   case DebayerMethodParameter::Bilinear:
      DoBilinear( target, source );
      break;
   case DebayerMethodParameter::VNG:
      DoVNG( target, source );
      break;
   default:
      throw Error( "Internal error: Invalid de-Bayer method!" );
   }

   String filePath = view.Window().FilePath();
   if ( !filePath.IsEmpty() )
      targetWindow.MainView().SetPropertyValue( "CFASourceFilePath", filePath, true/*notify*/, ViewPropertyAttribute::Storable );

   FITSKeywordArray keywords;
   view.Window().GetKeywords( keywords );

   keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "Debayering with "  + PixInsightVersion::AsString() ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Debayering with "  + Module->ReadableVersion() ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Debayer.pattern: " + patternId ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Debayer.method: "  + methodId ) );

   if ( p_evaluateNoise )
   {
      DebayerEngine::EvaluateNoise( o_noiseEstimates, o_noiseFractions, o_noiseAlgorithms, target, p_noiseEvaluationAlgorithm );

      /*
       * ### NB: Remove other existing NOISExxx keywords.
       *         *Only* our NOISExxx keywords must be present in the header.
       */
      for ( size_type i = 0; i < keywords.Length(); )
         if ( keywords[i].name.StartsWithIC( "NOISE" ) )
            keywords.Remove( keywords.At( i ) );
         else
            ++i;

      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Noise evaluation with " + Module->ReadableVersion() ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY",
                                       IsoString(),
                                       IsoString().Format( "Debayer.noiseEstimates: %.3e %.3e %.3e",
                                             o_noiseEstimates[0], o_noiseEstimates[1], o_noiseEstimates[2] ) ) );
      for ( int i = 0; i < 3; ++i )
      {
         keywords.Add( FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                          IsoString().Format( "%.3e", o_noiseEstimates[i] ),
                                          IsoString().Format( "Gaussian noise estimate, channel #%d", i ) ) );
         keywords.Add( FITSHeaderKeyword( IsoString().Format( "NOISEF%02d", i ),
                                          IsoString().Format( "%.3f", o_noiseFractions[i] ),
                                          IsoString().Format( "Fraction of noise pixels, channel #%d", i ) ) );
         keywords.Add( FITSHeaderKeyword( IsoString().Format( "NOISEA%02d", i ),
                                          IsoString( o_noiseAlgorithms[i] ),
                                          IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) ) );
      }
   }

   targetWindow.SetKeywords( keywords );

   if ( p_showImages )
   {
      targetWindow.Show();
      targetWindow.ZoomToFit( false/*allowZoomIn*/ );
   }

   o_imageId = targetWindow.MainView().Id();

   return true;
}

void DebayerInstance::DoSuperPixel( Image& target, const ImageVariant& source )
{
   if ( source.IsFloatSample() )
      switch ( source.BitsPerSample() )
      {
      case 32: DebayerEngine::SuperPixelThreaded( target, static_cast<const Image&>( *source ), *this ); break;
      case 64: DebayerEngine::SuperPixelThreaded( target, static_cast<const DImage&>( *source ), *this ); break;
      }
   else
      switch ( source.BitsPerSample() )
      {
      case  8: DebayerEngine::SuperPixelThreaded( target, static_cast<const UInt8Image&>( *source ), *this ); break;
      case 16: DebayerEngine::SuperPixelThreaded( target, static_cast<const UInt16Image&>( *source ), *this ); break;
      case 32: DebayerEngine::SuperPixelThreaded( target, static_cast<const UInt32Image&>( *source ), *this ); break;
      }
}

void DebayerInstance::DoBilinear( Image& target, const ImageVariant& source )
{
   if ( source.IsFloatSample() )
      switch ( source.BitsPerSample() )
      {
      case 32: DebayerEngine::BilinearThreaded( target, static_cast<const Image&>( *source ), *this ); break;
      case 64: DebayerEngine::BilinearThreaded( target, static_cast<const DImage&>( *source ), *this ); break;
      }
   else
      switch ( source.BitsPerSample() )
      {
      case  8: DebayerEngine::BilinearThreaded( target, static_cast<const UInt8Image&>( *source ), *this ); break;
      case 16: DebayerEngine::BilinearThreaded( target, static_cast<const UInt16Image&>( *source ), *this ); break;
      case 32: DebayerEngine::BilinearThreaded( target, static_cast<const UInt32Image&>( *source ), *this ); break;
      }
}

void DebayerInstance::DoVNG( Image& target, const ImageVariant& source )
{
   if ( source.IsFloatSample() )
      switch ( source.BitsPerSample() )
      {
      case 32: DebayerEngine::VNGThreaded( target, static_cast<const Image&>( *source ), *this ); break;
      case 64: DebayerEngine::VNGThreaded( target, static_cast<const DImage&>( *source ), *this ); break;
      }
   else
      switch ( source.BitsPerSample() )
      {
      case  8: DebayerEngine::VNGThreaded( target, static_cast<const UInt8Image&>( *source ), *this ); break;
      case 16: DebayerEngine::VNGThreaded( target, static_cast<const UInt16Image&>( *source ), *this ); break;
      case 32: DebayerEngine::VNGThreaded( target, static_cast<const UInt32Image&>( *source ), *this ); break;
      }
}

// ----------------------------------------------------------------------------

void* DebayerInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheDebayerBayerPatternParameter )
      return &p_bayerPattern;
   if ( p == TheDebayerMethodParameter )
      return &p_debayerMethod;
   if ( p == TheDebayerEvaluateNoiseParameter )
      return &p_evaluateNoise;
   if ( p == TheDebayerNoiseEvaluationAlgorithmParameter )
      return &p_noiseEvaluationAlgorithm;
   if ( p == TheDebayerShowImagesParameter )
      return &p_showImages;
   if ( p == TheDebayerOutputImageParameter )
      return o_imageId.c_str();
   if ( p == TheDebayerNoiseEstimateRParameter )
      return o_noiseEstimates.At( 0 );
   if ( p == TheDebayerNoiseEstimateGParameter )
      return o_noiseEstimates.At( 1 );
   if ( p == TheDebayerNoiseEstimateBParameter )
      return o_noiseEstimates.At( 2 );
   if ( p == TheDebayerNoiseFractionRParameter )
      return o_noiseFractions.At( 0 );
   if ( p == TheDebayerNoiseFractionGParameter )
      return o_noiseFractions.At( 1 );
   if ( p == TheDebayerNoiseFractionBParameter )
      return o_noiseFractions.At( 2 );
   if ( p == TheDebayerNoiseAlgorithmRParameter )
      return o_noiseAlgorithms[0].c_str();
   if ( p == TheDebayerNoiseAlgorithmGParameter )
      return o_noiseAlgorithms[1].c_str();
   if ( p == TheDebayerNoiseAlgorithmBParameter )
      return o_noiseAlgorithms[2].c_str();

   return 0;
}

bool DebayerInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDebayerOutputImageParameter )
   {
      o_imageId.Clear();
      if ( sizeOrLength > 0 )
         o_imageId.SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerNoiseAlgorithmRParameter )
   {
      o_noiseAlgorithms[0].Clear();
      if ( sizeOrLength > 0 )
         o_noiseAlgorithms[0].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerNoiseAlgorithmGParameter )
   {
      o_noiseAlgorithms[1].Clear();
      if ( sizeOrLength > 0 )
         o_noiseAlgorithms[1].SetLength( sizeOrLength );
   }
   else if ( p == TheDebayerNoiseAlgorithmBParameter )
   {
      o_noiseAlgorithms[2].Clear();
      if ( sizeOrLength > 0 )
         o_noiseAlgorithms[2].SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type DebayerInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheDebayerOutputImageParameter )
      return o_imageId.Length();
   if ( p == TheDebayerNoiseAlgorithmRParameter )
      return o_noiseAlgorithms[0].Length();
   if ( p == TheDebayerNoiseAlgorithmGParameter )
      return o_noiseAlgorithms[1].Length();
   if ( p == TheDebayerNoiseAlgorithmBParameter )
      return o_noiseAlgorithms[2].Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DebayerInstance.cpp - Released 2015/12/18 08:55:08 UTC
