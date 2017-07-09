//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/PyramidalWaveletTransform.cpp - Released 2017-07-09T18:07:16Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/PyramidalWaveletTransform.h>
#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// Predefined wavelet filters
// ----------------------------------------------------------------------------

PCL_DATA Daubechies4Filter    Daubechies4;
PCL_DATA Daubechies6Filter    Daubechies6;
PCL_DATA Daubechies8Filter    Daubechies8;
PCL_DATA Daubechies10Filter   Daubechies10;
PCL_DATA Daubechies12Filter   Daubechies12;
PCL_DATA Daubechies20Filter   Daubechies20;

// ----------------------------------------------------------------------------

Daubechies4Filter::Daubechies4Filter() : WaveletFilter()
{
   static const double K[] =
   {
       0.4829629131445341,
       0.8365163037378079,
       0.2241438680420134,
      -0.1294095225512604
   };

   Kc = K;
   N = 4;
   Initialize();
}

// ----------------------------------------------------------------------------

Daubechies6Filter::Daubechies6Filter() : WaveletFilter()
{
   static const double K[] =
   {
       0.3326705529500825,
       0.8068915093110924,
       0.4598775021184914,
      -0.1350110200102546,
      -0.0854412738820267,
       0.0352262918857095
   };

   Kc = K;
   N = 6;
   Initialize();
}

// ----------------------------------------------------------------------------

Daubechies8Filter::Daubechies8Filter() : WaveletFilter()
{
   static const double K[] =
   {
       0.230377813309,
       0.714846570553,
       0.630880766793,
      -0.027983769417,
      -0.187034811719,
       0.030841381836,
       0.032883011667,
      -0.010597401785
   };

   Kc = K;
   N = 8;
   Initialize();
}

// ----------------------------------------------------------------------------

Daubechies10Filter::Daubechies10Filter() : WaveletFilter()
{
   static const double K[] =
   {
       0.1601023979741929,
       0.6038292697971895,
       0.7243085284377726,
       0.1384281459013203,
      -0.2422948870663823,
      -0.0322448695846381,
       0.0775714938400459,
      -0.0062414902127983,
      -0.0125807519990820,
       0.0033357252854738
   };

   Kc = K;
   N = 10;
   Initialize();
}

// ----------------------------------------------------------------------------

Daubechies12Filter::Daubechies12Filter() : WaveletFilter()
{
   static const double K[] =
   {
       0.111540743350,
       0.494623890398,
       0.751133908021,
       0.315250351709,
      -0.226264693965,
      -0.129766867567,
       0.097501605587,
       0.027522865530,
      -0.031582039318,
       0.000553842201,
       0.004777257511,
      -0.001077301085
   };

   Kc = K;
   N = 12;
   Initialize();
}

// ----------------------------------------------------------------------------

Daubechies20Filter::Daubechies20Filter() : WaveletFilter()
{
   static const double K[ 20 ] =
   {
       0.026670057901,
       0.188176800078,
       0.527201188932,
       0.688459039454,
       0.281172343661,
      -0.249846424327,
      -0.195946274377,
       0.127369340336,
       0.093057364604,
      -0.071394147166,
      -0.029457536822,
       0.033212674059,
       0.003606553567,
      -0.010733175483,
       0.001395351747,
       0.001992405295,
      -0.000685856695,
      -0.000116466855,
       0.000093588670,
      -0.000013264203
   };

   Kc = K;
   N = 20;
   Initialize();
}

// ----------------------------------------------------------------------------
// Discrete in-place one-dimensional wavelet transform.
// Vector length must be an integer power of two.
// ----------------------------------------------------------------------------

#define IMPLEMENT_1DWAVELET_TRANSFORM( T )                        \
   PCL_PRECONDITION( a != 0 )                                     \
   PCL_PRECONDITION( pcl::Frac( pcl::Log2( double( n ) ) ) == 0 ) \
                                                                  \
   if ( n < 4 )                                                   \
      return;                                                     \
                                                                  \
   size_type nmod = N * n;                                        \
   size_type n1 = n - 1;                                          \
   size_type nh = n >> 1;                                         \
                                                                  \
   Array<T> wksp( n, T( 0 ) );                                    \
                                                                  \
   if ( inverse )                                                 \
   {                                                              \
      for ( size_type ii = 0, i = 1; i <= n; i += 2, ++ii )       \
      {                                                           \
         T ai = a[ii];                                            \
         T ai1 = a[ii+nh];                                        \
         size_type ni = i + nmod + ioff;                          \
         size_type nj = i + nmod + joff;                          \
                                                                  \
         for ( int k = 0; k < N; ++k )                            \
         {                                                        \
            size_type jf = n1 & (ni+k+1);                         \
            size_type jr = n1 & (nj+k+1);                         \
            wksp[jf] += Kc[k] * ai;                               \
            wksp[jr] += Kr[k] * ai1;                              \
         }                                                        \
      }                                                           \
   }                                                              \
   else                                                           \
   {                                                              \
      for ( size_type ii = 0, i = 1; i <= n; i += 2, ++ii )       \
      {                                                           \
         size_type ni = i + nmod + ioff;                          \
         size_type nj = i + nmod + joff;                          \
                                                                  \
         for ( int k = 0; k < N; ++k )                            \
         {                                                        \
            size_type jf = n1 & (ni+k+1);                         \
            size_type jr = n1 & (nj+k+1);                         \
            wksp[ii] += Kc[k] * a[jf];                            \
            wksp[ii+nh] += Kr[k] * a[jr];                         \
         }                                                        \
      }                                                           \
   }                                                              \
                                                                  \
   for ( size_type j = 0; j < n; ++j )                            \
      a[j] = wksp[j];

void WaveletFilter::Apply( float* a, size_type n, bool inverse ) const
{
   IMPLEMENT_1DWAVELET_TRANSFORM( float )
}

void WaveletFilter::Apply( double* a, size_type n, bool inverse ) const
{
   IMPLEMENT_1DWAVELET_TRANSFORM( double )
}

// ----------------------------------------------------------------------------

/*
 * Discrete Two-Dimensional Wavelet Transforms - Pyramidal Algorithm
 *
 * f        Address of data to transform.
 *
 * width    Number of data columns (most rapidly varying matrix subscript).
 *          Must be an integer power of two.
 *
 * height   Number of data rows (most slowly varying matrix subscript).
 *          Must be an integer power of two.
 *
 * flt      Wavelet basis function (wavelet filter).
 *
 * inverse  true: perform inverse wavelet transform.
 *          false: perform wavelet transform.
 *
 * Wavelet transforms are performed in-place: on output, wavelet coefficients
 * replace original function values.
 *
 * Dimensions and sizes must be integer powers of two.
 */

template <class T>
inline void StandardWaveletTransform2D( T* a, int width, int height, const WaveletFilter& wf, bool inverse,
                                        StatusMonitor* status )
{
   PCL_PRECONDITION( a != nullptr )
   PCL_PRECONDITION( width >= 0 && height >= 0 )
   PCL_PRECONDITION( Frac( Log2( double( width ) ) ) == 0 && Frac( Log2( double( height ) ) ) == 0 )

   size_type N = size_type( width )*size_type( height );
   if ( N == 0 )
      return;

   if ( status != nullptr && status->IsInitializationEnabled() )
      status->Initialize( inverse ? "Inverse pyramidal wavelet transform" :
                                    "Pyramidal wavelet transform", width+height );

   Array<T> wksp( pcl::Max( width, height ) );
   size_type nprev = 1;

   // Main loop: i=0 -> height / i=1 -> width
   for ( int i = 0; i < 2; ++i )
   {
      size_type n = (i == 0) ? height : width;
      size_type nnew = n*nprev;

      if ( n > 4 )
      {
         for ( size_type i2 = 0; i2 < N; i2 += nnew )
         {
            for ( size_type i1 = 0; i1 < nprev; ++i1 )
            {
               size_type nz = 0;

               // Copy this row or column into workspace
               for ( size_type i3 = i1+i2, k = 0; k < n; ++k, i3 += nprev )
                  if ( (wksp[k] = a[i3]) != 0 )
                     ++nz;

               if ( nz )   // if this is a non-zero row or column
               {
                  if ( inverse )  // Inverse transform
                     for( size_type nt = 4; nt <= n; nt <<= 1 )
                        wf( wksp.Begin(), nt, true );
                  else // One-dimensional wavelet transform
                     for ( size_type nt = n; nt >= 4; nt >>= 1 )
                        wf( wksp.Begin(), nt, false );

                  // Copy back from workspace
                  for ( size_type i3 = i1+i2, k = 0; k < n; ++k, i3 += nprev )
                     a[i3] = wksp[k];
               }

               if ( status != nullptr )
                  ++*status;
            }
         }
      }
      else
      {
         if ( status != nullptr )
            *status += n;
      }

      nprev = nnew;
   }
}

template <class T>
inline void NonstandardWaveletTransform2D( T* a, size_type size, const WaveletFilter& wf, bool inverse,
                                           StatusMonitor* status )
{
   PCL_PRECONDITION( a != nullptr )
   PCL_PRECONDITION( size >= 0 )
   PCL_PRECONDITION( Frac( Log2( double( size ) ) ) == 0 )

   if ( size == 0 )
      return;

   if ( status != nullptr && status->IsInitializationEnabled() )
      status->Initialize( inverse ? "Inverse nonstandard pyramidal wavelet transform" :
                                    "Nonstandard pyramidal wavelet transform", 4*size - 4 );

   Array<T> wksp( size );

   if ( inverse )
      for ( size_type n = 2; n <= size; n <<= 1 )
         for ( int d = 0; d < 2; ++d )
            for ( size_type i = 0; i < n; ++i )
            {
               size_type nz = 0;

               // Copy this row or column into workspace
               for ( size_type j = 0, i1 = d ? i : i*size, di = d ? size : 1; j < n; ++j, i1 += di )
                  if ( (wksp[j] = a[i1]) != 0 )
                     ++nz;

               if ( nz )   // if this is a non-zero row or column
               {
                  // Do one-dimensional inverse wavelet transform
                  wf( wksp.Begin(), n, true );

                  // Copy back from workspace
                  for ( size_type j = 0, i1 = d ? i : i*size, di = d ? size : 1; j < n; ++j, i1 += di )
                     a[i1] = wksp[j];
               }

               if ( status != nullptr )
                  ++*status;
            }
   else
      for ( size_type n = size; n > 1; n >>= 1 )
         for ( int d = 0; d < 2; ++d )
            for ( size_type i = 0; i < n; ++i )
            {
               size_type nz = 0;

               // Copy this row or column into workspace
               for ( size_type j = 0, i1 = d ? i : i*size, di = d ? size : 1; j < n; ++j, i1 += di )
                  if ( (wksp[j] = a[i1]) != 0 )
                     ++nz;

               if ( nz )   // if this is a non-zero row or column
               {
                  // Do one-dimensional wavelet transform
                  wf( wksp.Begin(), n, false );

                  // Copy back from workspace
                  for ( size_type j = 0, i1 = d ? i : i*size, di = d ? size : 1; j < n; ++j, i1 += di )
                     a[i1] = wksp[j];
               }

               if ( status != nullptr )
                  ++*status;
            }
}

void PyramidalWaveletTransform::Validate() const
{
   if ( m_scalingFunction == nullptr )
      throw Error( "Invalid wavelet transform: scaling function not initialized." );
}

//-----------------------------------------------------------------------------
// Transform (decomposition)
//-----------------------------------------------------------------------------

void PyramidalWaveletTransform::DoTransform( StatusMonitor& status )
{
   bool statusInitialized = false;

   try
   {
      if ( status.IsInitializationEnabled() )
      {
         status.Initialize( "Pyramidal wavelet transform",
                            m_transform.NumberOfChannels()
                          * (m_nonstandard ? 4*m_transform.Width() - 4 : m_transform.Width()+m_transform.Height()) );
         status.DisableInitialization();
         statusInitialized = true;
      }

      for ( int c = 0; c <= m_transform.NumberOfChannels(); ++c )
         if ( m_nonstandard )
            NonstandardWaveletTransform2D( m_transform[c],
                                           m_transform.NumberOfPixels(),
                                           *m_scalingFunction, false, &status );
         else
            StandardWaveletTransform2D( m_transform[c],
                                        m_transform.Width(),
                                        m_transform.Height(),
                                        *m_scalingFunction, false, &status );

      if ( statusInitialized )
         status.EnableInitialization();
   }
   catch ( ... )
   {
      if ( statusInitialized )
         status.EnableInitialization();
      Clear();
      throw;
   }
}

#define IMPLEMENT_TRANSFORM                        \
   Validate();                                     \
   if ( image.IsEmptySelection() )                 \
   {                                               \
      Clear();                                     \
      return;                                      \
   }                                               \
   m_transform = image;                            \
   DoTransform( (StatusMonitor&)image.Status() );


void PyramidalWaveletTransform::Transform( const pcl::Image& image )
{
   IMPLEMENT_TRANSFORM
}

void PyramidalWaveletTransform::Transform( const pcl::DImage& image )
{
   IMPLEMENT_TRANSFORM
}

void PyramidalWaveletTransform::Transform( const pcl::ComplexImage& image )
{
   IMPLEMENT_TRANSFORM
}

void PyramidalWaveletTransform::Transform( const pcl::DComplexImage& image )
{
   IMPLEMENT_TRANSFORM
}

void PyramidalWaveletTransform::Transform( const pcl::UInt8Image& image )
{
   IMPLEMENT_TRANSFORM
}

void PyramidalWaveletTransform::Transform( const pcl::UInt16Image& image )
{
   IMPLEMENT_TRANSFORM
}

void PyramidalWaveletTransform::Transform( const pcl::UInt32Image& image )
{
   IMPLEMENT_TRANSFORM
}

#undef IMPLEMENT_TRANSFORM

//-----------------------------------------------------------------------------
// Inverse transform (reconstruction)
//-----------------------------------------------------------------------------

void PyramidalWaveletTransform::Apply( Image& image ) const
{
   Validate();

   if ( !m_transform )
   {
      image.FreeData();
      return;
   }

   bool statusInitialized = false;
   StatusMonitor sm = image.Status();

   try
   {
      image = m_transform;
      image.Status() = sm;

      if ( image.Status().IsInitializationEnabled() )
      {
         image.Status().Initialize( "Inverse pyramidal wavelet transform",
                                  image.NumberOfChannels()*(m_nonstandard ? 4*image.Width() - 4 : image.Width()+image.Height()) );
         image.Status().DisableInitialization();
         statusInitialized = true;
      }

      for ( int c = 0; c < image.NumberOfChannels(); ++c )
         if ( m_nonstandard )
            NonstandardWaveletTransform2D( image[c], image.NumberOfPixels(), *m_scalingFunction, true, &image.Status() );
         else
            StandardWaveletTransform2D( image[c], image.Width(), image.Height(), *m_scalingFunction, true, &image.Status() );

      if ( statusInitialized )
         image.Status().EnableInitialization();
   }
   catch ( ... )
   {
      if ( statusInitialized )
         image.Status().EnableInitialization();
      throw;
   }
}

#define IMPLEMENT_APPLY \
   pcl::Image F;        \
   Apply( F );          \
   image = F;

void PyramidalWaveletTransform::Apply( DImage& image ) const
{
   IMPLEMENT_APPLY
}

void PyramidalWaveletTransform::Apply( pcl::ComplexImage& image ) const
{
   IMPLEMENT_APPLY
}

void PyramidalWaveletTransform::Apply( pcl::DComplexImage& image ) const
{
   IMPLEMENT_APPLY
}

void PyramidalWaveletTransform::Apply( pcl::UInt8Image& image ) const
{
   IMPLEMENT_APPLY
}

void PyramidalWaveletTransform::Apply( pcl::UInt16Image& image ) const
{
   IMPLEMENT_APPLY
}

void PyramidalWaveletTransform::Apply( pcl::UInt32Image& image ) const
{
   IMPLEMENT_APPLY
}

#undef IMPLEMENT_APPLY

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PyramidalWaveletTransform.cpp - Released 2017-07-09T18:07:16Z
