//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0405
// ----------------------------------------------------------------------------
// RescaleInterface.cpp - Released 2017-08-01T14:26:58Z
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

#include "RescaleInterface.h"
#include "RescaleProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RescaleInterface* TheRescaleInterface = nullptr;

// ----------------------------------------------------------------------------

#include "RescaleIcon.xpm"

// ----------------------------------------------------------------------------

RescaleInterface::RescaleInterface() :
   instance( TheRescaleProcess )
{
   TheRescaleInterface = this;
}

// ----------------------------------------------------------------------------

RescaleInterface::~RescaleInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString RescaleInterface::Id() const
{
   return "Rescale";
}

// ----------------------------------------------------------------------------

MetaProcess* RescaleInterface::Process() const
{
   return TheRescaleProcess;
}

// ----------------------------------------------------------------------------

const char** RescaleInterface::IconImageXPM() const
{
   return RescaleIcon_XPM;
}

// ----------------------------------------------------------------------------

void RescaleInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void RescaleInterface::ResetInstance()
{
   RescaleInstance defaultInstance( TheRescaleProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool RescaleInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Rescale" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheRescaleProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* RescaleInterface::NewProcess() const
{
   return new RescaleInstance( instance );
}

// ----------------------------------------------------------------------------

bool RescaleInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const RescaleInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Rescale instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool RescaleInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RescaleInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void RescaleInterface::UpdateControls()
{
   GUI->RGBK_RadioButton.SetChecked( instance.mode == RescalingMode::RGBK );
   GUI->RGBK_Individual_RadioButton.SetChecked( instance.mode == RescalingMode::RGBK_Individual );
   GUI->CIEL_RadioButton.SetChecked( instance.mode == RescalingMode::CIEL );
   GUI->CIEY_RadioButton.SetChecked( instance.mode == RescalingMode::CIEY );
}

// ----------------------------------------------------------------------------

void RescaleInterface::RescaleButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->RGBK_RadioButton )
      instance.mode = RescalingMode::RGBK;
   else if ( sender == GUI->RGBK_Individual_RadioButton )
      instance.mode = RescalingMode::RGBK_Individual;
   else if ( sender == GUI->CIEL_RadioButton )
      instance.mode = RescalingMode::CIEL;
   else if ( sender == GUI->CIEY_RadioButton )
      instance.mode = RescalingMode::CIEY;
}

// ----------------------------------------------------------------------------

RescaleInterface::GUIData::GUIData( RescaleInterface& w )
{
   RGBK_RadioButton.SetText( "RGB/K" );
   RGBK_RadioButton.SetToolTip( "<p>Apply a rescaling operation in the RGB/gray color space.</p>"
      "<p>In this mode the rescaling range is taken as the existing minimum and maximum values "
      "among all nominal channels in the target image, which preserves color balance.</p>" );
   RGBK_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   RGBK_Individual_RadioButton.SetText( "RGB/K, individual channels" );
   RGBK_Individual_RadioButton.SetToolTip( "<p>Rescale each nominal channel separately.</p>"
      "<p>In this mode a different rescaling range is used for each nominal channel, which can modify "
      "color balance.</p>" );
   RGBK_Individual_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   CIEL_RadioButton.SetText( "CIE L* (lightness)" );
   CIEL_RadioButton.SetToolTip( "<p>Rescale the CIE L* component.</p>"
      "<p>The CIE L* component (lightness) is extracted, rescaled, and then reinserted in the target RGB image.</p>"
      "<p><b>Important &mdash</b> If the target image is linear, this rescaling operation will break "
      "linearity, as CIE L* is a nonlinear function of the R, G and B sample values for each pixel.</p>" );
   CIEL_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   CIEY_RadioButton.SetText( "CIE Y (luminance)" );
   CIEY_RadioButton.SetToolTip( "<p>Rescale the CIE Y component.</p>"
      "<p>The CIE Y component (relative luminance) is extracted, rescaled, and then reinserted in the "
      "target RGB image.</p>"
      "<p><b>Important &mdash</b> The CIE Y component is a linear combination of the R, G and B sample "
      "values for each pixel. However, if the target image is linear, a linear RGB working space should "
      "be used (gamma=1) to preserve linearity.</p>" );
   CIEY_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   Rescale_Sizer.SetMargin( 6 );
   Rescale_Sizer.SetSpacing( 4 );
   Rescale_Sizer.Add( RGBK_RadioButton );
   Rescale_Sizer.Add( RGBK_Individual_RadioButton );
   Rescale_Sizer.Add( CIEL_RadioButton );
   Rescale_Sizer.Add( CIEY_RadioButton );

   Rescale_GroupBox.SetTitle( "Rescaling Mode" );
   Rescale_GroupBox.SetSizer( Rescale_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Rescale_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RescaleInterface.cpp - Released 2017-08-01T14:26:58Z
