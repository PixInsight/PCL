//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0336
// ----------------------------------------------------------------------------
// ColorCalibrationInterface.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ColorCalibrationInterface.h"
#include "ColorCalibrationProcess.h"
#include "ColorCalibrationParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorCalibrationInterface* TheColorCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "ColorCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

ColorCalibrationInterface::ColorCalibrationInterface() :
   instance( TheColorCalibrationProcess )
{
   TheColorCalibrationInterface = this;
}

ColorCalibrationInterface::~ColorCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString ColorCalibrationInterface::Id() const
{
   return "ColorCalibration";
}

MetaProcess* ColorCalibrationInterface::Process() const
{
   return TheColorCalibrationProcess;
}

const char** ColorCalibrationInterface::IconImageXPM() const
{
   return ColorCalibrationIcon_XPM;
}

InterfaceFeatures ColorCalibrationInterface::Features() const
{
   return InterfaceFeature::Default /*| InterfaceFeature::RealTimeButton*/;
}

void ColorCalibrationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

/*
void ColorCalibrationInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         newRTData = true;
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      }
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}
*/

void ColorCalibrationInterface::ResetInstance()
{
   ColorCalibrationInstance defaultInstance( TheColorCalibrationProcess );
   ImportProcess( defaultInstance );
}

bool ColorCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ColorCalibration" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheColorCalibrationProcess;
}

ProcessImplementation* ColorCalibrationInterface::NewProcess() const
{
   return new ColorCalibrationInstance( instance );
}

bool ColorCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const ColorCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a ColorCalibration instance.";
   return false;
}

bool ColorCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ColorCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );

   UpdateControls();

   /*
   if ( IsRealTimePreviewActive() )
      UpdateRealTimePreview();
   */

   return true;
}

/*
bool ColorCalibrationInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const
{
   return true;
}

bool ColorCalibrationInterface::GenerateRealTimePreview( UInt16Image& img, const View& view, int zoomLevel, String& info ) const
{
   USMRTStatus status;

   UInt16Image wrk;

   unsigned savedRate = StatusMonitor::RefreshRate();
   StatusMonitor::SetRefreshRate( 50 );

   bool ok;

   do
   {
      newRTData = false;
      ok = true;

      wrk.Assign( img );
      wrk.SetStatusCallback( &status );

      ColorCalibrationInstance wrkInstance( instance );

      if ( zoomLevel < 0 )
         wrkInstance.sigma /= -zoomLevel;

      try
      {
         wrkInstance.USM16( wrk );
      }

      catch ( ProcessAborted& )
      {
      }

      catch ( ... )
      {
         ok = false;
         break;
      }

      if ( !IsRealTimePreviewActive() )
         ok = false;
   }
   while ( ok && newRTData );

   StatusMonitor::SetRefreshRate( savedRate );

   if ( !ok )
      return false;

   wrk.ResetSelections();
   img.Assign( wrk );

   return true;
}
*/

// ----------------------------------------------------------------------------

#define TARGET_IMAGE             String( "<target image>" )

#define REFERENCE_ID( x )        (x.IsEmpty() ? TARGET_IMAGE : x)

#define WHITE_REFERENCE_ID       REFERENCE_ID( instance.whiteReferenceViewId )

#define BACKGROUND_REFERENCE_ID  REFERENCE_ID( instance.backgroundReferenceViewId )

void ColorCalibrationInterface::UpdateControls()
{
   GUI->WhiteReferenceView_Label.Enable( !instance.manualWhiteBalance );

   GUI->WhiteReferenceView_Edit.Enable( !instance.manualWhiteBalance );
   GUI->WhiteReferenceView_Edit.SetText( WHITE_REFERENCE_ID );

   GUI->WhiteReferenceView_ToolButton.Enable( !instance.manualWhiteBalance );

   GUI->WhiteLow_NumericControl.Enable( !instance.manualWhiteBalance );
   GUI->WhiteLow_NumericControl.SetValue( instance.whiteLow );

   GUI->WhiteHigh_NumericControl.Enable( !instance.manualWhiteBalance );
   GUI->WhiteHigh_NumericControl.SetValue( instance.whiteHigh );

   GUI->WhiteROI_GroupBox.SetChecked( instance.whiteUseROI );
   GUI->WhiteROIX0_SpinBox.SetValue( instance.whiteROI.x0 );
   GUI->WhiteROIY0_SpinBox.SetValue( instance.whiteROI.y0 );
   GUI->WhiteROIWidth_SpinBox.SetValue( instance.whiteROI.Width() );
   GUI->WhiteROIHeight_SpinBox.SetValue( instance.whiteROI.Height() );

   GUI->StructureDetection_GroupBox.Enable( !instance.manualWhiteBalance );
   GUI->StructureDetection_GroupBox.SetChecked( instance.structureDetection );

   GUI->StructureLayers_Label.Enable( !instance.manualWhiteBalance && instance.structureDetection );

   GUI->StructureLayers_SpinBox.Enable( !instance.manualWhiteBalance && instance.structureDetection );
   GUI->StructureLayers_SpinBox.SetValue( instance.structureLayers );

   GUI->NoiseLayers_Label.Enable( !instance.manualWhiteBalance && instance.structureDetection );

   GUI->NoiseLayers_SpinBox.Enable( !instance.manualWhiteBalance && instance.structureDetection );
   GUI->NoiseLayers_SpinBox.SetValue( instance.noiseLayers );

   GUI->ManualWhiteBalance_GroupBox.Enable( !instance.structureDetection );
   GUI->ManualWhiteBalance_GroupBox.SetChecked( instance.manualWhiteBalance );

   GUI->ManualRedFactor_NumericControl.Enable( instance.manualWhiteBalance );
   GUI->ManualRedFactor_NumericControl.SetValue( instance.manualRedFactor );

   GUI->ManualGreenFactor_NumericControl.Enable( instance.manualWhiteBalance );
   GUI->ManualGreenFactor_NumericControl.SetValue( instance.manualGreenFactor );

   GUI->ManualBlueFactor_NumericControl.Enable( instance.manualWhiteBalance );
   GUI->ManualBlueFactor_NumericControl.SetValue( instance.manualBlueFactor );

   GUI->OutputWhiteReferenceMask_CheckBox.Enable( !instance.manualWhiteBalance );
   GUI->OutputWhiteReferenceMask_CheckBox.SetChecked( instance.outputWhiteReferenceMask );

   GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );

   GUI->BackgroundLow_NumericControl.SetValue( instance.backgroundLow );

   GUI->BackgroundHigh_NumericControl.SetValue( instance.backgroundHigh );

   GUI->BackgroundROI_GroupBox.SetChecked( instance.backgroundUseROI );
   GUI->BackgroundROIX0_SpinBox.SetValue( instance.backgroundROI.x0 );
   GUI->BackgroundROIY0_SpinBox.SetValue( instance.backgroundROI.y0 );
   GUI->BackgroundROIWidth_SpinBox.SetValue( instance.backgroundROI.Width() );
   GUI->BackgroundROIHeight_SpinBox.SetValue( instance.backgroundROI.Height() );

   GUI->OutputBackgroundReferenceMask_CheckBox.SetChecked( instance.outputBackgroundReferenceMask );
}

/*
void ColorCalibrationInterface::UpdateRealTimePreview()
{
   if ( !GUI->UpdateRealTime_Timer.IsRunning() )
   {
      newRTData = true;
      RealTimePreview::Update();
   }
}
*/

// ----------------------------------------------------------------------------

void ColorCalibrationInterface::__GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != nullptr )
      if ( e->Text() == TARGET_IMAGE )
         e->Clear();
}

void ColorCalibrationInterface::__EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text();
      id.Trim();
      if ( id == TARGET_IMAGE )
         id.Clear();
      if ( !id.IsEmpty() )
      {
         bool valid;
         size_type p = id.Find( "->" );
         if ( p == String::notFound )
            valid = id.IsValidIdentifier();
         else
            valid = id.Left( p ).IsValidIdentifier() && id.Substring( p+2 ).IsValidIdentifier();
         if ( !valid )
            throw Error( "Invalid view identifier: " + id );
      }

      if ( sender == GUI->WhiteReferenceView_Edit )
      {
         instance.whiteReferenceViewId = id;
         sender.SetText( WHITE_REFERENCE_ID );
      }
      else if ( sender == GUI->BackgroundReferenceView_Edit )
      {
         instance.backgroundReferenceViewId = id;
         sender.SetText( BACKGROUND_REFERENCE_ID );
      }
      return;
   }
   catch ( ... )
   {
      if ( sender == GUI->WhiteReferenceView_Edit )
         sender.SetText( WHITE_REFERENCE_ID );
      else if ( sender == GUI->BackgroundReferenceView_Edit )
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

void ColorCalibrationInterface::__EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->WhiteLow_NumericControl )
      instance.whiteLow = value;
   else if ( sender == GUI->WhiteHigh_NumericControl )
      instance.whiteHigh = value;
   else if ( sender == GUI->ManualRedFactor_NumericControl )
      instance.manualRedFactor = value;
   else if ( sender == GUI->ManualGreenFactor_NumericControl )
      instance.manualGreenFactor = value;
   else if ( sender == GUI->ManualBlueFactor_NumericControl )
      instance.manualBlueFactor = value;
   else if ( sender == GUI->BackgroundLow_NumericControl )
      instance.backgroundLow = value;
   else if ( sender == GUI->BackgroundHigh_NumericControl )
      instance.backgroundHigh = value;
}

void ColorCalibrationInterface::__SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->StructureLayers_SpinBox )
      instance.structureLayers = value;
   else if ( sender == GUI->NoiseLayers_SpinBox )
      instance.noiseLayers = value;

   else if ( sender == GUI->WhiteROIX0_SpinBox )
      instance.whiteROI.x0 = value;
   else if ( sender == GUI->WhiteROIY0_SpinBox )
      instance.whiteROI.y0 = value;
   else if ( sender == GUI->WhiteROIWidth_SpinBox )
      instance.whiteROI.x1 = instance.whiteROI.x0 + value;
   else if ( sender == GUI->WhiteROIHeight_SpinBox )
      instance.whiteROI.y1 = instance.whiteROI.y0 + value;

   else if ( sender == GUI->BackgroundROIX0_SpinBox )
      instance.backgroundROI.x0 = value;
   else if ( sender == GUI->BackgroundROIY0_SpinBox )
      instance.backgroundROI.y0 = value;
   else if ( sender == GUI->BackgroundROIWidth_SpinBox )
      instance.backgroundROI.x1 = instance.backgroundROI.x0 + value;
   else if ( sender == GUI->BackgroundROIHeight_SpinBox )
      instance.backgroundROI.y1 = instance.backgroundROI.y0 + value;
}

void ColorCalibrationInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->WhiteReferenceView_ToolButton )
   {
      ViewSelectionDialog d( instance.whiteReferenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         instance.whiteReferenceViewId = d.Id();
         GUI->WhiteReferenceView_Edit.SetText( WHITE_REFERENCE_ID );
      }
   }
   else if ( sender == GUI->BackgroundReferenceView_ToolButton )
   {
      ViewSelectionDialog d( instance.backgroundReferenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         instance.backgroundReferenceViewId = d.Id();
         GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
      }
   }
   else if ( sender == GUI->OutputWhiteReferenceMask_CheckBox )
      instance.outputWhiteReferenceMask = checked;
   else if ( sender == GUI->OutputBackgroundReferenceMask_CheckBox )
      instance.outputBackgroundReferenceMask = checked;
   else if ( sender == GUI->WhiteROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               instance.whiteROI = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
   else if ( sender == GUI->BackgroundROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               instance.backgroundROI = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
}

/*
void ColorCalibrationInterface::__UpdateRealTime_Timer( Timer& sender )
{
   if ( IsRealTimePreviewActive() )
      UpdateRealTimePreview();
}
*/

void ColorCalibrationInterface::__GroupBoxCheck( GroupBox& sender, bool checked )
{
   if ( sender == GUI->WhiteROI_GroupBox )
      instance.whiteUseROI = checked;
   else if ( sender == GUI->StructureDetection_GroupBox )
   {
      instance.structureDetection = checked;
      UpdateControls();
   }
   else if ( sender == GUI->ManualWhiteBalance_GroupBox )
   {
      instance.manualWhiteBalance = checked;
      UpdateControls();
   }
   else if ( sender == GUI->BackgroundROI_GroupBox )
      instance.backgroundUseROI = checked;
}

void ColorCalibrationInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->WhiteReferenceView_Edit || sender == GUI->BackgroundReferenceView_Edit )
      wantsView = true;
   else if ( sender == GUI->WhiteROI_GroupBox || sender == GUI->WhiteROISelectPreview_Button ||
             sender == GUI->BackgroundROI_GroupBox || sender == GUI->BackgroundROISelectPreview_Button )
      wantsView = view.IsPreview();
}

void ColorCalibrationInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->WhiteReferenceView_Edit )
   {
      instance.whiteReferenceViewId = view.FullId();
      GUI->WhiteReferenceView_Edit.SetText( WHITE_REFERENCE_ID );
   }
   else if ( sender == GUI->BackgroundReferenceView_Edit )
   {
      instance.backgroundReferenceViewId = view.FullId();
      GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
   }
   else if ( sender == GUI->WhiteROI_GroupBox || sender == GUI->WhiteROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         instance.whiteUseROI = true;
         instance.whiteROI = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
   else if ( sender == GUI->BackgroundROI_GroupBox || sender == GUI->BackgroundROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         instance.backgroundUseROI = true;
         instance.backgroundROI = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ColorCalibrationInterface::GUIData::GUIData( ColorCalibrationInterface& w )
{
#define DELTA_FRAME  1

   pcl::Font fnt = w.Font();
   int labelWidth1 = Max( fnt.Width( String( "Structure layers:" ) + 'T' ),
                          fnt.Width( String( "Reference image:" ) + 'T' ) );
   int labelWidth2 = fnt.Width( String( "Height:" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   WhiteReference_SectionBar.SetTitle( "White Reference" );
   WhiteReference_SectionBar.SetSection( WhiteReference_Control );

   WhiteReferenceView_Label.SetText( "Reference image:" );
   WhiteReferenceView_Label.SetFixedWidth( labelWidth1 );
   WhiteReferenceView_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   WhiteReferenceView_Edit.SetToolTip( "<p>White reference image.</p>"
      "<p>ColorCalibration will use pixels read from this image to compute a set of three color calibration "
      "factors, one for each channel. If you leave this field blank (or with its default &lt;target image&gt; "
      "value), the target image will be also the white reference image during the calibration process.</p>" );
   WhiteReferenceView_Edit.OnGetFocus( (Control::event_handler)&ColorCalibrationInterface::__GetFocus, w );
   WhiteReferenceView_Edit.OnEditCompleted( (Edit::edit_event_handler)&ColorCalibrationInterface::__EditCompleted, w );
   WhiteReferenceView_Edit.OnViewDrag( (Control::view_drag_event_handler)&ColorCalibrationInterface::__ViewDrag, w );
   WhiteReferenceView_Edit.OnViewDrop( (Control::view_drop_event_handler)&ColorCalibrationInterface::__ViewDrop, w );

   WhiteReferenceView_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   WhiteReferenceView_ToolButton.SetScaledFixedSize( 20, 20 );
   WhiteReferenceView_ToolButton.SetToolTip( "<p>Select the white reference image</p>" );
   WhiteReferenceView_ToolButton.OnClick( (Button::click_event_handler)&ColorCalibrationInterface::__Click, w );

   WhiteReferenceView_Sizer.SetSpacing( 4 );
   WhiteReferenceView_Sizer.Add( WhiteReferenceView_Label );
   WhiteReferenceView_Sizer.Add( WhiteReferenceView_Edit );
   WhiteReferenceView_Sizer.Add( WhiteReferenceView_ToolButton );

   WhiteLow_NumericControl.label.SetText( "Lower limit:" );
   WhiteLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   WhiteLow_NumericControl.slider.SetRange( 0, 100 );
   WhiteLow_NumericControl.slider.SetScaledMinWidth( 200 );
   WhiteLow_NumericControl.SetReal();
   WhiteLow_NumericControl.SetRange( TheCCWhiteLowParameter->MinimumValue(), TheCCWhiteLowParameter->MaximumValue() );
   WhiteLow_NumericControl.SetPrecision( TheCCWhiteLowParameter->Precision() );
   WhiteLow_NumericControl.edit.SetFixedWidth( editWidth1 );
   WhiteLow_NumericControl.SetToolTip( "<p>Lower bound of the set of white reference pixels. White reference "
      "pixels less than or equal to this value will be rejected for calculation of color correction factors. "
      "Note that since the minimum allowed value for this parameter is 0, black pixels are always rejected.</p>" );
   WhiteLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorCalibrationInterface::__EditValueUpdated, w );

   WhiteHigh_NumericControl.label.SetText( "Upper limit:" );
   WhiteHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   WhiteHigh_NumericControl.slider.SetRange( 0, 100 );
   WhiteHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   WhiteHigh_NumericControl.SetReal();
   WhiteHigh_NumericControl.SetRange( TheCCWhiteHighParameter->MinimumValue(), TheCCWhiteHighParameter->MaximumValue() );
   WhiteHigh_NumericControl.SetPrecision( TheCCWhiteHighParameter->Precision() );
   WhiteHigh_NumericControl.edit.SetFixedWidth( editWidth1 );
   WhiteHigh_NumericControl.SetToolTip( "<p>Upper bound of the set of white reference pixels. White reference "
      "pixels greater than or equal to this value will be rejected for calculation of color correction factors. "
      "This parameter allows you to reject saturated pixels, or pixels with very high values pertaining to the "
      "nonlinear regions of most CCD response curves. Note that since the maximum allowed value for this parameter "
      "is 1, white pixels are always rejected.</p>" );
   WhiteHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorCalibrationInterface::__EditValueUpdated, w );

   //

   WhiteROI_GroupBox.SetTitle( "Region of Interest" );
   WhiteROI_GroupBox.EnableTitleCheckBox();
   WhiteROI_GroupBox.OnCheck( (GroupBox::check_event_handler)&ColorCalibrationInterface::__GroupBoxCheck, w );
   WhiteROI_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&ColorCalibrationInterface::__ViewDrag, w );
   WhiteROI_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&ColorCalibrationInterface::__ViewDrop, w );

   const char* whiteROIX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI, white reference.</p>";

   WhiteROIX0_Label.SetText( "Left:" );
   WhiteROIX0_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   WhiteROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WhiteROIX0_Label.SetToolTip( whiteROIX0ToolTip );

   WhiteROIX0_SpinBox.SetRange( 0, int_max );
   WhiteROIX0_SpinBox.SetToolTip( whiteROIX0ToolTip );
   WhiteROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   const char* whiteROIY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI, white reference.</p>";

   WhiteROIY0_Label.SetText( "Top:" );
   WhiteROIY0_Label.SetFixedWidth( labelWidth2 );
   WhiteROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WhiteROIY0_Label.SetToolTip( whiteROIY0ToolTip );

   WhiteROIY0_SpinBox.SetRange( 0, int_max );
   WhiteROIY0_SpinBox.SetToolTip( whiteROIY0ToolTip );
   WhiteROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   WhiteROIRow1_Sizer.SetSpacing( 4 );
   WhiteROIRow1_Sizer.Add( WhiteROIX0_Label );
   WhiteROIRow1_Sizer.Add( WhiteROIX0_SpinBox );
   WhiteROIRow1_Sizer.Add( WhiteROIY0_Label );
   WhiteROIRow1_Sizer.Add( WhiteROIY0_SpinBox );
   WhiteROIRow1_Sizer.AddStretch();

   const char* whiteROIWidthToolTip = "<p>Width of the ROI in pixels, white reference.</p>";

   WhiteROIWidth_Label.SetText( "Width:" );
   WhiteROIWidth_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   WhiteROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WhiteROIWidth_Label.SetToolTip( whiteROIWidthToolTip );

   WhiteROIWidth_SpinBox.SetRange( 0, int_max );
   WhiteROIWidth_SpinBox.SetToolTip( whiteROIWidthToolTip );
   WhiteROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   const char* whiteROIHeightToolTip = "<p>Height of the ROI in pixels, white reference.</p>";

   WhiteROIHeight_Label.SetText( "Height:" );
   WhiteROIHeight_Label.SetFixedWidth( labelWidth2 );
   WhiteROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WhiteROIHeight_Label.SetToolTip( whiteROIHeightToolTip );

   WhiteROIHeight_SpinBox.SetRange( 0, int_max );
   WhiteROIHeight_SpinBox.SetToolTip( whiteROIHeightToolTip );
   WhiteROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   WhiteROISelectPreview_Button.SetText( "From Preview" );
   WhiteROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview, white reference.</p>" );
   WhiteROISelectPreview_Button.OnClick( (Button::click_event_handler)&ColorCalibrationInterface::__Click, w );
   WhiteROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&ColorCalibrationInterface::__ViewDrag, w );
   WhiteROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&ColorCalibrationInterface::__ViewDrop, w );

   WhiteROIRow2_Sizer.SetSpacing( 4 );
   WhiteROIRow2_Sizer.Add( WhiteROIWidth_Label );
   WhiteROIRow2_Sizer.Add( WhiteROIWidth_SpinBox );
   WhiteROIRow2_Sizer.Add( WhiteROIHeight_Label );
   WhiteROIRow2_Sizer.Add( WhiteROIHeight_SpinBox );
   WhiteROIRow2_Sizer.AddSpacing( 12 );
   WhiteROIRow2_Sizer.Add( WhiteROISelectPreview_Button );
   WhiteROIRow2_Sizer.AddStretch();

   WhiteROI_Sizer.SetMargin( 6 );
   WhiteROI_Sizer.SetSpacing( 4 );
   WhiteROI_Sizer.Add( WhiteROIRow1_Sizer );
   WhiteROI_Sizer.Add( WhiteROIRow2_Sizer );

   WhiteROI_GroupBox.SetSizer( WhiteROI_Sizer );

   //

   StructureDetection_GroupBox.SetTitle( "Structure Detection" );
   StructureDetection_GroupBox.EnableTitleCheckBox();
   StructureDetection_GroupBox.OnCheck( (GroupBox::check_event_handler)&ColorCalibrationInterface::__GroupBoxCheck, w );
   StructureDetection_GroupBox.SetToolTip( "<p>Detect significant structures at small dimensional scales prior to "
      "evaluation of color calibration factors.</p>"
      "<p>When this option is selected, ColorCalibration uses a multiscale, wavelet-based structure detection "
      "routine to isolate bright image structures within a specified range of dimensional scales (see the next two "
      "parameters). This feature can be used to perform a color calibration based on the stars recorded in the white "
      "reference image.</p>" );

   const char* structureLayersTip =
      "<p>Number of small-scale wavelet layers used for structure detection.</p>"
      "<p>More layers will use larger image structures for calculation of color calibration factors.</p>";

   StructureLayers_Label.SetText( "Structure layers:" );
   StructureLayers_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   StructureLayers_Label.SetToolTip( structureLayersTip );
   StructureLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StructureLayers_SpinBox.SetRange( int( TheCCStructureLayersParameter->MinimumValue() ), int( TheCCStructureLayersParameter->MaximumValue() ) );
   StructureLayers_SpinBox.SetFixedWidth( editWidth1 );
   StructureLayers_SpinBox.SetToolTip( structureLayersTip );
   StructureLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   StructureLayers_Sizer.SetSpacing( 4 );
   StructureLayers_Sizer.Add( StructureLayers_Label );
   StructureLayers_Sizer.Add( StructureLayers_SpinBox );
   StructureLayers_Sizer.AddStretch();

   const char* noiseLayersTip =
      "<p>Number of wavelet layers used for noise reduction.</p>"
      "<p>Noise reduction prevents detection of bright noise structures, including hot pixels and cosmic rays. "
      "This parameter can also be used to control the sizes of the smallest detected stars (increase "
      "to exclude more stars).</p>";

   NoiseLayers_Label.SetText( "Noise layers:" );
   NoiseLayers_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   NoiseLayers_Label.SetToolTip( noiseLayersTip );
   NoiseLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   NoiseLayers_SpinBox.SetRange( int( TheCCNoiseLayersParameter->MinimumValue() ), int( TheCCNoiseLayersParameter->MaximumValue() ) );
   NoiseLayers_SpinBox.SetFixedWidth( editWidth1 );
   NoiseLayers_SpinBox.SetToolTip( noiseLayersTip );
   NoiseLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   NoiseLayers_Sizer.SetSpacing( 4 );
   NoiseLayers_Sizer.Add( NoiseLayers_Label );
   NoiseLayers_Sizer.Add( NoiseLayers_SpinBox );
   NoiseLayers_Sizer.AddStretch();

   StructureDetection_Sizer.SetMargin( 6 );
   StructureDetection_Sizer.SetSpacing( 4 );
   StructureDetection_Sizer.Add( StructureLayers_Sizer );
   StructureDetection_Sizer.Add( NoiseLayers_Sizer );

   StructureDetection_GroupBox.SetSizer( StructureDetection_Sizer );

   //

   ManualWhiteBalance_GroupBox.SetTitle( "Manual White Balance" );
   ManualWhiteBalance_GroupBox.EnableTitleCheckBox();
   ManualWhiteBalance_GroupBox.OnCheck( (GroupBox::check_event_handler)&ColorCalibrationInterface::__GroupBoxCheck, w );
   ManualWhiteBalance_GroupBox.SetToolTip( "<p>Perform a manual white balance by specifying the three color "
      "correction factors literally.</p>"
      "<p>If you select this option, no automatic color calibration routine will be applied, and you'll "
      "be allowed to enter the correction factors for red, green and blue, as the next three parameters.</p>" );

   ManualRedFactor_NumericControl.label.SetText( "Red:" );
   ManualRedFactor_NumericControl.label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   ManualRedFactor_NumericControl.slider.SetRange( 0, 100 );
   ManualRedFactor_NumericControl.slider.SetScaledMinWidth( 200 );
   ManualRedFactor_NumericControl.SetReal();
   ManualRedFactor_NumericControl.SetRange( 0, 1 );
   ManualRedFactor_NumericControl.SetPrecision( TheCCManualRedFactorParameter->Precision() );
   ManualRedFactor_NumericControl.edit.SetFixedWidth( editWidth1 );
   ManualRedFactor_NumericControl.SetToolTip( "<p>Color correction factor, red channel.</p>" );
   ManualRedFactor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorCalibrationInterface::__EditValueUpdated, w );

   ManualGreenFactor_NumericControl.label.SetText( "Green:" );
   ManualGreenFactor_NumericControl.label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   ManualGreenFactor_NumericControl.slider.SetRange( 0, 100 );
   ManualGreenFactor_NumericControl.slider.SetScaledMinWidth( 200 );
   ManualGreenFactor_NumericControl.SetReal();
   ManualGreenFactor_NumericControl.SetRange( 0, 1 );
   ManualGreenFactor_NumericControl.SetPrecision( TheCCManualGreenFactorParameter->Precision() );
   ManualGreenFactor_NumericControl.edit.SetFixedWidth( editWidth1 );
   ManualGreenFactor_NumericControl.SetToolTip( "<p>Color correction factor, green channel.</p>" );
   ManualGreenFactor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorCalibrationInterface::__EditValueUpdated, w );

   ManualBlueFactor_NumericControl.label.SetText( "Blue:" );
   ManualBlueFactor_NumericControl.label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   ManualBlueFactor_NumericControl.slider.SetRange( 0, 100 );
   ManualBlueFactor_NumericControl.slider.SetScaledMinWidth( 200 );
   ManualBlueFactor_NumericControl.SetReal();
   ManualBlueFactor_NumericControl.SetRange( 0, 1 );
   ManualBlueFactor_NumericControl.SetPrecision( TheCCManualBlueFactorParameter->Precision() );
   ManualBlueFactor_NumericControl.edit.SetFixedWidth( editWidth1 );
   ManualBlueFactor_NumericControl.SetToolTip( "<p>Color correction factor, blue channel.</p>" );
   ManualBlueFactor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorCalibrationInterface::__EditValueUpdated, w );

   ManualWhiteBalance_Sizer.SetMargin( 6 );
   ManualWhiteBalance_Sizer.SetSpacing( 4 );
   ManualWhiteBalance_Sizer.Add( ManualRedFactor_NumericControl );
   ManualWhiteBalance_Sizer.Add( ManualGreenFactor_NumericControl );
   ManualWhiteBalance_Sizer.Add( ManualBlueFactor_NumericControl );

   ManualWhiteBalance_GroupBox.SetSizer( ManualWhiteBalance_Sizer );

   //

   OutputWhiteReferenceMask_CheckBox.SetText( "Output white reference mask" );
   OutputWhiteReferenceMask_CheckBox.SetToolTip( "<p>If this option is selected, ColorCalibration will "
      "create a new image window with a <i>white reference mask</i>.</p>"
      "<p>A white reference mask is white for pixels in the white reference image that have been used to "
      "calculate color correction factors, black anywhere else. You can use this mask to check whether the "
      "<i>White pixels</i> and <i>Saturation point</i> parameters are doing a good job selecting the "
      "pixels that you intend to use as a white reference.</p>" );
   OutputWhiteReferenceMask_CheckBox.OnClick( (Button::click_event_handler)&ColorCalibrationInterface::__Click, w );

   OutputWhiteReferenceMask_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputWhiteReferenceMask_Sizer.Add( OutputWhiteReferenceMask_CheckBox );
   OutputWhiteReferenceMask_Sizer.AddStretch();

   WhiteReference_Sizer.SetSpacing( 4 );
   WhiteReference_Sizer.Add( WhiteReferenceView_Sizer );
   WhiteReference_Sizer.Add( WhiteLow_NumericControl );
   WhiteReference_Sizer.Add( WhiteHigh_NumericControl );
   WhiteReference_Sizer.Add( WhiteROI_GroupBox );
   WhiteReference_Sizer.Add( StructureDetection_GroupBox );
   WhiteReference_Sizer.Add( ManualWhiteBalance_GroupBox );
   WhiteReference_Sizer.Add( OutputWhiteReferenceMask_Sizer );

   WhiteReference_Control.SetSizer( WhiteReference_Sizer );

   //

   BackgroundReference_SectionBar.SetTitle( "Background Reference" );
   BackgroundReference_SectionBar.SetSection( BackgroundReference_Control );

   BackgroundReferenceView_Label.SetText( "Reference image:" );
   BackgroundReferenceView_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceView_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceView_Edit.SetToolTip( "<p>Background reference image.</p>"
      "<p>ColorCalibration will use pixels read from this image to compute an initial mean background level for "
      "each color channel. If you leave this field blank (or with its default &lt;target image&gt; value), the "
      "target image will be also the background reference image during the color calibration process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>" );
   BackgroundReferenceView_Edit.OnGetFocus( (Control::event_handler)&ColorCalibrationInterface::__GetFocus, w );
   BackgroundReferenceView_Edit.OnEditCompleted( (Edit::edit_event_handler)&ColorCalibrationInterface::__EditCompleted, w );
   BackgroundReferenceView_Edit.OnViewDrag( (Control::view_drag_event_handler)&ColorCalibrationInterface::__ViewDrag, w );
   BackgroundReferenceView_Edit.OnViewDrop( (Control::view_drop_event_handler)&ColorCalibrationInterface::__ViewDrop, w );

   BackgroundReferenceView_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceView_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceView_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceView_ToolButton.OnClick( (Button::click_event_handler)&ColorCalibrationInterface::__Click, w );

   BackgroundReferenceView_Sizer.SetSpacing( 4 );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Label );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Edit );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_ToolButton );

   BackgroundLow_NumericControl.label.SetText( "Lower limit:" );
   BackgroundLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundLow_NumericControl.slider.SetRange( 0, 100 );
   BackgroundLow_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundLow_NumericControl.SetReal();
   BackgroundLow_NumericControl.SetRange( TheCCBackgroundLowParameter->MinimumValue(), TheCCBackgroundLowParameter->MaximumValue() );
   BackgroundLow_NumericControl.SetPrecision( TheCCBackgroundLowParameter->Precision() );
   BackgroundLow_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundLow_NumericControl.SetToolTip( "<p>Lower bound of the set of background pixels. Background reference "
      "pixels below this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorCalibrationInterface::__EditValueUpdated, w );

   BackgroundHigh_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh_NumericControl.slider.SetRange( 0, 100 );
   BackgroundHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundHigh_NumericControl.SetReal();
   BackgroundHigh_NumericControl.SetRange( TheCCBackgroundHighParameter->MinimumValue(), TheCCBackgroundHighParameter->MaximumValue() );
   BackgroundHigh_NumericControl.SetPrecision( TheCCBackgroundHighParameter->Precision() );
   BackgroundHigh_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundHigh_NumericControl.SetToolTip( "<p>Upper bound of the set of background pixels. Background reference "
      "pixels above this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorCalibrationInterface::__EditValueUpdated, w );

   //

   BackgroundROI_GroupBox.SetTitle( "Region of Interest" );
   BackgroundROI_GroupBox.EnableTitleCheckBox();
   BackgroundROI_GroupBox.OnCheck( (GroupBox::check_event_handler)&ColorCalibrationInterface::__GroupBoxCheck, w );
   BackgroundROI_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&ColorCalibrationInterface::__ViewDrag, w );
   BackgroundROI_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&ColorCalibrationInterface::__ViewDrop, w );

   const char* backgroundROIX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI, background reference.</p>";

   BackgroundROIX0_Label.SetText( "Left:" );
   BackgroundROIX0_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   BackgroundROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIX0_Label.SetToolTip( backgroundROIX0ToolTip );

   BackgroundROIX0_SpinBox.SetRange( 0, int_max );
   BackgroundROIX0_SpinBox.SetToolTip( backgroundROIX0ToolTip );
   BackgroundROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   const char* backgroundROIY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI, background reference.</p>";

   BackgroundROIY0_Label.SetText( "Top:" );
   BackgroundROIY0_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIY0_Label.SetToolTip( backgroundROIY0ToolTip );

   BackgroundROIY0_SpinBox.SetRange( 0, int_max );
   BackgroundROIY0_SpinBox.SetToolTip( backgroundROIY0ToolTip );
   BackgroundROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   BackgroundROIRow1_Sizer.SetSpacing( 4 );
   BackgroundROIRow1_Sizer.Add( BackgroundROIX0_Label );
   BackgroundROIRow1_Sizer.Add( BackgroundROIX0_SpinBox );
   BackgroundROIRow1_Sizer.Add( BackgroundROIY0_Label );
   BackgroundROIRow1_Sizer.Add( BackgroundROIY0_SpinBox );
   BackgroundROIRow1_Sizer.AddStretch();

   const char* backgroundROIWidthToolTip = "<p>Width of the ROI in pixels, background reference.</p>";

   BackgroundROIWidth_Label.SetText( "Width:" );
   BackgroundROIWidth_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   BackgroundROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIWidth_Label.SetToolTip( backgroundROIWidthToolTip );

   BackgroundROIWidth_SpinBox.SetRange( 0, int_max );
   BackgroundROIWidth_SpinBox.SetToolTip( backgroundROIWidthToolTip );
   BackgroundROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   const char* backgroundROIHeightToolTip = "<p>Height of the ROI in pixels, background reference.</p>";

   BackgroundROIHeight_Label.SetText( "Height:" );
   BackgroundROIHeight_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIHeight_Label.SetToolTip( backgroundROIHeightToolTip );

   BackgroundROIHeight_SpinBox.SetRange( 0, int_max );
   BackgroundROIHeight_SpinBox.SetToolTip( backgroundROIHeightToolTip );
   BackgroundROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorCalibrationInterface::__SpinValueUpdated, w );

   BackgroundROISelectPreview_Button.SetText( "From Preview" );
   BackgroundROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview, background reference.</p>" );
   BackgroundROISelectPreview_Button.OnClick( (Button::click_event_handler)&ColorCalibrationInterface::__Click, w );
   BackgroundROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&ColorCalibrationInterface::__ViewDrag, w );
   BackgroundROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&ColorCalibrationInterface::__ViewDrop, w );

   BackgroundROIRow2_Sizer.SetSpacing( 4 );
   BackgroundROIRow2_Sizer.Add( BackgroundROIWidth_Label );
   BackgroundROIRow2_Sizer.Add( BackgroundROIWidth_SpinBox );
   BackgroundROIRow2_Sizer.Add( BackgroundROIHeight_Label );
   BackgroundROIRow2_Sizer.Add( BackgroundROIHeight_SpinBox );
   BackgroundROIRow2_Sizer.AddSpacing( 12 );
   BackgroundROIRow2_Sizer.Add( BackgroundROISelectPreview_Button );
   BackgroundROIRow2_Sizer.AddStretch();

   BackgroundROI_Sizer.SetMargin( 6 );
   BackgroundROI_Sizer.SetSpacing( 4 );
   BackgroundROI_Sizer.Add( BackgroundROIRow1_Sizer );
   BackgroundROI_Sizer.Add( BackgroundROIRow2_Sizer );

   BackgroundROI_GroupBox.SetSizer( BackgroundROI_Sizer );

   //

   OutputBackgroundReferenceMask_CheckBox.SetText( "Output background reference mask" );
   OutputBackgroundReferenceMask_CheckBox.SetToolTip( "<p>If this option is selected, ColorCalibration will "
      "create a new image window with a <i>background reference mask</i>.</p>"
      "<p>A background reference mask is white for pixels in the background reference image that have been "
      "used to calculate mean background levels, black anywhere else. You can use this mask to check whether the "
      "<i>Lower limit</i> and <i>Upper limit</i> parameters define a suitable range of values to select the "
      "pixels that you intend to use as background reference.</p>" );
   OutputBackgroundReferenceMask_CheckBox.OnClick( (Button::click_event_handler)&ColorCalibrationInterface::__Click, w );

   OutputBackgroundReferenceMask_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputBackgroundReferenceMask_Sizer.Add( OutputBackgroundReferenceMask_CheckBox );
   OutputBackgroundReferenceMask_Sizer.AddStretch();

   //

   BackgroundReference_Sizer.SetSpacing( 4 );
   BackgroundReference_Sizer.Add( BackgroundReferenceView_Sizer );
   BackgroundReference_Sizer.Add( BackgroundLow_NumericControl );
   BackgroundReference_Sizer.Add( BackgroundHigh_NumericControl );
   BackgroundReference_Sizer.Add( BackgroundROI_GroupBox );
   BackgroundReference_Sizer.Add( OutputBackgroundReferenceMask_Sizer );

   BackgroundReference_Control.SetSizer( BackgroundReference_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( WhiteReference_SectionBar );
   Global_Sizer.Add( WhiteReference_Control );
   Global_Sizer.Add( BackgroundReference_SectionBar );
   Global_Sizer.Add( BackgroundReference_Control );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   /*
   UpdateRealTime_Timer.SetSingleShot();
   UpdateRealTime_Timer.SetInterval( 0.040 );
   UpdateRealTime_Timer.OnTimer( (Timer::timer_event_handler)&ColorCalibrationInterface::__UpdateRealTime_Timer, w );
   */
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorCalibrationInterface.cpp - Released 2019-01-21T12:06:41Z
