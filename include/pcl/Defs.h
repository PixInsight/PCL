//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/Defs.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_Defs_h
#define __PCL_Defs_h

/// \file pcl/Defs.h

/*!
 * \mainpage notitle
 *
 * <div style="margin-top: 3em; text-align: center;">
 *    <h1>
 *       PCL - PixInsight Class Library Version 2.1
 *    </h1>
 *    <h1>
 *       Reference Documentation
 *    </h1>
 *    <p>
 *       <strong>Copyright (c) 2003-2017, The PixInsight Development Team.
 *       All Rights Reserved</strong>
 *    </p>
 * </div>
 * <div style="margin-top: 3em; margin-bottom: 3em;">
 *    <p>
 *       <a href="http://pixinsight.com/">PixInsight</a> is an advanced image
 *       processing software platform designed specifically for
 *       astrophotography and other technical imaging fields. PixInsight is a
 *       modular, open-architecture system where the entire processing and file
 *       handling capabilities are implemented as external installable modules.
 *    </p>
 *    <p>
 *       The PixInsight core application provides the infrastructure on top of
 *       which external modules can implement processes, image file formats,
 *       and their associated user interfaces. The
 *       <a href="http://pixinsight.com/developer/pcl/">PixInsight Class
 *       Library</a> (PCL) is a C++ development framework to build PixInsight
 *       modules.
 *    </p>
 *    <p>
 *       PixInsight modules are special shared libraries (.so files on FreeBSD
 *       and Linux; .dylib under macOS; .dll files on Windows) that communicate
 *       with the PixInsight core application through a high-level API provided
 *       by PCL. Along with a core communication API, PCL includes a
 *       comprehensive set of image processing algorithms, ranging from
 *       geometrical transformations to multiscale analysis algorithms, most of
 *       them available as multithreaded parallel implementations.
 *    </p>
 *    <p>
 *       PCL is highly portable code. As of writing this document, it is
 *       available on the same platforms supported by the PixInsight core
 *       application: 64-bit FreeBSD, Linux, Mac OS X, and Windows. PixInsight
 *       modules written around PCL are directly portable to all supported
 *       platforms <em>without changing a single line of source code</em>. This
 *       is possible because PCL is a high-level framework. PCL isolates your
 *       module from platform-specific implementation details: all
 *       platform-dependent complexities are handled behind the scenes by the
 *       PixInsight core application and internal PCL routines.
 *    </p>
 *    <p>
 *       Starting from version 2.0, which was published in December of 2012,
 *       PCL is an open-source library released under the
 *       <a href="http://pixinsight.com/license/PCL_PJSR_1.0.html">PixInsight
 *       Class Library License</a> (PCLL). In essence, PCLL is a liberal
 *       BSD-like license that allows you to develop open-source and
 *       closed-source, free and commercial PixInsight modules without
 *       restrictions. As long as you observe all PCLL terms, you can modify
 *       PCL and use your modified version with or without releasing your
 *       source code.
 *    </p>
 *    <p>
 *       The entire source code of PCL, along with all PixInsight modules
 *       released as open-source products, is available at the
 *       <a href="https://github.com/PixInsight/">official PixInsight
 *       repositories on GitHub</a>. Open-source modules are a good starting
 *       point for developing new PixInsight modules. PixInsight and PCL are
 *       supported by a vibrant community on
 *       <a href="http://forum.pixinsight.com">PixInsight Forum</a>, which also
 *       contains a developers section. Join us there!
 *    </p>
 * </div>
 */

/*!
 * \namespace pcl
 * \brief PCL root namespace
 *
 * pcl is the main namespace of the PixInsight Class Library (PCL), where all
 * public PCL classes and functions are defined.
 */

// ----------------------------------------------------------------------------
// General definitions used by all PixInsight modules and programs.
// Included directly by PCL API and PixInsight source modules.
// ----------------------------------------------------------------------------

/*
 * One and only one platform selection macro has to be defined.
 */
#ifndef __PCL_FREEBSD
#  ifndef __PCL_LINUX
#    ifndef __PCL_MACOS
#      ifndef __PCL_MACOSX
#        ifndef __PCL_WINDOWS
#          error No platform selection macro has been defined.
#        endif
#      endif
#    endif
#  endif
#endif

/*
 * __PCL_MACOS and __PCL_MACOSX are synonyms and both always defined on macOS.
 */
#ifdef __PCL_MACOS
#  ifndef __PCL_MACOSX
#    define __PCL_MACOSX
#  endif
#endif
#ifdef __PCL_MACOSX
#  ifndef __PCL_MACOS
#    define __PCL_MACOS
#  endif
#endif

/*
 * Check for conflicting platform selection macros.
 */
#ifdef __PCL_FREEBSD
#  if defined( __PCL_LINUX ) || defined( __PCL_MACOSX ) || defined( __PCL_WINDOWS )
#    error Multiple platform selection macros have been defined.
#  endif
#endif
#ifdef __PCL_LINUX
#  if defined( __PCL_FREEBSD ) || defined( __PCL_MACOSX ) || defined( __PCL_WINDOWS )
#    error Multiple platform selection macros have been defined.
#  endif
#endif
#ifdef __PCL_MACOSX
#  if defined( __PCL_FREEBSD ) || defined( __PCL_LINUX ) || defined( __PCL_WINDOWS )
#    error Multiple platform selection macros have been defined.
#  endif
#endif
#ifdef __PCL_WINDOWS
#  if defined( __PCL_FREEBSD ) || defined( __PCL_LINUX ) || defined( __PCL_MACOSX )
#    error Multiple platform selection macros have been defined.
#  endif
#endif

/*
 * __PCL_UNIX is always defined on FreeBSD, Linux and macOS platforms.
 */
#if defined( __PCL_LINUX ) || defined( __PCL_FREEBSD ) || defined( __PCL_MACOSX )
#  ifndef __PCL_UNIX
#    define __PCL_UNIX
#  endif
#endif

/*
 * __PCL_X11 is always defined on FreeBSD and Linux platforms.
 */
#if defined( __PCL_LINUX ) || defined( __PCL_FREEBSD )
#  ifndef __PCL_X11
#    define __PCL_X11
#  endif
#endif

/*
 * Platform architecture requirements:
 *
 * - x86_64 / EM64T / AMD64 architecture.
 */
#ifndef __x86_64__
#  ifndef __x86_64
#    ifndef __amd64__
#      ifndef __amd64
#        ifndef _M_X64
#          ifndef _M_AMD64
#            error This version of PCL requires an x86_64 / EM64T / AMD64 architecture.
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef __PCL_X64
#  define __PCL_X64
#endif

/*
 * Compiler requirements:
 *
 * - GCC >= 4.8 on UNIX/Linux
 * - Clang >= 3.3 on UNIX/Linux
 * - MSVC++ >= 14.0 / Visual Studio 2015 on Windows
 */
#ifdef __PCL_WINDOWS
#  ifdef _MSC_VER
#    if _MSC_VER < 1900
#      error This version of PCL requires MSVC++ 14.0 / Visual Studio 2015 or higher on MS Windows platforms.
#    endif
#  else
#    error Unsupported C++ compiler on MS Windows platform.
#  endif
#else
#  ifdef _MSC_VER
#    error _MSC_VER should not be #defined on UNIX/Linux platforms.
#  endif
#  ifdef __GNUC__
#    define GCC_VERSION (__GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__)
#  endif
#  ifdef __clang__
#    if !__has_feature( cxx_inheriting_constructors )
#      error This version of PCL requires Clang 3.3 or higher on UNIX/Linux platforms.
#    endif
#  else
#    ifdef __GNUC__
#      if GCC_VERSION < 40800
#        error This version of PCL requires GCC 4.8 or higher on UNIX/Linux platforms.
#      endif
#    else
#      error Unsupported C++ compiler on UNIX/Linux platform.
#    endif
#  endif
#endif

/*
 * Brain-damaged compilers emit particularly useless warning messages.
 * N.B.: This must be done *before* including windows.h
 */
#ifndef __PCL_NO_WARNING_MUTE_PRAGMAS
#  ifdef _MSC_VER
#    define _CRT_SECURE_NO_DEPRECATE 1 // Deal with MS's attempt at deprecating C standard runtime functions
#    pragma warning( disable: 4996 )   // ...
#    pragma warning( disable: 4018 )   // '<' : signed/unsigned mismatch
#    pragma warning( disable: 4049 )   // More than 64k source lines
#    pragma warning( disable: 4244 )   // Conversion from 'type1' to 'type2', possible loss of data
#    pragma warning( disable: 4345 )   // Behavior change: an object of POD type constructed...
#    pragma warning( disable: 4355 )   // 'this' : used in base member initializer list
#    pragma warning( disable: 4521 )   // Multiple copy constructors defined
#    pragma warning( disable: 4522 )   // Multiple assignment operators defined
#    pragma warning( disable: 4710 )   // Function not inlined
#    pragma warning( disable: 4723 )   // Potential divide by 0
#    pragma warning( disable: 4800 )   // 'boolean' : forcing value to bool 'true' or 'false' (performance warning)
#  endif
#endif

/*
 * Compiler warning management macros
 */
#ifdef _MSC_VER
#  define PCL_WARNINGS_PUSH \
      __pragma(warning(push))
#else
#  define PCL_WARNINGS_PUSH \
      _Pragma("GCC diagnostic push")
#endif

#ifdef _MSC_VER
#  define PCL_WARNINGS_POP \
      __pragma(warning(pop))
#else
#  define PCL_WARNINGS_POP \
      _Pragma("GCC diagnostic pop")
#endif

#ifdef _MSC_VER
#  define PCL_WARNINGS_DISABLE_UNINITIALIZED_VARIABLE \
      __pragma(warning( disable: 6001 )) \
      __pragma(warning( disable: 4700 ))
#else
#  define PCL_WARNINGS_DISABLE_UNINITIALIZED_VARIABLE \
      _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#endif

// Disable C4267: 'initializing' : conversion from 'size_t' to 'int', possible loss of data
#ifdef _MSC_VER
#  define PCL_WARNINGS_DISABLE_SIZE_T_TO_INT_LOSS \
      __pragma(warning( disable: 4267 ))
#endif

/*
 * On Windows/MSVC++, _CRT_RAND_S must be #defined for rand_s() to work:
 * http://msdn.microsoft.com/en-us/library/sxtz2fa8.aspx
 */
#ifdef _MSC_VER
#  define _CRT_RAND_S
#endif

/*
 * C standard definitions.
 */
#ifndef __stddef_h
#  include <stddef.h>
#  ifndef __stddef_h
#    define __stddef_h
#  endif
#endif

/*
 * C++11: Utility functions.
 */
#ifndef __utility_h
#  include <utility>
#  ifndef __utility_h
#    define __utility_h
#  endif
#endif

/*
 * C++11: Compile-time type information classes.
 */
#ifndef __type_traits_h
#  include <type_traits>
#  ifndef __type_traits_h
#    define __type_traits_h
#  endif
#endif

/*
 * Macros that control global symbol visibility.
 *
 * On UNIX/Linux, compilation with -fvisibility=hidden is required for PCL and
 * all PixInsight modules and applications.
 */
#ifdef _MSC_VER               // Windows
#  define PCL_EXPORT          __declspec(dllexport)
#  define PCL_MODULE_EXPORT   extern "C" PCL_EXPORT
#  define PCL_IMPORT          __declspec(dllimport)
#  define PCL_LOCAL
#  define PCL_BEGIN_LOCAL
#  define PCL_END_LOCAL
#  define PCL_INTERNAL
#  define PCL_BEGIN_INTERNAL
#  define PCL_END_INTERNAL
#else                         // Linux/X11, FreeBSD/X11 and Mac OS X
#  define PCL_EXPORT          __attribute__((visibility ("default")))
#  if defined( __clang__ )    // Clang does not have the "externally_visible" attribute
#    define PCL_MODULE_EXPORT   extern "C" __attribute__((visibility ("default")))
#  else
#    define PCL_MODULE_EXPORT   extern "C" __attribute__((visibility ("default"), externally_visible))
#  endif
#  define PCL_IMPORT          __attribute__((visibility ("default")))
#  define PCL_LOCAL           __attribute__((visibility ("hidden")))
#  define PCL_BEGIN_LOCAL     _Pragma ( "GCC visibility push( hidden )" )
#  define PCL_END_LOCAL       _Pragma ( "GCC visibility pop" )
#  define PCL_INTERNAL        __attribute__((visibility ("internal")))
#  define PCL_BEGIN_INTERNAL  _Pragma ( "GCC visibility push( internal )" )
#  define PCL_END_INTERNAL    _Pragma ( "GCC visibility pop" )
#endif
#ifdef __PCL_DSO              // Using PCL as a dynamic shared object - ### untested/experimental
#  ifdef __PCL_BUILDING_PCL_DSO     // building the PCL DSO
#    define PCL_PUBLIC        PCL_EXPORT
#  else                             // linking against the PCL DSO
#    define PCL_PUBLIC        PCL_IMPORT
#  endif
#else                         // Using PCL as a static library
#  define PCL_PUBLIC
#endif

/*
 * The PCL_CLASS, PCL_FUNC and PCL_DATA macros allow us to modify attributes in
 * all declarations of PCL API classes, functions and data items, respectively.
 */
#define PCL_CLASS             PCL_PUBLIC
#define PCL_FUNC              PCL_PUBLIC
#define PCL_DATA              PCL_PUBLIC

/*
 * Fastcall calling convention.
 */
#ifndef __PCL_NO_FASTCALL
#  ifdef _MSC_VER
#    define PCL_FASTCALL      __fastcall
#  else
#    define PCL_FASTCALL      __attribute__((fastcall))
#  endif
#else
#  define PCL_FASTCALL
#endif

/*
 * Always inline functions.
 */
#ifndef __PCL_NO_FORCE_INLINE
#  ifdef _MSC_VER // Visual Studio
#    define PCL_FORCE_INLINE  __forceinline
#  else
#    define PCL_FORCE_INLINE  inline __attribute__((always_inline))
#  endif
#else
#  define PCL_FORCE_INLINE
#endif

/*
 * Special GCC function optimizations
 */
#ifndef __PCL_NO_HOT_FUNCTIONS
#  ifdef _MSC_VER
#    define PCL_HOT_FUNCTION
#  else
#    define PCL_HOT_FUNCTION  __attribute__((hot))
#  endif
#else
#  define PCL_HOT_FUNCTION
#endif

/*
 * Aligned memory allocation functions.
 */
#ifdef _MSC_VER
#  define PCL_ALIGNED_MALLOC  _aligned_malloc
#  define PCL_ALIGNED_FREE    _aligned_free
#else
#  define PCL_ALIGNED_MALLOC  _mm_malloc
#  define PCL_ALIGNED_FREE    _mm_free
#endif

/*
 * 16-byte aligned blocks for automatic vectorization.
 */
#ifdef _MSC_VER               // Windows
#  define PCL_ALIGNED16       __declspec(align(16))
#  define PCL_ASSUME16( x )   x
#else                         // GCC and Clang
#  define PCL_ALIGNED16       __attribute__((aligned(16)))
#  define PCL_ASSUME16( x )   __builtin_assume_aligned( x, 16 )
#endif

/*
 * Current function name.
 */
#ifdef _MSC_VER
#  define PCL_FUNCTION_NAME   __FUNCTION__
#else
#  define PCL_FUNCTION_NAME   __func__
#endif

/*
 * Minimum Win32 versions supported.
 */
#ifdef __PCL_WINDOWS
#  ifndef __PCL_NO_WIN32_MINIMUM_VERSIONS
#    define WINVER            0x0601 // Windows 7
#    define _WIN32_WINNT      0x0601
#  endif
#endif

/*
 * C++11 features not supported by Visual Studio 2013.
 */
#ifdef _MSC_VER
#  if _MSC_VER < 1900
#    define constexpr
#    define noexcept
#  endif
#endif

/*
 * Size and distance types.
 */
#ifndef __PCL_NO_SIZE_AND_DISTANCE_TYPES

namespace pcl
{
/*!
 * \defgroup size_distance_types Size and Distance Types
 */

/*!
 * A type used to represent block sizes, lengths and absolute positions.
 * \ingroup size_distance_types
 */
typedef size_t                size_type;

/*!
 * A type used to represent distances and relative positions.
 * \ingroup size_distance_types
 */
typedef ptrdiff_t             distance_type;

}  // pcl

#endif   // !__PCL_NO_SIZE_AND_DISTANCE_TYPES

/*
 * Portable integer types
 */
#ifndef __PCL_NO_PORTABLE_INTEGER_TYPES

namespace pcl
{
/*!
 * \defgroup portable_integer_types Portable Integer Types
 */

/*!
 * Signed two's complement 8-bit integer type.
 * \ingroup portable_integer_types
 */
typedef signed char           int8;

/*!
 * Unsigned 8-bit integer type.
 * \ingroup portable_integer_types
 */
typedef unsigned char         uint8;

/*!
 * Signed two's complement 16-bit integer type.
 * \ingroup portable_integer_types
 */
typedef signed short          int16;

/*!
 * Unsigned 16-bit integer type.
 * \ingroup portable_integer_types
 */
typedef unsigned short        uint16;

/*!
 * Signed two's complement 32-bit integer type.
 * \ingroup portable_integer_types
 */
typedef signed int            int32;

/*!
 * Unsigned 32-bit integer type.
 * \ingroup portable_integer_types
 */
typedef unsigned int          uint32;

#ifdef _MSC_VER   // Valid for MS Visual C++
typedef signed __int64        int64;
typedef unsigned __int64      uint64;
#else             // Valid for gcc
/*!
 * Signed two's complement 64-bit integer type.
 * \ingroup portable_integer_types
 */
typedef signed long long      int64;

/*!
 * Unsigned 64-bit integer type.
 * \ingroup portable_integer_types
 */
typedef unsigned long long    uint64;
#endif // _MSC_VER

struct PCL_AssertScalarSizes
{
   static_assert( sizeof( int8 ) == 1, "Invalid sizeof( int8 )" );
   static_assert( sizeof( uint8 ) == 1, "Invalid sizeof( uint8 )" );
   static_assert( sizeof( int16 ) == 2, "Invalid sizeof( int16 )" );
   static_assert( sizeof( uint16 ) == 2, "Invalid sizeof( uint16 )" );
   static_assert( sizeof( int32 ) == 4, "Invalid sizeof( int32 )" );
   static_assert( sizeof( uint32 ) == 4, "Invalid sizeof( uint32 )" );
   static_assert( sizeof( int64 ) == 8, "Invalid sizeof( int64 )" );
   static_assert( sizeof( uint64 ) == 8, "Invalid sizeof( uint64 )" );
   static_assert( sizeof( float ) == sizeof( uint32 ), "Invalid sizeof( float )" );
   static_assert( sizeof( double ) == sizeof( uint64 ), "Invalid sizeof( double )" );
};

} // pcl

/*
 * Integer limits
 */
#ifndef __PCL_NO_INTEGER_LIMITS

/*!
 * \defgroup integer_limits Integer Limits
 */

/*!
 * \defgroup integer_conversion_factors Integer Conversion Factors
 */

#ifdef _MSC_VER   // Valid for MS Visual C++

#define int8_min            (-127i8-1)
#define int8_max              127i8
#define int8_ofs              128ui8
#define char_min              int8_min
#define char_max              int8_max

#define uint8_max             0xffui8

#define int16_min           (-32767i16-1)
#define int16_max             32767i16
#define int16_ofs             32768ui16
#define short_min             int16_min
#define short_max             int16_max

#define uint16_max            0xffffui16
#define ushort_max            uint16_max

#define uint20_max            0xfffffui32

#define uint22_max            0x3fffffui32

#define uint24_max            0xffffffui32

#define int32_min           (-2147483647i32-1)
#define int32_max             2147483647i32
#define int32_ofs             2147483648ui32
#define int_min               int32_min
#define int_max               int32_max

#define uint32_max            0xffffffffui32
#define uint_max              uint32_max

#define int64_min           (-9223372036854775807i64-1)
#define int64_max             9223372036854775807i64
#define int64_ofs             9223372036854775808ui64
#define longlong_min          int64_min
#define longlong_max          int64_max

#define uint64_max            0xffffffffffffffffui64
#define ulonglong_max         uint64_max

#define uint8_to_uint16       257ui16
#define uint8_to_uint20       4.1120588235294117647058823529412e+03
#define uint8_to_uint24       65793ui32
#define uint8_to_uint32       16843009ui32
#define uint8_to_uint64       72340172838076673ui64

#define uint16_to_uint8       3.8910505836575875486381322957198e-03
#define uint16_to_uint20      1.6000228885328450446326390478370e+01
#define uint16_to_uint24      2.5600389105058365758754863813230e+02
#define uint16_to_uint32      65537ui32
#define uint16_to_uint64      281479271743489ui64

#define uint20_to_uint8       2.4318718260496387955081896860024e-04
#define uint20_to_uint16      6.2499105929475717044560474930262e-02
#define uint20_to_uint24      1.6000014305128388527287032401116e+01
#define uint20_to_uint32      4.0960039053000500679493598455046e+03
#define uint20_to_uint64      1.7592202821648000014305128388527e+13

#define uint24_to_uint8       1.5199185323666651467481343000015e-05
#define uint24_to_uint16      3.9061906281823294271427051510039e-03
#define uint24_to_uint20      6.2499944120642192402016663671533e-02
#define uint24_to_uint32      2.5600001519918532366665146748134e+02
#define uint24_to_uint64      1.0995116933120039061906281823294e+12

#define uint32_to_uint8       5.9371814145560333073502484027646e-08
#define uint32_to_uint16      1.5258556235409005599890138395105e-05
#define uint32_to_uint20      2.4414039222619971079430536152662e-04
#define uint32_to_uint24      3.9062497680788509939049489316309e-03
#define uint32_to_uint64      4294967297ui64

#define uint64_to_uint8       1.3823577699190181534344400859741e-17
#define uint64_to_uint16      3.5526594686918766543265110209534e-15
#define uint64_to_uint20      5.6843364650699390597549726005894e-14
#define uint64_to_uint24      9.0949464756281961368912116576494e-13
#define uint64_to_uint32      2.3283064359965952029459655278022e-10

#else // Non-MS C++, i.e. GCC or compatible

/*!
 * The smallest 8-bit signed integer.
 * \ingroup integer_limits
 */
#define int8_min            (-int8( 127 )-1)

/*!
 * The largest 8-bit signed integer.
 * \ingroup integer_limits
 */
#define int8_max              int8( 127 )
#define int8_ofs              uint8( 128u )
#define char_min              int8_min
#define char_max              int8_max

/*!
 * The largest 8-bit unsigned integer.
 * \ingroup integer_limits
 */
#define uint8_max             uint8( 0xffu )

/*!
 * The smallest 16-bit signed integer.
 * \ingroup integer_limits
 */
#define int16_min           (-int16( 32767 )-1)

/*!
 * The largest 16-bit signed integer.
 * \ingroup integer_limits
 */
#define int16_max             int16( 32767 )
#define int16_ofs             uint16( 32768u )
#define short_min             int16_min
#define short_max             int16_max

/*!
 * The largest 16-bit unsigned integer.
 * \ingroup integer_limits
 */
#define uint16_max            uint16( 0xffffu )
#define ushort_max            uint16_max

/*!
 * The largest 20-bit unsigned integer.
 * \ingroup integer_limits
 */
#define uint20_max            uint32( 0xfffffu )

/*!
 * The largest 22-bit unsigned integer.
 * \ingroup integer_limits
 */
#define uint22_max            uint32( 0x3fffffu )

/*!
 * The largest 24-bit unsigned integer.
 * \ingroup integer_limits
 */
#define uint24_max            uint32( 0xffffffu )

/*!
 * The smallest 32-bit signed integer.
 * \ingroup integer_limits
 */
#define int32_min           (-int32( 2147483647 )-1)

/*!
 * The largest 32-bit signed integer.
 * \ingroup integer_limits
 */
#define int32_max             int32( 2147483647 )
#define int32_ofs             uint32( 2147483648u )
#define int_min               int32_min
#define int_max               int32_max

/*!
 * The largest 32-bit unsigned integer.
 * \ingroup integer_limits
 */
#define uint32_max            uint32( 0xffffffffu )
#define uint_max              uint32_max

/*!
 * The smallest 64-bit signed integer.
 * \ingroup integer_limits
 */
#define int64_min           (-int64( 9223372036854775807ll )-1)

/*!
 * The largest 64-bit signed integer.
 * \ingroup integer_limits
 */
#define int64_max             int64( 9223372036854775807ll )
#define int64_ofs             uint64( 9223372036854775808ull )
#define longlong_min          int64_min
#define longlong_max          int64_max

/*!
 * The largest 64-bit unsigned integer.
 * \ingroup integer_limits
 */
#define uint64_max            uint64( 0xffffffffffffffffull )
#define ulonglong_max         uint64_max

/*!
 * Conversion factor from 8-bit unsigned integer to 16-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint8_to_uint16       uint16( 257u )

/*!
 * Conversion factor from 8-bit unsigned integer to 20-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint8_to_uint20       4.1120588235294117647058823529412e+03

/*!
 * Conversion factor from 8-bit unsigned integer to 24-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint8_to_uint24       uint32( 65793u )

/*!
 * Conversion factor from 8-bit unsigned integer to 32-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint8_to_uint32       uint32( 16843009u )

/*!
 * Conversion factor from 8-bit unsigned integer to 64-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint8_to_uint64       uint64( 72340172838076673ull )

/*!
 * Conversion factor from 16-bit unsigned integer to 8-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint16_to_uint8       3.8910505836575875486381322957198e-03

/*!
 * Conversion factor from 16-bit unsigned integer to 20-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint16_to_uint20      1.6000228885328450446326390478370e+01

/*!
 * Conversion factor from 16-bit unsigned integer to 24-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint16_to_uint24      2.5600389105058365758754863813230e+02

/*!
 * Conversion factor from 16-bit unsigned integer to 32-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint16_to_uint32      uint32( 65537u )

/*!
 * Conversion factor from 16-bit unsigned integer to 64-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint16_to_uint64      uint64( 281479271743489ull )

/*!
 * Conversion factor from 20-bit unsigned integer to 8-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint20_to_uint8       2.4318718260496387955081896860024e-04

/*!
 * Conversion factor from 20-bit unsigned integer to 16-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint20_to_uint16      6.2499105929475717044560474930262e-02

/*!
 * Conversion factor from 20-bit unsigned integer to 24-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint20_to_uint24      1.6000014305128388527287032401116e+01

/*!
 * Conversion factor from 20-bit unsigned integer to 32-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint20_to_uint32      4.0960039053000500679493598455046e+03

/*!
 * Conversion factor from 20-bit unsigned integer to 64-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint20_to_uint64      1.7592202821648000014305128388527e+13

/*!
 * Conversion factor from 24-bit unsigned integer to 8-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint24_to_uint8       1.5199185323666651467481343000015e-05

/*!
 * Conversion factor from 24-bit unsigned integer to 16-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint24_to_uint16      3.9061906281823294271427051510039e-03

/*!
 * Conversion factor from 24-bit unsigned integer to 20-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint24_to_uint20      6.2499944120642192402016663671533e-02

/*!
 * Conversion factor from 24-bit unsigned integer to 32-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint24_to_uint32      2.5600001519918532366665146748134e+02

/*!
 * Conversion factor from 24-bit unsigned integer to 64-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint24_to_uint64      1.0995116933120039061906281823294e+12

/*!
 * Conversion factor from 32-bit unsigned integer to 8-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint32_to_uint8       5.9371814145560333073502484027646e-08

/*!
 * Conversion factor from 32-bit unsigned integer to 16-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint32_to_uint16      1.5258556235409005599890138395105e-05

/*!
 * Conversion factor from 32-bit unsigned integer to 20-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint32_to_uint20      2.4414039222619971079430536152662e-04

/*!
 * Conversion factor from 32-bit unsigned integer to 24-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint32_to_uint24      3.9062497680788509939049489316309e-03

/*!
 * Conversion factor from 32-bit unsigned integer to 64-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint32_to_uint64      uint64( 4294967297ull )

/*!
 * Conversion factor from 64-bit unsigned integer to 8-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint64_to_uint8       1.3823577699190181534344400859741e-17

/*!
 * Conversion factor from 64-bit unsigned integer to 16-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint64_to_uint16      3.5526594686918766543265110209534e-15

/*!
 * Conversion factor from 64-bit unsigned integer to 20-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint64_to_uint20      5.6843364650699390597549726005894e-14

/*!
 * Conversion factor from 64-bit unsigned integer to 24-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint64_to_uint24      9.0949464756281961368912116576494e-13

/*!
 * Conversion factor from 64-bit unsigned integer to 32-bit unsigned integer.
 * \ingroup integer_conversion_factors
 */
#define uint64_to_uint32      2.3283064359965952029459655278022e-10

#endif   // _MSC_VER

namespace pcl
{

inline double BitMin( int n )
{
   switch ( n )
   {
   case  8: return  double( int8_min );
   case 16: return  double( int16_min );
   case 32: return  double( int32_min );
   case 64: return  double( int64_min );
   default: return -double( (uint64( 1 ) << n) >> 1 );
   }
}

inline double BitMax( int n )
{
   switch ( n )
   {
   case  8: return  double( int8_max );
   case 16: return  double( int16_max );
   case 32: return  double( int32_max );
   case 64: return  double( int64_max );
   default: return  double( ((uint64( 1 ) << n) >> 1) - 1 );
   }
}

inline double UBitMax( int n )
{
   switch ( n )
   {
   case  8: return  double( uint8_max );
   case 16: return  double( uint16_max );
   case 32: return  double( uint32_max );
   case 64: return  double( uint64_max );
   default: return  double( (uint64( 1 ) << n) - 1 );
   }
}

} // pcl

#endif   // !__PCL_NO_INTEGER_LIMITS

/*
 * Unicode character types.
 */
#ifndef __PCL_NO_UNICODE_CHARACTER_TYPES

namespace pcl
{

/*!
 * \defgroup unicode_char_types Unicode Character Types
 */

/*!
 * 16-bit Unicode character (UTF-16)
 * \ingroup unicode_char_types
 */
typedef uint16                char16_type;

/*!
 * 32-bit Unicode character (UTF-32)
 * \ingroup unicode_char_types
 */
typedef uint32                char32_type;

} // pcl

#endif   // !__PCL_NO_UNICODE_CHARACTER_TYPES

/*
 * File size and position types.
 */
#ifndef __PCL_NO_FILE_SIZE_AND_POSITION_TYPES

#ifndef __PCL_WINDOWS
#  ifndef _FILE_OFFSET_BITS
#    define _FILE_OFFSET_BITS  64
#  endif
#  define _LARGEFILE_SOURCE   1
#  define _LARGEFILE64_SOURCE 1
#endif

namespace pcl
{

/*!
 * \defgroup file_size_and_pos_types File Size and Position Types
 *
 * File positions are always signed numbers. We support 64-bit file systems.
 */

/*!
 * A type used to store a byte position or a byte offset in a file.
 * \ingroup file_size_and_pos_types
 */
typedef int64                 fpos_type;

/*!
 * A type used to store the length of a file in bytes.
 * \ingroup file_size_and_pos_types
 */
typedef int64                 fsize_type;

}  // pcl

#endif   // !__PCL_NO_FILE_SIZE_AND_POSITION_TYPES

#endif   // !__PCL_NO_PORTABLE_INTEGER_TYPES

/*
 * Maximum number of processors allowed.
 */
#define PCL_MAX_PROCESSORS          1023
#define PCL_MAX_PROCESSORS_BITCOUNT 10

// ----------------------------------------------------------------------------

#endif   // __PCL_Defs_h

// ----------------------------------------------------------------------------
// EOF pcl/Defs.h - Released 2017-07-09T18:07:07Z
