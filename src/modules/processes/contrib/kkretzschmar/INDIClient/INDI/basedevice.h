//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.04.0108
// ----------------------------------------------------------------------------
// basedevice.h - Released 2016/04/15 15:37:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#ifndef INDIBASEDRIVER_H
#define INDIBASEDRIVER_H

#include <vector>
#include <string>


#include "indiapi.h"
#include "indidevapi.h"
#include "indibase.h"
#include "indiproperty.h"

#define MAXRBUF 2048

/**
 * \class INDI::BaseDevice
   \brief Class to provide basic INDI device functionality.

   INDI::BaseDevice is the base device for all INDI devices and contains a list of all properties defined by the device either explicity or via a skeleton file.
   You don't need to subclass INDI::BaseDevice class directly, it is inheritied by INDI::DefaultDevice which takes care of building a standard INDI device. Moreover, INDI::BaseClient
   maintains a list of INDI::BaseDevice objects as they get defined from the INDI server, and those objects may be accessed to retrieve information on the object properties or message log.

   \author Jasem Mutlaq
 */
class INDI::BaseDevice
{
public:
    BaseDevice();
    ~BaseDevice();

    /*! INDI error codes. */
    enum INDI_ERROR
    {
        INDI_DEVICE_NOT_FOUND=-1,       /*!< INDI Device was not found. */
        INDI_PROPERTY_INVALID=-2,       /*!< Property has an invalid syntax or attribute. */
        INDI_PROPERTY_DUPLICATED = -3,  /*!< INDI Device was not found. */
        INDI_DISPATCH_ERROR=-4          /*!< Dispatching command to driver failed. */
    };

    /** \return Return vector number property given its name */
    INumberVectorProperty * getNumber(const char *name);
    /** \return Return vector text property given its name */
    ITextVectorProperty * getText(const char *name);
    /** \return Return vector switch property given its name */
    ISwitchVectorProperty * getSwitch(const char *name);
    /** \return Return vector light property given its name */
    ILightVectorProperty * getLight(const char *name);
    /** \return Return vector BLOB property given its name */
    IBLOBVectorProperty * getBLOB(const char *name);

    void registerProperty(void *p, INDI_TYPE type);

    /** \brief Remove a property
        \param name name of property to be removed
        \param errmsg buffer to store error message.
        \return 0 if successul, -1 otherwise.
    */
    int removeProperty(const char *name, char *errmsg);

    /** \brief Return a property and its type given its name.
        \param name of property to be found.
        \param type of property found.
        \return If property is found, the raw void * pointer to the IXXXVectorProperty is returned. To be used you must use static_cast with given the type of property
        returned. For example, INumberVectorProperty *num = static_cast<INumberVectorProperty> getRawProperty("FOO", INDI_NUMBER);

        \note This is a low-level function and should not be called directly unless necessary. Use getXXX instead where XXX
        is the property type (Number, Text, Switch..etc).

    */
    void * getRawProperty(const char *name, INDI_TYPE type = INDI_UNKNOWN);

    /** \brief Return a property and its type given its name.
        \param name of property to be found.
        \param type of property found.
        \return If property is found, it is returned. To be used you must use static_cast with given the type of property
        returned.
    */
    INDI::Property * getProperty(const char *name, INDI_TYPE type = INDI_UNKNOWN);

    /** \brief Return a list of all properties in the device.
    */
    std::vector<INDI::Property *> * getProperties() { return &pAll; }
	std::vector<INDI::Property *>  getPropertiesSafe() { return pAll; }

    /** \brief Build driver properties from a skeleton file.
        \param filename full path name of the file.

    A skeloton file defines the properties supported by this driver. It is a list of defXXX elements enclosed by @<INDIDriver>@
 and @</INDIDriver>@ opening and closing tags. After the properties are created, they can be rerieved, manipulated, and defined
 to other clients.

 \see An example skeleton file can be found under examples/tutorial_four_sk.xml

    */
    void buildSkeleton(const char *filename);

    /** \return True if the device is connected (CONNECT=ON), False otherwise */
    bool isConnected();

    /** \brief Set the device name
      \param dev new device name
      */
    void setDeviceName(const char *dev);

    /** \return Returns the device name */
    const char *getDeviceName();

    /** \brief Add message to the driver's message queue.
        \param msg Message to add.
    */
    void addMessage(const char *msg);


    void checkMessage (XMLEle *root);
    void doMessage (XMLEle *msg);

    /** \return Returns a specific message. */
    const char * messageQueue(int index);

    /** \return Returns last message message. */
    const char * lastMessage();

    /** \brief Set the driver's mediator to receive notification of news devices and updated property values. */
    void setMediator(INDI::BaseMediator *med) { mediator = med; }

    /** \returns Get the meditator assigned to this driver */
    INDI::BaseMediator * getMediator() { return mediator; }

    /** \return driver name
     *  \note This can only be valid if DRIVER_INFO is defined by the driver.
     **/
    const char *getDriverName();

    /** \return driver executable name
     *  \note This can only be valid if DRIVER_INFO is defined by the driver.
     **/
    const char *getDriverExec();

protected:

    /** \brief Build a property given the supplied XML element (defXXX)
      \param root XML element to parse and build.
      \param errmsg buffer to store error message in parsing fails.
      \return 0 if parsing is successful, -1 otherwise and errmsg is set */
    int buildProp(XMLEle *root, char *errmsg);

    /** \brief handle SetXXX commands from client */
    int setValue (XMLEle *root, char * errmsg);
    /** \brief Parse and store BLOB in the respective vector */
    int setBLOB(IBLOBVectorProperty *pp, XMLEle * root, char * errmsg);

private:

    char *deviceID;

    std::vector<INDI::Property *> pAll;

    LilXML *lp;

    std::vector<const char *> messageLog;

    INDI::BaseMediator *mediator;

    friend class INDI::BaseClientImpl;
    friend class INDI::DefaultDevice;

};

#endif // INDIBASEDRIVER_H

// ----------------------------------------------------------------------------
// EOF basedevice.h - Released 2016/04/15 15:37:39 UTC
