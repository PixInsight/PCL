// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/APIDefs.h - Released 2014/10/29 07:34:13 UTC
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

#ifndef __PCL_API_APIDefs_h
#define __PCL_API_APIDefs_h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

using namespace pcl;

// Global namespace

/*
 * PixInsight API context structure
 */
#define api_context

/*
 * PixInsight API function
 */
#define api_func

/*
 * Low-level C API integer types
 */

// Integer type used to implement bool values in C API calls

typedef uint32    api_bool;

#define api_false 0u
#define api_true  1u

// Integer type used to implement enumeration parameter values

typedef int32     api_enum;

#define api_ok     1
#define api_abort  0
#define api_error -1

/*
 * API structures
 */

// Keyboard code
typedef uint32    api_key_code;

// Mouse buttons
typedef uint32    api_mouse_button;
typedef uint32    api_mouse_buttons;

// Keyboard modifiers
typedef uint32    api_key_modifiers;

// RGB working space
struct api_RGBWS
{
   float    gamma;          // gamma exponent if isSRGBGamma=false
   api_bool isSRGBGamma;    // true: sRGB gamma function
   float    x[ 3 ], y[ 3 ]; // chromaticity coordinates
   float    Y[ 3 ];         // luminance coefficients
};

// Process properties
struct api_process_props
{
   bool   isAssignable           :  1;
   bool   needsInitialization    :  1;
   bool   needsValidation        :  1;
   bool   canProcessViews        :  1;
   bool   canProcessGlobal       :  1;
   bool   canProcessImages       :  1;
   bool   canProcessCommandLines :  1;
   bool   canEditPreferences     :  1;
   bool   canBrowseDocumentation :  1;
   bool   prefersGlobalExecution :  1;
   int                           : 22;
};

// File format capabilities
struct api_format_caps
{
   bool   canRead                :  1;
   bool   canWrite               :  1;
   bool   canReadIncrementally   :  1;
   bool   canWriteIncrementally  :  1;
   bool   canStore8bit           :  1;
   bool   canStore16bit          :  1;
   bool   canStore32bit          :  1;
   bool   canStore64bit          :  1;
   bool   canStoreFloat          :  1;
   bool   canStoreDouble         :  1;
   bool   canStoreComplex        :  1;
   bool   canStoreDComplex       :  1;
   bool   canStoreGrayscale      :  1;
   bool   canStoreRGBColor       :  1;
   bool   canStoreAlphaChannels  :  1;
   bool   canStoreResolution     :  1;
   bool   canStoreKeywords       :  1;
   bool   canStoreICCProfiles    :  1;
   bool   canStoreMetaData       :  1;
   bool   canStoreThumbnails     :  1;
   bool   canStoreProperties     :  1;
   bool   supportsCompression    :  1;
   bool   supportsMultipleImages :  1;
   bool   canEditPreferences     :  1;
   bool   usesFormatSpecificData :  1;
   int                           :  7;
};

// Image information
struct api_image_info
{
   uint32 width;
   uint32 height;
   uint32 numberOfChannels;
   uint32 colorSpace;
   bool   supported              :  1;
   int                           : 31;
};

// Format-independent image options
struct api_image_options
{
   uint8  bitsPerSample          :  8;
   bool   ieeefpSampleFormat     :  1;
   bool   complexSample          :  1;
   bool   signedIntegers         :  1;
   bool   metricResolution       :  1;
   bool   readNormalized         :  1;
   bool   embedICCProfile        :  1;
   bool   embedMetadata          :  1;
   bool   embedThumbnail         :  1;
   bool   embedProperties        :  1;
   int                           : 15;
   double lowerRange, upperRange;
   double xResolution, yResolution;
   int32  isoSpeed;
   float  exposure;
   float  aperture;
   float  focalLength;
   int32  cfaType;
};

struct api_image_file_info
{
   uint8  bitsPerSample          :  8;
   bool   ieeefpSampleFormat     :  1;
   bool   complexSample          :  1;
   bool   signedIntegers         :  1;
   bool   metricResolution       :  1;
   bool   embedICCProfile        :  1;
   bool   embedMetadata          :  1;
   bool   embedThumbnail         :  1;
   bool   embedProperties        :  1;
   int                           : 16;
   double xResolution, yResolution;
   int32  isoSpeed;
   float  exposure;
   float  aperture;
   float  focalLength;
   int32  cfaType;
};

// Image statistics.
// ### PCL 2.0: Deprecate
struct api_image_statistics
{
   size_type count;
   double mean;
   double median;
   double stdDev;
   double avgDev;
   double variance;
   double minimum;
   int32  xMin, yMin;
   double maximum;
   int32  xMax, yMax;
};

// Module description
struct api_module_description
{
   const char*        uniqueId;      // [<unique-id>]<nul>
   const char*        versionInfo;   // MM.mm.rr.bbbb.LLL[.<status>]<nul>
   const char*        name;
   const char16_type* description;
   const char16_type* company;
   const char16_type* author;
   const char16_type* copyright;
   const char16_type* tradeMarks;
   const char16_type* originalFileName;

   struct
   {
      uint16 year;
      uint8  month;
      uint8  day;
   } releaseDate;

   uint32 apiVersion;
};

// Gradient stop
struct api_gradient_stop
{
   double position;
   uint32 color;
};

// Readout options
struct api_readout_options
{
   int32   data;
   int32   mode;
   int32   probeSize;
   int32   previewSize;
   int32   previewZoom;
   int32   precision;
   uint32  range;
   bool    showAlpha          :  1;
   bool    showMask           :  1;
   bool    showPreview        :  1;
   bool    previewCenter      :  1;
   bool    broadcast          :  1;
   bool    real               :  1;
   int                        : 26;
};

// PixelTraits LUT
struct api_pixtraits_lut
{
    float  pFLUT8  [ uint8_max  + 1 ];
    float  pFLUTA  [ uint8_max  + 1 ];
    float  p1FLUT8 [ uint8_max  + 1 ];
    float  pFLUT16 [ uint16_max + 1 ];
    float  pFLUT20 [ uint20_max + 1 ];
    double pDLUT8  [ uint8_max  + 1 ];
    double pDLUTA  [ uint8_max  + 1 ];
    double p1DLUT8 [ uint8_max  + 1 ];
    double pDLUT16 [ uint16_max + 1 ];
    double pDLUT20 [ uint20_max + 1 ];
    uint8  p8LUT16 [ uint16_max + 1 ];
    uint8  p8LUT20 [ uint20_max + 1 ];
    uint16 p16LUT8 [ uint8_max  + 1 ];
    uint16 p16LUT20[ uint20_max + 1 ];
    uint32 p20LUT8 [ uint8_max  + 1 ];
    uint32 p20LUT16[ uint16_max + 1 ];
    uint32 p24LUT8 [ uint8_max  + 1 ];
    uint32 p24LUT16[ uint16_max + 1 ];
    uint32 p32LUT8 [ uint8_max  + 1 ];
    uint32 p32LUT16[ uint16_max + 1 ];
    uint32 p32LUT20[ uint20_max + 1 ];
};

/*
 * MetaParameter Types
 */
#define PTYPE_TYPE_MASK             0x000000FF
#define PTYPE_UINT8                 0x00000001
#define PTYPE_INT8                  0x00000002
#define PTYPE_UINT16                0x00000003
#define PTYPE_INT16                 0x00000004
#define PTYPE_UINT32                0x00000005
#define PTYPE_INT32                 0x00000006
#define PTYPE_UINT64                0x00000007
#define PTYPE_INT64                 0x00000008
#define PTYPE_FLOAT                 0x00000009
#define PTYPE_DOUBLE                0x0000000A
#define PTYPE_BOOL                  0x0000000B
#define PTYPE_ENUM                  0x0000000C
#define PTYPE_STRING                0x0000000D
#define PTYPE_TABLE                 0x0000000E
#define PTYPE_BLOCK                 0x0000000F

/*
 * Block parameter interpretation
 */
#define PTYPE_BLOCK_INTERPRETATION  0x000F0000
#define PTYPE_BLOCK_UI8             0x00000000
#define PTYPE_BLOCK_I8              0x00010000
#define PTYPE_BLOCK_UI16            0x00020000
#define PTYPE_BLOCK_I16             0x00030000
#define PTYPE_BLOCK_UI32            0x00040000
#define PTYPE_BLOCK_I32             0x00050000
#define PTYPE_BLOCK_UI64            0x00060000
#define PTYPE_BLOCK_I64             0x00070000
#define PTYPE_BLOCK_FLOAT           0x00080000
#define PTYPE_BLOCK_DOUBLE          0x00090000

/*
 * File format instance open modes
 */
#define FMODE_READ                  0x00000001
#define FMODE_WRITE                 0x00000002
#define FMODE_READWRITE             0x00000003
#define FMODE_INCREMENTAL           0x00008000
#define FMODE_ANALYSIS              0x80000000

/*
 * View property types
 */
#define VTYPE_INVALID               0x0000000000000000
#define VTYPE_BOOLEAN               0x0000000000000001
#define VTYPE_INT8                  0x0000000000000010
#define VTYPE_INT16                 0x0000000000000020
#define VTYPE_INT32                 0x0000000000000040
#define VTYPE_INT64                 0x0000000000000080
#define VTYPE_UINT8                 0x0000000000000100
#define VTYPE_UINT16                0x0000000000000200
#define VTYPE_UINT32                0x0000000000000400
#define VTYPE_UINT64                0x0000000000000800
#define VTYPE_FLOAT                 0x0000000000004000
#define VTYPE_DOUBLE                0x0000000000008000
#define VTYPE_SIMPLE_MASK           0x000000000000FFFF

#define VTYPE_BYTEARRAY             0x0000000000010000
#define VTYPE_IVECTOR               0x0000000000040000
#define VTYPE_UIVECTOR              0x0000000000140000
#define VTYPE_INT64_VECTOR          0x0000000000080000
#define VTYPE_UINT64_VECTOR         0x0000000000180000
#define VTYPE_FVECTOR               0x0000000000240000
#define VTYPE_DVECTOR               0x0000000000280000
#define VTYPE_VECTOR_MASK           0x0000000000FF0000

#define VTYPE_BYTEMATRIX            0x0000000001000000
#define VTYPE_IMATRIX               0x0000000004000000
#define VTYPE_UIMATRIX              0x0000000014000000
#define VTYPE_INT64_MATRIX          0x0000000008000000
#define VTYPE_UINT64_MATRIX         0x0000000018000000
#define VTYPE_FMATRIX               0x0000000024000000
#define VTYPE_DMATRIX               0x0000000028000000
#define VTYPE_MATRIX_MASK           0x00000000FF000000

#define VTYPE_ISOSTRING             0x0000000100000000
#define VTYPE_STRING                0x0000000200000000
#define VTYPE_STRING_MASK           0x0000000F00000000

/*
 * View property attributes
 */
#define VATTR_WRITE_PROTECTED       0x00000001
#define VATTR_READ_PROTECTED        0x00000002
#define VATTR_VOLATILE              0x00000010
#define VATTR_PERMANENT             0x00000020
#define VATTR_NOT_SERIALIZABLE      0x00000040
#define VATTR_STORABLE              0x00000080
#define VATTR_RESERVED              0x10000000
#define VATTR_NO_CHANGE             0x80000000

/*
 * View property value
 */
struct api_property_value
{
   union
   {
      api_bool    boolValue;
      int8        int8Value;
      int16       int16Value;
      int32       int32Value;
      int64       int64Value;
      uint8       uint8Value;
      uint16      uint16Value;
      uint32      uint32Value;
      uint64      uint64Value;
      float       floatValue;
      double      doubleValue;
      const void* blockValue;
   }
   data;                // property value or starting address of data block

   size_type   dimX;    // first dimension
   size_type   dimY;    // second dimension
   size_type   dimZ;    // third dimension
   size_type   dimT;    // fourth dimension
   uint64      type;    // property type (VTYPE_xxx)
};

/*
 * Real-Time Preview LUT color spaces - ### Deprecated -> REMOVEME in 2.0.x
 */
#define RTP_RGBK                    0x00000001
#define RTP_CIEXYZ                  0x00000002
#define RTP_CIELAB                  0x00000004
#define RTP_CIELCH                  0x00000008
#define RTP_HSV                     0x00000010
#define RTP_HSI                     0x00000020

/*
 * API formal handle types
 */

typedef void*              api_handle;
typedef const void*        const_api_handle;

// Handle to a metaprocess

typedef const_api_handle   meta_process_handle;

// Handle to a metaparameter

typedef const_api_handle   meta_parameter_handle;

// Handle to a process instance

typedef api_handle         process_handle;
typedef const_api_handle   const_process_handle;

// Handle to a metainterface

typedef const_api_handle   meta_interface_handle;

// Handle to an interface instance

typedef api_handle         interface_handle;
typedef const_api_handle   const_interface_handle;

// Handle to a metaformat

typedef const_api_handle   meta_format_handle;

// Handle to a file format instance

typedef api_handle         file_format_handle;
typedef const_api_handle   const_file_format_handle;

// Handle to a PixInsight image object

typedef api_handle         image_handle;
typedef const_api_handle   const_image_handle;

// Handle to a PixInsight view object

typedef api_handle         view_handle;
typedef const_api_handle   const_view_handle;

// Handle to a PixInsight image window

typedef api_handle         window_handle;
typedef const_api_handle   const_window_handle;

// Handle to an ICC profile

typedef api_handle         icc_profile_handle;
typedef const_api_handle   const_icc_profile_handle;

// Handle to a color transformation

typedef api_handle         color_transform_handle;
typedef const_api_handle   const_color_transform_handle;

// Handle to a console object

typedef api_handle         console_handle;
typedef const_api_handle   const_console_handle;

// Handle to a timer object

typedef api_handle         timer_handle;
typedef const_api_handle   const_timer_handle;

// Handle to a thread object

typedef api_handle         thread_handle;
typedef const_api_handle   const_thread_handle;

// Handle to a mutex object

typedef api_handle         mutex_handle;
typedef const_api_handle   const_mutex_handle;

// Handle to a control

typedef api_handle         control_handle;
typedef const_api_handle   const_control_handle;

// Handle to a bitmap

typedef api_handle         bitmap_handle;
typedef const_api_handle   const_bitmap_handle;

// Handle to a SVG object

typedef api_handle         svg_handle;
typedef const_api_handle   const_svg_handle;

// Handle to a brush

typedef api_handle         brush_handle;
typedef const_api_handle   const_brush_handle;

// Handle to a pen

typedef api_handle         pen_handle;
typedef const_api_handle   const_pen_handle;

// Handle to a font

typedef api_handle         font_handle;
typedef const_api_handle   const_font_handle;

// Handle to a cursor

typedef api_handle         cursor_handle;
typedef const_api_handle   const_cursor_handle;

// Handle to a sizer

typedef api_handle         sizer_handle;
typedef const_api_handle   const_sizer_handle;

// Handle to a graphics context

typedef api_handle         graphics_handle;
typedef const_api_handle   const_graphics_handle;

// Handle to a dynamic action

typedef api_handle         action_handle;
typedef const_api_handle   const_action_handle;

// Handle to a fast Fourier transform

typedef api_handle         fft_handle;
typedef const_api_handle   const_fft_handle;

// Handle to a cryptographic hash / encryption

typedef api_handle         crypto_handle;
typedef const_api_handle   const_crypto_handle;

// Handle to an external process

typedef api_handle         external_process_handle;
typedef const_api_handle   const_external_process_handle;

// Handle to a network transfer

typedef api_handle         network_transfer_handle;
typedef const_api_handle   const_network_transfer_handle;

// End global namespace

// ----------------------------------------------------------------------------

namespace pcl
{

/*
 * Prototypes of low-level C API callback routines
 */

// API Function Provider

typedef void*                 (api_func* function_resolver)                           ( const char* );

// Global Context

typedef int32                 (api_func* waitable_routine)                            ( void* );

// Enumeration Callbacks

typedef api_bool              (api_func* category_enumeration_callback)               ( const char*, void* );
typedef api_bool              (api_func* process_enumeration_callback)                ( meta_process_handle, void* );
typedef api_bool              (api_func* property_enumeration_callback)               ( const char*, uint64, void* );
typedef api_bool              (api_func* icon_enumeration_callback)                   ( const char*, void* );
typedef api_bool              (api_func* parameter_enumeration_callback)              ( meta_parameter_handle, void* );
typedef api_bool              (api_func* interface_enumeration_callback)              ( interface_handle, void* );
typedef api_bool              (api_func* format_enumeration_callback)                 ( meta_format_handle, void* );
typedef api_bool              (api_func* file_enumeration_callback)                   ( const char16_type*, void* );
typedef api_bool              (api_func* window_enumeration_callback)                 ( window_handle, void* );
typedef api_bool              (api_func* view_enumeration_callback)                   ( view_handle, void* );
typedef api_bool              (api_func* font_enumeration_callback)                   ( const char16_type*, void* );
typedef api_bool              (api_func* font_size_enumeration_callback)              ( const double*, void* );
typedef api_bool              (api_func* environment_enumeration_callback)            ( const char16_type*, void* );

// Module Definition Context

typedef uint32                (api_func* module_identification_routine)               ( const void**, int32 );
typedef uint32                (api_func* module_initialization_routine)               ( api_handle, function_resolver, uint32, void* );
typedef int32                 (api_func* module_installation_routine)                 ( int32 );
typedef void                  (api_func* module_on_load_routine)                      ();
typedef void                  (api_func* module_on_unload_routine)                    ();
typedef void*                 (api_func* module_allocation_routine)                   ( size_type );
typedef void                  (api_func* module_deallocation_routine)                 ( void* );

// Process Definition Context

typedef void                  (api_func* process_class_initialization_routine)        ( meta_process_handle );
typedef process_handle        (api_func* process_creation_routine)                    ( meta_process_handle );
typedef void                  (api_func* process_destruction_routine)                 ( process_handle );
typedef process_handle        (api_func* process_clonation_routine)                   ( const_process_handle );
typedef process_handle        (api_func* process_test_clonation_routine)              ( const_process_handle );
typedef api_bool              (api_func* process_assignment_routine)                  ( process_handle, const_process_handle );
typedef api_bool              (api_func* process_initialization_routine)              ( process_handle );
typedef api_bool              (api_func* process_validation_routine)                  ( process_handle, char16_type*, uint32 );
typedef int32                 (api_func* process_command_line_processing_routine)     ( meta_process_handle, int32, const char16_type** );
typedef api_bool              (api_func* process_edit_preferences_routine)            ( meta_process_handle );
typedef api_bool              (api_func* process_browse_documentation_routine)        ( meta_process_handle );
typedef uint32                (api_func* process_execution_preferences_routine)       ( meta_process_handle );
typedef api_bool              (api_func* process_execution_validation_routine)        ( const_view_handle, const_process_handle, char16_type*, uint32 );
typedef api_bool              (api_func* process_mask_validation_routine)             ( const_view_handle, const_window_handle, const_process_handle );
typedef api_bool              (api_func* process_history_update_validation_routine)   ( const_view_handle, const_process_handle );
typedef uint32                (api_func* process_undo_mode_routine)                   ( const_view_handle, const_process_handle );
typedef api_bool              (api_func* process_pre_execution_routine)               ( view_handle, process_handle );
typedef api_bool              (api_func* process_execution_routine)                   ( view_handle, process_handle );
typedef void                  (api_func* process_post_execution_routine)              ( view_handle, process_handle );
typedef api_bool              (api_func* process_global_execution_validation_routine) ( const_process_handle, char16_type*, uint32 );
typedef api_bool              (api_func* process_pre_global_execution_routine)        ( process_handle );
typedef api_bool              (api_func* process_global_execution_routine)            ( process_handle );
typedef void                  (api_func* process_post_global_execution_routine)       ( process_handle );
typedef api_bool              (api_func* process_image_execution_validation_routine)  ( const_image_handle, const_process_handle, char16_type*, uint32 );
typedef api_bool              (api_func* process_image_execution_routine)             ( image_handle, process_handle, const char*, uint32 );
typedef meta_interface_handle (api_func* process_default_interface_selection_routine) ( meta_process_handle );
typedef meta_interface_handle (api_func* process_interface_selection_routine)         ( const_process_handle );
typedef api_bool              (api_func* process_interface_validation_routine)        ( const_process_handle, meta_interface_handle );
typedef api_bool              (api_func* process_pre_reading_routine)                 ( process_handle );
typedef void                  (api_func* process_post_reading_routine)                ( process_handle );
typedef api_bool              (api_func* process_pre_writing_routine)                 ( const_process_handle );
typedef void                  (api_func* process_post_writing_routine)                ( const_process_handle );

typedef void*                 (api_func* parameter_lock_routine)                      ( process_handle, meta_parameter_handle, size_type );
typedef void                  (api_func* parameter_unlock_routine)                    ( process_handle, meta_parameter_handle, size_type );
typedef api_bool              (api_func* parameter_validation_routine)                ( void*, const_process_handle, meta_parameter_handle, size_type );
typedef api_bool              (api_func* parameter_allocation_routine)                ( size_type, process_handle, meta_parameter_handle, size_type );
typedef size_type             (api_func* parameter_length_query_routine)              ( const_process_handle, meta_parameter_handle, size_type );

// Interface Definition Context

typedef void                  (api_func* interface_control_routine)                   ( interface_handle );
typedef void                  (api_func* interface_control_state_routine)             ( interface_handle, api_bool );
typedef void                  (api_func* interface_initialization_routine)            ( interface_handle, control_handle );
typedef api_bool              (api_func* interface_launch_routine)                    ( interface_handle, meta_process_handle, const_process_handle, api_bool*, uint32* );
typedef process_handle        (api_func* interface_process_instantiation_routine)     ( const_interface_handle, meta_process_handle* );
typedef api_bool              (api_func* interface_process_validation_routine)        ( const_interface_handle, const_process_handle, char16_type*, uint32 );
typedef api_bool              (api_func* interface_process_import_routine)            ( interface_handle, const_process_handle );
typedef api_bool              (api_func* interface_dynamic_mode_enter_routine)        ( interface_handle );
typedef void                  (api_func* interface_dynamic_mode_exit_routine)         ( interface_handle );
typedef api_bool              (api_func* interface_dynamic_view_event_routine)        ( interface_handle, view_handle );
typedef api_bool              (api_func* interface_dynamic_mouse_routine)             ( interface_handle, view_handle, double, double, api_mouse_buttons, api_key_modifiers );
typedef api_bool              (api_func* interface_dynamic_mouse_button_routine)      ( interface_handle, view_handle, double, double, api_mouse_button, api_mouse_buttons, api_key_modifiers );
typedef api_bool              (api_func* interface_dynamic_keyboard_routine)          ( interface_handle, view_handle, api_key_code, api_key_modifiers );
typedef api_bool              (api_func* interface_dynamic_wheel_routine)             ( interface_handle, view_handle, double, double, int32, api_mouse_buttons, api_key_modifiers );
typedef api_bool              (api_func* interface_dynamic_update_query_routine)      ( const_interface_handle, const_view_handle, double, double, double, double );
typedef void                  (api_func* interface_dynamic_paint_routine)             ( const_interface_handle, const_view_handle, graphics_handle, double, double, double, double );
typedef api_bool              (api_func* interface_real_time_update_query_routine)    ( const_interface_handle, const_image_handle, const_view_handle, int32 );
typedef api_bool              (api_func* interface_real_time_generation_routine)      ( const_interface_handle, image_handle, const_view_handle, int32, char16_type*, uint32 );
typedef void                  (api_func* interface_real_time_cancel_routine)          ( interface_handle );

// Interface Notifications

typedef void                  (api_func* view_property_notification_routine)          ( interface_handle, const_view_handle, const char* );
typedef void                  (api_func* image_notification_routine)                  ( interface_handle, const_view_handle );
typedef void                  (api_func* readout_notification_routine)                ( interface_handle, const_view_handle, double, double, double, double, double, double );
typedef void                  (api_func* process_notification_routine)                ( interface_handle, const_process_handle );
typedef void                  (api_func* window_notification_routine)                 ( interface_handle, const_window_handle );
typedef void                  (api_func* interface_notification_routine)              ( interface_handle, interface_handle );
typedef void                  (api_func* lut_notification_routine)                    ( interface_handle, int32 );
typedef void                  (api_func* global_notification_routine)                 ( interface_handle );

// File Format Definition Context

typedef file_format_handle    (api_func* format_creation_routine)                     ( meta_format_handle );
typedef void                  (api_func* format_destruction_routine)                  ( file_format_handle );
typedef api_bool              (api_func* format_validate_format_specific_data_routine)( meta_format_handle, const void* );
typedef void                  (api_func* format_dispose_format_specific_data_routine) ( meta_format_handle, const void* );
typedef api_bool              (api_func* format_edit_preferences_routine)             ( meta_format_handle );
typedef api_bool              (api_func* format_open_routine)                         ( file_format_handle, const char16_type*, const char*, uint32 );
typedef uint32                (api_func* format_get_image_count_routine)              ( const_file_format_handle );
typedef api_bool              (api_func* format_get_image_id_routine)                 ( const_file_format_handle, char*, uint32, uint32 );
typedef api_bool              (api_func* format_get_image_description_routine)        ( const_file_format_handle, api_image_info*, api_image_options*, uint32 );
typedef api_bool              (api_func* format_close_routine)                        ( file_format_handle );
typedef api_bool              (api_func* format_is_open_routine)                      ( const_file_format_handle );
typedef const char16_type*    (api_func* format_get_file_path_routine)                ( const_file_format_handle );
typedef uint32                (api_func* format_get_selected_image_index_routine)     ( const_file_format_handle );
typedef api_bool              (api_func* format_set_selected_image_index_routine)     ( file_format_handle, uint32 );
typedef api_bool              (api_func* format_set_format_specific_data_routine)     ( file_format_handle, const void* );
typedef void*                 (api_func* format_get_format_specific_data_routine)     ( const_file_format_handle );
typedef const char16_type*    (api_func* format_get_image_properties_routine)         ( const_file_format_handle );
typedef api_bool              (api_func* format_begin_extraction_routine)             ( file_format_handle );
typedef void                  (api_func* format_end_extraction_routine)               ( file_format_handle );
typedef size_type             (api_func* format_get_keyword_count_routine)            ( file_format_handle );
typedef api_bool              (api_func* format_get_next_keyword_routine)             ( file_format_handle, char*, char*, char*, uint32 );
typedef const void*           (api_func* format_get_icc_profile_routine)              ( file_format_handle );
typedef const void*           (api_func* format_get_metadata_routine)                 ( file_format_handle, uint32* );
typedef api_bool              (api_func* format_get_thumbnail_routine)                ( file_format_handle, image_handle );
typedef api_bool              (api_func* format_enumerate_image_properties_routine)   ( file_format_handle, property_enumeration_callback, char*, size_type*, void* );
typedef api_bool              (api_func* format_get_image_property_routine)           ( file_format_handle, const char*, api_property_value* );
typedef api_bool              (api_func* format_set_image_property_routine)           ( file_format_handle, const char*, const api_property_value* );
typedef api_bool              (api_func* format_read_image_routine)                   ( file_format_handle, image_handle );
typedef api_bool              (api_func* format_read_pixels_routine)                  ( file_format_handle, void*, uint32, uint32, uint32, uint32, api_bool, api_bool );
typedef api_bool              (api_func* format_query_options_routine)                ( file_format_handle, api_image_options*, const void**, uint32 );
typedef api_bool              (api_func* format_create_routine)                       ( file_format_handle, const char16_type*, int32, const char*, uint32 );
typedef api_bool              (api_func* format_set_image_id_routine)                 ( file_format_handle, const char* );
typedef api_bool              (api_func* format_set_image_options_routine)            ( file_format_handle, const api_image_options* );
typedef api_bool              (api_func* format_create_image_routine)                 ( file_format_handle, const api_image_info* );
typedef api_bool              (api_func* format_begin_embedding_routine)              ( file_format_handle );
typedef void                  (api_func* format_end_embedding_routine)                ( file_format_handle );
typedef api_bool              (api_func* format_add_keyword_routine)                  ( file_format_handle, const char*, const char*, const char* );
typedef api_bool              (api_func* format_set_icc_profile_routine)              ( file_format_handle, const void* );
typedef api_bool              (api_func* format_set_metadata_routine)                 ( file_format_handle, const void*, uint32 );
typedef api_bool              (api_func* format_set_thumbnail_routine)                ( file_format_handle, const_image_handle );
typedef api_bool              (api_func* format_write_image_routine)                  ( file_format_handle, const_image_handle );
typedef api_bool              (api_func* format_write_pixels_routine)                 ( file_format_handle, const void*, uint32, uint32, uint32, uint32, api_bool, api_bool );
typedef api_bool              (api_func* format_query_inexact_read_routine)           ( const_file_format_handle );
typedef api_bool              (api_func* format_query_lossy_write_routine)            ( const_file_format_handle );

// Action

typedef void                  (api_func* action_execution_routine)                    ( action_handle );
typedef api_bool              (api_func* action_state_query_routine)                  ( const_action_handle, const void* guiInfo );

// Interface Events

typedef api_bool              (api_func* control_event_routine)                       ( control_handle, control_handle );
typedef api_bool              (api_func* child_event_routine)                         ( control_handle, control_handle, control_handle );
typedef api_bool              (api_func* move_event_routine)                          ( control_handle, control_handle, int32, int32, int32, int32 );
typedef api_bool              (api_func* resize_event_routine)                        ( control_handle, control_handle, int32, int32, int32, int32 );
typedef api_bool              (api_func* paint_event_routine)                         ( control_handle, control_handle, int32, int32, int32, int32 );
typedef api_bool              (api_func* keyboard_event_routine)                      ( control_handle, control_handle, api_key_code, api_key_modifiers );
typedef api_bool              (api_func* mouse_event_routine)                         ( control_handle, control_handle, int32, int32, api_mouse_buttons, api_key_modifiers );
typedef api_bool              (api_func* mouse_button_event_routine)                  ( control_handle, control_handle, int32, int32, api_mouse_button, api_mouse_buttons, api_key_modifiers );
typedef api_bool              (api_func* wheel_event_routine)                         ( control_handle, control_handle, int32, int32, int32, api_mouse_buttons, api_key_modifiers );

typedef void                  (api_func* destroy_event_routine)                       ( control_handle );

typedef void                  (api_func* button_click_event_routine)                  ( control_handle, control_handle, api_bool );
typedef void                  (api_func* button_check_event_routine)                  ( control_handle, control_handle, int32 );

typedef void                  (api_func* ascii_event_routine)                         ( control_handle, control_handle, const char* );
typedef void                  (api_func* unicode_event_routine)                       ( control_handle, control_handle, const char16_type* );

typedef void                  (api_func* event_routine)                               ( control_handle, control_handle );
typedef void                  (api_func* value_event_routine)                         ( control_handle, control_handle, int32 );
typedef void                  (api_func* range_event_routine)                         ( control_handle, control_handle, int32, int32 );
typedef void                  (api_func* rect_event_routine)                          ( control_handle, control_handle, int32, int32, int32, int32 );
typedef void                  (api_func* state_event_routine)                         ( control_handle, control_handle, api_bool );
typedef void                  (api_func* item_event_routine)                          ( control_handle, control_handle, api_handle );
typedef void                  (api_func* item_value_event_routine)                    ( control_handle, control_handle, api_handle, int32 );
typedef void                  (api_func* item_range_event_routine)                    ( control_handle, control_handle, api_handle, api_handle );
typedef void                  (api_func* view_event_routine)                          ( control_handle, control_handle, view_handle );

typedef void                  (api_func* external_process_event_routine)              ( external_process_handle, control_handle );
typedef void                  (api_func* external_process_status_event_routine)       ( external_process_handle, control_handle, int32 );
typedef void                  (api_func* external_process_exit_status_event_routine)  ( external_process_handle, control_handle, int32, int32 );

typedef api_bool              (api_func* network_download_event_routine)              ( network_transfer_handle, control_handle, const void* buffer, fsize_type size );
typedef fsize_type            (api_func* network_upload_event_routine)                ( network_transfer_handle, control_handle, void* buffer, fsize_type maxSize );
typedef api_bool              (api_func* network_progress_event_routine)              ( network_transfer_handle, control_handle, fsize_type dlTotal, fsize_type dlCurrent, fsize_type ulTotal, fsize_type ulCurrent );

typedef void                  (api_func* timer_event_routine)                         ( timer_handle, control_handle );

typedef void                  (api_func* thread_exec_routine)                         ( thread_handle );

/*
 * Instance launch modes
 */
namespace InstanceLaunchMode
{
   enum value_type
   {
      Default,       // Use a default instance activation procedure
      Interface,     // Export to a selected interface
      Global,        // Execute in the global context
      CurrentView,   // Execute in the current view context
      CurrentWindow  // Execute in the current window context
   };
}

/*
 * Process enumeration helper class
 */
class InternalProcessEnumerator
{
public:

   static api_bool ProcessCallback( meta_process_handle, void* );
   static api_bool CategoryCallback( meta_process_handle, void* );
};

/*
 * Parameter enumeration helper class
 */
class InternalParameterEnumerator
{
public:

   static api_bool ParameterCallback( meta_parameter_handle, void* );
};

/*
 * Icon enumeration helper class
 */
class InternalIconEnumerator
{
public:

   static api_bool IconCallback( const char*, void* );
   static api_bool ProcessCallback( const char*, void* );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_API_APIDefs_h

// ****************************************************************************
// EOF pcl/APIDefs.h - Released 2014/10/29 07:34:13 UTC
