//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureThread.cpp - Released 2017-08-01T14:26:58Z
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

#include "SubframeSelectorMeasureThread.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorParameters.h"
#include "SubframeSelectorStarDetector.h"

#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ElapsedTime.h>

#define MAX_STARS 20000
#define STAR_DETECTEDOK 1 // DynamicPSF Status Enumeration
#define PSF_FITTEDOK 1 // DynamicPSF Status Enumeration
#define FWHM_GAUSSIAN 1.551850 // return 2.0 * pcl::Sqrt( 2.0 * pcl::Log( 2.0 ) );
#define FWHM_MOFFAT10 0.535811 // return 2.0 * pcl::Sqrt( pcl::Pow( 2.0, 1.0 / 10.0 ) - 1.0 );
#define FWHM_MOFFAT8 0.601690 // return 2.0 * pcl::Sqrt( pcl::Pow( 2.0, 1.0 / 8.0 ) - 1.0 );
#define FWHM_MOFFAT6 0.699892 // return 2.0 * pcl::Sqrt( pcl::Pow( 2.0, 1.0 / 6.0 ) - 1.0 );
#define FWHM_MOFFAT4 0.869959 // return 2.0 * pcl::Sqrt( pcl::Pow( 2.0, 1.0 / 4.0 ) - 1.0 );
#define FWHM_MOFFAT25 1.130501 // return 2.0 * pcl::Sqrt( pcl::Pow( 2.0, 1.0 / 2.5 ) - 1.0 );
#define FWHM_MOFFAT15 1.532842 // return 2.0 * pcl::Sqrt( pcl::Pow( 2.0, 1.0 / 1.5 ) - 1.0 );
#define FWHM_LORENTZIAN 2.000000 // return 2.0 * pcl::Sqrt( pcl::Pow( 2.0, 1.0 / 1.0 ) - 1.0 );

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorMeasureThread::SubframeSelectorMeasureThread( ImageVariant& subframe,
                                                              const String& subframePath, const String& subframeWindowId,
                                                              const MeasureThreadInputData& data ) :
   m_subframe( subframe ),
   m_outputData( subframePath ),
   m_subframePath( subframePath ),
   m_subframeWindowId( subframeWindowId ),
   m_success( false ),
   m_data( data )
{
}

void SubframeSelectorMeasureThread::Run()
{
   try
   {
      m_success = false;

      Console console;
      console.NoteLn( "<end><cbr><br>Measuring: " + m_subframePath );
      Module->ProcessEvents();

      ElapsedTime T;

      // Crop if the ROI was set
      if ( m_data.roi.IsRect() ) {
         console.WriteLn( String().Format( "Cropping to: (%i, %i), (%i x %i)",
                                             m_data.roi.x0, m_data.roi.y0, m_data.roi.Width(), m_data.roi.Height() ) );
         Module->ProcessEvents();
         m_subframe.CropTo( m_data.roi );
      }

      // Setup StarDetector parameters and find the list of stars
      SubframeSelectorStarDetector starDetector;
      starDetector.showStarDetectionMaps                 = m_data.showStarDetectionMaps;
      starDetector.structureLayers                       = m_data.structureLayers;
      starDetector.noiseLayers                           = m_data.noiseLayers;
      starDetector.hotPixelFilterRadius                  = m_data.hotPixelFilterRadius;
      starDetector.noiseReductionFilterRadius            = m_data.noiseReductionFilterRadius;
      starDetector.applyHotPixelFilterToDetectionImage   = m_data.applyHotPixelFilterToDetectionImage;
      starDetector.sensitivity                           = m_data.sensitivity;
      starDetector.peakResponse                          = m_data.peakResponse;
      starDetector.maxDistortion                         = m_data.maxDistortion;
      starDetector.upperLimit                            = m_data.upperLimit;
      starDetector.backgroundExpansion                   = m_data.backgroundExpansion;
      starDetector.xyStretch                             = m_data.xyStretch;
      Array<Star> stars = starDetector.GetStars( m_subframe );
      m_subframe.Free();
      console.WriteLn( "Star Detector: " + T.ToIsoString() );
      T.Reset();

      // Stop if just showing the maps
      if ( m_data.showStarDetectionMaps ) {
         m_success = true;
         return;
      }

      // Setup a DynamicPSF process
      Process dPSFProcess( IsoString( "DynamicPSF" ) );
      ProcessInstance dPSF( dPSFProcess );
      if ( dPSF.IsNull() )
         throw Error( "Couldn't instantiate the DynamicPSF process: null" );

      // Find the PSF parameters and setup the options
      dPSF.SetParameterValue( false, ProcessParameter( dPSFProcess, "autoPSF" ), 0 );
      dPSF.SetParameterValue( false, ProcessParameter( dPSFProcess, "moffatPSF" ), 0 );
      dPSF.SetParameterValue( (bool) m_data.psfFitCircular, ProcessParameter( dPSFProcess, "circularPSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Gaussian,
                              ProcessParameter( dPSFProcess, "gaussianPSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Moffat10,
                              ProcessParameter( dPSFProcess, "moffat10PSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Moffat8,
                              ProcessParameter( dPSFProcess, "moffat8PSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Moffat6,
                              ProcessParameter( dPSFProcess, "moffat6PSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Moffat4,
                              ProcessParameter( dPSFProcess, "moffat4PSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Moffat25,
                              ProcessParameter( dPSFProcess, "moffat25PSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Moffat15,
                              ProcessParameter( dPSFProcess, "moffat15PSF" ), 0 );
      dPSF.SetParameterValue( m_data.psfFit == SSPSFFit::Lorentzian,
                              ProcessParameter( dPSFProcess, "lorentzianPSF" ), 0 );

      // Find the Views parameter and setup 1 View with this window
      ProcessParameter viewsParameter( dPSFProcess, "views" );

      if ( !dPSF.AllocateTableRows( viewsParameter, 1 ) )
         throw Error( "Cannot allocate DynamicPSF views" );

      dPSF.SetParameterValue( m_subframeWindowId, ProcessParameter( viewsParameter, "id" ), 0 );

      // Find the Stars parameter and setup N Stars with the previous results
      size_type starRows = stars.Length();
      if ( stars.Length() > MAX_STARS )
         starRows = MAX_STARS;

      ProcessParameter starsParameter( dPSFProcess, "stars" );

      if ( !dPSF.AllocateTableRows( starsParameter, starRows ) )
         throw Error( "Cannot allocate DynamicPSF views" );

      for ( size_type i = 0; i < starRows; ++i )
      {
         Star star = stars[i];
         int radius = pcl::Max(3, pcl::Ceil(pcl::Sqrt(star.size)));
         int ox = 0;
         int oy = 0;
         if ( m_data.roi.IsRect() ) {
            ox += m_data.roi.x0;
            oy += m_data.roi.y0;
         }
         dPSF.SetParameterValue( STAR_DETECTEDOK,
                                 ProcessParameter( starsParameter, "status" ), i );
         dPSF.SetParameterValue( ox + star.position.x,
                                 ProcessParameter( starsParameter, "x" ),      i );
         dPSF.SetParameterValue( ox + star.position.x - radius,
                                 ProcessParameter( starsParameter, "x0" ),     i );
         dPSF.SetParameterValue( ox + star.position.x + radius,
                                 ProcessParameter( starsParameter, "x1" ),     i );
         dPSF.SetParameterValue( oy + star.position.y,
                                 ProcessParameter( starsParameter, "y" ),      i );
         dPSF.SetParameterValue( oy + star.position.y - radius,
                                 ProcessParameter( starsParameter, "y0" ),     i );
         dPSF.SetParameterValue( oy + star.position.y + radius,
                                 ProcessParameter( starsParameter, "y1" ),     i );
      }

      // Run the DynamicPSF process if possible
      String whyNot;
      if ( !dPSF.CanExecuteGlobal( whyNot ) )
         throw Error( "Cannot execute DynamicPSF instance <br/>"
                 "Reason: " + whyNot );

      if ( !dPSF.ExecuteGlobal() )
         throw CaughtException();

      console.WriteLn( "DynamicPSF: " + T.ToIsoString() );
      T.Reset();

      // Find the output PSF parameter and get the results
      ProcessParameter psfParameter( dPSFProcess, "psf" );
      size_type psfRows = dPSF.TableRowCount( psfParameter );
      // Determine the best fit to weight the others against
      double minMAD = -1;
      for ( size_type i = 0; i < psfRows; ++i ) {
         Variant fit = dPSF.ParameterValue( ProcessParameter( psfParameter, "status" ), i );
         if ( !fit.IsValid())
            continue;
         if ( fit.ToInt() != PSF_FITTEDOK )
            continue;

         Variant MADV = dPSF.ParameterValue( ProcessParameter( psfParameter, "mad" ), i );
         if ( !MADV.IsValid())
            continue;
         double MAD = MADV.ToDouble();

         if ( minMAD <= 0 )
         {
            minMAD = MAD;
            continue;
         }
         
         if ( MAD < minMAD )
            minMAD = MAD;
      }
      // Analyze each star parameter
      size_type fits = 0;
      double sumSigma = 0;
      double sumWeight = 0;
      for ( size_type i = 0; i < psfRows; ++i ) {
         Variant fit = dPSF.ParameterValue( ProcessParameter( psfParameter, "status" ), i );
         if ( !fit.IsValid() )
            continue;
         if ( fit.ToInt() != PSF_FITTEDOK )
            continue;

         Variant sx = dPSF.ParameterValue( ProcessParameter( psfParameter, "sx" ), i );
         if ( !sx.IsValid() )
            continue;
         Variant sy = dPSF.ParameterValue( ProcessParameter( psfParameter, "sy" ), i );
         if ( !sy.IsValid() )
            continue;

         Variant MADV = dPSF.ParameterValue( ProcessParameter( psfParameter, "mad" ), i );
         if ( !MADV.IsValid())
            continue;
         double MAD = MADV.ToDouble();

         double weight = minMAD / MAD;
         sumWeight += weight;

         sumSigma += weight * pcl::Sqrt( sx.ToDouble() * sy.ToDouble() );
         ++fits;
      }

      if ( fits <= 0 )
         m_success = false;
      else {
         double FWHM = sumSigma / sumWeight;
         if ( m_data.psfFit == SSPSFFit::Gaussian )
            FWHM *= FWHM_GAUSSIAN;
         if ( m_data.psfFit == SSPSFFit::Moffat10 )
            FWHM *= FWHM_MOFFAT10;
         if ( m_data.psfFit == SSPSFFit::Moffat8 )
            FWHM *= FWHM_MOFFAT8;
         if ( m_data.psfFit == SSPSFFit::Moffat6 )
            FWHM *= FWHM_MOFFAT6;
         if ( m_data.psfFit == SSPSFFit::Moffat4 )
            FWHM *= FWHM_MOFFAT4;
         if ( m_data.psfFit == SSPSFFit::Moffat25 )
            FWHM *= FWHM_MOFFAT25;
         if ( m_data.psfFit == SSPSFFit::Moffat15 )
            FWHM *= FWHM_MOFFAT15;
         if ( m_data.psfFit == SSPSFFit::Lorentzian )
            FWHM *= FWHM_LORENTZIAN;
         m_outputData.fwhm = FWHM;
         m_success = true;
      }

      // Cleanup
      dPSF.AllocateTableRows( viewsParameter, 0 );
      dPSF.AllocateTableRows( starsParameter, 0 );
      dPSF.AllocateTableRows( psfParameter, 0 );
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

String SubframeSelectorMeasureThread::SubframePath() const
{
   return m_subframePath;
}

String SubframeSelectorMeasureThread::SubframeWindowId() const
{
   return m_subframeWindowId;
}

const MeasureData& SubframeSelectorMeasureThread::OutputData() const
{
   return m_outputData;
}

bool SubframeSelectorMeasureThread::Success() const
{
   return m_success;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasureThread.cpp - Released 2017-08-01T14:26:58Z
