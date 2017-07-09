//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/API.cpp - Released 2017-07-09T18:07:16Z
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

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#ifdef __PCL_WINDOWS
#  include <stdio.h> // _setmaxstdio()
#endif

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
      if ( info.IsEmpty() )
         info << String().Format( "API error code 0x%04x", apiErrorCode );
      else
         info << String().Format( " (0x%04x)", apiErrorCode );

      String apiMessage;
      if ( apiErrorCode != 0 )
      {
         size_type len = 0;
         (*API->Global->ErrorMessage)( apiErrorCode, 0, &len );
         if ( len > 0 )
         {
            apiMessage.SetLength( len );
            (*API->Global->ErrorMessage)( apiErrorCode, apiMessage.Begin(), &len );
            apiMessage.ResizeToNullTerminated();
         }
      }

      if ( !apiMessage.IsEmpty() )
         info << ": " << apiMessage;
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

#ifdef __PCL_WINDOWS
         /*
          * Win32 specific: Set the maximum possible number of simultaneously
          * open files at the "stdio level":
          * https://msdn.microsoft.com/en-us/library/6e3b887c.aspx
          */
         (void)_setmaxstdio( 2048 );
#endif
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

/*!
 * PixInsight Module Identification (PMIDN) function.
 *
 * This function will be called by the PixInsight core application when it
 * needs to retrieve identification and descriptive data from a module. This
 * happens when a module is about to be installed, but also when a module is
 * being inspected, either for security reasons or after a direct user request.
 *
 * \param description   Pointer to a pointer to an API module description
 *          structure. See the declaration of api_module_description in
 *          API/APIDefs.h for details. This structure must be provided by the
 *          called module as POD, and its starting address must be written to
 *          the pointer pointed to by this argument in a call to this function
 *          with \a phase = 1 (see below).
 *
 * \param phase         Module identification request:\n
 *          \n
 *          <table border="1" cellpadding="4" cellspacing="0">
 *          <tr>
 *          <td>\a phase = 0x00 - Prepare for module identification.</td>
 *          </tr>
 *          <tr>
 *          <td>\a phase = 0x01 - Report module descriptive data in a structure
 *          of type 'api_module_description', whose address must be stored in
 *          the pointer pointed to by the \a description argument.</td>
 *          </tr>
 *          <tr>
 *          <td>\a phase = 0xff - Module identification process completed. The
 *          module description structure provided when \a phase = 1 can be
 *          deallocated and disposed as necessary.</td>
 *          </tr>
 *          </table>
 *
 * Other values of \a phase may be passed in additional calls to a PMIDN. Those
 * values and calls are reserved for special modules pertaining to the core of
 * the PixInsight platform, and hence not in the user land. In current versions
 * of PixInsight, such special calls must be ignored by the invoked module.
 *
 * A PMIDN must return zero upon success. Any other return value will be
 * interpreted as a module-specific error code.
 *
 * Module developers using the standard PixInsight Class Library (PCL)
 * distribution don't have to care about PMIDN, since it is already implemented
 * internally by PCL.
 *
 * \note A PMIDN is mandatory for all PixInsight modules, and must be
 * implemented as a publicly visible symbol following the standard 'C' naming
 * and calling conventions.
 */
PCL_MODULE_EXPORT uint32
IdentifyPixInsightModule( api_module_description** description,
                          int32                    phase )
{
   switch ( phase )
   {
   case 0:
      // Prepare for module identification.
      return APIInitializer::BeginIdentification();
   case 1:
      // Report module description data.
      return APIInitializer::Identify( description );
   case 255:
      // Done with module identification.
      return APIInitializer::EndIdentification();
   default:
      // Reserved identification request - ignore it.
      break;
   }

   return 0;
}

/*!
 * PixInsight Module Initialization (PMINI) function.
 *
 * This function will be called by the PixInsight core application when a
 * module is being installed. It provides a module with the necessary elements
 * to perform a bidirectional communication with the core application via the
 * standard PCL API callback system.
 *
 * \param hModule       Handle to the module being installed. This handle
 *                      uniquely identifies the module, and must be used in all
 *                      subsequent API calls requiring a module handle.
 *
 * \param R             Pointer to an API function resolver callback. See the
 *                      declaration of function_resolver in API/APIDefs.h and
 *                      the automatically generated file pcl/APIInterface.cpp
 *                      for details.
 *
 * \param apiVersion    API version number.
 *
 * \param reserved      Reserved for special invocations to core platform
 *                      modules. Must not be used or altered in any way.
 *
 * A PMINI must return zero upon success. Any other return value will be
 * interpreted as a module-specific error code.
 *
 * Module developers using the standard PixInsight Class Library (PCL)
 * distribution don't have to care about PMINI, since it is already implemented
 * internally by PCL.
 *
 * \note A PMINI is mandatory for all PixInsight modules, and must be
 * implemented as a publicly visible symbol following the standard 'C' naming
 * and calling conventions.
 */
PCL_MODULE_EXPORT uint32
InitializePixInsightModule( api_handle        hModule,
                            function_resolver R,
                            uint32            apiVersion,
                            void*             reserved )
{
   s_moduleHandle = hModule;
   return APIInitializer::InitAPI( R, apiVersion );
}

// ----------------------------------------------------------------------------
// EOF pcl/API.cpp - Released 2017-07-09T18:07:16Z
