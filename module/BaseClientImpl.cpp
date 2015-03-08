/*******************************************************************************

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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#if defined(WIN32)
#include <io.h>
#include <stdio.h>
#define snprintf _snprintf
#else
#include <cstdarg>
#include <unistd.h>
#endif
#include <sys/types.h>

#if defined(WIN32)
#include <winsock2.h>
#include <WS2tcpip.h>
//#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <fcntl.h>

#include "BaseClientImpl.h"
#include "basedevice.h"
#include "indicom.h"

#include <pcl/Exception.h>

#include <errno.h>

#define MAXINDIBUF 256

void INDIListener::Run(){

	try {
		m_client->listenINDI();
	} catch (pcl::ProcessAborted& e){
		// do something
	}

}


INDI::BaseClientImpl::BaseClientImpl(bool initThreads)
{
    cServer = "localhost";
    cPort   = 7624;
    svrwfp = NULL;    
    sConnected = false;

    if (initThreads){
    	pcl::Thread::NumberOfThreads (1);
    }
#if defined(WIN32)
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
#endif
}


INDI::BaseClientImpl::~BaseClientImpl()
{
   // close(m_sendFd);
   // close(m_receiveFd);
}


void INDI::BaseClientImpl::setServer(const char * hostname, unsigned int port)
{
    cServer = hostname;
    cPort   = port;

}

void INDI::BaseClientImpl::watchDevice(const char * deviceName)
{
    cDeviceNames.push_back(deviceName);
}

bool INDI::BaseClientImpl::isWatched(const char * deviceName)
{
	assert(deviceName!=NULL);
	for (vector<string> ::iterator iter=cDeviceNames.begin(); iter!= cDeviceNames.end(); ++iter){
		if (strcmp(iter->c_str(),deviceName)==0){
			return true;
		}
	}
	return false;
}

bool INDI::BaseClientImpl::connectServer()
{
    struct sockaddr_in serv_addr;
    struct hostent *hp;
#if defined(WIN32)
    SOCKET pipefd[2];
#else
	int pipefd[2];
#endif
    int ret = 0;

    /* lookup host address */
	hp = gethostbyname(cServer.c_str());
    if (!hp)
    {
        perror ("gethostbyname");
        return false;
    }

    /* create a socket to the INDI server */
    //(void) memset ((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr_list[0]))->s_addr;
    serv_addr.sin_port = htons(cPort);
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("socket");
        return false;
    }

    /* connect */
    if (::connect (sockfd,(struct sockaddr *)&serv_addr,sizeof(sockaddr))<0)
    {
        perror ("connect");
        return false;
    }


#if defined(WIN32)
    ret = socketpair(AF_INET,SOCK_STREAM,IPPROTO_TCP,(SOCKET*)pipefd);
#else
    /* prepare for line-oriented i/o with client */
    svrwfp = fdopen (sockfd, "w");

    if (svrwfp == NULL)
    {
        perror("fdopen");
        return false;
    }
    ret = socketpair(PF_UNIX,SOCK_STREAM,0,pipefd);
#endif
    if (ret < 0)
    {
        IDLog("notify pipe: %s\n", strerror(errno));
        return false;
    }

    m_receiveFd = pipefd[0];
    m_sendFd = pipefd[1];

    m_listener = new INDIListener(this);
    m_listener->Start();

    sConnected = true;
    serverConnected();

    return true;
}

bool INDI::BaseClientImpl::disconnectServer()
{
    //IDLog("Server disconnected called\n");
    if (sConnected == false)
        return true;

    sConnected = false;
#if defined(WIN32)
#define SHUT_RDWR SD_BOTH
#endif

    shutdown(sockfd, SHUT_RDWR);
#if defined(WIN32) && !defined(_DEBUG)
    while (write(m_sendFd,"1",1) <= 0)
#endif


    if (svrwfp != NULL)
        fclose(svrwfp);
   svrwfp = NULL;


   return true;
}

void INDI::BaseClientImpl::connectDevice(const char *deviceName)
{
    setDriverConnection(true, deviceName);
}

void INDI::BaseClientImpl::disconnectDevice(const char *deviceName)
{
        setDriverConnection(false, deviceName);
}


void INDI::BaseClientImpl::setDriverConnection(bool status, const char *deviceName)
{
    INDI::BaseDevice *drv = getDevice(deviceName);
    ISwitchVectorProperty *drv_connection = NULL;

    if (drv == NULL)
    {
        IDLog("INDI::BaseClient: Error. Unable to find driver %s\n", deviceName);
        return;
    }

    drv_connection = drv->getSwitch("CONNECTION");

    if (drv_connection == NULL)
        return;

    // If we need to connect
    if (status)
    {
        // If there is no need to do anything, i.e. already connected.
        if (drv_connection->sp[0].s == ISS_ON)
            return;

        IUResetSwitch(drv_connection);
        drv_connection->s = IPS_BUSY;
        drv_connection->sp[0].s = ISS_ON;
        drv_connection->sp[1].s = ISS_OFF;

        sendNewSwitch(drv_connection);
    }
    else
    {
        // If there is no need to do anything, i.e. already disconnected.
        if (drv_connection->sp[1].s == ISS_ON)
            return;

        IUResetSwitch(drv_connection);
        drv_connection->s = IPS_BUSY;
        drv_connection->sp[0].s = ISS_OFF;
        drv_connection->sp[1].s = ISS_ON;

        sendNewSwitch(drv_connection);

    }
}


INDI::BaseDevice * INDI::BaseClientImpl::getDevice(const char * deviceName)
{
    vector<INDI::BaseDevice *>::const_iterator devi;
    for ( devi = cDevices.begin(); devi != cDevices.end(); devi++){
        if (!strcmp(deviceName, (*devi)->getDeviceName()))
            return (*devi);
	}

    return NULL;
}

void * INDI::BaseClientImpl::listenHelper(void *context)
{
  (static_cast<INDI::BaseClientImpl *> (context))->listenINDI();
  return NULL;
}

void INDI::BaseClientImpl::listenINDI()
{
    char buffer[MAXINDIBUF];
    char msg[MAXRBUF];

    int n=0, err_code=0;
    int maxfd=0;
    fd_set rs;

    if (cDeviceNames.empty())
       sendCommand((char*)"<getProperties version='%g'/>\n", INDIV);
    else
    {
        vector<string>::const_iterator stri;
        for ( stri = cDeviceNames.begin(); stri != cDeviceNames.end(); stri++)
            sendCommand((char*)"<getProperties version='%g' device='%s'/>\n", INDIV, (*stri).c_str());
    }

#if !defined(WIN32)
    fflush (svrwfp);
#endif 

    FD_ZERO(&rs);

    FD_SET((unsigned)sockfd, &rs);
    if (sockfd > maxfd)
        maxfd = sockfd;

    FD_SET((unsigned)m_receiveFd, &rs);
    if (m_receiveFd > maxfd)
        maxfd = m_receiveFd;


    lillp = newLilXML();

    /* read from server, exit if find all requested properties */
    while (sConnected)
    {
        n = select (maxfd+1, &rs, NULL, NULL, NULL);

        if (n < 0)
        {
            fprintf (stderr,"INDI server %s/%d disconnected.\n", cServer.c_str(), cPort);
#if defined(WIN32)  
			closesocket(sockfd);
#else
            close(sockfd);
#endif
            break;
        }

        // Received terminiation string from main thread
        if (n > 0 && FD_ISSET(m_receiveFd, &rs))
        {
            sConnected = false;
            break;
        }


        if (n > 0 && FD_ISSET(sockfd, &rs))
        {
#if defined(WIN32)  
          n = recv(sockfd, buffer, MAXINDIBUF, 0);//MSG_DONTWAIT);
#else
          n = recv(sockfd, buffer, MAXINDIBUF, MSG_DONTWAIT);
#endif

            if (n<=0)
            {

                if (n==0)
                {
                    fprintf (stderr,"INDI server %s/%d disconnected.\n", cServer.c_str(), cPort);
#if defined(WIN32)  
					closesocket(sockfd);
#else
                    close(sockfd);
#endif
                    sConnected = false;
                    break;
                }
                else
                    continue;
            }

            for (int i=0; i < n; i++)
            {
                //IDLog("Getting #%d bytes in for loop, calling readXMLEle for byte %d\n", n, i);
                XMLEle *root = readXMLEle (lillp, buffer[i], msg);
                //IDLog("############# BLOCK # POST READ XML ELE #####################\n");

                if (root)
                {
                    if ( (err_code = dispatchCommand(root, msg)) < 0)
                    {
                         // Silenty ignore property duplication errors
                         if (err_code != INDI_PROPERTY_DUPLICATED)
                         {
                             IDLog("Dispatch command error(%d): %s\n", err_code, msg);
                             prXMLEle (stderr, root, 0);
                         }
                    }

                   delXMLEle (root);	// not yet, delete and continue
                }
                else if (msg[0])
                {
                   fprintf (stderr, "Bad XML from %s/%d: %s\n%s\n", cServer.c_str(), cPort, msg, buffer);
                   m_listener->Abort();
                   return;
                }
            }
        }

    }

    delLilXML(lillp);

    serverDisconnected( (sConnected == false) ? 0 : -1);
    sConnected = false;

    m_listener->Abort();

}

int INDI::BaseClientImpl::dispatchCommand(XMLEle *root, char * errmsg)
{
   // printf("dispatch %s\n", tagXMLEle(root));
    if  (!strcmp (tagXMLEle(root), "message"))
        return messageCmd(root, errmsg);
    else if  (!strcmp (tagXMLEle(root), "delProperty"))
        return delPropertyCmd(root, errmsg);

    /* Get the device, if not available, create it */
    INDI::BaseDevice *dp = findDev (root, 1, errmsg);
    if (dp == NULL)
    {
        strcpy(errmsg,"No device available and none was created");
        return INDI_DEVICE_NOT_FOUND;
    }

    // FIXME REMOVE THIS

    // Ignore echoed newXXX
    if (strstr(tagXMLEle(root), "new"))
        return 0;

    if ((!strcmp (tagXMLEle(root), "defTextVector"))  ||
       (!strcmp (tagXMLEle(root), "defNumberVector")) ||
       (!strcmp (tagXMLEle(root), "defSwitchVector")) ||
       (!strcmp (tagXMLEle(root), "defLightVector"))  ||
       (!strcmp (tagXMLEle(root), "defBLOBVector")))
        return dp->buildProp(root, errmsg);
    else if (!strcmp (tagXMLEle(root), "setTextVector") ||
             !strcmp (tagXMLEle(root), "setNumberVector") ||
             !strcmp (tagXMLEle(root), "setSwitchVector") ||
             !strcmp (tagXMLEle(root), "setLightVector") ||
             !strcmp (tagXMLEle(root), "setBLOBVector"))
            return dp->setValue(root, errmsg);

    return INDI_DISPATCH_ERROR;
}

/* delete the property in the given device, including widgets and data structs.
 * when last property is deleted, delete the device too.
 * if no property name attribute at all, delete the whole device regardless.
 * return 0 if ok, else -1 with reason in errmsg[].
 */
int INDI::BaseClientImpl::delPropertyCmd (XMLEle *root, char * errmsg)
{
    XMLAtt *ap;
    INDI::BaseDevice *dp;

    /* dig out device and optional property name */
    dp = findDev (root, 0, errmsg);
    if (!dp)
        return INDI_DEVICE_NOT_FOUND;

    dp->checkMessage(root);

    ap = findXMLAtt (root, "name");

    /* Delete property if it exists, otherwise, delete the whole device */
    if (ap)
    {
        INDI::Property *rProp = dp->getProperty(valuXMLAtt(ap));
        removeProperty(rProp);
        int errCode = dp->removeProperty(valuXMLAtt(ap), errmsg);
		rProp = NULL;
        
		return errCode;
    }
    // delete the whole device
    else {
    	deleteDevice(dp);
        return removeDevice(dp->getDeviceName(), errmsg);
    }
}

int INDI::BaseClientImpl::removeDevice( const char * devName, char * errmsg )
{
    std::vector<INDI::BaseDevice *>::iterator devicei;

    for (devicei =cDevices.begin(); devicei != cDevices.end();)
    {

      if (!strcmp(devName, (*devicei)->getDeviceName()))
      {
          delete *devicei;
          devicei = cDevices.erase(devicei);
          return 0;
      }
      else
          ++devicei;
    }

    snprintf(errmsg, MAXRBUF, "Device %s not found", devName);
    return INDI_DEVICE_NOT_FOUND;
}

INDI::BaseDevice * INDI::BaseClientImpl::findDev( const char * devName, char * errmsg )
{

    std::vector<INDI::BaseDevice *>::const_iterator devicei;

    for (devicei = cDevices.begin(); devicei != cDevices.end(); devicei++)
    {
        if ((*devicei)!=NULL && !strcmp(devName, (*devicei)->getDeviceName()))
         return (*devicei);

    }

    snprintf(errmsg, MAXRBUF, "Device %s not found", devName);
    return NULL;
}

/* add new device */
INDI::BaseDevice * INDI::BaseClientImpl::addDevice (XMLEle *dep, char * errmsg)
{
    //devicePtr dp(new INDI::BaseDriver());
    INDI::BaseDevice *dp = new INDI::BaseDevice();
    XMLAtt *ap;
    char * device_name;

    /* allocate new INDI::BaseDriver */
    ap = findXMLAtt (dep, "device");
    if (!ap)
    {
        strncpy(errmsg, "Unable to find device attribute in XML element. Cannot add device.", MAXRBUF);
        return NULL;
    }

    device_name = valuXMLAtt(ap);

    dp->setMediator(this);
    dp->setDeviceName(device_name);

    cDevices.push_back(dp);

    newDevice(dp);

    /* ok */
    return dp;
}

INDI::BaseDevice * INDI::BaseClientImpl::findDev (XMLEle *root, int create, char * errmsg)
{
    XMLAtt *ap;
    INDI::BaseDevice *dp;
    char *dn;

    /* get device name */
    ap = findXMLAtt (root, "device");
    if (!ap)
    {
        snprintf(errmsg, MAXRBUF, "No device attribute found in element %s", tagXMLEle(root));
        return (NULL);
    }

    dn = valuXMLAtt(ap);

    dp = findDev(dn, errmsg);

    if (dp)
        return dp;

    /* not found, create if ok */
    if (create)
        return (addDevice (root, errmsg));

    snprintf(errmsg, MAXRBUF, "INDI: <%s> no such device %s", tagXMLEle(root), dn);
    return NULL;
}

/* a general message command received from the device.
 * return 0 if ok, else -1 with reason in errmsg[].
 */
int INDI::BaseClientImpl::messageCmd (XMLEle *root, char * errmsg)
{
    INDI::BaseDevice *dp =findDev (root, 0, errmsg);

    if (dp)
        dp->checkMessage(root);

    return (0);
}


void INDI::BaseClientImpl::sendNewText (ITextVectorProperty *tvp)
{
    tvp->s = IPS_BUSY;

    sendCommand((char*)"<newTextVector\n");
    sendCommand((char*)"  device='%s'\n", tvp->device);
    sendCommand((char*)"  name='%s'\n>", tvp->name);

    for (int i=0; i < tvp->ntp; i++)
    {
        sendCommand((char*)"  <oneText\n");
        sendCommand((char*)"    name='%s'>\n", tvp->tp[i].name);
        sendCommand((char*)"      %s\n", tvp->tp[i].text);
        sendCommand((char*)"  </oneText>\n");
    }
    sendCommand((char*)"</newTextVector>\n");

    fflush(svrwfp);
}

void INDI::BaseClientImpl::sendNewText (const char * deviceName, const char * propertyName, const char* elementName, const char *text)
{

    INDI::BaseDevice *drv = getDevice(deviceName);

    if (drv == NULL)
        return;

    ITextVectorProperty *tvp = drv->getText(propertyName);

    if (tvp == NULL)
        return;

    IText * tp = IUFindText(tvp, elementName);

    if (tp == NULL)
        return;

    IUSaveText(tp, text);

    sendNewText(tvp);
}

void INDI::BaseClientImpl::sendNewNumber (INumberVectorProperty *nvp)
{
    nvp->s = IPS_BUSY;

    sendCommand((char*)"<newNumberVector\n");
    sendCommand((char*)"  device='%s'\n", nvp->device);
    sendCommand((char*)"  name='%s'\n>", nvp->name);

    for (int i=0; i < nvp->nnp; i++)
    {
        sendCommand((char*)"  <oneNumber\n");
        sendCommand((char*)"    name='%s'>\n", nvp->np[i].name);
        sendCommand((char*)"      %g\n", nvp->np[i].value);
        sendCommand((char*)"  </oneNumber>\n");
    }
    sendCommand((char*)"</newNumberVector>\n");

   fflush(svrwfp);
}

void INDI::BaseClientImpl::sendNewNumber (const char *deviceName, const char *propertyName, const char* elementName, double value)
{
    INDI::BaseDevice *drv = getDevice(deviceName);

    if (drv == NULL)
        return;

    INumberVectorProperty *nvp = drv->getNumber(propertyName);

    if (nvp == NULL)
        return;

    INumber * np = IUFindNumber(nvp, elementName);

    if (np == NULL)
        return;

    np->value = value;

    sendNewNumber(nvp);

}

void INDI::BaseClientImpl::sendNewSwitch (ISwitchVectorProperty *svp)
{
    svp->s = IPS_BUSY;
    ISwitch *onSwitch = IUFindOnSwitch(svp);

    sendCommand((char*)"<newSwitchVector\n");

    sendCommand((char*)"  device='%s'\n", svp->device);
    sendCommand((char*)"  name='%s'>\n", svp->name);

    if (svp->r == ISR_1OFMANY && onSwitch)
    {
        sendCommand((char*)"  <oneSwitch\n");
        sendCommand((char*)"    name='%s'>\n", onSwitch->name);
        sendCommand((char*)"      %s\n", (onSwitch->s == ISS_ON) ? "On" : "Off");
        sendCommand((char*)"  </oneSwitch>\n");
    }
    else
    {
        for (int i=0; i < svp->nsp; i++)
        {
            sendCommand((char*)"  <oneSwitch\n");
            sendCommand((char*)"    name='%s'>\n", svp->sp[i].name);
            sendCommand((char*)"      %s\n", (svp->sp[i].s == ISS_ON) ? "On" : "Off");
            sendCommand((char*)"  </oneSwitch>\n");

        }
    }

    sendCommand((char*)"</newSwitchVector>\n");

    fflush(svrwfp);
}

void INDI::BaseClientImpl::sendNewSwitch (const char *deviceName, const char *propertyName, const char *elementName)
{

    INDI::BaseDevice *drv = getDevice(deviceName);

    if (drv == NULL)
        return;

    ISwitchVectorProperty *svp = drv->getSwitch(propertyName);

    if (svp == NULL)
        return;

    ISwitch * sp = IUFindSwitch(svp, elementName);

    if (sp == NULL)
        return;

	IUResetSwitch(svp);
    sp->s = ISS_ON;

    sendNewSwitch(svp);

}

void INDI::BaseClientImpl::startBlob( const char *devName, const char *propName, const char *timestamp)
{
    sendCommand((char*)"<newBLOBVector\n");
    sendCommand((char*)"  device='%s'\n", devName);
    sendCommand((char*)"  name='%s'\n", propName);
    sendCommand((char*)"  timestamp='%s'>\n",  timestamp);
}

void INDI::BaseClientImpl::sendOneBlob( const char *blobName, unsigned int blobSize, const char *blobFormat, void * blobBuffer)
{
    sendCommand((char*)"  <oneBLOB\n");
    sendCommand((char*)"    name='%s'\n", blobName);
    sendCommand((char*)"    size='%d'\n", blobSize);
    sendCommand((char*)"    format='%s'>\n", blobFormat);

    for (unsigned i = 0; i < blobSize; i += 72)
        sendCommand((char*)"    %.72s\n", ((char *) blobBuffer+i));

    sendCommand((char*)"   </oneBLOB>\n");
}

void INDI::BaseClientImpl::finishBlob()
{
    sendCommand((char*)"</newBLOBVector>\n");
    fflush(svrwfp);

}

void INDI::BaseClientImpl::setBLOBMode(BLOBHandling blobH, const char *dev, const char *prop)
{
    char blobOpenTag[MAXRBUF];

    if (!dev[0])
        return;

   if (prop != NULL)
           snprintf(blobOpenTag, MAXRBUF, "<enableBLOB device='%s' name='%s'>", dev, prop);
   else
          snprintf(blobOpenTag, MAXRBUF, "<enableBLOB device='%s'>", dev);

    switch (blobH)
    {
    case B_NEVER:
        sendCommand((char*)"%sNever</enableBLOB>\n", blobOpenTag);
        break;
    case B_ALSO:
        sendCommand((char*)"%sAlso</enableBLOB>\n", blobOpenTag);
        break;
    case B_ONLY:
        sendCommand((char*)"%sOnly</enableBLOB>\n", blobOpenTag);
        break;
    }

    fflush(svrwfp);
}

#if defined(WIN32)
int INDI::BaseClientImpl::socketpair(int af, int type, int proto, SOCKET socks[2]) {
  int make_overlapped=0;
  union {
       struct sockaddr_in inaddr;
       struct sockaddr addr;
    } a;
    SOCKET listener;
    int e;
    socklen_t addrlen = sizeof(a.inaddr);
    DWORD flags = (make_overlapped ? WSA_FLAG_OVERLAPPED : 0);
    int reuse = 1;

    if (socks == 0) {
      WSASetLastError(WSAEINVAL);
      return SOCKET_ERROR;
    }

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) 
        return SOCKET_ERROR;

    memset(&a, 0, sizeof(a));
    a.inaddr.sin_family = AF_INET;
    a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.inaddr.sin_port = 0; 

    socks[0] = socks[1] = INVALID_SOCKET;
    do {
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, 
               (char*) &reuse, (socklen_t) sizeof(reuse)) == -1)
            break;
        if  (bind(listener, &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR)
            break;
        if  (getsockname(listener, &a.addr, &addrlen) == SOCKET_ERROR)
            break;
        if (listen(listener, 1) == SOCKET_ERROR)
            break;
        socks[0] = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, flags);
        if (socks[0] == INVALID_SOCKET)
            break;
        if (connect(socks[0], &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR)
            break;
        socks[1] = accept(listener, NULL, NULL);
        if (socks[1] == INVALID_SOCKET)
            break;

        closesocket(listener);
        return 0;

    } while (0);

    e = WSAGetLastError();
    closesocket(listener);
    closesocket(socks[0]);
    closesocket(socks[1]);
    WSASetLastError(e);
    return SOCKET_ERROR;

}
#endif 

void INDI::BaseClientImpl::sendCommand(char *fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);

    char sendbuf[2048];
    vsprintf(sendbuf, fmt, argList);
    va_end(argList);

#if defined(WIN32)
    int iResult = send(sockfd , sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR)
    {
        int k=WSAGetLastError();
        //perror("send failed with error: %d\n", WSAGetLastError());
    }
#else
	fprintf(svrwfp,sendbuf);
#endif
}
