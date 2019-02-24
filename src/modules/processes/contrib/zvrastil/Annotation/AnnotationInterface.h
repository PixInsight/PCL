//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 01.00.00.0239
// ----------------------------------------------------------------------------
// AnnotationInterface.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2018 Zbynek Vrastil
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#ifndef __AnnotationInterface_h
#define __AnnotationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/FontComboBox.h>
#include <pcl/ColorComboBox.h>
#include <pcl/CheckBox.h>
#include <pcl/SpinBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>

#include "AnnotationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// AnnotationInterface
// ----------------------------------------------------------------------------

namespace DraggingType
{
    enum dragging_type {
      None,
      Text,
      Leader,
      Both
    };
}

class AnnotationInterface : public ProcessInterface
{
public:

   AnnotationInterface();
   virtual ~AnnotationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual bool IsDynamicInterface() const;
   virtual void ExitDynamicMode();

   virtual void Execute();
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );
   virtual void DynamicMouseMove( View&, const DPoint&, unsigned buttons, unsigned modifiers );
   virtual void DynamicMouseRelease( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );

   virtual bool RequiresDynamicUpdate( const View&, const DRect& ) const;
   virtual void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const;

   virtual IsoString SettingsKey() const;
   virtual void LoadSettings();
   virtual void SaveSettings() const;

private:



   AnnotationInstance          instance;          // the current annotation instance

   View*                       view;              // dynamic target
   bool                        annotationPlaced;  // flag whether annotation is placed in active view
   bool                        leaderPlaced;      // flag whether leader is placed in active view
   Rect                        textRect;          // actual position of the annotation text in the image
   Rect                        leaderRect;        // actual position of the annotation leader in the image
   Rect                        totalRect;         // union of text and leader rect
   Rect                        invalidateRect;    // rectangle to invalidate in next UpdateView call
   DraggingType::dragging_type dragging;          // actual dragging mode
   int                         lastX, lastY;      // last position of the mouse while dragging
   mutable int                 relPosX, relPosY;  // cached relative position of the annotation within bitmap
   mutable Bitmap              annotationBmp;     // cached annotation bitmap
   mutable Bitmap              screenBmp;         // cached screen bitmap

   struct GUIData
   {
      GUIData( AnnotationInterface& );

      VerticalSizer     Global_Sizer;
         GroupBox          Content_GroupBox;
            HorizontalSizer   AnnotationText_Sizer;
               Label             AnnotationText_Label;
               Edit              AnnotationText_Edit;
               CheckBox          AnnotationShowLeader_CheckBox;
         GroupBox          Font_GroupBox;
            VerticalSizer     Font_Sizer;
	            HorizontalSizer   AnnotationFont_Sizer;
		            Label             AnnotationFont_Label;
                  FontComboBox      AnnotationFont_ComboBox;
			         Label             AnnotationFontSize_Label;
	   		      SpinBox           AnnotationFontSize_SpinBox;
		         HorizontalSizer   AnnotationFontOptions_Sizer;
		            CheckBox          AnnotationFontBold_CheckBox;
		    	      CheckBox          AnnotationFontItalic_CheckBox;
			         CheckBox          AnnotationFontUnderline_CheckBox;
			         CheckBox          AnnotationFontShadow_CheckBox;
	            HorizontalSizer   AnnotationColor_Sizer;
		            Label             AnnotationColor_Label;
			         ColorComboBox     AnnotationColor_ComboBox;
			         Control           AnnotationColor_CustomColorSample;
               NumericControl    AnnotationOpacity_NumericControl;
   };

   GUIData* GUI;

   // Destroy working bitmaps
   void ClearBitmaps();

   // synchronize control values with current instance
   void UpdateControls();

   // redraw current invalidate rectangle
   void UpdateView();

   // update text and leader rectangles
   void UpdateAnnotationRect(bool reset = false);

   // place leader to default position (if not yet placed)
   void PlaceLeaderDefault();

   // Event Handlers
   void __EditTextUpdated( Edit& sender, const String &text );
   void __FontComboBoxFontSelected( FontComboBox &sender, const String &font );
   void __FontSizeSpinBoxValueUpdated( SpinBox &sender, int value );
   void __FontOptionCheckBoxChecked( Button &sender, Button::check_state state );
   void __ColorComboBoxColorSelected( ColorComboBox &sender, RGBA color );
   void __ColorSample_Paint( Control& sender, const Rect& );
   void __ColorSample_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __OpacityUpdated( NumericEdit& sender, double value );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AnnotationInterface* TheAnnotationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AnnotationInterface_h

// ----------------------------------------------------------------------------
// EOF AnnotationInterface.h - Released 2019-01-21T12:06:42Z
