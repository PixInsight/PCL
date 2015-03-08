#include <stdio.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "PixInsightINDIclient.h"
#include "PropertyNode.h"
#include <pcl/Console.h>




namespace pcl {

	INDIClient::INDIClient(IPixInsightINDIInstance* instance,bool initThreads):BaseClientImpl(initThreads),m_Instance(instance),m_ScriptInstance(NULL)
	 {
		 if (m_Instance==NULL){
			 throw FatalError(ERR_MSG("Invalid instance pointer."));
		 }
	 }

	void INDIClient::runOnPropertyTable(IProperty* INDIProperty, const ArrayOperator<INDIPropertyListItem>* arrayOp, PropertyFlagType flag){

		if (indiClient.get()!=NULL){
			indiClient.get()->m_mutex.Lock();
		}
		String sep("/");
		INDIPropertyListItem propertyListItem;
		propertyListItem.Device=INDIProperty->getDeviceName();		
		propertyListItem.Property=INDIProperty->getName();
		propertyListItem.PropertyType=INDIProperty->getType();
		propertyListItem.PropertyTypeStr=INDIProperty->getTypeStr();
		propertyListItem.PropertyState =INDIProperty->getState();
		propertyListItem.PropertyLabel =INDIProperty->getLabel();

		for (size_t i=0; i<INDIProperty->getNumOfElements();i++) {
			propertyListItem.Element=INDIProperty->getElementName(i);
			propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
			propertyListItem.PropertyValue=INDIProperty->getElementValue(i);
			propertyListItem.PropertyFlag=flag;
			propertyListItem.PropertyNumberFormat=INDIProperty->getNumberFormat(i);
			propertyListItem.ElementLabel=INDIProperty->getElementLabel(i);
			arrayOp->run(m_Instance->getPropertyList(),propertyListItem);
		    if (m_ScriptInstance) {
			  arrayOp->run(m_ScriptInstance->getPropertyList(), propertyListItem);
		    }
		}
		if (indiClient.get()!=NULL){
			indiClient.get()->m_mutex.Unlock();
		}
	}

	void INDIClient::newDevice(INDI::BaseDevice *dp){
		assert(dp!=NULL);
		if (m_Instance!=NULL){
			INDIDeviceListItem deviceListItem;
			deviceListItem.DeviceName=String(dp->getDeviceName());
			deviceListItem.DeviceLabel=String(dp->getDriverName());
			m_Instance->getDeviceList().Append(deviceListItem);
			if (m_ScriptInstance) {
				m_ScriptInstance->getDeviceList().Append(deviceListItem);
			}
		}

	}

	void INDIClient::deleteDevice(INDI::BaseDevice *dp){
		assert(dp!=NULL);

		if (m_Instance!=NULL){
			for (PixInsightINDIInstance::PropertyListType::iterator iter=m_Instance->getPropertyList().Begin() ; iter!=m_Instance->getPropertyList().End(); ++iter){
				if (iter->Device==String(dp->getDeviceName())){
					iter->PropertyFlag=Remove;
				}
			}
			// TODO script instance
		}
	}


	void INDIClient::newProperty(INDI::Property *property){
		assert(property!=NULL);
		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* append=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayAppend<INDIPropertyListItem>());
		// add property to the property process parameter table

		runOnPropertyTable(INDIProperty,append,Insert);

		setBLOBMode(B_ALSO,property->getDeviceName());
	}

	void INDIClient::removeProperty(INDI::Property *property){
		if (property!=NULL){
			if (indiClient.get()!=NULL){
				indiClient.get()->m_mutex.Lock();
			}
			ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

			IProperty* INDIProperty = PropertyFactory::create(property);

			String sep("/");
			INDIPropertyListItem propertyListItem;
			propertyListItem.Device = INDIProperty->getDeviceName();
			propertyListItem.Property = INDIProperty->getName();
			propertyListItem.PropertyType = INDIProperty->getType();
			propertyListItem.PropertyTypeStr = INDIProperty->getTypeStr();
			propertyListItem.PropertyState = INDIProperty->getState();

			for (size_t i = 0; i < INDIProperty->getNumOfElements(); i++) {
				propertyListItem.Element = INDIProperty->getElementName(i);
				propertyListItem.PropertyKey = sep + propertyListItem.Device + sep
						+ propertyListItem.Property + sep
						+ propertyListItem.Element;
				propertyListItem.PropertyValue = INDIProperty->getElementValue(i);
				propertyListItem.PropertyFlag=Remove;
				update->run(m_Instance->getPropertyList(), propertyListItem);
				if (m_ScriptInstance) {
					update->run(m_ScriptInstance->getPropertyList(), propertyListItem);
				}
			}
		}
		if (indiClient.get()!=NULL){
			indiClient.get()->m_mutex.Unlock();
		}
	}

	void INDIClient::newMessage(INDI::BaseDevice *dp, int messageID){
		const char* message = dp->messageQueue(messageID);
		if (message!=NULL){
			m_Instance->getCurrentMessage()=IsoString(message);
			if (m_ScriptInstance) {
				m_ScriptInstance->getCurrentMessage()=IsoString(message);
			}
		}
	}

	void INDIClient::newSwitch(ISwitchVectorProperty *svp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(svp);
		property->setType(INDI_SWITCH);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());
		runOnPropertyTable(INDIProperty,update,Update);

	}

	void INDIClient::newNumber(INumberVectorProperty *nvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(nvp);
		property->setType(INDI_NUMBER);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,update,Update);
	}

	void INDIClient::newText(ITextVectorProperty *tvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(tvp);
		property->setType(INDI_TEXT);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,update,Update);
	}

	void INDIClient::newLight(ILightVectorProperty *lvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(lvp);
		property->setType(INDI_LIGHT);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,update,Update);
	}

	void INDIClient::newBLOB(IBLOB *bp){
#if defined(WIN32) 
	  const char* tmpFolder = getenv("TMP");
#else
	  const char* tmpFolder = getenv("TMPDIR");
#endif
        if (tmpFolder!=NULL)
	    {
        	IsoString fileName = IsoString(tmpFolder) + IsoString("/") + IsoString(bp->label) + IsoString(bp->format);
        	ofstream myfile;
        	myfile.open (fileName.c_str(),ios::out|ios::binary);
        	myfile.write((const char*) bp->blob,bp->size);
        	myfile.close();
        	m_Instance->setImageDownloadedFlag(true);
		if (m_ScriptInstance) {
			m_ScriptInstance->setImageDownloadedFlag(true);
		}
	    }
	  else {
	    m_Instance->getCurrentMessage() =IsoString("TMPDIR environment variable not set.");
	    if (m_ScriptInstance) {
	    	m_ScriptInstance->getCurrentMessage() =IsoString("TMPDIR environment variable not set.");
	    }
	  }
	}
}
