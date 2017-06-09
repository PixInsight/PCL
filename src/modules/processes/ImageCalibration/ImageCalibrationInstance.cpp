//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0300
// ----------------------------------------------------------------------------
// ImageCalibrationInstance.cpp - Released 2017-05-17T17:41:56Z
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

#include "ImageCalibrationInstance.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoPointer.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ICCProfile.h>
#include <pcl/IntegerResample.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/MuteStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * 5x5 B3-spline wavelet scaling function. Used by the noise estimation routine
 * for dark frame optimization.
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

/*
 * Maximum dark scaling factor to issue a 'no correlation' warning.
 */
#define NO_DARK_CORRELATION   0.005F

/*
 * Maximum fraction of surviving pixels after optimization thresholding to
 * issue an 'optimization threshold too high' warning.
 */
#define DARK_COUNT_LOW        0.001

/*
 * Minimum cardinality of a usable dark frame optimization set. Smaller sets
 * disable dark frame optimization.
 */
#define DARK_COUNT_SMALL      16

/*
 * Spinning sticks.
 */
static size_type nspin = 0;
static const char* cspin[ 4 ] = { "&mdash;", "\\", "|", "/" };
#define SPIN ("<end>\b" + String( cspin[nspin++%4] ) + "<flush>")

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageCalibrationInstance::ImageCalibrationInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   targetFrames(),
   inputHints(),
   outputHints(),
   pedestal( TheICPedestalParameter->DefaultValue() ),
   pedestalMode( ICPedestalMode::Default ),
   pedestalKeyword(),
   overscan(),
   masterBias(),
   masterDark(),
   masterFlat(),
   calibrateBias( TheICCalibrateBiasParameter->DefaultValue() ),
   calibrateDark( TheICCalibrateDarkParameter->DefaultValue() ),
   calibrateFlat( TheICCalibrateFlatParameter->DefaultValue() ),
   optimizeDarks( TheICOptimizeDarksParameter->DefaultValue() ),
   darkOptimizationThreshold( TheICDarkOptimizationThresholdParameter->DefaultValue() ),
   darkOptimizationLow( TheICDarkOptimizationLowParameter->DefaultValue() ),
   darkOptimizationWindow( TheICDarkOptimizationWindowParameter->DefaultValue() ),
   darkCFADetectionMode( ICDarkCFADetectionMode::Default ),
   evaluateNoise( TheICEvaluateNoiseParameter->DefaultValue() ),
   noiseEvaluationAlgorithm( ICNoiseEvaluationAlgorithm::Default ),
   outputDirectory( TheICOutputDirectoryParameter->DefaultValue() ),
   outputExtension( TheICOutputExtensionParameter->DefaultValue() ),
   outputPrefix( TheICOutputPrefixParameter->DefaultValue() ),
   outputPostfix( TheICOutputPostfixParameter->DefaultValue() ),
   outputSampleFormat( ICOutputSampleFormat::Default ),
   outputPedestal( TheICOutputPedestalParameter->DefaultValue() ),
   overwriteExistingFiles( TheICOverwriteExistingFilesParameter->DefaultValue() ),
   onError( ICOnError::Default ),
   noGUIMessages( TheICNoGUIMessagesParameter->DefaultValue() ),
   output()
{
}

ImageCalibrationInstance::ImageCalibrationInstance( const ImageCalibrationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void ImageCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const ImageCalibrationInstance* x = dynamic_cast<const ImageCalibrationInstance*>( &p );
   if ( x != 0 )
   {
      targetFrames              = x->targetFrames;
      inputHints                = x->inputHints;
      outputHints               = x->outputHints;
      pedestal                  = x->pedestal;
      pedestalMode              = x->pedestalMode;
      pedestalKeyword           = x->pedestalKeyword;
      overscan                  = x->overscan;
      masterBias                = x->masterBias;
      masterDark                = x->masterDark;
      masterFlat                = x->masterFlat;
      calibrateBias             = x->calibrateBias;
      calibrateDark             = x->calibrateDark;
      calibrateFlat             = x->calibrateFlat;
      optimizeDarks             = x->optimizeDarks;
      darkOptimizationThreshold = x->darkOptimizationThreshold;
      darkOptimizationLow       = x->darkOptimizationLow;
      darkOptimizationWindow    = x->darkOptimizationWindow;
      darkCFADetectionMode      = x->darkCFADetectionMode;
      evaluateNoise             = x->evaluateNoise;
      noiseEvaluationAlgorithm  = x->noiseEvaluationAlgorithm;
      outputDirectory           = x->outputDirectory;
      outputExtension           = x->outputExtension;
      outputPrefix              = x->outputPrefix;
      outputPostfix             = x->outputPostfix;
      outputSampleFormat        = x->outputSampleFormat;
      outputPedestal            = x->outputPedestal;
      overwriteExistingFiles    = x->overwriteExistingFiles;
      onError                   = x->onError;
      noGUIMessages             = x->noGUIMessages;
      output                    = x->output;
   }
}

bool ImageCalibrationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "ImageCalibration can only be executed in the global context.";
   return false;
}

bool ImageCalibrationInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

bool ImageCalibrationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( targetFrames.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }

   if ( !overscan.IsValid() )
   {
      whyNot = "Invalid overscan region(s) defined.";
      return false;
   }

   if ( !masterBias.IsValid() )
   {
      whyNot = "Missing or invalid master bias frame.";
      return false;
   }

   if ( !masterDark.IsValid() )
   {
      whyNot = "Missing or invalid master dark frame.";
      return false;
   }

   if ( !masterFlat.IsValid() )
   {
      whyNot = "Missing or invalid master flat frame.";
      return false;
   }

   if ( !overscan.enabled && !masterBias.enabled && !masterDark.enabled && !masterFlat.enabled )
   {
      whyNot = "No master calibration frames or overscan regions have been specified.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Calibration Primitives
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Overscan subtraction
 */
static void SubtractOverscan( Image& target, const ImageCalibrationInstance::overscan_table& O, const Rect& r )
{
   target.SetRangeClipping( 0.00002, 0.99998 ); // ignore black and saturated pixels

   for ( ImageCalibrationInstance::overscan_table::const_iterator o = O.Begin(); o != O.End(); ++o )
   {
      FVector v( 0.0F, target.NumberOfChannels() );
      for ( ImageCalibrationInstance::overscan_list::const_iterator r = o->Begin(); r != o->End(); ++r )
         for ( int c = 0; c < target.NumberOfChannels(); ++c )
            v[c] += target.Median( r->sourceRect, c, c, false/*parallel*/ );
      v /= o->Length();

      target.SelectRectangle( o->Begin()->targetRect );
      for ( int c = 0; c < target.NumberOfChannels(); ++c )
         if ( 1 + v[c] != 1 )
         {
            target.SelectChannel( c );
            target -= v[c];
         }
   }

   target.ResetSelections();

   if ( r != target.Bounds() )
   {
      target.SelectRectangle( r );
      target.Crop();
   }
}

#define LOOP         while ( t < t1 )
#define BIAS         *b++
#define DARK         *d++
#define SCALED_DARK  k * *d++
#define FLAT         s/Max( *f++, TINY )

/*
 * Bias subtraction
 */
static void SubtractBias( Image& target, const Image& bias )
{
   for ( int c = 0; c < target.NumberOfChannels(); ++c )
   {
            float* t  = target.PixelData( c );
            float* t1 = t + target.NumberOfPixels();
      const float* b  = bias.PixelData( Min( c, bias.NumberOfChannels()-1 ) );
      LOOP *t++ -= BIAS;
   }
}

/*
 * Dark subtraction
 */
/* -- Currently this routine is not used
static void SubtractDark( Image& target, const Image& dark, const FVector& dScale )
{
   for ( int c = 0; c < target.NumberOfChannels(); ++c )
   {
            float* t  = target.PixelData( c );
            float* t1 = t + target.NumberOfPixels();
      const float* d  = dark.PixelData( Min( c, dark.NumberOfChannels()-1 ) );
            float  k  = dScale[c];
      if ( k != 1 )
         LOOP *t++ -= SCALED_DARK;
      else
         LOOP *t++ -= DARK;
   }
}
*/

/*
 * One-channel dark subtraction with scaling
 */
static void SubtractOneChannelDark( Image& target, int tCh, const Image& dark, int dCh, float k )
{
         float* t  = target.PixelData( tCh );
         float* t1 = t + target.NumberOfPixels();
   const float* d  = dark.PixelData( dCh );
   LOOP *t++ -= SCALED_DARK;
}

/*
 * Full calibration routine
 */
static void Calibrate( Image& target,
                       const Image* bias,
                       const Image* dark, const FVector& dScale,
                       const Image* flat, const FVector& fScale,
                       float        pedestal = 0 )
{
   static const float TINY = 1.0e-15F; // to prevent divisions by zero flat pixels

   int nt = target.NumberOfChannels();
   int nb = (bias != 0) ? bias->NumberOfChannels() : 0;
   int nd = (dark != 0) ? dark->NumberOfChannels() : 0;
   int nf = (flat != 0) ? flat->NumberOfChannels() : 0;

   for ( int c = 0; c < nt; ++c )
   {
      Module->ProcessEvents();

            float* t  = target.PixelData( c );
            float* t1 = t + target.NumberOfPixels();
      const float* b  = (bias != 0) ? bias->PixelData( Min( c, nb-1 ) ) : 0;
      const float* d  = (dark != 0) ? dark->PixelData( Min( c, nd-1 ) ) : 0;
      const float* f  = (flat != 0) ? flat->PixelData( Min( c, nf-1 ) ) : 0;
            float  k  = (dark != 0) ? dScale[c] : 0;
            float  s  = (flat != 0) ? fScale[c] : 0;

      if ( b != 0 )
      {
         if ( d != 0 )
         {
            if ( k != 1 )
            {
               if ( f != 0 )
                  LOOP *t = (*t - BIAS - SCALED_DARK) * FLAT, ++t;
               else
                  LOOP *t++ -= BIAS + SCALED_DARK;
            }
            else
            {
               if ( f != 0 )
                  LOOP *t = (*t - BIAS - DARK) * FLAT, ++t;
               else
                  LOOP *t++ -= BIAS + DARK;
            }
         }
         else
         {
            if ( f != 0 )
               LOOP *t = (*t - BIAS) * FLAT, ++t;
            else
               LOOP *t++ -= BIAS;
         }
      }
      else
      {
         if ( d != 0 )
         {
            if ( k != 1 )
            {
               if ( f != 0 )
                  LOOP *t = (*t - SCALED_DARK) * FLAT, ++t;
               else
                  LOOP *t++ -= SCALED_DARK;
            }
            else
            {
               if ( f != 0 )
                  LOOP *t = (*t - DARK) * FLAT, ++t;
               else
                  LOOP *t++ -= DARK;
            }
         }
         else
         {
            if ( f != 0 )
               LOOP *t++ *= FLAT;
         }
      }
   } // for each channel

   /*
    * Optional pedestal to enforce positivity.
    */
   if ( pedestal != 0 )
      target += pedestal;

   /*
    * Constrain the calibrated target image to the [0,1] range.
    */
   target.Truncate();
}

#undef LOOP
#undef BIAS
#undef DARK
#undef SCALED_DARK
#undef FLAT

// ----------------------------------------------------------------------------

/*
 * Quick estimation of noise sigma after dark subtraction by the iterative
 * k-sigma method.
 *
 * J.L. Stark F. Murtagh, Astronomical Image and Data Analysis, pp. 37-38.
 *
 * J.L. Stark et al., Automatic Noise Estimation from the Multiresolution
 *    Support Publications of the Royal Astronomical Society of the Pacific,
 *    vol. 110, February 1998, pp. 193-199
 *
 * Returns a noise estimate for (target - k*dark).
 */
static double TestDarkOptimization( float k, const Image& target, const Image& dark )
{
   Image t( target );
   t.Status().DisableInitialization();

   SubtractOneChannelDark( t, 0, dark, Min( target.SelectedChannel(), dark.NumberOfChannels()-1 ), k );

   SeparableFilter H( __5x5B3Spline_hv, __5x5B3Spline_hv, 5 );
   ATrousWaveletTransform W( H, 1 );
   W.DisableParallelProcessing();
   W.DisableLayer( 1 );
   W << t;
   return W.NoiseKSigma( 0 ) /*/__5x5B3Spline_kj[0]*/; // we can work with unscaled noise estimates here
}

/*
 * Initial bracketing of the dark optimization factor.
 *
 * Minimization routines based on Numerical Recipes in C, 2nd Edition
 * (section 10.1, pp. 397-402).
 */

template <typename T>
inline T SameSign( const T& x, const T& sameAs )
{
   return (sameAs < 0) ? ((x < 0) ? x : -x) : ((x < 0) ? -x : x);
}

#define TEST_DARK( x )  TestDarkOptimization( x, target, dark )

static void BracketDarkOptimization( float& ax, float& bx, float& cx,
                                     const Image& target, const Image& dark, bool useConsole )
{
   if ( useConsole )
   {
      Console().Write( "<end><cbr>Bracketing...  " );
      Console().Flush();
   }

   static const float GOLD = 1.618034;
   static const int GLIMIT = 10;
   static const float TINY = 1.0e-20F;

   // Start with the [1/2,2] interval
   ax = 0.5F;
   bx = 2.0F;
   float fa = TEST_DARK( ax );
   float fb = TEST_DARK( bx );
   if ( fb > fa )
   {
      Swap( ax, bx );
      Swap( fb, fa );
   }

   cx = bx + GOLD*(bx - ax);
   float fc = TEST_DARK( cx );
   while ( fb > fc )
   {
      Module->ProcessEvents();

      if ( useConsole )
      {
         if ( Console().AbortRequested() )
            throw ProcessAborted();
         Console().Write( SPIN );
      }

      float r = (bx - ax)*(fb - fc);
      float q = (bx - cx)*(fb - fa);
      float u = bx - ((bx - cx)*q - (bx - ax)*r)/2/SameSign( Max( Abs( q-r ), TINY ), q-r );
      float ulim = bx + GLIMIT*(cx - bx);
      float fu;
      if ( (bx - u)*(u - cx) > 0 )
      {
         fu = TEST_DARK( u );
         if ( fu < fc )
         {
            ax = bx;
            bx = u;
            return;
         }
         else if ( fu > fb )
         {
            cx = u;
            return;
         }
         u = cx + GOLD*(cx - bx);
         fu = TEST_DARK( u );
      }
      else if ( (cx - u)*(u - ulim) > 0 )
      {
         fu = TEST_DARK( u );
         if ( fu < fc )
         {
            float du = GOLD*(u - cx);
            bx = cx;
            cx = u;
            u += du;
            fb = fc;
            fc = fu;
            fu = TEST_DARK( u );
         }
      }
      else if ( (u - ulim)*(ulim - cx) >= 0 )
      {
         u = ulim;
         fu = TEST_DARK( u );
      }
      else
      {
         u = cx + GOLD*(cx - bx);
         fu = TEST_DARK( u );
      }

      ax = bx;
      bx = cx;
      cx = u;
      fa = fb;
      fb = fc;
      fc = fu;
   }
}

/*
 * Optimal dark scaling factor by multiscale noise evaluation and golden
 * section search minimization.
 *
 * Noise evaluation algorithms described in:
 *
 * J.L. Stark F. Murtagh, Astronomical Image and Data Analysis, pp. 37-38.
 *
 * J.L. Stark et al., Automatic Noise Estimation from the Multiresolution
 *    Support Publications of the Royal Astronomical Society of the Pacific,
 *    vol. 110, February 1998, pp. 193-199
 *
 * Function minimization routines based on:
 *
 * W.H. Press et al, Numerical Recipes in C, 2nd Edition
 * (section 10.1, pp. 397-402).
 */
static float DarkOptimization( const Image& target, const Image& dark, int c, bool useConsole = false )
{
   /*
    * The golden ratios
    */
   const float R = 0.61803399;
   const float C = 1 - R;

   target.ResetSelections();
   target.SelectChannel( c );

   /*
    * Find an initial triplet ax,bx,cx that brackets the minimum.
    */
   float ax, bx, cx;
   BracketDarkOptimization( ax, bx, cx, target, dark, useConsole );

   if ( useConsole ) // if we are not running into a thread
   {
      Console().Write( "<end>\b\nOptimizing...  " );
      Console().Flush();
   }

   /*
    * [x0,x3] is the total search interval.
    */
   float x0 = ax;
   float x3 = cx;

   /*
    * [x1,x2] is the inner search interval.
    * Start by sectioning the largest segment: either ax-bx or bx-cx.
    */
   float x1, x2;
   if ( Abs( bx - ax ) < Abs( cx - bx ) )
   {
      x1 = bx;
      x2 = bx + C*(cx - bx);
   }
   else
   {
      x1 = bx - C*(bx - ax);
      x2 = bx;
   }

   /*
    * Start with noise estimates at the inner interval boundaries.
    */
   float f1 = TEST_DARK( x1 );
   float f2 = TEST_DARK( x2 );

   /*
    * Perform a golden section search for the minimum dark-induced noise.
    * We stop when we find a minimum to 1/1000 fractional accuracy.
    * Actually, trying to achieve very high accuracy is useless because when we
    * get close to the minimum, the function approximates a parabola so we are
    * trying to minimize something similar to a horizontal straight line.
    * Note that this is common to any minimization (or maximization) routine.
    * We perhaps could achieve 1/10000 at the cost of much more iterations, but
    * that would be a marginal improvement to dark optimization that doesn't
    * justify the increase in computation time.
    */
   while ( Abs( x3 - x0 ) > 0.0005F )
   {
      Module->ProcessEvents();

      if ( useConsole )
      {
         if ( Console().AbortRequested() )
            throw ProcessAborted();
         Console().Write( SPIN );
      }

      if ( f2 < f1 )
      {
         /*
          * Go downhill by sectioning at the upper segment of the search
          * interval.
          */
         x0 = x1;
         x1 = x2;
         x2 = R*x2 + C*x3;
         f1 = f2;
         f2 = TEST_DARK( x2 );
      }
      else
      {
         /*
          * Go downhill by sectioning at the lower segment of the search
          * interval.
          */
         x3 = x2;
         x2 = x1;
         x1 = R*x1 + C*x0;
         f2 = f1;
         f1 = TEST_DARK( x1 );
      }
   }

   if ( useConsole )
      Console().WriteLn( "<end>\b" );

   /*
    * Return the dark scaling factor that minimizes noise in the target image.
    * ### NB: The dark frame optimization factor cannot be negative.
    */
   return Max( .0F, (f1 < f2) ? x1 : x2 );
}

#undef TEST_DARK

static Rect OptimizingRect( const Image& target, int windowSize )
{
   return
   Rect( Min( windowSize, target.Width() ),
         Min( windowSize, target.Height() ) ).MovedTo( Max( 0, (target.Width()-windowSize)>>1 ),
                                                       Max( 0, (target.Height()-windowSize)>>1 ) );
}

static FVector OptimizeDark( const Image& target, const Image& optimizingDark, bool isDarkCFA )
{
   // The console cannot be used from a thread.
   bool useConsole = Thread::IsRootThread();

   FVector K( target.NumberOfChannels() );

   Image optimizingTarget( target );
   optimizingTarget.Status().DisableInitialization();

   if ( isDarkCFA )
      IntegerResample( -2 ) >> optimizingTarget;

   if ( optimizingTarget.Bounds() != optimizingDark.Bounds() )
   {
      int windowSize = Max( optimizingDark.Width(), optimizingDark.Height() );
      optimizingTarget.SelectRectangle( OptimizingRect( optimizingTarget, windowSize ) );
      optimizingTarget.Crop();
      optimizingTarget.ResetSelections();
   }

   for ( int c = 0; c < optimizingTarget.NumberOfChannels(); ++c )
      K[c] = DarkOptimization( optimizingTarget, optimizingDark, c, useConsole );

   return K;
}

/*
 * Estimation of the standard deviation of the noise, assuming a Gaussian
 * noise distribution.
 *
 * - Use MRS noise evaluation when the algorithm converges for 4 >= J >= 2
 *
 * - Use k-sigma noise evaluation when either MRS doesn't converge, or when the
 *   length of the set of noise pixels is less than a 1% of the image area.
 *
 * - Automatically iterate to find the highest wavelet layer where noise can be
 *   successfully evaluated, in the [1,3] range.
 */
static void EvaluateNoise( FVector& noiseEstimates, FVector& noiseFractions, StringList& noiseAlgorithms,
                           const Image& target,
                           int algorithm,
                           int maxProcessors, bool cfaImage )
{
   MuteStatus status;
   target.SetStatusCallback( &status );
   target.Status().DisableInitialization();

   if ( cfaImage )
   {
      Image evaluationTarget( target );
      IntegerResample( -2 ) >> evaluationTarget;
      EvaluateNoise( noiseEstimates, noiseFractions, noiseAlgorithms, evaluationTarget, algorithm, maxProcessors, false );
      return;
   }

   noiseEstimates = FVector( 0.0F, target.NumberOfChannels() );
   noiseFractions = FVector( 0.0F, target.NumberOfChannels() );
   noiseAlgorithms = StringList( target.NumberOfChannels(), String() );

   SeparableFilter H( __5x5B3Spline_hv, __5x5B3Spline_hv, 5 );

   for ( int c = 0; c < target.NumberOfChannels(); ++c )
   {
      target.SelectChannel( c );

      switch ( algorithm )
      {
      case ICNoiseEvaluationAlgorithm::KSigma:
         {
            ATrousWaveletTransform W( H, 1 );
            W.EnableParallelProcessing( maxProcessors > 1, maxProcessors );
            W << target;
            size_type N;
            noiseEstimates[c] = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/__5x5B3Spline_kj[0];
            noiseFractions[c] = float( double( N )/target.NumberOfPixels() );
            noiseAlgorithms[c] = String( "K-Sigma" );
         }
         break;
      default:
      case ICNoiseEvaluationAlgorithm::MRS:
         {
            double s0 = 0;
            float f0 = 0;
            for ( int n = 4; ; )
            {
               Module->ProcessEvents();

               ATrousWaveletTransform W( H, n );
               W.EnableParallelProcessing( maxProcessors > 1, maxProcessors );
               W << target;

               size_type N;
               if ( n == 4 )
               {
                  s0 = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/__5x5B3Spline_kj[0];
                  f0 = float( double( N )/target.NumberOfPixels() );
               }
               noiseEstimates[c] = W.NoiseMRS( ImageVariant( const_cast<Image*>( &target ) ), __5x5B3Spline_kj, s0, 3, &N );
               noiseFractions[c] = float( double( N )/target.NumberOfPixels() );

               if ( noiseEstimates[c] > 0 && noiseFractions[c] >= 0.01F )
               {
                  noiseAlgorithms[c] = String( "MRS" );
                  break;
               }

               if ( --n == 1 )
               {
                  noiseEstimates[c] = s0;
                  noiseFractions[c] = f0;
                  noiseAlgorithms[c] = String( "K-Sigma" );
                  break;
               }
            }
         }
         break;
      }
   }

   target.ResetSelections();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// File Management Routines
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct OutputFileData
{
   AutoPointer<FileFormat> format;           // the file format of retrieved data
   const void*             fsData = nullptr; // format-specific data
   ImageOptions            options;          // currently used for resolution only
   PropertyArray           properties;       // image properties
   FITSKeywordArray        keywords;         // FITS keywords
   ICCProfile              profile;          // ICC profile

   OutputFileData() = default;

   OutputFileData( FileFormatInstance& file, const ImageOptions& o ) : options( o )
   {
      format = new FileFormat( file.Format() );

      if ( format->UsesFormatSpecificData() )
         fsData = file.FormatSpecificData();

      if ( format->CanStoreImageProperties() )
         properties = file.ReadImageProperties();

      if ( format->CanStoreKeywords() )
         file.ReadFITSKeywords( keywords );

      if ( format->CanStoreICCProfiles() )
         file.ReadICCProfile( profile );
   }

   ~OutputFileData()
   {
      if ( format )
      {
         if ( fsData != nullptr )
         {
            format->DisposeFormatSpecificData( const_cast<void*>( fsData ) );
            fsData = nullptr;
         }
      }
   }
};

static Image* LoadImageFile( FileFormatInstance& file, int index = 0 )
{
   // Select the image at index
   if ( !file.SelectImage( index ) )
      throw CatchedException();

   // Create a shared image, 32-bit floating point
   Image* image = new Image( (void*)0, 0, 0 );

   // Read the image
   if ( !file.ReadImage( *image ) )
      throw CatchedException();

   return image;
}

static String UniqueFilePath( const String& filePath )
{
   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Calibration Thread
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct CalibrationThreadData
{
   ImageCalibrationInstance* instance; // the instance being executed
   ImageCalibrationInstance::overscan_table overscan; // overscan regions grouped by target regions
   Image*  bias;           // master bias frame, overscan corrected
   Image*  dark;           // master dark frame, overscan+bias corrected
   Image*  optimizingDark; // calibrated dark frame, central window for dark optimization, maybe thresholded, maybe binned 2x2
   bool    isDarkCFA;      // if true, the master dark frame is a CFA and we must bin 2x2 prior to optimization
   Image*  flat;           // master flat frame, overscan+bias+dark corrected
   FVector fScale;         // flat scaling factor
   int     maxProcessors;  // maximum number of threads allowed (for noise estimation)
};

class CalibrationThread : public Thread
{
public:

   /*
    * Optimized dark scaling factors
    */
   FVector K;

   /*
    * Gaussian noise evaluation
    */
   FVector    noiseEstimates;
   FVector    noiseFractions;
   StringList noiseAlgorithms;

   CalibrationThread( Image* target, OutputFileData* outputData, const String& targetPath, int subimageIndex,
                      const CalibrationThreadData& data ) :
      m_target( target ),
      m_outputData( outputData ),
      m_targetPath( targetPath ),
      m_subimageIndex( subimageIndex ),
      m_success( false ),
      m_data( data )
   {
   }

   virtual void Run()
   {
      try
      {
         m_success = false;

         m_target->Status().DisableInitialization();

         /*
          * Overscan correction.
          */
         if ( m_data.instance->overscan.enabled )
            SubtractOverscan( *m_target, m_data.overscan, m_data.instance->overscan.imageRect );

         /*
          * Dark frame optimization.
          */
         if ( m_data.dark != nullptr && m_data.instance->optimizeDarks && m_data.optimizingDark != nullptr )
            K = OptimizeDark( *m_target,
                              *m_data.optimizingDark,
                               m_data.isDarkCFA );
         else
            K = FVector( 1.0F, m_target->NumberOfChannels() );

         /*
          * Target frame calibration.
          */
         Calibrate( *m_target,
                     m_data.bias,
                     m_data.dark, K,
                     m_data.flat, m_data.fScale,
                     m_data.instance->outputPedestal/65535.0 );

         /*
          * Noise evaluation.
          *
          * ### TODO: Implement a different way to tell the noise evaluation
          *           routine that we have CFA images (not tied to dark frame
          *           optimization).
          */
         if ( m_data.instance->evaluateNoise )
            EvaluateNoise( noiseEstimates, noiseFractions, noiseAlgorithms,
                          *m_target,
                           m_data.instance->noiseEvaluationAlgorithm,
                           m_data.maxProcessors,
                           m_data.instance->darkCFADetectionMode == ICDarkCFADetectionMode::ForceCFA || m_data.isDarkCFA );

         m_success = true;
      }
      catch ( ... )
      {
         ClearConsoleOutputText();
         try
         {
            throw;
         }
         ERROR_HANDLER
      }
   }

   const CalibrationThreadData& CalibrationData() const
   {
      return m_data;
   }

   const Image* TargetImage() const
   {
      return m_target.Pointer();
   }

   String TargetPath() const
   {
      return m_targetPath;
   }

   const OutputFileData& OutputData() const
   {
      return *m_outputData;
   }

   int SubimageIndex() const
   {
      return m_subimageIndex;
   }

   bool Success() const
   {
      return m_success;
   }

private:

   AutoPointer<Image>          m_target;        // The image being calibrated. It belongs to this thread.
   AutoPointer<OutputFileData> m_outputData;    // Target image parameters and embedded m_data. It belongs to this thread.
   String                      m_targetPath;    // File path of this m_target image
   int                         m_subimageIndex; // >= 0 in case of a multiple image; = 0 otherwise
   bool                        m_success : 1;   // The thread completed execution successfully

   const CalibrationThreadData& m_data;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Routine to enforce validity of image geometries.
 */
void ImageCalibrationInstance::ValidateImageGeometry( const Image* image, bool uncalibrated )
{
   if ( uncalibrated )
   {
      if ( geometry.IsRect() )
      {
         if ( image->Bounds() != geometry )
            throw Error( "Incompatible image geometry" );
      }
      else
      {
         geometry = image->Bounds();

         if ( overscan.enabled )
         {
            if ( !geometry.Includes( overscan.imageRect ) )
               throw Error( "Inconsistent overscan geometry" );

            for ( int i = 0; i < 4; ++i )
               if ( overscan.overscan[i].enabled )
                  if ( !geometry.Includes( overscan.overscan[i].sourceRect ) ||
                       !geometry.Includes( overscan.overscan[i].targetRect ) )
                     throw Error( "Inconsistent overscan geometry" );
         }
         else
         {
            if ( calibratedGeometry.IsRect() )
            {
               if ( geometry != calibratedGeometry )
                  throw Error( "Incompatible image geometry" );
            }
            else
               calibratedGeometry = geometry;
         }
      }
   }
   else
   {
      if ( calibratedGeometry.IsRect() )
      {
         if ( image->Bounds() != calibratedGeometry )
            throw Error( "Incompatible image geometry" );
      }
      else
      {
         calibratedGeometry = image->Bounds();

         if ( overscan.enabled )
         {
            if ( calibratedGeometry.Width() != overscan.imageRect.Width() ||
                 calibratedGeometry.Height() != overscan.imageRect.Height() )
               throw Error( "Inconsistent overscan geometry" );
         }
         else
         {
            if ( geometry.IsRect() )
            {
               if ( calibratedGeometry != geometry )
                  throw Error( "Incompatible image geometry" );
            }
            else
               geometry = calibratedGeometry;
         }
      }
   }
}

/*
 * Routine to group overscan source regions with the same target regions.
 */
ImageCalibrationInstance::overscan_table
ImageCalibrationInstance::BuildOverscanTable() const
{
   overscan_table O;

   OverscanRegions r[ 4 ];
   for ( int i = 0; i < 4; ++i )
      r[i] = overscan.overscan[i];

   for ( int i = 0; i < 4; ++i )
      if ( r[i].enabled )
      {
         overscan_list o;
         o.Add( r[i] );
         for ( int j = i; ++j < 4; )
            if ( r[j].enabled && r[j].targetRect == r[i].targetRect )
            {
               o.Add( r[j] );
               r[j].enabled = false;
            }
         O.Add( o );
      }

   return O;
}

/*
 * Subtraction of an input pedestal to get zero-based pixel values.
 */
void ImageCalibrationInstance::SubtractPedestal( Image* image, FileFormatInstance& file )
{
   switch ( pedestalMode )
   {
   case ICPedestalMode::Literal:
      if ( pedestal != 0 )
      {
         Console().NoteLn( String().Format( "* Subtracting pedestal: %d DN", pedestal ) );
         image->Apply( pedestal/65535.0, ImageOp::Sub );
      }
      break;
   case ICPedestalMode::Keyword:
   case ICPedestalMode::CustomKeyword:
      if ( file.Format().CanStoreKeywords() )
      {
         IsoString keyName( (pedestalMode == ICPedestalMode::Keyword) ? "PEDESTAL" : pedestalKeyword );
         FITSKeywordArray keywords;
         if ( !file.ReadFITSKeywords( keywords ) )
            break;
         double d = 0;
         for ( const FITSHeaderKeyword& keyword : keywords )
            if ( !keyword.name.CompareIC( keyName ) )
               if ( keyword.IsNumeric() )
                  if ( keyword.GetNumericValue( d ) ) // GetNumericValue() sets d=0 if keyword can't be converted
                     break;
         if ( d != 0 )
         {
            Console().NoteLn( String().Format( "* Subtracting pedestal keyword '%s': %.4f DN", keyName.c_str(), d ) );
            image->Apply( d/65535.0, ImageOp::Sub );
         }
      }
      break;
   }
}

/*
 * Read a source calibration frame.
 */
Image* ImageCalibrationInstance::LoadCalibrationFrame( const String& filePath, bool willCalibrate, bool* hasCFA )
{
   Console console;

   console.WriteLn( "<end><cbr>Loading calibration frame image:" );
   console.WriteLn( filePath );

   /*
    * Find out an installed file format that can read image files with the
    * specified extension ...
    */
   FileFormat format( File::ExtractExtension( filePath ), true, false );

   /*
    * ... and create a format instance (usually a disk file) to access this
    * master calibration frame image.
    */
   FileFormatInstance file( format );

   /*
    * Open input stream.
    */
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, inputHints ) )
      throw CatchedException();

   /*
    * Check for an empty calibration frame.
    */
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty master calibration frame image." );

   /*
    * Master calibration files can store multiple images - only the first image
    * will be used in such case, and the rest will be ignored. For example, a
    * master file can store integration rejection maps along with a master
    * bias, dark or flat frame.
    */
   if ( images.Length() > 1 )
      console.NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in master calibration frame.", images.Length()-1 ) );

   /*
    * Optional CFA type retrieval.
    */
   if ( hasCFA != nullptr )
      *hasCFA = images[0].options.cfaType != CFAType::None;

   /*
    * Load the master calibration frame.
    */
   Image* image = LoadImageFile( file );

   /*
    * Enforce consistency of frame dimensions.
    */
   ValidateImageGeometry( image, willCalibrate );

   /*
    * Optional pedestal subtraction.
    */
   SubtractPedestal( image, file );

   /*
    * Close the input stream.
    */
   file.Close();

   return image;
}

/*
 * Read a target frame file. Returns a list of calibration threads ready to
 * calibrate all subimages loaded from the file.
 */
thread_list
ImageCalibrationInstance::LoadTargetFrame( const String& filePath, const CalibrationThreadData& threadData )
{
   Console console;

   console.WriteLn( "<end><cbr>Loading target frame:" );
   console.WriteLn( filePath );

   /*
    * Find out an installed file format that can read image files with the
    * specified extension ...
    */
   FileFormat format( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );

   /*
    * ... and create a format instance (usually a disk file) to access this
    * target image.
    */
   FileFormatInstance file( format );

   /*
    * Open the image file.
    */
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, inputHints ) )
      throw CatchedException();

   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty image file." );

   /*
    * Multiple-image file formats are supported and implemented in PixInsight
    * (e.g.: XISF, FITS), so when we open a file, what we get is an array of
    * images, usually consisting of a single image, but we must provide for a
    * set of subimages.
    */
   thread_list threads;
   try
   {
      for ( size_type j = 0; j < images.Length(); ++j )
      {
         if ( images.Length() > 1 )
            console.WriteLn( String().Format( "* Subimage %u of %u", j+1, images.Length() ) );

         AutoPointer<Image> target( LoadImageFile( file, j ) );

         Module->ProcessEvents();

         /*
          * NB: At this point, LoadImageFile() has already called
          * file.SelectImage().
          */

         /*
          * Enforce consistency of image dimensions.
          */
         ValidateImageGeometry( target );

         /*
          * Optional pedestal subtraction.
          */
         SubtractPedestal( target, file );

         /*
          * Retrieve metadata and auxiliary data structures that must be
          * preserved in the output image (properties, header keywords, etc).
          */
         OutputFileData* outputData = new OutputFileData( file, images[j].options );

         /*
          * Create a new calibration thread and add it to the thread list.
          */
         threads.Add( new CalibrationThread( target,
                                             outputData,
                                             filePath,
                                             (images.Length() > 1) ? j+1 : 0,
                                             threadData ) );
         // The thread owns the target image
         target.Release();
      }

      /*
       * Close the input stream.
       */
      file.Close();

      return threads;
   }
   catch ( ... )
   {
      threads.Destroy();
      throw;
   }
}

void ImageCalibrationInstance::WriteCalibratedImage( const CalibrationThread* t )
{
   Console console;

   /*
    * Output directory.
    */
   String fileDir = outputDirectory.Trimmed();
   if ( fileDir.IsEmpty() )
      fileDir = File::ExtractDrive( t->TargetPath() ) + File::ExtractDirectory( t->TargetPath() );
   if ( fileDir.IsEmpty() )
      throw Error( t->TargetPath() + ": Unable to determine an output directory." );
   if ( !fileDir.EndsWith( '/' ) )
      fileDir.Append( '/' );

   /*
    * Output file extension, which defines the output file format.
    */
   String fileExtension = outputExtension.Trimmed();
   if ( fileExtension.IsEmpty() )
      fileExtension = File::ExtractExtension( t->TargetPath() );
   if ( fileExtension.IsEmpty() )
      throw Error( t->TargetPath() + ": Unable to determine an output file extension." );
   if ( !fileExtension.StartsWith( '.' ) )
      fileExtension.Prepend( '.' );

   /*
    * Output file name.
    */
   String fileName = File::ExtractName( t->TargetPath() ).Trimmed();
   if ( t->SubimageIndex() > 0 )
      fileName.Append( String().Format( "_%02d", t->SubimageIndex() ) );
   if ( !outputPrefix.IsEmpty() )
      fileName.Prepend( outputPrefix );
   if ( !outputPostfix.IsEmpty() )
      fileName.Append( outputPostfix );
   if ( fileName.IsEmpty() )
      throw Error( t->TargetPath() + ": Unable to determine an output file name." );

   /*
    * Output file path.
    */
   String outputFilePath = fileDir + fileName + fileExtension;

   console.WriteLn( "<end><cbr><br>Writing output file: " + outputFilePath );

   /*
    * Write dark optimization factors to the console.
    */
   if ( optimizeDarks )
   {
      console.WriteLn( "Dark scaling factors:" );
      for ( int i = 0; i < t->K.Length(); ++i )
      {
         console.WriteLn( String().Format( "k%d = %.3f", i, t->K[i] ) );
         if ( t->K[i] <= NO_DARK_CORRELATION )
            console.WarningLn( String().Format( "** Warning: No correlation "
                               "between the master dark and target frames (channel %d).", i ) );
      }
   }

   /*
    * Write noise evaluation information to the console.
    */
   if ( evaluateNoise )
   {
      console.WriteLn( "Gaussian noise estimates:" );
      for ( int i = 0; i < t->noiseEstimates.Length(); ++i )
         console.WriteLn( String().Format( "s%d = %.3e, n%d = %.3f ",
                              i, t->noiseEstimates[i], i, t->noiseFractions[i] ) + '(' + t->noiseAlgorithms[i] + ')' );
   }

   /*
    * Check for an already existing file, and either overwrite it (but show a
    * warning message if that happens), or find a unique file name, depending
    * on the overwriteExistingFiles parameter.
    */
   if ( File::Exists( outputFilePath ) )
      if ( overwriteExistingFiles )
         console.WarningLn( "** Warning: Overwriting already existing file." );
      else
      {
         outputFilePath = UniqueFilePath( outputFilePath );
         console.NoteLn( "* File already exists, writing to: " + outputFilePath );
      }

   /*
    * Find an installed file format able to write files with the
    * specified extension ...
    */
   FileFormat outputFormat( fileExtension, false/*read*/, true/*write*/ );

   if ( outputFormat.IsDeprecated() )
      console.WarningLn( "** Warning: Deprecated file format: " + outputFormat.Name() );

   FileFormatInstance outputFile( outputFormat );

   /*
    * ... and create a format instance (usually a disk file).
    */
   if ( !outputFile.Create( outputFilePath, outputHints ) )
      throw CatchedException();

   /*
    * Gather relevant image options, including the output sample format (bits
    * per pixel sample and numeric type).
    */
   const OutputFileData& data = t->OutputData();

   ImageOptions options = data.options; // get resolution, sample format, etc.

   /*
    * Determine the output sample format: bits per sample and whether integer
    * or real samples.
    */
   switch ( outputSampleFormat )
   {
   case ICOutputSampleFormat::I16 : options.bitsPerSample = 16; options.ieeefpSampleFormat = false; break;
   case ICOutputSampleFormat::I32 : options.bitsPerSample = 32; options.ieeefpSampleFormat = false; break;
   default:
   case ICOutputSampleFormat::F32 : options.bitsPerSample = 32; options.ieeefpSampleFormat = true;  break;
   case ICOutputSampleFormat::F64 : options.bitsPerSample = 64; options.ieeefpSampleFormat = true;  break;
   }

   outputFile.SetOptions( options );

   /*
    * We want to be sure that the selected (or automatically found) output
    * format is able to store an image with the selected (or automatically
    * determined) sample format. If it doesn't, then the user should know what
    * he/she is doing, but we always inform anyway.
    */
   bool canStore = true;
   if ( options.ieeefpSampleFormat )
      switch ( options.bitsPerSample )
      {
      case 32 : canStore = outputFormat.CanStoreFloat(); break;
      case 64 : canStore = outputFormat.CanStoreDouble(); break;
      }
   else
      switch ( options.bitsPerSample )
      {
      case 16 : canStore = outputFormat.CanStore16Bit(); break;
      case 32 : canStore = outputFormat.CanStore32Bit(); break;
      case 64 : canStore = outputFormat.CanStore64Bit(); break;
      }

   if ( !canStore )
      console.WarningLn( "** Warning: The output format does not support the required sample data format." );

   /*
    * File formats often use format-specific data.
    * Keep track of private data structures.
    */
   if ( data.fsData != nullptr )
      if ( outputFormat.UsesFormatSpecificData() && outputFormat.ValidateFormatSpecificData( data.fsData ) )
         outputFile.SetFormatSpecificData( data.fsData );

   /*
    * Set image properties.
    */
   if ( !data.properties.IsEmpty() )
      if ( outputFormat.CanStoreImageProperties() )
      {
         outputFile.WriteImageProperties( data.properties );
         if ( !outputFormat.SupportsViewProperties() )
            console.WarningLn( "** Warning: The output format cannot store view properties; existing properties have been stored as BLOB data." );
      }
      else
         console.WarningLn( "** Warning: The output format cannot store image properties; existing properties will be lost." );

   /*
    * Add FITS header keywords and preserve existing ones, if possible.
    * N.B.: A COMMENT or HISTORY keyword cannot have a value; these keywords
    * have only the name and comment components.
    */
   if ( outputFormat.CanStoreKeywords() )
   {
      FITSKeywordArray keywords = data.keywords;

      keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Calibration with " + PixInsightVersion::AsString() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Calibration with " + Module->ReadableVersion() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Calibration with ImageCalibration process" );

      if ( !inputHints.IsEmpty() )
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.inputHints: " + IsoString( inputHints ) );
      if ( !outputHints.IsEmpty() )
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.outputHints: " + IsoString( outputHints ) );

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.overscan.enabled: " + IsoString( bool( overscan.enabled ) ) );
      if ( overscan.enabled )
      {
         const Rect& r = overscan.imageRect;
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.overscan.imageRect: {%d,%d,%d,%d}",
                                                        r.x0, r.y0, r.x1, r.y1 ) );
         for ( int i = 0; i < 4; ++i )
            if ( overscan.overscan[i].enabled )
            {
               const Rect& s = overscan.overscan[i].sourceRect;
               const Rect& t = overscan.overscan[i].targetRect;
               keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.overscan[%d].sourceRect: {%d,%d,%d,%d}",
                                                        i, s.x0, s.y0, s.x1, s.y1 ) )
                        << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.overscan[%d].targetRect: {%d,%d,%d,%d}",
                                                        i, t.x0, t.y0, t.x1, t.y1 ) );
            }
      }

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterBias.enabled: " + IsoString( bool( masterBias.enabled ) ) );
      if ( masterBias.enabled )
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterBias.fileName: " +
                                    File::ExtractNameAndExtension( masterBias.path ).To7BitASCII() )
                  << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterBias.calibrate: " + IsoString( bool( calibrateBias ) ) );

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterDark.enabled: " + IsoString( bool( masterDark.enabled ) ) );
      if ( masterDark.enabled )
      {
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterDark.fileName: " +
                                    File::ExtractNameAndExtension( masterDark.path ).To7BitASCII() )
                  << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterDark.calibrate: " + IsoString( bool( calibrateDark ) ) );
         if ( optimizeDarks )
         {
            IsoString darkScalingFactors = IsoString().Format( "ImageCalibration.masterDark.scalingFactors: %.3f", t->K[0] );
            for ( int i = 1; i < t->K.Length(); ++i )
               darkScalingFactors.AppendFormat( " %.3f", t->K[i] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), darkScalingFactors );

            if ( darkOptimizationThreshold > 0 )
               keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.masterDark.optimizationThreshold: %.5f", darkOptimizationThreshold ) )
                        << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.masterDark.optimizationLow: %.4f", darkOptimizationLow ) );

            if ( darkOptimizationWindow > 0 )
               keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.masterDark.optimizationWindow: %d px", darkOptimizationWindow ) );
         }
      }

      keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterFlat.enabled: " + IsoString( bool( masterFlat.enabled ) ) );
      if ( masterFlat.enabled )
      {
         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterFlat.fileName: " +
                                    File::ExtractNameAndExtension( masterFlat.path ).To7BitASCII() )
                  << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    "ImageCalibration.masterFlat.calibrate: " + IsoString( bool( calibrateFlat ) ) );

         IsoString flatScalingFactors = IsoString().Format( "ImageCalibration.masterFlat.scalingFactors: %.6f", t->CalibrationData().fScale[0] );
         for ( int i = 1; i < t->CalibrationData().fScale.Length(); ++i )
            flatScalingFactors.AppendFormat( " %.6f", t->CalibrationData().fScale[i] );
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), flatScalingFactors );
      }

      if ( evaluateNoise )
      {
         /*
          * ### NB: Remove other existing NOISExxx keywords.
          *         *Only* our NOISExxx keywords must be present in the header.
          */
         for ( size_type i = 0; i < keywords.Length(); )
            if ( keywords[i].name.StartsWithIC( "NOISE" ) )
               keywords.Remove( keywords.At( i ) );
            else
               ++i;

         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "Noise evaluation with " + Module->ReadableVersion() );

         IsoString noiseEstimates = IsoString().Format( "ImageCalibration.noiseEstimates: %.3e", t->noiseEstimates[0] );
         for ( int i = 1; i < t->noiseEstimates.Length(); ++i )
            noiseEstimates.AppendFormat( " %.3e", t->noiseEstimates[i] );
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noiseEstimates );

         for ( int i = 0; i < t->noiseEstimates.Length(); ++i )
            keywords << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                           IsoString().Format( "%.3e", t->noiseEstimates[i] ),
                                           IsoString().Format( "Gaussian noise estimate, channel #%d", i ) )
                     << FITSHeaderKeyword( IsoString().Format( "NOISEF%02d", i ),
                                           IsoString().Format( "%.3f", t->noiseFractions[i] ),
                                           IsoString().Format( "Fraction of noise pixels, channel #%d", i ) )
                     << FITSHeaderKeyword( IsoString().Format( "NOISEA%02d", i ),
                                           IsoString( t->noiseAlgorithms[i] ),
                                           IsoString().Format( "Noise evaluation algorithm, channel #%d", i ) );
      }

      if ( outputPedestal != 0 )
      {
         /*
          * ### NB: Remove other existing PEDESTAL keywords.
          *         *Only* our PEDESTAL keyword must be present in the header.
          */
         for ( size_type i = 0; i < keywords.Length(); )
            if ( !keywords[i].name.CompareIC( "PEDESTAL" ) )
               keywords.Remove( keywords.At( i ) );
            else
               ++i;

         keywords << FITSHeaderKeyword( "HISTORY",
                                    IsoString(),
                                    IsoString().Format( "ImageCalibration.outputPedestal: %d", outputPedestal ) )
                  << FITSHeaderKeyword( "PEDESTAL",
                                    IsoString( outputPedestal ),
                                    "Value in DN added to enforce positivity" );
      }

      outputFile.WriteFITSKeywords( keywords );
   }
   else
   {
      console.WarningLn( "** Warning: The output format cannot store FITS header keywords - calibration metadata not embedded." );
   }

   /*
    * Preserve ICC profile, if possible.
    */
   if ( data.profile.IsProfile() )
      if ( outputFormat.CanStoreICCProfiles() )
         outputFile.WriteICCProfile( data.profile );
      else
         console.WarningLn( "** Warning: The output format cannot store color profiles - original ICC profile not embedded." );

   /*
    * Write the output image.
    */
   t->TargetImage()->ResetSelections();
   if ( !outputFile.WriteImage( *t->TargetImage() ) )
      throw CatchedException();

   /*
    * Close the output stream.
    */
   outputFile.Close();

   /*
    * Store output data
    */
   OutputData o;
   o.outputFilePath = outputFilePath;
   if ( masterDark.enabled )
      if ( optimizeDarks )
         for ( int i = 0; i < Min( 3, t->K.Length() ); ++i )
            o.darkScalingFactors[i] = t->K[i];
   if ( evaluateNoise )
      for ( int i = 0; i < Min( 3, t->noiseEstimates.Length() ); ++i )
      {
         o.noiseEstimates[i] = t->noiseEstimates[i];
         o.noiseFractions[i] = t->noiseFractions[i];
         o.noiseAlgorithms[i] = String( t->noiseAlgorithms[i] );
      }
   output.Add( o );
}

/*
 * Returns true if a CFA pattern is matched on the upper left corner of an
 * image. Each element of z is 1 if the corresponding CFA element is zero.
 */
static bool MatchCFAPattern( const Image& image, const int* z )
{
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
      for ( int y = 0; y < 4; ++y )
      {
         const float* f = image.ScanLine( y, c );
         for ( int x = 0; x < 4; ++x, ++f, ++z )
            if ( (*f == 0) != (*z != 0) )
               return false;
      }
   return true;
}

/*
 * Returns true if one of the four RGB Bayer patterns is detected in the
 * specified image.
 */
bool IsBayerPattern( const Image& image )
{
   /*
    * We detect Bayer patterns by looking at zero filter elements at each color
    * channel in the top left corner of the image. A pattern is detected if all
    * zeros are matched in the corresponding 4x4 array.
    */
                                    /*
                                     * B G B G
                                     * G R G R
                                     * B G B G
                                     * G R G R
                                     */
   static const int BayerZeros1[] = { 1, 1, 1, 1,
                                      1, 0, 1, 0,
                                      1, 1, 1, 1,
                                      1, 0, 1, 0,

                                      1, 0, 1, 0,
                                      0, 1, 0, 1,
                                      1, 0, 1, 0,
                                      0, 1, 0, 1,

                                      0, 1, 0, 1,
                                      1, 1, 1, 1,
                                      0, 1, 0, 1,
                                      1, 1, 1, 1 };

                                    /*
                                     * G R G R
                                     * B G B G
                                     * G R G R
                                     * B G B G
                                     */
   static const int BayerZeros2[] = { 1, 0, 1, 0,
                                      1, 1, 1, 1,
                                      1, 0, 1, 0,
                                      1, 1, 1, 1,

                                      0, 1, 0, 1,
                                      1, 0, 1, 0,
                                      0, 1, 0, 1,
                                      1, 0, 1, 0,

                                      1, 1, 1, 1,
                                      0, 1, 0, 1,
                                      1, 1, 1, 1,
                                      0, 1, 0, 1 };

                                    /*
                                     * G B G B
                                     * R G R G
                                     * G B G B
                                     * R G R G
                                     */
   static const int BayerZeros3[] = { 1, 1, 1, 1,
                                      0, 1, 0, 1,
                                      1, 1, 1, 1,
                                      0, 1, 0, 1,

                                      0, 1, 0, 1,
                                      1, 0, 1, 0,
                                      0, 1, 0, 1,
                                      1, 0, 1, 0,

                                      1, 0, 1, 0,
                                      1, 1, 1, 1,
                                      1, 0, 1, 0,
                                      1, 1, 1, 1 };

                                    /*
                                     * R G R G
                                     * G B G B
                                     * R G R G
                                     * G B G B
                                     */
   static const int BayerZeros4[] = { 0, 1, 0, 1,
                                      1, 1, 1, 1,
                                      0, 1, 0, 1,
                                      1, 1, 1, 1,

                                      1, 0, 1, 0,
                                      0, 1, 0, 1,
                                      1, 0, 1, 0,
                                      0, 1, 0, 1,

                                      1, 1, 1, 1,
                                      1, 0, 1, 0,
                                      1, 1, 1, 1,
                                      1, 0, 1, 0 };

   // ### TODO: Include also CMYG CFA zero matrices

   if ( image.NumberOfChannels() != 3 )
      return false;

   return MatchCFAPattern( image, BayerZeros1 ) ||
          MatchCFAPattern( image, BayerZeros2 ) ||
          MatchCFAPattern( image, BayerZeros3 ) ||
          MatchCFAPattern( image, BayerZeros4 );
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );

      if ( !outputDirectory.IsEmpty() && !File::DirectoryExists( outputDirectory ) )
         throw( "The specified output directory does not exist: " + outputDirectory );

      for ( image_list::const_iterator i = targetFrames.Begin(); i != targetFrames.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ) )
            throw( "No such file exists on the local filesystem: " + i->path );
   }

   try
   {
      /*
       * Master frames in use.
       */
      AutoPointer<Image> bias, dark, optimizingDark, flat;

      /*
       * Flag true if the master dark frame is mosaiced with a Color Filter Array
       * (CFA, e.g. a Bayer pattern), either explicitly reported by the file
       * format (DSLR_RAW), detected heuristically (e.g., a CFA stored as a
       * proprietary FITS file), or forced with darkCFADetectionMode = ForceCFA.
       */
      bool isDarkCFA = false;

      /*
       * Per-channel flat scaling factors.
       */
      FVector s;

      /*
       * Reset output data
       */
      output.Clear();

      /*
       * For all errors generated, we want a report on the console. This is
       * customary in PixInsight for all batch processes.
       */
      Exception::EnableConsoleOutput();
      Exception::DisableGUIOutput();

      /*
       * Allow the user to abort the calibration process.
       */
      Console console;
      console.EnableAbort();

      /*
       * Initialize validation geometries.
       */
      geometry = calibratedGeometry = 0;

      /*
       * Overscan regions grouped by target regions.
       */
      overscan_table O;
      if ( overscan.enabled )
         O = BuildOverscanTable();

      /*
       * Load and calibrate all master calibration frames.
       */
      if ( masterBias.enabled || masterDark.enabled || masterFlat.enabled )
      {
         console.WriteLn( "<end><cbr><br>Loading master calibration frames:" );

         /*
          * Load master bias, dark and flat frames, and validate their
          * geometries.
          */

         if ( masterBias.enabled )
            bias = LoadCalibrationFrame( masterBias.path, calibrateBias );

         if ( masterDark.enabled )
         {
            dark = LoadCalibrationFrame( masterDark.path, calibrateDark, &isDarkCFA );

            switch ( darkCFADetectionMode )
            {
            default:
            case ICDarkCFADetectionMode::DetectCFA:
               if ( !isDarkCFA )
                  isDarkCFA = IsBayerPattern( *dark );
               if ( isDarkCFA )
                  console.NoteLn( "<end><cbr>* Note: CFA pattern detected." );
               break;
            case ICDarkCFADetectionMode::ForceCFA:
               if ( !isDarkCFA )
                  console.WarningLn( "<end><cbr>"
                              "** Warning: The file format reports no CFA pattern for the master dark frame, "
                                  "but it is being forced as per process instance parameters." );
               isDarkCFA = true;
               break;
            case ICDarkCFADetectionMode::IgnoreCFA:
               if ( isDarkCFA )
                  console.WarningLn( "<end><cbr>"
                              "** Warning: The file format reports a CFA pattern for the master dark frame, "
                                  "but it is being ignored as per process instance parameters." );
               isDarkCFA = false;
               break;
            }
         }

         if ( masterFlat.enabled )
            flat = LoadCalibrationFrame( masterFlat.path, calibrateFlat );

         Module->ProcessEvents();

         /*
          * Apply overscan corrections.
          */
         if ( overscan.enabled )
         {
            if ( calibrateBias )
               if ( masterBias.enabled )
               {
                  console.WriteLn( "<end><cbr><br>Applying overscan correction: master bias frame ..." );
                  SubtractOverscan( *bias, O, overscan.imageRect );
               }
            if ( calibrateDark )
               if ( masterDark.enabled )
               {
                  console.WriteLn( "<end><cbr><br>Applying overscan correction: master dark frame ..." );
                  SubtractOverscan( *dark, O, overscan.imageRect );
               }
            if ( calibrateFlat )
               if ( masterFlat.enabled )
               {
                  console.WriteLn( "<end><cbr><br>Applying overscan correction: master flat frame ..." );
                  SubtractOverscan( *flat, O, overscan.imageRect );
               }
         }

         Module->ProcessEvents();

         /*
          * Calibrate the master dark frame by subtracting the master bias
          * frame.
          */
         if ( masterDark.enabled )
         {
            if ( calibrateDark )
               if ( masterBias.enabled )
               {
                  console.WriteLn( "<end><cbr><br>Applying bias correction: master dark frame ..." );
                  SubtractBias( *dark, *bias );
               }

            /*
             * If necessary, create the dark optimization window image.
             * NB: This must be done after calibrating the master dark frame.
             */
            if ( optimizeDarks )
            {
               dark->ResetSelections();
               optimizingDark = new Image( *dark );
               if ( isDarkCFA )
                  IntegerResample( -2 ) >> *optimizingDark;

               console.WriteLn( "<end><cbr><br>Dark frame optimization thresholds:" );
               for ( int c = 0; c < optimizingDark->NumberOfChannels(); ++c )
               {
                  optimizingDark->SelectChannel( c );
                  double location = optimizingDark->Median();
                  double scale = optimizingDark->MAD( location ) * 1.4826;
                  double t;
                  if ( darkOptimizationThreshold > 0 ) // ### be backwards-compatible
                  {
                     t = Range( darkOptimizationThreshold, 0.0F, 1.0F );
                     darkOptimizationLow = (t - location)/scale;
                     darkOptimizationThreshold = 0;
                  }
                  else
                  {
                     t = Range( location + darkOptimizationLow * scale, 0.0, 1.0 );
                  }
                  size_type N = optimizingDark->NumberOfPixels();
                  for ( Image::sample_iterator i( *optimizingDark, c ); i; ++i )
                     if ( *i < t )
                     {
                        *i = 0;
                        --N;
                     }
                  console.WriteLn( String().Format( "<end><cbr>Td%d = %.8lf (%llu px = %.3lf%%)",
                                                      c, t, N, 100.0*N/optimizingDark->NumberOfPixels() ) );
                  if ( N < DARK_COUNT_LOW*optimizingDark->NumberOfPixels() )
                     console.WarningLn( String().Format( "** Warning: The dark frame optimization threshold is probably too high (channel %d).", c ) );
                  if ( N < DARK_COUNT_SMALL )
                  {
                     console.WarningLn( String().Format( "** Warning: The dark frame optimization pixel set is too small - disabling dark frame optimization (channel %d).", c ) );
                     optimizingDark.Destroy();
                  }
               }

               if ( optimizingDark )
               {
                  optimizingDark->ResetSelections();

                  if ( darkOptimizationWindow > 0
                     && (darkOptimizationWindow < dark->Width() || darkOptimizationWindow < dark->Height()) )
                  {
                     optimizingDark->SelectRectangle( OptimizingRect( *optimizingDark, darkOptimizationWindow ) );
                     optimizingDark->Crop();
                     optimizingDark->ResetSelections();
                  }
               }
            }
         }

         Module->ProcessEvents();

         /*
          * Calibrate and scale the master flat frame
          */
         if ( masterFlat.enabled )
         {
            /*
             * Calibrate the master flat frame by subtracting bias and dark
             * master frames. If selected, perform a dark optimization for the
             * master flat.
             */
            if ( calibrateFlat )
               if ( masterBias.enabled || masterDark.enabled )
               {
                  console.WriteLn( "<end><cbr><br>Calibrating master flat frame ..." );

                  FVector K;
                  if ( masterDark.enabled )
                     if ( optimizeDarks && optimizingDark )
                     {
                        console.WriteLn( "Optimizing master dark frame:" );
                        K = OptimizeDark( *flat, *optimizingDark, isDarkCFA );
                        for ( int c = 0; c < flat->NumberOfChannels(); ++c )
                        {
                           console.WriteLn( String().Format( "<end><cbr>k%d = %.3f", c, K[c] ) );
                           if ( K[c] <= NO_DARK_CORRELATION )
                              console.WarningLn( String().Format( "** Warning: No correlation between "
                                                "the master dark and master flat frames (channel %d).", c ) );
                        }
                     }
                     else
                        K = FVector( 1.0F, flat->NumberOfChannels() );

                  Module->ProcessEvents();
                  Calibrate( *flat, bias, dark, K, 0, FVector() );
               }

            Module->ProcessEvents();

            /*
             * Compute flat scaling factors as the mean pixel value of each
             * channel.
             */
            console.WriteLn( "<end><cbr>Computing master flat scaling factors ..." );

            s = FVector( flat->NumberOfChannels() );

            flat->SetRangeClipping( 0.00002, 0.99998 );

            for ( int c = 0; c < flat->NumberOfChannels(); ++c )
            {
               flat->SelectChannel( c );
               s[c] = flat->Mean();
               if ( 1 + s[c] == 1 )
                  throw Error( String().Format( "Zero or insignificant scaling factor for flat frame channel# %d. "
                                                "(empty master flat frame image?)", c ) );
            }

            flat->ResetSelections();

            for ( int c = 0; c < flat->NumberOfChannels(); ++c )
               console.WriteLn( String().Format( "s%d = %.6f", c, s[c] ) );
         }
      } // if bias or dark or flat

      Module->ProcessEvents();

      /*
       * Begin light frame calibration process.
       */

      int succeeded = 0;
      int failed = 0;
      int skipped = 0;

      /*
       * Running threads list. Note that IndirectArray<> initializes all item
       * pointers to zero.
       */
      int numberOfThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 );
      thread_list runningThreads( Min( int( targetFrames.Length() ), numberOfThreads ) );

      /*
       * Waiting threads list. We use this list for temporary storage of
       * calibration threads for multiple image files.
       */
      thread_list waitingThreads;

      /*
       * Flag to signal the beginning of the final waiting period, when there
       * are no more pending images but there are still running threads.
       */
      bool waitingForFinished = false;

      /*
       * Prepare working thread data.
       */
      CalibrationThreadData threadData;
      threadData.instance = this;
      threadData.overscan = O;
      threadData.bias = bias;
      threadData.dark = dark;
      threadData.optimizingDark = optimizingDark;
      threadData.isDarkCFA = isDarkCFA;
      threadData.flat = flat;
      threadData.fScale = s;
      threadData.maxProcessors = 1 + (numberOfThreads - runningThreads.Length())/runningThreads.Length();

      /*
       * We'll work on a temporary duplicate of the target frames list. This
       * allows us to modify the working list without altering the instance.
       */
      image_list targets( targetFrames );

      console.WriteLn( String().Format( "<end><cbr><br>Calibration of %u target frames:", targetFrames.Length() ) );
      console.WriteLn( String().Format( "* Using %u worker threads", runningThreads.Length() ) );

      try
      {
         for ( ;; )
         {
            try
            {
               /*
                * Thread watching loop.
                */
               thread_list::iterator i = 0;
               size_type unused = 0;
               for ( thread_list::iterator j = runningThreads.Begin(); j != runningThreads.End(); ++j )
               {
                  // Keep the GUI responsive
                  Module->ProcessEvents();
                  if ( console.AbortRequested() )
                     throw ProcessAborted();

                  if ( *j == 0 )
                  {
                     /*
                      * This is a free thread slot. Ignore it if we don't have
                      * more pending images to feed in.
                      */
                     if ( targets.IsEmpty() && waitingThreads.IsEmpty() )
                     {
                        ++unused;
                        continue;
                     }
                  }
                  else
                  {
                     /*
                      * This is an existing thread. If this thread is still
                      * alive, wait for 0.1 seconds and then continue watching.
                      */
                     if ( (*j)->IsActive() )
                     {
                        pcl::Sleep( 100 );
                        continue;
                     }
                  }

                  /*
                   * If we have a useful free thread slot, or a thread has just
                   * finished, exit the watching loop and work it out.
                   */
                  i = j;
                  break;
               }

               /*
                * Keep watching while there is no useful free thread slots or a
                * finished thread.
                */
               if ( i == 0 )
                  if ( unused == runningThreads.Length() )
                     break;
                  else
                     continue;

               /*
                * At this point we have found either a unused thread slot that
                * we can reuse, or a thread that has just finished execution.
                */
               if ( *i != 0 )
               {
                  /*
                   * This is a just-finished thread.
                   */
                  try
                  {
                     if ( !(*i)->Success() )
                        throw Error( (*i)->ConsoleOutputText() );

                     // Write the calibrated image.
                     WriteCalibratedImage( *i );

                     // Dispose this calibration thread, since we are done with
                     // it. NB: IndirectArray<T>::Delete() sets to zero the
                     // pointer pointed to by the iterator, but does not remove
                     // the array element.
                     runningThreads.Delete( i );
                  }
                  catch ( ... )
                  {
                     // Ensure the thread is also destroyed in the event of
                     // error; we'd enter an infinite loop otherwise!
                     runningThreads.Delete( i );
                     throw;
                  }

                  ++succeeded;
               }

               // Keep the GUI responsive
               Module->ProcessEvents();
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               if ( !waitingThreads.IsEmpty() )
               {
                  /*
                   * If there are threads waiting, pop the first one from the
                   * waiting threads list and put it in the free thread slot
                   * for immediate firing.
                   */
                  *i = *waitingThreads;
                  waitingThreads.Remove( waitingThreads.Begin() );
               }
               else
               {
                  /*
                   * If there are no more target frames to calibrate, simply
                   * wait until all running threads terminate execution.
                   */
                  if ( targets.IsEmpty() )
                  {
                     bool someRunning = false;
                     for ( thread_list::iterator j = runningThreads.Begin(); j != runningThreads.End(); ++j )
                        if ( *j != 0 )
                        {
                           someRunning = true;
                           break;
                        }

                     /*
                      * If there are still running threads, continue waiting.
                      */
                     if ( someRunning )
                     {
                        if ( !waitingForFinished )
                        {
                           console.WriteLn( "<end><cbr><br>* Waiting for running tasks to terminate ..." );
                           waitingForFinished = true;
                        }

                        continue;
                     }

                     /*
                      * If there are no running threads, no waiting threads,
                      * and no pending target frames, then we are done.
                      */
                     break;
                  }

                  /*
                   * We still have pending work to do: Extract the next target
                   * frame from the targets list, load and calibrate it.
                   */
                  ImageItem item = *targets;
                  targets.Remove( targets.Begin() );

                  console.WriteLn( String().Format( "<end><cbr><br>Calibrating target frame %u of %u",
                                                      targetFrames.Length()-targets.Length(),
                                                      targetFrames.Length() ) );
                  if ( !item.enabled )
                  {
                     console.NoteLn( "* Skipping disabled target" );
                     ++skipped;
                     continue;
                  }

                  /*
                   * Create a new thread for this target frame image, or if
                   * this is a multiple-image file, a set of threads for all
                   * subimages in this file.
                   */
                  thread_list threads = LoadTargetFrame( item.path, threadData );

                  /*
                   * Put the new thread --or the first thread if this is a
                   * multiple-image file-- in the free slot.
                   */
                  *i = *threads;
                  threads.Remove( threads.Begin() );

                  /*
                   * Add the rest of subimages in a multiple-image file to the
                   * list of waiting threads.
                   */
                  if ( !threads.IsEmpty() )
                     waitingThreads.Add( threads );
               }

               /*
                * If we have produced a new thread, run it immediately. Note
                * that we support thread CPU affinity, if it has been enabled
                * on the platform via global preferences - hence the second
                * argument to Thread::Start() below.
                */
               if ( *i != 0 )
                  (*i)->Start( ThreadPriority::DefaultMax, i - runningThreads.Begin() );
            } // try
            catch ( ProcessAborted& )
            {
               /*
                * The user has requested to abort the process.
                */
               throw;
            }
            catch ( ... )
            {
               /*
                * The user has requested to abort the process.
                */
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               /*
                * Other errors handled according to the selected error policy.
                */

               ++failed;

               try
               {
                  throw;
               }
               ERROR_HANDLER

               console.ResetStatus();
               console.EnableAbort();

               console.Note( "<end><cbr><br>* Applying error policy: " );

               switch ( onError )
               {
               default: // ?
               case ICOnError::Continue:
                  console.NoteLn( "Continue on error." );
                  continue;

               case ICOnError::Abort:
                  console.NoteLn( "Abort on error." );
                  throw ProcessAborted();

               case ICOnError::AskUser:
                  {
                     console.NoteLn( "Ask on error..." );

                     int r = MessageBox( "<p style=\"white-space:pre;\">"
                        "An error occurred during ImageCalibration execution. What do you want to do?</p>",
                        "ImageCalibration",
                        StdIcon::Error,
                        StdButton::Ignore, StdButton::Abort ).Execute();

                     if ( r == StdButton::Abort )
                     {
                        console.NoteLn( "* Aborting as per user request." );
                        throw ProcessAborted();
                     }

                     console.NoteLn( "* Ignoring error as per user request." );
                     continue;
                  }
               }
            }
         } // for ( ;; )
      } // try

      catch ( ... )
      {
         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate ..." );
         for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            if ( *i != 0 ) (*i)->Abort();
         for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            if ( *i != 0 ) (*i)->Wait();
         runningThreads.Destroy();
         waitingThreads.Destroy();
         throw;
      }

      /*
       * Fail if no images have been calibrated.
       */
      if ( succeeded == 0 )
      {
         if ( failed == 0 )
            throw Error( "No images were calibrated: Empty target frames list? No enabled target frames?" );
         throw Error( "No image could be calibrated." );
      }

      /*
       * Write the final report to the console.
       */
      console.NoteLn( String().Format( "<end><cbr><br>===== ImageCalibration: %u succeeded, %u failed, %u skipped =====",
                                       succeeded, failed, skipped ) );
      return true;
   } // try

   catch ( ... )
   {
      /*
       * All breaking errors are caught here.
       */
      Exception::EnableGUIOutput( !noGUIMessages );
      throw;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void* ImageCalibrationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICTargetFrameEnabledParameter )
      return &targetFrames[tableRow].enabled;
   if ( p == TheICTargetFramePathParameter )
      return targetFrames[tableRow].path.Begin();

   if ( p == TheICInputHintsParameter )
      return inputHints.Begin();
   if ( p == TheICOutputHintsParameter )
      return outputHints.Begin();

   if ( p == TheICPedestalParameter )
      return &pedestal;
   if ( p == TheICPedestalModeParameter )
      return &pedestalMode;
   if ( p == TheICPedestalKeywordParameter )
      return pedestalKeyword.Begin();

   if ( p == TheICOverscanEnabledParameter )
      return &overscan.enabled;

   if ( p == TheICOverscanImageX0Parameter )
      return &overscan.imageRect.x0;
   if ( p == TheICOverscanImageY0Parameter )
      return &overscan.imageRect.y0;
   if ( p == TheICOverscanImageX1Parameter )
      return &overscan.imageRect.x1;
   if ( p == TheICOverscanImageY1Parameter )
      return &overscan.imageRect.y1;

   if ( p == TheICOverscanRegionEnabledParameter )
      return &overscan.overscan[tableRow].enabled;

   if ( p == TheICOverscanSourceX0Parameter )
      return &overscan.overscan[tableRow].sourceRect.x0;
   if ( p == TheICOverscanSourceY0Parameter )
      return &overscan.overscan[tableRow].sourceRect.y0;
   if ( p == TheICOverscanSourceX1Parameter )
      return &overscan.overscan[tableRow].sourceRect.x1;
   if ( p == TheICOverscanSourceY1Parameter )
      return &overscan.overscan[tableRow].sourceRect.y1;

   if ( p == TheICOverscanTargetX0Parameter )
      return &overscan.overscan[tableRow].targetRect.x0;
   if ( p == TheICOverscanTargetY0Parameter )
      return &overscan.overscan[tableRow].targetRect.y0;
   if ( p == TheICOverscanTargetX1Parameter )
      return &overscan.overscan[tableRow].targetRect.x1;
   if ( p == TheICOverscanTargetY1Parameter )
      return &overscan.overscan[tableRow].targetRect.y1;

   if ( p == TheICMasterBiasEnabledParameter )
      return &masterBias.enabled;
   if ( p == TheICMasterBiasPathParameter )
      return masterBias.path.Begin();

   if ( p == TheICMasterDarkEnabledParameter )
      return &masterDark.enabled;
   if ( p == TheICMasterDarkPathParameter )
      return masterDark.path.Begin();

   if ( p == TheICMasterFlatEnabledParameter )
      return &masterFlat.enabled;
   if ( p == TheICMasterFlatPathParameter )
      return masterFlat.path.Begin();

   if ( p == TheICCalibrateBiasParameter )
      return &calibrateBias;
   if ( p == TheICCalibrateDarkParameter )
      return &calibrateDark;
   if ( p == TheICCalibrateFlatParameter )
      return &calibrateFlat;
   if ( p == TheICOptimizeDarksParameter )
      return &optimizeDarks;
   if ( p == TheICDarkOptimizationThresholdParameter )
      return &darkOptimizationThreshold;
   if ( p == TheICDarkOptimizationLowParameter )
      return &darkOptimizationLow;
   if ( p == TheICDarkOptimizationWindowParameter )
      return &darkOptimizationWindow;
   if ( p == TheICDarkCFADetectionModeParameter )
      return &darkCFADetectionMode;

   if ( p == TheICEvaluateNoiseParameter )
      return &evaluateNoise;
   if ( p == TheICNoiseEvaluationAlgorithmParameter )
      return &noiseEvaluationAlgorithm;

   if ( p == TheICOutputDirectoryParameter )
      return outputDirectory.Begin();
   if ( p == TheICOutputExtensionParameter )
      return outputExtension.Begin();
   if ( p == TheICOutputPrefixParameter )
      return outputPrefix.Begin();
   if ( p == TheICOutputPostfixParameter )
      return outputPostfix.Begin();
   if ( p == TheICOutputSampleFormatParameter )
      return &outputSampleFormat;
   if ( p == TheICOutputPedestalParameter )
      return &outputPedestal;
   if ( p == TheICOverwriteExistingFilesParameter )
      return &overwriteExistingFiles;
   if ( p == TheICOnErrorParameter )
      return &onError;
   if ( p == TheICNoGUIMessagesParameter )
      return &noGUIMessages;

   if ( p == TheICOutputFilePathParameter )
      return output[tableRow].outputFilePath.Begin();
   if ( p == TheICDarkScalingFactorRKParameter )
      return output[tableRow].darkScalingFactors.At( 0 );
   if ( p == TheICDarkScalingFactorGParameter )
      return output[tableRow].darkScalingFactors.At( 1 );
   if ( p == TheICDarkScalingFactorBParameter )
      return output[tableRow].darkScalingFactors.At( 2 );
   if ( p == TheICNoiseEstimateRKParameter )
      return output[tableRow].noiseEstimates.At( 0 );
   if ( p == TheICNoiseEstimateGParameter )
      return output[tableRow].noiseEstimates.At( 1 );
   if ( p == TheICNoiseEstimateBParameter )
      return output[tableRow].noiseEstimates.At( 2 );
   if ( p == TheICNoiseFractionRKParameter )
      return output[tableRow].noiseFractions.At( 0 );
   if ( p == TheICNoiseFractionGParameter )
      return output[tableRow].noiseFractions.At( 1 );
   if ( p == TheICNoiseFractionBParameter )
      return output[tableRow].noiseFractions.At( 2 );
   if ( p == TheICNoiseAlgorithmRKParameter )
      return output[tableRow].noiseAlgorithms[0].Begin();
   if ( p == TheICNoiseAlgorithmGParameter )
      return output[tableRow].noiseAlgorithms[1].Begin();
   if ( p == TheICNoiseAlgorithmBParameter )
      return output[tableRow].noiseAlgorithms[2].Begin();

   return 0;
}

bool ImageCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICTargetFramesParameter )
   {
      targetFrames.Clear();
      if ( sizeOrLength > 0 )
         targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheICTargetFramePathParameter )
   {
      targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         targetFrames[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheICInputHintsParameter )
   {
      inputHints.Clear();
      if ( sizeOrLength > 0 )
         inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputHintsParameter )
   {
      outputHints.Clear();
      if ( sizeOrLength > 0 )
         outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheICPedestalKeywordParameter )
   {
      pedestalKeyword.Clear();
      if ( sizeOrLength > 0 )
         pedestalKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheICOverscanRegionsParameter )
   {
      // Fixed-length table, so nothing to do.
   }
   else if ( p == TheICMasterBiasPathParameter )
   {
      masterBias.path.Clear();
      if ( sizeOrLength > 0 )
         masterBias.path.SetLength( sizeOrLength );
   }
   else if ( p == TheICMasterDarkPathParameter )
   {
      masterDark.path.Clear();
      if ( sizeOrLength > 0 )
         masterDark.path.SetLength( sizeOrLength );
   }
   else if ( p == TheICMasterFlatPathParameter )
   {
      masterFlat.path.Clear();
      if ( sizeOrLength > 0 )
         masterFlat.path.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputDirectoryParameter )
   {
      outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputExtensionParameter )
   {
      outputExtension.Clear();
      if ( sizeOrLength > 0 )
         outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputPrefixParameter )
   {
      outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputPostfixParameter )
   {
      outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheICOutputDataParameter )
   {
      output.Clear();
      if ( sizeOrLength > 0 )
         output.Add( OutputData(), sizeOrLength );
   }
   else if ( p == TheICOutputFilePathParameter )
   {
      output[tableRow].outputFilePath.Clear();
      if ( sizeOrLength > 0 )
         output[tableRow].outputFilePath.SetLength( sizeOrLength );
   }
   else if ( p == TheICNoiseAlgorithmRKParameter )
   {
      output[tableRow].noiseAlgorithms[0].Clear();
      if ( sizeOrLength > 0 )
         output[tableRow].noiseAlgorithms[0].SetLength( sizeOrLength );
   }
   else if ( p == TheICNoiseAlgorithmGParameter )
   {
      output[tableRow].noiseAlgorithms[1].Clear();
      if ( sizeOrLength > 0 )
         output[tableRow].noiseAlgorithms[1].SetLength( sizeOrLength );
   }
   else if ( p == TheICNoiseAlgorithmBParameter )
   {
      output[tableRow].noiseAlgorithms[2].Clear();
      if ( sizeOrLength > 0 )
         output[tableRow].noiseAlgorithms[2].SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type ImageCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheICTargetFramesParameter )
      return targetFrames.Length();
   if ( p == TheICTargetFramePathParameter )
      return targetFrames[tableRow].path.Length();
   if ( p == TheICInputHintsParameter )
      return inputHints.Length();
   if ( p == TheICOutputHintsParameter )
      return outputHints.Length();
   if ( p == TheICPedestalKeywordParameter )
      return pedestalKeyword.Length();
   if ( p == TheICOverscanRegionsParameter )
      return 4;
   if ( p == TheICMasterBiasPathParameter )
      return masterBias.path.Length();
   if ( p == TheICMasterDarkPathParameter )
      return masterDark.path.Length();
   if ( p == TheICMasterFlatPathParameter )
      return masterFlat.path.Length();
   if ( p == TheICOutputDirectoryParameter )
      return outputDirectory.Length();
   if ( p == TheICOutputExtensionParameter )
      return outputExtension.Length();
   if ( p == TheICOutputPrefixParameter )
      return outputPrefix.Length();
   if ( p == TheICOutputPostfixParameter )
      return outputPostfix.Length();
   if ( p == TheICOutputDataParameter )
      return output.Length();
   if ( p == TheICOutputFilePathParameter )
      return output[tableRow].outputFilePath.Length();
   if ( p == TheICNoiseAlgorithmRKParameter )
      return output[tableRow].noiseAlgorithms[0].Length();
   if ( p == TheICNoiseAlgorithmGParameter )
      return output[tableRow].noiseAlgorithms[1].Length();
   if ( p == TheICNoiseAlgorithmBParameter )
      return output[tableRow].noiseAlgorithms[2].Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCalibrationInstance.cpp - Released 2017-05-17T17:41:56Z
