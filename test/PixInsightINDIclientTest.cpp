#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakes/FakePixInsightINDIInstance.h"
#include "fakes/FakeINDIClient.h"


using namespace pcl;

namespace pcl {
	void mySendNewNumber(const char * deviceName, const char *propertyName, const char *elementName){

	}
}

class PixInsightINDIClientTest : public testing::Test {
protected:

};




TEST_F(PixInsightINDIClientTest,TestFixture)
{

}

TEST_F(PixInsightINDIClientTest,TestINDIClientCreationFailed)
{
	FakePixInsightINDIInstance* pInstance= NULL;
	EXPECT_THROW(INDIClient iclient(pInstance,false),FatalError);
}

TEST_F(PixInsightINDIClientTest,TestINDIClientCreation)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);
}


TEST_F(PixInsightINDIClientTest,TestNewProperties)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);

	iclient.connectServer();
	Array<INDIPropertyListItem> properties = pInstance->getPropertyList();
	EXPECT_EQ( properties.Length(),(size_t)4);
	EXPECT_STREQ(IsoString(properties[0].Device).c_str(),"TestDevice");
	EXPECT_STREQ(IsoString(properties[0].Property).c_str(),"TestSwitchProperty");
	EXPECT_STREQ(IsoString(properties[0].Element).c_str(),"TestElement");
	EXPECT_STREQ(IsoString(properties[0].PropertyValue).c_str(),"OFF");
	EXPECT_STREQ(IsoString(properties[0].PropertyTypeStr).c_str(),"INDI_SWITCH");
	EXPECT_STREQ(IsoString(properties[0].PropertyKey).c_str(),"/TestDevice/TestSwitchProperty/TestElement");
	EXPECT_TRUE(properties[0].PropertyFlag==Insert);

	EXPECT_STREQ(IsoString(properties[1].Device).c_str(), "TestDevice");
	EXPECT_STREQ(IsoString(properties[1].Property).c_str(),"TestNumberProperty");
	EXPECT_STREQ(IsoString(properties[1].Element).c_str(), "TestElement");
	EXPECT_STREQ(IsoString(properties[1].PropertyValue).c_str(), "1.5");
	EXPECT_STREQ(IsoString(properties[1].PropertyTypeStr).c_str(),"INDI_NUMBER");
	EXPECT_STREQ(IsoString(properties[1].PropertyKey).c_str(),"/TestDevice/TestNumberProperty/TestElement");
	EXPECT_TRUE(properties[1].PropertyFlag==Insert);

	EXPECT_STREQ(IsoString(properties[2].Device).c_str(), "TestDevice");
	EXPECT_STREQ(IsoString(properties[2].Property).c_str(),"TestTextProperty");
	EXPECT_STREQ(IsoString(properties[2].Element).c_str(), "TestElement");
	EXPECT_STREQ(IsoString(properties[2].PropertyValue).c_str(), "value");
	EXPECT_STREQ(IsoString(properties[2].PropertyTypeStr).c_str(),"INDI_TEXT");
	EXPECT_STREQ(IsoString(properties[2].PropertyKey).c_str(),"/TestDevice/TestTextProperty/TestElement");
	EXPECT_TRUE(properties[2].PropertyFlag==Insert);

	EXPECT_STREQ(IsoString(properties[3].Device).c_str(), "TestDevice");
	EXPECT_STREQ(IsoString(properties[3].Property).c_str(),"TestLightProperty");
	EXPECT_STREQ(IsoString(properties[3].Element).c_str(), "TestElement");
	EXPECT_STREQ(IsoString(properties[3].PropertyValue).c_str(), "IDLE");
	EXPECT_STREQ(IsoString(properties[3].PropertyTypeStr).c_str(),"INDI_LIGHT");
	EXPECT_STREQ(IsoString(properties[3].PropertyKey).c_str(),"/TestDevice/TestLightProperty/TestElement");
	EXPECT_TRUE(properties[3].PropertyFlag==Insert);
}

TEST_F(PixInsightINDIClientTest,TestNewSwitch)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);

	iclient.connectServer();
	Array<INDIPropertyListItem> properties = pInstance->getPropertyList();
	EXPECT_EQ( properties.Length(), (size_t)4);

	iclient.sendNewSwitch("TestDevice","TestSwitchProperty","TestElement");
	EXPECT_STREQ(IsoString(properties[0].PropertyValue).c_str(),"ON");
}

TEST_F(PixInsightINDIClientTest,TestNewNumber)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);

	iclient.connectServer();
	Array<INDIPropertyListItem> properties = pInstance->getPropertyList();
	EXPECT_EQ( properties.Length(), (size_t)4);

	iclient.sendNewNumber("TestDevice","TestNumberProperty","TestElement",1.0);
	EXPECT_STREQ(IsoString(properties[1].PropertyValue).c_str(),"1");
}

TEST_F(PixInsightINDIClientTest,TestNewText)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);

	iclient.connectServer();
	Array<INDIPropertyListItem> properties = pInstance->getPropertyList();
	EXPECT_EQ( properties.Length(), (size_t)4);

	iclient.sendNewText("TestDevice","TestTextProperty","TestElement","newValue");
	EXPECT_STREQ(IsoString(properties[2].PropertyValue).c_str(),"newValue");
}

TEST_F(PixInsightINDIClientTest,TestMarkForRemovalText)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);

	iclient.connectServer();
	Array<INDIPropertyListItem> properties = pInstance->getPropertyList();
	EXPECT_EQ( properties.Length(), (size_t)4);

	iclient.removeText("TestDevice","TestTextProperty","TestElement","newValue");
	EXPECT_STREQ(IsoString(properties[2].PropertyValue).c_str(),"newValue");
	EXPECT_TRUE(properties[2].PropertyFlag==Remove);
}

TEST_F(PixInsightINDIClientTest,TestMarkForRemovalNumber)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);

	iclient.connectServer();
	Array<INDIPropertyListItem> properties = pInstance->getPropertyList();
	EXPECT_EQ( properties.Length(), (size_t)4);

	iclient.removeNumber("TestDevice","TestNumberProperty","TestElement",1.0);
	EXPECT_STREQ(IsoString(properties[1].PropertyValue).c_str(),"1");
	EXPECT_TRUE(properties[1].PropertyFlag==Remove);
}

TEST_F(PixInsightINDIClientTest,TestMarkForRemovalSwitch)
{
	FakePixInsightINDIInstance* pInstance = new FakePixInsightINDIInstance();
	FakeINDIClient iclient(pInstance);

	iclient.connectServer();
	Array<INDIPropertyListItem> properties = pInstance->getPropertyList();
	EXPECT_EQ( properties.Length(), (size_t)4);

	iclient.removeSwitch("TestDevice","TestSwitchProperty","TestElement");
	EXPECT_STREQ(IsoString(properties[0].PropertyValue).c_str(),"ON");
	EXPECT_TRUE(properties[0].PropertyFlag==Remove);
}
