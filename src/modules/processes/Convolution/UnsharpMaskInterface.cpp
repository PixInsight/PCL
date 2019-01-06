//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0276
// ----------------------------------------------------------------------------
// UnsharpMaskInterface.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#include "UnsharpMaskInterface.h"
#include "UnsharpMaskParameters.h"
#include "UnsharpMaskProcess.h"

#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

UnsharpMaskInterface* TheUnsharpMaskInterface = nullptr;

// ----------------------------------------------------------------------------

#include "UnsharpMaskIcon.xpm"

// ----------------------------------------------------------------------------

// Target ComboBox items
enum { ToCIEL, ToCIEY, ToRGB };

// ----------------------------------------------------------------------------

UnsharpMaskInterface::UnsharpMaskInterface() :
   instance( TheUnsharpMaskProcess )
{
   TheUnsharpMaskInterface = this;
}

// ----------------------------------------------------------------------------

UnsharpMaskInterface::~UnsharpMaskInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString UnsharpMaskInterface::Id() const
{
   return "UnsharpMask";
}

// ----------------------------------------------------------------------------

MetaProcess* UnsharpMaskInterface::Process() const
{
   return TheUnsharpMaskProcess;
}

// ----------------------------------------------------------------------------

const char** UnsharpMaskInterface::IconImageXPM() const
{
   return UnsharpMaskIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures UnsharpMaskInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

void UnsharpMaskInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void UnsharpMaskInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void UnsharpMaskInterface::ResetInstance()
{
   UnsharpMaskInstance defaultInstance( TheUnsharpMaskProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool UnsharpMaskInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "UnsharpMask" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheUnsharpMaskProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* UnsharpMaskInterface::NewProcess() const
{
   return new UnsharpMaskInstance( instance );
}

// ----------------------------------------------------------------------------

bool UnsharpMaskInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const UnsharpMaskInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an UnsharpMask instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool UnsharpMaskInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool UnsharpMaskInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   UpdateRealTimePreview();
   return true;
}

// ----------------------------------------------------------------------------

bool UnsharpMaskInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const
{
   return true;
}

// ----------------------------------------------------------------------------

UnsharpMaskInterface::RealTimeThread::RealTimeThread() :
   m_instance( TheUnsharpMaskProcess )
{
}

void UnsharpMaskInterface::RealTimeThread::Reset( const UInt16Image& image, const UnsharpMaskInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

void UnsharpMaskInterface::RealTimeThread::Run()
{
   m_instance.Preview( m_image );
}

bool UnsharpMaskInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int zoomLevel, String& ) const
{
   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      UnsharpMaskInstance previewInstance( instance );
      if ( zoomLevel < 0 )
         previewInstance.sigma /= -zoomLevel;

      m_realTimeThread->Reset( image, previewInstance );
      m_realTimeThread->Start();

      while ( m_realTimeThread->IsActive() )
      {
         ProcessEvents();

         if ( !IsRealTimePreviewActive() )
         {
            m_realTimeThread->Abort();
            m_realTimeThread->Wait();

            delete m_realTimeThread;
            m_realTimeThread = nullptr;
            return false;
         }
      }

      if ( !m_realTimeThread->IsAborted() )
      {
         image.Assign( m_realTimeThread->m_image );

         delete m_realTimeThread;
         m_realTimeThread = nullptr;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void UnsharpMaskInterface::UpdateControls()
{
  UpdateFilterControls();
  UpdateDeringingControls();
  UpdateRangeControls();
}

void UnsharpMaskInterface::UpdateFilterControls()
{
   if ( instance.sigma <= 10 )
   {
      GUI->SigmaCoarse_Slider.SetValue( 10 );
      GUI->SigmaFine_Slider.SetValue( int( 10 * instance.sigma ) );
      GUI->SigmaFine_Slider.Enable();
   }
   else
   {
      int s = int( instance.sigma );
      instance.sigma = s;
      GUI->SigmaCoarse_Slider.SetValue( s );
      GUI->SigmaFine_Slider.SetValue( 100 );
      GUI->SigmaFine_Slider.Disable();
   }

   GUI->Sigma_NumericEdit.SetValue( instance.sigma );

   GUI->Amount_NumericControl.SetValue( instance.amount );

   GUI->Target_ComboBox.SetCurrentItem( instance.useLuminance ? (instance.linear ? ToCIEY : ToCIEL) : ToRGB );
}

void UnsharpMaskInterface::UpdateDeringingControls()
{
   GUI->Deringing_SectionBar.SetChecked( instance.deringing );

   GUI->DeringingDark_NumericControl.SetValue( instance.deringingDark );
   GUI->DeringingBright_NumericControl.SetValue( instance.deringingBright );

   GUI->OutputDeringingMaps_CheckBox.SetChecked( instance.outputDeringingMaps );
}

void UnsharpMaskInterface::UpdateRangeControls()
{
   GUI->RangeLow_NumericControl.SetValue( instance.rangeLow );
   GUI->RangeHigh_NumericControl.SetValue( instance.rangeHigh );
}

void UnsharpMaskInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != nullptr )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void UnsharpMaskInterface::__Filter_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Sigma_NumericEdit )
      instance.sigma = value;
   else if ( sender == GUI->Amount_NumericControl )
      instance.amount = value;

   UpdateFilterControls();
   UpdateRealTimePreview();
}

void UnsharpMaskInterface::__Filter_SliderUpdated( Slider& sender, int value )
{
   if ( sender == GUI->SigmaCoarse_Slider )
      instance.sigma = value;
   else if ( sender == GUI->SigmaFine_Slider )
      instance.sigma = 0.1 * value;

   UpdateFilterControls();
   UpdateRealTimePreview();
}

void UnsharpMaskInterface::__Target_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Target_ComboBox )
   {
      switch ( itemIndex )
      {
      case ToCIEL:
         instance.useLuminance = true;
         instance.linear = false;
         break;
      case ToCIEY:
         instance.useLuminance = true;
         instance.linear = true;
         break;
      case ToRGB:
         instance.useLuminance = false;
         instance.linear = false;
         break;
      }
   }

   UpdateFilterControls();
   UpdateRealTimePreview();
}

void UnsharpMaskInterface::__Deringing_Check( SectionBar& sender, bool checked )
{
   if ( sender == GUI->Deringing_SectionBar )
      instance.deringing = checked;

   UpdateDeringingControls();
   UpdateRealTimePreview();
}

void UnsharpMaskInterface::__Deringing_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->DeringingDark_NumericControl )
      instance.deringingDark = value;
   else if ( sender == GUI->DeringingBright_NumericControl )
      instance.deringingBright = value;

   UpdateDeringingControls();
   UpdateRealTimePreview();
}

void UnsharpMaskInterface::__Deringing_Click( Button& sender, bool checked )
{
   if ( sender == GUI->OutputDeringingMaps_CheckBox )
      instance.outputDeringingMaps = checked;

   UpdateDeringingControls();
   UpdateRealTimePreview();
}

void UnsharpMaskInterface::__Range_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RangeLow_NumericControl )
      instance.rangeLow = value;
   else if ( sender == GUI->RangeHigh_NumericControl )
      instance.rangeHigh = value;

   UpdateRangeControls();
   UpdateRealTimePreview();
}

void UnsharpMaskInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != nullptr )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

UnsharpMaskInterface::GUIData::GUIData( UnsharpMaskInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "High Range:" ) + 'M' );
   int editWidth1 = fnt.Width( String( '0', 11 ) );
   int editHeight = CanonicalControlHeight( Edit );

   // -------------------------------------------------------------------------

   Filter_SectionBar.SetTitle( "USM Filter" );
   Filter_SectionBar.SetSection( Filter_Control );

   //

   Sigma_NumericEdit.label.SetText( "StdDev:" );
   Sigma_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Sigma_NumericEdit.SetReal();
   Sigma_NumericEdit.SetRange( TheUSMSigmaParameter->MinimumValue(), TheUSMSigmaParameter->MaximumValue() );
   Sigma_NumericEdit.SetPrecision( TheUSMSigmaParameter->Precision() );
   Sigma_NumericEdit.SetToolTip( "<p>Standard deviation of the Gaussian low-pass filter, in pixels. "
      "By increasing this parameter the unsharp mask filter will act at higher dimensional scales.</p>" );
   Sigma_NumericEdit.edit.SetFixedWidth( editWidth1 );
   Sigma_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&UnsharpMaskInterface::__Filter_ValueUpdated, w );

   SigmaCoarse_Slider.SetScaledMinWidth( 250 );
   SigmaCoarse_Slider.SetRange( 10, 250 );
   SigmaCoarse_Slider.SetFixedHeight( RoundInt( 0.75*editHeight ) );
   SigmaCoarse_Slider.SetToolTip( "<p>StdDev coarse adjustment: From 10 to 250 pixels.</p>" );
   SigmaCoarse_Slider.OnValueUpdated( (Slider::value_event_handler)&UnsharpMaskInterface::__Filter_SliderUpdated, w );

   SigmaFine_Slider.SetScaledMinWidth( 250 );
   SigmaFine_Slider.SetRange( 1, 100 );
   SigmaFine_Slider.SetFixedHeight( RoundInt( 0.75*editHeight ) );
   SigmaFine_Slider.SetToolTip( "<p>StdDev fine adjustment: From 0.1 to 10.0 pixels.</p>" );
   SigmaFine_Slider.OnValueUpdated( (Slider::value_event_handler)&UnsharpMaskInterface::__Filter_SliderUpdated, w );

   SigmaSliders_Sizer.SetSpacing( 2 );
   SigmaSliders_Sizer.Add( SigmaCoarse_Slider );
   SigmaSliders_Sizer.Add( SigmaFine_Slider );

   Sigma_Sizer.SetSpacing( 4 );
   Sigma_Sizer.Add( Sigma_NumericEdit );
   Sigma_Sizer.Add( SigmaSliders_Sizer, 100 );

   //

   Amount_NumericControl.label.SetText( "Amount:" );
   Amount_NumericControl.label.SetFixedWidth( labelWidth1 );
   Amount_NumericControl.slider.SetScaledMinWidth( 250 );
   Amount_NumericControl.slider.SetRange( 0, 110 );
   Amount_NumericControl.SetReal();
   Amount_NumericControl.SetRange( TheUSMAmountParameter->MinimumValue(), TheUSMAmountParameter->MaximumValue() );
   Amount_NumericControl.SetPrecision( TheUSMAmountParameter->Precision() );
   Amount_NumericControl.SetToolTip( "<p>USM filtering strength.</p>" );
   Amount_NumericControl.edit.SetFixedWidth( editWidth1 );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&UnsharpMaskInterface::__Filter_ValueUpdated, w );

   //

   Target_Label.SetText( "Target:" );
   Target_Label.SetFixedWidth( labelWidth1 );
   Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Target_ComboBox.AddItem( "Lightness (CIE L*)" );
   Target_ComboBox.AddItem( "Luminance (CIE Y)" );
   Target_ComboBox.AddItem( "RGB/K components" );
   Target_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&UnsharpMaskInterface::__Target_ItemSelected, w );

   Target_Sizer.SetSpacing( 4 );
   Target_Sizer.Add( Target_Label );
   Target_Sizer.Add( Target_ComboBox );
   Target_Sizer.AddStretch();

   //

   Filter_Sizer.SetSpacing( 4 );
   Filter_Sizer.Add( Sigma_Sizer );
   Filter_Sizer.Add( Amount_NumericControl );
   Filter_Sizer.Add( Target_Sizer );

   Filter_Control.SetSizer( Filter_Sizer );
   Filter_Control.AdjustToContents();

   // -------------------------------------------------------------------------

   Deringing_SectionBar.SetTitle( "Deringing" );
   Deringing_SectionBar.EnableTitleCheckBox();
   Deringing_SectionBar.OnCheck( (SectionBar::check_event_handler)&UnsharpMaskInterface::__Deringing_Check, w );
   Deringing_SectionBar.SetSection( Deringing_Control );

   //

   DeringingDark_NumericControl.label.SetText( "Dark:" );
   DeringingDark_NumericControl.label.SetMinWidth( labelWidth1 );
   DeringingDark_NumericControl.slider.SetRange( 0, 100 );
   DeringingDark_NumericControl.SetReal();
   DeringingDark_NumericControl.SetRange( TheUSMDeringingDarkParameter->MinimumValue(), TheUSMDeringingDarkParameter->MaximumValue() );
   DeringingDark_NumericControl.SetPrecision( TheUSMDeringingDarkParameter->Precision() );
   DeringingDark_NumericControl.SetToolTip( "<p>Deringing regularization strength, dark ringing artifacts.</p>" );
   DeringingDark_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&UnsharpMaskInterface::__Deringing_ValueUpdated, w );

   DeringingBright_NumericControl.label.SetText( "Bright:" );
   DeringingBright_NumericControl.label.SetMinWidth( labelWidth1 );
   DeringingBright_NumericControl.slider.SetRange( 0, 100 );
   DeringingBright_NumericControl.SetReal();
   DeringingBright_NumericControl.SetRange( TheUSMDeringingBrightParameter->MinimumValue(), TheUSMDeringingBrightParameter->MaximumValue() );
   DeringingBright_NumericControl.SetPrecision( TheUSMDeringingBrightParameter->Precision() );
   DeringingBright_NumericControl.SetToolTip( "<p>Deringing regularization strength, bright ringing artifacts.</p>" );
   DeringingBright_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&UnsharpMaskInterface::__Deringing_ValueUpdated, w );

   OutputDeringingMaps_CheckBox.SetText( "Output deringing maps" );
   OutputDeringingMaps_CheckBox.SetToolTip( "<p>Generate an image window for each deringing map image. "
      "New image windows will be created for the dark and bright deringing maps, if the corresponding amount "
      "parameters are nonzero.</p>" );
   OutputDeringingMaps_CheckBox.OnClick( (Button::click_event_handler)&UnsharpMaskInterface::__Deringing_Click, w );

   OutputDeringingMaps_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   OutputDeringingMaps_Sizer.Add( OutputDeringingMaps_CheckBox );
   OutputDeringingMaps_Sizer.AddStretch();

   //

   Deringing_Sizer.SetSpacing( 4 );
   Deringing_Sizer.Add( DeringingDark_NumericControl );
   Deringing_Sizer.Add( DeringingBright_NumericControl );
   Deringing_Sizer.Add( OutputDeringingMaps_Sizer );

   Deringing_Control.SetSizer( Deringing_Sizer );

   // -------------------------------------------------------------------------

   Range_SectionBar.SetTitle( "Dynamic Range Extension" );
   Range_SectionBar.SetSection( Range_Control );

   //

   RangeLow_NumericControl.label.SetText( "Low Range:" );
   RangeLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeLow_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeLow_NumericControl.slider.SetRange( 0, 100 );
   RangeLow_NumericControl.SetReal();
   RangeLow_NumericControl.SetRange( TheUSMRangeLowParameter->MinimumValue(), TheUSMRangeLowParameter->MaximumValue() );
   RangeLow_NumericControl.SetPrecision( TheUSMRangeLowParameter->Precision() );
   RangeLow_NumericControl.SetToolTip( "<p>Shadows dynamic range extension.</p>" );
   RangeLow_NumericControl.edit.SetFixedWidth( editWidth1 );
   RangeLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&UnsharpMaskInterface::__Range_ValueUpdated, w );

   RangeHigh_NumericControl.label.SetText( "High Range:" );
   RangeHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeHigh_NumericControl.slider.SetRange( 0, 100 );
   RangeHigh_NumericControl.SetReal();
   RangeHigh_NumericControl.SetRange( TheUSMRangeHighParameter->MinimumValue(), TheUSMRangeHighParameter->MaximumValue() );
   RangeHigh_NumericControl.SetPrecision( TheUSMRangeHighParameter->Precision() );
   RangeHigh_NumericControl.SetToolTip( "<p>Highlights dynamic range extension.</p>" );
   RangeHigh_NumericControl.edit.SetFixedWidth( editWidth1 );
   RangeHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&UnsharpMaskInterface::__Range_ValueUpdated, w );

   //

   Range_Sizer.SetSpacing( 8 );
   Range_Sizer.Add( RangeLow_NumericControl );
   Range_Sizer.Add( RangeHigh_NumericControl );

   Range_Control.SetSizer( Range_Sizer );
   Range_Control.AdjustToContents();

   // -------------------------------------------------------------------------

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Filter_SectionBar );
   Global_Sizer.Add( Filter_Control );
   Global_Sizer.Add( Deringing_SectionBar );
   Global_Sizer.Add( Deringing_Control );
   Global_Sizer.Add( Range_SectionBar );
   Global_Sizer.Add( Range_Control );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetMinWidth();

   Deringing_Control.Hide();
   Range_Control.Hide();

   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&UnsharpMaskInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF UnsharpMaskInterface.cpp - Released 2018-12-12T09:25:24Z
