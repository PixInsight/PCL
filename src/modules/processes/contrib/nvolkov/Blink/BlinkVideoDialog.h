//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 01.02.01.0211
// ----------------------------------------------------------------------------
// BlinkVideoDialog.h - Released 2015/12/18 08:55:08 UTC
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

#ifndef __BlinkVideoDialog_h
#define __BlinkVideoDialog_h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/ExternalProcess.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Rectangle.h>
#include <pcl/Sizer.h>
#include <pcl/TextBox.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BlinkInterface;

class BlinkVideoDialog : public Dialog
{
public:

   BlinkVideoDialog( BlinkInterface* parent );

private:

   VerticalSizer     Global_Sizer;
      TextBox           StdOut_TextBox;
      Edit              StdIn_Edit;
      HorizontalSizer   PreferencesLine1_Sizer;
         Label             Program_Label;
         Edit              Program_Edit;
      HorizontalSizer   PreferencesLine2_Sizer;
         Label             Arguments_Label;
         Edit              Arguments_Edit;
      HorizontalSizer   PreferencesLine3_Sizer;
         Label             OutputDir_Label;
         Edit              OutputDir_Edit;
         ToolButton        OutputDir_SelectButton;
      HorizontalSizer   PreferencesLine4_Sizer;
         Label             FrameExtension_Label;
         Edit              FrameExtension_Edit;
         CheckBox          DeleteVideoFrames_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Load_PushButton;
         PushButton        Save_PushButton;
         PushButton        WriteLog_PushButton;
         PushButton        Run_PushButton;
         PushButton        Cancel_PushButton;

   BlinkInterface*   m_parent;       // the parent interface
   ExternalProcess   m_videoEncoder; // the external video encoder process
   int               m_frameCount;   // file counter in the output sequence
   StringList        m_framesDone;   // already generated video frames in a given format ?
   String            m_command;      // command executed
   IsoString         m_timestamp;    // time of execution

   void  DisableGUI( bool disable = true );
   void  EnableGUI( bool enable = true ) { DisableGUI( !enable ); }
   void  Init();
   void  CreateFrames(); // create sequence with format: "dir/Blink00001.jpg"
   void  DeleteFrames();
   void  ExecuteVideoEncoder();
   bool  TerminateVideoEncoder();
   void  WriteLogFile();
   void  AddLog( const String& );

   void  __Process_Started( ExternalProcess& );
   void  __Process_Finished( ExternalProcess&, int exitCode, bool exitOk );
   void  __Process_StandardOutputDataAvailable( ExternalProcess& );
   void  __Process_Error( ExternalProcess&, ExternalProcess::error_code error );
   void  __Button_Click( Button& sender, bool checked );
   void  __Edit_Completed( Edit& sender );
   void  __Dialog_Close( Control& sender, bool& allowClose );
   void  __Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BlinkVideoDialog_h

// ----------------------------------------------------------------------------
// EOF BlinkVideoDialog.h - Released 2015/12/18 08:55:08 UTC
