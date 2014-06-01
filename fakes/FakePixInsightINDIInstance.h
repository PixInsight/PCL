/*
 * FakePixInsightINDIInstance.h
 *
 *  Created on: May 26, 2014
 *      Author: klaus
 */

#ifndef FAKEPIXINSIGHTINDIINSTANCE_H_
#define FAKEPIXINSIGHTINDIINSTANCE_H_

#include "../IPixInsightINDIInstance.h"


namespace pcl {

class FakePixInsightINDIInstance: public IPixInsightINDIInstance {
public:
	FakePixInsightINDIInstance();
	virtual ~FakePixInsightINDIInstance();

	virtual Array<INDIPropertyListItem>& getPropertyList() {return m_propertyList;}
	virtual Array<INDIDeviceListItem>& getDeviceList(){return m_deviceList; }
	virtual IsoString& getCurrentMessage() {return m_currentMessage;}

private:
	Array<INDIDeviceListItem>    m_deviceList;
	Array<INDIPropertyListItem>  m_propertyList;
	IsoString                    m_currentMessage;
};

} /* namespace pcl */
#endif /* FAKEPIXINSIGHTINDIINSTANCE_H_ */
