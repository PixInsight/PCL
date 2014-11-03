// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ComboBox.cpp - Released 2014/10/29 07:34:21 UTC
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

#include <pcl/ComboBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ComboBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class ComboBoxEventDispatcher
{
public:

   static void api_func ItemSelected( control_handle hSender, control_handle hReceiver, int32 itemIndex )
   {
      if ( sender->onItemSelected != 0 )
         (receiver->*sender->onItemSelected)( *sender, itemIndex );
   }

   static void api_func ItemHighlighted( control_handle hSender, control_handle hReceiver, int32 itemIndex )
   {
      if ( sender->onItemHighlighted != 0 )
         (receiver->*sender->onItemHighlighted)( *sender, itemIndex );
   }

   static void api_func EditTextUpdated( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onEditTextUpdated != 0 )
         (receiver->*sender->onEditTextUpdated)( *sender );
   }
}; // ComboBoxEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

ComboBox::ComboBox( Control& parent ) :
Control( (*API->ComboBox->CreateComboBox)( ModuleHandle(), this, parent.handle, 0 /*flags*/ ) ),
onItemSelected( 0 ),
onItemHighlighted( 0 ),
onEditTextUpdated( 0 )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateComboBox" );
}

// ----------------------------------------------------------------------------

void ComboBox::OnItemSelected( item_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onItemSelected = 0;
   if ( (*API->ComboBox->SetComboBoxItemSelectedEventRoutine)( handle, &receiver,
        (f != 0) ? ComboBoxEventDispatcher::ItemSelected : 0 ) == api_false )
   {
      throw APIFunctionError( "SetComboBoxItemSelectedEventRoutine" );
   }
   onItemSelected = f;
}

void ComboBox::OnItemHighlighted( item_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onItemHighlighted = 0;
   if ( (*API->ComboBox->SetComboBoxItemHighlightedEventRoutine)( handle, &receiver,
        (f != 0) ? ComboBoxEventDispatcher::ItemHighlighted : 0 ) == api_false )
   {
      throw APIFunctionError( "SetComboBoxItemHighlightedEventRoutine" );
   }
   onItemHighlighted = f;
}

void ComboBox::OnEditTextUpdated( edit_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onEditTextUpdated = 0;
   if ( (*API->ComboBox->SetComboBoxEditTextUpdatedEventRoutine)( handle, &receiver,
        (f != 0) ? ComboBoxEventDispatcher::EditTextUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetComboBoxEditTextUpdatedEventRoutine" );
   }
   onEditTextUpdated = f;
}

// ----------------------------------------------------------------------------

int ComboBox::NumberOfItems() const
{
   return (*API->ComboBox->GetComboBoxLength)( handle );
}

// ----------------------------------------------------------------------------

int ComboBox::CurrentItem() const
{
   return (*API->ComboBox->GetComboBoxCurrentItem)( handle );
}

// ----------------------------------------------------------------------------

void ComboBox::SetCurrentItem( int idx )
{
   (*API->ComboBox->SetComboBoxCurrentItem)( handle, idx );
}

// ----------------------------------------------------------------------------

int ComboBox::FindItem( const String& text, int fromIdx, bool exactMatch, bool caseSensitive ) const
{
   return (*API->ComboBox->FindComboBoxItem)( handle, text.c_str(), fromIdx, exactMatch, caseSensitive );
}

// ----------------------------------------------------------------------------

void ComboBox::InsertItem( int idx, const String& text, const Bitmap& icon )
{
   (*API->ComboBox->InsertComboBoxItem)( handle, idx, text.c_str(), icon.handle );
}

// ----------------------------------------------------------------------------

void ComboBox::RemoveItem( int idx )
{
   (*API->ComboBox->RemoveComboBoxItem)( handle, idx );
}

// ----------------------------------------------------------------------------

void ComboBox::Clear()
{
   (*API->ComboBox->ClearComboBox)( handle );
}

// ----------------------------------------------------------------------------

String ComboBox::ItemText( int idx ) const
{
   size_type len = 0;
   (*API->ComboBox->GetComboBoxItemText)( handle, idx, 0, &len );

   String text;

   if ( len != 0 )
   {
      text.Reserve( len );

      if ( (*API->ComboBox->GetComboBoxItemText)( handle, idx, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetComboBoxItemText" );
   }

   return text;
}

// ----------------------------------------------------------------------------

void ComboBox::SetItemText( int idx, const String& text )
{
   (*API->ComboBox->SetComboBoxItemText)( handle, idx, text.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap ComboBox::ItemIcon( int idx ) const
{
   return Bitmap( (*API->ComboBox->GetComboBoxItemIcon)( handle, idx ) );
}

// ----------------------------------------------------------------------------

void ComboBox::SetItemIcon( int idx, const Bitmap& icon )
{
   (*API->ComboBox->SetComboBoxItemIcon)( handle, idx, icon.handle );
}

// ----------------------------------------------------------------------------

bool ComboBox::IsEditEnabled() const
{
   return (*API->ComboBox->GetComboBoxEditEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ComboBox::EnableEdit( bool enable )
{
   (*API->ComboBox->SetComboBoxEditEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

String ComboBox::EditText() const
{
   size_type len = 0;
   (*API->ComboBox->GetComboBoxEditText)( handle, 0, &len );

   String text;

   if ( len != 0 )
   {
      text.Reserve( len );

      if ( (*API->ComboBox->GetComboBoxEditText)( handle, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetComboBoxEditText" );
   }

   return text;
}

// ----------------------------------------------------------------------------

void ComboBox::SetEditText( const String& text )
{
   (*API->ComboBox->SetComboBoxEditText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool ComboBox::IsAutoCompletionEnabled() const
{
   return (*API->ComboBox->GetComboBoxAutoCompletionEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ComboBox::EnableAutoCompletion( bool enable )
{
   (*API->ComboBox->SetComboBoxAutoCompletionEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

void ComboBox::GetIconSize( int& width, int& height ) const
{
   (*API->ComboBox->GetComboBoxIconSize)( handle, &width, &height );
}

// ----------------------------------------------------------------------------

void ComboBox::SetIconSize( int width, int height )
{
   (*API->ComboBox->SetComboBoxIconSize)( handle, width, height );
}

// ----------------------------------------------------------------------------

int ComboBox::MaxVisibleItemCount() const
{
   return (*API->ComboBox->GetComboBoxMaxVisibleItemCount)( handle );
}

// ----------------------------------------------------------------------------

void ComboBox::SetMaxVisibleItemCount( int n )
{
   (*API->ComboBox->SetComboBoxMaxVisibleItemCount)( handle, n );
}

// ----------------------------------------------------------------------------

int ComboBox::MinItemCharWidth() const
{
   return (*API->ComboBox->GetComboBoxMinItemCharWidth)( handle );
}

// ----------------------------------------------------------------------------

void ComboBox::SetMinItemCharWidth( int n )
{
   (*API->ComboBox->SetComboBoxMinItemCharWidth)( handle, n );
}

// ----------------------------------------------------------------------------

void ComboBox::ShowList()
{
   (*API->ComboBox->SetComboBoxListVisible)( handle, api_true );
}

// ----------------------------------------------------------------------------

void ComboBox::HideList()
{
   (*API->ComboBox->SetComboBoxListVisible)( handle, api_false );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/ComboBox.cpp - Released 2014/10/29 07:34:21 UTC
