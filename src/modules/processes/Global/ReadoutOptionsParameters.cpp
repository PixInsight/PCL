//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0393
// ----------------------------------------------------------------------------
// ReadoutOptionsParameters.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "ReadoutOptionsParameters.h"

#include <pcl/ReadoutOptions.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptionsData*              TheReadoutOptionsDataParameter = nullptr;
ReadoutOptionsMode*              TheReadoutOptionsModeParameter = nullptr;
ReadoutOptionsProbeSize*         TheReadoutOptionsProbeSizeParameter = nullptr;
ReadoutOptionsPreviewSize*       TheReadoutOptionsPreviewSizeParameter = nullptr;
ReadoutOptionsPreviewZoomFactor* TheReadoutOptionsPreviewZoomFactorParameter = nullptr;
ReadoutRealPrecision*            TheReadoutRealPrecisionParameter = nullptr;
ReadoutIntegerRange*             TheReadoutIntegerRangeParameter = nullptr;
ReadoutAlpha*                    TheReadoutAlphaParameter = nullptr;
ReadoutMask*                     TheReadoutMaskParameter = nullptr;
ReadoutPreview*                  TheReadoutPreviewParameter = nullptr;
ReadoutPreviewCenter*            TheReadoutPreviewCenterParameter = nullptr;
ReadoutShowEquatorial*           TheReadoutShowEquatorialParameter = nullptr;
ReadoutShowEcliptic*             TheReadoutShowEclipticParameter = nullptr;
ReadoutShowGalactic*             TheReadoutShowGalacticParameter = nullptr;
ReadoutCoordinateItems*          TheReadoutCoordinateItemsParameter = nullptr;
ReadoutCoordinatePrecision*      TheReadoutCoordinatePrecisionParameter = nullptr;
ReadoutBroadcast*                TheReadoutBroadcastParameter = nullptr;
ReadoutReal*                     TheReadoutRealParameter = nullptr;

// ----------------------------------------------------------------------------

ReadoutOptionsData::ReadoutOptionsData( MetaProcess* p ) : MetaEnumeration( p )
{
   TheReadoutOptionsDataParameter = this;
}

IsoString ReadoutOptionsData::Id() const
{
   return "data";
}

size_type ReadoutOptionsData::NumberOfElements() const
{
   return ReadoutData::NumberOfModes;
}

IsoString ReadoutOptionsData::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case ReadoutData::RGBK:   return "RGBK";
   case ReadoutData::RGBL:   return "RGBL";
   case ReadoutData::RGBY:   return "RGBY";
   case ReadoutData::CIEXYZ: return "CIEXYZ";
   case ReadoutData::CIELab: return "CIELab";
   case ReadoutData::CIELch: return "CIELch";
   case ReadoutData::HSV:    return "HSV";
   case ReadoutData::HSI:    return "HSI";
   }
}

int ReadoutOptionsData::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ReadoutOptionsData::DefaultValueIndex() const
{
   return ReadoutOptions().Data();
}

// ----------------------------------------------------------------------------

ReadoutOptionsMode::ReadoutOptionsMode( MetaProcess* p ) : MetaEnumeration( p )
{
   TheReadoutOptionsModeParameter = this;
}

IsoString ReadoutOptionsMode::Id() const
{
   return "mode";
}

size_type ReadoutOptionsMode::NumberOfElements() const
{
   return ReadoutMode::NumberOfModes;
}

IsoString ReadoutOptionsMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case ReadoutMode::Mean:    return "mean";
   case ReadoutMode::Median:  return "median";
   case ReadoutMode::Minimum: return "minimum";
   case ReadoutMode::Maximum: return "maximum";
   }
}

int ReadoutOptionsMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ReadoutOptionsMode::DefaultValueIndex() const
{
   return ReadoutOptions().Mode();
}

// ----------------------------------------------------------------------------

ReadoutOptionsProbeSize::ReadoutOptionsProbeSize( MetaProcess* p ) : MetaInt32( p )
{
   TheReadoutOptionsProbeSizeParameter = this;
}

IsoString ReadoutOptionsProbeSize::Id() const
{
   return "probeSize";
}

bool ReadoutOptionsProbeSize::NeedsValidation() const
{
   return true;
}

double ReadoutOptionsProbeSize::DefaultValue() const
{
   return ReadoutOptions().ProbeSize();
}

double ReadoutOptionsProbeSize::MinimumValue() const
{
   return 1;
}

double ReadoutOptionsProbeSize::MaximumValue() const
{
   return ReadoutOptions::MaxProbeSize;
}

// ----------------------------------------------------------------------------

ReadoutOptionsPreviewSize::ReadoutOptionsPreviewSize( MetaProcess* p ) : MetaInt32( p )
{
   TheReadoutOptionsPreviewSizeParameter = this;
}

IsoString ReadoutOptionsPreviewSize::Id() const
{
   return "previewSize";
}

bool ReadoutOptionsPreviewSize::NeedsValidation() const
{
   return true;
}

double ReadoutOptionsPreviewSize::DefaultValue() const
{
   return ReadoutOptions().PreviewSize();
}

double ReadoutOptionsPreviewSize::MinimumValue() const
{
   return ReadoutOptions::MinPreviewSize;
}

double ReadoutOptionsPreviewSize::MaximumValue() const
{
   return ReadoutOptions::MaxPreviewSize;
}

// ----------------------------------------------------------------------------

ReadoutOptionsPreviewZoomFactor::ReadoutOptionsPreviewZoomFactor( MetaProcess* p ) : MetaInt32( p )
{
   TheReadoutOptionsPreviewZoomFactorParameter = this;
}

IsoString ReadoutOptionsPreviewZoomFactor::Id() const
{
   return "previewZoomFactor";
}

double ReadoutOptionsPreviewZoomFactor::DefaultValue() const
{
   return ReadoutOptions().PreviewZoomFactor();
}

double ReadoutOptionsPreviewZoomFactor::MinimumValue() const
{
   return 1;
}

double ReadoutOptionsPreviewZoomFactor::MaximumValue() const
{
   return ReadoutOptions::MaxPreviewZoomFactor;
}

// ----------------------------------------------------------------------------

ReadoutRealPrecision::ReadoutRealPrecision( MetaProcess* p ) : MetaInt32( p )
{
   TheReadoutRealPrecisionParameter = this;
}

IsoString ReadoutRealPrecision::Id() const
{
   return "precision";
}

double ReadoutRealPrecision::DefaultValue() const
{
   return ReadoutOptions().Precision();
}

double ReadoutRealPrecision::MinimumValue() const
{
   return 0;
}

double ReadoutRealPrecision::MaximumValue() const
{
   return ReadoutOptions::MaxPrecision;
}

// ----------------------------------------------------------------------------

ReadoutIntegerRange::ReadoutIntegerRange( MetaProcess* p ) : MetaUInt32( p )
{
   TheReadoutIntegerRangeParameter = this;
}

IsoString ReadoutIntegerRange::Id() const
{
   return "integerRange";
}

double ReadoutIntegerRange::DefaultValue() const
{
   return ReadoutOptions().IntegerRange();
}

double ReadoutIntegerRange::MinimumValue() const
{
   return 1;
}

double ReadoutIntegerRange::MaximumValue() const
{
   return uint32_max;
}

// ----------------------------------------------------------------------------

ReadoutAlpha::ReadoutAlpha( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutAlphaParameter = this;
}

IsoString ReadoutAlpha::Id() const
{
   return "showAlpha";
}

bool ReadoutAlpha::DefaultValue() const
{
   return ReadoutOptions().ShowAlphaChannel();
}

// ----------------------------------------------------------------------------

ReadoutMask::ReadoutMask( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutMaskParameter = this;
}

IsoString ReadoutMask::Id() const
{
   return "showMask";
}

bool ReadoutMask::DefaultValue() const
{
   return ReadoutOptions().ShowMaskChannel();
}

// ----------------------------------------------------------------------------

ReadoutPreview::ReadoutPreview( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutPreviewParameter = this;
}

IsoString ReadoutPreview::Id() const
{
   return "showPreview";
}

bool ReadoutPreview::DefaultValue() const
{
   return ReadoutOptions().ShowPreview();
}

// ----------------------------------------------------------------------------

ReadoutPreviewCenter::ReadoutPreviewCenter( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutPreviewCenterParameter = this;
}

IsoString ReadoutPreviewCenter::Id() const
{
   return "previewCenter";
}

bool ReadoutPreviewCenter::DefaultValue() const
{
   return ReadoutOptions().ShowPreviewCenter();
}

// ----------------------------------------------------------------------------

ReadoutShowEquatorial::ReadoutShowEquatorial( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutShowEquatorialParameter = this;
}

IsoString ReadoutShowEquatorial::Id() const
{
   return "showEquatorialCoordinates";
}

bool ReadoutShowEquatorial::DefaultValue() const
{
   return ReadoutOptions().ShowEquatorialCoordinates();
}

// ----------------------------------------------------------------------------

ReadoutShowEcliptic::ReadoutShowEcliptic( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutShowEclipticParameter = this;
}

IsoString ReadoutShowEcliptic::Id() const
{
   return "showEclipticCoordinates";
}

bool ReadoutShowEcliptic::DefaultValue() const
{
   return ReadoutOptions().ShowEclipticCoordinates();
}

// ----------------------------------------------------------------------------

ReadoutShowGalactic::ReadoutShowGalactic( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutShowGalacticParameter = this;
}

IsoString ReadoutShowGalactic::Id() const
{
   return "showGalacticCoordinates";
}

bool ReadoutShowGalactic::DefaultValue() const
{
   return ReadoutOptions().ShowGalacticCoordinates();
}

// ----------------------------------------------------------------------------

ReadoutCoordinateItems::ReadoutCoordinateItems( MetaProcess* p ) : MetaInt32( p )
{
   TheReadoutCoordinateItemsParameter = this;
}

IsoString ReadoutCoordinateItems::Id() const
{
   return "coordinateItems";
}

double ReadoutCoordinateItems::DefaultValue() const
{
   return ReadoutOptions().CoordinateItems();
}

double ReadoutCoordinateItems::MinimumValue() const
{
   return 1;
}

double ReadoutCoordinateItems::MaximumValue() const
{
   return 3;
}

// ----------------------------------------------------------------------------

ReadoutCoordinatePrecision::ReadoutCoordinatePrecision( MetaProcess* p ) : MetaInt32( p )
{
   TheReadoutCoordinatePrecisionParameter = this;
}

IsoString ReadoutCoordinatePrecision::Id() const
{
   return "coordinatePrecision";
}

double ReadoutCoordinatePrecision::DefaultValue() const
{
   return ReadoutOptions().CoordinatePrecision();
}

double ReadoutCoordinatePrecision::MinimumValue() const
{
   return 0;
}

double ReadoutCoordinatePrecision::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

ReadoutBroadcast::ReadoutBroadcast( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutBroadcastParameter = this;
}

IsoString ReadoutBroadcast::Id() const
{
   return "broadcast";
}

bool ReadoutBroadcast::DefaultValue() const
{
   return ReadoutOptions().IsBroadcastEnabled();
}

// ----------------------------------------------------------------------------

ReadoutReal::ReadoutReal( MetaProcess* p ) : MetaBoolean( p )
{
   TheReadoutRealParameter = this;
}

IsoString ReadoutReal::Id() const
{
   return "real";
}

bool ReadoutReal::DefaultValue() const
{
   return ReadoutOptions().IsReal();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsParameters.cpp - Released 2018-11-23T18:45:58Z
