//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0394
// ----------------------------------------------------------------------------
// FITSHeaderInterface.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "FITSHeaderInterface.h"
#include "FITSHeaderProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FITSHeaderInterface* TheFITSHeaderInterface = nullptr;

// ----------------------------------------------------------------------------

#include "FITSHeaderIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView  GUI->AllImages_ViewList.CurrentView()

// ----------------------------------------------------------------------------

FITSHeaderInterface::FITSHeaderInterface() :
   instance( TheFITSHeaderProcess )
{
   TheFITSHeaderInterface = this;
}

FITSHeaderInterface::~FITSHeaderInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString FITSHeaderInterface::Id() const
{
   return "FITSHeader";
}

MetaProcess* FITSHeaderInterface::Process() const
{
   return TheFITSHeaderProcess;
}

const char** FITSHeaderInterface::IconImageXPM() const
{
   return FITSHeaderIcon_XPM;
}

void FITSHeaderInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

InterfaceFeatures FITSHeaderInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

void FITSHeaderInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.MainView() );
         else
            UpdateControls();
      }
}

void FITSHeaderInterface::ResetInstance()
{
   FITSHeaderInstance defaultInstance( TheFITSHeaderProcess );
   ImportProcess( defaultInstance );
}

bool FITSHeaderInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FITSHeader" );
      GUI->AllImages_ViewList.Regenerate( true, false ); // exclude previews
      UpdateControls();
   }

   dynamic = false;
   return &P == TheFITSHeaderProcess;
}

ProcessImplementation* FITSHeaderInterface::NewProcess() const
{
   return new FITSHeaderInstance( instance );
}

bool FITSHeaderInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const FITSHeaderInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a FITSHeader instance.";
   return false;
}

bool FITSHeaderInterface::RequiresInstanceValidation() const
{
   return true;
}

bool FITSHeaderInterface::ImportProcess( const ProcessImplementation& p )
{
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );

   instance.Assign( p );
   UpdateControls();
   return true;
}

bool FITSHeaderInterface::WantsImageNotifications() const
{
   return true;
}

void FITSHeaderInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
      {
         instance.ImportKeywords( v.Window() );
         UpdateControls();
      }
}

void FITSHeaderInterface::ImageFocused( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( !v.IsNull() )
         {
            ImageWindow w = v.Window();

            // This is normally not necessary, but in this way we can invoke this
            // function from TrackViewUpdated().
            GUI->AllImages_ViewList.SelectView( w.MainView() );

            instance.ImportKeywords( v.Window() );
            UpdateControls();
         }
}

// ----------------------------------------------------------------------------

// This one is invoked by FITSHeaderAction.
void FITSHeaderInterface::SelectImage( const ImageWindow& w )
{
   bool firstTime = GUI == nullptr;

   bool dum1;
   unsigned dum2;
   (void)Launch( *TheFITSHeaderProcess, 0, dum1, dum2 );

   if ( firstTime )
      RestoreGeometry();

   if ( !w.IsNull() )
   {
      GUI->AllImages_ViewList.SelectView( w.MainView() );
      instance.ImportKeywords( w );
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void FITSHeaderInterface::UpdateControls()
{
   UpdateKeywordList();
   UpdateInfo();
   UpdateButtons();
}

void FITSHeaderInterface::UpdateKeywordList()
{
   GUI->Keywords_List.DisableUpdates();

   GUI->Keywords_List.Clear();
   for ( size_type i = 0; i < instance.keywords.Length(); ++i )
   {
      TreeBox::Node* node = new TreeBox::Node;
      GUI->Keywords_List.Add( node );
      UpdateKeywordNode( int( i ) );
   }

   GUI->Keywords_List.AdjustColumnWidthToContents( 0 );
   GUI->Keywords_List.AdjustColumnWidthToContents( 1 );
   GUI->Keywords_List.AdjustColumnWidthToContents( 2 );

   if ( GUI->Keywords_List.NumberOfChildren() > 0 )
      GUI->Keywords_List.SetCurrentNode( GUI->Keywords_List[0] );

   GUI->Keywords_List.EnableUpdates();
}

void FITSHeaderInterface::UpdateButtons()
{
   IndirectArray<TreeBox::Node> selected = GUI->Keywords_List.SelectedNodes();

   bool reserved = false;
   for ( IndirectArray<TreeBox::Node>::const_iterator i = selected.ReverseBegin(); i != selected.ReverseEnd(); --i )
   {
      int index = GUI->Keywords_List.ChildIndex( *i );
      if ( size_type( index ) >= instance.keywords.Length() ||
           FITSHeaderInstance::IsReservedKeyword( instance.keywords[index].name ) )
      {
         reserved = true;
         break;
      }
   }

   GUI->Remove_PushButton.Enable( !selected.IsEmpty() && !reserved );
   GUI->Replace_PushButton.Enable( selected.Length() == 1 && !reserved );
}

void FITSHeaderInterface::UpdateInfo()
{
   String info;

   TreeBox::Node* node = GUI->Keywords_List.CurrentNode();
   if ( node != nullptr )
   {
      int index = GUI->Keywords_List.ChildIndex( node );
      if ( index >= 0 )
         info.Format( "%d/%d", index+1, GUI->Keywords_List.NumberOfChildren() );
   }

   GUI->Info_Label.SetText( info );
}

void FITSHeaderInterface::UpdateKeywordNode( int i )
{
   TreeBox::Node* node = GUI->Keywords_List[i];
   if ( node == nullptr )
      return;

   bool reserved = false;

   if ( size_type( i ) >= instance.keywords.Length() )
   {
      node->SetText( 0, String() );
      node->SetText( 1, String() );
      node->SetText( 2, String() );
   }
   else
   {
      node->SetText( 0, instance.keywords[i].name );
      node->SetText( 1, instance.keywords[i].value );
      node->SetText( 2, instance.keywords[i].comment );

      reserved = FITSHeaderInstance::IsReservedKeyword( instance.keywords[i].name );
   }

   RGBA c = reserved ? RGBAColor( 200, 80, 80 ) : RGBAColor( 0, 0, 0 );
   node->SetTextColor( 0, c );
   node->SetTextColor( 1, c );
   node->SetTextColor( 2, c );
}

bool FITSHeaderInterface::GetKeywordData( FITSHeaderInstance::Keyword& k )
{
   int step = -1;

   try
   {
      ++step;

      k.name = GUI->Name_Edit.Text();
      k.name.Trim();

      if ( k.name.IsEmpty() )
         throw Error( "The keyword name cannot be empty." );

      if ( FITSHeaderInstance::IsReservedKeyword( k.name ) )
         throw Error( '\'' + k.name + "\' is a reserved FITS keyword that you cannot use." );

      if ( !GUI->HIERARCH_CheckBox.IsChecked() )
      {
         IsoString ascName( k.name );
         if ( strspn( ascName.c_str(), "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_" ) != ascName.Length() )
            throw Error( "Invalid character(s) in standard FITS keyword name." );
         if ( ascName.Length() > 8 )
            throw Error( "A standard FITS keyword name cannot have more than eight characters." );
      }

      ++step;

      k.value = GUI->Value_Edit.Text();
      k.value.Trim();

      if ( !k.value.IsEmpty() )
      {
         char c1 = k.value[0];

         if ( c1 == '\'' )
         {
            if ( !k.value.EndsWith( '\'' ) )
               k.value += '\'';
         }
         else if ( c1 == '\"' )
         {
            k.value[0] = '\'';

            if ( k.value.EndsWith( '\"' ) )
               k.value[k.value.Length()-1] = '\'';
            else
               k.value += '\'';
         }
         else if ( c1 == '.' || c1 == '-' || c1 == '+' || (c1 >= '0' && c1 <= '9') )
         {
            (void)k.value.ToDouble();
         }
         else if ( k.value.Length() != 1 || c1 != 'T' && c1 != 'F' )
         {
            k.value -= '\'';
            k.value += '\'';
         }
      }

      ++step;

      k.comment = GUI->Comment_Edit.Text();
      k.comment.Trim();

      // The maximum length of a FITS header card is 80 characters.
      // Three characters are reserved for the string " = " between key name and
      // value. This separator is present even if the value is empty.
      // If comment is not empty, the string " / " is written between value and
      // comment, wasting three additional characters.

      size_type totalLength = k.name.Length() + 3 + k.value.Length() +
                        (k.comment.IsEmpty() ? 0 : 3) + k.comment.Length();

      if ( totalLength > 80 )
         throw Error( "The total length of this FITS header \'card\' would exceed 80 characters." );

      return true;
   }

   ERROR_HANDLER

   Edit* e = nullptr;
   switch ( step )
   {
   case 0: e = &GUI->Name_Edit; break;
   case 1: e = &GUI->Value_Edit; break;
   case 2: e = &GUI->Comment_Edit; break;
   }
   if ( e != nullptr )
   {
      e->SelectAll();
      e->Focus();
   }

   return false;
}

// ----------------------------------------------------------------------------

void FITSHeaderInterface::__ViewList_ViewSelected( ViewList& sender, View& view )
{
   DeactivateTrackView();
   if ( !currentView.IsNull() )
      instance.ImportKeywords( view.Window() );
   UpdateControls();
}

void FITSHeaderInterface::__Keywords_CurrentNodeUpdated( TreeBox& /*sender*/, TreeBox::Node& /*current*/, TreeBox::Node& /*oldCurrent*/ )
{
   UpdateInfo();
}

void FITSHeaderInterface::__Keywords_NodeActivated( TreeBox& /*sender*/, TreeBox::Node& node, int /*col*/ )
{
   String name = node.Text( 0 ); name.Trim();
   String value = node.Text( 1 ); value.Trim();
   String comment = node.Text( 2 ); comment.Trim();
   GUI->Name_Edit.SetText( name );
   GUI->Value_Edit.SetText( value );
   GUI->Comment_Edit.SetText( comment );
   UpdateButtons();
}

void FITSHeaderInterface::__Keywords_NodeSelectionUpdated( TreeBox& /*sender*/ )
{
   UpdateButtons();
}

void FITSHeaderInterface::__Keyword_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Remove_PushButton )
   {
      IndirectArray<TreeBox::Node> selected = GUI->Keywords_List.SelectedNodes();

      for ( IndirectArray<TreeBox::Node>::iterator i = selected.ReverseBegin(); i != selected.ReverseEnd(); --i )
      {
         int index = GUI->Keywords_List.ChildIndex( *i );

         GUI->Keywords_List.Remove( index );

         if ( size_type( index ) < instance.keywords.Length() )
            instance.keywords.Remove( instance.keywords.At( size_type( index ) ) );
      }
   }
   else
   {
      FITSHeaderInstance::Keyword k;
      if ( !GetKeywordData( k ) )
         return;

      TreeBox::Node* node = GUI->Keywords_List.CurrentNode();

      if ( sender == GUI->Add_PushButton )
      {
         int index = 0;
         if ( node != nullptr )
            index = GUI->Keywords_List.ChildIndex( node ) + 1;

         if ( size_type( index ) < instance.keywords.Length() )
            instance.keywords.Insert( instance.keywords.At( index ), k );
         else
            instance.keywords.Add( k );

         TreeBox::Node* node = new TreeBox::Node;
         GUI->Keywords_List.Insert( index, node );
         UpdateKeywordNode( index );
      }
      else if ( sender == GUI->Replace_PushButton )
      {
         if ( node != nullptr )
         {
            int index = GUI->Keywords_List.ChildIndex( node );

            if ( size_type( index ) < instance.keywords.Length() )
               instance.keywords[index] = k;

            UpdateKeywordNode( index );
         }
      }
      else
         return;
   }

   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );
}

void FITSHeaderInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllImages_ViewList || sender == GUI->Keywords_List.Viewport() )
      wantsView = view.IsMainView();
}

void FITSHeaderInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->AllImages_ViewList || sender == GUI->Keywords_List.Viewport() )
      if ( view.IsMainView() )
      {
         DeactivateTrackView();
         instance.ImportKeywords( view.Window() );
         GUI->AllImages_ViewList.SelectView( view );
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

FITSHeaderInterface::GUIData::GUIData( FITSHeaderInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Comment:" ) + 'M' );

   //

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&FITSHeaderInterface::__ViewList_ViewSelected, w );
   AllImages_ViewList.OnViewDrag( (Control::view_drag_event_handler)&FITSHeaderInterface::__ViewDrag, w );
   AllImages_ViewList.OnViewDrop( (Control::view_drop_event_handler)&FITSHeaderInterface::__ViewDrop, w );

   //

   Keywords_List.SetScaledMinWidth( 500 );
   Keywords_List.SetScaledMinHeight( 250 );
   Keywords_List.SetNumberOfColumns( 3 );
   Keywords_List.SetHeaderText( 0, "Name" );
   Keywords_List.SetHeaderText( 1, "Value" );
   Keywords_List.SetHeaderText( 2, "Comment" );
   Keywords_List.DisableRootDecoration();
   Keywords_List.EnableAlternateRowColor();
   Keywords_List.EnableMultipleSelections();
   Keywords_List.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&FITSHeaderInterface::__Keywords_CurrentNodeUpdated, w );
   Keywords_List.OnNodeActivated( (TreeBox::node_event_handler)&FITSHeaderInterface::__Keywords_NodeActivated, w );
   Keywords_List.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&FITSHeaderInterface::__Keywords_NodeSelectionUpdated, w );
   Keywords_List.Viewport().OnViewDrag( (Control::view_drag_event_handler)&FITSHeaderInterface::__ViewDrag, w );
   Keywords_List.Viewport().OnViewDrop( (Control::view_drop_event_handler)&FITSHeaderInterface::__ViewDrop, w );

   //

   Name_Label.SetText( "Name:" );
   Name_Label.SetFixedWidth( labelWidth1 );
   Name_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Name_Sizer.SetSpacing( 4 );
   Name_Sizer.Add( Name_Label );
   Name_Sizer.Add( Name_Edit, 100 );

   //

   Value_Label.SetText( "Value:" );
   Value_Label.SetFixedWidth( labelWidth1 );
   Value_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Value_Sizer.SetSpacing( 4 );
   Value_Sizer.Add( Value_Label );
   Value_Sizer.Add( Value_Edit, 100 );

   //

   Comment_Label.SetText( "Comment:" );
   Comment_Label.SetFixedWidth( labelWidth1 );
   Comment_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Comment_Sizer.SetSpacing( 4 );
   Comment_Sizer.Add( Comment_Label );
   Comment_Sizer.Add( Comment_Edit, 100 );

   //

   HIERARCH_CheckBox.SetText( "HIERARCH Convention" );
   HIERARCH_CheckBox.SetToolTip( "<p>Enables the HIERARCH convention to edit FITS keyword names.</p>"
                                 "<p>If enabled, keyword names can have more than eight characters, "
                                 "and the whole ASCII character set is permitted.</p>"
                                 "<p>If disabled, new keyword names must follow FITS standard rules: "
                                 "no more than eight characters containing only letters, decimal digits, "
                                 "and an underscore or minus sign.</p>" );
   HIERARCH_CheckBox.SetChecked();
   HIERARCH_CheckBox.OnClick( (Button::click_event_handler)&FITSHeaderInterface::__Keyword_ButtonClick, w );

   Info_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   Add_PushButton.SetText( " Add " );
   Add_PushButton.SetToolTip( "<p>Add a new FITS keyword with the current name, value and comment.</p>" );
   Add_PushButton.OnClick( (Button::click_event_handler)&FITSHeaderInterface::__Keyword_ButtonClick, w );

   Replace_PushButton.SetText( " Replace " );
   Replace_PushButton.SetToolTip( "<p>Replace the selected keyword with the current name, value and comment.</p>" );
   Replace_PushButton.OnClick( (Button::click_event_handler)&FITSHeaderInterface::__Keyword_ButtonClick, w );

   Remove_PushButton.SetText( " Remove " );
   Remove_PushButton.SetToolTip( "<p>Remove all selected keywords.</p>" );
   Remove_PushButton.OnClick( (Button::click_event_handler)&FITSHeaderInterface::__Keyword_ButtonClick, w );

   Buttons_Sizer.SetSpacing( 6 );
   Buttons_Sizer.Add( HIERARCH_CheckBox );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Info_Label );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Add_PushButton );
   Buttons_Sizer.Add( Replace_PushButton );
   Buttons_Sizer.Add( Remove_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( AllImages_ViewList );
   Global_Sizer.Add( Keywords_List );
   Global_Sizer.Add( Name_Sizer );
   Global_Sizer.Add( Value_Sizer );
   Global_Sizer.Add( Comment_Sizer );
   Global_Sizer.Add( Buttons_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSHeaderInterface.cpp - Released 2017-07-18T16:14:18Z
