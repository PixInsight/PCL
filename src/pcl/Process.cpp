//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/Process.cpp - Released 2018-11-23T16:14:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Process.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ProcessPrivate
{
public:

   meta_process_handle    handle;
   api_process_properties properties;

   ProcessPrivate() : handle( nullptr )
   {
      GetProperties();
   }

   ProcessPrivate( meta_process_handle h ) : handle( h )
   {
      GetProperties();
   }

   ~ProcessPrivate()
   {
      handle = nullptr;
      GetProperties();
   }

private:

   void GetProperties()
   {
      // Strict aliasing safe code
      static_assert( sizeof( api_process_properties ) <= sizeof( uint64 ), "Invalid sizeof( api_process_properties )" );
      union { api_process_properties properties; uint64 u; } v;
      v.u = 0u;
      properties = v.properties;

      if ( handle != nullptr )
         if ( !(*API->Process->GetProcessProperties)( handle, &properties ) )
            throw APIFunctionError( "GetProcessProperties" );
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Process::Process( const IsoString& id ) :
   ProcessBase()
{
   m_data = new ProcessPrivate( (*API->Process->GetProcessByName)( ModuleHandle(), id.c_str() ) );
   if ( m_data->handle == nullptr )
   {
      if ( id.IsEmpty() )
         throw Error( "Process: Empty process identifier specified" );
      if ( !id.IsValidIdentifier() )
         throw Error( "Process: Invalid process identifier specified: \'" + id + '\'' );
      throw Error( "Process: No installed process was found "
                   "with the specified identifier \'" + id + '\'' );
   }
}

Process::Process( const Process& p ) :
   ProcessBase()
{
   m_data = new ProcessPrivate( p.m_data->handle );
}

Process::Process( const void* handle ) :
   ProcessBase()
{
   m_data = new ProcessPrivate( handle );
   if ( m_data->handle == nullptr )
      throw Error( "Process: Null process handle" );
}

Process::~Process()
{
}

// ----------------------------------------------------------------------------

IsoString Process::Id() const
{
   size_type len = 0;
   (*API->Process->GetProcessIdentifier)( m_data->handle, 0, &len );

   IsoString id;
   if ( len > 0 )
   {
      id.SetLength( len );
      if ( (*API->Process->GetProcessIdentifier)( m_data->handle, id.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetProcessIdentifier" );
      id.ResizeToNullTerminated();
   }
   return id;
}

// ----------------------------------------------------------------------------

IsoStringList Process::Aliases() const
{
   size_type len = 0;
   (*API->Process->GetProcessAliasIdentifiers)( m_data->handle, 0, &len );

   IsoStringList aliases;
   if ( len > 0 )
   {
      IsoString csAliases;
      csAliases.SetLength( len );
      if ( (*API->Process->GetProcessAliasIdentifiers)( m_data->handle, csAliases.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetProcessAliasIdentifiers" );
      csAliases.ResizeToNullTerminated();
      csAliases.Break( aliases, ',' );
   }
   return aliases;
}

// ----------------------------------------------------------------------------

IsoStringList Process::Categories() const
{
   size_type len = 0;
   (*API->Process->GetProcessCategory)( m_data->handle, 0, &len );

   IsoStringList categories;
   if ( len > 0 )
   {
      IsoString csCategories;
      csCategories.SetLength( len );
      if ( (*API->Process->GetProcessCategory)( m_data->handle, csCategories.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetProcessCategory" );
      csCategories.ResizeToNullTerminated();
      csCategories.Break( categories, ',' );
   }
   return categories;
}

// ----------------------------------------------------------------------------

uint32 Process::Version() const
{
   return (*API->Process->GetProcessVersion)( m_data->handle );
}

// ----------------------------------------------------------------------------

String Process::Description() const
{
   size_type len = 0;
   (*API->Process->GetProcessDescription)( m_data->handle, 0, &len );

   String description;
   if ( len > 0 )
   {
      description.SetLength( len );
      if ( (*API->Process->GetProcessDescription)( m_data->handle, description.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetProcessDescription" );
      description.ResizeToNullTerminated();
   }
   return description;
}

// ----------------------------------------------------------------------------

String Process::ScriptComment() const
{
   size_type len = 0;
   (*API->Process->GetProcessScriptComment)( m_data->handle, 0, &len );

   String comment;
   if ( len > 0 )
   {
      comment.SetLength( len );
      if ( (*API->Process->GetProcessScriptComment)( m_data->handle, comment.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetProcessScriptComment" );
      comment.ResizeToNullTerminated();
   }
   return comment;
}

// ----------------------------------------------------------------------------

Bitmap Process::Icon() const
{
   return Bitmap( (*API->Process->GetProcessIcon)( m_data->handle ) );
}

// ----------------------------------------------------------------------------

Bitmap Process::SmallIcon() const
{
   return Bitmap( (*API->Process->GetProcessSmallIcon)( m_data->handle ) );
}

// ----------------------------------------------------------------------------

bool Process::CanProcessViews() const
{
   return m_data->properties.canProcessViews;
}

// ----------------------------------------------------------------------------

bool Process::CanProcessGlobal() const
{
   return m_data->properties.canProcessGlobal;
}

// ----------------------------------------------------------------------------

bool Process::CanProcessImages() const
{
   return m_data->properties.canProcessImages;
}

// ----------------------------------------------------------------------------

bool Process::CanProcessCommandLines() const
{
   return m_data->properties.canProcessCommandLines;
}

// ----------------------------------------------------------------------------

bool Process::CanEditPreferences() const
{
   return m_data->properties.canEditPreferences;
}

// ----------------------------------------------------------------------------

bool Process::CanBrowseDocumentation() const
{
   return m_data->properties.canBrowseDocumentation;
}

// ----------------------------------------------------------------------------

bool Process::IsAssignable() const
{
   return m_data->properties.isAssignable;
}

// ----------------------------------------------------------------------------

bool Process::NeedsInitialization() const
{
   return m_data->properties.needsInitialization;
}

// ----------------------------------------------------------------------------

bool Process::NeedsValidation() const
{
   return m_data->properties.needsValidation;
}

// ----------------------------------------------------------------------------

bool Process::PrefersGlobalExecution() const
{
   return m_data->properties.prefersGlobalExecution;
}

// ----------------------------------------------------------------------------

bool Process::EditPreferences() const
{
   return (*API->Process->EditProcessPreferences)( m_data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool Process::BrowseDocumentation() const
{
   return (*API->Process->BrowseProcessDocumentation)( m_data->handle, 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

int Process::RunCommandLine( const String& arguments ) const
{
   return int( (*API->Process->RunProcessCommandLine)( m_data->handle, arguments.c_str() ) );
}

// ----------------------------------------------------------------------------

bool Process::Launch() const
{
   return (*API->Process->LaunchProcess)( m_data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

Process::parameter_list Process::Parameters() const
{
   parameter_list parameters;
   if ( (*API->Process->EnumerateProcessParameters)( m_data->handle,
                  InternalParameterEnumerator::ParameterCallback, &parameters ) == api_false )
      throw APIFunctionError( "EnumerateProcessParameters" );
   return parameters;
}

// ----------------------------------------------------------------------------

bool Process::HasParameter( const IsoString& paramId ) const
{
   return (*API->Process->GetParameterByName)( m_data->handle, paramId.c_str() ) != 0;
}

bool Process::HasTableColumn( const IsoString& tableId, const IsoString& colId ) const
{
   meta_parameter_handle hTable = (*API->Process->GetParameterByName)( m_data->handle, tableId.c_str() );
   return hTable != 0 && (*API->Process->GetTableColumnByName)( m_data->handle, colId.c_str() ) != 0;
}

// ----------------------------------------------------------------------------

const void* Process::Handle() const
{
   return m_data->handle;
}

// ----------------------------------------------------------------------------

static api_bool CategoryEnumerationCallback( const char* category, void* m_data )
{
   reinterpret_cast<IsoStringList*>( m_data )->Add( IsoString( category ) );
   return api_true;
}

IsoStringList Process::AllProcessCategories()
{
   size_type len = 0;
   (*API->Process->EnumerateProcessCategories)( 0, 0, &len, 0 );

   IsoStringList categories;
   if ( len > 0 )
   {
      IsoString category;
      category.Reserve( len );
      if ( (*API->Process->EnumerateProcessCategories)( CategoryEnumerationCallback,
                                          category.Begin(), &len, &categories ) == api_false )
         throw APIFunctionError( "EnumerateProcessCategories" );
   }
   return categories;
}

// ----------------------------------------------------------------------------

api_bool InternalProcessEnumerator::ProcessCallback( meta_process_handle handle, void* m_data )
{
   reinterpret_cast<Array<Process>*>( m_data )->Add( Process( handle ) );
   return api_true;
}

Array<Process> Process::AllProcesses()
{
   Array<Process> processes;
   if ( (*API->Process->EnumerateProcesses)( InternalProcessEnumerator::ProcessCallback, &processes ) == api_false )
      throw APIFunctionError( "EnumerateProcesses" );
   return processes;
}

// ----------------------------------------------------------------------------

struct ProcessesByCategoryEnumerationData
{
   IsoString      category;
   Array<Process> processes;

   ProcessesByCategoryEnumerationData( const IsoString& id ) : category( id )
   {
   }
};

api_bool InternalProcessEnumerator::CategoryCallback( meta_process_handle handle, void* m_data )
{
#define enumeration reinterpret_cast<ProcessesByCategoryEnumerationData*>( m_data )
   Process P( handle );
   if ( P.Categories().Contains( enumeration->category ) )
      enumeration->processes.Add( P );
   return api_true;
#undef enumeration
}

Array<Process> Process::ProcessesByCategory( const IsoString& category )
{
   ProcessesByCategoryEnumerationData enumeration( category );
   if ( (*API->Process->EnumerateProcesses)( InternalProcessEnumerator::CategoryCallback, &enumeration ) == api_false )
      throw APIFunctionError( "EnumerateProcesses" );
   return enumeration.processes;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Process.cpp - Released 2018-11-23T16:14:32Z
