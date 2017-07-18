//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/MetaModule.cpp - Released 2017-07-18T16:14:00Z
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
#include <pcl/MetaModule.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Version.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MetaModule* Module = nullptr;

// ----------------------------------------------------------------------------

MetaModule::MetaModule() :
   MetaObject( nullptr )
{
   if ( Module != nullptr )
      throw Error( "MetaModule: Module redefinition not allowed" );
   Module = this;
}

MetaModule::~MetaModule()
{
   if ( this == Module )
      Module = nullptr;
}

/*
 * ### REMOVEME - deprecated function
 */
const char* MetaModule::UniqueId() const
{
   return nullptr;
}

bool MetaModule::IsInstalled() const
{
   return API != nullptr;
}

void MetaModule::ProcessEvents( bool excludeUserInputEvents )
{
   thread_handle thread = (*API->Thread->GetCurrentThread)();
   if ( thread == 0 ) // if root thread
      (*API->Global->ProcessEvents)( excludeUserInputEvents );
   else
   {
      uint32 threadStatus = (*API->Thread->GetThreadStatus)( thread );
      if ( threadStatus & 0x80000000 ) // see Thread.cpp
         throw ProcessAborted();
   }
}

// ----------------------------------------------------------------------------

/*
 * Length of the version marker string: "PIXINSIGHT_MODULE_VERSION_".
 *
 * PIXINSIGHT_MODULE_VERSION_
 * 12345678901234567890123456
 *          1         2
 *
 * Note that we cannot define the marker string in source code, or the module
 * authentication routines would detect our instance, instead of the actual
 * version information string. This would prevent module authentication.
 */
#define LengthOfVersionMarker 26

void MetaModule::GetVersion( int& major, int& minor, int& release, int& build,
                             IsoString& language, IsoString& status ) const
{
   // Set undefined states for all variables, in case of error.
   major = minor = release = build = 0;
   language.Clear();
   status.Clear();

   IsoString vs( Version() );

   // A version string must begin with a version marker
   if ( vs.Length() < LengthOfVersionMarker )
      return;

   // Split the string of version numbers into tokens separated by dots
   StringList tokens;
   vs.Break( tokens, '.', false/*trim*/, LengthOfVersionMarker );

   // Required: MM.mm.rr.bbbb.LLL
   // Optional: .<status>
   if ( tokens.Length() < 5 || tokens.Length() > 6 )
      return;

   // Extract version numbers
   try
   {
      int MM   = tokens[0].ToInt( 10 );
      int mm   = tokens[1].ToInt( 10 );
      int rr   = tokens[2].ToInt( 10 );
      int bbbb = tokens[3].ToInt( 10 );

      major = MM;
      minor = mm;
      release = rr;
      build = bbbb;
   }
   catch ( ... ) // silently eat all parse exceptions here
   {
      return;
   }

   // Language code
   language = tokens[4]; // ### TODO: Verify validity of ISO 639.2 code

   // Optional status word
   if ( tokens.Length() == 6 )
      status = tokens[5];  // ### TODO: Verify validity of the status word
}

IsoString MetaModule::ReadableVersion() const
{
   int major, minor, release, build;
   IsoString dum1, dum2;
   GetVersion( major, minor, release, build, dum1, dum2 );
   return Name() + IsoString().Format( " module version %02d.%02d.%02d.%04d", major, minor, release, build );
}

// ----------------------------------------------------------------------------

void MetaModule::LoadResource( const String& filePath, const String& rootPath )
{
   if ( (*API->Module->LoadResource)( ModuleHandle(), filePath.c_str(), rootPath.c_str() ) == api_false )
      throw APIFunctionError( "LoadResource" );
}

void MetaModule::UnloadResource( const String& filePath, const String& rootPath )
{
   if ( (*API->Module->UnloadResource)( ModuleHandle(), filePath.c_str(), rootPath.c_str() ) == api_false )
      throw APIFunctionError( "UnloadResource" );
}

// ----------------------------------------------------------------------------

Variant MetaModule::EvaluateScript( const String& sourceCode, const IsoString& language )
{
   api_property_value result;
   if ( (*API->Module->EvaluateScript)( ModuleHandle(), &result, sourceCode.c_str(), language.c_str() ) == api_false )
      throw APIFunctionError( "EvaluateScript" );
   return VariantFromAPIPropertyValue( result );
}

// ----------------------------------------------------------------------------
// Global Context
// ----------------------------------------------------------------------------

class GlobalContextDispatcher
{
public:

   static void api_func OnLoad()
   {
      try
      {
         Module->OnLoad();
      }
      ERROR_HANDLER
   }

   static void api_func OnUnload()
   {
      try
      {
         Module->OnUnload();

         if ( Module != nullptr )
            for ( size_type i = 0; i < Module->Length(); ++i )
               if ( (*Module)[i] != nullptr )
               {
                  const ProcessInterface* iface = dynamic_cast<const ProcessInterface*>( (*Module)[i] );
                  if ( iface != nullptr && iface->LaunchCount() != 0 )
                  {
                     if ( iface->IsAutoSaveGeometryEnabled() )
                        iface->SaveGeometry();
                     iface->SaveSettings();
                  }
               }
      }
      ERROR_HANDLER
   }

   static void* api_func Allocate( size_type sz )
   {
      try
      {
         return Module->Allocate( sz );
      }
      ERROR_HANDLER
      return nullptr;
   }

   static void api_func Deallocate( void* p )
   {
      try
      {
         Module->Deallocate( p );
      }
      ERROR_HANDLER
   }
}; // GlobalContextDispatcher

// ----------------------------------------------------------------------------

void MetaModule::PerformAPIDefinitions() const
{
   (*API->ModuleDefinition->EnterModuleDefinitionContext)();

   (*API->ModuleDefinition->SetModuleOnLoadRoutine)( GlobalContextDispatcher::OnLoad );
   (*API->ModuleDefinition->SetModuleOnUnloadRoutine)( GlobalContextDispatcher::OnUnload );
   (*API->ModuleDefinition->SetModuleAllocationRoutine)( GlobalContextDispatcher::Allocate );
   (*API->ModuleDefinition->SetModuleDeallocationRoutine)( GlobalContextDispatcher::Deallocate );

   /*
    * Meta object Definitions
    */
   for ( size_type i = 0; i < Module->Length(); ++i )
   {
      const MetaObject* o = (*Module)[i];
      if ( o != nullptr )
         o->PerformAPIDefinitions();
   }

   (*API->ModuleDefinition->ExitModuleDefinitionContext)();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MetaModule.cpp - Released 2017-07-18T16:14:00Z
