//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0332
// ----------------------------------------------------------------------------
// DefectMapInstance.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "DefectMapInstance.h"
#include "DefectMapParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/Matrix.h>
#include <pcl/StdStatus.h>
#include <pcl/Vector.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

DefectMapInstance::DefectMapInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   operation( DMPOperation::Default ),
   structure( DMPStructure::Default ),
   isCFA( TheDMPIsCFAParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

DefectMapInstance::DefectMapInstance( const DefectMapInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void DefectMapInstance::Assign( const ProcessImplementation& p )
{
   const DefectMapInstance* x = dynamic_cast<const DefectMapInstance*>( &p );
   if ( x != nullptr )
   {
      mapId     = x->mapId;
      operation = x->operation;
      structure = x->structure;
      isCFA     = x->isCFA;
   }
}

// ----------------------------------------------------------------------------

bool DefectMapInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.GetImage().IsComplexSample() )
   {
      whyNot = "DefectMap cannot be executed on complex images.";
      return false;
   }

   if ( view.IsPreview() )
   {
      whyNot = "DefectMap cannot be executed on previews.";
      return false;
   }

   return true;
}

// -------------------------------------------------------------------------

bool IsSameCFA( int x, int y, int i, int j )
{

   if ( x%2 == 0 && y%2 == 0 ) // is red
   {
      if ( i%2 == 0 && j%2 == 0 )
         return true;
   }
   else if ( x%2 != 0 && y%2 != 0 ) // is blue
   {
      if ( ( i%2 != 0 && j%2 != 0 ) )
         return true;
   }
   else
   {
      if ( (x%2 == 0 && y%2 != 0) || (x%2 != 0 && y%2 == 0) )
         return true;
   }

   return false;
}

// -------------------------------------------------------------------------

template <class P2> static
int MapRadius( const GenericImage<P2>& map, int x, int y, pcl_bool isCFA )
{
   int radius = 0;
   for ( ;; )
   {
      ++radius;
      float acc = 0;
      for ( int i = x-radius; i <= x+radius; ++i )
         if ( i >= 0 && i < map.Width() )
            for ( int j = y-radius; j <= y+radius; ++j )
               if ( j >= 0 && j < map.Height() )
                  if ( !isCFA || IsSameCFA( x, y, i, j ) )
                  {
                     float f;
                     P2::FromSample( f, map.Pixel( i, j, map.SelectedChannel() ) );
                     acc += f;
                  }
      acc /= (2*radius + 1)*(2*radius + 1);
      if ( isCFA )
         acc *= 3;
      if ( acc >= 0.35 || radius >= 100 )
         break;
   }

   return radius;
}

static
FMatrix StructuringElement( pcl_enum structure, int radius )
{
   FMatrix S( 2*radius + 1, 2*radius + 1 );
   switch ( structure )
   {
   default:
   case DMPStructure::Square:
      S = 1;
      break;
   case DMPStructure::Circular :
      {
         float r2 = radius*radius;
         for ( int i = -radius, r = 0; i <= radius; ++i, ++r )
            for ( int j = -radius, c = 0; j <= radius; ++j, ++c )
               S[r][c] = (r2 < i*i + j*j) ? 0 : 1;
      }
      break;
   case DMPStructure::Horizontal :
      S = 0;
      for ( int i = -radius, c = 0; i <= radius; ++i, ++c )
         S[radius][c] = 1;
      break;
   case DMPStructure::Vertical :
      S = 0;
      for ( int i = -radius, r = 0; i <= radius; ++i, ++r )
         S[r][radius] = 1;
      break;
   }

   return S;
}

// ----------------------------------------------------------------------------

/*
 * Fixing bad pixels with a convolution
 */

template <class P1, class P2> static
void Convolution2( GenericImage<P1>& image, const GenericImage<P2>& map, pcl_enum operation, pcl_enum structure, pcl_bool isCFA )
{
   StandardStatus callback;
   StatusMonitor monitor;
   monitor.SetCallback( &callback );
   monitor.Initialize( "<end><cbr>Fixing bad pixels by convolution", image.NumberOfPixels() );

   int chi = image.SelectedChannel();
   int chm = map.SelectedChannel();

   for ( int x = 0; x < image.Width(); ++x )
      for( int y = 0; y < image.Height(); ++y, ++monitor )
         if ( map.Pixel( x, y, chm ) == 0 ) // process only bad pixels
         {
            // Neighborhood radius
            int radius = MapRadius( map, x, y, isCFA );

            // Convolution filter
            FMatrix ConvKernel( 2*radius + 1, 2*radius + 1 );
            switch ( operation )
            {
            default:
            case DMPOperation::Mean:
               ConvKernel = 1;
               break;
            case DMPOperation::Gaussian:
               {
                  float twor2 = 2*radius*radius;
                  for ( int i = -radius, r = 0; i <= radius; ++i, ++r )
                     for ( int j = -radius, c = 0; j <= radius; ++j, ++c )
                        ConvKernel[r][c] = Exp( -(i*i + j*j)/twor2 ); // no need to normalize at this point
               }
               break;
            }

            // Structuring element
            FMatrix Structure = StructuringElement( structure, radius );

            // Recalculate the filter kernel, weighting by the defect map and the structuring element
            for ( int i = -radius, c = 0; i <= radius; ++i, ++c )
               for ( int j = -radius, r = 0; j <= radius; ++j, ++r )
                  if ( ( i+x >= 0 && i+x < image.Width() && j+y >= 0 && j+y < image.Height() ) && ( !isCFA || IsSameCFA(x,y,x+i,y+j) ))
                  {
                     float m;
                     P2::FromSample( m, map.Pixel( i+x, j+y, chm ) );
                     ConvKernel[r][c] *= Structure[r][c] * m;
                  }
                  else
                     ConvKernel[r][c] = 0;

            // Normalize all kernel elements to unit weight
            double norm = ConvKernel.Modulus();
            if ( 1 + norm != 1 )
               ConvKernel /= norm;

            // Perform convolution, single pixel.
            double f = 0;
            for ( int px = x-radius, c = 0; px <= x+radius; ++px, ++c )
               if ( px >= 0 && px < image.Width() )
                  for ( int py = y-radius, r = 0; py <= y+radius; ++py, ++r )
                     if ( py >= 0 && py < image.Height() )
                     {
                        float t;
                        P1::FromSample( t, image.Pixel( px, py, chi ) );
                        f += t * ConvKernel[r][c];
                     }
            image.Pixel( x, y, chi ) = P1::ToSample( Range( f, 0.0, 1.0 ) );
         }
}

template <class P1> static
void Convolution1( GenericImage<P1>& image, const ImageVariant& map, pcl_enum operation, pcl_enum structure, pcl_bool isCFA )
{
   if ( map.IsFloatSample() )
      switch ( map.BitsPerSample() )
      {
      case 32: Convolution2( image, static_cast<const pcl::Image&>( *map ), operation, structure, isCFA ); break;
      case 64: Convolution2( image, static_cast<const pcl::DImage&>( *map ), operation, structure, isCFA ); break;
      }
   else
      switch ( map.BitsPerSample() )
      {
      case  8: Convolution2( image, static_cast<const UInt8Image&>( *map ), operation, structure, isCFA ); break;
      case 16: Convolution2( image, static_cast<const UInt16Image&>( *map ), operation, structure, isCFA ); break;
      case 32: Convolution2( image, static_cast<const UInt32Image&>( *map ), operation, structure, isCFA ); break;
      }
}

static
void Convolution( ImageVariant& image, const ImageVariant& map, pcl_enum operation, pcl_enum structure, pcl_bool isCFA )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: Convolution1( static_cast<pcl::Image&>( *image ), map, operation, structure, isCFA ); break;
      case 64: Convolution1( static_cast<pcl::DImage&>( *image ), map, operation, structure, isCFA ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: Convolution1( static_cast< UInt8Image&>( *image ), map, operation, structure, isCFA ); break;
      case 16: Convolution1( static_cast<UInt16Image&>( *image ), map, operation, structure, isCFA ); break;
      case 32: Convolution1( static_cast<UInt32Image&>( *image ), map, operation, structure, isCFA ); break;
      }
}

// ----------------------------------------------------------------------------

/*
 * Fixing bad pixels with a morphological operation
 */

static
const String OperationName( pcl_enum operation )
{
   switch ( operation )
   {
   case DMPOperation::Minimum: return "minimum";
   case DMPOperation::Maximum: return "maximum";
   default:
   case DMPOperation::Median:  return "median";
   }
}

template <class P1, class P2> inline
void Morphological2( GenericImage<P1>& image, const GenericImage<P2>& map, pcl_enum operation, pcl_enum structure, pcl_bool isCFA )
{
   StandardStatus callback;
   StatusMonitor monitor;
   monitor.SetCallback( &callback );
   monitor.Initialize( "<end><cbr>Fixing bad pixels, morphological " + OperationName( operation ), image.NumberOfPixels() );

   int chi = image.SelectedChannel();
   int chm = map.SelectedChannel();

   for ( int x = 0; x < image.Width(); ++x )
      for( int y = 0; y < image.Height(); ++y, ++monitor )
         if ( map.Pixel( x, y, chm ) == 0 ) // process only bad pixels
         {
            // Neighborhood radius
            int radius = MapRadius( map, x, y, isCFA );

            // Structuring element
            FMatrix Structure = StructuringElement( structure, radius );

            // Remove structure elements where the defect map is zero
            for ( int i = -radius, c = 0; i <= radius; ++i, ++c )
               for ( int j = -radius, r = 0; j <= radius; ++j, ++r )
                  if ( Structure[r][c] != 0 )
                     if ( image.Includes( i+x, j+y ) )
                     {
                        float m;
                        P2::FromSample( m, map.Pixel( i+x, j+y, chm ) );
                        if ( m == 0 || ( isCFA && !IsSameCFA(x,y,x+i,y+j) ) )
                           Structure[r][c] = 0;

                     }

            // Perform the morphological filter operation on the current image pixel

            DVector pixels( (2*radius + 1)*(2*radius + 1) );
            int n = 0;
            for ( int i = x-radius, c = 0; i <= x+radius; ++i, ++c )
               if ( i >= 0 && i < image.Width() )
                  for ( int j = y-radius, r = 0; j <= y+radius; ++j, ++r )
                     if ( Structure[r][c] != 0 )
                        if ( j >= 0 && j < image.Height() )
                           P1::FromSample( pixels[n++], image.Pixel( i, j, chi ) );

            double f;
            switch ( operation )
            {
            case DMPOperation::Minimum:
               f = *pcl::MinItem( pixels.Begin(), pixels.At( n ) );
               break;
            case DMPOperation::Maximum:
               f = *pcl::MaxItem( pixels.Begin(), pixels.At( n ) );
               break;
            default:
            case DMPOperation::Median:
               f = pcl::Median( pixels.Begin(), pixels.At( n ) );
               break;
            }

            image.Pixel( x, y, chi ) = P1::ToSample( Range( f, 0.0, 1.0 ) );
         }
}

template <class P1> static
void Morphological1( GenericImage<P1>& image, const ImageVariant& map, pcl_enum operation, pcl_enum structure, pcl_bool isCFA )
{
   if ( map.IsFloatSample() )
      switch ( map.BitsPerSample() )
      {
      case 32: Morphological2( image, static_cast<const pcl::Image&>( *map ), operation, structure, isCFA ); break;
      case 64: Morphological2( image, static_cast<const pcl::DImage&>( *map ), operation, structure, isCFA ); break;
      }
   else
      switch ( map.BitsPerSample() )
      {
      case  8: Morphological2( image, static_cast<const UInt8Image&>( *map ), operation, structure, isCFA ); break;
      case 16: Morphological2( image, static_cast<const UInt16Image&>( *map ), operation, structure, isCFA ); break;
      case 32: Morphological2( image, static_cast<const UInt32Image&>( *map ), operation, structure, isCFA ); break;
      }
}

static
void Morphological( ImageVariant& image, const ImageVariant& map, pcl_enum operation, pcl_enum structure, pcl_bool isCFA )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: Morphological1( static_cast<pcl::Image&>( *image ), map, operation, structure, isCFA ); break;
      case 64: Morphological1( static_cast<pcl::DImage&>( *image ), map, operation, structure, isCFA ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: Morphological1( static_cast<UInt8Image&>( *image ), map, operation, structure, isCFA ); break;
      case 16: Morphological1( static_cast<UInt16Image&>( *image ), map, operation, structure, isCFA ); break;
      case 32: Morphological1( static_cast<UInt32Image&>( *image ), map, operation, structure, isCFA ); break;
      }
}

// ----------------------------------------------------------------------------

bool DefectMapInstance::ExecuteOn( View& view )
{
   AutoViewLock lock1( view );

   ImageVariant image = view.GetImage();

   ImageWindow mapWindow = ImageWindow::WindowById( mapId );
   if ( mapWindow.IsNull() )
      throw Error( "No such defect map image: " + mapId );

   View mapView = mapWindow.MainView();

   AutoViewLock lock2( mapView, false/*lock*/ );
   lock2.LockForWrite();

   ImageVariant map = mapView.GetImage();

   Console().EnableAbort();

   // The map cannot be a color image if the image is grayscale
   if ( map.IsColor() && !image.IsColor() )
      throw Error( "Invalid color defect map image for a grayscale target image: " + mapId );

   // Both images must have the same geometry
   if ( image.Width() != map.Width() ||
      image.Height() != map.Height() )
      throw Error( "Incompatible image geometry: " + view.Id() );

   // For each target channel
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      Console().WriteLn( "<end><cbr><br>Channel #" + String( c ) );

      image.SelectChannel( c );
      map.SelectChannel( map.IsColor() ? c : 0 );

      switch ( operation )
      {
      case DMPOperation::Minimum:
      case DMPOperation::Maximum:
      case DMPOperation::Median:
         Morphological( image, map, operation, structure, isCFA );
         break;
      case DMPOperation::Mean:
      case DMPOperation::Gaussian:
         Convolution( image, map, operation, structure, isCFA );
         break;
      }
   }

   return true;
}

// ----------------------------------------------------------------------------

void* DefectMapInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheDMPMapIdParameter )
      return mapId.Begin();
   if ( p == TheDMPOperationParameter )
      return &operation;
   if ( p == TheDMPStructureParameter )
      return &structure;
   if ( p == TheDMPIsCFAParameter )
      return &isCFA;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool DefectMapInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheDMPMapIdParameter )
   {
      mapId.Clear();
      if ( sizeOrLength > 0 )
         mapId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type DefectMapInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheDMPMapIdParameter )
      return mapId.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DefectMapInstance.cpp - Released 2017-08-01T14:26:58Z
