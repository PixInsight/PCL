//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.02.0025
// ----------------------------------------------------------------------------
// GraphWebView.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#ifndef __GRAPHWEBVIEW_h
#define __GRAPHWEBVIEW_h

#include <pcl/WebView.h>
#include <pcl/Timer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct DataPoint
{
   double x;
   double data;
   double weight;
   bool   approved;
   bool   locked;
};

typedef GenericVector<DataPoint> DataPointVector;
typedef Array<DataPoint> DataPointArray;

// ----------------------------------------------------------------------------

class DataPointSortingBinaryPredicate
{
public:

   DataPointSortingBinaryPredicate( bool y = false ) : m_y( y )
   {
   }

   constexpr bool operator()( const DataPoint& s1, const DataPoint& s2 ) const
   {
      return m_y ? s1.data < s2.data : s1.x < s2.x;
   }

private:

   bool m_y;
};

// ----------------------------------------------------------------------------

class GraphWebView : public WebView
{
public:

   GraphWebView( Control& parent );

   void SetDataset( const String& dataname, const DataPointVector* dataset );

   typedef void (Control::*approve_event_handler)( GraphWebView& sender, int& index );
   typedef void (Control::*unlock_event_handler)( GraphWebView& sender, int& index );

   void OnApprove( approve_event_handler handler, Control& receiver );
   void OnUnlock( unlock_event_handler handler, Control& receiver );

   void Cleanup();

private:

   String Header() const;
   String Footer() const;

   void __MouseEnter( Control& sender );
   void __MouseLeave( Control& sender );
   void __Timer( Timer& sender );
   void __JSResult( WebView& sender, const Variant& result );

   Timer  m_eventCheckTimer;
   bool   m_keepChecking = true;
   String m_htmlFilePath;

   struct EventHandlers
   {
      approve_event_handler   onApprove = nullptr;
      Control*                onApproveReceiver = nullptr;
      unlock_event_handler    onUnlock = nullptr;
      Control*                onUnlockReceiver = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_eventHandlers;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GRAPHWEBVIEW_h

// ----------------------------------------------------------------------------
// EOF GraphWebView.h - Released 2019-01-21T12:06:42Z
