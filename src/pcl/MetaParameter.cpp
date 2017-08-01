//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/MetaParameter.cpp - Released 2017-08-01T14:23:38Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/ErrorHandler.h>
#include <pcl/MetaParameter.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessImplementation.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MetaParameter::MetaParameter( MetaProcess* p ) : MetaObject( p )
{
   if ( p == nullptr )
      throw Error( "MetaParameter: Illegal null MetaProcess parent pointer" );
}

// ----------------------------------------------------------------------------

MetaParameter::MetaParameter( MetaTable* t ) : MetaObject( t )
{
   if ( t == nullptr )
      throw Error( "MetaParameter: Illegal null MetaTable parent pointer" );
}

// ----------------------------------------------------------------------------

const MetaParameter* MetaTable::operator[]( size_type i ) const
{
   const MetaParameter* p = dynamic_cast<const MetaParameter*>( m_children[i] );
   if ( p == nullptr )
      throw Error( "MetaTable: Invalid non-parameter table column instance" );
   return p;
}

// ----------------------------------------------------------------------------

MetaTable::MetaTable( MetaTable* t ) : MetaVariableLengthParameter( t )
{
   throw Error( "MetaTable: Nested tables not allowed" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool MetaParameter::inTableDefinition = false;

// ----------------------------------------------------------------------------
// Process Context
// ----------------------------------------------------------------------------

class ParameterContextDispatcher
{
public:

   static void* api_func LockParameter( process_handle hp, meta_parameter_handle hr, size_type trow )
   {
      try
      {
         return reinterpret_cast<ProcessImplementation*>( hp )->LockParameter( reinterpret_cast<const pcl::MetaParameter*>( hr ), trow );
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static void api_func UnlockParameter( process_handle hp, meta_parameter_handle hr, size_type trow )
   {
      try
      {
         reinterpret_cast<ProcessImplementation*>( hp )->UnlockParameter( reinterpret_cast<const pcl::MetaParameter*>( hr ), trow );
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateParameter( void* p, const_process_handle hp, meta_parameter_handle hr, size_type trow )
   {
      try
      {
         return (api_bool)reinterpret_cast<const ProcessImplementation*>( hp )->ValidateParameter( p, reinterpret_cast<const pcl::MetaParameter*>( hr ), trow );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func AllocateParameter( size_type n, process_handle hp, meta_parameter_handle hr, size_type trow )
   {
      try
      {
         return (api_bool)reinterpret_cast<ProcessImplementation*>( hp )->AllocateParameter( n, reinterpret_cast<const pcl::MetaParameter*>( hr ), trow );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static size_type api_func QueryParameterLength( const_process_handle hp, meta_parameter_handle hr, size_type trow )
   {
      try
      {
         return reinterpret_cast<const ProcessImplementation*>( hp )->ParameterLength( reinterpret_cast<const pcl::MetaParameter*>( hr ), trow );
      }
      ERROR_HANDLER
      return 0;
   }
}; // ParameterContextDispatcher

// ----------------------------------------------------------------------------

void MetaParameter::PerformAPIDefinitions() const
{
   {
      IsoString id = Id();
      (*(inTableDefinition ?
         API->ProcessDefinition->BeginTableColumnDefinition :
         API->ProcessDefinition->BeginParameterDefinition))( this, id.c_str(), APIParType() );
   }

   (*API->ProcessDefinition->SetParameterProcessVersionRange)( FirstProcessVersion(), LastProcessVersion() );

   (*API->ProcessDefinition->SetParameterRequired)( IsRequired() );
   (*API->ProcessDefinition->SetParameterReadOnly)( IsReadOnly() );

   {
      IsoString aliases = Aliases().Trimmed();
      if ( !aliases.IsEmpty() )
         (*API->ProcessDefinition->SetParameterAliasIdentifiers)( aliases.c_str() );
   }

   {
      String desc = Description();
      if ( !desc.IsEmpty() )
         (*API->ProcessDefinition->SetParameterDescription)( desc.c_str() );
   }

   {
      String cmnt = Comment();
      if ( !cmnt.IsEmpty() )
         (*API->ProcessDefinition->SetParameterScriptComment)( cmnt.c_str() );
   }

   (*API->ProcessDefinition->SetParameterLockRoutine)( ParameterContextDispatcher::LockParameter );

   if ( NeedsUnlocking() )
      (*API->ProcessDefinition->SetParameterUnlockRoutine)( ParameterContextDispatcher::UnlockParameter );

   if ( NeedsValidation() )
      (*API->ProcessDefinition->SetParameterValidationRoutine)( ParameterContextDispatcher::ValidateParameter );

   if ( IsVariableLength() )
   {
      (*API->ProcessDefinition->SetParameterAllocationRoutine)( ParameterContextDispatcher::AllocateParameter );
      (*API->ProcessDefinition->SetParameterLengthQueryRoutine)( ParameterContextDispatcher::QueryParameterLength );
   }

   PerformTypeAPIDefinitions();

   (*(inTableDefinition ? API->ProcessDefinition->EndTableColumnDefinition :
                          API->ProcessDefinition->EndParameterDefinition))();
}

// ----------------------------------------------------------------------------

void MetaNumeric::PerformTypeAPIDefinitions() const
{
   (*API->ProcessDefinition->SetDefaultNumericValue)( DefaultValue() );

   double a = MinimumValue();
   double b = MaximumValue();
   if ( a <= b && (a != -DBL_MAX || b != +DBL_MAX) )
      (*API->ProcessDefinition->SetValidNumericRange)( a, b );
}

// ----------------------------------------------------------------------------

uint32 MetaUInt8::APIParType() const
{
   return PTYPE_UINT8;
}

uint32 MetaUInt16::APIParType() const
{
   return PTYPE_UINT16;
}

uint32 MetaUInt32::APIParType() const
{
   return PTYPE_UINT32;
}

uint32 MetaUInt64::APIParType() const
{
   return PTYPE_UINT64;
}

uint32 MetaInt8::APIParType() const
{
   return PTYPE_INT8;
}

uint32 MetaInt16::APIParType() const
{
   return PTYPE_INT16;
}

uint32 MetaInt32::APIParType() const
{
   return PTYPE_INT32;
}

uint32 MetaInt64::APIParType() const
{
   return PTYPE_INT64;
}

// ----------------------------------------------------------------------------

void MetaReal::PerformTypeAPIDefinitions() const
{
   MetaNumeric::PerformTypeAPIDefinitions();
   (*API->ProcessDefinition->SetPrecision)( this->Precision() );
   (*API->ProcessDefinition->SetScientificNotation)( this->ScientificNotation() );
}

// ----------------------------------------------------------------------------

uint32 MetaFloat::APIParType() const
{
   return PTYPE_FLOAT;
}

uint32 MetaDouble::APIParType() const
{
   return PTYPE_DOUBLE;
}

// ----------------------------------------------------------------------------

void MetaBoolean::PerformTypeAPIDefinitions() const
{
   (*API->ProcessDefinition->SetDefaultBooleanValue)( this->DefaultValue() );
}

// ----------------------------------------------------------------------------

uint32 MetaBoolean::APIParType() const
{
   return PTYPE_BOOL;
}

// ----------------------------------------------------------------------------

void MetaEnumeration::PerformTypeAPIDefinitions() const
{
   for ( size_type n = NumberOfElements(), i = 0; i < n; ++i )
   {
      IsoString id = ElementId( i );
      (*API->ProcessDefinition->DefineEnumerationElement)( id.c_str(), ElementValue( i ) );
   }

   (*API->ProcessDefinition->SetDefaultEnumerationValueIndex)( uint32( DefaultValueIndex() ) );

   IsoString aliases = ElementAliases().Trimmed();
   if ( !aliases.IsEmpty() )
   {
      IsoStringList items;
      aliases.Break( items, ',', true/*trimElements*/ );
      for ( const IsoString& item : items )
      {
         size_type eq = item.Find( '=' );
         if ( eq != IsoString::notFound )
         {
            IsoString aliasId = item.Left( eq ).Trimmed();
            IsoString actualId = item.Substring( eq+1 ).Trimmed();
            if ( !aliasId.IsEmpty() && !actualId.IsEmpty() )
               (*API->ProcessDefinition->DefineEnumerationAlias)( aliasId.c_str(), actualId.c_str() );
         }
      }
   }
}

// ----------------------------------------------------------------------------

uint32 MetaEnumeration::APIParType() const
{
   return PTYPE_ENUM;
}

// ----------------------------------------------------------------------------

void MetaString::PerformTypeAPIDefinitions() const
{
   {
      String defv = DefaultValue();
      if ( !defv.IsEmpty() )
         (*API->ProcessDefinition->SetDefaultStringValue)( defv.c_str() );
   }

   {
      String allwd = AllowedCharacters();
      if ( !allwd.IsEmpty() )
         (*API->ProcessDefinition->SetStringAllowedCharacters)( allwd.c_str() );
   }

   size_type minl = MinLength();
   size_type maxl = MaxLength();
   if ( minl != 0 || maxl != 0 )
      (*API->ProcessDefinition->SetStringLengthLimits)( minl, maxl );
}

// ----------------------------------------------------------------------------

uint32 MetaString::APIParType() const
{
   return PTYPE_STRING;
}

// ----------------------------------------------------------------------------

void MetaTable::PerformTypeAPIDefinitions() const
{
   inTableDefinition = true;

   for ( size_type i = 0; i < Length(); ++i )
      (*this)[i]->PerformAPIDefinitions();

   inTableDefinition = false;

   size_type minl = MinLength();
   size_type maxl = MaxLength();
   if ( minl != 0 || maxl != 0 )
      (*API->ProcessDefinition->SetTableRowLimits)( minl, maxl );
}

// ----------------------------------------------------------------------------

uint32 MetaTable::APIParType() const
{
   return PTYPE_TABLE;
}

// ----------------------------------------------------------------------------

void MetaBlock::PerformTypeAPIDefinitions() const
{
   size_type minl = MinLength();
   size_type maxl = MaxLength();
   if ( minl != 0 || maxl != 0 )
      (*API->ProcessDefinition->SetBlockSizeLimits)( minl, maxl );
}

// ----------------------------------------------------------------------------

uint32 MetaBlock::APIParType() const
{
   uint32 dataInterpretation;

   switch ( DataInterpretation() )
   {
   default:
   case DataAsUInt8:  dataInterpretation = PTYPE_BLOCK_UI8   ; break;
   case DataAsInt8:   dataInterpretation = PTYPE_BLOCK_I8    ; break;
   case DataAsUInt16: dataInterpretation = PTYPE_BLOCK_UI16  ; break;
   case DataAsInt16:  dataInterpretation = PTYPE_BLOCK_I16   ; break;
   case DataAsUInt32: dataInterpretation = PTYPE_BLOCK_UI32  ; break;
   case DataAsInt32:  dataInterpretation = PTYPE_BLOCK_I32   ; break;
   case DataAsUInt64: dataInterpretation = PTYPE_BLOCK_UI64  ; break;
   case DataAsInt64:  dataInterpretation = PTYPE_BLOCK_I64   ; break;
   case DataAsFloat:  dataInterpretation = PTYPE_BLOCK_FLOAT ; break;
   case DataAsDouble: dataInterpretation = PTYPE_BLOCK_DOUBLE; break;
   }

   return PTYPE_BLOCK | dataInterpretation;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MetaParameter.cpp - Released 2017-08-01T14:23:38Z
