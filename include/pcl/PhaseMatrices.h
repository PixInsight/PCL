//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/PhaseMatrices.h - Released 2015/12/17 18:52:09 UTC
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

#ifndef __PCL_PhaseMatrices_h
#define __PCL_PhaseMatrices_h

/// \file pcl/PhaseMatrices.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Image_h
#include <pcl/Image.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * Calculation of the phase correlation matrix (PCM).
 *
 * \param[out] R  The resulting PCM as a single-channel complex image with the
 *                same dimensions of the source images \a A and \a B.
 *
 * \param      A  A single-channel complex image with the discrete Fourier
 *                transform of the reference image.
 *
 * \param      B  A single-channel complex image with the discrete Fourier
 *                transform of the working image.
 *
 * Both source images \a A and \a B must be nonempty and have the same
 * dimensions. If the result image \a R does not have the same geometry as the
 * source images, it is reallocated as necessary.
 *
 * \note Increments the status monitoring object of the result image \a R by
 * its area in square pixels.
 *
 * \ingroup phase_matrices
 */
template <class P1, class P2> inline
void PCL_FUNC PhaseCorrelationMatrix( GenericImage<P1>& R, const GenericImage<P2>& A, const GenericImage<P2>& B )
{
   const typename GenericImage<P2>::sample::component tiny( 1.0e-20 );

   size_type N = A.NumberOfPixels();

   if ( A.IsEmpty() || B.NumberOfPixels() != N )
      throw Error( "Invalid image geometry in PhaseCorrelationMatrix()" );

   if ( R.NumberOfPixels() != N )
      R.AllocateData( A.Width(), B.Width() );

   if ( R.Status().IsInitializationEnabled() )
      R.Status().Initialize( "Phase correlation matrix", N );

   const typename GenericImage<P2>::sample* a  = *A;
   const typename GenericImage<P2>::sample* b  = *B;
         typename GenericImage<P1>::sample* r  = *R;
         typename GenericImage<P1>::sample* rN = r + N;

   for ( ; r < rN; ++r, ++a, ++b, ++R.Status() )
   {
      typename GenericImage<P2>::sample num = *a * ~*b;
      *r = P1::ToSample( num/Max( tiny, Abs( num ) ) );
   }
}

/*!
 * Calculation of the cross power spectrum matrix (CPSM).
 *
 * \param[out] R  The resulting CPSM as a single-channel complex image with the
 *                same dimensions of the source images \a A and \a B.
 *
 * \param      A  A single-channel complex image with the discrete Fourier
 *                transform of the reference image.
 *
 * \param      B  A single-channel complex image with the discrete Fourier
 *                transform of the working image.
 *
 * Both source images \a A and \a B must be nonempty and have the same
 * dimensions. If the result image \a R does not have the same geometry as the
 * source images, it is reallocated as necessary.
 *
 * \note Increments the status monitoring object of the result image \a R by
 * its area in square pixels.
 *
 * \ingroup phase_matrices
 */
template <class P1, class P2> inline
void PCL_FUNC CrossPowerSpectrumMatrix( GenericImage<P1>& R, const GenericImage<P2>& A, const GenericImage<P2>& B )
{
   const typename GenericImage<P2>::sample::component tiny( 1.0e-20 );

   size_type N = A.NumberOfPixels();

   if ( A.IsEmpty() || B.NumberOfPixels() != N )
      throw Error( "Invalid image geometry in CrossPowerSpectrumMatrix()" );

   if ( R.NumberOfPixels() != N )
      R.AllocateData( A.Width(), B.Width() );

   if ( R.Status().IsInitializationEnabled() )
      R.Status().Initialize( "Cross power spectrum matrix", N );

   const typename GenericImage<P2>::sample* a  = *A;
   const typename GenericImage<P2>::sample* b  = *B;
         typename GenericImage<P1>::sample* r  = *R;
         typename GenericImage<P1>::sample* rN = r + N;

   for ( ; r < rN; ++r, ++a, ++b, ++R.Status() )
      *r = P1::ToSample( (*b * ~*a)/Max( tiny, Abs( *a ) * Abs( *b ) ) );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PhaseMatrices_h

// ----------------------------------------------------------------------------
// EOF pcl/PhaseMatrices.h - Released 2015/12/17 18:52:09 UTC
