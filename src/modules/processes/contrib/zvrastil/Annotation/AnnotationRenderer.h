//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 01.00.00.0154
// ----------------------------------------------------------------------------
// AnnotationRenderer.h - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2015 Zbynek Vrastil
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#ifndef __AnnotationRenderer_h
#define __AnnotationRenderer_h

#include "AnnotationInstance.h"

#include "pcl/Graphics.h"
#include "pcl/Font.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// AnnotationProcess
// ----------------------------------------------------------------------------

// Auxiliary static class, providing annotation rendering for dynamic interface
// and final execution
class AnnotationRenderer
{
public:

   // Builds the font according to current instance parameters
   static Font GetAnnotationFont( const AnnotationInstance& instance, double zoomFactor = 1.0 );

   static Rect GetTextRect( const AnnotationInstance& instance, Graphics& g,
      int posX, int posY );

   // computes the rectangle, which contains both text and leader
   // also computes relative positions of the text and leader endpoint in this rectangle
   static Rect GetBoundingRectangle( const AnnotationInstance& instance,
      int &relPosX, int &relPosY, int &relLeaderPosX, int &relLeaderPosY );

   // Renders the given annotation to a given graphics object.
   // Coordinates should be given relative to actual drawing context
   static void RenderAnnotation( const AnnotationInstance& instance, Graphics& g,
      int posX, int posY, int leaderPosX, int leaderPosY);

   // Renders the given annotation to a given graphics object.
   // Coordinates should be given relative to actual drawing context
   static void RenderGrips( const AnnotationInstance& instance, Graphics& g,
      int posX, int posY, int leaderPosX, int leaderPosY);

   // Creates bitmap and renders annotation with leader into the bitmap
   // returns the bitmap and relative position of the text in the bitmap
   static Bitmap CreateAnnotationBitmap( const AnnotationInstance& instance,
      int &relPosX, int &relPosY,
      bool renderGrips );

private:

   // Helper method. Renders text and leader with current pen to given location + offset.
   static void RenderTextAndLeader( const AnnotationInstance& instance, Graphics& g,
      const Rect& textRect, int leaderPosX, int leaderPosY, int offsetX, int offsetY );
};


// ----------------------------------------------------------------------------

} // pcl

#endif   // __AnnotationRenderer_h

// ----------------------------------------------------------------------------
// EOF AnnotationRenderer.h - Released 2015/11/26 16:00:13 UTC
