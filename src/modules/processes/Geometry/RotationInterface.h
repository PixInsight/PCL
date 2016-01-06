//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0310
// ----------------------------------------------------------------------------
// RotationInterface.h - Released 2015/12/18 08:55:08 UTC
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

#ifndef __RotationInterface_h
#define __RotationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/Label.h>
#include <pcl/SpinBox.h>
#include <pcl/ComboBox.h>
#include <pcl/CheckBox.h>
#include <pcl/NumericControl.h>

#include "RotationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// RotationInterface
// ----------------------------------------------------------------------------

class RotationInterface : public ProcessInterface
{
public:

   RotationInterface();
   virtual ~RotationInterface();

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

   virtual bool WantsReadoutNotifications() const;
   virtual void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A );

   // -------------------------------------------------------------------------

private:

   RotationInstance instance;

   // -------------------------------------------------------------------------

   struct GUIData
   {
      GUIData( RotationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Rotation_SectionBar;
      Control           Rotation_Control;
      HorizontalSizer   Rotation_Sizer;
         VerticalSizer     RotationLeft_Sizer;
            NumericEdit       Angle_NumericEdit;
            HorizontalSizer   Clockwise_Sizer;
               Label             Clockwise_Label;
               CheckBox          Clockwise_CheckBox;
            HorizontalSizer   OptimizeFast_Sizer;
               Label             OptimizeFast_Label;
               CheckBox          OptimizeFast_CheckBox;
         Control           Dial_Control;

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

   bool dragging; // dragging the mouse cursor on the angle dial

   void UpdateControls();
   void UpdateNumericControls();
   void UpdateFillColorControls();

   /*
    * GUI Event Handlers
    */

   void __Angle_ValueUpdated( NumericEdit& sender, double value );
   void __Clockwise_Click( Button& sender, bool checked );
   void __OptimizeFast_Click( Button& sender, bool checked );

   void __AngleDial_Paint( Control& sender, const Rect& updateRect );
   void __AngleDial_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void __AngleDial_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __AngleDial_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );

   void __Algorithm_ItemSelected( ComboBox& sender, int itemIndex );
   void __Algorithm_ValueUpdated( NumericEdit& sender, double value );

   void __FilColor_ValueUpdated( NumericEdit& sender, double value );

   void __ColorSample_Paint( Control& sender, const Rect& updateRect );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RotationInterface* TheRotationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RotationInterface_h

// ----------------------------------------------------------------------------
// EOF RotationInterface.h - Released 2015/12/18 08:55:08 UTC
