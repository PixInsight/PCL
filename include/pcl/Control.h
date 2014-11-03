// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Control.h - Released 2014/10/29 07:34:07 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_Control_h
#define __PCL_Control_h

/// \file pcl/Control.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Flags_h
#include <pcl/Flags.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_Rectangle_h
#include <pcl/Rectangle.h>
#endif

#ifndef __PCL_Sizer_h
#include <pcl/Sizer.h>
#endif

#ifndef __PCL_KeyCodes_h
#include <pcl/KeyCodes.h>
#endif

#ifndef __PCL_ButtonCodes_h
#include <pcl/ButtonCodes.h>
#endif

#ifndef __PCL_Color_h
#include <pcl/Color.h>
#endif

#ifndef __PCL_Cursor_h
#include <pcl/Cursor.h>
#endif

#ifndef __PCL_Font_h
#include <pcl/Font.h>
#endif

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace FocusStyle
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
    * The optional \a flags argument is currently not used; it is reserved for
    * future extensions and must be zero (its default value).
    */
   Control( Control& parent = Null(), uint32 flags = 0 );

   /*!
    * Contructs a %Control object as an alias of an existing \a control object.
    *
    * \note    It is very important to point out that this constructor <em>does
    * not create a new control</em> in the PixInsight core application. It only
    * creates an \e alias object for an existing control <em>in your
    * module</em>. The alias and aliased objects are interchangeable; their
    * behaviors are identical since they refer to a unique object living in the
    * PixInsight core application.
    *
    * \note    Specifying event handlers for an alias %Control object is
    * incorrect and would have (if allowed) no effect. This is because the
    * PixInsight core application actually knows nothing about an alias object
    * constructed in a module (except the fact that the aliased object is being
    * referenced more than once). Note that this limitation is in effect even
    * if the aliased object is destroyed and the alias object still lives.
    * Events are always dispatched to original, non-alias UI objects. To
    * prevent hard-to-debug errors due to control aliasing, an Error exception
    * is thrown if an attempt to specify an event handler from an aliased
    * control is detected. For example:
    *
    * \code
    * Control c1;
    * Control c2 = c1; // c2 is an alias for c1
    * c1.OnPaint( myPaintRoutine, myPaintReceiver ); // OK
    * c2.Move( 100, 100 ); // OK: Moves c1 to {100,100} coordinates
    * c2.OnShow( handler, receiver ); // Error: aliased controls cannot set event handlers
    * \endcode
    */
   Control( const Control& control );

   /*!
    * Destroys a %Control object.
    */
   virtual ~Control()
   {
   }

   /*!
    * Assignment operator. Detaches this object from its previously referenced
    * server-side control object and makes it an alias of the specified
    * \a control. Returns a reference to this object.
    *
    * This object is detached from the previously referenced control, and if it
    * becomes unreferenced, the server-side control object is destroyed. Then
    * this object is attached as an alias to the specified \a control. See the
    * documentation for the Control::Control( const Control& ) copy constructor
    * for more information on aliased controls.
    *
    * This operator resets all event handlers in this object to zero function
    * pointers. This is because alias controls cannot specify event handlers.
    */
   Control& operator =( const Control& control );

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since server-side controls are unique objects, calling this member
    * function has no effect.
    */
   virtual void SetUnique()
   {
      // Controls are unique objects by definition
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
      Returns a reference to the parent control, or Control::Null() if this
      control has no parent.
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
   // void OnChildCreate( Control& sender, Control& child );
   // void OnChildDestroy( Control& sender, Control& child );

   /*! #
    */
   typedef void (Control::*event_handler)( Control& sender );

   /*! #
    */
   typedef void (Control::*close_event_handler)( Control& sender, bool& allowClose );

   /*! #
    */
   typedef void (Control::*move_event_handler)( Control& sender, const pcl::Point& newPos, const pcl::Point& oldPos );

   /*! #
    */
   typedef void (Control::*resize_event_handler)( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );

   /*! #
    */
   typedef void (Control::*paint_event_handler)( Control& sender, const pcl::Rect& updateRect );

   /*! #
    */
   typedef void (Control::*keyboard_event_handler)( Control& sender, int key, unsigned modifiers, bool& wantsKey );

   /*! #
    */
   typedef void (Control::*mouse_event_handler)( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );

   /*! #
    */
   typedef void (Control::*mouse_button_event_handler)( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );

   /*! #
    */
   typedef void (Control::*mouse_wheel_event_handler)( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );

   /*! #
    */
   typedef void (Control::*child_event_handler)( Control& sender, Control& child );

   /*! #
    */
   void OnDestroy( event_handler, Control& );

   /*! #
    */
   void OnShow( event_handler, Control& );

   /*! #
    */
   void OnHide( event_handler, Control& );

   /*! #
    */
   void OnClose( close_event_handler, Control& );

   /*! #
    */
   void OnGetFocus( event_handler, Control& );

   /*! #
    */
   void OnLoseFocus( event_handler, Control& );

   /*! #
    */
   void OnEnter( event_handler, Control& );

   /*! #
    */
   void OnLeave( event_handler, Control& );

   /*! #
    */
   void OnMove( move_event_handler, Control& );

   /*! #
    */
   void OnResize( resize_event_handler, Control& );

   /*! #
    */
   void OnPaint( paint_event_handler, Control& );

   /*! #
    */
   void OnKeyPress( keyboard_event_handler, Control& );

   /*! #
    */
   void OnKeyRelease( keyboard_event_handler, Control& );

   /*! #
    */
   void OnMouseMove( mouse_event_handler, Control& );

   /*! #
    */
   void OnMouseDoubleClick( mouse_event_handler, Control& );

   /*! #
    */
   void OnMousePress( mouse_button_event_handler, Control& );

   /*! #
    */
   void OnMouseRelease( mouse_button_event_handler, Control& );

   /*! #
    */
   void OnMouseWheel( mouse_wheel_event_handler, Control& );

   /*! #
    */
   void OnChildCreate( child_event_handler, Control& );

   /*! #
    */
   void OnChildDestroy( child_event_handler, Control& );

   // -------------------------------------------------------------------------

protected:

   event_handler                 onDestroy;
   event_handler                 onShow;
   event_handler                 onHide;
   close_event_handler           onClose;
   event_handler                 onGetFocus;
   event_handler                 onLoseFocus;
   event_handler                 onEnter;
   event_handler                 onLeave;
   move_event_handler            onMove;
   resize_event_handler          onResize;
   paint_event_handler           onPaint;
   keyboard_event_handler        onKeyPress;
   keyboard_event_handler        onKeyRelease;
   mouse_event_handler           onMouseMove;
   mouse_event_handler           onMouseDoubleClick;
   mouse_button_event_handler    onMousePress;
   mouse_button_event_handler    onMouseRelease;
   mouse_wheel_event_handler     onMouseWheel;
   child_event_handler           onChildCreate;
   child_event_handler           onChildDestroy;

   Control( void* );

   virtual void* CloneHandle() const;

   friend class BitmapBox;
   friend class CheckBox;
   friend class CodeEditor;
   friend class ComboBox;
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

   friend class ControlEventDispatcher;
};

// ----------------------------------------------------------------------------

template <class C> inline
int CanonicalControlHeightImplementation( C* )
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
 * \defgroup aux_control_functions_and_classes Auxiliary Control Functions and
 * Classes
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
#define CanonicalControlHeight( control_type )  CanonicalControlHeightImplementation( (control_type*)0 )

// ----------------------------------------------------------------------------

#define __PCL_NO_ALIAS_HANDLER                                                \
   if ( IsAlias() )                                                           \
      throw Error( "Aliased controls cannot set event handlers" )

// ----------------------------------------------------------------------------

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Control_h

// ****************************************************************************
// EOF pcl/Control.h - Released 2014/10/29 07:34:07 UTC
