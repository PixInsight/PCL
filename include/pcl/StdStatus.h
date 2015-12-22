//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/StdStatus.h - Released 2015/12/17 18:52:09 UTC
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

#ifndef __PCL_StdStatus_h
#define __PCL_StdStatus_h

/// \file pcl/StdStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_StatusMonitor_h
#include <pcl/StatusMonitor.h>
#endif

#ifndef __PCL_Console_h
#include <pcl/Console.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class StandardStatus
 * \brief A status monitoring callback that sends progress information to the
 *        process console.
 *
 * %StandardStatus uses the processing console associated with the current
 * processing thread to provide feedback and progress information on a
 * process being monitored by a StatusMonitor object.
 *
 * %StandardStatus is suitable for processes where a total <em>status
 * monitoring count</em> can be computed in advance. For example, this happens
 * with processes that operate on a pixel-by-pixel basis and perform a fixed
 * number of iterations, or a number of iterations that can be predicted before
 * running the process. For cases where a total count is not available or
 * cannot be computed, the SpinStatus class is more suitable.
 *
 * %StandardStatus and %SpinStatus are used by all standard PixInsight
 * processes and file formats. Unless a specific module has an extremely good
 * reason to proceed otherwise, we recommend that all modules also use these
 * monitoring classes. This ensures that efficient and coherent feedback is
 * provided to the user on the entire PixInsight platform.
 *
 * In addition, using %StandardStatus (and also %SpinStatus) allows the
 * PixInsight core application to process mouse, keyboard and screen update
 * events. In this way the application's GUI can be responsive if the user
 * requests aborting or suspending the monitored process.
 *
 * \sa StatusCallback, StatusMonitor, Console, SpinStatus, MuteStatus
 */
class PCL_CLASS StandardStatus : public StatusCallback
{
public:

   /*!
    * Constructs a default %StandardStatus object.
    */
   StandardStatus() : StatusCallback(), m_console(), m_last( -1 ), m_thread( 0 )
   {
   }

   /*!
    * Constructs a %StandardStatus object as a copy of an existing instance.
    */
   StandardStatus( const StandardStatus& x ) : StatusCallback( x ), m_console(), m_last( -1 ), m_thread( 0 )
   {
   }

   /*!
    * Destroys a %StandardStatus instance.
    */
   virtual ~StandardStatus()
   {
   }

   /*!
    * This function is called by a status \a monitor object when a new
    * monitored process is about to start.
    */
   virtual int Initialized( const StatusMonitor& monitor ) const;

   /*!
    * Function called by a status \a monitor object to signal an update of the
    * progress count for the current process.
    */
   virtual int Updated( const StatusMonitor& monitor ) const;

   /*!
    * Function called by a status \a monitor object to signal that the current
    * process has finished.
    */
   virtual int Completed( const StatusMonitor& monitor ) const;

   /*!
    * Function called by a status \a monitor object when the progress
    * information for the current process has been changed.
    */
   virtual void InfoUpdated( const StatusMonitor& monitor ) const;

private:

   mutable pcl::Console m_console;
   mutable int          m_last;
   mutable void*        m_thread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_StdStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/StdStatus.h - Released 2015/12/17 18:52:09 UTC
