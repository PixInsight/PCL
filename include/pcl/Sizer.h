//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/Sizer.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Sizer_h
#define __PCL_Sizer_h

/// \file pcl/Sizer.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/UIObject.h>

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::Align
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
 * %Sizer allows laying out child controls within their parent controls as
 * simple one-dimensional stacks with precise control over orientations,
 * margins, distances between items, and item alignments. By distributing
 * controls in nested %Sizer objects with horizontal and vertical orientations,
 * complex albeit clean user interfaces can be easily defined.
 *
 * \b Automatic \b Scaling
 *
 * The %Sizer class provides an automatic scaling feature that simplifies the
 * adaptation of existing and new PCL-based UI generation code to varying
 * display resolutions. This functionality can be controlled with a Boolean
 * \a autoScaling parameter available for the following member functions:
 *
 * Sizer::SetMargin( int margin, bool autoScaling = true );\n
 * Sizer::SetSpacing( int spacing, bool autoScaling = true );\n
 * Sizer::AddSpacing( int spacing, bool autoScaling = true );\n
 * Sizer::InsertSpacing( int index, int spacing, bool autoScaling = true );\n
 * int Margin( bool autoScaling = true );\n
 * int Spacing( bool autoScaling = true );
 *
 * As you can see, automatic scaling is always enabled by default. On the
 * PixInsight platform, the reference display density is 109 dpi, corresponding
 * to a 27 inch monitor at QHD resolution (2560x1440 physical pixels).
 *
 * The automatic scaling feature applies the scaling ratio returned by
 * Sizer::DisplayPixelRatio() to convert horizontal and vertical spaces between
 * items from device-independent (or \e logical) pixel units to physical device
 * pixel units. When this feature is enabled through the corresponding
 * \a autoScaling parameters, scaling is applied to both automatic and manual
 * spacings between controls, that is, to spacings defined by SetSpacing(),
 * SetMargin(), AddSpacing(), and InsertSpacing(). The reverse transformation
 * is applied from physical pixels to logical units when dimensions are
 * retrieved by Spacing() and Margin().
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
   void EnsureUnique() override
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
    * Returns true iff this %Sizer lays out items vertically.
    */
   bool IsVertical() const;

   /*!
    * Returns true iff this %Sizer lays out items horizontally.
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
    * Returns true iff the specified child sizer \a s belongs to this %Sizer.
    */
   bool Contains( const Sizer& s ) const
   {
      return IndexOf( s ) >= 0;
   }

   /*!
    * Returns true iff the specified child control \a c belongs to this %Sizer.
    */
   bool Contains( const Control& c ) const
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
    * If the \a autoScaling parameter is set to true, the specified \a size
    * will be converted from logical device-independent pixel units to physical
    * device pixels automatically. If \a autoScaling is false, the specified
    * \a size is assumed to be expressed in physical pixel units, and no
    * automatic scaling will be performed.
    *
    * \sa InsertSpacing(), AddStretch(), InsertStretch()
    */
   void AddSpacing( int size, bool autoScaling = true );

   /*!
    * Adds a non-stretchable space of \a size logical pixels to this %Sizer.
    * This is a convenience member function equivalent to:
    *
    * \code AddSpacing( size, true ); \endcode
    */
   void AddScaledSpacing( int size )
   {
      AddSpacing( size, true );
   }

   /*!
    * Adds a non-stretchable space of \a size physical pixels to this %Sizer.
    * This is a convenience member function equivalent to:
    *
    * \code AddSpacing( size, false ); \endcode
    */
   void AddUnscaledSpacing( int size )
   {
      AddSpacing( size, false );
   }

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
    * If the \a autoScaling parameter is set to true, the specified \a size
    * will be converted from logical device-independent pixel units to physical
    * device pixels automatically. If \a autoScaling is false, the specified
    * \a size is assumed to be expressed in physical pixel units, and no
    * automatic scaling will be performed.
    *
    * \sa AddSpacing(), InsertStretch(), AddStretch()
    */
   void InsertSpacing( int index, int size, bool autoScaling = true );

   /*!
    * Inserts a non-stretchable space of \a size logical pixels in this %Sizer.
    * This is a convenience member function equivalent to:
    *
    * \code InsertSpacing( index, size, true ); \endcode
    */
   void InsertScaledSpacing( int index, int size )
   {
      InsertSpacing( index, size, true );
   }

   /*!
    * Inserts a non-stretchable space of \a size physical pixels in this
    * %Sizer. This is a convenience member function equivalent to:
    *
    * \code InsertSpacing( index, size, false ); \endcode
    */
   void InsertUnscaledSpacing( int index, int size )
   {
      InsertSpacing( index, size, false );
   }

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
    * If the \a autoScaling parameter is set to true, the returned value will
    * be expressed in logical device-independent pixel units. This means that
    * the returned value has been converted from physical device pixels
    * automatically. If \a autoScaling is false, the returned value will be
    * expressed in physical device pixels, and no automatic scaling is applied.
    *
    * \sa SetMargin(), Spacing(), SetSpacing()
    */
   int Margin( bool autoScaling = true ) const;

   /*!
    * Sets the margin in pixels around this sizer.
    *
    * If the \a autoScaling parameter is set to true, the specified \a margin
    * will be converted from logical device-independent pixel units to physical
    * device pixels automatically. If \a autoScaling is false, the specified
    * \a margin is assumed to be expressed in physical pixel units, and no
    * automatic scaling will be performed.
    *
    * \sa Margin(), Spacing(), SetSpacing()
    */
   void SetMargin( int margin, bool autoScaling = true );

   /*!
    * Returns the spacing in pixels between items in this %Sizer object.
    *
    * If the \a autoScaling parameter is set to true, the returned value will
    * be expressed in logical device-independent pixel units. This means that
    * the returned value has been converted from physical device pixels
    * automatically. If \a autoScaling is false, the returned value will be
    * expressed in physical device pixels, and no automatic scaling is applied.
    *
    * \sa SetSpacing(), Margin(), SetMargin()
    */
   int Spacing( bool autoScaling = true ) const;

   /*!
    * Sets the spacing in pixels between items in this %Sizer object.
    *
    * If the \a autoScaling parameter is set to true, the specified \a spacing
    * will be converted from logical device-independent pixel units to physical
    * device pixels automatically. If \a autoScaling is false, the specified
    * \a spacing is assumed to be expressed in physical pixel units, and no
    * automatic scaling will be performed.
    *
    * \sa Spacing(), Margin(), SetMargin()
    */
   void SetSpacing( int spacing, bool autoScaling = true );

   /*!
    * Returns the ratio between physical screen pixels and device-independent
    * logical units for the parent control of this %Sizer object. This ratio is
    * used as a scaling factor by the LogicalPixelsToPhysical() and PhysicalPixelsToLogical() functions, and
    * is also applied automatically when \a autoScaling parameters are set to
    * true in a number of member functions; see the class description for more
    * information.
    *
    * The returned value is greater than or equal to one. Typical pixel ratios
    * are 2.0 for high-dpi displays such as Retina monitors, or 1.0 for normal
    * 96 dpi monitors.
    *
    * \sa LogicalPixelsToPhysical(), PhysicalPixelsToLogical()
    */
   double DisplayPixelRatio() const;

   /*!
    * Returns the specified \a size in logical device-independent pixel units
    * converted to physical device pixels.
    *
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
    * \sa DisplayPixelRatio(), LogicalPixelsToPhysical()
    */
   int PhysicalPixelsToLogical( int size ) const
   {
      return RoundInt( size/DisplayPixelRatio() );
   }

private:

   Sizer( void* h ) : UIObject( h )
   {
   }

   void* CloneHandle() const override;

   friend class Control;
};

// ----------------------------------------------------------------------------

/*!
 * \class HorizontalSizer
 * \brief Client-side interface to a PixInsight horizontal sizer.
 *
 * %HorizontalSizer lays out a sequence of items as a horizontal stack.
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
 * %VerticalSizer lays out a sequence of as a vertical stack.
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

// ----------------------------------------------------------------------------
// EOF pcl/Sizer.h - Released 2019-01-21T12:06:07Z
