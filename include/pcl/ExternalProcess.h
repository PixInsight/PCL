//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/ExternalProcess.h - Released 2019-01-21T12:06:07Z
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

#ifndef __PCL_ExternalProcess_h
#define __PCL_ExternalProcess_h

/// \file pcl/ExternalProcess.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>
#include <pcl/Flags.h>
#include <pcl/UIObject.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ExternalProcessError
 * \brief External process error conditions.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ExternalProcessError::FailedToStart</td> <td>The external process failed to start.</td></tr>
 * <tr><td>ExternalProcessError::Crashed</td>       <td>The external process crashed.</td></tr>
 * <tr><td>ExternalProcessError::TimedOut</td>      <td>A wait operation timed out.</td></tr>
 * <tr><td>ExternalProcessError::ReadError</td>     <td>An error occurred while trying to read data from a process' stdout or stderr streams.</td></tr>
 * <tr><td>ExternalProcessError::WriteError</td>    <td>An error occurred while trying to write data to a process' stdin stream.</td></tr>
 * <tr><td>ExternalProcessError::UnknownError</td>  <td>An unknown error occurred trying to execute or communicate with an external process.</td></tr>
 * </table>
 */
namespace ExternalProcessError
{
   enum value_type
   {
      FailedToStart,
      Crashed,
      TimedOut,
      ReadError,
      WriteError,
      UnknownError
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class ExternalProcess
 * \brief Execution of external programs.
 *
 * %ExternalProcess is essentially a PCL wrapper to Qt's QProcess class, which
 * the PixInsight core application uses as the underlying implementation for
 * management of external processes on the PixInsight platform.
 *
 * ### TODO: Write a detailed description for %ExternalProcess.
 */
class PCL_CLASS ExternalProcess : public UIObject
{
public:

#ifndef __PCL_WINDOWS
   /*!
    * The type of a platform-dependent Process Identifier (PID).
    *
    * On Linux/UNIX (Linux, FreeBSD and Mac OS X), a PID is a 64-bit integer.
    *
    * On Windows platforms, a PID is a pointer to a _PROCESS_INFORMATION
    * structure (hence, on 64-bit Windows platforms a PID is also a 64-bit
    * integer number).
    */
   typedef int64  pid_type;
#else
   typedef void*  pid_type;
#endif

   /*!
    * The type used to represent error conditions in external processes.
    */
   typedef ExternalProcessError::value_type  error_code;

   /*!
    * Constructs an %ExternalProcess object.
    */
   ExternalProcess();

   /*!
    * Destroys the client-side %ExternalProcess instance and dereferences the
    * server-side object. If the server-side object becomes unreferenced, it
    * will be garbage-collected and eventually destroyed by the core
    * application.
    */
   virtual ~ExternalProcess()
   {
   }

   /*!
    * Copy constructor. Copy semantics are disabled for %ExternalProcess
    * because this class represents unique server-side objects.
    */
   ExternalProcess( const ExternalProcess& ) = delete;

   /*!
    * Copy assignment. Copy semantics are disabled for %ExternalProcess because
    * this class represents unique server-side objects.
    */
   ExternalProcess& operator =( const ExternalProcess& ) = delete;

   /*!
    * Move constructor. Move semantics are disabled for %ExternalProcess
    * because of parent-child server-side object relations.
    */
   ExternalProcess( ExternalProcess&& ) = delete;

   /*!
    * Move assignment. Move semantics are disabled for %ExternalProcess because
    * of parent-child server-side object relations.
    */
   ExternalProcess& operator =( ExternalProcess&& ) = delete;

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since external processes are unique objects, calling this member function
    * has no effect.
    */
   void EnsureUnique() override
   {
   }

   /*!
    * Returns a reference to a null %ExternalProcess instance. A null
    * %ExternalProcess does not correspond to an existing external process in
    * the PixInsight core application.
    */
   static ExternalProcess& Null();

   /*!
    * Attempts to execute an external process.
    *
    * \param program    Path to the executable file.
    *
    * \param arguments  Optional list of command-line arguments that will be
    *                   passed to the executable. Arguments containing spaces
    *                   will be enclosed by quotes automatically. The default
    *                   value is an empty list.
    *
    * If the specified \a program cannot be executed for some reason (for
    * example, because the specified \a program does not exist, or because the
    * calling process doesn't have enough privileges), this function throws an
    * Error exception.
    *
    * This function implicitly opens the standard input, output and error I/O
    * streams (stdin, stdout and stderr, respectively) of the running process.
    * You can read from and write to the process using the Read() and Write()
    * member functions of %ExternalProcess, respectively. You can also close
    * the I/O streams using the CloseXX() member functions.
    */
   void Start( const String& program, const StringList& arguments = StringList() );

   /*!
    * Waits for the process to start execution.
    *
    * \param ms         Maximum waiting time in milliseconds. If a negative
    *                   value is specified, this function will never time out.
    *                   The default waiting time is 6000 ms (six seconds).
    *
    * This function suspends the calling task for at most the specified time,
    * or forever if a negative time value is specified. Returns true if the
    * process started before the specified time elapsed. Returns false if the
    * process timed out. In the latter case, this function can be called again
    * to continue waiting for the process to start.
    *
    * \warning When called from the root thread, this function can freeze the
    * entire PixInsight graphical interface. For asynchronous external process
    * execution, consider calling WaitForXX() functions from a separate thread.
    * Be aware that if you specify a large waiting time or a negative value,
    * this function may crash the calling thread.
    */
   bool WaitForStarted( int ms = 6000 );

   /*!
    * Waits for the running process to finish execution.
    *
    * \param ms         Maximum waiting time in milliseconds. If a negative
    *                   value is specified, this function will never time out.
    *                   The default waiting time is 6000 ms (six seconds).
    *
    * This function suspends the calling task for at most the specified time,
    * or forever if a negative time value is specified. Returns true if the
    * process finished its execution before the specified time elapsed. Returns
    * false if the process timed out. In the latter case, this function can be
    * called again to continue waiting for the process to finish.
    *
    * \warning When called from the root thread, this function can freeze the
    * entire PixInsight graphical interface. For asynchronous external process
    * execution, consider calling WaitForXX() functions from a separate thread.
    * Be aware that if you specify a large waiting time or a negative value,
    * this function may crash the calling thread.
    */
   bool WaitForFinished( int ms = 6000 );

   /*!
    * Waits for the running process to send data.
    *
    * \param ms         Maximum waiting time in milliseconds. If a negative
    *                   value is specified, this function will never time out.
    *                   The default waiting time is 6000 ms (six seconds).
    *
    * This function suspends the calling task for at most the specified time,
    * or forever if a negative time value is specified. Returns true if the
    * process sent some data through one of its output streams (standard output
    * or standard error) before the specified time elapsed. Returns false if
    * the process timed out. In the latter case, this function can be called
    * again to continue waiting for the process to generate output data. When
    * new output data is available from the process, it can be read immediately
    * by calling one of the Read() member functions.
    *
    * \warning When called from the root thread, this function can freeze the
    * entire PixInsight graphical interface. For asynchronous external process
    * execution, consider calling WaitForXX() functions from a separate thread.
    * Be aware that if you specify a large waiting time or a negative value,
    * this function may crash the calling thread.
    */
   bool WaitForDataAvailable( int ms = 6000 );

   /*!
    * Waits for the running process to receive data.
    *
    * \param ms         Maximum waiting time in milliseconds. If a negative
    *                   value is specified, this function will never time out.
    *                   The default waiting time is 6000 ms (six seconds).
    *
    * This function suspends the calling task for at most the specified time,
    * or forever if a negative time value is specified. Returns true if the
    * process received the data sent previously through its input stream (the
    * process' <em>standard input</em>) before the specified time elapsed.
    * Returns false if the process timed out. In the latter case, this function
    * can be called again to continue waiting for the process to fetch its
    * input data. Data can be sent to a running process by calling one of the
    * Write() member functions.
    *
    * \warning When called from the root thread, this function can freeze the
    * entire PixInsight graphical interface. For asynchronous external process
    * execution, consider calling WaitForXX() functions from a separate thread.
    * Be aware that if you specify a large waiting time or a negative value,
    * this function may crash the calling thread.
    */
   bool WaitForDataWritten( int ms = 6000 );

   /*!
    * Attempts to terminate the running process.
    *
    * On Linux/UNIX platforms (Linux, FreeBSD and Mac OS X) a SIGTERM signal is
    * sent to the process.
    *
    * On Windows, this function sends WM_CLOSE messages to all top-level
    * windows owned by the process and to its main thread. Console Windows
    * applications that don't have a message loop, or GUI Windows applications
    * that don't process WM_CLOSE messages adequately, cannot be terminated by
    * calling this member function. In these cases, the only way to terminate
    * the running process is by calling Kill().
    *
    * \note Note that there is no guarantee that the running process will
    * terminate after calling this function.
    */
   void Terminate();

   /*!
    * Forces termination of the running process.
    *
    * On Linux/UNIX platforms (Linux, FreeBSD and Mac OS X) a SIGKILL signal is
    * sent to the process.
    *
    * On Windows, the TerminateProcess() API function is called to force
    * process termination.
    *
    * \warning Unconditional termination of processes is always risky and
    * should be avoided. For example, the running program wil not have an
    * opportunity to close open files or save modified data. This function
    * should only be used as a last resort to terminate a process.
    */
   void Kill();

   /*!
    * Closes the standard input stream (stdin) of the running process.
    *
    * After calling this function, you no longer can write to the process using
    * one of the Write() member functions (doing so will always throw an Error
    * exception due to an I/O error).
    *
    * There are programs that require you to close the input stream explicitly.
    * An example is the 'cat' UNIX utility. For example, if you enter the 'cat'
    * command from a UNIX terminal, the program starts reading data from its
    * stdin stream (which is the terminal in this example) until you press the
    * "Ctrl+D" combination to close it.
    *
    * \note This function can only be called for a running process. If the
    * process is not running, an Error exception is thrown.
    */
   void CloseStandardInput();

   /*!
    * Closes the standard output stream (stdout) of the running process.
    *
    * After calling this function, you no longer can read data from the process
    * through its stdin stream. Calling this function can be useful to save
    * memory for processes that generate large amounts of output data, or if a
    * running process starts producing loads of data that you don't want to
    * process anymore.
    *
    * \note This function can only be called for a running process. If the
    * process is not running, an Error exception is thrown.
    */
   void CloseStandardOutput();

   /*!
    * Closes the standard error stream (stderr) of the running process.
    *
    * After calling this function, you no longer can read data from the process
    * through its stderr stream. Calling this function can be useful to save
    * memory for processes that generate large amounts of output data, or if a
    * running process starts producing loads of data that you don't want to
    * process anymore.
    *
    * \note This function can only be called for a running process. If the
    * process is not running, an Error exception is thrown.
    */
   void CloseStandardError();

   /*!
    * Redirects the standard output stream (stdout) of the process to the
    * specified file.
    *
    * \param filePath   Path to the file where the process will write its
    *                   standard output data.
    *
    * \param append     If true and the specified file already exists, its
    *                   contents will be preserved and newly generated data
    *                   will be appended. If false, the existing file will be
    *                   overwritten and its previous contents will be lost. The
    *                   default value is false (overwrite).
    *
    * Example:
    *
    * \code
    * ExternalProcess p1, p2;
    * p1.RedirectStandardOutput( "/tmp/foo.txt" );
    * p1.Start( "ls", StringList() << "-la" << "*.bar" );
    * \endcode
    *
    * The above code snippet is equivalent to this command:
    *
    * <tt>ls -la *.bar >/tmp/foo.txt</tt>
    *
    * \note This member function can only be called before running the process,
    * i.e. before calling Start(). Otherwise an Error exception is thrown.
    */
   void RedirectStandardOutput( const String& filePath, bool append = false );

   /*!
    * Redirects the standard output stream (stdout) of the process to the
    * specified process.
    *
    * \param process    Reference to a process where this process will write
    *                   its standard output data.
    *
    * This function can be used to build a \e pipe between two programs. For
    * example:
    *
    * \code
    * ExternalProcess p1, p2;
    * p1.RedirectStandardOutput( p2 );
    * p1.Start( "cat", StringList() << "*.bar" );
    * p2.Start( "grep", StringList() << "\'foo\'" );
    * \endcode
    *
    * would produce the same result as this command:
    *
    * <tt>cat *.bar |grep 'foo'</tt>
    *
    * \note This member function can only be called before running both
    * processes, i.e. before calling Start() for this process \e and the
    * specified \a process. Otherwise an Error exception is thrown.
    */
   void RedirectStandardOutput( ExternalProcess& process );

   /*!
    * Redirects the standard error output stream (stderr) of the process to the
    * specified file.
    *
    * \param filePath   Path to the file where the process will write its
    *                   standard error data.
    *
    * \param append     If true and the specified file already exists, its
    *                   contents will be preserved and newly generated data
    *                   will be appended. If false, the existing file will be
    *                   overwritten and its previous contents will be lost. The
    *                   default value is false (overwrite).
    *
    * For more information on redirection of process output, see the
    * RedirectStandardOutput() functions.
    */
   void RedirectStandardError( const String& filePath, bool append = false );

   /*!
    * Redirects the standard input stream (stdin) of the process from the
    * specified file.
    *
    * \param filePath   Path to an existing file from which the process will
    *                   read its standard input data. The current contents of
    *                   the file won't be modified or affected in any way by
    *                   calling this function.
    *
    * Example:
    *
    * \code
    * ExternalProcess p;
    * p.RedirectStandardInput( "/tmp/foo.txt" );
    * p.Start( "grep", StringList() << "\'bar\'" );
    * \endcode
    *
    * The above code snippet is equivalent to this command:
    *
    * <tt>grep 'bar' \</tmp/foo.txt</tt>
    *
    * \note This member function can only be called before running the process,
    * i.e. before calling Start(). Otherwise an Error exception is thrown.
    */
   void RedirectStandardInput( const String& filePath );

   /*!
    * Returns the working directory (aka folder) of this process.
    */
   String WorkingDirectory() const;

   /*!
    * Sets the working directory for this process.
    *
    * \param dirPath    Path to an existing directory, which will be the
    *                   working directory of the process when it is started.
    *
    * \note This member function can only be called before running the process,
    * i.e. before calling Start(). Otherwise an Error exception is thrown.
    */
   void SetWorkingDirectory( const String& dirPath );

   /*!
    * Returns true iff the process is running. An %ExternalProcess object
    * represents a running process briefly (usually) after calling its Start()
    * member function, when the IsStarting() function returns false, and just
    * before an OnStarted() event is generated.
    */
   bool IsRunning() const;

   /*!
    * Returns true iff the process is starting. An %ExternalProcess object
    * enters the starting state after calling its Start() member function, but
    * before the actual process is running.
    */
   bool IsStarting() const;

   /*!
    * Returns true iff the running process has crashed.
    */
   bool HasCrashed() const;

   /*!
    * Returns the platform-dependent Process Identifier (PID) of the running
    * process. See the ExternalProcess::pid_type type definition for more
    * information.
    *
    * If the process is not running this member function returns zero.
    */
   pid_type PID() const;

   /*!
    * Returns the exit code of the finished process. This is normally the
    * return value of the process' main() function.
    */
   int ExitCode() const;

   /*!
    * Returns the number of bytes waiting to be read from this running process.
    * If the process is not running, this member function returns zero.
    *
    * When this function returns a nonzero value, the available data can be
    * read by calling one of the Read() member functions.
    */
   size_type BytesAvailable() const;

   /*!
    * Returns the number of bytes pending to be written to this running
    * process. If the process is not running, this function returns zero.
    *
    * If this function returns a nonzero value, it is because one of the
    * Write() member functions has been called previously for this process with
    * nonempty data.
    */
   size_type BytesToWrite() const;

   /*!
    * Reads all the data available from the standard output stream (stdout) of
    * this process, and returns them as a ByteArray object.
    *
    * If the process has not sent any new data through its stdout stream, or if
    * the process is not running, an empty %ByteArray is returned. Note that
    * after the reading operation the stdout stream is cleared, i.e. the data
    * can only be read once.
    */
   ByteArray StandardOutput();

   /*!
    * Reads all the data available from the standard error stream (stderr) of
    * this process, and returns them as a ByteArray object.
    *
    * If the process has not sent any new data through its stderr stream, or if
    * the process is not running, an empty %ByteArray is returned. Note that
    * after the reading operation the stderr stream is cleared, i.e. the data
    * can only be read once.
    */
   ByteArray StandardError();

   /*!
    * Reads all the data available from the standard output and standard error
    * streams (stdout and stderr, respectively) of this process, and returns
    * them as a ByteArray object.
    *
    * If the process has not sent any new data through its output streams, or
    * if the process is not running, an empty %ByteArray is returned. Note that
    * after the reading operation, both stdout and stderr streams are cleared,
    * i.e. the data can only be read once.
    *
    * \note In the returned %ByteArray, the data from both stdout and stderr
    * streams will be mixed because they are gathered as they are generated. If
    * you need the data from both streams separately, call the StandardOutput()
    * and StandardError() member functions as appropriate.
    */
   ByteArray Read();

   /*!
    * Writes the specified \a data to the standard input stream (stdin) of this
    * process.
    */
   void Write( const ByteArray& data );

   /*!
    * Writes the specified \a text to the standard input stream (stdin) of this
    * process. The specified UTF-16 string will be treated as a sequence of
    * bytes, and the terminating null character of the string (two bytes) will
    * be excluded.
    */
   void Write( const String& text );

   /*!
    * Writes the specified \a text to the standard input stream (stdin) of this
    * process. The specified 8-bit string will be treated as a sequence of
    * bytes, and the terminating null character of the string will be excluded.
    */
   void Write( const IsoString& text );

   /*!
    * Writes the specified \a text to the standard input stream (stdin) of this
    * process. The specified 8-bit string will be treated as a sequence of
    * bytes, and the terminating null character of the string will be excluded.
    */
   void Write( const char* text );

   /*!
    * Writes \a count contiguous bytes starting at the specified \a data
    * location to the standard input stream (stdin) of this process.
    */
   void Write( const void* data, size_type count );

   /*!
    * Returns the list of environment variables for this process. Environment
    * variables are 'name=value' items that most programs can interpret to
    * modify their functionality.
    *
    * If no environment has been set for this process, an empty list is
    * returned.
    */
   StringList Environment() const;

   /*!
    * Sets the \a environment for this process.
    *
    * The environment is a list of 'name=value' elements, knwon as
    * <em>environment variables</em>, that most programs can interpret to
    * modify their functionality.
    *
    * If no environment is explicitly set for a process, it will inherit the
    * environment of the calling process.
    *
    * \note This member function can only be called before running the process,
    * i.e. before calling Start(). Otherwise an Error exception is thrown.
    */
   void SetEnvironment( const StringList& environment );

   /*!
    * \defgroup external_process_event_handlers ExternalProcess Event Handlers
    */

   /*!
    * Defines the prototype of a <em>process event handler</em>.
    *
    * A process event is generated when an %ExternalProcess instance changes
    * its state, or generates a simple notification.
    *
    * \param sender  The object that sends a process event.
    *
    * \ingroup external_process_event_handlers
    */
   typedef void (Control::*process_event_handler)( ExternalProcess& sender );

   /*!
    * Defines the prototype of a <em>process exit event handler</em>.
    *
    * A process exit event is generated when an %ExternalProcess instance
    * notifies that a running process has finished execution.
    *
    * \param sender     The object that sends a process exit event.
    *
    * \param exitCode   The process' exit code. Typically, this is the return
    *                   value of the main() function in the process.
    *
    * \param exitOk     True if the process finished execution normally. False
    *                   if the process crashed.
    *
    * \ingroup external_process_event_handlers
    */
   typedef void (Control::*process_exit_event_handler)( ExternalProcess& sender, int exitCode, bool exitOk );

   /*!
    * Defines the prototype of a <em>process error event handler</em>.
    *
    * A process error event is generated when an %ExternalProcess instance
    * reports an error condition, such as an I/O error, a nonexistent
    * executable file or lack of execution permissions.
    *
    * \param sender  The object that sends a process error event.
    *
    * \param error   Identifies the error condition that generated the event.
    *
    * \ingroup external_process_event_handlers
    */
   typedef void (Control::*process_error_event_handler)( ExternalProcess& sender, error_code error );

   /*!
    * Sets the handler for <em>process started</em> events generated by this
    * %ExternalProcess object. A process started event is generated when the
    * process has just started and is running.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive process started events
    *                   from this object.
    *
    * \ingroup external_process_event_handlers
    */
   void OnStarted( process_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>process finished</em> events generated by this
    * %ExternalProcess object. A process finished event is generated when the
    * running process has just finished execution.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive process finished events
    *                   from this object.
    *
    * \ingroup external_process_event_handlers
    */
   void OnFinished( process_exit_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>stdout data available</em> events generated by
    * this %ExternalProcess object. These events are generated when the running
    * process sends data through its standard output stream, and the data is
    * ready to be read.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive stdout data available
    *                   events from this object.
    *
    * \ingroup external_process_event_handlers
    */
   void OnStandardOutputDataAvailable( process_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>stderr data available</em> events generated by
    * this %ExternalProcess object. These events are generated when the running
    * process sends data through its standard error stream, and the data is
    * ready to be read.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive stderr data available
    *                   events from this object.
    *
    * \ingroup external_process_event_handlers
    */
   void OnStandardErrorDataAvailable( process_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>process error</em> events generated by this
    * %ExternalProcess object. A process error event is generated when the
    * process reports an error condition.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive process error events
    *                   from this object.
    *
    * \ingroup external_process_event_handlers
    */
   void OnError( process_error_event_handler handler, Control& receiver );

   /*!
    * Executes a program as a child process and waits for it to terminate.
    *
    * \param program    Path to an existing executable file.
    *
    * \param arguments  Optional list of command-line arguments that will be
    *                   passed to the executable. Arguments containing spaces
    *                   will be enclosed by quotes automatically. The default
    *                   value is an empty list.
    *
    * Returns the program's exit code. If the specified \a program cannot be
    * executed, this member function throws an Error exception.
    */
   static int ExecuteProgram( const String& program, const StringList& arguments = StringList() );

   /*!
    * Executes a program as an independent process.
    *
    * \param program    Path to an existing executable file.
    *
    * \param arguments  Optional list of command-line arguments that will be
    *                   passed to the executable. Arguments containing spaces
    *                   will be enclosed by quotes automatically. The default
    *                   value is an empty list.
    *
    * \param workingDirectory Path to an existing directory, which will be the
    *                   working directory of the process. The default value is
    *                   an empty string.
    *
    * The program is executed as a standalone process, also known as a
    * \e daemon on Linux/UNIX. It can continue running after the calling
    * application terminates.
    *
    * Returns the platform-dependent process identifier (PID) of the running
    * process. If the specified \a program cannot be executed, this member
    * function throws an Error exception.
    */
   static pid_type StartProgram( const String& program,
                                 const StringList& arguments = StringList(),
                                 const String& workingDirectory = String() );

private:

   struct EventHandlers
   {
      process_event_handler       onStarted                     = nullptr;
      process_exit_event_handler  onFinished                    = nullptr;
      process_event_handler       onStandardOutputDataAvailable = nullptr;
      process_event_handler       onStandardErrorDataAvailable  = nullptr;
      process_error_event_handler onError                       = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   ExternalProcess( void* );
   void* CloneHandle() const override;

   friend class ExternalProcessPrivate;
   friend class ExternalProcessEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ExternalProcess_h

// ----------------------------------------------------------------------------
// EOF pcl/ExternalProcess.h - Released 2019-01-21T12:06:07Z
