//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0364
// ----------------------------------------------------------------------------
// MaskedStretchInterface.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "MaskedStretchInterface.h"
#include "MaskedStretchParameters.h"
#include "MaskedStretchProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ImageWindow.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MaskedStretchInterface* TheMaskedStretchInterface = nullptr;

// ----------------------------------------------------------------------------

#include "MaskedStretchIcon.xpm"

// ----------------------------------------------------------------------------

MaskedStretchInterface::MaskedStretchInterface() :
   m_instance( TheMaskedStretchProcess )
{
   TheMaskedStretchInterface = this;
}

MaskedStretchInterface::~MaskedStretchInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString MaskedStretchInterface::Id() const
{
   return "MaskedStretch";
}

MetaProcess* MaskedStretchInterface::Process() const
{
   return TheMaskedStretchProcess;
}

const char** MaskedStretchInterface::IconImageXPM() const
{
   return MaskedStretchIcon_XPM;
}

void MaskedStretchInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

void MaskedStretchInterface::ResetInstance()
{
   MaskedStretchInstance defaultInstance( TheMaskedStretchProcess );
   ImportProcess( defaultInstance );
}

bool MaskedStretchInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "MaskedStretch" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheMaskedStretchProcess;
}

ProcessImplementation* MaskedStretchInterface::NewProcess() const
{
   return new MaskedStretchInstance( m_instance );
}

bool MaskedStretchInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const MaskedStretchInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a MaskedStretch instance.";
   return false;
}

bool MaskedStretchInterface::RequiresInstanceValidation() const
{
   return true;
}

bool MaskedStretchInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

#define TARGET_IMAGE             String( "<target image>" )

#define REFERENCE_ID( x )        (x.IsEmpty() ? TARGET_IMAGE : x)

#define BACKGROUND_REFERENCE_ID  REFERENCE_ID( m_instance.p_backgroundReferenceViewId )

void MaskedStretchInterface::UpdateControls()
{
   GUI->TargetBackground_NumericControl.SetValue( m_instance.p_targetBackground );
   GUI->NumberOfIterations_SpinBox.SetValue( m_instance.p_numberOfIterations );
   GUI->ClippingFraction_NumericEdit.SetValue( m_instance.p_clippingFraction );
   GUI->MaskType_ComboBox.SetCurrentItem( m_instance.p_maskType );
   GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
   GUI->BackgroundLow_NumericControl.SetValue( m_instance.p_backgroundLow );
   GUI->BackgroundHigh_NumericControl.SetValue( m_instance.p_backgroundHigh );
   GUI->ROI_SectionBar.SetChecked( m_instance.p_useROI );
   GUI->ROIX0_SpinBox.SetValue( m_instance.p_roi.x0 );
   GUI->ROIY0_SpinBox.SetValue( m_instance.p_roi.y0 );
   GUI->ROIWidth_SpinBox.SetValue( m_instance.p_roi.Width() );
   GUI->ROIHeight_SpinBox.SetValue( m_instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------

void MaskedStretchInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->TargetBackground_NumericControl )
      m_instance.p_targetBackground = value;
   else if ( sender == GUI->ClippingFraction_NumericEdit )
      m_instance.p_clippingFraction = value;
   else if ( sender == GUI->BackgroundLow_NumericControl )
      m_instance.p_backgroundLow = value;
   else if ( sender == GUI->BackgroundHigh_NumericControl )
      m_instance.p_backgroundHigh = value;
}

void MaskedStretchInterface::__SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->NumberOfIterations_SpinBox )
      m_instance.p_numberOfIterations = value;
   else if ( sender == GUI->ROIX0_SpinBox )
      m_instance.p_roi.x0 = value;
   else if ( sender == GUI->ROIY0_SpinBox )
      m_instance.p_roi.y0 = value;
   else if ( sender == GUI->ROIWidth_SpinBox )
      m_instance.p_roi.x1 = m_instance.p_roi.x0 + value;
   else if ( sender == GUI->ROIHeight_SpinBox )
      m_instance.p_roi.y1 = m_instance.p_roi.y0 + value;
}

void MaskedStretchInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->MaskType_ComboBox )
      m_instance.p_maskType = itemIndex;
}

void MaskedStretchInterface::__GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != nullptr )
      if ( e->Text() == TARGET_IMAGE )
         e->Clear();
}

void MaskedStretchInterface::__EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text().Trimmed();
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
            throw Error( "Invalid identifier: " + id );
      }

      if ( sender == GUI->BackgroundReferenceView_Edit )
      {
         m_instance.p_backgroundReferenceViewId = id;
         sender.SetText( BACKGROUND_REFERENCE_ID );
      }

      return;
   }
   catch ( ... )
   {
      if ( sender == GUI->BackgroundReferenceView_Edit )
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

void MaskedStretchInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->BackgroundReferenceView_ToolButton )
   {
      ViewSelectionDialog d( m_instance.p_backgroundReferenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         m_instance.p_backgroundReferenceViewId = d.Id();
         GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
      }
   }
   else if ( sender == GUI->ROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               m_instance.p_roi = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
   else if ( sender == GUI->ClippingFractionClear_ToolButton )
   {
      if ( m_instance.p_clippingFraction != 0 )
         m_instance.p_clippingFraction = 0;
      else
         m_instance.p_clippingFraction = TheMSClippingFractionParameter->DefaultValue();

      GUI->ClippingFraction_NumericEdit.SetValue( m_instance.p_clippingFraction );
   }
}

void MaskedStretchInterface::__SectionBarCheck( SectionBar& sender, bool checked )
{
   if ( sender == GUI->ROI_SectionBar )
      m_instance.p_useROI = checked;
}

void MaskedStretchInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->BackgroundReferenceView_Edit )
      wantsView = true;
   else if ( sender == GUI->ROI_SectionBar || sender == GUI->ROI_Control || sender == GUI->ROISelectPreview_Button )
      wantsView = view.IsPreview();
}

void MaskedStretchInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->BackgroundReferenceView_Edit )
   {
      m_instance.p_backgroundReferenceViewId = view.FullId();
      GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
   }
   else if ( sender == GUI->ROI_SectionBar || sender == GUI->ROI_Control || sender == GUI->ROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         m_instance.p_useROI = true;
         m_instance.p_roi = view.Window().PreviewRect( view.Id() );
         GUI->ROI_SectionBar.ShowSection();
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

MaskedStretchInterface::GUIData::GUIData( MaskedStretchInterface& w )
{
#define DELTA_FRAME  1

   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Background reference:" ) + 'T' );
   int labelWidth2 = fnt.Width( String( "Height:" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 13 ) );

   //

   const char* targetBackgroundToolTip =
   "<p>The mean background value of the reference background image (or region) after the nonlinear masked stretch process.</p>";

   TargetBackground_NumericControl.label.SetText( "Target background:" );
   TargetBackground_NumericControl.label.SetMinWidth( labelWidth1 );
   TargetBackground_NumericControl.slider.SetRange( 0, 200 );
   TargetBackground_NumericControl.slider.SetScaledMinWidth( 350 );
   TargetBackground_NumericControl.SetReal();
   TargetBackground_NumericControl.SetToolTip( targetBackgroundToolTip );
   TargetBackground_NumericControl.SetRange( TheMSTargetBackgroundParameter->MinimumValue(), TheMSTargetBackgroundParameter->MaximumValue() );
   TargetBackground_NumericControl.SetPrecision( TheMSTargetBackgroundParameter->Precision() );
   TargetBackground_NumericControl.edit.SetFixedWidth( editWidth1 );
   TargetBackground_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&MaskedStretchInterface::__ValueUpdated, w );

   //

   const char* numberOfIterationsToolTip =
   "<p>Number of masked stretch iterations.</p>"
   "<p>In most cases, a moderate number of iterations between 50 and 100 will lead to a smooth result with excellent protection of "
   "bright image features. With too few iterations, the MS algorithm may create artifacts around the brightest objects. In general, "
   "the largest the difference between the initial and final mean background values, the more iterations are necessary to compute "
   "accurate protection masks. For this reason, HDR images usually require more iterations. The default value is 100 iterations, "
   "which are more than sufficient for most nonlinear deep-sky images. You can reduce this value to speed up the process during "
   "trial-error work. If you get dark halos around bright stars, this means that more iterations are necessary. The maximum allowed "
   "is 1000 iterations.</p>";

   NumberOfIterations_Label.SetText( "Iterations:" );
   NumberOfIterations_Label.SetMinWidth( labelWidth1 );
   NumberOfIterations_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NumberOfIterations_Label.SetToolTip( numberOfIterationsToolTip );

   NumberOfIterations_SpinBox.SetRange( int( TheMSNumberOfIterationsParameter->MinimumValue() ),
                                        int( TheMSNumberOfIterationsParameter->MaximumValue() ) );
   NumberOfIterations_SpinBox.SetToolTip( numberOfIterationsToolTip );
   NumberOfIterations_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&MaskedStretchInterface::__SpinValueUpdated, w );

   NumberOfIterations_Sizer.SetSpacing( 4 );
   NumberOfIterations_Sizer.Add( NumberOfIterations_Label );
   NumberOfIterations_Sizer.Add( NumberOfIterations_SpinBox );
   NumberOfIterations_Sizer.AddStretch();

   //

   const char* clippingFractionToolTip =
   "<p>For optimal results, the masked stretch algorithm requires full coverage of the dynamic range at the shadows, that is, "
   "no unused section at the left side of the main histogram peak. This is the fraction of dark pixels that will be clipped "
   "before the masked stretch process. Specify zero to disable the shadows auto clipping feature.</p>";

   ClippingFraction_NumericEdit.label.SetText( "Clipping fraction:" );
   ClippingFraction_NumericEdit.label.SetMinWidth( labelWidth1 );
   ClippingFraction_NumericEdit.SetReal();
   ClippingFraction_NumericEdit.SetToolTip( clippingFractionToolTip );
   ClippingFraction_NumericEdit.SetRange( TheMSClippingFractionParameter->MinimumValue(), TheMSClippingFractionParameter->MaximumValue() );
   ClippingFraction_NumericEdit.SetPrecision( TheMSClippingFractionParameter->Precision() );
   ClippingFraction_NumericEdit.edit.SetFixedWidth( editWidth1 );
   ClippingFraction_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&MaskedStretchInterface::__ValueUpdated, w );

   ClippingFractionClear_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/clear.png" ) ) );
   ClippingFractionClear_ToolButton.SetScaledFixedSize( 20, 20 );
   ClippingFractionClear_ToolButton.SetToolTip( "<p>Disable/reset auto clipping.</p>" );
   ClippingFractionClear_ToolButton.OnClick( (Button::click_event_handler)&MaskedStretchInterface::__Click, w );

   ClippingFraction_Sizer.SetSpacing( 4 );
   ClippingFraction_Sizer.Add( ClippingFraction_NumericEdit );
   ClippingFraction_Sizer.Add( ClippingFractionClear_ToolButton );
   ClippingFraction_Sizer.AddStretch();

   //

   const char* maskTypeToolTip =
   "<p>Type of the mask used to protect the highlights of color images."
   "<p>Two options are available: the intensity (I) component of the HSI color ordering system, "
   "and the value (V) component of HSV.</p>"
   "<p>For grayscale images the mask is always generated from a duplicate of the image at each iteration, "
   "and hence this parameter is not used.</p>";

   MaskType_Label.SetText( "Color mask type:" );
   MaskType_Label.SetMinWidth( labelWidth1 );
   MaskType_Label.SetToolTip( maskTypeToolTip );
   MaskType_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   MaskType_ComboBox.AddItem( "HSI Intensity" );
   MaskType_ComboBox.AddItem( "HSV Value" );
   MaskType_ComboBox.SetToolTip( maskTypeToolTip );
   MaskType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&MaskedStretchInterface::__ItemSelected, w );

   MaskType_Sizer.SetSpacing( 4 );
   MaskType_Sizer.Add( MaskType_Label );
   MaskType_Sizer.Add( MaskType_ComboBox );
   MaskType_Sizer.AddStretch();

   //

   BackgroundReferenceView_Label.SetText( "Background reference:" );
   BackgroundReferenceView_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceView_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceView_Edit.SetToolTip( "<p>MaskedStretch will use pixels read from this image to "
      "compute an initial mean background level. If you leave this field blank (or with its default "
      "&lt;target image&gt; value), the target image will be also the background reference image for "
      "the masked stretch process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>" );
   BackgroundReferenceView_Edit.OnGetFocus( (Control::event_handler)&MaskedStretchInterface::__GetFocus, w );
   BackgroundReferenceView_Edit.OnEditCompleted( (Edit::edit_event_handler)&MaskedStretchInterface::__EditCompleted, w );
   BackgroundReferenceView_Edit.OnViewDrag( (Control::view_drag_event_handler)&MaskedStretchInterface::__ViewDrag, w );
   BackgroundReferenceView_Edit.OnViewDrop( (Control::view_drop_event_handler)&MaskedStretchInterface::__ViewDrop, w );

   BackgroundReferenceView_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceView_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceView_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceView_ToolButton.OnClick( (Button::click_event_handler)&MaskedStretchInterface::__Click, w );

   BackgroundReferenceView_Sizer.SetSpacing( 4 );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Label );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Edit );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_ToolButton );

   //

   BackgroundLow_NumericControl.label.SetText( "Lower limit:" );
   BackgroundLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundLow_NumericControl.slider.SetRange( 0, 100 );
   BackgroundLow_NumericControl.slider.SetScaledMinWidth( 350 );
   BackgroundLow_NumericControl.SetReal();
   BackgroundLow_NumericControl.SetRange( TheMSBackgroundLowParameter->MinimumValue(), TheMSBackgroundLowParameter->MaximumValue() );
   BackgroundLow_NumericControl.SetPrecision( TheMSBackgroundLowParameter->Precision() );
   BackgroundLow_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundLow_NumericControl.SetToolTip( "<p>Lower bound of the set of background pixels. Background reference "
      "pixels with values less than or equal to this limit will be rejected for calculation of the initial mean background. "
      "Note that since the minimum allowed value for this parameter is zero, black pixels are never taken into account.</p>" );
   BackgroundLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&MaskedStretchInterface::__ValueUpdated, w );

   BackgroundHigh_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh_NumericControl.slider.SetRange( 0, 100 );
   BackgroundHigh_NumericControl.slider.SetScaledMinWidth( 350 );
   BackgroundHigh_NumericControl.SetReal();
   BackgroundHigh_NumericControl.SetRange( TheMSBackgroundHighParameter->MinimumValue(), TheMSBackgroundHighParameter->MaximumValue() );
   BackgroundHigh_NumericControl.SetPrecision( TheMSBackgroundHighParameter->Precision() );
   BackgroundHigh_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundHigh_NumericControl.SetToolTip( "<p>Upper bound of the set of background pixels. Background reference "
      "pixels above this limit will be rejected for calculation of the initial mean background.</p>" );
   BackgroundHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&MaskedStretchInterface::__ValueUpdated, w );

   //

   ROI_SectionBar.SetTitle( "Region of Interest" );
   ROI_SectionBar.SetSection( ROI_Control );
   ROI_SectionBar.EnableTitleCheckBox();
   ROI_SectionBar.SetToolTip( "<p>By default, MaskedStretch analyzes the entire background reference image to compute an "
      "initial mean background level. When necessary, a region of interest (ROI) can be defined to restrict the analysis to a "
      "rectangular region of the background reference image. This may help to find an optimal transformation by excluding image "
      "structures that are not relevant for the current stretching task.</p>" );
   ROI_SectionBar.OnCheck( (SectionBar::check_event_handler)&MaskedStretchInterface::__SectionBarCheck, w );
   ROI_SectionBar.OnViewDrag( (Control::view_drag_event_handler)&MaskedStretchInterface::__ViewDrag, w );
   ROI_SectionBar.OnViewDrop( (Control::view_drop_event_handler)&MaskedStretchInterface::__ViewDrop, w );

   const char* roiX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIX0_Label.SetText( "Left:" );
   ROIX0_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   ROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIX0_Label.SetToolTip( roiX0ToolTip );

   ROIX0_SpinBox.SetRange( 0, int_max );
   ROIX0_SpinBox.SetFixedWidth( editWidth1 );
   ROIX0_SpinBox.SetToolTip( roiX0ToolTip );
   ROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&MaskedStretchInterface::__SpinValueUpdated, w );

   const char* roiY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>";

   ROIY0_Label.SetText( "Top:" );
   ROIY0_Label.SetFixedWidth( labelWidth2 );
   ROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIY0_Label.SetToolTip( roiY0ToolTip );

   ROIY0_SpinBox.SetRange( 0, int_max );
   ROIY0_SpinBox.SetFixedWidth( editWidth1 );
   ROIY0_SpinBox.SetToolTip( roiY0ToolTip );
   ROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&MaskedStretchInterface::__SpinValueUpdated, w );

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
   ROIWidth_SpinBox.SetFixedWidth( editWidth1 );
   ROIWidth_SpinBox.SetToolTip( roiWidthToolTip );
   ROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&MaskedStretchInterface::__SpinValueUpdated, w );

   const char* roiHeightToolTip = "<p>Height of the ROI in pixels.</p>";

   ROIHeight_Label.SetText( "Height:" );
   ROIHeight_Label.SetFixedWidth( labelWidth2 );
   ROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ROIHeight_Label.SetToolTip( roiHeightToolTip );

   ROIHeight_SpinBox.SetRange( 0, int_max );
   ROIHeight_SpinBox.SetFixedWidth( editWidth1 );
   ROIHeight_SpinBox.SetToolTip( roiHeightToolTip );
   ROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&MaskedStretchInterface::__SpinValueUpdated, w );

   ROISelectPreview_Button.SetText( "From Preview" );
   ROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>" );
   ROISelectPreview_Button.OnClick( (Button::click_event_handler)&MaskedStretchInterface::__Click, w );

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

   ROI_Control.SetSizer( ROI_Sizer );
   ROI_Control.OnViewDrag( (Control::view_drag_event_handler)&MaskedStretchInterface::__ViewDrag, w );
   ROI_Control.OnViewDrop( (Control::view_drop_event_handler)&MaskedStretchInterface::__ViewDrop, w );

   //

   Global_Sizer.SetMargin( 6 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( TargetBackground_NumericControl );
   Global_Sizer.Add( NumberOfIterations_Sizer );
   Global_Sizer.Add( ClippingFraction_Sizer );
   Global_Sizer.Add( MaskType_Sizer );
   Global_Sizer.Add( BackgroundReferenceView_Sizer );
   Global_Sizer.Add( BackgroundLow_NumericControl );
   Global_Sizer.Add( BackgroundHigh_NumericControl );
   Global_Sizer.Add( ROI_SectionBar );
   Global_Sizer.Add( ROI_Control );

   ROI_Control.Hide();

   w.SetSizer( Global_Sizer );

   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MaskedStretchInterface.cpp - Released 2017-04-14T23:07:12Z
