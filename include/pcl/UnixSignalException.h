//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/UnixSignalException.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_UnixSignalException_h
#define __PCL_UnixSignalException_h

/// \file pcl/UnixSignalException.h

#if defined( __PCL_WINDOWS ) || !defined( __PCL_LINUX ) && !defined( __PCL_FREEBSD ) && !defined( __PCL_MACOSX )
#  error UnixSignalException can only be used on Linux, FreeBSD and OS X platforms.
#endif

#include <pcl/Defs.h>

#include <pcl/Exception.h>

#include <signal.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class UnixSignalException
 * \brief A UNIX synchronous signal handler that throws C++ exceptions.
 *
 * This class can only be used on Linux, FreeBSD and OS X platforms. On Windows
 * platforms, including this header file from compilable code raises a compiler
 * error.
 *
 * For this handler to work properly, all code that may raise synchronous
 * signals (SIGSEGV and the like) must be compiled with GCC's
 * '-fnon-call-exceptions' flag. Otherwise the exceptions will be thrown
 * but terminate() will be called, which is the default critical signal
 * management behavior.
 *
 * To generate a backtrace report (see the UnixSignalException::Details()
 * member function), the code must be compiled with the '-rdynamic' GCC flag,
 * which instructs the linker to add all symbols to the dynamic symbol table.
 * In addition, the generated binaries should not be stripped with the '-s'
 * linker flag.
 */
class PCL_CLASS UnixSignalException : public pcl::Exception
{
public:

   /*!
    * Constructs a new %UnixSignalException object with the specified \a signal
    * number and optional backtrace \a details.
    */
   UnixSignalException( int signal, const IsoString& details = IsoString() ) :
      pcl::Exception(), m_signal( signal ), m_details( details )
   {
   }

   /*!
    * Copy constructor.
    */
   UnixSignalException( const UnixSignalException& ) = default;

   /*!
    * Returns the signal number associated with this object.
    */
   int SignalNumber() const
   {
      return m_signal;
   }

   /*!
    * Returns the backtrace information associated with this exception.
    *
    * The returned string will be empty if no backtrace data were available at
    * the time this signal exception was generated. This happens when the code
    * has not been compiled and linked with the appropriate options (see the
    * -rdynamic compiler flag).
    */
   const IsoString& Details() const
   {
      return m_details;
   }

   /*!
    * Returns an error or warning message corresponding to this signal
    * exception. Typical messages are "segmentation violation", "bus error" and
    * "floating point exception", returned by specific derived classes.
    */
   virtual String Message() const
   {
      return "Undefined signal";
   }

   /*!
    * Returns a formatted error message with information on this signal
    * exception.
    */
   virtual String FormatInfo() const
   {
      String info = String().Format( "Critical signal caught (%d): ", SignalNumber() ) + Message();
      if ( !m_details.IsEmpty() )
      {
         info.Append( '\n' );
         info.Append( m_details );
      }
      return info;
   }

   /*!
    * Returns the type of this exception, intended to be used as a caption for
    * a message box. As reimplemented in this class, this member function
    * returns the string "PCL Unix Signal Handler".
    */
   virtual String Caption() const
   {
      return "PCL Unix Signal Handler";
   }

   /*!
    * Initializes the UNIX synchronous signal handler. This static member
    * function must be called before the calling process can raise any
    * synchronous signal.
    *
    * \note A module should never call this member function. It is invoked when
    * appropriate by the PixInsight Core application and internal PCL routines.
    */
   static void Initialize();

protected:

   int       m_signal;  // signal number
   IsoString m_details; // backtrace information
};

// ----------------------------------------------------------------------------

#define DECLARE_UNIX_SIGNAL_EXCEPTION( className, sigNum, message )           \
   class PCL_CLASS className : public pcl::UnixSignalException                \
   {                                                                          \
   public:                                                                    \
      className( const IsoString& details = IsoString() ) :                   \
         pcl::UnixSignalException( sigNum, details )                          \
      {                                                                       \
      }                                                                       \
      className( const className& ) = default;                                \
      virtual String Message() const                                          \
      {                                                                       \
         return message;                                                      \
      }                                                                       \
   }

// ----------------------------------------------------------------------------

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixSegmentationViolation, SIGSEGV,
                               "Segmentation violation" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixBusError, SIGBUS,
                               "Bus error" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixFloatingPointException, SIGFPE,
                               "Floating point exception" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixIllegalInstructionException, SIGILL,
                               "Illegal instruction" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixIBrokenPipeException, SIGPIPE,
                               "Broken pipe" );

// ----------------------------------------------------------------------------

#undef DECLARE_UNIX_SIGNAL_EXCEPTION

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_UnixSignalException_h

// ----------------------------------------------------------------------------
// EOF pcl/UnixSignalException.h - Released 2017-06-21T11:36:33Z
