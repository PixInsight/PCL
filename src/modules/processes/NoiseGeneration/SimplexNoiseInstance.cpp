//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 01.00.02.0227
// ----------------------------------------------------------------------------
// SimplexNoiseInstance.cpp - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
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

#include "SimplexNoiseInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/Random.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SimplexNoiseInstance::SimplexNoiseInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_amount( TheSNAmountParameter->DefaultValue() ),
p_scale( uint32( TheSNScaleParameter->DefaultValue() ) ),
p_offsetX( int32( TheSNOffsetXParameter->DefaultValue() ) ),
p_offsetY( int32( TheSNOffsetYParameter->DefaultValue() ) ),
p_operator( SNOperator::Default )
{
}

// ----------------------------------------------------------------------------

SimplexNoiseInstance::SimplexNoiseInstance( const SimplexNoiseInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void SimplexNoiseInstance::Assign( const ProcessImplementation& p )
{
   const SimplexNoiseInstance* x = dynamic_cast<const SimplexNoiseInstance*>( &p );
   if ( x != 0 )
   {
      p_amount = x->p_amount;
      p_scale = x->p_scale;
      p_offsetX = x->p_offsetX;
      p_offsetY = x->p_offsetY;
      p_operator = x->p_operator;
   }
}

// ----------------------------------------------------------------------------

bool SimplexNoiseInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "SimplexNoise cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

class SimplexNoise
{
public:

   // 2D simplex noise
   static double Noise( double xin, double yin )
   {
      static const double F2 = 0.5*( Sqrt( 3.0 ) - 1.0 );
      static const double G2 = (3.0 - Sqrt( 3.0 ))/6;

      if ( perm == 0 )
      {
         perm = new int[ 512 ];
         for ( int i = 0; i < 512; ++i )
            perm[i] = p[i & 255];
      }

      double n0, n1, n2; // Noise contributions from the three corners

      // Skew the input space to determine which simplex cell we're in

      double s = (xin + yin)*F2; // Hairy factor for 2D

      int i = FastFloor( xin + s );
      int j = FastFloor( yin + s );

      double t = (i + j)*G2;
      double X0 = i - t;    // Unskew the cell origin back to (x,y) space
      double Y0 = j - t;
      double x0 = xin - X0; // The x,y distances from the cell origin
      double y0 = yin - Y0;

      // For the 2D case, the simplex shape is an equilateral triangle.
      // Determine which simplex we are in.
      int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords

      if( x0 > y0 )  // lower triangle, XY order: (0,0)->(1,0)->(1,1)
      {
         i1 = 1;
         j1 = 0;
      }
      else           // upper triangle, YX order: (0,0)->(0,1)->(1,1)
      {
         i1 = 0;
         j1 = 1;
      }

      // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
      // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
      // c = (3-sqrt(3))/6
      double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
      double y1 = y0 - j1 + G2;
      double x2 = x0 - 1.0 + 2.0*G2; // Offsets for last corner in (x,y) unskewed coords
      double y2 = y0 - 1.0 + 2.0*G2;

      // Work out the hashed gradient indices of the three simplex corners
      int ii = i & 255;
      int jj = j & 255;
      int gi0 = perm[ii + perm[jj]] % 12;
      int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
      int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

      // Calculate the contribution from the three corners
      double t0 = 0.5 - x0*x0 - y0*y0;
      if ( t0 < 0 )
         n0 = 0;
      else
      {
         t0 *= t0;
         n0 = t0*t0 * Dot( grad3[gi0], x0, y0 ); // (x,y) of grad3 used for 2D gradient
      }

      double t1 = 0.5 - x1*x1 - y1*y1;
      if( t1 < 0 )
         n1 = 0;
      else
      {
         t1 *= t1;
         n1 = t1*t1 * Dot( grad3[gi1], x1, y1 );
      }

      double t2 = 0.5 - x2*x2 - y2*y2;
      if( t2 < 0 )
         n2 = 0;
      else
      {
         t2 *= t2;
         n2 = t2*t2 * Dot( grad3[gi2], x2, y2 );
      }

      // Add contributions from each corner to get the final noise value.
      // The result is scaled to return values in the interval [-1,1].
      return 70*(n0 + n1 + n2);
   }

private:

   static int grad3[ 12 ][ 3 ];
   static int p[ 256 ];

   // To remove the need for index wrapping, double the permutation table length
   static int* perm;

   static int FastFloor( double x )
   {
      return (x > 0) ? TruncInt( x ) : TruncInt( x-1 );
   }

   static double Dot( int g[], double x, double y )
   {
      return g[0]*x + g[1]*y;
   }
};

int SimplexNoise::grad3[ 12 ][ 3 ] =
{
   {1, 1, 0}, {-1, 1, 0}, {1,-1, 0}, {-1,-1, 0},
   {1, 0, 1}, {-1, 0, 1}, {1, 0,-1}, {-1, 0,-1},
   {0, 1, 1}, { 0,-1, 1}, {0, 1,-1}, { 0,-1,-1}
};

int SimplexNoise::p[ 256 ] =
{
   151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

int* SimplexNoise::perm = 0;

// ----------------------------------------------------------------------------

class SimplexNoiseEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const SimplexNoiseInstance& instance )
   {
      size_type N = image.NumberOfNominalSamples();

      image.Status().Initialize( "Generating 2D simplex noise", N );
      image.Status().DisableInitialization();

      double a = instance.p_amount;
      double a1 = 1 - a;

      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         typename GenericImage<P>::sample_iterator i( image, c );
         for ( int y = 0; y < image.Height(); ++y, image.Status() += image.Width() )
            for ( int x = 0; x < image.Width(); ++x, ++i )
            {
               double v; P::FromSample( v, *i );
               double r = (1 + SimplexNoise::Noise( double( x + instance.p_offsetX )/instance.p_scale,
                                                    double( y + instance.p_offsetY )/instance.p_scale ))/2;
               switch ( instance.p_operator )
               {
               default:
               case SNOperator::Copy:
                  break;
               case SNOperator::Add:
                  r = Min( v + r, 1.0 );
                  break;
               case SNOperator::Sub:
                  r = Max( 0.0, v - r );
                  break;
               case SNOperator::Mul:
                  r *= v;
                  break;
               case SNOperator::Div:
                  r = (r + 1 != 1) ? Range( v/r, 0.0, 1.0 ) : 1.0;
                  break;
               case SNOperator::Pow:
                  r = Pow( v, r );
                  break;
               case SNOperator::Dif:
                  r = Abs( v - r );
                  break;
               case SNOperator::Screen:
                  r = 1 - (1 - v)*(1 - r);
                  break;
               case SNOperator::Or:
                  r = double( uint16( RoundInt( 0xffff*v ) ) | uint16( RoundInt( 0xffff*r ) ) )/0xffff;
                  break;
               case SNOperator::And:
                  r = double( uint16( RoundInt( 0xffff*v ) ) & uint16( RoundInt( 0xffff*r ) ) )/0xffff;
                  break;
               case SNOperator::Xor:
                  r = double( uint16( RoundInt( 0xffff*v ) ) ^ uint16( RoundInt( 0xffff*r ) ) )/0xffff;
                  break;
               case SNOperator::Nor:
                  r = double( ~(uint16( RoundInt( 0xffff*v ) ) | uint16( RoundInt( 0xffff*r ) )) )/0xffff;
                  break;
               case SNOperator::Nand:
                  r = double( ~(uint16( RoundInt( 0xffff*v ) ) & uint16( RoundInt( 0xffff*r ) )) )/0xffff;
                  break;
               case SNOperator::Xnor:
                  r = double( ~(uint16( RoundInt( 0xffff*v ) ) ^ uint16( RoundInt( 0xffff*r ) )) )/0xffff;
                  break;
               }

               *i = P::ToSample( a1*v + a*r );
            }
      }
   }
};

bool SimplexNoiseInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( !image.IsComplexSample() )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: SimplexNoiseEngine::Apply( static_cast<Image&>( *image ), *this ); break;
         case 64: SimplexNoiseEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: SimplexNoiseEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
         case 16: SimplexNoiseEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
         case 32: SimplexNoiseEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
         }

   return true;
}

// ----------------------------------------------------------------------------

void* SimplexNoiseInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSNAmountParameter )
      return &p_amount;
   if ( p == TheSNScaleParameter )
      return &p_scale;
   if ( p == TheSNOffsetXParameter )
      return &p_offsetX;
   if ( p == TheSNOffsetYParameter )
      return &p_offsetY;
   if ( p == TheSNOperatorParameter )
      return &p_operator;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SimplexNoiseInstance.cpp - Released 2015/07/31 11:49:48 UTC
