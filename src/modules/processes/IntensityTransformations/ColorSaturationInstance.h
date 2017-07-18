//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0397
// ----------------------------------------------------------------------------
// ColorSaturationInstance.h - Released 2017-07-18T16:14:18Z
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

#ifndef __ColorSaturationInstance_h
#define __ColorSaturationInstance_h

#include <pcl/ProcessImplementation.h>

#include "CurveBase.h"
#include "ColorSaturationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class HSCurve : public CurveBase
{
public:

   typedef CurveBase::input_list    input_list;
   typedef CurveBase::output_list   output_list;
   typedef CurveBase::interpolator  interpolator;

   HSCurve() : CurveBase()
   {
      Initialize();
   }

   HSCurve( const HSCurve& ) = default;
   HSCurve& operator =( const HSCurve& ) = default;

   virtual void Reverse()
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         y[i] = -y[i];
   }

   virtual bool IsIdentity() const
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         if ( y[i] + 1 != 1 )
            return false;
      return true;
   }

   static double Interpolate( interpolator* i, double x )
   {
      return Range( (*i)( x ), -10.0, 10.0 );
   }

private:

   virtual void Initialize()
   {
      x.Add( 0.0 ); y.Add( 0.0 );
      x.Add( 1.0 ); y.Add( 0.0 );
   }
};

// ----------------------------------------------------------------------------

class ColorSaturationInstance : public ProcessImplementation
{
public:

   ColorSaturationInstance( const MetaProcess* );
   ColorSaturationInstance( const ColorSaturationInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View& v, String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   const HSCurve& Curve() const
   {
      return C;
   }

   template <typename T> T ShiftHueValue( T f ) const
   {
      if ( f > hueShift )
         f -= hueShift;
      else
         f += 1 - hueShift;
      return f;
   }

   template <typename T> T UnshiftHueValue( T f ) const
   {
      if ( f > 1 - hueShift )
         f -= 1-hueShift;
      else
         f += hueShift;
      return f;
   }

   void Preview( UInt16Image& ) const;

private:

   HSCurve C;
   float   hueShift;

   friend class ColorSaturationInterface;
   friend class ColorSaturationProcess;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorSaturationInstance_h

// ----------------------------------------------------------------------------
// EOF ColorSaturationInstance.h - Released 2017-07-18T16:14:18Z
