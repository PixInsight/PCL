//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0424
// ----------------------------------------------------------------------------
// ColorSaturationParameters.h - Released 2018-12-12T09:25:25Z
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

#ifndef __ColorSaturationParameters_h
#define __ColorSaturationParameters_h

#include "CurvesTransformationParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

DECLARE_CURVE_TABLE_PARAMETER( HueSaturationCurve, "HS", 0 );

// ----------------------------------------------------------------------------

class HSCurvePointValue : public CurvePointValue
{
public:

   HSCurvePointValue( MetaTable* );

   virtual IsoString Id() const = 0;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   virtual bool IsX() const = 0;
};

#define DECLARE_HSCURVE_POINT_VALUE_PARAMETER( classId, parId, isX )       \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public HSCurvePointValue                                   \
{                                                                          \
public:                                                                    \
   classId( MetaTable* t ) : HSCurvePointValue( t )                        \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   virtual IsoString Id() const { return parId; }                          \
   virtual bool IsX() const { return isX; }                                \
}

DECLARE_HSCURVE_POINT_VALUE_PARAMETER( XHS, "x", true  );
DECLARE_HSCURVE_POINT_VALUE_PARAMETER( YHS, "y", false );

// ----------------------------------------------------------------------------

DECLARE_CURVE_TYPE_PARAMETER( HueSaturationCurveType, "HSt", 0 );

// ----------------------------------------------------------------------------

class HueShift : public MetaFloat
{
public:

   HueShift( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern HueShift* TheHueShiftParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ColorSaturationParameters_h

// ----------------------------------------------------------------------------
// EOF ColorSaturationParameters.h - Released 2018-12-12T09:25:25Z
