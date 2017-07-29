//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0249
// ----------------------------------------------------------------------------
// ConvolutionInterface.h - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#ifndef __ConvolutionInterface_h
#define __ConvolutionInterface_h

#include <pcl/Button.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TabBox.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>

#include "ConvolutionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvolutionInterface : public ProcessInterface
{
public:

   ConvolutionInterface();
   virtual ~ConvolutionInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   virtual void ResetInstance();
   virtual void RealTimePreviewUpdated( bool active );

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned&/*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const;
   virtual bool GenerateRealTimePreview( UInt16Image&, const View&, int zoomLevel, String& info ) const;

   virtual void SaveSettings() const;
   virtual void LoadSettings();

   virtual bool WantsImageNotifications() const;
   virtual void ImageUpdated( const View& );
   virtual void ImageRenamed( const View& );
   virtual void ImageDeleted( const View& );

private:

   ConvolutionInstance instance;

   class RealTimeThread : public Thread
   {
   public:

      Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const ConvolutionInstance&, int );

      virtual void Run();

   private:

      ConvolutionInstance m_instance;
      int                 m_zoomLevel;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( ConvolutionInterface& );

      VerticalSizer  Global_Sizer;
         HorizontalSizer     Filter_Sizer;
            TabBox               FilterMode_TabBox;

               Control              Parametric_Control;
               VerticalSizer        Parametric_Sizer;
                  HorizontalSizer   Sigma_Sizer;
                     NumericEdit       Sigma_NumericEdit;
                     VerticalSizer     SigmaSliders_Sizer;
                        Slider            SigmaCoarse_Slider;
                        Slider            SigmaFine_Slider;
                  NumericControl       Shape_NumericControl;
                  NumericControl       AspectRatio_NumericControl;
                  NumericControl       RotationAngle_NumericControl;

               Control           Library_Control;
               VerticalSizer     Library_Sizer;
                  Label             LibraryPath_Label;
                  HorizontalSizer   LibraryPath_Sizer;
                     Edit              LibraryPath_Edit;
                     ToolButton        LibraryPath_ToolButton;
                  HorizontalSizer   LibraryButtons_Sizer;
                     PushButton        SaveAsLibrary_PushButton;
                     PushButton        NewLibrary_PushButton;
                     PushButton        DefaultLibrary_PushButton;
                  ComboBox          Filter_ComboBox;
                  CheckBox          RescaleHighPass_CheckBox;
                  HorizontalSizer   FilterButtons_Sizer;
                     PushButton        ViewFilterElements_PushButton;
                     PushButton        EditFilter_PushButton;
                     PushButton        NewFilter_PushButton;
                     PushButton        RemoveFilter_PushButton;

               Control              Image_Control;
               VerticalSizer        Image_Sizer;
                  Label                View_Label;
                  HorizontalSizer      View_Sizer;
                     Edit                 View_Edit;
                     ToolButton           View_ToolButton;

            VerticalSizer        FilterThumbnail_Sizer;
               Control              FilterThumbnail_Control;
               HorizontalSizer      FilterInfo_Sizer;
                  Label                FilterInfo_Label;
                  ToolButton           RenderFilter_ToolButton;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateCurrentModeControls();
   void UpdateParametricFilterControls();
   void UpdateLibraryFilterControls();
   void UpdateImageFilterControls();
   void UpdateRealTimePreview();
   void RegenerateFiltersComboBox( int selectedItemIndex );
   void RegenerateFiltersComboBox( const String& selectedFilterName );

   void __Filter_PageSelected( TabBox& sender, int pageIndex );
   void __Filter_ValueUpdated( NumericEdit& sender, double value );
   void __Filter_SliderUpdated( Slider& sender, int value );
   void __Filter_Paint( Control& sender, const Rect& updateRect );
   //void __Library_EditCompleted( Edit& sender );
   void __Library_ItemSelected( ComboBox& sender, int itemIndex );
   void __Library_Click( Button& sender, bool checked );
   void __Image_Click( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ConvolutionInterface* TheConvolutionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ConvolutionInterface_h

// ----------------------------------------------------------------------------
// EOF ConvolutionInterface.h - Released 2017-07-18T16:14:18Z
