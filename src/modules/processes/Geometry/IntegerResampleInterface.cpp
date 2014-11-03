// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Geometry Process Module Version 01.01.00.0245
// ****************************************************************************
// IntegerResampleInterface.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "IntegerResampleInterface.h"
#include "IntegerResampleProcess.h"
#include "IntegerResampleParameters.h"

#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegerResampleInterface* TheIntegerResampleInterface = 0;

// ----------------------------------------------------------------------------

#include "IntegerResampleIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView  GUI->AllImages_ViewList.CurrentView()
#define downsample   GUI->Downsample_RadioButton.IsChecked()
#define upsample     GUI->Upsample_RadioButton.IsChecked()

// ----------------------------------------------------------------------------

IntegerResampleInterface::IntegerResampleInterface() :
ProcessInterface(),
instance( TheIntegerResampleProcess ),
sourceWidth( 1000 ),
sourceHeight( 1000 ),
GUI( 0 )
{
   TheIntegerResampleInterface = this;
}

IntegerResampleInterface::~IntegerResampleInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString IntegerResampleInterface::Id() const
{
   return "IntegerResample";
}

MetaProcess* IntegerResampleInterface::Process() const
{
   return TheIntegerResampleProcess;
}

const char** IntegerResampleInterface::IconImageXPM() const
{
   return IntegerResampleIcon_XPM;
}

InterfaceFeatures IntegerResampleInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

void IntegerResampleInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void IntegerResampleInterface::TrackViewUpdated( bool active )
{
   if ( GUI != 0 && active )
   {
      ImageWindow w = ImageWindow::ActiveWindow();

      if ( !w.IsNull() )
         ImageFocused( w.MainView() );
      else
         UpdateControls();
   }
}

void IntegerResampleInterface::ResetInstance()
{
   IntegerResampleInstance defaultInstance( TheIntegerResampleProcess );
   ImportProcess( defaultInstance );
}

bool IntegerResampleInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "IntegerResample" );
      GUI->AllImages_ViewList.Regenerate( true, false ); // exclude previews
      UpdateControls();
   }

   dynamic = false;
   return &P == TheIntegerResampleProcess;
}

ProcessImplementation* IntegerResampleInterface::NewProcess() const
{
   return new IntegerResampleInstance( instance );
}

bool IntegerResampleInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const IntegerResampleInstance* r = dynamic_cast<const IntegerResampleInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not an IntegerResample instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool IntegerResampleInterface::RequiresInstanceValidation() const
{
   return true;
}

bool IntegerResampleInterface::ImportProcess( const ProcessImplementation& p )
{
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );

   instance.Assign( p );
   UpdateControls();
   return true;
}

bool IntegerResampleInterface::WantsImageNotifications() const
{
   return true;
}

void IntegerResampleInterface::ImageUpdated( const View& v )
{
   if ( GUI != 0 && v == currentView )
   {
      v.Window().MainView().GetSize( sourceWidth, sourceHeight );
      UpdateControls();
   }
}

void IntegerResampleInterface::ImageFocused( const View& v )
{
   if ( GUI != 0 && IsTrackViewActive() )
      if ( !v.IsNull() )
      {
         ImageWindow w = v.Window();
         View mainView = w.MainView();

         mainView.GetSize( sourceWidth, sourceHeight );

         GUI->AllImages_ViewList.SelectView( mainView ); // normally not necessary, but we can invoke this f() directly

         double xRes, yRes;
         bool metric;
         w.GetResolution( xRes, yRes, metric );

         instance.p_resolution.x = xRes;
         instance.p_resolution.y = yRes;
         instance.p_metric = metric;

         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::UpdateControls()
{
   GUI->ResampleFactor_SpinBox.SetValue( Abs( instance.p_zoomFactor ) );

   GUI->Downsample_RadioButton.SetChecked( instance.p_zoomFactor < 0 );
   GUI->Upsample_RadioButton.SetChecked( instance.p_zoomFactor > 0 );

   GUI->DownsampleMode_ComboBox.SetCurrentItem( instance.p_downsampleMode );

   int w = sourceWidth, h = sourceHeight;
   instance.GetNewSizes( w, h );

   double wcm, hcm, win, hin;

   if ( instance.p_metric )
   {
      wcm = w/instance.p_resolution.x;
      hcm = h/instance.p_resolution.y;
      win = wcm/2.54;
      hin = hcm/2.54;
   }
   else
   {
      win = w/instance.p_resolution.x;
      hin = h/instance.p_resolution.y;
      wcm = win*2.54;
      hcm = hin*2.54;
   }

   GUI->SourceWidthPixels_NumericEdit.SetValue( sourceWidth );
   GUI->SourceWidthPixels_NumericEdit.Enable( currentView.IsNull() );

   GUI->TargetWidthPixels_NumericEdit.SetValue( w );
   GUI->TargetWidthCentimeters_NumericEdit.SetValue( wcm );
   GUI->TargetWidthInches_NumericEdit.SetValue( win );

   GUI->SourceHeightPixels_NumericEdit.SetValue( sourceHeight );
   GUI->SourceHeightPixels_NumericEdit.Enable( currentView.IsNull() );

   GUI->TargetHeightPixels_NumericEdit.SetValue( h );
   GUI->TargetHeightCentimeters_NumericEdit.SetValue( hcm );
   GUI->TargetHeightInches_NumericEdit.SetValue( hin );

   String info;

   size_type wasArea = size_type( sourceWidth )*size_type( sourceHeight );
   size_type area = size_type( w )*size_type( h );

   if ( currentView.IsNull() )
      info.Format( "32-bit channel size: %.3lf MB, was %.3lf MB", (area*4)/1048576.0, (wasArea*4)/1048576.0 );
   else
   {
      ImageVariant image = currentView.Window().MainView().Image();

      size_type wasBytes = wasArea * image.NumberOfChannels() * image.BytesPerSample();
      size_type bytes = area * image.NumberOfChannels() * image.BytesPerSample();

      info.Format( "%d-bit total size: %.3lf MB, was %.3lf MB", image.BitsPerSample(), bytes/1048576.0, wasBytes/1048576.0 );
   }

   GUI->SizeInfo_Label.SetText( info );

   GUI->HorizontalResolution_NumericEdit.SetValue( instance.p_resolution.x );
   GUI->VerticalResolution_NumericEdit.SetValue( instance.p_resolution.y );

   GUI->CentimeterUnits_RadioButton.SetChecked( instance.p_metric );
   GUI->InchUnits_RadioButton.SetChecked( !instance.p_metric );

   GUI->ForceResolution_CheckBox.SetChecked( instance.p_forceResolution );
}

// ----------------------------------------------------------------------------

void IntegerResampleInterface::__ViewList_ViewSelected( ViewList& sender, View& view )
{
   DeactivateTrackView();

   if ( !currentView.IsNull() )
   {
      ImageWindow w = currentView.Window();

      w.MainView().GetSize( sourceWidth, sourceHeight );

      double xRes, yRes;
      bool metric;
      w.GetResolution( xRes, yRes, metric );

      instance.p_resolution.x = xRes;
      instance.p_resolution.y = yRes;
      instance.p_metric = metric;
   }

   UpdateControls();
}

void IntegerResampleInterface::__ResampleFactor_ValueUpdated( SpinBox& /*sender*/, int value )
{
   instance.p_zoomFactor = downsample ? -value : +value;
   UpdateControls();
}

void IntegerResampleInterface::__ResampleType_ButtonClick( Button& /*sender*/, bool checked )
{
   if ( checked )
      instance.p_zoomFactor = -instance.p_zoomFactor;
   UpdateControls();
}

void IntegerResampleInterface::__DownsampleMode_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   instance.p_downsampleMode = itemIndex;
}

void IntegerResampleInterface::__Width_ValueUpdated( NumericEdit& /*sender*/, double value )
{
   sourceWidth = int( value );
   UpdateControls();
}

void IntegerResampleInterface::__Height_ValueUpdated( NumericEdit& /*sender*/, double value )
{
   sourceHeight = int( value );
   UpdateControls();
}

void IntegerResampleInterface::__Resolution_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->HorizontalResolution_NumericEdit )
      instance.p_resolution.x = value;
   else if ( sender == GUI->VerticalResolution_NumericEdit )
      instance.p_resolution.y = value;
   UpdateControls();
}

void IntegerResampleInterface::__Units_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->CentimeterUnits_RadioButton )
      instance.p_metric = true;
   else if ( sender == GUI->InchUnits_RadioButton )
      instance.p_metric = false;
   UpdateControls();
}

void IntegerResampleInterface::__ForceResolution_ButtonClick( Button& sender, bool checked )
{
   instance.p_forceResolution = checked;
}

// ----------------------------------------------------------------------------

IntegerResampleInterface::GUIData::GUIData( IntegerResampleInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Height:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( "Horizontal:" ) + 'M' );
   int labelWidth3 = fnt.Width( String( "Resample Factor:" ) + 'M' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );

   // -------------------------------------------------------------------------

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&IntegerResampleInterface::__ViewList_ViewSelected, w );

   // -------------------------------------------------------------------------

   IntegerResample_SectionBar.SetTitle( "Integer Resample" );
   IntegerResample_SectionBar.SetSection( IntegerResample_Control );

   ResampleFactor_Label.SetText( "Resample Factor:" );
   ResampleFactor_Label.SetMinWidth( labelWidth3 );
   ResampleFactor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ResampleFactor_SpinBox.SetRange( 1, int( TheZoomFactorParameter->MaximumValue() ) );
   ResampleFactor_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&IntegerResampleInterface::__ResampleFactor_ValueUpdated, w );

   DownsampleMode_Label.SetText( "Downsample Mode:" );
   DownsampleMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DownsampleMode_ComboBox.AddItem( "Average" );
   DownsampleMode_ComboBox.AddItem( "Median" );
   DownsampleMode_ComboBox.AddItem( "Maximum" );
   DownsampleMode_ComboBox.AddItem( "Minimum" );
   DownsampleMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&IntegerResampleInterface::__DownsampleMode_ItemSelected, w );

   IntegerResampleRow1_Sizer.SetSpacing( 4 );
   IntegerResampleRow1_Sizer.Add( ResampleFactor_Label );
   IntegerResampleRow1_Sizer.Add( ResampleFactor_SpinBox );
   IntegerResampleRow1_Sizer.AddStretch();
   IntegerResampleRow1_Sizer.Add( DownsampleMode_Label );
   IntegerResampleRow1_Sizer.Add( DownsampleMode_ComboBox );

   Downsample_RadioButton.SetText( "Downsample" );
   Downsample_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::__ResampleType_ButtonClick, w );

   IntegerResampleRow2_Sizer.AddSpacing( labelWidth3 + 4 );
   IntegerResampleRow2_Sizer.Add( Downsample_RadioButton );

   Upsample_RadioButton.SetText( "Upsample" );
   Upsample_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::__ResampleType_ButtonClick, w );

   IntegerResampleRow3_Sizer.AddSpacing( labelWidth3 + 4 );
   IntegerResampleRow3_Sizer.Add( Upsample_RadioButton );

   IntegerResample_Sizer.Add( IntegerResampleRow1_Sizer );
   IntegerResample_Sizer.AddSpacing( 4 );
   IntegerResample_Sizer.Add( IntegerResampleRow2_Sizer );
   IntegerResample_Sizer.Add( IntegerResampleRow3_Sizer );

   IntegerResample_Control.SetSizer( IntegerResample_Sizer );

   // -------------------------------------------------------------------------

   Dimensions_SectionBar.SetTitle( "Dimensions" );
   Dimensions_SectionBar.SetSection( Dimensions_Control );

   SourcePixels_Label.SetText( "Original px" );
   SourcePixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   SourcePixels_Label.SetFixedWidth( editWidth1 );

   TargetPixels_Label.SetText( "Target px" );
   TargetPixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetPixels_Label.SetFixedWidth( editWidth1 );

   TargetCentimeters_Label.SetText( "cm" );
   TargetCentimeters_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetCentimeters_Label.SetFixedWidth( editWidth1 );

   TargetInches_Label.SetText( "inch" );
   TargetInches_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetInches_Label.SetFixedWidth( editWidth1 );

   DimensionsRow1_Sizer.SetSpacing( 6 );
   DimensionsRow1_Sizer.AddSpacing( labelWidth1 + 6 );
   DimensionsRow1_Sizer.Add( SourcePixels_Label );
   DimensionsRow1_Sizer.Add( TargetPixels_Label );
   DimensionsRow1_Sizer.Add( TargetCentimeters_Label );
   DimensionsRow1_Sizer.Add( TargetInches_Label );

   Width_Label.SetText( "Width:" );
   Width_Label.SetMinWidth( labelWidth1 );
   Width_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   SourceWidthPixels_NumericEdit.label.Hide();
   SourceWidthPixels_NumericEdit.SetInteger();
   SourceWidthPixels_NumericEdit.SetRange( 1, int_max );
   SourceWidthPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   SourceWidthPixels_NumericEdit.SetFixedWidth( editWidth1 );
   SourceWidthPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::__Width_ValueUpdated, w );

   TargetWidthPixels_NumericEdit.label.Hide();
   TargetWidthPixels_NumericEdit.SetInteger();
   TargetWidthPixels_NumericEdit.SetRange( 1, int_max );
   TargetWidthPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.Disable();

   TargetWidthCentimeters_NumericEdit.label.Hide();
   TargetWidthCentimeters_NumericEdit.SetReal();
   TargetWidthCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthCentimeters_NumericEdit.SetPrecision( 3 );
   TargetWidthCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.Disable();

   TargetWidthInches_NumericEdit.label.Hide();
   TargetWidthInches_NumericEdit.SetReal();
   TargetWidthInches_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthInches_NumericEdit.SetPrecision( 3 );
   TargetWidthInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.Disable();

   DimensionsRow2_Sizer.SetSpacing( 6 );
   DimensionsRow2_Sizer.Add( Width_Label );
   DimensionsRow2_Sizer.Add( SourceWidthPixels_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthPixels_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthCentimeters_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthInches_NumericEdit );

   Height_Label.SetText( "Height:" );
   Height_Label.SetMinWidth( labelWidth1 );
   Height_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   SourceHeightPixels_NumericEdit.label.Hide();
   SourceHeightPixels_NumericEdit.SetInteger();
   SourceHeightPixels_NumericEdit.SetRange( 1, int_max );
   SourceHeightPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   SourceHeightPixels_NumericEdit.SetFixedWidth( editWidth1 );
   SourceHeightPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::__Height_ValueUpdated, w );

   TargetHeightPixels_NumericEdit.label.Hide();
   TargetHeightPixels_NumericEdit.SetInteger();
   TargetHeightPixels_NumericEdit.SetRange( 1, int_max );
   TargetHeightPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.Disable();

   TargetHeightCentimeters_NumericEdit.label.Hide();
   TargetHeightCentimeters_NumericEdit.SetReal();
   TargetHeightCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightCentimeters_NumericEdit.SetPrecision( 3 );
   TargetHeightCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.Disable();

   TargetHeightInches_NumericEdit.label.Hide();
   TargetHeightInches_NumericEdit.SetReal();
   TargetHeightInches_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightInches_NumericEdit.SetPrecision( 3 );
   TargetHeightInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.Disable();

   DimensionsRow3_Sizer.SetSpacing( 6 );
   DimensionsRow3_Sizer.Add( Height_Label );
   DimensionsRow3_Sizer.Add( SourceHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightCentimeters_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightInches_NumericEdit );

   SizeInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   DimensionsRow4_Sizer.AddSpacing( labelWidth1 + 6 );
   DimensionsRow4_Sizer.Add( SizeInfo_Label, 100 );

   Dimensions_Sizer.SetSpacing( 4 );
   Dimensions_Sizer.Add( DimensionsRow1_Sizer );
   Dimensions_Sizer.Add( DimensionsRow2_Sizer );
   Dimensions_Sizer.Add( DimensionsRow3_Sizer );
   Dimensions_Sizer.Add( DimensionsRow4_Sizer );

   Dimensions_Control.SetSizer( Dimensions_Sizer );

   // -------------------------------------------------------------------------

   Resolution_SectionBar.SetTitle( "Resolution" );
   Resolution_SectionBar.SetSection( Resolution_Control );

   HorizontalResolution_NumericEdit.label.SetText( "Horizontal:" );
   HorizontalResolution_NumericEdit.label.SetFixedWidth( labelWidth2 );
   HorizontalResolution_NumericEdit.SetReal();
   HorizontalResolution_NumericEdit.SetRange( 1, 10000 );
   HorizontalResolution_NumericEdit.SetPrecision( 3 );
   HorizontalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::__Resolution_ValueUpdated, w );

   VerticalResolution_NumericEdit.label.SetText( "Vertical:" );
   VerticalResolution_NumericEdit.SetReal();
   VerticalResolution_NumericEdit.SetRange( 1, 10000 );
   VerticalResolution_NumericEdit.SetPrecision( 3 );
   VerticalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&IntegerResampleInterface::__Resolution_ValueUpdated, w );

   ResolutionRow1_Sizer.SetSpacing( 16 );
   ResolutionRow1_Sizer.Add( HorizontalResolution_NumericEdit );
   ResolutionRow1_Sizer.Add( VerticalResolution_NumericEdit );
   ResolutionRow1_Sizer.AddStretch();

   CentimeterUnits_RadioButton.SetText( "Centimeters" );
   CentimeterUnits_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::__Units_ButtonClick, w );

   InchUnits_RadioButton.SetText( "Inches" );
   InchUnits_RadioButton.OnClick( (Button::click_event_handler)&IntegerResampleInterface::__Units_ButtonClick, w );

   ForceResolution_CheckBox.SetText( "Force Resolution" );
   ForceResolution_CheckBox.SetToolTip( "Modify resolution of target image(s)" );
   ForceResolution_CheckBox.OnClick( (Button::click_event_handler)&IntegerResampleInterface::__ForceResolution_ButtonClick, w );

   ResolutionRow2_Sizer.SetSpacing( 8 );
   ResolutionRow2_Sizer.AddSpacing( labelWidth2 + 4 );
   ResolutionRow2_Sizer.Add( CentimeterUnits_RadioButton );
   ResolutionRow2_Sizer.Add( InchUnits_RadioButton );
   ResolutionRow2_Sizer.Add( ForceResolution_CheckBox );
   ResolutionRow2_Sizer.AddStretch();

   Resolution_Sizer.SetSpacing( 6 );
   Resolution_Sizer.Add( ResolutionRow1_Sizer );
   Resolution_Sizer.Add( ResolutionRow2_Sizer );

   Resolution_Control.SetSizer( Resolution_Sizer );

   // -------------------------------------------------------------------------

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( AllImages_ViewList );
   Global_Sizer.Add( IntegerResample_SectionBar );
   Global_Sizer.Add( IntegerResample_Control );
   Global_Sizer.Add( Dimensions_SectionBar );
   Global_Sizer.Add( Dimensions_Control );
   Global_Sizer.Add( Resolution_SectionBar );
   Global_Sizer.Add( Resolution_Control );

   Resolution_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF IntegerResampleInterface.cpp - Released 2014/10/29 07:34:55 UTC
