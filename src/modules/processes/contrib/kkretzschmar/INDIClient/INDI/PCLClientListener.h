#ifndef CLIENTLISTENER_H
#define CLIENTLISTENER_H

#include "baseclient.h"
#include "IClientListener.h"

#include <pcl/Thread.h>
#include <pcl/Exception.h>

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

class PclClientListener : public IClientListener, public pcl::Thread {
private:
	// does not own the client
	BaseClient* m_baseClient;
public:
	PclClientListener(BaseClient* baseclient):m_baseClient(baseclient) {}
	virtual ~PclClientListener() {}

	void Run() override {
		try {
			BaseClient::listenHelper(m_baseClient);
		} catch (pcl::ProcessAborted& e) {
			// If a  thread is aborted a Processaborted is thrown and has to be catched here
		}
	}
	void Start() override {
		pcl::Thread::Start();
	}
	void Stop() override {
		pcl::Thread::Abort();
	}
	void Join() override {
		pcl::Thread::Wait();
	}

};

}

#endif // CLIENTLISTENER_H
