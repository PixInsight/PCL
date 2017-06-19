//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/ProcessBase.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_ProcessBase_h
#define __PCL_ProcessBase_h

/// \file pcl/ProcessBase.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Bitmap.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ProcessBase
 * \brief Abstract base class for process descriptions.
 *
 * %ProcessBase defines a number of descriptive properties of a process in the
 * PixInsight platform.
 *
 * %ProcessBase is an abstract base class of the MetaProcess and Process
 * instantiable classes. %MetaProcess describes an installable process, while
 * the %Process class provides direct access to an installed process through
 * <em>intermodule communication</em>.
 *
 * In more practical terms, modules defining new processes must implement
 * derived classes of %MetaProcess, while %Process is used by modules requiring
 * direct access to process instances through installed processes.
 *
 * Note that the %ProcessBase, %MetaProcess, %Process, ProcessImplementation
 * and ProcessInstance classes are conceptually parallel to the FileFormatBase,
 * MetaFileFormat, FileFormat, FileFormatImplementation and FileFormatInstance
 * classes, respectively. This is just a reflection of the strong object
 * orientation and modularity that characterize the PixInsight platform.
 *
 * \sa MetaProcess, Process, ProcessImplementation, ProcessInstance
 */
class PCL_CLASS ProcessBase
{
public:

   /*!
    * Constructs a %ProcessBase object.
    */
   ProcessBase()
   {
   }

   /*!
    * Destroys this %ProcessBase object.
    */
   virtual ~ProcessBase() noexcept( false )
   {
   }

   /*!
    * Returns an identifier for this process.
    *
    * %Process identifiers are unique C identifiers.
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns a version number for this process, encoded as a hexadecimal
    * number.
    *
    * For example, version 1.0.5 should be returned as 0x105, and version
    * 3.11.5 as 0x3B5.
    */
   virtual uint32 Version() const = 0;

   /*!
    * Returns a descriptive text for this process.
    *
    * This function should provide a brief but sufficiently informative
    * description of this process. The returned description appears on the
    * %Process Explorer window, and should provide information about what this
    * process does and how it can be used, avoiding too exhaustive descriptions
    * that are better reserved for a technical manual.
    *
    * %Process descriptions are always printed on PixInsight consoles. This
    * means that the text output functionality of the Console class can be used
    * to format the string returned by this function. Refer to that class and
    * its documentation for further information.
    */
   virtual String Description() const = 0;

   /*!
    * Returns a brief description for generated scripts.
    *
    * The returned text is intended to provide a brief comment to describe this
    * process and its parameters, for quick reference on generated scripts.
    * When writing these comments, think as you do when you're commenting your
    * own source code.
    */
   virtual String ScriptComment() const = 0;

   /*!
    * Returns a <em>large icon</em> image that identifies this process.
    *
    * The returned image is used to identify all instances of this process in
    * the core application's GUI. It is used on the Process Explorer window, on
    * all icons transporting instances of this process, and in general for
    * every graphical item related to this process or to an instance of this
    * process.
    */
   virtual Bitmap Icon() const = 0;

   /*!
    * Returns a <em>small icon</em> image that identifies this process.
    *
    * For details on process icon images, see the documentation for Icon().
    *
    * Small icons are used on interface elements where screen space must be
    * preserved. Two good examples are the Process Explorer window and the
    * ProcessContainer interface.
    */
   virtual Bitmap SmallIcon() const = 0;

   /*!
    * Returns true iff this process is able to process views.
    */
   virtual bool CanProcessViews() const = 0;

   /*!
    * Returns true iff this process can be executed globally.
    */
   virtual bool CanProcessGlobal() const = 0;

   /*!
    * Returns true iff this process is able to process standalone images.
    */
   virtual bool CanProcessImages() const = 0;

   /*!
    * Returns true iff this process is able to process specific command-line
    * invocations.
    */
   virtual bool CanProcessCommandLines() const = 0;

   /*!
    * Returns true iff this process is able to edit specific preferences.
    */
   virtual bool CanEditPreferences() const = 0;

   /*!
    * Returns true iff this process is able to open a documentation browser with
    * specific documentation contents.
    *
    * Starting from version 1.7, the PixInsight Core application implements an
    * integrated documentation system. The core application provides automatic
    * documentation browsing and searching functionality, so normally this
    * member function returns true for all processes.
    */
   virtual bool CanBrowseDocumentation() const = 0;

   /*!
    * Returns true iff the instances of this process are \e assignable.
    *
    * Assignable processes allow copying the parameters of one instance (and
    * possibly more module-dependent data) to another.
    */
   virtual bool IsAssignable() const = 0;

   /*!
    * Returns true iff the instances of this process require <em>special
    * initialization</em>.
    *
    * Special initialization takes place just after instance creation.
    * Sometimes, there are actions that cannot be implemented in the
    * constructor of a process instance class. For example, this often happens
    * when an instance must be initialized by calling reimplemented virtual
    * functions from a base class constructor. Reimplemented virtual functions
    * cannot be resolved from a base class constructor in C++.
    */
   virtual bool NeedsInitialization() const = 0;

   /*!
    * Returns true iff the instances of this process require <em>special
    * validation</em>.
    *
    * Some processes may require validation of their instances just before they
    * are executed. This is known as <em>special validation</em>. The core
    * application will never execute an instance of a process if its validation
    * function fails.
    */
   virtual bool NeedsValidation() const = 0;

   /*!
    * Returns true iff the instances of this process prefer execution in the
    * global context, instead of being executed on views.
    *
    * Note that the option indicated by this function is just a \e hint to the
    * PixInsight core application, not necessarily observed. This means that
    * the fact that this member function returns true for a process does not
    * mean that the process cannot be executed on views.
    */
   virtual bool PrefersGlobalExecution() const = 0;

   /*!
    * Handles a request to edit process preferences. Returns true if the
    * preferences were successfully edited.
    *
    * By calling this member function for a process able to edit preferences
    * (that is, when CanEditPreferences() returns true), the user is allowed to
    * define a set of preferences specific to this process. Usually this task
    * is implemented on a modal dialog box. In such case, if the user closes
    * the dialog box without accepting the new preferences settings (e.g. by
    * clicking the dialog's Cancel button) this member function should return
    * false. This member function should only return true if the user has
    * edited and accepted a new set of preferences for this process.
    */
   virtual bool EditPreferences() const = 0;

   /*!
    * Handles a request to browse documentation specific for this process.
    * Returns true iff the documentation was loaded successfully.
    *
    * Since version 1.7 of the PixInsight Core application, the integrated
    * documentation system works in a completely automatic and standardized way
    * to provide documentation browsing and searching functionality.
    *
    * The default implementation of this member function automatically launches
    * and loads the integrated documentation browser with the documentation for
    * this process, if it exists and has been installed. For more information,
    * please read the documentation for the CanBrowseDocumentation() function.
    */
   virtual bool BrowseDocumentation() const = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ProcessBase_h

// ----------------------------------------------------------------------------
// EOF pcl/ProcessBase.h - Released 2017-06-17T10:55:43Z
