//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/Console.cpp - Released 2015/10/08 11:24:19 UTC
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

#include <pcl/Console.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Console::Console() : m_handle( 0 ), m_thread( 0 )
{
   if ( (m_handle = (*API->Global->GetConsole)()) == 0 )
      throw APIFunctionError( "GetConsole" );

   m_thread = (*API->Thread->GetCurrentThread)();
}

Console::~Console()
{
   m_thread = 0;
}

void Console::Write( const String& s )
{
   if ( m_thread == 0 )
   {
      if ( (*API->Global->WriteConsole)( m_handle, s.c_str(), api_false ) == api_false )
         throw APIFunctionError( "WriteConsole" );
   }
   else
      (*API->Thread->AppendThreadConsoleOutputText)( m_thread, s.c_str(), api_false );
}

void Console::WriteLn( const String& s )
{
   if ( m_thread == 0 )
   {
      if ( (*API->Global->WriteConsole)( m_handle, s.c_str(), api_true ) == api_false )
         throw APIFunctionError( "WriteConsole" );
   }
   else
      (*API->Thread->AppendThreadConsoleOutputText)( m_thread, s.c_str(), api_true );
}

void Console::WriteLn()
{
   if ( m_thread == 0 )
   {
      if ( (*API->Global->WriteConsole)( m_handle, 0, api_true ) == api_false )
         throw APIFunctionError( "WriteConsole" );
   }
   else
      (*API->Thread->AppendThreadConsoleOutputText)( m_thread, 0, api_true );
}

// ----------------------------------------------------------------------------

int Console::ReadChar()
{
   if ( m_thread == 0 )
      return (*API->Global->ReadConsoleChar)( m_handle );
   return 0;
}

// ----------------------------------------------------------------------------

String Console::ReadString()
{
   if ( m_thread == 0 )
   {
      char16_type* s = (*API->Global->ReadConsoleString)( ModuleHandle(), m_handle );
      if ( s != 0 )
      {
         String str( s );
         Module->Deallocate( s );
         return str;
      }
   }
   return String();
}

// ----------------------------------------------------------------------------

/*
 *
 * ### DISABLED FUNCTION - DO NOT ENABLE FOR PRODUCTION MODULES ###
 *
 *
String Console::Text() const
{
   if ( m_thread == 0 )
   {
      char16_type* s = (*API->Global->GetConsoleText)( ModuleHandle(), m_handle );
      if ( s != 0 )
      {
         String str( s );
         Module->Deallocate( s );
         return str;
      }
   }
   return String();
}
 */

// ----------------------------------------------------------------------------

bool Console::Suspended() const
{
   return ((*API->Global->GetProcessStatus)() & 0x00000002) != 0;
}

// ----------------------------------------------------------------------------

bool Console::Waiting() const
{
   return ((*API->Global->GetProcessStatus)() & 0x00000004) != 0;
}

// ----------------------------------------------------------------------------

bool Console::AbortEnabled() const
{
   return ((*API->Global->GetProcessStatus)() & 0x40000000) != 0;
}

// ----------------------------------------------------------------------------

bool Console::AbortRequested() const
{
   return ((*API->Global->GetProcessStatus)() & 0x80000000) != 0;
}

// ----------------------------------------------------------------------------

void Console::ResetStatus()
{
   if ( m_thread == 0 )
      if ( (*API->Global->ResetProcessStatus)() == api_false )
         throw APIFunctionError( "ResetProcessStatus" );
}

// ----------------------------------------------------------------------------

void Console::EnableAbort()
{
   if ( m_thread == 0 )
      if ( (*API->Global->EnableAbort)() == api_false )
         throw APIFunctionError( "EnableAbort" );
}

// ----------------------------------------------------------------------------

void Console::DisableAbort()
{
   if ( m_thread == 0 )
      if ( (*API->Global->DisableAbort)() == api_false )
         throw APIFunctionError( "DisableAbort" );
}

// ----------------------------------------------------------------------------

void Console::Abort()
{
   if ( m_thread == 0 )
      if ( (*API->Global->Abort)() == api_false )
         throw APIFunctionError( "Abort" );
}

// ----------------------------------------------------------------------------

bool Console::IsValid() const
{
   return m_handle != 0 && (*API->Global->ValidateConsole)( m_handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool Console::IsCurrentThreadConsole() const
{
   return m_handle != 0 && m_thread == (*API->Thread->GetCurrentThread)();
}

// ----------------------------------------------------------------------------

void Console::Flush()
{
   if ( m_thread == 0 )
      if ( (*API->Global->FlushConsole)( m_handle ) == api_false )
         throw APIFunctionError( "FlushConsole" );
}

// ----------------------------------------------------------------------------

bool Console::Show( bool show )
{
   if ( m_thread == 0 )
      return (*API->Global->ShowConsole)( m_handle, api_bool( show ) ) != api_false;
   return false;
}

// ----------------------------------------------------------------------------

void Console::Clear()
{
   if ( m_thread == 0 )
      Write( "<clr>" );
}

// ----------------------------------------------------------------------------

/*
 *
 * ### DISABLED FUNCTION - DO NOT ENABLE FOR PRODUCTION MODULES ###
 *
 *
void Console::Execute( const String& command )
{
   if ( m_thread == 0 )
      if ( (*API->Global->ExecuteCommand)( ModuleHandle(), m_handle, command.c_str() ) == api_false )
         throw APIFunctionError( "ExecuteCommand" );
}
 */

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Console.cpp - Released 2015/10/08 11:24:19 UTC
