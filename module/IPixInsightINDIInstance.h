/*
 * IPixInsightINDIInstance.h
 *
 *  Created on: May 26, 2014
 *      Author: klaus
 */

#ifndef IPIXINSIGHTINDIINSTANCE_H_
#define IPIXINSIGHTINDIINSTANCE_H_

#include <pcl/Array.h>
#include <pcl/String.h>
#include "PixInsightINDIParamListTypes.h"

namespace pcl {

class IPixInsightINDIInstance {
public:
	virtual ~IPixInsightINDIInstance(){}
	virtual Array<INDIPropertyListItem>& getPropertyList() = 0;
	virtual Array<INDIDeviceListItem>& getDeviceList() = 0;
	virtual IsoString& getCurrentMessage() = 0;
	virtual void setImageDownloadedFlag(bool flag) =0 ;
	virtual bool getImageDownloadedFlag()=0;
};

} /* namespace pcl */
#endif /* IPIXINSIGHTINDIINSTANCE_H_ */
