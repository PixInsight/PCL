//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0371
// ----------------------------------------------------------------------------
// NewImageInstance.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "NewImageInstance.h"

#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

NewImageInstance::NewImageInstance( const MetaProcess* p ) :
   ProcessImplementation( p )
{
   width = height = 256;
   numberOfChannels = 3;
   colorSpace = NewImageColorSpace::RGB;
   sampleFormat = NewImageSampleFormat::F32;
   v0 = v1 = v2 = 0;
   va = 1;
}

// ----------------------------------------------------------------------------

NewImageInstance::NewImageInstance( const NewImageInstance& x ) :
   ProcessImplementation( x ),
   id( x.id ),
   width( x.width ), height( x.height ), numberOfChannels( x.numberOfChannels ),
   colorSpace( x.colorSpace ),
   sampleFormat( x.sampleFormat ),
   v0( x.v0 ), v1( x.v1 ), v2( x.v2 ), va( x.va )
{
}

// ----------------------------------------------------------------------------

bool NewImageInstance::Validate( pcl::String& info )
{
   if ( !id.IsEmpty() && !id.IsValidIdentifier() )
   {
      info = "Invalid image identifier: " + id;
      return false;
   }

   if ( width < 1 || height < 1 )
   {
      info = "Invalid image dimensions";
      return false;
   }

   if ( colorSpace != NewImageColorSpace::RGB && colorSpace != NewImageColorSpace::Gray )
   {
      info = "Invalid color space";
      return false;
   }

   if ( numberOfChannels < ((colorSpace == NewImageColorSpace::RGB) ? 3 : 1) )
   {
      info = "Insufficient number of channels";
      return false;
   }

   if ( sampleFormat != NewImageSampleFormat::I8 &&
        sampleFormat != NewImageSampleFormat::I16 &&
        sampleFormat != NewImageSampleFormat::I32 &&
        sampleFormat != NewImageSampleFormat::F32 &&
        sampleFormat != NewImageSampleFormat::F64 )
   {
      info = "Invalid sample format";
      return false;
   }

   if ( sizeof( void* ) == 4 )
   {
      int sampleSize = (sampleFormat == NewImageSampleFormat::I8) ? 1 :
                           ((sampleFormat == NewImageSampleFormat::I16) ? 2 :
                              ((sampleFormat == NewImageSampleFormat::F64) ? 8 : 4));

      uint64 totalSize = uint64( width )*uint64( height )*sampleSize*numberOfChannels;
      if ( totalSize > uint64( uint32_max )+1 )
      {
         info = "The requested image dimensions would exceed four gigabytes";
         return false;
      }
   }

   if ( v0 < 0 || v0 > 1 || v1 < 0 || v1 > 1 || v2 < 0 || v2 > 1 || va < 0 || va > 1 )
   {
      info = "Invalid initial value(s)";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

void NewImageInstance::Assign( const ProcessImplementation& p )
{
   const NewImageInstance* x = dynamic_cast<const NewImageInstance*>( &p );
   if ( x != 0 )
   {
      id               = x->id;
      width            = x->width;
      height           = x->height;
      numberOfChannels = x->numberOfChannels;
      colorSpace       = x->colorSpace;
      sampleFormat     = x->sampleFormat;
      v0               = x->v0;
      v1               = x->v1;
      v2               = x->v2;
      va               = x->va;
   }
}

// ----------------------------------------------------------------------------

bool NewImageInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "NewImage can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool NewImageInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

template <class P>
static void FillImage( GenericImage<P>& image, double v0, double v1, double v2, double va )
{
   // First channel always exists
   image.SelectChannel( 0 );
   image.Fill( v0 );

   // 2nd and 3rd channels for color images only
   if ( image.IsColor() )
   {
      image.SelectChannel( 1 );
      image.Fill( v1 );
      image.SelectChannel( 2 );
      image.Fill( v2 );
   }

   // Alpha channels
   if ( image.HasAlphaChannels() )
   {
      int c = image.NumberOfNominalChannels();

      // 1st alpha channel
      image.SelectChannel( c );
      image.Fill( va );

      // Initialize the rest of alpha channels to black
      for ( ; ++c < image.NumberOfChannels(); )
      {
         image.SelectChannel( c );
         image.Zero();
      }
   }
}

bool NewImageInstance::ExecuteGlobal()
{
#define FILL( I ) FillImage( static_cast<pcl::I&>( *image ), v0, v1, v2, va )

   int bitsPerSample;
   bool floatSample;

   switch ( sampleFormat )
   {
   case NewImageSampleFormat::I8:  bitsPerSample =  8; floatSample = false; break;
   case NewImageSampleFormat::I16: bitsPerSample = 16; floatSample = false; break;
   case NewImageSampleFormat::I32: bitsPerSample = 32; floatSample = false; break;
   default:
   case NewImageSampleFormat::F32: bitsPerSample = 32; floatSample = true; break;
   case NewImageSampleFormat::F64: bitsPerSample = 64; floatSample = true; break;
   }

   ImageWindow w( width, height, numberOfChannels,
                  bitsPerSample, floatSample,
                  colorSpace == NewImageColorSpace::RGB, true, id );
   if ( !w.IsNull() )
   {
      ImageVariant image = w.MainView().Image();
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: FILL( Image ); break;
         case 64: FILL( DImage ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: FILL( UInt8Image ); break;
         case 16: FILL( UInt16Image ); break;
         case 32: FILL( UInt32Image ); break;
         }

      w.Show();
      return true;
   }

   return false;

#undef FILL
}

// ----------------------------------------------------------------------------

void* NewImageInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheNewImageIdentifierParameter )
      return id.Begin();
   if ( p == TheNewImageWidthParameter )
      return &width;
   if ( p == TheNewImageHeightParameter )
      return &height;
   if ( p == TheNewImageNumberOfChannelsParameter )
      return &numberOfChannels;
   if ( p == TheNewImageColorSpaceParameter )
      return &colorSpace;
   if ( p == TheNewImageSampleFormatParameter )
      return &sampleFormat;
   if ( p == TheNewImageV0Parameter )
      return &v0;
   if ( p == TheNewImageV1Parameter )
      return &v1;
   if ( p == TheNewImageV2Parameter )
      return &v2;
   if ( p == TheNewImageVAParameter )
      return &va;
   return 0;
}

// ----------------------------------------------------------------------------

bool NewImageInstance::ValidateParameter( void* value, const MetaParameter* p, size_type /*tableRow*/ ) const
{
#define i32 *reinterpret_cast<int32*>( value )
#define dbl *reinterpret_cast<double*>( value )
#define str  reinterpret_cast<char*>( value )

   if ( p == TheNewImageIdentifierParameter )
   {
      pcl::String vId( str );
      vId.Trim();

      if ( !vId.IsEmpty() && !vId.IsValidIdentifier() )
      {
         *str = '\0';
         return false;
      }
   }
   else if ( p == TheNewImageWidthParameter || p == TheNewImageHeightParameter || p == TheNewImageNumberOfChannelsParameter )
   {
      if ( i32 <= 0 )
      {
         i32 = 0;
         return false;
      }
   }
   else if ( p == TheNewImageColorSpaceParameter )
   {
      if ( i32 != NewImageColorSpace::RGB && i32 != NewImageColorSpace::Gray )
      {
         i32 = NewImageColorSpace::RGB;
         return false;
      }
   }
   else if ( p == TheNewImageSampleFormatParameter )
   {
      switch ( i32 )
      {
      case NewImageSampleFormat::I8:
      case NewImageSampleFormat::I16:
      case NewImageSampleFormat::I32:
      case NewImageSampleFormat::F32:
      case NewImageSampleFormat::F64:
         break;
      default:
         i32 = NewImageSampleFormat::F32;
         return false;
      }
   }
   else if ( p == TheNewImageV0Parameter || p == TheNewImageV1Parameter || p == TheNewImageV2Parameter || p == TheNewImageVAParameter )
   {
      if ( dbl < 0 || dbl > 1 )
      {
         dbl = 0;
         return false;
      }
   }

   return true;

#undef i32
#undef dbl
#undef str
}

// ----------------------------------------------------------------------------

bool NewImageInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheNewImageIdentifierParameter )
   {
      id.Clear();
      if ( sizeOrLength > 0 )
         id.SetLength( sizeOrLength );
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

size_type NewImageInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheNewImageIdentifierParameter )
      return id.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NewImageInstance.cpp - Released 2017-05-02T09:43:00Z
