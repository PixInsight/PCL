//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.07.0369
// ----------------------------------------------------------------------------
// TIFFOptionsDialog.h - Released 2019-01-21T12:06:31Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __TIFFOptionsDialog_h
#define __TIFFOptionsDialog_h

#include "TIFF.h"

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/CheckBox.h>
#include <pcl/TextBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class TIFFOptionsDialog : public Dialog
{
public:

   TIFFOptionsDialog( const pcl::ImageOptions&, const pcl::TIFFImageOptions& );

   virtual ~TIFFOptionsDialog()
   {
   }

   pcl::ImageOptions     options;
   pcl::TIFFImageOptions tiffOptions;

private:

   HorizontalSizer   Global_Sizer;

   VerticalSizer     LeftPanel_Sizer;
      GroupBox          SampleFormat_GroupBox;
      VerticalSizer     SampleFormat_Sizer;
         RadioButton       UInt8_RadioButton;
         RadioButton       UInt16_RadioButton;
         RadioButton       UInt32_RadioButton;
         RadioButton       Float_RadioButton;
         RadioButton       Double_RadioButton;
      GroupBox          Compression_GroupBox;
      VerticalSizer     Compression_Sizer;
         RadioButton       NoCompression_RadioButton;
         RadioButton       ZIP_RadioButton;
         RadioButton       LZW_RadioButton;
      GroupBox          EmbeddedData_GroupBox;
      VerticalSizer     EmbeddedData_Sizer;
         CheckBox          ICCProfile_CheckBox;
      GroupBox          Miscellaneous_GroupBox;
      VerticalSizer     Miscellaneous_Sizer;
         CheckBox          Planar_CheckBox;
         CheckBox          AssociatedAlpha_CheckBox;
         CheckBox          PremultipliedAlpha_CheckBox;

   VerticalSizer     RightPanel_Sizer;
      GroupBox          ImageDescription_GroupBox;
      HorizontalSizer   ImageDescription_Sizer;
         TextBox           ImageDescription_TextBox;
      GroupBox          ImageCopyright_GroupBox;
      HorizontalSizer   ImageCopyright_Sizer;
         TextBox           ImageCopyright_TextBox;
      GroupBox          SoftwareDescription_GroupBox;
      HorizontalSizer   SoftwareDescription_Sizer;
         TextBox           SoftwareDescription_TextBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __TIFFOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF TIFFOptionsDialog.h - Released 2019-01-21T12:06:31Z
