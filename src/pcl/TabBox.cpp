// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/TabBox.cpp - Released 2014/11/14 17:17:01 UTC
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

#include <pcl/TabBox.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<TabBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class TabBoxEventDispatcher
{
public:

   static void api_func PageSelected( control_handle hSender, control_handle hReceiver, int32 pageIndex )
   {
      if ( sender->onPageSelected != 0 )
         (receiver->*sender->onPageSelected)( *sender, pageIndex );
   }

}; // TabBoxEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

TabBox::TabBox( Control& parent ) :
Control( (*API->TabBox->CreateTabBox)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) ),
onPageSelected( 0 )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateTabBox" );
}

// ----------------------------------------------------------------------------

void TabBox::OnPageSelected( page_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onPageSelected = 0;
   if ( (*API->TabBox->SetTabBoxPageSelectedEventRoutine)( handle, &receiver,
               (f != 0) ? TabBoxEventDispatcher::PageSelected : 0 ) == api_false )
   {
      throw APIFunctionError( "SetTabBoxPageSelectedEventRoutine" );
   }
   onPageSelected = f;
}

// ----------------------------------------------------------------------------

int TabBox::NumberOfPages() const
{
   return (*API->TabBox->GetTabBoxLength)( handle );
}

// ----------------------------------------------------------------------------

int TabBox::CurrentPageIndex() const
{
   return (*API->TabBox->GetTabBoxCurrentPageIndex)( handle );
}

// ----------------------------------------------------------------------------

void TabBox::SetCurrentPageIndex( int idx )
{
   (*API->TabBox->SetTabBoxCurrentPageIndex)( handle, idx );
}

// ----------------------------------------------------------------------------

Control& TabBox::CurrentPageControl() const
{
   int i = CurrentPageIndex();
   return (i >= 0) ? PageControlByIndex( i ) : Control::Null();
}

// ----------------------------------------------------------------------------

Control& TabBox::PageControlByIndex( int idx ) const
{
   Control* w = reinterpret_cast<Control*>( (*API->TabBox->GetTabBoxPageByIndex)( handle, idx ) );
   return (w != 0) ? *w : Control::Null();
}

// ----------------------------------------------------------------------------

void TabBox::InsertPage( int idx, Control& page, const String& label, const Bitmap& icon )
{
   (*API->TabBox->InsertTabBoxPage)( handle, idx, page.handle, label.c_str(), icon.handle );
}

// ----------------------------------------------------------------------------

void TabBox::RemovePage( int idx )
{
   (*API->TabBox->RemoveTabBoxPage)( handle, idx );
}

// ----------------------------------------------------------------------------

TabBox::tab_position TabBox::TabPosition() const
{
   return tab_position( (*API->TabBox->GetTabBoxPosition)( handle ) );
}

// ----------------------------------------------------------------------------

void TabBox::SetTabPosition( TabBox::tab_position pos )
{
   (*API->TabBox->SetTabBoxPosition)( handle, pos );
}

// ----------------------------------------------------------------------------

bool TabBox::IsPageEnabled( int idx ) const
{
   return (*API->TabBox->GetTabBoxPageEnabled)( handle, idx ) != api_false;
}

// ----------------------------------------------------------------------------

void TabBox::EnablePage( int idx, bool enable )
{
   (*API->TabBox->SetTabBoxPageEnabled)( handle, idx, enable );
}

// ----------------------------------------------------------------------------

String TabBox::PageLabel( int idx ) const
{
   size_type len = 0;
   (*API->TabBox->GetTabBoxPageLabel)( handle, idx, 0, &len );

   String label;

   if ( len != 0 )
   {
      label.Reserve( len );

      if ( (*API->TabBox->GetTabBoxPageLabel)( handle, idx, label.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetTabBoxPageLabel" );
   }

   return label;
}

// ----------------------------------------------------------------------------

void TabBox::SetPageLabel( int idx, const String& label )
{
   (*API->TabBox->SetTabBoxPageLabel)( handle, idx, label.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap TabBox::PageIcon( int idx ) const
{
   return Bitmap( (*API->TabBox->GetTabBoxPageIcon)( handle, idx ) );
}

// ----------------------------------------------------------------------------

void TabBox::SetPageIcon( int idx, const Bitmap& pm )
{
   (*API->TabBox->SetTabBoxPageIcon)( handle, idx, pm.handle );
}

// ----------------------------------------------------------------------------

String TabBox::PageToolTip( int idx ) const
{
   size_type len = 0;
   (*API->TabBox->GetTabBoxPageToolTip)( handle, idx, 0, &len );

   String tip;

   if ( len != 0 )
   {
      tip.Reserve( len );

      if ( (*API->TabBox->GetTabBoxPageToolTip)( handle, idx, tip.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetTabBoxPageToolTip" );
   }

   return tip;
}

// ----------------------------------------------------------------------------

void TabBox::SetPageToolTip( int idx, const String& toolTip )
{
   (*API->TabBox->SetTabBoxPageToolTip)( handle, idx, toolTip.c_str() );
}

// ----------------------------------------------------------------------------

Control& TabBox::LeftControl() const
{
   Control* w = reinterpret_cast<Control*>( (*API->TabBox->GetTabBoxLeftControl)( handle ) );
   return (w != 0) ? *w : Control::Null();
}

// ----------------------------------------------------------------------------

Control& TabBox::RightControl() const
{
   Control* w = reinterpret_cast<Control*>( (*API->TabBox->GetTabBoxRightControl)( handle ) );
   return (w != 0) ? *w : Control::Null();
}

// ----------------------------------------------------------------------------

void TabBox::SetControls( Control& left, Control& right )
{
   (*API->TabBox->SetTabBoxControls)( handle, left.handle, right.handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/TabBox.cpp - Released 2014/11/14 17:17:01 UTC
