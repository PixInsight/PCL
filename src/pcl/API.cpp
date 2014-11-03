// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/API.cpp - Released 2014/10/29 07:34:21 UTC
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

PCL_DATA const APIInterface* API = 0;

static api_handle s_moduleHandle = 0;

api_handle ModuleHandle()
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
   if ( API != 0 && API->Global != 0 && API->Global->LastError != 0 )
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

         if ( len != 0 )
         {
            apiMessage.Reserve( len );
            (*API->Global->ErrorMessage)( apiErrorCode, apiMessage.c_str(), &len );
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
         if ( API != 0 )
            HackingError( "Unexpected API module initialization call." );

         if ( R == 0 )
            HackingError( "Invalid API function resolver address." );

         if ( apiVersion < PCL_API_Version )
            throw Error( String().Format( "Unsupported API version %X (expected >= %X).",
                                          apiVersion, PCL_API_Version ) );

         if ( Module == 0 )
            throw Error( "Module metadata not available. "
                         "Please instantiate a MetaModule descendant upon module installation." );

         API = new APIInterface( R );

         Module->PerformAPIDefinitions();

         return 0; // ok
      }

      ERROR_CLEANUP(
         if ( API != 0 )
         {
            delete API;
            API = 0;
         }
      )
      return 1;
   }

   static uint32 BeginIdentification()
   {
      try
      {
         if ( description != 0 )
            HackingError( "Invalid API identification call." );

         if ( Module == 0 )
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
         if ( description == 0 )
            HackingError( "Invalid API identification call." );

         if ( description->name != 0 )
            delete [] description->name;
         if ( description->description != 0 )
            delete [] description->description;
         if ( description->company != 0 )
            delete [] description->company;
         if ( description->author != 0 )
            delete [] description->author;
         if ( description->copyright != 0 )
            delete [] description->copyright;
         if ( description->tradeMarks != 0 )
            delete [] description->tradeMarks;
         if ( description->originalFileName != 0 )
            delete [] description->originalFileName;

         delete description;
         description = 0;

         return 0; // ok
      }

      ERROR_HANDLER
      return 1;
   }

   static uint32 Identify( api_module_description** d )
   {
      try
      {
         if ( description == 0 )
            HackingError( "Invalid API identification call." );

         if ( d == 0 )
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

api_module_description* APIInitializer::description = 0;

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

// ****************************************************************************
// EOF pcl/API.cpp - Released 2014/10/29 07:34:21 UTC
