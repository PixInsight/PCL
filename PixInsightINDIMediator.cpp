
#include "PixInsightINDIMediator.h"
#include <pcl/Console.h>

//#include "PixInsightINDIInterface.h"
//#include "indibase.h"



namespace pcl {

	PixInsightINDIMediator::PixInsightINDIMediator(PixInsightINDIInterface* pixInterface):m_pixInterface(pixInterface){
		
	}


	void PixInsightINDIMediator::newProperty(INDI::Property *property){
		//m_pixInterface->m_PropertyListNeedsUpdate=true;
	}

	void PixInsightINDIMediator::newMessage(INDI::BaseDevice *dp, int messageID){
		m_pixInterface->GUI->DrvPropDlg.m_serverMessage = IsoString(dp->messageQueue(messageID));	
		m_pixInterface->m_PropertyListNeedsUpdate=true;
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
}