//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0345
// ----------------------------------------------------------------------------
// CurveBase.h - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __CurveBase_h
#define __CurveBase_h

#include <pcl/Array.h>
#include <pcl/Point.h>
#include <pcl/UnidimensionalInterpolation.h>

#include "CurvesTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CurveBase
{
public:

   typedef SortedArray<double>   input_list;
   typedef Array<double>         output_list;

   typedef UnidimensionalInterpolation<double>  interpolator;

   CurveBase() :
      x(),
      y(),
      type( CurveType::Default )
   {
   }

   CurveBase( const CurveBase& ) = default;
   CurveBase& operator =( const CurveBase& ) = default;

   virtual ~CurveBase()
   {
   }

   size_type Length() const
   {
      return x.Length();
   }

   DPoint operator []( size_type i ) const
   {
      return DPoint( x[i], y[i] );
   }

   double X( size_type i ) const
   {
      return x[i];
   }

   double Y( size_type i ) const
   {
      return y[i];
   }

   double& Y( size_type i )
   {
      return y[i];
   }

   const double* XVector() const
   {
      return x.Begin();
   }

   const double* YVector() const
   {
      return y.Begin();
   }

   void Reset()
   {
      x.Clear();
      y.Clear();
      Initialize();
   }

   void Remove( size_type i )
   {
      if ( i > 0 && i < Length()-1 )
      {
         x.Remove( x.MutableAt( i ) );
         y.Remove( y.At( i ) );
      }
   }

   void Add( double px, double py )
   {
      input_list::const_iterator i = x.Add( px );
      y.Insert( y.At( i - x.Begin() ), py );
   }

   void Add( const DPoint& p )
   {
      Add( p.x, p.y );
   }

   size_type Search( double px ) const
   {
      return x.Search( px ) - x.Begin();
   }

   int Type() const
   {
      return type;
   }

   void SetType( int t )
   {
      type = t;
   }

   virtual void Reverse() = 0;

   virtual bool IsIdentity() const = 0;

   interpolator* InitInterpolator() const;

   static double Interpolate( interpolator* i, double x )
   {
      return Range( (*i)( x ), 0.0, 1.0 );
   }

protected:

   input_list  x;
   output_list y;
   int32       type;

   virtual void Initialize() = 0;

   friend class CurvesTransformationInstance;
   friend class ColorSaturationInstance;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CurveBase_h

// ----------------------------------------------------------------------------
// EOF CurveBase.h - Released 2015/11/26 16:00:13 UTC
