//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 01.00.09.0171
// ----------------------------------------------------------------------------
// XISFOptionsDialog.h - Released 2018-11-01T11:07:09Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
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

#ifndef XISFOptionsDialog_h
#define XISFOptionsDialog_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
//#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFOptionsDialogBase : public Dialog
{
public:

   XISFOptions options;

   XISFOptionsDialogBase( const XISFOptions&, const char* labelForWidth = nullptr );

protected:

   int m_labelWidth;

   GroupBox          DataCompression_GroupBox;
   VerticalSizer     DataCompression_Sizer;
      HorizontalSizer   CompressionCodec_Sizer;
         Label             CompressionCodec_Label;
         ComboBox          CompressionCodec_ComboBox;
      HorizontalSizer   CompressionLevel_Sizer;
         Label             CompressionLevel_Label;
         SpinBox           CompressionLevel_SpinBox;
      HorizontalSizer   CompressionShuffle_Sizer;
         CheckBox          CompressionShuffle_CheckBox;

   GroupBox          Security_GroupBox;
   VerticalSizer     Security_Sizer;
      HorizontalSizer   Checksums_Sizer;
         Label             Checksums_Label;
         ComboBox          Checksums_ComboBox;

   HorizontalSizer   BottomSection_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   int CompressionCodecToComboBoxItem( XISF::block_compression );
   XISF::block_compression ComboBoxItemToCompressionCodec( int );

   int ChecksumAlgorithmToComboBoxItem( XISF::block_checksum );
   XISF::block_checksum ComboBoxItemToChecksumAlgorithm( int );

   void GetBaseParameters();

   void Base_Button_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

class XISFOptionsDialog : public XISFOptionsDialogBase
{
public:

   ImageOptions imageOptions;
   IsoString    outputHints;

   XISFOptionsDialog( const ImageOptions&, const XISFOptions&, const IsoString& = IsoString() );

private:

   VerticalSizer     Global_Sizer;

   GroupBox          SampleFormat_GroupBox;
   VerticalSizer     SampleFormat_Sizer;
      HorizontalSizer   UInt8_Sizer;
         RadioButton       UInt8_RadioButton;
      HorizontalSizer   UInt16_Sizer;
         RadioButton       UInt16_RadioButton;
      HorizontalSizer   UInt32_Sizer;
         RadioButton       UInt32_RadioButton;
      HorizontalSizer   Float_Sizer;
         RadioButton       Float_RadioButton;
      HorizontalSizer   Double_Sizer;
         RadioButton       Double_RadioButton;

   /*
    * ### TODO: Core 1.8.x: Support hints in format queries.
    *
   GroupBox          OutputHints_GroupBox;
   HorizontalSizer   OutputHints_Sizer;
      Label             OutputHints_Label;
      Edit              OutputHints_Edit;
   */

   GroupBox          EmbeddedData_GroupBox;
   VerticalSizer     EmbeddedData_Sizer;
      HorizontalSizer      Properties_Sizer;
         CheckBox             Properties_CheckBox;
      HorizontalSizer      FITSKeywords_Sizer;
         CheckBox             FITSKeywords_CheckBox;
      HorizontalSizer      ICCProfile_Sizer;
         CheckBox             ICCProfile_CheckBox;
      HorizontalSizer      DisplayFunction_Sizer;
         CheckBox             DisplayFunction_CheckBox;
      HorizontalSizer      RGBWorkingSpace_Sizer;
         CheckBox             RGBWorkingSpace_CheckBox;
      HorizontalSizer      Thumbnail_Sizer;
         CheckBox             Thumbnail_CheckBox;
      HorizontalSizer      PreviewRects_Sizer;
         CheckBox             PreviewRects_CheckBox;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // XISFOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF XISFOptionsDialog.h - Released 2018-11-01T11:07:09Z
