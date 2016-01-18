//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// IINDIProperty.cpp - Released 2015/10/13 15:55:45 UTC
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

/*
 * IINDIProperty.cpp
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#include "IINDIProperty.h"
#include "pcl/Exception.h"
#include "pcl/String.h"
#include "error.h"
namespace pcl {

#define CHECK_INDEX_THROWS(EXPR) \
		if ((int) i > EXPR - 1){\
			throw Error(ERR_MSG("Invalid element index."));\
		}


 IProperty* PropertyFactory::create(INDI::Property* property){
		switch(property->getType()){
		case INDI_NUMBER:
			return new NumberProperty(property);
			break;
		case INDI_TEXT:
			return new TextProperty(property);
			break;
		case INDI_SWITCH:
			return new SwitchProperty(property);
			break;
		case INDI_LIGHT:
			return new LightProperty(property);
			break;
		default:
			return new IProperty(property);
		}
	}

 IProperty* PropertyFactory::create(INDI::Property* property,INDI_TYPE type){
	 	property->setType(type);
 		switch(type){
 		case INDI_NUMBER:
 		{
 			INumberVectorProperty* nProperty = new INumberVectorProperty();
 			CHECK_POINTER(nProperty);
 			property->setProperty(nProperty);
 			return new NumberProperty(property);
 			break;
 		}
 		case INDI_TEXT:
 		{
 			ITextVectorProperty* tProperty = new ITextVectorProperty();
 			CHECK_POINTER(tProperty);
 		 	property->setProperty(tProperty);
 			return new TextProperty(property);
 			break;
 		}
 		case INDI_SWITCH:
 		{
 			ISwitchVectorProperty* sProperty = new ISwitchVectorProperty();
 			CHECK_POINTER(sProperty);
 			property->setProperty(sProperty);
 			return new SwitchProperty(property);
 			break;
 		}
 		case INDI_LIGHT:
 		{
 			ILightVectorProperty* lProperty = new ILightVectorProperty();
 			CHECK_POINTER(lProperty);
			property->setProperty(lProperty);
 			return new LightProperty(property);
 			break;
 		}
 		default:
 			throw FatalError(ERR_MSG("Unknown property."));
 		}
 	}

String SwitchProperty::getElementName(size_t i) {
	CHECK_INDEX_THROWS(
			((ISwitchVectorProperty*) m_property->getProperty())->nsp);

	return String(m_property->getSwitch()->sp[i].name);
}

String SwitchProperty::getElementLabel(size_t i) {
	CHECK_INDEX_THROWS(
			((ISwitchVectorProperty*) m_property->getProperty())->nsp);

	return String(m_property->getSwitch()->sp[i].label);
}

String SwitchProperty::getElementValue(size_t i) {
	CHECK_INDEX_THROWS(
			((ISwitchVectorProperty*) m_property->getProperty())->nsp);

	return String(m_property->getSwitch()->sp[i].s == ISS_ON  ? "ON" : "OFF");
}




 void SwitchProperty::addElement(IsoString elementName, IsoString value){
 	 ISwitch* sp = ((ISwitchVectorProperty*) m_property->getProperty())->sp;
 	 int nsp = ((ISwitchVectorProperty*) m_property->getProperty())->nsp;
 	 sp = (ISwitch*) realloc(sp, (nsp+1) * sizeof(ISwitch));
 	 CHECK_POINTER(sp);
 	 strcpy(sp->name, elementName.c_str());
 	 sp->s = (value == "ON") ? ISS_ON : ISS_OFF ;
 	 sp->svp =(ISwitchVectorProperty*) m_property->getProperty();
 	 ((ISwitchVectorProperty*) m_property->getProperty())->nsp++;
 	 ((ISwitchVectorProperty*) m_property->getProperty())->sp = sp;
  }


 void NumberProperty::addElement(IsoString elementName, IsoString value){
	 if (value.IsEmpty()){
		 return;
	 }
	 INumber* np = ((INumberVectorProperty*) m_property->getProperty())->np;
	 int nnp = ((INumberVectorProperty*) m_property->getProperty())->nnp;
	 np = (INumber*) realloc(np, (nnp+1) * sizeof(INumber));
	 CHECK_POINTER(np);
	 strcpy(np->name, elementName.c_str());
	 value.TryToDouble(np->value);
	 np->nvp=(INumberVectorProperty*) m_property->getProperty();
	 ((INumberVectorProperty*) m_property->getProperty())->nnp++;
	 ((INumberVectorProperty*) m_property->getProperty())->np = np;
 }

String NumberProperty::getElementName(size_t i) {
	CHECK_INDEX_THROWS(
			((INumberVectorProperty*) m_property->getProperty())->nnp);

	return String(m_property->getNumber()->np[i].name);
 }

String NumberProperty::getElementLabel(size_t i)  {
	CHECK_INDEX_THROWS(
			((INumberVectorProperty*) m_property->getProperty())->nnp);

	return String(m_property->getNumber()->np[i].label);
}

String NumberProperty::getElementValue(size_t i) {
	CHECK_INDEX_THROWS(
			((INumberVectorProperty*) m_property->getProperty())->nnp);

	return String(m_property->getNumber()->np[i].value);
}

String NumberProperty::getNumberFormat(size_t i){
	CHECK_INDEX_THROWS(
			((INumberVectorProperty*) m_property->getProperty())->nnp);

	return String(m_property->getNumber()->np[i].format);
}

String TextProperty::getElementName(size_t i) {
	CHECK_INDEX_THROWS(
			((ITextVectorProperty*) m_property->getProperty())->ntp);

	return String(m_property->getText()->tp[i].name);
}

String TextProperty::getElementLabel(size_t i)  {
	CHECK_INDEX_THROWS(
			((ITextVectorProperty*) m_property->getProperty())->ntp);

	return String(m_property->getText()->tp[i].label);
}

String TextProperty::getElementValue(size_t i) {
	CHECK_INDEX_THROWS(
			((ITextVectorProperty*) m_property->getProperty())->ntp);

	return String(m_property->getText()->tp[i].text);
}


void TextProperty::addElement(IsoString elementName, IsoString value) {
	IText* tp = ((ITextVectorProperty*) m_property->getProperty())->tp;
	int ntp = ((ITextVectorProperty*) m_property->getProperty())->ntp;
	tp = (IText*) realloc(tp, (ntp + 1) * sizeof(IText));
	CHECK_POINTER(tp);
	strcpy(tp->name, elementName.c_str());

// 	//FIXME Leaks memory?
// 	tp->text = (char*) malloc(sizeof(value.c_str()));
// 	strcpy(tp->text, value.c_str());
   tp->text = value.Release(); // N.B.: can do this because value is being passed by value.

	tp->tvp = (ITextVectorProperty*) m_property->getProperty();
	((ITextVectorProperty*) m_property->getProperty())->ntp++;
	((ITextVectorProperty*) m_property->getProperty())->tp = tp;
 }


 void LightProperty::addElement(IsoString elementName, IsoString value) {
	ILight* lp = ((ILightVectorProperty*) m_property->getProperty())->lp;
	int nlp = ((ILightVectorProperty*) m_property->getProperty())->nlp;
	lp = (ILight*) realloc(lp, (nlp + 1) * sizeof(ILight));
	CHECK_POINTER(lp);
	strcpy(lp->name, elementName.c_str());
	if (value == "IDLE") {
		lp->s = IPS_IDLE;
	} else if (value == "OK") {
		lp->s = IPS_OK;
	} else if (value == "BUSY") {
		lp->s = IPS_BUSY;
	} else if (value == "ALERT") {
		lp->s = IPS_ALERT;
	} else {
		throw Error(ERR_MSG("Invalid property value;"));
	}
	lp->lvp = (ILightVectorProperty*) m_property->getProperty();
	((ILightVectorProperty*) m_property->getProperty())->nlp++;
	((ILightVectorProperty*) m_property->getProperty())->lp = lp;
}




String LightProperty::getElementName(size_t i) {
	CHECK_INDEX_THROWS(((ILightVectorProperty*) m_property->getProperty())->nlp);
	return String(m_property->getLight()->lp[i].name);
}

String LightProperty::getElementLabel(size_t i) {
	CHECK_INDEX_THROWS(((ILightVectorProperty*) m_property->getProperty())->nlp);

	return String(m_property->getLight()->lp[i].label);
}

String LightProperty::getElementValue(size_t i) {
	CHECK_INDEX_THROWS(((ILightVectorProperty*) m_property->getProperty())->nlp);

	switch (m_property->getLight()->lp[i].s) {
	case IPS_IDLE:
		return String("IDLE");
		break;
	case IPS_OK:
		return String("OK");
		break;
	case IPS_BUSY:
		return String("BUSY");
		break;
	case IPS_ALERT:
		return String("ALERT");
		break;
	default:
		return String("UNSUPPORTED");
	}
}





}

// ----------------------------------------------------------------------------
// EOF IINDIProperty.cpp - Released 2015/10/13 15:55:45 UTC
