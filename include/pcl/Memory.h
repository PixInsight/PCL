//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/Memory.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_Memory_h
#define __PCL_Memory_h

#include <pcl/Defs.h>

// Template formal parameters:
//
// FI   Forward iterator
// BI   Bidirectional iterator
// RI   Random access iterator
// UP   Unary predicate
// BP   Binary predicate
// T    Item type
// F    Function

// ### TODO: Document this file

namespace pcl
{

// ----------------------------------------------------------------------------
// void Fill( FI, FI, const T& )
// ----------------------------------------------------------------------------

template <class FI, class T> inline
void Fill( FI i, FI j, const T& v )
{
   for ( ; i != j; ++i )
      *i = v;
}

// ----------------------------------------------------------------------------
// void Fill( FI, FI, const T&, F )
// ----------------------------------------------------------------------------

template <class FI, class T, class F> inline
void Fill( FI i, FI j, const T& v, F f )
{
   for ( ; i != j; ++i )
      f( *i, v );
}

// ----------------------------------------------------------------------------
// FI1 Copy( FI1, FI2, FI2 )
// ----------------------------------------------------------------------------

template <class FI1, class FI2> inline
FI1 Copy( FI1 i1, FI2 i2, FI2 j2 )
{
   for ( ; i2 != j2; ++i1, ++i2 )
      *i1 = *i2;
   return i1;
}

// ----------------------------------------------------------------------------
// FI1 Copy( FI1, FI2, FI2, F )
// ----------------------------------------------------------------------------

template <class FI1, class FI2, class F> inline
FI1 Copy( FI1 i1, FI2 i2, FI2 j2, F f )
{
   for ( ; i2 != j2; ++i1, ++i2 )
      f( *i1, *i2 );
   return i1;
}

// ----------------------------------------------------------------------------
// BI1 CopyBackward( BI1, BI2, BI2 )
// ----------------------------------------------------------------------------

template <class BI1, class BI2> inline
BI1 CopyBackward( BI1 j1, BI2 i2, BI2 j2 )
{
   while ( i2 != j2 )
      *--j1 = *--j2;
   return j1;
}

// ----------------------------------------------------------------------------
// BI1 CopyBackward( BI1, BI2, BI2, F )
// ----------------------------------------------------------------------------

template <class BI1, class BI2, class F> inline
BI1 CopyBackward( BI1 j1, BI2 i2, BI2 j2, F f )
{
   while ( i2 != j2 )
      f( *--j1, *--j2 );
   return j1;
}

// ----------------------------------------------------------------------------
// BI1 CopyReversed( BI1, FI2, FI2 )
// ----------------------------------------------------------------------------

template <class BI1, class FI2> inline
BI1 CopyReversed( BI1 j1, FI2 i2, FI2 j2 )
{
   for ( ; i2 != j2; ++i2 )
      *--j1 = *i2;
   return j1;
}

// ----------------------------------------------------------------------------
// BI1 CopyReversed( BI1, FI2, FI2, F )
// ----------------------------------------------------------------------------

template <class BI1, class FI2, class F> inline
BI1 CopyReversed( BI1 j1, FI2 i2, FI2 j2, F f )
{
   for ( ; i2 != j2; ++i2 )
      f( *--j1, *i2 );
   return j1;
}

// ----------------------------------------------------------------------------
// RI1 Move( RI1, RI2, RI2 )
// ----------------------------------------------------------------------------

template <class RI1, class RI2> inline
RI1 Move( RI1 i1, RI2 i2, RI2 j2 )
{
   RI1 j1;
   if ( i1 < i2 )
      j1 = Copy( i1, i2, j2 );
   else
   {
      j1 = i1 + (j2-i2);
      CopyBackward( j1, i2, j2 );
   }
   return j1;
}

// ----------------------------------------------------------------------------
// RI1 Move( RI1, RI2, RI2, F )
// ----------------------------------------------------------------------------

template <class RI1, class RI2, class F> inline
RI1 Move( RI1 i1, RI2 i2, RI2 j2, F f )
{
   RI1 j1;
   if ( i1 < i2 )
      j1 = Copy( i1, i2, j2, f );
   else
   {
      j1 = i1 + (j2-i2);
      CopyBackward( j1, i2, j2, f );
   }
   return j1;
}

// ----------------------------------------------------------------------------
// RI1 MoveBackward( RI1, RI2, RI2 )
// ----------------------------------------------------------------------------

template <class RI1, class RI2> inline
RI1 MoveBackward( RI1 j1, RI2 i2, RI2 j2 )
{
   RI1 i1;
   if ( j2 < j1 )
      i1 = CopyBackward( j1, i2, j2 );
   else
   {
      i1 = j1 - (j2-i2);
      Copy( i1, i2, j2 );
   }
   return i1;
}

// ----------------------------------------------------------------------------
// RI1 MoveBackward( RI1, RI2, RI2, F )
// ----------------------------------------------------------------------------

template <class RI1, class RI2, class F> inline
RI1 MoveBackward( RI1 j1, RI2 i2, RI2 j2, F f )
{
   RI1 i1;
   if ( j2 < j1 )
      i1 = CopyBackward( j1, i2, j2, f );
   else
   {
      i1 = j1 - (j2-i2);
      Copy( i1, i2, j2, f );
   }
   return i1;
}

// ----------------------------------------------------------------------------
// void Replace( FI, FI, const T1&, const T2& )
// ----------------------------------------------------------------------------

template <class FI, class T1, class T2> inline
void Replace( FI i, FI j, const T1& v1, const T2& v2 )
{
   for ( ; i != j; ++i )
      if ( *i == v1 )
         *i = v2;
}

// ----------------------------------------------------------------------------
// void Replace( FI, FI, const T1&, const T2&, BP )
// ----------------------------------------------------------------------------

template <class FI, class T1, class T2, class BP> inline
void Replace( FI i, FI j, const T1& v1, const T2& v2, BP p )
{
   for ( ; i != j; ++i )
      if ( p( *i, v1 ) )
         *i = v2;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Memory_h

// ----------------------------------------------------------------------------
// EOF pcl/Memory.h - Released 2017-06-21T11:36:33Z
