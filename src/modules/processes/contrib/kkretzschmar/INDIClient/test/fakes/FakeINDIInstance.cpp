/*
 * FakePixInsightINDIInstance.cpp
 *
 *  Created on: May 26, 2014
 *      Author: klaus
 */

#include "FakeINDIInstance.h"

namespace pcl {

FakeINDIInstance::FakeINDIInstance(): m_deviceList(),m_propertyList(),m_exclusivePropertyList(m_propertyList), m_currentMessage("") {
	// TODO Auto-generated constructor stub

}

FakeINDIInstance::~FakeINDIInstance() {
	// TODO Auto-generated destructor stub
}

} /* namespace pcl */
