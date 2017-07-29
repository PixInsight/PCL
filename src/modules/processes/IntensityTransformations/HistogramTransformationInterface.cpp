//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0397
// ----------------------------------------------------------------------------
// HistogramTransformationInterface.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "CurvesTransformationInterface.h"
#include "HistogramTransformationInterface.h"
#include "HistogramTransformationProcess.h"
#include "HistogramAutoClipSetupDialog.h"
#include "ScreenTransferFunctionInstance.h"
#include "ScreenTransferFunctionParameters.h" // for STFInteraction

#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ImageWindow.h>
#include <pcl/RealTimePreview.h>
#include <pcl/Settings.h>
#include <pcl/Vector.h>

#define m_currentView      GUI->AllViews_ViewList.CurrentView()

#define WHEEL_STEP_ANGLE   PixInsightSettings::GlobalInteger( "ImageWindow/WheelStepAngle" )

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramTransformationInterface* TheHistogramTransformationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "HistogramTransformationIcon.xpm"
#include "raw_rgb.xpm"
#include "lock_output.xpm"
#include "show_curve.xpm"
#include "show_grid.xpm"
#include "reject_saturated.xpm"
#include "auto_zero_shadows.xpm"
#include "auto_zero_highlights.xpm"
#include "auto_clip_shadows.xpm"
#include "auto_clip_highlights.xpm"

// ----------------------------------------------------------------------------

static int s_plotResolutions[] = { 64, 128, 256, 512, 1024, 4096, 16384, 65536, 100, 1000, 10000 };

static const char* s_plotResolutionItems[] =
{
   "6-bit (64)",
   "7-bit (128)",
   "8-bit (256)",
   "9-bit (512)",
   "10-bit (1024)",
   "12-bit (4096)",
   "14-bit (16K)",
   "16-bit (64K)",
   "100 levels",
   "1000 levels",
   "10000 levels"
};

static const int s_maxZoom = 999;

// ----------------------------------------------------------------------------

HistogramTransformationInterface::HistogramTransformationInterface() :
   m_instance( TheHistogramTransformationProcess ),
   m_plotResolution( 256 ),
   m_mode( ReadoutMode ),
   m_savedMode( NoMode ),
   m_readoutMode( NormalReadout ),
   m_channel( 3 ), // 3=RGB/K
   m_graphStyle( LineStyle ),
   m_shadowsAutoClipping( 0.01 ),
   m_highlightsAutoClipping( 0.01 ),
   m_shadowsCount( uint64( 0 ), 5 ),
   m_highlightsCount( uint64( 0 ), 5 ),
   m_readoutActive( false ),
   m_inputReadouts( 0.0, 5 ),
   m_outputReadouts( 0.0, 5 ),
   m_inputZoomX( 1 ),
   m_inputZoomY( 1 ),
   m_outputZoomX( 1 ),
   m_outputZoomY( 1 ),
   m_wheelSteps( 0 ),
   m_rejectSaturated( true ),
   m_rawRGBInput( true ),
   m_lockOutputChannel( true ),
   m_showMTF( true ),
   m_showGrid( true ),
   m_sliderBeingDragged( NoSlider ),
   m_panning( 0 ),
   m_panOrigin( 0 ),
   m_cursorStatus( NoCursor ),
   m_cursorPos( -1 ),
   m_histogramPos( 0 ),
   m_inputBitmap( Bitmap::Null() ),
   m_inputDirty( true ),
   m_outputBitmap( Bitmap::Null() ),
   m_outputDirty( true ),
   m_slidersBitmap( Bitmap::Null() ),
   m_slidersDirty( true ),
   m_outputSectionVisible( true ),
   m_rangeSectionVisible( false ),
   m_channelColors( 5 ),
   m_minHistogramWidth( 400 ),
   m_minHistogramHeight( 200 ),
   m_sliderControlSize( 12 ),
   m_settingUp( false )
{
   TheHistogramTransformationInterface = this;

   m_channelColors[0] = RGBAColor( 0xFF, 0x20, 0x20 );
   m_channelColors[1] = RGBAColor( 0x00, 0xFF, 0x00 );
   m_channelColors[2] = RGBAColor( 0x00, 0xA0, 0xFF );
   m_channelColors[3] = RGBAColor( 0xE0, 0xE0, 0xE0 );
   m_channelColors[4] = RGBAColor( 0xFF, 0xFF, 0xFF );
   m_gridColor0       = RGBAColor( 0x50, 0x50, 0x50 );
   m_gridColor1       = RGBAColor( 0x37, 0x37, 0x37 );
   m_backgroundColor  = RGBAColor( 0x00, 0x00, 0x00 );
}

HistogramTransformationInterface::~HistogramTransformationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString HistogramTransformationInterface::Id() const
{
   return "HistogramTransformation";
}

// ----------------------------------------------------------------------------

MetaProcess* HistogramTransformationInterface::Process() const
{
   return TheHistogramTransformationProcess;
}

// ----------------------------------------------------------------------------

const char** HistogramTransformationInterface::IconImageXPM() const
{
   return HistogramTransformationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures HistogramTransformationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton | InterfaceFeature::TrackViewButton;
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         GUI->AllViews_ViewList.Regenerate( true, true, false );
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      }
      else
      {
         GUI->AllViews_ViewList.Regenerate( true, true, true );
         RealTimePreview::SetOwner( ProcessInterface::Null() );
      }
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.CurrentView() );
      }
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ResetInstance()
{
   HistogramTransformationInstance defaultInstance( TheHistogramTransformationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "HistogramTransformation" );
      GUI->AllViews_ViewList.Regenerate( true/*mainViews*/, true/*previews*/, true/*virtualViews*/ );
      OnKeyPress( (Control::keyboard_event_handler)&HistogramTransformationInterface::__KeyPress, *this );
      OnKeyRelease( (Control::keyboard_event_handler)&HistogramTransformationInterface::__KeyRelease, *this );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheHistogramTransformationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* HistogramTransformationInterface::NewProcess() const
{
   return new HistogramTransformationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const HistogramTransformationInstance*>( &p ) != nullptr )
      return true;
   if ( dynamic_cast<const ScreenTransferFunctionInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Must be an instance of either HistogramTransformation or ScreenTransferFunction.";
   return false;
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::ImportProcess( const ProcessImplementation& p )
{
   const ScreenTransferFunctionInstance* stf = dynamic_cast<const ScreenTransferFunctionInstance*>( &p );
   if ( stf != nullptr )
   {
      m_instance.Reset();

      /*
       * Notes on importing instances to the HT interface:
       *
       * - We assign STF parameters to the combined RGB/K histogram channel if
       *   the STF instance declares grayscale interaction, or when the three
       *   STF parameter sets are identical.
       *
       * - Otherwise, we assign STF parameters to individual RGB histogram
       *   channels.
       *
       * - Luminance STF parameters are never assigned, since they are not
       *   defined in HistogramTransformation.
       */

      const STF& f0 = (*stf)[0];
      const STF& f1 = (*stf)[1];
      const STF& f2 = (*stf)[2];

      if ( stf->Interaction() == STFInteraction::Grayscale || f0 == f1 && f1 == f2 )
      {
         m_instance.SetMidtonesBalance( 3, f0.m );
         m_instance.SetShadowsClipping( 3, f0.c0 );
         m_instance.SetHighlightsClipping( 3, f0.c1 );
         m_instance.SetLowRange( 3, f0.r0 );
         m_instance.SetHighRange( 3, f0.r1 );
      }
      else
      {
         for ( int c = 0; c < 3; ++c )
         {
            const STF& f = (*stf)[c];
            m_instance.SetMidtonesBalance( c, f.m );
            m_instance.SetShadowsClipping( c, f.c0 );
            m_instance.SetHighlightsClipping( c, f.c1 );
            m_instance.SetLowRange( c, f.r0 );
            m_instance.SetHighRange( c, f.r1 );
         }
      }
   }
   else
   {
      m_instance.Assign( p );
   }

   CalculateOutputHistograms();
   CalculateClippingCounts();

   UpdateControls();
   UpdateHistograms();
   UpdateRealTimePreview();

   return true;
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::RequiresRealTimePreviewUpdate( const UInt16Image& img, const View&, int zoomLevel ) const
{
   return true;
}

// ----------------------------------------------------------------------------

HistogramTransformationInterface::RealTimeThread::RealTimeThread() :
   m_instance( TheHistogramTransformationProcess )
{
}

void HistogramTransformationInterface::RealTimeThread::Reset( const UInt16Image& image,
                                                              const HistogramTransformationInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

void HistogramTransformationInterface::RealTimeThread::Run()
{
   m_instance.Preview( m_image );
}

bool HistogramTransformationInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int/*zoomLevel*/, String&/*info*/ ) const
{
   m_realTimeThread = new RealTimeThread;

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

         delete m_realTimeThread;
         m_realTimeThread = nullptr;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == m_currentView )
         if ( !m_currentView.HasProperty( "Histogram16" ) )
            m_currentView.ComputeProperty( "Histogram16" );
         else
            SynchronizeWithCurrentView();
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ImageFocused( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
      {
         GUI->AllViews_ViewList.SelectView( v ); // normally not necessary, but we can invoke this f() directly
         if ( !m_currentView.IsNull() && !m_currentView.HasProperty( "Histogram16" ) )
            m_currentView.ComputeProperty( "Histogram16" );
         else
            SynchronizeWithCurrentView();
      }
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::WantsViewPropertyNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ViewPropertyUpdated( const View& v, const IsoString& property )
{
   if ( GUI != nullptr )
      if ( v == m_currentView )
         if ( property == "Histogram16" || property == "*" )
            SynchronizeWithCurrentView();
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ViewPropertyDeleted( const View& v, const IsoString& property )
{
   if ( GUI != nullptr )
      if ( v == m_currentView )
         if ( property == "Histogram16" || property == "*" )
         {
            DeactivateTrackView();
            GUI->AllViews_ViewList.SelectView( View::Null() );
            DestroyOutputHistograms();
            UpdateHistograms();
         }
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::BeginReadout( const View& v )
{
   if ( GUI != nullptr )
      if ( IsVisible() )
         m_readoutActive = true;
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double A )
{
   if ( !m_readoutActive )
      return;

   m_inputReadouts[0] = m_outputReadouts[0] = R;
   m_inputReadouts[1] = m_outputReadouts[1] = G;
   m_inputReadouts[2] = m_outputReadouts[2] = B;
   m_inputReadouts[4] = m_outputReadouts[4] = A;

   HistogramTransformation H3( m_instance.MidtonesBalance( 3 ),
                               m_instance.ShadowsClipping( 3 ), m_instance.HighlightsClipping( 3 ),
                               m_instance.LowRange( 3 ), m_instance.HighRange( 3 ) );

   for ( int c = 0; c < 3; ++c )
   {
      HistogramTransformation H( m_instance.MidtonesBalance( c ),
                                 m_instance.ShadowsClipping( c ), m_instance.HighlightsClipping( c ),
                                 m_instance.LowRange( c ), m_instance.HighRange( c ) );
      H.Add( H3 );

      if ( !H.IsIdentityTransformationSet() )
         H.Apply( m_outputReadouts.At( c ), 1 );
   }

   HistogramTransformation HA( m_instance.MidtonesBalance( 4 ),
                               m_instance.ShadowsClipping( 4 ), m_instance.HighlightsClipping( 4 ),
                               m_instance.LowRange( 4 ), m_instance.HighRange( 4 ) );

   if ( !HA.IsIdentityTransformation() )
      HA.Apply( m_outputReadouts.At( 4 ), 1 );

   if ( m_readoutMode != NormalReadout )
   {
      int i0, i1;
      if ( v.IsColor() )
      {
         i0 = (m_channel == 3) ? 0 : m_channel;
         i1 = (m_channel == 3) ? 3 : m_channel+1;
      }
      else
      {
         i0 = (m_channel == 4) ? 4 : 3;
         i1 = i0 + 1;
      }

      for ( int i = i0; i < i1; ++i )
      {
         double r = m_inputReadouts[i];

         switch ( m_readoutMode )
         {
         case BlackPointReadout :
            m_instance.SetShadowsClipping( i, Min( r, m_instance.HighlightsClipping( i ) ) );
            break;
         case MidtonesReadout :
            m_instance.SetMidtonesBalance( i, r );
            break;
         case WhitePointReadout :
            m_instance.SetHighlightsClipping( i, Max( r, m_instance.ShadowsClipping( i ) ) );
            break;
         default:
            break;
         }
      }

      CalculateOutputHistograms();

      if ( m_readoutMode != MidtonesReadout )
      {
         CalculateClippingCounts();
         UpdateClippingCountControls();
      }

      UpdateHistogramControls();
      UpdateRealTimePreview();
   }

   UpdateHistograms();
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::EndReadout( const View& v )
{
   if ( m_readoutActive )
   {
      m_readoutActive = false;
      UpdateHistograms();
   }
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::WantsRealTimePreviewNotifications() const
{
   return true;
}

void HistogramTransformationInterface::RealTimePreviewOwnerChanged( ProcessInterface& iface )
{
   if ( GUI != nullptr )
      GUI->AllViews_ViewList.Regenerate( true, true, iface != *this );
}

// ----------------------------------------------------------------------------

#define KEY_HT    SettingsKey()
#define KEY_CLIP  SettingsKey() + "AutoClip/"
#define KEY_GEOM  SettingsKey() + "Geometry/"

void HistogramTransformationInterface::SaveSettings() const
{
   IsoString key = KEY_HT;

   Settings::Write( key + "PlotResolution", m_plotResolution );
   Settings::Write( key + "GraphStyle", m_graphStyle );
   Settings::Write( key + "RejectSaturated", m_rejectSaturated );

   key = KEY_CLIP;
   Settings::Write( key + "AutoClipShadows", m_shadowsAutoClipping );
   Settings::Write( key + "AutoClipHighlights", m_highlightsAutoClipping );

   key = KEY_GEOM;
   Settings::Write( key + "OutputSectionVisible", m_outputSectionVisible );
   Settings::Write( key + "RangeSectionVisible", m_rangeSectionVisible );
}

void HistogramTransformationInterface::LoadSettings()
{
   IsoString key = KEY_HT;

   int newPlotResolution = m_plotResolution;
   Settings::Read( key + "PlotResolution", newPlotResolution );
   if ( newPlotResolution != m_plotResolution )
      for ( size_type i = 0; i < ItemsInArray( s_plotResolutions ); ++i )
         if ( s_plotResolutions[i] == newPlotResolution )
         {
            SetPlotResolution( newPlotResolution );
            break;
         }

   int graphStyle;
   Settings::Read( key + "GraphStyle", graphStyle );
   if ( graphStyle >= 0 && graphStyle < NumberOfGraphStyles )
      SetGraphStyle( graph_style( graphStyle ) );

   bool rejectSaturated;
   Settings::Read( key + "RejectSaturated", rejectSaturated );
   SetRejectSaturated( rejectSaturated );

   key = KEY_CLIP;
   Settings::Read( key + "AutoClipShadows", m_shadowsAutoClipping );
   Settings::Read( key + "AutoClipHighlights", m_highlightsAutoClipping );
   UpdateAutoClippingToolTips();

   key = KEY_GEOM;
   bool newOutputSectionVisible = m_outputSectionVisible;
   bool newRangeSectionVisible = m_rangeSectionVisible;

   Settings::Read( key + "OutputSectionVisible", newOutputSectionVisible );
   Settings::Read( key + "RangeSectionVisible", newRangeSectionVisible );

   if ( newOutputSectionVisible != m_outputSectionVisible )
      ToggleOutputHistogram();

   if ( newRangeSectionVisible != m_rangeSectionVisible )
      ToggleRangeControls();

   AdjustToContents();
}

#undef KEY_HT
#undef KEY_CLIP
#undef KEY_GEOM

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool HistogramTransformationInterface::GetSourceHistograms()
{
   m_sourceData.Clear();
   if ( m_currentView.IsNull() )
      return false;
   if ( !m_currentView.HasProperty( "Histogram16" ) )
      return false;
   UI64Matrix M = m_currentView.PropertyValue( "Histogram16" ).ToUI64Matrix();
   for ( int i = 0; i < M.Rows(); ++i )
      m_sourceData.Add( Histogram( M.RowVector( i ) ) );
   return true;
}

void HistogramTransformationInterface::CalculateInputHistograms()
{
   m_sourceData.Clear();
   m_inputData.Clear();

   if ( !GetSourceHistograms() )
      return;

   for ( histogram_list::const_iterator i = m_sourceData.Begin(); i != m_sourceData.End(); ++i )
   {
      Histogram h( m_plotResolution );
      i->Resample( h );
      m_inputData.Add( h );
   }
}

void HistogramTransformationInterface::CalculateOutputHistograms()
{
   DestroyOutputHistograms();

   if ( m_sourceData.IsEmpty() )
      if ( !GetSourceHistograms() )
         return;

   ImageVariant image = m_currentView.Image();

   HistogramTransformation T3( m_instance.MidtonesBalance( 3 ),
                               m_instance.ShadowsClipping( 3 ), m_instance.HighlightsClipping( 3 ),
                               m_instance.LowRange( 3 ), m_instance.HighRange( 3 ) );

   for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
   {
      Histogram h( m_plotResolution );

      if ( image->IsColor() )
      {
         HistogramTransformation T( m_instance.MidtonesBalance( c ),
                                    m_instance.ShadowsClipping( c ), m_instance.HighlightsClipping( c ),
                                    m_instance.LowRange( c ), m_instance.HighRange( c ) );
         Histogram hc( m_plotResolution );
         T.Apply( hc, m_sourceData[c] );
         T.Add( T3 );
         T.Apply( h, m_sourceData[c] );
         m_outputRGBData.Add( hc );
      }
      else
      {
         T3.Apply( h, m_sourceData[c] );
      }

      m_outputData.Add( h );
   }

   if ( image->HasAlphaChannels() )
   {
      HistogramTransformation TA( m_instance.MidtonesBalance( 4 ),
                                  m_instance.ShadowsClipping( 4 ), m_instance.HighlightsClipping( 4 ),
                                  m_instance.LowRange( 4 ), m_instance.HighRange( 4 ) );
      Histogram ha( m_plotResolution );
      TA.Apply( ha, m_sourceData[image->NumberOfNominalChannels()] );
      m_outputData.Add( ha );
   }
}

void HistogramTransformationInterface::DestroyOutputHistograms()
{
   m_outputData.Clear();
   m_outputRGBData.Clear();
}

void HistogramTransformationInterface::CalculateClippingCounts()
{
   m_shadowsCount = 0;
   m_highlightsCount = 0;

   if ( m_sourceData.IsEmpty() )
      if ( !GetSourceHistograms() )
         return;

   ImageVariant image = m_currentView.Image();

   for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
   {
      int r = m_sourceData[c].Resolution();
      int cc = image->IsColor() ? c : 3;
      int i0 = RoundInt( m_instance.ShadowsClipping( cc )*(r - 1) );
      int i1 = RoundInt( m_instance.HighlightsClipping( cc )*(r - 1) );

      for ( int i = 0; i < i0; ++i )
         m_shadowsCount[cc] += m_sourceData[c].Count( i );
      for ( int i = i1; ++i < r; )
         m_highlightsCount[cc] += m_sourceData[c].Count( i );
   }

   if ( image->IsColor() )
   {
      if ( m_outputRGBData.IsEmpty() )
         CalculateOutputHistograms();

      for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
      {
         int r = m_outputRGBData[c].Resolution();
         int i0 = RoundInt( m_instance.ShadowsClipping( 3 )*(r - 1) );
         int i1 = RoundInt( m_instance.HighlightsClipping( 3 )*(r - 1) );

         for ( int i = 0; i < i0; ++i )
            m_shadowsCount[3] += m_outputRGBData[c].Count( i );
         for ( int i = i1; ++i < r; )
            m_highlightsCount[3] += m_outputRGBData[c].Count( i );
      }
   }

   if ( image->HasAlphaChannels() )
   {
      int c = image->NumberOfNominalChannels();
      int r = m_sourceData[c].Resolution();
      int i0 = RoundInt( m_instance.ShadowsClipping( 4 )*(r - 1) );
      int i1 = RoundInt( m_instance.HighlightsClipping( 4 )*(r - 1) );

      for ( int i = 0; i < i0; ++i )
         m_shadowsCount[4] += m_sourceData[c].Count( i );
      for ( int i = i1; ++i < r; )
         m_highlightsCount[4] += m_sourceData[c].Count( i );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HistogramTransformationInterface::SetClippingParameters( double c0, double c1 )
{
   m_instance.SetShadowsClipping( m_channel, c0 );
   m_instance.SetHighlightsClipping( m_channel, c1 );

   GUI->ShadowsClipping_NumericEdit.SetValue( m_instance.ShadowsClipping( m_channel ) );
   GUI->HighlightsClipping_NumericEdit.SetValue( m_instance.HighlightsClipping( m_channel ) );

   GUI->HistogramSliders_Control.Update();

   if ( m_showMTF )
      GUI->InputHistogramPlot_Control.Update();

   CalculateOutputHistograms();
   CalculateClippingCounts();

   UpdateClippingCountControls();
   UpdateOutputHistogram();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::SetShadowsClipping( double x )
{
   x = Min( x, m_instance.HighlightsClipping( m_channel ) );
   m_instance.SetShadowsClipping( m_channel, x );

   GUI->ShadowsClipping_NumericEdit.SetValue( x );

   GUI->HistogramSliders_Control.Update();

   if ( m_showMTF )
      GUI->InputHistogramPlot_Control.Update();

   CalculateOutputHistograms();
   CalculateClippingCounts();

   UpdateClippingCountControls();
   UpdateOutputHistogram();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::SetHighlightsClipping( double x )
{
   x = Max( x, m_instance.ShadowsClipping( m_channel ) );
   m_instance.SetHighlightsClipping( m_channel, x );

   GUI->HighlightsClipping_NumericEdit.SetValue( x );

   GUI->HistogramSliders_Control.Update();

   if ( m_showMTF )
      GUI->InputHistogramPlot_Control.Update();

   CalculateOutputHistograms();
   CalculateClippingCounts();

   UpdateClippingCountControls();
   UpdateOutputHistogram();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::SetMidtonesBalance( double x )
{
   m_instance.SetMidtonesBalance( m_channel, x );

   GUI->MidtonesBalance_NumericEdit.SetValue( x );

   GUI->HistogramSliders_Control.Update();

   if ( m_showMTF )
      GUI->InputHistogramPlot_Control.Update();

   CalculateOutputHistograms();

   UpdateOutputHistogram();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::SetLowRange( double x )
{
   m_instance.SetLowRange( m_channel, x );

   GUI->LowRange_NumericControl.SetValue( x );

   CalculateOutputHistograms();

   UpdateOutputHistogram();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::SetHighRange( double x )
{
   m_instance.SetHighRange( m_channel, x );

   GUI->HighRange_NumericControl.SetValue( x );

   CalculateOutputHistograms();

   UpdateOutputHistogram();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::SetChannel( int c )
{
   m_channel = c;

   UpdateChannelControls();
   UpdateHistogramControls();
   UpdateClippingCountControls();
   UpdateHistograms();
}

void HistogramTransformationInterface::SetPlotResolution( int r )
{
   m_plotResolution = r;

   SynchronizeWithCurrentView();
   UpdateGraphicsControls();

   if ( TheCurvesTransformationInterface->IsVisible() )
      TheCurvesTransformationInterface->UpdateHistograms();
}

void HistogramTransformationInterface::SetGraphStyle( graph_style s )
{
   m_graphStyle = s;

   UpdateGraphicsControls();
   UpdateHistograms();

   if ( TheCurvesTransformationInterface->IsVisible() )
      TheCurvesTransformationInterface->UpdateHistograms();
}

void HistogramTransformationInterface::SetRejectSaturated( bool reject )
{
   m_rejectSaturated = reject;

   UpdateGraphicsControls();
   UpdateHistograms();

   if ( TheCurvesTransformationInterface->IsVisible() )
      TheCurvesTransformationInterface->UpdateHistograms();
}

void HistogramTransformationInterface::SetInputZoom( int hz, int vz, const Point* p )
{
   m_settingUp = true;

   m_inputZoomX = hz;
   m_inputZoomY = vz;

   bool hsb = m_inputZoomX > 1;
   bool vsb = m_inputZoomY > 1;

   GUI->InputHistogram_ScrollBox.ShowScrollBars( hsb, vsb );

   int visibleWidth = GUI->InputHistogram_ScrollBox.Viewport().Width();
   int visibleHeight = GUI->InputHistogram_ScrollBox.Viewport().Height();

   int sliderControlSize = RoundInt( LogicalPixelsToPhysical( m_sliderControlSize ) );
   int contentsWidth = visibleWidth * m_inputZoomX;
   int contentsHeight = (visibleHeight - sliderControlSize) * m_inputZoomY + sliderControlSize;

   if ( hsb )
   {
      int m = contentsWidth - visibleWidth;
      GUI->InputHistogram_ScrollBox.SetHorizontalScrollRange( 0, m );

      if ( p != nullptr )
         GUI->InputHistogram_ScrollBox.SetHorizontalScrollPosition(
                  Range( p->x*m_inputZoomX - (visibleWidth >> 1), 0, m ) );
   }
   else
      GUI->InputHistogram_ScrollBox.SetHorizontalScrollRange( 0, 0 );

   GUI->InputHistogram_ScrollBox.SetPageWidth( visibleWidth );

   if ( vsb )
   {
      int m = contentsHeight - visibleHeight;
      GUI->InputHistogram_ScrollBox.SetVerticalScrollRange( 0, m );

      if ( p != nullptr )
         GUI->InputHistogram_ScrollBox.SetVerticalScrollPosition(
                  Range( p->y*m_inputZoomY - (visibleHeight >> 1), 0, m ) );
   }
   else
      GUI->InputHistogram_ScrollBox.SetVerticalScrollRange( 0, 0 );

   GUI->InputHistogram_ScrollBox.SetPageHeight( visibleHeight );

   UpdateZoomControls();
   UpdateInputHistogram();

   m_settingUp = false;
}

void HistogramTransformationInterface::SetOutputZoom( int hz, int vz, const Point* p )
{
   m_settingUp = true;

   m_outputZoomX = hz;
   m_outputZoomY = vz;

   bool hsb = m_outputZoomX > 1;
   bool vsb = m_outputZoomY > 1;

   GUI->OutputHistogram_ScrollBox.ShowScrollBars( hsb, vsb );

   int visibleWidth = GUI->OutputHistogram_ScrollBox.Viewport().Width();
   int visibleHeight = GUI->OutputHistogram_ScrollBox.Viewport().Height();

   int contentsWidth = visibleWidth * m_outputZoomX;
   int contentsHeight = visibleHeight * m_outputZoomY;

   if ( hsb )
   {
      int m = contentsWidth - visibleWidth;
      GUI->OutputHistogram_ScrollBox.SetHorizontalScrollRange( 0, m );

      if ( p != nullptr )
         GUI->OutputHistogram_ScrollBox.SetHorizontalScrollPosition(
                  Range( p->x*m_outputZoomX - (visibleWidth >> 1), 0, m ) );
   }
   else
      GUI->OutputHistogram_ScrollBox.SetHorizontalScrollRange( 0, 0 );

   GUI->OutputHistogram_ScrollBox.SetPageWidth( visibleWidth );

   if ( vsb )
   {
      int m = contentsHeight - visibleHeight;
      GUI->OutputHistogram_ScrollBox.SetVerticalScrollRange( 0, m );

      if ( p != nullptr )
         GUI->OutputHistogram_ScrollBox.SetVerticalScrollPosition(
                  Range( p->y*m_outputZoomY - (visibleHeight >> 1), 0, m ) );
   }
   else
      GUI->OutputHistogram_ScrollBox.SetVerticalScrollRange( 0, 0 );

   GUI->OutputHistogram_ScrollBox.SetPageHeight( visibleHeight );

   UpdateZoomControls();
   UpdateOutputHistogram();

   m_settingUp = false;
}

void HistogramTransformationInterface::SetMode( working_mode m )
{
   m_mode = m;

   if ( GUI->InputHistogramPlot_Control.IsUnderMouse() )
      GUI->InputHistogramPlot_Control.Update();
   else if ( GUI->OutputHistogram_ScrollBox.Viewport().IsUnderMouse() )
      GUI->OutputHistogram_ScrollBox.Viewport().Update();

   UpdateModeControls();
   UpdateHistogramInfo();
}

void HistogramTransformationInterface::SetReadoutMode( readout_mode m )
{
   m_readoutMode = m;
   UpdateReadoutModeControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HistogramTransformationInterface::UpdateControls()
{
   UpdateModeControls();
   UpdateZoomControls();
   UpdateGraphicsControls();
   UpdateChannelControls();
   UpdateReadoutModeControls();
   UpdateHistogramControls();
   UpdateClippingCountControls();
   UpdateAutoClippingToolTips();
}

void HistogramTransformationInterface::UpdateModeControls()
{
   GUI->ReadoutMode_ToolButton.SetChecked( m_mode == ReadoutMode );
   GUI->ZoomInMode_ToolButton.SetChecked( m_mode == ZoomInMode );
   GUI->ZoomOutMode_ToolButton.SetChecked( m_mode == ZoomOutMode );
   GUI->PanMode_ToolButton.SetChecked( m_mode == PanMode );
}

void HistogramTransformationInterface::UpdateZoomControls()
{
   GUI->HorizontalZoom_SpinBox.SetValue( m_inputZoomX );
   GUI->VerticalZoom_SpinBox.SetValue( m_inputZoomY );
   GUI->OutputHorizontalZoom_SpinBox.SetValue( m_outputZoomX );
   GUI->OutputVerticalZoom_SpinBox.SetValue( m_outputZoomY );
}

void HistogramTransformationInterface::UpdateGraphicsControls()
{
   int i = pcl::LinearSearch( s_plotResolutions,
      s_plotResolutions+ItemsInArray( s_plotResolutions ), m_plotResolution ) - s_plotResolutions;

   GUI->PlotResolution_ComboBox.SetCurrentItem( i );

   GUI->GraphStyle_ComboBox.SetCurrentItem( int( m_graphStyle ) );

   GUI->RejectSaturated_ToolButton.SetChecked( m_rejectSaturated );
   GUI->ShowRawRGB_ToolButton.SetChecked( m_rawRGBInput );
   GUI->LockOutput_ToolButton.SetChecked( m_lockOutputChannel );
   GUI->ShowCurve_ToolButton.SetChecked( m_showMTF );
   GUI->ShowGrid_ToolButton.SetChecked( m_showGrid );
}

void HistogramTransformationInterface::UpdateChannelControls()
{
   GUI->R_ToolButton.SetChecked( m_channel == 0 );
   GUI->G_ToolButton.SetChecked( m_channel == 1 );
   GUI->B_ToolButton.SetChecked( m_channel == 2 );
   GUI->RGBK_ToolButton.SetChecked( m_channel == 3 );
   GUI->A_ToolButton.SetChecked( m_channel == 4 );
}

void HistogramTransformationInterface::UpdateReadoutModeControls()
{
   GUI->NormalReadout_ToolButton.SetChecked( m_readoutMode == NormalReadout );
   GUI->BlackPointReadout_ToolButton.SetChecked( m_readoutMode == BlackPointReadout );
   GUI->MidtonesReadout_ToolButton.SetChecked( m_readoutMode == MidtonesReadout );
   GUI->WhitePointReadout_ToolButton.SetChecked( m_readoutMode == WhitePointReadout );
}

void HistogramTransformationInterface::UpdateHistogramControls()
{
   GUI->ShadowsClipping_NumericEdit.SetValue( m_instance.ShadowsClipping( m_channel ) );
   GUI->MidtonesBalance_NumericEdit.SetValue( m_instance.MidtonesBalance( m_channel ) );
   GUI->HighlightsClipping_NumericEdit.SetValue( m_instance.HighlightsClipping( m_channel ) );
   GUI->LowRange_NumericControl.SetValue( m_instance.LowRange( m_channel ) );
   GUI->HighRange_NumericControl.SetValue( m_instance.HighRange( m_channel ) );
}

void HistogramTransformationInterface::UpdateClippingCountControls()
{
   if ( m_currentView.IsNull() )
   {
      GUI->ShadowsClippingCount_Label.Clear();
      GUI->HighlightsClippingCount_Label.Clear();
   }
   else
   {
      count_type N  = m_currentView.Image()->NumberOfPixels();
      count_type n0 = m_shadowsCount[m_channel];
      count_type n1 = m_highlightsCount[m_channel];
      GUI->ShadowsClippingCount_Label.SetText( String().Format( "%llu, %%%.4f", n0, (100.0*n0)/N ) );
      GUI->HighlightsClippingCount_Label.SetText( String().Format( "%llu, %%%.4f", n1, (100.0*n1)/N ) );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HistogramTransformationInterface::UpdateHistograms()
{
   UpdateInputHistogram();
   UpdateOutputHistogram();
}

void HistogramTransformationInterface::UpdateInputHistogram()
{
   UpdateHistogramSliders();
   UpdateHistogramInfo();

   m_inputDirty = true;
   GUI->InputHistogramPlot_Control.Update();
}

void HistogramTransformationInterface::UpdateOutputHistogram()
{
   m_outputDirty = true;
   GUI->OutputHistogram_ScrollBox.Viewport().Update();
}

void HistogramTransformationInterface::UpdateHistogramSliders()
{
   m_slidersDirty = true;
   GUI->HistogramSliders_Control.Update();
}

void HistogramTransformationInterface::UpdateHistogramInfo()
{
   if ( m_cursorStatus == NoCursor )
   {
      GUI->Info_Label.Clear();
      return;
   }

   String s;

   int w = (m_cursorStatus == InputCursor) ?
      GUI->InputHistogramPlot_Control.Width()*m_inputZoomX :
      GUI->OutputHistogram_ScrollBox.Viewport().Width()*m_outputZoomX;

   int dx = Max( 1, RoundInt( double( m_plotResolution )/w ) );

   double x = m_histogramPos.x*(m_plotResolution - 1);
   int i = int( x );
   int j = Min( i+dx, m_plotResolution );

   if ( m_mode == ReadoutMode )
   {
      histogram_list H;

      if ( !m_currentView.IsNull() )
         if ( m_cursorStatus == InputCursor )
         {
            if ( !m_rawRGBInput && m_channel == 3 && m_currentView.IsColor() )
            {
               if ( m_outputRGBData.IsEmpty() )
                  CalculateOutputHistograms();

               H = m_outputRGBData;
            }
            else
            {
               if ( m_inputData.IsEmpty() )
                  CalculateInputHistograms();

               H = m_inputData;
            }
         }
         else
         {
            H = m_outputData;
         }

      if ( !H.IsEmpty() )
      {
         bool ch3 = (m_channel == 3 || (m_cursorStatus == OutputCursor && m_lockOutputChannel)) && m_channel != 4;

         count_type total = 0;
         count_type count = 0;

         int alpha = m_currentView.IsColor() ? 3 : 1;
         int c0 = (m_channel < 3) ? m_channel : alpha;

         for ( int c = 0; c < int( H.Length() ); ++c )
            if ( ch3 && c < alpha || c == c0 )
            {
               for ( int k = i; k < j; ++k )
                  count += H[c].Count( k );
               total += H[c].Count();
            }

         s.AppendFormat( " x=%.4f(%d-%d) %llu, %%%.4f | y=%.4f",
                           m_histogramPos.x, i, j-1, count, 100.0*count/total, m_histogramPos.y );
      }
   }

   if ( s.IsEmpty() )
   {
      switch ( m_mode )
      {
      case ReadoutMode:  s = " Readout"; break;
      case ZoomInMode:   s = " Zoom In"; break;
      case ZoomOutMode:  s = " Zoom Out"; break;
      case PanMode:      s = " Pan"; break;
      default: break;
      }

      s.AppendFormat( " | x=%.4f(%d-%d) | y=%.4f",
                        m_histogramPos.x, i, j-1, m_histogramPos.y );
   }

   GUI->Info_Label.SetText( s );
}

void HistogramTransformationInterface::UpdateAutoClippingToolTips()
{
   GUI->ShadowsClippingAutoClip_ToolButton.SetToolTip(
      String().Format( "Auto clip shadows: %%%.3f", m_shadowsAutoClipping*100 ) );
   GUI->HighlightsClippingAutoClip_ToolButton.SetToolTip(
      String().Format( "Auto clip highlights: %%%.3f", m_highlightsAutoClipping*100 ) );
}

void HistogramTransformationInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != nullptr )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

void HistogramTransformationInterface::SynchronizeWithCurrentView()
{
   CalculateInputHistograms();
   CalculateOutputHistograms();
   CalculateClippingCounts();

   UpdateHistograms();
   UpdateClippingCountControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ToggleOutputHistogram()
{
   DisableUpdates();

   if ( m_outputSectionVisible )
   {
      GUI->OutputHistogram_ScrollBox.Hide();
      GUI->ShowOutput_ToolButton.SetIcon( ScaledResource( ":/process-interface/contract-vert.png" ) );
      GUI->ShowOutput_ToolButton.SetToolTip( "Show Output Histograms" );
      GUI->OutputHorizontalZoom_SpinBox.Disable();
      GUI->OutputVerticalZoom_SpinBox.Disable();
      m_outputSectionVisible = false;
   }
   else
   {
      GUI->OutputHistogram_ScrollBox.Show();
      GUI->ShowOutput_ToolButton.SetIcon( ScaledResource( ":/process-interface/expand-vert.png" ) );
      GUI->ShowOutput_ToolButton.SetToolTip( "Hide Output Histograms" );
      GUI->OutputHorizontalZoom_SpinBox.Enable();
      GUI->OutputVerticalZoom_SpinBox.Enable();
      m_outputSectionVisible = true;
   }

   ProcessEvents();

   AdjustToContents();
   EnableUpdates();
}

// ----------------------------------------------------------------------------

void HistogramTransformationInterface::ToggleRangeControls()
{
   DisableUpdates();

   if ( m_rangeSectionVisible )
   {
      GUI->RangeSection_Control.Hide();
      GUI->ShowRangeControls_ToolButton.SetIcon( ScaledResource( ":/process-interface/expand-vert.png" ) );
      GUI->ShowRangeControls_ToolButton.SetToolTip( "Show Dynamic Range Expansion Controls" );
      m_rangeSectionVisible = false;
   }
   else
   {
      GUI->RangeSection_Control.Show();
      GUI->ShowRangeControls_ToolButton.SetIcon( ScaledResource( ":/process-interface/contract-vert.png" ) );
      GUI->ShowRangeControls_ToolButton.SetToolTip( "Hide Dynamic Range Expansion Controls" );
      m_rangeSectionVisible = true;
   }

   ProcessEvents();

   AdjustToContents();
   EnableUpdates();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HistogramTransformationInterface::PlotGrid(
   Graphics& g, const Rect& r, int width, int height, int hZoom, int vZoom )
{
   int n = 8 * Min( hZoom, vZoom );

   double dx = double( width - 1 )/n;
   double dy = double( height - 1 )/n;

   int ix0 = int( r.x0/dx );
   int ix1 = int( r.x1/dx );

   int iy0 = int( r.y0/dy );
   int iy1 = int( r.y1/dy );

   int w = r.Width();
   int h = r.Height();

   Pen p0( m_gridColor0, DisplayPixelRatio(), PenStyle::Solid );
   Pen p1( m_gridColor1, DisplayPixelRatio(), PenStyle::Dot );

   for ( int i = ix0; i <= ix1; ++i )
   {
      int x = RoundInt( dx*i ) - r.x0;
      if ( x >= w )
         break;

      g.SetPen( (i & 1) ? p1 : p0 );
      g.DrawLine( x, 0, x, h );
   }

   for ( int i = iy0; i <= iy1; ++i )
   {
      int y = RoundInt( dy*i ) - r.y0;
      if ( y >= h )
         break;

      g.SetPen( (i & 1) ? p1 : p0 );
      g.DrawLine( 0, y, w, y );
   }
}

void HistogramTransformationInterface::PlotHistogram(
   Graphics& g, const Rect& r, const Histogram& H, count_type peak, int width, int height, int hZoom, int vZoom )
{
   double dx = double( width - 1 )/(H.Resolution() - 1);
   int i0 = int( r.x0/dx );
   int i1 = Min( H.Resolution()-1, int( Ceil( r.x1/dx ) ) );

   Array<Point> points;
   for ( int i = i0, x0 = 0, y0 = 0; i <= i1; ++i )
   {
      int x = RoundInt( i*dx ) - r.x0;
      int y = Max( -1, RoundInt( (height - 1)*(1 - double( H.Count( i ) )/peak) ) - r.y0 ); // -1 to allow clipping at top

      if ( i == i0 || i == i1 || x != x0 ) // always put both extreme points
         points.Add( Point( x0 = x, y0 = y ) );
      else
      {
         if ( y < y0 )
            (*points.ReverseBegin()).y = y0 = y;
      }
   }

   switch ( m_graphStyle )
   {
   default: // ?!
   case LineStyle:
      for ( Array<Point>::const_iterator i0 = points.Begin(), i = i0; ++i < points.End(); i0 = i )
         g.DrawLine( *i0, *i );
      break;

   case AreaStyle:
      points.Add( Point( (*points.ReverseBegin()).x, height-1 ) );
      points.Add( Point( (*points.Begin()).x, height-1 ) );
      g.SetBrush( g.Pen().Color() );
      g.DrawPolygon( points );
      break;

   case BarStyle:
      for ( Array<Point>::const_iterator i = points.Begin(); i != points.End(); ++i )
         g.DrawLine( *i, Point( i->x, height-1 ) );
      break;

   case DotStyle:
      for ( Array<Point>::const_iterator i = points.Begin(); i != points.End(); ++i )
         g.DrawPoint( *i );
      break;
   }
}

void HistogramTransformationInterface::PlotScale( Graphics& g, const Rect& r, int width )
{
   int w = r.Width();
   int h = r.Height();

   bool c0 = m_channel == 0 || m_channel >= 3;
   bool c1 = m_channel == 1 || m_channel >= 3;
   bool c2 = m_channel == 2 || m_channel >= 3;

   float v0 = float( r.x0 )/(width - 1);
   float v1 = float( r.x1 - 1 )/(width - 1);

   GradientBrush::stop_list stops;
   stops.Add( GradientBrush::Stop( 0.0, RGBAColor( c0 ? v0 : 0.0F, c1 ? v0 : 0.0F, c2 ? v0 : 0.0F ) ) );
   stops.Add( GradientBrush::Stop( 1.0, RGBAColor( c0 ? v1 : 0.0F, c1 ? v1 : 0.0F, c2 ? v1 : 0.0F ) ) );

   g.FillRect( 0, 0, w, h, LinearGradientBrush( 0, 0, w, 0, stops ) );
}

RGBA HistogramTransformationInterface::HandlerColor( double v ) const
{
   // Ensure visibility of handlers on R, G, B and gray backgrounds.
   if ( (m_channel == 0 || m_channel == 1 || m_channel >= 3) && v > 0.5 )
      return 0xFF000000;
   return 0xFFFFFFFF;
}

void HistogramTransformationInterface::PlotHandler( Graphics& g, double v, int x0, int width )
{
   int h = RoundInt( LogicalPixelsToPhysical( m_sliderControlSize ) );
   int h2 = (h >> 1) + 1;

   int x = RoundInt( v*(width - 1) ) - x0;

   GenericVector<Point> notch( 4 );
   notch[0] = Point( x,      h-h2 );
   notch[1] = Point( x-h2+1, h-1  );
   notch[2] = Point( x+h2-1, h-1  );
   notch[3] = Point( x,      h-h2 );

   g.SetPen( HandlerColor( v ), DisplayPixelRatio() );
   g.DrawLine( x, 0, x, h-h2-1 );
   g.DrawPolyline( notch );
}

void HistogramTransformationInterface::PlotMidtonesTransferCurve( Graphics& g, const Rect& r, int width, int height )
{
   int xc0 = RoundInt( m_instance.ShadowsClipping( m_channel )*(width - 1) );
   if ( xc0 >= r.x1 )
      return;

   int xc1 = RoundInt( m_instance.HighlightsClipping( m_channel )*(width - 1) );
   if ( xc1 < r.x0 )
      return;

   g.SetPen( m_channelColors[m_channel], DisplayPixelRatio() );

   if ( xc1 - xc0 < 2 )
   {
      g.DrawLine( xc0-r.x0, 0, xc1-r.x0, r.Height()-1 );
      return;
   }

   double dx = 1.0/(xc1 - xc0);
   double m = m_instance.MidtonesBalance( m_channel );

   Array<Point> points;

   int px0 = Max( r.x0-1, xc0 );
   int px1 = Min( r.x1, xc1 );

   for ( int xi = px0, x0 = -1, y0 = -1; xi < px1; ++xi )
   {
      int x = xi - r.x0;
      int y = RoundInt( (height - 1)*(1 - HistogramTransformation::MTF( m, (xi - xc0)*dx )) ) - r.y0;
      if ( x != x0 || y != y0 )
         points.Add( Point( x0 = x, y0 = y ) );
   }

   points.Add( Point( px1 - r.x0,
                      RoundInt( (height - 1)*(1 - HistogramTransformation::MTF( m, (px1 - xc0)*dx )) ) - r.y0 ) );

   g.DrawPolyline( points );
}

void HistogramTransformationInterface::PlotReadouts( Graphics& g,
                        const Bitmap& bmp, const Rect& r, const DVector& readouts, int width, int height )
{
   int w = bmp.Width();
   int h = bmp.Height();
   float d = DisplayPixelRatio();

   if ( m_channel == 3 )
   {
      for ( int c = 0; c < 3; ++c )
      {
         int x = RoundInt( readouts[c]*(width - 1) ) - r.x0;
         if ( x >= 0 && x < w )
         {
            g.SetPen( m_channelColors[c], d );
            g.DrawLine( x, 0, x, h );
         }
      }
   }
   else
   {
      int x = RoundInt( readouts[m_channel]*(width - 1) ) - r.x0;
      if ( x >= 0 && x < w )
      {
         g.SetPen( m_channelColors[m_channel], d );
         g.DrawLine( x, 0, x, h );
      }
   }
}

void HistogramTransformationInterface::PlotCursor( Graphics& g, const Rect& r )
{
   int w = r.Width();
   int h = r.Height();
   int x = m_cursorPos.x - r.x0;
   int y = m_cursorPos.y - r.y0;

   if ( m_mode == ReadoutMode )
   {
      g.SetPen( 0xffffffff, DisplayPixelRatio() );
      if ( x >= 0 && x < w )
         g.DrawLine( x, 0, x, h );
      if ( y >= 0 && y < h )
         g.DrawLine( 0, y, w, y );
   }
   else
   {
      String resource;
      switch ( m_mode )
      {
      case ZoomInMode:
         resource = ":/cursors/view/zoom_in.png";
         break;
      case ZoomOutMode:
         resource = ":/cursors/view/zoom_out.png";
         break;
      case PanMode:
         resource = m_panning ? ":/cursors/view/bidi_pan.png" : ":/cursors/view/pan.png";
         break;
      default: // ?!
         return;
      }

      Bitmap csr( ScaledResource( resource ) );
      g.DrawBitmap( x - (csr.Width() >> 1), y - (csr.Height() >> 1), csr );
   }
}

void HistogramTransformationInterface::RegenerateInputViewport()
{
   Rect r0 = GUI->InputHistogramPlot_Control.BoundsRect();
   int w0 = r0.Width();
   int h0 = r0.Height();

   m_inputDirty = false;

   if ( m_inputBitmap.IsNull() )
      m_inputBitmap = Bitmap( w0, h0, BitmapFormat::RGB32 );
   m_inputBitmap.Fill( m_backgroundColor );

   Rect r( r0 + GUI->InputHistogram_ScrollBox.ScrollPosition() );
   int w = w0*m_inputZoomX;
   int h = h0*m_inputZoomY;

   if ( m_showGrid )
   {
      Graphics g( m_inputBitmap );
      g.EnableAntialiasing();
      PlotGrid( g, r, w, h, m_inputZoomX, m_inputZoomY );
   }

   if ( m_currentView.IsNull() )
      return;

   histogram_list H;

   if ( !m_rawRGBInput && m_channel == 3 && m_currentView.IsColor() )
   {
      if ( m_outputRGBData.IsEmpty() )
         CalculateOutputHistograms();

      H = m_outputRGBData;
   }
   else
   {
      if ( m_inputData.IsEmpty() )
         CalculateInputHistograms();

      H = m_inputData;
   }

   if ( !H.IsEmpty() )
   {
      ImageVariant image = m_currentView.Image();

      Bitmap bmp( w0, h0, BitmapFormat::RGB32 );
      {
         bmp.Fill( 0xFF000000 );
         Graphics g( bmp );
         g.EnableAntialiasing();
         g.SetCompositionOperator( CompositionOp::Screen );

         if ( m_channel == 3 )
         {
            count_type peak = 0;
            if ( m_rejectSaturated )
               for ( size_type i = 0; i < H.Length(); ++i )
                  peak = Max( peak, H[i].PeakCount( 1, H[i].Resolution()-2 ) );
            if ( !m_rejectSaturated || peak == 0 )
               for ( size_type i = 0; i < H.Length(); ++i )
                  peak = Max( peak, H[i].PeakCount() );

            for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
            {
               g.SetPen( m_channelColors[image->IsColor() ? c : 3], DisplayPixelRatio() );
               PlotHistogram( g, r, H[c], peak, w, h, m_inputZoomX, m_inputZoomY );
            }
         }
         else
         {
            if ( m_channel < 3 && image->IsColor() || m_channel == 4 && image->HasAlphaChannels() )
            {
               int c = (m_channel < 3) ? m_channel : image->NumberOfNominalChannels();

               count_type peak = 0;
               if ( m_rejectSaturated )
                  peak = H[c].PeakCount( 1, H[c].Resolution()-2 );
               if ( !m_rejectSaturated || peak == 0 )
                  peak = H[c].PeakCount();

               g.SetPen( m_channelColors[m_channel], DisplayPixelRatio() );
               PlotHistogram( g, r, H[c], peak, w, h, m_inputZoomX, m_inputZoomY );
            }
         }
      }

      Graphics g( m_inputBitmap );
      g.SetCompositionOperator( CompositionOp::Difference );
      g.DrawBitmap( 0, 0, bmp );
   }
}

void HistogramTransformationInterface::RegenerateOutputViewport()
{
   Rect r0 = GUI->OutputHistogram_ScrollBox.Viewport().BoundsRect();
   int w0 = r0.Width();
   int h0 = r0.Height();

   m_outputDirty = false;

   if ( m_outputBitmap.IsNull() )
      m_outputBitmap = Bitmap( w0, h0, BitmapFormat::RGB32 );
   m_outputBitmap.Fill( m_backgroundColor );

   Rect r( r0 + GUI->OutputHistogram_ScrollBox.ScrollPosition() );
   int w = w0*m_outputZoomX;
   int h = h0*m_outputZoomY;

   if ( m_showGrid )
   {
      Graphics g( m_outputBitmap );
      g.EnableAntialiasing();
      PlotGrid( g, r, w, h, m_outputZoomX, m_outputZoomY );
   }

   if ( m_currentView.IsNull() )
      return;

   if ( m_outputData.IsEmpty() )
      CalculateOutputHistograms();

   if ( !m_outputData.IsEmpty() )
   {
      ImageVariant image = m_currentView.Image();

      Bitmap bmp( w0, h0, BitmapFormat::RGB32 );
      {
         bmp.Fill( 0xFF000000 );
         Graphics g( bmp );
         g.EnableAntialiasing();
         g.SetCompositionOperator( CompositionOp::Screen );

         if ( m_channel == 3 || m_lockOutputChannel && m_channel != 4 )
         {
            count_type peak = 0;
            if ( m_rejectSaturated )
               for ( size_type i = 0; i < m_outputData.Length(); ++i )
                  peak = Max( peak, m_outputData[i].PeakCount( 1, m_outputData[i].Resolution()-2 ) );
            if ( !m_rejectSaturated || peak == 0 )
               for ( size_type i = 0; i < m_outputData.Length(); ++i )
                  peak = Max( peak, m_outputData[i].PeakCount() );

            for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
            {
               g.SetPen( m_channelColors[image->IsColor() ? c : 3], DisplayPixelRatio() );
               PlotHistogram( g, r, m_outputData[c], peak, w, h, m_outputZoomX, m_outputZoomY );
            }
         }
         else
         {
            if ( m_channel < 3 && image->IsColor() || m_channel == 4 && image->HasAlphaChannels() )
            {
               int c = (m_channel < 3) ? m_channel : image->NumberOfNominalChannels();

               count_type peak = 0;
               if ( m_rejectSaturated )
                  peak = m_outputData[c].PeakCount( 1, m_outputData[c].Resolution()-2 );
               if ( !m_rejectSaturated || peak == 0 )
                  peak = m_outputData[c].PeakCount();

               g.SetPen( m_channelColors[m_channel], DisplayPixelRatio() );
               PlotHistogram( g, r, m_outputData[c], peak, w, h, m_outputZoomX, m_outputZoomY );
            }
         }
      }

      Graphics g( m_outputBitmap );
      g.SetCompositionOperator( CompositionOp::Difference );
      g.DrawBitmap( 0, 0, bmp );
   }
}

void HistogramTransformationInterface::RegenerateSlidersViewport()
{
   Rect r0 = GUI->HistogramSliders_Control.BoundsRect();
   int w0 = r0.Width();
   int h0 = r0.Height();

   if ( m_slidersBitmap.IsNull() )
      m_slidersBitmap = Bitmap( w0, h0, BitmapFormat::RGB32 );

   m_slidersDirty = false;

   Graphics g( m_slidersBitmap );
   Rect r( r0 );
   r += GUI->InputHistogram_ScrollBox.ScrollPosition();
   PlotScale( g, r, w0*m_inputZoomX );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HistogramTransformationInterface::__ViewList_ViewSelected( ViewList&, View& )
{
   DeactivateTrackView();

   if ( !m_currentView.IsNull() && !m_currentView.HasProperty( "Histogram16" ) )
      m_currentView.ComputeProperty( "Histogram16" );
   else
      SynchronizeWithCurrentView();
}

void HistogramTransformationInterface::__Histogram_Paint( Control& sender, const pcl::Rect& updateRect )
{
   if ( sender == GUI->InputHistogramPlot_Control )
   {
      if ( m_inputDirty )
         RegenerateInputViewport();

      if ( m_showMTF || m_readoutActive || m_cursorStatus == InputCursor )
      {
         Bitmap bmp = m_inputBitmap.Subimage( updateRect );
         {
            Graphics g( bmp );
            g.EnableAntialiasing();
            g.SetCompositionOperator( CompositionOp::Difference );

            if ( m_showMTF || m_readoutActive )
            {
               Rect r0 = sender.ClientRect();
               int w = r0.Width()*m_inputZoomX;
               int h = r0.Height()*m_inputZoomY;
               Rect r( updateRect + GUI->InputHistogram_ScrollBox.ScrollPosition() );
               if ( m_showMTF )
                  PlotMidtonesTransferCurve( g, r, w, h );
               if ( m_readoutActive )
                  PlotReadouts( g, bmp, r, m_inputReadouts, w, h );
            }

            if ( m_cursorStatus == InputCursor )
               PlotCursor( g, updateRect );
         }

         Graphics g( sender );
         g.DrawBitmap( updateRect.LeftTop(), bmp );
      }
      else
      {
         Graphics g( sender );
         g.DrawBitmapRect( updateRect.LeftTop(), m_inputBitmap, updateRect );
      }
   }
   else if ( sender == GUI->OutputHistogram_ScrollBox.Viewport() )
   {
      if ( m_outputDirty )
         RegenerateOutputViewport();

      if ( m_readoutActive || m_cursorStatus == OutputCursor )
      {
         Bitmap bmp = m_outputBitmap.Subimage( updateRect );
         {
            Graphics g( bmp );
            g.EnableAntialiasing();

            if ( m_readoutActive )
            {
               Rect r0 = sender.ClientRect();
               int w = r0.Width()*m_outputZoomX;
               int h = r0.Height()*m_outputZoomY;
               Rect r( updateRect + GUI->OutputHistogram_ScrollBox.ScrollPosition() );
               PlotReadouts( g, bmp, r, m_outputReadouts, w, h );
            }

            if ( m_cursorStatus == OutputCursor )
               PlotCursor( g, updateRect );
         }

         Graphics g( sender );
         g.DrawBitmap( updateRect.LeftTop(), bmp );
      }
      else
      {
         Graphics g( sender );
         g.DrawBitmapRect( updateRect.LeftTop(), m_outputBitmap, updateRect );
      }
   }
}

void HistogramTransformationInterface::__Sliders_Paint( Control& sender, const pcl::Rect& updateRect )
{
   if ( m_slidersDirty )
      RegenerateSlidersViewport();

   double c0 = m_instance.ShadowsClipping( m_channel );
   double m = m_instance.MidtonesBalance( m_channel );
   double c1 = m_instance.HighlightsClipping( m_channel );

   int w = sender.Width()*m_inputZoomX;
   int x0 = GUI->InputHistogram_ScrollBox.HorizontalScrollPosition();

   Graphics g( sender );
   g.EnableAntialiasing();
   g.DrawBitmapRect( updateRect.LeftTop(), m_slidersBitmap, updateRect );
   PlotHandler( g, c0, x0, w );
   PlotHandler( g, c0 + m*(c1 - c0), x0, w );
   PlotHandler( g, c1, x0, w );
}

void HistogramTransformationInterface::__Histogram_Resize( Control& sender,
                                             int/*newWidth*/, int/*newHeight*/, int/*oldWidth*/, int/*oldHeight*/ )
{
   if ( sender == GUI->InputHistogramPlot_Control )
   {
      m_inputBitmap = Bitmap::Null();
      m_inputDirty = true;

      if ( !m_settingUp )
         SetInputZoom( m_inputZoomX, m_inputZoomY );
   }
   else if ( sender == GUI->OutputHistogram_ScrollBox.Viewport() )
   {
      m_outputBitmap = Bitmap::Null();
      m_outputDirty = true;

      if ( !m_settingUp )
         SetOutputZoom( m_outputZoomX, m_outputZoomY );
   }
   else if ( sender == GUI->HistogramSliders_Control )
   {
      m_slidersBitmap = Bitmap::Null();
      m_slidersDirty = true;
   }
}

void HistogramTransformationInterface::__Histogram_ScrollPosUpdated( ScrollBox& sender, int pos )
{
   if ( sender == GUI->InputHistogram_ScrollBox )
      UpdateInputHistogram();
   else if ( sender == GUI->OutputHistogram_ScrollBox )
      UpdateOutputHistogram();
}

void HistogramTransformationInterface::__Histogram_Enter( Control& sender )
{
   if ( sender == GUI->InputHistogramPlot_Control )
      m_cursorStatus = InputCursor;
   else if ( sender == GUI->OutputHistogram_ScrollBox.Viewport() )
      m_cursorStatus = OutputCursor;
   m_cursorPos = -1;
}

void HistogramTransformationInterface::__Histogram_Leave( Control& sender )
{
   m_cursorStatus = NoCursor;
   UpdateHistogramInfo();
   sender.Update();
}

void HistogramTransformationInterface::__Histogram_MousePress( Control& sender,
                                             const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button == MouseButton::Left )
   {
      switch ( m_mode )
      {
      case ZoomInMode:
      case ZoomOutMode:
         if ( sender == GUI->InputHistogramPlot_Control )
         {
            Point p = pos + GUI->InputHistogram_ScrollBox.ScrollPosition();
            p.x /= m_inputZoomX;
            p.y /= m_inputZoomY;
            if ( m_mode == ZoomInMode )
               SetInputZoom( Min( m_inputZoomX+1, s_maxZoom ), Min( m_inputZoomY+1, s_maxZoom ), &p );
            else
               SetInputZoom( Max( 1, m_inputZoomX-1 ), Max( 1, m_inputZoomY-1 ), &p );
         }
         else if ( sender == GUI->OutputHistogram_ScrollBox.Viewport() )
         {
            Point p = pos + GUI->OutputHistogram_ScrollBox.ScrollPosition();
            p.x /= m_outputZoomX;
            p.y /= m_outputZoomY;
            if ( m_mode == ZoomInMode )
               SetOutputZoom( Min( m_outputZoomX+1, s_maxZoom ), Min( m_outputZoomY+1, s_maxZoom ), &p );
            else
               SetOutputZoom( Max( 1, m_outputZoomX-1 ), Max( 1, m_outputZoomY-1 ), &p );
         }
         break;

      case PanMode:
         m_panOrigin = pos;
         ++m_panning;
         break;

      default:
         break;
      }
   }
   else if ( button == MouseButton::Middle )
   {
      if ( m_savedMode == NoMode )
      {
         m_savedMode = m_mode;
         SetMode( PanMode );
         m_panOrigin = pos;
         ++m_panning;
      }
   }
}

void HistogramTransformationInterface::__Histogram_MouseRelease( Control& sender,
                                             const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button == MouseButton::Middle && m_savedMode != NoMode )
   {
      SetMode( m_savedMode );
      m_savedMode = NoMode;
   }

   m_panning = 0;
}

void HistogramTransformationInterface::__Histogram_MouseMove( Control& sender,
                                             const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_cursorStatus != NoCursor )
   {
      Rect r = sender.ClientRect();
      int w = r.Width();
      int h = r.Height();

      for ( int i = 0; i < 2; ++i )
      {
         double f = DisplayPixelRatio();
         int ui1 = RoundInt( f );
         if ( m_mode == ReadoutMode )
         {
            sender.Update( m_cursorPos.x-ui1-ui1, 0, m_cursorPos.x+ui1+ui1, h );
            sender.Update( 0, m_cursorPos.y-ui1-ui1, w, m_cursorPos.y+ui1+ui1 );
         }
         else
         {
            int ui16 = RoundInt( f*16 );
            sender.Update( m_cursorPos.x-ui16-ui1, m_cursorPos.y-ui16-ui1, m_cursorPos.x+ui16+ui1, m_cursorPos.y+ui16+ui1 );
         }

         if ( i == 0 )
            m_cursorPos = pos;
      }

      if ( m_cursorStatus == InputCursor )
      {
         w *= m_inputZoomX;
         h *= m_inputZoomY;
      }
      else
      {
         w *= m_outputZoomX;
         h *= m_outputZoomY;
      }

      Point p = pos + ((m_cursorStatus == InputCursor) ?
                        GUI->InputHistogram_ScrollBox.ScrollPosition() :
                        GUI->OutputHistogram_ScrollBox.ScrollPosition());

      m_histogramPos.x = Range( double( p.x )/(w - 1), 0.0, 1.0 );
      m_histogramPos.y = Range( 1 - double( p.y )/(h - 1), 0.0, 1.0 );

      UpdateHistogramInfo();
   }

   if ( m_panning )
   {
      Point d = m_panOrigin - pos;
      m_panOrigin = pos;
      if ( sender == GUI->InputHistogramPlot_Control )
         GUI->InputHistogram_ScrollBox.SetScrollPosition( GUI->InputHistogram_ScrollBox.ScrollPosition() + d );
      else
         GUI->OutputHistogram_ScrollBox.SetScrollPosition( GUI->OutputHistogram_ScrollBox.ScrollPosition() + d );
   }
}

void HistogramTransformationInterface::__Histogram_MouseWheel( Control& sender,
                                             const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   m_wheelSteps += delta; // delta is rotation angle in 1/8 degree steps
   if ( Abs( m_wheelSteps ) >= WHEEL_STEP_ANGLE*8 )
   {
      int d = (delta > 0) ? -1 : +1;

      if ( sender == GUI->InputHistogramPlot_Control )
      {
         Point p = pos + GUI->InputHistogram_ScrollBox.ScrollPosition();
         p.x /= m_inputZoomX;
         p.y /= m_inputZoomY;
         p.y += 8; // favor bottom histogram locations
         SetInputZoom( Range( m_inputZoomX+d, 1, s_maxZoom ), Range( m_inputZoomY+d, 1, s_maxZoom ), &p );
      }
      else if ( sender == GUI->OutputHistogram_ScrollBox.Viewport() )
      {
         Point p = pos + GUI->OutputHistogram_ScrollBox.ScrollPosition();
         p.x /= m_outputZoomX;
         p.y /= m_outputZoomY;
         p.y += 8; // favor bottom histogram locations
         SetOutputZoom( Range( m_outputZoomX+d, 1, s_maxZoom ), Range( m_outputZoomY+d, 1, s_maxZoom ), &p );
      }

      m_wheelSteps = 0;
   }
}

HistogramTransformationInterface::slider_id HistogramTransformationInterface::FindHandler( double v ) const
{
   double c0 = m_instance.ShadowsClipping( m_channel );
   double c1 = m_instance.HighlightsClipping( m_channel );
   double cm = c0 + m_instance.MidtonesBalance( m_channel )*(c1 - c0);

   double dcm = Abs( v - cm );
   double dc0 = Abs( v - c0 );
   double dc1 = Abs( v - c1 );

   if ( dcm < dc0 )
   {
      if ( dcm < dc1 )
         return MSlider;
   }

   if ( dc0 < dcm )
   {
      if ( dc0 < dc1 )
         return C0Slider;
   }

   return C1Slider;
}

double HistogramTransformationInterface::SliderToHistogram( int x ) const
{
   return double( x + GUI->InputHistogram_ScrollBox.HorizontalScrollPosition() ) /
               (GUI->HistogramSliders_Control.Width()*m_inputZoomX - 1);
}

void HistogramTransformationInterface::__Sliders_MousePress( Control& sender,
                                             const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   m_sliderBeingDragged = FindHandler( SliderToHistogram( pos.x ) );

   __Sliders_MouseMove( sender, pos, buttons, modifiers );
}

void HistogramTransformationInterface::__Sliders_MouseRelease(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   __Sliders_MouseMove( sender, pos, buttons, modifiers );
   m_sliderBeingDragged = NoSlider;
}

void HistogramTransformationInterface::__Sliders_MouseMove( Control& sender,
                                             const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_sliderBeingDragged != NoSlider )
   {
      double v = SliderToHistogram( pos.x );

      double c0 = m_instance.ShadowsClipping( m_channel );
      double c1 = m_instance.HighlightsClipping( m_channel );

      if ( m_sliderBeingDragged == MSlider )
      {
         SetMidtonesBalance( Round( (c0 != c1) ? (Range( v, c0, c1 ) - c0)/(c1 - c0) : c0,
                             TheMidtonesBalanceParameter->Precision() ) );
      }
      else if ( m_sliderBeingDragged == C0Slider )
      {
         v = Round( v, TheShadowsClippingParameter->Precision() );
         SetClippingParameters( v, Max( v, c1 ) );
      }
      else if ( m_sliderBeingDragged == C1Slider )
      {
         v = Round( v, TheHighlightsClippingParameter->Precision() );
         SetClippingParameters( Min( c0, v ), v );
      }

      ProcessEvents();
   }
}

void HistogramTransformationInterface::__HistogramParameter_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ShadowsClipping_NumericEdit )
      SetShadowsClipping( value );
   else if ( sender == GUI->HighlightsClipping_NumericEdit )
      SetHighlightsClipping( value );
   else if ( sender == GUI->MidtonesBalance_NumericEdit )
      SetMidtonesBalance( value );
   else if ( sender == GUI->LowRange_NumericControl )
      SetLowRange( value );
   else if ( sender == GUI->HighRange_NumericControl )
      SetHighRange( value );
}

void HistogramTransformationInterface::__Reset_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->ShadowsClippingReset_ToolButton )
      SetShadowsClipping( 0 );
   else if ( sender == GUI->HighlightsClippingReset_ToolButton )
      SetHighlightsClipping( 1 );
   else if ( sender == GUI->MidtonesBalanceReset_ToolButton )
      SetMidtonesBalance( 0.5 );
   else if ( sender == GUI->LowRangeReset_ToolButton )
      SetLowRange( 0 );
   else if ( sender == GUI->HighRangeReset_ToolButton )
      SetHighRange( 1 );
}

void HistogramTransformationInterface::__AutoZero_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( m_sourceData.IsEmpty() )
      if ( !GetSourceHistograms() )
         return;

   ImageVariant image = m_currentView.Image();

   for ( int c = 0; c < image->NumberOfChannels(); ++c )
      if ( m_channel == 3 && c < image->NumberOfNominalChannels() || image->IsColor() && c == m_channel )
      {
         int r1 = m_sourceData[c].Resolution() - 1;

         if ( sender == GUI->ShadowsClippingAutoZero_ToolButton )
         {
            int c0;
            for ( c0 = 0; c0 < r1; ++c0 )
               if ( m_sourceData[c].Count( c0 ) != 0 )
                  break;
            m_instance.SetShadowsClipping( image->IsColor() ? c : 3, double( c0 )/r1 );
         }
         else if ( sender == GUI->HighlightsClippingAutoZero_ToolButton )
         {
            int c1;
            for ( c1 = r1; c1 > 0; --c1 )
               if ( m_sourceData[c].Count( c1 ) != 0 )
                  break;
            m_instance.SetHighlightsClipping( image->IsColor() ? c : 3, double( c1 )/r1 );
         }
      }

   CalculateOutputHistograms();
   CalculateClippingCounts();

   UpdateHistogramControls();
   UpdateHistograms();
   UpdateClippingCountControls();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::__AutoClip_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( m_sourceData.IsEmpty() )
      if ( !GetSourceHistograms() )
         return;

   ImageVariant image = m_currentView.Image();

   count_type N = image->NumberOfPixels();
   count_type N0 = count_type( m_shadowsAutoClipping*N );
   count_type N1 = count_type( m_highlightsAutoClipping*N );

   for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
      if ( m_channel == 3 && c < image->NumberOfNominalChannels() || image->IsColor() && c == m_channel )
      {
         int r1 = m_sourceData[c].Resolution() - 1;
         count_type n = 0;
         if ( sender == GUI->ShadowsClippingAutoClip_ToolButton )
         {
            int c0;
            for ( c0 = 0; c0 < r1; ++c0 )
               if ( (n += m_sourceData[c].Count( c0 )) >= N0 )
                  break;
            m_instance.SetShadowsClipping( image->IsColor() ? c : 3, double( c0 )/r1 );
         }
         else if ( sender == GUI->HighlightsClippingAutoClip_ToolButton )
         {
            int c1;
            for ( c1 = r1; c1 > 0; --c1 )
               if ( (n += m_sourceData[c].Count( c1 )) >= N1 )
                  break;
            m_instance.SetHighlightsClipping( image->IsColor() ? c : 3, double( c1 )/r1 );
         }
      }

   CalculateOutputHistograms();
   CalculateClippingCounts();

   UpdateHistogramControls();
   UpdateHistograms();
   UpdateClippingCountControls();
   UpdateRealTimePreview();
}

void HistogramTransformationInterface::__AutoClipSetup_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   HistogramAutoClipSetupDialog dlg;
   dlg.Execute();
}

void HistogramTransformationInterface::__Mode_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->ReadoutMode_ToolButton )
      SetMode( ReadoutMode );
   else if ( sender == GUI->ZoomInMode_ToolButton )
      SetMode( ZoomInMode );
   else if ( sender == GUI->ZoomOutMode_ToolButton )
      SetMode( ZoomOutMode );
   else if ( sender == GUI->PanMode_ToolButton )
      SetMode( PanMode );
}

void HistogramTransformationInterface::__ReadoutMode_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->NormalReadout_ToolButton )
      SetReadoutMode( NormalReadout );
   else if ( sender == GUI->BlackPointReadout_ToolButton )
      SetReadoutMode( BlackPointReadout );
   else if ( sender == GUI->MidtonesReadout_ToolButton )
      SetReadoutMode( MidtonesReadout );
   else if ( sender == GUI->WhitePointReadout_ToolButton )
      SetReadoutMode( WhitePointReadout );
}

void HistogramTransformationInterface::__Channel_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->R_ToolButton )
      SetChannel( 0 );
   else if ( sender == GUI->G_ToolButton )
      SetChannel( 1 );
   else if ( sender == GUI->B_ToolButton )
      SetChannel( 2 );
   else if ( sender == GUI->RGBK_ToolButton )
      SetChannel( 3 );
   else if ( sender == GUI->A_ToolButton )
      SetChannel( 4 );
}

void HistogramTransformationInterface::__Zoom_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Zoom11_ToolButton )
   {
      SetInputZoom( 1, 1 );
      SetOutputZoom( 1, 1 );
   }
}

void HistogramTransformationInterface::__Zoom_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->HorizontalZoom_SpinBox )
      SetInputZoom( value, m_inputZoomY );
   else if ( sender == GUI->VerticalZoom_SpinBox )
      SetInputZoom( m_inputZoomX, value );
   else if ( sender == GUI->OutputHorizontalZoom_SpinBox )
      SetOutputZoom( value, m_outputZoomY );
   else if ( sender == GUI->OutputVerticalZoom_SpinBox )
      SetOutputZoom( m_outputZoomX, value );
}

void HistogramTransformationInterface::__PlotResolution_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   if ( itemIndex >= 0 )
      SetPlotResolution( s_plotResolutions[itemIndex] );
}

void HistogramTransformationInterface::__GraphStyle_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   SetGraphStyle( graph_style( itemIndex ) );
}

void HistogramTransformationInterface::__RejectSaturated_ButtonClick( Button& /*sender*/, bool checked )
{
   SetRejectSaturated( checked );
}

void HistogramTransformationInterface::__ShowRawRGB_ButtonClick( Button& /*sender*/, bool checked )
{
   m_rawRGBInput = checked;
   if ( !m_currentView.IsNull() && m_currentView.IsColor() && m_channel == 3 )
      UpdateInputHistogram();
}

void HistogramTransformationInterface::__LockOutput_ButtonClick( Button& /*sender*/, bool checked )
{
   m_lockOutputChannel = checked;
   UpdateOutputHistogram();
}

void HistogramTransformationInterface::__ShowCurve_ButtonClick( Button& /*sender*/, bool checked )
{
   m_showMTF = checked;
   UpdateInputHistogram();
}

void HistogramTransformationInterface::__ShowGrid_ButtonClick( Button& /*sender*/, bool checked )
{
   m_showGrid = checked;
   UpdateHistograms();
}

void HistogramTransformationInterface::__ToggleExtension_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->ShowOutput_ToolButton )
      ToggleOutputHistogram();
   else if ( sender == GUI->ShowRangeControls_ToolButton )
      ToggleRangeControls();
}

void HistogramTransformationInterface::__KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   bool modify =
#if defined( __PCL_MACOSX )
      (modifiers & KeyModifier::Shift) != 0;
#else
      (modifiers & KeyModifier::Alt) != 0;
#endif

   bool spaceBar = (modifiers & KeyModifier::SpaceBar) != 0;

   wantsKey = false;

   switch ( key )
   {
#if defined( __PCL_MACOSX )
   case KeyCode::Shift:
#else
   case KeyCode::Alt:
#endif
      switch ( m_mode )
      {
      case ZoomInMode:
         if ( m_savedMode == NoMode || spaceBar )
         {
            SetMode( ZoomOutMode );
            if ( m_savedMode == NoMode )
               m_savedMode = m_mode;
            wantsKey = true;
         }
         break;

      case ZoomOutMode:
         if ( m_savedMode == NoMode )
         {
            m_savedMode = m_mode;
            SetMode( ZoomInMode );
            wantsKey = true;
         }
         break;

      default:
         break;
      }
      break;

#if defined( __PCL_MACOSX )
   case KeyCode::Alt:
#else
   case KeyCode::Control:
#endif
      if ( spaceBar )
      {
         SetMode( modify ? ZoomOutMode : ZoomInMode );
         wantsKey = true;
      }
      break;

   case KeyCode::Space:
      if ( m_savedMode == NoMode )
      {
         m_savedMode = m_mode;
         SetMode(
#if defined( __PCL_MACOSX )
         ((modifiers & KeyModifier::Alt) != 0) ?
#else
         ((modifiers & KeyModifier::Control) != 0) ?
#endif
            (modify ? ZoomOutMode : ZoomInMode) : PanMode );
      }
      wantsKey = true;
      break;
   }
}

void HistogramTransformationInterface::__KeyRelease( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   bool spaceBar = (modifiers & KeyModifier::SpaceBar) != 0;

   wantsKey = false;

   switch ( key )
   {
#if defined( __PCL_MACOSX )
   case KeyCode::Shift:
#else
   case KeyCode::Alt:
#endif
      switch ( m_mode )
      {
      case ZoomInMode:
         if ( m_savedMode == ZoomOutMode )
         {
            SetMode( ZoomOutMode );
            wantsKey = true;
         }
         break;

      case ZoomOutMode:
         if ( m_savedMode == ZoomInMode || spaceBar )
         {
            SetMode( ZoomInMode );
            wantsKey = true;
         }
         break;

      default:
         break;
      }

      if ( !spaceBar )
         m_savedMode = NoMode;

      break;

#if defined( __PCL_MACOSX )
   case KeyCode::Alt:
#else
   case KeyCode::Control:
#endif
      if ( spaceBar )
      {
         SetMode( PanMode );
         wantsKey = true;
      }
      break;

   case KeyCode::Space:
      if ( !spaceBar && m_savedMode != NoMode ) // avoid auto-repeat kbd. events
      {
         SetMode( m_savedMode );
         m_savedMode = NoMode;
         wantsKey = true;
      }
      break;
   }
}

void HistogramTransformationInterface::__ViewDrag( Control& sender,
                                             const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllViews_ViewList )
      wantsView = true;
}

void HistogramTransformationInterface::__ViewDrop( Control& sender,
                                             const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->AllViews_ViewList )
   {
      DeactivateTrackView();
      GUI->AllViews_ViewList.SelectView( view );
      if ( !m_currentView.HasProperty( "Histogram16" ) )
         m_currentView.ComputeProperty( "Histogram16" );
      SynchronizeWithCurrentView();
   }
}

void HistogramTransformationInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != nullptr )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

HistogramTransformationInterface::GUIData::GUIData( HistogramTransformationInterface& w )
{
   int ui16 = w.LogicalPixelsToPhysical( 16 );
   int ui32 = w.LogicalPixelsToPhysical( 32 );
   pcl::Font font = w.Font();
   int labelWidth = font.Width( "High range:" ) + font.Width( 'T' );
   int channelLabelWidth = ui32 + font.Width( 'M' );
   int rgbkLabelWidth = ui32 + font.Width( "RGB/K" );

   //

   OutputHistogram_ScrollBox.DisableAutoScroll();
   OutputHistogram_ScrollBox.SetScaledMinSize( w.m_minHistogramWidth, w.m_minHistogramHeight );
   OutputHistogram_ScrollBox.OnHorizontalScrollPosUpdated( (ScrollBox::pos_event_handler)&HistogramTransformationInterface::__Histogram_ScrollPosUpdated, w );
   OutputHistogram_ScrollBox.OnVerticalScrollPosUpdated( (ScrollBox::pos_event_handler)&HistogramTransformationInterface::__Histogram_ScrollPosUpdated, w );

   OutputHistogram_ScrollBox.Viewport().SetCursor( StdCursor::NoCursor );
   OutputHistogram_ScrollBox.Viewport().OnPaint( (Control::paint_event_handler)&HistogramTransformationInterface::__Histogram_Paint, w );
   OutputHistogram_ScrollBox.Viewport().OnResize( (Control::resize_event_handler)&HistogramTransformationInterface::__Histogram_Resize, w );
   OutputHistogram_ScrollBox.Viewport().OnEnter( (Control::event_handler)&HistogramTransformationInterface::__Histogram_Enter, w );
   OutputHistogram_ScrollBox.Viewport().OnLeave( (Control::event_handler)&HistogramTransformationInterface::__Histogram_Leave, w );
   OutputHistogram_ScrollBox.Viewport().OnMouseMove( (Control::mouse_event_handler)&HistogramTransformationInterface::__Histogram_MouseMove, w );
   OutputHistogram_ScrollBox.Viewport().OnMousePress( (Control::mouse_button_event_handler)&HistogramTransformationInterface::__Histogram_MousePress, w );
   OutputHistogram_ScrollBox.Viewport().OnMouseRelease( (Control::mouse_button_event_handler)&HistogramTransformationInterface::__Histogram_MouseRelease, w );
   OutputHistogram_ScrollBox.Viewport().OnMouseWheel( (Control::mouse_wheel_event_handler)&HistogramTransformationInterface::__Histogram_MouseWheel, w );

   if ( !w.m_outputSectionVisible )
      OutputHistogram_ScrollBox.Hide();

   //

   ReadoutMode_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-readout.png" ) );
   ReadoutMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ReadoutMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ReadoutMode_ToolButton.SetToolTip( "Readout mode" );
   ReadoutMode_ToolButton.SetCheckable();
   ReadoutMode_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Mode_ButtonClick, w );

   ZoomInMode_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-zoom-in.png" ) );
   ZoomInMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomInMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomInMode_ToolButton.SetToolTip( "Zoom In mode "
#ifdef __PCL_MACOSX
      "[Alt+SpaceBar]" );
#else
      "[Ctrl+SpaceBar]" );
#endif
   ZoomInMode_ToolButton.SetCheckable();
   ZoomInMode_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Mode_ButtonClick, w );

   ZoomOutMode_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-zoom-out.png" ) );
   ZoomOutMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomOutMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomOutMode_ToolButton.SetToolTip( "Zoom Out mode "
#ifdef __PCL_MACOSX
      "[Shift+Alt+SpaceBar]" );
#else
      "[Shift+Ctrl+SpaceBar]" );
#endif
   ZoomOutMode_ToolButton.SetCheckable();
   ZoomOutMode_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Mode_ButtonClick, w );

   PanMode_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-pan.png" ) );
   PanMode_ToolButton.SetScaledFixedSize( 20, 20 );
   PanMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PanMode_ToolButton.SetToolTip( "Pan mode [Spacebar]" );
   PanMode_ToolButton.SetCheckable();
   PanMode_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Mode_ButtonClick, w );

   HorizontalZoom_SpinBox.SetRange( 1, s_maxZoom );
   HorizontalZoom_SpinBox.SetToolTip( "Horizontal zoom, input histogram" );
   HorizontalZoom_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HistogramTransformationInterface::__Zoom_ValueUpdated, w );

   VerticalZoom_SpinBox.SetRange( 1, s_maxZoom );
   VerticalZoom_SpinBox.SetToolTip( "Vertical zoom, input histogram" );
   VerticalZoom_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HistogramTransformationInterface::__Zoom_ValueUpdated, w );

   Zoom11_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/view-zoom-1-1.png" ) );
   Zoom11_ToolButton.SetScaledFixedSize( 20, 20 );
   Zoom11_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Zoom11_ToolButton.SetToolTip( "Zoom 1:1" );
   Zoom11_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Zoom_ButtonClick, w );

   ShowOutput_ToolButton.SetIcon( w.ScaledResource( ":/process-interface/expand-vert.png" ) );
   ShowOutput_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowOutput_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowOutput_ToolButton.SetToolTip( "Hide output histogram" );
   ShowOutput_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ToggleExtension_ButtonClick, w );

   OutputHorizontalZoom_SpinBox.SetRange( 1, s_maxZoom );
   OutputHorizontalZoom_SpinBox.SetToolTip( "Horizontal zoom, output histogram" );
   OutputHorizontalZoom_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HistogramTransformationInterface::__Zoom_ValueUpdated, w );

   OutputVerticalZoom_SpinBox.SetRange( 1, s_maxZoom );
   OutputVerticalZoom_SpinBox.SetToolTip( "Vertical zoom, output histogram" );
   OutputVerticalZoom_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&HistogramTransformationInterface::__Zoom_ValueUpdated, w );

   Mode_Sizer.SetSpacing( 4 );
   Mode_Sizer.Add( ReadoutMode_ToolButton );
   Mode_Sizer.Add( ZoomInMode_ToolButton );
   Mode_Sizer.Add( ZoomOutMode_ToolButton );
   Mode_Sizer.Add( PanMode_ToolButton );
   Mode_Sizer.Add( HorizontalZoom_SpinBox );
   Mode_Sizer.Add( VerticalZoom_SpinBox );
   Mode_Sizer.Add( Zoom11_ToolButton );
   Mode_Sizer.AddStretch();
   Mode_Sizer.Add( ShowOutput_ToolButton );
   Mode_Sizer.Add( OutputHorizontalZoom_SpinBox );
   Mode_Sizer.Add( OutputVerticalZoom_SpinBox );

   //

   InputHistogramPlot_Control.EnableMouseTracking();
   InputHistogramPlot_Control.SetCursor( StdCursor::NoCursor );
   InputHistogramPlot_Control.OnPaint( (Control::paint_event_handler)&HistogramTransformationInterface::__Histogram_Paint, w );
   InputHistogramPlot_Control.OnResize( (Control::resize_event_handler)&HistogramTransformationInterface::__Histogram_Resize, w );
   InputHistogramPlot_Control.OnEnter( (Control::event_handler)&HistogramTransformationInterface::__Histogram_Enter, w );
   InputHistogramPlot_Control.OnLeave( (Control::event_handler)&HistogramTransformationInterface::__Histogram_Leave, w );
   InputHistogramPlot_Control.OnMouseMove( (Control::mouse_event_handler)&HistogramTransformationInterface::__Histogram_MouseMove, w );
   InputHistogramPlot_Control.OnMousePress( (Control::mouse_button_event_handler)&HistogramTransformationInterface::__Histogram_MousePress, w );
   InputHistogramPlot_Control.OnMouseRelease( (Control::mouse_button_event_handler)&HistogramTransformationInterface::__Histogram_MouseRelease, w );
   InputHistogramPlot_Control.OnMouseWheel( (Control::mouse_wheel_event_handler)&HistogramTransformationInterface::__Histogram_MouseWheel, w );

   HistogramSliders_Control.EnableMouseTracking();
   HistogramSliders_Control.SetCursor( StdCursor::UpArrow );
   HistogramSliders_Control.SetScaledFixedHeight( w.m_sliderControlSize );
   HistogramSliders_Control.OnPaint( (Control::paint_event_handler)&HistogramTransformationInterface::__Sliders_Paint, w );
   HistogramSliders_Control.OnResize( (Control::resize_event_handler)&HistogramTransformationInterface::__Histogram_Resize, w );
   HistogramSliders_Control.OnMouseMove( (Control::mouse_event_handler)&HistogramTransformationInterface::__Sliders_MouseMove, w );
   HistogramSliders_Control.OnMousePress( (Control::mouse_button_event_handler)&HistogramTransformationInterface::__Sliders_MousePress, w );
   HistogramSliders_Control.OnMouseRelease( (Control::mouse_button_event_handler)&HistogramTransformationInterface::__Sliders_MouseRelease, w );

   InputHistogramViewport_Sizer.Add( InputHistogramPlot_Control, 100 );
   InputHistogramViewport_Sizer.Add( HistogramSliders_Control );

   InputHistogram_ScrollBox.DisableAutoScroll();
   InputHistogram_ScrollBox.SetScaledMinSize( w.m_minHistogramWidth, w.m_minHistogramHeight+w.m_sliderControlSize );
   InputHistogram_ScrollBox.OnHorizontalScrollPosUpdated( (ScrollBox::pos_event_handler)&HistogramTransformationInterface::__Histogram_ScrollPosUpdated, w );
   InputHistogram_ScrollBox.OnVerticalScrollPosUpdated( (ScrollBox::pos_event_handler)&HistogramTransformationInterface::__Histogram_ScrollPosUpdated, w );

   InputHistogram_ScrollBox.Viewport().SetSizer( InputHistogramViewport_Sizer );

   //

   Info_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   Info_Label.SetStyle( FrameStyle::Sunken );
   Info_Label.SetLineWidth( 1 );

   //

   for ( size_type i = 0; i < ItemsInArray( s_plotResolutionItems ); ++i )
      PlotResolution_ComboBox.AddItem( s_plotResolutionItems[i] );
   PlotResolution_ComboBox.SetToolTip( "Plot resolution" );
   PlotResolution_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&HistogramTransformationInterface::__PlotResolution_ItemSelected, w );

   GraphStyle_ComboBox.AddItem( "Lines" );
   GraphStyle_ComboBox.AddItem( "Area" );
   GraphStyle_ComboBox.AddItem( "Bars" );
   GraphStyle_ComboBox.AddItem( "Dots" );
   GraphStyle_ComboBox.SetToolTip( "Graph style" );
   GraphStyle_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&HistogramTransformationInterface::__GraphStyle_ItemSelected, w );

   RejectSaturated_ToolButton.SetIcon( Bitmap( reject_saturated_XPM ).ScaledToSize( ui16, ui16 ) );
   RejectSaturated_ToolButton.SetScaledFixedSize( 20, 20 );
   RejectSaturated_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RejectSaturated_ToolButton.SetToolTip( "Reject saturated pixels for histogram representations" );
   RejectSaturated_ToolButton.SetCheckable();
   RejectSaturated_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__RejectSaturated_ButtonClick, w );

   ShowRawRGB_ToolButton.SetIcon( Bitmap( raw_rgb_XPM ).ScaledToSize( ui16, ui16 ) );
   ShowRawRGB_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowRawRGB_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowRawRGB_ToolButton.SetToolTip( "Show raw RGB histograms" );
   ShowRawRGB_ToolButton.SetCheckable();
   ShowRawRGB_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ShowRawRGB_ButtonClick, w );

   LockOutput_ToolButton.SetIcon( Bitmap( lock_output_XPM ).ScaledToSize( ui16, ui16 ) );
   LockOutput_ToolButton.SetScaledFixedSize( 20, 20 );
   LockOutput_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   LockOutput_ToolButton.SetToolTip( "Lock output histogram channel" );
   LockOutput_ToolButton.SetCheckable();
   LockOutput_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__LockOutput_ButtonClick, w );

   ShowCurve_ToolButton.SetIcon( Bitmap( show_curve_XPM ).ScaledToSize( ui16, ui16 ) );
   ShowCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowCurve_ToolButton.SetToolTip( "Show MTF curve" );
   ShowCurve_ToolButton.SetCheckable();
   ShowCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ShowCurve_ButtonClick, w );

   ShowGrid_ToolButton.SetIcon( Bitmap( show_grid_XPM ).ScaledToSize( ui16, ui16 ) );
   ShowGrid_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowGrid_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowGrid_ToolButton.SetToolTip( "Show grids" );
   ShowGrid_ToolButton.SetCheckable();
   ShowGrid_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ShowGrid_ButtonClick, w );

   Graphics_Sizer.SetSpacing( 4 );
   Graphics_Sizer.Add( PlotResolution_ComboBox );
   Graphics_Sizer.Add( GraphStyle_ComboBox );
   Graphics_Sizer.AddStretch();
   Graphics_Sizer.Add( RejectSaturated_ToolButton );
   Graphics_Sizer.Add( ShowRawRGB_ToolButton );
   Graphics_Sizer.Add( LockOutput_ToolButton );
   Graphics_Sizer.Add( ShowCurve_ToolButton );
   Graphics_Sizer.Add( ShowGrid_ToolButton );

   //

   AllViews_ViewList.OnViewSelected( (ViewList::view_event_handler)&HistogramTransformationInterface::__ViewList_ViewSelected, w );
   AllViews_ViewList.OnViewDrag( (Control::view_drag_event_handler)&HistogramTransformationInterface::__ViewDrag, w );
   AllViews_ViewList.OnViewDrop( (Control::view_drop_event_handler)&HistogramTransformationInterface::__ViewDrop, w );

   //

   R_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-red.png" ) );
   R_ToolButton.SetText( "R" );
   R_ToolButton.SetFixedWidth( channelLabelWidth );
   R_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   R_ToolButton.SetToolTip( "Red channel" );
   R_ToolButton.SetCheckable();
   R_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Channel_ButtonClick, w );

   G_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-green.png" ) );
   G_ToolButton.SetText( "G" );
   G_ToolButton.SetFixedWidth( channelLabelWidth );
   G_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   G_ToolButton.SetToolTip( "Green channel" );
   G_ToolButton.SetCheckable();
   G_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Channel_ButtonClick, w );

   B_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-blue.png" ) );
   B_ToolButton.SetText( "B" );
   B_ToolButton.SetFixedWidth( channelLabelWidth );
   B_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   B_ToolButton.SetToolTip( "Blue channel" );
   B_ToolButton.SetCheckable();
   B_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Channel_ButtonClick, w );

   RGBK_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-rgb.png" ) );
   RGBK_ToolButton.SetText( "RGB/K" );
   RGBK_ToolButton.SetFixedWidth( rgbkLabelWidth );
   RGBK_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RGBK_ToolButton.SetToolTip( "RGB/Gray channel" );
   RGBK_ToolButton.SetCheckable();
   RGBK_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Channel_ButtonClick, w );

   A_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-alpha.png" ) );
   A_ToolButton.SetText( "A" );
   A_ToolButton.SetFixedWidth( channelLabelWidth );
   A_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   A_ToolButton.SetToolTip( "Alpha channel" );
   A_ToolButton.SetCheckable();
   A_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Channel_ButtonClick, w );

   NormalReadout_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-readout-normal.png" ) );
   NormalReadout_ToolButton.SetScaledFixedSize( 20, 20 );
   NormalReadout_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   NormalReadout_ToolButton.SetToolTip( "Normal readout mode" );
   NormalReadout_ToolButton.SetCheckable();
   NormalReadout_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ReadoutMode_ButtonClick, w );

   BlackPointReadout_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-readout-shadows.png" ) );
   BlackPointReadout_ToolButton.SetScaledFixedSize( 20, 20 );
   BlackPointReadout_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   BlackPointReadout_ToolButton.SetToolTip( "Black point readout mode" );
   BlackPointReadout_ToolButton.SetCheckable();
   BlackPointReadout_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ReadoutMode_ButtonClick, w );

   MidtonesReadout_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-readout-midtones.png" ) );
   MidtonesReadout_ToolButton.SetScaledFixedSize( 20, 20 );
   MidtonesReadout_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MidtonesReadout_ToolButton.SetToolTip( "Midtones readout mode" );
   MidtonesReadout_ToolButton.SetCheckable();
   MidtonesReadout_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ReadoutMode_ButtonClick, w );

   WhitePointReadout_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-readout-highlights.png" ) );
   WhitePointReadout_ToolButton.SetScaledFixedSize( 20, 20 );
   WhitePointReadout_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   WhitePointReadout_ToolButton.SetToolTip( "White point readout mode" );
   WhitePointReadout_ToolButton.SetCheckable();
   WhitePointReadout_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ReadoutMode_ButtonClick, w );

   Selectors_Sizer.SetSpacing( 4 );
   Selectors_Sizer.Add( R_ToolButton );
   Selectors_Sizer.Add( G_ToolButton );
   Selectors_Sizer.Add( B_ToolButton );
   Selectors_Sizer.Add( RGBK_ToolButton );
   Selectors_Sizer.Add( A_ToolButton );
   Selectors_Sizer.AddStretch();
   Selectors_Sizer.Add( NormalReadout_ToolButton );
   Selectors_Sizer.Add( BlackPointReadout_ToolButton );
   Selectors_Sizer.Add( MidtonesReadout_ToolButton );
   Selectors_Sizer.Add( WhitePointReadout_ToolButton );

   //

   ShadowsClipping_NumericEdit.label.SetText( "Shadows:" );
   ShadowsClipping_NumericEdit.label.SetFixedWidth( labelWidth );
   ShadowsClipping_NumericEdit.SetReal();
   ShadowsClipping_NumericEdit.SetRange( 0, 1 );
   ShadowsClipping_NumericEdit.SetPrecision( TheShadowsClippingParameter->Precision() );
   ShadowsClipping_NumericEdit.SetToolTip( "Shadows clipping" );
   ShadowsClipping_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramTransformationInterface::__HistogramParameter_ValueUpdated, w );

   ShadowsClippingCount_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   ShadowsClippingCount_Label.SetToolTip( "Clipped pixels count, shadows" );
   ShadowsClippingCount_Label.SetStyle( FrameStyle::Sunken );
   ShadowsClippingCount_Label.SetFixedHeight( CanonicalControlHeight( Edit ) );

   ShadowsClippingAutoZero_ToolButton.SetIcon( Bitmap( auto_zero_shadows_XPM ).ScaledToSize( ui16, ui16 ) );
   ShadowsClippingAutoZero_ToolButton.SetScaledFixedSize( 20, 20 );
   ShadowsClippingAutoZero_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShadowsClippingAutoZero_ToolButton.SetToolTip( "Auto zero shadows" );
   ShadowsClippingAutoZero_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__AutoZero_ButtonClick, w );

   ShadowsClippingAutoClip_ToolButton.SetIcon( Bitmap( auto_clip_shadows_XPM ).ScaledToSize( ui16, ui16 ) );
   ShadowsClippingAutoClip_ToolButton.SetScaledFixedSize( 20, 20 );
   ShadowsClippingAutoClip_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   //ShadowsClippingAutoClip_ToolButton.SetToolTip( "Auto Clip Shadows" );
   ShadowsClippingAutoClip_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__AutoClip_ButtonClick, w );

   ShadowsClippingReset_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/clear.png" ) ) );
   ShadowsClippingReset_ToolButton.SetScaledFixedSize( 20, 20 );
   ShadowsClippingReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShadowsClippingReset_ToolButton.SetToolTip( "Reset shadows clipping" );
   ShadowsClippingReset_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Reset_ButtonClick, w );

   ShadowsClipping_Sizer.SetSpacing( 4 );
   ShadowsClipping_Sizer.Add( ShadowsClipping_NumericEdit );
   ShadowsClipping_Sizer.Add( ShadowsClippingCount_Label, 100 );
   ShadowsClipping_Sizer.Add( ShadowsClippingAutoZero_ToolButton );
   ShadowsClipping_Sizer.Add( ShadowsClippingAutoClip_ToolButton );
   ShadowsClipping_Sizer.Add( ShadowsClippingReset_ToolButton );

   //

   HighlightsClipping_NumericEdit.label.SetText( "Highlights:" );
   HighlightsClipping_NumericEdit.label.SetFixedWidth( labelWidth );
   HighlightsClipping_NumericEdit.SetReal();
   HighlightsClipping_NumericEdit.SetRange( 0, 1 );
   HighlightsClipping_NumericEdit.SetPrecision( TheHighlightsClippingParameter->Precision() );
   HighlightsClipping_NumericEdit.SetToolTip( "Highlights clipping" );
   HighlightsClipping_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramTransformationInterface::__HistogramParameter_ValueUpdated, w );

   HighlightsClippingCount_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
   HighlightsClippingCount_Label.SetToolTip( "Clipped pixels count, highlights" );
   HighlightsClippingCount_Label.SetStyle( FrameStyle::Sunken );
   HighlightsClippingCount_Label.SetFixedHeight( CanonicalControlHeight( Edit ) );

   HighlightsClippingAutoZero_ToolButton.SetIcon( Bitmap( auto_zero_highlights_XPM ).ScaledToSize( ui16, ui16 ) );
   HighlightsClippingAutoZero_ToolButton.SetScaledFixedSize( 20, 20 );
   HighlightsClippingAutoZero_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   HighlightsClippingAutoZero_ToolButton.SetToolTip( "Auto zero highlights" );
   HighlightsClippingAutoZero_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__AutoZero_ButtonClick, w );

   HighlightsClippingAutoClip_ToolButton.SetIcon( Bitmap( auto_clip_highlights_XPM ).ScaledToSize( ui16, ui16 ) );
   HighlightsClippingAutoClip_ToolButton.SetScaledFixedSize( 20, 20 );
   HighlightsClippingAutoClip_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   //HighlightsClippingAutoClip_ToolButton.SetToolTip( "Auto Clip Highlights" );
   HighlightsClippingAutoClip_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__AutoClip_ButtonClick, w );

   HighlightsClippingReset_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   HighlightsClippingReset_ToolButton.SetScaledFixedSize( 20, 20 );
   HighlightsClippingReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   HighlightsClippingReset_ToolButton.SetToolTip( "Reset highlights clipping" );
   HighlightsClippingReset_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Reset_ButtonClick, w );

   HighlightsClipping_Sizer.SetSpacing( 4 );
   HighlightsClipping_Sizer.Add( HighlightsClipping_NumericEdit );
   HighlightsClipping_Sizer.Add( HighlightsClippingCount_Label, 100 );
   HighlightsClipping_Sizer.Add( HighlightsClippingAutoZero_ToolButton );
   HighlightsClipping_Sizer.Add( HighlightsClippingAutoClip_ToolButton );
   HighlightsClipping_Sizer.Add( HighlightsClippingReset_ToolButton );

   //

   MidtonesBalance_NumericEdit.label.SetText( "Midtones:" );
   MidtonesBalance_NumericEdit.label.SetFixedWidth( labelWidth );
   MidtonesBalance_NumericEdit.SetReal();
   MidtonesBalance_NumericEdit.SetRange( 0, 1 );
   MidtonesBalance_NumericEdit.SetPrecision( TheMidtonesBalanceParameter->Precision() );
   MidtonesBalance_NumericEdit.SetToolTip( "Midtones balance" );
   MidtonesBalance_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramTransformationInterface::__HistogramParameter_ValueUpdated, w );

   MidtonesBalanceReset_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   MidtonesBalanceReset_ToolButton.SetScaledFixedSize( 20, 20 );
   MidtonesBalanceReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   MidtonesBalanceReset_ToolButton.SetToolTip( "Reset midtones balance" );
   MidtonesBalanceReset_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Reset_ButtonClick, w );

   AutoClipSetup_PushButton.SetText( "Auto Clip Setup" );
   AutoClipSetup_PushButton.SetFixedHeight( CanonicalControlHeight( NumericEdit ) );
   AutoClipSetup_PushButton.SetFocusStyle( FocusStyle::NoFocus );
   AutoClipSetup_PushButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__AutoClipSetup_ButtonClick, w );

   ShowRangeControls_ToolButton.SetIcon( w.ScaledResource( ":/process-interface/expand-vert.png" ) );
   ShowRangeControls_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowRangeControls_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowRangeControls_ToolButton.SetToolTip( "Show dynamic range expansion controls" );
   ShowRangeControls_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__ToggleExtension_ButtonClick, w );

   MidtonesBalance_Sizer.SetSpacing( 4 );
   MidtonesBalance_Sizer.Add( MidtonesBalance_NumericEdit );
   MidtonesBalance_Sizer.Add( MidtonesBalanceReset_ToolButton );
   MidtonesBalance_Sizer.AddStretch();
   MidtonesBalance_Sizer.Add( AutoClipSetup_PushButton );
   MidtonesBalance_Sizer.Add( ShowRangeControls_ToolButton );

   //

   LowRange_NumericControl.label.SetText( "Low range:" );
   LowRange_NumericControl.label.SetFixedWidth( labelWidth );
   LowRange_NumericControl.slider.SetRange( 0, 100 );
   LowRange_NumericControl.SetReal();
   LowRange_NumericControl.SetRange( TheLowRangeParameter->MinimumValue(), TheLowRangeParameter->MaximumValue() );
   LowRange_NumericControl.SetPrecision( TheLowRangeParameter->Precision()-1 );
   LowRange_NumericControl.edit.SetFixedWidth( ShadowsClipping_NumericEdit.edit.Width() );
   LowRange_NumericControl.SetToolTip( "Dynamic range expansion, lower bound" );
   LowRange_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramTransformationInterface::__HistogramParameter_ValueUpdated, w );

   LowRangeReset_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   LowRangeReset_ToolButton.SetScaledFixedSize( 20, 20 );
   LowRangeReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   LowRangeReset_ToolButton.SetToolTip( "Reset low dynamic range expansion" );
   LowRangeReset_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Reset_ButtonClick, w );

   LowRange_Sizer.SetSpacing( 4 );
   LowRange_Sizer.Add( LowRange_NumericControl, 100 );
   LowRange_Sizer.Add( LowRangeReset_ToolButton );

   //

   HighRange_NumericControl.label.SetText( "High range:" );
   HighRange_NumericControl.label.SetFixedWidth( labelWidth );
   HighRange_NumericControl.slider.SetRange( 0, 100 );
   HighRange_NumericControl.SetReal();
   HighRange_NumericControl.SetRange( TheHighRangeParameter->MinimumValue(), TheHighRangeParameter->MaximumValue() );
   HighRange_NumericControl.SetPrecision( TheHighRangeParameter->Precision()-1 );
   HighRange_NumericControl.edit.SetFixedWidth( ShadowsClipping_NumericEdit.edit.Width() );
   HighRange_NumericControl.SetToolTip( "Dynamic range expansion, upper bound" );
   HighRange_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramTransformationInterface::__HistogramParameter_ValueUpdated, w );

   HighRangeReset_ToolButton.SetIcon( w.ScaledResource( ":/icons/clear.png" ) );
   HighRangeReset_ToolButton.SetScaledFixedSize( 20, 20 );
   HighRangeReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   HighRangeReset_ToolButton.SetToolTip( "Reset high dynamic range expansion" );
   HighRangeReset_ToolButton.OnClick( (ToolButton::click_event_handler)&HistogramTransformationInterface::__Reset_ButtonClick, w );

   HighRange_Sizer.SetSpacing( 4 );
   HighRange_Sizer.Add( HighRange_NumericControl, 100 );
   HighRange_Sizer.Add( HighRangeReset_ToolButton );

   //

   RangeSection_Sizer.SetSpacing( 4 );
   RangeSection_Sizer.Add( LowRange_Sizer );
   RangeSection_Sizer.Add( HighRange_Sizer );

   RangeSection_Control.SetSizer( RangeSection_Sizer );
   RangeSection_Control.AdjustToContents();
   RangeSection_Control.SetMinHeight();

   if ( !w.m_rangeSectionVisible )
      RangeSection_Control.Hide();

   //

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( OutputHistogram_ScrollBox );
   Global_Sizer.Add( Mode_Sizer );
   Global_Sizer.Add( InputHistogram_ScrollBox );
   Global_Sizer.Add( Info_Label );
   Global_Sizer.Add( Graphics_Sizer );
   Global_Sizer.Add( AllViews_ViewList );
   Global_Sizer.Add( Selectors_Sizer );
   Global_Sizer.Add( ShadowsClipping_Sizer );
   Global_Sizer.Add( HighlightsClipping_Sizer );
   Global_Sizer.Add( MidtonesBalance_Sizer );
   Global_Sizer.Add( RangeSection_Control );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&HistogramTransformationInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HistogramTransformationInterface.cpp - Released 2017-07-18T16:14:18Z
