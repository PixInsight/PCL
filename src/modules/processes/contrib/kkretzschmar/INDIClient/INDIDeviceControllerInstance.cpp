//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.04.0108
// ----------------------------------------------------------------------------
// INDIDeviceControllerInstance.cpp - Released 2016/04/15 15:37:39 UTC
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
#include <pcl/Mutex.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

#ifdef __PCL_LINUX
#include <memory>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/* Our client auto pointer */
AutoPointer<INDIClient> indiClient;

// ----------------------------------------------------------------------------

INDIDeviceControllerInstance::INDIDeviceControllerInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_serverHostName( TheIDCServerHostNameParameter->DefaultValue() ),
   p_serverPort( uint32( TheIDCServerPortParameter->DefaultValue() ) ),
   p_serverConnect( TheIDCServerConnectParameter->DefaultValue() ),
   p_serverCommand(),
   p_abort( false ),
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
         Console().CriticalLn( "<end><cbr>** Assertion in getPropertyFromKeyString: Should not be here" );

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
      return false; //Nothing to do

   if ( indiClient.IsNull() ){
	   Console().CriticalLn("*** Error: Internal: The INDI device controller has not been initialized");
	   return false;
   }

   Console().WriteLn( "<end><cbr><br>------------------------------------------------------------------------------" );
   Console().WriteLn( "Sending new property value(s) to INDI server '" + String( p_serverHostName ) + ':' + String( p_serverPort ) + "':" );

   for ( pcl::Array<INDINewPropertyListItem>::iterator iter = p_newProperties.Begin(); iter != p_newProperties.End(); ++iter )
   {
      if ( iter->NewPropertyValue.IsEmpty() )
      {
         Console().CriticalLn( "*** Error: Internal: Empty property value in " + String( __func__ ) );
         return false;
      }

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
               device = indiClient->getDevice( s.c_str() );
            }
            if ( !device )
            {
               Console().CriticalLn( "*** Error: Device '" + String( deviceStr ) + "' not found." );
               return false;
            }

            // get property vector
            if ( iter->PropertyType == "INDI_SWITCH" )
            {
               {
                  IsoString s( propertyStr );
                  switchVecProp = device->getSwitch( s.c_str() );
               }
               if ( !switchVecProp )
               {
                  Console().CriticalLn( "*** Error: Could not get property '" + String( propertyStr ) + "' from server. "
                                 "Please check that INDI device '" + String( deviceStr ) + "' is connected." );
                  return false;
               }
            }
            else if ( iter->PropertyType == "INDI_NUMBER" )
            {
               {
                  IsoString s( propertyStr );
                  numberVecProp = device->getNumber( s.c_str() );
               }
               if ( !numberVecProp )
               {
                  Console().CriticalLn( "*** Error: Could not get property '" + String( propertyStr ) + "' from server."
                                 "Please check that INDI device '" + String( deviceStr ) + "' is connected." );
                  return false;
               }
            }
            else if ( iter->PropertyType == "INDI_TEXT" )
            {
               {
                  IsoString s( propertyStr );
                  textVecProp = device->getText( s.c_str() );
               }
               if ( !textVecProp )
               {
                  Console().CriticalLn( "*** Error: Could not get property '" + String( propertyStr ) + "' from server. "
                                 "Please check that INDI device '" + String( deviceStr ) + "' is connected." );
                  return false;
               }
            }
            else
            {
               Console().CriticalLn( "*** Error: Property '" + String( propertyStr ) + "' not supported." );
            }
         }
         else
         {
            Console().CriticalLn( "*** Error: Invalid property key '" + String( iter->PropertyKey ) + "' not supported." );
         }
      }

      if (    getPropertyFromKeyString( *iter, iter->PropertyKey )
           || !iter->Device.IsEmpty() && !iter->Property.IsEmpty() && !iter->PropertyType.IsEmpty() )
      {
         if ( switchVecProp )
         {
            // set new switch
            ISwitch* sp;
            {
               IsoString s( iter->Element );
               sp = IUFindSwitch( switchVecProp, s.c_str() );
            }
            if ( !sp )
            {
               Console().CriticalLn( "*** Error: Could not find element '" + String( iter->Element ) + "'." );
               return false;
            }
            IUResetSwitch( switchVecProp );
            if ( iter->NewPropertyValue == "ON" )
               sp->s = ISS_ON;
            else
               sp->s = ISS_OFF;
         }
         else if ( numberVecProp )
         {
            // set new number value
            INumber* np;
            {
               IsoString s( iter->Element );
               np = IUFindNumber( numberVecProp, s.c_str() );
            }
            if ( !np )
            {
               Console().CriticalLn( "*** Error: Could not find element '" + String( iter->Element ) + "'." );
               return false;
            }
            np->value = iter->NewPropertyValue.ToDouble();
         }
         else if ( textVecProp )
         {
            // set new text value
            IText* np;
            {
               IsoString s( iter->Element );
               np = IUFindText( textVecProp, s.c_str() );
            }
            if ( !np )
            {
               Console().CriticalLn( "*** Error: Could not find element '" + String( iter->Element ) + "'." );
               return false;
            }
            {
               IsoString s( iter->NewPropertyValue );
               IUSaveText( np, s.c_str() );
            }
         }
         else
         {
            Console().CriticalLn( "*** Error: Internal: Wrong property type in INDIDeviceControllerInstance::sendNewProperty()" );
            return false;
         }
      }
      else
      {
         Console().CriticalLn( "*** Error: Invalid property key '" + String( iter->PropertyKey ) + "'." );
         return false;
      }

      Console().WriteLn( "<end><cbr>"
                         "Device  : '" + String( deviceStr ) + "'" );
      Console().WriteLn( "Property: '" + String( propertyStr ) + "'" );
      Console().WriteLn( "Element : '" + String( iter->Element ) + "'" );
      Console().WriteLn( "Value   : '" + String( iter->NewPropertyValue ) + "'" );

   } // for

   Console().WriteLn( "<end><cbr>------------------------------------------------------------------------------" );

   // send new properties to server and wait for response
   if ( switchVecProp )
   {
      indiClient->sendNewSwitch( switchVecProp );
      // if synch mode wait until completed or abort
      while ( switchVecProp->s != IPS_OK && switchVecProp->s != IPS_IDLE && !p_abort && !m_internalAbortFlag && !isAsynchCall )
         if ( switchVecProp->s == IPS_ALERT )
         {
            writeCurrentMessageToConsole();
            m_internalAbortFlag = false;
            return false;
         }
   }
   else if ( numberVecProp )
   {
      indiClient->sendNewNumber( numberVecProp );
      // if synch mode wait until completed or abort
      while ( numberVecProp->s != IPS_OK && numberVecProp->s != IPS_IDLE && !p_abort && !m_internalAbortFlag && !isAsynchCall )
         if ( numberVecProp->s == IPS_ALERT )
         {
            writeCurrentMessageToConsole();
            m_internalAbortFlag = false;
            return false;
         }
   }
   else if ( textVecProp )
   {
      indiClient->sendNewText( textVecProp );
      // if synch mode wait until completed or abort
      while ( textVecProp->s != IPS_OK && textVecProp->s != IPS_IDLE && !p_abort && !m_internalAbortFlag && !isAsynchCall )
         if ( textVecProp->s == IPS_ALERT )
         {
            writeCurrentMessageToConsole();
            m_internalAbortFlag = false;
            return false;
         }
   }

   p_newProperties.Clear();

   m_internalAbortFlag = false;

   return true;
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

void INDIDeviceControllerInstance::writeCurrentMessageToConsole()
{
   if ( Console().IsCurrentThreadConsole() )
      Console().NoteLn( "<end><cbr>* Message from INDI server: " + String( m_currentMessage ) );
}

bool INDIDeviceControllerInstance::ExecuteGlobal()
{

   Console console;

   console.NoteLn( "<end><cbr>INDI Control Client --- (C) Klaus Kretzschmar, 2014-2016" );
   console.Flush();


   o_getCommandResult.Clear();

   if ( !p_serverConnect )
   {
	   if ( indiClient.IsNull() ){
		   console.CriticalLn("The INDI device controller has not been initialized");
		   return false;
	   }

      // disconnect from server
      if ( indiClient->serverIsConnected() )
      {
         console.NoteLn( "* Disconnect from INDI server " + p_serverHostName + ", port=" + String( p_serverPort ) );
         indiClient->disconnectServer();
      }
      indiClient.Destroy();
      indiClient=nullptr;
      return true;
   }

   {
      IsoString host8 = p_serverHostName.ToUTF8();
      if ( indiClient.IsNull() )
         indiClient = new INDIClient( this, host8.c_str(), p_serverPort );
   }

   if ( !indiClient->serverIsConnected() )
      indiClient->connectServer();

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
      console.WriteLn( "Device=" + String( device ) + ", Property=" + String( property ) + ", Element=" + String( element ) );
      if ( getINDIPropertyItem( device, property, element, propItem ) )
      {
         o_getCommandResult = propItem.PropertyValue;
         console.WriteLn( "Value=" + String( o_getCommandResult ) );
      }
      else
      {
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
      indiClient->registerScriptInstance( this );
   }
   else if ( p_serverCommand == "RELEASE_INSTANCE" )
   {
      indiClient->registerScriptInstance( nullptr );
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
// EOF INDIDeviceControllerInstance.cpp - Released 2016/04/15 15:37:39 UTC
