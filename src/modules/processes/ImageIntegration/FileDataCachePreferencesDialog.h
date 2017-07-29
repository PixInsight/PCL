//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.15.00.0398
// ----------------------------------------------------------------------------
// FileDataCachePreferencesDialog.h - Released 2017-05-05T08:37:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __FileDataCachePreferencesDialog_h
#define __FileDataCachePreferencesDialog_h

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/CheckBox.h>
#include <pcl/SpinBox.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FileDataCache;

class FileDataCachePreferencesDialog : public Dialog
{
public:

   FileDataCachePreferencesDialog( FileDataCache* cache );

private:

   FileDataCache* cache;

   bool cacheEnabled;
   int cacheDuration;

   VerticalSizer  Global_Sizer;
      HorizontalSizer   PersistentCache_Sizer;
         CheckBox          PersistentCache_CheckBox;
      HorizontalSizer   CacheDuration_Sizer;
         Label             CacheDuration_Label;
         SpinBox           CacheDuration_SpinBox;
      HorizontalSizer   ClearCache_Sizer;
         PushButton        ClearCache_PushButton;
      HorizontalSizer   PurgeCache_Sizer;
         PushButton        PurgeCache_PushButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void Update();

   void SpinBox_ValueUpdated( SpinBox& sender, int value );
   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FileDataCachePreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF FileDataCachePreferencesDialog.h - Released 2017-05-05T08:37:32Z
