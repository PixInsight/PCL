//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.08.0405
// ----------------------------------------------------------------------------
// ReadoutOptionsInterface.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ReadoutOptionsInterface_h
#define __ReadoutOptionsInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "ReadoutOptionsInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ReadoutOptionsInterface : public ProcessInterface
{
public:

   ReadoutOptionsInterface();
   virtual ~ReadoutOptionsInterface();

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

   ReadoutOptionsInstance instance;

   struct GUIData
   {
      GUIData( ReadoutOptionsInterface& );

      HorizontalSizer   Global_Sizer;

         VerticalSizer     Left_Sizer;
            HorizontalSizer   Data_Sizer;
               Label             Data_Label;
               ComboBox          Data_ComboBox;
            HorizontalSizer   Mode_Sizer;
               Label             Mode_Label;
               ComboBox          Mode_ComboBox;
            HorizontalSizer   ProbeSize_Sizer;
               Label             ProbeSize_Label;
               ComboBox          ProbeSize_ComboBox;
            HorizontalSizer   Alpha_Sizer;
               CheckBox          ShowAlphaChannel_CheckBox;
            HorizontalSizer   Mask_Sizer;
               CheckBox          ShowMaskChannel_CheckBox;
            HorizontalSizer   Preview_Sizer;
               CheckBox          ShowPreview_CheckBox;
            HorizontalSizer   PreviewCenter_Sizer;
               CheckBox          PreviewCenter_CheckBox;
            HorizontalSizer   PreviewSize_Sizer;
               Label             PreviewSize_Label;
               SpinBox           PreviewSize_SpinBox;
            HorizontalSizer   PreviewZoom_Sizer;
               Label             PreviewZoom_Label;
               ComboBox          PreviewZoom_ComboBox;

         VerticalSizer     Right_Sizer;
            HorizontalSizer   Real_Sizer;
               Label             Precision_Label;
               ComboBox          Precision_ComboBox;
               CheckBox          Real_CheckBox;
            HorizontalSizer   BinaryInteger_Sizer;
               Label             BinaryInteger_Label;
               ComboBox          BinaryInteger_ComboBox;
               CheckBox          BinaryInteger_CheckBox;
            HorizontalSizer   ArbitraryInteger_Sizer;
               Label             ArbitraryInteger_Label;
               SpinBox           ArbitraryInteger_SpinBox;
               CheckBox          ArbitraryInteger_CheckBox;
            HorizontalSizer   ShowEquatorial_Sizer;
               CheckBox          ShowEquatorial_CheckBox;
            HorizontalSizer   ShowEcliptic_Sizer;
               CheckBox          ShowEcliptic_CheckBox;
            HorizontalSizer   ShowGalactic_Sizer;
               CheckBox          ShowGalactic_CheckBox;
            HorizontalSizer   CoordinateItems_Sizer;
               Label             CoordinateItems_Label;
               ComboBox          CoordinateItems_ComboBox;
            HorizontalSizer   CoordinatePrecision_Sizer;
               Label             CoordinatePrecision_Label;
               SpinBox           CoordinatePrecision_SpinBox;
            HorizontalSizer   Broadcast_Sizer;
               CheckBox          Broadcast_CheckBox;
            HorizontalSizer   LoadCurrentOptions_Sizer;
               PushButton        LoadCurrentOptions_PushButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ButtonClick( Button& sender, bool checked );
   void __ValueUpdated( SpinBox& sender, int value );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ReadoutOptionsInterface* TheReadoutOptionsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ReadoutOptionsInterface_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsInterface.h - Released 2019-01-21T12:06:41Z
