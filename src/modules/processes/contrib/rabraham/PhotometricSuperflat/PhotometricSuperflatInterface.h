// ****************************************************************************
// PixInsight Class Library - PCL 02.00.09.0651
// Standard PhotometricSuperflat Process Module Version 01.00.02.0116
// ****************************************************************************
// PhotometricSuperflatInterface.h - Released 2013/12/13 00:03:11 UTC
// ****************************************************************************
// This file is part of the standard PhotometricSuperflat PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PhotometricSuperflatInterface_h
#define __PhotometricSuperflatInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "PhotometricSuperflatInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PhotometricSuperflatInterface : public ProcessInterface
{
public:

   PhotometricSuperflatInterface();
   virtual ~PhotometricSuperflatInterface();

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

   PhotometricSuperflatInstance instance;

   struct GUIData
   {
      GUIData( PhotometricSuperflatInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   StarDatabase_Sizer;
            Label             StarDatabase_Label;
            Edit              StarDatabase_Edit;
            ToolButton        StarDatabase_ToolButton;
       HorizontalSizer   FitDegree_Sizer;
            Label             FitDegree_Label;
            SpinBox           FitDegree_SpinBox;
   };

   GUIData* GUI;

   void UpdateControls();

   // Event Handlers

   void __EditGetFocus( Control& sender );
   void __EditCompleted( Edit& sender );
   void __Button_Clicked( Button& sender, bool checked );
   void __IntegerValueUpdated ( SpinBox& sender, int value );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern PhotometricSuperflatInterface* ThePhotometricSuperflatInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PhotometricSuperflatInterface_h

// ****************************************************************************
// EOF PhotometricSuperflatInterface.h - Released 2013/12/13 00:03:11 UTC
