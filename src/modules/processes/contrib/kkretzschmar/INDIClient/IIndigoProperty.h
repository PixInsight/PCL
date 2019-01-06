//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0228
// ----------------------------------------------------------------------------
// IINDIProperty.h - Released 2018-11-23T18:45:59Z
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



#ifndef __IndigoProperty_h
#define __IIndigoProperty_h

#include<memory>

#include "indigo/indigo_bus.h"

#include <pcl/Exception.h>
#include <pcl/String.h>

#define CHECK_INDEX_THROWS( x )  \
   if ( int( i ) >= x )          \
      throw Error( "*** Error: " + String( PCL_FUNCTION_NAME ) + ": Invalid element index '" + String( i ) + "'" );


namespace pcl
{

// ----------------------------------------------------------------------------


class IProperty
{
public:

   IProperty( indigo_property* property ) : m_property( property )
   {
   }

   virtual ~IProperty()
   {
   }

   bool isHidden() const {
      return m_property->hidden;
   }

   virtual indigo_property* getProperty()
   {
      return m_property;
   }

   virtual const indigo_property* getProperty() const
   {
      return m_property;
   }

   virtual String getDeviceName() const
   {
      return String( m_property->device );
   }

   virtual String getName() const
   {
      return String( m_property->name );
   }

   virtual String getLabel() const
   {
      return String( m_property->label );
   }

   virtual indigo_property_type getType() const
   {
      return m_property->type;
   }

   virtual String getTypeStr() const
   {
      switch( m_property->type )
      {
      case INDIGO_SWITCH_VECTOR:
         return "INDI_SWITCH";
      case INDIGO_NUMBER_VECTOR:
         return "INDI_NUMBER";
      case INDIGO_LIGHT_VECTOR:
         return "INDI_LIGHT";
      case INDIGO_TEXT_VECTOR:
         return "INDI_TEXT";
      default:
         return "INDI_UNKNOWN";
      }
   }

   virtual indigo_property_state getState() const
   {
      return m_property->state;
   }

   virtual size_type getNumOfElements() const
   {
      return m_property->count;
   }

   virtual String getElementName( size_type i ) const
   {
      CHECK_INDEX_THROWS( m_property->count );
      return m_property->items[i].name;
   }

   virtual String getElementLabel( size_type i ) const
   {
      CHECK_INDEX_THROWS( m_property->count );
      return m_property->items[i].label;
   }

   virtual String getElementValue( size_type i ) const
   {
      return "unsupported value";
   }

   virtual String getNumberFormat( size_type i ) const
   {
      return String();
   }

   virtual void setDeviceName( IsoString device )
   {
      strcpy( m_property->device, device.c_str() );
   }

   virtual void setName( IsoString name )
   {
      strcpy( m_property->name, name.c_str() );
   }

   virtual std::unique_ptr<indigo_property> clone() const  {
      return nullptr;
   }

protected:

   indigo_property* m_property;
};

// ----------------------------------------------------------------------------

class NumberProperty : public IProperty
{
public:

   NumberProperty( indigo_property* property ) : IProperty( property )
   {
   }


   virtual String getElementValue( size_type i ) const;
   virtual String getElementTarget( size_type i ) const;
   virtual String getElementMinValue( size_type i ) const;
   virtual String getElementMaxValue( size_type i ) const;
   virtual String getElementValueStep( size_type i ) const;
   virtual String getNumberFormat( size_type i ) const;

   virtual std::unique_ptr<indigo_property> clone() const;

};

// ----------------------------------------------------------------------------

class TextProperty : public IProperty
{
public:

   TextProperty( indigo_property* property ) : IProperty( property )
   {
   }

   virtual String getElementValue( size_type i ) const;

   virtual std::unique_ptr<indigo_property> clone() const;
};

// ----------------------------------------------------------------------------

class SwitchProperty : public IProperty
{
public:

   SwitchProperty( indigo_property* property ) : IProperty( property )
   {
   }


   virtual String getElementValue( size_type i ) const;

   virtual std::unique_ptr<indigo_property> clone() const;
};

// ----------------------------------------------------------------------------

class LightProperty : public IProperty
{
public:

   LightProperty( indigo_property* property ) : IProperty( property )
   {
   }

   virtual String getElementValue( size_type i ) const;

   virtual std::unique_ptr<indigo_property> clone() const;
};

// ----------------------------------------------------------------------------

class PropertyUtils
{
public:

   static String Device( const String& key )
   {
      size_type startpos = key.Find( '/' );
      size_type endpos   = key.Find( '/', 1 );
      return key.Substring( startpos+1, endpos-startpos-1 );
   }

   static String Property( const String& key )
   {
      size_type startpos = key.Find( '/', 1 );
      size_type endpos   = key.Find( '/', startpos+1 );
      return key.Substring( startpos+1, endpos-startpos-1 );
   }

   static String Element( const String& key )
   {
      size_type startpos1 = key.Find( '/', 1 );
      size_type startpos2 = key.Find( '/', startpos1+1 );
      size_type endpos    = key.Find( '/', startpos2+1 );
      return key.Substring( startpos2+1, endpos-startpos2 );
   }

   static String Key( const String& device )
   {
      return '/' + device;
   }

   static String Key( const String& device, const String& property )
   {
      return '/' + device + '/' + property;
   }

   static String Key( const String& device, const String& property, const String& element )
   {
      return '/' + device + '/' + property + '/' + element;
   }

   static String FormattedNumber( const String& number, IsoString format );
};

// ----------------------------------------------------------------------------

class PropertyFactory
{
public:

   static IProperty* Create( indigo_property* );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IIndigoProperty_h

// ----------------------------------------------------------------------------
// EOF IIndigoProperty.h - Released 2018-11-23T18:45:59Z
