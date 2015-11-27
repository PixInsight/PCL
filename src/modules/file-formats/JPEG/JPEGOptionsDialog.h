//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 01.00.03.0285
// ----------------------------------------------------------------------------
// JPEGOptionsDialog.h - Released 2015/11/26 15:59:58 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
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

#ifndef __JPEGOptionsDialog_h
#define __JPEGOptionsDialog_h

#include "JPEG.h"

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/CheckBox.h>
#include <pcl/Label.h>
#include <pcl/SpinBox.h>
#include <pcl/Slider.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class JPEGOptionsDialog : public Dialog
{
public:

   JPEGOptionsDialog( const ImageOptions&, const JPEGImageOptions& );

   virtual ~JPEGOptionsDialog()
   {
   }

   ImageOptions      options;
   JPEGImageOptions  jpegOptions;

protected:

   VerticalSizer     Global_Sizer;

   GroupBox          JPEGCompression_GroupBox;
   VerticalSizer     Compression_Sizer;
      HorizontalSizer   CompressionRow1_Sizer;
         Label             Quality_Label;
         SpinBox           Quality_SpinBox;
         HorizontalSlider  Quality_Slider;
      HorizontalSizer   CompressionRow2_Sizer;
         CheckBox          OptimizedCoding_CheckBox;
      HorizontalSizer   CompressionRow3_Sizer;
         CheckBox          ArithmeticCoding_CheckBox;
      HorizontalSizer   CompressionRow4_Sizer;
         CheckBox          Progressive_CheckBox;
   GroupBox          EmbeddedData_GroupBox;
   HorizontalSizer   EmbeddedData_Sizer;
      HorizontalSizer   ICCProfile_Sizer;
         CheckBox          ICCProfile_CheckBox;
   HorizontalSizer   BottomSection_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void SpinBox_ValueUpdated( SpinBox& sender, int value );
   void Slider_ValueUpdated( Slider& sender, int value );

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEGOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF JPEGOptionsDialog.h - Released 2015/11/26 15:59:58 UTC
