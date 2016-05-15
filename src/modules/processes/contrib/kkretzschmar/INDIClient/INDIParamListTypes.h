//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.09.0153
// ----------------------------------------------------------------------------
// INDIParamListTypes.h - Released 2016/05/08 20:36:42 UTC
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

#ifndef __INDIParamListTypes_h
#define __INDIParamListTypes_h

#include "INDI/indibase.h"

#include <pcl/Array.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct INDIDeviceListItem
{
   String DeviceName;
   String DeviceLabel;

   bool operator ==( const INDIDeviceListItem& rhs ) const
   {
      return DeviceName == rhs.DeviceName;
   }

   bool operator <( const INDIDeviceListItem& rhs ) const
   {
      return DeviceName < rhs.DeviceName;
   }
};

struct INDIPropertyListItem
{
   String    Device;
   String    Property;
   INDI_TYPE PropertyType;
   String    PropertyTypeStr;
   String    Element;
   unsigned  PropertyState;
   String    PropertyNumberFormat;
   String    PropertyLabel;
   String    ElementLabel;
   String    PropertyKey;
   String    PropertyValue;
   String    NewPropertyValue;

   bool operator ==( const INDIPropertyListItem& rhs ) const
   {
      return PropertyKey == rhs.PropertyKey;
   }

   bool operator <( const INDIPropertyListItem& rhs ) const
   {
      return PropertyKey < rhs.PropertyKey;
   }
};


struct ElementValuePair {
	String Element;
	String Value;
	ElementValuePair(String element, String value):Element(element), Value(value){}
	bool operator == (const ElementValuePair& rhs) const
	{
		return Element == rhs.Element &&
			   Value == rhs.Value;
	}
};

struct INDINewPropertyItem
{

   String Device;
   String Property;
   String PropertyKey;
   String PropertyType;
   Array<ElementValuePair> ElementValue;

   bool operator ==( const INDINewPropertyItem& rhs ) const
   {
      return Device == rhs.Device &&
             Property == rhs.Property &&
			 PropertyKey == rhs.PropertyKey &&
             PropertyType == rhs.PropertyType &&
			 ElementValue == rhs.ElementValue ;
   }
};


struct INDINewPropertyListItem
{
	String PropertyKey;
	String PropertyType;
	String NewPropertyValue;

	bool operator ==( const INDINewPropertyListItem& rhs ) const
	   {
	      return PropertyKey == rhs.PropertyKey &&
	             PropertyType == rhs.PropertyType &&
				 NewPropertyValue == rhs.NewPropertyValue ;
	   }

};

typedef Array<INDIDeviceListItem>      INDIDeviceListItemArray;
typedef Array<INDIPropertyListItem>    INDIPropertyListItemArray;
typedef Array<INDINewPropertyListItem> INDINewPropertyListItemArray;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIParamListTypes_h

// ----------------------------------------------------------------------------
// EOF INDIParamListTypes.h - Released 2016/05/08 20:36:42 UTC
