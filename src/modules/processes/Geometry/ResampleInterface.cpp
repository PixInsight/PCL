//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0346
// ----------------------------------------------------------------------------
// ResampleInterface.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "ResampleInterface.h"
#include "ResampleProcess.h"
#include "ResampleParameters.h"

#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ResampleInterface* TheResampleInterface = nullptr;

// ----------------------------------------------------------------------------

#include "ResampleIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView        GUI->AllImages_ViewList.CurrentView()
#define keepAspectRatio    GUI->PreserveAspectRatio_CheckBox.IsChecked()

// ----------------------------------------------------------------------------

ResampleInterface::ResampleInterface() :
   instance( TheResampleProcess )
{
   TheResampleInterface = this;
}

ResampleInterface::~ResampleInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString ResampleInterface::Id() const
{
   return "Resample";
}

MetaProcess* ResampleInterface::Process() const
{
   return TheResampleProcess;
}

const char** ResampleInterface::IconImageXPM() const
{
   return ResampleIcon_XPM;
}

InterfaceFeatures ResampleInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

void ResampleInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void ResampleInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.MainView() );
         else
            UpdateControls();
      }
}

void ResampleInterface::ResetInstance()
{
   ResampleInstance defaultInstance( TheResampleProcess );
   ImportProcess( defaultInstance );
}

bool ResampleInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Resample" );
      GUI->AllImages_ViewList.Regenerate( true, false ); // exclude previews
      UpdateControls();
   }

   dynamic = false;
   return &P == TheResampleProcess;
}

ProcessImplementation* ResampleInterface::NewProcess() const
{
   return new ResampleInstance( instance );
}

bool ResampleInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ResampleInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Resample instance.";
   return false;
}

bool ResampleInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ResampleInterface::ImportProcess( const ProcessImplementation& p )
{
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );

   instance.Assign( p );
   UpdateControls();
   return true;
}

bool ResampleInterface::WantsImageNotifications() const
{
   return true;
}

void ResampleInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
      {
         v.Window().MainView().GetSize( sourceWidth, sourceHeight );
         UpdateControls();
      }
}

void ResampleInterface::ImageFocused( const View& v )
{
   if ( !v.IsNull() )
      if ( GUI != nullptr )
         if ( IsTrackViewActive() )
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

void ResampleInterface::UpdateControls()
{
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
   GUI->TargetWidthPercent_NumericEdit.SetValue( 100.0*w/sourceWidth );
   GUI->TargetWidthCentimeters_NumericEdit.SetValue( wcm );
   GUI->TargetWidthInches_NumericEdit.SetValue( win );

   GUI->SourceHeightPixels_NumericEdit.SetValue( sourceHeight );
   GUI->SourceHeightPixels_NumericEdit.Enable( currentView.IsNull() );

   GUI->TargetHeightPixels_NumericEdit.SetValue( h );
   GUI->TargetHeightPercent_NumericEdit.SetValue( 100.0*h/sourceHeight );
   GUI->TargetHeightCentimeters_NumericEdit.SetValue( hcm );
   GUI->TargetHeightInches_NumericEdit.SetValue( hin );

   GUI->PreserveAspectRatio_CheckBox.Enable( instance.p_mode == int( RSMode::RelativeDimensions ) );

   if ( instance.p_mode != int( RSMode::RelativeDimensions ) )
      GUI->PreserveAspectRatio_CheckBox.SetChecked(
         instance.p_mode == int( RSMode::ForceArea ) ||
            instance.p_mode != int( RSMode::RelativeDimensions ) &&
            instance.p_absMode != int( RSAbsoluteMode::ForceWidthAndHeight ) );

   String info;
   size_type wasArea = size_type( sourceWidth )*size_type( sourceHeight );
   size_type area = size_type( w )*size_type( h );
   if ( currentView.IsNull() )
      info.Format( "32-bit channel size: %.3lf MiB, was %.3lf MiB", (area*4)/1048576.0, (wasArea*4)/1048576.0 );
   else
   {
      ImageVariant image = currentView.Window().MainView().Image();
      size_type wasBytes = wasArea * image.NumberOfChannels() * image.BytesPerSample();
      size_type bytes = area * image.NumberOfChannels() * image.BytesPerSample();
      info.Format( "%d-bit total size: %.3lf MiB, was %.3lf MiB", image.BitsPerSample(), bytes/1048576.0, wasBytes/1048576.0 );
   }

   GUI->SizeInfo_Label.SetText( info );

   GUI->HorizontalResolution_NumericEdit.SetValue( instance.p_resolution.x );
   GUI->VerticalResolution_NumericEdit.SetValue( instance.p_resolution.y );

   GUI->CentimeterUnits_RadioButton.SetChecked( instance.p_metric );
   GUI->InchUnits_RadioButton.SetChecked( !instance.p_metric );

   GUI->ForceResolution_CheckBox.SetChecked( instance.p_forceResolution );

   GUI->Algorithm_ComboBox.SetCurrentItem( instance.p_interpolation );

   GUI->ClampingThreshold_NumericControl.SetValue( instance.p_clampingThreshold );
   GUI->ClampingThreshold_NumericControl.Enable( InterpolationAlgorithm::IsClampedInterpolation( instance.p_interpolation ) );

   GUI->Smoothness_NumericControl.SetValue( instance.p_smoothness );
   GUI->Smoothness_NumericControl.Enable( InterpolationAlgorithm::IsCubicFilterInterpolation( instance.p_interpolation ) );

   GUI->ResampleMode_ComboBox.SetCurrentItem( instance.p_mode );

   GUI->AbsMode_ComboBox.SetCurrentItem( instance.p_absMode );
   GUI->AbsMode_ComboBox.Enable( instance.p_mode != int( RSMode::RelativeDimensions ) &&
                                 instance.p_mode != int( RSMode::ForceArea ) );
}

// ----------------------------------------------------------------------------

void ResampleInterface::__ViewList_ViewSelected( ViewList&, View& )
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

void ResampleInterface::__Width_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SourceWidthPixels_NumericEdit )
      sourceWidth = int( value );
   else
   {
      int px;

      if ( sender == GUI->TargetWidthPixels_NumericEdit )
         px = int( value );
      else if ( sender == GUI->TargetWidthPercent_NumericEdit )
         px = RoundI( (value/100)*sourceWidth );
      else if ( sender == GUI->TargetWidthCentimeters_NumericEdit )
      {
         double u = value;
         if ( !instance.p_metric )
            u /= 2.54;
         px = RoundI( u*instance.p_resolution.x );
      }
      else if ( sender == GUI->TargetWidthInches_NumericEdit )
      {
         double u = value;
         if ( instance.p_metric )
            u *= 2.54;
         px = RoundI( u*instance.p_resolution.x );
      }
      else
         return; // ??

      int py = Max( 1, RoundI( sourceHeight*double( px )/sourceWidth ) );

      px = Max( 1, px );

      switch ( instance.p_mode )
      {
      default:
      case RSMode::RelativeDimensions:
         instance.p_size.x = double( px )/sourceWidth;
         if ( keepAspectRatio )
            instance.p_size.y = instance.p_size.x;
         break;
      case RSMode::AbsolutePixels:
         instance.p_size.x = px;
         if ( keepAspectRatio )
            instance.p_size.y = py;
         break;
      case RSMode::AbsoluteCentimeters:
         instance.p_size.x = px/instance.p_resolution.x;
         if ( !instance.p_metric )
            instance.p_size.x *= 2.54;
         if ( keepAspectRatio )
         {
            instance.p_size.y = py/instance.p_resolution.y;
            if ( !instance.p_metric )
               instance.p_size.y *= 2.54;
         }
         break;
      case RSMode::AbsoluteInches:
         instance.p_size.x = px/instance.p_resolution.x;
         if ( instance.p_metric )
            instance.p_size.x /= 2.54;
         if ( keepAspectRatio )
         {
            instance.p_size.y = py/instance.p_resolution.y;
            if ( instance.p_metric )
               instance.p_size.y /= 2.54;
         }
         break;
      case RSMode::ForceArea:
         instance.p_size.x = size_type( px )*size_type( py ); // size.x is area in pixels; size.y not used
         break;
      }
   }

   UpdateControls();
}

void ResampleInterface::__Height_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SourceHeightPixels_NumericEdit )
      sourceHeight = int( value );
   else
   {
      int py;

      if ( sender == GUI->TargetHeightPixels_NumericEdit )
         py = int( value );
      else if ( sender == GUI->TargetHeightPercent_NumericEdit )
         py = RoundI( (value/100)*sourceHeight );
      else if ( sender == GUI->TargetHeightCentimeters_NumericEdit )
      {
         double u = value;
         if ( !instance.p_metric )
            u /= 2.54;
         py = RoundI( u*instance.p_resolution.y );
      }
      else if ( sender == GUI->TargetHeightInches_NumericEdit )
      {
         double u = value;
         if ( instance.p_metric )
            u *= 2.54;
         py = RoundI( u*instance.p_resolution.y );
      }
      else
         return; // ??

      int px = Max( 1, RoundI( sourceWidth*double( py )/sourceHeight ) );

      py = Max( 1, py );

      switch ( instance.p_mode )
      {
      default:
      case RSMode::RelativeDimensions:
         instance.p_size.y = double( py )/sourceHeight;
         if ( keepAspectRatio )
            instance.p_size.x = instance.p_size.y;
         break;
      case RSMode::AbsolutePixels:
         instance.p_size.y = py;
         if ( keepAspectRatio )
            instance.p_size.x = px;
         break;
      case RSMode::AbsoluteCentimeters:
         instance.p_size.y = py/instance.p_resolution.y;
         if ( !instance.p_metric )
            instance.p_size.y *= 2.54;
         if ( keepAspectRatio )
         {
            instance.p_size.x = px/instance.p_resolution.x;
            if ( !instance.p_metric )
               instance.p_size.x *= 2.54;
         }
         break;
      case RSMode::AbsoluteInches:
         instance.p_size.y = py/instance.p_resolution.y;
         if ( instance.p_metric )
            instance.p_size.y /= 2.54;
         if ( keepAspectRatio )
         {
            instance.p_size.x = px/instance.p_resolution.x;
            if ( instance.p_metric )
               instance.p_size.x /= 2.54;
         }
         break;
      case RSMode::ForceArea:
         instance.p_size.x = size_type( py )*size_type( px ); // size.x is area in pixels; size.y not used
         break;
      }
   }

   UpdateControls();
}

void ResampleInterface::__Resolution_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->HorizontalResolution_NumericEdit )
      instance.p_resolution.x = value;
   else if ( sender == GUI->VerticalResolution_NumericEdit )
      instance.p_resolution.y = value;
   UpdateControls();
}

void ResampleInterface::__Units_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->CentimeterUnits_RadioButton )
      instance.p_metric = true;
   else if ( sender == GUI->InchUnits_RadioButton )
      instance.p_metric = false;
   UpdateControls();
}

void ResampleInterface::__ForceResolution_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->ForceResolution_CheckBox  )
      instance.p_forceResolution = checked;
}

void ResampleInterface::__Algorithm_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Algorithm_ComboBox  )
   {
      instance.p_interpolation = itemIndex;
      GUI->ClampingThreshold_NumericControl.Enable( InterpolationAlgorithm::IsClampedInterpolation( instance.p_interpolation ) );
      GUI->Smoothness_NumericControl.Enable( InterpolationAlgorithm::IsCubicFilterInterpolation( instance.p_interpolation ) );
   }
}

void ResampleInterface::__Algorithm_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ClampingThreshold_NumericControl )
      instance.p_clampingThreshold = value;
   else if ( sender == GUI->Smoothness_NumericControl )
      instance.p_smoothness = value;
}

void ResampleInterface::__Mode_ItemSelected( ComboBox& sender, int itemIndex )
{
   int w = sourceWidth, h = sourceHeight;

   instance.GetNewSizes( w, h );

   if ( sender == GUI->ResampleMode_ComboBox )
      instance.p_mode = itemIndex;
   else if ( sender == GUI->AbsMode_ComboBox )
      instance.p_absMode = itemIndex;

   switch ( instance.p_mode )
   {
   default:
   case RSMode::RelativeDimensions:
      instance.p_size.x = double( w )/sourceWidth;
      instance.p_size.y = double( h )/sourceHeight;
      GUI->PreserveAspectRatio_CheckBox.SetChecked( instance.p_size.x == instance.p_size.y );
      break;

   case RSMode::AbsolutePixels:
      instance.p_size.x = w;
      instance.p_size.y = h;
      break;

   case RSMode::AbsoluteCentimeters:
      instance.p_size.x = w/instance.p_resolution.x;
      instance.p_size.y = h/instance.p_resolution.y;

      if ( !instance.p_metric )
      {
         instance.p_size.x *= 2.54;
         instance.p_size.y *= 2.54;
      }

      break;

   case RSMode::AbsoluteInches:
      instance.p_size.x = w/instance.p_resolution.x;
      instance.p_size.y = h/instance.p_resolution.y;

      if ( instance.p_metric )
      {
         instance.p_size.x /= 2.54;
         instance.p_size.y /= 2.54;
      }

      break;

   case RSMode::ForceArea:
      instance.p_size.x = size_type( w )*size_type( h );
      break;
   }

   UpdateControls();
}

void ResampleInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllImages_ViewList )
      wantsView = view.IsMainView();
}

void ResampleInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->AllImages_ViewList )
      if ( view.IsMainView() )
      {
         GUI->AllImages_ViewList.SelectView( view );
         View theView = view;
         __ViewList_ViewSelected( GUI->AllImages_ViewList, theView );
      }
}

// ----------------------------------------------------------------------------

ResampleInterface::GUIData::GUIData( ResampleInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Height:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( "Clamping threshold:" ) + 'M' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );
   int comboBoxWidth1 = fnt.Width( String( "Force area in pixels, keep aspect ratio" ) + String( 'M', 5 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );
   int ui6 = w.LogicalPixelsToPhysical( 6 );

   // -------------------------------------------------------------------------

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&ResampleInterface::__ViewList_ViewSelected, w );
   AllImages_ViewList.OnViewDrag( (Control::view_drag_event_handler)&ResampleInterface::__ViewDrag, w );
   AllImages_ViewList.OnViewDrop( (Control::view_drop_event_handler)&ResampleInterface::__ViewDrop, w );

   // -------------------------------------------------------------------------

   Dimensions_SectionBar.SetTitle( "Dimensions" );
   Dimensions_SectionBar.SetSection( Dimensions_Control );

   SourcePixels_Label.SetText( "Original px" );
   SourcePixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   TargetPixels_Label.SetText( "Target px" );
   TargetPixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   TargetPercent_Label.SetText( "%" );
   TargetPercent_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   TargetCentimeters_Label.SetText( "cm" );
   TargetCentimeters_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   TargetInches_Label.SetText( "inch" );
   TargetInches_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   DimensionsRow1_Sizer.SetSpacing( 6 );
   DimensionsRow1_Sizer.AddUnscaledSpacing( labelWidth1 + ui6 );
   DimensionsRow1_Sizer.Add( SourcePixels_Label );
   DimensionsRow1_Sizer.Add( TargetPixels_Label );
   DimensionsRow1_Sizer.Add( TargetPercent_Label );
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
   SourceWidthPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Width_ValueUpdated, w );

   TargetWidthPixels_NumericEdit.label.Hide();
   TargetWidthPixels_NumericEdit.SetInteger();
   TargetWidthPixels_NumericEdit.SetRange( 1, int_max );
   TargetWidthPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Width_ValueUpdated, w );

   TargetWidthPercent_NumericEdit.label.Hide();
   TargetWidthPercent_NumericEdit.SetReal();
   TargetWidthPercent_NumericEdit.SetRange( 0.0001, int_max );
   TargetWidthPercent_NumericEdit.SetPrecision( 4 );
   TargetWidthPercent_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthPercent_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthPercent_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Width_ValueUpdated, w );

   TargetWidthCentimeters_NumericEdit.label.Hide();
   TargetWidthCentimeters_NumericEdit.SetReal();
   TargetWidthCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthCentimeters_NumericEdit.SetPrecision( 3 );
   TargetWidthCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Width_ValueUpdated, w );

   TargetWidthInches_NumericEdit.label.Hide();
   TargetWidthInches_NumericEdit.SetReal();
   TargetWidthInches_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthInches_NumericEdit.SetPrecision( 3 );
   TargetWidthInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Width_ValueUpdated, w );

   DimensionsRow2_Sizer.SetSpacing( 6 );
   DimensionsRow2_Sizer.Add( Width_Label );
   DimensionsRow2_Sizer.Add( SourceWidthPixels_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthPixels_NumericEdit );
   DimensionsRow2_Sizer.Add( TargetWidthPercent_NumericEdit );
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
   SourceHeightPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Height_ValueUpdated, w );

   TargetHeightPixels_NumericEdit.label.Hide();
   TargetHeightPixels_NumericEdit.SetInteger();
   TargetHeightPixels_NumericEdit.SetRange( 1, int_max );
   TargetHeightPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Height_ValueUpdated, w );

   TargetHeightPercent_NumericEdit.label.Hide();
   TargetHeightPercent_NumericEdit.SetReal();
   TargetHeightPercent_NumericEdit.SetRange( 0.0001, int_max );
   TargetHeightPercent_NumericEdit.SetPrecision( 4 );
   TargetHeightPercent_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightPercent_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightPercent_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Height_ValueUpdated, w );

   TargetHeightCentimeters_NumericEdit.label.Hide();
   TargetHeightCentimeters_NumericEdit.SetReal();
   TargetHeightCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightCentimeters_NumericEdit.SetPrecision( 3 );
   TargetHeightCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Height_ValueUpdated, w );

   TargetHeightInches_NumericEdit.label.Hide();
   TargetHeightInches_NumericEdit.SetReal();
   TargetHeightInches_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightInches_NumericEdit.SetPrecision( 3 );
   TargetHeightInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Height_ValueUpdated, w );

   DimensionsRow3_Sizer.SetSpacing( 6 );
   DimensionsRow3_Sizer.Add( Height_Label );
   DimensionsRow3_Sizer.Add( SourceHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightPercent_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightCentimeters_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightInches_NumericEdit );

   PreserveAspectRatio_CheckBox.SetText( "Preserve aspect ratio" );
   PreserveAspectRatio_CheckBox.Check(); // default = on

   DimensionsRow4_Sizer.AddUnscaledSpacing( labelWidth1 + ui6 );
   DimensionsRow4_Sizer.Add( PreserveAspectRatio_CheckBox );
   DimensionsRow4_Sizer.AddStretch();

   SizeInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   DimensionsRow5_Sizer.AddUnscaledSpacing( labelWidth1 + ui6 );
   DimensionsRow5_Sizer.Add( SizeInfo_Label, 100 );

   Dimensions_Sizer.SetSpacing( 4 );
   Dimensions_Sizer.Add( DimensionsRow1_Sizer );
   Dimensions_Sizer.Add( DimensionsRow2_Sizer );
   Dimensions_Sizer.Add( DimensionsRow3_Sizer );
   Dimensions_Sizer.Add( DimensionsRow4_Sizer );
   Dimensions_Sizer.Add( DimensionsRow5_Sizer );

   Dimensions_Control.SetSizer( Dimensions_Sizer );

   // -------------------------------------------------------------------------

   Interpolation_SectionBar.SetTitle( "Interpolation" );
   Interpolation_SectionBar.SetSection( Interpolation_Control );

   Algorithm_Label.SetText( "Algorithm:" );
   Algorithm_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Algorithm_Label.SetFixedWidth( labelWidth2 );

   Algorithm_ComboBox.AddItem( "Nearest Neighbor" );
   Algorithm_ComboBox.AddItem( "Bilinear" );
   Algorithm_ComboBox.AddItem( "Bicubic Spline" );
   Algorithm_ComboBox.AddItem( "Bicubic B-Spline" );
   Algorithm_ComboBox.AddItem( "Lanczos-3" );
   Algorithm_ComboBox.AddItem( "Lanczos-4" );
   Algorithm_ComboBox.AddItem( "Mitchell-Netravali Cubic Filter" );
   Algorithm_ComboBox.AddItem( "Catmull-Rom Cubic Spline Filter" );
   Algorithm_ComboBox.AddItem( "Cubic B-Spline Filter" );
   Algorithm_ComboBox.AddItem( "Auto" );
   Algorithm_ComboBox.SetMaxVisibleItemCount( 16 );
   Algorithm_ComboBox.SetMinWidth( comboBoxWidth1 );
   Algorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ResampleInterface::__Algorithm_ItemSelected, w );

   Algorithm_Sizer.SetSpacing( 4 );
   Algorithm_Sizer.Add( Algorithm_Label );
   Algorithm_Sizer.Add( Algorithm_ComboBox );
   Algorithm_Sizer.AddStretch();

   ClampingThreshold_NumericControl.label.SetText( "Clamping threshold:" );
   ClampingThreshold_NumericControl.label.SetFixedWidth( labelWidth2 );
   ClampingThreshold_NumericControl.slider.SetRange( 0, 100 );
   ClampingThreshold_NumericControl.slider.SetScaledMinWidth( 250 );
   ClampingThreshold_NumericControl.SetReal();
   ClampingThreshold_NumericControl.SetRange( TheRSClampingThresholdParameter->MinimumValue(),
                                              TheRSClampingThresholdParameter->MaximumValue() );
   ClampingThreshold_NumericControl.SetPrecision( TheRSClampingThresholdParameter->Precision() );
   ClampingThreshold_NumericControl.SetToolTip( "<p>Deringing clamping threshold for bicubic spline and Lanczos interpolation algorithms.</p>" );
   ClampingThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Algorithm_ValueUpdated, w );

   Smoothness_NumericControl.label.SetText( "Smoothness:" );
   Smoothness_NumericControl.label.SetFixedWidth( labelWidth2 );
   Smoothness_NumericControl.slider.SetRange( 100, 500 );
   Smoothness_NumericControl.slider.SetScaledMinWidth( 250 );
   Smoothness_NumericControl.SetReal();
   Smoothness_NumericControl.SetRange( TheRSSmoothnessParameter->MinimumValue(),
                                       TheRSSmoothnessParameter->MaximumValue() );
   Smoothness_NumericControl.SetPrecision( TheRSSmoothnessParameter->Precision() );
   Smoothness_NumericControl.SetToolTip( "<p>Smoothness level for cubic filter interpolation algorithms.</p>" );
   Smoothness_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Algorithm_ValueUpdated, w );

   Interpolation_Sizer.SetSpacing( 4 );
   Interpolation_Sizer.Add( Algorithm_Sizer );
   Interpolation_Sizer.Add( ClampingThreshold_NumericControl );
   Interpolation_Sizer.Add( Smoothness_NumericControl );

   Interpolation_Control.SetSizer( Interpolation_Sizer );

   // -------------------------------------------------------------------------

   Resolution_SectionBar.SetTitle( "Resolution" );
   Resolution_SectionBar.SetSection( Resolution_Control );

   HorizontalResolution_NumericEdit.label.SetText( "Horizontal:" );
   HorizontalResolution_NumericEdit.label.SetFixedWidth( labelWidth2 );
   HorizontalResolution_NumericEdit.SetReal();
   HorizontalResolution_NumericEdit.SetRange( 1, 10000 );
   HorizontalResolution_NumericEdit.SetPrecision( 3 );
   HorizontalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Resolution_ValueUpdated, w );

   VerticalResolution_NumericEdit.label.SetText( "Vertical:" );
   VerticalResolution_NumericEdit.SetReal();
   VerticalResolution_NumericEdit.SetRange( 1, 10000 );
   VerticalResolution_NumericEdit.SetPrecision( 3 );
   VerticalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ResampleInterface::__Resolution_ValueUpdated, w );

   ResolutionRow1_Sizer.SetSpacing( 16 );
   ResolutionRow1_Sizer.Add( HorizontalResolution_NumericEdit );
   ResolutionRow1_Sizer.Add( VerticalResolution_NumericEdit );
   ResolutionRow1_Sizer.AddStretch();

   CentimeterUnits_RadioButton.SetText( "Centimeters" );
   CentimeterUnits_RadioButton.OnClick( (Button::click_event_handler)&ResampleInterface::__Units_ButtonClick, w );

   InchUnits_RadioButton.SetText( "Inches" );
   InchUnits_RadioButton.OnClick( (Button::click_event_handler)&ResampleInterface::__Units_ButtonClick, w );

   ForceResolution_CheckBox.SetText( "Force resolution" );
   ForceResolution_CheckBox.SetToolTip( "Modify resolution metadata of target image(s)" );
   ForceResolution_CheckBox.OnClick( (Button::click_event_handler)&ResampleInterface::__ForceResolution_ButtonClick, w );

   ResolutionRow2_Sizer.SetSpacing( 8 );
   ResolutionRow2_Sizer.AddUnscaledSpacing( labelWidth2 + ui4 );
   ResolutionRow2_Sizer.Add( CentimeterUnits_RadioButton );
   ResolutionRow2_Sizer.Add( InchUnits_RadioButton );
   ResolutionRow2_Sizer.Add( ForceResolution_CheckBox );
   ResolutionRow2_Sizer.AddStretch();

   Resolution_Sizer.SetSpacing( 6 );
   Resolution_Sizer.Add( ResolutionRow1_Sizer );
   Resolution_Sizer.Add( ResolutionRow2_Sizer );

   Resolution_Control.SetSizer( Resolution_Sizer );

   // -------------------------------------------------------------------------

   Mode_SectionBar.SetTitle( "Process Mode" );
   Mode_SectionBar.SetSection( Mode_Control );

   ResampleMode_Label.SetText( "Resample Mode:" );
   ResampleMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ResampleMode_Label.SetFixedWidth( labelWidth2 );

   ResampleMode_ComboBox.AddItem( "Relative resize" );
   ResampleMode_ComboBox.AddItem( "Absolute dimensions in pixels" );
   ResampleMode_ComboBox.AddItem( "Absolute dimensions in centimeters" );
   ResampleMode_ComboBox.AddItem( "Absolute dimensions in inches" );
   ResampleMode_ComboBox.AddItem( "Force area in pixels, keep aspect ratio" );
   ResampleMode_ComboBox.SetMinWidth( comboBoxWidth1 );
   ResampleMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ResampleInterface::__Mode_ItemSelected, w );

   ModeRow1_Sizer.SetSpacing( 4 );
   ModeRow1_Sizer.Add( ResampleMode_Label );
   ModeRow1_Sizer.Add( ResampleMode_ComboBox );
   ModeRow1_Sizer.AddStretch();

   AbsMode_Label.SetText( "Absolute Mode:" );
   AbsMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   AbsMode_Label.SetFixedWidth( labelWidth2 );

   AbsMode_ComboBox.AddItem( "Force width and height" );
   AbsMode_ComboBox.AddItem( "Force width, keep aspect ratio" );
   AbsMode_ComboBox.AddItem( "Force height, keep aspect ratio" );
   AbsMode_ComboBox.SetMinWidth( comboBoxWidth1 );
   AbsMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ResampleInterface::__Mode_ItemSelected, w );

   ModeRow2_Sizer.SetSpacing( 4 );
   ModeRow2_Sizer.Add( AbsMode_Label );
   ModeRow2_Sizer.Add( AbsMode_ComboBox );
   ModeRow2_Sizer.AddStretch();

   Mode_Sizer.SetSpacing( 4 );
   Mode_Sizer.Add( ModeRow1_Sizer );
   Mode_Sizer.Add( ModeRow2_Sizer );

   Mode_Control.SetSizer( Mode_Sizer );

   // -------------------------------------------------------------------------

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( AllImages_ViewList );
   Global_Sizer.Add( Dimensions_SectionBar );
   Global_Sizer.Add( Dimensions_Control );
   Global_Sizer.Add( Interpolation_SectionBar );
   Global_Sizer.Add( Interpolation_Control );
   Global_Sizer.Add( Resolution_SectionBar );
   Global_Sizer.Add( Resolution_Control );
   Global_Sizer.Add( Mode_SectionBar );
   Global_Sizer.Add( Mode_Control );

   Interpolation_Control.Hide();
   Resolution_Control.Hide();
   Mode_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ResampleInterface.cpp - Released 2017-05-02T09:43:00Z
