//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0226
// ----------------------------------------------------------------------------
// LarsonSekaninaInterface.h - Released 2017-05-02T09:43:00Z
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

#ifndef __LarsonSekaninaInterface_h
#define __LarsonSekaninaInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/NumericControl.h>
#include <pcl/GroupBox.h>
#include <pcl/SectionBar.h>
#include <pcl/Control.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>

#include "LarsonSekaninaInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class LarsonSekaninaInterface : public ProcessInterface
{
public:

   LarsonSekaninaInterface();
   virtual ~LarsonSekaninaInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   LarsonSekaninaInstance instance;

   struct GUIData
   {
      GUIData( LarsonSekaninaInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Filter_SectionBar;
      Control           Filter_Control;
      VerticalSizer     Filter_Sizer;
         HorizontalSizer   Radius_Sizer;
            NumericEdit       Radius_NumericEdit;
            VerticalSizer     RadiusSliders_Sizer;
            Slider               RadiusCoarse_Slider;
            Slider               RadiusFine_Slider;
         HorizontalSizer   Angle_Sizer;
            NumericEdit       Angle_NumericEdit;
            VerticalSizer     AngleSliders_Sizer;
            Slider               AngleCoarse_Slider;
            Slider               AngleFine_Slider;
         NumericEdit       X_NumericEdit;
         NumericEdit       Y_NumericEdit;
         HorizontalSizer   Interpolation_Sizer;
           Label             Interpolation_Label;
           ComboBox          Interpolation_ComboBox;

      SectionBar        ApplyMode_SectionBar;
      Control           ApplyMode_Control;
      VerticalSizer     ApplyMode_Sizer;
         NumericControl    Amount_NumericControl;
         NumericControl    Threshold_NumericControl;
         NumericControl    Deringing_NumericControl;
         HorizontalSizer   UseLuminance_Sizer;
            CheckBox          UseLuminance_CheckBox;
            CheckBox          HighPassMode_CheckBox;

      SectionBar        Range_SectionBar;
      Control           Range_Control;
      VerticalSizer     Range_Sizer;
         NumericControl    RangeLow_NumericControl;
         NumericControl    RangeHigh_NumericControl;
         HorizontalSizer   Disable_Sizer;
            CheckBox          Disable_CheckBox;
   };

   GUIData* GUI;

   void UpdateControls();
   void UpdateFilterControls();
   void UpdateApplyModeControls();
   void UpdateRangeControls();

   // Event Handlers

   void __RadiusEdit_ValueUpdated( NumericEdit& sender, double value );
   void __Radius_ValueUpdated( Slider& sender, int value );
   void __AngleEdit_ValueUpdated( NumericEdit& sender, double value );
   void __Angle_ValueUpdated( Slider& sender, int value );
   void __Coords_ValueUpdated( NumericEdit& sender, double value );
   void __Interpolation_ItemSelected( ComboBox& sender, int itemIndex );

   void __Amount_ValueUpdated( NumericEdit& sender, double value );
   void __Threshold_ValueUpdated( NumericEdit& sender, double value );
   void __Deringing_ValueUpdated( NumericEdit& sender, double value );
   void __UseLuminance_Click( Button& sender, bool checked );
   void __HighPassMode_Click( Button& sender, bool checked );

   void __Range_ValueUpdated( NumericEdit& sender, double value );
   void __Disable_Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern LarsonSekaninaInterface* TheLarsonSekaninaInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LarsonSekaninaInterface_h

// ----------------------------------------------------------------------------
// EOF LarsonSekaninaInterface.h - Released 2017-05-02T09:43:00Z
