/*
 * FakeBaseClient.h
 *
 *  Created on: May 28, 2014
 *      Author: klaus
 */

#ifndef FAKEINDICLIENT_H_
#define FAKEINDICLIENT_H_

#include "PixInsightINDIclient.h"
#include "FakePixInsightINDIInstance.h"
namespace pcl {

class FakeINDIClient: public INDIClient {
public:
	FakeINDIClient(IPixInsightINDIInstance* instance):INDIClient(instance,false /*initThreads*/){}
	virtual ~FakeINDIClient(){}
	virtual void sendNewSwitch (const char * deviceName, const char *propertyName, const char *elementName);
	virtual void sendNewNumber (const char * deviceName, const char *propertyName, const char *elementName, double value);
	virtual void sendNewText (const char * deviceName, const char *propertyName, const char *elementName, const char* text);
	virtual void removeSwitch(const char * deviceName, const char *propertyName, const char *elementName);
	virtual void removeNumber(const char * deviceName, const char *propertyName, const char *elementName,double value);
	virtual void removeText(const char * deviceName, const char *propertyName, const char *elementName,const char* text);
	virtual bool connectServer();
	virtual void setBLOBMode(BLOBHandling blobH, const char *dev, const char *prop = NULL){}
	virtual bool isWatched(char const* device){return true;}


};

} /* namespace pcl */
#endif /* FAKEINDICLIENT_H_ */
