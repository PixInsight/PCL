//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0420
// ----------------------------------------------------------------------------
// AdaptiveStretchInterface.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "AdaptiveStretchInterface.h"
#include "AdaptiveStretchProcess.h"
#include "AdaptiveStretchCurveGraphInterface.h"

#include <pcl/PreviewSelectionDialog.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "AdaptiveStretchIcon.xpm"

// ----------------------------------------------------------------------------

AdaptiveStretchInterface* TheAdaptiveStretchInterface = nullptr;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

AdaptiveStretchInterface::AdaptiveStretchInterface() :
   m_instance( TheAdaptiveStretchProcess )
{
   TheAdaptiveStretchInterface = this;
}

// ----------------------------------------------------------------------------

AdaptiveStretchInterface::~AdaptiveStretchInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString AdaptiveStretchInterface::Id() const
{
   return "AdaptiveStretch";
}

// ----------------------------------------------------------------------------

MetaProcess* AdaptiveStretchInterface::Process() const
{
   return TheAdaptiveStretchProcess;
}

// ----------------------------------------------------------------------------

const char** AdaptiveStretchInterface::IconImageXPM() const
{
   return AdaptiveStretchIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures AdaptiveStretchInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::ResetInstance()
{
   AdaptiveStretchInstance defaultInstance( TheAdaptiveStretchProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "AdaptiveStretch" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheAdaptiveStretchProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* AdaptiveStretchInterface::NewProcess() const
{
   return new AdaptiveStretchInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const AdaptiveStretchInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an AdaptiveStretch instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   UpdateRealTimePreview();
   return true;
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const
{
   return true;
}

// ----------------------------------------------------------------------------

AdaptiveStretchInterface::RealTimeThread::RealTimeThread( const View& view ) :
   m_instance( TheAdaptiveStretchProcess ),
   m_view( view )
{
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::RealTimeThread::Reset( const UInt16Image& image, const AdaptiveStretchInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::RealTimeThread::Run()
{
   m_curve = m_instance.Preview( m_image, m_view );
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchInterface::GenerateRealTimePreview( UInt16Image& image, const View& view, int, String& ) const
{
   m_realTimeThread = new RealTimeThread( view );

   for ( ;; )
   {
      m_realTimeThread->Reset( image, m_instance );
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
         if ( GUI->PlotCurveGraph_CheckBox.IsChecked() )
            UpdateCurveGraph( m_realTimeThread->m_curve );

         delete m_realTimeThread;
         m_realTimeThread = nullptr;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::UpdateControls()
{
   GUI->NoiseThreshold_NumericControl.SetValue( m_instance.p_noiseThreshold );
   GUI->Protection_NumericControl.SetValue( m_instance.p_protection );
   GUI->Protection_NumericControl.Enable( m_instance.p_useProtection );
   GUI->UseProtection_CheckBox.SetChecked( m_instance.p_useProtection );
   GUI->MaxCurvePoints_NumericEdit.SetValue( m_instance.p_maxCurvePoints );
   GUI->ROI_SectionBar.SetChecked( m_instance.p_useROI );
   GUI->ROIX0_NumericEdit.SetValue( m_instance.p_roi.x0 );
   GUI->ROIY0_NumericEdit.SetValue( m_instance.p_roi.y0 );
   GUI->ROIWidth_NumericEdit.SetValue( m_instance.p_roi.Width() );
   GUI->ROIHeight_NumericEdit.SetValue( m_instance.p_roi.Height() );
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != nullptr )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::OpenCurveGraph() const
{
   if ( !TheAdaptiveStretchCurveGraphInterface->IsVisible() )
      TheAdaptiveStretchCurveGraphInterface->Launch();
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::CloseCurveGraph() const
{
   if ( TheAdaptiveStretchCurveGraphInterface->IsVisible() )
      TheAdaptiveStretchCurveGraphInterface->Hide();
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::UpdateCurveGraph( const StretchCurve& curve ) const
{
   OpenCurveGraph();
   TheAdaptiveStretchCurveGraphInterface->UpdateGraph( curve );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->NoiseThreshold_NumericControl )
      m_instance.p_noiseThreshold = value;
   else if ( sender == GUI->Protection_NumericControl )
      m_instance.p_protection = value;
   else if ( sender == GUI->MaxCurvePoints_NumericEdit )
      m_instance.p_maxCurvePoints = int( value );
   else if ( sender == GUI->ROIX0_NumericEdit )
      m_instance.p_roi.MoveTo( int( value ), m_instance.p_roi.y0 );
   else if ( sender == GUI->ROIY0_NumericEdit )
      m_instance.p_roi.MoveTo( m_instance.p_roi.x0, int( value ) );
   else if ( sender == GUI->ROIWidth_NumericEdit )
      m_instance.p_roi.SetWidth( int( value ) );
   else if ( sender == GUI->ROIHeight_NumericEdit )
      m_instance.p_roi.SetHeight( int( value ) );

   UpdateControls();
   UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->UseProtection_CheckBox )
   {
      m_instance.p_useProtection = checked;
      UpdateControls();
      UpdateRealTimePreview();
   }
   else if ( sender == GUI->PlotCurveGraph_CheckBox )
   {
      if ( checked )
         OpenCurveGraph();
      else
         CloseCurveGraph();
   }
   else if ( sender == GUI->ROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               m_instance.p_roi = view.Window().PreviewRect( view.Id() );
               UpdateControls();
               UpdateRealTimePreview();
            }
         }
   }
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::__SectionBarCheck( SectionBar& sender, bool checked )
{
   if ( sender == GUI->ROI_SectionBar )
   {
      m_instance.p_useROI = checked;
      UpdateControls();
      UpdateRealTimePreview();
   }
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::__Hide( Control& sender )
{
   CloseCurveGraph();
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != 0 )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ROI_SectionBar || sender == GUI->ROI_Control || sender == GUI->ROISelectPreview_Button )
      wantsView = view.IsPreview();
}

// ----------------------------------------------------------------------------

void AdaptiveStretchInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ROI_SectionBar || sender == GUI->ROI_Control || sender == GUI->ROISelectPreview_Button )
      if ( view.IsPreview() )
      {
         m_instance.p_useROI = true;
         m_instance.p_roi = view.Window().PreviewRect( view.Id() );
         GUI->ROI_SectionBar.ShowSection();
         UpdateControls();
         UpdateRealTimePreview();
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

AdaptiveStretchInterface::GUIData::GUIData( AdaptiveStretchInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Maximum curve points:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( "Height:" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 13 ) );

   NoiseThreshold_NumericControl.sizer.AddStretch();
   NoiseThreshold_NumericControl.label.SetText( "Noise threshold:" );
   NoiseThreshold_NumericControl.label.SetMinWidth( labelWidth1 );
   NoiseThreshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   NoiseThreshold_NumericControl.SetToolTip( "<p>The AdaptiveStretch process computes a nonlinear curve to enhance existing "
      "brightness differences above the noise threshold parameter. Brightness differences smaller than the noise threshold "
      "will be diminished or not enhanced in the processed image. Decrease this parameter to apply a more aggressive nonlinear "
      "transformation. In general, you should fine tune this parameter along with contrast protection to find an optimal "
      "combination.</p>" );
   NoiseThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AdaptiveStretchInterface::__ValueUpdated, w );

   Protection_NumericControl.label.SetText( "Contrast protection:" );
   Protection_NumericControl.label.SetMinWidth( labelWidth1 );
   Protection_NumericControl.edit.SetFixedWidth( editWidth1 );
   Protection_NumericControl.SetToolTip( "<p>The contrast protection parameter limits the increase of contrast on very bright "
      "and dark areas. Increasing this parameter can help control excessive intensification of highlights and darkening of "
      "shadows.</p>" );
   Protection_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AdaptiveStretchInterface::__ValueUpdated, w );

   UseProtection_CheckBox.SetText( String() );
   UseProtection_CheckBox.SetToolTip( "<p>Enable this option to apply the contrast protection feature.</p>" );
   UseProtection_CheckBox.OnClick( (Button::click_event_handler)&AdaptiveStretchInterface::__Click, w );

   Protection_Sizer.Add( Protection_NumericControl );
   Protection_Sizer.AddSpacing( 8 );
   Protection_Sizer.Add( UseProtection_CheckBox );

   MaxCurvePoints_NumericEdit.SetInteger();
   MaxCurvePoints_NumericEdit.SetRange( TheASMaxCurvePointsParameter->MinimumValue(),
                                        TheASMaxCurvePointsParameter->MaximumValue() );
   MaxCurvePoints_NumericEdit.label.SetText( "Maximum curve points:" );
   MaxCurvePoints_NumericEdit.label.SetFixedWidth( labelWidth1 );
   MaxCurvePoints_NumericEdit.edit.SetFixedWidth( editWidth1 );
   MaxCurvePoints_NumericEdit.sizer.AddStretch();
   MaxCurvePoints_NumericEdit.SetToolTip( "<p>Maximum number of curve points. The default value is 10<sup>6</sup> points. "
      "For 8-bit and 16-bit integer images, AdaptiveStretch can compute a maximum of 256 and 65536 curve points, respectively. "
      "For 32-bit and 64-bit floating point images, a maximum of 10<sup>7</sup> and 10<sup>8</sup> curve points can be "
      "calculated, respectively, but the default maximum of 10<sup>6</sup> is normally more than sufficient.</p>"
      "<p>Increasing this parameter can improve the result for very rich floating point and 32-bit integer images, usually "
      "resulting from deep HDR compositions. Keep in mind that large curves will increase computation times considerably, "
      "so they should only be used when actually required. In most cases, the default value of this parameter is quite "
      "appropriate.</p>" );
   MaxCurvePoints_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&AdaptiveStretchInterface::__ValueUpdated, w );

   PlotCurveGraph_CheckBox.SetText( "Real-time curve graph" );
   PlotCurveGraph_CheckBox.SetToolTip( "<p>Plot the computed AdaptiveStretch curve on an auxiliary window during real-time "
      "preview operations. Uncheck this option to close the graph window, if it is visible.</p>" );
   PlotCurveGraph_CheckBox.SetChecked();
   PlotCurveGraph_CheckBox.OnClick( (Button::click_event_handler)&AdaptiveStretchInterface::__Click, w );

   PlotCurveGraph_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   PlotCurveGraph_Sizer.Add( PlotCurveGraph_CheckBox );
   PlotCurveGraph_Sizer.AddStretch();

   ROI_SectionBar.SetTitle( "Region of Interest" );
   ROI_SectionBar.SetSection( ROI_Control );
   ROI_SectionBar.EnableTitleCheckBox();
   ROI_SectionBar.SetToolTip( "<p>By default, the AdaptiveStretch process analyzes pairwise pixel brightness differences on "
      "the whole target image. When necessary, a region of interest (ROI) can be defined to restrict the analysis to a "
      "rectangular region of the image. This may help find an optimal transformation for image structures that are not "
      "dominant on the entire image.</p>" );
   ROI_SectionBar.OnCheck( (SectionBar::check_event_handler)&AdaptiveStretchInterface::__SectionBarCheck, w );
   ROI_SectionBar.OnViewDrag( (Control::view_drag_event_handler)&AdaptiveStretchInterface::__ViewDrag, w );
   ROI_SectionBar.OnViewDrop( (Control::view_drop_event_handler)&AdaptiveStretchInterface::__ViewDrop, w );

   ROIX0_NumericEdit.SetInteger();
   ROIX0_NumericEdit.SetRange( 0, int_max );
   ROIX0_NumericEdit.label.SetText( "Left:" );
   ROIX0_NumericEdit.label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 ) ); // minus ROI_Sizer.Margin()
   ROIX0_NumericEdit.edit.SetFixedWidth( editWidth1 );
   ROIX0_NumericEdit.SetToolTip( "<p>X pixel coordinate of the upper left corner of the ROI.</p>");
   ROIX0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&AdaptiveStretchInterface::__ValueUpdated, w );

   ROIY0_NumericEdit.SetInteger();
   ROIY0_NumericEdit.SetRange( 0, int_max );
   ROIY0_NumericEdit.label.SetText( "Top:" );
   ROIY0_NumericEdit.label.SetFixedWidth( labelWidth2 );
   ROIY0_NumericEdit.edit.SetFixedWidth( editWidth1 );
   ROIY0_NumericEdit.SetToolTip( "<p>Y pixel coordinate of the upper left corner of the ROI.</p>");
   ROIY0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&AdaptiveStretchInterface::__ValueUpdated, w );

   ROIRow1_Sizer.SetSpacing( 4 );
   ROIRow1_Sizer.Add( ROIX0_NumericEdit );
   ROIRow1_Sizer.Add( ROIY0_NumericEdit );
   ROIRow1_Sizer.AddStretch();

   ROIWidth_NumericEdit.SetInteger();
   ROIWidth_NumericEdit.SetRange( 0, int_max );
   ROIWidth_NumericEdit.label.SetText( "Width:" );
   ROIWidth_NumericEdit.label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 ) ); // minus ROI_Sizer.Margin()
   ROIWidth_NumericEdit.edit.SetFixedWidth( editWidth1 );
   ROIWidth_NumericEdit.SetToolTip( "<p>Width of the ROI in pixels.</p>");
   ROIWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&AdaptiveStretchInterface::__ValueUpdated, w );

   ROIHeight_NumericEdit.SetInteger();
   ROIHeight_NumericEdit.SetRange( 0, int_max );
   ROIHeight_NumericEdit.label.SetText( "Height:" );
   ROIHeight_NumericEdit.label.SetFixedWidth( labelWidth2 );
   ROIHeight_NumericEdit.edit.SetFixedWidth( editWidth1 );
   ROIHeight_NumericEdit.SetToolTip( "<p>Height of the ROI in pixels.</p>");
   ROIHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&AdaptiveStretchInterface::__ValueUpdated, w );

   ROISelectPreview_Button.SetText( "From Preview" );
   ROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>" );
   ROISelectPreview_Button.OnClick( (Button::click_event_handler)&AdaptiveStretchInterface::__Click, w );
   ROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&AdaptiveStretchInterface::__ViewDrag, w );
   ROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&AdaptiveStretchInterface::__ViewDrop, w );

   ROIRow2_Sizer.SetSpacing( 4 );
   ROIRow2_Sizer.Add( ROIWidth_NumericEdit );
   ROIRow2_Sizer.Add( ROIHeight_NumericEdit );
   ROIRow2_Sizer.AddSpacing( 24 );
   ROIRow2_Sizer.Add( ROISelectPreview_Button );
   ROIRow2_Sizer.AddStretch();

   ROI_Sizer.SetMargin( 6 );
   ROI_Sizer.SetSpacing( 4 );
   ROI_Sizer.Add( ROIRow1_Sizer );
   ROI_Sizer.Add( ROIRow2_Sizer );

   ROI_Control.SetSizer( ROI_Sizer );
   ROI_Control.OnViewDrag( (Control::view_drag_event_handler)&AdaptiveStretchInterface::__ViewDrag, w );
   ROI_Control.OnViewDrop( (Control::view_drop_event_handler)&AdaptiveStretchInterface::__ViewDrop, w );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( NoiseThreshold_NumericControl );
   Global_Sizer.Add( Protection_Sizer );
   Global_Sizer.Add( MaxCurvePoints_NumericEdit );
   Global_Sizer.Add( PlotCurveGraph_Sizer );
   Global_Sizer.Add( ROI_SectionBar );
   Global_Sizer.Add( ROI_Control );

   ROI_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   w.OnHide( (Control::event_handler)&AdaptiveStretchInterface::__Hide, w );

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&AdaptiveStretchInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchInterface.cpp - Released 2018-11-23T18:45:58Z
