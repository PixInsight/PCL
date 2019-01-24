//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0238
// ----------------------------------------------------------------------------
// INDIDeviceControllerInstance.cpp - Released 2019-01-21T12:06:42Z
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
#include "INDIDeviceControllerInstance.h"
#include "INDIDeviceControllerParameters.h"
#include "INDIDeviceControllerProcess.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/Mutex.h>
#include <pcl/StdStatus.h>

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
   o_getCommandResult()
{
   AcquireINDIClientProperties();
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
      p_verbosity = x->p_verbosity;
      p_newProperties = x->p_newProperties;
      p_getCommandParameters = x->p_getCommandParameters;
      o_devices = x->o_devices;
      o_properties = x->o_properties;
      o_getCommandResult = x->o_getCommandResult;
   }
}

bool INDIDeviceControllerInstance::CanExecuteOn( const View&, String& whyNot ) const
{
   whyNot = "INDIDeviceController can only be executed in the global context.";
   return false;
}

bool INDIDeviceControllerInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

static void GetNewPropertyListItemParametersFromKey( const INDINewPropertyListItem& listItem, INDINewPropertyItem& item )
{
   StringList items;
   listItem.PropertyKey.Break( items, '/', true/*trim*/, listItem.PropertyKey.StartsWith( '/' ) ? 1 : 0 );
   if ( items.Length() == 3 )
      if ( !items[0].IsEmpty() && !items[1].IsEmpty() && !items[2].IsEmpty() )
      {
         item.Device = items[0];
         item.Property = items[1];
         item.PropertyKey = '/' + items[0] + '/' + items[1] + '/' + items[2];
         item.PropertyType = listItem.PropertyType;
         item.ElementValues << ElementValue( items[2], listItem.NewPropertyValue );
         return;
      }
   throw Error( "Invalid property key '" + item.PropertyKey + "'" );
}

bool INDIDeviceControllerInstance::ExecuteGlobal()
{
   Console console;

   Exception::DisableGUIOutput();

   INDIClient* indi = INDIClient::TheClient();

   try
   {
      if ( p_serverConnect )
      {
         if ( indi == nullptr )
         {
            indi = INDIClient::NewClient( p_serverHostName.ToUTF8(), p_serverPort );
            if ( p_verbosity > 0 )
            {
               console.NoteLn( "<end><cbr>INDI Control Client --- (C) Klaus Kretzschmar, 2014-2016" );
               console.Flush();
            }
         }

         indi->SetVerbosity( p_verbosity );

         if ( !indi->IsServerConnected() )
         {
            indi->connectServer();
            if ( !indi->IsServerConnected() )
               throw Error( "INDIDeviceControllerInstance: Failure to connect to INDI server " + p_serverHostName + ", port=" + String( p_serverPort ) );

            if ( p_verbosity > 0 )
            {
               console.NoteLn( "* Successfully connected to INDI server " + p_serverHostName + ", port=" + String( p_serverPort ) );
               console.Flush();
            }
         }

         if ( !p_serverCommand.IsEmpty() )
         {
            console.EnableAbort();

            bool isTryGet = p_serverCommand == "TRY_GET";
            if ( p_serverCommand == "GET" || isTryGet)
            {
               o_getCommandResult.Clear();
               String device( PropertyUtils::Device( p_getCommandParameters ) );
               String property( PropertyUtils::Property( p_getCommandParameters ) );
               String element( PropertyUtils::Element( p_getCommandParameters ) );
               INDIPropertyListItem item;
               if ( !indi->GetPropertyItem( device, property, element, item ) && !isTryGet)
                  throw Error( "INDIDeviceControllerInstance: Could not get value of property '" + String( p_getCommandParameters ) + "'" );
               o_getCommandResult = item.PropertyValue;
               if ( p_verbosity > 1 )
                  console.WriteLn( "<end><cbr>Device=" + device +
                                   ", Property=" + property +
                                   ", Element=" + element +
                                   ", Value=" + o_getCommandResult );
            }
            else if ( p_serverCommand == "SET" )
            {
               for ( auto newListProperty : p_newProperties )
               {
                  INDINewPropertyItem newProperty;
                  GetNewPropertyListItemParametersFromKey( newListProperty, newProperty );
                  if ( !indi->SendNewPropertyItem( newProperty, false/*async*/ ) )
                     throw Error( "INDIDeviceControllerInstance: Failure to send new property values." );
               }
               p_newProperties.Clear();
            }
            else if ( p_serverCommand == "SET_ASYNC" )
            {
               for ( auto newListProperty : p_newProperties )
               {
                  INDINewPropertyItem newProperty;
                  GetNewPropertyListItemParametersFromKey( newListProperty, newProperty );
                  if ( !indi->SendNewPropertyItem( newProperty, true/*async*/ ) )
                     throw Error( "INDIDeviceControllerInstance: Failure to send new property values (asynchronous)." );
               }
               p_newProperties.Clear();
            }
            else
               throw Error( "INDIDeviceControllerInstance: Unknown command '" + p_serverCommand + "'" );

            console.Flush();
         }

         AcquireINDIClientProperties();
      }
      else
      {
         if ( indi != nullptr )
         {
            if ( indi->IsServerConnected() )
            {
               AcquireINDIClientProperties();
               indi->SetVerbosity( p_verbosity );
               indi->disconnectServer();
               if ( p_verbosity > 0 )
                  console.NoteLn( "* Disconnected from INDI server " + p_serverHostName + ", port=" + String( p_serverPort ) );
            }

            INDIClient::DestroyClient();
         }
      }

      return true;
   }
   catch ( const Exception& x )
   {
      if ( p_verbosity > 0 )
         x.Show();
      return false;
   }
   catch ( ... )
   {
      throw;
   }
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

void INDIDeviceControllerInstance::AcquireINDIClientProperties()
{
   o_devices.Clear();
   o_properties.Clear();

   if ( INDIClient::HasClient() )
      if ( INDIClient::TheClient()->IsServerConnected() )
      {
         {
            ExclConstDeviceList x = INDIClient::TheClient()->ConstDeviceList();
            o_devices = static_cast<const INDIDeviceListItemArray&>( x );
         }
         {
            ExclConstPropertyList y = INDIClient::TheClient()->ConstPropertyList();
            o_properties = static_cast<const INDIPropertyListItemArray&>( y );
         }
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInstance.cpp - Released 2019-01-21T12:06:42Z
