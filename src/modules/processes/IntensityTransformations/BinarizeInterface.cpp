//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0420
// ----------------------------------------------------------------------------
// BinarizeInterface.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "BinarizeInterface.h"
#include "BinarizeParameters.h"
#include "BinarizeProcess.h"

#include <pcl/Color.h>
#include <pcl/Graphics.h>
#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeInterface* TheBinarizeInterface = 0;

// ----------------------------------------------------------------------------

#include "BinarizeIcon.xpm"

// ----------------------------------------------------------------------------

BinarizeInterface::BinarizeInterface() :
ProcessInterface(), instance( TheBinarizeProcess ), GUI( 0 )
{
   TheBinarizeInterface = this;
}

BinarizeInterface::~BinarizeInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString BinarizeInterface::Id() const
{
   return "Binarize";
}

MetaProcess* BinarizeInterface::Process() const
{
   return TheBinarizeProcess;
}

const char** BinarizeInterface::IconImageXPM() const
{
   return BinarizeIcon_XPM;
}

InterfaceFeatures BinarizeInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

void BinarizeInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != 0 )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

void BinarizeInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void BinarizeInterface::ResetInstance()
{
   BinarizeInstance defaultInstance( TheBinarizeProcess );
   ImportProcess( defaultInstance );
}

bool BinarizeInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Binarize" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheBinarizeProcess;
}

ProcessImplementation* BinarizeInterface::NewProcess() const
{
   return new BinarizeInstance( instance );
}

bool BinarizeInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const BinarizeInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Binarize instance.";
   return false;
}

bool BinarizeInterface::RequiresInstanceValidation() const
{
   return true;
}

bool BinarizeInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

bool BinarizeInterface::WantsReadoutNotifications() const
{
   return true;
}

void BinarizeInterface::UpdateReadout( const View& v, const DPoint&, double R, double G, double B, double /*A*/ )
{
   if ( GUI != 0 && IsVisible() )
   {
      if ( instance.isGlobal )
      {
         RGBColorSystem rgbws;
         v.Window().GetRGBWS( rgbws );
         instance.level[0] = instance.level[1] = instance.level[2] = rgbws.Lightness( R, G, B );
      }
      else
      {
         instance.level[0] = R;
         instance.level[1] = G;
         instance.level[2] = B;
      }

      UpdateControls();

      if ( !RealTimePreview::IsUpdating() )
         RealTimePreview::Update();
   }
}

bool BinarizeInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int ) const
{
   return true;
}

// ----------------------------------------------------------------------------

BinarizeInterface::RealTimeThread::RealTimeThread() : Thread(), m_instance( TheBinarizeProcess )
{
}

void BinarizeInterface::RealTimeThread::Reset( const UInt16Image& image, const BinarizeInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

void BinarizeInterface::RealTimeThread::Run()
{
   MuteStatus status;
   m_image.SetStatusCallback( &status );
   m_instance.Preview( m_image );
   m_image.ResetSelections();
}

bool BinarizeInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int, String& ) const
{
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

void BinarizeInterface::UpdateControls()
{
   GUI->GlobalK_RadioButton.SetChecked( instance.isGlobal );
   GUI->RGB_RadioButton.SetChecked( !instance.isGlobal );

   GUI->V0_NumericControl.label.SetText( instance.isGlobal ? "RGB/K:" : "R/K:" );
   GUI->V0_NumericControl.SetValue( instance.level[0] );

   GUI->V1_NumericControl.Enable( !instance.isGlobal );
   GUI->V1_NumericControl.SetValue( instance.level[1] );

   GUI->V2_NumericControl.Enable( !instance.isGlobal );
   GUI->V2_NumericControl.SetValue( instance.level[2] );

   GUI->ColorSample_Control.Update();
}

void BinarizeInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != 0 )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void BinarizeInterface::__Mode_Click( Button& sender, bool checked )
{
   if ( sender == GUI->RGB_RadioButton )
      instance.isGlobal = false;
   else if ( sender == GUI->GlobalK_RadioButton )
   {
      instance.isGlobal = true;
      instance.level[1] = instance.level[2] = instance.level[0];
   }

   UpdateControls();
   UpdateRealTimePreview();
}

void BinarizeInterface::__LevelValues_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->V0_NumericControl )
   {
      instance.level[0] = value;

      if ( instance.isGlobal )
      {
         instance.level[1] = instance.level[2] = value;
         GUI->V1_NumericControl.SetValue( value );
         GUI->V2_NumericControl.SetValue( value );
      }
   }
   else if ( sender == GUI->V1_NumericControl )
      instance.level[1] = value;
   else if ( sender == GUI->V2_NumericControl )
      instance.level[2] = value;

   GUI->ColorSample_Control.Update();

   UpdateRealTimePreview();
}

void BinarizeInterface::__ColorSample_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   Graphics g( sender );
   g.SetBrush( RGBAColor( float( instance.level[0] ), float( instance.level[1] ), float( instance.level[2] ) ) );
   g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
   g.DrawRect( sender.BoundsRect() );
}

void BinarizeInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != 0 )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BinarizeInterface::GUIData::GUIData( BinarizeInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth = fnt.Width( String( "RGB/K:" ) + 'T' );

   //

   RGB_RadioButton.SetText( "Individual RGB/K channels" );
   RGB_RadioButton.SetToolTip( "Use an independent binarization threshold for each RGB/gray channel." );
   RGB_RadioButton.OnClick( (Button::click_event_handler)&BinarizeInterface::__Mode_Click, w );

   GlobalK_RadioButton.SetText( "Joint RGB/K channels" );
   GlobalK_RadioButton.SetToolTip( "Use a unique binarization threshold for all RGB/gray channels." );
   GlobalK_RadioButton.OnClick( (Button::click_event_handler)&BinarizeInterface::__Mode_Click, w );

   Mode_Sizer.SetSpacing( 8 );
   Mode_Sizer.Add( GlobalK_RadioButton );
   Mode_Sizer.Add( RGB_RadioButton );
   Mode_Sizer.AddStretch();

   //

   V0_NumericControl.label.SetText( "RGB/K:" );
   V0_NumericControl.label.SetFixedWidth( labelWidth );
   V0_NumericControl.slider.SetScaledMinWidth( 200 );
   V0_NumericControl.slider.SetRange( 0, 100 );
   V0_NumericControl.SetReal();
   V0_NumericControl.SetRange( TheBinarizeLevelRParameter->MinimumValue(), TheBinarizeLevelRParameter->MaximumValue() );
   V0_NumericControl.SetPrecision( TheBinarizeLevelRParameter->Precision() );
   V0_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BinarizeInterface::__LevelValues_ValueUpdated, w );

   //

   V1_NumericControl.label.SetText( "G:" );
   V1_NumericControl.label.SetFixedWidth( labelWidth );
   V1_NumericControl.slider.SetScaledMinWidth( 200 );
   V1_NumericControl.slider.SetRange( 0, 100 );
   V1_NumericControl.SetReal();
   V1_NumericControl.SetRange( TheBinarizeLevelGParameter->MinimumValue(), TheBinarizeLevelGParameter->MaximumValue() );
   V1_NumericControl.SetPrecision( TheBinarizeLevelGParameter->Precision() );
   V1_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BinarizeInterface::__LevelValues_ValueUpdated, w );

   //

   V2_NumericControl.label.SetText( "B:" );
   V2_NumericControl.label.SetFixedWidth( labelWidth );
   V2_NumericControl.slider.SetScaledMinWidth( 200 );
   V2_NumericControl.slider.SetRange( 0, 100 );
   V2_NumericControl.SetReal();
   V2_NumericControl.SetRange( TheBinarizeLevelBParameter->MinimumValue(), TheBinarizeLevelBParameter->MaximumValue() );
   V2_NumericControl.SetPrecision( TheBinarizeLevelBParameter->Precision() );
   V2_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BinarizeInterface::__LevelValues_ValueUpdated, w );

   //

   Level_Sizer.SetSpacing( 4 );
   Level_Sizer.Add( V0_NumericControl );
   Level_Sizer.Add( V1_NumericControl );
   Level_Sizer.Add( V2_NumericControl );

   //

   ColorSample_Control.SetScaledFixedWidth( 60 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&BinarizeInterface::__ColorSample_Paint, w );

   Threshold_Sizer.SetSpacing( 4 );
   Threshold_Sizer.Add( Level_Sizer, 100 );
   Threshold_Sizer.Add( ColorSample_Control );

   //

   Parameters_Sizer.SetMargin( 6 );
   Parameters_Sizer.SetSpacing( 4 );
   Parameters_Sizer.Add( Mode_Sizer );
   Parameters_Sizer.Add( Threshold_Sizer );

   Parameters_GroupBox.SetTitle( "Binarization Threshold" );
   Parameters_GroupBox.SetSizer( Parameters_Sizer );

   //

   Global_Sizer.SetMargin( 6 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( Parameters_GroupBox );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&BinarizeInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BinarizeInterface.cpp - Released 2018-11-23T18:45:58Z
