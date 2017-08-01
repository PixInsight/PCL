//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 01.02.05.0199
// ----------------------------------------------------------------------------
// CosmeticCorrectionInterface.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2017 Nikolay Volkov
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#ifndef __CosmeticCorrectionInterface_h
#define __CosmeticCorrectionInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/NumericControl.h>
#include <pcl/TreeBox.h>
#include <pcl/CheckBox.h>
#include <pcl/GroupBox.h>

#include "CosmeticCorrectionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CosmeticCorrectionInterface : public ProcessInterface
{
public:

   CosmeticCorrectionInterface();
   virtual ~CosmeticCorrectionInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   //virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;

   virtual void RealTimePreviewUpdated( bool active );
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool WantsReadoutNotifications() const;
   virtual void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A );

   virtual bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const;
   virtual bool GenerateRealTimePreview( UInt16Image&, const View&, int zoomLevel, String& info ) const;
   virtual bool WantsRealTimePreviewNotifications() const;
   virtual void RealTimePreviewOwnerChanged( ProcessInterface& );

   virtual void SaveSettings() const;

private:

   CosmeticCorrectionInstance instance;

   struct GUIData
   {
      GUIData( CosmeticCorrectionInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        TargetImages_SectionBar;
      Control           TargetImages_Control;
      HorizontalSizer   TargetImages_Sizer;
         TreeBox           TargetImages_TreeBox;
         VerticalSizer     TargetButtons_Sizer;
            PushButton        AddFiles_PushButton;
            PushButton        SelectAll_PushButton;
            PushButton        InvertSelection_PushButton;
            PushButton        ToggleSelected_PushButton;
            PushButton        RemoveSelected_PushButton;
            PushButton        Clear_PushButton;
            CheckBox          FullPaths_CheckBox;

      SectionBar        Output_SectionBar;
      Control           Output_Control;
      VerticalSizer     Output_Sizer;
         HorizontalSizer   OutputDir_Sizer;
            Label             OutputDir_Label;
            Edit              OutputDir_Edit;
            ToolButton        OutputDir_SelectButton;
            ToolButton        OutputDir_ClearButton;
         HorizontalSizer   OutputChunks_Sizer;
            Label             Prefix_Label;
            Edit              Prefix_Edit;
            Label             Postfix_Label;
            Edit              Postfix_Edit;
         HorizontalSizer   CFA_Sizer;
            CheckBox          CFA_CheckBox;
         HorizontalSizer   Overwrite_Sizer;
            CheckBox          Overwrite_CheckBox;
         NumericControl    Amount_NumericControl;

      // detect via MasterDark section
      SectionBar        UseMasterDark_SectionBar;
      Control           UseMasterDark_Control;
      VerticalSizer     UseMasterDark_Sizer;

         GroupBox          MasterDark_GroupBox;
         VerticalSizer     MasterDark_Sizer;
            HorizontalSizer   MasterDarkPath_Sizer;
               Edit              MasterDarkPath_Edit;
               ToolButton        MasterDarkPath_SelectButton;
               ToolButton         MasterDarkPath_ClearButton;

         GroupBox          Hot_GroupBox;
         VerticalSizer     Hot_Sizer;
            CheckBox          HotDark_CheckBox;
            NumericControl    HotDarkLevel_NumericControl;
            NumericControl    HotDarkSigma_NumericControl;
            NumericControl    HotDarkQty_NumericControl;
            HorizontalSizer   HotRealQty_Sizer;
               Label             HotRealQty_Label;
               Label               HotRealQty_Value;

         GroupBox          Cold_GroupBox;
         VerticalSizer     Cold_Sizer;
            CheckBox          ColdDark_CheckBox;
            NumericControl    ColdDarkLevel_NumericControl;
            NumericControl    ColdDarkSigma_NumericControl;
            NumericControl    ColdDarkQty_NumericControl;
            HorizontalSizer   ColdRealQty_Sizer;
               Label               ColdRealQty_Label;
               Label               ColdRealQty_Value;

      // auto detect section
      SectionBar        UseAutoDetect_SectionBar;
      Control           UseAutoDetect_Control;
      VerticalSizer     UseAutoDetect_Sizer;
         CheckBox          DetectHot_CheckBox;
            NumericControl      DetectHot_NumericControl;
         CheckBox          DetectCold_CheckBox;
            NumericControl      DetectCold_NumericControl;

      // via defective list section
      SectionBar        UseDefectList_SectionBar;
      Control           UseDefectList_Control;
      VerticalSizer     UseDefectList_Sizer;

         HorizontalSizer   DefectListTreeBox_Sizer;
            TreeBox           DefectList_TreeBox;
            VerticalSizer     DefectListButtons_Sizer;
               PushButton         LoadList_PushButton;
               PushButton         SaveList_PushButton;
               PushButton         SelectAllDefect_PushButton;
               PushButton         InvertSelectionDefect_PushButton;
               PushButton         ToggleSelectedDefect_PushButton;
               PushButton         RemoveSelectedDefect_PushButton;
               PushButton         ClearDefect_PushButton;

         VerticalSizer      DefineDefect_Sizer;
            HorizontalSizer   DefineDefect_Sizer1;
               RadioButton         Row_RadioButton;
               RadioButton         Col_RadioButton;
               NumericEdit         DefectPosition;
               PushButton          AddDefect_PushButton;

            HorizontalSizer   DefineDefect_Sizer2;
               CheckBox            SelectRange_CheckBox;
               NumericEdit         DefectStart;
               NumericEdit         DefectEnd;

      // RTP section
      SectionBar        RTP_SectionBar;
      Control           RTP_Control;
      VerticalSizer     RTP_Sizer;
         HorizontalSizer      RTPLine1_Sizer;
            Label                  TRPTitleHot_Label;
            Label                  TRPTitleCold_Label;
         HorizontalSizer      RTPLine2_Sizer;
            NumericEdit            RTPDarkQtyHot_Edit;
            NumericEdit            RTPDarkQtyCold_Edit;
         HorizontalSizer      RTPLine3_Sizer;
            NumericEdit            RTPAutoQtyHot_Edit;
            NumericEdit            RTPAutoQtyCold_Edit;
            CheckBox               RTPShowMap_CheckBox;
            PushButton             RTPSnapshot_PushButton;

   };

   GUIData* GUI;

   // Workbench

   DarkImg*         m_md;                              // masterDark image
   int              m_channels;                        // qty channels in MasterDark image
   Array<Histogram> m_H;                               // histograms, native PI's resolution
   double           m_Mean;                            // average mean of all chanels
   double           m_StdDev;                          // average StdDev of all chanels
   int              m_MinSlider;                       // Slider range <= minimum
   int              m_MaxSlider;                       // Slider range >= maximum

   // Main routines
   void   SelectDir();                                 // Select output directory
   void   LoadMasterDark( const String& filePath );
   void   SelectMasterFrameDialog();

   void   HotUpdateGUI( const size_t );
   void   HotLevelUpdated( const float, const bool );  // calculate & set Qty from Level
   void   HotQtyUpdated( const size_t );               // calculate & set Level from Qty
   void   HotSigmaUpdated( const float );              // calculate Level and QTY

   void   ColdUpdateGUI( const size_t );
   void   ColdLevelUpdated( const float, const bool);  // calculate & set Qty from Level
   void   ColdQtyUpdated( const size_t );              // calculate & set Level from Qty
   void   ColdSigmaUpdated( const float );             // calculate Level and QTY

   void   AddDefect( const bool isRow, const int position, const bool setRange, const int start, const int end );

   // CosmeticCorrectionInterface Updates
   void   UpdateControls();
   void   UpdateTargetImageItem( size_type );
   void   UpdateTargetImagesList();
   void   UpdateImageSelectionButtons();
   void   ClearMasterDark();
   void   UpdateMasterDarkControls();
   void   UpdateAutoDetectControls();
   void   UpdateDefectListSelectionButtons();
   void   UpdateDefectListItem( size_type );
   void   UpdateDefectListControls();

   // RTP
          void RTPApplyBadMap( UInt16Image& target, const UInt8Image& map, const UInt16Image& value ) const;
   inline void RTPGetStatistics( Array<double>& avgDev, const View& view )const;
   inline void RTPGetImageRectangle( UInt16Image& wrk, const View& view ) const;
   inline void RTPGetDarkRectangle( UInt16Image& mdRect ) const;

   inline void RTPGetAvr( UInt16Image& avr, const UInt16Image& wrk ) const;
   inline void RTPGetMed( UInt16Image& med, const UInt16Image& wrk ) const;
   inline void RTPGetBkg( UInt16Image& bkg, const UInt16Image& wrk ) const;

   inline int RTPGetHotAutoMap( UInt8Image& mapAutoHot, float& hotAutoValue, const UInt16Image& med, const UInt16Image& bkg, const UInt16Image& avr, const UInt16Image& wrk, const Array<double>& avgDev) const;
   inline int RTPGetColdAutoMap( UInt8Image& mapAutoCold, float& coldAutoValue, const UInt16Image& med, const UInt16Image& bkg,   const UInt16Image& wrk, const Array<double>& avgDev) const;
   inline int RTPGetHotDarkMap( UInt8Image& mapDarkHot, float& hotDarkValue, const UInt16Image& mdRect ) const;
   inline int RTPGetColdDarkMap( UInt8Image& mapDarkCold, float& coldDarkValue, const UInt16Image& mdRect ) const;
   inline void RTPGetListDefectMap( UInt8Image& mapListDef, const View& view ) const;
   inline void RTPMakeSnapshot( const UInt16Image& img, const View& view ) const;
          void RTPUpdate();

   void   __TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void   __NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void   __NodeUpdated( TreeBox& sender, TreeBox::Node& node, int col );
   void   __NodeSelectionUpdated( TreeBox& sender );
   void   __TargetImages_Click( Button& sender, bool checked );
   void   __ToggleSection( SectionBar& sender, Control& section, bool start );
   void   __MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void   __EditCompleted( Edit& sender );
   void   __Button_Click( Button& sender, bool checked );
   void   __RealValueUpdated( NumericEdit& sender, double value );
   void   __CheckSection( SectionBar& sender, bool checked );
   void   __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void   __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CosmeticCorrectionInterface* TheCosmeticCorrectionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CosmeticCorrectionInterface_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionInterface.h - Released 2017-08-01T14:26:58Z
