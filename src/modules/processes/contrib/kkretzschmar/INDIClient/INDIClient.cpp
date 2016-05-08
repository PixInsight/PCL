//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.09.0153
// ----------------------------------------------------------------------------
// INDIClient.cpp - Released 2016/05/08 20:36:42 UTC
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

#include "INDIClient.h"

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/File.h>
#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h>

#define CHECK_POINTER( p ) \
   if ( p == nullptr )     \
      throw Error( "INDIClient: Internal error: Null pointer argument in " + String( PCL_FUNCTION_NAME ) );

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
         if ( formatted && item.PropertyTypeStr == "INDI_NUMBER" )
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

bool INDIClient::SendNewPropertyItem( const INDINewPropertyListItem& newItem, bool async )
{
   if ( !IsServerConnected() )
      return false;

   int verbosity = Verbosity();
   Console console;

   try
   {
      if ( verbosity > 1 )
      {
         console.WriteLn( "<end><cbr><br>------------------------------------------------------------------------------" );
         console.WriteLn( "Sending new property element to INDI server '" + HostName() + ':' + IsoString( Port() ) + "':" );
      }

      if ( newItem.NewPropertyValue.IsEmpty() )
         throw String( "INDIClient: Internal error: Empty property value in " + String( PCL_FUNCTION_NAME ) );

      if ( newItem.Device.IsEmpty() || newItem.Property.IsEmpty() || newItem.Element.IsEmpty() || newItem.PropertyType.IsEmpty() )
         throw String( "INDIClient: Internal error: Invalid property data in " + String( PCL_FUNCTION_NAME ) );

      if ( verbosity > 1 )
      {
         console.WriteLn( "<end><cbr>"
                          "Device   : '" + newItem.Device + "'" );
         console.WriteLn( "Property : '" + newItem.Property + "'" );
         console.WriteLn( "Element  : '" + newItem.Element + "'" );
         console.WriteLn( "Value    : '" + newItem.NewPropertyValue + "'" );
      }

      INDI::BaseDevice* device = nullptr;
      {
         IsoString s( newItem.Device );
         device = getDevice( s.c_str() );
      }
      if ( device == nullptr )
         throw String( "Device '" + newItem.Device + "' not found." );

      if ( newItem.PropertyType == "INDI_NUMBER" )
      {
         INumberVectorProperty* numberVecProp;
         {
            IsoString s( newItem.Property );
            numberVecProp = device->getNumber( s.c_str() );
         }
         if ( numberVecProp == nullptr )
            throw String( "Could not get number property '" + newItem.Property + "' from server."
                          "Please check that INDI device '" + newItem.Device + "' is connected." );
         INumber* np;
         {
            IsoString s( newItem.Element );
            np = IUFindNumber( numberVecProp, s.c_str() );
         }
         if ( np == nullptr )
            throw String( "Could not find element '" + String( newItem.Element ) + "'." );
         np->value = newItem.NewPropertyValue.ToDouble();
         sendNewNumber( numberVecProp );
      }
      else if ( newItem.PropertyType == "INDI_TEXT" )
      {
         ITextVectorProperty* textVecProp;
         {
            IsoString s( newItem.Property );
            textVecProp = device->getText( s.c_str() );
         }
         if ( textVecProp == nullptr )
            throw String( "Could not get text property '" + newItem.Property + "' from server. "
                          "Please check that INDI device '" + newItem.Device + "' is connected." );
         IText* np;
         {
            IsoString s( newItem.Element );
            np = IUFindText( textVecProp, s.c_str() );
         }
         if ( np == nullptr )
            throw String( "Could not find element '" + String( newItem.Element ) + "'." );
         IsoString s( newItem.NewPropertyValue.ToUTF8() );
         IUSaveText( np, s.c_str() );
         sendNewText( textVecProp );
      }
      else if ( newItem.PropertyType == "INDI_SWITCH" )
      {
         ISwitchVectorProperty* switchVecProp;
         {
            IsoString s( newItem.Property );
            switchVecProp = device->getSwitch( s.c_str() );
         }
         if ( switchVecProp == nullptr )
            throw String( "Could not get switch property '" + newItem.Property + "' from server. "
                          "Please check that INDI device '" + newItem.Device + "' is connected." );
         ISwitch* sp;
         {
            IsoString s( newItem.Element );
            sp = IUFindSwitch( switchVecProp, s.c_str() );
         }
         if ( sp == nullptr )
            throw String( "Could not find element '" + String( newItem.Element ) + "'." );
         IUResetSwitch( switchVecProp );
         if ( newItem.NewPropertyValue == "ON" )
            sp->s = ISS_ON;
         else
            sp->s = ISS_OFF;
         sendNewSwitch( switchVecProp );
      }
      else
      {
         throw String( "Property '" + newItem.Property + "' not supported." );
      }

      // In asynchronous calls, wait until the server has processed all of our
      // property update requests.
      if ( !async )
         for ( ;; )
         {
            Module->ProcessEvents();
            if ( console.AbortRequested() )
               throw ProcessAborted();

            INDIPropertyListItem p;
            if ( GetPropertyItem( newItem.Device, newItem.Property, newItem.Element, p, false/*formatted*/ ) )
            {
               if ( p.PropertyState == IPS_OK || p.PropertyState == IPS_IDLE )
                  break;
               if ( p.PropertyState == IPS_ALERT )
                  throw String( "Failure to send '"
                           + newItem.Device + '.' + newItem.Property + '.' + newItem.Element
                           + "' property newItem. Message from INDI server: " + CurrentServerMessage() );
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

void INDIClient::newDevice( INDI::BaseDevice* d )
{
   CHECK_POINTER( d )

   ExclDeviceList x = DeviceList();
   INDIDeviceListItemArray& devices( x );

   for ( auto device : devices )
      if ( device.DeviceName == d->getDeviceName() )
         return;

   INDIDeviceListItem deviceListItem;
   deviceListItem.DeviceName = d->getDeviceName();
   deviceListItem.DeviceLabel = d->getDriverName();
   devices << deviceListItem;

   {
      volatile AutoLock lock( m_mutex );
      m_createdDevices << deviceListItem;
   }
}

void INDIClient::deleteDevice( INDI::BaseDevice* d )
{
   CHECK_POINTER( d )

   ExclDeviceList x = DeviceList();
   INDIDeviceListItemArray& devices( x );
   INDIDeviceListItemArray newDevices;

   ExclPropertyList y = PropertyList();
   INDIPropertyListItemArray& properties( y );
   INDIPropertyListItemArray newProperties;

   volatile AutoLock lock( m_mutex );

   for ( auto device : devices )
      if ( device.DeviceName == d->getDeviceName() )
         m_removedDevices << device;
      else
         newDevices << device;

   for ( auto property : properties )
      if ( property.Device == d->getDeviceName() )
         m_removedProperties << property;
      else
         newProperties << property;

   devices = newDevices;
   properties = newProperties;
}

void INDIClient::newProperty( INDI::Property* p )
{
   CHECK_POINTER( p )
   ApplyToPropertyList( p, PropertyListGenerator() );
   setBLOBMode( B_ALSO, p->getDeviceName() );
}

void INDIClient::removeProperty( INDI::Property* p )
{
   CHECK_POINTER( p )
   ApplyToPropertyList( p, PropertyListRemover() );
}

void INDIClient::newBLOB( IBLOB* b )
{
   CHECK_POINTER( b )
   String dir = PixInsightSettings::GlobalString( "ImageWindow/DownloadsDirectory" );
   if ( dir.IsEmpty() ) // this cannot happen
      dir = File::SystemTempDirectory();
   String filePath = dir + '/' + b->label + b->format;
   File myfile = File::CreateFileForWriting( filePath );
   myfile.Write( b->blob, b->size );
   myfile.Close();
   m_downloadedImagePath = filePath;
}

void INDIClient::newSwitch( ISwitchVectorProperty* svp )
{
   CHECK_POINTER( svp )
   INDI::Property p;
   p.setProperty( svp );
   p.setType( INDI_SWITCH );
   ApplyToPropertyList( &p, PropertyListUpdater() );
}

void INDIClient::newNumber( INumberVectorProperty* nvp )
{
   CHECK_POINTER( nvp )
   INDI::Property p;
   p.setProperty( nvp );
   p.setType( INDI_NUMBER );
   ApplyToPropertyList( &p, PropertyListUpdater() );
}

void INDIClient::newText( ITextVectorProperty* tvp )
{
   CHECK_POINTER( tvp )
   INDI::Property p;
   p.setProperty( tvp );
   p.setType( INDI_TEXT );
   ApplyToPropertyList( &p, PropertyListUpdater() );
}

void INDIClient::newLight( ILightVectorProperty* lvp )
{
   CHECK_POINTER( lvp )
   INDI::Property p;
   p.setProperty( lvp );
   p.setType( INDI_LIGHT );
   ApplyToPropertyList( &p, PropertyListUpdater() );
}

void INDIClient::newMessage( INDI::BaseDevice* d, int messageID )
{
   const char* message = d->messageQueue( messageID );
   if ( message != nullptr )
   {
      volatile AutoLock lock( m_mutex );
      m_currentServerMessage = message;
   }
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

void INDIClient::ApplyToPropertyList( INDI::Property* p, const PropertyListMutator& mutate )
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
      item.PropertyNumberFormat = ip->getNumberFormat( i );
      item.ElementLabel = ip->getElementLabel( i );

      mutate( this, properties, item );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIClient.cpp - Released 2016/05/08 20:36:42 UTC
