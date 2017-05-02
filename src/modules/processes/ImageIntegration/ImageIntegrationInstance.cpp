//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.14.00.0390
// ----------------------------------------------------------------------------
// ImageIntegrationInstance.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "ImageIntegrationInstance.h"
#include "IntegrationCache.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoPointer.h>
#include <pcl/DrizzleData.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ICCProfile.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/MetaModule.h>
#include <pcl/Settings.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Version.h>
#include <pcl/View.h>

#define IKSS_K        4.0
#define IKSS_NORMAL   0.991
#define IKSS_ACCURACY 1.0e-06

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIntegrationInstance::ImageIntegrationInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_images(),
   p_inputHints(),
   p_combination( IICombination::Default ),
   p_normalization( IINormalization::Default ),
   p_weightMode( IIWeightMode::Default ),
   p_weightKeyword( TheIIWeightKeywordParameter->DefaultValue() ),
   p_weightScale( IIWeightScale::Default ),
   p_ignoreNoiseKeywords( TheIIIgnoreNoiseKeywordsParameter->DefaultValue() ),
   p_rejection( IIRejection::Default ),
   p_rejectionNormalization( IIRejectionNormalization::Default ),
   p_minMaxLow( TheIIMinMaxLowParameter->DefaultValue() ),
   p_minMaxHigh( TheIIMinMaxHighParameter->DefaultValue() ),
   p_pcClipLow( TheIIPercentileLowParameter->DefaultValue() ),
   p_pcClipHigh( TheIIPercentileHighParameter->DefaultValue() ),
   p_sigmaLow( TheIISigmaLowParameter->DefaultValue() ),
   p_sigmaHigh( TheIISigmaHighParameter->DefaultValue() ),
   p_linearFitLow( TheIILinearFitLowParameter->DefaultValue() ),
   p_linearFitHigh( TheIILinearFitHighParameter->DefaultValue() ),
   p_ccdGain( TheIICCDGainParameter->DefaultValue() ),
   p_ccdReadNoise( TheIICCDReadNoiseParameter->DefaultValue() ),
   p_ccdScaleNoise( TheIICCDScaleNoiseParameter->DefaultValue() ),
   p_clipLow( TheIIClipLowParameter->DefaultValue() ),
   p_clipHigh( TheIIClipHighParameter->DefaultValue() ),
   p_rangeClipLow( TheIIRangeClipLowParameter->DefaultValue() ),
   p_rangeLow( TheIIRangeLowParameter->DefaultValue() ),
   p_rangeClipHigh( TheIIRangeClipHighParameter->DefaultValue() ),
   p_rangeHigh( TheIIRangeHighParameter->DefaultValue() ),
   p_reportRangeRejection( TheIIReportRangeRejectionParameter->DefaultValue() ),
   p_mapRangeRejection( TheIIMapRangeRejectionParameter->DefaultValue() ),
   p_generate64BitResult( TheIIGenerate64BitResultParameter->DefaultValue() ),
   p_generateRejectionMaps( TheIIGenerateRejectionMapsParameter->DefaultValue() ),
   p_generateIntegratedImage( TheIIGenerateIntegratedImageParameter->DefaultValue() ),
   p_generateDrizzleData( TheIIGenerateDrizzleDataParameter->DefaultValue() ),
   p_closePreviousImages( TheIIClosePreviousImagesParameter->DefaultValue() ),
   p_bufferSizeMB( TheIIBufferSizeParameter->DefaultValue() ),
   p_stackSizeMB( TheIIStackSizeParameter->DefaultValue() ),
   p_useROI( TheIIUseROIParameter->DefaultValue() ),
   p_roi( 0 ),
   p_useCache( TheIIUseCacheParameter->DefaultValue() ),
   p_evaluateNoise( TheIIEvaluateNoiseParameter->DefaultValue() ),
   p_mrsMinDataFraction( TheIIMRSMinDataFractionParameter->DefaultValue() ),
   p_noGUIMessages( TheIINoGUIMessagesParameter->DefaultValue() ),
   p_useFileThreads( TheIIUseFileThreadsParameter->DefaultValue() ),
   p_fileThreadOverload( TheIIFileThreadOverloadParameter->DefaultValue() ),
   o_output()
{
}

// ----------------------------------------------------------------------------

ImageIntegrationInstance::ImageIntegrationInstance( const ImageIntegrationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ImageIntegrationInstance::Assign( const ProcessImplementation& p )
{
   const ImageIntegrationInstance* x = dynamic_cast<const ImageIntegrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_images                  = x->p_images;
      p_inputHints              = x->p_inputHints;
      p_combination             = x->p_combination;
      p_normalization           = x->p_normalization;
      p_weightMode              = x->p_weightMode;
      p_weightKeyword           = x->p_weightKeyword;
      p_weightScale             = x->p_weightScale;
      p_ignoreNoiseKeywords     = x->p_ignoreNoiseKeywords;
      p_rejection               = x->p_rejection;
      p_rejectionNormalization  = x->p_rejectionNormalization;
      p_minMaxLow               = x->p_minMaxLow;
      p_minMaxHigh              = x->p_minMaxHigh;
      p_pcClipLow               = x->p_pcClipLow;
      p_pcClipHigh              = x->p_pcClipHigh;
      p_sigmaLow                = x->p_sigmaLow;
      p_sigmaHigh               = x->p_sigmaHigh;
      p_linearFitLow            = x->p_linearFitLow;
      p_linearFitHigh           = x->p_linearFitHigh;
      p_ccdGain                 = x->p_ccdGain;
      p_ccdReadNoise            = x->p_ccdReadNoise;
      p_ccdScaleNoise           = x->p_ccdScaleNoise;
      p_clipLow                 = x->p_clipLow;
      p_clipHigh                = x->p_clipHigh;
      p_rangeClipLow            = x->p_rangeClipLow;
      p_rangeLow                = x->p_rangeLow;
      p_rangeClipHigh           = x->p_rangeClipHigh;
      p_rangeHigh               = x->p_rangeHigh;
      p_reportRangeRejection    = x->p_reportRangeRejection;
      p_mapRangeRejection       = x->p_mapRangeRejection;
      p_generate64BitResult     = x->p_generate64BitResult;
      p_generateRejectionMaps   = x->p_generateRejectionMaps;
      p_generateIntegratedImage = x->p_generateIntegratedImage;
      p_generateDrizzleData     = x->p_generateDrizzleData;
      p_closePreviousImages     = x->p_closePreviousImages;
      p_bufferSizeMB            = x->p_bufferSizeMB;
      p_stackSizeMB             = x->p_stackSizeMB;
      p_useROI                  = x->p_useROI;
      p_roi                     = x->p_roi;
      p_useCache                = x->p_useCache;
      p_evaluateNoise           = x->p_evaluateNoise;
      p_mrsMinDataFraction      = x->p_mrsMinDataFraction;
      p_noGUIMessages           = x->p_noGUIMessages;
      p_useFileThreads          = x->p_useFileThreads;
      p_fileThreadOverload      = x->p_fileThreadOverload;
      o_output                  = x->o_output;
   }
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "ImageIntegration can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_images.Length() < 3 )
   {
      whyNot = "This instance of ImageIntegration defines less than three source images.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Estimation of the standard deviation of the noise, assuming a Gaussian
 * noise distribution.
 *
 * - Use MRS noise evaluation when the algorithm converges for 4 >= J >= 2
 *
 * - Use k-sigma noise evaluation when either MRS doesn't converge or when the
 *   length of the set of noise pixels is less than the specified fraction of
 *   the image area (minDataFraction).
 *
 * - Automatically iterate to find the highest wavelet layer where noise can be
 *   successfully evaluated, in the [1,4] range.
 */
static DVector EvaluateNoise( const ImageVariant& image, float minDataFraction )
{
   /*
    * 5x5 B3-spline wavelet scaling function. Used by the noise estimation
    * routines.
    *
    * Kernel filter coefficients:
    *
    *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256,
    *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
    *   3.0/128, 3.0/32, 9.0/64,  3.0/32, 3.0/128,
    *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
    *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256
    *
    * Note that we use this scaling function as a separable filter (row and
    * column vectors) for performance reasons.
    */
   // Separable filter coefficients.
   static const float B3S_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
   // Gaussian noise scaling factors
   static const float B3S_kj[] =
   { 0.8907F, 0.2007F, 0.0856F, 0.0413F, 0.0205F, 0.0103F, 0.0052F, 0.0026F, 0.0013F, 0.0007F };

   SpinStatus spin;
   image->SetStatusCallback( &spin );
   image->Status().Initialize( "MRS noise evaluation" );
   image->Status().DisableInitialization();

   Console console;

   image->ResetSelections();

   ATrousWaveletTransform::WaveletScalingFunction B3S( SeparableFilter( B3S_hv, B3S_hv, 5 ) );

   DVector noise( image->NumberOfNominalChannels() );

   for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
   {
      Module->ProcessEvents();
      if ( console.AbortRequested() )
         throw ProcessAborted();

      image->SelectChannel( c );

      double s0 = 0;
      for ( int n = 4; ; )
      {
         ATrousWaveletTransform W( B3S, n );
         W << image;
         if ( n == 4 )
            s0 = W.NoiseKSigma( 0, image, 0.00002F, 0.99998F, 3 )/B3S_kj[0];
         size_type N;
         noise[c] = W.NoiseMRS( image, B3S_kj, s0, 3, &N, 0.00002F, 0.99998F );

         if ( noise[c] > 0 && N >= size_type( minDataFraction*image->NumberOfPixels() ) )
            break;

         if ( --n == 1 )
         {
            console.WarningLn( "<end><cbr>** Warning: No convergence in MRS noise evaluation routine"
                               " - using K-sigma noise estimate." );
            noise[c] = s0;
            break;
         }
      }
   }

   image->Status().Complete();
   image->Status().EnableInitialization();
   image->SetStatusCallback( nullptr );
   image->ResetSelections();

   return noise;
}

// ----------------------------------------------------------------------------

/*
 * Estimation of the signal-to-noise ratio function:
 *
 * SNR(f,g) = E(f^2) / E((f - g)^2)
 *
 * where f is a distribution function, g is a sample, and E(.) is the expected
 * value of the argument. The denominator is the mean square error, which we
 * approximate with the variance of the noise.
 */
template <class P>
static DVector EvaluateSNR( const GenericImage<P>& image, const DVector& noise, double low, double high )
{
   image.PushSelections();
   image.SetRangeClipping( low, high );

   DVector snr( image.NumberOfNominalChannels() );
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
   {
      image.SelectChannel( c );
      snr[c] = image.MeanOfSquares()/noise[c]/noise[c];
   }

   image.PopSelections();

   return snr;
}

static DVector EvaluateSNR( const ImageVariant& image, const DVector& noise, double low = 0, double high = 0.85 )
{
   if ( image )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: return EvaluateSNR( static_cast<const FImage&>( *image ), noise, low, high );
         case 64: return EvaluateSNR( static_cast<const DImage&>( *image ), noise, low, high );
         }
   return DVector(); // ?!
}

// ----------------------------------------------------------------------------

/*
 * Iterative k-sigma / biweight midvariance robust estimator of scale.
 */
template <class P>
static DVector EvaluateIKSS( DVector& location, const GenericImage<P>& image, double low = 0, double high = 1 )
{
   location = DVector( image.NumberOfNominalChannels() );
   DVector scale( image.NumberOfNominalChannels() );

   Console console;

   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
   {
      Module->ProcessEvents();
      if ( console.AbortRequested() )
         throw ProcessAborted();

      Array<double> A;
      for ( typename GenericImage<P>::const_sample_iterator i( image, c ); i; ++i )
      {
         double a; P::FromSample( a, *i );
         if ( a > low && a < high )
            A << a;
      }

      A.Sort();

      double s0 = 1;
      Array<double>::iterator i = A.Begin();
      Array<double>::iterator j = A.End();
      for ( int it = 1; ; ++it )
      {
         Module->ProcessEvents();
         if ( console.AbortRequested() )
            throw ProcessAborted();

         distance_type n = j - i;
         if ( n < 1 )
         {
            scale[c] = location[c] = 0;
            break;
         }

         distance_type n2 = n >> 1;
         double m = *(i+n2);
         if ( (n & 1) == 0 )
            m = (m + *(i+n2-1))/2;

         ++image.Status();

         double s = Sqrt( pcl::BiweightMidvariance( i, j, m, pcl::MAD( i, j, m ) ) );
         if ( 1 + s == 1 )
         {
            location[c] = m;
            scale[c] = 0;
            break;
         }

         if ( (s0 - s)/s0 < IKSS_ACCURACY )
         {
            location[c] = m;
            scale[c] = IKSS_NORMAL*s;
            break;
         }

         s0 = s;

         ++image.Status();

         for ( double v0 = m - IKSS_K*s; *i < v0; ++i ) {}
         for ( double v1 = m + IKSS_K*s; *(j-1) > v1; --j ) {}
      }
   }

   return scale;
}

static DVector EvaluateIKSS( DVector& location, const ImageVariant& image, double low = 0, double high = 1 )
{
   if ( image )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: return EvaluateIKSS( location, static_cast<const Image&>( *image ), low, high );
         case 64: return EvaluateIKSS( location, static_cast<const DImage&>( *image ), low, high );
         }
   return DVector(); // ?!
}

template <class P>
static DVector EvaluateIKSS( const GenericImage<P>& image, double low = 0, double high = 1 )
{
   DVector dum;
   return EvaluateIKSS( dum, image, low, high );
}

// static DVector EvaluateIKSS( const ImageVariant& image, double low = 0, double high = 1 )
// {
//    DVector dum;
//    return EvaluateIKSS( dum, image, low, high );
// }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class IntegrationFile
{
public:

   static void OpenFiles( const ImageIntegrationInstance& );

   virtual ~IntegrationFile()
   {
   }

   String Path() const
   {
      return m_file.IsValid() ? m_file->FilePath() : String();
   }

   String DrizzleDataPath() const
   {
      return m_drzPath;
   }

   const float* operator []( int row ) const
   {
      if ( s_incremental )
         return m_buffer[row] + s_roi.x0;
      return m_image->ScanLine( row, m_currentChannel );
   }

   bool HasLocation() const
   {
      return !m_location.IsEmpty();
   }

   double Location( int c ) const
   {
      return m_location[c];
   }

   bool HasDispersion() const
   {
      return !m_dispersion.IsEmpty();
   }

   double Dispersion( int c ) const
   {
      return m_dispersion[c];
   }

   bool HasNoise() const
   {
      return !m_noise.IsEmpty();
   }

   double Noise( int c ) const
   {
      return m_noise[c];
   }

   bool HasScale() const
   {
      return !m_scale.IsEmpty();
   }

   double Scale( int c ) const
   {
      return m_scale[c];
   }

   bool HasZeroOffset() const
   {
      return HasLocation();
   }

   double ZeroOffset( int c ) const
   {
      return s_files[0]->m_location[c] - m_location[c];
   }

   bool HasWeight() const
   {
      return !m_weight.IsEmpty();
   }

   double Weight( int c ) const
   {
      return m_weight[c];
   }

   bool HasImageWeight() const
   {
      return HasWeight();
   }

   double ImageWeight( int c ) const
   {
      return m_weight[c]/s_files[0]->m_weight[c];
   }

   double Mean( int c ) const
   {
      return m_mean[c];
   }

   double AvgDev( int c ) const
   {
      return m_avgDev[c];
   }

   double MAD( int c ) const
   {
      return m_mad[c];
   }

   double BWMV( int c ) const
   {
      return m_bwmv[c];
   }

   double PBMV( int c ) const
   {
      return m_pbmv[c];
   }

   double Sn( int c ) const
   {
      return m_sn[c];
   }

   double Qn( int c ) const
   {
      return m_qn[c];
   }

   double IKSS( int c ) const
   {
      return m_ikss[c];
   }

   double IKSL( int c ) const
   {
      return m_iksl[c];
   }

   double Median( int c ) const
   {
      return m_median[c];
   }

   float Pedestal() const
   {
      return m_pedestal;
   }

   void AppendDrizzleHighRejectionData( int x, int y, int channel )
   {
      if ( m_drzRejectionMap.IsEmpty() )
         InitializeDrizzleRejectionMap();
      m_drzRejectionMap( x, y, channel ) |= 1;
   }

   void AppendDrizzleLowRejectionData( int x, int y, int channel )
   {
      if ( m_drzRejectionMap.IsEmpty() )
         InitializeDrizzleRejectionMap();
      m_drzRejectionMap( x, y, channel ) |= 2;
   }

   void ToDrizzleData( DrizzleData& ) const;

   static bool Incremental()
   {
      return s_incremental;
   }

   static int NumberOfFiles()
   {
      return int( s_files.Length() );
   }

   static IntegrationFile& FileByIndex( int i )
   {
      return *s_files[i];
   }

   static int Width()
   {
      return s_roi.Width();
   }

   static int Height()
   {
      return s_roi.Height();
   }

   static int NumberOfChannels()
   {
      return s_numberOfChannels;
   }

   static size_type NumberOfPixels()
   {
      return size_type( s_roi.Width() ) * size_type( s_roi.Height() );
   }

   static bool IsColor()
   {
      return s_isColor;
   }

   static int BufferRows()
   {
      return s_bufferRows;
   }

   static void UpdateBuffers( int startRow, int channel )
   {
      for ( file_list::const_iterator i = s_files.Begin(); i != s_files.End(); ++i )
         (*i)->Read( startRow, channel );
   }

   static void CloseAll()
   {
      s_files.Destroy();
   }

private:

   typedef IndirectArray<IntegrationFile> file_list;

   AutoPointer<FileFormatInstance> m_file;
   String                          m_drzPath;
   UInt8Image                      m_drzRejectionMap;
   AutoPointer<Image>              m_image;  // used for non-incremental file reading
   int                             m_currentChannel;
   FMatrix                         m_buffer; // used for incremental file reading
   DVector                         m_scale;
   DVector                         m_mean;
   DVector                         m_median;
   DVector                         m_avgDev;
   DVector                         m_mad;
   DVector                         m_bwmv;
   DVector                         m_pbmv;
   DVector                         m_sn;
   DVector                         m_qn;
   DVector                         m_ikss;
   DVector                         m_iksl;
   DVector                         m_location;
   DVector                         m_dispersion;
   DVector                         m_noise;
   DVector                         m_weight;
   float                           m_pedestal;

   static file_list                s_files;
   static Rect                     s_roi;
   static int                      s_width;
   static int                      s_height;
   static int                      s_numberOfChannels;
   static bool                     s_isColor;
   static bool                     s_incremental;
   static int                      s_bufferRows;

   struct ThreadIndex
   {
      size_type itemIndex, fileIndex;

      ThreadIndex( size_type i, size_type f ) : itemIndex( i ), fileIndex( f )
      {
      }
   };

   class OpenFileThread : public Thread
   {
   public:

      OpenFileThread( const ThreadIndex& index, const ImageIntegrationInstance& instance, bool isReference = false ) :
         m_file( s_files[index.fileIndex] ),
         m_item( instance.p_images[index.itemIndex] ),
         m_instance( instance ),
         m_isReference( isReference ),
         m_success( false )
      {
      }

      virtual void Run()
      {
         try
         {
            m_file->Open( m_item.path, m_item.drzPath, m_instance, m_isReference );
            m_success = true;
         }
         catch ( ... )
         {
            if ( IsRootThread() )
               throw;

            String text = ConsoleOutputText();
            ClearConsoleOutputText();
            try
            {
               throw;
            }
            ERROR_HANDLER
            m_errorInfo = ConsoleOutputText();
            ClearConsoleOutputText();
            Console().Write( text );
         }
      }

      String FilePath() const
      {
         return m_item.path;
      }

      String DrizzleFilePath() const
      {
         return m_item.drzPath;
      }

      bool Succeeded() const
      {
         return m_success;
      }

      String ErrorInfo() const
      {
         return m_errorInfo;
      }

   private:

            IntegrationFile*                     m_file;
      const ImageIntegrationInstance::ImageItem  m_item;
      const ImageIntegrationInstance&            m_instance;
            bool                                 m_isReference : 1;
            bool                                 m_success     : 1;
            String                               m_errorInfo;
   };

   typedef IndirectArray<OpenFileThread> file_thread_list;

   IntegrationFile() = default;

   void Open( const String&, const String&, const ImageIntegrationInstance&, bool isReference );

   void Read( int startRow, int channel )
   {
      if ( s_incremental )
      {
         startRow += s_roi.y0;
         if ( !m_file->ReadSamples( *m_buffer, startRow, Min( s_bufferRows, s_roi.y1 - startRow ), channel ) )
            throw CatchedException();
      }
      else
         m_currentChannel = channel;
   }

   double KeywordValue( const IsoString& keyName );

   template <class S>
   double KeywordValue( const S& keyName )
   {
      return KeywordValue( IsoString( keyName ) );
   }

   void InitializeDrizzleRejectionMap()
   {
      m_drzRejectionMap.AllocateData( s_width, s_height, s_numberOfChannels );
      m_drzRejectionMap.Zero();
   }

   void ResetCacheableData();
   void AddToCache( const String& path ) const;
   bool GetFromCache( const String& path );
};

IntegrationFile::file_list IntegrationFile::s_files;
Rect IntegrationFile::s_roi = Rect( 0 );
int IntegrationFile::s_width = 0;
int IntegrationFile::s_height = 0;
int IntegrationFile::s_numberOfChannels = 0;
bool IntegrationFile::s_isColor = false;
bool IntegrationFile::s_incremental = false;
int IntegrationFile::s_bufferRows = 0;

// ----------------------------------------------------------------------------

void IntegrationFile::OpenFiles( const ImageIntegrationInstance& instance )
{
   CloseAll();

   Console console;
   console.WriteLn( "<end><cbr><br>Opening files:<br>" );

   Array<ThreadIndex> pendingItems;
   for ( size_type i = 0, f = 0; i < instance.p_images.Length(); ++i )
      if ( instance.p_images[i].enabled )
         pendingItems << ThreadIndex( i, f++ );
      else
         console.WriteLn( "* Skipping disabled image: " + instance.p_images[i].path );

   if ( pendingItems.IsEmpty() )
      throw Error( "No images have been selected." );

   if ( pendingItems.Length() < 3 )
      throw Error( "ImageIntegration requires at least three images; this instance defines only " +
                     String( pendingItems.Length() ) + " image(s)." );

   if ( instance.p_rejection == IIRejection::LinearFit )
      if ( pendingItems.Length() < 5 )
         throw Error( "Linear fit rejection requires at least five images; only " +
                        String( pendingItems.Length() ) + " images have been selected." );

   for ( size_type i = 0; i < pendingItems.Length(); ++i )
      s_files << new IntegrationFile;

   OpenFileThread( *pendingItems, instance, true/*isReference*/ ).Run();
   pendingItems.Remove( pendingItems.Begin() );

   int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * instance.p_fileThreadOverload );

   if ( instance.p_useFileThreads )
   {
      int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
      file_thread_list runningThreads( numberOfThreads ); // N.B.: all pointers are set to nullptr by IndirectArray's ctor.
      console.NoteLn( String().Format( "<end><br>* Using %d worker threads.", numberOfThreads ) );

      try
      {
         /*
          * Thread watching loop.
          */
         for ( int count = 0; ; )
         {
            int running = 0;
            for ( file_thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            {
               Module->ProcessEvents();
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               if ( *i != nullptr )
               {
                  if ( !(*i)->Wait( 150 ) )
                  {
                     ++running;
                     continue;
                  }

                  /*
                   * A thread has just finished.
                   */
                  (*i)->FlushConsoleOutputText();
                  console.WriteLn();
                  String errorInfo;
                  if ( !(*i)->Succeeded() )
                     errorInfo = (*i)->ErrorInfo();

                  /*
                   * N.B.: IndirectArray<>::Delete() sets to zero the pointer
                   * pointed to by the iterator, but does not remove the array
                   * element.
                   */
                  runningThreads.Delete( i );

                  if ( !errorInfo.IsEmpty() )
                     throw Error( errorInfo );

                  ++count;
               }

               /*
                * If there are one or more items pending, create a new thread
                * and fire it immediately.
                */
               if ( !pendingItems.IsEmpty() )
               {
                  *i = new OpenFileThread( *pendingItems, instance );
                  pendingItems.Remove( pendingItems.Begin() );
                  size_type threadIndex = i - runningThreads.Begin();
                  console.NoteLn( String().Format( "<end><cbr>[%03u] ", threadIndex ) + (*i)->FilePath() );
                  (*i)->Start( ThreadPriority::DefaultMax/*, threadIndex*/ );
                  ++running;
                  if ( pendingItems.IsEmpty() )
                     console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                  else if ( count > 0 )
                     console.WriteLn();
               }
            }

            if ( !running )
               break;
         }
      }
      catch ( ... )
      {
         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
         for ( OpenFileThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( OpenFileThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Wait();
         runningThreads.Destroy();
         throw;
      }
   }
   else // !p_useFileThreads
   {
      for ( auto item : pendingItems )
      {
         console.WriteLn( "<end><cbr><br>" );
         OpenFileThread( item, instance ).Run();
      }
   }
}

// ----------------------------------------------------------------------------

void IntegrationFile::Open( const String& path, const String& drzPath, const ImageIntegrationInstance& instance, bool isReference )
{
   Console console;

   console.WriteLn( "<end><cbr>" + path );
   if ( !drzPath.IsEmpty() )
      if ( instance.p_generateDrizzleData )
         console.WriteLn( drzPath );
      else
         console.WarningLn( "** Warning: No drizzle integration data will be generated." );

   FileFormat format( File::ExtractExtension( path ), true/*read*/, false/*write*/ );

   m_file = new FileFormatInstance( format );

   ImageDescriptionArray images;

   if ( !m_file->Open( images, path, instance.p_inputHints ) )
      throw CatchedException();

   if ( images.IsEmpty() )
      throw Error( m_file->FilePath() + ": Empty image file." );

   if ( images.Length() > 1 )
      console.NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in input file.", images.Length()-1 ) );

   if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
      throw Error( m_file->FilePath() + ": Invalid or unsupported image." );

   if ( isReference )
   {
      s_width = images[0].info.width;
      s_height = images[0].info.height;

      if ( instance.p_useROI )
      {
         s_roi = Rect( s_width, s_height ).Intersection( instance.p_roi.Ordered() );
         if ( !s_roi.IsRect() )
            throw Error( "Invalid ROI coordinates" );
      }
      else
         s_roi = Rect( s_width, s_height );

      s_numberOfChannels = images[0].info.numberOfChannels;
      s_isColor = images[0].info.colorSpace != ColorSpace::Gray;

      s_incremental = format.CanReadIncrementally();
      if ( s_incremental )
         s_bufferRows = Range( int( (uint64( instance.p_bufferSizeMB )*1024*1024)/(s_width * sizeof( float )) ), 1, s_roi.Height() );
      else
      {
         console.NoteLn( "<end><cbr><br>* Incremental image integration disabled due to lack of file format support: " + format.Name() + "<br>" );
         s_bufferRows = s_roi.Height();
      }
   }
   else
   {
      if ( s_incremental )
         if ( !format.CanReadIncrementally() )
            throw Error( "Invalid combination of file formats with and without incremental file read capabilities: " + format.Name() );

      if ( s_width != images[0].info.width ||
           s_height != images[0].info.height ||
           s_numberOfChannels != images[0].info.numberOfChannels )
         throw Error( m_file->FilePath() + ": Incompatible image geometry." );
   }

   if ( s_incremental )
      m_buffer = FMatrix( s_bufferRows, s_width );
   else
   {
      m_image = new Image( (void*)0, 0, 0 ); // shared image
      if ( !m_file->ReadImage( *m_image ) )
         throw CatchedException();
   }

   m_drzPath         = String();
   m_drzRejectionMap = UInt8Image();
   m_scale           = DVector();
   m_mean            = DVector();
   m_median          = DVector();
   m_avgDev          = DVector();
   m_mad             = DVector();
   m_bwmv            = DVector();
   m_pbmv            = DVector();
   m_sn              = DVector();
   m_qn              = DVector();
   m_ikss            = DVector();
   m_iksl            = DVector();
   m_location        = DVector();
   m_dispersion      = DVector();
   m_noise           = DVector();
   m_pedestal        = 0;

   bool generateOutput = instance.p_generateIntegratedImage || instance.p_generateDrizzleData;

   bool needNoise =               instance.p_generateIntegratedImage &&
                                 (instance.p_evaluateNoise ||
                                  instance.p_combination == IICombination::Average &&
                                  instance.p_weightMode == IIWeightMode::NoiseEvaluationWeight)
                               || instance.p_generateDrizzleData &&
                                  instance.p_weightMode == IIWeightMode::NoiseEvaluationWeight;

   bool needScale = needNoise  || generateOutput &&
                                 (instance.p_normalization == IINormalization::AdditiveWithScaling ||
                                  instance.p_normalization == IINormalization::MultiplicativeWithScaling)
                               || instance.p_rejection != IIRejection::NoRejection &&
                                  instance.p_rejectionNormalization == IIRejectionNormalization::Scale;

   bool needMedian = needScale || generateOutput &&
                                 (instance.p_normalization != IINormalization::NoNormalization ||
                                  instance.p_weightMode == IIWeightMode::SignalWeight ||
                                  instance.p_weightMode == IIWeightMode::MedianWeight)
                               || instance.p_rejection != IIRejection::NoRejection &&
                                  instance.p_rejectionNormalization == IIRejectionNormalization::EqualizeFluxes;

   bool needIKSS =                instance.p_weightScale == IIWeightScale::IKSS
                               || instance.p_generateIntegratedImage &&
                                  instance.p_evaluateNoise;

   bool needAvgDev =              instance.p_weightScale == IIWeightScale::AvgDev
                               || generateOutput &&
                                  instance.p_weightMode == IIWeightMode::SignalWeight;

   bool needMean =                generateOutput &&
                                  instance.p_weightMode == IIWeightMode::AverageWeight;

   if ( instance.p_useCache && GetFromCache( path ) )
      console.NoteLn( "<end><cbr>* Retrieved data from file cache." );

   bool doMean   = m_mean.IsEmpty()   && needMean;
   bool doMedian = m_median.IsEmpty() && needMedian;
   bool doAvgDev = m_avgDev.IsEmpty() && needAvgDev;
   bool doMAD    = m_mad.IsEmpty()    && instance.p_weightScale == IIWeightScale::MAD;
   bool doBWMV   = m_bwmv.IsEmpty()   && instance.p_weightScale == IIWeightScale::BWMV;
   bool doPBMV   = m_pbmv.IsEmpty()   && instance.p_weightScale == IIWeightScale::PBMV;
   bool doSn     = m_sn.IsEmpty()     && instance.p_weightScale == IIWeightScale::Sn;
   bool doQn     = m_qn.IsEmpty()     && instance.p_weightScale == IIWeightScale::Qn;
   bool doIKSS   = (m_ikss.IsEmpty() || m_iksl.IsEmpty()) && needIKSS;
   bool doNoise  = m_noise.IsEmpty()  && needNoise;

   SpinStatus spin;

   if ( doMean || doMedian || doAvgDev || doMAD || doBWMV || doPBMV || doSn || doQn || doIKSS || doNoise )
   {
      if ( s_incremental )
      {
         m_image = new Image( (void*)0, 0, 0 ); // shared image
         if ( !m_file->ReadImage( *m_image ) )
            throw CatchedException();
      }

      m_image->SetStatusCallback( &spin );
      m_image->Status().Initialize( "<end><cbr>Computing image statistics" );
      m_image->Status().DisableInitialization();
   }

   if ( doMean || doMedian || doAvgDev || doMAD || doBWMV || doPBMV || doSn || doQn )
   {
      ImageStatistics S;
      S.SetRejectionLimits( 0.00002, 0.99998 ); // 2e-5 ~= 1.2/65536
      S.EnableRejection();
      S.DisableExtremes();
      S.DisableSumOfSquares();
      S.DisableVariance();

      if ( !doMedian && !doAvgDev && !doMAD && !doBWMV && !doPBMV )
      {
         S.DisableMedian();
      }
      else
      {
         if ( !doBWMV )
         {
            S.DisableBWMV();
            if ( !doMAD )
               S.DisableMAD();
         }

         if ( !doPBMV )
            S.DisablePBMV();

         if ( !doAvgDev )
            S.DisableAvgDev();
      }

      // Sn and Qn are disabled by default in ImageStatistics
      if ( doSn )
         S.EnableSn();
      if ( doQn )
         S.EnableQn();

      if ( doMean )
         m_mean   = DVector( s_numberOfChannels );
      if ( doMedian )
         m_median = DVector( s_numberOfChannels );
      if ( doAvgDev )
         m_avgDev = DVector( s_numberOfChannels );
      if ( doMAD )
         m_mad    = DVector( s_numberOfChannels );
      if ( doBWMV )
         m_bwmv   = DVector( s_numberOfChannels );
      if ( doPBMV )
         m_pbmv   = DVector( s_numberOfChannels );
      if ( doSn )
         m_sn     = DVector( s_numberOfChannels );
      if ( doQn )
         m_qn     = DVector( s_numberOfChannels );

      for ( int c = 0; c < s_numberOfChannels; ++c )
      {
         m_image->SelectChannel( c );
         S << *m_image;
         if ( 1 + S.Mean() == 1 )
            throw Error( m_file->FilePath() + ": Zero or insignificant signal detected (empty image?)" );
         if ( doMean )
            m_mean[c] = S.Mean();
         if ( doMedian )
            m_median[c] = S.Median();
         if ( doAvgDev )
            m_avgDev[c] = S.AvgDev();
         if ( doMAD )
            m_mad[c] = S.MAD();
         if ( doBWMV )
            m_bwmv[c] = Sqrt( S.BWMV() );
         if ( doPBMV )
            m_pbmv[c] = Sqrt( S.PBMV() );
         if ( doSn )
            m_sn[c] = S.Sn();
         if ( doQn )
            m_qn[c] = S.Qn();
      }
   }

   if ( doIKSS )
      m_ikss = EvaluateIKSS( m_iksl, *m_image );

   if ( m_image.IsValid() )
   {
      m_image->Status().Complete();
      m_image->SetStatusCallback( nullptr );
   }

   if ( doNoise )
   {
      m_noise = DVector( s_numberOfChannels );
      bool noiseOk = false;
      if ( !instance.p_ignoreNoiseKeywords )
         if ( format.CanStoreKeywords() )
         {
            noiseOk = true;
            for ( int c = 0; c < s_numberOfChannels; ++c )
            {
               m_noise[c] = KeywordValue( IsoString().Format( "NOISE%02d", c ) );
               if ( m_noise[c] <= 0 )
               {
                  m_noise[c] = KeywordValue( "NOISE" );
                  if ( m_noise[c] <= 0 )
                  {
                     noiseOk = false;
                     break;
                  }
               }
            }
         }

      if ( !noiseOk )
         m_noise = EvaluateNoise( ImageVariant( m_image.Ptr() ), instance.p_mrsMinDataFraction );
   }

   if ( s_incremental )
      m_image.Destroy();
   else
      m_image->ResetSelections();

   if ( m_pedestal <= 0 )
   {
      if ( format.CanStoreKeywords() )
         m_pedestal = KeywordValue( "PEDESTAL" );
      if ( m_pedestal < 0 )
         m_pedestal = 0;
   }

   if ( instance.p_useCache )
      AddToCache( path );

   switch ( instance.p_weightScale )
   {
   case IIWeightScale::AvgDev: m_dispersion = m_avgDev; break;
   case IIWeightScale::MAD:    m_dispersion = m_mad; break;
   case IIWeightScale::BWMV:   m_dispersion = m_bwmv; break;
   case IIWeightScale::PBMV:   m_dispersion = m_pbmv; break;
   case IIWeightScale::Sn:     m_dispersion = m_sn; break;
   case IIWeightScale::Qn:     m_dispersion = m_qn; break;
   default:
   case IIWeightScale::IKSS:   m_dispersion = m_ikss; break;
   }

   if ( instance.p_weightScale == IIWeightScale::IKSS )
      m_location = m_iksl;
   else
      m_location = m_median;

   if ( needScale )
   {
      m_scale = DVector( s_numberOfChannels );

      for ( int c = 0; c < s_numberOfChannels; ++c )
      {
         if ( 1 + m_dispersion[c] == 1 )
            throw Error( m_file->FilePath() + ": Zero or insignificant signal detected (empty image?)" );
         m_scale[c] = isReference ? 1.0 : s_files[0]->m_dispersion[c]/m_dispersion[c];
      }

      console.Write( "<end><cbr>Scale factors   : " );
      for ( int c = 0; c < s_numberOfChannels; ++c )
         console.Write( String().Format( " %8.5f", m_scale[c] ) );
      console.Write(       "<br>Zero offset     : " );
      for ( int c = 0; c < s_numberOfChannels; ++c )
         console.Write( String().Format( " %+.6e", ZeroOffset( c ) ) );

      if ( needNoise )
      {
         console.Write(    "<br>Noise estimates : " );
         for ( int c = 0; c < s_numberOfChannels; ++c )
            console.Write( String().Format( " %.4e", m_noise[c] ) );
      }

      console.WriteLn();
   }

   if ( generateOutput )
   {
      m_weight = DVector( s_numberOfChannels );

      /*
       * Image weighting only makes sense for average combination.
       */
      if ( instance.p_generateDrizzleData || instance.p_combination == IICombination::Average )
      {
         switch ( instance.p_weightMode )
         {
         default:
         case IIWeightMode::DontCare:
            m_weight = 1.0;
            break;
         case IIWeightMode::ExposureTimeWeight:
            m_weight[0] = images[0].options.exposure;
            if ( m_weight[0] <= 0 )
               if ( format.CanStoreKeywords() )
               {
                  m_weight[0] = KeywordValue( "EXPTIME" );
                  if ( m_weight[0] <= 0 )
                     m_weight[0] = KeywordValue( "EXPOSURE" );
               }
            if ( m_weight[0] <= 0 )
               throw Error( m_file->FilePath() + ": No way to know the exposure time." );
            m_weight = m_weight[0];
            break;
         case IIWeightMode::NoiseEvaluationWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
            {
               /*
                * Weighting by inverse scaled mean square error.
                */
               double e2 = m_scale[c] * m_noise[c]; // scaled noise estimate
               e2 *= e2;
               if ( !IsFinite( e2 ) || 1 + e2 == 1 )
                  throw Error( m_file->FilePath() + String().Format( " (channel #%d): Unable to compute noise estimate.", c ) );
               m_weight[c] = 1/e2;
            }
            break;
         case IIWeightMode::KeywordWeight:
            if ( !format.CanStoreKeywords() )
               throw Error( m_file->FilePath() + ": The " + format.Name() + " format cannot store keywords." );
            m_weight[0] = KeywordValue( instance.p_weightKeyword );
            if ( m_weight[0] <= 0 )
               throw Error( m_file->FilePath() + ": Nonexistent or invalid weight keyword '" + instance.p_weightKeyword + "'." );
            m_weight = m_weight[0];
            break;
         case IIWeightMode::SignalWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
               m_weight[c] = m_avgDev[c];
            break;
         case IIWeightMode::MedianWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
               m_weight[c] = m_median[c];
            break;
         case IIWeightMode::AverageWeight:
            for ( int c = 0; c < s_numberOfChannels; ++c )
               m_weight[c] = m_mean[c];
            break;
         }

         for ( int c = 0; c < s_numberOfChannels; ++c )
            if ( !IsFinite( m_weight[c] ) || 1 + m_weight[c] == 1 )
               throw Error( m_file->FilePath() + ": Zero or insignificant signal detected (empty image?)." );

         console.Write(           "Weight          : " );
         for ( int c = 0; c < s_numberOfChannels; ++c )
            console.Write( String().Format( " %10.5f", ImageWeight( c ) ) );
         console.WriteLn();
      }
      else
      {
         /*
          * Set all weights equal to one for median, minimum and maximum
          * combinations.
          */
         m_weight = 1.0;
      }
   }

   if ( !s_incremental )
      if ( s_roi != m_image->Bounds() )
         m_image->CropTo( s_roi );

   if ( instance.p_generateDrizzleData )
   {
      m_drzPath = drzPath.Trimmed();
      if ( m_drzPath.IsEmpty() )
         throw Error( m_file->FilePath() + ": Missing drizzle data file path." );
      if ( !File::Exists( m_drzPath ) )
         throw Error( "No such file: " + m_drzPath );
   }
}

// ----------------------------------------------------------------------------

void IntegrationFile::ToDrizzleData( DrizzleData& drz ) const
{
   drz.SetLocation( m_location );
   drz.SetReferenceLocation( s_files[0]->m_location );
   drz.SetScale( m_scale );
   drz.SetWeight( m_weight/s_files[0]->m_weight );
   if ( !m_drzRejectionMap.IsEmpty() )
      drz.SetRejectionMap( m_drzRejectionMap );
}

// ----------------------------------------------------------------------------

double IntegrationFile::KeywordValue( const IsoString& keyName )
{
   FITSKeywordArray keywords;
   if ( !m_file->ReadFITSKeywords( keywords ) )
      throw CatchedException();

   for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
      if ( !i->name.CompareIC( keyName ) )
      {
         if ( i->IsNumeric() )
         {
            double v;
            if ( i->GetNumericValue( v ) )
               return v;
         }

         break;
      }

   return -1;
}

// ----------------------------------------------------------------------------

void IntegrationFile::ResetCacheableData()
{
   m_mean     = DVector();
   m_median   = DVector();
   m_avgDev   = DVector();
   m_mad      = DVector();
   m_bwmv     = DVector();
   m_pbmv     = DVector();
   m_sn       = DVector();
   m_qn       = DVector();
   m_ikss     = DVector();
   m_iksl     = DVector();
   m_noise    = DVector();
   m_pedestal = 0;
}

// ----------------------------------------------------------------------------

void IntegrationFile::AddToCache( const String& path ) const
{
   IntegrationCacheItem item( path );
   item.mean     = m_mean;
   item.median   = m_median;
   item.avgDev   = m_avgDev;
   item.mad      = m_mad;
   item.bwmv     = m_bwmv;
   item.pbmv     = m_pbmv;
   item.sn       = m_sn;
   item.qn       = m_qn;
   item.ikss     = m_ikss;
   item.iksl     = m_iksl;
   item.noise    = m_noise;
   item.pedestal = m_pedestal;
   TheIntegrationCache->Add( item );
}

// ----------------------------------------------------------------------------

bool IntegrationFile::GetFromCache( const String& path )
{
   ResetCacheableData();

   IntegrationCacheItem item;
   if ( !TheIntegrationCache->Get( item, path ) )
      return false;

   m_mean     = item.mean;
   m_median   = item.median;
   m_avgDev   = item.avgDev;
   m_mad      = item.mad;
   m_bwmv     = item.bwmv;
   m_pbmv     = item.pbmv;
   m_sn       = item.sn;
   m_qn       = item.qn;
   m_ikss     = item.ikss;
   m_iksl     = item.iksl;
   m_noise    = item.noise;
   m_pedestal = Max( 0.0F, item.pedestal );

   return
   (m_mean.IsEmpty()   || m_mean.Length()   == s_numberOfChannels) &&
   (m_median.IsEmpty() || m_median.Length() == s_numberOfChannels) &&
   (m_avgDev.IsEmpty() || m_avgDev.Length() == s_numberOfChannels) &&
   (m_mad.IsEmpty()    || m_mad.Length()    == s_numberOfChannels) &&
   (m_bwmv.IsEmpty()   || m_bwmv.Length()   == s_numberOfChannels) &&
   (m_pbmv.IsEmpty()   || m_pbmv.Length()   == s_numberOfChannels) &&
   (m_sn.IsEmpty()     || m_sn.Length()     == s_numberOfChannels) &&
   (m_qn.IsEmpty()     || m_qn.Length()     == s_numberOfChannels) &&
   (m_ikss.IsEmpty()   || m_ikss.Length()   == s_numberOfChannels) &&
   (m_iksl.IsEmpty()   || m_iksl.Length()   == s_numberOfChannels) &&
   (m_noise.IsEmpty()  || m_noise.Length()  == s_numberOfChannels);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ImageIntegrationEngine
{
public:

   ImageIntegrationEngine( const ImageIntegrationInstance& i, StatusMonitor& m ) :
      instance( i ),
      monitor( m ),
      threadData( monitor, 0 )
   {
   }

protected:

   const ImageIntegrationInstance& instance;
         StatusMonitor&            monitor;
         AbstractImage::ThreadData threadData;

   class EngineThread : public Thread
   {
   public:

      EngineThread( const ImageIntegrationEngine& engine, int firstStack, int endStack ) :
         m_data( engine.threadData ),
         m_firstStack( firstStack ),
         m_endStack( endStack )
      {
      }

      virtual ~EngineThread()
      {
      }

      virtual void Run() = 0;

   protected:

      const AbstractImage::ThreadData& m_data;
            int                        m_firstStack, m_endStack;
   };
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DataLoaderEngine : public ImageIntegrationEngine
{
public:

   typedef ImageIntegrationInstance::RejectionDataItem   RejectionDataItem;
   typedef ImageIntegrationInstance::RejectionMatrix     RejectionMatrix;
   typedef ImageIntegrationInstance::RejectionStacks     RejectionStacks;
   typedef ImageIntegrationInstance::RejectionCounts     RejectionCounts;
   typedef ImageIntegrationInstance::RejectionSlopes     RejectionSlopes;

   DataLoaderEngine( const ImageIntegrationInstance& instance_, StatusMonitor& monitor_,
                     int r_, RejectionStacks& R_, RejectionCounts& N_, RejectionSlopes& M_ ) :
      ImageIntegrationEngine( instance_, monitor_ ),
      r( r_ ), R( R_ ), N( N_ ), M( M_ )
   {
      int numberOfThreads = Thread::NumberOfThreads( R.Length(), 1 );
      int stacksPerThread = R.Length()/numberOfThreads;
      for ( int i = 0; i < numberOfThreads; ++i )
      {
         int k0 = i*stacksPerThread;
         int k1 = (i == numberOfThreads-1) ? R.Length() : k0+stacksPerThread;
         threads << new DataLoaderThread( *this, k0, k1 );
      }
   }

   virtual ~DataLoaderEngine()
   {
      threads.Destroy();
   }

   void LoadData()
   {
      if ( !threads.IsEmpty() )
      {
         threadData.total = R.Length();
         threadData.count = 0;
         AbstractImage::RunThreads( threads, threadData );
         monitor = threadData.status;
      }
   }

private:

   class DataLoaderThread : public EngineThread
   {
   public:

      DataLoaderThread( DataLoaderEngine& engine, int firstStack, int endStack ) :
         EngineThread( engine, firstStack, endStack ),
         m_engine( engine )
      {
      }

      virtual void Run();

   private:

      DataLoaderEngine& m_engine;
   };

   typedef ReferenceArray<DataLoaderThread> thread_list;

   int              r; // starting row
   RejectionStacks& R; // set of pixel stacks
   RejectionCounts& N; // set of counts
   RejectionSlopes& M; // set of slopes, for linear fit clipping only
   thread_list      threads;

   friend class DataLoaderThread;
};

// ----------------------------------------------------------------------------

#define E   m_engine
#define I   m_engine.instance

// ----------------------------------------------------------------------------

void DataLoaderEngine::DataLoaderThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );
   FVector* M = (I.p_rejection == IIRejection::LinearFit) ? E.M.ComponentPtr( m_firstStack ) : nullptr;

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      // Source pixels
      *R = RejectionMatrix( IntegrationFile::Width(), IntegrationFile::NumberOfFiles() );
      for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
      {
         const float* b = IntegrationFile::FileByIndex( i )[E.r+k];
         for ( int x = 0; x < IntegrationFile::Width(); ++x )
            R->RowPtr( x )[i].Set( *b++, i );
      }

      // Pixel counters
      *N = IVector( IntegrationFile::NumberOfFiles(), IntegrationFile::Width() );

      // Rejection slopes for the linear fit clipping algorithm
      if ( I.p_rejection == IIRejection::LinearFit )
         *M++ = FVector( .0F, IntegrationFile::Width() );

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class RejectionEngine : public ImageIntegrationEngine
{
public:

   typedef ImageIntegrationInstance::RejectionDataItem   RejectionDataItem;
   typedef ImageIntegrationInstance::RejectionMatrix     RejectionMatrix;
   typedef ImageIntegrationInstance::RejectionStacks     RejectionStacks;
   typedef ImageIntegrationInstance::RejectionCounts     RejectionCounts;
   typedef ImageIntegrationInstance::RejectionSlopes     RejectionSlopes;

   RejectionEngine( const ImageIntegrationInstance& instance_, StatusMonitor& monitor_,
                    RejectionStacks& R_, RejectionCounts& N_, RejectionSlopes& M_,
                    const DVector& m_, const DVector& s_, const DVector& q_ ) :
      ImageIntegrationEngine( instance_, monitor_ ),
      R( R_ ), N( N_ ), M( M_ ), m( m_ ), s( s_ ), q( q_ )
   {
      int numberOfThreads = Thread::NumberOfThreads( R.Length(), 1 );
      int stacksPerThread = R.Length()/numberOfThreads;

      for ( int i = 0; i < numberOfThreads; ++i )
      {
         int k0 = i*stacksPerThread;
         int k1 = (i == numberOfThreads-1) ? R.Length() : k0+stacksPerThread;

         if ( instance.p_rejection != IIRejection::NoRejection )
            normalizeThreads << new NormalizationThread( *this, k0, k1 );

         if ( instance.p_rangeClipLow || instance.p_rangeClipHigh )
            rangeThreads << new RangeRejectionThread( *this, k0, k1 );

         switch ( instance.p_rejection )
         {
         case IIRejection::MinMax:
            rejectThreads << new MinMaxRejectionThread( *this, k0, k1 );
            break;
         case IIRejection::PercentileClip:
            rejectThreads << new PercentileClipRejectionThread( *this, k0, k1 );
            break;
         case IIRejection::SigmaClip:
            rejectThreads << new SigmaClipRejectionThread( *this, k0, k1 );
            break;
         case IIRejection::WinsorizedSigmaClip:
            rejectThreads << new WinsorizedSigmaClipRejectionThread( *this, k0, k1 );
            break;
         case IIRejection::AveragedSigmaClip:
            rejectThreads << new AveragedSigmaClipRejectionThread( *this, k0, k1 );
            break;
         case IIRejection::LinearFit:
            rejectThreads << new LinearFitRejectionThread( *this, k0, k1 );
            break;
         case IIRejection::CCDClip:
            rejectThreads << new CCDClipRejectionThread( *this, k0, k1 );
            break;
         default:
            break;
         }
      }
   }

   virtual ~RejectionEngine()
   {
      normalizeThreads.Destroy();
      rangeThreads.Destroy();
      rejectThreads.Destroy();
      threadPrivate.Destroy();
   }

   void Normalize()
   {
      if ( !normalizeThreads.IsEmpty() )
      {
         threadData.total = R.Length();
         threadData.count = 0;
         AbstractImage::RunThreads( normalizeThreads, threadData );
         monitor = threadData.status;
      }
   }

   void RejectRange()
   {
      if ( !rangeThreads.IsEmpty() )
      {
         threadData.total = R.Length();
         threadData.count = 0;
         AbstractImage::RunThreads( rangeThreads, threadData );
         monitor = threadData.status;
      }
   }

   void Reject()
   {
      if ( !rejectThreads.IsEmpty() )
      {
         for ( RejectionThread& thread : rejectThreads )
            thread.PreRun();

         threadData.total = R.Length();
         threadData.count = 0;
         AbstractImage::RunThreads( rejectThreads, threadData );
         monitor = threadData.status;

         for ( RejectionThread& thread : rejectThreads )
            thread.PostRun();
      }
   }

   static double RejectionMedian( const RejectionDataItem* r, int n )
   {
      // Assume that {r0...rn} is already sorted by value.
      if ( n < 2 )
         return 0;
      int n2 = n >> 1;
      return (n & 1) ? r[n2].value : (r[n2].value + r[n2-1].value)/2;
   }

   static double RejectionSigma( const RejectionDataItem* r, int n )
   {
      if ( n < 2 )
         return 0;
      double mean = 0;
      for ( int i = 0; i < n; ++i )
         mean += r[i].value;
      mean /= n;
      double var = 0, eps = 0;
      for ( int i = 0; i < n; ++i )
      {
         double d = r[i].value - mean;
         var += d*d;
         eps += d;
      }
      return Sqrt( (var - (eps*eps)/n)/(n - 1) );
   }

   static double RejectionADev( const RejectionDataItem* r, int n, double median )
   {
      if ( n < 2 )
         return 0;
      double sd = 0;
      for ( int i = 0; i < n; ++i )
         sd += Abs( r[i].value - median );
      return sd/n;
   }

   static double RejectionMAD( const RejectionDataItem* r, int n, double median )
   {
      if ( n < 2 )
         return 0;
      DVector d( n );
      for ( int i = 0; i < n; ++i )
         d[i] = r[i].value - median;
      return pcl::Median( d.Begin(), d.End() );
   }

   static void RejectionWinsorization( double& mean, double& sigma, const RejectionDataItem* r, int n )
   {
      if ( n < 2 )
      {
         mean = sigma = 0;
         return;
      }

      mean = RejectionMedian( r, n );
      sigma = RejectionSigma( r, n );

      DVector v( n );
      for ( int i = 0; i < n; ++i )
         v[i] = r[i].value;

      for ( int it = 0; ; )
      {
         if ( 1 + sigma == 1 )
            break;

         double t0 = mean - 1.5*sigma;
         double t1 = mean + 1.5*sigma;

         for ( int i = 0; i < n; ++i )
            if ( v[i] < t0 )
               v[i] = t0;
            else if ( v[i] > t1 )
               v[i] = t1;

         double s0 = sigma;
         sigma = 1.134*v.StdDev();
         if ( ++it > 1 && Abs( s0 - sigma )/s0 < 0.0005 )
            break;
      }
   }

private:

   class RejectionThreadPrivate
   {
   public:

      RejectionThreadPrivate()
      {
      }

      virtual ~RejectionThreadPrivate()
      {
      }
   };

   class RejectionThread : public EngineThread
   {
   public:

      RejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         EngineThread( engine, firstStack, endStack ),
         m_engine( engine )
      {
      }

      virtual ~RejectionThread()
      {
      }

      virtual void PreRun()
      {
      }

      virtual void Run() = 0;

      virtual void PostRun()
      {
      }

   protected:

      RejectionEngine& m_engine;
   };

   typedef ReferenceArray<RejectionThread> thread_list;

   class NormalizationThread : public RejectionThread
   {
   public:

      NormalizationThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void Run();
   };

   class RangeRejectionThread : public RejectionThread
   {
   public:

      RangeRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void Run();
   };

   class MinMaxRejectionThread : public RejectionThread
   {
   public:

      MinMaxRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void Run();
   };

   class PercentileClipRejectionThread : public RejectionThread
   {
   public:

      PercentileClipRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void Run();
   };

   class SigmaClipRejectionThread : public RejectionThread
   {
   public:

      SigmaClipRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void Run();
   };

   class WinsorizedSigmaClipRejectionThread : public RejectionThread
   {
   public:

      WinsorizedSigmaClipRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void Run();
   };

   class AveragedSigmaClipRejectionThread : public RejectionThread
   {
   public:

      AveragedSigmaClipRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void PreRun();
      virtual void Run();
      virtual void PostRun();

   private:

      class RejectionData : public RejectionThreadPrivate
      {
      public:

         Array<DVector> m;
         Array<DVector> s;

         RejectionData( RejectionStacks&, RejectionCounts& );
      };
   };

   class LinearFitRejectionThread : public RejectionThread
   {
   public:

      LinearFitRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void Run();
   };

   class CCDClipRejectionThread : public RejectionThread
   {
   public:

      CCDClipRejectionThread( RejectionEngine& engine, int firstStack, int endStack ) :
         RejectionThread( engine, firstStack, endStack )
      {
      }

      virtual void PreRun();
      virtual void Run();
      virtual void PostRun();

   private:

      class RejectionData : public RejectionThreadPrivate
      {
      public:

         double r2g2, gk, sn2;
         bool isScaleNoise;

         RejectionData( float ccdGain, float ccdReadNoise, float ccdScaleNoise, int bits );
      };
   };

         RejectionStacks& R; // set of pixel stacks
         RejectionCounts& N; // set of counts
         RejectionSlopes& M; // set of slopes, for linear fit clipping only
   const DVector&         m; // rejection normalization: median
   const DVector&         s; //                        : scaling
   const DVector&         q; //                        : fluxes

         thread_list      rangeThreads, normalizeThreads, rejectThreads;
         AutoPointer<RejectionThreadPrivate> threadPrivate;

   friend class NormalizationThread;
   friend class RangeRejectionThread;
   friend class MinMaxRejectionThread;
   friend class PercentileClipRejectionThread;
   friend class SigmaClipRejectionThread;
   friend class WinsorizedSigmaClipRejectionThread;
   friend class AveragedSigmaClipRejectionThread;
   friend class LinearFitRejectionThread;
   friend class CCDClipRejectionThread;
};

// ----------------------------------------------------------------------------

#define P   static_cast<RejectionData*>( m_engine.threadPrivate.Ptr() )

// ----------------------------------------------------------------------------

void RejectionEngine::RangeRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 1 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];

         /*
          * ### N.B.: Do not change the order in the stack of pixels here (e.g.
          * do not sort them). Pixels are required in file order for rejection
          * normalization, which takes place AFTER range rejection.
          *
          * Note that the sets of low and high out-of-range pixels are
          * disjoint. For this reason we can accumulate a single rejection
          * counter in the nr variable.
          */
         int nr = 0;
         for ( int j = 0; j < n; ++j )
            if ( I.p_rangeClipLow && r[j].value <= I.p_rangeLow )
               r[j].rejectRangeLow = true, ++nr;
            else if ( I.p_rangeClipHigh && r[j].value >= I.p_rangeHigh )
               r[j].rejectRangeHigh = true, ++nr;
         N->DataPtr()[i] -= nr;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void RejectionEngine::NormalizationThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R )
   {
      switch ( I.p_rejectionNormalization )
      {
      case IIRejectionNormalization::Scale:
         {
            /*
             * Scale + zero offset normalization.
             *
             * ### N.B.: This normalization can yield negative pixel values. If
             * that happens, we must raise all pixels to remove negative
             * values, since all rejection routines expect positive data.
             */
            const DVector& m = E.m;
            const DVector& s = E.s;

            float rmin = 0;
            for ( int x = 0; x < R->Rows(); ++x )
            {
               RejectionDataItem* r = R->RowPtr( x ) + 1;
               for ( int i = 1; i < R->Columns(); ++i, ++r )
                  if ( !r->IsRejected() )
                  {
                     r->value = (r->value - m[i])*s[i] + m[0];
                     if ( r->value < rmin )
                        rmin = r->value;
                  }
            }

            if ( rmin < 0 )
               for ( int x = 0; x < R->Rows(); ++x )
               {
                  RejectionDataItem* r = R->RowPtr( x );
                  for ( int i = 0; i < R->Columns(); ++i, ++r )
                     if ( !r->IsRejected() )
                        r->value -= rmin;
               }
         }
         break;

      case IIRejectionNormalization::EqualizeFluxes:
         {
            /*
             * Flux equalization normalization.
             */
            const DVector& q = E.q;

            for ( int x = 0; x < R->Rows(); ++x )
            {
               RejectionDataItem* r = R->RowPtr( x ) + 1;
               for ( int i = 1; i < R->Columns(); ++i, ++r )
                  if ( !r->IsRejected() )
                     r->value *= q[i];
            }
         }
         break;

      default: // ?!
         break;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void RejectionEngine::MinMaxRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 1 )
            continue;

         int nl = I.p_clipLow ? Min( I.p_minMaxLow, n ) : 0;
         int nh = I.p_clipHigh ? Min( I.p_minMaxHigh, n ) : 0;

         if ( nl > 0 || nh > 0 )
         {
            RejectionDataItem* r = R->DataPtr()[i];
            Sort( r, r + n );

            for ( int j = 0; j < nl; ++j )
               r[j].rejectLow = true;
            for ( int j = n, nj = nh; nj > 0; --nj )
               r[--j].rejectHigh = true;

            Sort( r, r + n );
            N->DataPtr()[i] -= Min( n, nl+nh );
         }
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void RejectionEngine::PercentileClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 2 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         double median = E.RejectionMedian( r, n );
         if ( 1 + median == 1 )
            continue;

         int nc = 0;

         if ( I.p_clipLow )
            for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
            {
               if ( (median - d->value)/median <= I.p_pcClipLow )
                  break;
               d->rejectLow = true, ++nc;
            }

         if ( I.p_clipHigh )
            for ( RejectionDataItem* d = r + n; --d >= r; )
            {
               if ( (d->value - median)/median <= I.p_pcClipHigh )
                  break;
               d->rejectHigh = true, ++nc;
            }

         if ( nc > 0 )
         {
            Sort( r, r + n );
            N->DataPtr()[i] -= nc;
         }
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void RejectionEngine::SigmaClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( ;; )
         {
            double sigma = E.RejectionSigma( r, n );
            if ( 1 + sigma == 1 )
               break;

            double median = E.RejectionMedian( r, n );

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (median - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - median)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

void RejectionEngine::WinsorizedSigmaClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( ;; )
         {
            double mean, sigma;
            RejectionWinsorization( mean, sigma, r, n );
            if ( 1 + sigma == 1 )
               break;

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (mean - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - mean)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

RejectionEngine::AveragedSigmaClipRejectionThread::RejectionData::RejectionData( RejectionStacks& R_, RejectionCounts& N_ ) :
   RejectionEngine::RejectionThreadPrivate(),
   m( R_.Length() ),
   s( R_.Length() )
{
   RejectionMatrix* R = R_.DataPtr();
   IVector* N = N_.DataPtr();

   for ( int k = 0; k < R_.Length(); ++k, ++R, ++N )
   {
      m[k] = DVector( R->Rows() );
      s[k] = DVector( R->Rows() );

      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
         {
            m[k][i] = s[k][i] = .0F;
            continue;
         }

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         double median = m[k][i] = RejectionEngine::RejectionMedian( r, n );
         if ( 1 + median != 1 )
         {
            double acc = 0;
            for ( int j = 0; j < n; ++j )
            {
               double d = r[j].value - median;
               acc += d*d/median;
            }

            s[k][i] = Sqrt( acc/(n - 1) );
         }
         else
         {
            s[k][i] = m[k][i] = 0;
            N->DataPtr()[i] = 0;
         }
      }
   }
}

void RejectionEngine::AveragedSigmaClipRejectionThread::PreRun()
{
   if ( E.threadPrivate.IsNull() )
      E.threadPrivate = new RejectionData( E.R, E.N );
}

void RejectionEngine::AveragedSigmaClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      const DVector& m = P->m[k];
      const DVector& s = P->s[k];

      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 3 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];

         double median = m[i];

         for ( ;; )
         {
            double sigma = s[i] * Sqrt( median );
            if ( 1 + sigma == 1 )
               break;

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (median - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - median)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;

            median = E.RejectionMedian( r, n );
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

void RejectionEngine::AveragedSigmaClipRejectionThread::PostRun()
{
   E.threadPrivate.Destroy();
}

// ----------------------------------------------------------------------------

void RejectionEngine::LinearFitRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );
   FVector* M = E.M.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N, ++M )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 5 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( ;; )
         {
            FVector X( n ), Y( n );
            for ( int j = 0; j < n; ++j )
               X[j] = float( j ), Y[j] = r[j].value;

            LinearFit L( X, Y );
            if ( !L.IsValid() )
            {
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
                  d->rejectLow = d->rejectHigh = true;
               N->DataPtr()[i] = 0;
               M->DataPtr()[i] = 0;
               break;
            }

            double sigma = 2*L.adev; // mult. by 2 to compatibilize with sigma clipping
            if ( 1 + sigma == 1 )
               break;

            int nc = 0;
            for ( int j = 0; j < n; ++j )
            {
               double y = L( j );

               if ( r[j].value < y )
               {
                  if ( I.p_clipLow )
                     if ( (y - r[j].value)/sigma >= I.p_linearFitLow )
                        r[j].rejectLow = true, ++nc;
               }
               else
               {
                  if ( I.p_clipHigh )
                     if ( (r[j].value - y)/sigma >= I.p_linearFitHigh )
                        r[j].rejectHigh = true, ++nc;
               }
            }

            if ( nc == 0 )
            {
               /*
                * N.B.: The fitted slope could be an insignificant negative
                * number because of roundoff - do not propagate it. See bug
                * report: http://pixinsight.com/forum/index.php?topic=8704.0
                */
               M->DataPtr()[i] = (L.b > 0) ? ((L.b < 1e7) ? ArcTan( L.b )/Const<double>::pi4() : 1.0) : 0.0;
               break;
            }

            Sort( r, r + n );

            n -= nc;
            if ( n < 3 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

RejectionEngine::CCDClipRejectionThread::RejectionData::RejectionData( float ccdGain, float ccdReadNoise, float ccdScaleNoise, int bits ) :
   RejectionEngine::RejectionThreadPrivate()
{
   /*
   r2g2 = double( ccdReadNoise )/ccdGain; r2g2 *= r2g2/n;
   gk = n/double( ccdGain )*n;
   sn2 = double( ccdScaleNoise )*ccdScaleNoise*n;
   */
   double n = Pow2I<double>()( bits ) - 1;
   r2g2 = double( ccdReadNoise )/ccdGain; r2g2 *= r2g2/n;
   gk = 1.0/ccdGain/n;
   sn2 = double( ccdScaleNoise )*ccdScaleNoise/n;
   isScaleNoise = sn2 != 0;
}

void RejectionEngine::CCDClipRejectionThread::PreRun()
{
   if ( E.threadPrivate.IsNull() )
      E.threadPrivate = new RejectionData( I.p_ccdGain, I.p_ccdReadNoise, I.p_ccdScaleNoise, 16 );
}

void RejectionEngine::CCDClipRejectionThread::Run()
{
   INIT_THREAD_MONITOR()

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );
   IVector* N = E.N.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R, ++N )
   {
      for ( int i = 0; i < R->Rows(); ++i )
      {
         int n = N->DataPtr()[i];
         if ( n < 2 )
            continue;

         RejectionDataItem* r = R->DataPtr()[i];
         Sort( r, r + n );

         for ( ;; )
         {
            double median = E.RejectionMedian( r, n );
            double sigma = P->r2g2 + P->gk*median;
            if ( P->isScaleNoise )
               sigma += P->sn2*median*median;
            sigma = Sqrt( sigma );
            /*
            double DN = double( median )*65535;
            double sigma = Sqrt( ((ccdReadNoise*ccdReadNoise)/(ccdGain*ccdGain)
                                   + DN/ccdGain
                                   + ccdScaleNoise*ccdScaleNoise*DN*DN)/65535 );
            */

            int nc = 0;

            if ( I.p_clipLow )
               for ( RejectionDataItem* d = r, * d1 = r + n; d < d1; ++d )
               {
                  if ( (median - d->value)/sigma <= I.p_sigmaLow )
                     break;
                  d->rejectLow = true, ++nc;
               }

            if ( I.p_clipHigh )
               for ( RejectionDataItem* d = r + n; --d >= r; )
               {
                  if ( (d->value - median)/sigma <= I.p_sigmaHigh )
                     break;
                  d->rejectHigh = true, ++nc;
               }

            if ( nc == 0 )
               break;

            Sort( r, r + n );

            n -= nc;
            if ( n < 2 )
               break;
         }

         N->DataPtr()[i] = n;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

void RejectionEngine::CCDClipRejectionThread::PostRun()
{
   E.threadPrivate.Destroy();
}

// ----------------------------------------------------------------------------

#undef P

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class IntegrationEngine : public ImageIntegrationEngine
{
public:

   typedef ImageIntegrationInstance::RejectionDataItem   RejectionDataItem;
   typedef ImageIntegrationInstance::RejectionMatrix     RejectionMatrix;
   typedef ImageIntegrationInstance::RejectionStacks     RejectionStacks;
   typedef ImageIntegrationInstance::RejectionCounts     RejectionCounts;
   typedef ImageIntegrationInstance::RejectionSlopes     RejectionSlopes;

   IntegrationEngine( const ImageIntegrationInstance& aInstance, StatusMonitor& aMonitor,
                      int c, RejectionStacks& aR, const RejectionCounts& aN,
                      const DVector& ad, DVector& am, const DVector& as,
                      float* aResult32, double* aResult64 ) :
      ImageIntegrationEngine( aInstance, aMonitor ),
      chn( c ), R( aR ), N( aN ), d( ad ), m( am ), s( as ),
      result32( aResult32 ), result64( aResult64 ),
      threads()
   {
      int numberOfThreads = Thread::NumberOfThreads( R.Length(), 1 );
      int stacksPerThread = R.Length()/numberOfThreads;

      for ( int i = 0; i < numberOfThreads; ++i )
      {
         int k0 = i*stacksPerThread;
         int k1 = (i == numberOfThreads-1) ? R.Length() : k0+stacksPerThread;
         threads << new IntegrationThread( *this, k0, k1 );
      }
   }

   virtual ~IntegrationEngine()
   {
      threads.Destroy();
   }

   void Integrate()
   {
      if ( !threads.IsEmpty() )
      {
         threadData.total = R.Length();
         threadData.count = 0;
         AbstractImage::RunThreads( threads, threadData );
         monitor = threadData.status;
      }
   }

private:

   class IntegrationThread : public EngineThread
   {
   public:

      IntegrationThread( IntegrationEngine& engine, int firstStack, int endStack ) :
         EngineThread( engine, firstStack, endStack ),
         m_engine( engine )
      {
      }

      virtual void Run();

   private:

      IntegrationEngine& m_engine;
   };

   typedef ReferenceArray<IntegrationThread> thread_list;

         int              chn; // current channel
         RejectionStacks& R;   // set of pixel stacks
   const RejectionCounts& N;   // set of counts
   const DVector&         d;   // normalization: zero offset
   const DVector&         m;   //              : median
   const DVector&         s;   //              : scaling
         float*           result32;
         double*          result64;
         thread_list      threads;

   friend class IntegrationThread;
};

// ----------------------------------------------------------------------------

void IntegrationEngine::IntegrationThread::Run()
{
   INIT_THREAD_MONITOR()

   float* result32 = nullptr;
   double* result64 = nullptr;
   if ( I.p_generate64BitResult )
      result64 = E.result64 + size_type( m_firstStack )*size_type( IntegrationFile::Width() );
   else
      result32 = E.result32 + size_type( m_firstStack )*size_type( IntegrationFile::Width() );

   RejectionMatrix* R = E.R.ComponentPtr( m_firstStack );

   for ( int k = m_firstStack; k < m_endStack; ++k, ++R )
   {
      const IVector& N = E.N[k];

      for ( int x = 0; x < R->Rows(); ++x )
      {
         RejectionDataItem* r = R->DataPtr()[x];
         int n = N[x];
         double f;

         pcl_enum thisCombination = I.p_combination;
         if ( n == 0 )
         {
            // If all pixels have been rejected, take the median as
            // the final pixel value for this stack.
            n = IntegrationFile::NumberOfFiles();
            thisCombination = IICombination::Median;
         }

         const DVector& d = E.d;
         const DVector& m = E.m;
         const DVector& s = E.s;

         // Normalize/scale output
         switch ( I.p_normalization )
         {
         case IINormalization::NoNormalization:
            break;
         default:
         case IINormalization::Additive:
            for ( int i = 0; i < n; ++i )
               r[i].raw += d[r[i].index];
            break;
         case IINormalization::Multiplicative:
            for ( int i = 0; i < n; ++i )
               r[i].raw *= d[r[i].index];
            break;
         case IINormalization::AdditiveWithScaling:
            for ( int i = 0; i < n; ++i )
               r[i].raw = (double( r[i].raw ) - m[r[i].index])*s[r[i].index] + m[0];
            break;
         case IINormalization::MultiplicativeWithScaling:
            for ( int i = 0; i < n; ++i )
               r[i].raw = (double( r[i].raw ) / m[r[i].index])*s[r[i].index] * m[0];
            break;
         }

         // Integrate
         switch ( thisCombination )
         {
         default:
         case IICombination::Average:
            {
               f = 0;
               if ( I.p_weightMode == IIWeightMode::DontCare )
               {
                  for ( int i = 0; i < n; ++i, ++r )
                     f += r->raw;
                  f /= n;
               }
               else
               {
                  double ws = 0;
                  for ( int i = 0; i < n; ++i, ++r )
                  {
                     double w = IntegrationFile::FileByIndex( r->index ).Weight( E.chn );
                     f += w*r->raw;
                     ws += w;
                  }
                  f /= ws;
               }
            }
            break;

         case IICombination::Median:
         case IICombination::Minimum:
         case IICombination::Maximum:
            Sort( r, r+n, RejectionDataItem::CompareRaw );
            switch ( thisCombination )
            {
            default:
            case IICombination::Median:
               {
                  int n2 = n >> 1;
                  if ( n & 1 )
                     f = r[n2].raw;
                  else
                     f = 0.5*(r[n2].raw + r[n2-1].raw);
               }
               break;
            case IICombination::Minimum:
               f = r[0].raw;
               break;
            case IICombination::Maximum:
               f = r[n-1].raw;
               break;
            }
            break;
         }

         if ( result32 != nullptr )
            *result32++ = float( f );
         else
            *result64++ = f;
      }

      UPDATE_THREAD_MONITOR( 10 )
   }
}

// ----------------------------------------------------------------------------

#undef E
#undef I

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageWindow ImageIntegrationInstance::CreateImageWindow( const IsoString& id, int bitsPerSample )
{
   if ( p_closePreviousImages )
   {
      ImageWindow window = ImageWindow::WindowById( id );
      if ( !window.IsNull() )
         window.Close();
   }

   ImageWindow window( IntegrationFile::Width(), IntegrationFile::Height(), IntegrationFile::NumberOfChannels(),
                       bitsPerSample, true, IntegrationFile::IsColor(), true, id );
   if ( window.IsNull() )
      throw Error( "Unable to create image window: " + id );

   return window;
}

// ----------------------------------------------------------------------------

inline static String YesNo( bool b )
{
   return String( b ? "yes" : "no" );
}

ImageIntegrationInstance::IntegrationDescriptionItems::IntegrationDescriptionItems( const ImageIntegrationInstance& instance )
{
   if ( instance.p_generateIntegratedImage )
   {
      switch ( instance.p_combination )
      {
      default:
      case IICombination::Average: pixelCombination = "average"; break;
      case IICombination::Median:  pixelCombination = "median"; break;
      case IICombination::Minimum: pixelCombination = "minimum"; break;
      case IICombination::Maximum: pixelCombination = "maximum"; break;
      }

      switch ( instance.p_normalization )
      {
      case IINormalization::NoNormalization:           outputNormalization = "none"; break;
      default:
      case IINormalization::Additive:                  outputNormalization = "additive"; break;
      case IINormalization::Multiplicative:            outputNormalization = "multiplicative"; break;
      case IINormalization::AdditiveWithScaling:       outputNormalization = "additive + scaling"; break;
      case IINormalization::MultiplicativeWithScaling: outputNormalization = "multiplicative + scaling"; break;
      }
   }
   else
   {
      pixelCombination = "disabled";
   }

   if ( instance.p_combination == IICombination::Average )
      switch ( instance.p_weightMode )
      {
      case IIWeightMode::DontCare:              weightMode = "don't care"; break;
      case IIWeightMode::ExposureTimeWeight:    weightMode = "exposure time"; break;
      default:
      case IIWeightMode::NoiseEvaluationWeight: weightMode = "noise evaluation"; break;
      case IIWeightMode::SignalWeight:          weightMode = "average absolute deviation"; break;
      case IIWeightMode::MedianWeight:          weightMode = "median value"; break;
      case IIWeightMode::AverageWeight:         weightMode = "average value"; break;
      case IIWeightMode::KeywordWeight:         weightMode = "custom keyword: " + instance.p_weightKeyword; break;
      }

   switch ( instance.p_weightScale )
   {
   case IIWeightScale::AvgDev: scaleEstimator = "average absolute deviation"; break;
   case IIWeightScale::MAD:    scaleEstimator = "MAD"; break;
   case IIWeightScale::BWMV:   scaleEstimator = "biweight midvariance"; break;
   case IIWeightScale::PBMV:   scaleEstimator = "percentage bend midvariance"; break;
   case IIWeightScale::Sn:     scaleEstimator = "Sn"; break;
   case IIWeightScale::Qn:     scaleEstimator = "Qn"; break;
   default:
   case IIWeightScale::IKSS:   scaleEstimator = "iterative k-sigma / BWMV"; break;
   }

   if ( instance.p_rangeClipLow || instance.p_rangeClipHigh )
   {
      if ( instance.p_rangeClipLow )
         rangeRejection.Format( "range_low=%.6f", instance.p_rangeLow );
      if ( instance.p_rangeClipHigh )
      {
         if ( instance.p_rangeClipLow )
            rangeRejection += ' ';
         rangeRejection.AppendFormat( "range_high=%.6f", instance.p_rangeHigh );
      }
   }

   switch ( instance.p_rejection )
   {
   default:
   case IIRejection::NoRejection:         pixelRejection = "none"; break;
   case IIRejection::MinMax:              pixelRejection = "min/max clipping"; break;
   case IIRejection::PercentileClip:      pixelRejection = "percentile clipping"; break;
   case IIRejection::SigmaClip:           pixelRejection = "sigma clipping"; break;
   case IIRejection::WinsorizedSigmaClip: pixelRejection = "Winsorized sigma clipping"; break;
   case IIRejection::AveragedSigmaClip:   pixelRejection = "averaged sigma clipping"; break;
   case IIRejection::LinearFit:           pixelRejection = "linear fit clipping"; break;
   case IIRejection::CCDClip:             pixelRejection = "CCD noise model"; break;
   }

   if ( instance.p_rejection != IIRejection::NoRejection )
   {
      switch ( instance.p_rejectionNormalization )
      {
      default: // ?!
      case IIRejectionNormalization::NoRejectionNormalization: rejectionNormalization = "none"; break;
      case IIRejectionNormalization::Scale:                    rejectionNormalization = "scale + zero offset"; break;
      case IIRejectionNormalization::EqualizeFluxes:           rejectionNormalization = "equalize fluxes"; break;
      }

      rejectionClippings = "low=" + YesNo( instance.p_clipLow ) + " high=" + YesNo( instance.p_clipHigh );

      switch ( instance.p_rejection )
      {
      case IIRejection::MinMax:
         rejectionParameters.Format( "clip_low=%d clip_high=%d", instance.p_minMaxLow, instance.p_minMaxHigh );
         break;
      case IIRejection::PercentileClip:
         rejectionParameters.Format( "pc_low=%.3f pc_high=%.3f", instance.p_pcClipLow, instance.p_pcClipHigh );
         break;
      case IIRejection::SigmaClip:
      case IIRejection::WinsorizedSigmaClip:
      case IIRejection::AveragedSigmaClip:
         rejectionParameters.Format( "sigma_low=%.3f sigma_high=%.3f", instance.p_sigmaLow, instance.p_sigmaHigh );
         break;
      case IIRejection::LinearFit:
         rejectionParameters.Format( "lfit_low=%.3f lfit_high=%.3f", instance.p_linearFitLow, instance.p_linearFitHigh );
         break;
      case IIRejection::CCDClip:
         rejectionParameters.Format( "gain=%.2f read_noise=%.2f scale_noise=%.2f",
                                     instance.p_ccdGain, instance.p_ccdReadNoise, instance.p_ccdScaleNoise );
         break;
      default: // ?!
         break;
      }
   }

   if ( instance.p_useROI )
      regionOfInterest.Format( "left=%d, top=%d, width=%d, height=%d",
                               instance.p_roi.x0, instance.p_roi.y0, instance.p_roi.Width(), instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------

String ImageIntegrationInstance::IntegrationDescription() const
{
   IntegrationDescriptionItems items( *this );
   String description;
   if ( !items.pixelCombination.IsEmpty() )
      description +=   "Pixel combination ......... " + items.pixelCombination;
   if ( !items.outputNormalization.IsEmpty() )
      description += "\nOutput normalization ...... " + items.outputNormalization;
   if ( !items.weightMode.IsEmpty() )
      description += "\nWeighting mode ............ " + items.weightMode;
   if ( !items.scaleEstimator.IsEmpty() )
      description += "\nScale estimator ........... " + items.scaleEstimator;
   if ( !items.rangeRejection.IsEmpty() )
      description += "\nRange rejection ........... " + items.rangeRejection;
   if ( !items.pixelRejection.IsEmpty() )
      description += "\nPixel rejection ........... " + items.pixelRejection;
   if ( !items.rejectionNormalization.IsEmpty() )
      description += "\nRejection normalization ... " + items.rejectionNormalization;
   if ( !items.rejectionClippings.IsEmpty() )
      description += "\nRejection clippings ....... " + items.rejectionClippings;
   if ( !items.rejectionParameters.IsEmpty() )
      description += "\nRejection parameters ...... " + items.rejectionParameters;
   if ( !items.regionOfInterest.IsEmpty() )
      description += "\nRegion of interest ........ " + items.regionOfInterest;
   return description;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::ExecuteGlobal()
{
   Exception::DisableGUIOutput( p_noGUIMessages );

   o_output = OutputData();

   ImageWindow resultWindow, lowRejectionMapWindow, highRejectionMapWindow, slopeMapWindow;

   try
   {
      if ( p_useROI )
      {
         p_roi.Order();
         if ( !p_roi.IsRect() )
            throw Error( "Empty ROI defined" );
      }

      Console console;
      console.EnableAbort();

      if ( TheIntegrationCache == nullptr )
      {
         new IntegrationCache; // loads cache upon construction
         if ( TheIntegrationCache->IsEnabled() )
            if ( TheIntegrationCache->IsEmpty() )
               console.NoteLn( "<end><cbr><br>* Empty file cache" );
            else
               console.NoteLn( "<end><cbr><br>* Loaded cache: " + String( TheIntegrationCache->NumberOfItems() ) + " item(s)" );
      }

      IntegrationFile::OpenFiles( *this );

      console.WriteLn( String().Format( "<end><cbr><br>Integration of %d images:", IntegrationFile::NumberOfFiles() ) );
      console.WriteLn( IntegrationDescription() );

      bool doReject = p_rejection != IIRejection::NoRejection || p_rangeClipLow || p_rangeClipHigh;

      ImageVariant result;
      if ( p_generateIntegratedImage )
      {
         resultWindow = CreateImageWindow( "integration", p_generate64BitResult ? 64 : 32 );
         result = resultWindow.MainView().Image();
      }

      ImageVariant lowRejectionMap;
      bool generateLowRejectionMap = p_generateRejectionMaps && (p_clipLow || p_rangeClipLow) && doReject;
      if ( generateLowRejectionMap )
      {
         lowRejectionMapWindow = CreateImageWindow( "rejection_low", 32 );
         lowRejectionMap = lowRejectionMapWindow.MainView().Image();
      }

      ImageVariant highRejectionMap;
      bool generateHighRejectionMap = p_generateRejectionMaps && (p_clipHigh || p_rangeClipHigh) && doReject;
      if ( generateHighRejectionMap )
      {
         highRejectionMapWindow = CreateImageWindow( "rejection_high", 32 );
         highRejectionMap = highRejectionMapWindow.MainView().Image();
      }

      ImageVariant slopeMap;
      bool generateSlopeMap = p_generateRejectionMaps && p_rejection == IIRejection::LinearFit;
      if ( generateSlopeMap )
      {
         slopeMapWindow = CreateImageWindow( "slope", 32 );
         slopeMap = slopeMapWindow.MainView().Image();
      }

      int totalStacks;
      {
         size_type stackSize = size_type( IntegrationFile::Width() ) * (
                                    IntegrationFile::NumberOfFiles()*sizeof( RejectionDataItem )
                                  + sizeof( int )
                                  + ((p_rejection == IIRejection::LinearFit) ? sizeof( float ) : 0) );
         size_type stackBufferSize = p_stackSizeMB;
         if ( sizeof( void* ) == 4 )
            stackBufferSize = Min( stackBufferSize, size_type( 4096 ) );
         stackBufferSize *= 1024*1024;
         totalStacks = int( Min( Max( size_type( 1 ), stackBufferSize/stackSize ),
                                 size_type( IntegrationFile::BufferRows() ) ) );
         console.WriteLn( String().Format( "<end><cbr><br>* Using %d concurrent pixel stack(s) = %.2f MiB",
                                           totalStacks, totalStacks*stackSize/1024/1024.0 ) );
      }

      o_output.imageData.Add( OutputData::ImageData(), IntegrationFile::NumberOfFiles() );

      // For each channel
      for ( int c = 0; c < IntegrationFile::NumberOfChannels(); ++c )
      {
         if ( IntegrationFile::NumberOfChannels() > 1 )
            console.WriteLn( String().Format( "<br>* Integrating channel %d of %d:", c+1, IntegrationFile::NumberOfChannels() ) );

         // Keep the GUI responsive
         Module->ProcessEvents();
         if ( console.AbortRequested() )
            throw ProcessAborted();

         /*
          * Output data pointers
          */
         float* resultData32 = nullptr;
         double* resultData64 = nullptr;
         if ( p_generateIntegratedImage )
            if ( p_generate64BitResult )
               resultData64 = static_cast<DImage&>( *result )[c];
            else
               resultData32 = static_cast<Image&>( *result )[c];

         /*
          * Rejection map data pointers
          */
         float* rejectionLowData = nullptr;
         if ( generateLowRejectionMap )
            rejectionLowData = static_cast<Image&>( *lowRejectionMap )[c];
         float* rejectionHighData = nullptr;
         if ( generateHighRejectionMap )
            rejectionHighData = static_cast<Image&>( *highRejectionMap )[c];
         float* slopeData = nullptr;
         if ( generateSlopeMap )
            slopeData = static_cast<Image&>( *slopeMap )[c];

         /*
          * Normalization and scaling vectors
          */
         DVector d, m, s, q;

         if ( p_generateIntegratedImage &&
                  (p_normalization == IINormalization::Additive ||
                   p_normalization == IINormalization::Multiplicative) )
         {
            d = DVector( IntegrationFile::NumberOfFiles() );
            double m0 = IntegrationFile::FileByIndex( 0 ).Location( c );
            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
               switch ( p_normalization )
               {
               default:
               case IINormalization::Additive:
                  d[i] = m0 - IntegrationFile::FileByIndex( i ).Location( c );
                  break;
               case IINormalization::Multiplicative:
                  d[i] = m0 / IntegrationFile::FileByIndex( i ).Location( c );
                  break;
               }
         }

         if ( p_generateDrizzleData ||
              p_generateIntegratedImage &&
                  (p_normalization == IINormalization::AdditiveWithScaling ||
                   p_normalization == IINormalization::MultiplicativeWithScaling) ||
              p_rejection != IIRejection::NoRejection
                  && p_rejectionNormalization == IIRejectionNormalization::Scale )
         {
            m = DVector( IntegrationFile::NumberOfFiles() );
            s = DVector( IntegrationFile::NumberOfFiles() );
            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
            {
               m[i] = IntegrationFile::FileByIndex( i ).Location( c );
               s[i] = IntegrationFile::FileByIndex( i ).Scale( c );
            }
         }

         if ( p_rejection != IIRejection::NoRejection &&
              p_rejectionNormalization == IIRejectionNormalization::EqualizeFluxes )
         {
            q = DVector( IntegrationFile::NumberOfFiles() );
            double m0 = IntegrationFile::FileByIndex( 0 ).Location( c );
            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
               q[i] = m0 / IntegrationFile::FileByIndex( i ).Location( c );
         }

         /*
          * Rejected pixel counters
          */
         GenericVector<size_type> NRL( size_type( 0 ), IntegrationFile::NumberOfFiles() );
         GenericVector<size_type> NRH( size_type( 0 ), IntegrationFile::NumberOfFiles() );
         GenericVector<size_type> NRRL( size_type( 0 ), IntegrationFile::NumberOfFiles() );
         GenericVector<size_type> NRRH( size_type( 0 ), IntegrationFile::NumberOfFiles() );

         /*
          * Integrate pixels
          */
         for ( int y0 = 0; y0 < IntegrationFile::Height(); y0 += IntegrationFile::BufferRows() )
         {
            // Number of rows in this strip
            int numberOfRows = Min( IntegrationFile::BufferRows(), IntegrationFile::Height() - y0 );

            StandardStatus status;
            StatusMonitor monitor;
            monitor.SetCallback( &status );
            monitor.Initialize( String().Format(
               "<end><cbr>Integrating pixel rows: %5d -> %5d", y0, y0+numberOfRows-1 ), 5*numberOfRows );

            IntegrationFile::UpdateBuffers( y0, c );

            /*
             * Integrate pixels in the current strip of pixel rows.
             */
            for ( int r = 0, numberOfStacks; r < numberOfRows; r += numberOfStacks )
            {
               // Number of pixel stacks for rejection/integrationto in this strip
               numberOfStacks = Min( numberOfRows-r, totalStacks );

               // Keep the GUI responsive
               Module->ProcessEvents();
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               RejectionStacks stacks( numberOfStacks );
               RejectionCounts counts( numberOfStacks );
               RejectionSlopes slopes;
               if ( p_rejection == IIRejection::LinearFit )
                  slopes = RejectionSlopes( numberOfStacks );

               DataLoaderEngine( *this, monitor, r, stacks, counts, slopes ).LoadData();

               /*
                * Perform pixel rejection.
                */
               if ( doReject )
               {
                  RejectionEngine rejector( *this, monitor, stacks, counts, slopes, m, s, q );

                  /*
                   * Range rejection must be done in first place to exclude all
                   * out-of-range pixels.
                   *
                   * ### N.B.: The order of pixels in each stack must NOT be
                   * changed because rejection normalization depends on file
                   * order.
                   */
                  if ( p_rangeClipLow || p_rangeClipHigh )
                     rejector.RejectRange();
                  else
                     monitor += numberOfStacks;

                  /*
                   * Rejection normalization.
                   */
                  if ( p_rejection != IIRejection::NoRejection )
                     rejector.Normalize();
                  else
                     monitor += numberOfStacks;

                  /*
                   * In case we have performed range rejection, each stack with
                   * out-of-range pixels must be sorted to send rejected pixels
                   * to the bottom of the stack.
                   */
                  if ( p_rangeClipLow || p_rangeClipHigh )
                     for ( int k = 0; k < numberOfStacks; ++k )
                     {
                        RejectionMatrix& R = stacks[k];
                        IVector& N = counts[k];
                        for ( int x = 0; x < IntegrationFile::Width(); ++x )
                           if ( N[x] < IntegrationFile::NumberOfFiles() )
                              Sort( R[x], R[x] + IntegrationFile::NumberOfFiles() );
                     }

                  /*
                   * Reject pixels.
                   */
                  if ( p_rejection != IIRejection::NoRejection )
                     rejector.Reject();
                  else
                     monitor += numberOfStacks;
               }
               else
                  monitor += 3*numberOfStacks;

               if ( p_generateIntegratedImage )
               {
                  /*
                   * Integrate pixel stacks.
                   */
                  IntegrationEngine( *this, monitor, c,
                                     stacks, counts, d, m, s, resultData32, resultData64 ).Integrate();
                  size_type delta = size_type( numberOfStacks )*size_type( IntegrationFile::Width() );
                  if ( resultData32 != nullptr )
                     resultData32 += delta;
                  else
                     resultData64 += delta;
               }
               else
                  monitor += numberOfStacks;

               /*
                * Update rejection counts and maps.
                */
               if ( doReject )
                  for ( int k = 0, y = y0+r; k < numberOfStacks; ++k, ++y )
                  {
                     const RejectionMatrix& R = stacks[k];

                     for ( int x = 0; x < IntegrationFile::Width(); ++x )
                     {
                        if ( p_clipLow || p_rangeClipLow )
                        {
                           int n = 0, nr = 0;
                           const RejectionDataItem* r = R[x];
                           for ( int i = 0; i < R.Cols(); ++i, ++r )
                           {
                              if ( r->rejectLow )
                              {
                                 ++n;
                                 ++NRL[r->index];
                              }
                              if ( r->rejectRangeLow )
                              {
                                 ++nr;
                                 ++NRRL[r->index];
                              }

                              if ( p_generateDrizzleData )
                                 if ( r->rejectLow || r->rejectRangeLow )
                                    IntegrationFile::FileByIndex( r->index ).AppendDrizzleLowRejectionData( x, y, c );
                           }

                           if ( generateLowRejectionMap )
                           {
                              if ( p_mapRangeRejection )
                                 n += nr;
                              *rejectionLowData++ = float( n )/R.Cols();
                           }
                        }

                        if ( p_clipHigh || p_rangeClipHigh )
                        {
                           int n = 0, nr = 0;
                           const RejectionDataItem* r = R[x];
                           for ( int i = 0; i < R.Cols(); ++i, ++r )
                           {
                              if ( r->rejectHigh )
                              {
                                 ++n;
                                 ++NRH[r->index];
                              }
                              if ( r->rejectRangeHigh )
                              {
                                 ++nr;
                                 ++NRRH[r->index];
                              }

                              if ( p_generateDrizzleData )
                                 if ( r->rejectHigh || r->rejectRangeHigh )
                                    IntegrationFile::FileByIndex( r->index ).AppendDrizzleHighRejectionData( x, y, c );
                           }

                           if ( generateHighRejectionMap )
                           {
                              if ( p_mapRangeRejection )
                                 n += nr;
                              *rejectionHighData++ = float( n )/R.Cols();
                           }
                        }

                        if ( generateSlopeMap )
                           *slopeData++ = slopes[k][x];
                     }
                  }
            } // for each strip

            monitor.Complete();
         } // for each row

         if ( doReject )
         {
            bool showReject = p_rejection != IIRejection::NoRejection || (p_rangeClipLow || p_rangeClipHigh) && p_reportRangeRejection;

            /*
             * Show rejection stats and update output rejection data.
             */
            Module->ProcessEvents();

            if ( showReject )
               console.WriteLn( "<end><cbr><br>Pixel rejection counts:" );

            size_type N = IntegrationFile::NumberOfPixels();
            size_type NL = 0, NH = 0, NLR = 0, NHR = 0;

            for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
            {
               size_type nl = NRL[i];
               size_type nlr = NRRL[i];
               NL += nl;
               NLR += nlr;
               size_type nh = NRH[i];
               size_type nhr = NRRH[i];
               NH += nh;
               NHR += nhr;

               if ( showReject )
               {
                  console.WriteLn( IntegrationFile::FileByIndex( i ).Path() );
                  if ( p_reportRangeRejection )
                     console.WriteLn( String().Format( "%5d : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                       i+1, nl+nh+nlr+nhr, 100.0*(nl+nh+nlr+nhr)/N,
                                                       nl+nlr, nh+nhr, 100.0*(nl+nlr)/N, 100.0*(nh+nhr)/N ) );
                  else
                     console.WriteLn( String().Format( "%5d : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                       i+1, nl+nh, 100.0*(nl+nh)/N,
                                                       nl, nh, 100.0*nl/N, 100.0*nh/N ) );
               }

               if ( c < 3 )
               {
                  o_output.imageData[i].rejectedLow[c] = nl + nlr;
                  o_output.imageData[i].rejectedHigh[c] = nh + nhr;
               }
            }

            if ( showReject )
            {
               N *= IntegrationFile::NumberOfFiles();
               if ( p_reportRangeRejection )
                  console.WriteLn( String().Format( "<end><cbr><br><b>Total</b> : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                   NL+NH+NLR+NHR, 100.0*(NL+NH+NLR+NHR)/N,
                                                   NL+NLR, NH+NHR, 100.0*(NL+NLR)/N, 100.0*(NH+NHR)/N ) );
               else
                  console.WriteLn( String().Format( "<end><cbr><br><b>Total</b> : %9u %7.3f%% (%9u + %9u = %7.3f%% + %7.3f%%)",
                                                   NL+NH, 100.0*(NL+NH)/N,
                                                   NL, NH, 100.0*NL/N, 100.0*NH/N ) );
            }

            if ( c < 3 )
            {
               o_output.totalRejectedLow[c] = NL + NLR;
               o_output.totalRejectedHigh[c] = NH + NHR;
            }

            Module->ProcessEvents();
         }
      } // for each channel

      o_output.numberOfChannels = IntegrationFile::NumberOfChannels();
      o_output.numberOfPixels = IntegrationFile::NumberOfPixels();
      o_output.totalPixels = o_output.numberOfPixels * IntegrationFile::NumberOfFiles();

      if ( p_generateIntegratedImage )
      {
         result.Normalize();

         if ( p_evaluateNoise )
         {
            /*
             * Show noise evaluation info.
             */
            console.WriteLn( "<end><cbr>" );

            DVector noise = EvaluateNoise( result, p_mrsMinDataFraction );
            DVector snr = EvaluateSNR( result, noise );

            SpinStatus spin;
            result->SetStatusCallback( &spin );
            result->Status().Initialize( "<end><cbr>Computing noise scaling factors" );
            result->Status().DisableInitialization();

            DVector iksl;
            DVector ikss = EvaluateIKSS( iksl, result );

            result->Status().Complete();
            result->Status().EnableInitialization();
            result->SetStatusCallback( 0 );
            result->ResetSelections();

            console.Write( "<end><cbr>"
                           "<br>Gaussian noise estimates  :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " %.4e", noise[c] ) );
               o_output.finalNoiseEstimates[c] = noise[c];
            }

            console.Write( "<br>Scale estimates           :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " %.4e", ikss[c] ) );
               o_output.finalScaleEstimates[c] = ikss[c];
            }

            console.Write( "<br>Location estimates        :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " %.4e", iksl[c] ) );
               o_output.finalLocationEstimates[c] = iksl[c];
            }

            console.Write( "<br>SNR estimates             :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               console.Write( String().Format( " %.4e", snr[c] ) );
               //o_output.finalSNREstimates[c] = snr[c];
            }

            console.Write( "<br>Reference noise reduction :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               double s = noise[c];
               s *= IntegrationFile::FileByIndex( 0 ).IKSS( c )/ikss[c];
               double d = IntegrationFile::FileByIndex( 0 ).Noise( c ) / s;
               console.Write( String().Format( " %.4f", d ) );
               o_output.referenceNoiseReductions[c] = d;
            }

            console.Write( "<br>Median noise reduction    :" );
            for ( int c = 0; c < result->NumberOfNominalChannels(); ++c )
            {
               DVector m( IntegrationFile::NumberOfFiles() );
               for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
               {
                  double s = noise[c];
                  s *= IntegrationFile::FileByIndex( i ).IKSS( c )/ikss[c];
                  m[i] = IntegrationFile::FileByIndex( i ).Noise( c ) / s;
               }
               double d = m.Median();
               console.Write( String().Format( " %.4f", d ) );
               o_output.medianNoiseReductions[c] = d;
            }

            console.WriteLn();
            console.WriteLn();

         } // if p_evaluateNoise
      } // if p_generateIntegratedImage

      Module->ProcessEvents();

      if ( p_generateIntegratedImage )
      {
         float pedestal = 0;
         for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
         {
            float d = IntegrationFile::FileByIndex( i ).Pedestal();
            if ( d != 0 )
            {
               if ( pedestal == 0 )
                  pedestal = d;
            }
            if ( pedestal != d )
            {
               pedestal = -1;
               break;
            }
         }

         /*
          * Update HISTORY and PEDESTAL FITS keywords.
          * ### FIXME: This should be optional and disabled by default.
          * ### TODO: Implement generation of XISF properties ASAP.
          */
         FITSKeywordArray keywords;
         resultWindow.GetKeywords( keywords );

         keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Integration with " + PixInsightVersion::AsString() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with ImageIntegration process" );

         IntegrationDescriptionItems items( *this );
         if ( !items.pixelCombination.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.pixelCombination: " + items.pixelCombination );
         if ( !items.outputNormalization.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.outputNormalization: " + items.outputNormalization );
         if ( !items.weightMode.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.weightMode: " + items.weightMode );
         if ( !items.scaleEstimator.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.scaleEstimator: " + items.scaleEstimator );
         if ( !items.rangeRejection.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rangeRejection: " + items.rangeRejection );
         if ( !items.pixelRejection.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.pixelRejection: " + items.pixelRejection );
         if ( !items.rejectionNormalization.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rejectionNormalization: " + items.rejectionNormalization );
         if ( !items.rejectionClippings.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rejectionClippings: " + items.rejectionClippings );
         if ( !items.rejectionParameters.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.rejectionParameters: " + items.rejectionParameters );
         if ( !items.regionOfInterest.IsEmpty() )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), "ImageIntegration.regionOfInterest: " + items.regionOfInterest );

         keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "ImageIntegration.numberOfImages: %d", IntegrationFile::NumberOfFiles() ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "ImageIntegration.totalPixels: %lu", o_output.totalPixels ) );

         IsoString totalRejectedLow = IsoString().Format( "ImageIntegration.totalRejectedLow: %lu(%.3f%%)",
                                 o_output.totalRejectedLow[0], 100.0*o_output.totalRejectedLow[0]/o_output.totalPixels );
         IsoString totalRejectedHigh = IsoString().Format( "ImageIntegration.totalRejectedHigh: %lu(%.3f%%)",
                                 o_output.totalRejectedHigh[0], 100.0*o_output.totalRejectedHigh[0]/o_output.totalPixels );
         if ( o_output.numberOfChannels > 1 )
         {
            totalRejectedLow.AppendFormat( " %lu(%.3f%%) %lu(%.3f%%)",
                                 o_output.totalRejectedLow[1], 100.0*o_output.totalRejectedLow[1]/o_output.totalPixels,
                                 o_output.totalRejectedLow[2], 100.0*o_output.totalRejectedLow[2]/o_output.totalPixels );
            totalRejectedHigh.AppendFormat( " %lu(%.3f%%) %lu(%.3f%%)",
                                 o_output.totalRejectedHigh[1], 100.0*o_output.totalRejectedHigh[1]/o_output.totalPixels,
                                 o_output.totalRejectedHigh[2], 100.0*o_output.totalRejectedHigh[2]/o_output.totalPixels );
         }
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(), totalRejectedLow )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), totalRejectedHigh );

         if ( p_evaluateNoise )
         {
            IsoString finalNoiseEstimates = IsoString().Format( "ImageIntegration.finalNoiseEstimates: %.4e",
                                                               o_output.finalNoiseEstimates[0] );
            IsoString finalScaleEstimates = IsoString().Format( "ImageIntegration.finalScaleEstimates: %.4e",
                                                               o_output.finalScaleEstimates[0] );
            IsoString finalLocationEstimates = IsoString().Format( "ImageIntegration.finalLocationEstimates: %.4e",
                                                               o_output.finalLocationEstimates[0] );
            IsoString referenceNoiseReductions = IsoString().Format( "ImageIntegration.referenceNoiseReductions: %.4f",
                                                               o_output.referenceNoiseReductions[0] );
            IsoString medianNoiseReductions = IsoString().Format( "ImageIntegration.medianNoiseReductions: %.4f",
                                                               o_output.medianNoiseReductions[0] );
            if ( o_output.numberOfChannels > 1 )
            {
               finalNoiseEstimates.AppendFormat( " %.4e %.4e", o_output.finalNoiseEstimates[1], o_output.finalNoiseEstimates[2] );
               finalScaleEstimates.AppendFormat( " %.4e %.4e", o_output.finalScaleEstimates[1], o_output.finalScaleEstimates[2] );
               finalLocationEstimates.AppendFormat( " %.4e %.4e", o_output.finalLocationEstimates[1], o_output.finalLocationEstimates[2] );
               referenceNoiseReductions.AppendFormat( " %.4f %.4f", o_output.referenceNoiseReductions[1], o_output.referenceNoiseReductions[2] );
               medianNoiseReductions.AppendFormat( " %.4f %.4f", o_output.medianNoiseReductions[1], o_output.medianNoiseReductions[2] );
            }
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), finalNoiseEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalScaleEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), finalLocationEstimates )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), referenceNoiseReductions )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), medianNoiseReductions );

            for ( int i = 0; i < o_output.numberOfChannels && i < 3; ++i )
               keywords << FITSHeaderKeyword( IsoString().Format( "NOISE%02d", i ),
                                              IsoString().Format( "%.4e", o_output.finalNoiseEstimates[i] ),
                                              IsoString().Format( "Gaussian noise estimate for channel #%d", i ) );
         }

         if ( pedestal != 0 )
         {
            if ( pedestal > 0 )
            {
               keywords << FITSHeaderKeyword( "HISTORY",
                                              IsoString(),
                                              IsoString().Format( "ImageIntegration.outputPedestal: %.4g DN", pedestal ) )
                        << FITSHeaderKeyword( "PEDESTAL",
                                              IsoString().Format( "%.4g", pedestal ),
                                              "Value in DN added to enforce positivity" );
               console.NoteLn( String().Format( "* PEDESTAL keyword created with value: %.4g DN", pedestal ) );
            }
            else
               console.WarningLn( "** Warning: Inconsistent pedestal values detected - PEDESTAL keyword not created." );
         }

         /*
          * Per-image HISTORY keywords.
          */
         for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
         {
            const IntegrationFile& file = IntegrationFile::FileByIndex( i );

            if ( file.HasDispersion() )
            {
               IsoString dispersion = IsoString().Format( "ImageIntegration.scaleEstimates_%d: %.4e", i, file.Dispersion( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  dispersion.AppendFormat( " %.4e %.4e", file.Dispersion( 1 ), file.Dispersion( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), dispersion );
            }

            if ( file.HasLocation() )
            {
               IsoString location = IsoString().Format( "ImageIntegration.locationEstimates_%d: %.4e", i, file.Location( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  location.AppendFormat( " %.4e %.4e", file.Location( 1 ), file.Location( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), location );
            }

            if ( file.HasNoise() )
            {
               IsoString noise = IsoString().Format( "ImageIntegration.noiseEstimates_%d: %.4e", i, file.Noise( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  noise.AppendFormat( " %.4e %.4e", file.Noise( 1 ), file.Noise( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), noise );
            }

            if ( file.HasImageWeight() )
            {
               IsoString weight = IsoString().Format( "ImageIntegration.imageWeights_%d: %.5e", i, file.ImageWeight( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  weight.AppendFormat( " %.5e %.5e", file.ImageWeight( 1 ), file.ImageWeight( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), weight );
            }

            if ( file.HasScale() )
            {
               IsoString scale = IsoString().Format( "ImageIntegration.scaleFactors_%d: %.5e", i, file.Scale( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  scale.AppendFormat( " %.5e %.5e", file.Scale( 1 ), file.Scale( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), scale );
            }

            if ( file.HasZeroOffset() )
            {
               IsoString zeroOffset = IsoString().Format( "ImageIntegration.zeroOffsets_%d: %+.6e", i, file.ZeroOffset( 0 ) );
               if ( IntegrationFile::NumberOfChannels() > 1 )
                  zeroOffset.AppendFormat( " %+.6e %+.6e", file.ZeroOffset( 1 ), file.ZeroOffset( 2 ) );
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(), zeroOffset );
            }

            IsoString rejectedLow = IsoString().Format( "ImageIntegration.rejectedLow_%d: %lu", i, o_output.imageData[i].rejectedLow[0] );
            if ( IntegrationFile::NumberOfChannels() > 1 )
               rejectedLow.AppendFormat( " %lu %lu", o_output.imageData[i].rejectedLow[1], o_output.imageData[i].rejectedLow[2] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), rejectedLow );

            IsoString rejectedHigh = IsoString().Format( "ImageIntegration.rejectedHigh_%d: %lu", i, o_output.imageData[i].rejectedHigh[0] );
            if ( IntegrationFile::NumberOfChannels() > 1 )
               rejectedHigh.AppendFormat( " %lu %lu", o_output.imageData[i].rejectedHigh[1], o_output.imageData[i].rejectedHigh[2] );
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(), rejectedHigh );
         }

         resultWindow.SetKeywords( keywords );
         resultWindow.Show();

         /*
          * Update instance output properties.
          */
         o_output.integrationImageId = resultWindow.MainView().Id();
         for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
            for ( int c = 0; c < IntegrationFile::NumberOfChannels() && c < 3; ++c )
               o_output.imageData[i].weights[c] = IntegrationFile::FileByIndex( i ).Weight( c );
      }

      if ( p_generateDrizzleData )
      {
         /*
          * Generate Drizzle integration data.
          */
         console.WriteLn( "* Updating drizzle data files:" );

         int succeeded = 0;
         for ( int i = 0; i < IntegrationFile::NumberOfFiles(); ++i )
         {
            const IntegrationFile& F = IntegrationFile::FileByIndex( i );
            if ( F.DrizzleDataPath().IsEmpty() )
            {
               console.WarningLn( "** Warning: Drizzle data file not defined: " + F.Path() );
               continue;
            }
            if ( !File::Exists( F.DrizzleDataPath() ) )
            {
               console.WarningLn( "** Warning: Drizzle data file not found: " + F.DrizzleDataPath() );
               continue;
            }

            Exception::DisableGUIOutput( p_noGUIMessages );

            Module->ProcessEvents();

            try
            {
               DrizzleData drz( F.DrizzleDataPath(), true/*ignoreIntegrationData*/ );
               F.ToDrizzleData( drz );
               String newDrzDataPath = File::ChangeExtension( F.DrizzleDataPath(), ".xdrz" ); // don't overwrite old .drz files
               drz.SerializeToFile( newDrzDataPath );
               console.WriteLn( newDrzDataPath );
               ++succeeded;
            }
            catch ( ProcessAborted& )
            {
               throw;
            }
            catch ( ... )
            {
               try
               {
                  throw;
               }
               ERROR_HANDLER
            }

            Module->ProcessEvents();
         }

         if ( succeeded < IntegrationFile::NumberOfFiles() )
            console.WarningLn( String().Format( "<end><cbr>** Warning: Drizzle data file generation: %d succeeded, %d errors.",
                                                succeeded, IntegrationFile::NumberOfFiles()-succeeded ) );
         else
            console.NoteLn( String().Format( "<end><cbr>* %d drizzle data files have been updated successfully.", succeeded ) );
      }

      IntegrationFile::CloseAll();

      if ( generateLowRejectionMap )
      {
         lowRejectionMapWindow.Show();
         o_output.lowRejectionMapImageId = lowRejectionMapWindow.MainView().Id();
      }

      if ( generateHighRejectionMap )
      {
         highRejectionMapWindow.Show();
         o_output.highRejectionMapImageId = highRejectionMapWindow.MainView().Id();
      }

      if ( generateSlopeMap )
      {
         slopeMapWindow.Show();
         o_output.slopeMapImageId = slopeMapWindow.MainView().Id();
      }

      return true;
   }
   catch ( ... )
   {
      IntegrationFile::CloseAll();

      if ( !resultWindow.IsNull() )
         resultWindow.Close();

      if ( !lowRejectionMapWindow.IsNull() )
         lowRejectionMapWindow.Close();

      if ( !highRejectionMapWindow.IsNull() )
         highRejectionMapWindow.Close();

      if ( !slopeMapWindow.IsNull() )
         slopeMapWindow.Close();

      throw;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void* ImageIntegrationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIIImageEnabledParameter )
      return &p_images[tableRow].enabled;
   if ( p == TheIIImagePathParameter )
      return p_images[tableRow].path.Begin();
   if ( p == TheIIDrizzlePathParameter )
      return p_images[tableRow].drzPath.Begin();
   if ( p == TheIIInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheIICombinationParameter )
      return &p_combination;
   if ( p == TheIINormalizationParameter )
      return &p_normalization;
   if ( p == TheIIWeightModeParameter )
      return &p_weightMode;
   if ( p == TheIIWeightKeywordParameter )
      return p_weightKeyword.Begin();
   if ( p == TheIIWeightScaleParameter )
      return &p_weightScale;
   if ( p == TheIIIgnoreNoiseKeywordsParameter )
      return &p_ignoreNoiseKeywords;
   if ( p == TheIIRejectionParameter )
      return &p_rejection;
   if ( p == TheIIRejectionNormalizationParameter )
      return &p_rejectionNormalization;
   if ( p == TheIIMinMaxLowParameter )
      return &p_minMaxLow;
   if ( p == TheIIMinMaxHighParameter )
      return &p_minMaxHigh;
   if ( p == TheIIPercentileLowParameter )
      return &p_pcClipLow;
   if ( p == TheIIPercentileHighParameter )
      return &p_pcClipHigh;
   if ( p == TheIISigmaLowParameter )
      return &p_sigmaLow;
   if ( p == TheIISigmaHighParameter )
      return &p_sigmaHigh;
   if ( p == TheIILinearFitLowParameter )
      return &p_linearFitLow;
   if ( p == TheIILinearFitHighParameter )
      return &p_linearFitHigh;
   if ( p == TheIICCDGainParameter )
      return &p_ccdGain;
   if ( p == TheIICCDReadNoiseParameter )
      return &p_ccdReadNoise;
   if ( p == TheIICCDScaleNoiseParameter )
      return &p_ccdScaleNoise;
   if ( p == TheIIClipLowParameter )
      return &p_clipLow;
   if ( p == TheIIClipHighParameter )
      return &p_clipHigh;
   if ( p == TheIIRangeClipLowParameter )
      return &p_rangeClipLow;
   if ( p == TheIIRangeLowParameter )
      return &p_rangeLow;
   if ( p == TheIIRangeClipHighParameter )
      return &p_rangeClipHigh;
   if ( p == TheIIRangeHighParameter )
      return &p_rangeHigh;
   if ( p == TheIIReportRangeRejectionParameter )
      return &p_reportRangeRejection;
   if ( p == TheIIMapRangeRejectionParameter )
      return &p_mapRangeRejection;
   if ( p == TheIIGenerate64BitResultParameter )
      return &p_generate64BitResult;
   if ( p == TheIIGenerateRejectionMapsParameter )
      return &p_generateRejectionMaps;
   if ( p == TheIIGenerateIntegratedImageParameter )
      return &p_generateIntegratedImage;
   if ( p == TheIIGenerateDrizzleDataParameter )
      return &p_generateDrizzleData;
   if ( p == TheIIClosePreviousImagesParameter )
      return &p_closePreviousImages;
   if ( p == TheIIBufferSizeParameter )
      return &p_bufferSizeMB;
   if ( p == TheIIStackSizeParameter )
      return &p_stackSizeMB;
   if ( p == TheIIUseROIParameter )
      return &p_useROI;
   if ( p == TheIIROIX0Parameter )
      return &p_roi.x0;
   if ( p == TheIIROIY0Parameter )
      return &p_roi.y0;
   if ( p == TheIIROIX1Parameter )
      return &p_roi.x1;
   if ( p == TheIIROIY1Parameter )
      return &p_roi.y1;
   if ( p == TheIIUseCacheParameter )
      return &p_useCache;
   if ( p == TheIIEvaluateNoiseParameter )
      return &p_evaluateNoise;
   if ( p == TheIIMRSMinDataFractionParameter )
      return &p_mrsMinDataFraction;
   if ( p == TheIINoGUIMessagesParameter )
      return &p_noGUIMessages;
   if ( p == TheIIUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheIIFileThreadOverloadParameter )
      return &p_fileThreadOverload;

   if ( p == TheIIIntegrationImageIdParameter )
      return o_output.integrationImageId.Begin();
   if ( p == TheIILowRejectionMapImageIdParameter )
      return o_output.lowRejectionMapImageId.Begin();
   if ( p == TheIIHighRejectionMapImageIdParameter )
      return o_output.highRejectionMapImageId.Begin();
   if ( p == TheIISlopeMapImageIdParameter )
      return o_output.slopeMapImageId.Begin();
   if ( p == TheIINumberOfChannelsParameter )
      return &o_output.numberOfChannels;
   if ( p == TheIINumberOfPixelsParameter )
      return &o_output.numberOfPixels;
   if ( p == TheIITotalPixelsParameter )
      return &o_output.totalPixels;

   if ( p == TheIITotalRejectedLowRKParameter )
      return o_output.totalRejectedLow.At( 0 );
   if ( p == TheIITotalRejectedLowGParameter )
      return o_output.totalRejectedLow.At( 1 );
   if ( p == TheIITotalRejectedLowBParameter )
      return o_output.totalRejectedLow.At( 2 );

   if ( p == TheIITotalRejectedHighRKParameter )
      return o_output.totalRejectedHigh.At( 0 );
   if ( p == TheIITotalRejectedHighGParameter )
      return o_output.totalRejectedHigh.At( 1 );
   if ( p == TheIITotalRejectedHighBParameter )
      return o_output.totalRejectedHigh.At( 2 );

   if ( p == TheIIFinalNoiseEstimateRKParameter )
      return o_output.finalNoiseEstimates.At( 0 );
   if ( p == TheIIFinalNoiseEstimateGParameter )
      return o_output.finalNoiseEstimates.At( 1 );
   if ( p == TheIIFinalNoiseEstimateBParameter )
      return o_output.finalNoiseEstimates.At( 2 );

   if ( p == TheIIFinalScaleEstimateRKParameter )
      return o_output.finalScaleEstimates.At( 0 );
   if ( p == TheIIFinalScaleEstimateGParameter )
      return o_output.finalScaleEstimates.At( 1 );
   if ( p == TheIIFinalScaleEstimateBParameter )
      return o_output.finalScaleEstimates.At( 2 );

   if ( p == TheIIFinalLocationEstimateRKParameter )
      return o_output.finalLocationEstimates.At( 0 );
   if ( p == TheIIFinalLocationEstimateGParameter )
      return o_output.finalLocationEstimates.At( 1 );
   if ( p == TheIIFinalLocationEstimateBParameter )
      return o_output.finalLocationEstimates.At( 2 );

   if ( p == TheIIReferenceNoiseReductionRKParameter )
      return o_output.referenceNoiseReductions.At( 0 );
   if ( p == TheIIReferenceNoiseReductionGParameter )
      return o_output.referenceNoiseReductions.At( 1 );
   if ( p == TheIIReferenceNoiseReductionBParameter )
      return o_output.referenceNoiseReductions.At( 2 );

   if ( p == TheIIMedianNoiseReductionRKParameter )
      return o_output.medianNoiseReductions.At( 0 );
   if ( p == TheIIMedianNoiseReductionGParameter )
      return o_output.medianNoiseReductions.At( 1 );
   if ( p == TheIIMedianNoiseReductionBParameter )
      return o_output.medianNoiseReductions.At( 2 );

   if ( p == TheIIReferenceSNRIncrementRKParameter )
      return o_output.referenceSNRIncrements.At( 0 );
   if ( p == TheIIReferenceSNRIncrementGParameter )
      return o_output.referenceSNRIncrements.At( 1 );
   if ( p == TheIIReferenceSNRIncrementBParameter )
      return o_output.referenceSNRIncrements.At( 2 );

   if ( p == TheIIAverageSNRIncrementRKParameter )
      return o_output.averageSNRIncrements.At( 0 );
   if ( p == TheIIAverageSNRIncrementGParameter )
      return o_output.averageSNRIncrements.At( 1 );
   if ( p == TheIIAverageSNRIncrementBParameter )
      return o_output.averageSNRIncrements.At( 2 );

   if ( p == TheIIImageWeightRKParameter )
      return o_output.imageData[tableRow].weights.At( 0 );
   if ( p == TheIIImageWeightGParameter )
      return o_output.imageData[tableRow].weights.At( 1 );
   if ( p == TheIIImageWeightBParameter )
      return o_output.imageData[tableRow].weights.At( 2 );

   if ( p == TheIIImageRejectedLowRKParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 0 );
   if ( p == TheIIImageRejectedLowGParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 1 );
   if ( p == TheIIImageRejectedLowBParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 2 );

   if ( p == TheIIImageRejectedHighRKParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 0 );
   if ( p == TheIIImageRejectedHighGParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 1 );
   if ( p == TheIIImageRejectedHighBParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 2 );

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ImageIntegrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIIImagesParameter )
   {
      p_images.Clear();
      if ( sizeOrLength > 0 )
         p_images.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheIIImagePathParameter )
   {
      p_images[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_images[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheIIDrizzlePathParameter )
   {
      p_images[tableRow].drzPath.Clear();
      if ( sizeOrLength > 0 )
         p_images[tableRow].drzPath.SetLength( sizeOrLength );
   }
   else if ( p == TheIIInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheIIWeightKeywordParameter )
   {
      p_weightKeyword.Clear();
      if ( sizeOrLength > 0 )
         p_weightKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheIIIntegrationImageIdParameter )
   {
      o_output.integrationImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.integrationImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIILowRejectionMapImageIdParameter )
   {
      o_output.lowRejectionMapImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.lowRejectionMapImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIIHighRejectionMapImageIdParameter )
   {
      o_output.highRejectionMapImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.highRejectionMapImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIISlopeMapImageIdParameter )
   {
      o_output.slopeMapImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.slopeMapImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheIIImageDataParameter )
   {
      o_output.imageData.Clear();
      if ( sizeOrLength > 0 )
         o_output.imageData.Append( OutputData::ImageData(), sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type ImageIntegrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheIIImagesParameter )
      return p_images.Length();
   if ( p == TheIIImagePathParameter )
      return p_images[tableRow].path.Length();
   if ( p == TheIIDrizzlePathParameter )
      return p_images[tableRow].drzPath.Length();
   if ( p == TheIIInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheIIWeightKeywordParameter )
      return p_weightKeyword.Length();
   if ( p == TheIIIntegrationImageIdParameter )
      return o_output.integrationImageId.Length();
   if ( p == TheIILowRejectionMapImageIdParameter )
      return o_output.lowRejectionMapImageId.Length();
   if ( p == TheIIHighRejectionMapImageIdParameter )
      return o_output.highRejectionMapImageId.Length();
   if ( p == TheIISlopeMapImageIdParameter )
      return o_output.slopeMapImageId.Length();
   if ( p == TheIIImageDataParameter )
      return o_output.imageData.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIntegrationInstance.cpp - Released 2017-05-02T09:43:00Z
