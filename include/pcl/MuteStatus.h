//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/MuteStatus.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_MuteStatus_h
#define __PCL_MuteStatus_h

/// \file pcl/MuteStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Console.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MuteStatus
 * \brief A status monitoring callback that does not provide any progress
 *        information.
 *
 * %MuteStatus does not provide any information on the ongoing monitored
 * process. However, it does check if either the current thread has been
 * aborted (when the status monitoring process has been initiated from a
 * running Thread object), or if the user has requested to cancel the current
 * process (when the process has been initiated from the main thread). In both
 * cases %MuteStatus attempts to terminate the process being monitored.
 *
 * %MuteStatus is useful as a specialized monitoring callback for processes
 * running in threads, or for fast processes that don't require providing
 * detailed progress information to the user. Compared to other callback
 * objects such as StandardStatus and SpinStatus, %MuteStatus saves the tasks
 * of generating textual information and writing it to GUI controls. These
 * auxiliary tasks can be expensive for fast processes, and in some cases they
 * may require even more computing time than the processes themselves.
 *
 * \sa StatusCallback, StatusMonitor, StandardStatus, SpinStatus,
 * RealTimeProgressStatus, Console
 */
class PCL_CLASS MuteStatus : public StatusCallback
{
public:

   /*!
    * Constructs a default %MuteStatus object.
    */
   MuteStatus() : StatusCallback(), m_thread( 0 ), m_console()
   {
   }

   /*!
    * Constructs a %MuteStatus object as a copy of an existing instance.
    */
   MuteStatus( const MuteStatus& x ) : StatusCallback( x ), m_thread( 0 ), m_console()
   {
   }

   /*!
    * Destroys a %MuteStatus instance.
    */
   virtual ~MuteStatus()
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

   mutable void*        m_thread;
   mutable pcl::Console m_console;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_MuteStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/MuteStatus.h - Released 2017-08-01T14:23:31Z
