//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0420
// ----------------------------------------------------------------------------
// ScreenTransferFunctionInterface.cpp - Released 2018-11-23T18:45:58Z
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

#include "HistogramTransformationInstance.h"
#include "ScreenTransferFunctionInterface.h"
#include "ScreenTransferFunctionProcess.h"

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/NumericControl.h>
#include <pcl/Settings.h>
#include <pcl/ViewList.h>

/*
 * Default AutoStretch parameters
 */
// Shadows clipping point in (normalized) MAD units from the median.
#define DEFAULT_AUTOSTRETCH_SCLIP        -2.80
// Target mean background in the [0,1] range.
#define DEFAULT_AUTOSTRETCH_TBGND         0.25
// Boost shadows clipping factor in the [0,1] range.
#define DEFAULT_AUTOSTRETCH_BOOST_SCLIP   0.75
// Boost target background factor in the [1,10] range.
#define DEFAULT_AUTOSTRETCH_BOOST_TBGND   2.00

namespace pcl
{

// ----------------------------------------------------------------------------

ScreenTransferFunctionInterface* TheScreenTransferFunctionInterface = 0;

// ----------------------------------------------------------------------------

#include "ScreenTransferFunctionIcon.xpm"

static const char* chToolTip[ 4 ] =
{ "Red/Gray Channel", "Green Channel", "Blue Channel", "CIE L* and CIE Y" };

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class STFParametersDialog : public Dialog
{
public:

   ScreenTransferFunctionInstance m_instance;

   STFParametersDialog( const ScreenTransferFunctionInstance& );

private:

   VerticalSizer  Global_Sizer;
      HorizontalSizer   Channel_Sizer[ 4 ];
         Label             Channel_Label[ 4 ];
         NumericEdit       C0_NumericEdit[ 4 ];
         NumericEdit       M_NumericEdit[ 4 ];
         NumericEdit       C1_NumericEdit[ 4 ];
      HorizontalSizer   Buttons_Sizer;
         PushButton        Reset_PushButton;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void __EditValueUpdated( NumericEdit& sender, double value );
   void __Button_Click( Button& sender, bool checked );
};

STFParametersDialog::STFParametersDialog( const ScreenTransferFunctionInstance& i ) : Dialog(), m_instance( i )
{
   static const char* chLabel[ 4 ] = { "R/K:", "G:", "B:", "L:" };

   int labelWidth = Font().Width( String( 'M', 4 ) );

   for ( int c = 0; c < 4; ++c )
   {
      Channel_Label[c].SetText( chLabel[c] );
      Channel_Label[c].SetFixedWidth( labelWidth );
      Channel_Label[c].SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      C0_NumericEdit[c].label.Hide();
      C0_NumericEdit[c].SetReal();
      C0_NumericEdit[c].SetRange( 0, 1 );
      C0_NumericEdit[c].SetPrecision( 6 );
      C0_NumericEdit[c].AdjustEditWidth();
      C0_NumericEdit[c].SetToolTip( String( "Shadows Clipping, " ) + chToolTip[c] );
      C0_NumericEdit[c].OnValueUpdated( (NumericEdit::value_event_handler)&STFParametersDialog::__EditValueUpdated, *this );

      M_NumericEdit[c].label.Hide();
      M_NumericEdit[c].SetReal();
      M_NumericEdit[c].SetRange( 0, 1 );
      M_NumericEdit[c].SetPrecision( 6 );
      M_NumericEdit[c].AdjustEditWidth();
      M_NumericEdit[c].SetToolTip( String( "Midtones Balance, " ) + chToolTip[c] );
      M_NumericEdit[c].OnValueUpdated( (NumericEdit::value_event_handler)&STFParametersDialog::__EditValueUpdated, *this );

      C1_NumericEdit[c].label.Hide();
      C1_NumericEdit[c].SetReal();
      C1_NumericEdit[c].SetRange( 0, 1 );
      C1_NumericEdit[c].SetPrecision( 6 );
      C1_NumericEdit[c].AdjustEditWidth();
      C1_NumericEdit[c].SetToolTip( String( "Highlights Clipping, " ) + chToolTip[c] );
      C1_NumericEdit[c].OnValueUpdated( (NumericEdit::value_event_handler)&STFParametersDialog::__EditValueUpdated, *this );

      Channel_Sizer[c].SetSpacing( 6 );
      Channel_Sizer[c].Add( Channel_Label[c] );
      Channel_Sizer[c].AddSpacing( 2 );
      Channel_Sizer[c].Add( C0_NumericEdit[c] );
      Channel_Sizer[c].Add( M_NumericEdit[c] );
      Channel_Sizer[c].Add( C1_NumericEdit[c] );
   }

   //

   Reset_PushButton.SetText( "Reset" );
   Reset_PushButton.OnClick( (Button::click_event_handler)&STFParametersDialog::__Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&STFParametersDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&STFParametersDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( Reset_PushButton );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Channel_Sizer[0] );
   Global_Sizer.Add( Channel_Sizer[1] );
   Global_Sizer.Add( Channel_Sizer[2] );
   Global_Sizer.Add( Channel_Sizer[3] );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "STF Parameters" );

   UpdateControls();
}

void STFParametersDialog::UpdateControls()
{
   for ( int c = 0; c < 4; ++c )
   {
      C0_NumericEdit[c].SetValue( m_instance[c].c0 );
      M_NumericEdit[c].SetValue( m_instance[c].m );
      C1_NumericEdit[c].SetValue( m_instance[c].c1 );
   }
}

void STFParametersDialog::__EditValueUpdated( NumericEdit& sender, double value )
{
   int c;
   for ( c = 0; ; )
   {
      STF& f = m_instance[c];

      if ( sender == C0_NumericEdit[c] )
      {
         if ( (f.c0 = value) > f.c1 )
            C1_NumericEdit[c].SetValue( f.c1 = 1 );
         break;
      }
      else if ( sender == M_NumericEdit[c] )
      {
         f.m = value;
         break;
      }
      else if ( sender == C1_NumericEdit[c] )
      {
         if ( (f.c1 = value) < f.c0 )
            C0_NumericEdit[c].SetValue( f.c0 = 0 );
         break;
      }

      if ( ++c == 4 )
         break;
   }
}

void STFParametersDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
   else if ( sender == Reset_PushButton )
   {
      for ( int c = 0; c < 4; ++c )
         m_instance[c].Reset();
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class STFAutoStretchDialog : public Dialog
{
public:

   View   view;
   double shadowsClipping;       // in sigma units
   double targetBackground;      // in [0,1]
   double boostClippingFactor;   // in [0,1]
   double boostBackgroundFactor; // in [1,10]

   STFAutoStretchDialog();

private:

   VerticalSizer  Global_Sizer;
      ViewList          Images_ViewList;
      HorizontalSizer   IncludePreviews_Sizer;
         CheckBox          IncludePreviews_CheckBox;
      NumericControl    ShadowsClipping_NumericControl;
      NumericControl    TargetBackground_NumericControl;
      NumericControl    BoostClippingFactor_NumericControl;
      NumericControl    BoostBackgroundFactor_NumericControl;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Reset_PushButton;
         PushButton        Load_PushButton;
         PushButton        Save_PushButton;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();
   void LoadSettings();
   void SaveSettings() const;

   void __Execute( Dialog& );
   void __ViewSelected( ViewList& sender, View& view );
   void __EditValueUpdated( NumericEdit& sender, double value );
   void __Button_Click( Button& sender, bool checked );
};

STFAutoStretchDialog::STFAutoStretchDialog() :
   Dialog(),
   view(),
   shadowsClipping( DEFAULT_AUTOSTRETCH_SCLIP ),
   targetBackground( DEFAULT_AUTOSTRETCH_TBGND ),
   boostClippingFactor( DEFAULT_AUTOSTRETCH_BOOST_SCLIP ),
   boostBackgroundFactor( DEFAULT_AUTOSTRETCH_BOOST_TBGND )
{
   int labelWidth1 = Font().Width( String( "Boost background factor:" ) + 'T' );
   int editWidth1 = Font().Width( String( '0', 10 ) );

   OnExecute( (Dialog::execute_event_handler)&STFAutoStretchDialog::__Execute, *this );

   Images_ViewList.GetAll();
   Images_ViewList.SetMinWidth( Font().Width( String( 'M', 40 ) ) );
   Images_ViewList.OnViewSelected( (ViewList::view_event_handler)&STFAutoStretchDialog::__ViewSelected, *this );

   IncludePreviews_CheckBox.SetText( "Include previews" );
   IncludePreviews_CheckBox.OnClick( (Button::click_event_handler)&STFAutoStretchDialog::__Button_Click, *this );
   IncludePreviews_CheckBox.SetChecked();

   IncludePreviews_Sizer.AddUnscaledSpacing( labelWidth1 + LogicalPixelsToPhysical( 4 ) );
   IncludePreviews_Sizer.Add( IncludePreviews_CheckBox );
   IncludePreviews_Sizer.AddStretch();

   ShadowsClipping_NumericControl.label.SetText( "Shadows clipping:" );
   ShadowsClipping_NumericControl.label.SetFixedWidth( labelWidth1 );
   ShadowsClipping_NumericControl.slider.SetScaledMinWidth( 250 );
   ShadowsClipping_NumericControl.slider.SetRange( 0, 200 );
   ShadowsClipping_NumericControl.SetReal();
   ShadowsClipping_NumericControl.SetRange( -10.0, 0.0 );
   ShadowsClipping_NumericControl.SetPrecision( 4 );
   ShadowsClipping_NumericControl.edit.SetFixedWidth( editWidth1 );
   ShadowsClipping_NumericControl.SetToolTip(
      "<p>Shadows clipping point, measured in sigma units from the median of the target image.</p>" );
   ShadowsClipping_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&STFAutoStretchDialog::__EditValueUpdated, *this );

   TargetBackground_NumericControl.label.SetText( "Target background:" );
   TargetBackground_NumericControl.label.SetFixedWidth( labelWidth1 );
   TargetBackground_NumericControl.slider.SetScaledMinWidth( 250 );
   TargetBackground_NumericControl.slider.SetRange( 0, 200 );
   TargetBackground_NumericControl.SetReal();
   TargetBackground_NumericControl.SetRange( 0.0, 1.0 );
   TargetBackground_NumericControl.SetPrecision( 4 );
   TargetBackground_NumericControl.edit.SetFixedWidth( editWidth1 );
   TargetBackground_NumericControl.SetToolTip(
      "<p>Mean background in the normalized range [0,1]. ScreenTransferFunction will calculate the required "
      "nonlinear midtones balance to achieve the specified mean background level on the screen representation "
      "of the target image.</p>" );
   TargetBackground_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&STFAutoStretchDialog::__EditValueUpdated, *this );

   BoostClippingFactor_NumericControl.label.SetText( "Boost clipping factor:" );
   BoostClippingFactor_NumericControl.label.SetFixedWidth( labelWidth1 );
   BoostClippingFactor_NumericControl.slider.SetScaledMinWidth( 250 );
   BoostClippingFactor_NumericControl.slider.SetRange( 0, 200 );
   BoostClippingFactor_NumericControl.SetReal();
   BoostClippingFactor_NumericControl.SetRange( 0.0, 1.0 );
   BoostClippingFactor_NumericControl.SetPrecision( 2 );
   BoostClippingFactor_NumericControl.edit.SetFixedWidth( editWidth1 );
   BoostClippingFactor_NumericControl.SetToolTip(
      "<p>Multiplying factor for the shadows clipping point, when a boosted AutoStretch is applied.</p>" );
   BoostClippingFactor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&STFAutoStretchDialog::__EditValueUpdated, *this );

   BoostBackgroundFactor_NumericControl.label.SetText( "Boost background factor:" );
   BoostBackgroundFactor_NumericControl.label.SetFixedWidth( labelWidth1 );
   BoostBackgroundFactor_NumericControl.slider.SetScaledMinWidth( 250 );
   BoostBackgroundFactor_NumericControl.slider.SetRange( 0, 180 );
   BoostBackgroundFactor_NumericControl.SetReal();
   BoostBackgroundFactor_NumericControl.SetRange( 1.0, 10.0 );
   BoostBackgroundFactor_NumericControl.SetPrecision( 2 );
   BoostBackgroundFactor_NumericControl.edit.SetFixedWidth( editWidth1 );
   BoostBackgroundFactor_NumericControl.SetToolTip(
      "<p>Multiplying factor for the target background, when a boosted AutoStretch is applied.</p>" );
   BoostBackgroundFactor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&STFAutoStretchDialog::__EditValueUpdated, *this );

   //

   Reset_PushButton.SetText( "Reset" );
   Reset_PushButton.SetToolTip( "<p>Load a factory-default set of AutoStretch settings.</p>" );
   Reset_PushButton.OnClick( (Button::click_event_handler)&STFAutoStretchDialog::__Button_Click, *this );

   Load_PushButton.SetText( "Load" );
   Load_PushButton.SetToolTip( "<p>Reload the current set of default AutoStretch settings.</p>" );
   Load_PushButton.OnClick( (Button::click_event_handler)&STFAutoStretchDialog::__Button_Click, *this );

   Save_PushButton.SetText( "Save" );
   Save_PushButton.SetToolTip( "<p>Save the current set of AutoStretch settings as default.</p>" );
   Save_PushButton.OnClick( (Button::click_event_handler)&STFAutoStretchDialog::__Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&STFAutoStretchDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&STFAutoStretchDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( Reset_PushButton );
   Buttons_Sizer.Add( Load_PushButton );
   Buttons_Sizer.Add( Save_PushButton );
   Buttons_Sizer.AddSpacing( 32 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Images_ViewList );
   Global_Sizer.Add( IncludePreviews_Sizer );
   Global_Sizer.Add( ShadowsClipping_NumericControl );
   Global_Sizer.Add( TargetBackground_NumericControl );
   Global_Sizer.Add( BoostClippingFactor_NumericControl );
   Global_Sizer.Add( BoostBackgroundFactor_NumericControl );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "STF Auto Stretch" );

   LoadSettings();
   UpdateControls();
}

#define KEY_STF   TheScreenTransferFunctionInterface->SettingsKey()

void STFAutoStretchDialog::LoadSettings()
{
   IsoString key = KEY_STF + "AutoStretch/";
   Settings::Read( key + "ShadowsClipping", shadowsClipping );
   Settings::Read( key + "TargetBackground", targetBackground );
   Settings::Read( key + "BoostClippingFactor", boostClippingFactor );
   Settings::Read( key + "BoostBackgroundFactor", boostBackgroundFactor );
}

void STFAutoStretchDialog::SaveSettings() const
{
   IsoString key = KEY_STF + "AutoStretch/";
   Settings::Write( key + "ShadowsClipping", shadowsClipping );
   Settings::Write( key + "TargetBackground", targetBackground );
   Settings::Write( key + "BoostClippingFactor", boostClippingFactor );
   Settings::Write( key + "BoostBackgroundFactor", boostBackgroundFactor );
}

void STFAutoStretchDialog::UpdateControls()
{
   ShadowsClipping_NumericControl.SetValue( shadowsClipping );
   TargetBackground_NumericControl.SetValue( targetBackground );
   BoostClippingFactor_NumericControl.SetValue( boostClippingFactor );
   BoostBackgroundFactor_NumericControl.SetValue( boostBackgroundFactor );

   if ( view.IsNull() )
   {
      OK_PushButton.Disable();
      Cancel_PushButton.SetDefault();
   }
   else
   {
      OK_PushButton.Enable();
      OK_PushButton.SetDefault();
   }
}

void STFAutoStretchDialog::__Execute( Dialog& )
{
   view = ImageWindow::ActiveWindow().CurrentView();
   if ( view.IsPreview() && !IncludePreviews_CheckBox.IsChecked() )
      view = view.Window().MainView();

   Images_ViewList.SelectView( view );

   UpdateControls();
}

void STFAutoStretchDialog::__ViewSelected( ViewList& sender, View& v )
{
   view = v;
   UpdateControls();
}

void STFAutoStretchDialog::__EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == ShadowsClipping_NumericControl )
      shadowsClipping = value;
   else if ( sender == TargetBackground_NumericControl )
      targetBackground = value;
   else if ( sender == BoostClippingFactor_NumericControl )
      boostClippingFactor = value;
   else if ( sender == BoostBackgroundFactor_NumericControl )
      boostBackgroundFactor = value;
}

void STFAutoStretchDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
   {
      if ( !view.IsNull() )
         Ok();
   }
   else if ( sender == Cancel_PushButton )
   {
      Cancel();
   }
   else if ( sender == Save_PushButton )
   {
      SaveSettings();
   }
   else if ( sender == Load_PushButton )
   {
      LoadSettings();
      UpdateControls();
   }
   else if ( sender == Reset_PushButton )
   {
      shadowsClipping = DEFAULT_AUTOSTRETCH_SCLIP;
      targetBackground = DEFAULT_AUTOSTRETCH_TBGND;
      boostClippingFactor = DEFAULT_AUTOSTRETCH_BOOST_SCLIP;
      boostBackgroundFactor = DEFAULT_AUTOSTRETCH_BOOST_TBGND;
      UpdateControls();
   }
   else if ( sender == IncludePreviews_CheckBox )
   {
      if ( !checked )
         if ( view.IsPreview() )
            view = view.Window().MainView();

      Images_ViewList.Regenerate( true, checked );
      Images_ViewList.SelectView( view );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ScreenTransferFunctionInterface::ScreenTransferFunctionInterface() :
   m_instance( TheScreenTransferFunctionProcess ),
   m_mode( EditMode ),
   m_readoutMode( NoReadout ),
   m_rgbLinked( true )
{
   TheScreenTransferFunctionInterface = this;
}

// ----------------------------------------------------------------------------

ScreenTransferFunctionInterface::~ScreenTransferFunctionInterface()
{
   if ( m_autoAdjustDialog != nullptr )
      delete m_autoAdjustDialog, m_autoAdjustDialog = nullptr;
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ScreenTransferFunctionInterface::Id() const
{
   return "ScreenTransferFunction";
}

// ----------------------------------------------------------------------------

MetaProcess* ScreenTransferFunctionInterface::Process() const
{
   return TheScreenTransferFunctionProcess;
}

// ----------------------------------------------------------------------------

const char** ScreenTransferFunctionInterface::IconImageXPM() const
{
   return ScreenTransferFunctionIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures ScreenTransferFunctionInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ApplyInstance() const
{
   /*
    * ### N.B.: We cannot use LaunchOnCurrentView() here because
    * ApplyInstance() is invoked to modify the current view's STF in real time.
    */
   //m_instance.LaunchOnCurrentView();

   View v = ImageWindow::ActiveWindow().CurrentView();
   if ( !v.IsNull() )
      const_cast<ScreenTransferFunctionInterface*>( this )->m_instance.ExecuteOn( v );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
   {
      bool rgb = true;

      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
         {
            View v = w.CurrentView();

            if ( !v.IsColor() )
               rgb = false;

            m_instance.GetViewSTF( v );
            UpdateControls();
            UpdateTitle( v );
            UpdateSTFEnabledButton( v );

            goto updateSliders;
         }
      }

      UpdateTitle( View::Null() );
      UpdateSTFEnabledButton( View::Null() );

updateSliders:
      for ( int c = 0; c < 4; ++c )
         GUI->Sliders_Control[c].SetChannel( c, rgb );

      UpdateFirstChannelLabelText();
   }
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ResetInstance()
{
   ScreenTransferFunctionInstance defaultInstance( TheScreenTransferFunctionProcess );
   ImportProcess( defaultInstance );
   for ( int i = 0; i < 4; ++i )
      GUI->Sliders_Control[i].SetVisibleRange( 0, 1 );
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      UpdateTitle( View::Null() );
      UpdateControls();
      UpdateSTFEnabledButton( View::Null() );
      SetWorkingMode( EditMode );
      GUI->LinkRGB_ToolButton.SetChecked( m_rgbLinked );
   }

   dynamic = false;
   return &P == TheScreenTransferFunctionProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ScreenTransferFunctionInterface::NewProcess() const
{
   return new ScreenTransferFunctionInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ScreenTransferFunctionInstance*>( &p ) != nullptr )
      return true;
   if ( dynamic_cast<const HistogramTransformationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Must be an instance of either ScreenTransferFunction or HistogramTransformation.";
   return false;
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInterface::ImportProcess( const ProcessImplementation& p )
{
   const HistogramTransformationInstance* h = dynamic_cast<const HistogramTransformationInstance*>( &p );
   if ( h != nullptr )
   {
      for ( int c = 0; c < 4; ++c )
         m_instance[c].Reset();

      /*
       * Notes on importing instances to the STF interface:
       *
       * - We import combined RGB/K channel parameters if:
       *
       *    - Track view is active and the active view is a grayscale image.
       *
       *    - Track view is not active, or track view is active and the active
       *      view is a RGB color image, and RGB/K parameters define a
       *      non-identity transformation.
       *
       * - We import individual RGB/K channel parameters otherwise.
       *
       * - Luminance parameters are not assigned, since they are not defined in
       *   HistogramTransformation.
       *
       * - Dynamic range extension parameters are not assigned, since they are
       *   not available on the standard STF interface.
       */

      bool isColor = true;
      if ( IsTrackViewActive() )
      {
         View v = ImageWindow::ActiveWindow().CurrentView();
         if ( !v.IsNull() )
            isColor = v.IsColor();
      }

      if ( !isColor || h->MidtonesBalance( 3 ) != 0.5 ||
                       h->ShadowsClipping( 3 ) != 0 ||
                       h->HighlightsClipping( 3 ) != 1 )
      {
         for ( int c = 0; c < 3; ++c )
         {
            STF& f = m_instance[c];
            f.m = h->MidtonesBalance( 3 );
            f.c0 = h->ShadowsClipping( 3 );
            f.c1 = h->HighlightsClipping( 3 );
         }
      }
      else
      {
         for ( int c = 0; c < 3; ++c )
         {
            STF& f = m_instance[c];
            f.m = h->MidtonesBalance( c );
            f.c0 = h->ShadowsClipping( c );
            f.c1 = h->HighlightsClipping( c );
         }
      }
   }
   else
      m_instance.Assign( p );

   UpdateControls();

   if ( IsTrackViewActive() )
      ApplyToCurrentView();

   return true;
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ImageFocused( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
      {
         m_instance.GetViewSTF( v );

         UpdateTitle( v );
         UpdateControls();

         bool rgb = v.IsNull() || v.IsColor();
         for ( int c = 0; c < 4; ++c )
            GUI->Sliders_Control[c].SetChannel( c, rgb );

         UpdateSTFEnabledButton( v );
         UpdateFirstChannelLabelText();
      }
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( v == ImageWindow::ActiveWindow().CurrentView() )
         {
            bool rgb = v.IsColor();
            for ( int c = 0; c < 4; ++c )
               GUI->Sliders_Control[c].SetChannel( c, rgb );

            UpdateFirstChannelLabelText();
         }
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ImageSTFEnabled( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( v == ImageWindow::ActiveWindow().CurrentView() )
            UpdateSTFEnabledButton( v );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ImageSTFDisabled( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( v == ImageWindow::ActiveWindow().CurrentView() )
            UpdateSTFEnabledButton( v );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ImageSTFUpdated( const View& v )
{
   PCL_CLASS_REENTRANCY_GUARDED_BEGIN

   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( v == ImageWindow::ActiveWindow().CurrentView() )
         {
            m_instance.GetViewSTF( v );
            UpdateControls();
         }

   PCL_REENTRANCY_GUARDED_END
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ImageRenamed( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( v == ImageWindow::ActiveWindow().CurrentView() )
            UpdateTitle( v );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ImageDeleted( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( v == ImageWindow::ActiveWindow().CurrentView() )
         {
            UpdateTitle( View::Null() );
            UpdateSTFEnabledButton( View::Null() );
         }
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::UpdateReadout( const View& v, const DPoint&, double R, double G, double B, double /*A*/ )
{
   PCL_CLASS_REENTRANCY_GUARDED_BEGIN

   if ( GUI != nullptr )
      if ( m_readoutMode != NoReadout )
         if ( IsVisible() )
         {
            ImageWindow w = v.Window();

            int dispCh = w.CurrentChannel();
            bool chR = dispCh == DisplayChannel::Red || dispCh == DisplayChannel::RGBK && !v.IsColor();
            bool chG = dispCh == DisplayChannel::Green;
            bool chB = dispCh == DisplayChannel::Blue;
            bool chL = dispCh == DisplayChannel::CIE_L;
            bool chY = dispCh == DisplayChannel::CIE_Y;
            bool chRGB = !(chR || chG || chB || chL || chY);

            double L = 0; // calm compilers
            if ( chL || chY )
            {
               RGBColorSystem rgb;
               w.GetRGBWS( rgb );
               L = chL ? rgb.CIEL( R, G, B ) : rgb.CIEY( R, G, B );
            }

            switch ( m_readoutMode )
            {
            case BlackPointReadout:
               if ( chR || chRGB )
                  m_instance[0].c0 = R;
               if ( chG || chRGB )
                  m_instance[1].c0 = G;
               if ( chB || chRGB )
                  m_instance[2].c0 = B;
               if ( chL )
                  m_instance[3].c0 = L;
               break;
            case MidtonesReadout:
               if ( chR || chRGB )
                  m_instance[0].m = R;
               if ( chG || chRGB )
                  m_instance[1].m = G;
               if ( chB || chRGB )
                  m_instance[2].m = B;
               if ( chL )
                  m_instance[3].m = L;
               break;
            case WhitePointReadout:
               if ( chR || chRGB )
                  m_instance[0].c1 = R;
               if ( chG || chRGB )
                  m_instance[1].c1 = G;
               if ( chB || chRGB )
                  m_instance[2].c1 = B;
               if ( chL )
                  m_instance[3].c1 = L;
               break;
            default:
               break;
            }

            UpdateControls();

            if ( IsTrackViewActive() )
               ApplyToCurrentView();
         }

   PCL_REENTRANCY_GUARDED_END
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::UpdateTitle( const View& v )
{
   String title = "ScreenTransferFunction";
   if ( !v.IsNull() )
      title += ": " + v.FullId();
   SetWindowTitle( title );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::UpdateControls()
{
   for ( int c = 0; c < 4; ++c )
      GUI->Sliders_Control[c].SetValues( m_instance[c] );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::UpdateFirstChannelLabelText()
{
   if ( GUI != nullptr )
      GUI->ChannelId_Label[0].SetText( GUI->Sliders_Control[0].IsRGB() ? "R:" : "K:" );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::UpdateSTFEnabledButton( const View& v )
{
   if ( !v.IsNull() )
   {
      bool stfEnabled = v.AreScreenTransferFunctionsEnabled();
      GUI->STFEnabled_ToolButton.Enable();
      GUI->STFEnabled_ToolButton.SetChecked( stfEnabled );
      GUI->STFEnabled_ToolButton.SetToolTip( stfEnabled ? "STF Enabled" : "STF Disabled" );
   }
   else
   {
      GUI->STFEnabled_ToolButton.Disable();
      GUI->STFEnabled_ToolButton.SetChecked( false );
      GUI->STFEnabled_ToolButton.SetToolTip( "Unknown STF State" );
   }
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::SetWorkingMode( working_mode newMode )
{
   Cursor::std_cursor c;
   switch ( newMode )
   {
   default:
   case EditMode:
      c = StdCursor::UpArrow;
      GUI->EditMode_ToolButton.SetChecked( true );
      GUI->ZoomInMode_ToolButton.SetChecked( false );
      GUI->ZoomOutMode_ToolButton.SetChecked( false );
      GUI->PanMode_ToolButton.SetChecked( false );
      break;
   case ZoomInMode:
      c = StdCursor::SquarePlus;
      GUI->EditMode_ToolButton.SetChecked( false );
      GUI->ZoomInMode_ToolButton.SetChecked( true );
      GUI->ZoomOutMode_ToolButton.SetChecked( false );
      GUI->PanMode_ToolButton.SetChecked( false );
      break;
   case ZoomOutMode:
      c = StdCursor::SquareMinus;
      GUI->EditMode_ToolButton.SetChecked( false );
      GUI->ZoomInMode_ToolButton.SetChecked( false );
      GUI->ZoomOutMode_ToolButton.SetChecked( true );
      GUI->PanMode_ToolButton.SetChecked( false );
      break;
   case PanMode:
      c = StdCursor::OpenHand;
      GUI->EditMode_ToolButton.SetChecked( false );
      GUI->ZoomInMode_ToolButton.SetChecked( false );
      GUI->ZoomOutMode_ToolButton.SetChecked( false );
      GUI->PanMode_ToolButton.SetChecked( true );
      break;
   }

   for ( int i = 0; i < 4; ++i )
      GUI->Sliders_Control[i].SetCursor( c );

   m_mode = newMode;
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::LinkRGBChannels( bool linked )
{
   m_rgbLinked = linked;
   if ( GUI != nullptr )
      GUI->LinkRGB_ToolButton.SetChecked( m_rgbLinked );
}

void ScreenTransferFunctionInterface::ComputeAutoStretch( View& view, bool boost )
{
   if ( view.IsNull() )
      return;

   /*
    * Get statistics from reserved view properties.
    */

   DVector mad = view.ComputeOrFetchProperty( "MAD" ).ToDVector() * 1.4826;
   DVector median = view.ComputeOrFetchProperty( "Median" ).ToDVector();

   int n = Min( 3, median.Length() );

   if ( m_autoAdjustDialog == 0 )
      m_autoAdjustDialog = new STFAutoStretchDialog;

   double shadowsClipping = m_autoAdjustDialog->shadowsClipping;
   double targetBackground = m_autoAdjustDialog->targetBackground;
   if ( boost )
   {
      shadowsClipping *= m_autoAdjustDialog->boostClippingFactor;
      targetBackground *= m_autoAdjustDialog->boostBackgroundFactor;
   }

   if ( m_rgbLinked )
   {
      /*
       * Try to find how many channels look as channels of an inverted image.
       * We know a channel has been inverted because the main histogram peak is
       * located over the right-hand half of the histogram. Seems simplistic
       * but this is consistent with astronomical images.
       */
      int invertedChannels = 0;
      for ( int c = 0; c < n; ++c )
         if ( median[c] > 0.5 )
            ++invertedChannels;

      if ( invertedChannels < n )
      {
         /*
          * Noninverted image
          */
         double c0 = 0, m = 0;
         for ( int c = 0; c < n; ++c )
         {
            if ( 1 + mad[c] != 1 )
               c0 += median[c] + shadowsClipping * mad[c];
            m  += median[c];
         }
         c0 = Range( c0/n, 0.0, 1.0 );
         m = HistogramTransformation::MTF( targetBackground, m/n - c0 );

         for ( int c = 0; c < 3; ++c )
         {
            STF& f = m_instance[c];
            f.c0 = c0;
            f.m  = m;
            f.c1 = 1.0;
         }
      }
      else
      {
         /*
          * Inverted image
          */
         double c1 = 0, m = 0;
         for ( int c = 0; c < n; ++c )
         {
            m  += median[c];
            if ( 1 + mad[c] != 1 )
               c1 += median[c] - shadowsClipping * mad[c];
            else
               c1 += 1;
         }
         c1 = Range( c1/n, 0.0, 1.0 );
         m = HistogramTransformation::MTF( c1 - m/n, targetBackground );

         for ( int c = 0; c < 3; ++c )
         {
            STF& f = m_instance[c];
            f.c0 = 0;
            f.m  = m;
            f.c1 = c1;
         }
      }
   }
   else
   {
      /*
       * Unlinked RGB channnels: Compute automatic stretch functions for
       * individual RGB channels separately.
       */
      for ( int c = 0; c < n; ++c )
      {
         if ( median[c] < 0.5 )
         {
            /*
             * Noninverted channel
             */
            STF& f = m_instance[c];
            f.c0 = (1 + mad[c] != 1) ? Range( median[c] + shadowsClipping * mad[c], 0.0, 1.0 ) : 0.0;
            f.m  = HistogramTransformation::MTF( targetBackground, median[c] - f.c0 );
            f.c1 = 1.0;
         }
         else
         {
            /*
             * Inverted channel
             */
            STF& f = m_instance[c];
            f.c1 = (1 + mad[c] != 1) ? Range( median[c] - shadowsClipping * mad[c], 0.0, 1.0 ) : 1.0;
            f.m  = HistogramTransformation::MTF( f.c1 - median[c], targetBackground );
            f.c0 = 0;
         }
      }
   }

   if ( GUI != nullptr ) // can be called before initialization
      UpdateControls();
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::ApplyTo( View& view ) const
{
   PCL_CLASS_REENTRANCY_GUARDED_BEGIN // prevent reentrant ImageSTFUpdated() and UpdateReadout() events

   if ( !view.IsNull() )
      const_cast<ScreenTransferFunctionInterface*>( this )->m_instance.ApplyTo( view );

   PCL_REENTRANCY_GUARDED_END
}

void ScreenTransferFunctionInterface::ApplyToCurrentView() const
{
   PCL_CLASS_REENTRANCY_GUARDED_BEGIN // prevent reentrant ImageSTFUpdated() and UpdateReadout() events

   View currentView = ImageWindow::ActiveWindow().CurrentView();
   if ( !currentView.IsNull() )
      const_cast<ScreenTransferFunctionInterface*>( this )->m_instance.ApplyTo( currentView );

   ProcessEvents();

   PCL_REENTRANCY_GUARDED_END
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ScreenTransferFunctionInterface::__SliderValueUpdated( STFSliders& sender,
                     int c, int item, double value, unsigned modifiers, bool final )
{
   STF& f = m_instance[c];

   if ( !m_rgbLinked || c == 3 )
   {
      switch ( item )
      {
      case 0: f.m = value; break;
      case 1: f.c0 = value; break;
      case 2: f.c1 = value; break;
      }
   }
   else
   {
      double delta = 0;
      switch ( item )
      {
      case 0: delta = value - f.m; break;
      case 1: delta = value - f.c0; break;
      case 2: delta = value - f.c1; break;
      }

      for ( int i = 0; i < 3; ++i )
      {
         STF& fi = m_instance[i];

         double vi;

         if ( i != c )
         {
            switch ( item )
            {
            default:
            case 0: vi = fi.m; break;
            case 1: vi = fi.c0; break;
            case 2: vi = fi.c1; break;
            }

            vi = Range( vi+delta, 0.0, 1.0 );
         }
         else
            vi = value;

         switch ( item )
         {
         default:
         case 0: fi.m = vi; break;
         case 1: fi.c0 = vi; break;
         case 2: fi.c1 = vi; break;
         }

         if ( i != c )
            GUI->Sliders_Control[i].SetValues( fi );
      }
   }

   if ( (modifiers & KeyModifier::Shift) == 0 || final )
      if ( IsTrackViewActive() )
         ApplyToCurrentView();
}

void ScreenTransferFunctionInterface::__SliderRangeUpdated( STFSliders& sender,
                           int c, double v0, double v1, unsigned modifiers )
{
   if ( m_rgbLinked && c != 3 )
      for ( int i = 0; i < 3; ++i )
         GUI->Sliders_Control[i].SetVisibleRange( v0, v1 );
}

void ScreenTransferFunctionInterface::__STFAutoStretch_MouseRelease(
                           Control& sender, const pcl::Point& /*pos*/, int button,
                           unsigned /*buttons*/, unsigned modifiers )
{
   if ( sender != GUI->Auto_ToolButton )
      return;

   if ( button != MouseButton::Left )
      return;

   if ( m_autoAdjustDialog == nullptr )
      m_autoAdjustDialog = new STFAutoStretchDialog;

   View v;
   if ( modifiers & KeyModifier::Control )
   {
      if ( !m_autoAdjustDialog->Execute() )
         return;
      v = m_autoAdjustDialog->view;
      if ( v.IsNull() )
         return;
      ProcessEvents(); // to hide the dialog
   }
   else
   {
      v = ImageWindow::ActiveWindow().CurrentView();
      if ( v.IsNull() )
      {
         MessageBox( "Cannot apply STF AutoStretch because there is no view available.",
                     "ScreenTransferFunction",
                     StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }
   }

   v.Window().BringToFront();
   v.Window().SelectView( v );

   ActivateTrackView();

   ComputeAutoStretch( v, modifiers & KeyModifier::Shift );

   if ( IsTrackViewActive() )
      ApplyToCurrentView();
}

void ScreenTransferFunctionInterface::__ModeButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->LinkRGB_ToolButton )
      m_rgbLinked = checked;
   else if ( sender == GUI->Zoom11_ToolButton )
      for ( int i = 0; i < 4; ++i )
         GUI->Sliders_Control[i].SetVisibleRange( 0, 1 );
   else if ( sender == GUI->Edit_ToolButton )
   {
      STFParametersDialog d( m_instance );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         m_instance = d.m_instance;
         UpdateControls();
         if ( IsTrackViewActive() )
            ApplyToCurrentView();
      }
   }
   else if ( sender == GUI->EditMode_ToolButton )
      SetWorkingMode( EditMode );
   else if ( sender == GUI->ZoomInMode_ToolButton )
      SetWorkingMode( ZoomInMode );
   else if ( sender == GUI->ZoomOutMode_ToolButton )
      SetWorkingMode( ZoomOutMode );
   else if ( sender == GUI->PanMode_ToolButton )
      SetWorkingMode( PanMode );
}

void ScreenTransferFunctionInterface::__ResetButtonClick( Button& sender, bool /*checked*/ )
{
   int c;
   for ( c = 0; ; )
   {
      if ( sender == GUI->Reset_ToolButton[c] )
         break;
      if ( ++c == 4 )
         return;
   }

   m_instance[c].Reset();
   GUI->Sliders_Control[c].SetValues( m_instance[c] );

   if ( IsTrackViewActive() )
      ApplyToCurrentView();
}

void ScreenTransferFunctionInterface::__ReadoutButtonClick( Button& sender, bool checked )
{
   m_readoutMode = NoReadout;
   if ( checked )
      if ( sender == GUI->ReadoutShadows_ToolButton )
      {
         GUI->ReadoutMidtones_ToolButton.Uncheck();
         GUI->ReadoutHighlights_ToolButton.Uncheck();
         m_readoutMode = BlackPointReadout;
      }
      else if ( sender == GUI->ReadoutMidtones_ToolButton )
      {
         GUI->ReadoutShadows_ToolButton.Uncheck();
         GUI->ReadoutHighlights_ToolButton.Uncheck();
         m_readoutMode = MidtonesReadout;
      }
      else if ( sender == GUI->ReadoutHighlights_ToolButton )
      {
         GUI->ReadoutShadows_ToolButton.Uncheck();
         GUI->ReadoutMidtones_ToolButton.Uncheck();
         m_readoutMode = WhitePointReadout;
      }
}

void ScreenTransferFunctionInterface::__STFEnabledButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   if ( IsTrackViewActive() )
   {
      View v = ImageWindow::ActiveWindow().CurrentView();
      if ( !v.IsNull() )
      {
         v.EnableScreenTransferFunctions( !v.AreScreenTransferFunctionsEnabled() );
         UpdateSTFEnabledButton( v );
      }
   }
}

void ScreenTransferFunctionInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( dynamic_cast<STFSliders*>( &sender ) != nullptr )
      wantsView = true;
}

void ScreenTransferFunctionInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( dynamic_cast<STFSliders*>( &sender ) != nullptr )
   {
      m_instance.GetViewSTF( view );
      UpdateControls();
      bool rgb = view.IsColor();
      for ( int c = 0; c < 4; ++c )
         GUI->Sliders_Control[c].SetChannel( c, rgb );
      if ( IsTrackViewActive() )
         ApplyToCurrentView();
   }
}

// ----------------------------------------------------------------------------

ScreenTransferFunctionInterface::GUIData::GUIData( ScreenTransferFunctionInterface& w )
{
   static const char* chLabel[ 4 ] = { "R:", "G:", "B:", "L:" };

   int labelWidth = w.Font().Width( "G:" ) + 2;

   LinkRGB_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/link.png" ) ) );
   LinkRGB_ToolButton.SetScaledFixedSize( 20, 20 );
   LinkRGB_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   LinkRGB_ToolButton.SetToolTip( "Link RGB Channels" );
   LinkRGB_ToolButton.SetCheckable();
   LinkRGB_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ModeButtonClick, w );

   Zoom11_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/view-zoom-1-1.png" ) ) );
   Zoom11_ToolButton.SetScaledFixedSize( 20, 20 );
   Zoom11_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Zoom11_ToolButton.SetToolTip( "Zoom 1:1" );
   Zoom11_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ModeButtonClick, w );

   Auto_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/burn.png" ) ) );
   Auto_ToolButton.SetScaledFixedSize( 20, 20 );
   Auto_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Auto_ToolButton.SetToolTip( "<p>Auto Stretch</p>"
                               "<p>"
#ifdef __PCL_MACOSX
                               "Cmd"
#else
                               "Ctrl"
#endif
                               "+Click to open the STF AutoStretch Parameters dialog.</p>"
                               "<p>Shift+Click to apply boosted AutoStretch functions.</p>" );
   Auto_ToolButton.OnMouseRelease( (Control::mouse_button_event_handler)&ScreenTransferFunctionInterface::__STFAutoStretch_MouseRelease, w );

   Edit_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/wrench.png" ) ) );
   Edit_ToolButton.SetScaledFixedSize( 20, 20 );
   Edit_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Edit_ToolButton.SetToolTip( "Edit STF Parameters" );
   Edit_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ModeButtonClick, w );

   EditMode_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select.png" ) ) );
   EditMode_ToolButton.SetScaledFixedSize( 20, 20 );
   EditMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   EditMode_ToolButton.SetToolTip( "<p>Edit STF Mode</p>"
                                   "<p>Hold Shift pressed while dragging to block image update events.</p>");
   EditMode_ToolButton.SetCheckable();
   EditMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ModeButtonClick, w );

   ZoomInMode_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/view-zoom-in.png" ) ) );
   ZoomInMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomInMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomInMode_ToolButton.SetToolTip( "Zoom In Mode" );
   ZoomInMode_ToolButton.SetCheckable();
   ZoomInMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ModeButtonClick, w );

   ZoomOutMode_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/view-zoom-out.png" ) ) );
   ZoomOutMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomOutMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomOutMode_ToolButton.SetToolTip( "Zoom Out Mode" );
   ZoomOutMode_ToolButton.SetCheckable();
   ZoomOutMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ModeButtonClick, w );

   PanMode_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-mode-pan-horz.png" ) ) );
   PanMode_ToolButton.SetScaledFixedSize( 20, 20 );
   PanMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PanMode_ToolButton.SetToolTip( "Scroll Mode" );
   PanMode_ToolButton.SetCheckable();
   PanMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ModeButtonClick, w );

   ReadoutShadows_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-mode-readout-shadows.png" ) ) );
   ReadoutShadows_ToolButton.SetScaledFixedSize( 20, 20 );
   ReadoutShadows_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ReadoutShadows_ToolButton.SetToolTip( "Black Point Readout" );
   ReadoutShadows_ToolButton.SetCheckable();
   ReadoutShadows_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ReadoutButtonClick, w );

   ReadoutMidtones_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-mode-readout-midtones.png" ) ) );
   ReadoutMidtones_ToolButton.SetScaledFixedSize( 20, 20 );
   ReadoutMidtones_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ReadoutMidtones_ToolButton.SetToolTip( "Midtones Readout" );
   ReadoutMidtones_ToolButton.SetCheckable();
   ReadoutMidtones_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ReadoutButtonClick, w );

   ReadoutHighlights_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-mode-readout-highlights.png" ) ) );
   ReadoutHighlights_ToolButton.SetScaledFixedSize( 20, 20 );
   ReadoutHighlights_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ReadoutHighlights_ToolButton.SetToolTip( "White Point Readout" );
   ReadoutHighlights_ToolButton.SetCheckable();
   ReadoutHighlights_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ReadoutButtonClick, w );

   STFEnabled_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-stf.png" ) ) );
   STFEnabled_ToolButton.SetScaledFixedSize( 20, 20 );
   STFEnabled_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   STFEnabled_ToolButton.SetCheckable();
   STFEnabled_ToolButton.OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__STFEnabledButtonClick, w );
   // STFEnabled_ToolButton: tool tip and state will be set by parent

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 2 );

   for ( int c = 0; c < 4; ++c )
   {
      ChannelId_Label[c].SetText( chLabel[c] );
      ChannelId_Label[c].SetFixedWidth( labelWidth );
      ChannelId_Label[c].SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      Sliders_Control[c].SetChannel( c, true );
      Sliders_Control[c].SetScaledMinWidth( 400 );
      Sliders_Control[c].SetFocusStyle( FocusStyle::NoFocus );
      Sliders_Control[c].OnValueUpdated( (STFSliders::value_event_handler)&ScreenTransferFunctionInterface::__SliderValueUpdated, w );
      Sliders_Control[c].OnRangeUpdated( (STFSliders::range_event_handler)&ScreenTransferFunctionInterface::__SliderRangeUpdated, w );
      Sliders_Control[c].OnViewDrag( (Control::view_drag_event_handler)&ScreenTransferFunctionInterface::__ViewDrag, w );
      Sliders_Control[c].OnViewDrop( (Control::view_drop_event_handler)&ScreenTransferFunctionInterface::__ViewDrop, w );

      Reset_ToolButton[c].SetIcon( Bitmap( w.ScaledResource( ":/icons/clear.png" ) ) );
      Reset_ToolButton[c].SetScaledFixedSize( 20, 20 );
      Reset_ToolButton[c].SetFocusStyle( FocusStyle::NoFocus );
      Reset_ToolButton[c].SetToolTip( String( "Reset " ) + chToolTip[c] + " Parameters" );
      Reset_ToolButton[c].OnClick( (ToolButton::click_event_handler)&ScreenTransferFunctionInterface::__ResetButtonClick, w );

      STF_Sizer[c].SetSpacing( 4 );

      switch ( c )
      {
      case 0:
         STF_Sizer[c].Add( LinkRGB_ToolButton );
         STF_Sizer[c].Add( EditMode_ToolButton );
         break;
      case 1:
         STF_Sizer[c].Add( Zoom11_ToolButton );
         STF_Sizer[c].Add( ZoomInMode_ToolButton );
         break;
      case 2:
         STF_Sizer[c].Add( Auto_ToolButton );
         STF_Sizer[c].Add( ZoomOutMode_ToolButton );
         break;
      case 3:
         STF_Sizer[c].Add( Edit_ToolButton );
         STF_Sizer[c].Add( PanMode_ToolButton );
         break;
      }

      STF_Sizer[c].Add( ChannelId_Label[c] );
      STF_Sizer[c].Add( Sliders_Control[c], 100 );
      STF_Sizer[c].Add( Reset_ToolButton[c] );

      switch ( c )
      {
      case 0: STF_Sizer[c].Add( ReadoutShadows_ToolButton ); break;
      case 1: STF_Sizer[c].Add( ReadoutMidtones_ToolButton ); break;
      case 2: STF_Sizer[c].Add( ReadoutHighlights_ToolButton ); break;
      case 3: STF_Sizer[c].Add( STFEnabled_ToolButton ); break;
      }

      Global_Sizer.Add( STF_Sizer[c] );
   }

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetMinWidth();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionInterface.cpp - Released 2018-11-23T18:45:58Z
