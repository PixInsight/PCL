#ifndef __PixInsightINDIMediator_h
#define __PixInsightINDIMediator_h


#include "PixInsightINDIInterface.h"
#include "indibase.h"
#include "basedevice.h"



namespace pcl {

	class PixInsightINDIMediator : public INDI::BaseMediator{
	public:

		//PixInsightINDIMediator(DevicePropertiesDialog* pixInsightGUIData);
		PixInsightINDIMediator(PixInsightINDIInterface* pixInterface);

			
		void newMessage(INDI::BaseDevice *dp, int messageID);
		void newSwitch(ISwitchVectorProperty *svp);
		void newNumber(INumberVectorProperty *nvp);
		void newText(ITextVectorProperty *tvp);
		void newLight(ILightVectorProperty *lvp);
		void newBLOB(IBLOB *bp);
		void newProperty(INDI::Property *property);
	private:

		//DevicePropertiesDialog* m_pixInsightGUIData;
		PixInsightINDIInterface* m_pixInterface;

	};

}

#endif