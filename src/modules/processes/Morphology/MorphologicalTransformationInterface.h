//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0331
// ----------------------------------------------------------------------------
// MorphologicalTransformationInterface.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
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

#ifndef __MorphologicalTransformationInterface_h
#define __MorphologicalTransformationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/Label.h>
#include <pcl/ComboBox.h>
#include <pcl/SpinBox.h>
#include <pcl/NumericControl.h>
#include <pcl/CheckBox.h>
#include <pcl/PushButton.h>

#include "MorphologicalTransformationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// MorphologicalTransformationInterface
// ----------------------------------------------------------------------------

class MorphologicalTransformationInterface : public ProcessInterface
{
public:

   MorphologicalTransformationInterface();
   virtual ~MorphologicalTransformationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual void SaveSettings() const;
   virtual void LoadSettings();

   // -------------------------------------------------------------------------

   const MorphologicalTransformationInstance& Instance() const
   {
      return instance;
   }

   MorphologicalTransformationInstance& Instance()
   {
      return instance;
   }

   // -------------------------------------------------------------------------

private:

   class UndoItem
   {
   public:

      UndoItem()
      {
      }

      UndoItem( const UndoItem& )
      {
      }

      virtual ~UndoItem()
      {
      }

      virtual bool UndoesStructureName() const
      {
         return false;
      }

      virtual String Description() const = 0;
      virtual void Apply( MorphologicalTransformationInterface& ) = 0;
   };

   class PaintUndo : public UndoItem
   {
   public:

      PaintUndo( const MorphologicalTransformationInterface& i, const String& dsc = "Paint" ) :
      UndoItem(), description( dsc )
      {
         i.instance.GetStructure().GetWay( wayIndex = i.currentWayIndex, wayData );
      }

      PaintUndo( const PaintUndo& x ) :
      UndoItem( x ), wayIndex( x.wayIndex ), wayData( x.wayData ), description( x.description )
      {
      }

      virtual ~PaintUndo()
      {
      }

      virtual String Description() const
      {
         return description;
      }

      virtual void Apply( MorphologicalTransformationInterface& i )
      {
         ByteArray oldWayData;
         i.instance.GetStructure().GetWay( wayIndex, oldWayData );

         i.instance.GetStructure().SetWay( i.currentWayIndex = wayIndex, wayData );
         i.UpdateStructureControls();

         wayData = oldWayData;
      }

   private:
      int       wayIndex;
      ByteArray wayData;
      String    description;
   };

   class RenameUndo : public UndoItem
   {
   public:

      RenameUndo( const MorphologicalTransformationInterface& i ) :
      UndoItem(), name( i.instance.GetStructure().Name() )
      {
      }

      RenameUndo( const RenameUndo& x ) : UndoItem( x ), name( x.name )
      {
      }

      virtual ~RenameUndo()
      {
      }

      virtual bool UndoesStructureName() const
      {
         return true;
      }

      virtual String Description() const
      {
         return "Rename";
      }

      virtual void Apply( MorphologicalTransformationInterface& i )
      {
         String oldName = i.instance.GetStructure().Name();
         i.instance.GetStructure().Rename( name );
         name = oldName;
         i.UpdateStructureControls();
      }

   private:
      String name;
   };

   class WholeStructureUndo : public UndoItem
   {
   public:

      WholeStructureUndo( const MorphologicalTransformationInterface& i ) : UndoItem(), structure( i.instance.GetStructure() )
      {
      }

      WholeStructureUndo( const WholeStructureUndo& x ) : UndoItem( x ), structure( x.structure )
      {
      }

      virtual ~WholeStructureUndo()
      {
      }

      virtual bool UndoesStructureName() const
      {
         return true;
      }

      virtual String Description() const = 0;

      virtual void Apply( MorphologicalTransformationInterface& i )
      {
         Structure oldStructure = i.instance.GetStructure();

         i.instance.GetStructure() = structure;

         if ( i.currentWayIndex >= structure.NumberOfWays() )
            i.currentWayIndex = structure.NumberOfWays() - 1;

         i.UpdateStructureControls();

         structure = oldStructure;
      }

   private:
      Structure structure;
   };

   class GlobalPaintUndo : public WholeStructureUndo
   {
   public:

      GlobalPaintUndo( const MorphologicalTransformationInterface& i, const String& dsc = "Global paint" ) :
      WholeStructureUndo( i ), description( dsc )
      {
      }

      GlobalPaintUndo( const GlobalPaintUndo& x ) : WholeStructureUndo( x ), description( x.description )
      {
      }

      virtual ~GlobalPaintUndo()
      {
      }

      virtual String Description() const
      {
         return description;
      }

   private:
      String description;
   };

   class ResizeUndo : public WholeStructureUndo
   {
   public:

      ResizeUndo( const MorphologicalTransformationInterface& i ) : WholeStructureUndo( i )
      {
      }

      ResizeUndo( const ResizeUndo& x ) : WholeStructureUndo( x )
      {
      }

      virtual ~ResizeUndo()
      {
      }

      virtual String Description() const
      {
         return "Resize";
      }
   };

   class ChangeWayCountUndo : public WholeStructureUndo
   {
   public:

      ChangeWayCountUndo( const MorphologicalTransformationInterface& i ) : WholeStructureUndo( i )
      {
      }

      ChangeWayCountUndo( const ChangeWayCountUndo& x ) : WholeStructureUndo( x )
      {
      }

      virtual ~ChangeWayCountUndo()
      {
      }

      virtual String Description() const
      {
         return "Change way count";
      }
   };

   typedef IndirectArray<UndoItem>  undo_item_list;

   // -------------------------------------------------------------------------

   //
   // Current instance and structure collection
   //
   MorphologicalTransformationInstance instance;
   StructureCollection                 collection;

   //
   // Workflow
   //

   int                  currentWayIndex;

   undo_item_list  undoList;
   undo_item_list  redoList;

   String               initialStructureName;

   ByteArray            storedWay;

   bool                 paintMode;
   bool                 painting;

   bool                 showAllWays;

   bool CanUndo() const
   {
      return !undoList.IsEmpty();
   }

   bool CanRedo() const
   {
      return !redoList.IsEmpty();
   }

   void Do( UndoItem* item )
   {
      if ( undoList.IsEmpty() && !item->UndoesStructureName() )
         if ( !initialStructureName.IsEmpty() )
         {
            instance.structure.name = String();
            UpdateStructureControls();
         }

      redoList.Destroy();
      undoList.Add( item );

      UpdateStructureUndoControls();
   }

   void Undo()
   {
      if ( CanUndo() )
      {
         undo_item_list::iterator i = undoList.At( undoList.UpperBound() );
         UndoItem* item = *i;
         undoList.Remove( i );

         if ( undoList.IsEmpty() && !item->UndoesStructureName() )
            if ( !initialStructureName.IsEmpty() )
               instance.structure.name = initialStructureName;

         item->Apply( *this );

         redoList.Add( item );

         UpdateStructureUndoControls();
      }
   }

   void Redo()
   {
      if ( CanRedo() )
      {
         undo_item_list::iterator i = redoList.At( redoList.UpperBound() );
         UndoItem* item = *i;
         redoList.Remove( i );

         if ( undoList.IsEmpty() && !item->UndoesStructureName() )
            if ( !initialStructureName.IsEmpty() )
               instance.structure.name = String();

         item->Apply( *this );

         undoList.Add( item );

         UpdateStructureUndoControls();
      }
   }

   void ResetUndo()
   {
      undoList.Destroy();
      redoList.Destroy();
      UpdateStructureUndoControls();
   }

   //

   bool CanRestoreStructure()
   {
      return !storedWay.IsEmpty();
   }

   void StoreStructure()
   {
      if ( instance.structure.IsEmptyWay( currentWayIndex ) )
         storedWay.Clear();
      else
         instance.structure.GetWay( currentWayIndex, storedWay );
      GUI->RestoreWay_ToolButton.Enable( CanRestoreStructure() );
   }

   void RestoreStructure()
   {
      if ( CanRestoreStructure() )
      {
         Do( new PaintUndo( *this, "Restore structure" ) );
         instance.structure.SetWay( currentWayIndex, storedWay );
         UpdateStructureControls();
      }
   }

   //
   // GUI
   //

   struct GUIData
   {
      GUIData( MorphologicalTransformationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Filter_SectionBar;
      Control           Filter_Control;
      VerticalSizer     Filter_Sizer;
         HorizontalSizer   Operator_Sizer;
            Label             Operator_Label;
            ComboBox          Operator_ComboBox;
         HorizontalSizer   Interlacing_Sizer;
            Label             Interlacing_Label;
            SpinBox           Interlacing_SpinBox;
         HorizontalSizer   Iterations_Sizer;
            Label             Iterations_Label;
            SpinBox           Iterations_SpinBox;
         NumericControl    Amount_NumericControl;
         NumericControl    Selection_NumericControl;

      SectionBar        Structure_SectionBar;
      Control           Structure_Control;
      VerticalSizer     Structure_Sizer;
         HorizontalSizer   StructureTop_Sizer;
            VerticalSizer     StructureEditor_Sizer;
               Control           StructureEditor_Control;
            VerticalSizer     StructureData_Sizer;
               HorizontalSizer   StructureSize_Sizer;
                  Label             StructureSize_Label;
                  ComboBox          StructureSize_ComboBox;
               HorizontalSizer   StructureWay_Sizer;
                  Label             StructureWay_Label;
                  ComboBox          StructureWay_ComboBox;
               HorizontalSizer   StructureActions1_Sizer;
                  ToolButton        UndoStructure_ToolButton;
                  ToolButton        InvertWay_ToolButton;
                  ToolButton        RotateWay_ToolButton;
                  ToolButton        SetWay_ToolButton;
                  ToolButton        AddWay_ToolButton;
                  ToolButton        DeleteWay_ToolButton;
               HorizontalSizer   StructureActions2_Sizer;
                  ToolButton        RedoStructure_ToolButton;
                  ToolButton        CircularWay_ToolButton;
                  ToolButton        DiamondWay_ToolButton;
                  ToolButton        ClearWay_ToolButton;
                  ToolButton        SetAllWays_ToolButton;
                  ToolButton        ClearAllWays_ToolButton;
               HorizontalSizer   StructureActions3_Sizer;
                  ToolButton        ResetStructure_ToolButton;
                  ToolButton        OrthogonalWay_ToolButton;
                  ToolButton        DiagonalWay_ToolButton;
                  ToolButton        StoreWay_ToolButton;
                  ToolButton        RestoreWay_ToolButton;
         HorizontalSizer   StructureMiddle_Sizer;
            ToolButton        PaintSet_ToolButton;
            ToolButton        PaintClear_ToolButton;
            ToolButton        ShowAllWays_ToolButton;
            Label             StructureInfo_Label;
            PushButton        ManageStructures_PushButton;
         Edit              StructureName_Edit;

      SectionBar        Thresholds_SectionBar;
      Control           Thresholds_Control;
      VerticalSizer     Thresholds_Sizer;
         NumericControl    LowThreshold_NumericControl;
         NumericControl    HighThreshold_NumericControl;
   };

   GUIData* GUI;

   //
   // GUI Updates
   //
   void UpdateControls();
   void UpdateFilterControls();
   void UpdateStructureControls();
   void UpdateStructureUndoControls();
   void UpdateThresholdControls();

   //
   // GUI Event Handlers
   //

   void __Operator_ItemSelected( ComboBox& sender, int itemIndex );
   void __Interlacing_ValueUpdated( SpinBox& sender, int value );
   void __Iterations_ValueUpdated( SpinBox& sender, int value );
   void __Amount_ValueUpdated( NumericEdit& sender, double value );
   void __Selection_ValueUpdated( NumericEdit& sender, double value );

   void __Structure_Paint( Control& sender, const Rect& updateRect );
   void __Structure_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void __Structure_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Structure_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __Structure_Leave( Control& sender );

   void __StructureSize_ItemSelected( ComboBox& sender, int itemIndex );
   void __StructureWay_ItemSelected( ComboBox& sender, int itemIndex );

   void __WayAction_ButtonClick( Button& sender, bool checked );
   void __PaintMode_ButtonClick( Button& sender, bool checked );

   void __DrawingOption_ButtonClick( Button& sender, bool checked );

   void __StructureName_GetFocus( Control& sender );
   void __StructureName_EditCompleted( Edit& sender );

   void __ManageStructures_ButtonClick( Button& sender, bool checked );

   void __Threshold_ValueUpdated( NumericEdit& sender, double value );

   // -------------------------------------------------------------------------

   friend struct GUIData;

   friend class StructureManagerDialog;

   friend class PaintUndo;
   friend class WholeStructureUndo;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern MorphologicalTransformationInterface* TheMorphologicalTransformationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MorphologicalTransformationInterface_h

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationInterface.h - Released 2017-08-01T14:26:58Z
