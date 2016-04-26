//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.04.0108
// ----------------------------------------------------------------------------
// INDICCDFrameParameters.cpp - Released 2016/04/15 15:37:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#include "INDICCDFrameParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ICFDeviceName*             TheICFDeviceNameParameter = nullptr;
ICFUploadMode*             TheICFUploadModeParameter = nullptr;
ICFServerUploadDirectory*  TheICFServerUploadDirectoryParameter = nullptr;
ICFServerFileNameTemplate* TheICFServerFileNameTemplateParameter = nullptr;
ICFFrameType*              TheICFFrameTypeParameter = nullptr;
ICFBinningX*               TheICFBinningXParameter = nullptr;
ICFBinningY*               TheICFBinningYParameter = nullptr;
ICFExposureTime*           TheICFExposureTimeParameter = nullptr;
ICFExposureDelay*          TheICFExposureDelayParameter = nullptr;
ICFExposureCount*          TheICFExposureCountParameter = nullptr;
ICFNewImageIdTemplate*     TheICFNewImageIdTemplateParameter = nullptr;
ICFReuseImageWindow*       TheICFReuseImageWindowParameter = nullptr;
ICFAutoStretch*            TheICFAutoStretchParameter = nullptr;
ICFLinkedAutoStretch*      TheICFLinkedAutoStretchParameter = nullptr;

// ----------------------------------------------------------------------------

ICFDeviceName::ICFDeviceName( MetaProcess* P ) : MetaString( P )
{
   TheICFDeviceNameParameter = this;
}

IsoString ICFDeviceName::Id() const
{
   return "deviceName";
}

// ----------------------------------------------------------------------------

ICFUploadMode::ICFUploadMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICFUploadModeParameter = this;
}

IsoString ICFUploadMode::Id() const
{
   return "uploadMode";
}

size_type ICFUploadMode::NumberOfElements() const
{
   return NumberOfUploadModes;
}

IsoString ICFUploadMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case UploadClient:          return "UploadMode_Client";
   case UploadServer:          return "UploadMode_Server";
   case UploadServerAndClient: return "UploadMode_ServerAndClient";
   }
}

int ICFUploadMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICFUploadMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICFServerUploadDirectory::ICFServerUploadDirectory( MetaProcess* P ) : MetaString( P )
{
   TheICFServerUploadDirectoryParameter = this;
}

IsoString ICFServerUploadDirectory::Id() const
{
   return "serverUploadDirectory";
}

// ----------------------------------------------------------------------------

ICFServerFileNameTemplate::ICFServerFileNameTemplate( MetaProcess* P ) : MetaString( P )
{
   TheICFServerFileNameTemplateParameter = this;
}

IsoString ICFServerFileNameTemplate::Id() const
{
   return "serverFileNameTemplate";
}

String ICFServerFileNameTemplate::DefaultValue() const
{
   return "%f_B%b_E%e_%n";
}

// ----------------------------------------------------------------------------

ICFFrameType::ICFFrameType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICFFrameTypeParameter = this;
}

IsoString ICFFrameType::Id() const
{
   return "frameType";
}

size_type ICFFrameType::NumberOfElements() const
{
   return NumberOfFrameTypes;
}

IsoString ICFFrameType::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case LightFrame: return "FrameType_Light";
   case BiasFrame:  return "FrameType_Bias";
   case DarkFrame:  return "FrameType_Dark";
   case FlatFrame:  return "FrameType_Flat";
   }
}

int ICFFrameType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICFFrameType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICFBinningX::ICFBinningX( MetaProcess* P ) : MetaInt32( P )
{
   TheICFBinningXParameter = this;
}

IsoString ICFBinningX::Id() const
{
   return "binningX";
}

double ICFBinningX::DefaultValue() const
{
   return 1;
}

double ICFBinningX::MinimumValue() const
{
   return 1;
}

double ICFBinningX::MaximumValue() const
{
   return 64;
}

// ----------------------------------------------------------------------------

ICFBinningY::ICFBinningY( MetaProcess* P ) : MetaInt32( P )
{
   TheICFBinningYParameter = this;
}

IsoString ICFBinningY::Id() const
{
   return "binningY";
}

double ICFBinningY::DefaultValue() const
{
   return 1;
}

double ICFBinningY::MinimumValue() const
{
   return 1;
}

double ICFBinningY::MaximumValue() const
{
   return 64;
}

// ----------------------------------------------------------------------------

ICFExposureTime::ICFExposureTime( MetaProcess* P ) : MetaDouble( P )
{
   TheICFExposureTimeParameter = this;
}

IsoString ICFExposureTime::Id() const
{
   return "exposureTime";
}

int ICFExposureTime::Precision() const
{
   return 3;
}

double ICFExposureTime::DefaultValue() const
{
   return 1;
}

double ICFExposureTime::MinimumValue() const
{
   return 0.001;
}

double ICFExposureTime::MaximumValue() const
{
   return 60000;
}

// ----------------------------------------------------------------------------

ICFExposureDelay::ICFExposureDelay( MetaProcess* P ) : MetaDouble( P )
{
   TheICFExposureDelayParameter = this;
}

IsoString ICFExposureDelay::Id() const
{
   return "exposureDelay";
}

int ICFExposureDelay::Precision() const
{
   return 3;
}

double ICFExposureDelay::DefaultValue() const
{
   return 0;
}

double ICFExposureDelay::MinimumValue() const
{
   return 0;
}

double ICFExposureDelay::MaximumValue() const
{
   return 600;
}

// ----------------------------------------------------------------------------

ICFExposureCount::ICFExposureCount( MetaProcess* P ) : MetaInt32( P )
{
   TheICFExposureCountParameter = this;
}

IsoString ICFExposureCount::Id() const
{
   return "exposureCount";
}

double ICFExposureCount::DefaultValue() const
{
   return 1;
}

double ICFExposureCount::MinimumValue() const
{
   return 1;
}

double ICFExposureCount::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

ICFNewImageIdTemplate::ICFNewImageIdTemplate( MetaProcess* P ) : MetaString( P )
{
   TheICFNewImageIdTemplateParameter = this;
}

IsoString ICFNewImageIdTemplate::Id() const
{
   return "newImageIdTemplate";
}

String ICFNewImageIdTemplate::DefaultValue() const
{
   return "INDICCDFrame";
}

// ----------------------------------------------------------------------------

ICFReuseImageWindow::ICFReuseImageWindow( MetaProcess* P ) : MetaBoolean( P )
{
   TheICFReuseImageWindowParameter = this;
}

IsoString ICFReuseImageWindow::Id() const
{
   return "reuseImageWindow";
}

bool ICFReuseImageWindow::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICFAutoStretch::ICFAutoStretch( MetaProcess* P ) : MetaBoolean( P )
{
   TheICFAutoStretchParameter = this;
}

IsoString ICFAutoStretch::Id() const
{
   return "autoStretch";
}

bool ICFAutoStretch::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICFLinkedAutoStretch::ICFLinkedAutoStretch( MetaProcess* P ) : MetaBoolean( P )
{
   TheICFLinkedAutoStretchParameter = this;
}

IsoString ICFLinkedAutoStretch::Id() const
{
   return "linkedAutoStretch";
}

bool ICFLinkedAutoStretch::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameParameters.cpp - Released 2016/04/15 15:37:39 UTC
