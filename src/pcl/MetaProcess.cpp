//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/MetaProcess.cpp - Released 2017-06-28T11:58:42Z
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

#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/MetaParameter.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/View.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define MANDATORY( funcName )    \
   MandatoryError( Id(), funcName )

static void MandatoryError( const IsoString& procId, const char* funcName )
{
   throw Error( String( procId ) + ": MetaProcess::" +
                funcName + "() must be reimplemented in descendant class." );
}

// ----------------------------------------------------------------------------

MetaProcess::MetaProcess() : MetaObject( Module )
{
   if ( Module == nullptr )
      throw Error( "MetaProcess: Module not initialized - illegal MetaProcess instantiation." );
}

// ----------------------------------------------------------------------------

int MetaProcess::ProcessCommandLine( const StringList& /*argv*/ ) const
{
   MANDATORY( "ProcessCommandLine" );
   return -1;
}

// ----------------------------------------------------------------------------

bool MetaProcess::EditPreferences() const
{
   MANDATORY( "EditPreferences" );
   return false;
}

// ----------------------------------------------------------------------------

bool MetaProcess::BrowseDocumentation() const
{
#if PCL_API_Version >= 0x0126
   return (*API->Global->BrowseProcessDocumentation)( this, 0/*flags*/ ) != api_false;
#else
   return false;
#endif
}

// ----------------------------------------------------------------------------

Bitmap MetaProcess::Icon() const
{
   const char** xpm = IconImageXPM();
   if ( xpm != nullptr )
      return Bitmap( xpm );

   String filePath = IconImageFile();
   if ( !filePath.IsEmpty() )
      return Bitmap( filePath );

   return Bitmap();
}

Bitmap MetaProcess::SmallIcon() const
{
   const char** xpm = SmallIconImageXPM();
   if ( xpm != nullptr )
      return Bitmap( xpm );

   String filePath = SmallIconImageFile();
   if ( !filePath.IsEmpty() )
      return Bitmap( filePath );

   return Bitmap();
}

// ----------------------------------------------------------------------------

const MetaParameter* MetaProcess::operator[]( size_type i ) const
{
   const MetaParameter* p = dynamic_cast<const MetaParameter*>( m_children[i] );
   if ( p == nullptr )
      throw Error( "MetaProcess: invalid non-parameter child instance" );
   return p;
}

// ----------------------------------------------------------------------------
// Process Context
// ----------------------------------------------------------------------------

#define process         reinterpret_cast<const MetaProcess*>( hp )
#define instance        reinterpret_cast<ProcessImplementation*>( hp )
#define constInstance   reinterpret_cast<const ProcessImplementation*>( hp )

class ProcessContextDispatcher
{
public:

   static void api_func InitializeClass( meta_process_handle hp )
   {
      try
      {
         return const_cast<MetaProcess*>( process )->InitializeClass();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static process_handle api_func CreateProcess( meta_process_handle hp )
   {
      try
      {
         return process->Create();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static void api_func DestroyProcess( process_handle hp )
   {
      try
      {
         if ( hp != 0 )
            delete instance;
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static process_handle api_func CloneProcess( const_process_handle hp )
   {
      try
      {
         return constInstance->Meta()->Clone( *constInstance );
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static process_handle api_func TestCloneProcess( const_process_handle hp )
   {
      try
      {
         return constInstance->Meta()->TestClone( *constInstance );
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func AssignProcess( process_handle hpDst, const_process_handle hpSrc )
   {
      try
      {
         reinterpret_cast<ProcessImplementation*>( hpDst )->Assign( *reinterpret_cast<const ProcessImplementation*>( hpSrc ) );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func InitializeProcess( process_handle hp )
   {
      try
      {
         instance->Initialize();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcess( process_handle hp, char16_type* info, uint32 maxLen )
   {
      try
      {
         String infoStr;
         bool valid = instance->Validate( infoStr );

         if ( !valid && !infoStr.IsEmpty() && info != 0 && maxLen > 0 )
            infoStr.c_copy( info, maxLen );

         return api_bool( valid );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static uint32 api_func ProcessExecutionPreferences( meta_process_handle hp )
   {
      try
      {
         /*
          * Currently, only bit #0 of the result is used. It is 1 if global
          * execution is preferred, 0 if view execution is preferred. Future
          * versions may return more elaborated values.
          */
         return process->PrefersGlobalExecution() ? 1 : 0;
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessExecution( const_view_handle hv, const_process_handle hp, char16_type* whyNot, uint32 maxLen )
   {
      try
      {
         String whyNotStr;
         bool ok = constInstance->CanExecuteOn( pcl::View( hv ), whyNotStr );

         if ( !ok && !whyNotStr.IsEmpty() && whyNot != 0 && maxLen > 0 )
               whyNotStr.c_copy( whyNot, maxLen );

         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessMask( const_view_handle hv, const_window_handle hw, const_process_handle hp )
   {
      try
      {
         return (api_bool)constInstance->IsMaskable( pcl::View( hv ), pcl::ImageWindow( window_handle( hw ) ) );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessHistoryUpdate( const_view_handle hv, const_process_handle hp )
   {
      try
      {
         return (api_bool)constInstance->IsHistoryUpdater( pcl::View( hv ) );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static uint32 api_func ProcessUndoMode( const_view_handle hv, const_process_handle hp )
   {
      try
      {
         return constInstance->UndoMode( pcl::View( hv ) );
      }
      ERROR_HANDLER
      return ~uint32( 0 );
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeProcessExecution( view_handle hv, process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::EnableGUIOutput();

         pcl::View v( hv );
         api_bool retVal = (api_bool)instance->BeforeExecution( v );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ExecuteProcess( view_handle hv, process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::EnableGUIOutput();

         pcl::View v( hv );
         api_bool retVal = (api_bool)instance->ExecuteOn( v );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterProcessExecution( view_handle hv, process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::EnableGUIOutput();

         pcl::View v( hv );
         instance->AfterExecution( v );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessGlobalExecution( const_process_handle hp, char16_type* whyNot, uint32 maxLen )
   {
      try
      {
         String whyNotStr;
         bool ok = constInstance->CanExecuteGlobal( whyNotStr );

         if ( !ok && !whyNotStr.IsEmpty() && whyNot != 0 && maxLen > 0 )
               whyNotStr.c_copy( whyNot, maxLen );

         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeProcessGlobalExecution( process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::EnableGUIOutput();

         api_bool retVal = (api_bool)instance->BeforeGlobalExecution();

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ExecuteProcessGlobal( process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::EnableGUIOutput();

         api_bool retVal = (api_bool)instance->ExecuteGlobal();

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterProcessGlobalExecution( process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::EnableGUIOutput();

         instance->AfterGlobalExecution();

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessImageExecution( const_image_handle hImage, const_process_handle hp, char16_type* whyNot, uint32 maxLen )
   {
      try
      {
         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !(*API->SharedImage->GetImageFormat)( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         ImageVariant image;
         void* h = const_cast<image_handle>( hImage );
         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               image = ImageVariant( new pcl::Image( h ) );
               break;
            case 64 :
               image = ImageVariant( new pcl::DImage( h ) );
               break;
            default :
               return api_false; // ?!
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               image = ImageVariant( new UInt8Image( h ) );
               break;
            case 16 :
               image = ImageVariant( new UInt16Image( h ) );
               break;
            case 32 :
               image = ImageVariant( new UInt32Image( h ) );
               break;
            default :
               return api_false; // ?!
            }

         image.SetOwnership( true );

         String whyNotStr;
         bool ok = constInstance->CanExecuteOn( image, whyNotStr );

         if ( !ok && !whyNotStr.IsEmpty() && whyNot != 0 && maxLen > 0 )
               whyNotStr.c_copy( whyNot, maxLen );

         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ExecuteProcessImage( image_handle hImage, process_handle hp, const char* hints, uint32/*flags*/ )
   {
      try
      {
         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !(*API->SharedImage->GetImageFormat)( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         ImageVariant image;
         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               image = ImageVariant( new pcl::Image( hImage ) );
               break;
            case 64 :
               image = ImageVariant( new pcl::DImage( hImage ) );
               break;
            default :
               return api_false; // ?!
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               image = ImageVariant( new UInt8Image( hImage ) );
               break;
            case 16 :
               image = ImageVariant( new UInt16Image( hImage ) );
               break;
            case 32 :
               image = ImageVariant( new UInt32Image( hImage ) );
               break;
            default :
               return api_false; // ?!
            }

         image.SetOwnership( true );

         bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
         bool wasGUIOutput = Exception::IsGUIOutputEnabled();

         try
         {
            Exception::EnableConsoleOutput();
            Exception::EnableGUIOutput();

            api_bool retVal = (api_bool)instance->ExecuteOn( image, hints );

            Exception::EnableConsoleOutput( wasConsoleOutput );
            Exception::EnableGUIOutput( wasGUIOutput );

            return retVal;
         }
         catch ( ... )
         {
            Exception::EnableConsoleOutput( wasConsoleOutput );
            Exception::EnableGUIOutput( wasGUIOutput );
            throw;
         }
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static meta_interface_handle api_func DefaultProcessInterface( meta_process_handle hp )
   {
      try
      {
         return process->DefaultInterface();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static meta_interface_handle api_func SelectProcessInterface( const_process_handle hp )
   {
      try
      {
         return constInstance->SelectInterface();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessInterface( const_process_handle hp, meta_interface_handle hi )
   {
      try
      {
         return constInstance->IsValidInterface( reinterpret_cast<const ProcessInterface*>( hi ) );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeReadingProcess( process_handle hp )
   {
      try
      {
         return (api_bool)instance->BeforeReading();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterReadingProcess( process_handle hp )
   {
      try
      {
         instance->AfterReading();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeWritingProcess( const_process_handle hp )
   {
      try
      {
         return (api_bool)constInstance->BeforeWriting();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterWritingProcess( const_process_handle hp )
   {
      try
      {
         constInstance->AfterWriting();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static int32 api_func ProcessCommandLine( meta_process_handle hp, int32 argc, const char16_type** argv )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         StringList args;
         for ( int i = 0; i < argc; ++i )
            args.Add( String( argv[i] ) );

         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         int result = process->ProcessCommandLine( args );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return result;
      }
      catch ( Exception& x )
      {
         Console().WriteLn( "<end><cbr>" + x.FormatInfo() );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return -1;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func EditProcessPreferences( meta_process_handle hp )
   {
      try
      {
         return (api_bool)process->EditPreferences();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BrowseProcessDocumentation( meta_process_handle hp )
   {
      try
      {
         return (api_bool)process->BrowseDocumentation();
      }
      ERROR_HANDLER
      return api_false;
   }

}; // ProcessContextDispatcher

#undef process
#undef instance
#undef constInstance

// ----------------------------------------------------------------------------

void MetaProcess::PerformAPIDefinitions() const
{
   (*API->ProcessDefinition->EnterProcessDefinitionContext)();

   {
      IsoString id = Id();
      (*API->ProcessDefinition->BeginProcessDefinition)( this, id.c_str() );
   }

   {
      IsoString cats = Categories();
      if ( cats.IsEmpty() )
         cats = Category(); // ### NB: Required for compatibility with PCL 1.x
      if ( !cats.IsEmpty() )
         (*API->ProcessDefinition->SetProcessCategory)( cats.c_str() );
   }

   (*API->ProcessDefinition->SetProcessVersion)( Version() );

   {
      IsoString aliases = Aliases().Trimmed();
      if ( !aliases.IsEmpty() )
         (*API->ProcessDefinition->SetProcessAliasIdentifiers)( aliases.c_str() );
   }

   {
      String desc = Description();
      if ( !desc.IsEmpty() )
         (*API->ProcessDefinition->SetProcessDescription)( desc.c_str() );
   }

   {
      String cmnt = ScriptComment();
      if ( !cmnt.IsEmpty() )
         (*API->ProcessDefinition->SetProcessScriptComment)( cmnt.c_str() );
   }

   if ( IconImageXPM() != nullptr )
      (*API->ProcessDefinition->SetProcessIconImage)( IconImageXPM() );
   else
   {
      String path = IconImageFile();
      if ( !path.IsEmpty() )
         (*API->ProcessDefinition->SetProcessIconImageFile)( path.c_str() );
   }

   if ( SmallIconImageXPM() != nullptr )
      (*API->ProcessDefinition->SetProcessIconSmallImage)( SmallIconImageXPM() );
   else
   {
      String path = SmallIconImageFile();
      if ( !path.IsEmpty() )
         (*API->ProcessDefinition->SetProcessIconSmallImageFile)( path.c_str() );
   }

   (*API->ProcessDefinition->SetProcessClassInitializationRoutine)( ProcessContextDispatcher::InitializeClass );
   (*API->ProcessDefinition->SetProcessCreationRoutine)( ProcessContextDispatcher::CreateProcess );
   (*API->ProcessDefinition->SetProcessDestructionRoutine)( ProcessContextDispatcher::DestroyProcess );
   (*API->ProcessDefinition->SetProcessClonationRoutine)( ProcessContextDispatcher::CloneProcess );
   (*API->ProcessDefinition->SetProcessTestClonationRoutine)( ProcessContextDispatcher::TestCloneProcess );

   if ( IsAssignable() )
      (*API->ProcessDefinition->SetProcessAssignmentRoutine)( ProcessContextDispatcher::AssignProcess );

   if ( NeedsInitialization() )
      (*API->ProcessDefinition->SetProcessInitializationRoutine)( ProcessContextDispatcher::InitializeProcess );

   if ( NeedsValidation() )
      (*API->ProcessDefinition->SetProcessValidationRoutine)( ProcessContextDispatcher::ValidateProcess );

   if ( CanProcessCommandLines() )
      (*API->ProcessDefinition->SetProcessCommandLineProcessingRoutine)( ProcessContextDispatcher::ProcessCommandLine, 0 );

   if ( CanEditPreferences() )
      (*API->ProcessDefinition->SetProcessEditPreferencesRoutine)( ProcessContextDispatcher::EditProcessPreferences );

   if ( CanBrowseDocumentation() )
      (*API->ProcessDefinition->SetProcessBrowseDocumentationRoutine)( ProcessContextDispatcher::BrowseProcessDocumentation );

   (*API->ProcessDefinition->SetProcessExecutionPreferencesRoutine)( ProcessContextDispatcher::ProcessExecutionPreferences );

   if ( CanProcessViews() )
   {
      (*API->ProcessDefinition->SetProcessExecutionValidationRoutine)( ProcessContextDispatcher::ValidateProcessExecution );
      (*API->ProcessDefinition->SetProcessMaskValidationRoutine)( ProcessContextDispatcher::ValidateProcessMask );
      (*API->ProcessDefinition->SetProcessHistoryUpdateValidationRoutine)( ProcessContextDispatcher::ValidateProcessHistoryUpdate );
      (*API->ProcessDefinition->SetProcessUndoModeRoutine)( ProcessContextDispatcher::ProcessUndoMode );
      (*API->ProcessDefinition->SetProcessPreExecutionRoutine)( ProcessContextDispatcher::BeforeProcessExecution );
      (*API->ProcessDefinition->SetProcessExecutionRoutine)( ProcessContextDispatcher::ExecuteProcess );
      (*API->ProcessDefinition->SetProcessPostExecutionRoutine)( ProcessContextDispatcher::AfterProcessExecution );
   }

   if ( CanProcessGlobal() )
   {
      (*API->ProcessDefinition->SetProcessGlobalExecutionValidationRoutine)( ProcessContextDispatcher::ValidateProcessGlobalExecution );
      (*API->ProcessDefinition->SetProcessPreGlobalExecutionRoutine)( ProcessContextDispatcher::BeforeProcessGlobalExecution );
      (*API->ProcessDefinition->SetProcessGlobalExecutionRoutine)( ProcessContextDispatcher::ExecuteProcessGlobal );
      (*API->ProcessDefinition->SetProcessPostGlobalExecutionRoutine)( ProcessContextDispatcher::AfterProcessGlobalExecution );
   }

   if ( CanProcessImages() )
   {
      (*API->ProcessDefinition->SetProcessImageExecutionValidationRoutine)( ProcessContextDispatcher::ValidateProcessImageExecution );
      (*API->ProcessDefinition->SetProcessImageExecutionRoutine)( ProcessContextDispatcher::ExecuteProcessImage );
   }

   (*API->ProcessDefinition->SetProcessDefaultInterfaceSelectionRoutine)( ProcessContextDispatcher::DefaultProcessInterface );
   (*API->ProcessDefinition->SetProcessInterfaceSelectionRoutine)( ProcessContextDispatcher::SelectProcessInterface );
   (*API->ProcessDefinition->SetProcessInterfaceValidationRoutine)( ProcessContextDispatcher::ValidateProcessInterface );

   (*API->ProcessDefinition->SetProcessPreReadingRoutine)( ProcessContextDispatcher::BeforeReadingProcess );
   (*API->ProcessDefinition->SetProcessPostReadingRoutine)( ProcessContextDispatcher::AfterReadingProcess );
   (*API->ProcessDefinition->SetProcessPreWritingRoutine)( ProcessContextDispatcher::BeforeWritingProcess );
   (*API->ProcessDefinition->SetProcessPostWritingRoutine)( ProcessContextDispatcher::AfterWritingProcess );

   for ( size_type j = 0; j < Length(); ++j )
      (*this)[j]->PerformAPIDefinitions();

   (*API->ProcessDefinition->EndProcessDefinition)();

   (*API->ProcessDefinition->ExitProcessDefinitionContext)();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MetaProcess.cpp - Released 2017-06-28T11:58:42Z
