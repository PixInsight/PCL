// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard XISF File Format Module Version 01.00.00.0023
// ****************************************************************************
// XISFPreferencesDialog.h - Released 2014/11/30 10:38:10 UTC
// ****************************************************************************
// This file is part of the standard XISF PixInsight module.
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

#ifndef __XISFPreferencesDialog_h
#define __XISFPreferencesDialog_h

#include "XISFFormat.h"

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFPreferencesDialog: public Dialog
{
public:

   XISFFormat::EmbeddingOverrides overrides;
   XISFOptions                    options;

   XISFPreferencesDialog( const XISFFormat::EmbeddingOverrides&, const XISFOptions& );
   virtual ~XISFPreferencesDialog() = default;

private:

   VerticalSizer     Global_Sizer;

   GroupBox          FITSCompatibility_GroupBox;
   VerticalSizer     FITSCompatibility_Sizer;
      HorizontalSizer   StoreFITSKeywords_Sizer;
         CheckBox          StoreFITSKeywords_CheckBox;
      HorizontalSizer   IgnoreFITSKeywords_Sizer;
         CheckBox          IgnoreFITSKeywords_CheckBox;
      HorizontalSizer   ImportFITSKeywords_Sizer;
         CheckBox          ImportFITSKeywords_CheckBox;

   GroupBox          DataCompression_GroupBox;
   VerticalSizer     DataCompression_Sizer;
      HorizontalSizer   Compression_Sizer;
         CheckBox          Compression_CheckBox;
      HorizontalSizer   CompressionLevel_Sizer;
         Label             CompressionLevel_Label;
         SpinBox           CompressionLevel_SpinBox;

   GroupBox          DataAlignment_GroupBox;
   VerticalSizer     DataAlignment_Sizer;
      HorizontalSizer   AlignmentSize_Sizer;
         Label             AlignmentSize_Label;
         SpinBox           AlignmentSize_SpinBox;
      HorizontalSizer   MaxInlineSize_Sizer;
         Label             MaxInlineSize_Label;
         SpinBox           MaxInlineSize_SpinBox;

   GroupBox          EmbeddedData_GroupBox;
   VerticalSizer     EmbeddedData_Sizer;
      HorizontalSizer      Properties_Sizer;
         CheckBox             Properties_CheckBox;
      HorizontalSizer      ICCProfile_Sizer;
         CheckBox             ICCProfile_CheckBox;
      HorizontalSizer      Metadata_Sizer;
         CheckBox             Metadata_CheckBox;
      HorizontalSizer      Thumbnail_Sizer;
         CheckBox             Thumbnail_CheckBox;

   HorizontalSizer   BottomSection_Sizer;
      PushButton        Reset_PushButton;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void __Button_Click( Button& sender, bool checked );
   void __Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __XISFPreferencesDialog_h

// ****************************************************************************
// EOF XISFPreferencesDialog.h - Released 2014/11/30 10:38:10 UTC
