//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SSSubframes*              TheSSSubframesParameter = nullptr;
SSSubframeEnabled*        TheSSSubframeEnabledParameter = nullptr;
SSSubframePath*           TheSSSubframePathParameter = nullptr;

SSSubframeScale*          TheSSSubframeScaleParameter = nullptr;
SSCameraGain*             TheSSCameraGainParameter = nullptr;
SSCameraResolution*       TheSSCameraResolutionParameter = nullptr;
SSSiteLocalMidnight*      TheSSSiteLocalMidnightParameter = nullptr;
SSScaleUnit*              TheSSScaleUnitParameter = nullptr;
SSDataUnit*               TheSSDataUnitParameter = nullptr;

SSMeasurements*              TheSSMeasurementsParameter = nullptr;
SSMeasurementEnabled*        TheSSMeasurementEnabledParameter = nullptr;
SSMeasurementLocked*         TheSSMeasurementLockedParameter = nullptr;
SSMeasurementPath*           TheSSMeasurementPathParameter = nullptr;
SSMeasurementFWHM*           TheSSMeasurementFWHMParameter = nullptr;

// ----------------------------------------------------------------------------

SSSubframes::SSSubframes( MetaProcess* P ) : MetaTable( P )
{
   TheSSSubframesParameter = this;
}

IsoString SSSubframes::Id() const
{
   return "subframes";
}

// ----------------------------------------------------------------------------

SSSubframeEnabled::SSSubframeEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheSSSubframeEnabledParameter = this;
}

IsoString SSSubframeEnabled::Id() const
{
   return "subframesEnabled";
}

bool SSSubframeEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSSubframePath::SSSubframePath( MetaTable* T ) : MetaString( T )
{
   TheSSSubframePathParameter = this;
}

IsoString SSSubframePath::Id() const
{
   return "subframesPath";
}

// ----------------------------------------------------------------------------

SSSubframeScale::SSSubframeScale( MetaProcess* P ) : MetaFloat( P )
{
   TheSSSubframeScaleParameter = this;
}

IsoString SSSubframeScale::Id() const
{
   return "subframeScale";
}

int SSSubframeScale::Precision() const
{
   return 4;
}

double SSSubframeScale::DefaultValue() const
{
   return 1;
}

double SSSubframeScale::MinimumValue() const
{
   return 0.01;
}

double SSSubframeScale::MaximumValue() const
{
   return 100.0;
}

// ----------------------------------------------------------------------------

SSCameraGain::SSCameraGain( MetaProcess* P ) : MetaFloat( P )
{
   TheSSCameraGainParameter = this;
}

IsoString SSCameraGain::Id() const
{
   return "cameraGain";
}

int SSCameraGain::Precision() const
{
   return 4;
}

double SSCameraGain::DefaultValue() const
{
   return 1;
}

double SSCameraGain::MinimumValue() const
{
   return 0.01;
}

double SSCameraGain::MaximumValue() const
{
   return 100.0;
}

// ----------------------------------------------------------------------------

SSCameraResolution::SSCameraResolution( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSCameraResolutionParameter = this;
}

IsoString SSCameraResolution::Id() const
{
   return "cameraResolution";
}

size_type SSCameraResolution::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSCameraResolution::ElementId( size_type i ) const
{
   switch ( i )
   {
      default:
      case Bits8:  return "Bits8";
      case Bits10: return "Bits10";
      case Bits12:  return "Bits12";
      case Bits14:  return "Bits14";
      case Bits16:  return "Bits16";
   }
}

int SSCameraResolution::ElementValue( size_type i ) const
{
   return int( i );
}

int SSCameraResolution::ElementData( size_type i ) const
{
   switch ( i )
   {
      default:
      case Bits8:  return 255;
      case Bits10: return 1023;
      case Bits12:  return 4095;
      case Bits14:  return 16383;
      case Bits16:  return 65535;
   }
}

IsoString SSCameraResolution::ElementLabel( size_type i ) const
{
   switch ( i )
   {
      default:
      case Bits8:  return "8-bit [0, 255]";
      case Bits10: return "10-bit [0, 1023]";
      case Bits12:  return "12-bit [0, 4095]";
      case Bits14:  return "14-bit [0, 16383]";
      case Bits16:  return "16-bit [0, 65535]";
   }
}

size_type SSCameraResolution::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SSScaleUnit::SSScaleUnit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSScaleUnitParameter = this;
}

IsoString SSScaleUnit::Id() const
{
   return "scaleUnit";
}

size_type SSScaleUnit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSScaleUnit::ElementId( size_type i ) const
{
   switch ( i )
   {
      default:
      case ArcSeconds:  return "ArcSeconds";
      case Pixel: return "Pixel";
   }
}

int SSScaleUnit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSScaleUnit::ElementData( size_type i ) const
{
   switch ( i )
   {
      default:
      case ArcSeconds:  return "arcsec";
      case Pixel: return "pixel";
   }
}

IsoString SSScaleUnit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
      default:
      case ArcSeconds:  return "Arcseconds (arcsec)";
      case Pixel: return "Pixels (pixel)";
   }
}

size_type SSScaleUnit::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SSDataUnit::SSDataUnit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSDataUnitParameter = this;
}

IsoString SSDataUnit::Id() const
{
   return "dataUnit";
}

size_type SSDataUnit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSDataUnit::ElementId( size_type i ) const
{
   switch ( i )
   {
      default:
      case Electron:  return "Electron";
      case DataNumber: return "DataNumber";
   }
}

int SSDataUnit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSDataUnit::ElementData( size_type i ) const
{
   switch ( i )
   {
      default:
      case Electron:  return "e-";
      case DataNumber: return "DN";
   }
}

IsoString SSDataUnit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
      default:
      case Electron:  return "Electrons (e-)";
      case DataNumber: return "Data Numbers (DN)";
   }
}

size_type SSDataUnit::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SSSiteLocalMidnight::SSSiteLocalMidnight( MetaProcess* P ) : MetaInt32( P )
{
   TheSSSiteLocalMidnightParameter = this;
}

IsoString SSSiteLocalMidnight::Id() const
{
   return "siteLocalMidnight";
}

double SSSiteLocalMidnight::DefaultValue() const
{
   return 24;
}

double SSSiteLocalMidnight::MinimumValue() const
{
   return 0;
}

double SSSiteLocalMidnight::MaximumValue() const
{
   return 24;
}

// ----------------------------------------------------------------------------

SSMeasurements::SSMeasurements( MetaProcess* P ) : MetaTable( P )
{
   TheSSMeasurementsParameter = this;
}

IsoString SSMeasurements::Id() const
{
   return "measurements";
}

// ----------------------------------------------------------------------------

SSMeasurementEnabled::SSMeasurementEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheSSMeasurementEnabledParameter = this;
}

IsoString SSMeasurementEnabled::Id() const
{
   return "measurementsEnabled";
}

bool SSMeasurementEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementLocked::SSMeasurementLocked( MetaTable* T ) : MetaBoolean( T )
{
   TheSSMeasurementLockedParameter = this;
}

IsoString SSMeasurementLocked::Id() const
{
   return "measurementsLocked";
}

bool SSMeasurementLocked::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementPath::SSMeasurementPath( MetaTable* T ) : MetaString( T )
{
   TheSSMeasurementPathParameter = this;
}

IsoString SSMeasurementPath::Id() const
{
   return "measurementsPath";
}

// ----------------------------------------------------------------------------

SSMeasurementFWHM::SSMeasurementFWHM( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementFWHMParameter = this;
}

IsoString SSMeasurementFWHM::Id() const
{
   return "measurementsFWHM";
}

int SSMeasurementFWHM::Precision() const
{
   return 4;
}

double SSMeasurementFWHM::DefaultValue() const
{
   return 1;
}

double SSMeasurementFWHM::MinimumValue() const
{
   return 0.01;
}

double SSMeasurementFWHM::MaximumValue() const
{
   return 100.0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.cpp - Released 2017-08-01T14:26:58Z
