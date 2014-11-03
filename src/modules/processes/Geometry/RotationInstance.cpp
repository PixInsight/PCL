// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Geometry Process Module Version 01.01.00.0245
// ****************************************************************************
// RotationInstance.cpp - Released 2014/10/29 07:34:55 UTC
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

#include "RotationInstance.h"
#include "RotationParameters.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/FastRotation.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/Rotation.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RotationInstance::RotationInstance( const MetaProcess* P ) :
ProcessImplementation( P ),
p_angle( TheRotationAngleRotationParameter->DefaultValue() ),
p_optimizeFast( TheOptimizeFastRotationsRotationParameter->DefaultValue() ),
p_interpolation( TheInterpolationAlgorithmRotationParameter->Default ),
p_clampingThreshold( TheClampingThresholdRotationParameter->DefaultValue() ),
p_smoothness( TheSmoothnessRotationParameter->DefaultValue() ),
p_fillColor( 4 )
{
   p_fillColor[0] = TheFillRedRotationParameter->DefaultValue();
   p_fillColor[1] = TheFillGreenRotationParameter->DefaultValue();
   p_fillColor[2] = TheFillBlueRotationParameter->DefaultValue();
   p_fillColor[3] = TheFillAlphaRotationParameter->DefaultValue();
}

// ----------------------------------------------------------------------------

RotationInstance::RotationInstance( const RotationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void RotationInstance::Assign( const ProcessImplementation& p )
{
   const RotationInstance* x = dynamic_cast<const RotationInstance*>( &p );
   if ( x != 0 )
   {
      p_angle = x->p_angle;
      p_optimizeFast = x->p_optimizeFast;
      p_interpolation = x->p_interpolation;
      p_clampingThreshold = x->p_clampingThreshold;
      p_smoothness = x->p_smoothness;
      p_fillColor = x->p_fillColor;
   }
}

// ----------------------------------------------------------------------------

bool RotationInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "Rotation cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool RotationInstance::BeforeExecution( View& view )
{
   ImageWindow w = view.Window();

   if ( w.HasPreviews() || w.HasMaskReferences() || !w.Mask().IsNull() )
      if ( MessageBox( view.Id() + ":\n"
                       "Existing previews and mask references will be destroyed.\n"
                       "Some of these side effects could be irreversible. Proceed?",
                       "Rotation", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
      {
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

void RotationInstance::GetNewSizes( int& width, int& height ) const
{
   BilinearPixelInterpolation I;
   Rotation R( I, p_angle );
   R.GetNewSizes( width, height );
}

// ----------------------------------------------------------------------------

bool RotationInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;  // should never reach this point!

   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   double degrees = Round( Deg( p_angle ), 4 );
   if ( degrees == 0 )
   {
      Console().WriteLn( "<end><cbr>&lt;* Identity *&gt;" );
      return true;
   }

   ImageWindow window = view.Window();

   window.RemoveMaskReferences();
   window.RemoveMask();
   window.DeletePreviews();

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   if ( p_optimizeFast )
      switch ( TruncI( degrees ) )
      {
      case 90:
         Rotate90CCW() >> image;
         return true;
      case -90:
         Rotate90CW() >> image;
         return true;
      case 180:
      case -180:
         Rotate180() >> image;
         return true;
      default:
         break;
      }

   AutoPointer<PixelInterpolation> interpolation( NewInterpolation(
         p_interpolation, 1, 1, 1, 1, true, p_clampingThreshold, p_smoothness, image ) );

   Rotation T( *interpolation, p_angle );

   /*
    * On 32-bit systems, make sure the resulting image requires less than 4 GB.
    */
   if ( sizeof( void* ) == sizeof( uint32 ) )
   {
      int width = image.Width(), height = image.Height();
      T.GetNewSizes( width, height );
      uint64 sz = uint64( width )*uint64( height )*image.NumberOfChannels()*image.BytesPerSample();
      if ( sz > uint64( uint32_max-256 ) )
         throw Error( "Rotation: Invalid operation: Target image dimensions would exceed four gigabytes" );
   }

   T.SetFillValues( p_fillColor );
   T >> image;

   return true;
}

// ----------------------------------------------------------------------------

void* RotationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheRotationAngleRotationParameter )
      return &p_angle;
   if ( p == TheOptimizeFastRotationsRotationParameter )
      return &p_optimizeFast;
   if ( p == TheInterpolationAlgorithmRotationParameter )
      return &p_interpolation;
   if ( p == TheClampingThresholdRotationParameter )
      return &p_clampingThreshold;
   if ( p == TheSmoothnessRotationParameter )
      return &p_smoothness;
   if ( p == TheFillRedRotationParameter )
      return p_fillColor.At( 0 );
   if ( p == TheFillGreenRotationParameter )
      return p_fillColor.At( 1 );
   if ( p == TheFillBlueRotationParameter )
      return p_fillColor.At( 2 );
   if ( p == TheFillAlphaRotationParameter )
      return p_fillColor.At( 3 );
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF RotationInstance.cpp - Released 2014/10/29 07:34:55 UTC
