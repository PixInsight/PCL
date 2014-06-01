// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIParameters.h - Released 2013/03/24 18:42:27 UTC
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

#ifndef __PixInsightINDIParameters_h
#define __PixInsightINDIParameters_h

#include <pcl/MetaParameter.h>
#include "PixInsightINDIParamListTypes.h"
namespace pcl
{

PCL_BEGIN_LOCAL





// ----------------------------------------------------------------------------
// The hostname of the INDI server 

class INDIServerHostname: public MetaString
{
public:

   INDIServerHostname( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
};

extern INDIServerHostname* TheINDIServerHostname;

class INDIServerConnect: public MetaUInt32
{
public:

   INDIServerConnect( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern INDIServerConnect* TheINDIServerConnect;

class INDIServerCommand: public MetaString
{
public:

   INDIServerCommand( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;

};

extern INDIServerCommand* TheINDIServerCommand;

class INDIServerGetPropertyReturnValue: public MetaString
{
public:

	INDIServerGetPropertyReturnValue( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual bool IsReadOnly();
};

extern INDIServerGetPropertyReturnValue* TheINDIServerGetPropertyReturnValue;

class INDIServerGetPropertyPropertyParam: public MetaString
{
public:

	INDIServerGetPropertyPropertyParam( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
};

extern INDIServerGetPropertyPropertyParam* TheINDIServerGetPropertyPropertyParam;


// ----------------------------------------------------------------------------
// The port of the INDI server
class INDIServerPort: public MetaUInt32
{
public:

   INDIServerPort( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern INDIServerPort* TheINDIServerPort;

class INDIProcessFlagDoAbort: public MetaBoolean
{
public:
	INDIProcessFlagDoAbort(MetaProcess*);
	virtual IsoString Id() const;
};

extern INDIProcessFlagDoAbort* TheINDIProcessFlagDoAbort;


class INDIDevices: public MetaTable
{
public:
	INDIDevices ( MetaProcess* );
	virtual IsoString Id() const;
};
extern INDIDevices* TheINDIDevicesParameter ;

class INDIDeviceName: public MetaString
{
public:
	INDIDeviceName ( MetaTable* );
	virtual IsoString Id() const;
};
extern INDIDeviceName* TheINDIDeviceNameParameter ;


class INDIProperties: public MetaTable
{
public:
	INDIProperties ( MetaProcess* );
	virtual IsoString Id() const;
};
extern INDIProperties* TheINDIPropertiesParameter ;

class INDIPropertyName: public MetaString
{
public:
	INDIPropertyName ( MetaTable* );
	virtual IsoString Id() const;
};
extern INDIPropertyName* TheINDIPropertyNameParameter ;

class INDIPropertyValue: public MetaString
{
public:
	INDIPropertyValue ( MetaTable* );
	virtual IsoString Id() const;
};
extern INDIPropertyValue* TheINDIPropertyValueParameter ;

class INDIPropertyType: public MetaString
{
public:
	INDIPropertyType ( MetaTable* );
	virtual IsoString Id() const;
};
extern INDIPropertyType* TheINDIPropertyTypeParameter ;

class INDIPropertyState: public MetaUInt32
{
public:

   INDIPropertyState( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};
extern INDIPropertyState* TheINDIPropertyStateParameter ;


class INDINewProperties: public MetaTable
{
public:
	INDINewProperties ( MetaProcess* );
	virtual IsoString Id() const;
};
extern INDINewProperties* TheINDINewPropertiesParameter ;

class INDINewPropertyKey: public MetaString
{
public:
	INDINewPropertyKey ( MetaTable* );
	virtual IsoString Id() const;
};
extern INDINewPropertyKey* TheINDINewPropertyKeyParameter ;

class INDINewPropertyType: public MetaString
{
public:
	INDINewPropertyType ( MetaTable* );
	virtual IsoString Id() const;
};
extern INDINewPropertyType* TheINDINewPropertyTypeParameter ;


class INDINewPropertyValue: public MetaString
{
public:
	INDINewPropertyValue ( MetaTable* );
	virtual IsoString Id() const;
};
extern INDINewPropertyValue* TheINDINewPropertyValueParameter ;




// ----------------------------------------------------------------------------





PCL_END_LOCAL

} // pcl

#endif   // __PixInsightINDIParameters_h

// ****************************************************************************
// EOF PixInsightINDIParameters.h - Released 2013/03/24 18:42:27 UTC
