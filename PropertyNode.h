// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInterface.h - Released 2013/03/24 18:42:27 UTC
// ****************************************************************************
// This file is part of the standard PixInsightINDI PixInsight module.
//
// Copyright (c) 2003-2013, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef PROPERTY_NODE_H
#define PROPERTY_NODE_H

#include <vector>
#include <map>
#include <pcl/TreeBox.h>
#include "PixInsightINDIclient.h"

namespace pcl {

	// forward declaration
	class IPropertyVisitor;

	static const IsoString c_sep = IsoString("/"); 

	// Utiliy functions
	class PropertyUtils{
	public:
		static IsoString getDevice(IsoString keyString);
		static IsoString getProperty(IsoString keyString);
		static IsoString getElement(IsoString keyString);
		static IsoString getKey(IsoString INDI_device) {return c_sep+INDI_device;}
		static IsoString getKey(IsoString INDI_device, IsoString INDI_property) { return c_sep+INDI_device+c_sep+INDI_property;}
		static IsoString getKey(IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement){return c_sep+INDI_device+c_sep+INDI_property+c_sep+INDI_propertyElement; }
	};

	class PropertyNode {
	private:
		std::vector<PropertyNode*>  m_childs;
		IsoString                   m_keyStr;
		TreeBox::Node*              m_thisTreeBoxNode;
	protected:
		PropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement);
	public:
		PropertyNode(TreeBox& parentTreeBox);
		PropertyNode(PropertyNode* parent,IsoString INDI_device);
		PropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property);

		IsoString getPropertyKeyString() {return m_keyStr;}

		void RemoveChild(PropertyNode* child);

		virtual TreeBox::Node* getTreeBoxNode() {return m_thisTreeBoxNode;}

		virtual ~PropertyNode() {
			if (m_thisTreeBoxNode!=NULL){
				delete m_thisTreeBoxNode;
			}
		}

		virtual bool accept(IPropertyVisitor* visitor, IsoString propertyKeyStr, IsoString newValue);

		static PropertyNode* create(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement, IsoString INDI_propertyType);
	};

	class SwitchPropertyNode : public PropertyNode {
	public:
		SwitchPropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement):PropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement){}
	};

	class NumberPropertyNode : public PropertyNode {
	public:
		NumberPropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement):PropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement){}
	};

	class TextPropertyNode : public PropertyNode {
	public:
		TextPropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement):PropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement){}
	};

	class LightPropertyNode : public PropertyNode {
	public:
		LightPropertyNode(PropertyNode* parent,IsoString INDI_device, IsoString INDI_property,IsoString INDI_propertyElement):PropertyNode(parent,INDI_device,INDI_property,INDI_propertyElement){}
	};



	class IPropertyVisitor {
	public:
		virtual bool visit(PropertyNode* pNode,  IsoString propertyKeyStr, IsoString newPropertyValue) = 0;
		virtual void postVisit(PropertyNode* pNode,  IsoString propertyKeyStr, IsoString newPropertyValue) = 0;
		virtual ~IPropertyVisitor(){}
	};



	class UpdateVisitor : public IPropertyVisitor {
	protected:
		UpdateVisitor(){}
	public:
		static IPropertyVisitor* create(){return new UpdateVisitor();}
		bool visit(PropertyNode* pNode, IsoString propertyKeyString, IsoString newPropertyValue);
		void postVisit(PropertyNode* pNode, IsoString propertyKeyString, IsoString newPropertyValue);
		virtual ~UpdateVisitor(){}
	
	};
}

#endif //PROPERTY_NODE_H
