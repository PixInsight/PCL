//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.02.0025
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureData.h - Released 2019-01-21T12:06:42Z
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

#ifndef __SubframeSelectorMeasureData_h
#define __SubframeSelectorMeasureData_h

#include <pcl/MetaParameter.h>

#include "SubframeSelectorCache.h"
#include "SubframeSelectorParameters.h"

#define CACHE_VERSION 1

namespace pcl
{

// ----------------------------------------------------------------------------

static double DeviationNormalize( const double& value, const double& median,
                                  const double& deviation )
{
   return (value - median)/((deviation != 0.0) ? deviation : 1.0);
}

struct MeasureProperty
{
   double min = DBL_MAX;
   double max = DBL_MIN;
   double median = 0;
   double deviation = 0;
};

// ----------------------------------------------------------------------------

struct MeasureProperties
{
   MeasureProperty weight;
   MeasureProperty fwhm;
   MeasureProperty fwhmMeanDev;
   MeasureProperty eccentricity;
   MeasureProperty eccentricityMeanDev;
   MeasureProperty snrWeight;
   MeasureProperty median;
   MeasureProperty medianMeanDev;
   MeasureProperty noise;
   MeasureProperty noiseRatio;
   MeasureProperty stars;
   MeasureProperty starResidual;
   MeasureProperty starResidualMeanDev;

   MeasureProperties()
   {
      weight = MeasureProperty();
      fwhm = MeasureProperty();
      fwhmMeanDev = MeasureProperty();
      eccentricity = MeasureProperty();
      eccentricityMeanDev = MeasureProperty();
      snrWeight = MeasureProperty();
      median = MeasureProperty();
      medianMeanDev = MeasureProperty();
      noise = MeasureProperty();
      noiseRatio = MeasureProperty();
      stars = MeasureProperty();
      starResidual = MeasureProperty();
      starResidualMeanDev = MeasureProperty();
   }
};

// ----------------------------------------------------------------------------

struct MeasureData
{
   String path;
   double fwhm;
   double fwhmMeanDev;
   double eccentricity;
   double eccentricityMeanDev;
   double snrWeight;
   double median;
   double medianMeanDev;
   double noise;
   double noiseRatio;
   uint16 stars;
   double starResidual;
   double starResidualMeanDev;

   MeasureData( const String& p = String() ) :
      path( p )
   {
      ResetCacheableData();
   }

   MeasureData( const MeasureData& ) = default;

   void ResetCacheableData()
   {
      fwhm = TheSSMeasurementFWHMParameter->DefaultValue();
      fwhmMeanDev = 0;
      eccentricity = TheSSMeasurementEccentricityParameter->DefaultValue();
      eccentricityMeanDev = 0;
      snrWeight = TheSSMeasurementSNRWeightParameter->DefaultValue();
      median = TheSSMeasurementMedianParameter->DefaultValue();
      medianMeanDev = 0;
      noise = TheSSMeasurementNoiseParameter->DefaultValue();
      noiseRatio = TheSSMeasurementNoiseRatioParameter->DefaultValue();
      stars = TheSSMeasurementStarsParameter->DefaultValue();
      starResidual = TheSSMeasurementStarResidualParameter->DefaultValue();
      starResidualMeanDev = 0;
   }

   void AddToCache() const
   {
      SubframeSelectorCacheItem item( path );
      item.cacheVersion        = CACHE_VERSION;
      item.fwhm                = fwhm;
      item.fwhmMeanDev         = fwhmMeanDev;
      item.eccentricity        = eccentricity;
      item.eccentricityMeanDev = eccentricityMeanDev;
      item.snrWeight           = snrWeight;
      item.median              = median;
      item.medianMeanDev       = medianMeanDev;
      item.noise               = noise;
      item.noiseRatio          = noiseRatio;
      item.stars               = stars;
      item.starResidual        = starResidual;
      item.starResidualMeanDev = starResidualMeanDev;
      TheSubframeSelectorCache->Add( item );
   }

   bool GetFromCache()
   {
      ResetCacheableData();

      SubframeSelectorCacheItem item;
      if ( !TheSubframeSelectorCache->Get( item, path ) )
         return false;

      fwhm                = item.fwhm;
      fwhmMeanDev         = item.fwhmMeanDev;
      eccentricity        = item.eccentricity;
      eccentricityMeanDev = item.eccentricityMeanDev;
      snrWeight           = item.snrWeight;
      median              = item.median;
      medianMeanDev       = item.medianMeanDev;
      noise               = item.noise;
      noiseRatio          = item.noiseRatio;
      stars               = item.stars;
      starResidual        = item.starResidual;
      starResidualMeanDev = item.starResidualMeanDev;

      return item.cacheVersion   == CACHE_VERSION &&
             fwhm                != TheSSMeasurementFWHMParameter->DefaultValue() &&
             fwhmMeanDev         != 0 &&
             eccentricity        != TheSSMeasurementEccentricityParameter->DefaultValue() &&
             eccentricityMeanDev != 0 &&
             snrWeight           != TheSSMeasurementSNRWeightParameter->DefaultValue() &&
             median              != TheSSMeasurementMedianParameter->DefaultValue() &&
             medianMeanDev       != 0 &&
             noise               != TheSSMeasurementNoiseParameter->DefaultValue() &&
             noiseRatio          != TheSSMeasurementNoiseRatioParameter->DefaultValue() &&
             stars               != TheSSMeasurementStarsParameter->DefaultValue() &&
             starResidual        != TheSSMeasurementStarResidualParameter->DefaultValue() &&
             starResidualMeanDev != 0;
   }
};

// ----------------------------------------------------------------------------

struct MeasureItem
{
   uint16   index;
   pcl_bool enabled;
   pcl_bool locked;
   String   path;
   float    weight;
   float    fwhm;
   float    eccentricity;
   float    snrWeight;
   float    median;
   float    medianMeanDev;
   float    noise;
   float    noiseRatio;
   uint16   stars;
   float    starResidual;
   float    fwhmMeanDev;
   float    eccentricityMeanDev;
   float    starResidualMeanDev;

   MeasureItem( uint16 i, const String& p = String() ) :
      index( i ),
      enabled( TheSSMeasurementEnabledParameter->DefaultValue() ),
      locked( TheSSMeasurementLockedParameter->DefaultValue() ),
      path( p ),
      weight( TheSSMeasurementWeightParameter->DefaultValue() ),
      fwhm( TheSSMeasurementFWHMParameter->DefaultValue() ),
      eccentricity( TheSSMeasurementEccentricityParameter->DefaultValue() ),
      snrWeight( TheSSMeasurementSNRWeightParameter->DefaultValue() ),
      median( TheSSMeasurementMedianParameter->DefaultValue() ),
      medianMeanDev( TheSSMeasurementMedianMeanDevParameter->DefaultValue() ),
      noise( TheSSMeasurementNoiseParameter->DefaultValue() ),
      noiseRatio( TheSSMeasurementNoiseRatioParameter->DefaultValue() ),
      stars( TheSSMeasurementStarsParameter->DefaultValue() ),
      starResidual( TheSSMeasurementStarResidualParameter->DefaultValue() ),
      fwhmMeanDev( TheSSMeasurementFWHMMeanDevParameter->DefaultValue() ),
      eccentricityMeanDev( TheSSMeasurementEccentricityMeanDevParameter->DefaultValue() ),
      starResidualMeanDev( TheSSMeasurementStarResidualMeanDevParameter->DefaultValue() )
   {
   }

   MeasureItem( const MeasureItem& ) = default;

   void Input( const MeasureData& measureData )
   {
      path = measureData.path;
      fwhm = measureData.fwhm;
      eccentricity = measureData.eccentricity;
      snrWeight = measureData.snrWeight;
      median = measureData.median;
      medianMeanDev = measureData.medianMeanDev;
      noise = measureData.noise;
      noiseRatio = measureData.noiseRatio;
      stars = measureData.stars;
      starResidual = measureData.starResidual;
      fwhmMeanDev = measureData.fwhmMeanDev;
      eccentricityMeanDev = measureData.eccentricityMeanDev;
      starResidualMeanDev = measureData.starResidualMeanDev;
   }

   float FWHM( float subframeScale, int scaleUnit ) const
   {
      if ( scaleUnit == SSScaleUnit::ArcSeconds )
         return fwhm * subframeScale;
      if ( scaleUnit == SSScaleUnit::Pixel )
         return fwhm;
      return fwhm;
   }

   float Median( float cameraGain, int cameraResolution, int dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return median * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return median * cameraResolution;
      return median;
   }

   float MedianMeanDev( float cameraGain, int cameraResolution, int dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return medianMeanDev * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return medianMeanDev * cameraResolution;
      return medianMeanDev;
   }

   float Noise( float cameraGain, int cameraResolution, int dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return noise * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return noise * cameraResolution;
      return noise;
   }

   float FWHMMeanDeviation( float subframeScale, int scaleUnit ) const
   {
      if ( scaleUnit == SSScaleUnit::ArcSeconds )
         return fwhmMeanDev * subframeScale;
      if ( scaleUnit == SSScaleUnit::Pixel )
         return fwhmMeanDev;
      return fwhmMeanDev;
   }

   String JavaScriptParameters( float subframeScale, int scaleUnit, float cameraGain,
                                int cameraResolution, int dataUnit, const MeasureProperties& properties ) const
   {
      return

      String().Format( "let Index = %i;\n", index ) +

      String().Format( "let Weight = %.4f;\n", weight ) +
      String().Format( "let WeightMin = %.4f;\n", properties.weight.min ) +
      String().Format( "let WeightMax = %.4f;\n", properties.weight.max ) +
      String().Format( "let WeightMedian = %.4f;\n", properties.weight.median ) +
      String().Format( "let WeightSigma = %.4f;\n", DeviationNormalize(
               weight, properties.weight.median, properties.weight.deviation
      ) ) +

      String().Format( "let FWHM = %.4f;\n", FWHM( subframeScale, scaleUnit ) ) +
      String().Format( "let FWHMMin = %.4f;\n", properties.fwhm.min ) +
      String().Format( "let FWHMMax = %.4f;\n", properties.fwhm.max ) +
      String().Format( "let FWHMMedian = %.4f;\n", properties.fwhm.median ) +
      String().Format( "let FWHMSigma = %.4f;\n", DeviationNormalize(
               FWHM( subframeScale, scaleUnit ), properties.fwhm.median, properties.fwhm.deviation
      ) ) +

      String().Format( "let Eccentricity = %.4f;\n", eccentricity ) +
      String().Format( "let EccentricityMin = %.4f;\n", properties.eccentricity.min ) +
      String().Format( "let EccentricityMax = %.4f;\n", properties.eccentricity.max ) +
      String().Format( "let EccentricityMedian = %.4f;\n", properties.eccentricity.median ) +
      String().Format( "let EccentricitySigma = %.4f;\n", DeviationNormalize(
               eccentricity, properties.eccentricity.median, properties.eccentricity.deviation
      ) ) +

      String().Format( "let SNRWeight = %.4f;\n", snrWeight ) +
      String().Format( "let SNRWeightMin = %.4f;\n", properties.snrWeight.min ) +
      String().Format( "let SNRWeightMax = %.4f;\n", properties.snrWeight.max ) +
      String().Format( "let SNRWeightMedian = %.4f;\n", properties.snrWeight.median ) +
      String().Format( "let SNRWeightSigma = %.4f;\n", DeviationNormalize(
               snrWeight, properties.snrWeight.median, properties.snrWeight.deviation
      ) ) +

      String().Format( "let Median = %.4f;\n", Median( cameraGain, cameraResolution, dataUnit ) ) +
      String().Format( "let MedianMin = %.4f;\n", properties.median.min ) +
      String().Format( "let MedianMax = %.4f;\n", properties.median.max ) +
      String().Format( "let MedianMedian = %.4f;\n", properties.median.median ) +
      String().Format( "let MedianSigma = %.4f;\n", DeviationNormalize(
               Median( cameraGain, cameraResolution, dataUnit ),
               properties.median.median, properties.median.deviation
      ) ) +

      String().Format( "let MedianMeanDev = %.4f;\n", MedianMeanDev( cameraGain, cameraResolution, dataUnit ) ) +
      String().Format( "let MedianMeanDevMin = %.4f;\n", properties.medianMeanDev.min ) +
      String().Format( "let MedianMeanDevMax = %.4f;\n", properties.medianMeanDev.max ) +
      String().Format( "let MedianMeanDevMedian = %.4f;\n", properties.medianMeanDev.median ) +
      String().Format( "let MedianMeanDevSigma = %.4f;\n", DeviationNormalize(
               MedianMeanDev( cameraGain, cameraResolution, dataUnit ),
               properties.medianMeanDev.median, properties.medianMeanDev.deviation
      ) ) +

      String().Format( "let Noise = %.4f;\n", Noise( cameraGain, cameraResolution, dataUnit ) ) +
      String().Format( "let NoiseMin = %.4f;\n", properties.noise.min ) +
      String().Format( "let NoiseMax = %.4f;\n", properties.noise.max ) +
      String().Format( "let NoiseMedian = %.4f;\n", properties.noise.median ) +
      String().Format( "let NoiseSigma = %.4f;\n", DeviationNormalize(
               Noise( cameraGain, cameraResolution, dataUnit ),
               properties.noise.median, properties.noise.deviation
      ) ) +

      String().Format( "let NoiseRatio = %.4f;\n", noiseRatio ) +
      String().Format( "let NoiseRatioMin = %.4f;\n", properties.noiseRatio.min ) +
      String().Format( "let NoiseRatioMax = %.4f;\n", properties.noiseRatio.max ) +
      String().Format( "let NoiseRatioMedian = %.4f;\n", properties.noiseRatio.median ) +
      String().Format( "let NoiseRatioSigma = %.4f;\n", DeviationNormalize(
               noiseRatio, properties.noiseRatio.median, properties.noiseRatio.deviation
      ) ) +

      String().Format( "let Stars = %i;\n", stars ) +
      String().Format( "let StarsMin = %.4f;\n", properties.stars.min ) +
      String().Format( "let StarsMax = %.4f;\n", properties.stars.max ) +
      String().Format( "let StarsMedian = %.4f;\n", properties.stars.median ) +
      String().Format( "let StarsSigma = %.4f;\n", DeviationNormalize(
               stars, properties.stars.median, properties.stars.deviation
      ) ) +

      String().Format( "let StarResidual = %.4f;\n", starResidual ) +
      String().Format( "let StarResidualMin = %.4f;\n", properties.starResidual.min ) +
      String().Format( "let StarResidualMax = %.4f;\n", properties.starResidual.max ) +
      String().Format( "let StarResidualMedian = %.4f;\n", properties.starResidual.median ) +
      String().Format( "let StarResidualSigma = %.4f;\n", DeviationNormalize(
               starResidual, properties.starResidual.median, properties.starResidual.deviation
      ) ) +

      String().Format( "let FWHMMeanDev = %.4f;\n", FWHMMeanDeviation( subframeScale, scaleUnit ) ) +
      String().Format( "let FWHMMeanDevMin = %.4f;\n", properties.fwhmMeanDev.min ) +
      String().Format( "let FWHMMeanDevMax = %.4f;\n", properties.fwhmMeanDev.max ) +
      String().Format( "let FWHMMeanDevMedian = %.4f;\n", properties.fwhmMeanDev.median ) +
      String().Format( "let FWHMMeanDevSigma = %.4f;\n", DeviationNormalize(
               FWHMMeanDeviation( subframeScale, scaleUnit ),
               properties.fwhmMeanDev.median, properties.fwhmMeanDev.deviation
      ) ) +

      String().Format( "let EccentricityMeanDev = %.4f;\n", eccentricityMeanDev ) +
      String().Format( "let EccentricityMeanDevMin = %.4f;\n", properties.eccentricityMeanDev.min ) +
      String().Format( "let EccentricityMeanDevMax = %.4f;\n", properties.eccentricityMeanDev.max ) +
      String().Format( "let EccentricityMeanDevMedian = %.4f;\n", properties.eccentricityMeanDev.median ) +
      String().Format( "let EccentricityMeanDevSigma = %.4f;\n", DeviationNormalize(
               eccentricityMeanDev, properties.eccentricityMeanDev.median, properties.eccentricityMeanDev.deviation
      ) ) +

      String().Format( "let StarResidualMeanDev = %.4f;\n", starResidualMeanDev ) +
      String().Format( "let StarResidualMeanDevMin = %.4f;\n", properties.starResidualMeanDev.min ) +
      String().Format( "let StarResidualMeanDevMax = %.4f;\n", properties.starResidualMeanDev.max ) +
      String().Format( "let StarResidualMeanDevMedian = %.4f;\n", properties.starResidualMeanDev.median ) +
      String().Format( "let StarResidualMeanDevSigma = %.4f;\n", DeviationNormalize(
               starResidualMeanDev, properties.starResidualMeanDev.median, properties.starResidualMeanDev.deviation
      ) );
   }

   double SortingValue( pcl_enum sortBy ) const
   {
      switch ( sortBy )
      {
      case SSSortingProperty::Index:               return index;
      case SSSortingProperty::Weight:              return weight;
      case SSSortingProperty::FWHM:                return fwhm;
      case SSSortingProperty::Eccentricity:        return eccentricity;
      case SSSortingProperty::SNRWeight:           return snrWeight;
      case SSSortingProperty::Median:              return median;
      case SSSortingProperty::MedianMeanDev:       return medianMeanDev;
      case SSSortingProperty::Noise:               return noise;
      case SSSortingProperty::NoiseRatio:          return noiseRatio;
      case SSSortingProperty::Stars:               return stars;
      case SSSortingProperty::StarResidual:        return starResidual;
      case SSSortingProperty::FWHMMeanDev:         return fwhmMeanDev;
      case SSSortingProperty::EccentricityMeanDev: return eccentricityMeanDev;
      case SSSortingProperty::StarResidualMeanDev: return starResidualMeanDev;
      default:                                     return 0; // ?
      }
   }
};

// ----------------------------------------------------------------------------

struct MeasureUtils
{
   // Rudimentary first-line check for a JS Expression
   static bool IsValidExpression( const String& expression )
   {
      if ( expression.IsEmpty() )
         return true;

      int pOpen = 0;
      int pClose = 0;
      int bOpen = 0;
      int bClose = 0;
      int cOpen = 0;
      int cClose = 0;
      int a = 0;
      int o = 0;
      for ( char16_type c : expression )
      {
         if (    !CharTraits::IsAlpha( c )
              && !CharTraits::IsDigit( c )
              && !CharTraits::IsSpace( c )
              && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}'
              && c != '&' && c != '|' && c != '*' && c != '+' && c != '-' && c != '/'
              && c != '.' && c != ',' && c != '%' && c != '?' && c != ':' && c != '_'
              && c != '<' && c != '>' && c != '=' && c != '!' && c != ';' )
            return false;

         if ( c == '(' )
            ++pOpen;
         if ( c == ')' )
            ++pClose;

         if ( c == '[' )
            ++bOpen;
         if ( c == ']' )
            ++bClose;

         if ( c == '{' )
            ++cOpen;
         if ( c == '}' )
            ++cClose;

         if ( c == '&' )
            ++a;
         if ( c == '|' )
            ++o;
      }

      return pOpen == pClose && bOpen == bClose && cOpen == cClose && a%2 == 0 && o%2 == 0;
   }

   static void MeasureProperties( const Array<MeasureItem>& measures, float subframeScale,
                                  int scaleUnit, float cameraGain,
                                  int cameraResolution, int dataUnit,
                                  MeasureProperties& properties )
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

      MeasureUtils::MeasureProperty( weight, properties.weight );
      MeasureUtils::MeasureProperty( fwhm, properties.fwhm );
      MeasureUtils::MeasureProperty( eccentricity, properties.eccentricity );
      MeasureUtils::MeasureProperty( snrWeight, properties.snrWeight );
      MeasureUtils::MeasureProperty( median, properties.median );
      MeasureUtils::MeasureProperty( medianMeanDev, properties.medianMeanDev );
      MeasureUtils::MeasureProperty( noise, properties.noise );
      MeasureUtils::MeasureProperty( noiseRatio, properties.noiseRatio );
      MeasureUtils::MeasureProperty( stars, properties.stars );
      MeasureUtils::MeasureProperty( starResidual, properties.starResidual );
      MeasureUtils::MeasureProperty( fwhmMeanDev, properties.fwhmMeanDev );
      MeasureUtils::MeasureProperty( eccentricityMeanDev, properties.eccentricityMeanDev );
      MeasureUtils::MeasureProperty( starResidualMeanDev, properties.starResidualMeanDev );
   }

   static void MeasureProperty( Array<double>& values, MeasureProperty& property )
   {
      for ( Array<double>::const_iterator i = values.Begin(); i != values.End(); ++i )
      {
         property.min = pcl::Min( *i, property.min );
         property.max = pcl::Max( *i, property.max );
      }
      property.median = pcl::Median( values.Begin(), values.End() );
      property.deviation = pcl::AvgDev( values.Begin(), values.End(), property.median );
   }

   static void MedianAndMeanDeviation( Array<double>& values,
                                       double& median, double& deviation )
   {
      median = pcl::Median( values.Begin(), values.End() );
      deviation = pcl::AvgDev( values.Begin(), values.End(), median );
   }
};

// ----------------------------------------------------------------------------

class SubframeSortingBinaryPredicate
{
public:

   SubframeSortingBinaryPredicate( pcl_enum sortBy, int ascending ) :
      m_sortBy( sortBy ),
      m_ascending( ascending )
   {
   }

   bool operator()( const MeasureItem& s1, const MeasureItem& s2 ) const
   {
      if ( m_ascending > 0 )
         return s1.SortingValue( m_sortBy ) > s2.SortingValue( m_sortBy );
      else
         return s1.SortingValue( m_sortBy ) < s2.SortingValue( m_sortBy );
   }

private:

   pcl_enum m_sortBy;
   int      m_ascending;
};

// ----------------------------------------------------------------------------

} // pcl

#undef CACHE_VERSION

#endif   // __SubframeSelectorMeasureData_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasureData.h - Released 2019-01-21T12:06:42Z
