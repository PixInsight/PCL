//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.cpp - Released 2017-08-01T14:26:58Z
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

#include "PSF.h"
#include "SubframeSelectorInstance.h"
#include "SubframeSelectorUtils.h"
#include "SubframeSelectorStarDetector.h"
#include "SubframeSelectorInterface.h"

#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ProcessInstance.h>

namespace pcl
{

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
   roi( 0 ),
   psfFit( SSPSFFit::Default ),
   psfFitCircular( TheSSPSFFitCircularParameter->DefaultValue() ),
   approvalExpression( "" ),
   weightingExpression( "" ),
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
      roi                                    = x->roi;
      psfFit                                 = x->psfFit;
      psfFitCircular                         = x->psfFitCircular;
      approvalExpression                     = x->approvalExpression;
      weightingExpression                    = x->weightingExpression;
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

   SubframeSelectorMeasureThread( ImageVariant* subframe,
                                  const String& subframePath, const String& subframeWindowId,
                                  MeasureThreadInputData* data ) :
           m_outputData( subframePath ),
           m_subframeWindowId( subframeWindowId ),
           m_success( false ),
           m_data( data )
   {
      subframe->GetIntensity( m_subframe );
   }

   virtual void Run()
   {

      try
      {
         m_success = false;

         Console console;
         console.NoteLn( "<end><cbr><br>Measuring: " + m_outputData.path );

         // Crop if the ROI was set
         if ( m_data->instance->roi.IsRect() )
         {
            console.WriteLn( String().Format( "Cropping to: (%i, %i), (%i x %i)",
                                              m_data->instance->roi.x0, m_data->instance->roi.y0,
                                              m_data->instance->roi.Width(), m_data->instance->roi.Height()));
            Module->ProcessEvents();
            m_subframe.CropTo( m_data->instance->roi );
         }

         // Run the Star Detector
         ElapsedTime T;
         star_list stars = StarDetector();
         console.WriteLn( String().Format( "%i Star(s) detected", stars.Length() ) );
         console.WriteLn( "Star Detector: " + T.ToIsoString() );

         // Stop if just showing the maps
         if ( m_data->showStarDetectionMaps )
         {
            m_subframe.Free(); // no longer required
            m_success = true;
            return;
         }

         // Stop if no stars found
         if ( stars.IsEmpty() )
         {
            m_subframe.Free(); // no longer required
            m_success = false;
            return;
         }

         // Run the PSF Fitter
         T.Reset();
         psf_list fits = FitPSFs( stars.Begin(), stars.End() );
         console.WriteLn( String().Format( "%i PSF(s) fitted", fits.Length() ) );
         console.WriteLn( "Fit PSFs: " + T.ToIsoString() );

         if ( fits.IsEmpty() )
         {
            m_subframe.Free(); // no longer required
            m_success = false;
            return;
         }

         // Measure Data
         T.Reset();
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
         // Analyze each star parameter
         double sumSigma = 0;
         double sumWeight = 0;
         for ( psf_list::const_iterator i = fits.Begin(); i != fits.End(); ++i )
         {
            double MAD = i->mad;

            double weight = minMAD / MAD;
            sumWeight += weight;

            sumSigma += weight * pcl::Sqrt( i->sx * i->sy );
         }
         double FWHM = sumSigma / sumWeight;
         m_outputData.fwhm = PSFData::FWHM( PSFFunction( m_data->instance->psfFit ), FWHM, 0 ); // beta is unused here

         console.WriteLn( "Calculations: " + T.ToIsoString() );

         m_subframe.Free(); // no longer required

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

   String SubframeWindowId() const
   {
      return m_subframeWindowId;
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

         PSFFit psfFit( m_subframe, i->position, rect, PSFFunction( m_data->instance->psfFit ),
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

   ImageVariant               m_subframe;
   MeasureData                m_outputData;
   String                     m_subframeWindowId;
   bool                       m_success : 1;

   MeasureThreadInputData* m_data;
};

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

         thread_list threads = CreateThreadForSubframe( item.path, &inputThreadData );
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
                     MeasureItem m( measures.Length() + 1 );
                     m.Input( (*i)->OutputData() );
                     measures.Append( m );

                     // Close open image
                     View::ViewById( (*i)->SubframeWindowId() ).Window().Close();

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
                  thread_list threads = CreateThreadForSubframe( item.path, &inputThreadData );

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

      if ( TheSubframeSelectorInterface != nullptr )
      {
         TheSubframeSelectorInterface->ClearMeasurements();
         for ( size_type i = 0; i < measures.Length(); ++i )
            TheSubframeSelectorInterface->AddMeasurement( measures[i] );
         TheSubframeSelectorInterface->UpdateMeasurementImagesList();
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
      for ( size_type i = 0; i < measures.Length(); ++i )
         if ( !measures[i].locked )
            measures[i].enabled = true;
   }
   else
   {
      for ( size_type i = 0; i < measures.Length(); ++i )
      {
         if ( measures[i].locked )
            continue;

         String JSEvaluator = measures[i].JavaScriptParameters( subframeScale, scaleUnit );
         JSEvaluator += approvalExpression;
         Console().NoteLn( "\nScript:" );
         Console().WriteLn( JSEvaluator.EncodedHTMLSpecialChars() );

         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Approval Error: Invalid script execution" );
         String resultText = result.ToString();
         Console().NoteLn( "\nResult:" + resultText );

         measures[i].enabled = result.ToBool();
      }
   }
}

void SubframeSelectorInstance::WeightMeasurements()
{
   if ( weightingExpression.IsEmpty() )
   {
      for ( size_type i = 0; i < measures.Length(); ++i )
         measures[i].weight = 0;
   }
   else
   {
      for ( size_type i = 0; i < measures.Length(); ++i )
      {
         String JSEvaluator = measures[i].JavaScriptParameters( subframeScale, scaleUnit );
         JSEvaluator += weightingExpression;
         Console().NoteLn( "\nScript:" );
         Console().WriteLn( JSEvaluator.EncodedHTMLSpecialChars() );

         Variant result = Module->EvaluateScript( JSEvaluator.DecodedHTMLSpecialChars(), "JavaScript" );
         if ( !result.IsValid() )
            throw Error( "Approval Error: Invalid script execution" );
         String resultText = result.ToString();
         Console().NoteLn( "\nResult:" + resultText );

         measures[i].weight = result.ToFloat();
      }
   }
}

bool SubframeSelectorInstance::CanExecuteGlobal( String &whyNot ) const {
   if ( subframes.IsEmpty())
   {
       whyNot = "No subframes have been specified.";
       return false;
   }

   if ( routine == SSRoutine::StarDetectionPreview )
      return true;
   else if ( routine == SSRoutine::MeasureSubframes )
      return true;
   else if ( routine == SSRoutine::OutputSubframes )
   {
      if ( measures.IsEmpty())
      {
         whyNot = "No measurements have been made.";
         return false;
      }
   }

   whyNot = "Unknown routine.";
   return false;
}

bool SubframeSelectorInstance::ExecuteGlobal() {
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
         throw Error("Unimplemented Routine: Output Subframes");

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
   else if ( p == TheSSROIX0Parameter )
      return &roi.x0;
   else if ( p == TheSSROIY0Parameter )
      return &roi.y0;
   else if ( p == TheSSROIX1Parameter )
      return &roi.x1;
   else if ( p == TheSSROIY1Parameter )
      return &roi.y1;
   else if ( p == TheSSPSFFitParameter )
      return &psfFit;
   else if ( p == TheSSPSFFitCircularParameter )
      return &psfFitCircular;

   else if ( p == TheSSApprovalExpressionParameter )
      return approvalExpression.Begin();
   else if ( p == TheSSWeightingExpressionParameter )
      return weightingExpression.Begin();

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


/*
 * Read a subframe file. Returns a list of measuring threads ready to
 * measure all subimages loaded from the file.
 */
thread_list
SubframeSelectorInstance::CreateThreadForSubframe( const String& filePath, MeasureThreadInputData* threadData )
{
   thread_list threads;
   Array<ImageWindow> imageWindows = ImageWindow::Open( filePath );
   if ( imageWindows.Length() != 1 )
      throw Error( String().Format( "Image Window incorrect length: 1 != %i : %s", imageWindows.Length(), filePath ) );
   ImageWindow subframeWindow = imageWindows[0];
   ImageVariant subframe = subframeWindow.MainView().Image();
   threads.Add( new SubframeSelectorMeasureThread( &subframe,
                                                   filePath,
                                                   subframeWindow.MainView().FullId(),
                                                   threadData ));
   return threads;
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.cpp - Released 2017-08-01T14:26:58Z
