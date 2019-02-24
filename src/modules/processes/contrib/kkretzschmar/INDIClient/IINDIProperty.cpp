//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0238
// ----------------------------------------------------------------------------
// IINDIProperty.cpp - Released 2019-01-21T12:06:42Z
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

/*
 * IINDIProperty.cpp
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */

#include "IINDIProperty.h"

#include <pcl/Exception.h>
#include <pcl/StringList.h>

#define CHECK_INDEX_THROWS( x )  \
   if ( int( i ) >= x )          \
      throw Error( "*** Error: " + String( PCL_FUNCTION_NAME ) + ": Invalid element index '" + String( i ) + "'" );

namespace pcl
{

// ----------------------------------------------------------------------------

String SwitchProperty::getElementName( size_t i ) const
{
   ISwitchVectorProperty* svp = m_property->getSwitch();
   CHECK_INDEX_THROWS( svp->nsp );
   return svp->sp[i].name;
}

String SwitchProperty::getElementLabel( size_t i ) const
{
   ISwitchVectorProperty* svp = m_property->getSwitch();
   CHECK_INDEX_THROWS( svp->nsp );
   return svp->sp[i].label;
}

String SwitchProperty::getElementValue( size_t i ) const
{
   ISwitchVectorProperty* svp = m_property->getSwitch();
   CHECK_INDEX_THROWS( svp->nsp );
   return svp->sp[i].s == ISS_ON  ? "ON" : "OFF";
}

void SwitchProperty::addElement( IsoString elementName, IsoString value )
{
   ISwitchVectorProperty* svp = m_property->getSwitch();
   ISwitch* sp = svp->sp;
   int nsp = svp->nsp;
   sp = reinterpret_cast<ISwitch*>( realloc( sp, (nsp + 1)*sizeof( ISwitch ) ) );
   if ( sp == nullptr )
      throw std::bad_alloc();
   strcpy( sp->name, elementName.c_str() );
   sp->s = (value == "ON") ? ISS_ON : ISS_OFF;
   sp->svp = svp;
   svp->nsp++;
   svp->sp = sp;
}

// ----------------------------------------------------------------------------

String NumberProperty::getElementName( size_t i ) const
{
   INumberVectorProperty* nvp = m_property->getNumber();
   CHECK_INDEX_THROWS( nvp->nnp );
   return nvp->np[i].name;
}

String NumberProperty::getElementLabel( size_t i ) const
{
   INumberVectorProperty* nvp = m_property->getNumber();
   CHECK_INDEX_THROWS( nvp->nnp );
   return nvp->np[i].label;
}

String NumberProperty::getElementValue( size_t i ) const
{
   INumberVectorProperty* nvp = m_property->getNumber();
   CHECK_INDEX_THROWS( nvp->nnp );
   return String( nvp->np[i].value ); // ### N.B. All String::String( <number> ) constructors are explicit.
}

String NumberProperty::getNumberFormat( size_t i ) const
{
   INumberVectorProperty* nvp = m_property->getNumber();
   CHECK_INDEX_THROWS( nvp->nnp );
   return nvp->np[i].format;
}

void NumberProperty::addElement( IsoString elementName, IsoString value )
{
   if ( !value.IsEmpty() )
   {
      INumberVectorProperty* nvp = m_property->getNumber();
      INumber* np = nvp->np;
      int nnp = nvp->nnp;
      np = reinterpret_cast<INumber*>( realloc( np, (nnp + 1)*sizeof( INumber ) ) );
      if ( np == nullptr )
         throw std::bad_alloc();
      strcpy( np->name, elementName.c_str() );
      value.TryToDouble( np->value );
      np->nvp = nvp;
      nvp->nnp++;
      nvp->np = np;
   }
}

// ----------------------------------------------------------------------------

String TextProperty::getElementName( size_t i ) const
{
   ITextVectorProperty* tvp = m_property->getText();
   CHECK_INDEX_THROWS( tvp->ntp );
   return tvp->tp[i].name;
}

String TextProperty::getElementLabel( size_t i ) const
{
   ITextVectorProperty* tvp = m_property->getText();
   CHECK_INDEX_THROWS( tvp->ntp );
   return tvp->tp[i].label;
}

String TextProperty::getElementValue( size_t i ) const
{
   ITextVectorProperty* tvp = m_property->getText();
   CHECK_INDEX_THROWS( tvp->ntp );
   return tvp->tp[i].text;
}

void TextProperty::addElement( IsoString elementName, IsoString value )
{
   ITextVectorProperty* tvp = m_property->getText();
   IText* tp = tvp->tp;
   int ntp = tvp->ntp;
   tp = reinterpret_cast<IText*>( realloc( tp, (ntp + 1)* sizeof( IText ) ) );
   if ( tp == nullptr )
      throw std::bad_alloc();
   strcpy( tp->name, elementName.c_str() );
   tp->text = value.Release(); // N.B.: Can do this because value is being passed by value, so we have a local copy.
   tp->tvp = tvp;
   tvp->ntp++;
   tvp->tp = tp;
}

// ----------------------------------------------------------------------------

String LightProperty::getElementName( size_t i ) const
{
   ILightVectorProperty* lvp = m_property->getLight();
   CHECK_INDEX_THROWS( lvp->nlp );
   return lvp->lp[i].name;
}

String LightProperty::getElementLabel( size_t i ) const
{
   ILightVectorProperty* lvp = m_property->getLight();
   CHECK_INDEX_THROWS( lvp->nlp );
   return lvp->lp[i].label;
}

String LightProperty::getElementValue( size_t i ) const
{
   ILightVectorProperty* lvp = m_property->getLight();
   CHECK_INDEX_THROWS( lvp->nlp );
   switch ( lvp->lp[i].s )
   {
   case IPS_IDLE:
      return "IDLE";
   case IPS_OK:
      return "OK";
   case IPS_BUSY:
      return "BUSY";
   case IPS_ALERT:
      return "ALERT";
   default:
      return "UNSUPPORTED";
   }
}

void LightProperty::addElement( IsoString elementName, IsoString value )
{
   ILightVectorProperty* lvp = m_property->getLight();
   ILight* lp = lvp->lp;
   int nlp = lvp->nlp;
   lp = reinterpret_cast<ILight*>( realloc( lp, (nlp + 1)*sizeof( ILight ) ) );
   if ( lp == nullptr )
      throw std::bad_alloc();
   strcpy( lp->name, elementName.c_str() );
   if (value == "IDLE")
      lp->s = IPS_IDLE;
   else if (value == "OK")
      lp->s = IPS_OK;
   else if (value == "BUSY")
      lp->s = IPS_BUSY;
   else if (value == "ALERT")
      lp->s = IPS_ALERT;
   else
      throw Error( "Internal error: INDIDeviceController: Invalid property value in " + String( PCL_FUNCTION_NAME ) );
   lp->lvp = lvp;
   lvp->nlp++;
   lvp->lp = lp;
}

// ----------------------------------------------------------------------------

String PropertyUtils::FormattedNumber( const String& number, IsoString format )
{
   if ( number.IsEmpty() )
      return String();

   float value = 0;
   number.TryToFloat( value );

   size_type im = format.Find( 'm' );
   if ( im == String::notFound )
      return String().Format( format.c_str(), value );

   format.DeleteRight( im );
   format.DeleteLeft( 1 );
   StringList tokens;
   format.Break( tokens, '.', true/*trim*/ );

   int fraction = 0;
   tokens[1].TryToInt( fraction );

   int width = 0;
   tokens[0].TryToInt( width );
   width -= fraction;
   if ( width <= 0 )
      throw Error( "Internal error: INDIClient: Invalid number format in " + String( PCL_FUNCTION_NAME ) );

   int items, precision;
   switch ( fraction )
   {
   case 3:
      items = 2;
      precision = 0;
      break;
   case 5:
      items = 2;
      precision = 1;
      break;
   case 6:
      items = 3;
      precision = 0;
      break;
   case 8:
      items = 3;
      precision = 1;
      break;
   case 9:
   default: // ?!
      items = 3;
      precision = 2;
      break;
   }

   return String::ToSexagesimal( value, SexagesimalConversionOptions( items, precision, false/*sign*/, width ) );
}

// ----------------------------------------------------------------------------

IProperty* PropertyFactory::Create( INDI::Property* property )
{
   switch( property->getType() )
   {
   case INDI_NUMBER:
      return new NumberProperty( property );
   case INDI_TEXT:
      return new TextProperty( property );
   case INDI_SWITCH:
      return new SwitchProperty( property );
   case INDI_LIGHT:
      return new LightProperty( property );
   default:
      return new IProperty( property );
   }
}

IProperty* PropertyFactory::Create( INDI::Property* property, INDI_TYPE type )
{
   property->setType( type );
   switch( type )
   {
   case INDI_NUMBER:
      {
         INumberVectorProperty* nProperty = new INumberVectorProperty;
         property->setProperty( nProperty );
         return new NumberProperty( property );
      }
   case INDI_TEXT:
      {
         ITextVectorProperty* tProperty = new ITextVectorProperty;
         property->setProperty( tProperty );
         return new TextProperty( property );
      }
   case INDI_SWITCH:
      {
         ISwitchVectorProperty* sProperty = new ISwitchVectorProperty;
         property->setProperty( sProperty );
         return new SwitchProperty( property );
      }
   case INDI_LIGHT:
      {
         ILightVectorProperty* lProperty = new ILightVectorProperty;
         property->setProperty( lProperty );
         return new LightProperty( property );
      }
   default:
      throw Error( "Internal error: INDIClient: Unknown property type in " + String( PCL_FUNCTION_NAME ) );
   }
}


} // pcl

// ----------------------------------------------------------------------------
// EOF IINDIProperty.cpp - Released 2019-01-21T12:06:42Z
