// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIParameters.cpp - Released 2013/03/24 18:42:27 UTC
// ****************************************************************************
// This file is part of the standard PixInsightINDI PixInsight module.
//
// Copyright (c) 2003-2013, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "PixInsightINDIParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

INDIServerHostname*			TheINDIServerHostname =0;
INDIServerPort*				TheINDIServerPort =0;
INDIServerConnect*			TheINDIServerConnect =0;
INDIProperties*				TheINDIPropertiesParameter = 0;
INDIPropertyName*			TheINDIPropertyNameParameter = 0;
INDIPropertyValue*			TheINDIPropertyValueParameter = 0;
INDIPropertyState*          TheINDIPropertyStateParameter = 0 ;
INDINewProperties*			TheINDINewPropertiesParameter = 0;
INDINewPropertyKey*         TheINDINewPropertyKeyParameter = 0;
INDINewPropertyType*		TheINDINewPropertyTypeParameter = 0;
INDINewPropertyValue*		TheINDINewPropertyValueParameter = 0;
// ----------------------------------------------------------------------------
// The INDI server hostname

INDIServerHostname::INDIServerHostname( MetaProcess* P ) : MetaString( P )
{
   TheINDIServerHostname = this;
}

IsoString INDIServerHostname::Id() const
{
   return "INDI_ServerHostname";
}

size_type INDIServerHostname::MinLength() const
{
   return 0;
}

// ----------------------------------------------------------------------------
// The INDI server port

INDIServerPort::INDIServerPort( MetaProcess* P ) : MetaUInt32( P )
{
   TheINDIServerPort = this;
}

IsoString INDIServerPort::Id() const
{
   return "INDI_ServerPort";
}

double INDIServerPort::DefaultValue() const
{
   return 0;
}


double INDIServerPort::MinimumValue() const
{
   return 0;
}

double INDIServerPort::MaximumValue() const
{
   return 65535;
}

INDIServerConnect::INDIServerConnect( MetaProcess* P ) : MetaUInt32( P )
{
   TheINDIServerConnect = this;
}

IsoString INDIServerConnect::Id() const
{
   return "INDI_ServerConnect";
}

double INDIServerConnect::DefaultValue() const
{
   return 1;
}


double INDIServerConnect::MinimumValue() const
{
   return 0;
}

double INDIServerConnect::MaximumValue() const
{
   return 1;
}

INDIProperties::INDIProperties ( MetaProcess* P): MetaTable(P)
{
	TheINDIPropertiesParameter = this;
}

IsoString INDIProperties::Id() const
{
   return "INDI_Properties";
}


INDIPropertyName::INDIPropertyName(MetaTable* T):MetaString(T){
	TheINDIPropertyNameParameter = this;
}

IsoString INDIPropertyName::Id() const
{
   return "INDI_PropertyName";
}

INDIPropertyValue::INDIPropertyValue(MetaTable* T):MetaString(T){
	TheINDIPropertyValueParameter = this;
}

IsoString INDIPropertyValue::Id() const
{
   return "INDI_PropertyValue";
}

INDIPropertyState::INDIPropertyState( MetaTable* P ) : MetaUInt32( P )
{
   TheINDIPropertyStateParameter = this;
}

IsoString INDIPropertyState::Id() const
{
   return "INDI_PropertyState";
}

double INDIPropertyState::DefaultValue() const
{
   return 0;
}


double INDIPropertyState::MinimumValue() const
{
   return 0;
}

double INDIPropertyState::MaximumValue() const
{
   return 3;
}

INDINewProperties::INDINewProperties ( MetaProcess* P): MetaTable(P)
{
	TheINDINewPropertiesParameter = this;
}

IsoString INDINewProperties::Id() const
{
   return "INDI_NewProperties";
}

INDINewPropertyKey::INDINewPropertyKey(MetaTable* T):MetaString(T){
	TheINDINewPropertyKeyParameter = this;
}

IsoString INDINewPropertyKey::Id() const
{
   return "INDI_NewPropertyKey";
}

INDINewPropertyType::INDINewPropertyType(MetaTable* T):MetaString(T){
	TheINDINewPropertyTypeParameter = this;
}

IsoString INDINewPropertyType::Id() const
{
   return "INDI_NewPropertyType";
}

INDINewPropertyValue::INDINewPropertyValue(MetaTable* T):MetaString(T){
	TheINDINewPropertyValueParameter = this;
}

IsoString INDINewPropertyValue::Id() const
{
   return "INDI_NewPropertyValue";
}



// ----------------------------------------------------------------------------

bool INDINewPropertyListItem::operator==(const INDINewPropertyListItem& rhs) const{
	return (this->Device == rhs.Device) &&
		(this->Element == rhs.Element) &&
		(this->Property == rhs.Property) &&
		(this->PropertyKey == rhs.PropertyKey) &&
		(this->PropertyType == rhs.PropertyType) &&
		(this->NewPropertyValue == rhs.NewPropertyValue);
}

} // pcl

// ****************************************************************************
// EOF PixInsightINDIParameters.cpp - Released 2013/03/24 18:42:27 UTC
