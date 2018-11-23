//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/MultiViewSelectionDialog.cpp - Released 2018-11-23T16:14:31Z
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

#include <pcl/MultiViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MultiViewSelectionDialog::MultiViewSelectionDialog( bool allowPreviews ) :
   Dialog(),
   m_selectedViews(),
   m_allowPreviews( allowPreviews )
{
   SetSizer( Global_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Views_TreeBox );
   Global_Sizer.Add( Row2_Sizer );

   Row2_Sizer.SetSpacing( 8 );
   Row2_Sizer.Add( SelectAll_PushButton );
   Row2_Sizer.Add( UnselectAll_PushButton );
   Row2_Sizer.AddStretch();

   if ( m_allowPreviews )
   {
      Row2_Sizer.Add( IncludePreviews_CheckBox );
      Row2_Sizer.Add( IncludeMainViews_CheckBox );
   }

   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( Buttons_Sizer );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Views_TreeBox.SetNumberOfColumns( 1 );
   Views_TreeBox.HideHeader();
   Views_TreeBox.DisableRootDecoration();
   Views_TreeBox.EnableAlternateRowColor();
   Views_TreeBox.SetScaledMinHeight( 300 );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   UnselectAll_PushButton.SetText( "Unselect All" );
   UnselectAll_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   if ( m_allowPreviews )
   {
      IncludeMainViews_CheckBox.SetText( "Main views" );
      IncludeMainViews_CheckBox.SetToolTip( "<p>Include main views in the list of selectable views.</p>" );
      IncludeMainViews_CheckBox.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::OptionClick, *this );
      IncludeMainViews_CheckBox.SetChecked();

      IncludePreviews_CheckBox.SetText( "Previews" );
      IncludePreviews_CheckBox.SetToolTip( "<p>Include previews in the list of selectable views.</p>" );
      IncludePreviews_CheckBox.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::OptionClick, *this );
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
   OK_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   SetWindowTitle( "Select Views" );

   OnShow( (Control::event_handler)&MultiViewSelectionDialog::ControlShow, *this );

   Regenerate();
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::Regenerate()
{
   Views_TreeBox.Clear();

   Array<View> views = View::AllViews();

   bool includeMainViews = !m_allowPreviews || IncludeMainViews_CheckBox.IsChecked();
   bool includePreviews = m_allowPreviews && IncludePreviews_CheckBox.IsChecked();

   for ( const View& view : views )
   {
      bool isPreview = view.IsPreview();
      if ( isPreview ? includePreviews : includeMainViews )
      {
         TreeBox::Node* node = new TreeBox::Node( Views_TreeBox );
         node->SetCheckable();
         node->Uncheck();
         node->SetText( 0, view.FullId() );
      }
   }

   Views_TreeBox.AdjustColumnWidthToContents( 0 );
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::OptionClick( Button& sender, bool checked )
{
   Regenerate();
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::ButtonClick( Button& sender, bool checked )
{
   if ( sender == SelectAll_PushButton )
   {
      for ( int i = 0, n = Views_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         TreeBox::Node* node = Views_TreeBox[i];
         if ( node != nullptr )
            node->Check();
      }
   }
   else if ( sender == UnselectAll_PushButton )
   {
      for ( int i = 0, n = Views_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         TreeBox::Node* node = Views_TreeBox[i];
         if ( node != nullptr )
            node->Uncheck();
      }
   }
   else if ( sender == OK_PushButton )
   {
      m_selectedViews.Clear();
      for ( int i = 0, n = Views_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         const TreeBox::Node* node = Views_TreeBox[i];
         if ( node != nullptr && node->IsChecked() )
         {
            View view = View::ViewById( node->Text( 0 ) );
            if ( !view.IsNull() )
               m_selectedViews.Add( view );
         }
      }
      Ok();
   }
   else if ( sender == Cancel_PushButton )
   {
      m_selectedViews.Clear();
      Cancel();
   }
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::ControlShow( Control& sender )
{
   AdjustToContents();
   SetMinSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MultiViewSelectionDialog.cpp - Released 2018-11-23T16:14:31Z
