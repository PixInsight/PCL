//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 01.00.04.0233
// ----------------------------------------------------------------------------
// InverseFourierTransformInterface.h - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
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

#ifndef __InverseFourierTransformInterface_h
#define __InverseFourierTransformInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/ToolButton.h>
#include <pcl/ComboBox.h>

#include "InverseFourierTransformInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class InverseFourierTransformInterface : public ProcessInterface
{
public:

   InverseFourierTransformInterface();
   virtual ~InverseFourierTransformInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   InverseFourierTransformInstance instance;

   struct GUIData
   {
      GUIData( InverseFourierTransformInterface& );

      VerticalSizer  Global_Sizer;
      HorizontalSizer   FirstComponent_Sizer;
         Label             FirstComponent_Label;
         Edit              FirstComponent_Edit;
         ToolButton        FirstComponent_ToolButton;
      HorizontalSizer   SecondComponent_Sizer;
         Label             SecondComponent_Label;
         Edit              SecondComponent_Edit;
         ToolButton        SecondComponent_ToolButton;
      HorizontalSizer   OnOutOfRangeResult_Sizer;
         Label             OnOutOfRangeResult_Label;
         ComboBox          OnOutOfRangeResult_ComboBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __EditCompleted( Edit& sender );
   void __Click( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

extern InverseFourierTransformInterface* TheInverseFourierTransformInterface;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __InverseFourierTransformInterface_h

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformInterface.h - Released 2017-07-18T16:14:18Z
