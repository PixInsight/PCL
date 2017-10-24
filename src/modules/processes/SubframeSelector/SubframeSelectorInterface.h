//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorInterface.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#ifndef __SubframeSelectorInterface_h
#define __SubframeSelectorInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/TreeBox.h>
#include <pcl/SpinBox.h>
#include <pcl/ComboBox.h>
#include <pcl/CheckBox.h>
#include <pcl/Label.h>

#include "SubframeSelectorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorInterface : public ProcessInterface
{
public:

   SubframeSelectorInterface();
   virtual ~SubframeSelectorInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   SubframeSelectorInstance instance;

   struct GUIData
   {
      GUIData( SubframeSelectorInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        SubframeImages_SectionBar;
      Control           SubframeImages_Control;
      HorizontalSizer   SubframeImages_Sizer;
         TreeBox           SubframeImages_TreeBox;
         VerticalSizer     SubframeButtons_Sizer;
            PushButton        AddFiles_PushButton;
            PushButton        SelectAll_PushButton;
            PushButton        InvertSelection_PushButton;
            PushButton        ToggleSelected_PushButton;
            PushButton        RemoveSelected_PushButton;
            PushButton        Clear_PushButton;

      SectionBar        SystemParameters_SectionBar;
      Control           SystemParameters_Control;
      VerticalSizer     SystemParameters_Sizer;
         HorizontalSizer   SystemParameters_SubframeScale_Sizer;
            Label             SystemParameters_SubframeScale_Label;
            NumericControl    SystemParameters_SubframeScale_Control;
            Label             SystemParameters_SubframeScale_Unit;
         HorizontalSizer   SystemParameters_CameraGain_Sizer;
            Label             SystemParameters_CameraGain_Label;
            NumericControl    SystemParameters_CameraGain_Control;
            Label             SystemParameters_CameraGain_Unit;
         HorizontalSizer   SystemParameters_CameraResolution_Sizer;
            Label             SystemParameters_CameraResolution_Label;
            ComboBox          SystemParameters_CameraResolution_Control;
         HorizontalSizer   SystemParameters_SiteLocalMidnight_Sizer;
            Label             SystemParameters_SiteLocalMidnight_Label;
            SpinBox           SystemParameters_SiteLocalMidnight_Control;
            Label             SystemParameters_SiteLocalMidnight_Unit;
         HorizontalSizer   SystemParameters_ScaleUnit_Sizer;
            Label             SystemParameters_ScaleUnit_Label;
            ComboBox          SystemParameters_ScaleUnit_Control;
         HorizontalSizer   SystemParameters_DataUnit_Sizer;
            Label             SystemParameters_DataUnit_Label;
            ComboBox          SystemParameters_DataUnit_Control;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateSubframeImageItem( size_type );
   void UpdateSubframeImagesList();
   void UpdateSubframeImageSelectionButtons();
   void UpdateSystemParameters();

   // Event Handlers

   void __ToggleSection( SectionBar& sender, Control& section, bool start );

   void __SubframeImages_CurrentNodeUpdated( TreeBox &sender, TreeBox::Node &current, TreeBox::Node &oldCurrent );
   void __SubframeImages_NodeActivated( TreeBox &sender, TreeBox::Node &node, int col );
   void __SubframeImages_NodeSelectionUpdated( TreeBox &sender );
   void __SubframeImages_Click( Button &sender, bool checked );

   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __IntegerValueUpdated( SpinBox& sender, int value );
   void __ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorInterface* TheSubframeSelectorInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SubframeSelectorInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.h - Released 2017-08-01T14:26:58Z
