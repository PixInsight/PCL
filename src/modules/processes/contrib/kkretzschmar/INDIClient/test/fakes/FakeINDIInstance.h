/*
 * FakePixInsightINDIInstance.h
 *
 *  Created on: May 26, 2014
 *      Author: klaus
 */

#ifndef FAKEPIXINSIGHTINDIINSTANCE_H_
#define FAKEPIXINSIGHTINDIINSTANCE_H_

#include "IINDIDeviceControllerInstance.h"


namespace pcl {

class FakeINDIInstance: public IINDIDeviceControllerInstance {
public:
	FakeINDIInstance();
	virtual ~FakeINDIInstance();

	virtual ExclPropertyList getExclusivePropertyList() {return m_exclusivePropertyList;};
	virtual Array<INDIPropertyListItem>& getPropertyList() {return m_propertyList;}
	virtual Array<INDIDeviceListItem>& getDeviceList(){return m_deviceList; }
	virtual IsoString& getCurrentMessage() {return m_currentMessage;}
	virtual void setImageDownloadedFlag(bool flag){}
	virtual bool getImageDownloadedFlag(){return true;}

private:
	Array<INDIDeviceListItem>    m_deviceList;
	Array<INDIPropertyListItem>  m_propertyList;
	ExclPropertyList             m_exclusivePropertyList;
	IsoString                    m_currentMessage;
};

} /* namespace pcl */
#endif /* FAKEPIXINSIGHTINDIINSTANCE_H_ */
