#ifndef CLIENTLISTENER_H
#define CLIENTLISTENER_H

#include "baseclient.h"
#include "IClientListener.h"

#include <thread>

namespace INDI {


/* === ICLientListener =====================================================================
 *
 * Interface for the listener of the INDI client.
 *
 * Any listener that is intended to be used in the INDI Client has to implement this
 * interface. The interface abstracts from a specific thread implementation and therefore
 * supports to use different thread implementations, e.g. special thread management
 * frameworks that control the execution of threads on multiprocessor systems.
 *
 ========================================================================================= */

class ClientListener : public IClientListener {
private:
	// does not own the client
	BaseClient* m_baseClient;
	std::thread* m_listenerThread;
public:
	ClientListener(BaseClient* baseclient):m_baseClient(baseclient), m_listenerThread(nullptr) {}
	virtual ~ClientListener(){
		delete m_listenerThread;
	}
	void Start() override {
		m_listenerThread = new std::thread(BaseClient::listenHelper, this);
	}
	void Join() override {
		if (m_listenerThread != nullptr){
			m_listenerThread->join();
		}
	}
	void Stop() override {
		pthread_exit(0);
	}

};

}

#endif // CLIENTLISTENER_H
