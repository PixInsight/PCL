//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/SurfaceSplines.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_SurfaceSplines_h
#define __PCL_SurfaceSplines_h

#include <pcl/Array.h>
#include <pcl/BicubicInterpolation.h>
#include <pcl/Console.h>
#include <pcl/File.h>
#include <pcl/Matrix.h>
#include <pcl/Point.h>
#include <pcl/Rectangle.h>
#include <pcl/ReferenceArray.h>
#include <pcl/SurfaceSpline.h>
#include <pcl/Thread.h>

/*
 * ### TODO: Rename SurfaceSplines to something more descriptive.
 * ### TODO: Rename GridInterpolation to something more descriptive.
 * ### TODO: Document this file.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

template <typename T>
class SerializableSurfaceSpline : public SurfaceSpline<T>
{
public:

   typedef typename SurfaceSpline<T>::vector_type  vector_type;

   SerializableSurfaceSpline() : SurfaceSpline<T>()
   {
   }

   SerializableSurfaceSpline( const SurfaceSpline<T>& S ) : SurfaceSpline<T>( S )
   {
   }

   virtual ~SerializableSurfaceSpline()
   {
   }

   SerializableSurfaceSpline& operator =( const SurfaceSpline<T>& S )
   {
      return SurfaceSpline<T>::operator =( S );
   }

   void ToDrizzleData( File& f ) const
   {
      if ( this->IsValid() )
      {
         f.OutText( "x{" );
         for ( int i = 0, n = this->m_x.Length()-1; ; )
         {
            f.OutText( IsoString().Format( "%.16g,", this->m_x[i] ) );
            if ( ++i == n )
            {
               f.OutText( IsoString().Format( "%.16g}", this->m_x[n] ) );
               break;
            }
         }
         f.OutText( "y{" );
         for ( int i = 0, n = this->m_y.Length()-1; ; )
         {
            f.OutText( IsoString().Format( "%.16g,", this->m_y[i] ) );
            if ( ++i == n )
            {
               f.OutText( IsoString().Format( "%.16g}", this->m_y[n] ) );
               break;
            }
         }
         f.OutText( IsoString().Format( "r0{%.16g}", this->m_r0 ) );
         f.OutText( IsoString().Format( "x0{%.16g}", this->m_x0 ) );
         f.OutText( IsoString().Format( "y0{%.16g}", this->m_y0 ) );
         f.OutText( IsoString().Format( "m{%d}", this->m_order ) );
         f.OutText( IsoString().Format( "r{%.8g}", this->m_smoothing ) );
         if ( this->m_smoothing > 0 )
            if ( !this->m_weights.IsEmpty() )
            {
               f.OutText( "w{" );
               for ( int i = 0, n = this->m_weights.Length()-1; ; )
               {
                  f.OutText( IsoString().Format( "%.8g,", this->m_weights[i] ) );
                  if ( ++i == n )
                  {
                     f.OutText( IsoString().Format( "%.8g}", this->m_weights[n] ) );
                     break;
                  }
               }
            }
         f.OutText( "s{" );
         for ( int i = 0, n = this->m_spline.Length()-1; ; )
         {
            f.OutText( IsoString().Format( "%.16g,", this->m_spline[i] ) );
            if ( ++i == n )
            {
               f.OutText( IsoString().Format( "%.16g}", this->m_spline[n] ) );
               break;
            }
         }
      }
   }

   friend class DrizzleSplineDecoder;
};

// ----------------------------------------------------------------------------

/*
 * Surface spline interpolation in two dimensions
 */
class SurfaceSplines
{
public:

   typedef Array<FPoint>                     point_list;
   typedef SerializableSurfaceSpline<double> spline;

   SurfaceSplines()
   {
   }

   SurfaceSplines( const SurfaceSplines& S ) : m_Sx( S.m_Sx ), m_Sy( S.m_Sy )
   {
   }

   SurfaceSplines( const point_list& P1, const point_list& P2, float smoothness, int order = 2 )
   {
      Initialize( P1, P2, order );
   }

   void SetOrder( int order )
   {
      m_Sx.SetOrder( order );
      m_Sy.SetOrder( order );
   }

   void Initialize( const point_list& P1, const point_list& P2, float smoothness, int order = 2 )
   {
      PCL_PRECONDITION( P1.Length() >= 3 )
      PCL_PRECONDITION( P1.Length() <= P2.Length() )
      PCL_PRECONDITION( order >= 2 )

      m_Sx.Clear();
      m_Sy.Clear();

      m_Sx.SetOrder( order );
      m_Sy.SetOrder( order );

      m_Sx.SetSmoothing( smoothness );
      m_Sy.SetSmoothing( smoothness );

      if ( P1.Length() < 3 || P2.Length() < 3 )
         return;

      DVector X( P1.Length() ),
              Y( P1.Length() ),
              Zx( P1.Length() ),
              Zy( P1.Length() );
      for ( int i = 0; i < X.Length(); ++i )
      {
          X[i] = P1[i].x;
          Y[i] = P1[i].y;
         Zx[i] = P2[i].x;
         Zy[i] = P2[i].y;
      }
      m_Sx.Initialize( X.Begin(), Y.Begin(), Zx.Begin(), X.Length() );
      m_Sy.Initialize( X.Begin(), Y.Begin(), Zy.Begin(), X.Length() );
   }

   void Clear()
   {
      m_Sx.Clear();
      m_Sy.Clear();
   }

   bool IsValid() const
   {
      return m_Sx.IsValid() && m_Sy.IsValid();
   }

   void ToDrizzleData( File& f ) const
   {
      if ( IsValid() )
      {
         f.OutText( "Sx{" );
         m_Sx.ToDrizzleData( f );
         f.OutText( "}Sy{" );
         m_Sy.ToDrizzleData( f );
         f.OutText( "}" );
      }
   }

   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      return DPoint( m_Sx( x, y ), m_Sy( x, y ) );
   }

   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

private:

   spline m_Sx, m_Sy;

   friend class DrizzleDataDecoder;
};

// ----------------------------------------------------------------------------

/*
 * SurfaceSpline / Grid interpolation in two dimensions
 */
class GridInterpolation
{
public:

   GridInterpolation() : m_parallel( true )
   {
   }

   void Initialize( const Rect& r, int d, const SurfaceSplines& S, bool verbose = true )
   {
      m_rect = r;
      m_delta = d;

      int w = r.Width();
      int h = r.Height();
      int rows = 1 + h/m_delta + ((h%m_delta) ? 1 : 0);
      int cols = 1 + w/m_delta + ((w%m_delta) ? 1 : 0);

      m_Gx = DMatrix( rows, cols );
      m_Gy = DMatrix( rows, cols );

      if ( verbose )
         Console().WriteLn( "<end><cbr>Building 2D surface interpolation grid...<flush>" );

      int numberOfThreads = m_parallel ? Thread::NumberOfThreads( rows, 1 ) : 1;
      int rowsPerThread = rows/numberOfThreads;

      ReferenceArray<GridInitializationThread> threads;

      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new GridInitializationThread( *this, S,
                                                    i*rowsPerThread,
                                                    (j < numberOfThreads) ? j*rowsPerThread : rows ) );
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Start( ThreadPriority::DefaultMax, i );
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Wait();

      threads.Destroy();

      m_Ix.Initialize( m_Gx.Begin(), cols, rows );
      m_Iy.Initialize( m_Gy.Begin(), cols, rows );
   }

   bool IsValid() const
   {
      return !(m_Gx.IsEmpty() || m_Gy.IsEmpty());
   }

   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      double fx = (double( x ) - m_rect.x0)/m_delta;
      double fy = (double( y ) - m_rect.y0)/m_delta;
      return DPoint( m_Ix( fx, fy ), m_Iy( fx, fy ) );
   }

   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   void EnableParallelProcessing( bool enable )
   {
      m_parallel = enable;
   }

   void DisableParallelProcessing( bool disable )
   {
      EnableParallelProcessing( !disable );
   }

private:

   typedef BicubicSplineInterpolation<double> grid_interpolation;

   Rect               m_rect;
   int                m_delta;
   DMatrix            m_Gx, m_Gy;
   grid_interpolation m_Ix, m_Iy;
   bool               m_parallel : 1;

   class GridInitializationThread : public Thread
   {
   public:

      GridInitializationThread( GridInterpolation& grid, const SurfaceSplines& splines, int startRow, int endRow ) :
         m_grid( grid ), m_splines( splines ), m_startRow( startRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         for ( int i = m_startRow; i < m_endRow; ++i )
            for ( int j = 0, dx = 0, y = m_grid.m_rect.y0 + i*m_grid.m_delta; j < m_grid.m_Gx.Cols(); ++j, dx += m_grid.m_delta )
            {
               DPoint p = m_splines( m_grid.m_rect.x0 + dx, y );
               m_grid.m_Gx[i][j] = p.x;
               m_grid.m_Gy[i][j] = p.y;
            }
      }

   private:

      GridInterpolation&    m_grid;
      const SurfaceSplines& m_splines;
      int                   m_startRow, m_endRow;
   };

   friend class GridInterpolation::GridInitializationThread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SurfaceSplines_h

// ----------------------------------------------------------------------------
// EOF pcl/SurfaceSplines.h - Released 2015/07/30 17:15:18 UTC
