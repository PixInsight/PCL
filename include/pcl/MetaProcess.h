//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/MetaProcess.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_MetaProcess_h
#define __PCL_MetaProcess_h

/// \file pcl/MetaProcess.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/MetaObject.h>
#include <pcl/ProcessBase.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class View;
class ImageWindow;
class ProcessImplementation;
class ProcessInterface;
class MetaParameter;

/*!
 * \class MetaProcess
 * \brief A formal description of a PixInsight process.
 *
 * %MetaProcess provides a formal description of the basic functionality and
 * properties of a process in a PixInsight module.
 *
 * In a PixInsight module, every process must be implemented as a derived class
 * of %MetaProcess. Such subclass will describe general properties of the
 * process, and will provide the basic mechanisms to instantiate a process and
 * generate duplicates of existing process instances.
 *
 * %MetaProcess and its subclasses don't implement any processing capabilities.
 * The actual processing work must be implemented through a <em>process
 * instance implementation class</em>, which is always a descendant of the
 * ProcessImplementation class in the PixInsight/PCL framework.
 *
 * Each process must have a unique identifier. %Process identifiers are crucial
 * for many key features of the PixInsight environment, like process
 * scripting, command-line interfaces, process icons and containers, etc.
 *
 * Processes are organized by <em>process categories</em> in PixInsight. A
 * process category groups similar or related processes together. The whole set
 * of process categories forms a <em>categorized tree</em> of processes that
 * can be accessed, for example, from the %Process Explorer window in the core
 * PixInsight GUI.
 *
 * \sa MetaModule, MetaParameter, ProcessImplementation
 */
class PCL_CLASS MetaProcess : public MetaObject, public ProcessBase
{
public:

   /*!
    * Constructs a %MetaProcess object.
    */
   MetaProcess();

   /*!
    * Destroys this %MetaProcess object. Also destroys all MetaParameter
    * children of this %MetaProcess.
    */
   virtual ~MetaProcess() noexcept( false )
   {
   }

   /*!
    * Copy constructor. Copy semantics are disabled for %MetaProcess because
    * this class represents unique server-side objects.
    */
   MetaProcess( const MetaProcess& ) = delete;

   /*!
    * Copy assignment. Copy semantics are disabled for %MetaProcess because
    * this class represents unique server-side objects.
    */
   MetaProcess& operator =( const MetaProcess& ) = delete;

   /*!
    * Move constructor. Move semantics are disabled for %MetaProcess because
    * because of parent-child server-side object relations.
    */
   MetaProcess( MetaProcess&& x ) = delete;

   /*!
    * Move assignment. Move semantics are disabled for %MetaProcess because
    * because of parent-child server-side object relations.
    */
   MetaProcess& operator =( MetaProcess&& x ) = delete;

   /*!
   */
   virtual IsoString Id() const override = 0;

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
    * The returned string is a comma-separated list of distinct identifiers
    * that will be treated as synonyms to the actual identifier of this
    * process, which is returned by the Id() member function. Two installed
    * processes can't have the same alias, so each identifier included in the
    * returned list must be unique among all installed processes on the entire
    * PixInsight platform.
    *
    * \note The default implementation of this member function returns an empty
    * string (no aliases).
    *
    * \sa Id()
    */
   virtual IsoString Aliases() const
   {
      return IsoString();
   }

   /*!
    * Returns the identifiers of one or more categories to which this process
    * belongs.
    *
    * Processes are organized hierarchically by categories in PixInsight. All
    * installed processes appear classified by their categories on the %Process
    * Explorer window, and also under the %Process main menu item.
    *
    * This member function can return a comma-separated list of category
    * names, or a single category name. Each category name must be a valid C
    * identifier. If one or more of the specified names are nonempty strings
    * that don't correspond to already existing process categories, new
    * categories will be created and this process will be added to them. If a
    * category already exists with the specified identifier, then this process
    * will be added to the existing category.
    *
    * If this function returns an empty string, then this process will not be
    * associated to any particular category. It will then appear under a
    * special, fallback pseudo-category identified as &lt;Etc&gt; on both the
    * %Process Explorer window and the %Process main menu item.
    *
    * \note The default implementation of this function returns an empty
    * string, so a process is not associated to any specific category unless a
    * derived class reimplements this function.
    */
   virtual IsoString Categories() const
   {
      return IsoString(); // General category
   }

   /*!
    * A synonym for Categories(), provided for compatibility with previous PCL
    * versions.
    *
    * \deprecated This member function has been deprecated. Use Categories() in newly
    * produced code.
    */
   virtual IsoString Category() const // ### PCL 2.x: Remove
   {
      return Categories(); // General category
   }

   /*!
    * Returns a version number for this process, encoded as a hexadecimal
    * number.
    *
    * For example, version 1.0.5 should be returned as 0x105, and version
    * 3.11.5 as 0x3B5. The default return value is 0x100, corresponding to
    * version 1.0.0.
    */
   uint32 Version() const override
   {
      return 0x100;
   }

   /*!
    * Returns a descriptive text for this process.
    *
    * This function must provide a brief but sufficiently informative
    * description of this process. The returned description will appear on the
    * %Process Explorer window, and should provide information about what this
    * process does and how it can be used, avoiding too exhaustive descriptions
    * that are better reserved for a technical manual.
    *
    * %Process descriptions are always printed on PixInsight consoles. This
    * means that the text output functionality of the Console class can be used
    * to format the string returned by this function. Refer to that class and
    * its documentation for further information.
    *
    * \note The default implementation of this function returns an empty
    * string, so by default a process does not provide a description. This is
    * legal, but it is considered bad practice, so this function \e should be
    * reimplemented.
    */
   String Description() const override
   {
      return String();
   }

   /*!
    * Returns a brief description for generated scripts.
    *
    * The returned text is intended to provide a brief comment to describe this
    * process and its parameters, for quick reference on generated scripts.
    * When writing these comments, think as you do when you're commenting your
    * own source code.
    *
    * \note The default implementation of this function returns an empty
    * string, so by default processes provide no script comments.
    */
   String ScriptComment() const override
   {
      return String();
   }

   /*!
    * Returns a <em>large icon</em> for this process as an image in the
    * standard XPM format.
    *
    * The specified image will be used to identify all instances of this
    * process in the core application's GUI. It will be used on the %Process
    * Explorer window, on process icons of this class, and in general for every
    * graphical item related to this process or to an instance of this process.
    *
    * 32-bit RGBA color images (including an alpha channel) are fully
    * supported.
    *
    * If this function returns nullptr, a default icon will be assigned to this
    * process automatically.
    *
    * \note The default implementation of this function returns nullptr.
    *
    * \sa IconImageFile()
    */
   virtual const char** IconImageXPM() const
   {
      return nullptr;
   }

   /*!
    * Returns a <em>large icon</em> for this process as a path specification to
    * an existing image file.
    *
    * Supported image file formats include PNG, XPM, JPG and BMP.
    *
    * For details on process icon images, see the documentation for
    * IconImageXPM().
    *
    * \deprecated Using this function is discouraged, since it produces an
    * unnecessary dependency on an external file, which complicates the module
    * installation procedure. The recommended method is always reimplementing
    * the IconImageXPM() member function in a derived class, to provide the
    * address of a static image in the XPM format.
    *
    * \note The default implementation of this function returns an empty
    * string.
    *
    * \sa IconImageXPM()
    */
   virtual String IconImageFile() const
   {
      return String();
   }

   /*!
    * Returns a <em>small icon</em> for this process as an image in the
    * standard XPM format.
    *
    * For details on process icon images, see the documentation for
    * IconImageXPM().
    *
    * Small icons are used on interface elements where screen space must be
    * preserved. Two good examples are the History Explorer window and the
    * ProcessContainer interface.
    *
    * When this function is not reimplemented in a derived class, the core
    * PixInsight application automatically generates a small icon by resampling
    * down the large icon provided by IconImageXPM().
    *
    * You normally should not need to reimplement this function; the core
    * application usually does a fine work resampling large icons to obtain
    * reduced versions.
    *
    * \sa SmallIconImageFile()
    */
   virtual const char** SmallIconImageXPM() const
   {
      return nullptr;
   }

   /*!
    * Returns a <em>small icon</em> for this process as a path specification to
    * an existing image file.
    *
    * Supported image file formats include PNG, XPM, JPG and BMP.
    *
    * For details on small process icon images, see the documentation for
    * SmallIconImageXPM().
    *
    * \deprecated Using this function is discouraged, for the same reasons
    * explained on the documentation entry for IconImageFile().
    *
    * \sa SmallIconImageXPM()
    */
   virtual String SmallIconImageFile() const
   {
      return String();
   }

   /*!
    */
   Bitmap Icon() const override;

   /*!
    */
   Bitmap SmallIcon() const override;

   /*!
    * %Process class initialization routine.
    *
    * This member function is called just after the module where this process
    * lives has been completely installed. When this function is called,
    * communication with the PixInsight core application is fully operative.
    *
    * This function is seldom reimplemented by derived classes. Reimplement
    * it if your process requires some initialization that cannot be
    * accomplished in the class constructor. This includes calling PCL
    * functions that require active communication with the PixInsight core
    * application, as retrieving global settings, for example (see the
    * pcl/GlobalSettings.h header).
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa MetaProcess()
    */
   virtual void InitializeClass()
   {
   }

   /*!
    * Provides a default interface for this process.
    *
    * The interface addressed by this function will be activated in situations
    * where this process is activated without a specific \e instance. In
    * such situations, the core application will try to find a suitable
    * <em>default interface</em> by calling this function, and if the address
    * of a valid object is returned, the indicated interface will be launched.
    *
    * An example of process activation without an instance is when the user
    * double-clicks an item on the %Process Explorer window.
    *
    * When an instance is available, the core application invokes
    * ProcessImplementation::SelectInterface() instead of this member function.
    * An example of process activation with an instance is when the user
    * double-clicks a process icon on the core application's workspace.
    *
    * If this function returns zero, no interface will be launched when this
    * process is activated without an instance.
    *
    * \note The default implementation of this function returns zero, so by
    * default a process has no associated interface.
    *
    * \sa ProcessImplementation::SelectInterface()
    */
   virtual ProcessInterface* DefaultInterface() const
   {
      return 0; // by default, processes have no associated interfaces.
   }

   /*!
    * Returns true iff this process class is able to process views.
    *
    * View execution of a process, also known as <em>view context</em>, is
    * mainly implemented by the following member functions:
    *
    * bool ProcessImplementation::CanExecuteOn( const View&, String& )
    * bool ProcessImplementation::ExecuteOn( View& )
    *
    * which must be reimplemented by any instance class corresponding to a
    * process with view execution capabilities.
    *
    * \note The default implementation of this function returns true, so by
    * default a process is supposed to be executable on views. This is indeed
    * the case for the vast majority of processes.
    */
   bool CanProcessViews() const override
   {
      return true;
   }

   /*!
    * Returns true iff this process class can be executed globally.
    *
    * Global execution of a process, also known as <em>global context</em>, is
    * mainly implemented by the following member functions:
    *
    * bool ProcessImplementation::CanExecuteGlobal( String& )
    * bool ProcessImplementation::ExecuteGlobal()
    *
    * which must be reimplemented by any instance class corresponding to a
    * process with global execution capabilities.
    *
    * \note For compatibility with previous PCL versions, the default
    * implementation of this function returns true, so by default a process is
    * supposed to be executable globally. There are not many global processes,
    * and only a few processes can be executed in both the global and view
    * contexts, so this default behavior is clearly suboptimal. However, as
    * noted above it is necessary to maintain compatibility with 1.x PCL
    * versions. This may change in a future version 2.x of PCL.
    */
   bool CanProcessGlobal() const override
   {
      return true;
   }

   /*!
    * Returns true iff this process class is able to process standalone images.
    *
    * This refers to the ability of process instances to be executed on images
    * passed to a reimplementation of the following member function:
    *
    * bool ProcessImplementation::ExecuteOn( ImageVariant&, const IsoString& )
    *
    * Note that this is completely different from processing views with a
    * reimplementation of:
    *
    * bool ProcessImplementation::ExecuteOn( View& )
    *
    * Although both functions usually share the majority of their working code
    * for most processes, they are unrelated from the point of view of the
    * PixInsight core application, and they also play very different roles for
    * the integration of a process with the platform.
    *
    * If a derived class reimplements this function to return true, the
    * corresponding process instance implementation class should also
    * reimplement the following ones:
    *
    * bool ProcessImplementation::CanExecuteOn( const ImageVariant&, String& ) const
    * bool ProcessImplementation::ExecuteOn( ImageVariant& )
    *
    * \note The default implementation of this function returns false, so by
    * default a process has no standalone image processing capabilities.
    */
   bool CanProcessImages() const override
   {
      return false;
   }

   /*!
    * Returns true iff this process class is able to process specific
    * command-line invocations.
    *
    * If a derived class reimplements this function to return true, it should
    * also reimplement the ProcessCommandLine() member function to perform
    * actual command line processing, or a runtime exception will be thrown.
    *
    * \note The default implementation of this function returns false, so by
    * default processes can't process command-line arguments.
    *
    * \sa ProcessCommandLine()
    */
   bool CanProcessCommandLines() const override
   {
      return false;
   }

   /*!
    * Returns true iff this process is able to edit specific preferences.
    *
    * If a derived class reimplements this function to return true, it should
    * also reimplement the EditPreferences() member function to perform the
    * actual preferences edit work, or a runtime exception will be thrown.
    *
    * \note The default implementation of this function returns false, so by
    * default processes can't edit specific preferences.
    *
    * \sa EditPreferences()
    */
   bool CanEditPreferences() const override
   {
      return false;
   }

   /*!
    * Returns true iff this process is able to open a documentation browser with
    * specific documentation contents.
    *
    * Starting from version 1.7, the PixInsight Core application implements an
    * integrated documentation system. The core application provides automatic
    * documentation browsing and searching functionality. For this reason, the
    * default implementation of this member function returns true.
    *
    * Unless a process has specific documentation requirements beyond the
    * integrated documentation system --which is strongly discouraged--, the
    * BrowseDocumentation() member function of %MetaProcess should not be
    * reimplemented in derived classes. The default implementation drives the
    * integrated documentation system automatically in a standardized way.
    *
    * \note The default implementation of this function returns true, for the
    * same reasons explained above.
    *
    * \sa BrowseDocumentation()
    */
   bool CanBrowseDocumentation() const override
   {
      return true;
   }

   /*!
    * Creates a new instance of this process. Returns a pointer to the newly
    * created process instance.
    *
    * A process instance is implemented as an instance of a subclass of the
    * ProcessImplementation class.
    *
    * \note This is a pure virtual member function that has to be implemented
    * in a derived class. Every process in PixInsight must be able to generate
    * new process instances.
    *
    * \sa Clone()
    */
   virtual ProcessImplementation* Create() const = 0;

   /*!
    * Creates a new instance of this process as a duplicate of an existing
    * process instance. Returns a pointer to the newly created process
    * instance.
    *
    * \note This is a pure virtual member function that has to be implemented
    * in a derived class. Every process in PixInsight must be able to generate
    * clones of existing process instances.
    *
    * \sa Create(), TestClone()
    */
   virtual ProcessImplementation* Clone( const ProcessImplementation& ) const = 0;

   /*!
    * Creates a new instance of this process as a duplicate of an existing
    * process instance, for strict testing purposes. Returns a pointer to the
    * newly created process instance.
    *
    * In certain situations, the core application needs a temporary process
    * instance exclusively for testing purposes. This happens, for example, to
    * verify if a given instance can be executed on a given view, or in the
    * global context, during complex GUI operations. The core application tries
    * to optimize GUI performance by calling this function when possible.
    *
    * If your process can provide a substantially simplified version of an
    * existing instance, and such a simplified version still is able to know if
    * it can be executed globally or on a given view, then reimplement this
    * function.
    *
    * For example, if your process requires (or may require) some megabytes of
    * data to work, but these data are not needed for testing purposes, then it
    * is a pretty good candidate to reimplement this function. In practice,
    * however, requiring a reimplementation of this function is infrequent.
    *
    * \note The default implementation of this function returns Clone( p ).
    *
    * \sa Clone()
    */
   virtual ProcessImplementation* TestClone( const ProcessImplementation& p ) const
   {
      return Clone( p );
   }

   /*!
    * Returns true iff the instances of this process are \e assignable.
    *
    * Assignable processes allow copying one instance to another.
    *
    * If you have a good reason to avoid instance assignment for your process,
    * then reimplement this function to return false. A good reason could be
    * that your process has no parameters, hence no data could be assigned.
    *
    * \note The default implementation of this function returns true, so
    * processes are assignable by default.
    */
   bool IsAssignable() const override
   {
      return true; // by default, processes are considered assignable.
   }

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
    *
    * If your process instance class requires some initialization that cannot
    * be implemented in a regular constructor function, then reimplement this
    * function to return true. In this case, you must also reimplement
    * ProcessImplementation::Initialize() for your instance implementation
    * class to implement the special initialization work, or a runtime
    * exception will be thrown.
    *
    * \note The default implementation of this function returns false, so by
    * default processes don't require special instance initialization.
    *
    * \note Don't confuse the instance initialization that this function refers
    * to with the InitializeClass() member function of %MetaProcess.
    *
    * \sa ProcessImplementation::Initialize()
    */
   bool NeedsInitialization() const override
   {
      return false; // by default, process instances are not initialized.
   }

   /*!
    * Returns true iff the instances of this process require <em>special
    * validation</em>.
    *
    * Some processes may require validation of their instances just before they
    * are executed. This is known as <em>special validation</em>. The core
    * application will never execute an instance of a process if its validation
    * function fails. To implement your special validation, reimplement this
    * function to return true, as well as ProcessImplementation::Validate() for
    * your instance implementation subclass, or a runtime exception will be
    * thrown.
    *
    * \note The default implementation of this function returns false, so by
    * default processes don't require special instance validation.
    *
    * \sa ProcessImplementation::Validate()
    */
   bool NeedsValidation() const override
   {
      return false; // by default, process instances are not validated.
   }

   /*!
    * Returns true iff the instances of this process prefer execution in the
    * global context, instead of being executed on views.
    *
    * Note that the option indicated by this function is just a \e hint to the
    * PixInsight core application, not necessarily observed, depending on the
    * context.
    *
    * To prevent (or select) execution on a particular context, your process
    * instance class must reimplement ProcessImplementation::CanExecuteOn()
    * and/or ProcessImplementation::CanExecuteGlobal(), as appropriate.
    *
    * \note The default implementation of this function returns false, so by
    * default processes prefer execution on views, instead of execution in the
    * global context.
    */
   bool PrefersGlobalExecution() const override
   {
      return false; // by default, processes prefer execution on views.
   }

   /*!
    * Handles a command line invocation. Returns a conventional exit code
    * (customarily, zero signals a "normal" or successful execution).
    *
    * \param argv    The list of command-line arguments.
    *
    * \note This function will never be called unless the
    * CanProcessCommandLines() member function is reimplemented to return true.
    *
    * \sa CanProcessCommandLines()
    */
   virtual int ProcessCommandLine( const StringList& argv ) const;

   /*!
    * Handles a request to edit process preferences. Returns true if the
    * preferences were successfully edited.
    *
    * Reimplement this function to allow the user to edit a set of preferences
    * specific to this process. This task is usually implemented on a modal
    * dialog box. In such case, if the user closes the dialog box without
    * accepting the new preferences settings (e.g. by clicking the dialog's
    * Cancel button) this member function should return false. This function
    * should only return true if the user has edited and accepted a new set of
    * preferences for this process.
    *
    * \note This function will never be called unless the CanEditPreferences()
    * member function is reimplemented to return true.
    *
    * \sa CanEditPreferences(), ProcessInterface::EditPreferences()
    */
   bool EditPreferences() const override;

   /*!
    * Handles a request to browse documentation specific for this process.
    * Returns true iff the documentation was loaded successfully.
    *
    * Reimplementing this function is strongly discouraged, unless some
    * nonstandard behavior is absolutely necessary for a particular process.
    * Since version 1.7 of the PixInsight Core application, the integrated
    * documentation system works in a completely automatic and standardized way
    * to provide documentation browsing and searching functionality.
    *
    * The default implementation of this member function automatically launches
    * and loads the integrated documentation browser with the documentation for
    * this process, if it exists and has been installed. For more information,
    * please read the documentation for the CanBrowseDocumentation() function.
    *
    * \note This function will never be called unless the
    * CanBrowseDocumentation() member function is reimplemented to return true.
    *
    * \sa CanBrowseDocumentation()
    */
   bool BrowseDocumentation() const override;

   /*!
    * Provides access to the list of parameters defined for this process.
    *
    * This member is intended for internal use of API module initialization
    * routines. You should not need to call this function directly under
    * normal conditions.
    */
   const MetaParameter* operator[]( size_type i ) const;

private:

   void PerformAPIDefinitions() const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_MetaProcess_h

// ----------------------------------------------------------------------------
// EOF pcl/MetaProcess.h - Released 2018-12-12T09:24:21Z
