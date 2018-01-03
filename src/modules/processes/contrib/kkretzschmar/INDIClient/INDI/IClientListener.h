#ifndef ICLIENTLISTENER_H
#define ICLIENTLISTENER_H

namespace INDI {
	class BaseClient;
}

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

class IClientListener {
public:
	virtual ~IClientListener() noexcept (false){}
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Join() = 0;
};

}

#endif // ICLIENTLISTENER_H
