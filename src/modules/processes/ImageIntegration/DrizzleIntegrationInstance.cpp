//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0282
// ----------------------------------------------------------------------------
// DrizzleIntegrationInstance.cpp - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "DrizzleIntegrationInstance.h"

#include "../ImageRegistration/Homography.h"

#include <pcl/DrizzleDataDecoder.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Version.h>
#include <pcl/View.h>

#define DRIZZLE_RESOLUTION 5

namespace pcl
{

// ----------------------------------------------------------------------------

DrizzleIntegrationInstance::DrizzleIntegrationInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_inputData(),
p_inputHints(),
p_inputDirectory(),
p_scale( TheDZScaleParameter->DefaultValue() ),
p_dropShrink( TheDZDropShrinkParameter->DefaultValue() ),
p_enableRejection( TheDZEnableRejectionParameter->DefaultValue() ),
p_enableImageWeighting( TheDZEnableImageWeightingParameter->DefaultValue() ),
p_enableSurfaceSplines( TheDZEnableSurfaceSplinesParameter->DefaultValue() ),
p_useROI( TheDZUseROIParameter->DefaultValue() ),
p_roi( 0 ),
p_closePreviousImages( TheDZClosePreviousImagesParameter->DefaultValue() ),
p_noGUIMessages( TheDZNoGUIMessagesParameter->DefaultValue() ),
p_onError( DZOnError::Default ),
o_output()
{
}

DrizzleIntegrationInstance::DrizzleIntegrationInstance( const DrizzleIntegrationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void DrizzleIntegrationInstance::Assign( const ProcessImplementation& p )
{
   const DrizzleIntegrationInstance* x = dynamic_cast<const DrizzleIntegrationInstance*>( &p );
   if ( x != 0 )
   {
      p_inputData            = x->p_inputData;
      p_inputHints           = x->p_inputHints;
      p_inputDirectory       = x->p_inputDirectory;
      p_scale                = x->p_scale;
      p_dropShrink           = x->p_dropShrink;
      p_enableRejection      = x->p_enableRejection;
      p_enableImageWeighting = x->p_enableImageWeighting;
      p_enableSurfaceSplines = x->p_enableSurfaceSplines;
      p_useROI               = x->p_useROI;
      p_roi                  = x->p_roi;
      p_closePreviousImages  = x->p_closePreviousImages;
      p_noGUIMessages        = x->p_noGUIMessages;
      p_onError              = x->p_onError;
      o_output               = x->o_output;
   }
}

bool DrizzleIntegrationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "DrizzleIntegration can only be executed in the global context.";
   return false;
}

bool DrizzleIntegrationInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

bool DrizzleIntegrationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_inputData.Length() < 1 )
   {
      whyNot = "This instance of DrizzleIntegration has no input data items.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

class DrizzleIntegrationEngine
{
public:

   DrizzleIntegrationEngine( DrizzleIntegrationInstance& instance ) : m_instance( instance )
   {
      Clear();
   }

   ~DrizzleIntegrationEngine()
   {
      Clear();
   }

   void Perform();

   void Clear()
   {
      m_referenceWidth = m_referenceHeight = m_width = m_height = m_numberOfChannels = 0;
      m_pixelSize = 0;
      m_decoder.Clear();
   }

private:

   typedef DrizzleIntegrationInstance::input_data_list   input_data_list;

   DrizzleIntegrationInstance& m_instance;
   DrizzleDataDecoder          m_decoder;    // current drizzle data
   int                         m_referenceWidth;
   int                         m_referenceHeight;
   Point                       m_origin;    // output ROI origin
   int                         m_width;     // output ROI dimensions in pixels
   int                         m_height;
   int                         m_numberOfChannels;
   double                      m_pixelSize; // in reference pixel units

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const DrizzleIntegrationEngine& a_engine,
                  const Image& a_source, Image& a_result, Image& a_weight,
                  const StatusMonitor& monitor, size_type count ) :
      AbstractImage::ThreadData( monitor, count ),
      engine( a_engine ), source( a_source ), result( a_result ), weight( a_weight )
      {
      }

      const DrizzleIntegrationEngine& engine;
      const Image&                    source;
            Image&                    result;
            Image&                    weight;
            Homography                H;
            PointGridInterpolation    G;
            double                    dropDelta0;
            double                    dropDelta1;
            bool                      splines;
            bool                      rejection;
            bool                      perChannelRejection;
   };

   class DrizzleThread : public Thread
   {
   public:

      double totalDropArea; // total data gathered by this thread in drop area units

      DrizzleThread( const ThreadData& data, int firstRow, int endRow ) :
         Thread(),
         totalDropArea( 0 ),
         m_data( data ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         totalDropArea = 0;

         Image::pixel_iterator r( m_data.result );
         r.MoveBy( 0, m_firstRow );
         Image::pixel_iterator w( m_data.weight );
         w.MoveBy( 0, m_firstRow );

         for ( int y = m_firstRow; y < m_endRow; ++y )
         {
            DRect referenceRect;
            referenceRect.y0 = (y + m_data.engine.m_origin.y) * m_data.engine.m_pixelSize;
            referenceRect.y1 = referenceRect.y0 + m_data.engine.m_pixelSize;

            Point referencePixel;
            if ( m_data.rejection )
            {
               referencePixel.y = TruncInt( referenceRect.y0 + 0.5 );
               if ( referencePixel.y >= m_data.engine.m_referenceHeight )
                  referencePixel.y = m_data.engine.m_referenceHeight-1;
            }

            for ( int x = 0; x < m_data.engine.m_width; ++x, ++r, ++w )
            {
               referenceRect.x0 = (x + m_data.engine.m_origin.x) * m_data.engine.m_pixelSize;

               if ( m_data.rejection )
               {
                  referencePixel.x = TruncInt( referenceRect.x0 + 0.5 );
                  if ( referencePixel.x >= m_data.engine.m_referenceWidth )
                     referencePixel.x = m_data.engine.m_referenceWidth-1;
                  if ( m_data.engine.Reject( referencePixel ) )
                     continue;
               }

               referenceRect.x1 = referenceRect.x0 + m_data.engine.m_pixelSize;

               DPoint sourceP0, sourceP1, sourceP2, sourceP3;
               if ( m_data.splines )
               {
                  sourceP0 = m_data.G( referenceRect.x0, referenceRect.y1 );
                  sourceP1 = m_data.G( referenceRect.x0, referenceRect.y0 );
                  sourceP2 = m_data.G( referenceRect.x1, referenceRect.y0 );
                  sourceP3 = m_data.G( referenceRect.x1, referenceRect.y1 );
               }
               else
               {
                  sourceP0 = m_data.H( referenceRect.x0, referenceRect.y1 );
                  sourceP1 = m_data.H( referenceRect.x0, referenceRect.y0 );
                  sourceP2 = m_data.H( referenceRect.x1, referenceRect.y0 );
                  sourceP3 = m_data.H( referenceRect.x1, referenceRect.y1 );
               }

               sourceP0.MoveBy( 0.5 );
               sourceP1.MoveBy( 0.5 );
               sourceP2.MoveBy( 0.5 );
               sourceP3.MoveBy( 0.5 );

               sourceP0.Round( DRIZZLE_RESOLUTION );
               sourceP1.Round( DRIZZLE_RESOLUTION );
               sourceP2.Round( DRIZZLE_RESOLUTION );
               sourceP3.Round( DRIZZLE_RESOLUTION );

               DRect sourceBounds( Min( Min( Min( sourceP0.x, sourceP1.x ), sourceP2.x ), sourceP3.x ) + 1.0e-5,
                                   Min( Min( Min( sourceP0.y, sourceP1.y ), sourceP2.y ), sourceP3.y ) + 1.0e-5,
                                   Max( Max( Max( sourceP0.x, sourceP1.x ), sourceP2.x ), sourceP3.x ) - 1.0e-5,
                                   Max( Max( Max( sourceP0.y, sourceP1.y ), sourceP2.y ), sourceP3.y ) - 1.0e-5 );

               Array<Point> sourcePixels;
               {
                  Rect b = sourceBounds.TruncatedToInt();
                  for ( int y = b.y0; y <= b.y1; ++y )
                     if ( y >= 0 )
                        if ( y < m_data.source.Height() )
                           for ( int x = b.x0; x <= b.x1; ++x )
                              if ( x >= 0 )
                                 if ( x < m_data.source.Width() )
                                    sourcePixels.Append( Point( x, y ) );
               }

               for ( Array<Point>::const_iterator p = sourcePixels.Begin(); p != sourcePixels.End(); ++p )
               {
                  DRect dropRect( p->x + m_data.dropDelta0,
                                  p->y + m_data.dropDelta0,
                                  p->x + m_data.dropDelta1,
                                  p->y + m_data.dropDelta1 );

                  dropRect.Round( DRIZZLE_RESOLUTION );

                  if ( CanRectsIntersect( dropRect, sourceBounds ) )
                  {
                     double area;
                     if ( GetAreaOfIntersectionOfQuadAndRect( area, dropRect, sourceP0, sourceP1, sourceP2, sourceP3 ) )
                     {
                        for ( int c = 0; c < m_data.engine.m_numberOfChannels; ++c )
                        {
                           double value = m_data.source( *p, c );
                           if ( 1 + value != 1 )
                              if ( !m_data.perChannelRejection || !m_data.engine.Reject( referencePixel, c ) )
                              {
                                 double normalizedValue = (value - m_data.engine.Location( c ))
                                                         * m_data.engine.Scale( c )
                                                         + m_data.engine.ReferenceLocation( c );

                                 double weightedArea = area * m_data.engine.Weight( c );

                                 r[c] += weightedArea * normalizedValue;
                                 w[c] += weightedArea;
                              }
                        }

                        totalDropArea += area;
                     }
                  }
               }
            }

            UPDATE_THREAD_MONITOR( 1 )
         }
      }

   private:

      const ThreadData& m_data;
            int         m_firstRow, m_endRow;
   };

   /*
    * A functional class for sorting a set of points in clockwise direction.
    */
   class PointsClockwisePredicate
   {
   public:

      PointsClockwisePredicate( const Array<DPoint>& P ) : c( 0 )
      {
         /*
          * Compute the polygon's barycenter.
          */
         PCL_PRECONDITION( P.Length() > 2 )
         int n = P.Length();
         for ( int i = 0; i < n; ++i )
            c += P[i];
         c /= n;
      }

      bool operator()( const DPoint& a, const DPoint& b ) const
      {
         /*
          * Points on different sides of the vertical line passing through c.
          *
          * - a to the right and b to the left: a < b
          * - a to the left and b to the right: a > b
          */
         if ( a.x >= c.x )
         {
            if ( b.x < c.x )
               return true;
         }
         else
         {
            if ( b.x >= c.x )
               return false;
         }

         /*
          * If the points are not collinear, sort in clockwise direction.
          *
          * d:  > 0 if b is to the left of the line c-a
          *     < 0 if b is to the right of the line c-a
          *    == 0 if b is on the line c-a
          */
         double d = (a.x - c.x)*(b.y - c.y) - (b.x - c.x)*(a.y - c.y);
         if ( d != 0 )
            return d < 0;

         /*
          * Sort collinear points by their distance to the center.
          */
         double dxa = a.x - c.x;
         double dxb = b.x - c.x;
         if ( dxa != dxb )
            return Abs( dxa ) < Abs( dxb );
         double dya = a.y - c.y;
         double dyb = b.y - c.y;
         return Abs( dya ) < Abs( dyb );
      }

   private:

      DPoint c; // barycenter
   };

   static bool CanRectsIntersect( const DRect& r, const DRect& s )
   {
      return s.x0 < r.x1 && s.x1 > r.x0 && s.y0 < r.y1 && s.y1 > r.y0;
   }

   static bool CanSegmentAndRectIntersect( const DPoint& p0, const DPoint& p1, const DRect& r )
   {
      return ((p0.y < p1.y) ? p0.y < r.y1 && p1.y > r.y0 : p1.y < r.y1 && p0.y > r.y0) &&
             ((p0.x < p1.x) ? p0.x < r.x1 && p1.x > r.x0 : p1.x < r.x1 && p0.x > r.x0);
   }

   static bool PointInsideRect( const DPoint& p, const DRect& r )
   {
      return p.x >= r.x0 && p.x <= r.x1 && p.y >= r.y0 && p.y <= r.y1;
   }

   /*
    * Adapted from the Wolfram Demonstrations Project:
    * "An Efficient Test for a Point to Be in a Convex Polygon"
    * http://demonstrations.wolfram.com/AnEfficientTestForAPointToBeInAConvexPolygon/
    * Contributed by Robert Nowak
    */
   static bool PointInsideConvexQuad( const DPoint& p, const DPoint& p0, const DPoint& p1, const DPoint& p2, const DPoint& p3 )
   {
      DPoint q0 = p0 - p;
      DPoint q1 = p1 - p;
      DPoint q2 = p2 - p;
      DPoint q3 = p3 - p;
      return (q1.x*q0.y - q0.x*q1.y < 0) ? q2.x*q1.y - q1.x*q2.y < 0 &&
                                           q3.x*q2.y - q2.x*q3.y < 0 &&
                                           q0.x*q3.y - q3.x*q0.y < 0 :
                                           q2.x*q1.y - q1.x*q2.y >= 0 &&
                                           q3.x*q2.y - q2.x*q3.y >= 0 &&
                                           q0.x*q3.y - q3.x*q0.y >= 0;
   }

   static void GetIntersectionOfSegmentAndHorizontalSegment( Array<DPoint>& P, const DPoint& a, const DPoint& b,
                                                             double x0, double y, double x1 )
   {
      // Fail if the lines are parallel
      if ( a.y == b.y )
         return;

      // Fail if the segment is above or below the horizontal line
      if ( a.y <= y && b.y <= y || a.y > y && b.y > y )
         return;

      // Fail if the segments share an end-point
      if ( (a.x == x0 || a.x == x1) && a.y == y || (b.x == x0 || b.x == x1) && b.y == y )
         return;

      if ( a.x == b.x )
      {
         // Perpendicular lines
         if ( a.x > x0 && a.x < x1 )
            P.Append( DPoint( a.x, y ) );
      }
      else
      {
         double m = (b.y - a.y)/(b.x - a.x);
         double x = (y - a.y + m*a.x)/m;
         if ( x > x0 && x < x1 )
            P.Append( DPoint( x, y ) );
      }
   }

   static void GetIntersectionOfSegmentAndVerticalSegment( Array<DPoint>& P, const DPoint& a, const DPoint& b,
                                                           double x, double y0, double y1 )
   {
      // Fail if the lines are parallel
      if ( a.x == b.x )
         return;

      // Fail if the segment is at one side of the vertical line
      if ( a.x <= x && b.x <= x || a.x > x && b.x > x )
         return;

      // Fail if the segments share an end-point
      if ( (a.y == y0 || a.y == y1) && a.x == x || (b.y == y0 || b.y == y1) && b.x == x )
         return;

      if ( a.y == b.y )
      {
         // Perpendicular lines
         if ( a.y > y0 && a.y < y1 )
            P.Append( DPoint( x, a.y ) );
      }
      else
      {
         double m = (b.y - a.y)/(b.x - a.x);
         double y = m*x + a.y - m*a.x;
         if ( y > y0 && y < y1 )
            P.Append( DPoint( x, y ) );
      }
   }

   static void GetIntersectionsOfSegmentAndRect( Array<DPoint>& P, const DPoint& a, const DPoint& b, const DRect& r )
   {
      GetIntersectionOfSegmentAndHorizontalSegment( P, a, b, r.x0, r.y0, r.x1 );
      GetIntersectionOfSegmentAndHorizontalSegment( P, a, b, r.x0, r.y1, r.x1 );
      GetIntersectionOfSegmentAndVerticalSegment( P, a, b, r.x0, r.y0, r.y1 );
      GetIntersectionOfSegmentAndVerticalSegment( P, a, b, r.x1, r.y0, r.y1 );
   }

   /*
    * Adapted from a public-domain function by Darel Rex Finley, 2006.
    * http://alienryderflex.com/polygon_area/
    */
   static double AreaOfPolygon( const Array<DPoint>& P )
   {
      double s = 0;
      for ( int n = P.Length(), i = 0, j = n-1; i < n; ++i )
      {
         s += (P[j].x + P[i].x)*(P[j].y - P[i].y);
         j = i;
      }
      return ((s < 0) ? -s : s)/2; // s > 0 if points are listed clockwise
   }

   static bool GetAreaOfIntersectionOfQuadAndRect( double& f, const DRect& r,
                                                   const DPoint& p0, const DPoint& p1, const DPoint& p2, const DPoint& p3 )
   {
      Array<DPoint> P;
      if ( CanSegmentAndRectIntersect( p0, p1, r ) )
         GetIntersectionsOfSegmentAndRect( P, p0, p1, r );
      if ( CanSegmentAndRectIntersect( p1, p2, r ) )
         GetIntersectionsOfSegmentAndRect( P, p1, p2, r );
      if ( CanSegmentAndRectIntersect( p2, p3, r ) )
         GetIntersectionsOfSegmentAndRect( P, p2, p3, r );
      if ( CanSegmentAndRectIntersect( p3, p0, r ) )
         GetIntersectionsOfSegmentAndRect( P, p3, p0, r );

      if ( PointInsideRect( p0, r ) )
         P.Append( p0 );
      if ( PointInsideRect( p1, r ) )
         P.Append( p1 );
      if ( PointInsideRect( p2, r ) )
         P.Append( p2 );
      if ( PointInsideRect( p3, r ) )
         P.Append( p3 );

      if ( PointInsideConvexQuad( DPoint( r.x0, r.y0 ), p0, p1, p2, p3 ) )
         P.Append( DPoint( r.x0, r.y0 ) );
      if ( PointInsideConvexQuad( DPoint( r.x0, r.y1 ), p0, p1, p2, p3 ) )
         P.Append( DPoint( r.x0, r.y1 ) );
      if ( PointInsideConvexQuad( DPoint( r.x1, r.y0 ), p0, p1, p2, p3 ) )
         P.Append( DPoint( r.x1, r.y0 ) );
      if ( PointInsideConvexQuad( DPoint( r.x1, r.y1 ), p0, p1, p2, p3 ) )
         P.Append( DPoint( r.x1, r.y1 ) );

      if ( P.Length() < 3 )
         return false;

      // Make sure the intersection is a convex polygon.
      if ( P.Length() > 3 )
         InsertionSort( P.Begin(), P.End(), PointsClockwisePredicate( P ) );

      f = AreaOfPolygon( P );
      return true;
   }

   bool Reject( const Point& p ) const
   {
      // Assume m_decoder.HasRejectionData() == true
      for ( int c = 0; c < m_decoder.RejectionMap().NumberOfChannels(); ++c )
         if ( m_decoder.RejectionMap()( p, c ) != 0 )
            return true;
      return false;
   }

   bool Reject( const Point& p, int c ) const
   {
      // Assume m_decoder.HasRejectionData() == true
      return m_decoder.RejectionMap()( p, c ) != 0;
   }

   const double Location( int c ) const
   {
      return m_decoder.Location()[c];
   }

   const double ReferenceLocation( int c ) const
   {
      return m_decoder.ReferenceLocation()[c];
   }

   const double Scale( int c ) const
   {
      return m_decoder.Scale()[c];
   }

   const double Weight( int c ) const
   {
      if ( m_instance.p_enableImageWeighting )
         return m_decoder.Weight()[c];
      return 1.0;
   }

   ImageWindow CreateImageWindow( const IsoString& id ) const
   {
      if ( m_instance.p_closePreviousImages )
      {
         ImageWindow window = ImageWindow::WindowById( id );
         if ( !window.IsNull() )
            window.Close();
      }

      ImageWindow window( m_width, m_height, m_numberOfChannels,
                          32/*bitsPerSample*/, true/*float*/,
                          m_numberOfChannels >= 3/*color*/, true/*initialState*/, id );
      if ( window.IsNull() )
         throw Error( "Unable to create image window: " + id );
      return window;
   }

   static bool GetKeywordValue( double& value, FileFormatInstance& file, const IsoString& keyName )
   {
      if ( file.Format().CanStoreKeywords() )
      {
         FITSKeywordArray keywords;
         if ( !file.Extract( keywords ) )
            throw CatchedException();
         for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
            if ( !i->name.CompareIC( keyName ) )
            {
               if ( i->IsNumeric() )
                  if ( i->GetNumericValue( value ) )
                     return true;
               break;
            }
      }
      return false;
   }

   void Normalize( ImageVariant& result, ImageVariant& weight ) const
   {
      Image::pixel_iterator r( static_cast<Image&>( *result ) );
      Image::pixel_iterator w( static_cast<Image&>( *weight ) );
      float wm = static_cast<Image&>( *weight ).MaximumSampleValue();
      float s2 = m_instance.p_dropShrink*m_instance.p_dropShrink;
      for ( ; r; ++r, ++w )
         for ( int i = 0; i < result.NumberOfChannels(); ++i )
         {
            float ws = w[i] / s2;
            if ( 1 + ws != 1 )
               if ( (r[i] /= ws) > 1 )
                  r[i] = 1;
            w[i] /= wm;
         }
   }
};

void DrizzleIntegrationEngine::Perform()
{
   Clear();

   ImageWindow resultWindow, weightWindow;
   ImageVariant resultImage, weightImage;
   Image sourceImage;

   try
   {
      Rect roi;
      double totalOutputData = 0;
      bool havePedestal = false;
      bool ignoringPedestal = false;
      float pedestal = 0;

      Console console;

      int count = 0;
      int succeeded = 0;
      int skipped = 0;
      int failed = 0;
      for ( input_data_list::const_iterator i = m_instance.p_inputData.Begin(); i != m_instance.p_inputData.End(); ++i )
      {
         try
         {
            ++count;

            if ( !i->enabled )
            {
               ++skipped;
               continue;
            }

            console.WriteLn( String().Format( "<end><cbr><br>* Parsing drizzle data file %d of %d:",
                                             count, m_instance.p_inputData.Length() ) );
            console.WriteLn( "<raw>" + i->path + "</raw>" );

            if ( !File::Exists( i->path ) )
               throw Error( "No such file: " + i->path );

            {
               IsoString text = File::ReadTextFile( i->path );
               m_decoder.Decode( text );
               if ( !m_instance.p_enableSurfaceSplines || !m_decoder.HasSplines() )
                  if ( !m_decoder.HasMatrix() )
                     throw Error( "Missing alignment matrix definition." );
            }

            if ( m_referenceWidth <= 0 )
            {
               m_referenceWidth = m_decoder.ReferenceWidth();
               m_referenceHeight = m_decoder.ReferenceHeight();

               if ( m_instance.p_useROI )
               {
                  roi = m_instance.p_roi.Intersection( Rect( m_referenceWidth, m_referenceHeight ) );
                  if ( !roi.IsRect() )
                     throw Error( "Empty or invalid ROI defined." );
                  roi.Order();
                  m_origin.x = RoundInt( roi.x0 * m_instance.p_scale );
                  m_origin.y = RoundInt( roi.y0 * m_instance.p_scale );
                  m_width = RoundInt( roi.Width() * m_instance.p_scale );
                  m_height = RoundInt( roi.Height() * m_instance.p_scale );
               }
               else
               {
                  roi = Rect( m_referenceWidth, m_referenceHeight );
                  m_origin = 0;
                  m_width = RoundInt( m_referenceWidth * m_instance.p_scale );
                  m_height = RoundInt( m_referenceHeight * m_instance.p_scale );
               }

               m_numberOfChannels = m_decoder.NumberOfChannels();

               m_pixelSize = 1.0/m_instance.p_scale;

               resultWindow = CreateImageWindow( "drizzle_integration" );
               resultImage = resultWindow.MainView().Image();

               weightWindow = CreateImageWindow( "drizzle_weights" );
               weightImage = weightWindow.MainView().Image();

               console.WriteLn( String().Format(
                           "<end><cbr>Reference dimensions : w=%d h=%d n=%d",
                           m_referenceWidth, m_referenceHeight, m_numberOfChannels ) );
               console.WriteLn( String().Format(
                                    "Input geometry       : x0=%d y0=%d w=%d h=%d",
                           roi.x0, roi.y0, roi.Width(), roi.Height() ) );
               console.WriteLn( String().Format(
                                    "Drizzle geometry     : x0=%d y0=%d w=%d h=%d",
                           m_origin.x, m_origin.y, m_width, m_height ) );
               console.WriteLn();
            }
            else
            {
               if ( m_decoder.ReferenceWidth() != m_referenceWidth ||
                  m_decoder.ReferenceHeight() != m_referenceHeight ||
                  m_decoder.NumberOfChannels() != m_numberOfChannels )
                  throw Error( "Inconsistent image geometry: " + i->path );
            }

            String filePath = m_decoder.FilePath();
            if ( !m_instance.p_inputDirectory.IsEmpty() )
            {
               String nameAndSuffix = File::ExtractNameAndSuffix( filePath );
               filePath = m_instance.p_inputDirectory;
               if ( !filePath.EndsWith( '/' ) )
                  filePath.Append( '/' );
               filePath.Append( nameAndSuffix );
            }

            {
               console.WriteLn( "Loading image:" );
               console.WriteLn( filePath );

               if ( !File::Exists( filePath ) )
                  throw Error( filePath + ": No such file." );

               FileFormat format( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );

               FileFormatInstance file( format );

               ImageDescriptionArray images;

               if ( !file.Open( images, filePath, m_instance.p_inputHints ) )
                  throw CatchedException();

               if ( images.IsEmpty() )
                  throw Error( file.FilePath() + ": Empty image file." );

               if ( images.Length() > 1 )
                  Console().NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in input file.", images.Length()-1 ) );

               if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
                  throw Error( file.FilePath() + ": Invalid or unsupported image." );

               if ( !ignoringPedestal )
               {
                  double thisPedestal = 0;
                  bool hasPedestal = GetKeywordValue( thisPedestal, file, "PEDESTAL" );
                  if ( hasPedestal )
                  {
                     if ( thisPedestal < 0 )
                     {
                        console.WarningLn( String().Format( "** Warning: Illegal PEDESTAL keyword value: %.4g", thisPedestal ) );
                        ignoringPedestal = true;
                     }
                     else if ( havePedestal )
                     {
                        if ( thisPedestal != pedestal )
                        {
                           console.WarningLn( String().Format( "** Warning: Inconsistent PEDESTAL keyword value: %.4g", thisPedestal ) );
                           ignoringPedestal = true;
                        }
                     }
                     else
                     {
                        havePedestal = true;
                        pedestal = thisPedestal;
                     }
                  }
                  else
                  {
                     if ( havePedestal )
                        if ( pedestal != 0 )
                        {
                           console.WarningLn( "** Warning: Missing PEDESTAL keyword" );
                           ignoringPedestal = true;
                        }
                  }
                  if ( ignoringPedestal )
                     console.WarningLn( "** Warning: Ignoring PEDESTAL keyword values." );
               }

               sourceImage = Image( (void*)0, 0, 0 ); // shared image
               if ( !file.ReadImage( sourceImage ) )
                  throw CatchedException();
            }

            console.Write( "<end><cbr>Scale factors : " );
            for ( int c = 0; c < m_numberOfChannels; ++c )
               console.Write( String().Format( " %8.5f", m_decoder.Scale()[c] ) );
            console.Write(       "<br>Zero offset   : " );
            for ( int c = 0; c < m_numberOfChannels; ++c )
               console.Write( String().Format( " %+.6e", m_decoder.ReferenceLocation()[c] - m_decoder.Location()[c] ) );
            if ( m_instance.p_enableImageWeighting )
            {
               console.Write(    "<br>Weight        : " );
               for ( int c = 0; c < m_numberOfChannels; ++c )
                  console.Write( String().Format( " %10.5f", m_decoder.Weight()[c] ) );
               console.WriteLn();
            }

            ProcessInterface::ProcessEvents();

            StandardStatus status;
            StatusMonitor monitor;
            monitor.SetCallback( &status );

            ThreadData threadData( *this,
                                 sourceImage,
                                 static_cast<Image&>( *resultImage ),
                                 static_cast<Image&>( *weightImage ),
                                 monitor, m_height );
            threadData.H = Homography( m_decoder.AlignmentMatrix() );
            if ( m_instance.p_enableSurfaceSplines )
               if ( m_decoder.HasSplines() )
                  threadData.G.Initialize( Rect( m_referenceWidth, m_referenceHeight ), 8, m_decoder.AlignmentSplines() );
            threadData.dropDelta0 = (1 - m_instance.p_dropShrink)/2;
            threadData.dropDelta1 = 1 - threadData.dropDelta0;
            threadData.splines = threadData.G.IsValid();
            threadData.rejection = m_instance.p_enableRejection && m_decoder.HasRejectionData();
            threadData.perChannelRejection = threadData.rejection && m_numberOfChannels > 1;
            threadData.status.Initialize( "Integrating pixels", m_height );

            int numberOfThreads = Thread::NumberOfThreads( m_height, Max( 1, 4096/m_width ) );
            int rowsPerThread = m_height/numberOfThreads;

            ReferenceArray<DrizzleThread> threads;
            for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
               threads.Add( new DrizzleThread( threadData,
                                             i*rowsPerThread,
                                             (j < numberOfThreads) ? j*rowsPerThread : m_height ) );

            AbstractImage::RunThreads( threads, threadData );

            double outputData = 0;
            for ( int i = 0; i < numberOfThreads; ++i )
               outputData += threads[i].totalDropArea;
            outputData /= roi.Area();
            double inputData = outputData/m_instance.p_dropShrink/m_instance.p_dropShrink;
            outputData *= m_pixelSize*m_pixelSize;

            totalOutputData += outputData;

            threads.Destroy();

            monitor = threadData.status;

            sourceImage.FreeData();

            console.WriteLn( String().Format( "<end><cbr>Input data  : %.3f", inputData ) );
            console.WriteLn( String().Format( "<end><cbr>Output data : %.3f", outputData ) );

            for ( int i = 0; i < m_numberOfChannels && i < 3; ++i )
            {
               m_instance.o_output.totalRejectedLow[i] += m_decoder.RejectionLowCount()[i];
               m_instance.o_output.totalRejectedHigh[i] += m_decoder.RejectionHighCount()[i];
            }

            DrizzleIntegrationInstance::OutputData::ImageData imageData( filePath );
            for ( int i = 0; i < m_numberOfChannels && i < 3; ++i )
            {
               imageData.weight[i] = m_decoder.Weight()[i];
               imageData.location[i] = m_decoder.Location()[i];
               imageData.referenceLocation[i] = m_decoder.ReferenceLocation()[i];
               imageData.scale[i] = m_decoder.Scale()[i];
               imageData.rejectedLow[i] = m_decoder.RejectionLowCount()[i];
               imageData.rejectedHigh[i] = m_decoder.RejectionHighCount()[i];
            }
            imageData.outputData = outputData;
            m_instance.o_output.imageData.Append( imageData );

            ++succeeded;
         }
         catch ( ProcessAborted& )
         {
            throw;
         }
         catch ( ... )
         {
            if ( console.AbortRequested() )
               throw ProcessAborted();

            sourceImage.FreeData();

            ++failed;

            try
            {
               throw;
            }
            ERROR_HANDLER

            if ( count < int( m_instance.p_inputData.Length() ) )
            {
               console.ResetStatus();
               console.EnableAbort();

               console.Note( "<end><cbr><br>* Applying error policy: " );

               switch ( m_instance.p_onError )
               {
               default: // ?
               case DZOnError::Continue:
                  console.NoteLn( "Continue on error." );
                  break;

               case DZOnError::Abort:
                  console.NoteLn( "Abort on error." );
                  throw ProcessAborted();

               case DZOnError::AskUser:
                  {
                     console.NoteLn( "Ask on error..." );

                     if ( MessageBox( "<p style=\"white-space:pre;\">"
                                    "An error occurred during DrizzleIntegration execution. What do you want to do?</p>",
                                    "DrizzleIntegration",
                                    StdIcon::Error,
                                    StdButton::Ignore, StdButton::Abort ).Execute() == StdButton::Abort )
                     {
                        console.NoteLn( "* Aborting as per user request." );
                        throw ProcessAborted();
                     }

                     console.NoteLn( "* Error ignored as per user request." );
                  }
                  break;
               }
            }
         }
      }

      if ( succeeded == 0 )
      {
         if ( failed == 0 )
            throw Error( "No images were integrated: Empty input list, or no enabled input items?" );
         throw Error( "No image could be integrated." );
      }

      Normalize( resultImage, weightImage );

      console.WriteLn( String().Format( "<end><cbr><br>Total output data : %.3f", totalOutputData ) );

      m_instance.o_output.integrationImageId = resultWindow.MainView().Id();
      m_instance.o_output.weightImageId = weightWindow.MainView().Id();
      m_instance.o_output.numberOfChannels = m_numberOfChannels;
      m_instance.o_output.outputPixels = resultImage.NumberOfPixels();
      m_instance.o_output.integratedPixels = size_type( succeeded )*size_type( m_referenceWidth )*size_type( m_referenceHeight );
      m_instance.o_output.outputData = totalOutputData;

      FITSKeywordArray keywords;
      resultWindow.GetKeywords( keywords );

      keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "Integration with " + PixInsightVersion::AsString() ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with " + Module->ReadableVersion() ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with DrizzleIntegration process" ) );

      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.scale: %.2f", m_instance.p_scale ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.dropShrink: %.2f", m_instance.p_dropShrink ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       "DrizzleIntegration.enableRejection: " + IsoString( bool( m_instance.p_enableRejection ) ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       "DrizzleIntegration.enableImageWeighting: " + IsoString( bool( m_instance.p_enableImageWeighting ) ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       "DrizzleIntegration.enableSurfaceSplines: " + IsoString( bool( m_instance.p_enableSurfaceSplines ) ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.referenceDimensions: width=%d, height=%d",
                                                         m_referenceWidth, m_referenceHeight ) ) );
      if ( m_instance.p_useROI )
         keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.regionOfInterest: left=%d, top=%d, width=%d, height=%d",
                                                         roi.x0, roi.y0, roi.Width(), roi.Height() ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.drizzleGeometry: left=%d, top=%d, width=%d, height=%d",
                                                         m_origin.x, m_origin.y, m_width, m_height ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.pixelSize: %.3f", m_pixelSize ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.numberOfImages: %d", succeeded ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.outputPixels: %lu", m_instance.o_output.outputPixels ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.integratedPixels: %lu", m_instance.o_output.integratedPixels ) ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                       IsoString().Format( "DrizzleIntegration.outputData: %.3f", totalOutputData ) ) );
      if ( !ignoringPedestal )
         if ( havePedestal )
            if ( pedestal > 0 )
            {
               keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(),
                                                IsoString().Format( "DrizzleIntegration.outputPedestal: %.4g DN", pedestal ) ) );
               keywords.Add( FITSHeaderKeyword( "PEDESTAL",
                                                IsoString().Format( "%.4g", pedestal ),
                                                "Value in DN added to enforce positivity" ) );
               console.NoteLn( String().Format( "* PEDESTAL keyword created with value: %.4g DN", pedestal ) );
            }

      resultWindow.SetKeywords( keywords );

      resultWindow.Show();
      resultWindow.ZoomToFit( false/*allowZoomIn*/ );

      weightWindow.Show();
      weightWindow.ZoomToFit( false/*allowZoomIn*/ );

      console.NoteLn( String().Format( "<end><cbr><br>===== DrizzleIntegration: %u succeeded, %u failed, %u skipped =====",
                                       succeeded, failed, skipped ) );
   }
   catch ( ... )
   {
      sourceImage.FreeData();
      if ( !resultWindow.IsNull() )
         resultWindow.Close();
      if ( !weightWindow.IsNull() )
         weightWindow.Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

bool DrizzleIntegrationInstance::ExecuteGlobal()
{
   Exception::DisableGUIOutput( p_noGUIMessages );

   o_output = OutputData();

   Console().EnableAbort();

   if ( p_useROI )
   {
      p_roi.Order();
      if ( !p_roi.IsRect() )
         throw Error( "Empty ROI defined" );
   }

   DrizzleIntegrationEngine( *this ).Perform();

   return true;
}

// ----------------------------------------------------------------------------

void* DrizzleIntegrationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDZItemEnabledParameter )
      return &p_inputData[tableRow].enabled;
   if ( p == TheDZItemPathParameter )
      return p_inputData[tableRow].path.c_str();
   if ( p == TheDZInputHintsParameter )
      return p_inputHints.c_str();
   if ( p == TheDZInputDirectoryParameter )
      return p_inputDirectory.c_str();
   if ( p == TheDZScaleParameter )
      return &p_scale;
   if ( p == TheDZDropShrinkParameter )
      return &p_dropShrink;
   if ( p == TheDZEnableRejectionParameter )
      return &p_enableRejection;
   if ( p == TheDZEnableImageWeightingParameter )
      return &p_enableImageWeighting;
   if ( p == TheDZEnableSurfaceSplinesParameter )
      return &p_enableSurfaceSplines;
   if ( p == TheDZUseROIParameter )
      return &p_useROI;
   if ( p == TheDZROIX0Parameter )
      return &p_roi.x0;
   if ( p == TheDZROIY0Parameter )
      return &p_roi.y0;
   if ( p == TheDZROIX1Parameter )
      return &p_roi.x1;
   if ( p == TheDZROIY1Parameter )
      return &p_roi.y1;
   if ( p == TheDZClosePreviousImagesParameter )
      return &p_closePreviousImages;
   if ( p == TheDZNoGUIMessagesParameter )
      return &p_noGUIMessages;
   if ( p == TheDZOnErrorParameter )
      return &p_onError;

   if ( p == TheDZIntegrationImageIdParameter )
      return o_output.integrationImageId.c_str();
   if ( p == TheDZWeightImageIdParameter )
      return o_output.weightImageId.c_str();
   if ( p == TheDZNumberOfChannelsParameter )
      return &o_output.numberOfChannels;
   if ( p == TheDZOutputPixelsParameter )
      return &o_output.outputPixels;
   if ( p == TheDZIntegratedPixelsParameter )
      return &o_output.integratedPixels;
   if ( p == TheDZTotalRejectedLowRKParameter )
      return o_output.totalRejectedLow.At( 0 );
   if ( p == TheDZTotalRejectedLowGParameter )
      return o_output.totalRejectedLow.At( 1 );
   if ( p == TheDZTotalRejectedLowBParameter )
      return o_output.totalRejectedLow.At( 2 );
   if ( p == TheDZTotalRejectedHighRKParameter )
      return o_output.totalRejectedHigh.At( 0 );
   if ( p == TheDZTotalRejectedHighGParameter )
      return o_output.totalRejectedHigh.At( 1 );
   if ( p == TheDZTotalRejectedHighBParameter )
      return o_output.totalRejectedHigh.At( 2 );
   if ( p == TheDZOutputDataParameter )
      return &o_output.outputData;
   if ( p == TheDZImageFilePathParameter )
      return o_output.imageData[tableRow].filePath.c_str();
   if ( p == TheDZImageWeightRKParameter )
      return o_output.imageData[tableRow].weight.At( 0 );
   if ( p == TheDZImageWeightGParameter )
      return o_output.imageData[tableRow].weight.At( 1 );
   if ( p == TheDZImageWeightBParameter )
      return o_output.imageData[tableRow].weight.At( 2 );
   if ( p == TheDZImageLocationRKParameter )
      return o_output.imageData[tableRow].location.At( 0 );
   if ( p == TheDZImageLocationGParameter )
      return o_output.imageData[tableRow].location.At( 1 );
   if ( p == TheDZImageLocationBParameter )
      return o_output.imageData[tableRow].location.At( 2 );
   if ( p == TheDZImageReferenceLocationRKParameter )
      return o_output.imageData[tableRow].referenceLocation.At( 0 );
   if ( p == TheDZImageReferenceLocationGParameter )
      return o_output.imageData[tableRow].referenceLocation.At( 1 );
   if ( p == TheDZImageReferenceLocationBParameter )
      return o_output.imageData[tableRow].referenceLocation.At( 2 );
   if ( p == TheDZImageScaleRKParameter )
      return o_output.imageData[tableRow].scale.At( 0 );
   if ( p == TheDZImageScaleGParameter )
      return o_output.imageData[tableRow].scale.At( 1 );
   if ( p == TheDZImageScaleBParameter )
      return o_output.imageData[tableRow].scale.At( 2 );
   if ( p == TheDZImageRejectedLowRKParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 0 );
   if ( p == TheDZImageRejectedLowGParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 1 );
   if ( p == TheDZImageRejectedLowBParameter )
      return o_output.imageData[tableRow].rejectedLow.At( 2 );
   if ( p == TheDZImageRejectedHighRKParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 0 );
   if ( p == TheDZImageRejectedHighGParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 1 );
   if ( p == TheDZImageRejectedHighBParameter )
      return o_output.imageData[tableRow].rejectedHigh.At( 2 );
   if ( p == TheDZImageOutputDataParameter )
      return &o_output.imageData[tableRow].outputData;

   return 0;
}

bool DrizzleIntegrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{

   if ( p == TheDZInputDataParameter )
   {
      p_inputData.Clear();
      if ( sizeOrLength > 0 )
         p_inputData.Add( DataItem(), sizeOrLength );
   }
   else if ( p == TheDZItemPathParameter )
   {
      p_inputData[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_inputData[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheDZInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheDZInputDirectoryParameter )
   {
      p_inputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_inputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheDZIntegrationImageIdParameter )
   {
      o_output.integrationImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.integrationImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheDZWeightImageIdParameter )
   {
      o_output.weightImageId.Clear();
      if ( sizeOrLength > 0 )
         o_output.weightImageId.SetLength( sizeOrLength );
   }
   else if ( p == TheDZImageDataParameter )
   {
      o_output.imageData.Clear();
      if ( sizeOrLength > 0 )
         o_output.imageData.Append( OutputData::ImageData(), sizeOrLength );
   }
   else if ( p == TheDZImageFilePathParameter )
   {
      o_output.imageData[tableRow].filePath.Clear();
      if ( sizeOrLength > 0 )
         o_output.imageData[tableRow].filePath.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type DrizzleIntegrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheDZInputDataParameter )
      return p_inputData.Length();
   if ( p == TheDZItemPathParameter )
      return p_inputData[tableRow].path.Length();
   if ( p == TheDZInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheDZInputDirectoryParameter )
      return p_inputDirectory.Length();

   if ( p == TheDZIntegrationImageIdParameter )
      return o_output.integrationImageId.Length();
   if ( p == TheDZWeightImageIdParameter )
      return o_output.weightImageId.Length();
   if ( p == TheDZImageDataParameter )
      return o_output.imageData.Length();
   if ( p == TheDZImageFilePathParameter )
      return o_output.imageData[tableRow].filePath.Length();

   return 0;
}

// ----------------------------------------------------------------------------

// /*
//  * Tests if a point is Left|On|Right of an infinite line.
//  * Input:  three points a, b, and c
//  * Return: > 0 for c left of the line through a and b
//  *         = 0 for c on the line
//  *         < 0 for c right of the line
//  */
// inline double IsLeftTurn( const DPoint& a, const DPoint& b, const DPoint& c )
// {
//    return (b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y);
// }
//
/*
 * Andrew's monotone chain 2D convex hull algorithm
 * Input:  P = Array of 2D points presorted by increasing x and y coordinates
 * Output: H = Array of the convex hull vertices
 *
 * Adapted from an implementation by Dan Sunday:
 *    http://geomalgorithms.com/a10-_hull-1.html
 * Copyright 2001 softSurfer, 2012 Dan Sunday
 */
// static void GetConvexHullOfSortedPolygon( Array<DPoint>& H, const Array<DPoint>& P )
// {
//    int n = P.Length();
//
//    // Get the indices of points with min x-coord and min|max y-coord
//    int minmin = 0, minmax;
//    double xmin = P[0].x;
//    for ( int i = 1; i < n; ++i )
//       if ( P[i].x != xmin )
//       {
//          minmax = i-1;
//          break;
//       }
//    if ( minmax == n-1 ) // degenerate case: all x-coords == xmin
//    {
//       H.Append( P[minmin] );
//       if ( P[minmax].y != P[minmin].y ) // a nontrivial segment
//          H.Append( P[minmax] );
//       H.Append( P[minmin] );             // add polygon endpoint
//       return;
//    }
//
//    // Get the indices of points with max x-coord and min|max y-coord
//    int maxmin, maxmax = n-1;
//    double xmax = P[n-1].x;
//    for ( int i = n-2; i >= 0; --i )
//       if ( P[i].x != xmax )
//       {
//          maxmin = i+1;
//          break;
//       }
//
//    // the output array H[] will be used as the stack
//    H = Array<DPoint>( n );
//    int bot = 0, top = -1; // indices for bottom and top of the stack
//
//    // Compute the lower hull on the stack H
//    H[++top] = P[minmin];      // push minmin point onto stack
//    for ( int i = minmax; ++i <= maxmin; )
//    {
//       // the lower line joins P[minmin] with P[maxmin]
//       if ( IsLeftTurn( P[minmin], P[maxmin], P[i] ) >= 0 && i < maxmin )
//          continue;           // ignore P[i] above or on the lower line
//
//       while ( top > 0 )         // there are at least 2 points on the stack
//       {
//          // test if P[i] is left of the line at the stack top
//          if ( IsLeftTurn( H[top-1], H[top], P[i]) > 0 )
//             break;         // P[i] is a new hull vertex
//          --top;         // pop top point off stack
//       }
//       H[++top] = P[i];        // push P[i] onto stack
//    }
//
//    // Next, compute the upper hull on the stack H above the bottom hull
//    if ( maxmax != maxmin )      // if distinct xmax points
//       H[++top] = P[maxmax];  // push maxmax point onto stack
//    bot = top;                  // the bottom point of the upper hull stack
//    for ( int i = maxmin; --i >= minmax; )
//    {
//       // the upper line joins P[maxmax] with P[minmax]
//       if ( IsLeftTurn( P[maxmax], P[minmax], P[i] ) >= 0 && i > minmax )
//          continue;           // ignore P[i] below or on the upper line
//
//       while ( top > bot )     // at least 2 points on the upper stack
//       {
//          // test if P[i] is left of the line at the stack top
//          if ( IsLeftTurn( H[top-1], H[top], P[i]) > 0 )
//             break;         // P[i] is a new hull vertex
//          --top;         // pop top point off stack
//       }
//       H[++top] = P[i];        // push P[i] onto stack
//    }
//    if ( minmax != minmin )
//       H[++top] = P[minmin];  // push joining endpoint onto stack
//
//    H.Remove( H.At( top+1 ), H.End() );
// }

/*
 * Adapted from a public-domain function by Darel Rex Finley, 2006
 * http://alienryderflex.com/intersect/
 *
 * Determines the intersection point of the line segment defined by points A and B
 * with the line segment defined by points C and D.
 *
 * Returns true if the intersection point was found, and stores the
 * intersection coordinates in p.
 *
 * Returns false if there is no determinable intersection point, in which case
 * p will be unmodified.
 *
 * Returns false if the segments are collinear, even if they overlap.
 */
// inline bool GetIntersectionOfLineSegments( DPoint& p, const DPoint& a, const DPoint& b, const DPoint& c, const DPoint& d )
// {
//    // Fail if either line segment is zero-length.
//    if ( a.x == b.x && a.y == b.y || c.x == d.x && c.y == d.y )
//       return false;
//
//    // Fail if the segments share an end-point.
//    if ( a == c || b == c || a == d || b == d )
//       return false;
//
//    // (1) Translate the system so that point A is at the origin.
//    DPoint b0 = b - a;
//    DPoint c0 = c - a;
//    DPoint d0 = d - a;
//
//    // Discover the length of segment A-B.
//    double r = Sqrt( b0.x*b0.x + b0.y*b0.y );
//
//    // (2) Rotate the system so that point B is on the positive X axis.
//    double theCos = b0.x/r;
//    double theSin = b0.y/r;
//    double newX = c0.x*theCos + c0.y*theSin;
//    c0.y = c0.y*theCos - c0.x*theSin;
//    c0.x = newX;
//    newX = d0.x*theCos + d0.y*theSin;
//    d0.y = d0.y*theCos - d0.x*theSin;
//    d0.x = newX;
//
//    //  Fail if segment C-D doesn't cross line A-B.
//    if ( c0.y < 0 && d0.y < 0 || c0.y >= 0 && d0.y >= 0 )
//       return false;
//
//    // (3) Discover the position of the intersection point along line A-B.
//    double abPos = d0.x + (c0.x - d0.x)*d0.y/(d0.y - c0.y);
//
//    // Fail if segment C-D crosses line A-B outside of segment A-B.
//    if ( abPos < 0 || abPos > r )
//       return false;
//
//    // (4) Apply the discovered position to line A-B in the original coordinate system.
//    p.x = a.x + abPos*theCos;
//    p.y = a.y + abPos*theSin;
//    return true;
// }

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationInstance.cpp - Released 2015/10/08 11:24:40 UTC
