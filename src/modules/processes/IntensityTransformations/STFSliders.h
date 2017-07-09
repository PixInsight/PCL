//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// STFSliders.h - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __STFSliders_h
#define __STFSliders_h

#include <pcl/Control.h>

#include "ScreenTransferFunctionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Bitmap;
class Graphics;

class STFSliders : public Control
{
public:

   STFSliders();
   virtual ~STFSliders();

   void SetChannel( int c, bool rgb );

   bool IsRGB() const
   {
      return rgb;
   }

   void SetValues( const STF& );

   void SetVisibleRange( double a, double b );

   void GetVisibleRange( double& a, double& b ) const
   {
      a = v0; b = v1;
   }

   // Event handlers

   typedef void (Control::*value_event_handler)( STFSliders& sender,
                                                 int channel, int item, double value,
                                                 unsigned modifiers, bool final );

   typedef void (Control::*range_event_handler)( STFSliders& sender,
                                                 int channel, double v0, double v1,
                                                 unsigned modifiers );

   void OnValueUpdated( value_event_handler, Control& );
   void OnRangeUpdated( range_event_handler, Control& );

private:

   int      channel;
   bool     rgb;
   double   m, c0, c1;
   double   v0, v1;
   Bitmap   gradient;
   int      beingDragged;
   bool     scrolling;
   int      scrollOrigin;
   int      m_wheelSteps;  // accumulated 1/8-degree wheel steps


   value_event_handler onValueUpdated         = nullptr;
   Control*            onValueUpdatedReceiver = nullptr;

   range_event_handler onRangeUpdated         = nullptr;
   Control*            onRangeUpdatedReceiver = nullptr;

   double SliderToSTF( int x ) const
   {
      return v0 + x*(v1 - v0)/(Width() - 1);
   }

   int STFToSlider( double v ) const
   {
      return RoundInt( (Width() - 1)*(v - v0)/(v1 - v0) );
   }

   FPoint ZoomInRange( int x ) const
   {
      double dv = v1 - v0;
      double a = Max( 0.0, SliderToSTF( x ) - dv/4 );
      double b = Min( a + dv/2, 1.0 );
      return FPoint( a, b );
   }

   FPoint ZoomOutRange( int x ) const
   {
      double dv = v1 - v0;
      double a = Max( 0.0, SliderToSTF( x ) - dv );
      double b = Min( a + 2*dv, 1.0 );
      return FPoint( a, b );
   }

   void InvalidateGradient();
   void RebuildGradient();
   RGBA HandlerColor( double ) const;
   void DrawHandler( Graphics&, double );
   int FindHandler( double );
   double UpdateDragging( int );
   bool UpdateScrolling( int );

   void __Paint( Control& sender, const pcl::Rect& updateRect );
   void __Resize( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );
   void __MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void __MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __MouseWheel( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __STFSliders_h

// ----------------------------------------------------------------------------
// EOF STFSliders.h - Released 2017-07-09T18:07:33Z
