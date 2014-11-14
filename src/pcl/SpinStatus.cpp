// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/SpinStatus.cpp - Released 2014/11/14 17:17:01 UTC
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

#include <pcl/SpinStatus.h>

#include <pcl/api/APIInterface.h>

#define THREAD_ABORT ((*API->Thread->GetThreadStatus)( m_thread ) & 0x80000000) != 0

namespace pcl
{

// ----------------------------------------------------------------------------

static const char* cspin[ 4 ] = { "&mdash;", "\\", "|", "/" };

// ----------------------------------------------------------------------------

/*
 * Non-blocking thread status
 */
inline static
bool ThreadAborted( void* thread )
{
   uint32 status;
   return (*API->Thread->GetThreadStatusEx)( thread, &status, 0x00000001 ) != api_false &&
          (status & 0x80000000) != 0;
}

// ----------------------------------------------------------------------------

int SpinStatus::Initialized( const StatusMonitor& monitor ) const
{
   m_thread = (*API->Thread->GetCurrentThread)();
   if ( m_thread != 0 )
   {
      if ( ThreadAborted( m_thread ) )
         return 1;

      m_console.Write( "<end><cbr>" + monitor.Info() + ": " );
   }
   else
   {
      if ( m_console.AbortRequested() )
      {
         m_console.Abort();
         return 1;
      }

      m_console.Write( "<end><cbr>" + monitor.Info() + ": " + cspin[0] );
      m_spinCount = 0;
   }

   return 0;
}

// ----------------------------------------------------------------------------

int SpinStatus::Updated( const StatusMonitor& monitor ) const
{
   if ( m_thread != 0 )
   {
      if ( ThreadAborted( m_thread ) )
      {
         m_console.WriteLn( "<end>*" );
         return 1;
      }
   }
   else
   {
      if ( m_console.AbortRequested() )
      {
         m_console.Write( "<end>\b*" );
         m_console.Abort();
         return 1;
      }

      m_console.Write( "<end>\b" + String( cspin[m_spinCount++%4] ) );
   }

   return 0;
}

// ----------------------------------------------------------------------------

int SpinStatus::Completed( const StatusMonitor& monitor ) const
{
   (void)Updated( monitor );
   m_console.WriteLn( m_thread ? "<end>done" : "<end>\bdone" );
   return 0;
}

// ----------------------------------------------------------------------------

void SpinStatus::InfoUpdated( const StatusMonitor& monitor ) const
{
   if ( m_thread != 0 )
   {
      m_console.WriteLn( "<end>*" );
      m_console.Write( "<end><cbr>" + monitor.Info() + ": " );
   }
   else
   {
      if ( !m_console.AbortRequested() )
         m_console.Write( "<end><bol><clreol>" + monitor.Info() + ": " + cspin[m_spinCount%4] );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/SpinStatus.cpp - Released 2014/11/14 17:17:01 UTC
