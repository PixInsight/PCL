//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0274
// ----------------------------------------------------------------------------
// FastRotationInterface.cpp - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "FastRotationInterface.h"
#include "FastRotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FastRotationInterface* TheFastRotationInterface = 0;

// ----------------------------------------------------------------------------

#include "FastRotationIcon.xpm"

// ----------------------------------------------------------------------------

FastRotationInterface::FastRotationInterface() :
ProcessInterface(), instance( TheFastRotationProcess ), GUI( 0 )
{
   TheFastRotationInterface = this;
}

// ----------------------------------------------------------------------------

FastRotationInterface::~FastRotationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString FastRotationInterface::Id() const
{
   return "FastRotation";
}

// ----------------------------------------------------------------------------

MetaProcess* FastRotationInterface::Process() const
{
   return TheFastRotationProcess;
}

// ----------------------------------------------------------------------------

const char** FastRotationInterface::IconImageXPM() const
{
   return FastRotationIcon_XPM;
}

// ----------------------------------------------------------------------------

void FastRotationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

void FastRotationInterface::ResetInstance()
{
   FastRotationInstance defaultInstance( TheFastRotationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FastRotation" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheFastRotationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FastRotationInterface::NewProcess() const
{
   return new FastRotationInstance( instance );
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const FastRotationInstance* r = dynamic_cast<const FastRotationInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a FastRotation instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void FastRotationInterface::UpdateControls()
{
   switch ( instance.p_mode )
   {
   default:
   case FastRotationMode::Rotate180:
      GUI->Rotate180_RadioButton.Check();
      break;
   case FastRotationMode::Rotate90CW:
      GUI->Rotate90CW_RadioButton.Check();
      break;
   case FastRotationMode::Rotate90CCW:
      GUI->Rotate90CCW_RadioButton.Check();
      break;
   case FastRotationMode::HorizontalMirror:
      GUI->HorizontalMirror_RadioButton.Check();
      break;
   case FastRotationMode::VerticalMirror:
      GUI->VerticalMirror_RadioButton.Check();
      break;
   }
}

// ----------------------------------------------------------------------------

void FastRotationInterface::TransformButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Rotate180_RadioButton )
      instance.p_mode = FastRotationMode::Rotate180;
   else if ( sender == GUI->Rotate90CW_RadioButton )
      instance.p_mode = FastRotationMode::Rotate90CW;
   else if ( sender == GUI->Rotate90CCW_RadioButton )
      instance.p_mode = FastRotationMode::Rotate90CCW;
   else if ( sender == GUI->HorizontalMirror_RadioButton )
      instance.p_mode = FastRotationMode::HorizontalMirror;
   else if ( sender == GUI->VerticalMirror_RadioButton )
      instance.p_mode = FastRotationMode::VerticalMirror;
}

// ----------------------------------------------------------------------------

FastRotationInterface::GUIData::GUIData( FastRotationInterface& w )
{
   Rotate180_RadioButton.SetText( "Rotate 180\xb0" );
   Rotate180_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::TransformButtonClick, w );

   Rotate90CW_RadioButton.SetText( "Rotate 90\xb0 Clockwise" );
   Rotate90CW_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::TransformButtonClick, w );

   Rotate90CCW_RadioButton.SetText( "Rotate 90\xb0 Counter-clockwise" );
   Rotate90CCW_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::TransformButtonClick, w );

   HorizontalMirror_RadioButton.SetText( "Horizontal Mirror" );
   HorizontalMirror_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::TransformButtonClick, w );

   VerticalMirror_RadioButton.SetText( "Vertical Mirror" );
   VerticalMirror_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::TransformButtonClick, w );

   Transform_Sizer.SetMargin( 6 );
   Transform_Sizer.Add( Rotate180_RadioButton );
   Transform_Sizer.Add( Rotate90CW_RadioButton );
   Transform_Sizer.Add( Rotate90CCW_RadioButton );
   Transform_Sizer.Add( HorizontalMirror_RadioButton );
   Transform_Sizer.Add( VerticalMirror_RadioButton );

   Transform_GroupBox.SetTitle( "Transformation" );
   Transform_GroupBox.SetSizer( Transform_Sizer );
   Transform_GroupBox.AdjustToContents();
   Transform_GroupBox.SetMinSize();

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Transform_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FastRotationInterface.cpp - Released 2015/10/08 11:24:39 UTC
