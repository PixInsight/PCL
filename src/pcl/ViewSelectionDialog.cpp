//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/ViewSelectionDialog.cpp - Released 2018-12-12T09:24:30Z
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

#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ViewSelectionDialog::ViewSelectionDialog( const IsoString& id, bool allowPreviews ) :
   Dialog(),
   m_id( id ),
   m_allowPreviews( allowPreviews )
{
   SetSizer( Global_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Images_ViewList );

   if ( m_allowPreviews )
   {
      Global_Sizer.Add( IncludeMainViews_CheckBox );
      Global_Sizer.Add( IncludePreviews_CheckBox );
   }

   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( Buttons_Sizer );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   if ( m_allowPreviews )
      Images_ViewList.GetAll();
   else
      Images_ViewList.GetMainViews();

   Images_ViewList.SelectView( View::ViewById( m_id ) );
   Images_ViewList.SetMinWidth( Font().Width( String( 'M', 40 ) ) );
   Images_ViewList.OnViewSelected( (ViewList::view_event_handler)&ViewSelectionDialog::ViewSelected, *this );

   if ( m_allowPreviews )
   {
      IncludeMainViews_CheckBox.SetText( "Include main views" );
      IncludeMainViews_CheckBox.OnClick( (Button::click_event_handler)&ViewSelectionDialog::OptionClick, *this );
      IncludeMainViews_CheckBox.SetChecked();

      IncludePreviews_CheckBox.SetText( "Include previews" );
      IncludePreviews_CheckBox.OnClick( (Button::click_event_handler)&ViewSelectionDialog::OptionClick, *this );
      IncludePreviews_CheckBox.SetChecked();
   }
   else
   {
      IncludeMainViews_CheckBox.Hide();
      IncludePreviews_CheckBox.Hide();
   }

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&ViewSelectionDialog::ButtonClick, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&ViewSelectionDialog::ButtonClick, *this );

   SetWindowTitle( "Select View" );

   OnShow( (Control::event_handler)&ViewSelectionDialog::ControlShow, *this );
}

void ViewSelectionDialog::ViewSelected( ViewList& sender, View& view )
{
   m_id = view.IsNull() ? IsoString() : view.FullId();
}

void ViewSelectionDialog::OptionClick( Button& sender, bool checked )
{
   bool includeMainViews = !m_allowPreviews || IncludeMainViews_CheckBox.IsChecked();
   bool includePreviews = m_allowPreviews && IncludePreviews_CheckBox.IsChecked();
   Images_ViewList.Regenerate( includeMainViews, includePreviews );
   Images_ViewList.SelectView( View::ViewById( m_id ) );
}

void ViewSelectionDialog::ButtonClick( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void ViewSelectionDialog::ControlShow( Control& sender )
{
   AdjustToContents();
   SetMinWidth();
   SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ViewSelectionDialog.cpp - Released 2018-12-12T09:24:30Z
