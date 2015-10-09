//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.06.0288
// ----------------------------------------------------------------------------
// ReadoutOptionsInterface.cpp - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "ReadoutOptionsInterface.h"
#include "ReadoutOptionsProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptionsInterface* TheReadoutOptionsInterface = 0;

// ----------------------------------------------------------------------------

#include "ReadoutOptionsIcon.xpm"

// ----------------------------------------------------------------------------

ReadoutOptionsInterface::ReadoutOptionsInterface() :
ProcessInterface(), instance( TheReadoutOptionsProcess ), GUI( 0 )
{
   TheReadoutOptionsInterface = this;
}

// ----------------------------------------------------------------------------

ReadoutOptionsInterface::~ReadoutOptionsInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString ReadoutOptionsInterface::Id() const
{
   return "ReadoutOptions";
}

// ----------------------------------------------------------------------------

MetaProcess* ReadoutOptionsInterface::Process() const
{
   return TheReadoutOptionsProcess;
}

// ----------------------------------------------------------------------------

const char** ReadoutOptionsInterface::IconImageXPM() const
{
   return ReadoutOptionsIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures ReadoutOptionsInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInterface::ApplyInstance() const
{
   instance.LaunchGlobal();
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInterface::ResetInstance()
{
   ReadoutOptionsInstance defaultInstance( TheReadoutOptionsProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ReadoutOptions" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheReadoutOptionsProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ReadoutOptionsInterface::NewProcess() const
{
   return new ReadoutOptionsInstance( instance );
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const ReadoutOptionsInstance* r = dynamic_cast<const ReadoutOptionsInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a ReadoutOptions instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInterface::UpdateControls()
{
   ReadoutOptions o( instance.Options() );

   GUI->Data_ComboBox.SetCurrentItem( o.Data() );

   GUI->Mode_ComboBox.SetCurrentItem( o.Mode() );

   GUI->ProbeSize_ComboBox.SetCurrentItem( o.ProbeSize() >> 1 );

   GUI->ShowAlphaChannel_CheckBox.SetChecked( o.IsAlphaChannelEnabled() );
   GUI->ShowMaskChannel_CheckBox.SetChecked( o.IsMaskChannelEnabled() );

   GUI->ShowPreview_CheckBox.SetChecked( o.IsPreviewEnabled() );
   GUI->PreviewCenter_CheckBox.SetChecked( o.IsPreviewCenterEnabled() );

   GUI->PreviewSize_Label.Enable( o.IsPreviewEnabled() );
   GUI->PreviewSize_SpinBox.Enable( o.IsPreviewEnabled() );
   GUI->PreviewSize_SpinBox.SetValue( o.PreviewSize() );

   GUI->PreviewZoom_Label.Enable( o.IsPreviewEnabled() );
   GUI->PreviewZoom_ComboBox.Enable( o.IsPreviewEnabled() );
   GUI->PreviewZoom_ComboBox.SetCurrentItem( o.PreviewZoomFactor()-1 );

   GUI->Broadcast_CheckBox.SetChecked( o.IsBroadcastEnabled() );

   GUI->Precision_ComboBox.SetCurrentItem( o.Precision() );

   bool isBinary = o.IntegerRange() == 255 ||
                   o.IntegerRange() == 1023 ||
                   o.IntegerRange() == 4095 ||
                   o.IntegerRange() == 16383 ||
                   o.IntegerRange() == 65535 ||
                   o.IntegerRange() == 1048575 ||
                   o.IntegerRange() == 16777215 ||
                   o.IntegerRange() == 4294967295ul;

   if ( isBinary )
   {
      int idx = 0;
      switch ( o.IntegerRange() )
      {
      case 255:          idx = 0; break;
      case 1023:         idx = 1; break;
      case 4095:         idx = 2; break;
      case 16383:        idx = 3; break;
      case 65535:        idx = 4; break;
      case 1048575:      idx = 5; break;
      case 16777215:     idx = 6; break;
      case 4294967295ul: idx = 7; break;
      }

      GUI->BinaryInteger_ComboBox.SetCurrentItem( idx );
   }
   else
      GUI->BinaryInteger_ComboBox.SetCurrentItem( 4 ); // default 16-bit

   GUI->ArbitraryInteger_SpinBox.SetValue(
      (o.IntegerRange() <= size_type( int_max )) ? o.IntegerRange() : 100 );

   if ( o.IsReal() )
   {
      GUI->Real_CheckBox.Check();
      GUI->Precision_Label.Enable();
      GUI->Precision_ComboBox.Enable();

      GUI->BinaryInteger_CheckBox.Uncheck();
      GUI->BinaryInteger_Label.Disable();
      GUI->BinaryInteger_ComboBox.Disable();

      GUI->ArbitraryInteger_CheckBox.Uncheck();
      GUI->ArbitraryInteger_Label.Disable();
      GUI->ArbitraryInteger_SpinBox.Disable();
   }
   else
   {
      GUI->Real_CheckBox.Uncheck();
      GUI->Precision_Label.Disable();
      GUI->Precision_ComboBox.Disable();

      if ( isBinary )
      {
         if ( !GUI->BinaryInteger_CheckBox.IsChecked() &&
              !GUI->ArbitraryInteger_CheckBox.IsChecked() )
         {
            GUI->BinaryInteger_CheckBox.Check();
            GUI->BinaryInteger_Label.Enable();
            GUI->BinaryInteger_ComboBox.Enable();

            GUI->ArbitraryInteger_CheckBox.Uncheck();
            GUI->ArbitraryInteger_Label.Disable();
            GUI->ArbitraryInteger_SpinBox.Disable();
         }
      }
      else
      {
         GUI->BinaryInteger_CheckBox.Uncheck();
         GUI->BinaryInteger_Label.Disable();
         GUI->BinaryInteger_ComboBox.Disable();

         GUI->ArbitraryInteger_CheckBox.Check();
         GUI->ArbitraryInteger_Label.Enable();
         GUI->ArbitraryInteger_SpinBox.Enable();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ReadoutOptionsInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   ReadoutOptions o = instance.Options();

   if ( sender == GUI->Data_ComboBox )
      o.SetData( ReadoutOptions::readout_data( itemIndex ) );
   else if ( sender == GUI->Mode_ComboBox )
      o.SetMode( ReadoutOptions::readout_mode( itemIndex ) );
   else if ( sender == GUI->ProbeSize_ComboBox )
      o.SetProbeSize( (itemIndex << 1) | 1 );
   else if ( sender == GUI->PreviewZoom_ComboBox )
      o.SetPreviewZoomFactor( itemIndex+1 );
   else if ( sender == GUI->Precision_ComboBox )
      o.SetPrecision( itemIndex );
   else if ( sender == GUI->BinaryInteger_ComboBox )
   {
      unsigned n;
      switch ( itemIndex )
      {
      case 0: n = 255; break;
      case 1: n = 1023; break;
      case 2: n = 4095; break;
      case 3: n = 16383; break;
      default:
      case 4: n = 65535; break;
      case 5: n = 1048575; break;
      case 6: n = 16777215; break;
      case 7: n = 4294967295ul; break;
      }
      o.SetIntegerRange( n );
   }

   instance.SetOptions( o );
   UpdateControls();
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInterface::__ButtonClick( Button& sender, bool checked )
{
   ReadoutOptions o = instance.Options();

   if ( sender == GUI->ShowAlphaChannel_CheckBox )
      o.EnableAlphaChannel( checked );
   else if ( sender == GUI->ShowMaskChannel_CheckBox )
      o.EnableMaskChannel( checked );
   else if ( sender == GUI->ShowPreview_CheckBox )
      o.EnablePreview( checked );
   else if ( sender == GUI->PreviewCenter_CheckBox )
      o.EnablePreviewCenter( checked );
   else if ( sender == GUI->Broadcast_CheckBox )
      o.EnableBroadcast( checked );
   else if ( sender == GUI->Real_CheckBox )
   {
      if ( checked )
         o.SetReal();
   }
   else if ( sender == GUI->BinaryInteger_CheckBox )
   {
      if ( checked )
      {
         // Transition from arbitrary range to binary

         o.SetInteger();

         if ( o.IntegerRange() < 1023 )
            o.SetIntegerRange( 255 );
         else if ( o.IntegerRange() < 4095 )
            o.SetIntegerRange( 1023 );
         else if ( o.IntegerRange() < 16383 )
            o.SetIntegerRange( 4095 );
         else if ( o.IntegerRange() < 65535 )
            o.SetIntegerRange( 16383 );
         else if ( o.IntegerRange() < 1048575 )
            o.SetIntegerRange( 65535 );
         else if ( o.IntegerRange() < 16777215 )
            o.SetIntegerRange( 1048575 );
         else if ( o.IntegerRange() < 4294967295ul )
            o.SetIntegerRange( 16777215 );
         else
            o.SetIntegerRange( 4294967295ul );

         GUI->BinaryInteger_CheckBox.Check();
         GUI->BinaryInteger_Label.Enable();
         GUI->BinaryInteger_ComboBox.Enable();

         GUI->ArbitraryInteger_CheckBox.Uncheck();
         GUI->ArbitraryInteger_Label.Disable();
         GUI->ArbitraryInteger_SpinBox.Disable();
      }
   }
   else if ( sender == GUI->ArbitraryInteger_CheckBox )
   {
      if ( checked )
      {
         // Transition from binary range to arbitrary

         o.SetInteger();

         GUI->BinaryInteger_CheckBox.Uncheck();
         GUI->BinaryInteger_Label.Disable();
         GUI->BinaryInteger_ComboBox.Disable();

         GUI->ArbitraryInteger_CheckBox.Check();
         GUI->ArbitraryInteger_Label.Enable();
         GUI->ArbitraryInteger_SpinBox.Enable();
      }
   }
   else if ( sender == GUI->LoadCurrentOptions_PushButton )
      o = ReadoutOptions::GetCurrentOptions();

   instance.SetOptions( o );
   UpdateControls();
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInterface::__ValueUpdated( SpinBox& sender, int value )
{
   ReadoutOptions o = instance.Options();

   if ( sender == GUI->PreviewSize_SpinBox )
   {
      value |= 1;
      o.SetPreviewSize( value );
      GUI->PreviewSize_SpinBox.SetValue( value );
   }
   else if ( sender == GUI->ArbitraryInteger_SpinBox )
      o.SetIntegerRange( value );

   instance.SetOptions( o );
   UpdateControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ReadoutOptionsInterface::GUIData::GUIData( ReadoutOptionsInterface& w )
{
   int labelWidth1 = w.Font().Width( String( "Preview size in pixels:" ) + 'M' );
   int labelWidth2 = w.Font().Width( String( "Arbitrary integer range. Max. Value:" ) + 'M' );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   Data_Label.SetText( "Data / Color space:" );
   Data_Label.SetFixedWidth( labelWidth1 );
   Data_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Data_ComboBox.AddItem( "RGB / K" );
   Data_ComboBox.AddItem( "RGB + L" );
   Data_ComboBox.AddItem( "RGB + Y" );
   Data_ComboBox.AddItem( "CIE XYZ" );
   Data_ComboBox.AddItem( "CIE L*a*b*" );
   Data_ComboBox.AddItem( "CIE L*c*h*" );
   Data_ComboBox.AddItem( "HSV" );
   Data_ComboBox.AddItem( "HSI" );
   Data_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ReadoutOptionsInterface::__ItemSelected, w );

   Data_Sizer.SetSpacing( 4 );
   Data_Sizer.Add( Data_Label );
   Data_Sizer.Add( Data_ComboBox, 100 );

   //

   Mode_Label.SetText( "Calculation mode:" );
   Mode_Label.SetFixedWidth( labelWidth1 );
   Mode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Mode_ComboBox.AddItem( "Average" );
   Mode_ComboBox.AddItem( "Median" );
   Mode_ComboBox.AddItem( "Minimum" );
   Mode_ComboBox.AddItem( "Maximum" );
   Mode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ReadoutOptionsInterface::__ItemSelected, w );

   Mode_Sizer.SetSpacing( 4 );
   Mode_Sizer.Add( Mode_Label );
   Mode_Sizer.Add( Mode_ComboBox, 100 );

   //

   ProbeSize_Label.SetText( "Probe size in pixels:" );
   ProbeSize_Label.SetFixedWidth( labelWidth1 );
   ProbeSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ProbeSize_ComboBox.AddItem( "Single Pixel" );
   ProbeSize_ComboBox.AddItem( "3x3" );
   ProbeSize_ComboBox.AddItem( "5x5" );
   ProbeSize_ComboBox.AddItem( "7x7" );
   ProbeSize_ComboBox.AddItem( "9x9" );
   ProbeSize_ComboBox.AddItem( "11x11" );
   ProbeSize_ComboBox.AddItem( "13x13" );
   ProbeSize_ComboBox.AddItem( "15x15" );
   ProbeSize_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ReadoutOptionsInterface::__ItemSelected, w );

   ProbeSize_Sizer.SetSpacing( 4 );
   ProbeSize_Sizer.Add( ProbeSize_Label );
   ProbeSize_Sizer.Add( ProbeSize_ComboBox, 100 );

   //

   ShowAlphaChannel_CheckBox.SetText( "Include alpha channel" );
   ShowAlphaChannel_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   Alpha_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Alpha_Sizer.Add( ShowAlphaChannel_CheckBox );

   //

   ShowMaskChannel_CheckBox.SetText( "Include mask channel" );
   ShowMaskChannel_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   Mask_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Mask_Sizer.Add( ShowMaskChannel_CheckBox );

   //

   ShowPreview_CheckBox.SetText( "Show readout preview" );
   ShowPreview_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   Preview_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Preview_Sizer.Add( ShowPreview_CheckBox );

   //

   PreviewCenter_CheckBox.SetText( "Preview center hairlines" );
   PreviewCenter_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   PreviewCenter_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PreviewCenter_Sizer.Add( PreviewCenter_CheckBox );

   //

   PreviewSize_Label.SetText( "Preview size in pixels:" );
   PreviewSize_Label.SetFixedWidth( labelWidth1 );
   PreviewSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PreviewSize_SpinBox.SetRange( ReadoutOptions::MinPreviewSize, ReadoutOptions::MaxPreviewSize );
   PreviewSize_SpinBox.SetStepSize( 2 );
   PreviewSize_SpinBox.SetVariableWidth();
   PreviewSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ReadoutOptionsInterface::__ValueUpdated, w );

   PreviewSize_Sizer.SetSpacing( 4 );
   PreviewSize_Sizer.Add( PreviewSize_Label );
   PreviewSize_Sizer.Add( PreviewSize_SpinBox );
   PreviewSize_Sizer.AddStretch();

   //

   PreviewZoom_Label.SetText( "Preview zoom factor:" );
   PreviewZoom_Label.SetFixedWidth( labelWidth1 );
   PreviewZoom_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   PreviewZoom_ComboBox.AddItem( "1:1" );
   PreviewZoom_ComboBox.AddItem( "2:1" );
   PreviewZoom_ComboBox.AddItem( "3:1" );
   PreviewZoom_ComboBox.AddItem( "4:1" );
   PreviewZoom_ComboBox.AddItem( "5:1" );
   PreviewZoom_ComboBox.AddItem( "6:1" );
   PreviewZoom_ComboBox.AddItem( "7:1" );
   PreviewZoom_ComboBox.AddItem( "8:1" );
   PreviewZoom_ComboBox.AddItem( "9:1" );
   PreviewZoom_ComboBox.AddItem( "10:1" );
   PreviewZoom_ComboBox.AddItem( "11:1" );
   PreviewZoom_ComboBox.AddItem( "12:1" );
   PreviewZoom_ComboBox.AddItem( "13:1" );
   PreviewZoom_ComboBox.AddItem( "14:1" );
   PreviewZoom_ComboBox.AddItem( "15:1" );
   PreviewZoom_ComboBox.AddItem( "16:1" );
   PreviewZoom_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ReadoutOptionsInterface::__ItemSelected, w );

   PreviewZoom_Sizer.SetSpacing( 4 );
   PreviewZoom_Sizer.Add( PreviewZoom_Label );
   PreviewZoom_Sizer.Add( PreviewZoom_ComboBox, 100 );

   //

   Left_Sizer.SetSpacing( 6 );
   Left_Sizer.Add( Data_Sizer );
   Left_Sizer.Add( Mode_Sizer );
   Left_Sizer.Add( ProbeSize_Sizer );
   Left_Sizer.Add( Alpha_Sizer );
   Left_Sizer.Add( Mask_Sizer );
   Left_Sizer.Add( Preview_Sizer );
   Left_Sizer.Add( PreviewCenter_Sizer );
   Left_Sizer.Add( PreviewSize_Sizer );
   Left_Sizer.Add( PreviewZoom_Sizer );

   //

   Precision_Label.SetText( "Normalized real range. Resolution:" );
   Precision_Label.SetFixedWidth( labelWidth2 );
   Precision_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Precision_ComboBox.AddItem( "1.0" );
   Precision_ComboBox.AddItem( "0.1" );
   Precision_ComboBox.AddItem( "0.01" );
   Precision_ComboBox.AddItem( "0.001" );
   Precision_ComboBox.AddItem( "0.0001" );
   Precision_ComboBox.AddItem( "1e-05" );
   Precision_ComboBox.AddItem( "1e-06" );
   Precision_ComboBox.AddItem( "1e-07" );
   Precision_ComboBox.AddItem( "1e-08" );
   Precision_ComboBox.AddItem( "1e-09" );
   Precision_ComboBox.AddItem( "1e-10" );
   Precision_ComboBox.AddItem( "1e-11" );
   Precision_ComboBox.AddItem( "1e-12" );
   Precision_ComboBox.AddItem( "1e-13" );
   Precision_ComboBox.AddItem( "1e-14" );
   Precision_ComboBox.AddItem( "1e-15" );
   Precision_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ReadoutOptionsInterface::__ItemSelected, w );

   Real_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   Real_Sizer.SetSpacing( 4 );
   Real_Sizer.Add( Precision_Label );
   Real_Sizer.Add( Precision_ComboBox, 100 );
   Real_Sizer.Add( Real_CheckBox );

   //

   BinaryInteger_Label.SetText( "Binary integer range. Bit Count:" );
   BinaryInteger_Label.SetFixedWidth( labelWidth2 );
   BinaryInteger_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BinaryInteger_ComboBox.AddItem( "8 (256 values)" );
   BinaryInteger_ComboBox.AddItem( "10 (1,024)" );
   BinaryInteger_ComboBox.AddItem( "12 (4,096)" );
   BinaryInteger_ComboBox.AddItem( "14 (16,384)" );
   BinaryInteger_ComboBox.AddItem( "16 (65,536)" );
   BinaryInteger_ComboBox.AddItem( "20 (1M)" );
   BinaryInteger_ComboBox.AddItem( "24 (16M)" );
   BinaryInteger_ComboBox.AddItem( "32 (4G)" );
   BinaryInteger_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ReadoutOptionsInterface::__ItemSelected, w );

   BinaryInteger_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   BinaryInteger_Sizer.SetSpacing( 4 );
   BinaryInteger_Sizer.Add( BinaryInteger_Label );
   BinaryInteger_Sizer.Add( BinaryInteger_ComboBox, 100 );
   BinaryInteger_Sizer.Add( BinaryInteger_CheckBox );

   //

   ArbitraryInteger_Label.SetText( "Arbitrary integer range. Max. Value:" );
   ArbitraryInteger_Label.SetFixedWidth( labelWidth2 );
   ArbitraryInteger_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ArbitraryInteger_SpinBox.SetRange( 1, int_max );
   ArbitraryInteger_SpinBox.SetVariableWidth();
   ArbitraryInteger_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ReadoutOptionsInterface::__ValueUpdated, w );

   ArbitraryInteger_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   ArbitraryInteger_Sizer.SetSpacing( 4 );
   ArbitraryInteger_Sizer.Add( ArbitraryInteger_Label );
   ArbitraryInteger_Sizer.Add( ArbitraryInteger_SpinBox, 100 );
   ArbitraryInteger_Sizer.Add( ArbitraryInteger_CheckBox );

   //

   Broadcast_CheckBox.SetText( "Broadcast readouts" );
   Broadcast_CheckBox.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   Broadcast_Sizer.AddUnscaledSpacing( labelWidth2 + ui4 );
   Broadcast_Sizer.Add( Broadcast_CheckBox );

   //

   LoadCurrentOptions_PushButton.SetText( "  Load Current Readout Options  " );
   LoadCurrentOptions_PushButton.OnClick( (Button::click_event_handler)&ReadoutOptionsInterface::__ButtonClick, w );

   LoadCurrentOptions_Sizer.AddStretch();
   LoadCurrentOptions_Sizer.Add( LoadCurrentOptions_PushButton );

   //

   Right_Sizer.SetSpacing( 6 );
   Right_Sizer.Add( Real_Sizer );
   Right_Sizer.Add( BinaryInteger_Sizer );
   Right_Sizer.Add( ArbitraryInteger_Sizer );
   Right_Sizer.AddStretch();
   Right_Sizer.Add( Broadcast_Sizer );
   Right_Sizer.AddSpacing( 12 );
   Right_Sizer.Add( LoadCurrentOptions_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 16 );
   Global_Sizer.Add( Left_Sizer );
   Global_Sizer.Add( Right_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsInterface.cpp - Released 2015/10/08 11:24:39 UTC
