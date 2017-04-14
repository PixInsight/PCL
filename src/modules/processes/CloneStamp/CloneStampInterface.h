//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 01.00.02.0295
// ----------------------------------------------------------------------------
// CloneStampInterface.h - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#ifndef __CloneStampInterface_h
#define __CloneStampInterface_h

#include <pcl/CheckBox.h>
#include <pcl/IndirectSortedArray.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>

#include "CloneStampInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class View;

class CloneStampInterface : public ProcessInterface
{
public:

   CloneStampInterface();
   virtual ~CloneStampInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual void Execute();
   virtual void Cancel();
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool IsDynamicInterface() const;

   virtual void ExitDynamicMode();

   virtual void DynamicMouseEnter( View& );
   virtual void DynamicMouseLeave( View& );
   virtual void DynamicMouseMove( View&, const DPoint&, unsigned buttons, unsigned modifiers );
   virtual void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );
   virtual void DynamicMouseRelease( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );
   virtual bool DynamicKeyPress( View& v, int key, unsigned modifiers );
   virtual bool DynamicKeyRelease( View& v, int key, unsigned modifiers );

   virtual bool RequiresDynamicUpdate( const View&, const DRect& ) const;
   virtual void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const;

   virtual void SaveSettings() const;
   virtual void LoadSettings();

   virtual bool WantsMaskNotifications() const;
   virtual void MaskUpdated( const View& v );
   virtual void MaskEnabled( const View& v );
   virtual void MaskDisabled( const View& v );

   // -------------------------------------------------------------------------

   typedef CloneStampInstance::ClonerData       ClonerData;
   typedef CloneStampInstance::BrushData        BrushData;
   typedef CloneStampInstance::cloner_sequence  cloner_sequence;

   struct ViewRef
   {
      View  view;
      int   refCount;

      ViewRef( View& );
      ViewRef( const View&, int );
      ~ViewRef();

      bool operator ==( const ViewRef& x ) const
      {
         return view == x.view;
      }

      bool operator <( const ViewRef& x ) const
      {
         return view < x.view;
      }
   };

   typedef IndirectSortedArray<ViewRef>   source_view_list;

   struct ClonerAction
   {
      ViewRef*        source;
      BrushData       brush;
      Point           delta;
      cloner_sequence cloner;
      Rect            bounds;

      ClonerAction( View&, const BrushData&, const Point& );
      ClonerAction( const ClonerAction& );
      ~ClonerAction();

      void UpdateBounds();
   };

   typedef Array<ClonerAction> action_list;

private:

   View*     targetView; // dynamic targets
   View*     sourceView;

   BrushData brush;      // current brush parameters
   DPoint    delta;      // current target - source offset
   DPoint    targetPos;  // current target position

   bool      initialized;     // a source point has been selected for the 1st time
   bool      selectingSource; // selecting a source view/point
   bool      selectingTarget; // selecting a target point

   bool      executing;       // the current instance is being executed
   bool      onTarget;        // the mouse cursor is on the target view
   bool      dragging;        // dragging the mouse
   bool      imageChanged;    // the image has been changed during the current action

   Rect      regenRect;

   uint32    color;       // drawing color, cloner
   uint32    boundsColor; // drawing color, action boundaries

   bool      showBounds;  // show action boundaries

   Array<ImageVariant> swapTiles; // swap tile matrix
   int                 tileRows;
   int                 tileCols;

   action_list done, undone; // history lists

   DPoint TargetPos() const
   {
      return selectingTarget ? targetPos + delta : targetPos;
   }

   DPoint SourcePos() const
   {
      return (selectingSource || selectingTarget) ? targetPos : targetPos - delta;
   }

   // Swap tiles
   void StoreRect( const Rect& );
   void RestoreRect( const Rect& );
   void RestoreView();
   void XchgTiles();

   // History
   void Undo();
   void UndoAll();
   void Redo();
   void RedoAll();
   void Delete();
   void PlayBack( action_list::const_iterator i, action_list::const_iterator j );

   //

   void ExportInstance( CloneStampInstance& ) const;
   void Reset();

   void SelectTarget( View& );
   void SelectSource( View& );

   void UpdateViews();
   void RegenerateView();

   void JumpToCurrentAction();

   void Broadcast();

   Rect Bounds() const;

   void Rasterize( int dx, int dy, int step );
   void RasterizeStep( int dx, int dy );

   struct GUIData
   {
      GUIData( CloneStampInterface& );

      VerticalSizer  Global_Sizer;

      HorizontalSizer   Brush_Sizer;
         VerticalSizer     BrushParameters_Sizer;
            HorizontalSizer   BrushRadius_Sizer;
               Label             BrushRadius_Label;
               SpinBox           BrushRadius_SpinBox;
               PushButton        CopyBrush_PushButton;
               CheckBox          ShowBounds_CheckBox;
            NumericControl    BrushSoftness_NumericControl;
            NumericControl    BrushOpacity_NumericControl;
         Control           Brush_Control;

      HorizontalSizer   History_Sizer;
         Label             HistoryInfo_Label;
         ToolButton        Delete_ToolButton;
         ToolButton        UndoAll_ToolButton;
         ToolButton        UndoOne_ToolButton;
         ToolButton        RedoOne_ToolButton;
         ToolButton        RedoAll_ToolButton;

#ifndef __PCL_WINDOWS
      Timer Regen_Timer;
#endif
   };

   GUIData* GUI;

   void UpdateControls();
   void UpdateHistoryInfo();

   void __BrushRadius_ValueUpdated( SpinBox& sender, int value );
   void __Brush_ValueUpdated( NumericEdit& sender, double value );
   void __History_Click( Button& sender, bool checked );
   void __Brush_Paint( Control& sender, const Rect& updateRect );
   void __Option_Click( Button& sender, bool checked );
#ifndef __PCL_WINDOWS
   void __Regen_Timer( Timer& sender );
#endif

   friend struct GUIData;
   friend class CloneStampInstance;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CloneStampInterface* TheCloneStampInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CloneStampInterface_h

// ----------------------------------------------------------------------------
// EOF CloneStampInterface.h - Released 2017-04-14T23:07:12Z
