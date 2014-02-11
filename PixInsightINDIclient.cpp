#include <stdio.h>
#include <iostream>
#include <fstream>
#include "PixInsightINDIclient.h"
#include <pcl/Console.h>




namespace pcl {

	IProperty* PropertyFactory::create(INDI::Property* property){
		switch(property->getType()){
		case INDI_NUMBER:
			return new NumberProperty(property);
			break;
		case INDI_TEXT:
			return new TextProperty(property);
			break;
		case INDI_SWITCH:
			return new SwitchProperty(property);
			break;
		case INDI_LIGHT:
			return new LightProperty(property);
			break;
		default:
			return new IProperty(property);
		}
	}

	void INDIClient::newProperty(INDI::Property *property){
		String sep("/");
		IProperty* INDIProperty = PropertyFactory::create(property);

		INDIPropertyListItem propertyListItem;
		propertyListItem.Device=INDIProperty->getDeviceName();		
		propertyListItem.Property=INDIProperty->getName();
		propertyListItem.PropertyType=INDIProperty->getType();
		propertyListItem.PropertyState =INDIProperty->getState();
		for (size_t i=0; i<INDIProperty->getNumOfElements();i++) {
			propertyListItem.Element=INDIProperty->getElementName(i); 		
			propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
			propertyListItem.PropertyValue=INDIProperty->getElementValue(i);
			m_Instance->p_propertyList.Append(propertyListItem);
		}
		
	}

	void INDIClient::newMessage(INDI::BaseDevice *dp, int messageID){
		m_Instance->p_currentMessage=IsoString(dp->messageQueue(messageID));
	}

	void INDIClient::newSwitch(ISwitchVectorProperty *svp){
		
		//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void INDIClient::newNumber(INumberVectorProperty *nvp){
		String sep("/");
		INDIPropertyListItem propertyListItem;
		propertyListItem.Device=String(nvp->device);
		propertyListItem.Property=String(nvp->name);
		propertyListItem.PropertyType=INDI_NUMBER;
		propertyListItem.PropertyState = nvp->s;
		for (int i=0; i<nvp->nnp;i++) {
			propertyListItem.Element=(nvp->np[i].name);
			IsoString number(nvp->np[i].value);
			propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
			propertyListItem.PropertyValue=number.c_str();

			pcl::Array<INDIPropertyListItem>::iterator iter = m_Instance->p_propertyList.Search(propertyListItem);
			if (iter==m_Instance->p_propertyList.End()){
				m_Instance->p_currentMessage=IsoString("Warning: Property not found.");
				return;
			}
			*iter=propertyListItem;
		}	
	}
	void INDIClient::newText(ITextVectorProperty *tvp){
				//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void INDIClient::newLight(ILightVectorProperty *lvp){
				//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void INDIClient::newBLOB(IBLOB *bp){
		IsoString fileName = IsoString("C:/Users/klaus/tmp/") + IsoString(bp->label) + IsoString(".fits"); 
	    ofstream myfile;
		myfile.open (fileName.c_str(),ios::out|ios::binary);
		myfile.write((const char*) bp->blob,bp->bloblen);
		myfile.close();		
	}
}