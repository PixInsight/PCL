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
// Copyright (c) 2014-2019 Klaus Kretzschmar
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

#include "IndigoClient.h"
#include <unistd.h>
#include <string.h>

#include "indigo_json.h"
#include "indigo_driver_json.h"

IndigoClient::IndigoClient(const char* clientName) {
   strncpy(m_indigoClient.name, clientName, strlen(clientName));
   m_indigoClient.client_context =  reinterpret_cast<void*>(this);
   indigo_start();
}

IndigoClient::IndigoClient(const char* clientName, const char* host, int32_t port): m_serverHost(host), m_port(static_cast<uint32_t>(port)) {
   strncpy(m_indigoClient.name, clientName, strlen(clientName));
   m_indigoClient.client_context =  reinterpret_cast<void*>(this);
   indigo_start();
   indigo_attach_client(&m_indigoClient);
}

IndigoClient::~IndigoClient() {
    indigo_detach_client(&m_indigoClient);
    indigo_stop();
    for (auto iter = m_devices.begin(); iter != m_devices.end(); iter++) {
       indigo_remove_driver(*iter);
    }
}


bool IndigoClient::connectServer(std::ostream &errMessage) {
   indigo_server_entry* serverEntry = nullptr;
   indigo_result rc = indigo_connect_server(m_serverHost.c_str(), m_serverHost.c_str(), m_port, &serverEntry);
   if (rc != INDIGO_OK) {
      errMessage <<"Internal error: Server connection failed: rc = "<<rc<<" -- "<<__FILE__<<":"<<__LINE__;
      return false;
   }
   return true;
}

bool IndigoClient::disconnectServer() {
   indigo_server_entry* server = getServerEntry(m_serverHost.c_str(), static_cast<uint32_t>(m_port));
   if (server != nullptr) {
      indigo_result rc = indigo_disconnect_server(server);
      if (rc != INDIGO_OK){
         return false;
      }
      while (server->thread_started) {
      }
   }
   return true;
}

bool IndigoClient::serverIsConnected(std::ostream& errMessage) const {
   indigo_server_entry* server = getServerEntry(m_serverHost.c_str(), static_cast<uint32_t>(m_port));
   if (server == nullptr) {
      return false;
   } else if (server->socket <= 0){
      if (strlen(server->last_error) != 0){
         errMessage <<"Error: "<<server->last_error;
      }
      return false;
   }
   return true;
}


 bool IndigoClient::sendNewProperty(indigo_property *property) {
   indigo_result rc = indigo_change_property(&m_indigoClient, property);
   return rc == INDIGO_OK;
 }

 bool IndigoClient::sendNewNumberProperty(char* deviceName,  const char* propertyName, size_t numOfItems, const char** items, const double* values) {
    indigo_property *property = indigo_init_number_property(NULL, deviceName, propertyName, NULL, NULL, static_cast<indigo_property_state>(0), static_cast<indigo_property_perm>(0), numOfItems);
    for (size_t i = 0; i < numOfItems; i++)
       indigo_init_number_item(&property->items[i], items[i], NULL, 0, 0, 0, values[i]);
    indigo_result rc = indigo_change_property(&m_indigoClient, property);
    if (rc == INDIGO_OK){
        property->state = INDIGO_BUSY_STATE;
        newNumber(property);
        free(property);
        return true;
    }
    free(property);
    return false;
 }


 bool IndigoClient::sendNewSwitchProperty(char* deviceName,  const char* propertyName, size_t numOfItems, const char** items, const bool* values) {
     indigo_property *property = indigo_init_switch_property(NULL, deviceName, propertyName, NULL, NULL, static_cast<indigo_property_state>(0), static_cast<indigo_property_perm>(0), static_cast<indigo_rule>(0), numOfItems);
     for (size_t i = 0; i < numOfItems; i++)
        indigo_init_switch_item(&property->items[i], items[i], NULL, values[i]);
     indigo_result rc = indigo_change_property(&m_indigoClient, property);
     if (rc == INDIGO_OK){
         property->state = INDIGO_BUSY_STATE;
         newSwitch(property);
         free(property);
         return true;
     }
     free(property);
     return false;
  }

 bool IndigoClient::sendNewTextProperty(char* deviceName,  const char* propertyName, size_t numOfItems, const char** items, const char **values) {
     indigo_property *property = indigo_init_text_property(NULL, deviceName, propertyName, NULL, NULL, static_cast<indigo_property_state>(0), static_cast<indigo_property_perm>(0), numOfItems);
     for (size_t i = 0; i < numOfItems; i++)
        indigo_init_text_item(&property->items[i], items[i], NULL, values[i]);
     indigo_result rc = indigo_change_property(&m_indigoClient, property);
     if (rc == INDIGO_OK){
         property->state = INDIGO_BUSY_STATE;
         newText(property);
         free(property);
         return true;
     }
     free(property);
     return false;
  }

indigo_result IndigoClient::clientAttach(indigo_client* client) {
   indigo_enumerate_properties(client, &INDIGO_ALL_PROPERTIES);
   return INDIGO_OK;
}

indigo_result IndigoClient::defineProperty(indigo_client *client, indigo_device *device, indigo_property *property, const char *message) {
   IndigoClient* indigoClient = reinterpret_cast<IndigoClient*>(client->client_context);
   if (property == nullptr) {
      return INDIGO_FAILED;
   }
   indigoClient->newProperty(property);

   if (property->type == INDIGO_TEXT_VECTOR ) {
      if (!strcmp(property->name,"INFO")){
         std::string deviceName(property->device);
         indigoClient->newDevice(deviceName);
      }
   } else if (property->type == INDIGO_BLOB_VECTOR) {
      indigo_enable_blob(client, property, INDIGO_ENABLE_BLOB_ALSO);
   }
   return INDIGO_OK;
}

indigo_result IndigoClient::deleteProperty(indigo_client *client, indigo_device *device, indigo_property *property, const char *message) {
   IndigoClient* indigoClient = reinterpret_cast<IndigoClient*>(client->client_context);
   if (property == nullptr) {
      return INDIGO_FAILED;
   }
   indigoClient->removeProperty(property);

   if (property->type == INDIGO_TEXT_VECTOR ) {
      if (!strcmp(property->name,"INFO")){
         std::string deviceName(property->items[0].text.value);
         indigoClient->removeDevice(deviceName);
      }
   }
   return INDIGO_OK;
}

indigo_result IndigoClient::updateProperty(indigo_client *client, indigo_device *device, indigo_property *property, const char *message) {
   IndigoClient* indigoClient = reinterpret_cast<IndigoClient*>(client->client_context);
   if (property == nullptr) {
      return INDIGO_FAILED;
   }
   if (property->type == INDIGO_SWITCH_VECTOR) {
      indigoClient->newSwitch(property);
   } else if (property->type == INDIGO_NUMBER_VECTOR){
      indigoClient->newNumber(property);
   } else if (property->type == INDIGO_TEXT_VECTOR) {
      indigoClient->newText(property);
   } else if (property->type == INDIGO_LIGHT_VECTOR){
      indigoClient->newLight(property);
   } else if (property->type == INDIGO_BLOB_VECTOR) {
      indigoClient->newBlob(property);
   }
   else {
      return INDIGO_FAILED;
   }
   return INDIGO_OK;
}


indigo_result IndigoClient::getMessage(indigo_client *client, indigo_device *device, const char *message) {
   IndigoClient* indigoClient = reinterpret_cast<IndigoClient*>(client->client_context);
   if (message != nullptr) {
      indigoClient->newMessage(message);
      return INDIGO_OK;
   }
   return INDIGO_FAILED;
}


indigo_server_entry* IndigoClient::getServerEntry(const char* host, int32_t port) const {
   for (int dc = 0; dc < INDIGO_MAX_SERVERS; dc++) {
      if (!strcmp(indigo_available_servers[dc].host, host) && indigo_available_servers[dc].port == port){
         return  &indigo_available_servers[dc];
      }
   }
   return nullptr;
}

bool IndigoClient::loadDeviceDriver(const std::string& driver) {
   indigo_driver_entry* driverEntry = nullptr;
   indigo_result rc = indigo_load_driver(driver.c_str(), true, &driverEntry);
   if (driverEntry != nullptr) {
      m_devices.insert(driverEntry);
   }
   indigo_attach_client(&m_indigoClient);
   return true;
}

bool IndigoClient::connectDevice(const std::string& deviceName)  {
   indigo_result rc = indigo_device_connect(&m_indigoClient, const_cast<char*>(deviceName.c_str()));
   return rc == INDIGO_OK;
}

bool IndigoClient::disconnectDevice(const std::string& deviceName)  {
   indigo_result rc = indigo_device_disconnect(&m_indigoClient, const_cast<char*>(deviceName.c_str()));
   return rc == INDIGO_OK;
}
