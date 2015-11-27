//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0318
// ----------------------------------------------------------------------------
// PreferencesInstance.cpp - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "PreferencesInstance.h"
#include "PreferencesParameters.h"

#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PreferencesInstance::PreferencesInstance( const MetaProcess* p ) :
ProcessImplementation( p )
{
   LoadDefaultSettings();
}

PreferencesInstance::PreferencesInstance( const PreferencesInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void PreferencesInstance::Assign( const ProcessImplementation& p )
{
   const PreferencesInstance* x = dynamic_cast<const PreferencesInstance*>( &p );
   if ( x != 0 )
   {
      application = x->application;
      mainWindow  = x->mainWindow;
      imageWindow = x->imageWindow;
      identifiers = x->identifiers;
      process     = x->process;
   }
}

// ----------------------------------------------------------------------------

bool PreferencesInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "Preferences can only be executed in the global context.";
   return false;
}

bool PreferencesInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool PreferencesInstance::ExecuteGlobal()
{
   PixInsightSettings::BeginUpdate();

   try
   {
      PixInsightSettings::SetGlobalString ( "Application/StyleSheetFile",                 application.styleSheetFile );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile01",                 application.resourceFile01 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile02",                 application.resourceFile02 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile03",                 application.resourceFile03 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile04",                 application.resourceFile04 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile05",                 application.resourceFile05 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile06",                 application.resourceFile06 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile07",                 application.resourceFile07 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile08",                 application.resourceFile08 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile09",                 application.resourceFile09 );
      PixInsightSettings::SetGlobalString ( "Application/ResourceFile10",                 application.resourceFile10 );
      PixInsightSettings::SetGlobalFlag   ( "Application/AutoUIScaling",                  application.autoUIScaling );
      PixInsightSettings::SetGlobalReal   ( "Application/UIScalingFactor",                application.uiScalingFactor );
      PixInsightSettings::SetGlobalInteger( "Application/FontResolution",                 application.fontResolution );
      PixInsightSettings::SetGlobalString ( "Application/LowResFont",                     application.lowResFont );
      PixInsightSettings::SetGlobalString ( "Application/HighResFont",                    application.highResFont );
      PixInsightSettings::SetGlobalString ( "Application/LowResMonoFont",                 application.lowResMonoFont );
      PixInsightSettings::SetGlobalString ( "Application/HighResMonoFont",                application.highResMonoFont );

      PixInsightSettings::SetGlobalFlag   ( "MainWindow/MaximizeAtStartup",               mainWindow.maximizeAtStartup );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/FullScreenAtStartup",             mainWindow.fullScreenAtStartup );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/ShowSplashAtStartup",             mainWindow.showSplashAtStartup );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/CheckForUpdatesAtStartup",        mainWindow.checkForUpdatesAtStartup );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/ConfirmProgramTermination",       mainWindow.confirmProgramTermination );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/AcceptDroppedFiles",              mainWindow.acceptDroppedFiles );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/DoubleClickLaunchesOpenDialog",   mainWindow.doubleClickLaunchesOpenDialog );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/HoverableAutoHideWindows",        mainWindow.hoverableAutoHideWindows );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/DesktopSettingsAware",            mainWindow.desktopSettingsAware );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/NativeMenuBar",                   mainWindow.nativeMenuBar );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/CapitalizedMenuBars",             mainWindow.capitalizedMenuBars );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/WindowButtonsOnTheLeft",          mainWindow.windowButtonsOnTheLeft );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/TranslucentWindows",              mainWindow.translucentWindows );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/TranslucentChildWindows",         mainWindow.translucentChildWindows );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/FadeWindows",                     mainWindow.fadeWindows );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/FadeAutoHideWindows",             mainWindow.fadeAutoHideWindows );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/TranslucentAutoHideWindows",      mainWindow.translucentAutoHideWindows );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/FadeWorkspaces",                  mainWindow.fadeWorkspaces );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/FadeMenu",                        mainWindow.fadeMenu );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/FadeToolTip",                     mainWindow.fadeToolTip );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/ExplodeIcons",                    mainWindow.explodeIcons );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/ImplodeIcons",                    mainWindow.implodeIcons );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/AnimateWindows",                  mainWindow.animateWindows );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/AnimateMenu",                     mainWindow.animateMenu );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/AnimateCombo",                    mainWindow.animateCombo );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/AnimateToolTip",                  mainWindow.animateToolTip );
      PixInsightSettings::SetGlobalFlag   ( "MainWindow/AnimateToolBox",                  mainWindow.animateToolBox );
      PixInsightSettings::SetGlobalInteger( "MainWindow/MaxRecentFiles",                  mainWindow.maxRecentFiles );

      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/BackupFiles",                    imageWindow.backupFiles );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/DefaultMasksShown",              imageWindow.defaultMasksShown );
      PixInsightSettings::SetGlobalInteger( "ImageWindow/DefaultMaskMode",                imageWindow.defaultMaskMode );
      PixInsightSettings::SetGlobalInteger( "ImageWindow/DefaultTransparencyMode",        imageWindow.defaultTransparencyMode );
      PixInsightSettings::SetGlobalReal   ( "ImageWindow/DefaultHorizontalResolution",    imageWindow.defaultHorizontalResolution );
      PixInsightSettings::SetGlobalReal   ( "ImageWindow/DefaultVerticalResolution",      imageWindow.defaultVerticalResolution );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/DefaultMetricResolution",        imageWindow.defaultMetricResolution );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/DefaultEmbedThumbnails",         imageWindow.defaultEmbedThumbnails );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/DefaultEmbedProperties",         imageWindow.defaultEmbedProperties );
      PixInsightSettings::SetGlobalString ( "ImageWindow/DefaultFileExtension",           imageWindow.defaultFileExtension );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/NativeFileDialogs",              imageWindow.nativeFileDialogs );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/RememberFileOpenType",           imageWindow.rememberFileOpenType );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/RememberFileSaveType",           imageWindow.rememberFileSaveType );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/StrictFileSaveMode",             imageWindow.strictFileSaveMode );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/FileFormatWarnings",             imageWindow.fileFormatWarnings );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/UseFileNamesAsImageIdentifiers", imageWindow.useFileNamesAsImageIdentifiers );
      PixInsightSettings::SetGlobalInteger( "ImageWindow/CursorTolerance",                imageWindow.cursorTolerance );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/TouchEvents",                    imageWindow.touchEvents );
      PixInsightSettings::SetGlobalReal   ( "ImageWindow/PinchSensitivity",               imageWindow.pinchSensitivity );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/FastScreenRenditions",           imageWindow.fastScreenRenditions );
      PixInsightSettings::SetGlobalInteger( "ImageWindow/FastScreenRenditionThreshold",   imageWindow.fastScreenRenditionThreshold );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/HighDPIRenditions",              imageWindow.highDPIRenditions );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/Default24BitScreenLUT",          imageWindow.default24BitScreenLUT );
      // Better don't mess during a global settings update - for sanity, we'll do this after EndUpdate()
      //ImageWindow::SetSwapDirectories( imageWindow.swapDirectories);
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/SwapCompression",                imageWindow.swapCompression );
      PixInsightSettings::SetGlobalString ( "ImageWindow/DownloadsDirectory",             imageWindow.downloadsDirectory );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/FollowDownloadLocations",        imageWindow.followDownloadLocations );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/VerboseNetworkOperations",       imageWindow.verboseNetworkOperations );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/ShowCaptionCurrentChannels",     imageWindow.showCaptionCurrentChannels );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/ShowCaptionZoomRatios",          imageWindow.showCaptionZoomRatios );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/ShowCaptionIdentifiers",         imageWindow.showCaptionIdentifiers );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/ShowCaptionFullPaths",           imageWindow.showCaptionFullPaths );
      PixInsightSettings::SetGlobalFlag   ( "ImageWindow/ShowActiveSTFIndicators",        imageWindow.showActiveSTFIndicators );
      PixInsightSettings::SetGlobalInteger( "TransparencyBrush/Brush",                    imageWindow.transparencyBrush );
      PixInsightSettings::SetGlobalColor  ( "TransparencyBrush/ForegroundColor",          imageWindow.transparencyBrushForegroundColor );
      PixInsightSettings::SetGlobalColor  ( "TransparencyBrush/BackgroundColor",          imageWindow.transparencyBrushBackgroundColor );
      PixInsightSettings::SetGlobalColor  ( "TransparencyBrush/DefaultColor",             imageWindow.defaultTransparencyColor );

      PixInsightSettings::SetGlobalString ( "Workspace/Prefix",                           identifiers.workspacePrefix );
      PixInsightSettings::SetGlobalString ( "ImageWindow/Prefix",                         identifiers.imagePrefix );
      PixInsightSettings::SetGlobalString ( "Preview/Prefix",                             identifiers.previewPrefix );
      PixInsightSettings::SetGlobalString ( "ProcessIcon/Prefix",                         identifiers.processIconPrefix );
      PixInsightSettings::SetGlobalString ( "ImageContainerIcon/Prefix",                  identifiers.imageContainerIconPrefix );
      PixInsightSettings::SetGlobalString ( "ImageWindow/NewImageCaption",                identifiers.newImageCaption );
      PixInsightSettings::SetGlobalString ( "ImageWindow/ClonePostfix",                   identifiers.clonePostfix );
      PixInsightSettings::SetGlobalString ( "ViewList/NoViewsAvailableText",              identifiers.noViewsAvailableText );
      PixInsightSettings::SetGlobalString ( "ViewList/NoViewSelectedText",                identifiers.noViewSelectedText );
      PixInsightSettings::SetGlobalString ( "ViewList/NoPreviewsAvailableText",           identifiers.noPreviewsAvailableText );
      PixInsightSettings::SetGlobalString ( "ViewList/NoPreviewSelectedText",             identifiers.noPreviewSelectedText );
      PixInsightSettings::SetGlobalString ( "View/BrokenLinkText",                        identifiers.brokenLinkText );

      PixInsightSettings::SetGlobalFlag   ( "Process/EnableParallelProcessing",           process.enableParallelProcessing );
      PixInsightSettings::SetGlobalFlag   ( "Process/EnableParallelCoreRendering",        process.enableParallelCoreRendering );
      PixInsightSettings::SetGlobalFlag   ( "Process/EnableParallelCoreColorManagement",  process.enableParallelCoreColorManagement );
      PixInsightSettings::SetGlobalFlag   ( "Process/EnableParallelModuleProcessing",     process.enableParallelModuleProcessing );
      PixInsightSettings::SetGlobalFlag   ( "Process/EnableThreadCPUAffinity",            process.enableThreadCPUAffinity );
      PixInsightSettings::SetGlobalInteger( "Process/MaxModuleThreadPriority",            process.maxModuleThreadPriority );
      PixInsightSettings::SetGlobalInteger( "Process/MaxProcessors",                      process.maxProcessors );
      PixInsightSettings::SetGlobalFlag   ( "Process/BackupFiles",                        process.backupFiles );
      PixInsightSettings::SetGlobalFlag   ( "Process/GenerateScriptComments",             process.generateScriptComments );
      PixInsightSettings::SetGlobalInteger( "Process/MaxConsoleLines",                    process.maxConsoleLines );
      PixInsightSettings::SetGlobalInteger( "Process/ConsoleDelay",                       process.consoleDelay );
      PixInsightSettings::SetGlobalInteger( "Process/AutoSavePSMPeriod",                  process.autoSavePSMPeriod );
      PixInsightSettings::SetGlobalFlag   ( "Process/AlertOnProcessCompleted",            process.alertOnProcessCompleted );

      PixInsightSettings::EndUpdate();

      // See note above
      ImageWindow::SetSwapDirectories( imageWindow.swapDirectories );

      return true;
   }

   catch ( ... )
   {
      // ### Never forget this, or the core will bit your ass :)
      PixInsightSettings::CancelUpdate();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* PreferencesInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == METAPARAMETER_INSTANCE_ID( Application, styleSheetFile ) )
      return application.styleSheetFile.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile01 ) )
      return application.resourceFile01.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile02 ) )
      return application.resourceFile02.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile03 ) )
      return application.resourceFile03.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile04 ) )
      return application.resourceFile04.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile05 ) )
      return application.resourceFile05.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile06 ) )
      return application.resourceFile06.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile07 ) )
      return application.resourceFile07.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile08 ) )
      return application.resourceFile08.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile09 ) )
      return application.resourceFile09.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile10 ) )
      return application.resourceFile10.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, autoUIScaling ) )
      return &application.autoUIScaling;
   if ( p == METAPARAMETER_INSTANCE_ID( Application, uiScalingFactor ) )
      return &application.uiScalingFactor;
   if ( p == METAPARAMETER_INSTANCE_ID( Application, fontResolution ) )
      return &application.fontResolution;
   if ( p == METAPARAMETER_INSTANCE_ID( Application, lowResFont ) )
      return application.lowResFont.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, highResFont ) )
      return application.highResFont.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, lowResMonoFont ) )
      return application.lowResMonoFont.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Application, highResMonoFont ) )
      return application.highResMonoFont.c_str();

   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, maximizeAtStartup ) )
      return &mainWindow.maximizeAtStartup;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, fullScreenAtStartup ) )
      return &mainWindow.fullScreenAtStartup;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, showSplashAtStartup ) )
      return &mainWindow.showSplashAtStartup;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, checkForUpdatesAtStartup ) )
      return &mainWindow.checkForUpdatesAtStartup;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, confirmProgramTermination ) )
      return &mainWindow.confirmProgramTermination;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, acceptDroppedFiles ) )
      return &mainWindow.acceptDroppedFiles;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, doubleClickLaunchesOpenDialog ) )
      return &mainWindow.doubleClickLaunchesOpenDialog;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, hoverableAutoHideWindows ) )
      return &mainWindow.hoverableAutoHideWindows;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, desktopSettingsAware ) )
      return &mainWindow.desktopSettingsAware;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, nativeMenuBar ) )
      return &mainWindow.nativeMenuBar;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, capitalizedMenuBars ) )
      return &mainWindow.capitalizedMenuBars;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, windowButtonsOnTheLeft ) )
      return &mainWindow.windowButtonsOnTheLeft;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, translucentWindows ) )
      return &mainWindow.translucentWindows;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, translucentChildWindows ) )
      return &mainWindow.translucentChildWindows;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, fadeWindows ) )
      return &mainWindow.fadeWindows;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, fadeAutoHideWindows ) )
      return &mainWindow.fadeAutoHideWindows;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, translucentAutoHideWindows ) )
      return &mainWindow.translucentAutoHideWindows;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, fadeWorkspaces ) )
      return &mainWindow.fadeWorkspaces;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, fadeMenu ) )
      return &mainWindow.fadeMenu;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, fadeToolTip ) )
      return &mainWindow.fadeToolTip;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, explodeIcons ) )
      return &mainWindow.explodeIcons;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, implodeIcons ) )
      return &mainWindow.implodeIcons;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, animateWindows ) )
      return &mainWindow.animateWindows;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, animateMenu ) )
      return &mainWindow.animateMenu;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, animateCombo ) )
      return &mainWindow.animateCombo;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, animateToolTip ) )
      return &mainWindow.animateToolTip;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, animateToolBox ) )
      return &mainWindow.animateToolBox;
   if ( p == METAPARAMETER_INSTANCE_ID( MainWindow, maxRecentFiles ) )
      return &mainWindow.maxRecentFiles;

   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, backupFiles ) )
      return &imageWindow.backupFiles;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultMasksShown ) )
      return &imageWindow.defaultMasksShown;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultMaskMode ) )
      return &imageWindow.defaultMaskMode;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultTransparencyMode ) )
      return &imageWindow.defaultTransparencyMode;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultHorizontalResolution ) )
      return &imageWindow.defaultHorizontalResolution;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultVerticalResolution ) )
      return &imageWindow.defaultVerticalResolution;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultMetricResolution ) )
      return &imageWindow.defaultMetricResolution;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultEmbedThumbnails ) )
      return &imageWindow.defaultEmbedThumbnails;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultEmbedProperties ) )
      return &imageWindow.defaultEmbedProperties;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultFileExtension ) )
      return imageWindow.defaultFileExtension.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, nativeFileDialogs ) )
      return &imageWindow.nativeFileDialogs;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, rememberFileOpenType ) )
      return &imageWindow.rememberFileOpenType;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, rememberFileSaveType ) )
      return &imageWindow.rememberFileSaveType;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, strictFileSaveMode ) )
      return &imageWindow.strictFileSaveMode;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, fileFormatWarnings ) )
      return &imageWindow.fileFormatWarnings;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, useFileNamesAsImageIdentifiers ) )
      return &imageWindow.useFileNamesAsImageIdentifiers;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, cursorTolerance ) )
      return &imageWindow.cursorTolerance;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, touchEvents ) )
      return &imageWindow.touchEvents;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, pinchSensitivity ) )
      return &imageWindow.pinchSensitivity;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, fastScreenRenditions ) )
      return &imageWindow.fastScreenRenditions;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, fastScreenRenditionThreshold ) )
      return &imageWindow.fastScreenRenditionThreshold;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, highDPIRenditions ) )
      return &imageWindow.highDPIRenditions;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, default24BitScreenLUT ) )
      return &imageWindow.default24BitScreenLUT;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, swapDirectory ) )
      return imageWindow.swapDirectories[tableRow].c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, swapCompression ) )
      return &imageWindow.swapCompression;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, downloadsDirectory ) )
      return imageWindow.downloadsDirectory.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, followDownloadLocations ) )
      return &imageWindow.followDownloadLocations;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, verboseNetworkOperations ) )
      return &imageWindow.verboseNetworkOperations;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionCurrentChannels ) )
      return &imageWindow.showCaptionCurrentChannels;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionZoomRatios ) )
      return &imageWindow.showCaptionZoomRatios;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionIdentifiers ) )
      return &imageWindow.showCaptionIdentifiers;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionFullPaths ) )
      return &imageWindow.showCaptionFullPaths;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, showActiveSTFIndicators ) )
      return &imageWindow.showActiveSTFIndicators;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, transparencyBrush ) )
      return &imageWindow.transparencyBrush;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, transparencyBrushForegroundColor ) )
      return &imageWindow.transparencyBrushForegroundColor;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, transparencyBrushBackgroundColor ) )
      return &imageWindow.transparencyBrushBackgroundColor;
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultTransparencyColor ) )
      return &imageWindow.defaultTransparencyColor;

   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, workspacePrefix ) )
      return identifiers.workspacePrefix.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, imagePrefix ) )
      return identifiers.imagePrefix.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, previewPrefix ) )
      return identifiers.previewPrefix.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, processIconPrefix ) )
      return identifiers.processIconPrefix.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, imageContainerIconPrefix ) )
      return identifiers.imageContainerIconPrefix.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, newImageCaption ) )
      return identifiers.newImageCaption.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, clonePostfix ) )
      return identifiers.clonePostfix.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noViewsAvailableText ) )
      return identifiers.noViewsAvailableText.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noViewSelectedText ) )
      return identifiers.noViewSelectedText.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noPreviewsAvailableText ) )
      return identifiers.noPreviewsAvailableText.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noPreviewSelectedText ) )
      return identifiers.noPreviewSelectedText.c_str();
   if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, brokenLinkText ) )
      return identifiers.brokenLinkText.c_str();

   if ( p == METAPARAMETER_INSTANCE_ID( Process, enableParallelProcessing ) )
      return &process.enableParallelProcessing;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, enableParallelCoreRendering ) )
      return &process.enableParallelCoreRendering;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, enableParallelCoreColorManagement ) )
      return &process.enableParallelCoreColorManagement;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, enableParallelModuleProcessing ) )
      return &process.enableParallelModuleProcessing;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, enableThreadCPUAffinity ) )
      return &process.enableThreadCPUAffinity;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, maxModuleThreadPriority ) )
      return &process.maxModuleThreadPriority;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, maxProcessors ) )
      return &process.maxProcessors;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, backupFiles ) )
      return &process.backupFiles;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, generateScriptComments ) )
      return &process.generateScriptComments;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, maxConsoleLines ) )
      return &process.maxConsoleLines;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, consoleDelay ) )
      return &process.consoleDelay;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, autoSavePSMPeriod ) )
      return &process.autoSavePSMPeriod;
   if ( p == METAPARAMETER_INSTANCE_ID( Process, alertOnProcessCompleted ) )
      return &process.alertOnProcessCompleted;

   return 0;
}

bool PreferencesInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, swapDirectories ) )
   {
      imageWindow.swapDirectories.Clear();
      if ( sizeOrLength > 0 )
         imageWindow.swapDirectories.Add( String(), sizeOrLength );
   }
   else if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, swapDirectory ) )
   {
      imageWindow.swapDirectories[tableRow].Clear();
      if ( sizeOrLength > 0 )
         imageWindow.swapDirectories[tableRow].SetLength( sizeOrLength );
   }
   else
   {
      String* s = StringParameterFromMetaParameter( p );
      if ( s == 0 )
         return false;

      if ( sizeOrLength != 0 )
         s->SetLength( sizeOrLength );
      else
         s->Clear();
   }

   return true;
}

size_type PreferencesInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, swapDirectories ) )
      return imageWindow.swapDirectories.Length();

   if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, swapDirectory ) )
      return imageWindow.swapDirectories[tableRow].Length();

   String* s = const_cast<PreferencesInstance*>( this )->StringParameterFromMetaParameter( p );
   if ( s != 0 )
      return s->Length();

   return 0;
}

// ----------------------------------------------------------------------------

void PreferencesInstance::LoadDefaultSettings()
{
   application.styleSheetFile                   =         METAPARAMETER_INSTANCE_ID( Application, styleSheetFile                   )->DefaultValue();
   application.resourceFile01                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile01                   )->DefaultValue();
   application.resourceFile02                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile02                   )->DefaultValue();
   application.resourceFile03                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile03                   )->DefaultValue();
   application.resourceFile04                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile04                   )->DefaultValue();
   application.resourceFile05                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile05                   )->DefaultValue();
   application.resourceFile06                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile06                   )->DefaultValue();
   application.resourceFile07                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile07                   )->DefaultValue();
   application.resourceFile08                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile08                   )->DefaultValue();
   application.resourceFile09                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile09                   )->DefaultValue();
   application.resourceFile10                   =         METAPARAMETER_INSTANCE_ID( Application, resourceFile10                   )->DefaultValue();
   application.autoUIScaling                    =         METAPARAMETER_INSTANCE_ID( Application, autoUIScaling                    )->DefaultValue();
   application.uiScalingFactor                  =         METAPARAMETER_INSTANCE_ID( Application, uiScalingFactor                  )->DefaultValue();
   application.fontResolution                   =  int32( METAPARAMETER_INSTANCE_ID( Application, fontResolution                   )->DefaultValue() );
   application.lowResFont                       =         METAPARAMETER_INSTANCE_ID( Application, lowResFont                       )->DefaultValue();
   application.highResFont                      =         METAPARAMETER_INSTANCE_ID( Application, highResFont                      )->DefaultValue();
   application.lowResMonoFont                   =         METAPARAMETER_INSTANCE_ID( Application, lowResMonoFont                   )->DefaultValue();
   application.highResMonoFont                  =         METAPARAMETER_INSTANCE_ID( Application, highResMonoFont                  )->DefaultValue();

   mainWindow.maximizeAtStartup                 =         METAPARAMETER_INSTANCE_ID( MainWindow, maximizeAtStartup                 )->DefaultValue();
   mainWindow.fullScreenAtStartup               =         METAPARAMETER_INSTANCE_ID( MainWindow, fullScreenAtStartup               )->DefaultValue();
   mainWindow.showSplashAtStartup               =         METAPARAMETER_INSTANCE_ID( MainWindow, showSplashAtStartup               )->DefaultValue();
   mainWindow.checkForUpdatesAtStartup          =         METAPARAMETER_INSTANCE_ID( MainWindow, checkForUpdatesAtStartup          )->DefaultValue();
   mainWindow.confirmProgramTermination         =         METAPARAMETER_INSTANCE_ID( MainWindow, confirmProgramTermination         )->DefaultValue();
   mainWindow.acceptDroppedFiles                =         METAPARAMETER_INSTANCE_ID( MainWindow, acceptDroppedFiles                )->DefaultValue();
   mainWindow.doubleClickLaunchesOpenDialog     =         METAPARAMETER_INSTANCE_ID( MainWindow, doubleClickLaunchesOpenDialog     )->DefaultValue();
   mainWindow.hoverableAutoHideWindows          =         METAPARAMETER_INSTANCE_ID( MainWindow, hoverableAutoHideWindows          )->DefaultValue();
   mainWindow.desktopSettingsAware              =         METAPARAMETER_INSTANCE_ID( MainWindow, desktopSettingsAware              )->DefaultValue();
   mainWindow.nativeMenuBar                     =         METAPARAMETER_INSTANCE_ID( MainWindow, nativeMenuBar                     )->DefaultValue();
   mainWindow.capitalizedMenuBars               =         METAPARAMETER_INSTANCE_ID( MainWindow, capitalizedMenuBars               )->DefaultValue();
   mainWindow.windowButtonsOnTheLeft            =         METAPARAMETER_INSTANCE_ID( MainWindow, windowButtonsOnTheLeft            )->DefaultValue();
   mainWindow.translucentWindows                =         METAPARAMETER_INSTANCE_ID( MainWindow, translucentWindows                )->DefaultValue();
   mainWindow.translucentChildWindows           =         METAPARAMETER_INSTANCE_ID( MainWindow, translucentChildWindows           )->DefaultValue();
   mainWindow.fadeWindows                       =         METAPARAMETER_INSTANCE_ID( MainWindow, fadeWindows                       )->DefaultValue();
   mainWindow.fadeAutoHideWindows               =         METAPARAMETER_INSTANCE_ID( MainWindow, fadeAutoHideWindows               )->DefaultValue();
   mainWindow.translucentAutoHideWindows        =         METAPARAMETER_INSTANCE_ID( MainWindow, translucentAutoHideWindows        )->DefaultValue();
   mainWindow.fadeWorkspaces                    =         METAPARAMETER_INSTANCE_ID( MainWindow, fadeWorkspaces                    )->DefaultValue();
   mainWindow.fadeMenu                          =         METAPARAMETER_INSTANCE_ID( MainWindow, fadeMenu                          )->DefaultValue();
   mainWindow.fadeToolTip                       =         METAPARAMETER_INSTANCE_ID( MainWindow, fadeToolTip                       )->DefaultValue();
   mainWindow.explodeIcons                      =         METAPARAMETER_INSTANCE_ID( MainWindow, explodeIcons                      )->DefaultValue();
   mainWindow.implodeIcons                      =         METAPARAMETER_INSTANCE_ID( MainWindow, implodeIcons                      )->DefaultValue();
   mainWindow.animateWindows                    =         METAPARAMETER_INSTANCE_ID( MainWindow, animateWindows                    )->DefaultValue();
   mainWindow.animateMenu                       =         METAPARAMETER_INSTANCE_ID( MainWindow, animateMenu                       )->DefaultValue();
   mainWindow.animateCombo                      =         METAPARAMETER_INSTANCE_ID( MainWindow, animateCombo                      )->DefaultValue();
   mainWindow.animateToolTip                    =         METAPARAMETER_INSTANCE_ID( MainWindow, animateToolTip                    )->DefaultValue();
   mainWindow.animateToolBox                    =         METAPARAMETER_INSTANCE_ID( MainWindow, animateToolBox                    )->DefaultValue();
   mainWindow.maxRecentFiles                    =  int32( METAPARAMETER_INSTANCE_ID( MainWindow, maxRecentFiles                    )->DefaultValue() );

   imageWindow.backupFiles                      =         METAPARAMETER_INSTANCE_ID( ImageWindow, backupFiles                      )->DefaultValue();
   imageWindow.defaultMasksShown                =         METAPARAMETER_INSTANCE_ID( ImageWindow, defaultMasksShown                )->DefaultValue();
   imageWindow.defaultMaskMode                  =  int32( METAPARAMETER_INSTANCE_ID( ImageWindow, defaultMaskMode                  )->DefaultValue() );
   imageWindow.defaultTransparencyMode          =  int32( METAPARAMETER_INSTANCE_ID( ImageWindow, defaultTransparencyMode          )->DefaultValue() );
   imageWindow.defaultHorizontalResolution      =         METAPARAMETER_INSTANCE_ID( ImageWindow, defaultHorizontalResolution      )->DefaultValue();
   imageWindow.defaultVerticalResolution        =         METAPARAMETER_INSTANCE_ID( ImageWindow, defaultVerticalResolution        )->DefaultValue();
   imageWindow.defaultMetricResolution          =         METAPARAMETER_INSTANCE_ID( ImageWindow, defaultMetricResolution          )->DefaultValue();
   imageWindow.defaultEmbedThumbnails           =         METAPARAMETER_INSTANCE_ID( ImageWindow, defaultEmbedThumbnails           )->DefaultValue();
   imageWindow.defaultEmbedProperties           =         METAPARAMETER_INSTANCE_ID( ImageWindow, defaultEmbedProperties           )->DefaultValue();
   imageWindow.defaultFileExtension             =         METAPARAMETER_INSTANCE_ID( ImageWindow, defaultFileExtension             )->DefaultValue();
   imageWindow.nativeFileDialogs                =         METAPARAMETER_INSTANCE_ID( ImageWindow, nativeFileDialogs                )->DefaultValue();
   imageWindow.rememberFileOpenType             =         METAPARAMETER_INSTANCE_ID( ImageWindow, rememberFileOpenType             )->DefaultValue();
   imageWindow.rememberFileSaveType             =         METAPARAMETER_INSTANCE_ID( ImageWindow, rememberFileSaveType             )->DefaultValue();
   imageWindow.strictFileSaveMode               =         METAPARAMETER_INSTANCE_ID( ImageWindow, strictFileSaveMode               )->DefaultValue();
   imageWindow.fileFormatWarnings               =         METAPARAMETER_INSTANCE_ID( ImageWindow, fileFormatWarnings               )->DefaultValue();
   imageWindow.useFileNamesAsImageIdentifiers   =         METAPARAMETER_INSTANCE_ID( ImageWindow, useFileNamesAsImageIdentifiers   )->DefaultValue();
   imageWindow.cursorTolerance                  =  int32( METAPARAMETER_INSTANCE_ID( ImageWindow, cursorTolerance                  )->DefaultValue() );
   imageWindow.touchEvents                      =         METAPARAMETER_INSTANCE_ID( ImageWindow, touchEvents                      )->DefaultValue();
   imageWindow.pinchSensitivity                 =         METAPARAMETER_INSTANCE_ID( ImageWindow, pinchSensitivity                 )->DefaultValue();
   imageWindow.fastScreenRenditions             =         METAPARAMETER_INSTANCE_ID( ImageWindow, fastScreenRenditions             )->DefaultValue();
   imageWindow.fastScreenRenditionThreshold     =  int32( METAPARAMETER_INSTANCE_ID( ImageWindow, fastScreenRenditionThreshold     )->DefaultValue() );
   imageWindow.highDPIRenditions                =         METAPARAMETER_INSTANCE_ID( ImageWindow, highDPIRenditions                )->DefaultValue();
   imageWindow.default24BitScreenLUT            =         METAPARAMETER_INSTANCE_ID( ImageWindow, default24BitScreenLUT            )->DefaultValue();
   imageWindow.swapDirectories.Clear();
   imageWindow.swapDirectories.Add(                File::SystemTempDirectory() );
   imageWindow.swapCompression                  =         METAPARAMETER_INSTANCE_ID( ImageWindow, swapCompression                  )->DefaultValue();
   imageWindow.downloadsDirectory               =  File::SystemTempDirectory();
   imageWindow.followDownloadLocations          =         METAPARAMETER_INSTANCE_ID( ImageWindow, followDownloadLocations          )->DefaultValue();
   imageWindow.verboseNetworkOperations         =         METAPARAMETER_INSTANCE_ID( ImageWindow, verboseNetworkOperations         )->DefaultValue();
   imageWindow.showCaptionCurrentChannels       =         METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionCurrentChannels       )->DefaultValue();
   imageWindow.showCaptionZoomRatios            =         METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionZoomRatios            )->DefaultValue();
   imageWindow.showCaptionIdentifiers           =         METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionIdentifiers           )->DefaultValue();
   imageWindow.showCaptionFullPaths             =         METAPARAMETER_INSTANCE_ID( ImageWindow, showCaptionFullPaths             )->DefaultValue();
   imageWindow.showActiveSTFIndicators          =         METAPARAMETER_INSTANCE_ID( ImageWindow, showActiveSTFIndicators          )->DefaultValue();
   imageWindow.transparencyBrush                =  int32( METAPARAMETER_INSTANCE_ID( ImageWindow, transparencyBrush                )->DefaultValue() );
   imageWindow.transparencyBrushForegroundColor = uint32( METAPARAMETER_INSTANCE_ID( ImageWindow, transparencyBrushForegroundColor )->DefaultValue() );
   imageWindow.transparencyBrushBackgroundColor = uint32( METAPARAMETER_INSTANCE_ID( ImageWindow, transparencyBrushBackgroundColor )->DefaultValue() );
   imageWindow.defaultTransparencyColor         = uint32( METAPARAMETER_INSTANCE_ID( ImageWindow, defaultTransparencyColor         )->DefaultValue() );

   identifiers.workspacePrefix                  =         METAPARAMETER_INSTANCE_ID( Identifiers, workspacePrefix                  )->DefaultValue();
   identifiers.imagePrefix                      =         METAPARAMETER_INSTANCE_ID( Identifiers, imagePrefix                      )->DefaultValue();
   identifiers.previewPrefix                    =         METAPARAMETER_INSTANCE_ID( Identifiers, previewPrefix                    )->DefaultValue();
   identifiers.processIconPrefix                =         METAPARAMETER_INSTANCE_ID( Identifiers, processIconPrefix                )->DefaultValue();
   identifiers.imageContainerIconPrefix         =         METAPARAMETER_INSTANCE_ID( Identifiers, imageContainerIconPrefix         )->DefaultValue();
   identifiers.newImageCaption                  =         METAPARAMETER_INSTANCE_ID( Identifiers, newImageCaption                  )->DefaultValue();
   identifiers.clonePostfix                     =         METAPARAMETER_INSTANCE_ID( Identifiers, clonePostfix                     )->DefaultValue();
   identifiers.noViewsAvailableText             =         METAPARAMETER_INSTANCE_ID( Identifiers, noViewsAvailableText             )->DefaultValue();
   identifiers.noViewSelectedText               =         METAPARAMETER_INSTANCE_ID( Identifiers, noViewSelectedText               )->DefaultValue();
   identifiers.noPreviewsAvailableText          =         METAPARAMETER_INSTANCE_ID( Identifiers, noPreviewsAvailableText          )->DefaultValue();
   identifiers.noPreviewSelectedText            =         METAPARAMETER_INSTANCE_ID( Identifiers, noPreviewSelectedText            )->DefaultValue();
   identifiers.brokenLinkText                   =         METAPARAMETER_INSTANCE_ID( Identifiers, brokenLinkText                   )->DefaultValue();

   process.enableParallelProcessing             =         METAPARAMETER_INSTANCE_ID( Process, enableParallelProcessing             )->DefaultValue();
   process.enableParallelCoreRendering          =         METAPARAMETER_INSTANCE_ID( Process, enableParallelCoreRendering          )->DefaultValue();
   process.enableParallelCoreColorManagement    =         METAPARAMETER_INSTANCE_ID( Process, enableParallelCoreColorManagement    )->DefaultValue();
   process.enableParallelModuleProcessing       =         METAPARAMETER_INSTANCE_ID( Process, enableParallelModuleProcessing       )->DefaultValue();
   process.enableThreadCPUAffinity              =         METAPARAMETER_INSTANCE_ID( Process, enableThreadCPUAffinity              )->DefaultValue();
   process.maxModuleThreadPriority              =  int32( METAPARAMETER_INSTANCE_ID( Process, maxModuleThreadPriority              )->DefaultValue() );
   process.maxProcessors                        =  int32( METAPARAMETER_INSTANCE_ID( Process, maxProcessors                        )->DefaultValue() );
   process.backupFiles                          =         METAPARAMETER_INSTANCE_ID( Process, backupFiles                          )->DefaultValue();
   process.generateScriptComments               =         METAPARAMETER_INSTANCE_ID( Process, generateScriptComments               )->DefaultValue();
   process.maxConsoleLines                      =  int32( METAPARAMETER_INSTANCE_ID( Process, maxConsoleLines                      )->DefaultValue() );
   process.consoleDelay                         =  int32( METAPARAMETER_INSTANCE_ID( Process, consoleDelay                         )->DefaultValue() );
   process.autoSavePSMPeriod                    =  int32( METAPARAMETER_INSTANCE_ID( Process, autoSavePSMPeriod                    )->DefaultValue() );
   process.alertOnProcessCompleted              =         METAPARAMETER_INSTANCE_ID( Process, alertOnProcessCompleted              )->DefaultValue();
}

void PreferencesInstance::LoadCurrentSettings()
{
   application.styleSheetFile                   = PixInsightSettings::GlobalString ( "Application/StyleSheetFile" );
   application.resourceFile01                   = PixInsightSettings::GlobalString ( "Application/ResourceFile01" );
   application.resourceFile02                   = PixInsightSettings::GlobalString ( "Application/ResourceFile02" );
   application.resourceFile03                   = PixInsightSettings::GlobalString ( "Application/ResourceFile03" );
   application.resourceFile04                   = PixInsightSettings::GlobalString ( "Application/ResourceFile04" );
   application.resourceFile05                   = PixInsightSettings::GlobalString ( "Application/ResourceFile05" );
   application.resourceFile06                   = PixInsightSettings::GlobalString ( "Application/ResourceFile06" );
   application.resourceFile07                   = PixInsightSettings::GlobalString ( "Application/ResourceFile07" );
   application.resourceFile08                   = PixInsightSettings::GlobalString ( "Application/ResourceFile08" );
   application.resourceFile09                   = PixInsightSettings::GlobalString ( "Application/ResourceFile09" );
   application.resourceFile10                   = PixInsightSettings::GlobalString ( "Application/ResourceFile10" );
   application.autoUIScaling                    = PixInsightSettings::GlobalFlag   ( "Application/AutoUIScaling" );
   application.uiScalingFactor                  = PixInsightSettings::GlobalReal   ( "Application/UIScalingFactor" );
   application.fontResolution                   = PixInsightSettings::GlobalInteger( "Application/FontResolution" );
   application.lowResFont                       = PixInsightSettings::GlobalString ( "Application/LowResFont" );
   application.highResFont                      = PixInsightSettings::GlobalString ( "Application/HighResFont" );
   application.lowResMonoFont                   = PixInsightSettings::GlobalString ( "Application/LowResMonoFont" );
   application.highResMonoFont                  = PixInsightSettings::GlobalString ( "Application/HighResMonoFont" );

   mainWindow.maximizeAtStartup                 = PixInsightSettings::GlobalFlag   ( "MainWindow/MaximizeAtStartup" );
   mainWindow.fullScreenAtStartup               = PixInsightSettings::GlobalFlag   ( "MainWindow/FullScreenAtStartup" );
   mainWindow.showSplashAtStartup               = PixInsightSettings::GlobalFlag   ( "MainWindow/ShowSplashAtStartup" );
   mainWindow.checkForUpdatesAtStartup          = PixInsightSettings::GlobalFlag   ( "MainWindow/CheckForUpdatesAtStartup" );
   mainWindow.confirmProgramTermination         = PixInsightSettings::GlobalFlag   ( "MainWindow/ConfirmProgramTermination" );
   mainWindow.acceptDroppedFiles                = PixInsightSettings::GlobalFlag   ( "MainWindow/AcceptDroppedFiles" );
   mainWindow.doubleClickLaunchesOpenDialog     = PixInsightSettings::GlobalFlag   ( "MainWindow/DoubleClickLaunchesOpenDialog" );
   mainWindow.hoverableAutoHideWindows          = PixInsightSettings::GlobalFlag   ( "MainWindow/HoverableAutoHideWindows" );
   mainWindow.desktopSettingsAware              = PixInsightSettings::GlobalFlag   ( "MainWindow/DesktopSettingsAware" );
   mainWindow.nativeMenuBar                     = PixInsightSettings::GlobalFlag   ( "MainWindow/NativeMenuBar" );
   mainWindow.capitalizedMenuBars               = PixInsightSettings::GlobalFlag   ( "MainWindow/CapitalizedMenuBars" );
   mainWindow.windowButtonsOnTheLeft            = PixInsightSettings::GlobalFlag   ( "MainWindow/WindowButtonsOnTheLeft" );
   mainWindow.translucentWindows                = PixInsightSettings::GlobalFlag   ( "MainWindow/TranslucentWindows" );
   mainWindow.translucentChildWindows           = PixInsightSettings::GlobalFlag   ( "MainWindow/TranslucentChildWindows" );
   mainWindow.fadeWindows                       = PixInsightSettings::GlobalFlag   ( "MainWindow/FadeWindows" );
   mainWindow.fadeAutoHideWindows               = PixInsightSettings::GlobalFlag   ( "MainWindow/FadeAutoHideWindows" );
   mainWindow.translucentAutoHideWindows        = PixInsightSettings::GlobalFlag   ( "MainWindow/TranslucentAutoHideWindows" );
   mainWindow.fadeWorkspaces                    = PixInsightSettings::GlobalFlag   ( "MainWindow/FadeWorkspaces" );
   mainWindow.fadeMenu                          = PixInsightSettings::GlobalFlag   ( "MainWindow/FadeMenu" );
   mainWindow.fadeToolTip                       = PixInsightSettings::GlobalFlag   ( "MainWindow/FadeToolTip" );
   mainWindow.explodeIcons                      = PixInsightSettings::GlobalFlag   ( "MainWindow/ExplodeIcons" );
   mainWindow.implodeIcons                      = PixInsightSettings::GlobalFlag   ( "MainWindow/ImplodeIcons" );
   mainWindow.animateWindows                    = PixInsightSettings::GlobalFlag   ( "MainWindow/AnimateWindows" );
   mainWindow.animateMenu                       = PixInsightSettings::GlobalFlag   ( "MainWindow/AnimateMenu" );
   mainWindow.animateCombo                      = PixInsightSettings::GlobalFlag   ( "MainWindow/AnimateCombo" );
   mainWindow.animateToolTip                    = PixInsightSettings::GlobalFlag   ( "MainWindow/AnimateToolTip" );
   mainWindow.animateToolBox                    = PixInsightSettings::GlobalFlag   ( "MainWindow/AnimateToolBox" );
   mainWindow.maxRecentFiles                    = PixInsightSettings::GlobalInteger( "MainWindow/MaxRecentFiles" );

   imageWindow.backupFiles                      = PixInsightSettings::GlobalFlag   ( "ImageWindow/BackupFiles" );
   imageWindow.defaultMasksShown                = PixInsightSettings::GlobalFlag   ( "ImageWindow/DefaultMasksShown" );
   imageWindow.defaultMaskMode                  = PixInsightSettings::GlobalInteger( "ImageWindow/DefaultMaskMode" );
   imageWindow.defaultTransparencyMode          = PixInsightSettings::GlobalInteger( "ImageWindow/DefaultTransparencyMode" );
   imageWindow.defaultHorizontalResolution      = PixInsightSettings::GlobalReal   ( "ImageWindow/DefaultHorizontalResolution" );
   imageWindow.defaultVerticalResolution        = PixInsightSettings::GlobalReal   ( "ImageWindow/DefaultVerticalResolution" );
   imageWindow.defaultMetricResolution          = PixInsightSettings::GlobalFlag   ( "ImageWindow/DefaultMetricResolution" );
   imageWindow.defaultEmbedThumbnails           = PixInsightSettings::GlobalFlag   ( "ImageWindow/DefaultEmbedThumbnails" );
   imageWindow.defaultEmbedProperties           = PixInsightSettings::GlobalFlag   ( "ImageWindow/DefaultEmbedProperties" );
   imageWindow.defaultFileExtension             = PixInsightSettings::GlobalString ( "ImageWindow/DefaultFileExtension" );
   imageWindow.nativeFileDialogs                = PixInsightSettings::GlobalFlag   ( "ImageWindow/NativeFileDialogs" );
   imageWindow.rememberFileOpenType             = PixInsightSettings::GlobalFlag   ( "ImageWindow/RememberFileOpenType" );
   imageWindow.rememberFileSaveType             = PixInsightSettings::GlobalFlag   ( "ImageWindow/RememberFileSaveType" );
   imageWindow.strictFileSaveMode               = PixInsightSettings::GlobalFlag   ( "ImageWindow/StrictFileSaveMode" );
   imageWindow.fileFormatWarnings               = PixInsightSettings::GlobalFlag   ( "ImageWindow/FileFormatWarnings" );
   imageWindow.useFileNamesAsImageIdentifiers   = PixInsightSettings::GlobalFlag   ( "ImageWindow/UseFileNamesAsImageIdentifiers" );
   imageWindow.cursorTolerance                  = PixInsightSettings::GlobalInteger( "ImageWindow/CursorTolerance" );
   imageWindow.touchEvents                      = PixInsightSettings::GlobalFlag   ( "ImageWindow/TouchEvents" );
   imageWindow.pinchSensitivity                 = PixInsightSettings::GlobalReal   ( "ImageWindow/PinchSensitivity" );
   imageWindow.fastScreenRenditions             = PixInsightSettings::GlobalFlag   ( "ImageWindow/FastScreenRenditions" );
   imageWindow.fastScreenRenditionThreshold     = PixInsightSettings::GlobalInteger( "ImageWindow/FastScreenRenditionThreshold" );
   imageWindow.highDPIRenditions                = PixInsightSettings::GlobalFlag   ( "ImageWindow/HighDPIRenditions" );
   imageWindow.default24BitScreenLUT            = PixInsightSettings::GlobalFlag   ( "ImageWindow/Default24BitScreenLUT" );
   imageWindow.swapDirectories                  = ImageWindow::SwapDirectories();
   imageWindow.swapCompression                  = PixInsightSettings::GlobalFlag   ( "ImageWindow/SwapCompression" );
   imageWindow.downloadsDirectory               = PixInsightSettings::GlobalString ( "ImageWindow/DownloadsDirectory" );
   imageWindow.followDownloadLocations          = PixInsightSettings::GlobalFlag   ( "ImageWindow/FollowDownloadLocations" );
   imageWindow.verboseNetworkOperations         = PixInsightSettings::GlobalFlag   ( "ImageWindow/VerboseNetworkOperations" );
   imageWindow.showCaptionCurrentChannels       = PixInsightSettings::GlobalFlag   ( "ImageWindow/ShowCaptionCurrentChannels" );
   imageWindow.showCaptionZoomRatios            = PixInsightSettings::GlobalFlag   ( "ImageWindow/ShowCaptionZoomRatios" );
   imageWindow.showCaptionIdentifiers           = PixInsightSettings::GlobalFlag   ( "ImageWindow/ShowCaptionIdentifiers" );
   imageWindow.showCaptionFullPaths             = PixInsightSettings::GlobalFlag   ( "ImageWindow/ShowCaptionFullPaths" );
   imageWindow.showActiveSTFIndicators          = PixInsightSettings::GlobalFlag   ( "ImageWindow/ShowActiveSTFIndicators" );
   imageWindow.transparencyBrush                = PixInsightSettings::GlobalInteger( "TransparencyBrush/Brush" );
   imageWindow.transparencyBrushForegroundColor = PixInsightSettings::GlobalColor  ( "TransparencyBrush/ForegroundColor" );
   imageWindow.transparencyBrushBackgroundColor = PixInsightSettings::GlobalColor  ( "TransparencyBrush/BackgroundColor" );
   imageWindow.defaultTransparencyColor         = PixInsightSettings::GlobalColor  ( "TransparencyBrush/DefaultColor" );

   identifiers.workspacePrefix                  = PixInsightSettings::GlobalString ( "Workspace/Prefix" );
   identifiers.imagePrefix                      = PixInsightSettings::GlobalString ( "ImageWindow/Prefix" );
   identifiers.previewPrefix                    = PixInsightSettings::GlobalString ( "Preview/Prefix" );
   identifiers.processIconPrefix                = PixInsightSettings::GlobalString ( "ProcessIcon/Prefix" );
   identifiers.imageContainerIconPrefix         = PixInsightSettings::GlobalString ( "ImageContainerIcon/Prefix" );
   identifiers.newImageCaption                  = PixInsightSettings::GlobalString ( "ImageWindow/NewImageCaption" );
   identifiers.clonePostfix                     = PixInsightSettings::GlobalString ( "ImageWindow/ClonePostfix" );
   identifiers.noViewsAvailableText             = PixInsightSettings::GlobalString ( "ViewList/NoViewsAvailableText" );
   identifiers.noViewSelectedText               = PixInsightSettings::GlobalString ( "ViewList/NoViewSelectedText" );
   identifiers.noPreviewsAvailableText          = PixInsightSettings::GlobalString ( "ViewList/NoPreviewsAvailableText" );
   identifiers.noPreviewSelectedText            = PixInsightSettings::GlobalString ( "ViewList/NoPreviewSelectedText" );
   identifiers.brokenLinkText                   = PixInsightSettings::GlobalString ( "View/BrokenLinkText" );

   process.enableParallelProcessing             = PixInsightSettings::GlobalFlag   ( "Process/EnableParallelProcessing" );
   process.enableParallelCoreRendering          = PixInsightSettings::GlobalFlag   ( "Process/EnableParallelCoreRendering" );
   process.enableParallelCoreColorManagement    = PixInsightSettings::GlobalFlag   ( "Process/EnableParallelCoreColorManagement" );
   process.enableParallelModuleProcessing       = PixInsightSettings::GlobalFlag   ( "Process/EnableParallelModuleProcessing" );
   process.enableThreadCPUAffinity              = PixInsightSettings::GlobalFlag   ( "Process/EnableThreadCPUAffinity" );
   process.maxModuleThreadPriority              = PixInsightSettings::GlobalInteger( "Process/MaxModuleThreadPriority" );
   process.maxProcessors                        = PixInsightSettings::GlobalInteger( "Process/MaxProcessors" );
   process.backupFiles                          = PixInsightSettings::GlobalFlag   ( "Process/BackupFiles" );
   process.generateScriptComments               = PixInsightSettings::GlobalFlag   ( "Process/GenerateScriptComments" );
   process.maxConsoleLines                      = PixInsightSettings::GlobalInteger( "Process/MaxConsoleLines" );
   process.consoleDelay                         = PixInsightSettings::GlobalInteger( "Process/ConsoleDelay" );
   process.autoSavePSMPeriod                    = PixInsightSettings::GlobalInteger( "Process/AutoSavePSMPeriod" );
   process.alertOnProcessCompleted              = PixInsightSettings::GlobalFlag   ( "Process/AlertOnProcessCompleted" );
}

// ----------------------------------------------------------------------------

String* PreferencesInstance::StringParameterFromMetaParameter( const MetaParameter* p )
{
   String* s = 0;

   if ( p == METAPARAMETER_INSTANCE_ID( Application, styleSheetFile ) )
      s = &application.styleSheetFile;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile01 ) )
      s = &application.resourceFile01;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile02 ) )
      s = &application.resourceFile02;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile03 ) )
      s = &application.resourceFile03;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile04 ) )
      s = &application.resourceFile04;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile05 ) )
      s = &application.resourceFile05;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile06 ) )
      s = &application.resourceFile06;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile07 ) )
      s = &application.resourceFile07;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile08 ) )
      s = &application.resourceFile08;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile09 ) )
      s = &application.resourceFile09;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, resourceFile10 ) )
      s = &application.resourceFile10;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, lowResFont ) )
      s = &application.lowResFont;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, highResFont ) )
      s = &application.highResFont;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, lowResMonoFont ) )
      s = &application.lowResMonoFont;
   else if ( p == METAPARAMETER_INSTANCE_ID( Application, highResMonoFont ) )
      s = &application.highResMonoFont;

   else if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, defaultFileExtension ) )
      s = &imageWindow.defaultFileExtension;
   else if ( p == METAPARAMETER_INSTANCE_ID( ImageWindow, downloadsDirectory ) )
      s = &imageWindow.downloadsDirectory;

   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, workspacePrefix ) )
      s = &identifiers.workspacePrefix;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, imagePrefix ) )
      s = &identifiers.imagePrefix;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, previewPrefix ) )
      s = &identifiers.previewPrefix;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, processIconPrefix ) )
      s = &identifiers.processIconPrefix;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, imageContainerIconPrefix ) )
      s = &identifiers.imageContainerIconPrefix;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, newImageCaption ) )
      s = &identifiers.newImageCaption;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, clonePostfix ) )
      s = &identifiers.clonePostfix;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noViewsAvailableText ) )
      s = &identifiers.noViewsAvailableText;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noViewSelectedText ) )
      s = &identifiers.noViewSelectedText;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noPreviewsAvailableText ) )
      s = &identifiers.noPreviewsAvailableText;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, noPreviewSelectedText ) )
      s = &identifiers.noPreviewSelectedText;
   else if ( p == METAPARAMETER_INSTANCE_ID( Identifiers, brokenLinkText ) )
      s = &identifiers.brokenLinkText;
   return s;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PreferencesInstance.cpp - Released 2015/11/26 16:00:12 UTC
