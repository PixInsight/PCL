// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/GlobalSettings.h - Released 2014/11/14 17:16:40 UTC
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

#ifndef __PCL_GlobalSettings_h
#define __PCL_GlobalSettings_h

/// \file pcl/GlobalSettings.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_Color_h
#include <pcl/Color.h>
#endif

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
#ifndef __PCL_Font_h
#include <pcl/Font.h>
#endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace GlobalVariableType
 * \brief Data types for global platform variables
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>GlobalVariableType::Undefined</td>  <td>Indicates that the requested global variable does not exist</td></tr>
 * <tr><td>GlobalVariableType::Flag</td>       <td>Boolean</td></tr>
 * <tr><td>GlobalVariableType::Integer</td>    <td>Signed integer</td></tr>
 * <tr><td>GlobalVariableType::Unsigned</td>   <td>Unsigned integer</td></tr>
 * <tr><td>GlobalVariableType::Real</td>       <td>Floating point real (64-bit floating point)</td></tr>
 * <tr><td>GlobalVariableType::Color</td>      <td>RGBA color (= uint32)</td></tr>
 * <tr><td>GlobalVariableType::Font</td>       <td>A font face (a string)</td></tr>
 * <tr><td>GlobalVariableType::String</td>     <td>String</td></tr>
 * </table>
 */
namespace GlobalVariableType
{
   enum value_type
   {
      Undefined,  // The requested global variable doesn't exist
      Flag,       // bool
      Integer,    // int
      Unsigned,   // unsigned
      Real,       // double
      Color,      // RGBA (= uint32)
      Font,       // Font
      String      // String
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class PixInsightSettings
 * \brief Retrieves global settings from the PixInsight core application
 *
 * ### TODO: Write a detailed description for %PixInsightSettings.
 *
 * <h2>PixInsight Public Global Variables</h2>
 *
 * <h3>Global Flags</h3>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>MainWindow/MaximizeAtStartup</td>                          <td></td></tr>
 * <tr><td>MainWindow/FullScreenAtStartup</td>                        <td></td></tr>
 * <tr><td>MainWindow/ShowSplashAtStartup</td>                        <td></td></tr>
 * <tr><td>MainWindow/CheckForUpdatesAtStartup</td>                   <td></td></tr>
 * <tr><td>MainWindow/ConfirmProgramTermination</td>                  <td></td></tr>
 * <tr><td>MainWindow/AcceptDroppedFiles</td>                         <td></td></tr>
 * <tr><td>MainWindow/DoubleClickLaunchesOpenDialog</td>              <td></td></tr>
 * <tr><td>MainWindow/HoverableAutoHideWindows</td>                   <td></td></tr>
 * <tr><td>MainWindow/DesktopSettingsAware</td>                       <td></td></tr>
 * <tr><td>MainWindow/NativeMenuBar</td>                              <td></td></tr>
 * <tr><td>MainWindow/CapitalizedMenuBars</td>                        <td></td></tr>
 * <tr><td>MainWindow/TranslucentWindows</td>                         <td></td></tr>
 * <tr><td>MainWindow/TranslucentChildWindows</td>                    <td></td></tr>
 * <tr><td>MainWindow/FadeWindows</td>                                <td></td></tr>
 * <tr><td>MainWindow/FadeAutoHideWindows</td>                        <td></td></tr>
 * <tr><td>MainWindow/FadeWorkspaces</td>                             <td></td></tr>
 * <tr><td>MainWindow/FadeMenu</td>                                   <td></td></tr>
 * <tr><td>MainWindow/FadeToolTip</td>                                <td></td></tr>
 * <tr><td>MainWindow/ExplodeIcons</td>                               <td></td></tr>
 * <tr><td>MainWindow/ImplodeIcons</td>                               <td></td></tr>
 * <tr><td>MainWindow/AnimateWindows</td>                             <td></td></tr>
 * <tr><td>MainWindow/AnimateMenu</td>                                <td></td></tr>
 * <tr><td>MainWindow/AnimateCombo</td>                               <td></td></tr>
 * <tr><td>MainWindow/AnimateToolTip</td>                             <td></td></tr>
 * <tr><td>MainWindow/AnimateToolBox</td>                             <td></td></tr>
 * <tr><td>ImageWindow/BackupFiles</td>                               <td></td></tr>
 * <tr><td>ImageWindow/DefaultMasksShown</td>                         <td></td></tr>
 * <tr><td>ImageWindow/DefaultMetricResolution</td>                   <td></td></tr>
 * <tr><td>ImageWindow/DefaultEmbedMetadata</td>                      <td></td></tr>
 * <tr><td>ImageWindow/DefaultEmbedThumbnails</td>                    <td></td></tr>
 * <tr><td>ImageWindow/DefaultEmbedProperties</td>                    <td></td></tr>
 * <tr><td>ImageWindow/FollowDownloadLocations</td>                   <td></td></tr>
 * <tr><td>ImageWindow/VerboseNetworkOperations</td>                  <td>Not available on Windows.</td></tr>
 * <tr><td>ImageWindow/ShowCaptionCurrentChannels</td>                <td></td></tr>
 * <tr><td>ImageWindow/ShowCaptionZoomRatios</td>                     <td></td></tr>
 * <tr><td>ImageWindow/ShowCaptionIdentifiers</td>                    <td></td></tr>
 * <tr><td>ImageWindow/ShowCaptionFullPaths</td>                      <td></td></tr>
 * <tr><td>ImageWindow/ShowActiveSTFIndicators</td>                   <td></td></tr>
 * <tr><td>ImageWindow/NativeFileDialogs</td>                         <td></td></tr>
 * <tr><td>ImageWindow/RememberFileOpenType</td>                      <td></td></tr>
 * <tr><td>ImageWindow/RememberFileSaveType</td>                      <td></td></tr>
 * <tr><td>ImageWindow/StrictFileSaveMode</td>                        <td></td></tr>
 * <tr><td>ImageWindow/UseFileNamesAsImageIdentifiers</td>            <td></td></tr>
 * <tr><td>ImageWindow/TouchEvents</td>                               <td></td></tr>
 * <tr><td>ImageWindow/FastScreenRenditions</td>                      <td></td></tr>
 * <tr><td>ImageWindow/Default20BitScreenLUT</td>                     <td></td></tr>
 * <tr><td>Process/EnableParallelProcessing</td>                      <td></td></tr>
 * <tr><td>Process/EnableParallelCoreRendering</td>                   <td></td></tr>
 * <tr><td>Process/EnableParallelCoreColorManagement</td>             <td></td></tr>
 * <tr><td>Process/EnableParallelModuleProcessing</td>                <td></td></tr>
 * <tr><td>Process/EnableThreadCPUAffinity</td>                       <td></td></tr>
 * <tr><td>Process/BackupFiles</td>                                   <td></td></tr>
 * <tr><td>Process/GenerateScriptComments</td>                        <td></td></tr>
 * <tr><td>Process/AlertOnProcessCompleted</td>                       <td></td></tr>
 * <tr><td>ColorManagement/IsValid</td>                               <td>Read-only</td></tr>
 * <tr><td>ColorManagement/IsEnabled</td>                             <td></td></tr>
 * <tr><td>ColorManagement/DefaultEmbedProfilesInRGBImages</td>       <td></td></tr>
 * <tr><td>ColorManagement/DefaultEmbedProfilesInGrayscaleImages</td> <td></td></tr>
 * <tr><td>ColorManagement/UseProofingBPC</td>                        <td></td></tr>
 * <tr><td>ColorManagement/DefaultProofingEnabled</td>                <td></td></tr>
 * <tr><td>ColorManagement/DefaultGamutCheckEnabled</td>              <td></td></tr>
 * </table>
 *
 * <h3>Global Integers</h3>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>MainWindow/MaxRecentFiles</td>                             <td></td></tr>
 * <tr><td>ImageWindow/DefaultMaskMode</td>                           <td>See the pcl::MaskMode namespace.</td></tr>
 * <tr><td>ImageWindow/DefaultTransparencyMode</td>                   <td>See the pcl::TransparencyMode namespace.</td></tr>
 * <tr><td>ImageWindow/CursorTolerance</td>                           <td>In device pixels.</td></tr>
 * <tr><td>ImageWindow/FastScreenRenditionThreshold</td>              <td>In MiB.</td></tr>
 * <tr><td>Workspace/PrimaryScreenCenterX</td>                        <td>Read-only. In device pixels.</td></tr>
 * <tr><td>Workspace/PrimaryScreenCenterY</td>                        <td>Read-only. In device pixels.</td></tr>
 * <tr><td>TransparencyBrush/Brush</td>                               <td>See the pcl::BackgroundBrush namespace.</td></tr>
 * <tr><td>ColorManagement/DefaultRenderingIntent</td>                <td>See the pcl::ICCRenderingIntent namespace.</td></tr>
 * <tr><td>ColorManagement/ProofingIntent</td>                        <td>See the pcl::ICCRenderingIntent namespace.</td></tr>
 * <tr><td>ColorManagement/OnProfileMismatch</td>                     <td>Core application policies. See the documentation for ColorManagementSetup.</td></tr>
 * <tr><td>ColorManagement/OnMissingProfile</td>                      <td>Core application policies. See the documentation for ColorManagementSetup.</td></tr>
 * <tr><td>Process/MaxProcessors</td>                                 <td></td></tr>
 * <tr><td>Process/MaxModuleThreadPriority</td>                       <td>From 0=idle to 7=real-time.</td></tr>
 * <tr><td>Process/MaxConsoleLines</td>                               <td></td></tr>
 * <tr><td>Process/ConsoleDelay</td>                                  <td>In milliseconds.</td></tr>
 * <tr><td>Process/AutoSavePSMPeriod</td>                             <td>In seconds.</td></tr>
 * <tr><td>System/NumberOfProcessors</td>                             <td>Read-only</td></tr>
 * </table>
 *
 * <h3>Global Reals</h3>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Application/StartUTC</td>                                  <td>Read-only</td></tr>
 * <tr><td>ImageWindow/DefaultHorizontalResolution</td>               <td>In device pixels per resolution unit. See the ImageWindow/DefaultMetricResolution global variable.</td></tr>
 * <tr><td>ImageWindow/DefaultVerticalResolution</td>                 <td>In device pixels per resolution unit. See the ImageWindow/DefaultMetricResolution global variable.</td></tr>
 * <tr><td>ImageWindow/PinchSensitivity</td>                          <td>In device pixels.</td></tr>
 * </table>
 *
 * <h3>Global Colors</h3>
 *
 * Since PixInsight Core version 1.8.0, most UI colors and fonts are defined in
 * cascading style sheet files loaded automatically on startup. Only the
 * following four color variables remain because they are purely functional
 * (i.e., not related to the appearance of GUI controls).
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>TransparencyBrush/ForegroundColor</td>                     <td></td></tr>
 * <tr><td>TransparencyBrush/BackgroundColor</td>                     <td></td></tr>
 * <tr><td>TransparencyBrush/DefaultColor</td>                        <td></td></tr>
 * <tr><td>ColorManagement/GamutWarningColor</td>                     <td></td></tr>
 * </table>
 *
 * <h3>Global Fonts</h3>
 *
 * Currently there are no global font variables. Since PixInsight Core version
 * 1.8.0, all UI fonts are defined through cascading style sheet files loaded
 * automatically on startup.
 *
 * <h3>Global Strings</h3>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Application/CoreFilePath</td>                              <td>Read-only</td></tr>
 * <tr><td>Application/CoreDirectory</td>                             <td>Read-only</td></tr>
 * <tr><td>Application/BaseDirectory</td>                             <td>Read-only</td></tr>
 * <tr><td>Application/BinDirectory</td>                              <td>Read-only</td></tr>
 * <tr><td>Application/EtcDirectory</td>                              <td>Read-only</td></tr>
 * <tr><td>Application/RscDirectory</td>                              <td>Read-only</td></tr>
 * <tr><td>Application/IncludeDirectory</td>                          <td>Read-only</td></tr>
 * <tr><td>Application/SrcDirectory</td>                              <td>Read-only</td></tr>
 * <tr><td>Application/LibDirectory</td>                              <td>Read-only</td></tr>
 * <tr><td>Application/DocDirectory</td>                              <td>Read-only</td></tr>
 * <tr><td>Application/StyleSheetFile</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile01</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile02</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile03</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile04</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile05</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile06</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile07</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile08</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile09</td>                            <td></td></tr>
 * <tr><td>Application/ResourceFile10</td>                            <td></td></tr>
 * <tr><td>Workspace/Prefix</td>                                      <td></td></tr>
 * <tr><td>ImageWindow/Prefix</td>                                    <td></td></tr>
 * <tr><td>Preview/Prefix</td>                                        <td></td></tr>
 * <tr><td>ProcessIcon/Prefix</td>                                    <td></td></tr>
 * <tr><td>ImageContainerIcon/Prefix</td>                             <td></td></tr>
 * <tr><td>ImageWindow/NewImageCaption</td>                           <td></td></tr>
 * <tr><td>ImageWindow/ClonePostfix</td>                              <td></td></tr>
 * <tr><td>ViewList/NoViewsAvailableText</td>                         <td></td></tr>
 * <tr><td>ViewList/NoViewSelectedText</td>                           <td></td></tr>
 * <tr><td>ViewList/NoPreviewsAvailableText</td>                      <td></td></tr>
 * <tr><td>ViewList/NoPreviewSelectedText</td>                        <td></td></tr>
 * <tr><td>View/BrokenLinkText</td>                                   <td></td></tr>
 * <tr><td>ImageWindow/DefaultFileExtension</td>                      <td></td></tr>
 * <tr><td>ImageWindow/DownloadsDirectory</td>                        <td></td></tr>
 * <tr><td>ColorManagement/MonitorProfilePath</td>                    <td>Read-only</td></tr>
 * <tr><td>ColorManagement/UpdateMonitorProfile</td>                  <td>Write-only. Only available on X11 (UNIX/Linux). See ColorManagementSetup documentation.</td></tr>
 * <tr><td>ColorManagement/DefaultRGBProfilePath</td>                 <td></td></tr>
 * <tr><td>ColorManagement/DefaultGrayscaleProfilePath</td>           <td></td></tr>
 * <tr><td>ColorManagement/ProofingProfilePath</td>                   <td></td></tr>
 * <tr><td>FileFormat/ReadFilters</td>                                <td>Read-only. Updated dynamically on-demand.</td></tr>
 * <tr><td>FileFormat/WriteFilters</td>                               <td>Read-only. Updated dynamically on-demand.</td></tr>
 * </table>
 */
class PCL_CLASS PixInsightSettings
{
public:

   /*!
    * Represents the data type of a global variable.
    */
   typedef GlobalVariableType::value_type variable_type;

   /*!
    * Retrieves the type of a global variable \a globalId.
    */
   static variable_type GlobalVariableType( const IsoString& globalId );

   /*!
    * Returns true if the specified global variable \a globalId is defined in
    * the current PixInsight platform.
    */
   static bool IsGlobalVariableDefined( const IsoString& globalId )
   {
      return GlobalVariableType( globalId ) != GlobalVariableType::Undefined;
   }

   /*!
    * Returns the value of a global boolean variable.
    */
   static bool GlobalFlag( const IsoString& globalId );

   /*!
    * Returns the value of a global signed integer variable.
    */
   static int GlobalInteger( const IsoString& globalId );

   /*!
    * Returns the value of a global unsigned integer variable.
    */
   static unsigned GlobalUnsigned( const IsoString& globalId );

   /*!
    * Returns the value of a global floating point real variable.
    */
   static double GlobalReal( const IsoString& globalId );

   /*!
    * Returns the value of a global RGBA color variable.
    */
   static RGBA GlobalColor( const IsoString& globalId );

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   /*!
    * Returns the value of a global font face variable.
    */
   static Font GlobalFont( const IsoString& globalId );
#endif

   /*!
    * Returns the value of a global string variable.
    */
   static String GlobalString( const IsoString& globalId );

   /*!
    * Begins a global variable update operation.
    *
    * \sa EndUpdate(), CalcelUpdate()
    */
   static void BeginUpdate();

   /*!
    * Terminates a global variable update operation.
    *
    * \sa BeginUpdate(), CancelUpdate()
    */
   static void EndUpdate();

   /*!
    * Aborts an ongoing global variable update operation.
    *
    * Use this function if you catch an exception during the sequence of
    * SetGlobalXXX() calls. After cancelling an update operation, you
    * shouldn't call EndUpdate().
    *
    * \sa BeginUpdate(), EndUpdate()
    */
   static void CancelUpdate();

   /*!
    * Sets the value of a global boolean variable.
    */
   static void SetGlobalFlag( const IsoString& globalId, bool );

   /*!
    * Sets the value of a global signed integer variable.
    */
   static void SetGlobalInteger( const IsoString& globalId, int );

   /*!
    * Sets the value of a global unsigned integer variable.
    */
   static void SetGlobalUnsigned( const IsoString& globalId, unsigned );

   /*!
    * Sets the value of a global floating point real variable.
    */
   static void SetGlobalReal( const IsoString& globalId, double );

   /*!
    * Sets the value of a global RGBA color variable.
    */
   static void SetGlobalColor( const IsoString& globalId, RGBA );

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   /*!
    * Sets the value of a global font face variable.
    */
   static void SetGlobalFont( const IsoString& globalId, const Font& );
#endif

   /*!
    * Sets the value of a global string variable.
    */
   static void SetGlobalString( const IsoString& globalId, const String& );

private:

   // Not an instantiable class
   PixInsightSettings() { PCL_CHECK( 0 ) }
   PixInsightSettings ( const PixInsightSettings& ) { PCL_CHECK( 0 ) }
   virtual ~PixInsightSettings() {}
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_GlobalSettings_h

// ****************************************************************************
// EOF pcl/GlobalSettings.h - Released 2014/11/14 17:16:40 UTC
