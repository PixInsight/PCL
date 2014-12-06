/*
 * PropertyVisitorTest.cpp
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../PropertyNode.h"
#include <pcl/TreeBox.h>

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnNew;
using ::testing::Invoke;
using ::testing::NiceMock;
namespace pcl {

void PrintTo(const IsoString& value, ::std::ostream* os) {
	*os <<value.c_str();
}

class FakePropertyNode : public PropertyNode {
public:


	virtual TreeBox::Node* createTreeBoxNode1(TreeBox::Node* ){
		return NULL;
	}
	virtual TreeBox::Node* createTreeBoxNode0(){
		return NULL;
	}
	virtual TreeBox::Node* getTreeBoxNode( ){
		return NULL;
	}
	virtual void addTreeBoxNodeToParent(PropertyNode*){

	}

	virtual void setNodeINDIText(IsoString text){m_INDIText=text;}
	virtual void setNodeINDIValue(IsoString value){m_INDIValue=value;}
	virtual void setNodeINDIType(IsoString type){m_INDIType=type;}
	virtual void setNodeINDIState(int state){}
	virtual void setNodeINDINumberFormat(int numberFormat){}

	virtual IsoString getNodeINDIText() const {return m_INDIText; }
	virtual IsoString getNodeINDIValue() const {return m_INDIValue;}
	virtual IsoString getNodeINDIType() const {return m_INDIType;}

private:
	IsoString m_INDIText;
	IsoString m_INDIValue;
	IsoString m_INDIType;
};

class MockPropertyNode : public PropertyNode {
public:

	MockPropertyNode():PropertyNode(){
		ON_CALL(*this,createTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode0));
		ON_CALL(*this,createTreeBoxNode(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode1));
		ON_CALL(*this,getTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getTreeBoxNode));
		ON_CALL(*this,addTreeBoxNodeToParent(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::addTreeBoxNodeToParent));
		ON_CALL(*this,setNodeINDIText(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIText));
		ON_CALL(*this,setNodeINDIValue(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIValue));
		ON_CALL(*this,setNodeINDIType(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIType));
		ON_CALL(*this,setNodeINDIState(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIState));
		ON_CALL(*this,setNodeINDINumberFormat(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDINumberFormat));
		ON_CALL(*this,getNodeINDIText()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIText));
		ON_CALL(*this,getNodeINDIValue()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIValue));
		ON_CALL(*this,getNodeINDIType()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIType));
	}

	MockPropertyNode(IsoString device):PropertyNode(device){
		ON_CALL(*this,createTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode0));
		ON_CALL(*this,createTreeBoxNode(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode1));
		ON_CALL(*this,getTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getTreeBoxNode));
		ON_CALL(*this,addTreeBoxNodeToParent(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::addTreeBoxNodeToParent));
		ON_CALL(*this,setNodeINDIText(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIText));
		ON_CALL(*this,setNodeINDIValue(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIValue));
		ON_CALL(*this,setNodeINDIType(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIType));
		ON_CALL(*this,setNodeINDIState(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIState));
		ON_CALL(*this,setNodeINDINumberFormat(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDINumberFormat));
		ON_CALL(*this,getNodeINDIText()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIText));
		ON_CALL(*this,getNodeINDIValue()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIValue));
		ON_CALL(*this,getNodeINDIType()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIType));
	}

	MOCK_METHOD1(setTreeBoxNode,void(TreeBox::Node*));
	MOCK_METHOD0(getTreeBoxNode,TreeBox::Node*(void));
	MOCK_METHOD1(addTreeBoxNodeToParent,void(PropertyNode*));
	MOCK_METHOD0(createTreeBoxNode,TreeBox::Node*(void));
	MOCK_METHOD1(createTreeBoxNode,TreeBox::Node*(TreeBox::Node*));
	MOCK_METHOD0(getNodeINDIText,IsoString(void));
	MOCK_METHOD0(getNodeINDIValue,IsoString(void));
	MOCK_METHOD0(getNodeINDIType,IsoString(void));
	MOCK_METHOD1(setNodeINDIText,void(IsoString));
	MOCK_METHOD1(setNodeINDIValue,void(IsoString));
	MOCK_METHOD1(setNodeINDIType,void(IsoString));
	MOCK_METHOD1(setNodeINDIState,void(int));
	MOCK_METHOD1(setNodeINDINumberFormat,void(int));

private:
	FakePropertyNode m_fake;
};


class MockPropertyNodeFactory : public PropertyNodeFactory {
public:
	MOCK_METHOD1(create,PropertyNode*(IsoString));
};



class PropertyTreeTest : public testing::Test {

public:



};

TEST_F(PropertyTreeTest, TestFixture)
{

}

TEST_F(PropertyTreeTest, TestPropertyUtils){

	IsoString testString("/Device/Property/Element");

	IsoString deviceStr=PropertyUtils::getDevice(testString);
	EXPECT_STREQ("Device",deviceStr.c_str());

	IsoString propertyStr=PropertyUtils::getProperty(testString);
	EXPECT_STREQ("Property",propertyStr.c_str());

	IsoString elementStr=PropertyUtils::getElement(testString);
	EXPECT_STREQ("Element",elementStr.c_str());

	IsoString testString2("/Device/Property");
	elementStr=PropertyUtils::getElement(testString2);
	EXPECT_STREQ("/",elementStr.c_str());


}

TEST_F(PropertyTreeTest, TestPropertyUtilsNumberFormat){

	IsoString testString("3.23456676");
	IsoString formattedStr = PropertyUtils::getFormattedNumber(testString,IsoString("%4.2f"));
	EXPECT_STREQ("3.23",formattedStr.c_str());

	IsoString formattedStr2 = PropertyUtils::getFormattedNumber(testString,IsoString("%4.3m"));
	EXPECT_STREQ("3:14",formattedStr2.c_str());

	IsoString formattedStr3 = PropertyUtils::getFormattedNumber(testString,IsoString("%7.3m"));
	EXPECT_STREQ("   3:14",formattedStr3.c_str());

	IsoString testString2("12.238");
	IsoString formattedStr4 = PropertyUtils::getFormattedNumber(testString2,IsoString("%7.5m"));
	EXPECT_STREQ("12:14.2",formattedStr4.c_str());

	IsoString formattedStr5 = PropertyUtils::getFormattedNumber(testString,IsoString("%7.5m"));
	EXPECT_STREQ(" 3:14.0",formattedStr5.c_str());

	IsoString formattedStr6 = PropertyUtils::getFormattedNumber(testString,IsoString("%8.6m"));
	EXPECT_STREQ(" 3:14:04",formattedStr6.c_str());

	IsoString formattedStr7 = PropertyUtils::getFormattedNumber(testString,IsoString("%10.8m"));
	EXPECT_STREQ(" 3:14:04.4",formattedStr7.c_str());

	IsoString formattedStr8 = PropertyUtils::getFormattedNumber(testString,IsoString("%11.9m"));
	EXPECT_STREQ(" 3:14:04.44",formattedStr8.c_str());



}

TEST_F(PropertyTreeTest, TestCreatePropertyNode)
{
	PropertyNode* propRootNode   = new PropertyNode();

	PropertyNode* propDeviceNode = new PropertyNode(PropertyUtils::getKey(IsoString("Device")));
	propDeviceNode->addToParentNode(propRootNode);

	FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
	propRootNode->accept(findNodeVisitor,IsoString("/Device"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Devic"),IsoString(""));
	EXPECT_FALSE(findNodeVisitor->foundNode());

	PropertyNode* propPropertyNode = new PropertyNode(PropertyUtils::getKey(IsoString("Device"),IsoString("Property")));
	propPropertyNode->addToParentNode(propDeviceNode);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Device/Propert"),IsoString(""));
	EXPECT_FALSE(findNodeVisitor->foundNode());

	PropertyNode* propElementNode = new PropertyNode(PropertyUtils::getKey(IsoString("Device"), IsoString("Property"), IsoString("Element")));
	propElementNode->addToParentNode(propPropertyNode);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor, IsoString("/Device/Property/Element"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor, IsoString("/Device/Propert/Element"),IsoString(""));
	EXPECT_FALSE(findNodeVisitor->foundNode());

}


TEST_F(PropertyTreeTest,TestPropertyTreeAddNode){

	NiceMock<MockPropertyNode>* propRootNode   = new NiceMock<MockPropertyNode>();

	MockPropertyNodeFactory factory;

	PropertyTree*  propTree = new PropertyTree(propRootNode,&factory);

	EXPECT_CALL(factory,create(IsoString("/Device"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device")));
	PropertyNode* deviceNode = propTree->addNode(IsoString("Device"));

	FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
	propRootNode->accept(findNodeVisitor,IsoString("/Device"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	findNodeVisitor->reset();

	EXPECT_CALL(factory,create(IsoString("/Device/Property"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property")));
	PropertyNode* propNode = propTree->addNode(deviceNode,IsoString("Device"),IsoString("Property"));

	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();

	EXPECT_CALL(factory,create(IsoString("/Device/Property/Element"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property/Element")));
	PropertyNode* elemNode = propTree->addNode(propNode,IsoString("Device"),IsoString("Property"),IsoString("Element"));

	EXPECT_TRUE(elemNode!=NULL);

	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property/Element"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	delete propTree;
}

TEST_F(PropertyTreeTest,TestPropertyTreeLeafs){

	NiceMock<MockPropertyNode>* propRootNode   = new NiceMock<MockPropertyNode>();

	MockPropertyNodeFactory factory;

	PropertyTree  propTree(propRootNode,&factory);

	EXPECT_CALL(factory,create(IsoString("/Device"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device")));
	EXPECT_CALL(factory,create(IsoString("/Device/Property"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property")));
	EXPECT_CALL(factory,create(IsoString("/Device/Property/Element"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property/Element")));

	PropertyNode* elemNode = propTree.addElementNode(IsoString("Device"),IsoString("Property"),IsoString("Element"));

	EXPECT_TRUE(elemNode!=NULL);

	FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property/Element"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	EXPECT_CALL(factory,create(IsoString("/Device"))).Times(0);
	EXPECT_CALL(factory,create(IsoString("/Device/Property"))).Times(0);
	EXPECT_CALL(factory,create(IsoString("/Device/Property/Element2"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property/Element2")));

	elemNode = propTree.addElementNode(IsoString("Device"),IsoString("Property"),IsoString("Element2"));

	EXPECT_TRUE(elemNode!=NULL);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property/Element2"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	EXPECT_CALL(factory,create(IsoString("/Device"))).Times(0);
	EXPECT_CALL(factory,create(IsoString("/Device/Property2"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property2")));
	EXPECT_CALL(factory,create(IsoString("/Device/Property2/Element2"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property2/Element2")));

	elemNode = propTree.addElementNode(IsoString("Device"),IsoString("Property2"),IsoString("Element2"));

	EXPECT_TRUE(elemNode!=NULL);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property2/Element2"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	EXPECT_CALL(factory,create(IsoString("/Device"))).Times(0);
	EXPECT_CALL(factory,create(IsoString("/Device/Property2"))).Times(0);
	EXPECT_CALL(factory,create(IsoString("/Device/Property2/Element2"))).Times(0);

	PropertyNode* elemNode2 = propTree.addElementNode(IsoString("Device"),IsoString("Property2"),IsoString("Element2"));

	EXPECT_TRUE(elemNode==elemNode2);

	// check node names
	MockPropertyNode* foundNode;
	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Device"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	foundNode = dynamic_cast<MockPropertyNode*>(findNodeVisitor->getNode());
	EXPECT_STREQ(foundNode->getNodeINDIText().c_str(),"Device");

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property2"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	foundNode = dynamic_cast<MockPropertyNode*>(findNodeVisitor->getNode());
	EXPECT_STREQ(foundNode->getNodeINDIText().c_str(),"Property2");

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property2/Element2"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	foundNode = dynamic_cast<MockPropertyNode*>(findNodeVisitor->getNode());
	EXPECT_STREQ(foundNode->getNodeINDIText().c_str(),"Element2");

}


} /* namespace pcl*/
