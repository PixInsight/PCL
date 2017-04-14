//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0256
// ----------------------------------------------------------------------------
// StarGeneratorParameters.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#include "StarGeneratorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SGStarDatabasePath*   TheSGStarDatabasePathParameter = 0;
SGCenterRA*           TheSGCenterRAParameter = 0;
SGCenterDec*          TheSGCenterDecParameter = 0;
SGEpoch*              TheSGEpochParameter = 0;
SGProjectionSystem*   TheSGProjectionSystemParameter = 0;
SGFocalLength*        TheSGFocalLengthParameter = 0;
SGPixelSize*          TheSGPixelSizeParameter = 0;
SGSensorWidth*        TheSGSensorWidthParameter = 0;
SGSensorHeight*       TheSGSensorHeightParameter = 0;
SGLimitMagnitude*     TheSGLimitMagnitudeParameter = 0;
SGStarFWHM*           TheSGStarFWHMParameter = 0;
SGNonlinear*          TheSGNonlinearParameter = 0;
SGTargetMinimumValue* TheSGTargetMinimumValueParameter = 0;
SGOutputMode*         TheSGOutputModeParameter = 0;
SGOutputFilePath*     TheSGOutputFilePathParameter = 0;

// ----------------------------------------------------------------------------

SGStarDatabasePath::SGStarDatabasePath( MetaProcess* P ) : MetaString( P )
{
   TheSGStarDatabasePathParameter = this;
}

IsoString SGStarDatabasePath::Id() const
{
   return "starDatabasePath";
}

// ----------------------------------------------------------------------------

SGCenterRA::SGCenterRA( MetaProcess* P ) : MetaDouble( P )
{
   TheSGCenterRAParameter = this;
}

IsoString SGCenterRA::Id() const
{
   return "centerRA";
}

int SGCenterRA::Precision() const
{
   return 7; // 1 mas
}

double SGCenterRA::DefaultValue() const
{
   return 0;
}

double SGCenterRA::MinimumValue() const
{
   return 0;
}

double SGCenterRA::MaximumValue() const
{
   return 360;
}

// ----------------------------------------------------------------------------

SGCenterDec::SGCenterDec( MetaProcess* P ) : MetaDouble( P )
{
   TheSGCenterDecParameter = this;
}

IsoString SGCenterDec::Id() const
{
   return "centerDec";
}

int SGCenterDec::Precision() const
{
   return 7; // 1 mas
}

double SGCenterDec::DefaultValue() const
{
   return 0;
}

double SGCenterDec::MinimumValue() const
{
   return -90;
}

double SGCenterDec::MaximumValue() const
{
   return +90;
}

// ----------------------------------------------------------------------------

SGEpoch::SGEpoch( MetaProcess* P ) : MetaDouble( P )
{
   TheSGEpochParameter = this;
}

IsoString SGEpoch::Id() const
{
   return "epoch";
}

int SGEpoch::Precision() const
{
   return 1;
}

double SGEpoch::DefaultValue() const
{
   return 2451545.0; // J2000.0
}

double SGEpoch::MinimumValue() const
{
   return 0;
}

double SGEpoch::MaximumValue() const
{
   return int_max;
}

// ----------------------------------------------------------------------------

SGProjectionSystem::SGProjectionSystem( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSGProjectionSystemParameter = this;
}

IsoString SGProjectionSystem::Id() const
{
   return "projectionSystem";
}

size_type SGProjectionSystem::NumberOfElements() const
{
   return NumberOfProjectionSystems;
}

IsoString SGProjectionSystem::ElementId( size_type idx ) const
{
   switch ( idx )
   {
   default:
   case Conformal: return "Conformal";
   case Gnomonic:  return "Gnomonic";
   }
}

int SGProjectionSystem::ElementValue( size_type idx ) const
{
   return int( idx );
}

size_type SGProjectionSystem::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

SGFocalLength::SGFocalLength( MetaProcess* P ) : MetaFloat( P )
{
   TheSGFocalLengthParameter = this;
}

IsoString SGFocalLength::Id() const
{
   return "focalLength";
}

int SGFocalLength::Precision() const
{
   return 2;
}

double SGFocalLength::DefaultValue() const
{
   return 500;
}

double SGFocalLength::MinimumValue() const
{
   return 5;
}

double SGFocalLength::MaximumValue() const
{
   return int_max;
}

// ----------------------------------------------------------------------------

SGPixelSize::SGPixelSize( MetaProcess* P ) : MetaFloat( P )
{
   TheSGPixelSizeParameter = this;
}

IsoString SGPixelSize::Id() const
{
   return "pixelSize";
}

int SGPixelSize::Precision() const
{
   return 2;
}

double SGPixelSize::DefaultValue() const
{
   return 9;
}

double SGPixelSize::MinimumValue() const
{
   return 0.10;
}

double SGPixelSize::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

SGSensorWidth::SGSensorWidth( MetaProcess* P ) : MetaInt32( P )
{
   TheSGSensorWidthParameter = this;
}

IsoString SGSensorWidth::Id() const
{
   return "sensorWidth";
}

double SGSensorWidth::DefaultValue() const
{
   return 2048;
}

double SGSensorWidth::MinimumValue() const
{
   return 256;
}

double SGSensorWidth::MaximumValue() const
{
   return int_max;
}

// ----------------------------------------------------------------------------

SGSensorHeight::SGSensorHeight( MetaProcess* P ) : MetaInt32( P )
{
   TheSGSensorHeightParameter = this;
}

IsoString SGSensorHeight::Id() const
{
   return "sensorHeight";
}

double SGSensorHeight::DefaultValue() const
{
   return 2048;
}

double SGSensorHeight::MinimumValue() const
{
   return 256;
}

double SGSensorHeight::MaximumValue() const
{
   return int_max;
}

// ----------------------------------------------------------------------------

SGLimitMagnitude::SGLimitMagnitude( MetaProcess* P ) : MetaFloat( P )
{
   TheSGLimitMagnitudeParameter = this;
}

IsoString SGLimitMagnitude::Id() const
{
   return "limitMagnitude";
}

int SGLimitMagnitude::Precision() const
{
   return 2;
}

double SGLimitMagnitude::DefaultValue() const
{
   return 12;
}

double SGLimitMagnitude::MinimumValue() const
{
   return -2;
}

double SGLimitMagnitude::MaximumValue() const
{
   return 30;
}

// ----------------------------------------------------------------------------

SGStarFWHM::SGStarFWHM( MetaProcess* P ) : MetaFloat( P )
{
   TheSGStarFWHMParameter = this;
}

IsoString SGStarFWHM::Id() const
{
   return "starFWHM";
}

int SGStarFWHM::Precision() const
{
   return 2;
}

double SGStarFWHM::DefaultValue() const
{
   return 10;
}

double SGStarFWHM::MinimumValue() const
{
   return 0.01;
}

double SGStarFWHM::MaximumValue() const
{
   return 1800; // 30 arcminutes
}

// ----------------------------------------------------------------------------

SGNonlinear::SGNonlinear( MetaProcess* P ) : MetaBoolean( P )
{
   TheSGNonlinearParameter = this;
}

IsoString SGNonlinear::Id() const
{
   return "nonlinear";
}

bool SGNonlinear::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SGTargetMinimumValue::SGTargetMinimumValue( MetaProcess* P ) : MetaFloat( P )
{
   TheSGTargetMinimumValueParameter = this;
}

IsoString SGTargetMinimumValue::Id() const
{
   return "targetMinimumValue";
}

int SGTargetMinimumValue::Precision() const
{
   return 6;
}

double SGTargetMinimumValue::DefaultValue() const
{
   return 0.05;
}

double SGTargetMinimumValue::MinimumValue() const
{
   return 0.000001;
}

double SGTargetMinimumValue::MaximumValue() const
{
   return 0.25;
}

// ----------------------------------------------------------------------------

SGOutputMode::SGOutputMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSGOutputModeParameter = this;
}

IsoString SGOutputMode::Id() const
{
   return "outputMode";
}

size_type SGOutputMode::NumberOfElements() const
{
   return NumberOfOutputModes;
}

IsoString SGOutputMode::ElementId( size_type idx ) const
{
   switch ( idx )
   {
   default:
   case Image:   return "Output_Image";
   case CSVFile: return "Output_CSVFile";
   }
}

int SGOutputMode::ElementValue( size_type idx ) const
{
   return int( idx );
}

size_type SGOutputMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

SGOutputFilePath::SGOutputFilePath( MetaProcess* P ) : MetaString( P )
{
   TheSGOutputFilePathParameter = this;
}

IsoString SGOutputFilePath::Id() const
{
   return "outputFilePath";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarGeneratorParameters.cpp - Released 2017-04-14T23:07:12Z
