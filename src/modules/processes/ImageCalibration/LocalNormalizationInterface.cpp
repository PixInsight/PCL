//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0332
// ----------------------------------------------------------------------------
// LocalNormalizationInterface.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "LocalNormalizationInterface.h"
#include "LocalNormalizationProcess.h"
#include "LocalNormalizationParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/MultiViewSelectionDialog.h>
#include <pcl/ViewSelectionDialog.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

LocalNormalizationInterface* TheLocalNormalizationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "LocalNormalizationIcon.xpm"

// ----------------------------------------------------------------------------

LocalNormalizationInterface::LocalNormalizationInterface() :
   m_instance( TheLocalNormalizationProcess )
{
   TheLocalNormalizationInterface = this;
}

// ----------------------------------------------------------------------------

LocalNormalizationInterface::~LocalNormalizationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString LocalNormalizationInterface::Id() const
{
   return "LocalNormalization";
}

// ----------------------------------------------------------------------------

MetaProcess* LocalNormalizationInterface::Process() const
{
   return TheLocalNormalizationProcess;
}

// ----------------------------------------------------------------------------

const char** LocalNormalizationInterface::IconImageXPM() const
{
   return LocalNormalizationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures LocalNormalizationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::ResetInstance()
{
   LocalNormalizationInstance defaultInstance( TheLocalNormalizationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "LocalNormalization" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheLocalNormalizationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* LocalNormalizationInterface::NewProcess() const
{
   return new LocalNormalizationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const LocalNormalizationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a LocalNormalization instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateControls()
{
   UpdateGeneralParameterControls();
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateOutputFilesControls();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateGeneralParameterControls()
{
   GUI->ReferenceImage_Edit.SetText( m_instance.p_referencePathOrViewId );
   GUI->ReferenceImage_ComboBox.SetCurrentItem( m_instance.p_referenceIsView ? 1 : 0 );

   GUI->Scale_SpinBox.SetValue( m_instance.p_scale );

   GUI->Rejection_CheckBox.SetChecked( m_instance.p_rejection );

   GUI->HotPixelFilterRadius_SpinBox.SetValue( m_instance.p_hotPixelFilterRadius );
   GUI->HotPixelFilterRadius_SpinBox.Enable( m_instance.p_rejection );

   GUI->NoiseReductionFilterRadius_SpinBox.SetValue( m_instance.p_noiseReductionFilterRadius );
   GUI->NoiseReductionFilterRadius_SpinBox.Enable( m_instance.p_rejection );

   GUI->BackgroundRejectionLimit_NumericControl.SetValue( m_instance.p_backgroundRejectionLimit );
   GUI->BackgroundRejectionLimit_NumericControl.Enable( m_instance.p_rejection );

   GUI->ReferenceRejectionThreshold_NumericControl.SetValue( m_instance.p_referenceRejectionThreshold );
   GUI->ReferenceRejectionThreshold_NumericControl.Enable( m_instance.p_rejection );

   GUI->TargetRejectionThreshold_NumericControl.SetValue( m_instance.p_targetRejectionThreshold );
   GUI->TargetRejectionThreshold_NumericControl.Enable( m_instance.p_rejection );

   GUI->GenerateNormalizedImages_ComboBox.SetCurrentItem( m_instance.p_generateNormalizedImages );

   GUI->GenerateNormalizationData_CheckBox.SetChecked( m_instance.p_generateNormalizationData );

   GUI->ShowBackgroundModels_CheckBox.SetChecked( m_instance.p_showBackgroundModels );

   GUI->ShowRejectionMaps_CheckBox.SetChecked( m_instance.p_showRejectionMaps );
   GUI->ShowRejectionMaps_CheckBox.Enable( m_instance.p_rejection );

   GUI->PlotNormalizationFunctions_ComboBox.SetCurrentItem( m_instance.p_plotNormalizationFunctions );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const LocalNormalizationInstance::Item& item = m_instance.p_targets[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   if ( !GUI->FullPaths_CheckBox.IsChecked() )
      node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   else
      node->SetText( 2, item.path );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateTargetImagesList()
{
   int currentIdx = GUI->TargetImages_TreeBox.ChildIndex( GUI->TargetImages_TreeBox.CurrentNode() );

   GUI->TargetImages_TreeBox.DisableUpdates();
   GUI->TargetImages_TreeBox.Clear();

   for ( size_type i = 0; i < m_instance.p_targets.Length(); ++i )
   {
      new TreeBox::Node( GUI->TargetImages_TreeBox );
      UpdateTargetImageItem( i );
   }

   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !m_instance.p_targets.IsEmpty() )
      if ( currentIdx >= 0 )
         if ( currentIdx < GUI->TargetImages_TreeBox.NumberOfChildren() )
            GUI->TargetImages_TreeBox.SetCurrentNode( GUI->TargetImages_TreeBox[currentIdx] );

   GUI->TargetImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateImageSelectionButtons()
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

void LocalNormalizationInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory );
   GUI->OutputPrefix_Edit.SetText( m_instance.p_outputPrefix );
   GUI->OutputPostfix_Edit.SetText( m_instance.p_outputPostfix );
   GUI->OverwriteExistingFiles_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );
   GUI->OnError_ComboBox.SetCurrentItem( m_instance.p_onError );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_EditCompleted( Edit& sender )
{
   try
   {
      String text = sender.Text().Trimmed();

      if ( sender == GUI->ReferenceImage_Edit )
      {
         if ( !text.IsEmpty() )
            if ( m_instance.p_referenceIsView )
               if ( !View::IsValidViewId( text ) )
               {
                  sender.SetText( m_instance.p_referencePathOrViewId );
                  throw Error( "Invalid view identifier '" + text + '\'' );
               }

         m_instance.p_referencePathOrViewId = text;
      }
      else if ( sender == GUI->InputHints_Edit )
      {
         m_instance.p_inputHints = text;
      }
      else if ( sender == GUI->OutputHints_Edit )
      {
         m_instance.p_outputHints = text;
      }
      else if ( sender == GUI->OutputDirectory_Edit )
      {
         m_instance.p_outputDirectory = text;
      }
      else if ( sender == GUI->OutputPrefix_Edit )
      {
         m_instance.p_outputPrefix = text;
      }
      else if ( sender == GUI->OutputPostfix_Edit )
      {
         m_instance.p_outputPostfix = text;
      }

      sender.SetText( text );
   }
   ERROR_CLEANUP(
      sender.SelectAll();
      sender.Focus()
   )
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ReferenceImage_ComboBox )
   {
      bool isView = itemIndex > 0;
      if ( isView != bool( m_instance.p_referenceIsView ) ) // VC++ needs an explicit cast to bool. Argh!
      {
         m_instance.p_referenceIsView = isView;
         GUI->ReferenceImage_Edit.SetText( m_instance.p_referencePathOrViewId = String() );
      }
   }
   else if ( sender == GUI->GenerateNormalizedImages_ComboBox )
   {
      m_instance.p_generateNormalizedImages = itemIndex;
   }
   else if ( sender == GUI->PlotNormalizationFunctions_ComboBox )
   {
      m_instance.p_plotNormalizationFunctions = itemIndex;
   }
   else if ( sender == GUI->OnError_ComboBox )
   {
      m_instance.p_onError = itemIndex;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ReferenceImage_ToolButton )
   {
      if ( m_instance.p_referenceIsView )
      {
         ViewSelectionDialog d( m_instance.p_referencePathOrViewId );
         d.SetWindowTitle( "LocalNormalization: Select Reference Image" );
         if ( d.Execute() != StdDialogCode::Ok )
            return;
         m_instance.p_referencePathOrViewId = d.Id();
      }
      else
      {
         OpenFileDialog d;
         d.SetCaption( "LocalNormalization: Select Reference Image" );
         d.LoadImageFilters();
         d.DisableMultipleSelections();
         if ( !d.Execute() )
            return;
         m_instance.p_referencePathOrViewId = d.FileName();
      }

      GUI->ReferenceImage_Edit.SetText( m_instance.p_referencePathOrViewId );
   }
   else if ( sender == GUI->Rejection_CheckBox )
   {
      m_instance.p_rejection = checked;
      UpdateGeneralParameterControls();
   }
   else if ( sender == GUI->GenerateNormalizationData_CheckBox )
   {
      m_instance.p_generateNormalizationData = checked;
   }
   else if ( sender == GUI->ShowBackgroundModels_CheckBox )
   {
      m_instance.p_showBackgroundModels = checked;
   }
   else if ( sender == GUI->ShowRejectionMaps_CheckBox )
   {
      m_instance.p_showRejectionMaps = checked;
   }
   else if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "LocalNormalization: Select Target Images" );
      d.LoadImageFilters();
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( const String& fileName : d.FileNames() )
            m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), LocalNormalizationInstance::Item( fileName ) );

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
            m_instance.p_targets[i].enabled = !m_instance.p_targets[i].enabled;

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      LocalNormalizationInstance::item_list newTargets;
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->TargetImages_TreeBox[i]->IsSelected() )
            newTargets.Add( m_instance.p_targets[i] );
      m_instance.p_targets = newTargets;

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      m_instance.p_targets.Clear();
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "LocalNormalization: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
   {
      m_instance.p_overwriteExistingFiles = checked;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->BackgroundRejectionLimit_NumericControl )
   {
      m_instance.p_backgroundRejectionLimit = value;
   }
   else if ( sender == GUI->ReferenceRejectionThreshold_NumericControl )
   {
      m_instance.p_referenceRejectionThreshold = value;
   }
   else if ( sender == GUI->TargetRejectionThreshold_NumericControl )
   {
      m_instance.p_targetRejectionThreshold = value;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "LocalNormalizationInterface: *Warning* Corrupted interface structures" );

   // ### If there's something that depends on which image is selected in the list, do it now.
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "LocalNormalizationInterface: *Warning* Corrupted interface structures" );

   LocalNormalizationInstance::Item& item = m_instance.p_targets[index];

   switch ( col )
   {
   case 0:
      break;
   case 1:
      item.enabled = !item.enabled;
      UpdateTargetImageItem( index );
      break;
   case 2:
      {
         Array<ImageWindow> windows = ImageWindow::Open( item.path, IsoString()/*id*/, m_instance.p_inputHints );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Scale_SpinBox )
      m_instance.p_scale = value;
   else if ( sender == GUI->HotPixelFilterRadius_SpinBox )
      m_instance.p_hotPixelFilterRadius = value;
   else if ( sender == GUI->NoiseReductionFilterRadius_SpinBox )
      m_instance.p_noiseReductionFilterRadius = value;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
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

void LocalNormalizationInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->ReferenceImage_Edit )
      wantsFiles = files.Length() == 1 && FileFormat::IsSupportedFileFormatBySuffix( files[0], true/*toRead*/ ) && File::Exists( files[0] );
   else if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->ReferenceImage_Edit )
   {
      if ( FileFormat::IsSupportedFileFormatBySuffix( files[0], true/*toRead*/ ) )
         if ( File::Exists( files[0] ) )
         {
            m_instance.p_referenceIsView = false;
            m_instance.p_referencePathOrViewId = files[0];
            UpdateGeneralParameterControls();
         }
   }
   else if ( sender == GUI->TargetImages_TreeBox.Viewport() )
   {
      StringList inputFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
         if ( File::Exists( item ) )
            inputFiles << item;
         else if ( File::DirectoryExists( item ) )
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );
      size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );

      inputFiles.Sort();
      for ( const String& file : inputFiles )
         m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), LocalNormalizationInstance::Item( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = files[0] );
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ReferenceImage_Edit )
      wantsView = true;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ReferenceImage_Edit )
   {
      m_instance.p_referenceIsView = true;
      m_instance.p_referencePathOrViewId = view.FullId();
      UpdateGeneralParameterControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LocalNormalizationInterface::GUIData::GUIData( LocalNormalizationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Reference threshold:" ) + 'M' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );
   int editWidth2 = fnt.Width( String( 'M', 5 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   const char* referenceToolTip = "<p>Normalization reference image.</p>"
      "<p>Local normalization functions will be calculated for each target image with respect to the reference image, "
      "which will be acquired either from an image view, or from an existing disk file.</p>"
      "<p><b>Important:</b> For execution in the global context (batch procedure), if the reference image refers to a disk"
      "file that will be part of an image integration task, it must also be included in the target images list. Otherwise no "
      ".xnml file and/or normalized image will be generated for the reference image.</p>";

   ReferenceImage_Label.SetText( "Reference image:" );
   ReferenceImage_Label.SetFixedWidth( labelWidth1 );
   ReferenceImage_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ReferenceImage_Label.SetToolTip( referenceToolTip );

   ReferenceImage_Edit.SetToolTip( referenceToolTip );
   ReferenceImage_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );
   ReferenceImage_Edit.OnFileDrag( (Control::file_drag_event_handler)&LocalNormalizationInterface::e_FileDrag, w );
   ReferenceImage_Edit.OnFileDrop( (Control::file_drop_event_handler)&LocalNormalizationInterface::e_FileDrop, w );
   ReferenceImage_Edit.OnViewDrag( (Control::view_drag_event_handler)&LocalNormalizationInterface::e_ViewDrag, w );
   ReferenceImage_Edit.OnViewDrop( (Control::view_drop_event_handler)&LocalNormalizationInterface::e_ViewDrop, w );

   ReferenceImage_ComboBox.AddItem( "File" );
   ReferenceImage_ComboBox.AddItem( "View" );
   ReferenceImage_ComboBox.SetToolTip( "<p>Select what the reference image refers to: Either a disk file or a view.</p>" );
   ReferenceImage_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   ReferenceImage_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select.png" ) ) );
   ReferenceImage_ToolButton.SetScaledFixedSize( 20, 20 );
   ReferenceImage_ToolButton.SetToolTip( "<p>Select the reference image</p>" );
   ReferenceImage_ToolButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ReferenceImage_Sizer.SetSpacing( 4 );
   ReferenceImage_Sizer.Add( ReferenceImage_Label );
   ReferenceImage_Sizer.Add( ReferenceImage_Edit, 100 );
   ReferenceImage_Sizer.Add( ReferenceImage_ComboBox );
   ReferenceImage_Sizer.Add( ReferenceImage_ToolButton );

   //

   const char* scaleTip = "<p>Normalization scale.</p>"
      "<p>LocalNormalization implements a multiscale normalization algorithm. This parameter is the size in pixels of the "
      "sampling scale for local image normalization. The larger this parameter, the less locally adaptive will be the local "
      "normalization task. Smaller values tend to reproduce variations among small-scale structures in the reference image. "
      "Larger values tend to reproduce variations among large-scale structures.</p>"
      "<p>To better understand the role of this parameter, suppose we applied the algorithm at the scale of one pixel. The "
      "result would be an exact copy of the reference image. On the other hand, if we applied the algorithm at a scale similar "
      "to the size of the whole image, the result would be a <i>global normalization</i>: a single linear function would be "
      "applied for normalization of the target image.</p>"
      "<p>The default scale is 128 pixels, which is quite appropriate for most digital images. Suitable scales are generally "
      "in the range from 64 to 256 pixels. Although the value of this parameter can be freely set in the range from 32 to "
      "65536 pixels, the current implementation is optimized for multiples of 32 pixels: 32, 64, 96, 128, 160, 192, etc.</p>";

   Scale_Label.SetText( "Scale:" );
   Scale_Label.SetFixedWidth( labelWidth1 );
   Scale_Label.SetToolTip( scaleTip );
   Scale_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Scale_SpinBox.SetRange( int( TheLNScaleParameter->MinimumValue() ), int( TheLNScaleParameter->MaximumValue() ) );
   Scale_SpinBox.SetStepSize( 32 );
   Scale_SpinBox.SetMinWidth( editWidth1 );
   Scale_SpinBox.SetToolTip( scaleTip );
   Scale_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( Scale_Label );
   Scale_Sizer.Add( Scale_SpinBox );
   Scale_Sizer.AddStretch();

   //

   Rejection_CheckBox.SetText( "Outlier rejection" );
   Rejection_CheckBox.SetToolTip( "<p>The implemented local normalization algorithms are sensitive to differences between the "
      "reference and target images caused by spurious image features, such as hot pixels, plane and satellite trails, cosmic rays, etc.</p>"
      "<p>Under normal working conditions, this option should be enabled to detect and reject outlier image structures prior to "
      "calculation of local normalization functions.</p>" );
   Rejection_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   Rejection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Rejection_Sizer.Add( Rejection_CheckBox );
   Rejection_Sizer.AddStretch();

   //

   const char* hotPixelFilterRadiusToolTip =
   "<p>Size of the hot pixel removal filter.</p>"
   "<p>This is the radius in pixels of a median filter applied for robust outlier rejection. A median filter is very efficient "
   "to remove hot pixels, cold pixels, and similar small-scale outlier image structures that can contaminate the local "
   "normalization functions, degrading their accuracy, or even introducing artifacts in extreme cases.</p>"
   "<p>To disable hot pixel removal (not recommended), set this parameter to zero. The default value is two pixels.</p>";

   HotPixelFilterRadius_Label.SetText( "Hot pixel removal:" );
   HotPixelFilterRadius_Label.SetFixedWidth( labelWidth1 );
   HotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   HotPixelFilterRadius_Label.SetToolTip( hotPixelFilterRadiusToolTip );

   HotPixelFilterRadius_SpinBox.SetRange( int( TheLNHotPixelFilterRadiusParameter->MinimumValue() ), int( TheLNHotPixelFilterRadiusParameter->MaximumValue() ) );
   HotPixelFilterRadius_SpinBox.SetToolTip( hotPixelFilterRadiusToolTip );
   HotPixelFilterRadius_SpinBox.SetFixedWidth( editWidth2 );
   HotPixelFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   HotPixelFilterRadius_Sizer.SetSpacing( 4 );
   HotPixelFilterRadius_Sizer.Add( HotPixelFilterRadius_Label );
   HotPixelFilterRadius_Sizer.Add( HotPixelFilterRadius_SpinBox );
   HotPixelFilterRadius_Sizer.AddStretch();

   //

   const char* noiseReductionFilterRadiusToolTip =
   "<p>Size of the noise reduction filter.</p>"
   "<p>This is the radius in pixels of a Gaussian convolution filter applied for noise reduction. Noise reduction is disabled "
   "by default; it is only necessary for images suffering from severe noise, especially images with clipped histograms and similar "
   "artifacts. Increasing this parameter should not be necessary for well calibrated images.</p>";

   NoiseReductionFilterRadius_Label.SetText( "Noise reduction:" );
   NoiseReductionFilterRadius_Label.SetFixedWidth( labelWidth1 );
   NoiseReductionFilterRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NoiseReductionFilterRadius_Label.SetToolTip( noiseReductionFilterRadiusToolTip );

   NoiseReductionFilterRadius_SpinBox.SetRange( int( TheLNNoiseReductionFilterRadiusParameter->MinimumValue() ), int( TheLNNoiseReductionFilterRadiusParameter->MaximumValue() ) );
   NoiseReductionFilterRadius_SpinBox.SetStepSize( 5 );
   NoiseReductionFilterRadius_SpinBox.SetToolTip( noiseReductionFilterRadiusToolTip );
   NoiseReductionFilterRadius_SpinBox.SetFixedWidth( editWidth2 );
   NoiseReductionFilterRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::e_SpinValueUpdated, w );

   NoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   NoiseReductionFilterRadius_Sizer.Add( NoiseReductionFilterRadius_Label );
   NoiseReductionFilterRadius_Sizer.Add( NoiseReductionFilterRadius_SpinBox );
   NoiseReductionFilterRadius_Sizer.AddStretch();

   //

   BackgroundRejectionLimit_NumericControl.label.SetText( "Background limit:" );
   BackgroundRejectionLimit_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundRejectionLimit_NumericControl.slider.SetRange( 1, 500 );
   BackgroundRejectionLimit_NumericControl.slider.SetScaledMinWidth( 250 );
   BackgroundRejectionLimit_NumericControl.SetReal();
   BackgroundRejectionLimit_NumericControl.SetRange( TheLNBackgroundRejectionLimitParameter->MinimumValue(), TheLNBackgroundRejectionLimitParameter->MaximumValue() );
   BackgroundRejectionLimit_NumericControl.SetPrecision( TheLNBackgroundRejectionLimitParameter->Precision() );
   BackgroundRejectionLimit_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundRejectionLimit_NumericControl.SetToolTip( "<p>LocalNormalization computes an initial, approximate model of the "
      "<i>local background</i> for the reference and target images. This model is used, among other tasks, for classification "
      "of small-scale image structures to detect potential outliers.</p>"
      "<p>This parameter is the minimum relative difference with respect to the approximate local background, in absolute value, "
      "for a structure to be considered as significant.</p>"
      "<p>Increase this parameter to force a stronger outlier rejection. Decrease it to relax the outlier detection algorithm. "
      "The default value of 0.05 is a good option in most cases. For difficult cases where the images are very noisy and/or "
      "there are large outliers such as plane trails, you can use the <i>show rejection maps</i> option to evaluate outlier "
      "rejection with different values of normalization rejection parameters.</p>" );
   BackgroundRejectionLimit_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_EditValueUpdated, w );

   //

   ReferenceRejectionThreshold_NumericControl.label.SetText( "Reference threshold:" );
   ReferenceRejectionThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   ReferenceRejectionThreshold_NumericControl.slider.SetRange( 1, 500 );
   ReferenceRejectionThreshold_NumericControl.slider.SetScaledMinWidth( 250 );
   ReferenceRejectionThreshold_NumericControl.SetReal();
   ReferenceRejectionThreshold_NumericControl.SetRange( TheLNReferenceRejectionThresholdParameter->MinimumValue(), TheLNReferenceRejectionThresholdParameter->MaximumValue() );
   ReferenceRejectionThreshold_NumericControl.SetPrecision( TheLNReferenceRejectionThresholdParameter->Precision() );
   ReferenceRejectionThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   ReferenceRejectionThreshold_NumericControl.SetToolTip( "<p>This is the minimum relative difference with respect to the local "
      "background, in absolute value, for a structure to be classified as a potential outlier in the reference image. See the "
      "<i>background limit</i> parameter for more information on the local background and its role for local normalization.</p>"
      "<p>Increase this parameter to relax the outlier rejection algorithm with respect to the reference image. Decrease it to "
      "strengthen outlier rejection. The default value is 0.5.</p>" );
   ReferenceRejectionThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_EditValueUpdated, w );

   //

   TargetRejectionThreshold_NumericControl.label.SetText( "Target threshold:" );
   TargetRejectionThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   TargetRejectionThreshold_NumericControl.slider.SetRange( 1, 500 );
   TargetRejectionThreshold_NumericControl.slider.SetScaledMinWidth( 250 );
   TargetRejectionThreshold_NumericControl.SetReal();
   TargetRejectionThreshold_NumericControl.SetRange( TheLNTargetRejectionThresholdParameter->MinimumValue(), TheLNTargetRejectionThresholdParameter->MaximumValue() );
   TargetRejectionThreshold_NumericControl.SetPrecision( TheLNTargetRejectionThresholdParameter->Precision() );
   TargetRejectionThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   TargetRejectionThreshold_NumericControl.SetToolTip( "<p>This is the minimum relative difference with respect to the local "
      "background, in absolute value, for a structure to be considered as a potential outlier in the target image. See the "
      "<i>reference threshold</i> and <i>background limit</i> parameters for more information on outlier rejection and its role "
      "for local normalization.</p>" );
   TargetRejectionThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalNormalizationInterface::e_EditValueUpdated, w );

   //

   const char* generateNormalizedImagesToolTip = "<p>This parameter defines when to apply the local normalization functions to "
      "generate normalized images. This includes newly created image files for global execution, as well as modifying the target "
      "image for view execution.</p>"
      "<p>This option is set to <i>view execution only</i> by default because local normalization functions, stored in .xnml files, "
      "can be used by the ImageIntegration and DrizzleIntegration processes for normalization in the pixel rejection and/or "
      "integration output tasks. This means that the normalization functions can be applied internally by these processes, so "
      "writing normalized images to disk files is generally not necessary. For view execution, however, inspecting the transformed "
      "image is often necessary for testing purposes (although graphs can be even more informative).</p>"
      "<p>Generation of normalized image files during batch execution can also be useful in difficult cases, for example when the "
      "data set includes strong light pollution gradients and large artifacts, such as plane trails. In these cases you may want "
      "to inspect locally normalized images manually with analysis tools such as Blink. Normalized images may also be necessary for "
      "applications of local normalization different from integration of astronomical images.</p>";

   GenerateNormalizedImages_Label.SetText( "Apply normalization:" );
   GenerateNormalizedImages_Label.SetFixedWidth( labelWidth1 );
   GenerateNormalizedImages_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GenerateNormalizedImages_Label.SetToolTip( generateNormalizedImagesToolTip );

   GenerateNormalizedImages_ComboBox.AddItem( "Disabled" );
   GenerateNormalizedImages_ComboBox.AddItem( "Always" );
   GenerateNormalizedImages_ComboBox.AddItem( "View execution only" );
   GenerateNormalizedImages_ComboBox.AddItem( "Global execution only" );
   GenerateNormalizedImages_ComboBox.SetToolTip( generateNormalizedImagesToolTip );
   GenerateNormalizedImages_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   GenerateNormalizedImages_Sizer.SetSpacing( 4 );
   GenerateNormalizedImages_Sizer.Add( GenerateNormalizedImages_Label );
   GenerateNormalizedImages_Sizer.Add( GenerateNormalizedImages_ComboBox );
   GenerateNormalizedImages_Sizer.AddStretch();

   //

   GenerateNormalizationData_CheckBox.SetText( "Generate normalization data" );
   GenerateNormalizationData_CheckBox.SetToolTip( "<p>Generate XML normalization data files (XNML format, .xnml files) that can be "
      "used with the ImageIntegration and DrizzleIntegration processes. XNML files store normalization parameters, image references "
      "and function matrices, that can be used by these processes to generate normalized pixel sample values required for pixel "
      "rejection and integration output. For applications of local normalization to integration of astronomical images, this option "
      "should always be enabled.</p>" );
   GenerateNormalizationData_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   GenerateNormalizationData_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateNormalizationData_Sizer.Add( GenerateNormalizationData_CheckBox );
   GenerateNormalizationData_Sizer.AddStretch();

   //

   ShowBackgroundModels_CheckBox.SetText( "Show background models" );
   ShowBackgroundModels_CheckBox.SetToolTip( "<p>Generate floating point images with the computed local background models for the "
      "reference and target images. This option is only applicable to execution on views, and is always ignored when LocalNormalization "
      "is executed in the global context (that is, as a batch procedure).</p>"
      "<p>Analysis of local backgrounds can be important for testing purposes, especially in difficult cases, for example to know how "
      "complex gradients are being evaluated, or to judge the quality of outlier rejection.</p>" );
   ShowBackgroundModels_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ShowBackgroundModels_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowBackgroundModels_Sizer.Add( ShowBackgroundModels_CheckBox );
   ShowBackgroundModels_Sizer.AddStretch();

   //

   ShowRejectionMaps_CheckBox.SetText( "Show rejection maps" );
   ShowRejectionMaps_CheckBox.SetToolTip( "<p>Generate unsigned 8-bit images where nonzero pixel samples correspond to rejected "
      "samples in the reference and target images. This option is only applicable to execution on views, and is always ignored when "
      "LocalNormalization is executed in the global context (that is, as a batch procedure).</p>"
      "<p>Rejection maps are essential for testing purposes, especially in difficult cases where the images have large outliers, such "
      "as plane and satellite trails, and/or complex gradients.</p>" );
   ShowRejectionMaps_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ShowRejectionMaps_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowRejectionMaps_Sizer.Add( ShowRejectionMaps_CheckBox );
   ShowRejectionMaps_Sizer.AddStretch();

   //

   const char* plotNormalizationFunctionsToolTip = "<p>Generate graphical representations of the local normalization function matrices. "
      "This option is only applicable to execution on views; it is always ignored when LocalNormalization is executed in the global context "
      "(that is, as a batch procedure).</p>"
      "<p>When this option is enabled, two images are generated for each image channel with graphs representing the parameters of the "
      "two-dimensional surface linear normalization function: one for the scaling coefficients (or slopes), and another for the zero offsets "
      "(or Y-intercepts). These graphs, besides being beautiful, can be outstandingly useful for evaluation and testing purposes.</p>"
      "<p>The <i>lines 3D</i> mode is the fastest option, but also the less accurate because of potential artifacts caused by crossing grid "
      "lines. The <i>palette 3D</i> and <i>map 3D</i> modes generate high-quality, smooth and accurate renditions, but require more computation "
      "time. The default option is <i>palette 3D</i>.</p>";

   PlotNormalizationFunctions_Label.SetText( "Plot functions:" );
   PlotNormalizationFunctions_Label.SetFixedWidth( labelWidth1 );
   PlotNormalizationFunctions_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PlotNormalizationFunctions_Label.SetToolTip( plotNormalizationFunctionsToolTip );

   PlotNormalizationFunctions_ComboBox.AddItem( "Disabled" );
   PlotNormalizationFunctions_ComboBox.AddItem( "Lines 3D" );
   PlotNormalizationFunctions_ComboBox.AddItem( "Palette 3D" );
   PlotNormalizationFunctions_ComboBox.AddItem( "Map 3D" );
   PlotNormalizationFunctions_ComboBox.SetToolTip( plotNormalizationFunctionsToolTip );
   PlotNormalizationFunctions_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   PlotNormalizationFunctions_Sizer.SetSpacing( 4 );
   PlotNormalizationFunctions_Sizer.Add( PlotNormalizationFunctions_Label );
   PlotNormalizationFunctions_Sizer.Add( PlotNormalizationFunctions_ComboBox );
   PlotNormalizationFunctions_Sizer.AddStretch();

   //

   GeneralParameters_Sizer.SetSpacing( 4 );
   GeneralParameters_Sizer.Add( ReferenceImage_Sizer );
   GeneralParameters_Sizer.Add( Scale_Sizer );
   GeneralParameters_Sizer.Add( Rejection_Sizer );
   GeneralParameters_Sizer.Add( HotPixelFilterRadius_Sizer );
   GeneralParameters_Sizer.Add( NoiseReductionFilterRadius_Sizer );
   GeneralParameters_Sizer.Add( BackgroundRejectionLimit_NumericControl );
   GeneralParameters_Sizer.Add( ReferenceRejectionThreshold_NumericControl );
   GeneralParameters_Sizer.Add( TargetRejectionThreshold_NumericControl );
   GeneralParameters_Sizer.Add( GenerateNormalizedImages_Sizer );
   GeneralParameters_Sizer.Add( GenerateNormalizationData_Sizer );
   GeneralParameters_Sizer.Add( ShowBackgroundModels_Sizer );
   GeneralParameters_Sizer.Add( ShowRejectionMaps_Sizer );
   GeneralParameters_Sizer.Add( PlotNormalizationFunctions_Sizer );

   //

   TargetImages_SectionBar.SetTitle( "Target Images" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&LocalNormalizationInterface::e_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&LocalNormalizationInterface::e_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&LocalNormalizationInterface::e_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&LocalNormalizationInterface::e_FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&LocalNormalizationInterface::e_FileDrop, w );
   TargetImages_TreeBox.Viewport().OnViewDrag( (Control::view_drag_event_handler)&LocalNormalizationInterface::e_ViewDrag, w );
   TargetImages_TreeBox.Viewport().OnViewDrop( (Control::view_drop_event_handler)&LocalNormalizationInterface::e_ViewDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target images.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target images.</p>"
      "<p>Disabled target images will be ignored during the registration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target file items.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

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

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings for image files used "
      "by specific processes. In LocalNormalization, input hints change the way input images of some particular file formats "
      "(reference and target image files) are loaded, while output hints modify the way output normalized image files are written.</p>"
      "<p>For example, you can use the \"lower-range\" and \"upper-range\" input hints to load floating point FITS and TIFF files "
      "generated by other applications that don't use PixInsight's normalized [0,1] range. Similarly, you can specify the \"up-bottom\" "
      "output hint to write all normalized images (in FITS format) with the coordinate origin at the top-left corner. Most standard "
      "file format modules support hints; each format supports a number of input and/or output hints that you can use for different "
      "purposes with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

   OutputHints_Sizer.SetSpacing( 4 );
   OutputHints_Sizer.Add( OutputHints_Label );
   OutputHints_Sizer.Add( OutputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );
   FormatHints_Sizer.Add( OutputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );

   //

   OutputFiles_SectionBar.SetTitle( "Output Files" );
   OutputFiles_SectionBar.SetSection( OutputFiles_Control );
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&LocalNormalizationInterface::e_ToggleSection, w );

   const char* outputDirectoryToolTip = "<p>This is the directory (or folder) where all output files will be written. "
      "This includes .xnml data files and, if the corresponding option has been enabled, normalized image files.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their  corresponding "
      "target files. In this case, make sure that source directories are writable, or the normalization process will fail.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&LocalNormalizationInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&LocalNormalizationInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   const char* outputPrefixToolTip = "<p>This is a prefix that will be prepended to the file name of each normalized image.</p>";

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( labelWidth1 );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPrefix_Label.SetToolTip( outputPrefixToolTip );

   OutputPrefix_Edit.SetFixedWidth( editWidth2 );
   OutputPrefix_Edit.SetToolTip( outputPrefixToolTip );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

   const char* outputPostfixToolTip = "<p>This is a postfix that will be appended to the file name of each normalized image.</p>";

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPostfix_Label.SetToolTip( outputPostfixToolTip );

   OutputPostfix_Edit.SetFixedWidth( editWidth2 );
   OutputPostfix_Edit.SetToolTip( outputPostfixToolTip );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::e_EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( "<p>If this option is selected, LocalNormalization will overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::e_Click, w );

   const char* onErrorToolTip = "<p>Specify what to do if there are errors during a batch local normalization process.</p>";

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetToolTip( onErrorToolTip );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OnError_ComboBox.AddItem( "Continue" );
   OnError_ComboBox.AddItem( "Abort" );
   OnError_ComboBox.AddItem( "Ask User" );
   OnError_ComboBox.SetToolTip( onErrorToolTip );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalNormalizationInterface::e_ItemSelected, w );

   OutputOptions_Sizer.SetSpacing( 4 );
   OutputOptions_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputOptions_Sizer.Add( OverwriteExistingFiles_CheckBox );
   OutputOptions_Sizer.AddSpacing( 16 );
   OutputOptions_Sizer.Add( OnError_Label );
   OutputOptions_Sizer.Add( OnError_ComboBox );
   OutputOptions_Sizer.AddStretch();

   OutputFiles_Sizer.SetSpacing( 4 );
   OutputFiles_Sizer.Add( OutputDirectory_Sizer );
   OutputFiles_Sizer.Add( OutputChunks_Sizer );
   OutputFiles_Sizer.Add( OutputOptions_Sizer );

   OutputFiles_Control.SetSizer( OutputFiles_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( GeneralParameters_Sizer );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( OutputFiles_SectionBar );
   Global_Sizer.Add( OutputFiles_Control );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedWidth();

   FormatHints_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInterface.cpp - Released 2017-08-01T14:26:58Z
