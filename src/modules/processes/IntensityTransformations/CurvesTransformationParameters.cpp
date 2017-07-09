//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// CurvesTransformationParameters.cpp - Released 2017-07-09T18:07:33Z
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

#include "CurvesTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#define DEFINE_PARAMETER( classId )    classId* The##classId##Parameter = 0

DEFINE_PARAMETER( RedCurve );
DEFINE_PARAMETER( XR );
DEFINE_PARAMETER( YR );
DEFINE_PARAMETER( RedCurveType );

DEFINE_PARAMETER( GreenCurve );
DEFINE_PARAMETER( XG );
DEFINE_PARAMETER( YG );
DEFINE_PARAMETER( GreenCurveType );

DEFINE_PARAMETER( BlueCurve );
DEFINE_PARAMETER( XB );
DEFINE_PARAMETER( YB );
DEFINE_PARAMETER( BlueCurveType );

DEFINE_PARAMETER( RGBKCurve );
DEFINE_PARAMETER( XRGBK );
DEFINE_PARAMETER( YRGBK );
DEFINE_PARAMETER( RGBKCurveType );

DEFINE_PARAMETER( AlphaCurve );
DEFINE_PARAMETER( XA );
DEFINE_PARAMETER( YA );
DEFINE_PARAMETER( AlphaCurveType );

DEFINE_PARAMETER( CIELCurve );
DEFINE_PARAMETER( XL );
DEFINE_PARAMETER( YL );
DEFINE_PARAMETER( CIELCurveType );

DEFINE_PARAMETER( CIEaCurve );
DEFINE_PARAMETER( Xa );
DEFINE_PARAMETER( Ya );
DEFINE_PARAMETER( CIEaCurveType );

DEFINE_PARAMETER( CIEbCurve );
DEFINE_PARAMETER( Xb );
DEFINE_PARAMETER( Yb );
DEFINE_PARAMETER( CIEbCurveType );

DEFINE_PARAMETER( CIEcCurve );
DEFINE_PARAMETER( Xc );
DEFINE_PARAMETER( Yc );
DEFINE_PARAMETER( CIEcCurveType );

DEFINE_PARAMETER( HueCurve );
DEFINE_PARAMETER( XH );
DEFINE_PARAMETER( YH );
DEFINE_PARAMETER( HueCurveType );

DEFINE_PARAMETER( SaturationCurve );
DEFINE_PARAMETER( XS );
DEFINE_PARAMETER( YS );
DEFINE_PARAMETER( SaturationCurveType );

// ----------------------------------------------------------------------------

CurvePointValue::CurvePointValue( MetaTable* t ) : MetaDouble( t )
{
}

int CurvePointValue::Precision() const
{
   return 5;
}

double CurvePointValue::DefaultValue() const
{
   return 0;
}

double CurvePointValue::MinimumValue() const
{
   return 0;
}

double CurvePointValue::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CurveType::CurveType( MetaProcess* P ) : MetaEnumeration( P )
{
}

size_type CurveType::NumberOfElements() const
{
   return NumberOfCurveTypes;
}

IsoString CurveType::ElementId( size_type i ) const
{
   switch ( i )
   {
   case CubicSpline:     return "CubicSpline";
   case Linear:          return "Linear";
   default:
   case AkimaSubsplines: return "AkimaSubsplines";
   }
}

int CurveType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CurveType::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CurvesTransformationParameters.cpp - Released 2017-07-09T18:07:33Z
