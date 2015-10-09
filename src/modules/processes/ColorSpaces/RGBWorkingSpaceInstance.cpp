//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0258
// ----------------------------------------------------------------------------
// RGBWorkingSpaceInstance.cpp - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "RGBWorkingSpaceInstance.h"
#include "RGBWorkingSpaceProcess.h"
#include "RGBWorkingSpaceParameters.h"

#include <pcl/RGBColorSystem.h>
#include <pcl/View.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RGBWorkingSpaceInstance::RGBWorkingSpaceInstance( const MetaProcess* P ) :
ProcessImplementation( P ),
Y( RGBColorSystem::sRGB.LuminanceCoefficients() ),
x( RGBColorSystem::sRGB.ChromaticityXCoordinates() ),
y( RGBColorSystem::sRGB.ChromaticityYCoordinates() ),
gamma( RGBColorSystem::sRGB.Gamma() ),
sRGB( true ),
applyGlobalRGBWS( false )
{
}

RGBWorkingSpaceInstance::RGBWorkingSpaceInstance( const RGBWorkingSpaceInstance& p ) :
ProcessImplementation( p )
{
   Assign( p );
}

RGBWorkingSpaceInstance::RGBWorkingSpaceInstance( const MetaProcess* P, const RGBColorSystem& rgbws ) :
ProcessImplementation( P ),
Y( rgbws.LuminanceCoefficients() ),
x( rgbws.ChromaticityXCoordinates() ),
y( rgbws.ChromaticityYCoordinates() ),
gamma( rgbws.Gamma() ),
sRGB( rgbws.IsSRGB() ),
applyGlobalRGBWS( false )
{
}

bool RGBWorkingSpaceInstance::Validate( String& info )
{
   try
   {
      RGBWorkingSpaceInstance t( *this );
      t.NormalizeLuminanceCoefficients();

      RGBColorSystem rgbws( t.gamma, t.sRGB != 0, t.x, t.y, t.Y );

      info.Clear();
      return true;
   }
   catch ( Exception& e )
   {
      info = e.Message();
      return false;
   }
}

void RGBWorkingSpaceInstance::Assign( const ProcessImplementation& p )
{
   const RGBWorkingSpaceInstance* t = dynamic_cast<const RGBWorkingSpaceInstance*>( &p );
   if ( t != 0 )
   {
      Y = t->Y;
      x = t->x;
      y = t->y;
      gamma = t->gamma;
      sRGB = t->sRGB;
      applyGlobalRGBWS = t->applyGlobalRGBWS;
   }
}

bool RGBWorkingSpaceInstance::IsMaskable( const View&, const ImageWindow& /*mask*/ ) const
{
   return false;
}

UndoFlags RGBWorkingSpaceInstance::UndoMode( const View& ) const
{
   return UndoFlag::RGBWS;
}

bool RGBWorkingSpaceInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "RGBWorkingSpace cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool RGBWorkingSpaceInstance::ExecuteOn( View& view )
{
   RGBWorkingSpaceInstance t( *this );
   t.NormalizeLuminanceCoefficients();

   RGBColorSystem rgbws( t.gamma, t.sRGB != 0, t.x, t.y, t.Y );

   view.Window().SetRGBWS( rgbws );
   return true;
}

bool RGBWorkingSpaceInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

bool RGBWorkingSpaceInstance::ExecuteGlobal()
{
   RGBWorkingSpaceInstance t( *this );
   t.NormalizeLuminanceCoefficients();

   RGBColorSystem rgbws( t.gamma, t.sRGB != 0, t.x, t.y, t.Y );

   ImageWindow::SetGlobalRGBWS( rgbws );
   return true;
}

void* RGBWorkingSpaceInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheRGBWSLuminanceCoefficientParameter )
      return Y.Begin() + tableRow;
   if ( p == TheRGBWSChromaticityXParameter )
      return x.Begin() + tableRow;
   if ( p == TheRGBWSChromaticityYParameter )
      return y.Begin() + tableRow;
   if ( p == TheRGBWSGammaParameter )
      return &gamma;
   if ( p == TheRGBWSsRGBGParameter )
      return &sRGB;
   if ( p == TheRGBWSApplyGlobalParameter )
      return &applyGlobalRGBWS;
   return 0;
}

bool RGBWorkingSpaceInstance::AllocateParameter( size_type length, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheRGBWSChannelTableParameter )
      return true;
   return false;
}

size_type RGBWorkingSpaceInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheRGBWSChannelTableParameter )
      return 3;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceInstance.cpp - Released 2015/10/08 11:24:39 UTC
