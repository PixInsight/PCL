//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 01.00.00.0184
// ----------------------------------------------------------------------------
// AssistedColorCalibrationInterface.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2017 Zbynek Vrastil
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#ifndef __AssistedColorCalibrationInterface_h
#define __AssistedColorCalibrationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/NumericControl.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/ViewList.h>

#include "AssistedColorCalibrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS Graphics;

#define HIST_PREVIEW_RESOLUTION 256

class AssistedColorCalibrationInterface : public ProcessInterface
{
public:

   AssistedColorCalibrationInterface();
   virtual ~AssistedColorCalibrationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   AssistedColorCalibrationInstance instance;

   struct GUIData
   {
      GUIData( AssistedColorCalibrationInterface& );

      // helper methods to set up repeating controls
      void SetUpCorrectionFactorNumericControl( AssistedColorCalibrationInterface& w, NumericControl &control,
         MetaFloat *parameter, const String &label );
      void SetUpHistogramNumericEdit( AssistedColorCalibrationInterface& w, NumericEdit &control,
         MetaFloat *parameter, const String &label, const String &tooltip );
      void SetUpSaturationNumericControl( AssistedColorCalibrationInterface& w, NumericControl &control,
         MetaFloat *parameter, const String &label, const String &tooltip );

      VerticalSizer     Global_Sizer;
         SectionBar        WhiteBalance_Section;
            Control           WhiteBalance_Control;
            HorizontalSizer   WhiteBalance_SizerH;
               VerticalSizer     WhiteBalance_Sizer;
                  NumericControl    RedCorrectionFactor_NumericControl;
                  NumericControl    GreenCorrectionFactor_NumericControl;
                  NumericControl    BlueCorrectionFactor_NumericControl;
               VerticalSizer     WhiteBalanceReset_Sizer;
                  ToolButton        WhiteBalanceReset_ToolButton;
         SectionBar        PreviewParams_Section;
            Control           PreviewParams_Control;
            VerticalSizer     PreviewParams_Sizer;
               GroupBox          Background_Group;
                  VerticalSizer     Background_Sizer;
                     HorizontalSizer   BackgroundRef_Sizer;
                        Label             BackgroundRef_Label;
                        ViewList          BackgroundRef_ViewList;
               GroupBox          Histogram_Group;
                  HorizontalSizer   Histogram_Sizer;
                     VerticalSizer     HistogramPlot_Sizer;
                        Control           HistogramPlot_Control;
                        Control           HistogramSliders_Control;
                     VerticalSizer     HistogramValues_Sizer;
                        NumericEdit       HistogramShadows_NumericEdit;
                        NumericEdit       HistogramHighlights_NumericEdit;
                        NumericEdit       HistogramMidtones_NumericEdit;
                        HorizontalSizer   HistogramReset_Sizer;
                           ToolButton        HistogramReset_ToolButton;
               GroupBox          Saturation_Group;
                  HorizontalSizer   Saturation_Sizer;
                     NumericControl    SaturationBoost_NumericControl;
   };

   GUIData* GUI;

   enum SliderId     { C0Slider, MSlider, C1Slider, NoSlider = -1 };

   Bitmap*        inputScrBmp;         // screen bitmap, input histogram viewport
   Bitmap*        slidersScrBmp;       // screen bitmap, slider area

   SliderId       sliderBeingDragged;  // moving one of our little triangular things?

   void UpdateControls();

   // Screen bitmap generation
   void GenerateInputScreenBitmap();
   void GenerateSlidersScreenBitmap();

   // Histogram plot graphics
   void PlotGrid( Graphics& g, const Rect& r, int width, int height );
   void PlotMidtonesTransferCurve( Graphics& g, const Rect& r, int width, int height );
   void PlotHandler( Graphics& g, double v, int x0, int width );
   void PlotScale( Graphics& g, const Rect& r, int width );

   // Helper methods
   RGBA HandlerColor( double v ) const;
   SliderId FindHandler( double v ) const;
   double SliderToHistogram( int x ) const;
   void SetClippingParameters( double, double );
   void SetShadowsClipping( double );
   void SetHighlightsClipping( double );
   void SetMidtonesBalance( double );
   void ResetWhiteBalance();

   // Event Handlers
   void __CorrectionFactorUpdated( NumericEdit& sender, double value );
   void __HistogramParameterUpdated( NumericEdit& sender, double value );
   void __SaturationBoostUpdated( NumericEdit& sender, double value );
   void __BackgroundRefViewSelected( ViewList& sender, View& view );
   void __Histogram_Paint( Control& sender, const pcl::Rect& updateRect );
   void __Sliders_Paint( Control& sender, const pcl::Rect& updateRect );
   void __Sliders_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Sliders_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Sliders_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void __Reset_ButtonClick( Button& sender, bool );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AssistedColorCalibrationInterface* TheAssistedColorCalibrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AssistedColorCalibrationInterface_h

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationInterface.h - Released 2017-05-02T09:43:01Z
