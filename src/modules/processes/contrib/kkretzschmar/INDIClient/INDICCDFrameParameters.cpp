//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0228
// ----------------------------------------------------------------------------
// INDICCDFrameParameters.cpp - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

ICFDeviceName*              TheICFDeviceNameParameter = nullptr;
ICFUploadMode*              TheICFUploadModeParameter = nullptr;
ICFServerUploadDirectory*   TheICFServerUploadDirectoryParameter = nullptr;
ICFServerFileNameTemplate*  TheICFServerFileNameTemplateParameter = nullptr;
ICFFrameType*               TheICFFrameTypeParameter = nullptr;
ICFBinningX*                TheICFBinningXParameter = nullptr;
ICFBinningY*                TheICFBinningYParameter = nullptr;
ICFFilterSlot*              TheICFFilterSlotParameter = nullptr;
ICFExposureTime*            TheICFExposureTimeParameter = nullptr;
ICFExposureDelay*           TheICFExposureDelayParameter = nullptr;
ICFExposureCount*           TheICFExposureCountParameter = nullptr;
ICFOpenClientImages*        TheICFOpenClientImagesParameter = nullptr;
ICFNewImageIdTemplate*      TheICFNewImageIdTemplateParameter = nullptr;
ICFReuseImageWindow*        TheICFReuseImageWindowParameter = nullptr;
ICFAutoStretch*             TheICFAutoStretchParameter = nullptr;
ICFLinkedAutoStretch*       TheICFLinkedAutoStretchParameter = nullptr;
ICFSaveClientImages*        TheICFSaveClientImagesParameter = nullptr;
ICFOverwriteClientImages*   TheICFOverwriteClientImagesParameter = nullptr;
ICFClientDownloadDirectory* TheICFClientDownloadDirectoryParameter = nullptr;
ICFClientFileNameTemplate*  TheICFClientFileNameTemplateParameter = nullptr;
ICFClientOutputFormatHints* TheICFClientOutputFormatHintsParameter = nullptr;
ICFObjectName*              TheICFObjectNameParameter = nullptr;
ICFTelescopeSelection*  	TheICFTelescopeSelectionParameter = nullptr;
ICFRequireSelectedTelescope* TheICFRequireSelectedTelescopeParameter = nullptr;
ICFTelescopeDeviceName*     TheICFTelescopeDeviceNameParameter = nullptr;
ICFClientFrames*            TheICFClientFramesParameter = nullptr;
ICFClientViewId*            TheICFClientViewIdParameter = nullptr;
ICFClientFilePath*          TheICFClientFilePathParameter = nullptr;
ICFServerFrames*            TheICFServerFramesParameter = nullptr;
ICFServerFrame*             TheICFServerFrameParameter = nullptr;
REGISTER_MODULE_PARAMETER(ICFExternalFilterWheelDeviceName);
REGISTER_MODULE_PARAMETER(ICFEnableAlignmentCorrection);
REGISTER_MODULE_PARAMETER(ICFAlignmentFile);


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
   return 0; // 0 -> no change
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
   return 0; // 0 -> no change
}

double ICFBinningY::MaximumValue() const
{
   return 64;
}

// ----------------------------------------------------------------------------

ICFFilterSlot::ICFFilterSlot( MetaProcess* P ) : MetaInt32( P )
{
   TheICFFilterSlotParameter = this;
}

IsoString ICFFilterSlot::Id() const
{
   return "filterSlot";
}

double ICFFilterSlot::DefaultValue() const
{
   return 1;
}

double ICFFilterSlot::MinimumValue() const
{
   return 0; // 0 -> no change
}

double ICFFilterSlot::MaximumValue() const
{
   return 256;
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

ICFOpenClientImages::ICFOpenClientImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheICFOpenClientImagesParameter = this;
}

IsoString ICFOpenClientImages::Id() const
{
   return "openClientImages";
}

bool ICFOpenClientImages::DefaultValue() const
{
   return true;
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

ICFSaveClientImages::ICFSaveClientImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheICFSaveClientImagesParameter = this;
}

IsoString ICFSaveClientImages::Id() const
{
   return "saveClientImages";
}

bool ICFSaveClientImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICFOverwriteClientImages::ICFOverwriteClientImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheICFOverwriteClientImagesParameter = this;
}

IsoString ICFOverwriteClientImages::Id() const
{
   return "overwriteClientImages";
}

bool ICFOverwriteClientImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICFClientDownloadDirectory::ICFClientDownloadDirectory( MetaProcess* P ) : MetaString( P )
{
   TheICFClientDownloadDirectoryParameter = this;
}

IsoString ICFClientDownloadDirectory::Id() const
{
   return "clientDownloadDirectory";
}

// ----------------------------------------------------------------------------

ICFClientFileNameTemplate::ICFClientFileNameTemplate( MetaProcess* P ) : MetaString( P )
{
   TheICFClientFileNameTemplateParameter = this;
}

IsoString ICFClientFileNameTemplate::Id() const
{
   return "clientFileNameTemplate";
}

String ICFClientFileNameTemplate::DefaultValue() const
{
   return "%f_B%b_E%e_%n";
}

// ----------------------------------------------------------------------------

ICFClientOutputFormatHints::ICFClientOutputFormatHints( MetaProcess* P ) : MetaString( P )
{
   TheICFClientOutputFormatHintsParameter = this;
}

IsoString ICFClientOutputFormatHints::Id() const
{
   return "clientOutputFormatHints";
}

String ICFClientOutputFormatHints::DefaultValue() const
{
   return "compression-codec zlib+sh";
}

// ----------------------------------------------------------------------------

ICFObjectName::ICFObjectName( MetaProcess* P ) : MetaString( P )
{
   TheICFObjectNameParameter = this;
}

IsoString ICFObjectName::Id() const
{
   return "objectName";
}

String ICFObjectName::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

ICFTelescopeSelection::ICFTelescopeSelection( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICFTelescopeSelectionParameter = this;
}

IsoString ICFTelescopeSelection::Id() const
{
   return "telescopeSelection";
}

size_type ICFTelescopeSelection::NumberOfElements() const
{
   return NumberOfTelescopeSelections;
}

IsoString ICFTelescopeSelection::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case NoTelescope:                      return "TelescopeSelection_NoTelescope";
   case ActiveTelescope:                  return "TelescopeSelection_ActiveTelescope";
   case MountControllerTelescope:         return "TelescopeSelection_MountController";
   case MountControllerOrActiveTelescope: return "TelescopeSelection_ActiveOrMountController";
   case TelescopeDeviceName:              return "TelescopeSelection_DeviceName";
   }
}

int ICFTelescopeSelection::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICFTelescopeSelection::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICFRequireSelectedTelescope::ICFRequireSelectedTelescope( MetaProcess* P ) : MetaBoolean( P )
{
   TheICFRequireSelectedTelescopeParameter = this;
}

IsoString ICFRequireSelectedTelescope::Id() const
{
   return "requireSelectedTelescope";
}

bool ICFRequireSelectedTelescope::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICFTelescopeDeviceName::ICFTelescopeDeviceName( MetaProcess* P ) : MetaString( P )
{
   TheICFTelescopeDeviceNameParameter = this;
}

IsoString ICFTelescopeDeviceName::Id() const
{
   return "telescopeDeviceName";
}

String ICFTelescopeDeviceName::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ICFClientFrames::ICFClientFrames( MetaProcess* P ) : MetaTable( P )
{
   TheICFClientFramesParameter = this;
}

IsoString ICFClientFrames::Id() const
{
   return "clientFrames";
}

bool ICFClientFrames::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICFClientViewId::ICFClientViewId( MetaTable* T ) : MetaString( T )
{
   TheICFClientViewIdParameter = this;
}

IsoString ICFClientViewId::Id() const
{
   return "viewId";
}

bool ICFClientViewId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICFClientFilePath::ICFClientFilePath( MetaTable* T ) : MetaString( T )
{
   TheICFClientFilePathParameter = this;
}

IsoString ICFClientFilePath::Id() const
{
   return "filePath";
}

bool ICFClientFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICFServerFrames::ICFServerFrames( MetaProcess* P ) : MetaTable( P )
{
   TheICFServerFramesParameter = this;
}

IsoString ICFServerFrames::Id() const
{
   return "serverFrames";
}

bool ICFServerFrames::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICFServerFrame::ICFServerFrame( MetaTable* T ) : MetaString( T )
{
   TheICFServerFrameParameter = this;
}

IsoString ICFServerFrame::Id() const
{
   return "fileName";
}

bool ICFServerFrame::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DEFINE_STRING_PARAMETER_CLASS(ICFExternalFilterWheelDeviceName,"externalFilterWheelDeviceName");

// ----------------------------------------------------------------------------

DEFINE_BOOLEAN_PARAMETER_CLASS(ICFEnableAlignmentCorrection,"enableAlignmentCorrection",false);
DEFINE_STRING_PARAMETER_CLASS(ICFAlignmentFile,"alignmentModelFile");


// ----------------------------------------------------------------------------



} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameParameters.cpp - Released 2018-11-23T18:45:59Z
