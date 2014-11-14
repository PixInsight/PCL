// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Geometry Process Module Version 01.01.00.0247
// ****************************************************************************
// FastRotationInstance.cpp - Released 2014/11/14 17:18:46 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
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

#include "FastRotationInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/FastRotation.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FastRotationInstance::FastRotationInstance( const MetaProcess* m, int r ) :
ProcessImplementation( m ), p_mode( r )
{
}

FastRotationInstance::FastRotationInstance( const FastRotationInstance& x ) :
ProcessImplementation( x ), p_mode( x.p_mode )
{
}

void FastRotationInstance::Assign( const ProcessImplementation& p )
{
   const FastRotationInstance* x = dynamic_cast<const FastRotationInstance*>( &p );
   if ( x != 0 )
      p_mode = x->p_mode;
}

// ----------------------------------------------------------------------------

bool FastRotationInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "FastRotation cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool FastRotationInstance::BeforeExecution( View& view )
{
   ImageWindow window = view.Window();

   if ( window.HasPreviews() )
      if ( MessageBox( view.Id() + ":\n"
                       "Existing previews will be destroyed.\n"
                       "This might lead to irreversible data losses. Proceed?",
                       "FastRotation", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
      {
         return false;
      }

   if ( (window.HasMaskReferences() || !window.Mask().IsNull()) &&
        (p_mode == FastRotationMode::Rotate90CW || p_mode == FastRotationMode::Rotate90CCW) )
   {
      ImageVariant image = window.MainView().Image();
      if ( image.Width() != image.Height() )
         if ( MessageBox( view.Id() + ":\n"
                         "Existing mask references will be invalidated. Proceed?",
                         "FastRotation", // caption
                         StdIcon::Warning,
                         StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
         {
            return false;
         }
   }

   return true;
}

// ----------------------------------------------------------------------------

bool FastRotationInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;  // should not happen!

   AutoViewLock lock( view );

   ImageWindow window = view.Window();
   ImageVariant image = view.Image();

   if ( p_mode == FastRotationMode::Rotate90CW || p_mode == FastRotationMode::Rotate90CCW )
      if ( image.Width() != image.Height() )
      {
         window.RemoveMaskReferences();
         window.RemoveMask();
      }

   window.DeletePreviews();

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   switch ( p_mode )
   {
   default:
   case FastRotationMode::Rotate180:
      Rotate180() >> image;
      break;
   case FastRotationMode::Rotate90CW:
      Rotate90CW() >> image;
      break;
   case FastRotationMode::Rotate90CCW:
      Rotate90CCW() >> image;
      break;
   case FastRotationMode::HorizontalMirror:
      HorizontalMirror() >> image;
      break;
   case FastRotationMode::VerticalMirror:
      VerticalMirror() >> image;
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* FastRotationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheFastRotationModeParameter )
      return &p_mode;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF FastRotationInstance.cpp - Released 2014/11/14 17:18:46 UTC
