/*
 * FakeINDIClient.cpp
 *
 *  Created on: May 28, 2014
 *      Author: klaus
 */
#include<string.h>
#include <malloc.h>
#include "FakeINDIClient.h"
//#include "../indiapi.h"
#include "../INDIPropertyBuilder.h"
#include "../error.h"


namespace pcl {

bool FakeINDIClient::connectServer() {
	INDIPropertyBuilder switchBuilder(INDI_SWITCH);
	switchBuilder.device("TestDevice").property("TestSwitchProperty").addElement("TestElement","OFF");

	INDI::Property* prop = switchBuilder.getProperty()->getProperty();
	CHECK_POINTER(prop);
	newProperty(prop);

	INDIPropertyBuilder numberBuilder(INDI_NUMBER);
	numberBuilder.device("TestDevice").property("TestNumberProperty").addElement("TestElement","1.5");

	prop = numberBuilder.getProperty()->getProperty();
	CHECK_POINTER(prop);
	newProperty(prop);

	INDIPropertyBuilder textBuilder(INDI_TEXT);
	textBuilder.device("TestDevice").property("TestTextProperty").addElement("TestElement", "value");

	prop = textBuilder.getProperty()->getProperty();
	CHECK_POINTER(prop);
	newProperty(prop);

	INDIPropertyBuilder lightBuilder(INDI_LIGHT);
	lightBuilder.device("TestDevice").property("TestLightProperty").addElement("TestElement", "IDLE");

	prop = lightBuilder.getProperty()->getProperty();
	CHECK_POINTER(prop);
	newProperty(prop);


	return true;

}



void FakeINDIClient::sendNewSwitch (const char * deviceName, const char *propertyName, const char *elementName){

	INDIPropertyBuilder builder(INDI_SWITCH);
	builder.device("TestDevice").property("TestSwitchProperty").addElement("TestElement","ON");

	INDI::Property* prop = builder.getProperty()->getProperty();
	CHECK_POINTER(prop);

	newSwitch((ISwitchVectorProperty*) prop->getProperty());
}

void FakeINDIClient::sendNewNumber (const char * deviceName, const char *propertyName, const char *elementName, double value){

	INDIPropertyBuilder builder(INDI_NUMBER);
	builder.device("TestDevice").property("TestNumberProperty").addElement("TestElement",IsoString(value));

	INDI::Property* prop = builder.getProperty()->getProperty();
	CHECK_POINTER(prop);

	newNumber((INumberVectorProperty*) prop->getProperty());
}

void FakeINDIClient::sendNewText (const char * deviceName, const char *propertyName, const char *elementName, const char* text){

	INDIPropertyBuilder builder(INDI_TEXT);
	builder.device("TestDevice").property("TestTextProperty").addElement("TestElement",IsoString(text));

	INDI::Property* prop = builder.getProperty()->getProperty();
	CHECK_POINTER(prop);

	newText((ITextVectorProperty*) prop->getProperty());
}

void FakeINDIClient::removeText(const char * deviceName, const char *propertyName, const char *elementName, const char* text){
	INDIPropertyBuilder builder(INDI_TEXT);
	builder.device("TestDevice").property("TestTextProperty").addElement(
			"TestElement", IsoString(text));

	INDI::Property* prop = builder.getProperty()->getProperty();
	CHECK_POINTER(prop);

	removeProperty( prop );
}

void FakeINDIClient::removeNumber (const char * deviceName, const char *propertyName, const char *elementName, double value){

	INDIPropertyBuilder builder(INDI_NUMBER);
	builder.device("TestDevice").property("TestNumberProperty").addElement("TestElement",IsoString(value));

	INDI::Property* prop = builder.getProperty()->getProperty();
	CHECK_POINTER(prop);

	removeProperty( prop );
}

void FakeINDIClient::removeSwitch (const char * deviceName, const char *propertyName, const char *elementName){

	INDIPropertyBuilder builder(INDI_SWITCH);
	builder.device("TestDevice").property("TestSwitchProperty").addElement("TestElement","ON");

	INDI::Property* prop = builder.getProperty()->getProperty();
	CHECK_POINTER(prop);

	removeProperty( prop);
}

} /* namespace pcl */
