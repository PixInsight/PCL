#include <stdio.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "PixInsightINDIclient.h"
#include "PropertyNode.h"
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


	void INDIClient::runOnPropertyTable(IProperty* INDIProperty, const ArrayOperator<INDIPropertyListItem>* arrayOp){
		String sep("/");
		INDIPropertyListItem propertyListItem;
		propertyListItem.Device=INDIProperty->getDeviceName();		
		propertyListItem.Property=INDIProperty->getName();
		propertyListItem.PropertyType=INDIProperty->getType();
		propertyListItem.PropertyTypeStr=INDIProperty->getTypeStr();
		propertyListItem.PropertyState =INDIProperty->getState();
		for (size_t i=0; i<INDIProperty->getNumOfElements();i++) {
			propertyListItem.Element=INDIProperty->getElementName(i); 		
			propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
			propertyListItem.PropertyValue=INDIProperty->getElementValue(i);
			arrayOp->run(m_Instance->p_propertyList,propertyListItem);
		}

	}

	void INDIClient::newDevice(INDI::BaseDevice *dp){
		assert(dp!=NULL);
		if (m_Instance!=NULL){
			INDIDeviceListItem deviceListItem;
			deviceListItem.DeviceName=String(dp->getDeviceName());
			deviceListItem.DeviceLabel=String(dp->getDriverName());
			m_Instance->p_deviceList.Append(deviceListItem);
		}

	}

	void INDIClient::newProperty(INDI::Property *property){		
		assert(property!=NULL);
		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* append=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayAppend<INDIPropertyListItem>());

		// add property to the property process parameter table
		runOnPropertyTable(INDIProperty,append);
		

		setBLOBMode(B_ALSO,property->getDeviceName());
	}

	void INDIClient::removeProperty(INDI::Property *property){
		if (property!=NULL){
			IProperty* INDIProperty = PropertyFactory::create(property);
			ArrayOperator<INDIPropertyListItem>* _delete=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayDelete<INDIPropertyListItem>());

			runOnPropertyTable(INDIProperty,_delete);
		}
	}

	void INDIClient::newMessage(INDI::BaseDevice *dp, int messageID){
		const char* message = dp->messageQueue(messageID);
		if (message!=NULL){
			m_Instance->p_currentMessage=IsoString();
		}
	}

	void INDIClient::newSwitch(ISwitchVectorProperty *svp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(svp);
		property->setType(INDI_SWITCH);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,update);
	}

	void INDIClient::newNumber(INumberVectorProperty *nvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(nvp);
		property->setType(INDI_NUMBER);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,update);	
	}

	void INDIClient::newText(ITextVectorProperty *tvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(tvp);
		property->setType(INDI_TEXT);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,update);
	}

	void INDIClient::newLight(ILightVectorProperty *lvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(lvp);
		property->setType(INDI_LIGHT);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,update);
	}

	void INDIClient::newBLOB(IBLOB *bp){
#if defined(WIN32) 
	  const char* tmpFolder = getenv("TMP");
#else
	  const char* tmpFolder = getenv("TMPDIR");
#endif
        if (tmpFolder!=NULL)
	    {
        	IsoString fileName = IsoString(tmpFolder) + IsoString("/") + IsoString(bp->label) + IsoString(".fits");
        	ofstream myfile;
        	myfile.open (fileName.c_str(),ios::out|ios::binary);
        	myfile.write((const char*) bp->blob,bp->bloblen);
        	myfile.close();
	    }
	  else {
	    m_Instance->p_currentMessage=IsoString("TMPDIR environment variable not set.");
	  }
	}
}
