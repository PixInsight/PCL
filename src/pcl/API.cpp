//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/API.cpp - Released 2015/11/26 15:59:45 UTC
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

#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Global API Data
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PCL_DATA const APIInterface* API = nullptr;

static api_handle s_moduleHandle = nullptr;

api_handle PCL_FUNC ModuleHandle()
{
   return s_moduleHandle;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// API Exceptions
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void APIError::GetAPIErrorCode()
{
   if ( API != nullptr && API->Global != nullptr && API->Global->LastError != 0 )
      apiErrorCode = (*API->Global->LastError)();
   else
      apiErrorCode = 0;
}

String APIError::FormatInfo() const
{
   String info = Error::FormatInfo();

   if ( apiErrorCode != 0 )
   {
      info += String().Format( "\nAPI error code = %u", apiErrorCode );

      String apiMessage;
      if ( apiErrorCode != 0 )
      {
         size_type len = 0;
         (*API->Global->ErrorMessage)( apiErrorCode, 0, &len );
         if ( len > 0 )
         {
            apiMessage.SetLength( len );
            (*API->Global->ErrorMessage)( apiErrorCode, apiMessage.c_str(), &len );
            apiMessage.ResizeToNullTerminated();
         }
      }

      if ( apiMessage.IsEmpty() )
         info += "\n<* No additional information *>";
      else
         info += ":\n", info += apiMessage;
   }

   return info;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// API Initialization Routines
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class APIInitializer
{
public:

   static void HackingError( const String& msg )
   {
      throw Error( "<* Warning *> Possible hacking attempt: " + msg );
   }

   static uint32 InitAPI( function_resolver R, uint32 apiVersion )
   {
      try
      {
         if ( API != nullptr )
            HackingError( "Unexpected API module initialization call." );

         if ( R == nullptr )
            HackingError( "Invalid API function resolver address." );

         if ( apiVersion < PCL_API_Version )
            throw Error( String().Format( "Unsupported API version %X (expected >= %X).",
                                          apiVersion, PCL_API_Version ) );

         if ( Module == nullptr )
            throw Error( "Module metadata not available. "
                         "Please instantiate a MetaModule descendant upon module installation." );

         API = new APIInterface( R );

         Module->PerformAPIDefinitions();

         return 0; // ok
      }

      ERROR_CLEANUP(
         if ( API != nullptr )
         {
            delete API;
            API = nullptr;
         }
      )
      return 1;
   }

   static uint32 BeginIdentification()
   {
      try
      {
         if ( description != nullptr )
            HackingError( "Invalid API identification call." );

         if ( Module == nullptr )
            throw Error( "Module metadata not available. "
                         "Please instantiate a MetaModule descendant upon module installation." );

         description = new api_module_description;

         description->uniqueId          = Module->UniqueId(); // ### deprecated
         description->versionInfo       = Module->Version();
         description->name              = Module->Name().Release();
         description->description       = Module->Description().Release();
         description->company           = Module->Company().Release();
         description->author            = Module->Author().Release();
         description->copyright         = Module->Copyright().Release();
         description->tradeMarks        = Module->TradeMarks().Release();
         description->originalFileName  = Module->OriginalFileName().Release();

         int year, month, day;
         Module->GetReleaseDate( year, month, day );

         description->releaseDate.year  = year;
         description->releaseDate.month = month;
         description->releaseDate.day   = day;

         description->apiVersion        = PCL_API_Version;

         return 0; // ok
      }

      ERROR_HANDLER
      return 1;
   }

   static uint32 EndIdentification()
   {
      try
      {
         if ( description == nullptr )
            HackingError( "Invalid API identification call." );

         if ( description->name != nullptr )
            delete [] description->name;
         if ( description->description != nullptr )
            delete [] description->description;
         if ( description->company != nullptr )
            delete [] description->company;
         if ( description->author != nullptr )
            delete [] description->author;
         if ( description->copyright != nullptr )
            delete [] description->copyright;
         if ( description->tradeMarks != nullptr )
            delete [] description->tradeMarks;
         if ( description->originalFileName != nullptr )
            delete [] description->originalFileName;

         delete description;
         description = nullptr;

         return 0; // ok
      }

      ERROR_HANDLER
      return 1;
   }

   static uint32 Identify( api_module_description** d )
   {
      try
      {
         if ( description == nullptr )
            HackingError( "Invalid API identification call." );

         if ( d == nullptr )
            HackingError( "Invalid API module description structure." );

         *d = description;

         return 0; // ok
      }

      ERROR_HANDLER
      return 1;
   }

private:

   static api_module_description* description;

}; // APIInitializer

api_module_description* APIInitializer::description = nullptr;

} // pcl

// ----------------------------------------------------------------------------

// global namespace

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// uint32 IdentifyPixInsightModule( const void**, int32 )
//
// PixInsight Module Identification Callback.
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT uint32
IdentifyPixInsightModule( void** apiModuleDescription, int32  phase )
{
   switch ( phase )
   {
   case 0:
      // Begin module identification
      return APIInitializer::BeginIdentification();
   case 1:
      // Report module identification
      return APIInitializer::Identify( reinterpret_cast<api_module_description**>( apiModuleDescription ) );
   case 255:
      // End module identification
      return APIInitializer::EndIdentification();
   default:
      // Unknown identification request --should not happen!
      break;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// uint32 InitializePixInsightModule( api_handle, function_resolver, uint32, void* )
//
// PixInsight Module Entry Point.
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT uint32
InitializePixInsightModule( api_handle        hModule,
                            function_resolver R,
                            uint32            apiVersion,
                            void*             /*reserved*/ )
{
   s_moduleHandle = hModule;
   return APIInitializer::InitAPI( R, apiVersion );
}

// ----------------------------------------------------------------------------
// EOF pcl/API.cpp - Released 2015/11/26 15:59:45 UTC
