//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.07.00.0308
// ----------------------------------------------------------------------------
// DebayerInterface.cpp - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#include "DebayerInterface.h"
#include "DebayerProcess.h"
#include "DebayerParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/Settings.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

// Settings keys
#define KEY_BAYERPATTERN   "BayerPattern"
#define KEY_DEBAYERMETHOD  "DebayerMethod"
#define KEY_EVALUATENOISE  "DebayerEvaluateNoise"
#define KEY_NOISEEVALALG   "DebayerNoiseEvaluationAlgorithm"

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerInterface* TheDebayerInterface = nullptr;

// ----------------------------------------------------------------------------

#include "DebayerIcon.xpm"

// ----------------------------------------------------------------------------

DebayerInterface::DebayerInterface() :
   m_instance( TheDebayerProcess )
{
   TheDebayerInterface = this;
}

// ----------------------------------------------------------------------------

DebayerInterface::~DebayerInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString DebayerInterface::Id() const
{
   return "Debayer";
}

// ----------------------------------------------------------------------------

MetaProcess* DebayerInterface::Process() const
{
   return TheDebayerProcess;
}

// ----------------------------------------------------------------------------

const char** DebayerInterface::IconImageXPM() const
{
   return Pixmap;
}

// ----------------------------------------------------------------------------

InterfaceFeatures DebayerInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton;
}

// ----------------------------------------------------------------------------

void DebayerInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void DebayerInterface::ResetInstance()
{
   DebayerInstance defaultInstance( TheDebayerProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool DebayerInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Debayer" );
      LoadSettings();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheDebayerProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* DebayerInterface::NewProcess() const
{
   return new DebayerInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool DebayerInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const DebayerInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Debayer instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool DebayerInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool DebayerInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DebayerInterface::UpdateControls()
{
   UpdateGeneralParameterControls();
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateFormatHintsControls();
   UpdateOutputFilesControls();
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateGeneralParameterControls()
{
   GUI->Pattern_ComboBox.SetCurrentItem( m_instance.p_bayerPattern );

   GUI->DebayerMethod_ComboBox.SetCurrentItem( m_instance.p_debayerMethod );

   GUI->EvaluateNoise_CheckBox.SetChecked( m_instance.p_evaluateNoise );

   GUI->NoiseEvaluation_Label.Enable( m_instance.p_evaluateNoise );

   GUI->NoiseEvaluation_ComboBox.SetCurrentItem( m_instance.p_noiseEvaluationAlgorithm );
   GUI->NoiseEvaluation_ComboBox.Enable( m_instance.p_evaluateNoise );
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const DebayerInstance::Item& item = m_instance.p_targets[i];

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

void DebayerInterface::UpdateTargetImagesList()
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

void DebayerInterface::UpdateImageSelectionButtons()
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

void DebayerInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
}

// ----------------------------------------------------------------------------

void DebayerInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory );
   GUI->OutputPrefix_Edit.SetText( m_instance.p_outputPrefix );
   GUI->OutputPostfix_Edit.SetText( m_instance.p_outputPostfix );
   GUI->OverwriteExistingFiles_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );
   GUI->OnError_ComboBox.SetCurrentItem( m_instance.p_onError );
}

// ----------------------------------------------------------------------------

void DebayerInterface::SaveSettings()
{
   Settings::Write ( KEY_BAYERPATTERN,  m_instance.p_bayerPattern );
   Settings::Write ( KEY_DEBAYERMETHOD, m_instance.p_debayerMethod );
   Settings::WriteI( KEY_EVALUATENOISE, m_instance.p_evaluateNoise );
   Settings::Write ( KEY_NOISEEVALALG,  m_instance.p_noiseEvaluationAlgorithm );
}

// ----------------------------------------------------------------------------

void DebayerInterface::LoadSettings()
{
   Settings::Read ( KEY_BAYERPATTERN,   m_instance.p_bayerPattern );
   Settings::Read ( KEY_DEBAYERMETHOD,  m_instance.p_debayerMethod );
   Settings::ReadI( KEY_EVALUATENOISE,  m_instance.p_evaluateNoise );
   Settings::Read ( KEY_NOISEEVALALG,   m_instance.p_noiseEvaluationAlgorithm );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_EditCompleted( Edit& sender )
{
   try
   {
      String text = sender.Text().Trimmed();

      if ( sender == GUI->InputHints_Edit )
         m_instance.p_inputHints = text;
      else if ( sender == GUI->OutputHints_Edit )
         m_instance.p_outputHints = text;
      else if ( sender == GUI->OutputDirectory_Edit )
         m_instance.p_outputDirectory = text;
      else if ( sender == GUI->OutputPrefix_Edit )
         m_instance.p_outputPrefix = text;
      else if ( sender == GUI->OutputPostfix_Edit )
         m_instance.p_outputPostfix = text;

      sender.SetText( text );
   }
   ERROR_CLEANUP(
      sender.SelectAll();
      sender.Focus()
   )
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Pattern_ComboBox )
      m_instance.p_bayerPattern = itemIndex;
   else if ( sender == GUI->NoiseEvaluation_ComboBox )
      m_instance.p_noiseEvaluationAlgorithm = itemIndex;
   else if ( sender == GUI->DebayerMethod_ComboBox )
      m_instance.p_debayerMethod = itemIndex;
   else if ( sender == GUI->OnError_ComboBox )
      m_instance.p_onError = itemIndex;
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->EvaluateNoise_CheckBox )
   {
      m_instance.p_evaluateNoise = checked;
      UpdateGeneralParameterControls();
   }
   else if ( sender == GUI->Save_PushButton )
   {
      SaveSettings();
   }
   else if ( sender == GUI->Restore_PushButton )
   {
      LoadSettings();
      UpdateGeneralParameterControls();
   }
   else if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "Debayer: Select Target Images" );
      d.LoadImageFilters();
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( const String& fileName : d.FileNames() )
            m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), DebayerInstance::Item( fileName ) );

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
      DebayerInstance::item_list newTargets;
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
      d.SetCaption( "Debayer: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
   {
      m_instance.p_overwriteExistingFiles = checked;
   }
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "DebayerInterface: *Warning* Corrupted interface structures" );

   // ### If there's something that depends on which image is selected in the list, do it now.
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "DebayerInterface: *Warning* Corrupted interface structures" );

   DebayerInstance::Item& item = m_instance.p_targets[index];

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

void DebayerInterface::e_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
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

void DebayerInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void DebayerInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
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
      size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );

      inputFiles.Sort();
      for ( const String& file : inputFiles )
         m_instance.p_targets.Insert( m_instance.p_targets.At( i0++ ), DebayerInstance::Item( file ) );

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
// ----------------------------------------------------------------------------

DebayerInterface::GUIData::GUIData( DebayerInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Bayer/mosaic pattern:" ) ) + 16; // the longest label text
   int editWidth1 = fnt.Width( String( 'M', 5 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   // color parameters

   const char* patternToolTip =
      "<p>Select the CFA pattern of the camera (DSLR or OSC) used to acquire the target image.</p>"
      "<p>The Auto option requires view properties available in the target view. These properties are normally "
      "generated by the RAW format support module and stored in XISF files.</p>"
      "<p>For images acquired with X-Trans sensors this parameter is ignored and CFA patterns are always "
      "extracted from existing image properties.</p>";

   Pattern_Label.SetText( "Bayer/mosaic pattern:" );
   Pattern_Label.SetFixedWidth( labelWidth1 );
   Pattern_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Pattern_Label.SetToolTip( patternToolTip );

   Pattern_ComboBox.AddItem( "Auto" );
   Pattern_ComboBox.AddItem( "RGGB" );
   Pattern_ComboBox.AddItem( "BGGR" );
   Pattern_ComboBox.AddItem( "GBRG" );
   Pattern_ComboBox.AddItem( "GRBG" );
   Pattern_ComboBox.AddItem( "GRGB" );
   Pattern_ComboBox.AddItem( "GBGR" );
   Pattern_ComboBox.AddItem( "RGBG" );
   Pattern_ComboBox.AddItem( "BGRG" );
   Pattern_ComboBox.SetToolTip( patternToolTip );
   Pattern_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

   Pattern_Sizer.SetSpacing( 4 );
   Pattern_Sizer.Add( Pattern_Label );
   Pattern_Sizer.Add( Pattern_ComboBox );
   Pattern_Sizer.AddStretch();

   const char* methodToolTip =
      "<p>Select the demosaicing algorithm. SuperPixel creates a half sized image, very fast, good quality. "
      "The remaining methods create a full sized image, increasing in quality and computational overhead.</p>"
      "<p>For images acquired with X-Trans sensors this parameter is ignored and a special X-Trans interpolation "
      "algorithm is always applied.</p>";

   DebayerMethod_Label.SetText( "Demosaicing method:" );
   DebayerMethod_Label.SetFixedWidth( labelWidth1 );
   DebayerMethod_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DebayerMethod_Label.SetToolTip( methodToolTip );

   DebayerMethod_ComboBox.AddItem( "SuperPixel" );
   DebayerMethod_ComboBox.AddItem( "Bilinear" );
   DebayerMethod_ComboBox.AddItem( "VNG" );
   DebayerMethod_ComboBox.SetToolTip( methodToolTip );
   DebayerMethod_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );
   //DebayerMethod_ComboBox.AddItem("AHD (not yet implemented)");

   DebayerMethod_Sizer.SetSpacing( 4 );
   DebayerMethod_Sizer.Add( DebayerMethod_Label );
   DebayerMethod_Sizer.Add( DebayerMethod_ComboBox );
   DebayerMethod_Sizer.AddStretch();

   EvaluateNoise_CheckBox.SetText( "Evaluate noise" );
   EvaluateNoise_CheckBox.SetToolTip( "<p>If this option is selected, Debayer will compute per-channel noise "
      "estimates for each target image using a wavelet-based algorithm (MRS noise evaluation by default). Noise "
      "estimates will be computed from demosaiced data and will be stored as NOISExxx FITS header keywords in the "
      "output files. These estimates can be used later by several processes and scripts, most notably by the "
      "ImageIntegration tool, which uses them by default for robust image weighting based on relative SNR values.</p>" );
   EvaluateNoise_CheckBox.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   EvaluateNoise_Sizer.SetSpacing( 4 );
   EvaluateNoise_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   EvaluateNoise_Sizer.Add( EvaluateNoise_CheckBox );
   EvaluateNoise_Sizer.AddStretch();

   const char* noiseEvaluationToolTip = "<p>Noise evaluation algorithm. This option selects an algorithm for automatic "
      "estimation of the standard deviation of the noise in the demosaiced images. In all cases noise estimates assume "
      "a Gaussian distribution of the noise.</p>"
      "<p>The iterative k-sigma clipping algorithm can be used as a last-resort option in cases where the MRS algorithm "
      "(see below) does not converge systematically. This can happen for images without detectable small-scale noise; "
      "for example, images that have been smoothed as a result of bilinear demosaicing interpolation.</p>"
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
   NoiseEvaluation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

   NoiseEvaluation_Sizer.SetSpacing( 4 );
   NoiseEvaluation_Sizer.Add( NoiseEvaluation_Label );
   NoiseEvaluation_Sizer.Add( NoiseEvaluation_ComboBox );
   NoiseEvaluation_Sizer.AddStretch();

   Save_PushButton.SetText( "Save as Default" );
   Save_PushButton.SetMinWidth( labelWidth1 );
   Save_PushButton.OnClick( (ToolButton::click_event_handler)&DebayerInterface::e_Click, w );

   Restore_PushButton.SetText( "Restore from Default" );
   Restore_PushButton.OnClick( (ToolButton::click_event_handler)&DebayerInterface::e_Click, w );

   Button_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Button_Sizer.Add( Save_PushButton );
   Button_Sizer.AddSpacing( 8 );
   Button_Sizer.Add( Restore_PushButton );

   GeneralParameters_Sizer.SetSpacing( 4 );
   GeneralParameters_Sizer.Add( Pattern_Sizer );
   GeneralParameters_Sizer.Add( DebayerMethod_Sizer );
   GeneralParameters_Sizer.Add( EvaluateNoise_Sizer );
   GeneralParameters_Sizer.Add( NoiseEvaluation_Sizer );
   GeneralParameters_Sizer.Add( Button_Sizer );

   //

   TargetImages_SectionBar.SetTitle( "Target Images" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&DebayerInterface::e_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&DebayerInterface::e_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&DebayerInterface::e_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&DebayerInterface::e_FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&DebayerInterface::e_FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target images.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target images.</p>"
      "<p>Disabled target images will be ignored during the registration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target file items.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

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
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings for image files used "
      "by specific processes. In Debayer, input hints change the way input images of some particular file formats are loaded, "
      "while output hints modify the way output demosaiced image files are written.</p>"
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
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

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
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DebayerInterface::e_ToggleSection, w );

   const char* outputDirectoryToolTip = "<p>This is the directory (or folder) where all output demosaiced files will be written.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their corresponding "
      "target files. In this case, make sure that source directories are writable, or the demosaicing process will fail.</p>";

   OutputDirectory_Label.SetText( "Output directory:" );
   OutputDirectory_Label.SetFixedWidth( labelWidth1 );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   OutputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&DebayerInterface::e_FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&DebayerInterface::e_FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.SetToolTip( "<p>Select the output directory</p>" );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   const char* outputPrefixToolTip = "<p>This is a prefix that will be prepended to the file name of each demosaiced image.</p>";

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( labelWidth1 );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPrefix_Label.SetToolTip( outputPrefixToolTip );

   OutputPrefix_Edit.SetFixedWidth( editWidth1 );
   OutputPrefix_Edit.SetToolTip( outputPrefixToolTip );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

   const char* outputPostfixToolTip = "<p>This is a postfix that will be appended to the file name of each demosaiced image.</p>";

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputPostfix_Label.SetToolTip( outputPostfixToolTip );

   OutputPostfix_Edit.SetFixedWidth( editWidth1 );
   OutputPostfix_Edit.SetToolTip( outputPostfixToolTip );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&DebayerInterface::e_EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( "<p>If this option is selected, Debayer will overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&DebayerInterface::e_Click, w );

   const char* onErrorToolTip = "<p>Specify what to do if there are errors during a batch demosaicing process.</p>";

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetToolTip( onErrorToolTip );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OnError_ComboBox.AddItem( "Continue" );
   OnError_ComboBox.AddItem( "Abort" );
   OnError_ComboBox.AddItem( "Ask User" );
   OnError_ComboBox.SetToolTip( onErrorToolTip );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::e_ItemSelected, w );

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

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedWidth();

   FormatHints_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DebayerInterface.cpp - Released 2018-11-23T18:45:59Z
