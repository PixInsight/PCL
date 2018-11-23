//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0420
// ----------------------------------------------------------------------------
// CurvesTransformationInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "CurvesTransformationInstance.h"

#include <pcl/AkimaInterpolation.h>
#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/CubicSplineInterpolation.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearInterpolation.h>
#include <pcl/MuteStatus.h>
#include <pcl/Mutex.h>
#include <pcl/ReferenceArray.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CurveBase::interpolator* CurveBase::InitInterpolator() const
{
   interpolator* i = nullptr;
   switch ( Type() )
   {
   case CurveType::AkimaSubsplines:
      if ( Length() >= 5)
      {
         i = new AkimaInterpolation<double>;
         break;
      }
      // else fall through
   case CurveType::CubicSpline:
      i = new CubicSplineInterpolation<double>;
      break;
   case CurveType::Linear:
      i = new LinearInterpolation<double>;
      break;
   default: // ?!
      throw Error( "CurveBase: Internal error: Invalid curve type" );
   }

   i->Initialize( XVector(), YVector(), Length() );
   return i;
}

// ----------------------------------------------------------------------------

CurvesTransformationInstance::CurvesTransformationInstance( const MetaProcess* P ) :
   ProcessImplementation( P )
{
}

CurvesTransformationInstance::CurvesTransformationInstance( const CurvesTransformationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void CurvesTransformationInstance::Assign( const ProcessImplementation& p )
{
   const CurvesTransformationInstance* x = dynamic_cast<const CurvesTransformationInstance*>( &p );
   if ( x != 0 )
      for ( int i = 0; i < CurveIndex::NumberOfCurves; ++i )
         C[i] = x->C[i];
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInstance::Validate( String& info )
{
   for ( int i = 0; i < CurveIndex::NumberOfCurves; ++i )
   {
      size_type n = C[i].Length();

      if ( n < 2 || C[i].X( 0 ) != 0 || C[i].X( n-1 ) != 1 )
      {
         info = "Invalid ending point(s) in curve #" + String( i );
         return false;
      }

      for ( size_type j = 0; j < n; ++j )
         if ( C[i].X( j ) < 0 || C[i].X( j ) > 1 || C[i].Y( j ) < 0 || C[i].Y( j ) > 1 )
         {
            info = "Point coordinate(s) out of range in curve #" + String( i ) + ", index " + String( j );
            return false;
         }
   }

   info.Clear();
   return true;
}

// ----------------------------------------------------------------------------

UndoFlags CurvesTransformationInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "CurvesTransformation cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CurvesEngine
{
public:

   static void Apply( ImageVariant& image, const CurvesTransformationInstance& instance, bool useLUT = false )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32 : Apply( static_cast<Image&>( *image ), instance, useLUT ); break;
         case 64 : Apply( static_cast<DImage&>( *image ), instance, useLUT ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8 : Apply( static_cast<UInt8Image&>( *image ), instance, useLUT ); break;
         case 16 : Apply( static_cast<UInt16Image&>( *image ), instance, useLUT ); break;
         case 32 : Apply( static_cast<UInt32Image&>( *image ), instance, useLUT ); break;
         }
   }

   template <class P>
   static void Apply( GenericImage<P>& image, const CurvesTransformationInstance& instance, bool useLUT = false )
   {
      int numberOfCurves = 0;

      if ( !instance[CurveIndex::RGBK].IsIdentity() )
         numberOfCurves = image.NumberOfNominalChannels();

      if ( image.IsColor() )
      {
         for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
            if ( !instance[c].IsIdentity() )
               ++numberOfCurves;
         if ( !instance[CurveIndex::L].IsIdentity() || !instance[CurveIndex::a].IsIdentity() || !instance[CurveIndex::b].IsIdentity() || !instance[CurveIndex::c].IsIdentity() )
            ++numberOfCurves;
         if ( !instance[CurveIndex::H].IsIdentity() || !instance[CurveIndex::S].IsIdentity() )
            ++numberOfCurves;
      }

      if ( image.HasAlphaChannels() && !instance[CurveIndex::A].IsIdentity() )
         ++numberOfCurves;

      if ( numberOfCurves == 0 )
      {
         Console().WriteLn( "<end><cbr>&lt;* identity *&gt;" );
         return;
      }

      size_type N = image.NumberOfPixels();

      int numberOfThreads = Thread::NumberOfThreads( N, 256 );
      size_type pixelsPerThread = N/numberOfThreads;

      image.Status().Initialize( "Curves transformation", numberOfCurves*N );

      ThreadData data( image, numberOfCurves*N );
      if ( useLUT )
         data.lut.Generate( image, instance );

      ReferenceArray<CurvesThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new CurvesThread<P>( instance, data, image,
                                           i*pixelsPerThread,
                                           (j < numberOfThreads) ? j*pixelsPerThread : N ) );

      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      image.Status() = data.status;
   }

private:

   typedef CurveBase::interpolator interpolator;

   static const int lutLen = 65536;
   static const int lutMax = lutLen-1;

   struct LUT
   {
      IVector R;
      IVector G;
      IVector B;
      IVector K;
      IVector A;
      DVector L;
      DVector a;
      DVector b;
      DVector c;
      DVector H;
      DVector S;

      LUT()
      {
      }

      LUT( const LUT& x ) :
      R( x.R ), G( x.G ), B( x.B ), K( x.K ), A( x.A ), L( x.L ), a( x.a ), b( x.b ), c( x.c ), H( x.H ), S( x.S )
      {
      }

      LUT( const AbstractImage& image, const CurvesTransformationInstance& instance )
      {
         Generate( image, instance );
      }

      void Generate( const AbstractImage& image, const CurvesTransformationInstance& instance )
      {
         R = G = B = K = A = L = a = b = c = H = S = DVector();

         bool isColor = image.IsColor();
         bool isR = isColor && !instance[CurveIndex::R].IsIdentity();
         bool isG = isColor && !instance[CurveIndex::G].IsIdentity();
         bool isB = isColor && !instance[CurveIndex::B].IsIdentity();
         bool isK = !instance[CurveIndex::RGBK].IsIdentity();
         bool isA = image.HasAlphaChannels() && !instance[CurveIndex::A].IsIdentity();
         bool isL = isColor && !instance[CurveIndex::L].IsIdentity();
         bool isa = isColor && !instance[CurveIndex::a].IsIdentity();
         bool isb = isColor && !instance[CurveIndex::b].IsIdentity();
         bool isc = isColor && !instance[CurveIndex::c].IsIdentity();
         bool isH = isColor && !instance[CurveIndex::H].IsIdentity();
         bool isS = isColor && !instance[CurveIndex::S].IsIdentity();

         AutoPointer<interpolator> iR;
         AutoPointer<interpolator> iG;
         AutoPointer<interpolator> iB;
         AutoPointer<interpolator> iK;
         AutoPointer<interpolator> iA;
         AutoPointer<interpolator> iL;
         AutoPointer<interpolator> ia;
         AutoPointer<interpolator> ib;
         AutoPointer<interpolator> ic;
         AutoPointer<interpolator> iH;
         AutoPointer<interpolator> iS;

         if ( isR )
         {
            R = IVector( lutLen );
            iR = instance[CurveIndex::R].InitInterpolator();
         }

         if ( isG )
         {
            G = IVector( lutLen );
            iG = instance[CurveIndex::G].InitInterpolator();
         }

         if ( isB )
         {
            B = IVector( lutLen );
            iB = instance[CurveIndex::B].InitInterpolator();
         }

         if ( isK )
         {
            K = IVector( lutLen );
            iK = instance[CurveIndex::RGBK].InitInterpolator();
         }

         if ( isA )
         {
            A = IVector( lutLen );
            iA = instance[CurveIndex::A].InitInterpolator();
         }

         if ( isL )
         {
            L = DVector( lutLen );
            iL = instance[CurveIndex::L].InitInterpolator();
         }

         if ( isa )
         {
            a = DVector( lutLen );
            ia = instance[CurveIndex::a].InitInterpolator();
         }

         if ( isb )
         {
            b = DVector( lutLen );
            ib = instance[CurveIndex::b].InitInterpolator();
         }

         if ( isc )
         {
            c = DVector( lutLen );
            ic = instance[CurveIndex::c].InitInterpolator();
         }

         if ( isH )
         {
            H = DVector( lutLen );
            iH = instance[CurveIndex::H].InitInterpolator();
         }

         if ( isS )
         {
            S = DVector( lutLen );
            iS = instance[CurveIndex::S].InitInterpolator();
         }

         for ( int i = 0; i < lutLen; ++i )
         {
            double f = double( i )/lutMax;
            if ( isR ) R[i] = RoundI( lutMax*CurveBase::Interpolate( iR, f ) );
            if ( isG ) G[i] = RoundI( lutMax*CurveBase::Interpolate( iG, f ) );
            if ( isB ) B[i] = RoundI( lutMax*CurveBase::Interpolate( iB, f ) );
            if ( isK ) K[i] = RoundI( lutMax*CurveBase::Interpolate( iK, f ) );
            if ( isA ) A[i] = RoundI( lutMax*CurveBase::Interpolate( iA, f ) );
            if ( isL ) L[i] = CurveBase::Interpolate( iL, f );
            if ( isa ) a[i] = CurveBase::Interpolate( ia, f );
            if ( isb ) b[i] = CurveBase::Interpolate( ib, f );
            if ( isc ) c[i] = CurveBase::Interpolate( ic, f );
            if ( isH ) H[i] = CurveBase::Interpolate( iH, f );
            if ( isS ) S[i] = CurveBase::Interpolate( iS, f );
         }
      }

      operator bool() const
      {
         return R || G || B || K || A || L || a || b || c || H || S;
      }

      static uint16 InterpolateUInt( const IVector& lut, uint16 u16 )
      {
         return lut[u16];
      }

      static uint8 InterpolateUInt( const IVector& lut, uint8 u8 )
      {
         return uint8( lut[int( u8 )*uint8_to_uint16]/uint8_to_uint16 );
      }

      static uint32 InterpolateUInt( const IVector& lut, uint32 u32 )
      {
         return uint32( lut[RoundI( u32*uint32_to_uint16 )] )*uint16_to_uint32;
      }

      static float InterpolateUInt( const IVector& lut, float f )
      {
         return float( lut[RoundI( f*lutMax )] )/lutMax;
      }

      static double InterpolateUInt( const IVector& lut, double d )
      {
         return double( lut[RoundI( d*lutMax )] )/lutMax;
      }
   };

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const AbstractImage& image, size_type count ) :
      AbstractImage::ThreadData( image, count )
      {
      }

      LUT lut;
   };

   template <class P>
   class CurvesThread : public Thread
   {
   public:

      CurvesThread( const CurvesTransformationInstance& instance, const ThreadData& data,
                    GenericImage<P>& image, size_type start, size_type end ) :
      Thread(), m_instance( instance ), m_data( data ), m_image( image ), m_start( start ), m_end( end )
      {
      }

      virtual void Run()
      {
         if ( m_data.lut )
         {
            if ( m_data.lut.R )
               RGBATransformation( 0, m_data.lut.R );
            if ( m_data.lut.G )
               RGBATransformation( 1, m_data.lut.G );
            if ( m_data.lut.B )
               RGBATransformation( 2, m_data.lut.B );

            if ( m_data.lut.K )
               for ( int c = 0; c < m_image.NumberOfNominalChannels(); ++c )
                  RGBATransformation( c, m_data.lut.K );

            if ( m_data.lut.A )
               RGBATransformation( m_image.NumberOfNominalChannels(), m_data.lut.A );

            if ( m_data.lut.c )
            {
               if ( m_data.lut.a || m_data.lut.b )
                  LabcTransformation( m_data.lut.L, m_data.lut.a, m_data.lut.b, m_data.lut.c );
               else
                  LchTransformation( m_data.lut.L, m_data.lut.c );
            }
            else if ( m_data.lut.L || m_data.lut.a || m_data.lut.b )
               LabTransformation( m_data.lut.L, m_data.lut.a, m_data.lut.b );

            if ( m_data.lut.H || m_data.lut.S )
               HSVTransformation( m_data.lut.H, m_data.lut.S );
         }
         else
         {
            if ( m_image.IsColor() )
               for ( int c = 0; c < m_image.NumberOfNominalChannels(); ++c )
                  if ( IsCurve( c ) )
                     RGBATransformation( c, m_instance[c] );

            if ( IsCurve( CurveIndex::RGBK ) )
               for ( int c = 0; c < m_image.NumberOfNominalChannels(); ++c )
                  RGBATransformation( c, m_instance[CurveIndex::RGBK] );

            if ( m_image.HasAlphaChannels() && IsCurve( CurveIndex::A ) )
               RGBATransformation( m_image.NumberOfNominalChannels(), m_instance[CurveIndex::A] );

            if ( m_image.IsColor() )
            {
               if ( IsCurve( CurveIndex::c ) )
               {
                  if ( IsCurve( CurveIndex::a ) || IsCurve( CurveIndex::b ) )
                     LabcTransformation( m_instance[CurveIndex::L], m_instance[CurveIndex::a], m_instance[CurveIndex::b], m_instance[CurveIndex::c] );
                  else
                     LchTransformation( m_instance[CurveIndex::L], m_instance[CurveIndex::c] );
               }
               else if ( IsCurve( CurveIndex::L ) || IsCurve( CurveIndex::a ) || IsCurve( CurveIndex::b ) )
                  LabTransformation( m_instance[CurveIndex::L], m_instance[CurveIndex::a], m_instance[CurveIndex::b] );

               if ( IsCurve( CurveIndex::H ) || IsCurve( CurveIndex::S ) )
                  HSVTransformation( m_instance[CurveIndex::H], m_instance[CurveIndex::S] );
            }
         }
      }

   private:

      const CurvesTransformationInstance& m_instance;
      const ThreadData&                   m_data;
      GenericImage<P>&                    m_image;
      size_type                           m_start;
      size_type                           m_end;

      bool IsCurve( int c ) const
      {
         return !m_instance[c].IsIdentity();
      }

      void RGBATransformation( int c, const Curve& C )
      {
         INIT_THREAD_MONITOR()

         AutoPointer<interpolator> i( C.InitInterpolator() );

         typename P::sample* p  = m_image[c] + m_start;
         typename P::sample* pN = m_image[c] + m_end;

         for ( ; p < pN; ++p )
         {
            double f;
            P::FromSample( f, *p );
            *p = P::ToSample( CurveBase::Interpolate( i, f ) );
            UPDATE_THREAD_MONITOR( 16384 )
         }
      }

      void RGBATransformation( int c, const IVector& lut )
      {
         INIT_THREAD_MONITOR()

         typename P::sample* p  = m_image[c] + m_start;
         typename P::sample* pN = m_image[c] + m_end;

         for ( ; p < pN; ++p )
         {
            *p = LUT::InterpolateUInt( lut, *p );
            UPDATE_THREAD_MONITOR( 16384 )
         }
      }

      void LabTransformation( const Curve& cL, const Curve& ca, const Curve& cb )
      {
         INIT_THREAD_MONITOR()

         AutoPointer<interpolator> iL, ia, ib;

         if ( !cL.IsIdentity() )
            iL = cL.InitInterpolator();
         if ( !ca.IsIdentity() )
            ia = ca.InitInterpolator();
         if ( !cb.IsIdentity() )
            ib = cb.InitInterpolator();

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B, L, a, b;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            rgbws.RGBToCIELab( L, a, b, R, G, B );
            rgbws.CIELabToRGB( R, G, B, iL ? CurveBase::Interpolate( iL, L ) : L,
                                        ia ? CurveBase::Interpolate( ia, a ) : a,
                                        ib ? CurveBase::Interpolate( ib, b ) : b );
            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 4096 )
         }
      }

      void LabTransformation( const DVector& lutL, const DVector& luta, const DVector& lutb )
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B, L, a, b;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            rgbws.RGBToCIELab( L, a, b, R, G, B );
            rgbws.CIELabToRGB( R, G, B, lutL ? lutL[RoundI( L*lutMax )] : L,
                                        luta ? luta[RoundI( a*lutMax )] : a,
                                        lutb ? lutb[RoundI( b*lutMax )] : b );
            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 4096 )
         }
      }

      void LchTransformation( const Curve& cL, const Curve& cc )
      {
         INIT_THREAD_MONITOR()

         AutoPointer<interpolator> iL, ic;

         if ( !cL.IsIdentity() )
            iL = cL.InitInterpolator();
         if ( !cc.IsIdentity() )
            ic = cc.InitInterpolator();

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B, L, c, h;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            rgbws.RGBToCIELch( L, c, h, R, G, B );
            rgbws.CIELchToRGB( R, G, B, iL ? CurveBase::Interpolate( iL, L ) : L,
                                        ic ? CurveBase::Interpolate( ic, c ) : c,
                                        h );
            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 4096 )
         }
      }

      void LchTransformation( const DVector& lutL, const DVector& lutc )
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B, L, c, h;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            rgbws.RGBToCIELch( L, c, h, R, G, B );
            rgbws.CIELchToRGB( R, G, B, lutL ? lutL[RoundI( L*lutMax )] : L,
                                        lutc ? lutc[RoundI( c*lutMax )] : c,
                                        h );
            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 4096 )
         }
      }

      void LabcTransformation( const Curve& cL, const Curve& ca, const Curve& cb, const Curve& cc )
      {
         INIT_THREAD_MONITOR()

         AutoPointer<interpolator> iL, ia, ib, ic;

         if ( !cL.IsIdentity() )
            iL = cL.InitInterpolator();
         if ( !ca.IsIdentity() )
            ia = ca.InitInterpolator();
         if ( !cb.IsIdentity() )
            ib = cb.InitInterpolator();
         if ( !cb.IsIdentity() )
            ic = cc.InitInterpolator();

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            if ( ia || ib )
            {
               RGBColorSystem::sample L, a, b;
               rgbws.RGBToCIELab( L, a, b, R, G, B );
               rgbws.CIELabToRGB( R, G, B, iL ? CurveBase::Interpolate( iL, L ) : L,
                                           ia ? CurveBase::Interpolate( ia, a ) : a,
                                           ib ? CurveBase::Interpolate( ib, b ) : b );
            }

            if ( ic )
            {
               RGBColorSystem::sample L, c, h;
               rgbws.RGBToCIELch( L, c, h, R, G, B );
               rgbws.CIELchToRGB( R, G, B, (iL && !ia && !ib) ? CurveBase::Interpolate( iL, L ) : L,
                                           CurveBase::Interpolate( ic, c ),
                                           h );
            }

            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 2048 )
         }
      }

      void LabcTransformation( const DVector& lutL, const DVector& luta, const DVector& lutb, const DVector& lutc )
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            if ( luta || lutb )
            {
               RGBColorSystem::sample L, a, b;
               rgbws.RGBToCIELab( L, a, b, R, G, B );
               rgbws.CIELabToRGB( R, G, B, lutL ? lutL[RoundI( L*lutMax )] : L,
                                           luta ? luta[RoundI( a*lutMax )] : a,
                                           lutb ? lutb[RoundI( b*lutMax )] : b );
            }

            if ( lutc )
            {
               RGBColorSystem::sample L, c, h;
               rgbws.RGBToCIELch( L, c, h, R, G, B );
               rgbws.CIELchToRGB( R, G, B, (lutL && !luta && !lutb) ? lutL[RoundI( L*lutMax )] : L,
                                           lutc[RoundI( c*lutMax )],
                                           h );
            }

            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 2048 )
         }
      }

      void HSVTransformation( const Curve& cH, const Curve& cS )
      {
         INIT_THREAD_MONITOR()

         AutoPointer<interpolator> iH, iS;

         if ( !cH.IsIdentity() )
            iH = cH.InitInterpolator();
         if ( !cS.IsIdentity() )
            iS = cS.InitInterpolator();

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B, H, S, V, L;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            rgbws.RGBToHSVL( H, S, V, L, R, G, B );
            rgbws.HSVLToRGB( R, G, B, iH ? CurveBase::Interpolate( iH, H ) : H,
                                      iS ? CurveBase::Interpolate( iS, S ) : S,
                                      V,
                                      L );
            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 4096 )
         }
      }

      void HSVTransformation( const DVector& lutH, const DVector& lutS )
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         for ( ; pR < pN; ++pR, ++pG, ++pB )
         {
            RGBColorSystem::sample R, G, B, H, S, V, L;
            P::FromSample( R, *pR );
            P::FromSample( G, *pG );
            P::FromSample( B, *pB );

            rgbws.RGBToHSVL( H, S, V, L, R, G, B );
            rgbws.HSVLToRGB( R, G, B, lutH ? lutH[RoundI( H*lutMax )] : H,
                                      lutS ? lutS[RoundI( S*lutMax )] : S,
                                      V,
                                      L );
            *pR = P::ToSample( R );
            *pG = P::ToSample( G );
            *pB = P::ToSample( B );

            UPDATE_THREAD_MONITOR( 4096 )
         }
      }
   };
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CurvesTransformationInstance::Preview( UInt16Image& image ) const
{
   MuteStatus status;
   image.SetStatusCallback( &status );
   CurvesEngine::Apply( image, *this, true/*useLUT*/ );
   image.ResetSelections();
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   CurvesEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* CurvesTransformationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   const TableIndexer* i = dynamic_cast<const TableIndexer*>( p );
   if ( i == 0 )
      return 0;

   Curve& c = C[i->CurveTableIndex()];

   const CurvePointValue* v = dynamic_cast<const CurvePointValue*>( p );
   if ( v != 0 )
      if ( v->IsX() )
         return static_cast<void*>( const_cast<Curve::input_list::iterator>( c.x.At( tableRow ) ) );
      else
         return static_cast<void*>( c.y.At( tableRow ) );

   const CurveType* t = dynamic_cast<const CurveType*>( p );
   if ( t == 0 )
      return 0;

   return &c.type;
}

bool CurvesTransformationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   const CurveTable* t = dynamic_cast<const CurveTable*>( p );
   if ( t == 0 )
      return false;

   int i = t->CurveTableIndex();

   C[i].x.Clear();
   C[i].y.Clear();

   C[i].x.Add( 0.0, sizeOrLength );
   C[i].y.Add( 0.0, sizeOrLength );

   return true;
}

size_type CurvesTransformationInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   const CurveTable* t = dynamic_cast<const CurveTable*>( p );
   if ( t == 0 )
      return 0;

   return C[t->CurveTableIndex()].Length();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CurvesTransformationInstance.cpp - Released 2018-11-23T18:45:58Z
