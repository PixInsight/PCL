// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard ImageIntegration Process Module Version 01.09.04.0251
// ****************************************************************************
// DrizzleIntegrationInterface.cpp - Released 2014/10/29 07:35:23 UTC
// ****************************************************************************
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "DrizzleIntegrationInterface.h"
#include "DrizzleIntegrationProcess.h"

#include <pcl/FileDialog.h>
#include <pcl/MessageBox.h>
#include <pcl/PreviewSelectionDialog.h>

#define FILELIST_MINHEIGHT( fnt )  (8*fnt.Height() + 2)

namespace pcl
{

// ----------------------------------------------------------------------------

DrizzleIntegrationInterface* TheDrizzleIntegrationInterface = 0;

// ----------------------------------------------------------------------------

#include "DrizzleIntegrationIcon.xpm"

// ----------------------------------------------------------------------------

DrizzleIntegrationInterface::DrizzleIntegrationInterface() :
ProcessInterface(), m_instance( TheDrizzleIntegrationProcess ), GUI( 0 )
{
   TheDrizzleIntegrationInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. TreeBox) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

DrizzleIntegrationInterface::~DrizzleIntegrationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString DrizzleIntegrationInterface::Id() const
{
   return "DrizzleIntegration";
}

MetaProcess* DrizzleIntegrationInterface::Process() const
{
   return TheDrizzleIntegrationProcess;
}

const char** DrizzleIntegrationInterface::IconImageXPM() const
{
   return DrizzleIntegrationIcon_XPM;
}

InterfaceFeatures DrizzleIntegrationInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void DrizzleIntegrationInterface::ResetInstance()
{
   DrizzleIntegrationInstance defaultInstance( TheDrizzleIntegrationProcess );
   ImportProcess( defaultInstance );
}

bool DrizzleIntegrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "DrizzleIntegration" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheDrizzleIntegrationProcess;
}

ProcessImplementation* DrizzleIntegrationInterface::NewProcess() const
{
   return new DrizzleIntegrationInstance( m_instance );
}

bool DrizzleIntegrationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const DrizzleIntegrationInstance* r = dynamic_cast<const DrizzleIntegrationInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not an DrizzleIntegration m_instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool DrizzleIntegrationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool DrizzleIntegrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

void DrizzleIntegrationInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DrizzleIntegrationInterface::UpdateControls()
{
   UpdateInputDataList();
   UpdateDataSelectionButtons();
   UpdateFormatHintsControls();
   UpdateIntegrationControls();
   UpdateROIControls();
}

void DrizzleIntegrationInterface::UpdateInputDataItem( size_type i )
{
   TreeBox::Node* node = GUI->InputData_TreeBox[i];
   if ( node == 0 )
      return;

   const DrizzleIntegrationInstance::DataItem& item = m_instance.p_inputData[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( String( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   String fileText;
   if ( GUI->FullPaths_CheckBox.IsChecked() )
      fileText.Append( item.path );
   else
      fileText.Append( File::ExtractNameAndSuffix( item.path ) );

   node->SetText( 2, fileText );
   node->SetAlignment( 2, TextAlign::Left );
   if ( !GUI->FullPaths_CheckBox.IsChecked() )
      node->SetToolTip( 2, item.path );
}

void DrizzleIntegrationInterface::UpdateInputDataList()
{
   int currentIdx = GUI->InputData_TreeBox.ChildIndex( GUI->InputData_TreeBox.CurrentNode() );

   GUI->InputData_TreeBox.DisableUpdates();
   GUI->InputData_TreeBox.Clear();

   for ( size_type i = 0; i < m_instance.p_inputData.Length(); ++i )
   {
      new TreeBox::Node( GUI->InputData_TreeBox );
      UpdateInputDataItem( i );
   }

   GUI->InputData_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->InputData_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->InputData_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !m_instance.p_inputData.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->InputData_TreeBox.NumberOfChildren() )
         GUI->InputData_TreeBox.SetCurrentNode( GUI->InputData_TreeBox[currentIdx] );

   GUI->InputData_TreeBox.EnableUpdates();
}

void DrizzleIntegrationInterface::UpdateDataSelectionButtons()
{
   bool hasItems = GUI->InputData_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->InputData_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
}

void DrizzleIntegrationInterface::UpdateFormatHintsControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->InputDirectory_Edit.SetText( m_instance.p_inputDirectory );
}

void DrizzleIntegrationInterface::UpdateIntegrationControls()
{
   GUI->Scale_SpinBox.SetValue( RoundInt( m_instance.p_scale ) );
   GUI->DropShrink_NumericControl.SetValue( m_instance.p_dropShrink );
   GUI->EnableRejection_CheckBox.SetChecked( m_instance.p_enableRejection );
   GUI->EnableImageWeighting_CheckBox.SetChecked( m_instance.p_enableImageWeighting );
   GUI->EnableSurfaceSplines_CheckBox.SetChecked( m_instance.p_enableSurfaceSplines );
   GUI->ClosePreviousImages_CheckBox.SetChecked( m_instance.p_closePreviousImages );
}

void DrizzleIntegrationInterface::UpdateROIControls()
{
   GUI->ROI_SectionBar.SetChecked( m_instance.p_useROI );
   GUI->ROIX0_SpinBox.SetValue( m_instance.p_roi.x0 );
   GUI->ROIY0_SpinBox.SetValue( m_instance.p_roi.y0 );
   GUI->ROIWidth_SpinBox.SetValue( m_instance.p_roi.Width() );
   GUI->ROIHeight_SpinBox.SetValue( m_instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------

void DrizzleIntegrationInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->DropShrink_NumericControl )
      m_instance.p_dropShrink = value;
}


void DrizzleIntegrationInterface::__CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_inputData.Length() )
      throw Error( "DrizzleIntegrationInterface: *Warning* Corrupted interface structures" );

   // ### If there's something that depends on which item is selected in the list, do it here.
}

void DrizzleIntegrationInterface::__NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_inputData.Length() )
      throw Error( "DrizzleIntegrationInterface: *Warning* Corrupted interface structures" );

   DrizzleIntegrationInstance::DataItem& item = m_instance.p_inputData[index];

   switch ( col )
   {
   case 0:
      break;
   case 1:
      item.enabled = !item.enabled;
      UpdateInputDataItem( index );
      break;
   case 2:
      {
         /*
          * ### TODO: Open drizzle data file and show a summary of drizzle data.
          */
      }
      break;
   }
}

void DrizzleIntegrationInterface::__NodeSelectionUpdated( TreeBox& sender )
{
   UpdateDataSelectionButtons();
}

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != 0) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

void DrizzleIntegrationInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      FileFilter drzFiles;
      drzFiles.SetDescription( "Drizzle Data Files" );
      drzFiles.AddExtension( ".drz" );

      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.Filters().Clear();
      d.Filters().Add( drzFiles );
      d.SetCaption( "DrizzleIntegration: Select Drizzle Data Files" );
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->InputData_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            m_instance.p_inputData.Insert( m_instance.p_inputData.At( i0++ ), DrizzleIntegrationInstance::DataItem( *i ) );
         UpdateInputDataList();
         UpdateDataSelectionButtons();
      }
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->InputData_TreeBox.SelectAllNodes();
      UpdateDataSelectionButtons();
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->InputData_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->InputData_TreeBox[i]->Select( !GUI->InputData_TreeBox[i]->IsSelected() );
      UpdateDataSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->InputData_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->InputData_TreeBox[i]->IsSelected() )
            m_instance.p_inputData[i].enabled = !m_instance.p_inputData[i].enabled;
      UpdateInputDataList();
      UpdateDataSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      DrizzleIntegrationInstance::input_data_list newImages;
      for ( int i = 0, n = GUI->InputData_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->InputData_TreeBox[i]->IsSelected() )
            newImages.Add( m_instance.p_inputData[i] );
      m_instance.p_inputData = newImages;
      UpdateInputDataList();
      UpdateDataSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      m_instance.p_inputData.Clear();
      UpdateInputDataList();
      UpdateDataSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateInputDataList();
      UpdateDataSelectionButtons();
   }
   else if ( sender == GUI->InputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "DrizzleIntegration: Select Input Directory" );
      if ( d.Execute() )
         GUI->InputDirectory_Edit.SetText( m_instance.p_inputDirectory = d.Directory() );
   }
   else if ( sender == GUI->EnableRejection_CheckBox )
   {
      m_instance.p_enableRejection = checked;
   }
   else if ( sender == GUI->EnableImageWeighting_CheckBox )
   {
      m_instance.p_enableImageWeighting = checked;
   }
   else if ( sender == GUI->EnableSurfaceSplines_CheckBox )
   {
      m_instance.p_enableSurfaceSplines = checked;
   }
   else if ( sender == GUI->ClosePreviousImages_CheckBox )
   {
      m_instance.p_closePreviousImages = checked;
   }
   else if ( sender == GUI->SelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               m_instance.p_roi = view.Window().PreviewRect( view.Id() );
               UpdateROIControls();
            }
         }
   }
}

void DrizzleIntegrationInterface::__EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();
   if ( sender == GUI->InputHints_Edit )
      m_instance.p_inputHints = text;
   else if ( sender == GUI->InputDirectory_Edit )
      m_instance.p_inputDirectory = text;
   sender.SetText( text );
}

void DrizzleIntegrationInterface::__SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Scale_SpinBox )
      m_instance.p_scale = value;
   else if ( sender == GUI->ROIX0_SpinBox )
      m_instance.p_roi.x0 = value;
   else if ( sender == GUI->ROIY0_SpinBox )
      m_instance.p_roi.y0 = value;
   else if ( sender == GUI->ROIWidth_SpinBox )
      m_instance.p_roi.x1 = m_instance.p_roi.x0 + value;
   else if ( sender == GUI->ROIHeight_SpinBox )
      m_instance.p_roi.y1 = m_instance.p_roi.y0 + value;
}

void DrizzleIntegrationInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->InputData_TreeBox.SetFixedHeight();
   else
   {
      GUI->InputData_TreeBox.SetMinHeight( FILELIST_MINHEIGHT( Font() ) );
      GUI->InputData_TreeBox.SetMaxHeight( int_max );

      if ( GUI->InputData_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

void DrizzleIntegrationInterface::__CheckSection( SectionBar& sender, bool checked )
{
   if ( sender == GUI->ROI_SectionBar )
      m_instance.p_useROI = checked;
}

// ----------------------------------------------------------------------------

DrizzleIntegrationInterface::GUIData::GUIData( DrizzleIntegrationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Input directory:" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 8 ) );
   //int spinWidth1 = fnt.Width( String( '0', 11 ) );

   const int sliderWidth = 250;

   //

   InputData_SectionBar.SetTitle( "Input Data" );
   InputData_SectionBar.SetSection( InputData_Control );
   InputData_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DrizzleIntegrationInterface::__ToggleSection, w );

   InputData_TreeBox.SetMinHeight( FILELIST_MINHEIGHT( fnt ) );
   InputData_TreeBox.SetNumberOfColumns( 3 );
   InputData_TreeBox.HideHeader();
   InputData_TreeBox.EnableMultipleSelections();
   InputData_TreeBox.DisableRootDecoration();
   InputData_TreeBox.EnableAlternateRowColor();
   InputData_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&DrizzleIntegrationInterface::__CurrentNodeUpdated, w );
   InputData_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&DrizzleIntegrationInterface::__NodeActivated, w );
   InputData_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&DrizzleIntegrationInterface::__NodeSelectionUpdated, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing drizzle data files to the list of input data files.</p>"
      "<p>Drizzle data files carry the .drz suffix. Normally you should select .drz files generated by the "
      "StarAlignment and ImageIntegration tools for the images that you want to integrate with DrizzleIntegration.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all input data files.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of input data files.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected input data files.</p>"
      "<p>Disabled input files will be ignored during the drizzle integration process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected input data files.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of input data files.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for input data files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   InputButtons_Sizer.SetSpacing( 4 );
   InputButtons_Sizer.Add( AddFiles_PushButton );
   InputButtons_Sizer.Add( SelectAll_PushButton );
   InputButtons_Sizer.Add( InvertSelection_PushButton );
   InputButtons_Sizer.Add( ToggleSelected_PushButton );
   InputButtons_Sizer.Add( RemoveSelected_PushButton );
   InputButtons_Sizer.Add( Clear_PushButton );
   InputButtons_Sizer.Add( FullPaths_CheckBox );
   InputButtons_Sizer.AddStretch();

   InputData_Sizer.SetSpacing( 4 );
   InputData_Sizer.Add( InputData_TreeBox, 100 );
   InputData_Sizer.Add( InputButtons_Sizer );

   InputData_Control.SetSizer( InputData_Sizer );
   InputData_Control.AdjustToContents();

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DrizzleIntegrationInterface::__ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings "
      "for image files used by specific processes. In DrizzleIntegration, input hints change the way input images "
      "of some particular file formats are loaded during the integration process. There are no output hints in "
      "DrizzleIntegration since this process does not write images to disk files.</p>"
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
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&DrizzleIntegrationInterface::__EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   const char* outputDirectoryToolTip = "<p>Override the directories of input image files.</p>"
      "<p>If this field is left blank, input images will be loaded from the directories specified in their corresponding "
      "drizzle data files. This is the default behavior.</p>"
      "<p>If an existing directory is specified, it will override the directories specified in all input drizzle data "
      "files. This is useful when one is going to execute an instance of DrizzleIntegration that has been generated on a "
      "different machine or filesystem; for example, if you borrow a process icon from a friend, quite likely the same "
      "images will be stored on different locations of your hard disk.</p>";

   InputDirectory_Label.SetText( "Input directory:" );
   InputDirectory_Label.SetFixedWidth( labelWidth1 );
   InputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputDirectory_Label.SetToolTip( outputDirectoryToolTip );

   InputDirectory_Edit.SetToolTip( outputDirectoryToolTip );
   InputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&DrizzleIntegrationInterface::__EditCompleted, w );

   InputDirectory_ToolButton.SetIcon( Bitmap( String( ":/icons/select-file.png" ) ) );
   InputDirectory_ToolButton.SetFixedSize( 20, 20 );
   InputDirectory_ToolButton.SetToolTip( "<p>Select the input directory</p>" );
   InputDirectory_ToolButton.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   InputDirectory_Sizer.SetSpacing( 4 );
   InputDirectory_Sizer.Add( InputDirectory_Label );
   InputDirectory_Sizer.Add( InputDirectory_Edit, 100 );
   InputDirectory_Sizer.Add( InputDirectory_ToolButton );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );
   FormatHints_Sizer.Add( InputDirectory_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );
   FormatHints_Control.AdjustToContents();

   //

   Integration_SectionBar.SetTitle( "Drizzle" );
   Integration_SectionBar.SetSection( Integration_Control );
   Integration_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DrizzleIntegrationInterface::__ToggleSection, w );

   const char* scaleToolTip = "<p>Drizzle output scale, or <i>subsampling ratio</i>. This is the factor that "
      "multiplies input image dimensions (width, height) to compute the dimensions in pixels of the output integrated "
      "image. For example, to perform a 'drizzle x2' integration, the corresponding drizzle scale is 2 and the output "
      "image will have four times the area of the input reference image in square pixels.</p>";

   Scale_Label.SetText( "Scale:" );
   Scale_Label.SetFixedWidth( labelWidth1 );
   Scale_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Scale_Label.SetToolTip( scaleToolTip );

   Scale_SpinBox.SetRange( int( TheDZScaleParameter->MinimumValue() ), int( TheDZScaleParameter->MaximumValue() ) );
   Scale_SpinBox.SetFixedWidth( editWidth1 );
   Scale_SpinBox.SetToolTip( scaleToolTip );
   Scale_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DrizzleIntegrationInterface::__SpinValueUpdated, w );

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( Scale_Label );
   Scale_Sizer.Add( Scale_SpinBox );
   Scale_Sizer.AddStretch();

   DropShrink_NumericControl.label.SetText( "Drop shrink:" );
   DropShrink_NumericControl.label.SetFixedWidth( labelWidth1 );
   DropShrink_NumericControl.slider.SetRange( 0, 200 );
   DropShrink_NumericControl.slider.SetMinWidth( sliderWidth );
   DropShrink_NumericControl.SetReal();
   DropShrink_NumericControl.SetRange( TheDZDropShrinkParameter->MinimumValue(), TheDZDropShrinkParameter->MaximumValue() );
   DropShrink_NumericControl.SetPrecision( TheDZDropShrinkParameter->Precision() );
   DropShrink_NumericControl.edit.SetFixedWidth( editWidth1 );
   DropShrink_NumericControl.SetToolTip( "<p>Drizzle drop shrink factor. This is a reduction factor applied to input "
      "image pixels. Smaller input pixels or <i>drops</i> tend to yield sharper results because the integrated image is "
      "formed by convolution with a smaller PSF. However, smaller input pixels are more prone to <i>dry</i> (uncovered) "
      "output pixels, visible patterns caused by partial sampling, and overall decreased SNR. Low shrink factors require "
      "more and better dithered input images. The default drop shrink factor is 0.9.</p>" );
   DropShrink_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&DrizzleIntegrationInterface::__ValueUpdated, w );

   EnableRejection_CheckBox.SetText( "Enable pixel rejection" );
   EnableRejection_CheckBox.SetToolTip( "<p>When the input drizzle files include pixel rejection data, use them. Pixel "
      "rejection data are generated by the ImageIntegration tool, which you should have used with the <i>generate drizzle "
      "data</i> option enabled after StarAlignment and before the DrizzleIntegration tool. If this option is disabled, "
      "existing pixel rejection data will be ignored and hence no pixel rejection will be performed to generate the output "
      "integrated image.</p>" );
   EnableRejection_CheckBox.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   EnableRejection_Sizer.AddSpacing( labelWidth1 + 4 );
   EnableRejection_Sizer.Add( EnableRejection_CheckBox );
   EnableRejection_Sizer.AddStretch();

   EnableImageWeighting_CheckBox.SetText( "Enable image weighting" );
   EnableImageWeighting_CheckBox.SetToolTip( "<p>When the input drizzle files include image weighting data, use them. Image "
      "weights are generated by the ImageIntegration tool, which you should have used with the <i>generate drizzle data</i> "
      "option enabled after StarAlignment and before the DrizzleIntegration tool. If this option is disabled, existing image "
      "weighting data will be ignored and hence all input images will contribute equally to the output integrated image. In "
      "general, image weights based on scaled noise estimates (as generated by the ImageIntegration tool) should always be "
      "used to achieve an optimal result in terms of SNR improvement.</p>" );
   EnableImageWeighting_CheckBox.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   EnableImageWeighting_Sizer.AddSpacing( labelWidth1 + 4 );
   EnableImageWeighting_Sizer.Add( EnableImageWeighting_CheckBox );
   EnableImageWeighting_Sizer.AddStretch();

   EnableSurfaceSplines_CheckBox.SetText( "Enable surface splines" );
   EnableSurfaceSplines_CheckBox.SetToolTip( "<p>If the input drizzle files include surface splines for image registration, "
      "use them. Registration splines are generated by the StarAlignment tool. If this option is disabled, projective "
      "transformations will always be used. This can be useful to save time during tests, since surface splines require "
      "additional slow computations for each input image.</p>" );
   EnableSurfaceSplines_CheckBox.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   EnableSurfaceSplines_Sizer.AddSpacing( labelWidth1 + 4 );
   EnableSurfaceSplines_Sizer.Add( EnableSurfaceSplines_CheckBox );
   EnableSurfaceSplines_Sizer.AddStretch();

   ClosePreviousImages_CheckBox.SetText( "Close previous images" );
   ClosePreviousImages_CheckBox.SetToolTip( "<p>Select this option to close existing drizzle integration and weight images "
      "before running a new integration process. This is useful to avoid accumulation of multiple results on the workspace, "
      "when the same integration is being tested repeatedly.</p>" );
   ClosePreviousImages_CheckBox.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

   ClosePreviousImages_Sizer.AddSpacing( labelWidth1 + 4 );
   ClosePreviousImages_Sizer.Add( ClosePreviousImages_CheckBox );
   ClosePreviousImages_Sizer.AddStretch();

   Integration_Sizer.SetSpacing( 4 );
   Integration_Sizer.Add( Scale_Sizer );
   Integration_Sizer.Add( DropShrink_NumericControl );
   Integration_Sizer.Add( EnableRejection_Sizer );
   Integration_Sizer.Add( EnableImageWeighting_Sizer );
   Integration_Sizer.Add( EnableSurfaceSplines_Sizer );
   Integration_Sizer.Add( ClosePreviousImages_Sizer );

   Integration_Control.SetSizer( Integration_Sizer );
   Integration_Control.AdjustToContents();

   //

   ROI_SectionBar.SetTitle( "Region of Interest" );
   ROI_SectionBar.EnableTitleCheckBox();
   ROI_SectionBar.OnCheck( (SectionBar::check_event_handler)&DrizzleIntegrationInterface::__CheckSection, w );
   ROI_SectionBar.SetSection( ROI_Control );
   ROI_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DrizzleIntegrationInterface::__ToggleSection, w );
   ROI_SectionBar.SetToolTip( "<p>A region of interest (ROI) defines a rectangular subset of the image to perform "
      "the drizzle integration task. This is useful because the task's execution time (and also its memory space "
      "consumption) grows quadratically with the dimensions of the output integrated image. By defining a small ROI "
      "one can speed up the process significantly, which can be very helpful to test the drizzle process repeatedly.</p>"
      "<p>Note that the coordinates and dimensions of the ROI are expressed in input reference pixels, <i>not</i> in "
      "output pixels. You can define ROI coordinates on an integrated image generated by the ImageIntegration tool, or "
      "on a registered image created by StarAlignment, for example by defining a preview and clicking the "
      "<i>from preview</i> button below.</p>" );

   const char* roiX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIX0_Label.SetText( "Left:" );
   ROIX0_Label.SetFixedWidth( labelWidth1 );
   ROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIX0_Label.SetToolTip( roiX0ToolTip );

   ROIX0_SpinBox.SetRange( 0, int_max );
   //ROIX0_SpinBox.SetFixedWidth( spinWidth1 );
   ROIX0_SpinBox.SetToolTip( roiX0ToolTip );
   ROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DrizzleIntegrationInterface::__SpinValueUpdated, w );

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
   ROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DrizzleIntegrationInterface::__SpinValueUpdated, w );

   ROIY0_Sizer.SetSpacing( 4 );
   ROIY0_Sizer.Add( ROIY0_Label );
   ROIY0_Sizer.Add( ROIY0_SpinBox );
   ROIY0_Sizer.AddStretch();

   const char* roiWidthToolTip = "<p>Width of the ROI in reference image pixels.</p>";

   ROIWidth_Label.SetText( "Width:" );
   ROIWidth_Label.SetFixedWidth( labelWidth1 );
   ROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIWidth_Label.SetToolTip( roiWidthToolTip );

   ROIWidth_SpinBox.SetRange( 0, int_max );
   //ROIWidth_SpinBox.SetFixedWidth( spinWidth1 );
   ROIWidth_SpinBox.SetToolTip( roiWidthToolTip );
   ROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DrizzleIntegrationInterface::__SpinValueUpdated, w );

   ROIWidth_Sizer.SetSpacing( 4 );
   ROIWidth_Sizer.Add( ROIWidth_Label );
   ROIWidth_Sizer.Add( ROIWidth_SpinBox );
   ROIWidth_Sizer.AddStretch();

   const char* roiHeightToolTip = "<p>Height of the ROI in reference image pixels.</p>";

   ROIHeight_Label.SetText( "Height:" );
   ROIHeight_Label.SetFixedWidth( labelWidth1 );
   ROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIHeight_Label.SetToolTip( roiHeightToolTip );

   ROIHeight_SpinBox.SetRange( 0, int_max );
   //ROIHeight_SpinBox.SetFixedWidth( spinWidth1 );
   ROIHeight_SpinBox.SetToolTip( roiHeightToolTip );
   ROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DrizzleIntegrationInterface::__SpinValueUpdated, w );

   SelectPreview_Button.SetText( "From Preview" );
   SelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview in input reference image coordinates.</p>" );
   SelectPreview_Button.OnClick( (Button::click_event_handler)&DrizzleIntegrationInterface::__Click, w );

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
   Global_Sizer.Add( InputData_SectionBar );
   Global_Sizer.Add( InputData_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( Integration_SectionBar );
   Global_Sizer.Add( Integration_Control );
   Global_Sizer.Add( ROI_SectionBar );
   Global_Sizer.Add( ROI_Control );

   w.SetSizer( Global_Sizer );
   w.SetFixedWidth();

   FormatHints_Control.Hide();
   ROI_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF DrizzleIntegrationInterface.cpp - Released 2014/10/29 07:35:23 UTC
