//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 01.00.00.0222
// ----------------------------------------------------------------------------
// AnnotationInterface.cpp - Released 2018-11-01T11:07:21Z
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

#include "AnnotationInterface.h"
#include "AnnotationProcess.h"
#include "AnnotationParameters.h"
#include "AnnotationRenderer.h"

#include <pcl/Graphics.h>
#include <pcl/ColorDialog.h>
#include <pcl/Settings.h>
#include <pcl/MessageBox.h>


namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationInterface* TheAnnotationInterface = 0;

// ----------------------------------------------------------------------------

#include "AnnotationIcon.xpm"

#include "cursors/move_all.xpm"

// ----------------------------------------------------------------------------

AnnotationInterface::AnnotationInterface() :
   ProcessInterface(),
   instance( TheAnnotationProcess ),
   view( 0 ),
   annotationPlaced( false ),
   leaderPlaced( false ),
   dragging( DraggingType::None ),
   lastX ( -1 ),
   lastY ( -1 ),
   relPosX( 0 ),
   relPosY( 0 ),
   annotationBmp( Bitmap::Null() ),
   screenBmp( Bitmap::Null() ),
   GUI( 0 )
{
   TheAnnotationInterface = this;
}

AnnotationInterface::~AnnotationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString AnnotationInterface::Id() const
{
   return "Annotation";
}

MetaProcess* AnnotationInterface::Process() const
{
   return TheAnnotationProcess;
}

const char** AnnotationInterface::IconImageXPM() const
{
   return AnnotationIcon_XPM;
}

InterfaceFeatures AnnotationInterface::Features() const
{
   return InterfaceFeature::DefaultDynamic;
}

bool AnnotationInterface::IsDynamicInterface() const
{
   return true;
}

void AnnotationInterface::ExitDynamicMode()
{
   // Forget current target view.
   if ( view != 0 )
   {
      // Repaint view to hide the dynamic annotation
      UpdateView();
      delete view, view = 0;
   }

   ClearBitmaps();
   annotationPlaced = false;
   leaderPlaced = false;
}

void AnnotationInterface::Execute()
{
   // can't execute if annotation is not placed in a valid view
   if (view == 0 || !annotationPlaced)
   {
      MessageBox mb("Can't execute. Use left mouse button to place annotation on the image first.",
         "Not executed",
         StdIcon::Information);
      mb.Execute();
      return;
   }

   // check mask. if enabled, offer user to temporarily disable it
   bool disableMask = false;
   if ( view->Window().IsMaskEnabled() && !view->Window().Mask().IsNull() )
   {
      MessageBox mb("The view has a mask enabled. Mask will probably interfere with the annotation rendering.<br/>"
         "Do you want to temporarily disable the mask?",
         "Mask Enabled",
         StdIcon::Question,
         StdButton::Yes, StdButton::No, StdButton::Cancel, 0, 2);
      MessageBox::std_button result = mb.Execute();
      if (result == StdButton::Cancel) return;
      disableMask = result == StdButton::Yes;
   }

   // Obtain local working references to the target view and window.
   View v = *view;
   ImageWindow w = v.Window();

   // Reset reference to the target view in the dynamic interface. This
   // prevents inconsistent behavior during execution.
   delete view, view = 0;
   ClearBitmaps();

   // Since active dynamic targets cannot be modified, we have to remove our
   // target view from the dynamic targets set before attempting to process.
   v.RemoveFromDynamicTargets();

   // Ensure that our target view is selected as the current view.
   w.BringToFront();
   w.SelectView( v );

   // Disable mask if required
   if ( disableMask )
      w.EnableMask( false );

   // Execute the instance on the target window.
   instance.LaunchOn( w );

   // Re-enable mask if required
   if ( disableMask )
      w.EnableMask( true );

   // keep parameters but reset state
   annotationPlaced = false;
   leaderPlaced = false;
}

void AnnotationInterface::ResetInstance()
{
   AnnotationInstance defaultInstance( TheAnnotationProcess );
   ImportProcess( defaultInstance );
}

bool AnnotationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Annotation" );
      UpdateControls();
   }

   dynamic = true;
   return &P == TheAnnotationProcess;
}

ProcessImplementation* AnnotationInterface::NewProcess() const
{
   return new AnnotationInstance( instance );
}

bool AnnotationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const AnnotationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an Annotation instance.";
   return false;
}

bool AnnotationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool AnnotationInterface::ImportProcess( const ProcessImplementation& p )
{
   const AnnotationInstance* i = dynamic_cast<const AnnotationInstance*>( &p );
   if ( i == nullptr )
      throw Error( "Not an Annotation instance." );

   if ( view == nullptr )
   {
      ImageWindow w = ImageWindow::ActiveWindow();
      if ( w.IsNull() )
      {
         throw Error( "The Annotation interface requires an active image window to import a process instance." );
         return false;
      }
      view = new View( w.MainView() );
      view->AddToDynamicTargets();
   }

   ClearBitmaps();

   instance.Assign( *i );

   annotationPlaced = false;
   leaderPlaced = false;

   if ( view->Window().CurrentView() != *view )
      view->Window().SelectView( *view );

   UpdateControls();

   return true;
}

// ----------------------------------------------------------------------------

IsoString AnnotationInterface::SettingsKey() const
{
   return "Annotation";
}

// ----------------------------------------------------------------------------

void AnnotationInterface::LoadSettings()
{
   Settings::Read( SettingsKey() + "Text", instance.annotationText );

   int boolValue = instance.annotationShowLeader;
   Settings::Read( SettingsKey() + "ShowLeader", boolValue );
   instance.annotationShowLeader = boolValue != 0;

   Settings::Read( SettingsKey() + "Font", instance.annotationFont );

   int fontSize = instance.annotationFontSize;
   Settings::Read( SettingsKey() + "FontSize", fontSize );
   instance.annotationFontSize = (uint8)fontSize;

   boolValue = instance.annotationFontBold;
   Settings::Read( SettingsKey() + "FontBold", boolValue );
   instance.annotationFontBold = boolValue != 0;

   boolValue = instance.annotationFontItalic;
   Settings::Read( SettingsKey() + "FontItalic", boolValue );
   instance.annotationFontItalic = boolValue != 0;

   boolValue = instance.annotationFontUnderline;
   Settings::Read( SettingsKey() + "FontUnderline", boolValue );
   instance.annotationFontUnderline = boolValue != 0;

   boolValue = instance.annotationFontShadow;
   Settings::Read( SettingsKey() + "FontShadow", boolValue );
   instance.annotationFontShadow = boolValue;

   Settings::Read( SettingsKey() + "FontColor", instance.annotationColor );

   int opacity = instance.annotationOpacity;
   Settings::Read( SettingsKey() + "Opacity", opacity );
   instance.annotationOpacity = (uint8)opacity;

   UpdateControls();
}

// ----------------------------------------------------------------------------

void AnnotationInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "Text", instance.annotationText );
   Settings::Write( SettingsKey() + "ShowLeader", (int)instance.annotationShowLeader );
   Settings::Write( SettingsKey() + "Font", instance.annotationFont );
   Settings::Write( SettingsKey() + "FontSize", (int)instance.annotationFontSize );
   Settings::Write( SettingsKey() + "FontBold", (int)instance.annotationFontBold );
   Settings::Write( SettingsKey() + "FontItalic", (int)instance.annotationFontItalic );
   Settings::Write( SettingsKey() + "FontUnderline", (int)instance.annotationFontUnderline );
   Settings::Write( SettingsKey() + "FontShadow", (int)instance.annotationFontShadow );
   Settings::Write( SettingsKey() + "FontColor", instance.annotationColor );
   Settings::Write( SettingsKey() + "Opacity", instance.annotationOpacity );
}

// ----------------------------------------------------------------------------

void AnnotationInterface::DynamicMousePress( View& v, const DPoint& p, int button, unsigned buttons, unsigned modifiers )
{
   // we only use left mouse button
   if ( button != MouseButton::Left )
      return;

   if (view == 0)
   {
      // can not run on previews
      if ( !v.IsMainView() )
         throw Error( "Annotation cannot run on previews. Please select a main view." );

      view = new View( v );
   }

   // only handle events in our active view
   if (v != *view)
      return;

   // get view image window
   ImageWindow w = view->Window();

   // and image coordinates of the click point
   int imageX = RoundI(p.x);
   int imageY = RoundI(p.y);

   // if annotation is not yet placed, place it now
   if (!annotationPlaced)
   {
      // set annotation position
      instance.annotationPositionX = imageX;
      instance.annotationPositionY = imageY;
      annotationPlaced = true;

      // place leader if needed
      if (instance.annotationShowLeader)
      {
         PlaceLeaderDefault();
      }

      // update annotation rectangle
      UpdateAnnotationRect( true );

      // redraw dynamic view
      UpdateView();

      // start dragging mode until user release the mouse
      dragging = DraggingType::Text;
      w.SetDynamicCursor(move_all_XPM, 10, 10);
   }
   // if the annotation is already placed
   else
   {
      // if mouse is pressed on annotation text rectangle
      // let's start dragging
      if (textRect.Includes(imageX, imageY))
      {
         // with Ctrl, both text and leader are dragged simulaneously
         if (modifiers & KeyModifier::Control)
         {
            dragging = DraggingType::Both;
         }
         // otherwise, just text is dragged
         else
         {
            dragging = DraggingType::Text;
         }
      }
      // if mouse is pressed on annotation leader rectangle and leader is visible
      // let's start dragging
      else if (instance.annotationShowLeader && leaderRect.Includes(imageX, imageY))
      {
         // with Ctrl, both text and leader are dragged simulaneously
         if (modifiers & KeyModifier::Control)
         {
            dragging = DraggingType::Both;
         }
         // otherwise, just leader endpoint is dragged
         else
         {
            dragging = DraggingType::Leader;
         }
      }
   }

   // if any dragging is started, remember current point
   if (dragging != DraggingType::None)
   {
      lastX = imageX;
      lastY = imageY;
   }
}

void AnnotationInterface::DynamicMouseMove( View& v, const DPoint& p, unsigned buttons, unsigned modifiers )
{
   // mouse move is processed only on active view
   if (view == 0 || v != *view)
      return;

   // get view image window
   ImageWindow w = view->Window();

   // and image coordinates of the mouse position
   int imageX = RoundI(p.x);
   int imageY = RoundI(p.y);

   // set cursor to dragging cursor if mouse is inside one of the grip rectangles
   if (annotationPlaced && textRect.Includes(imageX, imageY))
      w.SetDynamicCursor(move_all_XPM, 10, 10);
   else if (leaderPlaced && instance.annotationShowLeader && leaderRect.Includes(imageX, imageY))
      w.SetDynamicCursor(move_all_XPM, 10, 10);
   else // otherwise, reset cursor
      w.ResetDynamicCursor();

   // if we are currently dragging something
   if ( dragging != DraggingType::None )
   {
      // compute delta
      int deltaX = imageX-lastX;
      int deltaY = imageY-lastY;

      // update text position if needed
      if (dragging == DraggingType::Text || dragging == DraggingType::Both)
      {
         instance.annotationPositionX += deltaX;
         instance.annotationPositionY += deltaY;
      }

      // update leader endpoint position if needed
      if (dragging == DraggingType::Leader || dragging == DraggingType::Both)
      {
         instance.annotationLeaderPositionX += deltaX;
         instance.annotationLeaderPositionY += deltaY;
      }

      // update annotation rectangle
      UpdateAnnotationRect();

      // redraw dynamic view
      UpdateView();

      // remember current point
      lastX = imageX;
      lastY = imageY;
   }
}

void AnnotationInterface::DynamicMouseRelease( View& v, const DPoint&, int button, unsigned buttons, unsigned modifiers )
{
   // mouse release is processed only on active view
   if (view == 0 || v != *view)
      return;

   // stop any dragging
   if (dragging != DraggingType::None)
   {
      dragging = DraggingType::None;
      lastX = lastY = -1;
   }
}

bool AnnotationInterface::RequiresDynamicUpdate( const View& v, const DRect& r ) const
{
   // updates are required only in active view, if the annotation is already placed
   if ( view == 0 || v != *view || !annotationPlaced )
      return false;
   // and only if update rectangle intersects with annotation rectangle
   return totalRect.Intersects( r );
}

void AnnotationInterface::DynamicPaint( const View& v, VectorGraphics& g, const DRect& r ) const
{
   // we need valid view and annotation must be placed
   if ( view == 0 || v != *view || !annotationPlaced )
      return;

   // are we painting to rect with annotation?
   if ( !totalRect.Intersects( r ) )
      return;

   // get the image window
   ImageWindow w = view->Window();

   // render annotation to bitmap if needed
   if ( annotationBmp.IsNull() )
   {
      relPosX = 0;
      relPosY = 0;
      annotationBmp = AnnotationRenderer::CreateAnnotationBitmap( instance, relPosX, relPosY, true );
      screenBmp = Bitmap::Null();
   }

   // compute viewport coordinates of annotation text
   int posX = instance.annotationPositionX - relPosX;
   int posY = instance.annotationPositionY - relPosY;
   w.ImageToViewport( posX, posY );

   // compute zoom factor
   int zoomFactor = w.ZoomFactor();
   double z = (zoomFactor < 0) ? -1.0/zoomFactor : double( zoomFactor );

   // draw bitmap, scaled according to zoom factor
   Rect zr( RoundI( annotationBmp.Width()*z ), RoundI( annotationBmp.Height()*z ) );
   if ( screenBmp.IsNull() || zr != screenBmp.Bounds() )
      if ( zoomFactor < 0 )
         screenBmp = annotationBmp.Scaled( z );
      else
         screenBmp = annotationBmp;

   if ( zoomFactor < 0 )
      g.DrawBitmap( posX, posY, screenBmp );
   else
      g.DrawScaledBitmap( zr.MovedTo( posX, posY ), screenBmp );
}

// ----------------------------------------------------------------------------

void AnnotationInterface::UpdateControls()
{
   // synchronize controls with current instance
   GUI->AnnotationText_Edit.SetText( instance.annotationText );
   GUI->AnnotationShowLeader_CheckBox.SetChecked( instance.annotationShowLeader );
   GUI->AnnotationFont_ComboBox.SetCurrentFont( instance.annotationFont );
   GUI->AnnotationFontSize_SpinBox.SetValue( instance.annotationFontSize );
   GUI->AnnotationFontBold_CheckBox.SetChecked( instance.annotationFontBold );
   GUI->AnnotationFontItalic_CheckBox.SetChecked( instance.annotationFontItalic );
   GUI->AnnotationFontUnderline_CheckBox.SetChecked( instance.annotationFontUnderline );
   GUI->AnnotationFontShadow_CheckBox.SetChecked( instance.annotationFontShadow );
   GUI->AnnotationColor_ComboBox.SetCurrentColor( instance.annotationColor );
   GUI->AnnotationColor_CustomColorSample.Update();
   GUI->AnnotationOpacity_NumericControl.SetValue( instance.annotationOpacity );
}

// ----------------------------------------------------------------------------

void AnnotationInterface::ClearBitmaps()
{
   annotationBmp = Bitmap::Null();
   screenBmp = Bitmap::Null();
}

void AnnotationInterface::UpdateView()
{
   // update current invalidate rect of the view
   if ( view != 0 && annotationPlaced )
   {
      annotationBmp = Bitmap::Null();
      ImageWindow w = view->Window();
      w.UpdateImageRect( invalidateRect );
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::UpdateAnnotationRect(bool reset)
{
   // compute current image rectangle for annotation text and leader
   // also compute its union and rectangle for invalidation (union of old and new rectangles)
   if (view != 0)
   {
      // flag whether leader is to be shown
      bool showLeader = leaderPlaced && instance.annotationShowLeader;

      // get view image window
      ImageWindow w = view->Window();

      // store actual rectangles - we'll need them for invalidate rectangle
      Rect oldRect( totalRect );

      // get annotation font
      pcl::Font f = AnnotationRenderer::GetAnnotationFont( instance );

      // compute current text rectangle
      textRect = Rect( f.Width( instance.annotationText ) + 1, f.Height() + 1 );
      textRect.MoveBy( instance.annotationPositionX, instance.annotationPositionY );

      // compute leader rectangle if needed
      if ( showLeader )
      {
         leaderRect = Rect( 11, 11 );
         leaderRect.MoveBy( instance.annotationLeaderPositionX - 5, instance.annotationLeaderPositionY - 5 );
      }

      // compute total rectangle (text and leader) using annotation renderer
      int relPosX, relPosY, dummy3, dummy4;
      totalRect = AnnotationRenderer::GetBoundingRectangle(instance, relPosX, relPosY, dummy3, dummy4);
      totalRect.MoveBy(instance.annotationPositionX-relPosX, instance.annotationPositionY-relPosY);

      // if oldRect is not valid, don't take it into account when computing invalidate rectangle
      if ( reset )
      {
         invalidateRect = totalRect;
      }
      // otherwise make invalidate rectangle union of old and new rectangles
      else
      {
         invalidateRect = totalRect.Union( oldRect );
      }
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::PlaceLeaderDefault()
{
   // place leader at default position

   // get the current annotation font
   pcl::Font f = AnnotationRenderer::GetAnnotationFont( instance );

   // scale by actual font height
   int textWidth = f.Width( instance.annotationText );
   int textHeight = f.Height();

   // place leader X position to the left of the text
   instance.annotationLeaderPositionX = instance.annotationPositionX - 5*textHeight;

   // if outside of image, place it to the right
   if (instance.annotationLeaderPositionX < 0)
   {
      instance.annotationLeaderPositionX = instance.annotationPositionX + textWidth + 5*textHeight;
   }

   // place leader Y position to the top of the text
   instance.annotationLeaderPositionY = instance.annotationPositionY - 3*textHeight;
   if (instance.annotationLeaderPositionY < 0)
   {
      instance.annotationLeaderPositionY = instance.annotationPositionY + 3*textHeight;
   }

   leaderPlaced = true;
}

// ----------------------------------------------------------------------------

void AnnotationInterface::__EditTextUpdated( Edit& sender, const String &text )
{
   if ( sender == GUI->AnnotationText_Edit )
   {
      instance.annotationText = text;
      UpdateAnnotationRect();
      UpdateView();
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::__FontComboBoxFontSelected( FontComboBox &sender, const String &font )
{
   if ( sender == GUI->AnnotationFont_ComboBox )
   {
      instance.annotationFont = font;
      UpdateAnnotationRect();
      UpdateView();
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::__FontSizeSpinBoxValueUpdated( SpinBox &sender, int value )
{
   if (sender == GUI->AnnotationFontSize_SpinBox )
   {
      instance.annotationFontSize = (uint8)value;
      UpdateAnnotationRect();
      UpdateView();
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::__FontOptionCheckBoxChecked( Button &sender, Button::check_state state )
{
   if ( sender == GUI->AnnotationFontBold_CheckBox )
   {
      instance.annotationFontBold = state == CheckState::Checked ? 1 : 0;
      UpdateAnnotationRect();
      UpdateView();
   }
   else if ( sender == GUI->AnnotationFontItalic_CheckBox )
   {
      instance.annotationFontItalic = state == CheckState::Checked ? 1 : 0;
      UpdateAnnotationRect();
      UpdateView();
   }
   else if ( sender == GUI->AnnotationFontUnderline_CheckBox )
   {
      instance.annotationFontUnderline = state == CheckState::Checked ? 1 : 0;
      UpdateView();
   }
   else if ( sender == GUI->AnnotationFontShadow_CheckBox )
   {
      instance.annotationFontShadow = state == CheckState::Checked ? 1 : 0;
      UpdateView();
   }
   else if ( sender == GUI->AnnotationShowLeader_CheckBox )
   {
      // set show leader flag
      instance.annotationShowLeader = state == CheckState::Checked ? 1 : 0;

      // if not yet placed, place it to default position
      if (instance.annotationShowLeader &&
         annotationPlaced && !leaderPlaced)
      {
         PlaceLeaderDefault();
      }

      // update rectangles
      UpdateAnnotationRect();

      // redraw view
      UpdateView();
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::__ColorComboBoxColorSelected( ColorComboBox &sender, RGBA color )
{
   if ( sender == GUI->AnnotationColor_ComboBox )
   {
      instance.annotationColor = color;
      GUI->AnnotationColor_CustomColorSample.Update();
      UpdateView();
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::__ColorSample_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   Graphics g( sender );
   g.SetBrush( instance.annotationColor );
   g.SetPen( RGBAColor( 0, 0, 0 ) );
   g.DrawRect( sender.BoundsRect() );
}

void AnnotationInterface::__ColorSample_MouseRelease(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( sender.IsUnderMouse() )
   {
      // launch custom color selection dialog
      SimpleColorDialog dlg( &instance.annotationColor );
      if ( dlg.Execute() == StdDialogCode::Ok )
      {
         GUI->AnnotationColor_ComboBox.SetCurrentColor( instance.annotationColor );
         GUI->AnnotationColor_CustomColorSample.Update();
         UpdateView();
      }
   }
}

// ----------------------------------------------------------------------------

void AnnotationInterface::__OpacityUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->AnnotationOpacity_NumericControl )
   {
      instance.annotationOpacity = (uint8)value;
      UpdateView();
   }
}

// ----------------------------------------------------------------------------

AnnotationInterface::GUIData::GUIData( AnnotationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Opacity:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 30 ) );

   // AnnotationText

   AnnotationText_Label.SetText( "Text:" );
   AnnotationText_Label.SetMinWidth( labelWidth1 );
   AnnotationText_Label.SetToolTip( "<p>The annotation text.</p>" );
   AnnotationText_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   AnnotationText_Edit.SetMinWidth( editWidth1 );
   AnnotationText_Edit.SetToolTip( "<p>The annotation text.</p>" );
   AnnotationText_Edit.OnTextUpdated( (Edit::text_event_handler)&AnnotationInterface::__EditTextUpdated, w );

   AnnotationShowLeader_CheckBox.SetText("Show Leader");
   AnnotationShowLeader_CheckBox.SetToolTip( "<p>Shows annotation leader - line leading from annotation to object on image.</p>" );
   AnnotationShowLeader_CheckBox.OnCheck( (Button::check_event_handler)&AnnotationInterface::__FontOptionCheckBoxChecked, w );

   AnnotationText_Sizer.SetSpacing( 4 );
   AnnotationText_Sizer.SetMargin( 8 );
   AnnotationText_Sizer.Add( AnnotationText_Label );
   AnnotationText_Sizer.Add( AnnotationText_Edit, 100 );
   AnnotationText_Sizer.Add( AnnotationShowLeader_CheckBox );

   Content_GroupBox.SetTitle( "Content" );
   Content_GroupBox.SetSizer( AnnotationText_Sizer );
   Content_GroupBox.AdjustToContents();

   // AnnotationFont

   AnnotationFont_Label.SetText("Font:");
   AnnotationFont_Label.SetMinWidth( labelWidth1 );
   AnnotationFont_Label.SetToolTip( "<p>The annotation font.</p>" );
   AnnotationFont_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   AnnotationFont_ComboBox.SetMinWidth( editWidth1 );
   AnnotationFont_ComboBox.SetToolTip( "<p>The annotation font.</p>" );
   AnnotationFont_ComboBox.OnFontSelected( (FontComboBox::font_event_handler)&AnnotationInterface::__FontComboBoxFontSelected, w );

   AnnotationFontSize_Label.SetText("Size (pt):");
   AnnotationFontSize_Label.SetToolTip("<p>The annotation font size.</p>");
   AnnotationFontSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   AnnotationFontSize_SpinBox.SetMinWidth(fnt.Width( String( "255XX" ) ));
   AnnotationFontSize_SpinBox.SetMinValue(1);
   AnnotationFontSize_SpinBox.SetMaxValue(255);
   AnnotationFontSize_SpinBox.SetToolTip("<p>The annotation font size.</p>");
   AnnotationFontSize_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&AnnotationInterface::__FontSizeSpinBoxValueUpdated, w );

   AnnotationFont_Sizer.SetSpacing( 4 );
   AnnotationFont_Sizer.Add( AnnotationFont_Label );
   AnnotationFont_Sizer.Add( AnnotationFont_ComboBox, 100 );
   AnnotationFont_Sizer.Add( AnnotationFontSize_Label );
   AnnotationFont_Sizer.Add( AnnotationFontSize_SpinBox );

   // AnnotationFontOptions

   AnnotationFontBold_CheckBox.SetText( "Bold" );
   AnnotationFontBold_CheckBox.SetToolTip("<p>Bold text</p>");
   AnnotationFontBold_CheckBox.OnCheck( (Button::check_event_handler)&AnnotationInterface::__FontOptionCheckBoxChecked, w );
   AnnotationFontItalic_CheckBox.SetText( "Italic" );
   AnnotationFontItalic_CheckBox.SetToolTip("<p>Italic text</p>");
   AnnotationFontItalic_CheckBox.OnCheck( (Button::check_event_handler)&AnnotationInterface::__FontOptionCheckBoxChecked, w );
   AnnotationFontUnderline_CheckBox.SetText( "Underline" );
   AnnotationFontUnderline_CheckBox.SetToolTip("<p>Underlined text</p>");
   AnnotationFontUnderline_CheckBox.OnCheck( (Button::check_event_handler)&AnnotationInterface::__FontOptionCheckBoxChecked, w );
   AnnotationFontShadow_CheckBox.SetText( "Shadow" );
   AnnotationFontShadow_CheckBox.SetToolTip("<p>Text drops 1px-wide black shadow.</p>");
   AnnotationFontShadow_CheckBox.OnCheck( (Button::check_event_handler)&AnnotationInterface::__FontOptionCheckBoxChecked, w );

   AnnotationFontOptions_Sizer.SetSpacing( 4 );
   AnnotationFontOptions_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   AnnotationFontOptions_Sizer.Add( AnnotationFontBold_CheckBox );
   AnnotationFontOptions_Sizer.Add( AnnotationFontItalic_CheckBox );
   AnnotationFontOptions_Sizer.Add( AnnotationFontUnderline_CheckBox );
   AnnotationFontOptions_Sizer.Add( AnnotationFontShadow_CheckBox );
   AnnotationFontOptions_Sizer.AddStretch( 100 );

   // AnnotationColor

   AnnotationColor_Label.SetText("Color:");
   AnnotationColor_Label.SetMinWidth( labelWidth1 );
   AnnotationColor_Label.SetToolTip( "<p>The annotation color.</p>" );
   AnnotationColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   AnnotationColor_ComboBox.SetMinWidth( editWidth1 );
   AnnotationColor_ComboBox.SetToolTip( "<p>The annotation color.</p>" );
   AnnotationColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&AnnotationInterface::__ColorComboBoxColorSelected, w );

   AnnotationColor_CustomColorSample.SetScaledMinWidth( 30 );
   AnnotationColor_CustomColorSample.SetToolTip( "<p>Click to define custom color.</p>" );
   AnnotationColor_CustomColorSample.SetCursor( StdCursor::UpArrow );
   AnnotationColor_CustomColorSample.OnPaint( (Control::paint_event_handler)&AnnotationInterface::__ColorSample_Paint, w );
   AnnotationColor_CustomColorSample.OnMouseRelease( (Control::mouse_button_event_handler)&AnnotationInterface::__ColorSample_MouseRelease, w );

   AnnotationColor_Sizer.SetSpacing( 4 );
   AnnotationColor_Sizer.Add( AnnotationColor_Label );
   AnnotationColor_Sizer.Add( AnnotationColor_ComboBox, 100 );
   AnnotationColor_Sizer.Add( AnnotationColor_CustomColorSample );

   AnnotationOpacity_NumericControl.label.SetText("Opacity:");
   AnnotationOpacity_NumericControl.label.SetMinWidth( labelWidth1 );
   AnnotationOpacity_NumericControl.slider.SetRange( 0, 255 );
   AnnotationOpacity_NumericControl.SetInteger();
   AnnotationOpacity_NumericControl.SetRange( 0, 255 );
   AnnotationOpacity_NumericControl.SetToolTip("<p>Sets opacity of the annotation text and leader. Lower values mean more transparency.</p>");
   AnnotationOpacity_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AnnotationInterface::__OpacityUpdated, w );

   Font_Sizer.SetSpacing( 6 );
   Font_Sizer.SetMargin( 8 );
   Font_Sizer.Add( AnnotationFont_Sizer );
   Font_Sizer.Add( AnnotationFontOptions_Sizer );
   Font_Sizer.Add( AnnotationColor_Sizer );
   Font_Sizer.Add( AnnotationOpacity_NumericControl );

   Font_GroupBox.SetTitle( "Font" );
   Font_GroupBox.SetSizer( Font_Sizer );
   Font_GroupBox.AdjustToContents();

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Content_GroupBox );
   Global_Sizer.Add( Font_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AnnotationInterface.cpp - Released 2018-11-01T11:07:21Z
