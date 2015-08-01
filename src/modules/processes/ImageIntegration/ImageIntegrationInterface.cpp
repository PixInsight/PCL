//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0274
// ----------------------------------------------------------------------------
// ImageIntegrationInterface.cpp - Released 2015/07/31 11:49:48 UTC
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

#include "FileDataCachePreferencesDialog.h"
#include "ImageIntegrationInterface.h"
#include "ImageIntegrationProcess.h"
#include "IntegrationCache.h"

#include <pcl/DrizzleDataDecoder.h>
#include <pcl/FileDialog.h>
//#include <pcl/FileFormat.h>
#include <pcl/MessageBox.h>
#include <pcl/PreviewSelectionDialog.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIntegrationInterface* TheImageIntegrationInterface = 0;

// ----------------------------------------------------------------------------

#include "ImageIntegrationIcon.xpm"

// ----------------------------------------------------------------------------

ImageIntegrationInterface::ImageIntegrationInterface() :
ProcessInterface(), instance( TheImageIntegrationProcess ), GUI( 0 )
{
   TheImageIntegrationInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. TreeBox) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

ImageIntegrationInterface::~ImageIntegrationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString ImageIntegrationInterface::Id() const
{
   return "ImageIntegration";
}

MetaProcess* ImageIntegrationInterface::Process() const
{
   return TheImageIntegrationProcess;
}

const char** ImageIntegrationInterface::IconImageXPM() const
{
   return ImageIntegrationIcon_XPM;
}

InterfaceFeatures ImageIntegrationInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal | InterfaceFeature::PreferencesButton;
}

void ImageIntegrationInterface::EditPreferences()
{
   if ( TheIntegrationCache == 0 )
      new IntegrationCache; // loads upon construction
   FileDataCachePreferencesDialog dlg( TheIntegrationCache );
   dlg.Execute();
}

void ImageIntegrationInterface::ResetInstance()
{
   ImageIntegrationInstance defaultInstance( TheImageIntegrationProcess );
   ImportProcess( defaultInstance );
}

bool ImageIntegrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ImageIntegration" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheImageIntegrationProcess;
}

ProcessImplementation* ImageIntegrationInterface::NewProcess() const
{
   return new ImageIntegrationInstance( instance );
}

bool ImageIntegrationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const ImageIntegrationInstance* r = dynamic_cast<const ImageIntegrationInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not an ImageIntegration instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool ImageIntegrationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ImageIntegrationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

void ImageIntegrationInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageIntegrationInterface::UpdateControls()
{
   UpdateInputImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateIntegrationControls();
   UpdateRejectionControls();
   UpdateROIControls();
}

void ImageIntegrationInterface::UpdateInputImagesItem( size_type i )
{
   TreeBox::Node* node = GUI->InputImages_TreeBox[i];
   if ( node == 0 )
      return;

   const ImageIntegrationInstance::ImageItem& item = instance.p_images[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   String fileText;
   if ( !item.drzPath.IsEmpty() )
      fileText = "<d> ";
   if ( GUI->FullPaths_CheckBox.IsChecked() )
      fileText.Append( item.path );
   else
      fileText.Append( File::ExtractNameAndSuffix( item.path ) );

   node->SetText( 2, fileText );
   node->SetAlignment( 2, TextAlign::Left );

   String toolTip = item.path;
   if ( !item.drzPath.IsEmpty() )
   {
      toolTip.Append( '\n' );
      toolTip.Append( item.drzPath );
   }
   node->SetToolTip( 2, toolTip );
}

void ImageIntegrationInterface::UpdateInputImagesList()
{
   int currentIdx = GUI->InputImages_TreeBox.ChildIndex( GUI->InputImages_TreeBox.CurrentNode() );

   GUI->InputImages_TreeBox.DisableUpdates();
   GUI->InputImages_TreeBox.Clear();

   for ( size_type i = 0; i < instance.p_images.Length(); ++i )
   {
      new TreeBox::Node( GUI->InputImages_TreeBox );
      UpdateInputImagesItem( i );
   }

   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->InputImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !instance.p_images.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->InputImages_TreeBox.NumberOfChildren() )
         GUI->InputImages_TreeBox.SetCurrentNode( GUI->InputImages_TreeBox[currentIdx] );

   GUI->InputImages_TreeBox.EnableUpdates();
}

void ImageIntegrationInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->InputImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->InputImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->AddDrizzleFiles_PushButton.Enable( hasItems );
   GUI->ClearDrizzleFiles_PushButton.Enable( hasItems );
   GUI->SetReference_PushButton.Enable( GUI->InputImages_TreeBox.CurrentNode() != 0 );
   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
}

void ImageIntegrationInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( instance.p_inputHints );
}

void ImageIntegrationInterface::UpdateIntegrationControls()
{
   bool isAverage = instance.p_generateIntegratedImage && instance.p_combination == IICombination::Average;
   bool isKeywordWeight = instance.p_weightMode == IIWeightMode::KeywordWeight;

   GUI->Combination_Label.Enable( instance.p_generateIntegratedImage );

   GUI->Combination_ComboBox.SetCurrentItem( instance.p_combination );
   GUI->Combination_ComboBox.Enable( instance.p_generateIntegratedImage );

   GUI->Normalization_ComboBox.SetCurrentItem( instance.p_normalization );

   GUI->WeightMode_Label.Enable( isAverage );

   GUI->WeightMode_ComboBox.Enable( isAverage );
   GUI->WeightMode_ComboBox.SetCurrentItem( instance.p_weightMode );

   GUI->WeightKeyword_Label.Enable( isAverage && isKeywordWeight );

   GUI->WeightScale_ComboBox.Enable( isAverage );
   GUI->WeightScale_ComboBox.SetCurrentItem( instance.p_weightScale );

   GUI->WeightKeyword_Edit.Enable( isAverage && isKeywordWeight );
   GUI->WeightKeyword_Edit.SetText( instance.p_weightKeyword );

   GUI->GenerateIntegratedImage_CheckBox.SetChecked( instance.p_generateIntegratedImage );

   GUI->IgnoreNoiseKeywords_CheckBox.SetChecked( instance.p_ignoreNoiseKeywords );

   GUI->Generate64BitResult_CheckBox.SetChecked( instance.p_generate64BitResult );
   GUI->Generate64BitResult_CheckBox.Enable( instance.p_generateIntegratedImage );

   GUI->GenerateDrizzleData_CheckBox.SetChecked( instance.p_generateDrizzleData );

   GUI->EvaluateNoise_CheckBox.SetChecked( instance.p_evaluateNoise );
   GUI->EvaluateNoise_CheckBox.Enable( instance.p_generateIntegratedImage );

   GUI->ClosePreviousImages_CheckBox.SetChecked( instance.p_closePreviousImages );

   GUI->BufferSize_SpinBox.SetValue( instance.p_bufferSizeMB );
   GUI->StackSize_SpinBox.SetValue( instance.p_stackSizeMB );

   GUI->UseCache_CheckBox.SetChecked( instance.p_useCache );
}

void ImageIntegrationInterface::UpdateRejectionControls()
{
   bool doesRejection = instance.p_rejection != IIRejection::NoRejection;
   bool doesRangeRejection = instance.p_rangeClipLow || instance.p_rangeClipHigh;

   bool doesMinMaxRejection = instance.p_rejection == IIRejection::MinMax;

   bool doesPercentileClipRejection = instance.p_rejection == IIRejection::PercentileClip;

   bool doesSigmaClipRejection = instance.p_rejection == IIRejection::SigmaClip ||
                                 instance.p_rejection == IIRejection::WinsorizedSigmaClip ||
                                 instance.p_rejection == IIRejection::AveragedSigmaClip;

   bool doesLinearFitRejection = instance.p_rejection == IIRejection::LinearFit;

   bool doesCCDClipRejection = instance.p_rejection == IIRejection::CCDClip;

   GUI->RejectionAlgorithm_ComboBox.SetCurrentItem( instance.p_rejection );

   GUI->RejectionNormalization_ComboBox.Enable( doesRejection );
   GUI->RejectionNormalization_ComboBox.SetCurrentItem( instance.p_rejectionNormalization );

   GUI->GenerateRejectionMaps_CheckBox.Enable( doesRejection || doesRangeRejection );
   GUI->GenerateRejectionMaps_CheckBox.SetChecked( instance.p_generateRejectionMaps );

   GUI->ClipLow_CheckBox.Enable( doesRejection );
   GUI->ClipLow_CheckBox.SetChecked( instance.p_clipLow );

   GUI->ClipHigh_CheckBox.Enable( doesRejection );
   GUI->ClipHigh_CheckBox.SetChecked( instance.p_clipHigh );

   GUI->ClipLowRange_CheckBox.SetChecked( instance.p_rangeClipLow );

   GUI->ClipHighRange_CheckBox.SetChecked( instance.p_rangeClipHigh );

   GUI->ReportRangeRejection_CheckBox.Enable( doesRangeRejection );
   GUI->ReportRangeRejection_CheckBox.SetChecked( instance.p_reportRangeRejection );

   GUI->MapRangeRejection_CheckBox.Enable( instance.p_generateRejectionMaps && doesRangeRejection );
   GUI->MapRangeRejection_CheckBox.SetChecked( instance.p_mapRangeRejection );

   GUI->MinMaxLow_Label.Enable( doesMinMaxRejection && instance.p_clipLow );

   GUI->MinMaxLow_SpinBox.Enable( doesMinMaxRejection && instance.p_clipLow );
   GUI->MinMaxLow_SpinBox.SetValue( instance.p_minMaxLow );

   GUI->MinMaxHigh_Label.Enable( doesMinMaxRejection && instance.p_clipHigh );

   GUI->MinMaxHigh_SpinBox.Enable( doesMinMaxRejection && instance.p_clipHigh );
   GUI->MinMaxHigh_SpinBox.SetValue( instance.p_minMaxHigh );

   GUI->PercentileLow_NumericControl.Enable( doesPercentileClipRejection && instance.p_clipLow );
   GUI->PercentileLow_NumericControl.SetValue( instance.p_pcClipLow );

   GUI->PercentileHigh_NumericControl.Enable( doesPercentileClipRejection && instance.p_clipHigh );
   GUI->PercentileHigh_NumericControl.SetValue( instance.p_pcClipHigh );

   GUI->SigmaLow_NumericControl.Enable( doesSigmaClipRejection && instance.p_clipLow );
   GUI->SigmaLow_NumericControl.SetValue( instance.p_sigmaLow );

   GUI->SigmaHigh_NumericControl.Enable( doesSigmaClipRejection && instance.p_clipHigh );
   GUI->SigmaHigh_NumericControl.SetValue( instance.p_sigmaHigh );

   GUI->LinearFitLow_NumericControl.Enable( doesLinearFitRejection );
   GUI->LinearFitLow_NumericControl.SetValue( instance.p_linearFitLow );

   GUI->LinearFitHigh_NumericControl.Enable( doesLinearFitRejection );
   GUI->LinearFitHigh_NumericControl.SetValue( instance.p_linearFitHigh );

   GUI->CCDGain_NumericControl.Enable( doesCCDClipRejection );
   GUI->CCDGain_NumericControl.SetValue( instance.p_ccdGain );

   GUI->CCDReadNoise_NumericControl.Enable( doesCCDClipRejection );
   GUI->CCDReadNoise_NumericControl.SetValue( instance.p_ccdReadNoise );

   GUI->CCDScaleNoise_NumericControl.Enable( doesCCDClipRejection );
   GUI->CCDScaleNoise_NumericControl.SetValue( instance.p_ccdScaleNoise );

   GUI->RangeLow_NumericControl.Enable( instance.p_rangeClipLow );
   GUI->RangeLow_NumericControl.SetValue( instance.p_rangeLow );

   GUI->RangeHigh_NumericControl.Enable( instance.p_rangeClipHigh );
   GUI->RangeHigh_NumericControl.SetValue( instance.p_rangeHigh );
}

void ImageIntegrationInterface::UpdateROIControls()
{
   GUI->ROI_SectionBar.SetChecked( instance.p_useROI );
   GUI->ROIX0_SpinBox.SetValue( instance.p_roi.x0 );
   GUI->ROIY0_SpinBox.SetValue( instance.p_roi.y0 );
   GUI->ROIWidth_SpinBox.SetValue( instance.p_roi.Width() );
   GUI->ROIHeight_SpinBox.SetValue( instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ImageIntegrationInterface::__InputImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.p_images.Length() )
      throw Error( "ImageIntegrationInterface: *Warning* Corrupted interface structures" );

   // ### If there's something that depends on which image is selected in the list, do it here.
}

void ImageIntegrationInterface::__InputImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.p_images.Length() )
      throw Error( "ImageIntegrationInterface: *Warning* Corrupted interface structures" );

   ImageIntegrationInstance::ImageItem& item = instance.p_images[index];

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

void ImageIntegrationInterface::__InputImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != 0) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

String ImageIntegrationInterface::DrizzleTargetName( const String& filePath )
{
   /*
    * Load drizzle data file contents.
    */
   IsoString text = File::ReadTextFile( filePath );

   /*
    * If the .drz file includes a target path, return it.
    */
   DrizzleTargetDecoder decoder;
   decoder.Decode( text );
   if ( decoder.HasTarget() )
   {
      if ( GUI->StaticDrizzleTargets_CheckBox.IsChecked() )
         return File::ChangeExtension( decoder.TargetPath(), String() );
      return File::ExtractName( decoder.TargetPath() );
   }

   /*
    * Otherwise the target should have the same name as the .drz file.
    */
   if ( GUI->StaticDrizzleTargets_CheckBox.IsChecked() )
      return File::ChangeExtension( filePath, String() );
   return File::ExtractName( filePath );
}

void ImageIntegrationInterface::__InputImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();

      d.LoadImageFilters();

      /*
      FileDialog::filter_list filters;
      for ( FileDialog::filter_list::const_iterator i = d.Filters().Begin(); i != d.Filters().End(); ++i )
      {
         FileFormat format( i->Extensions()[0], true, false );
         if ( format.CanReadIncrementally() )
            filters.Add( *i );
      }

      if ( filters.IsEmpty() )
      {
         MessageBox( "No installed file format supports incremental reading",
                     "ImageIntegration",
                     StdIcon::Error,
                     StdButton::Ok ).Execute();
         return;
      }

      d.Filters() = filters;
      */
      d.SetCaption( "ImageIntegration: Select Input Images" );

      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->InputImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.p_images.Insert( instance.p_images.At( i0++ ), ImageIntegrationInstance::ImageItem( *i ) );

         UpdateInputImagesList();
         UpdateImageSelectionButtons();
      }
   }
   else if ( sender == GUI->AddDrizzleFiles_PushButton )
   {
      FileFilter drzFiles;
      drzFiles.SetDescription( "Drizzle Data Files" );
      drzFiles.AddExtension( ".drz" );

      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.Filters().Clear();
      d.Filters().Add( drzFiles );
      d.SetCaption( "ImageIntegration: Select Drizzle Data Files" );
      if ( d.Execute() )
      {
         IVector assigned( 0, int( instance.p_images.Length() ) );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
         {
            String targetName = DrizzleTargetName( *i );
            IVector::iterator n = assigned.Begin();
            for ( ImageIntegrationInstance::image_list::iterator j = instance.p_images.Begin(); j != instance.p_images.End(); ++j, ++n )
            {
               String name = GUI->StaticDrizzleTargets_CheckBox.IsChecked() ?
                                 File::ChangeExtension( j->path, String() ) : File::ExtractName( j->path );
               if ( name == targetName )
               {
                  j->drzPath = *i;
                  ++*n;
                  break;
               }
            }
         }

         UpdateInputImagesList();

         int total = 0;
         int duplicates = 0;
         for ( int i = 0; i < assigned.Length(); ++i )
            if ( assigned[i] > 0 )
            {
               ++total;
               if ( assigned[i] > 1 )
                  ++duplicates;
            }

         if ( total == 0 )
         {
            MessageBox( "<p>No drizzle data files have been assigned to integration source images.</p>",
                        "ImageIntegration",
                        StdIcon::Error,
                        StdButton::Ok ).Execute();
         }
         else
         {
            if ( total < assigned.Length() || duplicates )
               MessageBox( String().Format( "<p>%d of %d drizzle data files have been assigned.<br/>"
                                            "%d duplicate assignment(s)</p>", total, assigned.Length(), duplicates ),
                           "ImageIntegration",
                           StdIcon::Warning,
                           StdButton::Ok ).Execute();

            if ( !instance.p_generateDrizzleData )
            {
               instance.p_generateDrizzleData = true;
               UpdateIntegrationControls();
            }
         }
      }
   }
   else if ( sender == GUI->ClearDrizzleFiles_PushButton )
   {
      for ( ImageIntegrationInstance::image_list::iterator j = instance.p_images.Begin(); j != instance.p_images.End(); ++j )
         j->drzPath.Clear();
      UpdateInputImagesList();

      if ( instance.p_generateDrizzleData )
      {
         instance.p_generateDrizzleData = false;
         UpdateIntegrationControls();
      }
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->InputImages_TreeBox.SelectAllNodes();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->SetReference_PushButton )
   {
      TreeBox::Node* node = GUI->InputImages_TreeBox.CurrentNode();
      if ( node != 0 )
      {
         int idx = GUI->InputImages_TreeBox.ChildIndex( node );
         if ( idx > 0 )
         {
            ImageIntegrationInstance::ImageItem item = instance.p_images[idx];
            instance.p_images.Remove( instance.p_images.At( idx ) );
            item.enabled = true;
            instance.p_images.Insert( instance.p_images.Begin(), item );
            UpdateInputImagesList();
            GUI->InputImages_TreeBox.SetCurrentNode( GUI->InputImages_TreeBox[0] );
            UpdateImageSelectionButtons();
         }
      }
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
            instance.p_images[i].enabled = !instance.p_images[i].enabled;
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      ImageIntegrationInstance::image_list newImages;
      for ( int i = 0, n = GUI->InputImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->InputImages_TreeBox[i]->IsSelected() )
            newImages.Add( instance.p_images[i] );
      instance.p_images = newImages;
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      instance.p_images.Clear();
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateInputImagesList();
      UpdateImageSelectionButtons();
   }
}

void ImageIntegrationInterface::__FormatHints_EditCompleted( Edit& sender )
{
   String hints = sender.Text().Trimmed();

   if ( sender == GUI->InputHints_Edit )
      instance.p_inputHints = hints;

   sender.SetText( hints );
}

void ImageIntegrationInterface::__Integration_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Combination_ComboBox )
   {
      instance.p_combination = itemIndex;
      UpdateIntegrationControls();
   }
   else if ( sender == GUI->Normalization_ComboBox )
   {
      instance.p_normalization = itemIndex;
   }
   else if ( sender == GUI->WeightMode_ComboBox )
   {
      instance.p_weightMode = itemIndex;
      UpdateIntegrationControls();
   }
   else if ( sender == GUI->WeightScale_ComboBox )
   {
      instance.p_weightScale = itemIndex;
      UpdateIntegrationControls();
   }
}

void ImageIntegrationInterface::__Integration_EditCompleted( Edit& sender )
{
   if ( sender == GUI->WeightKeyword_Edit )
   {
      instance.p_weightKeyword = sender.Text();
      instance.p_weightKeyword.Trim();
      instance.p_weightKeyword.ToUppercase();
      sender.SetText( instance.p_weightKeyword );
   }
}

void ImageIntegrationInterface::__Integration_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->BufferSize_SpinBox )
      instance.p_bufferSizeMB = value;
   else if ( sender == GUI->StackSize_SpinBox )
      instance.p_stackSizeMB = value;
}

void ImageIntegrationInterface::__Integration_Click( Button& sender, bool checked )
{
   if ( sender == GUI->GenerateIntegratedImage_CheckBox )
   {
      instance.p_generateIntegratedImage = checked;
      UpdateIntegrationControls();
   }
   else if ( sender == GUI->Generate64BitResult_CheckBox )
      instance.p_generate64BitResult = checked;
   else if ( sender == GUI->GenerateDrizzleData_CheckBox )
      instance.p_generateDrizzleData = checked;
   else if ( sender == GUI->IgnoreNoiseKeywords_CheckBox )
      instance.p_ignoreNoiseKeywords = checked;
   else if ( sender == GUI->EvaluateNoise_CheckBox )
      instance.p_evaluateNoise = checked;
   else if ( sender == GUI->ClosePreviousImages_CheckBox )
      instance.p_closePreviousImages = checked;
   else if ( sender == GUI->UseCache_CheckBox )
      instance.p_useCache = checked;
}

void ImageIntegrationInterface::__Rejection_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->RejectionAlgorithm_ComboBox )
   {
      instance.p_rejection = itemIndex;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->RejectionNormalization_ComboBox )
   {
      instance.p_rejectionNormalization = itemIndex;
      UpdateRejectionControls();
   }
}

void ImageIntegrationInterface::__Rejection_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->MinMaxLow_SpinBox )
      instance.p_minMaxLow = value;
   else if ( sender == GUI->MinMaxHigh_SpinBox )
      instance.p_minMaxHigh = value;
}

void ImageIntegrationInterface::__Rejection_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->PercentileLow_NumericControl )
      instance.p_pcClipLow = value;
   else if ( sender == GUI->PercentileHigh_NumericControl )
      instance.p_pcClipHigh = value;
   else if ( sender == GUI->SigmaLow_NumericControl )
      instance.p_sigmaLow = value;
   else if ( sender == GUI->SigmaHigh_NumericControl )
      instance.p_sigmaHigh = value;
   else if ( sender == GUI->LinearFitLow_NumericControl )
      instance.p_linearFitLow = value;
   else if ( sender == GUI->LinearFitHigh_NumericControl )
      instance.p_linearFitHigh = value;
   else if ( sender == GUI->CCDGain_NumericControl )
      instance.p_ccdGain = value;
   else if ( sender == GUI->CCDReadNoise_NumericControl )
      instance.p_ccdReadNoise = value;
   else if ( sender == GUI->CCDScaleNoise_NumericControl )
      instance.p_ccdScaleNoise = value;
   else if ( sender == GUI->RangeLow_NumericControl )
      instance.p_rangeLow = value;
   else if ( sender == GUI->RangeHigh_NumericControl )
      instance.p_rangeHigh = value;
}

void ImageIntegrationInterface::__Rejection_Click( Button& sender, bool checked )
{
   if ( sender == GUI->GenerateRejectionMaps_CheckBox )
   {
      instance.p_generateRejectionMaps = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipLow_CheckBox )
   {
      instance.p_clipLow = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipHigh_CheckBox )
   {
      instance.p_clipHigh = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipLowRange_CheckBox )
   {
      instance.p_rangeClipLow = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ClipHighRange_CheckBox )
   {
      instance.p_rangeClipHigh = checked;
      UpdateRejectionControls();
   }
   else if ( sender == GUI->ReportRangeRejection_CheckBox )
      instance.p_reportRangeRejection = checked;
   else if ( sender == GUI->MapRangeRejection_CheckBox )
      instance.p_mapRangeRejection = checked;
}

void ImageIntegrationInterface::__ROI_Check( SectionBar& sender, bool checked )
{
   if ( sender == GUI->ROI_SectionBar )
      instance.p_useROI = checked;
}

void ImageIntegrationInterface::__ROI_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ROIX0_SpinBox )
      instance.p_roi.x0 = value;
   else if ( sender == GUI->ROIY0_SpinBox )
      instance.p_roi.y0 = value;
   else if ( sender == GUI->ROIWidth_SpinBox )
      instance.p_roi.x1 = instance.p_roi.x0 + value;
   else if ( sender == GUI->ROIHeight_SpinBox )
      instance.p_roi.y1 = instance.p_roi.y0 + value;
}

void ImageIntegrationInterface::__ROI_Click( Button& sender, bool checked )
{
   if ( sender == GUI->SelectPreview_Button )
   {
      PreviewSelectionDialog d;
      d.SetWindowTitle( "Select ROI Preview" );
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               instance.p_roi = view.Window().PreviewRect( view.Id() );
               UpdateROIControls();
            }
         }
   }
}

void ImageIntegrationInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
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

ImageIntegrationInterface::GUIData::GUIData( ImageIntegrationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "CCD readout noise:" ) + 'T' );
   int editWidth1 = fnt.Width( String( 'M', 16 ) );
   int editWidth2 = fnt.Width( String( '0', 11 ) );
   //int spinWidth1 = fnt.Width( String( '0', 11 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   InputImages_SectionBar.SetTitle( "Input Images" );
   InputImages_SectionBar.SetSection( InputImages_Control );
   InputImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::__ToggleSection, w );

   InputImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   InputImages_TreeBox.SetNumberOfColumns( 3 );
   InputImages_TreeBox.HideHeader();
   InputImages_TreeBox.EnableMultipleSelections();
   InputImages_TreeBox.DisableRootDecoration();
   InputImages_TreeBox.EnableAlternateRowColor();
   InputImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&ImageIntegrationInterface::__InputImages_CurrentNodeUpdated, w );
   InputImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&ImageIntegrationInterface::__InputImages_NodeActivated, w );
   InputImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&ImageIntegrationInterface::__InputImages_NodeSelectionUpdated, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of input images.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   AddDrizzleFiles_PushButton.SetText( "Add Drizzle Files" );
   AddDrizzleFiles_PushButton.SetToolTip( "<p>Associate existing drizzle data files with input images.</p>"
      "<p>Drizzle data files carry the .drz suffix. Normally you should select .drz files generated by "
      "the StarAlignment tool for the same files that you are integrating.</p>" );
   AddDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   ClearDrizzleFiles_PushButton.SetText( "Clear Drizzle Files" );
   ClearDrizzleFiles_PushButton.SetToolTip( "<p>Remove all drizzle data files currently associated with input images.</p>"
      "<p>This removes just file associations, not the actual drizzle data files.</p>" );
   ClearDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   SetReference_PushButton.SetText( "Set Reference" );
   SetReference_PushButton.SetToolTip( "<p>Make the currently selected file on the list the reference image.</p>"
      "<p>The reference image is the first one in the list of images to integrate. Its statistical properties "
      "will be taken as the basis to calculate normalization parameters and relative combination weights for the "
      "rest of integrated images.</p>" );
   SetReference_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all input images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of input images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected input images.</p>"
      "<p>Disabled input images will be ignored during the integration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected input images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   StaticDrizzleTargets_CheckBox.SetText( "Static drizzle targets" );
   StaticDrizzleTargets_CheckBox.SetToolTip( "<p>When assigning drizzle data files to target images, take into account "
      "full file paths stored in .drz files. This allows you to integrate drizzle target images with duplicate file "
      "names on different directories. However, by enabling this option your data set gets tied to specific locations "
      "on the local filesystem. When this option is disabled (the default state), only file names are used to associate "
      "target images with .drz files, which allows you to move your images freely throughout the filesystem, including "
      "the possibility to migrate them to different machines.</p>"
      "<p>Changes to this option will come into play the next time you use Add Drizzle Files to associate .drz files "
      "with target images. Existing file associations are not affected.</p>");
   //StaticDrizzleTargets_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for input image files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__InputImages_Click, w );

   InputButtons_Sizer.SetSpacing( 4 );
   InputButtons_Sizer.Add( AddFiles_PushButton );
   InputButtons_Sizer.Add( AddDrizzleFiles_PushButton );
   InputButtons_Sizer.Add( ClearDrizzleFiles_PushButton );
   InputButtons_Sizer.Add( SetReference_PushButton );
   InputButtons_Sizer.Add( SelectAll_PushButton );
   InputButtons_Sizer.Add( InvertSelection_PushButton );
   InputButtons_Sizer.Add( ToggleSelected_PushButton );
   InputButtons_Sizer.Add( RemoveSelected_PushButton );
   InputButtons_Sizer.Add( Clear_PushButton );
   InputButtons_Sizer.Add( StaticDrizzleTargets_CheckBox );
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
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::__ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings "
      "for image files used by specific processes. In ImageIntegration, input hints change the way input images "
      "of some particular file formats are loaded during the integration process. There are no output hints in "
      "ImageIntegration since this process does not write images to disk files.</p>"
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
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageIntegrationInterface::__FormatHints_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );
   FormatHints_Control.AdjustToContents();

   //

   Integration_SectionBar.SetTitle( "Image Integration" );
   Integration_SectionBar.SetSection( Integration_Control );
   Integration_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::__ToggleSection, w );

   const char* combinationToolTip = "<p>Select a pixel combination operation.</p>"
      "<p>Average combination provides the best signal-to-noise ratio in the integrated result.</p>"
      "<p>Median combination provides more robust rejection of outliers, but at the cost of more noise.</p>";

   Combination_Label.SetText( "Combination:" );
   Combination_Label.SetFixedWidth( labelWidth1 );
   Combination_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Combination_Label.SetToolTip( combinationToolTip );

   Combination_ComboBox.AddItem( "Average" );
   Combination_ComboBox.AddItem( "Median" );
   Combination_ComboBox.AddItem( "Minimum" );
   Combination_ComboBox.AddItem( "Maximum" );
   Combination_ComboBox.SetToolTip( combinationToolTip );
   Combination_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::__Integration_ItemSelected, w );

   Combination_Sizer.SetSpacing( 4 );
   Combination_Sizer.Add( Combination_Label );
   Combination_Sizer.Add( Combination_ComboBox );
   Combination_Sizer.AddStretch();

   const char* normalizationToolTip = "<p>Image normalization for combination.</p>"
      "<p>If one of these options is selected, ImageIntegration will normalize and scale all the input images "
      "before combining them to generate the output image. Note that these normalization and scaling operations "
      "are independent from the similar operations performed before pixel rejection.</p>"

      "<p>Normalization matches mean background values. This can be done either as an additive or as a "
      "multiplicative process. In general, both ways lead to very similar results, but multiplicative "
      "normalization should be used to integrate images that are to be further combined or applied by "
      "multiplication or division, such as flat fields.</p>"

      "<p>Scaling matches dispersion. This works as a sort of <i>automatic weighting</i> to combine the input "
      "images, and tends to improve the SNR of the result, especially for images with different overall illumination.</p>"

      "<p>Additive normalization with scaling is the default option.</p>";

   Normalization_Label.SetText( "Normalization:" );
   Normalization_Label.SetFixedWidth( labelWidth1 );
   Normalization_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Normalization_Label.SetToolTip( normalizationToolTip );

   Normalization_ComboBox.AddItem( "No normalization" );
   Normalization_ComboBox.AddItem( "Additive" );
   Normalization_ComboBox.AddItem( "Multiplicative" );
   Normalization_ComboBox.AddItem( "Additive with scaling" );
   Normalization_ComboBox.AddItem( "Multiplicative with scaling" );
   Normalization_ComboBox.SetToolTip( normalizationToolTip );
   Normalization_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::__Integration_ItemSelected, w );

   Normalization_Sizer.SetSpacing( 4 );
   Normalization_Sizer.Add( Normalization_Label );
   Normalization_Sizer.Add( Normalization_ComboBox );
   Normalization_Sizer.AddStretch();

   const char* weightModeToolTip = "<p>Image weighting criterion.</p>"
      "<p>Exposure times will be retrieved from standard EXPTIME and EXPOSURE FITS keywords (in that order).</p>"

      "<p>The <i>noise evaluation</i> option uses multiscale noise evaluation techniques to compute relative "
      "SNR values that minimize mean squared error in the integrated image. This is the most accurate approach "
      "for automatic image weighting, and therefore the default option.</p>"

      "<p>Noise estimates are obtained either from cached data, if available, or from NOISExx FITS keywords, if "
      "they are present in the images. Otherwise ImageIntegration will compute Gaussian noise estimates using the "
      "MRS noise evaluation algorithm.</p>"

      "<p>The <i>average signal strength</i> option tries to derive relative exposures directly from statistical "
      "properties of the data. This option may not work if some images have additive illumination variations, "
      "such as sky gradients.</p>"

      "<p>If you select the <i>FITS keyword</i> option, please specify the name of a FITS keyword to retrieve "
      "image weights. The specified keyword must be present in all input images and its value must be of numeric "
      "type.</p>";

   WeightMode_Label.SetText( "Weights:" );
   WeightMode_Label.SetFixedWidth( labelWidth1 );
   WeightMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WeightMode_Label.SetToolTip( weightModeToolTip );

   WeightMode_ComboBox.AddItem( "Don't care (all weights = 1)" );
   WeightMode_ComboBox.AddItem( "Exposure time" );
   WeightMode_ComboBox.AddItem( "Noise evaluation" );
   WeightMode_ComboBox.AddItem( "Average signal strength" );
   WeightMode_ComboBox.AddItem( "Median value" );
   WeightMode_ComboBox.AddItem( "Average value" );
   WeightMode_ComboBox.AddItem( "FITS keyword" );
   WeightMode_ComboBox.SetToolTip( weightModeToolTip );
   WeightMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::__Integration_ItemSelected, w );

   WeightMode_Sizer.SetSpacing( 4 );
   WeightMode_Sizer.Add( WeightMode_Label );
   WeightMode_Sizer.Add( WeightMode_ComboBox );
   WeightMode_Sizer.AddStretch();

   const char* weightKeywordToolTip = "<p>Custom FITS keyword to retrieve image weights.</p>"
      "<p>This is the name of a FITS keyword that will be used to retrieve image weights, if the "
      "<i>FITS keyword</i> option has been selected as the weighting criterion.</p>";

   WeightKeyword_Label.SetText( "Weight keyword:" );
   WeightKeyword_Label.SetFixedWidth( labelWidth1 );
   WeightKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WeightKeyword_Label.SetToolTip( weightKeywordToolTip );

   WeightKeyword_Edit.SetMinWidth( editWidth1 );
   WeightKeyword_Edit.SetToolTip( weightKeywordToolTip );
   WeightKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&ImageIntegrationInterface::__Integration_EditCompleted, w );

   WeightKeyword_Sizer.SetSpacing( 4 );
   WeightKeyword_Sizer.Add( WeightKeyword_Label );
   WeightKeyword_Sizer.Add( WeightKeyword_Edit );
   WeightKeyword_Sizer.AddStretch();

   const char* weightScaleToolTip = "<p>Estimator of scale used for image weighting and scaling.</p>"
      "<p>Statistical estimates of scale, or dispersion, are necessary for most image weighting, output and "
      "rejection normalization/scaling algorithms. Accurate and robust scale estimates are crucial to make the data "
      "from the different images statistically compatible.</p>"

      "<p>The <b>average absolute deviation from the median</b> has been the default scale estimator used in previous "
      "versions of the ImageIntegration tool. It is robustified by trimming all pixel samples outside the [0.00002,0.99998] "
      "range, which excludes cold and hot pixels, as well as saturated pixels and bright spurious features (cosmics, etc).</p>"

      "<p>The <b>median absolute deviation from the median (MAD)</b> is a very robust estimator of scale. Although it has "
      "the best possible breakdown point (50%), its Gaussian efficiency is rather low (37%).</p>"

      "<p>The square root of the <b>biweight midvariance</b> is a robust estimator of scale with a 50% breakdown point "
      "(as good as MAD) and high efficiency with respect to several distributions (about 86%).</p>"

      "<p>The square root of the <b>percentage bend midvariance</b> is another robust estimator of scale with high "
      "efficiency (67%) and good resistance to outliers.</p>"

      "<p>The average deviation, MAD, biweight and bend midvariance estimators measure the variability of pixel sample "
      "values around the median. This makes sense for deep-sky images because the median closely represents the mean "
      "background of the image in most cases. However, these estimators work under the assumption that dispersion is "
      "symmetric with respect to the central value, which may not be quite true in many cases.</p>"

      "<p>The <b>Sn and Qn scale estimators</b> of Rousseeuw and Croux don't measure dispersion around a central value. "
      "They evaluate dispersion based on differences between pairs of data points, which makes them robust to asymmetric "
      "and skewed distributions. Sn and Qn are as robust to outliers as MAD, but their Gaussian efficiencies are higher "
      "(58% and 87%, respectively).</p>"

      "<p>The <b>iterative k-sigma scale estimator</b> implements a robust sigma-clipping routine based on the biweight "
      "midvariance, with 92% efficiency and high resistance to outliers. This is the default estimator of scale in current "
      "versions of the ImageIntegration tool.</p>";

   WeightScale_Label.SetText( "Scale estimator:" );
   WeightScale_Label.SetFixedWidth( labelWidth1 );
   WeightScale_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WeightScale_Label.SetToolTip( weightScaleToolTip );

   WeightScale_ComboBox.AddItem( "Average absolute deviation from the median" );
   WeightScale_ComboBox.AddItem( "Median absolute deviation from the median (MAD)" );
   WeightScale_ComboBox.AddItem( "Biweight midvariance" );
   WeightScale_ComboBox.AddItem( "Percentage bend midvariance" );
   WeightScale_ComboBox.AddItem( "Sn estimator of Rousseeuw and Croux" );
   WeightScale_ComboBox.AddItem( "Qn estimator of Rousseeuw and Croux" );
   WeightScale_ComboBox.AddItem( "Iterative k-sigma / biweight midvariance" );
   WeightScale_ComboBox.SetToolTip( weightScaleToolTip );
   WeightScale_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::__Integration_ItemSelected, w );

   WeightScale_Sizer.SetSpacing( 4 );
   WeightScale_Sizer.Add( WeightScale_Label );
   WeightScale_Sizer.Add( WeightScale_ComboBox );
   WeightScale_Sizer.AddStretch();

   IgnoreNoiseKeywords_CheckBox.SetText( "Ignore noise keywords" );
   IgnoreNoiseKeywords_CheckBox.SetToolTip( "<p>Ignore NOISExx keywords. Always compute new noise estimates for "
      "input images, or retrieve them from the cache (if the cache is enabled and valid cached noise estimates are "
      "available).</p>" );
   IgnoreNoiseKeywords_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Integration_Click, w );

   IgnoreNoiseKeywords_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   IgnoreNoiseKeywords_Sizer.Add( IgnoreNoiseKeywords_CheckBox );
   IgnoreNoiseKeywords_Sizer.AddStretch();

   GenerateIntegratedImage_CheckBox.SetText( "Generate integrated image" );
   GenerateIntegratedImage_CheckBox.SetToolTip( "<p>Generate the result of the integration process as a new "
      "image window.</p>"
      "<p>This option should be enabled for normal use. If you disable it, the integrated image won't be "
      "generated at the end of the process. You can disable this option to save a relatively modest amount of "
      "computation time and resources while you are trying out rejection parameters. To evaluate the quality "
      "of pixel rejection, you normally are only interested in rejection pixel counts and/or rejection maps.</p>" );
   GenerateIntegratedImage_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Integration_Click, w );

   GenerateIntegratedImage_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateIntegratedImage_Sizer.Add( GenerateIntegratedImage_CheckBox );
   GenerateIntegratedImage_Sizer.AddStretch();

   Generate64BitResult_CheckBox.SetText( "Generate a 64-bit result image" );
   Generate64BitResult_CheckBox.SetToolTip( "<p>If this option is selected, ImageIntegration will generate a "
      "64-bit floating point result image. Otherwise the integration result will be generated as a 32-bit "
      "floating point image.</p>" );
   Generate64BitResult_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Integration_Click, w );

   Generate64BitResult_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Generate64BitResult_Sizer.Add( Generate64BitResult_CheckBox );
   Generate64BitResult_Sizer.AddStretch();

   GenerateDrizzleData_CheckBox.SetText( "Generate drizzle data" );
   GenerateDrizzleData_CheckBox.SetToolTip( "<p>Generate a drizzle data file for each integrated image.</p>"
      "<p>Drizzle data files contain geometrical projection parameters, pixel rejection and statistical data "
      "for the DrizzleIntegration tool. StarAlignment creates drizzle files with projection data, and the "
      "ImageIntegration tool appends rejection and statistical data to the same files. Drizzle files carry "
      "the .drz file suffix.</p>" );
   GenerateDrizzleData_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Integration_Click, w );

   GenerateDrizzleData_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateDrizzleData_Sizer.Add( GenerateDrizzleData_CheckBox );
   GenerateDrizzleData_Sizer.AddStretch();

   EvaluateNoise_CheckBox.SetText( "Evaluate noise" );
   EvaluateNoise_CheckBox.SetToolTip( "<p>Evaluate the standard deviation of Gaussian noise for the final "
      "integrated image.</p>"
      "<p>Noise evaluation uses wavelet-based techniques and provides estimates to within 1% accuracy. "
      "This option is useful to compare the results of different integration procedures. For example, by "
      "comparing noise estimates you can know which image normalization and weighting criteria lead to the "
      "best result in terms of signal-to-noise ratio improvement.</p>" );
   EvaluateNoise_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Integration_Click, w );

   EvaluateNoise_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   EvaluateNoise_Sizer.Add( EvaluateNoise_CheckBox );
   EvaluateNoise_Sizer.AddStretch();

   ClosePreviousImages_CheckBox.SetText( "Close previous images" );
   ClosePreviousImages_CheckBox.SetToolTip( "<p>Select this option to close existing integration and rejection "
      "map images before running a new integration process. This is useful to avoid accumulation of multiple "
      "results on the workspace, when the same integration is being tested repeatedly.</p>" );
   ClosePreviousImages_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Integration_Click, w );

   ClosePreviousImages_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClosePreviousImages_Sizer.Add( ClosePreviousImages_CheckBox );
   ClosePreviousImages_Sizer.AddStretch();

   const char* bufferSizeToolTip =
      "<p>This parameter defines the size of the working buffers used to read pixel rows. There is an "
      "independent buffer per input image. A reasonably large buffer size will improve performance by "
      "minimizing disk reading operations. The default value of 16 MiB is usually quite appropriate.</p>"
      "<p>Decrease this parameter if you experience out-of-memory errors during integration. This may "
      "be necessary for integration of large image sets on systems with low memory resources. The "
      "minimum value is zero, which will use a single row of pixels per input image.</p>";

   BufferSize_Label.SetText( "Buffer size (MiB):" );
   BufferSize_Label.SetFixedWidth( labelWidth1 );
   BufferSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BufferSize_Label.SetToolTip( bufferSizeToolTip );

   BufferSize_SpinBox.SetRange( int( TheIIBufferSizeParameter->MinimumValue() ), int( TheIIBufferSizeParameter->MaximumValue() ) );
   BufferSize_SpinBox.SetToolTip( bufferSizeToolTip );
   BufferSize_SpinBox.SetFixedWidth( editWidth2 );
   BufferSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__Integration_SpinValueUpdated, w );

   BufferSize_Sizer.SetSpacing( 4 );
   BufferSize_Sizer.Add( BufferSize_Label );
   BufferSize_Sizer.Add( BufferSize_SpinBox );
   BufferSize_Sizer.AddStretch();

   const char* stackSizeToolTip =
      "<p>This is the size of the working integration stack structure. In general, the larger this "
      "parameter, the better the performance, especially on multiprocessor/multicore systems.</p>"
      "<p>The best performance is achieved when the whole set of integrated pixels can be loaded at "
      "once in the integration stack. For this to happen, the following conditions must be true: (1) "
      "<i>buffer size</i> must be large enough as to allow loading an input file (in 32-bit floating "
      "point format) completely in a single file reading operation, and (2) <i>stack size</i> must be "
      "larger than or equal to W*H*(12*N + 4), where W and H are the image width and height in pixels, "
      "respectively, and N is the number of integrated images. For linear fit clipping rejection, "
      "replace 4 with 8 in the above equation. Note that this may require a large amount of RAM "
      "available for relatively large image sets. As an example, the default stack size of 1024 (1 GiB) "
      "is sufficient to integrate 20 2048x2048 monochrome images optimally with the default buffer size "
      "of 16 MiB. With a stack size of 4 GiB and a buffer size of 64 MiB you could integrate 20 4Kx4K "
      "monochrome images with optimum performance on a 64-bit version of PixInsight.</p>";

   StackSize_Label.SetText( "Stack size (MiB):" );
   StackSize_Label.SetFixedWidth( labelWidth1 );
   StackSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StackSize_Label.SetToolTip( stackSizeToolTip );

   // Limit to one half the parameter's maximum (500 GiB) to keep the SpinBox size within editWidth2.
   StackSize_SpinBox.SetRange( int( TheIIStackSizeParameter->MinimumValue() ), int( TheIIStackSizeParameter->MaximumValue() ) >> 1 );
   StackSize_SpinBox.SetToolTip( stackSizeToolTip );
   StackSize_SpinBox.SetFixedWidth( editWidth2 );
   StackSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__Integration_SpinValueUpdated, w );

   StackSize_Sizer.SetSpacing( 4 );
   StackSize_Sizer.Add( StackSize_Label );
   StackSize_Sizer.Add( StackSize_SpinBox );
   StackSize_Sizer.AddStretch();

   UseCache_CheckBox.SetText( "Use file cache" );
   UseCache_CheckBox.SetToolTip( "<p>By default, ImageIntegration generates and uses a dynamic cache of "
      "working image parameters, including pixel statistics and normalization data. This cache greatly "
      "improves performance when the same images are being integrated several times, for example to find "
      "optimal pixel rejection parameters.</p>"
      "<p>Disable this option if for some reason you don't want to use the cache. This will force "
      "recalculation of all statistical data required for normalization, which involves loading all "
      "integrated image files from disk.</p>"
      "<p>The file cache can also be <i>persistent</i> across PixInsight Core executions. The persistent "
      "cache and its options can be controlled with the ImageIntegration Preferences dialog.</p>");
   UseCache_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Integration_Click, w );

   Cache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Cache_Sizer.Add( UseCache_CheckBox );
   Cache_Sizer.AddStretch();

   Integration_Sizer.SetSpacing( 4 );
   Integration_Sizer.Add( Combination_Sizer );
   Integration_Sizer.Add( Normalization_Sizer );
   Integration_Sizer.Add( WeightMode_Sizer );
   Integration_Sizer.Add( WeightKeyword_Sizer );
   Integration_Sizer.Add( WeightScale_Sizer );
   Integration_Sizer.Add( IgnoreNoiseKeywords_Sizer );
   Integration_Sizer.Add( GenerateIntegratedImage_Sizer );
   Integration_Sizer.Add( Generate64BitResult_Sizer );
   Integration_Sizer.Add( GenerateDrizzleData_Sizer );
   Integration_Sizer.Add( EvaluateNoise_Sizer );
   Integration_Sizer.Add( ClosePreviousImages_Sizer );
   Integration_Sizer.Add( BufferSize_Sizer );
   Integration_Sizer.Add( StackSize_Sizer );
   Integration_Sizer.Add( Cache_Sizer );

   Integration_Control.SetSizer( Integration_Sizer );
   Integration_Control.AdjustToContents();

   //

   Rejection1_SectionBar.SetTitle( "Pixel Rejection (1)" );
   Rejection1_SectionBar.SetSection( Rejection1_Control );
   Rejection1_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::__ToggleSection, w );

   const char* rejectionAlgorithmToolTip = "<p>Pixel rejection algorithm</p>"

      "<p>The <b>iterative sigma clipping</b> algorithm is usually a good option to integrate more than "
      "10 or 15 images. Keep in mind that for sigma clipping to work, the standard deviation must be a good "
      "estimate of dispersion, which requires a sufficient number of pixels per stack (the more images the "
      "better).</p>"

      "<p><b>Winsorized sigma clipping</b> is similar to the normal sigma clipping algorithm, but uses a "
      "special iterative procedure based on Huber's method of robust estimation of parameters through "
      "<i>Winsorization</i>. This algorithm can yield superior rejection of outliers with better preservation "
      "of significant data for large sets of images.</p>"

      "<p><b>Percentile clipping</b> rejection is excellent to integrate reduced sets of images, such as "
      "3 to 6 images. This is a single-pass algorithm that rejects pixels outside a fixed range of values "
      "relative to the median of each pixel stack.</p>"

      "<p><b>Averaged iterative sigma clipping</b> is intended for sets of 10 or more images. This algorithm "
      "tries to derive the gain of an ideal CCD detector from existing pixel data, assuming zero readout noise, "
      "then uses a Poisson noise model to perform rejection. For large sets of images however, sigma clipping "
      "tends to be superior.</p>"

      "<p><b>Linear fit clipping</b> fits each pixel stack to a straigtht line. The linear fit is optimized "
      "in the twofold sense of minimizing average absolute deviation and maximizing inliers. This rejection "
      "algorithm is more robust than sigma clipping for large sets of images, especially in presence of "
      "additive sky gradients of varying intensity and spatial distribution. For the best performance, use "
      "this algorithm for large sets of at least 15 images. Five images is the minimum required.</p>"

      "<p>The <b>min/max</b> method can be used to ensure rejection of extreme values. Min/max performs an "
      "unconditional rejection of a fixed number of pixels from each stack, without any statistical basis. "
      "Rejection methods based on robust statistics, such as percentile, Winsorized sigma clipping, linear "
      "fitting and averaged sigma clipping are in general preferable.</p>"

      "<p>Finally, the <b>CCD noise model</b> algorithm requires unmodified (uncalibrated) data and accurate "
      "sensor parameters. This rejection algorithm can only be useful to integrate calibration images (bias "
      "frames, dark frames and flat fields).</p>";

   RejectionAlgorithm_Label.SetText( "Rejection algorithm:" );
   RejectionAlgorithm_Label.SetFixedWidth( labelWidth1 );
   RejectionAlgorithm_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RejectionAlgorithm_Label.SetToolTip( rejectionAlgorithmToolTip );

   RejectionAlgorithm_ComboBox.AddItem( "No rejection" );
   RejectionAlgorithm_ComboBox.AddItem( "Min/Max" );
   RejectionAlgorithm_ComboBox.AddItem( "Percentile Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Sigma Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Winsorized Sigma Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Averaged Sigma Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "Linear Fit Clipping" );
   RejectionAlgorithm_ComboBox.AddItem( "CCD Noise Model" );
   RejectionAlgorithm_ComboBox.SetToolTip( rejectionAlgorithmToolTip );
   RejectionAlgorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::__Rejection_ItemSelected, w );

   RejectionAlgorithm_Sizer.SetSpacing( 4 );
   RejectionAlgorithm_Sizer.Add( RejectionAlgorithm_Label );
   RejectionAlgorithm_Sizer.Add( RejectionAlgorithm_ComboBox );
   RejectionAlgorithm_Sizer.AddStretch();

   const char* rejectionNormalizationToolTip = "<p>Image normalization for pixel rejection.</p>"
      "<p>Normalization is essential to perform a correct pixel rejection, since it ensures that the data "
      "from all integrated images are compatible in terms of their statistical distribution (mean background "
      "values and dispersion).</p>"
      "<p><i>Scale + zero offset</i> matches mean background values and dispersion. This involves "
      "multiplicative and additive transformations. This is the default rejection normalization method that "
      "should be used to integrate calibrated images.</p>"
      "<p><i>Equalize fluxes</i> simply matches the main histogram peaks of all images prior to pixel rejection. "
      "This is done by multiplication with the ratio of the reference median to the median of each integrated image. "
      "This is the method of choice to integrate sky flat fields, since in this case trying to match dispersion "
      "does not make sense, due to the irregular illumination distribution. For the same reason, this type of "
      "rejection normalization can also be useful to integrate uncalibrated images, or images suffering from "
      "strong gradients.</p>";

   RejectionNormalization_Label.SetText( "Normalization:" );
   RejectionNormalization_Label.SetFixedWidth( labelWidth1 );
   RejectionNormalization_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RejectionNormalization_Label.SetToolTip( rejectionNormalizationToolTip );

   RejectionNormalization_ComboBox.AddItem( "No normalization" );
   RejectionNormalization_ComboBox.AddItem( "Scale + zero offset" );
   RejectionNormalization_ComboBox.AddItem( "Equalize fluxes" );
   RejectionNormalization_ComboBox.SetToolTip( rejectionNormalizationToolTip );
   RejectionNormalization_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ImageIntegrationInterface::__Rejection_ItemSelected, w );

   RejectionNormalization_Sizer.SetSpacing( 4 );
   RejectionNormalization_Sizer.Add( RejectionNormalization_Label );
   RejectionNormalization_Sizer.Add( RejectionNormalization_ComboBox );
   RejectionNormalization_Sizer.AddStretch();

   GenerateRejectionMaps_CheckBox.SetText( "Generate rejection maps" );
   GenerateRejectionMaps_CheckBox.SetToolTip( "<p>Rejection maps have pixel values proportional to the "
      "number of rejected pixels for each output pixel. Low and high rejected pixels are represented as two "
      "separate rejection maps, which are generated as 32-bit floating point images.</p>"
      "<p>Use rejection maps along with console rejection statistics to evaluate performance of pixel "
      "rejection parameters.</p>" );
   GenerateRejectionMaps_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Rejection_Click, w );

   GenerateRejectionMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateRejectionMaps_Sizer.Add( GenerateRejectionMaps_CheckBox );
   GenerateRejectionMaps_Sizer.AddStretch();

   ClipLow_CheckBox.SetText( "Clip low pixels" );
   ClipLow_CheckBox.SetToolTip( "<p>Enable rejection of low pixels. Low pixels have values below the median of "
      "a pixel stack.</p>" );
   ClipLow_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Rejection_Click, w );

   ClipLow_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipLow_Sizer.Add( ClipLow_CheckBox );
   ClipLow_Sizer.AddStretch();

   ClipHigh_CheckBox.SetText( "Clip high pixels" );
   ClipHigh_CheckBox.SetToolTip( "<p>Enable rejection of high pixels. High pixels have values above the median of "
      "a pixel stack.</p>" );
   ClipHigh_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Rejection_Click, w );

   ClipHigh_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipHigh_Sizer.Add( ClipHigh_CheckBox );
   ClipHigh_Sizer.AddStretch();

   ClipLowRange_CheckBox.SetText( "Clip low range" );
   ClipLowRange_CheckBox.SetToolTip( "<p>Enable rejection of pixels with values less than or equal to the "
      "<i>low rejection range</i> parameter.</p>" );
   ClipLowRange_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Rejection_Click, w );

   ClipLowRange_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipLowRange_Sizer.Add( ClipLowRange_CheckBox );
   ClipLowRange_Sizer.AddStretch();

   ClipHighRange_CheckBox.SetText( "Clip high range" );
   ClipHighRange_CheckBox.SetToolTip( "<p>Enable rejection of pixels with values greater than or equal to the "
      "<i>high rejection range</i> parameter.</p>" );
   ClipHighRange_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Rejection_Click, w );

   ClipHighRange_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClipHighRange_Sizer.Add( ClipHighRange_CheckBox );
   ClipHighRange_Sizer.AddStretch();

   ReportRangeRejection_CheckBox.SetText( "Report range rejection" );
   ReportRangeRejection_CheckBox.SetToolTip( "<p>Include range rejected pixels in rejected pixel counts "
      "reported on console summaries.</p>" );
   ReportRangeRejection_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Rejection_Click, w );

   ReportRangeRejection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ReportRangeRejection_Sizer.Add( ReportRangeRejection_CheckBox );
   ReportRangeRejection_Sizer.AddStretch();

   MapRangeRejection_CheckBox.SetText( "Map range rejection" );
   MapRangeRejection_CheckBox.SetToolTip( "<p>Include range rejected pixels in pixel rejection maps.</p>" );
   MapRangeRejection_CheckBox.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__Rejection_Click, w );

   MapRangeRejection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MapRangeRejection_Sizer.Add( MapRangeRejection_CheckBox );
   MapRangeRejection_Sizer.AddStretch();

   Rejection1_Sizer.SetSpacing( 4 );
   Rejection1_Sizer.Add( RejectionAlgorithm_Sizer );
   Rejection1_Sizer.Add( RejectionNormalization_Sizer );
   Rejection1_Sizer.Add( GenerateRejectionMaps_Sizer );
   Rejection1_Sizer.Add( ClipLow_Sizer );
   Rejection1_Sizer.Add( ClipHigh_Sizer );
   Rejection1_Sizer.Add( ClipLowRange_Sizer );
   Rejection1_Sizer.Add( ClipHighRange_Sizer );
   Rejection1_Sizer.Add( ReportRangeRejection_Sizer );
   Rejection1_Sizer.Add( MapRangeRejection_Sizer );

   Rejection1_Control.SetSizer( Rejection1_Sizer );
   Rejection1_Control.AdjustToContents();

   //

   Rejection2_SectionBar.SetTitle( "Pixel Rejection (2)" );
   Rejection2_SectionBar.SetSection( Rejection2_Control );
   Rejection2_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::__ToggleSection, w );

   const char* minMaxLowToolTip = "<p>Number of low (dark) pixels to be rejected by the min/max algorithm.</p>";

   MinMaxLow_Label.SetText( "Min/Max low:" );
   MinMaxLow_Label.SetFixedWidth( labelWidth1 );
   MinMaxLow_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MinMaxLow_Label.SetToolTip( minMaxLowToolTip );

   MinMaxLow_SpinBox.SetRange( int( TheIIMinMaxLowParameter->MinimumValue() ), 65536 );
   MinMaxLow_SpinBox.SetToolTip( minMaxLowToolTip );
   MinMaxLow_SpinBox.SetFixedWidth( editWidth2 );
   MinMaxLow_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__Rejection_SpinValueUpdated, w );

   MinMaxLow_Sizer.SetSpacing( 4 );
   MinMaxLow_Sizer.Add( MinMaxLow_Label );
   MinMaxLow_Sizer.Add( MinMaxLow_SpinBox );
   MinMaxLow_Sizer.AddStretch();

   const char* minMaxHighToolTip = "<p>Number of high (bright) pixels to be rejected by the min/max algorithm.</p>";

   MinMaxHigh_Label.SetText( "Min/Max high:" );
   MinMaxHigh_Label.SetFixedWidth( labelWidth1 );
   MinMaxHigh_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MinMaxHigh_Label.SetToolTip( minMaxHighToolTip );

   MinMaxHigh_SpinBox.SetRange( int( TheIIMinMaxHighParameter->MinimumValue() ), 65536 );
   MinMaxHigh_SpinBox.SetToolTip( minMaxHighToolTip );
   MinMaxHigh_SpinBox.SetFixedWidth( editWidth2 );
   MinMaxHigh_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__Rejection_SpinValueUpdated, w );

   MinMaxHigh_Sizer.SetSpacing( 4 );
   MinMaxHigh_Sizer.Add( MinMaxHigh_Label );
   MinMaxHigh_Sizer.Add( MinMaxHigh_SpinBox );
   MinMaxHigh_Sizer.AddStretch();

   PercentileLow_NumericControl.label.SetText( "Percentile low:" );
   PercentileLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   PercentileLow_NumericControl.slider.SetRange( 0, 100 );
   PercentileLow_NumericControl.slider.SetScaledMinWidth( 250 );
   PercentileLow_NumericControl.SetReal();
   PercentileLow_NumericControl.SetRange( TheIIPercentileLowParameter->MinimumValue(), TheIIPercentileLowParameter->MaximumValue() );
   PercentileLow_NumericControl.SetPrecision( TheIIPercentileLowParameter->Precision() );
   PercentileLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   PercentileLow_NumericControl.SetToolTip( "<p>Low clipping factor for the percentile clipping rejection algorithm.</p>" );
   PercentileLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   PercentileHigh_NumericControl.label.SetText( "Percentile high:" );
   PercentileHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   PercentileHigh_NumericControl.slider.SetRange( 0, 100 );
   PercentileHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   PercentileHigh_NumericControl.SetReal();
   PercentileHigh_NumericControl.SetRange( TheIIPercentileHighParameter->MinimumValue(), TheIIPercentileHighParameter->MaximumValue() );
   PercentileHigh_NumericControl.SetPrecision( TheIIPercentileHighParameter->Precision() );
   PercentileHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   PercentileHigh_NumericControl.SetToolTip( "<p>High clipping factor for the percentile clipping rejection algorithm.</p>" );
   PercentileHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   SigmaLow_NumericControl.label.SetText( "Sigma low:" );
   SigmaLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   SigmaLow_NumericControl.slider.SetRange( 0, 100 );
   SigmaLow_NumericControl.slider.SetScaledMinWidth( 250 );
   SigmaLow_NumericControl.SetReal();
   SigmaLow_NumericControl.SetRange( TheIISigmaLowParameter->MinimumValue(), TheIISigmaLowParameter->MaximumValue() );
   SigmaLow_NumericControl.SetPrecision( TheIISigmaLowParameter->Precision() );
   SigmaLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   SigmaLow_NumericControl.SetToolTip( "<p>Low sigma clipping factor for the sigma clipping and averaged sigma clipping "
      "rejection algorithms.</p>" );
   SigmaLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   SigmaHigh_NumericControl.label.SetText( "Sigma high:" );
   SigmaHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   SigmaHigh_NumericControl.slider.SetRange( 0, 100 );
   SigmaHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   SigmaHigh_NumericControl.SetReal();
   SigmaHigh_NumericControl.SetRange( TheIISigmaHighParameter->MinimumValue(), TheIISigmaHighParameter->MaximumValue() );
   SigmaHigh_NumericControl.SetPrecision( TheIISigmaHighParameter->Precision() );
   SigmaHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   SigmaHigh_NumericControl.SetToolTip( "<p>High sigma clipping factor for the sigma clipping and averaged sigma clipping "
      "rejection algorithms.</p>" );
   SigmaHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   LinearFitLow_NumericControl.label.SetText( "Linear fit low:" );
   LinearFitLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   LinearFitLow_NumericControl.slider.SetRange( 0, 100 );
   LinearFitLow_NumericControl.slider.SetScaledMinWidth( 250 );
   LinearFitLow_NumericControl.SetReal();
   LinearFitLow_NumericControl.SetRange( TheIILinearFitLowParameter->MinimumValue(), TheIILinearFitLowParameter->MaximumValue() );
   LinearFitLow_NumericControl.SetPrecision( TheIILinearFitLowParameter->Precision() );
   LinearFitLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   LinearFitLow_NumericControl.SetToolTip( "<p>Tolerance of the linear fit clipping algorithm for low pixel values, in sigma units.</p>" );
   LinearFitLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   LinearFitHigh_NumericControl.label.SetText( "Linear fit high:" );
   LinearFitHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   LinearFitHigh_NumericControl.slider.SetRange( 0, 100 );
   LinearFitHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   LinearFitHigh_NumericControl.SetReal();
   LinearFitHigh_NumericControl.SetRange( TheIILinearFitHighParameter->MinimumValue(), TheIILinearFitHighParameter->MaximumValue() );
   LinearFitHigh_NumericControl.SetPrecision( TheIILinearFitHighParameter->Precision() );
   LinearFitHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   LinearFitHigh_NumericControl.SetToolTip( "<p>Tolerance of the linear fit clipping algorithm for high pixel values, in sigma units.</p>" );
   LinearFitHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   RangeLow_NumericControl.label.SetText( "Range low:" );
   RangeLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeLow_NumericControl.slider.SetRange( 0, 100 );
   RangeLow_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeLow_NumericControl.SetReal();
   RangeLow_NumericControl.SetRange( TheIIRangeLowParameter->MinimumValue(), TheIIRangeLowParameter->MaximumValue() );
   RangeLow_NumericControl.SetPrecision( TheIIRangeLowParameter->Precision() );
   RangeLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   RangeLow_NumericControl.SetToolTip( "<p>Low rejection range. If the <i>clip low range</i> option is enabled, "
      "all pixels with values less than or equal to this low rejection range parameter will be rejected.</p>" );
   RangeLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   RangeHigh_NumericControl.label.SetText( "Range high:" );
   RangeHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeHigh_NumericControl.slider.SetRange( 0, 100 );
   RangeHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeHigh_NumericControl.SetReal();
   RangeHigh_NumericControl.SetRange( TheIIRangeHighParameter->MinimumValue(), TheIIRangeHighParameter->MaximumValue() );
   RangeHigh_NumericControl.SetPrecision( TheIIRangeHighParameter->Precision() );
   RangeHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   RangeHigh_NumericControl.SetToolTip( "<p>High rejection range. If the <i>clip high range</i> option is enabled, "
      "all pixels with values greater than or equal to this high rejection range parameter will be rejected.</p>" );
   RangeHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   Rejection2_Sizer.SetSpacing( 4 );
   Rejection2_Sizer.Add( MinMaxLow_Sizer );
   Rejection2_Sizer.Add( MinMaxHigh_Sizer );
   Rejection2_Sizer.Add( PercentileLow_NumericControl );
   Rejection2_Sizer.Add( PercentileHigh_NumericControl );
   Rejection2_Sizer.Add( SigmaLow_NumericControl );
   Rejection2_Sizer.Add( SigmaHigh_NumericControl );
   Rejection2_Sizer.Add( LinearFitLow_NumericControl );
   Rejection2_Sizer.Add( LinearFitHigh_NumericControl );
   Rejection2_Sizer.Add( RangeLow_NumericControl );
   Rejection2_Sizer.Add( RangeHigh_NumericControl );

   Rejection2_Control.SetSizer( Rejection2_Sizer );
   Rejection2_Control.AdjustToContents();

   //

   Rejection3_SectionBar.SetTitle( "Pixel Rejection (3)" );
   Rejection3_SectionBar.SetSection( Rejection3_Control );
   Rejection3_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::__ToggleSection, w );

   CCDGain_NumericControl.label.SetText( "CCD gain:" );
   CCDGain_NumericControl.label.SetFixedWidth( labelWidth1 );
   CCDGain_NumericControl.slider.SetRange( 0, 100 );
   CCDGain_NumericControl.slider.SetScaledMinWidth( 250 );
   CCDGain_NumericControl.SetReal();
   CCDGain_NumericControl.SetRange( TheIICCDGainParameter->MinimumValue(), TheIICCDGainParameter->MaximumValue() );
   CCDGain_NumericControl.SetPrecision( TheIICCDGainParameter->Precision() );
   CCDGain_NumericControl.edit.SetFixedWidth( editWidth2 );
   CCDGain_NumericControl.SetToolTip( "<p>CCD sensor gain in electrons per data number (e-/ADU). Only used by the CCD noise "
      "clipping rejection algorithm.</p>" );
   CCDGain_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   CCDReadNoise_NumericControl.label.SetText( "CCD readout noise:" );
   CCDReadNoise_NumericControl.label.SetFixedWidth( labelWidth1 );
   CCDReadNoise_NumericControl.slider.SetRange( 0, 100 );
   CCDReadNoise_NumericControl.slider.SetScaledMinWidth( 250 );
   CCDReadNoise_NumericControl.SetReal();
   CCDReadNoise_NumericControl.SetRange( TheIICCDReadNoiseParameter->MinimumValue(), TheIICCDReadNoiseParameter->MaximumValue() );
   CCDReadNoise_NumericControl.SetPrecision( TheIICCDReadNoiseParameter->Precision() );
   CCDReadNoise_NumericControl.edit.SetFixedWidth( editWidth2 );
   CCDReadNoise_NumericControl.SetToolTip( "<p>CCD readout noise in electrons. Only used by the CCD noise "
      "clipping rejection algorithm.</p>" );
   CCDReadNoise_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   CCDScaleNoise_NumericControl.label.SetText( "CCD scale noise:" );
   CCDScaleNoise_NumericControl.label.SetFixedWidth( labelWidth1 );
   CCDScaleNoise_NumericControl.slider.SetRange( 0, 100 );
   CCDScaleNoise_NumericControl.slider.SetScaledMinWidth( 250 );
   CCDScaleNoise_NumericControl.SetReal();
   CCDScaleNoise_NumericControl.SetRange( TheIICCDScaleNoiseParameter->MinimumValue(), TheIICCDScaleNoiseParameter->MaximumValue() );
   CCDScaleNoise_NumericControl.SetPrecision( TheIICCDScaleNoiseParameter->Precision() );
   CCDScaleNoise_NumericControl.edit.SetFixedWidth( editWidth2 );
   CCDScaleNoise_NumericControl.SetToolTip( "<p>CCD <i>scale noise</i> (AKA <i>sensitivity noise</i>). This is a "
      "dimensionless factor, only used by the CCD noise clipping rejection algorithm. Scale noise typically comes "
      "from noise introduced during flat fielding.</p>" );
   CCDScaleNoise_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ImageIntegrationInterface::__Rejection_EditValueUpdated, w );

   Rejection3_Sizer.SetSpacing( 4 );
   Rejection3_Sizer.Add( CCDGain_NumericControl );
   Rejection3_Sizer.Add( CCDReadNoise_NumericControl );
   Rejection3_Sizer.Add( CCDScaleNoise_NumericControl );

   Rejection3_Control.SetSizer( Rejection3_Sizer );
   Rejection3_Control.AdjustToContents();

   //

   ROI_SectionBar.SetTitle( "Region of Interest" );
   ROI_SectionBar.EnableTitleCheckBox();
   ROI_SectionBar.OnCheck( (SectionBar::check_event_handler)&ImageIntegrationInterface::__ROI_Check, w );
   ROI_SectionBar.SetSection( ROI_Control );
   ROI_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&ImageIntegrationInterface::__ToggleSection, w );

   const char* roiX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIX0_Label.SetText( "Left:" );
   ROIX0_Label.SetFixedWidth( labelWidth1 );
   ROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIX0_Label.SetToolTip( roiX0ToolTip );

   ROIX0_SpinBox.SetRange( 0, int_max );
   //ROIX0_SpinBox.SetFixedWidth( spinWidth1 );
   ROIX0_SpinBox.SetToolTip( roiX0ToolTip );
   ROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__ROI_SpinValueUpdated, w );

   ROIX0_Sizer.SetSpacing( 4 );
   ROIX0_Sizer.Add( ROIX0_Label );
   ROIX0_Sizer.Add( ROIX0_SpinBox );
   ROIX0_Sizer.AddStretch();

   const char* roiY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIY0_Label.SetText( "Top:" );
   ROIY0_Label.SetFixedWidth( labelWidth1 );
   ROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIY0_Label.SetToolTip( roiY0ToolTip );

   ROIY0_SpinBox.SetRange( 0, int_max );
   //ROIY0_SpinBox.SetFixedWidth( spinWidth1 );
   ROIY0_SpinBox.SetToolTip( roiY0ToolTip );
   ROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__ROI_SpinValueUpdated, w );

   ROIY0_Sizer.SetSpacing( 4 );
   ROIY0_Sizer.Add( ROIY0_Label );
   ROIY0_Sizer.Add( ROIY0_SpinBox );
   ROIY0_Sizer.AddStretch();

   const char* roiWidthToolTip = "<p>Width of the ROI in pixels.</p>";

   ROIWidth_Label.SetText( "Width:" );
   ROIWidth_Label.SetFixedWidth( labelWidth1 );
   ROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIWidth_Label.SetToolTip( roiWidthToolTip );

   ROIWidth_SpinBox.SetRange( 0, int_max );
   //ROIWidth_SpinBox.SetFixedWidth( spinWidth1 );
   ROIWidth_SpinBox.SetToolTip( roiWidthToolTip );
   ROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__ROI_SpinValueUpdated, w );

   ROIWidth_Sizer.SetSpacing( 4 );
   ROIWidth_Sizer.Add( ROIWidth_Label );
   ROIWidth_Sizer.Add( ROIWidth_SpinBox );
   ROIWidth_Sizer.AddStretch();

   const char* roiHeightToolTip = "<p>Height of the ROI in pixels.</p>";

   ROIHeight_Label.SetText( "Height:" );
   ROIHeight_Label.SetFixedWidth( labelWidth1 );
   ROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIHeight_Label.SetToolTip( roiHeightToolTip );

   ROIHeight_SpinBox.SetRange( 0, int_max );
   //ROIHeight_SpinBox.SetFixedWidth( spinWidth1 );
   ROIHeight_SpinBox.SetToolTip( roiHeightToolTip );
   ROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ImageIntegrationInterface::__ROI_SpinValueUpdated, w );

   SelectPreview_Button.SetText( "From Preview" );
   SelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>" );
   SelectPreview_Button.OnClick( (Button::click_event_handler)&ImageIntegrationInterface::__ROI_Click, w );

   ROIHeight_Sizer.SetSpacing( 4 );
   ROIHeight_Sizer.Add( ROIHeight_Label );
   ROIHeight_Sizer.Add( ROIHeight_SpinBox );
   ROIHeight_Sizer.AddStretch();
   ROIHeight_Sizer.Add( SelectPreview_Button );

   ROI_Sizer.SetSpacing( 4 );
   ROI_Sizer.Add( ROIX0_Sizer );
   ROI_Sizer.Add( ROIY0_Sizer );
   ROI_Sizer.Add( ROIWidth_Sizer );
   ROI_Sizer.Add( ROIHeight_Sizer );

   ROI_Control.SetSizer( ROI_Sizer );
   ROI_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( InputImages_SectionBar );
   Global_Sizer.Add( InputImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( Integration_SectionBar );
   Global_Sizer.Add( Integration_Control );
   Global_Sizer.Add( Rejection1_SectionBar );
   Global_Sizer.Add( Rejection1_Control );
   Global_Sizer.Add( Rejection2_SectionBar );
   Global_Sizer.Add( Rejection2_Control );
   Global_Sizer.Add( Rejection3_SectionBar );
   Global_Sizer.Add( Rejection3_Control );
   Global_Sizer.Add( ROI_SectionBar );
   Global_Sizer.Add( ROI_Control );

   w.SetSizer( Global_Sizer );
   w.SetFixedWidth();

   FormatHints_Control.Hide();
   Rejection1_Control.Hide();
   Rejection2_Control.Hide();
   Rejection3_Control.Hide();
   ROI_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIntegrationInterface.cpp - Released 2015/07/31 11:49:48 UTC
