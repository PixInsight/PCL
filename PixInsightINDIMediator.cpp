#include <stdio.h>
#include <iostream>
#include <fstream>
#include "PixInsightINDIMediator.h"
#include <pcl/Console.h>

//#include "PixInsightINDIInterface.h"
//#include "indibase.h"



namespace pcl {

	
	void PixInsightINDIMediator::newProperty(INDI::Property *property){
		//m_Instance->getProperties();
	}

	void PixInsightINDIMediator::newMessage(INDI::BaseDevice *dp, int messageID){
		if (m_pixInterface) {
			m_pixInterface->GUI->DrvPropDlg.m_serverMessage = IsoString(dp->messageQueue(messageID));	
			m_pixInterface->m_PropertyListNeedsUpdate=true;
		}
		m_Instance->p_currentMessage=IsoString(dp->messageQueue(messageID));
	}

	void PixInsightINDIMediator::newSwitch(ISwitchVectorProperty *svp){
		
		//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void PixInsightINDIMediator::newNumber(INumberVectorProperty *nvp){
				//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void PixInsightINDIMediator::newText(ITextVectorProperty *tvp){
				//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void PixInsightINDIMediator::newLight(ILightVectorProperty *lvp){
				//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}
	void PixInsightINDIMediator::newBLOB(IBLOB *bp){
		IsoString fileName = IsoString("C:/Users/klaus/tmp/") + IsoString(bp->label) + IsoString(".fits"); 
	    ofstream myfile;
		myfile.open (fileName.c_str(),ios::out|ios::binary);
		myfile.write((const char*) bp->blob,bp->bloblen);
		myfile.close();		
	}
}