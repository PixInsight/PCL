//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0374
// ----------------------------------------------------------------------------
// RGBWorkingSpaceInterface.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "RGBWorkingSpaceInterface.h"
#include "RGBWorkingSpaceProcess.h"
#include "RGBWorkingSpaceParameters.h"

#include <pcl/ImageWindow.h>
#include <pcl/GlobalSettings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RGBWorkingSpaceInterface* TheRGBWorkingSpaceInterface = nullptr;

// ----------------------------------------------------------------------------

#include "RGBWorkingSpaceIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView  GUI->AllImages_ViewList.CurrentView()

// ----------------------------------------------------------------------------

RGBWorkingSpaceInterface::RGBWorkingSpaceInterface() :
   instance( TheRGBWorkingSpaceProcess )
{
   TheRGBWorkingSpaceInterface = this;
}

RGBWorkingSpaceInterface::~RGBWorkingSpaceInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString RGBWorkingSpaceInterface::Id() const
{
   return "RGBWorkingSpace";
}

MetaProcess* RGBWorkingSpaceInterface::Process() const
{
   return TheRGBWorkingSpaceProcess;
}

const char** RGBWorkingSpaceInterface::IconImageXPM() const
{
   return RGBWorkingSpaceIcon_XPM;
}

InterfaceFeatures RGBWorkingSpaceInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton | InterfaceFeature::TrackViewButton;
}

void RGBWorkingSpaceInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void RGBWorkingSpaceInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         instance.applyGlobalRGBWS = false;
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.MainView() );
         else
            UpdateControls();
      }
}

void RGBWorkingSpaceInterface::ResetInstance()
{
   RGBWorkingSpaceInstance defaultInstance( TheRGBWorkingSpaceProcess );
   ImportProcess( defaultInstance );
}

bool RGBWorkingSpaceInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "RGBWorkingSpace" );
      GUI->AllImages_ViewList.Regenerate( true, false ); // exclude previews
      UpdateControls();
   }

   dynamic = false;
   return &P == TheRGBWorkingSpaceProcess;
}

ProcessImplementation* RGBWorkingSpaceInterface::NewProcess() const
{
   return new RGBWorkingSpaceInstance( instance );
}

bool RGBWorkingSpaceInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const RGBWorkingSpaceInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a RGBWorkingSpace instance.";
   return false;
}

bool RGBWorkingSpaceInterface::RequiresInstanceValidation() const
{
   return true;
}

bool RGBWorkingSpaceInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );
   UpdateControls();
   return true;
}

bool RGBWorkingSpaceInterface::WantsImageNotifications() const
{
   return true;
}

void RGBWorkingSpaceInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
      {
         RGBColorSystem rgbws;
         currentView.Window().GetRGBWS( rgbws );

         instance = RGBWorkingSpaceInstance( TheRGBWorkingSpaceProcess, rgbws );

         UpdateControls();
      }
}

void RGBWorkingSpaceInterface::ImageFocused( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( !v.IsNull() )
         {
            View mainView = v.Window().MainView();

            GUI->AllImages_ViewList.SelectView( mainView ); // normally not necessary, but we can invoke this f() directly

            if ( !currentView.IsNull() )
            {
               RGBColorSystem rgbws;
               currentView.Window().GetRGBWS( rgbws );

               instance = RGBWorkingSpaceInstance( TheRGBWorkingSpaceProcess, rgbws );

               UpdateControls();
            }
         }
}

void RGBWorkingSpaceInterface::ImageRGBWSUpdated( const View& v )
{
   ImageUpdated( v );
}

bool RGBWorkingSpaceInterface::WantsGlobalNotifications() const
{
   return true;
}

void RGBWorkingSpaceInterface::GlobalRGBWSUpdated()
{
   if ( GUI != nullptr )
      if ( instance.AppliesGlobalRGBWS() )
      {
         RGBColorSystem rgbws;
         ImageWindow::GetGlobalRGBWS( rgbws );

         instance = RGBWorkingSpaceInstance( TheRGBWorkingSpaceProcess, rgbws );

         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

void RGBWorkingSpaceInterface::UpdateControls()
{
   bool appliesGlobal = instance.AppliesGlobalRGBWS();

   GUI->RY_NumericControl.SetValue( instance.LuminanceCoefficients()[0] );
   GUI->RY_NumericControl.Disable( appliesGlobal );

   GUI->GY_NumericControl.SetValue( instance.LuminanceCoefficients()[1] );
   GUI->GY_NumericControl.Disable( appliesGlobal );

   GUI->BY_NumericControl.SetValue( instance.LuminanceCoefficients()[2] );
   GUI->BY_NumericControl.Disable( appliesGlobal );

   GUI->NormalizeY_PushButton.Disable( appliesGlobal );

   GUI->Rx_NumericEdit.SetValue( instance.ChromaticityXCoordinates()[0] );
   GUI->Rx_NumericEdit.Disable( appliesGlobal );

   GUI->Gx_NumericEdit.SetValue( instance.ChromaticityXCoordinates()[1] );
   GUI->Gx_NumericEdit.Disable( appliesGlobal );

   GUI->Bx_NumericEdit.SetValue( instance.ChromaticityXCoordinates()[2] );
   GUI->Bx_NumericEdit.Disable( appliesGlobal );

   GUI->Ry_NumericEdit.SetValue( instance.ChromaticityYCoordinates()[0] );
   GUI->Ry_NumericEdit.Disable( appliesGlobal );

   GUI->Gy_NumericEdit.SetValue( instance.ChromaticityYCoordinates()[1] );
   GUI->Gy_NumericEdit.Disable( appliesGlobal );

   GUI->By_NumericEdit.SetValue( instance.ChromaticityYCoordinates()[2] );
   GUI->By_NumericEdit.Disable( appliesGlobal );

   GUI->Gamma_NumericControl.SetValue( instance.Gamma() );
   GUI->Gamma_NumericControl.Disable( instance.IsSRGB() || appliesGlobal );

   GUI->sRGBGamma_CheckBox.SetChecked( instance.IsSRGB() );
   GUI->sRGBGamma_CheckBox.Disable( appliesGlobal );

   GUI->LoadDefaultRGBWS_PushButton.Disable( appliesGlobal );
   GUI->NormalizeY_PushButton.Disable( appliesGlobal );
   GUI->LoadGlobalRGBWS_PushButton.Disable( appliesGlobal );

   GUI->ApplyGlobalRGBWS_CheckBox.SetChecked( appliesGlobal );
}

// ----------------------------------------------------------------------------

void RGBWorkingSpaceInterface::__ViewList_ViewSelected( ViewList& /*sender*/, View& )
{
   DeactivateTrackView();

   if ( !currentView.IsNull() )
   {
      RGBColorSystem rgbws;
      currentView.Window().GetRGBWS( rgbws );
      instance = RGBWorkingSpaceInstance( TheRGBWorkingSpaceProcess, rgbws );
      UpdateControls();
   }
}

void RGBWorkingSpaceInterface::__LuminanceCoefficient_ValueUpdated( NumericEdit& sender, double value )
{
   int i;
   if ( sender == GUI->RY_NumericControl )
      i = 0;
   else if ( sender == GUI->GY_NumericControl )
      i = 1;
   else if ( sender == GUI->BY_NumericControl )
      i = 2;
   else
      return;

   if ( instance.Y[i] != value )
   {
      DeactivateTrackView();
      GUI->AllImages_ViewList.SelectView( View::Null() );
      instance.Y[i] = value;
      UpdateControls();
   }
}

void RGBWorkingSpaceInterface::__ChromaticityCoordinate_ValueUpdated( NumericEdit& sender, double value )
{
   int i;
   bool x;
   if ( sender == GUI->Rx_NumericEdit )
   {
      i = 0;
      x = true;
   }
   else if ( sender == GUI->Gx_NumericEdit )
   {
      i = 1;
      x = true;
   }
   else if ( sender == GUI->Bx_NumericEdit )
   {
      i = 2;
      x = true;
   }
   else if ( sender == GUI->Ry_NumericEdit )
   {
      i = 0;
      x = false;
   }
   else if ( sender == GUI->Gy_NumericEdit )
   {
      i = 1;
      x = false;
   }
   else if ( sender == GUI->By_NumericEdit )
   {
      i = 2;
      x = false;
   }
   else
      return;

   float& item = (x ? instance.x : instance.y)[i];
   if ( item != value )
   {
      DeactivateTrackView();
      GUI->AllImages_ViewList.SelectView( View::Null() );
      item = value;
      UpdateControls();
   }
}

void RGBWorkingSpaceInterface::__Gamma_ValueUpdated( NumericEdit& /*sender*/, double value )
{
   if ( instance.gamma != value )
   {
      DeactivateTrackView();
      GUI->AllImages_ViewList.SelectView( View::Null() );
      instance.gamma = value;
      UpdateControls();
   }
}

void RGBWorkingSpaceInterface::__NormalizeY_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );
   instance.NormalizeLuminanceCoefficients();
   UpdateControls();
}

void RGBWorkingSpaceInterface::__sRGBGamma_ButtonClick( Button& /*sender*/, bool checked )
{
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );
   instance.sRGB = checked;
   if ( instance.sRGB )
      instance.gamma = RGBColorSystem::sRGB.Gamma();
   UpdateControls();
}

void RGBWorkingSpaceInterface::__LoadRGBWS_ButtonClick( Button& sender, bool /*checked*/ )
{
   RGBColorSystem rgbws;
   if ( sender == GUI->LoadDefaultRGBWS_PushButton )
      rgbws = RGBColorSystem::sRGB;
   else if ( sender == GUI->LoadGlobalRGBWS_PushButton )
      ImageWindow::GetGlobalRGBWS( rgbws );
   else
      return;

   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );
   instance = RGBWorkingSpaceInstance( TheRGBWorkingSpaceProcess, rgbws );
   UpdateControls();
}

void RGBWorkingSpaceInterface::__ApplyGlobalRGBWS_ButtonClick( Button& /*sender*/, bool checked )
{
   RGBColorSystem rgbws;
   ImageWindow::GetGlobalRGBWS( rgbws );
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );
   instance = RGBWorkingSpaceInstance( TheRGBWorkingSpaceProcess, rgbws );
   instance.applyGlobalRGBWS = checked;
   UpdateControls();
}

void RGBWorkingSpaceInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllImages_ViewList )
      wantsView = view.IsMainView();
}

void RGBWorkingSpaceInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->AllImages_ViewList )
      if ( view.IsMainView() )
      {
         DeactivateTrackView();
         RGBColorSystem rgbws;
         view.Window().GetRGBWS( rgbws );
         instance = RGBWorkingSpaceInstance( TheRGBWorkingSpaceProcess, rgbws );
         GUI->AllImages_ViewList.SelectView( view );
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

RGBWorkingSpaceInterface::GUIData::GUIData( RGBWorkingSpaceInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Gamma:" ) + 'M' );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&RGBWorkingSpaceInterface::__ViewList_ViewSelected, w );
   AllImages_ViewList.OnViewDrag( (Control::view_drag_event_handler)&RGBWorkingSpaceInterface::__ViewDrag, w );
   AllImages_ViewList.OnViewDrop( (Control::view_drop_event_handler)&RGBWorkingSpaceInterface::__ViewDrop, w );

   //

   LuminanceCoefficients_SectionBar.SetTitle( "Luminance Coefficients (D50)" );
   LuminanceCoefficients_SectionBar.SetSection( LuminanceCoefficients_Control );

   //

   RY_NumericControl.label.SetText( "Red:" );
   RY_NumericControl.label.SetMinWidth( labelWidth1 );
   RY_NumericControl.slider.SetRange( 0, 100 );
   RY_NumericControl.slider.SetScaledMinWidth( 200 );
   RY_NumericControl.SetReal();
   RY_NumericControl.SetRange( TheRGBWSLuminanceCoefficientParameter->MinimumValue(), TheRGBWSLuminanceCoefficientParameter->MaximumValue() );
   RY_NumericControl.SetPrecision( TheRGBWSLuminanceCoefficientParameter->Precision() );
   RY_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__LuminanceCoefficient_ValueUpdated, w );

   GY_NumericControl.label.SetText( "Green:" );
   GY_NumericControl.label.SetMinWidth( labelWidth1 );
   GY_NumericControl.slider.SetRange( 0, 100 );
   GY_NumericControl.slider.SetScaledMinWidth( 200 );
   GY_NumericControl.SetReal();
   GY_NumericControl.SetRange( TheRGBWSLuminanceCoefficientParameter->MinimumValue(), TheRGBWSLuminanceCoefficientParameter->MaximumValue() );
   GY_NumericControl.SetPrecision( TheRGBWSLuminanceCoefficientParameter->Precision() );
   GY_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__LuminanceCoefficient_ValueUpdated, w );

   BY_NumericControl.label.SetText( "Blue:" );
   BY_NumericControl.label.SetMinWidth( labelWidth1 );
   BY_NumericControl.slider.SetRange( 0, 100 );
   BY_NumericControl.slider.SetScaledMinWidth( 200 );
   BY_NumericControl.SetReal();
   BY_NumericControl.SetRange( TheRGBWSLuminanceCoefficientParameter->MinimumValue(), TheRGBWSLuminanceCoefficientParameter->MaximumValue() );
   BY_NumericControl.SetPrecision( TheRGBWSLuminanceCoefficientParameter->Precision() );
   BY_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__LuminanceCoefficient_ValueUpdated, w );

   LuminanceCoefficients_Sizer.SetSpacing( 4 );
   LuminanceCoefficients_Sizer.Add( RY_NumericControl );
   LuminanceCoefficients_Sizer.Add( GY_NumericControl );
   LuminanceCoefficients_Sizer.Add( BY_NumericControl );
#ifdef __PCL_WINDOWS
   LuminanceCoefficients_Sizer.AddSpacing( 2 ); // fix a strange problem on Windows
#endif

   LuminanceCoefficients_Control.SetSizer( LuminanceCoefficients_Sizer );

   //

   ChromaticityCoordinates_SectionBar.SetTitle( "Chromaticity Coordinates (D50)" );
   ChromaticityCoordinates_SectionBar.SetSection( ChromaticityCoordinates_Control );

   //

   RxyTitle_Label.SetText( "Red" );
   RxyTitle_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   GxyTitle_Label.SetText( "Green" );
   GxyTitle_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   BxyTitle_Label.SetText( "Blue" );
   BxyTitle_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   xyTitle_Sizer.SetSpacing( 4 );
   xyTitle_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   xyTitle_Sizer.Add( RxyTitle_Label );
   xyTitle_Sizer.Add( GxyTitle_Label );
   xyTitle_Sizer.Add( BxyTitle_Label );
   xyTitle_Sizer.AddStretch();

   //

   x_Label.SetText( "x:" );
   x_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   x_Label.SetMinWidth( labelWidth1 );

   Rx_NumericEdit.label.Hide();
   Rx_NumericEdit.SetReal();
   Rx_NumericEdit.SetRange( TheRGBWSChromaticityXParameter->MinimumValue(), TheRGBWSChromaticityXParameter->MaximumValue() );
   Rx_NumericEdit.SetPrecision( TheRGBWSChromaticityXParameter->Precision() );
   Rx_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__ChromaticityCoordinate_ValueUpdated, w );

   Gx_NumericEdit.label.Hide();
   Gx_NumericEdit.SetReal();
   Gx_NumericEdit.SetRange( TheRGBWSChromaticityXParameter->MinimumValue(), TheRGBWSChromaticityXParameter->MaximumValue() );
   Gx_NumericEdit.SetPrecision( TheRGBWSChromaticityXParameter->Precision() );
   Gx_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__ChromaticityCoordinate_ValueUpdated, w );

   Bx_NumericEdit.label.Hide();
   Bx_NumericEdit.SetReal();
   Bx_NumericEdit.SetRange( TheRGBWSChromaticityXParameter->MinimumValue(), TheRGBWSChromaticityXParameter->MaximumValue() );
   Bx_NumericEdit.SetPrecision( TheRGBWSChromaticityXParameter->Precision() );
   Bx_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__ChromaticityCoordinate_ValueUpdated, w );

   xValues_Sizer.SetSpacing( 4 );
   xValues_Sizer.Add( x_Label );
   xValues_Sizer.Add( Rx_NumericEdit );
   xValues_Sizer.Add( Gx_NumericEdit );
   xValues_Sizer.Add( Bx_NumericEdit );
   xValues_Sizer.AddStretch();

   //

   y_Label.SetText( "y:" );
   y_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   y_Label.SetMinWidth( labelWidth1 );

   Ry_NumericEdit.label.Hide();
   Ry_NumericEdit.SetReal();
   Ry_NumericEdit.SetRange( TheRGBWSChromaticityYParameter->MinimumValue(), TheRGBWSChromaticityYParameter->MaximumValue() );
   Ry_NumericEdit.SetPrecision( TheRGBWSChromaticityYParameter->Precision() );
   Ry_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__ChromaticityCoordinate_ValueUpdated, w );

   Gy_NumericEdit.label.Hide();
   Gy_NumericEdit.SetReal();
   Gy_NumericEdit.SetRange( TheRGBWSChromaticityYParameter->MinimumValue(), TheRGBWSChromaticityYParameter->MaximumValue() );
   Gy_NumericEdit.SetPrecision( TheRGBWSChromaticityYParameter->Precision() );
   Gy_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__ChromaticityCoordinate_ValueUpdated, w );

   By_NumericEdit.label.Hide();
   By_NumericEdit.SetReal();
   By_NumericEdit.SetRange( TheRGBWSChromaticityYParameter->MinimumValue(), TheRGBWSChromaticityYParameter->MaximumValue() );
   By_NumericEdit.SetPrecision( TheRGBWSChromaticityYParameter->Precision() );
   By_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__ChromaticityCoordinate_ValueUpdated, w );

   yValues_Sizer.SetSpacing( 4 );
   yValues_Sizer.Add( y_Label );
   yValues_Sizer.Add( Ry_NumericEdit );
   yValues_Sizer.Add( Gy_NumericEdit );
   yValues_Sizer.Add( By_NumericEdit );
   yValues_Sizer.AddStretch();

   //

   ChromaticityCoordinates_Sizer.SetSpacing( 4 );
   ChromaticityCoordinates_Sizer.Add( xyTitle_Sizer );
   ChromaticityCoordinates_Sizer.Add( xValues_Sizer );
   ChromaticityCoordinates_Sizer.Add( yValues_Sizer );

   ChromaticityCoordinates_Control.SetSizer( ChromaticityCoordinates_Sizer );

   //

   Gamma_SectionBar.SetTitle( "Gamma" );
   Gamma_SectionBar.SetSection( Gamma_Control );

   //

   Gamma_NumericControl.label.SetText( "Gamma:" );
   Gamma_NumericControl.label.SetMinWidth( labelWidth1 );
   Gamma_NumericControl.slider.SetRange( 1, 100 );
   Gamma_NumericControl.slider.SetScaledMinWidth( 200 );
   Gamma_NumericControl.SetReal();
   Gamma_NumericControl.SetRange( TheRGBWSGammaParameter->MinimumValue(), TheRGBWSGammaParameter->MaximumValue() );
   Gamma_NumericControl.SetPrecision( TheRGBWSGammaParameter->Precision() );
   Gamma_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RGBWorkingSpaceInterface::__Gamma_ValueUpdated, w );

   sRGBGamma_CheckBox.SetText( "Use sRGB Gamma Function" );
   sRGBGamma_CheckBox.OnClick( (Button::click_event_handler)&RGBWorkingSpaceInterface::__sRGBGamma_ButtonClick, w );

   sRGBGamma_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   sRGBGamma_Sizer.Add( sRGBGamma_CheckBox );

   Gamma_Sizer.SetSpacing( 4 );
   Gamma_Sizer.Add( Gamma_NumericControl );
   Gamma_Sizer.Add( sRGBGamma_Sizer );

   Gamma_Control.SetSizer( Gamma_Sizer );

   //

   BottomSectionSeparator_Control.SetObjectId( "IWSectionBar" );
   BottomSectionSeparator_Control.SetScaledMinHeight( 4 );

   //

   LoadDefaultRGBWS_PushButton.SetText( "Load Default RGBWS" );
   LoadDefaultRGBWS_PushButton.OnClick( (Button::click_event_handler)&RGBWorkingSpaceInterface::__LoadRGBWS_ButtonClick, w );

   NormalizeY_PushButton.SetText( "Normalize Luminance Coefficients" );
   NormalizeY_PushButton.OnClick( (Button::click_event_handler)&RGBWorkingSpaceInterface::__NormalizeY_ButtonClick, w );

   LoadDefaultRGBWS_Sizer.Add( LoadDefaultRGBWS_PushButton );
   LoadDefaultRGBWS_Sizer.AddSpacing( 8 );
   LoadDefaultRGBWS_Sizer.Add( NormalizeY_PushButton );
   LoadDefaultRGBWS_Sizer.AddStretch();

   //

   LoadGlobalRGBWS_PushButton.SetText( "Load Global RGBWS" );
   LoadGlobalRGBWS_PushButton.OnClick( (Button::click_event_handler)&RGBWorkingSpaceInterface::__LoadRGBWS_ButtonClick, w );

   ApplyGlobalRGBWS_CheckBox.SetText( "Apply Global RGBWS" );
   ApplyGlobalRGBWS_CheckBox.OnClick( (Button::click_event_handler)&RGBWorkingSpaceInterface::__ApplyGlobalRGBWS_ButtonClick, w );

   LoadGlobalRGBWS_Sizer.Add( LoadGlobalRGBWS_PushButton );
   LoadGlobalRGBWS_Sizer.AddSpacing( 8 );
   LoadGlobalRGBWS_Sizer.Add( ApplyGlobalRGBWS_CheckBox );
   LoadGlobalRGBWS_Sizer.AddStretch();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( AllImages_ViewList );
   Global_Sizer.Add( LuminanceCoefficients_SectionBar );
   Global_Sizer.Add( LuminanceCoefficients_Control );
   Global_Sizer.Add( ChromaticityCoordinates_SectionBar );
   Global_Sizer.Add( ChromaticityCoordinates_Control );
   Global_Sizer.Add( Gamma_SectionBar );
   Global_Sizer.Add( Gamma_Control );
   Global_Sizer.Add( BottomSectionSeparator_Control );
   Global_Sizer.Add( LoadDefaultRGBWS_Sizer );
   Global_Sizer.Add( LoadGlobalRGBWS_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();

   RxyTitle_Label.SetFixedWidth( Rx_NumericEdit.edit.MinWidth() );
   GxyTitle_Label.SetFixedWidth( Gx_NumericEdit.edit.MinWidth() );
   BxyTitle_Label.SetFixedWidth( Bx_NumericEdit.edit.MinWidth() );

   Gamma_NumericControl.edit.SetFixedWidth( RY_NumericControl.edit.MinWidth() );

   ChromaticityCoordinates_Control.Hide();
   Gamma_Control.Hide();

   int btnWidth = fnt.Width( "Apply Global RGBWS" ) + 32;
   LoadDefaultRGBWS_PushButton.SetFixedWidth( btnWidth );
   LoadGlobalRGBWS_PushButton.SetFixedWidth( btnWidth );

   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceInterface.cpp - Released 2019-01-21T12:06:41Z
