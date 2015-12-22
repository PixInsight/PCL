//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 01.02.01.0211
// ----------------------------------------------------------------------------
// BlinkStatisticsDialog.h - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#ifndef __BlinkStatisticsDialog_h
#define __BlinkStatisticsDialog_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Rectangle.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/StringList.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BlinkInterface;

class BlinkStatisticsDialog : public Dialog
{
public:

   BlinkStatisticsDialog( BlinkInterface* parent );

private:

   VerticalSizer     Global_Sizer;

   ComboBox          File_ComboBox;
   TreeBox           Keyword_TreeBox;

   HorizontalSizer   StatPreferencesLine1_Sizer;
      RadioButton       Range0_RadioButton;
      RadioButton       Range1_RadioButton;
      Label             Digits_Label;
      SpinBox           Digits0_SpinBox;
      SpinBox           Digits1_SpinBox;
      CheckBox          SortChannels_CheckBox;
      CheckBox          StatCropMode_CheckBox;
      CheckBox          WriteStatsFile_CheckBox;

   HorizontalSizer   StatPreferencesLine2_Sizer;
      Label             OutputDir_Label;
      Edit              OutputDir_Edit;
      ToolButton        OutputDir_SelectButton;

   HorizontalSizer   Buttons_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   BlinkInterface*   m_parent;    // parent interface

   Rect              m_rect;      // CurrentView Rectangle
   Array<StringList> m_stat;      // All images statistics incuding header in String format
   String            m_outputDir; // folder for Statistics file

   void        Init();
   StringList  FindKeywords( int fileIndex );
   void        ConvertStatisticsToText();
   void        PrepareData();
   void        ExecuteStatistics();
   void        SelectOutputDir();

   void        __RangeMode_Click( Button& sender, bool checked );
   void        __TreeBox_ItemSelected( ComboBox& sender, int itemIndex );
   void        __Button_Click( Button& sender, bool checked );
   void        __Control_DoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void        __Edit_Completed( Edit& sender );
   void        __Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BlinkStatisticsDialog_h

// ----------------------------------------------------------------------------
// EOF BlinkStatisticsDialog.h - Released 2015/12/18 08:55:08 UTC
