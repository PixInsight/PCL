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
};

class MockPropertyNode : public PropertyNode {
public:

	MockPropertyNode():PropertyNode(){
		ON_CALL(*this,createTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode0));
		ON_CALL(*this,createTreeBoxNode(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode1));
		ON_CALL(*this,getTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getTreeBoxNode));
		ON_CALL(*this,addTreeBoxNodeToParent(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::addTreeBoxNodeToParent));
	}

	MockPropertyNode(IsoString device):PropertyNode(device){
		ON_CALL(*this,createTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode0));
		ON_CALL(*this,createTreeBoxNode(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::createTreeBoxNode1));
		ON_CALL(*this,getTreeBoxNode()).WillByDefault(Invoke(&m_fake,&FakePropertyNode::getTreeBoxNode));
		ON_CALL(*this,addTreeBoxNodeToParent(_)).WillByDefault(Invoke(&m_fake,&FakePropertyNode::addTreeBoxNodeToParent));
	}

	MOCK_METHOD1(setTreeBoxNode,void(TreeBox::Node*));
	MOCK_METHOD0(getTreeBoxNode,TreeBox::Node*(void));
	MOCK_METHOD1(addTreeBoxNodeToParent,void(PropertyNode*));
	MOCK_METHOD0(createTreeBoxNode,TreeBox::Node*(void));
	MOCK_METHOD1(createTreeBoxNode,TreeBox::Node*(TreeBox::Node*));


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
	propTree->addNode(IsoString("Device"));

	FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
	propRootNode->accept(findNodeVisitor,IsoString("/Device"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());
	findNodeVisitor->reset();

	EXPECT_CALL(factory,create(IsoString("/Device/Property"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property")));
	propTree->addNode(IsoString("Device"),IsoString("Property"));

	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	findNodeVisitor->reset();

	EXPECT_CALL(factory,create(IsoString("/Device/Property/Element"))).Times(1).WillOnce(testing::ReturnNew<NiceMock<MockPropertyNode> >(IsoString("/Device/Property/Element")));
	propTree->addNode(IsoString("Device"),IsoString("Property"),IsoString("Element"));

	propRootNode->accept(findNodeVisitor,IsoString("/Device/Property/Element"),IsoString(""));
	EXPECT_TRUE(findNodeVisitor->foundNode());

	delete propTree;
}



} /* namespace pcl*/
