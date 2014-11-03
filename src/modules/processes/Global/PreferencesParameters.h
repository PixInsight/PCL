// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Global Process Module Version 01.02.05.0258
// ****************************************************************************
// PreferencesParameters.h - Released 2014/10/29 07:34:56 UTC
// ****************************************************************************
// This file is part of the standard Global PixInsight module.
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

#ifndef __PreferencesParameters_h
#define __PreferencesParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/ImageWindow.h> // for pcl::BackgroundBrush
#include <pcl/Thread.h> // for pcl::ThreadPriority
#include <pcl/Color.h>

//

#define METAPARAMETER_ID( prefix, param )          prefix##_##param
#define METAPARAMETER_ID_STR( prefix, param )      #prefix "_"  #param
#define METAPARAMETER_CLASS_ID( prefix, param )    prefix##_##param
#define METAPARAMETER_INSTANCE_ID( prefix, param ) The##prefix##_##param##Parameter

//

#define DECLARE_METAPARAMETER_INSTANCE( prefix, param ) \
extern METAPARAMETER_CLASS_ID( prefix, param )*  METAPARAMETER_INSTANCE_ID( prefix, param );

#define IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param ) \
METAPARAMETER_CLASS_ID( prefix, param )*         METAPARAMETER_INSTANCE_ID( prefix, param ) = 0;

//

#define DECLARE_BOOLEAN_METAPARAMETER( prefix, param, defaultValue )                      \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaBoolean                        \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* );                               \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
   virtual bool DefaultValue() const { return defaultValue; }                             \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_BOOLEAN_METAPARAMETER( prefix, param )                                           \
IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param )                                                  \
METAPARAMETER_CLASS_ID( prefix, param )::METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* P ) \
: MetaBoolean( P ) { METAPARAMETER_INSTANCE_ID( prefix, param ) = this; }

//

#define DECLARE_INT32_METAPARAMETER( prefix, param, defaultValue, minValue, maxValue )    \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaInt32                          \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* );                               \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
   virtual double DefaultValue() const { return defaultValue; }                           \
   virtual double MinimumValue() const { return minValue; }                               \
   virtual double MaximumValue() const { return maxValue; }                               \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_INT32_METAPARAMETER( prefix, param )                                             \
IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param )                                                  \
METAPARAMETER_CLASS_ID( prefix, param )::METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* P ) \
: MetaInt32( P ) { METAPARAMETER_INSTANCE_ID( prefix, param ) = this; }

//

#define DECLARE_UINT32_METAPARAMETER( prefix, param, defaultValue, minValue, maxValue )   \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaUInt32                         \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* );                               \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
   virtual double DefaultValue() const { return defaultValue; }                           \
   virtual double MinimumValue() const { return minValue; }                               \
   virtual double MaximumValue() const { return maxValue; }                               \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_UINT32_METAPARAMETER( prefix, param )                                            \
IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param )                                                  \
METAPARAMETER_CLASS_ID( prefix, param )::METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* P ) \
: MetaUInt32( P ) { METAPARAMETER_INSTANCE_ID( prefix, param ) = this; }

//

#define DECLARE_COLOR_METAPARAMETER( prefix, param, defaultValue )                        \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaUInt32                         \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* );                               \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
   virtual double DefaultValue() const { return defaultValue; }                           \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_COLOR_METAPARAMETER( prefix, param ) \
IMPLEMENT_UINT32_METAPARAMETER( prefix, param )

//

#define DECLARE_DOUBLE_METAPARAMETER( prefix, param, defaultValue, minValue, maxValue )   \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaDouble                         \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* );                               \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
   virtual double DefaultValue() const { return defaultValue; }                           \
   virtual double MinimumValue() const { return minValue; }                               \
   virtual double MaximumValue() const { return maxValue; }                               \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_DOUBLE_METAPARAMETER( prefix, param )                                            \
IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param )                                                  \
METAPARAMETER_CLASS_ID( prefix, param )::METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* P ) \
: MetaDouble( P ) { METAPARAMETER_INSTANCE_ID( prefix, param ) = this; }

//

#define DECLARE_STRING_METAPARAMETER( prefix, param, defaultValue )                       \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaString                         \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* );                               \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
   virtual String DefaultValue() const { return defaultValue; }                           \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_STRING_METAPARAMETER( prefix, param )                                            \
IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param )                                                  \
METAPARAMETER_CLASS_ID( prefix, param )::METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* P ) \
: MetaString( P ) { METAPARAMETER_INSTANCE_ID( prefix, param ) = this; }

//

#define DECLARE_TABLE_METAPARAMETER( prefix, param )                                      \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaTable                          \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* );                               \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_TABLE_METAPARAMETER( prefix, param )                                             \
IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param )                                                  \
METAPARAMETER_CLASS_ID( prefix, param )::METAPARAMETER_CLASS_ID( prefix, param )( MetaProcess* P ) \
: MetaTable( P ) { METAPARAMETER_INSTANCE_ID( prefix, param ) = this; }

//

#define DECLARE_ROW_STRING_METAPARAMETER( prefix, param, defaultValue )                   \
class METAPARAMETER_CLASS_ID( prefix, param ) : public MetaString                         \
{                                                                                         \
public:                                                                                   \
   METAPARAMETER_CLASS_ID( prefix, param )( MetaTable* );                                 \
   virtual IsoString Id() const { return METAPARAMETER_ID_STR( prefix, param ); }         \
   virtual String DefaultValue() const { return defaultValue; }                           \
};                                                                                        \
DECLARE_METAPARAMETER_INSTANCE( prefix, param )

#define IMPLEMENT_ROW_STRING_METAPARAMETER( prefix, param )                                        \
IMPLEMENT_METAPARAMETER_INSTANCE( prefix, param )                                                  \
METAPARAMETER_CLASS_ID( prefix, param )::METAPARAMETER_CLASS_ID( prefix, param )( MetaTable* T )   \
: MetaString( T ) { METAPARAMETER_INSTANCE_ID( prefix, param ) = this; }

//

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

#ifdef __PCL_WINDOWS
// Interestingly, MSVC++ is unable to handle conditional preprocessor
// directives within macro definitions... g++ does allow them, see below.
DECLARE_STRING_METAPARAMETER  ( Application, styleSheetFile, String( "win.qss" ) )
#else
DECLARE_STRING_METAPARAMETER  ( Application, styleSheetFile,
#ifdef __PCL_X11
         String( "x11.qss" )
#endif
#ifdef __PCL_MACOSX
         String( "osx.qss" )
#endif
                              )
#endif

DECLARE_STRING_METAPARAMETER  ( Application, resourceFile01,                   "core-icons.rcc" )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile02,                   "core-compat-1.7.rcc" )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile03,                   String() )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile04,                   String() )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile05,                   String() )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile06,                   String() )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile07,                   String() )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile08,                   String() )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile09,                   String() )
DECLARE_STRING_METAPARAMETER  ( Application, resourceFile10,                   String() )

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  maximizeAtStartup,                true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  fullScreenAtStartup,              false )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  showSplashAtStartup,              true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  checkForUpdatesAtStartup,         true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  confirmProgramTermination,        false )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  acceptDroppedFiles,               true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  doubleClickLaunchesOpenDialog,    true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  hoverableAutoHideWindows,         true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  desktopSettingsAware,             true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  nativeMenuBar,                    true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  capitalizedMenuBars,              true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  translucentWindows,               true )
#ifdef __PCL_X11
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  translucentChildWindows,          true )
#else
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  translucentChildWindows,          false )
#endif
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  fadeWindows,                      true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  fadeAutoHideWindows,              true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  fadeWorkspaces,                   true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  fadeMenu,                         true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  fadeToolTip,                      true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  explodeIcons,                     true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  implodeIcons,                     true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  animateWindows,                   true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  animateMenu,                      true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  animateCombo,                     true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  animateToolTip,                   true )
DECLARE_BOOLEAN_METAPARAMETER ( MainWindow,  animateToolBox,                   true )
DECLARE_INT32_METAPARAMETER   ( MainWindow,  maxRecentFiles,                   32, 2, 128 )

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, backupFiles,                      false )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, defaultMasksShown,                true )
DECLARE_INT32_METAPARAMETER   ( ImageWindow, defaultMaskMode,                  MaskMode::Default, 0, MaskMode::NumberOfModes-1 )
DECLARE_INT32_METAPARAMETER   ( ImageWindow, defaultTransparencyMode,          TransparencyMode::Default, 0, TransparencyMode::NumberOfModes-1 )
DECLARE_DOUBLE_METAPARAMETER  ( ImageWindow, defaultHorizontalResolution,      72, 1, 10000 )
DECLARE_DOUBLE_METAPARAMETER  ( ImageWindow, defaultVerticalResolution,        72, 1, 10000 )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, defaultMetricResolution,          false )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, defaultEmbedMetadata,             true )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, defaultEmbedThumbnails,           false )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, defaultEmbedProperties,           true )
DECLARE_STRING_METAPARAMETER  ( ImageWindow, defaultFileExtension,             ".fit" )
#ifdef __PCL_X11
/*
 * ### FIXME: Native KDE file dialogs are causing problems, especially an
 * ill-posed interaction with ScriptEditor, leading to GUI corruption.
 */
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, nativeFileDialogs,                false )
#else
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, nativeFileDialogs,                true )
#endif
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, rememberFileOpenType,             false )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, rememberFileSaveType,             true )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, strictFileSaveMode,               true )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, useFileNamesAsImageIdentifiers,   true )
DECLARE_INT32_METAPARAMETER   ( ImageWindow, cursorTolerance,                  3, 1, 16 )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, touchEvents,                      true )
DECLARE_DOUBLE_METAPARAMETER  ( ImageWindow, pinchSensitivity,                 0.050, 0, 1 )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, fastScreenRenditions,             true )
DECLARE_INT32_METAPARAMETER   ( ImageWindow, fastScreenRenditionThreshold,     6, 0, 64 )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, default24BitScreenLUT,            false )
DECLARE_TABLE_METAPARAMETER   ( ImageWindow, swapDirectories )
DECLARE_ROW_STRING_METAPARAMETER( ImageWindow, swapDirectory,                  String() )
DECLARE_STRING_METAPARAMETER  ( ImageWindow, downloadsDirectory,               String() )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, followDownloadLocations,          false )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, verboseNetworkOperations,         false )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionCurrentChannels,       true )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionZoomRatios,            true )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionIdentifiers,           true )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, showCaptionFullPaths,             false )
DECLARE_BOOLEAN_METAPARAMETER ( ImageWindow, showActiveSTFIndicators,          true )
DECLARE_INT32_METAPARAMETER   ( ImageWindow, transparencyBrush,                BackgroundBrush::Default, 0, BackgroundBrush::NumberOfBrushes-1 )
DECLARE_COLOR_METAPARAMETER   ( ImageWindow, transparencyBrushForegroundColor, RGBAColor( 204, 204, 204, 255 ) )
DECLARE_COLOR_METAPARAMETER   ( ImageWindow, transparencyBrushBackgroundColor, RGBAColor( 255, 255, 255, 255 ) )
DECLARE_COLOR_METAPARAMETER   ( ImageWindow, defaultTransparencyColor,         RGBAColor( 255, 255, 255, 255 ) )

// ----------------------------------------------------------------------------

DECLARE_STRING_METAPARAMETER  ( Identifiers, workspacePrefix,                  "Workspace" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, imagePrefix,                      "Image" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, previewPrefix,                    "Preview" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, processIconPrefix,                "Process" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, imageContainerIconPrefix,         "ImageContainer" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, newImageCaption,                  "<*new*>" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, clonePostfix,                     "_clone" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, noViewsAvailableText,             "<No View Available>" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, noViewSelectedText,               "<No View Selected>" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, noPreviewsAvailableText,          "<No Preview Available>" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, noPreviewSelectedText,            "<No Preview Selected>" )
DECLARE_STRING_METAPARAMETER  ( Identifiers, brokenLinkText,                   "<*broken*>" )

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_METAPARAMETER ( Process, enableParallelProcessing,             true )
DECLARE_BOOLEAN_METAPARAMETER ( Process, enableParallelCoreRendering,          true )
DECLARE_BOOLEAN_METAPARAMETER ( Process, enableParallelCoreColorManagement,    true )
DECLARE_BOOLEAN_METAPARAMETER ( Process, enableParallelModuleProcessing,       true )
DECLARE_BOOLEAN_METAPARAMETER ( Process, enableThreadCPUAffinity,              true )
DECLARE_INT32_METAPARAMETER   ( Process, maxModuleThreadPriority,              ThreadPriority::TimeCritical, ThreadPriority::Lowest, ThreadPriority::TimeCritical ) // ### Warning: DO NOT change these numbers
DECLARE_INT32_METAPARAMETER   ( Process, maxProcessors,                        int_max, 1, int_max )
DECLARE_BOOLEAN_METAPARAMETER ( Process, backupFiles,                          false )
DECLARE_BOOLEAN_METAPARAMETER ( Process, generateScriptComments,               true )
DECLARE_INT32_METAPARAMETER   ( Process, maxConsoleLines,                      1000, 0, int_max )
DECLARE_INT32_METAPARAMETER   ( Process, consoleDelay,                         750, 0, 60000 )
DECLARE_INT32_METAPARAMETER   ( Process, autoSavePSMPeriod,                    30, 0, 600 )
DECLARE_BOOLEAN_METAPARAMETER ( Process, alertOnProcessCompleted,              false )

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __PreferencesParameters_h

// ****************************************************************************
// EOF PreferencesParameters.h - Released 2014/10/29 07:34:56 UTC
