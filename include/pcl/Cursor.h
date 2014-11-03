// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Cursor.h - Released 2014/10/29 07:34:07 UTC
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

#ifndef __PCL_Cursor_h
#define __PCL_Cursor_h

/// \file pcl/Cursor.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_Point_h
#include <pcl/Point.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace StdCursor
 * \brief Standard cursor shapes available in the PixInsight core application
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>StdCursor::NoCursor</td>               <td>No cursor is shown</td></tr>
 * <tr><td>StdCursor::Arrow</td>                  <td>Standard arrow cursor (pointing left)</td></tr>
 * <tr><td>StdCursor::InvArrow</td>               <td>Inverted arrow cursor (pointing right)</td></tr>
 * <tr><td>StdCursor::UpArrow</td>                <td>Upwards arrow</td></tr>
 * <tr><td>StdCursor::DownArrow</td>              <td>Downwards arrow</td></tr>
 * <tr><td>StdCursor::LeftArrow</td>              <td>Leftwards arrow</td></tr>
 * <tr><td>StdCursor::RightArrow</td>             <td>Rightwards arrow</td></tr>
 * <tr><td>StdCursor::Checkmark</td>              <td>Checkmark (ok) cursor</td></tr>
 * <tr><td>StdCursor::Crossmark</td>              <td>Crossmark (cancel) cursor</td></tr>
 * <tr><td>StdCursor::Accept</td>                 <td>Arrow + checkmark</td></tr>
 * <tr><td>StdCursor::Reject</td>                 <td>Arrow + crossmark</td></tr>
 * <tr><td>StdCursor::Add</td>                    <td>Arrow + plus sign</td></tr>
 * <tr><td>StdCursor::Copy</td>                   <td>Arrow + square</td></tr>
 * <tr><td>StdCursor::Cross</td>                  <td>Crosshair</td></tr>
 * <tr><td>StdCursor::Hourglass</td>              <td>Hourglass (native Windows wait cursor)</td></tr>
 * <tr><td>StdCursor::Watch</td>                  <td>Watch (native Macintosh wait cursor)</td></tr>
 * <tr><td>StdCursor::Wait</td>                   <td>Default wait cursor (same as StdCursor::Watch)</td></tr>
 * <tr><td>StdCursor::ArrowWait</td>              <td>Arrow + hourglass/watch</td></tr>
 * <tr><td>StdCursor::ArrowQuestion</td>          <td>Arrow + question mark</td></tr>
 * <tr><td>StdCursor::IBeam</td>                  <td>I-beam cursor (text edition)</td></tr>
 * <tr><td>StdCursor::VerticalSize</td>           <td>Vertical resize</td></tr>
 * <tr><td>StdCursor::HorizontalSize</td>         <td>Horizontal resize</td></tr>
 * <tr><td>StdCursor::ForwardDiagonalSize</td>    <td>Forward diagonal resize (/)</td></tr>
 * <tr><td>StdCursor::BackwardDiagonalSize</td>   <td>Backward diagonal resize (\\)</td></tr>
 * <tr><td>StdCursor::SizeAll</td>                <td>Resize in all directions</td></tr>
 * <tr><td>StdCursor::VerticalSplit</td>          <td>Split vertical</td></tr>
 * <tr><td>StdCursor::HorizontalSplit</td>        <td>Split horizontal</td></tr>
 * <tr><td>StdCursor::Hand</td>                   <td>Pointing hand cursor</td></tr>
 * <tr><td>StdCursor::PointingHand</td>           <td>Pointing hand cursor (same as StdCursor::Hand)</td></tr>
 * <tr><td>StdCursor::OpenHand</td>               <td>Open hand cursor (used to inform about dragging operations)</td></tr>
 * <tr><td>StdCursor::ClosedHand</td>             <td>Closed hand cursor (used during active dragging operations)</td></tr>
 * <tr><td>StdCursor::SquarePlus</td>             <td>Plus sign into a square (used for zooming in)</td></tr>
 * <tr><td>StdCursor::SquareMinus</td>            <td>Minus sign into a square (used for zooming out)</td></tr>
 * <tr><td>StdCursor::CirclePlus</td>             <td>Plus sign into a circle (used for zooming in)</td></tr>
 * <tr><td>StdCursor::CircleMinus</td>            <td>Minus sign into a circle (used for zooming out)</td></tr>
 * <tr><td>StdCursor::Forbidden</td>              <td>Stop cursor</td></tr>
 * </table>
 */
namespace StdCursor
{
   enum value_type
   {
      NoCursor,               // no cursor is shown
      Arrow,                  // standard arrow cursor (pointing left)
      InvArrow,               // inverted arrow cursor (pointing right)
      UpArrow,                // upwards arrow
      DownArrow,              // downwards arrow
      LeftArrow,              // leftwards arrow
      RightArrow,             // rightwards arrow
      Checkmark,              // checkmark (ok) cursor
      Crossmark,              // crossmark (cancel) cursor
      Accept,                 // arrow + checkmark
      Reject,                 // arrow + crossmark
      Add,                    // arrow + plus sign
      Copy,                   // arrow + square
      Cross,                  // crosshair
      Hourglass,              // hourglass (native Windows wait cursor)
      Watch,                  // watch (native Macintosh wait cursor)
      Wait           = Watch, // wait cursor: we like the watch! :)
      ArrowWait,              // arrow + hourglass/watch
      ArrowQuestion,          // arrow + question mark
      IBeam,                  // I-beam cursor (text edition)
      VerticalSize,           // vertical resize
      HorizontalSize,         // horizontal resize
      ForwardDiagonalSize,    // forward diagonal resize (/)
      BackwardDiagonalSize,   // backward diagonal resize (\)
      SizeAll,                // resize in all directions
      VerticalSplit,          // split vertical
      HorizontalSplit,        // split horizontal
      Hand,                   // pointing hand cursor
      PointingHand   = Hand,  // pointing hand cursor (same as Hand)
      OpenHand,               // open hand cursor
      ClosedHand,             // closed hand cursor
      SquarePlus,             // plus sign into a square (used for zoom in)
      SquareMinus,            // minus sign into a square (used for zoom out)
      CirclePlus,             // plus sign into a circle (used for zoom in)
      CircleMinus,            // minus sign into a circle (used for zoom out)
      Forbidden               // stop cursor
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

class Bitmap;

// ----------------------------------------------------------------------------

/*!
 * \class Cursor
 * \brief Client-side interface to a PixInsight %Cursor object
 *
 * ### TODO: Write a detailed description for %Cursor.
 */
class PCL_CLASS Cursor : public UIObject
{
public:

   /*!
    * Represents a standard cursor shape.
    */
   typedef StdCursor::value_type std_cursor;

   /*!
    * Constructs a %Cursor object with the specified standard cursor \a shape.
    */
   Cursor( std_cursor shape = StdCursor::Arrow );

   /*!
    * Constructs a %Cursor object with the specified \a bmp shape and
    * hot spot coordinates \a hotSpot.
    */
   Cursor( const Bitmap& bmp, const pcl::Point& hotSpot );

   /*!
    * Constructs a %Cursor object with the specified \a bmp shape and
    * hot spot coordinates \a hotSpotX and \a hotSpotY.
    */
   Cursor( const Bitmap& bmp, int hotSpotX = 0, int hotSpotY = 0 );

   /*!
    * Copy constructor. This object will reference the same server-side cursor
    * as the specified instance \a c.
    */
   Cursor( const Cursor& c ) : UIObject( c )
   {
   }

   /*!
    * Destroys a %Cursor object. If this object references an existing cursor
    * in the PixInsight core application, its reference count is decremented.
    * If it becomes unreferenced, it will be garbage-collected.
    */
   virtual ~Cursor()
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side cursor as the specified
    * instance \a c. If the previous cursor becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   Cursor& operator =( const Cursor& c )
   {
      SetHandle( c.handle );
      return *this;
   }

   /*!
    * Returns a reference to a <em>null cursor</em>. A null %Cursor object does
    * not correspond to an existing cursor object in the PixInsight core
    * application.
    */
   static Cursor& Null();

   /*!
    * Returns the hot spot coordinates of this %Cursor object.
    *
    * The hot spot is the <em>active point</em> of the cursor. It is used by
    * the operating system to calculate the current cursor position. Hot spot
    * coordinates are given relative to the upper left corner of the cursor's
    * shape bitmap.
    */
   pcl::Point HotSpot() const;

   /*!
    * Returns the current cursor position in global coordinates.
    */
   static pcl::Point Position();

   /*!
    * Sets the current cursor position to the specified \a x and \a y global
    * coordinates.
    */
   static void SetPosition( int x, int y );

   /*!
    * Sets the current cursor position in global coordinates.
    *
    * This function is equivalent to SetPosition( pos.x, pos.y ).
    */
   static void SetPosition( const pcl::Point& pos )
   {
      SetPosition( pos.x, pos.y );
   }

private:

   Cursor( void* );
   virtual void* CloneHandle() const;

   friend class Control;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Cursor_h

// ****************************************************************************
// EOF pcl/Cursor.h - Released 2014/10/29 07:34:07 UTC
