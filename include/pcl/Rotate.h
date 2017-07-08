//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/Rotate.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_Rotate_h
#define __PCL_Rotate_h

#include <pcl/Defs.h>

#include <pcl/Iterator.h>
#include <pcl/Utility.h>

// Template formal parameters:
//
// FI   Forward iterator
// BI   Bidirectional iterator
// RI   Random access iterator
// UP   Unary predicate
// BP   Binary predicate
// T    Item type
// F    Function

// ### TODO: Document this file.

namespace pcl
{

// ----------------------------------------------------------------------------
// void Reverse( BI, BI )
// ----------------------------------------------------------------------------

template <class BI> inline
void Reverse( BI i, BI j )
{
   __reverse__( i, j, IteratorClass( i ) );
}

template <class BI> inline
void __reverse__( BI i, BI j, BidirectionalIterator )
{
   for ( ; i != j && i != --j; ++i )
      Swap( *i, *j );
}

template <class RI> inline
void __reverse__( RI i, RI j, RandomAccessIterator )
{
   if ( i < j )
      for ( ; i < --j; ++i )
         Swap( *i, *j );
}

// ----------------------------------------------------------------------------
// void Rotate( FI, FI, FI )
// ----------------------------------------------------------------------------

template <class FI> inline
void Rotate( FI i, FI m, FI j )
{
   if ( m != i && m != j )
      __rotate__( i, m, j, IteratorClass( i ) );
}

template <class FI> inline
void __rotate__( FI i, FI m, FI j, ForwardIterator )
{
   for ( FI k = m; ; )
   {
      Swap( *i, *k );
      if ( ++i == m )
         if ( ++k == j )
            break;
         else
            m = k;
      else if ( ++k == j )
         k = m;
   }
}

template <class BI> inline
void __rotate__( BI i, BI m, BI j, BidirectionalIterator )
{
   Reverse( i, m );
   Reverse( m, j );
   Reverse( i, j );
}

template <class RI> inline
void __rotate__( RI i, RI m, RI j, RandomAccessIterator )
{
   __rotate_ri__( i, m, j, ItemType( i ) );
}

template <class RI, class T> inline
void __rotate_ri__( RI i, RI m, RI j, const T* )
{
   distance_type d = m-i;
   distance_type n = j-i;

   for ( distance_type a = d; a != 0; )
   {
      distance_type b = n % a;
      n = a;
      a = b;
   }

   if ( n < j-i )
      for ( ; 0 < n; --n )
      {
         RI x = i+n;
         RI y = x;
         RI z = y+d;
         if ( z == j )
            z = i;

         T v = *x;

         while ( z != x )
         {
            *y = *z;
            y = z;
            z = (d < j-z) ? z+d : i+(d-(j-z));
         }

         *y = v;
      }
}

// ----------------------------------------------------------------------------
// void Shift( FI, FI, FI, T )
// void ShiftLeft( FI, FI, FI, T )
// ----------------------------------------------------------------------------

template <class FI, class T> inline
void Shift( FI i, FI m, FI j, const T& v )
{
   if ( m != i )
   {
      for ( ; m != j; ++m, ++i )
         *i = *m;
      for ( ; i != j; ++i )
         *i = v;
   }
}

template <class FI, class T> inline
void ShiftLeft( FI i, FI m, FI j, const T& v )
{
   Shift( i, m, j, v );
}

// ----------------------------------------------------------------------------
// void ShiftRight( FI, FI, FI, T )
// ----------------------------------------------------------------------------

template <class FI, class T> inline
void ShiftRight( FI i, FI m, FI j, const T& v )
{
   if ( i != j && m != j )
      __shift_right__( i, m, j, v, IteratorClass( i ) );
}

template <class FI, class T> inline
void __shift_right__( FI i, FI m, FI j, const T& v, ForwardIterator )
{
   do
   {
      if ( m != i )
      {
         FI k = i;
         do
            Swap( *++k, *i );
         while ( k != m );
         *m = *i;
      }

      *i = v;
      ++i;
   }
   while ( ++m != j );
}

template <class BI, class T> inline
void __shift_right__( BI i, BI m, BI j, const T& v, BidirectionalIterator )
{
   while ( m != i )
      *--j = *--m;
   do
      *m = v;
   while ( ++m != j );
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Rotate_h

// ----------------------------------------------------------------------------
// EOF pcl/Rotate.h - Released 2017-06-28T11:58:36Z
