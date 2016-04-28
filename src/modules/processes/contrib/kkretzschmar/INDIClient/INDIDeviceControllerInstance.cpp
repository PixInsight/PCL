//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIDeviceControllerInstance.cpp - Released 2016/04/28 15:13:36 UTC
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
#include "INDIDeviceControllerInstance.h"
#include "INDIDeviceControllerParameters.h"
#include "PropertyNode.h"

#include "INDI/basedevice.h"
#include "INDI/indicom.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/Mutex.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

#ifdef __PCL_LINUX
#include <memory>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

INDIDeviceControllerInstance::INDIDeviceControllerInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_serverHostName( TheIDCServerHostNameParameter->DefaultValue() ),
   p_serverPort( uint32( TheIDCServerPortParameter->DefaultValue() ) ),
   p_serverConnect( TheIDCServerConnectParameter->DefaultValue() ),
   p_serverCommand(),
   p_abort( false ),
   p_verbosity( int32( TheIDCVerbosityParameter->DefaultValue() ) ),
   p_newProperties(),
   p_getCommandParameters(),
   o_devices(),
   o_properties(),
   o_getCommandResult(),
   m_currentMessage(),
   m_internalAbortFlag( false ),
   m_downloadedImagePath()
{
}

INDIDeviceControllerInstance::INDIDeviceControllerInstance( const INDIDeviceControllerInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void INDIDeviceControllerInstance::Assign( const ProcessImplementation& p )
{
   const INDIDeviceControllerInstance* x = dynamic_cast<const INDIDeviceControllerInstance*>( &p );
   if ( x != nullptr )
   {
      p_serverHostName = x->p_serverHostName;
      p_serverPort = x->p_serverPort;
      p_serverConnect = x->p_serverConnect;
      p_serverCommand = x->p_serverCommand;
      p_abort = x->p_abort;
      p_newProperties = x->p_newProperties;
      p_getCommandParameters = x->p_getCommandParameters;
      p_verbosity = x->p_verbosity;
      o_devices = x->o_devices;
      o_properties = x->o_properties;
      o_getCommandResult = x->o_getCommandResult;
   }
}

bool INDIDeviceControllerInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "INDIDeviceController can only be executed in the global context.";
   return false;
}

bool INDIDeviceControllerInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

bool INDIDeviceControllerInstance::getPropertyFromKeyString( INDINewPropertyListItem& propertyKey, const String& keyString )
{
   size_type start = 0;
   size_type nextStart = keyString.FindFirst( "/", start );
   size_type count = 0;

   while ( nextStart != String::notFound )
   {
      if ( count == 0 )
      {
         start = nextStart + 1;
         count++;
         nextStart = keyString.FindFirst( "/", start );
         continue;
      }

      if ( count == 1 )
         propertyKey.Device = keyString.Substring( start, nextStart-start );
      else if ( count == 2 )
         propertyKey.Property = keyString.Substring( start, nextStart-start );
      else if ( count == 3 )
         break;
      else
      {
         if ( p_verbosity > 0 )
            Console().CriticalLn( "<end><cbr>*** Error: Internal: Assertion in getPropertyFromKeyString(): Should not be here!" );
         return false;
      }

      count++;
      start = nextStart + 1;
      nextStart = keyString.FindFirst( "/", start );
   }

   if ( nextStart == String::notFound && count != 0 )
   {
      propertyKey.Element = keyString.Substring( start, keyString.Length()-start );
      return true;
   }

   return false;
}

bool INDIDeviceControllerInstance::sendNewPropertyVector( const NewPropertyListType& propVector, bool isAsynch )
{
   p_newProperties.Append( propVector );
   return sendNewProperty( isAsynch );
}

bool INDIDeviceControllerInstance::sendNewPropertyValue( const INDINewPropertyListItem& propItem, bool isAsynch )
{
   p_newProperties.Append( propItem );
   return sendNewProperty( isAsynch );
}

bool INDIDeviceControllerInstance::sendNewProperty( bool isAsynchCall )
{
   //Precondition: NewPropertyList contains only elements of the same property
   String deviceStr;
   String propertyStr;
   String propertyTypeStr;
   INDI::BaseDevice* device = nullptr;
   bool firstTime = true;
   INumberVectorProperty* numberVecProp = nullptr;
   ITextVectorProperty* textVecProp = nullptr;
   ISwitchVectorProperty* switchVecProp = nullptr;

   if ( p_newProperties.IsEmpty() )
      return false; // nothing to do

   try
   {
      if ( !INDIClient::HasClient() )
         throw String( "Internal: The INDI device controller has not been initialized." );

      if ( p_verbosity > 1 )
      {
         Console().WriteLn( "<end><cbr><br>------------------------------------------------------------------------------" );
         Console().WriteLn( "Sending new property value(s) to INDI server '" + String( p_serverHostName ) + ':' + String( p_serverPort ) + "':" );
      }

      Array<INDINewPropertyListItem> watchNewProperties;

      for ( Array<INDINewPropertyListItem>::iterator iter = p_newProperties.Begin(); iter != p_newProperties.End(); ++iter )
      {
         if ( iter->NewPropertyValue.IsEmpty() )
            throw String( "Internal: Empty property value in " + String( __func__ ) );

         // initialize
         if ( firstTime )
         {
            if (    getPropertyFromKeyString( *iter,iter->PropertyKey )
               || (!iter->Device.IsEmpty() && !iter->Property.IsEmpty() && !iter->PropertyType.IsEmpty()) )
            {
               deviceStr = iter->Device;
               propertyStr = iter->Property;
               propertyTypeStr = iter->PropertyType;
               firstTime = false;

               // get device
               {
                  IsoString s( deviceStr );
                  device = INDIClient::TheClient()->getDevice( s.c_str() );
               }
               if ( device == nullptr )
                  throw String( "Device '" + String( deviceStr ) + "' not found." );

               // get property vector
               if ( iter->PropertyType == "INDI_SWITCH" )
               {
                  {
                     IsoString s( propertyStr );
                     switchVecProp = device->getSwitch( s.c_str() );
                  }
                  if ( switchVecProp == nullptr )
                     throw String( "Could not get switch property '" + String( propertyStr ) + "' from server. "
                                   "Please check that INDI device '" + String( deviceStr ) + "' is connected." );
               }
               else if ( iter->PropertyType == "INDI_NUMBER" )
               {
                  {
                     IsoString s( propertyStr );
                     numberVecProp = device->getNumber( s.c_str() );
                  }
                  if ( numberVecProp == nullptr )
                     throw String( "Could not get number property '" + String( propertyStr ) + "' from server."
                                   "Please check that INDI device '" + String( deviceStr ) + "' is connected." );
               }
               else if ( iter->PropertyType == "INDI_TEXT" )
               {
                  {
                     IsoString s( propertyStr );
                     textVecProp = device->getText( s.c_str() );
                  }
                  if ( textVecProp == nullptr )
                        throw String( "Could not get text property '" + String( propertyStr ) + "' from server. "
                                      "Please check that INDI device '" + String( deviceStr ) + "' is connected." );
               }
               else
               {
                  throw String( "Property '" + String( propertyStr ) + "' is not supported." );
               }
            }
            else
            {
               throw String( "Invalid property key '" + String( iter->PropertyKey ) + "' not supported." );
            }
         }

         if (    getPropertyFromKeyString( *iter, iter->PropertyKey )
            || !iter->Device.IsEmpty() && !iter->Property.IsEmpty() && !iter->PropertyType.IsEmpty() )
         {
            if ( switchVecProp != nullptr )
            {
               // set new switch
               ISwitch* sp;
               {
                  IsoString s( iter->Element );
                  sp = IUFindSwitch( switchVecProp, s.c_str() );
               }
               if ( sp == nullptr )
                  throw String( "Could not find element '" + String( iter->Element ) + "'." );
               IUResetSwitch( switchVecProp );
               if ( iter->NewPropertyValue == "ON" )
                  sp->s = ISS_ON;
               else
                  sp->s = ISS_OFF;
            }
            else if ( numberVecProp != nullptr )
            {
               // set new number value
               INumber* np;
               {
                  IsoString s( iter->Element );
                  np = IUFindNumber( numberVecProp, s.c_str() );
               }
               if ( np == nullptr )
                  throw String( "Could not find element '" + String( iter->Element ) + "'." );
               np->value = iter->NewPropertyValue.ToDouble();
            }
            else if ( textVecProp != nullptr )
            {
               // set new text value
               IText* np;
               {
                  IsoString s( iter->Element );
                  np = IUFindText( textVecProp, s.c_str() );
               }
               if ( np == nullptr )
                  throw String( "Could not find element '" + String( iter->Element ) + "'." );
               IsoString s( iter->NewPropertyValue );
               IUSaveText( np, s.c_str() );
            }
            else
            {
               throw String( "Internal: Wrong property type in INDIDeviceControllerInstance::sendNewProperty()" );
            }
         }
         else
         {
            throw String( "Invalid property key '" + String( iter->PropertyKey ) + "'." );
         }

         if ( isAsynchCall )
            watchNewProperties << *iter;

         if ( p_verbosity > 1 )
         {
            Console().WriteLn( "<end><cbr>"
                               "Device   : '" + String( deviceStr ) + "'" );
            Console().WriteLn( "Property : '" + String( propertyStr ) + "'" );
            Console().WriteLn( "Element  : '" + String( iter->Element ) + "'" );
            Console().WriteLn( "Value    : '" + String( iter->NewPropertyValue ) + "'" );
         }
      } // for

      if ( p_verbosity > 1 )
         Console().WriteLn( "<end><cbr>------------------------------------------------------------------------------" );

      // Send new properties to server
      if ( switchVecProp != nullptr )
         INDIClient::TheClient()->sendNewSwitch( switchVecProp );
      else if ( numberVecProp != nullptr )
         INDIClient::TheClient()->sendNewNumber( numberVecProp );
      else if ( textVecProp != nullptr )
         INDIClient::TheClient()->sendNewText( textVecProp );

      p_newProperties.Clear();

      m_internalAbortFlag = false;

      // In asynchronous calls, wait until the server has processed all of our
      // new property requests.
      for ( auto item : watchNewProperties )
      {
         Module->ProcessEvents();
         INDIPropertyListItem p;
         if ( !getINDIPropertyItem( item.Device, item.Property, item.Element, p, false/*formatted*/ ) )
            throw String( "Unable to watch '"
                     + item.Device + '.' + item.Property + '.' + item.Element
                     + "' property item. Message from INDI server: " + String( m_currentMessage ) );
         if ( p_abort || m_internalAbortFlag )
            break;
         if ( p.PropertyState == IPS_OK || p.PropertyState == IPS_IDLE )
            break;
         if ( p.PropertyState == IPS_ALERT )
            throw String( "Failure to send '"
                     + item.Device + '.' + item.Property + '.' + item.Element
                     + "' property item. Message from INDI server: " + String( m_currentMessage ) );
      }

      return true;
   }
   catch ( const String& message )
   {
      if ( p_verbosity > 0 )
         Console().CriticalLn( "*** Error: " + message );
      return false;
   }
   catch ( ... )
   {
      throw;
   }
}

bool INDIDeviceControllerInstance::getINDIPropertyItem( String device, String property, String element, INDIPropertyListItem& result, bool formatted )
{
   // get popertyList with exclusive access
   ExclPropertyList propertyList = getExclusivePropertyList();

   for ( pcl::Array<INDIPropertyListItem>::iterator iter = propertyList.get()->Begin(); iter != propertyList.get()->End(); ++iter )
      if ( iter->Device == device && iter->Property == property && iter->Element == element )
      {
         result.Device = device;
         result.Property = property;
         result.Element = element;
         if ( formatted && iter->PropertyTypeStr == "INDI_NUMBER" )
         {
            result.PropertyValue = PropertyUtils::getFormattedNumber( iter->PropertyValue, IsoString( iter->PropertyNumberFormat ) );
            if ( result.PropertyValue.IsEmpty() ) // invalid property value?
               return false;
         }
         else
         {
            result.PropertyValue = iter->PropertyValue;
         }
         result.PropertyState = iter->PropertyState;
         return true;
      }

   return false;
}

bool INDIDeviceControllerInstance::ExecuteGlobal()
{
   Console console;

   if ( p_verbosity > 0 )
   {
      console.NoteLn( "<end><cbr>INDI Control Client --- (C) Klaus Kretzschmar, 2014-2016" );
      console.Flush();
   }

   o_getCommandResult.Clear();

   INDIClient* indiClient = INDIClient::TheClient();

   if ( !p_serverConnect )
   {
      // disconnect from server
      if ( indiClient == nullptr )
         throw Error( "The INDI device controller has not been initialized" );
      if ( indiClient->serverIsConnected() )
      {
         if ( p_verbosity > 0 )
            console.NoteLn( "* Disconnect from INDI server " + p_serverHostName + ", port=" + String( p_serverPort ) );
         indiClient->disconnectServer();
      }
      INDIClient::DestroyClient();
      return true;
   }

   if ( indiClient == nullptr )
      indiClient = INDIClient::NewClient( this, p_serverHostName.ToUTF8(), p_serverPort );

   if ( !indiClient->serverIsConnected() )
      indiClient->connectServer();

   if ( p_verbosity > 0 )
      if ( indiClient->serverIsConnected() )
         console.NoteLn( "* Successfully connected to INDI server " + p_serverHostName + ", port=" + String( p_serverPort ) );

   console.Flush();
   console.EnableAbort();

   if ( p_serverCommand == "GET" )
   {
      INDIPropertyListItem propItem;
      String device( PropertyUtils::getDevice( p_getCommandParameters ) );
      String property( PropertyUtils::getProperty( p_getCommandParameters ) );
      String element( PropertyUtils::getElement( p_getCommandParameters ) );
      if ( p_verbosity > 1 )
         console.WriteLn( "Device=" + String( device ) + ", Property=" + String( property ) + ", Element=" + String( element ) );
      if ( getINDIPropertyItem( device, property, element, propItem ) )
      {
         o_getCommandResult = propItem.PropertyValue;
         if ( p_verbosity > 1 )
            console.WriteLn( "Value=" + String( o_getCommandResult ) );
      }
      else
      {
         if ( p_verbosity > 0 )
            console.CriticalLn( "*** Error: Could not get value for property '" + String( p_getCommandParameters ) + "'." );
      }
   }
   else if ( p_serverCommand == "SET_ASYNCH" )
   {
      if ( !sendNewProperty( true/*isAsynchCall*/ ) )
         p_newProperties.Clear(); // cleanup new propertyList
   }
   else if ( p_serverCommand == "REGISTER_INSTANCE" )
   {
      indiClient->RegisterScriptInstance( this );
   }
   else if ( p_serverCommand == "RELEASE_INSTANCE" )
   {
      indiClient->RegisterScriptInstance( nullptr );
   }
   else
   {
      if ( !sendNewProperty() )
         p_newProperties.Clear(); // cleanup new propertyList
   }

   // disable abort to continue running
   p_abort = false;
   m_internalAbortFlag = false;

   return true;
}

void* INDIDeviceControllerInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIDCServerHostNameParameter )
      return p_serverHostName.Begin();
   if ( p == TheIDCServerPortParameter )
      return &p_serverPort;
   if ( p == TheIDCServerConnectParameter )
      return &p_serverConnect;
   if ( p == TheIDCNewPropertyValueParameter )
      return p_newProperties[tableRow].NewPropertyValue.Begin();
   if ( p == TheIDCNewPropertyKeyParameter )
      return p_newProperties[tableRow].PropertyKey.Begin();
   if ( p == TheIDCNewPropertyTypeParameter )
      return p_newProperties[tableRow].PropertyType.Begin();
   if ( p == TheIDCServerCommandParameter )
      return p_serverCommand.Begin();
   if ( p == TheIDCAbortParameter )
      return &p_abort;
   if ( p == TheIDCVerbosityParameter )
      return &p_verbosity;
   if ( p == TheIDCGetCommandParametersParameter )
      return p_getCommandParameters.Begin();

   if ( p == TheIDCDeviceNameParameter )
      return o_devices[tableRow].DeviceName.Begin();
   if ( p == TheIDCDeviceLabelParameter )
      return o_devices[tableRow].DeviceLabel.Begin();
   if ( p == TheIDCPropertyNameParameter )
      return o_properties[tableRow].PropertyKey.Begin();
   if ( p == TheIDCPropertyValueParameter )
      return o_properties[tableRow].PropertyValue.Begin();
   if ( p == TheIDCPropertyStateParameter )
      return &o_properties[tableRow].PropertyState;
   if ( p == TheIDCPropertyTypeParameter )
      return o_properties[tableRow].PropertyTypeStr.Begin();
   if ( p == TheIDCGetCommandResultParameter )
      return o_getCommandResult.Begin();

   return nullptr;
}

bool INDIDeviceControllerInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIDCServerHostNameParameter )
   {
      p_serverHostName.Clear();
      if ( sizeOrLength > 0 )
         p_serverHostName.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCNewPropertiesParameter )
   {
      p_newProperties.Clear();
      if ( sizeOrLength > 0 )
         p_newProperties.Add( INDINewPropertyListItem(), sizeOrLength );
   }
   else if ( p == TheIDCNewPropertyValueParameter )
   {
      p_newProperties[tableRow].NewPropertyValue.Clear();
      if ( sizeOrLength > 0 )
         p_newProperties[tableRow].NewPropertyValue.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCNewPropertyKeyParameter )
   {
      p_newProperties[tableRow].PropertyKey.Clear();
      if ( sizeOrLength > 0 )
         p_newProperties[tableRow].PropertyKey.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCNewPropertyTypeParameter )
   {
      p_newProperties[tableRow].PropertyType.Clear();
      if ( sizeOrLength > 0 )
         p_newProperties[tableRow].PropertyType.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCServerCommandParameter )
   {
      p_serverCommand.Clear();
      if ( sizeOrLength > 0 )
         p_serverCommand.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCGetCommandParametersParameter )
   {
      p_getCommandParameters.Clear();
      if ( sizeOrLength > 0 )
         p_getCommandParameters.SetLength( sizeOrLength );
   }

   else if ( p == TheIDCDevicesParameter )
   {
      o_devices.Clear();
      if ( sizeOrLength > 0 )
         o_devices.Add( INDIDeviceListItem(), sizeOrLength );
   }
   else if ( p == TheIDCDeviceNameParameter )
   {
      o_devices[tableRow].DeviceName.Clear();
      if ( sizeOrLength > 0 )
         o_devices[tableRow].DeviceName.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCDeviceLabelParameter )
   {
      o_devices[tableRow].DeviceLabel.Clear();
      if ( sizeOrLength > 0 )
         o_devices[tableRow].DeviceLabel.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCPropertiesParameter )
   {
      o_properties.Clear();
      if ( sizeOrLength > 0 )
         o_properties.Add( INDIPropertyListItem(), sizeOrLength );
   }
   else if ( p == TheIDCPropertyNameParameter )
   {
      o_properties[tableRow].PropertyKey.Clear();
      if ( sizeOrLength > 0 )
         o_properties[tableRow].PropertyKey.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCPropertyValueParameter )
   {
      o_properties[tableRow].PropertyValue.Clear();
      if ( sizeOrLength > 0 )
         o_properties[tableRow].PropertyValue.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCPropertyTypeParameter )
   {
      o_properties[tableRow].PropertyTypeStr.Clear();
      if ( sizeOrLength > 0 )
         o_properties[tableRow].PropertyTypeStr.SetLength( sizeOrLength );
   }
   else if ( p == TheIDCGetCommandResultParameter )
   {
      o_getCommandResult.Clear();
      if ( sizeOrLength > 0 )
         o_getCommandResult.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type INDIDeviceControllerInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheIDCServerHostNameParameter )
      return p_serverHostName.Length();
   if ( p == TheIDCNewPropertiesParameter )
      return p_newProperties.Length();
   if ( p == TheIDCNewPropertyValueParameter )
      return p_newProperties[tableRow].NewPropertyValue.Length();
   if ( p == TheIDCNewPropertyKeyParameter )
      return p_newProperties[tableRow].PropertyKey.Length();
   if ( p == TheIDCNewPropertyTypeParameter )
      return p_newProperties[tableRow].PropertyType.Length();
   if ( p == TheIDCServerCommandParameter )
      return p_serverCommand.Length();
   if ( p == TheIDCGetCommandParametersParameter )
      return p_getCommandParameters.Length();

   if ( p == TheIDCDevicesParameter )
      return o_devices.Length();
   if ( p == TheIDCDeviceNameParameter )
      return o_devices[tableRow].DeviceName.Length();
   if ( p == TheIDCDeviceLabelParameter )
      return o_devices[tableRow].DeviceLabel.Length();
   if ( p == TheIDCPropertiesParameter )
      return o_properties.Length();
   if ( p == TheIDCPropertyNameParameter )
      return o_properties[tableRow].PropertyKey.Length();
   if ( p == TheIDCPropertyValueParameter )
      return o_properties[tableRow].PropertyValue.Length();
   if ( p == TheIDCPropertyTypeParameter )
      return o_properties[tableRow].PropertyTypeStr.Length();
   if ( p == TheIDCGetCommandResultParameter )
      return o_getCommandResult.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInstance.cpp - Released 2016/04/28 15:13:36 UTC
