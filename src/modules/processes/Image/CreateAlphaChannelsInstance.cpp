// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Image Process Module Version 01.02.08.0281
// ****************************************************************************
// CreateAlphaChannelsInstance.cpp - Released 2014/10/29 07:35:22 UTC
// ****************************************************************************
// This file is part of the standard Image PixInsight module.
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

#include "CreateAlphaChannelsInstance.h"

#include <pcl/View.h>
#include <pcl/ImageWindow.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/StdStatus.h>

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

CreateAlphaChannelsInstance::CreateAlphaChannelsInstance( const CreateAlphaChannelsInstance& x ) : ProcessImplementation( x )
{
   Assign( x );
}

void CreateAlphaChannelsInstance::Assign( const ProcessImplementation& p )
{
   const CreateAlphaChannelsInstance* x = dynamic_cast<const CreateAlphaChannelsInstance*>( &p );
   if ( x != 0 && x != this )
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

   whyNot.Clear();
   return true;
}

template <class P1, class P2>
static void __CreateFromImage( GenericImage<P1>& image, int w, int h,
                               const GenericImage<P2>& source, const Point& p0,
                               bool invert, int c0, int n )
{
   PixelInterpolation::Interpolator<P2>* interpolator0 = 0;
   PixelInterpolation::Interpolator<P2>* interpolator1 = 0;
   PixelInterpolation::Interpolator<P2>* interpolator2 = 0;

   try
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

         interpolator0 = interpolation.NewInterpolator( (P2*)0, source.PixelData( 0 ), source.Width(), source.Height() );

         if ( source.IsColor() )
         {
            interpolator1 = interpolation.NewInterpolator( (P2*)0, source.PixelData( 1 ), source.Width(), source.Height() );
            interpolator2 = interpolation.NewInterpolator( (P2*)0, source.PixelData( 2 ), source.Width(), source.Height() );

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

            delete interpolator2, interpolator2 = 0;
            delete interpolator1, interpolator1 = 0;
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

         delete interpolator0, interpolator0 = 0;
      }
   }

   catch ( ... )
   {
      if ( interpolator0 != 0 )
         delete interpolator0;
      if ( interpolator1 != 0 )
         delete interpolator1;
      if ( interpolator2 != 0 )
         delete interpolator2;
      throw;
   }
}

template <class P1>
static void __CreateFromImage( GenericImage<P1>& image, int w, int h,
                               const ImageVariant& source, const Point& p0,
                               bool invert, int c0, int n )
{
   if ( source.IsFloatSample() )
      switch ( source.BitsPerSample() )
      {
      case 32: __CreateFromImage( image, w, h, *reinterpret_cast<const Image*>( source.AnyImage() ), p0, invert, c0, n ); break;
      case 64: __CreateFromImage( image, w, h, *reinterpret_cast<const DImage*>( source.AnyImage() ), p0, invert, c0, n ); break;
      }
   else
      switch ( source.BitsPerSample() )
      {
      case  8: __CreateFromImage( image, w, h, *reinterpret_cast<const UInt8Image*>( source.AnyImage() ), p0, invert, c0, n ); break;
      case 16: __CreateFromImage( image, w, h, *reinterpret_cast<const UInt16Image*>( source.AnyImage() ), p0, invert, c0, n ); break;
      case 32: __CreateFromImage( image, w, h, *reinterpret_cast<const UInt32Image*>( source.AnyImage() ), p0, invert, c0, n ); break;
      }
}

static void __CreateFromImage( ImageVariant& image, int w, int h,
                               const ImageVariant& source, const Point& p0,
                               bool invert, int c0, int n )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: __CreateFromImage( *reinterpret_cast<Image*>( image.AnyImage() ), w, h, source, p0, invert, c0, n ); break;
      case 64: __CreateFromImage( *reinterpret_cast<DImage*>( image.AnyImage() ), w, h, source, p0, invert, c0, n ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: __CreateFromImage( *reinterpret_cast<UInt8Image*>( image.AnyImage() ), w, h, source, p0, invert, c0, n ); break;
      case 16: __CreateFromImage( *reinterpret_cast<UInt16Image*>( image.AnyImage() ), w, h, source, p0, invert, c0, n ); break;
      case 32: __CreateFromImage( *reinterpret_cast<UInt32Image*>( image.AnyImage() ), w, h, source, p0, invert, c0, n ); break;
      }
}

//

template <class P>
static void __CreateFromTransparency( GenericImage<P>& image, double t, int c0, int n )
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

static void __CreateFromTransparency( ImageVariant& image, double t, int c0, int n )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: __CreateFromTransparency( *reinterpret_cast<Image*>( image.AnyImage() ), t, c0, n ); break;
      case 64: __CreateFromTransparency( *reinterpret_cast<DImage*>( image.AnyImage() ), t, c0, n ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: __CreateFromTransparency( *reinterpret_cast<UInt8Image*>( image.AnyImage() ), t, c0, n ); break;
      case 16: __CreateFromTransparency( *reinterpret_cast<UInt16Image*>( image.AnyImage() ), t, c0, n ); break;
      case 32: __CreateFromTransparency( *reinterpret_cast<UInt32Image*>( image.AnyImage() ), t, c0, n ); break;
      }
}

//

template <class P>
static int __CreateAlphaChannels( GenericImage<P>& image, bool replace, int n )
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

static int __CreateAlphaChannels( ImageVariant& image, bool replace, int n )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return __CreateAlphaChannels( *reinterpret_cast<Image*>( image.AnyImage() ), replace, n );
      case 64: return __CreateAlphaChannels( *reinterpret_cast<DImage*>( image.AnyImage() ), replace, n );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return __CreateAlphaChannels( *reinterpret_cast<UInt8Image*>( image.AnyImage() ), replace, n );
      case 16: return __CreateAlphaChannels( *reinterpret_cast<UInt16Image*>( image.AnyImage() ), replace, n );
      case 32: return __CreateAlphaChannels( *reinterpret_cast<UInt32Image*>( image.AnyImage() ), replace, n );
      }
   return 0; // ??
}

//

bool CreateAlphaChannelsInstance::ExecuteOn( View& view )
{
   try
   {
      view.Lock();

      Console().EnableAbort();

      ImageVariant v;
      v = view.Image();

      StandardStatus status;
      v.AnyImage()->SetStatusCallback( &status );

      int c0 = __CreateAlphaChannels( v, replace, count );

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
            w = v.AnyImage()->Width();
            h = v.AnyImage()->Height();
            p0 = 0;
         }

         ImageWindow sourceWindow = imageId.IsEmpty() ? targetWindow : ImageWindow::WindowById( imageId );

         if ( sourceWindow.IsNull() )
            throw Error( "Source image not found: " + imageId );

         View sourceView = sourceWindow.MainView();

         try
         {
            if ( sourceView != view )
               sourceView.LockForWrite();

            ImageVariant v0 = sourceView.Image();
            __CreateFromImage( v, w, h, v0, p0, invertSourceImage, c0, count );

            if ( sourceView != view )
               sourceView.Unlock();
         }

         catch ( ... )
         {
            if ( sourceView != view )
               sourceView.Unlock();
            throw;
         }

         if ( closeSourceImage && sourceWindow != targetWindow )
            sourceWindow.Close();
      }
      else
         __CreateFromTransparency( v, transparency, c0, count );

      view.Unlock();

      return true;
   }

   catch ( ... )
   {
      view.Unlock();
      throw;
   }
}

void* CreateAlphaChannelsInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCAFromImageParameter )
      return &fromImage;
   if ( p == TheCASourceImageIdentifierParameter )
      return imageId.c_str();
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
         imageId.Reserve( sizeOrLength );
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

// ****************************************************************************
// EOF CreateAlphaChannelsInstance.cpp - Released 2014/10/29 07:35:22 UTC
