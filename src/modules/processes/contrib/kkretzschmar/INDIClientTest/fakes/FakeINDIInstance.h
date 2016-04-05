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
	virtual String CurrentServerMessage() const {return m_currentMessage;}
	virtual void SetCurrentServerMessage(const String& message) {m_currentMessage = message;}

	virtual String DownloadedImagePath() const {return m_downloadedImagePath;}
	virtual void SetDownloadedImagePath( const String& path) {m_downloadedImagePath=path;}

	virtual void setImageDownloadedFlag(bool flag){}
	virtual bool getImageDownloadedFlag(){return true;}

private:
	Array<INDIDeviceListItem>    m_deviceList;
	Array<INDIPropertyListItem>  m_propertyList;
	ExclPropertyList             m_exclusivePropertyList;
	String                       m_currentMessage;
	String                       m_downloadedImagePath;
};

} /* namespace pcl */
#endif /* FAKEPIXINSIGHTINDIINSTANCE_H_ */
