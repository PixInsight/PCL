//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0310
// ----------------------------------------------------------------------------
// DynamicCropInterface.h - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __DynamicCropInterface_h
#define __DynamicCropInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Graphics.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "DynamicCropInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS View;

class DynamicCropInterface : public ProcessInterface
{
public:

   DynamicCropInterface();
   virtual ~DynamicCropInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual void Execute();
   virtual void EditPreferences();
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool IsDynamicInterface() const;

   virtual void ExitDynamicMode();

   virtual void DynamicMouseEnter( View& );
   virtual void DynamicMouseMove( View&, const DPoint&, unsigned buttons, unsigned modifiers );
   virtual void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );
   virtual void DynamicMouseRelease( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );
   virtual void DynamicMouseDoubleClick( View&, const DPoint&, unsigned buttons, unsigned modifiers );
   virtual bool DynamicKeyPress( View& v, int key, unsigned modifiers );

   virtual bool RequiresDynamicUpdate( const View&, const DRect& ) const;
   virtual void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const;

   virtual bool WantsReadoutNotifications() const;
   virtual void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A );

   virtual void SaveSettings() const;
   virtual void LoadSettings();

private:

   DynamicCropInstance instance;

   /*
    * Workbench
    */

   union Flags
   {
      struct
      {
         bool moving       :  1; // moving the cropping rectangle
         bool movingCenter :  1; // moving the center of rotation
         bool rotating     :  1; // rotating the cropping rectangle
         bool resizing     :  1; // resizing the cropping rectangle
         bool resizeLeft   :  1; // resizing on left edge
         bool resizeTop    :  1; // resizing on top edge
         bool resizeRight  :  1; // resizing on right edge
         bool resizeBottom :  1; // resizing on bottom edge
         int               : 24;
      }
      bits;

      uint32 allBits;

      Flags() { allBits = 0; }
      Flags( const Flags& x ) { allBits = x.allBits; }

      bool operator ==( const Flags& x ) const { return allBits == x.allBits; }
   };

   View*    view;             // dynamic target
   double   width, height;    // dimensions of cropping rectangle
   DPoint   center;           // center of cropping rectangle
   DPoint   rotationCenter;   // center of rotation
   bool     rotationFixed;    // rotationCenter fixed, or moves with crop rect.
   int      anchorPoint;      // anchor point from 0=top/left to 8=bottom/right
   DPoint   anchor;           // position of anchor point

   Flags    flags;            // current operation flags

   bool     dragging;         // dragging the mouse
   DPoint   dragOrigin;       // initial drag position

   bool     initializing;     // defining the initial cropping rectangle
   Rect     rect;             // initial cropping rectangle

   RGBA     selectionColor;   // color for the cropping rectangle
   RGBA     centerColor;      // color for the center mark
   RGBA     fillColor;        // color to fill the cropping rectangle

   void Initialize( const Rect& );

   void GetRotatedRect( DPoint& topLeft, DPoint& topRight, DPoint& bottomLeft, DPoint& bottomRight ) const;
   void GetRotatedBounds( DRect& ) const;

   void GetUnrotatedRect( DRect& ) const;

   void PaintRect( VectorGraphics&, ImageWindow& ) const;

   Flags OperationInfo( const DPoint& ) const;

   void UpdateDynamicCursor() const;

   void BeginOperation( const DPoint&, unsigned modifiers );
   void UpdateOperation( const DPoint&, unsigned modifiers );
   void UpdateRotation( DPoint&, unsigned modifiers );
   void UpdateCenterMove( DPoint&, unsigned modifiers );
   void UpdateMove( DPoint&, unsigned modifiers );
   void UpdateResize( DPoint&, unsigned modifiers );
   void EndOperation();

   void SetRotationAngle( double a );
   void SetRotationCenter( const DPoint& );
   void MoveTo( const DPoint& );
   void ResizeBy( double dL, double dT, double dR, double dB );

   void UpdateAnchorPosition();

   void UpdateInstance();

   bool IsPointInsideRect( const DPoint& ) const;
   bool IsPointNearRect( const DPoint& ) const;
   bool IsPointOnRectEdges( const DPoint&, bool& left, bool& top, bool& right, bool& bottom ) const;
   bool IsPointOnRectCenter( const DPoint& ) const;
   bool IsPointOnRotationCenter( const DPoint& ) const;

   // Rotation angle of a point with respect to the center of the cropping system
   double RotationAngle( const DPoint& p ) const
   {
      return ArcTan( rotationCenter.y - p.y, p.x - rotationCenter.x );
   }

   /*
    * GUI
    */

   struct GUIData
   {
      GUIData( DynamicCropInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        SizePos_SectionBar;
      Control           SizePos_Control;
      HorizontalSizer   SizePos_Sizer;
         VerticalSizer     SizePosLeft_Sizer;
            NumericEdit       Width_NumericEdit;
            NumericEdit       Height_NumericEdit;
            NumericEdit       PosX_NumericEdit;
            NumericEdit       PosY_NumericEdit;
         Control           AnchorSelectors_Control;

      SectionBar        Rotation_SectionBar;
      Control           Rotation_Control;
      VerticalSizer     Rotation_Sizer;
         HorizontalSizer   RotationTop_Sizer;
            VerticalSizer     RotationLeft_Sizer;
               NumericEdit       Angle_NumericEdit;
               HorizontalSizer   Clockwise_Sizer;
                  Label             Clockwise_Label;
                  CheckBox          Clockwise_CheckBox;
               NumericEdit       CenterX_NumericEdit;
               NumericEdit       CenterY_NumericEdit;
            Control           Dial_Control;
         HorizontalSizer   RotationBottom_Sizer;
            CheckBox          OptimizeFast_CheckBox;

      SectionBar        Scale_SectionBar;
      Control           Scale_Control;
      VerticalSizer     Scale_Sizer;
         HorizontalSizer   ScaleX_Sizer;
            NumericEdit       ScaleX_NumericEdit;
            NumericEdit       ScaledWidth_NumericEdit;
         HorizontalSizer   ScaleY_Sizer;
            NumericEdit       ScaleY_NumericEdit;
            NumericEdit       ScaledHeight_NumericEdit;

      SectionBar        Interpolation_SectionBar;
      Control           Interpolation_Control;
      VerticalSizer     Interpolation_Sizer;
         HorizontalSizer   Algorithm_Sizer;
            Label             Algorithm_Label;
            ComboBox          Algorithm_ComboBox;
         NumericEdit       ClampingThreshold_NumericEdit;
         NumericEdit       Smoothness_NumericEdit;

      SectionBar        FillColor_SectionBar;
      Control           FillColor_Control;
      VerticalSizer     FillColor_Sizer;
         NumericControl    Red_NumericControl;
         NumericControl    Green_NumericControl;
         NumericControl    Blue_NumericControl;
         NumericControl    Alpha_NumericControl;
         Control           ColorSample_Control;
   };

   GUIData* GUI;

   /*
    * GUI Updates
    */

   void InitControls();

   void UpdateControls();
   void UpdateSizePosControls();
   void UpdateRotationControls();
   void UpdateScaleControls();
   void UpdateInterpolationControls();
   void UpdateFillColorControls();

   void UpdateView();

   /*
    * Event Handlers
    */

   void __Size_ValueUpdated( NumericEdit& sender, double value );
   void __Pos_ValueUpdated( NumericEdit& sender, double value );

   void __AnchorSelector_Paint( Control& sender, const Rect& updateRect );
   void __AnchorSelector_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __AnchorSelector_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __AnchorSelector_MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );

   void __Angle_ValueUpdated( NumericEdit& sender, double value );
   void __Clockwise_Click( Button& sender, bool checked );
   void __Center_ValueUpdated( NumericEdit& sender, double value );
   void __OptimizeFast_Click( Button& sender, bool checked );

   void __AngleDial_Paint( Control& sender, const Rect& updateRect );
   void __AngleDial_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void __AngleDial_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __AngleDial_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );

   void __Scale_ValueUpdated( NumericEdit& sender, double value );
   void __ScaledSize_ValueUpdated( NumericEdit& sender, double value );

   void __Algorithm_ItemSelected( ComboBox& sender, int itemIndex );
   void __Algorithm_ValueUpdated( NumericEdit& sender, double value );

   void __FilColor_ValueUpdated( NumericEdit& sender, double value );

   void __ColorSample_Paint( Control& sender, const Rect& updateRect );

   //

   friend struct GUIData;
   friend class DynamicCropPreferencesDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern DynamicCropInterface* TheDynamicCropInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicCropInterface_h

// ----------------------------------------------------------------------------
// EOF DynamicCropInterface.h - Released 2015/12/18 08:55:08 UTC
