//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0331
// ----------------------------------------------------------------------------
// MorphologicalTransformationInterface.cpp - Released 2017-08-01T14:26:58Z
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

#include "MorphologicalTransformationInterface.h"
#include "MorphologicalTransformationProcess.h"
#include "MorphologicalTransformationParameters.h"
#include "StructureManagerDialog.h"

#include <pcl/ImageWindow.h>
#include <pcl/Graphics.h>
#include <pcl/Settings.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ErrorHandler.h>

#define AUTO_ID   "<Unnamed>"

namespace pcl
{

// ----------------------------------------------------------------------------

MorphologicalTransformationInterface* TheMorphologicalTransformationInterface = 0;

// ----------------------------------------------------------------------------

#include "MorphologicalTransformationIcon.xpm"

#include "paint_clear.xpm"
#include "paint_set.xpm"
#include "show_all_ways.xpm"
#include "way_add.xpm"
#include "way_delete.xpm"
#include "way_set.xpm"
#include "way_set_all.xpm"
#include "way_clear.xpm"
#include "way_clear_all.xpm"
#include "way_store.xpm"
#include "way_restore.xpm"
#include "way_invert.xpm"
#include "way_rotate.xpm"
#include "way_circular.xpm"
#include "way_diamond.xpm"
#include "way_orthogonal.xpm"
#include "way_diagonal.xpm"
#include "undo_structure.xpm"
#include "redo_structure.xpm"
#include "reset_structure.xpm"
#include "add_structure.xpm"
#include "replace_structure.xpm"
#include "delete_structure.xpm"
#include "delete_all_structures.xpm"
#include "pen_cursor.xpm"

// ----------------------------------------------------------------------------

MorphologicalTransformationInterface::MorphologicalTransformationInterface() :
ProcessInterface(),
instance( TheMorphologicalTransformationProcess ),
currentWayIndex( 0 ),
undoList(),
redoList(),
initialStructureName(),
storedWay(),
paintMode( true ),
painting( false ),
showAllWays( true ),
GUI( 0 )
{
   TheMorphologicalTransformationInterface = this;
}

MorphologicalTransformationInterface::~MorphologicalTransformationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString MorphologicalTransformationInterface::Id() const
{
   return "MorphologicalTransformation";
}

MetaProcess* MorphologicalTransformationInterface::Process() const
{
   return TheMorphologicalTransformationProcess;
}

const char** MorphologicalTransformationInterface::IconImageXPM() const
{
   return MorphologicalTransformationIcon_XPM;
}

void MorphologicalTransformationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void MorphologicalTransformationInterface::ResetInstance()
{
   MorphologicalTransformationInstance defaultInstance( TheMorphologicalTransformationProcess );
   ImportProcess( defaultInstance );
}

bool MorphologicalTransformationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "MorphologicalTransformation" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheMorphologicalTransformationProcess;
}

ProcessImplementation* MorphologicalTransformationInterface::NewProcess() const
{
   return new MorphologicalTransformationInstance( instance );
}

bool MorphologicalTransformationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const MorphologicalTransformationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a MorphologicalTransformation instance.";
   return false;
}

bool MorphologicalTransformationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool MorphologicalTransformationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   currentWayIndex = 0;
   initialStructureName = instance.structure.name;
   ResetUndo();
   UpdateControls();
   return true;
}

void MorphologicalTransformationInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "InitialStructureName", instance.structure.name );
   Settings::Write( SettingsKey() + "ShowAllWays", showAllWays );
   Settings::Write( SettingsKey() + "StructureSetModule/FilePath", collection.Path() );
   instance.GetStructure().Save( SettingsKey() + "CurrentStructure/" );
}

#define DEFAULT_SSM_FILE   "default.ssm"

void MorphologicalTransformationInterface::LoadSettings()
{
   Settings::Read( SettingsKey() + "InitialStructureName", initialStructureName );

   Settings::Read( SettingsKey() + "ShowAllWays", showAllWays );

   String ssmFilePath;
   Settings::Read( SettingsKey() + "StructureSetModule/FilePath", ssmFilePath );

   try
   {
      if ( ssmFilePath.IsEmpty() || !File::Exists( ssmFilePath ) )
      {
         ssmFilePath = PixInsightSettings::GlobalString( "Application/BaseDirectory" ) + "/library/" + DEFAULT_SSM_FILE;
         if ( !File::Exists( ssmFilePath ) )
            throw Error( "The structure set module file does not exist: " + ssmFilePath );
      }

      collection.Read( ssmFilePath );

      instance.GetStructure().Load( SettingsKey() + "CurrentStructure/" );
   }
   ERROR_HANDLER

   currentWayIndex = 0;
   UpdateControls();
}

// ----------------------------------------------------------------------------

void MorphologicalTransformationInterface::UpdateControls()
{
   UpdateFilterControls();
   UpdateStructureControls();
   UpdateStructureUndoControls();
   UpdateThresholdControls();
}

void MorphologicalTransformationInterface::UpdateFilterControls()
{
   GUI->Operator_ComboBox.SetCurrentItem( instance.morphologicalOperator );

   GUI->Interlacing_SpinBox.SetValue( instance.interlacingDistance );

   GUI->Iterations_SpinBox.SetValue( instance.numberOfIterations );

   GUI->Amount_NumericControl.SetValue( instance.amount );

   GUI->Selection_NumericControl.SetValue( instance.selectionPoint );
   GUI->Selection_NumericControl.Enable( instance.morphologicalOperator == MorphologicalOp::Selection );
}

void MorphologicalTransformationInterface::UpdateStructureControls()
{
   GUI->StructureEditor_Control.Update();

   GUI->StructureSize_ComboBox.SetCurrentItem( (instance.structure.Size() >> 1) - 1 );

   GUI->StructureWay_ComboBox.Clear();
   for ( int i = 0; i < instance.structure.NumberOfWays(); ++i )
      GUI->StructureWay_ComboBox.AddItem( String().Format( "%d of %d", i+1, instance.structure.NumberOfWays() ) );
   GUI->StructureWay_ComboBox.SetCurrentItem( currentWayIndex );

   GUI->DeleteWay_ToolButton.Enable( instance.structure.NumberOfWays() > 1 );

   GUI->PaintSet_ToolButton.SetChecked( paintMode );
   GUI->PaintClear_ToolButton.SetChecked( !paintMode );
   GUI->ShowAllWays_ToolButton.SetChecked( showAllWays );

   //GUI->StructureInfo_Label;

   GUI->StructureName_Edit.SetText( instance.structure.name.IsEmpty() ? AUTO_ID : instance.structure.name );
}

void MorphologicalTransformationInterface::UpdateStructureUndoControls()
{
   GUI->UndoStructure_ToolButton.Enable( CanUndo() );
   GUI->UndoStructure_ToolButton.SetToolTip( CanUndo() ? "Undo " + undoList.Last()->Description() : "Nothing to undo" );
   GUI->RedoStructure_ToolButton.Enable( CanRedo() );
   GUI->RedoStructure_ToolButton.SetToolTip( CanRedo() ? "Redo " + redoList.Last()->Description() : "Nothing to redo" );
}

void MorphologicalTransformationInterface::UpdateThresholdControls()
{
   GUI->LowThreshold_NumericControl.SetValue( instance.lowThreshold );
   GUI->HighThreshold_NumericControl.SetValue( instance.highThreshold );
}

// ----------------------------------------------------------------------------

void MorphologicalTransformationInterface::__Operator_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   instance.morphologicalOperator = itemIndex;
   GUI->Selection_NumericControl.Enable( itemIndex == MorphologicalOp::Selection );
}

void MorphologicalTransformationInterface::__Interlacing_ValueUpdated( SpinBox& /*sender*/, int value )
{
   instance.interlacingDistance = value;
}

void MorphologicalTransformationInterface::__Iterations_ValueUpdated( SpinBox& /*sender*/, int value )
{
   instance.numberOfIterations = value;
}

void MorphologicalTransformationInterface::__Amount_ValueUpdated( NumericEdit& /*sender*/, double value )
{
   instance.amount = value;
}

void MorphologicalTransformationInterface::__Selection_ValueUpdated( NumericEdit& /*sender*/, double value )
{
   instance.selectionPoint = value;
}

void MorphologicalTransformationInterface::__Structure_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   int w = sender.Width();
   int h = sender.Height();
   int sz = instance.structure.Size();

   float dx = float( w - 1 )/sz;
   float dy = float( h - 1 )/sz;

   Graphics g( sender );

   g.EraseRect( sender.BoundsRect() );

   g.SetPen( 0xff606060 );

   for ( int y = 0, gridCount = 0; ; ++y )
   {
      int ey = RoundI( y*dy );

      g.DrawLine( 0, ey, w, ey );

      if ( y == sz )
         break;

      int ey1 = RoundI( (y + 1)*dy );

      for ( int x = 0; ; ++x )
      {
         int ex = RoundI( x*dx );

         if ( gridCount <= sz )
         {
            g.DrawLine( ex, 0, ex, h );
            ++gridCount;
         }

         if ( x == sz )
            break;

         int ex1 = RoundI( (x + 1)*dx );

         if ( instance.structure.NumberOfWays() > 1 && showAllWays )
         {
            for ( int i = 0; i < instance.structure.NumberOfWays(); ++i )
               if ( i != currentWayIndex && instance.structure.Element( i, x, y ) )
               {
                  RGBA color;
                  switch ( i )
                  {
                  case 0:  color = 0xffff0000; break; // red
                  case 1:  color = 0xff00ff00; break; // green
                  case 2:  color = 0xff0000ff; break; // blue
                  case 3:  color = 0xffffff00; break; // yellow
                  case 4:  color = 0xff00ffff; break; // cyan
                  case 5:  color = 0xffff00ff; break; // magenta
                  case 6:  color = 0xffff7f00; break; // orange
                  case 7:  color = 0xff96288c; break; // violet
                  case 8:  color = 0xffebb940; break; // gold
                  case 9:  color = 0xff4898ea; break; // light blue
                  default: color = 0xffb4b4b4; break; // gray
                  }

                  g.FillRect( ex+1, ey+1, ex1, ey1, color );
               }
         }

         if ( instance.structure.Element( currentWayIndex, x, y ) )
            g.FillRect( ex+1, ey+1, ex1, ey1, 0xffffffff );
      }
   }
}

void MorphologicalTransformationInterface::__Structure_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   float dx = float( sender.Width() )/instance.structure.Size();
   float dy = float( sender.Height() )/instance.structure.Size();

   int x = pos.x/dx;
   int y = pos.y/dy;

   if ( x >= 0 && x < instance.structure.Size() && y >= 0 && y < instance.structure.Size() )
   {
      bool value = instance.structure.Element( currentWayIndex, x, y );

      String info = String().Format( "row:%d col:%d %s", y, x, value ? "on" : "off" );

      if ( painting )
      {
         bool newValue = paintMode;

         if ( modifiers & KeyModifier::Control )
            newValue = !newValue;

         if ( value != newValue )
         {
            instance.structure.SetElement( currentWayIndex, x, y, newValue );
            GUI->StructureEditor_Control.Update();

            info.AppendFormat( "->%s", newValue ? "on" : "off" );
         }
      }

      GUI->StructureInfo_Label.SetText( info );
   }
   else
      GUI->StructureInfo_Label.Clear();
}

void MorphologicalTransformationInterface::__Structure_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   Do( new PaintUndo( *this ) );
   painting = true;
   __Structure_MouseMove( sender, pos, buttons, modifiers );
}

void MorphologicalTransformationInterface::__Structure_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   painting = false;
}

void MorphologicalTransformationInterface::__Structure_Leave( Control& /*sender*/ )
{
   GUI->StructureInfo_Label.Clear();
}

void MorphologicalTransformationInterface::__StructureSize_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   Do( new ResizeUndo( *this ) );
   instance.structure.Resize( 3 + itemIndex*2 );
   GUI->StructureEditor_Control.Update();
}

void MorphologicalTransformationInterface::__StructureWay_ItemSelected( ComboBox& sender, int itemIndex )
{
   currentWayIndex = itemIndex;
   UpdateStructureControls();
}

void MorphologicalTransformationInterface::__WayAction_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->UndoStructure_ToolButton )
   {
      if ( CanUndo() )
         Undo();
   }
   else if ( sender == GUI->InvertWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Invert way" ) );
      instance.structure.Invert( currentWayIndex );
   }
   else if ( sender == GUI->RotateWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Rotate way" ) );
      instance.structure.Rotate( currentWayIndex );
   }
   else if ( sender == GUI->SetWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Set way" ) );
      instance.structure.SetWay( currentWayIndex );
   }
   else if ( sender == GUI->AddWay_ToolButton )
   {
      Do( new ChangeWayCountUndo( *this ) );
      instance.structure.AddWay();
      currentWayIndex = instance.structure.NumberOfWays() - 1;
   }
   else if ( sender == GUI->DeleteWay_ToolButton )
   {
      if ( instance.structure.NumberOfWays() > 1 )
      {
         Do( new ChangeWayCountUndo( *this ) );
         instance.structure.RemoveWay( currentWayIndex );
         if ( currentWayIndex >= instance.structure.NumberOfWays() )
            --currentWayIndex;
      }
   }
   else if ( sender == GUI->RedoStructure_ToolButton )
   {
      if ( CanRedo() )
         Redo();
   }
   else if ( sender == GUI->CircularWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Circular structure" ) );
      instance.structure.Circular( currentWayIndex );
   }
   else if ( sender == GUI->DiamondWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Diamond structure" ) );
      instance.structure.Diamond( currentWayIndex );
   }
   else if ( sender == GUI->ClearWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Clear way" ) );
      instance.structure.ClearWay( currentWayIndex );
   }
   else if ( sender == GUI->SetAllWays_ToolButton )
   {
      Do( new GlobalPaintUndo( *this, "Set all" ) );
      instance.structure.Set();
   }
   else if ( sender == GUI->ClearAllWays_ToolButton )
   {
      Do( new GlobalPaintUndo( *this, "Clear all" ) );
      instance.structure.Clear();
   }
   else if ( sender == GUI->ResetStructure_ToolButton )
   {
      MorphologicalTransformationInstance defaultInstance( TheMorphologicalTransformationProcess );
      instance.structure = defaultInstance.structure;
      currentWayIndex = 0;
      ResetUndo();
      UpdateStructureControls();
   }
   else if ( sender == GUI->OrthogonalWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Orthogonal structure" ) );
      instance.structure.Orthogonal( currentWayIndex );
   }
   else if ( sender == GUI->DiagonalWay_ToolButton )
   {
      Do( new PaintUndo( *this, "Diagonal structure" ) );
      instance.structure.Diagonal( currentWayIndex );
   }
   else if ( sender == GUI->StoreWay_ToolButton )
   {
      StoreStructure();
   }
   else if ( sender == GUI->RestoreWay_ToolButton )
   {
      if ( CanRestoreStructure() )
         RestoreStructure();
   }

   UpdateStructureControls();
}

void MorphologicalTransformationInterface::__PaintMode_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->PaintSet_ToolButton )
   {
      paintMode = true;
      GUI->PaintClear_ToolButton.Uncheck();
   }
   else if ( sender == GUI->PaintClear_ToolButton )
   {
      paintMode = false;
      GUI->PaintSet_ToolButton.Uncheck();
   }
}

void MorphologicalTransformationInterface::__DrawingOption_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->ShowAllWays_ToolButton )
   {
      showAllWays = checked;
      GUI->StructureEditor_Control.Update();
   }
}

void MorphologicalTransformationInterface::__StructureName_GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != 0 && e->Text() == AUTO_ID )
      e->Clear();
}

void MorphologicalTransformationInterface::__StructureName_EditCompleted( Edit& sender )
{
   pcl::String name = sender.Text();
   name.Trim();

   if ( name == AUTO_ID )
      name.Clear();

   if ( name != instance.structure.name )
   {
      Do( new RenameUndo( *this ) );
      instance.structure.name = name;
   }

   UpdateStructureControls();
}

void MorphologicalTransformationInterface::__ManageStructures_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   StructureManagerDialog d( *this );

   d.Execute();
   UpdateControls();
}

void MorphologicalTransformationInterface::__Threshold_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->LowThreshold_NumericControl )
      instance.lowThreshold = value;
   else if ( sender == GUI->HighThreshold_NumericControl )
      instance.highThreshold = value;
   UpdateThresholdControls();
}

// ----------------------------------------------------------------------------

MorphologicalTransformationInterface::GUIData::GUIData( MorphologicalTransformationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Interlacing:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( "Size:" ) + 'M' );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   Filter_SectionBar.SetTitle( "Morphological Filter" );
   Filter_SectionBar.SetSection( Filter_Control );

   //

   Operator_Label.SetText( "Operator:" );
   Operator_Label.SetFixedWidth( labelWidth1 );
   Operator_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Operator_ComboBox.AddItem( "Erosion (Minimum)" );
   Operator_ComboBox.AddItem( "Dilation (Maximum)" );
   Operator_ComboBox.AddItem( "Opening (Dilation + Erosion)" );
   Operator_ComboBox.AddItem( "Closing (Erosion + Dilation)" );
   Operator_ComboBox.AddItem( "Morphological Median" );
   Operator_ComboBox.AddItem( "Morphological Selection" );
   Operator_ComboBox.AddItem( "Midpoint: (Min+Max)/2" );
   Operator_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&MorphologicalTransformationInterface::__Operator_ItemSelected, w );

   Operator_Sizer.SetSpacing( 4 );
   Operator_Sizer.Add( Operator_Label );
   Operator_Sizer.Add( Operator_ComboBox, 100 );

   //

   Interlacing_Label.SetText( "Interlacing:" );
   Interlacing_Label.SetFixedWidth( labelWidth1 );
   Interlacing_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Interlacing_SpinBox.SetRange( 1, int( TheInterlacingDistanceParameter->MaximumValue() ) );
   Interlacing_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&MorphologicalTransformationInterface::__Interlacing_ValueUpdated, w );

   Interlacing_Sizer.SetSpacing( 4 );
   Interlacing_Sizer.Add( Interlacing_Label );
   Interlacing_Sizer.Add( Interlacing_SpinBox );
   Interlacing_Sizer.AddStretch();

   //

   Iterations_Label.SetText( "Iterations:" );
   Iterations_Label.SetFixedWidth( labelWidth1 );
   Iterations_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Iterations_SpinBox.SetRange( 1, int( TheNumberOfIterationsParameter->MaximumValue() ) );
   Iterations_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&MorphologicalTransformationInterface::__Iterations_ValueUpdated, w );

   Iterations_Sizer.SetSpacing( 4 );
   Iterations_Sizer.Add( Iterations_Label );
   Iterations_Sizer.Add( Iterations_SpinBox );
   Iterations_Sizer.AddStretch();

   //

   Amount_NumericControl.label.SetText( "Amount:" );
   Amount_NumericControl.label.SetFixedWidth( labelWidth1 );
   Amount_NumericControl.slider.SetRange( 0, 100 );
   Amount_NumericControl.slider.SetScaledMinWidth( 150 );
   Amount_NumericControl.SetReal();
   Amount_NumericControl.SetRange( 0, 1 );
   Amount_NumericControl.SetPrecision( TheAmountParameter->Precision() );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&MorphologicalTransformationInterface::__Amount_ValueUpdated, w );

   //

   Selection_NumericControl.label.SetText( "Selection:" );
   Selection_NumericControl.label.SetFixedWidth( labelWidth1 );
   Selection_NumericControl.slider.SetRange( 0, 100 );
   Selection_NumericControl.slider.SetScaledMinWidth( 150 );
   Selection_NumericControl.SetReal();
   Selection_NumericControl.SetRange( 0, 1 );
   Selection_NumericControl.SetPrecision( TheSelectionPointParameter->Precision() );
   Selection_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&MorphologicalTransformationInterface::__Selection_ValueUpdated, w );

   //

   Filter_Sizer.SetSpacing( 4 );
   Filter_Sizer.Add( Operator_Sizer );
   Filter_Sizer.Add( Interlacing_Sizer );
   Filter_Sizer.Add( Iterations_Sizer );
   Filter_Sizer.Add( Amount_NumericControl );
   Filter_Sizer.Add( Selection_NumericControl );

   Filter_Control.SetSizer( Filter_Sizer );

   //

   Structure_SectionBar.SetTitle( "Structuring Element" );
   Structure_SectionBar.SetSection( Structure_Control );

   //

   StructureEditor_Control.SetBackgroundColor( StringToRGBAColor( "black" ) );
   StructureEditor_Control.SetScaledFixedSize( 126, 126 );  // 25 elements x 5 pixels + bottom/right frame
   StructureEditor_Control.EnableMouseTracking();
   StructureEditor_Control.SetCursor( pcl::Cursor( Bitmap( pen_cursor_XPM ), 5, 0 ) );
   StructureEditor_Control.OnPaint( (Control::paint_event_handler)&MorphologicalTransformationInterface::__Structure_Paint, w );
   StructureEditor_Control.OnMousePress( (Control::mouse_button_event_handler)&MorphologicalTransformationInterface::__Structure_MousePress, w );
   StructureEditor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&MorphologicalTransformationInterface::__Structure_MouseRelease, w );
   StructureEditor_Control.OnMouseMove( (Control::mouse_event_handler)&MorphologicalTransformationInterface::__Structure_MouseMove, w );
   StructureEditor_Control.OnLeave( (Control::event_handler)&MorphologicalTransformationInterface::__Structure_Leave, w );

   StructureEditor_Sizer.AddStretch();
   StructureEditor_Sizer.Add( StructureEditor_Control );
   StructureEditor_Sizer.AddStretch();

   //

   StructureSize_Label.SetText( "Size:" );
   StructureSize_Label.SetFixedWidth( labelWidth2 );
   StructureSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   for ( int i = 3, m = int( TheStructureSizeParameter->MaximumValue() ); i <= m; i += 2 )
      StructureSize_ComboBox.AddItem( String().Format( "%d  (%d elements)", i, i*i ) );
   StructureSize_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&MorphologicalTransformationInterface::__StructureSize_ItemSelected, w );

   StructureSize_Sizer.SetSpacing( 4 );
   StructureSize_Sizer.Add( StructureSize_Label );
   StructureSize_Sizer.Add( StructureSize_ComboBox );

   //

   StructureWay_Label.SetText( "Way:" );
   StructureWay_Label.SetFixedWidth( labelWidth2 );
   StructureWay_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StructureWay_ComboBox.AddItem( "1 of 1" );
   StructureWay_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&MorphologicalTransformationInterface::__StructureWay_ItemSelected, w );

   StructureWay_Sizer.SetSpacing( 4 );
   StructureWay_Sizer.Add( StructureWay_Label );
   StructureWay_Sizer.Add( StructureWay_ComboBox );

   //

   UndoStructure_ToolButton.SetIcon( Bitmap( undo_structure_XPM ) );
   UndoStructure_ToolButton.SetScaledFixedSize( 19, 19 );
   UndoStructure_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   UndoStructure_ToolButton.SetToolTip( "Undo" );
   UndoStructure_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   InvertWay_ToolButton.SetIcon( Bitmap( way_invert_XPM ) );
   InvertWay_ToolButton.SetScaledFixedSize( 19, 19 );
   InvertWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   InvertWay_ToolButton.SetToolTip( "Invert Way" );
   InvertWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   RotateWay_ToolButton.SetIcon( Bitmap( way_rotate_XPM ) );
   RotateWay_ToolButton.SetScaledFixedSize( 19, 19 );
   RotateWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RotateWay_ToolButton.SetToolTip( "Rotate Way" );
   RotateWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   SetWay_ToolButton.SetIcon( Bitmap( way_set_XPM ) );
   SetWay_ToolButton.SetScaledFixedSize( 19, 19 );
   SetWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SetWay_ToolButton.SetToolTip( "Set Way" );
   SetWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   AddWay_ToolButton.SetIcon( Bitmap( way_add_XPM ) );
   AddWay_ToolButton.SetScaledFixedSize( 19, 19 );
   AddWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   AddWay_ToolButton.SetToolTip( "Add Way" );
   AddWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   DeleteWay_ToolButton.SetIcon( Bitmap( way_delete_XPM ) );
   DeleteWay_ToolButton.SetScaledFixedSize( 19, 19 );
   DeleteWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DeleteWay_ToolButton.SetToolTip( "Delete Way" );
   DeleteWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   StructureActions1_Sizer.SetSpacing( 4 );
   StructureActions1_Sizer.AddUnscaledSpacing( labelWidth2 + ui4 );
   StructureActions1_Sizer.Add( UndoStructure_ToolButton );
   StructureActions1_Sizer.AddStretch();
   StructureActions1_Sizer.Add( InvertWay_ToolButton );
   StructureActions1_Sizer.Add( RotateWay_ToolButton );
   StructureActions1_Sizer.Add( SetWay_ToolButton );
   StructureActions1_Sizer.AddStretch();
   StructureActions1_Sizer.Add( AddWay_ToolButton );
   StructureActions1_Sizer.Add( DeleteWay_ToolButton );

   //

   RedoStructure_ToolButton.SetIcon( Bitmap( redo_structure_XPM ) );
   RedoStructure_ToolButton.SetScaledFixedSize( 19, 19 );
   RedoStructure_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RedoStructure_ToolButton.SetToolTip( "Redo" );
   RedoStructure_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   CircularWay_ToolButton.SetIcon( Bitmap( way_circular_XPM ) );
   CircularWay_ToolButton.SetScaledFixedSize( 19, 19 );
   CircularWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   CircularWay_ToolButton.SetToolTip( "Circular Structure" );
   CircularWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   DiamondWay_ToolButton.SetIcon( Bitmap( way_diamond_XPM ) );
   DiamondWay_ToolButton.SetScaledFixedSize( 19, 19 );
   DiamondWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DiamondWay_ToolButton.SetToolTip( "Diamond Structure" );
   DiamondWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   ClearWay_ToolButton.SetIcon( Bitmap( way_clear_XPM ) );
   ClearWay_ToolButton.SetScaledFixedSize( 19, 19 );
   ClearWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ClearWay_ToolButton.SetToolTip( "Clear Way" );
   ClearWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   SetAllWays_ToolButton.SetIcon( Bitmap( way_set_all_XPM ) );
   SetAllWays_ToolButton.SetScaledFixedSize( 19, 19 );
   SetAllWays_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SetAllWays_ToolButton.SetToolTip( "Set All" );
   SetAllWays_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   ClearAllWays_ToolButton.SetIcon( Bitmap( way_clear_all_XPM ) );
   ClearAllWays_ToolButton.SetScaledFixedSize( 19, 19 );
   ClearAllWays_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ClearAllWays_ToolButton.SetToolTip( "Clear All" );
   ClearAllWays_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   StructureActions2_Sizer.SetSpacing( 4 );
   StructureActions2_Sizer.AddUnscaledSpacing( labelWidth2 + ui4 );
   StructureActions2_Sizer.Add( RedoStructure_ToolButton );
   StructureActions2_Sizer.AddStretch();
   StructureActions2_Sizer.Add( CircularWay_ToolButton );
   StructureActions2_Sizer.Add( DiamondWay_ToolButton );
   StructureActions2_Sizer.Add( ClearWay_ToolButton );
   StructureActions2_Sizer.AddStretch();
   StructureActions2_Sizer.Add( SetAllWays_ToolButton );
   StructureActions2_Sizer.Add( ClearAllWays_ToolButton );

   //

   ResetStructure_ToolButton.SetIcon( Bitmap( reset_structure_XPM ) );
   ResetStructure_ToolButton.SetScaledFixedSize( 19, 19 );
   ResetStructure_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ResetStructure_ToolButton.SetToolTip( "Reset Structure" );
   ResetStructure_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   OrthogonalWay_ToolButton.SetIcon( Bitmap( way_orthogonal_XPM ) );
   OrthogonalWay_ToolButton.SetScaledFixedSize( 19, 19 );
   OrthogonalWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   OrthogonalWay_ToolButton.SetToolTip( "Orthogonal Structure" );
   OrthogonalWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   DiagonalWay_ToolButton.SetIcon( Bitmap( way_diagonal_XPM ) );
   DiagonalWay_ToolButton.SetScaledFixedSize( 19, 19 );
   DiagonalWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DiagonalWay_ToolButton.SetToolTip( "Diagonal Structure" );
   DiagonalWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   StoreWay_ToolButton.SetIcon( Bitmap( way_store_XPM ) );
   StoreWay_ToolButton.SetScaledFixedSize( 19, 19 );
   StoreWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   StoreWay_ToolButton.SetToolTip( "Store Way" );
   StoreWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );

   RestoreWay_ToolButton.SetIcon( Bitmap( way_restore_XPM ) );
   RestoreWay_ToolButton.SetScaledFixedSize( 19, 19 );
   RestoreWay_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RestoreWay_ToolButton.SetToolTip( "Restore Way" );
   RestoreWay_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__WayAction_ButtonClick, w );
   RestoreWay_ToolButton.Disable();

   StructureActions3_Sizer.SetSpacing( 4 );
   StructureActions3_Sizer.AddUnscaledSpacing( labelWidth2 + ui4 );
   StructureActions3_Sizer.Add( ResetStructure_ToolButton );
   StructureActions3_Sizer.AddStretch();
   StructureActions3_Sizer.Add( OrthogonalWay_ToolButton );
   StructureActions3_Sizer.Add( DiagonalWay_ToolButton );
   StructureActions3_Sizer.AddSpacing( 19 + 4 );
   StructureActions3_Sizer.AddStretch();
   StructureActions3_Sizer.Add( StoreWay_ToolButton );
   StructureActions3_Sizer.Add( RestoreWay_ToolButton );

   //

   StructureData_Sizer.SetSpacing( 4 );
   StructureData_Sizer.Add( StructureSize_Sizer );
   StructureData_Sizer.Add( StructureWay_Sizer );
   StructureData_Sizer.AddStretch();
   StructureData_Sizer.Add( StructureActions1_Sizer );
   StructureData_Sizer.Add( StructureActions2_Sizer );
   StructureData_Sizer.Add( StructureActions3_Sizer );

   //

   StructureTop_Sizer.SetSpacing( 4 );
   StructureTop_Sizer.Add( StructureEditor_Sizer );
   StructureTop_Sizer.Add( StructureData_Sizer );

   //

   PaintSet_ToolButton.SetIcon( Bitmap( paint_set_XPM ) );
   PaintSet_ToolButton.SetScaledFixedSize( 19, 19 );
   PaintSet_ToolButton.SetCheckable();
   PaintSet_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PaintSet_ToolButton.SetToolTip( "Paint mode: Set element" );
   PaintSet_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__PaintMode_ButtonClick, w );

   PaintClear_ToolButton.SetIcon( Bitmap( paint_clear_XPM ) );
   PaintClear_ToolButton.SetScaledFixedSize( 19, 19 );
   PaintClear_ToolButton.SetCheckable();
   PaintClear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PaintClear_ToolButton.SetToolTip( "Paint mode: Clear element" );
   PaintClear_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__PaintMode_ButtonClick, w );

   ShowAllWays_ToolButton.SetIcon( Bitmap( show_all_ways_XPM ) );
   ShowAllWays_ToolButton.SetScaledFixedSize( 19, 19 );
   ShowAllWays_ToolButton.SetCheckable();
   ShowAllWays_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowAllWays_ToolButton.SetToolTip( "Show all ways" );
   ShowAllWays_ToolButton.OnClick( (ToolButton::click_event_handler)&MorphologicalTransformationInterface::__DrawingOption_ButtonClick, w );

   StructureInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   ManageStructures_PushButton.SetText( "Manage" );
   ManageStructures_PushButton.SetToolTip( "Open Structure Manager dialog" );
   ManageStructures_PushButton.OnClick( (PushButton::click_event_handler)&MorphologicalTransformationInterface::__ManageStructures_ButtonClick, w );

   StructureMiddle_Sizer.SetSpacing( 4 );
   StructureMiddle_Sizer.Add( PaintSet_ToolButton );
   StructureMiddle_Sizer.Add( PaintClear_ToolButton );
   StructureMiddle_Sizer.Add( ShowAllWays_ToolButton );
   StructureMiddle_Sizer.Add( StructureInfo_Label, 100 );
   StructureMiddle_Sizer.Add( ManageStructures_PushButton );

   //

   StructureName_Edit.OnEditCompleted( (Edit::edit_event_handler)&MorphologicalTransformationInterface::__StructureName_EditCompleted, w );
   StructureName_Edit.OnGetFocus( (Control::event_handler)&MorphologicalTransformationInterface::__StructureName_GetFocus, w );

   //

   Structure_Sizer.SetSpacing( 4 );
   Structure_Sizer.Add( StructureTop_Sizer );
   Structure_Sizer.Add( StructureMiddle_Sizer );
   Structure_Sizer.Add( StructureName_Edit );

   Structure_Control.SetSizer( Structure_Sizer );

   //

   Thresholds_SectionBar.SetTitle( "Thresholds" );
   Thresholds_SectionBar.SetSection( Thresholds_Control );

   //

   LowThreshold_NumericControl.label.SetText( "Low:" );
   LowThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   LowThreshold_NumericControl.slider.SetRange( 0, 100 );
   LowThreshold_NumericControl.SetReal();
   LowThreshold_NumericControl.SetRange( 0, 1 );
   LowThreshold_NumericControl.SetPrecision( 6 );
   LowThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&MorphologicalTransformationInterface::__Threshold_ValueUpdated, w );

   HighThreshold_NumericControl.label.SetText( "High:" );
   HighThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   HighThreshold_NumericControl.slider.SetRange( 0, 100 );
   HighThreshold_NumericControl.SetReal();
   HighThreshold_NumericControl.SetRange( 0, 1 );
   HighThreshold_NumericControl.SetPrecision( 6 );
   HighThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&MorphologicalTransformationInterface::__Threshold_ValueUpdated, w );

   Thresholds_Sizer.SetSpacing( 4 );
   Thresholds_Sizer.Add( LowThreshold_NumericControl );
   Thresholds_Sizer.Add( HighThreshold_NumericControl );

   Thresholds_Control.SetSizer( Thresholds_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Filter_SectionBar );
   Global_Sizer.Add( Filter_Control );
   Global_Sizer.Add( Structure_SectionBar );
   Global_Sizer.Add( Structure_Control );
   Global_Sizer.Add( Thresholds_SectionBar );
   Global_Sizer.Add( Thresholds_Control );

   //

   Thresholds_Control.Hide();

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   //

   int editWidth1 = Interlacing_SpinBox.Width();
   Iterations_SpinBox.SetMinWidth( editWidth1 );
   Amount_NumericControl.edit.SetMinWidth( editWidth1 );
   Selection_NumericControl.edit.SetMinWidth( editWidth1 );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationInterface.cpp - Released 2017-08-01T14:26:58Z
