//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0427
// ----------------------------------------------------------------------------
// NewImageInterface.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "NewImageInterface.h"
#include "NewImageProcess.h"

#include <pcl/Color.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>

#define AUTO_ID   "<Auto>"

namespace pcl
{

// ----------------------------------------------------------------------------

NewImageInterface* TheNewImageInterface = nullptr;

// ----------------------------------------------------------------------------

#include "NewImageIcon.xpm"

// ----------------------------------------------------------------------------

NewImageInterface::NewImageInterface() :
   instance( TheNewImageProcess )
{
   TheNewImageInterface = this;
}

// ----------------------------------------------------------------------------

NewImageInterface::~NewImageInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString NewImageInterface::Id() const
{
   return "NewImage";
}

// ----------------------------------------------------------------------------

MetaProcess* NewImageInterface::Process() const
{
   return TheNewImageProcess;
}

// ----------------------------------------------------------------------------

const char** NewImageInterface::IconImageXPM() const
{
   return NewImageIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures NewImageInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void NewImageInterface::ApplyInstance() const
{
   instance.LaunchGlobal();
}

// ----------------------------------------------------------------------------

void NewImageInterface::ResetInstance()
{
   NewImageInstance defaultInstance( TheNewImageProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool NewImageInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "NewImage" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheNewImageProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* NewImageInterface::NewProcess() const
{
   return new NewImageInstance( instance );
}

// ----------------------------------------------------------------------------

bool NewImageInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const NewImageInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a NewImage instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool NewImageInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool NewImageInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

bool NewImageInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

#define ISCOLOR   (instance.colorSpace == NewImageColorSpace::RGB)
#define ISGRAY    (instance.colorSpace == NewImageColorSpace::Gray)
#define HASALPHA  (instance.numberOfChannels > (ISCOLOR ? 3 : 1))

void NewImageInterface::UpdateReadout( const View& v, const DPoint&, double R, double G, double B, double A )
{
   if ( GUI != nullptr )
      if ( IsVisible() )
      {
         if ( ISCOLOR || !v.IsColor() )
         {
            instance.v0 = R;
            instance.v1 = G;
            instance.v2 = B;
         }
         else
         {
            RGBColorSystem rgb;
            v.Window().GetRGBWS( rgb );
            instance.v0 = instance.v1 = instance.v2 = rgb.Lightness( R, G, B );
         }

         GUI->V0_NumericControl.SetValue( instance.v0 );
         GUI->V1_NumericControl.SetValue( instance.v1 );
         GUI->V2_NumericControl.SetValue( instance.v2 );

         if ( HASALPHA )
         {
            instance.va = A;
            GUI->VA_NumericControl.SetValue( instance.va );
         }

         GUI->ColorSample_Control.Update();
      }
}

// ----------------------------------------------------------------------------

void NewImageInterface::UpdateControls()
{
   GUI->Identifier_Edit.SetText( instance.id.IsEmpty() ? AUTO_ID : instance.id );

   GUI->SampleFormat_ComboBox.SetCurrentItem( instance.sampleFormat );

   GUI->RGBColor_RadioButton.SetChecked( ISCOLOR );
   GUI->Grayscale_RadioButton.SetChecked( ISGRAY );

   GUI->Width_SpinBox.SetValue( instance.width );
   GUI->Height_SpinBox.SetValue( instance.height );
   GUI->Channels_SpinBox.SetValue( instance.numberOfChannels );

   UpdateSizeInfo();

   GUI->V0_NumericControl.label.SetText( ISCOLOR ? "R:" : "K:" );
   GUI->V0_NumericControl.SetValue( instance.v0 );

   GUI->V1_NumericControl.Enable( ISCOLOR );
   GUI->V1_NumericControl.SetValue( instance.v1 );

   GUI->V2_NumericControl.Enable( ISCOLOR );
   GUI->V2_NumericControl.SetValue( instance.v2 );

   GUI->VA_NumericControl.Enable( HASALPHA );
   GUI->VA_NumericControl.SetValue( instance.va );

   GUI->ColorSample_Control.Update();
}

void NewImageInterface::UpdateInitialValue( Edit& e, Slider& s, double v )
{
   e.SetText( String().Format( "%.8f", v ) );
   s.SetValue( RoundInt( v*s.MaxValue() ) );
}

void NewImageInterface::UpdateSizeInfo()
{
   size_type bytesPerSample;

   switch ( instance.sampleFormat )
   {
   case NewImageSampleFormat::I8:   bytesPerSample = 1; break;
   case NewImageSampleFormat::I16:  bytesPerSample = 2; break;
   case NewImageSampleFormat::I32:
   default:
   case NewImageSampleFormat::F32:  bytesPerSample = 4; break;
   case NewImageSampleFormat::F64:  bytesPerSample = 8; break;
   }

   double N = (double( instance.width )/1048576)
      * instance.height * instance.numberOfChannels * bytesPerSample;
   GUI->SizeInfo_Label.SetText( (N < 10000) ? String().Format( "%.3f MB", N ) : String().Format( "%.3f GB", N/1024 ) );
}

// ----------------------------------------------------------------------------

void NewImageInterface::__Identifier_GetFocus( Control& /*sender*/ )
{
   if ( GUI->Identifier_Edit.Text() == AUTO_ID )
      GUI->Identifier_Edit.Clear();
}

void NewImageInterface::__Identifier_EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text().Trimmed();
      if ( !id.IsEmpty() && id != AUTO_ID && !id.IsValidIdentifier() )
         throw Error( "Invalid identifier: " + id );

      instance.id = (id != AUTO_ID) ? id : String();
      sender.SetText( instance.id.IsEmpty() ? AUTO_ID : instance.id );
      return;
   }

   ERROR_CLEANUP(
      sender.SetText( instance.id );
      sender.SelectAll();
      sender.Focus()
   )
}

void NewImageInterface::__SampleFormat_ItemSelected( ComboBox& sender, int itemIndex )
{
   instance.sampleFormat = itemIndex;
   UpdateSizeInfo();
}

void NewImageInterface::__ColorSpace_Click( Button& sender, bool checked )
{
   int alphaChannels = instance.numberOfChannels - (ISCOLOR ? 3 : 1);

   instance.colorSpace = (sender == GUI->RGBColor_RadioButton) ?
                           NewImageColorSpace::RGB : NewImageColorSpace::Gray;

   if ( ISCOLOR )
   {
      instance.numberOfChannels = 3 + alphaChannels;
      GUI->Channels_SpinBox.SetValue( 3 );
      GUI->Channels_SpinBox.SetMinValue( 3 );
   }
   else
   {
      instance.numberOfChannels = 1 + alphaChannels;
      GUI->Channels_SpinBox.SetValue( 1 );
      GUI->Channels_SpinBox.SetMinValue( 1 );

      instance.v1 = instance.v2 = instance.v0;
   }

   UpdateControls();
}

void NewImageInterface::__Geometry_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Width_SpinBox )
      instance.width = value;
   else if ( sender == GUI->Height_SpinBox )
      instance.height = value;
   else if ( sender == GUI->Channels_SpinBox )
      instance.numberOfChannels = value;

   UpdateControls();
}

void NewImageInterface::SetAsImage( const ImageWindow& window )
{
   if ( !window.IsNull() )
   {
      ImageVariant image = window.MainView().Image();
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: instance.sampleFormat = NewImageSampleFormat::F32; break;
         case 64: instance.sampleFormat = NewImageSampleFormat::F64; break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: instance.sampleFormat = NewImageSampleFormat::I8; break;
         case 16: instance.sampleFormat = NewImageSampleFormat::I16; break;
         case 32: instance.sampleFormat = NewImageSampleFormat::I32; break;
         }

      instance.colorSpace = image->IsColor() ? NewImageColorSpace::RGB : NewImageColorSpace::Gray;
      instance.width = image->Width();
      instance.height = image->Height();
      instance.numberOfChannels = image->NumberOfChannels();
      UpdateControls();
   }
}

void NewImageInterface::__SetAsActiveImage_Click( Button& /*sender*/, bool /*checked*/ )
{
   SetAsImage( ImageWindow::ActiveWindow() );
}

void NewImageInterface::__InitialValue_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->V0_NumericControl )
   {
      instance.v0 = value;

      if ( ISGRAY )
      {
         GUI->V1_NumericControl.SetValue( instance.v1 = value );
         GUI->V2_NumericControl.SetValue( instance.v2 = value );
      }
   }
   else if ( sender == GUI->V1_NumericControl )
      instance.v1 = value;
   else if ( sender == GUI->V2_NumericControl )
      instance.v2 = value;
   else if ( sender == GUI->VA_NumericControl )
      instance.va = value;

   GUI->ColorSample_Control.Update();
}

void NewImageInterface::__ColorSample_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   Graphics g( sender );

   RGBA rgba = RGBAColor( float( instance.v0 ), float( instance.v1 ), float( instance.v2 ) );

   if ( HASALPHA )
   {
      g.SetBrush( Bitmap( sender.ScaledResource( ":/image-window/transparent-small.png" ) ) );
      g.SetPen( Pen::Null() );
      g.DrawRect( sender.BoundsRect() );

      SetAlpha( rgba, RoundInt( 255*instance.va ) );
   }

   g.SetBrush( rgba );
   g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
   g.DrawRect( sender.BoundsRect() );
}

void NewImageInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->Identifier_Edit || sender == GUI->SetAsActiveImage_Button )
      wantsView = view.IsMainView();
}

void NewImageInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( view.IsMainView() )
      if ( sender == GUI->Identifier_Edit )
         GUI->Identifier_Edit.SetText( instance.id = view.Id() );
      else if ( sender == GUI->SetAsActiveImage_Button )
         SetAsImage( view.Window() );
}

// ----------------------------------------------------------------------------

NewImageInterface::GUIData::GUIData( NewImageInterface& w )
{
   int labelWidth1 = w.Font().Width( String( "Sample format:" ) + 'M' );
   int labelWidth2 = w.Font().Width( String( 'M',  2 ) );

   //

   Identifier_Label.SetText( "Identifier:" );
   Identifier_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Identifier_Label.SetFixedWidth( labelWidth1 );

   Identifier_Edit.OnGetFocus( (Control::event_handler)&NewImageInterface::__Identifier_GetFocus, w );
   Identifier_Edit.OnEditCompleted( (Edit::edit_event_handler)&NewImageInterface::__Identifier_EditCompleted, w );
   Identifier_Edit.OnViewDrag( (Control::view_drag_event_handler)&NewImageInterface::__ViewDrag, w );
   Identifier_Edit.OnViewDrop( (Control::view_drop_event_handler)&NewImageInterface::__ViewDrop, w );

   Identifier_Sizer.SetSpacing( 4 );
   Identifier_Sizer.Add( Identifier_Label );
   Identifier_Sizer.Add( Identifier_Edit, 100 );

   //

   SampleFormat_Label.SetText( "Sample format:" );
   SampleFormat_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SampleFormat_Label.SetFixedWidth( labelWidth1 );

   SampleFormat_ComboBox.AddItem( "8-bit unsigned integer (byte, 0 -> 255)" );
   SampleFormat_ComboBox.AddItem( "16-bit unsigned integer (word, 0 -> 65535)" );
   SampleFormat_ComboBox.AddItem( "32-bit unsigned integer (double word, 0 -> 4G)" );
   SampleFormat_ComboBox.AddItem( "32-bit IEEE 754 floating point (single precision)" );
   SampleFormat_ComboBox.AddItem( "64-bit IEEE 754 floating point (double precision)" );
   SampleFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&NewImageInterface::__SampleFormat_ItemSelected, w );

   SampleFormat_Sizer.SetSpacing( 4 );
   SampleFormat_Sizer.Add( SampleFormat_Label );
   SampleFormat_Sizer.Add( SampleFormat_ComboBox, 100 );

   //

   ColorSpace_Label.SetText( "Color space:" );
   ColorSpace_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ColorSpace_Label.SetFixedWidth( labelWidth1 );

   RGBColor_RadioButton.SetText( "RGB Color" );
   RGBColor_RadioButton.OnClick( (Button::click_event_handler)&NewImageInterface::__ColorSpace_Click, w );

   Grayscale_RadioButton.SetText( "Grayscale" );
   Grayscale_RadioButton.OnClick( (Button::click_event_handler)&NewImageInterface::__ColorSpace_Click, w );

   ColorSpace_Sizer.SetSpacing( 4 );
   ColorSpace_Sizer.Add( ColorSpace_Label );
   ColorSpace_Sizer.Add( RGBColor_RadioButton );
   ColorSpace_Sizer.Add( Grayscale_RadioButton );
   ColorSpace_Sizer.AddStretch();

   //

   Width_Label.SetText( "Width:" );
   Width_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Width_Label.SetFixedWidth( labelWidth1 );

   Width_SpinBox.SetRange( 1, int_max );
   Width_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&NewImageInterface::__Geometry_ValueUpdated, w );

   Width_Sizer.SetSpacing( 4 );
   Width_Sizer.Add( Width_Label );
   Width_Sizer.Add( Width_SpinBox );
   Width_Sizer.AddStretch();

   //

   Height_Label.SetText( "Height:" );
   Height_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Height_Label.SetFixedWidth( labelWidth1 );

   Height_SpinBox.SetRange( 1, int_max );
   Height_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&NewImageInterface::__Geometry_ValueUpdated, w );

   Height_Sizer.SetSpacing( 4 );
   Height_Sizer.Add( Height_Label );
   Height_Sizer.Add( Height_SpinBox );
   Height_Sizer.AddStretch();

   //

   Channels_Label.SetText( "Channels:" );
   Channels_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Channels_Label.SetFixedWidth( labelWidth1 );

   Channels_SpinBox.SetRange( 1, int_max );
   Channels_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&NewImageInterface::__Geometry_ValueUpdated, w );

   Channels_Sizer.SetSpacing( 4 );
   Channels_Sizer.Add( Channels_Label );
   Channels_Sizer.Add( Channels_SpinBox );
   Channels_Sizer.AddStretch();

   //

   GeometryLeft_Sizer.SetSpacing( 4 );
   GeometryLeft_Sizer.Add( Width_Sizer );
   GeometryLeft_Sizer.Add( Height_Sizer );
   GeometryLeft_Sizer.Add( Channels_Sizer );

   //

   GeometryMiddle_Sizer.AddStretch();
   GeometryMiddle_Sizer.Add( SizeInfo_Label );
   GeometryMiddle_Sizer.AddStretch();

   //

   SetAsActiveImage_Button.SetText( "Set As\nActive Image" );
   SetAsActiveImage_Button.SetVariableWidth();
   SetAsActiveImage_Button.OnClick( (Button::click_event_handler)&NewImageInterface::__SetAsActiveImage_Click, w );
   SetAsActiveImage_Button.OnViewDrag( (Control::view_drag_event_handler)&NewImageInterface::__ViewDrag, w );
   SetAsActiveImage_Button.OnViewDrop( (Control::view_drop_event_handler)&NewImageInterface::__ViewDrop, w );

   GeometryRight_Sizer.AddStretch();
   GeometryRight_Sizer.Add( SetAsActiveImage_Button );
   GeometryRight_Sizer.AddStretch();

   //

   Geometry_Sizer.SetSpacing( 10 );
   Geometry_Sizer.Add( GeometryLeft_Sizer );
   Geometry_Sizer.Add( GeometryMiddle_Sizer, 100 );
   Geometry_Sizer.Add( GeometryRight_Sizer );

   //

   ImageParameters_Sizer.SetSpacing( 4 );
   ImageParameters_Sizer.SetMargin( 6 );
   ImageParameters_Sizer.Add( Identifier_Sizer );
   ImageParameters_Sizer.Add( SampleFormat_Sizer );
   ImageParameters_Sizer.Add( ColorSpace_Sizer );
   ImageParameters_Sizer.Add( Geometry_Sizer );

   ImageParameters_GroupBox.SetTitle( "Image Parameters" );
   ImageParameters_GroupBox.SetSizer( ImageParameters_Sizer );
   ImageParameters_GroupBox.AdjustToContents();
   ImageParameters_GroupBox.SetMinSize();

   //

   V0_NumericControl.label.SetText( "R:" );
   V0_NumericControl.label.SetFixedWidth( labelWidth2 );
   V0_NumericControl.slider.SetRange( 0, 100 );
   V0_NumericControl.SetReal();
   V0_NumericControl.SetRange( 0, 1 );
   V0_NumericControl.SetPrecision( 8 );
   V0_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&NewImageInterface::__InitialValue_ValueUpdated, w );

   //

   V1_NumericControl.label.SetText( "G:" );
   V1_NumericControl.label.SetFixedWidth( labelWidth2 );
   V1_NumericControl.slider.SetRange( 0, 100 );
   V1_NumericControl.SetReal();
   V1_NumericControl.SetRange( 0, 1 );
   V1_NumericControl.SetPrecision( 8 );
   V1_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&NewImageInterface::__InitialValue_ValueUpdated, w );

   //

   V2_NumericControl.label.SetText( "B:" );
   V2_NumericControl.label.SetFixedWidth( labelWidth2 );
   V2_NumericControl.slider.SetRange( 0, 100 );
   V2_NumericControl.SetReal();
   V2_NumericControl.SetRange( 0, 1 );
   V2_NumericControl.SetPrecision( 8 );
   V2_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&NewImageInterface::__InitialValue_ValueUpdated, w );

   //

   VA_NumericControl.label.SetText( "A:" );
   VA_NumericControl.label.SetFixedWidth( labelWidth2 );
   VA_NumericControl.slider.SetRange( 0, 100 );
   VA_NumericControl.SetReal();
   VA_NumericControl.SetRange( 0, 1 );
   VA_NumericControl.SetPrecision( 8 );
   VA_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&NewImageInterface::__InitialValue_ValueUpdated, w );

   //

   InitialValuesLeft_Sizer.SetSpacing( 4 );
   InitialValuesLeft_Sizer.Add( V0_NumericControl );
   InitialValuesLeft_Sizer.Add( V1_NumericControl );
   InitialValuesLeft_Sizer.Add( V2_NumericControl );
   InitialValuesLeft_Sizer.Add( VA_NumericControl );

   //

   ColorSample_Control.SetScaledFixedWidth( 60 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&NewImageInterface::__ColorSample_Paint, w );

   //

   InitialValues_Sizer.SetSpacing( 4 );
   InitialValues_Sizer.SetMargin( 6 );
   InitialValues_Sizer.Add( InitialValuesLeft_Sizer );
   InitialValues_Sizer.Add( ColorSample_Control );

   InitialValues_GroupBox.SetTitle( "Initial Values" );
   InitialValues_GroupBox.SetSizer( InitialValues_Sizer );
   InitialValues_GroupBox.SetMinWidth( ImageParameters_GroupBox.Width() );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( ImageParameters_GroupBox );
   Global_Sizer.Add( InitialValues_GroupBox );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NewImageInterface.cpp - Released 2018-11-23T18:45:58Z
