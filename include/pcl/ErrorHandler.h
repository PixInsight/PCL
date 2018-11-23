//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/ErrorHandler.h - Released 2018-11-23T16:14:19Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_ErrorHandler_h
#define __PCL_ErrorHandler_h

/// \file pcl/ErrorHandler.h

#include <pcl/Defs.h>

#include <pcl/Exception.h>

#include <new> // std::bad_alloc

// ----------------------------------------------------------------------------

#ifdef __PCL_SILENTLY_REPLACE_USER_ERROR_MACROS

#undef ERROR_HANDLER
#undef ERROR_CLEANUP

#else

#ifdef ERROR_HANDLER
#error ERROR_HANDLER macro: Already defined!
#endif

#ifdef ERROR_CLEANUP
#error ERROR_CLEANUP macro: Already defined!
#endif

#endif

// ----------------------------------------------------------------------------

/*!
 * \defgroup error_handling_macros Error Handling Macros
 */

// ----------------------------------------------------------------------------

/*!
 * \def ERROR_HANDLER
 * \brief Standard PCL error handler macro
 *
 * Use the ERROR_HANDLER macro after try blocks in all PCL programs and
 * modules.
 *
 * \ingroup error_handling_macros
 */
#define ERROR_HANDLER                                                         \
   catch ( pcl::FatalError& )                                                 \
   {                                                                          \
      /* Launch fatal errors to the hyperspace... */                          \
      throw;                                                                  \
   }                                                                          \
   catch ( pcl::ProcessAborted& )                                             \
   {                                                                          \
      /* Abort process: Let the PI application do the GUI output */           \
   }                                                                          \
   catch ( pcl::CaughtException& )                                            \
   {                                                                          \
      /* Exceptions already caught and managed: simply ignore them */         \
   }                                                                          \
   catch ( pcl::Exception& x )                                                \
   {                                                                          \
      /* Other PCL exceptions */                                              \
      x.Show();                                                               \
   }                                                                          \
   catch ( pcl::String& s )                                                   \
   {                                                                          \
      /* Nonstandard ways of throwing things... */                            \
      try                                                                     \
      {                                                                       \
         throw pcl::Error( s );                                               \
      }                                                                       \
      catch ( pcl::Exception& x )                                             \
      {                                                                       \
         x.Show();                                                            \
      }                                                                       \
   }                                                                          \
   catch ( std::bad_alloc& )                                                  \
   {                                                                          \
      /* Standard bad allocation exception */                                 \
      try                                                                     \
      {                                                                       \
         throw pcl::Error( "Out of memory" );                                 \
      }                                                                       \
      catch ( pcl::Exception& x )                                             \
      {                                                                       \
         x.Show();                                                            \
      }                                                                       \
   }                                                                          \
   catch ( ... )                                                              \
   {                                                                          \
      /* Catch-all */                                                         \
      try                                                                     \
      {                                                                       \
         throw pcl::Error( "Unknown exception" );                             \
      }                                                                       \
      catch ( pcl::Exception& x )                                             \
      {                                                                       \
         x.Show();                                                            \
      }                                                                       \
   }

// ----------------------------------------------------------------------------

/*!
 * \def ERROR_CLEANUP
 * \brief Standard PCL error handler macro with cleanup code
 *
 * Use ERROR_CLEANUP in the same way as the ERROR_HANDLER macro, when you need
 * to specify a local clean up code, e.g. to delete dynamically allocated local
 * variables.
 *
 * \ingroup error_handling_macros
 */
#define ERROR_CLEANUP( cleanup_code )                                         \
   catch ( ... )                                                              \
   {                                                                          \
      cleanup_code;                                                           \
                                                                              \
      try                                                                     \
      {                                                                       \
         throw;                                                               \
      }                                                                       \
      ERROR_HANDLER                                                           \
   }

// ----------------------------------------------------------------------------

#endif   // __PCL_ErrorHandler_h

// ----------------------------------------------------------------------------
// EOF pcl/ErrorHandler.h - Released 2018-11-23T16:14:19Z
