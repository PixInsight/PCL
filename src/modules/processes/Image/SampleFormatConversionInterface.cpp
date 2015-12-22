//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0348
// ----------------------------------------------------------------------------
// SampleFormatConversionInterface.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "SampleFormatConversionInterface.h"
#include "SampleFormatConversionProcess.h"

#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SampleFormatConversionInterface* TheSampleFormatConversionInterface = 0;

// ----------------------------------------------------------------------------

#include "SampleFormatConversionIcon.xpm"

// ----------------------------------------------------------------------------

SampleFormatConversionInterface::SampleFormatConversionInterface() :
ProcessInterface(), instance( TheSampleFormatConversionProcess ), GUI( 0 )
{
   TheSampleFormatConversionInterface = this;
}

// ----------------------------------------------------------------------------

SampleFormatConversionInterface::~SampleFormatConversionInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString SampleFormatConversionInterface::Id() const
{
   return "SampleFormatConversion";
}

// ----------------------------------------------------------------------------

MetaProcess* SampleFormatConversionInterface::Process() const
{
   return TheSampleFormatConversionProcess;
}

// ----------------------------------------------------------------------------

const char** SampleFormatConversionInterface::IconImageXPM() const
{
   return SampleFormatConversionIcon_XPM;
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

InterfaceFeatures SampleFormatConversionInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::TrackViewUpdated( bool active )
{
   if ( GUI != 0 )
   {
      GUI->SetupTrackViewControls( *this, active );
      if ( active )
         UpdateCurrentViewInfoControls();
   }
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ResetInstance()
{
   SampleFormatConversionInstance defaultInstance( TheSampleFormatConversionProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );

      SetWindowTitle( "SampleFormatConversion" );

      UpdateConversionControls();

      if ( IsTrackViewActive() )
         UpdateCurrentViewInfoControls();
   }

   dynamic = false;
   return &P == TheSampleFormatConversionProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SampleFormatConversionInterface::NewProcess() const
{
   return new SampleFormatConversionInstance( instance );
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const SampleFormatConversionInstance* r = dynamic_cast<const SampleFormatConversionInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a SampleFormatConversion instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );

   UpdateConversionControls();

   if ( IsTrackViewActive() )
      UpdateCurrentViewInfoControls();

   return true;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ImageCreated( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() && v == ImageWindow::ActiveWindow().MainView() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ImageUpdated( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() && v == ImageWindow::ActiveWindow().MainView() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ImageRenamed( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() && v == ImageWindow::ActiveWindow().MainView() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ImageDeleted( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ImageFocused( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ImageLocked( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() && v == ImageWindow::ActiveWindow().MainView() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::ImageUnlocked( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() && v == ImageWindow::ActiveWindow().MainView() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::UpdateConversionControls()
{
   switch ( instance.sampleFormat )
   {
   case SampleFormatConversion::To8Bit:
      GUI->Integer8Bit_RadioButton.Check();
      break;
   case SampleFormatConversion::To16Bit:
      GUI->Integer16Bit_RadioButton.Check();
      break;
   case SampleFormatConversion::To32Bit:
      GUI->Integer32Bit_RadioButton.Check();
      break;
   case SampleFormatConversion::ToFloat:
      GUI->FloatingPoint32Bit_RadioButton.Check();
      break;
   case SampleFormatConversion::ToDouble:
      GUI->FloatingPoint64Bit_RadioButton.Check();
      break;
   }
}

void SampleFormatConversionInterface::UpdateCurrentViewInfoControls()
{
   ImageWindow w( ImageWindow::ActiveWindow() );

   if ( w.IsNull() )
      GUI->CurrentViewInfo_Label.SetText( "<* No image *>" );
   else if ( !w.MainView().CanRead() )
      GUI->CurrentViewInfo_Label.SetText( "<* Image locked *>" );
   else
   {
      int newBytesPerSample;
      switch ( instance.sampleFormat )
      {
      case SampleFormatConversion::To8Bit:
         newBytesPerSample = 1; break;
      case SampleFormatConversion::To16Bit:
         newBytesPerSample = 2; break;
      default:
      case SampleFormatConversion::To32Bit:
      case SampleFormatConversion::ToFloat:
         newBytesPerSample = 4; break;
      case SampleFormatConversion::ToDouble:
         newBytesPerSample = 8; break;
      }

      int bitsPerSample;
      bool isFloatSample;
      w.GetSampleFormat( bitsPerSample, isFloatSample );
      View v = w.MainView();
      size_type N = v.Image()->NumberOfSamples();
      GUI->CurrentViewInfo_Label.SetText( v.Id().AppendFormat(
                        ":\n%d-bit %s, %.3lf MB, new size: %.3lf MB",
                        bitsPerSample,
                        isFloatSample ? "floating-point" : "integer",
                        (N*(bitsPerSample >> 3))/1048576.0,
                        (N*newBytesPerSample)/1048576.0 ) );
   }
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::SampleFormatButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Integer8Bit_RadioButton )
      instance.sampleFormat = SampleFormatConversion::To8Bit;
   else if ( sender == GUI->Integer16Bit_RadioButton )
      instance.sampleFormat = SampleFormatConversion::To16Bit;
   else if ( sender == GUI->Integer32Bit_RadioButton )
      instance.sampleFormat = SampleFormatConversion::To32Bit;
   else if ( sender == GUI->FloatingPoint32Bit_RadioButton )
      instance.sampleFormat = SampleFormatConversion::ToFloat;
   else if ( sender == GUI->FloatingPoint64Bit_RadioButton )
      instance.sampleFormat = SampleFormatConversion::ToDouble;

   if ( IsTrackViewActive() )
      UpdateCurrentViewInfoControls();
}

// ----------------------------------------------------------------------------

SampleFormatConversionInterface::GUIData::GUIData( SampleFormatConversionInterface& w )
{
   Integer8Bit_RadioButton.SetText( "8-bit unsigned integer [0,255]" );
   Integer8Bit_RadioButton.OnClick( (pcl::Button::click_event_handler)&SampleFormatConversionInterface::SampleFormatButtonClick, w );

   Integer16Bit_RadioButton.SetText( "16-bit unsigned integer [0,65535]" );
   Integer16Bit_RadioButton.OnClick( (pcl::Button::click_event_handler)&SampleFormatConversionInterface::SampleFormatButtonClick, w );

   Integer32Bit_RadioButton.SetText( "32-bit unsigned integer [0,4294967295]" );
   Integer32Bit_RadioButton.OnClick( (pcl::Button::click_event_handler)&SampleFormatConversionInterface::SampleFormatButtonClick, w );

   FloatingPoint32Bit_RadioButton.SetText( "IEEE 754 32-bit floating point (single precision)" );
   FloatingPoint32Bit_RadioButton.SetToolTip( "<p>Normalized real range: [0,1]<br>"
                                              "<i>6 - 7 decimal digits accuracy</i></p>" );
   FloatingPoint32Bit_RadioButton.OnClick( (pcl::Button::click_event_handler)&SampleFormatConversionInterface::SampleFormatButtonClick, w );

   FloatingPoint64Bit_RadioButton.SetText( "IEEE 754 64-bit floating point (double precision)" );
   FloatingPoint64Bit_RadioButton.SetToolTip( "<p>Normalized real range: [0,1]<br>"
                                              "<i>14 - 15 decimal digits accuracy</i></p>" );
   FloatingPoint64Bit_RadioButton.OnClick( (pcl::Button::click_event_handler)&SampleFormatConversionInterface::SampleFormatButtonClick, w );

   Conversion_Sizer.SetMargin( 6 );
   Conversion_Sizer.SetSpacing( 0 );
   Conversion_Sizer.Add( Integer8Bit_RadioButton );
   Conversion_Sizer.Add( Integer16Bit_RadioButton );
   Conversion_Sizer.Add( Integer32Bit_RadioButton );
   Conversion_Sizer.Add( FloatingPoint32Bit_RadioButton );
   Conversion_Sizer.Add( FloatingPoint64Bit_RadioButton );

   Conversion_GroupBox.SetTitle( "Sample Format" );
   Conversion_GroupBox.SetSizer( Conversion_Sizer );
   Conversion_GroupBox.AdjustToContents();
   Conversion_GroupBox.SetMinSize();

   CurrentViewInfo_Label.SetText( "T\nT" ); // ensure enough space to show two lines of text
   CurrentViewInfo_Label.AdjustToContents();
   CurrentViewInfo_Label.SetFixedHeight();
   CurrentViewInfo_Label.Clear();
   CurrentViewInfo_Label.DisableWordWrapping();

   CurrentViewInfo_Sizer.SetMargin( 6 );
   CurrentViewInfo_Sizer.Add( CurrentViewInfo_Label, 100 );

   CurrentViewInfo_GroupBox.SetTitle( "Current Image" );
   CurrentViewInfo_GroupBox.SetSizer( CurrentViewInfo_Sizer );
   CurrentViewInfo_GroupBox.SetMinWidth( Conversion_GroupBox.Width() );
   CurrentViewInfo_GroupBox.AdjustToContents();
   CurrentViewInfo_GroupBox.SetMinHeight();

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Conversion_GroupBox );
   Global_Sizer.Add( CurrentViewInfo_GroupBox );

   if ( !w.IsTrackViewActive() )
      CurrentViewInfo_GroupBox.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

void SampleFormatConversionInterface::GUIData::SetupTrackViewControls( SampleFormatConversionInterface& w, bool active )
{
   if ( active )
      CurrentViewInfo_GroupBox.Show();
   else
      CurrentViewInfo_GroupBox.Hide();

   w.SetVariableHeight();
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionInterface.cpp - Released 2015/12/18 08:55:08 UTC
