/*******************************************************************************
  Copyright(c) 2011 Jasem Mutlaq. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#ifndef INDIBASECLIENTIMPL_H
#define INDIBASECLIENTIMPL_H

#include <vector>
#include <map>
#include <string>

#if defined (WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#endif

#include "indiapi.h"
#include "indidevapi.h"
#include "indibase.h"
#include "BaseClient.h"

#define MAXRBUF 2048

#include <pcl/Thread.h>
#include <pcl/Mutex.h>

using namespace std;


class INDIListener : public pcl::Thread{
public:
	INDIListener(INDI::BaseClientImpl * client):pcl::Thread(),m_client(client){}
	virtual ~INDIListener(){}

	virtual void Run();
private:
	INDI::BaseClientImpl * m_client;
};


/**
 * \class INDI::BaseClient
   \brief Class to provide basic client functionality.

   BaseClient enables accelerated development of INDI Clients by providing a framework that facilitates communication, device
   handling, and event notification. By subclassing BaseClient, clients can quickly connect to an INDI server, and query for
   a set of INDI::BaseDevice devices, and read and write properties seamlessly. Event driven programming is possible due to
   notifications upon reception of new devices or properties.

   \attention All notifications functions defined in INDI::BaseMediator must be implemented in the client class even if
   they are not used because these are pure virtual functions.

   \see <a href=http://indilib.org/index.php?title=Learn_how_to_write_INDI_clients>INDI Client Tutorial</a> for more details.
   \author Jasem Mutlaq

 */
class INDI::BaseClientImpl : public INDI::BaseClient
{
public:
    enum { INDI_DEVICE_NOT_FOUND=-1, INDI_PROPERTY_INVALID=-2, INDI_PROPERTY_DUPLICATED = -3, INDI_DISPATCH_ERROR=-4 };
    //typedef boost::shared_ptr<INDI::BaseDevice> devicePtr;

    BaseClientImpl(bool initThreads=true);
    virtual ~BaseClientImpl();

    /** \brief Set the server host name and port
        \param hostname INDI server host name or IP address.
        \param port INDI server port.
    */
    virtual void setServer(const char * hostname, unsigned int port);

    /** \brief Add a device to the watch list.

        A client may select to receive notifications of only a specific device or a set of devices.
        If the client encounters any of the devices set via this function, it will create a corresponding
        INDI::BaseDevice object to handle them. If no devices are watched, then all devices owned by INDI server
        will be created and handled.
    */
    virtual void watchDevice(const char * deviceName);
    virtual bool isWatched(const char * deviceName);

    /** \brief Connect to INDI server.

        \returns True if the connection is successful, false otherwise.
        \note This function blocks until connection is either successull or unsuccessful.
    */
    virtual bool connectServer();
    virtual bool serverIsConnected(){return sConnected;}
    /** \brief Disconnect from INDI server.

        Disconnects from INDI servers. Any devices previously created will be deleted and memory cleared.
        \return True if disconnection is successful, false otherwise.
    */
    virtual bool disconnectServer();

    /** \brief Connect to INDI driver
        \param deviceName Name of the device to connect to.
    */
    virtual void connectDevice(const char *deviceName);

    /** \brief Disconnect INDI driver
        \param deviceName Name of the device to disconnect.
    */
    virtual void disconnectDevice(const char *deviceName);

    /** \param deviceName Name of device to search for in the list of devices owned by INDI server,
         \returns If \e deviceName exists, it returns an instance of the device. Otherwise, it returns NULL.
    */
    virtual INDI::BaseDevice * getDevice(const char * deviceName);

    /** \returns Returns a vector of all devices created in the client.
    */
    virtual const vector<INDI::BaseDevice *> & getDevices() const { return cDevices; }

    /** \brief Set Binary Large Object policy mode

      Set the BLOB handling mode for the client. The client may either receive:
      <ul>
      <li>Only BLOBS</li>
      <li>BLOBs mixed with normal messages</li>
      <li>Normal messages only, no BLOBs</li>
      </ul>

      If \e dev and \e prop are supplied, then the BLOB handling policy is set for this particular device and property.
      if \e prop is NULL, then the BLOB policy applies to the whole device.


      \param blobH BLOB handling policy
      \param dev name of device, required.
      \param prop name of property, optional.
    */
    virtual void setBLOBMode(BLOBHandling blobH, const char *dev, const char *prop = NULL);

    // Update
    static void * listenHelper(void *context);

    virtual const char * getHost() { return cServer.c_str();}
    virtual int getPort() { return cPort; }

    /** \brief Send new Text command to server */
    virtual void sendNewText (ITextVectorProperty *pp);
    /** \brief Send new Text command to server */
    virtual void sendNewText (const char * deviceName, const char * propertyName, const char* elementName, const char *text);
    /** \brief Send new Number command to server */
    virtual void sendNewNumber (INumberVectorProperty *pp);
    /** \brief Send new Number command to server */
    virtual void sendNewNumber (const char * deviceName, const char *propertyName, const char* elementName, double value);
    /** \brief Send new Switch command to server */
    virtual void sendNewSwitch (ISwitchVectorProperty *pp);
    /** \brief Send new Switch command to server */
    virtual void sendNewSwitch (const char * deviceName, const char *propertyName, const char *elementName);

    /** \brief Send opening tag for BLOB command to server */
    virtual void startBlob( const char *devName, const char *propName, const char *timestamp);
    /** \brief Send ONE blob content to server */
    virtual void sendOneBlob( const char *blobName, unsigned int blobSize, const char *blobFormat, void * blobBuffer);
    /** \brief Send closing tag for BLOB command to server */
    virtual void finishBlob();

    pcl::Mutex m_mutex;
    // Listen to INDI server and process incoming messages
    void listenINDI();

protected:

    virtual void newDevice(INDI::BaseDevice *dp) {}
    virtual void newProperty(INDI::Property *property){}
    virtual void removeProperty(INDI::Property *property){}
    virtual void newBLOB(IBLOB *bp){}
    virtual void newSwitch(ISwitchVectorProperty *svp){}
    virtual void newNumber(INumberVectorProperty *nvp){}
    virtual void newText(ITextVectorProperty *tvp){}
    virtual void newLight(ILightVectorProperty *lvp){}
    virtual void newMessage(INDI::BaseDevice *dp, int messageID){}
    virtual void serverConnected(){}
    virtual void serverDisconnected(int exit_code){}

    /** \brief Dispatch command received from INDI server to respective devices handled by the client */
    int dispatchCommand(XMLEle *root, char* errmsg);

    /** \brief Remove device */
    int removeDevice( const char * devName, char * errmsg );

    /** \brief Delete property command */
    int delPropertyCmd (XMLEle *root, char * errmsg);

    /** \brief Find and return a particular device */
    INDI::BaseDevice * findDev( const char * devName, char * errmsg);
    /** \brief Add a new device */
    INDI::BaseDevice * addDevice (XMLEle *dep, char * errmsg);
    /** \brief Find a device, and if it doesn't exist, create it if create is set to 1 */
    INDI::BaseDevice * findDev (XMLEle *root, int create, char * errmsg);

    /**  Process messages */
    int messageCmd (XMLEle *root, char * errmsg);



private:

    /** \brief Connect/Disconnect to INDI driver
        \param status If true, the client will attempt to turn on CONNECTION property within the driver (i.e. turn on the device).
         Otherwise, CONNECTION will be turned off.
        \param deviceName Name of the device to connect to.
    */
    void setDriverConnection(bool status, const char *deviceName);



    // Thread for listenINDI()
    INDIListener* m_listener;
    vector<INDI::BaseDevice *> cDevices;
    vector<string> cDeviceNames;

    string cServer;
    unsigned int cPort;
    bool sConnected;

    // Parse & FILE buffers for IO
#if defined(WIN32)
    SOCKET sockfd;
#else
	int sockfd;
#endif
    LilXML *lillp;			/* XML parser context */
    FILE *svrwfp;			/* FILE * to talk to server */
#if defined(WIN32)
	SOCKET m_receiveFd;
    SOCKET m_sendFd;
#else
	int m_receiveFd;
    int m_sendFd;
#endif

    void sendCommand(char *fmt, ...);
#if defined (WIN32)
    int socketpair(int af, int type, int proto, SOCKET sock[2]);
#endif
};

#endif // INDIBASECLIENTIMPL_H
