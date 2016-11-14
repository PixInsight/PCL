//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.00.0320
// ----------------------------------------------------------------------------
// CropInterface.h - Released 2016/11/14 19:38:23 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __CropInterface_h
#define __CropInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/ViewList.h>

#include "CropInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CropInterface : public ProcessInterface
{
public:

   CropInterface();
   virtual ~CropInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual void ApplyInstance() const;
   virtual void TrackViewUpdated( bool active );
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool WantsImageNotifications() const;
   virtual void ImageUpdated( const View& );
   virtual void ImageFocused( const View& );

   virtual bool WantsReadoutNotifications() const;
   virtual void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A );

private:

   CropInstance instance;

   // Source dimensions in pixels
   int sourceWidth, sourceHeight;

   // Current anchor point:
   // 0 1 2
   // 3 4 5
   // 6 7 8
   int anchor;

   struct GUIData
   {
      GUIData( CropInterface& );

      VerticalSizer     Global_Sizer;

      ViewList          AllImages_ViewList;

      SectionBar        CropMargins_SectionBar;
      Control           CropMargins_Control;
      VerticalSizer     CropMargins_Sizer;
         HorizontalSizer   CropMarginsTop_Sizer;
            NumericEdit       TopMargin_NumericEdit;
         HorizontalSizer   CropMarginsMiddle_Sizer;
            NumericEdit       LeftMargin_NumericEdit;
            VerticalSizer        AnchorButtons_Sizer;
               HorizontalSizer      AnchorRow1_Sizer;
                  ToolButton           TL_ToolButton;
                  ToolButton           TM_ToolButton;
                  ToolButton           TR_ToolButton;
               HorizontalSizer      AnchorRow2_Sizer;
                  ToolButton           ML_ToolButton;
                  ToolButton           MM_ToolButton;
                  ToolButton           MR_ToolButton;
               HorizontalSizer      AnchorRow3_Sizer;
                  ToolButton           BL_ToolButton;
                  ToolButton           BM_ToolButton;
                  ToolButton           BR_ToolButton;
            NumericEdit       RightMargin_NumericEdit;
         HorizontalSizer   CropMarginsBottom_Sizer;
            NumericEdit       BottomMargin_NumericEdit;

      SectionBar        Dimensions_SectionBar;
      Control           Dimensions_Control;
      VerticalSizer     Dimensions_Sizer;
         HorizontalSizer   DimensionsRow1_Sizer;
            Label             SourcePixels_Label;
            Label             TargetPixels_Label;
            Label             TargetPercent_Label;
            Label             TargetCentimeters_Label;
            Label             TargetInches_Label;
         HorizontalSizer   DimensionsRow2_Sizer;
            Label             Width_Label;
            NumericEdit       SourceWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPercent_NumericEdit;
            NumericEdit       TargetWidthCentimeters_NumericEdit;
            NumericEdit       TargetWidthInches_NumericEdit;
         HorizontalSizer   DimensionsRow3_Sizer;
            Label             Height_Label;
            NumericEdit       SourceHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPercent_NumericEdit;
            NumericEdit       TargetHeightCentimeters_NumericEdit;
            NumericEdit       TargetHeightInches_NumericEdit;
         HorizontalSizer   DimensionsRow4_Sizer;
            Label             SizeInfo_Label;

      SectionBar        Resolution_SectionBar;
      Control           Resolution_Control;
      VerticalSizer     Resolution_Sizer;
         HorizontalSizer   ResolutionRow1_Sizer;
            NumericEdit       HorizontalResolution_NumericEdit;
            NumericEdit       VerticalResolution_NumericEdit;
         HorizontalSizer   ResolutionRow2_Sizer;
            RadioButton       CentimeterUnits_RadioButton;
            RadioButton       InchUnits_RadioButton;
            CheckBox          ForceResolution_CheckBox;

      SectionBar        Mode_SectionBar;
      Control           Mode_Control;
      HorizontalSizer   Mode_Sizer;
         Label             CropMode_Label;
         ComboBox          CropMode_ComboBox;

      SectionBar        FillColor_SectionBar;
      Control           FillColor_Control;
      VerticalSizer     FillColor_Sizer;
         NumericControl    Red_NumericControl;
         NumericControl    Green_NumericControl;
         NumericControl    Blue_NumericControl;
         NumericControl    Alpha_NumericControl;
         Control           ColorSample_Control;

      Bitmap L_Bitmap, R_Bitmap, T_Bitmap, B_Bitmap, TL_Bitmap, TR_Bitmap, BL_Bitmap, BR_Bitmap;
   };

   GUIData* GUI;

   bool dragging; // dragging the mouse cursor on the angle dial

   void UpdateControls();
   void UpdateAnchors();
   void UpdateNumericControls();
   void UpdateFillColorControls();

   /*
    * Event Handlers
    */

   void __ViewList_ViewSelected( ViewList& sender, View& view );

   void __Margin_ValueUpdated( NumericEdit& sender, double value );

   void __Anchor_ButtonClick( Button& sender, bool checked );

   void __Width_ValueUpdated( NumericEdit& sender, double value );
   void __Height_ValueUpdated( NumericEdit& sender, double value );

   void __Resolution_ValueUpdated( NumericEdit& sender, double value );
   void __Units_ButtonClick( Button& sender, bool checked );
   void __ForceResolution_ButtonClick( Button& sender, bool checked );

   void __Mode_ItemSelected( ComboBox& sender, int itemIndex );

   void __FilColor_ValueUpdated( NumericEdit& sender, double value );

   void __ColorSample_Paint( Control& sender, const Rect& updateRect );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CropInterface* TheCropInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CropInterface_h

// ----------------------------------------------------------------------------
// EOF CropInterface.h - Released 2016/11/14 19:38:23 UTC
