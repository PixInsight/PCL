//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/GroupBox.cpp - Released 2017-05-28T08:29:05Z
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

#include <pcl/GroupBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

GroupBox::GroupBox( const String& title, Control& parent ) :
   Control( (*API->GroupBox->CreateGroupBox)( ModuleHandle(), this, title.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateGroupBox" );
}

// ----------------------------------------------------------------------------

String GroupBox::Title() const
{
   size_type len = 0;
   (*API->GroupBox->GetGroupBoxTitle)( handle, 0, &len );

   String title;
   if ( len > 0 )
   {
      title.SetLength( len );
      if ( (*API->GroupBox->GetGroupBoxTitle)( handle, title.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetGroupBoxTitle" );
      title.ResizeToNullTerminated();
   }
   return title;
}

// ----------------------------------------------------------------------------

void GroupBox::SetTitle( const String& title )
{
   (*API->GroupBox->SetGroupBoxTitle)( handle, title.c_str() );
}

// ----------------------------------------------------------------------------

bool GroupBox::HasTitleCheckBox() const
{
   return (*API->GroupBox->GetGroupBoxCheckable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GroupBox::EnableTitleCheckBox( bool enabled )
{
   (*API->GroupBox->SetGroupBoxCheckable)( handle, enabled );
}

// ----------------------------------------------------------------------------

bool GroupBox::IsChecked() const
{
   return (*API->GroupBox->GetGroupBoxChecked)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GroupBox::SetChecked( bool checked )
{
   (*API->GroupBox->SetGroupBoxChecked)( handle, checked );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<GroupBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class GroupBoxEventDispatcher
{
public:

   static void api_func Check( control_handle hSender, control_handle hReceiver, int32 state )
   {
      if ( handlers->onCheck != nullptr )
         (receiver->*handlers->onCheck)( *sender, state != 0 );
   }
}; // GroupBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void GroupBox::OnCheck( check_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->GroupBox->SetGroupBoxCheckEventRoutine)( handle, &receiver,
                  (f != nullptr) ? GroupBoxEventDispatcher::Check : nullptr ) == api_false )
      throw APIFunctionError( "SetGroupBoxCheckEventRoutine" );
   m_handlers->onCheck = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/GroupBox.cpp - Released 2017-05-28T08:29:05Z
