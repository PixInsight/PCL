// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInstance.cpp - Released 2013/03/24 18:42:27 UTC
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

#ifndef INDI_CLIENT_H
#define INDI_CLIENT_H

#include <pcl/Exception.h>
#include "PixInsightINDIInstance.h"
#include "PixInsightINDIInterface.h"
#include "IINDIProperty.h"

#include "indidevapi.h"
#include "indicom.h"
#include "BaseClientImpl.h"
#include "basedevice.h"

#include "error.h"

namespace pcl {

class PropertyNode;


template<typename T>
class ArrayOperator{
public:
	virtual void run(Array<T>& _array,T& _element) const {}
};

template<typename T>
class ArrayAppend : public ArrayOperator<T> {
public:
	virtual void run (Array<T>& _array,T& _element) const  {_array.Append(_element);}
};

template<typename T>
class ArrayDelete : public ArrayOperator<T> {
public:
	virtual void run (Array<T>& _array,T& _element) const  {
		typename pcl::Array<T>::iterator iter = _array.Search(_element);
		if (iter==_array.End()){
			return;
		}
		_array.Remove(iter);
	}
};

template<typename T>
class ArrayUpdate : public ArrayOperator<T> {
public:
	virtual void run (Array<T>& _array,T& _element) const  {
		typename pcl::Array<T>::iterator iter = _array.Search(_element);
		if (iter==_array.End()){
			return;
		}
		*iter=_element;
	}
};


class INDIClient : public INDI::BaseClientImpl
{
 public:

	 INDIClient(IPixInsightINDIInstance* instance,bool initThreads=true);
	 ~INDIClient(){}

	 void registerScriptInstance(IPixInsightINDIInstance* scriptInstance){
		m_ScriptInstance=scriptInstance;
	 }


protected:
	void newDevice(INDI::BaseDevice *dp);
	void deleteDevice(INDI::BaseDevice *dp);
	void newProperty(INDI::Property *property);
    void removeProperty(INDI::Property *property);
    void newBLOB(IBLOB *bp);
    void newSwitch(ISwitchVectorProperty *svp);
	void newNumber(INumberVectorProperty *nvp);
	void newMessage(INDI::BaseDevice *dp, int messageID);
    void newText(ITextVectorProperty *tvp);
    void newLight(ILightVectorProperty *lvp);
	
private:
   
   IPixInsightINDIInstance*         m_Instance;
   IPixInsightINDIInstance*         m_ScriptInstance;
   void runOnPropertyTable(IProperty* INDIProperty, const ArrayOperator<INDIPropertyListItem>* arrayOp, PropertyFlagType flag=Idle);

};


extern auto_ptr<INDIClient> indiClient;

}

#endif
