//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0200
// ----------------------------------------------------------------------------
// B3EInterface.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#include "B3EInterface.h"
#include "B3EParameters.h"
#include "B3EProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/ViewList.h>
#include <pcl/ViewSelectionDialog.h>

#define CLIGHT 299792.455  // speed of light in km/s

#define SYNTHETIC_IMAGE       0
#define THERMAL_MAP           1
#define SYNTHETIC_AND_THERMAL 2

#define TARGET_IMAGE             String( "<target image>" )
#define REFERENCE_ID( x )        (x.IsEmpty() ? TARGET_IMAGE : x)
#define BACKGROUND_REFERENCE_ID1  REFERENCE_ID( instance.p_inputView[0].backgroundReferenceViewId )
#define BACKGROUND_REFERENCE_ID2  REFERENCE_ID( instance.p_inputView[1].backgroundReferenceViewId )

namespace pcl
{

// ----------------------------------------------------------------------------

B3EInterface* TheB3EInterface = nullptr;

// ----------------------------------------------------------------------------

#include "B3EIcon.xpm"

// ----------------------------------------------------------------------------

B3EInterface::B3EInterface() :
   instance( TheB3EProcess )
{
   TheB3EInterface = this;
}

B3EInterface::~B3EInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString B3EInterface::Id() const
{
   return "B3Estimator";
}

MetaProcess* B3EInterface::Process() const
{
   return TheB3EProcess;
}

const char** B3EInterface::IconImageXPM() const
{
   return B3EIcon_XPM;
}

InterfaceFeatures B3EInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void B3EInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void B3EInterface::ResetInstance()
{
   B3EInstance defaultInstance( TheB3EProcess );
   ImportProcess( defaultInstance );
}

bool B3EInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
#ifdef __PCL_WINDOWS
      // Windows doesn't support Unicode for standard UI fonts
      SetWindowTitle( "B3E (Ballesteros Blackbody Estimator)" );
#else
      const char16_type title[] = { 'B', '3', 0x2107, 0 };
      SetWindowTitle( String( title ) + " (Ballesteros Blackbody Estimator)" );
#endif
      UpdateControls();
   }

   dynamic = false;
   return &P == TheB3EProcess;
}

ProcessImplementation* B3EInterface::NewProcess() const
{
   return new B3EInstance( instance );
}

bool B3EInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const B3EInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a B3E instance.";
   return false;
}

bool B3EInterface::RequiresInstanceValidation() const
{
   return true;
}

bool B3EInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void B3EInterface::UpdateControls()
{
   GUI->InputImage1_Edit.SetText( instance.p_inputView[0].id );
   GUI->InputImage2_Edit.SetText( instance.p_inputView[1].id );
   GUI->BackgroundReferenceView1_Edit.SetText(instance.p_inputView[0].backgroundReferenceViewId);
   GUI->BackgroundReferenceView2_Edit.SetText(instance.p_inputView[1].backgroundReferenceViewId);

   bool isFrequency = instance.p_intensityUnits == 0 || instance.p_intensityUnits == 1;

   GUI->CenterInput1_NumericEdit.label.SetText( String( "Input " ) + (isFrequency ? "frequency 1 (THz):" : "wavelength 1 (nm):") );
   GUI->CenterInput1_NumericEdit.SetValue( instance.p_inputView[0].center );

   GUI->CenterInput2_NumericEdit.label.SetText( String( "Input " ) + (isFrequency ? "frequency 2 (THz):" : "wavelength 2 (nm):") );
   GUI->CenterInput2_NumericEdit.SetValue( instance.p_inputView[1].center );

   GUI->CenterOutput_NumericEdit.label.SetText( String( "Output " ) + (isFrequency ? "frequency (THz):" : "wavelength (nm):") );
   GUI->CenterOutput_NumericEdit.SetValue( instance.p_outputCenter );
   GUI->CenterOutput_NumericEdit.Enable( instance.p_syntheticImage );

   GUI->IntensityUnits_ComboBox.SetCurrentItem( instance.p_intensityUnits );

   if ( instance.p_syntheticImage && !instance.p_thermalMap )
      GUI->OutputImages_ComboBox.SetCurrentItem( SYNTHETIC_IMAGE );
   else if ( !instance.p_syntheticImage && instance.p_thermalMap )
      GUI->OutputImages_ComboBox.SetCurrentItem( THERMAL_MAP );
   else
      GUI->OutputImages_ComboBox.SetCurrentItem( SYNTHETIC_AND_THERMAL );

   GUI->OutOfRangeMask_CheckBox.SetChecked( instance.p_outOfRangeMask );
   GUI->OutOfRangeMask_CheckBox.Enable( instance.p_syntheticImage );

   // Brackground Calibration 1
   GUI->BackgroundReferenceView1_Edit.SetText( BACKGROUND_REFERENCE_ID1 );
   GUI->BackgroundLow1_NumericControl.SetValue( instance.p_inputView[0].backgroundLow );
   GUI->BackgroundHigh1_NumericControl.SetValue( instance.p_inputView[0].backgroundHigh );
   GUI->BackgroundROI1_GroupBox.SetChecked( instance.p_inputView[0].backgroundUseROI );
   GUI->BackgroundROIX01_SpinBox.SetValue( instance.p_inputView[0].backgroundROI.x0 );
   GUI->BackgroundROIY01_SpinBox.SetValue( instance.p_inputView[0].backgroundROI.y0 );
   GUI->BackgroundROIWidth1_SpinBox.SetValue( instance.p_inputView[0].backgroundROI.Width() );
   GUI->BackgroundROIHeight1_SpinBox.SetValue( instance.p_inputView[0].backgroundROI.Height() );
   GUI->OutputBackgroundReferenceMask1_CheckBox.SetChecked( instance.p_inputView[0].outputBackgroundReferenceMask );

   // Brackground Calibration 2
   GUI->BackgroundReferenceView2_Edit.SetText( BACKGROUND_REFERENCE_ID2 );
   GUI->BackgroundLow2_NumericControl.SetValue( instance.p_inputView[1].backgroundLow );
   GUI->BackgroundHigh2_NumericControl.SetValue( instance.p_inputView[1].backgroundHigh );
   GUI->BackgroundROI2_GroupBox.SetChecked( instance.p_inputView[1].backgroundUseROI );
   GUI->BackgroundROIX02_SpinBox.SetValue( instance.p_inputView[1].backgroundROI.x0 );
   GUI->BackgroundROIY02_SpinBox.SetValue( instance.p_inputView[1].backgroundROI.y0 );
   GUI->BackgroundROIWidth2_SpinBox.SetValue( instance.p_inputView[1].backgroundROI.Width() );
   GUI->BackgroundROIHeight2_SpinBox.SetValue( instance.p_inputView[1].backgroundROI.Height() );
   GUI->OutputBackgroundReferenceMask2_CheckBox.SetChecked( instance.p_inputView[1].outputBackgroundReferenceMask );
}

// ----------------------------------------------------------------------------

void B3EInterface::__GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != nullptr )
      if ( e->Text() == TARGET_IMAGE )
         e->Clear();
}

void B3EInterface::__EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text().Trimmed();
      if ( !id.IsEmpty() )
         if ( !View::IsValidViewId( id ) )
            throw Error( "Invalid view identifier: " + id );

      if ( sender == GUI->InputImage1_Edit )
         instance.p_inputView[0].id = id;
      else if ( sender == GUI->InputImage2_Edit )
         instance.p_inputView[1].id = id;

      sender.SetText( id );
   }
   ERROR_CLEANUP(
      sender.SelectAll();
      sender.Focus();
      UpdateControls();
   )
}

void B3EInterface::__EditCompleted_bkg( Edit& sender )
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
            throw Error( "Invalid view identifier: " + id );
      }

      if ( sender == GUI->BackgroundReferenceView1_Edit )
      {
         instance.p_inputView[0].backgroundReferenceViewId = id;
         sender.SetText( BACKGROUND_REFERENCE_ID1 );
      }
      else if ( sender == GUI->BackgroundReferenceView2_Edit )
      {
         instance.p_inputView[1].backgroundReferenceViewId = id;
         sender.SetText( BACKGROUND_REFERENCE_ID2 );
      }
   }
   catch ( ... )
   {
      if ( sender == GUI->BackgroundReferenceView1_Edit )
         sender.SetText( BACKGROUND_REFERENCE_ID1 );
      else if ( sender == GUI->BackgroundReferenceView2_Edit )
         sender.SetText( BACKGROUND_REFERENCE_ID2 );

      try
      {
         throw;
      }
      ERROR_HANDLER

      sender.SelectAll();
      sender.Focus();
   }
}

void B3EInterface::__Clicked( Button& sender, bool checked )
{
   if ( sender == GUI->InputImage1_ToolButton )
   {
      ViewSelectionDialog d( instance.p_inputView[0].id );
      d.SetWindowTitle( "Select First Input Image" );
      if ( d.Execute() )
         instance.p_inputView[0].id = d.Id();
      GUI->InputImage1_Edit.SetText( instance.p_inputView[0].id );
   }
   else if ( sender == GUI->InputImage2_ToolButton )
   {
      ViewSelectionDialog d( instance.p_inputView[1].id );
      d.SetWindowTitle( "Select Second Input Image" );
      if ( d.Execute() )
         instance.p_inputView[1].id = d.Id();
      GUI->InputImage2_Edit.SetText( instance.p_inputView[1].id );
   }
   else if ( sender == GUI->OutOfRangeMask_CheckBox )
   {
      instance.p_outOfRangeMask = checked;
   }
   else if ( sender == GUI->BackgroundReferenceView1_ToolButton )
   {
      ViewSelectionDialog d( instance.p_inputView[0].backgroundReferenceViewId );
      d.SetWindowTitle( "Select First Background Reference Image" );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         instance.p_inputView[0].backgroundReferenceViewId = d.Id();
         GUI->BackgroundReferenceView1_Edit.SetText( BACKGROUND_REFERENCE_ID1 );
      }
   }
   else if ( sender == GUI->OutputBackgroundReferenceMask1_CheckBox )
      instance.p_inputView[0].outputBackgroundReferenceMask = checked;
   else if ( sender == GUI->BackgroundROISelectPreview1_Button )
   {
      PreviewSelectionDialog d;
      d.SetWindowTitle( "Select First Background ROI" );
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
               instance.p_inputView[0].backgroundROI = view.Window().PreviewRect( view.Id() );
         }
   }
   else if ( sender == GUI->BackgroundReferenceView2_ToolButton )
   {
      ViewSelectionDialog d( instance.p_inputView[1].backgroundReferenceViewId );
      d.SetWindowTitle( "Select Second Background Reference Image" );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         instance.p_inputView[1].backgroundReferenceViewId = d.Id();
         GUI->BackgroundReferenceView2_Edit.SetText( BACKGROUND_REFERENCE_ID2 );
      }
   }
   else if ( sender == GUI->OutputBackgroundReferenceMask2_CheckBox )
      instance.p_inputView[1].outputBackgroundReferenceMask = checked;
   else if ( sender == GUI->BackgroundROISelectPreview2_Button )
   {
      PreviewSelectionDialog d;
      d.SetWindowTitle( "Select Second Background ROI" );
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
               instance.p_inputView[1].backgroundROI = view.Window().PreviewRect( view.Id() );
         }
   }

   UpdateControls();
}

void B3EInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->IntensityUnits_ComboBox )
   {
      if ( ( instance.p_intensityUnits == 0 || instance.p_intensityUnits == 1 ) && (itemIndex == 2 || itemIndex == 3))
      {
         instance.p_inputView[0].center = CLIGHT/instance.p_inputView[0].center;
         instance.p_inputView[1].center = CLIGHT/instance.p_inputView[1].center;
         instance.p_outputCenter = CLIGHT/instance.p_outputCenter;
      }

      if ( ( instance.p_intensityUnits == 2 || instance.p_intensityUnits == 3 ) && (itemIndex == 0 || itemIndex == 1))
      {
         instance.p_inputView[0].center = CLIGHT/instance.p_inputView[0].center;
         instance.p_inputView[1].center = CLIGHT/instance.p_inputView[1].center;
         instance.p_outputCenter = CLIGHT/instance.p_outputCenter;
      }

      instance.p_intensityUnits = itemIndex;
   }
   else if ( sender == GUI->OutputImages_ComboBox )
   {
      switch ( itemIndex )
      {
      case SYNTHETIC_IMAGE:
         instance.p_syntheticImage = true;
         instance.p_thermalMap = false;
         break;
      case THERMAL_MAP:
         instance.p_syntheticImage = false;
         instance.p_thermalMap = true;
         break;
      case SYNTHETIC_AND_THERMAL:
         instance.p_syntheticImage = true;
         instance.p_thermalMap = true;
         break;
      default: // ?!
         throw Error( "B3EInterface: Internal error" );
         break;
      }
   }

   UpdateControls();
}

void B3EInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->CenterInput1_NumericEdit )
      instance.p_inputView[0].center = value;
   else if ( sender == GUI->BackgroundLow1_NumericControl )
      instance.p_inputView[0].backgroundLow = value;
   else if ( sender == GUI->BackgroundHigh1_NumericControl )
      instance.p_inputView[0].backgroundHigh = value;

   else if ( sender == GUI->CenterInput2_NumericEdit )
      instance.p_inputView[1].center = value;
   else if ( sender == GUI->BackgroundLow2_NumericControl )
      instance.p_inputView[1].backgroundLow = value;
   else if ( sender == GUI->BackgroundHigh2_NumericControl )
      instance.p_inputView[1].backgroundHigh = value;

   else if ( sender == GUI->CenterOutput_NumericEdit )
      instance.p_outputCenter = value;
}

void B3EInterface::__SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->BackgroundROIX01_SpinBox )
      instance.p_inputView[0].backgroundROI.x0 = value;
   else if ( sender == GUI->BackgroundROIY01_SpinBox )
      instance.p_inputView[0].backgroundROI.y0 = value;
   else if ( sender == GUI->BackgroundROIWidth1_SpinBox )
      instance.p_inputView[0].backgroundROI.x1 = instance.p_inputView[0].backgroundROI.x0 + value;
   else if ( sender == GUI->BackgroundROIHeight1_SpinBox )
      instance.p_inputView[0].backgroundROI.y1 = instance.p_inputView[0].backgroundROI.y0 + value;

   else if ( sender == GUI->BackgroundROIX02_SpinBox )
      instance.p_inputView[1].backgroundROI.x0 = value;
   else if ( sender == GUI->BackgroundROIY02_SpinBox )
      instance.p_inputView[1].backgroundROI.y0 = value;
   else if ( sender == GUI->BackgroundROIWidth2_SpinBox )
      instance.p_inputView[1].backgroundROI.x1 = instance.p_inputView[1].backgroundROI.x0 + value;
   else if ( sender == GUI->BackgroundROIHeight2_SpinBox )
      instance.p_inputView[1].backgroundROI.y1 = instance.p_inputView[1].backgroundROI.y0 + value;
}

void B3EInterface::__GroupBoxCheck( GroupBox& sender, bool checked )
{
   if ( sender == GUI->BackgroundROI1_GroupBox )
      instance.p_inputView[0].backgroundUseROI = checked;
   else if ( sender == GUI->BackgroundROI2_GroupBox )
      instance.p_inputView[1].backgroundUseROI = checked;
}

void B3EInterface::__BackgroundReference_Check( SectionBar& sender, bool checked )
{
   if ( sender == GUI->BackgroundReference1_SectionBar )
      instance.p_inputView[0].subtractBackground = checked;
   else if ( sender == GUI->BackgroundReference2_SectionBar )
      instance.p_inputView[1].subtractBackground = checked;
}

void B3EInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->InputImage1_Edit || sender == GUI->InputImage2_Edit ||
        sender == GUI->BackgroundReferenceView1_Edit || sender == GUI->BackgroundReferenceView2_Edit ||
        sender == GUI->BackgroundReference1_SectionBar || sender == GUI->BackgroundReference2_SectionBar )
   {
      wantsView = true;
   }
   else if ( sender == GUI->BackgroundROI1_GroupBox || sender == GUI->BackgroundROI2_GroupBox ||
             sender == GUI->BackgroundROISelectPreview1_Button || sender == GUI->BackgroundROISelectPreview2_Button )
   {
      wantsView = view.IsPreview();
   }
}

void B3EInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->InputImage1_Edit )
   {
      GUI->InputImage1_Edit.SetText( instance.p_inputView[0].id = view.FullId() );
   }
   else if ( sender == GUI->InputImage2_Edit )
   {
      GUI->InputImage2_Edit.SetText( instance.p_inputView[1].id = view.FullId() );
   }
   else if ( sender == GUI->BackgroundReference1_SectionBar )
   {
      GUI->BackgroundReferenceView1_Edit.SetText( instance.p_inputView[0].backgroundReferenceViewId = view.FullId() );
      GUI->BackgroundReference1_SectionBar.SetChecked( instance.p_inputView[0].subtractBackground = true );
      GUI->BackgroundReference1_SectionBar.ShowSection();
   }
   else if ( sender == GUI->BackgroundReferenceView1_Edit )
   {
      GUI->BackgroundReferenceView1_Edit.SetText( instance.p_inputView[0].backgroundReferenceViewId = view.FullId() );
   }
   else if ( sender == GUI->BackgroundReference2_SectionBar )
   {
      GUI->BackgroundReferenceView2_Edit.SetText( instance.p_inputView[1].backgroundReferenceViewId = view.FullId() );
      GUI->BackgroundReference2_SectionBar.SetChecked( instance.p_inputView[1].subtractBackground = true );
      GUI->BackgroundReference2_SectionBar.ShowSection();
   }
   else if ( sender == GUI->BackgroundReferenceView2_Edit )
   {
      GUI->BackgroundReferenceView2_Edit.SetText( instance.p_inputView[1].backgroundReferenceViewId = view.FullId() );
   }
   else if ( sender == GUI->BackgroundROI1_GroupBox || sender == GUI->BackgroundROISelectPreview1_Button )
   {
      if ( view.IsPreview() )
      {
         instance.p_inputView[0].backgroundUseROI = true;
         instance.p_inputView[0].backgroundROI = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
   else if ( sender == GUI->BackgroundROI2_GroupBox || sender == GUI->BackgroundROISelectPreview2_Button )
   {
      if ( view.IsPreview() )
      {
         instance.p_inputView[1].backgroundUseROI = true;
         instance.p_inputView[1].backgroundROI = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

B3EInterface::GUIData::GUIData( B3EInterface& w )
{
#define DELTA_FRAME  1

   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Input wavelength 1 (nm):" ) + 'M' ); // the longest label text
   int labelWidth2 = fnt.Width( String( "Height:" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 15 ) );
   int editWidth2 = fnt.Width( String( 'M', 30 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   const char* inputImage1ToolTip = "<p>Identifier of the first input view. Must be a flux-calibrated, grayscale image.</p>";

   InputImage1_Label.SetText( "Input image 1:" );
   InputImage1_Label.SetMinWidth( labelWidth1 );
   InputImage1_Label.SetToolTip( inputImage1ToolTip );
   InputImage1_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   InputImage1_Edit.SetMinWidth( editWidth2 );
   InputImage1_Edit.SetToolTip( inputImage1ToolTip );
   InputImage1_Edit.OnEditCompleted( (Edit::edit_event_handler)&B3EInterface::__EditCompleted, w );
   InputImage1_Edit.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   InputImage1_Edit.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   InputImage1_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   InputImage1_ToolButton.SetToolTip( "<p>Select the first input view.</p>" );
   InputImage1_ToolButton.SetScaledFixedSize( 20, 20 );
   InputImage1_ToolButton.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );

   InputImage1_Sizer.SetSpacing( 4 );
   InputImage1_Sizer.Add( InputImage1_Label );
   InputImage1_Sizer.Add( InputImage1_Edit, 100 );
   InputImage1_Sizer.Add( InputImage1_ToolButton );

   const char* inputImage2ToolTip = "<p>Identifier of the second input view. Must be a flux-calibrated, grayscale image.</p>";

   InputImage2_Label.SetText( "Input image 2:" );
   InputImage2_Label.SetMinWidth( labelWidth1 );
   InputImage2_Label.SetToolTip( inputImage2ToolTip );
   InputImage2_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   InputImage2_Edit.SetMinWidth( editWidth2 );
   InputImage2_Edit.SetToolTip( inputImage2ToolTip );
   InputImage2_Edit.OnEditCompleted( (Edit::edit_event_handler)&B3EInterface::__EditCompleted, w );
   InputImage2_Edit.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   InputImage2_Edit.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   InputImage2_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   InputImage2_ToolButton.SetToolTip( "<p>Select the second input view.</p>" );
   InputImage2_ToolButton.SetScaledFixedSize( 20, 20 );
   InputImage2_ToolButton.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );

   InputImage2_Sizer.SetSpacing( 4 );
   InputImage2_Sizer.Add( InputImage2_Label );
   InputImage2_Sizer.Add( InputImage2_Edit, 100 );
   InputImage2_Sizer.Add( InputImage2_ToolButton );

   Input_Sizer.SetSpacing( 6 );
   Input_Sizer.Add( InputImage1_Sizer );
   Input_Sizer.Add( InputImage2_Sizer );

   //CenterInput1_NumericEdit.label.SetText( "" );     // Text updated in UpdateControls().
   CenterInput1_NumericEdit.label.SetMinWidth( labelWidth1 );
   CenterInput1_NumericEdit.SetReal();
   CenterInput1_NumericEdit.SetRange( TheB3EInputCenter1Parameter->MinimumValue(), TheB3EInputCenter1Parameter->MaximumValue() );
   CenterInput1_NumericEdit.SetPrecision( TheB3EInputCenter1Parameter->Precision() );
   CenterInput1_NumericEdit.SetToolTip( "<p>Effective filter wavelength or frequency for the first input image.</p>" );
   CenterInput1_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CenterInput1_NumericEdit.sizer.AddStretch();
   CenterInput1_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&B3EInterface::__ValueUpdated, w );

   //CenterInput2_NumericEdit.label.SetText( "" );     // Text updated in UpdateControls().
   CenterInput2_NumericEdit.label.SetMinWidth( labelWidth1 );
   CenterInput2_NumericEdit.SetReal();
   CenterInput2_NumericEdit.SetRange( TheB3EInputCenter2Parameter->MinimumValue(), TheB3EInputCenter2Parameter->MaximumValue() );
   CenterInput2_NumericEdit.SetPrecision( TheB3EInputCenter2Parameter->Precision() );
   CenterInput2_NumericEdit.SetToolTip( "<p>Effective filter wavelength or frequency for the second input image.</p>" );
   CenterInput2_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CenterInput2_NumericEdit.sizer.AddStretch();
   CenterInput2_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&B3EInterface::__ValueUpdated, w );

   //CenterOutput_NumericEdit.label.SetText( "" );     // Text updated in UpdateControls().
   CenterOutput_NumericEdit.label.SetMinWidth( labelWidth1 );
   CenterOutput_NumericEdit.SetReal();
   CenterOutput_NumericEdit.SetRange( TheB3EOutputCenterParameter->MinimumValue(), TheB3EOutputCenterParameter->MaximumValue() );
   CenterOutput_NumericEdit.SetPrecision( TheB3EOutputCenterParameter->Precision() );
   CenterOutput_NumericEdit.SetToolTip( "<p>Effective filter wavelength or frequency for the output synthetic image. "
      "This parameter is not used when only a thermal map is generated.</p>" );
   CenterOutput_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CenterOutput_NumericEdit.sizer.AddStretch();
   CenterOutput_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&B3EInterface::__ValueUpdated, w );

   const char* intensityUnitsToolTip = "<p>Working flux units. Pixel values in the input and output images "
      "can represent either photons or energy per units of wavelength (nm) or frequency (THz). Energy can be "
      "expressed in any suitable units, with the condition that the same units are used for both input images. "
      "These will be also the units of the output synthetic image.</p>";

   IntensityUnits_Label.SetText( "Intensity units:" );
   IntensityUnits_Label.SetMinWidth( labelWidth1 );
   IntensityUnits_Label.SetToolTip( intensityUnitsToolTip );
   IntensityUnits_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   IntensityUnits_ComboBox.AddItem( "Photons / Frequency" );
   IntensityUnits_ComboBox.AddItem( "Energy / Frequency" );
   IntensityUnits_ComboBox.AddItem( "Photons / Wavelength" );
   IntensityUnits_ComboBox.AddItem( "Energy / Wavelength" );
   IntensityUnits_ComboBox.SetToolTip( intensityUnitsToolTip );
   IntensityUnits_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&B3EInterface::__ItemSelected, w );

   IntensityUnits_Sizer.SetSpacing( 4 );
   IntensityUnits_Sizer.Add( IntensityUnits_Label );
   IntensityUnits_Sizer.Add( IntensityUnits_ComboBox );
   IntensityUnits_Sizer.AddStretch();

   ProcessParameters_Sizer.SetSpacing( 6 );
   ProcessParameters_Sizer.Add( CenterInput1_NumericEdit );
   ProcessParameters_Sizer.Add( CenterInput2_NumericEdit );
   ProcessParameters_Sizer.Add( CenterOutput_NumericEdit );
   ProcessParameters_Sizer.Add( IntensityUnits_Sizer );
   ProcessParameters_Sizer.AddStretch();

   const char* outputImagesToolTip = "<p>B3E uses the laws of blackbody radiation to generate a synthetic image, "
      "which is an estimate of flux at the specified effective output wavelength or frequency, and a thermal map "
      "as an estimate of temperature. Flux estimates are in the same units as input images, and thermal maps are "
      "unsigned 16-bit integer images whose pixels represent blackbody temperature in degrees Kelvin.</p>";

   OutputImages_Label.SetText( "Output image/s:" );
   OutputImages_Label.SetMinWidth( labelWidth1 );
   OutputImages_Label.SetToolTip( outputImagesToolTip );
   OutputImages_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputImages_ComboBox.AddItem( "Synthetic Image" );
   OutputImages_ComboBox.AddItem( "Thermal Map" );
   OutputImages_ComboBox.AddItem( "Synthetic Image & Thermal Map" );
   OutputImages_ComboBox.SetToolTip( outputImagesToolTip );
   OutputImages_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&B3EInterface::__ItemSelected, w );

   OutputImages_Sizer.SetSpacing( 4 );
   OutputImages_Sizer.Add( OutputImages_Label );
   OutputImages_Sizer.Add( OutputImages_ComboBox );
   OutputImages_Sizer.AddStretch();

   OutOfRangeMask_CheckBox.SetText( "Generate out-of-range mask" );
   OutOfRangeMask_CheckBox.SetToolTip( "<p>When this option is selected, B3E generates a binary mask where white "
      "pixels correspond to out-of-range flux values in the output synthetic image. Out-of-range flux values are "
      "those above the maximum flux estimated from the input images.</p>" );
   OutOfRangeMask_CheckBox.OnClick( (pcl::Button::click_event_handler)&B3EInterface::__Clicked, w );

   OutOfRangeMask_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutOfRangeMask_Sizer.Add( OutOfRangeMask_CheckBox );
   OutOfRangeMask_Sizer.AddStretch();

   OutputData_Sizer.SetSpacing( 4 );
   OutputData_Sizer.Add( OutputImages_Sizer );
   OutputData_Sizer.Add( OutOfRangeMask_Sizer );
   OutputData_Sizer.AddStretch();

   BackgroundReference1_SectionBar.SetTitle( "Background Reference 1" );
   BackgroundReference1_SectionBar.SetSection( BackgroundReference1_Control );
   BackgroundReference1_SectionBar.EnableTitleCheckBox();
   BackgroundReference1_SectionBar.OnCheck( (SectionBar::check_event_handler)&B3EInterface::__BackgroundReference_Check, w );
   BackgroundReference1_SectionBar.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundReference1_SectionBar.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   BackgroundReferenceView1_Label.SetText( "Reference image 1:" );
   BackgroundReferenceView1_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceView1_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceView1_Edit.SetToolTip( "<p>Background reference image.</p>"
      "<p>B3Estimator will use pixels read from this image to compute an initial mean background level for "
      "each color channel. If you leave this field blank (or with its default &lt;target image&gt; value), the "
      "target image will be also the background reference image during the color calibration process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>" );
   BackgroundReferenceView1_Edit.OnGetFocus( (Control::event_handler)&B3EInterface::__GetFocus, w );
   BackgroundReferenceView1_Edit.OnEditCompleted( (Edit::edit_event_handler)&B3EInterface::__EditCompleted_bkg, w );
   BackgroundReferenceView1_Edit.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundReferenceView1_Edit.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   BackgroundReferenceView1_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceView1_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceView1_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceView1_ToolButton.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );

   BackgroundReferenceView1_Sizer.SetSpacing( 4 );
   BackgroundReferenceView1_Sizer.Add( BackgroundReferenceView1_Label );
   BackgroundReferenceView1_Sizer.Add( BackgroundReferenceView1_Edit );
   BackgroundReferenceView1_Sizer.Add( BackgroundReferenceView1_ToolButton );

   BackgroundLow1_NumericControl.label.SetText( "Lower limit:" );
   BackgroundLow1_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundLow1_NumericControl.slider.SetRange( 0, 100 );
   BackgroundLow1_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundLow1_NumericControl.SetReal();
   BackgroundLow1_NumericControl.SetRange( TheB3EBackgroundLow1Parameter->MinimumValue(), TheB3EBackgroundLow1Parameter->MaximumValue() );
   BackgroundLow1_NumericControl.SetPrecision( TheB3EBackgroundLow1Parameter->Precision() );
   BackgroundLow1_NumericControl.SetToolTip( "<p>Lower bound of the set of background pixels. Background reference "
      "pixels below this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundLow1_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&B3EInterface::__ValueUpdated, w );

   BackgroundHigh1_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh1_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh1_NumericControl.slider.SetRange( 0, 100 );
   BackgroundHigh1_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundHigh1_NumericControl.SetReal();
   BackgroundHigh1_NumericControl.SetRange( TheB3EBackgroundHigh1Parameter->MinimumValue(), TheB3EBackgroundHigh1Parameter->MaximumValue() );
   BackgroundHigh1_NumericControl.SetPrecision( TheB3EBackgroundHigh1Parameter->Precision() );
   BackgroundHigh1_NumericControl.SetToolTip( "<p>Upper bound of the set of background pixels. Background reference "
      "pixels above this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundHigh1_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&B3EInterface::__ValueUpdated, w );

   BackgroundROI1_GroupBox.SetTitle( "Region of Interest" );
   BackgroundROI1_GroupBox.EnableTitleCheckBox();
   BackgroundROI1_GroupBox.OnCheck( (GroupBox::check_event_handler)&B3EInterface::__GroupBoxCheck, w );
   BackgroundROI1_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundROI1_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   const char* backgroundROIX01ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI, background reference.</p>";

   BackgroundROIX01_Label.SetText( "Left:" );
   BackgroundROIX01_Label.SetFixedWidth( labelWidth2 - w.LogicalPixelsToPhysical( DELTA_FRAME ) );
   BackgroundROIX01_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIX01_Label.SetToolTip( backgroundROIX01ToolTip );

   BackgroundROIX01_SpinBox.SetRange( 0, int_max );
   BackgroundROIX01_SpinBox.SetToolTip( backgroundROIX01ToolTip );
   BackgroundROIX01_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   const char* backgroundROIY01ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI, background reference.</p>";

   BackgroundROIY01_Label.SetText( "Top:" );
   BackgroundROIY01_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIY01_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIY01_Label.SetToolTip( backgroundROIY01ToolTip );

   BackgroundROIY01_SpinBox.SetRange( 0, int_max );
   BackgroundROIY01_SpinBox.SetToolTip( backgroundROIY01ToolTip );
   BackgroundROIY01_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   BackgroundROIRow11_Sizer.SetSpacing( 4 );
   BackgroundROIRow11_Sizer.Add( BackgroundROIX01_Label );
   BackgroundROIRow11_Sizer.Add( BackgroundROIX01_SpinBox );
   BackgroundROIRow11_Sizer.Add( BackgroundROIY01_Label );
   BackgroundROIRow11_Sizer.Add( BackgroundROIY01_SpinBox );
   BackgroundROIRow11_Sizer.AddStretch();

   const char* backgroundROIWidth1ToolTip = "<p>Width of the ROI in pixels, background reference.</p>";

   BackgroundROIWidth1_Label.SetText( "Width:" );
   BackgroundROIWidth1_Label.SetFixedWidth( labelWidth2 - w.LogicalPixelsToPhysical( DELTA_FRAME ) );
   BackgroundROIWidth1_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIWidth1_Label.SetToolTip( backgroundROIWidth1ToolTip );

   BackgroundROIWidth1_SpinBox.SetRange( 0, int_max );
   BackgroundROIWidth1_SpinBox.SetToolTip( backgroundROIWidth1ToolTip );
   BackgroundROIWidth1_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   const char* backgroundROIHeight1ToolTip = "<p>Height of the ROI in pixels, background reference.</p>";

   BackgroundROIHeight1_Label.SetText( "Height:" );
   BackgroundROIHeight1_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIHeight1_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIHeight1_Label.SetToolTip( backgroundROIHeight1ToolTip );

   BackgroundROIHeight1_SpinBox.SetRange( 0, int_max );
   BackgroundROIHeight1_SpinBox.SetToolTip( backgroundROIHeight1ToolTip );
   BackgroundROIHeight1_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   BackgroundROISelectPreview1_Button.SetText( "From Preview" );
   BackgroundROISelectPreview1_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview, background reference.</p>" );
   BackgroundROISelectPreview1_Button.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );
   BackgroundROISelectPreview1_Button.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundROISelectPreview1_Button.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   BackgroundROIRow21_Sizer.SetSpacing( 4 );
   BackgroundROIRow21_Sizer.Add( BackgroundROIWidth1_Label );
   BackgroundROIRow21_Sizer.Add( BackgroundROIWidth1_SpinBox );
   BackgroundROIRow21_Sizer.Add( BackgroundROIHeight1_Label );
   BackgroundROIRow21_Sizer.Add( BackgroundROIHeight1_SpinBox );
   BackgroundROIRow21_Sizer.AddSpacing( 12 );
   BackgroundROIRow21_Sizer.Add( BackgroundROISelectPreview1_Button );
   BackgroundROIRow21_Sizer.AddStretch();

   BackgroundROI1_Sizer.SetMargin( 6 );
   BackgroundROI1_Sizer.SetSpacing( 4 );
   BackgroundROI1_Sizer.Add( BackgroundROIRow11_Sizer );
   BackgroundROI1_Sizer.Add( BackgroundROIRow21_Sizer );

   BackgroundROI1_GroupBox.SetSizer( BackgroundROI1_Sizer );

   OutputBackgroundReferenceMask1_CheckBox.SetText( "Output background reference mask" );
   OutputBackgroundReferenceMask1_CheckBox.SetToolTip( "<p>If this option is selected, B3Estimator will "
      "create a new image window with a <i>background reference mask</i>.</p>"
      "<p>A background reference mask is white for pixels in the background reference image that have been "
      "used to calculate mean background levels, black anywhere else. You can use this mask to check whether the "
      "<i>Lower limit</i> and <i>Upper limit</i> parameters define a suitable range of values to select the "
      "pixels that you intend to use as background reference.</p>" );
   OutputBackgroundReferenceMask1_CheckBox.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );

   OutputBackgroundReferenceMask1_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputBackgroundReferenceMask1_Sizer.Add( OutputBackgroundReferenceMask1_CheckBox );
   OutputBackgroundReferenceMask1_Sizer.AddStretch();

   BackgroundReference1_Sizer.SetSpacing( 4 );
   BackgroundReference1_Sizer.Add( BackgroundReferenceView1_Sizer );
   BackgroundReference1_Sizer.Add( BackgroundLow1_NumericControl );
   BackgroundReference1_Sizer.Add( BackgroundHigh1_NumericControl );
   BackgroundReference1_Sizer.Add( BackgroundROI1_GroupBox );
   BackgroundReference1_Sizer.Add( OutputBackgroundReferenceMask1_Sizer );

   BackgroundReference1_Control.SetSizer( BackgroundReference1_Sizer );

   BackgroundReference2_SectionBar.SetTitle( "Background Reference 2" );
   BackgroundReference2_SectionBar.SetSection( BackgroundReference2_Control );
   BackgroundReference2_SectionBar.EnableTitleCheckBox();
   BackgroundReference2_SectionBar.OnCheck( (SectionBar::check_event_handler)&B3EInterface::__BackgroundReference_Check, w );
   BackgroundReference2_SectionBar.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundReference2_SectionBar.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   BackgroundReferenceView2_Label.SetText( "Reference image 2:" );
   BackgroundReferenceView2_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceView2_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceView2_Edit.SetToolTip( "<p>Background reference image.</p>"
      "<p>B3Estimator will use pixels read from this image to compute an initial mean background level for "
      "each color channel. If you leave this field blank (or with its default &lt;target image&gt; value), the "
      "target image will be also the background reference image during the color calibration process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>" );
   BackgroundReferenceView2_Edit.OnGetFocus( (Control::event_handler)&B3EInterface::__GetFocus, w );
   BackgroundReferenceView2_Edit.OnEditCompleted( (Edit::edit_event_handler)&B3EInterface::__EditCompleted_bkg, w );
   BackgroundReferenceView2_Edit.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundReferenceView2_Edit.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   BackgroundReferenceView2_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceView2_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceView2_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceView2_ToolButton.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );

   BackgroundReferenceView2_Sizer.SetSpacing( 4 );
   BackgroundReferenceView2_Sizer.Add( BackgroundReferenceView2_Label );
   BackgroundReferenceView2_Sizer.Add( BackgroundReferenceView2_Edit );
   BackgroundReferenceView2_Sizer.Add( BackgroundReferenceView2_ToolButton );

   BackgroundLow2_NumericControl.label.SetText( "Lower limit:" );
   BackgroundLow2_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundLow2_NumericControl.slider.SetRange( 0, 100 );
   BackgroundLow2_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundLow2_NumericControl.SetReal();
   BackgroundLow2_NumericControl.SetRange( TheB3EBackgroundLow2Parameter->MinimumValue(), TheB3EBackgroundLow2Parameter->MaximumValue() );
   BackgroundLow2_NumericControl.SetPrecision( TheB3EBackgroundLow2Parameter->Precision() );
   BackgroundLow2_NumericControl.SetToolTip( "<p>Lower bound of the set of background pixels. Background reference "
      "pixels below this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundLow2_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&B3EInterface::__ValueUpdated, w );

   BackgroundHigh2_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh2_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh2_NumericControl.slider.SetRange( 0, 100 );
   BackgroundHigh2_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundHigh2_NumericControl.SetReal();
   BackgroundHigh2_NumericControl.SetRange( TheB3EBackgroundHigh2Parameter->MinimumValue(), TheB3EBackgroundHigh2Parameter->MaximumValue() );
   BackgroundHigh2_NumericControl.SetPrecision( TheB3EBackgroundHigh2Parameter->Precision() );
   BackgroundHigh2_NumericControl.SetToolTip( "<p>Upper bound of the set of background pixels. Background reference "
      "pixels above this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundHigh2_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&B3EInterface::__ValueUpdated, w );

   BackgroundROI2_GroupBox.SetTitle( "Region of Interest" );
   BackgroundROI2_GroupBox.EnableTitleCheckBox();
   BackgroundROI2_GroupBox.OnCheck( (GroupBox::check_event_handler)&B3EInterface::__GroupBoxCheck, w );
   BackgroundROI2_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundROI2_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   const char* backgroundROIX02ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI, background reference.</p>";

   BackgroundROIX02_Label.SetText( "Left:" );
   BackgroundROIX02_Label.SetFixedWidth( labelWidth2 - w.LogicalPixelsToPhysical( DELTA_FRAME ) );
   BackgroundROIX02_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIX02_Label.SetToolTip( backgroundROIX02ToolTip );

   BackgroundROIX02_SpinBox.SetRange( 0, int_max );
   BackgroundROIX02_SpinBox.SetToolTip( backgroundROIX02ToolTip );
   BackgroundROIX02_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   const char* backgroundROIY02ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI, background reference.</p>";

   BackgroundROIY02_Label.SetText( "Top:" );
   BackgroundROIY02_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIY02_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIY02_Label.SetToolTip( backgroundROIY02ToolTip );

   BackgroundROIY02_SpinBox.SetRange( 0, int_max );
   BackgroundROIY02_SpinBox.SetToolTip( backgroundROIY02ToolTip );
   BackgroundROIY02_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   BackgroundROIRow12_Sizer.SetSpacing( 4 );
   BackgroundROIRow12_Sizer.Add( BackgroundROIX02_Label );
   BackgroundROIRow12_Sizer.Add( BackgroundROIX02_SpinBox );
   BackgroundROIRow12_Sizer.Add( BackgroundROIY02_Label );
   BackgroundROIRow12_Sizer.Add( BackgroundROIY02_SpinBox );
   BackgroundROIRow12_Sizer.AddStretch();

   const char* backgroundROIWidth2ToolTip = "<p>Width of the ROI in pixels, background reference.</p>";

   BackgroundROIWidth2_Label.SetText( "Width:" );
   BackgroundROIWidth2_Label.SetFixedWidth( labelWidth2 - w.LogicalPixelsToPhysical( DELTA_FRAME ) );
   BackgroundROIWidth2_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIWidth2_Label.SetToolTip( backgroundROIWidth2ToolTip );

   BackgroundROIWidth2_SpinBox.SetRange( 0, int_max );
   BackgroundROIWidth2_SpinBox.SetToolTip( backgroundROIWidth2ToolTip );
   BackgroundROIWidth2_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   const char* backgroundROIHeight2ToolTip = "<p>Height of the ROI in pixels, background reference.</p>";

   BackgroundROIHeight2_Label.SetText( "Height:" );
   BackgroundROIHeight2_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIHeight2_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIHeight2_Label.SetToolTip( backgroundROIHeight2ToolTip );

   BackgroundROIHeight2_SpinBox.SetRange( 0, int_max );
   BackgroundROIHeight2_SpinBox.SetToolTip( backgroundROIHeight2ToolTip );
   BackgroundROIHeight2_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&B3EInterface::__SpinValueUpdated, w );

   BackgroundROISelectPreview2_Button.SetText( "From Preview" );
   BackgroundROISelectPreview2_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview, background reference.</p>" );
   BackgroundROISelectPreview2_Button.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );
   BackgroundROISelectPreview2_Button.OnViewDrag( (Control::view_drag_event_handler)&B3EInterface::__ViewDrag, w );
   BackgroundROISelectPreview2_Button.OnViewDrop( (Control::view_drop_event_handler)&B3EInterface::__ViewDrop, w );

   BackgroundROIRow22_Sizer.SetSpacing( 4 );
   BackgroundROIRow22_Sizer.Add( BackgroundROIWidth2_Label );
   BackgroundROIRow22_Sizer.Add( BackgroundROIWidth2_SpinBox );
   BackgroundROIRow22_Sizer.Add( BackgroundROIHeight2_Label );
   BackgroundROIRow22_Sizer.Add( BackgroundROIHeight2_SpinBox );
   BackgroundROIRow22_Sizer.AddSpacing( 12 );
   BackgroundROIRow22_Sizer.Add( BackgroundROISelectPreview2_Button );
   BackgroundROIRow22_Sizer.AddStretch();

   BackgroundROI2_Sizer.SetMargin( 6 );
   BackgroundROI2_Sizer.SetSpacing( 4 );
   BackgroundROI2_Sizer.Add( BackgroundROIRow12_Sizer );
   BackgroundROI2_Sizer.Add( BackgroundROIRow22_Sizer );

   BackgroundROI2_GroupBox.SetSizer( BackgroundROI2_Sizer );

   OutputBackgroundReferenceMask2_CheckBox.SetText( "Output background reference mask" );
   OutputBackgroundReferenceMask2_CheckBox.SetToolTip( "<p>If this option is selected, B3Estimator will "
      "create a new image window with a <i>background reference mask</i>.</p>"
      "<p>A background reference mask is white for pixels in the background reference image that have been "
      "used to calculate mean background levels, black anywhere else. You can use this mask to check whether the "
      "<i>Lower limit</i> and <i>Upper limit</i> parameters define a suitable range of values to select the "
      "pixels that you intend to use as background reference.</p>" );
   OutputBackgroundReferenceMask2_CheckBox.OnClick( (Button::click_event_handler)&B3EInterface::__Clicked, w );

   OutputBackgroundReferenceMask2_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputBackgroundReferenceMask2_Sizer.Add( OutputBackgroundReferenceMask2_CheckBox );
   OutputBackgroundReferenceMask2_Sizer.AddStretch();

   BackgroundReference2_Sizer.SetSpacing( 4 );
   BackgroundReference2_Sizer.Add( BackgroundReferenceView2_Sizer );
   BackgroundReference2_Sizer.Add( BackgroundLow2_NumericControl );
   BackgroundReference2_Sizer.Add( BackgroundHigh2_NumericControl );
   BackgroundReference2_Sizer.Add( BackgroundROI2_GroupBox );
   BackgroundReference2_Sizer.Add( OutputBackgroundReferenceMask2_Sizer );

   BackgroundReference2_Control.SetSizer( BackgroundReference2_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Input_Sizer );
   Global_Sizer.Add( ProcessParameters_Sizer );
   Global_Sizer.Add( OutputData_Sizer );
   Global_Sizer.Add( BackgroundReference1_SectionBar );
   Global_Sizer.Add( BackgroundReference1_Control );
   Global_Sizer.Add( BackgroundReference2_SectionBar );
   Global_Sizer.Add( BackgroundReference2_Control );

   w.SetSizer( Global_Sizer );
   w.SetFixedWidth();

   BackgroundReference1_Control.Hide();
   BackgroundReference2_Control.Hide();

   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF B3EInterface.cpp - Released 2018-11-23T18:45:58Z
