//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/ImageStatistics.h - Released 2018-11-01T11:06:36Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_ImageStatistics_h
#define __PCL_ImageStatistics_h

/// \file pcl/ImageStatistics.h

#include <pcl/Defs.h>

#include <pcl/ImageVariant.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageStatistics
 * \brief Descriptive image statistics.
 *
 * %ImageStatistics provides basic descriptive statistics of pixel sample
 * values:
 *
 * \li Pixel sample count.
 *
 * \li Arithmetic mean.
 *
 * \li Median.
 *
 * \li Variance and standard deviation from the mean.
 *
 * \li Sum of squared samples.
 *
 * \li Average deviation from the median.
 *
 * \li Median absolute deviation from the median (MAD).
 *
 * \li Biweight midvariance.
 *
 * \li Percentage bend midvariance.
 *
 * \li Sn and Qn scale estimators of Rousseeuw/Croux.
 *
 * \li Extreme pixel sample values: minimum and maximum, and the image
 * coordinates of their first occurrences.
 *
 * All statistics can be enabled/disabled individually.
 *
 * %ImageStatistics works like a unidirectional image transformation, although
 * it isn't actually a derived class of ImageTransformation. Example of use:
 *
 * \code
 * Image image;
 * // ...
 * ImageStatistics S;
 * S.DisableVariance(); // disable variance and sigma calculations
 * image.SelectChannel( 2 ); // select the third channel of image
 * image.SelectRectangle( 80, 100, 450, 600 ); // select a rectangular region
 * S << image; // calculate image statistics for selected samples
 * Console().WriteLn( String().Format( "\<end\>\<cbr\>The median is: %.5f", S.Median() ) );
 * \endcode
 *
 * %ImageStatistics calculates statistical values for the currently selected
 * region and channel of its source image.
 */
class PCL_CLASS ImageStatistics : public ParallelProcess
{
public:

   /*!
    * \class pcl::ImageStatistics::Data
    * \brief Statistical data in the normalized [0,1] range.
    */
   struct PCL_CLASS Data
   {
      size_type count = 0;              //!< Total number of evaluated samples.
      double    mean = 0;               //!< Arithmetic mean.
      double    sumOfSquares = 0;       //!< Sum of squared samples.
      double    median = 0;             //!< Median sample value.
      double    variance = 0;           //!< Variance from the mean.
      double    stdDev = 0;             //!< Standard deviation (=Sqrt(variance)).
      double    avgDev = 0;             //!< Average deviation from the median.
      double    MAD = 0;                //!< Median absolute deviation from the median.
      double    bwmv = 0;               //!< Biweight midvariance.
      double    pbmv = 0;               //!< Percentage bend midvariance.
      double    Sn = 0;                 //!< Sn scale estimator of Rousseeuw and Croux.
      double    Qn = 0;                 //!< Qn scale estimator of Rousseeuw and Croux.
      double    minimum = 0;            //!< Minimum sample value.
      Point     minPos = Point( 0 );    //!< Position of the minimum sample value.
      double    maximum = 0;            //!< Maximum sample value.
      Point     maxPos = Point( 0 );    //!< Position of the maximum sample value.
      double    low = 0;                //!< If rejectLow, ignore samples less than or equal to this value.
      double    high = 0;               //!< If rejectHigh, ignore samples greater than or equal to this value.
      bool      rejectLow = false;      //!< Ignore samples with values less than or equal to low.
      bool      rejectHigh = false;     //!< Ignore samples with values greater than or equal to high.
      bool      noExtremes = false;     //!< Do not compute minimum and maximum sample values.
      bool      noMean = false;         //!< Do not compute the mean, variance, and standard deviation.
      bool      noSumOfSquares = false; //!< Do not compute the sum of squared samples.
      bool      noVariance = false;     //!< Do not compute variance and standard deviation.
      bool      noMedian = false;       //!< Do not compute median, average deviation, MAD, biweight and bend midvariances.
      bool      noAvgDev = false;       //!< Do not compute average deviation.
      bool      noMAD = false;          //!< Do not compute MAD.
      bool      noBWMV = false;         //!< Do not compute biweight midvariance.
      bool      noPBMV = false;         //!< Do not compute percentage bend midvariance.
      bool      noSn = true;            //!< Do not compute Sn estimator of scale.
      bool      noQn = true;            //!< Do not compute Qn estimator of scale.

      /*!
       * Constructs a default %ImageStatistics::Data structure.
       */
      Data() = default;

      /*!
       * Copy constructor.
       */
      Data( const Data& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      Data& operator =( const Data& ) = default;

      /*!
       * Copies all data items from another instance \a x.
       */
      void Assign( const Data& x )
      {
         (void)operator =( x );
      }

      /*!
       * Copies only statistical data items from another instance \a x. Does
       * not copy operating parameters (such as low, high, noVariance, etc.).
       */
      void AssignStatisticalData( const Data& x )
      {
         count        = x.count;
         mean         = x.mean;
         sumOfSquares = x.sumOfSquares;
         median       = x.median;
         variance     = x.variance;
         stdDev       = x.stdDev;
         avgDev       = x.avgDev;
         MAD          = x.MAD;
         bwmv         = x.bwmv;
         pbmv         = x.pbmv;
         Sn           = x.Sn;
         Qn           = x.Qn;
         minimum      = x.minimum;
         minPos       = x.minPos;
         maximum      = x.maximum;
         maxPos       = x.maxPos;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * Constructs an %ImageStatistics object.
    */
   ImageStatistics() = default;

   /*!
    * Copy constructor.
    */
   ImageStatistics( const ImageStatistics& ) = default;

   /*!
    * Destroys an %ImageStatistics object.
    */
   virtual ~ImageStatistics()
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   ImageStatistics& operator =( const ImageStatistics& ) = default;

   /*!
    * Copies other %ImageStatistics object \a x to this object.
    */
   void Assign( const ImageStatistics& x )
   {
      (void)operator =( x );
   }

   /*!
    * Returns a constant reference to the statistical data in this object.
    */
   const Data& GetData() const
   {
      return m_data;
   }

   /*!
    * Returns the total number of evaluated samples.
    */
   size_type Count() const
   {
      return m_data.count;
   }

   /*!
    * Returns the arithmetic mean of the evaluated sample values in the
    * normalized [0,1] range.
    */
   double Mean() const
   {
      return m_data.mean;
   }

   /*!
    * Returns the sum of squares for the evaluated sample values. Samples are
    * evaluated in the normalized [0,1] range.
    */
   double SumOfSquares() const
   {
      return m_data.sumOfSquares;
   }

   /*!
    * Returns the median of the evaluated sample values in the normalized [0,1]
    * range.
    */
   double Median() const
   {
      return m_data.median;
   }

   /*!
    * Returns the variance from the mean of the evaluated sample valuess in the
    * normalized [0,1] range.
    */
   double Variance() const
   {
      return m_data.variance;
   }

   /*!
    * Returns the variance from the mean of the evaluated sample valuess in the
    * normalized [0,1] range.
    *
    * This member function is a synonym for Variance().
    */
   double Var() const
   {
      return Variance();
   }

   /*!
    * Returns the standard deviation from the mean of the evaluated sample
    * values in the normalized [0,1] range.
    */
   double StandardDeviation() const
   {
      return m_data.stdDev;
   }

   /*!
    * Returns the standard deviation from the mean of the evaluated sample
    * values in the normalized [0,1] range.
    *
    * This member function is a synonym for StandardDeviation().
    */
   double StdDev() const
   {
      return StandardDeviation();
   }

   /*!
    * Returns the average deviation from the median of the evaluated sample
    * values in the normalized [0,1] range.
    *
    * The average deviation is an estimator of the variability of a
    * distribution around its central value. In the case of a deep sky image,
    * the average deviation provides a good estimation of the width of the
    * main histogram peak.
    */
   double AverageDeviation() const
   {
      return m_data.avgDev;
   }

   /*!
    * Returns the average deviation from the median of the evaluated sample
    * values in the normalized [0,1] range.
    *
    * This member function is a synonym for AverageDeviation().
    */
   double AvgDev() const
   {
      return AverageDeviation();
   }

   /*!
    * Returns the median absolute deviation from the median (MAD) of the
    * evaluated sample values in the normalized [0,1] range.
    *
    * The MAD is a well-known robust estimator of dispersion.
    */
   double MAD() const
   {
      return m_data.MAD;
   }

   /*!
    * Returns the biweight midvariance of the evaluated pixel sample values in
    * the normalized [0,1] range.
    *
    * The square root of the biweight midvariance is a robust estimator of
    * scale. It is an efficient estimator with respect to many statistical
    * distributions (about 87% Gaussian efficiency), and appears to have a
    * breakdown point close to 0.5 (the best possible breakdown).
    */
   double BiweightMidvariance() const
   {
      return m_data.bwmv;
   }

   /*!
    * Returns the biweight midvariance of the evaluated pixel sample values in
    * the normalized [0,1] range.
    *
    * This member function is a synonym for BiweightMidvariance().
    */
   double BWMV() const
   {
      return m_data.bwmv;
   }

   /*!
    * Returns the percentage bend midvariance of the evaluated pixel sample
    * values in the normalized [0,1] range.
    *
    * The square root of the percentage bend midvariance is a robust estimator
    * of scale. As implemented in %ImageStatistics (beta=0.2), its Gaussian
    * efficiency is 67%.
    */
   double BendMidvariance() const
   {
      return m_data.pbmv;
   }

   /*!
    * Returns the percentage bend midvariance of the evaluated pixel sample
    * values in the normalized [0,1] range.
    *
    * This member function is a synonym for BendMidvariance().
    */
   double PBMV() const
   {
      return m_data.pbmv;
   }

   /*!
    * Returns the Sn scale estimator of Rousseeuw and Croux in the normalized
    * [0,1] range.
    *
    * Sn is a robust estimator of scale (dispersion). It has the same outlier
    * rejection properties as the MAD but its Gaussian efficiency is higher
    * (58%; the efficiency of MAD is only 37%). Unlike the MAD, Sn does not
    * measure variability around a central value, but between pairs of data
    * points. This makes Sn a potentially better estimator for asymmetrical or
    * skewed distributions.
    */
   double Sn() const
   {
      return m_data.Sn;
   }

   /*!
    * Returns the Qn scale estimator of Rousseeuw and Croux in the normalized
    * [0,1] range.
    *
    * Qn is a robust estimator of scale (dispersion). It has the same outlier
    * rejection properties as the MAD, but its Gaussian efficiency is higher
    * (87%; the efficiency of MAD is only 37%). Unlike the MAD, Qn does not
    * measure variability around a central value, but between pairs of data
    * points. This makes Qn a potentially better estimator for asymmetrical or
    * skewed distributions.
    */
   double Qn() const
   {
      return m_data.Qn;
   }

   /*!
    * Returns the minimum evaluated sample value in the normalized [0,1] range.
    */
   double Minimum() const
   {
      return m_data.minimum;
   }

   /*!
    * Returns the minimum evaluated sample value in the normalized [0,1] range.
    *
    * This member function is a synonym for Maximum().
    */
   double Min() const
   {
      return Minimum();
   }

   /*!
    * Returns the position (x,y coordinates) of the first occurrence of the
    * minimum evaluated sample value.
    */
   Point MinimumPosition() const
   {
      return m_data.minPos;
   }

   /*!
    * Returns the position (x,y coordinates) of the first occurrence of the
    * minimum evaluated sample value.
    *
    * This member function is a synonym for MinimumPosition().
    */
   Point MinPos() const
   {
      return MinimumPosition();
   }

   /*!
    * Returns the maximum evaluated sample value in the normalized [0,1] range.
    */
   double Maximum() const
   {
      return m_data.maximum;
   }

   /*!
    * Returns the maximum evaluated sample value in the normalized [0,1] range.
    *
    * This member function is a synonym for Maximum().
    */
   double Max() const
   {
      return Maximum();
   }

   /*!
    * Returns the position (x,y coordinates) of the first occurrence of the
    * maximum evaluated sample value.
    */
   Point MaximumPosition() const
   {
      return m_data.maxPos;
   }

   /*!
    * Returns the position (x,y coordinates) of the first occurrence of the
    * maximum evaluated sample value.
    *
    * This member function is a synonym for MaximumPosition().
    */
   Point MaxPos() const
   {
      return MaximumPosition();
   }

   /*!
    * Returns the low sample rejection limit in the [0,1] range.
    *
    * When low sample rejection is enabled, sample values less than or equal to
    * the value returned by this function will be ignored during statistics
    * calculation.
    */
   double RejectionLow() const
   {
      return m_data.low;
   }

   /*!
    * Returns the high sample rejection limit in the [0,1] range.
    *
    * When high sample rejection is enabled, sample values greater than or
    * equal to the value returned by this function will be ignored during
    * statistics calculation.
    */
   double RejectionHigh() const
   {
      return m_data.high;
   }

   /*!
    * Sets the \a low and \a high rejection limits in the [0,1] range.
    *
    * When sample rejection is enabled, sample values inside the ranges [0,low]
    * and [high,1] will be ignored during statistics calculation.
    */
   void SetRejectionLimits( double low, double high )
   {
      m_data.low = Range( low, 0.0, 1.0 );
      m_data.high = Range( high, 0.0, 1.0 );
      if ( m_data.high < m_data.low )
         pcl::Swap( m_data.low, m_data.high );
   }

   /*!
    * Returns true iff low sample rejection is enabled in this %ImageStatistics
    * object.
    *
    * When low sample rejection is enabled, sample values less than or equal to
    * the <em>low sample rejection limit</em> will be ignored during statistics
    * calculation.
    */
   bool IsLowRejectionEnabled() const
   {
      return m_data.rejectLow;
   }

   /*!
    * Returns true iff high sample rejection is enabled in this %ImageStatistics
    * object.
    *
    * When high sample rejection is enabled, sample values greater than or
    * equal to the <em>high sample rejection limit</em> will be ignored during
    * statistics calculation.
    */
   bool IsHighRejectionEnabled() const
   {
      return m_data.rejectHigh;
   }

   /*!
    * Enables low and/or high sample rejections.
    *
    * When sample rejection is enabled, sample values inside the ranges [0,low]
    * and [high,1] will be ignored during statistics calculation (where low and
    * high are the current low and high rejection limits).
    */
   void EnableRejection( bool enableLow = true, bool enableHigh = true )
   {
      m_data.rejectLow = enableLow;
      m_data.rejectHigh = enableHigh;
   }

   /*!
    * Disables low and/or high sample rejection.
    *
    * This is a convenience member function, equivalent to
    * EnableRejection( !disableLow, !disableHigh ).
    */
   void DisableRejection( bool disableLow = true, bool disableHigh = true )
   {
      EnableRejection( !disableLow, !disableHigh );
   }

   /*!
    * Returns true iff calculation of extreme pixel sample values is currently
    * enabled for this instance of %ImageStatistics.
    */
   bool IsExtremesEnabled() const
   {
      return !m_data.noExtremes;
   }

   /*!
    * Enables calculation of extreme pixel sample values for this
    * %ImageStatistics object.
    */
   void EnableExtremes( bool enable = true )
   {
      m_data.noExtremes = !enable;
   }

   /*!
    * Disables calculation of extreme pixel sample values for this
    * %ImageStatistics object.
    */
   void DisableExtremes( bool disable = true )
   {
      m_data.noExtremes = disable;
   }

   /*!
    * Returns true iff calculation of the arithmetic mean is currently enabled
    * for this instance of %ImageStatistics.
    *
    * When arithmetic mean calculation is disabled, the variance (and hence the
    * standard deviation) is also disabled implicitly.
    */
   bool IsMeanEnabled() const
   {
      return !m_data.noMean;
   }

   /*!
    * Enables calculation of the arithmetic mean for this %ImageStatistics
    * object.
    *
    * When arithmetic mean calculation is disabled, the variance (and hence the
    * standard deviation) is also disabled implicitly.
    */
   void EnableMean( bool enable = true )
   {
      m_data.noMean = !enable;
   }

   /*!
    * Disables calculation of the arithmetic mean for this %ImageStatistics
    * object.
    *
    * When arithmetic mean calculation is disabled, the variance (and hence the
    * standard deviation) is also disabled implicitly.
    */
   void DisableMean( bool disable = true )
   {
      m_data.noMean = disable;
   }

   /*!
    * Returns true iff calculation of the sum of squares is currently enabled
    * for this instance of %ImageStatistics.
    */
   bool IsSumOfSquaresEnabled() const
   {
      return !m_data.noSumOfSquares;
   }

   /*!
    * Enables calculation of the sum of squares for this object.
    */
   void EnableSumOfSquares( bool enable = true )
   {
      m_data.noSumOfSquares = !enable;
   }

   /*!
    * Disables calculation of the sum of squares for this object.
    */
   void DisableSumOfSquares( bool disable = true )
   {
      m_data.noSumOfSquares = disable;
   }

   /*!
    * Returns true iff calculation of the variance is currently enabled for
    * this instance of %ImageStatistics.
    */
   bool IsVarianceEnabled() const
   {
      return !m_data.noVariance;
   }

   /*!
    * Enables calculation of the variance for this %ImageStatistics object.
    */
   void EnableVariance( bool enable = true )
   {
      m_data.noVariance = !enable;
   }

   /*!
    * Disables calculation of the variance for this %ImageStatistics object.
    */
   void DisableVariance( bool disable = true )
   {
      m_data.noVariance = disable;
   }

   /*!
    * Returns true iff calculation of the median is currently enabled for this
    * instance of %ImageStatistics.
    *
    * When median calculation is disabled, the average deviation, MAD and
    * biweight midvariance estimators are also disabled implicitly.
    */
   bool IsMedianEnabled() const
   {
      return !m_data.noMedian;
   }

   /*!
    * Enables calculation of the median for this %ImageStatistics object.
    *
    * When median calculation is disabled, the average deviation, MAD, biweight
    * midvariance and bend midvariance estimators are also disabled implicitly.
    */
   void EnableMedian( bool enable = true )
   {
      m_data.noMedian = !enable;
   }

   /*!
    * Disables calculation of the median for this %ImageStatistics object.
    *
    * When median calculation is disabled, the average deviation, MAD, biweight
    * midvariance and bend midvariance estimators are also disabled implicitly.
    */
   void DisableMedian( bool disable = true )
   {
      m_data.noMedian = disable;
   }

   /*!
    * Returns true iff calculation of the average deviation from the median is
    * currently enabled for this instance of %ImageStatistics.
    */
   bool IsAvgDevEnabled() const
   {
      return !m_data.noAvgDev;
   }

   /*!
    * Enables calculation of the average deviation from the median for this
    * %ImageStatistics object.
    */
   void EnableAvgDev( bool enable = true )
   {
      m_data.noAvgDev = !enable;
   }

   /*!
    * Disables calculation of the average deviation from the median for this
    * %ImageStatistics object.
    */
   void DisableAvgDev( bool disable = true )
   {
      m_data.noAvgDev = disable;
   }

   /*!
    * Returns true iff calculation of the median deviation from the median (MAD)
    * is currently enabled for this instance of %ImageStatistics.
    *
    * When MAD calculation is disabled, the biweight midvariance estimator is
    * also disabled implicitly.
    */
   bool IsMADEnabled() const
   {
      return !m_data.noMAD;
   }

   /*!
    * Enables calculation of the median deviation from the median (MAD) for
    * this %ImageStatistics object.
    *
    * When MAD calculation is disabled, the biweight midvariance estimator is
    * also disabled implicitly.
    */
   void EnableMAD( bool enable = true )
   {
      m_data.noMAD = !enable;
   }

   /*!
    * Disables calculation of the median deviation from the median (MAD) for
    * this %ImageStatistics object.
    *
    * When MAD calculation is disabled, the biweight midvariance estimator is
    * also disabled implicitly.
    */
   void DisableMAD( bool disable = true )
   {
      m_data.noMAD = disable;
   }

   /*!
    * Returns true iff calculation of the biweight midvariance (BWMV) is
    * currently enabled for this instance of %ImageStatistics.
    */
   bool IsBWMVEnabled() const
   {
      return !m_data.noBWMV;
   }

   /*!
    * Enables calculation of the biweight midvariance (BWMV) for this
    * %ImageStatistics object.
    */
   void EnableBWMV( bool enable = true )
   {
      m_data.noBWMV = !enable;
   }

   /*!
    * Disables calculation of the biweight midvariance (BWMV) for this
    * %ImageStatistics object.
    */
   void DisableBWMV( bool disable = true )
   {
      m_data.noBWMV = disable;
   }

   /*!
    * Returns true iff calculation of the percentage bend midvariance (PBMV) is
    * currently enabled for this instance of %ImageStatistics.
    */
   bool IsPBMVEnabled() const
   {
      return !m_data.noPBMV;
   }

   /*!
    * Enables calculation of the percentage bend midvariance (PBMV) for this
    * %ImageStatistics object.
    */
   void EnablePBMV( bool enable = true )
   {
      m_data.noPBMV = !enable;
   }

   /*!
    * Disables calculation of the percentage bend midvariance (PBMV) for this
    * %ImageStatistics object.
    */
   void DisablePBMV( bool disable = true )
   {
      m_data.noPBMV = disable;
   }

   /*!
    * Returns true iff calculation of the Rousseeuw/Croux Sn estimator of scale
    * is currently enabled for this instance of %ImageStatistics.
    *
    * \note The Sn estimator is disabled by default.
    */
   bool IsSnEnabled() const
   {
      return !m_data.noSn;
   }

   /*!
    * Enables calculation of the Rousseeuw/Croux Sn estimator of scale for
    * this %ImageStatistics object.
    *
    * \note The Sn estimator is disabled by default.
    */
   void EnableSn( bool enable = true )
   {
      m_data.noSn = !enable;
   }

   /*!
    * Disables calculation of the Rousseeuw/Croux Sn estimator of scale for
    * this %ImageStatistics object.
    *
    * \note The Sn estimator is disabled by default.
    */
   void DisableSn( bool disable = true )
   {
      m_data.noSn = disable;
   }

   /*!
    * Returns true iff calculation of the Rousseeuw/Croux Qn estimator of scale
    * is currently enabled for this instance of %ImageStatistics.
    *
    * \note The Qn estimator is disabled by default.
    */
   bool IsQnEnabled() const
   {
      return !m_data.noQn;
   }

   /*!
    * Enables calculation of the Rousseeuw/Croux Qn estimator of scale for
    * this %ImageStatistics object.
    *
    * \note The Qn estimator is disabled by default.
    */
   void EnableQn( bool enable = true )
   {
      m_data.noQn = !enable;
   }

   /*!
    * Disables calculation of the Rousseeuw/Croux Qn estimator of scale for
    * this %ImageStatistics object.
    *
    * \note The Qn estimator is disabled by default.
    */
   void DisableQn( bool disable = true )
   {
      m_data.noQn = disable;
   }

   /*!
    * Calculates statistical data for a 32-bit floating point image. Returns a
    * reference to the source \a image.
    */
   const Image& operator <<( const Image& image );

   /*!
    * Calculates statistical data for a 64-bit floating point image. Returns a
    * reference to the source \a image.
    */
   const DImage& operator <<( const DImage& image );

   /*!
    * Calculates statistical data for an 8-bit unsigned integer image. Returns
    * a reference to the source \a image.
    */
   const UInt8Image& operator <<( const UInt8Image& image );

   /*!
    * Calculates statistical data for a 16-bit unsigned integer image. Returns
    * a reference to the source \a image.
    */
   const UInt16Image& operator <<( const UInt16Image& image );

   /*!
    * Calculates statistical data for a 32-bit unsigned integer image. Returns
    * a reference to the source \a image.
    */
   const UInt32Image& operator <<( const UInt32Image& image );

   /*!
    * Calculates statistical data for an image transported by an ImageVariant
    * instance. Returns a reference to the source \a image.
    */
   const ImageVariant& operator <<( const ImageVariant& image )
   {
      if ( image )
         if ( !image.IsComplexSample() )
            if ( image.IsFloatSample() )
               switch ( image.BitsPerSample() )
               {
               case 32: *this << static_cast<const Image&>( *image ); break;
               case 64: *this << static_cast<const DImage&>( *image ); break;
               }
            else
               switch ( image.BitsPerSample() )
               {
               case  8: *this << static_cast<const UInt8Image&>( *image ); break;
               case 16: *this << static_cast<const UInt16Image&>( *image ); break;
               case 32: *this << static_cast<const UInt32Image&>( *image ); break;
               }
      return image;
   }

protected:

   Data m_data; // statistical data

   friend class View;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageStatistics_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageStatistics.h - Released 2018-11-01T11:06:36Z
