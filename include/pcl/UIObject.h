//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/UIObject.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_UIObject_h
#define __PCL_UIObject_h

/// \file pcl/UIObject.h

#include <pcl/Defs.h>

#include <pcl/Relational.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class UIObject
 * \brief Root base class for all user interface objects.
 *
 * %UIObject is a high-level, managed client-side interface to a server-side
 * object living in the PixInsight core application. %UIObject is the root base
 * class of all PCL classes representing objects that can be constructed and
 * destroyed in the PixInsight platform, such as most user interface elements,
 * file format instances, and process instances.
 *
 * For example, Bitmap and ProcessInstance are derived classes of %UIObject
 * because you can generate new instances of the server-side objects
 * represented by these classes. Contrarily, FileFormat and Process are not
 * %UIObject descendants because these classes represent installed components
 * that cannot be created or modified by a module.
 *
 * All the server-side objects represented by %UIObject and its derived classes
 * are <em>reference counted</em> objects. When you create an instance of
 * %UIObject (or a derived class) in your module, you either generate a new
 * object in the platform, or increase the reference counter of an already
 * existing object. When a server-side object becomes unreferenced, that is,
 * when no living %UIObject represents it, it is either destroyed or recycled
 * by the PixInsight core application as soon as possible, in order to reuse
 * and optimize the available resources. This process is known as <em>garbage
 * collection</em>, and works in a completely automatic and transparent fashion
 * in the PixInsight platform.
 */
class PCL_CLASS UIObject
{
public:

   /*!
    * Destroys a %UIObject instance.
    *
    * After destruction of this %UIObject, the server-side object is also
    * destroyed or recycled if it is no longer referenced by other high-level
    * interfaces (e.g., other instances of %UIObject in this or other modules).
    */
   virtual ~UIObject() noexcept( false );

   /*!
    * Returns true iff this is a \e null %UIObject instance.
    *
    * A null %UIObject does not represent an existing object in the PixInsight
    * core application.
    */
   bool IsNull() const
   {
      return handle == nullptr;
   }

   /*!
    * Returns a reference to a null %UIObject instance.
    *
    * A null %UIObject does not represent an existing object in the PixInsight
    * core application.
    */
   static UIObject& Null();

   /*!
    * Returns the total number of existing references to the server-side object
    * managed by this %UIObject.
    *
    * The returned value is the sum of all references to the server-side object
    * on the whole PixInsight platform, including all installed modules and the
    * PixInsight core application.
    */
   size_type RefCount() const;

   /*!
    * Returns true iff the server-side object is uniquely referenced by this
    * %UIObject instance.
    *
    * A uniquely referenced server-side object will be destroyed or recycled
    * upon destruction of this %UIObject.
    */
   bool IsUnique() const
   {
      return RefCount() < 2;
   }

   /*!
    * Returns true iff this %UIObject instance is an alias to an existing object
    * in the calling module.
    *
    * An alias and its aliased object are interchangeable; other than
    * module-specific functionality, their behaviors are identical since they
    * refer to a unique object living in the PixInsight core application. An
    * alias %UIObject is generated each time a new %UIObject descendant class
    * is instantiated using (explicitly or indirectly) a copy constructor. For
    * example:
    *
    * \code
    * ImageWindow w1( 256, 256 ); // ImageWindow is an UIObject derived class
    * ImageWindow w2 = w1;    // w2 is an alias to w1.
    * bool b1 = w1.IsAlias(); // b1 is now false
    * bool b2 = w2.IsAlias(); // b2 is now true
    * w2.Show(); // OK, shows w1.
    * w1.HideMask();
    * bool b3 = w2.IsMaskVisible(); // b3 is now false
    * w2.Close(); // OK, the window is destroyed and both w1 and w2 are now invalid objects.
    * \endcode
    *
    * The only limit specific to aliased objects is the fact that an alias
    * Control (or descendant) cannot set event handlers. See the documentation
    * for Control( const Control& ) for more information.
    */
   bool IsAlias() const
   {
      return alias;
   }

   /*!
    * Returns true iff this managed object represents an unreferenced
    * server-side object. Such objects are said to be 'garbage' in the
    * programming jargon, and are subject to eventual destruction through a
    * process known as \e garbage \e collection.
    */
   bool IsGarbage() const
   {
      return RefCount() < 1;
   }

   /*!
    * Returns true iff this instance and other %UIObject \a o reference the same
    * server-side object.
    *
    * If two %UIObject instances reference the same server-side object, each of
    * them act as an alias for the other: In all respects, both high-level
    * instances are interchangeable.
    */
   bool IsSameObject( const UIObject& o ) const
   {
      return handle == o.handle;
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * If there are other %UIObject instances that refer to the same server-side
    * object, this member function attempts to create a new server-side object
    * as a copy of the currently managed object, and references it.
    *
    * If the server-side object is only referenced by this %UIObject instance,
    * calling this member function has no effect.
    *
    * Some derived classes represent object types whose instances are unique by
    * nature. In those cases, calling this member function has no effect.
    */
   virtual void EnsureUnique();

   /*!
    * A convenience synonym for IsSameObject( o ).
    */
   bool operator ==( const UIObject& o ) const
   {
      return IsSameObject( o );
   }

   /*!
    * Returns true iff this %UIObject instance precedes another %UIObject \a o.
    * Performs a comparison based on the low-level handle internally maintained
    * by each %UIObject.
    *
    * This member function is actually intended for fast access to large sets
    * of %UIObject instances stored in containers (as for example the whole set
    * of user interface objects defined in a module). It is seldom used
    * explicitly by modules.
    */
   bool operator <( const UIObject& o ) const
   {
      return handle < o.handle;
   }

   /*!
    * Returns a string that uniquely identifies the type of the server-side UI
    * object managed by this %UIObject.
    */
   IsoString ObjectType() const;

   /*!
    * Returns the identifier of this UI object. The object identifier is a
    * text string associated with the server-side UI object managed by this
    * %UIObject instance.
    *
    * When this function is called for the first time for a given object, and
    * no object identifier has been set explicitly in a previous call to
    * SetObjectId(), a new identifier is generated automatically. In this case
    * the generated object identifier is guaranteed to be unique across the
    * entire PixInsight platform.
    *
    * Object identifiers are useful to select individual instances. For
    * example, an object identifier can be used to restrict the scope of an
    * interface style sheet to a particular control. See the documentation for
    * Control::SetStyleSheet() for more information.
    *
    * \sa SetObjectId()
    */
   String ObjectId() const;

   /*!
    * Sets the identifier of this UI object. Forcing an object identifier can
    * be useful to select one or more controls for CSS styling with the
    * Control::SetStyleSheet() member function. See the documentation for
    * ObjectId() for more information.
    *
    * \sa ObjectId()
    */
   void SetObjectId( const String& id );

protected:

   void* handle = nullptr;
   bool  alias  = false;

   /*!
    * Constructs a null %UIObject instance.
    *
    * A null %UIObject does not correspond to an existing object in the
    * PixInsight core application.
    */
   UIObject() = default;

   /*!
    * Copy constructor.
    *
    * Unless a derived class redefines it otherwise (which is rare), this
    * constructor does not create a new object in the PixInsight core
    * application. It simply creates an alias %UIObject instance that
    * references the same server-side object as the source instance \a x.
    */
   UIObject( const UIObject& x );

   /*!
    * Move constructor.
    */
   UIObject( UIObject&& x ) : handle( x.handle ), alias( x.alias )
   {
      x.handle = nullptr;
      x.alias = false;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   UIObject& operator =( const UIObject& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   UIObject& operator =( UIObject&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * \internal
    */
   UIObject( void* );

   /*!
    * \internal
    */
   UIObject( const void* );

   /*!
    * \internal
    */
   UIObject( std::nullptr_t )
   {
   }

   /*!
    * \internal
    */
   void SetHandle( void* );

   /*!
    * \internal
    */
   void TransferHandle( void* );

   /*!
    * \internal
    * This virtual function is called to construct object duplicates. Derived
    * classes must reimplement this function to return the handle of a newly
    * constructed UI object.
    */
   virtual void* CloneHandle() const
   {
      return nullptr;
   }

   /*!
    * \internal
    */
   bool Assign( const UIObject& x )
   {
      if ( x.handle != handle )
      {
         SetHandle( x.handle );
         return true;
      }
      return false;
   }

   /*!
    * \internal
    */
   bool Transfer( UIObject& x )
   {
      if ( &x != this )
      {
         SetHandle( nullptr );
         handle = x.handle;
         alias = x.alias;
         x.handle = nullptr;
         x.alias = false;
         return true;
      }
      return false;
   }

   friend class UIObjectIndex;
   friend class UIEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_UIObject_h

// ----------------------------------------------------------------------------
// EOF pcl/UIObject.h - Released 2017-07-09T18:07:07Z
