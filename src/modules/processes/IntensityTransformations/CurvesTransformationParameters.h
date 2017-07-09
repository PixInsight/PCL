//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// CurvesTransformationParameters.h - Released 2017-07-09T18:07:33Z
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

#ifndef __CurvesTransformationParameters_h
#define __CurvesTransformationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CurveIndex
{
public:
   enum { R, G, B, RGBK, A, L, a, b, c, H, S, NumberOfCurves };
};

// ----------------------------------------------------------------------------

class TableIndexer
{
public:
   virtual int CurveTableIndex() const = 0;
};

// ----------------------------------------------------------------------------

class CurveTable : public MetaTable, public TableIndexer
{
public:

   CurveTable( MetaProcess* P ) : MetaTable( P ), TableIndexer()
   {
   }

   virtual IsoString Id() const = 0;
   virtual int CurveTableIndex() const = 0;
};

#define DECLARE_CURVE_TABLE_PARAMETER( classId, parId, idx )               \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public CurveTable                                          \
{                                                                          \
public:                                                                    \
   classId( MetaProcess* P ) : CurveTable( P )                             \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   virtual IsoString Id() const { return parId; }                          \
   virtual int CurveTableIndex() const { return idx; }                     \
}

DECLARE_CURVE_TABLE_PARAMETER( RedCurve,        "R", CurveIndex::R );
DECLARE_CURVE_TABLE_PARAMETER( GreenCurve,      "G", CurveIndex::G );
DECLARE_CURVE_TABLE_PARAMETER( BlueCurve,       "B", CurveIndex::B );
DECLARE_CURVE_TABLE_PARAMETER( RGBKCurve,       "K", CurveIndex::RGBK );
DECLARE_CURVE_TABLE_PARAMETER( AlphaCurve,      "A", CurveIndex::A );
DECLARE_CURVE_TABLE_PARAMETER( CIELCurve,       "L", CurveIndex::L );
DECLARE_CURVE_TABLE_PARAMETER( CIEaCurve,       "a", CurveIndex::a );
DECLARE_CURVE_TABLE_PARAMETER( CIEbCurve,       "b", CurveIndex::b );
DECLARE_CURVE_TABLE_PARAMETER( CIEcCurve,       "c", CurveIndex::c );
DECLARE_CURVE_TABLE_PARAMETER( HueCurve,        "H", CurveIndex::H );
DECLARE_CURVE_TABLE_PARAMETER( SaturationCurve, "S", CurveIndex::S );

// ----------------------------------------------------------------------------

class CurvePointValue : public MetaDouble, public TableIndexer
{
public:

   CurvePointValue( MetaTable* );

   virtual IsoString Id() const = 0;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   virtual int CurveTableIndex() const
   {
      return dynamic_cast<const TableIndexer*>( Parent() )->CurveTableIndex();
   }

   virtual bool IsX() const = 0;
};

#define DECLARE_CURVE_POINT_VALUE_PARAMETER( classId, parId, isX )         \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public CurvePointValue                                     \
{                                                                          \
public:                                                                    \
   classId( MetaTable* t ) : CurvePointValue( t )                          \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   virtual IsoString Id() const { return parId; }                          \
   virtual bool IsX() const { return isX; }                                \
}

DECLARE_CURVE_POINT_VALUE_PARAMETER( XR, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YR, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XG, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YG, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XB, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YB, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XRGBK, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YRGBK, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XA, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YA, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XL, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YL, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( Xa, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( Ya, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( Xb, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( Yb, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( Xc, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( Yc, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XH, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YH, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XS, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YS, "y", false );

// ----------------------------------------------------------------------------

class CurveType : public MetaEnumeration, public TableIndexer
{
public:

   enum { CubicSpline,
          Linear,
          AkimaSubsplines,
          NumberOfCurveTypes,
          Default = AkimaSubsplines };

   CurveType( MetaProcess* );

   virtual IsoString Id() const = 0;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;

   virtual int CurveTableIndex() const = 0;
};

#define DECLARE_CURVE_TYPE_PARAMETER( classId, parId, idx )                \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public CurveType                                           \
{                                                                          \
public:                                                                    \
   classId( MetaProcess* P ) : CurveType( P )                              \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   virtual IsoString Id() const { return parId; }                          \
   virtual int CurveTableIndex() const { return idx; }                     \
}

DECLARE_CURVE_TYPE_PARAMETER( RedCurveType,        "Rt", CurveIndex::R );
DECLARE_CURVE_TYPE_PARAMETER( GreenCurveType,      "Gt", CurveIndex::G );
DECLARE_CURVE_TYPE_PARAMETER( BlueCurveType,       "Bt", CurveIndex::B );
DECLARE_CURVE_TYPE_PARAMETER( RGBKCurveType,       "Kt", CurveIndex::RGBK );
DECLARE_CURVE_TYPE_PARAMETER( AlphaCurveType,      "At", CurveIndex::A );
DECLARE_CURVE_TYPE_PARAMETER( CIELCurveType,       "Lt", CurveIndex::L );
DECLARE_CURVE_TYPE_PARAMETER( CIEaCurveType,       "at", CurveIndex::a );
DECLARE_CURVE_TYPE_PARAMETER( CIEbCurveType,       "bt", CurveIndex::b );
DECLARE_CURVE_TYPE_PARAMETER( CIEcCurveType,       "ct", CurveIndex::c );
DECLARE_CURVE_TYPE_PARAMETER( HueCurveType,        "Ht", CurveIndex::H );
DECLARE_CURVE_TYPE_PARAMETER( SaturationCurveType, "St", CurveIndex::S );

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CurvesTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF CurvesTransformationParameters.h - Released 2017-07-09T18:07:33Z
