//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/ProcessParameter.cpp - Released 2015/12/17 18:52:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/AutoLock.h>
#include <pcl/Process.h>
#include <pcl/ProcessParameter.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ProcessParameterPrivate
{
public:

   meta_parameter_handle handle;
   mutable AutoPointer<Process>  process;

   ProcessParameterPrivate() :
      handle( nullptr )
   {
   }

   ProcessParameterPrivate( meta_parameter_handle hParam ) :
      handle( nullptr )
   {
      if ( hParam != nullptr )
      {
         meta_process_handle hProcess = (*API->Process->GetParameterProcess)( hParam );
         if ( hProcess == nullptr )
            throw Error( "ProcessParameter: Internal error: Invalid process handle" );

         handle = hParam;
         process = new Process( hProcess );
      }
   }

   ~ProcessParameterPrivate()
   {
      handle = nullptr;
   }
};

// ----------------------------------------------------------------------------

api_bool InternalParameterEnumerator::ParameterCallback( meta_parameter_handle hParam, void* data )
{
   reinterpret_cast<Process::parameter_list*>( data )->Add( ProcessParameter( hParam ) );
   return api_true;
}

// ----------------------------------------------------------------------------

ProcessParameter::ProcessParameter( const Process& process, const IsoString& paramId )
{
   m_data = new ProcessParameterPrivate( (*API->Process->GetParameterByName)( process.Handle(), paramId.c_str() ) );
   if ( m_data->handle == nullptr )
   {
      if ( paramId.IsEmpty() )
         throw Error( "ProcessParameter: Empty process parameter identifier specified" );
      if ( !paramId.IsValidIdentifier() )
         throw Error( "ProcessParameter: Invalid process parameter identifier specified: \'" + paramId + '\'' );

      throw Error( "ProcessParameter: No parameter was found "
                   "with the specified identifier \'" + paramId + "\' for process \'" + process.Id() + '\'' );
   }
}

ProcessParameter::ProcessParameter( const ProcessParameter& table, const IsoString& colId )
{
   m_data = new ProcessParameterPrivate( (*API->Process->GetTableColumnByName)( table.m_data->handle, colId.c_str() ) );
   if ( m_data->handle == nullptr )
   {
      if ( table.IsNull() )
         throw Error( "ProcessParameter: Null table parameter" );
      if ( colId.IsEmpty() )
         throw Error( "ProcessParameter: Empty table column parameter identifier specified" );
      if ( !colId.IsValidIdentifier() )
         throw Error( "ProcessParameter: Invalid table column parameter identifier specified: \'" + colId + '\'' );
      if ( !table.IsTable() )
         throw Error( "ProcessParameter: The specified parameter \'" + table.Id() + "\' is not a table parameter" );

      throw Error( "ProcessParameter: No table column parameter was found "
                   "with the specified identifier \'" + colId + "\' "
                   "for table parameter \'" + table.Id() + "\' "
                   "of process \'" + table.ParentProcess().Id() + '\'' );
   }
}

ProcessParameter::ProcessParameter( const ProcessParameter& p )
{
   m_data = new ProcessParameterPrivate( p.m_data->handle );
}

ProcessParameter::ProcessParameter( const void* hParam )
{
   m_data = new ProcessParameterPrivate( hParam );
}

ProcessParameter::~ProcessParameter()
{
}

// ----------------------------------------------------------------------------

bool ProcessParameter::IsNull() const
{
   return m_data->handle == nullptr;
}

ProcessParameter& ProcessParameter::Null()
{
   static ProcessParameter* nullParameter = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullParameter == nullptr )
      nullParameter = new ProcessParameter( (const void*)0 );
   return *nullParameter;
}

// ----------------------------------------------------------------------------

Process& ProcessParameter::ParentProcess() const
{
   if ( m_data->process.IsNull() )
      throw Error( "ProcessParameter::ParentProcess(): Invoked for a null ProcessParameter object" );
   return *m_data->process;
}

ProcessParameter ProcessParameter::ParentTable() const
{
   return ProcessParameter( (*API->Process->GetParameterTable)( m_data->handle ) );
}

// ----------------------------------------------------------------------------

IsoString ProcessParameter::Id() const
{
   size_type len = 0;
   (*API->Process->GetParameterIdentifier)( m_data->handle, 0, &len );

   IsoString id;
   if ( len > 0 )
   {
      id.SetLength( len );
      if ( (*API->Process->GetParameterIdentifier)( m_data->handle, id.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterIdentifier" );
      id.ResizeToNullTerminated();
   }
   return id;
}

// ----------------------------------------------------------------------------

IsoStringList ProcessParameter::Aliases() const
{
   size_type len = 0;
   (*API->Process->GetParameterAliasIdentifiers)( m_data->handle, 0, &len );

   IsoStringList aliases;
   if ( len > 0 )
   {
      IsoString csAliases;
      csAliases.SetLength( len );
      if ( (*API->Process->GetParameterAliasIdentifiers)( m_data->handle, csAliases.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterAliasIdentifiers" );
      csAliases.ResizeToNullTerminated();
      csAliases.Break( aliases, ',' );
   }
   return aliases;
}

// ----------------------------------------------------------------------------

bool ProcessParameter::IsRequired() const
{
   return (*API->Process->GetParameterRequired)( m_data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ProcessParameter::IsReadOnly() const
{
   return (*API->Process->GetParameterReadOnly)( m_data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

String ProcessParameter::Description() const
{
   size_type len = 0;
   (*API->Process->GetParameterDescription)( m_data->handle, 0, &len );

   String description;
   if ( len > 0 )
   {
      description.SetLength( len );
      if ( (*API->Process->GetParameterDescription)( m_data->handle, description.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterDescription" );
      description.ResizeToNullTerminated();
   }
   return description;
}

// ----------------------------------------------------------------------------

String ProcessParameter::ScriptComment() const
{
   size_type len = 0;
   (*API->Process->GetParameterScriptComment)( m_data->handle, 0, &len );

   String comment;
   if ( len > 0 )
   {
      comment.SetLength( len );
      if ( (*API->Process->GetParameterScriptComment)( m_data->handle, comment.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterScriptComment" );
      comment.ResizeToNullTerminated();
   }
   return comment;
}

// ----------------------------------------------------------------------------

ProcessParameter::data_type ProcessParameter::Type() const
{
   if ( IsNull() )
      return ProcessParameterType::Invalid;

   uint32 apiType = (*API->Process->GetParameterType)( m_data->handle );
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterType" );

   switch ( apiType & PTYPE_TYPE_MASK )
   {
   case PTYPE_UINT8:  return ProcessParameterType::UInt8;
   case PTYPE_INT8:   return ProcessParameterType::Int8;
   case PTYPE_UINT16: return ProcessParameterType::UInt16;
   case PTYPE_INT16:  return ProcessParameterType::Int16;
   case PTYPE_UINT32: return ProcessParameterType::UInt32;
   case PTYPE_INT32:  return ProcessParameterType::Int32;
   case PTYPE_UINT64: return ProcessParameterType::UInt64;
   case PTYPE_INT64:  return ProcessParameterType::Int64;
   case PTYPE_FLOAT:  return ProcessParameterType::Float;
   case PTYPE_DOUBLE: return ProcessParameterType::Double;
   case PTYPE_BOOL:   return ProcessParameterType::Boolean;
   case PTYPE_ENUM:   return ProcessParameterType::Enumeration;
   case PTYPE_STRING: return ProcessParameterType::String;
   case PTYPE_TABLE:  return ProcessParameterType::Table;
   case PTYPE_BLOCK:  return ProcessParameterType::Block;
   default:           throw Error( "ProcessParameter::Type(): Internal error: Unknown parameter type" );
   }
}

ProcessParameter::data_type ProcessParameter::DataInterpretation() const
{
   if ( !IsBlock() )
      return ProcessParameterType::Invalid;

   uint32 apiType = (*API->Process->GetParameterType)( m_data->handle );
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterType" );

   switch ( apiType & PTYPE_BLOCK_INTERPRETATION )
   {
   case PTYPE_BLOCK_UI8:    return ProcessParameterType::UInt8;
   case PTYPE_BLOCK_I8:     return ProcessParameterType::Int8;
   case PTYPE_BLOCK_UI16:   return ProcessParameterType::UInt16;
   case PTYPE_BLOCK_I16:    return ProcessParameterType::Int16;
   case PTYPE_BLOCK_UI32:   return ProcessParameterType::UInt32;
   case PTYPE_BLOCK_I32:    return ProcessParameterType::Int32;
   case PTYPE_BLOCK_UI64:   return ProcessParameterType::UInt64;
   case PTYPE_BLOCK_I64:    return ProcessParameterType::Int64;
   case PTYPE_BLOCK_FLOAT:  return ProcessParameterType::Float;
   case PTYPE_BLOCK_DOUBLE: return ProcessParameterType::Double;
   default:                 throw Error( "ProcessParameter::DataInterpretation(): Internal error: Unknown parameter type" );
   }
}

// ----------------------------------------------------------------------------

Variant ProcessParameter::DefaultValue() const
{
   if ( IsNull() )
      return Variant();

   uint32 apiType = (*API->Process->GetParameterType)( m_data->handle );
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterType" );

   apiType &= PTYPE_TYPE_MASK;

   if ( apiType == PTYPE_TABLE )
      throw Error( "ProcessParameter::DefaultValue(): Invoked for a table parameter" );

   if ( apiType == PTYPE_BLOCK )
      return ByteArray();

   if ( apiType == PTYPE_STRING )
   {
      size_type len = 0;
      (*API->Process->GetParameterDefaultValue)( m_data->handle, 0, &len );

      String value;
      if ( len > 0 )
      {
         value.SetLength( len );
         if ( (*API->Process->GetParameterDefaultValue)( m_data->handle, value.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetParameterDefaultValue" );
         value.ResizeToNullTerminated();
      }
      return value;
   }

   if ( apiType == PTYPE_ENUM )
   {
      int index = (*API->Process->GetParameterDefaultElementIndex)( m_data->handle );
      if ( index < 0 )
         throw APIFunctionError( "GetParameterDefaultElementIndex" );
      return index;
   }

   union
   {
      uint8    u8;
      int8     i8;
      uint16   u16;
      int16    i16;
      uint32   u32;
      int32    i32;
      uint64   u64;
      int64    i64;
      float    f;
      double   d;
      api_bool b;
   }
   value;

   if ( (*API->Process->GetParameterDefaultValue)( m_data->handle, &value, 0 ) == api_false )
      throw APIFunctionError( "GetParameterDefaultValue" );

   switch ( apiType )
   {
   case PTYPE_UINT8:  return value.u8;
   case PTYPE_INT8:   return value.i8;
   case PTYPE_UINT16: return value.u16;
   case PTYPE_INT16:  return value.i16;
   case PTYPE_UINT32: return value.u32;
   case PTYPE_INT32:  return value.i32;
   case PTYPE_UINT64: return value.u64;
   case PTYPE_INT64:  return value.i64;
   case PTYPE_FLOAT:  return value.f;
   case PTYPE_DOUBLE: return value.d;
   case PTYPE_BOOL:   return value.b != api_false;
   default:
      throw Error( "ProcessParameter::DefaultValue(): Internal error: Unknown parameter type" );
   }
}

// ----------------------------------------------------------------------------

void ProcessParameter::GetNumericRange( double& minValue, double& maxValue ) const
{
   if ( !IsNumeric() )
   {
      minValue = maxValue = 0;
      return;
   }

   if ( (*API->Process->GetParameterRange)( m_data->handle, &minValue, &maxValue ) == api_false )
      throw APIFunctionError( "GetParameterRange" );
}

// ----------------------------------------------------------------------------

int ProcessParameter::Precision() const
{
   if ( !IsReal() )
      return 0;
   return (*API->Process->GetParameterPrecision)( m_data->handle );
}

// ----------------------------------------------------------------------------

bool ProcessParameter::ScientificNotation() const
{
   if ( !IsReal() )
      return false;
   return (*API->Process->GetParameterScientificNotation)( m_data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ProcessParameter::GetLengthLimits( size_type& minLength, size_type& maxLength ) const
{
   if ( !IsVariableLength() )
   {
      minLength = maxLength = 0;
      return;
   }

   if ( (*API->Process->GetParameterLengthLimits)( m_data->handle, &minLength, &maxLength ) == api_false )
      throw APIFunctionError( "GetParameterLengthLimits" );
}

// ----------------------------------------------------------------------------

ProcessParameter::enumeration_element_list ProcessParameter::EnumerationElements() const
{
   if ( !IsEnumeration() )
      return enumeration_element_list();

   size_type count = (*API->Process->GetParameterElementCount)( m_data->handle );
   if ( count == 0 )
      throw APIFunctionError( "GetParameterElementCount" );

   enumeration_element_list elements( count );

   for ( size_type i = 0; i < count; ++i )
   {
      size_type len = 0;
      (*API->Process->GetParameterElementIdentifier)( m_data->handle, i, 0, &len );
      if ( len == 0 )
         throw APIFunctionError( "GetParameterElementIdentifier" );
      elements[i].id.SetLength( len );
      if ( (*API->Process->GetParameterElementIdentifier)( m_data->handle, i, elements[i].id.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterElementIdentifier" );
      elements[i].id.ResizeToNullTerminated();

      len = 0;
      (*API->Process->GetParameterElementAliasIdentifiers)( m_data->handle, i, 0, &len );
      if ( len > 0 )
      {
         IsoString aliases;
         aliases.SetLength( len );
         if ( (*API->Process->GetParameterElementAliasIdentifiers)( m_data->handle, i, aliases.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetParameterElementAliasIdentifiers" );
         aliases.ResizeToNullTerminated();
         aliases.Break( elements[i].aliases, ',' );
      }

      elements[i].value = (*API->Process->GetParameterElementValue)( m_data->handle, i );
   }

   return elements;
}

// ----------------------------------------------------------------------------

String ProcessParameter::AllowedCharacters() const
{
   String allowed;

   if ( IsString() )
   {
      size_type len = 0;
      (*API->Process->GetParameterAllowedCharacters)( m_data->handle, 0, &len );

      if ( len > 0 )
      {
         allowed.SetLength( len );
         if ( (*API->Process->GetParameterAllowedCharacters)( m_data->handle, allowed.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetParameterAllowedCharacters" );
         allowed.ResizeToNullTerminated();
      }
   }

   return allowed;
}

// ----------------------------------------------------------------------------

ProcessParameter::parameter_list ProcessParameter::TableColumns() const
{
   parameter_list parameters;
   if ( IsTable() )
      if ( (*API->Process->EnumerateTableColumns)( m_data->handle,
                                    InternalParameterEnumerator::ParameterCallback, &parameters ) == api_false )
         throw APIFunctionError( "EnumerateTableColumns" );
   return parameters;
}

// ----------------------------------------------------------------------------

const void* ProcessParameter::Handle() const
{
   return m_data->handle;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProcessParameter.cpp - Released 2015/12/17 18:52:18 UTC
