//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/ProcessInstance.cpp - Released 2017-06-28T11:58:42Z
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

#include <pcl/AutoLock.h>
#include <pcl/ImageInfo.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/ProcessInstance.h>
#include <pcl/View.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#define SWAPFILE_DISABLE_BIT  0x00008000u

#define WHYNOT_MAXLENGTH      512

namespace pcl
{

// ----------------------------------------------------------------------------

static void APIHackingAttempt( const String& routineId )
{
   throw Error( "* Warning * Hacking attempt detected in low-level API call: Process->" + routineId );
}

// ----------------------------------------------------------------------------

ProcessInstance::ProcessInstance( const Process& process ) :
   UIObject( (*API->Process->CreateProcessInstance)( ModuleHandle(), process.Handle() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateProcessInstance" );
}

// ----------------------------------------------------------------------------

ProcessInstance& ProcessInstance::Null()
{
   static ProcessInstance* nullInstance = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullInstance == nullptr )
      nullInstance = new ProcessInstance( reinterpret_cast<void*>( 0 ) );
   return *nullInstance;
}

// ----------------------------------------------------------------------------

Process ProcessInstance::ParentProcess() const
{
   return Process( (const void*)(*API->Process->GetProcessInstanceProcess)( handle ) );
}

// ----------------------------------------------------------------------------

ProcessInstance ProcessInstance::Clone() const
{
   return ProcessInstance( (const void*)(*API->Process->CloneProcessInstance)( ModuleHandle(), handle, 0/*flags*/ ) );
}

// ----------------------------------------------------------------------------

bool ProcessInstance::Assign( const ProcessInstance& p )
{
   return (*API->Process->AssignProcessInstance)( handle, p.handle, 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

bool ProcessInstance::Validate( String& whyNot )
{
   whyNot.Reserve( WHYNOT_MAXLENGTH );
   bool result = (*API->Process->ValidateProcessInstance)( handle, whyNot.Begin(), WHYNOT_MAXLENGTH ) != api_false;
   whyNot.ResizeToNullTerminated();
   return result;
}

// ----------------------------------------------------------------------------

bool ProcessInstance::IsHistoryUpdater( const View& view ) const
{
   return (*API->Process->GetUpdatesViewHistory)( handle, view.handle ) != api_false;
}

bool ProcessInstance::IsMaskable( const View& view, const ImageWindow& mask ) const
{
   return (*API->Process->ValidateViewExecutionMask)( handle, view.handle, mask.handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ProcessInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot.Reserve( WHYNOT_MAXLENGTH );
   bool result = (*API->Process->ValidateViewExecution)( handle, view.handle, whyNot.Begin(), WHYNOT_MAXLENGTH ) != api_false;
   whyNot.ResizeToNullTerminated();
   return result;
}

bool ProcessInstance::ExecuteOn( View& view, bool swapFile )
{
   return (*API->Process->ExecuteOnView)( handle, view.handle, swapFile ? 0u : SWAPFILE_DISABLE_BIT ) != api_false;
}

// ----------------------------------------------------------------------------

bool ProcessInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot.Reserve( WHYNOT_MAXLENGTH );
   bool result = (*API->Process->ValidateGlobalExecution)( handle, whyNot.Begin(), WHYNOT_MAXLENGTH ) != api_false;
   whyNot.ResizeToNullTerminated();
   return result;
}

bool ProcessInstance::ExecuteGlobal()
{
   return (*API->Process->ExecuteGlobal)( handle, 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

bool ProcessInstance::CanExecuteOn( const ImageVariant& image, String& whyNot ) const
{
   if ( !image.IsSharedImage() || !image.IsCompletelySelected() )
   {
      ImageVariant tmp;
      tmp.CreateSharedImageAs( image ).AssignImage( image );
      return CanExecuteOn( tmp, whyNot );
   }

   image.PushSelections();
   image.ResetSelections();

   ImageInfo info1( *image );

   image.PopSelections();

   whyNot.Reserve( WHYNOT_MAXLENGTH );
   bool ok = (*API->Process->ValidateImageExecution)( handle, image.SharedImageHandle(), whyNot.Begin(), WHYNOT_MAXLENGTH ) != api_false;
   whyNot.ResizeToNullTerminated();

   image.PushSelections();
   image.ResetSelections();
   ImageInfo info2( *image );
   image.PopSelections();
   if ( info1 != info2 )
      APIHackingAttempt( "ValidateImageExecution" );

   return ok;
}

bool ProcessInstance::ExecuteOn( ImageVariant& image, const IsoString& hints )
{
   if ( !image.IsSharedImage() || !image.IsCompletelySelected() )
   {
      ImageVariant tmp;
      tmp.CreateSharedImageAs( image ).AssignImage( image );
      if ( !ExecuteOn( tmp ) )
         return false;
      if ( image.IsCompletelySelected() )
         image.AssignImage( tmp );
      else
         image.Apply( tmp, ImageOp::Mov, image.SelectedRectangle().LeftTop() );
      return true;
   }

   return (*API->Process->ExecuteOnImage)( handle, image.SharedImageHandle(), hints.c_str(), 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

bool ProcessInstance::Launch()
{
   return (*API->Process->LaunchProcessInstance)( handle ) != api_false;
}

bool ProcessInstance::CanLaunchInterface() const
{
   return (*API->Process->ValidateInterfaceLaunch)( handle ) != api_false;
}


bool ProcessInstance::LaunchInterface()
{
   return (*API->Process->LaunchInterface)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String ProcessInstance::Description() const
{
   size_type len = 0;
   (*API->Process->GetProcessInstanceDescription)( handle, 0, &len );

   String description;
   if ( len > 0 )
   {
      description.SetLength( len );
      if ( (*API->Process->GetProcessInstanceDescription)( handle, description.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetProcessInstanceDescription" );
      description.ResizeToNullTerminated();
   }
   return description;
}

void ProcessInstance::SetDescription( const String& text )
{
   (void)(*API->Process->SetProcessInstanceDescription)( handle, text.c_str() );
}


// ----------------------------------------------------------------------------

void ProcessInstance::GetExecutionTimes( double& startJD, double& elapsedSecs ) const
{
   (void)(*API->Process->GetExecutionTimes)( handle, &startJD, &elapsedSecs );
}

// ----------------------------------------------------------------------------

String ProcessInstance::ToSource( const IsoString& language, const IsoString& varId, int indent ) const
{
   char16_type* s = (*API->Process->GetProcessInstanceSourceCode)(
                              ModuleHandle(), handle, language.c_str(), varId.c_str(), indent );
   if ( s == 0 )
      throw APIFunctionError( "GetProcessInstanceSourceCode" );
   String source( s );
   Module->Deallocate( s );
   return source;
}

ProcessInstance ProcessInstance::FromSource( const String& source, const IsoString& language )
{
   return ProcessInstance( (*API->Process->CreateProcessInstanceFromSourceCode)( source.c_str(), language.c_str() ) );
}

// ----------------------------------------------------------------------------

ProcessInstance ProcessInstance::FromIcon( const IsoString& iconId )
{
   return ProcessInstance( (*API->Process->CreateProcessInstanceFromIcon)( ModuleHandle(), iconId.c_str() ) );
}

// ----------------------------------------------------------------------------

api_bool InternalIconEnumerator::IconCallback( const char* iconId, void* data )
{
   reinterpret_cast<IsoStringList*>( data )->Add( IsoString( iconId ) );
   return api_true;
}

IsoStringList ProcessInstance::Icons()
{
   size_type len = 0;
   (*API->Process->EnumerateProcessIcons)( 0, 0, &len, 0 );

   IsoStringList icons;
   if ( len > 0 )
   {
      IsoString iconId;
      iconId.Reserve( len );
      if ( (*API->Process->EnumerateProcessIcons)( InternalIconEnumerator::IconCallback,
                                          iconId.Begin(), &len, &icons ) == api_false )
         throw APIFunctionError( "EnumerateProcessIcons" );
   }
   return icons;
}

// ----------------------------------------------------------------------------

struct ProcessIconsByProcessIdEnumerationData
{
   IsoString     processId;
   IsoStringList icons;

   ProcessIconsByProcessIdEnumerationData( const IsoString& id ) : processId( id )
   {
   }
};

api_bool InternalIconEnumerator::ProcessCallback( const char* iconId, void* data )
{
#define enumeration reinterpret_cast<ProcessIconsByProcessIdEnumerationData*>( data )
   ProcessInstance instance( (*API->Process->CreateProcessInstanceFromIcon)( ModuleHandle(), iconId ) );
   if ( instance.IsNull() ) // ?! should not happen
      return api_false;
   if ( instance.ParentProcess().Id() == enumeration->processId )
      enumeration->icons.Add( IsoString( iconId ) );
   return api_true;
#undef enumeration
}

IsoStringList ProcessInstance::IconsByProcessId( const IsoString& processId )
{
   size_type len = 0;
   (*API->Process->EnumerateProcessIcons)( 0, 0, &len, 0 );

   ProcessIconsByProcessIdEnumerationData data( processId );
   if ( len > 0 )
   {
      IsoString iconId;
      iconId.Reserve( len );
      if ( (*API->Process->EnumerateProcessIcons)( InternalIconEnumerator::ProcessCallback,
                                          iconId.Begin(), &len, &data ) == api_false )
         throw APIFunctionError( "EnumerateProcessIcons" );
   }
   return data.icons;
}

// ----------------------------------------------------------------------------

Variant ProcessInstance::ParameterValue( const ProcessParameter& parameter, size_type rowIndex ) const
{
   if ( parameter.IsNull() )
      return Variant();

   // First call to get the parameter type and length
   uint32 apiType = 0;
   size_type len = 0;
   (*API->Process->GetParameterValue)( handle, parameter.Handle(), rowIndex, &apiType, 0, &len );
   apiType &= PTYPE_TYPE_MASK;
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterValue" );

   if ( apiType == PTYPE_TABLE )
      throw Error( "ProcessInstance::ParameterValue(): Invoked for a table parameter" );

   if ( apiType == PTYPE_STRING )
   {
      String value;
      if ( len > 0 )
      {
         value.SetLength( len );
         if ( (*API->Process->GetParameterValue)( handle, parameter.Handle(),
                                                  rowIndex, &apiType, value.Begin(), &len ) == api_false )
            throw APIFunctionError( "GetParameterValue" );
         value.ResizeToNullTerminated();
      }
      return value;
   }

   if ( apiType == PTYPE_BLOCK )
   {
      ByteArray value( len );
      if ( len > 0 )
      {
         if ( (*API->Process->GetParameterValue)( handle, parameter.Handle(),
                                                  rowIndex, &apiType, value.Begin(), &len ) == api_false )
            throw APIFunctionError( "GetParameterValue" );
      }
      return value;
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
      api_enum e;
   }
   value;

   if ( (*API->Process->GetParameterValue)( handle, parameter.Handle(),
                                            rowIndex, &apiType, &value, 0 ) == api_false )
      throw APIFunctionError( "GetParameterValue" );

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
   case PTYPE_ENUM:   return int( value.e );
   default:
      throw Error( "ProcessParameter::ParameterValue(): Internal error: Unknown parameter type" );
   }
}

// ----------------------------------------------------------------------------

bool ProcessInstance::SetParameterValue( const Variant& value,
                                         const ProcessParameter& parameter, size_type rowIndex )
{
   if ( !value.IsValid() )
      return false;

   uint32 apiType = (*API->Process->GetParameterType)( parameter.Handle() ) & PTYPE_TYPE_MASK;
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterType" );

   if ( apiType == PTYPE_TABLE )
      throw Error( "ProcessInstance::SetParameterValue(): Invoked for a table parameter" );

   if ( apiType == PTYPE_STRING )
   {
      String s = value.ToString();
      return (*API->Process->SetParameterValue)( handle, parameter.Handle(), rowIndex, s.c_str(), s.Length() ) != api_false;
   }

   if ( apiType == PTYPE_BLOCK )
   {
      ByteArray a = value.ToByteArray();
      return (*API->Process->SetParameterValue)( handle, parameter.Handle(), rowIndex, a.Begin(), a.Length() ) != api_false;
   }

   union
   {
      uint32   u32;
      int32    i32;
      uint64   u64;
      int64    i64;
      float    f;
      double   d;
      api_bool b;
      api_enum e;
   }
   apiValue;

   switch ( apiType )
   {
   case PTYPE_UINT8:
   case PTYPE_UINT16:
   case PTYPE_UINT32:
      apiValue.u32 = value.ToUInt();
      break;
   case PTYPE_INT8:
   case PTYPE_INT16:
   case PTYPE_INT32:
      apiValue.i32 = value.ToInt();
      break;
   case PTYPE_UINT64:
      apiValue.u64 = value.ToUInt64();
      break;
   case PTYPE_INT64:
      apiValue.i64 = value.ToInt64();
      break;
   case PTYPE_FLOAT:
      apiValue.f = value.ToFloat();
      break;
   case PTYPE_DOUBLE:
      apiValue.d = value.ToDouble();
      break;
   case PTYPE_BOOL:
      apiValue.b = api_bool( value.ToBoolean() );
      break;
   case PTYPE_ENUM:
      apiValue.e = api_enum( value.ToInt() );
      break;
   default:
      throw Error( "ProcessParameter::SetParameterValue(): Internal error: Unknown parameter type" );
   }

   return (*API->Process->SetParameterValue)( handle, parameter.Handle(), rowIndex, &apiValue, 1 ) != api_false;
}

// ----------------------------------------------------------------------------

size_type ProcessInstance::TableRowCount( const ProcessParameter& table ) const
{
   return (*API->Process->GetTableRowCount)( handle, table.Handle() );
}

// ----------------------------------------------------------------------------

bool ProcessInstance::AllocateTableRows( const ProcessParameter& table, size_type rowCount )
{
   return (*API->Process->AllocateTableRows)( handle, table.Handle(), rowCount ) != api_false;
}

// ----------------------------------------------------------------------------

void* ProcessInstance::CloneHandle() const
{
   return (*API->Process->CloneProcessInstance)( ModuleHandle(), handle, 0/*flags*/ );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProcessInstance.cpp - Released 2017-06-28T11:58:42Z
