//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/Median.cpp - Released 2018-11-23T16:14:32Z
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

#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define MEAN( a, b ) \
   (double( a ) + double( b ))/2

#define CMPXCHG( a, b )  \
   if ( i[b] < i[a] ) Swap( i[a], i[b] )

template <typename T>
static double MedianImplementation( T* i, T* j )
{
   distance_type n = j - i;
   if ( n < 1 )
      return 0;

   switch ( n )
   {
   case  1:
      return i[0];
   case  2:
      return MEAN( i[0], i[1] );
   case  3:
      CMPXCHG( 0, 1 ); CMPXCHG( 1, 2 );
      return pcl::Max( i[0], i[1] );
   case  4:
      CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 ); CMPXCHG( 0, 2 );
      CMPXCHG( 1, 3 );
      return MEAN( i[1], i[2] );
   case  5:
      CMPXCHG( 0, 1 ); CMPXCHG( 3, 4 ); CMPXCHG( 0, 3 );
      CMPXCHG( 1, 4 ); CMPXCHG( 1, 2 ); CMPXCHG( 2, 3 );
      return pcl::Max( i[1], i[2] );
   case  6:
      CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 ); CMPXCHG( 0, 2 );
      CMPXCHG( 1, 3 ); CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 );
      CMPXCHG( 0, 4 ); CMPXCHG( 1, 5 ); CMPXCHG( 1, 4 );
      CMPXCHG( 2, 4 ); CMPXCHG( 3, 5 ); CMPXCHG( 3, 4 );
      return MEAN( i[2], i[3] );
   case  7:
      CMPXCHG( 0, 5 ); CMPXCHG( 0, 3 ); CMPXCHG( 1, 6 );
      CMPXCHG( 2, 4 ); CMPXCHG( 0, 1 ); CMPXCHG( 3, 5 );
      CMPXCHG( 2, 6 ); CMPXCHG( 2, 3 ); CMPXCHG( 3, 6 );
      CMPXCHG( 4, 5 ); CMPXCHG( 1, 4 ); CMPXCHG( 1, 3 );
      return pcl::Min( i[3], i[4] );
   case  8:
      CMPXCHG( 0, 4 ); CMPXCHG( 1, 5 ); CMPXCHG( 2, 6 );
      CMPXCHG( 3, 7 ); CMPXCHG( 0, 2 ); CMPXCHG( 1, 3 );
      CMPXCHG( 4, 6 ); CMPXCHG( 5, 7 ); CMPXCHG( 2, 4 );
      CMPXCHG( 3, 5 ); CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 );
      CMPXCHG( 4, 5 ); CMPXCHG( 6, 7 ); CMPXCHG( 1, 4 );
      CMPXCHG( 3, 6 );
      return MEAN( i[3], i[4] );
   case  9:
      CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 ); CMPXCHG( 7, 8 );
      CMPXCHG( 0, 1 ); CMPXCHG( 3, 4 ); CMPXCHG( 6, 7 );
      CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 ); CMPXCHG( 7, 8 );
      CMPXCHG( 0, 3 ); CMPXCHG( 5, 8 ); CMPXCHG( 4, 7 );
      CMPXCHG( 3, 6 ); CMPXCHG( 1, 4 ); CMPXCHG( 2, 5 );
      CMPXCHG( 4, 7 ); CMPXCHG( 4, 2 ); CMPXCHG( 6, 4 );
      return pcl::Min( i[2], i[4] );
   case 10:
      CMPXCHG( 4, 9 ); CMPXCHG( 3, 8 ); CMPXCHG( 2, 7 );
      CMPXCHG( 1, 6 ); CMPXCHG( 0, 5 ); CMPXCHG( 1, 4 );
      CMPXCHG( 6, 9 ); CMPXCHG( 0, 3 ); CMPXCHG( 5, 8 );
      CMPXCHG( 0, 2 ); CMPXCHG( 3, 6 ); CMPXCHG( 7, 9 );
      CMPXCHG( 0, 1 ); CMPXCHG( 2, 4 ); CMPXCHG( 5, 7 );
      CMPXCHG( 8, 9 ); CMPXCHG( 1, 2 ); CMPXCHG( 4, 6 );
      CMPXCHG( 7, 8 ); CMPXCHG( 3, 5 ); CMPXCHG( 2, 5 );
      CMPXCHG( 6, 8 ); CMPXCHG( 1, 3 ); CMPXCHG( 4, 7 );
      CMPXCHG( 2, 3 ); CMPXCHG( 6, 7 ); CMPXCHG( 3, 4 );
      CMPXCHG( 5, 6 );
      return MEAN( i[4], i[5] );
   case 11:
      CMPXCHG( 0,  1 ); CMPXCHG( 2,  3 ); CMPXCHG( 4,  5 );
      CMPXCHG( 6,  7 ); CMPXCHG( 8,  9 ); CMPXCHG( 1,  3 );
      CMPXCHG( 5,  7 ); CMPXCHG( 0,  2 ); CMPXCHG( 4,  6 );
      CMPXCHG( 8, 10 ); CMPXCHG( 1,  2 ); CMPXCHG( 5,  6 );
      CMPXCHG( 9, 10 ); CMPXCHG( 1,  5 ); CMPXCHG( 6, 10 );
      CMPXCHG( 5,  9 ); CMPXCHG( 2,  6 ); CMPXCHG( 1,  5 );
      CMPXCHG( 6, 10 ); CMPXCHG( 0,  4 ); CMPXCHG( 3,  7 );
      CMPXCHG( 4,  8 ); CMPXCHG( 0,  4 ); CMPXCHG( 1,  4 );
      CMPXCHG( 7, 10 ); CMPXCHG( 3,  8 ); CMPXCHG( 2,  3 );
      CMPXCHG( 8,  9 ); CMPXCHG( 3,  5 ); CMPXCHG( 6,  8 );
      return pcl::Min( i[5], i[6] );
   case 12:
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG(  1,  3 ); CMPXCHG(  5,  7 ); CMPXCHG(  9, 11 );
      CMPXCHG(  0,  2 ); CMPXCHG(  4,  6 ); CMPXCHG(  8, 10 );
      CMPXCHG(  1,  2 ); CMPXCHG(  5,  6 ); CMPXCHG(  9, 10 );
      CMPXCHG(  1,  5 ); CMPXCHG(  6, 10 ); CMPXCHG(  5,  9 );
      CMPXCHG(  2,  6 ); CMPXCHG(  1,  5 ); CMPXCHG(  6, 10 );
      CMPXCHG(  0,  4 ); CMPXCHG(  7, 11 ); CMPXCHG(  3,  7 );
      CMPXCHG(  4,  8 ); CMPXCHG(  0,  4 ); CMPXCHG(  7, 11 );
      CMPXCHG(  1,  4 ); CMPXCHG(  7, 10 ); CMPXCHG(  3,  8 );
      CMPXCHG(  2,  3 ); CMPXCHG(  8,  9 ); CMPXCHG(  3,  5 );
      CMPXCHG(  6,  8 );
      return MEAN( i[5], i[6] );
   case 13:
      CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 );
      CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 ); CMPXCHG(  0,  4 );
      CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 );
      CMPXCHG(  8, 12 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG(  0,  2 );
      CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 );
      CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 ); CMPXCHG(  2,  8 );
      CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 ); CMPXCHG(  2,  4 );
      CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 );
      CMPXCHG( 10, 12 ); CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 );
      CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 );
      CMPXCHG( 10, 11 ); CMPXCHG(  1,  8 ); CMPXCHG(  3, 10 );
      CMPXCHG(  5, 12 ); CMPXCHG(  3,  6 ); CMPXCHG(  5,  8 );
      return pcl::Max( i[5], i[6] );
   case 14:
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG(  0,  2 ); CMPXCHG(  4,  6 );
      CMPXCHG(  8, 10 ); CMPXCHG(  1,  3 ); CMPXCHG(  5,  7 );
      CMPXCHG(  9, 11 ); CMPXCHG(  0,  4 ); CMPXCHG(  8, 12 );
      CMPXCHG(  1,  5 ); CMPXCHG(  9, 13 ); CMPXCHG(  2,  6 );
      CMPXCHG(  3,  7 ); CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 );
      CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 );
      CMPXCHG(  5, 13 ); CMPXCHG(  5, 10 ); CMPXCHG(  6,  9 );
      CMPXCHG(  3, 12 ); CMPXCHG(  7, 11 ); CMPXCHG(  1,  2 );
      CMPXCHG(  4,  8 ); CMPXCHG(  7, 13 ); CMPXCHG(  2,  8 );
      CMPXCHG(  5,  6 ); CMPXCHG(  9, 10 ); CMPXCHG(  3,  8 );
      CMPXCHG(  7, 12 ); CMPXCHG(  6,  8 ); CMPXCHG(  3,  5 );
      CMPXCHG(  7,  9 ); CMPXCHG(  5,  6 ); CMPXCHG(  7,  8 );
      return MEAN( i[6], i[7] );
   case 15:
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG(  0,  2 ); CMPXCHG(  4,  6 );
      CMPXCHG(  8, 10 ); CMPXCHG( 12, 14 ); CMPXCHG(  1,  3 );
      CMPXCHG(  5,  7 ); CMPXCHG(  9, 11 ); CMPXCHG(  0,  4 );
      CMPXCHG(  8, 12 ); CMPXCHG(  1,  5 ); CMPXCHG(  9, 13 );
      CMPXCHG(  2,  6 ); CMPXCHG( 10, 14 ); CMPXCHG(  3,  7 );
      CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 );
      CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 );
      CMPXCHG(  6, 14 ); CMPXCHG(  5, 10 ); CMPXCHG(  6,  9 );
      CMPXCHG(  3, 12 ); CMPXCHG( 13, 14 ); CMPXCHG(  7, 11 );
      CMPXCHG(  1,  2 ); CMPXCHG(  4,  8 ); CMPXCHG(  7, 13 );
      CMPXCHG(  2,  8 ); CMPXCHG(  5,  6 ); CMPXCHG(  9, 10 );
      CMPXCHG(  3,  8 ); CMPXCHG(  7, 12 ); CMPXCHG(  6,  8 );
      CMPXCHG(  3,  5 ); CMPXCHG(  7,  9 ); CMPXCHG(  5,  6 );
      CMPXCHG(  7,  8 );
      return pcl::Max( i[6], i[7] );
   case 16:
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 ); CMPXCHG(  0,  2 );
      CMPXCHG(  4,  6 ); CMPXCHG(  8, 10 ); CMPXCHG( 12, 14 );
      CMPXCHG(  1,  3 ); CMPXCHG(  5,  7 ); CMPXCHG(  9, 11 );
      CMPXCHG( 13, 15 ); CMPXCHG(  0,  4 ); CMPXCHG(  8, 12 );
      CMPXCHG(  1,  5 ); CMPXCHG(  9, 13 ); CMPXCHG(  2,  6 );
      CMPXCHG( 10, 14 ); CMPXCHG(  3,  7 ); CMPXCHG( 11, 15 );
      CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 );
      CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 );
      CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 ); CMPXCHG(  5, 10 );
      CMPXCHG(  6,  9 ); CMPXCHG(  3, 12 ); CMPXCHG( 13, 14 );
      CMPXCHG(  7, 11 ); CMPXCHG(  1,  2 ); CMPXCHG(  4,  8 );
      CMPXCHG(  7, 13 ); CMPXCHG(  2,  8 ); CMPXCHG(  5,  6 );
      CMPXCHG(  9, 10 ); CMPXCHG(  3,  8 ); CMPXCHG(  7, 12 );
      CMPXCHG(  6,  8 ); CMPXCHG( 10, 12 ); CMPXCHG(  3,  5 );
      CMPXCHG(  7,  9 ); CMPXCHG(  5,  6 ); CMPXCHG(  7,  8 );
      CMPXCHG(  9, 10 ); CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 );
      return MEAN( i[7], i[8] );
   case 17:
      CMPXCHG(  0, 16 ); CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 );
      CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 );
      CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 );
      CMPXCHG(  8, 16 ); CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 );
      CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 );
      CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 );
      CMPXCHG(  4, 16 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 );
      CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 );
      CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 ); CMPXCHG(  2, 16 );
      CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 );
      CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 ); CMPXCHG(  2,  4 );
      CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 );
      CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 );
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 ); CMPXCHG(  1, 16 );
      CMPXCHG(  1,  8 ); CMPXCHG(  3, 10 ); CMPXCHG(  5, 12 );
      CMPXCHG(  7, 14 ); CMPXCHG(  5,  8 ); CMPXCHG(  7, 10 );
      return pcl::Max( i[7], i[8] );
   case 18:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  0,  8 );
      CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 );
      CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 );
      CMPXCHG(  7, 15 ); CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 );
      CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 );
      CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 );
      CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 ); CMPXCHG(  4, 16 );
      CMPXCHG(  5, 17 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 );
      CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 );
      CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 );
      CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 ); CMPXCHG(  2,  8 );
      CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 );
      CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 ); CMPXCHG(  2,  4 );
      CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 );
      CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 );
      CMPXCHG( 15, 17 ); CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 );
      CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 );
      CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 );
      CMPXCHG( 16, 17 ); CMPXCHG(  1, 16 ); CMPXCHG(  1,  8 );
      CMPXCHG(  3, 10 ); CMPXCHG(  5, 12 ); CMPXCHG(  7, 14 );
      CMPXCHG(  9, 16 ); CMPXCHG(  5,  8 ); CMPXCHG(  7, 10 );
      CMPXCHG(  9, 12 ); CMPXCHG(  7,  8 ); CMPXCHG(  9, 10 );
      return MEAN( i[8], i[9] );
   case 19:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 );
      CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 );
      CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 ); CMPXCHG(  8, 16 );
      CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 ); CMPXCHG(  0,  4 );
      CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 );
      CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 );
      CMPXCHG( 11, 15 ); CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 );
      CMPXCHG(  6, 18 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG(  0,  2 );
      CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 );
      CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 );
      CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 ); CMPXCHG(  2, 16 );
      CMPXCHG(  3, 17 ); CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 );
      CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 );
      CMPXCHG( 11, 17 ); CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 );
      CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 );
      CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 );
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 );
      CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 ); CMPXCHG(  3, 10 );
      CMPXCHG(  5, 12 ); CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 );
      CMPXCHG(  7, 10 ); CMPXCHG(  9, 12 );
      return pcl::Min( i[9], i[10] );
   case 20:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 );
      CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 );
      CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 );
      CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 );
      CMPXCHG( 11, 19 ); CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 );
      CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 );
      CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 );
      CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 );
      CMPXCHG(  7, 19 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 );
      CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 );
      CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 );
      CMPXCHG( 17, 19 ); CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 );
      CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 );
      CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 );
      CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 );
      CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 );
      CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 ); CMPXCHG(  0,  1 );
      CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 );
      CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 );
      CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 );
      CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 ); CMPXCHG(  3, 10 );
      CMPXCHG(  5, 12 ); CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 );
      CMPXCHG(  7, 10 ); CMPXCHG(  9, 12 );
      return MEAN( i[9], i[10] );
   case 21:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  0,  8 );
      CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 );
      CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 );
      CMPXCHG(  7, 15 ); CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 );
      CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 ); CMPXCHG( 12, 20 );
      CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 );
      CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 );
      CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 ); CMPXCHG( 16, 20 );
      CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 );
      CMPXCHG(  7, 19 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 );
      CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 );
      CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 );
      CMPXCHG( 17, 19 ); CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 );
      CMPXCHG(  6, 20 ); CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 );
      CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 );
      CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 ); CMPXCHG(  2,  4 );
      CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 );
      CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 );
      CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 ); CMPXCHG(  0,  1 );
      CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 );
      CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 );
      CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 );
      CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 ); CMPXCHG(  5, 20 );
      CMPXCHG(  3, 10 ); CMPXCHG(  5, 12 ); CMPXCHG(  7, 14 );
      CMPXCHG(  9, 16 ); CMPXCHG(  7, 10 ); CMPXCHG(  9, 12 );
      return pcl::Max( i[9], i[10] );
   case 22:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 );
      CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 );
      CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 ); CMPXCHG(  8, 16 );
      CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 );
      CMPXCHG( 12, 20 ); CMPXCHG( 13, 21 ); CMPXCHG(  0,  4 );
      CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 );
      CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 );
      CMPXCHG( 11, 15 ); CMPXCHG( 16, 20 ); CMPXCHG( 17, 21 );
      CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 );
      CMPXCHG(  7, 19 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 );
      CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 );
      CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 );
      CMPXCHG( 17, 19 ); CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 );
      CMPXCHG(  6, 20 ); CMPXCHG(  7, 21 ); CMPXCHG(  2,  8 );
      CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 );
      CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 );
      CMPXCHG( 15, 21 ); CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 );
      CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 );
      CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 );
      CMPXCHG( 18, 20 ); CMPXCHG( 19, 21 ); CMPXCHG(  0,  1 );
      CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 );
      CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 );
      CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 );
      CMPXCHG( 20, 21 ); CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 );
      CMPXCHG(  5, 20 ); CMPXCHG(  3, 10 ); CMPXCHG(  5, 12 );
      CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 ); CMPXCHG( 11, 18 );
      CMPXCHG(  7, 10 ); CMPXCHG(  9, 12 ); CMPXCHG( 11, 14 );
      CMPXCHG(  9, 10 ); CMPXCHG( 11, 12 );
      return MEAN( i[10], i[11] );
   case 23:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 );
      CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 );
      CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 );
      CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 );
      CMPXCHG( 11, 19 ); CMPXCHG( 12, 20 ); CMPXCHG( 13, 21 );
      CMPXCHG( 14, 22 ); CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 );
      CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 );
      CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 );
      CMPXCHG( 16, 20 ); CMPXCHG( 17, 21 ); CMPXCHG( 18, 22 );
      CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 );
      CMPXCHG(  7, 19 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 );
      CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 );
      CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 );
      CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 ); CMPXCHG(  2, 16 );
      CMPXCHG(  3, 17 ); CMPXCHG(  6, 20 ); CMPXCHG(  7, 21 );
      CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 );
      CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 );
      CMPXCHG( 14, 20 ); CMPXCHG( 15, 21 ); CMPXCHG(  2,  4 );
      CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 );
      CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 );
      CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 ); CMPXCHG( 19, 21 );
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 );
      CMPXCHG( 18, 19 ); CMPXCHG( 20, 21 ); CMPXCHG(  1, 16 );
      CMPXCHG(  3, 18 ); CMPXCHG(  5, 20 ); CMPXCHG(  7, 22 );
      CMPXCHG(  5, 12 ); CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 );
      CMPXCHG( 11, 18 ); CMPXCHG(  9, 12 ); CMPXCHG( 11, 14 );
      return pcl::Min( i[11], i[12] );
   case 24:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  0,  8 );
      CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 );
      CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 );
      CMPXCHG(  7, 15 ); CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 );
      CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 ); CMPXCHG( 12, 20 );
      CMPXCHG( 13, 21 ); CMPXCHG( 14, 22 ); CMPXCHG( 15, 23 );
      CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 );
      CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 );
      CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 ); CMPXCHG( 16, 20 );
      CMPXCHG( 17, 21 ); CMPXCHG( 18, 22 ); CMPXCHG( 19, 23 );
      CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 );
      CMPXCHG(  7, 19 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 );
      CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 );
      CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 );
      CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 ); CMPXCHG( 21, 23 );
      CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 ); CMPXCHG(  6, 20 );
      CMPXCHG(  7, 21 ); CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 );
      CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 );
      CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 ); CMPXCHG( 15, 21 );
      CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 );
      CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 );
      CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 );
      CMPXCHG( 19, 21 ); CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 );
      CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 );
      CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 );
      CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 ); CMPXCHG( 20, 21 );
      CMPXCHG( 22, 23 ); CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 );
      CMPXCHG(  5, 20 ); CMPXCHG(  7, 22 ); CMPXCHG(  5, 12 );
      CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 ); CMPXCHG( 11, 18 );
      CMPXCHG(  9, 12 ); CMPXCHG( 11, 14 );
      return MEAN( i[11], i[12] );
   case 25:
      CMPXCHG(  0,  1 ); CMPXCHG(  3,  4 ); CMPXCHG(  2,  4 );
      CMPXCHG(  2,  3 ); CMPXCHG(  6,  7 ); CMPXCHG(  5,  7 );
      CMPXCHG(  5,  6 ); CMPXCHG(  9, 10 ); CMPXCHG(  8, 10 );
      CMPXCHG(  8,  9 ); CMPXCHG( 12, 13 ); CMPXCHG( 11, 13 );
      CMPXCHG( 11, 12 ); CMPXCHG( 15, 16 ); CMPXCHG( 14, 16 );
      CMPXCHG( 14, 15 ); CMPXCHG( 18, 19 ); CMPXCHG( 17, 19 );
      CMPXCHG( 17, 18 ); CMPXCHG( 21, 22 ); CMPXCHG( 20, 22 );
      CMPXCHG( 20, 21 ); CMPXCHG( 23, 24 ); CMPXCHG(  2,  5 );
      CMPXCHG(  3,  6 ); CMPXCHG(  0,  6 ); CMPXCHG(  0,  3 );
      CMPXCHG(  4,  7 ); CMPXCHG(  1,  7 ); CMPXCHG(  1,  4 );
      CMPXCHG( 11, 14 ); CMPXCHG(  8, 14 ); CMPXCHG(  8, 11 );
      CMPXCHG( 12, 15 ); CMPXCHG(  9, 15 ); CMPXCHG(  9, 12 );
      CMPXCHG( 13, 16 ); CMPXCHG( 10, 16 ); CMPXCHG( 10, 13 );
      CMPXCHG( 20, 23 ); CMPXCHG( 17, 23 ); CMPXCHG( 17, 20 );
      CMPXCHG( 21, 24 ); CMPXCHG( 18, 24 ); CMPXCHG( 18, 21 );
      CMPXCHG( 19, 22 ); CMPXCHG(  8, 17 ); CMPXCHG(  9, 18 );
      CMPXCHG(  0, 18 ); CMPXCHG(  0,  9 ); CMPXCHG( 10, 19 );
      CMPXCHG(  1, 19 ); CMPXCHG(  1, 10 ); CMPXCHG( 11, 20 );
      CMPXCHG(  2, 20 ); CMPXCHG(  2, 11 ); CMPXCHG( 12, 21 );
      CMPXCHG(  3, 21 ); CMPXCHG(  3, 12 ); CMPXCHG( 13, 22 );
      CMPXCHG(  4, 22 ); CMPXCHG(  4, 13 ); CMPXCHG( 14, 23 );
      CMPXCHG(  5, 23 ); CMPXCHG(  5, 14 ); CMPXCHG( 15, 24 );
      CMPXCHG(  6, 24 ); CMPXCHG(  6, 15 ); CMPXCHG(  7, 16 );
      CMPXCHG(  7, 19 ); CMPXCHG( 13, 21 ); CMPXCHG( 15, 23 );
      CMPXCHG(  7, 13 ); CMPXCHG(  7, 15 ); CMPXCHG(  1,  9 );
      CMPXCHG(  3, 11 ); CMPXCHG(  5, 17 ); CMPXCHG( 11, 17 );
      CMPXCHG(  9, 17 ); CMPXCHG(  4, 10 ); CMPXCHG(  6, 12 );
      CMPXCHG(  7, 14 ); CMPXCHG(  4,  6 ); CMPXCHG(  4,  7 );
      CMPXCHG( 12, 14 ); CMPXCHG( 10, 14 ); CMPXCHG(  6,  7 );
      CMPXCHG( 10, 12 ); CMPXCHG(  6, 10 ); CMPXCHG(  6, 17 );
      CMPXCHG( 12, 17 ); CMPXCHG(  7, 17 ); CMPXCHG(  7, 10 );
      CMPXCHG( 12, 18 ); CMPXCHG(  7, 12 ); CMPXCHG( 10, 18 );
      CMPXCHG( 12, 20 ); CMPXCHG( 10, 20 );
      return pcl::Max( i[10], i[12] );
   case 26:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  8, 24 );
      CMPXCHG(  9, 25 ); CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 );
      CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 );
      CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 );
      CMPXCHG( 16, 24 ); CMPXCHG( 17, 25 ); CMPXCHG(  8, 16 );
      CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 );
      CMPXCHG( 12, 20 ); CMPXCHG( 13, 21 ); CMPXCHG( 14, 22 );
      CMPXCHG( 15, 23 ); CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 );
      CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 );
      CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 );
      CMPXCHG( 16, 20 ); CMPXCHG( 17, 21 ); CMPXCHG( 18, 22 );
      CMPXCHG( 19, 23 ); CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 );
      CMPXCHG(  6, 18 ); CMPXCHG(  7, 19 ); CMPXCHG( 12, 24 );
      CMPXCHG( 13, 25 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG( 20, 24 ); CMPXCHG( 21, 25 ); CMPXCHG(  0,  2 );
      CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 );
      CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 );
      CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 ); CMPXCHG( 17, 19 );
      CMPXCHG( 20, 22 ); CMPXCHG( 21, 23 ); CMPXCHG(  2, 16 );
      CMPXCHG(  3, 17 ); CMPXCHG(  6, 20 ); CMPXCHG(  7, 21 );
      CMPXCHG( 10, 24 ); CMPXCHG( 11, 25 ); CMPXCHG(  2,  8 );
      CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 );
      CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 );
      CMPXCHG( 15, 21 ); CMPXCHG( 18, 24 ); CMPXCHG( 19, 25 );
      CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 );
      CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 );
      CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 );
      CMPXCHG( 19, 21 ); CMPXCHG( 22, 24 ); CMPXCHG( 23, 25 );
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 );
      CMPXCHG( 18, 19 ); CMPXCHG( 20, 21 ); CMPXCHG( 22, 23 );
      CMPXCHG( 24, 25 ); CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 );
      CMPXCHG(  5, 20 ); CMPXCHG(  7, 22 ); CMPXCHG(  9, 24 );
      CMPXCHG(  5, 12 ); CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 );
      CMPXCHG( 11, 18 ); CMPXCHG( 13, 20 ); CMPXCHG(  9, 12 );
      CMPXCHG( 11, 14 ); CMPXCHG( 13, 16 ); CMPXCHG( 11, 12 );
      CMPXCHG( 13, 14 );
      return MEAN( i[12], i[13] );
   case 27:
      CMPXCHG( 0, 16 ); CMPXCHG( 1, 17 ); CMPXCHG( 2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  8, 24 );
      CMPXCHG(  9, 25 ); CMPXCHG( 10, 26 ); CMPXCHG(  0,  8 );
      CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 );
      CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 );
      CMPXCHG(  7, 15 ); CMPXCHG( 16, 24 ); CMPXCHG( 17, 25 );
      CMPXCHG( 18, 26 ); CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 );
      CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 ); CMPXCHG( 12, 20 );
      CMPXCHG( 13, 21 ); CMPXCHG( 14, 22 ); CMPXCHG( 15, 23 );
      CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 );
      CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 );
      CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 ); CMPXCHG( 16, 20 );
      CMPXCHG( 17, 21 ); CMPXCHG( 18, 22 ); CMPXCHG( 19, 23 );
      CMPXCHG(  4, 16 ); CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 );
      CMPXCHG(  7, 19 ); CMPXCHG( 12, 24 ); CMPXCHG( 13, 25 );
      CMPXCHG( 14, 26 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG( 20, 24 ); CMPXCHG( 21, 25 ); CMPXCHG( 22, 26 );
      CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 ); CMPXCHG(  4,  6 );
      CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 ); CMPXCHG(  9, 11 );
      CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 ); CMPXCHG( 16, 18 );
      CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 ); CMPXCHG( 21, 23 );
      CMPXCHG( 24, 26 ); CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 );
      CMPXCHG(  6, 20 ); CMPXCHG(  7, 21 ); CMPXCHG( 10, 24 );
      CMPXCHG( 11, 25 ); CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 );
      CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 );
      CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 ); CMPXCHG( 15, 21 );
      CMPXCHG( 18, 24 ); CMPXCHG( 19, 25 ); CMPXCHG(  2,  4 );
      CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 );
      CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 );
      CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 ); CMPXCHG( 19, 21 );
      CMPXCHG( 22, 24 ); CMPXCHG( 23, 25 ); CMPXCHG(  0,  1 );
      CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 );
      CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 );
      CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 );
      CMPXCHG( 20, 21 ); CMPXCHG( 22, 23 ); CMPXCHG( 24, 25 );
      CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 ); CMPXCHG(  5, 20 );
      CMPXCHG(  7, 22 ); CMPXCHG(  9, 24 ); CMPXCHG( 11, 26 );
      CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 ); CMPXCHG( 11, 18 );
      CMPXCHG( 13, 20 ); CMPXCHG( 11, 14 ); CMPXCHG( 13, 16 );
      return pcl::Min( i[13], i[14] );
   case 28:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  8, 24 );
      CMPXCHG(  9, 25 ); CMPXCHG( 10, 26 ); CMPXCHG( 11, 27 );
      CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 );
      CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 );
      CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 ); CMPXCHG( 16, 24 );
      CMPXCHG( 17, 25 ); CMPXCHG( 18, 26 ); CMPXCHG( 19, 27 );
      CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 );
      CMPXCHG( 11, 19 ); CMPXCHG( 12, 20 ); CMPXCHG( 13, 21 );
      CMPXCHG( 14, 22 ); CMPXCHG( 15, 23 ); CMPXCHG(  0,  4 );
      CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 );
      CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 );
      CMPXCHG( 11, 15 ); CMPXCHG( 16, 20 ); CMPXCHG( 17, 21 );
      CMPXCHG( 18, 22 ); CMPXCHG( 19, 23 ); CMPXCHG(  4, 16 );
      CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 ); CMPXCHG(  7, 19 );
      CMPXCHG( 12, 24 ); CMPXCHG( 13, 25 ); CMPXCHG( 14, 26 );
      CMPXCHG( 15, 27 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG( 20, 24 ); CMPXCHG( 21, 25 ); CMPXCHG( 22, 26 );
      CMPXCHG( 23, 27 ); CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 );
      CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 );
      CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 );
      CMPXCHG( 16, 18 ); CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 );
      CMPXCHG( 21, 23 ); CMPXCHG( 24, 26 ); CMPXCHG( 25, 27 );
      CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 ); CMPXCHG(  6, 20 );
      CMPXCHG(  7, 21 ); CMPXCHG( 10, 24 ); CMPXCHG( 11, 25 );
      CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 );
      CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 );
      CMPXCHG( 14, 20 ); CMPXCHG( 15, 21 ); CMPXCHG( 18, 24 );
      CMPXCHG( 19, 25 ); CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 );
      CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 );
      CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 );
      CMPXCHG( 18, 20 ); CMPXCHG( 19, 21 ); CMPXCHG( 22, 24 );
      CMPXCHG( 23, 25 ); CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 );
      CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 );
      CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 );
      CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 ); CMPXCHG( 20, 21 );
      CMPXCHG( 22, 23 ); CMPXCHG( 24, 25 ); CMPXCHG( 26, 27 );
      CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 ); CMPXCHG(  5, 20 );
      CMPXCHG(  7, 22 ); CMPXCHG(  9, 24 ); CMPXCHG( 11, 26 );
      CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 ); CMPXCHG( 11, 18 );
      CMPXCHG( 13, 20 ); CMPXCHG( 11, 14 ); CMPXCHG( 13, 16 );
      return MEAN( i[13], i[14] );
   case 29:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  8, 24 );
      CMPXCHG(  9, 25 ); CMPXCHG( 10, 26 ); CMPXCHG( 11, 27 );
      CMPXCHG( 12, 28 ); CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 );
      CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 );
      CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 );
      CMPXCHG( 16, 24 ); CMPXCHG( 17, 25 ); CMPXCHG( 18, 26 );
      CMPXCHG( 19, 27 ); CMPXCHG( 20, 28 ); CMPXCHG(  8, 16 );
      CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 );
      CMPXCHG( 12, 20 ); CMPXCHG( 13, 21 ); CMPXCHG( 14, 22 );
      CMPXCHG( 15, 23 ); CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 );
      CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 );
      CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 );
      CMPXCHG( 16, 20 ); CMPXCHG( 17, 21 ); CMPXCHG( 18, 22 );
      CMPXCHG( 19, 23 ); CMPXCHG( 24, 28 ); CMPXCHG(  4, 16 );
      CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 ); CMPXCHG(  7, 19 );
      CMPXCHG( 12, 24 ); CMPXCHG( 13, 25 ); CMPXCHG( 14, 26 );
      CMPXCHG( 15, 27 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG( 20, 24 ); CMPXCHG( 21, 25 ); CMPXCHG( 22, 26 );
      CMPXCHG( 23, 27 ); CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 );
      CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 );
      CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 );
      CMPXCHG( 16, 18 ); CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 );
      CMPXCHG( 21, 23 ); CMPXCHG( 24, 26 ); CMPXCHG( 25, 27 );
      CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 ); CMPXCHG(  6, 20 );
      CMPXCHG(  7, 21 ); CMPXCHG( 10, 24 ); CMPXCHG( 11, 25 );
      CMPXCHG( 14, 28 ); CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 );
      CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 );
      CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 ); CMPXCHG( 15, 21 );
      CMPXCHG( 18, 24 ); CMPXCHG( 19, 25 ); CMPXCHG( 22, 28 );
      CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 );
      CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 );
      CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 );
      CMPXCHG( 19, 21 ); CMPXCHG( 22, 24 ); CMPXCHG( 23, 25 );
      CMPXCHG( 26, 28 ); CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 );
      CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 );
      CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 );
      CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 ); CMPXCHG( 20, 21 );
      CMPXCHG( 22, 23 ); CMPXCHG( 24, 25 ); CMPXCHG( 26, 27 );
      CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 ); CMPXCHG(  5, 20 );
      CMPXCHG(  7, 22 ); CMPXCHG(  9, 24 ); CMPXCHG( 11, 26 );
      CMPXCHG( 13, 28 ); CMPXCHG(  7, 14 ); CMPXCHG(  9, 16 );
      CMPXCHG( 11, 18 ); CMPXCHG( 13, 20 ); CMPXCHG( 11, 14 );
      CMPXCHG( 13, 16 );
      return pcl::Max( i[13], i[14] );
   case 30:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  8, 24 );
      CMPXCHG(  9, 25 ); CMPXCHG( 10, 26 ); CMPXCHG( 11, 27 );
      CMPXCHG( 12, 28 ); CMPXCHG( 13, 29 ); CMPXCHG(  0,  8 );
      CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 );
      CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 );
      CMPXCHG(  7, 15 ); CMPXCHG( 16, 24 ); CMPXCHG( 17, 25 );
      CMPXCHG( 18, 26 ); CMPXCHG( 19, 27 ); CMPXCHG( 20, 28 );
      CMPXCHG( 21, 29 ); CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 );
      CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 ); CMPXCHG( 12, 20 );
      CMPXCHG( 13, 21 ); CMPXCHG( 14, 22 ); CMPXCHG( 15, 23 );
      CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 );
      CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 );
      CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 ); CMPXCHG( 16, 20 );
      CMPXCHG( 17, 21 ); CMPXCHG( 18, 22 ); CMPXCHG( 19, 23 );
      CMPXCHG( 24, 28 ); CMPXCHG( 25, 29 ); CMPXCHG(  4, 16 );
      CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 ); CMPXCHG(  7, 19 );
      CMPXCHG( 12, 24 ); CMPXCHG( 13, 25 ); CMPXCHG( 14, 26 );
      CMPXCHG( 15, 27 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG( 20, 24 ); CMPXCHG( 21, 25 ); CMPXCHG( 22, 26 );
      CMPXCHG( 23, 27 ); CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 );
      CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 );
      CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 );
      CMPXCHG( 16, 18 ); CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 );
      CMPXCHG( 21, 23 ); CMPXCHG( 24, 26 ); CMPXCHG( 25, 27 );
      CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 ); CMPXCHG(  6, 20 );
      CMPXCHG(  7, 21 ); CMPXCHG( 10, 24 ); CMPXCHG( 11, 25 );
      CMPXCHG( 14, 28 ); CMPXCHG( 15, 29 ); CMPXCHG(  2,  8 );
      CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 );
      CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 );
      CMPXCHG( 15, 21 ); CMPXCHG( 18, 24 ); CMPXCHG( 19, 25 );
      CMPXCHG( 22, 28 ); CMPXCHG( 23, 29 ); CMPXCHG(  2,  4 );
      CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 );
      CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 );
      CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 ); CMPXCHG( 19, 21 );
      CMPXCHG( 22, 24 ); CMPXCHG( 23, 25 ); CMPXCHG( 26, 28 );
      CMPXCHG( 27, 29 ); CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 );
      CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 );
      CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 );
      CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 ); CMPXCHG( 20, 21 );
      CMPXCHG( 22, 23 ); CMPXCHG( 24, 25 ); CMPXCHG( 26, 27 );
      CMPXCHG( 28, 29 ); CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 );
      CMPXCHG(  5, 20 ); CMPXCHG(  7, 22 ); CMPXCHG(  9, 24 );
      CMPXCHG( 11, 26 ); CMPXCHG( 13, 28 ); CMPXCHG(  7, 14 );
      CMPXCHG(  9, 16 ); CMPXCHG( 11, 18 ); CMPXCHG( 13, 20 );
      CMPXCHG( 15, 22 ); CMPXCHG( 11, 14 ); CMPXCHG( 13, 16 );
      CMPXCHG( 15, 18 ); CMPXCHG( 13, 14 ); CMPXCHG( 15, 16 );
      return MEAN( i[14], i[15] );
   case 31:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  8, 24 );
      CMPXCHG(  9, 25 ); CMPXCHG( 10, 26 ); CMPXCHG( 11, 27 );
      CMPXCHG( 12, 28 ); CMPXCHG( 13, 29 ); CMPXCHG( 14, 30 );
      CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 ); CMPXCHG(  2, 10 );
      CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 ); CMPXCHG(  5, 13 );
      CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 ); CMPXCHG( 16, 24 );
      CMPXCHG( 17, 25 ); CMPXCHG( 18, 26 ); CMPXCHG( 19, 27 );
      CMPXCHG( 20, 28 ); CMPXCHG( 21, 29 ); CMPXCHG( 22, 30 );
      CMPXCHG(  8, 16 ); CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 );
      CMPXCHG( 11, 19 ); CMPXCHG( 12, 20 ); CMPXCHG( 13, 21 );
      CMPXCHG( 14, 22 ); CMPXCHG( 15, 23 ); CMPXCHG(  0,  4 );
      CMPXCHG(  1,  5 ); CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 );
      CMPXCHG(  8, 12 ); CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 );
      CMPXCHG( 11, 15 ); CMPXCHG( 16, 20 ); CMPXCHG( 17, 21 );
      CMPXCHG( 18, 22 ); CMPXCHG( 19, 23 ); CMPXCHG( 24, 28 );
      CMPXCHG( 25, 29 ); CMPXCHG( 26, 30 ); CMPXCHG(  4, 16 );
      CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 ); CMPXCHG(  7, 19 );
      CMPXCHG( 12, 24 ); CMPXCHG( 13, 25 ); CMPXCHG( 14, 26 );
      CMPXCHG( 15, 27 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG( 20, 24 ); CMPXCHG( 21, 25 ); CMPXCHG( 22, 26 );
      CMPXCHG( 23, 27 ); CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 );
      CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 );
      CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 );
      CMPXCHG( 16, 18 ); CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 );
      CMPXCHG( 21, 23 ); CMPXCHG( 24, 26 ); CMPXCHG( 25, 27 );
      CMPXCHG( 28, 30 ); CMPXCHG(  2, 16 ); CMPXCHG(  3, 17 );
      CMPXCHG(  6, 20 ); CMPXCHG(  7, 21 ); CMPXCHG( 10, 24 );
      CMPXCHG( 11, 25 ); CMPXCHG( 14, 28 ); CMPXCHG( 15, 29 );
      CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 ); CMPXCHG(  6, 12 );
      CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 ); CMPXCHG( 11, 17 );
      CMPXCHG( 14, 20 ); CMPXCHG( 15, 21 ); CMPXCHG( 18, 24 );
      CMPXCHG( 19, 25 ); CMPXCHG( 22, 28 ); CMPXCHG( 23, 29 );
      CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 ); CMPXCHG(  6,  8 );
      CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 ); CMPXCHG( 11, 13 );
      CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 ); CMPXCHG( 18, 20 );
      CMPXCHG( 19, 21 ); CMPXCHG( 22, 24 ); CMPXCHG( 23, 25 );
      CMPXCHG( 26, 28 ); CMPXCHG( 27, 29 ); CMPXCHG(  0,  1 );
      CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 ); CMPXCHG(  6,  7 );
      CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 ); CMPXCHG( 12, 13 );
      CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 ); CMPXCHG( 18, 19 );
      CMPXCHG( 20, 21 ); CMPXCHG( 22, 23 ); CMPXCHG( 24, 25 );
      CMPXCHG( 26, 27 ); CMPXCHG( 28, 29 ); CMPXCHG(  1, 16 );
      CMPXCHG(  3, 18 ); CMPXCHG(  5, 20 ); CMPXCHG(  7, 22 );
      CMPXCHG(  9, 24 ); CMPXCHG( 11, 26 ); CMPXCHG( 13, 28 );
      CMPXCHG( 15, 30 ); CMPXCHG(  9, 16 ); CMPXCHG( 11, 18 );
      CMPXCHG( 13, 20 ); CMPXCHG( 15, 22 ); CMPXCHG( 13, 16 );
      CMPXCHG( 15, 18 ); CMPXCHG( 15, 16 );
      return pcl::Min( i[15], i[16] );
   case 32:
      CMPXCHG(  0, 16 ); CMPXCHG(  1, 17 ); CMPXCHG(  2, 18 );
      CMPXCHG(  3, 19 ); CMPXCHG(  4, 20 ); CMPXCHG(  5, 21 );
      CMPXCHG(  6, 22 ); CMPXCHG(  7, 23 ); CMPXCHG(  8, 24 );
      CMPXCHG(  9, 25 ); CMPXCHG( 10, 26 ); CMPXCHG( 11, 27 );
      CMPXCHG( 12, 28 ); CMPXCHG( 13, 29 ); CMPXCHG( 14, 30 );
      CMPXCHG( 15, 31 ); CMPXCHG(  0,  8 ); CMPXCHG(  1,  9 );
      CMPXCHG(  2, 10 ); CMPXCHG(  3, 11 ); CMPXCHG(  4, 12 );
      CMPXCHG(  5, 13 ); CMPXCHG(  6, 14 ); CMPXCHG(  7, 15 );
      CMPXCHG( 16, 24 ); CMPXCHG( 17, 25 ); CMPXCHG( 18, 26 );
      CMPXCHG( 19, 27 ); CMPXCHG( 20, 28 ); CMPXCHG( 21, 29 );
      CMPXCHG( 22, 30 ); CMPXCHG( 23, 31 ); CMPXCHG(  8, 16 );
      CMPXCHG(  9, 17 ); CMPXCHG( 10, 18 ); CMPXCHG( 11, 19 );
      CMPXCHG( 12, 20 ); CMPXCHG( 13, 21 ); CMPXCHG( 14, 22 );
      CMPXCHG( 15, 23 ); CMPXCHG(  0,  4 ); CMPXCHG(  1,  5 );
      CMPXCHG(  2,  6 ); CMPXCHG(  3,  7 ); CMPXCHG(  8, 12 );
      CMPXCHG(  9, 13 ); CMPXCHG( 10, 14 ); CMPXCHG( 11, 15 );
      CMPXCHG( 16, 20 ); CMPXCHG( 17, 21 ); CMPXCHG( 18, 22 );
      CMPXCHG( 19, 23 ); CMPXCHG( 24, 28 ); CMPXCHG( 25, 29 );
      CMPXCHG( 26, 30 ); CMPXCHG( 27, 31 ); CMPXCHG(  4, 16 );
      CMPXCHG(  5, 17 ); CMPXCHG(  6, 18 ); CMPXCHG(  7, 19 );
      CMPXCHG( 12, 24 ); CMPXCHG( 13, 25 ); CMPXCHG( 14, 26 );
      CMPXCHG( 15, 27 ); CMPXCHG(  4,  8 ); CMPXCHG(  5,  9 );
      CMPXCHG(  6, 10 ); CMPXCHG(  7, 11 ); CMPXCHG( 12, 16 );
      CMPXCHG( 13, 17 ); CMPXCHG( 14, 18 ); CMPXCHG( 15, 19 );
      CMPXCHG( 20, 24 ); CMPXCHG( 21, 25 ); CMPXCHG( 22, 26 );
      CMPXCHG( 23, 27 ); CMPXCHG(  0,  2 ); CMPXCHG(  1,  3 );
      CMPXCHG(  4,  6 ); CMPXCHG(  5,  7 ); CMPXCHG(  8, 10 );
      CMPXCHG(  9, 11 ); CMPXCHG( 12, 14 ); CMPXCHG( 13, 15 );
      CMPXCHG( 16, 18 ); CMPXCHG( 17, 19 ); CMPXCHG( 20, 22 );
      CMPXCHG( 21, 23 ); CMPXCHG( 24, 26 ); CMPXCHG( 25, 27 );
      CMPXCHG( 28, 30 ); CMPXCHG( 29, 31 ); CMPXCHG(  2, 16 );
      CMPXCHG(  3, 17 ); CMPXCHG(  6, 20 ); CMPXCHG(  7, 21 );
      CMPXCHG( 10, 24 ); CMPXCHG( 11, 25 ); CMPXCHG( 14, 28 );
      CMPXCHG( 15, 29 ); CMPXCHG(  2,  8 ); CMPXCHG(  3,  9 );
      CMPXCHG(  6, 12 ); CMPXCHG(  7, 13 ); CMPXCHG( 10, 16 );
      CMPXCHG( 11, 17 ); CMPXCHG( 14, 20 ); CMPXCHG( 15, 21 );
      CMPXCHG( 18, 24 ); CMPXCHG( 19, 25 ); CMPXCHG( 22, 28 );
      CMPXCHG( 23, 29 ); CMPXCHG(  2,  4 ); CMPXCHG(  3,  5 );
      CMPXCHG(  6,  8 ); CMPXCHG(  7,  9 ); CMPXCHG( 10, 12 );
      CMPXCHG( 11, 13 ); CMPXCHG( 14, 16 ); CMPXCHG( 15, 17 );
      CMPXCHG( 18, 20 ); CMPXCHG( 19, 21 ); CMPXCHG( 22, 24 );
      CMPXCHG( 23, 25 ); CMPXCHG( 26, 28 ); CMPXCHG( 27, 29 );
      CMPXCHG(  0,  1 ); CMPXCHG(  2,  3 ); CMPXCHG(  4,  5 );
      CMPXCHG(  6,  7 ); CMPXCHG(  8,  9 ); CMPXCHG( 10, 11 );
      CMPXCHG( 12, 13 ); CMPXCHG( 14, 15 ); CMPXCHG( 16, 17 );
      CMPXCHG( 18, 19 ); CMPXCHG( 20, 21 ); CMPXCHG( 22, 23 );
      CMPXCHG( 24, 25 ); CMPXCHG( 26, 27 ); CMPXCHG( 28, 29 );
      CMPXCHG( 30, 31 ); CMPXCHG(  1, 16 ); CMPXCHG(  3, 18 );
      CMPXCHG(  5, 20 ); CMPXCHG(  7, 22 ); CMPXCHG(  9, 24 );
      CMPXCHG( 11, 26 ); CMPXCHG( 13, 28 ); CMPXCHG( 15, 30 );
      CMPXCHG(  9, 16 ); CMPXCHG( 11, 18 ); CMPXCHG( 13, 20 );
      CMPXCHG( 15, 22 ); CMPXCHG( 13, 16 ); CMPXCHG( 15, 18 );
      return MEAN( i[15], i[16] );
   default:
      {
         distance_type n2 = n >> 1;
         if ( n & 1 )
            return *pcl::Select( i, j, n2 );
         return MEAN( *pcl::Select( i, j, n2 ), *pcl::Select( i, j, n2-1 ) );
      }
   }
}

#undef CMPXCHG

double PCL_FUNC Median( unsigned char* i, unsigned char* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( signed char* i, signed char* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( wchar_t* i, wchar_t* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( unsigned short* i, unsigned short* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( signed short* i, signed short* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( unsigned int* i, unsigned int* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( signed int* i, signed int* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( unsigned long* i, unsigned long* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( signed long* i, signed long* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( unsigned long long* i, unsigned long long* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( signed long long* i, signed long long* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( float* i, float* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( double* i, double* j )
{
   return MedianImplementation( i, j );
}

double PCL_FUNC Median( long double* i, long double* j )
{
   return MedianImplementation( i, j );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Median.cpp - Released 2018-11-23T16:14:32Z
