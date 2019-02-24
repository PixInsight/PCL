//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0238
// ----------------------------------------------------------------------------
// INDIClient.cpp - Released 2019-01-21T12:06:42Z
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

#include "INDIClient.h"
#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/File.h>
#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h>

#include<sstream>

#define CHECK_POINTER( p ) \
   if ( p == nullptr )     \
      throw Error( "Internal error: INDIClient: Null pointer argument in " + String( PCL_FUNCTION_NAME ) );

namespace pcl
{

// ----------------------------------------------------------------------------

static INDIClient* s_indiClient = nullptr;

INDIClient* INDIClient::TheClient()
{
   return s_indiClient;
}

INDIClient* INDIClient::TheClientOrDie()
{
   if ( s_indiClient == nullptr )
      throw Error( "The INDI device controller has not been initialized" );
   return s_indiClient;
}

INDIClient* INDIClient::NewClient( const IsoString& hostname, unsigned port )
{
   return s_indiClient = new INDIClient( hostname, port );
}

void INDIClient::DestroyClient()
{
   if ( s_indiClient != nullptr )
      delete s_indiClient, s_indiClient = nullptr;
}

// ----------------------------------------------------------------------------

bool INDIClient::GetPropertyItem( const String& device, const String& property, const String& element,
                                  INDIPropertyListItem& result, bool formatted ) const
{
   ExclConstPropertyList y = PropertyList();
   const INDIPropertyListItemArray& properties( y );

   for ( auto item : properties )
      if ( item.Device == device && item.Property == property && item.Element == element )
      {
         result.Device = device;
         result.Property = property;
         result.Element = element;
         if ( formatted && item.PropertyType == INDIGO_NUMBER_VECTOR )
            result.PropertyValue = PropertyUtils::FormattedNumber( item.PropertyValue, IsoString( item.PropertyNumberFormat ) );
         else
            result.PropertyValue = item.PropertyValue;
         if ( result.PropertyValue.IsEmpty() ) // invalid property value?
            return false;
         result.PropertyState = item.PropertyState;
         return true;
      }

   return false;
}

bool INDIClient::GetPropertyTargetItem( const String& device, const String& property, const String& element,
                                        INDIPropertyListItem& result, bool formatted ) const
{
   ExclConstPropertyList y = PropertyList();
   const INDIPropertyListItemArray& properties( y );

   for ( auto item : properties )
      if ( item.Device == device && item.Property == property && item.Element == element )
      {
         result.Device = device;
         result.Property = property;
         result.Element = element;
         if ( formatted && item.PropertyType == INDIGO_NUMBER_VECTOR ) {
            result.PropertyTarget = PropertyUtils::FormattedNumber( item.PropertyTarget, IsoString( item.PropertyNumberFormat ) );
            result.PropertyValue = PropertyUtils::FormattedNumber( item.PropertyValue, IsoString( item.PropertyNumberFormat ) );
         }
         else {
            result.PropertyValue = item.PropertyValue;
            result.PropertyTarget = item.PropertyTarget;
         }
         if ( result.PropertyValue.IsEmpty() || result.PropertyTarget.IsEmpty() ) // invalid property value?
            return false;
         result.PropertyState = item.PropertyState;
         return true;
      }

   return false;
}

bool INDIClient::SendNewPropertyItem( const INDINewPropertyItem& newItem, bool async )
{

   int verbosity = Verbosity();
   Console console;

   std::ostringstream errMessage;
   bool success = IsServerConnected(errMessage);
   if ( !success ){
      console.CriticalLn("<end><cbr><br>------------------------------------------------------------------------------" );
      console.CriticalLn("Server is not connected. Possible reason:" + IsoString(errMessage.str().c_str()));
      return false;
   }

   try
   {
      if ( verbosity > 1 )
      {
         console.WriteLn( "<end><cbr><br>------------------------------------------------------------------------------" );
         console.WriteLn( "Sending new property element to INDI server '" + HostName() + ':' + IsoString( Port() ) + "':" );
      }

      if ( newItem.Device.IsEmpty() || newItem.Property.IsEmpty() || newItem.ElementValues.IsEmpty() || newItem.PropertyType.IsEmpty() )
         throw String( "Internal error: INDIClient: Invalid property data in " + String( PCL_FUNCTION_NAME ) );

      if ( verbosity > 1 )
      {
         console.WriteLn( "<end><cbr>"
                        "Device   : '" + newItem.Device + "'" );
         console.WriteLn( "Property : '" + newItem.Property + "'" );
         console.Flush();
      }

      if ( newItem.Device.IsEmpty())
         throw String( "Device '" + newItem.Device + "' not found." );


      if ( newItem.PropertyType == "INDI_NUMBER" )
      {
         size_t numOfItems = newItem.ElementValues.Length();
         char** items = reinterpret_cast<char**>(malloc(numOfItems * sizeof(char*)));
         if (items == nullptr) {
            throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
         }
         double* values = reinterpret_cast<double*>(malloc(numOfItems * sizeof(double)));
         if (values == nullptr) {
            throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
         }

         {
            size_t count = 0;
            for ( auto element : newItem.ElementValues ) {
               items[count] = reinterpret_cast<char*>(malloc((element.Element.To7BitASCII().Length() + 1) * sizeof(char)));
               if (items[count] == nullptr) {
                  throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
               }
               strncpy(items[count], element.Element.To7BitASCII().c_str(), element.Element.To7BitASCII().Length() + 1);
               values[count] = element.Value.ToDouble();
               count++;
            }
            m_indigoClient.sendNewNumberProperty(const_cast<char*>(newItem.Device.To7BitASCII().c_str()), newItem.Property.To7BitASCII().c_str(), numOfItems, const_cast<const char**>(items), values);
         }

         for ( size_t count = 0; count < newItem.ElementValues.Length(); count++ ) {
            free(items[count]);
         }
         free(items);
         free(values);
      } else if (newItem.PropertyType == "INDI_SWITCH") {
         size_t numOfItems = newItem.ElementValues.Length();
         char** items = reinterpret_cast<char**>(malloc(numOfItems * sizeof(char*)));
         if (items == nullptr) {
            throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
         }
         bool* values = reinterpret_cast<bool*>(malloc(numOfItems * sizeof(bool)));
         if (values == nullptr) {
            throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
         }

         {
            size_t count = 0;
            for ( auto element : newItem.ElementValues ) {
               items[count] = reinterpret_cast<char*>(malloc((element.Element.To7BitASCII().Length() + 1) * sizeof(char)));
               if (items[count] == nullptr) {
                  throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
               }
               strncpy(items[count], element.Element.To7BitASCII().c_str(), element.Element.To7BitASCII().Length() + 1);
               values[count] = element.Value == "ON" ? true : false;
               count++;
            }
            m_indigoClient.sendNewSwitchProperty(const_cast<char*>(newItem.Device.To7BitASCII().c_str()), newItem.Property.To7BitASCII().c_str(), numOfItems, const_cast<const char**>(items), values);
         }

         for ( size_t count = 0; count < newItem.ElementValues.Length(); count++ ) {
            free(items[count]);
         }
         free(items);
         free(values);
      } else if (newItem.PropertyType == "INDI_TEXT") {
         size_t numOfItems = newItem.ElementValues.Length();
         char** items = reinterpret_cast<char**>(malloc(numOfItems * sizeof(char*)));
         if (items == nullptr) {
            throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
         }
         char** values = reinterpret_cast<char**>(malloc(numOfItems * sizeof(char*)));
         if (values == nullptr) {
            throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
         }

         {
            size_t count = 0;
            for ( auto element : newItem.ElementValues ) {
               items[count] = reinterpret_cast<char*>(malloc((element.Element.To7BitASCII().Length() + 1) * sizeof(char)));
               if (items[count] == nullptr) {
                  throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
               }
               strncpy(items[count], element.Element.To7BitASCII().c_str(), element.Element.To7BitASCII().Length() + 1);
               values[count] = reinterpret_cast<char*>(malloc((element.Value.To7BitASCII().Length() + 1) * sizeof(char)));
               if (values[count] == nullptr) {
                  throw String( "Internal error: out of memory in " + String( PCL_FUNCTION_NAME ) );
               }
               strncpy(values[count], element.Value.To7BitASCII().c_str(), element.Value.To7BitASCII().Length() + 1);
               count++;
            }
            m_indigoClient.sendNewTextProperty(const_cast<char*>(newItem.Device.To7BitASCII().c_str()), newItem.Property.To7BitASCII().c_str(), numOfItems, const_cast<const char**>(items), const_cast<const char**>(values));
         }

         for ( size_t count = 0; count < newItem.ElementValues.Length(); count++ ) {
            free(items[count]);
            free(values[count]);
         }
         free(items);
         free(values);
      }


      // In synchronous calls, wait until the server has processed all of our
      // property update requests.
      if ( !async ) {
         for ( ;; ) {
            Module->ProcessEvents();
            if ( console.AbortRequested() )
               throw ProcessAborted();

            size_type requestsDone = 0;
            for ( auto elementValue : newItem.ElementValues )
            {
               INDIPropertyListItem p;
               if ( GetPropertyItem( newItem.Device, newItem.Property, elementValue.Element, p, false/*formatted*/ ) )
                 switch ( p.PropertyState )
                  {
                  case INDIGO_OK_STATE:
                  case INDIGO_IDLE_STATE:
                     ++requestsDone;
                     break;
                  case INDIGO_ALERT_STATE:
                     throw String( "Failure to send '"
                           + newItem.Device + '.' + newItem.Property + '.' + elementValue.Element
                           + "' property newItem. Message from INDI server: " + CurrentServerMessage() );
                  default:
                     break;
                  }
            }

            if ( requestsDone == newItem.ElementValues.Length() )
               break;
         }
      }

      return true;

   }
   catch ( const String& message )
   {
      if ( verbosity > 0 )
         console.CriticalLn( "<end><cbr>*** Error: " + message );
      return false;
   }
   catch ( ... )
   {
      throw;
   }
}

void INDIClient::RestartChangeReports()
{
   volatile AutoLock lock( m_mutex );

   ExclConstDeviceList x = ConstDeviceList();
   const INDIDeviceListItemArray& devices( x );

   ExclConstPropertyList y = ConstPropertyList();
   const INDIPropertyListItemArray& properties( y );

   m_createdDevices = devices;
   m_removedDevices.Clear();

   m_createdProperties = properties;
   m_removedProperties.Clear();
   m_updatedProperties.Clear();
}

// ----------------------------------------------------------------------------


void INDIClient::registerNewDeviceCallback() {
   m_indigoClient.newDevice = [this] (const std::string& deviceName) {
      ExclDeviceList x = DeviceList();
      INDIDeviceListItemArray& devices( x );

      for ( auto device : devices )
         if ( device.DeviceName == deviceName.c_str() )
            return;

      INDIDeviceListItem deviceListItem;
      deviceListItem.DeviceName = deviceName.c_str();
      deviceListItem.DeviceLabel =  deviceName.c_str();
      devices << deviceListItem;

      {
         volatile AutoLock lock( m_mutex );
         m_createdDevices << deviceListItem;
      }
   };
}

void INDIClient::registerRemoveDeviceCallback() {
   m_indigoClient.removeDevice = [this] (const std::string& deviceName) {
      ExclDeviceList x = DeviceList();
      INDIDeviceListItemArray& devices( x );
      INDIDeviceListItemArray newDevices;

      ExclPropertyList y = PropertyList();
      INDIPropertyListItemArray& properties( y );
      INDIPropertyListItemArray newProperties;

      volatile AutoLock lock( m_mutex );

      for ( auto device : devices )
         if ( device.DeviceName == deviceName.c_str() )
            m_removedDevices << device;
         else
            newDevices << device;

      for ( auto property : properties )
         if ( property.Device == deviceName.c_str() )
            m_removedProperties << property;
         else
            newProperties << property;

      devices = newDevices;
      properties = newProperties;
   };
}


void INDIClient::registerNewPropertyCallback() {
   m_indigoClient.newProperty = [this] (indigo_property* property) {
      CHECK_POINTER( property );
      ApplyToPropertyList( property, PropertyListGenerator() );
   };
}

void INDIClient::registerRemovePropertyCallback() {
   m_indigoClient.removeProperty = [this] (indigo_property* property) {
      CHECK_POINTER( property );
      ApplyToPropertyList( property, PropertyListRemover() );
   };
}

void INDIClient::registerNewSwitchCallback() {
   m_indigoClient.newSwitch = [this] (indigo_property* property) {
      CHECK_POINTER( property );
      ApplyToPropertyList( property, PropertyListUpdater() );
   };
}

void INDIClient::registerNewNumberCallback() {
   m_indigoClient.newNumber = [this] (indigo_property* property) {
      CHECK_POINTER( property );
      ApplyToPropertyList( property, PropertyListUpdater() );
   };
}

void INDIClient::registerNewTextCallback() {
   m_indigoClient.newText = [this] (indigo_property* property) {
      CHECK_POINTER( property );
      ApplyToPropertyList( property, PropertyListUpdater() );
   };
}

void INDIClient::registerNewLightCallback() {
   m_indigoClient.newLight = [this] (indigo_property* property) {
      CHECK_POINTER( property );
      ApplyToPropertyList( property, PropertyListUpdater() );
   };
}

void INDIClient::registerNewBlobCallback() {
   m_indigoClient.newBlob = [this] (indigo_property* property) {
      CHECK_POINTER( property );
      AutoPointer<BlobProperty> blobProperty ( dynamic_cast<BlobProperty*>(PropertyFactory::Create( property )) );
      String dir = PixInsightSettings::GlobalString( "ImageWindow/DownloadsDirectory" );
      if ( dir.IsEmpty() ) // this cannot happen
         dir = File::SystemTempDirectory();
      String filePath = dir + '/' + blobProperty->getElementLabel(0) + blobProperty->getBlobFormat(0);
      File myfile = File::CreateFileForWriting( filePath );
      myfile.Write( blobProperty->getBlob(0), blobProperty->getBlobSize(0) );
      myfile.Close();
      m_downloadedImagePath = filePath;
   };
}

void INDIClient::registerGetMessageCallback() {
   m_indigoClient.newMessage = [this] (const char* message) {
      CHECK_POINTER( message );
      volatile AutoLock lock( m_mutex );
      m_currentServerMessage = message;

   };
}

void INDIClient::serverConnected()
{
   volatile AutoLock lock( m_mutex );
   m_serverConnectionChanged = true;
}

void INDIClient::serverDisconnected( int exitCode )
{
   volatile AutoLock lock( m_mutex );
   ExclDeviceList x = DeviceList();
   ExclPropertyList y = PropertyList();
   INDIDeviceListItemArray( x ).Clear();
   INDIPropertyListItemArray( y ).Clear();
   m_serverConnectionChanged = true;
}

bool INDIClient::IsDeviceConnected(const IsoString& deviceName) const {
   INDIPropertyListItem propItem;
   if (GetPropertyItem(deviceName, String("CONNECTION"), String("CONNECTED"), propItem, false)) {
      return propItem.PropertyValue == String("ON");
   }
   return false;
}


void INDIClient::ApplyToPropertyList( indigo_property* p, const PropertyListMutator& mutate )
{
   ExclPropertyList y = PropertyList();
   INDIPropertyListItemArray& properties( y );

   AutoPointer<IProperty> ip( PropertyFactory::Create( p ) );

   INDIPropertyListItem item;
   item.Device = ip->getDeviceName();
   item.Property = ip->getName();
   item.PropertyType = ip->getType();
   item.PropertyTypeStr = ip->getTypeStr();
   item.PropertyState = ip->getState();
   item.PropertyLabel = ip->getLabel();

   for ( size_t i = 0; i < ip->getNumOfElements(); i++ )
   {
      item.Element = ip->getElementName( i );
      item.PropertyKey = '/' + item.Device
                       + '/' + item.Property
                       + '/' + item.Element;
      item.PropertyValue = ip->getElementValue( i );
      item.PropertyTarget = ip->getElementTarget( i );
      item.PropertyNumberFormat = ip->getNumberFormat( i );
      item.ElementLabel = ip->getElementLabel( i );

      mutate( this, properties, item );
   }
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIClient.cpp - Released 2019-01-21T12:06:42Z
