//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0317
// ----------------------------------------------------------------------------
// ChannelExtractionInstance.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "ChannelExtractionInstance.h"
#include "ChannelExtractionParameters.h"
#include "ChannelExtractionProcess.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelExtractionInstance::ChannelExtractionInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_colorSpace( ColorSpaceId::Default ),
   p_sampleFormat( ChannelSampleFormat::Default )
{
   for ( int i = 0; i < 3; ++i )
   {
      p_channelEnabled[i] = true;
      p_channelId[i] = String( TheChannelIdExtractionParameter->DefaultValue() );
   }
}

// ----------------------------------------------------------------------------

ChannelExtractionInstance::ChannelExtractionInstance( const ChannelExtractionInstance& x ) :
   ProcessImplementation( x ),
   p_colorSpace( x.p_colorSpace ),
   p_sampleFormat( x.p_sampleFormat )
{
   for ( int i = 0; i < 3; ++i )
   {
      p_channelEnabled[i] = x.p_channelEnabled[i];
      p_channelId[i] = x.p_channelId[i];
   }
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInstance::Validate( pcl::String& info )
{
   int n = 0;
   for ( int i = 0; i < 3; ++i )
   {
      if ( !p_channelId[i].IsEmpty() && !p_channelId[i].IsValidIdentifier() )
      {
         info = "Invalid image identifier: " + p_channelId[i];
         return false;
      }

      if ( p_channelEnabled[i] )
         ++n;
   }

   if ( n == 0 )
   {
      info = "No channel selected for extraction";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

void ChannelExtractionInstance::Assign( const ProcessImplementation& p )
{
   const ChannelExtractionInstance* x = dynamic_cast<const ChannelExtractionInstance*>( &p );
   if ( x != nullptr )
   {
      p_colorSpace = x->p_colorSpace;
      p_sampleFormat = x->p_sampleFormat;

      for ( int i = 0; i < 3; ++i )
      {
         p_channelEnabled[i] = x->p_channelEnabled[i];
         p_channelId[i] = x->p_channelId[i];
      }
   }
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.Image().IsComplexSample() )
   {
      whyNot = "ChannelExtraction cannot be executed on complex images.";
      return false;
   }

   if ( v.Image()->ColorSpace() != ColorSpace::RGB )
   {
      whyNot = "ChannelExtraction can only be executed on RGB color images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

template <class P, class P1>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void ExtractChannels( const GenericImage<P>& img, const View& view,
                       const ChannelExtractionInstance& E, const String& baseId, GenericImage<P1>* )
{
   ImageWindow targetWindow[ 3 ];
   ImageVariant targetImage[ 3 ];

   try
   {
      typename P1::sample* data[ 3 ];
      int numberOfTargets = 0;

      RGBColorSystem rgbws;
      view.Window().GetRGBWS( rgbws );

      double xRes, yRes;
      bool metric;
      view.Window().GetResolution( xRes, yRes, metric );

      FITSKeywordArray keywords = view.Window().Keywords();

      PropertyArray properties = view.GetStorableProperties();

      for ( int i = 0; i < 3; ++i )
         if ( E.IsChannelEnabled( i ) )
         {
            String id = E.ChannelId( i );
            if ( id.IsEmpty() )
               id = baseId + '_' + ColorSpaceId::ChannelId( E.ColorSpace(), i );

            targetWindow[i] = ImageWindow( img.Width(), img.Height(), 1,
                                           P1::BitsPerSample(), P1::IsFloatSample(),
                                           false/*color*/, true/*initialProcessing*/, id );
            if ( targetWindow[i].IsNull() )
               throw Error( "ChannelExtraction: Unable to create target image: " + id );

            targetWindow[i].SetRGBWS( rgbws );
            targetWindow[i].SetResolution( xRes, yRes, metric );
            targetWindow[i].SetKeywords( keywords );
            targetWindow[i].MainView().SetProperties( properties, true/*notify*/, ViewPropertyAttribute::Storable );

            targetImage[i] = targetWindow[i].MainView().Image();
            if ( !targetImage[i] )
               throw Error( "ChannelExtraction: Invalid target image: " + id );

            data[i] = static_cast<GenericImage<P1>&>( *targetImage[i] ).PixelData();
            ++numberOfTargets;
         }

      if ( numberOfTargets == 0 ) // ?!
         return;

      bool isLightness = (E.ColorSpace() == ColorSpaceId::CIELab || E.ColorSpace() == ColorSpaceId::CIELch) &&
                           E.IsChannelEnabled( 0 ) && !E.IsChannelEnabled( 1 ) && !E.IsChannelEnabled( 2 );

      bool isValue = E.ColorSpace() == ColorSpaceId::HSV &&
                           !E.IsChannelEnabled( 0 ) && !E.IsChannelEnabled( 1 ) && E.IsChannelEnabled( 2 );

      bool isIntensity = E.ColorSpace() == ColorSpaceId::HSI &&
                           !E.IsChannelEnabled( 0 ) && !E.IsChannelEnabled( 1 ) && E.IsChannelEnabled( 2 );

      size_type N = img.NumberOfPixels();
      const char* what = "";
      if ( isLightness )
         what = "lightness";
      else if ( isValue )
         what = "value channel";
      else if ( isIntensity )
         what = "intensity channel";
      else
         switch ( E.ColorSpace() )
         {
         case ColorSpaceId::RGB :
           what = "RGB channels";
           break;
         case ColorSpaceId::CIEXYZ :
           what = "normalized CIE XYZ components";
           break;
         case ColorSpaceId::CIELab :
           what = "normalized CIE L*a*b* components";
           break;
         case ColorSpaceId::CIELch :
           what = "normalized CIE L*c*h* components";
           break;
         case ColorSpaceId::HSV :
           what = "normalized HSV components";
           break;
         case ColorSpaceId::HSI :
           what = "normalized HSI components";
           break;
         }
      img.Status().Initialize( String( "Extracting " ) + what, N );

      const typename P::sample* R = img.PixelData( 0 );
      const typename P::sample* G = img.PixelData( 1 );
      const typename P::sample* B = img.PixelData( 2 );
      const typename P::sample* RN = R + N;

      for ( ;; )
      {
         if ( E.ColorSpace() == ColorSpaceId::RGB )
         {
            if ( E.IsChannelEnabled( 0 ) )
               P::FromSample( *data[0], *R );
            if ( E.IsChannelEnabled( 1 ) )
               P::FromSample( *data[1], *G );
            if ( E.IsChannelEnabled( 2 ) )
               P::FromSample( *data[2], *B );
         }
         else
         {
            RGBColorSystem::sample r, g, b;
            P::FromSample( r, *R );
            P::FromSample( g, *G );
            P::FromSample( b, *B );

            RGBColorSystem::sample ch0, ch1, ch2;

            if ( isLightness )
               ch0 = rgbws.Lightness( r, g, b );
            else if ( isValue )
               ch2 = rgbws.Value( r, g, b );
            else if ( isIntensity )
               ch2 = rgbws.Intensity( r, g, b );
            else
               switch ( E.ColorSpace() )
               {
               case ColorSpaceId::CIEXYZ :
                  rgbws.RGBToCIEXYZ( ch0, ch1, ch2, r, g, b );
                  break;
               case ColorSpaceId::CIELab :
                  rgbws.RGBToCIELab( ch0, ch1, ch2, r, g, b );
                  break;
               case ColorSpaceId::CIELch :
                  rgbws.RGBToCIELch( ch0, ch1, ch2, r, g, b );
                  break;
               case ColorSpaceId::HSV :
                  rgbws.RGBToHSV( ch0, ch1, ch2, r, g, b );
                  break;
               case ColorSpaceId::HSI :
                  rgbws.RGBToHSI( ch0, ch1, ch2, r, g, b );
                  break;
               }

            if ( E.IsChannelEnabled( 0 ) )
               *data[0] = P1::ToSample( ch0 );
            if ( E.IsChannelEnabled( 1 ) )
               *data[1] = P1::ToSample( ch1 );
            if ( E.IsChannelEnabled( 2 ) )
               *data[2] = P1::ToSample( ch2 );
         }

         ++img.Status();

         if ( ++R == RN )
            break;

         ++G; ++B; ++data[0]; ++data[1]; ++data[2];
      }

      for ( int i = 0; i < 3; ++i )
         if ( E.IsChannelEnabled( i ) )
            targetWindow[i].Show();
   }
   catch ( ... )
   {
      for ( int i = 0; i < 3; ++i )
         targetWindow[i].Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

template <class P>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void ExtractChannels( const GenericImage<P>& img, const View& view, const ChannelExtractionInstance& E, const String& baseId )
{
   switch ( E.SampleFormat() )
   {
   case ChannelSampleFormat::F32:
      ExtractChannels( img, view, E, baseId, (Image*)( nullptr ) );
      break;
   case ChannelSampleFormat::F64:
      ExtractChannels( img, view, E, baseId, (DImage*)( nullptr ) );
      break;
   case ChannelSampleFormat::I8:
      ExtractChannels( img, view, E, baseId, (UInt8Image*)( nullptr ) );
      break;
   case ChannelSampleFormat::I16:
      ExtractChannels( img, view, E, baseId, (UInt16Image*)( nullptr ) );
      break;
   case ChannelSampleFormat::I32:
      ExtractChannels( img, view, E, baseId, (UInt32Image*)( nullptr ) );
      break;
   }
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view, false/*lock*/ );
   lock.LockForWrite();

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      throw Error( "ChannelExtraction cannot be executed on complex images." );

   if ( image->ColorSpace() != ColorSpace::RGB )
      throw Error( "ChannelExtraction requires a RGB color image." );

   Console().EnableAbort();

   StandardStatus status;
   image->SetStatusCallback( &status );

   String baseId = view.Id();
   if ( view.IsPreview() )
      baseId.Prepend( view.Window().MainView().Id() + '_' );

   if ( p_sampleFormat == ChannelSampleFormat::SameAsSource )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            ExtractChannels( static_cast<const Image&>( *image ), view, *this, baseId, (Image*)( nullptr ) );
            break;
         case 64:
            ExtractChannels( static_cast<const DImage&>( *image ), view, *this, baseId, (DImage*)( nullptr ) );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8:
            ExtractChannels( static_cast<const UInt8Image&>( *image ), view, *this, baseId, (UInt8Image*)( nullptr ) );
            break;
         case 16:
            ExtractChannels( static_cast<const UInt16Image&>( *image ), view, *this, baseId, (UInt16Image*)( nullptr ) );
            break;
         case 32:
            ExtractChannels( static_cast<const UInt32Image&>( *image ), view, *this, baseId, (UInt32Image*)( nullptr ) );
            break;
         }
   }
   else
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            ExtractChannels( static_cast<const Image&>( *image ), view, *this, baseId );
            break;
         case 64:
            ExtractChannels( static_cast<const DImage&>( *image ), view, *this, baseId );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8:
            ExtractChannels( static_cast<const UInt8Image&>( *image ), view, *this, baseId );
            break;
         case 16:
            ExtractChannels( static_cast<const UInt16Image&>( *image ), view, *this, baseId );
            break;
         case 32:
            ExtractChannels( static_cast<const UInt32Image&>( *image ), view, *this, baseId );
            break;
         }
   }

   return true;
}

// ----------------------------------------------------------------------------

void* ChannelExtractionInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheColorSpaceIdExtractionParameter )
      return &p_colorSpace;
   if ( p == TheChannelEnabledExtractionParameter )
      return p_channelEnabled+tableRow;
   if ( p == TheChannelIdExtractionParameter )
      return p_channelId[tableRow].Begin();
   if ( p == TheChannelSampleFormatExtractionParameter )
      return &p_sampleFormat;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ChannelExtractionInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheChannelIdExtractionParameter )
   {
      p_channelId[tableRow].Clear();
      if ( sizeOrLength > 0 )
         p_channelId[tableRow].SetLength( sizeOrLength );
      return true;
   }

   if ( p == TheChannelTableExtractionParameter )
      return true;

   return false;
}

// ----------------------------------------------------------------------------

size_type ChannelExtractionInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheChannelIdExtractionParameter )
      return p_channelId[tableRow].Length();
   if ( p == TheChannelTableExtractionParameter )
      return 3;

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelExtractionInstance.cpp - Released 2017-05-02T09:43:00Z
