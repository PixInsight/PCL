//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/Settings.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_Settings_h
#define __PCL_Settings_h

/// \file pcl/Settings.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ByteArray.h>
#include <pcl/String.h>

#ifndef __PCL_NO_FLAGS_SETTINGS_IO
#  include <pcl/Flags.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Settings
 * \brief Persistent module settings.
 *
 * %Settings implements an interface to store and manage module settings that
 * can persist across sessions. %Settings makes it possible for a module to
 * \e remember its own private data, as specific user preferences and other
 * operating parameters. %Settings data items are stored and retrieved as
 * \e key/value pairs.
 *
 * %Module settings can be accessed in two \e spaces. One is the <em>local
 * settings space</em>, which is used by a module to read and write its own
 * private settings data, or settings \e keys that have been created by the
 * module. %Module settings can also be accessed from the <em>global module
 * settings space</em>, which refers to settings data created by other modules.
 * Thanks to the global module settings space, processes and file formats can
 * share their working settings publicly on the whole PixInsight platform. For
 * example, a module can learn the current orientation of FITS images by
 * reading the value of the \e /FITS/FITSBottomUp key, which is maintained by
 * the standard FITS support module.
 *
 * %Settings provides a high-level, platform-independent mechanism for settings
 * management. The PixInsight core application carries out all the necessary
 * platform-specific work to store and retrieve module settings data behind the
 * scenes.
 *
 * This class must not be confused with PixInsightSettings, which provides
 * access to \e global settings pertaining to the whole PixInsight platform.
 * %Settings manages private module settings exclusively. Private module
 * settings cannot be retrieved through %PixInsightSettings.
 *
 * \sa PixInsightSettings
 */
class PCL_CLASS Settings
{
public:

   /*!
    * Default constructor. This constructor is disabled because %Settings is
    * not an instantiable class.
    */
   Settings() = delete;

   /*!
    * Copy constructor. This constructor is disabled because %Settings is not
    * an instantiable class.
    */
   Settings( const Settings& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %Settings is not an
    * instantiable class.
    */
   Settings& operator =( const Settings& ) = delete;

   /*!
    * Destructor. This destructor is disabled because %Settings is not an
    * instantiable class.
    */
   ~Settings() = delete;

   /*!
    * Reads a sequence of bytes from the <em>local settings space</em> of the
    * current module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] b  Reference to a ByteArray object where existing data will
    *                be stored.
    *
    * Returns true if the specified settings \a key exists in the local
    * settings space of the current module and its value can be converted to a
    * sequence of bytes, in which case the existing data are read and stored in
    * the \a b array. Otherwise this function returns false and the previous
    * contents of the \a b array are not modified.
    */
   static bool Read( const IsoString& key, ByteArray& b );

   /*!
    * Reads a sequence of bytes from the <em>global module settings space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] b  Reference to a ByteArray object where existing data will
    *                be stored.
    *
    * Returns true if the specified settings \a key exists in the global module
    * settings space, it has not been read-protected by its owner module, and
    * its value can be converted to a sequence of bytes, in which case the
    * existing data are read and stored in the \a b array. Otherwise this
    * function returns false and the previous contents of the \a b array are
    * not modified.
    */
   static bool ReadGlobal( const IsoString& key, ByteArray& b );

   /*!
    * Reads a Unicode string (UTF-16) from the <em>local settings space</em> of
    * the current module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] s  Reference to a String object where existing data will be
    *                stored.
    *
    * Returns true if the specified settings \a key exists in the local
    * settings space of the current module and its value can be converted to a
    * Unicode string, in which case the existing data are read and stored in
    * the \a s string. Otherwise this function returns false and the previous
    * contents of the \a s string are not modified.
    */
   static bool Read( const IsoString& key, String& s );

   /*!
    * Reads a Unicode string (UTF-16) from the <em>global module settings
    * space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] s  Reference to a String object where existing data will be
    *                stored.
    *
    * Returns true if the specified settings \a key exists in the global module
    * settings space, it has not been read-protected by its owner module, and
    * its value can be converted to a Unicode string, in which case the
    * existing data are read and stored in the \a s string. Otherwise this
    * function returns false and the previous contents of the \a s string are
    * not modified.
    */
   static bool ReadGlobal( const IsoString& key, String& s );

   /*!
    * Reads a Boolean value from the <em>local settings space</em> of the
    * current module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] b  Reference to a \c bool variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the local
    * settings space of the current module and its value can be converted to
    * the \c bool type, in which case the existing data are read and stored in
    * the \a b variable. Otherwise this function returns false and the previous
    * contents of the \a b variable are not modified.
    */
   static bool Read( const IsoString& key, bool& b );

   /*!
    * Reads a Boolean value from the <em>global module settings space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] b  Reference to a \c bool variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the global module
    * settings space, it has not been read-protected by its owner module, and
    * its value can be converted to the \c bool type, in which case the
    * existing data are read and stored in the \a b variable. Otherwise this
    * function returns false and the previous contents of the \a b variable are
    * not modified.
    */
   static bool ReadGlobal( const IsoString& key, bool& b );

   /*!
    * Reads an integer value from the <em>local settings space</em> of the
    * current module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] i  Reference to an \c int variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the local
    * settings space of the current module and its value can be converted to
    * the \c int type, in which case the existing data are read and stored in
    * the \a i variable. Otherwise this function returns false and the previous
    * contents of the \a i variable are not modified.
    */
   static bool Read( const IsoString& key, int& i );

   /*!
    * Reads an integer value from the <em>global module settings space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] i  Reference to an \c int variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the global module
    * settings space, it has not been read-protected by its owner module, and
    * its value can be converted to the \c int type, in which case the existing
    * data are read and stored in the \a i variable. Otherwise this function
    * returns false and the previous contents of the \a i variable are not
    * modified.
    */
   static bool ReadGlobal( const IsoString& key, int& i );

   /*!
    * Reads an unsigned integer value from the <em>local settings space</em> of
    * the current module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] u  Reference to an \c unsigned \c int variable where the
    *                existing value will be stored.
    *
    * Returns true if the specified settings \a key exists in the local
    * settings space of the current module and its value can be converted to
    * the \c unsigned type, in which case the existing data are read and stored
    * in the \a u variable. Otherwise this function returns false and the
    * previous contents of the \a u variable are not modified.
    */
   static bool Read( const IsoString& key, unsigned& u );

   /*!
    * Reads an unsigned integer value from the <em>global module settings
    * space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] u  Reference to an \c unsigned \c int variable where the
    *                existing value will be stored.
    *
    * Returns true if the specified settings \a key exists in the global module
    * settings space, it has not been read-protected by its owner module, and
    * its value can be converted to the \c unsigned type, in which case the
    * existing data are read and stored in the \a u variable. Otherwise this
    * function returns false and the previous contents of the \a u variable are
    * not modified.
    */
   static bool ReadGlobal( const IsoString& key, unsigned& u );

   /*!
    * Reads a \c double value from the <em>local settings space</em> of the
    * current module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] f  Reference to a \c double variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the local
    * settings space of the current module and its value can be converted to
    * the \c double type, in which case the existing data are read and stored
    * in the \a f variable. Otherwise this function returns false and the
    * previous contents of the \a f variable are not modified.
    */
   static bool Read( const IsoString& key, double& f );

   /*!
    * Reads a \c double value from the <em>global module settings space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] f  Reference to a \c double variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the global module
    * settings space, it has not been read-protected by its owner module, and
    * its value can be converted to the \c double type, in which case the
    * existing data are read and stored in the \a f variable. Otherwise this
    * function returns false and the previous contents of the \a f variable are
    * not modified.
    */
   static bool ReadGlobal( const IsoString& key, double& f );

   /*!
    * Reads a \c float value from the <em>local settings space</em> of the
    * current module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] f  Reference to a \c float variable where the existing value
    *                will be stored.
    *
    * This is an overloaded member function, provided for convenience. It
    * behaves essentially like the Read( const IsoString&, double& ) member
    * function.
    */
   static bool Read( const IsoString& key, float& f )
   {
      double d; Read( key, d );
      if ( LastReadOK() ) { f = float( d ); return true; }
      return false;
   }

   /*!
    * Reads a \c float value from the <em>global module settings space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] f  Reference to a \c float variable where the existing value
    *                will be stored.
    *
    * This is an overloaded member function, provided for convenience. It
    * behaves essentially like the ReadGlobal( const IsoString&, double& )
    * member function.
    */
   static bool ReadGlobal( const IsoString& key, float& f )
   {
      double d; ReadGlobal( key, d );
      if ( LastReadOK() ) { f = float( d ); return true; }
      return false;
   }

#ifndef __PCL_NO_SETTINGS_TEMPLATES

   /*!
    * A generalized function that reads a settings \a key as a \c signed
    * \c int, converts that value to type T, and stores it in the specified
    * variable \a t.
    *
    * Returns true iff the \a key exists and its value can be converted to
    * \c int. If the function fails (if it returns false), the previous value
    * of the \a t variable is not changed.
    *
    * This is a convenience template member function. It behaves essentially
    * like the Read( const IsoString&, int& ) member function.
    */
   template <typename T>
   static bool ReadI( const IsoString& key, T& t )
   {
      int i; Read( key, i );
      if ( LastReadOK() ) { t = T( i ); return true; }
      return false;
   }


   /*!
    * A generalized function that reads a settings \a key as an \c unsigned
    * \c int, converts that value to type T, and stores it in the specified
    * variable \a t.
    *
    * Returns true iff the \a key exists and its value can be converted to
    * \c unsigned \c int. If the function fails (if it returns false), the
    * previous value of the \a t variable is not changed.
    *
    * This is a convenience template member function. It behaves essentially
    * like the Read( const IsoString&, unsigned& ) member function.
    */
   template <typename T>
   static bool ReadU( const IsoString& key, T& t )
   {
      unsigned u; Read( key, u );
      if ( LastReadOK() ) { t = T( u ); return true; }
      return false;
   }

#endif

#ifndef __PCL_NO_FLAGS_SETTINGS_IO

   /*!
    * Reads a Flags value from the <em>local settings space</em> of the current
    * module.
    *
    * \param key     A settings key for the current module.
    *
    * \param[out] f  Reference to a Flags variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the local
    * settings space of the current module and its value can be converted to
    * the \c unsigned \c int type, in which case the existing data are read and
    * stored in the \a f variable as a <em>flags mask</em>. Otherwise this
    * function returns false and the previous contents of the \a f variable are
    * not modified.
    */
   template <typename E>
   static bool Read( const IsoString& key, Flags<E>& f )
   {
      return Read( key, f.m_flags );
   }

   /*!
    * Reads a Flags value from the <em>global module settings space</em>.
    *
    * \param key     A module settings key. Should include the name of a
    *                module, as in <em>/ModuleName/KeyName</em>.
    *
    * \param[out] f  Reference to a Flags variable where the existing value
    *                will be stored.
    *
    * Returns true if the specified settings \a key exists in the global module
    * settings space, it has not been read-protected by its owner module, and
    * its value can be converted to the \c unsigned \c int type, in which case
    * the existing data are read and stored in the \a f variable as a <em>flags
    * mask</em>. Otherwise this function returns false and the previous
    * contents of the \a f variable are not modified.
    */
   template <typename E>
   static bool ReadGlobal( const IsoString& key, Flags<E>& f )
   {
      return ReadGlobal( key, f.m_flags );
   }

#endif

   /*!
    * Returns true iff the last call to a Read member function successfully
    * read a settings key.
    */
   static bool LastReadOK();

   /*!
    * Writes a sequence of bytes to the <em>local settings space</em> of the
    * current module.
    *
    * \param key  A settings key for the current module.
    *
    * \param b    Reference to a ByteArray object whose contents will be
    *             written as the value associated to \a key.
    *
    * If the specified settings \a key does not exist in the local settings
    * space of the current module, it is newly created with the specified value
    * \a b. If \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   static void Write( const IsoString& key, const ByteArray& b );

   /*!
    * Writes a sequence of bytes to the <em>global module settings space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param b    Reference to a ByteArray object whose contents will be
    *             written as the value associated to \a key.
    *
    * If the specified settings \a key does not exist in the global module
    * settings space, it is newly created with the specified value \a b. If
    * \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   static void WriteGlobal( const IsoString& key, const ByteArray& b );

   /*!
    * Writes a Unicode string (UTF-16) to the <em>local settings space</em> of
    * the current module.
    *
    * \param key  A settings key for the current module.
    *
    * \param s    Reference to a String object whose contents will be written
    *             as the value associated to \a key.
    *
    * If the specified settings \a key does not exist in the local settings
    * space of the current module, it is newly created with the specified value
    * \a s. If \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   static void Write( const IsoString& key, const String& s );

   /*!
    * Writes a Unicode string (UTF-16) to the <em>global module settings
    * space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param s    Reference to a String object whose contents will be written
    *             as the value associated to \a key.
    *
    * If the specified settings \a key does not exist in the global module
    * settings space, it is newly created with the specified value \a s. If
    * \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   static void WriteGlobal( const IsoString& key, const String& s );

   /*!
    * Writes a Boolean value to the <em>local settings space</em> of the
    * current module.
    *
    * \param key  A settings key for the current module.
    *
    * \param b    A \c bool value that will be written as the value associated
    *             to \a key.
    *
    * If the specified settings \a key does not exist in the local settings
    * space of the current module, it is newly created with the specified value
    * \a b. If \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   static void Write( const IsoString& key, bool b );

   /*!
    * Writes a Boolean value to the <em>global module settings space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param b    A \c bool value that will be written as the value associated
    *             to \a key.
    *
    * If the specified settings \a key does not exist in the global module
    * settings space, it is newly created with the specified value \a b. If
    * \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   static void WriteGlobal( const IsoString& key, bool b );

   /*!
    * Writes an integer value to the <em>local settings space</em> of the
    * current module.
    *
    * \param key  A settings key for the current module.
    *
    * \param i    An \c int value that will be written as the value associated
    *             to \a key.
    *
    * If the specified settings \a key does not exist in the local settings
    * space of the current module, it is newly created with the specified value
    * \a i. If \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   static void Write( const IsoString& key, int i );

   /*!
    * Writes an integer value to the <em>global module settings space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param i    An \c int value that will be written as the value associated
    *             to \a key.
    *
    * If the specified settings \a key does not exist in the global module
    * settings space, it is newly created with the specified value \a i. If
    * \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   static void WriteGlobal( const IsoString& key, int i );

   /*!
    * Writes an unsigned integer value to the <em>local settings space</em> of
    * the current module.
    *
    * \param key  A settings key for the current module.
    *
    * \param u    An \c unsigned \c int value that will be written as the value
    *             associated to \a key.
    *
    * If the specified settings \a key does not exist in the local settings
    * space of the current module, it is newly created with the specified value
    * \a u. If \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   static void Write( const IsoString& key, unsigned u );

   /*!
    * Writes an unsigned integer value to the <em>global module settings
    * space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param u    An \c unsigned \c int value that will be written as the value
    *             associated to \a key.
    *
    * If the specified settings \a key does not exist in the global module
    * settings space, it is newly created with the specified value \a u. If
    * \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   static void WriteGlobal( const IsoString& key, unsigned u );

   /*!
    * Writes a \c double value to the <em>local settings space</em> of the
    * current module.
    *
    * \param key  A settings key for the current module.
    *
    * \param f    A \c double value that will be written as the value
    *             associated to \a key.
    *
    * If the specified settings \a key does not exist in the local settings
    * space of the current module, it is newly created with the specified value
    * \a f. If \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   static void Write( const IsoString& key, double f );

   /*!
    * Writes a \c double value to the <em>global module settings space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param f    A \c double value that will be written as the value
    *             associated to \a key.
    *
    * If the specified settings \a key does not exist in the global module
    * settings space, it is newly created with the specified value \a f. If
    * \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   static void WriteGlobal( const IsoString& key, double f );

   /*!
    * Writes a \c float value to the <em>local settings space</em> of the
    * current module.
    *
    * \param key  A settings key for the current module.
    *
    * \param f    A \c float value that will be written as the value associated
    *             to \a key.
    *
    * This is an overloaded member function, provided for convenience. It
    * behaves essentially like the Write( const IsoString&, double ) member
    * function.
    */
   static void Write( const IsoString& key, float f )
   {
      Write( key, double( f ) );
   }

   /*!
    * Writes a \c float value to the <em>global module settings space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param f    A \c float value that will be written as the value associated
    *             to \a key.
    *
    * This is an overloaded member function, provided for convenience. It
    * behaves essentially like the WriteGlobal( const IsoString&, double )
    * member function.
    */
   static void WriteGlobal( const IsoString& key, float f )
   {
      WriteGlobal( key, double( f ) );
   }

#ifndef __PCL_NO_SETTINGS_TEMPLATES

   /*!
    * A generalized function that converts a value of type T to \c signed
    * \c int and writes it to the specified settings \a key.
    *
    * This is a convenience template member function. It behaves essentially
    * like the Write( const IsoString&, int ) member function.
    */
   template <typename T>
   static void WriteI( const IsoString& key, T t )
   {
      Write( key, int( t ) );
   }

   /*!
    * A generalized function that converts a value of type T to \c unsigned
    * \c int and writes it to the specified settings \a key.
    *
    * This is a convenience template member function. It behaves essentially
    * like the Write( const IsoString&, unsigned ) member function.
    */
   template <typename T>
   static void WriteU( const IsoString& key, T t )
   {
      Write( key, unsigned( t ) );
   }

#endif

#ifndef __PCL_NO_FLAGS_SETTINGS_IO

   /*!
    * Writes a Flags value to the <em>local settings space</em> of the current
    * module.
    *
    * \param key  A settings key for the current module.
    *
    * \param f    A Flags value that will be written as the value associated to
    *             \a key (as an \c unsigned \c int value).
    *
    * If the specified settings \a key does not exist in the local settings
    * space of the current module, it is newly created with the specified value
    * \a f. If \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   template <typename E>
   static void Write( const IsoString& key, Flags<E> f )
   {
      Write( key, f.m_flags );
   }

   /*!
    * Writes a Flags value to the <em>global module settings space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * \param f    A Flags value that will be written as the value associated to
    *             \a key (as an \c unsigned \c int value).
    *
    * If the specified settings \a key does not exist in the global module
    * settings space, it is newly created with the specified value \a f. If
    * \a key already exists, its previous value is replaced.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   template <typename E>
   static void WriteGlobal( const IsoString& key, Flags<E> f )
   {
      WriteGlobal( key, f.m_flags );
   }

#endif

   /*!
    * Deletes the specified settings \a key from the <em>local settings
    * space</em> of the current module.
    *
    * In the event of I/O error this member function throws an Error exception.
    */
   static void Remove( const IsoString& key );

   /*!
    * Deletes a settings key from the <em>global module settings space</em>.
    *
    * \param key  A module settings key. Should include the name of a module,
    *             as in <em>/ModuleName/KeyName</em>.
    *
    * In the event of I/O error, or if the specified \a key is write-protected,
    * this member function throws an Error exception.
    *
    * \note By default, write access to module settings items is not allowed
    * unless explicitly enabled through a call to SetGlobalKeyAccess().
    */
   static void RemoveGlobal( const IsoString& key );

   /*!
    * Returns true iff the specified \a key can be read from the <em>global
    * module settings space</em>.
    *
    * By default, a module settings key can be freely read unless its owner
    * module explicitly disables global reading by calling the
    * SetGlobalKeyAccess() static member function.
    */
   static bool CanReadGlobal( const IsoString& key );

   /*!
    * Returns true iff the specified \a key can be written from the <em>global
    * module settings space</em>.
    *
    * By default, global write access to a module settings key is not allowed
    * unless its owner module explicitly enables global writing by calling the
    * SetGlobalKeyAccess() static member function.
    */
   static bool CanWriteGlobal( const IsoString& key );

   /*!
    * Changes global access permissions for the specified settings \a key in
    * the <em>local settings space</em> of the current module.
    *
    * \param key        A settings key for the current module.
    *
    * \param allowRead  Whether other modules can read the value of \a key.
    *
    * \param allowWrite Whether other modules can write the value of \a key
    *                   and delete \a key.
    *
    * If this function is not called for a particular settings key, only read
    * access is possible by default from the global module settings space.
    */
   static void SetGlobalKeyAccess( const IsoString& key, bool allowRead, bool allowWrite );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Settings_h

// ----------------------------------------------------------------------------
// EOF pcl/Settings.h - Released 2017-08-01T14:23:31Z
