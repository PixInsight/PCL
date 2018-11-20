//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0319
// ----------------------------------------------------------------------------
// BackgroundNeutralizationInterface.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "BackgroundNeutralizationInterface.h"
#include "BackgroundNeutralizationProcess.h"
#include "BackgroundNeutralizationParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BackgroundNeutralizationInterface* TheBackgroundNeutralizationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "BackgroundNeutralizationIcon.xpm"

// ----------------------------------------------------------------------------

BackgroundNeutralizationInterface::BackgroundNeutralizationInterface() :
   instance( TheBackgroundNeutralizationProcess )
{
   TheBackgroundNeutralizationInterface = this;
}

BackgroundNeutralizationInterface::~BackgroundNeutralizationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString BackgroundNeutralizationInterface::Id() const
{
   return "BackgroundNeutralization";
}

MetaProcess* BackgroundNeutralizationInterface::Process() const
{
   return TheBackgroundNeutralizationProcess;
}

const char** BackgroundNeutralizationInterface::IconImageXPM() const
{
   return BackgroundNeutralizationIcon_XPM;
}

InterfaceFeatures BackgroundNeutralizationInterface::Features() const
{
   return InterfaceFeature::Default;
}

void BackgroundNeutralizationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void BackgroundNeutralizationInterface::ResetInstance()
{
   BackgroundNeutralizationInstance defaultInstance( TheBackgroundNeutralizationProcess );
   ImportProcess( defaultInstance );
}

bool BackgroundNeutralizationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "BackgroundNeutralization" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheBackgroundNeutralizationProcess;
}

ProcessImplementation* BackgroundNeutralizationInterface::NewProcess() const
{
   return new BackgroundNeutralizationInstance( instance );
}

bool BackgroundNeutralizationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const BackgroundNeutralizationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a BackgroundNeutralization instance.";
   return false;
}

bool BackgroundNeutralizationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool BackgroundNeutralizationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

#define TARGET_IMAGE             String( "<target image>" )
#define REFERENCE_ID( x )        (x.IsEmpty() ? TARGET_IMAGE : x)
#define BACKGROUND_REFERENCE_ID  REFERENCE_ID( instance.backgroundReferenceViewId )

void BackgroundNeutralizationInterface::UpdateControls()
{
   GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );

   GUI->BackgroundLow_NumericControl.SetValue( instance.backgroundLow );

   GUI->BackgroundHigh_NumericControl.SetValue( instance.backgroundHigh );

   GUI->Mode_ComboBox.SetCurrentItem( instance.mode );

   GUI->TargetBackground_NumericControl.Enable( instance.mode == BNMode::TargetBackground );
   GUI->TargetBackground_NumericControl.SetValue( instance.targetBackground );

   GUI->ROI_GroupBox.SetChecked( instance.useROI );
   GUI->ROIX0_SpinBox.SetValue( instance.roi.x0 );
   GUI->ROIY0_SpinBox.SetValue( instance.roi.y0 );
   GUI->ROIWidth_SpinBox.SetValue( instance.roi.Width() );
   GUI->ROIHeight_SpinBox.SetValue( instance.roi.Height() );
}

// ----------------------------------------------------------------------------

void BackgroundNeutralizationInterface::__GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != nullptr )
      if ( e->Text() == TARGET_IMAGE )
         e->Clear();
}

void BackgroundNeutralizationInterface::__EditCompleted( Edit& sender )
{
   if ( sender == GUI->BackgroundReferenceView_Edit )
   {
      try
      {
         String id = sender.Text().Trimmed();
         if ( id == TARGET_IMAGE )
            id.Clear();
         if ( !id.IsEmpty() )
            if ( !View::IsValidViewId( id ) )
               throw Error( "Invalid view identifier: " + id );
         instance.backgroundReferenceViewId = id;
         sender.SetText( BACKGROUND_REFERENCE_ID );
      }
      catch ( ... )
      {
         sender.SetText( BACKGROUND_REFERENCE_ID );
         try
         {
            throw;
         }
         ERROR_HANDLER
         sender.SelectAll();
         sender.Focus();
      }
   }
}

void BackgroundNeutralizationInterface::__EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->BackgroundLow_NumericControl )
      instance.backgroundLow = value;
   else if ( sender == GUI->BackgroundHigh_NumericControl )
      instance.backgroundHigh = value;
   else if ( sender == GUI->TargetBackground_NumericControl )
      instance.targetBackground = value;
}

void BackgroundNeutralizationInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->BackgroundReferenceView_ToolButton )
   {
      ViewSelectionDialog d( instance.backgroundReferenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         instance.backgroundReferenceViewId = d.Id();
         GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
      }
   }
}

void BackgroundNeutralizationInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Mode_ComboBox )
   {
      instance.mode = itemIndex;
      UpdateControls();
   }
}

void BackgroundNeutralizationInterface::__ROI_Check( GroupBox& sender, bool checked )
{
   if ( sender == GUI->ROI_GroupBox )
      instance.useROI = checked;
}

void BackgroundNeutralizationInterface::__ROI_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ROIX0_SpinBox )
      instance.roi.x0 = value;
   else if ( sender == GUI->ROIY0_SpinBox )
      instance.roi.y0 = value;
   else if ( sender == GUI->ROIWidth_SpinBox )
      instance.roi.x1 = instance.roi.x0 + value;
   else if ( sender == GUI->ROIHeight_SpinBox )
      instance.roi.y1 = instance.roi.y0 + value;
}

void BackgroundNeutralizationInterface::__ROI_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ROISelectPreview_Button )
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

void BackgroundNeutralizationInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->BackgroundReferenceView_Edit )
      wantsView = true;
   else if ( sender == GUI->ROI_GroupBox || sender == GUI->ROISelectPreview_Button )
      wantsView = view.IsPreview();
}

void BackgroundNeutralizationInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->BackgroundReferenceView_Edit )
   {
      instance.backgroundReferenceViewId = view.FullId();
      GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
   }
   else if ( sender == GUI->ROI_GroupBox || sender == GUI->ROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         instance.useROI = true;
         instance.roi = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BackgroundNeutralizationInterface::GUIData::GUIData( BackgroundNeutralizationInterface& w )
{
#define DELTA_FRAME  1

   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Target background:" ) + 'T' );
   int labelWidth2 = fnt.Width( String( "Height:" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );

   //

   BackgroundReferenceView_Label.SetText( "Reference image:" );
   BackgroundReferenceView_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceView_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceView_Edit.SetToolTip( "<p>BackgroundNeutralization will use pixels read from this "
      "image to compute an initial mean background level for each color channel. If you leave this field blank "
      "(or with its default &lt;target image&gt; value), the target image will be also the background "
      "reference image during the neutralization process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>" );
   BackgroundReferenceView_Edit.OnGetFocus( (Control::event_handler)&BackgroundNeutralizationInterface::__GetFocus, w );
   BackgroundReferenceView_Edit.OnEditCompleted( (Edit::edit_event_handler)&BackgroundNeutralizationInterface::__EditCompleted, w );
   BackgroundReferenceView_Edit.OnViewDrag( (Control::view_drag_event_handler)&BackgroundNeutralizationInterface::__ViewDrag, w );
   BackgroundReferenceView_Edit.OnViewDrop( (Control::view_drop_event_handler)&BackgroundNeutralizationInterface::__ViewDrop, w );

   BackgroundReferenceView_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceView_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceView_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceView_ToolButton.OnClick( (Button::click_event_handler)&BackgroundNeutralizationInterface::__Click, w );

   BackgroundReferenceView_Sizer.SetSpacing( 4 );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Label );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Edit );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_ToolButton );

   BackgroundLow_NumericControl.label.SetText( "Lower limit:" );
   BackgroundLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundLow_NumericControl.slider.SetRange( 0, 100 );
   BackgroundLow_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundLow_NumericControl.SetReal();
   BackgroundLow_NumericControl.SetRange( TheBNBackgroundLowParameter->MinimumValue(), TheBNBackgroundLowParameter->MaximumValue() );
   BackgroundLow_NumericControl.SetPrecision( TheBNBackgroundLowParameter->Precision() );
   BackgroundLow_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundLow_NumericControl.SetToolTip( "<p>Lower bound of the set of background pixels. Background reference "
      "pixels with values less than or equal to this value will be rejected for calculation of mean background "
      "levels. Note that since the minimum allowed value for this parameter is zero, black pixels are never taken "
      "into account.</p>" );
   BackgroundLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BackgroundNeutralizationInterface::__EditValueUpdated, w );

   BackgroundHigh_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh_NumericControl.slider.SetRange( 0, 100 );
   BackgroundHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundHigh_NumericControl.SetReal();
   BackgroundHigh_NumericControl.SetRange( TheBNBackgroundHighParameter->MinimumValue(), TheBNBackgroundHighParameter->MaximumValue() );
   BackgroundHigh_NumericControl.SetPrecision( TheBNBackgroundHighParameter->Precision() );
   BackgroundHigh_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundHigh_NumericControl.SetToolTip( "<p>Upper bound of the set of background pixels. Background reference "
      "pixels above this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BackgroundNeutralizationInterface::__EditValueUpdated, w );

   const char* modeToolTip = "<p>Select a background neutralization mode.</p>"
      "<p>In <i>target background</i> mode, BackgroundNeutralization will force the target image to have "
      "the specified mean background value (see the target background parameter below) for the three RGB "
      "channels. In this mode, any resulting out-of-range values after neutralization will be truncated. "
      "There can be some (usually negligible) data clipping, but only additive transformations are applied "
      "to the data.</p>"
      "<p>In <i>rescale</i> mode, the target image will always be rescaled after neutralization. In this "
      "mode there are no data clippings, and the neutralized image maximizes dynamic range usage. However, "
      "in this mode you have no control over the resulting mean background value, and the rescaling operation "
      "is a multiplicative transformation that redistributes all pixel values throughout the available dynamic "
      "range.</p>"
      "<p>The <i>rescale as needed</i> mode is similar to <i>rescale</i>, but the target image is only rescaled "
      "if there are out-of-range-values after neutralization. This is the default mode.</p>"
      "<p>In <i>truncate</i> mode all resulting out-of-range pixels after neutralization will be truncated, "
      "which usually results in severely clipped data. This mode is useful to perform a background subtraction "
      "to a working image used for an intermediate analysis or processing step.</p>";

   Mode_Label.SetText( "Working mode:" );
   Mode_Label.SetFixedWidth( labelWidth1 );
   Mode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Mode_Label.SetToolTip( modeToolTip );

   Mode_ComboBox.AddItem( "Target Background" );
   Mode_ComboBox.AddItem( "Rescale" );
   Mode_ComboBox.AddItem( "Rescale as needed" );
   Mode_ComboBox.AddItem( "Truncate" );
   Mode_ComboBox.SetToolTip( modeToolTip );
   Mode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&BackgroundNeutralizationInterface::__ItemSelected, w );

   Mode_Sizer.SetSpacing( 4 );
   Mode_Sizer.Add( Mode_Label );
   Mode_Sizer.Add( Mode_ComboBox );
   Mode_Sizer.AddStretch();

   TargetBackground_NumericControl.label.SetText( "Target background:" );
   TargetBackground_NumericControl.label.SetFixedWidth( labelWidth1 );
   TargetBackground_NumericControl.slider.SetRange( 0, 100 );
   TargetBackground_NumericControl.slider.SetScaledMinWidth( 200 );
   TargetBackground_NumericControl.SetReal();
   TargetBackground_NumericControl.SetRange( TheBNTargetBackgroundParameter->MinimumValue(), TheBNTargetBackgroundParameter->MaximumValue() );
   TargetBackground_NumericControl.SetPrecision( TheBNTargetBackgroundParameter->Precision() );
   TargetBackground_NumericControl.edit.SetFixedWidth( editWidth1 );
   TargetBackground_NumericControl.SetToolTip( "<p>In the <i>target background</i> working mode, this is the "
      "final mean background level that will be imposed to the three RGB channels of the target image.</p>"
      "<p>In the rest of modes (<i>rescale</i>, <i>rescale as needed</i> and <i>truncate</i>) this parameter "
      "is not used.</p>" );
   TargetBackground_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BackgroundNeutralizationInterface::__EditValueUpdated, w );

   //

   ROI_GroupBox.SetTitle( "Region of Interest" );
   ROI_GroupBox.EnableTitleCheckBox();
   ROI_GroupBox.OnCheck( (GroupBox::check_event_handler)&BackgroundNeutralizationInterface::__ROI_Check, w );
   ROI_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&BackgroundNeutralizationInterface::__ViewDrag, w );
   ROI_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&BackgroundNeutralizationInterface::__ViewDrop, w );

   const char* roiX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIX0_Label.SetText( "Left:" );
   ROIX0_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   ROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIX0_Label.SetToolTip( roiX0ToolTip );

   ROIX0_SpinBox.SetRange( 0, int_max );
   ROIX0_SpinBox.SetToolTip( roiX0ToolTip );
   ROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundNeutralizationInterface::__ROI_SpinValueUpdated, w );

   const char* roiY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIY0_Label.SetText( "Top:" );
   ROIY0_Label.SetFixedWidth( labelWidth2 );
   ROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIY0_Label.SetToolTip( roiY0ToolTip );

   ROIY0_SpinBox.SetRange( 0, int_max );
   ROIY0_SpinBox.SetToolTip( roiY0ToolTip );
   ROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundNeutralizationInterface::__ROI_SpinValueUpdated, w );

   ROIRow1_Sizer.SetSpacing( 4 );
   ROIRow1_Sizer.Add( ROIX0_Label );
   ROIRow1_Sizer.Add( ROIX0_SpinBox );
   ROIRow1_Sizer.Add( ROIY0_Label );
   ROIRow1_Sizer.Add( ROIY0_SpinBox );
   ROIRow1_Sizer.AddStretch();

   const char* roiWidthToolTip = "<p>Width of the ROI in pixels.</p>";

   ROIWidth_Label.SetText( "Width:" );
   ROIWidth_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   ROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIWidth_Label.SetToolTip( roiWidthToolTip );

   ROIWidth_SpinBox.SetRange( 0, int_max );
   ROIWidth_SpinBox.SetToolTip( roiWidthToolTip );
   ROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundNeutralizationInterface::__ROI_SpinValueUpdated, w );

   const char* roiHeightToolTip = "<p>Height of the ROI in pixels.</p>";

   ROIHeight_Label.SetText( "Height:" );
   ROIHeight_Label.SetFixedWidth( labelWidth2 );
   ROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIHeight_Label.SetToolTip( roiHeightToolTip );

   ROIHeight_SpinBox.SetRange( 0, int_max );
   ROIHeight_SpinBox.SetToolTip( roiHeightToolTip );
   ROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundNeutralizationInterface::__ROI_SpinValueUpdated, w );

   ROISelectPreview_Button.SetText( "From Preview" );
   ROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>" );
   ROISelectPreview_Button.OnClick( (Button::click_event_handler)&BackgroundNeutralizationInterface::__ROI_Click, w );
   ROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&BackgroundNeutralizationInterface::__ViewDrag, w );
   ROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&BackgroundNeutralizationInterface::__ViewDrop, w );

   ROIRow2_Sizer.SetSpacing( 4 );
   ROIRow2_Sizer.Add( ROIWidth_Label );
   ROIRow2_Sizer.Add( ROIWidth_SpinBox );
   ROIRow2_Sizer.Add( ROIHeight_Label );
   ROIRow2_Sizer.Add( ROIHeight_SpinBox );
   ROIRow2_Sizer.AddSpacing( 12 );
   ROIRow2_Sizer.Add( ROISelectPreview_Button );
   ROIRow2_Sizer.AddStretch();

   ROI_Sizer.SetMargin( 6 );
   ROI_Sizer.SetSpacing( 4 );
   ROI_Sizer.Add( ROIRow1_Sizer );
   ROI_Sizer.Add( ROIRow2_Sizer );

   ROI_GroupBox.SetSizer( ROI_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );

   Global_Sizer.Add( BackgroundReferenceView_Sizer );
   Global_Sizer.Add( BackgroundLow_NumericControl );
   Global_Sizer.Add( BackgroundHigh_NumericControl );
   Global_Sizer.Add( Mode_Sizer );
   Global_Sizer.Add( TargetBackground_NumericControl );
   Global_Sizer.Add( ROI_GroupBox );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BackgroundNeutralizationInterface.cpp - Released 2018-11-01T11:07:20Z
