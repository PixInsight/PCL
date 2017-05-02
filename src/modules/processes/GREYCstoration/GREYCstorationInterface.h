//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0291
// ----------------------------------------------------------------------------
// GREYCstorationInterface.h - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#ifndef __GREYCstorationInterface_h
#define __GREYCstorationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/SpinBox.h>
#include <pcl/ComboBox.h>
#include <pcl/CheckBox.h>

#include "GREYCstorationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// GREYCstorationInterface
// ----------------------------------------------------------------------------

class GREYCstorationInterface : public ProcessInterface
{
public:

   GREYCstorationInterface();
   virtual ~GREYCstorationInterface();

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

   // -------------------------------------------------------------------------

private:

   GREYCstorationInstance instance;

   // -------------------------------------------------------------------------

   struct GUIData
   {
      GUIData( GREYCstorationInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   Iterations_Sizer;
         Label             Iterations_Label;
         SpinBox           Iterations_SpinBox;
      NumericControl    Amplitude_NumericControl;
      NumericControl    Sharpness_NumericControl;
      NumericControl    Anisotropy_NumericControl;
      NumericControl    Alpha_NumericControl;
      NumericControl    Sigma_NumericControl;
      NumericControl    SpatialStepSize_NumericControl;
      NumericControl    AngularStepSize_NumericControl;
      NumericControl    Precision_NumericControl;
      HorizontalSizer   Interpolation_Sizer;
         Label             Interpolation_Label;
         ComboBox          Interpolation_ComboBox;
      HorizontalSizer   FastApproximation_Sizer;
         CheckBox          FastApproximation_CheckBox;
         CheckBox          CoupledChannels_CheckBox;
   };

   GUIData* GUI;

   void UpdateControls();

   // Event Handlers

   void __ValueUpdated( NumericEdit& sender, double value );
   void __IntValueUpdated( SpinBox& sender, int value );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GREYCstorationInterface* TheGREYCstorationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GREYCstorationInterface_h

// ----------------------------------------------------------------------------
// EOF GREYCstorationInterface.h - Released 2017-05-02T09:43:00Z
