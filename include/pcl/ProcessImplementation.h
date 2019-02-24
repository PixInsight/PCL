//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/ProcessImplementation.h - Released 2019-01-21T12:06:07Z
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

#ifndef __PCL_ProcessImplementation_h
#define __PCL_ProcessImplementation_h

/// \file pcl/ProcessImplementation.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/ImageWindow.h>
#include <pcl/MetaProcess.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS MetaParameter;
class PCL_CLASS ProcessInterface;
class PCL_CLASS View;
class PCL_CLASS ImageWindow;

// ----------------------------------------------------------------------------

/*!
 * \class ProcessImplementation
 * \brief Implementation of a PixInsight process instance.
 *
 * %Process instances are the main actors in the PixInsight environment, since
 * they are responsible for all of the actual processing work. For example,
 * process instances can be executed on views, on images and on the global
 * context, edited through process interfaces, parsed by script interpreters,
 * stored in processing histories and process containers, and managed as
 * process icons and projects that can be transferred to and from special disk
 * files (XPSM and XOSM formats).
 *
 * In the PixInsight/PCL framework, a <em>process class</em> is formally
 * defined as a descendant of the MetaProcess class, along with several
 * descendants of MetaParameter classes. %ProcessImplementation defines the
 * behavior and functionality of an instance of a process class.
 *
 * \sa MetaProcess, MetaParameter
 */
class PCL_CLASS ProcessImplementation
{
public:

   /*!
    * Constructs a process instance.
    *
    * \param m    Pointer to a metaprocess that identifies the process class
    *             that this process instance belongs to.
    */
   ProcessImplementation( const MetaProcess* m );

   /*!
    * Copy constructor. Constructs a new process instance as a duplicate of an
    * existing instance.
    */
   ProcessImplementation( const ProcessImplementation& x );

   /*!
    * Destroys this process instance.
    */
   virtual ~ProcessImplementation();

   /*!
    * Returns a pointer to the metaprocess of this process instance.
    *
    * The metaprocess defines the process class this instance belongs to.
    */
   const MetaProcess* Meta() const
   {
      return meta;
   }

   /*!
    * Initializes this process instance after construction. This is knwon as
    * <em>special initialization</em> of process instances.
    *
    * The PixInsight core application calls this function to initialize an
    * instance just after it has been created by its class constructor.
    *
    * There are cases where the entire initialization work required for a
    * process instance cannot be accomplished in the instance class constructor
    * (e.g., because it requires calling reimplemented virtual functions from a
    * base class). In such cases, this function can be reimplemented to perform
    * the pending initialization tasks immediately after instance construction.
    *
    * This function is only called if the associated metaprocess class
    * reimplements MetaProcess::NeedsInitialization() to return true.
    */
   virtual void Initialize();

   /*!
    * Returns true iff this process instance is currently valid for execution.
    *
    * \param[out] info  If this function returns false, it should return also a
    *                   brief text (128 characters maximum) in this string,
    *                   describing why this is an invalid instance.
    *
    * The PixInsight core application calls this function to validate an
    * instance just before attempting to execute it, either on a view or in the
    * global context.
    *
    * This function is only called if the associated metaprocess class
    * reimplements MetaProcess::NeedsValidation() to return true.
    *
    * If this function returns false, giving a brief description of the reasons
    * in the \a info string is not mandatory, but neglecting it is considered
    * bad programming practice.
    */
   virtual bool Validate( String& info );

   /*!
    * Assigns an existing instance \a i to this instance, if it belongs to an
    * assignable process class.
    *
    * The PixInsight core application calls this function each time a process
    * instance has to be replaced with a copy of another existing instance.
    * This happens under a wide variety of situations. A good example is
    * copying a process icon by dragging it to another icon. Another, less
    * visible example is when a view's processing history is being copied as
    * the initial processing of a duplicate view.
    *
    * Assignment only occurs for \e assignable processes. A process is
    * assignable unless its defining MetaProcess descendant class reimplements
    * the MetaProcess::IsAssignable() member function to return false.
    *
    * Assignable process instance implementation classes must reimplement this
    * function, or a runtime exception will be thrown. This means that
    * virtually \e all descendants classes of %ProcessImplementation must
    * reimplement this function. A plausible reason to define a non-assignable
    * process (perhaps the only one) is because its instances have no data at
    * all that could be assigned - an example is the standard Invert process.
    */
   virtual void Assign( const ProcessImplementation& i );

   /*!
    * Returns true iff this process instance could be successfully executed on a
    * given \a view.
    *
    * \param view    Reference to a view on which this process is being tested
    *                to validate execution.
    *
    * \param[out] whyNot   If this function returns false, it should return
    *                also a brief text (128 characters maximum) in this string,
    *                explaining why this instance cannot be executed on the
    *                passed \a view.
    *
    * In general, among many other possible reasons, a process instance class
    * should reimplement this function if:
    *
    * \li It is a process that cannot be executed on views. In this case, the
    * reimplementation of this function should always return false.
    *
    * \li It cannot be executed on \e all views. For example, some processes
    * can only be executed on main views, but not on previews. Or perhaps this
    * process cannot be executed on color images, but only on grayscale images.
    *
    * \li Some instances, perhaps depending on specific parameter values or
    * user-selectable options, may have limitations or cannot be executed on
    * some types of views or images.
    *
    * If this function returns false, giving a brief description of the reasons
    * in the \a whyNot string is not mandatory, but neglecting it is considered
    * bad programming practice.
    *
    * \note The default implementation of this function returns true, so by
    * default a process instance can be executed on any view.
    */
   virtual bool CanExecuteOn( const View& view, String& whyNot ) const;

   /*!
    * Returns true iff this process instance would require a history update for
    * the specified \a view, if it was executed on it.
    *
    * A history update would be required if this instance was modifying the
    * \a view or its image in \e any way. By <em>any way</em> we mean not only
    * changing pixel values, but to alter whatever property of the view or its
    * image.
    *
    * \note The default implementation of this function returns true, so by
    * default a process instance is supposed to update all views.
    */
   virtual bool IsHistoryUpdater( const View& view ) const
   {
      return true; // by default, all processes are supposed to update images.
   }

   /*!
    * Returns true iff this is a <em>maskable process instance</em> for the
    * specified \a view.
    *
    * \param view Reference to a view on which this process is being tested to
    *             validate maskable execution.
    *
    * \param mask Reference to an image window whose image would act as a mask
    *             after execution of this process instance.
    *
    * A maskable instance is one whose resulting processed image can be masked.
    * When a target image is masked after execution of a process instance,
    * white mask pixels allow replacement of target pixels with fully processed
    * values, and black mask pixels preserve original target pixel values. Gray
    * mask pixel values yield proportional combinations of original and
    * processed target pixels.
    *
    * If a process cannot be masked, or if some process instances can't be
    * masked sometimes, then this function should be reimplemented to return
    * false, or an appropriate value depending on the execution context.
    *
    * \note The default implementation of this function returns true, so by
    * default a process instance is supposed to be maskable for all views.
    */
   virtual bool IsMaskable( const View& view, const ImageWindow& mask ) const
   {
      return true; // by default, all processes are supposed to be maskable.
   }

   /*!
    * Returns a combination of flags indicating the required contents of a
    * history update record for the specified \a view if this instance was
    * executed on it.
    *
    * The returned value is an OR'ed combination of flags that must be
    * specified with the symbolic constants defined in the UndoFlag namespace.
    *
    * When reimplementing this function, you should include flags for all data
    * items that your process instances modify in target views. For example, if
    * a process instance modifies the FITS header keywords of target images,
    * the reimplementation of this function should include the
    * UndoFlag::Keywords flag.
    *
    * \note The default implementation of this function returns
    * UndoFlag::DefaultMode. This value selects a default set of items,
    * including pixel data, that will be stored in a history record for the
    * specified \a view before executing this process instance on it. You
    * normally won't need to reimplement this function, since the default
    * history data set is appropriate for most processes.
    */
   virtual UndoFlags UndoMode( const View& view ) const
   {
      return UndoFlag::DefaultMode;
   }

   /*!
    * This function is called just before execution of this process instance on
    * the specified \a view. Returns true if execution can continue.
    *
    * This function is the last chance for a process instance to prevent
    * execution on a particular view.
    *
    * For example, a process that destroys the previews defined in the image
    * window of a target view should always ask the user if it is OK to do so
    * before execution. In this case, this function would be reimplemented to
    * open a standard yes/no message box asking something like <em>"About to
    * destroy all previews in this image window. Are you sure?"</em> to the
    * user. False should be returned if the user answered "No".
    *
    * \note The default implementation of this function returns true, so by
    * default execution on views is not stopped by this function.
    */
   virtual bool BeforeExecution( View& view )
   {
      return true;
   }

   /*!
    * View execution routine. Executes this process instance on the specified
    * target \a view. Returns true if execution took place successfully; false
    * in the event of error.
    *
    * This function must be reimplemented in process instance classes that can
    * be executed on views. By default, all instances are supposed to be
    * executable on views. To prevent view execution, an instance class should
    * reimplement the CanExecuteOn() member function.
    *
    * Exceptions can be freely thrown from this function; they will be caught
    * and handled automatically by core PCL routines. For this reason,
    * reimplementations of this function rarely return false when an error
    * occurs; they prefer throwing exceptions instead.
    *
    * \note The default implementation of this function throws a runtime
    * exception. This has been done on purpose to recall you that this function
    * must be reimplemented for processes with view execution capabilities.
    */
   virtual bool ExecuteOn( View& view );

   /*!
    * This function is called just after execution of this process instance on
    * the specified \a view.
    *
    * %ProcessImplementation instance classes rarely reimplement this function.
    * It can be useful as a place to perform some cleanup work after execution.
    *
    * When this function is invoked, the specified \a view has been completely
    * processed. This includes a full history update (if appropriate), all
    * involved notifications sent, and its screen rendition fully regenerated.
    */
   virtual void AfterExecution( View& view )
   {
   }

   /*!
    * Returns true iff this process instance could be successfully executed on
    * the passed \a image.
    *
    * \param image   Reference to an ImageVariant object on which this process
    *                is being tested to validate execution.
    *
    * \param[out] whyNot   If this function returns false, it should return
    *                also a brief text (128 characters maximum) in this string,
    *                explaining why this instance  be executed on the
    *                passed \a image.
    *
    * This function will only be called for processes able to be executed on
    * standalone images, that is, for processes that reimplement the following
    * member function:
    *
    * bool MetaProcess::CanProcessImages() const
    *
    * to return true. Note that the default implementation of the above
    * function returns false, so a process cannot be executed on %ImageVariant
    * instances by default.
    *
    * Don't confuse standalone \e image execution with \e view execution, which
    * corresponds to the ExecuteOn( View& ) member function.
    *
    * \note The default implementation of this function returns true, so by
    * default an instance of a process able to be executed on standalone images
    * can be executed on any image.
    */
   virtual bool CanExecuteOn( const ImageVariant& image, String& whyNot ) const;

   /*!
    * Image execution routine. Executes this process instance on the specified
    * target \a image. Returns true if execution was successful; false in the
    * event of error.
    *
    * \param image   Reference to an ImageVariant object that transports the
    *                image to be processed.
    *
    * \param hints   A string containing a (possibly empty) list of \e hints
    *          intended to modify the way an image is processed. A process can
    *          simply ignore all of these hints, or just look for one or more
    *          hints that it recognizes and supports, ignoring others. When two
    *          or more hints are specified, they must be separated by space
    *          characters (0x20). File formats also support hints in a similar
    *          way; see for example the FileFormatInstance::Open() member
    *          function for more information.
    *
    * This function must be reimplemented by processes that can be executed on
    * standalone images. To enable this functionality, the following member
    * function:
    *
    * bool MetaProcess::CanProcessImages() const
    *
    * must be reimplemented to return true, since it returns false by default.
    * Otherwise this function will never be called.
    *
    * By default, an instance of a process able to be executed on standalone
    * images is supposed to be executable on \e any image. To prevent image
    * execution selectively, for example as a function of image properties, an
    * instance class should reimplement:
    *
    * bool ProcessImplementation::CanExecuteOn( const ImageVariant&, String& ) const
    *
    * Exceptions can be freely thrown from this function; they will be caught
    * and handled automatically by core PCL routines. For this reason,
    * reimplementations of this function rarely return false when an error
    * occurs; they prefer throwing exceptions instead.
    *
    * \note The default implementation of this function throws a runtime
    * exception. This has been done on purpose to recall you that this function
    * must be reimplemented for processes with image execution capabilities.
    */
   virtual bool ExecuteOn( ImageVariant& image, const IsoString& hints );

   /*!
    * Returns true iff this process instance could be successfully executed in
    * the global context.
    *
    * \param[out] whyNot   If this function returns false, it should return
    *             also a brief text (128 characters maximum) in this string,
    *             explaining why this instance couldn't be executed in the
    *             global context.
    *
    * Execution in the global context occurs when a process instance is
    * executed without any specific target view. In such cases we say that a
    * <em>global process</em> is being executed.
    *
    * A good example of a \e pure global process is the GlobalPreferences
    * process. This process allows the user modifying a number of global
    * application options, but it does not apply to any image.
    *
    * Other processes can perform as global processes sometimes, depending on
    * specific parameter values. An example is the RGBWorkingSpaceParameters
    * process, which can be applied to change a view's RGB working space
    * (RGBWS), or to set the parameters of the global RGBWS.
    *
    * Processes willing to perform global execution, either on a regular basis
    * or under specific circumstances, must reimplement this function.
    *
    * \note The default implementation of this function returns false and
    * assigns a standard message to the \a whyNot string (something like
    * <em>"The xxx process cannot be executed in the global context"</em>). So
    * by default a process instance cannot be executed in the global context.
    */
   virtual bool CanExecuteGlobal( String& whyNot ) const;

   /*!
    * This function is called just before execution of this process instance in
    * the global context. Returns true if global execution can continue.
    *
    * This function is the last chance for a process instance to cancel
    * execution in the global context. Reimplement it if you need to do
    * something that the user should be aware of before global execution,
    * giving her an opportunity to stop this process.
    *
    * \note The default implementation of this function returns true, so by
    * default global execution is not stopped by this function.
    */
   virtual bool BeforeGlobalExecution()
   {
      return true;
   }

   /*!
    * Global execution routine. Executes this process instance in the global
    * context. Returns true if execution took place successfully; false in
    * the event of error.
    *
    * This function must be reimplemented in process instance classes that can
    * be executed globally. By default, instances cannot be executed globally.
    * To allow global execution, an instance class should reimplement the
    * CanExecuteGlobal() member function.
    *
    * Exceptions can be freely thrown from this function; they will be caught
    * and handled automatically by core PCL routines. For this reason,
    * reimplementations of this function rarely return false when an error
    * occurs; they prefer throwing exceptions instead.
    *
    * \note The default implementation of this function throws a runtime
    * exception. This has been done on purpose to recall you that this function
    * must be reimplemented for processes with global execution capabilities.
    */
   virtual bool ExecuteGlobal();

   /*!
    * This function is called just after execution of this process instance in
    * the global context.
    *
    * %ProcessImplementation instance classes rarely reimplement this function.
    * It can be useful as a place to perform some cleanup work after execution.
    */
   virtual void AfterGlobalExecution()
   {
   }

   /*!
    * Returns the address of an interface appropriate to edit this process
    * instance. Returns zero if there is no available interface for this
    * process instance.
    *
    * This function is called when an existing instance is activated in the
    * core application's GUI (e.g., by double-clicking a process icon). When
    * the process is selected in a generic way (e.g., from the %ProcessImplementation
    * Explorer window), MetaProcess::DefaultInterface() is called instead.
    *
    * This function must be reimplemented if multiple interfaces are
    * available for this process class that are selectable on a per-instance
    * basis.
    *
    * \note Reimplementing this function is very infrequent. This is because
    * the default implementation just returns MetaProcess::DefaultInterface(),
    * which is adequate for processes associated to a single interface, as are
    * the majority of them.
    */
   virtual ProcessInterface* SelectInterface() const
   {
      return meta->DefaultInterface(); // by default, process instances use
                                       //   their default interfaces.
   }

   /*!
    * Returns true iff the specified interface is a valid interface for this
    * process instance.
    *
    * This function is called when this instance is about to be imported by a
    * process interface, to validate the import operation.
    *
    * \note This function is rarely reimplemented, for similar reasons to those
    * exposed in the documentation for SelectInterface(). The default
    * implementation of this function returns true if the specified interface
    * is the same one that is returned by SelectInterface().
    */
   virtual bool IsValidInterface( const ProcessInterface* i ) const
   {
      return i == SelectInterface();
   }

   /*!
    * This function is called just before reading this process instance from an
    * input stream, as a disk file. Returns true if the reading procedure can
    * continue.
    *
    * This function is not intended to allow or disallow instance reading
    * operations based on source data contents. For that reason, this function
    * doesn't receive a reference to the input stream from which this instance
    * is about to be read.
    *
    * The purpose of this function is preparing process instances to receive
    * new parameters. This allows for a high degree of optimization in the
    * implementation of actual parameter data and their acquisition.
    *
    * \note The default implementation of this function returns true, so by
    * default instance reading is not stopped by this function.
    */
   virtual bool BeforeReading()
   {
      return true;
   }

   /*!
    * This function is called just after this process instance has been read
    * from an input stream, as a disk file.
    *
    * You usually should need a reimplementation of this function if you also
    * reimplemented BeforeReading(). Read the documentation for that member
    * function for more information.
    *
    * \note The default implementation of this function does nothing.
    */
   virtual void AfterReading()
   {
   }

   /*!
    * This function is called just before writing this process instance to an
    * output stream, as a disk file. Returns true if the writing procedure can
    * continue.
    *
    * This function is not intended to allow or disallow instance writing
    * operations based on target stream contents or properties. For that
    * reason, this function doesn't receive a reference to the output stream
    * to which this instance is about to be written.
    *
    * The purpose of this function is preparing process instances to export
    * their parameters. This allows for a high degree of optimization in the
    * implementation of actual parameter data and their availability.
    *
    * \note The default implementation of this function returns true, so by
    * default instance writing is not stopped by this function.
    */
   virtual bool BeforeWriting() const
   {
      return true;
   }

   /*!
    * This function is called just after this process instance has been written
    * to an output stream, as a disk file.
    *
    * You usually should need a reimplementation of this function if you also
    * reimplemented BeforeWriting(). Read the documentation for that member
    * function for more information.
    *
    * \note The default implementation of this function does nothing.
    */
   virtual void AfterWriting() const
   {
   }

   /*!
    * Returns the fixed address of an actual parameter in this process
    * instance, and locks it while the core application requires direct access
    * to its data.
    *
    * \param p    The address of a metaparameter that identifies the parameter
    *             being locked.
    *
    * \param tableRow   The index of a table row where the requested parameter
    *             is defined. For parameters not belonging to tables, this
    *             parameter must be ignored.
    *
    * The PixInsight core application calls this function to get the actual
    * address of a parameter in this process instance. The parameter should not
    * be destroyed, changed or moved physically in memory until the
    * UnlockParameter() member function is invoked for this instance with the
    * same \a p and \a tableRow arguments.
    *
    * Once UnlockParameter() has been called for a specific parameter, it can
    * be freely moved, allocated or deallocated by this process instance.
    *
    * \note Reimplementing this function is \e mandatory for processes defining
    * parameters.
    */
   virtual void* LockParameter( const MetaParameter* p, size_type tableRow );

   /*!
    * This function is called by the core application after LockParameter(),
    * when direct access to an specified actual parameter is no longer needed.
    *
    * \param p    The address of a metaparameter that identifies the parameter
    *             being unlocked.
    *
    * \param tableRow   The index of a table row where the requested parameter
    *             is defined. For parameters not belonging to tables, this
    *             parameter must be ignored.
    *
    * When this function is invoked, the instance is free to move or change the
    * parameter, until the next call to LockParameter() with the same \a p and
    * \a tableRow arguments.
    *
    * \note This function is not called if the corresponding
    * MetaParameter::NeedsUnlocking() member function has not been
    * reimplemented to return true in the derived class that the specified
    * parameter \a *p is an instance of.
    */
   virtual void UnlockParameter( const MetaParameter* p, size_type tableRow );

   /*!
    * This function is called to validate a parameter value for an actual
    * parameter in this process instance. Returns true if the specified value
    * is a valid value for the specified parameter in this instance.
    *
    * \param value   Pointer to the parameter value being tested.
    *
    * \param p       The address of a metaparameter that identifies the
    *                parameter being validated.
    *
    * \param tableRow   The index of a table row where the requested parameter
    *                is defined. For parameters not belonging to tables, this
    *                parameter must be ignored.
    *
    * \note This function is not called if the corresponding
    * MetaParameter::NeedsValidation() member function has not been
    * reimplemented to return true in the derived class that the specified
    * parameter \a *p is an instance of.
    */
   virtual bool ValidateParameter( void* value, const MetaParameter* p, size_type tableRow ) const;

   /*!
    * Allocates the required space to store a variable-length parameter value
    * of the specified length or size. Returns true if the necessary space was
    * successfully allocated.
    *
    * \param sizeOrLength  Requested size in bytes or length in elements of the
    *                parameter value being allocated.
    *
    * \param p       The address of a metaparameter that identifies the
    *                variable-length parameter being allocated.
    *
    * \param tableRow   The index of a table row where the requested parameter
    *                is defined. For parameters not belonging to tables, this
    *                parameter must be ignored.
    *
    * For table parameters, \a sizeOrLength is a row count. For a string
    * parameter, \a sizeOrLength is a string length in characters. For a block
    * parameter, \a sizeOrLength is a block size in bytes.
    *
    * \note Reimplementing this function is \e mandatory for processes defining
    * variable-length parameters.
    */
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );

   /*!
    * Returns the length or size of an actual variable-length parameter in this
    * process instance.
    *
    * \param p       The address of a metaparameter that identifies the
    *                variable-length parameter.
    *
    * \param tableRow   The index of a table row where the requested parameter
    *                is defined. For parameters not belonging to tables, this
    *                parameter must be ignored.
    *
    * For tables, you must reimplement this function to return the current
    * number of rows. For strings, you must return the string length in
    * characters (\e not bytes). For blocks, you must return the size in bytes.
    *
    * \note Reimplementing this function is \e mandatory for processes defining
    * variable-length parameters.
    */
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   /*!
    * Launches this process instance.
    *
    * This function will lead to one of the following actions, in decreasing
    * order of priority:
    *
    * \li The SelectInterface() member function will be called for this
    * instance. If the returned interface is valid, it will be activated and
    * a copy of this instance will be imported.
    *
    * \li If the MetaProcess::PrefersGlobalExecution() member function of the
    * metaprocess associated to this instance returns true, or if no current
    * view is available, a new thread will be created to execute this instance
    * in the global context.
    *
    * \li If the MetaProcess::PrefersGlobalExecution() member function of the
    * metaprocess associated to this instance returns false, and there is a
    * current view available, a new thread will be created to execute this
    * instance on the current view.
    *
    * \li A run-time exception will be thrown indicating that this instance
    * cannot be executed.
    */
   void Launch() const;

   /*!
    * Launches a process interface for this process instance.
    *
    * The SelectInterface() member function will be called for this instance.
    * The returned interface will be activated, and a copy of this instance
    * will be imported.
    */
   void LaunchInterface() const;

   /*!
    * Execute this instance in the global context.
    */
   void LaunchGlobal() const;

   /*!
    * Executes this instance on the current view.
    */
   void LaunchOnCurrentView() const;

   /*!
    * Executes this instance in the current window or, more precisely, on its
    * main view.
    */
   void LaunchOnCurrentWindow() const;

   /*!
    * Executes this instance on the specified view.
    */
   void LaunchOn( View& ) const;

   /*!
    * Executes this instance on the main view of the specified image window.
    */
   void LaunchOn( ImageWindow& ) const;

protected:

   const MetaProcess* meta = nullptr;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ProcessImplementation_h

// ----------------------------------------------------------------------------
// EOF pcl/ProcessImplementation.h - Released 2019-01-21T12:06:07Z
