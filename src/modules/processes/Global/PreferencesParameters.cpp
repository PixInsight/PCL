//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0347
// ----------------------------------------------------------------------------
// PreferencesParameters.cpp - Released 2017-05-02T09:43:00Z
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

#include "PreferencesParameters.h"

#include <pcl/Font.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IMPLEMENT_STRING_METAPARAMETER  ( Application, styleSheetFile )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile01 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile02 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile03 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile04 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile05 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile06 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile07 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile08 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile09 )
IMPLEMENT_STRING_METAPARAMETER  ( Application, resourceFile10 )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Application, autoUIScaling )
IMPLEMENT_DOUBLE_METAPARAMETER  ( Application, uiScalingFactor )
IMPLEMENT_INT32_METAPARAMETER   ( Application, fontResolution )
IMPLEMENT_STRING_METAPARAMETER  ( Application, lowResFont )
IMPLEMENT_STRING_METAPARAMETER  ( Application, highResFont )
IMPLEMENT_STRING_METAPARAMETER  ( Application, lowResMonoFont )
IMPLEMENT_STRING_METAPARAMETER  ( Application, highResMonoFont )

// ----------------------------------------------------------------------------

IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  maximizeAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  fullScreenAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  showSplashAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  checkForUpdatesAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  confirmProgramTermination )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  acceptDroppedFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  doubleClickLaunchesOpenDialog )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  hoverableAutoHideWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  desktopSettingsAware )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  nativeMenuBar )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  capitalizedMenuBars )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  windowButtonsOnTheLeft )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  translucentWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  translucentChildWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  fadeWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  fadeAutoHideWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  translucentAutoHideWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  fadeWorkspaces )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  fadeMenu )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  fadeToolTip )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  explodeIcons )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  implodeIcons )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  animateWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  animateMenu )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  animateCombo )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  animateToolTip )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  animateToolBox )
IMPLEMENT_INT32_METAPARAMETER   ( MainWindow,  maxRecentFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  showRecentlyUsed )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  showMostUsed )
IMPLEMENT_INT32_METAPARAMETER   ( MainWindow,  maxUsageListLength )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  expandUsageItemsAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER ( MainWindow,  openURLsWithInternalBrowser )

// ----------------------------------------------------------------------------

IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, backupFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, defaultMasksShown )
IMPLEMENT_INT32_METAPARAMETER   ( ImageWindow, defaultMaskMode )
IMPLEMENT_INT32_METAPARAMETER   ( ImageWindow, defaultTransparencyMode )
IMPLEMENT_DOUBLE_METAPARAMETER  ( ImageWindow, defaultHorizontalResolution )
IMPLEMENT_DOUBLE_METAPARAMETER  ( ImageWindow, defaultVerticalResolution )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, defaultMetricResolution )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, defaultEmbedThumbnails )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, defaultEmbedProperties )
IMPLEMENT_STRING_METAPARAMETER  ( ImageWindow, defaultFileExtension )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, nativeFileDialogs )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, rememberFileOpenType )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, rememberFileSaveType )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, strictFileSaveMode )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, fileFormatWarnings )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, useFileNamesAsImageIdentifiers )
IMPLEMENT_INT32_METAPARAMETER   ( ImageWindow, cursorTolerance )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, touchEvents )
IMPLEMENT_DOUBLE_METAPARAMETER  ( ImageWindow, pinchSensitivity )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, fastScreenRenditions )
IMPLEMENT_INT32_METAPARAMETER   ( ImageWindow, fastScreenRenditionThreshold )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, highDPIRenditions );
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, default24BitScreenLUT )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, createPreviewsFromCoreProperties )
IMPLEMENT_TABLE_METAPARAMETER   ( ImageWindow, swapDirectories )
IMPLEMENT_ROW_STRING_METAPARAMETER( ImageWindow, swapDirectory )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, swapCompression )
IMPLEMENT_STRING_METAPARAMETER  ( ImageWindow, downloadsDirectory )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, followDownloadLocations )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, verboseNetworkOperations )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionCurrentChannels )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionZoomRatios )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionIdentifiers )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionFullPaths )
IMPLEMENT_BOOLEAN_METAPARAMETER ( ImageWindow, showActiveSTFIndicators )
IMPLEMENT_INT32_METAPARAMETER   ( ImageWindow, transparencyBrush )
IMPLEMENT_COLOR_METAPARAMETER   ( ImageWindow, transparencyBrushForegroundColor )
IMPLEMENT_COLOR_METAPARAMETER   ( ImageWindow, transparencyBrushBackgroundColor )
IMPLEMENT_COLOR_METAPARAMETER   ( ImageWindow, defaultTransparencyColor )

// ----------------------------------------------------------------------------

IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, workspacePrefix )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, imagePrefix )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, previewPrefix )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, processIconPrefix )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, imageContainerIconPrefix )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, newImageCaption )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, clonePostfix )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, noViewsAvailableText )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, noViewSelectedText )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, noPreviewsAvailableText )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, noPreviewSelectedText )
IMPLEMENT_STRING_METAPARAMETER  ( Identifiers, brokenLinkText )

// ----------------------------------------------------------------------------

IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, enableParallelProcessing )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, enableParallelCoreRendering )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, enableParallelCoreColorManagement )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, enableParallelModuleProcessing )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, enableThreadCPUAffinity )
IMPLEMENT_INT32_METAPARAMETER   ( Process, maxModuleThreadPriority )
IMPLEMENT_INT32_METAPARAMETER   ( Process, maxProcessors )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, backupFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, generateScriptComments )
IMPLEMENT_INT32_METAPARAMETER   ( Process, maxConsoleLines )
IMPLEMENT_INT32_METAPARAMETER   ( Process, consoleDelay )
IMPLEMENT_INT32_METAPARAMETER   ( Process, autoSavePSMPeriod )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, alertOnProcessCompleted )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, enableExecutionStatistics )
IMPLEMENT_BOOLEAN_METAPARAMETER ( Process, enableLaunchStatistics )

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PreferencesParameters.cpp - Released 2017-05-02T09:43:00Z
