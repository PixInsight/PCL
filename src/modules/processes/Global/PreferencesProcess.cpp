//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0366
// ----------------------------------------------------------------------------
// PreferencesProcess.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "PreferencesProcess.h"
#include "PreferencesParameters.h"
#include "PreferencesInstance.h"
#include "PreferencesInterface.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PreferencesProcess* ThePreferencesProcess = 0;

// ----------------------------------------------------------------------------

#include "PreferencesIcon.xpm"

// ----------------------------------------------------------------------------

PreferencesProcess::PreferencesProcess() : MetaProcess()
{
   ThePreferencesProcess = this;

   // -------------------------------------------------------------------------

   new METAPARAMETER_ID( Application, styleSheetFile )( this );
   new METAPARAMETER_ID( Application, resourceFile01 )( this );
   new METAPARAMETER_ID( Application, resourceFile02 )( this );
   new METAPARAMETER_ID( Application, resourceFile03 )( this );
   new METAPARAMETER_ID( Application, resourceFile04 )( this );
   new METAPARAMETER_ID( Application, resourceFile05 )( this );
   new METAPARAMETER_ID( Application, resourceFile06 )( this );
   new METAPARAMETER_ID( Application, resourceFile07 )( this );
   new METAPARAMETER_ID( Application, resourceFile08 )( this );
   new METAPARAMETER_ID( Application, resourceFile09 )( this );
   new METAPARAMETER_ID( Application, resourceFile10 )( this );
   new METAPARAMETER_ID( Application, autoUIScaling )( this );
   new METAPARAMETER_ID( Application, uiScalingFactor )( this );
   new METAPARAMETER_ID( Application, fontResolution )( this );
   new METAPARAMETER_ID( Application, lowResFont )( this );
   new METAPARAMETER_ID( Application, highResFont )( this );
   new METAPARAMETER_ID( Application, lowResMonoFont )( this );
   new METAPARAMETER_ID( Application, highResMonoFont )( this );

   // -------------------------------------------------------------------------

   new METAPARAMETER_ID( MainWindow,  maximizeAtStartup )( this );
   new METAPARAMETER_ID( MainWindow,  fullScreenAtStartup )( this );
   new METAPARAMETER_ID( MainWindow,  showSplashAtStartup )( this );
   new METAPARAMETER_ID( MainWindow,  checkForUpdatesAtStartup )( this );
   new METAPARAMETER_ID( MainWindow,  confirmProgramTermination )( this );
   new METAPARAMETER_ID( MainWindow,  acceptDroppedFiles )( this );
   new METAPARAMETER_ID( MainWindow,  doubleClickLaunchesOpenDialog )( this );
   new METAPARAMETER_ID( MainWindow,  hoverableAutoHideWindows )( this );
   new METAPARAMETER_ID( MainWindow,  desktopSettingsAware )( this );
   new METAPARAMETER_ID( MainWindow,  nativeMenuBar )( this );
   new METAPARAMETER_ID( MainWindow,  capitalizedMenuBars )( this );
   new METAPARAMETER_ID( MainWindow,  windowButtonsOnTheLeft )( this );
   new METAPARAMETER_ID( MainWindow,  translucentWindows )( this );
   new METAPARAMETER_ID( MainWindow,  translucentChildWindows )( this );
   new METAPARAMETER_ID( MainWindow,  fadeWindows )( this );
   new METAPARAMETER_ID( MainWindow,  fadeAutoHideWindows )( this );
   new METAPARAMETER_ID( MainWindow,  translucentAutoHideWindows )( this );
   new METAPARAMETER_ID( MainWindow,  fadeWorkspaces )( this );
   new METAPARAMETER_ID( MainWindow,  fadeMenu )( this );
   new METAPARAMETER_ID( MainWindow,  fadeToolTip )( this );
   new METAPARAMETER_ID( MainWindow,  explodeIcons )( this );
   new METAPARAMETER_ID( MainWindow,  implodeIcons )( this );
   new METAPARAMETER_ID( MainWindow,  animateWindows )( this );
   new METAPARAMETER_ID( MainWindow,  animateMenu )( this );
   new METAPARAMETER_ID( MainWindow,  animateCombo )( this );
   new METAPARAMETER_ID( MainWindow,  animateToolTip )( this );
   new METAPARAMETER_ID( MainWindow,  animateToolBox )( this );
   new METAPARAMETER_ID( MainWindow,  maxRecentFiles )( this );
   new METAPARAMETER_ID( MainWindow,  showRecentlyUsed )( this );
   new METAPARAMETER_ID( MainWindow,  showMostUsed )( this );
   new METAPARAMETER_ID( MainWindow,  showFavorites )( this );
   new METAPARAMETER_ID( MainWindow,  maxUsageListLength )( this );
   new METAPARAMETER_ID( MainWindow,  expandRecentlyUsedAtStartup )( this );
   new METAPARAMETER_ID( MainWindow,  expandMostUsedAtStartup )( this );
   new METAPARAMETER_ID( MainWindow,  expandFavoritesAtStartup )( this );
   new METAPARAMETER_ID( MainWindow,  openURLsWithInternalBrowser )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile01 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile02 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile03 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile04 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile05 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile06 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile07 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile08 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile09 )( this );
   new METAPARAMETER_ID( MainWindow,  wallpaperFile10 )( this );
   new METAPARAMETER_ID( MainWindow,  useWallpapers )( this );

   // -------------------------------------------------------------------------

   new METAPARAMETER_ID( ImageWindow, backupFiles )( this );
   new METAPARAMETER_ID( ImageWindow, defaultMasksShown )( this );
   new METAPARAMETER_ID( ImageWindow, defaultMaskMode )( this );
   new METAPARAMETER_ID( ImageWindow, defaultTransparencyMode )( this );
   new METAPARAMETER_ID( ImageWindow, defaultHorizontalResolution )( this );
   new METAPARAMETER_ID( ImageWindow, defaultVerticalResolution )( this );
   new METAPARAMETER_ID( ImageWindow, defaultMetricResolution )( this );
   new METAPARAMETER_ID( ImageWindow, defaultEmbedThumbnails )( this );
   new METAPARAMETER_ID( ImageWindow, defaultEmbedProperties )( this );
   new METAPARAMETER_ID( ImageWindow, defaultFileExtension )( this );
   new METAPARAMETER_ID( ImageWindow, nativeFileDialogs )( this );
   new METAPARAMETER_ID( ImageWindow, rememberFileOpenType )( this );
   new METAPARAMETER_ID( ImageWindow, rememberFileSaveType )( this );
   new METAPARAMETER_ID( ImageWindow, strictFileSaveMode )( this );
   new METAPARAMETER_ID( ImageWindow, fileFormatWarnings )( this );
   new METAPARAMETER_ID( ImageWindow, useFileNamesAsImageIdentifiers )( this );
   new METAPARAMETER_ID( ImageWindow, cursorTolerance )( this );
   new METAPARAMETER_ID( ImageWindow, wheelStepAngle )( this );
   new METAPARAMETER_ID( ImageWindow, wheelDirection )( this );
   new METAPARAMETER_ID( ImageWindow, touchEvents )( this );
   new METAPARAMETER_ID( ImageWindow, pinchSensitivity )( this );
   new METAPARAMETER_ID( ImageWindow, fastScreenRenditions )( this );
   new METAPARAMETER_ID( ImageWindow, fastScreenRenditionThreshold )( this );
   new METAPARAMETER_ID( ImageWindow, highDPIRenditions )( this );
   new METAPARAMETER_ID( ImageWindow, default24BitScreenLUT )( this );
   new METAPARAMETER_ID( ImageWindow, createPreviewsFromCoreProperties )( this );
   new METAPARAMETER_ID( ImageWindow, swapDirectories )( this );
   new METAPARAMETER_ID( ImageWindow, swapDirectory )( METAPARAMETER_INSTANCE_ID( ImageWindow, swapDirectories ) );
   new METAPARAMETER_ID( ImageWindow, swapCompression )( this );
   new METAPARAMETER_ID( ImageWindow, downloadsDirectory )( this );
   new METAPARAMETER_ID( ImageWindow, followDownloadLocations )( this );
   new METAPARAMETER_ID( ImageWindow, verboseNetworkOperations )( this );
   new METAPARAMETER_ID( ImageWindow, showCaptionCurrentChannels )( this );
   new METAPARAMETER_ID( ImageWindow, showCaptionZoomRatios )( this );
   new METAPARAMETER_ID( ImageWindow, showCaptionIdentifiers )( this );
   new METAPARAMETER_ID( ImageWindow, showCaptionFullPaths )( this );
   new METAPARAMETER_ID( ImageWindow, showActiveSTFIndicators )( this );
   new METAPARAMETER_ID( ImageWindow, transparencyBrush )( this );
   new METAPARAMETER_ID( ImageWindow, transparencyBrushForegroundColor )( this );
   new METAPARAMETER_ID( ImageWindow, transparencyBrushBackgroundColor )( this );
   new METAPARAMETER_ID( ImageWindow, defaultTransparencyColor )( this );

   // -------------------------------------------------------------------------

   new METAPARAMETER_ID( Identifiers, imagePrefix )( this );
   new METAPARAMETER_ID( Identifiers, workspacePrefix )( this );
   new METAPARAMETER_ID( Identifiers, previewPrefix )( this );
   new METAPARAMETER_ID( Identifiers, processIconPrefix )( this );
   new METAPARAMETER_ID( Identifiers, imageContainerIconPrefix )( this );
   new METAPARAMETER_ID( Identifiers, newImageCaption )( this );
   new METAPARAMETER_ID( Identifiers, clonePostfix )( this );
   new METAPARAMETER_ID( Identifiers, noViewsAvailableText )( this );
   new METAPARAMETER_ID( Identifiers, noViewSelectedText )( this );
   new METAPARAMETER_ID( Identifiers, noPreviewsAvailableText )( this );
   new METAPARAMETER_ID( Identifiers, noPreviewSelectedText )( this );
   new METAPARAMETER_ID( Identifiers, brokenLinkText )( this );

   // -------------------------------------------------------------------------

   new METAPARAMETER_ID( Process, enableParallelProcessing )( this );
   new METAPARAMETER_ID( Process, enableParallelCoreRendering )( this );
   new METAPARAMETER_ID( Process, enableParallelCoreColorManagement )( this );
   new METAPARAMETER_ID( Process, enableParallelModuleProcessing )( this );
   new METAPARAMETER_ID( Process, enableThreadCPUAffinity )( this );
   new METAPARAMETER_ID( Process, maxModuleThreadPriority )( this );
   new METAPARAMETER_ID( Process, maxProcessors )( this );
   new METAPARAMETER_ID( Process, backupFiles )( this );
   new METAPARAMETER_ID( Process, generateScriptComments )( this );
   new METAPARAMETER_ID( Process, maxConsoleLines )( this );
   new METAPARAMETER_ID( Process, consoleDelay )( this );
   new METAPARAMETER_ID( Process, autoSavePSMPeriod )( this );
   new METAPARAMETER_ID( Process, alertOnProcessCompleted )( this );
   new METAPARAMETER_ID( Process, enableExecutionStatistics )( this );
   new METAPARAMETER_ID( Process, enableLaunchStatistics )( this );
}

// ----------------------------------------------------------------------------

IsoString PreferencesProcess::Id() const
{
   return "Preferences";
}

// ----------------------------------------------------------------------------

IsoString PreferencesProcess::Category() const
{
   return "Global";
}

// ----------------------------------------------------------------------------

uint32 PreferencesProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String PreferencesProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** PreferencesProcess::IconImageXPM() const
{
   return PreferencesIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* PreferencesProcess::DefaultInterface() const
{
   return ThePreferencesInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* PreferencesProcess::Create() const
{
   return new PreferencesInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* PreferencesProcess::Clone( const ProcessImplementation& p ) const
{
   const PreferencesInstance* instPtr = dynamic_cast<const PreferencesInstance*>( &p );
   return (instPtr != 0) ? new PreferencesInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool PreferencesProcess::PrefersGlobalExecution() const
{
   return true; // this is a global process
}

// ----------------------------------------------------------------------------

bool PreferencesProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Preferences [<arg_list>]"
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int PreferencesProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   PreferencesInstance instance( this );
   bool launchInterface = false;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
         throw Error( "Invalid non-parametric argument: " + arg.Token() );
   }

   if ( launchInterface || arguments.IsEmpty() )
      instance.LaunchInterface();
   else
      instance.LaunchGlobal();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PreferencesProcess.cpp - Released 2017-07-09T18:07:33Z
