//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0173
// ----------------------------------------------------------------------------
// B3EParameters.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#include "B3EParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

B3EInputViewId1*         TheB3EInputViewId1Parameter = 0;
B3EInputViewId2*         TheB3EInputViewId2Parameter = 0;
B3EInputCenter1*         TheB3EInputCenter1Parameter = 0;
B3EInputCenter2*         TheB3EInputCenter2Parameter = 0;
B3EOutputCenter*         TheB3EOutputCenterParameter = 0;
B3EIntensityUnits*       TheB3EIntensityUnitsParameter = 0;
B3ESyntheticImage*       TheB3ESyntheticImageParameter = 0;
B3EThermalMap*           TheB3EThermalMapParameter = 0;
B3EOutOfRangeMask*       TheB3EOutOfRangeMaskParameter = 0;
B3ESyntheticImageViewId* TheB3ESyntheticImageViewIdParameter = 0;
B3EThermalMapViewId*     TheB3EThermalMapViewIdParameter = 0;
B3EOutOfRangeMaskViewId* TheB3EOutOfRangeMaskViewIdParameter = 0;

// Background Calibration 1
B3ESubstractBackground1*               TheB3ESubstractBackground1Parameter = 0;
B3EBackgroundReferenceViewId1*         TheB3EBackgroundReferenceViewId1Parameter = 0;
B3EBackgroundLow1*                     TheB3EBackgroundLow1Parameter = 0;
B3EBackgroundHigh1*                    TheB3EBackgroundHigh1Parameter = 0;
B3EBackgroundUseROI1*                  TheB3EBackgroundUseROI1Parameter = 0;
B3EBackgroundROIX01*                   TheB3EBackgroundROIX01Parameter = 0;
B3EBackgroundROIY01*                   TheB3EBackgroundROIY01Parameter = 0;
B3EBackgroundROIX11*                   TheB3EBackgroundROIX11Parameter = 0;
B3EBackgroundROIY11*                   TheB3EBackgroundROIY11Parameter = 0;
B3EOutputBackgroundReferenceMask1*     TheB3EOutputBackgroundReferenceMask1Parameter = 0;

// Background Calibration 2
B3ESubstractBackground2*               TheB3ESubstractBackground2Parameter = 0;
B3EBackgroundReferenceViewId2*         TheB3EBackgroundReferenceViewId2Parameter = 0;
B3EBackgroundLow2*                     TheB3EBackgroundLow2Parameter = 0;
B3EBackgroundHigh2*                    TheB3EBackgroundHigh2Parameter = 0;
B3EBackgroundUseROI2*                  TheB3EBackgroundUseROI2Parameter = 0;
B3EBackgroundROIX02*                   TheB3EBackgroundROIX02Parameter = 0;
B3EBackgroundROIY02*                   TheB3EBackgroundROIY02Parameter = 0;
B3EBackgroundROIX12*                   TheB3EBackgroundROIX12Parameter = 0;
B3EBackgroundROIY12*                   TheB3EBackgroundROIY12Parameter = 0;
B3EOutputBackgroundReferenceMask2*     TheB3EOutputBackgroundReferenceMask2Parameter = 0;

// ----------------------------------------------------------------------------

B3EInputViewId1::B3EInputViewId1( MetaProcess* P ) : MetaString( P )
{
   TheB3EInputViewId1Parameter = this;
}

IsoString B3EInputViewId1::Id() const
{
   return "inputViewId1";
}

size_type B3EInputViewId1::MinLength() const
{
   return 1;
}

String B3EInputViewId1::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

B3EInputViewId2::B3EInputViewId2( MetaProcess* P ) : MetaString( P )
{
   TheB3EInputViewId2Parameter = this;
}

IsoString B3EInputViewId2::Id() const
{
   return "inputViewId2";
}

size_type B3EInputViewId2::MinLength() const
{
   return 1;
}

String B3EInputViewId2::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

B3EInputCenter1::B3EInputCenter1( MetaProcess* P ) : MetaDouble( P )
{
   TheB3EInputCenter1Parameter = this;
}

IsoString B3EInputCenter1::Id() const
{
   return "inputCenter1";
}

int B3EInputCenter1::Precision() const
{
   return 4;
}

double B3EInputCenter1::DefaultValue() const
{
   return 0;
}

double B3EInputCenter1::MinimumValue() const
{
   return 0; // 0 = undefined
}

double B3EInputCenter1::MaximumValue() const
{
   return 1.0e+10;
}

// ----------------------------------------------------------------------------

B3EInputCenter2::B3EInputCenter2( MetaProcess* P ) : MetaDouble( P )
{
   TheB3EInputCenter2Parameter = this;
}

IsoString B3EInputCenter2::Id() const
{
   return "inputCenter2";
}

int B3EInputCenter2::Precision() const
{
   return 4;
}

double B3EInputCenter2::DefaultValue() const
{
   return 0;
}

double B3EInputCenter2::MinimumValue() const
{
   return 0; // 0 = undefined
}

double B3EInputCenter2::MaximumValue() const
{
   return 1.0e+10;
}

// ----------------------------------------------------------------------------

B3EOutputCenter::B3EOutputCenter( MetaProcess* P ) : MetaDouble( P )
{
   TheB3EOutputCenterParameter = this;
}

IsoString B3EOutputCenter::Id() const
{
   return "outputCenter";
}

int B3EOutputCenter::Precision() const
{
   return 4;
}

double B3EOutputCenter::DefaultValue() const
{
   return 0;
}

double B3EOutputCenter::MinimumValue() const
{
   return 0; // 0 = undefined
}

double B3EOutputCenter::MaximumValue() const
{
   return 1.0e+10;
}

// ----------------------------------------------------------------------------

B3EIntensityUnits::B3EIntensityUnits( MetaProcess* P ) : MetaEnumeration( P )
{
   TheB3EIntensityUnitsParameter = this;
}

IsoString B3EIntensityUnits::Id() const
{
   return "intensityUnits";
}

size_type B3EIntensityUnits::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString B3EIntensityUnits::ElementId( size_type i ) const
{
   switch ( i )
   {
   case PhotonsFrequency:  return "IntensityUnits_PhotonsFrequency";
   case EnergyFrequency:   return "IntensityUnits_EnergyFrequency";
   default:
   case PhotonsWavelength: return "IntensityUnits_PhotonsWavelength";
   case EnergyWavelength:  return "IntensityUnits_EnergyWavelength";
   }
}

int B3EIntensityUnits::ElementValue( size_type i ) const
{
   return int( i );
}

size_type B3EIntensityUnits::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

B3ESyntheticImage::B3ESyntheticImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3ESyntheticImageParameter = this;
}

IsoString B3ESyntheticImage::Id() const
{
   return "syntheticImage";
}

bool B3ESyntheticImage::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

B3EThermalMap::B3EThermalMap( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3EThermalMapParameter = this;
}

IsoString B3EThermalMap::Id() const
{
   return "thermalMap";
}

bool B3EThermalMap::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

B3EOutOfRangeMask::B3EOutOfRangeMask( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3EOutOfRangeMaskParameter = this;
}

IsoString B3EOutOfRangeMask::Id() const
{
   return "outOfRangeMask";
}

bool B3EOutOfRangeMask::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

B3ESyntheticImageViewId::B3ESyntheticImageViewId( MetaProcess* P ) : MetaString( P )
{
   TheB3ESyntheticImageViewIdParameter = this;
}

IsoString B3ESyntheticImageViewId::Id() const
{
   return "syntheticImageViewId";
}

bool B3ESyntheticImageViewId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

B3EThermalMapViewId::B3EThermalMapViewId( MetaProcess* P ) : MetaString( P )
{
   TheB3EThermalMapViewIdParameter = this;
}

IsoString B3EThermalMapViewId::Id() const
{
   return "thermalMapViewId";
}

bool B3EThermalMapViewId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

B3EOutOfRangeMaskViewId::B3EOutOfRangeMaskViewId( MetaProcess* P ) : MetaString( P )
{
   TheB3EOutOfRangeMaskViewIdParameter = this;
}

IsoString B3EOutOfRangeMaskViewId::Id() const
{
   return "outOfRangeMaskViewId";
}

bool B3EOutOfRangeMaskViewId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

/*
 * Background Calibration 1
 */

// ----------------------------------------------------------------------------

B3ESubstractBackground1::B3ESubstractBackground1( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3ESubstractBackground1Parameter = this;
}

IsoString B3ESubstractBackground1::Id() const
{
   return "substractBackground1";
}

bool B3ESubstractBackground1::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

B3EBackgroundReferenceViewId1::B3EBackgroundReferenceViewId1( MetaProcess* P ) : MetaString( P )
{
   TheB3EBackgroundReferenceViewId1Parameter = this;
}

IsoString B3EBackgroundReferenceViewId1::Id() const
{
   return "backgroundReferenceViewId1";
}

// ----------------------------------------------------------------------------

B3EBackgroundLow1::B3EBackgroundLow1( MetaProcess* P ) : MetaFloat( P )
{
   TheB3EBackgroundLow1Parameter = this;
}

IsoString B3EBackgroundLow1::Id() const
{
   return "backgroundLow1";
}

int B3EBackgroundLow1::Precision() const
{
   return 6;
}

double B3EBackgroundLow1::MinimumValue() const
{
   return 0;
}

double B3EBackgroundLow1::MaximumValue() const
{
   return 1;
}

double B3EBackgroundLow1::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

B3EBackgroundHigh1::B3EBackgroundHigh1( MetaProcess* P ) : MetaFloat( P )
{
   TheB3EBackgroundHigh1Parameter = this;
}

IsoString B3EBackgroundHigh1::Id() const
{
   return "backgroundHigh1";
}

int B3EBackgroundHigh1::Precision() const
{
   return 6;
}

double B3EBackgroundHigh1::MinimumValue() const
{
   return 0;
}

double B3EBackgroundHigh1::MaximumValue() const
{
   return 1;
}

double B3EBackgroundHigh1::DefaultValue() const
{
   return 0.1;
}

// ----------------------------------------------------------------------------

B3EBackgroundUseROI1::B3EBackgroundUseROI1( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3EBackgroundUseROI1Parameter = this;
}

IsoString B3EBackgroundUseROI1::Id() const
{
   return "backgroundUseROI1";
}

bool B3EBackgroundUseROI1::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIX01::B3EBackgroundROIX01( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIX01Parameter = this;
}

IsoString B3EBackgroundROIX01::Id() const
{
   return "backgroundROIX01";
}

double B3EBackgroundROIX01::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIX01::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIX01::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIY01::B3EBackgroundROIY01( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIY01Parameter = this;
}

IsoString B3EBackgroundROIY01::Id() const
{
   return "backgroundROIY01";
}

double B3EBackgroundROIY01::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIY01::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIY01::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIX11::B3EBackgroundROIX11( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIX11Parameter = this;
}

IsoString B3EBackgroundROIX11::Id() const
{
   return "backgroundROIX11";
}

double B3EBackgroundROIX11::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIX11::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIX11::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIY11::B3EBackgroundROIY11( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIY11Parameter = this;
}

IsoString B3EBackgroundROIY11::Id() const
{
   return "backgroundROIY11";
}

double B3EBackgroundROIY11::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIY11::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIY11::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EOutputBackgroundReferenceMask1::B3EOutputBackgroundReferenceMask1( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3EOutputBackgroundReferenceMask1Parameter = this;
}

IsoString B3EOutputBackgroundReferenceMask1::Id() const
{
   return "outputBackgroundReferenceMask1";
}

bool B3EOutputBackgroundReferenceMask1::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

/*
 * Background Calibration 2
 */

// ----------------------------------------------------------------------------

B3ESubstractBackground2::B3ESubstractBackground2( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3ESubstractBackground2Parameter = this;
}

IsoString B3ESubstractBackground2::Id() const
{
   return "substractBackground2";
}

bool B3ESubstractBackground2::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

B3EBackgroundReferenceViewId2::B3EBackgroundReferenceViewId2( MetaProcess* P ) : MetaString( P )
{
   TheB3EBackgroundReferenceViewId2Parameter = this;
}

IsoString B3EBackgroundReferenceViewId2::Id() const
{
   return "backgroundReferenceViewId2";
}

// ----------------------------------------------------------------------------

B3EBackgroundLow2::B3EBackgroundLow2( MetaProcess* P ) : MetaFloat( P )
{
   TheB3EBackgroundLow2Parameter = this;
}

IsoString B3EBackgroundLow2::Id() const
{
   return "backgroundLow2";
}

int B3EBackgroundLow2::Precision() const
{
   return 6;
}

double B3EBackgroundLow2::MinimumValue() const
{
   return 0;
}

double B3EBackgroundLow2::MaximumValue() const
{
   return 2;
}

double B3EBackgroundLow2::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

B3EBackgroundHigh2::B3EBackgroundHigh2( MetaProcess* P ) : MetaFloat( P )
{
   TheB3EBackgroundHigh2Parameter = this;
}

IsoString B3EBackgroundHigh2::Id() const
{
   return "backgroundHigh2";
}

int B3EBackgroundHigh2::Precision() const
{
   return 6;
}

double B3EBackgroundHigh2::MinimumValue() const
{
   return 0;
}

double B3EBackgroundHigh2::MaximumValue() const
{
   return 2;
}

double B3EBackgroundHigh2::DefaultValue() const
{
   return 0.2;
}

// ----------------------------------------------------------------------------

B3EBackgroundUseROI2::B3EBackgroundUseROI2( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3EBackgroundUseROI2Parameter = this;
}

IsoString B3EBackgroundUseROI2::Id() const
{
   return "backgroundUseROI2";
}

bool B3EBackgroundUseROI2::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIX02::B3EBackgroundROIX02( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIX02Parameter = this;
}

IsoString B3EBackgroundROIX02::Id() const
{
   return "backgroundROIX02";
}

double B3EBackgroundROIX02::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIX02::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIX02::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIY02::B3EBackgroundROIY02( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIY02Parameter = this;
}

IsoString B3EBackgroundROIY02::Id() const
{
   return "backgroundROIY02";
}

double B3EBackgroundROIY02::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIY02::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIY02::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIX12::B3EBackgroundROIX12( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIX12Parameter = this;
}

IsoString B3EBackgroundROIX12::Id() const
{
   return "backgroundROIX12";
}

double B3EBackgroundROIX12::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIX12::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIX12::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EBackgroundROIY12::B3EBackgroundROIY12( MetaProcess* P ) : MetaInt32( P )
{
   TheB3EBackgroundROIY12Parameter = this;
}

IsoString B3EBackgroundROIY12::Id() const
{
   return "backgroundROIY12";
}

double B3EBackgroundROIY12::DefaultValue() const
{
   return 0;
}

double B3EBackgroundROIY12::MinimumValue() const
{
   return 0;
}

double B3EBackgroundROIY12::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

B3EOutputBackgroundReferenceMask2::B3EOutputBackgroundReferenceMask2( MetaProcess* P ) : MetaBoolean( P )
{
   TheB3EOutputBackgroundReferenceMask2Parameter = this;
}

IsoString B3EOutputBackgroundReferenceMask2::Id() const
{
   return "outputBackgroundReferenceMask2";
}

bool B3EOutputBackgroundReferenceMask2::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF B3EParameters.cpp - Released 2017-07-09T18:07:33Z
