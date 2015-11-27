//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// GradientsHdrInstance.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2015. Licensed under LGPL 2.1
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include "GradientsHdrInstance.h"
#include <cmath>

#include "GradientsHdrParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

#include "GradientsHdrCompression.h" // this contains the real magic...
#include "RgbPreserve.h" //color preserving intensity transform

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsHdrInstance::GradientsHdrInstance( const MetaProcess* m )
 :ProcessImplementation( m )
 ,expGradient( TheGradientsHdrParameterExpGradient->DefaultValue() )
 ,logMaxGradient( TheGradientsHdrParameterLogMaxGradient->DefaultValue() )
 ,logMinGradient( TheGradientsHdrParameterLogMinGradient->DefaultValue() )
 ,bRescale01(TheGradientsHdrParameterRescale01->DefaultValue())
 ,bPreserveColor(TheGradientsHdrParameterPreserveColor->DefaultValue())
{
}

GradientsHdrInstance::GradientsHdrInstance( const GradientsHdrInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void GradientsHdrInstance::Assign( const ProcessImplementation& p )
{
   const GradientsHdrInstance* x = dynamic_cast<const GradientsHdrInstance*>( &p );
   if ( x != 0 )
   {
     expGradient = x->expGradient;
     logMaxGradient = x->logMaxGradient;
     logMinGradient = x->logMinGradient;
     bRescale01= x->bRescale01;
     bPreserveColor= x->bPreserveColor;
   }
}

bool GradientsHdrInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "GradientsHdrCompression cannot be executed on complex images.";
      return false;
   }

   if (expGradient<0.0) {
     whyNot = "GradientsHdrCompression requires that expGradient>=0.0";
     return false;
   }
   if (logMaxGradient<=logMinGradient) {
     whyNot = "GradientsHdrCompression requires that logMaxGradient> logMinGradient";
     return false;
   }
   whyNot.Clear();
   return true;
}

class GradientsHdrEngine
{
public:

  /// takes any kind of image, and applies gradient hdr compression to its luminance.
  /// @param zoomlevel is the factor by which width/height have been divided compared
  /// to the original image
  template <class P>
  static void Apply( GenericImage<P>& img, const GradientsHdrInstance& instance)
  {
    // clamp this value to 0 if it is very small
    double minGradient=0.0;
    const double minRange=TheGradientsHdrParameterLogMinGradient->MaximumValue()-TheGradientsHdrParameterLogMinGradient->MinimumValue();
    if (instance.logMinGradient>TheGradientsHdrParameterLogMinGradient->MinimumValue()+0.05*minRange) {
      minGradient=std::pow(10.0,instance.logMinGradient);
    }
    GradientsHdrCompression::imageType_t lumImage;
    // we need to extract luminance, and convert to float. Also works for greyscale images
    img.GetLightness(lumImage);
    // do the work
    GradientsHdrCompression gradients;
    gradients.hdrCompressionSetImage(lumImage);
    GradientsHdrCompression::imageType_t resImage;
    gradients.hdrCompression(std::pow(10.0,instance.logMaxGradient),
			     minGradient,
			     instance.expGradient,
			     instance.bRescale01,
			     resImage);

    if(img.NumberOfNominalChannels()>1) {
      if (instance.bPreserveColor) {
	RgbPreserve rgbPreserve;
	RgbPreserve::imageType_t transformedImage(img);
	rgbPreserve.rgbTransformation(lumImage,resImage,transformedImage);
	img.Assign(transformedImage);

      } else {
	// Note: if this strongly stretches the image, we loose a lot of color here
	img.SetLightness(resImage);
      }
      // we need to insert the luminance into the original image
    }else{
	// unfortunately SetLightness() does not work for grayscale targets.
	// Therefore this is the result
      img.Assign(resImage);
    }
  }
};

void GradientsHdrInstance::ApplyClip16( UInt16Image& img, int zoom )
{
  // zoomlevel is the negative factor by which the original image dimensions
  // have been reduced. Example: Zoomlevel -3: imageWidth=orgWidth/3.
  // luckily, the parameters of GradientsHdrEngine::Apply() are independent of the
  // zoom level.
  GradientsHdrEngine::Apply(img, *this);
}

bool GradientsHdrInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   Console().EnableAbort(); // FIXME no abort implemented yet

   StandardStatus status;
   image->SetStatusCallback( &status );

   if ( !image.IsComplexSample() )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            GradientsHdrEngine::Apply( static_cast<pcl::Image&>( *image ), *this );
            break;
         case 64:
            GradientsHdrEngine::Apply( static_cast<pcl::DImage&>( *image ), *this );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8:
            GradientsHdrEngine::Apply( static_cast<pcl::UInt8Image&>( *image ), *this );
            break;
         case 16:
            GradientsHdrEngine::Apply( static_cast<pcl::UInt16Image&>( *image ), *this );
            break;
         case 32:
            GradientsHdrEngine::Apply( static_cast<pcl::UInt32Image&>( *image ), *this );
            break;
         }

   return true;
}

void* GradientsHdrInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheGradientsHdrParameterExpGradient )
      return &expGradient;
   if ( p == TheGradientsHdrParameterLogMaxGradient )
      return &logMaxGradient;
   if ( p == TheGradientsHdrParameterLogMinGradient )
      return &logMinGradient;
   if ( p == TheGradientsHdrParameterRescale01 )
      return &bRescale01;
   if ( p == TheGradientsHdrParameterPreserveColor )
      return &bPreserveColor;

   return 0;
}

bool GradientsHdrInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
  return false;
}

size_type GradientsHdrInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsHdrInstance.cpp - Released 2015/11/26 16:00:13 UTC
