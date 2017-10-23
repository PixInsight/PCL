//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorInterface.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#include "SubframeSelectorInterface.h"
#include "SubframeSelectorParameters.h"
#include "SubframeSelectorProcess.h"

#include <pcl/Dialog.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/ViewList.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ErrorHandler.h>

#define IMAGELIST_MINHEIGHT( fnt )  (8*fnt.Height() + 2)

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorInterface* TheSubframeSelectorInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "SubframeSelectorIcon.xpm"

// ----------------------------------------------------------------------------

SubframeSelectorInterface::SubframeSelectorInterface() :
   instance( TheSubframeSelectorProcess )
{
   TheSubframeSelectorInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. TreeBox) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

SubframeSelectorInterface::~SubframeSelectorInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString SubframeSelectorInterface::Id() const
{
   return "SubframeSelector";
}

MetaProcess* SubframeSelectorInterface::Process() const
{
   return TheSubframeSelectorProcess;
}

const char** SubframeSelectorInterface::IconImageXPM() const
{
   return nullptr; // SubframeSelectorIcon_XPM; ---> put a nice icon here
}

void SubframeSelectorInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void SubframeSelectorInterface::ResetInstance()
{
   SubframeSelectorInstance defaultInstance( TheSubframeSelectorProcess );
   ImportProcess( defaultInstance );
}

bool SubframeSelectorInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "SubframeSelector" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheSubframeSelectorProcess;
}

ProcessImplementation* SubframeSelectorInterface::NewProcess() const
{
   return new SubframeSelectorInstance( instance );
}

bool SubframeSelectorInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const SubframeSelectorInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a SubframeSelector instance.";
   return false;
}

bool SubframeSelectorInterface::RequiresInstanceValidation() const
{
   return true;
}

bool SubframeSelectorInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateControls()
{
   UpdateTargetImagesList();
   GUI->ParameterOne_NumericControl.SetValue( instance.p_one );
   GUI->ParameterTwo_SpinBox.SetValue( instance.p_two );
   GUI->ParameterThree_CheckBox.SetChecked( instance.p_three );
   GUI->ParameterFour_ComboBox.SetCurrentItem( instance.p_four );
   GUI->ParameterFive_Edit.SetText( instance.p_five );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const SubframeSelectorInstance::ImageItem& item = instance.targetFrames[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateTargetImagesList()
{
   int currentIdx = GUI->TargetImages_TreeBox.ChildIndex( GUI->TargetImages_TreeBox.CurrentNode() );

   GUI->TargetImages_TreeBox.DisableUpdates();
   GUI->TargetImages_TreeBox.Clear();

   for ( size_type i = 0; i < instance.targetFrames.Length(); ++i )
   {
      new TreeBox::Node( GUI->TargetImages_TreeBox );
      UpdateTargetImageItem( i );
   }

   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !instance.targetFrames.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->TargetImages_TreeBox.NumberOfChildren() )
         GUI->TargetImages_TreeBox.SetCurrentNode( GUI->TargetImages_TreeBox[currentIdx] );

   GUI->TargetImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->TargetImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->TargetImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->TargetImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->TargetImages_TreeBox.SetMaxHeight( int_max );

      if ( GUI->TargetImages_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__TargetImages_CurrentNodeUpdated( TreeBox& sender,
                                                                   TreeBox::Node& current,
                                                                   TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );

   SubframeSelectorInstance::ImageItem& item = instance.targetFrames[index];

   switch ( col )
   {
   case 0:
      // Activate the item's index number: ignore.
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
      UpdateTargetImageItem( index );
      break;
   case 2:
      {
         // Activate the item's path: open the image.
         Array<ImageWindow> windows = ImageWindow::Open( item.path, IsoString()/*id*/ );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__TargetImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__TargetImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "SubframeSelector: Select Target Frames" );

      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), SubframeSelectorInstance::ImageItem( *i ) );
         UpdateTargetImagesList();
         UpdateImageSelectionButtons();
      }
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->TargetImages_TreeBox.SelectAllNodes();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->TargetImages_TreeBox[i]->Select( !GUI->TargetImages_TreeBox[i]->IsSelected() );
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->TargetImages_TreeBox[i]->IsSelected() )
            instance.targetFrames[i].enabled = !instance.targetFrames[i].enabled;
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      SubframeSelectorInstance::image_list newTargets;
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->TargetImages_TreeBox[i]->IsSelected() )
            newTargets.Add( instance.targetFrames[i] );
      instance.targetFrames = newTargets;
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      instance.targetFrames.Clear();
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
   {
      StringList inputFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
         if ( File::Exists( item ) )
            inputFiles << item;
         else if ( File::DirectoryExists( item ) )
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );

      inputFiles.Sort();
      size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
      for ( const String& file : inputFiles )
         instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), SubframeSelectorInstance::ImageItem( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ParameterOne_NumericControl )
      instance.p_one = value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ParameterTwo_SpinBox )
      instance.p_two = value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->ParameterThree_CheckBox )
      instance.p_three = checked;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ParameterFour_ComboBox )
      instance.p_four = itemIndex;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__EditGetFocus( Control& sender )
{
   if ( sender == GUI->ParameterFive_Edit )
   {
      // If you need to do something when sender gets focus, do it here.
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__EditCompleted( Edit& sender )
{
   if ( sender == GUI->ParameterFive_Edit )
      instance.p_five = sender.Text();
}

// ----------------------------------------------------------------------------

SubframeSelectorInterface::GUIData::GUIData( SubframeSelectorInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Three:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 7 ) );

   //

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::__ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&SubframeSelectorInterface::__TargetImages_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&SubframeSelectorInterface::__TargetImages_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&SubframeSelectorInterface::__TargetImages_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&SubframeSelectorInterface::__FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&SubframeSelectorInterface::__FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target frames.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__TargetImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target frames.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__TargetImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target frames.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__TargetImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target frames.</p>"
      "<p>Disabled target frames will be ignored during the calibration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__TargetImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target frames.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__TargetImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of target frames.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__TargetImages_Click, w );

   TargetButtons_Sizer.SetSpacing( 4 );
   TargetButtons_Sizer.Add( AddFiles_PushButton );
   TargetButtons_Sizer.Add( SelectAll_PushButton );
   TargetButtons_Sizer.Add( InvertSelection_PushButton );
   TargetButtons_Sizer.Add( ToggleSelected_PushButton );
   TargetButtons_Sizer.Add( RemoveSelected_PushButton );
   TargetButtons_Sizer.Add( Clear_PushButton );
   TargetButtons_Sizer.AddStretch();

   TargetImages_Sizer.SetSpacing( 4 );
   TargetImages_Sizer.Add( TargetImages_TreeBox, 100 );
   TargetImages_Sizer.Add( TargetButtons_Sizer );

   TargetImages_Control.SetSizer( TargetImages_Sizer );
   TargetImages_Control.AdjustToContents();

   //

   ParameterOne_NumericControl.label.SetText( "One:" );
   ParameterOne_NumericControl.label.SetFixedWidth( labelWidth1 );
   ParameterOne_NumericControl.slider.SetScaledMinWidth( 250 );
   ParameterOne_NumericControl.slider.SetRange( 0, 100 );
   ParameterOne_NumericControl.SetReal();
   ParameterOne_NumericControl.SetRange( TheSubframeSelectorParameterOneParameter->MinimumValue(), TheSubframeSelectorParameterOneParameter->MaximumValue() );
   ParameterOne_NumericControl.SetPrecision( TheSubframeSelectorParameterOneParameter->Precision() );
   ParameterOne_NumericControl.SetToolTip( "<p>This is the first parameter.</p>" );
   ParameterOne_NumericControl.edit.SetFixedWidth( editWidth1 );
   ParameterOne_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   //

   ParameterTwo_Label.SetText( "Two:" );
   ParameterTwo_Label.SetMinWidth( labelWidth1 );
   ParameterTwo_Label.SetToolTip( "<p>This is the second parameter.</p>" );
   ParameterTwo_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterTwo_SpinBox.SetRange( int( TheSubframeSelectorParameterTwoParameter->MinimumValue() ), int( TheSubframeSelectorParameterTwoParameter->MaximumValue() ) );
   ParameterTwo_SpinBox.SetFixedWidth( editWidth1 );
   ParameterTwo_SpinBox.SetToolTip( "<p>This is the second parameter.</p>" );
   ParameterTwo_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   ParameterTwo_Sizer.SetSpacing( 4 );
   ParameterTwo_Sizer.Add( ParameterTwo_Label );
   ParameterTwo_Sizer.Add( ParameterTwo_SpinBox );
   ParameterTwo_Sizer.AddStretch();

   //

   ParameterThree_CheckBox.SetText( "Three" );
   ParameterThree_CheckBox.SetToolTip( "<p>This is the third parameter.</p>" );
   ParameterThree_CheckBox.OnClick( (pcl::Button::click_event_handler)&SubframeSelectorInterface::__ItemClicked, w );

   ParameterThree_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   ParameterThree_Sizer.Add( ParameterThree_CheckBox );
   ParameterThree_Sizer.AddStretch();

   //

   ParameterFour_Label.SetText( "Four:" );
   ParameterFour_Label.SetMinWidth( labelWidth1 );
   ParameterFour_Label.SetToolTip( "<p>This is the fourth parameter.</p>" );
   ParameterFour_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterFour_ComboBox.AddItem( "First Item" );
   ParameterFour_ComboBox.AddItem( "Second Item" );
   ParameterFour_ComboBox.AddItem( "Third Item" );
   ParameterFour_ComboBox.SetToolTip( "<p>This is the fourth parameter.</p>" );
   ParameterFour_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SubframeSelectorInterface::__ItemSelected, w );

   ParameterFour_Sizer.SetSpacing( 4 );
   ParameterFour_Sizer.Add( ParameterFour_Label );
   ParameterFour_Sizer.Add( ParameterFour_ComboBox );
   ParameterFour_Sizer.AddStretch();

   //

   ParameterFive_Label.SetText( "Five:" );
   ParameterFive_Label.SetMinWidth( labelWidth1 );
   ParameterFive_Label.SetToolTip( "<p>This is the fifth parameter.</p>" );
   ParameterFive_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterFive_Edit.SetMinWidth( editWidth1 );
   ParameterFive_Edit.SetToolTip( "<p>This is the fifth parameter.</p>" );
   ParameterFive_Edit.OnGetFocus( (Control::event_handler)&SubframeSelectorInterface::__EditGetFocus, w );
   ParameterFive_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::__EditCompleted, w );

   ParameterFive_Sizer.SetSpacing( 4 );
   ParameterFive_Sizer.Add( ParameterFive_Label );
   ParameterFive_Sizer.Add( ParameterFive_Edit, 100 );

   Parameters_Sizer.SetSpacing( 4 );
   Parameters_Sizer.Add( ParameterOne_NumericControl );
   Parameters_Sizer.Add( ParameterTwo_Sizer );
   Parameters_Sizer.Add( ParameterThree_Sizer );
   Parameters_Sizer.Add( ParameterFour_Sizer );
   Parameters_Sizer.Add( ParameterFive_Sizer );
   Parameters_Sizer.AddStretch();

   Parameters_Control.SetSizer( Parameters_Sizer );
   Parameters_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( Parameters_SectionBar );
   Global_Sizer.Add( Parameters_Control );

   w.SetSizer( Global_Sizer );

   Parameters_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.cpp - Released 2017-08-01T14:26:58Z
