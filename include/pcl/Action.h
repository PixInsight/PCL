//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/Action.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_Action_h
#define __PCL_Action_h

/// \file pcl/Action.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Bitmap.h>
#include <pcl/UIObject.h>

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ActionInfo
 * \brief Provides information about the current state of the core
 *        application's GUI to module-defined actions.
 *
 * This structure is passed to the Action::IsEnabled() virtual member function.
 * See the description of Action::IsEnabled() for more information.
 *
 * \sa Action
 */
struct ActionInfo
{
   bool  isImage           :  1; //!< true if there is an active image window and it is not read-locked
   bool  isCurrentPreview  :  1; //!< true if the active view is a preview
   bool  isColor           :  1; //!< true if the active view is a color image; false if it is grayscale
   int                     :  5; // reserved for future use, should be zero
   uint8 bitsPerSample     :  8; //!< number of bits per sample in the active view's image
   bool  isFloatSample     :  1; //!< true if the active view's image is in floating-point format
   bool  hasPreviews       :  1; //!< true if the active image window has one or more previews defined
   bool  hasMask           :  1; //!< true if the active image window is masked
   bool  thereAreImages    :  1; //!< true if there are one or more image windows currently open
   int                     : 12; // reserved for future use, should be zero
};

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class Action
 * \brief Client-side interface to a PixInsight module-defined action object.
 *
 * Module-defined actions manage integration of external processes and tools
 * with the main menu and tool bars of the PixInsight core application.
 *
 * Integration of actions is governed by two main properties: menu item and
 * tool bar.
 *
 * <b>Menu Item</b>
 *
 * This is a string indicating a menu item where an action will be integrated
 * within the PixInsight's main menu structure.
 *
 * The menu item string describes a path on PixInsight's main menu structure.
 * Multiple menu items can be specified, separated with the ">" character.
 *
 * If this parameter is an empty string, the newly created action won't be
 * integrated with the main menu. This can be useful for actions that only
 * respond to keyboard accelerators, or actions that only integrate with tool
 * bars (see the Tool Bar section below).
 *
 * <b>Integrating actions into existing top-level main menu items</b>
 *
 * "Image >> Geometry > Rotate 180 degrees"
 * "View > Memory Status"
 *
 * The above strings will integrate actions into existing top-level main menu
 * items (Image and View, respectively). In the first example, a submenu
 * entitled "Geometry" will be created under the Image top-level menu item, if
 * it doesn't exist yet. The sequence '>>' can be used to insert a separator
 * menu item, as has been done before "Geometry" in the example above. Then a
 * new item "Rotate 180 degrees" will be created and associated to the
 * corresponding action. Note that in this case the PixInsight core application
 * decides where exactly new actions are inserted within existing menu items,
 * applying different criteria for each menu. In general, in these cases new
 * actions are inserted on a per-module basis, in strict order of creation.
 *
 * <b>Creating custom top-level main menu items</b>
 *
 * Custom main menu top-level entries can also be created. For example, the
 * following menu item string:
 *
 * "MyTools > Color Tools > A better color saturation process"
 *
 * Will create a new custom "MyTools" top-level item in the main menu, if it
 * doesn't already exist. Custom top-level main menu items are inserted after
 * the standard Process menu item, in strict order of creation. Common sense
 * and care must be observed when creating custom top-level menu items. If
 * possible, it is always preferable to integrate actions into existing
 * top-level items; one must be motivated by a really strong reason to create a
 * custom menu.
 *
 * <b>Tool Bar</b>
 *
 * This is an optional parameter. This is the name of a tool bar where the
 * action will be integrated.
 *
 * If this string is empty, the action will not be integrated in a tool bar.
 * Otherwise, the string must be the name of a tool bar where a new tool button
 * will be created and associated to the action.
 *
 * A standard tool bar name can be used (i.e. View, File, and so on). If the
 * specified name does not correspond to an existing tool bar, a new, custom
 * tool bar will be created, and a new tool button corresponding to this action
 * will be added to it.
 *
 * The specified tool bar name can start with a ">" character to insert a tool
 * bar separator (a vertical or horizontal line, depending on the tool bar's
 * orientation) before the newly created tool button.
 */
class PCL_CLASS Action : public UIObject
{
public:

   /*!
    * Constructs a new %Action object.
    *
    * \param menuItem   Specifies the menu item that will be associated to this
    *             action.
    *
    * \param icon       A Bitmap object representing the icon image that will
    *             be associated to this action. The icon will be used for menu
    *             items and tool bar buttons. If this parameter is not
    *             specified, the action will have no associated icon.
    *
    * \param toolBar    The name of a tool bar where this action will be
    *             integrated. If this parameter is not specified, the action
    *             will not be integrated in a tool bar.
    *
    * To learn how the menu item and tool bar parameters work for actions, see
    * the description of the Action class.
    */
   Action( const String& menuItem,
           const Bitmap& icon = Bitmap::Null(), const String toolBar = String() );

   /*!
    * Move constructor.
    */
   Action( Action&& x ) : UIObject( std::move( x ) )
   {
   }

   /*!
    * Destroys this %Action object.
    *
    * This destructor does not destroy the actual action object, which is part
    * of the PixInsight core application. Only the managed alias object living
    * in the user-defined module is destroyed.
    */
   virtual ~Action()
   {
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Action& operator =( Action&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since actions are unique objects, calling this member function has no
    * effect.
    */
   virtual void EnsureUnique()
   {
   }

   /*!
    * Returns a reference to a null %Action instance. A null %Action does not
    * correspond to an existing action in the PixInsight core application.
    */
   static Action& Null();

   /*!
    * Returns the menu item where this action has been integrated in the main
    * menu of the PixInsight core application.
    *
    * An action's menu item is a read-only property: its value can only be set
    * with the constructor. In other words: Once an action has been created,
    * its menu text can be freely changed, but it cannot be moved in the main
    * menu structure.
    *
    * \sa MenuText()
    */
   String MenuItem() const;

   /*!
    * Returns the text of the menu item where this action has been integrated
    * in the main menu of the PixInsight core application.
    *
    * \sa SetMenuText(), MenuItem()
    */
   String MenuText() const;

   /*!
    * Changes the text of the menu item where this action has been integrated
    * in the main menu of the PixInsight core application.
    *
    * \param text    A string representing the new menu text for this action
    *                object.
    *
    * \sa MenuText()
    */
   void SetMenuText( const String& text );

   /*!
    * Returns the name of a tool bar where this action has been integrated in
    * the PixInsight core application.
    *
    * Integration of actions with tool bars is optional. If this action has not
    * been integrated in a tool bar, this function returns an empty string.
    *
    * An action's tool bar is a read-only property whose value can only be set
    * with the constructor.
    *
    * \sa SetToolBar()
    */
   String ToolBar() const;

   /*!
    * Obtains the optional keyboard accelerator associated to this action.
    *
    * \param[out] keyModifiers   Specifies an OR'ed combination of the Shift,
    *             Control and Alt keys. Use the KeyModifier namespace, defined
    *             in the pcl/ButtonCodes.h header, to decode this value.
    *
    * \param[out] keyCode        Any valid PCL key code, except those codes
    *             corresponding to modifier keys or system-managed keys. Use
    *             the KeyCode namespace, defined in the pcl/KeyCodes.h header,
    *             to decode the returned value.
    *
    * If no keyboard accelerator has been associated to this action, zero is
    * returned for both parameters.
    *
    * \sa SetAccelerator(), HasAccelerator(), RemoveAccelerator()
    */
   void GetAccelerator( int& keyModifiers, int& keyCode ) const;

   /*!
    * Changes the keyboard accelerator associated to this action.
    *
    * \param keyModifiers        An OR'ed combination of the Shift, Control and
    *             Alt keys. Use the KeyModifier namespace, defined in the
    *             pcl/ButtonCodes.h header, to build this value.
    *
    * \param keyCode             A valid PCL key code, except those codes
    *             corresponding to modifier keys or system-managed keys. Use
    *             the KeyCode namespace, defined in the pcl/KeyCodes.h header,
    *             to build this value. If this parameter is zero, no keyboard
    *             accelerator will be associated to this action.
    *
    * \sa GetAccelerator(), HasAccelerator(), RemoveAccelerator()
    */
   void SetAccelerator( int keyModifiers, int keyCode );

   /*!
    * Returns true iff this action has an associated keyboard accelerator.
    *
    * \sa GetAccelerator(), SetAccelerator(), RemoveAccelerator()
    */
   bool HasAccelerator() const
   {
      int dum, key; GetAccelerator( dum, key ); return key != 0;
   }

   /*!
    * Clears (deactivates) the keyboard accelerator associated to this action,
    * if any. This is an overloaded function, provided for convenience. It
    * is equivalent to SetAccelerator( 0, 0 ).
    *
    * \sa GetAccelerator(), SetAccelerator(), HasAccelerator()
    */
   void RemoveAccelerator()
   {
      SetAccelerator( 0, 0 );
   }

   /*!
    * Returns the tool tip text that has been assigned to this action. Tool
    * tips are used for tool buttons corresponding to the integration of
    * actions in tool bars.
    *
    * If this action has not been integrated in a tool bar, this function has
    * no meaning, and an empty string is always returned.
    *
    * \sa SetToolTip()
    */
   String ToolTip() const;

   /*!
    * Changes the tool tip text for this action. Tool tips are used for tool
    * buttons corresponding to the integration of actions in tool bars.
    *
    * \param tip  A string that represents the new tool tip text for this
    *             action object.
    *
    * If this action has not been integrated in a tool bar, this function is
    * ignored.
    *
    * \sa ToolTip()
    */
   void SetToolTip( const String& tip );

   /*!
    * Returns a Bitmap corresponding to the icon that has been assigned to
    * this action object. %Action icons are used to render the associated menu
    * items and tool bar buttons.
    *
    * If no icon has been associated to this object, a null Bitmap object is
    * returned.
    *
    * \sa SetIcon()
    */
   Bitmap Icon() const;

   /*!
    * Changes the icon assigned to this %Action object. %Action icons are used
    * to render the associated menu items and tool bar buttons.
    *
    * \param icon    The new icon Bitmap that will be assigned to this %Action
    *             object. If a null bitmap is specified, the core application
    *             will assign a default icon for the tool bar button (if this
    *             action has been integrated in a tool bar), and no icon will
    *             be drawn on the associated menu item.
    *
    * \sa Icon()
    */
   void SetIcon( const Bitmap& icon );

   /*!
    * Callback routine for this action. This function will be called each time
    * this action is activated, either by selecting the associated menu item or
    * by clicking the corresponding tool button.
    *
    * Despite this function has not been formalized as a pure virtual function,
    * it is such conceptually, so it must be always reimplemented in descendant
    * classes.
    */
   virtual void Execute();

   /*!
    * Returns the current \e enabled \e state of this %Action object.
    *
    * Disabled actions have their associated menu items and tool bar buttons
    * disabled, and cannot be activated by the user.
    *
    * The PixInsight application will call this function repeatedly to learn
    * the current state of this action, with the purpose of keeping the
    * associated interface elements up-to-date.
    *
    * The caller of this function is a specialized idle-priority thread that is
    * permanently running in the background. This means that this function will
    * not be executed in the core application's main GUI thread, so all code in
    * this function must be thread-safe. Reimplementations of this function in
    * derived classes should decide the action's current state and return as
    * quickly as possible.
    *
    * An ActionInfo structure is passed to this function, describing the
    * current state of the user interface. That description should be used by
    * the IsEnabled() function to decide whether this action should be enabled
    * or disabled, depending on the current interface context.
    *
    * \note The default implementation of this function always returns true,
    * regardless of the current context. This means that all actions are
    * enabled by default.
    *
    * \sa ActionInfo
    */
   virtual bool IsEnabled( ActionInfo info ) const
   {
      return true;
   }

private:

   Action( void* h ) : UIObject( h )
   {
   }

   virtual void* CloneHandle() const;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Action_h

// ----------------------------------------------------------------------------
// EOF pcl/Action.h - Released 2017-06-28T11:58:36Z
