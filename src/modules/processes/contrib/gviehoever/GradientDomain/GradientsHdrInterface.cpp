//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0215
// ----------------------------------------------------------------------------
// GradientsHdrInterface.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2015. Licensed under LGPL 2.1
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include "GradientsHdrInterface.h"
#include "GradientsHdrProcess.h"
#include "GradientsHdrParameters.h"
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsHdrInterface* TheGradientsHdrInterface = 0;

// ----------------------------------------------------------------------------

// FIXME
//#include "GradientsHdrIcon.xpm"

// ----------------------------------------------------------------------------

GradientsHdrInterface::GradientsHdrInterface() :
ProcessInterface(), instance( TheGradientsHdrProcess ), GUI( 0 )
{
   TheGradientsHdrInterface = this;
}

GradientsHdrInterface::~GradientsHdrInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString GradientsHdrInterface::Id() const
{
   return "GradientHDRCompression";
}

IsoString GradientsHdrInterface::Aliases() const
{
   return "GradientsHdrCompression";
}

MetaProcess* GradientsHdrInterface::Process() const
{
   return TheGradientsHdrProcess;
}

const char** GradientsHdrInterface::IconImageXPM() const
{
   return 0; // GradientsHdrIcon_XPM; FIXME
}

InterfaceFeatures
GradientsHdrInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

bool GradientsHdrInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const
{
   return true;
}

void GradientsHdrInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != 0 )
      if ( active )
      {
	//FIXME newRTData = true;
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      }
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

bool GradientsHdrInterface::GenerateRealTimePreview( UInt16Image& img, const View& view, int zoomLevel, String& info ) const
{
  //std::cout<<"GradientsHdrInterface::GenerateRealTimePreview() called, zoomlevel"<<zoomLevel<<std::endl;

  img.ResetSelections();
  UInt16Image wrk;
  bool ok=true;

  wrk.Assign( img );
  GradientsHdrInstance wrkInstance(instance);
  wrkInstance.ApplyClip16( wrk, zoomLevel);
  // FIXME currently no abort implemented...
  // FIXME could save some runtime here if I new if img was the same image as in
  // previous runs

  if ( !ok )
    return false;

  wrk.ResetSelections();
  wrk.SelectNominalChannels();
  img.Assign( wrk );
  return true;

}


void GradientsHdrInterface::UpdateRealTimePreview()
{
   if ( !GUI->UpdateRealTime_Timer.IsRunning() )
   {
     //FIXME newRTData = true;
      RealTimePreview::Update();
   }
}

void GradientsHdrInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void GradientsHdrInterface::ResetInstance()
{
   GradientsHdrInstance defaultInstance( TheGradientsHdrProcess );
   ImportProcess( defaultInstance );
}

bool GradientsHdrInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "GradientHDRCompression" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheGradientsHdrProcess;
}

ProcessImplementation* GradientsHdrInterface::NewProcess() const
{
   return new GradientsHdrInstance( instance );
}

bool GradientsHdrInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const GradientsHdrInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a GradientHDRCompression instance.";
   return false;
}

bool GradientsHdrInterface::RequiresInstanceValidation() const
{
   return true;
}

bool GradientsHdrInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void GradientsHdrInterface::UpdateControls()
{
   GUI->expGradient_NumericControl.SetValue( instance.expGradient );
   GUI->logMaxGradient_NumericControl.SetValue( instance.logMaxGradient );
   GUI->logMinGradient_NumericControl.SetValue( instance.logMinGradient );
   GUI->logMinGradient_NumericControl.SetValue( instance.logMinGradient );
   GUI->rescale01_CheckBox.SetChecked(instance.bRescale01);
   GUI->preserveColor_CheckBox.SetChecked(instance.bPreserveColor);
}

// ----------------------------------------------------------------------------

void GradientsHdrInterface::__expGradientUpdated( NumericEdit& sender, double value )
{
  if ( sender == GUI->expGradient_NumericControl ){
    instance.expGradient = value;
    if ( IsRealTimePreviewActive() ){
      GUI->UpdateRealTime_Timer.Start();
    }
  }
}

void GradientsHdrInterface::__logMaxGradientUpdated( NumericEdit& sender, double value )
{
  if ( sender == GUI->logMaxGradient_NumericControl ){
    instance.logMaxGradient = value;
    if (instance.logMinGradient>value){
      instance.logMinGradient = value;
      GUI->logMinGradient_NumericControl.SetValue(value);
    }
    if ( IsRealTimePreviewActive() ){
      GUI->UpdateRealTime_Timer.Start();
    }
  }
}

void GradientsHdrInterface::__logMinGradientUpdated( NumericEdit& sender, double value )
{
  if ( sender == GUI->logMinGradient_NumericControl ){
    instance.logMinGradient = value;
    if (instance.logMaxGradient<value){
      instance.logMaxGradient = value;
      GUI->logMaxGradient_NumericControl.SetValue(value+0.1);
    }
    if ( IsRealTimePreviewActive() ){
      GUI->UpdateRealTime_Timer.Start();
    }
  }
}

void GradientsHdrInterface::__rescale01Clicked( Button& sender, bool value )
{
  if ( sender == GUI->rescale01_CheckBox ){
    instance.bRescale01 = value;
    if ( IsRealTimePreviewActive() ){
      GUI->UpdateRealTime_Timer.Start();
    }
  }
}

void GradientsHdrInterface::__preserveColorClicked( Button& sender, bool value )
{
  if ( sender == GUI->preserveColor_CheckBox ){
    instance.bPreserveColor = value;
    if ( IsRealTimePreviewActive() ){
      GUI->UpdateRealTime_Timer.Start();
    }
  }
}


void GradientsHdrInterface::__UpdateRealTime_Timer( Timer& /*sender*/ )
{
   if ( IsRealTimePreviewActive() )
      UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

GradientsHdrInterface::GUIData::GUIData( GradientsHdrInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Max. log10(gradient):" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 7 ) );

   w.SetToolTip("<p>Enhances small gradients in the luminance of an image, thus sometimes revealing "
		"surprising details otherwise hidden in large contrasts. Note: Gradients are in the range of [-1,1]. The method "
		"works on the absolute gradient values (abs(gradient)), later restoring the sign.</p>"
		"<p>(c) 2011 Georg Viehoever, published under LGPL 2.1. "
		"With important contributions in terms of code and test data by Carlos Milovic, Harry Page and Vicent Peris.</p>");

   //

   expGradient_NumericControl.label.SetText( "Exponent gradient:" );
   expGradient_NumericControl.label.SetFixedWidth( labelWidth1 );
   expGradient_NumericControl.slider.SetScaledMinWidth( 250 );
   expGradient_NumericControl.slider.SetRange( 0, 100 );
   expGradient_NumericControl.SetReal();
   expGradient_NumericControl.SetRange( TheGradientsHdrParameterExpGradient->MinimumValue(), TheGradientsHdrParameterExpGradient->MaximumValue() );
   expGradient_NumericControl.SetPrecision( TheGradientsHdrParameterExpGradient->Precision() );
   expGradient_NumericControl.SetToolTip( "<p>Gradients are transformed via sign(gradient)*pow(gradient,value). Lower values increase the effect,"
					  " since the value of small abs(gradients) is moved closer to 1.0. Applied after the other two filters.</p>" );
   expGradient_NumericControl.edit.SetFixedWidth( editWidth1 );
   expGradient_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GradientsHdrInterface::__expGradientUpdated, w );

   //

   logMaxGradient_NumericControl.label.SetText( "Max. log10(gradient):" );
   logMaxGradient_NumericControl.label.SetFixedWidth( labelWidth1 );
   logMaxGradient_NumericControl.slider.SetScaledMinWidth( 250 );
   logMaxGradient_NumericControl.slider.SetRange( 0, 100 );
   logMaxGradient_NumericControl.SetReal();
   logMaxGradient_NumericControl.SetRange( TheGradientsHdrParameterLogMaxGradient->MinimumValue(), TheGradientsHdrParameterLogMaxGradient->MaximumValue() );
   logMaxGradient_NumericControl.SetPrecision( TheGradientsHdrParameterLogMaxGradient->Precision() );
   logMaxGradient_NumericControl.SetToolTip( "<p>Gradients with abs(gradient)>pow(10,value) are clipped to pow(10,value), thus reducing the influence of small gradients. Lower is stronger.</p>" );
   logMaxGradient_NumericControl.edit.SetFixedWidth( editWidth1 );
   logMaxGradient_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GradientsHdrInterface::__logMaxGradientUpdated, w );

   //

   logMinGradient_NumericControl.label.SetText( "Min. log10(gradient):" );
   logMinGradient_NumericControl.label.SetFixedWidth( labelWidth1 );
   logMinGradient_NumericControl.slider.SetScaledMinWidth( 250 );
   logMinGradient_NumericControl.slider.SetRange( 0, 100 );
   logMinGradient_NumericControl.SetReal();
   logMinGradient_NumericControl.SetRange( TheGradientsHdrParameterLogMinGradient->MinimumValue(), TheGradientsHdrParameterLogMinGradient->MaximumValue() );
   logMinGradient_NumericControl.SetPrecision( TheGradientsHdrParameterLogMinGradient->Precision() );
   logMinGradient_NumericControl.SetToolTip( "<p>Gradients with abs(gradient)>pow(10,value) are clipped to zero. This may be useful for noise reduction, but also leads to posterization. Higher is stronger. Smallest value switches off this kind of filtering.</p>" );
   logMinGradient_NumericControl.edit.SetFixedWidth( editWidth1 );
   logMinGradient_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GradientsHdrInterface::__logMinGradientUpdated, w );

   //

   rescale01_Label.SetText( "Rescale to [0,1]:" );
   rescale01_Label.SetMinWidth( labelWidth1 );
   const char *pRescaleHelp="<p>If checked, rescale resulting image to range [0,1]. Otherwise rescale to range of original image.</p>";
   rescale01_Label.SetToolTip( pRescaleHelp );
   rescale01_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   rescale01_CheckBox.SetToolTip( pRescaleHelp );
   rescale01_CheckBox.OnClick( (Button::click_event_handler)&GradientsHdrInterface::__rescale01Clicked, w );

   rescale01_Sizer.SetSpacing( 4 );
   rescale01_Sizer.Add( rescale01_Label );
   rescale01_Sizer.Add( rescale01_CheckBox );
   rescale01_Sizer.AddStretch();

   //

   preserveColor_Label.SetText( "Preserve Color:" );
   preserveColor_Label.SetMinWidth( labelWidth1 );
   preserveColor_Label.SetToolTip( "<p>If checked, the relative amounts of R:G:B are preserved</p>" );
   preserveColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   preserveColor_CheckBox.SetToolTip( "<p><p>If checked, the relative amounts of R:G:B are preserved</p>" );
   preserveColor_CheckBox.OnClick( (Button::click_event_handler)&GradientsHdrInterface::__preserveColorClicked, w );

   rescale01_Sizer.SetSpacing( 4 );
   rescale01_Sizer.Add( rescale01_Label );
   rescale01_Sizer.Add( rescale01_CheckBox );
   rescale01_Sizer.AddStretch();
   rescale01_Sizer.Add( preserveColor_Label );
   rescale01_Sizer.Add( preserveColor_CheckBox );

   //

   global_Sizer.SetMargin( 8 );
   global_Sizer.SetSpacing( 6 );
   global_Sizer.Add( logMaxGradient_NumericControl );
   global_Sizer.Add( logMinGradient_NumericControl );
   global_Sizer.Add( expGradient_NumericControl );
   global_Sizer.Add( rescale01_Sizer );

   w.SetSizer( global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTime_Timer.SetSingleShot();
   UpdateRealTime_Timer.SetInterval( 0.040 );
   UpdateRealTime_Timer.OnTimer( (Timer::timer_event_handler)&GradientsHdrInterface::__UpdateRealTime_Timer, w );

}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsHdrInterface.cpp - Released 2017-08-01T14:26:58Z
