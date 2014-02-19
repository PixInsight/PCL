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

	void INDIClient::newProperty(INDI::Property *property){		
		if (property==NULL)
			return;
		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* append=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayAppend<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,append);		
		setBLOBMode(B_ALSO,property->getDeviceName());
	}

	void INDIClient::removeProperty(INDI::Property *property){
		if (property==NULL)
			return;
		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* _delete=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayDelete<INDIPropertyListItem>());

		runOnPropertyTable(INDIProperty,_delete);
	}

	void INDIClient::newMessage(INDI::BaseDevice *dp, int messageID){
		m_Instance->p_currentMessage=IsoString(dp->messageQueue(messageID));
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
		IsoString fileName = IsoString("C:/Users/klaus/tmp/") + IsoString(bp->label) + IsoString(".fits"); 
	    ofstream myfile;
		myfile.open (fileName.c_str(),ios::out|ios::binary);
		myfile.write((const char*) bp->blob,bp->bloblen);
		myfile.close();		
	}
}