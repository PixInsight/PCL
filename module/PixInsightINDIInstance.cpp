/// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInstance.cpp - Released 2013/03/24 18:42:27 UTC
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

#include "PixInsightINDIInstance.h"
#include "PixInsightINDIParameters.h"
#include "basedevice.h"
#include "indicom.h"
#include "PixInsightINDIclient.h"
#include "PropertyNode.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>
#include <pcl/Mutex.h>
#if defined(__PCL_LINUX)
#include <memory>
#endif
namespace pcl
{


/* Our client auto pointer */
auto_ptr<INDIClient> indiClient(0);


// ----------------------------------------------------------------------------

PixInsightINDIInstance::PixInsightINDIInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_deviceList(),
p_propertyList(),
p_newPropertyList(),
p_host( TheINDIServerHostname->DefaultValue() ),
p_port( uint32( TheINDIServerPort->DefaultValue() ) ),
p_connect( uint32( TheINDIServerConnect->DefaultValue() ) ),
p_currentMessage(""),
p_command(""),
p_getPropertyReturnValue(""),
p_getPropertyParam(""),
p_doAbort(false),
m_internalAbortFlag(false),
m_ImageDownloaded(false)

{
	
}

PixInsightINDIInstance::PixInsightINDIInstance( const PixInsightINDIInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void PixInsightINDIInstance::Assign( const ProcessImplementation& p )
{
   const PixInsightINDIInstance* x = dynamic_cast<const PixInsightINDIInstance*>( &p );
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
   static void Apply( GenericImage<P>& image, const PixInsightINDIInstance& instance )
   {
      /*
       * Your magic comes here...
       */
      Console().WriteLn( "<end><cbr>Ah, did I mention that I do just _nothing_ at all? :D" );
   }
};


bool PixInsightINDIInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "INDI client can only be executed in the global context.";
   return false;
}

bool PixInsightINDIInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}


bool PixInsightINDIInstance::getPropertyFromKeyString(INDINewPropertyListItem& propertyKey, const String& keyString){

	
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
			propertyKey.Device = keyString.SubString(start,nextStart-start);
		else if (count==2)
			propertyKey.Property = keyString.SubString(start,nextStart-start);
		 else if (count==3)
			break;
		else
		  Console().WriteLn("Assertion in getPropertyFromKeyString: Should not be here");

		count++;
		start=nextStart+1;
		nextStart = keyString.FindFirst("/",start);
	}

	if (nextStart==String::notFound && count!=0)
		propertyKey.Element = keyString.SubString(start,keyString.Length()-start);
	else
		return false;
	
	return true;

}

bool PixInsightINDIInstance::sendNewPropertyValue(const INDINewPropertyListItem& propItem,bool isAsynch){

	p_newPropertyList.Append(propItem);
	return sendNewProperty(isAsynch);
}

bool PixInsightINDIInstance::sendNewProperty(bool isAsynchCall) {
	//Precondition: NewPropertyList contains ony elements of the same property
	String deviceStr;
	String propertyStr;
	String propertyTypeStr;
	INDI::BaseDevice* device = NULL;
	bool firstTime=true;
	INumberVectorProperty * numberVecProp=NULL;
    ITextVectorProperty * textVecProp=NULL;
    ISwitchVectorProperty * switchVecProp=NULL;;
//    ILightVectorProperty * lightVecProp=NULL;

	if (p_newPropertyList.IsEmpty())
		return false; //Nothing to do

	for (pcl::Array<INDINewPropertyListItem>::iterator iter=p_newPropertyList.Begin(); iter!=p_newPropertyList.End(); ++iter){
		if (iter->NewPropertyValue.IsEmpty()) {
			Console().WriteLn("Empty property value ... exiting.");
			return false;
		}
		// initialize
		if (firstTime){
			if (getPropertyFromKeyString(*iter,iter->PropertyKey) || (!iter->Device.IsEmpty() && !iter->Property.IsEmpty() && !iter->PropertyType.IsEmpty())){
				deviceStr=iter->Device;
				propertyStr = iter->Property;
				propertyTypeStr = iter->PropertyType;
				firstTime=false;
				Console().WriteLn(String().Format("Found new property value '%s' for property '%s' of device '%s'",
								  IsoString(iter->NewPropertyValue).c_str(),
								  IsoString(propertyStr).c_str(),
								  IsoString(deviceStr).c_str()));
				Console().Flush();
				// get device
				device = indiClient.get()->getDevice(IsoString(deviceStr).c_str());
				if (!device){
				  Console().WriteLn(String().Format("Device '%s' not found ... exiting.",IsoString(deviceStr).c_str()));
				  return false;
				}
				// get property vector
				if (iter->PropertyType == "INDI_SWITCH"){
					switchVecProp = device->getSwitch(IsoString(propertyStr).c_str());
					if (!switchVecProp){
					  Console().WriteLn(String().Format("Could not get property '%s' from server. Please check that INDI device is connected.",
														IsoString(propertyStr).c_str(),
														IsoString(deviceStr).c_str()));
					  return false;
					}
				} else if (iter->PropertyType == "INDI_NUMBER"){
					numberVecProp = device->getNumber(IsoString(propertyStr).c_str());
					if (!numberVecProp){
					  Console().WriteLn(String().Format("Could not get property '%s' from server. Please check that INDI device is connected.",
										IsoString(propertyStr).c_str(),
										IsoString(deviceStr).c_str()));
					  return false;
					}
				} else if (iter->PropertyType == "INDI_TEXT"){
					textVecProp = device->getText(IsoString(propertyStr).c_str());
					if (!textVecProp){
					  Console().WriteLn(String().Format("Could not get property '%s' from server. Please check that INDI device is connected.",
										IsoString(propertyStr).c_str(),
										IsoString(deviceStr).c_str()));
					  return false;
					}
				}
				else {
				  Console().WriteLn(String().Format("Property '%s' not supported.",IsoString(propertyStr).c_str()));
				}
			}
			else{
			  Console().WriteLn(String().Format("Invalid property key '%s' not supported. ",IsoString(iter->PropertyKey).c_str()));
			}
		}
		if (getPropertyFromKeyString(*iter,iter->PropertyKey) || (!iter->Device.IsEmpty() && !iter->Property.IsEmpty() && !iter->PropertyType.IsEmpty())){
			if (switchVecProp){
				// set new switch 
				ISwitch * sp = IUFindSwitch(switchVecProp, IsoString(iter->Element).c_str());
				if (!sp){
				  Console().WriteLn(String().Format("Could not find element '%s' ... exiting.",IsoString(iter->Element).c_str()));
				  return false;
				}
				IUResetSwitch(switchVecProp);
				if (iter->NewPropertyValue == String("ON")){
					sp->s = ISS_ON;
				}
				else {
				    sp->s = ISS_OFF;
					//Console().WriteLn(String().Format("Invalid value for INDI switch: '%s' ... exiting.",IsoString(iter->NewPropertyValue).c_str()));
					//return false;
				}

			}
			else if (numberVecProp){
				// set new number value
				INumber * np = IUFindNumber(numberVecProp, IsoString(iter->Element).c_str());
				if (!np){
				  Console().WriteLn(String().Format("Could not find element '%s' ... exiting.",IsoString(iter->Element).c_str()));
				  return false;
				}
				np->value = iter->NewPropertyValue.ToDouble();
			}
			else if (textVecProp){
				// set new text value
				IText * np = IUFindText(textVecProp, IsoString(iter->Element).c_str());
				if (!np){
					Console().WriteLn(String().Format("Could not find element '%s' ... exiting.",IsoString(iter->Element).c_str()));
					return false;
				}
				IUSaveText(np,IsoString(iter->NewPropertyValue).c_str());
			}
			else {
			  Console().WriteLn(String().Format("Should not be here %d ... exiting.",__LINE__));
			  return false;
			}
		}
		else{
		  Console().WriteLn(String().Format("Invalid property key '%s' ... exiting ",IsoString(iter->PropertyKey).c_str()));
		  return false;
		}
	} // for 
		

	// send new properties to server and wait for response
	if (switchVecProp){
		indiClient.get()->sendNewSwitch(switchVecProp);
		if (switchVecProp!=NULL){
			while (switchVecProp->s!=IPS_OK && switchVecProp->s!=IPS_IDLE &&  !p_doAbort && !m_internalAbortFlag && !isAsynchCall){
				if (switchVecProp->s==IPS_ALERT){
					writeCurrentMessageToConsole();
					m_internalAbortFlag=false;
					return false;
				}
			}
			
		}else{
		  Console().WriteLn(String().Format("Could not get property value from server. Please check that INDI device %s is connected.",IsoString(deviceStr).c_str()));
		  return false;
		}

	}
	else if (numberVecProp){
		indiClient.get()->sendNewNumber(numberVecProp);
		// wait until completed or abort
		if (numberVecProp!=NULL){
			while (numberVecProp->s!=IPS_OK && numberVecProp->s!=IPS_IDLE && !p_doAbort && !m_internalAbortFlag && !isAsynchCall){
				if (numberVecProp->s==IPS_ALERT){
					writeCurrentMessageToConsole();
					m_internalAbortFlag=false;
					return false;
				}
			}

		}else{
			Console().WriteLn(String().Format("Could not get property value from server. Please check that INDI device %s is connected.",IsoString(deviceStr).c_str()));
			return false;
		}
	}
	else if (textVecProp){
		indiClient.get()->sendNewText(textVecProp);
		// wait until completed or abort
		if (textVecProp!=NULL){
			while (textVecProp->s!=IPS_OK && textVecProp->s!=IPS_IDLE && !p_doAbort && !m_internalAbortFlag && !isAsynchCall){
				if (textVecProp->s==IPS_ALERT){
					writeCurrentMessageToConsole();
					m_internalAbortFlag=false;
					return false;
				}
			}

		}else{
			Console().WriteLn(String().Format("Could not get property value from server. Please check that INDI device %s is connected.",IsoString(deviceStr).c_str()));
			return false;
		}
	}
	else {
	   Console().WriteLn(String().Format("Should not be here %d ... exiting.",__LINE__));
	}

	p_newPropertyList.Clear();

	if (m_internalAbortFlag) {
		m_internalAbortFlag=false;
	}

	return true;
}

 bool PixInsightINDIInstance::getINDIPropertyItem(String device, String property, String element,INDIPropertyListItem& result ){

	for (pcl::Array<INDIPropertyListItem>::iterator iter=p_propertyList.Begin(); iter!=p_propertyList.End(); ++iter){

		if ((iter->Device==device) && (iter->Property==property) && (iter->Element==element) ){
			result.Device=device;
			result.Property=property;
			result.Element=element;
			if (iter->PropertyTypeStr==String("INDI_NUMBER")){
				result.PropertyValue=PropertyUtils::getFormattedNumber(iter->PropertyValue,iter->PropertyNumberFormat);
			} else {
				result.PropertyValue=iter->PropertyValue;
			}
			result.PropertyState=iter->PropertyState;
			return true;
		}

	}
	return false;
}

 bool PixInsightINDIInstance::getINDIActiveSwitchPropertyItem(String device, String property, INDIPropertyListItem& result ){

 	for (pcl::Array<INDIPropertyListItem>::iterator iter=p_propertyList.Begin(); iter!=p_propertyList.End(); ++iter){

 		if (iter->PropertyTypeStr!=String("INDI_SWITCH"))
 			continue;

 		if (iter->PropertyValue!=String("ON"))
 			continue;

 		if ((iter->Device==device) && (iter->Property==property)  ){
 			result.Device=device;
 			result.Property=property;
 			result.Element=iter->Element;
 			result.PropertyValue=iter->PropertyValue;
 			result.PropertyState=iter->PropertyState;
 			result.ElementLabel=iter->ElementLabel;
 			return true;
 		}

 	}
 	return false;
 }

void PixInsightINDIInstance::writeCurrentMessageToConsole(){
	if (Console().IsCurrentThreadConsole()){
		Console().WriteLn(String().Format("Message from INDI server: %s",IsoString(p_currentMessage).c_str()));
	}
}


bool PixInsightINDIInstance::ExecuteGlobal()
{
   Console().WriteLn("INDI control client --- (c) Klaus Kretzschmar, 2014");
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
   indiClient->setServer(ASCIIHost.c_str() , p_port);
   
   if (!indiClient.get()->serverIsConnected())
		indiClient->connectServer();
		
   
   if (indiClient.get()->serverIsConnected())
     Console().WriteLn(String().Format("Successfully connected to server %s , port=%d",IsoString(ASCIIHost).c_str(),p_port));

   Console().Flush();

   if (p_command == String("GET")){
	   INDIPropertyListItem propItem;
	   String device=String(PropertyUtils::getDevice(p_getPropertyParam));
	   String property=String(PropertyUtils::getProperty(p_getPropertyParam));
	   String element=String(PropertyUtils::getElement(p_getPropertyParam));
	   Console().WriteLn(String().Format("Device=%s, Property=%s, Element=%s",IsoString(device).c_str(),IsoString(property).c_str(),IsoString(element).c_str()));
	   if (getINDIPropertyItem(device,property,element, propItem)){
		   p_getPropertyReturnValue=propItem.PropertyValue;
		   Console().WriteLn(String().Format("Value=%s",IsoString(p_getPropertyReturnValue).c_str()));
	   }
	   else  {
		   Console().WriteLn(String().Format("Error: Could not get value for property %s.",IsoString(p_getPropertyParam).c_str()));
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

void* PixInsightINDIInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheINDIServerHostname )
      return p_host.c_str();
   if ( p == TheINDIServerPort )
      return &p_port;
   if ( p == TheINDIServerConnect )
      return &p_connect;
   if ( p == TheINDIProcessFlagDoAbort )
      return &p_doAbort;
   if (p == TheINDIDeviceNameParameter)
	   return p_deviceList[tableRow].DeviceName.c_str();
   if (p == TheINDIPropertyNameParameter)
	   return p_propertyList[tableRow].PropertyKey.c_str();
   if (p == TheINDIPropertyValueParameter)
	   return p_propertyList[tableRow].PropertyValue.c_str();
   if (p == TheINDIPropertyStateParameter)
	   return &p_propertyList[tableRow].PropertyState;
   if (p == TheINDIPropertyTypeParameter)
	   return p_propertyList[tableRow].PropertyTypeStr.c_str();
   if (p == TheINDINewPropertyValueParameter)
	   return p_newPropertyList[tableRow].NewPropertyValue.c_str();
   if (p == TheINDINewPropertyKeyParameter)
	   return p_newPropertyList[tableRow].PropertyKey.c_str();
   if (p == TheINDINewPropertyTypeParameter)
	   return p_newPropertyList[tableRow].PropertyType.c_str();
   if (p == TheINDIServerCommand)
	   return p_command.c_str();
   if (p == TheINDIServerGetPropertyReturnValue)
	   return p_getPropertyReturnValue.c_str();
   if (p == TheINDIServerGetPropertyPropertyParam)
	   return p_getPropertyParam.c_str();

   return 0;
}


bool PixInsightINDIInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
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

size_type PixInsightINDIInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
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

// ****************************************************************************
// EOF PixInsightINDIInstance.cpp - Released 2013/03/24 18:42:27 UTC
