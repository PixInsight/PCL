//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 01.00.00.0164
// ----------------------------------------------------------------------------
// AnnotationInstance.cpp - Released 2016/02/21 20:22:43 UTC
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

#include "AnnotationInstance.h"
#include "AnnotationParameters.h"
#include "AnnotationRenderer.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/Graphics.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationInstance::AnnotationInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
annotationText( TheAnnotationTextParameter->DefaultValue() ),
annotationFont( TheAnnotationFontParameter->DefaultValue() ),
annotationFontSize( TheAnnotationFontSizeParameter->DefaultValue() ),
annotationFontBold( TheAnnotationFontBoldParameter->DefaultValue() ),
annotationFontItalic( TheAnnotationFontItalicParameter->DefaultValue() ),
annotationFontUnderline( TheAnnotationFontUnderlineParameter->DefaultValue() ),
annotationFontShadow( TheAnnotationFontShadowParameter->DefaultValue() ),
annotationColor( TheAnnotationColorParameter->DefaultValue() ),
annotationPositionX( TheAnnotationPositionXParameter->DefaultValue() ),
annotationPositionY( TheAnnotationPositionYParameter->DefaultValue() ),
annotationShowLeader( TheAnnotationShowLeaderParameter->DefaultValue() ),
annotationLeaderPositionX( TheAnnotationLeaderPositionXParameter->DefaultValue() ),
annotationLeaderPositionY( TheAnnotationLeaderPositionYParameter->DefaultValue() ),
annotationOpacity( TheAnnotationOpacityParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

AnnotationInstance::AnnotationInstance( const AnnotationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void AnnotationInstance::Assign( const ProcessImplementation& p )
{
   const AnnotationInstance* x = dynamic_cast<const AnnotationInstance*>( &p );
   if ( x != 0 )
   {
      annotationText = x->annotationText;
      annotationFont = x->annotationFont;
      annotationFontSize = x->annotationFontSize;
      annotationFontBold = x->annotationFontBold;
      annotationFontItalic = x->annotationFontItalic;
      annotationFontUnderline = x->annotationFontUnderline;
      annotationFontShadow = x->annotationFontShadow;
      annotationColor = x->annotationColor;
      annotationPositionX = x->annotationPositionX;
      annotationPositionY = x->annotationPositionY;
      annotationShowLeader = x->annotationShowLeader;
      annotationLeaderPositionX = x->annotationLeaderPositionX;
      annotationLeaderPositionY = x->annotationLeaderPositionY;
      annotationOpacity = x->annotationOpacity;
   }
}

// ----------------------------------------------------------------------------

bool AnnotationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Annotation cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class AnnotationEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const AnnotationInstance& instance )
   {
      int relPosX = 0, relPosY = 0;
      Bitmap annotationBmp = AnnotationRenderer::CreateAnnotationBitmap( instance, relPosX, relPosY, false );
      // blend bitmap to the image
      image.Blend( annotationBmp, Point( instance.annotationPositionX - relPosX,
                                         instance.annotationPositionY - relPosY ) );
   }
};

// ----------------------------------------------------------------------------

bool AnnotationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( !image.IsComplexSample() )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
      {
         case 32: AnnotationEngine::Apply( static_cast<pcl::Image&>( *image ), *this ); break;
         case 64: AnnotationEngine::Apply( static_cast<pcl::DImage&>( *image ), *this ); break;
      }
      else
         switch ( image.BitsPerSample() )
      {
         case  8: AnnotationEngine::Apply( static_cast<pcl::UInt8Image&>( *image ), *this ); break;
         case 16: AnnotationEngine::Apply( static_cast<pcl::UInt16Image&>( *image ), *this ); break;
         case 32: AnnotationEngine::Apply( static_cast<pcl::UInt32Image&>( *image ), *this ); break;
      }

   return true;
}

// ----------------------------------------------------------------------------

void* AnnotationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheAnnotationTextParameter )
      return annotationText.Begin();
   if ( p == TheAnnotationFontParameter )
      return annotationFont.Begin();
   if ( p == TheAnnotationFontSizeParameter )
      return &annotationFontSize;
   if ( p == TheAnnotationFontBoldParameter )
      return &annotationFontBold;
   if ( p == TheAnnotationFontItalicParameter )
      return &annotationFontItalic;
   if ( p == TheAnnotationFontUnderlineParameter )
      return &annotationFontUnderline;
   if ( p == TheAnnotationFontShadowParameter )
      return &annotationFontShadow;
   if ( p == TheAnnotationColorParameter )
      return &annotationColor;
   if ( p == TheAnnotationPositionXParameter )
      return &annotationPositionX;
   if ( p == TheAnnotationPositionYParameter )
      return &annotationPositionY;
   if ( p == TheAnnotationShowLeaderParameter )
      return &annotationShowLeader;
   if ( p == TheAnnotationLeaderPositionXParameter )
      return &annotationLeaderPositionX;
   if ( p == TheAnnotationLeaderPositionYParameter )
      return &annotationLeaderPositionY;
   if ( p == TheAnnotationOpacityParameter )
      return &annotationOpacity;
   return 0;
}

// ----------------------------------------------------------------------------

bool AnnotationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheAnnotationTextParameter )
   {
      annotationText.Clear();
      if ( sizeOrLength > 0 )
         annotationText.SetLength( sizeOrLength );
   }
   else if ( p == TheAnnotationFontParameter )
   {
      annotationFont.Clear();
      if ( sizeOrLength > 0 )
         annotationFont.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type AnnotationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheAnnotationTextParameter )
      return annotationText.Length();
   if ( p == TheAnnotationFontParameter )
      return annotationFont.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AnnotationInstance.cpp - Released 2016/02/21 20:22:43 UTC
