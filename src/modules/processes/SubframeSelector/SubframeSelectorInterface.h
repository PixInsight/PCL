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

   virtual void ApplyInstance() const;
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

      SectionBar        TargetImages_SectionBar;
      Control           TargetImages_Control;
      HorizontalSizer   TargetImages_Sizer;
         TreeBox           TargetImages_TreeBox;
         VerticalSizer     TargetButtons_Sizer;
            PushButton        AddFiles_PushButton;
            PushButton        SelectAll_PushButton;
            PushButton        InvertSelection_PushButton;
            PushButton        ToggleSelected_PushButton;
            PushButton        RemoveSelected_PushButton;
            PushButton        Clear_PushButton;

      SectionBar        Parameters_SectionBar;
      Control           Parameters_Control;
      VerticalSizer     Parameters_Sizer;
         NumericControl    ParameterOne_NumericControl;
         HorizontalSizer   ParameterTwo_Sizer;
            Label             ParameterTwo_Label;
            SpinBox           ParameterTwo_SpinBox;
         HorizontalSizer   ParameterThree_Sizer;
            CheckBox          ParameterThree_CheckBox;
         HorizontalSizer   ParameterFour_Sizer;
            Label             ParameterFour_Label;
            ComboBox          ParameterFour_ComboBox;
         HorizontalSizer   ParameterFive_Sizer;
            Label             ParameterFive_Label;
            Edit              ParameterFive_Edit;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateTargetImageItem( size_type );
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();

   // Event Handlers

   void __ToggleSection( SectionBar& sender, Control& section, bool start );

   void __TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __TargetImages_NodeSelectionUpdated( TreeBox& sender );
   void __TargetImages_Click( Button& sender, bool checked );

   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __IntegerValueUpdated( SpinBox& sender, int value );
   void __ItemClicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __EditGetFocus( Control& sender );
   void __EditCompleted( Edit& sender );

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
