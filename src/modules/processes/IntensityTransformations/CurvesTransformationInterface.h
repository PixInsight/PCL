//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0355
// ----------------------------------------------------------------------------
// CurvesTransformationInterface.h - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __CurvesTransformationInterface_h
#define __CurvesTransformationInterface_h

#include <pcl/Histogram.h>
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

#include "CurvesTransformationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS Graphics;

class CurvesTransformationInterface : public ProcessInterface
{
public:

   CurvesTransformationInterface();
   virtual ~CurvesTransformationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   virtual void TrackViewUpdated( bool active );
   virtual void RealTimePreviewUpdated( bool active );
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const;
   virtual bool GenerateRealTimePreview( UInt16Image&, const View&, int zoomLevel, String& info ) const;

   virtual bool WantsImageNotifications() const;
   virtual void ImageUpdated( const View& );
   virtual void ImageFocused( const View& );
   virtual void ImageDeleted( const View& );

   virtual bool WantsReadoutNotifications() const;
   virtual void BeginReadout( const View& );
   virtual void UpdateReadout( const View&, const DPoint& p, double R, double G, double B, double A );
   virtual void EndReadout( const View& );

private:

   CurvesTransformationInstance m_instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const CurvesTransformationInstance& );

      virtual void Run();

   private:

      CurvesTransformationInstance m_instance;
   };

   mutable RealTimeThread* m_realTimeThread;

   struct GUIData
   {
      GUIData( CurvesTransformationInterface& );

      VerticalSizer     Global_Sizer;
         ScrollBox         Curve_ScrollBox;
         HorizontalSizer   Mode_Sizer;
            ToolButton        EditPointMode_ToolButton;
            ToolButton        SelectPointMode_ToolButton;
            ToolButton        DeletePointMode_ToolButton;
            ToolButton        ZoomInMode_ToolButton;
            ToolButton        ZoomOutMode_ToolButton;
            ToolButton        PanMode_ToolButton;
            SpinBox           Zoom_SpinBox;
            ToolButton        Zoom11_ToolButton;
            ToolButton        ShowAll_ToolButton;
            ToolButton        ShowGrid_ToolButton;
         Label             Info_Label;
         HorizontalSizer   Selectors_Sizer;
            ToolButton        R_ToolButton;
            ToolButton        G_ToolButton;
            ToolButton        B_ToolButton;
            ToolButton        RGBK_ToolButton;
            ToolButton        A_ToolButton;
            ToolButton        L_ToolButton;
            ToolButton        a_ToolButton;
            ToolButton        b_ToolButton;
            ToolButton        c_ToolButton;
            ToolButton        H_ToolButton;
            ToolButton        S_ToolButton;
         HorizontalSizer   CurveData_Row1_Sizer;
            NumericEdit       Input_NumericEdit;
            ToolButton        PrevPoint_ToolButton;
            ToolButton        FirstPoint_ToolButton;
            ToolButton        StoreCurve_ToolButton;
            ToolButton        RestoreCurve_ToolButton;
            ToolButton        ReverseCurve_ToolButton;
            ToolButton        ResetCurve_ToolButton;
         HorizontalSizer   CurveData_Row2_Sizer;
            NumericEdit       Output_NumericEdit;
            ToolButton        NextPoint_ToolButton;
            ToolButton        LastPoint_ToolButton;
            Label             CurrentPoint_Label;
            ToolButton        AkimaSubsplines_ToolButton;
            ToolButton        CubicSpline_ToolButton;
            ToolButton        Linear_ToolButton;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI;

   /*
    * Workbench
    */

   typedef Histogram::count_type       count_type;
   typedef Histogram::histogram_type   histogram_type;
   typedef Array<Histogram>            histogram_list;
   typedef GenericVector<RGBA>         channel_colors;

   enum working_mode { EditMode, SelectMode, DeleteMode, ZoomInMode, ZoomOutMode, PanMode, NoMode = -1 };

   working_mode   m_mode;
   working_mode   m_savedMode;      // for temporary keyboard mode switch

   int            m_channel;

   UIVector       m_currentPoint;   // point indices

   histogram_list m_histograms;     // image histograms, 16-bit resolution
   View           m_histogramView;
   bool           m_histogramColor;

   bool           m_readoutActive;
   DVector        m_readouts;       // 0=R 1=G 2=B 3=Alpha
   RGBColorSystem m_readoutRGBWS;

   int            m_zoomX;
   int            m_zoomY;

   int            m_wheelSteps;     // accumulated 1/8-degree wheel steps

   bool           m_showAllCurves;
   bool           m_showGrid;       // draw coordinate grids

   int            m_panning;        // panning the viewport?
   Point          m_panOrigin;

   bool           m_cursorVisible;
   bool           m_dragging;       // dragging a curve point?
   Point          m_cursorPos;      // cursor position in viewport coordinates.
   DPoint         m_curvePos;       // cursor position in normalized coordinates.

   Bitmap         m_viewportBitmap; // screen bitmap
   bool           m_viewportDirty : 1;

   Curve          m_storedCurve;

   channel_colors m_channelColor;
   RGBA           m_gridColor0;
   RGBA           m_gridColor1;
   RGBA           m_backgroundColor;

   bool           m_settingUp;      // true during viewport transitional states (e.g. resizing)

   /*
    * Current curve point
    */

   size_type CurrentPoint() const
   {
      return m_currentPoint[m_channel];
   }

   double CurrentInputValue() const
   {
      return m_instance[m_channel].X( CurrentPoint() );
   }

   double CurrentOutputValue() const
   {
      return m_instance[m_channel].Y( CurrentPoint() );
   }

   double& CurrentOutputValue()
   {
      return m_instance[m_channel].Y( CurrentPoint() );
   }

   /*
    * Curve point mutators
    */

   size_type FindPoint( int c, double x, double y, int tolerancePx = 0 ) const;

   size_type FindPoint( int c, const DPoint& p, int tolerancePx = 0 ) const
   {
      return FindPoint( c, p.x, p.y, tolerancePx );
   }

   size_type CreatePoint( int c, double x, double y, int tolerancePx = 0 );

   size_type CreatePoint( int c, const DPoint& p, int tolerancePx = 0 )
   {
      return CreatePoint( c, p.x, p.y, tolerancePx );
   }

   bool DragPoint( int c, size_type i, double x, double y );

   bool DragPoint( int c, size_type i, const DPoint& p )
   {
      return DragPoint( c, i, p.x, p.y );
   }

   size_type RemovePoint( int c, double x, double y, int tolerancePx = 0 );

   size_type RemovePoint( int c, const DPoint& p, int tolerancePx = 0 )
   {
      return RemovePoint( c, p.x, p.y, tolerancePx );
   }

   /*
    * Histograms
    */
   void GetHistograms();

   /*
    * Curve parameters
    */
   void SetChannel( int );
   void SetZoom( int, int, const Point* = 0 );
   void SetMode( working_mode );
   void SetInterpolation( int );
   void SelectPoint( size_type, bool pan = true );

   /*
    * GUI Updates
    */
   void UpdateControls();
   void UpdateModeControls();
   void UpdateZoomControls();
   void UpdateGraphicsControls();
   void UpdateChannelControls();
   void UpdateInterpolationControls();
   void UpdateCurveControls();
   void UpdateCurve();
   void UpdateCurveInfo();
   void UpdateHistograms();
   void UpdateRealTimePreview();

   /*
    * Screen bitmap regeneration
    */
   void RegenerateViewport();

   /*
    * Curve drawing primitives
    */
   void PlotGrid( Graphics&, const Rect& viewport, int width, int height, int hZoom, int vZoom );
   void PlotScale( Graphics&, const Rect& viewport, int width, int height );
   void PlotCurve( Graphics&, const Rect& viewport, int channel, int width, int height, int hZoom, int vZoom );
   void PlotReadouts( Graphics&, const Bitmap&, const Rect& viewport, int width, int height );
   void PlotCursor( Graphics&, const Rect& viewport );

   RGBA ScaleColor( float ) const;

   /*
    * GUI Event Handlers
    */

   void __Curve_Paint( Control& sender, const pcl::Rect& updateRect );
   void __Curve_Resize( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );
   void __Curve_ScrollPosUpdated( ScrollBox& sender, int pos );
   void __Curve_Enter( Control& sender );
   void __Curve_Leave( Control& sender );
   void __Curve_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Curve_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Curve_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void __Curve_MouseWheel( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );
   void __Curve_KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );

   void __CurveParameter_ValueUpdated( NumericEdit& sender, double value );

   void __Mode_ButtonClick( Button&, bool );
   void __Channel_ButtonClick( Button&, bool );
   void __Zoom_ButtonClick( Button&, bool );
   void __ShowAll_ButtonClick( Button&, bool );
   void __ShowGrid_ButtonClick( Button&, bool );
   void __PointNavigation_ButtonClick( Button&, bool );
   void __Interpolation_ButtonClick( Button&, bool );
   void __StoreCurve_ButtonClick( Button&, bool );
   void __RestoreCurve_ButtonClick( Button&, bool );
   void __ReverseCurve_ButtonClick( Button&, bool );
   void __ResetCurve_ButtonClick( Button&, bool );

   void __Zoom_ValueUpdated( SpinBox& sender, int value );

   void __KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void __KeyRelease( Control& sender, int key, unsigned modifiers, bool& wantsKey );

   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
   friend class HistogramTransformationInterface;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CurvesTransformationInterface* TheCurvesTransformationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CurvesTransformationInterface_h

// ----------------------------------------------------------------------------
// EOF CurvesTransformationInterface.h - Released 2016/02/21 20:22:43 UTC
