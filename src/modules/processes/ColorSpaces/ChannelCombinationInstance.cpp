//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0258
// ----------------------------------------------------------------------------
// ChannelCombinationInstance.cpp - Released 2015/10/08 11:24:39 UTC
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

#include "ChannelCombinationInstance.h"
#include "ChannelCombinationParameters.h"
#include "ChannelCombinationProcess.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelCombinationInstance::ChannelCombinationInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   colorSpace( TheColorSpaceIdCombinationParameter->ElementValue( TheColorSpaceIdCombinationParameter->Default ) )
{
   for ( int i = 0; i < 3; ++i )
   {
      channelEnabled[i] = true;
      channelId[i] = String( TheChannelIdCombinationParameter->DefaultValue() );
   }
}

ChannelCombinationInstance::ChannelCombinationInstance( const ChannelCombinationInstance& x ) :
   ProcessImplementation( x ),
   colorSpace( x.colorSpace )
{
   for ( int i = 0; i < 3; ++i )
   {
      channelEnabled[i] = x.channelEnabled[i];
      channelId[i] = x.channelId[i];
   }
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::Validate( pcl::String& info )
{
   int n = 0;
   for ( int i = 0; i < 3; ++i )
   {
      if ( !channelId[i].IsEmpty() && !channelId[i].IsValidIdentifier() )
      {
         info = "Invalid image identifier: " + channelId[i];
         return false;
      }

      if ( channelEnabled[i] )
         ++n;
   }

   if ( n == 0 )
   {
      info = "No channel selected for combination";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

void ChannelCombinationInstance::Assign( const ProcessImplementation& p )
{
   const ChannelCombinationInstance* x = dynamic_cast<const ChannelCombinationInstance*>( &p );
   if ( x != 0 )
   {
      colorSpace = x->colorSpace;

      for ( int i = 0; i < 3; ++i )
      {
         channelEnabled[i] = x->channelEnabled[i];
         channelId[i] = x->channelId[i];
      }
   }
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.Image().IsComplexSample() )
   {
      whyNot = "ChannelCombination cannot be executed on complex images.";
      return false;
   }

   if ( v.Image()->ColorSpace() != ColorSpace::RGB )
   {
      whyNot = "ChannelCombination can only be executed on RGB color images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void FromRGB( int colorSpace,
                     const RGBColorSystem& rgbws,
                     RGBColorSystem::sample& ch0, RGBColorSystem::sample& ch1, RGBColorSystem::sample& ch2,
                     RGBColorSystem::sample r, RGBColorSystem::sample g, RGBColorSystem::sample b )
{
   switch ( colorSpace )
   {
   case ColorSpaceId::CIEXYZ:
      rgbws.RGBToCIEXYZ( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::CIELab:
      rgbws.RGBToCIELab( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::CIELch:
      rgbws.RGBToCIELch( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::HSV:
      rgbws.RGBToHSV( ch0, ch1, ch2, r, g, b );
      break;
   case ColorSpaceId::HSI:
      rgbws.RGBToHSI( ch0, ch1, ch2, r, g, b );
      break;
   }
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void ToRGB( int colorSpace,
                   const RGBColorSystem& rgbws,
                   RGBColorSystem::sample& r, RGBColorSystem::sample& g, RGBColorSystem::sample& b,
                   RGBColorSystem::sample ch0, RGBColorSystem::sample ch1, RGBColorSystem::sample ch2 )
{
   switch ( colorSpace )
   {
   case ColorSpaceId::CIEXYZ:
      rgbws.CIEXYZToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::CIELab:
      rgbws.CIELabToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::CIELch:
      rgbws.CIELchToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::HSV:
      rgbws.HSVToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   case ColorSpaceId::HSI:
      rgbws.HSIToRGB( r, g, b, ch0, ch1, ch2 );
      break;
   }
}

template <class P, class P0, class P1, class P2>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             const GenericImage<P0>* src0, const GenericImage<P1>* src1, const GenericImage<P2>* src2 )
{
   bool allChannels = src0 != 0 && src1 != 0 && src2 != 0;

   typename P::sample* R = img.PixelData( 0 );
   typename P::sample* G = img.PixelData( 1 );
   typename P::sample* B = img.PixelData( 2 );

   const RGBColorSystem& rgbws = img.RGBWorkingSpace();

   for ( int y = r.y0; y < r.y1; ++y )
   {
      const typename P0::sample* data0 = (src0 != 0) ? src0->PixelAddress( r.x0, y ) : 0;
      const typename P1::sample* data1 = (src1 != 0) ? src1->PixelAddress( r.x0, y ) : 0;
      const typename P2::sample* data2 = (src2 != 0) ? src2->PixelAddress( r.x0, y ) : 0;

      for ( int x = r.x0; x < r.x1; ++x, ++img.Status() )
      {
         if ( colorSpace == ColorSpaceId::RGB )
         {
            if ( data0 != 0 )
               P0::FromSample( *R++, *data0++ );
            if ( data1 != 0 )
               P1::FromSample( *G++, *data1++ );
            if ( data2 != 0 )
               P2::FromSample( *B++, *data2++ );
         }
         else
         {
            RGBColorSystem::sample ch0, ch1, ch2;
            RGBColorSystem::sample r, g, b;

            if ( !allChannels )
            {
               P::FromSample( r, *R );
               P::FromSample( g, *G );
               P::FromSample( b, *B );

               FromRGB( colorSpace, rgbws, ch0, ch1, ch2, r, g, b );
            }

            if ( data0 != 0 )
               P0::FromSample( ch0, *data0++ );
            if ( data1 != 0 )
               P1::FromSample( ch1, *data1++ );
            if ( data2 != 0 )
               P2::FromSample( ch2, *data2++ );

            ToRGB( colorSpace, rgbws, r, g, b, ch0, ch1, ch2 );

            *R++ = P::ToSample( r );
            *G++ = P::ToSample( g );
            *B++ = P::ToSample( b );
         }
      }
   }
}

template <class P, class P0, class P1>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             const GenericImage<P0>* src0, const GenericImage<P1>* src1, ImageVariant& src2 )
{
   if ( src2 )
   {
      if ( src2.IsFloatSample() )
         switch ( src2.BitsPerSample() )
         {
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<Image*>( src2.ImagePtr() ) );
            break;
         case 64:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<DImage*>( src2.ImagePtr() ) );
            break;
         }
      else
         switch ( src2.BitsPerSample() )
         {
         case  8:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt8Image*>( src2.ImagePtr() ) );
            break;
         case 16:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt16Image*>( src2.ImagePtr() ) );
            break;
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt32Image*>( src2.ImagePtr() ) );
            break;
         }
   }
   else
      CombineChannels( img, colorSpace, baseId, r, src0, src1, static_cast<UInt8Image*>( nullptr ) );
}

template <class P, class P0>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             const GenericImage<P0>* src0, ImageVariant& src1, ImageVariant& src2 )
{
   if ( src1 )
   {
      if ( src1.IsFloatSample() )
         switch ( src1.BitsPerSample() )
         {
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<Image*>( src1.ImagePtr() ), src2 );
            break;
         case 64:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<DImage*>( src1.ImagePtr() ), src2 );
            break;
         }
      else
         switch ( src1.BitsPerSample() )
         {
         case  8:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt8Image*>( src1.ImagePtr() ), src2 );
            break;
         case 16:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt16Image*>( src1.ImagePtr() ), src2 );
            break;
         case 32:
            CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt32Image*>( src1.ImagePtr() ), src2 );
            break;
         }
   }
   else
      CombineChannels( img, colorSpace, baseId, r, src0, static_cast<UInt8Image*>( nullptr ), src2 );
}

template <class P>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineChannels( GenericImage<P>& img, int colorSpace, const String& baseId,
                             const Rect& r,
                             ImageVariant& src0, ImageVariant& src1, ImageVariant& src2 )
{
   if ( src0 )
   {
      if ( src0.IsFloatSample() )
         switch ( src0.BitsPerSample() )
         {
         case 32:
            CombineChannels( img, colorSpace, baseId, r, static_cast<Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         case 64:
            CombineChannels( img, colorSpace, baseId, r, static_cast<DImage*>( src0.ImagePtr() ), src1, src2 );
            break;
         }
      else
         switch ( src0.BitsPerSample() )
         {
         case  8:
            CombineChannels( img, colorSpace, baseId, r, static_cast<UInt8Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         case 16:
            CombineChannels( img, colorSpace, baseId, r, static_cast<UInt16Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         case 32:
            CombineChannels( img, colorSpace, baseId, r, static_cast<UInt32Image*>( src0.ImagePtr() ), src1, src2 );
            break;
         }
   }
   else
      CombineChannels( img, colorSpace, baseId, r, static_cast<UInt8Image*>( nullptr ), src1, src2 );
}

bool ChannelCombinationInstance::ExecuteOn( View& view )
{
   ImageWindow sourceWindow[ 3 ];
   ImageVariant sourceImage[ 3 ];

   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      throw Error( "ChannelCombination cannot be executed on complex images." );

   if ( image->ColorSpace() != ColorSpace::RGB )
      throw Error( "ChannelCombination requires a RGB color image." );

   Console().EnableAbort();

   StandardStatus status;
   image->SetStatusCallback( &status );

   String baseId;
   Rect r;
   int w0, h0;

   if ( view.IsPreview() )
   {
      ImageWindow w = view.Window();
      View mainView = w.MainView();

      baseId = mainView.Id();
      r = w.PreviewRect( view.Id() );
      mainView.GetSize( w0, h0 );
   }
   else
   {
      baseId = view.Id();
      r = image->Bounds();
      w0 = r.Width();
      h0 = r.Height();
   }

   int numberOfSources = 0;

   for ( int i = 0; i < 3; ++i )
      if ( channelEnabled[i] )
      {
         String id = channelId[i];
         if ( id.IsEmpty() )
            id = baseId + '_' + ColorSpaceId::ChannelId( colorSpace, i );

         sourceWindow[i] = ImageWindow::WindowById( id );

         if ( sourceWindow[i].IsNull() )
            throw Error( "ChannelCombination: Source image not found: " + id );

         sourceImage[i] = sourceWindow[i].MainView().Image();

         if ( !sourceImage[i] )
            throw Error( "ChannelCombination: Invalid source image: " + id );

         if ( sourceImage[i]->IsColor() )
            throw Error( "ChannelCombination: Invalid source color space: " + id );

         if ( sourceImage[i]->Width() != w0 || sourceImage[i]->Height() != h0 )
            throw Error( "ChannelCombination: Incompatible source image dimensions: " + id );

         ++numberOfSources;
      }

   if ( numberOfSources == 0 )
      return false;

   const char* what = "";
   switch ( colorSpace )
   {
   case ColorSpaceId::RGB:
      what = "RGB channels";
      break;
   case ColorSpaceId::CIEXYZ:
      what = "normalized CIE XYZ components";
      break;
   case ColorSpaceId::CIELab:
      what = "normalized CIE L*a*b* components";
      break;
   case ColorSpaceId::CIELch:
      what = "normalized CIE L*c*h* components";
      break;
   case ColorSpaceId::HSV:
      what = "normalized HSV components";
      break;
   case ColorSpaceId::HSI:
      what = "normalized HSI components";
      break;
   }
   image->Status().Initialize( String( "Combining " ) + what, image->NumberOfPixels() );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         CombineChannels( static_cast<Image&>( *image ), colorSpace, baseId, r,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      case 64:
         CombineChannels( static_cast<DImage&>( *image ), colorSpace, baseId, r,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         CombineChannels( static_cast<UInt8Image&>( *image ), colorSpace, baseId, r,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      case 16:
         CombineChannels( static_cast<UInt16Image&>( *image ), colorSpace, baseId, r,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      case 32:
         CombineChannels( static_cast<UInt32Image&>( *image ), colorSpace, baseId, r,
                          sourceImage[0], sourceImage[1], sourceImage[2] );
         break;
      }

   return true;
}

// ----------------------------------------------------------------------------

bool ChannelCombinationInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

bool ChannelCombinationInstance::ExecuteGlobal()
{
   ImageWindow sourceWindow[ 3 ];
   ImageVariant sourceImage[ 3 ];

   int numberOfSources = 0;

   int width = 0, height = 0;
   bool floatSample = false;
   int bitsPerSample = 0;

   for ( int i = 0; i < 3; ++i )
      if ( channelEnabled[i] && !channelId[i].IsEmpty() )
      {
         sourceWindow[i] = ImageWindow::WindowById( channelId[i] );
         if ( sourceWindow[i].IsNull() )
            throw Error( "ChannelCombination: Source image not found: " + channelId[i] );

         sourceImage[i] = sourceWindow[i].MainView().Image();
         if ( !sourceImage[i] )
            throw Error( "ChannelCombination: Invalid source image: " + channelId[i] );

         if ( sourceImage[i]->IsColor() )
            throw Error( "ChannelCombination: Invalid source color space: " + channelId[i] );

         if ( sourceImage[i].IsFloatSample() )
            floatSample = true;

         if ( sourceImage[i].BitsPerSample() > bitsPerSample )
            bitsPerSample = sourceImage[i].BitsPerSample();

         if ( width == 0 )
         {
            width = sourceImage[i]->Width();
            height = sourceImage[i]->Height();
         }
         else
         {
            if ( sourceImage[i]->Width() != width || sourceImage[i]->Height() != height )
               throw Error( "ChannelCombination: Incompatible source image dimensions: " + channelId[i] );
         }

         ++numberOfSources;
      }

   if ( numberOfSources == 0 )
      throw Error( "ChannelCombination: No source image(s)." );

   ImageWindow w( width, height, 3, bitsPerSample, floatSample, true, true );
   if ( w.IsNull() )
      throw Error( "ChannelCombination: Unable to create target image." );

   View mainView = w.MainView();

   AutoViewLock lock( mainView );

   try
   {
      ImageVariant image = mainView.Image();

      Console().EnableAbort();

      StandardStatus status;
      image->SetStatusCallback( &status );

      const char* what = "";
      switch ( colorSpace )
      {
      case ColorSpaceId::RGB:
         what = "RGB channels";
         break;
      case ColorSpaceId::CIEXYZ:
         what = "normalized CIE XYZ components";
         break;
      case ColorSpaceId::CIELab:
         what = "normalized CIE L*a*b* components";
         break;
      case ColorSpaceId::CIELch:
         what = "normalized CIE L*c*h* components";
         break;
      case ColorSpaceId::HSV:
         what = "normalized HSV components";
         break;
      case ColorSpaceId::HSI:
         what = "normalized HSI components";
         break;
      }
      image->Status().Initialize( String( "Combining " ) + what, image->NumberOfPixels() );

      String baseId = mainView.Id();
      Rect r = image->Bounds();

      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            CombineChannels( static_cast<Image&>( *image ), colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         case 64:
            CombineChannels( static_cast<DImage&>( *image ), colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8:
            CombineChannels( static_cast<UInt8Image&>( *image ), colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         case 16:
            CombineChannels( static_cast<UInt16Image&>( *image ), colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         case 32:
            CombineChannels( static_cast<UInt32Image&>( *image ), colorSpace, baseId, r,
                             sourceImage[0], sourceImage[1], sourceImage[2] );
            break;
         }

      w.Show();

      return true;
   }
   catch ( ... )
   {
      w.Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* ChannelCombinationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheColorSpaceIdCombinationParameter )
      return &colorSpace;
   if ( p == TheChannelEnabledCombinationParameter )
      return channelEnabled+tableRow;
   if ( p == TheChannelIdCombinationParameter )
      return channelId[tableRow].c_str();
   return 0;
}

bool ChannelCombinationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheChannelIdCombinationParameter )
   {
      channelId[tableRow].Clear();
      if ( sizeOrLength > 0 )
         channelId[tableRow].SetLength( sizeOrLength );
      return true;
   }

   if ( p == TheChannelTableCombinationParameter )
      return true;

   return false;
}

size_type ChannelCombinationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheChannelIdCombinationParameter )
      return channelId[tableRow].Length();
   if ( p == TheChannelTableCombinationParameter )
      return 3;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelCombinationInstance.cpp - Released 2015/10/08 11:24:39 UTC
