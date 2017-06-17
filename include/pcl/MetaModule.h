//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/MetaModule.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_MetaModule_h
#define __PCL_MetaModule_h

/// \file pcl/MetaModule.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/MetaObject.h>
#include <pcl/String.h>
#include <pcl/Variant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MetaModule
 * \brief A formal description of a PixInsight module.
 *
 * %MetaModule is the root element in the hierarchical structure of components
 * that constitute every PixInsight module. This class provides a formal
 * description of the basic functionality and properties of a module.
 *
 * Every PixInsight module must define a \e unique subclass of %MetaModule.
 * That subclass, besides describing the module, will be the primary interface
 * between all module components (processes, file formats and interfaces) and
 * the PixInsight Core application.
 *
 * In any PixInsight module, there exists a \e unique instance of a derived
 * class of %MetaModule, accessible as a global pointer variable declared in
 * the pcl namespace, namely:
 *
 * \code
 * namespace pcl
 * {
 *    extern MetaModule* Module;
 * } // pcl
 * \endcode
 *
 * The entire tree of existing module components can always be accessed by
 * using the MetaObject::operator[]( size_type ) operator function recursively,
 * starting from the \c Module global variable.
 *
 * \sa MetaObject, MetaProcess, MetaParameter, MetaFileFormat
 */
class MetaModule : public MetaObject
{
public:

   /*!
    * Constructs a %MetaModule object.
    */
   MetaModule();

   /*!
    * Destroys a %MetaModule object and all of its child module components.
    *
    * This function effectively destroys \e all existing module components by
    * indirect recursion of MetaObject::~MetaObject().
    */
   virtual ~MetaModule();

   /*!
    * Copy constructor. This constructor is disabled because %MetaModule
    * represents unique server-side objects.
    */
   MetaModule( const MetaModule& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %MetaModule represents
    * unique server-side objects.
    */
   MetaModule& operator =( const MetaModule& ) = delete;

   /*!
    * \deprecated This member function has been deprecated. It is maintained for
    * compatibility with existing code - do not use it in newly produced code.
    */
   virtual const char* UniqueId() const;

   /*!
    * Returns a <em>version information</em> string for this PixInsight module.
    *
    * This member function <em>must</em> be reimplemented to return the
    * starting address of an invariant sequence of 8-bit characters. The
    * returned character string shall have the following format:
    *
    * PIXINSIGHT_MODULE_VERSION_\<version-string\>\<nul\>
    *
    * where:
    *
    * * PIXINSIGHT_MODULE_VERSION_ is a mandatory prefix. It must be included
    * literally at the beginning of the returned string.
    *
    * * \<version-string\> is a mandatory <em>version string</em> composed by
    * at least 17 characters. It must have the following format:
    *
    * MM.mm.rr.bbbb.LLL[.\<status\>]
    *
    * where MM is the <em>major version number</em>, mm is the <em>minor
    * version number</em>, rr is the <em>release version number</em>, and bbbb
    * is a <em>build number</em>. The three version numbers must be in the
    * range from zero to 99. The build number must be in the range from 1 to
    * 9999. The four numbers must be left padded with zeros in the returned
    * string, if necessary, and their values are supposed to increase with each
    * successive version of the module.
    *
    * LLL is a three-letter ISO 639.2 language code, which must correspond to
    * the primary language of the module (generally, the language used to write
    * visible messages and information of the user interface).
    *
    * \<status\> is an optional version status word. If included, it can be one
    * of the following:
    *
    * alpha    Indicates an alpha version of the module (a version at the early
    *          development stages, where the final features and functionality
    *          of the module still have not been defined).
    *
    * beta     Indicates a beta version of the module (a development version
    *          that still is not suitable for production release).
    *
    * rc\<n\>  A release candidate version (an almost finished version, still
    *          under development but very close to a final release). \<n\> is
    *          an arbitrary release candidate number.
    *
    * release  Indicates a release (final, production) version.
    *
    * exp      An experimental version (a special development version, not
    *          intended for public release; usually a confidential version
    *          distributed among the members of a development team)
    *
    * If no status is specified in the version information string, a release
    * version will be assumed.
    *
    * \note This is a pure virtual function that must be reimplemented by all
    * derived classes.
    *
    * \sa GetVersion()
    */
   virtual const char* Version() const = 0;

   /*!
    * Provides the version numbers, language code and development status of
    * this module.
    *
    * \param[out] major       Main (major) version number.
    *
    * \param[out] minor       Secondary (minor) version number.
    *
    * \param[out] release     Release number.
    *
    * \param[out] build       Build number.
    *
    * \param[out] language    An ISO 639.2 language code (3-letter code) that
    *             identifies the primary language of this module.
    *
    * \param[out] status      A string indicating the development status of
    *             this module (alpha, beta, release, etc.), or an empty string
    *             if no specific status has been specified. For possible
    *             values and their meanings, see the documentation for the
    *             Version() member function.
    *
    * This is a utility function, provided for convenience. It parses the
    * <em>version information</em> string returned by the Version() member
    * function, and stores the extracted values in the specified argument
    * variables.
    *
    * \sa Version()
    */
   void GetVersion( int& major, int& minor, int& release, int& build,
                            IsoString& language, IsoString& status ) const;

   /*!
    * Returns a readable version string for this module of the form:
    *
    * &lt;module-name&gt; version M.m.r.b
    *
    * where &lt;module-name&gt; is the string returned by the (reimplemented)
    * Name() virtual member function, and M.m.r.b are the major, minor, release
    * and build numbers, as provided by GetVersion().
    *
    * The string returned by this member function is suitable to be used for
    * module identification purposes, including metadata such as FITS keywords.
    */
   IsoString ReadableVersion() const;

   /*!
    * Returns an identifier for this module.
    *
    * The returned string must be a valid C identifier. The identifier must be
    * unique among the identifiers of all installed modules in the calling
    * instance of the PixInsight Core application.
    *
    * This function is not intended to provide a descriptive name, but just a
    * reference identifier for this module. For example, some identifiers of
    * standard PixInsight modules are: Geometry, Image, FITS.
    *
    * \note This is a pure virtual function that must be reimplemented by all
    * derived classes.
    */
   virtual IsoString Name() const = 0;

   /*!
    * Provides a \e brief description of this module.
    *
    * This function is not intended to give a thorough description, but just a
    * succint sentence to quickly identify this module and its contents.
    *
    * Example of good module description:
    * <em>"MyFancyCrop Process Module"</em>.
    *
    * Example of \e bad module description:
    * <em>"This module implements MyFancyCrop: A high-performance process to
    * crop and rotate images arbitrarily."</em>.
    *
    * \note Reimplementing this function in a derived class is optional.
    * However, this function \e should be reimplemented by all modules, to
    * provide meaningful information to the users during module installation
    * procedures.
    */
   virtual String Description() const
   {
      return String();
   }

   /*!
    * Returns the name of the software company responsible for the development
    * of this module.
    *
    * Please don't advertise your company here. A module returning something
    * like <em>"MySoftwareCompany - Advanced Imaging Solutions for the Serious
    * Imager"</em> wouldn't be certified.
    *
    * \note Reimplementing this function is optional, but advisable.
    */
   virtual String Company() const
   {
      return String();
   }

   /*!
    * Returns the name of the author or authors of this module.
    *
    * To specify multiple author names, separate them with commas.
    *
    * \note Reimplementing this function is optional, but advisable.
    */
   virtual String Author() const
   {
      return String();
   }

   /*!
    * Provides copyright information for this module.
    *
    * Example: <em>"Copyright (c) 2007, MySoftwareCompany. All Rights
    * Reserved"</em>.
    *
    * \note Reimplementing this function is optional, but advisable.
    */
   virtual String Copyright() const
   {
      return String();
   }

   /*!
    * Returns a string with a trade mark or a list of trade marks.
    *
    * To specify multiple trade marks, separate them with commas.
    *
    * \note Reimplementing this function is optional.
    */
   virtual String TradeMarks() const
   {
      return String();
   }

   /*!
    * Returns the original file name of this module.
    *
    * \note Reimplementing this function is optional.
    */
   virtual String OriginalFileName() const
   {
      return String();
   }

   /*!
    * Provides release date information for this module.
    *
    * \param[out] year  The (full) year this module has been released.
    *
    * \param[out] month The month this module has been released,
    *                   1 <= month <= 12, 1=January.
    *
    * \param[out] day   The day this module has been released,
    *                   1 <= day <= 31.
    *
    * The specified date must be correct; for example, something like
    * 2007/02/30 will not be accepted (the module won't install).
    *
    * \note Reimplementing this function is optional. If it is not
    * reimplemented by a derived class to return a valid date, the module will
    * provide no specific release date.
    */
   virtual void GetReleaseDate( int& year, int& month, int& day ) const
   {
      year = month = day = 0; // unspecified
   }

   /*!
    * Module allocation routine. Allocates a contiguous block of \a sz bytes,
    * and returns its starting address.
    *
    * This routine is called by PCL internal code, and indirectly by the
    * PixInsight core application.
    *
    * The default implementation simply calls ::operator new( sz ) to allocate
    * a block of the specified size.
    *
    * You usually should not need to reimplement this function, unless you want
    * to implement a special memory management strategy for your module.
    *
    * \sa Deallocate()
    */
   virtual void* Allocate( size_type sz )
   {
      PCL_PRECONDITION( sz != 0 )
      return reinterpret_cast<void*>( ::operator new( sz ) );
   }

   /*!
    * Module deallocation routine. Deallocates a previously allocated
    * contiguous block of memory starting at address \a p.
    *
    * This routine is called by PCL internal code, and indirectly by the
    * PixInsight core application.
    *
    * The default implementation simply calls ::operator delete( p ) to free
    * the specified block.
    *
    * You usually should not need to reimplement this function, unless you want
    * to implement a special memory management strategy for your module.
    *
    * \sa Allocate()
    */
   virtual void Deallocate( void* p )
   {
      PCL_PRECONDITION( p != 0 )
      ::operator delete( p );
   }

   /*!
    * Routine invoked just after this module has been installed.
    *
    * This function can be reimplemented to perform module-specific
    * initialization procedures.
    *
    * \sa OnUnload()
    */
   virtual void OnLoad()
   {
   }

   /*!
    * Routine invoked just before this module is unloaded in response to an
    * uninstall request, or just before the PixInsight Core application is
    * about to terminate execution.
    *
    * This function can be reimplemented to perform module-specific
    * finalization procedures.
    *
    * \sa OnLoad()
    */
   virtual void OnUnload()
   {
   }

   /*!
    * Returns true iff this module has been successfully installed. Only when
    * the module has been installed, communication with the PixInsight core
    * application is fully operative.
    */
   bool IsInstalled() const;

   /*!
    * Processes pending user interface and thread events.
    *
    * Call this function from the root thread (aka <em>GUI thread</em>) to let
    * the PixInsight core application process pending interface events, which
    * may accumulate while your code is running.
    *
    * Modules typically call this function during real-time preview generation
    * procedures. Calling this function from the root thread ensures that the
    * GUI remains responsive during long, calculation-intensive operations.
    *
    * If the \a excludeUserInputEvents parameter is set to true, no user input
    * events will be processed by calling this function. This includes mainly
    * mouse and keyboard events. Unprocessed input events will remain pending
    * and will be processed as appropriate when execution returns to
    * PixInsight's main event handling loop, or when this function is called
    * with \a excludeUserInputEvents set to false.
    *
    * When this function is invoked from a running thread, a ProcessAborted
    * exception will be thrown automatically if the Thread::Abort() member
    * function has been previously called for the running thread. This allows
    * stopping running processes in a thread-safe and controlled way. For more
    * information, read the documentation for the Thread class.
    *
    * \note Do not call this function too frequently from the root thread, as
    * doing so may degrade the performance of the whole PixInsight graphical
    * interface. For example, calling this function at 250 ms intervals is
    * reasonable and more than sufficient in most cases. Normally, you should
    * only need to call this function during real-time image and graphics
    * generation procedures, or from time-consuming processes, especially from
    * multithreaded code.
    *
    * \sa Thread, Thread::Abort()
    */
   void ProcessEvents( bool excludeUserInputEvents = false );

   /*!
    * Loads a bitmap resource file.
    *
    * \param filePath   Path to a resource file in Qt's binary resource format
    *                   (RCC format). This path must point to a location on the
    *                   local file system (remote resources are not allowed in
    *                   current PCL versions).
    *
    * \param rootPath   Root path for all the loaded bitmap resources. This
    *                   parameter is an empty string by default.
    *
    * If the specified resource file does not exist or is invalid, this
    * function will throw an Error exception.
    *
    * <b>Automatic Resource Location</b>
    *
    * Resource files can be loaded from arbitrary locations. However, modules
    * typically install their resources on the /rsc/rcc/modules directory under
    * the local PixInsight installation. A module can specify the
    * "@module_resource_dir/" prefix in resource file paths to let the
    * PixInsight core application load the resources from the appropriate
    * standard distribution directory automatically. For example, if a module
    * whose name is "Foo" makes the following call:
    *
    * \code LoadResource( "@module_resource_dir/MyResources.rcc" ); \endcode
    *
    * the core application will attempt to load the following resource file:
    *
    * &lt;install-dir&gt;/rsc/rcc/module/Foo/MyResources.rcc
    *
    * where &lt;install-dir&gt; is the local directory where the running
    * PixInsight core application is installed.
    *
    * <b>Resource Root Paths</b>
    *
    * All the resources loaded by a call to this function will be rooted at the
    * specified \a rootPath under a standard ":/module/<module-name>/"
    * root path prefix set by the PixInsight core application automatically.
    * For example, if the Foo module calls this function as follows:
    *
    * \code LoadResource( "@module_resource_dir/MyResources.rcc" ); \endcode
    *
    * and the MyResources.rcc file contains a resource named "foo-icon.png", it
    * will be available at the following resource path:
    *
    * :/module/Foo/foo-icon.png
    *
    * This system guarantees that a module will never invalidate or replace
    * existing resources loaded by the core application or other modules. Note
    * that a module still can cause problems by loading resources incorrectly,
    * but these problems cannot propagate outside the failing module.
    *
    * To ease working with module-defined resources, the standard
    * ":/@module_root/" prefix can be specified in resource file paths:
    *
    * \code Bitmap bmp( ":/@module_root/foo-icon.png" ); \endcode
    *
    * Continuing with the Foo module example, this call would load the bitmap
    * at ":/module/Foo/foo-icon.png".
    *
    * \b References
    *
    * \li The Qt Resource System: http://doc.qt.io/qt-5/resources.html
    *
    * \sa UnloadResource()
    */
   void LoadResource( const String& filePath, const String& rootPath = String() );

   /*!
    * Unloads a bitmap resource file.
    *
    * After calling this function, all the resources available from the
    * specified resource file will be unavailable. The \a filePath and
    * \a rootPath arguments must be identical to the ones used when the
    * resource was loaded by a call to LoadResource().
    *
    * If the specified resource has not been loaded, or if there are active
    * references to the resource (for example, because one or more existing
    * Bitmap objects depend on data stored in the resource), this function
    * will throw an Error exception.
    *
    * \sa LoadResource()
    */
   void UnloadResource( const String& filePath, const String& rootPath = String() );

   /*!
    * Executes a script in the platform's core scripting engine.
    *
    * \param sourceCode    A string containing valid source code in the
    *                      specified \a language.
    *
    * \param language      The name of a supported scripting language.
    *                      Currently only the JavaScript language is supported
    *                      by this function. JavaScript is assumed if this
    *                      string is either empty or equal to "JavaScript".
    *
    * Returns the result value of the executed script. The result value is the
    * value of the last executed expression statement in the script that is not
    * in a function definition.
    *
    * The script will be executed in the core JavaScript Runtime (PJSR). All
    * PJSR resources are available.
    *
    * If the script cannot be evaluated, for example because it has syntax
    * errors, or attempts to execute invalid code, or throws an exception, this
    * member function throws an Error exception.
    *
    * \note This function can only be called from the root thread, since the
    * core JavaScript engine in not reentrant in current versions of
    * PixInsight. Calling this function from a running thread will throw an
    * Error exception.
    *
    * \warning You should make sure that your code has been well tested before
    * calling this function. PixInsight pursues efficiency and script execution
    * is no exception. Nothing will protect or watch your code, or help you
    * stop it at any point. If your code enters an infinite loop, it will crash
    * the whole PixInsight platform without remedy. Also bear in mind that
    * scripts are extremely powerful and potentially dangerous if you don't
    * know well what you are doing. What happens during execution of your
    * scripts is your entire responsibility. We mean it.
    */
   Variant EvaluateScript( const String& sourceCode, const IsoString& language = IsoString() );

private:

   virtual void PerformAPIDefinitions() const;

   friend class APIInitializer;
};

// ----------------------------------------------------------------------------

extern MetaModule* Module;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::InstallMode
 * \brief     Module installation modes.
 *
 * This namespace enumerates module installation modes that the PixInsight core
 * application passes to the module installation entry point (PMINS). See the
 * \ref module_entry_points "Module Entry Points" section for detailed
 * information.
 *
 * The current installation modes are:\n
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>InstallMode::FullInstall</td>     <td>Normal, full installation</td></tr>
 * <tr><td>InstallMode::QueryModuleInfo</td> <td>Temporary load to gather module properties</td></tr>
 * <tr><td>InstallMode::VerifyModule</td>    <td>Temporary load to verify module integrity</td></tr>
 * </table>
 */
namespace InstallMode
{
   enum value_type
   {
      FullInstall,     // Normal, full installation
      QueryModuleInfo, // Temporary load to gather module properties
      VerifyModule     // Temporary load to verify module integrity
   };
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup metamodule_utility_macros MetaModule Utility Macros
 */

// Stringification of macro-expanded arguments needs two levels of macros.
#define PCL_STRINGIFY( x ) #x

/*!
 * \def        PCL_MODULE_UNIQUE_ID
 * \deprecated This macro has been deprecated. It is kept as part of PCL for
 *             compatibility with existing modules. Do not use it in newly
 *             produced code.
 * \ingroup    metamodule_utility_macros
 */
#define PCL_MODULE_UNIQUE_ID( uid ) \
               ("PIXINSIGHT_MODULE_UNIQUE_ID_" PCL_STRINGIFY( uid ))

/*!
 * \def        PCL_MODULE_VERSION
 * \brief      A utility macro to build a module version string.
 * \ingroup    metamodule_utility_macros
 *
 * \param MM   The main (major) version number of the module. Must be an
 *             integer in the range from 0 to 99, and must be left-padded with
 *             a zero, when necessary.
 *
 * \param mm   The second (minor) version number of the module. Must be an
 *             integer in the range from 0 to 99, and must be left-padded with
 *             a zero, when necessary.
 *
 * \param rr   The revision version number of the module. Must be an integer in
 *             the range from 0 to 99, and must be left-padded with a zero,
 *             when necessary.
 *
 * \param bbbb The build version number of the module. Must be an integer in
 *             the range from 1 to 9999, and must be left-padded with a maximum
 *             of three zeros, as necessary.
 *
 * \param lan  The primary language of the module. Must be a valid ISO 639.2
 *             language code (a 3-letter code identifying a language).
 *
 * This macro generates a string literal with the mandatory
 * "PIXINSIGHT_MODULE_VERSION_" prefix prepended to the actual module version
 * string, which is composed by concatenation of the specified macro arguments.
 *
 * Example:
 *
 * \code
 * #define MODULE_VERSION_MAJOR     01
 * #define MODULE_VERSION_MINOR     00
 * #define MODULE_VERSION_REVISION  02
 * #define MODULE_VERSION_BUILD     0045
 * #define MODULE_VERSION_LANGUAGE  eng
 * ...
 * #include <pcl/MetaModule.h>
 * ...
 * class MyModule : public pcl::MetaModule
 * {
 *    // Define your module class here
 * };
 * ...
 * const char* MyModule::Version() const
 * {
 *    return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
 *                               MODULE_VERSION_MINOR,
 *                               MODULE_VERSION_REVISION,
 *                               MODULE_VERSION_BUILD,
 *                               MODULE_VERSION_LANGUAGE );
 * }
 * \endcode
 *
 * In the example above, the Version() member of MyModule would return the
 * following version string:
 *
 * "PIXINSIGHT_MODULE_VERSION_01.00.02.0045.eng"
 *
 * \sa PCL_MODULE_VERSION_S, PCL_MODULE_UNIQUE_ID
 */
#define PCL_MODULE_VERSION( MM, mm, rr, bbbb, lan )   \
               ("PIXINSIGHT_MODULE_VERSION_"          \
                PCL_STRINGIFY( MM )    "."            \
                PCL_STRINGIFY( mm )    "."            \
                PCL_STRINGIFY( rr )    "."            \
                PCL_STRINGIFY( bbbb )  "."            \
                PCL_STRINGIFY( lan ))

/*!
 * \def        PCL_MODULE_VERSION_S
 * \brief      A utility macro to build a module version string, including a
 *             development status specification.
 * \ingroup    metamodule_utility_macros
 *
 * This macro is an extension of the PCL_MODULE_VERSION macro. It generates a
 * version string that also includes a <em>development status</em>, specified
 * as the \a status macro argument.
 *
 * The \a status argument can have the following values:
 *
 * alpha    For an alpha development status
 *
 * beta     For a beta development status
 *
 * release  For a release (final) version
 *
 * rc\<n\>  For a release candidate version, where \<n\> is the release
 *          candidate number.
 *
 * exp      For an experimental version (usually a development version not
 *          intended for public release).
 *
 * Other values are not recognized by the platform and cause rejection of a
 * module for installation.
 *
 * This macro generates a string that is suitable to be returned by a
 * reimplementation of MetaModule::Version(). An example follows:
 *
 * Example:
 *
 * \code
 * #define MODULE_VERSION_MAJOR     01
 * #define MODULE_VERSION_MINOR     00
 * #define MODULE_VERSION_REVISION  02
 * #define MODULE_VERSION_BUILD     0045
 * #define MODULE_VERSION_LANGUAGE  eng
 * #define MODULE_VERSION_STATUS    rc3
 * ...
 * #include <pcl/MetaModule.h>
 * ...
 * class MyModule : public pcl::MetaModule
 * {
 *    // Define your module class here
 * };
 * ...
 * const char* MyModule::Version() const
 * {
 *    return PCL_MODULE_VERSION_S( MODULE_VERSION_MAJOR,
 *                                 MODULE_VERSION_MINOR,
 *                                 MODULE_VERSION_REVISION,
 *                                 MODULE_VERSION_BUILD,
 *                                 MODULE_VERSION_LANGUAGE,
 *                                 MODULE_VERSION_STATUS );
 * }
 * \endcode
 *
 * In the example above, the Version() member of MyModule would return the
 * following version string:
 *
 * "PIXINSIGHT_MODULE_VERSION_01.00.02.0045.eng.rc3"
 *
 * \sa PCL_MODULE_VERSION, PCL_MODULE_UNIQUE_ID
 */
#define PCL_MODULE_VERSION_S( MM, mm, rr, bbbb, lan, status ) \
               ("PIXINSIGHT_MODULE_VERSION_"          \
                PCL_STRINGIFY( MM )    "."            \
                PCL_STRINGIFY( mm )    "."            \
                PCL_STRINGIFY( rr )    "."            \
                PCL_STRINGIFY( bbbb )  "."            \
                PCL_STRINGIFY( lan )   "."            \
                PCL_STRINGIFY( status ))

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

/*!
 * \defgroup module_entry_points Module Entry Point Functions
 *
 * Module entry points are special functions required for module installation
 * and initialization. Entry points are called directly by the PixInsight core
 * application and must be implemented following the standard 'C' naming and
 * calling conventions as nonstatic, publicly visible global symbols.
 *
 * In current versions of the PixInsight platform, there are two mandatory
 * module entry points, namely the identification and initialization entry
 * points (known as PMIDN and PMINI, respectively), and an optional (although
 * usually necessary) installation entry point (PMINS). We describe these
 * special functions in detail in this section.
 *
 * \n
 * <h4>PixInsight Module Identification (PMIDN) entry point</h4>
 *
 * This function will be called by the PixInsight core application when it
 * needs to retrieve identification and descriptive data from a module. This
 * happens when a module is about to be installed, but also when a module is
 * being inspected, either for security reasons or after a direct user request.
 *
 * A PMIDN must have the following declaration:
 *
 * \code
 * PCL_MODULE_EXPORT uint32 IdentifyPixInsightModule( api_module_description** description, int32 phase );
 * \endcode
 *
 * \param description   Pointer to a pointer to an API module description
 *          structure. See the declaration of api_module_description in
 *          API/APIDefs.h for details. This structure must be provided by the
 *          called module as POD, and its starting address must be written to
 *          the pointer pointed to by this argument in a call to this function
 *          with \a phase = 1 (see below).
 *
 * \param phase         Module identification request:\n
 *          \li \a phase = 0x00 - Prepare for module identification.\n
 *          \li \a phase = 0x01 - Report module descriptive data in a structure
 *          of type 'api_module_description', whose address must be stored in
 *          the pointer pointed to by the \a description argument.\n
 *          \li \a phase = 0xff - Module identification process completed. The
 *          module description structure provided when \a phase = 1 can be
 *          deallocated and disposed as necessary.
 *
 * Other values of \a phase may be passed in additional calls to a PMIDN. Those
 * values and calls are reserved for special modules pertaining to the core of
 * the PixInsight platform, and hence not in the user land. In current versions
 * of PixInsight, such special calls must be ignored by the invoked module.
 *
 * A PMIDN must return zero upon success. Any other return value will be
 * interpreted as a module-specific error code.
 *
 * Module developers using the standard PixInsight Class Library (PCL)
 * distribution don't have to care about PMIDN, since it is already implemented
 * internally by PCL.
 *
 * \note A PMIDN is mandatory for all PixInsight modules, and must be
 * implemented as a nonstatic, publicly visible global symbol following the
 * standard 'C' naming and calling conventions. The PCL_MODULE_EXPORT macro
 * guarantees these conditions on all supported C++ compilers.
 *
 * \n
 * <h4>PixInsight Module Initialization (PMINI) entry point</h4>
 *
 * This function will be called by the PixInsight core application when a
 * module is being installed. It provides a module with the necessary elements
 * to perform a bidirectional communication with the core application via the
 * standard PCL API callback system.
 *
 * A PMINI must have the following declaration:
 *
 * \code
 * PCL_MODULE_EXPORT uint32 InitializePixInsightModule( api_handle hModule, function_resolver R, uint32 apiVersion, void* reserved );
 * \endcode
 *
 * \param hModule       Handle to the module being installed. This handle
 *                      uniquely identifies the module, and must be used in all
 *                      subsequent API calls requiring a module handle.
 *
 * \param R             Pointer to an API function resolver callback. See the
 *                      declaration of function_resolver in API/APIDefs.h and
 *                      the automatically generated file pcl/APIInterface.cpp
 *                      for details.
 *
 * \param apiVersion    API version number.
 *
 * \param reserved      Reserved for special invocations to core platform
 *                      modules. Must not be used or altered in any way.
 *
 * A PMINI must return zero upon success. Any other return value will be
 * interpreted as a module-specific error code.
 *
 * Module developers using the standard PixInsight Class Library (PCL)
 * distribution don't have to care about PMINI, since it is already implemented
 * internally by PCL.
 *
 * \note A PMINI is mandatory for all PixInsight modules, and must be
 * implemented as a nonstatic, publicly visible global symbol following the
 * standard 'C' naming and calling conventions. The PCL_MODULE_EXPORT macro
 * guarantees these conditions on all supported C++ compilers.
 *
 * \n
 * <h4>PixInsight Module Installation (PMINS) entry point</h4>
 *
 * If this function is defined as a public symbol in a module, the PixInsight
 * core application will call it just after loading and initializing the module
 * shared object or dynamic-link library. This happens after calling the
 * mandatory module entry points PMIDN and PMINI.
 *
 * A PMINS must have the following declaration:
 *
 * \code
 * PCL_MODULE_EXPORT int32 InstallPixInsightModule( int32 mode );
 * \endcode
 *
 * \param mode          Specifies the kind of installation being performed by
 *                      the PixInsight core application. See the
 *                      pcl::InstallMode namespace for more information.
 *
 * A PMINS must return zero upon successful installation. Any other return
 * value will be interpreted as a module-specific error code.
 *
 * Although a PMINS is optional, it normally has to be defined by non-trivial
 * modules in order to create and initialize the different objects and
 * meta-objects required to implement their functionality, since most of these
 * objects are dynamic in PCL. See the source code of the open-source standard
 * modules for examples. Look for &lt;module_name&gt;Module.cpp files.
 *
 * \note If available, a PMINS must be implemented as a nonstatic, publicly
 * visible global symbol following the standard 'C' naming and calling
 * conventions. The PCL_MODULE_EXPORT macro guarantees these conditions on all
 * supported C++ compilers.
 */

// ----------------------------------------------------------------------------

// end global namespace

#endif   // __PCL_MetaModule_h

// ----------------------------------------------------------------------------
// EOF pcl/MetaModule.h - Released 2017-06-17T10:55:43Z
