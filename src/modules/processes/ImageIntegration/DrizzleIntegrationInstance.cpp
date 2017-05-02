//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.14.00.0390
// ----------------------------------------------------------------------------
// DrizzleIntegrationInstance.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "DrizzleIntegrationInstance.h"

#include <pcl/AutoPointer.h>
#include <pcl/DrizzleData.h>
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

#define DRIZZLE_BOUNDS_TOLERANCE 1.0e-5
#define DRIZZLE_KERNEL_EPSILON   0.025

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Homography class borrowed from ImageRegistration to fix compilation issue:
 * http://pixinsight.com/forum/index.php?topic=7936.0
 *
 * ### TODO: This is a temporary fix - Must implement this class (improved) as
 * a public PCL class.
 */
class Homography
{
public:

   typedef Array<FPoint>   point_list;

   Homography() : H( Matrix::UnitMatrix( 3 ) )
   {
   }

   Homography( const Matrix& aH ) : H( aH )
   {
   }

   Homography( const Homography& h ) : H( h.H )
   {
   }

//    Homography( const point_list& P1, const point_list& P2 ) : H( HomographyDLT( P1, P2 ).H )
//    {
//    }

   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      double w = H[2][0]*x + H[2][1]*y + H[2][2];
      PCL_CHECK( 1 + w != 1 )
      return DPoint( (H[0][0]*x + H[0][1]*y + H[0][2])/w,
                     (H[1][0]*x + H[1][1]*y + H[1][2])/w );
   }

   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

   Homography Inverse() const
   {
      return Homography( H.Inverse() );
   }

   operator const Matrix&() const
   {
      return H;
   }

   bool IsValid() const
   {
      return !H.IsEmpty();
   }

   void EnsureUnique()
   {
      H.EnsureUnique();
   }

private:

   Matrix H;
};

// ----------------------------------------------------------------------------

/*
 * Abstract base class of drizzle kernel functions. Discretizes a
 * two-dimensional function on a uniform square lattice and computes LUT tables
 * for fast double integration over arbitrary regions on the XY plane.
 */
class DrizzleKernelFunction
{
public:

   DrizzleKernelFunction() = default;
   DrizzleKernelFunction( const DrizzleKernelFunction& ) = default;
   DrizzleKernelFunction& operator =( const DrizzleKernelFunction& ) = default;

   virtual ~DrizzleKernelFunction()
   {
   }

   /*
    * Initialize microdrop LUTs for the specified drop region and integration
    * grid size.
    */
   void Initialize( double dropSize, int n )
   {
      Reset( dropSize );
      m_pos = 0;
      int n2 = n*n;
      m_xlut = DVector( n2 );
      m_ylut = DVector( n2 );
      m_zlut = DVector( n2 );
      double r = dropSize/2, d = dropSize/n, d2 = d/2;
      for ( int i = 0, k = 0; i < n; ++i )
      {
         double x = i*d + d2;
         for ( int j = 0; j < n; ++j, ++k )
         {
            m_xlut[k] = x;
            m_ylut[k] = j*d + d2;
            m_zlut[k] = Value( x - r, m_ylut[k] - r );
         }
      }
      // Normalize for unit integration volume.
      m_zlut /= n2*Value( 0, 0 );
   }

   /*
    * Set the current drop position in alignment reference coordinates.
    */
   void MoveTo( double x, double y )
   {
      m_pos.x = x;
      m_pos.y = y;
   }

   /*
    * Immutable iterator for fast LUT-based double integration.
    */
   class const_iterator : public ForwardIterator
   {
   public:

      const_iterator( const DrizzleKernelFunction& F ) :
         f( F ), x( F.m_xlut.Begin() ), y( F.m_ylut.Begin() ), z( F.m_zlut.Begin() )
      {
      }

      const_iterator( const const_iterator& ) = default;
#ifndef _MSC_VER
      const_iterator( const_iterator&& ) = default;
#endif

      /*
       * Returns true iff this iterator is valid.
       */
      operator bool() const
      {
         return x < f.m_xlut.End();
      }

      /*
       * Horizontal microdrop coordinate for the current LUT item.
       */
      double X() const
      {
         return *x + f.m_pos.x;
      }

      /*
       * Vertical microdrop coordinate for the current LUT item.
       */
      double Y() const
      {
         return *y + f.m_pos.y;
      }

      /*
       * Function value for the current LUT item.
       */
      double Z() const
      {
         return *z;
      }

      /*
       * Preincrement operator.
       */
      const_iterator& operator ++()
      {
         ++x; ++y; ++z;
         return *this;
      }

      /*
       * Postincrement operator.
       */
      const_iterator operator ++( int )
      {
         const_iterator i( *this );
         ++x; ++y; ++z;
         return i;
      }

   private:

      const DrizzleKernelFunction& f;
      DVector::const_iterator      x, y, z;
   };

   /*
    * Returns a new iterator located at the beginning of all LUTs.
    */
   const_iterator Begin() const
   {
      return const_iterator( *this );
   }

protected:

   /*
    * Reset function parameters for the specified drop size and prepare for
    * LUT initialization.
    */
   virtual void Reset( double dropSize ) = 0;

   /*
    * Function value at the specified local coordinates in the [0,1] range. The
    * origin of coordinates dx=dy=0 is at the center of the sampling region.
    * The kernel function must be rotationally symmetric with its global
    * maximum at the origin.
    */
   virtual double Value( double dx, double dy ) const = 0;

private:

   /*
    * Current drop position.
    */
   DPoint m_pos;

   /*
    * Microdrop LUT for fast double integration.
    */
   DVector m_xlut; // horizontal coordinates
   DVector m_ylut; // vertical coordinates
   DVector m_zlut; // function values
};

// ----------------------------------------------------------------------------

/*
 * Gaussian drizzle kernel function.
 */
class GaussianDrizzleKernelFunction : public DrizzleKernelFunction
{
public:

   GaussianDrizzleKernelFunction() = default;

private:

   double m_2s2;

   virtual void Reset( double dropSize )
   {
      double sigma = dropSize/2/Sqrt( -2*Ln( DRIZZLE_KERNEL_EPSILON ) );
      PCL_CHECK( sigma > 0 )
      m_2s2 = 2*sigma*sigma;
   }

   virtual double Value( double dx, double dy ) const
   {
      return Exp( -(dx*dx + dy*dy)/m_2s2 );
   }
};

// ----------------------------------------------------------------------------

/*
 * Variable shape drizzle kernel function.
 */
class VariableShapeDrizzleKernelFunction : public DrizzleKernelFunction
{
public:

   VariableShapeDrizzleKernelFunction( double shape ) : DrizzleKernelFunction(), m_shape( shape ), m_rk( 1 )
   {
      PCL_PRECONDITION( m_shape > 0 )
   }

private:

   double m_shape;
   double m_rk;

   virtual void Reset( double dropSize )
   {
      double sigma = dropSize/2/Pow( -m_shape*Ln( DRIZZLE_KERNEL_EPSILON ), 1/m_shape );
      PCL_CHECK( sigma > 0 )
      m_rk = m_shape * Pow( sigma, m_shape );
   }

   virtual double Value( double dx, double dy ) const
   {
      return Exp( -Pow( Sqrt( (dx*dx + dy*dy) ), m_shape )/m_rk );
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Fast color filter array (CFA) pixel sample selection.
 */
class CFAIndex
{
public:

   typedef GenericMatrix<bool>   cfa_channel_index;

   CFAIndex() = default;
   CFAIndex( const CFAIndex& ) = default;

   CFAIndex( const String& pattern )
   {
      if ( pattern.IsEmpty() )
         throw Error( "Empty CFA pattern." );
      m_size = int( Sqrt( pattern.Length() ) );
      if ( m_size < 2 )
         throw Error( "Invalid CFA pattern '" + pattern + '\'' );
      if ( m_size*m_size != pattern.Length() )
         throw Error( "Non-square CFA patterns are not supported: '" + pattern + '\'' );
      for ( int c = 0; c < 3; ++c )
      {
         m_index[c] = cfa_channel_index( m_size, m_size );
         String::const_iterator p = pattern.Begin();
         for ( int i = 0; i < m_size; ++i )
            for ( int j = 0; j < m_size; ++j )
               m_index[c][i][j] = *p++ == "RGB"[c];
      }
   }

   operator bool() const
   {
      return m_size > 0;
   }

   bool operator ()( const Point p, int c ) const
   {
      return m_index[c][p.y % m_size][p.x % m_size];
   }

private:

   int               m_size = 0;
   cfa_channel_index m_index[ 3 ]; // RGB
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DrizzleIntegrationInstance::DrizzleIntegrationInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_inputData(),
   p_inputHints(),
   p_inputDirectory(),
   p_scale( TheDZScaleParameter->DefaultValue() ),
   p_dropShrink( TheDZDropShrinkParameter->DefaultValue() ),
   p_kernelFunction( DZKernelFunction::Default ),
   p_kernelGridSize( TheDZKernelGridSizeParameter->DefaultValue() ),
   p_origin( TheDZOriginXParameter->DefaultValue(), TheDZOriginYParameter->DefaultValue() ),
   p_enableCFA( TheDZEnableCFAParameter->DefaultValue() ),
   p_cfaPattern( TheDZCFAPatternParameter->DefaultValue() ),
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

// ----------------------------------------------------------------------------

DrizzleIntegrationInstance::DrizzleIntegrationInstance( const DrizzleIntegrationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void DrizzleIntegrationInstance::Assign( const ProcessImplementation& p )
{
   const DrizzleIntegrationInstance* x = dynamic_cast<const DrizzleIntegrationInstance*>( &p );
   if ( x != nullptr )
   {
      p_inputData            = x->p_inputData;
      p_inputHints           = x->p_inputHints;
      p_inputDirectory       = x->p_inputDirectory;
      p_scale                = x->p_scale;
      p_dropShrink           = x->p_dropShrink;
      p_kernelFunction       = x->p_kernelFunction;
      p_kernelGridSize       = x->p_kernelGridSize;
      p_origin               = x->p_origin;
      p_enableCFA            = x->p_enableCFA;
      p_cfaPattern           = x->p_cfaPattern;
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

// ----------------------------------------------------------------------------

bool DrizzleIntegrationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "DrizzleIntegration can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool DrizzleIntegrationInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool DrizzleIntegrationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_inputData.Length() < 1 )
   {
      whyNot = "This instance of DrizzleIntegration has no input data items.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DrizzleIntegrationEngine
{
public:

   DrizzleIntegrationEngine( DrizzleIntegrationInstance& instance ) :
      m_instance( instance )
   {
   }

   void Perform();

   void Clear()
   {
      m_decoder.Clear();
      m_referenceWidth = m_referenceHeight = m_width = m_height = m_numberOfChannels = 0;
      m_pixelSize = 0;
   }

private:

   typedef DrizzleIntegrationInstance::input_data_list   input_data_list;

   DrizzleIntegrationInstance& m_instance;
   DrizzleData                 m_decoder;              // current drizzle data
   int                         m_referenceWidth   = 0;
   int                         m_referenceHeight  = 0;
   Point                       m_origin           = Point( 0 ); // output ROI origin
   int                         m_width            = 0; // output ROI dimensions in pixels
   int                         m_height           = 0;
   int                         m_numberOfChannels = 0;
   double                      m_pixelSize        = 0; // in reference pixel units

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const DrizzleIntegrationEngine& engine_,
                  const Image& source_, const CFAIndex& cfaIndex_,
                  Image& result_, Image& weight_,
                  const StatusMonitor& monitor_, size_type count_ ) :
         AbstractImage::ThreadData( monitor_, count_ ),
         engine( engine_ ),
         source( source_ ), cfaIndex( cfaIndex_ ),
         result( result_ ), weight( weight_ )
      {
      }

      const DrizzleIntegrationEngine& engine;
      const Image&                    source;
      const CFAIndex&                 cfaIndex;
            Image&                    result;
            Image&                    weight;
            Homography                H, Hinv;
            PointGridInterpolation    G, Ginv;
            double                    dropDelta0;
            double                    dropDelta1;
            bool                      splines;
            bool                      rejection;
   };

   class DrizzleThread : public Thread
   {
   public:

      double totalDropArea = 0; // total data gathered by this thread in drop area units

      DrizzleThread( const ThreadData& data, int firstRow, int endRow ) :
         m_data( data ), m_firstRow( firstRow ), m_endRow( endRow ), m_kernel()
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         totalDropArea = 0;

         m_kernel.Reset();
         bool circular = false;
         switch ( m_data.engine.m_instance.p_kernelFunction )
         {
         default:
         case DZKernelFunction::Square:
            break;
         case DZKernelFunction::Circular:
            circular = true;
            break;
         case DZKernelFunction::Gaussian:
            m_kernel = new GaussianDrizzleKernelFunction;
            break;
         case DZKernelFunction::Variable10:
            m_kernel = new VariableShapeDrizzleKernelFunction( 1.0 );
            break;
         case DZKernelFunction::Variable15:
            m_kernel = new VariableShapeDrizzleKernelFunction( 1.5 );
            break;
         case DZKernelFunction::Variable30:
            m_kernel = new VariableShapeDrizzleKernelFunction( 3.0 );
            break;
         case DZKernelFunction::Variable40:
            m_kernel = new VariableShapeDrizzleKernelFunction( 4.0 );
            break;
         case DZKernelFunction::Variable50:
            m_kernel = new VariableShapeDrizzleKernelFunction( 5.0 );
            break;
         case DZKernelFunction::Variable60:
            m_kernel = new VariableShapeDrizzleKernelFunction( 6.0 );
            break;
         }

         if ( !m_kernel.IsNull() )
            m_kernel->Initialize( 1 - 2*m_data.dropDelta0, m_data.engine.m_instance.p_kernelGridSize );

         Image::pixel_iterator r( m_data.result );
         r.MoveBy( 0, m_firstRow );
         Image::pixel_iterator w( m_data.weight );
         w.MoveBy( 0, m_firstRow );

         for ( int y = m_firstRow; y < m_endRow; ++y )
         {
            DRect referenceRect;
            referenceRect.y0 = (y + m_data.engine.m_origin.y) * m_data.engine.m_pixelSize;
            referenceRect.y1 = referenceRect.y0 + m_data.engine.m_pixelSize;

            for ( int x = 0; x < m_data.engine.m_width; ++x, ++r, ++w )
            {
               referenceRect.x0 = (x + m_data.engine.m_origin.x) * m_data.engine.m_pixelSize;
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

               sourceP0 += m_data.engine.m_instance.p_origin;
               sourceP1 += m_data.engine.m_instance.p_origin;
               sourceP2 += m_data.engine.m_instance.p_origin;
               sourceP3 += m_data.engine.m_instance.p_origin;

               DRect sourceBounds( Min( Min( Min( sourceP0.x, sourceP1.x ), sourceP2.x ), sourceP3.x ) - DRIZZLE_BOUNDS_TOLERANCE,
                                   Min( Min( Min( sourceP0.y, sourceP1.y ), sourceP2.y ), sourceP3.y ) - DRIZZLE_BOUNDS_TOLERANCE,
                                   Max( Max( Max( sourceP0.x, sourceP1.x ), sourceP2.x ), sourceP3.x ) + DRIZZLE_BOUNDS_TOLERANCE,
                                   Max( Max( Max( sourceP0.y, sourceP1.y ), sourceP2.y ), sourceP3.y ) + DRIZZLE_BOUNDS_TOLERANCE );

               Array<Point> sourcePixels;
               {
                  Rect b = sourceBounds.TruncatedToInt();
                  for ( int y = b.y0; y <= b.y1; ++y )
                     if ( y >= 0 )
                        if ( y < m_data.source.Height() )
                           for ( int x = b.x0; x <= b.x1; ++x )
                              if ( x >= 0 )
                                 if ( x < m_data.source.Width() )
                                    sourcePixels << Point( x, y );
               }

               for ( auto p : sourcePixels )
               {
                  DRect dropRect( p.x + m_data.dropDelta0,
                                  p.y + m_data.dropDelta0,
                                  p.x + m_data.dropDelta1,
                                  p.y + m_data.dropDelta1 );

                  if ( CanRectsIntersect( dropRect, sourceBounds ) )
                  {
                     if ( !m_kernel.IsNull() )
                        m_kernel->MoveTo( dropRect.x0, dropRect.y0 );

                     double area;
                     if ( circular ?
                            GetAreaOfIntersectionOfQuadAndCircle( area, dropRect.Center(), dropRect.Width()/2, sourceP0, sourceP1, sourceP2, sourceP3 )
                          : GetAreaOfIntersectionOfQuadAndRect( area, dropRect, sourceP0, sourceP1, sourceP2, sourceP3, m_kernel ) )
                     {
                        Point q;
                        if ( m_data.rejection )
                           q = (m_data.splines ? m_data.Ginv( p ) : m_data.Hinv( p )).RoundedToInt();

                        for ( int c = 0; c < m_data.engine.m_numberOfChannels; ++c )
                        {
                           if ( m_data.cfaIndex )
                              if ( !m_data.cfaIndex( p, c ) )
                                 continue;

                           if ( !m_data.rejection || !m_data.engine.Reject( q, c ) )
                           {
                              double value = m_data.source( p, m_data.cfaIndex ? 0 : c );

                              if ( 1 + value != 1 )
                              {
                                 double normalizedValue = (value - m_data.engine.Location( c ))
                                                         * m_data.engine.Scale( c )
                                                         + m_data.engine.ReferenceLocation( c );

                                 double weightedArea = area * m_data.engine.Weight( c );

                                 r[c] += weightedArea * normalizedValue;
                                 w[c] += weightedArea;
                              }
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

      const ThreadData&                        m_data;
            int                                m_firstRow, m_endRow;
            AutoPointer<DrizzleKernelFunction> m_kernel;
   };

   /*
    * Predicate class for sorting a set of points in clockwise direction.
    */
   class PointsClockwisePredicate
   {
   public:

      /*
       * Initialize point sorting with respect to the specified center point.
       */
      PointsClockwisePredicate( const DPoint& center ) : c( center )
      {
      }

      /*
       * Initialize point sorting with respect to the barycenter of the
       * specified set of points.
       */
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

      /*
       * Predicate function: Returns true iff the point a precedes the point b
       * in clockwise sorting order.
       */
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

      DPoint c; // reference center point
   };

   /*
    * Returns true iff a nonempty intersection exists between the specified
    * rectangles r and s.
    */
   static bool CanRectsIntersect( const DRect& r, const DRect& s )
   {
      return s.x0 < r.x1 && s.x1 > r.x0 && s.y0 < r.y1 && s.y1 > r.y0;
   }

   /*
    * Returns true iff the specified line segment with end points p0 and p1
    * intersects the rectangle r.
    */
   static bool CanSegmentAndRectIntersect( const DPoint& p0, const DPoint& p1, const DRect& r )
   {
      return ((p0.y < p1.y) ? p0.y < r.y1 && p1.y > r.y0 : p1.y < r.y1 && p0.y > r.y0) &&
             ((p0.x < p1.x) ? p0.x < r.x1 && p1.x > r.x0 : p1.x < r.x1 && p0.x > r.x0);
   }

   /*
    * Returns true iff the specified point p is interior to the rectangle r.
    */
   static bool PointInsideRect( const DPoint& p, const DRect& r )
   {
      return p.x >= r.x0 && p.x <= r.x1 && p.y >= r.y0 && p.y <= r.y1;
   }

   static bool PointInsideConvexPolygon( double x, double y, const Array<DPoint>& P )
   {
      size_type n = P.Length()-1;
      if ( n < 2 )
         return false;
      bool s = (P[1].x - x)*(P[0].y - y) - (P[0].x - x)*(P[1].y - y) < 0;
      for ( size_type i = 1; i < n; ++i )
         if ( ((P[i+1].x - x)*(P[i].y - y) - (P[i].x - x)*(P[i+1].y - y) < 0) != s )
            return false;
      if ( ((P[0].x - x)*(P[n].y - y) - (P[n].x - x)*(P[0].y - y) < 0) != s )
         return false;
      return true;
   }

   /*
    * Returns true iff a point p is in the convex polygon defined by the set of
    * points P.
    *
    * Adapted from the Wolfram Demonstrations Project:
    * "An Efficient Test for a Point to Be in a Convex Polygon"
    * http://demonstrations.wolfram.com/AnEfficientTestForAPointToBeInAConvexPolygon/
    * Contributed by Robert Nowak
    */
   static bool PointInsideConvexPolygon( const DPoint& p, const Array<DPoint>& P )
   {
      return PointInsideConvexPolygon( p.x, p.y, P );
   }

   /*
    * Returns true iff a point p is in the convex quad {p0,p1,p2,p3}.
    *
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

   /*
    * Returns the area of a polygon defined by the set of points P. The points
    * must be sorted in clockwise or counter-clockwise direction with respect
    * to a common location.
    *
    * Adapted from a public-domain function by Darel Rex Finley, 2006:
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
      return ((s < 0) ? -s : s)/2; // s < 0 if points are listed counter-clockwise
   }

   /*
    * Returns true iff the point p is in the circle with center C and square
    * radius R2.
    */
   static bool PointInsideCircle( const DPoint& p, const DPoint& C, double R2 )
   {
      double dx = p.x - C.x;
      double dy = p.y - C.y;
      return dx*dx + dy*dy <= R2;
   }

   /*
    * Computes the intersection between an arbitrary line segment with end
    * points a, b and a horizontal line segment with end points {x0,y} and
    * {x1,y}, and appends it to the set of points P.
    */
   static void GetIntersectionOfSegmentAndHorizontalSegment( Array<DPoint>& P, const DPoint& a, const DPoint& b,
                                                             double x0, double y, double x1 )
   {
      // No intersection if the lines are parallel.
      if ( a.y == b.y )
         return;

      // No intersection if the segment is above or below the horizontal line.
      if ( a.y <= y && b.y <= y || a.y > y && b.y > y )
         return;

      // No intersection if the segments share an end point.
      if ( (a.x == x0 || a.x == x1) && a.y == y || (b.x == x0 || b.x == x1) && b.y == y )
         return;

      if ( a.x == b.x )
      {
         // Perpendicular lines
         if ( a.x > x0 && a.x < x1 )
            P << DPoint( a.x, y );
      }
      else
      {
         double m = (b.y - a.y)/(b.x - a.x);
         double x = (y - a.y + m*a.x)/m;
         if ( x > x0 && x < x1 )
            P << DPoint( x, y );
      }
   }

   /*
    * Computes the intersection between an arbitrary line segment with end
    * points a, b and a vertical line segment with end points {x,y0} and
    * {x,y1}, and appends it to the set of points P.
    */
   static void GetIntersectionOfSegmentAndVerticalSegment( Array<DPoint>& P, const DPoint& a, const DPoint& b,
                                                           double x, double y0, double y1 )
   {
      // No intersection if the lines are parallel.
      if ( a.x == b.x )
         return;

      // No intersection if the segment is at one side of the vertical line.
      if ( a.x <= x && b.x <= x || a.x > x && b.x > x )
         return;

      // No intersection if the segments share an end point.
      if ( (a.y == y0 || a.y == y1) && a.x == x || (b.y == y0 || b.y == y1) && b.x == x )
         return;

      if ( a.y == b.y )
      {
         // Perpendicular lines
         if ( a.y > y0 && a.y < y1 )
            P << DPoint( x, a.y );
      }
      else
      {
         double m = (b.y - a.y)/(b.x - a.x);
         double y = m*x + a.y - m*a.x;
         if ( y > y0 && y < y1 )
            P << DPoint( x, y );
      }
   }

   /*
    * Computes the intersections between a line segment with end points a, b
    * and a rectangle r, and appends them to the set of points P.
    */
   static void GetIntersectionsOfSegmentAndRect( Array<DPoint>& P, const DPoint& a, const DPoint& b, const DRect& r )
   {
      GetIntersectionOfSegmentAndHorizontalSegment( P, a, b, r.x0, r.y0, r.x1 );
      GetIntersectionOfSegmentAndHorizontalSegment( P, a, b, r.x0, r.y1, r.x1 );
      GetIntersectionOfSegmentAndVerticalSegment( P, a, b, r.x0, r.y0, r.y1 );
      GetIntersectionOfSegmentAndVerticalSegment( P, a, b, r.x1, r.y0, r.y1 );
   }

   /*
    * Computes the area of the intersection between a rectangle r and a convex
    * quad {p0,p1,p2,p3}. If the specified drizzle kernel function F is
    * not null, returns the double integration of the kernel function over the
    * intersection region. If a nonempty intersection exists, stores its area
    * in the specified variable f and returns true; otherwise returns false.
    */
   static bool GetAreaOfIntersectionOfQuadAndRect( double& f, const DRect& r,
                                                   const DPoint& p0, const DPoint& p1, const DPoint& p2, const DPoint& p3,
                                                   const DrizzleKernelFunction* F )
   {
      /*
       * Intersections with quad sides.
       */
      Array<DPoint> P;
      if ( CanSegmentAndRectIntersect( p0, p1, r ) )
         GetIntersectionsOfSegmentAndRect( P, p0, p1, r );
      if ( CanSegmentAndRectIntersect( p1, p2, r ) )
         GetIntersectionsOfSegmentAndRect( P, p1, p2, r );
      if ( CanSegmentAndRectIntersect( p2, p3, r ) )
         GetIntersectionsOfSegmentAndRect( P, p2, p3, r );
      if ( CanSegmentAndRectIntersect( p3, p0, r ) )
         GetIntersectionsOfSegmentAndRect( P, p3, p0, r );

      /*
       * Interior quad vertices.
       */
      if ( PointInsideRect( p0, r ) )
         P << p0;
      if ( PointInsideRect( p1, r ) )
         P << p1;
      if ( PointInsideRect( p2, r ) )
         P << p2;
      if ( PointInsideRect( p3, r ) )
         P << p3;

      /*
       * Interior rectangle vertices.
       */
      if ( PointInsideConvexQuad( DPoint( r.x0, r.y0 ), p0, p1, p2, p3 ) )
         P << DPoint( r.x0, r.y0 );
      if ( PointInsideConvexQuad( DPoint( r.x0, r.y1 ), p0, p1, p2, p3 ) )
         P << DPoint( r.x0, r.y1 );
      if ( PointInsideConvexQuad( DPoint( r.x1, r.y0 ), p0, p1, p2, p3 ) )
         P << DPoint( r.x1, r.y0 );
      if ( PointInsideConvexQuad( DPoint( r.x1, r.y1 ), p0, p1, p2, p3 ) )
         P << DPoint( r.x1, r.y1 );

      if ( P.Length() < 3 )
         return false;

      /*
       * Make sure the intersection is a convex polygon.
       */
      if ( P.Length() > 3 )
         InsertionSort( P.Begin(), P.End(), PointsClockwisePredicate( P ) );

      /*
       * If no kernel function is being applied, compute the area of the
       * intersection polygon. Otherwise compute an approximation to the double
       * integral of the kernel function over the intersection polygon.
       */
      if ( F == nullptr )
         f = AreaOfPolygon( P );
      else
      {
         f = 0;
         for ( DrizzleKernelFunction::const_iterator i = F->Begin(); i; ++i )
            if ( PointInsideConvexPolygon( i.X(), i.Y(), P ) )
               f += i.Z();
      }
      return true;
   }

   /*
    * Returns true iff the point p belongs to the line segment a-b, excluding
    * its end points. The three points a, b, p must be colinear.
    */
   static bool ColinearPointInSegment( const DPoint& a, const DPoint& b, const DPoint& p )
   {
      return p.x < Max( a.x, b.x ) &&
             p.x > Min( a.x, b.x ) &&
             p.y < Max( a.y, b.y ) &&
             p.y > Min( a.y, b.y );
   }

   /*
    * Computes the intersections between a line segment with end points p1, p2
    * and a cricle with center C and square radius R2. If two intersection
    * points exist, stores them in the set of points P. This function excludes
    * segments tangent to the circle and segment end points belonging to the
    * circumference.
    */
   static void GetIntersectionsOfSegmentAndCircle( Array<DPoint>& P, const DPoint& p1, const DPoint& p2, const DPoint& C, double R2 )
   {
      DPoint a = p1 - C;
      DPoint b = p2 - C;
      double dx = b.x - a.x;
      double dy = b.y - a.y;
      double dr2 = dx*dx + dy*dy;
      double D = a.x*b.y - b.x*a.y;
      double d = R2*dr2 - D*D;
      if ( d <= 0 ) // exterior or tangent line
         return;
      double sd = Sqrt( d );
      double tx = dx*sd;
      if ( dy < 0 )
         tx = -tx;
      double ty = Abs( dy )*sd;
      double Ddx = -D*dx;
      double Ddy = D*dy;
      DPoint q1( (Ddy + tx)/dr2, (Ddx + ty)/dr2 );
      DPoint q2( (Ddy - tx)/dr2, (Ddx - ty)/dr2 );
      if ( ColinearPointInSegment( a, b, q1 ) )
         P << q1 + C;
      if ( ColinearPointInSegment( a, b, q2 ) )
         P << q2 + C;
   }

   /*
    * Computes the area of the intersection between a circle with center C and
    * radius R and a convex quad {p0,p1,p2,p3}. If a nonempty intersection
    * exists, stores its area in the specified variable f and returns true;
    * otherwise returns false.
    */
   static bool GetAreaOfIntersectionOfQuadAndCircle( double& f, const DPoint& C, double R,
                                                     const DPoint& p0, const DPoint& p1, const DPoint& p2, const DPoint& p3 )
   {
      double R2 = R*R;
      bool p0Inside = PointInsideCircle( p0, C, R2 );
      bool p1Inside = PointInsideCircle( p1, C, R2 );
      bool p2Inside = PointInsideCircle( p2, C, R2 );
      bool p3Inside = PointInsideCircle( p3, C, R2 );

      /*
       * Special case: Quad completely inside circle.
       */
      if ( p0Inside && p1Inside && p2Inside && p3Inside )
      {
         f = AreaOfPolygon( Array<DPoint>() << p0 << p1 << p2 << p3 );
         return true;
      }

      /*
       * Intersections with quad sides.
       */
      Array<DPoint> P;
      GetIntersectionsOfSegmentAndCircle( P, p0, p1, C, R2 );
      GetIntersectionsOfSegmentAndCircle( P, p1, p2, C, R2 );
      GetIntersectionsOfSegmentAndCircle( P, p2, p3, C, R2 );
      GetIntersectionsOfSegmentAndCircle( P, p3, p0, C, R2 );

      /*
       * Accumulate circular segments.
       */
      f = 0;
      if ( P.Length() > 1 )
      {
         InsertionSort( P.Begin(), P.End(), PointsClockwisePredicate( C ) );
         for ( size_type i = 1; i < P.Length(); i += 2 )
         {
            double dx = P[i].x - P[i-1].x;
            double dy = P[i].y - P[i-1].y;
            double theta = 2*ArcSin( Min( Sqrt( dx*dx + dy*dy )/R/2, 1.0 ) );
            f += R2*(theta - Sin( theta ))/2;
         }
      }

      /*
       * Add quad vertices interior to circle.
       */
      if ( p0Inside )
         P << p0;
      if ( p1Inside )
         P << p1;
      if ( p2Inside )
         P << p2;
      if ( p3Inside )
         P << p3;

      if ( P.Length() < 3 )
      {
         /*
          * Special case: Circle completely inside quad, or only two
          * intersections with one quad side.
          */
         if ( PointInsideConvexQuad( C, p0, p1, p2, p3 ) )
         {
            // If there are two intersections, subtract circular segment.
            f = Const<double>::pi()*R2 - f;
            return true;
         }
         return false;
      }

      /*
       * Add area of intersected polygon.
       */
      InsertionSort( P.Begin(), P.End(), PointsClockwisePredicate( P ) );
      f += AreaOfPolygon( P );
      return true;
   }

   /*
    * Returns true iff the pixel with coordinates given by the specified point
    * p is being rejected for the channel c of the current source image.
    */
   bool Reject( const Point& p, int c ) const
   {
      // Assume m_decoder.HasRejectionData() == true
      const UInt8Image& map = m_decoder.RejectionMap();
      return map.Includes( p ) && map( p, c ) != 0;
   }

   /*
    * Returns the location estimate for the specified channel of the current
    * source image.
    */
   const double Location( int c ) const
   {
      return m_decoder.Location()[c];
   }

   /*
    * Returns the location estimate for the specified channel of the reference
    * source image.
    */
   const double ReferenceLocation( int c ) const
   {
      return m_decoder.ReferenceLocation()[c];
   }

   /*
    * Returns the scale estimate for the specified channel of the current
    * source image.
    */
   const double Scale( int c ) const
   {
      return m_decoder.Scale()[c];
   }

   /*
    * Returns the statistical weight for the specified channel of the current
    * source image.
    */
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
         if ( !file.ReadFITSKeywords( keywords ) )
            throw CatchedException();
         for ( auto k : keywords )
            if ( !k.name.CompareIC( keyName ) )
            {
               if ( k.IsNumeric() )
                  if ( k.GetNumericValue( value ) )
                     return true;
               break;
            }
      }
      return false;
   }

   /*
    * Normalizes the drizzle result image for the specified drizzle weight map.
    */
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

// ----------------------------------------------------------------------------

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
      for ( auto item : m_instance.p_inputData )
      {
         try
         {
            ++count;

            if ( !item.enabled )
            {
               ++skipped;
               continue;
            }

            console.WriteLn( String().Format( "<end><cbr><br>* Parsing drizzle data file %d of %d:",
                                              count, m_instance.p_inputData.Length() ) );
            console.WriteLn( "<raw>" + item.path + "</raw>" );

            if ( !File::Exists( item.path ) )
               throw Error( "No such file: " + item.path );

            m_decoder.Parse( item.path );
            if ( !m_instance.p_enableSurfaceSplines || !m_decoder.HasAlignmentSplines() )
               if ( !m_decoder.HasAlignmentMatrix() )
                  throw Error( "Missing alignment matrix definition." );

            if ( !m_decoder.HasIntegrationData() )
               throw Error( "Missing image integration data." );

            if ( m_instance.p_enableRejection )
               if ( !m_decoder.HasRejectionData() )
                  console.WarningLn( "<end><cbr>** Warning: The drizzle data file contains no pixel rejection data." );

            if ( m_instance.p_enableImageWeighting )
               if ( !m_decoder.HasImageWeightsData() )
                  console.WarningLn( "<end><cbr>** Warning: The drizzle data file contains no image weights data (weight=1 will be assumed)." );

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
                  throw Error( "Inconsistent image geometry" );
            }

            String filePath;
            CFAIndex cfaIndex;
            if ( m_instance.p_enableCFA )
            {
               filePath = m_decoder.CFASourceFilePath();
               if ( filePath.IsEmpty() )
                  throw Error( "Missing CFA source file path." );

               String cfaPattern = m_instance.p_cfaPattern;
               if ( cfaPattern.IsEmpty() ) // 'auto' CFA pattern setting
               {
                  cfaPattern = m_decoder.CFASourcePattern();
                  if ( cfaPattern.IsEmpty() )
                     throw Error( "Missing CFA pattern information." );
               }
               else if ( !m_decoder.CFASourcePattern().IsEmpty() )
                  if ( m_decoder.CFASourcePattern() != cfaPattern )
                     console.WarningLn( "<end><cbr>** Warning: CFA pattern mismatch: "
                           "The drizzle file says '" + m_decoder.CFASourcePattern() + "', "
                           "we are forcing '" + cfaPattern + "\' as per instance parameters." );

               cfaIndex = CFAIndex( cfaPattern );

               console.WriteLn(      "CFA pattern          : " + cfaPattern );

               if ( m_numberOfChannels < 3 )
                  throw Error( String( "CFA mosaiced frames imply integration of an RGB color image, but this file " ) +
                               "corresponds to a monochrome image." );
               if ( m_numberOfChannels > 3 )
                  throw Error( String( "CFA mosaiced frames imply integration of an RGB color image, but this file " ) +
                               "defines additional channels that cannot be retrieved from a CFA." );
            }
            else
               filePath = m_decoder.SourceFilePath();

            if ( !m_instance.p_inputDirectory.IsEmpty() )
            {
               String nameAndSuffix = File::ExtractNameAndSuffix( filePath );
               filePath = m_instance.p_inputDirectory;
               if ( !filePath.EndsWith( '/' ) )
                  filePath << '/';
               filePath << nameAndSuffix;
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
                        console.WarningLn( String().Format( "** Warning: Invalid negative PEDESTAL keyword value: %.4g", thisPedestal ) );
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
                     console.WarningLn( "** Warning: Ignoring all existing PEDESTAL keyword values because of inconsistent/invalid values." );
               }

               sourceImage = Image( (void*)0, 0, 0 ); // shared image
               if ( !file.ReadImage( sourceImage ) )
                  throw CatchedException();
            }

            console.Write( "<end><cbr>Scale factors : " );
            for ( int c = 0; c < m_numberOfChannels; ++c )
               console.Write( String().Format( " %8.5lf", m_decoder.Scale()[c] ) );
            console.Write(       "<br>Zero offset   : " );
            for ( int c = 0; c < m_numberOfChannels; ++c )
               console.Write( String().Format( " %+.6le", m_decoder.ReferenceLocation()[c] - m_decoder.Location()[c] ) );
            if ( m_instance.p_enableImageWeighting )
            {
               console.Write(    "<br>Weight        : " );
               for ( int c = 0; c < m_numberOfChannels; ++c )
                  console.Write( String().Format( " %10.5lf", m_decoder.Weight()[c] ) );
               console.WriteLn();
            }

            Module->ProcessEvents();

            StandardStatus status;
            StatusMonitor monitor;
            monitor.SetCallback( &status );

            ThreadData threadData( *this,
                                   sourceImage,
                                   cfaIndex,
                                   static_cast<Image&>( *resultImage ),
                                   static_cast<Image&>( *weightImage ),
                                   monitor, m_height );
            threadData.H = Homography( m_decoder.AlignmentMatrix() );
            if ( m_instance.p_enableSurfaceSplines )
               if ( m_decoder.HasAlignmentSplines() )
               {
                  console.WriteLn( "<end><cbr>Building 2D surface interpolation grids...<flush>" );
                  threadData.G.Initialize( Rect( m_referenceWidth, m_referenceHeight ), 8, m_decoder.AlignmentSplines(), false/*verbose*/ );
               }
            threadData.dropDelta0 = (1 - m_instance.p_dropShrink)/2;
            threadData.dropDelta1 = 1 - threadData.dropDelta0;
            threadData.splines = threadData.G.IsValid();
            threadData.rejection = m_instance.p_enableRejection && m_decoder.HasRejectionData();
            if ( threadData.rejection )
            {
               threadData.Hinv = threadData.H.Inverse();
               if ( threadData.splines )
                  threadData.Ginv.Initialize( threadData.G.ReferenceRect(), threadData.G.Delta(), m_decoder.AlignmentSplines().Inverse(), false/*verbose*/ );
            }
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

            console.WriteLn( String().Format( "<end><cbr>Input data    : %.3lf", inputData ) );
            console.WriteLn( String().Format( "<end><cbr>Output data   : %.3lf", outputData ) );

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
            m_instance.o_output.imageData << imageData;

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

      console.WriteLn( String().Format( "<end><cbr><br>Total output data : %.3lf", totalOutputData ) );

      m_instance.o_output.integrationImageId = resultWindow.MainView().Id();
      m_instance.o_output.weightImageId = weightWindow.MainView().Id();
      m_instance.o_output.numberOfChannels = m_numberOfChannels;
      m_instance.o_output.outputPixels = resultImage.NumberOfPixels();
      m_instance.o_output.integratedPixels = size_type( succeeded )*size_type( m_referenceWidth )*size_type( m_referenceHeight );
      m_instance.o_output.outputData = totalOutputData;

      FITSKeywordArray keywords;
      resultWindow.GetKeywords( keywords );

      keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Integration with " + PixInsightVersion::AsString() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with " + Module->ReadableVersion() )
               << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with DrizzleIntegration process" )
               << FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "DrizzleIntegration.scale: %.2lf", m_instance.p_scale ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                      IsoString().Format( "DrizzleIntegration.dropShrink: %.2lf", m_instance.p_dropShrink ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     "DrizzleIntegration.kernelFunction: " + TheDZKernelFunctionParameter->ElementId( m_instance.p_kernelFunction ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.kernelGridSize: %d", m_instance.p_kernelGridSize ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.originX: %.2f", m_instance.p_origin.x ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.originY: %.2f", m_instance.p_origin.y ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     "DrizzleIntegration.enableCFA: " + IsoString( bool( m_instance.p_enableCFA ) ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     "DrizzleIntegration.cfaPattern: '" + IsoString( m_instance.p_cfaPattern ) + '\'' )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     "DrizzleIntegration.enableRejection: " + IsoString( bool( m_instance.p_enableRejection ) ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     "DrizzleIntegration.enableImageWeighting: " + IsoString( bool( m_instance.p_enableImageWeighting ) ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     "DrizzleIntegration.enableSurfaceSplines: " + IsoString( bool( m_instance.p_enableSurfaceSplines ) ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.referenceDimensions: width=%d, height=%d", m_referenceWidth, m_referenceHeight ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.drizzleGeometry: left=%d, top=%d, width=%d, height=%d",
                                                         m_origin.x, m_origin.y, m_width, m_height ) );
      if ( m_instance.p_useROI )
         keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.regionOfInterest: left=%d, top=%d, width=%d, height=%d",
                                                         roi.x0, roi.y0, roi.Width(), roi.Height() ) );

      keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.pixelSize: %.3lf", m_pixelSize ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.numberOfImages: %d", succeeded ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.outputPixels: %lu", m_instance.o_output.outputPixels ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.integratedPixels: %lu", m_instance.o_output.integratedPixels ) )
               << FITSHeaderKeyword( "HISTORY", IsoString(),
                                     IsoString().Format( "DrizzleIntegration.outputData: %.3lf", totalOutputData ) );

      if ( !ignoringPedestal )
         if ( havePedestal )
            if ( pedestal > 0 )
            {
               keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                    IsoString().Format( "DrizzleIntegration.outputPedestal: %.4lg DN", pedestal ) )
                        << FITSHeaderKeyword( "PEDESTAL",
                                    IsoString().Format( "%.4lg", pedestal ), "Value in DN added to enforce positivity" );

               console.NoteLn( String().Format( "* PEDESTAL keyword created with value: %.4lg DN", pedestal ) );
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
      return p_inputData[tableRow].path.Begin();
   if ( p == TheDZInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheDZInputDirectoryParameter )
      return p_inputDirectory.Begin();
   if ( p == TheDZScaleParameter )
      return &p_scale;
   if ( p == TheDZDropShrinkParameter )
      return &p_dropShrink;
   if ( p == TheDZKernelFunctionParameter )
      return &p_kernelFunction;
   if ( p == TheDZKernelGridSizeParameter )
      return &p_kernelGridSize;
   if ( p == TheDZOriginXParameter )
      return &p_origin.x;
   if ( p == TheDZOriginYParameter )
      return &p_origin.y;
   if ( p == TheDZEnableCFAParameter )
      return &p_enableCFA;
   if ( p == TheDZCFAPatternParameter )
      return p_cfaPattern.Begin();
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
      return o_output.integrationImageId.Begin();
   if ( p == TheDZWeightImageIdParameter )
      return o_output.weightImageId.Begin();
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
      return o_output.imageData[tableRow].filePath.Begin();
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

   return nullptr;
}

// ----------------------------------------------------------------------------

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
   else if ( p == TheDZCFAPatternParameter )
   {
      p_cfaPattern.Clear();
      if ( sizeOrLength > 0 )
         p_cfaPattern.SetLength( sizeOrLength );
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

// ----------------------------------------------------------------------------

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
   if ( p == TheDZCFAPatternParameter )
      return p_cfaPattern.Length();

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

} // pcl

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationInstance.cpp - Released 2017-05-02T09:43:00Z
