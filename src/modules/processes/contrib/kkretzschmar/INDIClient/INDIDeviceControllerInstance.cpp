//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// INDIDeviceControllerInstance.cpp - Released 2015/10/13 15:55:45 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2015 Klaus Kretzschmar
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

#include "INDIClient.h"
#include "INDIDeviceControllerInstance.h"
#include "INDIParameters.h"
#include "PropertyNode.h"

#include "INDI/basedevice.h"
#include "INDI/indicom.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/Mutex.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

#ifdef __PCL_LINUX
#include <memory>
#endif

namespace pcl
{

/* Our client auto pointer */
auto_ptr<INDIClient> indiClient(0);

// ----------------------------------------------------------------------------

INDIDeviceControllerInstance::INDIDeviceControllerInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_deviceList(),
p_propertyList(),
p_newPropertyList(),
p_host( TheINDIServerHostname->DefaultValue() ),
p_port( uint32( TheINDIServerPort->DefaultValue() ) ),
p_connect( uint32( TheINDIServerConnect->DefaultValue() ) ),
p_currentMessage(),
p_command(),
p_getPropertyReturnValue(),
p_getPropertyParam(),
p_doAbort(false),
m_internalAbortFlag(false),
m_ImageDownloaded(false)

{

}

INDIDeviceControllerInstance::INDIDeviceControllerInstance( const INDIDeviceControllerInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void INDIDeviceControllerInstance::Assign( const ProcessImplementation& p )
{
   const INDIDeviceControllerInstance* x = dynamic_cast<const INDIDeviceControllerInstance*>( &p );
   if ( x != 0 )
   {
	  p_host  = x->p_host;
	  p_port  = x->p_port;
	  p_connect = x->p_connect;
	  p_propertyList = x->p_propertyList;
	  p_newPropertyList = x->p_newPropertyList;
	  p_deviceList = x->p_deviceList;

   }
}


class PixInsightINDIEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const INDIDeviceControllerInstance& instance )
   {
      /*
       * Your magic comes here...
       */
      Console().WriteLn( "<end><cbr>Ah, did I mention that I do just _nothing_ at all? :D" );
   }
};


bool INDIDeviceControllerInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "INDI client can only be executed in the global context.";
   return false;
}

bool INDIDeviceControllerInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}


bool INDIDeviceControllerInstance::getPropertyFromKeyString(INDINewPropertyListItem& propertyKey, const String& keyString){


	size_type start=0;
	size_type nextStart=keyString.FindFirst("/",start);
	size_type count=0;

	while (nextStart !=String::notFound){
		if (count==0){
			start=nextStart+1;
			count++;
			nextStart = keyString.FindFirst("/",start);
			continue;
		}
		if (count==1)
			propertyKey.Device = keyString.Substring(start,nextStart-start);
		else if (count==2)
			propertyKey.Property = keyString.Substring(start,nextStart-start);
		 else if (count==3)
			break;
		else
		  Console().WriteLn("Assertion in getPropertyFromKeyString: Should not be here");

		count++;
		start=nextStart+1;
		nextStart = keyString.FindFirst("/",start);
	}

	if (nextStart==String::notFound && count!=0)
		propertyKey.Element = keyString.Substring(start,keyString.Length()-start);
	else
		return false;

	return true;

}

bool INDIDeviceControllerInstance::sendNewPropertyVector(const NewPropertyListType& propVector,bool isAsynch){
	p_newPropertyList.Append(propVector);
	return sendNewProperty(isAsynch);
}

bool INDIDeviceControllerInstance::sendNewPropertyValue(const INDINewPropertyListItem& propItem,bool isAsynch){
	p_newPropertyList.Append(propItem);
	return sendNewProperty(isAsynch);
}

bool INDIDeviceControllerInstance::sendNewProperty(bool isAsynchCall) {
	//Precondition: NewPropertyList contains only elements of the same property
	String deviceStr;
	String propertyStr;
	String propertyTypeStr;
	INDI::BaseDevice* device = NULL;
	bool firstTime=true;
	INumberVectorProperty * numberVecProp=NULL;
    ITextVectorProperty * textVecProp=NULL;
    ISwitchVectorProperty * switchVecProp=NULL;

	if (p_newPropertyList.IsEmpty())
		return false; //Nothing to do
	Console().WriteLn("");
	Console().WriteLn("------------------------------------------------------------------------------");
	Console().WriteLn( "Sending new property value(s) to INDI server '" + String( p_host ) + ":" + String( p_port ) + "':" );

	for (pcl::Array<INDINewPropertyListItem>::iterator iter=p_newPropertyList.Begin(); iter!=p_newPropertyList.End(); ++iter){
		if (iter->NewPropertyValue.IsEmpty()) {
			Console().WriteLn(ERR_MSG("ERROR: Empty property value ... exiting."));
			return false;
		}
		// initialize
		if (firstTime){
			if (getPropertyFromKeyString(*iter,iter->PropertyKey) || (!iter->Device.IsEmpty() && !iter->Property.IsEmpty() && !iter->PropertyType.IsEmpty())){
				deviceStr=iter->Device;
				propertyStr = iter->Property;
				propertyTypeStr = iter->PropertyType;
				firstTime = false;
				// get device
            {
               IsoString s( deviceStr );
               device = indiClient.get()->getDevice( s.c_str() );
            }
				if (!device){
				  Console().WriteLn(ERR_MSG(String("ERROR: Device '" + String( deviceStr ) +  "' not found ... exiting.").c_str()));
				  return false;
				}
				// get property vector
				if (iter->PropertyType == "INDI_SWITCH"){
               {
                  IsoString s ( propertyStr );
                  switchVecProp = device->getSwitch( s.c_str() );
               }
					if (!switchVecProp){
					  Console().WriteLn( ERR_MSG(String("Could not get property '" + String( propertyStr ) + "' from server. "
                                                        "Please check that INDI device '" + String( deviceStr ) + "' is connected.").c_str()));
					  return false;
					}
				} else if (iter->PropertyType == "INDI_NUMBER"){
               {
                  IsoString s( propertyStr );
                  numberVecProp = device->getNumber( s.c_str() );
               }
					if (!numberVecProp){
					  Console().WriteLn(ERR_MSG(String("ERROR: Could not get property '" + String( propertyStr ) + "' from server."
                                                       "Please check that INDI device '" + String( deviceStr ) + "' is connected.").c_str()) );
					  return false;
					}
				} else if (iter->PropertyType == "INDI_TEXT"){
               {
                  IsoString s( propertyStr );
                  textVecProp = device->getText( s.c_str() );
               }
					if (!textVecProp){
					  Console().WriteLn(ERR_MSG(String("ERROR: Could not get property '" + String( propertyStr ) + "' from server. "
                                                       "Please check that INDI device '" + String( deviceStr ) + "' is connected.").c_str()) );
					  return false;
					}
				}
				else {
				  Console().WriteLn(ERR_MSG(String("ERROR: Property '" + String( propertyStr ) + "' not supported.").c_str()) );
				}
			}
			else{
			  Console().WriteLn(ERR_MSG(String("ERROR: Invalid property key '" + String( iter->PropertyKey ) + "' not supported. ").c_str()) );
			}
		}
		if (getPropertyFromKeyString(*iter,iter->PropertyKey) || (!iter->Device.IsEmpty() && !iter->Property.IsEmpty() && !iter->PropertyType.IsEmpty())){
			if (switchVecProp){
				// set new switch
            ISwitch* sp;
            {
               IsoString s( iter->Element );
               sp = IUFindSwitch( switchVecProp, s.c_str() );
            }
				if (!sp){
				  Console().WriteLn(ERR_MSG(String("ERROR: Could not find element '" + String( iter->Element ) + "' ... exiting.").c_str()) );
				  return false;
				}
				IUResetSwitch(switchVecProp);
				if (iter->NewPropertyValue == String("ON")){
					sp->s = ISS_ON;
				}
				else {
				    sp->s = ISS_OFF;
				}

			}
			else if (numberVecProp){
				// set new number value
            INumber* np;
            {
               IsoString s( iter->Element );
               np = IUFindNumber( numberVecProp, s.c_str() );
            }
				if (!np){
				  Console().WriteLn(ERR_MSG(String("ERROR: Could not find element '" + String( iter->Element ) + "' ... exiting.").c_str()) );
				  return false;
				}
				np->value = iter->NewPropertyValue.ToDouble();
			}
			else if (textVecProp){
				// set new text value
            IText* np;
            {
               IsoString s( iter->Element );
               np = IUFindText( textVecProp, s.c_str() );
            }
				if (!np){
					Console().WriteLn(ERR_MSG(String("ERROR: Could not find element '" + String( iter->Element ) + "' ... exiting.").c_str()) );
					return false;
				}
				{
               IsoString s( iter->NewPropertyValue );
               IUSaveText( np, s.c_str() );
            }
			}
			else {
			  Console().WriteLn(ERR_MSG(String("ERROR: Should not be here ... exiting.").c_str()) );
			  return false;
			}
		}
		else{
		  Console().WriteLn( "ERROR: Invalid property key '" + String( iter->PropertyKey ) + "' ... exiting." );
		  return false;
		}
		Console().WriteLn("Device  : '" + String( deviceStr ) + "'");
		Console().WriteLn("Property: '" + String( propertyStr ) + "'");
		Console().WriteLn("Element : '" + String( iter->Element ) + "'");
		Console().WriteLn("Value   : '" + String( iter->NewPropertyValue ) + "'");

	} // for
	Console().WriteLn("------------------------------------------------------------------------------");


	// send new properties to server and wait for response
	if (switchVecProp){
		indiClient.get()->sendNewSwitch(switchVecProp);
		// if synch mode wait until completed or abort
		while (switchVecProp->s!=IPS_OK && switchVecProp->s!=IPS_IDLE &&  !p_doAbort && !m_internalAbortFlag && !isAsynchCall){
			if (switchVecProp->s==IPS_ALERT){
				writeCurrentMessageToConsole();
				m_internalAbortFlag=false;
				return false;
			}
		}
	}
	else if (numberVecProp){
		indiClient.get()->sendNewNumber(numberVecProp);
		// if synch mode wait until completed or abort
		while (numberVecProp->s != IPS_OK && numberVecProp->s != IPS_IDLE && !p_doAbort && !m_internalAbortFlag && !isAsynchCall) {
			if (numberVecProp->s == IPS_ALERT) {
				writeCurrentMessageToConsole();
				m_internalAbortFlag = false;
				return false;
			}
		}
	}
	else if (textVecProp) {
		indiClient.get()->sendNewText(textVecProp);
		// if synch mode wait until completed or abort
		while (textVecProp->s != IPS_OK && textVecProp->s != IPS_IDLE && !p_doAbort && !m_internalAbortFlag && !isAsynchCall) {
			if (textVecProp->s == IPS_ALERT) {
				writeCurrentMessageToConsole();
				m_internalAbortFlag = false;
				return false;
			}
		}
	}
	else {
		Console().WriteLn(ERR_MSG(String("ERROR: Should not be here ... exiting.").c_str()) );
	}

	p_newPropertyList.Clear();

	if (m_internalAbortFlag) {
		m_internalAbortFlag=false;
	}

	return true;
}

 bool INDIDeviceControllerInstance::getINDIPropertyItem(String device, String property, String element,INDIPropertyListItem& result, bool formatted ){

	// get popertyList with exclusive access
	ExclPropertyList propertyList = getExclusivePropertyList();

	for (pcl::Array<INDIPropertyListItem>::iterator iter=propertyList.get()->Begin(); iter!=propertyList.get()->End(); ++iter){

		if ((iter->Device==device) && (iter->Property==property) && (iter->Element==element) ){
			result.Device=device;
			result.Property=property;
			result.Element=element;
			if (iter->PropertyTypeStr==String("INDI_NUMBER") && formatted){
				result.PropertyValue=PropertyUtils::getFormattedNumber( iter->PropertyValue, IsoString( iter->PropertyNumberFormat ) );
				if (result.PropertyValue.IsEmpty()) {
					// invalid property value
					return false;
				}
			} else {
				result.PropertyValue=iter->PropertyValue;
			}
			result.PropertyState=iter->PropertyState;
			return true;
		}
	}
	return false;
}

void INDIDeviceControllerInstance::writeCurrentMessageToConsole(){
	if (Console().IsCurrentThreadConsole()){
		Console().WriteLn( "Message from INDI server: " + String( p_currentMessage ) );
	}
}

bool INDIDeviceControllerInstance::ExecuteGlobal()
{
   Console().WriteLn("INDI control client --- (c) Klaus Kretzschmar, 2014-2016");
   Console().EnableAbort();
   if (!p_connect){
	  // disconnet from server
    if (indiClient.get()->serverIsConnected()){
	   indiClient.get()->disconnectServer();
	   p_propertyList.Clear();
	}

	indiClient.release();

	return true;
   }

   if (indiClient.get() == 0)
        indiClient.reset(new INDIClient(this));

   IsoString ASCIIHost(p_host);
   indiClient->setServer(ASCIIHost.c_str(), p_port);

   if (!indiClient.get()->serverIsConnected())
		indiClient->connectServer();


   if (indiClient.get()->serverIsConnected())
     Console().WriteLn( "Successfully connected to server " + String( ASCIIHost ) + ", port=" + String( p_port ) );

   Console().Flush();

   if (p_command == String("GET")){
	   INDIPropertyListItem propItem;
	   String device=String(PropertyUtils::getDevice(p_getPropertyParam));
	   String property=String(PropertyUtils::getProperty(p_getPropertyParam));
	   String element=String(PropertyUtils::getElement(p_getPropertyParam));
	   Console().WriteLn( "Device=" + String( device ) + ", Property=" + String( property ) + ", Element=" + String( element ) );
	   if (getINDIPropertyItem(device,property,element, propItem)){
		   p_getPropertyReturnValue=propItem.PropertyValue;
		   Console().WriteLn( "Value=" + String( p_getPropertyReturnValue ) );
	   }
	   else  {
		   Console().WriteLn(ERR_MSG(String("Error: Could not get value for property " + String( p_getPropertyParam )).c_str()) );
	   }
   } else if (p_command == String("SET_ASYNCH")){
	   bool isAsynchCall=true;
	   bool wasSuccessful=sendNewProperty(isAsynchCall);
	   if (!wasSuccessful){
		   // cleanup new propertyList
		   this->p_newPropertyList.Clear();
	   }
   } else if (p_command == String("REGISTER_INSTANCE")){
	   indiClient.get()->registerScriptInstance(this);
   }
   else if (p_command == String("RELEASE_INSTANCE")){
   	   indiClient.get()->registerScriptInstance(NULL);
   }
   else{
		bool wasSuccessful = sendNewProperty();
		if (!wasSuccessful) {
			// cleanup new propertyList
			this->p_newPropertyList.Clear();
		}
	}


   // disable abort to continue running
   this->p_doAbort=false;
   m_internalAbortFlag=false;

   return true;
}

void* INDIDeviceControllerInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheINDIServerHostname )
      return p_host.Begin();
   if ( p == TheINDIServerPort )
      return &p_port;
   if ( p == TheINDIServerConnect )
      return &p_connect;
   if ( p == TheINDIProcessFlagDoAbort )
      return &p_doAbort;
   if (p == TheINDIDeviceNameParameter)
      return p_deviceList[tableRow].DeviceName.Begin();
   if (p == TheINDIPropertyNameParameter)
      return p_propertyList[tableRow].PropertyKey.Begin();
   if (p == TheINDIPropertyValueParameter)
      return p_propertyList[tableRow].PropertyValue.Begin();
   if (p == TheINDIPropertyStateParameter)
      return &p_propertyList[tableRow].PropertyState;
   if (p == TheINDIPropertyTypeParameter)
      return p_propertyList[tableRow].PropertyTypeStr.Begin();
   if (p == TheINDINewPropertyValueParameter)
      return p_newPropertyList[tableRow].NewPropertyValue.Begin();
   if (p == TheINDINewPropertyKeyParameter)
      return p_newPropertyList[tableRow].PropertyKey.Begin();
   if (p == TheINDINewPropertyTypeParameter)
      return p_newPropertyList[tableRow].PropertyType.Begin();
   if (p == TheINDIServerCommand)
      return p_command.Begin();
   if (p == TheINDIServerGetPropertyReturnValue)
      return p_getPropertyReturnValue.Begin();
   if (p == TheINDIServerGetPropertyPropertyParam)
      return p_getPropertyParam.Begin();

   return 0;
}


bool INDIDeviceControllerInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{

	if ( p == TheINDIServerHostname )
	{
		p_host.Clear();
		if ( sizeOrLength > 0 )
			p_host.Reserve( sizeOrLength );
	}
	else if ( p == TheINDIDevicesParameter){
		p_deviceList.Clear();
		if ( sizeOrLength > 0 )
			p_deviceList.Add( INDIDeviceListItem(), sizeOrLength );
	}
	else if ( p == TheINDIDeviceNameParameter){
		p_deviceList[tableRow].DeviceName.Clear();
		if ( sizeOrLength > 0 )
			p_deviceList[tableRow].DeviceName.Reserve( sizeOrLength );
	}
	else if ( p == TheINDIPropertiesParameter){
		p_propertyList.Clear();
		if ( sizeOrLength > 0 )
			p_propertyList.Add( INDIPropertyListItem(), sizeOrLength );
	}
	else if ( p == TheINDIPropertyNameParameter){
		p_propertyList[tableRow].PropertyKey.Clear();
		if ( sizeOrLength > 0 )
			p_propertyList[tableRow].PropertyKey.Reserve( sizeOrLength );
	}
	else if ( p == TheINDIPropertyValueParameter){
		p_propertyList[tableRow].PropertyValue.Clear();
		if ( sizeOrLength > 0 )
			p_propertyList[tableRow].PropertyValue.Reserve( sizeOrLength );
	}
	else if ( p == TheINDIPropertyTypeParameter){
		p_propertyList[tableRow].PropertyTypeStr.Clear();
		if ( sizeOrLength > 0 )
			p_propertyList[tableRow].PropertyTypeStr.Reserve( sizeOrLength );
	}
	else if ( p == TheINDINewPropertiesParameter){
		p_newPropertyList.Clear();
		if ( sizeOrLength > 0 )
			p_newPropertyList.Add( INDINewPropertyListItem(), sizeOrLength );
	}
	else if ( p == TheINDINewPropertyValueParameter){
		p_newPropertyList[tableRow].NewPropertyValue.Clear();
		if ( sizeOrLength > 0 )
			p_newPropertyList[tableRow].NewPropertyValue.Reserve( sizeOrLength );
	}
	else if ( p == TheINDINewPropertyKeyParameter){
		p_newPropertyList[tableRow].PropertyKey.Clear();
		if ( sizeOrLength > 0 )
			p_newPropertyList[tableRow].PropertyKey.Reserve( sizeOrLength );
	}
	else if ( p == TheINDINewPropertyTypeParameter){
		p_newPropertyList[tableRow].PropertyType.Clear();
		if ( sizeOrLength > 0 )
			p_newPropertyList[tableRow].PropertyType.Reserve( sizeOrLength );
	}
	else if ( p == TheINDIServerCommand ) {
		p_command.Clear();
		if ( sizeOrLength > 0 )
			p_command.Reserve( sizeOrLength );
	}
	else if ( p == TheINDIServerGetPropertyReturnValue ) {
		p_getPropertyReturnValue.Clear();
		if ( sizeOrLength > 0 )
			p_getPropertyReturnValue.Reserve( sizeOrLength );
	}
	else if ( p == TheINDIServerGetPropertyPropertyParam ) {
		p_getPropertyParam.Clear();
		if ( sizeOrLength > 0 )
			p_getPropertyParam.Reserve( sizeOrLength );
	}
	else
		return false;

   return true;
}

size_type INDIDeviceControllerInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
	if ( p == TheINDIServerHostname )
		return p_host.Length();
	if (p == TheINDIDevicesParameter)
		return p_deviceList.Length();
	if ( p == TheINDIDeviceNameParameter )
		return p_deviceList[tableRow].DeviceName.Length();
	if (p == TheINDIPropertiesParameter)
		return p_propertyList.Length();
	if ( p == TheINDIPropertyNameParameter )
		return p_propertyList[tableRow].PropertyKey.Length();
	if ( p == TheINDIPropertyValueParameter )
		return p_propertyList[tableRow].PropertyValue.Length();
	if ( p == TheINDIPropertyTypeParameter )
		return p_propertyList[tableRow].PropertyTypeStr.Length();
	if (p == TheINDINewPropertiesParameter)
		return p_newPropertyList.Length();
	if ( p == TheINDINewPropertyValueParameter )
		return p_newPropertyList[tableRow].NewPropertyValue.Length();
	if ( p == TheINDINewPropertyKeyParameter )
		return p_newPropertyList[tableRow].PropertyKey.Length();
	if ( p == TheINDINewPropertyTypeParameter )
		return p_newPropertyList[tableRow].PropertyType.Length();
	if (p == TheINDIServerCommand)
		return p_command.Length();
	if (p == TheINDIServerGetPropertyReturnValue)
		return p_getPropertyReturnValue.Length();
	if (p == TheINDIServerGetPropertyPropertyParam)
		return p_getPropertyParam.Length();

	return 0;
}

// ----------------------------------------------------------------------------


} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInstance.cpp - Released 2015/10/13 15:55:45 UTC
