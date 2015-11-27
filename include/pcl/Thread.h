//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/Thread.h - Released 2015/11/26 15:59:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Thread_h
#define __PCL_Thread_h

/// \file pcl/Thread.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup thread_support Thread Support
 */

// ----------------------------------------------------------------------------

/*!
 * \namespace ThreadPriority
 * \brief     Thread scheduling priorities.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ThreadPriority::Inherit</td>      <td>Inherit the caller's thread priority. This is the default value.</td></tr>
 * <tr><td>ThreadPriority::Idle</td>         <td>Schedule when no other threads are busy.</td></tr>
 * <tr><td>ThreadPriority::Lowest</td>       <td>Schedule more often than \e idle priority, but less than \e low priority.</td></tr>
 * <tr><td>ThreadPriority::Low</td>          <td>Schedule more often than \e lowest priority, but less than \e normal priority.</td></tr>
 * <tr><td>ThreadPriority::Normal</td>       <td>Standard thread priority.</td></tr>
 * <tr><td>ThreadPriority::High</td>         <td>Schedule more often than \e normal priority, but less than \e highest priority.</td></tr>
 * <tr><td>ThreadPriority::Highest</td>      <td>Schedule more often than \e high priority, but less than \e time-critical priority.</td></tr>
 * <tr><td>ThreadPriority::TimeCritical</td> <td>Schedule as often as possible, taking precedence over any other threads.</td></tr>
 * <tr><td>ThreadPriority::DefaultMax</td>   <td>Default maximum priority. This is a platform-dependent, maximum thread priority recommended for processing modules.</td></tr>
 * </table>
 *
 * Note that not all platforms and operating systems handle thread scheduling
 * priorities in the same way. In general, modules should use the default
 * maximum thread priority by specifying ThreadPriority::DefaultMax instead of
 * hard-coded priority values. Time-critical priority should be reserved for
 * performance-critical, very brief and fast tasks exclusively, and should only
 * be used when it is \e really necessary. This is particularly important on
 * Windows platforms.
 *
 * \ingroup thread_support
 */
namespace ThreadPriority
{
   enum value_type
   {
      Inherit,       // Inherit caller's thread priority.
      Idle,          // Schedule when no other threads are busy.
      Lowest,
      Low,
      Normal,        // Standard thread priority
      High,
      Highest,
      TimeCritical,  // Schedule as often as possible, taking precedence over any other threads.
#ifdef __PCL_WINDOWS
      DefaultMax = Normal // On Windows, anything above Normal is the same as TimeCritical ...
#else
      DefaultMax = Highest
#endif
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class Thread
 * \brief Client-side interface to a PixInsight thread.
 *
 * ### TODO: Write a detailed description for %Thread
 *
 * \ingroup thread_support
 */
class PCL_CLASS Thread : public UIObject
{
public:

   /*!
    * Represents a thread priority.
    */
   typedef ThreadPriority::value_type  priority;

   /*!
    * Constructs a %Thread object.
    */
   Thread();

   /*!
    * Destroys a %Thread object.
    */
   virtual ~Thread()
   {
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since threads are unique objects by nature, calling this member function
    * has no effect.
    */
   virtual void EnsureUnique()
   {
      // Threads are unique objects by definition.
   }

   /*!
    * Returns a reference to a null %Thread instance.
    *
    * A null %Thread does not correspond to an existing thread in the core
    * PixInsight application.
    */
   static Thread& Null();

   /*!
    * Starts thread execution with the specified scheduling \a priority and CPU
    * affinity to the specified \a processor.
    *
    * After calling this member function, the PixInsight core application will
    * create a new execution thread from which the Run() virtual member
    * function will be invoked for this %Thread instance.
    *
    * By default, the calling thread's scheduling priority is inherited by
    * newly created threads. In general, to maximize performance in a way well
    * adapted to the running platform, ThreadPriority::DefaultMax should be
    * specified as the value of the \a priority parameter.
    *
    * By default, threads have no specific CPU affinity. By specifying a valid
    * \a processor index, a thread can be scheduled to run on a specific
    * logical processor. When the specified \a processor is >= 0, the internal
    * thread dispatcher will set the corresponding CPU affinity when this
    * thread is executed, just before calling its Run() virtual member
    * function. This is usually a more convenient way to control thread
    * affinity than the SetAffinity() member function, because it allows
    * setting CPU affinity before a thread starts execution.
    *
    * \note Currently, thread affinity can only be set on Linux and Windows
    * platforms. On FreeBSD and Mac OS X, specifying a processor with this
    * function has no effect. We hope to overcome this limitation in a future
    * version of PCL.
    *
    * \note Since version 1.8.0 of the PixInsight core application, nested
    * parallelism is fully supported. This means that multiple threads can be
    * run by calling Thread::Start() from either the root thread or any running
    * %Thread object. In versions prior to 1.8.0, a thread could only be run
    * from the root thread.
    */
   void Start( priority = ThreadPriority::Inherit, int processor = -1 );

   /*!
    * Returns a set of processor indices corresponding to this thread's
    * \e CPU \e affinity.
    *
    * The affinity of a thread defines the set of logical processors on which
    * the thread is eligible to run. Thread affinity allows to improve
    * execution speed by restricting each thread to run on a separate
    * processor. This prevents the performance cost caused by the cache
    * invalidation that occurs when a process ceases to execute on one
    * processor and then restarts execution on a different one (e.g. the
    * infamous <em>ping-pong effect</em>).
    *
    * If this thread is not running, this function returns the affinity of the
    * calling thread.
    *
    * \note Currently, this function only works on Linux. On FreeBSD, Mac OS X
    * and Windows platforms, this function always returns an empty array. We
    * hope to overcome this limitation in a future version of PCL.
    */
   Array<int> Affinity() const;

   /*!
    * Sets the affinity of this thread to the specified set of \a processors.
    *
    * Each element on the specified \a processors array is a zero-based index
    * corresponding to an existing logical processor. The first processor has
    * index zero and the last one has index n-1, where n is the total number of
    * logical processors on the host machine. Logical processors include
    * physical processors and processor cores, as well as logical processors on
    * systems with hyper-threading technology.
    *
    * Returns true iff the operation was performed correctly. In general, this
    * function returns false if this thread is not running, or if the specified
    * set contains nonexistent processor indices. It can also return false if
    * the calling process does not have the necessary privileges, altough this
    * should not happen under normal conditions.
    *
    * \note The CPU affinity of the calling thread cannot be changed with this
    * function. If this thread is not running, this function returns false and
    * does nothing.
    *
    * \note Currently, thread affinity can only be set on Linux and Windows
    * platforms. On FreeBSD and Mac OS X, calling this function has no effect.
    * We hope to overcome this limitation in a future version of PCL.
    */
   bool SetAffinity( const Array<int>& processors );

   /*!
    * Sets the CPU affinity of this thread to the specified \a processor.
    *
    * The specified \a processor is a zero-based index corresponding to an
    * existing logical processor. The first processor has index zero and the
    * last one has index n-1, where n is the total number of logical processors
    * on the host machine. Logical processors include physical processors and
    * processor cores, as well as logical processors on systems with
    * hyper-threading technology.
    *
    * Returns true iff the operation was performed correctly. In general, this
    * function returns false if this thread is not running, or if the specified
    * processor does not exist. It can also return false if the calling process
    * does not have the necessary privileges, altough this should not happen
    * under normal conditions.
    *
    * \note The CPU affinity of the calling thread cannot be changed with this
    * function. If this thread is not running, this function returns false and
    * does nothing.
    *
    * \note Currently, thread affinity can only be set on Linux and Windows
    * platforms. On FreeBSD and Mac OS X, calling this function has no effect.
    * We hope to overcome this limitation in a future version of PCL.
    */
   bool SetAffinity( int processor );

   /*!
    * Forces immediate termination of thread execution.
    *
    * \warning Calling this function is dangerous because it causes the
    * immediate termination of a running thread without giving it a chance to
    * perform any cleanup. For example, open files will not be closed, locked
    * objects will not be unlocked, and shared static variables may be left at
    * invalid states, with unpredictable results.
    *
    * \warning <b>Calling this member function is strongly discouraged - Use it
    * at your own risk</b>
    */
   void Kill(); // ### Dangerous - Use at your own risk! ###

   /*!
    * Returns true iff this thread is running.
    */
   bool IsActive() const;

   /*!
    * Returns this thread's priority.
    */
   priority Priority() const;

   /*!
    * Sets this thread's priority.
    */
   void SetPriority( priority );

   /*!
    * Resets this thread's priority to the default value, which inherits the
    * calling thread's priority.
    */
   void ResetPriority()
   {
      SetPriority( ThreadPriority::Inherit );
   }

   /*!
    * Suspends execution of the calling thread until this thread terminates
    * execution. If this thread is not running, this function returns
    * immediately.
    *
    * \warning If this thread does not return from its Run() member function,
    * for example because this thread hangs or enters an infinite loop, the
    * thread that calls this function will remain suspended forever (crashed).
    */
   void Wait();

   /*!
    * Suspends execution of the calling thread until one of the following
    * conditions is met:
    *
    * \li This thread terminates execution, or this thread is not running.
    * \li The specified time interval \a ms in milliseconds has elapsed.
    *
    * This member function returns true if this thread has finished execution
    * before the specified time \a ms in milliseconds has elapsed. It also
    * returns true if this thread is not running.
    *
    * \note Unlike Wait(), this function cannot crash the calling thread if
    * this thread is crashed, unless a huge amount of time is specified as the
    * function argument.
    */
   bool Wait( unsigned ms );

   /*!
    * Suspends execution of this thread during the specified time interval
    * \a ms in milliseconds.
    */
   void Sleep( unsigned ms );

   /*
    * Returns a reference to the current thread (the thread from which this
    * member function is invoked), or Thread::Null() if the current thread is
    * either (a) a thread not being controlled by the PixInsight API, or (b) a
    * thread that has been created by another module.
   static Thread& CurrentThread();
    */

   /*!
    * This member function returns true if and only if it is called from the
    * <em>root thread</em>. The root thread is the thread where the graphical
    * user interface is running on the PixInsight core application.
    *
    * \note Since version 1.8.0 of the PixInsight core application, nested
    * parallelism is fully supported. This means that multiple threads can be
    * run by calling Thread::Start() from either the root thread or any running
    * %Thread object. In versions prior to 1.8.0, a thread could only be run
    * from the root thread.
    */
   static bool IsRootThread();

   /*!
    * %Thread execution routine.
    *
    * This member function is invoked by the PixInsight core application upon
    * execution start, just after calling the Start() member function. During
    * execution of this routine, it is said that this is a <em>running
    * thread</em>.
    *
    * Derived classes must reimplement this member function to provide specific
    * thread functionality.
    *
    * \note There is no problem at all in calling this member function
    * directly. This may be useful, for example, if the same code must be
    * executed in a single thread (without calling Start()) and as
    * multithreaded code.
    */
   virtual void Run()
   {
      // Reimplement this function to provide your thread's functionality.
   }

   /*!
    * Returns the current status of this thread. The thread status is a 32-bit
    * unsigned integer number that can be set for an active thread by calling
    * its SetStatus() member function.
    *
    * Thread status is primarily intended as an efficient mechanism to send
    * custom messages to running threads, for thread synchronization or other
    * control purposes.
    *
    * \note This member function is thread-safe. It can be safely called for a
    * running thread.
    *
    * \note This member function blocks the caller's execution until the thread
    * status can be retrieved. Hence, the operation performed by this function
    * is expensive in terms of thread synchronization. For a non-blocking
    * version of this function see the documentation for TryGetStatus().
    */
   uint32 Status() const;

   /*!
    * Attempts to get the current status of this thread without blocking the
    * caller's execution.
    *
    * Returns true iff the thread status could be retrieved and stored in the
    * specified \a status variable.
    *
    * Returns false if the status couldn't be read. In this case the value of
    * the \a status variable won't be changed.
    *
    * \note This member function is thread-safe. It can be safely called for a
    * running thread.
    *
    * \note This is a non-blocking fast version of the Status() member
    * function. It should be used when acquiring the thread's status is not
    * strictly necessary to continue thread execution.
    */
   bool TryGetStatus( uint32& status ) const;

   /*!
    * Sets the current status of a running thread. See the documentation of
    * Thread::Status() for more information.
    *
    * \note Calling this function for an inactive thread has no effect. The
    * thread status can only be set for a running thread.
    *
    * \note If the high-order bit of \a status is set, an abort message will be
    * sent to the running thread. See the Abort() member function for more
    * information.
    *
    * \note This member function is thread-safe. It can be safely called for a
    * running thread, even from other running threads.
    */
   void SetStatus( uint32 status );

   /*!
    * Sends an abort message to a running thread.
    *
    * If the thread calls Module->ProcessEvents() after an abort message has
    * been sent, or if it uses some of the standard status monitoring classes
    * (such as StandardStatus for example), a ProcessAborted exception will be
    * thrown automatically in the thread. The exception will be thrown in the
    * (reimplemented) Thread::Run() member function, where it should be caught
    * and used to terminate thread execution by returning from Run(). This
    * mechanism can be used to synchronize and stop running threads in a
    * controlled and thread-safe way.
    *
    * \note A thread abort message means that the high-order bit of a running
    * thread's status has been set. This member function simply sets the thread
    * status to 0x80000000. To effectively abort the thread, it should call
    * Module->ProcessEvents(), as described above, and should stop its
    * execution after catching a ProcessAborted exception.
    *
    * \note This member function is thread-safe. It can be safely called for a
    * running thread, even from other running threads.
    */
   void Abort()
   {
      SetStatus( 0x80000000 );
   }

   /*!
    * Returns true iff this thread has been aborted. A thread has been aborted
    * if it has received an abort message by a previous call to Abort(), or by
    * setting the high-order bit of its thread status word.
    *
    * \note This member function is thread-safe. It can be safely called for a
    * running thread.
    *
    * \note This function calls Status() to read the current thread's status.
    * Consequently, it blocks the caller's execution and has the same
    * performance problems. For a non-blocking alternative, see TryIsAborted().
    */
   bool IsAborted() const
   {
      return (Status() & 0x80000000) != 0;
   }

   /*!
    * Returns true iff this thread has been aborted, and the current thread's
    * status could be retrieved without blocking the caller's execution.
    *
    * \note This member function is thread-safe. It can be safely called for a
    * running thread.
    *
    * \note This is a non-blocking fast version of the IsAborted() member
    * function. It should be used when acquiring the thread's status is not
    * strictly necessary to continue thread execution.
    */
   bool TryIsAborted() const
   {
      uint32 status;
      return TryGetStatus( status ) && (status & 0x80000000) != 0;
   }

   /*!
    * Returns the console output text currently accumulated in this thread.
    *
    * When a %Thread object is running (that is, when its IsActive() member
    * function returns true), no operation on the graphical user interface is
    * permitted from the thread. This includes console text output.
    *
    * When a running thread writes text to the console, for example by calling
    * Console::Write() or Console::WriteLn(), the text is not sent to the
    * console (which in fact would raise an Error exception), but it is instead
    * appended to the thread's <em>console output text</em>. The accumulated
    * output text can be sent to the console once the thread has finished
    * execution, or it can be retrieved by calling this member function.
    */
   String ConsoleOutputText() const;

   /*!
    * Returns true iff this thread has any accumulated console output text that
    * still has not been flushed or cleared.
    *
    * For information on thread console output, refer to the documentation for
    * the ConsoleOutputText() member function.
    */
   bool HasConsoleOutputText() const
   {
      return !ConsoleOutputText().IsEmpty();
   }

   /*!
    * Erases the accumulated console output text in this thread.
    *
    * For information on thread console output, refer to the documentation for
    * the ConsoleOutputText() member function.
    */
   void ClearConsoleOutputText();

   /*!
    * Sends any accumulated console output text in this thread to the console.
    *
    * After writing the accumulated text to the console, this member function
    * clears it, so that repeated calls to this function will have no effect.
    *
    * For information on thread console output, refer to the documentation for
    * the ConsoleOutputText() member function.
    *
    * \note This member function must only be called from the root thread.
    * Calling it from a running %Thread object has no effect.
    */
   void FlushConsoleOutputText();

   /*!
    * Returns the maximum number of threads that can be used concurrently to
    * process a set of items.
    *
    * \param count   Number of <em>processing units</em>. A processing unit can
    *             be a single pixel, a row of pixels, or any suitable item,
    *             according to the task being performed by the caller.
    *
    * \param overheadLimit    Thread overhead limit in processing units. The
    *             function returns a maximum number of threads such that no
    *             thread would have to process less processing units than this
    *             value. The default overhead limit is 16 processing units.
    *
    * This function takes into account the number of existing processors in
    * the system, as well as the maximum number of processors currently allowed
    * for external processes by the core application. The following global
    * variables are taken into account (see the PixInsightSettings class for
    * more information about global variables on the PixInsight platform):
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>Process/EnableParallelProcessing</td>
    *     <td>If this global flag is false, it means that parallel processing
    *     has been globally disabled for the entire platform, so this function
    *     will always return one, irrespective of the number of existing
    *     processors.</td></tr>
    *
    * <tr><td>Process/EnableParallelModuleProcessing</td>
    *     <td>If this global flag is false, it means that parallel processing
    *     has been disabled for all installed modules, so this function will
    *     always return one, as in the previous case.</td></tr>
    *
    * <tr><td>Process/MaxProcessors</td>
    *     <td>This global integer variable is the maximum number of processors
    *     allowed for installed modules, which is always less than or equal to
    *     the number of existing processors in the system. This function will
    *     never return a number of threads greater than the value of this
    *     variable.</td></tr>
    * </table>
    *
    * The <em>number of processors</em> term refers to the number of existing
    * <em>logical processors</em> in the system. These include all physical
    * processors in multiprocessor systems, as well as all existing processor
    * cores in multicore processors, and virtual processors in systems with
    * HyperThreading technology.
    *
    * \note A module must always call this function before trying to execute
    * multiple threads concurrently in any execution context, and should never
    * try to run more threads simultaneously than the amount returned by this
    * function. Failure to follow these rules will invalidate a module for
    * certification.
    *
    * \note Since version 1.8.0 of the PixInsight core application, nested
    * parallelism is fully supported. This means that multiple threads can be
    * run from a running thread. The calling module is responsible for ensuring
    * that the maximum number of threads allowed by the platform (see the
    * global variables in the table above) will never be surpassed.
    */
   static int NumberOfThreads( size_type count, size_type overheadLimit = 16u );

private:

   Thread( void* );
   virtual void* CloneHandle() const;

   int    m_processorIndex;
   String m_consoleOutputText;

   friend class ThreadDispatcher;
};

// ----------------------------------------------------------------------------

/*!
 * Suspends the calling thread from execution until the specified time \a ms in
 * milliseconds has elapsed, or until a signal is delivered to the calling
 * thread that terminates the process or causes activation of a signal-catching
 * routine.
 * \ingroup thread_support
 */
void PCL_FUNC Sleep( unsigned ms );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Thread_h

// ----------------------------------------------------------------------------
// EOF pcl/Thread.h - Released 2015/11/26 15:59:39 UTC
