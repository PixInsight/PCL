//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0230
// ----------------------------------------------------------------------------
// GradientsHdrCompositionInterface.cpp - Released 2018-11-23T18:45:58Z
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

#include "GradientsHdrCompositionInterface.h"
#include "GradientsHdrCompositionProcess.h"

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

GradientsHdrCompositionInterface* TheGradientsHdrCompositionInterface = nullptr;

// ----------------------------------------------------------------------------
//FIXME
//#include "GradientsHdrCompositionIcon.xpm"

// ----------------------------------------------------------------------------

GradientsHdrCompositionInterface::GradientsHdrCompositionInterface() :
   instance( TheGradientsHdrCompositionProcess )
{
   TheGradientsHdrCompositionInterface = this;

   /*
    * The auto save geometry feature is of no good to interfaces that include
    * both auto-expanding controls (e.g. TreeBox) and collapsible sections
    * (e.g. SectionBar).
    */
   DisableAutoSaveGeometry();
}

GradientsHdrCompositionInterface::~GradientsHdrCompositionInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString GradientsHdrCompositionInterface::Id() const
{
   return "GradientHDRComposition";
}

IsoString GradientsHdrCompositionInterface::Aliases() const
{
   return "GradientsHdrComposition";
}

MetaProcess* GradientsHdrCompositionInterface::Process() const
{
   return TheGradientsHdrCompositionProcess;
}

const char** GradientsHdrCompositionInterface::IconImageXPM() const
{
  return nullptr;
  // FIXME
  // return GradientsHdrCompositionIcon_XPM;
}

InterfaceFeatures GradientsHdrCompositionInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void GradientsHdrCompositionInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void GradientsHdrCompositionInterface::ResetInstance()
{
   GradientsHdrCompositionInstance defaultInstance( TheGradientsHdrCompositionProcess );
   ImportProcess( defaultInstance );
}

bool GradientsHdrCompositionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "GradientHDRComposition" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheGradientsHdrCompositionProcess;
}

ProcessImplementation* GradientsHdrCompositionInterface::NewProcess() const
{
   return new GradientsHdrCompositionInstance( instance );
}

bool GradientsHdrCompositionInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const GradientsHdrCompositionInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a GradientHDRComposition instance.";
   return false;
}

bool GradientsHdrCompositionInterface::RequiresInstanceValidation() const
{
   return true;
}

bool GradientsHdrCompositionInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

void GradientsHdrCompositionInterface::SaveSettings() const
{
   SaveGeometry();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void GradientsHdrCompositionInterface::UpdateControls()
{
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateParameters();
}

void GradientsHdrCompositionInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const GradientsHdrCompositionInstance::ImageItem& item = instance.targetFrames[i];

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

void GradientsHdrCompositionInterface::UpdateTargetImagesList()
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

void GradientsHdrCompositionInterface::UpdateImageSelectionButtons()
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

void GradientsHdrCompositionInterface::UpdateParameters()
{
  GUI->LogBias_NumericControl.SetValue(instance.dLogBias);
  GUI->KeepLog_CheckBox.SetChecked(instance.bKeepLog);
  GUI->NegativeBias_CheckBox.SetChecked(instance.bNegativeBias);
  GUI->GenerateMask_CheckBox.SetChecked(instance.generateMask);
}

// ----------------------------------------------------------------------------

void GradientsHdrCompositionInterface::__TargetImages_CurrentNodeUpdated( TreeBox& sender,
                                                         TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "GradientsHdrCompositionInterface: *Warning* Corrupted interface structures" );

   // ### If there's something else that depends on which image is selected in the list, do it here.
}

void GradientsHdrCompositionInterface::__TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.targetFrames.Length() )
      throw Error( "GradientsHdrCompositionInterface: *Warning* Corrupted interface structures" );

   GradientsHdrCompositionInstance::ImageItem& item = instance.targetFrames[index];

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

void GradientsHdrCompositionInterface::__TargetImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

void GradientsHdrCompositionInterface::__TargetImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "GradientsHdrComposition: Select Target Frames" );
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), GradientsHdrCompositionInstance::ImageItem( *i ) );
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
      GradientsHdrCompositionInstance::image_list newTargets;
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

void GradientsHdrCompositionInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->TargetImages_TreeBox.SetFixedHeight();
   else
   {
      GUI->TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->TargetImages_TreeBox.SetMaxHeight( int_max );
   }
}

void GradientsHdrCompositionInterface::__logBiasUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->LogBias_NumericControl )
      instance.dLogBias = value;
}

void GradientsHdrCompositionInterface::__KeepLogClicked( Button& sender, bool checked )
{
   if ( sender == GUI->KeepLog_CheckBox )
      instance.bKeepLog=checked;
}

void GradientsHdrCompositionInterface::__NegativeBiasClicked( Button& sender, bool checked )
{
   if ( sender == GUI->NegativeBias_CheckBox )
      instance.bNegativeBias=checked;
}

void GradientsHdrCompositionInterface::__GenerateMaskClicked( Button& sender, bool checked )
{
   if ( sender == GUI->GenerateMask_CheckBox )
      instance.generateMask=checked;
}

void GradientsHdrCompositionInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
}

void GradientsHdrCompositionInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
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
         instance.targetFrames.Insert( instance.targetFrames.At( i0++ ), GradientsHdrCompositionInstance::ImageItem( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

GradientsHdrCompositionInterface::GUIData::GUIData( GradientsHdrCompositionInterface& w )
{
   pcl::Font fnt = w.Font();
   int const labelWidth1 = fnt.Width( String( "Optimization window (px):" ) + 'T' );
   int const editWidth1 = fnt.Width( String( 'M', 5  ) );

   //

   w.SetToolTip( "<p>Integrates images of different exposures into a combined image using a gradient domain method. "
      "Useful for creating HDR images.</p>"
      "<p>(c) 2011 Georg Viehoever, published under LGPL 2.1. "
      "With important contributions in terms of tests, test data, code and ideas "
      "by Carlos Milovic, Harry Page and Alejandro Tombolini.</p>" );

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&GradientsHdrCompositionInterface::__ToggleSection, w );

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&GradientsHdrCompositionInterface::__TargetImages_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&GradientsHdrCompositionInterface::__TargetImages_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&GradientsHdrCompositionInterface::__TargetImages_NodeSelectionUpdated, w );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&GradientsHdrCompositionInterface::__FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&GradientsHdrCompositionInterface::__FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files..." );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&GradientsHdrCompositionInterface::__TargetImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all images.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&GradientsHdrCompositionInterface::__TargetImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of images.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&GradientsHdrCompositionInterface::__TargetImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected images.</p>"
      "<p>Disabled images will be ignored during the process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&GradientsHdrCompositionInterface::__TargetImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected images.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&GradientsHdrCompositionInterface::__TargetImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of images.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&GradientsHdrCompositionInterface::__TargetImages_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for image files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&GradientsHdrCompositionInterface::__TargetImages_Click, w );

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

   Parameters_SectionBar.SetTitle( "Parameters" );
   Parameters_SectionBar.SetSection( Parameters_Control );
   Parameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&GradientsHdrCompositionInterface::__ToggleSection, w );

   const char *biasToolTip="<p>Log10 of bias of image. This should identify the true black point of the images. " // ### Clarify description
                           "Smallest value is neutral. Use if result image is too faint</p>";
   LogBias_NumericControl.label.SetText( "Log10 Bias:" );
   LogBias_NumericControl.label.SetFixedWidth( labelWidth1 );
   LogBias_NumericControl.slider.SetScaledMinWidth( 250 );
   LogBias_NumericControl.slider.SetRange( 0, 100 );
   LogBias_NumericControl.SetReal();
   LogBias_NumericControl.SetRange( TheGradientsHdrCompositionLogBiasParameter->MinimumValue(), TheGradientsHdrCompositionLogBiasParameter->MaximumValue() );
   LogBias_NumericControl.SetPrecision( TheGradientsHdrCompositionLogBiasParameter->Precision() );
   LogBias_NumericControl.SetToolTip( biasToolTip);
   LogBias_NumericControl.edit.SetFixedWidth( editWidth1 );
   LogBias_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GradientsHdrCompositionInterface::__logBiasUpdated, w );

   const char* negativeBiasToolTip="<p>Experimental: Use if you suspect your image has a negative bias (very unusual).</p>";
   NegativeBias_Label.SetText("NegativeBias:");
   NegativeBias_Label.SetFixedWidth(labelWidth1);
   NegativeBias_Label.SetTextAlignment(TextAlign::Right|TextAlign::VertCenter );
   NegativeBias_Label.SetToolTip(negativeBiasToolTip);
   NegativeBias_CheckBox.SetChecked(TheGradientsHdrCompositionNegativeBiasParameter->DefaultValue());
   NegativeBias_CheckBox.SetToolTip(negativeBiasToolTip);
   NegativeBias_CheckBox.OnClick( (pcl::Button::click_event_handler)&GradientsHdrCompositionInterface::__NegativeBiasClicked,w);

   Bias_Sizer.SetSpacing( 4 );
   Bias_Sizer.Add(LogBias_NumericControl);

   const char* keepLogToolTip="<p>Check to keep the result in the logarithmic scale, which is better for visualization. "
                              "Uncheck to get a more or less linear result, which is better for further processing.</p>";
   KeepLog_Label.SetText("Keep in log() scale:");
   KeepLog_Label.SetFixedWidth(labelWidth1);
   KeepLog_Label.SetTextAlignment(TextAlign::Right|TextAlign::VertCenter );
   KeepLog_Label.SetToolTip(keepLogToolTip);
   KeepLog_CheckBox.SetChecked(TheGradientsHdrCompositionKeepLogParameter->DefaultValue());
   KeepLog_CheckBox.SetToolTip(keepLogToolTip);
   KeepLog_CheckBox.OnClick( (pcl::Button::click_event_handler)&GradientsHdrCompositionInterface::__KeepLogClicked,w);

   KeepLog_Sizer.SetSpacing( 4 );
   KeepLog_Sizer.Add(KeepLog_Label);
   KeepLog_Sizer.Add(KeepLog_CheckBox);

   const char* maskToolTip="<p>Check to generate two masks showing the contributions of each image to horizontal and vertical "
                           "gradients. Each pixel identifies the image that contributed to the respective gradient. 0 means no image. "
                           "The masks are rescaled before displaying.</p>";
   GenerateMask_Label.SetText("Generate masks:");
   GenerateMask_Label.SetFixedWidth(labelWidth1);
   GenerateMask_Label.SetTextAlignment(TextAlign::Right|TextAlign::VertCenter );
   GenerateMask_Label.SetToolTip(maskToolTip);
   GenerateMask_CheckBox.SetChecked(TheGradientsHdrCompositionGenerateMaskParameter->DefaultValue());
   GenerateMask_CheckBox.SetToolTip(maskToolTip);
   GenerateMask_CheckBox.OnClick( (pcl::Button::click_event_handler)&GradientsHdrCompositionInterface::__GenerateMaskClicked,w);

   GenerateMask_Sizer.SetSpacing( 4 );
   GenerateMask_Sizer.Add(GenerateMask_Label);
   GenerateMask_Sizer.Add(GenerateMask_CheckBox);
   GenerateMask_Sizer.Add(NegativeBias_Label);
   GenerateMask_Sizer.Add(NegativeBias_CheckBox);

   Parameters_Sizer.SetSpacing( 4 );
   Parameters_Sizer.Add(Bias_Sizer);
   Parameters_Sizer.Add(KeepLog_Sizer);
   Parameters_Sizer.Add(GenerateMask_Sizer);

   Parameters_Control.SetSizer(Parameters_Sizer);
   Parameters_Control.AdjustToContents();

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
// EOF GradientsHdrCompositionInterface.cpp - Released 2018-11-23T18:45:58Z
