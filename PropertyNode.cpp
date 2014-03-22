#include "PropertyNode.h"

namespace pcl {

	IsoString PropertyUtils::getDevice(IsoString keyString){
		int startpos = keyString.Find("/");
		int endpos   = keyString.Find("/",1);
		return keyString.SubString(startpos,endpos-startpos);
	}

	IsoString PropertyUtils::getProperty(IsoString keyString){
		int startpos = keyString.Find("/",1);
		int endpos = keyString.Find("/",startpos+1);
		return keyString.SubString(startpos,endpos-startpos);
	}

	PropertyNode::PropertyNode(TreeBox& parent):m_keyStr(IsoString("/")) {
			m_thisTreeBoxNode = new TreeBox::Node(parent);
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

	void PropertyNode::accept(IPropertyVisitor* visitor, IProperty* INDI_property){
		visitor->visit(this,INDI_property);
		for (size_t i=0; i<this->m_childs.size(); ++i){
			m_childs[i]->accept(visitor,INDI_property);
		}
	}

	void CreateVisitor::visit(PropertyNode* pNode, IProperty* INDI_property){
		/*DeviceNodeMapType::iterator keyIter = m_deviceNodeMap.find(INDI_property->getDeviceName());
		IsoString keyStr=PropertyUtils::getKey(INDI_property->getDeviceName());
		if (keyIter == m_deviceNodeMap.end()){
			m_deviceNodeMap[keyStr]=pNode;
		}
		else {
			
		}*/
	}
}