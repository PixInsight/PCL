//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0356
// ----------------------------------------------------------------------------
// ImageCalibrationInterface.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "ImageCalibrationInterface.h"
#include "ImageCalibrationProcess.h"

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

ImageCalibrationInterface* TheImageCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "ImageCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

ImageCalibrationInterface::ImageCalibrationInterface() :
   instance( TheImageCalibrationProcess )
{
   TheImageCalibrationInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. TreeBox) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

ImageCalibrationInterface::~ImageCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ImageCalibrationInterface::Id() const
{
   return "ImageCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* ImageCalibrationInterface::Process() const
{
   return TheImageCalibrationProcess;
}

// ----------------------------------------------------------------------------

const char** ImageCalibrationInterface::IconImageXPM() const
{
   return ImageCalibrationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures ImageCalibrationInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::ResetInstance()
{
   ImageCalibrationInstance defaultInstance( TheImageCalibrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ImageCalibration" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheImageCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ImageCalibrationInterface::NewProcess() const
{
   return new ImageCalibrationInstance( instance );
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ImageCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an ImageCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ImageCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateControls()
{
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateOutputFilesControls();
   UpdatePedestalControls();
   UpdateMasterFrameControls();
   UpdateOverscanControls();
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const ImageCalibrationInstance::ImageItem& item = instance.targetFrames[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   if ( GUI->FullPaths_CheckBox.IsChecked() )
      node->SetText( 2, item.path );
   else
      node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateTargetImagesList()
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

void ImageCalibrationInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->TargetImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->TargetImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
   //GUI->FullPaths_CheckBox.Enable( hasItems );   // always enabled
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( instance.inputHints );
   GUI->OutputHints_Edit.SetText( instance.outputHints );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( instance.outputDirectory );

   GUI->OutputPrefix_Edit.SetText( instance.outputPrefix );

   GUI->OutputPostfix_Edit.SetText( instance.outputPostfix );

   GUI->OutputSampleFormat_ComboBox.SetCurrentItem( instance.outputSampleFormat );

   GUI->OutputPedestal_SpinBox.SetValue( instance.outputPedestal );

   GUI->EvaluateNoise_CheckBox.SetChecked( instance.evaluateNoise );

   GUI->NoiseEvaluation_Label.Enable( instance.evaluateNoise );
   GUI->NoiseEvaluation_ComboBox.SetCurrentItem( instance.noiseEvaluationAlgorithm );

   GUI->NoiseEvaluation_ComboBox.Enable( instance.evaluateNoise );

   GUI->OverwriteExistingFiles_CheckBox.SetChecked( instance.overwriteExistingFiles );

   GUI->OnError_ComboBox.SetCurrentItem( instance.onError );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdatePedestalControls()
{
   GUI->PedestalMode_ComboBox.SetCurrentItem( instance.pedestalMode );

   GUI->PedestalValue_Label.Enable( instance.pedestalMode == ICPedestalMode::Literal );

   GUI->PedestalValue_SpinBox.SetValue( instance.pedestal );
   GUI->PedestalValue_SpinBox.Enable( instance.pedestalMode == ICPedestalMode::Literal );

   GUI->PedestalKeyword_Label.Enable( instance.pedestalMode == ICPedestalMode::CustomKeyword );

   GUI->PedestalKeyword_Edit.SetText( instance.pedestalKeyword );
   GUI->PedestalKeyword_Edit.Enable( instance.pedestalMode == ICPedestalMode::CustomKeyword );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateMasterFrameControls()
{
   GUI->MasterBias_SectionBar.SetChecked( instance.masterBias.enabled );

   GUI->MasterBiasPath_Edit.SetText( instance.masterBias.path );

   GUI->CalibrateMasterBias_CheckBox.SetChecked( instance.calibrateBias );

   GUI->MasterDark_SectionBar.SetChecked( instance.masterDark.enabled );

   GUI->MasterDarkPath_Edit.SetText( instance.masterDark.path );

   GUI->CalibrateMasterDark_CheckBox.SetChecked( instance.calibrateDark );

   GUI->OptimizeDarks_CheckBox.SetChecked( instance.optimizeDarks );

   GUI->DarkOptimizationThreshold_NumericControl.SetValue( instance.darkOptimizationLow );
   GUI->DarkOptimizationThreshold_NumericControl.Enable( instance.optimizeDarks );

   GUI->DarkOptimizationWindow_Label.Enable( instance.optimizeDarks );

   GUI->DarkOptimizationWindow_SpinBox.SetValue( instance.darkOptimizationWindow );
   GUI->DarkOptimizationWindow_SpinBox.Enable( instance.optimizeDarks );

   GUI->DarkCFADetectionMode_Label.Enable( instance.optimizeDarks );

   GUI->DarkCFADetectionMode_ComboBox.SetCurrentItem( instance.darkCFADetectionMode );
   GUI->DarkCFADetectionMode_ComboBox.Enable( instance.optimizeDarks );

   GUI->MasterFlat_SectionBar.SetChecked( instance.masterFlat.enabled );

   GUI->MasterFlatPath_Edit.SetText( instance.masterFlat.path );

   GUI->CalibrateMasterFlat_CheckBox.SetChecked( instance.calibrateFlat );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::UpdateOverscanControls()
{
   GUI->Overscan_SectionBar.SetChecked( instance.overscan.enabled );

   GUI->ImageRegionX0_NumericEdit.SetValue( instance.overscan.imageRect.x0 );
   GUI->ImageRegionY0_NumericEdit.SetValue( instance.overscan.imageRect.y0 );
   GUI->ImageRegionWidth_NumericEdit.SetValue( instance.overscan.imageRect.Width() );
   GUI->ImageRegionHeight_NumericEdit.SetValue( instance.overscan.imageRect.Height() );

   const ImageCalibrationInstance::OverscanRegions& ov1 = instance.overscan.overscan[0];
   const ImageCalibrationInstance::OverscanRegions& ov2 = instance.overscan.overscan[1];
   const ImageCalibrationInstance::OverscanRegions& ov3 = instance.overscan.overscan[2];
   const ImageCalibrationInstance::OverscanRegions& ov4 = instance.overscan.overscan[3];

   bool en1 = ov1.enabled;
   bool en2 = ov2.enabled;
   bool en3 = ov3.enabled;
   bool en4 = ov4.enabled;

   GUI->Overscan1_CheckBox.SetChecked( en1 );

   GUI->Overscan1Source_Label.Enable( en1 );
   GUI->Overscan1SourceX0_NumericEdit.SetValue( ov1.sourceRect.x0 );
   GUI->Overscan1SourceX0_NumericEdit.Enable( en1 );
   GUI->Overscan1SourceY0_NumericEdit.SetValue( ov1.sourceRect.y0 );
   GUI->Overscan1SourceY0_NumericEdit.Enable( en1 );
   GUI->Overscan1SourceWidth_NumericEdit.SetValue( ov1.sourceRect.Width() );
   GUI->Overscan1SourceWidth_NumericEdit.Enable( en1 );
   GUI->Overscan1SourceHeight_NumericEdit.SetValue( ov1.sourceRect.Height() );
   GUI->Overscan1SourceHeight_NumericEdit.Enable( en1 );

   GUI->Overscan1Target_Label.Enable( en1 );
   GUI->Overscan1TargetX0_NumericEdit.SetValue( ov1.targetRect.x0 );
   GUI->Overscan1TargetX0_NumericEdit.Enable( en1 );
   GUI->Overscan1TargetY0_NumericEdit.SetValue( ov1.targetRect.y0 );
   GUI->Overscan1TargetY0_NumericEdit.Enable( en1 );
   GUI->Overscan1TargetWidth_NumericEdit.SetValue( ov1.targetRect.Width() );
   GUI->Overscan1TargetWidth_NumericEdit.Enable( en1 );
   GUI->Overscan1TargetHeight_NumericEdit.SetValue( ov1.targetRect.Height() );
   GUI->Overscan1TargetHeight_NumericEdit.Enable( en1 );

   GUI->Overscan2_CheckBox.SetChecked( en2 );

   GUI->Overscan2Source_Label.Enable( en2 );
   GUI->Overscan2SourceX0_NumericEdit.SetValue( ov2.sourceRect.x0 );
   GUI->Overscan2SourceX0_NumericEdit.Enable( en2 );
   GUI->Overscan2SourceY0_NumericEdit.SetValue( ov2.sourceRect.y0 );
   GUI->Overscan2SourceY0_NumericEdit.Enable( en2 );
   GUI->Overscan2SourceWidth_NumericEdit.SetValue( ov2.sourceRect.Width() );
   GUI->Overscan2SourceWidth_NumericEdit.Enable( en2 );
   GUI->Overscan2SourceHeight_NumericEdit.SetValue( ov2.sourceRect.Height() );
   GUI->Overscan2SourceHeight_NumericEdit.Enable( en2 );

   GUI->Overscan2Target_Label.Enable( en2 );
   GUI->Overscan2TargetX0_NumericEdit.SetValue( ov2.targetRect.x0 );
   GUI->Overscan2TargetX0_NumericEdit.Enable( en2 );
   GUI->Overscan2TargetY0_NumericEdit.SetValue( ov2.targetRect.y0 );
   GUI->Overscan2TargetY0_NumericEdit.Enable( en2 );
   GUI->Overscan2TargetWidth_NumericEdit.SetValue( ov2.targetRect.Width() );
   GUI->Overscan2TargetWidth_NumericEdit.Enable( en2 );
   GUI->Overscan2TargetHeight_NumericEdit.SetValue( ov2.targetRect.Height() );
   GUI->Overscan2TargetHeight_NumericEdit.Enable( en2 );

   GUI->Overscan3_CheckBox.SetChecked( en3 );

   GUI->Overscan3Source_Label.Enable( en3 );
   GUI->Overscan3SourceX0_NumericEdit.SetValue( ov3.sourceRect.x0 );
   GUI->Overscan3SourceX0_NumericEdit.Enable( en3 );
   GUI->Overscan3SourceY0_NumericEdit.SetValue( ov3.sourceRect.y0 );
   GUI->Overscan3SourceY0_NumericEdit.Enable( en3 );
   GUI->Overscan3SourceWidth_NumericEdit.SetValue( ov3.sourceRect.Width() );
   GUI->Overscan3SourceWidth_NumericEdit.Enable( en3 );
   GUI->Overscan3SourceHeight_NumericEdit.SetValue( ov3.sourceRect.Height() );
   GUI->Overscan3SourceHeight_NumericEdit.Enable( en3 );

   GUI->Overscan3Target_Label.Enable( en3 );
   GUI->Overscan3TargetX0_NumericEdit.SetValue( ov3.targetRect.x0 );
   GUI->Overscan3TargetX0_NumericEdit.Enable( en3 );
   GUI->Overscan3TargetY0_NumericEdit.SetValue( ov3.targetRect.y0 );
   GUI->Overscan3TargetY0_NumericEdit.Enable( en3 );
   GUI->Overscan3TargetWidth_NumericEdit.SetValue( ov3.targetRect.Width() );
   GUI->Overscan3TargetWidth_NumericEdit.Enable( en3 );
   GUI->Overscan3TargetHeight_NumericEdit.SetValue( ov3.targetRect.Height() );
   GUI->Overscan3TargetHeight_NumericEdit.Enable( en3 );

   GUI->Overscan4_CheckBox.SetChecked( en4 );

   GUI->Overscan4Source_Label.Enable( en4 );
   GUI->Overscan4SourceX0_NumericEdit.SetValue( ov4.sourceRect.x0 );
   GUI->Overscan4SourceX0_NumericEdit.Enable( en4 );
   GUI->Overscan4SourceY0_NumericEdit.SetValue( ov4.sourceRect.y0 );
   GUI->Overscan4SourceY0_NumericEdit.Enable( en4 );
   GUI->Overscan4SourceWidth_NumericEdit.SetValue( ov4.sourceRect.Width() );
   GUI->Overscan4SourceWidth_NumericEdit.Enable( en4 );
   GUI->Overscan4SourceHeight_NumericEdit.SetValue( ov4.sourceRect.Height() );
   GUI->Overscan4SourceHeight_NumericEdit.Enable( en4 );

   GUI->Overscan4Target_Label.Enable( en4 );
   GUI->Overscan4TargetX0_NumericEdit.SetValue( ov4.targetRect.x0 );
   GUI->Overscan4TargetX0_NumericEdit.Enable( en4 );
   GUI->Overscan4TargetY0_NumericEdit.SetValue( ov4.targetRect.y0 );
   GUI->Overscan4TargetY0_NumericEdit.Enable( en4 );
   GUI->Overscan4TargetWidth_NumericEdit.SetValue( ov4.targetRect.Width() );
   GUI->Overscan4TargetWidth_NumericEdit.Enable( en4 );
   GUI->Overscan4TargetHeight_NumericEdit.SetValue( ov4.targetRect.Height() );
   GUI->Overscan4TargetHeight_NumericEdit.Enable( en4 );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__TargetImages_CurrentNodeUpdated( TreeBox& sender,
                                                                   TreeBox::Node& current,
                                                                   TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "ImageCalibrationInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "ImageCalibrationInterface: *Warning* Corrupted interface structures" );

   ImageCalibrationInstance::ImageItem& item = instance.targetFrames[index];

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
         Array<ImageWindow> windows = ImageWindow::Open( item.path, IsoString()/*id*/, instance.inputHints );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__TargetImages_NodeSelectionUpdated( TreeBox& sender )
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

void ImageCalibrationInterface::__TargetImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Target Frames" );

      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), ImageCalibrationInstance::ImageItem( *i ) );
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
      ImageCalibrationInstance::image_list newTargets;
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

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__FormatHints_EditCompleted( Edit& sender )
{
   String hints = sender.Text().Trimmed();

   if ( sender == GUI->InputHints_Edit )
      instance.inputHints = hints;
   else if ( sender == GUI->OutputHints_Edit )
      instance.outputHints = hints;

   sender.SetText( hints );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__OutputFiles_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == GUI->OutputDirectory_Edit )
      instance.outputDirectory = text;
   else if ( sender == GUI->OutputPrefix_Edit )
      instance.outputPrefix = text;
   else if ( sender == GUI->OutputPostfix_Edit )
      instance.outputPostfix = text;

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__OutputFiles_Click( Button& sender, bool checked )
{
   if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "ImageCalibration: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( instance.outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->EvaluateNoise_CheckBox )
   {
      instance.evaluateNoise = checked;
      UpdateOutputFilesControls();
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
      instance.overwriteExistingFiles = checked;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__OutputFiles_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->OutputSampleFormat_ComboBox )
      instance.outputSampleFormat = itemIndex;
   else if ( sender == GUI->NoiseEvaluation_ComboBox )
      instance.noiseEvaluationAlgorithm = itemIndex;
   else if ( sender == GUI->OnError_ComboBox )
      instance.onError = itemIndex;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__OutputFiles_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->OutputPedestal_SpinBox )
      instance.outputPedestal = value;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__Pedestal_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->PedestalValue_SpinBox )
      instance.pedestal = value;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__Pedestal_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->PedestalMode_ComboBox )
   {
      instance.pedestalMode = itemIndex;
      UpdatePedestalControls();
   }
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__Pedestal_EditCompleted( Edit& sender )
{
   if ( sender == GUI->PedestalKeyword_Edit )
      sender.SetText( instance.pedestalKeyword = sender.Text().Trimmed() );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__MasterFrame_EditCompleted( Edit& sender )
{
   String path = sender.Text().Trimmed();
   /*
   if ( !path.IsEmpty() )
   {
      // ### Validate file path ?
   }
   */

   if ( sender == GUI->MasterBiasPath_Edit )
      instance.masterBias.path = path;
   else if ( sender == GUI->MasterDarkPath_Edit )
      instance.masterDark.path = path;
   else if ( sender == GUI->MasterFlatPath_Edit )
      instance.masterFlat.path = path;

   sender.SetText( path );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__MasterFrame_Click( Button& sender, bool checked )
{
   if ( sender == GUI->MasterBiasPath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Master Bias Frame" );
      if ( d.Execute() )
         instance.masterBias.path = d.FileName();
   }
   else if ( sender == GUI->CalibrateMasterBias_CheckBox )
      instance.calibrateBias = checked;
   else if ( sender == GUI->MasterDarkPath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Master Dark Frame" );
      if ( d.Execute() )
         instance.masterDark.path = d.FileName();
   }
   else if ( sender == GUI->CalibrateMasterDark_CheckBox )
      instance.calibrateDark = checked;
   else if ( sender == GUI->OptimizeDarks_CheckBox )
      instance.optimizeDarks = checked;
   else if ( sender == GUI->MasterFlatPath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "ImageCalibration: Select Master Flat Frame" );
      if ( d.Execute() )
         instance.masterFlat.path = d.FileName();
   }
   else if ( sender == GUI->CalibrateMasterFlat_CheckBox )
      instance.calibrateFlat = checked;

   UpdateMasterFrameControls();
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__MasterFrame_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->DarkOptimizationWindow_SpinBox )
      instance.darkOptimizationWindow = value;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__MasterFrame_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->DarkCFADetectionMode_ComboBox )
      instance.darkCFADetectionMode = itemIndex;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__MasterFrame_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->DarkOptimizationThreshold_NumericControl )
   {
      instance.darkOptimizationLow = value;
      instance.darkOptimizationThreshold = 0; // deprecated parameter, for compatibility with old versions.
   }
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__Overscan_ValueUpdated( NumericEdit& sender, double value_ )
{
#define SETX0( r, x0_ ) { int w = r.Width(); r.x1 = (r.x0 = x0_) + w; }
#define SETY0( r, y0_ ) { int h = r.Height(); r.y1 = (r.y0 = y0_) + h; }

   int value = TruncI( value_ );

   if ( sender == GUI->ImageRegionX0_NumericEdit )
      SETX0( instance.overscan.imageRect, value )
   else if ( sender == GUI->ImageRegionY0_NumericEdit )
      SETY0( instance.overscan.imageRect, value )
   else if ( sender == GUI->ImageRegionWidth_NumericEdit )
      instance.overscan.imageRect.SetWidth( value );
   else if ( sender == GUI->ImageRegionHeight_NumericEdit )
      instance.overscan.imageRect.SetHeight( value );

   else if ( sender == GUI->Overscan1SourceX0_NumericEdit )
      SETX0( instance.overscan.overscan[0].sourceRect, value )
   else if ( sender == GUI->Overscan1SourceY0_NumericEdit )
      SETY0( instance.overscan.overscan[0].sourceRect, value )
   else if ( sender == GUI->Overscan1SourceWidth_NumericEdit )
      instance.overscan.overscan[0].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan1SourceHeight_NumericEdit )
      instance.overscan.overscan[0].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan1TargetX0_NumericEdit )
      SETX0( instance.overscan.overscan[0].targetRect, value )
   else if ( sender == GUI->Overscan1TargetY0_NumericEdit )
      SETY0( instance.overscan.overscan[0].targetRect, value )
   else if ( sender == GUI->Overscan1TargetWidth_NumericEdit )
      instance.overscan.overscan[0].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan1TargetHeight_NumericEdit )
      instance.overscan.overscan[0].targetRect.SetHeight( value );

   else if ( sender == GUI->Overscan2SourceX0_NumericEdit )
      SETX0( instance.overscan.overscan[1].sourceRect, value )
   else if ( sender == GUI->Overscan2SourceY0_NumericEdit )
      SETY0( instance.overscan.overscan[1].sourceRect, value )
   else if ( sender == GUI->Overscan2SourceWidth_NumericEdit )
      instance.overscan.overscan[1].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan2SourceHeight_NumericEdit )
      instance.overscan.overscan[1].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan2TargetX0_NumericEdit )
      SETX0( instance.overscan.overscan[1].targetRect, value )
   else if ( sender == GUI->Overscan2TargetY0_NumericEdit )
      SETY0( instance.overscan.overscan[1].targetRect, value )
   else if ( sender == GUI->Overscan2TargetWidth_NumericEdit )
      instance.overscan.overscan[1].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan2TargetHeight_NumericEdit )
      instance.overscan.overscan[1].targetRect.SetHeight( value );

   else if ( sender == GUI->Overscan3SourceX0_NumericEdit )
      SETX0( instance.overscan.overscan[2].sourceRect, value )
   else if ( sender == GUI->Overscan3SourceY0_NumericEdit )
      SETY0( instance.overscan.overscan[2].sourceRect, value )
   else if ( sender == GUI->Overscan3SourceWidth_NumericEdit )
      instance.overscan.overscan[2].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan3SourceHeight_NumericEdit )
      instance.overscan.overscan[2].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan3TargetX0_NumericEdit )
      SETX0( instance.overscan.overscan[2].targetRect, value )
   else if ( sender == GUI->Overscan3TargetY0_NumericEdit )
      SETY0( instance.overscan.overscan[2].targetRect, value )
   else if ( sender == GUI->Overscan3TargetWidth_NumericEdit )
      instance.overscan.overscan[2].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan3TargetHeight_NumericEdit )
      instance.overscan.overscan[2].targetRect.SetHeight( value );

   else if ( sender == GUI->Overscan4SourceX0_NumericEdit )
      SETX0( instance.overscan.overscan[3].sourceRect, value )
   else if ( sender == GUI->Overscan4SourceY0_NumericEdit )
      SETY0( instance.overscan.overscan[3].sourceRect, value )
   else if ( sender == GUI->Overscan4SourceWidth_NumericEdit )
      instance.overscan.overscan[3].sourceRect.SetWidth( value );
   else if ( sender == GUI->Overscan4SourceHeight_NumericEdit )
      instance.overscan.overscan[3].sourceRect.SetHeight( value );

   else if ( sender == GUI->Overscan4TargetX0_NumericEdit )
      SETX0( instance.overscan.overscan[3].targetRect, value )
   else if ( sender == GUI->Overscan4TargetY0_NumericEdit )
      SETY0( instance.overscan.overscan[3].targetRect, value )
   else if ( sender == GUI->Overscan4TargetWidth_NumericEdit )
      instance.overscan.overscan[3].targetRect.SetWidth( value );
   else if ( sender == GUI->Overscan4TargetHeight_NumericEdit )
      instance.overscan.overscan[3].targetRect.SetHeight( value );

#undef SETX0
#undef SETY0
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__Overscan_Click( Button& sender, bool checked )
{
   if ( sender == GUI->Overscan1_CheckBox )
      instance.overscan.overscan[0].enabled = checked;
   else if ( sender == GUI->Overscan2_CheckBox )
      instance.overscan.overscan[1].enabled = checked;
   else if ( sender == GUI->Overscan3_CheckBox )
      instance.overscan.overscan[2].enabled = checked;
   else if ( sender == GUI->Overscan4_CheckBox )
      instance.overscan.overscan[3].enabled = checked;

   UpdateOverscanControls();
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__CheckSection( SectionBar& sender, bool checked )
{
   if ( sender == GUI->MasterBias_SectionBar )
      instance.masterBias.enabled = checked;
   else if ( sender == GUI->MasterDark_SectionBar )
      instance.masterDark.enabled = checked;
   else if ( sender == GUI->MasterFlat_SectionBar )
      instance.masterFlat.enabled = checked;
   else if ( sender == GUI->Overscan_SectionBar )
      instance.overscan.enabled = checked;
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
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

void ImageCalibrationInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
   else if ( sender == GUI->MasterBiasPath_Edit || sender == GUI->MasterDarkPath_Edit || sender == GUI->MasterFlatPath_Edit )
      wantsFiles = files.Length() == 1 && File::Exists( files[0] );
}

// ----------------------------------------------------------------------------

void ImageCalibrationInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
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
         instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), ImageCalibrationInstance::ImageItem( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( instance.outputDirectory = files[0] );
   }
   else if ( sender == GUI->MasterBiasPath_Edit )
   {
      if ( File::Exists( files[0] ) )
         GUI->MasterBiasPath_Edit.SetText( instance.masterBias.path = files[0] );
   }
   else if ( sender == GUI->MasterDarkPath_Edit )
   {
      if ( File::Exists( files[0] ) )
         GUI->MasterDarkPath_Edit.SetText( instance.masterDark.path = files[0] );
   }
   else if ( sender == GUI->MasterFlatPath_Edit )
   {
      if ( File::Exists( files[0] ) )
         GUI->MasterFlatPath_Edit.SetText( instance.masterFlat.path = files[0] );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageCalibrationInterface::GUIData::GUIData( ImageCalibrationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Optimization window (px):" ) + 'M' );
   int editWidth1 = fnt.Width( String( 'M', 5  ) );
   int editWidth2 = fnt.Width( String( '0', 10  ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&ImageCalibrationInterface::__TargetImages_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&ImageCalibrationInterface::__TargetImages_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&ImageCalibrationInterface::__TargetImages_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::__FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::__FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target frames.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__TargetImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target frames.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__TargetImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target frames.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__TargetImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target frames.</p>"
      "<p>Disabled target frames will be ignored during the calibration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__TargetImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target frames.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__TargetImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of target frames.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__TargetImages_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target frame files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__TargetImages_Click, w );

   TargetButtons_Sizer.SetSpacing( 4 );
   TargetButtons_Sizer.Add( AddFiles_PushButton );
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

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings "
      "for image files used by specific processes. In ImageCalibration, input hints change the way input images "
      "of some particular file formats (target frames and master calibration frames) are read, while output hints "
      "modify the way output calibrated images are written.</p>"
      "<p>For example, you can use the \"raw\" hint to force the DSLR_RAW format to load pure raw images without "
      "applying any deBayering, interpolation, white balance or black point correction. Similarly, you can use the "
      "\"up-bottom\" output hint to write all calibrated images with the coordinate origin at the top-left corner. "
      "Most standard file format modules support hints; each format supports a number of input and/or output hints "
      "that you can use for different purposes with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__FormatHints_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__FormatHints_EditCompleted, w );

   OutputHints_Sizer.SetSpacing( 4 );
   OutputHints_Sizer.Add( OutputHints_Label );
   OutputHints_Sizer.Add( OutputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );
   FormatHints_Sizer.Add( OutputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );
   FormatHints_Control.AdjustToContents();

   //

   OutputFiles_SectionBar.SetTitle( "Output Files" );
   OutputFiles_SectionBar.SetSection( OutputFiles_Control );
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );

   const char* outputDirectoryToolTip = "<p>This is the directory (or folder) where all output files "
      "will be written.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their "
      "corresponding target files. In this case, make sure that source directories are writable, or the "
      "calibration process will fail.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__OutputFiles_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::__FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::__FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__OutputFiles_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   const char* outputPrefixToolTip =
      "<p>This is a prefix that will be prepended to all output file names.</p>";

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( labelWidth1 );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPrefix_Label.SetToolTip( outputPrefixToolTip );

   OutputPrefix_Edit.SetFixedWidth( editWidth1 );
   OutputPrefix_Edit.SetToolTip( outputPrefixToolTip );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__OutputFiles_EditCompleted, w );

   const char* outputPostfixToolTip =
      "<p>This is a postfix that will be appended to the file name of each registered image.</p>";

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPostfix_Label.SetToolTip( outputPostfixToolTip );

   OutputPostfix_Edit.SetFixedWidth( editWidth1 );
   OutputPostfix_Edit.SetToolTip( outputPostfixToolTip );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__OutputFiles_EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   const char* sampleFormatToolTip = "<p>Sample data format for newly created calibrated images.</p>";

   OutputSampleFormat_Label.SetText( "Sample format:" );
   OutputSampleFormat_Label.SetToolTip( sampleFormatToolTip );
   OutputSampleFormat_Label.SetMinWidth( labelWidth1 );
   OutputSampleFormat_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputSampleFormat_ComboBox.AddItem( "16-bit integer" );
   OutputSampleFormat_ComboBox.AddItem( "32-bit integer" );
   OutputSampleFormat_ComboBox.AddItem( "32-bit floating point" );
   OutputSampleFormat_ComboBox.AddItem( "64-bit floating point" );
   OutputSampleFormat_ComboBox.SetToolTip( sampleFormatToolTip );
   OutputSampleFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::__OutputFiles_ItemSelected, w );

   OutputSampleFormat_Sizer.SetSpacing( 4 );
   OutputSampleFormat_Sizer.Add( OutputSampleFormat_Label );
   OutputSampleFormat_Sizer.Add( OutputSampleFormat_ComboBox );
   OutputSampleFormat_Sizer.AddStretch();

   const char* outputPedestalTip = "<p>The <i>output pedestal</i> is a small quantity expressed in the 16-bit "
      "unsigned integer range (from 0 to 65535). It is added at the end of the calibration process and its "
      "purpose is to prevent negative values that can occur sometimes as a result of overscan and bias subtraction. "
      "Under normal conditions, you should need no pedestal to calibrate science frames because mean sky background "
      "levels should be sufficiently high as to avoid negative values. A small pedestal can be necessary sometimes "
      "to calibrate individual bias and dark frames. The default value is zero.</p>";

   OutputPedestal_Label.SetText( "Output pedestal (DN):" );
   OutputPedestal_Label.SetFixedWidth( labelWidth1 );
   OutputPedestal_Label.SetToolTip( outputPedestalTip );
   OutputPedestal_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPedestal_SpinBox.SetRange( int( TheICOutputPedestalParameter->MinimumValue() ), int( TheICOutputPedestalParameter->MaximumValue() ) );
   OutputPedestal_SpinBox.SetToolTip( outputPedestalTip );
   OutputPedestal_SpinBox.SetFixedWidth( editWidth2 );
   OutputPedestal_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::__OutputFiles_SpinValueUpdated, w );

   OutputPedestal_Sizer.Add( OutputPedestal_Label );
   OutputPedestal_Sizer.AddSpacing( 4 );
   OutputPedestal_Sizer.Add( OutputPedestal_SpinBox );
   OutputPedestal_Sizer.AddStretch();

   EvaluateNoise_CheckBox.SetText( "Evaluate noise" );
   EvaluateNoise_CheckBox.SetToolTip( "<p>If this option is selected, ImageCalibration will compute per-channel "
      "noise estimates for each target image using a wavelet-based algorithm (MRS noise evaluation by default). Noise "
      "estimates will be stored as NOISExxx FITS header keywords in the output files. These estimates can be used "
      "later by several processes and scripts, most notably by the ImageIntegration tool, which uses them by default "
      "for robust image weighting based on relative SNR values.</p>" );
   EvaluateNoise_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__OutputFiles_Click, w );

   EvaluateNoise_Sizer.SetSpacing( 4 );
   EvaluateNoise_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   EvaluateNoise_Sizer.Add( EvaluateNoise_CheckBox );
   EvaluateNoise_Sizer.AddStretch();

   const char* noiseEvaluationToolTip = "<p>Noise evaluation algorithm. This option selects an algorithm for automatic "
      "estimation of the standard deviation of the noise in the calibrated images. In all cases noise estimates assume "
      "a Gaussian distribution of the noise.</p>"
      "<p>The iterative k-sigma clipping algorithm can be used as a last-resort option in cases where the MRS algorithm "
      "(see below) does not converge systematically. This can happen for images without detectable small-scale noise; "
      "for example, images that have been smoothed as a result of bilinear de-Bayer interpolation.</p>"
      "<p>The multiresolution support (MRS) noise estimation routine implements the iterative algorithm described by "
      "Jean-Luc Starck and Fionn Murtagh in their paper <em>Automatic Noise Estimation from the Multiresolution Support</em> "
      "(Publications of the Royal Astronomical Society of the Pacific, vol. 110, pp. 193-199). In our implementation, the "
      "standard deviation of the noise is evaluated on the first four wavelet layers. This is the most accurate algorithm "
      "available, and hence the default option.</p>";

   NoiseEvaluation_Label.SetText( "Noise evaluation:" );
   NoiseEvaluation_Label.SetFixedWidth( labelWidth1 );
   NoiseEvaluation_Label.SetToolTip( noiseEvaluationToolTip );
   NoiseEvaluation_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   NoiseEvaluation_ComboBox.AddItem( "Iterative K-Sigma Clipping" );
   NoiseEvaluation_ComboBox.AddItem( "Multiresolution Support" );
   NoiseEvaluation_ComboBox.SetToolTip( noiseEvaluationToolTip );
   NoiseEvaluation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::__OutputFiles_ItemSelected, w );

   NoiseEvaluation_Sizer.SetSpacing( 4 );
   NoiseEvaluation_Sizer.Add( NoiseEvaluation_Label );
   NoiseEvaluation_Sizer.Add( NoiseEvaluation_ComboBox );
   NoiseEvaluation_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( "<p>If this option is selected, ImageCalibration will overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__OutputFiles_Click, w );

   OverwriteExistingFiles_Sizer.SetSpacing( 4 );
   OverwriteExistingFiles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OverwriteExistingFiles_Sizer.Add( OverwriteExistingFiles_CheckBox );
   OverwriteExistingFiles_Sizer.AddStretch();

   const char* onErrorToolTip = "<p>Specify what to do if there are errors during the batch calibration process.</p>";

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetFixedWidth( labelWidth1 );
   OnError_Label.SetToolTip( onErrorToolTip );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OnError_ComboBox.AddItem( "Continue" );
   OnError_ComboBox.AddItem( "Abort" );
   OnError_ComboBox.AddItem( "Ask User" );
   OnError_ComboBox.SetToolTip( onErrorToolTip );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::__OutputFiles_ItemSelected, w );

   OnError_Sizer.SetSpacing( 4 );
   OnError_Sizer.Add( OnError_Label );
   OnError_Sizer.Add( OnError_ComboBox );
   OnError_Sizer.AddStretch();

   OutputFiles_Sizer.SetSpacing( 4 );
   OutputFiles_Sizer.Add( OutputDirectory_Sizer );
   OutputFiles_Sizer.Add( OutputChunks_Sizer );
   OutputFiles_Sizer.Add( OutputSampleFormat_Sizer );
   OutputFiles_Sizer.Add( OutputPedestal_Sizer );
   OutputFiles_Sizer.Add( EvaluateNoise_Sizer );
   OutputFiles_Sizer.Add( NoiseEvaluation_Sizer );
   OutputFiles_Sizer.Add( OverwriteExistingFiles_Sizer );
   OutputFiles_Sizer.Add( OnError_Sizer );

   OutputFiles_Control.SetSizer( OutputFiles_Sizer );
   OutputFiles_Control.AdjustToContents();

   //

   Pedestal_SectionBar.SetTitle( "Pedestal" );
   Pedestal_SectionBar.SetSection( Pedestal_Control );
   Pedestal_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );

   const char* pedestalModeTip = "<p>The <i>pedestal mode</i> option specifies how to retrieve a (usually small) "
      "quantity that is subtracted from input images as the very first calibration step. This quantity is known "
      "as <i>pedestal</i>, and must be expressed in the 16-bit unsigned integer range (from 0 to 65535). If present, "
      "a pedestal has been added by a previous calibration process to ensure positivity of the result. For example, "
      "you might have specified an <i>output pedestal</i> (see the corresponding parameter above) to enforce "
      "positivity of previously calibrated master bias, dark and flat frames.</p>"
      "<p><b>Literal value</b> lets you specify a pedestal value that will be subtracted from all target and "
      "master calibration frames. You usually don't need this option, except in very special cases.</p>"
      "<p><b>Default FITS keyword</b> is the default mode. When this mode is selected, ImageCalibration will "
      "retrieve pedestal values from 'PEDESTAL' FITS keywords, when they are present in the input images. Note "
      "that ImageCalibration writes PEDESTAL keywords to calibrated images when a nonzero <i>output pedestal</i> "
      "is specified.</p>"
      "<p><b>Custom FITS keyword</b> allows you to specify the name of a custom FITS keyword to retrieve "
      "pedestal values, when the specified keyword is present in the input images.</p>";

   PedestalMode_Label.SetText( "Pedestal mode:" );
   PedestalMode_Label.SetToolTip( pedestalModeTip );
   PedestalMode_Label.SetMinWidth( labelWidth1 );
   PedestalMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PedestalMode_ComboBox.AddItem( "Literal value" );
   PedestalMode_ComboBox.AddItem( "Default FITS keyword (PEDESTAL)" );
   PedestalMode_ComboBox.AddItem( "Custom FITS keyword" );
   PedestalMode_ComboBox.SetToolTip( pedestalModeTip );
   PedestalMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::__Pedestal_ItemSelected, w );

   PedestalMode_Sizer.SetSpacing( 4 );
   PedestalMode_Sizer.Add( PedestalMode_Label );
   PedestalMode_Sizer.Add( PedestalMode_ComboBox );
   PedestalMode_Sizer.AddStretch();

   const char* pedestalValueTip = "<p>Literal pedestal value in the 16-bit unsigned integer range (from 0 to 65535), "
      "when the <i>Literal value</i> option has been selected as <i>pedestal mode</i>.</p>";

   PedestalValue_Label.SetText( "Pedestal value (DN):" );
   PedestalValue_Label.SetFixedWidth( labelWidth1 );
   PedestalValue_Label.SetToolTip( pedestalValueTip );
   PedestalValue_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PedestalValue_SpinBox.SetRange( int( TheICPedestalParameter->MinimumValue() ), int( TheICPedestalParameter->MaximumValue() ) );
   PedestalValue_SpinBox.SetToolTip( pedestalValueTip );
   PedestalValue_SpinBox.SetFixedWidth( editWidth2 );
   PedestalValue_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::__Pedestal_SpinValueUpdated, w );

   PedestalValue_Sizer.Add( PedestalValue_Label );
   PedestalValue_Sizer.AddSpacing( 4 );
   PedestalValue_Sizer.Add( PedestalValue_SpinBox );
   PedestalValue_Sizer.AddStretch();

   const char* pedestalKeywordTip = "<p>Custom FITS keyword to retrieve pedestal values in 16-bit DN.</p>"
      "<p>This is the name of a FITS keyword that will be used to retrieve input pedestals, if the "
      "<i>Custom FITS keyword</i> option has been selected as <i>pedestal mode</i>.</p>";

   PedestalKeyword_Label.SetText( "Pedestal keyword:" );
   PedestalKeyword_Label.SetFixedWidth( labelWidth1 );
   PedestalKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PedestalKeyword_Label.SetToolTip( pedestalKeywordTip );

   PedestalKeyword_Edit.SetMinWidth( editWidth2 );
   PedestalKeyword_Edit.SetToolTip( pedestalKeywordTip );
   PedestalKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__Pedestal_EditCompleted, w );

   PedestalKeyword_Sizer.SetSpacing( 4 );
   PedestalKeyword_Sizer.Add( PedestalKeyword_Label );
   PedestalKeyword_Sizer.Add( PedestalKeyword_Edit );
   PedestalKeyword_Sizer.AddStretch();

   Pedestal_Sizer.SetSpacing( 4 );
   Pedestal_Sizer.Add( PedestalMode_Sizer );
   Pedestal_Sizer.Add( PedestalValue_Sizer );
   Pedestal_Sizer.Add( PedestalKeyword_Sizer );

   Pedestal_Control.SetSizer( Pedestal_Sizer );
   Pedestal_Control.AdjustToContents();

   //

   Overscan_SectionBar.SetTitle( "Overscan" );
   Overscan_SectionBar.SetSection( Overscan_Control );
   Overscan_SectionBar.EnableTitleCheckBox();
   Overscan_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );
   Overscan_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::__CheckSection, w );

   //

   ImageRegion_Label.SetText( "Image region:" );
   ImageRegion_Label.SetFixedWidth( labelWidth1 );
   ImageRegion_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ImageRegion_Label.SetToolTip( "<p>The image region defines a rectangular area of the CCD frame that "
      "will be cropped after overscan correction. It usually corresponds to the true science image "
      "without any overscan regions.</p>" );

   ImageRegionX0_NumericEdit.SetInteger();
   ImageRegionX0_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionX0_NumericEdit.label.Hide();
   ImageRegionX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionX0_NumericEdit.SetToolTip( "Image region, left pixel coordinate" );
   ImageRegionX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   ImageRegionY0_NumericEdit.SetInteger();
   ImageRegionY0_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionY0_NumericEdit.label.Hide();
   ImageRegionY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionY0_NumericEdit.SetToolTip( "Image region, top pixel coordinate" );
   ImageRegionY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   ImageRegionWidth_NumericEdit.SetInteger();
   ImageRegionWidth_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionWidth_NumericEdit.label.Hide();
   ImageRegionWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionWidth_NumericEdit.SetToolTip( "Image region, width in pixels" );
   ImageRegionWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   ImageRegionHeight_NumericEdit.SetInteger();
   ImageRegionHeight_NumericEdit.SetRange( 0, uint16_max );
   ImageRegionHeight_NumericEdit.label.Hide();
   ImageRegionHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ImageRegionHeight_NumericEdit.SetToolTip( "Image region, height in pixels" );
   ImageRegionHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   ImageRegion_Sizer.SetSpacing( 4 );
   ImageRegion_Sizer.Add( ImageRegion_Label );
   ImageRegion_Sizer.Add( ImageRegionX0_NumericEdit );
   ImageRegion_Sizer.Add( ImageRegionY0_NumericEdit );
   ImageRegion_Sizer.Add( ImageRegionWidth_NumericEdit );
   ImageRegion_Sizer.Add( ImageRegionHeight_NumericEdit );
   ImageRegion_Sizer.AddStretch();

   //

   Overscan1_CheckBox.SetText( "Overscan #1" );
   Overscan1_CheckBox.SetToolTip( "<p>Enable the first overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan1_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__Overscan_Click, w );

   Overscan1_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan1_Sizer.Add( Overscan1_CheckBox );
   Overscan1_Sizer.AddStretch();

   Overscan1Source_Label.SetText( "Source region:" );
   Overscan1Source_Label.SetFixedWidth( labelWidth1 );
   Overscan1Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan1Source_Label.SetToolTip( "<p>Overscan source region #1</p>" );

   Overscan1SourceX0_NumericEdit.SetInteger();
   Overscan1SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceX0_NumericEdit.label.Hide();
   Overscan1SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceX0_NumericEdit.SetToolTip( "Overscan #1, source region, left pixel coordinate" );
   Overscan1SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1SourceY0_NumericEdit.SetInteger();
   Overscan1SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceY0_NumericEdit.label.Hide();
   Overscan1SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceY0_NumericEdit.SetToolTip( "Overscan #1, source region, top pixel coordinate" );
   Overscan1SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1SourceWidth_NumericEdit.SetInteger();
   Overscan1SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceWidth_NumericEdit.label.Hide();
   Overscan1SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceWidth_NumericEdit.SetToolTip( "Overscan #1, source region, width in pixels" );
   Overscan1SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1SourceHeight_NumericEdit.SetInteger();
   Overscan1SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan1SourceHeight_NumericEdit.label.Hide();
   Overscan1SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1SourceHeight_NumericEdit.SetToolTip( "Overscan #1, source region, height in pixels" );
   Overscan1SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1Source_Sizer.SetSpacing( 4 );
   Overscan1Source_Sizer.Add( Overscan1Source_Label );
   Overscan1Source_Sizer.Add( Overscan1SourceX0_NumericEdit );
   Overscan1Source_Sizer.Add( Overscan1SourceY0_NumericEdit );
   Overscan1Source_Sizer.Add( Overscan1SourceWidth_NumericEdit );
   Overscan1Source_Sizer.Add( Overscan1SourceHeight_NumericEdit );
   Overscan1Source_Sizer.AddStretch();

   //

   Overscan1Target_Label.SetText( "Target region:" );
   Overscan1Target_Label.SetFixedWidth( labelWidth1 );
   Overscan1Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan1Target_Label.SetToolTip( "<p>Overscan target region #1</p>" );

   Overscan1TargetX0_NumericEdit.SetInteger();
   Overscan1TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetX0_NumericEdit.label.Hide();
   Overscan1TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetX0_NumericEdit.SetToolTip( "Overscan #1, target region, left pixel coordinate" );
   Overscan1TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1TargetY0_NumericEdit.SetInteger();
   Overscan1TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetY0_NumericEdit.label.Hide();
   Overscan1TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetY0_NumericEdit.SetToolTip( "Overscan #1, target region, top pixel coordinate" );
   Overscan1TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1TargetWidth_NumericEdit.SetInteger();
   Overscan1TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetWidth_NumericEdit.label.Hide();
   Overscan1TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetWidth_NumericEdit.SetToolTip( "Overscan #1, target region, width in pixels" );
   Overscan1TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1TargetHeight_NumericEdit.SetInteger();
   Overscan1TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan1TargetHeight_NumericEdit.label.Hide();
   Overscan1TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan1TargetHeight_NumericEdit.SetToolTip( "Overscan #1, target region, height in pixels" );
   Overscan1TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan1Target_Sizer.SetSpacing( 4 );
   Overscan1Target_Sizer.Add( Overscan1Target_Label );
   Overscan1Target_Sizer.Add( Overscan1TargetX0_NumericEdit );
   Overscan1Target_Sizer.Add( Overscan1TargetY0_NumericEdit );
   Overscan1Target_Sizer.Add( Overscan1TargetWidth_NumericEdit );
   Overscan1Target_Sizer.Add( Overscan1TargetHeight_NumericEdit );
   Overscan1Target_Sizer.AddStretch();

   //

   Overscan2_CheckBox.SetText( "Overscan #2" );
   Overscan2_CheckBox.SetToolTip( "<p>Enable the second overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan2_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__Overscan_Click, w );

   Overscan2_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan2_Sizer.Add( Overscan2_CheckBox );
   Overscan2_Sizer.AddStretch();

   Overscan2Source_Label.SetText( "Source region:" );
   Overscan2Source_Label.SetFixedWidth( labelWidth1 );
   Overscan2Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan2Source_Label.SetToolTip( "<p>Overscan source region #2</p>" );

   Overscan2SourceX0_NumericEdit.SetInteger();
   Overscan2SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceX0_NumericEdit.label.Hide();
   Overscan2SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceX0_NumericEdit.SetToolTip( "Overscan #2, source region, left pixel coordinate" );
   Overscan2SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2SourceY0_NumericEdit.SetInteger();
   Overscan2SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceY0_NumericEdit.label.Hide();
   Overscan2SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceY0_NumericEdit.SetToolTip( "Overscan #2, source region, top pixel coordinate" );
   Overscan2SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2SourceWidth_NumericEdit.SetInteger();
   Overscan2SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceWidth_NumericEdit.label.Hide();
   Overscan2SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceWidth_NumericEdit.SetToolTip( "Overscan #2, source region, width in pixels" );
   Overscan2SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2SourceHeight_NumericEdit.SetInteger();
   Overscan2SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan2SourceHeight_NumericEdit.label.Hide();
   Overscan2SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2SourceHeight_NumericEdit.SetToolTip( "Overscan #2, source region, height in pixels" );
   Overscan2SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2Source_Sizer.SetSpacing( 4 );
   Overscan2Source_Sizer.Add( Overscan2Source_Label );
   Overscan2Source_Sizer.Add( Overscan2SourceX0_NumericEdit );
   Overscan2Source_Sizer.Add( Overscan2SourceY0_NumericEdit );
   Overscan2Source_Sizer.Add( Overscan2SourceWidth_NumericEdit );
   Overscan2Source_Sizer.Add( Overscan2SourceHeight_NumericEdit );
   Overscan2Source_Sizer.AddStretch();

   //

   Overscan2Target_Label.SetText( "Target region:" );
   Overscan2Target_Label.SetFixedWidth( labelWidth1 );
   Overscan2Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan2Target_Label.SetToolTip( "<p>Overscan target region #2</p>" );

   Overscan2TargetX0_NumericEdit.SetInteger();
   Overscan2TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetX0_NumericEdit.label.Hide();
   Overscan2TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetX0_NumericEdit.SetToolTip( "Overscan #2, target region, left pixel coordinate" );
   Overscan2TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2TargetY0_NumericEdit.SetInteger();
   Overscan2TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetY0_NumericEdit.label.Hide();
   Overscan2TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetY0_NumericEdit.SetToolTip( "Overscan #2, target region, top pixel coordinate" );
   Overscan2TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2TargetWidth_NumericEdit.SetInteger();
   Overscan2TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetWidth_NumericEdit.label.Hide();
   Overscan2TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetWidth_NumericEdit.SetToolTip( "Overscan #2, target region, width in pixels" );
   Overscan2TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2TargetHeight_NumericEdit.SetInteger();
   Overscan2TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan2TargetHeight_NumericEdit.label.Hide();
   Overscan2TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan2TargetHeight_NumericEdit.SetToolTip( "Overscan #2, target region, height in pixels" );
   Overscan2TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan2Target_Sizer.SetSpacing( 4 );
   Overscan2Target_Sizer.Add( Overscan2Target_Label );
   Overscan2Target_Sizer.Add( Overscan2TargetX0_NumericEdit );
   Overscan2Target_Sizer.Add( Overscan2TargetY0_NumericEdit );
   Overscan2Target_Sizer.Add( Overscan2TargetWidth_NumericEdit );
   Overscan2Target_Sizer.Add( Overscan2TargetHeight_NumericEdit );
   Overscan2Target_Sizer.AddStretch();

   //

   Overscan3_CheckBox.SetText( "Overscan #3" );
   Overscan3_CheckBox.SetToolTip( "<p>Enable the third overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan3_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__Overscan_Click, w );

   Overscan3_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan3_Sizer.Add( Overscan3_CheckBox );
   Overscan3_Sizer.AddStretch();

   Overscan3Source_Label.SetText( "Source region:" );
   Overscan3Source_Label.SetFixedWidth( labelWidth1 );
   Overscan3Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan3Source_Label.SetToolTip( "<p>Overscan source region #3</p>" );

   Overscan3SourceX0_NumericEdit.SetInteger();
   Overscan3SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceX0_NumericEdit.label.Hide();
   Overscan3SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceX0_NumericEdit.SetToolTip( "Overscan #3, source region, left pixel coordinate" );
   Overscan3SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3SourceY0_NumericEdit.SetInteger();
   Overscan3SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceY0_NumericEdit.label.Hide();
   Overscan3SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceY0_NumericEdit.SetToolTip( "Overscan #3, source region, top pixel coordinate" );
   Overscan3SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3SourceWidth_NumericEdit.SetInteger();
   Overscan3SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceWidth_NumericEdit.label.Hide();
   Overscan3SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceWidth_NumericEdit.SetToolTip( "Overscan #3, source region, width in pixels" );
   Overscan3SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3SourceHeight_NumericEdit.SetInteger();
   Overscan3SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan3SourceHeight_NumericEdit.label.Hide();
   Overscan3SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3SourceHeight_NumericEdit.SetToolTip( "Overscan #3, source region, height in pixels" );
   Overscan3SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3Source_Sizer.SetSpacing( 4 );
   Overscan3Source_Sizer.Add( Overscan3Source_Label );
   Overscan3Source_Sizer.Add( Overscan3SourceX0_NumericEdit );
   Overscan3Source_Sizer.Add( Overscan3SourceY0_NumericEdit );
   Overscan3Source_Sizer.Add( Overscan3SourceWidth_NumericEdit );
   Overscan3Source_Sizer.Add( Overscan3SourceHeight_NumericEdit );
   Overscan3Source_Sizer.AddStretch();

   //

   Overscan3Target_Label.SetText( "Target region:" );
   Overscan3Target_Label.SetFixedWidth( labelWidth1 );
   Overscan3Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan3Target_Label.SetToolTip( "<p>Overscan target region #3</p>" );

   Overscan3TargetX0_NumericEdit.SetInteger();
   Overscan3TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetX0_NumericEdit.label.Hide();
   Overscan3TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetX0_NumericEdit.SetToolTip( "Overscan #3, target region, left pixel coordinate" );
   Overscan3TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3TargetY0_NumericEdit.SetInteger();
   Overscan3TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetY0_NumericEdit.label.Hide();
   Overscan3TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetY0_NumericEdit.SetToolTip( "Overscan #3, target region, top pixel coordinate" );
   Overscan3TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3TargetWidth_NumericEdit.SetInteger();
   Overscan3TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetWidth_NumericEdit.label.Hide();
   Overscan3TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetWidth_NumericEdit.SetToolTip( "Overscan #3, target region, width in pixels" );
   Overscan3TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3TargetHeight_NumericEdit.SetInteger();
   Overscan3TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan3TargetHeight_NumericEdit.label.Hide();
   Overscan3TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan3TargetHeight_NumericEdit.SetToolTip( "Overscan #3, target region, height in pixels" );
   Overscan3TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan3Target_Sizer.SetSpacing( 4 );
   Overscan3Target_Sizer.Add( Overscan3Target_Label );
   Overscan3Target_Sizer.Add( Overscan3TargetX0_NumericEdit );
   Overscan3Target_Sizer.Add( Overscan3TargetY0_NumericEdit );
   Overscan3Target_Sizer.Add( Overscan3TargetWidth_NumericEdit );
   Overscan3Target_Sizer.Add( Overscan3TargetHeight_NumericEdit );
   Overscan3Target_Sizer.AddStretch();

   //

   Overscan4_CheckBox.SetText( "Overscan #4" );
   Overscan4_CheckBox.SetToolTip( "<p>Enable the fourth overscan. Each overscan allows you to define "
      "arbitrary <i>source and target overscan regions</i> in CCD pixel coordinates. A source overscan "
      "region is used to compute an overscan correction value (as the median of all source region pixels) "
      "that will be subtracted from all pixels in the corresponding target region.</p>" );
   Overscan4_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__Overscan_Click, w );

   Overscan4_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Overscan4_Sizer.Add( Overscan4_CheckBox );
   Overscan4_Sizer.AddStretch();

   Overscan4Source_Label.SetText( "Source region:" );
   Overscan4Source_Label.SetFixedWidth( labelWidth1 );
   Overscan4Source_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan4Source_Label.SetToolTip( "<p>Overscan source region #4</p>" );

   Overscan4SourceX0_NumericEdit.SetInteger();
   Overscan4SourceX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceX0_NumericEdit.label.Hide();
   Overscan4SourceX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceX0_NumericEdit.SetToolTip( "Overscan #4, source region, left pixel coordinate" );
   Overscan4SourceX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4SourceY0_NumericEdit.SetInteger();
   Overscan4SourceY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceY0_NumericEdit.label.Hide();
   Overscan4SourceY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceY0_NumericEdit.SetToolTip( "Overscan #4, source region, top pixel coordinate" );
   Overscan4SourceY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4SourceWidth_NumericEdit.SetInteger();
   Overscan4SourceWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceWidth_NumericEdit.label.Hide();
   Overscan4SourceWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceWidth_NumericEdit.SetToolTip( "Overscan #4, source region, width in pixels" );
   Overscan4SourceWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4SourceHeight_NumericEdit.SetInteger();
   Overscan4SourceHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan4SourceHeight_NumericEdit.label.Hide();
   Overscan4SourceHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4SourceHeight_NumericEdit.SetToolTip( "Overscan #4, source region, height in pixels" );
   Overscan4SourceHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4Source_Sizer.SetSpacing( 4 );
   Overscan4Source_Sizer.Add( Overscan4Source_Label );
   Overscan4Source_Sizer.Add( Overscan4SourceX0_NumericEdit );
   Overscan4Source_Sizer.Add( Overscan4SourceY0_NumericEdit );
   Overscan4Source_Sizer.Add( Overscan4SourceWidth_NumericEdit );
   Overscan4Source_Sizer.Add( Overscan4SourceHeight_NumericEdit );
   Overscan4Source_Sizer.AddStretch();

   //

   Overscan4Target_Label.SetText( "Target region:" );
   Overscan4Target_Label.SetFixedWidth( labelWidth1 );
   Overscan4Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Overscan4Target_Label.SetToolTip( "<p>Overscan target region #4</p>" );

   Overscan4TargetX0_NumericEdit.SetInteger();
   Overscan4TargetX0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetX0_NumericEdit.label.Hide();
   Overscan4TargetX0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetX0_NumericEdit.SetToolTip( "Overscan #4, target region, left pixel coordinate" );
   Overscan4TargetX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4TargetY0_NumericEdit.SetInteger();
   Overscan4TargetY0_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetY0_NumericEdit.label.Hide();
   Overscan4TargetY0_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetY0_NumericEdit.SetToolTip( "Overscan #4, target region, top pixel coordinate" );
   Overscan4TargetY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4TargetWidth_NumericEdit.SetInteger();
   Overscan4TargetWidth_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetWidth_NumericEdit.label.Hide();
   Overscan4TargetWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetWidth_NumericEdit.SetToolTip( "Overscan #4, target region, width in pixels" );
   Overscan4TargetWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4TargetHeight_NumericEdit.SetInteger();
   Overscan4TargetHeight_NumericEdit.SetRange( 0, uint16_max );
   Overscan4TargetHeight_NumericEdit.label.Hide();
   Overscan4TargetHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Overscan4TargetHeight_NumericEdit.SetToolTip( "Overscan #4, target region, height in pixels" );
   Overscan4TargetHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__Overscan_ValueUpdated, w );

   Overscan4Target_Sizer.SetSpacing( 4 );
   Overscan4Target_Sizer.Add( Overscan4Target_Label );
   Overscan4Target_Sizer.Add( Overscan4TargetX0_NumericEdit );
   Overscan4Target_Sizer.Add( Overscan4TargetY0_NumericEdit );
   Overscan4Target_Sizer.Add( Overscan4TargetWidth_NumericEdit );
   Overscan4Target_Sizer.Add( Overscan4TargetHeight_NumericEdit );
   Overscan4Target_Sizer.AddStretch();

   //

   Overscan_Sizer.SetSpacing( 4 );
   Overscan_Sizer.Add( ImageRegion_Sizer );
   Overscan_Sizer.Add( Overscan1_Sizer );
   Overscan_Sizer.Add( Overscan1Source_Sizer );
   Overscan_Sizer.Add( Overscan1Target_Sizer );
   Overscan_Sizer.Add( Overscan2_Sizer );
   Overscan_Sizer.Add( Overscan2Source_Sizer );
   Overscan_Sizer.Add( Overscan2Target_Sizer );
   Overscan_Sizer.Add( Overscan3_Sizer );
   Overscan_Sizer.Add( Overscan3Source_Sizer );
   Overscan_Sizer.Add( Overscan3Target_Sizer );
   Overscan_Sizer.Add( Overscan4_Sizer );
   Overscan_Sizer.Add( Overscan4Source_Sizer );
   Overscan_Sizer.Add( Overscan4Target_Sizer );

   Overscan_Control.SetSizer( Overscan_Sizer );
   Overscan_Control.AdjustToContents();

   //

   MasterBias_SectionBar.SetTitle( "Master Bias" );
   MasterBias_SectionBar.SetSection( MasterBias_Control );
   MasterBias_SectionBar.EnableTitleCheckBox();
   MasterBias_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );
   MasterBias_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::__CheckSection, w );

   MasterBiasPath_Edit.SetToolTip( "<p>File path of the master bias frame.</p>" );
   MasterBiasPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__MasterFrame_EditCompleted, w );
   MasterBiasPath_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::__FileDrag, w );
   MasterBiasPath_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::__FileDrop, w );

   MasterBiasPath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   MasterBiasPath_ToolButton.SetScaledFixedSize( 20, 20 );
   MasterBiasPath_ToolButton.SetToolTip( "<p>Select the master bias image file.</p>" );
   MasterBiasPath_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__MasterFrame_Click, w );

   MasterBiasPath_Sizer.Add( MasterBiasPath_Edit, 100 );
   MasterBiasPath_Sizer.AddSpacing( 4 );
   MasterBiasPath_Sizer.Add( MasterBiasPath_ToolButton );

   CalibrateMasterBias_CheckBox.SetText( "Calibrate" );
   CalibrateMasterBias_CheckBox.SetToolTip( "<p>Select this option to calibrate the master bias frame "
      "at the beginning of the batch calibration process. Bias frames are only corrected for overscan, when "
      "one or more overscan regions have been defined and are enabled.</p>" );
   CalibrateMasterBias_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__MasterFrame_Click, w );

   CalibrateMasterBias_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CalibrateMasterBias_Sizer.Add( CalibrateMasterBias_CheckBox );
   CalibrateMasterBias_Sizer.AddStretch();

   MasterBias_Sizer.Add( MasterBiasPath_Sizer );
   MasterBias_Sizer.AddSpacing( 4 );
   MasterBias_Sizer.Add( CalibrateMasterBias_Sizer );

   MasterBias_Control.SetSizer( MasterBias_Sizer );
   MasterBias_Control.AdjustToContents();

   //

   MasterDark_SectionBar.SetTitle( "Master Dark" );
   MasterDark_SectionBar.SetSection( MasterDark_Control );
   MasterDark_SectionBar.EnableTitleCheckBox();
   MasterDark_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );
   MasterDark_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::__CheckSection, w );

   MasterDarkPath_Edit.SetToolTip( "<p>File path of the master dark frame.</p>" );
   MasterDarkPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__MasterFrame_EditCompleted, w );
   MasterDarkPath_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::__FileDrag, w );
   MasterDarkPath_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::__FileDrop, w );

   MasterDarkPath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   MasterDarkPath_ToolButton.SetScaledFixedSize( 20, 20 );
   MasterDarkPath_ToolButton.SetToolTip( "<p>Select the master dark image file.</p>" );
   MasterDarkPath_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__MasterFrame_Click, w );

   MasterDarkPath_Sizer.Add( MasterDarkPath_Edit, 100 );
   MasterDarkPath_Sizer.AddSpacing( 4 );
   MasterDarkPath_Sizer.Add( MasterDarkPath_ToolButton );

   CalibrateMasterDark_CheckBox.SetText( "Calibrate" );
   CalibrateMasterDark_CheckBox.SetToolTip( "<p>Select this option to calibrate the master dark frame "
      "at the beginning of the batch calibration process. The master dark frame is corrected for overscan, "
      "when one or more overscan regions have been defined and are enabled, and bias-subtracted, if a master "
      "bias frame has been selected and is enabled.</p>" );
   CalibrateMasterDark_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__MasterFrame_Click, w );

   CalibrateMasterDark_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CalibrateMasterDark_Sizer.Add( CalibrateMasterDark_CheckBox );
   CalibrateMasterDark_Sizer.AddStretch();

   OptimizeDarks_CheckBox.SetText( "Optimize" );
   OptimizeDarks_CheckBox.SetToolTip( "<p>Select this option to apply <i>dark frame optimization.</i></p>"
      "<p>The dark frame optimization routine computes a dark scaling factor to minimize the noise "
      "induced by dark subtraction. Optimization is carried out separately for each target frame, including "
      "the master flat frame, if selected. Dark frame optimization has been implemented using multiscale "
      "(wavelet-based) noise evaluation and linear minimization routines.</p>" );
   OptimizeDarks_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__MasterFrame_Click, w );

   OptimizeDarks_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OptimizeDarks_Sizer.Add( OptimizeDarks_CheckBox );
   OptimizeDarks_Sizer.AddStretch();

   DarkOptimizationThreshold_NumericControl.label.SetText( "Optimization threshold:" );
   DarkOptimizationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   DarkOptimizationThreshold_NumericControl.slider.SetRange( 0, 200 );
   DarkOptimizationThreshold_NumericControl.slider.SetScaledMinWidth( 250 );
   DarkOptimizationThreshold_NumericControl.SetReal();
   DarkOptimizationThreshold_NumericControl.SetRange( TheICDarkOptimizationLowParameter->MinimumValue(), TheICDarkOptimizationLowParameter->MaximumValue() );
   DarkOptimizationThreshold_NumericControl.SetPrecision( TheICDarkOptimizationLowParameter->Precision() );
   DarkOptimizationThreshold_NumericControl.edit.SetFixedWidth( editWidth2 );
   DarkOptimizationThreshold_NumericControl.SetToolTip( "<p>Lower bound for the set of dark optimization pixels, "
      "measured in sigma units from the median.</p>"
      "<p>This parameter defines the set of dark frame pixels that will be used to compute dark optimization "
      "factors adaptively. By restricting this set to relatively bright pixels, the optimization process can "
      "be more robust to readout noise present in the master bias and dark frames. Increase this parameter to "
      "remove more dark pixels from the optimization set.</p>" );
   DarkOptimizationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageCalibrationInterface::__MasterFrame_ValueUpdated, w );

   const char* darkOptimizationWindowTip = "<p>This parameter is the size in pixels of a square region "
      "used to compute noise estimates during the dark optimization procedure. The square region is centered "
      "on each target image. By default, a <i>window</i> of one megapixel (1024x1024 pixels) is used.</p>"
      "<p>By using a reduced subset of pixels, the dark optimization process can be much faster, and noise "
      "evaluation on a reduced region is in general as accurate as noise evaluation on the whole image, for "
      "dark frame scaling purposes.</p>"
      "<p>To disable this feature, and hence to use the whole image to compute noise estimates, select a "
      "zero window size. If the selected window size is larger than the dimensions of the target image, then "
      "it will be ignored and the whole image will be used for noise evaluation.</p>";

   DarkOptimizationWindow_Label.SetText( "Optimization window (px):" );
   DarkOptimizationWindow_Label.SetFixedWidth( labelWidth1 );
   DarkOptimizationWindow_Label.SetToolTip( darkOptimizationWindowTip );
   DarkOptimizationWindow_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DarkOptimizationWindow_SpinBox.SetRange( int( TheICDarkOptimizationWindowParameter->MinimumValue() ), int( TheICDarkOptimizationWindowParameter->MaximumValue() ) );
   DarkOptimizationWindow_SpinBox.SetToolTip( darkOptimizationWindowTip );
   DarkOptimizationWindow_SpinBox.SetFixedWidth( editWidth2 );
   DarkOptimizationWindow_SpinBox.SetStepSize( 256 );
   DarkOptimizationWindow_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageCalibrationInterface::__MasterFrame_SpinValueUpdated, w );

   DarkOptimizationWindow_Sizer.Add( DarkOptimizationWindow_Label );
   DarkOptimizationWindow_Sizer.AddSpacing( 4 );
   DarkOptimizationWindow_Sizer.Add( DarkOptimizationWindow_SpinBox );
   DarkOptimizationWindow_Sizer.AddStretch();

   const char* darkCFADetectionModeToolTip = "<p>Color Filter Array (CFA) detection mode for the master "
      "dark frame.</p>"
      "<p>Dark frame optimization of CFA images, such as Bayerized DSLR or OSC raw images, requires a "
      "previous demosaicing step in order to compute valid noise estimates. This parameter defines how CFA "
      "patters are handled for computation of dark frame scaling factors.</p>"
      "<p><b>Detect CFA</b> is the default option. In this mode, ImageCalibration will honor a CFA pattern "
      "reported by the file format that loads the master dark frame, or will try to detect a CFA pattern "
      "automatically, if the format does not report one. Note that automatic detection of CFA patterns is "
      "only possible for RGB separate Bayer images, such as those produced by the DSLR_RAW standard module. "
      "In general, this is the recommended option.</p>"
      "<p><b>Force CFA.</b> Pretend that all images have been mosaiced with a CFA pattern. This mode can be "
      "used in case ImageCalibration cannot detect existing CFA patterns. For example, you must select this "
      "mode if you work with monochrome CFA images.</p>"
      "<p><b>Ignore CFA.</b> Ignore detected CFA patterns. This mode will lead to computation of erroneous "
      "dark scaling factors if the images are mosaiced.</p>";

   DarkCFADetectionMode_Label.SetText( "CFA pattern detection:" );
   DarkCFADetectionMode_Label.SetToolTip( darkCFADetectionModeToolTip );
   DarkCFADetectionMode_Label.SetMinWidth( labelWidth1 );
   DarkCFADetectionMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DarkCFADetectionMode_ComboBox.AddItem( "Detect CFA" );
   DarkCFADetectionMode_ComboBox.AddItem( "Force CFA" );
   DarkCFADetectionMode_ComboBox.AddItem( "Ignore CFA" );
   DarkCFADetectionMode_ComboBox.SetToolTip( darkCFADetectionModeToolTip );
   DarkCFADetectionMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageCalibrationInterface::__MasterFrame_ItemSelected, w );

   DarkCFADetectionMode_Sizer.SetSpacing( 4 );
   DarkCFADetectionMode_Sizer.Add( DarkCFADetectionMode_Label );
   DarkCFADetectionMode_Sizer.Add( DarkCFADetectionMode_ComboBox );
   DarkCFADetectionMode_Sizer.AddStretch();

   MasterDark_Sizer.SetSpacing( 4 );
   MasterDark_Sizer.Add( MasterDarkPath_Sizer );
   MasterDark_Sizer.Add( CalibrateMasterDark_Sizer );
   MasterDark_Sizer.Add( OptimizeDarks_Sizer );
   MasterDark_Sizer.Add( DarkOptimizationThreshold_NumericControl );
   MasterDark_Sizer.Add( DarkOptimizationWindow_Sizer );
   MasterDark_Sizer.Add( DarkCFADetectionMode_Sizer );

   MasterDark_Control.SetSizer( MasterDark_Sizer );
   MasterDark_Control.AdjustToContents();

   //

   MasterFlat_SectionBar.SetTitle( "Master Flat" );
   MasterFlat_SectionBar.SetSection( MasterFlat_Control );
   MasterFlat_SectionBar.EnableTitleCheckBox();
   MasterFlat_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageCalibrationInterface::__ToggleSection, w );
   MasterFlat_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageCalibrationInterface::__CheckSection, w );

   MasterFlatPath_Edit.SetToolTip( "<p>File path of the master flat frame.</p>" );
   MasterFlatPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageCalibrationInterface::__MasterFrame_EditCompleted, w );
   MasterFlatPath_Edit.OnFileDrag( (Control::file_drag_event_handler)&ImageCalibrationInterface::__FileDrag, w );
   MasterFlatPath_Edit.OnFileDrop( (Control::file_drop_event_handler)&ImageCalibrationInterface::__FileDrop, w );

   MasterFlatPath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   MasterFlatPath_ToolButton.SetScaledFixedSize( 20, 20 );
   MasterFlatPath_ToolButton.SetToolTip( "<p>Select the master flat image file.</p>" );
   MasterFlatPath_ToolButton.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__MasterFrame_Click, w );

   MasterFlatPath_Sizer.Add( MasterFlatPath_Edit, 100 );
   MasterFlatPath_Sizer.AddSpacing( 4 );
   MasterFlatPath_Sizer.Add( MasterFlatPath_ToolButton );

   CalibrateMasterFlat_CheckBox.SetText( "Calibrate" );
   CalibrateMasterFlat_CheckBox.SetToolTip( "<p>Select this option to calibrate the master flat frame "
      "at the beginning of the batch calibration process. The master flat frame is corrected for overscan, "
      "bias-subtracted, and dark-subtracted with optional optimization, when the corresponding overscan "
      "regions and master calibration frames have been defined and are enabled.</p>" );
   CalibrateMasterFlat_CheckBox.OnClick( (Button::click_event_handler)&ImageCalibrationInterface::__MasterFrame_Click, w );

   CalibrateMasterFlat_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CalibrateMasterFlat_Sizer.Add( CalibrateMasterFlat_CheckBox );
   CalibrateMasterFlat_Sizer.AddStretch();

   MasterFlat_Sizer.Add( MasterFlatPath_Sizer );
   MasterFlat_Sizer.AddSpacing( 4 );
   MasterFlat_Sizer.Add( CalibrateMasterFlat_Sizer );

   MasterFlat_Control.SetSizer( MasterFlat_Sizer );
   MasterFlat_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( OutputFiles_SectionBar );
   Global_Sizer.Add( OutputFiles_Control );
   Global_Sizer.Add( Pedestal_SectionBar );
   Global_Sizer.Add( Pedestal_Control );
   Global_Sizer.Add( Overscan_SectionBar );
   Global_Sizer.Add( Overscan_Control );
   Global_Sizer.Add( MasterBias_SectionBar );
   Global_Sizer.Add( MasterBias_Control );
   Global_Sizer.Add( MasterDark_SectionBar );
   Global_Sizer.Add( MasterDark_Control );
   Global_Sizer.Add( MasterFlat_SectionBar );
   Global_Sizer.Add( MasterFlat_Control );

   w.SetSizer( Global_Sizer );

   w.SetFixedWidth();

   FormatHints_Control.Hide();
   Pedestal_Control.Hide();
   Overscan_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCalibrationInterface.cpp - Released 2018-12-12T09:25:25Z
