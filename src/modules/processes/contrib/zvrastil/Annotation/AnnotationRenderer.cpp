//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 01.00.00.0229
// ----------------------------------------------------------------------------
// AnnotationRenderer.cpp - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2018 Zbynek Vrastil
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#include "AnnotationRenderer.h"

namespace pcl
{

#define LEADER_GRIP_RADIUS 5

// ----------------------------------------------------------------------------

Font AnnotationRenderer::GetAnnotationFont( const AnnotationInstance& instance, double zoomFactor )
{
   // create font, accordint to current annotation parameters
   // font size is scaled by zoom factor
   Font font( instance.annotationFont, (double)instance.annotationFontSize * zoomFactor );

   font.SetBold( instance.annotationFontBold );
   font.SetItalic( instance.annotationFontItalic );
   font.SetUnderline( instance.annotationFontUnderline );

   return font;
}

// ----------------------------------------------------------------------------

Rect AnnotationRenderer::GetBoundingRectangle( const AnnotationInstance& instance,
      int &relPositionX, int &relPositionY, int &relLeaderPositionX, int &relLeaderPositionY )
{
   // get annotation font
   Font f = GetAnnotationFont( instance );
   int offset = f.Width("M")+1;

   // get the text rectangle
   Rect textRect( f.Width( instance.annotationText ) + 1, f.Height() + 1 );
   relPositionX = 0;
   relPositionY = 0;

   if ( instance.annotationShowLeader )
   {
      // first adjust horizontally

      // if the leader is to be drawn to the left of the text
      if ( instance.annotationLeaderPositionX < instance.annotationPositionX + textRect.Width() / 2 )
      {
         // check if leader line points to left
         if ( instance.annotationLeaderPositionX < instance.annotationPositionX - offset )
         {
            // adjust relative coordinates
            relPositionX = instance.annotationPositionX - instance.annotationLeaderPositionX;
            relLeaderPositionX = 0;
         }
         // if the leader point to the right, just extend by horizontal offset line
         else
         {
            // adjust relative coordinates
            relPositionX = offset;
            relLeaderPositionX = instance.annotationLeaderPositionX - ( instance.annotationPositionX - offset );
         }
         // add enough space to the rectangle
         textRect.ResizeBy( relPositionX, 0 );
      }
      // if the leader is to be drawn to the right of the text
      else
      {
         // check if leader line points to right
         if ( instance.annotationLeaderPositionX > instance.annotationPositionX+textRect.Width()+offset )
         {
            // add enough space to the rectangle
            textRect.ResizeBy( instance.annotationLeaderPositionX + 2 - ( instance.annotationPositionX + textRect.Width() ), 0 );
         }
         // if the leader point to the left, just extend by horizontal offset line
         else
         {
            // add enough space to the rectangle
            textRect.ResizeBy( offset+2, 0 );
         }

         // adjust relative coordinates
         relLeaderPositionX = instance.annotationLeaderPositionX-instance.annotationPositionX;
      }

      // then adjust vertically

      // if the leader goes above text
      if ( instance.annotationLeaderPositionY < instance.annotationPositionY )
      {
         relPositionY = instance.annotationPositionY - instance.annotationLeaderPositionY;
         relLeaderPositionY = 0;
         textRect.ResizeBy( 0, relPositionY );
      }
      else
      {
         // adjust relative coordinates
         relLeaderPositionY = instance.annotationLeaderPositionY - instance.annotationPositionY;

         // if the leader goes below the text
         if ( instance.annotationLeaderPositionY > instance.annotationPositionY + textRect.Height() )
         {
            // add enough space to the rectangle
            textRect.ResizeBy( 0, instance.annotationLeaderPositionY - ( instance.annotationPositionY + textRect.Height() ) );
         }
      }

      // add extra space for the leader grip
      textRect.ResizeBy( 2*LEADER_GRIP_RADIUS, 2*LEADER_GRIP_RADIUS );
      relPositionX += LEADER_GRIP_RADIUS;
      relPositionY += LEADER_GRIP_RADIUS;
      relLeaderPositionX += LEADER_GRIP_RADIUS;
      relLeaderPositionY += LEADER_GRIP_RADIUS;

      return textRect;
   }
   else
   {
      // add some safety offset to the right (needed for some fonts)
      textRect.ResizeBy( offset, 0 );
      return textRect;
   }
}

// ----------------------------------------------------------------------------

Rect AnnotationRenderer::GetTextRect( const AnnotationInstance& instance, Graphics& g,
      int posX, int posY )
{
   // create the font
   Font f = GetAnnotationFont( instance, 1.0 );

   // create text placement rectangle
   Rect textRect( f.Width( instance.annotationText ) + 1, f.Height() + 1 );
   textRect.MoveBy( posX, posY );

   return textRect;
}

// ----------------------------------------------------------------------------

void AnnotationRenderer::RenderAnnotation(
   const AnnotationInstance& instance, Graphics& g,
   int posX, int posY, int leaderPosX, int leaderPosY)
{
   // create text placement rectangle
   Rect textRect = GetTextRect( instance, g, posX, posY );

   // create the font
   Font f = GetAnnotationFont( instance, 1.0 );

   // set up font to Graphics object
   g.SetFont(f);

   // render shadow if required
   if ( instance.annotationFontShadow )
   {
      // set black pen
	   g.SetPen( RGBAColor( 0, 0, 0 ) );

      // render shifted black text and leader
      RenderTextAndLeader( instance, g, textRect, leaderPosX, leaderPosY, 1, 1 );
   }

   // set pen with actual text color
   g.SetPen( Pen( instance.annotationColor ) );

   // render final text and leader
   RenderTextAndLeader( instance, g, textRect, leaderPosX, leaderPosY, 0, 0 );
}

// ----------------------------------------------------------------------------

void AnnotationRenderer::RenderGrips(
   const AnnotationInstance& instance, Graphics& g,
   int posX, int posY, int leaderPosX, int leaderPosY)
{
   // create text placement rectangle
   Rect textRect = GetTextRect( instance, g, posX, posY );

   // use text color, but make it semitransparent
   RGBA rectColor = RGBAColor(
       Red( instance.annotationColor ),
       Green( instance.annotationColor ),
       Blue( instance.annotationColor ),
       96 );

   // set pen to this color and dashed line
   g.SetPen( Pen( rectColor, 0, PenStyle::Dash ));

   // draw text grip rectangle
   g.DrawRect( textRect );

   // if leader is shown, draw rectangle at its endpoint
   if ( instance.annotationShowLeader )
   {
      // draw the leader grip rectangle
      g.DrawRect( leaderPosX - LEADER_GRIP_RADIUS, leaderPosY - LEADER_GRIP_RADIUS,
         leaderPosX + LEADER_GRIP_RADIUS, leaderPosY + LEADER_GRIP_RADIUS );
   }
}

// ----------------------------------------------------------------------------

Bitmap AnnotationRenderer::CreateAnnotationBitmap( const AnnotationInstance& instance,
      int &relPosX, int &relPosY,
      bool renderGrips )
{
   // get annotation font
      Font f = GetAnnotationFont( instance );

      // get bitmap size and relative positions of text and leader in the bitmap
      int relLeaderPosX = 0, relLeaderPosY = 0;

      Rect bitmapRect = GetBoundingRectangle( instance,
         relPosX, relPosY, relLeaderPosX, relLeaderPosY );

      // create transparent bitmap
      Bitmap annotationBmp( bitmapRect.Width(),
                            bitmapRect.Height(),
                            BitmapFormat::ARGB32 );
      annotationBmp.Fill( RGBAColor( 0, 0, 0, 0 ) );

      // render annotation to the bitmap
      Graphics g;
      g.BeginPaint( annotationBmp );
      RenderAnnotation( instance, g,
         relPosX, relPosY, relLeaderPosX, relLeaderPosY);

      // multiply alpha channel with opacity
      uint32 opacity = instance.annotationOpacity;
      for (int y = 0; y < bitmapRect.Height(); y++)
      {
         RGBA* scanline = annotationBmp.ScanLine(y);
         for (int x = 0; x < bitmapRect.Width(); x++)
         {
            uint32 alpha = scanline[x] >> 24;
            alpha = (alpha * opacity) >> 8;
            scanline[x] = (scanline[x] & 0xFFFFFF) | (alpha << 24);
         }
      }

      // render grips if required (we do not want opacity to be applied to them)
      if ( renderGrips )
         RenderGrips( instance, g, relPosX, relPosY, relLeaderPosX, relLeaderPosY );

      g.EndPaint();

      return annotationBmp;
}

// ----------------------------------------------------------------------------

void AnnotationRenderer::RenderTextAndLeader( const AnnotationInstance& instance, Graphics& g,
      const Rect& textRect, int leaderPosX, int leaderPosY, int offsetX, int offsetY )
{
   // get not-offset text position
   int posX = textRect.x0;
   int posY = textRect.y0;

   // move text rect by offset
   Rect r( textRect );
   r.MoveBy( offsetX, offsetY );

   // render the annotation text
   g.DrawTextRect( r, instance.annotationText );

   // render leader if needed
   if ( instance.annotationShowLeader )
   {
      g.EnableAntialiasing();

      // offset line start by width of one letter
      int offset = g.GetFont().Width( "M" );

      // if leader end is to the left of the text, start at the text beginning
      if ( leaderPosX < posX + textRect.Width() / 2 )
      {
         // leader horizontal line
         g.DrawLine( posX - 1 + offsetX, posY + textRect.Height() / 2 + offsetY,
                     posX - 1 - offset + offsetX, posY + textRect.Height() / 2 + offsetY );

         // leader line to the endpoint
         g.DrawLine( posX - 1 - offset + offsetX, posY + textRect.Height() / 2 + offsetY,
                     leaderPosX + offsetX, leaderPosY + offsetY );
      }
      // if leader end is to the right of the text, start at the text ending
      else
      {
         // leader horizontal line
         g.DrawLine( posX + textRect.Width() + 1 + offsetX, posY + textRect.Height() / 2 + offsetY,
                     posX + textRect.Width() + offset + 1 + offsetX, posY + textRect.Height() / 2 + offsetY);

         // leader line to the endpoint
         g.DrawLine( posX + textRect.Width() + offset + 1 + offsetX, posY + textRect.Height() / 2 + offsetY,
                     leaderPosX + offsetX, leaderPosY + offsetY );
      }

      g.DisableAntialiasing();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AnnotationRenderer.cpp - Released 2018-11-23T18:45:59Z
