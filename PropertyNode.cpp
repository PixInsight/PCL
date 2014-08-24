#include "PropertyNode.h"
#include <assert.h>
#include <algorithm>

namespace pcl {

	IsoString PropertyUtils::getDevice(IsoString keyString){
		int startpos = keyString.Find("/");
		int endpos   = keyString.Find("/",1);
		return keyString.SubString(startpos+1,endpos-startpos-1);
	}

	IsoString PropertyUtils::getProperty(IsoString keyString){
		int startpos = keyString.Find("/",1);
		int endpos = keyString.Find("/",startpos+1);
		return keyString.SubString(startpos+1,endpos-startpos-1);
	}

	IsoString PropertyUtils::getElement(IsoString keyString){
		int startpos1 = keyString.Find("/",1);
		int startpos2 = keyString.Find("/",startpos1+1);
		int endpos = keyString.Find("/",startpos2+1);
		return keyString.SubString(startpos2+1,endpos-startpos2);
	}

	PropertyNode::PropertyNode(TreeBox& parentTreeBox):m_keyStr(IsoString("/")) {
			m_thisTreeBoxNode = new TreeBox::Node(parentTreeBox);
	}


	PropertyNode::PropertyNode(PropertyNode* parent,IsoString INDI_device):m_keyStr(IsoString("")), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device);
		if (parent!=NULL) {
			parent->m_childs.push_back(this);
			m_thisTreeBoxNode=new TreeBox::Node(*parent->m_thisTreeBoxNode);
		}	
	}

	PropertyNode::PropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property):m_keyStr(IsoString("")), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device,INDI_property);
		if (parent!=NULL) {
			parent->m_childs.push_back(this);
			m_thisTreeBoxNode=new TreeBox::Node();
			parent->getTreeBoxNode()->Add(m_thisTreeBoxNode);
		}
	}

	PropertyNode::PropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement):m_keyStr(IsoString("")), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device,INDI_property,INDI_propertyElement);
		if (parent!=NULL) {
			parent->m_childs.push_back(this);
			m_thisTreeBoxNode=new TreeBox::Node();
			parent->getTreeBoxNode()->Add(m_thisTreeBoxNode);
		}
	}

	PropertyNode* PropertyNode::create(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement, IsoString INDI_propertyType){
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



	bool PropertyNode::accept(IPropertyVisitor* visitor, IsoString propertyKeyStr, IsoString newValue){
		bool requiresPostVisit = visitor->visit(this,propertyKeyStr, newValue);
		for (size_t i=0; i<this->m_childs.size(); ++i){
			requiresPostVisit = requiresPostVisit || m_childs[i]->accept(visitor,propertyKeyStr, newValue);
		}
		if (requiresPostVisit){
			for (size_t i=0; i<m_childs.size(); ++i){
				visitor->postVisit(m_childs[i],propertyKeyStr, IsoString("OFF"));
			}
		}
		return requiresPostVisit;
	}

	void PropertyNode::RemoveChild(PropertyNode* child){
		m_childs.erase(std::remove(m_childs.begin(),m_childs.end(),child),m_childs.end());
	}

	bool UpdateVisitor::visit(PropertyNode* pNode, IsoString propertyKeyString, IsoString newValue){
		assert(pNode!=NULL && "property node is NULL");
		bool requiresPostVisit=false;
		if (pNode->getPropertyKeyString() == propertyKeyString){
			TreeBox::Node* currentTreeNode = pNode->getTreeBoxNode();
			currentTreeNode->SetText(1,newValue);

			SwitchPropertyNode* switchNode=dynamic_cast<SwitchPropertyNode*>(pNode);
			if (switchNode!=NULL){
				requiresPostVisit=true;
			}

		}

		return requiresPostVisit;
	}

	void UpdateVisitor::postVisit(PropertyNode* pNode, IsoString propertyKeyString, IsoString newValue){
		assert(pNode!=NULL && "property node is NULL");

		SwitchPropertyNode* switchNode=dynamic_cast<SwitchPropertyNode*>(pNode);
		if (switchNode!=NULL){
			if (pNode->getPropertyKeyString() != propertyKeyString){
				TreeBox::Node* currentTreeNode = pNode->getTreeBoxNode();
				currentTreeNode->SetText(1,newValue);
			}	
		}

	}


}
