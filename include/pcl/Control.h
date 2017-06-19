//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/Control.h - Released 2017-06-17T10:55:43Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_Control_h
#define __PCL_Control_h

/// \file pcl/Control.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/ButtonCodes.h>
#include <pcl/Color.h>
#include <pcl/Cursor.h>
#include <pcl/Flags.h>
#include <pcl/Font.h>
#include <pcl/KeyCodes.h>
#include <pcl/Rectangle.h>
#include <pcl/Sizer.h>
#include <pcl/UIObject.h>
#include <pcl/UIScaling.h>

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FocusStyle
 * \brief Control focus styles
 *
 * Focus styles refer to the way a control can gain the keyboard focus.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FocusStyle::NoFocus</td>      <td>The control does not accept the keyboard focus</td></tr>
 * <tr><td>FocusStyle::Tab</td>          <td>The control can be focused by pressing the tab key</td></tr>
 * <tr><td>FocusStyle::Click</td>        <td>The control can be focused by clicking on it with the mouse</td></tr>
 * <tr><td>FocusStyle::Wheel</td>        <td>The control can be focused with the mouse wheel</td></tr>
 * <tr><td>FocusStyle::TextListTab</td>  <td>Special mode reserved for Mac OS X</td></tr>
 * </table>
 */
namespace FocusStyle
{
   enum mask_type
   {
      NoFocus     = 0x00,  // The control doesn't accept focus
      Tab         = 0x01,  // Can focus control by pressing the tab key
      Click       = 0x02,  // Can focus control by mouse clicking
      Wheel       = 0x04,  // Can focus control with the mouse wheel
      TextListTab = 0x08   // Mac OS X only
   };
}

/*!
 * A combination of focus styles.
 */
typedef Flags<FocusStyle::mask_type>   FocusStyles;

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

class PCL_CLASS View;

/*!
 * \class Control
 * \brief Client-side interface to a PixInsight %Control object
 *
 * ### TODO: Write a detailed description for %Control.
 */
class PCL_CLASS Control : public UIObject
{
public:

   /*!
    * Constructs a control as a child of \a parent. If no non-null parent is
    * specified, the control will be an independent object not associated (in
    * the hierarchical sense) to any graphical interface element.
    *
    * The second \c uint32 argument is not used; it is reserved for future
    * extensions, and must be zero in current PCL versions.
    */
   Control( Control& parent = Null(), uint32 = 0 );

   /*!
    * Destroys a %Control object.
    */
   virtual ~Control()
   {
   }

   /*!
    * Copy constructor. Copy and move semantics are disabled for UI controls
    * because of client/server parent-children and event handling relations.
    */
   Control( const Control& ) = delete;

   /*!
    * Copy assignment. Copy and move semantics are disabled for UI controls
    * because of client/server parent-children and event handling relations.
    */
   Control& operator =( const Control& ) = delete;

   /*!
    * Move constructor. Copy and move semantics are disabled for UI controls
    * because of client/server parent-children and event handling relations.
    */
   Control( Control&& ) = delete;

   /*!
    * Move assignment. Copy and move semantics are disabled for UI controls
    * because of client/server parent-children and event handling relations.
    */
   Control& operator =( Control&& ) = delete;

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since server-side controls are unique objects, calling this member
    * function has no effect.
    */
   virtual void EnsureUnique()
   {
   }

   /*!
    * Returns a reference to a null %Control instance. A null %Control does not
    * correspond to an existing control in the PixInsight core application.
    */
   static Control& Null();

   /*!
    * Returns the frame rectangle of this control.
    */
   pcl::Rect FrameRect() const;

   /*!
    * Returns the frame width of this control in pixels.
    */
   int FrameWidth() const
   {
      return FrameRect().Width();
   }

   /*!
    * Returns the frame height of this control in pixels.
    */
   int FrameHeight() const
   {
      return FrameRect().Height();
   }

   /*!
    * Returns the client rectangle of this control in parent coordinates.
    */
   pcl::Rect ClientRect() const;

   /*!
    * Returns the width in pixels of the <em>client area</em> of this control.
    */
   int ClientWidth() const
   {
      return ClientRect().Width();
   }

   /*!
    * A convenient synonym for ClientWidth().
    */
   int Width() const
   {
      return ClientWidth();
   }

   /*!
    * Returns the height in pixels of the <em>client area</em> of this control.
    */
   int ClientHeight() const
   {
      return ClientRect().Height();
   }

   /*!
    * A convenient synonym for ClientHeight().
    */
   int Height() const
   {
      return ClientHeight();
   }

   /*!
    * Returns the client rectangle of this control in client coordinates.
    * This function is equivalent to:
    * Rect( 0, 0, ClientWidth(), ClientHeight() )
    */
   pcl::Rect BoundsRect() const
   {
      pcl::Rect r( ClientRect() );
      return pcl::Rect( r.Width(), r.Height() );
   }

   /*! #
    */
   void SetClientRect( const pcl::Rect& r )
   {
      SetClientRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void SetClientRect( int x0, int y0, int x1, int y1 );

   /*! #
    */
   void Resize( int w, int h );

   /*! #
    */
   void AdjustToContents();

   /*!
    * Returns the minimum width in pixels of the client rectangle.
    */
   int MinWidth() const
   {
      int w, dum; GetMinSize( w, dum ); return w;
   }

   /*!
    * Returns the minimum height in pixels of the client rectangle.
    */
   int MinHeight() const
   {
      int dum, h; GetMinSize( dum, h ); return h;
   }

   /*! #
    */
   void GetMinSize( int& w, int& h ) const;

   /*! #
    */
   void SetMinWidth( int w )
   {
      SetMinSize( w, -1 );
   }

   /*! #
    */
   void SetMinWidth()
   {
      SetMinWidth( Width() );
   }

   /*! #
    */
   void SetMinHeight( int h )
   {
      SetMinSize( -1, h );
   }

   /*! #
    */
   void SetMinHeight()
   {
      SetMinHeight( Height() );
   }

   /*! #
    */
   void SetMinSize( int w, int h );

   /*! #
    */
   void SetMinSize()
   {
      SetMinSize( Width(), Height() );
   }

   /*!
    * Returns the maximum width in pixels of the client rectangle.
    */
   int MaxWidth() const
   {
      int w, dum; GetMaxSize( w, dum ); return w;
   }

   /*!
    * Returns the maximum height in pixels of the client rectangle.
    */
   int MaxHeight() const
   {
      int dum, h; GetMaxSize( dum, h ); return h;
   }

   /*! #
    */
   void GetMaxSize( int& w, int& h ) const;

   /*! #
    */
   void SetMaxWidth( int w )
   {
      SetMaxSize( w, -1 );
   }

   /*! #
    */
   void SetMaxWidth()
   {
      SetMaxWidth( Width() );
   }

   /*! #
    */
   void SetMaxHeight( int h )
   {
      SetMaxSize( -1, h );
   }

   /*! #
    */
   void SetMaxHeight()
   {
      SetMaxHeight( Height() );
   }

   /*! #
    */
   void SetMaxSize( int w, int h );

   /*! #
    */
   void SetMaxSize()
   {
      SetMaxSize( Width(), Height() );
   }

   /*! #
    */
   void SetFixedWidth( int w )
   {
      SetFixedSize( w, -1 );
   }

   /*! #
    */
   void SetFixedWidth()
   {
      SetFixedWidth( Width() );
   }

   /*! #
    */
   void SetFixedHeight( int h )
   {
      SetFixedSize( -1, h );
   }

   /*! #
    */
   void SetFixedHeight()
   {
      SetFixedHeight( Height() );
   }

   /*! #
    */
   void SetFixedSize( int w, int h );

   /*! #
    */
   void SetFixedSize()
   {
      SetFixedSize( Width(), Height() );
   }

   /*! #
    */
   bool IsFixedWidth() const
   {
      return MinWidth() == MaxWidth();
   }

   /*! #
    */
   bool IsFixedHeight() const
   {
      return MinHeight() == MaxHeight();
   }

   /*! #
    */
   void SetVariableSize()
   {
      SetMinSize( 0, 0 );
      SetMaxSize( int_max, int_max );
   }

   /*! #
    */
   void SetVariableWidth()
   {
      SetMinWidth( 0 );
      SetMaxWidth( int_max );
   }

   /*! #
    */
   void SetVariableHeight()
   {
      SetMinHeight( 0 );
      SetMaxHeight( int_max );
   }

   /*!
    * Returns the minimum width in pixels of the client rectangle.
    */
   int ScaledMinWidth() const
   {
      int w, dum; GetMinSize( w, dum ); return PhysicalPixelsToLogical( w );
   }

   /*!
    * Returns the minimum height in pixels of the client rectangle.
    */
   int ScaledMinHeight() const
   {
      int dum, h; GetMinSize( dum, h ); return PhysicalPixelsToLogical( h );
   }

   /*! #
    */
   void GetScaledMinSize( int& w, int& h ) const
   {
      GetMinSize( w, h ); w = PhysicalPixelsToLogical( w ); h = PhysicalPixelsToLogical( h );
   }

   /*! #
    */
   void SetScaledMinWidth( int w )
   {
      SetMinSize( LogicalPixelsToPhysical( w ), -1 );
   }

   /*! #
    */
   void SetScaledMinHeight( int h )
   {
      SetMinSize( -1, LogicalPixelsToPhysical( h ) );
   }

   /*! #
    */
   void SetScaledMinSize( int w, int h )
   {
      SetMinSize( LogicalPixelsToPhysical( w ), LogicalPixelsToPhysical( h ) );
   }

   /*!
    * Returns the maximum width in pixels of the client rectangle.
    */
   int ScaledMaxWidth() const
   {
      int w, dum; GetMaxSize( w, dum ); return PhysicalPixelsToLogical( w );
   }

   /*!
    * Returns the maximum height in pixels of the client rectangle.
    */
   int ScaledMaxHeight() const
   {
      int dum, h; GetMaxSize( dum, h ); return PhysicalPixelsToLogical( h );
   }

   /*! #
    */
   void GetScaledMaxSize( int& w, int& h ) const
   {
      GetMaxSize( w, h ); w = PhysicalPixelsToLogical( w ); h = PhysicalPixelsToLogical( h );
   }

   /*! #
    */
   void SetScaledMaxWidth( int w )
   {
      SetMaxSize( LogicalPixelsToPhysical( w ), -1 );
   }

   /*! #
    */
   void SetScaledMaxHeight( int h )
   {
      SetMaxSize( -1, LogicalPixelsToPhysical( h ) );
   }

   /*! #
    */
   void SetScaledMaxSize( int w, int h )
   {
      SetMaxSize( LogicalPixelsToPhysical( w ), LogicalPixelsToPhysical( h ) );
   }

   /*! #
    */
   void SetScaledFixedWidth( int w )
   {
      SetFixedSize( LogicalPixelsToPhysical( w ), -1 );
   }

   /*! #
    */
   void SetScaledFixedHeight( int h )
   {
      SetFixedSize( -1, LogicalPixelsToPhysical( h ) );
   }

   /*! #
    */
   void SetScaledFixedSize( int w, int h )
   {
      SetFixedSize( LogicalPixelsToPhysical( w ), LogicalPixelsToPhysical( h ) );
   }

   /*! #
    */
   bool IsHorizontalExpansionEnabled() const;

   /*! #
    */
   bool IsVerticalExpansionEnabled() const;

   /*! #
    */
   void EnableExpansion( bool horzEnable = true, bool vertEnable = true );

   /*! #
    */
   void DisableExpansion( bool horzDisable = true, bool vertDisable = true )
   {
      EnableExpansion( !horzDisable, !vertDisable );
   }

   /*!
    * Returns the current position of this control in parent coordinates.
    *
    * The position of a control refers to the upper left corner of its frame
    * rectangle.
    *
    * \sa X(), Y()
    */
   pcl::Point Position() const;

   /*!
    * Returns the horizontal position of this control in parent coordinates.
    *
    * \sa Y(), Position()
    */
   int X() const
   {
      return Position().x;
   }

   /*!
    * Returns the horizontal position of this control in parent coordinates.
    *
    * \sa X(), Position()
    */
   int Y() const
   {
      return Position().y;
   }

   /*! #
    */
   void Move( const pcl::Point& p )
   {
      Move( p.x, p.y );
   }

   /*! #
    */
   void Move( int x, int y );

   /*!
      Returns true if the mouse cursor is over this control.
    */
   bool IsUnderMouse() const;

   /*! #
    */
   void BringToFront();

   /*! #
    */
   void SendToBack();

   /*! #
    */
   void StackUnder( Control& );

   /*! #
    */
   pcl::Sizer Sizer() const;

   /*! #
    * \deprecated Use Control::Sizer() in newly produced code.
    */
   pcl::Sizer GetSizer() const
   {
      return this->Sizer();
   }

   /*! #
    */
   void SetSizer( pcl::Sizer& );

   /*! #
    */
   pcl::Point GlobalToLocal( const pcl::Point& p ) const
   {
      pcl::Point p1 = p; GlobalToLocal( p1.x, p1.y ); return p1;
   }

   /*! #
    */
   void GlobalToLocal( int& x, int& y ) const;

   /*! #
    */
   pcl::Point LocalToGlobal( const pcl::Point& p ) const
   {
      pcl::Point p1 = p; LocalToGlobal( p1.x, p1.y ); return p1;
   }

   /*! #
    */
   void LocalToGlobal( int& x, int& y ) const;

   /*! #
    */
   pcl::Point ParentToLocal( const pcl::Point& p ) const
   {
      pcl::Point p1 = p; ParentToLocal( p1.x, p1.y ); return p1;
   }

   /*! #
    */
   void ParentToLocal( int& x, int& y ) const;

   /*! #
    */
   pcl::Point LocalToParent( const pcl::Point& p ) const
   {
      pcl::Point p1 = p; LocalToParent( p1.x, p1.y ); return p1;
   }

   /*! #
    */
   void LocalToParent( int& x, int& y ) const;

   /*! #
    */
   pcl::Point ControlToLocal( const Control& w, const pcl::Point& p ) const
   {
      pcl::Point p1 = p; ControlToLocal( w, p1.x, p1.y ); return p1;
   }

   /*! #
    */
   void ControlToLocal( const Control&, int& x, int& y ) const;

   /*! #
    */
   pcl::Point LocalToControl( const Control& w, const pcl::Point& p ) const
   {
      pcl::Point p1 = p; LocalToControl( w, p1.x, p1.y ); return p1;
   }

   /*! #
    */
   void LocalToControl( const Control&, int& x, int& y ) const;

   /*! #
    */
   Control& ChildByPos( const pcl::Point& p ) const
   {
      return ChildByPos( p.x, p.y );
   }

   /*! #
    */
   Control& ChildByPos( int x, int y ) const;

   /*! #
    */
   pcl::Rect ChildrenRect() const;

   /*! #
    */
   bool IsAncestorOf( const Control& ) const;

   /*!
    * Returns a reference to the parent control, or Control::Null() if this
    * control has no parent.
    */
   Control& Parent() const;

   /*! #
    */
   void SetParent( Control& );

   /*!
    * Returns a reference to the top-level window that owns this control,
    * either directly or through its parent.
    */
   Control& Window() const;

   /*! #
    */
   virtual bool IsEnabled() const;

   /*! #
    */
   virtual void Enable( bool enabled = true );

   /*! #
    */
   void Disable( bool disabled = true )
   {
      Enable( !disabled );
   }

   /*! #
    */
   bool IsMouseTrackingEnabled() const;

   /*! #
    */
   void EnableMouseTracking( bool = true );

   /*! #
    */
   void DisableMouseTracking( bool disable = true )
   {
      EnableMouseTracking( !disable );
   }

   /*! #
    */
   bool IsVisible() const;

   /*! #
    */
   void SetVisible( bool visible )
   {
      if ( visible )
         Show();
      else
         Hide();
   }

   /*! #
    */
   void Show();

   /*! #
    */
   pcl::Rect VisibleRect() const;

   /*! #
    */
   bool IsHidden() const
   {
      return !IsVisible();
   }

   /*! #
    */
   void Hide();

   /*! #
    */
   bool IsMaximized() const;

   /*! #
    */
   bool IsMinimized() const;

   /*! #
    */
   bool IsModal() const;

   /*! #
    */
   bool IsWindow() const;

   /*! #
    */
   bool IsActiveWindow() const;

   /*! #
    */
   void ActivateWindow();

   /*! #
    */
   bool IsFocused() const;

   /*! #
    */
   void Focus( bool focus = true );

   /*! #
    */
   void Unfocus()
   {
      Focus( false );
   }

   /*! #
    */
   FocusStyles FocusStyle() const;

   /*! #
    */
   void SetFocusStyle( FocusStyles );

   /*! #
    */
   Control& FocusedChild() const;

   /*! #
    */
   Control& ChildToFocus() const;

   /*! #
    */
   void SetChildToFocus( Control& );

   /*! #
    */
   Control& NextSiblingToFocus() const;

   /*! #
    */
   void SetNextSiblingToFocus( Control& );

   /*! #
    */
   bool CanUpdate() const;

   /*! #
    */
   void EnableUpdates( bool enable = true );

   /*! #
    */
   void DisableUpdates( bool disable = true )
   {
      EnableUpdates( !disable );
   }

   /*! #
    */
   void Update();

   /*! #
    */
   void Update( const pcl::Rect& r )
   {
      Update( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void Update( int x0, int y0, int x1, int y1 );

   /*! #
    */
   void Repaint();

   /*! #
    */
   void Repaint( const pcl::Rect& r )
   {
      Repaint( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void Repaint( int x0, int y0, int x1, int y1 );

   /*!
    * Recomputes all style properties for this control: colors, fonts,
    * spacings, margings, etc.
    *
    * Call this function to ensure that the screen rendition of this control
    * applies all current settings; for example, after setting a new CSS style
    * sheet with SetStyleSheet().
    */
   void Restyle();

   /*! #
    */
   void Scroll( const pcl::Point& d )
   {
      Scroll( d.x, d.y );
   }

   /*! #
    */
   void Scroll( int dx, int dy );

   /*! #
    */
   void Scroll( const pcl::Point& d, const pcl::Rect& r )
   {
      Scroll( d.x, d.y, r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void Scroll( int dx, int dy, int x0, int y0, int x1, int y1 );

   /*! #
    */
   pcl::Cursor Cursor() const;

   /*! #
    * \deprecated Use Control::Cursor() in newly produced code.
    */
   pcl::Cursor GetCursor() const
   {
      return this->Cursor();
   }

   /*! #
    */
   void SetCursor( const pcl::Cursor& );

   /*! #
    */
   void SetCursorToParent();

   /*! #
    */
   String StyleSheet() const;

   /*! #
    */
   void SetStyleSheet( const String& css );

   /*! #
    */
   RGBA BackgroundColor() const;

   /*! #
    */
   void SetBackgroundColor( RGBA );

   /*! #
    */
   RGBA ForegroundColor() const;

   /*! #
    */
   void SetForegroundColor( RGBA );

   /*! #
    */
   RGBA CanvasColor();

   /*! #
    */
   void SetCanvasColor( RGBA );

   /*! #
    */
   RGBA AlternateCanvasColor() const;

   /*! #
    */
   void SetAlternateCanvasColor( RGBA );

   /*! #
    */
   RGBA TextColor() const;

   /*! #
    */
   void SetTextColor( RGBA );

   /*! #
    */
   RGBA ButtonColor() const;

   /*! #
    */
   void SetButtonColor( RGBA );

   /*! #
    */
   RGBA ButtonTextColor() const;

   /*! #
    */
   void SetButtonTextColor( RGBA );

   /*! #
    */
   RGBA HighlightColor() const;

   /*! #
    */
   void SetHighlightColor( RGBA );

   /*! #
    */
   RGBA HighlightedTextColor() const;

   /*! #
    */
   void SetHighlightedTextColor( RGBA );

   /*! #
    */
   pcl::Font Font() const;

   /*!
    * \deprecated Use Control::Font() in newly produced code.
    */
   pcl::Font GetFont() const
   {
      return this->Font();
   }

   /*! #
    */
   void SetFont( const pcl::Font& );

   /*! #
    */
   double WindowOpacity() const;

   /*! #
    */
   void SetWindowOpacity( double );

   /*! #
    */
   String WindowTitle() const;

   /*! #
    */
   void SetWindowTitle( const String& );

   /*
    * Information areas on processing interface control bars.
    */

   /*! #
    */
   String InfoText() const;

   /*! #
    */
   void SetInfoText( const String& );

   /*! #
    */
   void ClearInfoText()
   {
      SetInfoText( String() );
   }

   /*
    * Track View check boxes on processing interface control bars.
    */

   /*! #
    */
   bool IsTrackViewActive() const;

   /*! #
    */
   void SetTrackViewActive( bool = true );

   /*! #
    */
   void ActivateTrackView()
   {
      SetTrackViewActive( true );
   }

   /*! #
    */
   void DeactivateTrackView()
   {
      SetTrackViewActive( false );
   }

   /*
    * Real-Time Preview check boxes on processing interface control bars.
    */

   /*! #
    */
   bool IsRealTimePreviewActive() const;

   /*! #
    */
   void SetRealTimePreviewActive( bool = true );

   /*! #
    */
   void ActivateRealTimePreview()
   {
      SetRealTimePreviewActive( true );
   }

   /*! #
    */
   void DeactivateRealTimePreview()
   {
      SetRealTimePreviewActive( false );
   }

   //

   /*! #
    */
   String ToolTip() const;

   /*! #
    */
   void SetToolTip( const String& );

   /*!
    * Shows a new tool tip window.
    *
    * \param x,y        Position (horizontal, vertical) of the new tool tip
    *             window in global coordinates.
    *
    * \param text       Tool tip text. Can be either a plain text string or
    *             include HTML tags with CSS style properties. If this is an
    *             empty string the tool tip window will be hidden, if already
    *             visible.
    *
    * \param control    If this is not a null control, the tool tip window will
    *             be hidden as soon as the cursor moves out of the control
    *             boundaries or, if specified, out of the \a rect rectangle in
    *             local coordinates.
    *
    * \param rect       If a non-void rectangle is specified, the tool tip
    *             window will be hidden as soon as the cursor moves out of it.
    *             If a non-null \a control is specified, the coordinates of
    *             \a rect are local to it; otherwise global coordinates are
    *             assumed.
    *
    * The new tool tip window will be shown (or hidden if an empty text is
    * specified) immediately.
    */
   static void ShowToolTip( int x, int y, const String& text,
                  const Control& control = Control::Null(), const Rect& rect = Rect( 0 ) );

   /*!
    * Shows a new tool tip window.
    *
    * This function is equivalent to
    * ShowToolTip( pos.x, pos.y, text, control, rect ).
    */
   static void ShowToolTip( const Point& pos, const String& text,
                  const Control& control = Control::Null(), const Rect& rect = Rect( 0 ) )
   {
      ShowToolTip( pos.x, pos.y, text, control, rect );
   }

   /*!
    * Hides the tool tip window, if one is currently visible.
    */
   static void HideToolTip();

   /*!
    * Returns the tool tip window's text, if a tool tip window is currently
    * visible.
    */
   static String ToolTipText();

   //

   /*!
    * Returns the ratio between physical screen pixels and device-independent
    * logical screen units for the parent top-level window of this %Control
    * object. This ratio is used as a scaling factor by the
    * LogicalPixelsToPhysical() and PhysicalPixelsToLogical() functions, which
    * are used internally by <em>automatic size scaling</em> member functions
    * such as SetScaledFixedSize(), ScaledMinWidth(), etc.
    *
    * The returned value is greater than or equal to one. Typical pixel ratios
    * are 1.5 and 2.0 for high-density displays such as 4K and 5K monitors,
    * respectively, or 1.0 for normal 96 dpi monitors.
    *
    * On OS X, this function normally returns 1.0 for Retina displays working
    * in high-dpi modes, since the operating system performs the conversion
    * from logical to physical pixels automatically. The ResourcePixelRatio()
    * member function returns the actual ratio between physical and logical
    * screen pixels on OS X.
    *
    * \ingroup ui_scaling_functions
    * \sa LogicalPixelsToPhysical(), PhysicalPixelsToLogical(),
    * ResourcePixelRatio()
    */
   double DisplayPixelRatio() const;

   /*!
    * Returns the ratio between physical screen pixels and pixels of
    * device-independent image resources reproduced on the parent top-level
    * window of this %Control object. This ratio is used as a scaling factor by
    * the ScaledResource() member function.
    *
    * The returned value is greater than or equal to one. Typical resource
    * pixel ratios are 1.5 and 2.0 for high-density displays such as 4K and 5K
    * monitors, respectively, 2.0 for Retina displays, or 1.0 for normal 96 dpi
    * monitors.
    *
    * On OS X with Retina monitors working in high-dpi modes, this function
    * returns a value greater than one (typically 2.0), while
    * DisplayPixelRatio() normally returns one by default. This is because in
    * high-dpi modes, OS X works in logical display coordinates to represent
    * text, control sizes and distances. However, image resources must be
    * provided with pixel data in the physical screen resolution. On X11 and
    * Windows platforms, where no automatic display scaling is performed, this
    * member function is equivalent to DisplayPixelRatio().
    *
    * Portable code should use the value returned by this member function to
    * scale image resources drawn on controls, such as icons and bitmaps. The
    * ScaledResource() function can be used to select the appropriate file
    * paths from PixInsight core resources.
    *
    * \ingroup ui_scaling_functions
    * \sa ScaledResource(), DisplayPixelRatio()
    */
   double ResourcePixelRatio() const;

   /*!
    * Returns the specified bitmap \a resource path adapted to be represented
    * with the physical display pixel ratio of this control. Calling this
    * function is equivalent to:
    *
    * \code pcl::UIScaledResource( ResourcePixelRatio(), resource ); \endcode
    *
    * Example:
    *
    * \code
    * Dialog dlg;
    * Bitmap bmp( dlg.ScaledResource( ":/browser/enabled.png" ) );
    * \endcode
    *
    * \ingroup ui_scaling_functions
    * \sa ResourcePixelRatio()
    */
   template <class R>
   String ScaledResource( R resource ) const
   {
      return UIScaledResource( ResourcePixelRatio(), resource );
   }

   /*!
    * Returns a CSS source code fragment transformed with scaled dimensions in
    * pixels and scaled resource file paths, and optionally point sizes
    * converted to scaled pixel sizes.
    *
    * \param cssCode    A string containing the input CSS source code. The
    *                   function will return a transformed version of this
    *                   string.
    *
    * \param fontDPI    If greater than zero, this is the font resolution, in
    *                   dots per inch (dpi), for transformation of point sizes
    *                   to scaled pixel sizes. If this parameter is zero (the
    *                   default value), this routine will use the font
    *                   resolution currently selected in core user preferences
    *                   (which is the value of the "Application/FontResolution"
    *                   global integer variable; see PixInsightSettings). If
    *                   this parameter is a negative integer, no point-to-pixel
    *                   conversions will be applied.
    *
    * Calling this function is equivalent to:
    *
    * \code
    * pcl::UIScaledStyleSheet( DisplayPixelRatio(), ResourcePixelRatio(), cssCode, fontDPI );
    * \endcode
    *
    * Example:
    *
    * \code
    * Data_TreeBox.SetStyleSheet( ScaledStyleSheet(
    *       "QTreeView {"
    *       "   font-family: DejaVu Sans Mono, Monospace;"
    *       "   font-size: 10pt;"
    *       "   background-image: url(:/image-window/transparent.png);"
    *       "}"
    *       "QTreeView::item {"
    *       "   padding: 4px 8px 4px 8px;"
    *       "}"
    *       "QHeaderView::section {"
    *       "   padding: 2px 8px 2px 8px;"
    *       "}"
    *    ) );
    * \endcode
    *
    * If the display and resource pixel ratios of \c Data_TreeBox were 1.5 (for
    * example, on a 4K monitor), and the current font resolution were 100 dpi,
    * the code above would be equivalent to:
    *
    * \code
    * Data_TreeBox.SetStyleSheet(
    *       "QTreeView {"
    *       "   font-family: DejaVu Sans Mono, Monospace;"
    *       "   font-size: 21px;"
    *       "   background-image: url(:/image-window/1.5/transparent.png);"
    *       "}"
    *       "QTreeView::item {"
    *       "   padding: 6px 12px 6px 12px;"
    *       "}"
    *       "QHeaderView::section {"
    *       "   padding: 3px 12px 3px 12px;"
    *       "}"
    *    );
    * \endcode
    *
    * \ingroup ui_scaling_functions
    * \sa DisplayPixelRatio(), ResourcePixelRatio(), ScaledResource()
    */
   template <class S>
   String ScaledStyleSheet( S cssCode, int fontDPI = 0 ) const
   {
      return UIScaledStyleSheet( DisplayPixelRatio(), ResourcePixelRatio(), cssCode, fontDPI );
   }

   /*!
    * Returns a cursor hot spot point with coordinates scaled according to the
    * physical display pixel ratio of this control.
    *
    * Standard cursor images in PixInsight are scaled in a special way to
    * guarantee that cursors always have a unique central pixel, irrespective
    * of the applied display pixel ratios. This is functionally equivalent to
    * ensure that cursors always have odd dimensions.
    *
    * According to this rule, a standard cursor is a square image with the
    * following dimensions in pixels:
    *
    * 21, 33, 43, 53, 63, 75, 85
    *
    * respectively for the standard scaling ratios 1.0, 1.5, 2.0, 2.5, 3.0, 3.5
    * and 4.0. For dynamic cursors this function must be used to calculate
    * scaled cursor hot spot coordinates, instead of simply scaling by
    * multiplication with ResourcePixelRatio() and rounding. This also
    * guarantees source code compatibility with future versions of PixInsight,
    * where standard cursor dimensions might change.
    *
    * \ingroup ui_scaling_functions
    * \sa ResourcePixelRatio();
    */
   Point ScaledCursorHotSpot( int xHot, int yHot ) const
   {
      double r = ResourcePixelRatio();
      return Point( RoundInt( r*(xHot + 0.499) ), RoundInt( r*(yHot + 0.499) ) );
   }

   /*!
    * Returns a cursor hot spot point with coordinates scaled according to the
    * physical display pixel ratio of this control.
    *
    * This function is equivalent to:
    *
    * \code ScaledCursorHotSpot( hotSpot.x, hotSpot.y ); \endcode
    *
    * \ingroup ui_scaling_functions
    */
   Point ScaledCursorHotSpot( const Point& hotSpot ) const
   {
      return ScaledCursorHotSpot( hotSpot.x, hotSpot.y );
   }

   /*!
    * Returns the specified \a size in logical device-independent pixel units
    * converted to physical device pixels.
    *
    * \ingroup ui_scaling_functions
    * \sa DisplayPixelRatio(), PhysicalPixelsToLogical()
    */
   int LogicalPixelsToPhysical( int size ) const
   {
      return RoundInt( DisplayPixelRatio()*size );
   }

   /*!
    * Returns the specified \a size in physical device pixels converted to
    * logical device-independent pixel units.
    *
    * \ingroup ui_scaling_functions
    * \sa DisplayPixelRatio(), LogicalPixelsToPhysical()
    */
   int PhysicalPixelsToLogical( int size ) const
   {
      return RoundInt( size/DisplayPixelRatio() );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnDestroy( Control& sender );
   // void OnShow( Control& sender );
   // void OnHide( Control& sender );
   // void OnGetFocus( Control& sender );
   // void OnLoseFocus( Control& sender );
   // void OnEnter( Control& sender );
   // void OnLeave( Control& sender );
   // void OnMove( Control& sender, const pcl::Point& newPos, const pcl::Point& oldPos );
   // void OnResize( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );
   // void OnPaint( Control& sender, const pcl::Rect& updateRect );
   // void OnKeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   // void OnKeyRelease( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   // void OnMouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   // void OnMouseDoubleClick( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   // void OnMousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   // void OnMouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   // void OnMouseWheel( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );
   // void OnFileDrag( Control& sender, const pcl::Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
   // void OnFileDrop( Control& sender, const pcl::Point& pos, const StringList& files, unsigned modifiers )
   // void OnViewDrag( Control& sender, const pcl::Point& pos, const View& view, unsigned modifiers, bool& wantsView )
   // void OnViewDrop( Control& sender, const pcl::Point& pos, const View& view, unsigned modifiers )
   // void OnChildCreate( Control& sender, Control& child );
   // void OnChildDestroy( Control& sender, Control& child );

   /*!
    * \defgroup control_event_handlers Control Event Handlers
    */

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*event_handler)( Control& sender );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*close_event_handler)( Control& sender, bool& allowClose );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*move_event_handler)( Control& sender, const pcl::Point& newPos, const pcl::Point& oldPos );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*resize_event_handler)( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*paint_event_handler)( Control& sender, const pcl::Rect& updateRect );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*keyboard_event_handler)( Control& sender, int key, unsigned modifiers, bool& wantsKey );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*mouse_event_handler)( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*mouse_button_event_handler)( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*mouse_wheel_event_handler)( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*file_drag_event_handler)( Control& sender, const pcl::Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*file_drop_event_handler)( Control& sender, const pcl::Point& pos, const StringList& files, unsigned modifiers );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*view_drag_event_handler)( Control& sender, const pcl::Point& pos, const View& view, unsigned modifiers, bool& wantsView );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*view_drop_event_handler)( Control& sender, const pcl::Point& pos, const View& view, unsigned modifiers );

   /*! #
    * \ingroup control_event_handlers
    */
   typedef void (Control::*child_event_handler)( Control& sender, Control& child );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnDestroy( event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnShow( event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnHide( event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnClose( close_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnGetFocus( event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnLoseFocus( event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnEnter( event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnLeave( event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnMove( move_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnResize( resize_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnPaint( paint_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnKeyPress( keyboard_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnKeyRelease( keyboard_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnMouseMove( mouse_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnMouseDoubleClick( mouse_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnMousePress( mouse_button_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnMouseRelease( mouse_button_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnMouseWheel( mouse_wheel_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnFileDrag( file_drag_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnFileDrop( file_drop_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnViewDrag( view_drag_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnViewDrop( view_drop_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnChildCreate( child_event_handler, Control& );

   /*! #
    * \ingroup control_event_handlers
    */
   void OnChildDestroy( child_event_handler, Control& );

   // -------------------------------------------------------------------------

private:

   struct EventHandlers
   {
      event_handler              onDestroy          = nullptr;
      event_handler              onShow             = nullptr;
      event_handler              onHide             = nullptr;
      close_event_handler        onClose            = nullptr;
      event_handler              onGetFocus         = nullptr;
      event_handler              onLoseFocus        = nullptr;
      event_handler              onEnter            = nullptr;
      event_handler              onLeave            = nullptr;
      move_event_handler         onMove             = nullptr;
      resize_event_handler       onResize           = nullptr;
      paint_event_handler        onPaint            = nullptr;
      keyboard_event_handler     onKeyPress         = nullptr;
      keyboard_event_handler     onKeyRelease       = nullptr;
      mouse_event_handler        onMouseMove        = nullptr;
      mouse_event_handler        onMouseDoubleClick = nullptr;
      mouse_button_event_handler onMousePress       = nullptr;
      mouse_button_event_handler onMouseRelease     = nullptr;
      mouse_wheel_event_handler  onMouseWheel       = nullptr;
      file_drag_event_handler    onFileDrag         = nullptr;
      file_drop_event_handler    onFileDrop         = nullptr;
      view_drag_event_handler    onViewDrag         = nullptr;
      view_drop_event_handler    onViewDrop         = nullptr;
      child_event_handler        onChildCreate      = nullptr;
      child_event_handler        onChildDestroy     = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

protected:

   /*!
    * \internal
    * Private constructor from a low-level opaque server handle.
    */
   Control( void* h ) : UIObject( h )
   {
   }

   /*!
    * \internal
    * Reimplemented from UIObject. Throws an Error exception because Control
    * objects are unique, and hence cannot be duplicated.
    */
   virtual void* CloneHandle() const;

   friend class BitmapBox;
   friend class CheckBox;
   friend class CodeEditor;
   friend class ComboBox;
   friend class ControlEventDispatcher;
   friend class Dialog;
   friend class Edit;
   friend class Frame;
   friend class GraphicsContextBase;
   friend class GroupBox;
   friend class ImageView;
   friend class Label;
   friend class PushButton;
   friend class RadioButton;
   friend class ScrollBox;
   friend class Sizer;
   friend class Slider;
   friend class SpinBox;
   friend class TabBox;
   friend class TextBox;
   friend class ToolButton;
   friend class TreeBox;
   friend class ViewList;
   friend class WebView;
};

// ----------------------------------------------------------------------------

template <class C> inline
int CanonicalControlHeightImplementation()
{
   Control       container;
   VerticalSizer sizer;
   C             control;

   sizer.Add( control );
   container.SetSizer( sizer );
   container.Restyle();
   container.AdjustToContents();
   container.SetFixedSize();
   return container.Height();
}

/*!
 * \defgroup aux_control_functions_and_classes Control Helper Functions and Classes
 */

/*!
 * \def CanonicalControlHeight( control_type )
 * \brief Returns the canonical height in pixels for a specified control type.
 *
 * The canonical height is the height in pixels that a control of the specified
 * type would have once adjusted into a VerticalSizer container and a parent
 * control object. This macro is useful to calculate the height of a control in
 * advance, before actually creating its component sub-controls.
 *
 * \ingroup aux_control_functions_and_classes
 */
#define CanonicalControlHeight( control_type ) \
   CanonicalControlHeightImplementation<control_type>()

// ----------------------------------------------------------------------------

#define __PCL_NO_ALIAS_HANDLERS \
   if ( IsAlias() )             \
      throw Error( "Aliased controls cannot set event handlers." )

// ----------------------------------------------------------------------------

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Control_h

// ----------------------------------------------------------------------------
// EOF pcl/Control.h - Released 2017-06-17T10:55:43Z
