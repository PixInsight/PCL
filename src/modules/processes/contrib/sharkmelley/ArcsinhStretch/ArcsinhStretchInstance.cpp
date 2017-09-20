//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 00.00.01.0112
// ----------------------------------------------------------------------------
// ArcsinhStretchInstance.cpp - Released 2017-09-20T13:03:37Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017 Mark Shelley
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

#include "ArcsinhStretchInstance.h"
#include "ArcsinhStretchParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretchInstance::ArcsinhStretchInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_stretch( TheArcsinhStretchParameter->DefaultValue() ),
   p_blackpoint( TheArcsinhStretchBlackPointParameter->DefaultValue() ),
   p_coarse( 0 ),
   p_fine( 0 ),
   p_protectHighlights( TheArcsinhStretchProtectHighlightsParameter->DefaultValue() ),
   p_useRgbws( TheArcsinhStretchUseRgbwsParameter->DefaultValue() ),
   p_previewClipped( TheArcsinhStretchPreviewClippedParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

ArcsinhStretchInstance::ArcsinhStretchInstance( const ArcsinhStretchInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInstance::Assign( const ProcessImplementation& p )
{
   const ArcsinhStretchInstance* x = dynamic_cast<const ArcsinhStretchInstance*>( &p );
   if ( x != nullptr )
   {
      p_stretch = x->p_stretch;
      p_blackpoint = x->p_blackpoint;
      p_protectHighlights = x->p_protectHighlights;
      p_useRgbws = x->p_useRgbws;
      p_previewClipped  = x->p_previewClipped;
   }
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ArcsinhStretch cannot be executed on complex images.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// ### TODO: Parallelize

class ArcsinhStretchEngine
{
public:

   static void Apply( ImageVariant& image, const ArcsinhStretchInstance& instance )
   {
      if ( !image.IsComplexSample() )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: Apply( static_cast<Image&>( *image ), instance, false/*preview_mode*/ ); break;
            case 64: Apply( static_cast<DImage&>( *image ), instance, false/*preview_mode*/ ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: Apply( static_cast<UInt8Image&>( *image ), instance, false/*preview_mode*/ ); break;
            case 16: Apply( static_cast<UInt16Image&>( *image ), instance, false/*preview_mode*/ ); break;
            case 32: Apply( static_cast<UInt32Image&>( *image ), instance, false/*preview_mode*/ ); break;
            }
   }

   template <class P>
   static void Apply( GenericImage<P>& image, const ArcsinhStretchInstance& instance, bool preview_mode )
   {
      double asinh_parm = 0;  // ArcsinhStretch parameter
      if ( instance.p_stretch != 1 )
      {
         // Binary search for parameter "a" that gives the right stretch
         double low = 0;
         double high = 10000;
         //double multiplier_low = 1;
         //double multiplier_high = high / ArcSinh( high );
         double mid;
         for ( int i = 0; i < 20; i++ )
         {
            mid = (low + high)/2;
            double multiplier_mid = mid / ArcSinh( mid );
            if ( instance.p_stretch <= multiplier_mid )
               high = mid;
            else
               low = mid;
         }
         asinh_parm = mid;
      }

      double maxStretchedVal = 0;
      double maxActual; P::FromSample( maxActual, image.MaximumSampleValue() );
      double denominator = ArcSinh( asinh_parm );

      int numChannels = image.NumberOfNominalChannels();

      // Set for RGB coefficients for calculating luminance and normalise them
      FVector lumCoeffs( float( 1/3.0 ), 3 ); // Default values are equally weighted 1/3 each
      if ( instance.p_useRgbws )
      {
         RGBColorSystem rgbws = image.RGBWorkingSpace();
         lumCoeffs = rgbws.LuminanceCoefficients();
      }
      // Normalise luminance coeffs
      double sumLumCoeffs = 0;
      for ( int ich = 0; ich < numChannels; ich++ )
         sumLumCoeffs += lumCoeffs[ich];
      for ( int ich = 0; ich < numChannels; ich++ )
         lumCoeffs[ich] /= sumLumCoeffs;

      //MDSTODO
      Console console;
      console.Write( String().Format( "<end><cbr>Normalized luminance coefficients: %.8g", lumCoeffs[0] ) );
      if ( numChannels > 1 )
         console.Write( String().Format( " %.8g %.8g", lumCoeffs[1], lumCoeffs[2] ) );
      console.WriteLn();

      /*
       * If scaling to protect highlights is switched on then pre-process the
       * data to calculate the required scaling factor. The maximum
       * post-stretch image value is then used in the second iteration as a
       * multiplier to prevent highlight clipping.
       */
      double multiplier = 1;
      if ( instance.p_protectHighlights )
      {
         for ( typename GenericImage<P>::const_pixel_iterator i( image ); i; ++i )
         {
            double intensity = 0;
            for ( int ich = 0; ich < numChannels; ich++ )
            {
               double v; P::FromSample( v, i[ich] );
               intensity += v * lumCoeffs[ich];
            }
            double rescaledintensity = (intensity - instance.p_blackpoint)/(maxActual - instance.p_blackpoint);
            double arcsinhScale = 1;
            if ( asinh_parm != 0 )
               if ( rescaledintensity != 0 )
                  arcsinhScale = ArcSinh( asinh_parm*rescaledintensity )/denominator/rescaledintensity;
            for ( int ich = 0; ich < numChannels; ich++ )
            {
               double v; P::FromSample( v, i[ich] );
               double newval = (v - instance.p_blackpoint)/(maxActual - instance.p_blackpoint) * arcsinhScale;
               maxStretchedVal = Max( maxStretchedVal, newval );
            }
         }

         //MDSTODO
         console.WriteLn( String().Format( "MaxStretchedVal: %.8g", maxStretchedVal ) );

         multiplier = 1/maxStretchedVal;
      }

      /*
       * Now treat the whole image using the multiplier just calculated.
       */
      for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i )
      {
         double intensity = 0;
         for ( int ich = 0; ich < numChannels; ich++ )
         {
            double v; P::FromSample( v, i[ich] );
            intensity += v * lumCoeffs[ich];
         }
         double rescaledintensity = (intensity - instance.p_blackpoint)/(maxActual - instance.p_blackpoint);
         double arcsinhScale = 1;
         if ( asinh_parm != 0 )
            if ( rescaledintensity != 0 )
               arcsinhScale = ArcSinh( asinh_parm*rescaledintensity )/denominator/rescaledintensity;

         for ( int ich = 0; ich < numChannels; ich++ )
         {
            double v; P::FromSample( v, i[ich] );
            double newval = (v - instance.p_blackpoint)/(maxActual - instance.p_blackpoint) * arcsinhScale * multiplier;
            if ( preview_mode )  // Preview will always be 16 bit integer UInt16Image (we hope!)
            {
               // In preview the value P::MaxSampleValue() is reserved for
               // highlighting values clipped to zero.
               typename P::sample v;
               if ( newval < 0 )
                  v = instance.p_previewClipped ? P::MaxSampleValue() : 0;
               else
                  v = Min( P::ToSample( Min( newval, 1.0 ) ), typename P::sample( P::MaxSampleValue()-1 ) );
               i[ich] = v;
            }
            else
               i[ich] = P::ToSample( Range( newval, 0.0, 1.0 ) );
         }
      }

      image.ResetSelections();
   }
};

// ----------------------------------------------------------------------------

void ArcsinhStretchInstance::Preview( UInt16Image& image ) const
{
   try
   {
      ArcsinhStretchEngine::Apply( image, *this, true/*preview_mode*/ );
   }
   catch ( ... )
   {
   }
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   ArcsinhStretchEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* ArcsinhStretchInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheArcsinhStretchParameter )
      return &p_stretch;
   if ( p == TheArcsinhStretchBlackPointParameter )
      return &p_blackpoint;
   if ( p == TheArcsinhStretchProtectHighlightsParameter )
      return &p_protectHighlights;
   if ( p == TheArcsinhStretchUseRgbwsParameter )
      return &p_useRgbws;
   if ( p == TheArcsinhStretchPreviewClippedParameter )
      return &p_previewClipped;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchInstance.cpp - Released 2017-09-20T13:03:37Z
