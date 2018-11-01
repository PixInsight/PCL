//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0225
// ----------------------------------------------------------------------------
// BaseClient.h - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

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

#ifndef INDIBASECLIENT_H
#define INDIBASECLIENT_H

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

#define MAXRBUF 2048

using namespace std;

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
class INDI::BaseClient : public INDI::BaseMediator
{
public:
    enum { INDI_DEVICE_NOT_FOUND=-1, INDI_PROPERTY_INVALID=-2, INDI_PROPERTY_DUPLICATED = -3, INDI_DISPATCH_ERROR=-4 };
    //typedef boost::shared_ptr<INDI::BaseDevice> devicePtr;

    virtual ~BaseClient(){};

    /** \brief Set the server host name and port
        \param hostname INDI server host name or IP address.
        \param port INDI server port.
    */
    virtual void setServer(const char * hostname, unsigned int port)=0;

    /** \brief Add a device to the watch list.

        A client may select to receive notifications of only a specific device or a set of devices.
        If the client encounters any of the devices set via this function, it will create a corresponding
        INDI::BaseDevice object to handle them. If no devices are watched, then all devices owned by INDI server
        will be created and handled.
    */
    virtual void watchDevice(const char * deviceName)=0;


    /** \brief Connect to INDI server.

        \returns True if the connection is successful, false otherwise.
        \note This function blocks until connection is either successull or unsuccessful.
    */
    virtual bool connectServer()=0;
	virtual bool serverIsConnected()=0;
    /** \brief Disconnect from INDI server.

        Disconnects from INDI servers. Any devices previously created will be deleted and memory cleared.
        \return True if disconnection is successful, false otherwise.
    */
    virtual bool disconnectServer()=0;

    /** \brief Connect to INDI driver
        \param deviceName Name of the device to connect to.
    */
    virtual void connectDevice(const char *deviceName)=0;

    /** \brief Disconnect INDI driver
        \param deviceName Name of the device to disconnect.
    */
    virtual void disconnectDevice(const char *deviceName)=0;

    /** \param deviceName Name of device to search for in the list of devices owned by INDI server,
         \returns If \e deviceName exists, it returns an instance of the device. Otherwise, it returns NULL.
    */
    virtual INDI::BaseDevice * getDevice(const char * deviceName)=0;

    /** \returns Returns a vector of all devices created in the client.
    */
    virtual const vector<INDI::BaseDevice *> & getDevices() const =0;

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
    virtual void setBLOBMode(BLOBHandling blobH, const char *dev, const char *prop = NULL)=0;

    // Update
    //static void * listenHelper(void *context);

    virtual const char * getHost() =0;
    virtual int getPort() =0;

    /** \brief Send new Text command to server */
    virtual void sendNewText (ITextVectorProperty *pp)=0;
    /** \brief Send new Text command to server */
    virtual void sendNewText (const char * deviceName, const char * propertyName, const char* elementName, const char *text)=0;
    /** \brief Send new Number command to server */
    virtual void sendNewNumber (INumberVectorProperty *pp)=0;
    /** \brief Send new Number command to server */
    virtual void sendNewNumber (const char * deviceName, const char *propertyName, const char* elementName, double value)=0;
    /** \brief Send new Switch command to server */
    virtual void sendNewSwitch (ISwitchVectorProperty *pp)=0;
    /** \brief Send new Switch command to server */
    virtual void sendNewSwitch (const char * deviceName, const char *propertyName, const char *elementName)=0;

    /** \brief Send opening tag for BLOB command to server */
    virtual void startBlob( const char *devName, const char *propName, const char *timestamp)=0;
    /** \brief Send ONE blob content to server */
    virtual void sendOneBlob( const char *blobName, unsigned int blobSize, const char *blobFormat, void * blobBuffer)=0;
    /** \brief Send closing tag for BLOB command to server */
    virtual void finishBlob()=0;


};

#endif // INDIBASECLIENT_H

// ----------------------------------------------------------------------------
// EOF BaseClient.h - Released 2018-11-01T11:07:21Z
