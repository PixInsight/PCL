//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0405
// ----------------------------------------------------------------------------
// RescaleInstance.cpp - Released 2017-08-01T14:26:58Z
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

#include "RescaleInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RescaleInstance::RescaleInstance( const MetaProcess* m, int r ) : ProcessImplementation( m ), mode( r )
{
}

RescaleInstance::RescaleInstance( const RescaleInstance& x ) : ProcessImplementation( x )
{
   Assign( x );
}

void RescaleInstance::Assign( const ProcessImplementation& p )
{
   const RescaleInstance* x = dynamic_cast<const RescaleInstance*>( &p );
   if ( x != 0 )
      mode = x->mode;
}

bool RescaleInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Rescale cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool RescaleInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   switch ( mode )
   {
   default:
   case RescalingMode::RGBK:
      image->SelectNominalChannels();
      image.Rescale();
      break;
   case RescalingMode::RGBK_Individual:
      for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
      {
         image->SelectChannel( c );
         image.Rescale();
      }
      break;
   case RescalingMode::CIEL:
      {
         ImageVariant L;
         image.GetLightness( L );
         L.Rescale();
         image.SetLightness( L );
      }
      break;
   case RescalingMode::CIEY:
      {
         ImageVariant Y;
         image.GetLuminance( Y );
         Y.Rescale();
         image.SetLuminance( Y );
      }
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* RescaleInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheRescalingModeParameter )
      return &mode;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RescaleInstance.cpp - Released 2017-08-01T14:26:58Z
