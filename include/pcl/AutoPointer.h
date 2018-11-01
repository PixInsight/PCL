//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/AutoPointer.h - Released 2018-11-01T11:06:36Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_AutoPointer_h
#define __PCL_AutoPointer_h

/// \file pcl/AutoPointer.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class StandardDeleter
 * \brief An object deleter that uses the standard delete operator.
 *
 * Deleter objects are used by smart pointer classes (e.g., AutoPointer) to
 * destroy and deallocate dynamically allocated objects. A valid deleter class
 * must implement the following member functions:
 *
 * \li Default constructor.
 *
 * \li Copy constructor.
 *
 * \li Move constructor.
 *
 * \li Function call operator with a single argument of type T* (pointer to T).
 * This member function will destroy and deallocate the T object pointed to by
 * its argument.
 *
 * \li Function call operator with a single argument of type T[] (array of T).
 * This member function will destroy and deallocate a contiguous sequence of
 * objects stored at the location pointed to by its argument.
 *
 * %StandardDeleter implements object and array destruction/deallocation by
 * calling the standard \c delete operator.
 *
 * \sa AutoPointer
 */
template <typename T>
class PCL_CLASS StandardDeleter
{
public:

   /*!
    * Represents the type of objects to destroy and deallocate.
    */
   typedef T         value_type;

   /*!
    * Represents a pointer to an object to destroy and deallocate.
    */

   typedef T*        pointer;

   /*!
    * Function call operator. Destroys and deallocates the object pointed to by
    * the specified pointer \a p.
    */
   void operator()( pointer p ) const
   {
      PCL_PRECONDITION( p != nullptr )
      delete p;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class AutoPointer
 * \brief A smart pointer with sole object ownership and optional automatic
 * object destruction.
 *
 * %AutoPointer stores a pointer to an object of which it is the sole owner.
 * The owned object can optionally be destroyed when the owner %AutoPointer
 * instance is destroyed.
 *
 * The template argument T represents the type of the objects owned by this
 * template instantiation. The template argument D represents a functor class
 * responsible for deletion of objects of type T. By default, %AutoPointer uses
 * the StandardDeleter template class, which is a simple wrapper for the
 * standard \c delete operator.
 *
 * Smart pointers are useful entities to guarantee proper destruction and
 * deallocation of data in a variety of scenarios, such as exception driven
 * code where the same objects have to be destroyed at different points in the
 * execution workflow. For example, consider the following pseudocode:
 *
 * \code
 * struct Foo { ... };
 * void Bar()
 * {
 *    Foo* one = nullptr, * two = nullptr;
 *    try
 *    {
 *       // ... some code that may create new Foo objects ...
 *       if ( condition1 ) one = new Foo;
 *       if ( condition2 ) two = new Foo;
 *       // ... some code that can throw exceptions ...
 *       if ( one != nullptr ) delete one;
 *       if ( two != nullptr ) delete two;
 *    }
 *    catch ( ... )
 *    {
 *       if ( one != nullptr ) delete one;
 *       if ( two != nullptr ) delete two;
 *       throw;
 *    }
 * }
 * \endcode
 *
 * Note that the objects pointed to by the \c one and \c two variables have to
 * be destroyed at two locations: at the bottom of the \c try block (normal
 * execution), and when an exception is caught, within the \c catch block. If
 * the %Bar() routine were more complex, even more deallocations might be
 * necessary at different locations, making the code intricate and prone to
 * memory leaks.
 *
 * All of these complexities and potential problems can be avoided easily with
 * smart pointers. For example, the following snippet would be equivalent to
 * the pseudocode above:
 *
 * \code
 * struct Foo { ... };
 * void Bar()
 * {
 *    AutoPointer<Foo> one, two;
 *    // ... some code that may create new Foo objects ...
 *    if ( condition1 ) one = new Foo;
 *    if ( condition2 ) two = new Foo;
 *    // ... some code that can throw exceptions ...
 * }
 * \endcode
 *
 * With smart pointers, there's no need to explicitly destroy the dynamically
 * allocated objects \c one and \c two: The %AutoPointer objects will destroy
 * and deallocate them automatically when they get out of scope. On the other
 * hand, the %AutoPointer instances behave just like normal pointers allowing
 * indirections, pointer assignments, and structure member selections for their
 * owned objects transparently. The resulting code is simpler and more robust.
 *
 * By default, when an %AutoPointer instance is destroyed it also destroys the
 * object pointed to by its contained pointer (if the %AutoPointer stores a
 * non-null pointer). This <em>automatic deletion</em> feature can be disabled
 * in some situations where a single %AutoPointer can store either a pointer to
 * a dynamically allocated object, or a pointer to an already existing object
 * that must be preserved (e.g., an object living in the stack). For example:
 *
 * \code
 * void Foo( Image& image )
 * {
 *    //
 *    // Extract the CIE L* component of a color image ...
 *    // ... or use the same image as the working lightness if it is grayscale.
 *    //
 *    AutoPointer<Image> lightness;
 *    if ( image.IsColor() )
 *    {
 *       lightness = new Image;
 *       image.ExtractLightness( *lightness );
 *    }
 *    else
 *    {
 *       lightness = &image;
 *       lightness.DisableAutoDelete();
 *    }
 *
 *    DoSomeStuffWithTheLightnessComponent( *lightness );
 *
 *    //
 *    // Insert the modified CIE L* component back in the color image.
 *    //
 *    if ( image.IsColor() )
 *       image.SetLightness( *lightness );
 * }
 * \endcode
 *
 * In the above code, the \c lightness variable can either store a newed image,
 * if the passed \a image is a color image, or a pointer to \a image if it is a
 * grayscale image. In the latter case we have disabled the automatic deletion
 * feature for the \c lightness %AutoPointer, so it won't delete its stored
 * pointer when it gets out of scope.
 *
 * \sa StandardDeleter, AutoPointerCloner
 */
template <class T, class D = StandardDeleter<T> >
class PCL_CLASS AutoPointer
{
public:

   /*!
    * Represents the type of the object pointed to by this smart pointer.
    */
   typedef T         value_type;

   /*!
    * Represents a pointer stored in this smart pointer.
    */
   typedef T*        pointer;

   /*!
    * Represents a pointer to an immutable object stored in this smart pointer.
    */
   typedef const T*  const_pointer;

   /*!
    * Represents the type of the object responsible for object deletion.
    */
   typedef D         deleter;

   /*!
    * Constructs a null smart pointer.
    *
    * \param autoDelete    Initial state of the automatic deletion feature. The
    *                      default value is true, so auto deletion is always
    *                      enabled by default for newly created %AutoPointer
    *                      objects.
    *
    * \param d             Deleter object, responsible for object destruction
    *                      when the automatic deletion feature is enabled.
    *
    * A null smart pointer stores a null pointer, so it does not point to a
    * valid object.
    *
    * A copy of the specified deleter \a d will be used. If no deleter is
    * specified, this object will use a default-constructed instance of the
    * deleter template argument class.
    */
   AutoPointer( bool autoDelete = true, const deleter& d = deleter() ) :
      m_pointer( nullptr ),
      m_deleter( d ),
      m_autoDelete( autoDelete )
   {
   }

   /*!
    * Constructs a smart pointer to store a given pointer.
    *
    * \param p             The pointer to store in this %AutoPointer instance.
    *
    * \param autoDelete    Initial state of the automatic deletion feature. The
    *                      default value is true, so auto deletion is always
    *                      enabled by default for newly created %AutoPointer
    *                      objects.
    *
    * \param d             Deleter object, responsible for object destruction
    *                      when the automatic deletion feature is enabled.
    *
    * A copy of the specified deleter \a d will be used. If no deleter is
    * specified, this object will use a default-constructed instance of the
    * deleter template argument class.
    */
   AutoPointer( pointer p, bool autoDelete = true, const deleter& d = deleter() ) :
      m_pointer( nullptr ),
      m_deleter( d ),
      m_autoDelete( autoDelete )
   {
      m_pointer = p;
   }

   /*!
    * Non-trivial copy constructor. Constructs a smart pointer by transferring
    * the pointer stored in another smart pointer \a x.
    *
    * The automatic deletion feature for this object will be in the same state
    * as it is currently set for the source object \a x.
    *
    * The smart pointer \a x will be a null pointer after this instance is
    * constructed. This happens irrespective of the state of the automatic
    * deletion feature. This guarantees that, as long as no two %AutoPointer
    * instances have been \e explicitly constructed to store the same pointer,
    * no two %AutoPointer instances can share the same pointer accidentally,
    * and hence multiple deletions are not possible.
    */
   AutoPointer( AutoPointer& x ) :
      m_pointer( x.Release() ),
      m_deleter( x.m_deleter ),
      m_autoDelete( x.m_autoDelete )
   {
   }

   /*!
    * Move constructor.
    */
   AutoPointer( AutoPointer&& x ) :
      m_pointer( x.Release() ),
      m_deleter( std::move( x.m_deleter ) ),
      m_autoDelete( x.m_autoDelete )
   {
   }

   /*!
    * Destroys an %AutoPointer object.
    *
    * If this instance stores a non-null pointer, and the automatic deletion
    * feature is enabled, the pointed object will be destroyed by calling the
    * deleter object.
    */
   virtual ~AutoPointer()
   {
      Reset();
   }

   /*!
    * Causes this smart pointer to store the specified pointer \a p.
    *
    * If this instance stores a non-null pointer when this function is called,
    * and the automatic deletion feature is enabled, the pointed object is
    * destroyed by calling the deleter object.
    */
   void SetPointer( pointer p )
   {
      if ( m_pointer != p )
      {
         if ( m_autoDelete )
            if ( m_pointer != nullptr )
               m_deleter( Release() ); // in case m_deleter throws
         m_pointer = p;
      }
   }

   /*!
    * Causes this smart pointer to store a null pointer.
    *
    * If this instance stores a non-null pointer when this function is called,
    * and the automatic deletion feature is enabled, the pointed object is
    * destroyed by calling the deleter object.
    *
    * This member function is functionally equivalent to SetPointer( nullptr ).
    */
   void Reset()
   {
      if ( m_pointer != nullptr )
      {
         pointer p = Release(); // in case m_deleter throws
         if ( m_autoDelete )
            m_deleter( p );
      }
   }

   /*!
    * A synonym for Reset(). Useful to enforce semantics when the smart pointer
    * owns the pointed object.
    */
   void Destroy()
   {
      Reset();
   }

   /*!
    * Returns the pointer stored in this %AutoPointer, and causes this object
    * to \e forget it by storing a null pointer.
    *
    * The object pointed is never destroyed by this function, irrespective of
    * the state of automatic deletion. In this way, ownership of the pointed
    * object (that is, the responsibility for destroying it) is transferred to
    * the caller.
    */
   pointer Release()
   {
      pointer p = m_pointer;
      m_pointer = nullptr;
      return p;
   }

   /*!
    * Returns a pointer to the immutable object pointed to by this %AutoPointer
    * instance.
    */
   const_pointer Pointer() const
   {
      return m_pointer;
   }

   /*!
    * Returns a copy of the pointer stored in this %AutoPointer instance.
    */
   pointer Pointer()
   {
      return m_pointer;
   }

   /*!
    * A synonym for Pointer() const.
    */
   const_pointer Ptr() const
   {
      return m_pointer;
   }

   /*!
    * A synonym for Pointer().
    */
   pointer Ptr()
   {
      return m_pointer;
   }

   /*!
    * Returns true iff this smart pointer object stores a null pointer.
    */
   bool IsNull() const
   {
      return m_pointer == nullptr;
   }

   /*!
    * Returns true iff this smart pointer object stores a non-null pointer.
    * Equivalent to !IsNull().
    */
   bool IsValid() const
   {
      return !IsNull();
   }

   /*!
    * Returns true iff the automatic deletion feature of %AutoPointer is
    * currently enabled for this object.
    *
    * When automatic deletion is enabled, the object pointed to by this
    * instance will be destroyed (by calling the deleter object) when this
    * instance is destroyed, or when it is assigned with a different pointer.
    *
    * When automatic deletion is disabled, the pointed object will not be
    * destroyed automatically.
    *
    * See the detailed description for the %AutoPointer class for more
    * information, including code examples.
    *
    * \sa EnableAutoDelete(), DisableAutoDelete()
    */
   bool IsAutoDelete() const
   {
      return m_autoDelete;
   }

   /*!
    * Enables (or disables) the automatic deletion feature of %AutoPointer for
    * this object.
    *
    * \sa IsAutoDelete(), DisableAutoDelete()
    */
   void EnableAutoDelete( bool enable = true )
   {
      m_autoDelete = enable;
   }

   /*!
    * Disables (or enables) the automatic deletion feature of %AutoPointer for
    * this object.
    *
    * \sa IsAutoDelete(), EnableAutoDelete()
    */
   void DisableAutoDelete( bool disable = true )
   {
      EnableAutoDelete( !disable );
   }

   /*!
    * Returns a reference to the immutable deleter object in this instance.
    */
   const deleter& Deleter() const
   {
      return m_deleter;
   }

   /*!
    * Returns a reference to the deleter object in this instance.
    */
   deleter& Deleter()
   {
      return m_deleter;
   }

   /*!
    * Copy assignment operator. Transfers the pointer stored in another smart
    * pointer to this object.
    *
    * This assignment operator performs the following actions:
    *
    * (1) If this smart pointer stores a valid (non-null) pointer, and the
    * automatic deletion feature is enabled, the pointed object is destroyed by
    * the deleter object.
    *
    * (2) The pointer stored in the other smart pointer \a x is copied to this
    * instance.
    *
    * (3) The other smart pointer \a x is forced to store a null pointer.
    *
    * (4) Returns a reference to this object.
    *
    * This operator function does nothing if the specified %AutoPointer \a x
    * stores the same pointer as this object. This prevents multiple deletions.
    */
   AutoPointer& operator =( AutoPointer& x )
   {
      SetPointer( x.Release() );
      m_deleter = x.m_deleter;
      m_autoDelete = x.m_autoDelete;
      return *this;
   }

   /*!
    * Move assignment operator. For the %AutoPointer class, this member
    * function performs the same actions as the copy assignment operator.
    */
   AutoPointer& operator =( AutoPointer&& x )
   {
      SetPointer( x.Release() );
      m_deleter = std::move( x.m_deleter );
      m_autoDelete = x.m_autoDelete;
      return *this;
   }

   /*!
    * Causes this smart pointer to store the specified pointer \a p. Returns a
    * reference to this object.
    *
    * If this instance stores a non-null pointer when this function is called,
    * and the automatic deletion feature is enabled, the pointed object is
    * destroyed by the deleter object.
    *
    * If this object already stores the specified pointer \a p, this function
    * does nothing.
    *
    * This member function is equivalent to:
    *
    * \code
    * SetPointer( p );
    * \endcode
    */
   AutoPointer& operator =( pointer p )
   {
      SetPointer( p );
      return *this;
   }

   /*!
    * Returns a pointer to the immutable object pointed to by this instance.
    *
    * This operator is a synonym for the Pointer() const member function.
    */
   operator const_pointer() const
   {
      return m_pointer;
   }

   /*!
    * Returns a copy of the pointer stored in this %AutoPointer instance.
    *
    * This operator is a synonym for the Pointer() member function.
    */
   operator pointer()
   {
      return m_pointer;
   }

   /*!
    * Structure member selection operator. Returns a pointer to the immutable
    * object pointed to by this %AutoPointer instance.
    */
   const_pointer operator ->() const
   {
      PCL_PRECONDITION( m_pointer != nullptr )
      return m_pointer;
   }

   /*!
    * Structure member selection operator. Returns a copy of the pointer stored
    * in this %AutoPointer instance.
    */
   pointer operator ->()
   {
      PCL_PRECONDITION( m_pointer != nullptr )
      return m_pointer;
   }

   /*!
    * Dereference operator. Returns a reference to the immutable object pointed
    * to by this smart pointer.
    */
   const value_type& operator *() const
   {
      PCL_PRECONDITION( m_pointer != nullptr )
      return *m_pointer;
   }

   /*!
    * Dereference operator. Returns a reference to the object pointed to by
    * this smart pointer.
    */
   value_type& operator *()
   {
      PCL_PRECONDITION( m_pointer != nullptr )
      return *m_pointer;
   }

   /*!
    * Returns true iff this smart pointer stores a non-null pointer. This
    * operator is equivalent to !IsNull().
    */
   operator bool() const
   {
      return !IsNull();
   }

   /*!
    * Exchanges two smart pointers \a x1 and \a x2.
    */
   friend void Swap( AutoPointer& x1, AutoPointer& x2 )
   {
      pcl::Swap( x1.m_pointer, x2.m_pointer );
      pcl::Swap( x1.m_deleter, x2.m_deleter );
      bool b = x1.m_autoDelete; x1.m_autoDelete = x2.m_autoDelete; x2.m_autoDelete = b;
   }

protected:

   pointer m_pointer;
   deleter m_deleter;
   bool    m_autoDelete : 1;
};

// ----------------------------------------------------------------------------

#define ASSERT_COPIABLE_T() \
   static_assert( std::is_copy_constructible<T>::value, "AutoPointerCloner<> requires a copy-constructible type." )

/*!
 * \class AutoPointerCloner
 * \brief A smart pointer able to clone the objects pointed to by other smart
 * pointers.
 *
 * %AutoPointerCloner is like AutoPointer, from which it derives publicly, with
 * the only and substantial difference that it makes a dynamically allocated
 * copy (or \e clone) of the object pointed to by another smart pointer upon
 * copy construction or copy assignment. This is useful in cases where a
 * dynamically allocated data member object must be duplicated automatically by
 * the copy constructors and copy assignment operator of its base class.
 *
 * \sa AutoPointer
 */
template <class T, class D = StandardDeleter<T> >
class PCL_CLASS AutoPointerCloner : public AutoPointer<T,D>
{
public:

   typedef AutoPointer<T,D>                  base_type;

   /*!
    * Represents the type of the object pointed to by this smart pointer.
    */
   typedef typename base_type::value_type    value_type;

   /*!
    * Represents a pointer stored in this smart pointer.
    */
   typedef typename base_type::pointer       pointer;

   /*!
    * Represents a pointer to an immutable object stored in this smart pointer.
    */
   typedef typename base_type::const_pointer const_pointer;

   /*!
    * Represents the type of the object responsible for object deletion.
    */
   typedef typename base_type::deleter       deleter;

   /*!
    * Constructs a null smart pointer cloner.
    *
    * \param autoDelete    Initial state of the automatic deletion feature. The
    *                      default value is true, so auto deletion is always
    *                      enabled by default for newly created %AutoPointer
    *                      objects.
    *
    * \param d             Deleter object, responsible for object destruction
    *                      when the automatic deletion feature is enabled.
    *
    * A null smart pointer stores a null pointer, so it does not point to a
    * valid object.
    *
    * A copy of the specified deleter \a d will be used. If no deleter is
    * specified, this object will use a default-constructed instance of the
    * deleter template argument class.
    */
   AutoPointerCloner( bool autoDelete = true, const deleter& d = deleter() ) :
      base_type( autoDelete, d )
   {
      ASSERT_COPIABLE_T();
   }

   /*!
    * Constructs a smart pointer cloner to store a given pointer.
    *
    * \param p             The pointer to store in this %AutoPointer instance.
    *
    * \param autoDelete    Initial state of the automatic deletion feature. The
    *                      default value is true, so auto deletion is always
    *                      enabled by default for newly created %AutoPointer
    *                      objects.
    *
    * \param d             Deleter object, responsible for object destruction
    *                      when the automatic deletion feature is enabled.
    *
    * A copy of the specified deleter \a d will be used. If no deleter is
    * specified, this object will use a default-constructed instance of the
    * deleter template argument class.
    */
   AutoPointerCloner( pointer p, bool autoDelete = true, const deleter& d = deleter() ) :
      base_type( p, autoDelete, d )
   {
      ASSERT_COPIABLE_T();
   }

   /*!
    * Non-trivial copy constructor. Constructs a smart pointer cloner by making
    * a dynamically allocated copy (or \e clone) of the object pointed to by
    * the pointer stored in another smart pointer \a x.
    *
    * The automatic deletion feature will be enabled for this object if a clone
    * object is generated; otherwise it will be in the same state as it is
    * currently set for the source object \a x.
    *
    * Contrarily to the copy constructor of AutoPointer, the source smart
    * pointer \a x will not be modified in any way by this constructor.
    */
   AutoPointerCloner( const base_type& x ) : base_type( nullptr )
   {
      ASSERT_COPIABLE_T();
      this->m_pointer = x ? new value_type( *x ) : nullptr;
      this->m_deleter = x.m_deleter;
      this->m_autoDelete = x || x.m_autoDelete;
   }

   /*!
    * Copy constructor. Constructs a smart pointer cloner by making a
    * dynamically allocated copy (or \e clone) of the object pointed to by
    * the pointer stored in another smart pointer cloner \a x.
    *
    * The automatic deletion feature will be enabled for this object if a clone
    * object is generated; otherwise it will be in the same state as it is
    * currently set for the source object \a x.
    *
    * Contrarily to the copy constructor of AutoPointer, the source smart
    * pointer \a x will not be modified in any way by this constructor.
    */
   AutoPointerCloner( const AutoPointerCloner& x ) : base_type( nullptr )
   {
      ASSERT_COPIABLE_T();
      this->m_pointer = x ? new value_type( *x ) : nullptr;
      this->m_deleter = x.m_deleter;
      this->m_autoDelete = x || x.m_autoDelete;
   }

   /*!
    * Move constructor.
    */
   AutoPointerCloner( base_type&& x ) : base_type( std::move( x ) )
   {
   }

   /*!
    * Copy assignment from base class operator.
    *
    * This assignment operator performs the following actions:
    *
    * (1) If this smart pointer stores a valid (non-null) pointer, and the
    * automatic deletion feature is enabled, the pointed object is destroyed by
    * the deleter object. This behavior is the same as for AutoPointer.
    *
    * (2) If the other smart pointer \a x stores a valid (non-null) pointer,
    * a dynamically allocated copy (or \e clone) of the pointed object will be
    * generated, and a pointer to the newly constructed object will be stored
    * in this instance. In such case, the automatic deletion feature will be
    * enabled for this object; otherwise, it will have the same state as in the
    * source object \a x, and this object will store a null pointer.
    *
    * (3) Returns a reference to this object.
    *
    * In contrast to AutoPointer's copy assignment operator, this operator does
    * not modify the specified source object \a x in any way.
    */
   AutoPointerCloner& operator =( const base_type& x )
   {
      this->SetPointer( x ? new value_type( *x ) : nullptr );
      this->m_deleter = x.m_deleter;
      this->m_autoDelete = x || x.m_autoDelete;
      return *this;
   }

   /*!
    * Copy assignment operator.
    *
    * This assignment operator performs the following actions:
    *
    * (1) If this smart pointer stores a valid (non-null) pointer, and the
    * automatic deletion feature is enabled, the pointed object is destroyed by
    * the deleter object. This behavior is the same as for AutoPointer.
    *
    * (2) If the other smart pointer \a x stores a valid (non-null) pointer,
    * a dynamically allocated copy (or \e clone) of the pointed object will be
    * generated, and a pointer to the newly constructed object will be stored
    * in this instance. In such case, the automatic deletion feature will be
    * enabled for this object; otherwise, it will have the same state as in the
    * source object \a x, and this object will store a null pointer.
    *
    * (3) Returns a reference to this object.
    *
    * In contrast to AutoPointer's copy assignment operator, this operator does
    * not modify the specified source object \a x in any way.
    */
   AutoPointerCloner& operator =( const AutoPointerCloner& x )
   {
      this->SetPointer( x ? new value_type( *x ) : nullptr );
      this->m_deleter = x.m_deleter;
      this->m_autoDelete = x || x.m_autoDelete;
      return *this;
   }

   /*!
    * Move assignment operator. This function simply calls the base class
    * version of the same operator and returns a reference to this object.
    */
   AutoPointerCloner& operator =( base_type&& x )
   {
      (void)base_type::operator =( std::move( x ) );
      return *this;
   }

   /*!
    * Causes this smart pointer cloner to store the specified pointer \a p.
    * Returns a reference to this object.
    *
    * This member function is identical to its base class counterpart
    * AutoPointer::operator =( pointer ). It is equivalent to:
    *
    * \code SetPointer( p ); \endcode
    */
   AutoPointerCloner& operator =( pointer p )
   {
      this->SetPointer( p );
      return *this;
   }
};

#undef ASSERT_COPIABLE_T

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_AutoPointer_h

// ----------------------------------------------------------------------------
// EOF pcl/AutoPointer.h - Released 2018-11-01T11:06:36Z
