#include "PropertyNode.h"
#include <assert.h>
#include <algorithm>
#include <pcl/Math.h>

namespace pcl {

	IsoString PropertyUtils::getDevice(IsoString keyString){
		size_t startpos = keyString.Find("/");
		size_t endpos   = keyString.Find("/",1);
		return keyString.SubString(startpos+1,endpos-startpos-1);
	}

	IsoString PropertyUtils::getProperty(IsoString keyString){
		size_t startpos = keyString.Find("/",1);
		size_t endpos = keyString.Find("/",startpos+1);
		return keyString.SubString(startpos+1,endpos-startpos-1);
	}

	IsoString PropertyUtils::getElement(IsoString keyString){
		size_t startpos1 = keyString.Find("/",1);
		size_t startpos2 = keyString.Find("/",startpos1+1);
		size_t endpos = keyString.Find("/",startpos2+1);
		return keyString.SubString(startpos2+1,endpos-startpos2);
	}

	IsoString PropertyUtils::getFormattedNumber(IsoString numberStr, IsoString numberFormat){
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
				IsoString formatStr = IsoString("%") + IsoString().Format("%dd",width) + IsoString(":%02d");
				return String().Format(formatStr.c_str(),hours,Abs(minutes));
			}
			case 5:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int minutesfrac = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*10);
				IsoString formatStr = IsoString("%") + IsoString().Format("%dd",width) + IsoString(":%02d.%d");
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(minutesfrac));
			}
			case 6:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int seconds     = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*60);
				IsoString formatStr = IsoString("%") + IsoString().Format("%dd",width) + IsoString(":%02d:%02d");
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(seconds));
			}
			case 8:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int seconds     = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*60);
				int secondsfrac = Trunc((((numberStr.ToFloat() - hours)*60-minutes)*60-seconds)*10);
				IsoString formatStr = IsoString("%") + IsoString().Format("%dd",width) + IsoString(":%02d:%02d.%d");
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(seconds),Abs(secondsfrac));
			}
			case 9:
			{
				int minutes     = Trunc((numberStr.ToFloat() - hours)*60);
				int seconds     = Trunc(((numberStr.ToFloat() - hours)*60-minutes)*60);
				int secondsfrac = Trunc((((numberStr.ToFloat() - hours)*60-minutes)*60-seconds)*100);
				IsoString formatStr = IsoString("%") + IsoString().Format("%dd",width) + IsoString(":%02d:%02d.%02d");
				return String().Format(formatStr.c_str(),hours,Abs(minutes),Abs(seconds),Abs(secondsfrac));
			}
			return IsoString("");
			}
		}else {
			return String().Format(numberFormat.c_str(),numberStr.ToFloat());
		}

		return IsoString("");
	}

	PropertyNode::PropertyNode(TreeBox& parentTreeBox):m_keyStr(IsoString("/")) {
			m_thisTreeBoxNode = createTreeBoxNodeForTreeBox(parentTreeBox);
	}


	PropertyNode::PropertyNode(PropertyNode* parent,IsoString INDI_device):m_keyStr(IsoString("")), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device);
		if (parent!=NULL) {
			addToParentNode(parent);
			// create new device node in TreeBox
			m_thisTreeBoxNode=createTreeBoxNode(parent->m_thisTreeBoxNode);
		}	
	}

	PropertyNode::PropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property):m_keyStr(IsoString("")), m_thisTreeBoxNode(NULL) {
		m_keyStr = PropertyUtils::getKey(INDI_device,INDI_property);
		if (parent!=NULL) {
			addToParentNode(parent);
			m_thisTreeBoxNode=createTreeBoxNode();
			// link property node to device node
			addTreeBoxNodeToParent(parent);
		}
	}

	PropertyNode::PropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement):m_keyStr(IsoString("")), m_thisTreeBoxNode(NULL) {
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



	void PropertyNode::accept(IPropertyVisitor* visitor, IsoString propertyKeyStr, IsoString newValue){
		visitor->visit(this,propertyKeyStr, newValue);
		for (size_t i=0; i<this->m_childs.size(); ++i){
			m_childs[i]->accept(visitor,propertyKeyStr, newValue);
		}
	}

	void PropertyNode::RemoveChild(PropertyNode* child){
		m_childs.erase(std::remove(m_childs.begin(),m_childs.end(),child),m_childs.end());
	}

	void PropertyNode:: setNodeINDIText(IsoString text)   {
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(TextColumn,text);
	}

	void PropertyNode::setNodeINDIValue(IsoString value) {
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(ValueColumn,value);
	}

	void PropertyNode::setNodeINDIType(IsoString type)   {
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(TypeColumn,type);
	}

	void PropertyNode::setNodeINDINumberFormat(IsoString numberFormat){
		if (m_thisTreeBoxNode!=NULL)
			m_thisTreeBoxNode->SetText(NumberFormatColumn,numberFormat);
	}

	void PropertyNode::setNodeINDILabel(IsoString label){
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

    IsoString PropertyNode::getNodeINDIText() const  {
    	if (m_thisTreeBoxNode!=NULL)
    		return m_thisTreeBoxNode->Text(TextColumn);
    	else return IsoString("");
    }

	IsoString PropertyNode::getNodeINDIValue() const {
		if (m_thisTreeBoxNode!=NULL)
			return m_thisTreeBoxNode->Text(ValueColumn);
		else return IsoString("");
	}

	IsoString PropertyNode::getNodeINDIType() const  {
		if (m_thisTreeBoxNode!=NULL)
			return m_thisTreeBoxNode->Text(TypeColumn);
		else return IsoString("");
	}


	bool FindNodeVisitor::visit(PropertyNode* pNode, IsoString propertyKeyString, IsoString newPropertyString){
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


	PropertyNode* PropertyTree::addNode(IsoString device){
		PropertyNode* node = m_factory->create(PropertyUtils::getKey(device));
		assert(m_rootNode!=NULL);
		addTreeBoxItem(node);
		return node;
	}
	PropertyNode* PropertyTree::addNode(PropertyNode* parent, IsoString device,IsoString property){
		IsoString keyStr = PropertyUtils::getKey(device,property);
		PropertyNode* node = m_factory->create(keyStr);
		node->addToParentNode(parent);
		node->setTreeBoxNode(node->createTreeBoxNode());
		node->addTreeBoxNodeToParent(parent);
		return node;
	}

	PropertyNode* PropertyTree::addNode(PropertyNode* parent, IsoString device,IsoString property,IsoString element){
		IsoString keyStr = PropertyUtils::getKey(device,property,element);
		PropertyNode* node = m_factory->create(keyStr);
		node->addToParentNode(parent);
		node->setTreeBoxNode(node->createTreeBoxNode());
		node->addTreeBoxNodeToParent(parent);
		return node;
	}

	PropertyNode* PropertyTree::addElementNode(IsoString device,IsoString property,IsoString element, int state, IsoString label){
		assert(m_rootNode!=NULL);
		// lookup device node
		IsoString deviceKeyStr = PropertyUtils::getKey(device);
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
		IsoString propertyKeyStr = PropertyUtils::getKey(device,property);
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
		IsoString elementKeyStr = PropertyUtils::getKey(device,property,element);
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

