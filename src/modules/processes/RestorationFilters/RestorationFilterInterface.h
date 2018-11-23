//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 01.00.05.0336
// ----------------------------------------------------------------------------
// RestorationFilterInterface.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
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

#ifndef __RestorationFilterInterface_h
#define __RestorationFilterInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TabBox.h>
#include <pcl/ToolButton.h>

#include "RestorationFilterInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RestorationFilterInterface : public ProcessInterface
{
public:

   RestorationFilterInterface();
   virtual ~RestorationFilterInterface();

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

private:

   RestorationFilterInstance instance;

   struct GUIData
   {
      GUIData( RestorationFilterInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        PSF_SectionBar;
      Control           PSF_Control;
      HorizontalSizer     PSF_Sizer;
         TabBox               PSFMode_TabBox;
            Control              Parametric_Control;
            VerticalSizer        Parametric_Sizer;
               NumericControl       ParametricSigma_NumericControl;
               NumericControl       ParametricShape_NumericControl;
               NumericControl       ParametricRatio_NumericControl;
               NumericControl       ParametricAngle_NumericControl;
            Control              Motion_Control;
            VerticalSizer        Motion_Sizer;
               NumericControl       MotionLength_NumericControl;
               NumericControl       MotionAngle_NumericControl;
            Control              PSFImage_Control;
            VerticalSizer        PSFImage_Sizer;
               Label                PSFImage_Label;
               HorizontalSizer      PSFImageEdit_Sizer;
               Edit                    PSFImage_Edit;
               ToolButton              PSFImage_Button;
         VerticalSizer        PSFDraw_Sizer;
            Control              PSFDraw_Control;
            Label                PSFDraw_Label;

      SectionBar        Noise_SectionBar;
      Control           Noise_Control;
      HorizontalSizer   Noise_Sizer;
         NumericEdit       Noise_NumericControl;
         SpinBox           NoiseFine_SpinBox;
         Label             NoiseCoarse_Label;
         SpinBox           NoiseCoarse_SpinBox;

      SectionBar        Filter_SectionBar;
      Control           Filter_Control;
      VerticalSizer     Filter_Sizer;
         HorizontalSizer   Algorithm_Sizer;
            Label             Algorithm_Label;
            ComboBox          Algorithm_ComboBox;
         NumericControl    Amount_NumericControl;
         HorizontalSizer   Target_Sizer;
            Label             Target_Label;
            ComboBox          Target_ComboBox;

      SectionBar        Deringing_SectionBar;
      Control           Deringing_Control;
      VerticalSizer     Deringing_Sizer;
         NumericControl    DeringingDark_NumericControl;
         NumericControl    DeringingBright_NumericControl;
         HorizontalSizer   OutputDeringingMaps_Sizer;
            CheckBox          OutputDeringingMaps_CheckBox;

      SectionBar        Range_SectionBar;
      Control           Range_Control;
      VerticalSizer     Range_Sizer;
         NumericControl    RangeLow_NumericControl;
         NumericControl    RangeHigh_NumericControl;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdatePSFControls();
   void UpdateNoiseControls();
   void UpdateFilterControls();
   void UpdateDeringingControls();
   void UpdateRangeControls();

   // Event handlers

   void __PSF_PageSelected( TabBox& sender, int pageIndex );
   void __PSF_ValueUpdated( NumericEdit& sender, double value );
   void __PSF_EditCompleted( Edit& sender );
   void __PSF_Click( Button& sender, bool checked );
   void __PSF_Paint( Control& sender, const Rect& updateRect );

   void __Noise_ValueUpdated( NumericEdit& sender, double value );
   void __Noise_SpinValueUpdated( SpinBox& sender, int value );

   void __Filter_ItemSelected( ComboBox& sender, int itemIndex );
   void __Filter_ValueUpdated( NumericEdit& sender, double value );

   void __Target_ItemSelected( ComboBox& sender, int itemIndex );

   void __Deringing_Check( SectionBar& sender, bool checked );
   void __Deringing_ValueUpdated( NumericEdit& sender, double value );
   void __Deringing_Click( Button& sender, bool checked );

   void __Range_ValueUpdated( NumericEdit& sender, double value );

   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RestorationFilterInterface* TheRestorationFilterInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RestorationFilterInterface_h

// ----------------------------------------------------------------------------
// EOF RestorationFilterInterface.h - Released 2018-11-23T18:45:58Z
