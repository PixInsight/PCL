//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/ExternalProcess.cpp - Released 2017-08-01T14:23:38Z
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
#include <pcl/ErrorHandler.h>
#include <pcl/ExternalProcess.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ExternalProcessPrivate
{
public:

   static api_bool EnvironmentEnumerationCallback( const char16_type* var, void* data )
   {
#define environment  reinterpret_cast<StringList*>( data )
      try
      {
         environment->Append( String( var ) );
         return api_true;
      }
      catch ( ... )
      {
         return api_false;
      }
#undef environment
   }

   static void Throw( int apiErrorCode )
   {
      String errorMessage = "ExternalProcess: ";
      switch ( apiErrorCode )
      {
      case ExternalProcessContext::FailedToStart:
         errorMessage += "Failed to start process";
         break;
      case ExternalProcessContext::Crashed:
         errorMessage += "The process crashed";
         break;
      case ExternalProcessContext::TimedOut:
         errorMessage += "The process timed out";
         break;
      case ExternalProcessContext::ReadError:
         errorMessage += "Read error";
         break;
      case ExternalProcessContext::WriteError:
         errorMessage += "Write error";
         break;
      default:
      case ExternalProcessContext::UnknownError:
         errorMessage += "Unknown error";
         break;
      case ExternalProcessContext::InvalidError:
         errorMessage += "Internal error: Invalid object";
         break;
      }

      throw Error( errorMessage );
   }
};

// ----------------------------------------------------------------------------

ExternalProcess::ExternalProcess() :
   UIObject( (*API->ExternalProcess->CreateExternalProcess)( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateExternalProcess" );
}

// ----------------------------------------------------------------------------

ExternalProcess::ExternalProcess( void* h ) :
   UIObject( h )
{
}

// ----------------------------------------------------------------------------

ExternalProcess& ExternalProcess::Null()
{
   static ExternalProcess* nullExternalProcess = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullExternalProcess == nullptr )
      nullExternalProcess = new ExternalProcess( nullptr );
   return *nullExternalProcess;
}

// ----------------------------------------------------------------------------

void ExternalProcess::Start( const String& program, const StringList& arguments )
{
   Array<const char16_type*> argv;
   for ( const String& arg : arguments )
      argv.Add( arg.c_str() );
   if ( (*API->ExternalProcess->StartExternalProcess)( handle, program.c_str(), argv.Begin(), argv.Length() ) == api_false )
      throw APIFunctionError( "StartExternalProcess" );
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForStarted( int ms )
{
   return (*API->ExternalProcess->WaitForExternalProcessStarted)( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForFinished( int ms )
{
   return (*API->ExternalProcess->WaitForExternalProcessFinished)( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForDataAvailable( int ms )
{
   return (*API->ExternalProcess->WaitForExternalProcessDataAvailable)( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForDataWritten( int ms )
{
   return (*API->ExternalProcess->WaitForExternalProcessDataWritten)( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

void ExternalProcess::Terminate()
{
   if ( (*API->ExternalProcess->TerminateExternalProcess)( handle ) == api_false )
      throw APIFunctionError( "TerminateExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Kill()
{
   if ( (*API->ExternalProcess->KillExternalProcess)( handle ) == api_false )
      throw APIFunctionError( "KillExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::CloseStandardInput()
{
   if ( (*API->ExternalProcess->CloseExternalProcessStream)( handle, ExternalProcessContext::StandardInput ) == api_false )
      throw APIFunctionError( "CloseExternalProcessStream" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::CloseStandardOutput()
{
   if ( (*API->ExternalProcess->CloseExternalProcessStream)( handle, ExternalProcessContext::StandardOutput ) == api_false )
      throw APIFunctionError( "CloseExternalProcessStream" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::CloseStandardError()
{
   if ( (*API->ExternalProcess->CloseExternalProcessStream)( handle, ExternalProcessContext::StandardError ) == api_false )
      throw APIFunctionError( "CloseExternalProcessStream" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardOutput( const String& filePath, bool append )
{
   if ( (*API->ExternalProcess->RedirectExternalProcessToFile)( handle,
               ExternalProcessContext::StandardOutput, filePath.c_str(), append ) == api_false )
      throw APIFunctionError( "RedirectExternalProcessToFile" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardOutput( ExternalProcess& process )
{
   if ( (*API->ExternalProcess->PipeExternalProcess)( handle,
               ExternalProcessContext::StandardOutput, process.handle ) == api_false )
      throw APIFunctionError( "PipeExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardError( const String& filePath, bool append )
{
   if ( (*API->ExternalProcess->RedirectExternalProcessToFile)( handle,
               ExternalProcessContext::StandardError, filePath.c_str(), append ) == api_false )
      throw APIFunctionError( "RedirectExternalProcessToFile" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardInput( const String& filePath )
{
   if ( (*API->ExternalProcess->RedirectExternalProcessToFile)( handle,
               ExternalProcessContext::StandardInput, filePath.c_str(), api_false/*append*/ ) == api_false )
      throw APIFunctionError( "RedirectExternalProcessToFile" );
}

// ----------------------------------------------------------------------------

String ExternalProcess::WorkingDirectory() const
{
   size_type len = 0;
   (*API->ExternalProcess->GetExternalProcessWorkingDirectory)( handle, 0, &len );

   String dirPath;
   if ( len > 0 )
   {
      dirPath.SetLength( len );
      if ( (*API->ExternalProcess->GetExternalProcessWorkingDirectory)( handle, dirPath.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetExternalProcessWorkingDirectory" );
      dirPath.ResizeToNullTerminated();
   }
   return dirPath;
}

// ----------------------------------------------------------------------------

void ExternalProcess::SetWorkingDirectory( const String& dirPath )
{
   if ( (*API->ExternalProcess->SetExternalProcessWorkingDirectory)( handle, dirPath.c_str() ) == api_false )
      throw APIFunctionError( "SetExternalProcessWorkingDirectory" );
}

// ----------------------------------------------------------------------------

bool ExternalProcess::IsRunning() const
{
   return (*API->ExternalProcess->GetExternalProcessIsRunning)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::IsStarting() const
{
   return (*API->ExternalProcess->GetExternalProcessIsStarting)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::HasCrashed() const
{
   return (*API->ExternalProcess->GetExternalProcessExitStatus)( handle ) == ExternalProcessContext::CrashedExit;
}

// ----------------------------------------------------------------------------

ExternalProcess::pid_type ExternalProcess::PID() const
{
   return pid_type( (*API->ExternalProcess->GetExternalProcessPID)( handle ) );
}

// ----------------------------------------------------------------------------

int ExternalProcess::ExitCode() const
{
   return (*API->ExternalProcess->GetExternalProcessExitCode)( handle );
}

// ----------------------------------------------------------------------------

size_type ExternalProcess::BytesAvailable() const
{
   return (*API->ExternalProcess->GetExternalProcessBytesAvailable)( handle );
}

// ----------------------------------------------------------------------------

size_type ExternalProcess::BytesToWrite() const
{
   return (*API->ExternalProcess->GetExternalProcessBytesToWrite)( handle );
}

// ----------------------------------------------------------------------------

ByteArray ExternalProcess::StandardOutput()
{
   /*
    * ### NB: ExternalProcess::ReadFromExternalProcess allocates the read data
    *         in the module's heap.
    */
   uint8* data = 0;
   size_type size = 0;
   if ( (*API->ExternalProcess->ReadFromExternalProcess)( ModuleHandle(), handle,
               ExternalProcessContext::StandardOutput, reinterpret_cast<void**>( &data ), &size ) == api_false )
      throw APIFunctionError( "ReadFromExternalProcess" );
   ByteArray b;
   b.Import( data, data+size );
   return b;
}

// ----------------------------------------------------------------------------

ByteArray ExternalProcess::StandardError()
{
   /*
    * ### NB: ExternalProcess::ReadFromExternalProcess allocates the read data
    *         in the module's heap.
    */
   uint8* data = 0;
   size_type size = 0;
   if ( (*API->ExternalProcess->ReadFromExternalProcess)( ModuleHandle(), handle,
               ExternalProcessContext::StandardError, reinterpret_cast<void**>( &data ), &size ) == api_false )
      throw APIFunctionError( "ReadFromExternalProcess" );
   ByteArray b;
   b.Import( data, data+size );
   return b;
}

// ----------------------------------------------------------------------------

ByteArray ExternalProcess::Read()
{
   /*
    * ### NB: ExternalProcess::ReadFromExternalProcess allocates the read data
    *         in the module's heap.
    */
   uint8* data = 0;
   size_type size = 0;
   if ( (*API->ExternalProcess->ReadFromExternalProcess)( ModuleHandle(), handle,
               ExternalProcessContext::CombinedOutput, reinterpret_cast<void**>( &data ), &size ) == api_false )
      throw APIFunctionError( "ReadFromExternalProcess" );
   ByteArray b;
   b.Import( data, data+size );
   return b;
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const ByteArray& data )
{
   if ( !data.IsEmpty() )
      if ( (*API->ExternalProcess->WriteToExternalProcess)( handle, data.Begin(), data.Length() ) == api_false )
         throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const String& text )
{
   if ( !text.IsEmpty() )
      if ( (*API->ExternalProcess->WriteToExternalProcess)( handle, text.c_str(), text.Size() ) == api_false )
         throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const IsoString& text )
{
   if ( !text.IsEmpty() )
      if ( (*API->ExternalProcess->WriteToExternalProcess)( handle, text.c_str(), text.Size() ) == api_false )
         throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const char* text )
{
   if ( text != nullptr )
      if ( *text != '\0' )
         if ( (*API->ExternalProcess->WriteToExternalProcess)( handle, text, strlen( text ) ) == api_false )
            throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const void* data, size_type count )
{
   if ( data != nullptr )
      if ( count > 0 )
         if ( (*API->ExternalProcess->WriteToExternalProcess)( handle, data, count ) == api_false )
            throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

StringList ExternalProcess::Environment() const
{
   StringList environment;
   if ( (*API->ExternalProcess->EnumerateExternalProcessEnvironment)( handle,
                        ExternalProcessPrivate::EnvironmentEnumerationCallback, &environment ) == api_false )
      return StringList();
   return environment;
}

// ----------------------------------------------------------------------------

void ExternalProcess::SetEnvironment( const StringList& environment )
{
   Array<const char16_type*> vars;
   for ( const String& env : environment )
      vars.Add( env.c_str() );
   if ( (*API->ExternalProcess->SetExternalProcessEnvironment)( handle, vars.Begin(), vars.Length() ) == api_false )
      throw APIFunctionError( "SetExternalProcessEnvironment" );
}

// ----------------------------------------------------------------------------

int ExternalProcess::ExecuteProgram( const String& program, const StringList& arguments )
{
   Array<const char16_type*> argv;
   for ( const String& arg : arguments )
      argv.Add( arg.c_str() );
   int retVal = (*API->ExternalProcess->ExecuteProgram)( program.c_str(), argv.Begin(), argv.Length() );
   if ( retVal < -1 )
      ExternalProcessPrivate::Throw( ExternalProcessContext::FailedToStart );
   return retVal;
}

// ----------------------------------------------------------------------------

ExternalProcess::pid_type ExternalProcess::StartProgram( const String& program, const StringList& arguments, const String& workingDirectory )
{
   Array<const char16_type*> argv;
   for ( const String& arg : arguments )
      argv.Add( arg.c_str() );
   uint64 pid = 0;
   api_bool ok = (*API->ExternalProcess->StartProgram)( program.c_str(), argv.Begin(), argv.Length(), workingDirectory.c_str(), &pid );
   if ( ok == api_false || pid == 0 )
      ExternalProcessPrivate::Throw( ExternalProcessContext::FailedToStart );
   return pid_type( pid );
}

// ----------------------------------------------------------------------------

void* ExternalProcess::CloneHandle() const
{
   throw Error( "Cannot clone an ExternalProcess handle" );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ExternalProcess*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ExternalProcessEventDispatcher
{
public:

   static void Started( external_process_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onStarted != nullptr )
         (receiver->*handlers->onStarted)( *sender );
   }

   static void Finished( external_process_handle hSender, control_handle hReceiver, int32 exitCode, int32 exitStatus )
   {
      if ( handlers->onFinished != nullptr )
         (receiver->*handlers->onFinished)( *sender, exitCode, exitStatus == ExternalProcessContext::NormalExit );
   }

   static void StandardOutputDataAvailable( external_process_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onStandardOutputDataAvailable != nullptr )
         (receiver->*handlers->onStandardOutputDataAvailable)( *sender );
   }

   static void StandardErrorDataAvailable( external_process_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onStandardErrorDataAvailable != nullptr )
         (receiver->*handlers->onStandardErrorDataAvailable)( *sender );
   }

   static void Error( external_process_handle hSender, control_handle hReceiver, int32 errorCode )
   {
      if ( handlers->onError != nullptr )
         (receiver->*handlers->onError)( *sender, ExternalProcess::error_code( errorCode ) );
   }

}; // ExternalProcessEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void ExternalProcess::OnStarted( process_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ExternalProcess->SetExternalProcessStartedEventRoutine)( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::Started : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessStartedEventRoutine" );
   m_handlers->onStarted = handler;
}

void ExternalProcess::OnFinished( process_exit_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ExternalProcess->SetExternalProcessFinishedEventRoutine)( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::Finished : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessFinishedEventRoutine" );
   m_handlers->onFinished = handler;
}

void ExternalProcess::OnStandardOutputDataAvailable( process_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ExternalProcess->SetExternalProcessStandardOutputDataAvailableEventRoutine)( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::StandardOutputDataAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessStandardOutputDataAvailableEventRoutine" );
   m_handlers->onStandardOutputDataAvailable = handler;
}

void ExternalProcess::OnStandardErrorDataAvailable( process_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ExternalProcess->SetExternalProcessStandardErrorDataAvailableEventRoutine)( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::StandardErrorDataAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessStandardErrorDataAvailableEventRoutine" );
   m_handlers->onStandardErrorDataAvailable = handler;
}

void ExternalProcess::OnError( process_error_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ExternalProcess->SetExternalProcessErrorEventRoutine)( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::Error : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessErrorEventRoutine" );
   m_handlers->onError = handler;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ExternalProcess.cpp - Released 2017-08-01T14:23:38Z
