//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIClient.cpp - Released 2016/04/28 15:13:36 UTC
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

#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "INDIClient.h"
#include "PropertyNode.h"

#include <pcl/Console.h>
#include <pcl/GlobalSettings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static AutoPointer<INDIClient> s_indiClient;

INDIClient* INDIClient::TheClient()
{
   return s_indiClient;
}

INDIClient* INDIClient::NewClient( IINDIDeviceControllerInstance* instance )
{
   return s_indiClient = new INDIClient( instance );
}

INDIClient* INDIClient::NewClient( IINDIDeviceControllerInstance* instance, const IsoString& hostname, unsigned int port )
{
   return s_indiClient = new INDIClient( instance, hostname.c_str(), port );
}

void INDIClient::DestroyClient()
{
   s_indiClient.Destroy();
}

// ----------------------------------------------------------------------------

void INDIClient::runOnPropertyTable( IProperty* INDIProperty, const ArrayOperator<INDIPropertyListItem>* arrayOp, PropertyFlagType flag )
{
   // get propertyList with exclusive access
   ExclPropertyList propertyList = m_instance->getExclusivePropertyList();
   INDIPropertyListItem propertyListItem;
   propertyListItem.Device = INDIProperty->getDeviceName();
   propertyListItem.Property = INDIProperty->getName();
   propertyListItem.PropertyType = INDIProperty->getType();
   propertyListItem.PropertyTypeStr = INDIProperty->getTypeStr();
   propertyListItem.PropertyState = INDIProperty->getState();
   propertyListItem.PropertyLabel = INDIProperty->getLabel();

   for ( size_t i = 0; i < INDIProperty->getNumOfElements(); i++ )
   {
      propertyListItem.Element = INDIProperty->getElementName( i );
      propertyListItem.PropertyKey = '/' + propertyListItem.Device
                                   + '/' + propertyListItem.Property
                                   + '/' + propertyListItem.Element;
      propertyListItem.PropertyValue = INDIProperty->getElementValue( i );
      propertyListItem.PropertyFlag = flag;
      propertyListItem.PropertyNumberFormat = INDIProperty->getNumberFormat( i );
      propertyListItem.ElementLabel = INDIProperty->getElementLabel( i );
      arrayOp->run( *propertyList.get(), propertyListItem );
      if ( m_scriptInstance )
      {
         ExclPropertyList scriptPropertyList = m_scriptInstance->getExclusivePropertyList();
         arrayOp->run( *scriptPropertyList.get(), propertyListItem );
      }
   }
}

void INDIClient::newDevice( INDI::BaseDevice* dp )
{
   assert( dp != nullptr );
   if ( m_instance != nullptr )
   {
      INDIDeviceListItem deviceListItem;
      deviceListItem.DeviceName = dp->getDeviceName();
      deviceListItem.DeviceLabel = dp->getDriverName();
      m_instance->getDeviceList().Append( deviceListItem );
      if ( m_scriptInstance )
         m_scriptInstance->getDeviceList().Append( deviceListItem );
   }
}

void INDIClient::deleteDevice( INDI::BaseDevice* dp )
{
   assert( dp != nullptr );
   if ( m_instance != nullptr )
   {
      for ( auto item : m_instance->getPropertyList() )
      {
         if ( item.Device == dp->getDeviceName() )
            item.PropertyFlag = Remove;
      }
      // TODO script instance
   }
}

void INDIClient::newProperty( INDI::Property* property )
{
   assert( property != nullptr );
   // add property to the property process parameter table
   runOnPropertyTable( PropertyFactory::create( property ),
                       dynamic_cast<ArrayOperator<INDIPropertyListItem>*>( new ArrayAppend<INDIPropertyListItem>() ),
                       Insert );
   setBLOBMode( B_ALSO, property->getDeviceName() );
}

void INDIClient::removeProperty( INDI::Property* property )
{
   if ( property != nullptr )
   {
      IProperty* INDIProperty = PropertyFactory::create( property );
      ArrayOperator<INDIPropertyListItem>* update = dynamic_cast<ArrayOperator<INDIPropertyListItem>*>( new ArrayUpdate<INDIPropertyListItem>() );
      INDIPropertyListItem propertyListItem;
      propertyListItem.Device = INDIProperty->getDeviceName();
      propertyListItem.Property = INDIProperty->getName();
      propertyListItem.PropertyType = INDIProperty->getType();
      propertyListItem.PropertyTypeStr = INDIProperty->getTypeStr();
      propertyListItem.PropertyState = INDIProperty->getState();

      for ( size_t i = 0; i < INDIProperty->getNumOfElements(); i++ )
      {
         propertyListItem.Element = INDIProperty->getElementName( i );
         propertyListItem.PropertyKey = '/' + propertyListItem.Device
                                      + '/' + propertyListItem.Property
                                      + '/' + propertyListItem.Element;
         propertyListItem.PropertyValue = INDIProperty->getElementValue( i );
         propertyListItem.PropertyFlag = Remove;
         update->run( m_instance->getPropertyList(), propertyListItem );
         if ( m_scriptInstance )
            update->run( m_scriptInstance->getPropertyList(), propertyListItem );
      }
   }
}

void INDIClient::newMessage( INDI::BaseDevice* dp, int messageID )
{
   const char* message = dp->messageQueue( messageID );
   if ( message != nullptr )
   {
      m_instance->SetCurrentServerMessage( message );
      if ( m_scriptInstance )
         m_scriptInstance->SetCurrentServerMessage( message );
   }
}

void INDIClient::newSwitch( ISwitchVectorProperty* svp )
{
   INDI::Property* property = new INDI::Property();
   property->setProperty( svp );
   property->setType( INDI_SWITCH );
   runOnPropertyTable( PropertyFactory::create( property ),
                       dynamic_cast<ArrayOperator<INDIPropertyListItem>*>( new ArrayUpdate<INDIPropertyListItem>() ),
                       Update );
}

void INDIClient::newNumber( INumberVectorProperty* nvp )
{
   INDI::Property* property = new INDI::Property();
   property->setProperty( nvp );
   property->setType( INDI_NUMBER );
   runOnPropertyTable( PropertyFactory::create( property ),
                       dynamic_cast<ArrayOperator<INDIPropertyListItem>*>( new ArrayUpdate<INDIPropertyListItem>() ),
                       Update );
}

void INDIClient::newText( ITextVectorProperty* tvp )
{
   INDI::Property* property = new INDI::Property();
   property->setProperty( tvp );
   property->setType( INDI_TEXT );
   runOnPropertyTable( PropertyFactory::create( property ),
                       dynamic_cast<ArrayOperator<INDIPropertyListItem>*>( new ArrayUpdate<INDIPropertyListItem>() ),
                       Update );
}

void INDIClient::newLight( ILightVectorProperty* lvp )
{
   INDI::Property* property = new INDI::Property();
   property->setProperty( lvp );
   property->setType( INDI_LIGHT );
   runOnPropertyTable( PropertyFactory::create( property ),
                       dynamic_cast<ArrayOperator<INDIPropertyListItem>*>( new ArrayUpdate<INDIPropertyListItem>() ),
                       Update );
}

void INDIClient::newBLOB( IBLOB* bp )
{
   String dir = PixInsightSettings::GlobalString( "ImageWindow/DownloadsDirectory" );
   if ( dir.IsEmpty() ) // this cannot happen
      dir = File::SystemTempDirectory();
   String filePath = dir + '/' + bp->label + bp->format;
   File myfile = File::CreateFileForWriting( filePath );
   myfile.Write( bp->blob, bp->size );
   myfile.Close();
   m_instance->SetDownloadedImagePath( filePath );
   if ( m_scriptInstance )
      m_scriptInstance->SetDownloadedImagePath( filePath );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIClient.cpp - Released 2016/04/28 15:13:36 UTC
