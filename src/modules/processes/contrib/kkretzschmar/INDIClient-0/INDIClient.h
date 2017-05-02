//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.10.0168
// ----------------------------------------------------------------------------
// INDIClient.h - Released 2016/05/18 10:06:42 UTC
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

#ifndef __INDIClient_h
#define __INDIClient_h

#include "IINDIProperty.h"
#include "INDIParamListTypes.h"

#include "INDI/BaseClientImpl.h"
#include "INDI/basedevice.h"
#include "INDI/indicom.h"
#include "INDI/indidevapi.h"

#include <pcl/AutoLock.h>

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

class INDIClient : public INDI::BaseClientImpl
{
public:

   INDIClient( const IsoString& hostName = "localhost", uint32 port = 7624 ) :
      BaseClientImpl( hostName.c_str(), port ),
      m_verbosity( 1 )
   {
   }

   virtual ~INDIClient()
   {
   }

   bool IsServerConnected() const
   {
      // Should be: INDI::BaseClientImpl::serverIsConnected() const
      return const_cast<INDIClient*>( this )->serverIsConnected();
   }

   IsoString HostName() const
   {
      // Should be: INDI::BaseClientImpl::getHost() const
      return const_cast<INDIClient*>( this )->getHost();
   }

   uint32 Port() const
   {
      // Should be: INDI::BaseClientImpl::getPort() const
      return uint32( const_cast<INDIClient*>( this )->getPort() );
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

   bool SendNewPropertyItem( const INDINewPropertyListItem& item, bool async = false );

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

protected:

   // Reimplemented from base class
   virtual void newDevice( INDI::BaseDevice* );
   virtual void deleteDevice( INDI::BaseDevice* );
   virtual void newProperty( INDI::Property* );
   virtual void removeProperty( INDI::Property* );
   virtual void newBLOB( IBLOB* );
   virtual void newSwitch( ISwitchVectorProperty* );
   virtual void newNumber( INumberVectorProperty* );
   virtual void newText( ITextVectorProperty* );
   virtual void newLight( ILightVectorProperty* );
   virtual void newMessage( INDI::BaseDevice*, int messageID );
   virtual void serverConnected();
   virtual void serverDisconnected( int exit_code );

private:

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

   void ApplyToPropertyList( INDI::Property*, const PropertyListMutator& );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIClient_h

// ----------------------------------------------------------------------------
// EOF INDIClient.h - Released 2016/05/18 10:06:42 UTC
