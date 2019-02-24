//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/ParallelProcess.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_ParallelProcess_h
#define __PCL_ParallelProcess_h

/// \file pcl/ParallelProcess.h

#include <pcl/Defs.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ParallelProcess
 * \brief A process using multiple concurrent execution threads.
 *
 * %ParallelProcess is a base class for classes implementing multithreaded
 * processes in PCL.
 */
class PCL_CLASS ParallelProcess
{
public:

   /*!
    * Default constructor.
    *
    * By default, parallel processing is always enabled, and any process can
    * use the maximum possible number of parallel execution threads.
    */
   ParallelProcess() = default;

   /*!
    * Copy constructor.
    */
   ParallelProcess( const ParallelProcess& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ParallelProcess& operator =( const ParallelProcess& ) = default;

   /*!
    * Returns true iff this process is allowed to use multiple parallel
    * execution threads, when multiple threads are permitted and available.
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this process.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance. If \a enable is false this parameter is ignored.
    *                A value &le; 0 is ignored. The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( m_parallel )
         if ( maxProcessors > 0 )
            SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this process.
    *
    * This is a convenience function, equivalent to:
    *
    * \code EnableParallelProcessing( !disable ) \endcode
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this process.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable. Refer to the GlobalSettings
    * class for information on global variables.
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this process.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable. Refer to the GlobalSettings
    * class for information on global variables.
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = Range( maxProcessors, 1, PCL_MAX_PROCESSORS );
   }

   /*!
    * Exchanges two %ParallelProcess instances.
    */
   void Swap( ParallelProcess& process )
   {
      pcl::Swap( m_maxProcessors, process.m_maxProcessors );
      pcl::Swap( m_parallel, process.m_parallel );
   }

protected:

   int  m_maxProcessors = PCL_MAX_PROCESSORS; // maximum number of processors allowed
   bool m_parallel = true; // use multiple threads?
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ParallelProcess_h

// ----------------------------------------------------------------------------
// EOF pcl/ParallelProcess.h - Released 2019-01-21T12:06:07Z
