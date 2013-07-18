#include <stdio.h>
#include "PixInsightINDIMediator.h"
#include <pcl/Console.h>

//#include "PixInsightINDIInterface.h"
//#include "indibase.h"



namespace pcl {

	void PixInsightINDIMediator::newDevice(INDI::BaseDevice *dp){
		
	}
	
	void PixInsightINDIMediator::newProperty(INDI::Property *property){
		m_Instance->getProperties();
	}

	void PixInsightINDIMediator::newMessage(INDI::BaseDevice *dp, int messageID){
		if (m_pixInterface) {
			m_pixInterface->GUI->DrvPropDlg.m_serverMessage = IsoString(dp->messageQueue(messageID));	
			m_pixInterface->m_PropertyListNeedsUpdate=true;
		}
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
		FILE* f;
		f = fopen(fileName.c_str(),"w+" );
		fwrite(bp->blob,bp->bloblen,1,f);
		fclose(f);
		
	}
}