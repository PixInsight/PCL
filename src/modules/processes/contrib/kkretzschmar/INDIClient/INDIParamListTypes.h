//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.03.0102
// ----------------------------------------------------------------------------
// INDIParamListTypes.h - Released 2016/03/18 13:15:37 UTC
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

/*
 * INDIParamListTypes.h
 *
 *  Created on: May 26, 2014
 *      Author: klaus
 */

#ifndef PIXINSIGHTINDIPARAMLISTTYPES_H_
#define PIXINSIGHTINDIPARAMLISTTYPES_H_

#include <memory>

#include <pcl/Mutex.h>
#include <pcl/AutoLock.h>

#include "INDI/indibase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

struct INDIDeviceListItem
{
   INDIDeviceListItem()
   {
   }

   String DeviceName;
   String DeviceLabel;

   bool operator ==( const INDIDeviceListItem& rhs ) const
   {
      return DeviceName == rhs.DeviceName;
   }
};

typedef enum
{
   Insert,
   Update,
   Remove,
   Idle
}
PropertyFlagType;

struct INDIPropertyListItem
{
   INDIPropertyListItem() : PropertyFlag( Idle )
   {
   }

   String           Device;
   String           Property;
   INDI_TYPE        PropertyType;
   String           PropertyTypeStr;
   String           Element;
   unsigned         PropertyState;
   PropertyFlagType PropertyFlag;
   String           PropertyNumberFormat;
   String           PropertyLabel;
   String           ElementLabel;
   String           PropertyKey;
   String           PropertyValue;
   String           NewPropertyValue;

   bool operator ==( const INDIPropertyListItem& rhs ) const
   {
      return PropertyKey == rhs.PropertyKey;
   }
};

struct INDINewPropertyListItem
{
   INDINewPropertyListItem()
   {
   }

   String Device;
   String Property;
   String PropertyKey;
   String PropertyType;
   String Element;
   String NewPropertyValue;

   bool operator ==( const INDINewPropertyListItem& rhs ) const
   {
      return Device == rhs.Device &&
             Property == rhs.Property &&
             PropertyKey == rhs.PropertyKey &&
             PropertyType == rhs.PropertyType &&
             Element == rhs.Element &&
             NewPropertyValue == rhs.NewPropertyValue;
   }
};

class ExclPropertyList
{
public:

   explicit
   ExclPropertyList( Array<INDIPropertyListItem>& propertyList ) :
      m_locker( nullptr ),
      m_propertyListRef( propertyList )
   {
   }

   ExclPropertyList( pcl::Mutex& mutex, Array<INDIPropertyListItem>&  propertyList ) :
      m_locker( new AutoLock( mutex ) ),
      m_propertyListRef( propertyList )
   {
   }

   Array<INDIPropertyListItem>* get()
   {
      return &m_propertyListRef;
   }

private:

   std::shared_ptr<pcl::AutoLock> m_locker;
   Array<INDIPropertyListItem>&   m_propertyListRef;
};

// ----------------------------------------------------------------------------

} // pcl

#endif /* PIXINSIGHTINDIPARAMLISTTYPES_H_ */

// ----------------------------------------------------------------------------
// EOF INDIParamListTypes.h - Released 2016/03/18 13:15:37 UTC
