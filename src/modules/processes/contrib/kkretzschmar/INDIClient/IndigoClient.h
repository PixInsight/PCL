//     ____   ______ __
//    / __  / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     ____//_____/   PCL
// ----------------------------------------------------------------------------
// Standard  Process Module Version
// ----------------------------------------------------------------------------
// : - Released 08.12.2018
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

#ifndef INDIGOCLIENT_H
#define INDIGOCLIENT_H

#include<string>
#include<set>
#include<functional>
#include<iostream>

#include "indigo/indigo_client.h"

class IndigoClient
{
   typedef indigo_result (*clientAttachCallBack_t)(indigo_client*) ;
   typedef indigo_result (*definePropertyCallBack_t)(indigo_client *client, indigo_device *device, indigo_property *property, const char *message);
   typedef indigo_result (*deletePropertyCallBack_t)(indigo_client *client, indigo_device *device, indigo_property *property, const char *message);
   typedef indigo_result (*updatePropertyCallBack_t)(indigo_client *client, indigo_device *device, indigo_property *property, const char *message);
   struct indigo_client m_indigoClient = {
      "PixInsight", false, nullptr, INDIGO_OK, INDIGO_VERSION_CURRENT, nullptr,
      reinterpret_cast<clientAttachCallBack_t>(&IndigoClient::clientAttach),
      reinterpret_cast<definePropertyCallBack_t>(&IndigoClient::defineProperty),
      reinterpret_cast<updatePropertyCallBack_t>(&IndigoClient::updateProperty),
      reinterpret_cast<deletePropertyCallBack_t>(&IndigoClient::deleteProperty),
      nullptr,
      nullptr
   };
   std::string m_serverHost;
   uint32_t    m_port;
   std::set<indigo_driver_entry*>    m_devices;
public:
   explicit IndigoClient(const char* clientName);
   IndigoClient(const char* clientName, const char* host, int32_t port);
   IndigoClient(const IndigoClient&) = delete;
   IndigoClient(IndigoClient&) = delete;

   ~IndigoClient();  // do not make it virtual, no vtable allowed
   bool connectServer(std::ostream& errMessage);
   bool disconnectServer();
   bool serverIsConnected(std::ostream& errMessage) const;
   bool sendNewProperty(indigo_property *property);
   bool loadDeviceDriver(const std::string& driver);
   std::function<void(const std::string&)> newDevice = [] (const std::string& ){};
   std::function<void(const std::string&)> removeDevice = [] (const std::string& ){};
   std::function<void(indigo_property*)> newProperty = [] (indigo_property*){};
   std::function<void(indigo_property*)> removeProperty = [] (indigo_property*){};
   std::function<void(indigo_property*)> newSwitch = [] (indigo_property*){};
   std::function<void(indigo_property*)> newNumber = [] (indigo_property*){};
   std::function<void(indigo_property*)> newText = [] (indigo_property*){};
   std::function<void(indigo_property*)> newLight = [] (indigo_property*){};
 private:
   indigo_server_entry* getServerEntry(const char* host, int32_t port) const;
   static indigo_result clientAttach(indigo_client *client);
   static indigo_result defineProperty(indigo_client *client, indigo_device *device, indigo_property *property, const char *message);
   static indigo_result deleteProperty(indigo_client *client, indigo_device *device, indigo_property *property, const char *message);
   static indigo_result updateProperty(indigo_client *client, indigo_device *device, indigo_property *property, const char *message);
};

#endif // INDIGOCLIENT_H
