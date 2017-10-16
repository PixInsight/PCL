//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.02.0379
// ----------------------------------------------------------------------------
// CropInterface.cpp - Released 2017-10-16T10:07:46Z
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

#include "CropInterface.h"
#include "CropParameters.h"
#include "CropProcess.h"

#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CropInterface* TheCropInterface = nullptr;

// ----------------------------------------------------------------------------

#include "CropIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView  GUI->AllImages_ViewList.CurrentView()

// ----------------------------------------------------------------------------

CropInterface::CropInterface() :
   instance( TheCropProcess )
{
   TheCropInterface = this;
}

CropInterface::~CropInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString CropInterface::Id() const
{
   return "Crop";
}

MetaProcess* CropInterface::Process() const
{
   return TheCropProcess;
}

const char** CropInterface::IconImageXPM() const
{
   return CropIcon_XPM;
}

InterfaceFeatures CropInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

void CropInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void CropInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if (active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.MainView() );
         else
            UpdateControls();
      }
}

void CropInterface::ResetInstance()
{
   CropInstance defaultInstance( TheCropProcess );
   ImportProcess( defaultInstance );
}

bool CropInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Crop" );
      GUI->AllImages_ViewList.Regenerate( true, false ); // exclude previews
      UpdateControls();
   }

   dynamic = false;
   return &P == TheCropProcess;
}

ProcessImplementation* CropInterface::NewProcess() const
{
   return new CropInstance( instance );
}

bool CropInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const CropInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Crop instance.";
   return false;
}

bool CropInterface::RequiresInstanceValidation() const
{
   return true;
}

bool CropInterface::ImportProcess( const ProcessImplementation& p )
{
   DeactivateTrackView();
   GUI->AllImages_ViewList.SelectView( View::Null() );
   instance.Assign( p );
   UpdateControls();
   return true;
}

bool CropInterface::WantsImageNotifications() const
{
   return true;
}

void CropInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
      {
         v.Window().MainView().GetSize( sourceWidth, sourceHeight );
         UpdateControls();
      }
}

void CropInterface::ImageFocused( const View& v )
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

bool CropInterface::WantsReadoutNotifications() const
{
   return true;
}

void CropInterface::UpdateReadout( const View&, const DPoint&, double R, double G, double B, double /*A*/ )
{
   if ( GUI != nullptr )
      if ( IsVisible() )
         if ( GUI->FillColor_SectionBar.Section().IsVisible() )
         {
            instance.p_fillColor[0] = R;
            instance.p_fillColor[1] = G;
            instance.p_fillColor[2] = B;
            UpdateFillColorControls();
         }
}

// ----------------------------------------------------------------------------

void CropInterface::UpdateControls()
{
   UpdateNumericControls();
   UpdateAnchors();
   UpdateFillColorControls();
}

void CropInterface::UpdateAnchors()
{
   if ( instance.p_margins.y0 == 0 )
   {
      if ( instance.p_margins.y1 == 0 )
      {
         if ( instance.p_margins.x0 == 0 )
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 4;
            else
               anchor = 3;
         }
         else
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 5;
            else
               anchor = 4;
         }
      }
      else
      {
         if ( instance.p_margins.x0 == 0 )
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 1;
            else
               anchor = 0;
         }
         else
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 2;
            else
               anchor = 1;
         }
      }
   }
   else
   {
      if ( instance.p_margins.y1 == 0 )
      {
         if ( instance.p_margins.x0 == 0 )
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 7;
            else
               anchor = 6;
         }
         else
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 8;
            else
               anchor = 7;
         }
      }
      else
      {
         if ( instance.p_margins.x0 == 0 )
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 4;
            else
               anchor = 3;
         }
         else
         {
            if ( instance.p_margins.x1 == 0 )
               anchor = 5;
            else
               anchor = 4;
         }
      }
   }

   switch ( anchor )
   {
   case 0:
      GUI->TL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TM_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->R_Bitmap : GUI->L_Bitmap );
      GUI->TR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->ML_ToolButton.SetIcon( (instance.p_margins.y1 >= 0) ? GUI->B_Bitmap : GUI->T_Bitmap );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->BR_Bitmap : GUI->TL_Bitmap );
      GUI->MR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BR_ToolButton.SetIcon( Bitmap::Null() );
      break;
   case 1:
      GUI->TL_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->L_Bitmap : GUI->R_Bitmap );
      GUI->TM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TR_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->R_Bitmap : GUI->L_Bitmap );
      GUI->ML_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->BL_Bitmap : GUI->TR_Bitmap );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.y1 >= 0) ? GUI->B_Bitmap : GUI->T_Bitmap );
      GUI->MR_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->BR_Bitmap : GUI->TL_Bitmap );
      GUI->BL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BR_ToolButton.SetIcon( Bitmap::Null() );
      break;
   case 2:
      GUI->TL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TM_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->L_Bitmap : GUI->R_Bitmap );
      GUI->TR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->ML_ToolButton.SetIcon( Bitmap::Null() );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->BL_Bitmap : GUI->TR_Bitmap );
      GUI->MR_ToolButton.SetIcon( (instance.p_margins.y1 >= 0) ? GUI->B_Bitmap : GUI->T_Bitmap );
      GUI->BL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BR_ToolButton.SetIcon( Bitmap::Null() );
      break;
   case 3:
      GUI->TL_ToolButton.SetIcon( (instance.p_margins.y0 >= 0) ? GUI->T_Bitmap : GUI->B_Bitmap );
      GUI->TM_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->TR_Bitmap : GUI->BL_Bitmap );
      GUI->TR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->ML_ToolButton.SetIcon( Bitmap::Null() );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->R_Bitmap : GUI->L_Bitmap );
      GUI->MR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BL_ToolButton.SetIcon( (instance.p_margins.y1 >= 0) ? GUI->B_Bitmap : GUI->T_Bitmap );
      GUI->BM_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->BR_Bitmap : GUI->TL_Bitmap );
      GUI->BR_ToolButton.SetIcon( Bitmap::Null() );
      break;
   case 4:
      GUI->TL_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->TL_Bitmap : GUI->BR_Bitmap );
      GUI->TM_ToolButton.SetIcon( (instance.p_margins.y0 >= 0) ? GUI->T_Bitmap : GUI->B_Bitmap );
      GUI->TR_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->TR_Bitmap : GUI->BL_Bitmap );
      GUI->ML_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->L_Bitmap : GUI->R_Bitmap );
      GUI->MM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->MR_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->R_Bitmap : GUI->L_Bitmap );
      GUI->BL_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->BL_Bitmap : GUI->TR_Bitmap );
      GUI->BM_ToolButton.SetIcon( (instance.p_margins.y1 >= 0) ? GUI->B_Bitmap : GUI->T_Bitmap );
      GUI->BR_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->BR_Bitmap : GUI->TL_Bitmap );
      break;
   case 5:
      GUI->TL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TM_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->TL_Bitmap : GUI->BR_Bitmap );
      GUI->TR_ToolButton.SetIcon( (instance.p_margins.y0 >= 0) ? GUI->T_Bitmap : GUI->B_Bitmap );
      GUI->ML_ToolButton.SetIcon( Bitmap::Null() );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->L_Bitmap : GUI->R_Bitmap );
      GUI->MR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BM_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->BL_Bitmap : GUI->TR_Bitmap );
      GUI->BR_ToolButton.SetIcon( (instance.p_margins.y1 >= 0) ? GUI->B_Bitmap : GUI->T_Bitmap );
      break;
   case 6:
      GUI->TL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->ML_ToolButton.SetIcon( (instance.p_margins.y0 >= 0) ? GUI->T_Bitmap : GUI->B_Bitmap );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->TR_Bitmap : GUI->BL_Bitmap );
      GUI->MR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BM_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->R_Bitmap : GUI->L_Bitmap );
      GUI->BR_ToolButton.SetIcon( Bitmap::Null() );
      break;
   case 7:
      GUI->TL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->ML_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->TL_Bitmap : GUI->BR_Bitmap );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.y0 >= 0) ? GUI->T_Bitmap : GUI->B_Bitmap );
      GUI->MR_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->TR_Bitmap : GUI->BL_Bitmap );
      GUI->BL_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->L_Bitmap : GUI->R_Bitmap );
      GUI->BM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BR_ToolButton.SetIcon( (instance.p_margins.x1 >= 0) ? GUI->R_Bitmap : GUI->L_Bitmap );
      break;
   case 8:
      GUI->TL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TM_ToolButton.SetIcon( Bitmap::Null() );
      GUI->TR_ToolButton.SetIcon( Bitmap::Null() );
      GUI->ML_ToolButton.SetIcon( Bitmap::Null() );
      GUI->MM_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->TL_Bitmap : GUI->BR_Bitmap );
      GUI->MR_ToolButton.SetIcon( (instance.p_margins.y0 >= 0) ? GUI->T_Bitmap : GUI->B_Bitmap );
      GUI->BL_ToolButton.SetIcon( Bitmap::Null() );
      GUI->BM_ToolButton.SetIcon( (instance.p_margins.x0 >= 0) ? GUI->L_Bitmap : GUI->R_Bitmap );
      GUI->BR_ToolButton.SetIcon( Bitmap::Null() );
      break;
   }
}

void CropInterface::UpdateNumericControls()
{
   if ( instance.p_mode == CRMode::AbsolutePixels )
   {
      GUI->LeftMargin_NumericEdit.SetInteger();
      GUI->TopMargin_NumericEdit.SetInteger();
      GUI->RightMargin_NumericEdit.SetInteger();
      GUI->BottomMargin_NumericEdit.SetInteger();
   }
   else
   {
      GUI->LeftMargin_NumericEdit.SetReal();
      GUI->TopMargin_NumericEdit.SetReal();
      GUI->RightMargin_NumericEdit.SetReal();
      GUI->BottomMargin_NumericEdit.SetReal();

      int p = (instance.p_mode == CRMode::RelativeMargins) ? 6 : 4;
      GUI->LeftMargin_NumericEdit.SetPrecision( p );
      GUI->TopMargin_NumericEdit.SetPrecision( p );
      GUI->RightMargin_NumericEdit.SetPrecision( p );
      GUI->BottomMargin_NumericEdit.SetPrecision( p );
   }

   GUI->LeftMargin_NumericEdit.SetValue( instance.p_margins.x0 );
   GUI->TopMargin_NumericEdit.SetValue( instance.p_margins.y0 );
   GUI->RightMargin_NumericEdit.SetValue( instance.p_margins.x1 );
   GUI->BottomMargin_NumericEdit.SetValue( instance.p_margins.y1 );

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

   String info;

   size_type wasArea = size_type( sourceWidth )*size_type( sourceHeight );
   size_type area = size_type( w )*size_type( h );

   if ( area == 0 )
      info = "<* Invalid target dimensions *>";
   else if ( currentView.IsNull() )
      info.Format( "32-bit channel size: %.3f MB, was %.3f MB", (area*4)/1048576.0, (wasArea*4)/1048576.0 );
   else
   {
      ImageVariant image = currentView.Window().MainView().Image();

      size_type wasBytes = wasArea * image.NumberOfChannels() * image.BytesPerSample();
      size_type bytes = area * image.NumberOfChannels() * image.BytesPerSample();

      info.Format( "%d-bit total size: %.3f MB, was %.3f MB", image.BitsPerSample(), bytes/1048576.0, wasBytes/1048576.0 );
   }

   GUI->SizeInfo_Label.SetText( info );

   GUI->HorizontalResolution_NumericEdit.SetValue( instance.p_resolution.x );
   GUI->VerticalResolution_NumericEdit.SetValue( instance.p_resolution.y );

   GUI->CentimeterUnits_RadioButton.SetChecked( instance.p_metric );
   GUI->InchUnits_RadioButton.SetChecked( !instance.p_metric );

   GUI->ForceResolution_CheckBox.SetChecked( instance.p_forceResolution );

   GUI->CropMode_ComboBox.SetCurrentItem( instance.p_mode );
}

void CropInterface::UpdateFillColorControls()
{
   GUI->Red_NumericControl.SetValue( instance.p_fillColor[0] );
   GUI->Green_NumericControl.SetValue( instance.p_fillColor[1] );
   GUI->Blue_NumericControl.SetValue( instance.p_fillColor[2] );
   GUI->Alpha_NumericControl.SetValue( instance.p_fillColor[3] );
   GUI->ColorSample_Control.Update();
}

// ----------------------------------------------------------------------------

void CropInterface::__ViewList_ViewSelected( ViewList& sender, View& )
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

void CropInterface::__Margin_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->LeftMargin_NumericEdit )
      instance.p_margins.x0 = value;
   else if ( sender == GUI->TopMargin_NumericEdit )
      instance.p_margins.y0 = value;
   else if ( sender == GUI->RightMargin_NumericEdit )
      instance.p_margins.x1 = value;
   else if ( sender == GUI->BottomMargin_NumericEdit )
      instance.p_margins.y1 = value;

   UpdateNumericControls();
   UpdateAnchors();
}

void CropInterface::__Anchor_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->TL_ToolButton || sender == GUI->ML_ToolButton || sender == GUI->BL_ToolButton )
   {
      instance.p_margins.x1 += instance.p_margins.x0;
      instance.p_margins.x0 = 0;
   }

   if ( sender == GUI->TR_ToolButton || sender == GUI->MR_ToolButton || sender == GUI->BR_ToolButton )
   {
      instance.p_margins.x0 += instance.p_margins.x1;
      instance.p_margins.x1 = 0;
   }

   if ( sender == GUI->TM_ToolButton || sender == GUI->MM_ToolButton || sender == GUI->BM_ToolButton )
   {
      if ( instance.p_mode == CRMode::AbsolutePixels )
      {
         int px = int( instance.p_margins.x0 + instance.p_margins.x1 );
         instance.p_margins.x0 = px >> 1;
         instance.p_margins.x1 = px - (px >> 1);
      }
      else
         instance.p_margins.x0 = instance.p_margins.x1 = 0.5*(instance.p_margins.x0 + instance.p_margins.x1);
   }

   if ( sender == GUI->TL_ToolButton || sender == GUI->TM_ToolButton || sender == GUI->TR_ToolButton )
   {
      instance.p_margins.y1 += instance.p_margins.y0;
      instance.p_margins.y0 = 0;
   }

   if ( sender == GUI->BL_ToolButton || sender == GUI->BM_ToolButton || sender == GUI->BR_ToolButton )
   {
      instance.p_margins.y0 += instance.p_margins.y1;
      instance.p_margins.y1 = 0;
   }

   if ( sender == GUI->ML_ToolButton || sender == GUI->MM_ToolButton || sender == GUI->MR_ToolButton )
   {
      if ( instance.p_mode == CRMode::AbsolutePixels )
      {
         int px = int( instance.p_margins.y0 + instance.p_margins.y1 );
         instance.p_margins.y0 = px >> 1;
         instance.p_margins.y1 = px - (px >> 1);
      }
      else
         instance.p_margins.y0 = instance.p_margins.y1 = 0.5*(instance.p_margins.y0 + instance.p_margins.y1);
   }

   UpdateNumericControls();
   UpdateAnchors();
}

void CropInterface::__Width_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SourceWidthPixels_NumericEdit )
      sourceWidth = int( value );
   else
   {
      int dx;

      if ( sender == GUI->TargetWidthPixels_NumericEdit )
         dx = int( value );
      else if ( sender == GUI->TargetWidthPercent_NumericEdit )
         dx = RoundI( (value/100)*sourceWidth );
      else if ( sender == GUI->TargetWidthCentimeters_NumericEdit )
      {
         double u = value;
         if ( !instance.p_metric )
            u /= 2.54;
         dx = RoundI( u*instance.p_resolution.x );
      }
      else if ( sender == GUI->TargetWidthInches_NumericEdit )
      {
         double u = value;
         if ( instance.p_metric )
            u *= 2.54;
         dx = RoundI( u*instance.p_resolution.x );
      }
      else
         return; // ??

      dx -= sourceWidth;

      int dl = 0, dr = 0;

      switch ( anchor )
      {
      case 0:  // Left
      case 3:
      case 6:
         dl = 0;
         dr = dx;
         break;
      case 1:  // Middle
      case 4:
      case 7:
         dl = dx >> 1;
         dr = dx - dl;
         break;
      case 2:  // Right
      case 5:
      case 8:
         dl = dx;
         dr = 0;
         break;
      }

      switch ( instance.p_mode )
      {
      default:
      case CRMode::RelativeMargins:
         instance.p_margins.x0 = double( dl )/sourceWidth;
         instance.p_margins.x1 = double( dr )/sourceWidth;
         break;
      case CRMode::AbsolutePixels:
         instance.p_margins.x0 = dl;
         instance.p_margins.x1 = dr;
         break;
      case CRMode::AbsoluteCentimeters:
         instance.p_margins.x0 = dl/instance.p_resolution.x;
         instance.p_margins.x1 = dr/instance.p_resolution.x;
         if ( !instance.p_metric )
         {
            instance.p_margins.x0 *= 2.54;
            instance.p_margins.x1 *= 2.54;
         }
         break;
      case CRMode::AbsoluteInches:
         instance.p_margins.x0 = dl/instance.p_resolution.x;
         instance.p_margins.x1 = dr/instance.p_resolution.x;
         if ( instance.p_metric )
         {
            instance.p_margins.x0 /= 2.54;
            instance.p_margins.x1 /= 2.54;
         }
         break;
      }
   }

   UpdateNumericControls();
}

void CropInterface::__Height_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SourceHeightPixels_NumericEdit )
      sourceHeight = int( value );
   else
   {
      int dy;

      if ( sender == GUI->TargetHeightPixels_NumericEdit )
         dy = int( value );
      else if ( sender == GUI->TargetHeightPercent_NumericEdit )
         dy = RoundI( (value/100)*sourceHeight );
      else if ( sender == GUI->TargetHeightCentimeters_NumericEdit )
      {
         double u = value;
         if ( !instance.p_metric )
            u /= 2.54;
         dy = RoundI( u*instance.p_resolution.y );
      }
      else if ( sender == GUI->TargetHeightInches_NumericEdit )
      {
         double u = value;
         if ( instance.p_metric )
            u *= 2.54;
         dy = RoundI( u*instance.p_resolution.y );
      }
      else
         return; // ??

      dy -= sourceHeight;

      int dt = 0, db = 0;

      switch ( anchor )
      {
      case 0:  // Left
      case 3:
      case 6:
         dt = 0;
         db = dy;
         break;
      case 1:  // Middle
      case 4:
      case 7:
         dt = dy >> 1;
         db = dy - dt;
         break;
      case 2:  // Right
      case 5:
      case 8:
         dt = dy;
         db = 0;
         break;
      }

      switch ( instance.p_mode )
      {
      default:
      case CRMode::RelativeMargins:
         instance.p_margins.y0 = double( dt )/sourceHeight;
         instance.p_margins.y1 = double( db )/sourceHeight;
         break;
      case CRMode::AbsolutePixels:
         instance.p_margins.y0 = dt;
         instance.p_margins.y1 = db;
         break;
      case CRMode::AbsoluteCentimeters:
         instance.p_margins.y0 = dt/instance.p_resolution.y;
         instance.p_margins.y1 = db/instance.p_resolution.y;
         if ( !instance.p_metric )
         {
            instance.p_margins.y0 *= 2.54;
            instance.p_margins.y1 *= 2.54;
         }
         break;
      case CRMode::AbsoluteInches:
         instance.p_margins.y0 = dt/instance.p_resolution.y;
         instance.p_margins.y1 = db/instance.p_resolution.y;
         if ( instance.p_metric )
         {
            instance.p_margins.y0 /= 2.54;
            instance.p_margins.y1 /= 2.54;
         }
         break;
      }
   }

   UpdateNumericControls();
}

void CropInterface::__Resolution_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->HorizontalResolution_NumericEdit )
      instance.p_resolution.x = value;
   else if ( sender == GUI->VerticalResolution_NumericEdit )
      instance.p_resolution.y = value;
   UpdateNumericControls();
}

void CropInterface::__Units_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->CentimeterUnits_RadioButton )
      instance.p_metric = true;
   else if ( sender == GUI->InchUnits_RadioButton )
      instance.p_metric = false;
   UpdateNumericControls();
}

void CropInterface::__ForceResolution_ButtonClick( Button& /*sender*/, bool checked )
{
   instance.p_forceResolution = checked;
}

void CropInterface::__Mode_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   DRect dpx = instance.p_margins;

   switch ( instance.p_mode )
   {
   default:
   case CRMode::RelativeMargins:
      dpx.x0 *= sourceWidth;
      dpx.y0 *= sourceHeight;
      dpx.x1 *= sourceWidth;
      dpx.y1 *= sourceHeight;
      break;
   case CRMode::AbsolutePixels:
      break;
   case CRMode::AbsoluteCentimeters:
      dpx.x0 *= instance.p_resolution.x;
      dpx.y0 *= instance.p_resolution.y;
      dpx.x1 *= instance.p_resolution.x;
      dpx.y1 *= instance.p_resolution.y;
      if ( !instance.p_metric )
         dpx *= 2.54;
      break;
   case CRMode::AbsoluteInches:
      dpx.x0 *= instance.p_resolution.x;
      dpx.y0 *= instance.p_resolution.y;
      dpx.x1 *= instance.p_resolution.x;
      dpx.y1 *= instance.p_resolution.y;
      if ( instance.p_metric )
         dpx /= 2.54;
      break;
   }

   instance.p_mode = itemIndex;

   switch ( instance.p_mode )
   {
   default:
   case CRMode::RelativeMargins:
      instance.p_margins.x0 = dpx.x0/sourceWidth;
      instance.p_margins.y0 = dpx.y0/sourceHeight;
      instance.p_margins.x1 = dpx.x1/sourceWidth;
      instance.p_margins.y1 = dpx.y1/sourceHeight;
      break;
   case CRMode::AbsolutePixels:
      instance.p_margins.x0 = RoundI( dpx.x0 );
      instance.p_margins.y0 = RoundI( dpx.y0 );
      instance.p_margins.x1 = RoundI( dpx.x1 );
      instance.p_margins.y1 = RoundI( dpx.y1 );
      break;
   case CRMode::AbsoluteCentimeters:
      instance.p_margins.x0 = dpx.x0/instance.p_resolution.x;
      instance.p_margins.y0 = dpx.y0/instance.p_resolution.y;
      instance.p_margins.x1 = dpx.x1/instance.p_resolution.x;
      instance.p_margins.y1 = dpx.y1/instance.p_resolution.y;
      if ( !instance.p_metric )
         dpx *= 2.54;
      break;
   case CRMode::AbsoluteInches:
      instance.p_margins.x0 = dpx.x0/instance.p_resolution.x;
      instance.p_margins.y0 = dpx.y0/instance.p_resolution.y;
      instance.p_margins.x1 = dpx.x1/instance.p_resolution.x;
      instance.p_margins.y1 = dpx.y1/instance.p_resolution.y;
      if ( instance.p_metric )
         dpx /= 2.54;
      break;
   }

   UpdateNumericControls();
}

void CropInterface::__FilColor_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Red_NumericControl )
      instance.p_fillColor[0] = value;
   else if ( sender == GUI->Green_NumericControl )
      instance.p_fillColor[1] = value;
   else if ( sender == GUI->Blue_NumericControl )
      instance.p_fillColor[2] = value;
   else if ( sender == GUI->Alpha_NumericControl )
      instance.p_fillColor[3] = value;

   GUI->ColorSample_Control.Update();
}

void CropInterface::__ColorSample_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );

   RGBA color = RGBAColor( float( instance.p_fillColor[0] ),
                           float( instance.p_fillColor[1] ),
                           float( instance.p_fillColor[2] ),
                           float( instance.p_fillColor[3] ) );

   if ( Alpha( color ) != 0 )
   {
      g.SetBrush( Bitmap( sender.ScaledResource( ":/image-window/transparent-small.png" ) ) );
      g.SetPen( Pen::Null() );
      g.DrawRect( sender.BoundsRect() );
   }

   g.SetBrush( color );
   g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
   g.DrawRect( sender.BoundsRect() );
}

void CropInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllImages_ViewList )
      wantsView = view.IsMainView();
}

void CropInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
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

CropInterface::GUIData::GUIData( CropInterface& w ) :
   L_Bitmap(  w.ScaledResource( ":/icons/move-left.png" ) ),
   R_Bitmap(  w.ScaledResource( ":/icons/move-right.png" ) ),
   T_Bitmap(  w.ScaledResource( ":/icons/move-up.png" ) ),
   B_Bitmap(  w.ScaledResource( ":/icons/move-down.png" ) ),
   TL_Bitmap( w.ScaledResource( ":/icons/move-left-up.png" ) ),
   TR_Bitmap( w.ScaledResource( ":/icons/move-right-up.png" ) ),
   BL_Bitmap( w.ScaledResource( ":/icons/move-left-down.png" ) ),
   BR_Bitmap( w.ScaledResource( ":/icons/move-right-down.png" ) )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Height:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( "Crop Mode:" ) + 'M' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );
   int ui6 = w.LogicalPixelsToPhysical( 6 );

   // -------------------------------------------------------------------------

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&CropInterface::__ViewList_ViewSelected, w );
   AllImages_ViewList.OnViewDrag( (Control::view_drag_event_handler)&CropInterface::__ViewDrag, w );
   AllImages_ViewList.OnViewDrop( (Control::view_drop_event_handler)&CropInterface::__ViewDrop, w );

   // -------------------------------------------------------------------------

   CropMargins_SectionBar.SetTitle( "Margins/Anchors" );
   CropMargins_SectionBar.SetSection( CropMargins_Control );

   TopMargin_NumericEdit.label.Hide();
   TopMargin_NumericEdit.SetReal();
   TopMargin_NumericEdit.SetRange( int_min, int_max );
   TopMargin_NumericEdit.SetPrecision( 6 );
   TopMargin_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TopMargin_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Margin_ValueUpdated, w );

   CropMarginsTop_Sizer.AddStretch();
   CropMarginsTop_Sizer.Add( TopMargin_NumericEdit );
   CropMarginsTop_Sizer.AddStretch();

   LeftMargin_NumericEdit.label.Hide();
   LeftMargin_NumericEdit.SetReal();
   LeftMargin_NumericEdit.SetRange( int_min, int_max );
   LeftMargin_NumericEdit.SetPrecision( 6 );
   LeftMargin_NumericEdit.edit.SetFixedWidth( editWidth1 );
   LeftMargin_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Margin_ValueUpdated, w );

   TL_ToolButton.SetScaledFixedSize( 20, 20 );
   TL_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   TM_ToolButton.SetScaledFixedSize( 20, 20 );
   TM_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   TR_ToolButton.SetScaledFixedSize( 20, 20 );
   TR_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   AnchorRow1_Sizer.SetSpacing( 2 );
   AnchorRow1_Sizer.Add( TL_ToolButton );
   AnchorRow1_Sizer.Add( TM_ToolButton );
   AnchorRow1_Sizer.Add( TR_ToolButton );

   ML_ToolButton.SetScaledFixedSize( 20, 20 );
   ML_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   MM_ToolButton.SetScaledFixedSize( 20, 20 );
   MM_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   MR_ToolButton.SetScaledFixedSize( 20, 20 );
   MR_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   AnchorRow2_Sizer.SetSpacing( 2 );
   AnchorRow2_Sizer.Add( ML_ToolButton );
   AnchorRow2_Sizer.Add( MM_ToolButton );
   AnchorRow2_Sizer.Add( MR_ToolButton );

   BL_ToolButton.SetScaledFixedSize( 20, 20 );
   BL_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   BM_ToolButton.SetScaledFixedSize( 20, 20 );
   BM_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   BR_ToolButton.SetScaledFixedSize( 20, 20 );
   BR_ToolButton.OnClick( (Button::click_event_handler)&CropInterface::__Anchor_ButtonClick, w );

   AnchorRow3_Sizer.SetSpacing( 2 );
   AnchorRow3_Sizer.Add( BL_ToolButton );
   AnchorRow3_Sizer.Add( BM_ToolButton );
   AnchorRow3_Sizer.Add( BR_ToolButton );

   AnchorButtons_Sizer.SetSpacing( 2 );
   AnchorButtons_Sizer.Add( AnchorRow1_Sizer );
   AnchorButtons_Sizer.Add( AnchorRow2_Sizer );
   AnchorButtons_Sizer.Add( AnchorRow3_Sizer );

   RightMargin_NumericEdit.label.Hide();
   RightMargin_NumericEdit.SetReal();
   RightMargin_NumericEdit.SetRange( int_min, int_max );
   RightMargin_NumericEdit.SetPrecision( 6 );
   RightMargin_NumericEdit.edit.SetFixedWidth( editWidth1 );
   RightMargin_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Margin_ValueUpdated, w );

   CropMarginsMiddle_Sizer.SetSpacing( 8 );
   CropMarginsMiddle_Sizer.AddStretch();
   CropMarginsMiddle_Sizer.Add( LeftMargin_NumericEdit );
   CropMarginsMiddle_Sizer.Add( AnchorButtons_Sizer );
   CropMarginsMiddle_Sizer.Add( RightMargin_NumericEdit );
   CropMarginsMiddle_Sizer.AddStretch();

   BottomMargin_NumericEdit.label.Hide();
   BottomMargin_NumericEdit.SetReal();
   BottomMargin_NumericEdit.SetRange( int_min, int_max );
   BottomMargin_NumericEdit.SetPrecision( 6 );
   BottomMargin_NumericEdit.edit.SetFixedWidth( editWidth1 );
   BottomMargin_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Margin_ValueUpdated, w );

   CropMarginsBottom_Sizer.AddStretch();
   CropMarginsBottom_Sizer.Add( BottomMargin_NumericEdit );
   CropMarginsBottom_Sizer.AddStretch();

   CropMargins_Sizer.SetSpacing( 8 );
   CropMargins_Sizer.Add( CropMarginsTop_Sizer );
   CropMargins_Sizer.Add( CropMarginsMiddle_Sizer );
   CropMargins_Sizer.Add( CropMarginsBottom_Sizer );

   CropMargins_Control.SetSizer( CropMargins_Sizer );

   // -------------------------------------------------------------------------

   Dimensions_SectionBar.SetTitle( "Dimensions" );
   Dimensions_SectionBar.SetSection( Dimensions_Control );

   SourcePixels_Label.SetText( "Original px" );
   SourcePixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   SourcePixels_Label.SetFixedWidth( editWidth1 );

   TargetPixels_Label.SetText( "Target px" );
   TargetPixels_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetPixels_Label.SetFixedWidth( editWidth1 );

   TargetPercent_Label.SetText( "%" );
   TargetPercent_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetPercent_Label.SetFixedWidth( editWidth1 );

   TargetCentimeters_Label.SetText( "cm" );
   TargetCentimeters_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetCentimeters_Label.SetFixedWidth( editWidth1 );

   TargetInches_Label.SetText( "inch" );
   TargetInches_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   TargetInches_Label.SetFixedWidth( editWidth1 );

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
   SourceWidthPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Width_ValueUpdated, w );

   TargetWidthPixels_NumericEdit.label.Hide();
   TargetWidthPixels_NumericEdit.SetInteger();
   TargetWidthPixels_NumericEdit.SetRange( 1, int_max );
   TargetWidthPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Width_ValueUpdated, w );

   TargetWidthPercent_NumericEdit.label.Hide();
   TargetWidthPercent_NumericEdit.SetReal();
   TargetWidthPercent_NumericEdit.SetRange( 0.0001, int_max );
   TargetWidthPercent_NumericEdit.SetPrecision( 4 );
   TargetWidthPercent_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthPercent_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthPercent_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Width_ValueUpdated, w );

   TargetWidthCentimeters_NumericEdit.label.Hide();
   TargetWidthCentimeters_NumericEdit.SetReal();
   TargetWidthCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthCentimeters_NumericEdit.SetPrecision( 3 );
   TargetWidthCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthCentimeters_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Width_ValueUpdated, w );

   TargetWidthInches_NumericEdit.label.Hide();
   TargetWidthInches_NumericEdit.SetReal();
   TargetWidthInches_NumericEdit.SetRange( 0.001, int_max );
   TargetWidthInches_NumericEdit.SetPrecision( 3 );
   TargetWidthInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetWidthInches_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Width_ValueUpdated, w );

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
   SourceHeightPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Height_ValueUpdated, w );

   TargetHeightPixels_NumericEdit.label.Hide();
   TargetHeightPixels_NumericEdit.SetInteger();
   TargetHeightPixels_NumericEdit.SetRange( 1, int_max );
   TargetHeightPixels_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightPixels_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Height_ValueUpdated, w );

   TargetHeightPercent_NumericEdit.label.Hide();
   TargetHeightPercent_NumericEdit.SetReal();
   TargetHeightPercent_NumericEdit.SetRange( 0.0001, int_max );
   TargetHeightPercent_NumericEdit.SetPrecision( 4 );
   TargetHeightPercent_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightPercent_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightPercent_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Height_ValueUpdated, w );

   TargetHeightCentimeters_NumericEdit.label.Hide();
   TargetHeightCentimeters_NumericEdit.SetReal();
   TargetHeightCentimeters_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightCentimeters_NumericEdit.SetPrecision( 3 );
   TargetHeightCentimeters_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightCentimeters_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Height_ValueUpdated, w );

   TargetHeightInches_NumericEdit.label.Hide();
   TargetHeightInches_NumericEdit.SetReal();
   TargetHeightInches_NumericEdit.SetRange( 0.001, int_max );
   TargetHeightInches_NumericEdit.SetPrecision( 3 );
   TargetHeightInches_NumericEdit.edit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.SetFixedWidth( editWidth1 );
   TargetHeightInches_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Height_ValueUpdated, w );

   DimensionsRow3_Sizer.SetSpacing( 6 );
   DimensionsRow3_Sizer.Add( Height_Label );
   DimensionsRow3_Sizer.Add( SourceHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightPixels_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightPercent_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightCentimeters_NumericEdit );
   DimensionsRow3_Sizer.Add( TargetHeightInches_NumericEdit );

   SizeInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   DimensionsRow4_Sizer.AddUnscaledSpacing( labelWidth1 + ui6 );
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
   HorizontalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Resolution_ValueUpdated, w );

   VerticalResolution_NumericEdit.label.SetText( "Vertical:" );
   VerticalResolution_NumericEdit.SetReal();
   VerticalResolution_NumericEdit.SetRange( 1, 10000 );
   VerticalResolution_NumericEdit.SetPrecision( 3 );
   VerticalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__Resolution_ValueUpdated, w );

   ResolutionRow1_Sizer.SetSpacing( 16 );
   ResolutionRow1_Sizer.Add( HorizontalResolution_NumericEdit );
   ResolutionRow1_Sizer.Add( VerticalResolution_NumericEdit );
   ResolutionRow1_Sizer.AddStretch();

   CentimeterUnits_RadioButton.SetText( "Centimeters" );
   CentimeterUnits_RadioButton.OnClick( (Button::click_event_handler)&CropInterface::__Units_ButtonClick, w );

   InchUnits_RadioButton.SetText( "Inches" );
   InchUnits_RadioButton.OnClick( (Button::click_event_handler)&CropInterface::__Units_ButtonClick, w );

   ForceResolution_CheckBox.SetText( "Force resolution" );
   ForceResolution_CheckBox.SetToolTip( "Modify resolution metadata of target image(s)" );
   ForceResolution_CheckBox.OnClick( (Button::click_event_handler)&CropInterface::__ForceResolution_ButtonClick, w );

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

   CropMode_Label.SetText( "Crop Mode:" );
   CropMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CropMode_Label.SetFixedWidth( labelWidth2 );

   CropMode_ComboBox.AddItem( "Relative margins" );
   CropMode_ComboBox.AddItem( "Absolute margins in pixels" );
   CropMode_ComboBox.AddItem( "Absolute margins in centimeters" );
   CropMode_ComboBox.AddItem( "Absolute margins in inches" );
   CropMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&CropInterface::__Mode_ItemSelected, w );

   Mode_Sizer.SetSpacing( 4 );
   Mode_Sizer.Add( CropMode_Label );
   Mode_Sizer.Add( CropMode_ComboBox );
   Mode_Sizer.AddStretch();

   Mode_Control.SetSizer( Mode_Sizer );

   // -------------------------------------------------------------------------

   FillColor_SectionBar.SetTitle( "Fill Color" );
   FillColor_SectionBar.SetSection( FillColor_Control );

   Red_NumericControl.label.SetText( "R:" );
   Red_NumericControl.label.SetFixedWidth( labelWidth2 );
   Red_NumericControl.slider.SetRange( 0, 100 );
   Red_NumericControl.SetReal();
   Red_NumericControl.SetRange( 0, 1 );
   Red_NumericControl.SetPrecision( 6 );
   Red_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__FilColor_ValueUpdated, w );

   Green_NumericControl.label.SetText( "G:" );
   Green_NumericControl.label.SetFixedWidth( labelWidth2 );
   Green_NumericControl.slider.SetRange( 0, 100 );
   Green_NumericControl.SetReal();
   Green_NumericControl.SetRange( 0, 1 );
   Green_NumericControl.SetPrecision( 6 );
   Green_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__FilColor_ValueUpdated, w );

   Blue_NumericControl.label.SetText( "B:" );
   Blue_NumericControl.label.SetFixedWidth( labelWidth2 );
   Blue_NumericControl.slider.SetRange( 0, 100 );
   Blue_NumericControl.SetReal();
   Blue_NumericControl.SetRange( 0, 1 );
   Blue_NumericControl.SetPrecision( 6 );
   Blue_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__FilColor_ValueUpdated, w );

   Alpha_NumericControl.label.SetText( "A:" );
   Alpha_NumericControl.label.SetFixedWidth( labelWidth2 );
   Alpha_NumericControl.slider.SetRange( 0, 100 );
   Alpha_NumericControl.SetReal();
   Alpha_NumericControl.SetRange( 0, 1 );
   Alpha_NumericControl.SetPrecision( 6 );
   Alpha_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CropInterface::__FilColor_ValueUpdated, w );

   ColorSample_Control.SetScaledFixedHeight( 20 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&CropInterface::__ColorSample_Paint, w );

   FillColor_Sizer.SetSpacing( 4 );
   FillColor_Sizer.Add( Red_NumericControl );
   FillColor_Sizer.Add( Green_NumericControl );
   FillColor_Sizer.Add( Blue_NumericControl );
   FillColor_Sizer.Add( Alpha_NumericControl );
   FillColor_Sizer.Add( ColorSample_Control );

   FillColor_Control.SetSizer( FillColor_Sizer );

   // -------------------------------------------------------------------------

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( AllImages_ViewList );
   Global_Sizer.Add( CropMargins_SectionBar );
   Global_Sizer.Add( CropMargins_Control );
   Global_Sizer.Add( Dimensions_SectionBar );
   Global_Sizer.Add( Dimensions_Control );
   Global_Sizer.Add( Resolution_SectionBar );
   Global_Sizer.Add( Resolution_Control );
   Global_Sizer.Add( Mode_SectionBar );
   Global_Sizer.Add( Mode_Control );
   Global_Sizer.Add( FillColor_SectionBar );
   Global_Sizer.Add( FillColor_Control );

   w.AdjustToContents();

   Resolution_Control.Hide();
   Mode_Control.Hide();
   FillColor_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropInterface.cpp - Released 2017-10-16T10:07:46Z
