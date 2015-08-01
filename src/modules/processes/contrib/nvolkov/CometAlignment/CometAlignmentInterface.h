//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 01.02.06.0089
// ----------------------------------------------------------------------------
// CometAlignmentInterface.h - Released 2015/07/31 11:49:49 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2015 Nikolay Volkov
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

#ifndef __CometAlignmentInterface_h
#define __CometAlignmentInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/TreeBox.h>
#include <pcl/CheckBox.h>
#include <pcl/GroupBox.h>
#include <pcl/ComboBox.h>
#include <pcl/RadioButton.h>

#include "CometAlignmentInstance.h"

namespace pcl
{

  // ----------------------------------------------------------------------------

  class CometAlignmentInterface : public ProcessInterface
  {
  public:

    CometAlignmentInterface ();
    virtual ~CometAlignmentInterface ();

    virtual IsoString Id () const;
    virtual MetaProcess* Process () const;
    virtual const char** IconImageXPM() const;

    virtual InterfaceFeatures Features () const;
    virtual void ApplyInstance () const;

    virtual void ResetInstance ();

    virtual bool Launch (const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/);

    virtual ProcessImplementation* NewProcess () const;

    virtual bool ValidateProcess (const ProcessImplementation&, pcl::String& whyNot) const;
    virtual bool RequiresInstanceValidation () const;

    virtual bool ImportProcess (const ProcessImplementation&);

    virtual bool IsDynamicInterface () const;
    virtual void DynamicMousePress (View&, const DPoint&, int button, unsigned buttons, unsigned modifiers);
    virtual bool RequiresDynamicUpdate (const View&, const DRect&) const;
    virtual void DynamicPaint (const View&, Graphics&, const DRect&) const;

    virtual void
    SaveSettings () const;

    // -------------------------------------------------------------------------

  private:

    CometAlignmentInstance m_instance;

    // -------------------------------------------------------------------------

    struct GUIData
    {
      GUIData (CometAlignmentInterface&);

      VerticalSizer Global_Sizer;

	  TreeBox Monitor_TreeBox;
      SectionBar TargetImages_SectionBar;
      Control TargetImages_Control;
      HorizontalSizer TargetImages_Sizer;
        TreeBox TargetImages_TreeBox;
        VerticalSizer TargetButtons_Sizer;
            PushButton AddFiles_PushButton;
            PushButton AddDrizzleFiles_PushButton;
            PushButton ClearDrizzleFiles_PushButton;
            PushButton SetReference_PushButton;
            PushButton SelectAll_PushButton;
            PushButton InvertSelection_PushButton;
            PushButton ToggleSelected_PushButton;
            PushButton RemoveSelected_PushButton;
            PushButton Clear_PushButton;
            CheckBox   FullPaths_CheckBox;

      SectionBar        FormatHints_SectionBar;
      Control           FormatHints_Control;
      VerticalSizer     FormatHints_Sizer;
         HorizontalSizer   InputHints_Sizer;
            Label             InputHints_Label;
            Edit              InputHints_Edit;
         HorizontalSizer   OutputHints_Sizer;
            Label             OutputHints_Label;
            Edit              OutputHints_Edit;

      SectionBar        Output_SectionBar;
      Control           Output_Control;
         VerticalSizer     Output_Sizer;
            HorizontalSizer   OutputDir_Sizer;
               Label             OutputDir_Label;
               Edit              OutputDir_Edit;
               ToolButton        OutputDir_SelectButton;
            HorizontalSizer   OutputChunks_Sizer;
               Label             OutputExtension_Label;
               Edit              OutputExtension_Edit;
               CheckBox          Overwrite_CheckBox;
               Label             Prefix_Label;
               Edit              Prefix_Edit;
               Label             Postfix_Label;
               Edit              Postfix_Edit;

    SectionBar		Parameter_SectionBar;
    Control			Parameter_Control;
	VerticalSizer		Parameter_Sizer;
		HorizontalSizer		File0_Sizer;
			NumericEdit			x0_NumericEdit;
			NumericEdit			y0_NumericEdit;
			PushButton			File0_PushButton;
			Label				File0_Label;
		HorizontalSizer		File1_Sizer;
			NumericEdit			x1_NumericEdit;
			NumericEdit			y1_NumericEdit;
			PushButton			File1_PushButton;
			Label				File1_Label;
		HorizontalSizer		Delta_Sizer;
			NumericEdit			xDelta_NumericEdit;
			NumericEdit			yDelta_NumericEdit;

	SectionBar		Subtract_SectionBar;
	Control			Subtract_Control;
	VerticalSizer	Subtract_Sizer;
		HorizontalSizer	SubtractFile_Sizer;
			Label			SubtractFile_Label;
			Edit			SubtractFile_Edit;
			ToolButton		SubtractFile_SelectButton;
			ToolButton		SubtractFile_ClearButton;
		HorizontalSizer	SubtractImgOption_Sizer;
			GroupBox        SubtractOrigin_GroupBox;
			VerticalSizer	SubtractOrigin_Sizer;
				RadioButton		SubtractII_RadioButton;
				RadioButton		SubtractDI_RadioButton;
			GroupBox        SubtractImgType_GroupBox;
			VerticalSizer	SubtractImgType_Sizer;
				RadioButton		SubtractStars_RadioButton;
				RadioButton		SubtractComet_RadioButton;
			GroupBox		DrzSave_GroupBox;
			VerticalSizer	DrzSave_Sizer;
				CheckBox		DrzSaveSA_CheckBox;
				CheckBox	    DrzSaveCA_CheckBox;
		HorizontalSizer	SubtractChekers_Sizer;
			CheckBox		LinearFit_CheckBox;
			CheckBox		Normalize_CheckBox;
			NumericControl	RejectLow_NumericControl;
			NumericControl	RejectHigh_NumericControl;

	SectionBar		Interpolation_SectionBar;
	Control			Interpolation_Control;
	VerticalSizer	Interpolation_Sizer;
		HorizontalSizer	PixelInterpolation_Sizer;
			Label			PixelInterpolation_Label;
			ComboBox		PixelInterpolation_ComboBox;
			NumericControl	ClampingThreshold_NumericControl;
    };

    GUIData* GUI;

    // Workbench
    size_t m_length;

    String m_path0; // File path of first image
    double m_date0; // Julian Date of first image
    DPoint m_pos0; // comet position(X,Y) in first image

    String m_path1; // File path of first image
    double m_date1; // Julian Date of last image
    DPoint m_pos1; // comet position(X,Y) in last image

    double m_DateDelta; // days passed from first to last image
    DPoint m_PosDelta; // how many X,Y pixels comet passed from first to last image

	bool drizzle; //true == is .drz file present for every image

    // Main routines
    void SelectDir (); // Select output directory
    void SelectSubtractFile (); // Select image for subtract from targets
    void GetPoint (DPoint& pos, const double jDate); // Calculate new x,y coordinate = FirstImage + Delta
    void SetFirst (const DPoint pos); // Set x,y, in First image
    void SetLast (const DPoint pos); // Set x,y, in Last image

    // CometAlignmentInterface Updates
    void UpdateControls ();
    void UpdateTargetImageItem (size_type);
    void UpdateTargetImagesList ();
    void UpdateImageSelectionButtons ();
    void UpdateSubtractSection ();
    void SetReference (const int index); // Settings for ExecuteGlobal: Move all images arround the reference image. Don't move the reference image.

    void __TargetImages_CurrentNodeUpdated (TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent);
    void __TargetImages_NodeActivated (TreeBox& sender, TreeBox::Node& node, int col);
    void __TargetImages_NodeSelectionUpdated (TreeBox& sender);
    void __TargetImages_BottonClick (Button& sender, bool checked);
    void __ToggleSection (SectionBar& sender, Control& section, bool start);
    void __MouseDoubleClick (Control& sender, const Point& pos, unsigned buttons, unsigned modifiers);
    void __EditCompleted (Edit& sender);
    void __Button_Click (Button& sender, bool checked);
    void __RealValueUpdated (NumericEdit& sender, double value);
    void __ItemSelected (ComboBox& sender, int itemIndex);


    String DrizzleTargetName( const String& filePath );

    friend struct GUIData;
	friend class  CometAlignmentInstance;
  };

  // ----------------------------------------------------------------------------

  PCL_BEGIN_LOCAL
          extern CometAlignmentInterface* TheCometAlignmentInterface;
  PCL_END_LOCAL

  // ----------------------------------------------------------------------------

} // pcl

#endif   // __CometAlignmentInterface_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentInterface.h - Released 2015/07/31 11:49:49 UTC
