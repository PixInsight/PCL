//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/ImageStatistics.cpp - Released 2015/12/17 18:52:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/ImageStatistics.h>
#include <pcl/Matrix.h>
#include <pcl/Selection.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_ImageStatisticsEngine
{
public:

   template <class P> static
   void Compute( const GenericImage<P>& image, ImageStatistics::Data& data, bool /*parallel*/, int /*maxProcessors*/ )
   {
      data.AssignStatisticalData( ImageStatistics::Data() );

      size_type N = image.NumberOfSelectedPixels();
      if ( N == 0 )
         return;

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Computing image statistics", N );

      size_type NS = N/8;
      size_type NN = N - 7*NS;

      Rect rect = image.SelectedRectangle();
      int channel = image.SelectedChannel();

      data.minimum = data.maximum = 0;
      data.minPos = data.maxPos = Point( 0 );

      if ( data.rejectLow || data.rejectHigh )
      {
         // Rejection bounds in the native range
         double s0 = 0, s1 = 0;
         if ( data.rejectLow )
            s0 = data.low * P::MaxSampleValue();
         if ( data.rejectHigh )
            s1 = data.high * P::MaxSampleValue();

         Array<double> v;
         v.Reserve( N ); // clearly, optimize for speed

         typename GenericImage<P>::const_roi_sample_iterator i( image, rect, channel );
         P::FromSample( data.minimum, *i );
         data.maximum = data.minimum;

         if ( data.noExtremes )
         {
            if ( data.rejectLow )
            {
               if ( data.rejectHigh )
               {
                  for ( ; i; ++i )
                     if ( *i > s0 )
                        if ( *i < s1 )
                        {
                           double f; P::FromSample( f, *i );
                           v.Append( f );
                        }
               }
               else
               {
                  for ( ; i; ++i )
                     if ( *i > s0 )
                     {
                        double f; P::FromSample( f, *i );
                        v.Append( f );
                     }
               }
            }
            else
            {
               for ( ; i; ++i )
                  if ( *i < s1 )
                  {
                     double f; P::FromSample( f, *i );
                     v.Append( f );
                  }
            }
         }
         else // !data.noExtremes
         {
            bool extremesSeen = false;
            if ( data.rejectLow )
            {
               if ( data.rejectHigh )
               {
                  for ( int y = rect.y0; y < rect.y1; ++y )
                     for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                        if ( *i > s0 )
                           if ( *i < s1 )
                           {
                              double f; P::FromSample( f, *i );
                              v.Append( f );

                              if ( extremesSeen )
                              {
                                 if ( f < data.minimum )
                                 {
                                    data.minimum = f;
                                    data.minPos.x = x;
                                    data.minPos.y = y;
                                 }
                                 else if ( f > data.maximum )
                                 {
                                    data.maximum = f;
                                    data.maxPos.x = x;
                                    data.maxPos.y = y;
                                 }
                              }
                              else
                              {
                                 data.minimum = data.maximum = f;
                                 data.minPos.x = data.maxPos.x = x;
                                 data.minPos.y = data.maxPos.y = y;
                                 extremesSeen = true;
                              }
                           }
               }
               else
               {
                  for ( int y = rect.y0; y < rect.y1; ++y )
                     for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                        if ( *i > s0 )
                        {
                           double f; P::FromSample( f, *i );
                           v.Append( f );

                           if ( extremesSeen )
                           {
                              if ( f < data.minimum )
                              {
                                 data.minimum = f;
                                 data.minPos.x = x;
                                 data.minPos.y = y;
                              }
                              else if ( f > data.maximum )
                              {
                                 data.maximum = f;
                                 data.maxPos.x = x;
                                 data.maxPos.y = y;
                              }
                           }
                           else
                           {
                              data.minimum = data.maximum = f;
                              data.minPos.x = data.maxPos.x = x;
                              data.minPos.y = data.maxPos.y = y;
                              extremesSeen = true;
                           }
                        }
               }
            }
            else
            {
               for ( int y = rect.y0; y < rect.y1; ++y )
                  for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                     if ( *i < s1 )
                     {
                        double f; P::FromSample( f, *i );
                        v.Append( f );

                        if ( extremesSeen )
                        {
                           if ( f < data.minimum )
                           {
                              data.minimum = f;
                              data.minPos.x = x;
                              data.minPos.y = y;
                           }
                           else if ( f > data.maximum )
                           {
                              data.maximum = f;
                              data.maxPos.x = x;
                              data.maxPos.y = y;
                           }
                        }
                        else
                        {
                           data.minimum = data.maximum = f;
                           data.minPos.x = data.maxPos.x = x;
                           data.minPos.y = data.maxPos.y = y;
                           extremesSeen = true;
                        }
                     }
            }
         }

         data.count = v.Length();

         if ( !data.noSumOfSquares )
            data.sumOfSquares = pcl::SumOfSquares( v.Begin(), v.End() );

         image.Status() += NS;

         if ( !data.noMean )
         {
            data.mean = pcl::Mean( v.Begin(), v.End() );

            image.Status() += NS;

            if ( !data.noVariance )
            {
               data.variance = pcl::Variance( v.Begin(), v.End(), data.mean );
               data.stdDev = Sqrt( data.variance );
            }

            image.Status() += NS;
         }
         else
         {
            image.Status() += 2*NS;
         }

         if ( !data.noMedian )
         {
            data.median = pcl::Median( v.Begin(), v.End() );

            image.Status() += NS;

            if ( !data.noAvgDev )
               data.avgDev = pcl::AvgDev( v.Begin(), v.End(), data.median );

            image.Status() += NS;

            if ( !data.noMAD )
            {
               data.MAD = pcl::MAD( v.Begin(), v.End(), data.median );

               if ( !data.noBWMV )
                  data.bwmv = pcl::BiweightMidvariance( v.Begin(), v.End(), data.median, data.MAD );
            }

            if ( !data.noPBMV )
               data.pbmv = pcl::BendMidvariance( v.Begin(), v.End(), data.median, 0.2 );

            image.Status() += NS;
         }
         else
         {
            image.Status() += 3*NS;
         }

         if ( !data.noSn )
            data.Sn = pcl::Sn( v.Begin(), v.End() );

         image.Status() += NS;

         if ( !data.noQn )
            data.Qn = pcl::Qn( v.Begin(), v.End() );

         image.Status() += NN;
      }
      else
      {
         data.count = N;

         DMatrix V( image, rect, channel );

         if ( !data.noExtremes )
         {
            double* i = V.Begin();
            data.minimum = data.maximum = *i;
            for ( int y = rect.y0; y < rect.y1; ++y )
               for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                  if ( *i < data.minimum )
                  {
                     data.minimum = *i;
                     data.minPos.x = x;
                     data.minPos.y = y;
                  }
                  else if ( *i > data.maximum )
                  {
                     data.maximum = *i;
                     data.maxPos.x = x;
                     data.maxPos.y = y;
                  }
         }

         if ( !data.noSumOfSquares )
            data.sumOfSquares = pcl::SumOfSquares( V.Begin(), V.End() );

         image.Status() += NS;

         if ( !data.noMean )
         {
            data.mean = pcl::Mean( V.Begin(), V.End() );

            image.Status() += NS;

            if ( !data.noVariance )
            {
               data.variance = pcl::Variance( V.Begin(), V.End(), data.mean );
               data.stdDev = Sqrt( data.variance );
            }

            image.Status() += NS;
         }
         else
         {
            image.Status() += 2*NS;
         }

         if ( !data.noMedian )
         {
            data.median = pcl::Median( V.Begin(), V.End() );

            image.Status() += NS;

            if ( !data.noAvgDev )
               data.avgDev = pcl::AvgDev( V.Begin(), V.End(), data.median );

            image.Status() += NS;

            if ( !data.noMAD )
            {
               data.MAD = pcl::MAD( V.Begin(), V.End(), data.median );

               if ( !data.noBWMV )
                  data.bwmv = pcl::BiweightMidvariance( V.Begin(), V.End(), data.median, data.MAD );
            }

            if ( !data.noPBMV )
               data.pbmv = pcl::BendMidvariance( V.Begin(), V.End(), data.median, 0.2 );

            image.Status() += NS;
         }
         else
         {
            image.Status() += 3*NS;
         }

         if ( !data.noSn )
            data.Sn = pcl::Sn( V.Begin(), V.End() );

         image.Status() += NS;

         if ( !data.noQn )
            data.Qn = pcl::Qn( V.Begin(), V.End() );

         image.Status() += NN;
      }
   }
};

// ----------------------------------------------------------------------------

const Image& ImageStatistics::operator <<( const Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const DImage& ImageStatistics::operator <<( const DImage& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const UInt8Image& ImageStatistics::operator <<( const UInt8Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const UInt16Image& ImageStatistics::operator <<( const UInt16Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const UInt32Image& ImageStatistics::operator <<( const UInt32Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageStatistics.cpp - Released 2015/12/17 18:52:18 UTC
