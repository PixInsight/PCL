//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0012
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ICCProfile.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/ProcessInstance.h>
#include <pcl/Version.h>

#include "PSF.h"
#include "SubframeSelectorCache.h"
#include "SubframeSelectorInstance.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorMeasurementsInterface.h"
#include "SubframeSelectorStarDetector.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// From Debayer
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

SubframeSelectorInstance::SubframeSelectorInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_routine( SSRoutine::Default ),
   p_fileCache( TheSSFileCacheParameter->DefaultValue() ),
   p_subframeScale( TheSSSubframeScaleParameter->DefaultValue() ),
   p_cameraGain( TheSSCameraGainParameter->DefaultValue() ),
   p_cameraResolution( SSCameraResolution::Default ),
   p_siteLocalMidnight( TheSSSiteLocalMidnightParameter->DefaultValue() ),
   p_scaleUnit( SSScaleUnit::Default ),
   p_dataUnit( SSDataUnit::Default ),
   p_structureLayers( TheSSStructureLayersParameter->DefaultValue() ),
   p_noiseLayers( TheSSNoiseLayersParameter->DefaultValue() ),
   p_hotPixelFilterRadius( TheSSHotPixelFilterRadiusParameter->DefaultValue() ),
   p_hotPixelFilter( TheSSApplyHotPixelFilterParameter->DefaultValue() ),
   p_noiseReductionFilterRadius( TheSSNoiseReductionFilterRadiusParameter->DefaultValue() ),
   p_sensitivity( TheSSSensitivityParameter->DefaultValue() ),
   p_peakResponse( TheSSPeakResponseParameter->DefaultValue() ),
   p_maxDistortion( TheSSMaxDistortionParameter->DefaultValue() ),
   p_upperLimit( TheSSUpperLimitParameter->DefaultValue() ),
   p_backgroundExpansion( TheSSBackgroundExpansionParameter->DefaultValue() ),
   p_xyStretch( TheSSXYStretchParameter->DefaultValue() ),
   p_psfFit( SSPSFFit::Default ),
   p_psfFitCircular( TheSSPSFFitCircularParameter->DefaultValue() ),
   p_pedestal( TheSSPedestalParameter->DefaultValue() ),
   p_roi( 0 ),
   p_inputHints( TheSSInputHintsParameter->DefaultValue() ),
   p_outputHints( TheSSOutputHintsParameter->DefaultValue() ),
   p_outputDirectory( TheSSOutputDirectoryParameter->DefaultValue() ),
   p_outputExtension( TheSSOutputExtensionParameter->DefaultValue() ),
   p_outputPrefix( TheSSOutputPrefixParameter->DefaultValue() ),
   p_outputPostfix( TheSSOutputPostfixParameter->DefaultValue() ),
   p_outputKeyword( TheSSOutputKeywordParameter->DefaultValue() ),
   p_overwriteExistingFiles( TheSSOverwriteExistingFilesParameter->DefaultValue() ),
   p_onError( SSOnError::Default ),
   p_sortingProperty( SSSortingProperty::Default ),
   p_graphProperty( SSGraphProperty::Default )
{
}

// ----------------------------------------------------------------------------

SubframeSelectorInstance::SubframeSelectorInstance( const SubframeSelectorInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::Assign( const ProcessImplementation& p )
{
   const SubframeSelectorInstance* x = dynamic_cast<const SubframeSelectorInstance*>( &p );
   if ( x != nullptr )
   {
      p_routine                    = x->p_routine;
      p_fileCache                  = x->p_fileCache;
      p_subframes                  = x->p_subframes;
      p_subframeScale              = x->p_subframeScale;
      p_cameraGain                 = x->p_cameraGain;
      p_cameraResolution           = x->p_cameraResolution;
      p_siteLocalMidnight          = x->p_siteLocalMidnight;
      p_scaleUnit                  = x->p_scaleUnit;
      p_dataUnit                   = x->p_dataUnit;
      p_structureLayers            = x->p_structureLayers;
      p_noiseLayers                = x->p_noiseLayers;
      p_hotPixelFilterRadius       = x->p_hotPixelFilterRadius;
      p_hotPixelFilter             = x->p_hotPixelFilter;
      p_noiseReductionFilterRadius = x->p_noiseReductionFilterRadius;
      p_sensitivity                = x->p_sensitivity;
      p_peakResponse               = x->p_peakResponse;
      p_maxDistortion              = x->p_maxDistortion;
      p_upperLimit                 = x->p_upperLimit;
      p_backgroundExpansion        = x->p_backgroundExpansion;
      p_xyStretch                  = x->p_xyStretch;
      p_psfFit                     = x->p_psfFit;
      p_psfFitCircular             = x->p_psfFitCircular;
      p_pedestal                   = x->p_pedestal;
      p_roi                        = x->p_roi;
      p_inputHints                 = x->p_inputHints;
      p_outputHints                = x->p_outputHints;
      p_outputDirectory            = x->p_outputDirectory;
      p_outputExtension            = x->p_outputExtension;
      p_outputPrefix               = x->p_outputPrefix;
      p_outputPostfix              = x->p_outputPostfix;
      p_outputKeyword              = x->p_outputKeyword;
      p_overwriteExistingFiles     = x->p_overwriteExistingFiles;
      p_onError                    = x->p_onError;
      p_approvalExpression         = x->p_approvalExpression;
      p_weightingExpression        = x->p_weightingExpression;
      p_sortingProperty            = x->p_sortingProperty;
      p_graphProperty              = x->p_graphProperty;
      p_measures                   = x->p_measures;
   }
}

// ----------------------------------------------------------------------------

struct MeasureThreadInputData
{
   // The static settings
   pcl_bool                  showStarDetectionMaps = false;
   SubframeSelectorInstance* instance = nullptr;
};

// ----------------------------------------------------------------------------

typedef Array<Star> star_list;
typedef Array<PSFData> psf_list;

class SubframeSelectorMeasureThread : public Thread
{
public:

   SubframeSelectorMeasureThread( int index,
                                  ImageVariant* subframe,
                                  const String& subframePath,
                                  MeasureThreadInputData* data ) :
      m_index( index ),
      m_subframe( subframe ),
      m_outputData( subframePath ),
      m_data( data )
   {
   }

   void Run() override
   {
      try
      {
         m_success = false;
         ElapsedTime T;

         if ( IsAborted() )
            throw Error( "Aborted" );

         Console console;
         console.NoteLn( "<end><cbr><br>Measuring: " + m_outputData.path );
         MeasureImage();
         console.WriteLn( "Image Calculations: " + T.ToString() );
         T.Reset();

         if ( IsAborted() )
            throw Error( "Aborted" );

         // Crop if the ROI was set
         if ( m_data->instance->p_roi.IsRect() )
            m_subframe->CropTo( m_data->instance->p_roi );

         // Run the Star Detector
         star_list stars = StarDetector();

         // Stop if just showing the maps
         if ( m_data->showStarDetectionMaps )
         {
            m_success = true;
            return;
         }

         // Stop if no stars found
         if ( stars.IsEmpty() )
         {
            m_success = false;
            return;
         }

         if ( IsAborted() )
            throw Error( "Aborted" );

         // Run the PSF Fitter
         psf_list fits = FitPSFs( stars.Begin(), stars.End() );

         if ( fits.IsEmpty() )
         {
            m_success = false;
            return;
         }

         if ( IsAborted() )
            throw Error( "Aborted" );

         // Measure Data
         MeasurePSFs( fits );
         console.WriteLn( "     Star Detector: " + T.ToIsoString() );

         console.WriteLn( String().Format( "%i Star(s) detected", stars.Length() ) );
         console.WriteLn( String().Format( "%i PSF(s) fitted", fits.Length() ) );

         m_success = true;
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }
   }

   int Index() const
   {
      return m_index;
   }

   const MeasureData& OutputData() const
   {
      return m_outputData;
   }

   bool Success() const
   {
      return m_success;
   }

private:

   void EvaluateNoise()
   {
      double noiseEstimate = 0;
      double noiseFraction = 0;
      double noiseEstimateKS = 0;
      double noiseFractionKS = 0;
      SeparableFilter H( __5x5B3Spline_hv, __5x5B3Spline_hv, 5 );
      for ( int n = 4; ; )
      {
         ATrousWaveletTransform W( H, n );
         W << *m_subframe;

         size_type N;
         if ( n == 4 )
         {
            noiseEstimateKS = W.NoiseKSigma( 0, 3, 0.01, 10, &N )/__5x5B3Spline_kj[0];
            noiseFractionKS = double( N )/m_subframe->NumberOfPixels();
         }
         noiseEstimate = W.NoiseMRS( ImageVariant( *m_subframe ), __5x5B3Spline_kj, noiseEstimateKS, 3, &N );
         noiseFraction = double( N )/m_subframe->NumberOfPixels();

         if ( noiseEstimate > 0 && noiseFraction >= 0.01 )
            break;

         if ( --n == 1 )
         {
            noiseEstimate = noiseEstimateKS;
            noiseFraction = noiseFractionKS;
            break;
         }
      }

      m_outputData.noise = noiseEstimate;
      m_outputData.noiseRatio = noiseFraction;
   }

   void MeasureImage()
   {
      // Most parameters here are defaults, but set it to use 1 CPU at the end
      m_outputData.median = m_subframe->Median( 0, -1, -1, 1 );
      m_outputData.medianMeanDev = m_subframe->AvgDev( m_outputData.median, 0, -1, -1, 1 );
      EvaluateNoise();
      m_outputData.snrWeight = m_outputData.noise != 0 ?
                               pcl::Pow( m_outputData.medianMeanDev, 2.0 ) / pcl::Pow( m_outputData.noise, 2.0 ) : 0;
   }

   star_list StarDetector()
   {
      // Setup StarDetector parameters and find the list of stars
      SubframeSelectorStarDetector starDetector;
      starDetector.showStarDetectionMaps      = m_data->showStarDetectionMaps;
      starDetector.structureLayers            = m_data->instance->p_structureLayers;
      starDetector.noiseLayers                = m_data->instance->p_noiseLayers;
      starDetector.hotPixelFilterRadius       = m_data->instance->p_hotPixelFilterRadius;
      starDetector.noiseReductionFilterRadius = m_data->instance->p_noiseReductionFilterRadius;
      starDetector.hotPixelFilter             = m_data->instance->p_hotPixelFilter;
      starDetector.sensitivity                = m_data->instance->p_sensitivity;
      starDetector.peakResponse               = m_data->instance->p_peakResponse;
      starDetector.maxDistortion              = m_data->instance->p_maxDistortion;
      starDetector.upperLimit                 = m_data->instance->p_upperLimit;
      starDetector.backgroundExpansion        = m_data->instance->p_backgroundExpansion;
      starDetector.xyStretch                  = m_data->instance->p_xyStretch;
      return starDetector.GetStars( m_subframe );
   }

   psf_list FitPSFs( star_list::const_iterator begin, star_list::const_iterator end )
   {
      psf_list PSFs;
      for ( star_list::const_iterator i = begin; i != end; ++i )
      {
         int radius = pcl::Max(3, pcl::Ceil(pcl::Sqrt(i->size)));
         Rect rect( Point( i->position.x - radius, i->position.y - radius ),
                    Point( i->position.x + radius, i->position.y + radius ) );

         PSFFit psfFit( *m_subframe, i->position, rect, PSFFunction( m_data->instance->p_psfFit ),
                        m_data->instance->p_psfFitCircular );
         PSFData psf = psfFit.psf;

         if ( psf.status == PSFFit::FittedOk )
            PSFs.Append( psf );
      }
      return PSFs;
   }

   PSFFit::Function PSFFunction( const pcl_enum& fit )
   {
      switch ( m_data->instance->p_psfFit )
      {
      case SSPSFFit::Gaussian:   return PSFFit::Function::Gaussian;
      case SSPSFFit::Moffat10:   return PSFFit::Function::MoffatA;
      case SSPSFFit::Moffat8:    return PSFFit::Function::Moffat8;
      case SSPSFFit::Moffat6:    return PSFFit::Function::Moffat6;
      case SSPSFFit::Moffat4:    return PSFFit::Function::Moffat4;
      case SSPSFFit::Moffat25:   return PSFFit::Function::Moffat25;
      case SSPSFFit::Moffat15:   return PSFFit::Function::Moffat15;
      case SSPSFFit::Lorentzian: return PSFFit::Function::Lorentzian;
      default: return PSFFit::Function::Gaussian;
      }
   }

   void MeasurePSFs( psf_list fits )
   {
      m_outputData.stars = fits.Length();

      // Determine the best fit to weight the others against
      double minMAD = DBL_MAX;
      for ( const PSFData& psf : fits )
         minMAD = pcl::Min( minMAD, psf.mad );

      // Analyze each star parameter against the best residual
      double fwhmSumSigma = 0;
      double eccentricitySumSigma = 0;
      double residualSumSigma = 0;
      double sumWeight = 0;
      Array<double> fwhms( fits.Length(), 0 );
      Array<double> eccentricities( fits.Length(), 0 );
      Array<double> residuals( fits.Length(), 0 );
      for ( size_type i = 0; i < fits.Length(); ++i )
      {
         PSFData* fit = &fits[i];

         double MAD = fit->mad;
         double weight = minMAD / MAD;
         sumWeight += weight;

         fwhms[i] = pcl::Sqrt( fit->sx * fit->sy );
         eccentricities[i] = pcl::Sqrt(1.0 - pcl::Pow(fit->sy / fit->sx, 2.0));
         residuals[i] = MAD;

         fwhmSumSigma += weight * fwhms[i];
         eccentricitySumSigma += weight * eccentricities[i];
         residualSumSigma += weight * MAD;
      }

      // Average each star parameter against the total weight
      double FWHM = fwhmSumSigma / sumWeight;
      m_outputData.fwhm = PSFData::FWHM( PSFFunction( m_data->instance->p_psfFit ), FWHM, 0 ); // beta is unused here
      m_outputData.eccentricity = eccentricitySumSigma / sumWeight;
      m_outputData.starResidual = residualSumSigma / sumWeight;

      // Determine Mean Deviation for each star parameter
      m_outputData.fwhmMeanDev = pcl::AvgDev( fwhms.Begin(), fwhms.End(),
                                              pcl::Median( fwhms.Begin(), fwhms.End() ) );
      m_outputData.fwhmMeanDev = PSFData::FWHM( PSFFunction( m_data->instance->p_psfFit ),
                                                m_outputData.fwhmMeanDev, 0 ); // beta is unused here
      m_outputData.eccentricityMeanDev = pcl::AvgDev( eccentricities.Begin(), eccentricities.End(),
                                                      pcl::Median( eccentricities.Begin(), eccentricities.End() ) );
      m_outputData.starResidualMeanDev = pcl::AvgDev( residuals.Begin(), residuals.End(),
                                                      pcl::Median( residuals.Begin(), residuals.End() ) );
   }

   int                        m_index;
   AutoPointer<ImageVariant>  m_subframe;
   MeasureData                m_outputData;
   bool                       m_success = false;
   MeasureThreadInputData*    m_data = nullptr;
};

// ----------------------------------------------------------------------------

ImageVariant* SubframeSelectorInstance::LoadSubframe( const String& filePath )
{
   Console console;

   /*
    * Find out an installed file format that can read image files with the
    * specified extension ...
    */
   FileFormat format( File::ExtractExtension( filePath ), true, false );

   /*
    * ... and create a format instance (usually a disk file) to access this
    * subframe image.
    */
   FileFormatInstance file( format );

   /*
    * Open input stream.
    */
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, p_inputHints ) )
      throw CaughtException();

   /*
    * Check for an empty subframe.
    */
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty subframe image." );

   /*
    * Subframe files can store multiple images -
    * this is not supported.
    */
   if ( images.Length() > 1 )
      throw Error( filePath + ": Has multiple images; unsupported " );

   // Create a shared image, 32-bit floating point
   Image* image = new Image( (void*)0, 0, 0 );

   // Read the image
   if ( !file.ReadImage( *image ) )
      throw CaughtException();

   /*
    * Optional pedestal subtraction.
    */
   if ( p_pedestal > 0 )
   {
      Console().WriteLn( String().Format( "Subtracting pedestal: %d DN", p_pedestal ) );
      image->Apply( double( p_pedestal )/TheSSCameraResolutionParameter->ElementData( p_cameraResolution ), ImageOp::Sub );
   }

   /*
    * Convert to grayscale.
    */
   ImageVariant* imageVariant = new ImageVariant();
   image->GetIntensity( *imageVariant );
   image->FreeData();

   /*
    * Close the input stream.
    */
   if ( !file.Close() )
      throw CaughtException();

   return imageVariant;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
    whyNot = "SubframeSelector can only be executed in the global context.";
    return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::IsHistoryUpdater( const View& view ) const
{
    return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanTestStarDetector( String &whyNot ) const
{
   if ( p_subframes.IsEmpty() )
   {
      whyNot = "No subframes have been specified.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::TestStarDetector()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanTestStarDetector( why ) )
         throw Error( why );

      for ( const SubframeItem& subframe : p_subframes )
         if ( subframe.enabled )
            if ( !File::Exists( subframe.path ) )
               throw Error( "No such file exists on the local filesystem: " + subframe.path );
   }

   Console console;

   // Setup common data for each thread
   MeasureThreadInputData inputThreadData;
   inputThreadData.showStarDetectionMaps = true;
   inputThreadData.instance = this;

   try
   {
      /*
       * For all errors generated, we want a report on the console. This is
       * customary in PixInsight for all batch processes.
       */
      Exception::EnableConsoleOutput();
      Exception::DisableGUIOutput();

      console.EnableAbort();
      Module->ProcessEvents();

      try
      {
         /*
          * Extract the first target
          * frame from the targets list,
          * load and calibrate it.
          */
         SubframeItem item = *p_subframes;

         console.WriteLn( String().Format( "<end><cbr><br>Measuring subframe %u of %u", 1, p_subframes.Length() ) );
         Module->ProcessEvents();

         SubframeSelectorMeasureThread* thread =
            new SubframeSelectorMeasureThread( 1, LoadSubframe( item.path ), item.path, &inputThreadData );

         // Keep the GUI responsive, last chance to abort
         Module->ProcessEvents();
         if ( console.AbortRequested() )
            throw ProcessAborted();

         thread->Run();

         Module->ProcessEvents();

         return true;

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

         try
         {
            throw;
         }
         ERROR_HANDLER

         console.ResetStatus();
         console.EnableAbort();

         console.NoteLn( "Abort on error." );
         throw ProcessAborted();
      }
   } // try
   catch ( ... )
   {
      /*
       * All breaking errors are caught here.
       */
      Exception::EnableGUIOutput( true );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanMeasure( String &whyNot ) const
{
   if ( p_subframes.IsEmpty() )
   {
      whyNot = "No subframes have been specified.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::Measure()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanMeasure( why ) )
         throw Error( why );

      for ( const SubframeItem& subframe : p_subframes )
         if ( subframe.enabled )
            if ( !File::Exists( subframe.path ) )
               throw Error( "No such file exists on the local filesystem: " + subframe.path );
   }

   /*
    * Reset measured values.
    */
   p_measures.Clear();

   Console console;

   /*
    * Setup common data for each thread.
    */
   MeasureThreadInputData inputThreadData;
   inputThreadData.instance = this;

   /*
    * Setup the cache.
    */
   if ( TheSubframeSelectorCache == nullptr )
   {
      new SubframeSelectorCache; // loads cache upon construction
      if ( TheSubframeSelectorCache->IsEnabled() )
         if ( TheSubframeSelectorCache->IsEmpty() )
            console.NoteLn( "<end><cbr><br>* Empty file cache" );
         else
            console.NoteLn( "<end><cbr><br>* Loaded cache: " + String( TheSubframeSelectorCache->NumberOfItems() ) + " item(s)" );
   }

   try
   {
      /*
       * For all errors generated, we want a report on the console. This is
       * customary in PixInsight for all batch processes.
       */
      Exception::EnableConsoleOutput();
      Exception::DisableGUIOutput();

      console.EnableAbort();
      Module->ProcessEvents();

      /*
       * Begin subframe measuring process.
       */
      int succeeded = 0;
      int failed = 0;
      int skipped = 0;

      /*
       * Running threads list. Note that IndirectArray<> initializes all item
       * pointers to zero.
       */
      int numberOfThreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 );
      thread_list runningThreads( Min( int( p_subframes.Length() ), numberOfThreads ) );

      /*
       * Pending subframes list. We use this list for temporary storage of
       * indices that need measuring.
       */
      Array<size_type> pendingItems;
      for ( size_type i = 0; i < p_subframes.Length(); ++i )
         if ( p_subframes[i].enabled )
            pendingItems << i;
         else
         {
            console.NoteLn( "* Skipping disabled target: " + p_subframes[i].path );
            ++skipped;
         }
      size_type pendingItemsTotal = pendingItems.Length();

      console.WriteLn( String().Format( "<end><cbr><br>Measuring of %u subframes:", pendingItemsTotal ) );
      console.WriteLn( String().Format( "* Using %u worker threads", runningThreads.Length() ) );

      try
      {
         /*
          * Thread watching loop.
          */
         for ( ;; )
         {
            try
            {
               int running = 0;
               for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
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
                     try
                     {
                        if ( !(*i)->Success() )
                           throw Error( (*i)->ConsoleOutputText() );

                        (*i)->FlushConsoleOutputText();
                        Module->ProcessEvents();

                        // Store output data
                        MeasureItem m( (*i)->Index() );
                        m.Input( (*i)->OutputData() );
                        p_measures << m;
                        (*i)->OutputData().AddToCache();

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

                  /*
                   * If there are items pending, create a new thread and
                   * fire the next one.
                   */
                  if ( !pendingItems.IsEmpty() )
                  {
                     SubframeItem item = p_subframes[*pendingItems];

                     size_type threadIndex = i - runningThreads.Begin();
                     console.NoteLn( String().Format( "<end><cbr><br>[%03u] Measuring subframe %u of %u",
                                                      threadIndex,
                                                      pendingItemsTotal-pendingItems.Length()+1,
                                                      pendingItemsTotal ) );

                     /*
                      * Check for the subframe in the cache, and use that if possible.
                      */
                     MeasureData cacheData( item.path );
                     if ( p_fileCache && cacheData.GetFromCache() )
                     {
                        console.NoteLn( "<end><cbr>* Retrieved data from file cache." );
                        MeasureItem m( pendingItemsTotal-pendingItems.Length()+1, item.path );
                        m.Input( cacheData );
                        p_measures << m;
                        ++succeeded;
                        ++running;
                     }
                     else
                     {
                        /*
                         * If not in cache, create a new thread for this subframe image.
                         */
                        *i = new SubframeSelectorMeasureThread( pendingItemsTotal-pendingItems.Length()+1,
                                                                LoadSubframe( item.path ),
                                                                item.path,
                                                                &inputThreadData );
                        (*i)->Start( ThreadPriority::DefaultMax );
                        ++running;
                     }

                     pendingItems.Remove( pendingItems.Begin() );

                     if ( pendingItems.IsEmpty() )
                        console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                  }
               }

               if ( !running )
                  break;
            }
            catch ( ProcessAborted& )
            {
               throw;
            }
            catch ( ... )
            {
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               ++failed;
               try
               {
                  throw;
               }
               ERROR_HANDLER

               console.ResetStatus();
               console.EnableAbort();

               console.NoteLn( "Abort on error." );
               throw ProcessAborted();
            }
         }
      }
      catch ( ... )
      {
         console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
         for ( SubframeSelectorMeasureThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Abort();
         for ( SubframeSelectorMeasureThread* thread : runningThreads )
            if ( thread != nullptr )
               thread->Wait();
         runningThreads.Destroy();
         throw;
      }

      /*
       * Fail if no images have been measured.
       */
      if ( succeeded == 0 ) {
         if ( failed == 0 )
            throw Error( "No images were measured: Empty subframes list? No enabled subframes?" );
         throw Error( "No image could be measured." );
      }

      /*
       * Write the final report to the console.
       */
      console.NoteLn( String().Format(
         "<end><cbr><br>===== SubframeSelector: %u succeeded, %u failed, %u skipped =====",
         succeeded, failed, skipped ) );

      p_measures.Sort( SubframeSortingBinaryPredicate( SSSortingProperty::Index, 0 ) );

      if ( TheSubframeSelectorMeasurementsInterface != nullptr )
         TheSubframeSelectorMeasurementsInterface->SetMeasurements( p_measures );

      if ( TheSubframeSelectorInterface != nullptr )
      {
         TheSubframeSelectorInterface->ShowExpressionsInterface();
         TheSubframeSelectorInterface->ShowMeasurementsInterface();
      }

      return true;
   } // try
   catch ( ... )
   {
      /*
       * All breaking errors are caught here.
       */
      Exception::EnableGUIOutput( true );
      throw;
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::ApproveMeasurements()
{
   if ( p_approvalExpression.IsEmpty() )
   {
      p_measures.Apply( []( MeasureItem& item )
                        {
                           if ( !item.locked )
                              item.enabled = true;
                        } );
   }
   else
   {
      // First, get all Medians and Mean Deviation from Medians for Sigma units
      MeasureProperties properties = MeasureProperties();
      MeasureUtils::MeasureProperties( p_measures, p_subframeScale, p_scaleUnit,
                                       p_cameraGain, p_cameraResolution, p_dataUnit,
                                       properties );

      for ( MeasureItem& item : p_measures )
      {
         if ( item.locked )
            continue;

         // The standard parameters for the MeasureItem
         String JSEvaluator = item.JavaScriptParameters( p_subframeScale, p_scaleUnit, p_cameraGain,
                                                         TheSSCameraResolutionParameter->ElementData( p_cameraResolution ),
                                                         p_dataUnit, properties );

         // The final expression that evaluates to a return value
         JSEvaluator += p_approvalExpression;

         // Try to get the final result and update the MeasureItem
         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Approval error: Invalid script execution" );
         String resultText = result.ToString();
         if ( resultText.Contains( "Error" ) )
            throw Error( resultText );

         item.enabled = result.ToBool();
      }
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInstance::WeightMeasurements()
{
   if ( p_weightingExpression.IsEmpty() )
   {
      p_measures.Apply( []( MeasureItem& item )
                        {
                           item.weight = 0;
                        } );
   }
   else
   {
      // First, get all Medians and Mean Deviation from Medians for Sigma units
      MeasureProperties properties = MeasureProperties();
      MeasureUtils::MeasureProperties( p_measures, p_subframeScale, p_scaleUnit,
                                       p_cameraGain, p_cameraResolution, p_dataUnit,
                                       properties );

      for ( MeasureItem& item : p_measures )
      {
         // The standard parameters for the MeasureItem
         String JSEvaluator = item.JavaScriptParameters( p_subframeScale, p_scaleUnit, p_cameraGain,
                                                         TheSSCameraResolutionParameter->ElementData( p_cameraResolution ),
                                                         p_dataUnit, properties );

         // The final expression that evaluates to a return value
         JSEvaluator += p_weightingExpression;

         // Try to get the final result and update the MeasureItem
         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Weighting error: Invalid script execution" );
         String resultText = result.ToString();
         if ( resultText.Contains( "Error" ) )
            throw Error( resultText );

         item.weight = result.ToFloat();
      }
   }
}

// ----------------------------------------------------------------------------

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

void SubframeSelectorInstance::WriteMeasuredImage( MeasureItem* item )
{
   Console console;

   /*
    * Output directory.
    */
   String fileDir = p_outputDirectory.Trimmed();
   if ( fileDir.IsEmpty() )
      fileDir = File::ExtractDrive( item->path ) + File::ExtractDirectory( item->path );
   if ( fileDir.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output directory." );
   if ( !fileDir.EndsWith( '/' ) )
      fileDir.Append( '/' );

   /*
    * Input file extension, which defines the input file format.
    */
   String inputFileExtension = File::ExtractExtension( item->path ).Trimmed();
   if ( inputFileExtension.IsEmpty() )
      throw Error( item->path + ": Unable to determine an input file extension." );

   /*
    * Output file extension, which defines the output file format.
    */
   String outputFileExtension = p_outputExtension.Trimmed();
   if ( outputFileExtension.IsEmpty() )
      outputFileExtension = inputFileExtension;
   else if ( !outputFileExtension.StartsWith( '.' ) )
      outputFileExtension.Prepend( '.' );

   /*
    * Output file name.
    */
   String fileName = File::ExtractName( item->path ).Trimmed();
   if ( !p_outputPrefix.IsEmpty() )
      fileName.Prepend( p_outputPrefix );
   if ( !p_outputPostfix.IsEmpty() )
      fileName.Append( p_outputPostfix );
   if ( fileName.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output file name." );

   /*
    * Output file path.
    */
   String outputFilePath = fileDir + fileName + outputFileExtension;

   console.WriteLn( "<end><cbr><br>Writing output file: " + outputFilePath );

   /*
    * Check for an already existing file, and either overwrite it (but show a
    * warning message if that happens), or find a unique file name, depending
    * on the overwriteExistingFiles parameter.
    */
   if ( File::Exists( outputFilePath ) )
      if ( p_overwriteExistingFiles )
         console.WarningLn( "** Warning: Overwriting already existing file." );
      else
      {
         outputFilePath = UniqueFilePath( outputFilePath );
         console.NoteLn( "* File already exists, writing to: " + outputFilePath );
      }

   /*
    * Find installed file formats able to perform the requested read/write
    * operations ...
    */
   FileFormat inputFormat( inputFileExtension, true/*read*/, false/*write*/ );
   FileFormat outputFormat( outputFileExtension, false/*read*/, true/*write*/ );

   if ( outputFormat.IsDeprecated() )
      console.WarningLn( "** Warning: Deprecated file format: " + outputFormat.Name() );

   /*
    * ... and create format instances (usually disk files).
    */
   FileFormatInstance inputFile( inputFormat );
   FileFormatInstance outputFile( outputFormat );

   /*
    * Open the input stream.
    */
   ImageDescriptionArray images;
   if ( !inputFile.Open( images, item->path, p_inputHints ) )
      throw CaughtException();

   /*
    * Check for an empty subframe.
    */
   if ( images.IsEmpty() )
      throw Error( item->path + ": Empty subframe image." );

   /*
    * Subframe files can store multiple images - we don't support them.
    */
   if ( images.Length() > 1 )
      throw Error ( item->path + ": Has multiple images; unsupported " );

   /*
    * Create a shared image with the same pixel format as the input image.
    */
   ImageVariant image;
   image.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                            images[0].options.complexSample,
                            images[0].options.bitsPerSample );
   /*
    * Read the image.
    */
   if ( !inputFile.ReadImage( image ) )
      throw CaughtException();

   /*
    * Create the output stream.
    */
   if ( !outputFile.Create( outputFilePath, p_outputHints ) )
      throw CaughtException();

   /*
    * Determine the output sample format: bits per sample and whether integer
    * or real samples.
    */
   outputFile.SetOptions( images[0].options );

   /*
    * File formats often use format-specific data.
    * Try to keep track of private data structures.
    */
   if ( inputFormat.UsesFormatSpecificData() )
      if ( outputFormat.ValidateFormatSpecificData( inputFile.FormatSpecificData() ) )
         outputFile.SetFormatSpecificData( inputFile.FormatSpecificData() );

   /*
    * Set image properties.
    */
   if ( !inputFile.ReadImageProperties().IsEmpty() )
      if ( outputFormat.CanStoreImageProperties() )
      {
         outputFile.WriteImageProperties( inputFile.ReadImageProperties() );
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
      FITSKeywordArray keywords;
      if ( inputFormat.CanStoreKeywords() )
         inputFile.ReadFITSKeywords( keywords );

      // Remove old weight keywords
      FITSKeywordArray newKeywords;
      for ( const FITSHeaderKeyword& keyword : keywords )
         if ( keyword.name != IsoString( p_outputKeyword ) )
            newKeywords << keyword;

      newKeywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Measured with " + PixInsightVersion::AsString() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with SubframeSelector process" );

      if ( !p_outputKeyword.IsEmpty() )
         newKeywords << FITSHeaderKeyword( p_outputKeyword,
                                           String().Format( "%.6f", item->weight ),
                                           "SubframeSelector.weight" );

      outputFile.WriteFITSKeywords( newKeywords );
   }
   else
   {
      console.WarningLn( "** Warning: The output format cannot store FITS header keywords - subframe weight metadata not embedded." );
   }

   /*
    * Preserve ICC profile, if possible.
    */
   if ( inputFormat.CanStoreICCProfiles() )
   {
      ICCProfile inputProfile;
      inputFile.ReadICCProfile( inputProfile );
      if ( inputProfile.IsProfile() )
         if ( outputFormat.CanStoreICCProfiles() )
            outputFile.WriteICCProfile( inputProfile );
         else
            console.WarningLn( "** Warning: The output format cannot store color profiles - original ICC profile not embedded." );
   }

   /*
    * Close the input stream.
    */
   (void)inputFile.Close();

   /*
    * Write the output image and close the output stream.
    */
   if ( !outputFile.WriteImage( image ) || !outputFile.Close() )
      throw CaughtException();
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanOutput( String &whyNot ) const
{
   if ( p_measures.IsEmpty() )
   {
      whyNot = "No measurements have been made.";
      return false;
   }

   if ( p_outputKeyword.IsEmpty() )
   {
      whyNot = "The specified output keyword is blank.";
      return false;
   }

   if ( !p_outputDirectory.IsEmpty() )
      if ( !File::DirectoryExists( p_outputDirectory ) )
      {
         whyNot = "The specified output directory does not exist.";
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::Output()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanOutput( why ) )
         throw Error( why );

      for ( const MeasureItem& item : p_measures )
         if ( item.enabled )
            if ( !File::Exists( item.path ) )
               throw Error( "No such file exists on the local filesystem: " + item.path );
   }

   size_type o = 0;
   size_type r = 0;

   for ( size_type i = 0; i < p_measures.Length(); ++i )
   {
      try
      {
         if ( p_measures[i].enabled )
         {
            Console().NoteLn( String().Format( "<end><cbr><br>Outputting subframe %u of %u",
                                                i+1, p_measures.Length() ) );
            WriteMeasuredImage( p_measures.At( i ) );
            ++o;
         }
         else
         {
            Console().NoteLn( String().Format( "<end><cbr><br>Skipping subframe %u of %u",
                                                i+1, p_measures.Length() ) );
            ++r;
         }
      }
      catch ( ... )
      {
         Console console;
         console.Note( "<end><cbr><br>* Applying error policy: " );

         switch ( p_onError )
         {
         default: // ?
         case SSOnError::Continue:
            console.NoteLn( "Continue on error." );
            continue;

         case SSOnError::Abort:
            console.NoteLn( "Abort on error." );
            throw;

         case SSOnError::AskUser:
            {
               console.NoteLn( "Ask on error..." );

               int r = MessageBox( "<p style=\"white-space:pre;\">"
                                   "An error occurred during SubframeSelector Output. What do you want to do?</p>",
                                   "SubframeSelector",
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
   }

   Console().NoteLn( String().Format( "<end><cbr><br>%u Output subframes, %u Rejected subframes, %u total",
                                      o, r, p_measures.Length() ) );
   return true;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanExecuteGlobal( String &whyNot ) const
{
   switch ( p_routine )
   {
   case SSRoutine::StarDetectionPreview:
      if ( p_subframes.IsEmpty() )
      {
         whyNot = "No subframes have been specified.";
         return false;
      }
      break;
   case SSRoutine::MeasureSubframes:
      if ( p_subframes.IsEmpty() )
      {
         whyNot = "No subframes have been specified.";
         return false;
      }
      break;
   case SSRoutine::OutputSubframes:
      if ( p_measures.IsEmpty() )
      {
         whyNot = "No measurements have been made.";
         return false;
      }
      if ( p_outputKeyword.IsEmpty() )
      {
         whyNot = "The specified output keyword is blank.";
         return false;
      }
      if ( !p_outputDirectory.IsEmpty() )
         if ( !File::DirectoryExists( p_outputDirectory ) )
         {
            whyNot = "The specified output directory does not exist.";
            return false;
         }
      break;
   default:
      whyNot = String().Format( "Internal error: Unknown routine 0x%04X", unsigned( p_routine ) );
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );

      for ( const SubframeItem& subframe : p_subframes )
         if ( subframe.enabled )
            if ( !File::Exists( subframe.path ) )
               throw Error( "No such file exists on the local filesystem: " + subframe.path );
   }

   /*
    * Perform the selected routine.
    */
   switch ( p_routine )
   {
   case SSRoutine::StarDetectionPreview:
      return TestStarDetector();
   case SSRoutine::MeasureSubframes:
      return Measure();
   case SSRoutine::OutputSubframes:
      return Output();
   default:
      throw Error( String().Format( "Internal error: Unknown routine 0x%04X", unsigned( p_routine ) ) );
   }
}

// ----------------------------------------------------------------------------

void* SubframeSelectorInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSRoutineParameter )
      return &p_routine;

   else if ( p == TheSSSubframeEnabledParameter )
      return &p_subframes[tableRow].enabled;
   else if ( p == TheSSSubframePathParameter )
      return p_subframes[tableRow].path.Begin();

   else if ( p == TheSSFileCacheParameter )
      return &p_fileCache;

   else if ( p == TheSSSubframeScaleParameter )
      return &p_subframeScale;
   else if ( p == TheSSCameraGainParameter )
      return &p_cameraGain;
   else if ( p == TheSSCameraResolutionParameter )
      return &p_cameraResolution;
   else if ( p == TheSSSiteLocalMidnightParameter )
      return &p_siteLocalMidnight;
   else if ( p == TheSSScaleUnitParameter )
      return &p_scaleUnit;
   else if ( p == TheSSDataUnitParameter )
      return &p_dataUnit;

   else if ( p == TheSSStructureLayersParameter )
      return &p_structureLayers;
   else if ( p == TheSSNoiseLayersParameter )
      return &p_noiseLayers;
   else if ( p == TheSSHotPixelFilterRadiusParameter )
      return &p_hotPixelFilterRadius;
   else if ( p == TheSSApplyHotPixelFilterParameter )
      return &p_hotPixelFilter;
   else if ( p == TheSSNoiseReductionFilterRadiusParameter )
      return &p_noiseReductionFilterRadius;
   else if ( p == TheSSSensitivityParameter )
      return &p_sensitivity;
   else if ( p == TheSSPeakResponseParameter )
      return &p_peakResponse;
   else if ( p == TheSSMaxDistortionParameter )
      return &p_maxDistortion;
   else if ( p == TheSSUpperLimitParameter )
      return &p_upperLimit;
   else if ( p == TheSSBackgroundExpansionParameter )
      return &p_backgroundExpansion;
   else if ( p == TheSSXYStretchParameter )
      return &p_xyStretch;
   else if ( p == TheSSPSFFitParameter )
      return &p_psfFit;
   else if ( p == TheSSPSFFitCircularParameter )
      return &p_psfFitCircular;
   else if ( p == TheSSPedestalParameter )
      return &p_pedestal;
   else if ( p == TheSSROIX0Parameter )
      return &p_roi.x0;
   else if ( p == TheSSROIY0Parameter )
      return &p_roi.y0;
   else if ( p == TheSSROIX1Parameter )
      return &p_roi.x1;
   else if ( p == TheSSROIY1Parameter )
      return &p_roi.y1;

   else if ( p == TheSSInputHintsParameter )
      return p_inputHints.Begin();
   else if ( p == TheSSOutputHintsParameter )
      return p_outputHints.Begin();

   else if ( p == TheSSOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   else if ( p == TheSSOutputExtensionParameter )
      return p_outputExtension.Begin();
   else if ( p == TheSSOutputPrefixParameter )
      return p_outputPrefix.Begin();
   else if ( p == TheSSOutputPostfixParameter )
      return p_outputPostfix.Begin();
   else if ( p == TheSSOutputKeywordParameter )
      return p_outputKeyword.Begin();
   else if ( p == TheSSOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   else if ( p == TheSSOnErrorParameter )
      return &p_onError;

   else if ( p == TheSSApprovalExpressionParameter )
      return p_approvalExpression.Begin();
   else if ( p == TheSSWeightingExpressionParameter )
      return p_weightingExpression.Begin();

   else if ( p == TheSSSortingPropertyParameter )
      return &p_sortingProperty;
   else if ( p == TheSSGraphPropertyParameter )
      return &p_graphProperty;

   else if ( p == TheSSMeasurementIndexParameter )
      return &p_measures[tableRow].index;
   else if ( p == TheSSMeasurementEnabledParameter )
      return &p_measures[tableRow].enabled;
   else if ( p == TheSSMeasurementLockedParameter )
      return &p_measures[tableRow].locked;
   else if ( p == TheSSMeasurementPathParameter )
      return p_measures[tableRow].path.Begin();
   else if ( p == TheSSMeasurementWeightParameter )
      return &p_measures[tableRow].weight;
   else if ( p == TheSSMeasurementFWHMParameter )
      return &p_measures[tableRow].fwhm;
   else if ( p == TheSSMeasurementEccentricityParameter )
      return &p_measures[tableRow].eccentricity;
   else if ( p == TheSSMeasurementSNRWeightParameter )
      return &p_measures[tableRow].snrWeight;
   else if ( p == TheSSMeasurementMedianParameter )
      return &p_measures[tableRow].median;
   else if ( p == TheSSMeasurementMedianMeanDevParameter )
      return &p_measures[tableRow].medianMeanDev;
   else if ( p == TheSSMeasurementNoiseParameter )
      return &p_measures[tableRow].noise;
   else if ( p == TheSSMeasurementNoiseRatioParameter )
      return &p_measures[tableRow].noiseRatio;
   else if ( p == TheSSMeasurementStarsParameter )
      return &p_measures[tableRow].stars;
   else if ( p == TheSSMeasurementStarResidualParameter )
      return &p_measures[tableRow].starResidual;
   else if ( p == TheSSMeasurementFWHMMeanDevParameter )
      return &p_measures[tableRow].fwhmMeanDev;
   else if ( p == TheSSMeasurementEccentricityMeanDevParameter )
      return &p_measures[tableRow].eccentricityMeanDev;
   else if ( p == TheSSMeasurementStarResidualMeanDevParameter )
      return &p_measures[tableRow].starResidualMeanDev;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSSubframesParameter )
   {
      p_subframes.Clear();
      if ( sizeOrLength > 0 )
         p_subframes.Add( SubframeItem(), sizeOrLength );
   }
   else if ( p == TheSSSubframePathParameter )
   {
      p_subframes[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_subframes[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheSSInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPrefixParameter )
   {
      p_outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPostfixParameter )
   {
      p_outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputKeywordParameter )
   {
      p_outputKeyword.Clear();
      if ( sizeOrLength > 0 )
         p_outputKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheSSApprovalExpressionParameter )
   {
      p_approvalExpression.Clear();
      if ( sizeOrLength > 0 )
         p_approvalExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSWeightingExpressionParameter )
   {
      p_weightingExpression.Clear();
      if ( sizeOrLength > 0 )
         p_weightingExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSMeasurementsParameter )
   {
      p_measures.Clear();
      if ( sizeOrLength > 0 )
         for ( size_type i = 0; i < sizeOrLength; ++i )
            p_measures.Add( MeasureItem( i ) );
   }
   else if ( p == TheSSMeasurementPathParameter )
   {
      p_measures[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_measures[tableRow].path.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type SubframeSelectorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSSSubframesParameter )
      return p_subframes.Length();
   else if ( p == TheSSSubframePathParameter )
      return p_subframes[tableRow].path.Length();

   else if ( p == TheSSInputHintsParameter )
      return p_inputHints.Length();
   else if ( p == TheSSOutputHintsParameter )
      return p_outputHints.Length();

   else if ( p == TheSSOutputDirectoryParameter )
      return p_outputDirectory.Length();
   else if ( p == TheSSOutputExtensionParameter )
      return p_outputExtension.Length();
   else if ( p == TheSSOutputPrefixParameter )
      return p_outputPrefix.Length();
   else if ( p == TheSSOutputPostfixParameter )
      return p_outputPostfix.Length();
   else if ( p == TheSSOutputKeywordParameter )
      return p_outputKeyword.Length();

   else if ( p == TheSSApprovalExpressionParameter )
      return p_approvalExpression.Length();
   else if ( p == TheSSWeightingExpressionParameter )
      return p_weightingExpression.Length();

   else if ( p == TheSSMeasurementsParameter )
      return p_measures.Length();
   else if ( p == TheSSMeasurementPathParameter )
      return p_measures[tableRow].path.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.cpp - Released 2018-11-23T18:45:58Z
