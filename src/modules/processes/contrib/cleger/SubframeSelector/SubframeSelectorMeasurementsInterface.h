//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.02.0025
// ----------------------------------------------------------------------------
// SubframeSelectorMeasurementsInterface.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#ifndef e_SubframeSelectorMeasurementsInterface_h
#define e_SubframeSelectorMeasurementsInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/TreeBox.h>

#include "GraphWebView.h"
#include "SubframeSelectorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorMeasurementsInterface : public ProcessInterface
{
public:

   SubframeSelectorMeasurementsInterface( SubframeSelectorInstance& );
   virtual ~SubframeSelectorMeasurementsInterface();

   IsoString Id() const override;

   MetaProcess* Process() const override;

   const char** IconImageXPM() const override;
   InterfaceFeatures Features() const override;

   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;

   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   using ProcessInterface::Launch;

   void Cleanup();

private:

   SubframeSelectorInstance& m_instance;

   struct GUIData
   {
      GUIData( SubframeSelectorMeasurementsInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        MeasurementTable_SectionBar;
      Control           MeasurementTable_Control;
      VerticalSizer     MeasurementTable_Sizer;
         HorizontalSizer   MeasurementsTable_Top1_Sizer;
            ComboBox          MeasurementsTable_SortingProperty_Control;
            ComboBox          MeasurementsTable_SortingMode_Control;
            PushButton        MeasurementsTable_ToggleApproved_PushButton;
            PushButton        MeasurementsTable_ToggleLocked_PushButton;
            PushButton        MeasurementsTable_Invert_PushButton;
         HorizontalSizer   MeasurementsTable_Top2_Sizer;
            Label             MeasurementsTable_Quantities_Label;
            PushButton        MeasurementsTable_Remove_PushButton;
            PushButton        MeasurementsTable_Clear_PushButton;
            PushButton        MeasurementsTable_ExportCSV_PushButton;
         TreeBox           MeasurementTable_TreeBox;

      SectionBar        MeasurementGraph_SectionBar;
      Control           MeasurementGraph_Control;
      VerticalSizer     MeasurementGraph_Sizer;
         HorizontalSizer   MeasurementGraph_Top_Sizer;
            ComboBox          MeasurementGraph_GraphProperty_Control;
            PushButton        MeasurementGraph_ExportPDF_PushButton;
         HorizontalSizer   MeasurementGraph_Bottom_Sizer;
            GraphWebView      MeasurementGraph_Graph;
            GraphWebView      MeasurementDistribution_Graph;
   };

   GUIData* GUI = nullptr;

   MeasureItem* GetMeasurementItem( size_type );
   TreeBox::Node* GetMeasurementNode( MeasureItem* );

   void UpdateControls();
   void UpdateMeasurementQuantity();
   void UpdateMeasurementImageItem( size_type, MeasureItem* );
   void UpdateMeasurementImagesList();
   void UpdateMeasurementGraph();

   void SetMeasurements( const Array<MeasureItem>& );

   void ExportCSV() const;
   void ExportPDF() const;

   /*
    * Event Handlers
    */
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_ButtonClick( Button& sender, bool checked );
   void e_GraphApprove( GraphWebView &sender, int& index );
   void e_GraphUnlock( GraphWebView &sender, int& index );
   void e_ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
   friend class SubframeSelectorInstance;
   friend class SubframeSelectorInterface;
   friend class SubframeSelectorExpressionsInterface;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorMeasurementsInterface* TheSubframeSelectorMeasurementsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // e_SubframeSelectorMeasurementsInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasurementsInterface.h - Released 2019-01-21T12:06:42Z
