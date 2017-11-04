//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureData.h - Released 2017-08-01T14:26:58Z
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

#ifndef __SubframeSelectorMeasureData_h
#define __SubframeSelectorMeasureData_h

#include <pcl/MetaParameter.h>

#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

struct MeasureData
{
   String   path;
   double   fwhm;
   double   fwhmMeanDev;
   double   eccentricity;
   double   eccentricityMeanDev;
   double   snrWeight;
   double   median;
   double   medianMeanDev;
   double   noise;
   double   noiseRatio;
   uint16   stars;
   double   starResidual;
   double   starResidualMeanDev;

   MeasureData( const String& p = String( "" ) ) :
           path( p ),
           fwhm( TheSSMeasurementFWHMParameter->DefaultValue() ),
           fwhmMeanDev( 0 ),
           eccentricity( TheSSMeasurementEccentricityParameter->DefaultValue() ),
           eccentricityMeanDev( 0 ),
           snrWeight( TheSSMeasurementSNRWeightParameter->DefaultValue() ),
           median( TheSSMeasurementMedianParameter->DefaultValue() ),
           medianMeanDev( 0 ),
           noise( TheSSMeasurementNoiseParameter->DefaultValue() ),
           noiseRatio( TheSSMeasurementNoiseRatioParameter->DefaultValue() ),
           stars( TheSSMeasurementStarsParameter->DefaultValue() ),
           starResidual( TheSSMeasurementStarResidualParameter->DefaultValue() ),
           starResidualMeanDev( 0 )
   {
   }

   MeasureData( const MeasureData& x ) = default;
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

   MeasureItem( uint16 i, const String& p = String( "" ) ) :
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

   MeasureItem( const MeasureItem& x ) = default;

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

   float FWHM( const float& subframeScale, const int& scaleUnit ) const
   {
      if ( scaleUnit == SSScaleUnit::ArcSeconds )
         return fwhm * subframeScale;
      if ( scaleUnit == SSScaleUnit::Pixel )
         return fwhm;
      return fwhm;
   }

   float Median( const float& cameraGain, const int& cameraResolution, const int& dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return median * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return median * cameraResolution;
      return median;
   }

   float MedianMeanDev( const float& cameraGain, const int& cameraResolution, const int& dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return medianMeanDev * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return medianMeanDev * cameraResolution;
      return medianMeanDev;
   }

   float Noise( const float& cameraGain, const int& cameraResolution, const int& dataUnit ) const
   {
      if ( dataUnit == SSDataUnit::Electron )
         return noise * cameraResolution * cameraGain;
      if ( dataUnit == SSDataUnit::DataNumber )
         return noise * cameraResolution;
      return noise;
   }

   float FWHMMeanDeviation( const float& subframeScale, const int& scaleUnit ) const
   {
      if ( scaleUnit == SSScaleUnit::ArcSeconds )
         return fwhmMeanDev * subframeScale;
      if ( scaleUnit == SSScaleUnit::Pixel )
         return fwhmMeanDev;
      return fwhmMeanDev;
   }

   String JavaScriptParameters( const float& subframeScale, const int& scaleUnit, const float& cameraGain,
                                const int& cameraResolution, const int& dataUnit ) const
   {
      return String().Format( "let Index = %i;\n", index ) +
              String().Format( "let Weight = %.4f;\n", weight ) +
              String().Format( "let FWHM = %.4f;\n", FWHM( subframeScale, scaleUnit ) ) +
              String().Format( "let Eccentricity = %.4f;\n", eccentricity ) +
              String().Format( "let SNRWeight = %.4f;\n", snrWeight ) +
              String().Format( "let Median = %.4f;\n", Median( cameraGain, cameraResolution, dataUnit ) ) +
              String().Format( "let MedianMeanDev = %.4f;\n", MedianMeanDev( cameraGain, cameraResolution, dataUnit ) ) +
              String().Format( "let Noise = %.4f;\n", Noise( cameraGain, cameraResolution, dataUnit ) ) +
              String().Format( "let NoiseRatio = %.4f;\n", noiseRatio ) +
              String().Format( "let Stars = %i;\n", stars ) +
              String().Format( "let StarResidual = %.4f;\n", starResidual ) +
              String().Format( "let FWHMMeanDev = %.4f;\n", FWHMMeanDeviation( subframeScale, scaleUnit ) ) +
              String().Format( "let EccentricityMeanDev = %.4f;\n", eccentricityMeanDev ) +
              String().Format( "let StarResidualMeanDev = %.4f;\n", starResidualMeanDev );
   }

   double SortingValue( pcl_enum sortBy ) const
   {
      switch ( sortBy )
      {
      case SSSortingProperty::Index: return index;
      case SSSortingProperty::Weight: return weight;
      case SSSortingProperty::FWHM: return fwhm;
      case SSSortingProperty::Eccentricity: return eccentricity;
      case SSSortingProperty::SNRWeight: return snrWeight;
      case SSSortingProperty::Median: return median;
      case SSSortingProperty::MedianMeanDev: return medianMeanDev;
      case SSSortingProperty::Noise: return noise;
      case SSSortingProperty::NoiseRatio: return noiseRatio;
      case SSSortingProperty::Stars: return stars;
      case SSSortingProperty::StarResidual: return starResidual;
      case SSSortingProperty::FWHMMeanDev: return fwhmMeanDev;
      case SSSortingProperty::EccentricityMeanDev: return eccentricityMeanDev;
      case SSSortingProperty::StarResidualMeanDev: return starResidualMeanDev;
      default: return 0; // ?
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
      int a = 0;
      int o = 0;
      for ( String::const_iterator i = expression.Begin(); i < expression.End(); ++i )
      {
         if ( !isalnum( *i ) && *i != '(' && *i != ')' && *i != '&' && *i != '|' && *i != ' '
              && *i != '*' && *i != '+' && *i != '-' && *i != '/' && *i != '.'
              && *i != '<' && *i != '>' && *i != '=' && *i != '!' )
            return false;

         if ( *i == '(' )
            ++pOpen;
         if ( *i == ')' )
            ++pClose;

         if ( *i == '&' )
            ++a;
         if ( *i == '|' )
            ++o;
      }

      return pOpen == pClose && a % 2 == 0 && o % 2 == 0;
   }

   static void MedianAndMeanDeviation( Array<double>& values,
                                       double& median, double& deviation )
   {
      median = pcl::Median( values.Begin(), values.End() );
      deviation = pcl::AvgDev( values.Begin(), values.End(), median );
   }

   static double DeviationNormalize( const double& value, const double& median,
                                     const double& deviation )
   {
      return ( value - median ) / ( deviation != 0.0 ? deviation : 1.0 );
   }
};

// ----------------------------------------------------------------------------

class SubframeSortingBinaryPredicate
{
public:

   SubframeSortingBinaryPredicate( pcl_enum sortBy, int ascending ) : sortBy( sortBy ), ascending( ascending )
   {
   }

   bool operator()( const MeasureItem& s1, const MeasureItem& s2 ) const
   {
      if ( ascending > 0 )
         return s1.SortingValue( sortBy ) > s2.SortingValue( sortBy );
      else
         return s1.SortingValue( sortBy ) < s2.SortingValue( sortBy );
   }

private:

   pcl_enum sortBy;
   int ascending;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __SubframeSelectorMeasureData_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasureData.h - Released 2017-08-01T14:26:58Z
