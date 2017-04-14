//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard CFA2RGB Process Module Version 01.01.01.0010
// ----------------------------------------------------------------------------
// CFA2RGBInterface.cpp - Released 2016/02/03 00:00:00 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard CFA2RGB PixInsight module.
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

#include "CFA2RGBInterface.h"
#include "CFA2RGBProcess.h"
#include "CFA2RGBParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CFA2RGBInterface* TheCFA2RGBInterface = 0;

// ----------------------------------------------------------------------------

CFA2RGBInterface::CFA2RGBInterface() :
ProcessInterface(), instance( TheCFA2RGBProcess ), GUI( 0 )
{
   TheCFA2RGBInterface = this;
}

CFA2RGBInterface::~CFA2RGBInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString CFA2RGBInterface::Id() const
{
   return "CFA2RGB";
}

MetaProcess* CFA2RGBInterface::Process() const
{
   return TheCFA2RGBProcess;
}

void CFA2RGBInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void CFA2RGBInterface::ResetInstance()
{
   CFA2RGBInstance defaultInstance( TheCFA2RGBProcess );
   ImportProcess( defaultInstance );
}

bool CFA2RGBInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "CFA->RGB" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheCFA2RGBProcess;
}

ProcessImplementation* CFA2RGBInterface::NewProcess() const
{
   return new CFA2RGBInstance( instance );
}

bool CFA2RGBInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const CFA2RGBInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a CFA2RGB instance.";
   return false;
}

bool CFA2RGBInterface::RequiresInstanceValidation() const
{
   return true;
}

bool CFA2RGBInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void CFA2RGBInterface::UpdateControls()
{
   GUI->BayerPatternCombo.SetCurrentItem( instance.p_bayerPattern );
}

// ----------------------------------------------------------------------------

void CFA2RGBInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->BayerPatternCombo )
      instance.p_bayerPattern = itemIndex;
}

// ----------------------------------------------------------------------------

CFA2RGBInterface::GUIData::GUIData( CFA2RGBInterface& w )
{
   PatternLabel.SetText( "Bayer pattern:" );
   PatternLabel.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PatternLabel.AdjustToContents();

   BayerPatternCombo.AddItem( "RGGB" );
   BayerPatternCombo.AddItem( "BGGR" );
   BayerPatternCombo.AddItem( "GBRG" );
   BayerPatternCombo.AddItem( "GRBG" );
   BayerPatternCombo.AdjustToContents();
   BayerPatternCombo.OnItemSelected( (ComboBox::item_event_handler)&CFA2RGBInterface::__ItemSelected, w );

   PatternSizer.SetSpacing( 4 );
   PatternSizer.Add( PatternLabel );
   PatternSizer.Add( BayerPatternCombo );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( PatternSizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CFA2RGBInterface.cpp - Released 2016/02/03 00:00:00 UTC
