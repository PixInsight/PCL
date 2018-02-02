//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 01.05.00.0397
// ----------------------------------------------------------------------------
// RawPreferencesDialog.h - Released 2018-02-02T19:48:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
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

#ifndef __RawPreferencesDialog_h
#define __RawPreferencesDialog_h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "RawPreferences.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RawPreferencesDialog : public Dialog
{
public:

   RawPreferencesDialog( RawPreferences& );

private:

   RawPreferences&   preferences;
   RawPreferences    savedPreferences;

   VerticalSizer     Global_Sizer;

      GroupBox          OutputOptions_GroupBox;
      VerticalSizer     OutputOptions_Sizer;
         HorizontalSizer   CreateSuperPixels_Sizer;
            CheckBox          CreateSuperPixels_CheckBox;
         HorizontalSizer   OutputRawRGB_Sizer;
            CheckBox          OutputRawRGB_CheckBox;
         HorizontalSizer   OutputCFA_Sizer;
            CheckBox          OutputCFA_CheckBox;
         HorizontalSizer   NoAutoFlip_Sizer;
            CheckBox          NoAutoFlip_CheckBox;
         HorizontalSizer   NoAutoCrop_Sizer;
            CheckBox          NoAutoCrop_CheckBox;
         HorizontalSizer   NoBlackPointCorrection_Sizer;
            CheckBox          NoBlackPointCorrection_CheckBox;
         HorizontalSizer   NoClipHighlights_Sizer;
            CheckBox          NoClipHighlights_CheckBox;
         NumericControl    NoiseThreshold_NumericControl;

      GroupBox          Interpolation_GroupBox;
      VerticalSizer     Interpolation_Sizer;
         HorizontalSizer   Bilinear_Sizer;
            RadioButton       Bilinear_RadioButton;
         HorizontalSizer   VNG_Sizer;
            RadioButton       VNG_RadioButton;
         HorizontalSizer   PPG_Sizer;
            RadioButton       PPG_RadioButton;
         HorizontalSizer   AHD_Sizer;
            RadioButton       AHD_RadioButton;
         HorizontalSizer   DCB_Sizer;
            RadioButton       DCB_RadioButton;
         HorizontalSizer   DCBIterations_Sizer;
            Label             DCBIterations_Label;
            SpinBox           DCBIterations_SpinBox;
         HorizontalSizer   DCBRefinement_Sizer;
            CheckBox          DCBRefinement_CheckBox;
         HorizontalSizer   DHT_Sizer;
            RadioButton       DHT_RadioButton;
         HorizontalSizer   AAHD_Sizer;
            RadioButton       AAHD_RadioButton;
         HorizontalSizer   HalfSize_Sizer;
            RadioButton       HalfSize_RadioButton;
         HorizontalSizer   InterpolateAs4Colors_Sizer;
            CheckBox          InterpolateAs4Colors_CheckBox;
         HorizontalSizer   FBDDNoiseReduction_Sizer;
            Label             FBDDNoiseReduction_Label;
            SpinBox           FBDDNoiseReduction_SpinBox;

      GroupBox          WhiteBalance_GroupBox;
      VerticalSizer     WhiteBalance_Sizer;
         HorizontalSizer   UseAutoWhiteBalance_Sizer;
            CheckBox          UseAutoWhiteBalance_CheckBox;
         HorizontalSizer   UseCameraWhiteBalance_Sizer;
            CheckBox          UseCameraWhiteBalance_CheckBox;
         HorizontalSizer   NoWhiteBalance_Sizer;
            CheckBox          NoWhiteBalance_CheckBox;

      HorizontalSizer   BottomRow_Sizer;
         PushButton        PureRaw_PushButton;
         PushButton        PureRGB_PushButton;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void NumericControl_ValueUpdated( NumericControl& sender, double value );
   void SpinBox_ValueUpdated( SpinBox& sender, int value );
   void Button_Click( Button& sender, bool checked );
   void Dialog_Show( Control& sender );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RawPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF RawPreferencesDialog.h - Released 2018-02-02T19:48:42Z
