//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0166
// ----------------------------------------------------------------------------
// MergeCFAInterface.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2017 Nikolay Volkov
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#ifndef __MergeCFAInterface_h
#define __MergeCFAInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/ViewList.h>
#include <pcl/Label.h>
#include <pcl/Sizer.h>

#include "MergeCFAInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// MergeCFAInterface
// ----------------------------------------------------------------------------

class MergeCFAInterface : public ProcessInterface
{
public:

   MergeCFAInterface();
   virtual ~MergeCFAInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   //virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );


private:

   MergeCFAInstance m_instance;

   struct GUIData
   {
      GUIData( MergeCFAInterface& );

      VerticalSizer     Global_Sizer;
         Label             CFA_Label;
         HorizontalSizer   MergeCFA0_Sizer;
            Label             CFA0_Label;
            ViewList          CFA0_ViewList;
         HorizontalSizer   MergeCFA1_Sizer;
            Label             CFA1_Label;
            ViewList          CFA1_ViewList;
         HorizontalSizer   MergeCFA2_Sizer;
            Label             CFA2_Label;
            ViewList          CFA2_ViewList;
         HorizontalSizer   MergeCFA3_Sizer;
            Label             CFA3_Label;
            ViewList          CFA3_ViewList;
   };

   GUIData* GUI;

   void UpdateControls();

   // Event Handlers

   void __ViewList_ViewSelected( ViewList& sender, View& view );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern MergeCFAInterface* TheMergeCFAInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MergeCFAInterface_h

// ----------------------------------------------------------------------------
// EOF MergeCFAInterface.h - Released 2017-08-01T14:26:58Z
