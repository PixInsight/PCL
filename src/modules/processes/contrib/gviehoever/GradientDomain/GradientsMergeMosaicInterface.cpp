//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0230
// ----------------------------------------------------------------------------
// GradientsMergeMosaicInterface.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2018. Licensed under LGPL 2.1
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include "GradientsMergeMosaicInterface.h"
#include "GradientsMergeMosaicProcess.h"

#include <pcl/Dialog.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ViewList.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsMergeMosaicInterface* TheGradientsMergeMosaicInterface = nullptr;

// ----------------------------------------------------------------------------
//FIXME
//#include "GradientsMergeMosaicIcon.xpm"

// ----------------------------------------------------------------------------

GradientsMergeMosaicInterface::GradientsMergeMosaicInterface() :
  instance( TheGradientsMergeMosaicProcess )
{
   TheGradientsMergeMosaicInterface = this;

   /*
    * The auto save geometry feature is of no good to interfaces that include
    * both auto-expanding controls (e.g. TreeBox) and collapsible sections
    * (e.g. SectionBar).
    */
   DisableAutoSaveGeometry();
}

GradientsMergeMosaicInterface::~GradientsMergeMosaicInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString GradientsMergeMosaicInterface::Id() const
{
   return "GradientMergeMosaic";
}

IsoString GradientsMergeMosaicInterface::Aliases() const
{
   return "GradientsMergeMosaic";
}

MetaProcess* GradientsMergeMosaicInterface::Process() const
{
   return TheGradientsMergeMosaicProcess;
}

const char** GradientsMergeMosaicInterface::IconImageXPM() const
{
  return nullptr;
  // FIXME
  // return GradientsMergeMosaicIcon_XPM;
}

InterfaceFeatures GradientsMergeMosaicInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void GradientsMergeMosaicInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void GradientsMergeMosaicInterface::ResetInstance()
{
   GradientsMergeMosaicInstance defaultInstance( TheGradientsMergeMosaicProcess );
   ImportProcess( defaultInstance );
}

bool GradientsMergeMosaicInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "GradientMergeMosaic" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheGradientsMergeMosaicProcess;
}

ProcessImplementation* GradientsMergeMosaicInterface::NewProcess() const
{
   return new GradientsMergeMosaicInstance( instance );
}

bool GradientsMergeMosaicInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const GradientsMergeMosaicInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a GradientMergeMosaic instance.";
   return false;
}

bool GradientsMergeMosaicInterface::RequiresInstanceValidation() const
{
   return true;
}

bool GradientsMergeMosaicInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

void GradientsMergeMosaicInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void GradientsMergeMosaicInterface::UpdateControls()
{
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateParameters();
}

void GradientsMergeMosaicInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const GradientsMergeMosaicInstance::ImageItem& item = instance.targetFrames[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/icons/enabled.png" : ":/icons/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/icons/document.png" ) ) );
   if ( GUI->FullPaths_CheckBox.IsChecked() )
      node->SetText( 2, item.path );
   else
   {
      String fileName = File::ExtractName( item.path ) + File::ExtractExtension( item.path );
      node->SetText( 2, fileName );
      node->SetToolTip( 2, item.path );
   }
   node->SetAlignment( 2, TextAlign::Left );
}

void GradientsMergeMosaicInterface::UpdateTargetImagesList()
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

void GradientsMergeMosaicInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->TargetImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->TargetImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->MoveUp_PushButton.Enable( hasSelection);
   GUI->MoveDown_PushButton.Enable( hasSelection);
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
   //GUI->FullPaths_CheckBox.Enable( hasItems );   // always enabled
}

void GradientsMergeMosaicInterface::UpdateParameters()
{
  GUI->Type_ComboBox.SetCurrentItem(instance.type);
  GUI->ShrinkCount_SpinBox.SetValue(instance.shrinkCount);
  GUI->FeatherRadius_SpinBox.SetValue(instance.featherRadius);
  GUI->BlackPoint_NumericControl.SetValue(instance.blackPoint);
  GUI->GenerateMask_CheckBox.SetChecked(instance.generateMask);

}

// ----------------------------------------------------------------------------

void GradientsMergeMosaicInterface::__TargetImages_CurrentNodeUpdated( TreeBox& sender,
                                                      TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "GradientsMergeMosaicInterface: *Warning* Corrupted interface structures" );

   // ### If there's something else that depends on which image is selected in the list, do it here.
}

void GradientsMergeMosaicInterface::__TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "GradientsMergeMosaicInterface: *Warning* Corrupted interface structures" );

   GradientsMergeMosaicInstance::ImageItem& item = instance.targetFrames[index];

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
         Array<ImageWindow> w = ImageWindow::Open( item.path );
         for ( Array<ImageWindow>::iterator i = w.Begin(); i != w.End(); ++i )
            i->Show();
      }
      break;
   }
}

void GradientsMergeMosaicInterface::__TargetImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

void GradientsMergeMosaicInterface::__TargetImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "GradientsMergeMosaic: Select Target Frames" );
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), GradientsMergeMosaicInstance::ImageItem( *i ) );
         UpdateTargetImagesList();
         UpdateImageSelectionButtons();
      }
   }
   else if ( sender == GUI->MoveUp_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->TargetImages_TreeBox[i]->IsSelected() )
            if ( i == 0 )
            {
               // first is selected, we cannot really do something about this
               break;
            }
            else
            {
               // swap order with previous one
               GradientsMergeMosaicInstance::ImageItem tmp=instance.targetFrames[i-1];
               instance.targetFrames[i-1]=instance.targetFrames[i];
               instance.targetFrames[i]=tmp;
               GUI->TargetImages_TreeBox[i]->Select(false);
               GUI->TargetImages_TreeBox[i-1]->Select(true);
               UpdateTargetImageItem(i);
               UpdateTargetImageItem(i-1);
            }
      // FIXME this restores the original selection list, which is just what I dont want.
      // Added UpdateTargetImageItem() above which is slightly less efficient
      // UpdateTargetImagesList();
   }
   else if ( sender == GUI->MoveDown_PushButton )
   {
      for ( int n = GUI->TargetImages_TreeBox.NumberOfChildren(), i = n-1; i >= 0; --i )
         if ( GUI->TargetImages_TreeBox[i]->IsSelected() )
            if ( i == n-1 )
            {
               // first is selected, we cannot really do something about this
               break;
            }
            else
            {
               // swap order with next one
               GradientsMergeMosaicInstance::ImageItem tmp=instance.targetFrames[i];
               instance.targetFrames[i]=instance.targetFrames[i+1];
               instance.targetFrames[i+1]=tmp;
               GUI->TargetImages_TreeBox[i]->Select(false);
               GUI->TargetImages_TreeBox[i+1]->Select(true);
               UpdateTargetImageItem(i);
               UpdateTargetImageItem(i+1);
            }
      // FIXME this restores the original selection list, which is just what I dont want.
      // Added UpdateTargetImageItem() above which is slightly less efficient
      // UpdateTargetImagesList();
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
      GradientsMergeMosaicInstance::image_list newTargets;
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
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
}

void GradientsMergeMosaicInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->TargetImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->TargetImages_TreeBox.SetMaxHeight( int_max );
   }
}

void GradientsMergeMosaicInterface::__TypeItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Type_ComboBox )
      instance.type = itemIndex;
}

void GradientsMergeMosaicInterface::__ShrinkCountValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ShrinkCount_SpinBox )
     instance.shrinkCount = value;
}

void GradientsMergeMosaicInterface::__FeatherRadiusValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->FeatherRadius_SpinBox )
     instance.featherRadius = value;
}

void GradientsMergeMosaicInterface::__BlackPointValueUpdated( NumericEdit& sender, double value )
{
  if (sender == GUI->BlackPoint_NumericControl)
    instance.blackPoint=value;
}

void GradientsMergeMosaicInterface::_GenerateMaskClicked( Button& sender, bool checked )
{
  if (sender == GUI->GenerateMask_CheckBox)
    instance.generateMask=checked;
}

void GradientsMergeMosaicInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
}

void GradientsMergeMosaicInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
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
         instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), GradientsMergeMosaicInstance::ImageItem( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

GradientsMergeMosaicInterface::GUIData::GUIData( GradientsMergeMosaicInterface& w )
{
   pcl::Font fnt = w.Font();
   int const labelWidth1 = fnt.Width( String( "Optimization window (px):" ) + 'T' );
   int const editWidth1 = fnt.Width( String( 'M', 5  ) );

   //

   w.SetToolTip( "<p>Merges together previously aligned images, usually successfully hiding any seams.</p>"
      "<p>(c) 2011 Georg Viehoever, published under LGPL 2.1. "
      "With important contributions in terms of tests, test data, code and ideas "
      "by Juan Conejero, Carlos Milovic, Harry Page, David Raphael, Geert Vanhauwaert, Zbynek Vrastil, and others.</p>" );

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&GradientsMergeMosaicInterface::__ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&GradientsMergeMosaicInterface::__TargetImages_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&GradientsMergeMosaicInterface::__TargetImages_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&GradientsMergeMosaicInterface::__TargetImages_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&GradientsMergeMosaicInterface::__FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&GradientsMergeMosaicInterface::__FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files..." );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   MoveUp_PushButton.SetText( "Move Up" );
   MoveUp_PushButton.SetToolTip( "<p>Move selected images one up</p>" );
   MoveUp_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   MoveDown_PushButton.SetText( "Move Down" );
   MoveDown_PushButton.SetToolTip( "<p>Move selected images one down</p>" );
   MoveDown_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected images.</p>"
      "<p>Disabled images will be ignored during the process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for image files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&GradientsMergeMosaicInterface::__TargetImages_Click, w );

   TargetButtons_Sizer.SetSpacing( 4 );
   TargetButtons_Sizer.Add( AddFiles_PushButton );
   TargetButtons_Sizer.Add( MoveUp_PushButton );
   TargetButtons_Sizer.Add( MoveDown_PushButton );
   TargetButtons_Sizer.Add( SelectAll_PushButton );
   TargetButtons_Sizer.Add( InvertSelection_PushButton );
   TargetButtons_Sizer.Add( ToggleSelected_PushButton );
   TargetButtons_Sizer.Add( RemoveSelected_PushButton );
   TargetButtons_Sizer.Add( Clear_PushButton );
   TargetButtons_Sizer.Add( FullPaths_CheckBox );
   TargetButtons_Sizer.AddStretch();

   TargetImages_Sizer.SetSpacing( 4 );
   TargetImages_Sizer.Add( TargetImages_TreeBox, 100 );
   TargetImages_Sizer.Add( TargetButtons_Sizer );

   TargetImages_Control.SetSizer( TargetImages_Sizer );
   TargetImages_Control.AdjustToContents();

   Parameters_SectionBar.SetTitle( "Parameters" );
   Parameters_SectionBar.SetSection( Parameters_Control );
   Parameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&GradientsMergeMosaicInterface::__ToggleSection, w );

   const char* typeToolTip ="<p>Select the image combination method. "
                            "For the overlay mode, the later images are displayed on top of the already loaded images. In this "
                            "case, the order of the images in the list box is important. For the average mode, the average "
                            "of all overlapping images is used. In this case, the order of loaded images in irrelevant.</p>";

   Type_Label.SetText( "Type of combination:" );
   Type_Label.SetFixedWidth( labelWidth1 );
   Type_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Type_Label.SetToolTip( typeToolTip );
   Type_ComboBox.SetToolTip( typeToolTip );
   Type_ComboBox.AddItem("Overlay");
   Type_ComboBox.AddItem("Average");
   Type_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GradientsMergeMosaicInterface::__TypeItemSelected,w);

   Type_Sizer.SetSpacing( 4);
   Type_Sizer.Add (Type_Label);
   Type_Sizer.Add (Type_ComboBox);

   const char* shrinkCountToolTip ="<p>Reduce image areas of each contributing image by "
                                   "erosions with a mask of the selected radius. "
                                   "This is useful to get rid "
                                   "of border regions that slowly fade into the background, for "
                                   "instance due to aliasing during alignment.</p>";
   ShrinkCount_Label.SetText("Shrink radius:");
   ShrinkCount_Label.SetFixedWidth(labelWidth1);
   ShrinkCount_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ShrinkCount_Label.SetToolTip( shrinkCountToolTip );
   ShrinkCount_SpinBox.SetRange( int( TheGradientsMergeMosaicShrinkCountParameter->MinimumValue() ), int( TheGradientsMergeMosaicShrinkCountParameter->MaximumValue() ) );
   ShrinkCount_SpinBox.SetFixedWidth( editWidth1 );
   ShrinkCount_SpinBox.SetToolTip( shrinkCountToolTip );
   ShrinkCount_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&GradientsMergeMosaicInterface::__ShrinkCountValueUpdated, w );
   ShrinkCount_Sizer.SetSpacing( 4);
   ShrinkCount_Sizer.Add (ShrinkCount_Label);
   ShrinkCount_Sizer.Add (ShrinkCount_SpinBox);
   ShrinkCount_Sizer.AddStretch();

   const char* featherRadiusToolTip ="<p>Smooth the border of each contributing image "
                                   "using a convolution of the specified radius. "
                                   "This is useful to reduce artifacts caused by stars located "
                                   "on the image border, or to smooth transitions in patterns.</p>";
   FeatherRadius_Label.SetText("Feather radius:");
   FeatherRadius_Label.SetFixedWidth(labelWidth1);
   FeatherRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FeatherRadius_Label.SetToolTip( featherRadiusToolTip );
   FeatherRadius_SpinBox.SetRange( int( TheGradientsMergeMosaicFeatherRadiusParameter->MinimumValue() ), int( TheGradientsMergeMosaicFeatherRadiusParameter->MaximumValue() ) );
   FeatherRadius_SpinBox.SetFixedWidth( editWidth1 );
   FeatherRadius_SpinBox.SetToolTip( featherRadiusToolTip );
   FeatherRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&GradientsMergeMosaicInterface::__FeatherRadiusValueUpdated, w );
   FeatherRadius_Sizer.SetSpacing( 4);
   FeatherRadius_Sizer.Add (FeatherRadius_Label);
   FeatherRadius_Sizer.Add (FeatherRadius_SpinBox);
   FeatherRadius_Sizer.AddStretch();

   BlackPoint_NumericControl.label.SetText( "Black point:");
   BlackPoint_NumericControl.label.SetFixedWidth( labelWidth1);
   BlackPoint_NumericControl.slider.SetScaledMinWidth(250);
   BlackPoint_NumericControl.slider.SetRange(0,100);
   BlackPoint_NumericControl.SetReal();
   BlackPoint_NumericControl.SetRange(TheGradientsMergeMosaicBlackPointParameter->MinimumValue(),TheGradientsMergeMosaicBlackPointParameter->MaximumValue());
   BlackPoint_NumericControl.SetPrecision(TheGradientsMergeMosaicBlackPointParameter->Precision() );
   BlackPoint_NumericControl.SetToolTip( "<p>Pixels equal or below this value are considered transparent.</p>");
   BlackPoint_NumericControl.edit.SetFixedWidth( editWidth1 );
   BlackPoint_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GradientsMergeMosaicInterface::__BlackPointValueUpdated, w );

   const char* maskToolTip="<p>Check to generate a mask showing how the image is composed. The mask indicates in a bitwise fashion which "
                           "images contribute to each pixel: image 1 has value 2**(n-1), image 2 has value 2**(n-2), ...,"
                           "image n has value 1. All values are added, and the resulting image is rescaled before presentation.</p>";
   GenerateMask_Label.SetText("Generate mask:");
   GenerateMask_Label.SetFixedWidth(labelWidth1);
   GenerateMask_Label.SetTextAlignment(TextAlign::Right|TextAlign::VertCenter );
   GenerateMask_Label.SetToolTip(maskToolTip);
   GenerateMask_CheckBox.SetChecked(TheGradientsMergeMosaicGenerateMaskParameter->DefaultValue());
   GenerateMask_CheckBox.SetToolTip(maskToolTip);
   GenerateMask_CheckBox.OnClick( (pcl::Button::click_event_handler)&GradientsMergeMosaicInterface::_GenerateMaskClicked,w);

   GenerateMask_Sizer.SetSpacing( 4);
   GenerateMask_Sizer.Add(GenerateMask_Label);
   GenerateMask_Sizer.Add(GenerateMask_CheckBox);

   Parameters_Sizer.SetSpacing( 4 );
   Parameters_Sizer.Add(Type_Sizer);
   Parameters_Sizer.Add(ShrinkCount_Sizer);
   Parameters_Sizer.Add(FeatherRadius_Sizer);
   Parameters_Sizer.Add(BlackPoint_NumericControl);
   Parameters_Sizer.Add(GenerateMask_Sizer);

   Parameters_Control.SetSizer(Parameters_Sizer);
   Parameters_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( Parameters_SectionBar );
   Global_Sizer.Add( Parameters_Control );

   w.SetSizer( Global_Sizer );
   w.SetFixedWidth();
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicInterface.cpp - Released 2018-11-23T18:45:58Z
