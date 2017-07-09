//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0319
// ----------------------------------------------------------------------------
// StructureManagerDialog.h - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
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

#ifndef __StructureManagerDialog_h
#define __StructureManagerDialog_h

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>

#include "MorphologicalTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class StructureManagerDialog : public Dialog
{
public:

   StructureManagerDialog( MorphologicalTransformationInterface& );

private:

   MorphologicalTransformationInterface& iface;

   VerticalSizer  Global_Sizer;

   GroupBox          Module_GroupBox;
   VerticalSizer     Module_Sizer;
      Label             ModulePath_Label;
      HorizontalSizer   ModuleButtons_Sizer;
         PushButton        LoadModule_PushButton;
         PushButton        SaveModule_PushButton;

   GroupBox          StructureList_GroupBox;
   VerticalSizer     StructureList_Sizer;
      ComboBox          StructureList_ComboBox;
      HorizontalSizer   StructureListButtons_Sizer;
         PushButton        PickStructure_PushButton;
         PushButton        DuplicateStructure_PushButton;
         PushButton        DeleteStructure_PushButton;
         PushButton        DeleteAll_PushButton;

   GroupBox          CurrentStructure_GroupBox;
   VerticalSizer     CurrentStructure_Sizer;
      Label             CurrentStructure_Label;
      HorizontalSizer   CurrentStructureButtons_Sizer;
         PushButton        SaveStructure_PushButton;

   HorizontalSizer   Buttons_Sizer;
      PushButton        Done_PushButton;

   void Update();

   void ModuleClick( Button& sender, bool checked );
   void StructureListClick( Button& sender, bool checked );
   void SaveStructureClick( Button& sender, bool checked );
   void DoneClick( Button& sender, bool checked );
   void ItemSelected( ComboBox& sender, int itemIndex );
   void ControlShow( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StructureManagerDialog_h

// ----------------------------------------------------------------------------
// EOF StructureManagerDialog.h - Released 2017-07-09T18:07:33Z
