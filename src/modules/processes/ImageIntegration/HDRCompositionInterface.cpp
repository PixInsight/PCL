//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0312
// ----------------------------------------------------------------------------
// HDRCompositionInterface.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "HDRCompositionInterface.h"
#include "HDRCompositionProcess.h"

#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/MessageBox.h>
#include <pcl/PreviewSelectionDialog.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

HDRCompositionInterface* TheHDRCompositionInterface = 0;

// ----------------------------------------------------------------------------

#include "HDRCompositionIcon.xpm"

// ----------------------------------------------------------------------------

HDRCompositionInterface::HDRCompositionInterface() :
ProcessInterface(), instance( TheHDRCompositionProcess ), GUI( 0 )
{
   TheHDRCompositionInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. TreeBox) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

HDRCompositionInterface::~HDRCompositionInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString HDRCompositionInterface::Id() const
{
   return "HDRComposition";
}

MetaProcess* HDRCompositionInterface::Process() const
{
   return TheHDRCompositionProcess;
}

const char** HDRCompositionInterface::IconImageXPM() const
{
   return HDRCompositionIcon_XPM;
}

InterfaceFeatures HDRCompositionInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void HDRCompositionInterface::ResetInstance()
{
   HDRCompositionInstance defaultInstance( TheHDRCompositionProcess );
   ImportProcess( defaultInstance );
}

bool HDRCompositionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "HDRComposition" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheHDRCompositionProcess;
}

ProcessImplementation* HDRCompositionInterface::NewProcess() const
{
   return new HDRCompositionInstance( instance );
}

bool HDRCompositionInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const HDRCompositionInstance* r = dynamic_cast<const HDRCompositionInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not an HDRComposition instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool HDRCompositionInterface::RequiresInstanceValidation() const
{
   return true;
}

bool HDRCompositionInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

void HDRCompositionInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HDRCompositionInterface::UpdateControls()
{
   UpdateInputImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateHDRCompositionControls();
   UpdateFittingRegionControls();
}

void HDRCompositionInterface::UpdateInputImagesItem( size_type i )
{
   TreeBox::Node* node = GUI->InputImages_TreeBox[i];
   if ( node == 0 )
      return;

   const HDRCompositionInstance::ImageItem& item = instance.images[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   if ( GUI->FullPaths_CheckBox.IsChecked() )
      node->SetText( 2, item.path );
   else
      node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

void HDRCompositionInterface::UpdateInputImagesList()
{
   int currentIdx = GUI->InputImages_TreeBox.ChildIndex( GUI->InputImages_TreeBox.CurrentNode() );

   GUI->InputImages_TreeBox.DisableUpdates();
   GUI->InputImages_TreeBox.Clear();

   for ( size_type i = 0; i < instance.images.Length(); ++i )
   {
      new TreeBox::Node( GUI->InputImages_TreeBox );
      UpdateInputImagesItem( i );
   }

   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !instance.images.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->InputImages_TreeBox.NumberOfChildren() )
         GUI->InputImages_TreeBox.SetCurrentNode( GUI->InputImages_TreeBox[currentIdx] );

   GUI->InputImages_TreeBox.EnableUpdates();
}

void HDRCompositionInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->InputImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->InputImages_TreeBox.HasSelectedTopLevelNodes();

   TreeBox::Node* node = hasSelection ? GUI->InputImages_TreeBox.CurrentNode() : 0;
   if ( node != 0 )
   {
      int idx = GUI->InputImages_TreeBox.ChildIndex( node );
      GUI->MoveUp_PushButton.Enable( idx > 0 );
      GUI->MoveDown_PushButton.Enable( idx < int( instance.images.Length()-1 ) );
   }
   else
   {
      GUI->MoveUp_PushButton.Disable();
      GUI->MoveDown_PushButton.Disable();
   }

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
}

void HDRCompositionInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( instance.inputHints );
}

void HDRCompositionInterface::UpdateHDRCompositionControls()
{
   GUI->MaskBinarizingThreshold_NumericControl.SetValue( instance.maskBinarizingThreshold );
   GUI->MaskSmoothness_SpinBox.SetValue( instance.maskSmoothness );
   GUI->MaskGrowth_SpinBox.SetValue( instance.maskGrowth );
   GUI->AutoExposures_CheckBox.SetChecked( instance.autoExposures );
   GUI->RejectBlack_CheckBox.SetChecked( instance.rejectBlack );
   GUI->Generate64BitResult_CheckBox.SetChecked( instance.generate64BitResult );
   GUI->OutputMasks_CheckBox.SetChecked( instance.outputMasks );
   GUI->ClosePreviousImages_CheckBox.SetChecked( instance.closePreviousImages );
}

void HDRCompositionInterface::UpdateFittingRegionControls()
{
   GUI->FittingRegion_SectionBar.SetChecked( instance.useFittingRegion );
   GUI->FittingRectX0_SpinBox.SetValue( instance.fittingRect.x0 );
   GUI->FittingRectY0_SpinBox.SetValue( instance.fittingRect.y0 );
   GUI->FittingRectWidth_SpinBox.SetValue( instance.fittingRect.Width() );
   GUI->FittingRectHeight_SpinBox.SetValue( instance.fittingRect.Height() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HDRCompositionInterface::__InputImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.images.Length() )
      throw Error( "HDRCompositionInterface: *Warning* Corrupted interface structures" );

   // ### If there's something that depends on which image is selected in the list, do it now.
}

void HDRCompositionInterface::__InputImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.images.Length() )
      throw Error( "HDRCompositionInterface: *Warning* Corrupted interface structures" );

   HDRCompositionInstance::ImageItem& item = instance.images[index];

   switch ( col )
   {
   case 0:
      break;
   case 1:
      item.enabled = !item.enabled;
      UpdateInputImagesItem( index );
      break;
   case 2:
      {
         Array<ImageWindow> w = ImageWindow::Open( item.path );
         for ( Array<ImageWindow>::iterator i = w.Begin(); i != w.End(); ++i )
            i->Show();
      }
      break;
   }
}

void HDRCompositionInterface::__InputImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != 0) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

void HDRCompositionInterface::__InputImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "HDRComposition: Select Input Images" );

      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->InputImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.images.Insert( instance.images.At( i0++ ), HDRCompositionInstance::ImageItem( *i ) );

         UpdateInputImagesList();
         UpdateImageSelectionButtons();
      }
   }
   else if ( sender == GUI->MoveUp_PushButton )
   {
      TreeBox::Node* node = GUI->InputImages_TreeBox.CurrentNode();
      if ( node != 0 )
      {
         int idx = GUI->InputImages_TreeBox.ChildIndex( node );
         if ( idx > 0 )
         {
            HDRCompositionInstance::ImageItem item = instance.images[idx];
            instance.images.Remove( instance.images.At( idx ) );
            instance.images.Insert( instance.images.At( idx-1 ), item );
            UpdateInputImagesList();
            GUI->InputImages_TreeBox.SetCurrentNode( GUI->InputImages_TreeBox[idx-1] );
            UpdateImageSelectionButtons();
         }
      }
   }
   else if ( sender == GUI->MoveDown_PushButton )
   {
      TreeBox::Node* node = GUI->InputImages_TreeBox.CurrentNode();
      if ( node != 0 )
      {
         int idx = GUI->InputImages_TreeBox.ChildIndex( node );
         if ( idx < int( instance.images.Length()-1 ) )
         {
            HDRCompositionInstance::ImageItem item = instance.images[idx];
            instance.images.Remove( instance.images.At( idx ) );
            instance.images.Insert( instance.images.At( idx+1 ), item );
            UpdateInputImagesList();
            GUI->InputImages_TreeBox.SetCurrentNode( GUI->InputImages_TreeBox[idx+1] );
            UpdateImageSelectionButtons();
         }
      }
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->InputImages_TreeBox.SelectAllNodes();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->InputImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->InputImages_TreeBox[i]->Select( !GUI->InputImages_TreeBox[i]->IsSelected() );

      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->InputImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->InputImages_TreeBox[i]->IsSelected() )
            instance.images[i].enabled = !instance.images[i].enabled;

      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      HDRCompositionInstance::image_list newImages;
      for ( int i = 0, n = GUI->InputImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->InputImages_TreeBox[i]->IsSelected() )
            newImages.Add( instance.images[i] );
      instance.images = newImages;

      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      instance.images.Clear();
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
}

void HDRCompositionInterface::__FormatHints_EditCompleted( Edit& sender )
{
   String hints = sender.Text().Trimmed();

   if ( sender == GUI->InputHints_Edit )
      instance.inputHints = hints;

   sender.SetText( hints );
}

void HDRCompositionInterface::__HDRComposition_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->MaskBinarizingThreshold_NumericControl )
      instance.maskBinarizingThreshold = value;
}

void HDRCompositionInterface::__HDRComposition_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->MaskSmoothness_SpinBox )
      instance.maskSmoothness = value;
   else if ( sender == GUI->MaskGrowth_SpinBox )
      instance.maskGrowth = value;
}

void HDRCompositionInterface::__HDRComposition_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AutoExposures_CheckBox )
      instance.autoExposures = checked;
   else if ( sender == GUI->RejectBlack_CheckBox )
      instance.rejectBlack = checked;
   else if ( sender == GUI->Generate64BitResult_CheckBox )
      instance.generate64BitResult = checked;
   else if ( sender == GUI->OutputMasks_CheckBox )
      instance.outputMasks = checked;
   else if ( sender == GUI->ClosePreviousImages_CheckBox )
      instance.closePreviousImages = checked;
}

void HDRCompositionInterface::__FittingRegion_Check( SectionBar& sender, bool checked )
{
   if ( sender == GUI->FittingRegion_SectionBar )
      instance.useFittingRegion = checked;
}

void HDRCompositionInterface::__FittingRegion_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->FittingRectX0_SpinBox )
      instance.fittingRect.x0 = value;
   else if ( sender == GUI->FittingRectY0_SpinBox )
      instance.fittingRect.y0 = value;
   else if ( sender == GUI->FittingRectWidth_SpinBox )
      instance.fittingRect.x1 = instance.fittingRect.x0 + value;
   else if ( sender == GUI->FittingRectHeight_SpinBox )
      instance.fittingRect.y1 = instance.fittingRect.y0 + value;
}

void HDRCompositionInterface::__FittingRegion_Click( Button& sender, bool checked )
{
   if ( sender == GUI->SelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               instance.fittingRect = view.Window().PreviewRect( view.Id() );
               UpdateFittingRegionControls();
            }
         }
   }
}

void HDRCompositionInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->InputImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->InputImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->InputImages_TreeBox.SetMaxHeight( int_max );

      if ( GUI->InputImages_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

HDRCompositionInterface::GUIData::GUIData( HDRCompositionInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Binarizing threshold:" ) + 'T' );
   int spinWidth1 = fnt.Width( String( '0', 11 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   InputImages_SectionBar.SetTitle( "Input Images" );
   InputImages_SectionBar.SetSection( InputImages_Control );
   InputImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&HDRCompositionInterface::__ToggleSection, w );

   InputImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   InputImages_TreeBox.SetNumberOfColumns( 3 );
   InputImages_TreeBox.HideHeader();
   InputImages_TreeBox.EnableMultipleSelections();
   InputImages_TreeBox.DisableRootDecoration();
   InputImages_TreeBox.EnableAlternateRowColor();
   InputImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&HDRCompositionInterface::__InputImages_CurrentNodeUpdated, w );
   InputImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&HDRCompositionInterface::__InputImages_NodeActivated, w );
   InputImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&HDRCompositionInterface::__InputImages_NodeSelectionUpdated, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of input images.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   MoveUp_PushButton.SetText( "Move Up" );
   MoveUp_PushButton.SetToolTip( "<p>Move the selected item upward in the list.</p>" );
   MoveUp_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   MoveDown_PushButton.SetText( "Move Down" );
   MoveDown_PushButton.SetToolTip( "<p>Move the selected item downward in the list.</p>" );
   MoveDown_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all input images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of input images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected input images.</p>"
      "<p>Disabled input images will be ignored during the HDR composition process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected input images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for input image files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__InputImages_Click, w );

   InputButtons_Sizer.SetSpacing( 4 );
   InputButtons_Sizer.Add( AddFiles_PushButton );
   InputButtons_Sizer.Add( MoveUp_PushButton );
   InputButtons_Sizer.Add( MoveDown_PushButton );
   InputButtons_Sizer.Add( SelectAll_PushButton );
   InputButtons_Sizer.Add( InvertSelection_PushButton );
   InputButtons_Sizer.Add( ToggleSelected_PushButton );
   InputButtons_Sizer.Add( RemoveSelected_PushButton );
   InputButtons_Sizer.Add( Clear_PushButton );
   InputButtons_Sizer.Add( FullPaths_CheckBox );
   InputButtons_Sizer.AddStretch();

   InputImages_Sizer.SetSpacing( 4 );
   InputImages_Sizer.Add( InputImages_TreeBox, 100 );
   InputImages_Sizer.Add( InputButtons_Sizer );

   InputImages_Control.SetSizer( InputImages_Sizer );
   InputImages_Control.AdjustToContents();

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&HDRCompositionInterface::__ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings "
      "for image files used by specific processes. In HDRComposition, input hints change the way input images "
      "of some particular file formats are loaded during the integration process. There are no output hints in "
      "HDRComposition since this process does not write images to disk files.</p>"
      "<p>For example, you can use the \"raw\" hint to force the DSLR_RAW format to load pure raw images without "
      "applying any deBayering, interpolation, white balance or black point correction. You can also specify the "
      "\"lower-range\" and \"upper-range\" hints to load floating point FITS and TIFF files generated by other "
      "applications that don't use PixInsight's normalized [0,1] range. Most standard file format modules support "
      "hints; each format supports a number of input and/or output hints that you can use for different purposes "
      "with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&HDRCompositionInterface::__FormatHints_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );
   FormatHints_Control.AdjustToContents();

   //

   HDRComposition_SectionBar.SetTitle( "HDR Composition" );
   HDRComposition_SectionBar.SetSection( HDRComposition_Control );
   HDRComposition_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&HDRCompositionInterface::__ToggleSection, w );

   //

   MaskBinarizingThreshold_NumericControl.label.SetText( "Binarizing threshold:" );
   MaskBinarizingThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   MaskBinarizingThreshold_NumericControl.slider.SetRange( 0, 100 );
   MaskBinarizingThreshold_NumericControl.slider.SetScaledMinWidth( 250 );
   MaskBinarizingThreshold_NumericControl.SetReal();
   MaskBinarizingThreshold_NumericControl.SetRange( TheHCMaskBinarizingThresholdParameter->MinimumValue(), TheHCMaskBinarizingThresholdParameter->MaximumValue() );
   MaskBinarizingThreshold_NumericControl.SetPrecision( TheHCMaskBinarizingThresholdParameter->Precision() );
   MaskBinarizingThreshold_NumericControl.SetToolTip(
      "<p>The HDR composition process performs a masked combination of each successive pair of images, in "
      "decreasing order of relative exposure. For each pair of combined images, the binarizing threshold "
      "defines the regions of the long-exposure image that will be replaced with pixels from the short-exposure "
      "image.</p>"
      "<p>Too high of a binarizing threshold may prevent appropriate repairing of saturated regions with "
      "the corresponding pixels from unsaturated images. A too low threshold may cause the opposite effect, "
      "leading to a result with insufficient contributions of large exposures.</p>" );
   MaskBinarizingThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&HDRCompositionInterface::__HDRComposition_EditValueUpdated, w );

   //

   const char* maskSmoothnessToolTip =
      "<p>Defines the amount of low-pass filtering applied to smooth the edges of HDR combination masks.</p>"
      "<p>Low-pass filtering is necessary to avoid artifacts caused by hard transitions on the regions where "
      "successive images are combined.</p>";

   MaskSmoothness_Label.SetText( "Mask smoothness:" );
   MaskSmoothness_Label.SetFixedWidth( labelWidth1 );
   MaskSmoothness_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MaskSmoothness_Label.SetToolTip( maskSmoothnessToolTip );

   MaskSmoothness_SpinBox.SetRange( int( TheHCMaskSmoothnessParameter->MinimumValue() ), int( TheHCMaskSmoothnessParameter->MaximumValue() ) );
   MaskSmoothness_SpinBox.SetToolTip( maskSmoothnessToolTip );
   MaskSmoothness_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HDRCompositionInterface::__HDRComposition_SpinValueUpdated, w );

   MaskSmoothness_Sizer.SetSpacing( 4 );
   MaskSmoothness_Sizer.Add( MaskSmoothness_Label );
   MaskSmoothness_Sizer.Add( MaskSmoothness_SpinBox );
   MaskSmoothness_Sizer.AddStretch();

   //

   const char* maskGrowthToolTip =
      "<p>HDR combination masks can be expanded by applying a special dilation filter.</p>"
      "<p>Mask growth can be useful to repair highly saturated areas, especially when these areas are "
      "surrounded by artifacts. For example, this option is particularly useful to fix bloomings, as they "
      "are often surrounded by dark artifacts.</p>";

   MaskGrowth_Label.SetText( "Mask growth:" );
   MaskGrowth_Label.SetFixedWidth( labelWidth1 );
   MaskGrowth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MaskGrowth_Label.SetToolTip( maskGrowthToolTip );

   MaskGrowth_SpinBox.SetRange( int( TheHCMaskGrowthParameter->MinimumValue() ), int( TheHCMaskGrowthParameter->MaximumValue() ) );
   MaskGrowth_SpinBox.SetToolTip( maskGrowthToolTip );
   MaskGrowth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HDRCompositionInterface::__HDRComposition_SpinValueUpdated, w );

   MaskGrowth_Sizer.SetSpacing( 4 );
   MaskGrowth_Sizer.Add( MaskGrowth_Label );
   MaskGrowth_Sizer.Add( MaskGrowth_SpinBox );
   MaskGrowth_Sizer.AddStretch();

   //

   AutoExposures_CheckBox.SetText( "Automatic exposure evaluation" );
   AutoExposures_CheckBox.SetToolTip( "<p>If this option is enabled, HDRComposition will sort its input images "
      "automatically by their computed relative exposures. If this option is disabled, the list of input images "
      "must be sorted by decreasing exposure order (the most exposed image at the top of the list). If the images "
      "are not correctly sorted by decreasing exposure, the HDR composition process will either fail or yield an "
      "erroneous result.</p>"
      "<p>This option is enabled by default, as automatic exposure evaluation is a very fast and robust process "
      "that should never fail under normal conditions. Note that the automatic exposure evaluation routine does "
      "not depend on existing image metadata, as FITS headers or EXIF data (which are simply ignored by this tool). "
      "The computed relative exposures depend exclusively on actual pixel values and their statistical properties.</p>");
   AutoExposures_CheckBox.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__HDRComposition_Click, w );

   AutoExposures_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoExposures_Sizer.Add( AutoExposures_CheckBox );
   AutoExposures_Sizer.AddStretch();

   //

   RejectBlack_CheckBox.SetText( "Reject black pixels" );
   RejectBlack_CheckBox.SetToolTip( "<p>If this option is selected, HDRComposition will ignore black pixels to "
   "perform HDR compositions, and hence black pixels will never replace saturated pixels, or pixels above the "
   "binarizing threshold. This option should normally be enabled, as it is by default.</p>" );
   RejectBlack_CheckBox.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__HDRComposition_Click, w );

   RejectBlack_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   RejectBlack_Sizer.Add( RejectBlack_CheckBox );
   RejectBlack_Sizer.AddStretch();

   //

   Generate64BitResult_CheckBox.SetText( "Generate a 64-bit HDR image" );
   Generate64BitResult_CheckBox.SetToolTip( "<p>If this option is selected, HDRComposition will generate a "
      "64-bit floating point result image. Otherwise the HDR composition result will be generated as a 32-bit "
      "floating point image.</p>"
      "<p>A 32-bit floating point image is sufficient to represent HDR compositions of small or moderate size. "
      "Keep in mind that a 32-bit image provides room for a maximum of about 10<sup>7</sup> discrete sample values, "
      "or approximately 153 times the range of a 16-bit image. Very large HDR images may easily exceed that limit "
      "and hence require a 64-bit floating point result, which provides a huge space of 10<sup>15</sup> discrete "
      "values.</p>"
      "<p>Note that the words 'small' and 'large' here don't refer to the number of images in an HDR composition, "
      "but to the length of the required <i>numeric range</i> to represent the whole set of tonal values in the "
      "final HDR result.</p>");
   Generate64BitResult_CheckBox.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__HDRComposition_Click, w );

   Generate64BitResult_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Generate64BitResult_Sizer.Add( Generate64BitResult_CheckBox );
   Generate64BitResult_Sizer.AddStretch();

   //

   OutputMasks_CheckBox.SetText( "Output composition masks" );
   OutputMasks_CheckBox.SetToolTip( "<p>If this option is selected, HDRComposition will generate a new "
      "image window with the HDR composition mask applied to combine each pair of successive input images. "
      "These masks allow tight control over the HDR composition process.</p>" );
   OutputMasks_CheckBox.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__HDRComposition_Click, w );

   OutputMasks_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputMasks_Sizer.Add( OutputMasks_CheckBox );
   OutputMasks_Sizer.AddStretch();

   //

   ClosePreviousImages_CheckBox.SetText( "Close previous images" );
   ClosePreviousImages_CheckBox.SetToolTip( "<p>Select this option to close existing HDR and composition "
      "mask images before running a new HDR composition process. This is useful to avoid accumulation of multiple "
      "results on the workspace, when the same HDR composition is being tested repeatedly.</p>" );
   ClosePreviousImages_CheckBox.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__HDRComposition_Click, w );

   ClosePreviousImages_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClosePreviousImages_Sizer.Add( ClosePreviousImages_CheckBox );
   ClosePreviousImages_Sizer.AddStretch();

   //

   HDRComposition_Sizer.SetSpacing( 4 );
   HDRComposition_Sizer.Add( MaskBinarizingThreshold_NumericControl );
   HDRComposition_Sizer.Add( MaskSmoothness_Sizer );
   HDRComposition_Sizer.Add( MaskGrowth_Sizer );
   HDRComposition_Sizer.Add( AutoExposures_Sizer );
   HDRComposition_Sizer.Add( RejectBlack_Sizer );
   HDRComposition_Sizer.Add( Generate64BitResult_Sizer );
   HDRComposition_Sizer.Add( OutputMasks_Sizer );
   HDRComposition_Sizer.Add( ClosePreviousImages_Sizer );

   HDRComposition_Control.SetSizer( HDRComposition_Sizer );

   //

   FittingRegion_SectionBar.SetTitle( "Fitting Region" );
   FittingRegion_SectionBar.EnableTitleCheckBox();
   FittingRegion_SectionBar.OnCheck( (SectionBar::check_event_handler)&HDRCompositionInterface::__FittingRegion_Check, w );
   FittingRegion_SectionBar.SetSection( FittingRegion_Control );
   FittingRegion_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&HDRCompositionInterface::__ToggleSection, w );

   const char* frX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the fitting rectangle.</p>";

   FittingRectX0_Label.SetText( "Left:" );
   FittingRectX0_Label.SetFixedWidth( labelWidth1 );
   FittingRectX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FittingRectX0_Label.SetToolTip( frX0ToolTip );

   FittingRectX0_SpinBox.SetRange( 0, int_max );
   FittingRectX0_SpinBox.SetFixedWidth( spinWidth1 );
   FittingRectX0_SpinBox.SetToolTip( frX0ToolTip );
   FittingRectX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HDRCompositionInterface::__FittingRegion_SpinValueUpdated, w );

   FittingRectX0_Sizer.SetSpacing( 4 );
   FittingRectX0_Sizer.Add( FittingRectX0_Label );
   FittingRectX0_Sizer.Add( FittingRectX0_SpinBox );
   FittingRectX0_Sizer.AddStretch();

   const char* frY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the fitting rectangle.</p>";

   FittingRectY0_Label.SetText( "Top:" );
   FittingRectY0_Label.SetFixedWidth( labelWidth1 );
   FittingRectY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FittingRectY0_Label.SetToolTip( frY0ToolTip );

   FittingRectY0_SpinBox.SetRange( 0, int_max );
   FittingRectY0_SpinBox.SetFixedWidth( spinWidth1 );
   FittingRectY0_SpinBox.SetToolTip( frY0ToolTip );
   FittingRectY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HDRCompositionInterface::__FittingRegion_SpinValueUpdated, w );

   FittingRectY0_Sizer.SetSpacing( 4 );
   FittingRectY0_Sizer.Add( FittingRectY0_Label );
   FittingRectY0_Sizer.Add( FittingRectY0_SpinBox );
   FittingRectY0_Sizer.AddStretch();

   const char* frWidthToolTip = "<p>Width of the fitting rectangle in pixels.</p>";

   FittingRectWidth_Label.SetText( "Width:" );
   FittingRectWidth_Label.SetFixedWidth( labelWidth1 );
   FittingRectWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FittingRectWidth_Label.SetToolTip( frWidthToolTip );

   FittingRectWidth_SpinBox.SetRange( 0, int_max );
   FittingRectWidth_SpinBox.SetFixedWidth( spinWidth1 );
   FittingRectWidth_SpinBox.SetToolTip( frWidthToolTip );
   FittingRectWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HDRCompositionInterface::__FittingRegion_SpinValueUpdated, w );

   FittingRectWidth_Sizer.SetSpacing( 4 );
   FittingRectWidth_Sizer.Add( FittingRectWidth_Label );
   FittingRectWidth_Sizer.Add( FittingRectWidth_SpinBox );
   FittingRectWidth_Sizer.AddStretch();

   const char* frHeightToolTip = "<p>Height of the fitting rectangle in pixels.</p>";

   FittingRectHeight_Label.SetText( "Height:" );
   FittingRectHeight_Label.SetFixedWidth( labelWidth1 );
   FittingRectHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FittingRectHeight_Label.SetToolTip( frHeightToolTip );

   FittingRectHeight_SpinBox.SetRange( 0, int_max );
   FittingRectHeight_SpinBox.SetFixedWidth( spinWidth1 );
   FittingRectHeight_SpinBox.SetToolTip( frHeightToolTip );
   FittingRectHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HDRCompositionInterface::__FittingRegion_SpinValueUpdated, w );

   SelectPreview_Button.SetText( "From Preview" );
   SelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>" );
   SelectPreview_Button.OnClick( (Button::click_event_handler)&HDRCompositionInterface::__FittingRegion_Click, w );

   FittingRectHeight_Sizer.SetSpacing( 4 );
   FittingRectHeight_Sizer.Add( FittingRectHeight_Label );
   FittingRectHeight_Sizer.Add( FittingRectHeight_SpinBox );
   FittingRectHeight_Sizer.AddStretch();
   FittingRectHeight_Sizer.Add( SelectPreview_Button );

   FittingRegion_Sizer.SetSpacing( 4 );
   FittingRegion_Sizer.Add( FittingRectX0_Sizer );
   FittingRegion_Sizer.Add( FittingRectY0_Sizer );
   FittingRegion_Sizer.Add( FittingRectWidth_Sizer );
   FittingRegion_Sizer.Add( FittingRectHeight_Sizer );

   FittingRegion_Control.SetSizer( FittingRegion_Sizer );
   FittingRegion_Control.AdjustToContents();

   FittingRegion_Control.SetToolTip( "<p>HDRComposition performs a linear fit to match the tonal ranges of each "
   "pair of combined images. Linear fitting can be restricted to a user-defined rectangular region of interest, "
   "which has two main advantages: (1) linear fitting is a relatively expensive process and restricting it to a "
   "smaller region improves performance, and (2) by restricting the region of pixels used to compute linear fitting "
   "functions, accuracy can be improved in case there are small flat fielding errors (nonuniform illumination) "
   "near the edges of the images, which is a frequent problem.</p>"
   "<p>When enabled, the fitting rectangle must define a nonempty subset of the input images. When the fitting region "
   "is disabled (the default option), the whole image is used to compute linear fitting functions.</p>" );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( InputImages_SectionBar );
   Global_Sizer.Add( InputImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( HDRComposition_SectionBar );
   Global_Sizer.Add( HDRComposition_Control );
   Global_Sizer.Add( FittingRegion_SectionBar );
   Global_Sizer.Add( FittingRegion_Control );

   w.SetSizer( Global_Sizer );
   w.SetFixedWidth();

   FormatHints_Control.Hide();
   FittingRegion_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HDRCompositionInterface.cpp - Released 2015/11/26 16:00:13 UTC
