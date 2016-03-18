/*
 * PropertyVisitorTest.cpp
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "PropertyNode.h"
#include <pcl/TreeBox.h>

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnNew;
using ::testing::Invoke;
using ::testing::NiceMock;
namespace pcl {

void PrintTo(const String& value, ::std::ostream* os) {
	*os <<value.ToIsoString().c_str();
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

	virtual void setNodeINDIText(String text){m_INDIText=text;}
	virtual void setNodeINDIValue(String value){m_INDIValue=value;}
	virtual void setNodeINDIType(String type){m_INDIType=type;}
	virtual void setNodeINDIState(int state){}
	virtual void setNodeINDINumberFormat(String numberFormat){}
	virtual void setNodeINDILabel(String label){}


	virtual String getNodeINDIText() const {return m_INDIText; }
	virtual String getNodeINDIValue() const {return m_INDIValue;}
	virtual String getNodeINDIType() const {return m_INDIType;}

private:
	String m_INDIText;
	String m_INDIValue;
	String m_INDIType;
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
		ON_CALL(*this,setNodeINDILabel(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDILabel));
		ON_CALL(*this,getNodeINDIText()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIText));
		ON_CALL(*this,getNodeINDIValue()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIValue));
		ON_CALL(*this,getNodeINDIType()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIType));
	}

	MockPropertyNode(String device):PropertyNode(device){
		ON_CALL(*this,createTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode0));
		ON_CALL(*this,createTreeBoxNode(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode1));
		ON_CALL(*this,getTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getTreeBoxNode));
		ON_CALL(*this,addTreeBoxNodeToParent(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::addTreeBoxNodeToParent));
		ON_CALL(*this,setNodeINDIText(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIText));
		ON_CALL(*this,setNodeINDIValue(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIValue));
		ON_CALL(*this,setNodeINDIType(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIType));
		ON_CALL(*this,setNodeINDIState(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDIState));
		ON_CALL(*this,setNodeINDINumberFormat(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDINumberFormat));
		ON_CALL(*this,setNodeINDILabel(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::setNodeINDILabel));
		ON_CALL(*this,getNodeINDIText()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIText));
		ON_CALL(*this,getNodeINDIValue()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIValue));
		ON_CALL(*this,getNodeINDIType()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getNodeINDIType));
	}

	MOCK_METHOD1(setTreeBoxNode,void(TreeBox::Node*));
	MOCK_METHOD0(getTreeBoxNode,TreeBox::Node*(void));
	MOCK_METHOD1(addTreeBoxNodeToParent,void(PropertyNode*));
	MOCK_METHOD0(createTreeBoxNode,TreeBox::Node*(void));
	MOCK_METHOD1(createTreeBoxNode,TreeBox::Node*(TreeBox::Node*));
	MOCK_METHOD0(getNodeINDIText,String(void));
	MOCK_METHOD0(getNodeINDIValue,String(void));
	MOCK_METHOD0(getNodeINDIType,String(void));
	MOCK_METHOD1(setNodeINDIText,void(String));
	MOCK_METHOD1(setNodeINDIValue,void(String));
	MOCK_METHOD1(setNodeINDIType,void(String));
	MOCK_METHOD1(setNodeINDIState,void(int));
	MOCK_METHOD1(setNodeINDINumberFormat,void(String));
	MOCK_METHOD1(setNodeINDILabel,void(String));

private:
	FakePropertyNode m_fake;
};


class MockPropertyNodeFactory : public PropertyNodeFactory {
public:
	MOCK_METHOD1(create,PropertyNode*(String));
};



class PropertyTreeTest : public testing::Test {

public:



};

TEST_F(PropertyTreeTest, TestFixture)
{

}

TEST_F(PropertyTreeTest, TestPropertyUtils){

	String testString("/Device/Property/Element");

	String deviceStr=PropertyUtils::getDevice(testString);
	EXPECT_STREQ("Device",deviceStr.ToIsoString().c_str());

	String propertyStr=PropertyUtils::getProperty(testString);
	EXPECT_STREQ("Property",propertyStr.ToIsoString().c_str());

	String elementStr=PropertyUtils::getElement(testString);
	EXPECT_STREQ("Element",elementStr.ToIsoString().c_str());

	String testString2("/Device/Property");
	elementStr=PropertyUtils::getElement(testString2);
	EXPECT_STREQ("/",elementStr.ToIsoString().c_str());
}

TEST_F(PropertyTreeTest, TestPropertyUtilsNumberFormat){

	String testString("3.23456676");
	String formattedStr = PropertyUtils::getFormattedNumber(testString,IsoString("%4.2f"));
	EXPECT_STREQ("3.23",formattedStr.ToIsoString().c_str());

	String formattedStr2 = PropertyUtils::getFormattedNumber(testString,IsoString("%4.3m"));
	EXPECT_STREQ("3:14",formattedStr2.ToIsoString().c_str());

	String formattedStr3 = PropertyUtils::getFormattedNumber(testString,IsoString("%7.3m"));
	EXPECT_STREQ("   3:14",formattedStr3.ToIsoString().c_str());

	String testString2("12.238");
	String formattedStr4 = PropertyUtils::getFormattedNumber(testString2,IsoString("%7.5m"));
	EXPECT_STREQ("12:14.2",formattedStr4.ToIsoString().c_str());

	String formattedStr5 = PropertyUtils::getFormattedNumber(testString,IsoString("%7.5m"));
	EXPECT_STREQ(" 3:14.0",formattedStr5.ToIsoString().c_str());

	String formattedStr6 = PropertyUtils::getFormattedNumber(testString,IsoString("%8.6m"));
	EXPECT_STREQ(" 3:14:04",formattedStr6.ToIsoString().c_str());

	String formattedStr7 = PropertyUtils::getFormattedNumber(testString,IsoString("%10.8m"));
	EXPECT_STREQ(" 3:14:04.4",formattedStr7.ToIsoString().c_str());

	String formattedStr8 = PropertyUtils::getFormattedNumber(testString,IsoString("%11.9m"));
	EXPECT_STREQ(" 3:14:04.44",formattedStr8.ToIsoString().c_str());



}

TEST_F(PropertyTreeTest, TestCreatePropertyNode)
{
	PropertyNode* propRootNode   = new PropertyNode();

	PropertyNode* propDeviceNode = new PropertyNode(PropertyUtils::getKey(String("Device")));
	propDeviceNode->addToParentNode(propRootNode);

	FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
	propRootNode->accept(findNodeVisitor,String("/Device"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Devic"),String(""));
	EXPECT_FALSE(findNodeVisitor->foundNode());

	PropertyNode* propPropertyNode = new PropertyNode(PropertyUtils::getKey(String("Device"),String("Property")));
	propPropertyNode->addToParentNode(propDeviceNode);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Device/Property"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Device/Propert"),String(""));
	EXPECT_FALSE(findNodeVisitor->foundNode());

	PropertyNode* propElementNode = new PropertyNode(PropertyUtils::getKey(String("Device"), String("Property"), String("Element")));
	propElementNode->addToParentNode(propPropertyNode);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor, String("/Device/Property/Element"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor, String("/Device/Propert/Element"),String(""));
	EXPECT_FALSE(findNodeVisitor->foundNode());

}


TEST_F(PropertyTreeTest,TestPropertyTreeAddNode){

	NiceMock<MockPropertyNode>* propRootNode   = new NiceMock<MockPropertyNode>();

	MockPropertyNodeFactory factory;

	PropertyTree*  propTree = new PropertyTree(propRootNode,&factory);

	EXPECT_CALL(factory,create(String("/Device"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device")));
	PropertyNode* deviceNode = propTree->addNode(String("Device"));

	FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
	propRootNode->accept(findNodeVisitor,String("/Device"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	findNodeVisitor->reset();

	EXPECT_CALL(factory,create(String("/Device/Property"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device/Property")));
	PropertyNode* propNode = propTree->addNode(deviceNode,String("Device"),String("Property"));

	propRootNode->accept(findNodeVisitor,String("/Device/Property"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();

	EXPECT_CALL(factory,create(String("/Device/Property/Element"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device/Property/Element")));
	PropertyNode* elemNode = propTree->addNode(propNode,String("Device"),String("Property"),String("Element"));

	EXPECT_TRUE(elemNode!=NULL);

	propRootNode->accept(findNodeVisitor,String("/Device/Property/Element"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	delete propTree;
}

TEST_F(PropertyTreeTest,TestPropertyTreeLeafs){

	NiceMock<MockPropertyNode>* propRootNode   = new NiceMock<MockPropertyNode>();

	MockPropertyNodeFactory factory;

	PropertyTree  propTree(propRootNode,&factory);

	EXPECT_CALL(factory,create(String("/Device"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device")));
	EXPECT_CALL(factory,create(String("/Device/Property"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device/Property")));
	EXPECT_CALL(factory,create(String("/Device/Property/Element"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device/Property/Element")));

	PropertyNode* elemNode = propTree.addElementNode(String("Device"),String("Property"),String("Element"));

	EXPECT_TRUE(elemNode!=NULL);

	FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
	propRootNode->accept(findNodeVisitor,String("/Device/Property/Element"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	EXPECT_CALL(factory,create(String("/Device"))).Times(0);
	EXPECT_CALL(factory,create(String("/Device/Property"))).Times(0);
	EXPECT_CALL(factory,create(String("/Device/Property/Element2"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device/Property/Element2")));

	elemNode = propTree.addElementNode(String("Device"),String("Property"),String("Element2"));

	EXPECT_TRUE(elemNode!=NULL);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Device/Property/Element2"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	EXPECT_CALL(factory,create(String("/Device"))).Times(0);
	EXPECT_CALL(factory,create(String("/Device/Property2"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device/Property2")));
	EXPECT_CALL(factory,create(String("/Device/Property2/Element2"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(String("/Device/Property2/Element2")));

	elemNode = propTree.addElementNode(String("Device"),String("Property2"),String("Element2"));

	EXPECT_TRUE(elemNode!=NULL);

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Device/Property2/Element2"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	EXPECT_CALL(factory,create(String("/Device"))).Times(0);
	EXPECT_CALL(factory,create(String("/Device/Property2"))).Times(0);
	EXPECT_CALL(factory,create(String("/Device/Property2/Element2"))).Times(0);

	PropertyNode* elemNode2 = propTree.addElementNode(String("Device"),String("Property2"),String("Element2"));

	EXPECT_TRUE(elemNode==elemNode2);

	// check node names
	MockPropertyNode* foundNode;
	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Device"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	foundNode = dynamic_cast<MockPropertyNode*>(findNodeVisitor->getNode());
	EXPECT_STREQ(foundNode->getNodeINDIText().ToIsoString().c_str(),"Device");

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Device/Property2"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	foundNode = dynamic_cast<MockPropertyNode*>(findNodeVisitor->getNode());
	EXPECT_STREQ(foundNode->getNodeINDIText().ToIsoString().c_str(),"Property2");

	findNodeVisitor->reset();
	propRootNode->accept(findNodeVisitor,String("/Device/Property2/Element2"),String(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	foundNode = dynamic_cast<MockPropertyNode*>(findNodeVisitor->getNode());
	EXPECT_STREQ(foundNode->getNodeINDIText().ToIsoString().c_str(),"Element2");

}


} /* namespace pcl*/
