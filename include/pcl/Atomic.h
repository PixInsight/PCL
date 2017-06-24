//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/Atomic.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_Atomic_h
#define __PCL_Atomic_h

/// \file pcl/Atomic.h

#include <pcl/Defs.h>

#ifdef __PCL_WINDOWS
# include <intrin.h>
# pragma intrinsic (_InterlockedIncrement)
# pragma intrinsic (_InterlockedDecrement)
# pragma intrinsic (_InterlockedCompareExchange)
# pragma intrinsic (_InterlockedExchange)
# pragma intrinsic (_InterlockedExchangeAdd)
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class AtomicInt
 * \brief Atomic operations on integers.
 *
 * %AtomicInt allows non-blocking synchronization of multithreaded code with
 * respect to reference counting and other critical operations in parallel
 * algorithm implementations. This class is used extensively by PCL code to
 * implement copy-on-write shared containers and container operations in a
 * thread-safe way. An example is the ReferenceCounter class, which is at the
 * hearth of most PCL container and image classes.
 *
 * %AtomicInt implements the following synchronization primitives on integers:
 *
 * reference \n
 * dereference \n
 * test-and-set \n
 * fetch-and-store \n
 * fetch-and-add
 *
 * \sa ReferenceCounter
 */
class PCL_CLASS AtomicInt
{
public:

   /*!
    * Constructs an %AtomicInt instance with the specified \a value. When not
    * explicitly specified, the default value is zero.
    */
   AtomicInt( int value = 0 ) :
      m_value( value )
   {
      // ### N.B.:
      // The default zero initialization is *critical* - DO NOT change it.
   }

   /*!
    * Copy constructor.
    */
   AtomicInt( const AtomicInt& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   AtomicInt& operator =( const AtomicInt& ) = default;

   /*!
    * Returns the current value of this atomic integer.
    *
    * \note This operation is not guaranteed to be atomic.
    */
   operator int() const
   {
      return m_value;
   }

   /*!
    * Logical negation operator. Returns true iff this atomic integer is zero.
    *
    * \note This operation is not guaranteed to be atomic.
    */
   bool operator !() const
   {
      return m_value == 0;
   }

   /*!
    * Equality operator. Returns true iff this atomic integer is equal to an
    * integer \a x.
    *
    * \note This operation is not guaranteed to be atomic.
    */
   bool operator ==( int x ) const
   {
      return m_value == x;
   }

   /*!
    * Inequality operator. Returns true iff this atomic integer is not equal to
    * an integer \a x.
    *
    * \note This operation is not guaranteed to be atomic.
    */
   bool operator !=( int x ) const
   {
      return m_value != x;
   }

   /*!
    * Integer assignment operator. Assigns the specified integer \a x to this
    * atomic integer. Returns a reference to this object.
    *
    * \note This operation is not guaranteed to be atomic.
    */
   AtomicInt& operator =( int x )
   {
      m_value = x;
      return *this;
   }

   /*!
    * Atomic load operation.
    *
    * Returns the current value of this atomic integer.
    *
    * \note The integer load operation is guaranteed to be atomic on all
    * supported platforms and architectures.
    */
   int Load()
   {
      return FetchAndAdd( 0 );
   }

   /*!
    * Atomic store operation.
    *
    * Assigns the specified \a newValue to this object.
    *
    * \note The integer store operation is guaranteed to be atomic on all
    * supported platforms and architectures.
    */
   void Store( int newValue )
   {
      (void)FetchAndStore( newValue );
   }

   /*!
    * Atomic increment operation.
    *
    * Increments the value of this object as an atomic operation.
    *
    * \note This operation is guaranteed to be atomic on all supported
    * platforms and architectures.
    */
   void Increment()
   {
#ifdef __PCL_WINDOWS
      (void)_InterlockedIncrement( &m_value );
#else
      asm volatile( "lock\n\t"
                    "incl %0\n"
                     : "=m" (m_value)
                     : "m" (m_value)
                     : "memory", "cc" );
#endif
   }

   /*!
    * Atomic decrement operation.
    *
    * Decrements the value of this object as an atomic operation.
    *
    * \note This operation is guaranteed to be atomic on all supported
    * platforms and architectures.
    */
   void Decrement()
   {
#ifdef __PCL_WINDOWS
      (void)_InterlockedDecrement( &m_value );
#else
      asm volatile( "lock\n\t"
                    "decl %0\n"
                     : "=m" (m_value)
                     : "m" (m_value)
                     : "memory", "cc" );
#endif
   }

   /*!
    * Atomic reference operation.
    *
    * Increments the value of this object as an atomic operation. Returns true
    * if the resulting value after incrementing this object is nonzero.
    *
    * \note This operation is guaranteed to be atomic on all supported
    * platforms and architectures.
    */
   bool Reference()
   {
#ifdef __PCL_WINDOWS
      return _InterlockedIncrement( &m_value ) != 0;
#else
      uint8 result;
      asm volatile( "lock\n\t"
                    "incl %0\n\t"
                    "setnz %1\n"
                     : "=m" (m_value), "=qm" (result)
                     : "m" (m_value)
                     : "memory", "cc" );
      return result != 0;
#endif
   }

   /*!
    * Atomic dereference operation.
    *
    * Decrements the value of this object as an atomic operation. Returns true
    * if the resulting value after decrementing this object is nonzero.
    *
    * \note This operation is guaranteed to be atomic on all supported
    * platforms and architectures.
    */
   bool Dereference()
   {
#ifdef __PCL_WINDOWS
      return _InterlockedDecrement( &m_value ) != 0;
#else
      uint8 result;
      asm volatile( "lock\n\t"
                    "decl %0\n\t"
                    "setnz %1\n"
                     : "=m" (m_value), "=qm" (result)
                     : "m" (m_value)
                     : "memory", "cc" );
      return result != 0;
#endif
   }

   /*!
    * Atomic test-and-set operation.
    *
    * If the current value of this object is equal to \a expectedValue, this
    * function assigns \a newValue to this object and returns true. If the
    * current value is not equal to \a expectedValue, this function performs no
    * operation and returns false.
    *
    * \note This operation is guaranteed to be atomic on all supported
    * platforms and architectures.
    */
   bool TestAndSet( int expectedValue, int newValue )
   {
#ifdef __PCL_WINDOWS
      return _InterlockedCompareExchange( &m_value, newValue, expectedValue ) == expectedValue;
#else
      uint8 result;
      asm volatile( "lock\n\t"
                    "cmpxchgl %3,%2\n\t"
                    "setz %1\n"
                     : "=a" (newValue), "=qm" (result), "+m" (m_value)
                     : "r" (newValue), "0" (expectedValue)
                     : "memory", "cc" );
      return result != 0;
#endif
   }

   /*!
    * Atomic fetch-and-store operation.
    *
    * Assigns \a newValue to this object and returns the initial value before
    * assignment, as an atomic operation.
    *
    * \note This operation is guaranteed to be atomic on all supported
    * platforms and architectures.
    */
   int FetchAndStore( int newValue )
   {
#ifdef __PCL_WINDOWS
      return _InterlockedExchange( &m_value, newValue );
#else
      asm volatile( "xchgl %0,%1\n"
                     : "=r" (newValue), "+m" (m_value)
                     : "0" (newValue)
                     : "memory" );
      return newValue;
#endif
   }

   /*!
    * Atomic fetch-and-add operation. Adds \a valueToAdd to this object and
    * returns the initial value before addition, as an atomic operation.
    *
    * \note This operation is guaranteed to be atomic on all supported
    * platforms and architectures.
    */
   int FetchAndAdd( int valueToAdd )
   {
#ifdef __PCL_WINDOWS
      return _InterlockedExchangeAdd( &m_value, valueToAdd );
#else
      asm volatile( "lock\n\t"
                    "xaddl %0,%1\n"
                     : "=r" (valueToAdd), "+m" (m_value)
                     : "0" (valueToAdd)
                     : "memory", "cc" );
      return valueToAdd;
#endif
   }

private:

#ifdef _MSC_VER
   __declspec(align(4)) volatile long m_value;
#else
   volatile int m_value __attribute__ ((aligned (4)));
#endif
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup reentrancy_protection Thread-Safe Protection of Non-Reentrant Code
 */

// ----------------------------------------------------------------------------

/*!
 * \class AutoReentrancyGuard
 * \brief Automatic reentrancy guard sentinel.
 *
 * %AutoReentrancyGuard allows you to protect a block of code ensuring that it
 * cannot be reentrant. All you need is a static AtomicInt variable that works
 * as a 'busy state' flag persistent across function invocations.
 *
 * Consider the following example:
 *
 * \code void foo()
 * {
 *    static AtomicInt flag;
 *    AutoReentrancyGuard guard( flag );
 *    if ( guard )
 *    {
 *       // protected code
 *    }
 * }
 * \endcode
 *
 * The function \c foo is not reentrant, so we want to protect it against
 * possible reentrant invocations while the function's code is being executed.
 *
 * The \c flag variable is initially zero (because AtomicInt's default
 * constructor initializes its integer member to zero). The first time \c foo
 * is called, %AutoReentrancyGuard's constructor can change the value of
 * \c flag from zero to one as an atomic operation. When this happens,
 * AutoReentrancyGuard::operator bool() returns true, and the code protected
 * within the \c if block can be executed.
 *
 * When the \c guard object gets out of scope (just before the \c foo function
 * returns), its class destructor resets \c flag to zero automatically, which
 * permits the next non-reentrant execution of \c foo. However, if \c foo is
 * called again before \c guard is destroyed, a newly constructed
 * %AutoReentrancyGuard object cannot make a transition 0 -> 1 with the static
 * \c flag variable, and hence a reentrant execution of the protected code is
 * not allowed (in this case, the function simply does nothing and returns
 * after the \c if block). Note that the protected code can freely return from
 * the function or throw exceptions; the \c flag variable will be reset to zero
 * automatically when \c guard gets out of scope.
 *
 * Since %AutoReentrancyGuard uses AtomicInt to implement atomic transitions,
 * code blocks can be protected against reentrant execution in multithreaded
 * environments.
 *
 * The macros PCL_REENTRANCY_GUARDED_BEGIN and PCL_REENTRANCY_GUARDED_END
 * greatly simplify reentrancy protection. For example, the above code could be
 * implemented as follows:
 *
 * \code void foo()
 * {
 *    PCL_REENTRANCY_GUARDED_BEGIN
 *    // protected code
 *    PCL_REENTRANCY_GUARDED_END
 * } \endcode
 *
 * In addition, the macros PCL_CLASS_REENTRANCY_GUARD and
 * PCL_CLASS_REENTRANCY_GUARDED_BEGIN are useful for protection of all
 * non-reentrant member functions of a class, and the macros
 * PCL_MEMBER_REENTRANCY_GUARD and PCL_MEMBER_REENTRANCY_GUARDED_BEGIN provide
 * protection of specific member functions. See these macros for examples.
 *
 * \ingroup reentrancy_protection
 */
class AutoReentrancyGuard
{
public:

   /*!
    * Constructs an %AutoReentrancyGuard object to monitor the specified
    * \a guard variable. If \c guard is zero, its value is set to one as an
    * atomic operation. If \c guard is nonzero, its value is not changed.
    *
    * \warning The monitored guard variable *must* be either a static variable
    * local to the function being protected, or a data member of the same class
    * to which a protected member function belongs. Otherwise the protection
    * mechanism will not work. This can be dangerous, especially because you
    * may erroneously think that your code is being protected when it is not.
    * In addition, the guard variable must be zero initially, or the protected
    * code will never be allowed to work. We strongly recommend you don't use
    * this class directly, but the PCL_REENTRANCY_GUARDED_BEGIN and
    * PCL_REENTRANCY_GUARDED_END macros to implement function level protection,
    * or PCL_CLASS_REENTRANCY_GUARD, PCL_CLASS_REENTRANCY_GUARDED_BEGIN and
    * PCL_CLASS_REENTRANCY_GUARDED_END to implement per-instance function
    * member protection.
    */
   AutoReentrancyGuard( AtomicInt& guard ) : m_guard( guard )
   {
      m_guarded = m_guard.TestAndSet( 0, 1 );
   }

   /*!
    * Destroys this object. If the value of the monitored guard variable (see
    * the class constructor) was zero when this object was constructed, its
    * value is reset to zero as an atomic operation.
    */
   ~AutoReentrancyGuard()
   {
      if ( m_guarded )
         m_guard.Store( 0 );
   }

   /*!
    * Returns true iff the value of the monitored guard variable (see the class
    * constructor) was zero when this object was constructed.
    */
   operator bool() const volatile
   {
      return m_guarded;
   }

private:

   AtomicInt& m_guard;
   bool       m_guarded : 1;
};

/*!
 * \def PCL_REENTRANCY_GUARDED_BEGIN
 *
 * This macro along with PCL_REENTRANCY_GUARDED_END simplifies protection of
 * non-reentrant code. See the AutoReentrancyGuard class for detailed
 * information and examples.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_REENTRANCY_GUARDED_BEGIN                                          \
   {                                                                          \
      static pcl::AtomicInt __r_g__( 0 );                                     \
      volatile pcl::AutoReentrancyGuard __a_r_g__( __r_g__ );                 \
      if ( __a_r_g__ )                                                        \
      {

/*!
 * \def PCL_REENTRANCY_GUARDED_END
 *
 * This macro, along with PCL_REENTRANCY_GUARDED_BEGIN, simplifies protection
 * of non-reentrant code. See the AutoReentrancyGuard class for detailed
 * information and examples.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_REENTRANCY_GUARDED_END                                            \
      }                                                                       \
   }

/*!
 * \def PCL_CLASS_REENTRANCY_GUARD
 *
 * Declares a class data member for class-wide protection of non-reentrant
 * member functions with the PCL_CLASS_REENTRANCY_GUARDED_BEGIN and
 * PCL_REENTRANCY_GUARDED_END macros.
 *
 * Example:
 *
 * \code class foo
 * {
 * public:
 *    // ...
 * private:
 *    PCL_CLASS_REENTRANCY_GUARD
 *
 *    void bar1()
 *    {
 *       PCL_CLASS_REENTRANCY_GUARDED_BEGIN
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 *
 *    void bar2() const
 *    {
 *       PCL_CLASS_REENTRANCY_GUARDED_BEGIN
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 * }; \endcode
 *
 * In this example the bar1 and bar2 member functions are protected against
 * reentrant execution. Note that reentrancy protection is a per-instance,
 * class-wide property in this case: the bar1() and bar2() functions can be
 * executed simultaneously for different objects, but they cannot be re-entered
 * for the same instance. Furthermore, one of these functions cannot call the
 * other for the same object, since both share the same reentrancy guard member
 * in the foo class.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_CLASS_REENTRANCY_GUARD                                            \
   mutable pcl::AtomicInt __pcl_guard__;

/*!
 * \def PCL_CLASS_REENTRANCY_GUARDED_BEGIN
 *
 * This macro, along with PCL_CLASS_REENTRANCY_GUARD and
 * PCL_REENTRANCY_GUARDED_END, simplifies per-instance protection of
 * non-reentrant class member functions. See PCL_CLASS_REENTRANCY_GUARD for an
 * example.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_CLASS_REENTRANCY_GUARDED_BEGIN                                    \
   {                                                                          \
      volatile pcl::AutoReentrancyGuard __a_r_g__( __pcl_guard__ );           \
      if ( __a_r_g__ )                                                        \
      {

/*!
 * \def PCL_MEMBER_REENTRANCY_GUARD
 *
 * Declares a class data member for protection of a specific non-reentrant
 * member function with the PCL_MEMBER_REENTRANCY_GUARDED_BEGIN and
 * PCL_REENTRANCY_GUARDED_END macros.
 *
 * Example:
 *
 * \code class foo
 * {
 * public:
 *    // ...
 * private:
 *    PCL_MEMBER_REENTRANCY_GUARD( bar1 )
 *    PCL_MEMBER_REENTRANCY_GUARD( bar2 )
 *
 *    void bar1()
 *    {
 *       PCL_MEMBER_REENTRANCY_GUARDED_BEGIN( bar1 )
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 *
 *    void bar2() const
 *    {
 *       PCL_MEMBER_REENTRANCY_GUARDED_BEGIN( bar2 )
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 * }; \endcode
 *
 * In this example the bar1 and bar2 member functions are protected against
 * reentrant execution. Note that reentrancy protection is a per-instance,
 * function-specific property in this case: the bar1() and bar2() functions can
 * be executed simultaneously for different objects, but they cannot be
 * re-entered for the same instance. Since each member function uses its own
 * reentrancy guard member in the foo class, each of them can safely call the
 * other for the same object.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_MEMBER_REENTRANCY_GUARD( member )                                 \
   mutable pcl::AtomicInt __pcl_guard_##member##__;

/*!
 * \def PCL_MEMBER_REENTRANCY_GUARDED_BEGIN
 *
 * This macro, along with PCL_MEMBER_REENTRANCY_GUARD and
 * PCL_REENTRANCY_GUARDED_END, simplifies per-instance protection of specific
 * non-reentrant member functions. See PCL_MEMBER_REENTRANCY_GUARD for an
 * example.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_MEMBER_REENTRANCY_GUARDED_BEGIN( member )                         \
   {                                                                          \
      volatile pcl::AutoReentrancyGuard __a_r_g__( __pcl_guard_##member##__ );\
      if ( __a_r_g__ )                                                        \
      {

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Atomic_h

// ----------------------------------------------------------------------------
// EOF pcl/Atomic.h - Released 2017-06-21T11:36:33Z
