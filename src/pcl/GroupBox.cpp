// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/GroupBox.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/GroupBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<GroupBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class GroupBoxEventDispatcher
{
public:

   static void api_func Check( control_handle hSender, control_handle hReceiver, int32 state )
   {
      if ( sender->onCheck != 0 )
         (receiver->*sender->onCheck)( *sender, state != 0 );
   }
}; // GroupBoxEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

GroupBox::GroupBox( const String& title, Control& parent ) :
Control( (*API->GroupBox->CreateGroupBox)(
            ModuleHandle(), this, title.c_str(), parent.handle, 0 /*flags*/ ) ),
onCheck( 0 )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateGroupBox" );
}

// ----------------------------------------------------------------------------

void GroupBox::OnCheck( check_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onCheck = 0;
   if ( (*API->GroupBox->SetGroupBoxCheckEventRoutine)( handle, &receiver,
            (f != 0) ? GroupBoxEventDispatcher::Check : 0 ) == api_false )
   {
      throw APIFunctionError( "SetGroupBoxCheckEventRoutine" );
   }
   onCheck = f;
}

// ----------------------------------------------------------------------------

String GroupBox::Title() const
{
   size_type len = 0;
   (*API->GroupBox->GetGroupBoxTitle)( handle, 0, &len );

   String title;

   if ( len != 0 )
   {
      title.Reserve( len );

      if ( (*API->GroupBox->GetGroupBoxTitle)( handle, title.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetGroupBoxTitle" );
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

} // pcl

// ****************************************************************************
// EOF pcl/GroupBox.cpp - Released 2014/11/14 17:17:00 UTC
