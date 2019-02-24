//     ____   ______ __
//    / __  / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     ____//_____/   PCL
// ----------------------------------------------------------------------------
// Standard  Process Module Version
// ----------------------------------------------------------------------------
// : - Released 07.12.2018
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
//    Alternatively, if that is where third-party acknowledgments normallytrue
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED trueWARRANTIES, INCLUDING, BUT NOT LIMITED
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

#include<set>
#include<map>
#include<pthread.h>

#include "gtest/gtest.h"

#include "../INDIClient/IndigoClient.h"
#include "../INDIClient/IIndigoProperty.h"

pthread_mutex_t s_mutex;

enum class TestMode {
   server,
   local
};

TestMode g_testMode;

namespace pcl {



class IndigoClientTest : public testing::Test {
};

class TestExecution {
public:
   static void attachClient (IndigoClient* indigoClient, std::ostream& errMsg) {
      if (g_testMode == TestMode::server) {
         ASSERT_TRUE(indigoClient->connectServer(errMsg));
         sleep(1);
         ASSERT_TRUE(indigoClient->serverIsConnected(errMsg));
      } else {
         indigoClient->loadDeviceDriver("/home/klaus/data/Cpp/indigo/build/drivers/indigo_ccd_simulator.so");
      }
   }

   static void detachClient(IndigoClient* indigoClient) {
      if (g_testMode == TestMode::server) {
         ASSERT_TRUE(indigoClient->disconnectServer());
      }
   }

   static std::unique_ptr<IndigoClient> createClient(const char* host, int32_t port) {
      if (g_testMode == TestMode::server) {
         return std::unique_ptr<IndigoClient>(new IndigoClient("TestClient", host, port));
      } else {
         return std::unique_ptr<IndigoClient>(new IndigoClient("TestClient"));
      }
   }

   static std::string getConnectionPropertyString() {
      if (g_testMode == TestMode::server) {
         return std::string("/CCD Imager Simulator @ localhost/CONNECTION");
      } else {
         return std::string("/CCD Imager Simulator/CONNECTION");
      }
   }

   static std::string getCoolerPropertyString() {
      if (g_testMode == TestMode::server) {
         return std::string("/CCD Imager Simulator @ localhost/CCD_COOLER");
      } else {
         return std::string("/CCD Imager Simulator/CCD_COOLER");
      }
   }

   static std::string getCCDImagerDeviceString() {
      if (g_testMode == TestMode::server) {
         return std::string("CCD Imager Simulator @ localhost");
      } else {
         return std::string("CCD Imager Simulator");
      }
   }

};

TEST(IndigoClientTest, connectServerFailedWrongHost) {
   if (g_testMode == TestMode::local) {
      return;
   }
   const char* host = "ocalhost";
   int32_t port = 7624;
   std::unique_ptr<IndigoClient> indigoClient = TestExecution::createClient(host, port);
   std::ostringstream errMsg;
   ASSERT_TRUE(indigoClient.get()->connectServer(errMsg));
   sleep(1);
   ASSERT_FALSE(indigoClient.get()->serverIsConnected(errMsg));
   ASSERT_STREQ("Error: Name or service not known", errMsg.str().c_str());
}

TEST(IndigoClientTest, connectServerFailedDuplicateSocket) {
   if (g_testMode == TestMode::local) {
      return;
   }
   const char* host = "localhost";
   int32_t port = 7624;
   std::unique_ptr<IndigoClient> indigoClient = TestExecution::createClient(host, port);
   std::ostringstream errMsg;
   TestExecution::attachClient(indigoClient.get(), errMsg);
   ASSERT_FALSE(indigoClient.get()->connectServer(errMsg));
   TestExecution::detachClient(indigoClient.get());
   ASSERT_STREQ("Internal error: Server connection failed: rc = 6 -- ../../IndigoClient.cpp:86", errMsg.str().c_str());
}

TEST(IndigoClientTest, connectServerSuccess) {
   if (g_testMode == TestMode::local) {
      return;
   }
   const char* host = "localhost";
   int32_t port = 7624;
   std::unique_ptr<IndigoClient> indigoClient = TestExecution::createClient(host, port);
   std::ostringstream errMsg;
   TestExecution::attachClient(indigoClient.get(), errMsg);
   TestExecution::detachClient(indigoClient.get());
   ASSERT_FALSE(indigoClient.get()->serverIsConnected(errMsg));
}

TEST(IndigoClientTest, receiveAttachedDevices) {
   const char* host = "localhost";
   int32_t port = 7624;
   std::set<std::string> devices;
   std::unique_ptr<IndigoClient> indigoClient = TestExecution::createClient(host, port);
   std::ostringstream errMsg;
   indigoClient.get()->newDevice = [&devices] (const std::string& deviceName) {
      pthread_mutex_lock(&s_mutex);
      devices.insert(deviceName);
      pthread_mutex_unlock(&s_mutex);
   };
   indigoClient.get()->removeDevice = [&devices] (const std::string& deviceName) {
      pthread_mutex_lock(&s_mutex);
      devices.erase(deviceName);
      pthread_mutex_unlock(&s_mutex);
   };
   TestExecution::attachClient(indigoClient.get(), errMsg);
   ASSERT_EQ(6, devices.size());
   {
      auto iter = devices.find(g_testMode == TestMode::server ? std::string("CCD Imager Simulator @ localhost") : std::string("CCD Imager Simulator"));
      ASSERT_TRUE(iter != devices.end());
   }
   {
      auto iter = devices.find(g_testMode == TestMode::server ? std::string("CCD Imager Simulator (focuser) @ localhost") : std::string("CCD Imager Simulator (focuser)"));
      ASSERT_TRUE(iter != devices.end());
   }
   {
      auto iter = devices.find(g_testMode == TestMode::server ? std::string("CCD Imager Simulator (wheel) @ localhost") : std::string("CCD Imager Simulator (wheel)"));
      ASSERT_TRUE(iter != devices.end());
   }
   {
      auto iter = devices.find(g_testMode == TestMode::server ? std::string("CCD Guider Simulator @ localhost") : std::string("CCD Guider Simulator"));
      ASSERT_TRUE(iter != devices.end());
   }
   {
      auto iter = devices.find(g_testMode == TestMode::server ? std::string("CCD Guider Simulator (guider) @ localhost") : std::string("CCD Guider Simulator (guider)"));
      ASSERT_TRUE(iter != devices.end());
   }
   {
      auto iter = devices.find(g_testMode == TestMode::server ? std::string("DSLR Simulator @ localhost") : std::string("DSLR Simulator"));
      ASSERT_TRUE(iter != devices.end());
   }
   TestExecution::detachClient(indigoClient.get());
}

TEST(IndigoClientTest, receiveNewProperties) {
   const char* host = "localhost";
   int32_t port = 7624;
   std::map<std::string, indigo_property*> properties;
   typedef std::pair<std::string, indigo_property*> propertiesElementType;
   std::unique_ptr<IndigoClient> indigoClient = TestExecution::createClient(host, port);
   std::ostringstream errMsg;
   indigoClient.get()->newProperty = [&properties] (indigo_property* property) {
      std::string key = std::string("/") + std::string(property->device) + std::string("/") + std::string(property->name);
      pthread_mutex_lock(&s_mutex);
      properties.insert(propertiesElementType(key, property));
      pthread_mutex_unlock(&s_mutex);
   };
   indigoClient.get()->removeProperty = [&properties] (indigo_property* property) {
      std::string key = std::string("/") + std::string(property->device) + std::string("/") + std::string(property->name);
      pthread_mutex_lock(&s_mutex);
      properties.erase(key);
      pthread_mutex_unlock(&s_mutex);
   };
   TestExecution::attachClient(indigoClient.get(), errMsg);
   ASSERT_TRUE(properties.size() > 0);
   {
      auto iter = properties.find(TestExecution::getConnectionPropertyString());
      ASSERT_TRUE(iter != properties.end());
      ASSERT_EQ(2, iter->second->count);
      ASSERT_EQ(std::string("CONNECTED"), std::string(iter->second->items[0].name));
      ASSERT_FALSE(iter->second->items[0].sw.value);
   }
   {
      // connect to device
      ASSERT_TRUE(indigoClient.get()->connectDevice(g_testMode == TestMode::server ? "CCD Imager Simulator @ localhost" : "CCD Imager Simulator"));
      sleep(1);
      auto iter = properties.find(TestExecution::getConnectionPropertyString());
      ASSERT_TRUE(iter != properties.end());
      ASSERT_EQ(2, iter->second->count);
      ASSERT_EQ(std::string("CONNECTED"), std::string(iter->second->items[0].name));
      ASSERT_TRUE(iter->second->items[0].sw.value);
      ASSERT_TRUE(indigoClient.get()->disconnectDevice(g_testMode == TestMode::server ? "CCD Imager Simulator @ localhost" : "CCD Imager Simulator"));
   }
   TestExecution::detachClient(indigoClient.get());
}

TEST(IndigoClientTest, changeProperties) {
   const char* host = "localhost";
   int32_t port = 7624;
   std::map<std::string, indigo_property*> properties;
   typedef std::pair<std::string, indigo_property*> propertiesElementType;
   std::unique_ptr<IndigoClient> indigoClient = TestExecution::createClient(host, port);;
   std::ostringstream errMsg;
   indigoClient.get()->newProperty = [&properties] (indigo_property* property) {
      std::string key = std::string("/") + std::string(property->device) + std::string("/") + std::string(property->name);
      pthread_mutex_lock(&s_mutex);
      properties.insert(propertiesElementType(key, property));
      pthread_mutex_unlock(&s_mutex);
   };
   indigoClient.get()->removeProperty = [&properties] (indigo_property* property) {
      std::string key = std::string("/") + std::string(property->device) + std::string("/") + std::string(property->name);
      pthread_mutex_lock(&s_mutex);
      properties.erase(key);
      pthread_mutex_unlock(&s_mutex);
   };
   TestExecution::attachClient(indigoClient.get(), errMsg);
   size_t initialNumOfProperties = properties.size();
   ASSERT_TRUE(initialNumOfProperties > 0);
   {
      auto iter = properties.find(TestExecution::getConnectionPropertyString());
      ASSERT_TRUE(iter != properties.end());
      IProperty* property = PropertyFactory::Create(iter->second);
      std::unique_ptr<indigo_property> newProperty = property->clone();
      newProperty->items[0].sw.value = true;
      newProperty->items[1].sw.value = false;
      ASSERT_TRUE(indigoClient.get()->sendNewProperty(newProperty.get()));
      sleep(1);
      ASSERT_TRUE(properties.size() > initialNumOfProperties);
   }
   {
      auto iter = properties.find(TestExecution::getConnectionPropertyString());
      ASSERT_TRUE(iter != properties.end());
      IProperty* property = PropertyFactory::Create(iter->second);
      std::unique_ptr<indigo_property>  newProperty = property->clone();
      newProperty->items[0].sw.value = false;
      newProperty->items[1].sw.value = true;
      ASSERT_TRUE(indigoClient.get()->sendNewProperty(newProperty.get()));
      sleep(1);
      auto iter2 = properties.find(TestExecution::getCoolerPropertyString());
      ASSERT_TRUE(iter2 == properties.end());
      ASSERT_EQ(initialNumOfProperties, properties.size());
   }
   TestExecution::detachClient(indigoClient.get());
}

}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    g_testMode = TestMode::server;
    if (RUN_ALL_TESTS() != 0) {
       return 1; // error
    }
    g_testMode = TestMode::local;
    return RUN_ALL_TESTS();
}
