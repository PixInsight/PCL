//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.02.01.0002
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.cpp - Released 2017-11-05T16:00:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ProcessInstance.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ATrousWaveletTransform.h>
#include <pcl/Version.h>
#include <pcl/ICCProfile.h>
#include <pcl/MessageBox.h>

#include "PSF.h"
#include "SubframeSelectorInstance.h"
#include "SubframeSelectorStarDetector.h"
#include "SubframeSelectorInterface.h"

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
   routine( SSRoutine::Default ),
   subframes(),
   subframeScale( TheSSSubframeScaleParameter->DefaultValue() ),
   cameraGain( TheSSCameraGainParameter->DefaultValue() ),
   cameraResolution( SSCameraResolution::Default ),
   siteLocalMidnight( TheSSSiteLocalMidnightParameter->DefaultValue() ),
   scaleUnit( SSScaleUnit::Default ),
   dataUnit( SSDataUnit::Default ),
   structureLayers( TheSSStructureLayersParameter->DefaultValue() ),
   noiseLayers( TheSSNoiseLayersParameter->DefaultValue() ),
   hotPixelFilterRadius( TheSSHotPixelFilterRadiusParameter->DefaultValue() ),
   applyHotPixelFilterToDetectionImage( TheSSApplyHotPixelFilterParameter->DefaultValue() ),
   noiseReductionFilterRadius( TheSSNoiseReductionFilterRadiusParameter->DefaultValue() ),
   sensitivity( TheSSSensitivityParameter->DefaultValue() ),
   peakResponse( TheSSPeakResponseParameter->DefaultValue() ),
   maxDistortion( TheSSMaxDistortionParameter->DefaultValue() ),
   upperLimit( TheSSUpperLimitParameter->DefaultValue() ),
   backgroundExpansion( TheSSBackgroundExpansionParameter->DefaultValue() ),
   xyStretch( TheSSXYStretchParameter->DefaultValue() ),
   psfFit( SSPSFFit::Default ),
   psfFitCircular( TheSSPSFFitCircularParameter->DefaultValue() ),
   pedestal( TheSSPedestalParameter->DefaultValue() ),
   roi( 0 ),
   outputDirectory( TheSSOutputDirectoryParameter->DefaultValue() ),
   outputPrefix( TheSSOutputPrefixParameter->DefaultValue() ),
   outputPostfix( TheSSOutputPostfixParameter->DefaultValue() ),
   outputKeyword( TheSSOutputKeywordParameter->DefaultValue() ),
   overwriteExistingFiles( TheSSOverwriteExistingFilesParameter->DefaultValue() ),
   onError( SSOnError::Default ),
   approvalExpression( "" ),
   weightingExpression( "" ),
   sortingProperty( SSSortingProperty::Default ),
   graphProperty( SSGraphProperty::Default ),
   measures()
{
}

SubframeSelectorInstance::SubframeSelectorInstance( const SubframeSelectorInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void SubframeSelectorInstance::Assign( const ProcessImplementation& p )
{
   const SubframeSelectorInstance* x = dynamic_cast<const SubframeSelectorInstance*>( &p );
   if ( x != nullptr )
   {
      routine                                = x->routine;
      subframes                              = x->subframes;
      subframeScale                          = x->subframeScale;
      cameraGain                             = x->cameraGain;
      cameraResolution                       = x->cameraResolution;
      siteLocalMidnight                      = x->siteLocalMidnight;
      scaleUnit                              = x->scaleUnit;
      dataUnit                               = x->dataUnit;
      structureLayers                        = x->structureLayers;
      noiseLayers                            = x->noiseLayers;
      hotPixelFilterRadius                   = x->hotPixelFilterRadius;
      applyHotPixelFilterToDetectionImage    = x->applyHotPixelFilterToDetectionImage;
      noiseReductionFilterRadius             = x->noiseReductionFilterRadius;
      sensitivity                            = x->sensitivity;
      peakResponse                           = x->peakResponse;
      maxDistortion                          = x->maxDistortion;
      upperLimit                             = x->upperLimit;
      backgroundExpansion                    = x->backgroundExpansion;
      xyStretch                              = x->xyStretch;
      psfFit                                 = x->psfFit;
      psfFitCircular                         = x->psfFitCircular;
      pedestal                               = x->pedestal;
      roi                                    = x->roi;
      outputDirectory                        = x->outputDirectory;
      outputPrefix                           = x->outputPrefix;
      outputPostfix                          = x->outputPostfix;
      outputKeyword                          = x->outputKeyword;
      overwriteExistingFiles                 = x->overwriteExistingFiles;
      onError                                = x->onError;
      approvalExpression                     = x->approvalExpression;
      weightingExpression                    = x->weightingExpression;
      sortingProperty                        = x->sortingProperty;
      graphProperty                          = x->graphProperty;
      measures                               = x->measures;
   }
}

// ----------------------------------------------------------------------------

struct MeasureThreadInputData
{
   // The static settings
   pcl_bool                   showStarDetectionMaps = false;
   SubframeSelectorInstance*  instance = nullptr;
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
           m_success( false ),
           m_data( data )
   {
   }

   virtual void Run()
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
         console.WriteLn( "Image Calculations: " + T.ToIsoString() );
         T.Reset();

         if ( IsAborted() )
            throw Error( "Aborted" );

         /*
          * Crop if the ROI was set
          */
         if ( m_data->instance->roi.IsRect() )
            m_subframe->CropTo( m_data->instance->roi );

         // Run the Star Detector
         star_list stars = StarDetector();
         console.WriteLn( "     Star Detector: " + T.ToIsoString() );

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
         T.Reset();
         psf_list fits = FitPSFs( stars.Begin(), stars.End() );
         console.WriteLn( "          Fit PSFs: " + T.ToIsoString() );

         if ( fits.IsEmpty() )
         {
            m_success = false;
            return;
         }

         if ( IsAborted() )
            throw Error( "Aborted" );

         // Measure Data
         T.Reset();
         MeasurePSFs( fits );
         console.WriteLn( "  PSF Calculations: " + T.ToIsoString() );

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
      starDetector.showStarDetectionMaps                 = m_data->showStarDetectionMaps;
      starDetector.structureLayers                       = m_data->instance->structureLayers;
      starDetector.noiseLayers                           = m_data->instance->noiseLayers;
      starDetector.hotPixelFilterRadius                  = m_data->instance->hotPixelFilterRadius;
      starDetector.noiseReductionFilterRadius            = m_data->instance->noiseReductionFilterRadius;
      starDetector.applyHotPixelFilterToDetectionImage   = m_data->instance->applyHotPixelFilterToDetectionImage;
      starDetector.sensitivity                           = m_data->instance->sensitivity;
      starDetector.peakResponse                          = m_data->instance->peakResponse;
      starDetector.maxDistortion                         = m_data->instance->maxDistortion;
      starDetector.upperLimit                            = m_data->instance->upperLimit;
      starDetector.backgroundExpansion                   = m_data->instance->backgroundExpansion;
      starDetector.xyStretch                             = m_data->instance->xyStretch;
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

         PSFFit psfFit( *m_subframe, i->position, rect, PSFFunction( m_data->instance->psfFit ),
                        m_data->instance->psfFitCircular );
         PSFData psf = psfFit.psf;

         if ( psf.status == PSFFit::FittedOk )
            PSFs.Append( psf );
      }
      return PSFs;
   }

   PSFFit::Function PSFFunction( const pcl_enum& fit )
   {
      switch ( m_data->instance->psfFit )
      {
      case SSPSFFit::Gaussian: return PSFFit::Function::Gaussian;
      case SSPSFFit::Moffat10: return PSFFit::Function::MoffatA;
      case SSPSFFit::Moffat8: return PSFFit::Function::Moffat8;
      case SSPSFFit::Moffat6: return PSFFit::Function::Moffat6;
      case SSPSFFit::Moffat4: return PSFFit::Function::Moffat4;
      case SSPSFFit::Moffat25: return PSFFit::Function::Moffat25;
      case SSPSFFit::Moffat15: return PSFFit::Function::Moffat15;
      case SSPSFFit::Lorentzian: return PSFFit::Function::Lorentzian;
      default: return PSFFit::Function::Gaussian;
      }
   }

   void MeasurePSFs( psf_list fits )
   {
      m_outputData.stars = fits.Length();

      // Determine the best fit to weight the others against
      double minMAD = -1;
      for ( psf_list::const_iterator i = fits.Begin(); i != fits.End(); ++i )
      {
         double MAD = i->mad;

         if ( minMAD <= 0 )
         {
            minMAD = MAD;
            continue;
         }

         if ( MAD < minMAD )
            minMAD = MAD;
      }

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
      m_outputData.fwhm = PSFData::FWHM( PSFFunction( m_data->instance->psfFit ), FWHM, 0 ); // beta is unused here
      m_outputData.eccentricity = eccentricitySumSigma / sumWeight;
      m_outputData.starResidual = residualSumSigma / sumWeight;

      // Determine Mean Deviation for each star parameter
      m_outputData.fwhmMeanDev = pcl::AvgDev( fwhms.Begin(), fwhms.End(),
                                              pcl::Mean( fwhms.Begin(), fwhms.End() ) );
      m_outputData.fwhmMeanDev = PSFData::FWHM( PSFFunction( m_data->instance->psfFit ),
                                                m_outputData.fwhmMeanDev, 0 ); // beta is unused here
      m_outputData.eccentricityMeanDev = pcl::AvgDev( eccentricities.Begin(), eccentricities.End(),
                                                      pcl::Mean( eccentricities.Begin(), eccentricities.End() ) );
      m_outputData.starResidualMeanDev = pcl::AvgDev( residuals.Begin(), residuals.End(),
                                                      pcl::Mean( residuals.Begin(), residuals.End() ) );
   }

   int                        m_index;
   AutoPointer<ImageVariant>  m_subframe;
   MeasureData                m_outputData;
   bool                       m_success : 1;

   MeasureThreadInputData* m_data;
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
   if ( !file.Open( images, filePath ) )
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
      throw Error ( filePath + ": Has multiple images; unsupported " );

   // Create a shared image, 32-bit floating point
   Image* image = new Image( (void*)0, 0, 0 );

   // Read the image
   if ( !file.ReadImage( *image ) )
      throw CaughtException();


   /*
    * Optional pedestal subtraction.
    */
   if ( pedestal > 0 )
   {
      Console().WriteLn( String().Format( "Subtracting pedestal: %d DN", pedestal ) );
      image->Apply( (double) pedestal / (double) TheSSCameraResolutionParameter->ElementData(cameraResolution), ImageOp::Sub );
   }

   /*
    * Convert to grayscale
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


/*
 * Read a subframe file. Returns a list of measuring threads ready to
 * measure all subimages loaded from the file.
 */
thread_list
SubframeSelectorInstance::CreateThreadForSubframe( int index, const String& filePath, MeasureThreadInputData* threadData )
{
   thread_list threads;
   threads.Add( new SubframeSelectorMeasureThread( index,
                                                   LoadSubframe( filePath ),
                                                   filePath,
                                                   threadData ) );
   return threads;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
    whyNot = "SubframeSelector can only be executed in the global context.";
    return false;
}

bool SubframeSelectorInstance::IsHistoryUpdater( const View& view ) const
{
    return false;
}

bool SubframeSelectorInstance::CanTestStarDetector( String &whyNot ) const {
   if ( subframes.IsEmpty()) {
      whyNot = "No subframes have been specified.";
      return false;
   }

   return true;
}

bool SubframeSelectorInstance::TestStarDetector() {
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanTestStarDetector( why ))
         throw Error( why );

      for ( subframe_list::const_iterator i = subframes.Begin(); i != subframes.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ))
            throw ("No such file exists on the local filesystem: " + i->path);
   }
   Console console;

   MeasureThreadInputData inputThreadData;
   inputThreadData.showStarDetectionMaps  = true;
   inputThreadData.instance               = this;

   try {
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
         SubframeItem item = *subframes;

         console.WriteLn( String().Format( "<end><cbr><br>Measuring subframe %u of %u", 1, subframes.Length() ) );
         Module->ProcessEvents();

         thread_list threads = CreateThreadForSubframe( 1, item.path, &inputThreadData );
         SubframeSelectorMeasureThread* thread = *threads;

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

   catch ( ... ) {
      /*
       * All breaking errors are caught here.
       */
      Exception::EnableGUIOutput( true );
      throw;
   }
}

bool SubframeSelectorInstance::CanMeasure( String &whyNot ) const {
   if ( subframes.IsEmpty()) {
      whyNot = "No subframes have been specified.";
      return false;
   }

   return true;
}

bool SubframeSelectorInstance::Measure() {
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanMeasure( why ))
         throw Error( why );

      for ( subframe_list::const_iterator i = subframes.Begin(); i != subframes.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ))
            throw ("No such file exists on the local filesystem: " + i->path);
   }

   // Reset measured values and create temporary list
   measures.Clear();

   Console console;

   MeasureThreadInputData inputThreadData;
   inputThreadData.instance = this;

   try {

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
      thread_list runningThreads( Min( int( subframes.Length()), numberOfThreads ));

      /*
       * Waiting threads list. We use this list for temporary storage of
       * measuring threads for multiple image files.
       */
      thread_list waitingThreads;

      /*
       * Flag to signal the beginning of the final waiting period, when there
       * are no more pending images but there are still running threads.
       */
      bool waitingForFinished = false;

      /*
       * We'll work on a temporary duplicate of the subframes list. This
       * allows us to modify the working list without altering the instance.
       */
      subframe_list subframes_copy( subframes );

      console.WriteLn( String().Format( "<end><cbr><br>Measuring of %u subframes:", subframes.Length() ) );
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

                  if ( *j == nullptr )
                  {
                     /*
                      * This is a free thread slot. Ignore it if we don't have
                      * more pending images to feed in.
                      */
                     if ( subframes_copy.IsEmpty() && waitingThreads.IsEmpty() )
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

                     (*i)->FlushConsoleOutputText();
                     Module->ProcessEvents();

                     // Store output data
                     MeasureItem m( (*i)->Index() );
                     m.Input( (*i)->OutputData() );
                     measures.Append( m );

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
                  if ( subframes_copy.IsEmpty() )
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
                           console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate ..." );
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
                  SubframeItem item = *subframes_copy;
                  subframes_copy.Remove( subframes_copy.Begin() );

                  console.WriteLn( String().Format( "<end><cbr><br>Measuring subframe %u of %u",
                                                    subframes.Length()-subframes_copy.Length(),
                                                    subframes.Length() ) );
                  if ( !item.enabled )
                  {
                     console.NoteLn( "* Skipping disabled target" );
                     ++skipped;
                     continue;
                  }

                  /*
                   * Create a new thread for this subframe image
                   */
                  thread_list threads = CreateThreadForSubframe( subframes.Length()-subframes_copy.Length(),
                                                                 item.path, &inputThreadData );

                  /*
                   * Put the new thread in the free slot.
                   */
                  *i = *threads;
                  threads.Remove( threads.Begin() );

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

               console.NoteLn( "Abort on error." );
               throw ProcessAborted();
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
      console.NoteLn(
              String().Format( "<end><cbr><br>===== SubframeSelector: %u succeeded, %u failed, %u skipped =====",
                               succeeded, failed, skipped ));

      measures.Sort( SubframeSortingBinaryPredicate( SSSortingProperty::Index, 0 ) );

      if ( TheSubframeSelectorInterface != nullptr )
      {
         TheSubframeSelectorInterface->ClearMeasurements();
         TheSubframeSelectorInterface->AddMeasurements( measures.Begin(), measures.End() );
         TheSubframeSelectorInterface->UpdateMeasurementImagesList();
         TheSubframeSelectorInterface->UpdateMeasurementGraph();
      }
      return true;
   } // try

   catch ( ... ) {
      /*
       * All breaking errors are caught here.
       */
      Exception::EnableGUIOutput( true );
      throw;
   }
}

void SubframeSelectorInstance::ApproveMeasurements()
{
   if ( approvalExpression.IsEmpty() )
   {
      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
         if ( !i->locked )
            i->enabled = true;
   }
   else
   {
      // First, get all Medians and Mean Deviation from Medians for Sigma units
      double weightMedian, weightDeviation;
      double fwhmMedian, fwhmDeviation;
      double eccentricityMedian, eccentricityDeviation;
      double snrWeightMedian, snrWeightDeviation;
      double medianMedian, medianDeviation;
      double medianMeanDevMedian, medianMeanDevDeviation;
      double noiseMedian, noiseDeviation;
      double noiseRatioMedian, noiseRatioDeviation;
      double starsMedian, starsDeviation;
      double starResidualMedian, starResidualDeviation;
      double fwhmMeanDevMedian, fwhmMeanDevDeviation;
      double eccentricityMeanDevMedian, eccentricityMeanDevDeviation;
      double starResidualMeanDevMedian, starResidualMeanDevDeviation;
      MedianAndMeanDeviation( weightMedian, weightDeviation,
                              fwhmMedian, fwhmDeviation,
                              eccentricityMedian, eccentricityDeviation,
                              snrWeightMedian, snrWeightDeviation,
                              medianMedian, medianDeviation,
                              medianMeanDevMedian, medianMeanDevDeviation,
                              noiseMedian, noiseDeviation,
                              noiseRatioMedian, noiseRatioDeviation,
                              starsMedian, starsDeviation,
                              starResidualMedian, starResidualDeviation,
                              fwhmMeanDevMedian, fwhmMeanDevDeviation,
                              eccentricityMeanDevMedian, eccentricityMeanDevDeviation,
                              starResidualMeanDevMedian, starResidualMeanDevDeviation
      );

      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
      {
         if ( i->locked )
            continue;

         // The standard parameters for the MeasureItem
         String JSEvaluator = i->JavaScriptParameters( subframeScale, scaleUnit, cameraGain,
                                                       TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                                       dataUnit );

         // The Sigma parameters for the MeasureItem
         JSEvaluator += String().Format( "let WeightSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->weight, weightMedian, weightDeviation
         ) );
         JSEvaluator += String().Format( "let FWHMSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->FWHM( subframeScale, scaleUnit ), fwhmMedian, fwhmDeviation
         ) );
         JSEvaluator += String().Format( "let EccentricitySigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->eccentricity, eccentricityMedian, eccentricityDeviation
         ) );
         JSEvaluator += String().Format( "let SNRWeightSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->snrWeight, snrWeightMedian, snrWeightDeviation
         ) );
         JSEvaluator += String().Format( "let MedianSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->Median( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit ),
                 medianMedian, medianDeviation
         ) );
         JSEvaluator += String().Format( "let MedianMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->MedianMeanDev( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                   dataUnit ),
                 medianMeanDevMedian, medianMeanDevDeviation
         ) );
         JSEvaluator += String().Format( "let NoiseSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->Noise( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit ),
                 noiseMedian, noiseDeviation
         ) );
         JSEvaluator += String().Format( "let NoiseRatioSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->noiseRatio, noiseRatioMedian, noiseRatioDeviation
         ) );
         JSEvaluator += String().Format( "let StarsSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->stars, starsMedian, starsDeviation
         ) );
         JSEvaluator += String().Format( "let StarResidualSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->starResidual, starResidualMedian, starResidualDeviation
         ) );
         JSEvaluator += String().Format( "let FWHMMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->FWHMMeanDeviation( subframeScale, scaleUnit ), fwhmMeanDevMedian, fwhmMeanDevDeviation
         ) );
         JSEvaluator += String().Format( "let EccentricityMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->eccentricityMeanDev, eccentricityMeanDevMedian, eccentricityMeanDevDeviation
         ) );
         JSEvaluator += String().Format( "let StarResidualMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->starResidualMeanDev, starResidualMeanDevMedian, starResidualMeanDevDeviation
         ) );

         // The final expression that evaluates to a return value
         JSEvaluator += approvalExpression;

         // Try to get the final result and update the MeasureItem
         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Approval Error: Invalid script execution" );
         String resultText = result.ToString();
         if ( resultText.Contains( "Error" ) )
            throw Error( resultText );

         i->enabled = result.ToBool();
      }
   }
}

void SubframeSelectorInstance::WeightMeasurements()
{
   if ( weightingExpression.IsEmpty() )
   {
      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
         i->weight = 0;
   }
   else
   {
      // First, get all Medians and Mean Deviation from Medians for Sigma units
      double weightMedian, weightDeviation;
      double fwhmMedian, fwhmDeviation;
      double eccentricityMedian, eccentricityDeviation;
      double snrWeightMedian, snrWeightDeviation;
      double medianMedian, medianDeviation;
      double medianMeanDevMedian, medianMeanDevDeviation;
      double noiseMedian, noiseDeviation;
      double noiseRatioMedian, noiseRatioDeviation;
      double starsMedian, starsDeviation;
      double starResidualMedian, starResidualDeviation;
      double fwhmMeanDevMedian, fwhmMeanDevDeviation;
      double eccentricityMeanDevMedian, eccentricityMeanDevDeviation;
      double starResidualMeanDevMedian, starResidualMeanDevDeviation;
      MedianAndMeanDeviation( weightMedian, weightDeviation,
                              fwhmMedian, fwhmDeviation,
                              eccentricityMedian, eccentricityDeviation,
                              snrWeightMedian, snrWeightDeviation,
                              medianMedian, medianDeviation,
                              medianMeanDevMedian, medianMeanDevDeviation,
                              noiseMedian, noiseDeviation,
                              noiseRatioMedian, noiseRatioDeviation,
                              starsMedian, starsDeviation,
                              starResidualMedian, starResidualDeviation,
                              fwhmMeanDevMedian, fwhmMeanDevDeviation,
                              eccentricityMeanDevMedian, eccentricityMeanDevDeviation,
                              starResidualMeanDevMedian, starResidualMeanDevDeviation
      );

      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
      {
         // The standard parameters for the MeasureItem
         String JSEvaluator = i->JavaScriptParameters( subframeScale, scaleUnit, cameraGain,
                                                       TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                                       dataUnit );

         // The Sigma parameters for the MeasureItem
         JSEvaluator += String().Format( "let WeightSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->weight, weightMedian, weightDeviation
         ) );
         JSEvaluator += String().Format( "let FWHMSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->FWHM( subframeScale, scaleUnit ), fwhmMedian, fwhmDeviation
         ) );
         JSEvaluator += String().Format( "let EccentricitySigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->eccentricity, eccentricityMedian, eccentricityDeviation
         ) );
         JSEvaluator += String().Format( "let SNRWeightSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->snrWeight, snrWeightMedian, snrWeightDeviation
         ) );
         JSEvaluator += String().Format( "let MedianSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->Median( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit ),
                 medianMedian, medianDeviation
         ) );
         JSEvaluator += String().Format( "let MedianMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->MedianMeanDev( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                   dataUnit ),
                 medianMeanDevMedian, medianMeanDevDeviation
         ) );
         JSEvaluator += String().Format( "let NoiseSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->Noise( cameraGain, TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit ),
                 noiseMedian, noiseDeviation
         ) );
         JSEvaluator += String().Format( "let NoiseRatioSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->noiseRatio, noiseRatioMedian, noiseRatioDeviation
         ) );
         JSEvaluator += String().Format( "let StarsSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->stars, starsMedian, starsDeviation
         ) );
         JSEvaluator += String().Format( "let StarResidualSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->starResidual, starResidualMedian, starResidualDeviation
         ) );
         JSEvaluator += String().Format( "let FWHMMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->FWHMMeanDeviation( subframeScale, scaleUnit ), fwhmMeanDevMedian, fwhmMeanDevDeviation
         ) );
         JSEvaluator += String().Format( "let EccentricityMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->eccentricityMeanDev, eccentricityMeanDevMedian, eccentricityMeanDevDeviation
         ) );
         JSEvaluator += String().Format( "let StarResidualMeanDevSigma = %.4f;\n", MeasureUtils::DeviationNormalize(
                 i->starResidualMeanDev, starResidualMeanDevMedian, starResidualMeanDevDeviation
         ) );

         // The final expression that evaluates to a return value
         JSEvaluator += weightingExpression;

         // Try to get the final result and update the MeasureItem
         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Approval Error: Invalid script execution" );
         String resultText = result.ToString();
         if ( resultText.Contains( "Error" ) )
            throw Error( resultText );

         i->weight = result.ToFloat();
      }
   }
}

void SubframeSelectorInstance::MedianAndMeanDeviation( double& weightMedian, double& weightDeviation,
                                                       double& fwhmMedian, double& fwhmDeviation,
                                                       double& eccentricityMedian, double& eccentricityDeviation,
                                                       double& snrWeightMedian, double& snrWeightDeviation,
                                                       double& medianMedian, double& medianDeviation,
                                                       double& medianMeanDevMedian, double& medianMeanDevDeviation,
                                                       double& noiseMedian, double& noiseDeviation,
                                                       double& noiseRatioMedian, double& noiseRatioDeviation,
                                                       double& starsMedian, double& starsDeviation,
                                                       double& starResidualMedian, double& starResidualDeviation,
                                                       double& fwhmMeanDevMedian, double& fwhmMeanDevDeviation,
                                                       double& eccentricityMeanDevMedian, double& eccentricityMeanDevDeviation,
                                                       double& starResidualMeanDevMedian, double& starResidualMeanDevDeviation
) const
{
   size_type measuresLength( measures.Length() );

   Array<double> weight( measuresLength );
   Array<double> fwhm( measuresLength );
   Array<double> eccentricity( measuresLength );
   Array<double> snrWeight( measuresLength );
   Array<double> median( measuresLength );
   Array<double> medianMeanDev( measuresLength );
   Array<double> noise( measuresLength );
   Array<double> noiseRatio( measuresLength );
   Array<double> stars( measuresLength );
   Array<double> starResidual( measuresLength );
   Array<double> fwhmMeanDev( measuresLength );
   Array<double> eccentricityMeanDev( measuresLength );
   Array<double> starResidualMeanDev( measuresLength );

   for ( size_type i = 0; i < measuresLength; ++i )
   {
      weight[i] = measures[i].weight;
      fwhm[i] = measures[i].FWHM( subframeScale, scaleUnit );
      eccentricity[i] = measures[i].eccentricity;
      snrWeight[i] = measures[i].snrWeight;
      median[i] = measures[i].Median( cameraGain,
                                      TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit );
      medianMeanDev[i] = measures[i].MedianMeanDev( cameraGain,
                                                    TheSSCameraResolutionParameter->ElementData( cameraResolution ),
                                                    dataUnit );
      noise[i] = measures[i].Noise( cameraGain,
                                     TheSSCameraResolutionParameter->ElementData( cameraResolution ), dataUnit );
      noiseRatio[i] = measures[i].noiseRatio;
      stars[i] = measures[i].stars;
      starResidual[i] = measures[i].starResidual;
      fwhmMeanDev[i] = measures[i].FWHMMeanDeviation( subframeScale, scaleUnit );
      eccentricityMeanDev[i] = measures[i].eccentricityMeanDev;
      starResidualMeanDev[i] = measures[i].starResidualMeanDev;
   }

   MeasureUtils::MedianAndMeanDeviation( weight, weightMedian, weightDeviation );
   MeasureUtils::MedianAndMeanDeviation( fwhm, fwhmMedian, fwhmDeviation );
   MeasureUtils::MedianAndMeanDeviation( eccentricity, eccentricityMedian, eccentricityDeviation );
   MeasureUtils::MedianAndMeanDeviation( snrWeight, snrWeightMedian, snrWeightDeviation );
   MeasureUtils::MedianAndMeanDeviation( median, medianMedian, medianDeviation );
   MeasureUtils::MedianAndMeanDeviation( medianMeanDev, medianMeanDevMedian, medianMeanDevDeviation );
   MeasureUtils::MedianAndMeanDeviation( noise, noiseMedian, noiseDeviation );
   MeasureUtils::MedianAndMeanDeviation( noiseRatio, noiseRatioMedian, noiseRatioDeviation );
   MeasureUtils::MedianAndMeanDeviation( stars, starsMedian, starsDeviation );
   MeasureUtils::MedianAndMeanDeviation( starResidual, starResidualMedian, starResidualDeviation );
   MeasureUtils::MedianAndMeanDeviation( fwhmMeanDev, fwhmMeanDevMedian, fwhmMeanDevDeviation );
   MeasureUtils::MedianAndMeanDeviation( eccentricityMeanDev, eccentricityMeanDevMedian, eccentricityMeanDevDeviation );
   MeasureUtils::MedianAndMeanDeviation( starResidualMeanDev, starResidualMeanDevMedian, starResidualMeanDevDeviation );
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

void SubframeSelectorInstance::WriteMeasuredImage( MeasureItem* item )
{
   Console console;

   /*
    * Output directory.
    */
   String fileDir = outputDirectory.Trimmed();
   if ( fileDir.IsEmpty() )
      fileDir = File::ExtractDrive( item->path ) + File::ExtractDirectory( item->path );
   if ( fileDir.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output directory." );
   if ( !fileDir.EndsWith( '/' ) )
      fileDir.Append( '/' );

   /*
    * Output file extension, which defines the output file format.
    */
   String fileExtension = File::ExtractExtension( item->path );
   if ( fileExtension.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output file extension." );
   if ( !fileExtension.StartsWith( '.' ) )
      fileExtension.Prepend( '.' );

   /*
    * Output file name.
    */
   String fileName = File::ExtractName( item->path ).Trimmed();
   if ( !outputPrefix.IsEmpty() )
      fileName.Prepend( outputPrefix );
   if ( !outputPostfix.IsEmpty() )
      fileName.Append( outputPostfix );
   if ( fileName.IsEmpty() )
      throw Error( item->path + ": Unable to determine an output file name." );

   /*
    * Output file path.
    */
   String outputFilePath = fileDir + fileName + fileExtension;

   console.WriteLn( "<end><cbr><br>Writing output file: " + outputFilePath );


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
   FileFormat format( fileExtension, false/*read*/, true/*write*/ );

   if ( format.IsDeprecated() )
      console.WarningLn( "** Warning: Deprecated file format: " + format.Name() );

   FileFormatInstance outputFile( format );

   /*
    * ... and create a format instance (usually a disk file).
    */
   FileFormatInstance inputFile( format );
   if ( !outputFile.Create( outputFilePath ) )
      throw CaughtException();

   /*
    * Open input stream.
    */
   ImageDescriptionArray images;
   if ( !inputFile.Open( images, item->path ) )
      throw CaughtException();

   /*
    * Check for an empty subframe.
    */
   if ( images.IsEmpty() )
      throw Error( item->path + ": Empty subframe image." );

   /*
    * Subframe files can store multiple images - only the first image
    * will be used in such case, and the rest will be ignored.
    */
   if ( images.Length() > 1 )
      throw Error ( item->path + ": Has multiple images; unsupported " );

   // Create a shared image, 32-bit floating point
   Image* image = new Image( (void*)0, 0, 0 );

   // Read the image
   if ( !inputFile.ReadImage( *image ) )
      throw CaughtException();

   /*
    * Determine the output sample format: bits per sample and whether integer
    * or real samples.
    */
   outputFile.SetOptions( images[0].options );

   /*
    * File formats often use format-specific data.
    * Keep track of private data structures.
    */
   if ( format.UsesFormatSpecificData() && format.ValidateFormatSpecificData( inputFile.FormatSpecificData() ) )
      outputFile.SetFormatSpecificData( inputFile.FormatSpecificData() );

   /*
    * Set image properties.
    */
   if ( !inputFile.ReadImageProperties().IsEmpty() )
      if ( format.CanStoreImageProperties() )
      {
         outputFile.WriteImageProperties( inputFile.ReadImageProperties() );
         if ( !format.SupportsViewProperties() )
            console.WarningLn( "** Warning: The output format cannot store view properties; existing properties have been stored as BLOB data." );
      }
      else
         console.WarningLn( "** Warning: The output format cannot store image properties; existing properties will be lost." );

   /*
    * Add FITS header keywords and preserve existing ones, if possible.
    * N.B.: A COMMENT or HISTORY keyword cannot have a value; these keywords
    * have only the name and comment components.
    */
   if ( format.CanStoreKeywords() )
   {
      FITSKeywordArray keywords;
      inputFile.ReadFITSKeywords( keywords );

      keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Measured with " + PixInsightVersion::AsString() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with " + Module->ReadableVersion() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Measured with SubframeSelector process" );

      if ( !outputKeyword.IsEmpty() )
         keywords << FITSHeaderKeyword( outputKeyword,
                                        String().Format( "%.6f", item->weight ),
                                        "SubframeSelector.weight" );

      outputFile.WriteFITSKeywords( keywords );
   }
   else
   {
      console.WarningLn( "** Warning: The output format cannot store FITS header keywords - subframe weight metadata not embedded." );
   }

   /*
    * Preserve ICC profile, if possible.
    */
   ICCProfile inputProfile;
   inputFile.ReadICCProfile( inputProfile );
   if ( inputProfile.IsProfile() )
      if ( format.CanStoreICCProfiles() )
         outputFile.WriteICCProfile( inputProfile );
      else
         console.WarningLn( "** Warning: The output format cannot store color profiles - original ICC profile not embedded." );

   /*
    * Write the output image and close the output stream.
    */
   if ( !outputFile.WriteImage( image ) || !outputFile.Close() )
   {
      image->FreeData();
      throw CaughtException();
   }
   image->FreeData();
}

bool SubframeSelectorInstance::CanOutput( String &whyNot ) const {
   if ( measures.IsEmpty())
   {
      whyNot = "No measurements have been made.";
      return false;
   }

   if ( outputKeyword.IsEmpty() )
   {
      whyNot = "The specified output keyword is blank.";
      return false;
   }

   if ( !outputDirectory.IsEmpty() && !File::DirectoryExists( outputDirectory ) )
   {
      whyNot = "The specified output directory does not exist.";
      return false;
   }

   return true;
}

bool SubframeSelectorInstance::Output()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanOutput( why ) )
         throw Error( why );

      for ( Array<MeasureItem>::const_iterator i = measures.Begin(); i != measures.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ) )
            throw ("No such file exists on the local filesystem: " + i->path);


      for ( Array<MeasureItem>::iterator i = measures.Begin(); i != measures.End(); ++i )
      {
         try
         {
            if ( i->enabled )
               WriteMeasuredImage( i );
         }
         catch ( ... )
         {
            Console console;
            console.Note( "<end><cbr><br>* Applying error policy: " );

            switch ( onError )
            {
            default: // ?
            case SSOnError::Continue:
               console.NoteLn( "Continue on error." );
               continue;

            case SSOnError::Abort:
               console.NoteLn( "Abort on error." );
               throw ProcessAborted();

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

      return true;
   }
}

bool SubframeSelectorInstance::CanExecuteGlobal( String &whyNot ) const
{
   if ( routine == SSRoutine::StarDetectionPreview )
   {
      if ( subframes.IsEmpty())
      {
         whyNot = "No subframes have been specified.";
         return false;
      }

      return true;
   }
   else if ( routine == SSRoutine::MeasureSubframes )
   {
      if ( subframes.IsEmpty())
      {
         whyNot = "No subframes have been specified.";
         return false;
      }

      return true;
   }
   else if ( routine == SSRoutine::OutputSubframes )
   {
      if ( measures.IsEmpty())
      {
         whyNot = "No measurements have been made.";
         return false;
      }

      if ( outputKeyword.IsEmpty() )
      {
         whyNot = "The specified output keyword is blank.";
         return false;
      }

      if ( !outputDirectory.IsEmpty() && !File::DirectoryExists( outputDirectory ) )
      {
         whyNot = "The specified output directory does not exist.";
         return false;
      }

      return true;
   }

   whyNot = "Unknown routine.";
   return false;
}

bool SubframeSelectorInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
   {
      String why;
      if ( !CanExecuteGlobal( why ))
         throw Error( why );

      for ( subframe_list::const_iterator i = subframes.Begin(); i != subframes.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ))
            throw ("No such file exists on the local filesystem: " + i->path);

      if ( routine == SSRoutine::StarDetectionPreview )
         return TestStarDetector();

      if ( routine == SSRoutine::MeasureSubframes )
         return Measure();

      if ( routine == SSRoutine::OutputSubframes )
         return Output();

      throw Error("Unimplemented Routine");
      return false;
   }
}

void* SubframeSelectorInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSRoutineParameter )
      return &routine;

   else if ( p == TheSSSubframeEnabledParameter )
      return &subframes[tableRow].enabled;
   else if ( p == TheSSSubframePathParameter )
      return subframes[tableRow].path.Begin();

   else if ( p == TheSSSubframeScaleParameter )
      return &subframeScale;
   else if ( p == TheSSCameraGainParameter )
      return &cameraGain;
   else if ( p == TheSSCameraResolutionParameter )
      return &cameraResolution;
   else if ( p == TheSSSiteLocalMidnightParameter )
      return &siteLocalMidnight;
   else if ( p == TheSSScaleUnitParameter )
      return &scaleUnit;
   else if ( p == TheSSDataUnitParameter )
      return &dataUnit;

   else if ( p == TheSSStructureLayersParameter )
      return &structureLayers;
   else if ( p == TheSSNoiseLayersParameter )
      return &noiseLayers;
   else if ( p == TheSSHotPixelFilterRadiusParameter )
      return &hotPixelFilterRadius;
   else if ( p == TheSSApplyHotPixelFilterParameter )
      return &applyHotPixelFilterToDetectionImage;
   else if ( p == TheSSNoiseReductionFilterRadiusParameter )
      return &noiseReductionFilterRadius;
   else if ( p == TheSSSensitivityParameter )
      return &sensitivity;
   else if ( p == TheSSPeakResponseParameter )
      return &peakResponse;
   else if ( p == TheSSMaxDistortionParameter )
      return &maxDistortion;
   else if ( p == TheSSUpperLimitParameter )
      return &upperLimit;
   else if ( p == TheSSBackgroundExpansionParameter )
      return &backgroundExpansion;
   else if ( p == TheSSXYStretchParameter )
      return &xyStretch;
   else if ( p == TheSSPSFFitParameter )
      return &psfFit;
   else if ( p == TheSSPSFFitCircularParameter )
      return &psfFitCircular;
   else if ( p == TheSSPedestalParameter )
      return &pedestal;
   else if ( p == TheSSROIX0Parameter )
      return &roi.x0;
   else if ( p == TheSSROIY0Parameter )
      return &roi.y0;
   else if ( p == TheSSROIX1Parameter )
      return &roi.x1;
   else if ( p == TheSSROIY1Parameter )
      return &roi.y1;

   else if ( p == TheSSOutputDirectoryParameter )
      return outputDirectory.Begin();
   else if ( p == TheSSOutputPrefixParameter )
      return outputPrefix.Begin();
   else if ( p == TheSSOutputPostfixParameter )
      return outputPostfix.Begin();
   else if ( p == TheSSOutputKeywordParameter )
      return outputKeyword.Begin();
   else if ( p == TheSSOverwriteExistingFilesParameter )
      return &overwriteExistingFiles;
   else if ( p == TheSSOnErrorParameter )
      return &onError;

   else if ( p == TheSSApprovalExpressionParameter )
      return approvalExpression.Begin();
   else if ( p == TheSSWeightingExpressionParameter )
      return weightingExpression.Begin();

   else if ( p == TheSSSortingPropertyParameter )
      return &sortingProperty;
   else if ( p == TheSSGraphPropertyParameter )
      return &graphProperty;

   else if ( p == TheSSMeasurementIndexParameter )
      return &measures[tableRow].index;
   else if ( p == TheSSMeasurementEnabledParameter )
      return &measures[tableRow].enabled;
   else if ( p == TheSSMeasurementLockedParameter )
      return &measures[tableRow].locked;
   else if ( p == TheSSMeasurementPathParameter )
      return measures[tableRow].path.Begin();
   else if ( p == TheSSMeasurementWeightParameter )
      return &measures[tableRow].weight;
   else if ( p == TheSSMeasurementFWHMParameter )
      return &measures[tableRow].fwhm;
   else if ( p == TheSSMeasurementEccentricityParameter )
      return &measures[tableRow].eccentricity;
   else if ( p == TheSSMeasurementSNRWeightParameter )
      return &measures[tableRow].snrWeight;
   else if ( p == TheSSMeasurementMedianParameter )
      return &measures[tableRow].median;
   else if ( p == TheSSMeasurementMedianMeanDevParameter )
      return &measures[tableRow].medianMeanDev;
   else if ( p == TheSSMeasurementNoiseParameter )
      return &measures[tableRow].noise;
   else if ( p == TheSSMeasurementNoiseRatioParameter )
      return &measures[tableRow].noiseRatio;
   else if ( p == TheSSMeasurementStarsParameter )
      return &measures[tableRow].stars;
   else if ( p == TheSSMeasurementStarResidualParameter )
      return &measures[tableRow].starResidual;
   else if ( p == TheSSMeasurementFWHMMeanDevParameter )
      return &measures[tableRow].fwhmMeanDev;
   else if ( p == TheSSMeasurementEccentricityMeanDevParameter )
      return &measures[tableRow].eccentricityMeanDev;
   else if ( p == TheSSMeasurementStarResidualMeanDevParameter )
      return &measures[tableRow].starResidualMeanDev;

   return nullptr;
}

bool SubframeSelectorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSSubframesParameter )
   {
      subframes.Clear();
      if ( sizeOrLength > 0 )
         subframes.Add( SubframeItem(), sizeOrLength );
   }
   else if ( p == TheSSSubframePathParameter )
   {
      subframes[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         subframes[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputDirectoryParameter )
   {
      outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPrefixParameter )
   {
      outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputPostfixParameter )
   {
      outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheSSOutputKeywordParameter )
   {
      outputKeyword.Clear();
      if ( sizeOrLength > 0 )
         outputKeyword.SetLength( sizeOrLength );
   }
   else if ( p == TheSSApprovalExpressionParameter )
   {
      approvalExpression.Clear();
      if ( sizeOrLength > 0 )
         approvalExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSWeightingExpressionParameter )
   {
      weightingExpression.Clear();
      if ( sizeOrLength > 0 )
         weightingExpression.SetLength( sizeOrLength );
   }
   else if ( p == TheSSMeasurementsParameter )
   {
      measures.Clear();
      if ( sizeOrLength > 0 )
         for ( size_type i = 0; i < sizeOrLength; ++i )
            measures.Add( MeasureItem( i ) );
   }
   else if ( p == TheSSMeasurementPathParameter )
   {
      measures[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         measures[tableRow].path.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type SubframeSelectorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSSSubframesParameter )
      return subframes.Length();
   else if ( p == TheSSSubframePathParameter )
      return subframes[tableRow].path.Length();

   else if ( p == TheSSOutputDirectoryParameter )
      return outputDirectory.Length();
   else if ( p == TheSSOutputPrefixParameter )
      return outputPrefix.Length();
   else if ( p == TheSSOutputPostfixParameter )
      return outputPostfix.Length();
   else if ( p == TheSSOutputKeywordParameter )
      return outputKeyword.Length();

   else if ( p == TheSSApprovalExpressionParameter )
      return approvalExpression.Length();
   else if ( p == TheSSWeightingExpressionParameter )
      return weightingExpression.Length();

   else if ( p == TheSSMeasurementsParameter )
      return measures.Length();
   else if ( p == TheSSMeasurementPathParameter )
      return measures[tableRow].path.Length();

   return 0;
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.cpp - Released 2017-11-05T16:00:00Z
