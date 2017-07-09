//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0319
// ----------------------------------------------------------------------------
// SuperbiasInterface.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "SuperbiasInterface.h"
#include "SuperbiasParameters.h"
#include "SuperbiasProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SuperbiasInterface* TheSuperbiasInterface = nullptr;

// ----------------------------------------------------------------------------

#include "SuperbiasIcon.xpm"

// ----------------------------------------------------------------------------

SuperbiasInterface::SuperbiasInterface() :
   m_instance( TheSuperbiasProcess )
{
   TheSuperbiasInterface = this;
}

// ----------------------------------------------------------------------------

SuperbiasInterface::~SuperbiasInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SuperbiasInterface::Id() const
{
   return "Superbias";
}

// ----------------------------------------------------------------------------

MetaProcess* SuperbiasInterface::Process() const
{
   return TheSuperbiasProcess;
}

// ----------------------------------------------------------------------------

const char** SuperbiasInterface::IconImageXPM() const
{
   return SuperbiasIcon_XPM;
}

// ----------------------------------------------------------------------------

void SuperbiasInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void SuperbiasInterface::ResetInstance()
{
   SuperbiasInstance defaultInstance( TheSuperbiasProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool SuperbiasInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned&/*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Superbias" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheSuperbiasProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SuperbiasInterface::NewProcess() const
{
   return new SuperbiasInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool SuperbiasInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const SuperbiasInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Superbias instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool SuperbiasInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool SuperbiasInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void SuperbiasInterface::UpdateControls()
{
   GUI->Orientation_ComboBox.SetCurrentItem( m_instance.p_rows ? (m_instance.p_columns ? 2 : 1) : 0 );
   GUI->MultiscaleLayers_SpinBox.SetValue( m_instance.p_multiscaleLayers );
   GUI->MedianTransform_CheckBox.SetChecked( m_instance.p_medianTransform );
   GUI->ExcludeLargeScale_CheckBox.SetChecked( m_instance.p_excludeLargeScale );
}

// ----------------------------------------------------------------------------

void SuperbiasInterface::__ComboBoxItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Orientation_ComboBox )
      switch ( itemIndex )
      {
      default:
      case 0:
         m_instance.p_columns = true;
         m_instance.p_rows = false;
         break;
      case 1:
         m_instance.p_columns = false;
         m_instance.p_rows = true;
         break;
      case 2:
         m_instance.p_columns = true;
         m_instance.p_rows = true;
         break;
      }
}

// ----------------------------------------------------------------------------

void SuperbiasInterface::__SpinBoxValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->MultiscaleLayers_SpinBox )
      m_instance.p_multiscaleLayers = value;
}

// ----------------------------------------------------------------------------

void SuperbiasInterface::__ButtonClicked( Button& sender, bool checked )
{
   if ( sender == GUI->MedianTransform_CheckBox )
      m_instance.p_medianTransform = checked;
   else if ( sender == GUI->ExcludeLargeScale_CheckBox )
      m_instance.p_excludeLargeScale = checked;
}

// ----------------------------------------------------------------------------

SuperbiasInterface::GUIData::GUIData( SuperbiasInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Multiscale layers:" ) + 'M' );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   const char* orientationToolTip =
      "<p>Superbias works by isolating pixel-level structures oriented either vertically (columns) or horizontally "
      "(rows). For normal superbias frame generation, the columns orientation should be used.</p>";

   Orientation_Label.SetText( "Orientation:" );
   Orientation_Label.SetFixedWidth( labelWidth1 );
   Orientation_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Orientation_Label.SetToolTip( orientationToolTip );

   Orientation_ComboBox.AddItem( "Columns" );
   Orientation_ComboBox.AddItem( "Rows" );
   Orientation_ComboBox.AddItem( "Columns and rows" );
   Orientation_ComboBox.SetToolTip( orientationToolTip );
   Orientation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SuperbiasInterface::__ComboBoxItemSelected, w );

   Orientation_Sizer.SetSpacing( 4 );
   Orientation_Sizer.Add( Orientation_Label );
   Orientation_Sizer.Add( Orientation_ComboBox );
   Orientation_Sizer.AddStretch();

   //

   const char* multiscaleLayersToolTip =
      "<p>Number of layers for multiscale analysis, with either the multiscale median transform or the starlet transform. "
      "A multiscale decomposition, following a dyadic scaling scheme, is used to isolate oriented and large-scale "
      "structures in the target image. For normal superbias frame generation, a high number of layers should be used. The "
      "default value is 7 (scale of 128 pixels), and the maximum allowed is 10 (1024 pixels).</p>";

   MultiscaleLayers_Label.SetText( "Multiscale layers:" );
   MultiscaleLayers_Label.SetFixedWidth( labelWidth1 );
   MultiscaleLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MultiscaleLayers_Label.SetToolTip( multiscaleLayersToolTip );

   MultiscaleLayers_SpinBox.SetRange( int( TheSBMultiscaleLayersParameter->MinimumValue() ), int( TheSBMultiscaleLayersParameter->MaximumValue() ) );
   MultiscaleLayers_SpinBox.SetToolTip( multiscaleLayersToolTip );
   MultiscaleLayers_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SuperbiasInterface::__SpinBoxValueUpdated, w );

   MultiscaleLayers_Sizer.SetSpacing( 4 );
   MultiscaleLayers_Sizer.Add( MultiscaleLayers_Label );
   MultiscaleLayers_Sizer.Add( MultiscaleLayers_SpinBox );
   MultiscaleLayers_Sizer.AddStretch();

   //

   const char* medianTransformToolTip =
      "<p>Use the multiscale median transform (MMT) instead of the starlet transform (AKA <i>&agrave; trous</i> transform) "
      "to isolate large-scale structures. The MMT can separate structures much better than linear transforms such as "
      "starlet, so it usually can reproduce better large-scale gradients present in most master bias frames. The MMT, "
      "despite being computationally intensive, is the default option.</p>";

   MedianTransform_CheckBox.SetText( "Median transform" );
   MedianTransform_CheckBox.SetToolTip( medianTransformToolTip );
   MedianTransform_CheckBox.OnClick( (pcl::Button::click_event_handler)&SuperbiasInterface::__ButtonClicked, w );

   MedianTransform_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   MedianTransform_Sizer.Add( MedianTransform_CheckBox );
   MedianTransform_Sizer.AddStretch();

   //

   const char* excludeLargeScaleToolTip =
      "<p>Remove large-scale multiscale layers from the working images used internally to compute column (or row) "
      "mean values. This improves isolation of purely oriented structures (either vertically or horizontally).</p>";

   ExcludeLargeScale_CheckBox.SetText( "Exclude large-scale structures" );
   ExcludeLargeScale_CheckBox.SetToolTip( excludeLargeScaleToolTip );
   ExcludeLargeScale_CheckBox.OnClick( (pcl::Button::click_event_handler)&SuperbiasInterface::__ButtonClicked, w );

   ExcludeLargeScale_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ExcludeLargeScale_Sizer.Add( ExcludeLargeScale_CheckBox );
   ExcludeLargeScale_Sizer.AddStretch();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Orientation_Sizer );
   Global_Sizer.Add( MultiscaleLayers_Sizer );
   Global_Sizer.Add( MedianTransform_Sizer );
   Global_Sizer.Add( ExcludeLargeScale_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SuperbiasInterface.cpp - Released 2017-07-09T18:07:33Z
