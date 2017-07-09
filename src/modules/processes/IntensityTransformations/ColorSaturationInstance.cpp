//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// ColorSaturationInstance.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ColorSaturationInstance.h"

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

ColorSaturationInstance::ColorSaturationInstance( const MetaProcess* P ) :
ProcessImplementation( P ),
C(), hueShift( 0 )
{
}

ColorSaturationInstance::ColorSaturationInstance( const ColorSaturationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void ColorSaturationInstance::Assign( const ProcessImplementation& p )
{
   const ColorSaturationInstance* x = dynamic_cast<const ColorSaturationInstance*>( &p );
   if ( x != 0 )
   {
      C = x->C;
      hueShift = x->hueShift;
   }
}

// ----------------------------------------------------------------------------

bool ColorSaturationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ColorSaturation cannot be executed on complex images.";
      return false;
   }
   else if ( !view.Image()->IsColor() )
   {
      whyNot = "ColorSaturation cannot be executed on grayscale images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ColorSaturationEngine
{
public:

   static void Apply( ImageVariant& image, const ColorSaturationInstance& instance, bool useLUT = false )
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
   static void Apply( GenericImage<P>& image, const ColorSaturationInstance& instance, bool useLUT = false )
   {
      if ( instance.Curve().IsIdentity() )
      {
         Console().WriteLn( "<end><cbr>&lt;* Identity *&gt;" );
         return;
      }

      size_type N = image.NumberOfPixels();

      int numberOfThreads = Thread::NumberOfThreads( N, 16 );
      size_type pixelsPerThread = N/numberOfThreads;

      image.Status().Initialize( "Color saturation transformation, HSVL space", N );

      ThreadData data( image, N );
      if ( useLUT )
         data.lut = MakeLUT( instance );

      ReferenceArray<ColorSaturationThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new ColorSaturationThread<P>( instance, data, image,
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

   /*
    * Transform saturation bias to a factor in the range [1/x,...,1,...,x]
    */
   static double BiasToSaturationFactor( double k )
   {
      k += (k < 0) ? -1 : +1; // Ensure central range value = 1
      if ( k < 0 )   // Negative biases are dividing factors
         k = 1/(-k);
      return k;
   }

   static DVector MakeLUT( const ColorSaturationInstance& instance )
   {
      DVector lut( lutLen );
      AutoPointer<interpolator> iHS( instance.Curve().InitInterpolator() );
      for ( int i = 0; i < lutLen; ++i )
         lut[i] = BiasToSaturationFactor( HSCurve::Interpolate( iHS, instance.UnshiftHueValue( double( i )/lutMax ) ) );
      return lut;
   }

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const AbstractImage& image, size_type count ) :
      AbstractImage::ThreadData( image, count )
      {
      }

      DVector lut;
   };

   template <class P>
   class ColorSaturationThread : public Thread
   {
   public:

      ColorSaturationThread( const ColorSaturationInstance& instance, const ThreadData& data,
                             GenericImage<P>& image, size_type start, size_type end ) :
      Thread(), m_instance( instance ), m_data( data ), m_image( image ), m_start( start ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* pR = m_image[0] + m_start;
         typename P::sample* pG = m_image[1] + m_start;
         typename P::sample* pB = m_image[2] + m_start;
         typename P::sample* pN = m_image[0] + m_end;

         if ( m_data.lut )
         {
            for ( ; pR < pN; ++pR, ++pG, ++pB )
            {
               RGBColorSystem::sample R, G, B, H, S, V, L;
               P::FromSample( R, *pR );
               P::FromSample( G, *pG );
               P::FromSample( B, *pB );

               rgbws.RGBToHSVL( H, S, V, L, R, G, B );
               rgbws.HSVLToRGB( R, G, B, H, Range( S*m_data.lut[RoundI( H*lutMax )], 0.0, 1.0 ), V, L );
               *pR = P::ToSample( R );
               *pG = P::ToSample( G );
               *pB = P::ToSample( B );

               UPDATE_THREAD_MONITOR( 4096 )
            }
         }
         else
         {
            AutoPointer<interpolator> iHS( m_instance.Curve().InitInterpolator() );

            for ( ; pR < pN; ++pR, ++pG, ++pB )
            {
               RGBColorSystem::sample R, G, B, H, S, V, L;
               P::FromSample( R, *pR );
               P::FromSample( G, *pG );
               P::FromSample( B, *pB );

               rgbws.RGBToHSVL( H, S, V, L, R, G, B );

               double k = HSCurve::Interpolate( iHS, m_instance.UnshiftHueValue( H ) );
               if ( k != 0 )
               {
                  rgbws.HSVLToRGB( R, G, B, H, Range( S*BiasToSaturationFactor( k ), 0.0, 1.0 ), V, L );
                  *pR = P::ToSample( R );
                  *pG = P::ToSample( G );
                  *pB = P::ToSample( B );
               }

               UPDATE_THREAD_MONITOR( 4096 )
            }
         }
      }

   private:

      const ColorSaturationInstance& m_instance;
      const ThreadData&              m_data;
      GenericImage<P>&               m_image;
      size_type                      m_start;
      size_type                      m_end;
   };
};

// ----------------------------------------------------------------------------

void ColorSaturationInstance::Preview( UInt16Image& image ) const
{
   MuteStatus status;
   image.SetStatusCallback( &status );
   ColorSaturationEngine::Apply( image, *this, true/*useLUT*/ );
   image.ResetSelections();
}

// ----------------------------------------------------------------------------

bool ColorSaturationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() || !image.IsColor() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   ColorSaturationEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* ColorSaturationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheHueShiftParameter )
      return &hueShift;

   const CurvePointValue* v = dynamic_cast<const CurvePointValue*>( p );

   if ( v != 0 )
      if ( v->IsX() )
         return static_cast<void*>( const_cast<HSCurve::input_list::iterator>( C.x.At( tableRow ) ) );
      else
         return static_cast<void*>( C.y.At( tableRow ) );

   const CurveType* t = dynamic_cast<const CurveType*>( p );

   if ( t == 0 )
      return 0;

   return &C.type;
}

bool ColorSaturationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* /*p*/, size_type /*tableRow*/ )
{
   C.x.Clear();
   C.y.Clear();

   C.x.Add( 0.0, sizeOrLength );
   C.y.Add( 0.0, sizeOrLength );

   return true;
}

size_type ColorSaturationInstance::ParameterLength( const MetaParameter* /*p*/, size_type /*tableRow*/ ) const
{
   return C.Length();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorSaturationInstance.cpp - Released 2017-07-09T18:07:33Z
