//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/Dialog.cpp - Released 2018-11-01T11:06:52Z
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

#include <pcl/Dialog.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Dialog::Dialog( Control& parent ) :
   Control( (*API->Dialog->CreateDialog)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateDialog" );
}

// ----------------------------------------------------------------------------

int Dialog::Execute()
{
   return (*API->Dialog->ExecuteDialog)( handle );
}

// ----------------------------------------------------------------------------

void Dialog::Open()
{
   (*API->Dialog->OpenDialog)( handle );
}

// ----------------------------------------------------------------------------

void Dialog::Return( int retVal )
{
   (*API->Dialog->ReturnDialog)( handle, retVal );
}

// ----------------------------------------------------------------------------

bool Dialog::IsUserResizable() const
{
   return (*API->Dialog->GetDialogResizable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Dialog::EnableUserResizing( bool enable )
{
   (*API->Dialog->SetDialogResizable)( handle, enable );
}

// ----------------------------------------------------------------------------

void Dialog::ProcessEvents( bool excludeUserInputEvents )
{
   if ( Module != nullptr )
      Module->ProcessEvents( excludeUserInputEvents );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Dialog*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class DialogEventDispatcher
{
public:

   static void Executed( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onExecute != nullptr )
         (receiver->*handlers->onExecute)( *sender );
   }

   static void Returned( control_handle hSender, control_handle hReceiver, int32 retVal )
   {
      if ( handlers->onReturn != nullptr )
         (receiver->*handlers->onReturn)( *sender, retVal );
   }
}; // DialogEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void Dialog::OnExecute( execute_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Dialog->SetExecuteDialogEventRoutine)( handle, &receiver,
                  (f != nullptr) ? DialogEventDispatcher::Executed : nullptr ) == api_false )
      throw APIFunctionError( "SetExecuteDialogEventRoutine" );
   m_handlers->onExecute = f;
}

void Dialog::OnReturn( return_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Dialog->SetReturnDialogEventRoutine)( handle, &receiver,
                  (f != nullptr) ? DialogEventDispatcher::Returned : nullptr ) == api_false )
      throw APIFunctionError( "SetReturnDialogEventRoutine" );
   m_handlers->onReturn = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Dialog.cpp - Released 2018-11-01T11:06:52Z
