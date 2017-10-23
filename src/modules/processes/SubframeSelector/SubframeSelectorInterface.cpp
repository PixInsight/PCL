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
#include <pcl/ProcessInterface.h>

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

InterfaceFeatures SubframeSelectorInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
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
   UpdateSubframeImagesList();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImageItem( size_type i )
{
   TreeBox::Node* node = GUI->SubframeImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const SubframeSelectorInstance::ImageItem& item = instance.subframes[i];

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

void SubframeSelectorInterface::UpdateSubframeImagesList()
{
   int currentIdx = GUI->SubframeImages_TreeBox.ChildIndex( GUI->SubframeImages_TreeBox.CurrentNode() );

   GUI->SubframeImages_TreeBox.DisableUpdates();
   GUI->SubframeImages_TreeBox.Clear();

   for ( size_type i = 0; i < instance.subframes.Length(); ++i )
   {
      new TreeBox::Node( GUI->SubframeImages_TreeBox );
      UpdateSubframeImageItem( i );
   }

   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !instance.subframes.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->SubframeImages_TreeBox.NumberOfChildren() )
         GUI->SubframeImages_TreeBox.SetCurrentNode( GUI->SubframeImages_TreeBox[currentIdx] );

   GUI->SubframeImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImageSelectionButtons()
{
   bool hasItems = GUI->SubframeImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->SubframeImages_TreeBox.HasSelectedTopLevelNodes();

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
      GUI->SubframeImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->SubframeImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->SubframeImages_TreeBox.SetMaxHeight( int_max );

      if ( GUI->SubframeImages_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_CurrentNodeUpdated( TreeBox &sender,
                                                                     TreeBox::Node &current,
                                                                     TreeBox::Node &oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.subframes.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_NodeActivated( TreeBox &sender, TreeBox::Node &node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.subframes.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );

   SubframeSelectorInstance::ImageItem& item = instance.subframes[index];

   switch ( col )
   {
   case 0:
      // Activate the item's index number: ignore.
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
         UpdateSubframeImageItem( index );
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

void SubframeSelectorInterface::__SubframeImages_NodeSelectionUpdated( TreeBox &sender )
{
   UpdateSubframeImageSelectionButtons();
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_Click( Button &sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "SubframeSelector: Select Subframes" );

      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->SubframeImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.subframes.Insert( instance.subframes.At( i0++ ), SubframeSelectorInstance::ImageItem( *i ) );
         UpdateSubframeImagesList();
         UpdateSubframeImageSelectionButtons();
      }
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->SubframeImages_TreeBox.SelectAllNodes();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->SubframeImages_TreeBox[i]->Select( !GUI->SubframeImages_TreeBox[i]->IsSelected() );
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->SubframeImages_TreeBox[i]->IsSelected() )
            instance.subframes[i].enabled = !instance.subframes[i].enabled;
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      SubframeSelectorInstance::image_list newTargets;
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->SubframeImages_TreeBox[i]->IsSelected() )
            newTargets.Add( instance.subframes[i] );
      instance.subframes = newTargets;
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      instance.subframes.Clear();
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->SubframeImages_TreeBox.Viewport() )
      wantsFiles = true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->SubframeImages_TreeBox.Viewport() )
   {
      StringList inputFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
         if ( File::Exists( item ) )
            inputFiles << item;
         else if ( File::DirectoryExists( item ) )
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );

      inputFiles.Sort();
      size_type i0 = TreeInsertionIndex( GUI->SubframeImages_TreeBox );
      for ( const String& file : inputFiles )
         instance.subframes.Insert( instance.subframes.At( i0++ ), SubframeSelectorInstance::ImageItem( file ) );

      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

SubframeSelectorInterface::GUIData::GUIData( SubframeSelectorInterface& w )
{
   pcl::Font fnt = w.Font();

   //

   SubframeImages_SectionBar.SetTitle( "Subframes" );
   SubframeImages_SectionBar.SetSection( SubframeImages_Control );
   SubframeImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::__ToggleSection, w );

   SubframeImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   SubframeImages_TreeBox.SetNumberOfColumns( 3 );
   SubframeImages_TreeBox.HideHeader();
   SubframeImages_TreeBox.EnableMultipleSelections();
   SubframeImages_TreeBox.DisableRootDecoration();
   SubframeImages_TreeBox.EnableAlternateRowColor();
   SubframeImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler) &SubframeSelectorInterface::__SubframeImages_CurrentNodeUpdated, w );
   SubframeImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler) &SubframeSelectorInterface::__SubframeImages_NodeActivated, w );
   SubframeImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler) &SubframeSelectorInterface::__SubframeImages_NodeSelectionUpdated, w );
   SubframeImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&SubframeSelectorInterface::__FileDrag, w );
   SubframeImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&SubframeSelectorInterface::__FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of subframes.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all subframes.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of subframes.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected subframes.</p>"
      "<p>Disabled subframes will be ignored during the measuring and output processes.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected subframes.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of subframes.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   SubframeButtons_Sizer.SetSpacing( 4 );
   SubframeButtons_Sizer.Add( AddFiles_PushButton );
   SubframeButtons_Sizer.Add( SelectAll_PushButton );
   SubframeButtons_Sizer.Add( InvertSelection_PushButton );
   SubframeButtons_Sizer.Add( ToggleSelected_PushButton );
   SubframeButtons_Sizer.Add( RemoveSelected_PushButton );
   SubframeButtons_Sizer.Add( Clear_PushButton );
   SubframeButtons_Sizer.AddStretch();

   SubframeImages_Sizer.SetSpacing( 4 );
   SubframeImages_Sizer.Add( SubframeImages_TreeBox, 100 );
   SubframeImages_Sizer.Add( SubframeButtons_Sizer );

   SubframeImages_Control.SetSizer( SubframeImages_Sizer );
   SubframeImages_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( SubframeImages_SectionBar );
   Global_Sizer.Add( SubframeImages_Control );

   w.SetSizer( Global_Sizer );

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.cpp - Released 2017-08-01T14:26:58Z
