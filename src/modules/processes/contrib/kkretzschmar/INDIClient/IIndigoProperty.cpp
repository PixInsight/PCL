//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0228
// ----------------------------------------------------------------------------
// IINDIProperty.cpp - Released 2018-11-23T18:45:59Z
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


#include "IIndigoProperty.h"

#include <pcl/StringList.h>


namespace pcl
{


// ----------------------------------------------------------------------------


String SwitchProperty::getElementValue( size_t i ) const
{
   CHECK_INDEX_THROWS( m_property->count );
   return m_property->items[i].sw.value  ? "ON" : "OFF";
}

std::unique_ptr<indigo_property> SwitchProperty::clone() const {
    std::unique_ptr<indigo_property> property(indigo_init_switch_property(NULL, m_property->device, m_property->name, NULL, NULL, INDIGO_OK_STATE, INDIGO_RW_PERM, m_property->rule, m_property->count));
    memcpy(property.get(), m_property, sizeof(indigo_property) + m_property->count * sizeof(indigo_item));
    return property;
}

// ----------------------------------------------------------------------------


String NumberProperty::getElementValue( size_type i ) const {
    CHECK_INDEX_THROWS( m_property->count );
    return IsoString(static_cast<double>(m_property->items[i].number.value));
}
String NumberProperty::getElementTarget( size_type i ) const {
    CHECK_INDEX_THROWS( m_property->count );
    return m_property->items[i].number.target;
}
String NumberProperty::getElementMinValue( size_type i ) const {
    CHECK_INDEX_THROWS( m_property->count );
    return m_property->items[i].number.min;
}
String NumberProperty::getElementMaxValue( size_type i ) const {
    CHECK_INDEX_THROWS( m_property->count );
    return m_property->items[i].number.max;
}
String NumberProperty::getElementValueStep( size_type i ) const {
    CHECK_INDEX_THROWS( m_property->count );
    return m_property->items[i].number.step;
}

String NumberProperty::getNumberFormat( size_t i ) const
{
   CHECK_INDEX_THROWS( m_property->count );
   return m_property->items[i].number.format;
}


std::unique_ptr<indigo_property> NumberProperty::clone() const {
    std::unique_ptr<indigo_property> property(indigo_init_number_property(NULL, m_property->device, m_property->name, NULL, NULL, INDIGO_OK_STATE, INDIGO_RW_PERM, m_property->count));
    memcpy(property.get(), m_property, sizeof(indigo_property) + m_property->count * sizeof(indigo_item));
    return property;
}

// ----------------------------------------------------------------------------


String TextProperty::getElementValue( size_t i ) const
{
   CHECK_INDEX_THROWS( m_property->count );
   return m_property->items[i].text.value;
}

std::unique_ptr<indigo_property> TextProperty::clone() const {
    std::unique_ptr<indigo_property> property(indigo_init_text_property(NULL, m_property->device, m_property->name, NULL, NULL, INDIGO_OK_STATE, INDIGO_RW_PERM, m_property->count));
    memcpy(property.get(), m_property, sizeof(indigo_property) + m_property->count * sizeof(indigo_item));
    return property;
}

// ----------------------------------------------------------------------------


String LightProperty::getElementValue( size_t i ) const
{
   CHECK_INDEX_THROWS( m_property->count );
   switch ( m_property->items[i].light.value)
   {
   case INDIGO_IDLE_STATE:
      return "IDLE";
   case INDIGO_OK_STATE:
      return "OK";
   case INDIGO_BUSY_STATE:
      return "BUSY";
   case INDIGO_ALERT_STATE:
      return "ALERT";
   default:
      return "UNSUPPORTED";
   }
}

std::unique_ptr<indigo_property> LightProperty::clone() const {
    std::unique_ptr<indigo_property> property(indigo_init_light_property(NULL, m_property->device, m_property->name, NULL, NULL, INDIGO_OK_STATE, m_property->count));
    memcpy(property.get(), m_property, sizeof(indigo_property) + m_property->count * sizeof(indigo_item));
    return property;
}

// ----------------------------------------------------------------------------

void*   BlobProperty::getBlob( size_type i ) const {
   CHECK_INDEX_THROWS( m_property->count );
   return m_property->items[i].blob.value;
}
size_type  BlobProperty::getBlobSize(size_type i) const {
   CHECK_INDEX_THROWS( m_property->count );
   return m_property->items[i].blob.size;
}
String BlobProperty::getBlobFormat(size_type i) const {
   CHECK_INDEX_THROWS( m_property->count );
   return m_property->items[i].blob.format;
}
String BlobProperty::getUrl(size_type i) const {
   CHECK_INDEX_THROWS( m_property->count );
   return m_property->items[i].blob.url;
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


IProperty* PropertyFactory::Create( indigo_property* property ) {
   switch( property->type )
   {
   case INDIGO_NUMBER_VECTOR:
      return new NumberProperty( property );
   case INDIGO_TEXT_VECTOR:
      return new TextProperty( property );
   case INDIGO_SWITCH_VECTOR:
      return new SwitchProperty( property );
   case INDIGO_LIGHT_VECTOR:
      return new LightProperty( property );
   case INDIGO_BLOB_VECTOR:
      return new BlobProperty( property );
   default:
      return new IProperty( property );
   }
}



} // pcl

// ----------------------------------------------------------------------------
// EOF IINDIProperty.cpp - Released 2018-11-23T18:45:59Z
