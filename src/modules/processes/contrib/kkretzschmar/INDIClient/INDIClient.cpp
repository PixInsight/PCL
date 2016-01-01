//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// INDIClient.cpp - Released 2015/10/13 15:55:45 UTC
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

#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "INDIClient.h"
#include "PropertyNode.h"

#include <pcl/Console.h>

namespace pcl
{

	void INDIClient::runOnPropertyTable(IProperty* INDIProperty, const ArrayOperator<INDIPropertyListItem>* arrayOp, PropertyFlagType flag){
		String sep("/");
		INDIPropertyListItem propertyListItem;
		propertyListItem.Device=INDIProperty->getDeviceName();
		propertyListItem.Property=INDIProperty->getName();
		propertyListItem.PropertyType=INDIProperty->getType();
		propertyListItem.PropertyTypeStr=INDIProperty->getTypeStr();
		propertyListItem.PropertyState =INDIProperty->getState();
		propertyListItem.PropertyLabel =INDIProperty->getLabel();

		for (size_t i=0; i<INDIProperty->getNumOfElements();i++) {
			propertyListItem.Element=INDIProperty->getElementName(i);
			propertyListItem.PropertyKey=sep + propertyListItem.Device + sep + propertyListItem.Property + sep + propertyListItem.Element;
			propertyListItem.PropertyValue=INDIProperty->getElementValue(i);
			propertyListItem.PropertyFlag=flag;
			propertyListItem.PropertyNumberFormat=INDIProperty->getNumberFormat(i);
			propertyListItem.ElementLabel=INDIProperty->getElementLabel(i);
			arrayOp->run(m_Instance->getPropertyList(),propertyListItem);
		    if (m_ScriptInstance) {
			  arrayOp->run(m_ScriptInstance->getPropertyList(), propertyListItem);
		    }
		}
	}

	void INDIClient::newDevice(INDI::BaseDevice *dp){
		assert(dp!=NULL);
		if (m_Instance!=NULL){
			INDIDeviceListItem deviceListItem;
			deviceListItem.DeviceName=String(dp->getDeviceName());
			deviceListItem.DeviceLabel=String(dp->getDriverName());
			m_Instance->getDeviceList().Append(deviceListItem);
			if (m_ScriptInstance) {
				m_ScriptInstance->getDeviceList().Append(deviceListItem);
			}
		}

	}

	void INDIClient::deleteDevice(INDI::BaseDevice *dp){
		assert(dp!=NULL);

		if (m_Instance!=NULL){
			for (INDIDeviceControllerInstance::PropertyListType::iterator iter=m_Instance->getPropertyList().Begin() ; iter!=m_Instance->getPropertyList().End(); ++iter){
				if (iter->Device==String(dp->getDeviceName())){
					iter->PropertyFlag=Remove;
				}
			}
			// TODO script instance
		}
	}


	void INDIClient::newProperty(INDI::Property *property){
		assert(property!=NULL);
		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* append=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayAppend<INDIPropertyListItem>());
		// add property to the property process parameter table
		runOnPropertyTable(INDIProperty,append,Insert);

		setBLOBMode(B_ALSO,property->getDeviceName());
	}

	void INDIClient::removeProperty(INDI::Property *property){
		if (property!=NULL){
			ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());

			IProperty* INDIProperty = PropertyFactory::create(property);

			String sep("/");
			INDIPropertyListItem propertyListItem;
			propertyListItem.Device = INDIProperty->getDeviceName();
			propertyListItem.Property = INDIProperty->getName();
			propertyListItem.PropertyType = INDIProperty->getType();
			propertyListItem.PropertyTypeStr = INDIProperty->getTypeStr();
			propertyListItem.PropertyState = INDIProperty->getState();

			for (size_t i = 0; i < INDIProperty->getNumOfElements(); i++) {
				propertyListItem.Element = INDIProperty->getElementName(i);
				propertyListItem.PropertyKey = sep + propertyListItem.Device + sep
						+ propertyListItem.Property + sep
						+ propertyListItem.Element;
				propertyListItem.PropertyValue = INDIProperty->getElementValue(i);
				propertyListItem.PropertyFlag=Remove;
				update->run(m_Instance->getPropertyList(), propertyListItem);
				if (m_ScriptInstance) {
					update->run(m_ScriptInstance->getPropertyList(), propertyListItem);
				}
			}
		}
	}

	void INDIClient::newMessage(INDI::BaseDevice *dp, int messageID){
		const char* message = dp->messageQueue(messageID);
		if (message!=NULL){
			m_Instance->getCurrentMessage()=IsoString(message);
			if (m_ScriptInstance) {
				m_ScriptInstance->getCurrentMessage()=IsoString(message);
			}
		}
	}

	void INDIClient::newSwitch(ISwitchVectorProperty *svp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(svp);
		property->setType(INDI_SWITCH);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());
		runOnPropertyTable(INDIProperty,update,Update);

	}

	void INDIClient::newNumber(INumberVectorProperty *nvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(nvp);
		property->setType(INDI_NUMBER);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());
		runOnPropertyTable(INDIProperty,update,Update);
	}

	void INDIClient::newText(ITextVectorProperty *tvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(tvp);
		property->setType(INDI_TEXT);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());
		runOnPropertyTable(INDIProperty,update,Update);
	}

	void INDIClient::newLight(ILightVectorProperty *lvp){
		INDI::Property* property = new INDI::Property();
		property->setProperty(lvp);
		property->setType(INDI_LIGHT);

		IProperty* INDIProperty = PropertyFactory::create(property);
		ArrayOperator<INDIPropertyListItem>* update=dynamic_cast<ArrayOperator<INDIPropertyListItem>*>(new ArrayUpdate<INDIPropertyListItem>());
		runOnPropertyTable(INDIProperty,update,Update);
	}

   void INDIClient::newBLOB( IBLOB *bp )
   {
      String fileName = File::SystemTempDirectory() + '/' + bp->label + bp->format;
      File myfile = File::CreateFileForWriting( fileName );
      myfile.Write( bp->blob, bp->size );
      myfile.Close();
      m_Instance->setImageDownloadedFlag( true );
      if ( m_ScriptInstance )
         m_ScriptInstance->setImageDownloadedFlag( true );
   }
}

// ----------------------------------------------------------------------------
// EOF INDIClient.cpp - Released 2015/10/13 15:55:45 UTC
