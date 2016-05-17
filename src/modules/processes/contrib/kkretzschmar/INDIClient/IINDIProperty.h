//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.10.0163
// ----------------------------------------------------------------------------
// IINDIProperty.h - Released 2016/05/17 15:40:50 UTC
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
 * IINDIProperty.h
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#ifndef __IINDIProperty_h
#define __IINDIProperty_h

#include "INDI/indiproperty.h"

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class IProperty
{
public:

   IProperty( INDI::Property* property ) : m_property( property )
   {
   }

   virtual ~IProperty()
   {
   }

   virtual INDI::Property* getProperty()
   {
      return m_property;
   }

   virtual const INDI::Property* getProperty() const
   {
      return m_property;
   }

   virtual String getDeviceName() const
   {
      return String( m_property->getDeviceName() );
   }

   virtual String getName() const
   {
      return String( m_property->getName() );
   }

   virtual String getLabel() const
   {
      return String( m_property->getLabel() );
   }

   virtual INDI_TYPE getType() const
   {
      return m_property->getType();
   }

   virtual String getTypeStr() const
   {
      switch( m_property->getType() )
      {
      case INDI_SWITCH:
         return "INDI_SWITCH";
      case INDI_NUMBER:
         return "INDI_NUMBER";
      case INDI_LIGHT:
         return "INDI_LIGHT";
      case INDI_TEXT:
         return "INDI_TEXT";
      default:
         return "INDI_UNKNOWN";
      }
   }

   virtual IPState getState() const
   {
      return m_property->getState();
   }

   virtual size_type getNumOfElements() const
   {
      return 0;
   }

   virtual String getElementName( size_type i ) const
   {
      return "unsupported element";
   }

   virtual String getElementLabel( size_type i ) const
   {
      return "unsupported element";
   }

   virtual String getElementValue( size_type i ) const
   {
      return "unsupported value";
   }

   virtual String getNumberFormat( size_type i ) const
   {
      return String();
   }

   virtual void addElement( IsoString elementName, IsoString value )
   {
   }

   virtual void setDeviceName( IsoString device )
   {
   }

   virtual void setName( IsoString name )
   {
   }

protected:

   INDI::Property* m_property;
};

// ----------------------------------------------------------------------------

class NumberProperty : public IProperty
{
public:

   NumberProperty( INDI::Property* property ) : IProperty( property )
   {
   }

   virtual size_type getNumOfElements() const
   {
      return m_property->getNumber()->nnp;
   }

   virtual String getElementName( size_type i ) const;
   virtual String getElementLabel( size_type i ) const;
   virtual String getElementValue( size_type i ) const;
   virtual String getNumberFormat( size_type i ) const;

   virtual void addElement( IsoString elementName, IsoString value );

   virtual void setDeviceName( IsoString device )
   {
      strcpy( m_property->getNumber()->device, device.c_str() );
   }

   virtual void setName( IsoString name )
   {
      strcpy( m_property->getNumber()->name, name.c_str() );
   }
};

// ----------------------------------------------------------------------------

class TextProperty : public IProperty
{
public:

   TextProperty( INDI::Property* property ) : IProperty( property )
   {
   }

   virtual size_type getNumOfElements() const
   {
      return m_property->getText()->ntp;
   }

   virtual String getElementName( size_type i ) const;
   virtual String getElementLabel( size_type i ) const;
   virtual String getElementValue( size_type i ) const;

   virtual void addElement( IsoString elementName, IsoString value );

   virtual void setDeviceName( IsoString device )
   {
      strcpy( m_property->getText()->device, device.c_str() );
   }

   virtual void setName( IsoString name )
   {
      strcpy( m_property->getText()->name, name.c_str() );
   }
};

// ----------------------------------------------------------------------------

class SwitchProperty : public IProperty
{
public:

   SwitchProperty( INDI::Property* property ) : IProperty( property )
   {
   }

   virtual size_type getNumOfElements() const
   {
      return m_property->getSwitch()->nsp;
   }

   virtual String getElementName( size_type i ) const;
   virtual String getElementLabel( size_type i ) const;
   virtual String getElementValue( size_type i ) const;

   virtual void addElement( IsoString elementName, IsoString value );

   virtual void setDeviceName( IsoString device )
   {
      strcpy( m_property->getSwitch()->device, device.c_str() );
   }

   virtual void setName( IsoString name )
   {
      strcpy( m_property->getSwitch()->name, name.c_str() );
   }
};

// ----------------------------------------------------------------------------

class LightProperty : public IProperty
{
public:

   LightProperty( INDI::Property* property ) : IProperty( property )
   {
   }

   virtual size_type getNumOfElements() const
   {
      return m_property->getLight()->nlp;
   }

   virtual String getElementName( size_type i ) const;
   virtual String getElementLabel( size_type i ) const;
   virtual String getElementValue( size_type i ) const;

   virtual void addElement( IsoString elementName, IsoString value );

   virtual void setDeviceName( IsoString device )
   {
      strcpy( m_property->getLight()->device, device.c_str() );
   }

   virtual void setName( IsoString name )
   {
      strcpy( m_property->getLight()->name, name.c_str() );
   }
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

   static IProperty* Create( INDI::Property* );
   static IProperty* Create( INDI::Property*, INDI_TYPE );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IINDIProperty_h

// ----------------------------------------------------------------------------
// EOF IINDIProperty.h - Released 2016/05/17 15:40:50 UTC
