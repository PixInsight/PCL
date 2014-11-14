// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Sizer.h - Released 2014/11/14 17:16:34 UTC
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

#ifndef __PCL_Sizer_h
#define __PCL_Sizer_h

/// \file pcl/Sizer.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace Align
 * \brief     Control alignment modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Align::Expand</td>  <td>The item can be expanded to occupy the available space</td></tr>
 * <tr><td>Align::Left</td>    <td>The item aligns to the left side of the available space</td></tr>
 * <tr><td>Align::Top</td>     <td>The item aligns to the top side of the available space</td></tr>
 * <tr><td>Align::Right</td>   <td>The item aligns to the right side of the available space</td></tr>
 * <tr><td>Align::Bottom</td>  <td>The item aligns to the bottom side of the available space</td></tr>
 * <tr><td>Align::Center</td>  <td>The item aligns centered within the available space</td></tr>
 * <tr><td>Align::Default</td> <td>Default alignment, equal to Align::Expand</td></tr>
 * </table>
 */
namespace Align
{
   enum value_type
   {
      Expand,
      Left,
      Top     = Left,
      Right,
      Bottom  = Right,
      Center,
      Default = Expand
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

class PCL_CLASS Control;

// ----------------------------------------------------------------------------

/*!
 * \class Sizer
 * \brief Base class for PixInsight sizer objects.
 *
 * %Sizer allows laying out child controls within their parent controls.
 *
 * ### TODO: Write a detailed description for %Sizer.
 *
 * \sa HorizontalSizer, VerticalSizer
 */
class PCL_CLASS Sizer : public UIObject
{
public:

   /*!
    * Represents the alignment of a %Sizer item.
    */
   typedef Align::value_type  item_alignment;

   /*!
    * Constructs a %Sizer object.
    *
    * \param vertical   If true, this sizer will lay out items vertically. If
    *                   false, items will be laid out horizontally.
    */
   Sizer( bool vertical );

   /*!
    * Destroys a %Sizer object.
    */
   virtual ~Sizer()
   {
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since sizers are unique objects by definition, calling this member
    * function has no effect.
    */
   virtual void SetUnique()
   {
      // Sizers are unique objects by definition
   }

   /*!
    * Returns a reference to a null %Sizer instance. A null %Sizer does not
    * correspond to an existing sizer in the PixInsight core application.
    */
   static Sizer& Null();

   /*!
    * Returns a reference to the parent control of this %Sizer object, or
    * Control::Null() if this sizer has no parent control.
    */
   Control& ParentControl() const;

   /*!
    * Returns true if this %Sizer lays out items vertically.
    */
   bool IsVertical() const;

   /*!
    * Returns true if this %Sizer lays out items horizontally.
    */
   bool IsHorizontal() const
   {
      return !IsVertical();
   }

   /*!
    * Returns the number of items in this %Sizer object.
    */
   int NumberOfItems() const;

   /*!
    * Returns the zero-based index of the specified child %Sizer object \a s in
    * this sizer, or -1 if the \a s sizer does not belong to this %Sizer.
    */
   int IndexOf( const Sizer& s ) const;

   /*!
    * Returns the zero-based index of the specified child control \a c in this
    * sizer, or -1 if the \a c control does not belong to this %Sizer.
    */
   int IndexOf( const Control& c ) const;

   /*!
    * Returns true if the specified child sizer \a s belongs to this %Sizer.
    */
   bool Has( const Sizer& s ) const
   {
      return IndexOf( s ) >= 0;
   }

   /*!
    * Returns true if the specified child control \a c belongs to this %Sizer.
    */
   bool Has( const Control& c ) const
   {
      return IndexOf( c ) >= 0;
   }

   /*!
    * Adds a sizer to this %Sizer object.
    *
    * \param s    The %Sizer object to be added.
    *
    * \param stretchFactor    Indicates how much space the sizer \a s will tend
    *             to occupy within its parent sizer (this sizer), relative to
    *             the rest of sibling sizer items.
    *
    * See the documentation for Add( Control&, int, item_alignment ) for a
    * detailed description.
    *
    * \sa Add( Control&, int, item_alignment ), AddSpacing(), AddStretch(),
    * Insert()
    */
   void Add( Sizer& s, int stretchFactor = 0 );

   /*!
    * Adds a control to this %Sizer object.
    *
    * \param c    The control to be added.
    *
    * \param stretchFactor    Indicates how much space the control \a c will
    *             try to occupy within its parent sizer (this sizer), relative
    *             to the rest of sibling sizer items. A value of zero indicates
    *             that the added control has no particular preference and can
    *             be adapted to the existing space, as necessary/possible.
    *
    * \param align   Item alignment.
    *
    * In the following example:
    *
    * \code
    * PushButton button1;
    * PushButton button2;
    * PushButton button3;
    * // ...
    * Sizer fooSizer;
    * fooSizer.Add( button1, 25 );
    * fooSizer.Add( button2, 50 );
    * fooSizer.Add( button3, 25 );
    * \endcode
    *
    * The button1 control will occupy one half of the total available space for
    * fooSizer, and each of button1 and button2 will occupy 1/4 of the space
    * available in fooSizer. Stretch factors don't have to be expressed as
    * percentages necessarily, although using percentage values is usually more
    * readable. The stretch factors 2, 4, 2 would have the same meaning as 25,
    * 50, 25 in the example above.
    *
    * \sa Add( Sizer&, int ), AddSpacing(), AddStretch(), Insert()
    */
   void Add( Control& c, int stretchFactor = 0, item_alignment align = Align::Default );

   /*!
    * Adds a non-stretchable space of \a size pixels to this %Sizer.
    *
    * \sa AddStretch(), InsertSpacing(), InsertStretch()
    */
   void AddSpacing( int size );

   /*!
    * Adds a stretchable space with the specified stretch factor.
    *
    * See the documentation for Add( Control&, int, item_alignment ) for a
    * detailed description of stretch factors.
    *
    * \sa AddSpacing(), InsertSpacing(), InsertStretch()
    */
   void AddStretch( int stretchFactor = 100 );

   /*!
    * Inserts a sizer in this %Sizer object.
    *
    * \param index   Zero-based index where the sizer \a s will be inserted.
    *
    * \param s       The %Sizer object to be added.
    *
    * \param stretchFactor    Indicates how much space the sizer \a s will tend
    *             to occupy within its parent sizer (this sizer), relative to
    *             the rest of sibling sizer items.
    *
    * See the documentation for Add( Control&, int, item_alignment ) for a
    * detailed description.
    *
    * \sa Insert( int, Control&, int, item_alignment ), InsertSpacing(),
    * InsertStretch(), Add()
    */
   void Insert( int index, Sizer& s, int stretchFactor = 0 );

   /*!
    * Inserts a control in this %Sizer object.
    *
    * \param index   Zero-based index where the control \a c will be inserted.
    *
    * \param c       The control to be added.
    *
    * \param stretchFactor    Indicates how much space the control \a c will
    *             tend to occupy within its parent sizer (this sizer), relative
    *             to the rest of sibling sizer items.
    *
    * \param align   Item alignment.
    *
    * See the documentation for Add( Control&, int, item_alignment ) for a
    * detailed description.
    *
    * \sa Insert( int, Sizer&, int ), InsertSpacing(), InsertStretch(), Add()
    */
   void Insert( int index, Control& c, int stretchFactor = 0, item_alignment align = Align::Default );

   /*!
    * Inserts a non-stretchable space of \a size pixels in this %Sizer.
    *
    * \sa InsertStretch(), AddSpacing(), AddStretch()
    */
   void InsertSpacing( int index, int size );

   /*!
    * Inserts a stretchable space with the specified stretch factor.
    *
    * See the documentation for Add( Control&, int, item_alignment ) for a
    * detailed description of stretch factors.
    *
    * \sa InsertSpacing(), AddSpacing(), AddStretch()
    */
   void InsertStretch( int index, int stretchFactor = 100 );

   /*!
    * Removes the specified sizer \a s from this %Sizer object. If \a s doesn't
    * belong to this sizer, nothing happens.
    */
   void Remove( Sizer& s );

   /*!
    * Removes the specified control \a c from this %Sizer object. If \a c
    * doesn't belong to this sizer, nothing happens.
    */
   void Remove( Control& c );

   /*!
    * Changes the stretch factor for a child sizer \a s belonging to this
    * %Sizer object.
    *
    * See the documentation for Add( Control&, int, item_alignment ) for a
    * detailed description of stretch factors.
    */
   void SetStretchFactor( Sizer& s, int stretch );

   /*!
    * Changes the stretch factor for a child control \a s belonging to this
    * %Sizer object.
    *
    * See the documentation for Add( Control&, int, item_alignment ) for a
    * detailed description of stretch factors.
    */
   void SetStretchFactor( Control& c, int stretch );

   /*!
    * Changes the alignment of a child sizer \a s belonging to this %Sizer
    * object.
    */
   void SetAlignment( Sizer& s, item_alignment align );

   /*!
    * Changes the alignment of a child control \a c belonging to this %Sizer
    * object.
    */
   void SetAlignment( Control& c, item_alignment align );

   /*!
    * Returns the margin in pixels around this sizer.
    *
    * The margin is the empty space that is automatically reserved around
    * this sizer on its parent control's (or parent sizer's) client area.
    * The default margin is always zero pixels for all sizer types, unless
    * explicitly changed with this function.
    *
    * \sa SetMargin(), Spacing(), SetSpacing()
    */
   int Margin() const;

   /*!
    * Sets the margin in pixels around this sizer.
    *
    * \sa Margin(), Spacing(), SetSpacing()
    */
   void SetMargin( int margin );

   /*!
    * Returns the spacing in pixels between items in this %Sizer object.
    *
    * \sa SetSpacing(), Margin(), SetMargin()
    */
   int Spacing() const;

   /*!
    * Sets the spacing in pixels between items in this %Sizer object.
    *
    * \sa Spacing(), Margin(), SetMargin()
    */
   void SetSpacing( int margin );

   /*!
    * Returns true if <em>auto scaling</em> is enabled for this sizer.
    *
    * The auto scaling feature adjusts horizontal and vertical spaces between
    * items automatically, depending on the current platform and on the style
    * and GUI theme selected in the PixInsight core application. Auto
    * scaling is applied to both automatic and manual spacings between
    * controls, that is to spacings defined by SetSpacing(), AddSpacing() or
    * InsertSpacing().
    *
    * Auto scaling is enabled by default in all %Sizer objects. Currently
    * this feature is only applied in Mac OS X versions of the PixInsight
    * platform, but it may be used on all platforms in future versions.
    *
    * If you need to ensure precise distances in pixels between controls, call
    * DisableAutoScaling() \e before calling SetSpacing(), or before defining
    * manual spacings with AddSpacing() or InsertSpacing().
    *
    * \sa EnableAutoScaling(), DisableAutoScaling(), SetSpacing(),
    * AddSpacing(), InsertSpacing()
    */
   bool IsAutoScalingEnabled() const;

   /*!
    * Enables (or disables) <em>auto scaling</em> for this sizer.
    *
    * For a description of the auto scaling sizer feature, see the
    * documentation for the IsAutoScalingEnabled() member function.
    *
    * \sa DisableAutoScaling(), IsAutoScalingEnabled()
    */
   void EnableAutoScaling( bool enable = true );

   /*!
    * Disables (or enables) <em>auto scaling</em> for this sizer.
    *
    * For a description of the auto scaling sizer feature, see the
    * documentation for the IsAutoScalingEnabled() member function.
    *
    * \sa EnableAutoScaling(), IsAutoScalingEnabled()
    */
   void DisableAutoScaling( bool disable = true )
   {
      EnableAutoScaling( !disable );
   }

private:

   Sizer( void* );
   virtual void* CloneHandle() const;

   friend class Control;
};

// ----------------------------------------------------------------------------

/*!
 * \class HorizontalSizer
 * \brief Client-side interface to a PixInsight horizontal sizer.
 *
 * %HorizontalSizer lays out a sequence of items horizontally.
 *
 * ### TODO: Write a detailed description for %HorizontalSizer.
 */
class PCL_CLASS HorizontalSizer : public Sizer
{
public:

   /*!
    */
   typedef Sizer::item_alignment item_alignment;

   /*!
    * Constructs a %HorizontalSizer object.
    */
   HorizontalSizer() : Sizer( false )
   {
   }

   /*!
    * Destroys a %HorizontalSizer object.
    */
   virtual ~HorizontalSizer()
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class VerticalSizer
 * \brief Client-side interface to a PixInsight vertical sizer.
 *
 * %VerticalSizer lays out a sequence of items vertically.
 *
 * ### TODO: Write a detailed description for %VerticalSizer.
 */
class PCL_CLASS VerticalSizer : public Sizer
{
public:

   /*!
    */
   typedef Sizer::item_alignment item_alignment;

   /*!
    * Constructs a %VerticalSizer object.
    */
   VerticalSizer() : Sizer( true )
   {
   }

   /*!
    * Destroys a %VerticalSizer object.
    */
   virtual ~VerticalSizer()
   {
   }
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Sizer_h

// ****************************************************************************
// EOF pcl/Sizer.h - Released 2014/11/14 17:16:34 UTC
