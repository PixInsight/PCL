//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/Exception.h - Released 2018-11-23T16:14:19Z
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

#ifndef __PCL_Exception_h
#define __PCL_Exception_h

/// \file pcl/Exception.h

#include <pcl/Defs.h>

#include <pcl/String.h>

#include <typeinfo>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup standard_exception_classes Standard Exception Classes
 */

/*!
 * \class Exception
 * \brief Root base class for all PCL exception classes
 *
 * All exceptions thrown by PCL classes and functions are descendants of the
 * %Exception class.
 *
 * \ingroup standard_exception_classes
 */
class PCL_CLASS Exception
{
public:

   /*!
    * Constructs an %Exception object.
    */
   Exception() = default;

   /*!
    * Copy constructor.
    */
   Exception( const pcl::Exception& ) = default;

   /*!
    * Destroys an %Exception object.
    */
   virtual ~Exception()
   {
   }

   /*!
    * Returns an error class for this exception.
    */
   virtual String ExceptionClass() const
   {
      return String();
   }

   /*!
    * Returns an error or warning message corresponding to this exception.
    */
   virtual String Message() const
   {
      return String();
   }

   /*!
    * Returns a formatted representation of the information transported by this
    * %Exception object.
    */
   virtual String FormatInfo() const;

   /*!
    * Returns a caption text suitable to represent the information in this
    * %Exception object.
    */
   virtual String Caption() const;

   /*!
    * Shows a representation of the information transported by this exception.
    *
    * Exception information can be shown as text on the platform console or
    * using graphical interfaces, depending on a global setting controlled with
    * the EnableConsoleOutput() and EnableGUIOutput() static member functions.
    *
    * \sa IsConsoleOutputEnabled(), IsGUIOutputEnabled()
    */
   virtual void Show() const;

   /*!
    * Returns true iff console text output is enabled for %Exception.
    *
    * When console output is enabled, exception information is presented as
    * text on the PixInsight core application's console.
    *
    * \sa IsGUIOutputEnabled(), EnableConsoleOutput(), EnableGUIOutput()
    */
   static bool IsConsoleOutputEnabled();

   /*!
    * Enables console output for %Exception.
    *
    * \sa IsConsoleOutputEnabled(), DisableConsoleOutput(),
    * IsGUIOutputEnabled(), EnableGUIOutput()
    */
   static void EnableConsoleOutput( bool = true );

   /*!
    * Disables console output for %Exception.
    *
    * \sa IsConsoleOutputEnabled(), EnableConsoleOutput(),
    * IsGUIOutputEnabled(), EnableGUIOutput()
    */
   static void DisableConsoleOutput( bool disable = true )
   {
      EnableConsoleOutput( !disable );
   }

   /*!
    * Returns true iff GUI output is enabled for %Exception.
    *
    * When GUI output is enabled, exception information is presented through
    * message boxes and other modal, graphical interface elements.
    *
    * \sa IsConsoleOutputEnabled(), EnableConsoleOutput(), EnableGUIOutput()
    */
   static bool IsGUIOutputEnabled();

   /*!
    * Enables GUI output for %Exception.
    *
    * \sa IsGUIOutputEnabled(), DisableGUIOutput(), IsConsoleOutputEnabled(),
    * EnableConsoleOutput()
    */
   static void EnableGUIOutput( bool = true );

   /*!
    * Disables GUI output for %Exception.
    *
    * \sa IsGUIOutputEnabled(), EnableGUIOutput(), IsConsoleOutputEnabled(),
    * EnableConsoleOutput()
    */
   static void DisableGUIOutput( bool disable = true )
   {
      EnableGUIOutput( !disable );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Error
 * \brief A simple exception with an associated error message.
 *
 * %Error is a general exception message very frequently used by PCL classes
 * and functions.
 *
 * \ingroup standard_exception_classes
 */
class PCL_CLASS Error : public pcl::Exception
{
public:

   /*!
    * Constructs an %Error object with an empty error message.
    */
   Error() = default;

   /*!
    * Copy constructor.
    */
   Error( const Error& ) = default;

   /*!
    * Constructs an %Error object with the specified error \a message.
    */
   Error( const String& message ) : m_message( message )
   {
   }

   /*!
    * Returns descriptive information for this %Error object. The default
    * implementation returns the message specified upon construction. A derived
    * class can reimplement this function to provide additional information
    * items such as file names, object identifiers, source code positions, date
    * and time representations, etc.
    */
   virtual String Message() const
   {
      return m_message;
   }

   /*!
    */
   virtual String Caption() const
   {
      return "Error";
   }

protected:

   String m_message;
};

// ----------------------------------------------------------------------------

/*!
 * \class FatalError
 * \brief Errors that cause immediate program termination
 *
 * %FatalError indicates an error situation that cannot be resumed and requires
 * immediate program termination.
 *
 * \ingroup standard_exception_classes
 */
class PCL_CLASS FatalError : public Error
{
public:

   /*!
    * Constructs a %FatalError object with an empty message.
    */
   FatalError() = default;

   /*!
    * Constructs a %FatalError object with the specified \a message.
    */
   FatalError( const String& message ) : Error( message )
   {
   }

   /*!
    * "Promote" any exception to have "fatal consequences".
    */
   FatalError( const pcl::Exception& x ) : Error( x.Message() )
   {
   }

   /*!
    * Copy constructor.
    */
   FatalError( const pcl::FatalError& ) = default;

   /*!
    */
   virtual String Caption() const
   {
      return "Fatal Error";
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class NotImplemented
 * \brief An exception that indicates an unsupported feature
 *
 * Throw one of these when you have an object that can't do something.
 *
 * \ingroup standard_exception_classes
 */
class PCL_CLASS NotImplemented : public Error
{
public:

   /*!
    * Constructs a %NotImplemented instance.
    *
    * \param object           The object that does not implement something
    *
    * \param notImplemented   A description of what \a object does not
    *                         implement.
    */
   template <typename T>
   NotImplemented( const T& object, const String& notImplemented ) :
      Error( String( IsoString( typeid( object ).name() )
#ifdef __PCL_WINDOWS
      .MBSToWCS()
#else
      .UTF8ToUTF16()
#endif
         ) + ": Not implemented: " + notImplemented )
   {
   }

   /*!
    * Copy constructor.
    */
   NotImplemented( const NotImplemented& ) = default;
};

// ----------------------------------------------------------------------------

/*!
 * \class ParseError
 * \brief Base class for exceptions thrown by parsing routines
 *
 * A %ParseError indicates an syntactic or semantic error found while
 * interpreting some code or trying to translate some text into an object.
 *
 * For example, String and IsoString throw %ParseError exceptions to indicate
 * syntax errors while converting strings to numbers.
 *
 * \ingroup standard_exception_classes
 */
class PCL_CLASS ParseError : public Error
{
public:

   /*!
    * Constructs an empty %ParseError object: no error message and no position
    * information.
    */
   ParseError() = default;

   /*!
    * Copy constructor.
    */
   ParseError( const ParseError& ) = default;

   /*!
    * Constructs a %ParseError object.
    *
    * \param message    The error message. HTML entities are not interpreted as
    *             ISO 8859-1 characters, and console tags are ignored, even if
    *             the exception is to be represented on a PixInsight console.
    *             HTML entities and console tags are always written literally.
    *             This limitation is necessary to ensure representativeness of
    *             source code fragments shown in exception messages.
    *
    * \param beingParsed   The string being parsed, or an empty string if the
    *             error is not associated to a particular source code string.
    *
    * \param errorPosition   The position >= 0 of the first offending character
    *             in the string \a beingParsed, or < 0 if no specific position
    *             can be associated with the error.
    */
   ParseError( const String& message, const String& beingParsed = String(), int errorPosition = -1 ) :
      Error( message ), m_beingParsed( beingParsed ), m_errorPosition( errorPosition )
   {
   }

   /*!
    */
   virtual String Message() const;

   /*!
    */
   virtual void Show() const;

protected:

   String m_beingParsed;
   int    m_errorPosition = -1;
};

// ----------------------------------------------------------------------------

/*!
 * \class SourceCodeError
 * \brief Base class for exceptions thrown by source code interpreters
 *
 * A %SourceCodeError represents a syntax error thrown by a source code parser
 * or interpreter, such as a scripting engine or an XML decoder.
 *
 * %SourceCodeError provides an error message string associated to a line
 * number and a column number that locate the error in the source code text
 * being processed.
 *
 * \ingroup standard_exception_classes
 */
class PCL_CLASS SourceCodeError : public Error
{
public:

   /*!
    * Constructs an empty %SourceCodeError object: no error message and no
    * source code location information.
    */
   SourceCodeError() = default;

   /*!
    * Copy constructor.
    */
   SourceCodeError( const SourceCodeError& ) = default;

   /*!
    * Constructs a %SourceCodeError object.
    *
    * \param message    The error message. HTML entities are not interpreted as
    *             ISO 8859-1 characters, and console tags are ignored, even if
    *             the exception is to be represented on a PixInsight console.
    *             HTML entities and console tags are always written literally.
    *             This limitation is necessary to ensure representativeness of
    *             source code fragments in exception messages.
    *
    * \param line       The line number (starting from 1) corresponding to the
    *             position of this error in the source code text, or <= 0 if no
    *             specific source code line can be associated with this error.
    *             The default value is -1, meaning that no line number is
    *             specified by default.
    *
    * \param column     The column number (starting from 1) corresponding to
    *             the position of this error in the source code text, or <= 0
    *             if no specific source code column can be associated with this
    *             error. The default value is -1, meaning that no column number
    *             is specified by default.
    */
   template <typename T1>
   SourceCodeError( const String& message, T1 line = -1, T1 column = -1 ) :
      Error( message ),
      m_lineNumber( Range( int( line ), -1, int_max ) ),
      m_columnNumber( Range( int( column ), -1, int_max ) )
   {
   }

   /*!
    * Returns the line number in the source code text being processed,
    * corresponding to this exception.
    *
    * If a valid source code line can be associated with this exception, the
    * returned value should be >= 1. Otherwise, if no text line can be
    * determined for this exception, -1 is returned.
    *
    * \sa ColumnNumber()
    */
   int LineNumber() const
   {
      return m_lineNumber;
   }

   /*!
    * Returns the column number in the source code text being processed,
    * corresponding to this exception.
    *
    * If a valid source code column can be associated with this exception, the
    * returned value should be >= 1. Otherwise, if no text column can be
    * determined for this exception, -1 is returned.
    *
    * \sa LineNumber()
    */
   int ColumnNumber() const
   {
      return m_columnNumber;
   }

   /*!
    * Sets the source code \a line number and \a column (optional) for this
    * exception.
    *
    * Valid (meaningful) source code coordinates must be >= 1. When a value < 0
    * is specified, the corresponding coordinate is interpreted to be
    * unavailable for the current exception.
    *
    * This member function is useful when nested sections of a source code
    * block are being processed recursively; for example in XML decoders.
    *
    * \sa AddToPosition()
    */
   template <typename T1>
   void SetPosition( T1 line, T1 column = -1 )
   {
      m_lineNumber = Range( int( line ), -1, int_max );
      m_columnNumber = Range( int( column ), -1, int_max );
   }

   /*!
    * Adds the specified \a lines and \a columns (optional) increments to the
    * source code coordinates for this exception.
    *
    * Valid (meaningful) source code coordinates must be >= 1. When a value
    * <= 0 results from a call to this function, the corresponding coordinate
    * is interpreted to be unavailable for the current exception.
    *
    * This member function is useful when nested sections of a source code
    * block are being processed recursively; for example in XML decoders.
    *
    * \sa SetPosition()
    */
   template <typename T1>
   void AddToPosition( T1 lines, T1 columns = 0 )
   {
      SetPosition( m_lineNumber+int( lines ), m_columnNumber+int( columns ) );
   }

   /*!
    */
   String Message() const override;

   /*!
    */
   void Show() const override;

protected:

   int m_lineNumber   = -1;
   int m_columnNumber = -1;
};

// ----------------------------------------------------------------------------

/*!
 * \class CaughtException
 * \brief An exception that has already been handled
 *
 * %CaughtException is used to signal exceptions requiring no additional
 * processing. %CaughtException is useful to terminate a process (by branching
 * execution to a catch() block) without causing generation of further error
 * messages.
 *
 * \ingroup standard_exception_classes
 */
class PCL_CLASS CaughtException : public pcl::Exception
{
public:

   /*!
    * Constructs a %CaughtException object.
    */
   CaughtException() = default;

   /*!
    * Copy constructor.
    */
   CaughtException( const pcl::CaughtException& ) = default;
};

}  // pcl

// ----------------------------------------------------------------------------

/*!
 * \def PCL_DECLARE_EXCEPTION_CLASS
 * \brief A macro to implement simple exception classes derived from Exception
 *
 * \param className        The identifier of the exception class.
 *
 * \param exceptionClass   The name of the exception class, as presented in
 *                         formatted representations.
 *
 * \param message          The error message shown for this exception class.
 *
 * Use this macro to define and implement exceptions classes that show
 * constant error or warning messages.
 *
 * In many cases using the %Error exception class is preferable to create new
 * exception classes with this macro.
 *
 * \ingroup standard_exception_classes
 */
#define PCL_DECLARE_EXCEPTION_CLASS( className, exceptionClass, message )  \
   class PCL_CLASS className : public pcl::Exception                       \
   {                                                                       \
   public:                                                                 \
      className() = default;                                               \
      className( const className& ) = default;                             \
      pcl::String ExceptionClass() const override                          \
      {                                                                    \
         return exceptionClass;                                            \
      }                                                                    \
      pcl::String Message() const override                                 \
      {                                                                    \
         return message;                                                   \
      }                                                                    \
   }

// ----------------------------------------------------------------------------

namespace pcl
{

/*!
 * \class pcl::ProcessAborted
 * \brief An exception class signaling the interruption of a process
 *
 * %ProcessAborted is thrown by processes that have been aborted by the user.
 *
 * \ingroup standard_exception_classes
 */

PCL_DECLARE_EXCEPTION_CLASS( ProcessAborted, "Process aborted", String() );

}  // pcl

#endif   // __PCL_Exception_h

// ----------------------------------------------------------------------------
// EOF pcl/Exception.h - Released 2018-11-23T16:14:19Z
