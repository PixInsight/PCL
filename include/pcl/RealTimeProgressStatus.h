//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/RealTimeProgressStatus.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_RealTimeProgressStatus_h
#define __PCL_RealTimeProgressStatus_h

/// \file pcl/RealTimeProgressStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class RealTimeProgressStatus
 * \brief A status monitoring callback to provide progress feedback during long
 * real-time preview generation procedures.
 *
 * %RealTimeProgressStatus allows you to drive the Real-Time Preview progress
 * dialog during long real-time preview generation tasks. Using this class is
 * recommended for real-time renditions consistently taking more than a couple
 * of seconds to complete on contemporary hardware.
 *
 * For detailed information on the Real-Time Preview system and its associated
 * modal progress dialog functionality, please refer to the RealTimePreview
 * class and the RealTimePreview::ShowProgressDialog() static member function.
 *
 * \sa RealTimePreview, StatusCallback, StatusMonitor, MuteStatus
 */
class PCL_CLASS RealTimeProgressStatus : public MuteStatus
{
public:

   /*!
    * Constructs a %RealTimeProgressStatus instance.
    *
    * \param title   The text that will be shown as the tile of the Real-Time
    *                Preview progress dialog window. Typically this text will
    *                be shown centered on the dialog's title bar.
    */
   RealTimeProgressStatus( const String& title ) : MuteStatus(), m_title( title )
   {
   }

   /*!
    * Copy constructor.
    */
   RealTimeProgressStatus( const RealTimeProgressStatus& x ) : MuteStatus( x ), m_title( x.m_title )
   {
   }

   /*!
    * This function is called by a status \a monitor object when a new
    * monitored process is about to start.
    *
    * As reimplemented in %RealTimeProgressStatus, this function opens the
    * Real-Time Preview progress dialog window and initializes it with the
    * monitor's total count and information text.
    */
   virtual int Initialized( const StatusMonitor& monitor ) const
   {
      if ( MuteStatus::Initialized( monitor ) == 0 )
      {
         RealTimePreview::ShowProgressDialog( m_title, monitor.Info(), monitor.Total() );
         return 0;
      }
      return 1;
   }

   /*!
    * Function called by a status \a monitor object to signal an update of the
    * progress count for the current process.
    *
    * As reimplemented in %RealTimeProgressStatus, this function updates the
    * Real-Time Preview progress dialog with the current monitor's count.
    */
   virtual int Updated( const StatusMonitor& monitor ) const
   {
      if ( MuteStatus::Updated( monitor ) == 0 )
      {
         RealTimePreview::SetProgressCount( monitor.Count() );
         return 0;
      }
      return 1;
   }

   /*!
    * Function called by a status \a monitor object to signal that the current
    * process has finished.
    *
    * As reimplemented in %RealTimeProgressStatus, this function closes the
    * Real-Time Preview progress dialog window.
    */
   virtual int Completed( const StatusMonitor& monitor ) const
   {
      if ( MuteStatus::Completed( monitor ) == 0 )
      {
         RealTimePreview::CloseProgressDialog();
         return 0;
      }
      return 1;
   }

   /*!
    * Function called by a status \a monitor object when the progress
    * information for the current process has been changed.
    *
    * As reimplemented in %RealTimeProgressStatus, this function updates the
    * single-line label text on the Real-Time Preview progress dialog.
    */
   virtual void InfoUpdated( const StatusMonitor& monitor ) const
   {
      RealTimePreview::SetProgressText( monitor.Info() );
   }

private:

   String m_title;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_RealTimeProgressStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/RealTimeProgressStatus.h - Released 2017-06-28T11:58:36Z
