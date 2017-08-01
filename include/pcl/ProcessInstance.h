//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/ProcessInstance.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_ProcessInstance_h
#define __PCL_ProcessInstance_h

/// \file pcl/ProcessInstance.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Process.h>
#include <pcl/ProcessParameter.h>
#include <pcl/Variant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS Process;
class PCL_CLASS ImageWindow;
class PCL_CLASS View;

/*!
 * \class ProcessInstance
 * \brief High-level interface to a process instance.
 *
 * %ProcessInstance, along with %Process, provides access to any installed
 * process through intermodule communication.
 *
 * %ProcessInstance represents a live instance of an installed process that can
 * be edited (by setting parameter values) and executed.
 *
 * \sa Process
 */
class PCL_CLASS ProcessInstance : public UIObject
{
public:

   /*!
    * Constructs this object as a reference to a new instance of the specified
    * \a process. The newly created instance has a set of default parameter
    * values.
    */
   ProcessInstance( const Process& process );

   /*!
    * Copy constructor. The newly constructed object references the same
    * server-side process instance as the specified object \a p.
    *
    * \note Note that this constructor does not generate a new process
    * instance. It just constructs a high-level reference to an existing
    * process instance.
    */
   ProcessInstance( const ProcessInstance& p ) : UIObject( p )
   {
   }

   /*!
    * Destroys a process instance.
    *
    * After destruction of this object, the server-side process instance is
    * also destroyed (garbage-collected) if it is no longer referenced by other
    * %ProcessInstance objects.
    */
   virtual ~ProcessInstance()
   {
   }

   /*!
    * Returns a reference to a <em>null process instance</em>. A null
    * %ProcessInstance object does not correspond to an existing process
    * instance in the PixInsight core application.
    */
   static ProcessInstance& Null();

   /*!
    * Returns a Process object that represents the installed process that
    * this instance belongs to.
    */
   Process ParentProcess() const;

   /*!
    * Assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side process instance as the
    * specified object \a p. If the previous instance becomes unreferenced, it
    * will be garbage-collected by the PixInsight core application.
    */
   ProcessInstance& operator =( const ProcessInstance& p )
   {
      SetHandle( p.handle );
      return *this;
   }

   /*!
    * Generates a uniquely referenced, newly created process instance as a
    * duplicate of the instance referenced by this object, and returns a
    * high-level reference to it as a new %ProcessInstance object.
    */
   ProcessInstance Clone() const;

   /*!
    * Assigns the parameters of a process instance referenced by the specified
    * object \a p to the instance referenced by this object. Returns true if
    * the assignment operation was successful; false in the event of error.
    *
    * Note that this member function does \e not do the same as an assignment
    * operator or a copy constructor, because the latter two operations simply
    * cause this object to reference an existing server-side instance (and
    * possibly leave a previous instance unreferenced). This member function
    * does not generate a new process instance and does not change existing
    * references to server-side objects; it just copies process parameters (and
    * probably other module-dependent data) from one existing instance to
    * another.
    *
    * \note For this member function to success, the parent Process has to be
    * an <em>assignable process</em>. Assignable processes return true from
    * reimplemented Process::IsAssignable() virtual functions.
    */
   bool Assign( const ProcessInstance& p );

   /*!
    * Requests the parent process to validate this process instance. Returns
    * true upon successful validation; false if the instance is not valid, or
    * in the event of error.
    *
    * If this member function returns false, the process should return a
    * description of the reasons to not validate the instance in the string
    * \a whyNot. If the function returns true, the string \a whyNot will be
    * cleared (so it will be empty and its previous contents will be lost).
    */
   bool Validate( String& whyNot );

   /*!
    * Returns true iff this process instance will modify the process history of
    * the specified \a view, in case it is executed on it.
    *
    * If this member function returns false, then this instance can be safely
    * executed on the \a view as a read-only process; the view won't be
    * modified in any way.
    */
   bool IsHistoryUpdater( const View& view ) const;

   /*!
    * Returns true iff this process instance can be masked with the specified
    * image \a window when executed on the specified \a view.
    *
    * When a process is masked, its result is mixed with the image in the state
    * it was before execution, according to the values of a mask image. This
    * process is carried out automatically by the PixInsight core application.
    */
   bool IsMaskable( const View& view, const ImageWindow& mask ) const;

   /*!
    * Validates execution of this process instance on the specified \a view.
    * Returns true iff this instance can be executed on the view.
    *
    * If this member function returns false, the process should return in the
    * string \a whyNot a description of the reasons why this instance cannot be
    * executed on the specified \a view. If the function returns true, the
    * string \a whyNot will be cleared (so it will be empty and its previous
    * contents will be lost).
    */
   bool CanExecuteOn( const View& view, String& whyNot ) const;

   /*!
    * Executes this process instance on the specified \a view. Returns true
    * upon successful execution; false in the event of error.
    *
    * The optional \a swapFile argument allows the caller to control generation
    * of swap data as part of the instance execution. If \a swapFile is true
    * (the default value), the PixInsight core application will save the
    * previous state of the target \a view before instance execution. In this
    * case the executed instance will be appended to the view's process history
    * in the usual way, and the process will be undoable.
    *
    * If \a swapFile is false, the previous state of the view is not saved, the
    * instance is not added to the view's history, and the process cannot be
    * undone after execution. In this case the caller module is responsible for
    * leaving the image transported by \a view in a valid and predictable
    * state. Keep in mind that if no swap data are generated before excution,
    * the user has no way to restore the previous state of the image, and also
    * has no way to control the sequence of processes that have given rise to
    * the current state of the image. If these side effects are not correctly
    * controlled and handled by the calling module, they will violate basic
    * design principles of the PixInsight platform, and hence they can be a
    * good reason for denying certification of a module.
    *
    * This member function is typically called with the \a swapFile argument
    * set to false by processes that execute other processes to implement their
    * own tasks. For example, the standard %ProcessContainer process (which is
    * internal to the core application) does exactly this to execute a sequence
    * of processes as a unique step in a view's process history. Another case
    * where \a swapFile should be set to false is when a process generates a
    * new image as a result of running a sequence of processes, but it wants to
    * appear as the unique process in the <em>initial state</em> element of the
    * view's process history.
    */
   bool ExecuteOn( View& view, bool swapFile = true );

   /*!
    * Returns true iff this process instance can be executed in the global
    * context.
    *
    * If this member function returns false, the process should return in the
    * string \a whyNot a description of the reasons why this instance cannot be
    * executed on the global context. If the function returns true, the string
    * \a whyNot will be cleared (so it will be empty and its previous contents
    * will be lost).
    *
    * A process is executed in the global context when it is not explicitly
    * executed on a particular view. Two well-known examples of global
    * processes are Preferences and ReadoutOptions. Note that a process can
    * work both globally and on views. For example, the PixelMath process can
    * work globally to generate new images, or locally when it is executed on a
    * target view.
    */
   bool CanExecuteGlobal( String& whyNot ) const;

   /*!
    * Executes this process instance on the global context. Returns true upon
    * successful execution; false in the event of error.
    *
    * See the documentation for CanExecuteGlobal() for additional comments on
    * process execution contexts.
    */
   bool ExecuteGlobal();

   /*!
    * Validates execution of this process instance on the specified \a image.
    * Returns true iff this instance can be executed on the image.
    *
    * If this member function returns false, the process should return in the
    * string \a whyNot a description of the reasons why this instance cannot be
    * executed on the specified \a image. If the function returns true, the
    * string \a whyNot will be cleared (so it will be empty and its previous
    * contents will be lost).
    */
   bool CanExecuteOn( const ImageVariant& image, String& whyNot ) const;

   /*!
    * Executes this process instance on the specified \a image. Returns true
    * upon successful execution; false in the event of error.
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
    *          function for more information. The default value is an empty
    *          string.
    */
   bool ExecuteOn( ImageVariant& image, const IsoString& hints = IsoString() );

   bool ExecuteOn( ImageVariant& image, const IsoString::ustring_base& hints )
   {
      return ExecuteOn( image, IsoString( hints ) );
   }

   /*!
    * Launches this process instance.
    *
    * If this process has an associated default interface, then its top level
    * window is shown and activated, and this instance is \e imported by the
    * interface. After importing this instance, the interface will show its
    * parameter values.
    *
    * If this process has no default interface, then the core application
    * attempts to execute this instance. If the process prefers global
    * execution (see the Process::PrefersGlobalExecution() member function),
    * then this instance will be executed in the global context; otherwise it
    * will be executed on the current view, if there is at least one image
    * available.
    *
    * Returns true if this instance was successfully launched; false in the
    * event of error, or if the core application was unable to launch or
    * execute the instance.
    */
   bool Launch();

   /*!
    * Returns true iff this process instance is able to open and activate a
    * process interface.
    *
    * Normally, each process has an associated interface that can always be
    * launched through any existing (and valid) instance of the appropriate
    * process class. However, there are some processes for which an interface
    * does not make sense. A well-known example is the Invert process.
    */
   bool CanLaunchInterface() const;

   /*!
    * Opens and activates the interface associated to this process, and causes
    * it to import this instance. After importing this instance, the interface
    * will show its parameter values. Returns true if the interface could be
    * successfully launched; false in the event of error.
    */
   bool LaunchInterface();

   /*!
    * Returns the descriptive text currently associated with this process
    * instance, or an empty string if this instance has no description.
    *
    * In PixInsight, each process instance has an associated text that the user
    * can edit to describe the purpose and conditions of the applied processes.
    * This feature is an extremely useful and efficient way to document a
    * processing procedure.
    */
   String Description() const;

   /*!
    * Sets the descriptive text for this process instance.
    *
    * Instance descriptions can make use of the whole formating capabilities of
    * PixInsight consoles; see the Console class for more information.
    */
   void SetDescription( const String& text );

   /*!
    * Gets the running times for the latest execution of this process instance.
    *
    * \param[out] startJD      Starting execution time as a Julian day number.
    *
    * \param[out] elapsedSecs  Total elapsed execution time in seconds.
    *
    * If this instance has never been executed, or if the execution times have
    * not been recorded for some reason, both output values are set to zero.
    */
   void GetExecutionTimes( double& startJD, double& elapsedSecs ) const;

   /*!
    * Returns the time when this process instance was last executed as a Julian
    * day number. If this instance has never been executed, or if the execution
    * time has not been recorded for some reason, this function returns zero.
    */
   double StartJD() const
   {
      double startJD, dum;
      GetExecutionTimes( startJD, dum );
      return startJD;
   }

   /*!
    * Returns the total elapsed time in seconds for the latest execution of
    * this process instance, or zero if either this instance has never been
    * executed, or if the execution time has not been recorded for some reason.
    */
   double ElapsedTime() const
   {
      double dum, elapsedSecs;
      GetExecutionTimes( dum, elapsedSecs );
      return elapsedSecs;
   }

   /*!
    * Returns a source code representation of this process instance.
    *
    * \param language   The language to use for the source code representation.
    *                   Currently the only languages supported are "JavaScript"
    *                   and "XPSM 1.0". The default language is JavaScript.
    *
    * \param varId      The identifier of a variable that will be used to
    *                   represent this instance. If an empty string is
    *                   specified (which is the default value), the PixInsight
    *                   core application wll assign a default identifier. This
    *                   identifier only applies to JavaScript source code
    *                   representations, not to XML representations.
    *
    * \param indent     Indentation distance in characters. This is a constant
    *                   number of white space characters (0x20) that will be
    *                   prepended to each source code line. The default value
    *                   is zero.
    *
    * The source code generated by this function can be used directly for
    * serialization of process instances in the specified language.
    */
   String ToSource( const IsoString& language = "JavaScript",
                    const IsoString& varId = IsoString(), int indent = 0 ) const;

   /*!
    * Returns a process instance generated from the specified \a source code.
    *
    * \note This member function is still not implemented; it will be
    * implemented in a future version of PCL. For now, this function always
    * returns a null process instance.
    */
   static ProcessInstance FromSource( const String& source,
                                      const IsoString& language = "JavaScript" );

   /*!
    * Returns a process instance generated as a duplicate of the instance
    * transported by a process icon with the specified identifier \a iconId.
    *
    * Returns a null instance if the specified icon does not exist, or if an
    * icon exists with the specified identifier but it is not a process icon
    * (e.g., an image icon).
    */
   static ProcessInstance FromIcon( const IsoString& iconId );

   static ProcessInstance FromIcon( const IsoString::ustring_base& iconId )
   {
      return FromIcon( IsoString( iconId ) );
   }

   /*!
    * Returns a string list with the identifiers of all existing process icons,
    * or an empty list if there are no process icons.
    */
   static IsoStringList Icons();

   /*!
    * Returns a string list with the identifiers of all existing process icons
    * transporting instances of the specified process, or an empty list if no
    * such icons exist.
    */
   static IsoStringList IconsByProcessId( const IsoString& processId );

   static IsoStringList IconsByProcessId( const IsoString::ustring_base& processId )
   {
      return IconsByProcessId( IsoString( processId ) );
   }

   /*!
    * Returns the value of a process parameter.
    *
    * \param parameter  Reference to an existing parameter in the parent
    *                   process of this instance (see Process::Parameters()).
    *
    * \param rowIndex   Zero-based index of the table row, when the specified
    *                   \a parameter is a table column parameter. Must be
    *                   within the range of existing rows for the corresponding
    *                   table parameter in this instance, that is, from zero to
    *                   the number of table rows minus one. If \a parameter is
    *                   not a table column parameter this argument is ignored.
    *
    * If the specified \a parameter is a table process parameter, this member
    * function throws an Error exception. A table process parameter does not
    * have a value, and its contents cannot be retrieved directly as a whole.
    *
    * Note that the default value of the \a rowIndex argument is an impossible
    * row index number: ~size_type(0). This value has been selected on purpose
    * in order to force the caller to specify a row index when \a parameter is
    * a table column parameter. This is a simple but effective security
    * mechanism to robustify parameter access code: if no row index is
    * specified for a table column parameter, this member function will always
    * return an invalid %Variant object (see below).
    *
    * This member function returns a Variant object with the current value of
    * the specified parameter or table cell parameter in this instance. An
    * invalid %Variant object is returned in the following cases:
    *
    * \li If the specified \a parameter is not a valid parameter of the parent
    * process.
    *
    * \li If \a parameter is a table column parameter and the specified
    * \a rowIndex is larger than or equal to the number of existing rows for
    * the parent table parameter in this instance.
    *
    * The calling process should always check Variant::IsValid() for the
    * objects returned by this function.
    */
   Variant ParameterValue( const ProcessParameter& parameter, size_type rowIndex = ~size_type( 0 ) ) const;

   /*!
    * Returns the value of a process parameter.
    *
    * This is an overloaded member function, provided for convenience. It
    * allows retrieving parameter values using parameter identifiers instead of
    * references to ProcessParameter objects. Otherwise this member function
    * behaves exactly like
    * ParameterValue( const ProcessParameter&, size_type ) const
    */
   Variant ParameterValue( const IsoString& parameterId, size_type rowIndex = ~size_type( 0 ) ) const
   {
      return ParameterValue( ProcessParameter( ParentProcess(), parameterId ), rowIndex );
   }

   Variant ParameterValue( const IsoString::ustring_base& parameterId, size_type rowIndex = ~size_type( 0 ) ) const
   {
      return ParameterValue( IsoString( parameterId ), rowIndex );
   }

   /*!
    * Sets the value of a process parameter.
    *
    * \param value      New parameter value. The specified Variant object must
    *                   transport an object of either the same type as the
    *                   \a parameter whose value will be changed, or of a type
    *                   that can legally be converted to the corresponding
    *                   parameter type. The value must also observe the limits
    *                   imposed by the parent process regarding acceptable
    *                   values, numeric ranges, etc.
    *
    * \param parameter  Reference to an existing parameter in the parent
    *                   process of this instance (see Process::Parameters()).
    *
    * \param rowIndex   Zero-based index of the table row, when the specified
    *                   \a parameter is a table column parameter. Must be
    *                   within the range of existing rows for the corresponding
    *                   table parameter in this instance, that is, from zero to
    *                   the number of table rows minus one. If \a parameter is
    *                   not a table column parameter this argument is ignored.
    *
    * If the specified \a parameter is a table process parameter, this member
    * function throws an Error exception. A table process parameter does not
    * have a value, and its contents cannot be set directly as a whole.
    *
    * Note that the default value of the \a rowIndex argument is an impossible
    * row index number: ~size_type(0). This value has been selected on purpose
    * in order to force the caller to specify a row index when \a parameter is
    * a table column parameter. This is a simple but effective security
    * mechanism to robustify parameter access code: if no row index is
    * specified for a table column parameter, this member function will always
    * return false.
    *
    * The \a value must observe the limits imposed by the parent process
    * regarding valid numeric ranges, length limits, acceptable characters,
    * etc., depending on parameter types. This member function returns true iff
    * the parameter value could be set correctly. If the type of \a value
    * cannot be converted to the required parameter type, an Error exception is
    * thrown with the appropriate error message.
    *
    * \note To set the values of table column parameters, the necessary table
    * rows must be allocated \e before calling this member function. To
    * allocate table rows, use the AllocateTableRows() member function.
    */
   bool SetParameterValue( const Variant& value,
                           const ProcessParameter& parameter, size_type rowIndex = ~size_type( 0 ) );

   /*!
    * Sets the value of a process parameter.
    *
    * This is an overloaded member function, provided for convenience. It
    * allows changing parameter values using parameter identifiers instead of
    * references to ProcessParameter objects. Otherwise this member function
    * behaves exactly like
    * SetParameterValue( const Variant&, const ProcessParameter&, size_type )
    */
   bool SetParameterValue( const Variant& value, const IsoString& parameterId, size_type rowIndex = ~size_type( 0 ) )
   {
      return SetParameterValue( value, ProcessParameter( ParentProcess(), parameterId ), rowIndex );
   }

   bool SetParameterValue( const Variant& value, const IsoString::ustring_base& parameterId, size_type rowIndex = ~size_type( 0 ) )
   {
      return SetParameterValue( value, IsoString( parameterId ), rowIndex );
   }

   /*!
    * Returns the number of existing rows in a table process parameter.
    *
    * \param table      Reference to a table parameter in the parent process.
    *
    * If the specified parameter is not a table process parameter of the parent
    * process, this member function returns zero.
    */
   size_type TableRowCount( const ProcessParameter& table ) const;

   /*!
    * Returns the number of existing rows in a table process parameter.
    *
    * This is an overloaded member function, provided for convenience. It
    * returns the number of existing table rows using parameter identifiers
    * instead of references to ProcessParameter objects. Otherwise this member
    * function behaves exactly like
    * TableRowCount( const ProcessParameter& ) const
    */
   size_type TableRowCount( const IsoString& tableId ) const
   {
      return TableRowCount( ProcessParameter( ParentProcess(), tableId ) );
   }

   size_type TableRowCount( const IsoString::ustring_base& tableId ) const
   {
      return TableRowCount( IsoString( tableId ) );
   }

   /*!
    * Allocates table rows for a table process parameter.
    *
    * \param table      Reference to a table parameter in the parent process.
    *
    * \param rowCount   Number of table rows to allocate. If the parameter has
    *                   table row limitations, this value must be within the
    *                   acceptable range.
    *
    * Calling this member function deallocates all previously existing table
    * rows in this instance, if there were any. Table rows cannot be appended
    * neither deleted selectively; the whole table parameter is reallocated as
    * a whole each time this member function is called. Calling this function
    * with \a rowCount set to zero (if zero rows are acceptable for the
    * specified \a table parameter) effectively deallocates the whole table
    * parameter in this instance.
    *
    * Returns true if the specified number of rows could be allocated
    * successfully in this process instance; false in the event of error.
    */
   bool AllocateTableRows( const ProcessParameter& table, size_type rowCount );

   /*!
    * Allocates table rows for a table process parameter.
    *
    * This is an overloaded member function, provided for convenience. It
    * allows reallocating a table parameter rows using a parameter identifier
    * instead of a reference to a ProcessParameter object. Otherwise this
    * member function behaves exactly like
    * AllocateTableRows( const ProcessParameter&, size_type )
    */
   bool AllocateTableRows( const IsoString& tableId, size_type rowCount )
   {
      return AllocateTableRows( ProcessParameter( ParentProcess(), tableId ), rowCount );
   }

   bool AllocateTableRows( const IsoString::ustring_base& tableId, size_type rowCount )
   {
      return AllocateTableRows( IsoString( tableId ), rowCount );
   }

private:

   ProcessInstance( void* h ) : UIObject( h )
   {
   }

   ProcessInstance( const void* h ) : UIObject( h )
   {
   }

   virtual void* CloneHandle() const;

   friend class ProcessInstancePrivate;
   friend class InterfaceDispatcher;
   friend class InternalIconEnumerator;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ProcessInstance_h

// ----------------------------------------------------------------------------
// EOF pcl/ProcessInstance.h - Released 2017-08-01T14:23:31Z
