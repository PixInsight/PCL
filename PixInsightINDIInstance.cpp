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
#include "PixInsightINDIMediator.h"
//#include "PixInsightINDIclient.h"

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
auto_ptr<PixInsightINDIMediator> mediator(0);


// ----------------------------------------------------------------------------

PixInsightINDIInstance::PixInsightINDIInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_host( TheINDIServerHostname->DefaultValue() ),
p_port( uint32( TheINDIServerPort->DefaultValue() ) ),
p_propertyList(200),
p_newPropertyList()
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
	  p_propertyList = x->p_propertyList;
	  p_newPropertyList = x->p_newPropertyList;

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
			Console()<<"Assertion in getPropertyFromKeyString: Should not be here";

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


void PixInsightINDIInstance::setNewProperties(){
	for (pcl::Array<INDINewPropertyListItem>::iterator iter=p_newPropertyList.Begin(); iter!=p_newPropertyList.End(); ++iter ){
		if (!iter->NewPropertyValue.IsEmpty()){

			Console()<<"Found new property value= "<<iter->NewPropertyValue<<" for property key= "<<iter->PropertyKey<<"\n";
			if (getPropertyFromKeyString(*iter,iter->PropertyKey)){
				Console()<<"DEVICE= "<<iter->Device<<", property="<<iter->Property<<", element="<<iter->Element<<"\n";
			
				if (iter->PropertyType == "INDI_SWITCH"){
					indiClient.get()->sendNewSwitch(IsoString(iter->Device).c_str(),IsoString(iter->Property).c_str(),IsoString(iter->Element).c_str());
					INDI::BaseDevice* device = indiClient.get()->getDevice(IsoString(iter->Device).c_str());
					if (device==NULL)
						return;
					ISwitchVectorProperty* prop = device->getSwitch(IsoString(iter->Property).c_str());
					while (prop->s==2 ){
						
					}
				}
				if (iter->PropertyType == "INDI_TEXT"){
					indiClient.get()->sendNewText(IsoString(iter->Device).c_str(),IsoString(iter->Property).c_str(),IsoString(iter->Element).c_str(),IsoString(iter->NewPropertyValue).c_str());
					
				}
				if (iter->PropertyType == "INDI_NUMBER"){
					indiClient.get()->sendNewNumber(IsoString(iter->Device).c_str(),IsoString(iter->Property).c_str(),IsoString(iter->Element).c_str(),iter->NewPropertyValue.ToDouble());
					INDI::BaseDevice* device = indiClient.get()->getDevice(IsoString(iter->Device).c_str());
					if (device==NULL)
						return;
					INumberVectorProperty* prop = device->getNumber(IsoString(iter->Property).c_str());
					while (prop->s==2 ){
						
					}
				}
			}

		}
	}
	p_newPropertyList.Clear();
}

void PixInsightINDIInstance::getProperties(){
	if (indiClient.get() == NULL){
		Console() <<"INDI client not initialized\n";
		return;
	}

	if (!indiClient.get()->serverIsConnected()){
		Console() <<"INDI server not connected. Please connect to an INDI server\n";
		return;
	}

	String sep=String("/");
	Mutex mutex;
	mutex.Lock();
	size_t count=0;
	vector<INDI::BaseDevice *> pDevices = indiClient.get()->getDevices();
	for (std::vector<INDI::BaseDevice *>::iterator it = pDevices.begin(); it!=pDevices.end(); ++it  )
	{
		indiClient->setBLOBMode(B_ALSO,(*it)->getDeviceName());
		vector<INDI::Property *>* pProperties = (*it)->getProperties();
		for (std::vector<INDI::Property*>::iterator propIt = pProperties->begin(); propIt!=pProperties->end(); ++propIt){

			switch((*propIt)->getType()){
			case INDI_TEXT:
				{
					for (int i=0; i<(*propIt)->getText()->ntp;i++) {
						INDIPropertyListItem propertyListItem;
						propertyListItem.Device=String((*it)->getDeviceName());
						propertyListItem.Property=String((*propIt)->getName());
						propertyListItem.PropertyType=INDI_TEXT;
						propertyListItem.Element = String((*propIt)->getText()->tp[i].name);
						propertyListItem.PropertyState = (*propIt)->getState();
						propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element ;
						propertyListItem.PropertyValue=(*propIt)->getText()->tp[i].text;
						p_propertyList[count]=propertyListItem;
						count++;
					}
					break;
				}
			case INDI_SWITCH:
				{
					for (int i=0; i<(*propIt)->getSwitch()->nsp;i++) {
						INDIPropertyListItem propertyListItem;
						propertyListItem.Device=String((*it)->getDeviceName());
						propertyListItem.Property=String((*propIt)->getName());
						propertyListItem.PropertyType=INDI_SWITCH;
						propertyListItem.Element=(*propIt)->getSwitch()->sp[i].name;
						propertyListItem.PropertyState = (*propIt)->getState();
						propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
						propertyListItem.PropertyValue=(*propIt)->getSwitch()->sp[i].s == ISS_ON  ? "ON" : "OFF"  ;
						p_propertyList[count]=propertyListItem;
						count++;
					}
					break;
				}
			case INDI_NUMBER:
				{
					for (int i=0; i<(*propIt)->getNumber()->nnp;i++) {
						INDIPropertyListItem propertyListItem;
						propertyListItem.Device=String((*it)->getDeviceName());
						propertyListItem.Property=String((*propIt)->getName());
						propertyListItem.PropertyType=INDI_NUMBER;
						propertyListItem.Element=(*propIt)->getNumber()->np[i].name;
						IsoString number((*propIt)->getNumber()->np[i].value);
						propertyListItem.PropertyState = (*propIt)->getState();
						propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
						propertyListItem.PropertyValue=number.c_str();
						p_propertyList[count]=propertyListItem;
						count++;
					}
					break;
				}
			default:
			  {
				INDIPropertyListItem propertyListItem;
				propertyListItem.Device=String((*it)->getDeviceName());
				propertyListItem.Property=String((*propIt)->getName());
				propertyListItem.PropertyType=INDI_UNKNOWN;
				propertyListItem.Element=String("no element");
				propertyListItem.PropertyState = (*propIt)->getState();
				propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
				propertyListItem.PropertyValue=String("no value");
				p_propertyList[count]=propertyListItem;
				count++;
			  }
			}
			
			//Console()<<"Detected property "<<(*propIt)->getName() <<"\n";
		}
	}
	
	mutex.Unlock();
}

bool PixInsightINDIInstance::ExecuteGlobal()
{
   if (indiClient.get() == 0)
        indiClient.reset(new INDIClient());

   if (mediator.get() == 0){
	   mediator.reset(new PixInsightINDIMediator(this));
   }
   
   IsoString ASCIIHost(p_host);
   indiClient->setServer(ASCIIHost.c_str() , p_port);
   
   if (!indiClient.get()->serverIsConnected())
		indiClient->connectServer();
		
   
   if (indiClient.get()->serverIsConnected())
     Console()<<"Successfully connected to server "<<ASCIIHost.c_str()<<", port="<<p_port<<"\n";

   Console().Flush();

   setNewProperties();

   getProperties();


   

   return true;
}

void* PixInsightINDIInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheINDIServerHostname )
      return p_host.c_str();
   if ( p == TheINDIServerPort )
      return &p_port;
   if (p == TheINDIPropertyNameParameter)
	   return p_propertyList[tableRow].PropertyKey.c_str();
   if (p == TheINDIPropertyValueParameter)
	   return p_propertyList[tableRow].PropertyValue.c_str();
   if (p == TheINDIPropertyStateParameter)
	   return &p_propertyList[tableRow].PropertyState;
   if (p == TheINDINewPropertyValueParameter)
	   return p_newPropertyList[tableRow].NewPropertyValue.c_str();
   if (p == TheINDINewPropertyKeyParameter)
	   return p_newPropertyList[tableRow].PropertyKey.c_str();
   if (p == TheINDINewPropertyTypeParameter)
	   return p_newPropertyList[tableRow].PropertyType.c_str();


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
	else
		return false;

   return true;
}

size_type PixInsightINDIInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
	if ( p == TheINDIServerHostname )
		return p_host.Length();
	if (p == TheINDIPropertiesParameter)
		return p_propertyList.Length();
	if ( p == TheINDIPropertyNameParameter )
		return p_propertyList[tableRow].PropertyKey.Length();
	if ( p == TheINDIPropertyValueParameter )
		return p_propertyList[tableRow].PropertyValue.Length();
	if (p == TheINDINewPropertiesParameter)
		return p_newPropertyList.Length();
	if ( p == TheINDINewPropertyValueParameter )
		return p_newPropertyList[tableRow].NewPropertyValue.Length();
	if ( p == TheINDINewPropertyKeyParameter )
		return p_newPropertyList[tableRow].PropertyKey.Length();
	if ( p == TheINDINewPropertyTypeParameter )
		return p_newPropertyList[tableRow].PropertyType.Length();


	return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF PixInsightINDIInstance.cpp - Released 2013/03/24 18:42:27 UTC
