//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0437
// ----------------------------------------------------------------------------
// StatisticsInterface.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __StatisticsInterface_h
#define __StatisticsInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>
#include <pcl/ViewList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class StatisticsInterface : public ProcessInterface
{
public:

   StatisticsInterface();
   virtual ~StatisticsInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   const char** IconImageXPM() const override;

   InterfaceFeatures Features() const override;

   void TrackViewUpdated( bool active ) override;

   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;

   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;

   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageFocused( const View& ) override;
   void ImageDeleted( const View& ) override;

   bool WantsViewPropertyNotifications() const override;
   void ViewPropertyDeleted( const View& v, const IsoString& property ) override;

   void SaveSettings() const override;
   void LoadSettings() override;

private:

   bool       m_doCount = true;
   UI64Vector m_count;

   bool       m_doMean = true;
   DVector    m_mean;

   bool       m_doModulus = false;
   DVector    m_modulus;

   bool       m_doNorm = false;
   DVector    m_norm;

   bool       m_doSumOfSquares = false;
   DVector    m_sumOfSquares;

   bool       m_doMeanOfSquares = false;
   DVector    m_meanOfSquares;

   bool       m_doMedian = true;
   DVector    m_median;

   bool       m_doVariance = false;
   DVector    m_variance;

   bool       m_doStdDev = false;
   DVector    m_stdDev;

   bool       m_doAvgDev = true;
   DVector    m_avgDev;

   bool       m_doMAD = true;
   DVector    m_MAD;

   bool       m_doBWMV = false;
   DVector    m_BWMV;

   bool       m_doPBMV = false;
   DVector    m_PBMV;

   bool       m_doSn = false;
   DVector    m_Sn;

   bool       m_doQn = false;
   DVector    m_Qn;

   bool       m_doMinimum = true;
   DVector    m_minimum;

   bool       m_doMinimumPos = false;
   IMatrix    m_minimumPos;

   bool       m_doMaximum = true;
   DVector    m_maximum;

   bool       m_doMaximumPos = false;
   IMatrix    m_maximumPos;

   int        m_rangeBits = 0; // 0=normalized

   struct GUIData
   {
      GUIData( StatisticsInterface& );

      VerticalSizer  Global_Sizer;
         ViewList       AllViews_ViewList;
         HorizontalSizer   Format_Sizer;
            ComboBox          Range_ComboBox;
            CheckBox          Scientific_CheckBox;
            CheckBox          Normalized_CheckBox;
            CheckBox          Unclipped_CheckBox;
            ToolButton        ToText_ToolButton;
            ToolButton        Options_ToolButton;
         TreeBox        Data_TreeBox;
   };

   GUIData* GUI = nullptr;

   bool ViewPropertyRequired( const IsoString& property ) const;

   String ToText() const;

   void CalculateStatistics();
   void UpdateControls();

   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __Button_Click( Button& sender, bool checked );
   void __ComboBox_ItemSelected( ComboBox& sender, int itemIndex );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
   friend class StatisticsOptionsDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern StatisticsInterface* TheStatisticsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StatisticsInterface_h

// ----------------------------------------------------------------------------
// EOF StatisticsInterface.h - Released 2019-01-21T12:06:41Z
