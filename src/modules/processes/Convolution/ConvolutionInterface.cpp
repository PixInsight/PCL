//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0265
// ----------------------------------------------------------------------------
// ConvolutionInterface.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#include "ConvolutionDialog.h"
#include "ConvolutionInterface.h"
#include "ConvolutionParameters.h"
#include "ConvolutionProcess.h"
#include "FilterLibrary.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>
#include <pcl/Settings.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ConvolutionInterface* TheConvolutionInterface = nullptr;

// ----------------------------------------------------------------------------

#include "ConvolutionIcon.xpm"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static bool CanAddFilter( const Filter& f )
{
   if ( TheFilterLibrary.FilterByName( f.Name() ) == nullptr )
      return true;
   if ( !TheFilterLibrary.IsWritable() )
      return true;

   return MessageBox( "<p>You are about to make permanent changes to the \"" + f.Name() + "\" filter.</p>"
                      "<p>Changes will be written to the current filter library on disk.</p>"
                      "<p><b>Proceed?</b></p>",
                      "Convolution",
                      StdIcon::Warning,
                      StdButton::Yes, StdButton::No ).Execute() == StdButton::Yes;
}

static bool CanRemoveFilter( const Filter& f )
{
   if ( TheFilterLibrary.FilterByName( f.Name() ) == nullptr )
      return true;
   if ( !TheFilterLibrary.IsWritable() )
      return true;

   return MessageBox( "<p>You are about to remove the \"" + f.Name() + "\" filter permanently.</p>"
                      "<p>Changes will be written to the current filter library on disk.</p>"
                      "<p><b>Proceed?</b></p>",
                      "Convolution",
                      StdIcon::Warning,
                      StdButton::Yes, StdButton::No ).Execute() == StdButton::Yes;
}

static bool CanSaveTheLibrary()
{
   if ( TheFilterLibrary.IsWritable() )
      return true;

   MessageBox( "<p>The current filter library is not writable:</p>"
               "<p>" + TheFilterLibrary.FilePath() + "</p>"
               "<p>Changes will only be in effect during the current session and won't be stored "
               "permanently until you save the current library to a different disk file.</p>",
               "Convolution",
               StdIcon::Information,
               StdButton::Ok ).Execute();

   return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConvolutionInterface::ConvolutionInterface() :
   instance( TheConvolutionProcess )
{
   TheConvolutionInterface = this;
}

// ----------------------------------------------------------------------------

ConvolutionInterface::~ConvolutionInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ConvolutionInterface::Id() const
{
   return "Convolution";
}

// ----------------------------------------------------------------------------

MetaProcess* ConvolutionInterface::Process() const
{
   return TheConvolutionProcess;
}

// ----------------------------------------------------------------------------

const char** ConvolutionInterface::IconImageXPM() const
{
   return ConvolutionIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures ConvolutionInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::ResetInstance()
{
   ConvolutionInstance defaultInstance( TheConvolutionProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the R-T preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

bool ConvolutionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      static bool firstTimeLaunch = true;
      if ( firstTimeLaunch )
      {
         String libraryPath = FilterLibrary::DefaultLibraryPath();
         Settings::Read( SettingsKey() + "FilterLibraryPath", libraryPath );
         if ( libraryPath.IsEmpty() || !File::Exists( libraryPath ) )
            TheFilterLibrary.LoadDefaultLibrary();
         else
            TheFilterLibrary.Load( libraryPath );
         firstTimeLaunch = false;
      }

      GUI = new GUIData( *this );
      SetWindowTitle( "Convolution" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheConvolutionProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvolutionInterface::NewProcess() const
{
   return new ConvolutionInstance( instance );
}

// ----------------------------------------------------------------------------

bool ConvolutionInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const ConvolutionInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Convolution instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool ConvolutionInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ConvolutionInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   UpdateRealTimePreview();
   return true;
}

// ----------------------------------------------------------------------------

bool ConvolutionInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const
{
   return true;
}

// ----------------------------------------------------------------------------

ConvolutionInterface::RealTimeThread::RealTimeThread() :
Thread(), m_instance( TheConvolutionProcess )
{
}

void ConvolutionInterface::RealTimeThread::Reset( const UInt16Image& image, const ConvolutionInstance& instance, int zoomLevel )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
   m_zoomLevel = zoomLevel;
}

void ConvolutionInterface::RealTimeThread::Run()
{
   MuteStatus status;
   m_image.SetStatusCallback( &status );
   m_instance.Convolve( m_image, m_zoomLevel );
   m_image.ResetSelections();
}

bool ConvolutionInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int zoomLevel, String& ) const
{
   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      m_realTimeThread->Reset( image, instance, zoomLevel );
      m_realTimeThread->Start();

      while ( m_realTimeThread->IsActive() )
      {
         ProcessEvents();

         if ( !IsRealTimePreviewActive() )
         {
            m_realTimeThread->Abort();
            m_realTimeThread->Wait();

            delete m_realTimeThread;
            m_realTimeThread = nullptr;
            return false;
         }
      }

      if ( !m_realTimeThread->IsAborted() )
      {
         image.Assign( m_realTimeThread->m_image );

         delete m_realTimeThread;
         m_realTimeThread = nullptr;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "FilterLibraryPath", TheFilterLibrary.FilePath() );
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::LoadSettings()
{
   // Placeholder
}

// ----------------------------------------------------------------------------

bool ConvolutionInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( instance.mode == ConMode::Image )
         if ( v.FullId() == IsoString( instance.viewId ) )
            UpdateImageFilterControls();
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::ImageRenamed( const View& v )
{
   if ( GUI != nullptr )
      if ( instance.mode == ConMode::Image )
         UpdateImageFilterControls();
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::ImageDeleted( const View& v )
{
   if ( GUI != nullptr )
      if ( instance.mode == ConMode::Image )
         if ( v.FullId() == IsoString( instance.viewId ) )
            UpdateImageFilterControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ConvolutionInterface::UpdateControls()
{
   GUI->FilterMode_TabBox.SelectPage( int( instance.mode ) );
   UpdateParametricFilterControls();
   UpdateLibraryFilterControls();
   UpdateImageFilterControls();
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::UpdateCurrentModeControls()
{
   switch ( instance.mode )
   {
   case ConMode::Parametric: UpdateParametricFilterControls(); break;
   case ConMode::Library:    UpdateLibraryFilterControls(); break;
   case ConMode::Image:      UpdateImageFilterControls(); break;
   }
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::UpdateParametricFilterControls()
{
   // We use two slider to define Sigma. The fine slider is used if Sigma is under
   // 10. The coarse slider is used for the rest of values.
   if ( instance.sigma <= 10 )
   {
      // We apply this 10 factor to Sigma in order to match the value with the
      // range of the slider. The call to RoundI() is mandatory since sigma
      // must be an integer in order to be applied to the slider.
      GUI->SigmaFine_Slider.SetValue( RoundI( 10*instance.sigma ) );

      // If we use the fine slider the value of the Sigma Coarse Slider must be
      // set to the minimum value (10).
      GUI->SigmaCoarse_Slider.SetValue( 10 );
   }
   else
   {
      GUI->SigmaCoarse_Slider.SetValue( instance.sigma );

      // If we use the coarse slider, the value of the Sigma Fine Slider must be
      // set to the maximum value (100 which means StdDev = 10)
      GUI->SigmaFine_Slider.SetValue( 100 );
   }

   GUI->Sigma_NumericEdit.SetValue( instance.sigma );
   GUI->Shape_NumericControl.SetValue( instance.shape );
   GUI->AspectRatio_NumericControl.SetValue( instance.aspectRatio );
   GUI->RotationAngle_NumericControl.SetValue( instance.rotationAngle );
   GUI->RotationAngle_NumericControl.Enable( instance.aspectRatio < 1 );

   GUI->FilterThumbnail_Control.Update();
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::UpdateLibraryFilterControls()
{
   GUI->LibraryPath_Edit.SetText( TheFilterLibrary.FilePath() );

   Filter filter;
   try
   {
      filter = Filter::FromSource( instance.sourceCode );
   }
   catch ( ... )
   {
   }

   if ( !filter.IsValid() || GUI->Filter_ComboBox.FindItem( filter.Name() ) < 0 )
   {
      GUI->Filter_ComboBox.SetCurrentItem( 0 );
      GUI->EditFilter_PushButton.Disable();
      GUI->ViewFilterElements_PushButton.Disable();
      GUI->RemoveFilter_PushButton.Disable();
      GUI->RescaleHighPass_CheckBox.Disable();
   }
   else
   {
      GUI->Filter_ComboBox.SetCurrentItem( GUI->Filter_ComboBox.FindItem( filter.Name() ) );
      GUI->EditFilter_PushButton.Enable();
      GUI->ViewFilterElements_PushButton.Enable();
      GUI->RemoveFilter_PushButton.Enable();
      GUI->RescaleHighPass_CheckBox.Enable( filter.IsHighPass() );
   }

   GUI->RescaleHighPass_CheckBox.SetChecked( instance.rescaleHighPass );

   GUI->FilterThumbnail_Control.Update();
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::UpdateImageFilterControls()
{
   GUI->View_Edit.SetText( instance.viewId );
   GUI->FilterThumbnail_Control.Update();
}

// ----------------------------------------------------------------------------

void ConvolutionInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != nullptr )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ConvolutionInterface::__Filter_PageSelected( TabBox& sender, int pageIndex )
{
   instance.mode = pageIndex;

   UpdateControls();
   UpdateRealTimePreview();
}

void ConvolutionInterface::__Filter_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Sigma_NumericEdit )
      instance.sigma = Max( TheConSigmaParameter->MinimumValue(), value );
   else if ( sender == GUI->Shape_NumericControl )
      instance.shape = Max( TheConShapeParameter->MinimumValue(), value );
   else if ( sender == GUI->AspectRatio_NumericControl )
      instance.aspectRatio = value;
   else if ( sender == GUI->RotationAngle_NumericControl )
      instance.rotationAngle = value;

   UpdateParametricFilterControls();
   UpdateRealTimePreview();
}

void ConvolutionInterface::__Filter_SliderUpdated( Slider& sender, int value )
{
   if ( sender == GUI->SigmaCoarse_Slider )
      instance.sigma = value;
   else if ( sender == GUI->SigmaFine_Slider )
      instance.sigma = 0.1 * value; // We apply this factor because the slider
                                    // range is from 0 to 100 instead of 0 to 10.
   UpdateParametricFilterControls();
   UpdateRealTimePreview();
}

void ConvolutionInterface::__Filter_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );
   Image f;
   if ( instance.CreateFilterImage( f ) )
   {
      int z = (sender.Width() >= f.Width()) ? sender.Width()/f.Width() : -f.Width()/sender.Width();
      g.DisableSmoothInterpolation();
      g.DrawScaledBitmap( sender.BoundsRect(), Bitmap::Render( ImageVariant( &f ), z ) );
      GUI->FilterInfo_Label.SetText( String().Format( "%d x %d", f.Width(), f.Height() ) );
   }
   else
   {
      g.FillRect( sender.BoundsRect(), 0xff000000 );
      GUI->FilterInfo_Label.SetText( "<* Not found *>" );
   }
}

void ConvolutionInterface::__Library_Click( Button& sender, bool checked )
{
   if ( sender == GUI->LibraryPath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.SetCaption( "Open Filter Library" );
      if ( d.Execute() )
      {
         try
         {
            TheFilterLibrary.Load( d.FileName() );
            RegenerateFiltersComboBox( 0 );
            UpdateLibraryFilterControls();
         }
         ERROR_HANDLER
      }
   }
   else if ( sender == GUI->SaveAsLibrary_PushButton )
   {
      SaveFileDialog d;
      d.SetInitialPath( TheFilterLibrary.FilePath() );
      d.EnableOverwritePrompt();
      d.SetCaption( "Save Filter Library" );
      if ( d.Execute() )
      {
         TheFilterLibrary.SaveAs( d.FileName() );
         UpdateLibraryFilterControls();
      }
   }
   else if ( sender == GUI->NewLibrary_PushButton )
   {
      SaveFileDialog d;
      d.SetInitialPath( TheFilterLibrary.FilePath() );
      d.EnableOverwritePrompt();
      d.SetCaption( "Create New Filter Library" );
      if ( d.Execute() )
      {
         TheFilterLibrary.New();
         TheFilterLibrary.SaveAs( d.FileName() );
         RegenerateFiltersComboBox( 0 );
         UpdateLibraryFilterControls();
      }
   }
   else if ( sender == GUI->DefaultLibrary_PushButton )
   {
      TheFilterLibrary.LoadDefaultLibrary();
      RegenerateFiltersComboBox( 0 );
      UpdateLibraryFilterControls();
   }
   else if ( sender == GUI->RescaleHighPass_CheckBox )
   {
      instance.rescaleHighPass = checked;
      UpdateRealTimePreview();
   }
   else if ( sender == GUI->RenderFilter_ToolButton )
   {
      ImageWindow window( 1, 1, 1, 32, true/*floatSample*/, false/*color*/ );
      ImageVariant image = window.MainView().Image();
      instance.CreateFilterImage( static_cast<Image&>( *image ) );
      window.Show();
      window.ZoomToFit( false/*allowMagnification*/ );
   }
   else if ( sender == GUI->ViewFilterElements_PushButton )
   {
      ConvolutionFilterCodeDialog d( "View", Filter::FromSource( instance.sourceCode ) );
      d.Execute();
   }
   else if ( sender == GUI->EditFilter_PushButton )
   {
      Filter f = Filter::FromSource( instance.sourceCode );
      ConvolutionFilterCodeDialog d( "Edit", f );
      if ( d.Execute() )
         if ( f != d.BeingEdited() )
            if ( CanAddFilter( d.BeingEdited() ) )
            {
               TheFilterLibrary.Add( d.BeingEdited() );
               if ( CanSaveTheLibrary() )
                  TheFilterLibrary.Save();
               RegenerateFiltersComboBox( d.BeingEdited().Name() ); // implicitly updates instance.sourceCode
            }
   }
   else if ( sender == GUI->NewFilter_PushButton )
   {
      ConvolutionFilterCodeDialog d( "New", Filter() );
      if ( d.Execute() )
         if ( CanAddFilter( d.BeingEdited() ) )
         {
            TheFilterLibrary.Add( d.BeingEdited() );
            if ( CanSaveTheLibrary() )
               TheFilterLibrary.Save();
            RegenerateFiltersComboBox( d.BeingEdited().Name() ); // implicitly updates instance.sourceCode
         }
   }
   else if ( sender == GUI->RemoveFilter_PushButton )
   {
      Filter f = Filter::FromSource( instance.sourceCode );
      if ( CanRemoveFilter( f ) )
         if ( TheFilterLibrary.Remove( f ) )
         {
            if ( CanSaveTheLibrary() )
               TheFilterLibrary.Save();
            RegenerateFiltersComboBox( 0 );
            UpdateLibraryFilterControls();
            instance.sourceCode.Clear();
            UpdateRealTimePreview();
         }
   }
}

void ConvolutionInterface::__Library_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Filter_ComboBox )
   {
      if ( itemIndex != 0 ) // 0 is the 'Pick a Filter' fake item
      {
         instance.sourceCode = TheFilterLibrary[itemIndex-1].ToSource();
         GUI->FilterThumbnail_Control.Update();
         UpdateRealTimePreview();
      }
      UpdateLibraryFilterControls();
   }
}

void ConvolutionInterface::RegenerateFiltersComboBox( int selectedItemIndex )
{
   GUI->Filter_ComboBox.Clear();
   GUI->Filter_ComboBox.AddItem( "<-- Pick a Filter -->" );
   for ( size_type i = 0; i < TheFilterLibrary.Length(); ++i )
      GUI->Filter_ComboBox.AddItem( TheFilterLibrary[i].Name() );

   __Library_ItemSelected( GUI->Filter_ComboBox,
                           Range( selectedItemIndex, 0, GUI->Filter_ComboBox.NumberOfItems()-1 ) );
}

void ConvolutionInterface::RegenerateFiltersComboBox( const String& selectedFilterName )
{
   GUI->Filter_ComboBox.Clear();
   GUI->Filter_ComboBox.AddItem( "<-- Pick a Filter -->" );
   for ( size_type i = 0; i < TheFilterLibrary.Length(); ++i )
      GUI->Filter_ComboBox.AddItem( TheFilterLibrary[i].Name() );

   int index = GUI->Filter_ComboBox.FindItem( selectedFilterName );
   __Library_ItemSelected( GUI->Filter_ComboBox, Max( 0, index ) );
}

void ConvolutionInterface::__Image_Click( Button& sender, bool checked )
{
   if ( sender == GUI->View_ToolButton )
   {
      ViewSelectionDialog d( instance.viewId );
      d.SetWindowTitle( "Select Filter Image" );
      if ( d.Execute() )
      {
         instance.viewId = d.Id();
         UpdateRealTimePreview();
      }
      UpdateImageFilterControls();
   }
}

void ConvolutionInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->View_Edit || sender == GUI->FilterMode_TabBox )
      wantsView = true;
}

void ConvolutionInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->View_Edit || sender == GUI->FilterMode_TabBox )
   {
      instance.mode = ConMode::Image;
      instance.viewId = view.FullId();
      UpdateControls();
      UpdateRealTimePreview();
   }
}

void ConvolutionInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != nullptr )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConvolutionInterface::GUIData::GUIData( ConvolutionInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "External image:" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 7 ) );
   int editHeight = CanonicalControlHeight( Edit );

   // ### Parametric TAB Elements

   // Standard Deviation NumericEdit
   Sigma_NumericEdit.label.SetText( "StdDev:" );
   Sigma_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Sigma_NumericEdit.SetReal();
   Sigma_NumericEdit.SetRange( TheConSigmaParameter->MinimumValue(), TheConSigmaParameter->MaximumValue() );
   Sigma_NumericEdit.SetPrecision( TheConSigmaParameter->Precision() );
   Sigma_NumericEdit.SetToolTip( "<p>Standard deviation of the parametric low-pass filter in pixels.</p>"
      "By increasing this parameter the parametric filter will be larger, and hence the convolution process will act "
      "at larger dimensional scales.</p>" );
   Sigma_NumericEdit.edit.SetFixedWidth( editWidth1 );
   Sigma_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ConvolutionInterface::__Filter_ValueUpdated, w );

   // Standard Deviation Coarse Slider
   SigmaCoarse_Slider.SetScaledMinWidth( 250 );
   SigmaCoarse_Slider.SetRange( 10, 250 );
   SigmaCoarse_Slider.SetFixedHeight( RoundInt( 0.75*editHeight ) );
   SigmaCoarse_Slider.SetToolTip( "<p>Standard deviation, coarse adjustment.</p>" );
   SigmaCoarse_Slider.OnValueUpdated( (Slider::value_event_handler)&ConvolutionInterface::__Filter_SliderUpdated, w );

   // Standard Deviation Fine Slider
   SigmaFine_Slider.SetScaledMinWidth( 250 );
   SigmaFine_Slider.SetRange( 1, 100 );
   SigmaFine_Slider.SetFixedHeight( RoundInt( 0.75*editHeight ) );
   SigmaFine_Slider.SetToolTip( "<p>Standard deviation, fine adjustment.</p>" );
   SigmaFine_Slider.OnValueUpdated( (Slider::value_event_handler)&ConvolutionInterface::__Filter_SliderUpdated, w );

   // Standard Deviation Slider's vertical sizer
   SigmaSliders_Sizer.SetSpacing( 2 );
   SigmaSliders_Sizer.Add( SigmaCoarse_Slider );
   SigmaSliders_Sizer.Add( SigmaFine_Slider );

   // Standard Deviation horizontal sizer
   Sigma_Sizer.SetSpacing( 4 );
   Sigma_Sizer.Add( Sigma_NumericEdit );
   Sigma_Sizer.Add( SigmaSliders_Sizer, 100 );

   // Shape NumericControl
   Shape_NumericControl.label.SetText( "Shape:" );
   Shape_NumericControl.label.SetFixedWidth( labelWidth1 );
   Shape_NumericControl.slider.SetScaledMinWidth( 250 );
   Shape_NumericControl.slider.SetRange( 0, 120 );
   Shape_NumericControl.SetReal();
   // ### We use a trick to achieve the correct slider stepping (see also the event handler)
   Shape_NumericControl.SetRange( 0 /*TheConShapeParameter->MinimumValue()*/, TheConShapeParameter->MaximumValue() );
   Shape_NumericControl.SetPrecision( TheConShapeParameter->Precision() );
   Shape_NumericControl.edit.SetFixedWidth( editWidth1 );
   Shape_NumericControl.sizer.AddSpacing( 8 );
   Shape_NumericControl.SetToolTip( "<p>Shape of the filter function.</p>"
      "<p>This parameter controls the <i>kurtosis</i> of the filter function distribution. When shape = 2 the filter "
      "distribution is Gaussian (also known as <i>normal distribution</i>). When shape &lt; 2 the distribution is "
      "<i>leptokurtic</i>, leading to a peaked filter that can be used, for example, to approximate the shape of a "
      "diffraction pattern. When shape &gt; 2 the distribution is <i>platykurtic</i> and the filter has a comparatively "
      "flat profile, resembling a truncated (or saturated) PSF.</p>" );
   Shape_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ConvolutionInterface::__Filter_ValueUpdated, w );

   // Parametric Ration NumericControl
   AspectRatio_NumericControl.label.SetText( "Aspect ratio:" );
   AspectRatio_NumericControl.label.SetFixedWidth( labelWidth1 );
   AspectRatio_NumericControl.slider.SetScaledMinWidth( 250 );
   AspectRatio_NumericControl.slider.SetRange( 0, 100 );
   AspectRatio_NumericControl.SetReal();
   AspectRatio_NumericControl.SetRange( TheConAspectRatioParameter->MinimumValue(), TheConAspectRatioParameter->MaximumValue() );
   AspectRatio_NumericControl.SetPrecision( TheConAspectRatioParameter->Precision() );
   AspectRatio_NumericControl.edit.SetFixedWidth( editWidth1 );
   AspectRatio_NumericControl.sizer.AddSpacing( 8 );
   AspectRatio_NumericControl.SetToolTip( "<p>Aspect ratio of the filter function (vertical/horizontal axis ratio).</p>" );
   AspectRatio_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ConvolutionInterface::__Filter_ValueUpdated, w );

   // Parametric Angle NumericControl
   RotationAngle_NumericControl.label.SetText( "Rotation:" );
   RotationAngle_NumericControl.label.SetFixedWidth( labelWidth1 );
   RotationAngle_NumericControl.slider.SetScaledMinWidth( 250 );
   RotationAngle_NumericControl.slider.SetRange( 0, 180 );
   RotationAngle_NumericControl.SetReal();
   RotationAngle_NumericControl.SetRange( TheConRotationAngleParameter->MinimumValue(), TheConRotationAngleParameter->MaximumValue() );
   RotationAngle_NumericControl.SetPrecision( TheConRotationAngleParameter->Precision() );
   RotationAngle_NumericControl.edit.SetFixedWidth( editWidth1 );
   RotationAngle_NumericControl.sizer.AddSpacing( 8 );
   RotationAngle_NumericControl.SetToolTip( "<p>Rotation angle of the distorted filter function in degrees.</p>" );
   RotationAngle_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ConvolutionInterface::__Filter_ValueUpdated, w );

   // Parametric parameter's vertical Sizer
   Parametric_Sizer.SetSpacing( 4 );
   Parametric_Sizer.SetMargin( 6 );
   Parametric_Sizer.Add( Sigma_Sizer );
   Parametric_Sizer.Add( Shape_NumericControl );
   Parametric_Sizer.Add( AspectRatio_NumericControl );
   Parametric_Sizer.Add( RotationAngle_NumericControl );

   // Parametric TAB Control set to Parametric_Sizer
   Parametric_Control.SetSizer( Parametric_Sizer );

   //
   LibraryPath_Label.SetText( "Library file:" );
   LibraryPath_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   LibraryPath_Label.SetToolTip( "<p>File path of the current filter library.</p>" );

   LibraryPath_Edit.SetReadOnly();
   LibraryPath_Edit.SetToolTip( "<p>File path of the current filter library.</p>" );

   LibraryPath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   LibraryPath_ToolButton.SetToolTip( "<p>Load a filter library.</p>" );
   LibraryPath_ToolButton.SetScaledFixedSize( 20, 20 );
   LibraryPath_ToolButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   LibraryPath_Sizer.SetSpacing( 4 );
   LibraryPath_Sizer.Add( LibraryPath_Edit, 100 );
   LibraryPath_Sizer.Add( LibraryPath_ToolButton );

   SaveAsLibrary_PushButton.SetText( "Save as" );
   SaveAsLibrary_PushButton.SetToolTip( "<p>Save the current filter library to a newly created file.</p>" );
   SaveAsLibrary_PushButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   NewLibrary_PushButton.SetText( "New" );
   NewLibrary_PushButton.SetToolTip( "<p>Create an empty filter library.</p>" );
   NewLibrary_PushButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   DefaultLibrary_PushButton.SetText( "Default" );
   DefaultLibrary_PushButton.SetToolTip( "<p>Load the default filter library.</p>" );
   DefaultLibrary_PushButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   LibraryButtons_Sizer.SetSpacing( 8 );
   LibraryButtons_Sizer.Add( SaveAsLibrary_PushButton );
   LibraryButtons_Sizer.Add( NewLibrary_PushButton );
   LibraryButtons_Sizer.Add( DefaultLibrary_PushButton );
   LibraryButtons_Sizer.AddStretch();

   Filter_ComboBox.AddItem( "<-- Pick a Filter -->" );
   for ( size_type i = 0; i < TheFilterLibrary.Length(); ++i )
      Filter_ComboBox.AddItem( TheFilterLibrary[i].Name() );
   Filter_ComboBox.SetToolTip( "<p>Select a filter from the current filter library.</p>" );
   Filter_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ConvolutionInterface::__Library_ItemSelected, w );

   RescaleHighPass_CheckBox.SetText( "High-pass rescaling" );
   RescaleHighPass_CheckBox.SetToolTip( "<p>Rescale out-of-range values for normalization of images after "
      "convolution with a high-pass filter.</p>"
      "<p>A high-pass filter has negative coefficients. As a result, some pixels in the convolved image may have "
      "negative values. Saturated pixels (values above one) can also result, depending on the filter coefficients. "
      "The standard behavior is to truncate out-of-range pixel values to the [0,1] range, which preserves the "
      "dynamics of the convolved image, so high-pass rescaling is disabled by default. When high-pass rescaling is "
      "enabled, the resulting image is <i>normalized</i> (that is, rescaled to [0,1] only if there are out-of-range "
      "values) and hence all the data after convolution are preserved at the cost of reducing the overall contrast "
      "of the image.</p>" );
   RescaleHighPass_CheckBox.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   ViewFilterElements_PushButton.SetText( "Coefficients" );
   ViewFilterElements_PushButton.SetToolTip( "<p>View the kernel filter coefficients for the currently selected filter.</p>" );
   ViewFilterElements_PushButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   EditFilter_PushButton.SetText( "Edit" );
   EditFilter_PushButton.SetToolTip( "<p>Edit the currently selected filter.</p>"
      "<p>To create a new filter based on the current one, change the filter name.</p>"
      "<p>Changes will be permanently saved to the current library file.</p>" );
   EditFilter_PushButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   NewFilter_PushButton.SetText( "New" );
   NewFilter_PushButton.SetToolTip( "<p>Create a new filter from scratch.</p>"
      "<p>The new filter will be added to the current library file.</p>" );
   NewFilter_PushButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   RemoveFilter_PushButton.SetText( "Remove" );
   RemoveFilter_PushButton.SetToolTip( "<p>Remove the currently selected filter</p>"
      "<p>The filter will be permanently removed from the current filter library file.</p>" );
   RemoveFilter_PushButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   FilterButtons_Sizer.SetSpacing( 8 );
   FilterButtons_Sizer.Add( ViewFilterElements_PushButton );
   FilterButtons_Sizer.Add( EditFilter_PushButton );
   FilterButtons_Sizer.Add( NewFilter_PushButton );
   FilterButtons_Sizer.Add( RemoveFilter_PushButton );
   FilterButtons_Sizer.AddStretch();

   Library_Sizer.SetSpacing( 6 );
   Library_Sizer.SetMargin( 6 );
   Library_Sizer.Add( LibraryPath_Label );
   Library_Sizer.Add( LibraryPath_Sizer );
   Library_Sizer.Add( LibraryButtons_Sizer );
   Library_Sizer.AddSpacing( 6 );
   Library_Sizer.Add( Filter_ComboBox );
   Library_Sizer.Add( RescaleHighPass_CheckBox );
   Library_Sizer.Add( FilterButtons_Sizer );
   Library_Sizer.AddStretch();

   // ### Library Tab
   Library_Control.SetSizer( Library_Sizer );

   View_Label.SetText( "Filter image:" );
   View_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   View_Label.SetToolTip( "<p>Identifier of a view (main view or preview) to be used as a convolution filter.</p>" );

   View_Edit.SetReadOnly();
   View_Edit.SetToolTip( "<p>Identifier of a view (main view or preview) to be used as a convolution filter.</p>" );
   View_Edit.OnViewDrag( (Control::view_drag_event_handler)&ConvolutionInterface::__ViewDrag, w );
   View_Edit.OnViewDrop( (Control::view_drop_event_handler)&ConvolutionInterface::__ViewDrop, w );

   View_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   View_ToolButton.SetScaledFixedSize( 20, 20 );
   View_ToolButton.SetToolTip( "<p>Select view.</p>" );
   View_ToolButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Image_Click, w );

   View_Sizer.SetSpacing( 4 );
   View_Sizer.Add( View_Edit, 100 );
   View_Sizer.Add( View_ToolButton );

   Image_Sizer.SetSpacing( 4 );
   Image_Sizer.SetMargin( 6 );
   Image_Sizer.AddStretch();
   Image_Sizer.Add( View_Label );
   Image_Sizer.Add( View_Sizer );
   Image_Sizer.AddStretch();

   // ### External Image Tab
   Image_Control.SetSizer( Image_Sizer );

   FilterMode_TabBox.AddPage( Parametric_Control, "Parametric" );
   FilterMode_TabBox.AddPage( Library_Control, "Library" );
   FilterMode_TabBox.AddPage( Image_Control, "Image" );
   FilterMode_TabBox.OnPageSelected( (TabBox::page_event_handler)&ConvolutionInterface::__Filter_PageSelected, w );
   FilterMode_TabBox.OnViewDrag( (Control::view_drag_event_handler)&ConvolutionInterface::__ViewDrag, w );
   FilterMode_TabBox.OnViewDrop( (Control::view_drop_event_handler)&ConvolutionInterface::__ViewDrop, w );

   FilterThumbnail_Control.SetScaledFixedSize( 160, 160 );
   FilterThumbnail_Control.SetToolTip( "<p>Thumbnail preview of the current convolution filter.</p>"
      "<p>Positive or zero filter coefficients are rendered as grayscale pixels on the thumbnail. "
      "Negative filter coefficients are rendered as red pixels.</p>" );
   FilterThumbnail_Control.OnPaint( (Control::paint_event_handler)&ConvolutionInterface::__Filter_Paint, w );

   FilterInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   FilterInfo_Label.SetToolTip( "<p>Current filter dimensions in pixels.</p>" );

   RenderFilter_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/camera.png" ) ) );
   RenderFilter_ToolButton.SetScaledFixedSize( 20, 20 );
   RenderFilter_ToolButton.SetToolTip( "<p>Render the current filter as a new image.</p>" );
   RenderFilter_ToolButton.OnClick( (Button::click_event_handler)&ConvolutionInterface::__Library_Click, w );

   FilterInfo_Sizer.SetSpacing( 4 );
   FilterInfo_Sizer.Add( FilterInfo_Label );
   FilterInfo_Sizer.AddStretch();
   FilterInfo_Sizer.Add( RenderFilter_ToolButton);

   FilterThumbnail_Sizer.AddStretch();
   FilterThumbnail_Sizer.Add( FilterThumbnail_Control );
   FilterThumbnail_Sizer.Add( FilterInfo_Sizer );

   Filter_Sizer.SetSpacing( 4 );
   Filter_Sizer.Add( FilterMode_TabBox );
   Filter_Sizer.Add( FilterThumbnail_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Filter_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&ConvolutionInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ConvolutionInterface.cpp - Released 2018-11-01T11:07:20Z
