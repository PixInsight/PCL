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
#include <pcl/PreviewSelectionDialog.h>

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
   GUI->Routine_Control.SetCurrentItem( instance.routine );
   UpdateSubframeImagesList();
   UpdateSystemParameters();
   UpdateStarDetectorParameters();
   UpdateMeasurementImagesList();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImageItem( size_type i )
{
   TreeBox::Node* node = GUI->SubframeImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const SubframeSelectorInstance::SubframeItem& item = instance.subframes[i];

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

void SubframeSelectorInterface::UpdateSystemParameters()
{
   GUI->SystemParameters_SubframeScale_Control.SetValue( instance.subframeScale );
   GUI->SystemParameters_CameraGain_Control.SetValue( instance.cameraGain );
   GUI->SystemParameters_CameraResolution_Control.SetCurrentItem( instance.cameraResolution );
   GUI->SystemParameters_SiteLocalMidnight_Control.SetValue( instance.siteLocalMidnight );
   GUI->SystemParameters_ScaleUnit_Control.SetCurrentItem( instance.scaleUnit );
   GUI->SystemParameters_DataUnit_Control.SetCurrentItem( instance.dataUnit );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateStarDetectorParameters()
{
   GUI->StarDetectorParameters_StructureLayers_Control.SetValue( instance.structureLayers );
   GUI->StarDetectorParameters_NoiseLayers_Control.SetValue( instance.noiseLayers );
   GUI->StarDetectorParameters_HotPixelFilterRadius_Control.SetValue( instance.hotPixelFilterRadius );
   GUI->StarDetectorParameters_ApplyHotPixelFilter_Control.SetChecked( instance.applyHotPixelFilterToDetectionImage );
   GUI->StarDetectorParameters_NoiseReductionFilterRadius_Control.SetValue( instance.noiseReductionFilterRadius );
   GUI->StarDetectorParameters_Sensitivity_Control.SetValue( instance.sensitivity );
   GUI->StarDetectorParameters_PeakResponse_Control.SetValue( instance.peakResponse );
   GUI->StarDetectorParameters_MaxDistortion_Control.SetValue( instance.maxDistortion );
   GUI->StarDetectorParameters_UpperLimit_Control.SetValue( instance.upperLimit );
   GUI->StarDetectorParameters_BackgroundExpansion_Control.SetValue( instance.backgroundExpansion );
   GUI->StarDetectorParameters_XYStretch_Control.SetValue( instance.xyStretch );
   GUI->StarDetectorParameters_ROIX0_Control.SetValue( instance.roi.x0 );
   GUI->StarDetectorParameters_ROIY0_Control.SetValue( instance.roi.y0 );
   GUI->StarDetectorParameters_ROIWidth_Control.SetValue( instance.roi.Width() );
   GUI->StarDetectorParameters_ROIHeight_Control.SetValue( instance.roi.Height() );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateMeasurementImageItem( size_type i )
{
   TreeBox::Node* node = GUI->SubframeImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const MeasureData& item = instance.measures[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( item.locked ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 2, TextAlign::Left );

   node->SetIcon( 3, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   node->SetText( 3, File::ExtractNameAndSuffix( item.path ) );
   node->SetToolTip( 3, item.path );
   node->SetAlignment( 3, TextAlign::Left );

   node->SetText( 4, String().Format( "%.03f", item.fwhm ) );
   node->SetAlignment( 4, TextAlign::Center );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateMeasurementImagesList()
{
   int currentIdx = GUI->MeasurementImages_TreeBox.ChildIndex( GUI->MeasurementImages_TreeBox.CurrentNode() );

   GUI->MeasurementImages_TreeBox.DisableUpdates();
   GUI->MeasurementImages_TreeBox.Clear();

   for ( size_type i = 0; i < instance.measures.Length(); ++i )
   {
      new TreeBox::Node( GUI->MeasurementImages_TreeBox );
      UpdateMeasurementImageItem( i );
   }

   GUI->MeasurementImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->MeasurementImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->MeasurementImages_TreeBox.AdjustColumnWidthToContents( 2 );
   GUI->MeasurementImages_TreeBox.AdjustColumnWidthToContents( 3 );
   GUI->MeasurementImages_TreeBox.AdjustColumnWidthToContents( 4 );

   if ( !instance.measures.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->MeasurementImages_TreeBox.NumberOfChildren() )
         GUI->MeasurementImages_TreeBox.SetCurrentNode( GUI->MeasurementImages_TreeBox[currentIdx] );

   GUI->MeasurementImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( sender == GUI->SubframeImages_SectionBar )
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
   else if ( sender == GUI->MeasurementImages_SectionBar )
   {
      if ( start )
         GUI->MeasurementImages_TreeBox.SetFixedHeight();
      else
      {
         GUI->MeasurementImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
         GUI->MeasurementImages_TreeBox.SetMaxHeight( int_max );

         if ( GUI->MeasurementImages_Control.IsVisible() )
            SetVariableHeight();
         else
            SetFixedHeight();
      }
   }
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
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

   SubframeSelectorInstance::SubframeItem& item = instance.subframes[index];

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
            instance.subframes.Insert( instance.subframes.At( i0++ ), SubframeSelectorInstance::SubframeItem( *i ) );
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
      SubframeSelectorInstance::subframe_list newTargets;
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

void SubframeSelectorInterface::__SubframeImages_FileDrag( Control &sender, const Point &pos, const StringList &files,
                                                           unsigned modifiers, bool &wantsFiles )
{
   if ( sender == GUI->SubframeImages_TreeBox.Viewport() )
      wantsFiles = true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_FileDrop( Control &sender, const Point &pos, const StringList &files,
                                                           unsigned modifiers )
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
         instance.subframes.Insert( instance.subframes.At( i0++ ), SubframeSelectorInstance::SubframeItem( file ) );

      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SystemParameters_SubframeScale_Control )
      instance.subframeScale = value;
   if ( sender == GUI->SystemParameters_CameraGain_Control )
      instance.cameraGain = value;
   if ( sender == GUI->StarDetectorParameters_Sensitivity_Control )
      instance.sensitivity = value;
   if ( sender == GUI->StarDetectorParameters_PeakResponse_Control )
      instance.peakResponse = value;
   if ( sender == GUI->StarDetectorParameters_MaxDistortion_Control )
      instance.maxDistortion = value;
   if ( sender == GUI->StarDetectorParameters_UpperLimit_Control )
      instance.upperLimit = value;
   if ( sender == GUI->StarDetectorParameters_XYStretch_Control )
      instance.xyStretch = value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->SystemParameters_SiteLocalMidnight_Control )
      instance.siteLocalMidnight = value;

   if ( sender == GUI->StarDetectorParameters_StructureLayers_Control )
      instance.structureLayers = value;
   if ( sender == GUI->StarDetectorParameters_NoiseLayers_Control )
      instance.noiseLayers = value;
   if ( sender == GUI->StarDetectorParameters_HotPixelFilterRadius_Control )
      instance.hotPixelFilterRadius = value;
   if ( sender == GUI->StarDetectorParameters_NoiseReductionFilterRadius_Control )
      instance.noiseReductionFilterRadius = value;
   if ( sender == GUI->StarDetectorParameters_BackgroundExpansion_Control )
      instance.backgroundExpansion = value;
   if ( sender == GUI->StarDetectorParameters_ROIX0_Control )
      instance.roi.x0 = value;
   else if ( sender == GUI->StarDetectorParameters_ROIY0_Control )
      instance.roi.y0 = value;
   else if ( sender == GUI->StarDetectorParameters_ROIWidth_Control )
      instance.roi.x1 = instance.roi.x0 + value;
   else if ( sender == GUI->StarDetectorParameters_ROIHeight_Control )
      instance.roi.y1 = instance.roi.y0 + value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Routine_Control )
      instance.routine = itemIndex;
   if ( sender == GUI->SystemParameters_CameraResolution_Control )
      instance.cameraResolution = itemIndex;
   if ( sender == GUI->SystemParameters_ScaleUnit_Control )
      instance.scaleUnit = itemIndex;
   if ( sender == GUI->SystemParameters_DataUnit_Control )
      instance.dataUnit = itemIndex;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__CheckboxUpdated( Button& sender, Button::check_state state )
{
   if ( sender == GUI->StarDetectorParameters_ApplyHotPixelFilter_Control )
      instance.applyHotPixelFilterToDetectionImage = state == CheckState::Checked;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->StarDetectorParameters_ROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               instance.roi = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->StarDetectorParameters_ROISelectPreview_Button )
      wantsView = view.IsPreview();
}

void SubframeSelectorInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->StarDetectorParameters_ROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         instance.roi = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__MeasurementImages_CurrentNodeUpdated( TreeBox &sender,
                                                                        TreeBox::Node &current,
                                                                        TreeBox::Node &oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.measures.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__MeasurementImages_NodeActivated( TreeBox &sender, TreeBox::Node &node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.measures.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );

   MeasureData& item = instance.measures[index];

   switch ( col )
   {
      case 0:
         // Activate the item's index number: ignore.
         break;
      case 1:
         // Activate the item's checkmark: toggle item's enabled state.
         item.enabled = !item.enabled;
         UpdateSubframeImageItem( index );
      case 2:
         // Activate the item's checkmark: toggle item's locked state.
         item.locked = !item.locked;
         UpdateSubframeImageItem( index );
         break;
      case 3:
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

SubframeSelectorInterface::GUIData::GUIData( SubframeSelectorInterface& w )
{
   pcl::Font fnt = w.Font();

   int currentLabelWidth = fnt.Width( String( "Routine:" ) ); // the longest label text
   int currentEditWidth = fnt.Width( String( '0', 7 ) ); // the longest edit text

   //

   Routine_Label.SetText( "Routine:" );
   Routine_Label.SetMinWidth( currentLabelWidth );
   Routine_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   for ( int i = 0; i < TheSSRoutineParameter->NumberOfElements(); ++i )
      Routine_Control.AddItem( TheSSRoutineParameter->ElementLabel( i ) );
   Routine_Control.SetToolTip( TheSSRoutineParameter->Tooltip() );
   Routine_Control.OnItemSelected( (ComboBox::item_event_handler)&SubframeSelectorInterface::__ItemSelected, w );

   Routine_Sizer.SetSpacing( 4 );
   Routine_Sizer.Add( Routine_Label );
   Routine_Sizer.Add( Routine_Control );
   Routine_Sizer.AddStretch();

   //

   currentLabelWidth = fnt.Width( String( "Site Local Midnight:" ) ); // the longest label text

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
   SubframeImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler) &SubframeSelectorInterface::__SubframeImages_FileDrag, w );
   SubframeImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler) &SubframeSelectorInterface::__SubframeImages_FileDrop, w );

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

   SystemParameters_SectionBar.SetTitle( "System Parameters" );
   SystemParameters_SectionBar.SetSection( SystemParameters_Control );
   SystemParameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::__ToggleSection, w );

   SystemParameters_SubframeScale_Control.label.SetText( "Subframe Scale:" );
   SystemParameters_SubframeScale_Control.label.SetMinWidth( currentLabelWidth );
   SystemParameters_SubframeScale_Control.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SystemParameters_SubframeScale_Control.slider.Hide();
   SystemParameters_SubframeScale_Control.SetReal();
   SystemParameters_SubframeScale_Control.SetRange( TheSSSubframeScaleParameter->MinimumValue(), TheSSSubframeScaleParameter->MaximumValue() );
   SystemParameters_SubframeScale_Control.SetPrecision( TheSSSubframeScaleParameter->Precision() );
   SystemParameters_SubframeScale_Control.SetToolTip( "TODO" ); // TODO
   SystemParameters_SubframeScale_Control.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   SystemParameters_SubframeScale_Unit.SetText( "arcseconds / pixel" );
   SystemParameters_SubframeScale_Unit.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   SystemParameters_SubframeScale_Sizer.SetSpacing( 4 );
   SystemParameters_SubframeScale_Sizer.Add( SystemParameters_SubframeScale_Control );
   SystemParameters_SubframeScale_Sizer.Add( SystemParameters_SubframeScale_Unit );
   SystemParameters_SubframeScale_Sizer.AddStretch();

   SystemParameters_CameraGain_Control.label.SetText( "Camera Gain:" );
   SystemParameters_CameraGain_Control.label.SetMinWidth( currentLabelWidth );
   SystemParameters_CameraGain_Control.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SystemParameters_CameraGain_Control.slider.Hide();
   SystemParameters_CameraGain_Control.SetReal();
   SystemParameters_CameraGain_Control.SetRange( TheSSCameraGainParameter->MinimumValue(), TheSSCameraGainParameter->MaximumValue() );
   SystemParameters_CameraGain_Control.SetPrecision( TheSSCameraGainParameter->Precision() );
   SystemParameters_CameraGain_Control.SetToolTip( "TODO" ); // TODO
   SystemParameters_CameraGain_Control.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   SystemParameters_CameraGain_Unit.SetText( "electrons / Data Number" );
   SystemParameters_CameraGain_Unit.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   SystemParameters_CameraGain_Sizer.SetSpacing( 4 );
   SystemParameters_CameraGain_Sizer.Add( SystemParameters_CameraGain_Control );
   SystemParameters_CameraGain_Sizer.Add( SystemParameters_CameraGain_Unit );
   SystemParameters_CameraGain_Sizer.AddStretch();

   SystemParameters_CameraResolution_Label.SetText( "Camera Resolution:" );
   SystemParameters_CameraResolution_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_CameraResolution_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   for ( int i = 0; i < TheSSCameraResolutionParameter->NumberOfElements(); ++i )
      SystemParameters_CameraResolution_Control.AddItem( TheSSCameraResolutionParameter->ElementLabel( i ) );
   SystemParameters_CameraResolution_Control.SetToolTip( "TODO" ); // TODO
   SystemParameters_CameraResolution_Control.OnItemSelected( (ComboBox::item_event_handler)&SubframeSelectorInterface::__ItemSelected, w );

   SystemParameters_CameraResolution_Sizer.SetSpacing( 4 );
   SystemParameters_CameraResolution_Sizer.Add( SystemParameters_CameraResolution_Label );
   SystemParameters_CameraResolution_Sizer.Add( SystemParameters_CameraResolution_Control );
   SystemParameters_CameraResolution_Sizer.AddStretch();

   SystemParameters_SiteLocalMidnight_Label.SetText( "Site Local Midnight:" );
   SystemParameters_SiteLocalMidnight_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_SiteLocalMidnight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   SystemParameters_SiteLocalMidnight_Control.SetRange( TheSSSiteLocalMidnightParameter->MinimumValue(), TheSSSiteLocalMidnightParameter->MaximumValue() );
   SystemParameters_SiteLocalMidnight_Control.SetToolTip( "<p>This parameters specifies the Coordinated Universal Time (UTC) of local midnight "
                                                          "at the site of target subframe observation, rounded to the nearest hour from 0 to "
                                                          "23. If this time is unknown or varies by more than six hours, set this parameter "
                                                          "to 24.</p> "
                                                          "<p>This parameter and the value of the FITS keyword DATE-OBS (if available) are used to "
                                                          "identify sequences of subframe observations that occurred during the same night for "
                                                          "data presentation purposes.</p>" );
   SystemParameters_SiteLocalMidnight_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   SystemParameters_SiteLocalMidnight_Unit.SetText( "hours (UTC)" );
   SystemParameters_SiteLocalMidnight_Unit.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   SystemParameters_SiteLocalMidnight_Sizer.SetSpacing( 4 );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Label );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Control );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Unit );
   SystemParameters_SiteLocalMidnight_Sizer.AddStretch();

   SystemParameters_ScaleUnit_Label.SetText( "Scale Unit:" );
   SystemParameters_ScaleUnit_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_ScaleUnit_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   for ( int i = 0; i < TheSSScaleUnitParameter->NumberOfElements(); ++i )
      SystemParameters_ScaleUnit_Control.AddItem( TheSSScaleUnitParameter->ElementLabel( i ) );
   SystemParameters_ScaleUnit_Control.SetToolTip( "TODO" ); // TODO
   SystemParameters_ScaleUnit_Control.OnItemSelected( (ComboBox::item_event_handler)&SubframeSelectorInterface::__ItemSelected, w );

   SystemParameters_ScaleUnit_Sizer.SetSpacing( 4 );
   SystemParameters_ScaleUnit_Sizer.Add( SystemParameters_ScaleUnit_Label );
   SystemParameters_ScaleUnit_Sizer.Add( SystemParameters_ScaleUnit_Control );
   SystemParameters_ScaleUnit_Sizer.AddStretch();

   SystemParameters_DataUnit_Label.SetText( "Data Unit:" );
   SystemParameters_DataUnit_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_DataUnit_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   for ( int i = 0; i < TheSSDataUnitParameter->NumberOfElements(); ++i )
      SystemParameters_DataUnit_Control.AddItem( TheSSDataUnitParameter->ElementLabel( i ) );
   SystemParameters_DataUnit_Control.SetToolTip( "TODO" ); // TODO
   SystemParameters_DataUnit_Control.OnItemSelected( (ComboBox::item_event_handler)&SubframeSelectorInterface::__ItemSelected, w );

   SystemParameters_DataUnit_Sizer.SetSpacing( 4 );
   SystemParameters_DataUnit_Sizer.Add( SystemParameters_DataUnit_Label );
   SystemParameters_DataUnit_Sizer.Add( SystemParameters_DataUnit_Control );
   SystemParameters_DataUnit_Sizer.AddStretch();

   SystemParameters_Sizer.SetSpacing( 4 );
   SystemParameters_Sizer.Add( SystemParameters_SubframeScale_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_CameraGain_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_CameraResolution_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_SiteLocalMidnight_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_ScaleUnit_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_DataUnit_Sizer );

   SystemParameters_Control.SetSizer( SystemParameters_Sizer );
   SystemParameters_Control.AdjustToContents();

   //

   currentLabelWidth = fnt.Width( String( "Background Expansion:" ) ); // the longest label text

   StarDetectorParameters_SectionBar.SetTitle( "Star Detector Parameters" );
   StarDetectorParameters_SectionBar.SetSection( StarDetectorParameters_Control );
   StarDetectorParameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::__ToggleSection, w );

   StarDetectorParameters_StructureLayers_Label.SetText( "Structure Layers:" );
   StarDetectorParameters_StructureLayers_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StarDetectorParameters_StructureLayers_Control.SetRange( TheSSStructureLayersParameter->MinimumValue(), TheSSStructureLayersParameter->MaximumValue() );
   StarDetectorParameters_StructureLayers_Control.SetToolTip( TheSSStructureLayersParameter->Tooltip() );
   StarDetectorParameters_StructureLayers_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_StructureLayers_Sizer.SetSpacing( 4 );
   StarDetectorParameters_StructureLayers_Sizer.Add( StarDetectorParameters_StructureLayers_Label );
   StarDetectorParameters_StructureLayers_Sizer.Add( StarDetectorParameters_StructureLayers_Control );
   StarDetectorParameters_StructureLayers_Sizer.AddStretch();

   StarDetectorParameters_NoiseLayers_Label.SetText( "Noise Layers:" );
   StarDetectorParameters_NoiseLayers_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_NoiseLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StarDetectorParameters_NoiseLayers_Control.SetRange( TheSSNoiseLayersParameter->MinimumValue(), TheSSNoiseLayersParameter->MaximumValue() );
   StarDetectorParameters_NoiseLayers_Control.SetToolTip( TheSSNoiseLayersParameter->Tooltip() );
   StarDetectorParameters_NoiseLayers_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_NoiseLayers_Sizer.SetSpacing( 4 );
   StarDetectorParameters_NoiseLayers_Sizer.Add( StarDetectorParameters_NoiseLayers_Label );
   StarDetectorParameters_NoiseLayers_Sizer.Add( StarDetectorParameters_NoiseLayers_Control );
   StarDetectorParameters_NoiseLayers_Sizer.AddStretch();

   StarDetectorParameters_HotPixelFilterRadius_Label.SetText( "Hot Pixel Filter:" );
   StarDetectorParameters_HotPixelFilterRadius_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_HotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StarDetectorParameters_HotPixelFilterRadius_Control.SetRange( TheSSHotPixelFilterRadiusParameter->MinimumValue(), TheSSHotPixelFilterRadiusParameter->MaximumValue() );
   StarDetectorParameters_HotPixelFilterRadius_Control.SetToolTip( TheSSHotPixelFilterRadiusParameter->Tooltip() );
   StarDetectorParameters_HotPixelFilterRadius_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_HotPixelFilterRadius_Sizer.SetSpacing( 4 );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Label );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Control );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.AddStretch();

   StarDetectorParameters_ApplyHotPixelFilter_Label.SetText( "Apply Hot Pixel Filter to Detection Image:" );
   StarDetectorParameters_ApplyHotPixelFilter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StarDetectorParameters_ApplyHotPixelFilter_Control.SetToolTip( TheSSApplyHotPixelFilterParameter->Tooltip() );
   StarDetectorParameters_ApplyHotPixelFilter_Control.OnCheck( (Button::check_event_handler)&SubframeSelectorInterface::__CheckboxUpdated, w );

   StarDetectorParameters_ApplyHotPixelFilter_Sizer.SetSpacing( 4 );
   StarDetectorParameters_ApplyHotPixelFilter_Sizer.Add( StarDetectorParameters_ApplyHotPixelFilter_Label );
   StarDetectorParameters_ApplyHotPixelFilter_Sizer.Add( StarDetectorParameters_ApplyHotPixelFilter_Control );
   StarDetectorParameters_ApplyHotPixelFilter_Sizer.AddStretch();

   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetText( "Noise Reduction Filter:" );
   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StarDetectorParameters_NoiseReductionFilterRadius_Control.SetRange( TheSSNoiseReductionFilterRadiusParameter->MinimumValue(), TheSSNoiseReductionFilterRadiusParameter->MaximumValue() );
   StarDetectorParameters_NoiseReductionFilterRadius_Control.SetToolTip( TheSSNoiseReductionFilterRadiusParameter->Tooltip() );
   StarDetectorParameters_NoiseReductionFilterRadius_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.Add( StarDetectorParameters_NoiseReductionFilterRadius_Label );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.Add( StarDetectorParameters_NoiseReductionFilterRadius_Control );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.AddStretch();

   StarDetectorParameters_Sensitivity_Control.label.SetText( "Sensitivity:" );
   StarDetectorParameters_Sensitivity_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_Sensitivity_Control.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_Sensitivity_Control.SetReal();
   StarDetectorParameters_Sensitivity_Control.SetRange( TheSSSensitivityParameter->MinimumValue(), TheSSSensitivityParameter->MaximumValue() );
   StarDetectorParameters_Sensitivity_Control.SetPrecision( TheSSSensitivityParameter->Precision() );
   StarDetectorParameters_Sensitivity_Control.SetToolTip( TheSSSensitivityParameter->Tooltip() );
   StarDetectorParameters_Sensitivity_Control.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_PeakResponse_Control.label.SetText( "Peak Response:" );
   StarDetectorParameters_PeakResponse_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_PeakResponse_Control.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_PeakResponse_Control.SetReal();
   StarDetectorParameters_PeakResponse_Control.SetRange( TheSSPeakResponseParameter->MinimumValue(), TheSSPeakResponseParameter->MaximumValue() );
   StarDetectorParameters_PeakResponse_Control.SetPrecision( TheSSPeakResponseParameter->Precision() );
   StarDetectorParameters_PeakResponse_Control.SetToolTip( TheSSPeakResponseParameter->Tooltip() );
   StarDetectorParameters_PeakResponse_Control.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_MaxDistortion_Control.label.SetText( "Max Distortion:" );
   StarDetectorParameters_MaxDistortion_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_MaxDistortion_Control.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_MaxDistortion_Control.SetReal();
   StarDetectorParameters_MaxDistortion_Control.SetRange( TheSSMaxDistortionParameter->MinimumValue(), TheSSMaxDistortionParameter->MaximumValue() );
   StarDetectorParameters_MaxDistortion_Control.SetPrecision( TheSSMaxDistortionParameter->Precision() );
   StarDetectorParameters_MaxDistortion_Control.SetToolTip( TheSSMaxDistortionParameter->Tooltip() );
   StarDetectorParameters_MaxDistortion_Control.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_UpperLimit_Control.label.SetText( "Upper Limit:" );
   StarDetectorParameters_UpperLimit_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_UpperLimit_Control.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_UpperLimit_Control.SetReal();
   StarDetectorParameters_UpperLimit_Control.SetRange( TheSSUpperLimitParameter->MinimumValue(), TheSSUpperLimitParameter->MaximumValue() );
   StarDetectorParameters_UpperLimit_Control.SetPrecision( TheSSUpperLimitParameter->Precision() );
   StarDetectorParameters_UpperLimit_Control.SetToolTip( TheSSUpperLimitParameter->Tooltip() );
   StarDetectorParameters_UpperLimit_Control.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_BackgroundExpansion_Label.SetText( "Background Expansion:" );
   StarDetectorParameters_BackgroundExpansion_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_BackgroundExpansion_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StarDetectorParameters_BackgroundExpansion_Control.SetRange( TheSSBackgroundExpansionParameter->MinimumValue(), TheSSBackgroundExpansionParameter->MaximumValue() );
   StarDetectorParameters_BackgroundExpansion_Control.SetToolTip( TheSSBackgroundExpansionParameter->Tooltip() );
   StarDetectorParameters_BackgroundExpansion_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_BackgroundExpansion_Sizer.SetSpacing( 4 );
   StarDetectorParameters_BackgroundExpansion_Sizer.Add( StarDetectorParameters_BackgroundExpansion_Label );
   StarDetectorParameters_BackgroundExpansion_Sizer.Add( StarDetectorParameters_BackgroundExpansion_Control );
   StarDetectorParameters_BackgroundExpansion_Sizer.AddStretch();

   StarDetectorParameters_XYStretch_Control.label.SetText( "XY Stretch:" );
   StarDetectorParameters_XYStretch_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_XYStretch_Control.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_XYStretch_Control.SetReal();
   StarDetectorParameters_XYStretch_Control.SetRange( TheSSXYStretchParameter->MinimumValue(), TheSSXYStretchParameter->MaximumValue() );
   StarDetectorParameters_XYStretch_Control.SetPrecision( TheSSXYStretchParameter->Precision() );
   StarDetectorParameters_XYStretch_Control.SetToolTip( TheSSXYStretchParameter->Tooltip() );
   StarDetectorParameters_XYStretch_Control.OnValueUpdated( (NumericEdit::value_event_handler)&SubframeSelectorInterface::__RealValueUpdated, w );

   currentLabelWidth = fnt.Width( String( "Height:" ) ); // the longest label text

   StarDetectorParameters_ROIX0_Label.SetText( "Left:" );
   StarDetectorParameters_ROIX0_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_ROIX0_Label.SetToolTip( "<p>X pixel coordinate of the upper-left corner of the ROI.</p>" );

   StarDetectorParameters_ROIX0_Control.SetRange( TheSSROIX0Parameter->MinimumValue(), TheSSROIX0Parameter->MaximumValue() );
   StarDetectorParameters_ROIX0_Control.SetToolTip( "<p>X pixel coordinate of the upper-left corner of the ROI.</p>" );
   StarDetectorParameters_ROIX0_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROIY0_Label.SetText( "Top:" );
   StarDetectorParameters_ROIY0_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_ROIY0_Label.SetToolTip( "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>" );

   StarDetectorParameters_ROIY0_Control.SetRange( TheSSROIY0Parameter->MinimumValue(), TheSSROIY0Parameter->MaximumValue() );
   StarDetectorParameters_ROIY0_Control.SetToolTip( "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>" );
   StarDetectorParameters_ROIY0_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROIRow1_Sizer.SetSpacing( 4 );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIX0_Label );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIX0_Control );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIY0_Label );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIY0_Control );
   StarDetectorParameters_ROIRow1_Sizer.AddStretch();

   StarDetectorParameters_ROIWidth_Label.SetText( "Width:" );
   StarDetectorParameters_ROIWidth_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_ROIWidth_Label.SetToolTip( "<p>Width of the ROI in pixels.</p>" );

   StarDetectorParameters_ROIWidth_Control.SetRange( TheSSROIX1Parameter->MinimumValue(), TheSSROIX1Parameter->MaximumValue() );
   StarDetectorParameters_ROIWidth_Control.SetToolTip( "<p>Width of the ROI in pixels.</p>" );
   StarDetectorParameters_ROIWidth_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROIHeight_Label.SetText( "Height:" );
   StarDetectorParameters_ROIHeight_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StarDetectorParameters_ROIHeight_Label.SetToolTip( "<p>Height of the ROI in pixels.</p>" );

   StarDetectorParameters_ROIHeight_Control.SetRange( TheSSROIY1Parameter->MinimumValue(), TheSSROIY1Parameter->MaximumValue() );
   StarDetectorParameters_ROIHeight_Control.SetToolTip( "<p>Height of the ROI in pixels.</p>" );
   StarDetectorParameters_ROIHeight_Control.OnValueUpdated( (SpinBox::value_event_handler)&SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROISelectPreview_Button.SetText( "From Preview" );
   StarDetectorParameters_ROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>" );
   StarDetectorParameters_ROISelectPreview_Button.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__ButtonClick, w );
   StarDetectorParameters_ROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&SubframeSelectorInterface::__ViewDrag, w );
   StarDetectorParameters_ROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&SubframeSelectorInterface::__ViewDrop, w );

   StarDetectorParameters_ROIRow2_Sizer.SetSpacing( 4 );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIWidth_Label );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIWidth_Control );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIHeight_Label );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIHeight_Control );
   StarDetectorParameters_ROIRow2_Sizer.AddSpacing( 12 );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROISelectPreview_Button );
   StarDetectorParameters_ROIRow2_Sizer.AddStretch();

   StarDetectorParameters_Sizer.SetSpacing( 4 );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_StructureLayers_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_NoiseLayers_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_ApplyHotPixelFilter_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_NoiseReductionFilterRadius_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_Sensitivity_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_PeakResponse_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_MaxDistortion_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_UpperLimit_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_BackgroundExpansion_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_XYStretch_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_ROIRow1_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_ROIRow2_Sizer );

   StarDetectorParameters_Control.SetSizer( StarDetectorParameters_Sizer );
   StarDetectorParameters_Control.AdjustToContents();

   //

   MeasurementImages_SectionBar.SetTitle( "Measurements" );
   MeasurementImages_SectionBar.SetSection( MeasurementImages_Control );
   MeasurementImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::__ToggleSection, w );

   MeasurementImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   MeasurementImages_TreeBox.SetNumberOfColumns( 5 );
   MeasurementImages_TreeBox.SetHeaderText( 0, "Index" );
   MeasurementImages_TreeBox.SetHeaderText( 1, "Output" );
   MeasurementImages_TreeBox.SetHeaderText( 2, "Lock" );
   MeasurementImages_TreeBox.SetHeaderText( 3, "Name" );
   MeasurementImages_TreeBox.SetHeaderText( 4, "FWHM" );
   MeasurementImages_TreeBox.EnableMultipleSelections();
   MeasurementImages_TreeBox.DisableRootDecoration();
   MeasurementImages_TreeBox.EnableAlternateRowColor();
   MeasurementImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler) &SubframeSelectorInterface::__MeasurementImages_CurrentNodeUpdated, w );
   MeasurementImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler) &SubframeSelectorInterface::__MeasurementImages_NodeActivated, w );

   MeasurementImages_Sizer.SetSpacing( 4 );
   MeasurementImages_Sizer.Add( MeasurementImages_TreeBox, 100 );

   MeasurementImages_Control.SetSizer( MeasurementImages_Sizer );
   MeasurementImages_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Routine_Sizer );
   Global_Sizer.Add( SubframeImages_SectionBar );
   Global_Sizer.Add( SubframeImages_Control );
   Global_Sizer.Add( SystemParameters_SectionBar );
   Global_Sizer.Add( SystemParameters_Control );
   Global_Sizer.Add( StarDetectorParameters_SectionBar );
   Global_Sizer.Add( StarDetectorParameters_Control );
   Global_Sizer.Add( MeasurementImages_SectionBar );
   Global_Sizer.Add( MeasurementImages_Control );

   w.SetSizer( Global_Sizer );

   SystemParameters_Control.Hide();
   StarDetectorParameters_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.cpp - Released 2017-08-01T14:26:58Z
