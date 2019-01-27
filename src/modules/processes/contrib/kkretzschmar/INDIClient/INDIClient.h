//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0232
// ----------------------------------------------------------------------------
// INDIClient.h - Released 2018-12-12T09:25:25Z
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

#ifndef __INDIClient_h
#define __INDIClient_h

#include "INDIParamListTypes.h"
#include "IIndigoProperty.h"
#include "IndigoClient.h"

#include <pcl/AutoLock.h>

#include<sstream>

namespace pcl
{

// ----------------------------------------------------------------------------

template <class T>
class ExclusiveAccess
{
public:

   typedef T   item_type;

   ExclusiveAccess( Mutex& mutex, item_type& item ) :
      m_lock( mutex ),
      m_item( item )
   {
   }

   operator item_type&()
   {
      return m_item;
   }

   operator const item_type&() const
   {
      return m_item;
   }

private:

   AutoLock   m_lock;
   item_type& m_item;
};

template <class T>
class ExclusiveConstAccess
{
public:

   typedef T   item_type;

   ExclusiveConstAccess( Mutex& mutex, const item_type& item ) :
      m_lock( mutex ),
      m_item( item )
   {
   }

   operator const item_type&() const
   {
      return m_item;
   }

private:

         AutoLock   m_lock;
   const item_type& m_item;
};

class ExclPropertyList : public ExclusiveAccess<INDIPropertyListItemArray>
{
public:

   ExclPropertyList( Mutex& mutex, INDIPropertyListItemArray& list ) :
      ExclusiveAccess( mutex, list )
   {
   }
};

class ExclConstPropertyList : public ExclusiveConstAccess<INDIPropertyListItemArray>
{
public:

   ExclConstPropertyList( Mutex& mutex, const INDIPropertyListItemArray& list ) :
      ExclusiveConstAccess( mutex, list )
   {
   }
};

class ExclDeviceList : public ExclusiveAccess<INDIDeviceListItemArray>
{
public:

   ExclDeviceList( Mutex& mutex, INDIDeviceListItemArray& list ) :
      ExclusiveAccess( mutex, list )
   {
   }
};

class ExclConstDeviceList : public ExclusiveConstAccess<INDIDeviceListItemArray>
{
public:

   ExclConstDeviceList( Mutex& mutex, const INDIDeviceListItemArray& list ) :
      ExclusiveConstAccess( mutex, list )
   {
   }
};

class INDIClient
{
public:

   INDIClient( const IsoString& hostName = "localhost", uint32 port = 7624 ): m_indigoClient("PixInsight", hostName.c_str(), port), m_serverHost(hostName), m_serverPort(port) {

      // register Indigo callbacks
      registerNewDeviceCallback();
      registerRemoveDeviceCallback();
      registerNewPropertyCallback();
      registerRemovePropertyCallback();
      registerNewSwitchCallback();
      registerNewNumberCallback();
      registerNewTextCallback();
      registerNewLightCallback();
      registerNewBlobCallback();
      registerGetMessageCallback();

   }

   virtual ~INDIClient()
   {
   }

   bool connectServer(std::ostream& errorMessage) {
      if (!m_indigoClient.connectServer(errorMessage)){
         return false;
      }
      return true;
   }

   bool disconnectServer() {
      if (IsServerConnected()) {
         return m_indigoClient.disconnectServer();
      }
      return true;
   }

   bool IsServerConnected() const
   {
      std::ostringstream errorMessage;
      return m_indigoClient.serverIsConnected(errorMessage);
   }

   bool IsServerConnected(std::ostream& errorMessage) const
   {
      return m_indigoClient.serverIsConnected(errorMessage);
   }

   bool connectDevice(const IsoString& deviceName)
   {
      return m_indigoClient.connectDevice(std::string(deviceName.c_str()));
   }

   bool disconnectDevice(const IsoString& deviceName)
   {
      return m_indigoClient.disconnectDevice(std::string(deviceName.c_str()));
   }

   bool IsDeviceConnected(const IsoString& deviceName) const;

   void setServer(const char *hostname, unsigned int port) {

   }

   IsoString HostName() const
   {
      return m_serverHost;
   }

   uint32 Port() const
   {
      return m_serverPort;
   }

   ExclDeviceList DeviceList()
   {
      return ExclDeviceList( m_deviceListMutex, m_deviceList );
   }

   ExclConstDeviceList ConstDeviceList() const
   {
      return ExclConstDeviceList( m_deviceListMutex, m_deviceList );
   }

   ExclConstDeviceList DeviceList() const
   {
      return ConstDeviceList();
   }

   bool HasDevices() const
   {
      ExclConstDeviceList x = DeviceList();
      return !static_cast<const INDIDeviceListItemArray&>( x ).IsEmpty();
   }

   ExclPropertyList PropertyList()
   {
      return ExclPropertyList( m_propertyListMutex, m_propertyList );
   }

   ExclConstPropertyList ConstPropertyList() const
   {
      return ExclConstPropertyList( m_propertyListMutex, m_propertyList );
   }

   ExclConstPropertyList PropertyList() const
   {
      return ConstPropertyList();
   }

   bool GetPropertyItem( const String& device, const String& property, const String& element,
                         INDIPropertyListItem& result,
                         bool formatted = true ) const;

   bool HasPropertyItem( const String& device, const String& property, const String& element ) const
   {
      INDIPropertyListItem dum;
      return GetPropertyItem( device, property, element, dum, false/*formatted*/ );
   }

   bool SendNewPropertyItem( const INDINewPropertyItem& item, bool async = false );

   template <typename T>
   bool SendNewPropertyItem( const String& device, const String& property, const String& type,
                             const String& element, const T& value, bool async = false )
   {
      return SendNewPropertyItem( INDINewPropertyItem( device, property, type, element, value ), async );
   }

   template <typename T>
   bool MaybeSendNewPropertyItem( const String& device, const String& property, const String& type,
                                  const String& element, const T& value, bool async = false )
   {
      return HasPropertyItem( device, property, element ) &&
             SendNewPropertyItem( device, property, type, element, value, async );
   }

   template <typename T1, typename T2>
   bool SendNewPropertyItem( const String& device, const String& property, const String& type,
                             const String& element1, const T1& value1,
                             const String& element2, const T2& value2, bool async = false )
   {
      return SendNewPropertyItem( INDINewPropertyItem( device, property, type, element1, value1, element2, value2 ), async );
   }

   template <typename T1, typename T2>
   bool MaybeSendNewPropertyItem( const String& device, const String& property, const String& type,
                                  const String& element1, const T1& value1,
                                  const String& element2, const T2& value2, bool async = false )
   {
      return HasPropertyItem( device, property, element1 ) &&
             HasPropertyItem( device, property, element2 ) &&
             SendNewPropertyItem( device, property, type, element1, value1, element2, value2, async );
   }

   String CurrentServerMessage() const
   {
      volatile AutoLock lock( m_mutex );
      return m_currentServerMessage;
   }

   void ClearCurrentServerMessage()
   {
      volatile AutoLock lock( m_mutex );
      m_currentServerMessage.Clear();
   }

   String DownloadedImagePath() const
   {
      volatile AutoLock lock( m_mutex );
      return m_downloadedImagePath;
   }

   void ClearDownloadedImagePath()
   {
      volatile AutoLock lock( m_mutex );
      m_downloadedImagePath.Clear();
   }

   bool HasDownloadedImage() const
   {
      volatile AutoLock lock( m_mutex );
      return !m_downloadedImagePath.IsEmpty();
   }

   int Verbosity() const
   {
      volatile AutoLock lock( m_mutex );
      return m_verbosity;
   }

   void SetVerbosity( int level )
   {
      volatile AutoLock lock( m_mutex );
      m_verbosity = Range( level, 0, 2 );
   }

   bool ServerConnectionHasChanged()
   {
      volatile AutoLock lock( m_mutex );
      bool hasChanged = m_serverConnectionChanged;
      m_serverConnectionChanged = false;
      return hasChanged;
   }

   bool ReportChangedDeviceLists( INDIDeviceListItemArray& created, INDIDeviceListItemArray& removed )
   {
      volatile AutoLock lock( m_mutex );
      created = m_createdDevices;
      m_createdDevices.Clear();
      removed = m_removedDevices;
      m_removedDevices.Clear();
      return !created.IsEmpty() || !removed.IsEmpty();
   }

   bool ReportChangedPropertyLists( INDIPropertyListItemArray& created, INDIPropertyListItemArray& removed, INDIPropertyListItemArray& updated )
   {
      volatile AutoLock lock( m_mutex );
      created = m_createdProperties;
      m_createdProperties.Clear();
      removed = m_removedProperties;
      m_removedProperties.Clear();
      updated = m_updatedProperties;
      m_updatedProperties.Clear();
      return !updated.IsEmpty() || !created.IsEmpty() || !removed.IsEmpty();
   }

   void RestartChangeReports();

   static bool HasClient()
   {
      return TheClient() != nullptr;
   }

   static INDIClient* TheClient();
   static INDIClient* TheClientOrDie();
   static INDIClient* NewClient( const IsoString& hostName = "localhost", uint32 port = 7624 );
   static void DestroyClient();




   void serverConnected();
   void serverDisconnected( int exit_code );

private:
   IndigoClient              m_indigoClient;

   IsoString                 m_serverHost;
   uint32_t                  m_serverPort;
   bool                      m_serverIsConnected;
   INDIDeviceListItemArray   m_deviceList;
   mutable Mutex             m_deviceListMutex;
   INDIPropertyListItemArray m_propertyList;
   mutable Mutex             m_propertyListMutex;
   String                    m_downloadedImagePath;
   String                    m_currentServerMessage;
   int                       m_verbosity = 1;
   bool                      m_serverConnectionChanged = false;
   INDIDeviceListItemArray   m_createdDevices;
   INDIDeviceListItemArray   m_removedDevices;
   INDIPropertyListItemArray m_createdProperties;
   INDIPropertyListItemArray m_removedProperties;
   INDIPropertyListItemArray m_updatedProperties;
   mutable Mutex             m_mutex;


   void registerNewDeviceCallback();
   void registerRemoveDeviceCallback();
   void registerNewPropertyCallback();
   void registerRemovePropertyCallback();
   void registerNewSwitchCallback();
   void registerNewNumberCallback();
   void registerNewTextCallback();
   void registerNewLightCallback();
   void registerNewBlobCallback();
   void registerGetMessageCallback();

   class PropertyListMutator
   {
   public:

      virtual void operator()( INDIClient*, INDIPropertyListItemArray&, const INDIPropertyListItem& ) const = 0;
   };

   class PropertyListGenerator : public PropertyListMutator
   {
   public:

      virtual void operator()( INDIClient* indi, INDIPropertyListItemArray& properties, const INDIPropertyListItem& item ) const
      {
         properties << item;

         {
            volatile AutoLock lock( indi->m_mutex );
            indi->m_createdProperties << item;
         }
      }
   };

   class PropertyListRemover : public PropertyListMutator
   {
   public:

      virtual void operator()( INDIClient* indi, INDIPropertyListItemArray& properties, const INDIPropertyListItem& item ) const
      {
         INDIPropertyListItemArray::iterator p = properties.Search( item );
         if ( p != properties.End() )
         {
            properties.Remove( p );

            {
               volatile AutoLock lock( indi->m_mutex );
               indi->m_removedProperties << item;
            }
         }
      }
   };

   class PropertyListUpdater : public PropertyListMutator
   {
   public:

      virtual void operator()( INDIClient* indi, INDIPropertyListItemArray& properties, const INDIPropertyListItem& item ) const
      {
         INDIPropertyListItemArray::iterator p = properties.Search( item );
         if ( p != properties.End() )
         {
            *p = item;

            {
               volatile AutoLock lock( indi->m_mutex );
               indi->m_updatedProperties << item;
            }
         }
      }
   };

   void ApplyToPropertyList( indigo_property*, const PropertyListMutator& );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIClient_h

// ----------------------------------------------------------------------------
// EOF INDIClient.h - Released 2018-12-12T09:25:25Z
