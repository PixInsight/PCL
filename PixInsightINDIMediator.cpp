#include <stdio.h>
#include <iostream>
#include <fstream>
#include "PixInsightINDIMediator.h"
#include <pcl/Console.h>

//#include "PixInsightINDIInterface.h"
//#include "indibase.h"



namespace pcl {

	
	void PixInsightINDIMediator::newProperty(INDI::Property *property){
		String sep("/");
		switch(property->getType()){
			case INDI_TEXT:
				{
					for (int i=0; i<property->getText()->ntp;i++) {
						INDIPropertyListItem propertyListItem;
						propertyListItem.Device=String(property->getDeviceName());
						propertyListItem.Property=String(property->getName());
						propertyListItem.PropertyType=INDI_TEXT;
						propertyListItem.Element = String(property->getText()->tp[i].name);
						propertyListItem.PropertyState = property->getState();
						propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element ;
						propertyListItem.PropertyValue=property->getText()->tp[i].text;
						m_Instance->p_propertyList.Append(propertyListItem);
					}
					break;
				}
			case INDI_SWITCH:
				{
					for (int i=0; i<property->getSwitch()->nsp;i++) {
						INDIPropertyListItem propertyListItem;
						propertyListItem.Device=String(property->getDeviceName());
						propertyListItem.Property=String(property->getName());
						propertyListItem.PropertyType=INDI_SWITCH;
						propertyListItem.Element=property->getSwitch()->sp[i].name;
						propertyListItem.PropertyState = property->getState();
						propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
						propertyListItem.PropertyValue=property->getSwitch()->sp[i].s == ISS_ON  ? "ON" : "OFF"  ;
						m_Instance->p_propertyList.Append(propertyListItem);
					}
					break;
				}
			case INDI_NUMBER:
				{
					for (int i=0; i<property->getNumber()->nnp;i++) {
						INDIPropertyListItem propertyListItem;
						propertyListItem.Device=String(property->getDeviceName());
						propertyListItem.Property=String(property->getName());
						propertyListItem.PropertyType=INDI_NUMBER;
						propertyListItem.Element=property->getNumber()->np[i].name;
						IsoString number(property->getNumber()->np[i].value);
						propertyListItem.PropertyState = property->getState();
						propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
						propertyListItem.PropertyValue=number.c_str();
						m_Instance->p_propertyList.Append(propertyListItem);
					}
					break;
				}
			case INDI_LIGHT:
				{
					for (int i=0; i<property->getLight()->nlp;i++) {
						INDIPropertyListItem propertyListItem;
						propertyListItem.Device=String(property->getDeviceName());
						propertyListItem.Property=String(property->getName());
						propertyListItem.PropertyType=INDI_LIGHT;
						propertyListItem.Element=property->getLight()->lp[i].name;
						propertyListItem.PropertyState = property->getState();
						propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
						switch (property->getLight()->lp[i].s){
						case IPS_IDLE:
							propertyListItem.PropertyValue="IDLE";
						    break;
						case IPS_OK:
							propertyListItem.PropertyValue="OK";
						    break;
						case IPS_BUSY:
							propertyListItem.PropertyValue="BUSY";
						    break;
						case IPS_ALERT:
							propertyListItem.PropertyValue="ALERT";
						    break;
						}
						m_Instance->p_propertyList.Append(propertyListItem);
					}
					break;
				}
			default:
			  {
				INDIPropertyListItem propertyListItem;
				propertyListItem.Device=String(property->getDeviceName());
				propertyListItem.Property=String(property->getName());
				propertyListItem.PropertyType=INDI_UNKNOWN;
				propertyListItem.Element=String("no element");
				propertyListItem.PropertyState = property->getState();
				propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
				propertyListItem.PropertyValue=String("no value");
				m_Instance->p_propertyList.Append(propertyListItem);
			  }
			}
	}

	void PixInsightINDIMediator::newMessage(INDI::BaseDevice *dp, int messageID){
		if (m_pixInterface) {
			m_pixInterface->GUI->DrvPropDlg.m_serverMessage = IsoString(dp->messageQueue(messageID));	
			m_pixInterface->m_PropertyListNeedsUpdate=true;
		}
		m_Instance->p_currentMessage=IsoString(dp->messageQueue(messageID));
	}

	void PixInsightINDIMediator::newSwitch(ISwitchVectorProperty *svp){
		
		//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void PixInsightINDIMediator::newNumber(INumberVectorProperty *nvp){
		String sep("/");
		for (int i=0; i<nvp->nnp;i++) {
			INDIPropertyListItem propertyListItem;
			propertyListItem.Device=String(nvp->device);
			propertyListItem.Property=String(nvp->name);
			propertyListItem.PropertyType=INDI_NUMBER;
			propertyListItem.Element=(nvp->np[i].name);
			IsoString number(nvp->np[i].value);
			propertyListItem.PropertyState = nvp->s;
			propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
			pcl::Array<INDIPropertyListItem>::iterator iter = m_Instance->p_propertyList.Search(propertyListItem);

			if (iter==m_Instance->p_propertyList.End()){
				m_Instance->p_currentMessage=IsoString("Warning: Property not found.");
				return;
			}

			propertyListItem.PropertyValue=number.c_str();
			*iter=propertyListItem;
		}	
	}
	void PixInsightINDIMediator::newText(ITextVectorProperty *tvp){
				//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void PixInsightINDIMediator::newLight(ILightVectorProperty *lvp){
				//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void PixInsightINDIMediator::newBLOB(IBLOB *bp){
		IsoString fileName = IsoString("C:/Users/klaus/tmp/") + IsoString(bp->label) + IsoString(".fits"); 
	    ofstream myfile;
		myfile.open (fileName.c_str(),ios::out|ios::binary);
		myfile.write((const char*) bp->blob,bp->bloblen);
		myfile.close();		
	}
}