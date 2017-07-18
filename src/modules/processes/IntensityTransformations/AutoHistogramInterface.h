//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0397
// ----------------------------------------------------------------------------
// AutoHistogramInterface.h - Released 2017-07-18T16:14:18Z
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

#ifndef __AutoHistogramInterface_h
#define __AutoHistogramInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/RadioButton.h>
#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/GroupBox.h>
#include <pcl/ComboBox.h>

#include "AutoHistogramInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class AutoHistogramInterface : public ProcessInterface
{
public:

   AutoHistogramInterface();
   virtual ~AutoHistogramInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;

   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool WantsReadoutNotifications() const;
   virtual void UpdateReadout( const View&, const pcl::DPoint&, double R, double G, double B, double A );

private:

   AutoHistogramInstance instance;

   struct GUIData
   {
      GUIData( AutoHistogramInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        ClipParameters_SectionBar;
      Control           ClipParameters_Control;
      VerticalSizer     ClipParameters_Sizer;
         HorizontalSizer   ClipMode_Sizer;
            RadioButton       ClipTogether_RadioButton;
            RadioButton       ClipSeparate_RadioButton;
         HorizontalSizer   ClipValues_Sizer;
            GroupBox          ClipLow_GroupBox;
            VerticalSizer     ClipLow_Sizer;
               NumericControl    ClipLowRK_NumericControl;
               NumericControl    ClipLowG_NumericControl;
               NumericControl    ClipLowB_NumericControl;
            GroupBox          ClipHigh_GroupBox;
            VerticalSizer     ClipHigh_Sizer;
               NumericControl    ClipHighRK_NumericControl;
               NumericControl    ClipHighG_NumericControl;
               NumericControl    ClipHighB_NumericControl;

      SectionBar      StretchParameters_SectionBar;
      Control         StretchParameters_Control;
      VerticalSizer   StretchParameters_Sizer;
         HorizontalSizer   StretchMethod_Sizer;
            Label             StretchMethod_Label;
            ComboBox          StretchMethod_ComboBox;
         HorizontalSizer   StretchMode_Sizer;
            RadioButton       StretchTogether_RadioButton;
            RadioButton       StretchSeparate_RadioButton;
         HorizontalSizer   TargetMedian_Sizer;
            VerticalSizer     TargetMedianValues_Sizer;
               NumericControl    TargetMedianRK_NumericControl;
               NumericControl    TargetMedianG_NumericControl;
               NumericControl    TargetMedianB_NumericControl;
            Control           ColorSample_Control;
         HorizontalSizer   CaptureReadouts_Sizer;
            CheckBox          CaptureReadouts_CheckBox;
         PushButton        SetToActiveImage_Button;
   };

   GUIData* GUI;

   void UpdateControls();

   void __Click( Button& sender, bool checked );
   void __CheckSection( SectionBar& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ValueUpdated( NumericEdit& sender, double value );
   void __Paint( Control& sender, const Rect& updateRect );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AutoHistogramInterface* TheAutoHistogramInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AutoHistogramInterface_h

// ----------------------------------------------------------------------------
// EOF AutoHistogramInterface.h - Released 2017-07-18T16:14:18Z
