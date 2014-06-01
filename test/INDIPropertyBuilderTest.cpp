/*
 * INDIPropertyBuilderTest.cpp
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../INDIPropertyBuilder.h"
#include <pcl/Exception.h>
namespace pcl {

class INDIPropertyBuilderTest : public testing::Test {
protected:

};

TEST_F(INDIPropertyBuilderTest, TestFixture)
{

}

TEST_F(INDIPropertyBuilderTest, CreateSwitchProperty)
{
	INDIPropertyBuilder builder(INDI_SWITCH);
	builder.device("TestDevice").property("TestProperty").addElement("TestElement","ON");
	EXPECT_STREQ(IsoString(builder.getProperty()->getDeviceName()).c_str(),"TestDevice");
	EXPECT_STREQ(IsoString(builder.getProperty()->getName()).c_str(),"TestProperty");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementName(0)).c_str(),"TestElement");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementValue(0)).c_str(),"ON");
}

TEST_F(INDIPropertyBuilderTest, AccessSwitchPropertyElementFailed)
{
	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_SWITCH); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementName(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_SWITCH); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementLabel(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_SWITCH); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementValue(0); },
			Error);

}

TEST_F(INDIPropertyBuilderTest, CreateNumberProperty)
{
	INDIPropertyBuilder builder(INDI_NUMBER);
	builder.device("TestDevice").property("TestProperty").addElement("TestElement","1.5");
	EXPECT_STREQ(IsoString(builder.getProperty()->getDeviceName()).c_str(),"TestDevice");
	EXPECT_STREQ(IsoString(builder.getProperty()->getName()).c_str(),"TestProperty");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementName(0)).c_str(),"TestElement");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementValue(0)).c_str(),"1.5");
}

TEST_F(INDIPropertyBuilderTest, AccessNumberPropertyElementFailed)
{
	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_NUMBER); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementName(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_NUMBER); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementLabel(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_NUMBER); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementValue(0); },
			Error);

}

TEST_F(INDIPropertyBuilderTest, CreateTextProperty)
{
	INDIPropertyBuilder builder(INDI_TEXT);
	builder.device("TestDevice").property("TestProperty").addElement("TestElement","Value");
	EXPECT_STREQ(IsoString(builder.getProperty()->getDeviceName()).c_str(),"TestDevice");
	EXPECT_STREQ(IsoString(builder.getProperty()->getName()).c_str(),"TestProperty");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementName(0)).c_str(),"TestElement");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementValue(0)).c_str(),"Value");
}


TEST_F(INDIPropertyBuilderTest, AccessTextPropertyElementFailed)
{
	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_TEXT); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementName(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_TEXT); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementLabel(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_TEXT); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementValue(0); },
			Error);


}

TEST_F(INDIPropertyBuilderTest, CreateLightProperty)
{
	INDIPropertyBuilder builder(INDI_LIGHT);
	builder.device("TestDevice").property("TestProperty").addElement("TestElement","IDLE");
	EXPECT_STREQ(IsoString(builder.getProperty()->getDeviceName()).c_str(),"TestDevice");
	EXPECT_STREQ(IsoString(builder.getProperty()->getName()).c_str(),"TestProperty");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementName(0)).c_str(),"TestElement");
	EXPECT_STREQ(IsoString(builder.getProperty()->getElementValue(0)).c_str(),"IDLE");
}

TEST_F(INDIPropertyBuilderTest, AccessLightPropertyElementFailed)
{
	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_LIGHT); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementName(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_LIGHT); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementLabel(0); },
			Error);

	EXPECT_THROW(
			{ INDIPropertyBuilder builder(INDI_LIGHT); builder.device("TestDevice").property("TestProperty"); builder.getProperty()->getElementValue(0); },
			Error);


}

} /* namespace pcl*/
