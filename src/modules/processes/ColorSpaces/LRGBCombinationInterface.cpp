//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0294
// ----------------------------------------------------------------------------
// LRGBCombinationInterface.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "LRGBCombinationInterface.h"
#include "LRGBCombinationProcess.h"
#include "LRGBCombinationParameters.h"

#include "ChannelSourceSelectionDialog.h"

#include <pcl/ErrorHandler.h>
#include <pcl/View.h>

#define AUTO_ID   "<Auto>"

namespace pcl
{

// ----------------------------------------------------------------------------

LRGBCombinationInterface* TheLRGBCombinationInterface = 0;

static const char* channelSuffixes[] = { "_R", "_G", "_B", "_L" };

// ----------------------------------------------------------------------------

#include "LRGBCombinationIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView  GUI->TargetImage_ViewList.CurrentView()

// ----------------------------------------------------------------------------

LRGBCombinationInterface::LRGBCombinationInterface() :
ProcessInterface(), instance( TheLRGBCombinationProcess ), GUI( 0 )
{
   TheLRGBCombinationInterface = this;
}

LRGBCombinationInterface::~LRGBCombinationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString LRGBCombinationInterface::Id() const
{
   return "LRGBCombination";
}

MetaProcess* LRGBCombinationInterface::Process() const
{
   return TheLRGBCombinationProcess;
}

const char** LRGBCombinationInterface::IconImageXPM() const
{
   return LRGBCombinationIcon_XPM;
}

InterfaceFeatures LRGBCombinationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton;
}

void LRGBCombinationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void LRGBCombinationInterface::ResetInstance()
{
   LRGBCombinationInstance defaultInstance( TheLRGBCombinationProcess );
   ImportProcess( defaultInstance );
}

bool LRGBCombinationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      GUI->TargetImage_ViewList.Regenerate( true, false ); // exclude previews
      SetWindowTitle( "LRGBCombination" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheLRGBCombinationProcess;
}

ProcessImplementation* LRGBCombinationInterface::NewProcess() const
{
   return new LRGBCombinationInstance( instance );
}

bool LRGBCombinationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const LRGBCombinationInstance* r = dynamic_cast<const LRGBCombinationInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a LRGBCombination instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool LRGBCombinationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool LRGBCombinationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   GUI->TargetImage_ViewList.SelectView( View::Null() );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LRGBCombinationInterface::UpdateControls()
{
   bool isL = instance.channelEnabled[3];
   bool isR = instance.channelEnabled[0];
   bool isG = instance.channelEnabled[1];
   bool isB = instance.channelEnabled[2];

   //

   GUI->L_CheckBox.SetChecked( isL );

   GUI->L_Edit.SetText( instance.channelId[3].IsEmpty() ? AUTO_ID : instance.channelId[3] );
   GUI->L_Edit.Enable( isL );

   GUI->L_ToolButton.Enable( isL );

   //

   GUI->R_CheckBox.SetChecked( isR );

   GUI->R_Edit.SetText( instance.channelId[0].IsEmpty() ? AUTO_ID : instance.channelId[0] );
   GUI->R_Edit.Enable( isR );

   GUI->R_ToolButton.Enable( isR );

   //

   GUI->G_CheckBox.SetChecked( isG );

   GUI->G_Edit.SetText( instance.channelId[1].IsEmpty() ? AUTO_ID : instance.channelId[1] );
   GUI->G_Edit.Enable( isG );

   GUI->G_ToolButton.Enable( isG );

   //

   GUI->B_CheckBox.SetChecked( isB );

   GUI->B_Edit.SetText( instance.channelId[2].IsEmpty() ? AUTO_ID : instance.channelId[2] );
   GUI->B_Edit.Enable( isB );

   GUI->B_ToolButton.Enable( isB );

   //

   GUI->L_Weight_NumericControl.SetValue( instance.channelWeight[3] );

   GUI->R_Weight_NumericControl.SetValue( instance.channelWeight[0] );

   GUI->G_Weight_NumericControl.SetValue( instance.channelWeight[1] );

   GUI->B_Weight_NumericControl.SetValue( instance.channelWeight[2] );

   GUI->ClipHighlights_CheckBox.SetChecked( instance.clipHighlights );

   //

   GUI->LuminanceMTF_NumericControl.SetValue( instance.luminanceMTF );

   GUI->SaturationMTF_NumericControl.SetValue( instance.saturationMTF );

   //

   GUI->NoiseReduction_SectionBar.SetChecked( instance.noiseReduction );

   GUI->LayersRemoved_SpinBox.SetValue( instance.numberOfRemovedWaveletLayers );

   GUI->LayersProtected_SpinBox.SetValue( instance.numberOfProtectedWaveletLayers );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LRGBCombinationInterface::__Channel_Click( Button& sender, bool checked )
{
   int i = -1;
   if ( sender == GUI->L_CheckBox )
      i = 3;
   else if ( sender == GUI->R_CheckBox )
      i = 0;
   else if ( sender == GUI->G_CheckBox )
      i = 1;
   else if ( sender == GUI->B_CheckBox )
      i = 2;

   if ( i >= 0 )
   {
      int n = 0;
      for ( int j = 0; j < 4; ++j )
      {
         if ( j == i )
            instance.channelEnabled[i] = checked;
         if ( instance.channelEnabled[j] )
            ++n;
      }

      if ( n == 0 )
         for ( int j = 0; j < 4; ++j )
            instance.channelEnabled[j] = true;

      UpdateControls();
   }
}

void LRGBCombinationInterface::__ChannelId_GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != 0 && e->Text() == AUTO_ID )
      e->Clear();
}

void LRGBCombinationInterface::__ChannelId_EditCompleted( Edit& sender )
{
   int i = -1;
   if ( sender == GUI->L_Edit )
      i = 3;
   else if ( sender == GUI->R_Edit )
      i = 0;
   else if ( sender == GUI->G_Edit )
      i = 1;
   else if ( sender == GUI->B_Edit )
      i = 2;

   if ( i >= 0 )
   {
      try
      {
         pcl::String id = sender.Text();
         id.Trim();

         if ( !id.IsEmpty() && id != AUTO_ID && !id.IsValidIdentifier() )
            throw Error( "Invalid identifier: " + id );

         instance.channelId[i] = (id != AUTO_ID) ? id : String();
         sender.SetText( instance.channelId[i].IsEmpty() ? AUTO_ID : instance.channelId[i] );
         return;
      }

      ERROR_CLEANUP(
         sender.SetText( instance.channelId[i] );
         sender.SelectAll();
         sender.Focus()
      )
   }
}

void LRGBCombinationInterface::__Channel_SelectSource_Click( Button& sender, bool /*checked*/ )
{
   String suffix;
   String description;

   int i = -1;
   if ( sender == GUI->L_ToolButton )
   {
      i = 3;
      suffix = "_L";
      description = "Lightness";
   }
   else if ( sender == GUI->R_ToolButton )
   {
      i = 0;
       suffix = "_R";
      description = "Red Channel";
  }
   else if ( sender == GUI->G_ToolButton )
   {
      i = 1;
      suffix = "_G";
      description = "Green Channel";
   }
   else if ( sender == GUI->B_ToolButton )
   {
      i = 2;
      suffix = "_B";
      description = "Blue Channel";
   }

   if ( i >= 0 )
   {
      ChannelSourceSelectionDialog dlg( currentView, suffix, description );

      if ( dlg.Execute() == StdDialogCode::Ok )
      {
         instance.channelId[i] = dlg.SourceId();
         UpdateControls();
      }
   }
}

void LRGBCombinationInterface::__TargetImage_ViewSelected( ViewList& /*sender*/, View& view )
{
   for ( int i = 0; i < 4; ++i )
      instance.channelId[i].Clear();

   if ( !view.IsNull() )
   {
      IsoString baseId = view.Id();
      for ( int i = 0; i < 4; ++i )
         if ( !View::ViewById( baseId + channelSuffixes[i] ).IsNull() )
            instance.channelId[i] = baseId + channelSuffixes[i];
   }

   UpdateControls();
}

void LRGBCombinationInterface::__Channel_Weight_ValueUpdated( NumericEdit& sender, double value )
{
        if ( sender == GUI->L_Weight_NumericControl )
      instance.channelWeight[3] = value;
   else if ( sender == GUI->R_Weight_NumericControl )
      instance.channelWeight[0] = value;
   else if ( sender == GUI->G_Weight_NumericControl )
      instance.channelWeight[1] = value;
   else if ( sender == GUI->B_Weight_NumericControl )
      instance.channelWeight[2] = value;
}

void LRGBCombinationInterface::__ClipHighlights_Click( Button& /*sender*/, bool checked )
{
   instance.clipHighlights = checked;
}

void LRGBCombinationInterface::__MTF_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->LuminanceMTF_NumericControl )
      instance.luminanceMTF = value;
   else if ( sender == GUI->SaturationMTF_NumericControl )
      instance.saturationMTF = value;
}

void LRGBCombinationInterface::__NoiseReduction_Check( SectionBar& /*sender*/, bool checked )
{
   instance.noiseReduction = checked;
}

void LRGBCombinationInterface::__NoiseReduction_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->LayersRemoved_SpinBox )
   {
      instance.numberOfRemovedWaveletLayers = value;

      if ( instance.numberOfProtectedWaveletLayers >= instance.numberOfRemovedWaveletLayers )
         GUI->LayersProtected_SpinBox.SetValue( instance.numberOfProtectedWaveletLayers = instance.numberOfRemovedWaveletLayers - 1 );
   }
   else if ( sender == GUI->LayersProtected_SpinBox )
   {
      instance.numberOfProtectedWaveletLayers = value;

      if ( instance.numberOfProtectedWaveletLayers >= instance.numberOfRemovedWaveletLayers )
         GUI->LayersRemoved_SpinBox.SetValue( instance.numberOfRemovedWaveletLayers = instance.numberOfProtectedWaveletLayers + 1 );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LRGBCombinationInterface::GUIData::GUIData( LRGBCombinationInterface& w )
{
   pcl::Font fnt = w.Font();
   int tgtLabelWidth = fnt.Width( "Target:" );
   int chEditWidth = fnt.Width( String( 'M', 35 ) );
   int chLabelWidth = fnt.Width( "M:" );
   int mtfLabelWidth = fnt.Width( String( "Lightness:" ) + 'M' );
   int nrLabelWidth = fnt.Width( String( "Smoothed wavelet layers:" ) + 'M' );

   //

   Channels_SectionBar.SetTitle( "Channels / Source Images" );
   Channels_SectionBar.SetSection( Channels_Control );

   //

   L_CheckBox.SetText( "L" );
   L_CheckBox.OnClick( (pcl::Button::click_event_handler)&LRGBCombinationInterface::__Channel_Click, w );
   L_CheckBox.SetFixedWidth( tgtLabelWidth );

   L_Edit.SetMinWidth( chEditWidth );
   L_Edit.OnGetFocus( (Control::event_handler)&LRGBCombinationInterface::__ChannelId_GetFocus, w );
   L_Edit.OnEditCompleted( (Edit::edit_event_handler)&LRGBCombinationInterface::__ChannelId_EditCompleted, w );

   L_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   L_ToolButton.SetScaledFixedSize( 20, 20 );
   L_ToolButton.SetToolTip( "Select the lightness (CIE L*) source image" );
   L_ToolButton.OnClick( (ToolButton::click_event_handler)&LRGBCombinationInterface::__Channel_SelectSource_Click, w );

   L_Sizer.SetSpacing( 4 );
   L_Sizer.Add( L_CheckBox );
   L_Sizer.Add( L_Edit, 100 );
   L_Sizer.Add( L_ToolButton );

   //

   R_CheckBox.SetText( "R" );
   R_CheckBox.OnClick( (pcl::Button::click_event_handler)&LRGBCombinationInterface::__Channel_Click, w );
   R_CheckBox.SetFixedWidth( tgtLabelWidth );

   R_Edit.SetMinWidth( chEditWidth );
   R_Edit.OnGetFocus( (Control::event_handler)&LRGBCombinationInterface::__ChannelId_GetFocus, w );
   R_Edit.OnEditCompleted( (Edit::edit_event_handler)&LRGBCombinationInterface::__ChannelId_EditCompleted, w );

   R_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   R_ToolButton.SetScaledFixedSize( 20, 20 );
   R_ToolButton.SetToolTip( "Select the red channel source image" );
   R_ToolButton.OnClick( (ToolButton::click_event_handler)&LRGBCombinationInterface::__Channel_SelectSource_Click, w );

   R_Sizer.SetSpacing( 4 );
   R_Sizer.Add( R_CheckBox );
   R_Sizer.Add( R_Edit, 100 );
   R_Sizer.Add( R_ToolButton );

   //

   G_CheckBox.SetText( "G" );
   G_CheckBox.OnClick( (pcl::Button::click_event_handler)&LRGBCombinationInterface::__Channel_Click, w );
   G_CheckBox.AdjustToContents();
   G_CheckBox.SetFixedWidth( tgtLabelWidth );

   G_Edit.SetMinWidth( chEditWidth );
   G_Edit.OnGetFocus( (Control::event_handler)&LRGBCombinationInterface::__ChannelId_GetFocus, w );
   G_Edit.OnEditCompleted( (Edit::edit_event_handler)&LRGBCombinationInterface::__ChannelId_EditCompleted, w );

   G_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   G_ToolButton.SetScaledFixedSize( 20, 20 );
   G_ToolButton.SetToolTip( "Select the green channel source image" );
   G_ToolButton.OnClick( (ToolButton::click_event_handler)&LRGBCombinationInterface::__Channel_SelectSource_Click, w );

   G_Sizer.SetSpacing( 4 );
   G_Sizer.Add( G_CheckBox );
   G_Sizer.Add( G_Edit, 100 );
   G_Sizer.Add( G_ToolButton );

   //

   B_CheckBox.SetText( "B" );
   B_CheckBox.OnClick( (pcl::Button::click_event_handler)&LRGBCombinationInterface::__Channel_Click, w );
   B_CheckBox.AdjustToContents();
   B_CheckBox.SetFixedWidth( tgtLabelWidth );

   B_Edit.SetMinWidth( chEditWidth );
   B_Edit.OnGetFocus( (Control::event_handler)&LRGBCombinationInterface::__ChannelId_GetFocus, w );
   B_Edit.OnEditCompleted( (Edit::edit_event_handler)&LRGBCombinationInterface::__ChannelId_EditCompleted, w );

   B_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   B_ToolButton.SetScaledFixedSize( 20, 20 );
   B_ToolButton.SetToolTip( "Select the blue channel source image" );
   B_ToolButton.OnClick( (ToolButton::click_event_handler)&LRGBCombinationInterface::__Channel_SelectSource_Click, w );

   B_Sizer.SetSpacing( 4 );
   B_Sizer.Add( B_CheckBox );
   B_Sizer.Add( B_Edit, 100 );
   B_Sizer.Add( B_ToolButton );

   //

   TargetImage_Label.SetText( "Target:" );
   TargetImage_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   TargetImage_Label.SetFixedWidth( tgtLabelWidth );
   TargetImage_Label.SetToolTip( "<p>Peek an existing view to automatically select source channel images with "
                                 "_L, _R, _G and _B suffixes, respectively.</p>" );

   TargetImage_ViewList.OnViewSelected( (ViewList::view_event_handler)&LRGBCombinationInterface::__TargetImage_ViewSelected, w );

   TargetImage_Sizer.SetSpacing( 4 );
   TargetImage_Sizer.Add( TargetImage_Label );
   TargetImage_Sizer.Add( TargetImage_ViewList, 100 );

   //

   Channels_Sizer.SetMargin( 6 );
   Channels_Sizer.SetSpacing( 4 );
   Channels_Sizer.Add( L_Sizer );
   Channels_Sizer.Add( R_Sizer );
   Channels_Sizer.Add( G_Sizer );
   Channels_Sizer.Add( B_Sizer );
   Channels_Sizer.Add( TargetImage_Sizer );

   Channels_Control.SetSizer( Channels_Sizer );
   Channels_Control.AdjustToContents();

   //

   Weights_SectionBar.SetTitle( "Channel Weights" );
   Weights_SectionBar.SetSection( Weights_Control );

   //

   L_Weight_NumericControl.label.SetText( "L:" );
   L_Weight_NumericControl.label.SetFixedWidth( chLabelWidth );
   L_Weight_NumericControl.slider.SetRange( 0, 100 );
   L_Weight_NumericControl.slider.SetScaledMinWidth( 150 );
   L_Weight_NumericControl.SetReal();
   L_Weight_NumericControl.SetRange( TheLRGBChannelWeightParameter->MinimumValue(), TheLRGBChannelWeightParameter->MaximumValue() );
   L_Weight_NumericControl.SetPrecision( TheLRGBChannelWeightParameter->Precision() );
   L_Weight_NumericControl.SetToolTip( "Lightness ratio (source:target)" );
   L_Weight_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LRGBCombinationInterface::__Channel_Weight_ValueUpdated, w );

   R_Weight_NumericControl.label.SetText( "R:" );
   R_Weight_NumericControl.label.SetFixedWidth( chLabelWidth );
   R_Weight_NumericControl.slider.SetRange( 0, 100 );
   R_Weight_NumericControl.slider.SetScaledMinWidth( 150 );
   R_Weight_NumericControl.SetReal();
   R_Weight_NumericControl.SetRange( TheLRGBChannelWeightParameter->MinimumValue(), TheLRGBChannelWeightParameter->MaximumValue() );
   R_Weight_NumericControl.SetPrecision( TheLRGBChannelWeightParameter->Precision() );
   R_Weight_NumericControl.SetToolTip( "Red channel weight" );
   R_Weight_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LRGBCombinationInterface::__Channel_Weight_ValueUpdated, w );

   G_Weight_NumericControl.label.SetText( "G:" );
   G_Weight_NumericControl.label.SetFixedWidth( chLabelWidth );
   G_Weight_NumericControl.slider.SetRange( 0, 100 );
   G_Weight_NumericControl.slider.SetScaledMinWidth( 150 );
   G_Weight_NumericControl.SetReal();
   G_Weight_NumericControl.SetRange( TheLRGBChannelWeightParameter->MinimumValue(), TheLRGBChannelWeightParameter->MaximumValue() );
   G_Weight_NumericControl.SetPrecision( TheLRGBChannelWeightParameter->Precision() );
   G_Weight_NumericControl.SetToolTip( "Green channel weight" );
   G_Weight_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LRGBCombinationInterface::__Channel_Weight_ValueUpdated, w );

   B_Weight_NumericControl.label.SetText( "B:" );
   B_Weight_NumericControl.label.SetFixedWidth( chLabelWidth );
   B_Weight_NumericControl.slider.SetRange( 0, 100 );
   B_Weight_NumericControl.slider.SetScaledMinWidth( 150 );
   B_Weight_NumericControl.SetReal();
   B_Weight_NumericControl.SetRange( TheLRGBChannelWeightParameter->MinimumValue(), TheLRGBChannelWeightParameter->MaximumValue() );
   B_Weight_NumericControl.SetPrecision( TheLRGBChannelWeightParameter->Precision() );
   B_Weight_NumericControl.SetToolTip( "Blue channel weight" );
   B_Weight_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LRGBCombinationInterface::__Channel_Weight_ValueUpdated, w );

   ClipHighlights_CheckBox.SetText( "Uniform RGB dynamic ranges" );
   ClipHighlights_CheckBox.OnClick( (pcl::Button::click_event_handler)&LRGBCombinationInterface::__ClipHighlights_Click, w );

   ClipHighlights_Sizer.AddSpacing( chLabelWidth + 4 );
   ClipHighlights_Sizer.Add( ClipHighlights_CheckBox );

   Weights_Sizer.SetMargin( 6 );
   Weights_Sizer.SetSpacing( 4 );
   Weights_Sizer.Add( L_Weight_NumericControl );
   Weights_Sizer.Add( R_Weight_NumericControl );
   Weights_Sizer.Add( G_Weight_NumericControl );
   Weights_Sizer.Add( B_Weight_NumericControl );
   Weights_Sizer.Add( ClipHighlights_Sizer );

   Weights_Control.SetSizer( Weights_Sizer );
   Weights_Control.AdjustToContents();

   //

   MTF_SectionBar.SetTitle( "Transfer Functions" );
   MTF_SectionBar.SetSection( MTF_Control );

   //

   LuminanceMTF_NumericControl.label.SetText( "Lightness:" );
   LuminanceMTF_NumericControl.label.SetMinWidth( mtfLabelWidth );
   LuminanceMTF_NumericControl.slider.SetRange( 1, 1000 );
   LuminanceMTF_NumericControl.SetReal();
   LuminanceMTF_NumericControl.SetRange( TheLRGBLuminanceMTFParameter->MinimumValue(), TheLRGBLuminanceMTFParameter->MaximumValue() );
   LuminanceMTF_NumericControl.SetPrecision( TheLRGBLuminanceMTFParameter->Precision() );
   LuminanceMTF_NumericControl.SetToolTip( "Lightness MTF" );
   LuminanceMTF_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LRGBCombinationInterface::__MTF_ValueUpdated, w );

   SaturationMTF_NumericControl.label.SetText( "Saturation:" );
   SaturationMTF_NumericControl.label.SetMinWidth( mtfLabelWidth );
   SaturationMTF_NumericControl.slider.SetRange( 1, 1000 );
   SaturationMTF_NumericControl.SetReal();
   SaturationMTF_NumericControl.SetRange( TheLRGBSaturationMTFParameter->MinimumValue(), TheLRGBSaturationMTFParameter->MaximumValue() );
   SaturationMTF_NumericControl.SetPrecision( TheLRGBSaturationMTFParameter->Precision() );
   SaturationMTF_NumericControl.SetToolTip( "Saturation MTF" );
   SaturationMTF_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LRGBCombinationInterface::__MTF_ValueUpdated, w );

   MTF_Sizer.SetMargin( 6 );
   MTF_Sizer.SetSpacing( 4 );
   MTF_Sizer.Add( LuminanceMTF_NumericControl );
   MTF_Sizer.Add( SaturationMTF_NumericControl );

   MTF_Control.SetSizer( MTF_Sizer );
   MTF_Control.AdjustToContents();

   //

   NoiseReduction_SectionBar.SetTitle( "Chrominance Noise Reduction" );
   NoiseReduction_SectionBar.EnableTitleCheckBox();
   NoiseReduction_SectionBar.OnCheck( (SectionBar::check_event_handler)&LRGBCombinationInterface::__NoiseReduction_Check, w );
   NoiseReduction_SectionBar.SetSection( NoiseReduction_Control );

   //

   LayersRemoved_Label.SetText( "Smoothed wavelet layers:" );
   LayersRemoved_Label.SetMinWidth( nrLabelWidth );
   LayersRemoved_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   LayersRemoved_SpinBox.SetRange( int( TheLRGBLayersRemovedParameter->MinimumValue() ),
                                   int( TheLRGBLayersRemovedParameter->MaximumValue() ) );
   LayersRemoved_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LRGBCombinationInterface::__NoiseReduction_ValueUpdated, w );

   NoiseReductionRow1_Sizer.SetSpacing( 4 );
   NoiseReductionRow1_Sizer.Add( LayersRemoved_Label );
   NoiseReductionRow1_Sizer.Add( LayersRemoved_SpinBox );
   NoiseReductionRow1_Sizer.AddStretch();

   LayersProtected_Label.SetText( "Protected wavelet layers:" );
   LayersProtected_Label.SetMinWidth( nrLabelWidth );
   LayersProtected_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   LayersProtected_SpinBox.SetRange( int( TheLRGBLayersProtectedParameter->MinimumValue() ),
                                     int( TheLRGBLayersProtectedParameter->MaximumValue() ) );
   LayersProtected_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LRGBCombinationInterface::__NoiseReduction_ValueUpdated, w );

   NoiseReductionRow2_Sizer.SetSpacing( 4 );
   NoiseReductionRow2_Sizer.Add( LayersProtected_Label );
   NoiseReductionRow2_Sizer.Add( LayersProtected_SpinBox );
   NoiseReductionRow2_Sizer.AddStretch();

   NoiseReduction_Sizer.SetMargin( 6 );
   NoiseReduction_Sizer.SetSpacing( 4 );
   NoiseReduction_Sizer.Add( NoiseReductionRow1_Sizer );
   NoiseReduction_Sizer.Add( NoiseReductionRow2_Sizer );

   NoiseReduction_Control.SetSizer( NoiseReduction_Sizer );
   NoiseReduction_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Channels_SectionBar );
   Global_Sizer.Add( Channels_Control );
   Global_Sizer.Add( Weights_SectionBar );
   Global_Sizer.Add( Weights_Control );
   Global_Sizer.Add( MTF_SectionBar );
   Global_Sizer.Add( MTF_Control );
   Global_Sizer.Add( NoiseReduction_SectionBar );
   Global_Sizer.Add( NoiseReduction_Control );

   w.SetSizer( Global_Sizer );

   Weights_Control.Hide();
   NoiseReduction_Control.Hide();

   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LRGBCombinationInterface.cpp - Released 2015/12/18 08:55:08 UTC
