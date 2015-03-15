/*
 * INDIPropertyBuilder.cpp
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */
#include <pcl/Exception.h>
#include "INDIPropertyBuilder.h"
#include "indiproperty.h"
#include "error.h"


namespace pcl {

INDIPropertyBuilder::INDIPropertyBuilder(INDI_TYPE t):m_property(NULL){
	INDI::Property* property = new INDI::Property();
	CHECK_POINTER(property);

	m_property = PropertyFactory::create(property,t);
	CHECK_POINTER(m_property);

}
INDIPropertyBuilder& INDIPropertyBuilder::device(IsoString device){
	m_property->setDeviceName(device);
	return *this;
}

INDIPropertyBuilder& INDIPropertyBuilder::property(IsoString property){
	m_property->setName(property);
	return *this;
}

INDIPropertyBuilder& INDIPropertyBuilder::addElement(IsoString elementName,IsoString value){
	m_property->addElement(elementName,value);
	return *this;
}



} /* namespace pcl */
