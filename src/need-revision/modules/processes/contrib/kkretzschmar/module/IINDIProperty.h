/*
 * IINDIProperty.h
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#ifndef IINDIPROPERTY_H_
#define IINDIPROPERTY_H_

#include "indiproperty.h"
#include <pcl/String.h>

namespace pcl {

class IProperty {
protected:
	INDI::Property* m_property;
public:
	IProperty(INDI::Property* property):m_property(property){}
	virtual ~IProperty(){}
	virtual INDI::Property* getProperty(){return m_property;}
	virtual String getDeviceName() {return String(m_property->getDeviceName());}
	virtual String getName()       {return String(m_property->getName());}
	virtual String getLabel()      {return String(m_property->getLabel());}
	virtual INDI_TYPE getType()    {return m_property->getType();}
	virtual String getTypeStr()    {
		switch(m_property->getType()){
		case INDI_SWITCH:
			return String("INDI_SWITCH");
			break;
		case INDI_NUMBER:
			return String("INDI_NUMBER");
			break;
		case INDI_LIGHT:
			return String("INDI_LIGHT");
			break;
		case INDI_TEXT:
			return String("INDI_TEXT");
			break;
		default:
			return String("INDI_UNKNOWN");
		}
	}
	virtual IPState getState()     {return m_property->getState();}

	virtual size_t getNumOfElements()  {return 0;}
	virtual String getElementName(size_t i)  {return String("unsupported element");}
	virtual String getElementLabel(size_t i)  {return String("unsupported element");}
	virtual String getElementValue(size_t i) {return String("unsupported value");}
	virtual String getNumberFormat(size_t i) {return String("");}

	virtual void addElement(IsoString elementName, IsoString value){}
	virtual void setDeviceName(IsoString device){}
	virtual void setName(IsoString name){}
//	virtual void setElementName(size_t i,IsoString elementName){}
//	virtual void setElementLabel(size_t i,IsoString elementLabel){}
//	virtual void setElementValue(size_t i,IsoString elementValue){}
};

class NumberProperty : public IProperty
{
public:
	NumberProperty(INDI::Property* property):IProperty(property){}
	virtual size_t getNumOfElements() {return m_property->getNumber()->nnp;}
	virtual String getElementName(size_t i);
	virtual String getElementLabel(size_t i);
	virtual String getElementValue(size_t i);
	virtual String getNumberFormat(size_t i);
	virtual void addElement(IsoString elementName, IsoString value);
	virtual void setDeviceName(IsoString device){strcpy(((INumberVectorProperty*) m_property->getProperty())->device,device.c_str());}
	virtual void setName(IsoString name){strcpy(((INumberVectorProperty*) m_property->getProperty())->name,name.c_str());}
//	virtual void setElementName(size_t i,IsoString name){}
//	virtual void setElementValue(size_t i,IsoString name){}
};

class TextProperty : public IProperty
{
public:
	TextProperty(INDI::Property* property):IProperty(property){}
	virtual size_t getNumOfElements() {return m_property->getText()->ntp;}
	virtual String getElementName(size_t i);
	virtual String getElementLabel(size_t i);
	virtual String getElementValue(size_t i);
	virtual void addElement(IsoString elementName, IsoString value);
	virtual void setDeviceName(IsoString device){strcpy(((ITextVectorProperty*) m_property->getProperty())->device,device.c_str());}
	virtual void setName(IsoString name){strcpy(((ITextVectorProperty*) m_property->getProperty())->name,name.c_str());}
//	virtual void setElementName(size_t i,IsoString name){strcpy(((ITextVectorProperty*) m_property->getProperty())->tp[i].name,name.c_str());}
//	virtual void setElementValue(size_t i,IsoString value);
};

class SwitchProperty : public IProperty
{
public:
	SwitchProperty(INDI::Property* property):IProperty(property){}
	virtual size_t getNumOfElements() {return m_property->getSwitch()->nsp;}
	virtual String getElementName(size_t i);
	virtual String getElementLabel(size_t i);
	virtual String getElementValue(size_t i);

	virtual void addElement(IsoString elementName, IsoString value);
	virtual void setDeviceName(IsoString device){strcpy(((ISwitchVectorProperty*) m_property->getProperty())->device,device.c_str());}
	virtual void setName(IsoString name){strcpy(((ISwitchVectorProperty*) m_property->getProperty())->name,name.c_str());}
//	virtual void setElementName(size_t i,IsoString name);
//	virtual void setElementValue(size_t i,IsoString value);
};

class LightProperty : public IProperty
{
public:
	LightProperty(INDI::Property* property):IProperty(property){}
	virtual size_t getNumOfElements() {return m_property->getLight()->nlp;}
	virtual String getElementName(size_t i);
	virtual String getElementLabel(size_t i);
	virtual String getElementValue(size_t i);

	virtual void addElement(IsoString elementName, IsoString value);
	virtual void setDeviceName(IsoString device){strcpy(((ILightVectorProperty*) m_property->getProperty())->device,device.c_str());}
	virtual void setName(IsoString name){strcpy(((ILightVectorProperty*) m_property->getProperty())->name,name.c_str());}
//	virtual void setElementName(size_t i,IsoString name){strcpy(((ILightVectorProperty*) m_property->getProperty())->lp[i].name,name.c_str());}
//	virtual void setElementValue(size_t i,IsoString value);
};


class PropertyFactory{
public:
	static IProperty* create(INDI::Property* property);
	static IProperty* create(INDI::Property* property,INDI_TYPE type);
};

}


#endif /* IINDIPROPERTY_H_ */
