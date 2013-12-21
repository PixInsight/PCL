#ifndef __PixInsightINDIMediator_h
#define __PixInsightINDIMediator_h


#include "PixInsightINDIInterface.h"
#include "indibase.h"
#include "basedevice.h"



namespace pcl {

	class PixInsightINDIMediator : public INDI::BaseMediator{
	public:

		
		PixInsightINDIMediator(PixInsightINDIInstance* instance):m_Instance(instance),m_pixInterface(NULL){}

		void newMessage(INDI::BaseDevice *dp, int messageID);
		void newSwitch(ISwitchVectorProperty *svp);
		void newNumber(INumberVectorProperty *nvp);
		void newText(ITextVectorProperty *tvp);
		void newLight(ILightVectorProperty *lvp);
		void newBLOB(IBLOB *bp);
		void newProperty(INDI::Property *property);

		void setInterfaceInstance(PixInsightINDIInterface* pixInterface) { if (!m_pixInterface) {m_pixInterface = pixInterface;} }

	private:
		PixInsightINDIInstance*         m_Instance;
		PixInsightINDIInterface*        m_pixInterface;

	};

}

#endif