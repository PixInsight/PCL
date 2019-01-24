//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0430
// ----------------------------------------------------------------------------
// HistogramTransformationInterface.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __HistogramTransformationInterface_h
#define __HistogramTransformationInterface_h

#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/ScrollBox.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>
#include <pcl/Vector.h>
#include <pcl/View.h>
#include <pcl/ViewList.h>

#include "HistogramTransformationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS Graphics;

class HistogramTransformationInterface : public ProcessInterface
{
public:

   HistogramTransformationInterface();
   virtual ~HistogramTransformationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   virtual void RealTimePreviewUpdated( bool active );
   virtual void TrackViewUpdated( bool active );
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const;
   virtual bool GenerateRealTimePreview( UInt16Image&, const View&, int zoomLevel, String& info ) const;

   virtual bool WantsImageNotifications() const;
   virtual void ImageUpdated( const View& v );
   virtual void ImageFocused( const View& v );

   virtual bool WantsViewPropertyNotifications() const;
   virtual void ViewPropertyUpdated( const View& v, const IsoString& property );
   virtual void ViewPropertyDeleted( const View& v, const IsoString& property );

   virtual bool WantsReadoutNotifications() const;
   virtual void BeginReadout( const View& v );
   virtual void UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double A );
   virtual void EndReadout( const View& v );

   virtual bool WantsRealTimePreviewNotifications() const;
   virtual void RealTimePreviewOwnerChanged( ProcessInterface& );

   virtual void SaveSettings() const;
   virtual void LoadSettings();

   void PlotHistogram( Graphics&, const Rect& viewport,
                       const Histogram&, Histogram::count_type peak,
                       int histogramWidth, int histogramHeight, int horizontalZoom, int verticalZoom );

   int PlotResolution() const
   {
      return m_plotResolution;
   }

   bool RejectingSaturated() const
   {
      return m_rejectSaturated;
   }

private:

   HistogramTransformationInstance m_instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const HistogramTransformationInstance& );

      virtual void Run();

   private:

      HistogramTransformationInstance m_instance;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( HistogramTransformationInterface& );

      VerticalSizer     Global_Sizer;

      ScrollBox         OutputHistogram_ScrollBox;
      HorizontalSizer   Mode_Sizer;
         ToolButton        ReadoutMode_ToolButton;
         ToolButton        ZoomInMode_ToolButton;
         ToolButton        ZoomOutMode_ToolButton;
         ToolButton        PanMode_ToolButton;
         SpinBox           HorizontalZoom_SpinBox;
         SpinBox           VerticalZoom_SpinBox;
         ToolButton        Zoom11_ToolButton;
         ToolButton        ShowOutput_ToolButton;
         SpinBox           OutputHorizontalZoom_SpinBox;
         SpinBox           OutputVerticalZoom_SpinBox;

      ScrollBox         InputHistogram_ScrollBox;
         VerticalSizer     InputHistogramViewport_Sizer;
            Control           InputHistogramPlot_Control;
            Control           HistogramSliders_Control;

      Label             Info_Label;

      HorizontalSizer   Graphics_Sizer;
         ComboBox          PlotResolution_ComboBox;
         ComboBox          GraphStyle_ComboBox;
         ToolButton        RejectSaturated_ToolButton;
         ToolButton        ShowRawRGB_ToolButton;
         ToolButton        LockOutput_ToolButton;
         ToolButton        ShowCurve_ToolButton;
         ToolButton        ShowGrid_ToolButton;
      ViewList          AllViews_ViewList;

      HorizontalSizer   Selectors_Sizer;
         ToolButton        R_ToolButton;
         ToolButton        G_ToolButton;
         ToolButton        B_ToolButton;
         ToolButton        RGBK_ToolButton;
         ToolButton        A_ToolButton;
         ToolButton        NormalReadout_ToolButton;
         ToolButton        BlackPointReadout_ToolButton;
         ToolButton        MidtonesReadout_ToolButton;
         ToolButton        WhitePointReadout_ToolButton;

      HorizontalSizer   ShadowsClipping_Sizer;
         NumericEdit       ShadowsClipping_NumericEdit;
         Label             ShadowsClippingCount_Label;
         ToolButton        ShadowsClippingAutoZero_ToolButton;
         ToolButton        ShadowsClippingAutoClip_ToolButton;
         ToolButton        ShadowsClippingReset_ToolButton;

      HorizontalSizer   HighlightsClipping_Sizer;
         NumericEdit       HighlightsClipping_NumericEdit;
         Label             HighlightsClippingCount_Label;
         ToolButton        HighlightsClippingAutoZero_ToolButton;
         ToolButton        HighlightsClippingAutoClip_ToolButton;
         ToolButton        HighlightsClippingReset_ToolButton;

      HorizontalSizer   MidtonesBalance_Sizer;
         NumericEdit       MidtonesBalance_NumericEdit;
         ToolButton        MidtonesBalanceReset_ToolButton;
         PushButton        AutoClipSetup_PushButton;
         ToolButton        ShowRangeControls_ToolButton;

      Control           RangeSection_Control;
      VerticalSizer     RangeSection_Sizer;
         HorizontalSizer   LowRange_Sizer;
            NumericControl    LowRange_NumericControl;
            ToolButton        LowRangeReset_ToolButton;
         HorizontalSizer   HighRange_Sizer;
            NumericControl    HighRange_NumericControl;
            ToolButton        HighRangeReset_ToolButton;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   /*
    * Workbench
    */
   enum working_mode  { ReadoutMode, ZoomInMode, ZoomOutMode, PanMode, NoMode = -1 };
   enum readout_mode  { NormalReadout, BlackPointReadout, MidtonesReadout, WhitePointReadout };
   enum graph_style   { LineStyle, AreaStyle, BarStyle, DotStyle, NumberOfGraphStyles };
   enum slider_id     { C0Slider, MSlider, C1Slider, NoSlider = -1 };
   enum cursor_status { NoCursor, InputCursor, OutputCursor };

   typedef Histogram::count_type       count_type;
   typedef Histogram::histogram_type   histogram_type;
   typedef Array<Histogram>            histogram_list;
   typedef GenericVector<RGBA>         channel_colors;

   // Histogram data.
   histogram_list m_sourceData;           // source input histograms, 16-bit resolution
   histogram_list m_inputData;            // input histograms, rescaled to the plot resolution
   histogram_list m_outputData;           // output RGBA histograms (R,G,B include the combined RGB/K transformation)
   histogram_list m_outputRGBData;        // intermediate RGB histograms, after individual RGB transformations

   // Histogram plot resolution, or the number of discrete histogram levels to
   // be represented.
   int            m_plotResolution = 256;

   // Working modes.
   working_mode   m_mode = ReadoutMode;
   working_mode   m_savedMode = NoMode;   // for temporary keyboard mode switch
   readout_mode   m_readoutMode = NormalReadout;

   // Current histogram channel.
   // 0=R 1=G 2=B 3=RGB/K 4=Alpha
   int            m_channel = 3;

   // Current graph style.
   graph_style    m_graphStyle = LineStyle;

   // Automatic histogram clippings.
   // Values in fraction of total pixels.
   double         m_shadowsAutoClipping = 0.01;
   double         m_highlightsAutoClipping = 0.01;

   // Clipping pixel counts.
   // 0=R 1=G 2=B 3=RGB/K 4=Alpha
   histogram_type m_shadowsCount;
   histogram_type m_highlightsCount;

   // Image readouts.
   bool           m_readoutActive = false;
   DVector        m_inputReadouts;  // 0=R 1=G 2=B 3=notUsed 4=Alpha
   DVector        m_outputReadouts;

   // Graph amplification factors.
   int            m_inputZoomX = 1;
   int            m_inputZoomY = 1;
   int            m_outputZoomX = 1;
   int            m_outputZoomY = 1;

   // Accumulated 1/8-degree wheel steps.
   int            m_wheelSteps = 0;

   // Histogram representation options.
   bool           m_rejectSaturated = true;   // ignore the first and last histogram counts to compute peaks
   bool           m_rawRGBInput = true;       // always show raw RGB input histograms when channel=RGB/K
   bool           m_lockOutputChannel = true; // always show RGB output histograms
   bool           m_showMTF = true;           // draw the midtones transfer function curve
   bool           m_showGrid = true;          // draw coordinate grids

   // Interactive states.
   slider_id      m_sliderBeingDragged = NoSlider; // moving one of our little triangular things?
   int            m_panning = 0;                   // panning one of our histogram viewports?
   Point          m_panOrigin = 0;

   // Graph cursor.
   cursor_status  m_cursorStatus = NoCursor;
   Point          m_cursorPos = -1;    // cursor position in viewport crds.
   DPoint         m_histogramPos = 0;  // cursor position in normalized crds.

   // Screen bitmap, input histogram viewport.
   Bitmap         m_inputBitmap;
   bool           m_inputDirty = true;

   // Screen bitmap, output histogram viewport.
   Bitmap         m_outputBitmap;
   bool           m_outputDirty = true;

   // Screen bitmap, slider area.
   Bitmap         m_slidersBitmap;
   bool           m_slidersDirty = true;

   // States of extensible interface sections.
   bool           m_outputSectionVisible = true;
   bool           m_rangeSectionVisible = false;

   // Graph colors
   // 0=R 1=G 2=B 3=RGB/K 4=Alpha
   channel_colors m_channelColors;
   RGBA           m_gridColor0;
   RGBA           m_gridColor1;
   RGBA           m_backgroundColor;

   // Minimum graph dimensions.
   int            m_minHistogramWidth = 400;
   int            m_minHistogramHeight = 200;
   int            m_sliderControlSize = 12;

   // Flag true during viewport transitional states (e.g. resizing).
   bool           m_settingUp = false;

   /*
    * Main calculation routines
    */
   bool GetSourceHistograms();
   void CalculateInputHistograms();
   void CalculateOutputHistograms();
   void DestroyOutputHistograms();
   void CalculateClippingCounts();
   void CalculateRealTimePreviewLUT();
   void DestroyRealTimePreviewLUT();

   /*
    * Setting parameters
    */
   void SetClippingParameters( double, double );
   void SetShadowsClipping( double );
   void SetHighlightsClipping( double );
   void SetMidtonesBalance( double );
   void SetLowRange( double );
   void SetHighRange( double );
   void SetChannel( int );
   void SetPlotResolution( int );
   void SetGraphStyle( graph_style );
   void SetRejectSaturated( bool );
   void SetInputZoom( int, int, const Point* = 0 );
   void SetOutputZoom( int, int, const Point* = 0 );
   void SetMode( working_mode );
   void SetReadoutMode( readout_mode );

   /*
    * GUI Updates
    */
   void UpdateControls();
   void UpdateModeControls();
   void UpdateZoomControls();
   void UpdateGraphicsControls();
   void UpdateChannelControls();
   void UpdateReadoutModeControls();
   void UpdateHistogramControls();
   void UpdateClippingCountControls();
   void UpdateHistograms();
   void UpdateInputHistogram();
   void UpdateOutputHistogram();
   void UpdateHistogramSliders();
   void UpdateHistogramInfo();
   void UpdateAutoClippingToolTips();
   void UpdateRealTimePreview();
   void SynchronizeWithCurrentView();

   /*
    * Screen bitmap regeneration
    */
   void RegenerateInputViewport();
   void RegenerateOutputViewport();
   void RegenerateSlidersViewport();

   /*
    * Histogram drawing primitives
    */
   //void PlotHistogram( Graphics&, const Rect& viewport,
   //                    const Histogram&, count_type peak,
   //                    int width, int height, int hZoom, int vZoom ); ### made public, see above
   void PlotGrid( Graphics&, const Rect& viewport, int width, int height, int hZoom, int vZoom );
   void PlotScale( Graphics&, const Rect& viewport, int width );
   void PlotHandler( Graphics&, double value, int x0, int width );
   void PlotMidtonesTransferCurve( Graphics& g, const Rect& viewport, int width, int height );
   void PlotReadouts( Graphics&, const Bitmap&, const Rect& viewport, const DVector&, int width, int height );
   void PlotCursor( Graphics&, const Rect& viewport );

   /*
    * Miscellaneous drawing and GUI helpers
    */
   RGBA HandlerColor( double ) const;
   slider_id FindHandler( double ) const;
   double SliderToHistogram( int ) const;

   /*
    * Routines to animate extensible sections
    */
   void ToggleOutputHistogram();
   void ToggleRangeControls();

   /*
    * GUI Event Handlers
    */
   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __Histogram_Paint( Control& sender, const pcl::Rect& updateRect );
   void __Sliders_Paint( Control& sender, const pcl::Rect& updateRect );
   void __Histogram_Resize( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );
   void __Histogram_ScrollPosUpdated( ScrollBox& sender, int pos );
   void __Histogram_Enter( Control& sender );
   void __Histogram_Leave( Control& sender );
   void __Histogram_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Histogram_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Histogram_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void __Histogram_MouseWheel( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );
   void __Sliders_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Sliders_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Sliders_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void __HistogramParameter_ValueUpdated( NumericEdit& sender, double value );
   void __Reset_ButtonClick( Button&, bool );
   void __AutoZero_ButtonClick( Button&, bool );
   void __AutoClip_ButtonClick( Button&, bool );
   void __AutoClipSetup_ButtonClick( Button&, bool );
   void __Mode_ButtonClick( Button&, bool );
   void __ReadoutMode_ButtonClick( Button&, bool );
   void __Channel_ButtonClick( Button&, bool );
   void __Zoom_ButtonClick( Button&, bool );
   void __Zoom_ValueUpdated( SpinBox& sender, int value );
   void __PlotResolution_ItemSelected( ComboBox& sender, int itemIndex );
   void __GraphStyle_ItemSelected( ComboBox& sender, int itemIndex );
   void __RejectSaturated_ButtonClick( Button&, bool );
   void __ShowRawRGB_ButtonClick( Button&, bool );
   void __LockOutput_ButtonClick( Button&, bool );
   void __ShowCurve_ButtonClick( Button&, bool );
   void __ShowGrid_ButtonClick( Button&, bool );
   void __ToggleExtension_ButtonClick( Button&, bool );
   void __KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void __KeyRelease( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
   friend class HistogramAutoClipSetupDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern HistogramTransformationInterface* TheHistogramTransformationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramTransformationInterface_h

// ----------------------------------------------------------------------------
// EOF HistogramTransformationInterface.h - Released 2019-01-21T12:06:41Z
