//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0371
// ----------------------------------------------------------------------------
// CreateAlphaChannelsInstance.cpp - Released 2017-05-02T09:43:00Z
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

#include "CreateAlphaChannelsInstance.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CreateAlphaChannelsInstance::CreateAlphaChannelsInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   fromImage( TheCAFromImageParameter->DefaultValue() ),
   imageId( TheCASourceImageIdentifierParameter->DefaultValue() ),
   invertSourceImage( TheCAInvertSourceImageParameter->DefaultValue() ),
   closeSourceImage( TheCACloseSourceImageParameter->DefaultValue() ),
   transparency( TheCATransparencyParameter->DefaultValue() ),
   replace( TheCAReplaceExistingChannelsParameter->DefaultValue() ),
   count( int( TheCAChannelCountParameter->DefaultValue() ) )
{
}

CreateAlphaChannelsInstance::CreateAlphaChannelsInstance( const CreateAlphaChannelsInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void CreateAlphaChannelsInstance::Assign( const ProcessImplementation& p )
{
   const CreateAlphaChannelsInstance* x = dynamic_cast<const CreateAlphaChannelsInstance*>( &p );
   if ( x != nullptr )
   {
      fromImage         = x->fromImage;
      imageId           = x->imageId;
      invertSourceImage = x->invertSourceImage;
      closeSourceImage  = x->closeSourceImage;
      transparency      = x->transparency;
      replace           = x->replace;
      count             = x->count;
   }
}

bool CreateAlphaChannelsInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

bool CreateAlphaChannelsInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "CreateAlphaChannels cannot be executed on complex images.";
      return false;
   }

   return true;
}

template <class P1, class P2>
static void CreateAlphaFromImage( GenericImage<P1>& image, int w, int h,
                                  const GenericImage<P2>& source, const Point& p0,
                                  bool invert, int c0, int n )
{
   image.Status().Initialize( String().Format( "Generating %d alpha channel(s)", n ), image.Height() );

   if ( source.Width() == w && source.Height() == h )
   {
      if ( source.IsColor() )
      {
         RGBColorSystem rgbws = source.RGBWorkingSpace();

         for ( int i = 0; i < image.Height(); ++i, ++image.Status() )
         {
            int y = p0.y + i;

            for ( int j = 0; j < image.Width(); ++j )
            {
               int x = p0.x + j;

               RGBColorSystem::sample r, g, b;
               P2::FromSample( r, source.Pixel( x, y, 0 ) );
               P2::FromSample( g, source.Pixel( x, y, 1 ) );
               P2::FromSample( b, source.Pixel( x, y, 2 ) );

               typename P1::sample L = P1::ToSample( rgbws.Lightness( r, g, b ) );
               if ( invert )
                  L = P1::MaxSampleValue() - L;

               for ( int c = 0; c < n; ++c )
                  image.Pixel( j, i, c0+c ) = L;
            }
         }
      }
      else
      {
         image.Status().DisableInitialization();

         image.ResetSelections();

         source.SelectRectangle( image.Bounds() + p0 );

         for ( int c = 0; c < n; ++c, ++image.Status() )
         {
            image.SelectChannel( c0+c );
            image.Apply( source );
            if ( invert )
               image.Invert();
         }

         image.ResetSelections();

         image.Status().Complete();
      }
   }
   else
   {
      BicubicPixelInterpolation interpolation;

      double rx = double( source.Width() )/w;
      double ry = double( source.Height() )/h;

      AutoPointer<PixelInterpolation::Interpolator<P2> > interpolator0(
            interpolation.NewInterpolator<P2>( source[0], source.Width(), source.Height() ) );

      if ( source.IsColor() )
      {
         AutoPointer<PixelInterpolation::Interpolator<P2> > interpolator1(
               interpolation.NewInterpolator<P2>( source[1], source.Width(), source.Height() ) );
         AutoPointer<PixelInterpolation::Interpolator<P2> > interpolator2(
               interpolation.NewInterpolator<P2>( source[2], source.Width(), source.Height() ) );

         RGBColorSystem rgbws = source.RGBWorkingSpace();

         for ( int i = 0; i < image.Height(); ++i, ++image.Status() )
         {
            double y = ry*(p0.y + i);

            for ( int j = 0; j < image.Width(); ++j )
            {
               double x = rx*(p0.x + j);

               RGBColorSystem::sample r, g, b;
               P2::FromSample( r, (*interpolator0)( x, y ) );
               P2::FromSample( g, (*interpolator1)( x, y ) );
               P2::FromSample( b, (*interpolator2)( x, y ) );

               typename P1::sample L = P1::ToSample( rgbws.Lightness( r, g, b ) );
               if ( invert )
                  L = P1::MaxSampleValue() - L;

               for ( int c = 0; c < n; ++c )
                  image.Pixel( j, i, c0+c ) = L;
            }
         }
      }
      else
      {
         for ( int i = 0; i < image.Height(); ++i, ++image.Status() )
         {
            double y = ry*(p0.y + i);

            for ( int j = 0; j < image.Width(); ++j )
            {
               typename P1::sample f;
               P2::FromSample( f, (*interpolator0)( rx*(p0.x + j), y ) );
               if ( invert )
                  f = P1::MaxSampleValue() - f;

               for ( int c = 0; c < n; ++c )
                  image.Pixel( j, i, c0+c ) = f;
            }
         }
      }
   }
}

template <class P1>
static void CreateAlphaFromImage( GenericImage<P1>& image, int w, int h,
                                  const ImageVariant& source, const Point& p0,
                                  bool invert, int c0, int n )
{
   if ( source.IsFloatSample() )
      switch ( source.BitsPerSample() )
      {
      case 32:
         CreateAlphaFromImage( image, w, h, static_cast<const Image&>( *source ), p0, invert, c0, n );
         break;
      case 64:
         CreateAlphaFromImage( image, w, h, static_cast<const DImage&>( *source ), p0, invert, c0, n );
         break;
      }
   else
      switch ( source.BitsPerSample() )
      {
      case  8:
         CreateAlphaFromImage( image, w, h, static_cast<const UInt8Image&>( *source ), p0, invert, c0, n );
         break;
      case 16:
         CreateAlphaFromImage( image, w, h, static_cast<const UInt16Image&>( *source ), p0, invert, c0, n );
         break;
      case 32:
         CreateAlphaFromImage( image, w, h, static_cast<const UInt32Image&>( *source ), p0, invert, c0, n );
         break;
      }
}

static void CreateAlphaFromImage( ImageVariant& image, int w, int h,
                                  const ImageVariant& source, const Point& p0,
                                  bool invert, int c0, int n )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         CreateAlphaFromImage( static_cast<Image&>( *image ), w, h, source, p0, invert, c0, n );
         break;
      case 64:
         CreateAlphaFromImage( static_cast<DImage&>( *image ), w, h, source, p0, invert, c0, n );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         CreateAlphaFromImage( static_cast<UInt8Image&>( *image ), w, h, source, p0, invert, c0, n );
         break;
      case 16:
         CreateAlphaFromImage( static_cast<UInt16Image&>( *image ), w, h, source, p0, invert, c0, n );
         break;
      case 32:
         CreateAlphaFromImage( static_cast<UInt32Image&>( *image ), w, h, source, p0, invert, c0, n );
         break;
      }
}

//

template <class P>
static void CreateAlphaFromTransparency( GenericImage<P>& image, double t, int c0, int n )
{
   image.Status().Initialize( String().Format( "Generating %d alpha channel(s)", n ), n );
   image.Status().DisableInitialization();

   typename P::sample f = P::ToSample( t );

   for ( int c = 0; c < n; ++c, ++image.Status() )
   {
      image.SelectChannel( c0+c );
      image.Fill( f );
   }
}

static void CreateAlphaFromTransparency( ImageVariant& image, double t, int c0, int n )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         CreateAlphaFromTransparency( static_cast<Image&>( *image ), t, c0, n );
         break;
      case 64:
         CreateAlphaFromTransparency( static_cast<DImage&>( *image ), t, c0, n );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         CreateAlphaFromTransparency( static_cast<UInt8Image&>( *image ), t, c0, n );
         break;
      case 16:
         CreateAlphaFromTransparency( static_cast<UInt16Image&>( *image ), t, c0, n );
         break;
      case 32:
         CreateAlphaFromTransparency( static_cast<UInt32Image&>( *image ), t, c0, n );
         break;
      }
}

template <class P>
static int CreateAlphaChannels( GenericImage<P>& image, bool replace, int n )
{
   int c0 = replace ? image.NumberOfNominalChannels() : image.NumberOfChannels();
   if ( replace )
   {
      if ( n > image.NumberOfAlphaChannels() )
         image.CreateAlphaChannels( n - image.NumberOfAlphaChannels() );
   }
   else
      image.CreateAlphaChannels( n );

   return c0;
}

static int CreateAlphaChannels( ImageVariant& image, bool replace, int n )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         return CreateAlphaChannels( static_cast<Image&>( *image ), replace, n );
      case 64:
         return CreateAlphaChannels( static_cast<DImage&>( *image ), replace, n );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         return CreateAlphaChannels( static_cast<UInt8Image&>( *image ), replace, n );
      case 16:
         return CreateAlphaChannels( static_cast<UInt16Image&>( *image ), replace, n );
      case 32:
         return CreateAlphaChannels( static_cast<UInt32Image&>( *image ), replace, n );
      }
   return 0; // ??
}

//

bool CreateAlphaChannelsInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   Console().EnableAbort();

   StandardStatus status;
   image->SetStatusCallback( &status );

   int c0 = CreateAlphaChannels( image, replace, count );

   if ( fromImage )
   {
      int w, h;
      Point p0;

      ImageWindow targetWindow = view.Window();

      if ( view.IsPreview() )
      {
         View mainView = targetWindow.MainView();
         w = mainView.Width();
         h = mainView.Height();
         p0 = targetWindow.PreviewRect( view.Id() ).LeftTop();
      }
      else
      {
         w = image->Width();
         h = image->Height();
         p0 = 0;
      }

      ImageWindow sourceWindow = imageId.IsEmpty() ? targetWindow : ImageWindow::WindowById( imageId );
      if ( sourceWindow.IsNull() )
         throw Error( "Source image not found: " + imageId );

      {
         View sourceView = sourceWindow.MainView();
         AutoViewLock lockSource( sourceView, false/*lock*/ );
         if ( sourceView != view )
            lockSource.LockForWrite();
         ImageVariant v0 = sourceView.Image();
         CreateAlphaFromImage( image, w, h, v0, p0, invertSourceImage, c0, count );
      }

      if ( closeSourceImage && sourceWindow != targetWindow )
         sourceWindow.Close();
   }
   else
      CreateAlphaFromTransparency( image, transparency, c0, count );

   return true;
}

void* CreateAlphaChannelsInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCAFromImageParameter )
      return &fromImage;
   if ( p == TheCASourceImageIdentifierParameter )
      return imageId.Begin();
   if ( p == TheCAInvertSourceImageParameter )
      return &invertSourceImage;
   if ( p == TheCACloseSourceImageParameter )
      return &closeSourceImage;
   if ( p == TheCATransparencyParameter )
      return &transparency;
   if ( p == TheCAReplaceExistingChannelsParameter )
      return &replace;
   if ( p == TheCAChannelCountParameter )
      return &count;
   return 0;
}

bool CreateAlphaChannelsInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheCASourceImageIdentifierParameter )
   {
      imageId.Clear();
      if ( sizeOrLength > 0 )
         imageId.SetLength( sizeOrLength );
   }
   else
      return false;
   return true;
}

size_type CreateAlphaChannelsInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheCASourceImageIdentifierParameter )
      return imageId.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsInstance.cpp - Released 2017-05-02T09:43:00Z
