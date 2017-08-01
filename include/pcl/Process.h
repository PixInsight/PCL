//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/Process.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_Process_h
#define __PCL_Process_h

/// \file pcl/Process.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/ProcessBase.h>
#include <pcl/ProcessParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ProcessPrivate;

/*!
 * \class Process
 * \brief High-level interface to an installed process.
 *
 * %Process instances are high-level, managed objects that represent
 * installed processes in the PixInsight platform. A module creates an
 * instance of %Process to gain access to an installed process through
 * <em>intermodule communication</em>.
 *
 * <b>%Process and %MetaProcess</b>
 *
 * %Process provides a description of the functionality and properties of an
 * <em>already installed</em> process. Contrarily, by subclassing the
 * MetaProcess class a module can define and implement a new process that can
 * be installed in the PixInsight platform. %MetaProcess is a formal
 * description of a process, while %Process describes an existing (and
 * installed) process.
 *
 * As a %MetaProcess subclass describes how a process can be instantiated,
 * %Process allows a module to create new instances of a process that can be
 * edited and executed. %Process does not provide any process execution
 * functionality; executable processes are represented by the ProcessInstance
 * class.
 *
 * \sa ProcessBase, MetaProcess, ProcessInstance
 */
class PCL_CLASS Process : public ProcessBase
{
public:

   /*!
    * A list of process parameters.
    *
    * This type is returned by the Parameters() member function to describe and
    * identify the set of parameters of a process.
    */
   typedef Array<ProcessParameter>  parameter_list;

   /*!
    * Constructs a %Process object.
    *
    * \param classId    Identifier of an installed process in the PixInsight
    *                   core application.
    *
    * If successful, this constructor creates a managed alias to an installed
    * process in the PixInsight core application.
    *
    * If there is no installed process with the specified identifier, if
    * \a classId is not a valid identifier, or if the alias object cannot be
    * generated for some reason, this constructor throws an Error exception
    * with the corresponding error message. Your code should guarantee that
    * these exceptions are always caught and handled appropriately.
    */
   Process( const IsoString& classId );

   Process( const IsoString::ustring_base& classId ) : Process( IsoString( classId ) )
   {
   }

   /*!
    * Copy constructor. Constructs an \e alias %Process object that refers to
    * the same process as the specified object \a proc.
    */
   Process( const Process& proc );

   /*!
    * Destroys this %Process object.
    *
    * \note This destructor does not destroy or uninstall the actual process it
    * refers to, which is part of the PixInsight core application. Only the
    * managed alias object living in the caller module is destroyed.
    */
   virtual ~Process();

   /*!
    */
   virtual IsoString Id() const;

   /*!
    * Returns a list of alias identifiers for this process.
    *
    * A process can have one or more <em>alias identifiers</em>. Aliases are
    * useful to maintain compatibility with previous versions of a process.
    *
    * When the PixInsight core application imports a process instance (e.g.,
    * from a process icon) it automatically replaces alias process identifiers
    * with actual (current) identifiers. This allows a developer to change the
    * identifier of a process without breaking compatibility with process
    * instances in existing icon collections or projects.
    *
    * The returned object is a list of distinct identifiers that are treated as
    * synonyms to the actual identifier of this process, which is returned by
    * the Id() member function. Two installed processes can't have the same
    * alias, so each identifier included in the returned list must be unique
    * among all installed processes on the entire PixInsight platform.
    */
   IsoStringList Aliases() const;

   /*!
    * Returns the identifiers of one or more categories to which this process
    * belongs.
    *
    * Processes are organized hierarchically by categories in PixInsight. All
    * installed processes appear classified by their categories on the %Process
    * Explorer window, and also under the %Process main menu item.
    *
    * This function returns a list of category names. Each category name must
    * be a valid C identifier. If the returned list is empty, then this process
    * is not associated with any particular category. In such case this process
    * appears under a special, fallback pseudo-category identified as
    * &lt;Etc&gt; on both the %Process Explorer window and the %Process main
    * menu item.
    */
   IsoStringList Categories() const;

   /*!
    */
   virtual uint32 Version() const;

   /*!
    */
   virtual String Description() const;

   /*!
    */
   virtual String ScriptComment() const;

   /*!
    */
   virtual Bitmap Icon() const;

   /*!
    */
   virtual Bitmap SmallIcon() const;

   /*!
    */
   virtual bool CanProcessViews() const;

   /*!
    */
   virtual bool CanProcessGlobal() const;

   /*!
    */
   virtual bool CanProcessImages() const;

   /*!
    */
   virtual bool CanProcessCommandLines() const;

   /*!
    */
   virtual bool CanEditPreferences() const;

   /*!
    */
   virtual bool CanBrowseDocumentation() const;

   /*!
    */
   virtual bool IsAssignable() const;

   /*!
    */
   virtual bool NeedsInitialization() const;

   /*!
    */
   virtual bool NeedsValidation() const;

   /*!
    */
   virtual bool PrefersGlobalExecution() const;

   /*!
   */
   virtual bool EditPreferences() const;

   /*!
    */
   virtual bool BrowseDocumentation() const;

   /*!
    * Invokes the command line execution routine for this process.
    *
    * \param arguments  A string with the list of arguments that will be passed
    *                   to the command line execution routine. Multiple
    *                   arguments must be separated with white space characters.
    *
    * If this process supports command line execution, then the corresponding
    * command line execution routine is invoked with the specified arguments.
    * Otherwise, this member function calls Launch(), and the specified command
    * line arguments are ignored.
    *
    * On the PixInsight platform, command line option arguments are customarily
    * specified with leading dash characters, following the common UNIX style.
    * Argument values are separated with equal signs. However, command line
    * syntax and the meaning of each argument are specific to each process.
    *
    * This function returns the exit code of the command line execution routine
    * of the process. By convention, exit code zero means successful execution,
    * and nonzero exit codes are returned to signal errors.
    */
   int RunCommandLine( const String& arguments ) const;

   /*!
    * Launches this process.
    *
    * If this process has an associated default interface, then its top level
    * window is shown and activated in its current state.
    *
    * If this process has no default interface, then the core application
    * attempts to generate a default instance of the process and execute it. If
    * the process prefers global execution (see the PrefersGlobalExecution()
    * member function), then the default instance is executed in the global
    * context; otherwise it is executed on the current view, if there is at
    * least one image available.
    *
    * Returns true if the process was successfully launched; false in the event
    * of error, or if the core application was unable to launch or execute it.
    */
   bool Launch() const;

   /*!
    * Returns the list of parameters of this process. The list can be empty for
    * processes that don't have parameters.
    */
   parameter_list Parameters() const;

   /*!
    * Returns true iff this process has a parameter with the specified
    * identifier \a paramId.
    */
   bool HasParameter( const IsoString& paramId ) const;

   /*!
    * Returns true iff this process has a table parameter \a tableId with the
    * specified column parameter \a colId.
    */
   bool HasTableColumn( const IsoString& tableId, const IsoString& colId ) const;

   /*!
    * Returns a list of all the currently defined process categories in the
    * PixInsight core application.
    */
   static IsoStringList AllProcessCategories();

   /*!
    * Returns a list of all the currently installed processes on the PixInsight
    * core application.
    */
   static Array<Process> AllProcesses();

   /*!
    * Returns a list of all the currently installed processes pertaining to the
    * specified \a category. Pass an empty string to obtain a list of all
    * uncategorized processes (i.e. the \<Etc\> fallback category).
    *
    * Returns an empty list if no currently installed process pertains to the
    * specified category.
    */
   static Array<Process> ProcessesByCategory( const IsoString& category );

   static Array<Process> ProcessesByCategory( const IsoString::ustring_base& category )
   {
      return ProcessesByCategory( IsoString( category ) );
   }

private:

   AutoPointer<ProcessPrivate> m_data;

   Process( const void* );

   const void* Handle() const;

   friend class ProcessInstance;
   friend class ProcessParameter;
   friend class ProcessParameterPrivate;
   friend class InternalProcessEnumerator;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_Process_h

// ----------------------------------------------------------------------------
// EOF pcl/Process.h - Released 2017-08-01T14:23:31Z
