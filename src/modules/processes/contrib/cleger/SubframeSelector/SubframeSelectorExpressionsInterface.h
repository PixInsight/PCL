//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0012
// ----------------------------------------------------------------------------
// SubframeSelectorExpressionsInterface.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#ifndef e_SubframeSelectorExpressionsInterface_h
#define e_SubframeSelectorExpressionsInterface_h

#include <pcl/BitmapBox.h>
#include <pcl/CodeEditor.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorInstance;

class SubframeSelectorExpressionsInterface : public ProcessInterface
{
public:

   SubframeSelectorExpressionsInterface( SubframeSelectorInstance& );
   virtual ~SubframeSelectorExpressionsInterface();

   IsoString Id() const override;

   MetaProcess* Process() const override;

   const char** IconImageXPM() const override;
   InterfaceFeatures Features() const override;

   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;

   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   using ProcessInterface::Launch;

   void ApplyApprovalExpression();
   void ApplyWeightingExpression();

private:

   SubframeSelectorInstance& m_instance;

   struct GUIData
   {
      GUIData( SubframeSelectorExpressionsInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   ExpressionParameters_Approval_Sizer;
         VerticalSizer     ExpressionParameters_Approval_Label_Sizer1;
            HorizontalSizer   ExpressionParameters_Approval_Label_Sizer2;
               BitmapBox         ExpressionParameters_Approval_Status;
               Label             ExpressionParameters_Approval_Label;
         CodeEditor        ExpressionParameters_Approval_Editor;
         ToolButton        ExpressionParameters_Approval_ToolButton;
      HorizontalSizer   ExpressionParameters_Weighting_Sizer;
         VerticalSizer     ExpressionParameters_Weighting_Label_Sizer1;
            HorizontalSizer   ExpressionParameters_Weighting_Label_Sizer2;
               BitmapBox         ExpressionParameters_Weighting_Status;
               Label             ExpressionParameters_Weighting_Label;
         CodeEditor        ExpressionParameters_Weighting_Editor;
         ToolButton        ExpressionParameters_Weighting_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   /*
    * Event Handlers
    */
   void e_TextUpdated( CodeEditor& sender );
   void e_KeyPressed( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void e_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
   friend class SubframeSelectorInterface;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorExpressionsInterface* TheSubframeSelectorExpressionsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // e_SubframeSelectorExpressionsInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorExpressionsInterface.h - Released 2018-11-23T18:45:58Z
