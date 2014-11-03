// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ProcessInterface.h - Released 2014/10/29 07:34:07 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_ProcessInterface_h
#define __PCL_ProcessInterface_h

/// \file pcl/ProcessInterface.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Flags_h
#include <pcl/Flags.h>
#endif

#ifndef __PCL_MetaObject_h
#include <pcl/MetaObject.h>
#endif

#ifndef __PCL_Control_h
#include <pcl/Control.h>
#endif

#ifndef __PCL_ImageVariant_h
#include <pcl/ImageVariant.h>
#endif

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace InterfaceFeature
 * \brief     Provides symbolic constants defining feature flags to customize
 *            process interfaces.
 *
 * Besides defining their specific GUI components and behavior, process
 * interfaces can be customized in a standardized way by means of a series of
 * flags that control the contents and functionality of their <em>interface
 * control bars</em>. Those flags are defined through symbolic constants in the
 * InterfaceFeature namespace.
 *
 * An interface control bar is a managed control located at its parent
 * interface window's bottom side. It is a relatively small panel where a
 * number of standard tool buttons and a one-line text area can be customized.
 *
 * A standard process interface window has the following layout in PixInsight:
 *
 * <pre>
 *    +---------------------------------+
 *    |      Interface Title Bar        +
 *    +---------------------------------+
 *    |                                 |
 *    |     Module-Defined              |
 *    |     Interface Client Area       |
 *    |                                 |
 *    +---------------------------------+
 *    | Info Area                       |
 *    +---------------------------------+
 *    | D A G T E X             B V P R |
 *    +---------------------------------+
 * </pre>
 *
 * Where the two sections at the bottom form the interface's
 * <em>control bar</em>:
 *
 * \li <b>Info Area</b> is a one-line text label that can be used to provide
 * some feedback and information to the user. The contents of the info area can
 * be modified programmatically, but cannot be changed by the user.
 *
 * \li <b>D = %Process Drag %Object</b>, used to create a new process instance
 * by dragging it with the mouse. The new instance can be dragged for execution
 * on selected views, to create process icons, to add instances to a process
 * container, etc.
 *
 * \li <b>A = Apply button</b>, used for execution of a new process instance on
 * the current view or window.
 *
 * \li <b>G = Apply Global button</b>, used for execution of a new process
 * instance in the global context.
 *
 * \li <b>T = Real-Time %Preview button</b>, to attach this interface to the
 * real-time previewing system.
 *
 * \li <b>E = Execute button</b>, used to commit execution of a dynamic
 * interface.
 *
 * \li <b>X = Cancel button</b>, used to cancel execution of a dynamic
 * interface.
 *
 * \li <b>B = Browse Documentation button</b>. When this button is activated,
 * the default behavior is to open the Documentation Browser interface and load
 * it with the documentation for the process for which this open the %Process
 * Explorer window and show the .
 *
 * \li <b>V = Track %View check box</b>. When this check box is checked, the
 * interface should modify its behavior in some way as a function of the
 * current view. This check box generates calls to
 * ProcessInterface::TrackViewUpdated(). Interfaces using this feature
 * also need receiving ProcessInterface::ImageFocused() notifications.
 *
 * \li <b>P = Preferences button</b>, to open a modal dialog box where
 * process and interface-specific preferences can be edited.
 *
 * \li <b>R = Reset button</b>, to reset the current instance to a default
 * state.
 *
 * The following symbolic constants have been publicly defined for interface
 * feature flags:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>InterfaceFeature::None</td>                      <td>This effectively suppresses a interface's control bar</td></tr>
 * <tr><td>InterfaceFeature::DragObject</td>                <td>%Process drag object - To generate new process instances</td></tr>
 * <tr><td>InterfaceFeature::ApplyToViewButton</td>         <td>Apply button - To execute a process on the active view</td></tr>
 * <tr><td>InterfaceFeature::ApplyGlobalButton</td>         <td>Apply Global button - To execute a process in the global context</td></tr>
 * <tr><td>InterfaceFeature::RealTimeButton</td>            <td>Real-Time Preview button - To activate the real-time preview interface</td></tr>
 * <tr><td>InterfaceFeature::ExecuteButton</td>             <td>Execute button - To commit a dynamic interface</td></tr>
 * <tr><td>InterfaceFeature::CancelButton</td>              <td>Cancel button - To cancel a dynamic interface</td></tr>
 * <tr><td>InterfaceFeature::BrowseDocumentationButton</td> <td>Browse documentation button - Browse process-specific documentation</td></tr>
 * <tr><td>InterfaceFeature::TrackViewButton</td>           <td>Track View check box - To activate/deactivate tracking of the current view</td></tr>
 * <tr><td>InterfaceFeature::PreferencesButton</td>         <td>Preferences button - To edit process and interface-specific preferences</td></tr>
 * <tr><td>InterfaceFeature::ResetButton</td>               <td>Reset button - To reset process instance parameters</td></tr>
 * <tr><td>InterfaceFeature::InfoArea</td>                  <td>Info Area - To create a single-line text area</td></tr>
 * <tr><td>InterfaceFeature::Default</td>                   <td>A default set of flags for static interfaces</td></tr>
 * <tr><td>InterfaceFeature::DefaultGlobal</td>             <td>A default set of flags for static interfaces that execute globally</td></tr>
 * <tr><td>InterfaceFeature::DefaultDynamic</td>            <td>A default set of flags for dynamic interfaces</td></tr>
 * </table>
 *
 * More functionality can be added to interface control bars in future
 * PixInsight and PCL releases.
 */
namespace InterfaceFeature
{
   enum mask_type
   {
      None                      = 0x00000000,  // This effectively suppresses a interface's control bar
      DragObject                = 0x00000001,  // Process drag object - To generate new process instances
      ApplyToViewButton         = 0x00000002,  // Apply button - To execute a process on the active view
      ApplyGlobalButton         = 0x00000004,  // Apply Global button - To execute a process in the global context
      RealTimeButton            = 0x00000008,  // Real-Time Preview button - To activate the real-time preview interface
      ExecuteButton             = 0x00000010,  // Execute button - To commit a dynamic interface
      CancelButton              = 0x00000020,  // Cancel button - To cancel a dynamic interface
      BrowseDocumentationButton = 0x00000080,  // Browse Documentation button - To browse process-specific documentation
      TrackViewButton           = 0x00000100,  // Track View check box - To activate/deactivate tracking of the current view
      PreferencesButton         = 0x00000400,  // Preferences button - To edit process and interface-specific preferences
      ResetButton               = 0x00000800,  // Reset button - To reset process instance parameters
      InfoArea                  = 0x00010000,  // Info Area - To create a single-line text area

#if defined( _MSC_VER ) && _MSC_VER >= 1700 // VC++ >= 2012
      Default        = 2179,
      DefaultGlobal  = 2181,
      DefaultDynamic = 2225
#else
      Default        = DragObject | ApplyToViewButton | BrowseDocumentationButton | ResetButton, // A default set of flags for static interfaces
      DefaultGlobal  = DragObject | ApplyGlobalButton | BrowseDocumentationButton | ResetButton, // A default set of flags for static interfaces that execute globally
      DefaultDynamic = DragObject | ExecuteButton | CancelButton | BrowseDocumentationButton | ResetButton // A default set of flags for dynamic interfaces
#endif
   };
}

/*!
 * \typedef InterfaceFeatures
 * \brief A collection of interface feature flags.
 */
typedef Flags<InterfaceFeature::mask_type>   InterfaceFeatures;

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

class PCL_CLASS MetaProcess;
class PCL_CLASS ProcessImplementation;
class PCL_CLASS View;
class PCL_CLASS Graphics;

// ----------------------------------------------------------------------------

/*!
 * \class ProcessInterface
 * \brief Client-side interface to a PixInsight process interface window.
 *
 * ### TODO: PCL 2.0.x: Rename this class to %ProcessInterfaceImplementation
 * ### TODO: PCL 2.x: New %ProcessInterface class for intermodule communication
 *
 * ### TODO: Write a detailed description for %ProcessInterface.
 */
class PCL_CLASS ProcessInterface : public Control, public MetaObject
{
public:

   /*!
    * Constructs a process interface window.
    */
   ProcessInterface();

   /*!
    * Destroys a %ProcessInterface object.
    */
   virtual ~ProcessInterface()
   {
   }

   /*!
    * Returns a reference to a null %ProcessInterface instance. A null
    * %ProcessInterface does not correspond to an existing interface window in
    * the PixInsight core application.
    */
   static ProcessInterface& Null();

   /*!
    * Returns an identifier for this interface.
    *
    * Interface identifiers must be unique (within the set of installed
    * interfaces), valid C identifiers.
    *
    * \note Although an arbitrary identifier can be used for a process
    * interface, it is customary (and also a good idea, for the sake of
    * clarity) to assign the same identifier to both a process and its
    * standard interface. For example, \c FastRotation is used as the
    * identifier of the FastRotation process and its interface window.
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns a list of alias identifiers for this interface.
    *
    * An interface can have one or more <em>alias identifiers</em>. Aliases are
    * useful to maintain compatibility with previous versions of an interface.
    *
    * When the PixInsight core application loads a project, it automatically
    * replaces alias interface identifiers with actual (current) identifiers.
    * This allows a developer to change the identifier of a process interface
    * without breaking compatibility with existing projects.
    *
    * The returned string is a comma-separated list of distinct identifiers
    * that will be treated as synonyms to the actual identifier of this
    * interface, which is returned by the Id() member function. Two installed
    * interfaces can't have the same alias, so each identifier included in the
    * list must be unique across all process interfaces on the entire
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
    * Returns a pointer to the process associated with this interface.
    *
    * In general, each interface window works mainly for a particular process.
    * This is what is known as the <em>standard interface</em> of a process in
    * PixInsight.
    */
   virtual MetaProcess* Process() const = 0;

   /*!
    * Returns a version number for this interface, encoded as a hexadecimal
    * number.
    *
    * For example, version 1.0.5 should be returned as 0x105, and version
    * 3.11.5 as 0x3B5. The default return value is 0x100, corresponding to
    * version 1.0.0.
    *
    * \deprecated This function has been deprecated and should not be used.
    * Interfaces no longer need version numbers. The value returned by this
    * member function is always ignored by the PixInsight core application.
    */
   virtual uint32 Version() const
   {
      return 0x100;
   }

   /*!
    * Returns a description of this interface.
    *
    * This function must provide a brief description of the functionality and
    * purpose of this process interface. This should include which
    * process(es) can be interfaced, and how.
    *
    * \deprecated This function has been deprecated and should not be used.
    * Interfaces must be described by the process(es) that use them. The value
    * returned by this member function is always ignored by the PixInsight Core
    * application.
    */
   virtual String Description() const
   {
      return String();
   }

   /*!
    * Returns a <em>large icon</em> for this interface as an image in the
    * standard XPM format.
    *
    * The specified image will be used as an icon appearing on the interface
    * window's title bar. To facilitate the visual identification of processes
    * and their interfaces (and to save hard icon design work :), it is a good
    * idea that this function returns the same image returned by its interfaced
    * process' MetaProcess::IconImageXPM().
    *
    * 32-bit RGBA color images (including an alpha channel) are fully
    * supported.
    *
    * If this function returns zero, a default icon will be assigned to this
    * interface automatically.
    *
    * \sa IconImageFile()
    */
   virtual const char** IconImageXPM() const
   {
      return 0;
   }

   /*!
    * Returns a <em>large icon</em> for this interface as a path specification
    * to an existing image file.
    *
    * Supported image file formats include PNG, XPM, JPG and BMP.
    *
    * For details on interface icon images, see the documentation for
    * IconImageXPM().
    *
    * \deprecated Using this function is \e discouraged, since it produces an
    * unnecessary dependency on an external file, which complicates the module
    * installation procedure. The recommended method is to reimplement the
    * IconImageXPM() member function in a derived class, to provide the address
    * of a static image in the XPM format.
    *
    * \sa IconImageXPM()
    */
   virtual String IconImageFile() const
   {
      return String();
   }

   /*!
    * Returns a <em>small icon</em> for this interface as an image in the
    * standard XPM format.
    *
    * For details on interface icon images, see the documentation for
    * IconImageXPM().
    *
    * Small icons are used on interface elements where screen space must be
    * preserved. This may include iconized interface windows. When this
    * function is not reimplemented in a derived class, the PixInsight core
    * application automatically generates a small icon by resampling down the
    * large icon provided by IconImageXPM().
    *
    * You normally should not need to reimplement this function; the core
    * application usually does a fine work resampling large icons to generate
    * reduced versions.
    *
    * \sa SmallIconImageFile()
    */
   virtual const char** SmallIconImageXPM() const
   {
      return 0;
   }

   /*!
    * Returns a <em>small icon</em> for this interface as a path specification
    * to an existing image file.
    *
    * Supported image file formats include PNG, XPM, JPG and BMP.
    *
    * For details on small interface icon images, see the documentation for
    * SmallIconImageXPM().
    *
    * \deprecated Using this function is discouraged for the same reasons
    * explained in the documentation entry for IconImageFile().
    *
    * \sa SmallIconImageXPM()
    */
   virtual String SmallIconImageFile() const
   {
      return String();
   }

   /*!
    * Returns a combination of flags defining the contents and functionality of
    * the interface control bar in this process interface.
    *
    * The returned value must be an OR'ed combination of flags defined by
    * symbolic constants in the InterfaceFeature namespace.
    *
    * \note The default implementation of this function returns
    * InterfaceFeature::Default, which is a default set of flags appropriate
    * for most interfaces working for processes that execute on views.
    *
    * \sa InterfaceFeature
    */
   virtual InterfaceFeatures Features() const
   {
      return InterfaceFeature::Default;
   }

   /*!
    * Function called when the user clicks the Apply button on the interface
    * control bar.
    *
    * For this function to be invoked, the InterfaceFeature::ApplyToViewButton
    * flag must be included in the set of feature flags returned by the
    * Features() member function, or a reimplementation of it, as appropriate.
    *
    * \note The default implementation of this function obtains a new process
    * instance by calling NewProcess() for this interface, then executes it on
    * the active view, if there is one.
    */
   virtual void ApplyInstance() const;

   /*!
    * Function called when the user clicks the Apply Global button on the
    * interface control bar.
    *
    * For this function to be invoked, the InterfaceFeature::ApplyGlobalButton
    * flag must be included in the set of feature flags returned by a
    * reimplementation of the Features() member function in a derived class.
    *
    * \note The default implementation of this function obtains a new process
    * instance by calling NewProcess() for this interface, then executes it in
    * the global context.
    */
   virtual void ApplyInstanceGlobal() const;

   /*!
    * Function called when the user clicks the Execute button on the interface
    * control bar.
    *
    * For this function to be invoked, the InterfaceFeature::ExecuteButton flag
    * must be included in the set of feature flags returned by a
    * reimplementation of the Features() member function in a derived class.
    *
    * \note The default implementation of this function does nothing. It is
    * reimplemented by derived classes implementing dynamic interfaces, to
    * commit the active dynamic session. An example is the standard DynamicCrop
    * interface. The reimplementation of this function in the DynamicCrop
    * interface executes the defined crop/rotate/rescale operation on the
    * interface's dynamic target image and terminates the dynamic session.
    */
   virtual void Execute()
   {
   }

   /*!
    * Function called when the user clicks the Cancel button on the
    * interface control bar.
    *
    * For this function to be invoked, the InterfaceFeature::CancelButton
    * flag must be included in the set of feature flags returned by a
    * reimplementation of the Features() member function in a derived class.
    *
    * \note The default implementation of this function terminates the active
    * dynamic session, if there is one. It is unlikely that this function needs
    * a reimplementation in a derived class, unless a nonstandard interface
    * behavior is desired.
    */
   virtual void Cancel();

   /*!
    * Function called when the user toggles the pushed state of the Real Time
    * Preview control bar button.
    *
    * \param active  True if the Real Time Preview button is currently active
    *             (pushed down); false otherwise (if the button is unpushed).
    *
    * For this function to be invoked, the InterfaceFeature::RealTimeButton
    * flag must be included in the set of feature flags returned by a
    * reimplementation of the Features() member function in a derived class.
    *
    * Derived classes for interfaces that use the real time previewing system
    * must reimplement this function to control ownership of the Real Time
    * Preview window and provide real-time preview images, as necessary.
    *
    * \note The default implementation of this function does nothing.
    */
   virtual void RealTimePreviewUpdated( bool active )
   {
   }

   /*!
    * Function called when the user clicks the Browse Documentation button on
    * the interface control bar. Returns true if the documentation was loaded
    * successfully.
    *
    * For this function to be invoked, the
    * InterfaceFeature::BrowseDocumentationButton flag must be included in the
    * set of feature flags returned by a reimplementation of the Features()
    * member function in a derived class.
    *
    * The default implementation of this member function is a wrapper for the
    * MetaProcess::BrowseDocumentation() virtual member function of the process
    * class associated with this interface. In other words, it basically does
    * the following:
    *
    * \code
    * const MetaProcess* P = Process();
    * if ( P != 0 && P->CanBrowseDocumentation() )
    *    P->BrowseDocumentation();
    * \endcode
    *
    * In general, a derived class should not need to reimplement this member
    * function, as the default behavior is appropriate in most cases.
    */
   virtual bool BrowseDocumentation() const;

   /*!
    * Function called when the user toggles the pushed state of the Track %View
    * control bar button.
    *
    * \param active  True if the Track %View button is currently active (pushed
    *                down); false otherwise (if the button is unpushed).
    *
    * For this function to be invoked, the InterfaceFeature::TrackViewButton
    * flag must be included in the set of feature flags returned by a
    * reimplementation of the Features() member function in a derived class.
    *
    * <em>%View tracking</em> is used by interfaces that modify their behavior
    * and/or information contents as a function of the current view in the
    * PixInsight application's GUI. When a view is selected, the interface
    * receives an ImageFocused() notification and, if view tracking is active,
    * it takes the opportune actions.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa Control::IsTrackViewActive(), Control::SetTrackViewActive(),
    * ImageFocused()
    */
   virtual void TrackViewUpdated( bool active )
   {
   }

   /*!
    * Function called when the user clicks the Preferences control bar button.
    *
    * For this function to be invoked, the InterfaceFeature::PreferencesButton
    * flag must be included in the set of feature flags returned by a
    * reimplementation of the Features() member function in a derived class.
    *
    * If you reimplement this function, its standard behavior is launching a
    * modal dialog to allow editing preferences and options pertaining to the
    * process(es) associated to this interface.
    *
    * \note Note that along with this function, a process should reimplement
    * the MetaProcess::EditPreferences() member in its MetaProcess subclass,
    * and that both implementations should be coherent.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa MetaProcess::CanEditPreferences(), MetaProcess::EditPreferences()
    */
   virtual void EditPreferences()
   {
   }

   /*!
    * Function called when the user clicks the Reset control bar button.
    *
    * For this function to be invoked, the InterfaceFeature::ResetButton
    * flag must be included in the set of feature flags returned by a
    * reimplementation of the Features() member function in a derived class.
    *
    * If you reimplement this function, its standard behavior is performing a
    * reset of the current process instance in this interface, loading its
    * default parameter values, and updating interface elements in consequence.
    *
    * \note The default implementation of this function does nothing.
    */
   virtual void ResetInstance()
   {
   }

   /*!
    * Interface initialization routine.
    *
    * This member function is called when the control handle of this interface
    * becomes valid, that is, just after the window holding this interface has
    * been created.
    *
    * You can reimplement this function to create all child controls of this
    * interface, as well as to set up a number of important control features,
    * like a window title, colors, fonts, and so on.
    *
    * \deprecated This function has been deprecated and should not be used.
    * Interface initialization should be carried out using the \e deferred
    * \e initialization technique by reimplementing the Launch() virtual member
    * function. See the documentation for these functions for more detailed
    * information.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa Launch( const MetaProcess&, const ProcessImplementation*, bool&, unsigned& ),
    * Launch( unsigned )
    */
   virtual void Initialize()
   {
   }

   /*!
    * %Process interface launch routine.
    *
    * \param process    Reference to the metaprocess describing the process
    *             class that is launching this interface, either directly as
    *             its default interface, or through an existing process
    *             instance.
    *
    * \param instance   Address of a process instance that is trying to launch
    *             this interface, or zero if it is being launched directly as
    *             the default interface of the \a process.
    *
    * \param[out] dynamic  True if this interface wants to be launched in
    *             dynamic mode; false if this interface will launch as a static
    *             interface.
    *
    * \param flags      Reserved for future extensions - must not be modified.
    *
    * This function is called whenever an interface is about to be \e launched.
    * An interface can be launched either directly as the default interface of
    * a process class (e.g., from PixInsight's %Process Explorer window), or
    * through an existing process instance (e.g., by double-clicking a process
    * icon). The argument values identify the process class and, if nonzero,
    * the process instance.
    *
    * This function must return true if it accepts to be launched under the
    * conditions specified by the arguments. After successfully launching an
    * interface from a process instance (i.e., if \a instance is nonzero), the
    * ValidateProcess() member function is invoked with a reference to the
    * instance. If the instance is validated, the ImportProcess() function is
    * finally called.
    *
    * If this function returns true, the \a dynamic referenced argument
    * variable must be set to \c true if this interface wants to start in
    * dynamic mode. If an interface launches in dynamic mode, a final call to
    * EnterDynamicMode() will occur before the interface window is shown and/or
    * activated. Note that the actual <em>dynamic session</em> will not really
    * start until a first view is selected by the user to serve as a
    * <em>dynamic target.</em>
    *
    * <b>Interface Initialization</b>
    *
    * Initializing an interface means building its sub-controls and setting up
    * a number of important control features, such as a window title, colors,
    * fonts, adjusting sizes, and so on. This job can be done by reimplementing
    * the Initialize() member function, which the PixInsight core application
    * calls as soon as the control handle of this interface becomes valid, that
    * is, just after the window holding this interface has been created.
    *
    * However, the Initialize() function is now deprecated and the standard way
    * to initialize an interface is by reimplementing its %Launch() function to
    * perform the initialization work <em>the first time the interface is
    * launched</em>. This technique is known as <em>deferred initialization</em>
    * of a process interface, and has the important advantage that it minimizes
    * resources and memory usage because an interface is only initialized if it
    * is ever used. Most interfaces are almost never launched by the user,
    * except on special occasions, so wasting resources for them continually is
    * very inefficient. All standard PixInsight interfaces use now the deferred
    * initialization technique.
    *
    * \note Developers using the deferred initialization technique must be very
    * careful to ensure that their interfaces will never try to access any
    * child controls and other interface elements prior to initializing them.
    *
    * \note The default implementation of this function sets \a dynamic = false
    * before returning true.
    *
    * \sa Initialize(), LaunchCount(), Launch( unsigned )
    */
   virtual bool Launch( const MetaProcess& process,
                        const ProcessImplementation* instance, bool& dynamic, unsigned& flags )
   {
      dynamic = false;  // interfaces are static by default
      return true;
   }

   /*!
    * Launches this interface with its default process.
    *
    * \param flags      Reserved for future extensions - must be zero.
    *
    * Returns true if the interface could be launched successfully; false in
    * the event of error.
    *
    * This is a convenience function to launch an interface without a process
    * instance. It is functionally equivalent to the following code:
    *
    * \code
    * bool Launch( ProcessInterface& iface, unsigned flags = 0 )
    * {
    *    const MetaProcess* process = iface.Process();
    *    if ( process == 0 )
    *       return false;
    *    bool dum;
    *    return iface.Launch( *process, 0, dum, flags );
    * }
    * \endcode
    *
    * \sa Launch( const MetaProcess&, const ProcessImplementation*, bool&, unsigned& ),
    */
   bool Launch( unsigned flags = 0 );

   /*!
    * Returns the number of times this interface has been launched since the
    * beginning of the current PixInsight session.
    *
    * \sa Launch()
    */
   size_type LaunchCount() const
   {
      return launchCount;
   }

   /*!
    * %Process instantiation function.
    *
    * Interfaces capable of generating process instances must reimplement this
    * function to create a new process instance, whose address is returned.
    *
    * The newly created instance should reflect the current state of this
    * interface. Interfaces usually maintain a private process instance, whose
    * parameters are modifiable through the interface's controls. Then a
    * reimplementation of this function simply creates a duplicate of the
    * private instance and returns its address.
    *
    * Returning \e arbitrary instances that have no logical connection with
    * the \e visible state of this interface is considered bad practice, and is
    * a main reason to deny certification of a PixInsight module. This is
    * because the user has always the right to know exactly what she or he can
    * expect to get, based on the information shown by an interface on its
    * controls.
    *
    * \note If an interface does not reimplement this function, then it will be
    * a plain tool with no processing capabilities. This is because the default
    * implementation of this function returns zero, meaning that no process
    * instance can be generated. In this case, it is \e mandatory that the
    * interface reimplements the IsInstanceGenerator() member function to
    * return false.
    *
    * \sa IsInstanceGenerator(), NewTestProcess(), MetaProcess::Clone()
    */
   virtual ProcessImplementation* NewProcess() const
   {
      return 0;
   }

   /*!
    * Test instantiation function.
    *
    * This function is invoked when a new instance is necessary for strict
    * testing purposes exclusively, not to perform actual processing.
    *
    * In certain situations, the core application needs a \e temporary process
    * instance exclusively for testing purposes. This happens, for example, to
    * verify if a given instance can be executed on a given view, or in the
    * global context, during complex GUI operations. The core application tries
    * to optimize GUI performance by calling this function when possible.
    *
    * For example, the PixInsight core application tries to create temporary
    * test instances each time the user drags a process icon over an image, to
    * learn if the instance that the icon encapsulates can be executed on the
    * view that holds the image. If the user drops the icon, then a regular,
    * non-test instance is generated by calling NewProcess(), then it is
    * executed, if possible. If the checks carried out by the dragged test
    * instances take significant time, PixInsight's user interface can suffer
    * from a severe performace degradation.
    *
    * If your process can provide a substantially simplified version of an
    * existing instance, and such a simplified version still is able to know if
    * it can be executed globally or on a given view, then reimplement this
    * function.
    *
    * If your process requires (or may require) some megabytes of data to work,
    * but these data are not needed for testing purposes, then it is a pretty
    * good candidate to reimplement this function. In practice, however,
    * requiring a reimplementation of this function is quite infrequent.
    *
    * \note The default implementation of this function just returns
    * NewProcess().
    *
    * \sa DistinguishesTestInstances(), NewProcess(), MetaProcess::TestClone()
    */
   virtual ProcessImplementation* NewTestProcess() const
   {
      return NewProcess();
   }

   /*!
    * Returns true if this interface can generate new process instances.
    *
    * If your interface has no actual processing capabilities, then reimplement
    * this function to return false. This helps the PixInsight core application
    * to save resources and improve GUI performance.
    *
    * An interface has no processing capabilities if its NewProcess() member
    * function returns zero. This happens if NewProcess() is not reimplemented.
    * Although quite infrequent, an interface can be an instance generator
    * \e sometimes, as a function of process parameters or other circumstances.
    * In this case both NewProcess() and IsInstanceGenerator() must be
    * reimplemented to provide coherent values.
    *
    * \note The default implementation of this function returns true, so all
    * interfaces are instance generators by default.
    *
    * \sa NewProcess()
    */
   virtual bool IsInstanceGenerator() const
   {
      return true;
   }

   /*!
    * Returns true if this interface can provide simplified versions of process
    * instances for strict testing purposes.
    *
    * For example, the PixInsight core application tries to create \e temporary
    * test instances each time the user drags a process icon over an image, to
    * learn if the instance that the icon encapsulates can be executed on the
    * view that holds the image. If the user drops the icon, then a regular,
    * non-test instance is generated and executed, if possible. If the checks
    * carried out by the dragged test instances take significant time,
    * PixInsight's user interface can suffer from a performace degradation.
    *
    * If you can provide substantially simplified instances, but still able to
    * know if they can be used in the context of a particular view, then
    * reimplement this function to return true, and also NewTestProcess() to
    * return simplified test instances.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't distinguish between normal (executable) and test
    * instances by default.
    *
    * \sa NewTestProcess()
    */
   virtual bool DistinguishesTestInstances() const
   {
      return false;
   }

   /*!
    * Instance validation function.
    *
    * \param instance   Reference to a process instance that is being validated
    *             before being imported by this interface.
    *
    * \param[out] whyNot   If this function returns false, it should return
    *             also a brief text (256 characters maximum) in this string,
    *             explaining why the specified \a instance is not a valid
    *             instance to be imported by this interface.
    *
    * This function will not be called unless the RequiresInstanceValidation()
    * member function is reimplemented to return true in a derived class.
    *
    * Instance validation takes place when an instance is about to be imported
    * by this interface. Importing a process instance means that its parameters
    * and data will be acquired by this interface, to edit them through the
    * available controls and GUI resources.
    *
    * \note The default implementation of this function returns false. This is
    * because it should be reimplemented if RequiresInstanceValidation() has
    * already been reimplemented to return true.
    *
    * \sa RequiresInstanceValidation(), ImportProcess()
    */
   virtual bool ValidateProcess( const ProcessImplementation& instance, String& whyNot ) const
   {
      whyNot.Empty();
      return false;  // Force a reimplementation if RequiresInstanceValidation() = true
   }

   /*!
    * Returns true if this interface requires validation of a process instance
    * before acquisition via ImportProcess().
    *
    * If you actually need this kind of validation, reimplement this function
    * to return true, and also the ValidateProcess() function to perform the
    * instance validation.
    *
    * \note The default implementation of this function returns false, so
    * instances are not validated before acquiring them with ImportProcess().
    *
    * \sa ValidateProcess(), ImportProcess()
    */
   virtual bool RequiresInstanceValidation() const
   {
      return false;
   }

   /*!
    * Instance import function.
    *
    * This function is called when a process instance is being acquired by this
    * interface. For example, this happens when the user drags a process icon
    * to the control bar panel of this interface, or when an instance has been
    * launched and it has selected this interface from a reimplementation of
    * ProcessImplementation::SelectInterface().
    *
    * When an instance is imported by an interface, instance parameters and
    * data items are usually acquired by the interface to edit them through its
    * available controls and GUI resources.
    *
    * This function returns true if the specified instance is accepted and
    * successfully imported, false otherwise.
    *
    * This function is never called if CanImportInstances() has been
    * reimplemented to return false.
    *
    * Note that ValidateProcess() has been called just before this function,
    * with a reference to the same instance, only if a reimplementation of
    * RequiresInstanceValidation() returns true. In this case, this function is
    * not called if ValidateProcess() does not return true.
    *
    * You must reimplement this function if your interface has processing
    * capabilities. Only plain tools don't need to reimplement this function,
    * since they know nothing about process instances.
    *
    * \note The default implementation of this function returns false, so by
    * default an interface cannot import instances.
    *
    * \sa ValidateProcess(), CanImportInstances(),
    * ProcessImplementation::SelectInterface()
    */
   virtual bool ImportProcess( const ProcessImplementation& )
   {
      return false;
   }

   /*!
    * Returns true if this interface is able to import process instances.
    *
    * If your interface has no processing capabilities, you should
    * reimplement this function to return false.
    *
    * \note The default implementation of this function returns true.
    *
    * \sa ImportProcess()
    */
   virtual bool CanImportInstances() const
   {
      return true;
   }

   /*!
    * Function called when the real-time preview image needs an update, and
    * this interface is the owner of the Real-Time Preview system.
    *
    * \param[in,out] image    Reference to a shared image where the real-time
    *             rendition would be generated. This image contains the pixel
    *             data of a view selected for real-time previewing. A
    *             subsequent call to GenerateRealTimePreview() would receive
    *             a reference to an image with the same pixel data, for
    *             effective real-time preview generation.
    *
    * \param view       Reference to a view that is currently selected in the
    *             Real-Time Preview system of the PixInsight core application.
    *             The passed \a image contains a representation of the image in
    *             this view.
    *
    * \param zoomLevel  Indicates the integer zoom ratio that has been applied
    *             to the specified \a image, with respect to the original
    *             view's image. If this parameter is a negative number, it
    *             represents a reduction factor; for example, -2 corresponds to
    *             a 1:2 reduction ratio, meaning that each pixel of \a image
    *             corresponds to 4 source pixels in the view being previewed.
    *             If this parameter is a positive number, it can only be equal
    *             to 1, indicating that each pixel in \a image corresponds to
    *             one source pixel. In other words, real-time preview images
    *             can be scaled down, but are never magnified or scaled up.
    *
    * Returns true if the real-time preview requires an update for the
    * specified image; false if no update is required.
    *
    * If this function returns true, a subsequent call to
    * GenerateRealTimePreview() will occur when appropriate.
    *
    * Both the passed \a image and \a view <em>cannot be modified</em> in any
    * way by this function.
    *
    * \note The default implementation of this function returns false.
    *
    * \sa GenerateRealTimePreview(), CancelRealTimePreview(), RealTimePreview
    */
   virtual bool RequiresRealTimePreviewUpdate( const UInt16Image& image, const View& view, int zoomLevel ) const
   {
      return false;
   }

   /*!
    * Function called to generate a new real-time preview rendition.
    *
    * \param[in,out] image    Reference to a shared image where the real-time
    *             rendition must be generated. On input, this image contains
    *             the pixel data of a view selected for real-time previewing.
    *             On output, this image must be transformed to represent a
    *             preview of the current process instance being edited on this
    *             process interface.
    *
    * \param view       Reference to a view that is currently selected in the
    *             Real-Time Preview system of the PixInsight core application.
    *             The passed \a image contains a representation of the image in
    *             this view.
    *
    * \param zoomLevel  Indicates the integer zoom ratio that has been applied
    *             to the specified \a image, with respect to the original
    *             view's image. If this parameter is a negative number, it
    *             represents a reduction factor; for example, -2 corresponds to
    *             a 1:2 reduction ratio, meaning that each pixel of \a image
    *             corresponds to 4 source pixels in the view being previewed.
    *             If this parameter is a positive number, it can only be equal
    *             to 1, indicating that each pixel in \a image corresponds to
    *             one source pixel. In other words, real-time preview images
    *             can be scaled down, but are never magnified or scaled up.
    *
    * \param[out] info  Reference to a string where a brief informative text
    *             (256 characters maximum) can optionally be stored to describe
    *             some special circumstances about the generated rendition.
    *             Typically, this string can be used to inform about an inexact
    *             or partial rendition, for example because the applied
    *             reduction ratio (\a zoomLevel) doesn't allow a reasonably
    *             accurate representation of some process features.
    *
    * Returns true to signal that the passed \a image has been modified and can
    * now be used to update the current real-time preview. Returns false to
    * indicate that the \a image has not been altered by this function, and
    * hence that the current real-time preview update should be aborted.
    *
    * The passed \a view <em>cannot be modified</em> in any way by this
    * function. This means that a reimplementation of this function <em>cannot
    * be used to modify a view indirectly</em> - we mean it!
    *
    * \note The default implementation of this function returns false without
    * modifying the passed \a image.
    *
    * \sa RequiresRealTimePreviewUpdate(), CancelRealTimePreview(),
    * RealTimePreview
    */
   virtual bool GenerateRealTimePreview( UInt16Image& image, const View& view, int zoomLevel, String& info ) const
   {
      return false;
   }

   /*!
    * Function called to request cancellation of the ongoing real-time preview
    * generation process.
    *
    * The core application may call this member function when the current
    * real-time generation process (which is being carried out by a
    * reimplemented GenerateRealTimePreview() function) should be aborted
    * immediately and unconditionally.
    *
    * The core application can make such a request at its own discretion---and
    * your interface should be ready to honor it during a real-time generation
    * task---, but currently (as of PixInsight Core version 1.8.0) this only
    * happens if the real-time progress dialog has been made visible and the
    * user has closed it prematurely (e.g., by clicking its Cancel button).
    *
    * \note The default implementation of this function simulates a manual
    * deactivation of the Real-Time Preview button of this interface by calling
    * Control::DeactivateRealTimePreview(). If your real-time generation code
    * is coherent with the current state of your interface, you normally should
    * not need to reimplement this member function.
    *
    * \sa RequiresRealTimePreviewUpdate(), GenerateRealTimePreview(),
    * RealTimePreview::ShowProgressDialog(), RealTimePreview
    */
   virtual void CancelRealTimePreview()
   {
      DeactivateRealTimePreview();
   }

   /*!
    * Returns true if this interface can be a <em>dynamic interface</em>.
    *
    * A dynamic interface is a process interface working in <em>dynamic
    * mode</em>. In dynamic mode, an interface provides a high degree of
    * interaction with the user through a set of special functions that allow
    * direct, real-time communication with the core application's GUI. This
    * includes the possibility of generating arbitrary graphical contents on
    * image views, and receiving pointing device (mouse, tablet) messages.
    *
    * Note that an interface can work in both static and dynamic modes (not
    * simultaneously, of course), depending on the value returned by the
    * Launch() function. However, dynamic interface semantics will not be
    * available to an interface unless it reimplements this function to return
    * true. This has been done in this way to help saving GUI resources, which
    * is a PCL design principle.
    *
    * If this function returns false (as it does by default), dynamic process
    * semantics will not be available for this interface.
    *
    * \note The default implementation of this function returns false, so
    * dynamic interface semantics are disabled by default.
    *
    * \sa Launch()
    */
   virtual bool IsDynamicInterface() const
   {
      return false;
   }

   /*!
    * Function called when a new <em>dynamic mode session</em> is about to
    * start. Returns true if the new dynamic session can continue; false if it
    * should be interrupted.
    *
    * \note The default implementation of this function returns true.
    *
    * \sa ExitDynamicMode()
    */
   virtual bool EnterDynamicMode()
   {
      return true;
   }

   /*!
    * Function called just after a dynamic session has been terminated.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa EnterDynamicMode()
    */
   virtual void ExitDynamicMode()
   {
   }

   /*!
    * Function called when the mouse cursor enters the viewport of a view \a v,
    * during an active dynamic session.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa DynamicMouseLeave(), DynamicMouseMove(), DynamicMousePress(),
    * DynamicMouseRelease(), DynamicMouseDoubleClick(), DynamicMouseWheel()
    */
   virtual void DynamicMouseEnter( View& v )
   {
   }

   /*!
    * Function called when the mouse cursor leaves the viewport of a view \a v,
    * during an active dynamic session.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa DynamicMouseEnter(), DynamicMouseMove(), DynamicMousePress(),
    * DynamicMouseRelease(), DynamicMouseDoubleClick(), DynamicMouseWheel()
    */
   virtual void DynamicMouseLeave( View& )
   {
   }

   /*!
    * Function called when the mouse cursor is displaced on a view, during an
    * active dynamic session.
    *
    * \param v    Reference to a view over which the mouse cursor has been
    *             displaced.
    *
    * \param p    New mouse cursor position in image coordinates.
    *
    * \param buttons    Indicates the current states of all mouse buttons. This
    *             value is an OR'ed combination of MouseButton flags. A flag
    *             set indicates a pressed mouse button.
    *
    * \param modifiers  Indicates the current states of all keyboard modifiers.
    *             This value is an OR'ed combination of KeyModifier flags. A
    *             flag set indicates a pressed modifier key.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa DynamicMouseEnter(), DynamicMouseLeave(), DynamicMousePress(),
    * DynamicMouseRelease(), DynamicMouseDoubleClick(), DynamicMouseWheel()
    */
   virtual void DynamicMouseMove( View& v, const DPoint& p,
                                       unsigned buttons, unsigned modifiers )
   {
   }

   /*!
    * Function called when a mouse button is pressed on a view, during an
    * active dynamic session.
    *
    * \param v    Reference to a view over which a mouse button has been
    *             pressed.
    *
    * \param p    Current mouse cursor position in image coordinates.
    *
    * \param button     Identifies the mouse button that has been pressed. This
    *             value is a unique MouseButton flag.
    *
    * \param buttons    Indicates the current states of all mouse buttons. This
    *             value is an OR'ed combination of MouseButton flags. A flag
    *             set indicates a pressed mouse button.
    *
    * \param modifiers  Indicates the current states of all keyboard modifiers.
    *             This value is an OR'ed combination of KeyModifier flags. A
    *             flag set indicates a pressed modifier key.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa DynamicMouseEnter(), DynamicMouseLeave(), DynamicMouseMove(),
    * DynamicMouseRelease(), DynamicMouseDoubleClick(), DynamicMouseWheel()
    */
   virtual void DynamicMousePress( View& v, const DPoint& p,
                           int button, unsigned buttons, unsigned modifiers )
   {
   }

   /*!
    * Function called when a mouse button is released on a view, during an
    * active dynamic session.
    *
    * \param v    Reference to a view over which a mouse button has been
    *             released.
    *
    * \param p    Current mouse cursor position in image coordinates.
    *
    * \param button     Identifies the mouse button that has been released.
    *             This value is a unique MouseButton flag.
    *
    * \param buttons    Indicates the current states of all mouse buttons. This
    *             value is an OR'ed combination of MouseButton flags. A flag
    *             set indicates a pressed mouse button.
    *
    * \param modifiers  Indicates the current states of all keyboard modifiers.
    *             This value is an OR'ed combination of KeyModifier flags. A
    *             flag set indicates a pressed modifier key.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa DynamicMouseEnter(), DynamicMouseLeave(), DynamicMouseMove(),
    * DynamicMousePress(), DynamicMouseDoubleClick(), DynamicMouseWheel()
    */
   virtual void DynamicMouseRelease( View& v, const DPoint& p,
                           int button, unsigned buttons, unsigned modifiers )
   {
   }

   /*!
    * Function called when the primary mouse button is double-clicked on a
    * view, during an active dynamic session.
    *
    * \param v    Reference to a view over which the primary mouse button has
    *             been double-clicked.
    *
    * \param p    Current mouse cursor position in image coordinates.
    *
    * \param buttons    Indicates the current states of all mouse buttons. This
    *             value is an OR'ed combination of MouseButton flags. A flag
    *             set indicates a pressed mouse button.
    *
    * \param modifiers  Indicates the current states of all keyboard modifiers.
    *             This value is an OR'ed combination of KeyModifier flags. A
    *             flag set indicates a pressed modifier key.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa DynamicMouseEnter(), DynamicMouseLeave(), DynamicMouseMove(),
    * DynamicMousePress(), DynamicMouseRelease(), DynamicMouseWheel()
    */
   virtual void DynamicMouseDoubleClick( View& v, const DPoint& p,
                                       unsigned buttons, unsigned modifiers )
   {
   }

   /*!
    * Function called if a key is pressed when a view has the keyboard focus,
    * during an active dynamic session.
    *
    * \param v    Reference to the view that had the keyboard focus when a key
    *             was pressed.
    *
    * \param key  A KeyCode code identifying the keyboard key that has been
    *             pressed.
    *
    * \param modifiers  Indicates the current states of all keyboard modifiers.
    *             This value is an OR'ed combination of KeyModifier flags. A
    *             flag set indicates a pressed modifier key.
    *
    * This function returns true if it processes the keyboard press event. If
    * false is returned, then the keyboard event may propagate to other
    * elements in the PixInsight core application GUI.
    *
    * \note The default implementation of this function returns false.
    *
    * \sa DynamicKeyRelease()
    */
   virtual bool DynamicKeyPress( View& v, int key, unsigned modifiers )
   {
      return false;
   }

   /*!
    * Function called if a key is released when a view has the keyboard focus,
    * during an active dynamic session.
    *
    * \param v    Reference to the view that had the keyboard focus when a key
    *             was released.
    *
    * \param key  A KeyCode code identifying the keyboard key that has been
    *             released.
    *
    * \param modifiers  Indicates the current states of all keyboard modifiers.
    *             This value is an OR'ed combination of KeyModifier flags. A
    *             flag set indicates a pressed modifier key.
    *
    * This function returns true if it processes the keyboard release event. If
    * false is returned, then the keyboard event may propagate to other
    * elements in the PixInsight core application GUI.
    *
    * \note The default implementation of this function returns false.
    *
    * \sa DynamicKeyPress()
    */
   virtual bool DynamicKeyRelease( View& v, int key, unsigned modifiers )
   {
      return false;
   }

   /*!
    * Function called when the mouse wheel is rotated while the mouse cursor is
    * over a view, during an active dynamic session.
    *
    * \param v    Reference to a view over which the mouse cursor was located
    *             when the mouse wheel was rotated.
    *
    * \param p    Current mouse cursor position in image coordinates.
    *
    * \param wheelDelta Wheel rotation increment. A positive increment
    *             indicates that the wheel has been rotated in the forward
    *             direction; negative increments correspond to backwards
    *             rotation. For example, negative increments should be used to
    *             scroll down a scrollable area, as a document browser.
    *
    * \param buttons    Indicates the current states of all mouse buttons. This
    *             value is an OR'ed combination of MouseButton flags. A flag
    *             set indicates a pressed mouse button.
    *
    * \param modifiers  Indicates the current states of all keyboard modifiers.
    *             This value is an OR'ed combination of KeyModifier flags. A
    *             flag set indicates a pressed modifier key.
    *
    * This function returns true if it processes the wheel rotation event. If
    * false is returned, then the wheel event will propagate to other elements
    * in the PixInsight core application GUI.
    *
    * \note The default implementation of this function returns false.
    *
    * \sa DynamicMouseEnter(), DynamicMouseLeave(), DynamicMouseMove(),
    * DynamicMousePress(), DynamicMouseRelease(), DynamicMouseDoubleClick()
    */
   virtual bool DynamicMouseWheel( View& v, const DPoint& p,
                        int wheelDelta, unsigned buttons, unsigned modifiers )
   {
      return false;
   }

   /*!
    * Function called when a view's screen rendition needs to be updated,
    * during an active dynamic session.
    *
    * \param v    Reference to a view that requires a screen update.
    *
    * \param r    Update region in image coordinates.
    *
    * This function \e must be reimplemented by dynamic interfaces that
    * maintain their own graphical content over image views.
    *
    * This function must return false if this interface requires no screen
    * update for the specified update region.
    *
    * If this function returns true, then a subsequent call to DynamicPaint()
    * will occur as soon as possible, including this update region, probably
    * into a wider region, due to optimization of screen updates.
    *
    * \note The default implementation of this function returns false.
    *
    * \sa DynamicPaint()
    */
   virtual bool RequiresDynamicUpdate( const View& v, const DRect& r ) const
   {
      return false;
   }

   /*!
    * Function called when a view's screen rendition has been updated.
    *
    * \param v    Reference to a view where the specified \a r update region
    *             should be redrawn by this interface.
    *
    * \param g    Reference to a graphics context where all screen drawing work
    *             must be performed.
    *
    * \param r    Update region in image coordinates.
    *
    * This function \e must be reimplemented by dynamic interfaces that
    * maintain their own graphical content over image views.
    *
    * When this function is invoked, the update region \a r on the passed view
    * \a v will contain just the screen rendition of the view's image, without
    * additional vectorial contents like preview rectangles, auxiliary
    * geometric items, cursors, etc, which are rendered after this function
    * returns.
    *
    * \note The default implementation of this function does nothing.
    *
    * \sa RequiresDynamicUpdate(), Graphics
    */
   virtual void DynamicPaint( const View& v, Graphics& g, const DRect& r ) const
   {
   }

   /*!
    * \defgroup image_notifications Image Notification Functions
    *
    * The PixInsight core application calls image notification functions to
    * keep interfaces informed about changes and events involving views and
    * their images.
    *
    * For a process interface to receive image notifications, the corresponding
    * ProcessInterface subclass must reimplement
    * ProcessInterface::WantsImageNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive image notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive image notifications by default.
    *
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual bool WantsImageNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when a new view has been created.
    *
    * \param v    Reference to the view that has been created. Can be either a
    *             main view or a preview.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageCreated( const View& v )
   {
   }

   /*!
    * Notification sent when the image in a view has been modified.
    *
    * \param v    Reference to the view whose image has been modified. Can be
    *             either a main view or a preview.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageUpdated( const View& v )
   {
   }

   /*!
    * Notification sent when the identifier of a view has been changed.
    *
    * \param v    Reference to the view whose identifier has been changed. Can
    *             be either a main view or a preview.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageRenamed( const View& v )
   {
   }

   /*!
    * Notification sent when an existing view is about to be destroyed.
    *
    * \param v    Reference to the view that will be destroyed. Can be either a
    *             main view or a preview.
    *
    * When an image window is closed and it has one or more previews defined, a
    * ImageDeleted() notification is sent for each of its previews, then a
    * final notification is sent for its main view.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageDeleted( const View& v )
   {
   }

   /*!
    * Notification sent when a view has been activated and has received the
    * input focus.
    *
    * \param v    Reference to the view that has been activated and focused.
    *             This is from now on the current <em>active view</em> in the
    *             core application's GUI. \a v can be either a main view or a
    *             preview.
    *
    * If the \a v view is a preview, this notification implicitly means that
    * its parent image window has been activated and is now the topmost, active
    * window in the GUI workspace. In this case, however, a notification for
    * the window's main view is not sent; only the active view is notified.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageFocused( const View& v )
   {
   }

   /*!
    * Notification sent when a view has been locked for read and/or write
    * operations.
    *
    * \param v    Reference to the view that has been locked.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageLocked( const View& v )
   {
   }

   /*!
    * Notification sent when a view has been unlocked for read and/or write
    * operations.
    *
    * \param v    Reference to the view that has been unlocked.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageUnlocked( const View& v )
   {
   }

   /*!
    * Notification sent when the <em>screen transfer functions</em> (STF) have
    * been enabled for a view.
    *
    * \param v    Reference to a view whose STF have been enabled.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageSTFEnabled( const View& v )
   {
   }

   /*!
    * Notification sent when the <em>screen transfer functions</em> (STF) have
    * been disabled for a view.
    *
    * \param v    Reference to a view whose STF have been disabled.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageSTFDisabled( const View& v )
   {
   }

   /*!
    * Notification sent when the <em>screen transfer functions</em> (STF) of a
    * view have been updated.
    *
    * \param v    Reference to a view whose STF have been updated.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageSTFUpdated( const View& v )
   {
   }

   /*!
    * Notification sent when the parameters of the <em>RGB working space</em>
    * (RGBWS) of a view have been updated.
    *
    * \param v    Reference to a view whose RGBWS has been updated.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageRGBWSUpdated( const View& v )
   {
   }

   /*!
    * Notification sent when <em>color management</em> (CM) has been enabled
    * for a view.
    *
    * \param v    Reference to a view for which color management has been
    *             enabled.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageCMEnabled( const View& v )
   {
   }

   /*!
    * Notification sent when <em>color management</em> (CM) has been disabled
    * for a view.
    *
    * \param v    Reference to a view for which color management has been
    *             disabled.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageCMDisabled( const View& v )
   {
   }

   /*!
    * Notification sent when the <em>color management</em> (CM) data of a view
    * have been updated.
    *
    * \param v    Reference to a view whose color management data have been
    *             changed.
    *
    * This notification is sent when a view's ICC profile is changed or
    * deleted, and when some color management features are modified, as
    * soft-proofing or gamut check.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageCMUpdated( const View& v )
   {
   }

   /*!
    * Notification sent when an image has been saved to a disk file.
    *
    * \param v    Reference to a view whose image has been written to a file.
    *
    * \ingroup image_notifications
    * \sa \ref image_notifications "Image Notification Functions"
    */
   virtual void ImageSaved( const View& v )
   {
   }

   /*!
    * \defgroup mask_notifications Mask Notification Functions
    *
    * The PixInsight core application calls mask notification functions to
    * keep interfaces informed about changes and events involving masks of
    * image windows.
    *
    * For a process interface to receive mask notifications, the corresponding
    * ProcessInterface subclass must reimplement
    * ProcessInterface::WantsMaskNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive mask notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive mask notifications by default.
    *
    * \sa \ref mask_notifications "Mask Notification Functions"
    */
   virtual bool WantsMaskNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when a mask has been selected for (or removed from) an
    * image window.
    *
    * \param v    Reference to the main view of an image window for which
    *             either a new mask has been selected, or an existing mask has
    *             been removed.
    *
    * \ingroup mask_notifications
    * \sa \ref mask_notifications "Mask Notification Functions"
    */
   virtual void MaskUpdated( const View& v )
   {
   }

   /*!
    * Notification sent when masking has been enabled for an image window.
    *
    * \param v    Reference to the main view of an image window for which
    *             masking has been enabled.
    *
    * \ingroup mask_notifications
    * \sa \ref mask_notifications "Mask Notification Functions"
    */
   virtual void MaskEnabled( const View& v )
   {
   }

   /*!
    * Notification sent when masking has been disabled for an image window.
    *
    * \param v    Reference to the main view of an image window for which
    *             masking has been disabled.
    *
    * \ingroup mask_notifications
    * \sa \ref mask_notifications "Mask Notification Functions"
    */
   virtual void MaskDisabled( const View& v )
   {
   }

   /*!
    * Notification sent when mask visibility has been enabled for an image
    * window.
    *
    * \param v    Reference to the main view of an image window whose mask is
    *             now shown.
    *
    * \ingroup mask_notifications
    * \sa \ref mask_notifications "Mask Notification Functions"
    */
   virtual void MaskShown( const View& v )
   {
   }

   /*!
    * Notification sent when mask visibility has been disabled for an image
    * window.
    *
    * \param v    Reference to the main view of an image window whose mask is
    *             now hidden.
    *
    * \ingroup mask_notifications
    * \sa \ref mask_notifications "Mask Notification Functions"
    */
   virtual void MaskHidden( const View& v )
   {
   }

   /*!
    * \defgroup transparency_notifications Image Transparency Notification
    *                                      Functions
    *
    * The PixInsight core application calls transparency notification functions
    * to keep interfaces informed about changes in transparency rendering modes
    * of image windows.
    *
    * For a process interface to receive transparency notifications, the
    * corresponding ProcessInterface subclass must reimplement
    * ProcessInterface::WantsTransparencyNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive transparency notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive transparency notifications by default.
    *
    * \sa \ref transparency_notifications
    *          "Image Transparency Notification Functions"
    */
   virtual bool WantsTransparencyNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when transparency rendering has been disabled for an
    * image window.
    *
    * \param v    Reference to the main view of an image window for which
    *             transparency rendering has been disabled.
    *
    * \ingroup transparency_notifications
    * \sa \ref transparency_notifications
    *          "Image Transparency Notification Functions"
    */
   virtual void TransparencyHidden( const View& v )
   {
   }

   /*!
    * Notification sent when the current transparency rendering mode has been
    * updated for an image window.
    *
    * \param v    Reference to the main view of an image window whose
    *             transparency rendering mode has been updated.
    *
    * \ingroup transparency_notifications
    * \sa \ref transparency_notifications
    *          "Image Transparency Notification Functions"
    */
   virtual void TransparencyModeUpdated( const View& v )
   {
   }

   /*!
    * \defgroup hist_notifications Histogram and Statistics Notification
    *                              Functions
    *
    * The PixInsight core application calls histogram and statistics
    * notification functions to keep interfaces informed about changes and
    * events involving shared histogram and statistical data of views.
    *
    * For a process interface to receive histogram and statistics
    * notifications, the corresponding ProcessInterface subclass must
    * reimplement ProcessInterface::WantsHistogramNotifications() and
    * ProcessInterface::WantsStatisticsNotifications(), respectively, to
    * return true.
    */

   /*!
    * Returns true if this interface will receive histogram notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive histogram notifications by default.
    *
    * \sa \ref hist_notifications "Histogram and Statistics Notification Functions"
    */
   virtual bool WantsHistogramNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when the histogram functions of a view have been
    * calculated.
    *
    * \param v    Reference to a view whose histograms have been calculated and
    *             are now available.
    *
    * \ingroup hist_notifications
    * \sa \ref hist_notifications "Histogram and Statistics Notification Functions"
    */
   virtual void HistogramsUpdated( const View& v )
   {
   }

   /*!
    * Notification sent when the histogram functions of a view have been
    * destroyed.
    *
    * \param v    Reference to a view whose histograms have been destroyed and
    *             are no longer available.
    *
    * \ingroup hist_notifications
    * \sa \ref hist_notifications "Histogram and Statistics Notification Functions"
    */
   virtual void HistogramsDestroyed( const View& v )
   {
   }

   /*!
    * Returns true if this interface will receive statistics notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive statistics notifications by default.
    *
    * \sa \ref hist_notifications "Histogram and Statistics Notification Functions"
    */
   virtual bool WantsStatisticsNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when the statistical data of a view have been
    * calculated.
    *
    * \param v    Reference to a view whose statistics have been calculated and
    *             are now available.
    *
    * \ingroup hist_notifications
    * \sa \ref hist_notifications "Histogram and Statistics Notification Functions"
    */
   virtual void StatisticsUpdated( const View& v )
   {
   }

   /*!
    * Notification sent when the statistical data of a view have been
    * destroyed.
    *
    * \param v    Reference to a view whose statistics have been destroyed and
    *             are no longer available.
    *
    * \ingroup hist_notifications
    * \sa \ref hist_notifications "Histogram and Statistics Notification Functions"
    */
   virtual void StatisticsDestroyed( const View& v )
   {
   }

   /*!
    * \defgroup view_property_notifications View Property Notification Functions
    *
    * The PixInsight core application calls view property notification
    * functions to keep interfaces informed about changes and events involving
    * view properties.
    *
    * For a process interface to receive view property notifications, the
    * corresponding ProcessInterface subclass must reimplement
    * ProcessInterface::WantsViewPropertyNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive view property notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive view property notifications by default.
    *
    * \sa \ref view_property_notifications "View Property Notification Functions"
    */
   virtual bool WantsViewPropertyNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when a view property has been created or modified.
    *
    * \param v    Reference to a view where a new property has been created or
    *             an existing property has been modified.
    *
    * \param property   Identifier of the view property that has been created
    *                   or modified.
    *
    * \ingroup view_property_notifications
    * \sa \ref view_property_notifications "View Property Notification Functions"
    */
   virtual void ViewPropertyUpdated( const View& v, const IsoString& property )
   {
   }

   /*!
    * Notification sent when a view property has been deleted.
    *
    * \param v    Reference to a view where a property has been deleted.
    *
    * \param property   Identifier of the view property that has been deleted.
    *
    * \ingroup view_property_notifications
    * \sa \ref view_property_notifications "View Property Notification Functions"
    */
   virtual void ViewPropertyDeleted( const View& v, const IsoString& property )
   {
   }

   /*!
    * \defgroup readout_notifications Readout Notification Functions
    *
    * The PixInsight core application calls readout notification functions to
    * send real-time readout pixel values to interfaces.
    *
    * Readout operations take place when the user clicks and drags the mouse
    * over a view in <em>readout mode</em> (ImageMode::Readout). During a
    * readout procedure, RGBA values are read from a view's image (according to
    * global readout options) and sent to interfaces via readout notifications.
    *
    * For example, an interface can receive readout notifications to let the
    * user select a color by clicking directly over an image. The NewImage
    * standard interface does exactly this to select an initial color for newly
    * created images.
    *
    * For a process interface to receive readout notifications, the
    * corresponding ProcessInterface subclass must reimplement
    * ProcessInterface::WantsReadoutNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive readout notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive readout notifications by default.
    *
    * \sa \ref readout_notifications "Readout Notification Functions"
    */
   virtual bool WantsReadoutNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent to signal the beginning of a new readout procedure.
    *
    * \param v    Reference to a view for which new readout data notifications
    *             will be sent from now on.
    *
    * \ingroup readout_notifications
    * \sa \ref readout_notifications "Readout Notification Functions"
    */
   virtual void BeginReadout( const View& v )
   {
   }

   /*!
    * Notification sent when new readout data are available.
    *
    * \param v    Reference to a view for which new readout data is being sent.
    *
    * \param p    Readout position in image coordinates.
    *
    * \param R,G,B,A   New readout red, green, blue and alpha values,
    *             respectively, in the normalized real range [0,1].
    *
    * \note Although images can have an unlimited number of alpha channels in
    * PixInsight, only readout values from the first alpha channel (which
    * defines transparency) are sent to interfaces.
    *
    * \ingroup readout_notifications
    * \sa \ref readout_notifications "Readout Notification Functions"
    */
   virtual void UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double A )
   {
   }

   /*!
    * Notification sent to signal the end of an active readout procedure.
    *
    * \param v    Reference to the view for which new readout data
    *             notifications will no longer be sent.
    *
    * \ingroup readout_notifications
    * \sa \ref readout_notifications "Readout Notification Functions"
    */
   virtual void EndReadout( const View& v )
   {
   }

   /*!
    * \defgroup process_notifications %Process Instance Notification Functions
    *
    * The PixInsight core application calls process instance notification
    * functions to keep interfaces informed about changes and events involving
    * process instances.
    *
    * For a process interface to receive process instance notifications, the
    * corresponding ProcessInterface subclass must reimplement
    * ProcessInterface::WantsProcessNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive process instance
    * notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive instance notifications by default.
    *
    * \sa \ref process_notifications "Process Instance Notification Functions"
    */
   virtual bool WantsProcessNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when a new process instance has been created.
    *
    * \param p    Reference to a process instance that has been newly created.
    *
    * \ingroup process_notifications
    * \sa \ref process_notifications "Process Instance Notification Functions"
    */
   virtual void ProcessCreated( const ProcessImplementation& p )
   {
   }

   /*!
    * Notification sent when an existing process instance has been updated.
    *
    * \param p    Reference to a process instance that has been modified.
    *
    * \ingroup process_notifications
    * \sa \ref process_notifications "Process Instance Notification Functions"
    */
   virtual void ProcessUpdated( const ProcessImplementation& p )
   {
   }

   /*!
    * Notification sent when a process instance is about to be destroyed.
    *
    * \param p    Reference to a process instance that will be destroyed.
    *
    * \ingroup process_notifications
    * \sa \ref process_notifications "Process Instance Notification Functions"
    */
   virtual void ProcessDeleted( const ProcessImplementation& p )
   {
   }

   /*!
    * Notification sent when a process instance has been saved to a disk file.
    *
    * \param p    Reference to a process instance that has been written to a
    *             disk file.
    *
    * \ingroup process_notifications
    * \sa \ref process_notifications "Process Instance Notification Functions"
    */
   virtual void ProcessSaved( const ProcessImplementation& p )
   {
   }

   /*!
    * \defgroup rtpreview_notifications Real Time Preview Notification Functions
    *
    * The PixInsight core application calls real time preview notification
    * functions to keep interfaces informed about changes and events involving
    * the PixInsight real time previewing system.
    *
    * For a process interface to receive real time preview notifications, the
    * corresponding ProcessInterface subclass must reimplement
    * ProcessInterface::WantsRealTimePreviewNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive notifications from the
    * real-time previewing system.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive real-time previewing notifications by default.
    *
    * \sa \ref rtpreview_notifications "Real Time Preview Notification Functions"
    */
   virtual bool WantsRealTimePreviewNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when the owner of the real time previewing system has
    * been changed.
    *
    * \param iface   Reference to a process interface that is now the owner of
    *                the real-time previewing system.
    *
    * \ingroup rtpreview_notifications
    * \sa \ref rtpreview_notifications "Real Time Preview Notification Functions"
    */
   virtual void RealTimePreviewOwnerChanged( ProcessInterface& iface )
   {
   }

   /*!
    * Notification sent when a real time preview LUT structure has been
    * generated.
    *
    * \param colorModel    Identifies the LUT structure that has been
    *             recalculated and is now available.
    *
    * \ingroup rtpreview_notifications
    * \sa \ref rtpreview_notifications "Real Time Preview Notification Functions"
    *
    * \deprecated  This function has been deprecated and should not be used.
    * The real-time previewing system no longer depends on look up table (LUT)
    * structures.
    */
   virtual void RealTimePreviewLUTUpdated( int colorModel )
   {
   }

   /*!
    * Notification sent when a new real-time rendition has started being
    * generated.
    *
    * \ingroup rtpreview_notifications
    * \sa \ref rtpreview_notifications "Real Time Preview Notification Functions"
    */
   virtual void RealTimePreviewGenerationStarted()
   {
   }

   /*!
    * Notification sent when a new real-time rendition has finished being
    * generated.
    *
    * \ingroup rtpreview_notifications
    * \sa \ref rtpreview_notifications "Real Time Preview Notification Functions"
    */
   virtual void RealTimePreviewGenerationFinished()
   {
   }

   /*!
    * \defgroup global_notifications Global Notification Functions
    *
    * The PixInsight core application calls global notification functions to
    * keep interfaces informed about changes and events occurring in the global
    * core application context.
    *
    * For a process interface to receive global notifications, the
    * corresponding ProcessInterface subclass must reimplement
    * ProcessInterface::WantsGlobalNotifications() to return true.
    */

   /*!
    * Returns true if this interface will receive global notifications.
    *
    * \note The default implementation of this function returns false, so
    * interfaces don't receive global notifications by default.
    *
    * \sa \ref global_notifications "Global Notification Functions"
    */
   virtual bool WantsGlobalNotifications() const
   {
      return false;
   }

   /*!
    * Notification sent when the parameters of the global RGB working space
    * (RGBWS) have been modified.
    *
    * \ingroup global_notifications
    * \sa \ref global_notifications "Global Notification Functions"
    */
   virtual void GlobalRGBWSUpdated()
   {
   }

   /*!
    * Notification sent when color management (CM) has been globally enabled.
    *
    * \ingroup global_notifications
    * \sa \ref global_notifications "Global Notification Functions"
    */
   virtual void GlobalCMEnabled()
   {
   }

   /*!
    * Notification sent when color management (CM) has been globally disabled.
    *
    * \ingroup global_notifications
    * \sa \ref global_notifications "Global Notification Functions"
    */
   virtual void GlobalCMDisabled()
   {
   }

   /*!
    * Notification sent when global color management (CM) data have been
    * modified. This includes selecting a new default ICC profile, or changing
    * some global color management parameters, as default profile mismatching
    * policies or rendering intents.
    *
    * \ingroup global_notifications
    * \sa \ref global_notifications "Global Notification Functions"
    */
   virtual void GlobalCMUpdated()
   {
   }

   /*!
    * Notification sent when global readout options have been modified.
    *
    * \ingroup global_notifications
    * \sa \ref global_notifications "Global Notification Functions"
    */
   virtual void ReadoutOptionsUpdated()
   {
   }

   /*!
    * Notification sent when one or more global preferences settings have been
    * modified.
    *
    * \ingroup global_notifications
    * \sa \ref global_notifications "Global Notification Functions"
    */
   virtual void GlobalPreferencesUpdated()
   {
   }

   /*!
    * Returns a settings key that should be used for storage of all settings
    * data associated to this interface.
    *
    * The returned settings key string will be equal to "Interfaces/&lt;id&gt;/",
    * where &lt;id&gt; is the identifier of this interface, as returned by the
    * Id() member function.
    *
    * \note Interfaces must only use the keys returned by this function for
    * storage of all their settings data. Using different keys is allowed, but
    * it is considered bad practice in most cases.
    *
    * \sa Settings, SaveSettings(), LoadSettings()
    */
   IsoString SettingsKey() const
   {
      return IsoString( "Interfaces/" ) + Id() + '/';
   }

   /*!
    * Saves settings data for this process interface.
    *
    * This function is called automatically for all process interfaces upon
    * module unload. If your interface needs storing and retrieving
    * configuration data to remember them across sessions, then you should
    * reimplement this function.
    *
    * See the documentation for the LoadSettings() member function for detailed
    * information about saving and loading settings data for process
    * interfaces.
    *
    * \note This function is only invoked for interfaces that have been
    * launched at least once. This is because if an interface has not been used
    * during a session, there is no plausible reason to modify its associated
    * configuration data. This policy has been adopted to maximize user control
    * over interface preferences and configuration settings.
    *
    * \sa Settings, LoadSettings(), SettingsKey()
    */
   virtual void SaveSettings() const
   {
   }

   /*!
    * Retrieves stored settings data for this process interface.
    *
    * This function is called automatically for all process interfaces the
    * first time they are launched. If your interface needs storing and
    * retrieving configuration data to remember them across sessions, then you
    * should reimplement this function.
    *
    * When saving and loading settings data, your code must use the key
    * returned by SettingsKey() as a prefix for the subkeys corresponding to
    * each settings data item. For example, the following code snippet:
    *
    * \code
    * IsoString key = SettingsKey() + "Geometry/";
    * // ...
    * Settings::Write( key + "Width", Width() );
    * \endcode
    *
    * would save the current width of this interface to the
    * "Interfaces/&lt;id&gt;/Geometry/Width" subkey in the global settings
    * space assigned to the current module. This is the recommended behavior.
    *
    * \sa Settings, SaveSettings(), SettingsKey()
    */
   virtual void LoadSettings()
   {
   }

   /*!
    * Saves the current position and dimensions of this interface in global
    * settings variables.
    *
    * Call this function to save the geometry of an interface window across
    * sessions. The RestoreGeometry() member function can be invoked to restore
    * previously saved position and size data.
    *
    * Before calling this function, all interface child controls and interface
    * elements should have been completely initialized. If this interface is
    * not resizable (e.g. because the SetFixedSize() function has been called),
    * size data are not stored.
    *
    * This function stores geometry data in the following settings keys:
    *
    * Interfaces/&lt;id&gt;/Geometry/Left
    * Interfaces/&lt;id&gt;/Geometry/Top
    * Interfaces/&lt;id&gt;/Geometry/Width
    * Interfaces/&lt;id&gt;/Geometry/Height
    *
    * where &lt;id&gt; is the identifier of this interface, as returned by the
    * Id() member function.
    *
    * \note Normally, you should not need to call this function, unless you
    * have explicitly disabled the <em>auto save geometry</em> feature by
    * calling the DisableAutoSaveGeometry() member function.
    *
    * \sa RestoreGeometry(), SetDefaultPosition(), IsAutoSaveGeometryEnabled()
    */
   void SaveGeometry() const;

   /*!
    * Restores previously saved position and dimensions of this interface from
    * global settings variables. Returns true if stored geometry data could be
    * successfully retrieved and established for this interface.
    *
    * Call this function to restore the geometry of an interface window as it
    * was previously saved by SaveGeometry(). This allows an interface to
    * \e remember its size and position on the core application's work space
    * across sessions.
    *
    * \note Normally, you should not need to call this function, unless you
    * have explicitly disabled the <em>auto save geometry</em> feature by
    * calling the DisableAutoSaveGeometry() member function.
    *
    * \sa SaveGeometry()
    */
   bool RestoreGeometry();

   /*!
    * Moves this interface window to a default position on the PixInsight core
    * application's work space.
    *
    * The default position is calculated to place the interface window centered
    * on the primary monitor.
    *
    * \sa SaveGeometry(), RestoreGeometry()
    */
   void SetDefaultPosition();

   /*!
    * Returns true if the <em>auto save geometry</em> feature is currently
    * active for this interface.
    *
    * When <em>auto save geometry</em> is active, the interface \e remembers
    * its size and position on the PixInsight core application's worspace
    * across sessions. Geometry data are saved to and retrieved from global
    * settings variables automatically. Geometry is only saved for interfaces
    * that have been launched at least once.
    *
    * \note By default, the <em>auto save geometry</em> feature is active for
    * every process interface. To avoid this automatic behavior, it must be
    * explicitly disabled.
    *
    * \sa EnableAutoSaveGeometry(), DisableAutoSaveGeometry(),
    * SaveGeometry(), RestoreGeometry(), SetDefaultPosition()
    */
   bool IsAutoSaveGeometryEnabled() const
   {
      return autoSaveGeometry;
   }

   /*!
    * Enables (or disables) the <em>auto save geometry</em> feature. For more
    * information on this feature, see the documentation for the
    * IsAutoSaveGeometryEnabled() member function.
    *
    * \sa DisableAutoSaveGeometry(), IsAutoSaveGeometryEnabled()
    */
   void EnableAutoSaveGeometry( bool enable = true )
   {
      autoSaveGeometry = enable;
   }

   /*!
    * Disables (or enables) the <em>auto save geometry</em> feature. For more
    * information on this feature, see the documentation for the
    * IsAutoSaveGeometryEnabled() member function.
    *
    * \sa EnableAutoSaveGeometry(), IsAutoSaveGeometryEnabled()
    */
   void DisableAutoSaveGeometry( bool disable = true )
   {
      EnableAutoSaveGeometry( !disable );
   }

   /*!
    * Issues an %ImageUpdated notification for a view.
    *
    * \param v    The view for which an ImageUpdated notification will be sent.
    *
    * The PixInsight core application will send ImageUpdated() notifications to
    * all objects that listen image notifications on the entire PixInsight
    * platform. This includes not only other process interfaces (including the
    * emitter), but also a large number of interface objects and processing
    * resources whose states can be automatically updated in response to the
    * notification received.
    *
    * Broadcasting is typically used by dynamic interfaces that modify images
    * as a function of direct user interaction. A good example of this is the
    * CloneStamp interface, which broadcasts ImageUpdated notifications to
    * keep the whole platform consistent with image changes due to clone stamp
    * actions performed by the user.
    *
    * \note This member function must \e not be used on a regular basis without
    * having a good reason to do so. Excessive use of this member function may
    * degrade the performance of the whole platform due to overhead.
    */
   static void BroadcastImageUpdated( const View& v );

   /*!
    * Processes all pending GUI events.
    *
    * Call this function to let the PixInsight core application process pending
    * GUI events, which may accumulate while your code is running.
    *
    * Modules typically call this function during real-time preview generation
    * procedures. Calling this function ensures that the GUI remains responsive
    * during long, calculation-intensive operations.
    *
    * \note Do not call this function on a regular basis. Normally, you should
    * not need calling it except during real-time generation procedures.
    */
   static void ProcessEvents();

protected:

   ProcessInterface( int );

private:

   size_type launchCount;
   bool autoSaveGeometry;

   virtual void PerformAPIDefinitions() const;

   friend class InterfaceDispatcher;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_ProcessInterface_h

// ****************************************************************************
// EOF pcl/ProcessInterface.h - Released 2014/10/29 07:34:07 UTC
