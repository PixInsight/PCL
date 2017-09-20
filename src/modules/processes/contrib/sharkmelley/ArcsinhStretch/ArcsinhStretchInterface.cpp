//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 00.00.01.0112
// ----------------------------------------------------------------------------
// ArcsinhStretchInterface.cpp - Released 2017-09-20T13:03:37Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017 Mark Shelley
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

#include "ArcsinhStretchInterface.h"
#include "ArcsinhStretchParameters.h"
#include "ArcsinhStretchProcess.h"

#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretchInterface* TheArcsinhStretchInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "ArcsinhStretchIcon.xpm"

// ----------------------------------------------------------------------------

ArcsinhStretchInterface::ArcsinhStretchInterface() :
   ProcessInterface(),
   instance( TheArcsinhStretchProcess )
{
   TheArcsinhStretchInterface = this;
}

// ----------------------------------------------------------------------------

ArcsinhStretchInterface::~ArcsinhStretchInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

InterfaceFeatures ArcsinhStretchInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

IsoString ArcsinhStretchInterface::Id() const
{
   return "ArcsinhStretch";
}

// ----------------------------------------------------------------------------

MetaProcess* ArcsinhStretchInterface::Process() const
{
   return TheArcsinhStretchProcess;
}

// ----------------------------------------------------------------------------

const char** ArcsinhStretchInterface::IconImageXPM() const
{
   return nullptr; // ArcsinhStretchIcon_XPM; ---> put a nice icon here
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInterface::WantsRealTimePreviewNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::RealTimePreviewOwnerChanged( ProcessInterface& iface )
{
   if ( GUI != nullptr )
   {
      bool ownedByMe = iface == *this && IsRealTimePreviewActive();
      GUI->EstimateBlackPoint_Button.Enable( ownedByMe );
      GUI->ParameterPreviewClipped_CheckBox.Enable( ownedByMe );
   }
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::RealTimePreviewUpdated(bool active)
{
   if ( GUI != nullptr )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the real time preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::ResetInstance()
{
   ArcsinhStretchInstance defaultInstance( TheArcsinhStretchProcess );
   ImportProcess( defaultInstance );
   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ArcsinhStretch" );
      UpdateSliderControls();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheArcsinhStretchProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ArcsinhStretchInterface::NewProcess() const
{
   return new ArcsinhStretchInstance( instance );
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const ArcsinhStretchInstance* r = dynamic_cast<const ArcsinhStretchInstance*>( &p );
   if ( r == nullptr )
   {
      whyNot = "Not a ArcsinhStretch instance.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateSliderControls();
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const
{
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ArcsinhStretchInterface::RealTimeThread::RealTimeThread() :
   m_instance( TheArcsinhStretchProcess )
{
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::RealTimeThread::Reset( const UInt16Image& image, const ArcsinhStretchInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::RealTimeThread::Run()
{
   MuteStatus status;
   m_image.SetStatusCallback( &status );
   m_instance.Preview( m_image );
   m_image.ResetSelections();
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int zoomLevel, String& ) const
{
   m_preview_blackpoint_level = double( image.MinimumPixelValue() )/image.MaximumPixelValue(); // mutable m_preview_blackpoint_level

   // Generate a histogram of pixel sample values
   UI64Vector histogram( uint64( 0 ), uint16_max+1 );
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      for ( UInt16Image::const_sample_iterator i( image, c ); i; ++i )
         ++histogram[*i];

   // Set the black point so that a 2% of pixel values in the preview window become clipped to zero
   int ihist = 0;
   for ( uint64 n2 = image.NumberOfNominalSamples()/50, sum = histogram[0]; sum < n2; sum += histogram[++ihist] ) {}
   if ( ihist == 0 )
      m_preview_blackpoint_level = 0.0;
   else
      m_preview_blackpoint_level = (ihist - 1)/65535.0;

   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      m_realTimeThread->Reset( image, instance );
      m_realTimeThread->Start();

      while ( m_realTimeThread->IsActive() )
      {
         ProcessEvents();

         if ( !IsRealTimePreviewActive() )
         {
            m_realTimeThread->Abort();
            m_realTimeThread->Wait();
            return false;
         }
      }

      if ( !m_realTimeThread->IsAborted() )
      {
         image.Assign( m_realTimeThread->m_image );
         return true;
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::UpdateControls()
{
   GUI->Stretch_NumericControl.SetValue( instance.p_stretch );
   GUI->BlackPoint_NumericControl.SetValue( instance.p_blackpoint );
   GUI->ParameterProtectHighlights_CheckBox.SetChecked( instance.p_protectHighlights );
   GUI->ParameterUseRgbws_CheckBox.SetChecked( instance.p_useRgbws );
   GUI->ParameterPreviewClipped_CheckBox.SetChecked( instance.p_previewClipped );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::UpdateSliderControls()
{
   GUI->FineAdjust_Slider.SetValue( 500 ); // Centre of the range 0-1000
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread.IsValid() )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Stretch_NumericControl )
      instance.p_stretch = value;
   if ( sender == GUI->BlackPoint_NumericControl )
      instance.p_blackpoint = value;

   UpdateSliderControls();
   UpdateRealTimePreview();

}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__SliderValueUpdated( Slider& sender, int value )
{
   if ( sender == GUI->FineAdjust_Slider )
   {
      instance.p_blackpoint = instance.m_blackpointBeforeAdjustment + double( value - 500 )/1000000; // value is in the range 0-1000
      if ( instance.p_blackpoint < 0 )
         instance.m_blackpointBeforeAdjustment -= instance.p_blackpoint;
      UpdateControls();
      UpdateRealTimePreview();
   }
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__FineAdjustSliderEnter( Control& sender )
{
   __FineAdjustSliderGetFocus( sender );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__FineAdjustSliderLeave( Control& sender )
{
   __FineAdjustSliderLoseFocus( sender );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__FineAdjustSliderMousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   __FineAdjustSliderGetFocus( sender );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__FineAdjustSliderMouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   __FineAdjustSliderLoseFocus( sender );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__FineAdjustSliderLoseFocus( Control& sender )
{
   GUI->FineAdjust_Slider.SetValue( 500 ); // Set it to the halfway point
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__FineAdjustSliderGetFocus( Control& sender )
{
   instance.m_blackpointBeforeAdjustment = instance.p_blackpoint;
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->ParameterProtectHighlights_CheckBox )
      instance.p_protectHighlights = checked;
   if ( sender == GUI->ParameterUseRgbws_CheckBox )
      instance.p_useRgbws = checked;
   if ( sender == GUI->ParameterPreviewClipped_CheckBox )
      instance.p_previewClipped = checked;
   if ( sender == GUI->EstimateBlackPoint_Button )
   {
      instance.p_blackpoint = m_preview_blackpoint_level;
      GUI->BlackPoint_NumericControl.SetValue( instance.p_blackpoint );
      UpdateSliderControls();
   }

   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread.IsValid() )
      if ( m_realTimeThread->IsActive() )
         return;
   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------

ArcsinhStretchInterface::GUIData::GUIData( ArcsinhStretchInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "XXXXXXX XXXXXXX" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 9 ) );

   //

   Stretch_NumericControl.label.SetText( "Stretch factor:" );
   Stretch_NumericControl.label.SetFixedWidth( labelWidth1 );
   Stretch_NumericControl.slider.SetScaledMinWidth( 250 );
   Stretch_NumericControl.slider.SetRange( 1, 1000 );
   Stretch_NumericControl.SetReal();
   Stretch_NumericControl.SetRange( TheArcsinhStretchParameter->MinimumValue(), TheArcsinhStretchParameter->MaximumValue() );
   Stretch_NumericControl.SetPrecision( TheArcsinhStretchParameter->Precision() );
   Stretch_NumericControl.SetToolTip( "<p>Multiplier applied to the faintest detail above the background level.</p>" );
   Stretch_NumericControl.edit.SetFixedWidth( editWidth1 );
   Stretch_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ArcsinhStretchInterface::__RealValueUpdated, w );

   //

   BlackPoint_NumericControl.label.SetText( "Black point:" );
   BlackPoint_NumericControl.label.SetFixedWidth( labelWidth1 );
   BlackPoint_NumericControl.slider.SetScaledMinWidth( 250 );
   BlackPoint_NumericControl.slider.SetRange( 0, 2000 );
   BlackPoint_NumericControl.SetReal();
   BlackPoint_NumericControl.SetRange( TheArcsinhStretchBlackPointParameter->MinimumValue(), TheArcsinhStretchBlackPointParameter->MaximumValue() );
   BlackPoint_NumericControl.SetPrecision( TheArcsinhStretchBlackPointParameter->Precision() );
   BlackPoint_NumericControl.SetToolTip( "<p>Constant value to subtract from the image. "
      "It is assumed that background extraction and/or neutralization has previously been performed on the image.</p>" );
   BlackPoint_NumericControl.edit.SetFixedWidth( editWidth1 );
   BlackPoint_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ArcsinhStretchInterface::__RealValueUpdated, w );

   //

   FineAdjust_Slider.SetScaledMinWidth( 250 );
   FineAdjust_Slider.SetRange( 0, 1000 ); // Do not change without examining the chain of consequences i.e. UpdateSliderControls and __SliderValueUpdated
   FineAdjust_Slider.SetStepSize( 1 );
   FineAdjust_Slider.SetToolTip( "<p>Black point fine adjustment slider with re-centering. "
      "The slider re-centers after being used so the next adjustment can be up or down.</p>" );
   FineAdjust_Slider.OnValueUpdated( (Slider::value_event_handler)&ArcsinhStretchInterface::__SliderValueUpdated, w );
   FineAdjust_Slider.OnEnter( (Control::event_handler)&ArcsinhStretchInterface::__FineAdjustSliderEnter, w ); // This allows us to re-centre the slider
   FineAdjust_Slider.OnLeave( (Control::event_handler)&ArcsinhStretchInterface::__FineAdjustSliderLeave, w ); // This allows us to re-centre the slider

   FineAdjust_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   FineAdjust_Sizer.Add( FineAdjust_Slider, 100 );

   //

   BlackPoint_Sizer.Add( BlackPoint_NumericControl );
   BlackPoint_Sizer.AddSpacing( 2 );
   BlackPoint_Sizer.Add( FineAdjust_Sizer );

   //

   ParameterProtectHighlights_CheckBox.SetText( "Protect highlights" );
   ParameterProtectHighlights_CheckBox.SetToolTip( "<p>Scale the image to prevent values exceeding 1.0 during stretch - to prevent highlight clipping.</p>" );
   ParameterProtectHighlights_CheckBox.OnClick( (pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w );

   ParameterProtectHighlights_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   ParameterProtectHighlights_Sizer.Add( ParameterProtectHighlights_CheckBox );
   ParameterProtectHighlights_Sizer.AddStretch();

   //

   ParameterUseRgbws_CheckBox.SetText( "Use RGB working space" );
   ParameterUseRgbws_CheckBox.SetToolTip( "<p>Use the RGB Working Space of the image to calculate luminance from RGB components. "
      "Otherwise R, G and B and equally weighted in the luminance calculation.</p>" );
   ParameterUseRgbws_CheckBox.OnClick( (pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w );

   ParameterUseRgbws_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   ParameterUseRgbws_Sizer.Add( ParameterUseRgbws_CheckBox );
   ParameterUseRgbws_Sizer.AddStretch();

   //

   ParameterPreviewClipped_CheckBox.SetText( "Highlight values clipped to zero" );
   ParameterPreviewClipped_CheckBox.SetToolTip( "<p>The real-time preview will highlight values that become clipped to zero "
      "when the black point is set. Otherwise the real-time preview shows values truncated to the [0,1] range.</p>" );
   ParameterPreviewClipped_CheckBox.OnClick( (pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Stretch_NumericControl );
   Global_Sizer.Add( BlackPoint_Sizer );
   Global_Sizer.Add( ParameterProtectHighlights_Sizer );
   Global_Sizer.Add( ParameterUseRgbws_Sizer );

   int labelWidth = fnt.Width( "Estimate Black Point" );
   EstimateBlackPoint_Button.SetMinWidth( labelWidth );
   EstimateBlackPoint_Button.SetText( "Estimate Black Point" );
   EstimateBlackPoint_Button.SetToolTip( "<p>Auto-estimate the black point level. The black point is set to a level that allows a "
      "2% of the values found in the real-time preview to become clipped to zero.</p>" );
   EstimateBlackPoint_Button.OnClick( (pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w );

   PreviewControls_Sizer.SetMargin( 6 );
   PreviewControls_Sizer.SetSpacing( 32 );
   PreviewControls_Sizer.Add( EstimateBlackPoint_Button );
   PreviewControls_Sizer.Add( ParameterPreviewClipped_CheckBox );

   PreviewControlsGroupBox.SetTitle( "Real-Time Preview" );
   PreviewControlsGroupBox.SetToolTip( "<p>Tools that operate only on the real-time preview.</p>" );
   PreviewControlsGroupBox.SetSizer( PreviewControls_Sizer );

   // Disable until a preview is activated
   EstimateBlackPoint_Button.Disable();
   ParameterPreviewClipped_CheckBox.Disable();

   Global_Sizer.Add( PreviewControlsGroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&ArcsinhStretchInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchInterface.cpp - Released 2017-09-20T13:03:37Z
