//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 01.00.00.0230
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationInterface.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2018 Zbynek Vrastil
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#include "LocalHistogramEqualizationInterface.h"
#include "LocalHistogramEqualizationProcess.h"
#include "LocalHistogramEqualizationParameters.h"

#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LocalHistogramEqualizationInterface* TheLocalHistogramEqualizationInterface = 0;

// ----------------------------------------------------------------------------

#include "LocalHistogramEqualizationIcon.xpm"

// ----------------------------------------------------------------------------

LocalHistogramEqualizationInterface::LocalHistogramEqualizationInterface() :
ProcessInterface(),
instance( TheLocalHistogramEqualizationProcess ), m_realTimeThread( 0 ), GUI( 0 )
{
   TheLocalHistogramEqualizationInterface = this;
}

LocalHistogramEqualizationInterface::~LocalHistogramEqualizationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString LocalHistogramEqualizationInterface::Id() const
{
   return "LocalHistogramEqualization";
}

// ----------------------------------------------------------------------------

MetaProcess* LocalHistogramEqualizationInterface::Process() const
{
   return TheLocalHistogramEqualizationProcess;
}

// ----------------------------------------------------------------------------

const char** LocalHistogramEqualizationInterface::IconImageXPM() const
{
   return LocalHistogramEqualizationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures LocalHistogramEqualizationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != 0 )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::ResetInstance()
{
   LocalHistogramEqualizationInstance defaultInstance( TheLocalHistogramEqualizationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool LocalHistogramEqualizationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "LocalHistogramEqualization" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheLocalHistogramEqualizationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* LocalHistogramEqualizationInterface::NewProcess() const
{
   return new LocalHistogramEqualizationInstance( instance );
}

// ----------------------------------------------------------------------------

bool LocalHistogramEqualizationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const LocalHistogramEqualizationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a LocalHistogramEqualization instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool LocalHistogramEqualizationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool LocalHistogramEqualizationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   UpdateRealTimePreview();
   return true;
}

// ----------------------------------------------------------------------------

bool LocalHistogramEqualizationInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int ) const
{
   return true;
}

// ----------------------------------------------------------------------------

LocalHistogramEqualizationInterface::RealTimeThread::RealTimeThread() :
Thread(), m_instance( TheLocalHistogramEqualizationProcess )
{
}

void LocalHistogramEqualizationInterface::RealTimeThread::Reset( const UInt16Image& image,
                                                                 const LocalHistogramEqualizationInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

void LocalHistogramEqualizationInterface::RealTimeThread::Run()
{
   m_instance.Preview( m_image );
}

bool LocalHistogramEqualizationInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int zoomLevel, String& ) const
{
   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      LocalHistogramEqualizationInstance previewInstance( instance );
      if ( zoomLevel < 0 )
         previewInstance.radius = Max( 2, previewInstance.radius/-zoomLevel );

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
            m_realTimeThread = 0;
            return false;
         }
      }

      if ( !m_realTimeThread->IsAborted() )
      {
         image.Assign( m_realTimeThread->m_image );

         delete m_realTimeThread;
         m_realTimeThread = 0;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::UpdateControls()
{
   GUI->Radius_NumericControl.SetValue( instance.radius );
   GUI->HistogramBins_ComboBox.SetCurrentItem( instance.histogramBins );
   GUI->SlopeLimit_NumericControl.SetValue( instance.slopeLimit );
   GUI->Amount_NumericControl.SetValue( instance.amount );
   GUI->CircularKernel_CheckBox.SetChecked( instance.circularKernel );
}

void LocalHistogramEqualizationInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != 0 )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Radius_NumericControl )
      instance.radius = (int)value;
   if ( sender == GUI->SlopeLimit_NumericControl )
      instance.slopeLimit = value;
   if ( sender == GUI->Amount_NumericControl )
      instance.amount = value;
   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::__ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->CircularKernel_CheckBox )
      instance.circularKernel = checked;
   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if (sender == GUI->HistogramBins_ComboBox )
      instance.histogramBins = itemIndex;
   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != 0 )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------

LocalHistogramEqualizationInterface::GUIData::GUIData( LocalHistogramEqualizationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Histogram Resolution:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 10 ) );

   Radius_NumericControl.label.SetText( "Kernel Radius:" );
   Radius_NumericControl.label.SetFixedWidth( labelWidth1 );
   Radius_NumericControl.slider.SetScaledMinWidth( 250 );
   Radius_NumericControl.slider.SetRange( 0, 248 );
   Radius_NumericControl.SetReal();
   Radius_NumericControl.SetRange( TheLHERadiusParameter->MinimumValue(), TheLHERadiusParameter->MaximumValue() );
   Radius_NumericControl.SetPrecision( 0 );
   Radius_NumericControl.SetToolTip( "<p>The radius (in pixels) of the circular or square kernel in which the local histogram is evaluated.</p>"
      "<p>For most images, values between 50 and 200 give the best result.</p>");
   Radius_NumericControl.edit.SetFixedWidth( editWidth1 );
   Radius_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalHistogramEqualizationInterface::__RealValueUpdated, w );

   //

   HistogramBins_Label.SetText( "Histogram Resolution:" );
   HistogramBins_Label.SetFixedWidth( labelWidth1 );
   HistogramBins_Label.SetToolTip( "<p>The resolution of the histogram used to evaluate contrast transfer function. Usually 8-bit resolution is enough. "
      "For large kernel size, you may try higher resolutions to improve precision, but the computation will be slower.</p>" );
   HistogramBins_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   HistogramBins_ComboBox.AddItem( "8-bit (256)" );
   HistogramBins_ComboBox.AddItem( "10-bit (1024)" );
   HistogramBins_ComboBox.AddItem( "12-bit (4096)" );

   HistogramBins_ComboBox.SetToolTip( HistogramBins_Label.ToolTip() );
   HistogramBins_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LocalHistogramEqualizationInterface::__ItemSelected, w );

   //

   SlopeLimit_NumericControl.label.SetText( "Contrast Limit:" );
   SlopeLimit_NumericControl.label.SetFixedWidth( labelWidth1 );
   SlopeLimit_NumericControl.slider.SetScaledMinWidth( 250 );
   SlopeLimit_NumericControl.slider.SetRange( 0, 126 );
   SlopeLimit_NumericControl.SetReal();
   SlopeLimit_NumericControl.SetRange( TheLHESlopeLimitParameter->MinimumValue(), TheLHESlopeLimitParameter->MaximumValue() );
   SlopeLimit_NumericControl.SetPrecision( TheLHESlopeLimitParameter->Precision() );
   SlopeLimit_NumericControl.SetToolTip( "<p>The maximal allowed slope of the local contrast transfer function. Value of 1 does not change original image at all. "
      "Higher values make an effect stronger. Too high values strenghten the noise in the image. Typical values are 1.5-3.0.</p>" );
   SlopeLimit_NumericControl.edit.SetFixedWidth( editWidth1 );
   SlopeLimit_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalHistogramEqualizationInterface::__RealValueUpdated, w );

   //

   Amount_NumericControl.label.SetText( "Amount:" );
   Amount_NumericControl.label.SetFixedWidth( labelWidth1 );
   Amount_NumericControl.slider.SetScaledMinWidth( 250 );
   Amount_NumericControl.slider.SetRange( 0, 200 );
   Amount_NumericControl.SetReal();
   Amount_NumericControl.SetRange( TheLHEAmountParameter->MinimumValue(), TheLHEAmountParameter->MaximumValue() );
   Amount_NumericControl.SetPrecision( TheLHEAmountParameter->Precision() );
   Amount_NumericControl.SetToolTip( "<p>Amount of the local histogram equalization. This parameter is used to blend processed image with original one. "
      "For example value of 0.75 means that resulting image is blend of 1/4 of the original with 3/4 of the processed image.</p>" );
   Amount_NumericControl.edit.SetFixedWidth( editWidth1 );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LocalHistogramEqualizationInterface::__RealValueUpdated, w );


   //

   CircularKernel_CheckBox.SetText( "Circular Kernel" );
   CircularKernel_CheckBox.SetToolTip( "<p>Use circular (instead of square) area around the pixel to evaluate histogram. Recommended.</p>" );
   CircularKernel_CheckBox.OnClick( (pcl::Button::click_event_handler)&LocalHistogramEqualizationInterface::__ItemClicked, w );

   CircularKernel_Sizer.Add( HistogramBins_Label );
   CircularKernel_Sizer.AddSpacing( 4 );
   CircularKernel_Sizer.Add( HistogramBins_ComboBox );
   CircularKernel_Sizer.AddSpacing( 20 );
   CircularKernel_Sizer.Add( CircularKernel_CheckBox );
   CircularKernel_Sizer.AddStretch();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Radius_NumericControl );
   Global_Sizer.Add( SlopeLimit_NumericControl );
   Global_Sizer.Add( Amount_NumericControl );
   Global_Sizer.Add( CircularKernel_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&LocalHistogramEqualizationInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationInterface.cpp - Released 2018-12-12T09:25:25Z
