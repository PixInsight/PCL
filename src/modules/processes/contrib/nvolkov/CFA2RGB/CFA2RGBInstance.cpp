//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard CFA2RGB Process Module Version 01.01.01.0010
// ----------------------------------------------------------------------------
// CFA2RGBInstance.cpp - Released 2016/02/03 00:00:00 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard CFA2RGB PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "CFA2RGBInstance.h"
#include "CFA2RGBParameters.h"

#include <pcl/AutoViewLock.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CFA2RGBInstance::CFA2RGBInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_bayerPattern( CFA2RGBBayerPatternParameter::Default )
{
}

CFA2RGBInstance::CFA2RGBInstance( const CFA2RGBInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void CFA2RGBInstance::Assign( const ProcessImplementation& p )
{
   const CFA2RGBInstance* x = dynamic_cast<const CFA2RGBInstance*>( &p );
   if ( x != 0 )
   {
      p_bayerPattern             = x->p_bayerPattern;
   }
}

bool CFA2RGBInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "CFA2RGB cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

template <class P>
void Apply(GenericImage<P>& img, pcl_enum bayerPattern )
{
	bool Rx, Ry, Gx0, Gx1, Bx, By;
    Rx = Ry = Gx0 = Gx1 = Bx = By = 0;
	switch ( bayerPattern )
      {
      case CFA2RGBBayerPatternParameter::RGGB: Gx0=Bx=By=1; break;
      case CFA2RGBBayerPatternParameter::BGGR: Rx=Ry=Gx0=1; break;
      case CFA2RGBBayerPatternParameter::GRBG: Rx=Gx1=1; break;
      case CFA2RGBBayerPatternParameter::GBRG: Ry=Gx1=Bx=1; break;
      }

	for ( int x = 0; x < img.Width(); ++x )
	{
		const bool x1(x&1);
		for ( int y = 0; y < img.Height(); ++y )
		{
			const bool y1(y&1);
			if ( x1!=Rx || y1!=Ry ) img.Pixel(x,y,0) = 0; //R
			if ( y1 ? x1!=Gx1 : x1!=Gx0 ) img.Pixel(x,y,1) = 0; //G
			if ( x1!=Bx || y1!=By ) img.Pixel(x,y,2) = 0; //B
		}
	}
}

bool CFA2RGBInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant source = view.Image();
   if ( source.IsComplexSample() )
      return false;

   source.SetColorSpace(ColorSpace::RGB);

   if ( source.IsFloatSample() )
      switch ( source.BitsPerSample() )
      {
      case 32: Apply( static_cast<Image&>( *source ), p_bayerPattern ); break;
      case 64: Apply( static_cast<DImage&>( *source ), p_bayerPattern ); break;
      }
   else
      switch ( source.BitsPerSample() )
      {
      case  8: Apply( static_cast<UInt8Image&>( *source ), p_bayerPattern ); break;
      case 16: Apply( static_cast<UInt16Image&>( *source ), p_bayerPattern ); break;
      case 32: Apply( static_cast<UInt32Image&>( *source ), p_bayerPattern ); break;
      }

   return true;
}


// ----------------------------------------------------------------------------

void* CFA2RGBInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCFA2RGBBayerPatternParameter )
      return &p_bayerPattern;

   return 0;
}

bool CFA2RGBInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   return false;
}

size_type CFA2RGBInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CFA2RGBInstance.cpp - Released 2016/02/03 00:00:00 UTC
