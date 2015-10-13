//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// PropertyNode.cpp - Released 2015/10/13 15:55:45 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2015 Klaus Kretzschmar
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#include "PropertyNode.h"
#include <assert.h>
#include <algorithm>
#include <pcl/Math.h>

namespace pcl {

	String PropertyUtils::getDevice(String keyString){
		size_t startpos = keyString.Find("/");
		size_t endpos   = keyString.Find("/",1);
		return keyString.Substring(startpos+1,endpos-startpos-1);
	}

	String PropertyUtils::getProperty(String keyString){
		size_t startpos = keyString.Find("/",1);
		size_t endpos = keyString.Find("/",startpos+1);
		return keyString.Substring(startpos+1,endpos-startpos-1);
	}

	String PropertyUtils::getElement(String keyString){
		size_t startpos1 = keyString.Find("/",1);
		size_t startpos2 = keyString.Find("/",startpos1+1);
		size_t endpos = keyString.Find("/",startpos2+1);
		return keyString.Substring(startpos2+1,endpos-startpos2);
	}

	String PropertyUtils::getFormattedNumber(String numberStr, IsoString numberFormat){
		size_t im = numberFormat.Find('m');
		if (im!=String::notFound){
			numberFormat.DeleteRight(im);
			numberFormat.DeleteLeft(1);
			StringList tokens;
			numberFormat.Break(tokens,'.',true);
			size_t fraction = tokens[1].ToInt();
			size_t width    = tokens[0].ToInt()-fraction;
			assert(width>0);
			int hours = Trunc(numberStr.ToFloat());
			switch (fraction) {
			case 3:
			{
				int minutes = Trunc((numberStr.ToFloat() - hours)*60);
				IsoString formatStr = '%' + IsoString().Format("%dd",width) + ":%02d";
				return String().Format(formatStr.c_str(),hours,Abs(minutes));
			}
			case 5:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int minutesfrac = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*10);
				IsoString formatStr = '%' + IsoString().Format("%dd",width) + ":%02d.%d";
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(minutesfrac));
			}
			case 6:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int seconds     = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*60);
				IsoString formatStr = '%' + IsoString().Format("%dd",width) + ":%02d:%02d";
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(seconds));
			}
			case 8:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int seconds     = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*60);
				int secondsfrac = Trunc((((numberStr.ToFloat() - hours)*60-minutes)*60-seconds)*10);
				IsoString formatStr = '%' + IsoString().Format("%dd",width) + ":%02d:%02d.%d";
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(seconds),Abs(secondsfrac));
			}
			case 9:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int seconds     = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*60);
				int secondsfrac = Trunc((((numberStr.ToFloat() - hours)*60-minutes)*60-seconds)*100);
				IsoString formatStr = '%' + IsoString().Format("%dd",width) + ":%02d:%02d.%02d";
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(seconds),Abs(secondsfrac));
			}
			return String();
			}
		}else {
			return String().Format(numberFormat.c_str(),numberStr.ToFloat());
		}

		return String();
	}

	PropertyNode::PropertyNode(TreeBox& parentTreeBox):m_keyStr("/") {
			m_thisTreeBoxNode = createTreeBoxNodeForTreeBox(parentTreeBox);
	}


	PropertyNode::PropertyNode(PropertyNode* parent,String INDI_device):m_keyStr(), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device);
		if (parent!=NULL) {
			addToParentNode(parent);
			// create new device node in TreeBox
			m_thisTreeBoxNode=createTreeBoxNode(parent->m_thisTreeBoxNode);
		}
	}

	PropertyNode::PropertyNode(PropertyNode* parent,String INDI_device, String INDI_property):m_keyStr(), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device,INDI_property);
		if (parent!=NULL) {
			addToParentNode(parent);
			m_thisTreeBoxNode=createTreeBoxNode();
			// link property node to device node
			addTreeBoxNodeToParent(parent);
		}
	}

	PropertyNode::PropertyNode(PropertyNode* parent,String INDI_device, String INDI_property,String INDI_propertyElement):m_keyStr(), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device,INDI_property,INDI_propertyElement);
		if (parent!=NULL) {
			addToParentNode(parent);
			m_thisTreeBoxNode=createTreeBoxNode();
			// link element node to property node
			addTreeBoxNodeToParent(parent);
		}
	}


	void PropertyNode::addToParentNode(PropertyNode* parent) {
		assert(parent!=NULL);
		assert(parent!=this);
		parent->m_childs.push_back(this);
	 }
    void PropertyNode::addTreeBoxNodeToParent(PropertyNode* parent) {
    	assert(parent!=NULL);
	    parent->getTreeBoxNode()->Add(m_thisTreeBoxNode);
    }


	PropertyNode* PropertyNode::create(PropertyNode* parent,String INDI_device, String INDI_property,String INDI_propertyElement, String INDI_propertyType){
		if (INDI_propertyType == "INDI_SWITCH"){
			return new SwitchPropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement);
		} else if (INDI_propertyType == "INDI_NUMBER"){
			return new NumberPropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement);
		} else if (INDI_propertyType == "INDI_TEXT"){
			return new TextPropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement);
		} else if (INDI_propertyType == "INDI_LIGHT"){
			return new LightPropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement);
		} else {
			return new PropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement);
		}
	}



	bool PropertyNode::accept(IPropertyVisitor* visitor, String propertyKeyStr, String newValue){
		bool requiresPostVisit = visitor->visit(this,propertyKeyStr, newValue);
		for (size_t i=0; i<this->m_childs.size(); ++i){
			requiresPostVisit = requiresPostVisit || m_childs[i]->accept(visitor,propertyKeyStr, newValue);
		}
		if (requiresPostVisit){
			for (size_t i=0; i<m_childs.size(); ++i){
				visitor->postVisit(m_childs[i],propertyKeyStr, String("OFF"));
			}
		}
		return requiresPostVisit;
	}

	void PropertyNode::RemoveChild(PropertyNode* child){
		m_childs.erase(std::remove(m_childs.begin(),m_childs.end(),child),m_childs.end());
	}

	void PropertyNode:: setNodeINDIText(String text)   {
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(TextColumn,text);
	}

	void PropertyNode::setNodeINDIValue(String value) {
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(ValueColumn,value);
	}

	void PropertyNode::setNodeINDIType(String type)   {
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(TypeColumn,type);
	}

	void PropertyNode::setNodeINDINumberFormat(String numberFormat){
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(NumberFormatColumn,numberFormat);
	}

	void PropertyNode::setNodeINDILabel(String label){
			if (m_thisTreeBoxNode!=NULL)
				m_thisTreeBoxNode->SetText(LabelColumn,label);
	}

	void PropertyNode::setNodeINDIState(int state)   {
		if (m_thisTreeBoxNode!=NULL)
			switch (state){
			case IPS_OK:
				m_thisTreeBoxNode->SetIcon(TextColumn,Bitmap(":/bullets/bullet-ball-glass-green.png"));
				break;
			case IPS_ALERT:
				m_thisTreeBoxNode->SetIcon(TextColumn,Bitmap(":/bullets/bullet-ball-glass-red.png"));
				break;
			case IPS_BUSY:
				m_thisTreeBoxNode->SetIcon(TextColumn,Bitmap(":/bullets/bullet-ball-glass-yellow.png"));
				break;
			case IPS_IDLE:
				m_thisTreeBoxNode->SetIcon(TextColumn,Bitmap(":/bullets/bullet-ball-glass-grey.png"));
				break;
			}
	}

    String PropertyNode::getNodeINDIText() const  {
    	if (m_thisTreeBoxNode!=NULL)
    		return m_thisTreeBoxNode->Text(TextColumn);
    	return String();
    }

	String PropertyNode::getNodeINDIValue() const {
		if (m_thisTreeBoxNode!=NULL)
			return m_thisTreeBoxNode->Text(ValueColumn);
		return String();
	}

	String PropertyNode::getNodeINDIType() const  {
		if (m_thisTreeBoxNode!=NULL)
			return m_thisTreeBoxNode->Text(TypeColumn);
		return String();
	}


	bool FindNodeVisitor::visit(PropertyNode* pNode, String propertyKeyString, String newPropertyString){
		assert(pNode!=NULL && "property node is NULL");
		bool requiresPostVisit=false;
		if (pNode->getPropertyKeyString() == propertyKeyString){
			m_found=true;
			m_foundNode = pNode;
		}
		return requiresPostVisit;
	}

	void PropertyTree::addTreeBoxItem(PropertyNode* node){
		node->addToParentNode(m_rootNode);
		node->setTreeBoxNode(node->createTreeBoxNode(m_rootNode->getTreeBoxNode()));
	}


	PropertyNode* PropertyTree::addNode(String device){
		PropertyNode* node = m_factory->create(PropertyUtils::getKey(device));
		assert(m_rootNode!=NULL);
		addTreeBoxItem(node);
		return node;
	}
	PropertyNode* PropertyTree::addNode(PropertyNode* parent, String device,String property){
		String keyStr = PropertyUtils::getKey(device,property);
		PropertyNode* node = m_factory->create(keyStr);
		node->addToParentNode(parent);
		node->setTreeBoxNode(node->createTreeBoxNode());
		node->addTreeBoxNodeToParent(parent);
		return node;
	}

	PropertyNode* PropertyTree::addNode(PropertyNode* parent, String device,String property,String element){
		String keyStr = PropertyUtils::getKey(device,property,element);
		PropertyNode* node = m_factory->create(keyStr);
		node->addToParentNode(parent);
		node->setTreeBoxNode(node->createTreeBoxNode());
		node->addTreeBoxNodeToParent(parent);
		return node;
	}

	PropertyNode* PropertyTree::addElementNode(String device,String property,String element, int state, String label){
		assert(m_rootNode!=NULL);
		// lookup device node
		String deviceKeyStr = PropertyUtils::getKey(device);
		FindNodeVisitor* findNodeVisitor = new FindNodeVisitor();
		m_rootNode->accept(findNodeVisitor,deviceKeyStr,"");
		PropertyNode* deviceNode=NULL;
		if (!findNodeVisitor->foundNode()){
			// create and add device node
			deviceNode=addNode(device);
		} else {
			deviceNode=findNodeVisitor->getNode();
		}
		assert(deviceNode!=NULL);
		deviceNode->setNodeINDIText(device);
		// lookup property node
		findNodeVisitor->reset();
		String propertyKeyStr = PropertyUtils::getKey(device,property);
		deviceNode->accept(findNodeVisitor,propertyKeyStr,"");
		PropertyNode* propNode=NULL;
		if (!findNodeVisitor->foundNode()){
			// create and add property node
			propNode=addNode(deviceNode,device,property);
		} else {
			propNode=findNodeVisitor->getNode();
		}
		assert(propNode!=NULL);
		propNode->setNodeINDIText(property);
		propNode->setNodeINDIState(state);
		propNode->setNodeINDILabel(label);
		// lookup element node
		findNodeVisitor->reset();
		String elementKeyStr = PropertyUtils::getKey(device,property,element);
		propNode->accept(findNodeVisitor,elementKeyStr,"");
		PropertyNode* elemNode=NULL;
		if (!findNodeVisitor->foundNode()){
			// create and add property node
			elemNode=addNode(propNode,device,property,element);
		} else {
			elemNode=findNodeVisitor->getNode();
		}
		assert(elemNode!=NULL);
		elemNode->setNodeINDIText(element);
		return elemNode;
	}

}

// ----------------------------------------------------------------------------
// EOF PropertyNode.cpp - Released 2015/10/13 15:55:45 UTC
