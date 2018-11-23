//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0326
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "PhotometricColorCalibrationGraphInterface.h"
#include "PhotometricColorCalibrationInstance.h"
#include "PhotometricColorCalibrationParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/MetaModule.h>
#include <pcl/StdStatus.h>
#include <pcl/Version.h>
#include <pcl/View.h>
#include <pcl/XISF.h>

/*
 * Working options for the Photometry script.
 */
#define STARFLAG_MULTIPLE     0x01
#define STARFLAG_OVERLAPPED   0x02
#define STARFLAG_BADPOS       0x04
#define STARFLAG_LOWSNR       0x08
#define STARFLAG_SATURATED    0x10

#define BKGWINDOW_RING        0
#define BKGWINDOW_PHOTOMETRIC 1

#define BKGMODEL_SOURCE       0
#define BKGMODEL_MMT          1
#define BKGMODEL_ABE          2

namespace pcl
{

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInstance::PhotometricColorCalibrationInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_whiteReferenceId( ThePCCWhiteReferenceIdParameter->DefaultValue() ),
   p_whiteReferenceName( ThePCCWhiteReferenceNameParameter->DefaultValue() ),
   p_whiteReferenceSr_JV( ThePCCWhiteReferenceSr_JVParameter->DefaultValue() ),
   p_whiteReferenceJB_JV( ThePCCWhiteReferenceJB_JVParameter->DefaultValue() ),
   p_zeroPointSr_JV( ThePCCZeroPointSr_JVParameter->DefaultValue() ),
   p_zeroPointJB_JV( ThePCCZeroPointJB_JVParameter->DefaultValue() ),
   p_focalLength( ThePCCFocalLengthParameter->DefaultValue() ),
   p_pixelSize( ThePCCPixelSizeParameter->DefaultValue() ),
   p_centerRA( ThePCCCenterRAParameter->DefaultValue() ),
   p_centerDec( ThePCCCenterDecParameter->DefaultValue() ),
   p_epochJD( ThePCCEpochJDParameter->DefaultValue() ),
   p_forcePlateSolve( ThePCCForcePlateSolveParameter->DefaultValue() ),
   p_ignoreImagePositionAndScale( ThePCCIgnoreImagePositionAndScaleParameter->DefaultValue() ),
   p_serverURL( ThePCCServerURLParameter->DefaultValue() ),
   p_solverCatalogName( ThePCCSolverCatalogNameParameter->DefaultValue() ),
   p_solverAutoCatalog( ThePCCSolverAutoCatalogParameter->DefaultValue() ),
   p_solverLimitMagnitude( ThePCCSolverLimitMagnitudeParameter->DefaultValue() ),
   p_solverAutoLimitMagnitude( ThePCCSolverAutoLimitMagnitudeParameter->DefaultValue() ),
   p_solverAutoLimitMagnitudeFactor( ThePCCSolverAutoLimitMagnitudeFactorParameter->DefaultValue() ),
   p_solverStarSensitivity( ThePCCSolverStarSensitivityParameter->DefaultValue() ),
   p_solverNoiseLayers( ThePCCSolverNoiseLayersParameter->DefaultValue() ),
   p_solverAlignmentDevice( PCCSolverAlignmentDevice::Default ),
   p_solverDistortionCorrection( ThePCCSolverDistortionCorrectionParameter->DefaultValue() ),
   p_solverSplineSmoothing( ThePCCSolverSplineSmoothingParameter->DefaultValue() ),
   p_solverProjection( PCCSolverProjection::Default ),
   p_photCatalogName( ThePCCPhotCatalogNameParameter->DefaultValue() ),
   p_photLimitMagnitude( ThePCCPhotLimitMagnitudeParameter->DefaultValue() ),
   p_photAutoLimitMagnitude( ThePCCPhotAutoLimitMagnitudeParameter->DefaultValue() ),
   p_photAutoLimitMagnitudeFactor( ThePCCPhotAutoLimitMagnitudeFactorParameter->DefaultValue() ),
   p_photAutoAperture( ThePCCPhotAutoApertureParameter->DefaultValue() ),
   p_photAperture( ThePCCPhotApertureParameter->DefaultValue() ),
   p_photUsePSF( ThePCCPhotUsePSFParameter->DefaultValue() ),
   p_photSaturationThreshold( ThePCCPhotSaturationThresholdParameter->DefaultValue() ),
   p_photShowDetectedStars( ThePCCPhotShowDetectedStarsParameter->DefaultValue() ),
   p_photShowBackgroundModels( ThePCCPhotShowBackgroundModelsParameter->DefaultValue() ),
   p_photGenerateGraphs( ThePCCPhotGenerateGraphsParameter->DefaultValue() ),
   p_applyCalibration( ThePCCApplyCalibrationParameter->DefaultValue() ),
   p_neutralizeBackground( ThePCCNeutralizeBackgroundParameter->DefaultValue() ),
   p_backgroundReferenceViewId( ThePCCBackgroundReferenceViewIdParameter->DefaultValue() ),
   p_backgroundLow( ThePCCBackgroundLowParameter->DefaultValue() ),
   p_backgroundHigh( ThePCCBackgroundHighParameter->DefaultValue() ),
   p_backgroundUseROI( ThePCCBackgroundUseROIParameter->DefaultValue() ),
   p_backgroundROI( 0 )
{
}

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInstance::PhotometricColorCalibrationInstance( const PhotometricColorCalibrationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const PhotometricColorCalibrationInstance* x = dynamic_cast<const PhotometricColorCalibrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_whiteReferenceId               = x->p_whiteReferenceId;
      p_whiteReferenceName             = x->p_whiteReferenceName;
      p_whiteReferenceSr_JV            = x->p_whiteReferenceSr_JV;
      p_whiteReferenceJB_JV            = x->p_whiteReferenceJB_JV;
      p_zeroPointSr_JV                 = x->p_zeroPointSr_JV;
      p_zeroPointJB_JV                 = x->p_zeroPointJB_JV;
      p_focalLength                    = x->p_focalLength;
      p_pixelSize                      = x->p_pixelSize;
      p_centerRA                       = x->p_centerRA;
      p_centerDec                      = x->p_centerDec;
      p_epochJD                        = x->p_epochJD;
      p_forcePlateSolve                = x->p_forcePlateSolve;
      p_ignoreImagePositionAndScale    = x->p_ignoreImagePositionAndScale;
      p_serverURL                      = x->p_serverURL;
      p_solverCatalogName              = x->p_solverCatalogName;
      p_solverAutoCatalog              = x->p_solverAutoCatalog;
      p_solverLimitMagnitude           = x->p_solverLimitMagnitude;
      p_solverAutoLimitMagnitude       = x->p_solverAutoLimitMagnitude;
      p_solverAutoLimitMagnitudeFactor = x->p_solverAutoLimitMagnitudeFactor;
      p_solverStarSensitivity          = x->p_solverStarSensitivity;
      p_solverNoiseLayers              = x->p_solverNoiseLayers;
      p_solverAlignmentDevice          = x->p_solverAlignmentDevice;
      p_solverDistortionCorrection     = x->p_solverDistortionCorrection;
      p_solverSplineSmoothing          = x->p_solverSplineSmoothing;
      p_solverProjection               = x->p_solverProjection;
      p_photCatalogName                = x->p_photCatalogName;
      p_photLimitMagnitude             = x->p_photLimitMagnitude;
      p_photAutoLimitMagnitude         = x->p_photAutoLimitMagnitude;
      p_photAutoLimitMagnitudeFactor   = x->p_photAutoLimitMagnitudeFactor;
      p_photAutoAperture               = x->p_photAutoAperture;
      p_photAperture                   = x->p_photAperture;
      p_photUsePSF                     = x->p_photUsePSF;
      p_photSaturationThreshold        = x->p_photSaturationThreshold;
      p_photShowDetectedStars          = x->p_photShowDetectedStars;
      p_photShowBackgroundModels       = x->p_photShowBackgroundModels;
      p_photGenerateGraphs             = x->p_photGenerateGraphs;
      p_applyCalibration               = x->p_applyCalibration;
      p_neutralizeBackground           = x->p_neutralizeBackground;
      p_backgroundReferenceViewId      = x->p_backgroundReferenceViewId;
      p_backgroundLow                  = x->p_backgroundLow;
      p_backgroundHigh                 = x->p_backgroundHigh;
      p_backgroundUseROI               = x->p_backgroundUseROI;
      p_backgroundROI                  = x->p_backgroundROI;
   }
}

// ----------------------------------------------------------------------------

UndoFlags PhotometricColorCalibrationInstance::UndoMode( const View& ) const
{
   return p_applyCalibration ? UndoFlag::Keywords
                             | UndoFlag::PixelData
                             | UndoFlag::AstrometricSolution : UndoFlag::Keywords;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.IsPreview() )
   {
      whyNot = "PhotometricColorCalibration cannot be executed on previews.";
      return false;
   }
   if ( !view.IsColor() )
   {
      whyNot = "PhotometricColorCalibration can only be executed on color images.";
      return false;
   }
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "PhotometricColorCalibration cannot be executed on complex images.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * POD structures to hold catalog and image star fluxes.
 */
struct RG
{
   double catR, catG, imgR, imgG;
};
struct BG
{
   double catB, catG, imgB, imgG;
};

/*
 * Writes a working image with the specified keywords and image properties. Can
 * write a single channel or the whole image is channel < 0.
 */
static void SaveImage( const String& path, const ImageVariant& image,
                       const FITSKeywordArray& keywords, const PropertyArray& properties, int channel = -1 )
{
   XISFWriter xisf;
   xisf.Create( path, 1/*count*/ );
   if ( !keywords.IsEmpty() )
      xisf.WriteFITSKeywords( keywords );
   if ( !properties.IsEmpty() )
      xisf.WriteImageProperties( properties );
   image.ResetSelections();
   if ( channel >= 0 )
      image.SelectChannel( channel );
   xisf.WriteImage( image );
   xisf.Close();
   image.ResetSelections();
}

/*
 * Retrieve the value of a numeric FITS keyword from a given keyword list.
 */
static bool GetNumericKeywordValue( double& value, const FITSKeywordArray& keywords, const IsoString& keyName )
{
   for ( const FITSHeaderKeyword& keyword : keywords )
      if ( !keyword.name.CompareIC( keyName ) )
         if ( keyword.IsNumeric() )
            if ( keyword.GetNumericValue( value ) )
               return true;
   return false;
}

/*
 * Fit two vectors to a straight line with robust outlier rejection.
 */
static LinearFit FitVectors( const Vector& x, const Vector& y )
{
   double mx = x.Median();
   double my = y.Median();
   double sx = 2.2219*x.Qn();
   double sy = 2.2219*y.Qn();
   Array<double> x1, y1;
   for ( int i = 0; i < x.Length(); ++i )
      if ( Abs( x[i] - mx ) < 3*sx )
         if ( Abs( y[i] - my ) < 3*sy )
         {
            x1 << x[i];
            y1 << y[i];
         }
   if ( x1.Length() < 5 )
      throw Error( "Insufficient photometric data: Got " + String( x1.Length() ) + " sample(s); at least 5 are required." );
   return LinearFit( x1, y1 );
}

// ----------------------------------------------------------------------------

template <class P>
static DVector BackgroundReference( const GenericImage<P>& image, double low, double high )
{
   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Evaluating background reference", image.NumberOfSelectedPixels() );

   Array<double> B0R, B0G, B0B;
   for ( typename GenericImage<P>::const_roi_pixel_iterator i( image ); i; ++i, ++monitor )
   {
      double R; P::FromSample( R, i[0] );
      if ( R > low && R < high )
      {
         double G; P::FromSample( G, i[1] );
         if ( G > low && G < high )
         {
            double B; P::FromSample( B, i[2] );
            if ( B > low && B < high )
            {
               B0R << R;
               B0G << G;
               B0B << B;
            }
         }
      }
   }

   DVector B0( 0.0, 3 );
   if ( !B0R.IsEmpty() )
   {
      B0[0] = pcl::Median( B0R.Begin(), B0R.End() );
      B0[1] = pcl::Median( B0G.Begin(), B0G.End() );
      B0[2] = pcl::Median( B0B.Begin(), B0B.End() );
   }
   return B0;
}

static DVector BackgroundReference( const ImageVariant& image, double low, double high )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return BackgroundReference( static_cast<const Image&>( *image ), low, high );
      case 64: return BackgroundReference( static_cast<const DImage&>( *image ), low, high );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return BackgroundReference( static_cast<const UInt8Image&>( *image ), low, high );
      case 16: return BackgroundReference( static_cast<const UInt16Image&>( *image ), low, high );
      case 32: return BackgroundReference( static_cast<const UInt32Image&>( *image ), low, high );
      }

   return 0;
}

// ----------------------------------------------------------------------------

template <class P>
static void NeutralizeBackground( GenericImage<P>& image, const DVector& B0 )
{
   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Applying background neutralization", image.NumberOfPixels() );

   for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i, ++monitor )
      for ( int c = 0; c < 3; ++c )
      {
         double v; P::FromSample( v, i[c] );
         i[c] = P::ToSample( v - B0[c] );
      }

   image.Normalize();
}

static void NeutralizeBackground( ImageVariant& image, const DVector& B0 )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: NeutralizeBackground( static_cast<Image&>( *image ), B0 ); break;
      case 64: NeutralizeBackground( static_cast<DImage&>( *image ), B0 ); break;
      }
   else
   {
      ImageVariant tmp;
      tmp.CreateFloatImage( (image.BitsPerSample() > 16) ? 64 : 32 );
      tmp.CopyImage( image );
      NeutralizeBackground( tmp, B0 );
      image.CopyImage( tmp );
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void ApplyWhiteBalance( GenericImage<P>& image, const DVector& W )
{
   StandardStatus status;
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Applying white balance", image.NumberOfPixels() );

   for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i, ++monitor )
      for ( int c = 0; c < 3; ++c )
      {
         double v; P::FromSample( v, i[c] );
         i[c] = P::ToSample( v * W[c] );
      }
}

static void ApplyWhiteBalance( ImageVariant& image, const DVector& W )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: ApplyWhiteBalance( static_cast<Image&>( *image ), W ); break;
      case 64: ApplyWhiteBalance( static_cast<DImage&>( *image ), W ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: ApplyWhiteBalance( static_cast<UInt8Image&>( *image ), W ); break;
      case 16: ApplyWhiteBalance( static_cast<UInt16Image&>( *image ), W ); break;
      case 32: ApplyWhiteBalance( static_cast<UInt32Image&>( *image ), W ); break;
      }
}

// ----------------------------------------------------------------------------

struct FromKeyword
{
   IsoString name;
   double    value;

   FromKeyword( double initialValue ) : value( initialValue )
   {
   }

   FromKeyword& operator =( const FITSHeaderKeyword& keyword )
   {
      IsoString text = keyword.StripValueDelimiters();
      if ( keyword.name == "DATE-OBS" )
      {
         int year, month, day;
         double dayf, tz;
         if ( !text.TryParseISO8601DateTime( year, month, day, dayf, tz ) )
            return *this;
         value = ComplexTimeToJD( year, month, day, dayf - tz/24 );
      }
      else
      {
         if ( !text.TryToDouble( value ) )
            return *this;
         if ( keyword.name == "OBJCTRA" )
            value *= 15;
      }

      name = keyword.name;
      return *this;
   }

   operator bool() const
   {
      return !name.IsEmpty();
   }
};

bool PhotometricColorCalibrationInstance::ExecuteOn( View& view )
{
   {
      String whyNot;
      if ( !CanExecuteOn( view, whyNot ) )
         throw Error( whyNot );
   }

   Console console;
   console.EnableAbort();

   String coreSrcDir = PixInsightSettings::GlobalString( "Application/SrcDirectory" );
   String tmpDir = File::SystemTempDirectory();

   String TPath = File::UniqueFileName( tmpDir, 12, "PCC_T_", ".xisf" );
   String RPath = File::UniqueFileName( tmpDir, 12, "PCC_R_", ".xisf" );
   String GPath = File::UniqueFileName( tmpDir, 12, "PCC_G_", ".xisf" );
   String BPath = File::UniqueFileName( tmpDir, 12, "PCC_B_", ".xisf" );

   try
   {
      FITSKeywordArray keywords;
      PropertyArray properties;

      /*
       * Perform plate solve.
       */
      {
         static SortedIsoStringList s_acquisitionKeywords;
         if ( s_acquisitionKeywords.IsEmpty() )
            s_acquisitionKeywords << "FOCALLEN"
                                  << "XPIXSZ"
                                  << "YPIXSZ"
                                  << "OBJCTRA"
                                  << "OBJCTDEC"
                                  << "DATE-OBS";

         static SortedIsoStringList s_requiredKeywords;
         if ( s_requiredKeywords.IsEmpty() )
            s_requiredKeywords    << "CTYPE1"
                                  << "CTYPE2"
                                  << "CRPIX1"
                                  << "CRPIX2"
                                  << "CRVAL1"
                                  << "CRVAL2"
                                  << "CD1_1"
                                  << "CD1_2"
                                  << "CD2_1"
                                  << "CD2_2"
                                  << "CDELT1"
                                  << "CDELT2"
                                  << "CROTA1"
                                  << "CROTA2";

         static SortedIsoStringList s_optionalKeywords;
         if ( s_optionalKeywords.IsEmpty() )
            s_optionalKeywords    << "REFSPLINE"; // ImageSolver script

         FITSKeywordArray inputKeywords = view.Window().Keywords();
         PropertyArray inputProperties = view.GetStorableProperties();

         bool doPlateSolve;
         if ( p_forcePlateSolve )
         {
            FITSKeywordArray newKeywords;
            for ( const FITSHeaderKeyword& keyword : inputKeywords )
               if ( !s_requiredKeywords.Contains( keyword.name ) )
                  if ( !s_optionalKeywords.Contains( keyword.name ) )
                     if ( !p_ignoreImagePositionAndScale || !s_acquisitionKeywords.Contains( keyword.name ) )
                        newKeywords << keyword;
            if ( newKeywords.Length() < inputKeywords.Length() )
               inputKeywords = newKeywords;

            // Delete a custom property generated by the ImageSolver script.
            inputProperties.Remove( Property( "Transformation_ImageToProjection" ) );

            doPlateSolve = true;
         }
         else
         {
            SortedIsoStringList found;
            for ( const FITSHeaderKeyword& keyword : inputKeywords )
               if ( s_requiredKeywords.Contains( keyword.name ) )
                  found << keyword.name;

            doPlateSolve = found != s_requiredKeywords;
         }

         if ( doPlateSolve )
         {
            FromKeyword focalLength( p_focalLength );
            FromKeyword pixelSize( p_pixelSize );
            FromKeyword centerRA( p_centerRA );
            FromKeyword centerDec( p_centerDec );
            FromKeyword epochJD( p_epochJD );

            for ( const FITSHeaderKeyword& keyword : inputKeywords )
               if ( keyword.name == "FOCALLEN" )
                  focalLength = keyword;
               else if ( keyword.name == "XPIXSZ" )
                  pixelSize = keyword;
               else if ( keyword.name == "OBJCTRA" )
                  centerRA = keyword;
               else if ( keyword.name == "OBJCTDEC" )
                  centerDec = keyword;
               else if ( keyword.name == "DATE-OBS" )
                  epochJD = keyword;

            if ( !focalLength )
               inputKeywords << FITSHeaderKeyword( "FOCALLEN", IsoString( p_focalLength ) );
            if ( !pixelSize )
               inputKeywords << FITSHeaderKeyword( "XPIXSZ", IsoString( p_pixelSize ) )
                             << FITSHeaderKeyword( "YPIXSZ", IsoString( p_pixelSize ) );
            if ( !centerRA )
               inputKeywords << FITSHeaderKeyword( "OBJCTRA",
                     '\'' +
                     IsoString::ToSexagesimal( p_centerRA/15,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/, 0/*width*/, ' '/*separator*/ ) ) +
                     '\'' );
            if ( !centerDec )
               inputKeywords << FITSHeaderKeyword( "OBJCTDEC",
                     '\'' +
                     IsoString::ToSexagesimal( p_centerDec,
                           SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/, 0/*width*/, ' '/*separator*/ ) ) +
                     '\'' );
            if ( !epochJD )
               inputKeywords << FITSHeaderKeyword( "DATE-OBS",
                     '\'' +
                     TimePoint( p_epochJD ).ToIsoString( ISO8601ConversionOptions( 3/*timeItems*/, 0/*precision*/, false/*timeZone*/ ) ) +
                     '\'' );

            int limitMagnitude = p_solverLimitMagnitude;
            String catalogName = p_solverCatalogName;
            if ( p_solverAutoLimitMagnitude || p_solverAutoCatalog )
            {
               Rect r = view.Image().Bounds();
               double h = Min( r.Width(), r.Height() )*pixelSize.value/1000;
               double fov = Deg( 2*ArcTan( h, focalLength.value ) );
               // Empiric formula for 1000 stars at 20 deg of galactic latitude
               int m = Range( RoundInt( p_solverAutoLimitMagnitudeFactor*Pow( fov, -0.179 ) ), 7, 20 );
               if ( p_solverAutoLimitMagnitude )
               {
                  limitMagnitude = m;
                  console.NoteLn( "<end><cbr><br>* Using an automatically calculated limit magnitude of " + String( limitMagnitude ) + '.' );
               }
               if ( p_solverAutoCatalog )
                  if ( m == 7 )
                     catalogName = "Bright Stars";
                  else if ( fov > 3 )
                     catalogName = "TYCHO-2";
                  else
                     catalogName = "Gaia";
            }

            SaveImage( TPath, view.Image(), inputKeywords, inputProperties );

            String scriptPath = coreSrcDir + "/scripts/AdP/ImageSolver.js";
            StringKeyValueList arguments = StringKeyValueList()
                                             << StringKeyValue( "metadata_focal", String( focalLength.value ) )
                                             << StringKeyValue( "metadata_xpixsz", String( pixelSize.value ) )
                                             << StringKeyValue( "metadata_ra", String( centerRA.value ) )
                                             << StringKeyValue( "metadata_dec", String( centerDec.value ) )
                                             << StringKeyValue( "metadata_epoch", String( epochJD.value ) )
                                             << StringKeyValue( "solver_vizierServer", p_serverURL )
                                             << StringKeyValue( "solver_catalog", catalogName )
                                             << StringKeyValue( "solver_magnitude", String( limitMagnitude ) )
                                             << StringKeyValue( "solver_sensitivity", String( p_solverStarSensitivity ) )
                                             << StringKeyValue( "solver_noiseLayers", String( p_solverNoiseLayers ) )
                                             << StringKeyValue( "solver_alignAlgorithm", String( p_solverAlignmentDevice ) )
                                             << StringKeyValue( "solver_distortionCorrection", String( bool( p_solverDistortionCorrection ) ) )
                                             << StringKeyValue( "solver_splineSmoothing", String( p_solverSplineSmoothing ) )
                                             << StringKeyValue( "solver_projection", String( p_solverProjection ) )
                                             << StringKeyValue( "solver_catalogMode", "1" )
                                             << StringKeyValue( "solver_autoMagnitude", "false" )
                                             << StringKeyValue( "solver_showStars", "false" )
                                             << StringKeyValue( "solver_showDistortion", "false" )
                                             << StringKeyValue( "solver_generateErrorImg", "false" )
                                             << StringKeyValue( "solver_generateDistortModel", "false" )
                                             << StringKeyValue( "solver_useDistortionModel", "false" )
                                             << StringKeyValue( "solver_onlyOptimize", "false" )
                                             << StringKeyValue( "solver_outSuffix", String() )
                                             << StringKeyValue( "solver_projectionOriginMode", "0" )
                                             << StringKeyValue( "solver_useActive", "false" )
                                             << StringKeyValue( "solver_files", TPath )
                                             << StringKeyValue( "non_interactive", "true" );

            console.ExecuteScriptGlobal( scriptPath, arguments );

            Variant result = Module->EvaluateScript( "__PJSR_AdpImageSolver_SuccessCount" );
            if ( !result.IsValid() )
               throw Error( "Internal error: Invalid script execution: " + scriptPath );
            int successCount = result.ToInt();
            if ( successCount != 1 )
               throw Error( "Failure to plate solve image: " + view.Id() );

            XISFReader xisf;
            xisf.Open( TPath );
            keywords = xisf.ReadFITSKeywords();
            properties = xisf.ReadImageProperties();
            Image image;
            xisf.ReadImage( image );
            xisf.Close();
            ImageVariant v( &image );
            SaveImage( RPath, v, keywords, properties, 0 );
            SaveImage( GPath, v, keywords, properties, 1 );
            SaveImage( BPath, v, keywords, properties, 2 );

            File::Remove( TPath );
         }
         else
         {
            keywords = inputKeywords;
            properties = inputProperties;
            ImageVariant v = view.Image();
            SaveImage( RPath, v, keywords, properties, 0 );
            SaveImage( GPath, v, keywords, properties, 1 );
            SaveImage( BPath, v, keywords, properties, 2 );
         }
      }

      Module->ProcessEvents();

      int minAperture = p_photAperture;
      int limitMagnitude = p_photLimitMagnitude;
      if ( p_photAutoAperture || p_photAutoLimitMagnitude )
      {
         double focalLength, pixelSize;
         if ( !GetNumericKeywordValue( focalLength, keywords, "FOCALLEN" ) ||
              !GetNumericKeywordValue( pixelSize, keywords, "XPIXSZ" ) )
            throw Error( "Unable to acquire image scale metadata, which is required to calculate an automatic photometric aperture." );

         if ( p_photAutoAperture )
         {
            // Be coherent with APASS' 15" aperture.
            minAperture = RoundInt( 15/(3.6 * Deg( 2*ArcTan( pixelSize, 2*focalLength ) )) );
            if ( minAperture < 5 )
            {
               console.WarningLn( "<end><cbr><br>** Warning: Forcing a photometric aperture of 5 pixels." );
               minAperture = 5;
            }
            else
               console.NoteLn( "<end><cbr><br>* Using an automatically calculated photometric aperture of " + String( minAperture ) + " pixels." );
         }

         if ( p_photAutoLimitMagnitude )
         {
            Rect r = view.Image().Bounds();
            double h = Min( r.Width(), r.Height() )*pixelSize/1000;
            double fov = Deg( 2*ArcTan( h, focalLength ) );
            // Empiric formula for 250 stars with APASS at 20 deg of galactic latitude
            limitMagnitude = Range( RoundInt( p_photAutoLimitMagnitudeFactor*Pow( fov, -0.177 ) ), 9, 17 );
            console.NoteLn( "<end><cbr><br>* Using an automatically calculated limit magnitude of " + String( limitMagnitude ) + '.' );
         }
      }

      String fluxFilePath = tmpDir +
         (p_photUsePSF ? "/PCC_PSF_Flux.csv" : "/PCC_Flux_Ap" + String( minAperture ) + ".csv");
      if ( File::Exists( fluxFilePath ) )
         File::Remove( fluxFilePath );

      /*
       * Perform photometry.
       */
      {
         String scriptPath = coreSrcDir + "/scripts/AdP/AperturePhotometry.js";
         StringKeyValueList arguments = StringKeyValueList()
                                          << StringKeyValue( "phot_apertureShape", "1" ) // circular aperture
                                          << StringKeyValue( "phot_minimumAperture", String( minAperture ) )
                                          << StringKeyValue( "phot_apertureSteps", "1" )
                                          << StringKeyValue( "phot_apertureStepSize", "1" )
                                          << StringKeyValue( "phot_bkgWindowMode", String( BKGWINDOW_PHOTOMETRIC ) )
                                          << StringKeyValue( "phot_bkgModel", String( BKGMODEL_MMT ) )
                                          << StringKeyValue( "phot_backgroundSigmaLow", "5" )
                                          << StringKeyValue( "phot_backgroundSigmaHigh", "2.7" )
                                          << StringKeyValue( "phot_bkgAperture1", "45" )
                                          << StringKeyValue( "phot_bkgAperture2", "60" )
                                          << StringKeyValue( "phot_bkgMMTlayers", "8" )
                                          << StringKeyValue( "phot_minSNR", "4" )
                                          << StringKeyValue( "phot_showFoundStars", String( bool( p_photShowDetectedStars ) ) )
                                          << StringKeyValue( "phot_maxMagnitude", String( limitMagnitude ) )
                                          << StringKeyValue( "phot_defaultFocal", String( p_focalLength ) )
                                          << StringKeyValue( "phot_gain", "1" )
                                          << StringKeyValue( "phot_saturationThreshold", String( p_photSaturationThreshold ) )
                                          << StringKeyValue( "phot_manualFilter", "true" )
                                          << StringKeyValue( "phot_filter", "Johnson V" )
                                          << StringKeyValue( "phot_filterKeyword", "FILTER" )
                                          << StringKeyValue( "phot_margin", "15" )
                                          << StringKeyValue( "phot_catalogName", p_photCatalogName )
                                          << StringKeyValue( "phot_catalogFilter", "Vmag" )
                                          << StringKeyValue( "phot_vizierServer", p_serverURL )
                                          << StringKeyValue( "phot_useActive", "false" )
                                          << StringKeyValue( "phot_extractMode", "0" )
                                          << StringKeyValue( "phot_manualObjects", "[]" )
                                          << StringKeyValue( "phot_outputDir", tmpDir )
                                          << StringKeyValue( "phot_outputPrefix", "PCC_" )
                                          << StringKeyValue( "phot_generateFileTable", "true" )
                                          << StringKeyValue( "phot_generateFluxTable", "true" )
                                          << StringKeyValue( "phot_generatePSFFluxTable", "true" )
                                          << StringKeyValue( "phot_generateBackgTable", "false" )
                                          << StringKeyValue( "phot_generateSNRTable", "false" )
                                          << StringKeyValue( "phot_generateFlagTable", "false" )
                                          << StringKeyValue( "phot_saveDiagnostic", "false" )
                                          << StringKeyValue( "phot_autoSolve", "false" )
                                          << StringKeyValue( "phot_forceSolve", "false" )
                                          << StringKeyValue( "phot_solverUseImageMetadata", "false" )
                                          << StringKeyValue( "phot_saveSolve", "false" )
                                          << StringKeyValue( "phot_solveSuffix", "_WCS" )
                                          << StringKeyValue( "phot_generateErrorLog", "false" )
                                          << StringKeyValue( "phot_showBackgroundModel", String( bool( p_photShowBackgroundModels ) ) )
                                          << StringKeyValue( "phot_files", RPath + '|' + GPath + '|' + BPath )
                                          << StringKeyValue( "non_interactive", "true" );

         console.ExecuteScriptGlobal( scriptPath, arguments );

         File::Remove( RPath );
         File::Remove( GPath );
         File::Remove( BPath );

         Variant result = Module->EvaluateScript( "__PJSR_AdpPhotometry_SuccessCount" );
         if ( !result.IsValid() )
            throw Error( "Internal error: Invalid script execution: " + scriptPath );
         int successCount = result.ToInt();
         if ( successCount != 3 )
            throw Error( "Failure to calculate photometry: " + view.Id() );
      }

      Module->ProcessEvents();

      /*
       * Collect flux measurements.
       */
      if ( !File::Exists( fluxFilePath ) )
         throw Error( "Internal error: Cannot access photometry data file: " + fluxFilePath );

      IsoStringList lines = File::ReadLines( fluxFilePath );

      int indexOfCatalogRmag = -1;
      int indexOfCatalogGmag = -1;
      int indexOfCatalogBmag = -1;
      int indexOfImageRFlux  = -1;
      int indexOfImageGFlux  = -1;
      int indexOfImageBFlux  = -1;
      for ( const IsoString& line : lines )
         if ( line.StartsWithIC( "StarId" ) )
         {
            IsoStringList tokens;
            line.Break( tokens, ';' );
            int index = 0;
            for ( const IsoString& token : tokens )
            {
               if ( token.CompareIC( "Catalog_Vmag" ) == 0 )
                  indexOfCatalogGmag = index;
               else if ( token.CompareIC( "Catalog_Bmag" ) == 0 )
                  indexOfCatalogBmag = index;
               else if ( token.CompareIC( "Catalog_r'mag" ) == 0 )
                  indexOfCatalogRmag = index;
               else if ( token.StartsWithIC( "Flux" ) || token.StartsWithIC( "PSF_Flux" ) )
                  if ( token.EndsWith( "_1" ) )
                     indexOfImageRFlux = index;
                  else if ( token.EndsWith( "_2" ) )
                     indexOfImageGFlux = index;
                  else if ( token.EndsWith( "_3" ) )
                     indexOfImageBFlux = index;
               ++index;
            }
         }

      if (  indexOfCatalogRmag < 0
         || indexOfCatalogGmag < 0
         || indexOfCatalogBmag < 0
         || indexOfImageRFlux  < 0
         || indexOfImageGFlux  < 0
         || indexOfImageBFlux  < 0 )
         throw Error( "Wrong or corrupted photometry data: <raw>" + fluxFilePath + "</raw>" );

      Array<RG> rg;
      Array<BG> bg;
      for ( const IsoString& line : lines )
         if ( IsoCharTraits::IsDigit( *line ) )
         {
            IsoStringList tokens;
            line.Break( tokens, ';' );

            double imgMagG;
            if ( !tokens[indexOfImageGFlux].TryToDouble( imgMagG ) )
               continue;
            imgMagG = -2.5118 * Log( imgMagG );
            if ( !IsFinite( imgMagG ) )
               continue;

            double catMagG;
            if ( !tokens[indexOfCatalogGmag].TryToDouble( catMagG ) )
               continue;

            double imgMagR;
            bool haveImgMagR = tokens[indexOfImageRFlux].TryToDouble( imgMagR );
            if ( haveImgMagR )
               imgMagR = -2.5118 * Log( imgMagR );
            if ( !IsFinite( imgMagR ) )
               haveImgMagR = false;

            double imgMagB;
            bool haveImgMagB = tokens[indexOfImageBFlux].TryToDouble( imgMagB );
            if ( haveImgMagB )
               imgMagB = -2.5118 * Log( imgMagB );
            if ( !IsFinite( imgMagB ) )
               haveImgMagB = false;

            if ( haveImgMagR )
            {
               double catMagR;
               if ( tokens[indexOfCatalogRmag].TryToDouble( catMagR ) )
                  rg << RG{ catMagR, catMagG, imgMagR, imgMagG };
            }

            if ( haveImgMagB )
            {
               double catMagB;
               if ( tokens[indexOfCatalogBmag].TryToDouble( catMagB ) )
                  bg << BG{ catMagB, catMagG, imgMagB, imgMagG };
            }
         }

      /*
       * Fit color transformation functions.
       */
      Vector catIndexRG( rg.Length() );
      Vector imgIndexRG( rg.Length() );
      for ( size_type i = 0; i < rg.Length(); ++i )
      {
         catIndexRG[i] = rg[i].catR - rg[i].catG;
         imgIndexRG[i] = rg[i].imgR - rg[i].imgG;
      }

      Vector catIndexBG( bg.Length() );
      Vector imgIndexBG( bg.Length() );
      for ( size_type i = 0; i < bg.Length(); ++i )
      {
         catIndexBG[i] = bg[i].catB - bg[i].catG;
         imgIndexBG[i] = bg[i].imgB - bg[i].imgG;
      }

      LinearFit LRG = FitVectors( catIndexRG, imgIndexRG );
      LinearFit LBG = FitVectors( catIndexBG, imgIndexBG );

      console.NoteLn( String().Format( "<end><cbr>* Color transformation functions:\n"
                                       "R-G = %+.6e %c %.6e*(Sr-JV) &plusmn; %.6e\n"
                                       "B-G = %+.6e %c %.6e*(JB-JV) &plusmn; %.6e",
                                       LRG.a, (LRG.b < 0) ? '-' : '+', Abs( LRG.b ), LRG.adev,
                                       LBG.a, (LBG.b < 0) ? '-' : '+', Abs( LBG.b ), LBG.adev ) );

      DVector W( Pow( 2.5118, LRG( p_whiteReferenceSr_JV - p_zeroPointSr_JV ) ),
                 1.0,
                 Pow( 2.5118, LBG( p_whiteReferenceJB_JV - p_zeroPointJB_JV ) ) );

      W /= W.MaxComponent();

      console.NoteLn( String().Format( "<end><cbr>* White balance factors:\n"
                                       "W_R : %.4e\n"
                                       "W_G : %.4e\n"
                                       "W_B : %.4e", W[0], W[1], W[2] ) );

      if ( p_photGenerateGraphs )
      {
         if ( !ThePhotometricColorCalibrationGraphInterface->IsVisible() )
            ThePhotometricColorCalibrationGraphInterface->Launch();
         ThePhotometricColorCalibrationGraphInterface->UpdateGraphs( view.FullId(), p_photCatalogName, "r'", "V", "B",
                                                                     catIndexRG, imgIndexRG, LRG,
                                                                     catIndexBG, imgIndexBG, LBG );
      }

      /*
       * Generate metadata.
       */
      {
         DVector W0( 2 );
         W0[0] = p_whiteReferenceSr_JV;
         W0[1] = p_whiteReferenceJB_JV;
         DVector Z0( 2 );
         Z0[0] = p_zeroPointSr_JV;
         Z0[1] = p_zeroPointJB_JV;

         properties << Property( "PCL:PCC:Fit_Sr_JV_R_G", DVector( LRG.a, LRG.b, LRG.adev ) )
                    << Property( "PCL:PCC:Fit_JB_JV_B_G", DVector( LBG.a, LBG.b, LBG.adev ) )
                    << Property( "PCL:PCC:White_Sr_JV", W0[0] )
                    << Property( "PCL:PCC:White_JB_JV", W0[1] )
                    << Property( "PCL:PCC:Zero_Sr_JV", Z0[0] )
                    << Property( "PCL:PCC:Zero_JB_JV", Z0[1] )
                    << Property( "PCL:PCC:Scale_Sr_JV_JB_JV", W );

         keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Color calibration with "  + PixInsightVersion::AsString() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Color calibration with "  + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "PhotometricColorCalibration.fit_Sr_JV_R_G: %.6e %.6e %.6e", LRG.a, LRG.b, LRG.adev ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "PhotometricColorCalibration.fit_JB_JV_B_G: %.6e %.6e %.6e", LBG.a, LBG.b, LBG.adev ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "PhotometricColorCalibration.white_Sr_JV: %.5e", W0[0] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "PhotometricColorCalibration.white_JB_JV: %.5e", W0[1] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "PhotometricColorCalibration.zero_Sr_JV: %.5e", Z0[0] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "PhotometricColorCalibration.zero_JB_JV: %.5e", Z0[1] ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "PhotometricColorCalibration.scale_Sr_JV_JB_JV: %.4e %.4e %.4e", W[0], W[1], W[2] ) );
      }

      /*
       * Apply white balance and background neutralization.
       */
      {
         AutoViewLock lock( view );

         if ( p_applyCalibration )
         {
            ImageVariant image = view.Image();

            ApplyWhiteBalance( image, W );

            if ( p_neutralizeBackground )
            {
               if ( p_backgroundUseROI )
               {
                  p_backgroundROI.Order();
                  if ( !p_backgroundROI.IsRect() )
                     throw Error( "Empty background reference ROI defined" );
               }

               if ( p_backgroundHigh < p_backgroundLow )
                  Swap( p_backgroundLow, p_backgroundHigh );

               DVector B0;
               {
                  View bkgView;
                  if ( p_backgroundReferenceViewId.IsEmpty() )
                     bkgView = view;
                  else
                  {
                     bkgView = View::ViewById( p_backgroundReferenceViewId );
                     if ( bkgView.IsNull() )
                        throw Error( "No such view (background reference): " + p_backgroundReferenceViewId );
                     if ( !bkgView.IsColor() )
                        throw Error( "The background reference view must be a color image: " + p_backgroundReferenceViewId );
                  }

                  AutoViewLock lock( bkgView, false/*lock*/ );
                  if ( bkgView.CanWrite() )
                     lock.LockForWrite();

                  if ( bkgView != view )
                  {
                     String message = "<end><cbr>** Warning: Using a ";
                     if ( bkgView.IsPreview() && bkgView.Window() == view.Window() )
                        message << "preview of the target image as background reference";
                     else
                        message << "background reference view different from the target image";
                     message << ". This usually does not work.";
                     console.WarningLn( message );
                  }

                  ImageVariant bkgImage = bkgView.Image();

                  if ( p_backgroundUseROI )
                  {
                     bkgImage.SelectRectangle( p_backgroundROI );
                     Rect r = bkgImage.SelectedRectangle();
                     if ( !r.IsRect() )
                        bkgImage.ResetSelection();
                     console.WriteLn( String().Format( "<end><cbr>Region of interest : left=%d, top=%d, width=%d, height=%d",
                                                       r.x0, r.y0, r.Width(), r.Height() ) );
                  }

                  B0 = BackgroundReference( bkgImage, p_backgroundLow, p_backgroundHigh );
               }

               console.NoteLn( String().Format( "<end><cbr>* Background reference:\n"
                                                "B_R : %.5e\n"
                                                "B_G : %.5e\n"
                                                "B_B : %.5e", B0[0], B0[1], B0[2] ) );

               properties << Property( "PCL:PCC:BackgroundReference", B0 );

               keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                              IsoString().Format( "PhotometricColorCalibration.backgroundReference: %.5e %.5e %.5e", B0[0], B0[1], B0[2] ) );

               NeutralizeBackground( image, B0 );
            }
         }

         view.Window().SetKeywords( keywords );

         view.Window().MainView().SetProperties( properties,
                                                 true/*notify*/,
                        ViewPropertyAttribute::Storable | ViewPropertyAttribute::Permanent/* | ViewPropertyAttribute::WriteProtected*/ );
      }

      return true;
   }
   catch ( ... )
   {
      try
      {
         if ( File::Exists( TPath ) )
            File::Remove( TPath );
         if ( File::Exists( RPath ) )
            File::Remove( RPath );
         if ( File::Exists( GPath ) )
            File::Remove( GPath );
         if ( File::Exists( BPath ) )
            File::Remove( BPath );
      }
      catch ( ... )
      {
         // Mute additional file access errors.
      }

      throw;
   }
}

// ----------------------------------------------------------------------------

void* PhotometricColorCalibrationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == ThePCCWhiteReferenceIdParameter )
      return p_whiteReferenceId.Begin();
   if ( p == ThePCCWhiteReferenceNameParameter )
      return p_whiteReferenceName.Begin();
   if ( p == ThePCCWhiteReferenceSr_JVParameter )
      return &p_whiteReferenceSr_JV;
   if ( p == ThePCCWhiteReferenceJB_JVParameter )
      return &p_whiteReferenceJB_JV;
   if ( p == ThePCCZeroPointSr_JVParameter )
      return &p_zeroPointSr_JV;
   if ( p == ThePCCZeroPointJB_JVParameter )
      return &p_zeroPointJB_JV;
   if ( p == ThePCCFocalLengthParameter )
      return &p_focalLength;
   if ( p == ThePCCPixelSizeParameter )
      return &p_pixelSize;
   if ( p == ThePCCCenterRAParameter )
      return &p_centerRA;
   if ( p == ThePCCCenterDecParameter )
      return &p_centerDec;
   if ( p == ThePCCEpochJDParameter )
      return &p_epochJD;
   if ( p == ThePCCForcePlateSolveParameter )
      return &p_forcePlateSolve;
   if ( p == ThePCCIgnoreImagePositionAndScaleParameter )
      return &p_ignoreImagePositionAndScale;
   if ( p == ThePCCServerURLParameter )
      return p_serverURL.Begin();
   if ( p == ThePCCSolverCatalogNameParameter )
      return p_solverCatalogName.Begin();
   if ( p == ThePCCSolverAutoCatalogParameter )
      return &p_solverAutoCatalog;
   if ( p == ThePCCSolverLimitMagnitudeParameter )
      return &p_solverLimitMagnitude;
   if ( p == ThePCCSolverAutoLimitMagnitudeParameter )
      return &p_solverAutoLimitMagnitude;
   if ( p == ThePCCSolverAutoLimitMagnitudeFactorParameter )
      return &p_solverAutoLimitMagnitudeFactor;
   if ( p == ThePCCSolverStarSensitivityParameter )
      return &p_solverStarSensitivity;
   if ( p == ThePCCSolverNoiseLayersParameter )
      return &p_solverNoiseLayers;
   if ( p == ThePCCSolverAlignmentDeviceParameter )
      return &p_solverAlignmentDevice;
   if ( p == ThePCCSolverDistortionCorrectionParameter )
      return &p_solverDistortionCorrection;
   if ( p == ThePCCSolverSplineSmoothingParameter )
      return &p_solverSplineSmoothing;
   if ( p == ThePCCSolverProjectionParameter )
      return &p_solverProjection;
   if ( p == ThePCCPhotCatalogNameParameter )
      return p_photCatalogName.Begin();
   if ( p == ThePCCPhotLimitMagnitudeParameter )
      return &p_photLimitMagnitude;
   if ( p == ThePCCPhotAutoLimitMagnitudeParameter )
      return &p_photAutoLimitMagnitude;
   if ( p == ThePCCPhotAutoLimitMagnitudeFactorParameter )
      return &p_photAutoLimitMagnitudeFactor;
   if ( p == ThePCCPhotAutoApertureParameter )
      return &p_photAutoAperture;
   if ( p == ThePCCPhotApertureParameter )
      return &p_photAperture;
   if ( p == ThePCCPhotUsePSFParameter )
      return &p_photUsePSF;
   if ( p == ThePCCPhotSaturationThresholdParameter )
      return &p_photSaturationThreshold;
   if ( p == ThePCCPhotShowDetectedStarsParameter )
      return &p_photShowDetectedStars;
   if ( p == ThePCCPhotShowBackgroundModelsParameter )
      return &p_photShowBackgroundModels;
   if ( p == ThePCCPhotGenerateGraphsParameter )
      return &p_photGenerateGraphs;
   if ( p == ThePCCNeutralizeBackgroundParameter )
      return &p_neutralizeBackground;
   if ( p == ThePCCApplyCalibrationParameter )
      return &p_applyCalibration;
   if ( p == ThePCCBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Begin();
   if ( p == ThePCCBackgroundLowParameter )
      return &p_backgroundLow;
   if ( p == ThePCCBackgroundHighParameter )
      return &p_backgroundHigh;
   if ( p == ThePCCBackgroundUseROIParameter )
      return &p_backgroundUseROI;
   if ( p == ThePCCBackgroundROIX0Parameter )
      return &p_backgroundROI.x0;
   if ( p == ThePCCBackgroundROIY0Parameter )
      return &p_backgroundROI.y0;
   if ( p == ThePCCBackgroundROIX1Parameter )
      return &p_backgroundROI.x1;
   if ( p == ThePCCBackgroundROIY1Parameter )
      return &p_backgroundROI.y1;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == ThePCCWhiteReferenceIdParameter )
   {
      p_whiteReferenceId.Clear();
      if ( sizeOrLength > 0 )
         p_whiteReferenceId.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCWhiteReferenceNameParameter )
   {
      p_whiteReferenceName.Clear();
      if ( sizeOrLength > 0 )
         p_whiteReferenceName.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCServerURLParameter )
   {
      p_serverURL.Clear();
      if ( sizeOrLength > 0 )
         p_serverURL.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCSolverCatalogNameParameter )
   {
      p_solverCatalogName.Clear();
      if ( sizeOrLength > 0 )
         p_solverCatalogName.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCPhotCatalogNameParameter )
   {
      p_photCatalogName.Clear();
      if ( sizeOrLength > 0 )
         p_photCatalogName.SetLength( sizeOrLength );
   }
   else if ( p == ThePCCBackgroundReferenceViewIdParameter )
   {
      p_backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         p_backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type PhotometricColorCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == ThePCCWhiteReferenceIdParameter )
      return p_whiteReferenceId.Length();
   if ( p == ThePCCWhiteReferenceNameParameter )
      return p_whiteReferenceName.Length();
   if ( p == ThePCCServerURLParameter )
      return p_serverURL.Length();
   if ( p == ThePCCSolverCatalogNameParameter )
      return p_solverCatalogName.Length();
   if ( p == ThePCCPhotCatalogNameParameter )
      return p_photCatalogName.Length();
   if ( p == ThePCCBackgroundReferenceViewIdParameter )
      return p_backgroundReferenceViewId.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationInstance.cpp - Released 2018-11-23T18:45:58Z
