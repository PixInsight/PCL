//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0205
// ----------------------------------------------------------------------------
// INDIDeviceControllerParameters.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#include "INDIDeviceControllerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IDCServerHostName*       TheIDCServerHostNameParameter = nullptr;
IDCServerPort*           TheIDCServerPortParameter = nullptr;
IDCServerConnect*        TheIDCServerConnectParameter = nullptr;
IDCServerCommand*        TheIDCServerCommandParameter = nullptr;
IDCAbort*                TheIDCAbortParameter = nullptr;
IDCVerbosity*            TheIDCVerbosityParameter = nullptr;
IDCGetCommandParameters* TheIDCGetCommandParametersParameter = nullptr;
IDCGetCommandResult*     TheIDCGetCommandResultParameter = nullptr;

IDCDevices*              TheIDCDevicesParameter = nullptr;
IDCDeviceName*           TheIDCDeviceNameParameter = nullptr;
IDCDeviceLabel*          TheIDCDeviceLabelParameter = nullptr;

IDCProperties*           TheIDCPropertiesParameter = nullptr;
IDCPropertyName*         TheIDCPropertyNameParameter = nullptr;
IDCPropertyValue*        TheIDCPropertyValueParameter = nullptr;
IDCPropertyState*        TheIDCPropertyStateParameter = nullptr;
IDCPropertyType*         TheIDCPropertyTypeParameter = nullptr;

IDCNewProperties*        TheIDCNewPropertiesParameter = nullptr;
IDCNewPropertyKey*       TheIDCNewPropertyKeyParameter = nullptr;
IDCNewPropertyType*      TheIDCNewPropertyTypeParameter = nullptr;
IDCNewPropertyValue*     TheIDCNewPropertyValueParameter = nullptr;

// ----------------------------------------------------------------------------

IDCServerHostName::IDCServerHostName( MetaProcess* P ) : MetaString( P )
{
   TheIDCServerHostNameParameter = this;
}

IsoString IDCServerHostName::Id() const
{
   return "serverHostName";
}

String IDCServerHostName::DefaultValue() const
{
   return "localhost";
}

// ----------------------------------------------------------------------------

IDCServerPort::IDCServerPort( MetaProcess* P ) : MetaUInt32( P )
{
   TheIDCServerPortParameter = this;
}

IsoString IDCServerPort::Id() const
{
   return "serverPort";
}

double IDCServerPort::DefaultValue() const
{
   return 7624;
}

double IDCServerPort::MinimumValue() const
{
   return 0;
}

double IDCServerPort::MaximumValue() const
{
   return 65535;
}

// ----------------------------------------------------------------------------

IDCServerConnect::IDCServerConnect( MetaProcess* P ) : MetaBoolean( P )
{
   TheIDCServerConnectParameter = this;
}

IsoString IDCServerConnect::Id() const
{
   return "serverConnect";
}

bool IDCServerConnect::DefaultValue() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCServerCommand::IDCServerCommand( MetaProcess* P ) : MetaString( P )
{
   TheIDCServerCommandParameter = this;
}

IsoString IDCServerCommand::Id() const
{
   return "serverCommand";
}

// ----------------------------------------------------------------------------

IDCAbort::IDCAbort( MetaProcess* P ) : MetaBoolean( P )
{
   TheIDCAbortParameter = this;
}

IsoString IDCAbort::Id() const
{
   return "abort";
}

// --------------------------------------------------------------------------

IDCVerbosity::IDCVerbosity( MetaProcess* P ) : MetaInt32( P )
{
   TheIDCVerbosityParameter = this;
}

IsoString IDCVerbosity::Id() const
{
   return "verbosity";
}

double IDCVerbosity::DefaultValue() const
{
   return 1; // 0=none 1=errors/notifications 2+=informative
}

// --------------------------------------------------------------------------

IDCGetCommandParameters::IDCGetCommandParameters( MetaProcess* P ) : MetaString( P )
{
   TheIDCGetCommandParametersParameter = this;
}

IsoString IDCGetCommandParameters::Id() const
{
   return "getCommandParameters";
}

// --------------------------------------------------------------------------

IDCGetCommandResult::IDCGetCommandResult( MetaProcess* P ) : MetaString( P )
{
   TheIDCGetCommandResultParameter = this;
}

IsoString IDCGetCommandResult::Id() const
{
   return "getCommandResult";
}

bool IDCGetCommandResult::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCDevices::IDCDevices( MetaProcess* P ) : MetaTable( P )
{
   TheIDCDevicesParameter = this;
}

IsoString IDCDevices::Id() const
{
   return "devices";
}

bool IDCDevices::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCDeviceName::IDCDeviceName( MetaTable* T ) : MetaString( T )
{
   TheIDCDeviceNameParameter = this;
}

IsoString IDCDeviceName::Id() const
{
   return "deviceName";
}

bool IDCDeviceName::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCDeviceLabel::IDCDeviceLabel( MetaTable* T ) : MetaString( T )
{
   TheIDCDeviceLabelParameter = this;
}

IsoString IDCDeviceLabel::Id() const
{
   return "deviceLabel";
}

bool IDCDeviceLabel::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCProperties::IDCProperties( MetaProcess* P ) : MetaTable( P )
{
   TheIDCPropertiesParameter = this;
}

IsoString IDCProperties::Id() const
{
   return "properties";
}

bool IDCProperties::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyName::IDCPropertyName( MetaTable* T ) : MetaString( T )
{
   TheIDCPropertyNameParameter = this;
}

IsoString IDCPropertyName::Id() const
{
   return "propertyName";
}

bool IDCPropertyName::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyValue::IDCPropertyValue( MetaTable* T ) : MetaString( T )
{
   TheIDCPropertyValueParameter = this;
}

IsoString IDCPropertyValue::Id() const
{
   return "propertyValue";
}

bool IDCPropertyValue::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyType::IDCPropertyType( MetaTable* T ) : MetaString( T )
{
   TheIDCPropertyTypeParameter = this;
}

IsoString IDCPropertyType::Id() const
{
   return "propertyType";
}

bool IDCPropertyType::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyState::IDCPropertyState( MetaTable* T ) : MetaUInt32( T )
{
   TheIDCPropertyStateParameter = this;
}

IsoString IDCPropertyState::Id() const
{
   return "propertyState";
}

double IDCPropertyState::DefaultValue() const
{
   return 0;
}

double IDCPropertyState::MinimumValue() const
{
   return 0;
}

double IDCPropertyState::MaximumValue() const
{
   return 3;
}

bool IDCPropertyState::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCNewProperties::IDCNewProperties( MetaProcess* P ): MetaTable( P )
{
   TheIDCNewPropertiesParameter = this;
}

IsoString IDCNewProperties::Id() const
{
   return "newProperties";
}

// --------------------------------------------------------------------------

IDCNewPropertyKey::IDCNewPropertyKey( MetaTable* T ) : MetaString( T )
{
   TheIDCNewPropertyKeyParameter = this;
}

IsoString IDCNewPropertyKey::Id() const
{
   return "newPropertyKey";
}

// --------------------------------------------------------------------------

IDCNewPropertyType::IDCNewPropertyType( MetaTable* T ) : MetaString( T )
{
   TheIDCNewPropertyTypeParameter = this;
}

IsoString IDCNewPropertyType::Id() const
{
   return "newPropertyType";
}

// --------------------------------------------------------------------------

IDCNewPropertyValue::IDCNewPropertyValue( MetaTable* T ) : MetaString( T )
{
   TheIDCNewPropertyValueParameter = this;
}

IsoString IDCNewPropertyValue::Id() const
{
   return "newPropertyValue";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerParameters.cpp - Released 2017-07-09T18:07:33Z
