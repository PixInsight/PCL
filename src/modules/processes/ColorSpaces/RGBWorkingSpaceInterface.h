//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0294
// ----------------------------------------------------------------------------
// RGBWorkingSpaceInterface.h - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#ifndef __RGBWorkingSpaceInterface_h
#define __RGBWorkingSpaceInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/NumericControl.h>
#include <pcl/PushButton.h>
#include <pcl/CheckBox.h>
#include <pcl/ViewList.h>

#include "RGBWorkingSpaceInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// RGBWorkingSpaceInterface
// ----------------------------------------------------------------------------

class RGBWorkingSpaceInterface : public ProcessInterface
{
public:

   RGBWorkingSpaceInterface();
   virtual ~RGBWorkingSpaceInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   virtual void TrackViewUpdated( bool active );
   virtual void ResetInstance();

   virtual void Initialize();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool WantsImageNotifications() const;
   virtual void ImageUpdated( const View& v );
   virtual void ImageFocused( const View& v );
   virtual void ImageRGBWSUpdated( const View& v );

   virtual bool WantsGlobalNotifications() const;
   virtual void GlobalRGBWSUpdated();

   // -------------------------------------------------------------------------

private:

   RGBWorkingSpaceInstance instance;

   struct GUIData
   {
      GUIData( RGBWorkingSpaceInterface& );

      VerticalSizer     Global_Sizer;

      ViewList          AllImages_ViewList;

      SectionBar        LuminanceCoefficients_SectionBar;
      Control           LuminanceCoefficients_Control;
      VerticalSizer     LuminanceCoefficients_Sizer;
         NumericControl    RY_NumericControl;
         NumericControl    GY_NumericControl;
         NumericControl    BY_NumericControl;

      SectionBar        ChromaticityCoordinates_SectionBar;
      Control           ChromaticityCoordinates_Control;
      VerticalSizer     ChromaticityCoordinates_Sizer;
         HorizontalSizer   xyTitle_Sizer;
            Label             RxyTitle_Label;
            Label             GxyTitle_Label;
            Label             BxyTitle_Label;
         HorizontalSizer   xValues_Sizer;
            Label             x_Label;
            NumericEdit       Rx_NumericEdit;
            NumericEdit       Gx_NumericEdit;
            NumericEdit       Bx_NumericEdit;
         HorizontalSizer   yValues_Sizer;
            Label             y_Label;
            NumericEdit       Ry_NumericEdit;
            NumericEdit       Gy_NumericEdit;
            NumericEdit       By_NumericEdit;

      SectionBar        Gamma_SectionBar;
      Control           Gamma_Control;
      VerticalSizer     Gamma_Sizer;
         NumericControl    Gamma_NumericControl;
         HorizontalSizer      sRGBGamma_Sizer;
            CheckBox          sRGBGamma_CheckBox;

      Control           BottomSectionSeparator_Control;

      HorizontalSizer   LoadDefaultRGBWS_Sizer;
         PushButton        LoadDefaultRGBWS_PushButton;
         PushButton        NormalizeY_PushButton;

      HorizontalSizer   LoadGlobalRGBWS_Sizer;
         PushButton        LoadGlobalRGBWS_PushButton;
         CheckBox          ApplyGlobalRGBWS_CheckBox;
   };

   GUIData* GUI;

   void UpdateControls();

   //
   // GUI Event Handlers
   //
   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __LuminanceCoefficient_ValueUpdated( NumericEdit& sender, double value );
   void __ChromaticityCoordinate_ValueUpdated( NumericEdit& sender, double value );
   void __Gamma_ValueUpdated( NumericEdit& sender, double value );
   void __NormalizeY_ButtonClick( Button& sender, bool checked );
   void __sRGBGamma_ButtonClick( Button& sender, bool checked );
   void __LoadRGBWS_ButtonClick( Button& sender, bool checked );
   void __ApplyGlobalRGBWS_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RGBWorkingSpaceInterface* TheRGBWorkingSpaceInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RGBWorkingSpaceInterface_h

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceInterface.h - Released 2015/12/18 08:55:08 UTC
