//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 01.00.02.0340
// ----------------------------------------------------------------------------
// SimplexNoiseInterface.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
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

#ifndef __SimplexNoiseInterface_h
#define __SimplexNoiseInterface_h

#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "SimplexNoiseInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SimplexNoiseInterface : public ProcessInterface
{
public:

   SimplexNoiseInterface();
   virtual ~SimplexNoiseInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   SimplexNoiseInstance instance;

   struct GUIData
   {
      GUIData( SimplexNoiseInterface& );

      VerticalSizer     Global_Sizer;

      NumericControl    Amount_NumericControl;
      Control           Scale_Control;
      HorizontalSizer   Scale_Sizer;
         Label             Scale_Label;
         SpinBox           Scale_SpinBox;
         RadioButton       Scale1_RadioButton;
         RadioButton       Scale10_RadioButton;
         RadioButton       Scale100_RadioButton;
      Control           OffsetX_Control;
      HorizontalSizer   OffsetX_Sizer;
         Label             OffsetX_Label;
         SpinBox           OffsetX_SpinBox;
         RadioButton       OffsetX1_RadioButton;
         RadioButton       OffsetX10_RadioButton;
         RadioButton       OffsetX100_RadioButton;
      Control           OffsetY_Control;
      HorizontalSizer   OffsetY_Sizer;
         Label             OffsetY_Label;
         SpinBox           OffsetY_SpinBox;
         RadioButton       OffsetY1_RadioButton;
         RadioButton       OffsetY10_RadioButton;
         RadioButton       OffsetY100_RadioButton;
      HorizontalSizer   Operator_Sizer;
         Label             Operator_Label;
         ComboBox          Operator_ComboBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ValueUpdated( NumericEdit& sender, double value );
   void __IntValueUpdated( SpinBox& sender, int value );
   void __Click( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SimplexNoiseInterface* TheSimplexNoiseInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SimplexNoiseInterface_h

// ----------------------------------------------------------------------------
// EOF SimplexNoiseInterface.h - Released 2018-11-23T18:45:58Z
