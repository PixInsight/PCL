// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Process.cpp - Released 2014/11/14 17:17:01 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include <pcl/Process.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ProcessPrivate
{
public:

   meta_process_handle handle;
   api_process_props   properties;

   ProcessPrivate() : handle( 0 )
   {
      GetProperties();
   }

   ProcessPrivate( meta_process_handle h ) : handle( h )
   {
      GetProperties();
   }

   ~ProcessPrivate()
   {
      handle = 0;
      GetProperties();
   }

private:

   void GetProperties()
   {
      // Strict aliasing safe code
      union { api_process_props properties; uint32 u; } v;
      v.u = 0u;
      properties = v.properties;

      if ( handle != 0 )
         if ( !(*API->Process->GetProcessProperties)( handle, &properties ) )
            throw APIFunctionError( "GetProcessProperties" );
   }
};

// ----------------------------------------------------------------------------

Process::Process( const IsoString& id ) :
ProcessBase(),
data( new ProcessPrivate( (*API->Process->GetProcessByName)( ModuleHandle(), id.c_str() ) ) )
{
   if ( data->handle == 0 )
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
ProcessBase(),
data( new ProcessPrivate( p.data->handle ) )
{
}

Process::Process( const void* handle ) :
ProcessBase(),
data( new ProcessPrivate( handle ) )
{
   if ( data->handle == 0 )
      throw Error( "Process: Null process handle" );
}

Process::~Process()
{
   if ( data != 0 )
      delete data, data = 0;
}

// ----------------------------------------------------------------------------

IsoString Process::Id() const
{
   size_type len = 0;
   (*API->Process->GetProcessIdentifier)( data->handle, 0, &len );

   IsoString id;

   if ( len != 0 )
   {
      id.Reserve( len );
      if ( (*API->Process->GetProcessIdentifier)( data->handle, id.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetProcessIdentifier" );
   }

   return id;
}

// ----------------------------------------------------------------------------

IsoStringList Process::Aliases() const
{
   size_type len = 0;
   (*API->Process->GetProcessAliasIdentifiers)( data->handle, 0, &len );

   IsoStringList aliases;

   if ( len != 0 )
   {
      IsoString csAliases;
      csAliases.Reserve( len );
      if ( (*API->Process->GetProcessAliasIdentifiers)( data->handle, csAliases.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetProcessAliasIdentifiers" );
      csAliases.Break( aliases, ',' );
   }

   return aliases;
}

// ----------------------------------------------------------------------------

IsoStringList Process::Categories() const
{
   size_type len = 0;
   (*API->Process->GetProcessCategory)( data->handle, 0, &len );

   IsoStringList categories;

   if ( len != 0 )
   {
      IsoString csCategories;
      csCategories.Reserve( len );
      if ( (*API->Process->GetProcessCategory)( data->handle, csCategories.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetProcessCategory" );
      csCategories.Break( categories, ',' );
   }

   return categories;
}

// ----------------------------------------------------------------------------

uint32 Process::Version() const
{
   return (*API->Process->GetProcessVersion)( data->handle );
}

// ----------------------------------------------------------------------------

String Process::Description() const
{
   size_type len = 0;
   (*API->Process->GetProcessDescription)( data->handle, 0, &len );

   String description;

   if ( len != 0 )
   {
      description.Reserve( len );
      if ( (*API->Process->GetProcessDescription)( data->handle, description.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetProcessDescription" );
   }

   return description;
}

// ----------------------------------------------------------------------------

String Process::ScriptComment() const
{
   size_type len = 0;
   (*API->Process->GetProcessScriptComment)( data->handle, 0, &len );

   String comment;

   if ( len != 0 )
   {
      comment.Reserve( len );
      if ( (*API->Process->GetProcessScriptComment)( data->handle, comment.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetProcessScriptComment" );
   }

   return comment;
}

// ----------------------------------------------------------------------------

Bitmap Process::Icon() const
{
   return Bitmap( (*API->Process->GetProcessIcon)( data->handle ) );
}

// ----------------------------------------------------------------------------

Bitmap Process::SmallIcon() const
{
   return Bitmap( (*API->Process->GetProcessSmallIcon)( data->handle ) );
}

// ----------------------------------------------------------------------------

bool Process::CanProcessViews() const
{
   return data->properties.canProcessViews;
}

// ----------------------------------------------------------------------------

bool Process::CanProcessGlobal() const
{
   return data->properties.canProcessGlobal;
}

// ----------------------------------------------------------------------------

bool Process::CanProcessImages() const
{
   return data->properties.canProcessImages;
}

// ----------------------------------------------------------------------------

bool Process::CanProcessCommandLines() const
{
   return data->properties.canProcessCommandLines;
}

// ----------------------------------------------------------------------------

bool Process::CanEditPreferences() const
{
   return data->properties.canEditPreferences;
}

// ----------------------------------------------------------------------------

bool Process::CanBrowseDocumentation() const
{
   return data->properties.canBrowseDocumentation;
}

// ----------------------------------------------------------------------------

bool Process::IsAssignable() const
{
   return data->properties.isAssignable;
}

// ----------------------------------------------------------------------------

bool Process::NeedsInitialization() const
{
   return data->properties.needsInitialization;
}

// ----------------------------------------------------------------------------

bool Process::NeedsValidation() const
{
   return data->properties.needsValidation;
}

// ----------------------------------------------------------------------------

bool Process::PrefersGlobalExecution() const
{
   return data->properties.prefersGlobalExecution;
}

// ----------------------------------------------------------------------------

bool Process::EditPreferences() const
{
   return (*API->Process->EditProcessPreferences)( data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool Process::BrowseDocumentation() const
{
   return (*API->Process->BrowseProcessDocumentation)( data->handle, 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

int Process::RunCommandLine( const String& arguments ) const
{
   return int( (*API->Process->RunProcessCommandLine)( data->handle, arguments.c_str() ) );
}

// ----------------------------------------------------------------------------

bool Process::Launch() const
{
   return (*API->Process->LaunchProcess)( data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

Process::parameter_list Process::Parameters() const
{
   parameter_list parameters;
   if ( (*API->Process->EnumerateProcessParameters)( data->handle,
                  InternalParameterEnumerator::ParameterCallback, &parameters ) == api_false )
      throw APIFunctionError( "EnumerateProcessParameters" );
   return parameters;
}

// ----------------------------------------------------------------------------

bool Process::HasParameter( const IsoString& paramId ) const
{
   return (*API->Process->GetParameterByName)( data->handle, paramId.c_str() ) != 0;
}

bool Process::HasTableColumn( const IsoString& tableId, const IsoString& colId ) const
{
   meta_parameter_handle hTable = (*API->Process->GetParameterByName)( data->handle, tableId.c_str() );
   return hTable != 0 && (*API->Process->GetTableColumnByName)( data->handle, colId.c_str() ) != 0;
}

// ----------------------------------------------------------------------------

const void* Process::Handle() const
{
   return data->handle;
}

// ----------------------------------------------------------------------------

static api_bool CategoryEnumerationCallback( const char* category, void* data )
{
   reinterpret_cast<IsoStringList*>( data )->Add( IsoString( category ) );
   return api_true;
}

IsoStringList Process::AllProcessCategories()
{
   size_type len = 0;
   (*API->Process->EnumerateProcessCategories)( 0, 0, &len, 0 );

   IsoStringList categories;

   if ( len != 0 )
   {
      IsoString category;
      category.Reserve( len );
      if ( (*API->Process->EnumerateProcessCategories)( CategoryEnumerationCallback,
                                          category.c_str(), &len, &categories ) == api_false )
         throw APIFunctionError( "EnumerateProcessCategories" );
   }

   return categories;
}

// ----------------------------------------------------------------------------

api_bool InternalProcessEnumerator::ProcessCallback( meta_process_handle handle, void* data )
{
   reinterpret_cast<Array<Process>*>( data )->Add( Process( handle ) );
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

api_bool InternalProcessEnumerator::CategoryCallback( meta_process_handle handle, void* data )
{
#define enumeration reinterpret_cast<ProcessesByCategoryEnumerationData*>( data )
   Process P( handle );
   if ( P.Categories().Has( enumeration->category ) )
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

// ****************************************************************************
// EOF pcl/Process.cpp - Released 2014/11/14 17:17:01 UTC
