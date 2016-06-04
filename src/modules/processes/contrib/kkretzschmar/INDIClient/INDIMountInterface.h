//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.12.0183
// ----------------------------------------------------------------------------
// INDIMountInterface.h - Released 2016/06/04 15:14:47 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#ifndef __INDIMountInterface_h
#define __INDIMountInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NetworkTransfer.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TextBox.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>

#include "INDIClient.h"
#include "INDIMountInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CoordinateSearchDialog : public Dialog
{
public:

   CoordinateSearchDialog();

   double TargetRA() const
   {
      return m_targetRA;
   }

   double TargetDec() const
   {
      return m_targetDec;
   }

   bool HasValidTargetCoordinates() const
   {
      return m_valid;
   }

   bool GoToTarget() const
   {
      return m_goto;
   }

private:

   VerticalSizer       Global_Sizer;
      HorizontalSizer      Search_Sizer;
         Label                ObjectName_Label;
         Edit                 ObjectName_Edit;
         PushButton           Search_Button;
      TextBox              SearchInfo_TextBox;
      HorizontalSizer      Buttons_Sizer;
         PushButton           Get_Button;
         PushButton           GoTo_Button;
         PushButton           Cancel_Button;

   double    m_targetRA;
   double    m_targetDec;
   bool      m_valid;
   bool      m_goto;
   bool      m_downloading;
   bool      m_abort;
   bool      m_firstTimeShown;
   IsoString m_downloadData;

   void e_Show( Control& sender );
   void e_GetFocus( Control& sender );
   void e_LoseFocus( Control& sender );
   bool e_Download( NetworkTransfer& sender, const void* buffer, fsize_type size );
   bool e_Progress( NetworkTransfer& sender,
                    fsize_type downloadTotal, fsize_type downloadCurrent,
                    fsize_type uploadTotal, fsize_type uploadCurrent );
   void e_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

class INDIMountInterfaceExecution;

class INDIMountInterface : public ProcessInterface
{
public:

   INDIMountInterface();
   virtual ~INDIMountInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;
   virtual InterfaceFeatures Features() const;
   virtual void ResetInstance();
   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );
   virtual ProcessImplementation* NewProcess() const;
   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;
   virtual bool ImportProcess( const ProcessImplementation& );

private:

   String                       m_device;
   INDIMountInterfaceExecution* m_execution;
   CoordinateSearchDialog*      m_searchDialog;

   struct GUIData
   {
      GUIData(INDIMountInterface& w);

      Timer UpdateDeviceList_Timer;
      Timer UpdateDeviceProperties_Timer;

      VerticalSizer        Global_Sizer;

      SectionBar        ServerParameters_SectionBar;
      Control           ServerParameters_Control;
      VerticalSizer     ServerParameters_Sizer;
         HorizontalSizer   MountDevice_Sizer;
            Label             MountDevice_Label;
            ComboBox          MountDevice_Combo;
         Control           MountProperties_Control;
         VerticalSizer     MountProperties_Sizer;
            HorizontalSizer   MountLST_Sizer;
               Label             LST_Label;
               Label             LST_Value_Label;
            HorizontalSizer   MountEQRA_Sizer;
               Label             RA_Label;
               Label             RA_Value_Label;
            HorizontalSizer   MountEQDec_Sizer;
               Label             Dec_Label;
               Label             Dec_Value_Label;
            HorizontalSizer   MountHZAZ_Sizer;
               Label             AZ_Label;
               Label             AZ_Value_Label;
            HorizontalSizer   MountHZALT_Sizer;
               Label             ALT_Label;
               Label             ALT_Value_Label;

      SectionBar        MountMove_SectionBar;
      Control           MountMove_Control;
      HorizontalSizer   MountMove_Sizer;
         Control           SlewLeft_Control;
         VerticalSizer     SlewLeft_Sizer;
            HorizontalSizer   MountMoveTopRow_Sizer;
               ToolButton        MountMoveTopLeft_Button;
               ToolButton        MountMoveTop_Button;
               ToolButton        MountMoveTopRight_Button;
            HorizontalSizer   MountMoveMiddleRow_Sizer;
               ToolButton        MountMoveLeft_Button;
               ToolButton        MountMoveStop_Button;
               ToolButton        MountMoveRight_Button;
            HorizontalSizer   MountMoveBottomRow_Sizer;
               ToolButton        MountMoveBottomLeft_Button;
               ToolButton        MountMoveBottom_Button;
               ToolButton        MountMoveBottomRight_Button;
         VerticalSizer     SlewRight_Sizer;
            Label             MountMoveSpeed_Label;
            ComboBox          MountMoveSpeed_ComboBox;

      SectionBar        MountGoTo_SectionBar;
      Control           MountGoTo_Control;
      VerticalSizer     MountGoTo_Sizer;
         HorizontalSizer   MountTargetRA_Sizer;
            Label             TargetRA_Label;
            SpinBox           TargetRA_H_SpinBox;
            SpinBox           TargetRA_M_SpinBox;
            NumericEdit       TargetRA_S_NumericEdit;
         HorizontalSizer   MountTargetDec_Sizer;
            Label             TargetDec_Label;
            SpinBox           TargetDec_H_SpinBox;
            SpinBox           TargetDec_M_SpinBox;
            NumericEdit       TargetDec_S_NumericEdit;
            CheckBox          MountTargetDECIsSouth_CheckBox;
         HorizontalSizer   MountSearch_Sizer;
            PushButton        MountSearch_Button;
         HorizontalSizer   MountGoToStart_Sizer;
            PushButton        MountGoToStart_Button;
            PushButton        MountSync_Button;
            PushButton        MountPark_Button;
         HorizontalSizer   MountGoToCancel_Sizer;
            PushButton        MountGoToCancel_Button;
         Label             MountGoToInfo_Label;
   };

   GUIData* GUI;

   void UpdateControls();

   void e_Timer( Timer& sender );
   void e_Edit( Edit& sender );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_Press( Button& sender );
   void e_Release( Button& sender );

   friend class INDIMountInterfaceExecution;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDIMountInterface* TheINDIMountInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIMountInterface_h

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.h - Released 2016/06/04 15:14:47 UTC
