//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/APIInterface.h - Released 2015/12/17 18:52:09 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_API_APIInterface_h
#define __PCL_API_APIInterface_h

#ifndef __PCL_API_APIDefs_h
#include <pcl/api/APIDefs.h>
#endif

// Global namespace

#define PCL_API_Version 0x0155

extern "C"
{

/*
 * GlobalContext
 */
struct api_context GlobalContext
{
   /*
    * Version information
    */
   void        (api_func* GetPixInsightVersion)( uint32* major, uint32* minor, uint32* release, uint32* build, uint32* betaRelease, uint32* confidentialRelease, uint32* leVersion, char* langCode );
   // ### Returns a string allocated by the caller module.
   char16_type*(api_func* GetPixInsightCodename)( api_handle );

   /*
    * Error information
    */
   uint32      (api_func* LastError)();
   void        (api_func* ClearError)();
   api_bool    (api_func* ErrorMessage)( uint32, char16_type*, size_type* );

   /*
    * Memory allocation
    */
   void*       (api_func* Allocate)( size_type );
   api_bool    (api_func* Deallocate)( void* );

   /*
    * Thread status functions
    */
   uint32      (api_func* GetProcessStatus)();
   api_bool    (api_func* ResetProcessStatus)();
   api_bool    (api_func* EnableAbort)();
   api_bool    (api_func* DisableAbort)();
   api_bool    (api_func* Abort)();
   void        (api_func* ProcessEvents)( api_bool excludeUserInputEvents );

   /*
    * Console functions
    */
   api_handle  (api_func* GetThreadWindowId)(); // *** ### disabled function

   console_handle (api_func* GetConsole)();
   api_bool    (api_func* ValidateConsole)( const_console_handle );
   api_bool    (api_func* WriteConsole)( console_handle, const char16_type*, api_bool appendNewline );
   int32       (api_func* ReadConsoleChar)( console_handle ); // TODO - still not implemented

   // ### The following two functions return strings allocated by the caller module.
   char16_type*(api_func* ReadConsoleString)( api_handle, console_handle ); // TODO - still not implemented
   char16_type*(api_func* GetConsoleText)( api_handle, const_console_handle ); // ### security issues

   api_bool    (api_func* FlushConsole)( console_handle );
   api_bool    (api_func* ShowConsole)( console_handle, api_bool );
   api_bool    (api_func* ExecuteCommand)( api_handle, console_handle, const char16_type* cmd );

   /*
    * Global cursor position
    */
   void        (api_func* GetCursorPosition)( int32* x, int32* y );
   void        (api_func* SetCursorPosition)( int32 x, int32 y );

   /*
    * Query key pressed state. N.B.: key1 and key2 are platform-dependent keyboard scan codes.
    */
   api_bool    (api_func* IsKeyPressed)( int32 key1, int32 key2 );

   /*
    * Tool tip window
    */
   void        (api_func* ShowToolTipWindow)( int32 x, int32 y, const char16_type*,
                                                const_control_handle, int32, int32, int32, int32 );
   void        (api_func* HideToolTipWindow)();
   api_bool    (api_func* GetToolTipWindowText)( char16_type*, size_type* );

   /*
    * MessageBox functions
    */
   uint32      (api_func* MessageBox)( const char16_type* text, const char16_type* caption, uint32 button0, uint32 button1, uint32 button2, uint32 defButton, uint32 escButton, uint32 icon );

   /*
    * Instance and interface launch functions
    */
   void        (api_func* LaunchProcessInstance)( meta_process_handle, const_process_handle, int32 mode, uint32 flags );
   void        (api_func* LaunchProcessInstanceOnView)( meta_process_handle, const_process_handle, view_handle, uint32 flags );
   api_bool    (api_func* LaunchProcessInterface)( meta_interface_handle, uint32 flags );

   /*
    * Readout options
    */
   void        (api_func* GetReadoutOptions)( api_readout_options* options );
   void        (api_func* SetReadoutOptions)( const api_readout_options* options );

   /*
    * Real-time preview
    * ### Obsolete -- preserved for compatibility -- see RealTimePreviewContext
    */
   api_bool    (api_func* SetRealTimePreviewOwner)( interface_handle, uint32 flags );
   api_bool    (api_func* IsRealTimePreviewUpdating)();
   void        (api_func* UpdateRealTimePreview)();

   /*
    * Integrated documentation system
    */
   api_bool    (api_func* BrowseProcessDocumentation)( meta_process_handle, uint32 flags );

   /*
    * Global settings
    */
   api_bool    (api_func* GetGlobalFlag)( const char*, api_bool* );
   api_bool    (api_func* GetGlobalInteger)( const char*, void*, api_bool isSigned );
   api_bool    (api_func* GetGlobalReal)( const char*, double* );
   api_bool    (api_func* GetGlobalColor)( const char*, uint32* );
   api_bool    (api_func* GetGlobalFont)( const char*, char16_type*, size_type*, int32* sizePt );
   api_bool    (api_func* GetGlobalString)( const char*, char16_type*, size_type* );

   api_bool    (api_func* EnterGlobalSettingsUpdateContext)();
   api_bool    (api_func* IsGlobalSettingsUpdateContextActive)();

   api_bool    (api_func* SetGlobalFlag)( const char*, api_bool );
   api_bool    (api_func* SetGlobalInteger)( const char*, uint32, api_bool isSigned );
   api_bool    (api_func* SetGlobalReal)( const char*, double );
   api_bool    (api_func* SetGlobalColor)( const char*, uint32 );
   api_bool    (api_func* SetGlobalFont)( const char*, const char16_type*, int32 sizePt );
   api_bool    (api_func* SetGlobalString)( const char*, const char16_type* );

   api_bool    (api_func* CancelGlobalSettingsUpdate)( api_handle, uint32 reserved );
   api_bool    (api_func* ExitGlobalSettingsUpdateContext)();

   /*
    * Module-defined settings
    */
   // ### The following two functions return data allocated by the caller module.
   api_bool    (api_func* ReadSettingsBlock)( api_handle, void**, size_type*, const char* key, api_bool global );
   api_bool    (api_func* ReadSettingsString)( api_handle, char16_type**, const char* key, api_bool global );

   api_bool    (api_func* ReadSettingsFlag)( api_handle, api_bool*, const char* key, api_bool global );
   api_bool    (api_func* ReadSettingsInteger)( api_handle, int32*, const char* key, api_bool global );
   api_bool    (api_func* ReadSettingsUnsignedInteger)( api_handle, uint32*, const char* key, api_bool global );
   api_bool    (api_func* ReadSettingsReal)( api_handle, double*, const char* key, api_bool global );

   api_bool    (api_func* WriteSettingsBlock)( api_handle, const void*, size_type, const char* key, api_bool global );
   api_bool    (api_func* WriteSettingsString)( api_handle, const char16_type*, const char* key, api_bool global );
   api_bool    (api_func* WriteSettingsFlag)( api_handle, api_bool, const char* key, api_bool global );
   api_bool    (api_func* WriteSettingsInteger)( api_handle, int32, const char* key, api_bool global );
   api_bool    (api_func* WriteSettingsUnsignedInteger)( api_handle, uint32, const char* key, api_bool global );
   api_bool    (api_func* WriteSettingsReal)( api_handle, double, const char* key, api_bool global );

   api_bool    (api_func* DeleteSettingsItem)( api_handle, const char* key, api_bool global );

   uint32      (api_func* GetSettingsItemGlobalAccess)( api_handle, const char* key );                // bit#0=W bit#1=R
   api_bool    (api_func* SetSettingsItemGlobalAccess)( api_handle, const char* key, uint32 flags );  // bit#0=W bit#1=R

   /*
    * Miscellaneous message broadcasting
    */
   void        (api_func* BroadcastImageUpdated)( const_view_handle, const void* );

   /*
    * Miscellaneous color management
    */
   api_bool    (api_func* GetProfilesDirectory)( int32, char16_type*, size_type* );

   /*
    * Access to the global PixelTraits LUT
    */
   const ::api_pixtraits_lut* (api_func* GetPixelTraitsLUT)( uint32 version ); // version must be zero
};

/*
 * ModuleDefinitionContext
 */
struct api_context ModuleDefinitionContext
{
   void        (api_func* EnterModuleDefinitionContext)();
   api_bool    (api_func* IsModuleDefinitionContextActive)();

   void        (api_func* SetModuleOnLoadRoutine)( pcl::module_on_load_routine );
   void        (api_func* SetModuleOnUnloadRoutine)( pcl::module_on_unload_routine );
   void        (api_func* SetModuleAllocationRoutine)( pcl::module_allocation_routine );
   void        (api_func* SetModuleDeallocationRoutine)( pcl::module_deallocation_routine );

   void        (api_func* ExitModuleDefinitionContext)();
};

/*
 * ProcessDefinitionContext
 */
struct api_context ProcessDefinitionContext
{
   void        (api_func* EnterProcessDefinitionContext)();
   api_bool    (api_func* IsProcessDefinitionContextActive)();

   void        (api_func* BeginProcessDefinition)( meta_process_handle, const char* procId );
   api_bool    (api_func* GetProcessBeingDefined)( char*, size_type* );
   void        (api_func* SetProcessCategory)( const char* );
   void        (api_func* SetProcessVersion)( uint32 );
   void        (api_func* SetProcessAliasIdentifiers)( const char* );
   void        (api_func* SetProcessDescription)( const char16_type* );
   void        (api_func* SetProcessScriptComment)( const char16_type* );
   void        (api_func* SetProcessIconImage)( const char** xpm ); // ### deprecated
   void        (api_func* SetProcessIconImageFile)( const char16_type* imgFileName ); // ### deprecated
   void        (api_func* SetProcessIconSmallImage)( const char** xpm ); // ### deprecated
   void        (api_func* SetProcessIconSmallImageFile)( const char16_type* imgFileName ); // ### deprecated

   void        (api_func* SetProcessClassInitializationRoutine)( pcl::process_class_initialization_routine );
   void        (api_func* SetProcessCreationRoutine)( pcl::process_creation_routine );
   void        (api_func* SetProcessDestructionRoutine)( pcl::process_destruction_routine );
   void        (api_func* SetProcessClonationRoutine)( pcl::process_clonation_routine);
   void        (api_func* SetProcessTestClonationRoutine)( pcl::process_test_clonation_routine );
   void        (api_func* SetProcessAssignmentRoutine)( pcl::process_assignment_routine );
   void        (api_func* SetProcessInitializationRoutine)( pcl::process_initialization_routine );
   void        (api_func* SetProcessValidationRoutine)( pcl::process_validation_routine );

   void        (api_func* SetProcessCommandLineProcessingRoutine)( pcl::process_command_line_processing_routine, uint32 flags );
   void        (api_func* SetProcessEditPreferencesRoutine)( pcl::process_edit_preferences_routine );
   void        (api_func* SetProcessBrowseDocumentationRoutine)( pcl::process_browse_documentation_routine );
   void        (api_func* SetProcessExecutionPreferencesRoutine)( pcl::process_execution_preferences_routine );

   void        (api_func* SetProcessExecutionValidationRoutine)( pcl::process_execution_validation_routine );
   void        (api_func* SetProcessMaskValidationRoutine)( pcl::process_mask_validation_routine );
   void        (api_func* SetProcessHistoryUpdateValidationRoutine)( pcl::process_history_update_validation_routine );
   void        (api_func* SetProcessUndoModeRoutine)( pcl::process_undo_mode_routine );
   void        (api_func* SetProcessPreExecutionRoutine)( pcl::process_pre_execution_routine );
   void        (api_func* SetProcessExecutionRoutine)( pcl::process_execution_routine );
   void        (api_func* SetProcessPostExecutionRoutine)( pcl::process_post_execution_routine );

   void        (api_func* SetProcessGlobalExecutionValidationRoutine)( pcl::process_global_execution_validation_routine );
   void        (api_func* SetProcessPreGlobalExecutionRoutine)( pcl::process_pre_global_execution_routine );
   void        (api_func* SetProcessGlobalExecutionRoutine)( pcl::process_global_execution_routine );
   void        (api_func* SetProcessPostGlobalExecutionRoutine)( pcl::process_post_global_execution_routine );

   void        (api_func* SetProcessImageExecutionValidationRoutine)( pcl::process_image_execution_validation_routine );
   void        (api_func* SetProcessImageExecutionRoutine)( pcl::process_image_execution_routine );

   void        (api_func* SetProcessDefaultInterfaceSelectionRoutine)( pcl::process_default_interface_selection_routine );
   void        (api_func* SetProcessInterfaceSelectionRoutine)( pcl::process_interface_selection_routine );
   void        (api_func* SetProcessInterfaceValidationRoutine)( pcl::process_interface_validation_routine );

   void        (api_func* SetProcessPreReadingRoutine)( pcl::process_pre_reading_routine );
   void        (api_func* SetProcessPostReadingRoutine)( pcl::process_post_reading_routine );
   void        (api_func* SetProcessPreWritingRoutine)( pcl::process_pre_writing_routine );
   void        (api_func* SetProcessPostWritingRoutine)( pcl::process_post_writing_routine );

   void        (api_func* BeginParameterDefinition)( meta_parameter_handle, const char* parId, uint32 parType );
   api_bool    (api_func* GetParameterBeingDefined)( char*, size_type* );
   void        (api_func* SetParameterProcessVersionRange)( uint32, uint32 );
   void        (api_func* SetParameterRequired)( api_bool );
   void        (api_func* SetParameterReadOnly)( api_bool );
   void        (api_func* SetParameterAliasIdentifiers)( const char* );
   void        (api_func* SetParameterDescription)( const char16_type* );
   void        (api_func* SetParameterScriptComment)( const char16_type* );
   void        (api_func* SetParameterLockRoutine)( pcl::parameter_lock_routine );
   void        (api_func* SetParameterUnlockRoutine)( pcl::parameter_unlock_routine );
   void        (api_func* SetParameterValidationRoutine)( pcl::parameter_validation_routine );
   void        (api_func* SetParameterAllocationRoutine)( pcl::parameter_allocation_routine );
   void        (api_func* SetParameterLengthQueryRoutine)( pcl::parameter_length_query_routine );
   void        (api_func* SetDefaultNumericValue)( double );
   void        (api_func* SetValidNumericRange)( double, double );
   void        (api_func* SetPrecision)( int32 );
   void        (api_func* SetScientificNotation)( api_bool );
   void        (api_func* SetDefaultBooleanValue)( api_bool );
   void        (api_func* DefineEnumerationElement)( const char*, api_enum );
   void        (api_func* DefineEnumerationAlias)( const char*, const char* );
   void        (api_func* SetDefaultEnumerationValueIndex)( uint32 );
   void        (api_func* SetDefaultStringValue)( const char16_type* );
   void        (api_func* SetStringAllowedCharacters)( const char16_type* );
   void        (api_func* SetStringLengthLimits)( size_type, size_type );
   void        (api_func* BeginTableColumnDefinition)( meta_parameter_handle, const char* colId, uint32 colType );
   void        (api_func* EndTableColumnDefinition)();
   void        (api_func* SetTableRowLimits)( size_type, size_type );
   void        (api_func* SetBlockSizeLimits)( size_type, size_type );

   void        (api_func* EndParameterDefinition)();
   void        (api_func* EndProcessDefinition)();
   void        (api_func* ExitProcessDefinitionContext)();
};

/*
 * InterfaceDefinitionContext
 */
struct api_context InterfaceDefinitionContext
{
   void           (api_func* EnterInterfaceDefinitionContext)();
   api_bool       (api_func* IsInterfaceDefinitionContextActive)();

   void           (api_func* BeginInterfaceDefinition)( meta_interface_handle, const char* ifaceId, uint32 flags );
   api_bool       (api_func* GetInterfaceBeingDefined)( char*, size_type* );
   void           (api_func* SetInterfaceVersion)( uint32 );
   void           (api_func* SetInterfaceAliasIdentifiers)( const char* );
   void           (api_func* SetInterfaceDescription)( const char16_type* );
   void           (api_func* SetInterfaceIconImage)( const char** xpm ); // ### deprecated
   void           (api_func* SetInterfaceIconImageFile)( const char16_type* imgFileName ); // ### deprecated
   void           (api_func* SetInterfaceIconSmallImage)( const char** xpm ); // ### deprecated
   void           (api_func* SetInterfaceIconSmallImageFile)( const char16_type* imgFileName ); // ### deprecated

   void           (api_func* SetInterfaceFeatures)( uint32, uint32 );

   void           (api_func* SetInterfaceInitializationRoutine)( pcl::interface_initialization_routine );
   void           (api_func* SetInterfaceLaunchRoutine)( pcl::interface_launch_routine );
   void           (api_func* SetInterfaceProcessInstantiationRoutine)( pcl::interface_process_instantiation_routine );
   void           (api_func* SetInterfaceProcessTestInstantiationRoutine)( pcl::interface_process_instantiation_routine );
   void           (api_func* SetInterfaceProcessValidationRoutine)( pcl::interface_process_validation_routine );
   void           (api_func* SetInterfaceProcessImportRoutine)( pcl::interface_process_import_routine );

   void           (api_func* SetInterfaceApplyRoutine)( pcl::interface_control_routine );
   void           (api_func* SetInterfaceApplyGlobalRoutine)( pcl::interface_control_routine );
   void           (api_func* SetInterfaceRealTimePreviewUpdatedRoutine)( pcl::interface_control_state_routine );
   void           (api_func* SetInterfaceExecuteRoutine)( pcl::interface_control_routine );
   void           (api_func* SetInterfaceCancelRoutine)( pcl::interface_control_routine );
   void           (api_func* SetInterfaceBrowseDocumentationRoutine)( pcl::interface_control_routine );
   void           (api_func* SetInterfaceTrackViewUpdatedRoutine)( pcl::interface_control_state_routine );
   void           (api_func* SetInterfaceEditPreferencesRoutine)( pcl::interface_control_routine );
   void           (api_func* SetInterfaceResetRoutine)( pcl::interface_control_routine );

   void           (api_func* SetInterfaceRealTimeUpdateQueryRoutine)( pcl::interface_real_time_update_query_routine );
   void           (api_func* SetInterfaceRealTimeGenerationRoutine)( pcl::interface_real_time_generation_routine );
   void           (api_func* SetInterfaceRealTimeCancelRoutine)( pcl::interface_real_time_cancel_routine );

   void           (api_func* SetInterfaceDynamicModeEnterRoutine)( pcl::interface_dynamic_mode_enter_routine );
   void           (api_func* SetInterfaceDynamicModeExitRoutine)( pcl::interface_dynamic_mode_exit_routine );
   void           (api_func* SetInterfaceDynamicMouseEnterRoutine)( pcl::interface_dynamic_view_event_routine );
   void           (api_func* SetInterfaceDynamicMouseLeaveRoutine)( pcl::interface_dynamic_view_event_routine );
   void           (api_func* SetInterfaceDynamicMouseMoveRoutine)( pcl::interface_dynamic_mouse_routine );
   void           (api_func* SetInterfaceDynamicMousePressRoutine)( pcl::interface_dynamic_mouse_button_routine );
   void           (api_func* SetInterfaceDynamicMouseReleaseRoutine)( pcl::interface_dynamic_mouse_button_routine );
   void           (api_func* SetInterfaceDynamicMouseDoubleClickRoutine)( pcl::interface_dynamic_mouse_routine );
   void           (api_func* SetInterfaceDynamicKeyPressRoutine)( pcl::interface_dynamic_keyboard_routine );
   void           (api_func* SetInterfaceDynamicKeyReleaseRoutine)( pcl::interface_dynamic_keyboard_routine );
   void           (api_func* SetInterfaceDynamicMouseWheelRoutine)( pcl::interface_dynamic_wheel_routine );
   void           (api_func* SetInterfaceDynamicUpdateQueryRoutine)( pcl::interface_dynamic_update_query_routine );
   void           (api_func* SetInterfaceDynamicPaintRoutine)( pcl::interface_dynamic_paint_routine );

   void           (api_func* SetImageCreatedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageRenamedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageDeletedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageFocusedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageLockedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageUnlockedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageSTFEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageSTFDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageSTFUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageRGBWSUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageCMEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageCMDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageCMUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetImageSavedNotificationRoutine)( pcl::image_notification_routine );

   void           (api_func* SetMaskUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetMaskEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetMaskDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetMaskShownNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetMaskHiddenNotificationRoutine)( pcl::image_notification_routine );

   void           (api_func* SetTransparencyHiddenNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetTransparencyModeUpdatedNotificationRoutine)( pcl::image_notification_routine );

   void           (api_func* SetViewPropertyUpdatedNotificationRoutine)( pcl::view_property_notification_routine );
   void           (api_func* SetViewPropertyDeletedNotificationRoutine)( pcl::view_property_notification_routine );

   void           (api_func* SetBeginReadoutNotificationRoutine)( pcl::image_notification_routine );
   void           (api_func* SetUpdateReadoutNotificationRoutine)( pcl::readout_notification_routine );
   void           (api_func* SetEndReadoutNotificationRoutine)( pcl::image_notification_routine );

   void           (api_func* SetProcessCreatedNotificationRoutine)( pcl::process_notification_routine );
   void           (api_func* SetProcessUpdatedNotificationRoutine)( pcl::process_notification_routine );
   void           (api_func* SetProcessDeletedNotificationRoutine)( pcl::process_notification_routine );
   void           (api_func* SetProcessSavedNotificationRoutine)( pcl::process_notification_routine );

   void           (api_func* SetRealTimePreviewOwnerChangeNotificationRoutine)( pcl::interface_notification_routine );
   void           (api_func* SetRealTimePreviewLUTUpdatedNotificationRoutine)( pcl::lut_notification_routine );
   void           (api_func* SetRealTimePreviewGenerationStartNotificationRoutine)( pcl::global_notification_routine );
   void           (api_func* SetRealTimePreviewGenerationFinishNotificationRoutine)( pcl::global_notification_routine );

   void           (api_func* SetGlobalRGBWSUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (api_func* SetGlobalCMEnabledNotificationRoutine)( pcl::global_notification_routine );
   void           (api_func* SetGlobalCMDisabledNotificationRoutine)( pcl::global_notification_routine );
   void           (api_func* SetGlobalCMUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (api_func* SetReadoutOptionsUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (api_func* SetGlobalPreferencesUpdatedNotificationRoutine)( pcl::global_notification_routine );

   void           (api_func* EndInterfaceDefinition)();
   void           (api_func* ExitInterfaceDefinitionContext)();
};

/*
 * FileFormatDefinitionContext
 */
struct api_context FileFormatDefinitionContext
{
   void           (api_func* EnterFileFormatDefinitionContext)();
   api_bool       (api_func* IsFileFormatDefinitionContextActive)();

   void           (api_func* BeginFileFormatDefinition)( meta_format_handle, const char* fmtName, const char16_type** fmtExtensions, const char** fmtMimeTypes );
   api_bool       (api_func* GetFileFormatBeingDefined)( char*, size_type* );
   void           (api_func* SetFileFormatVersion)( uint32 );
   void           (api_func* SetFileFormatDescription)( const char16_type* );
   void           (api_func* SetFileFormatImplementation)( const char16_type* );
   void           (api_func* SetFileFormatIconImage)( const char** xpm ); // ### deprecated
   void           (api_func* SetFileFormatIconImageFile)( const char16_type* imgFileName ); // ### deprecated
   void           (api_func* SetFileFormatIconSmallImage)( const char** xpm ); // ### deprecated
   void           (api_func* SetFileFormatIconSmallImageFile)( const char16_type* imgFileName ); // ### deprecated
   void           (api_func* SetFileFormatCaps)( const api_format_capabilities* );

   void           (api_func* SetFileFormatCreationRoutine)( pcl::format_creation_routine );
   void           (api_func* SetFileFormatDestructionRoutine)( pcl::format_destruction_routine );
   void           (api_func* SetFileFormatValidateFormatSpecificDataRoutine)( pcl::format_validate_format_specific_data_routine );
   void           (api_func* SetFileFormatDisposeFormatSpecificDataRoutine)( pcl::format_dispose_format_specific_data_routine );
   void           (api_func* SetFileFormatEditPreferencesRoutine)( pcl::format_edit_preferences_routine );

   void           (api_func* SetFileFormatOpenRoutine)( pcl::format_open_routine );
   void           (api_func* SetFileFormatGetImageCountRoutine)( pcl::format_get_image_count_routine );
   void           (api_func* SetFileFormatGetImageIdRoutine)( pcl::format_get_image_id_routine );
   void           (api_func* SetFileFormatGetImageDescriptionRoutine)( pcl::format_get_image_description_routine );
   void           (api_func* SetFileFormatCloseRoutine)( pcl::format_close_routine );
   void           (api_func* SetFileFormatIsOpenRoutine)( pcl::format_is_open_routine );
   void           (api_func* SetFileFormatGetFilePathRoutine)( pcl::format_get_file_path_routine );
   void           (api_func* SetFileFormatSetSelectedImageIndexRoutine)( pcl::format_set_selected_image_index_routine );
   void           (api_func* SetFileFormatGetSelectedImageIndexRoutine)( pcl::format_get_selected_image_index_routine );
   void           (api_func* SetFileFormatSetFormatSpecificDataRoutine)( pcl::format_set_format_specific_data_routine );
   void           (api_func* SetFileFormatGetFormatSpecificDataRoutine)( pcl::format_get_format_specific_data_routine );
   void           (api_func* SetFileFormatGetImagePropertiesRoutine)( pcl::format_get_image_properties_routine );
   void           (api_func* SetFileFormatBeginKeywordExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (api_func* SetFileFormatGetKeywordCountRoutine)( pcl::format_get_keyword_count_routine );
   void           (api_func* SetFileFormatGetNextKeywordRoutine)( pcl::format_get_next_keyword_routine );
   void           (api_func* SetFileFormatEndKeywordExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (api_func* SetFileFormatBeginICCProfileExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (api_func* SetFileFormatGetICCProfileRoutine)( pcl::format_get_icc_profile_routine );
   void           (api_func* SetFileFormatEndICCProfileExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (api_func* SetFileFormatBeginThumbnailExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (api_func* SetFileFormatGetThumbnailRoutine)( pcl::format_get_thumbnail_routine );
   void           (api_func* SetFileFormatEndThumbnailExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (api_func* SetFileFormatEnumerateImagePropertiesRoutine)( pcl::format_enumerate_image_properties_routine );
   void           (api_func* SetFileFormatBeginPropertyExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (api_func* SetFileFormatGetImagePropertyRoutine)( pcl::format_get_image_property_routine );
   void           (api_func* SetFileFormatEndPropertyExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (api_func* SetFileFormatBeginPropertyEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (api_func* SetFileFormatSetImagePropertyRoutine)( pcl::format_set_image_property_routine );
   void           (api_func* SetFileFormatEndPropertyEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (api_func* SetFileFormatBeginRGBWSExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (api_func* SetFileFormatGetImageRGBWSRoutine)( pcl::format_get_image_rgbws_routine );
   void           (api_func* SetFileFormatEndRGBWSExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (api_func* SetFileFormatBeginRGBWSEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (api_func* SetFileFormatSetImageRGBWSRoutine)( pcl::format_set_image_rgbws_routine );
   void           (api_func* SetFileFormatEndRGBWSEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (api_func* SetFileFormatBeginDisplayFunctionExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (api_func* SetFileFormatGetImageDisplayFunctionRoutine)( pcl::format_get_image_display_function_routine );
   void           (api_func* SetFileFormatEndDisplayFunctionExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (api_func* SetFileFormatBeginDisplayFunctionEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (api_func* SetFileFormatSetImageDisplayFunctionRoutine)( pcl::format_set_image_display_function_routine );
   void           (api_func* SetFileFormatEndDisplayFunctionEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (api_func* SetFileFormatBeginColorFilterArrayExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (api_func* SetFileFormatGetImageColorFilterArrayRoutine)( pcl::format_get_image_color_filter_array_routine );
   void           (api_func* SetFileFormatEndColorFilterArrayExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (api_func* SetFileFormatBeginColorFilterArrayEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (api_func* SetFileFormatSetImageColorFilterArrayRoutine)( pcl::format_set_image_color_filter_array_routine );
   void           (api_func* SetFileFormatEndColorFilterArrayEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (api_func* SetFileFormatReadImageRoutine)( pcl::format_read_image_routine );
   void           (api_func* SetFileFormatReadPixelsRoutine)( pcl::format_read_pixels_routine );
   void           (api_func* SetFileFormatQueryOptionsRoutine)( pcl::format_query_options_routine );
   void           (api_func* SetFileFormatCreateRoutine)( pcl::format_create_routine );
   void           (api_func* SetFileFormatSetImageIdRoutine)( pcl::format_set_image_id_routine );
   void           (api_func* SetFileFormatSetImageOptionsRoutine)( pcl::format_set_image_options_routine );
   void           (api_func* SetFileFormatCreateImageRoutine)( pcl::format_create_image_routine );
   void           (api_func* SetFileFormatBeginKeywordEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (api_func* SetFileFormatAddKeywordRoutine)( pcl::format_add_keyword_routine );
   void           (api_func* SetFileFormatEndKeywordEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (api_func* SetFileFormatBeginICCProfileEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (api_func* SetFileFormatSetICCProfileRoutine)( pcl::format_set_icc_profile_routine );
   void           (api_func* SetFileFormatEndICCProfileEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (api_func* SetFileFormatBeginThumbnailEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (api_func* SetFileFormatSetThumbnailRoutine)( pcl::format_set_thumbnail_routine );
   void           (api_func* SetFileFormatEndThumbnailEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (api_func* SetFileFormatWriteImageRoutine)( pcl::format_write_image_routine );
   void           (api_func* SetFileFormatWritePixelsRoutine)( pcl::format_write_pixels_routine );
   void           (api_func* SetFileFormatQueryInexactReadRoutine)( pcl::format_query_inexact_read_routine );
   void           (api_func* SetFileFormatQueryLossyWriteRoutine)( pcl::format_query_lossy_write_routine );
   void           (api_func* SetFileFormatQueryFormatStatusRoutine)( pcl::format_query_format_status_routine );
   void           (api_func* EndFileFormatDefinition)();

   void           (api_func* ExitFileFormatDefinitionContext)();
};

/*
 * ModuleContext
 */
struct api_context ModuleContext
{
   api_bool    (api_func* LoadResource)( api_handle, const char16_type*, const char16_type* );
   api_bool    (api_func* UnloadResource)( api_handle, const char16_type*, const char16_type* );
};

/*
 * ProcessContext
 */
struct api_context ProcessContext
{
   api_bool             (api_func* EnumerateProcessCategories)( pcl::category_enumeration_callback, char*, size_type*, void* );
   api_bool             (api_func* EnumerateProcesses)( pcl::process_enumeration_callback, void* );

   meta_process_handle  (api_func* GetProcessByName)( api_handle, const char* id );

   api_bool             (api_func* GetProcessIdentifier)( meta_process_handle, char*, size_type* );
   api_bool             (api_func* GetProcessCategory)( meta_process_handle, char*, size_type* );
   uint32               (api_func* GetProcessVersion)( meta_process_handle );
   api_bool             (api_func* GetProcessAliasIdentifiers)( meta_process_handle, char*, size_type* );
   api_bool             (api_func* GetProcessDescription)( meta_process_handle, char16_type*, size_type* );
   api_bool             (api_func* GetProcessScriptComment)( meta_process_handle, char16_type*, size_type* );
   bitmap_handle        (api_func* GetProcessIcon)( meta_process_handle );
   bitmap_handle        (api_func* GetProcessSmallIcon)( meta_process_handle );
   api_bool             (api_func* GetProcessProperties)( meta_process_handle, api_process_properties* );
   interface_handle     (api_func* GetProcessDefaultInterface)( meta_process_handle );

   api_bool             (api_func* EditProcessPreferences)( meta_process_handle );
   api_bool             (api_func* BrowseProcessDocumentation)( meta_process_handle, uint32 flags );
   int32                (api_func* RunProcessCommandLine)( meta_process_handle, const char16_type* );
   api_bool             (api_func* LaunchProcess)( meta_process_handle );

   api_bool             (api_func* EnumerateProcessParameters)( meta_process_handle, pcl::parameter_enumeration_callback, void* );
   api_bool             (api_func* EnumerateTableColumns)( meta_parameter_handle, pcl::parameter_enumeration_callback, void* );

   meta_parameter_handle (api_func* GetParameterByName)( meta_process_handle, const char* id );
   meta_parameter_handle (api_func* GetTableColumnByName)( meta_parameter_handle, const char* id );

   meta_process_handle  (api_func* GetParameterProcess)( meta_parameter_handle );
   meta_parameter_handle (api_func* GetParameterTable)( meta_parameter_handle );

   uint32               (api_func* GetParameterType)( meta_parameter_handle );
   api_bool             (api_func* GetParameterIdentifier)( meta_parameter_handle, char*, size_type* );
   api_bool             (api_func* GetParameterAliasIdentifiers)( meta_parameter_handle, char*, size_type* );
   api_bool             (api_func* GetParameterDescription)( meta_parameter_handle, char16_type*, size_type* );
   api_bool             (api_func* GetParameterScriptComment)( meta_parameter_handle, char16_type*, size_type* );
   api_bool             (api_func* GetParameterRequired)( meta_parameter_handle );
   api_bool             (api_func* GetParameterReadOnly)( meta_parameter_handle );

   // Boolean, numeric and string parameter types
   api_bool             (api_func* GetParameterDefaultValue)( meta_parameter_handle, void* value, size_type* length );

   // Enumerated parameters
   size_type            (api_func* GetParameterElementCount)( meta_parameter_handle );
   api_bool             (api_func* GetParameterElementIdentifier)( meta_parameter_handle, size_type index, char*, size_type* );
   api_bool             (api_func* GetParameterElementAliasIdentifiers)( meta_parameter_handle, size_type index, char*, size_type* );
   api_enum             (api_func* GetParameterElementValue)( meta_parameter_handle, size_type index );
   int32                (api_func* GetParameterDefaultElementIndex)( meta_parameter_handle );

   // Numeric parameters
   api_bool             (api_func* GetParameterRange)( meta_parameter_handle, double* minValue, double* maxValue );
   int32                (api_func* GetParameterPrecision)( meta_parameter_handle );
   api_bool             (api_func* GetParameterScientificNotation)( meta_parameter_handle );

   // Variable length parameters
   api_bool             (api_func* GetParameterLengthLimits)( meta_parameter_handle, size_type* minLength, size_type* maxLength );

   // String parameters
   api_bool             (api_func* GetParameterAllowedCharacters)( meta_parameter_handle, char16_type*, size_type* );

   process_handle       (api_func* CreateProcessInstance)( api_handle, meta_process_handle );

   meta_process_handle  (api_func* GetProcessInstanceProcess)( const_process_handle );

   process_handle       (api_func* CloneProcessInstance)( api_handle, const_process_handle, uint32 flags );
   api_bool             (api_func* AssignProcessInstance)( process_handle, const_process_handle, uint32 flags );
   api_bool             (api_func* ValidateProcessInstance)( process_handle, char16_type*, size_type );

   api_bool             (api_func* GetUpdatesViewHistory)( const_process_handle, const_view_handle );
   api_bool             (api_func* ValidateViewExecutionMask)( const_process_handle, const_view_handle, const_window_handle );
   api_bool             (api_func* ValidateViewExecution)( const_process_handle, const_view_handle, char16_type*, size_type );
   api_bool             (api_func* ExecuteOnView)( process_handle, view_handle, uint32 flags );

   api_bool             (api_func* ValidateGlobalExecution)( const_process_handle, char16_type*, size_type );
   api_bool             (api_func* ExecuteGlobal)( process_handle, uint32 flags );

   api_bool             (api_func* ValidateImageExecution)( const_process_handle, const_image_handle, char16_type*, size_type );
   api_bool             (api_func* ExecuteOnImage)( process_handle, image_handle, const char*, uint32 flags );

   api_bool             (api_func* LaunchProcessInstance)( process_handle );

   api_bool             (api_func* ValidateInterfaceLaunch)( const_process_handle );
   api_bool             (api_func* LaunchInterface)( process_handle );

   // ### TODO: The following two functions are not yet implemented.
   api_bool             (api_func* ValidateInterface)( const_process_handle, const_interface_handle, char16_type*, size_type );
   interface_handle     (api_func* GetInterface)( const_process_handle );

   api_bool             (api_func* GetProcessInstanceDescription)( const_process_handle, char16_type*, size_type* );
   api_bool             (api_func* SetProcessInstanceDescription)( process_handle, const char16_type* );

   api_bool             (api_func* GetExecutionTimes)( const_process_handle, double* startJD, double* elapsedSecs );

   // ### Returns a string allocated by the caller module.
   char16_type*         (api_func* GetProcessInstanceSourceCode)( api_handle, const_process_handle, const char* language, const char* varId, uint32 indent );
   process_handle       (api_func* CreateProcessInstanceFromSourceCode)( const char16_type* source, const char* language );

   process_handle       (api_func* CreateProcessInstanceFromIcon)( api_handle, const char* iconId );

   api_bool             (api_func* EnumerateProcessIcons)( pcl::icon_enumeration_callback, char*, size_type*, void* );

   api_bool             (api_func* GetParameterValue)( const_process_handle, meta_parameter_handle, size_type tableRow, uint32* parType, void* value, size_type* length );
   api_bool             (api_func* SetParameterValue)( process_handle, meta_parameter_handle, size_type tableRow, const void* value, size_type length );

   size_type            (api_func* GetTableRowCount)( const_process_handle, meta_parameter_handle );

   api_bool             (api_func* AllocateTableRows)( process_handle, meta_parameter_handle, size_type rowCount );
};

/*
 * InterfaceContext
 */
struct api_context InterfaceContext
{
   // ### TODO
};

/*
 * FileFormatContext
 */
struct api_context FileFormatContext
{
   api_bool             (api_func* EnumerateFileFormats)( pcl::format_enumeration_callback, void* );

   meta_format_handle   (api_func* GetFileFormatByName)( api_handle, const char* id );
   meta_format_handle   (api_func* GetFileFormatByFileExtension)( api_handle, const char16_type* ext, api_bool toRead, api_bool toWrite );
   meta_format_handle   (api_func* GetFileFormatByMimeType)( api_handle, const char* mimeType, api_bool toRead, api_bool toWrite );

   api_bool             (api_func* GetFileFormatName)( meta_format_handle, char*, size_type* );
   api_bool             (api_func* GetFileFormatFileExtensions)( meta_format_handle, char16_type**, size_type* extCount, size_type* maxExtLen );
   api_bool             (api_func* GetFileFormatMimeTypes)( meta_format_handle, char**, size_type* mimeCount, size_type* maxMimeLen );
   uint32               (api_func* GetFileFormatVersion)( meta_format_handle );
   api_bool             (api_func* GetFileFormatDescription)( meta_format_handle, char16_type*, size_type* );
   api_bool             (api_func* GetFileFormatImplementation)( meta_format_handle, char16_type*, size_type* );
   bitmap_handle        (api_func* GetFileFormatIcon)( meta_format_handle );
   bitmap_handle        (api_func* GetFileFormatSmallIcon)( meta_format_handle );
   api_bool             (api_func* GetFileFormatCapabilities)( meta_format_handle, api_format_capabilities* );
   api_bool             (api_func* GetFileFormatStatus)( meta_format_handle, char16_type*, size_type*, void* );

   api_bool             (api_func* EditFileFormatPreferences)( meta_format_handle );

   file_format_handle   (api_func* CreateFileFormatInstance)( api_handle, meta_format_handle );

   meta_format_handle   (api_func* GetFileFormatInstanceFormat)( const_file_format_handle );

   api_bool             (api_func* CloseImageFile)( file_format_handle );

   api_bool             (api_func* IsImageFileOpen)( const_file_format_handle );

   api_bool             (api_func* GetImageFilePath)( file_format_handle, char16_type*, size_type* );

   api_bool             (api_func* OpenImageFile)( file_format_handle, const char16_type* );
   api_bool             (api_func* OpenImageFileEx)( file_format_handle, const char16_type*, const char*, uint32 );
   uint32               (api_func* GetImageCount)( const_file_format_handle );
   api_bool             (api_func* GetImageId)( const_file_format_handle, char*, size_type*, uint32 );
   api_bool             (api_func* GetImageDescription)( const_file_format_handle, api_image_info*, api_image_options*, uint32 );

   api_bool             (api_func* SelectImage)( file_format_handle, uint32 );
   uint32               (api_func* GetSelectedImageIndex)( const_file_format_handle );

   const void*          (api_func* GetFormatSpecificData)( const_file_format_handle );
   api_bool             (api_func* SetFormatSpecificData)( file_format_handle, const void* );
   api_bool             (api_func* ValidateFormatSpecificData)( meta_format_handle, const void* );
   void                 (api_func* DisposeFormatSpecificData)( meta_format_handle, const void* );

   api_bool             (api_func* GetImageProperties)( const_file_format_handle, char16_type*, size_type* );

   api_bool             (api_func* BeginKeywordExtraction)( file_format_handle );
   size_type            (api_func* GetKeywordCount)( file_format_handle );
   api_bool             (api_func* GetNextKeyword)( file_format_handle, char*, char*, char*, uint32 );
   void                 (api_func* EndKeywordExtraction)( file_format_handle );

   api_bool             (api_func* BeginICCProfileExtraction)( file_format_handle );
   const void*          (api_func* GetICCProfile)( file_format_handle );
   void                 (api_func* EndICCProfileExtraction)( file_format_handle );

   api_bool             (api_func* BeginThumbnailExtraction)( file_format_handle );
   api_bool             (api_func* GetThumbnail)( file_format_handle, image_handle );
   void                 (api_func* EndThumbnailExtraction)( file_format_handle );

   api_bool             (api_func* EnumerateImageProperties)( file_format_handle, pcl::property_enumeration_callback, char*, size_type*, void* );

   api_bool             (api_func* BeginPropertyExtraction)( file_format_handle );
   api_bool             (api_func* GetImageProperty)( file_format_handle, const char* id, api_property_value* );
   void                 (api_func* EndPropertyExtraction)( file_format_handle );

   api_bool             (api_func* BeginPropertyEmbedding)( file_format_handle );
   api_bool             (api_func* SetImageProperty)( file_format_handle, const char* id, const api_property_value* );
   void                 (api_func* EndPropertyEmbedding)( file_format_handle );

   api_bool             (api_func* BeginRGBWSExtraction)( file_format_handle );
   api_bool             (api_func* GetImageRGBWS)( file_format_handle, float*, api_bool*, float*, float*, float* );
   void                 (api_func* EndRGBWSExtraction)( file_format_handle );

   api_bool             (api_func* BeginRGBWSEmbedding)( file_format_handle );
   api_bool             (api_func* SetImageRGBWS)( file_format_handle, float, api_bool, const float*, const float*, const float* );
   void                 (api_func* EndRGBWSEmbedding)( file_format_handle );

   api_bool             (api_func* BeginDisplayFunctionExtraction)( file_format_handle );
   api_bool             (api_func* GetImageDisplayFunction)( file_format_handle, double*, double*, double*, double*, double* );
   void                 (api_func* EndDisplayFunctionExtraction)( file_format_handle );

   api_bool             (api_func* BeginDisplayFunctionEmbedding)( file_format_handle );
   api_bool             (api_func* SetImageDisplayFunction)( file_format_handle, const double*, const double*, const double*, const double*, const double* );
   void                 (api_func* EndDisplayFunctionEmbedding)( file_format_handle );

   api_bool             (api_func* BeginColorFilterArrayExtraction)( file_format_handle );
   api_bool             (api_func* GetImageColorFilterArray)( file_format_handle, char*, size_type*, int32*, int32*, char16_type*, size_type* );
   void                 (api_func* EndColorFilterArrayExtraction)( file_format_handle );

   api_bool             (api_func* BeginColorFilterArrayEmbedding)( file_format_handle );
   api_bool             (api_func* SetImageColorFilterArray)( file_format_handle, const char*, int32, int32, const char16_type* );
   void                 (api_func* EndColorFilterArrayEmbedding)( file_format_handle );

   api_bool             (api_func* ReadImage)( file_format_handle, image_handle );

   api_bool             (api_func* ReadPixels)( file_format_handle, void*, uint32, uint32, uint32, uint32, api_bool, api_bool );

   api_bool             (api_func* QueryImageFileOptions)( file_format_handle, api_image_options*, const void**, uint32 );

   api_bool             (api_func* CreateImageFile)( file_format_handle, const char16_type*, uint32 );
   api_bool             (api_func* CreateImageFileEx)( file_format_handle, const char16_type*, uint32, const char*, uint32 );

   api_bool             (api_func* SetImageId)( file_format_handle, const char* );
   api_bool             (api_func* SetImageOptions)( file_format_handle, const api_image_options* );

   api_bool             (api_func* BeginKeywordEmbedding)( file_format_handle );
   api_bool             (api_func* AddKeyword)( file_format_handle, const char*, const char*, const char* );
   void                 (api_func* EndKeywordEmbedding)( file_format_handle );

   api_bool             (api_func* BeginICCProfileEmbedding)( file_format_handle );
   api_bool             (api_func* SetICCProfile)( file_format_handle, const void* );
   void                 (api_func* EndICCProfileEmbedding)( file_format_handle );

   api_bool             (api_func* BeginThumbnailEmbedding)( file_format_handle );
   api_bool             (api_func* SetThumbnail)( file_format_handle, const_image_handle );
   void                 (api_func* EndThumbnailEmbedding)( file_format_handle );

   api_bool             (api_func* WriteImage)( file_format_handle, const_image_handle );

   api_bool             (api_func* CreateImage)( file_format_handle, const api_image_info* );
   api_bool             (api_func* WritePixels)( file_format_handle, const void*, uint32, uint32, uint32, uint32, api_bool, api_bool );

   api_bool             (api_func* WasInexactRead)( const_file_format_handle );
   api_bool             (api_func* WasLossyWrite)( const_file_format_handle );
};

/*
 * UIContext
 */
struct api_context UIContext
{
   api_bool       (api_func* AttachToUIObject)( api_handle, api_handle );
   api_bool       (api_func* DetachFromUIObject)( api_handle, api_handle );

   api_handle     (api_func* GetUIObjectModule)( const_api_handle );

   size_type      (api_func* GetUIObjectRefCount)( const_api_handle );

   api_bool       (api_func* GetUIObjectType)( const_api_handle, char*, size_type* );

   api_bool       (api_func* GetUIObjectId)( const_api_handle, char16_type*, size_type* );
   api_bool       (api_func* SetUIObjectId)( api_handle, const char16_type* );

   api_bool       (api_func* SetHandleDestroyedEventRoutine)( api_handle, pcl::destroy_event_routine );
};

/*
 * ActionContext
 */
struct api_context ActionContext
{
   action_handle  (api_func* CreateAction)( api_handle, api_handle client,
                                            const char16_type* menuItem, const char16_type* toolBar,
                                            const_bitmap_handle icon,
                                            uint32 flags );

   api_bool       (api_func* GetActionMenuItem)( const_action_handle, char16_type*, size_type* );

   api_bool       (api_func* GetActionMenuText)( const_action_handle, char16_type*, size_type* );
   void           (api_func* SetActionMenuText)( action_handle, const char16_type* );

   api_bool       (api_func* GetActionToolBar)( const_action_handle, char16_type*, size_type* );

   api_bool       (api_func* GetActionToolTip)( const_action_handle, char16_type*, size_type* );
   void           (api_func* SetActionToolTip)( action_handle, const char16_type* );

   bitmap_handle  (api_func* GetActionIcon)( const_action_handle );
   void           (api_func* SetActionIcon)( action_handle, const_bitmap_handle );

   void           (api_func* GetActionAccelerator)( const_action_handle, int32* keyModifiers, int32* keyCode );
   void           (api_func* SetActionAccelerator)( action_handle, int32 keyModifiers, int32 keyCode );

   api_bool       (api_func* SetActionExecutionRoutine)( action_handle, pcl::action_execution_routine );
   api_bool       (api_func* SetActionStateQueryRoutine)( action_handle, pcl::action_state_query_routine );
};

/*
 * ControlContext
 */
struct api_context ControlContext
{
   control_handle (api_func* CreateControl)( api_handle, api_handle client, control_handle parent, uint32 flags );

   void           (api_func* GetFrameRect)( const_control_handle, int32*, int32*, int32*, int32* );

   void           (api_func* GetClientRect)( const_control_handle, int32*, int32*, int32*, int32* );
   void           (api_func* SetClientRect)( control_handle, int32, int32, int32, int32 );

   void           (api_func* SetControlSize)( control_handle, int32, int32 );

   void           (api_func* AdjustControlToContents)( control_handle );

   void           (api_func* GetControlMinSize)( const_control_handle, int32*, int32* );
   void           (api_func* SetControlMinSize)( control_handle, int32, int32 );

   void           (api_func* GetControlMaxSize)( const_control_handle, int32*, int32* );
   void           (api_func* SetControlMaxSize)( control_handle, int32, int32 );

   void           (api_func* SetControlFixedSize)( control_handle, int32, int32 );

   void           (api_func* GetControlExpansionEnabled)( const_control_handle, api_bool*, api_bool* );
   void           (api_func* SetControlExpansionEnabled)( control_handle, api_bool, api_bool );

   void           (api_func* GetControlPosition)( const_control_handle, int32*, int32* );
   void           (api_func* SetControlPosition)( control_handle, int32, int32 );

   api_bool       (api_func* GetControlUnderMouseStatus)( const_control_handle );

   void           (api_func* BringControlToFront)( control_handle );
   void           (api_func* SendControlToBack)( control_handle );

   void           (api_func* StackControls)( control_handle stackThis, control_handle underThis );

   sizer_handle   (api_func* GetControlSizer)( const_control_handle );
   void           (api_func* SetControlSizer)( control_handle, sizer_handle );

   void           (api_func* GlobalToLocal)( const_control_handle, int32*, int32* );
   void           (api_func* LocalToGlobal)( const_control_handle, int32*, int32* );
   void           (api_func* ParentToLocal)( const_control_handle, int32*, int32* );
   void           (api_func* LocalToParent)( const_control_handle, int32*, int32* );
   void           (api_func* ControlToLocal)( const_control_handle, const_control_handle, int32*, int32* );
   void           (api_func* LocalToControl)( const_control_handle, const_control_handle, int32*, int32* );

   control_handle (api_func* GetChildByPos)( const_control_handle, int32, int32 ); // returns client handle

   void           (api_func* GetChildrenRect)( const_control_handle, int32*, int32*, int32*, int32* );

   api_bool       (api_func* GetControlAncestry)( const_control_handle, const_control_handle );

   control_handle (api_func* GetControlParent)( const_control_handle ); // returns client handle
   void           (api_func* SetControlParent)( control_handle, control_handle );

   control_handle (api_func* GetControlWindow)( const_control_handle ); // returns client handle

   api_bool       (api_func* GetControlEnabled)( const_control_handle );
   void           (api_func* SetControlEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetControlMouseTrackingEnabled)( const_control_handle );
   void           (api_func* SetControlMouseTrackingEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetControlVisible)( const_control_handle );
   void           (api_func* SetControlVisible)( control_handle, api_bool );

   void           (api_func* GetControlVisibleRect)( const_control_handle, int32*, int32*, int32*, int32* );

   api_bool       (api_func* GetWindowState)( const_control_handle, api_bool* active, api_bool* modal, api_bool* maximized, api_bool* minimized ); // returns true if control is a window

   void           (api_func* ActivateWindow)( control_handle );

   api_bool       (api_func* GetControlFocus)( const_control_handle );
   void           (api_func* SetControlFocus)( control_handle, api_bool );

   int32          (api_func* GetControlFocusStyle)( const_control_handle );
   void           (api_func* SetControlFocusStyle)( control_handle, int32 );

   control_handle (api_func* GetFocusChildControl)( const_control_handle ); // returns client handle

   control_handle (api_func* GetChildControlToFocus)( const_control_handle ); // returns client handle
   void           (api_func* SetChildControlToFocus)( control_handle, control_handle );

   control_handle (api_func* GetNextSiblingControlToFocus)( const_control_handle ); // returns client handle
   void           (api_func* SetNextSiblingControlToFocus)( control_handle, control_handle );

   api_bool       (api_func* GetControlUpdatesEnabled)( const_control_handle );
   void           (api_func* SetControlUpdatesEnabled)( control_handle, api_bool );

   void           (api_func* UpdateControl)( control_handle );
   void           (api_func* UpdateControlRect)( control_handle, int32, int32, int32, int32 );

   void           (api_func* RepaintControl)( control_handle );
   void           (api_func* RepaintControlRect)( control_handle, int32, int32, int32, int32 );

   void           (api_func* RestyleControl)( control_handle );

   void           (api_func* ScrollControl)( control_handle, int32, int32 );
   void           (api_func* ScrollControlRect)( control_handle, int32, int32, int32, int32, int32, int32 );

   cursor_handle  (api_func* GetControlCursor)( const_control_handle );
   void           (api_func* SetControlCursor)( control_handle, const_cursor_handle );
   void           (api_func* SetControlCursorToParent)( control_handle );

   api_bool       (api_func* GetControlStyleSheet)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetControlStyleSheet)( control_handle, const char16_type* );

   uint32         (api_func* GetControlBackgroundColor)( const_control_handle );
   void           (api_func* SetControlBackgroundColor)( control_handle, uint32 );

   uint32         (api_func* GetControlForegroundColor)( const_control_handle );
   void           (api_func* SetControlForegroundColor)( control_handle, uint32 );

   uint32         (api_func* GetControlCanvasColor)( const_control_handle );
   void           (api_func* SetControlCanvasColor)( control_handle, uint32 );

   uint32         (api_func* GetControlAlternateCanvasColor)( const_control_handle );
   void           (api_func* SetControlAlternateCanvasColor)( control_handle, uint32 );

   uint32         (api_func* GetControlTextColor)( const_control_handle );
   void           (api_func* SetControlTextColor)( control_handle, uint32 );

   uint32         (api_func* GetControlButtonColor)( const_control_handle );
   void           (api_func* SetControlButtonColor)( control_handle, uint32 );

   uint32         (api_func* GetControlButtonTextColor)( const_control_handle );
   void           (api_func* SetControlButtonTextColor)( control_handle, uint32 );

   uint32         (api_func* GetControlHighlightColor)( const_control_handle );
   void           (api_func* SetControlHighlightColor)( control_handle, uint32 );

   uint32         (api_func* GetControlHighlightedTextColor)( const_control_handle );
   void           (api_func* SetControlHighlightedTextColor)( control_handle, uint32 );

   font_handle    (api_func* GetControlFont)( const_control_handle );
   void           (api_func* SetControlFont)( control_handle, const_font_handle );

   void           (api_func* GetWindowOpacity)( const_control_handle, double* );
   void           (api_func* SetWindowOpacity)( control_handle, double );

   api_bool       (api_func* GetWindowTitle)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetWindowTitle)( control_handle, const char16_type* );

   api_bool       (api_func* GetInfoText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetInfoText)( control_handle, const char16_type* );

   api_bool       (api_func* GetRealTimePreviewActive)( const_control_handle );
   void           (api_func* SetRealTimePreviewActive)( control_handle, api_bool );

   api_bool       (api_func* GetTrackViewActive)( const_control_handle );
   void           (api_func* SetTrackViewActive)( control_handle, api_bool );

   api_bool       (api_func* GetWindowToolTip)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetWindowToolTip)( control_handle, const char16_type* );

   api_bool       (api_func* GetControlDisplayPixelRatio)( const_control_handle, double* );
   api_bool       (api_func* GetControlResourcePixelRatio)( const_control_handle, double* );
   api_bool       (api_func* GetControlDevicePixelRatio)( const_control_handle, double* );

   api_bool       (api_func* SetDestroyEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetShowEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetHideEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetCloseEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetGetFocusEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetLoseFocusEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetEnterEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetLeaveEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (api_func* SetMoveEventRoutine)( control_handle, api_handle, pcl::move_event_routine );
   api_bool       (api_func* SetResizeEventRoutine)( control_handle, api_handle, pcl::resize_event_routine );
   api_bool       (api_func* SetPaintEventRoutine)( control_handle, api_handle, pcl::paint_event_routine );
   api_bool       (api_func* SetKeyPressEventRoutine)( control_handle, api_handle, pcl::keyboard_event_routine );
   api_bool       (api_func* SetKeyReleaseEventRoutine)( control_handle, api_handle, pcl::keyboard_event_routine );
   api_bool       (api_func* SetMouseMoveEventRoutine)( control_handle, api_handle, pcl::mouse_event_routine );
   api_bool       (api_func* SetMouseDoubleClickEventRoutine)( control_handle, api_handle, pcl::mouse_event_routine );
   api_bool       (api_func* SetMousePressEventRoutine)( control_handle, api_handle, pcl::mouse_button_event_routine );
   api_bool       (api_func* SetMouseReleaseEventRoutine)( control_handle, api_handle, pcl::mouse_button_event_routine );
   api_bool       (api_func* SetWheelEventRoutine)( control_handle, api_handle, pcl::wheel_event_routine );
   api_bool       (api_func* SetChildCreateEventRoutine)( control_handle, api_handle, pcl::child_event_routine );
   api_bool       (api_func* SetChildDestroyEventRoutine)( control_handle, api_handle, pcl::child_event_routine );
};

/*
 * DialogContext
 */
struct api_context DialogContext
{
   control_handle (api_func* CreateDialog)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (api_func* ExecuteDialog)( control_handle );
   void           (api_func* OpenDialog)( control_handle );

   void           (api_func* ReturnDialog)( control_handle, int32 );

   api_bool       (api_func* GetDialogResizable)( const_control_handle );
   void           (api_func* SetDialogResizable)( control_handle, api_bool );

   api_bool       (api_func* SetExecuteDialogEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (api_func* SetReturnDialogEventRoutine)( control_handle, api_handle, pcl::value_event_routine );

   api_bool       (api_func* ExecuteOpenFileDialog)( char16_type* fileName,
                                 const char16_type* caption, const char16_type* initialPath,
                                 const char16_type* filters, const char16_type* selectedExtension );

   api_bool       (api_func* ExecuteOpenMultipleFilesDialog)( char16_type* fileName,
                                 ::file_enumeration_callback, void*,
                                 const char16_type* caption, const char16_type* initialPath,
                                 const char16_type* filters, const char16_type* selectedExtension );

   api_bool       (api_func* ExecuteSaveFileDialog)( char16_type* filePath,
                                 const char16_type* caption, const char16_type* initialPath,
                                 const char16_type* filters, const char16_type* selectedExtension,
                                 api_bool overwritePrompt );

   api_bool       (api_func* ExecuteGetDirectoryDialog)( char16_type* dirPath,
                                 const char16_type* caption, const char16_type* initialPath );
};

/*
 * FrameContext
 */
struct api_context FrameContext
{
   control_handle (api_func* CreateFrame)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (api_func* GetFrameStyle)( const_control_handle );
   void           (api_func* SetFrameStyle)( control_handle, int32 );

   int32          (api_func* GetFrameLineWidth)( const_control_handle );
   void           (api_func* SetFrameLineWidth)( control_handle, int32 );

   int32          (api_func* GetFrameBorderWidth)( const_control_handle );
};

/*
 * GroupBoxContext
 */
struct api_context GroupBoxContext
{
   control_handle (api_func* CreateGroupBox)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (api_func* GetGroupBoxTitle)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetGroupBoxTitle)( control_handle, const char16_type* );

   api_bool       (api_func* GetGroupBoxCheckable)( const_control_handle );
   void           (api_func* SetGroupBoxCheckable)( control_handle, api_bool );

   api_bool       (api_func* GetGroupBoxChecked)( const_control_handle ); // returns true if group box checked
   void           (api_func* SetGroupBoxChecked)( control_handle, api_bool );

   api_bool       (api_func* SetGroupBoxCheckEventRoutine)( control_handle, api_handle, pcl::button_check_event_routine );
};

/*
 * TabBoxContext
 */
struct api_context TabBoxContext
{
   control_handle (api_func* CreateTabBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (api_func* GetTabBoxLength)( const_control_handle );

   int32          (api_func* GetTabBoxCurrentPageIndex)( const_control_handle );
   void           (api_func* SetTabBoxCurrentPageIndex)( control_handle, int32 );

   control_handle (api_func* GetTabBoxPageByIndex)( const_control_handle, int32 ); // returns client handle

   void           (api_func* InsertTabBoxPage)( control_handle, int32, control_handle, const char16_type*, const_bitmap_handle );

   void           (api_func* RemoveTabBoxPage)( control_handle, int32 );

   int32          (api_func* GetTabBoxPosition)( const_control_handle );  // 0=top, 1=bottom
   void           (api_func* SetTabBoxPosition)( control_handle, int32 ); //   idem.

   api_bool       (api_func* GetTabBoxPageEnabled)( const_control_handle, int32 );
   void           (api_func* SetTabBoxPageEnabled)( control_handle, int32, api_bool );

   api_bool       (api_func* GetTabBoxPageLabel)( const_control_handle, int32, char16_type*, size_type* );
   void           (api_func* SetTabBoxPageLabel)( control_handle, int32, const char16_type* );

   bitmap_handle  (api_func* GetTabBoxPageIcon)( const_control_handle, int32 );
   void           (api_func* SetTabBoxPageIcon)( control_handle, int32, const_bitmap_handle );

   api_bool       (api_func* GetTabBoxPageToolTip)( const_control_handle, int32, char16_type*, size_type* );
   void           (api_func* SetTabBoxPageToolTip)( control_handle, int32, const char16_type* );

   control_handle (api_func* GetTabBoxLeftControl)( const_control_handle ); // returns client handle
   control_handle (api_func* GetTabBoxRightControl)( const_control_handle ); // returns client handle
   void           (api_func* SetTabBoxControls)( control_handle, control_handle, control_handle );

   api_bool       (api_func* SetTabBoxPageSelectedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
};

/*
 * ButtonContext
 */
struct api_context ButtonContext
{
   control_handle (api_func* CreatePushButton)( api_handle, api_handle client, const char16_type*, const_bitmap_handle, control_handle parent, uint32 flags );
   control_handle (api_func* CreateCheckBox)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );
   control_handle (api_func* CreateRadioButton)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );
   control_handle (api_func* CreateToolButton)( api_handle, api_handle client, const char16_type*, const_bitmap_handle, api_bool checkable, control_handle parent, uint32 flags );

   api_bool       (api_func* GetButtonText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetButtonText)( control_handle, const char16_type* );

   bitmap_handle  (api_func* GetButtonIcon)( const_control_handle );
   void           (api_func* SetButtonIcon)( control_handle, const_bitmap_handle );

   void           (api_func* GetButtonIconSize)( const_control_handle, int32* w, int32* h );
   void           (api_func* SetButtonIconSize)( control_handle, int32 w, int32 h );

   api_bool       (api_func* GetButtonPushed)( const_control_handle ); // returns true if button pushed
   void           (api_func* SetButtonPushed)( control_handle, api_bool );

   uint32         (api_func* GetButtonChecked)( const_control_handle ); // returns 0=unchecked, 1=checked, 2=tristate
   void           (api_func* SetButtonChecked)( control_handle, uint32 );

   api_bool       (api_func* GetButtonDefaultEnabled)( const_control_handle ); // returns true if button is default
   void           (api_func* SetButtonDefaultEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetButtonTristateEnabled)( const_control_handle ); // returns true if button is in tristate mode
   void           (api_func* SetButtonTristateEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetToolButtonCheckable)( const_control_handle );
   void           (api_func* SetToolButtonCheckable)( control_handle, api_bool );

   api_bool       (api_func* SetButtonClickEventRoutine)( control_handle, api_handle, pcl::button_click_event_routine );
   api_bool       (api_func* SetButtonPressEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (api_func* SetButtonReleaseEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (api_func* SetButtonCheckEventRoutine)( control_handle, api_handle, pcl::button_check_event_routine );
};

/*
 * EditContext
 */
struct api_context EditContext
{
   control_handle (api_func* CreateEdit)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (api_func* GetEditText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetEditText)( control_handle, const char16_type* );

   api_bool       (api_func* GetEditReadOnly)( const_control_handle );
   void           (api_func* SetEditReadOnly)( control_handle, api_bool );

   api_bool       (api_func* GetEditModified)( const_control_handle );
   void           (api_func* SetEditModified)( control_handle, api_bool );

   api_bool       (api_func* GetEditPasswordEnabled)( const_control_handle );
   void           (api_func* SetEditPasswordEnabled)( control_handle, api_bool );

   int32          (api_func* GetEditMaxLength)( const_control_handle );
   void           (api_func* SetEditMaxLength)( control_handle, int32 );

   api_bool       (api_func* GetEditMask)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetEditMask)( control_handle, const char16_type* );

   void           (api_func* SetEditSelected)( control_handle, api_bool );

   int32          (api_func* GetEditAlignment)( const_control_handle );  // only left and right alignments
   void           (api_func* SetEditAlignment)( control_handle, int32 ); //    idem.

   int32          (api_func* GetEditCaretPosition)( const_control_handle );
   void           (api_func* SetEditCaretPosition)( control_handle, int32 );

   void           (api_func* GetEditSelection)( const_control_handle, int32*, int32* );
   void           (api_func* SetEditSelection)( control_handle, int32, int32 );

   api_bool       (api_func* GetEditSelectedText)( const_control_handle, char16_type*, size_type* );

   api_bool       (api_func* SetEditCompletedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (api_func* SetReturnPressedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (api_func* SetTextUpdatedEventRoutine)( control_handle, api_handle, pcl::unicode_event_routine );
   api_bool       (api_func* SetCaretPositionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (api_func* SetSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

/*
 * TextBoxContext
 */
struct api_context TextBoxContext
{
   control_handle (api_func* CreateTextBox)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (api_func* GetTextBoxText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetTextBoxText)( control_handle, const char16_type* );

   api_bool       (api_func* GetTextBoxReadOnly)( const_control_handle );
   void           (api_func* SetTextBoxReadOnly)( control_handle, api_bool );

   void           (api_func* SetTextBoxSelected)( control_handle, api_bool );

   int32          (api_func* GetTextBoxCaretPosition)( const_control_handle );
   void           (api_func* SetTextBoxCaretPosition)( control_handle, int32 );

   void           (api_func* GetTextBoxSelection)( const_control_handle, int32*, int32* );
   void           (api_func* SetTextBoxSelection)( control_handle, int32, int32 );

   api_bool       (api_func* GetTextBoxSelectedText)( const_control_handle, char16_type*, size_type* );

   void           (api_func* InsertTextBoxText)( control_handle, const char16_type* );
   void           (api_func* DeleteTextBoxText)( control_handle );

   api_bool       (api_func* SetTextBoxUpdatedEventRoutine)( control_handle, api_handle, pcl::unicode_event_routine );
   api_bool       (api_func* SetTextBoxCaretPositionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (api_func* SetTextBoxSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

/*
 * ComboBoxContext
 */
struct api_context ComboBoxContext
{
   control_handle (api_func* CreateComboBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (api_func* GetComboBoxLength)( const_control_handle );

   int32          (api_func* GetComboBoxCurrentItem)( const_control_handle );
   void           (api_func* SetComboBoxCurrentItem)( control_handle, int32 );

   int32          (api_func* FindComboBoxItem)( const_control_handle, const char16_type*, int32, api_bool exactMatch, api_bool caseSensitive );

   void           (api_func* InsertComboBoxItem)( control_handle, int32, const char16_type*, const_bitmap_handle );

   void           (api_func* RemoveComboBoxItem)( control_handle, int32 );

   void           (api_func* ClearComboBox)( control_handle );

   api_bool       (api_func* GetComboBoxItemText)( const_control_handle, int32, char16_type*, size_type* );
   void           (api_func* SetComboBoxItemText)( control_handle, int32, const char16_type* );

   bitmap_handle  (api_func* GetComboBoxItemIcon)( const_control_handle, int32 );
   void           (api_func* SetComboBoxItemIcon)( control_handle, int32, const_bitmap_handle );

   api_bool       (api_func* GetComboBoxEditEnabled)( const_control_handle );
   void           (api_func* SetComboBoxEditEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetComboBoxEditText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetComboBoxEditText)( control_handle, const char16_type* );

   api_bool       (api_func* GetComboBoxAutoCompletionEnabled)( const_control_handle );
   void           (api_func* SetComboBoxAutoCompletionEnabled)( control_handle, api_bool );

   void           (api_func* GetComboBoxIconSize)( const_control_handle, int32*, int32* );
   void           (api_func* SetComboBoxIconSize)( control_handle, int32, int32 );

   int32          (api_func* GetComboBoxMaxVisibleItemCount)( const_control_handle );
   void           (api_func* SetComboBoxMaxVisibleItemCount)( control_handle, int32 );

   int32          (api_func* GetComboBoxMinItemCharWidth)( const_control_handle );
   void           (api_func* SetComboBoxMinItemCharWidth)( control_handle, int32 );

   void           (api_func* SetComboBoxListVisible)( control_handle, api_bool );

   api_bool       (api_func* SetComboBoxItemSelectedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (api_func* SetComboBoxItemHighlightedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (api_func* SetComboBoxEditTextUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );
};

/*
 * SliderContext
 */
struct api_context SliderContext
{
   control_handle (api_func* CreateSlider)( api_handle, api_handle client, api_bool vertical, control_handle parent, uint32 flags );

   int32          (api_func* GetSliderValue)( const_control_handle );
   void           (api_func* SetSliderValue)( control_handle, int32 );

   void           (api_func* GetSliderRange)( const_control_handle, int32*, int32* );
   void           (api_func* SetSliderRange)( control_handle, int32, int32 );

   int32          (api_func* GetSliderStepSize)( const_control_handle );
   void           (api_func* SetSliderStepSize)( control_handle, int32 );

   int32          (api_func* GetSliderPageSize)( const_control_handle );
   void           (api_func* SetSliderPageSize)( control_handle, int32 );

   int32          (api_func* GetSliderTickInterval)( const_control_handle );
   void           (api_func* SetSliderTickInterval)( control_handle, int32 );

   int32          (api_func* GetSliderTickStyle)( const_control_handle );
   void           (api_func* SetSliderTickStyle)( control_handle, int32 );

   api_bool       (api_func* GetSliderTrackingEnabled)( const_control_handle );
   void           (api_func* SetSliderTrackingEnabled)( control_handle, api_bool );

   api_bool       (api_func* SetSliderValueUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (api_func* SetSliderRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

/*
 * SpinBoxContext
 */
struct api_context SpinBoxContext
{
   control_handle (api_func* CreateSpinBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (api_func* GetSpinBoxValue)( const_control_handle );
   void           (api_func* SetSpinBoxValue)( control_handle, int32 );

   void           (api_func* GetSpinBoxRange)( const_control_handle, int32*, int32* );
   void           (api_func* SetSpinBoxRange)( control_handle, int32, int32 );

   int32          (api_func* GetSpinBoxStepSize)( const_control_handle );
   void           (api_func* SetSpinBoxStepSize)( control_handle, int32 );

   api_bool       (api_func* GetSpinBoxWrappingEnabled)( const_control_handle );
   void           (api_func* SetSpinBoxWrappingEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetSpinBoxEditable)( const_control_handle );
   void           (api_func* SetSpinBoxEditable)( control_handle, api_bool );

   api_bool       (api_func* GetSpinBoxPrefix)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetSpinBoxPrefix)( control_handle, const char16_type* );

   api_bool       (api_func* GetSpinBoxSuffix)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetSpinBoxSuffix)( control_handle, const char16_type* );

   api_bool       (api_func* GetSpinBoxMinimumValueText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetSpinBoxMinimumValueText)( control_handle, const char16_type* );

   int32          (api_func* GetSpinBoxAlignment)( const_control_handle );  // only left and right alignments
   void           (api_func* SetSpinBoxAlignment)( control_handle, int32 ); //    idem.

   api_bool       (api_func* SetSpinBoxValueUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (api_func* SetSpinBoxRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

/*
 * LabelContext
 */
struct api_context LabelContext
{
   control_handle (api_func* CreateLabel)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (api_func* GetLabelText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetLabelText)( control_handle, const char16_type* );

   int32          (api_func* GetLabelMargin)( const_control_handle );
   void           (api_func* SetLabelMargin)( control_handle, int32 );

   int32          (api_func* GetLabelAlignment)( const_control_handle );
   void           (api_func* SetLabelAlignment)( control_handle, int32 );

   api_bool       (api_func* GetLabelWordWrappingEnabled)( const_control_handle );
   void           (api_func* SetLabelWordWrappingEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetLabelRichTextEnabled)( const_control_handle );
   void           (api_func* SetLabelRichTextEnabled)( control_handle, api_bool );
};

/*
 * BitmapBoxContext
 */
struct api_context BitmapBoxContext
{
   control_handle (api_func* CreateBitmapBox)( api_handle, api_handle client, const_bitmap_handle, control_handle parent, uint32 flags );

   bitmap_handle  (api_func* GetBitmapBoxBitmap)( const_control_handle );
   void           (api_func* SetBitmapBoxBitmap)( control_handle, const_bitmap_handle );

   int32          (api_func* GetBitmapBoxMargin)( const_control_handle );
   void           (api_func* SetBitmapBoxMargin)( control_handle, int32 );

   api_bool       (api_func* GetBitmapBoxAutoFitEnabled)( const_control_handle );
   void           (api_func* SetBitmapBoxAutoFitEnabled)( control_handle, api_bool );
};

/*
 * ScrollBoxContext
 */
struct api_context ScrollBoxContext
{
   control_handle (api_func* CreateScrollBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   control_handle (api_func* CreateScrollBoxViewport)( control_handle, api_handle client );

   void           (api_func* GetScrollBarsVisible)( const_control_handle, api_bool*, api_bool* );
   void           (api_func* SetScrollBarsVisible)( control_handle, api_bool, api_bool );

   void           (api_func* GetScrollBoxAutoScrollEnabled)( const_control_handle, api_bool*, api_bool* );
   void           (api_func* SetScrollBoxAutoScrollEnabled)( control_handle, api_bool, api_bool );

   void           (api_func* GetScrollBoxHorizontalRange)( const_control_handle, int32*, int32* );
   void           (api_func* SetScrollBoxHorizontalRange)( control_handle, int32, int32 );

   void           (api_func* GetScrollBoxVerticalRange)( const_control_handle, int32*, int32* );
   void           (api_func* SetScrollBoxVerticalRange)( control_handle, int32, int32 );

   void           (api_func* GetScrollBoxPageSize)( const_control_handle, int32*, int32* );
   void           (api_func* SetScrollBoxPageSize)( control_handle, int32, int32 );

   void           (api_func* GetScrollBoxLineSize)( const_control_handle, int32*, int32* );
   void           (api_func* SetScrollBoxLineSize)( control_handle, int32, int32 );

   void           (api_func* GetScrollBoxPosition)( const_control_handle, int32*, int32* );
   void           (api_func* SetScrollBoxPosition)( control_handle, int32, int32 );

   void           (api_func* GetScrollBoxTrackingEnabled)( const_control_handle, api_bool*, api_bool* );
   void           (api_func* SetScrollBoxTrackingEnabled)( control_handle, api_bool, api_bool );

   api_bool       (api_func* SetScrollBoxHorizontalPosUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (api_func* SetScrollBoxVerticalPosUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );

   api_bool       (api_func* SetScrollBoxHorizontalRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (api_func* SetScrollBoxVerticalRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

/*
 * TreeBoxContext
 */
struct api_context TreeBoxContext
{
   control_handle (api_func* CreateTreeBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   api_handle     (api_func* CreateTreeBoxNode)( api_handle, api_handle nodeClient );

   int32          (api_func* GetTreeBoxChildCount)( const_control_handle );

   api_handle     (api_func* GetTreeBoxChild)( const_control_handle, int32 idx ); // returns client handle

   int32          (api_func* GetTreeBoxChildIndex)( const_control_handle, const_api_handle );

   void           (api_func* InsertTreeBoxNode)( control_handle, int32, api_handle );
   void           (api_func* RemoveTreeBoxNode)( control_handle, int32 );

   void           (api_func* ClearTreeBox)( control_handle );

   api_bool       (api_func* GetTreeBoxUniformRowHeightEnabled)( const_control_handle );
   void           (api_func* SetTreeBoxUniformRowHeightEnabled)( control_handle, api_bool );

   api_handle     (api_func* GetTreeBoxCurrentNode)( const_control_handle ); // returns client handle
   void           (api_func* SetTreeBoxCurrentNode)( control_handle, api_handle );

   api_bool       (api_func* GetTreeBoxMultipleNodeSelectionEnabled)( const_control_handle );
   void           (api_func* SetTreeBoxMultipleNodeSelectionEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetTreeBoxSelectedNodes)( const_control_handle, api_handle*, size_type* );

   void           (api_func* SelectAllTreeBoxNodes)( control_handle );

   void           (api_func* BeginTreeBoxNodeEdition)( control_handle, api_handle, int32 col );
   void           (api_func* EndTreeBoxNodeEdition)( control_handle, api_handle, int32 col );

   void           (api_func* EditTreeBoxNode)( control_handle, api_handle, int32 col );

   api_handle     (api_func* GetTreeBoxNodeByPos)( const_control_handle, int32 x, int32 y ); // returns client handle

   void           (api_func* SetTreeBoxNodeIntoView)( control_handle, api_handle );

   void           (api_func* GetTreeBoxNodeRect)( const_control_handle, const_api_handle, int32*, int32*, int32*, int32* );

   int32          (api_func* GetTreeBoxColumnCount)( const_control_handle );
   void           (api_func* SetTreeBoxColumnCount)( control_handle, int32 );

   api_bool       (api_func* GetTreeBoxColumnVisible)( const_control_handle, int32 );
   void           (api_func* SetTreeBoxColumnVisible)( control_handle, int32, api_bool );

   int32          (api_func* GetTreeBoxColumnWidth)( const_control_handle, int32 );
   void           (api_func* SetTreeBoxColumnWidth)( control_handle, int32, int32 );

   void           (api_func* AdjustTreeBoxColumnWidthToContents)( control_handle, int32 );

   api_bool       (api_func* GetTreeBoxHeaderText)( const_control_handle, int32, char16_type*, size_type* );
   void           (api_func* SetTreeBoxHeaderText)( control_handle, int32, const char16_type* );

   bitmap_handle  (api_func* GetTreeBoxHeaderIcon)( const_control_handle, int32 );
   void           (api_func* SetTreeBoxHeaderIcon)( control_handle, int32, const_bitmap_handle );

   int32          (api_func* GetTreeBoxHeaderAlignment)( const_control_handle, int32 );
   void           (api_func* SetTreeBoxHeaderAlignment)( control_handle, int32, int32 );

   api_bool       (api_func* GetTreeBoxHeaderVisible)( const_control_handle );
   void           (api_func* SetTreeBoxHeaderVisible)( control_handle, api_bool );

   int32          (api_func* GetTreeBoxIndentSize)( const_control_handle );
   void           (api_func* SetTreeBoxIndentSize)( control_handle, int32 );

   api_bool       (api_func* GetTreeBoxNodeExpansionEnabled)( const_control_handle );
   void           (api_func* SetTreeBoxNodeExpansionEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetTreeBoxRootDecorationEnabled)( const_control_handle );
   void           (api_func* SetTreeBoxRootDecorationEnabled)( control_handle, api_bool );

   api_bool       (api_func* GetTreeBoxAlternateRowColorEnabled)( const_control_handle );
   void           (api_func* SetTreeBoxAlternateRowColorEnabled)( control_handle, api_bool );

   void           (api_func* GetTreeBoxIconSize)( const_control_handle, int32*, int32* );
   void           (api_func* SetTreeBoxIconSize)( control_handle, int32, int32 );

   api_bool       (api_func* GetTreeBoxHeaderSortingEnabled)( const_control_handle );
   void           (api_func* SetTreeBoxHeaderSortingEnabled)( control_handle, api_bool );

   void           (api_func* SortTreeBox)( control_handle, int32 col, api_bool ascending );

   api_bool       (api_func* GetTreeBoxNodeDraggingEnabled)( const_control_handle );
   void           (api_func* SetTreeBoxNodeDraggingEnabled)( control_handle, api_bool );

   // TreeBox Nodes

   control_handle (api_func* GetTreeBoxNodeParentBox)( const_api_handle ); // returns client handle

   api_handle     (api_func* GetTreeBoxNodeParent)( const_api_handle ); // returns client handle

   int32          (api_func* GetTreeBoxNodeChildCount)( const_api_handle );

   api_handle     (api_func* GetTreeBoxNodeChild)( const_api_handle, int32 ); // returns client handle

   void           (api_func* InsertTreeBoxNodeChild)( api_handle, int32, api_handle );
   void           (api_func* RemoveTreeBoxNodeChild)( api_handle, int32 );

   api_bool       (api_func* GetTreeBoxNodeEnabled)( const_api_handle );
   void           (api_func* SetTreeBoxNodeEnabled)( api_handle, api_bool );

   api_bool       (api_func* GetTreeBoxNodeExpanded)( const_api_handle );
   void           (api_func* SetTreeBoxNodeExpanded)( api_handle, api_bool );

   api_bool       (api_func* GetTreeBoxNodeSelectable)( const_api_handle );
   void           (api_func* SetTreeBoxNodeSelectable)( api_handle, api_bool );

   api_bool       (api_func* GetTreeBoxNodeSelected)( const_api_handle );
   void           (api_func* SetTreeBoxNodeSelected)( api_handle, api_bool );

   api_bool       (api_func* GetTreeBoxNodeCheckable)( const_api_handle );
   void           (api_func* SetTreeBoxNodeCheckable)( api_handle, api_bool );

   api_bool       (api_func* GetTreeBoxNodeChecked)( const_api_handle );
   void           (api_func* SetTreeBoxNodeChecked)( api_handle, api_bool );

   api_bool       (api_func* GetTreeBoxNodeEditable)( const_api_handle );
   void           (api_func* SetTreeBoxNodeEditable)( api_handle, api_bool );

   api_bool       (api_func* GetTreeBoxNodeColText)( const_api_handle, int32, char16_type*, size_type* );
   void           (api_func* SetTreeBoxNodeColText)( api_handle, int32, const char16_type* );

   bitmap_handle  (api_func* GetTreeBoxNodeColIcon)( const_api_handle, int32 );
   void           (api_func* SetTreeBoxNodeColIcon)( api_handle, int32, const_bitmap_handle );

   int32          (api_func* GetTreeBoxNodeColAlignment)( const_api_handle, int32 );
   void           (api_func* SetTreeBoxNodeColAlignment)( api_handle, int32, int32 );

   api_bool       (api_func* GetTreeBoxNodeColToolTip)( const_api_handle, int32, char16_type*, size_type* );
   void           (api_func* SetTreeBoxNodeColToolTip)( api_handle, int32, const char16_type* );

   font_handle    (api_func* GetTreeBoxNodeColFont)( const_api_handle, int32 );
   void           (api_func* SetTreeBoxNodeColFont)( api_handle, int32, const_font_handle );

   uint32         (api_func* GetTreeBoxNodeColBackgroundColor)( const_api_handle, int32 );
   void           (api_func* SetTreeBoxNodeColBackgroundColor)( api_handle, int32, uint32 );

   uint32         (api_func* GetTreeBoxNodeColTextColor)( const_api_handle, int32 );
   void           (api_func* SetTreeBoxNodeColTextColor)( api_handle, int32, uint32 );

   // TreeBox Events

   api_bool       (api_func* SetTreeBoxCurrentNodeUpdatedEventRoutine)( control_handle, api_handle, pcl::item_range_event_routine );
   api_bool       (api_func* SetTreeBoxNodeActivatedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (api_func* SetTreeBoxNodeUpdatedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (api_func* SetTreeBoxNodeEnteredEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (api_func* SetTreeBoxNodeClickedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (api_func* SetTreeBoxNodeDoubleClickedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (api_func* SetTreeBoxNodeExpandedEventRoutine)( control_handle, api_handle, pcl::item_event_routine );
   api_bool       (api_func* SetTreeBoxNodeCollapsedEventRoutine)( control_handle, api_handle, pcl::item_event_routine );
   api_bool       (api_func* SetTreeBoxNodeSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );
};

/*
 * TimerContext
 */
struct api_context TimerContext
{
   timer_handle   (api_func* CreateTimer)( api_handle, api_handle client, uint32 flags );

   void           (api_func* GetTimerInterval)( const_timer_handle, uint32* msec );
   void           (api_func* SetTimerInterval)( timer_handle, uint32 msec );

   api_bool       (api_func* GetTimerSingleShot)( const_timer_handle );
   void           (api_func* SetTimerSingleShot)( timer_handle, api_bool );

   api_bool       (api_func* IsTimerActive)( const_timer_handle );

   api_bool       (api_func* StartTimer)( timer_handle );
   void           (api_func* StopTimer)( timer_handle );

   api_bool       (api_func* SetTimerNotifyEventRoutine)( timer_handle, api_handle, pcl::timer_event_routine );
};

/*
 * ThreadContext
 */
struct api_context ThreadContext
{
   thread_handle  (api_func* CreateThread)( api_handle, api_handle client, uint32 flags );

   void           (api_func* StartThread)( thread_handle, uint32 priority );
   void           (api_func* KillThread)( thread_handle ); // ### dangerous - deprecate?

   api_bool       (api_func* IsThreadActive)( const_thread_handle );

   uint32         (api_func* GetThreadPriority)( const_thread_handle );
   void           (api_func* SetThreadPriority)( thread_handle, uint32 );

   uint32         (api_func* GetThreadStackSize)( const_thread_handle );
   void           (api_func* SetThreadStackSize)( thread_handle, uint32 );

   api_bool       (api_func* WaitThread)( thread_handle, uint32 msec );

   void           (api_func* SleepThread)( thread_handle, uint32 msec );

   uint32         (api_func* GetThreadStatus)( const_thread_handle );
   void           (api_func* SetThreadStatus)( thread_handle, uint32 );

   api_bool       (api_func* GetThreadStatusEx)( const_thread_handle, uint32* status, uint32 flags ); // 0x00=force_lock 0x01=try_lock

   api_bool       (api_func* GetThreadConsoleOutputText)( const_thread_handle, char16_type* text, size_type* len );
   void           (api_func* AppendThreadConsoleOutputText)( thread_handle, const char16_type* text, api_bool appendNewline );
   void           (api_func* ClearThreadConsoleOutputText)( thread_handle );

   thread_handle  (api_func* GetCurrentThread)();

   api_bool       (api_func* SetThreadExecRoutine)( thread_handle, pcl::thread_exec_routine );
};

/*
 * MutexContext
 */
struct api_context MutexContext
{
   mutex_handle   (api_func* CreateMutex)( api_handle, api_handle client, uint32 flags ); // ### deprecated
   mutex_handle   (api_func* CreateReadWriteMutex)( api_handle, api_handle client, uint32 flags );

   api_bool       (api_func* GetLockState)( const_mutex_handle ); // ### disabled ### returns api_true if the mutex is locked

   api_bool       (api_func* Lock)( mutex_handle, api_bool tryLock ); // ### deprecated
   api_bool       (api_func* LockForRead)( mutex_handle, api_bool tryLock );
   api_bool       (api_func* LockForWrite)( mutex_handle, api_bool tryLock );

   void           (api_func* Unlock)( mutex_handle );
};

/*
 * ViewListContext
 */
struct api_context ViewListContext
{
   control_handle (api_func* CreateViewList)( api_handle, api_handle client, control_handle parent, uint32 flags );

   void           (api_func* RegenerateViewList)( control_handle, api_bool mainViews, api_bool previews, api_bool realTimePreview );
   void           (api_func* GetViewListContents)( const_control_handle, api_bool* mainViews, api_bool* previews, api_bool* realTimePreview );

   const_view_handle (api_func* GetViewListExcludedView)( const_control_handle );
   void           (api_func* SetViewListExcludedView)( control_handle, const_view_handle );

   view_handle    (api_func* GetViewListCurrentView)( const_control_handle );
   void           (api_func* SetViewListCurrentView)( control_handle, view_handle );

   api_bool       (api_func* FindViewListView)( const_control_handle, const_view_handle );

   void           (api_func* RemoveViewListView)( control_handle, const_view_handle );

   api_bool       (api_func* SetViewListViewSelectedEventRoutine)( control_handle, api_handle, pcl::view_event_routine );
   api_bool       (api_func* SetViewListCurrentViewUpdatedEventRoutine)( control_handle, api_handle, pcl::view_event_routine );
};

/*
 * BitmapContext
 */
struct api_context BitmapContext
{
   bitmap_handle  (api_func* CreateBitmap)( api_handle, int32, int32, void* );
   bitmap_handle  (api_func* CreateBitmapXPM)( api_handle, const char** );
   bitmap_handle  (api_func* CreateBitmapFromFile)( api_handle, const char16_type* );
   bitmap_handle  (api_func* CreateBitmapFromFile8)( api_handle, const char* );
   bitmap_handle  (api_func* CreateBitmapFromData)( api_handle, const void*, size_type, const char*, uint32 );
   bitmap_handle  (api_func* CreateEmptyBitmap)( api_handle );
   bitmap_handle  (api_func* CloneBitmap)( api_handle, const_bitmap_handle );
   bitmap_handle  (api_func* CloneBitmapRect)( api_handle, const_bitmap_handle, int32, int32, int32, int32 );

   int32          (api_func* GetBitmapFormat)( bitmap_handle );
   void           (api_func* SetBitmapFormat)( bitmap_handle, int32 );

   api_bool       (api_func* GetBitmapDimensions)( const_bitmap_handle, int32*, int32* );
   api_bool       (api_func* IsEmptyBitmap)( const_bitmap_handle );

   uint32         (api_func* GetBitmapPixel)( const_bitmap_handle, int32, int32 );
   void           (api_func* SetBitmapPixel)( bitmap_handle, int32, int32, uint32 );

   uint32*        (api_func* GetBitmapScanLine)( bitmap_handle, int32 );

   bitmap_handle  (api_func* MirroredBitmap)( const_bitmap_handle, api_bool h, api_bool v );
   bitmap_handle  (api_func* ScaledBitmap)( const_bitmap_handle, int32, int32, api_bool );
   bitmap_handle  (api_func* RotatedBitmap)( const_bitmap_handle, double, api_bool );

   api_bool       (api_func* LoadBitmap)( bitmap_handle, const char16_type* );
   api_bool       (api_func* SaveBitmap)( const_bitmap_handle, const char16_type*, int32 q );

   api_bool       (api_func* LoadBitmapData)( bitmap_handle, const void*, size_type, const char*, uint32 );

   void           (api_func* CopyBitmap)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (api_func* FillBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );

   void           (api_func* OrBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );
   void           (api_func* OrBitmaps)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (api_func* AndBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );
   void           (api_func* AndBitmaps)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (api_func* XorBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );
   void           (api_func* XorBitmaps)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (api_func* XorBitmapRect)( bitmap_handle, int32, int32, int32, int32, uint32 );

   void           (api_func* ReplaceBitmapColor)( bitmap_handle, int32, int32, int32, int32, uint32, uint32 );

   void           (api_func* SetBitmapAlpha)( bitmap_handle, int32, int32, int32, int32, uint8 );

   void           (api_func* GetBitmapDevicePixelRatio)( const_bitmap_handle, double* );
   void           (api_func* SetBitmapDevicePixelRatio)( bitmap_handle, double );
};

/*
 * SVGContext
 */
struct api_context SVGContext
{
   svg_handle     (api_func* CreateSVGFile)( api_handle, const char16_type*, int32, int32, uint32 );
   svg_handle     (api_func* CreateSVGBuffer)( api_handle, int32, int32, uint32 );

   api_bool       (api_func* GetSVGDimensions)( const_svg_handle, int32*, int32* );
   api_bool       (api_func* SetSVGDimensions)( svg_handle, int32, int32 );

   api_bool       (api_func* GetSVGViewBox)( const_svg_handle, double*, double*, double*, double* );
   api_bool       (api_func* SetSVGViewBox)( svg_handle, double, double, double, double );

   int32          (api_func* GetSVGResolution)( const_svg_handle );
   void           (api_func* SetSVGResolution)( svg_handle, int32 );

   api_bool       (api_func* GetSVGFilePath)( const_svg_handle, char16_type*, size_type* );

   api_bool       (api_func* GetSVGDataBuffer)( const_svg_handle, void*, size_type* );

   api_bool       (api_func* GetSVGTitle)( const_svg_handle, char16_type*, size_type* );
   void           (api_func* SetSVGTitle)( svg_handle, const char16_type* );

   api_bool       (api_func* GetSVGDescription)( const_svg_handle, char16_type*, size_type* );
   void           (api_func* SetSVGDescription)( svg_handle, const char16_type* );

   api_bool       (api_func* IsSVGPainting)( const_svg_handle );
};

/*
 * BrushContext
 */
struct api_context BrushContext
{
   brush_handle   (api_func* CreateBrush)( api_handle, uint32, int32 );
   brush_handle   (api_func* CreateBitmapBrush)( api_handle, const_bitmap_handle );
   brush_handle   (api_func* CreateLinearGradientBrush)( api_handle, double x1, double y1, double x2, double y2,
                                                         int32 spread, const api_gradient_stop*, size_type count ); // spread: 0=pad 1=reflect 2=repeat
   brush_handle   (api_func* CreateRadialGradientBrush)( api_handle, double cx, double cy, double r, double fx, double fy,
                                                         int32 spread, const api_gradient_stop*, size_type count );
   brush_handle   (api_func* CreateConicalGradientBrush)( api_handle, double cx, double cy, double angle,
                                                         const api_gradient_stop*, size_type count );
   brush_handle   (api_func* CloneBrush)( api_handle, const_brush_handle );

   uint32         (api_func* GetBrushColor)( const_brush_handle );
   void           (api_func* SetBrushColor)( brush_handle, uint32 );

   int32          (api_func* GetBrushStyle)( const_brush_handle );
   void           (api_func* SetBrushStyle)( brush_handle, int32 );

   bitmap_handle  (api_func* GetBrushBitmap)( const_brush_handle );
   void           (api_func* SetBrushBitmap)( brush_handle, const_bitmap_handle );

   int32          (api_func* GetBrushGradientType)( const_brush_handle ); // 0=none 1=linear 2=radial 3=conical
   api_bool       (api_func* GetBrushLinearGradientParameters)( const_brush_handle, double* x1, double* y1, double* x2, double* y2 );
   api_bool       (api_func* GetBrushRadialGradientParameters)( const_brush_handle, double* cx, double* cy, double* r, double* fx, double* fy );
   api_bool       (api_func* GetBrushConicalGradientParameters)( const_brush_handle, double* cx, double* cy, double* angle );
   int32          (api_func* GetBrushGradientSpread)( const_brush_handle ); // -1=error 0=pad 1=reflect 2=repeat
   api_bool       (api_func* GetBrushGradientStops)( const_brush_handle, api_gradient_stop*, size_type *count );
};

/*
 * PenContext
 */
struct api_context PenContext
{
   pen_handle     (api_func* CreatePen)( api_handle, uint32, float, int32, int32, int32 );
   pen_handle     (api_func* ClonePen)( api_handle, const_pen_handle );

   api_bool       (api_func* GetPenWidth)( const_pen_handle, float* );
   void           (api_func* SetPenWidth)( pen_handle, float );

   uint32         (api_func* GetPenColor)( const_pen_handle );
   void           (api_func* SetPenColor)( pen_handle, uint32 );

   int32          (api_func* GetPenStyle)( const_pen_handle );
   void           (api_func* SetPenStyle)( pen_handle, int32 );

   int32          (api_func* GetPenCap)( const_pen_handle );
   void           (api_func* SetPenCap)( pen_handle, int32 );

   int32          (api_func* GetPenJoin)( const_pen_handle );
   void           (api_func* SetPenJoin)( pen_handle, int32 );

   brush_handle   (api_func* GetPenBrush)( const_pen_handle );
   void           (api_func* SetPenBrush)( pen_handle, const_brush_handle );
};

/*
 * FontContext
 */
struct api_context FontContext
{
   font_handle    (api_func* CreateFontByFamily)( api_handle, int32, double );
   font_handle    (api_func* CreateFontByFace)( api_handle, const char16_type*, double );
   font_handle    (api_func* CloneFont)( api_handle, const_font_handle );

   api_bool       (api_func* GetFontFace)( const_font_handle, char16_type*, size_type* );
   void           (api_func* SetFontFace)( font_handle, const char16_type* );

   api_bool       (api_func* GetFontExactMatch)( const_font_handle );

   int32          (api_func* GetFontPixelSize)( const_font_handle );
   void           (api_func* SetFontPixelSize)( font_handle, int32 );

   void           (api_func* GetFontPointSize)( const_font_handle, double* );
   void           (api_func* SetFontPointSize)( font_handle, double );

   api_bool       (api_func* GetFontFixedPitch)( const_font_handle );
   void           (api_func* SetFontFixedPitch)( font_handle, api_bool );

   api_bool       (api_func* GetFontKerning)( const_font_handle );
   void           (api_func* SetFontKerning)( font_handle, api_bool );

   int32          (api_func* GetFontStretchFactor)( const_font_handle );
   void           (api_func* SetFontStretchFactor)( font_handle, int32 );

   int32          (api_func* GetFontWeight)( const_font_handle );
   void           (api_func* SetFontWeight)( font_handle, int32 );

   api_bool       (api_func* GetFontItalic)( const_font_handle );
   void           (api_func* SetFontItalic)( font_handle, api_bool );

   api_bool       (api_func* GetFontUnderline)( const_font_handle );
   void           (api_func* SetFontUnderline)( font_handle, api_bool );

   api_bool       (api_func* GetFontOverline)( const_font_handle );
   void           (api_func* SetFontOverline)( font_handle, api_bool );

   api_bool       (api_func* GetFontStrikeOut)( const_font_handle );
   void           (api_func* SetFontStrikeOut)( font_handle, api_bool );

   int32          (api_func* GetFontAscent)( const_font_handle );
   int32          (api_func* GetFontDescent)( const_font_handle );

   int32          (api_func* GetFontHeight)( const_font_handle );

   int32          (api_func* GetFontLineSpacing)( const_font_handle );

   api_bool       (api_func* GetFontCharDefined)( const_font_handle, int32 );

   int32          (api_func* GetFontMaxWidth)( const_font_handle );

   int32          (api_func* GetStringPixelWidth)( const_font_handle, const char16_type* );
   int32          (api_func* GetCharPixelWidth)( const_font_handle, int32 );
   void           (api_func* GetStringPixelRect)( const_font_handle, const char16_type*, int32*, int32*, int32*, int32*, uint32 flags );

   api_bool       (api_func* EnumerateFonts)( font_enumeration_callback f,
                                                char16_type* fontFace, size_type* len, void* data,
                                                const char* writingSystem );

   api_bool       (api_func* EnumerateWritingSystems)( font_enumeration_callback f,
                                                char16_type* wrSystem, size_type* len, void* data,
                                                const char16_type* font );

   api_bool       (api_func* EnumerateFontStyles)( font_enumeration_callback f,
                                                char16_type* style, size_type* len, void* data,
                                                const char16_type* font );

   api_bool       (api_func* EnumerateOptimalFontPointSizes)(font_size_enumeration_callback f,
                                                double* ptSize, void* data,
                                                const char16_type* font, const char16_type* style );

   api_bool       (api_func* GetFontScalable)( const char16_type* font, const char16_type* style );
   api_bool       (api_func* GetNominalFontFixedPitch)( const char16_type* font, const char16_type* style );
   api_bool       (api_func* GetNominalFontItalic)( const char16_type* font, const char16_type* style );
   int32          (api_func* GetNominalFontWeight)( const char16_type* font, const char16_type* style );
};

/*
 * CursorContext
 */
struct api_context CursorContext
{
   cursor_handle  (api_func* CreateCursor)( api_handle, int32 );
   cursor_handle  (api_func* CreateBitmapCursor)( api_handle, const_bitmap_handle, int32, int32 );
   cursor_handle  (api_func* CloneCursor)( api_handle, const_cursor_handle );

   void           (api_func* GetCursorHotSpot)( const_cursor_handle, int32*, int32* );
};

/*
 * SizerContext
 */
struct api_context SizerContext
{
   sizer_handle   (api_func* CreateSizer)( api_handle, api_bool vertical );

   control_handle (api_func* GetSizerParentControl)( const_sizer_handle ); // returns client handle

   api_bool       (api_func* GetSizerOrientation)( const_sizer_handle ); // true=vertical

   int32          (api_func* GetSizerCount)( const_sizer_handle );

   int32          (api_func* GetSizerIndex)( const_sizer_handle, const_sizer_handle );
   int32          (api_func* GetSizerControlIndex)( const_sizer_handle, const_control_handle );

   void           (api_func* InsertSizer)( sizer_handle, int32, sizer_handle, int32 );
   void           (api_func* InsertSizerControl)( sizer_handle, int32, control_handle, int32, int32 );
   void           (api_func* InsertSizerSpacing)( sizer_handle, int32, int32 );
   void           (api_func* InsertSizerStretch)( sizer_handle, int32, int32 );

   void           (api_func* RemoveSizer)( sizer_handle, sizer_handle );
   void           (api_func* RemoveSizerControl)( sizer_handle, control_handle );

   void           (api_func* SetSizerStretchFactor)( sizer_handle, sizer_handle, int32 );
   void           (api_func* SetSizerControlStretchFactor)( sizer_handle, control_handle, int32 );

   void           (api_func* SetSizerAlignment)( sizer_handle, sizer_handle, int32 );
   void           (api_func* SetSizerControlAlignment)( sizer_handle, control_handle, int32 );

   int32          (api_func* GetSizerMargin)( const_sizer_handle );
   void           (api_func* SetSizerMargin)( sizer_handle, int32 );

   int32          (api_func* GetSizerSpacing)( const_sizer_handle );
   void           (api_func* SetSizerSpacing)( sizer_handle, int32 );

   api_bool       (api_func* GetSizerDisplayPixelRatio)( const_sizer_handle, double* );
   api_bool       (api_func* GetSizerResourcePixelRatio)( const_sizer_handle, double* );
   api_bool       (api_func* GetSizerDevicePixelRatio)( const_sizer_handle, double* );
};

/*
 * GraphicsContext
 */
struct api_context GraphicsContext
{
   graphics_handle (api_func* CreateGraphics)( api_handle );

   api_bool       (api_func* BeginControlPaint)( graphics_handle, control_handle );
   api_bool       (api_func* BeginBitmapPaint)( graphics_handle, bitmap_handle );
   api_bool       (api_func* BeginSVGPaint)( graphics_handle, svg_handle );
   void           (api_func* EndPaint)( graphics_handle );
   api_bool       (api_func* GetGraphicsStatus)( const_graphics_handle );

   api_bool       (api_func* GetGraphicsTransformationEnabled)( const_graphics_handle );
   void           (api_func* EnableGraphicsTransformation)( graphics_handle, api_bool );
   void           (api_func* GetGraphicsTransformationMatrix)( const_graphics_handle, double* m11, double* m12, double* m13, double* m21, double* m22, double* m23, double* m31, double* m32, double* m33 );
   void           (api_func* SetGraphicsTransformationMatrix)( graphics_handle, double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33 );
   void           (api_func* MultiplyGraphicsTransformationMatrix)( graphics_handle, double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33 );
   void           (api_func* RotateGraphicsTransformation)( graphics_handle, double );
   void           (api_func* ScaleGraphicsTransformation)( graphics_handle, double, double );
   void           (api_func* TranslateGraphicsTransformation)( graphics_handle, double, double );
   void           (api_func* ShearGraphicsTransformation)( graphics_handle, double, double );
   void           (api_func* ResetGraphicsTransformation)( graphics_handle );
   void           (api_func* TransformPoints)( const_graphics_handle, double* xy, size_type n );

   api_bool       (api_func* GetGraphicsClippingEnabled)( const_graphics_handle );
   void           (api_func* EnableGraphicsClipping)( graphics_handle, api_bool );
   void           (api_func* GetGraphicsClipRect)( const_graphics_handle, int32*, int32*, int32*, int32* );
   void           (api_func* SetGraphicsClipRect)( graphics_handle, int32, int32, int32, int32 );
   void           (api_func* GetGraphicsClipRectD)( const_graphics_handle, double*, double*, double*, double* );
   void           (api_func* SetGraphicsClipRectD)( graphics_handle, double, double, double, double );

   api_bool       (api_func* GetGraphicsAntialiasingEnabled)( const_graphics_handle );
   void           (api_func* EnableGraphicsAntialiasing)( graphics_handle, api_bool );

   api_bool       (api_func* GetGraphicsTextAntialiasingEnabled)( const_graphics_handle );
   void           (api_func* EnableGraphicsTextAntialiasing)( graphics_handle, api_bool );

   api_bool       (api_func* GetGraphicsSmoothInterpolationEnabled)( const_graphics_handle );
   void           (api_func* EnableGraphicsSmoothInterpolation)( graphics_handle, api_bool );

   int32          (api_func* GetGraphicsCompositionOperator)( const_graphics_handle );
   void           (api_func* SetGraphicsCompositionOperator)( graphics_handle, int32 );

   void           (api_func* GetGraphicsOpacity)( const_graphics_handle, double* );
   void           (api_func* SetGraphicsOpacity)( graphics_handle, double );

   brush_handle   (api_func* GetGraphicsBackgroundBrush)( const_graphics_handle );
   void           (api_func* SetGraphicsBackgroundBrush)( graphics_handle, const_brush_handle );

   api_bool       (api_func* GetGraphicsTransparentBackgroundEnabled)( const_graphics_handle );
   void           (api_func* SetGraphicsTransparentBackground)( graphics_handle, api_bool );

   pen_handle     (api_func* GetGraphicsPen)( const_graphics_handle );
   void           (api_func* SetGraphicsPen)( graphics_handle, const_pen_handle );

   brush_handle   (api_func* GetGraphicsBrush)( const_graphics_handle );
   void           (api_func* SetGraphicsBrush)( graphics_handle, const_brush_handle );

   void           (api_func* GetGraphicsBrushOrigin)( const_graphics_handle, int32*, int32* );
   void           (api_func* SetGraphicsBrushOrigin)( graphics_handle, int32, int32 );
   void           (api_func* GetGraphicsBrushOriginD)( const_graphics_handle, double*, double* );
   void           (api_func* SetGraphicsBrushOriginD)( graphics_handle, double, double );

   font_handle    (api_func* GetGraphicsFont)( const_graphics_handle );
   void           (api_func* SetGraphicsFont)( graphics_handle, const_font_handle );

   void           (api_func* PushGraphicsState)( graphics_handle );
   void           (api_func* PopGraphicsState)( graphics_handle );

   void           (api_func* DrawPoint)( graphics_handle, int32, int32 );
   void           (api_func* DrawPointD)( graphics_handle, double, double );

   void           (api_func* DrawLine)( graphics_handle, int32, int32, int32, int32 );
   void           (api_func* DrawLineD)( graphics_handle, double, double, double, double );

   void           (api_func* DrawRect)( graphics_handle, int32, int32, int32, int32 );
   void           (api_func* StrokeRect)( graphics_handle, int32, int32, int32, int32, const_pen_handle );
   void           (api_func* FillRect)( graphics_handle, int32, int32, int32, int32, const_brush_handle );
   void           (api_func* DrawRectD)( graphics_handle, double, double, double, double );
   void           (api_func* StrokeRectD)( graphics_handle, double, double, double, double, const_pen_handle );
   void           (api_func* FillRectD)( graphics_handle, double, double, double, double, const_brush_handle );

   void           (api_func* DrawRoundedRect)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (api_func* StrokeRoundedRect)( graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle );
   void           (api_func* FillRoundedRect)( graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle );
   void           (api_func* DrawRoundedRectD)( graphics_handle, double, double, double, double, double, double );
   void           (api_func* StrokeRoundedRectD)( graphics_handle, double, double, double, double, double, double, const_pen_handle );
   void           (api_func* FillRoundedRectD)( graphics_handle, double, double, double, double, double, double, const_brush_handle );

   void           (api_func* DrawEllipse)( graphics_handle, int32, int32, int32, int32 );
   void           (api_func* StrokeEllipse)( graphics_handle, int32, int32, int32, int32, const_pen_handle );
   void           (api_func* FillEllipse)( graphics_handle, int32, int32, int32, int32, const_brush_handle );
   void           (api_func* DrawEllipseD)( graphics_handle, double, double, double, double );
   void           (api_func* StrokeEllipseD)( graphics_handle, double, double, double, double, const_pen_handle );
   void           (api_func* FillEllipseD)( graphics_handle, double, double, double, double, const_brush_handle );

   void           (api_func* DrawPolygon)( graphics_handle, const int32*, size_type, int32 );
   void           (api_func* StrokePolygon)( graphics_handle, const int32*, size_type, int32, const_pen_handle );
   void           (api_func* FillPolygon)( graphics_handle, const int32*, size_type, int32, const_brush_handle );
   void           (api_func* DrawPolygonD)( graphics_handle, const double*, size_type, int32 );
   void           (api_func* StrokePolygonD)( graphics_handle, const double*, size_type, int32, const_pen_handle );
   void           (api_func* FillPolygonD)( graphics_handle, const double*, size_type, int32, const_brush_handle );

   void           (api_func* DrawPolyline)( graphics_handle, const int32*, size_type );
   void           (api_func* DrawPolylineD)( graphics_handle, const double*, size_type );

   void           (api_func* DrawArc)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (api_func* DrawArcD)( graphics_handle, double, double, double, double, double, double );

   void           (api_func* DrawChord)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (api_func* StrokeChord)( graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle );
   void           (api_func* FillChord)( graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle );
   void           (api_func* DrawChordD)( graphics_handle, double, double, double, double, double, double );
   void           (api_func* StrokeChordD)( graphics_handle, double, double, double, double, double, double, const_pen_handle );
   void           (api_func* FillChordD)( graphics_handle, double, double, double, double, double, double, const_brush_handle );

   void           (api_func* DrawPie)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (api_func* StrokePie)( graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle );
   void           (api_func* FillPie)( graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle );
   void           (api_func* DrawPieD)( graphics_handle, double, double, double, double, double, double );
   void           (api_func* StrokePieD)( graphics_handle, double, double, double, double, double, double, const_pen_handle );
   void           (api_func* FillPieD)( graphics_handle, double, double, double, double, double, double, const_brush_handle );

   void           (api_func* DrawBitmap)( graphics_handle, int32, int32, const_bitmap_handle );
   void           (api_func* DrawBitmapD)( graphics_handle, double, double, const_bitmap_handle );

   void           (api_func* DrawBitmapRect)( graphics_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );
   void           (api_func* DrawBitmapRectD)( graphics_handle, double, double, const_bitmap_handle, double, double, double, double );

   void           (api_func* DrawScaledBitmap)( graphics_handle, int32, int32, int32, int32, const_bitmap_handle );
   void           (api_func* DrawScaledBitmapD)( graphics_handle, double, double, double, double, const_bitmap_handle );

   void           (api_func* DrawScaledBitmapRect)( graphics_handle, int32, int32, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );
   void           (api_func* DrawScaledBitmapRectD)( graphics_handle, double, double, double, double, const_bitmap_handle, double, double, double, double );

   void           (api_func* DrawTiledBitmap)( graphics_handle, int32, int32, int32, int32, const_bitmap_handle, int32, int32 );
   void           (api_func* DrawTiledBitmapD)( graphics_handle, double, double, double, double, const_bitmap_handle, double, double );

   void           (api_func* DrawText)( graphics_handle, int32, int32, const char16_type* );
   void           (api_func* DrawTextD)( graphics_handle, double, double, const char16_type* );

   void           (api_func* DrawTextRect)( graphics_handle, int32, int32, int32, int32, const char16_type*, int32 );
   void           (api_func* DrawTextRectD)( graphics_handle, double, double, double, double, const char16_type*, int32 );

   void           (api_func* GetTextRect)( graphics_handle, int32, int32, int32, int32, const char16_type*, int32, int32*, int32*, int32*, int32* );
   void           (api_func* GetTextRectD)( graphics_handle, double, double, double, double, const char16_type*, int32, double*, double*, double*, double* );
};

/*
 * ColorManagementContext
 */
struct api_context ColorManagementContext
{
   enum PixelType
   {
      RGB_Int8,
      Gray_Int8,
      RGB_I16,
      Gray_I16,
      RGB_F32,
      Gray_F32,
      RGB_F64,
      Gray_F64,
      NumberOfPixelTypes
   };

   enum Intent // Compatible with pcl::ICCRenderingIntent
   {
      Perceptual,
      Saturation,
      RelativeColorimetric,
      AbsoluteColorimetric,
      NumberOfIntents
   };

   enum Direction
   {
      Input,
      Output,
      Proofing,
      NumberOfDirections
   };

   enum InfoType
   {
      Description,
      Manufacturer,
      Model,
      Copyright,
      NumberOfInfoTypes
   };

   enum Flags
   {
      BlackPointCompensation = 0x0001,
      HighResolutionCLUT     = 0x0002,
      LowResolutionCLUT      = 0x0004,
      GamutCheck             = 0x0100
   };

   icc_profile_handle (api_func* OpenProfile)( const void* data );
   icc_profile_handle (api_func* OpenProfileFile)( const char16_type* filePath );

   api_bool       (api_func* CloseProfile)( icc_profile_handle );

   api_bool       (api_func* IsValidProfile)( const void* data );
   api_bool       (api_func* IsValidProfileFile)( const char16_type* filePath );
   api_bool       (api_func* IsValidProfileHandle)( const_icc_profile_handle );

   int32          (api_func* GetProfileDeviceClass)( const_icc_profile_handle );
   int32          (api_func* GetProfileColorSpace)( const_icc_profile_handle );

   api_bool       (api_func* IsProfileIntentSupported)( const_icc_profile_handle,
                                       int32 intent, int32 direction );

   api_bool       (api_func* GetProfileInformation)( const_icc_profile_handle,
                           int32 infoType, const char* language, const char* country,
                           char16_type*, size_type* );

   api_bool       (api_func* GetLastErrorMessage)( char16_type*, size_type* );

   color_transform_handle (api_func* CreateTransformation)(
                                       icc_profile_handle srcProfile,
                                       icc_profile_handle dstProfile,
                                       int32 srcPixelType,
                                       int32 dstPixelType,
                                       int32 intent,
                                       uint32 flags );

   color_transform_handle (api_func* CreateMultiprofileTransformation)(
                                       icc_profile_handle* profiles,
                                       uint32 count,
                                       int32 srcPixelType,
                                       int32 dstPixelType,
                                       int32 intent,
                                       uint32 flags );

   color_transform_handle (api_func* CreateProofingTransformation)(
                                       icc_profile_handle srcProfile,
                                       icc_profile_handle dstProfile,
                                       icc_profile_handle proofingProfile,
                                       int32 srcPixelType,
                                       int32 dstPixelType,
                                       int32 intent,
                                       int32 proofingIntent,
                                       uint32 flags );

   api_bool       (api_func* DestroyTransformation)( color_transform_handle );

   api_bool       (api_func* Transfrom)( color_transform_handle,  /* thread-safe */
                                         const void* input, void* output,
                                         size_type count );

   api_bool       (api_func* IsValidTransformationHandle)( const_color_transform_handle );

   void           (api_func* SetGamutWarningColor)( float r, float g, float b );
   void           (api_func* GetGamutWarningColor)( float* r, float* g, float* b );
};

/*
 * RealTimePreviewContext
 */
struct api_context RealTimePreviewContext
{
   api_bool       (api_func* SetRealTimePreviewOwner)( interface_handle, uint32 flags );
   api_bool       (api_func* IsRealTimePreviewUpdating)();
   void           (api_func* UpdateRealTimePreview)();
   void           (api_func* ShowRealTimePreviewProgressDialog)( const char16_type* title, const char16_type* text, size_type total, uint32 flags );
   void           (api_func* CloseRealTimePreviewProgressDialog)();
   api_bool       (api_func* IsRealTimePreviewProgressDialogVisible)();
   void           (api_func* SetRealTimePreviewProgressCount)( size_type newCount, uint32 flags );
   void           (api_func* SetRealTimePreviewProgressText)( const char16_type* text, uint32 flags );
};

/*
 * NumericalContext
 */
struct api_context NumericalContext
{
   /*
    * Gauss-Jordan Solver: A*X = B
    * On output: A = Inv(A) and B = X
    */
   api_bool       (api_func* GaussJordanInPlaceF)( float** A, float** B, int32 rows, int32 cols );
   api_bool       (api_func* GaussJordanInPlaceD)( double** A, double** B, int32 rows, int32 cols );

   /*
    * Singular Value Decomposition: A = U*W*Vt
    * On output: A = U
    */
   api_bool       (api_func* SVDInPlaceF)( float** A, float* W, float** V, int32 rows, int32 cols );
   api_bool       (api_func* SVDInPlaceD)( double** A, double* W, double** V, int32 rows, int32 cols );

   /*
    * Linear Fit: y = a + b*x with inimized average absolute deviation.
    * Returns: 1=OK, -1=error, 0=aborted
    */
   api_enum       (api_func* LinearFitF)( double* a, double* b, double* adev,
                                          const float* fx, const float* fy, size_type n,
                                          api_bool (*callback)( void* ), void* );
   api_enum       (api_func* LinearFitD)( double* a, double* b, double* adev,
                                          const double* fx, const double* fy, size_type n,
                                          api_bool (*callback)( void* ), void* );

   /*
    * Cubic Splines
    */
   api_bool       (api_func* CubicSplineGenerateF)( float* dy2, const float* fx, const float* fy, float dy1, float dyn, int32 n );
   api_bool       (api_func* CubicSplineGenerateD)( double* dy2, const double* fx, const double* fy, double dy1, double dyn, int32 n );

   api_bool       (api_func* NaturalCubicSplineGenerateF)( float* dy2, const float* fx, const float* fy, int32 n );
   api_bool       (api_func* NaturalCubicSplineGenerateD)( double* dy2, const double* fx, const double* fy, int32 n );

   api_bool       (api_func* CubicSplineInterpolateF)( float* y, const float* fx, const float* fy, const float* dy2, int32 n,
                                                       double x, int32* k );
   api_bool       (api_func* CubicSplineInterpolateD)( double* y, const double* fx, const double* fy, const double* dy2, int32 n,
                                                       double x, int32* k );

   api_bool       (api_func* NaturalGridCubicSplineGenerateF)( float* dy2, const float* fy, int32 n );
   api_bool       (api_func* NaturalGridCubicSplineGenerateD)( double* dy2, const double* fy, int32 n );
   api_bool       (api_func* NaturalGridCubicSplineGenerateUI8)( float* dy2, const uint8* fy, int32 n );
   api_bool       (api_func* NaturalGridCubicSplineGenerateUI16)( float* dy2, const uint16* fy, int32 n );
   api_bool       (api_func* NaturalGridCubicSplineGenerateUI32)( double* dy2, const uint32* fy, int32 n );

   api_bool       (api_func* NaturalGridCubicSplineInterpolateF)( float* y, const float* fy, const float* dy2, int32 n, double x );
   api_bool       (api_func* NaturalGridCubicSplineInterpolateD)( double* y, const double* fy, const double* dy2, int32 n, double x );
   api_bool       (api_func* NaturalGridCubicSplineInterpolateUI8)( float* y, const uint8* fy, const float* dy2, int32 n, double x );
   api_bool       (api_func* NaturalGridCubicSplineInterpolateUI16)( float* y, const uint16* fy, const float* dy2, int32 n, double x );
   api_bool       (api_func* NaturalGridCubicSplineInterpolateUI32)( double* y, const uint32* fy, const double* dy2, int32 n, double x );

   /*
    * Two-Dimensional Surface Splines (Thin Plates)
    */
   api_bool       (api_func* SurfaceSplineGenerateF)( float* cv, double* rm, double* xm, double* ym,
                                                      float* fx, float* fy, const float* fz, int32 n, int32 m,
                                                      float rho, const float* w );
   api_bool       (api_func* SurfaceSplineGenerateD)( double* cv, double* rm, double* xm, double* ym,
                                                      double* fx, double* fy, const double* fz, int32 n, int32 m,
                                                      float rho, const float* w );

   api_bool       (api_func* SurfaceSplineInterpolateF)( float* z, const float* fx, const float* fy, const float* cv, int32 n, int32 m,
                                                         double x, double y );
   api_bool       (api_func* SurfaceSplineInterpolateD)( double* z, const double* fx, const double* fy, const double* cv, int32 n, int32 m,
                                                         double x, double y );

   /*
    * Fast Fourier Transforms (one-dimensional)
    */
   size_type      (api_func* FFTComplexOptimizedLengthF)( size_type n );
   size_type      (api_func* FFTComplexOptimizedLengthD)( size_type n );

   size_type      (api_func* FFTRealOptimizedLengthF)( size_type n );
   size_type      (api_func* FFTRealOptimizedLengthD)( size_type n );

   fft_handle     (api_func* FFTCreateComplexTransformF)( size_type n );
   fft_handle     (api_func* FFTCreateComplexTransformD)( size_type n );

   fft_handle     (api_func* FFTCreateComplexInverseTransformF)( size_type n );
   fft_handle     (api_func* FFTCreateComplexInverseTransformD)( size_type n );

   fft_handle     (api_func* FFTCreateRealTransformF)( size_type n );
   fft_handle     (api_func* FFTCreateRealTransformD)( size_type n );

   fft_handle     (api_func* FFTCreateRealInverseTransformF)( size_type n );
   fft_handle     (api_func* FFTCreateRealInverseTransformD)( size_type n );

   api_bool       (api_func* FFTDestroyTransform)( fft_handle hFFT );

   api_bool       (api_func* FFTComplexTransformF)( fft_handle hFFT, void* y, const void* x ); // void* = fcomplex*
   api_bool       (api_func* FFTComplexTransformD)( fft_handle hFFT, void* y, const void* x ); // void* = dcomplex*

   api_bool       (api_func* FFTRealTransformF)( fft_handle hFFT, void* y, const float* x );   // void* = fcomplex*
   api_bool       (api_func* FFTRealTransformD)( fft_handle hFFT, void* y, const double* x );  // void* = dcomplex*

   api_bool       (api_func* FFTRealInverseTransformF)( fft_handle hFFT, float* x, const void* y );  // void* = fcomplex*
   api_bool       (api_func* FFTRealInverseTransformD)( fft_handle hFFT, double* x, const void* y ); // void* = dcomplex*
};

/*
 * CompressionContext
 */
struct api_context CompressionContext
{
   uint32         (api_func* ZLibMinUncompressedBlockSize)();
   uint32         (api_func* ZLibMaxUncompressedBlockSize)();
   uint32         (api_func* ZLibMaxCompressedBlockSize)( uint32 size );
   int32          (api_func* ZLibMaxCompressionLevel)();
   int32          (api_func* ZLibDefaultCompressionLevel)();
   api_bool       (api_func* ZLibCompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize, int32 level );
   api_bool       (api_func* ZLibUncompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize );

   uint32         (api_func* LZ4MinUncompressedBlockSize)();
   uint32         (api_func* LZ4MaxUncompressedBlockSize)();
   uint32         (api_func* LZ4MaxCompressedBlockSize)( uint32 size );
   int32          (api_func* LZ4MaxCompressionLevel)();
   int32          (api_func* LZ4DefaultCompressionLevel)();
   api_bool       (api_func* LZ4CompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize, int32 level );
   api_bool       (api_func* LZ4UncompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize );

   uint32         (api_func* LZ4HCMinUncompressedBlockSize)();
   uint32         (api_func* LZ4HCMaxUncompressedBlockSize)();
   uint32         (api_func* LZ4HCMaxCompressedBlockSize)( uint32 size );
   int32          (api_func* LZ4HCMaxCompressionLevel)();
   int32          (api_func* LZ4HCDefaultCompressionLevel)();
   api_bool       (api_func* LZ4HCCompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize, int32 level );
   api_bool       (api_func* LZ4HCUncompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize );

   uint32         (api_func* BloscLZMinUncompressedBlockSize)();
   uint32         (api_func* BloscLZMaxUncompressedBlockSize)();
   uint32         (api_func* BloscLZMaxCompressedBlockSize)( uint32 size );
   int32          (api_func* BloscLZMaxCompressionLevel)();
   int32          (api_func* BloscLZDefaultCompressionLevel)();
   api_bool       (api_func* BloscLZCompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize, int32 level );
   api_bool       (api_func* BloscLZUncompressBlock)( void* outputData, uint32* outputSize, const void* inputData, uint32 inputSize );
};

/*
 * CryptographyContext
 */
struct api_context CryptographyContext
{
   enum HashAlgorithm
   {
      MD5    = 5,
      SHA1   = 1,
      SHA224 = 224,
      SHA256 = 256,
      SHA384 = 384,
      SHA512 = 512
   };

   crypto_handle  (api_func* CreateCryptographicHash)( api_handle, int32 hashAlg );

   api_bool       (api_func* InitializeCryptographicHash)( crypto_handle );
   api_bool       (api_func* UpdateCryptographicHash)( crypto_handle, const void* data, size_type length );
   api_bool       (api_func* FinalizeCryptographicHash)( crypto_handle, void* hash );

   crypto_handle  (api_func* CreateAES256Cipher)( api_handle, void* key ); // key is uint8[32] - key will be wiped out
   api_bool       (api_func* AES256EncryptBlock)( const_crypto_handle, void* output, const void* input ); // blocks are uint8[16]
   api_bool       (api_func* AES256DecryptBlock)( const_crypto_handle, void* output, const void* input ); // blocks are uint8[16]
};

/*
 * SharedImageContext
 */
struct api_context SharedImageContext
{
   image_handle   (api_func* CreateImage)( uint32 w, uint32 h, uint32 n, uint32 nbits, api_bool flt, uint32 cs, void* );

   void*          (api_func* GetImageOwner)( const_image_handle );

   api_bool       (api_func* GetImageRefCount)( const_image_handle, uint32* );

   api_bool       (api_func* IsValidImageHandle)( const_image_handle );

   api_bool       (api_func* AttachToImage)( image_handle, void* );
   api_bool       (api_func* DetachFromImage)( image_handle, void* );

   api_bool       (api_func* GetImageFormat)( const_image_handle, uint32* nbits, api_bool* flt );

   api_bool       (api_func* GetImageGeometry)( const_image_handle, uint32* w, uint32* h, uint32* n );
   api_bool       (api_func* SetImageGeometry)( image_handle, uint32 w, uint32 h, uint32 n );

   api_bool       (api_func* GetImageColorSpace)( const_image_handle, uint32* cs );
   api_bool       (api_func* SetImageColorSpace)( image_handle, uint32 cs );

   api_bool       (api_func* GetImageRGBWS)( const_image_handle, api_RGBWS* );
   api_bool       (api_func* SetImageRGBWS)( image_handle, const api_RGBWS* ); // ### must be set through ImageWindow

   api_bool       (api_func* GetImagePixelData)( image_handle, void*** );
   api_bool       (api_func* SetImagePixelData)( image_handle, void** );
};

/*
 * ViewContext
 *
 * ### TODO - ASAP: Implement history management (ala PJSR).
 */
struct api_context ViewContext
{
   view_handle    (api_func* GetViewById)( const char* fullId );

   void           (api_func* EnumerateViews)( pcl::view_enumeration_callback, void*, api_bool includeMainViews, api_bool includePreviews );

   api_bool       (api_func* IsPreview)( const_view_handle );
   api_bool       (api_func* IsVolatilePreview)( const_view_handle );
   api_bool       (api_func* IsStoredPreview)( const_view_handle );

   window_handle  (api_func* GetViewParentWindow)( const_view_handle );

   api_bool       (api_func* GetViewId)( const_view_handle, char*, size_type* );
   api_bool       (api_func* GetViewFullId)( const_view_handle, char*, size_type* );
   api_bool       (api_func* SetViewId)( view_handle, const char* );

   void           (api_func* GetViewLocks)( const_view_handle, api_bool*, api_bool* );
   void           (api_func* LockView)( view_handle, api_bool, api_bool, api_bool );
   void           (api_func* UnlockView)( view_handle, api_bool, api_bool, api_bool );

   api_bool       (api_func* IsViewDynamicTarget)( const_view_handle );
   void           (api_func* AddViewToDynamicTargets)( view_handle );
   void           (api_func* RemoveViewFromDynamicTargets)( view_handle );

   image_handle   (api_func* GetViewImage)( view_handle );
   api_bool       (api_func* IsViewColorImage)( const_view_handle );
   api_bool       (api_func* GetViewDimensions)( const_view_handle, int32*, int32* );

   api_bool       (api_func* GetViewScreenTransferFunctions)( const_view_handle, double* m, double* c0, double* c1, double* r0, double* r1 );
   api_bool       (api_func* SetViewScreenTransferFunctions)( view_handle, const double* m, const double* c0, const double* c1, const double* r0, const double* r1, api_bool );
   api_bool       (api_func* DestroyViewScreenTransferFunctions)( view_handle, api_bool );
   api_bool       (api_func* GetViewScreenTransferFunctionsEnabled)( view_handle );
   void           (api_func* SetViewScreenTransferFunctionsEnabled)( view_handle, api_bool, api_bool );

   api_bool       (api_func* GetViewPropertyValue)( api_handle hModule, const_view_handle, const char* id, api_property_value* );
   api_bool       (api_func* GetViewPropertyAttributes)( api_handle hModule, const_view_handle, const char* id, uint32* flags, uint64* type );
   api_bool       (api_func* SetViewPropertyValue)( api_handle hModule, view_handle, const char* id, const api_property_value*, uint32 flags, api_bool notify );
   api_bool       (api_func* SetViewPropertyAttributes)( api_handle hModule, view_handle, const char* id, uint32 flags, api_bool notify );
   api_bool       (api_func* GetViewPropertyExists)( api_handle hModule, const_view_handle, const char* id, uint64* type );
   api_bool       (api_func* DeleteViewProperty)( api_handle hModule, view_handle, const char* id, api_bool notify );
   api_bool       (api_func* ComputeViewProperty)( api_handle hModule, view_handle, const char* id, api_bool notify, api_property_value* );
};

/*
 * ImageWindowContext
 */
struct api_context ImageWindowContext
{
   window_handle  (api_func* CreateImageWindow)( int32 width, int32 height, int32 numberOfChannels,
                                                 int32 bitsPerSample, api_bool floatSample, api_bool color,
                                                 api_bool initialProcessing, const char* id );

   api_bool       (api_func* LoadImageWindows)( const char16_type* filePath, const char* id,
                                                api_bool asACopy, api_bool allowMessages,
                                                pcl::window_enumeration_callback, void* );

   api_bool       (api_func* CloseImageWindow)( window_handle );

   window_handle  (api_func* GetImageWindowById)( const char* );
   window_handle  (api_func* GetImageWindowByFilePath)( const char16_type* );

   window_handle  (api_func* GetActiveImageWindow)();

   void           (api_func* EnumerateImageWindows)( pcl::window_enumeration_callback, void*, api_bool includeIconic );

   void           (api_func* EnumeratePreviews)( const_window_handle, pcl::view_enumeration_callback, void* );

   api_bool       (api_func* GetImageWindowNewFlag)( const_window_handle );
   api_bool       (api_func* GetImageWindowCopyFlag)( const_window_handle );

   api_bool       (api_func* GetImageWindowFileURL)( const_window_handle, char16_type*, size_type* );

   api_bool       (api_func* GetImageWindowFilePath)( const_window_handle, char16_type*, size_type* );

   api_bool       (api_func* GetImageWindowFileInfo)( const_window_handle, api_image_file_info* );

   size_type      (api_func* GetImageWindowModifyCount)( const_window_handle );

   view_handle    (api_func* GetImageWindowMainView)( const_window_handle );

   view_handle    (api_func* GetImageWindowCurrentView)( const_window_handle );
   void           (api_func* SetImageWindowCurrentView)( window_handle, view_handle );

   void           (api_func* PurgeImageWindowProperties)( window_handle );

   api_bool       (api_func* ValidateImageWindowView)( const_window_handle, const_view_handle );

   int32          (api_func* GetPreviewCount)( const_window_handle );

   view_handle    (api_func* GetPreviewById)( const_window_handle, const char* );

   view_handle    (api_func* GetSelectedPreview)( const_window_handle );
   void           (api_func* SelectPreview)( window_handle, view_handle );

   view_handle    (api_func* CreatePreview)( window_handle, int32, int32, int32, int32, const char* );

   void           (api_func* ModifyPreview)( window_handle, const char*, int32, int32, int32, int32, const char* );

   void           (api_func* GetPreviewRect)( const_window_handle, const char*, int32*, int32*, int32*, int32* );

   void           (api_func* DeletePreview)( window_handle, const char* );
   void           (api_func* DeletePreviews)( window_handle );

   window_handle  (api_func* GetImageWindowMask)( const_window_handle, api_bool* inverted );
   void           (api_func* SetImageWindowMask)( window_handle, window_handle, api_bool inverted );

   api_bool       (api_func* GetImageWindowMaskEnabled)( const_window_handle );
   void           (api_func* SetImageWindowMaskEnabled)( window_handle, api_bool );

   api_bool       (api_func* GetImageWindowMaskVisible)( const_window_handle );
   void           (api_func* SetImageWindowMaskVisible)( window_handle, api_bool );

   api_bool       (api_func* ValidateImageWindowMask)( const_window_handle, const_window_handle );

   int32          (api_func* GetMaskReferenceCount)( const_window_handle );

   void           (api_func* RemoveImageWindowMaskReferences)( window_handle );
   void           (api_func* UpdateImageWindowMaskReferences)( window_handle );

   void           (api_func* GetImageWindowSampleFormat)( const_window_handle, uint32* nbits, api_bool* flt );
   void           (api_func* SetImageWindowSampleFormat)( window_handle, uint32 nbits, api_bool flt );

   void           (api_func* GetImageWindowRGBWS)( const_window_handle, api_RGBWS* );
   void           (api_func* SetImageWindowRGBWS)( window_handle, const api_RGBWS* );

   api_bool       (api_func* GetImageWindowGlobalRGBWS)( const_window_handle );
   void           (api_func* SetImageWindowGlobalRGBWS)( window_handle );

   void           (api_func* GetGlobalRGBWS)( api_RGBWS* );
   void           (api_func* SetGlobalRGBWS)( const api_RGBWS* );

   void           (api_func* GetImageWindowCMEnabled)( const_window_handle, api_bool* enableCM, api_bool* proofing, api_bool* gamutCheck );
   void           (api_func* SetImageWindowCMEnabled)( window_handle, api_bool enableCM, api_bool proofing, api_bool gamutCheck );

   uint32         (api_func* GetImageWindowICCProfileLength)( const_window_handle );
   void           (api_func* GetImageWindowICCProfile)( const_window_handle, void* );
   void           (api_func* SetImageWindowICCProfile)( window_handle, const void* );
   void           (api_func* LoadImageWindowICCProfile)( window_handle, const char16_type* );
   void           (api_func* DeleteImageWindowICCProfile)( window_handle );

   int32          (api_func* GetImageWindowKeywordCount)( const_window_handle );
   void           (api_func* GetImageWindowKeyword)( const_window_handle, int32, char*, size_type, char*, size_type, char*, size_type );
   void           (api_func* AddImageWindowKeyword)( window_handle, const char*, const char*, const char* );
   void           (api_func* ResetImageWindowKeywords)( window_handle );

   void           (api_func* GetImageWindowResolution)( const_window_handle, double*, double*, api_bool* );
   void           (api_func* SetImageWindowResolution)( window_handle, double, double, api_bool );

   void           (api_func* GetDefaultResolution)( double*, double*, api_bool* );
   void           (api_func* GetDefaultICCProfileEmbedding)( api_bool* rgb, api_bool* grayscale );
   api_bool       (api_func* GetDefaultThumbnailEmbedding)();
   api_bool       (api_func* GetDefaultPropertiesEmbedding)();

   api_bool       (api_func* GetSwapDirectory)( int32, char16_type*, size_type* );
   api_bool       (api_func* SetSwapDirectories)( const char16_type**, int32 );

   int32          (api_func* GetCursorTolerance)();

   int32          (api_func* GetImageWindowTransparencyMode)( const_window_handle, uint32* );
   void           (api_func* SetImageWindowTransparencyMode)( window_handle, int32, uint32 );

   int32          (api_func* GetTransparencyBackgroundBrush)( uint32* fgColor, uint32* bgColor );
   void           (api_func* SetTransparencyBackgroundBrush)( int32 brush, uint32 fgColor, uint32 bgColor );

   int32          (api_func* GetImageWindowMode)();
   void           (api_func* SetImageWindowMode)( int32 );

   int32          (api_func* GetImageWindowDisplayChannel)( const_window_handle );
   void           (api_func* SetImageWindowDisplayChannel)( window_handle, int32 );

   int32          (api_func* GetImageWindowMaskMode)( const_window_handle );
   void           (api_func* SetImageWindowMaskMode)( window_handle, int32 );

   void           (api_func* FitImageWindow)( window_handle );
   void           (api_func* ZoomImageWindowToFit)( window_handle, api_bool );

   int32          (api_func* GetImageWindowZoomFactor)( const_window_handle );
   void           (api_func* SetImageWindowZoomFactor)( window_handle, int32 );

   void           (api_func* UpdateImageWindowViewport)( window_handle );

   void           (api_func* RegenerateImageWindowViewport)( window_handle );

   void           (api_func* SetImageWindowViewport)( window_handle, double cx, double cy, int32 zoom );

   void           (api_func* GetImageWindowViewportSize)( const_window_handle, int32*, int32* );

   void           (api_func* GetImageWindowViewportOrigin)( const_window_handle, int32*, int32* );

   void           (api_func* GetImageWindowViewportPosition)( const_window_handle, int32*, int32* );
   void           (api_func* SetImageWindowViewportPosition)( window_handle, int32, int32 );

   void           (api_func* GetImageWindowVisibleViewportRect)( const_window_handle, int32*, int32*, int32*, int32* );

   api_bool       (api_func* GetImageWindowVisible)( const_window_handle );
   void           (api_func* SetImageWindowVisible)( window_handle, api_bool );

   api_bool       (api_func* GetImageWindowIconic)( const_window_handle );
   void           (api_func* SetImageWindowIconic)( window_handle, api_bool );

   void           (api_func* BringImageWindowToFront)( window_handle );
   void           (api_func* SendImageWindowToBack)( window_handle );

   interface_handle (api_func* GetActiveDynamicInterface)();
   api_bool       (api_func* TerminateDynamicSession)( api_bool closeInterface );

   void           (api_func* SetDynamicCursorXPM)( window_handle, const char**, int32 hx, int32 hy ); // ### deprecated
   void           (api_func* SetDynamicCursor)( window_handle, const_bitmap_handle, int32 hx, int32 hy );

   bitmap_handle  (api_func* GetDynamicCursorBitmap)( const_window_handle );
   void           (api_func* GetDynamicCursorHotSpot)( const_window_handle, int32* hx, int32* hy );

   void           (api_func* ViewportToImageArray)( const_window_handle, int32*, size_type n );
   void           (api_func* ViewportToImageArrayD)( const_window_handle, double*, size_type n );

   void           (api_func* ViewportToImage)( const_window_handle, int32* x, int32* y );
   void           (api_func* ViewportToImageD)( const_window_handle, double* x, double* y );

   void           (api_func* ImageToViewportArray)( const_window_handle, int32*, size_type n );
   void           (api_func* ImageToViewportArrayD)( const_window_handle, double*, size_type n );

   void           (api_func* ImageToViewport)( const_window_handle, int32* x, int32* y );
   void           (api_func* ImageToViewportD)( const_window_handle, double* x, double* y );

   void           (api_func* ViewportScalarToImageArray)( const_window_handle, int32*, size_type n );
   void           (api_func* ViewportScalarToImageArrayD)( const_window_handle, double*, size_type n );

   void           (api_func* ViewportScalarToImage)( const_window_handle, int32* );
   void           (api_func* ViewportScalarToImageD)( const_window_handle, double* );

   void           (api_func* ImageScalarToViewportArray)( const_window_handle, int32*, size_type n );
   void           (api_func* ImageScalarToViewportArrayD)( const_window_handle, double*, size_type n );

   void           (api_func* ImageScalarToViewport)( const_window_handle, int32* );
   void           (api_func* ImageScalarToViewportD)( const_window_handle, double* );

   void           (api_func* ViewportToGlobal)( const_window_handle, int32* x, int32* y );
   void           (api_func* GlobalToViewport)( const_window_handle, int32* x, int32* y );

   void           (api_func* UpdateViewportRect)( window_handle, int32, int32, int32, int32 );
   void           (api_func* UpdateImageRect)( window_handle, double, double, double, double );

   void           (api_func* RegenerateViewportRect)( window_handle, int32, int32, int32, int32 );
   void           (api_func* RegenerateImageRect)( window_handle, double, double, double, double );

   void           (api_func* CommitViewportUpdates)( window_handle );

   api_bool       (api_func* GetViewportUpdateRect)( const_window_handle, int32*, int32*, int32*, int32* );

   void           (api_func* BeginViewportSelection)( window_handle, int32 x, int32 y, uint32 flags );
   void           (api_func* ModifyViewportSelection)( window_handle, int32 x, int32 y, uint32 flags );
   void           (api_func* UpdateViewportSelection)( window_handle );
   void           (api_func* CancelViewportSelection)( window_handle );
   void           (api_func* EndViewportSelection)( window_handle );
   api_bool       (api_func* GetViewportSelection)( const_window_handle, int32* x0, int32* y0, int32* x1, int32* y1, uint32* flags );

   bitmap_handle  (api_func* GetViewportBitmap)( api_handle, const_window_handle, int32 x0, int32 y0, int32 x1, int32 y1, uint32 flags );

   api_bool       (api_func* GetImageWindowDisplayPixelRatio)( const_window_handle, double* );
   api_bool       (api_func* GetImageWindowResourcePixelRatio)( const_window_handle, double* );
   api_bool       (api_func* GetImageWindowDevicePixelRatio)( const_window_handle, double* );
};

/*
 * ImageViewContext
 */
struct api_context ImageViewContext
{
   control_handle (api_func* CreateImageView)( api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags,
                                               int32 width, int32 height, int32 numberOfChannels,
                                               int32 bitsPerSample, api_bool floatSample, api_bool color );

   control_handle (api_func* CreateImageViewViewport)( control_handle, api_handle hClient );

   image_handle   (api_func* GetImageViewImage)( const_control_handle );
   api_bool       (api_func* IsImageViewColorImage)( const_control_handle );
   api_bool       (api_func* GetImageViewImageGeometry)( const_control_handle hView, int32*, int32*, int32* );

   api_bool       (api_func* GetImageViewSampleFormat)( const_control_handle, int32* nbits, api_bool* flt );
   void           (api_func* SetImageViewSampleFormat)( control_handle, int32 nbits, api_bool flt );

   void           (api_func* GetImageViewRGBWS)( const_control_handle, api_RGBWS* );
   void           (api_func* SetImageViewRGBWS)( control_handle, const api_RGBWS* );

   void           (api_func* GetImageViewCMEnabled)( const_control_handle, api_bool* enableCM, api_bool* proofing, api_bool* gamutCheck );
   void           (api_func* SetImageViewCMEnabled)( control_handle, api_bool enableCM, api_bool proofing, api_bool gamutCheck );

   uint32         (api_func* GetImageViewICCProfileLength)( const_control_handle );
   void           (api_func* GetImageViewICCProfile)( const_control_handle, void* );
   void           (api_func* SetImageViewICCProfile)( control_handle, const void* );
   void           (api_func* LoadImageViewICCProfile)( control_handle, const char16_type* );
   void           (api_func* DeleteImageViewICCProfile)( control_handle );

   api_bool       (api_func* SetImageViewScrollEventRoutine)( control_handle, api_handle, pcl::range_event_routine );

   /*
    * ### TODO
    * Unify viewport routines for ImageWindow and ImageView, i.e. create a new ImageViewport control and context.
    */

   int32          (api_func* GetImageViewMode)( const_control_handle );
   void           (api_func* SetImageViewMode)( control_handle, int32 );

   int32          (api_func* GetImageViewDisplayChannel)( const_control_handle );
   void           (api_func* SetImageViewDisplayChannel)( control_handle, int32 );

   int32          (api_func* GetImageViewZoomFactor)( const_control_handle );
   void           (api_func* SetImageViewZoomFactor)( control_handle, int32 );

   int32          (api_func* GetImageViewTransparencyMode)( const_control_handle, uint32* );
   void           (api_func* SetImageViewTransparencyMode)( control_handle, int32, uint32 );

   void           (api_func* UpdateImageViewViewport)( control_handle );

   void           (api_func* RegenerateImageViewViewport)( control_handle );

   void           (api_func* SetImageViewViewport)( control_handle, double cx, double cy, int32 zoom );

   void           (api_func* GetImageViewViewportSize)( const_control_handle, int32*, int32* );

   void           (api_func* GetImageViewViewportOrigin)( const_control_handle, int32*, int32* );

   void           (api_func* GetImageViewViewportPosition)( const_control_handle, int32*, int32* );
   void           (api_func* SetImageViewViewportPosition)( control_handle, int32, int32 );

   void           (api_func* GetImageViewVisibleViewportRect)( const_control_handle, int32*, int32*, int32*, int32* );

   void           (api_func* ViewportToImageArray)( const_control_handle, int32*, size_type n );
   void           (api_func* ViewportToImageArrayD)( const_control_handle, double*, size_type n );

   void           (api_func* ViewportToImage)( const_control_handle, int32* x, int32* y );
   void           (api_func* ViewportToImageD)( const_control_handle, double* x, double* y );

   void           (api_func* ImageToViewportArray)( const_control_handle, int32*, size_type n );
   void           (api_func* ImageToViewportArrayD)( const_control_handle, double*, size_type n );

   void           (api_func* ImageToViewport)( const_control_handle, int32* x, int32* y );
   void           (api_func* ImageToViewportD)( const_control_handle, double* x, double* y );

   void           (api_func* ViewportScalarToImageArray)( const_control_handle, int32*, size_type n );
   void           (api_func* ViewportScalarToImageArrayD)( const_control_handle, double*, size_type n );

   void           (api_func* ViewportScalarToImage)( const_control_handle, int32* );
   void           (api_func* ViewportScalarToImageD)( const_control_handle, double* );

   void           (api_func* ImageScalarToViewportArray)( const_control_handle, int32*, size_type n );
   void           (api_func* ImageScalarToViewportArrayD)( const_control_handle, double*, size_type n );

   void           (api_func* ImageScalarToViewport)( const_control_handle, int32* );
   void           (api_func* ImageScalarToViewportD)( const_control_handle, double* );

   void           (api_func* ViewportToGlobal)( const_control_handle, int32* x, int32* y );
   void           (api_func* GlobalToViewport)( const_control_handle, int32* x, int32* y );

   void           (api_func* UpdateViewportRect)( control_handle, int32, int32, int32, int32 );
   void           (api_func* UpdateImageRect)( control_handle, double, double, double, double );

   void           (api_func* RegenerateViewportRect)( control_handle, int32, int32, int32, int32 );
   void           (api_func* RegenerateImageRect)( control_handle, double, double, double, double );

   void           (api_func* CommitViewportUpdates)( control_handle );

   api_bool       (api_func* GetViewportUpdateRect)( const_control_handle, int32*, int32*, int32*, int32* );

   void           (api_func* BeginViewportSelection)( control_handle, int32 x, int32 y, uint32 flags );
   void           (api_func* ModifyViewportSelection)( control_handle, int32 x, int32 y, uint32 flags );
   void           (api_func* UpdateViewportSelection)( control_handle );
   void           (api_func* CancelViewportSelection)( control_handle );
   void           (api_func* EndViewportSelection)( control_handle );
   api_bool       (api_func* GetViewportSelection)( const_control_handle, int32* x0, int32* y0, int32* x1, int32* y1, uint32* flags );

   bitmap_handle  (api_func* GetViewportBitmap)( api_handle, const_control_handle, int32 x0, int32 y0, int32 x1, int32 y1, uint32 flags );
};

/*
 * CodeEditorContext
 */
struct api_context CodeEditorContext
{
   control_handle (api_func* CreateCodeEditor)( api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags );

   control_handle (api_func* CreateEditorLineNumbersControl)( control_handle, api_handle hClient, control_handle hParent, uint32 flags );

   api_bool       (api_func* GetEditorFilePath)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetEditorFilePath)( control_handle, const char16_type* );

   api_bool       (api_func* GetEditorText)( const_control_handle, char16_type*, size_type* );
   void           (api_func* SetEditorText)( control_handle, const char16_type* );

   api_bool       (api_func* GetEditorEncodedText)( const_control_handle, char*, size_type*, const char* encoding );
   api_bool       (api_func* SetEditorEncodedText)( control_handle, const char*, const char* encoding );

   void           (api_func* ClearEditorText)( control_handle );

   api_bool       (api_func* GetEditorReadOnly)( const_control_handle );
   void           (api_func* SetEditorReadOnly)( control_handle, api_bool );

   api_bool       (api_func* SaveEditorText)( control_handle, const char16_type* filePath, const char* encoding );
   api_bool       (api_func* LoadEditorText)( control_handle, const char16_type* filePath, const char* encoding );

   int32          (api_func* GetEditorLineCount)( const_control_handle );
   int32          (api_func* GetEditorCharacterCount)( const_control_handle );

   void           (api_func* GetEditorCursorCoordinates)( const_control_handle, int32* line, int32* col );
   void           (api_func* SetEditorCursorCoordinates)( control_handle, int32 line, int32 col );

   api_bool       (api_func* GetEditorInsertMode)( const_control_handle );
   void           (api_func* SetEditorInsertMode)( control_handle, api_bool );

   api_bool       (api_func* GetEditorBlockSelectionMode)( const_control_handle );
   void           (api_func* SetEditorBlockSelectionMode)( control_handle, api_bool );

   api_bool       (api_func* GetEditorDynamicWordWrapMode)( const_control_handle );
   void           (api_func* SetEditorDynamicWordWrapMode)( control_handle, api_bool );

   int32          (api_func* GetEditorUndoSteps)( const_control_handle );
   int32          (api_func* GetEditorRedoSteps)( const_control_handle );

   api_bool       (api_func* GetEditorHasSelection)( const_control_handle );
   void           (api_func* GetEditorSelectionCoordinates)( const_control_handle, int32* fromLine, int32* fromCol, int32* toLine, int32* toCol );
   void           (api_func* SetEditorSelectionCoordinates)( control_handle, int32 fromLine, int32 fromCol, int32 toLine, int32 toCol );
   api_bool       (api_func* GetEditorSelectedText)( const_control_handle, char16_type*, size_type* );

   void           (api_func* InsertEditorText)( control_handle, const char16_type* );

   void           (api_func* EditorUndo)( control_handle );
   void           (api_func* EditorRedo)( control_handle );
   void           (api_func* EditorCut)( control_handle );
   void           (api_func* EditorCopy)( control_handle );
   void           (api_func* EditorPaste)( control_handle );
   void           (api_func* EditorDelete)( control_handle );
   void           (api_func* EditorSelectAll)( control_handle );
   void           (api_func* EditorUnselect)( control_handle );

   api_bool       (api_func* EditorGotoMatchedParenthesis)( control_handle );

   int32          (api_func* EditorHighlightAllMatches)( control_handle, const char16_type*, uint32 flags );
   void           (api_func* EditorClearMatches)( control_handle );

   api_bool       (api_func* EditorFind)( control_handle, const char16_type*, uint32 flags );

   api_bool       (api_func* EditorReplace)( control_handle, const char16_type* );

   int32          (api_func* EditorReplaceAll)( control_handle, const char16_type*, const char16_type*, uint32 flags );

   api_bool       (api_func* SetEditorTextUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (api_func* SetEditorCursorPositionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (api_func* SetEditorSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::rect_event_routine );
   api_bool       (api_func* SetEditorOverwriteModeUpdatedEventRoutine)( control_handle, api_handle, pcl::state_event_routine );
   api_bool       (api_func* SetEditorSelectionModeUpdatedEventRoutine)( control_handle, api_handle, pcl::state_event_routine );
   api_bool       (api_func* SetEditorDynamicWordWrapModeUpdatedEventRoutine)( control_handle, api_handle, pcl::state_event_routine );
};

/*
 * ExternalProcessContext
 */
struct api_context ExternalProcessContext
{
   enum IOStream
   {
      StandardInput,
      StandardOutput,
      StandardError,
      CombinedOutput
   };

   enum ExitStatus
   {
      NormalExit,
      CrashedExit,
      InvalidExit
   };

   enum ErrorCode
   {
      FailedToStart,
      Crashed,
      TimedOut,
      ReadError,
      WriteError,
      UnknownError,
      InvalidError
   };

   int32          (api_func* ExecuteProgram)( const char16_type* program, const char16_type** argv, size_type argc );

   api_bool       (api_func* StartProgram)( const char16_type* program, const char16_type** argv, size_type argc,
                                            const char16_type* workingDirectory, uint64* pid );

   external_process_handle (api_func* CreateExternalProcess)( api_handle hModule, api_handle hClient );

   api_bool       (api_func* StartExternalProcess)( external_process_handle,
                                                    const char16_type* program, const char16_type** argv, size_type argc );

   api_bool       (api_func* WaitForExternalProcessStarted)( external_process_handle, int32 ms );
   api_bool       (api_func* WaitForExternalProcessFinished)( external_process_handle, int32 ms );
   api_bool       (api_func* WaitForExternalProcessDataAvailable)( external_process_handle, int32 ms );
   api_bool       (api_func* WaitForExternalProcessDataWritten)( external_process_handle, int32 ms );

   api_bool       (api_func* TerminateExternalProcess)( external_process_handle );
   api_bool       (api_func* KillExternalProcess)( external_process_handle );

   api_bool       (api_func* CloseExternalProcessStream)( external_process_handle, int32 stream );

   api_bool       (api_func* RedirectExternalProcessToFile)( external_process_handle, int32 stream, const char16_type* fileName, api_bool append );
   api_bool       (api_func* PipeExternalProcess)( external_process_handle, int32 stream, external_process_handle toProcess );

   api_bool       (api_func* GetExternalProcessWorkingDirectory)( const_external_process_handle, char16_type*, size_type* );
   api_bool       (api_func* SetExternalProcessWorkingDirectory)( external_process_handle, const char16_type* );

   api_bool       (api_func* GetExternalProcessIsRunning)( const_external_process_handle );
   api_bool       (api_func* GetExternalProcessIsStarting)( const_external_process_handle );

   uint64         (api_func* GetExternalProcessPID)( const_external_process_handle );

   int32          (api_func* GetExternalProcessExitCode)( const_external_process_handle );
   int32          (api_func* GetExternalProcessExitStatus)( const_external_process_handle );
   int32          (api_func* GetExternalProcessErrorCode)( const_external_process_handle );

   size_type      (api_func* GetExternalProcessBytesAvailable)( const_external_process_handle );
   size_type      (api_func* GetExternalProcessBytesToWrite)( const_external_process_handle );

   // ### The following function returns data allocated by the caller module.
   api_bool       (api_func* ReadFromExternalProcess)( api_handle hModule, external_process_handle, int32 stream, void**, size_type* );
   api_bool       (api_func* WriteToExternalProcess)( external_process_handle, const void*, size_type count );

   api_bool       (api_func* EnumerateExternalProcessEnvironment)( const_external_process_handle, pcl::environment_enumeration_callback, void* );
   api_bool       (api_func* SetExternalProcessEnvironment)( external_process_handle, const char16_type** vars, size_type count );

   api_bool       (api_func* SetExternalProcessStartedEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (api_func* SetExternalProcessFinishedEventRoutine)( external_process_handle, api_handle, pcl::external_process_exit_status_event_routine );
   api_bool       (api_func* SetExternalProcessStandardOutputDataAvailableEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (api_func* SetExternalProcessStandardErrorDataAvailableEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (api_func* SetExternalProcessErrorEventRoutine)( external_process_handle, api_handle, pcl::external_process_status_event_routine );
};

/*
 * NetworkTransferContext
 *
 * ### TODO: Somewhere during the 1.8 cycle:
 *           Improve NetworkTransfer with asynchronous network access support.
 */
struct api_context NetworkTransferContext
{
   network_transfer_handle (api_func* CreateNetworkTransfer)( api_handle hModule, api_handle hClient );

   api_bool       (api_func* SetNetworkTransferURL)( network_transfer_handle, const char16_type* url, const char16_type* userName, const char16_type* userPassword );
   api_bool       (api_func* SetNetworkTransferProxyURL)( network_transfer_handle, const char16_type* proxy, const char16_type* userName, const char16_type* userPassword );
   api_bool       (api_func* SetNetworkTransferSSL)( network_transfer_handle, api_bool useSSL, api_bool forceSSL, api_bool verifyPeer, api_bool verifyHost );
   api_bool       (api_func* SetNetworkTransferConnectionTimeout)( network_transfer_handle, int32 seconds );

   api_bool       (api_func* PerformNetworkTransferDownload)( network_transfer_handle );
   api_bool       (api_func* PerformNetworkTransferUpload)( network_transfer_handle, fsize_type uploadSize );
   api_bool       (api_func* PerformNetworkTransferPOST)( network_transfer_handle, const char16_type* postFields );
   api_bool       (api_func* PerformNetworkTransferSMTP)( network_transfer_handle, const char16_type* mailFrom, const char16_type* mailRecipients );

   void           (api_func* CloseNetworkTransferConnection)( network_transfer_handle );

   api_bool       (api_func* GetNetworkTransferURL)( const_network_transfer_handle, char16_type*, size_type* );
   api_bool       (api_func* GetNetworkTransferProxyURL)( const_network_transfer_handle, char16_type*, size_type* );
   api_bool       (api_func* GetNetworkTransferStatus)( const_network_transfer_handle );
   api_bool       (api_func* GetNetworkTransferIsAborted)( const_network_transfer_handle );
   int32          (api_func* GetNetworkTransferResponseCode)( const_network_transfer_handle );
   api_bool       (api_func* GetNetworkTransferContentType)( const_network_transfer_handle, char16_type*, size_type* );
   fsize_type     (api_func* GetNetworkTransferBytesTransferred)( const_network_transfer_handle );
   void           (api_func* GetNetworkTransferTotalSpeed)( const_network_transfer_handle, double* ); // in KiB/s
   void           (api_func* GetNetworkTransferTotalTime)( const_network_transfer_handle, double* ); // in s
   api_bool       (api_func* GetNetworkTransferErrorInformation)( const_network_transfer_handle, char16_type*, size_type* );

   api_bool       (api_func* SetNetworkTransferDownloadEventRoutine)( network_transfer_handle, api_handle, pcl::network_download_event_routine );
   api_bool       (api_func* SetNetworkTransferUploadEventRoutine)( network_transfer_handle, api_handle, pcl::network_upload_event_routine );
   api_bool       (api_func* SetNetworkTransferProgressEventRoutine)( network_transfer_handle, api_handle, pcl::network_progress_event_routine );
};

// ----------------------------------------------------------------------------

}  // extern "C"

// End global namespace

// ----------------------------------------------------------------------------

namespace pcl
{

/*
 * APIInterface - Low-level C API interface
 */
struct APIInterface
{
   GlobalContext*                Global;
   ModuleDefinitionContext*      ModuleDefinition;
   ProcessDefinitionContext*     ProcessDefinition;
   InterfaceDefinitionContext*   InterfaceDefinition;
   FileFormatDefinitionContext*  FileFormatDefinition;
   ModuleContext*                Module;
   ProcessContext*               Process;
   InterfaceContext*             Interface;
   FileFormatContext*            FileFormat;
   UIContext*                    UI;
   ActionContext*                Action;
   ControlContext*               Control;
   DialogContext*                Dialog;
   FrameContext*                 Frame;
   GroupBoxContext*              GroupBox;
   TabBoxContext*                TabBox;
   ButtonContext*                Button;
   EditContext*                  Edit;
   TextBoxContext*               TextBox;
   ComboBoxContext*              ComboBox;
   SliderContext*                Slider;
   SpinBoxContext*               SpinBox;
   LabelContext*                 Label;
   BitmapBoxContext*             BitmapBox;
   ScrollBoxContext*             ScrollBox;
   TreeBoxContext*               TreeBox;
   TimerContext*                 Timer;
   ThreadContext*                Thread;
   MutexContext*                 Mutex;
   ViewListContext*              ViewList;
   BitmapContext*                Bitmap;
   SVGContext*                   SVG;
   BrushContext*                 Brush;
   PenContext*                   Pen;
   FontContext*                  Font;
   CursorContext*                Cursor;
   SizerContext*                 Sizer;
   GraphicsContext*              Graphics;
   ColorManagementContext*       ColorManagement;
   RealTimePreviewContext*       RealTimePreview;
   NumericalContext*             Numerical;
   CompressionContext*           Compression;
   CryptographyContext*          Cryptography;
   SharedImageContext*           SharedImage;
   ViewContext*                  View;
   ImageWindowContext*           ImageWindow;
   ImageViewContext*             ImageView;
   CodeEditorContext*            CodeEditor;
   ExternalProcessContext*       ExternalProcess;
   NetworkTransferContext*       NetworkTransfer;

   // Automatically generated APIInterface.cpp
   APIInterface( function_resolver );
   ~APIInterface();

private:

   bool Resolve( function_resolver );
};

/*
 * Global PCL API Data
 */

// implemented in API.cpp

extern PCL_DATA const pcl::APIInterface* API;

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

api_handle PCL_FUNC ModuleHandle();

#endif

} // pcl

// ----------------------------------------------------------------------------

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION

/*
 * Core API Function Resolver
 */
namespace pi
{
// Automatically generated APIFunctionResolver.cpp
extern "C" void* api_func APIFunctionResolver( const char* );
}

#endif

// ----------------------------------------------------------------------------

#endif   // __PCL_API_APIInterface_h

// ----------------------------------------------------------------------------
// EOF pcl/APIInterface.h - Released 2015/12/17 18:52:09 UTC
