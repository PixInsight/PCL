// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard NoiseGeneration Process Module Version 01.00.02.0208
// ****************************************************************************
// NoiseGeneratorInstance.cpp - Released 2014/11/14 17:19:23 UTC
// ****************************************************************************
// This file is part of the standard NoiseGeneration PixInsight module.
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

#include "NoiseGeneratorInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ImageWindow.h>
#include <pcl/Random.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

NoiseGeneratorInstance::NoiseGeneratorInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_amount( TheNGNoiseAmountParameter->DefaultValue() ),
p_distribution( NGNoiseDistribution::Default ),
p_impulsionalNoiseProbability( TheNGImpulsionalNoiseProbabilityParameter->DefaultValue() ),
p_preserveBrightness( false /*NGPreserveBrightness::Default*/ ) // ### deprecated
{
}

// ----------------------------------------------------------------------------

NoiseGeneratorInstance::NoiseGeneratorInstance( const NoiseGeneratorInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void NoiseGeneratorInstance::Assign( const ProcessImplementation& p )
{
   const NoiseGeneratorInstance* x = dynamic_cast<const NoiseGeneratorInstance*>( &p );
   if ( x != 0 )
   {
      p_amount = x->p_amount;
      p_distribution = x->p_distribution;
      p_impulsionalNoiseProbability = x->p_impulsionalNoiseProbability;
      p_preserveBrightness = x->p_preserveBrightness; // ### deprecated
   }
}

// ----------------------------------------------------------------------------

bool NoiseGeneratorInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "NoiseGenerator cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

class NoiseGeneratorEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const NoiseGeneratorInstance& G )
   {
      String sdist;
      switch ( G.p_distribution )
      {
      default:
      case NGNoiseDistribution::Uniform:     sdist = "uniform distribution"; break;
      case NGNoiseDistribution::Normal:      sdist = "normal distribution"; break;
      case NGNoiseDistribution::Poisson:     sdist = "Poisson distribution"; break;
      case NGNoiseDistribution::Impulsional: sdist = String().Format( "impulsional, probability=%.*f",
            TheNGImpulsionalNoiseProbabilityParameter->Precision(), G.p_impulsionalNoiseProbability ); break;
      }

      size_type N = image.NumberOfNominalSamples();

      image.Status().Initialize( "Generating noise, " + sdist, N );
      image.Status().DisableInitialization();

      RandomNumberGenerator R;
      double a = G.p_amount;

      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
         for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
         {
            double v; P::FromSample( v, *i );
            switch ( G.p_distribution )
            {
            default:
            case NGNoiseDistribution::Uniform:
               v += a*(R() - 0.5);
               break;
            case NGNoiseDistribution::Normal:
               v += R.Normal( 0.0, a );
               break;
            case NGNoiseDistribution::Poisson:
               v = double( R.Poisson( v*65535/a ) )/(65535/a);
               break;
            case NGNoiseDistribution::Impulsional:
               if ( R() <= G.p_impulsionalNoiseProbability )
                  v += (R() >= 0.5) ? a : -a;
               else
                  continue;
               break;
            }

            *i = P::ToSample( Range( v, 0.0, 1.0 ) );
         }
   }
};

bool NoiseGeneratorInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( !image.IsComplexSample() )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: NoiseGeneratorEngine::Apply( static_cast<Image&>( *image ), *this ); break;
         case 64: NoiseGeneratorEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: NoiseGeneratorEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
         case 16: NoiseGeneratorEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
         case 32: NoiseGeneratorEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
         }

   return true;
}

// ----------------------------------------------------------------------------

void* NoiseGeneratorInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheNGNoiseAmountParameter )
      return &p_amount;
   if ( p == TheNGNoiseDistributionParameter )
      return &p_distribution;
   if ( p == TheNGImpulsionalNoiseProbabilityParameter )
      return &p_impulsionalNoiseProbability;
   if ( p == TheNGPreserveBrightnessParameter ) // ### deprecated
      return &p_preserveBrightness;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF NoiseGeneratorInstance.cpp - Released 2014/11/14 17:19:23 UTC
