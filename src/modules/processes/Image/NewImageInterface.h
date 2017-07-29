//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0371
// ----------------------------------------------------------------------------
// NewImageInterface.h - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __NewImageInterface_h
#define __NewImageInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "NewImageInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class NewImageInterface : public ProcessInterface
{
public:

   NewImageInterface();
   virtual ~NewImageInterface();

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

   virtual bool WantsReadoutNotifications() const;
   virtual void UpdateReadout( const View&, const pcl::DPoint&, double R, double G, double B, double A );

private:

   NewImageInstance instance;

   struct GUIData
   {
      GUIData( NewImageInterface& );

      VerticalSizer     Global_Sizer;
         GroupBox          ImageParameters_GroupBox;
         VerticalSizer     ImageParameters_Sizer;
            HorizontalSizer   Identifier_Sizer;
               Label             Identifier_Label;
               Edit              Identifier_Edit;
            HorizontalSizer   SampleFormat_Sizer;
               Label             SampleFormat_Label;
               ComboBox          SampleFormat_ComboBox;
            HorizontalSizer   ColorSpace_Sizer;
               Label             ColorSpace_Label;
               RadioButton       RGBColor_RadioButton;
               RadioButton       Grayscale_RadioButton;
         HorizontalSizer   Geometry_Sizer;
            VerticalSizer     GeometryLeft_Sizer;
               HorizontalSizer   Width_Sizer;
                  Label             Width_Label;
                  SpinBox           Width_SpinBox;
               HorizontalSizer   Height_Sizer;
                  Label             Height_Label;
                  SpinBox           Height_SpinBox;
               HorizontalSizer   Channels_Sizer;
                  Label             Channels_Label;
                  SpinBox           Channels_SpinBox;
            VerticalSizer     GeometryMiddle_Sizer;
               Label             SizeInfo_Label;
            VerticalSizer     GeometryRight_Sizer;
               PushButton        SetAsActiveImage_Button;
         GroupBox          InitialValues_GroupBox;
         HorizontalSizer   InitialValues_Sizer;
            VerticalSizer     InitialValuesLeft_Sizer;
               NumericControl    V0_NumericControl;
               NumericControl    V1_NumericControl;
               NumericControl    V2_NumericControl;
               NumericControl    VA_NumericControl;
            Control           ColorSample_Control;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateInitialValue( Edit&, Slider&, double );
   void UpdateSizeInfo();

   void __Identifier_GetFocus( Control& sender );
   void __Identifier_EditCompleted( Edit& sender );
   void __SampleFormat_ItemSelected( ComboBox& sender, int itemIndex );
   void __ColorSpace_Click( Button& sender, bool checked );
   void __Geometry_ValueUpdated( SpinBox& sender, int value );
   void __SetAsActiveImage_Click( Button& sender, bool checked );
   void __InitialValue_ValueUpdated( NumericEdit& sender, double value );
   void __ColorSample_Paint( Control& sender, const Rect& updateRect );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   void SetAsImage( const ImageWindow& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern NewImageInterface* TheNewImageInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NewImageInterface_h

// ----------------------------------------------------------------------------
// EOF NewImageInterface.h - Released 2017-05-02T09:43:00Z
