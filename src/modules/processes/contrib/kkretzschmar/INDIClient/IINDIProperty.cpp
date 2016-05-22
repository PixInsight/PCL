//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.10.0168
// ----------------------------------------------------------------------------
// IINDIProperty.cpp - Released 2016/05/18 10:06:42 UTC
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
      throw Error( "INDIDeviceController: Internal error: Invalid property value in " + String( PCL_FUNCTION_NAME ) );
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
      throw Error( "INDIClient: Internal error: Invalid number format in " + String( PCL_FUNCTION_NAME ) );

   int hours = TruncInt( value );

   switch ( fraction )
   {
   case 3:
      {
         int minutes = RoundInt( (value - hours)*60 );
         IsoString formatStr = '%' + IsoString().Format( "%dd", width ) + ":%02d";
         return String().Format( formatStr.c_str(), hours, Abs( minutes ) );
      }
   case 5:
      {
         int minutes     = TruncInt( (value - hours)*60 );
         int minutesfrac = RoundInt( ((value - hours)*60 - minutes)*10 );
         IsoString formatStr = '%' + IsoString().Format( "%dd", width ) + ":%02d.%d";
         return String().Format( formatStr.c_str(), hours, Abs( minutes ), Abs( minutesfrac ) );
      }
   case 6:
      {
         int minutes     = TruncInt( (value - hours)*60 );
         int seconds     = RoundInt( ((value - hours)*60 - minutes)*60 );
         IsoString formatStr = '%' + IsoString().Format( "%dd", width ) + ":%02d:%02d";
         return String().Format( formatStr.c_str(), hours, Abs( minutes ), Abs( seconds ) );
      }
   case 8:
      {
         int minutes     = TruncInt( (value - hours)*60 );
         int seconds     = TruncInt( ((value - hours)*60 - minutes)*60 );
         int secondsfrac = RoundInt( (((value - hours)*60 - minutes)*60 - seconds)*10 );
         IsoString formatStr = '%' + IsoString().Format( "%dd", width ) + ":%02d:%02d.%d";
         return String().Format( formatStr.c_str(), hours, Abs( minutes ), Abs( seconds ), Abs( secondsfrac ) );
      }
   case 9:
      {
         int minutes     = TruncInt( (value - hours)*60 );
         int seconds     = TruncInt( ((value - hours)*60 - minutes)*60 );
         int secondsfrac = RoundInt( (((value - hours)*60 - minutes)*60 - seconds)*100 );
         IsoString formatStr = '%' + IsoString().Format( "%dd", width ) + ":%02d:%02d.%02d";
         return String().Format( formatStr.c_str(), hours, Abs( minutes ), Abs( seconds ), Abs( secondsfrac ) );
      }
   default:
      return String();
   }
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
      throw Error( "INDIClient: Internal error: Unknown property type in " + String( PCL_FUNCTION_NAME ) );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IINDIProperty.cpp - Released 2016/05/18 10:06:42 UTC
