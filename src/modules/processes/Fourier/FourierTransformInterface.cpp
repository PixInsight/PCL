//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 01.00.04.0191
// ----------------------------------------------------------------------------
// FourierTransformInterface.cpp - Released 2016/02/21 20:22:42 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "FourierTransformInterface.h"
#include "FourierTransformProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FourierTransformInterface* TheFourierTransformInterface = 0;

// ----------------------------------------------------------------------------

#include "FourierTransformIcon.xpm"

// ----------------------------------------------------------------------------

FourierTransformInterface::FourierTransformInterface() :
ProcessInterface(), instance( TheFourierTransformProcess ), GUI( 0 )
{
   TheFourierTransformInterface = this;
}

FourierTransformInterface::~FourierTransformInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString FourierTransformInterface::Id() const
{
   return "FourierTransform";
}

MetaProcess* FourierTransformInterface::Process() const
{
   return TheFourierTransformProcess;
}

const char** FourierTransformInterface::IconImageXPM() const
{
   return FourierTransformIcon_XPM;
}

void FourierTransformInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void FourierTransformInterface::ResetInstance()
{
   FourierTransformInstance defaultInstance( TheFourierTransformProcess );
   ImportProcess( defaultInstance );
}

bool FourierTransformInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FourierTransform" );
      ImportProcess( instance );
   }

   dynamic = false;
   return &P == TheFourierTransformProcess;
}

ProcessImplementation* FourierTransformInterface::NewProcess() const
{
   return new FourierTransformInstance( instance );
}

bool FourierTransformInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const FourierTransformInstance* r = dynamic_cast<const FourierTransformInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a FourierTransform instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool FourierTransformInterface::RequiresInstanceValidation() const
{
   return true;
}

bool FourierTransformInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FourierTransformInterface::UpdateControls()
{
   GUI->Shift_CheckBox.SetChecked( instance.centered );
   GUI->Radial_CheckBox.SetChecked( instance.radialCoordinates );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FourierTransformInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->Shift_CheckBox )
      instance.centered = checked;
   else if ( sender == GUI->Radial_CheckBox )
      instance.radialCoordinates = checked;
   UpdateControls();
}

// ----------------------------------------------------------------------------

FourierTransformInterface::GUIData::GUIData( FourierTransformInterface& w )
{
   Shift_CheckBox.SetText( "Centered" );
   Shift_CheckBox.SetToolTip(
      "<p>When checked, the origin of the DFT (the DC component) is centered in the transform matrix.</p>"
      "<p>When unchecked, the origin is at the top left corner of the transform." );
   Shift_CheckBox.OnClick( (Button::click_event_handler)&FourierTransformInterface::__Click, w );

   Radial_CheckBox.SetText( "Power Spectrum" );
   Radial_CheckBox.SetToolTip(
      "<p>When checked, magnitude and phase components are computed instead of the real and imaginary components.</p>" );
   Radial_CheckBox.OnClick( (Button::click_event_handler)&FourierTransformInterface::__Click, w );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Shift_CheckBox );
   Global_Sizer.Add( Radial_CheckBox );

   w.SetSizer( Global_Sizer );
   w.SetScaledMinWidth( 250 );
   w.SetFixedSize();
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FourierTransformInterface.cpp - Released 2016/02/21 20:22:42 UTC
