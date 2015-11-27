//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0345
// ----------------------------------------------------------------------------
// ExponentialTransformationInstance.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ExponentialTransformationInstance.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/FFTConvolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/Mutex.h>
#include <pcl/ReferenceArray.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialTransformationInstance::ExponentialTransformationInstance( const MetaProcess* P ) :
ProcessImplementation( P ),
type( TheExponentialFunctionTypeParameter->ElementValue( TheExponentialFunctionTypeParameter->DefaultValueIndex() ) ),
order( TheExponentialFunctionOrderParameter->DefaultValue() ),
sigma( TheExponentialFunctionSmoothingParameter->DefaultValue() ),
useLightnessMask( TheExponentialFunctionMaskParameter->DefaultValue() )
{
}

ExponentialTransformationInstance::ExponentialTransformationInstance( const ExponentialTransformationInstance& p ) :
ProcessImplementation( p )
{
   Assign( p );
}

// ----------------------------------------------------------------------------

void ExponentialTransformationInstance::Assign( const ProcessImplementation& p )
{
   const ExponentialTransformationInstance* x = dynamic_cast<const ExponentialTransformationInstance*>( &p );
   if ( x != 0 )
   {
      type = x->type;
      order = x->order;
      sigma = x->sigma;
      useLightnessMask = x->useLightnessMask;
   }
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ExponentialTransformation cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ExponentialTransformationEngine
{
public:

   static void Apply( ImageVariant& image, const ExponentialTransformationInstance& instance )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32 : Apply( static_cast<Image&>( *image ), instance ); break;
         case 64 : Apply( static_cast<DImage&>( *image ), instance ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8 : Apply( static_cast<UInt8Image&>( *image ), instance ); break;
         case 16 : Apply( static_cast<UInt16Image&>( *image ), instance ); break;
         case 32 : Apply( static_cast<UInt32Image&>( *image ), instance ); break;
         }
   }

   template <class P>
   static void Apply( GenericImage<P>& image, const ExponentialTransformationInstance& instance )
   {
      AutoPointer<Image> mask;

      if ( instance.sigma > 0 )
      {
         image.ResetSelections();
         image.SelectNominalChannels();
         mask = new Image( image );
         mask->Status().Initialize( "Generating smoothing mask", mask->NumberOfSamples() );
         mask->Status().DisableInitialization();
         mask->SelectNominalChannels();
         GaussianFilter G( instance.sigma );
         if ( G.Size() < PCL_FFT_CONVOLUTION_IS_FASTER_THAN_SEPARABLE_FILTER_SIZE )
            SeparableConvolution( G.AsSeparableFilter() ) >> *mask;
         else
            FFTConvolution( G ) >> *mask;
      }

      size_type N = image.NumberOfPixels();
      int numberOfThreads = Thread::NumberOfThreads( N, 16 );
      size_type pixelsPerThread = N/numberOfThreads;

      image.Status().Initialize( instance.TypeAsString() + " transformation", image.NumberOfNominalSamples() );

      ThreadData data( image, image.NumberOfNominalSamples() );

      if ( instance.order == 1 )
         data.iorder = 1;
      else if ( instance.order == 2 )
         data.iorder = 2;
      else if ( instance.order == 3 )
         data.iorder = 3;
      else if ( instance.order == 4 )
         data.iorder = 4;
      else if ( instance.order == 5 )
         data.iorder = 5;
      else if ( instance.order == 6 )
         data.iorder = 6;
      else if ( instance.order == 0.5 )
         data.iorder = -1;
      else
         data.iorder = 0;

      ReferenceArray<ExponentialThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new ExponentialThread<P>( instance, data, image, mask,
                                                i*pixelsPerThread,
                                                (j < numberOfThreads) ? j*pixelsPerThread : N ) );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
      image.Status() = data.status;
   }

private:

   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const AbstractImage& image, size_type count ) :
      AbstractImage::ThreadData( image, count )
      {
      }

      int iorder;
   };

   template <class P>
   class ExponentialThread : public Thread
   {
   public:

      ExponentialThread( const ExponentialTransformationInstance& instance, const ThreadData& data,
                         GenericImage<P>& image, const Image* mask, size_type start, size_type end ) :
      Thread(), m_instance( instance ), m_data( data ), m_image( image ), m_mask( mask ), m_start( start ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         int nc = m_image.NumberOfNominalChannels();
         typename P::sample* f[ 3 ];
         const float*        m[ 3 ];
         for ( int c = 0; c < nc; ++c )
         {
            f[c] = m_image[c] + m_start;
            m[c] = (m_mask != 0) ? (*m_mask)[c] + m_start : 0;
         }
         typename P::sample* fN = m_image[0] + m_end;

         for ( ; f[0] < fN; )
         {
            double L = 0, L1 = 0;
            if ( m_instance.useLightnessMask )
            {
               if ( nc == 1 )
                  P::FromSample( L, *f[0] );
               else
               {
                  RGBColorSystem::sample R, G, B;
                  P::FromSample( R, *f[0] );
                  P::FromSample( G, *f[1] );
                  P::FromSample( B, *f[2] );
                  L = rgbws.Lightness( R, G, B );
               }
               L1 = 1 - L;
            }

            for ( int c = 0; c < nc; ++c )
            {
               double f0;
               P::FromSample( f0, *f[c] );

               double fm;
               if ( m_mask != 0 )
                  fm = *(m[c])++;
               else
                  fm = f0;
               fm = 1 - fm;
               switch ( m_data.iorder )
               {
               case 6:
                  fm *= fm;
               case 5:
                  fm *= fm;
               case 4:
                  fm *= fm;
               case 3:
                  fm *= fm;
               case 2:
                  fm *= fm;
                  break;
               case 1:
                  break;
               case -1:
                  fm = Sqrt( fm );
                  break;
               default:
                  fm = Pow( fm, double( m_instance.order ) );
                  break;
               }

               double f1;
               switch ( m_instance.type )
               {
               default:
               case ExponentialFunctionType::PIP: f1 = Pow( f0, fm ); break;
               case ExponentialFunctionType::SMI: f1 = 1 - (1 - f0)*fm; break;
               }

               if ( m_instance.useLightnessMask )
                  f1 = L1*f1 + L*f0;

               *(f[c])++ = P::ToSample( f1 );

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      const ExponentialTransformationInstance& m_instance;
      const ThreadData&                        m_data;
      GenericImage<P>&                         m_image;
      const Image*                             m_mask;
      size_type                                m_start;
      size_type                                m_end;
   };
};

// ----------------------------------------------------------------------------

void ExponentialTransformationInstance::Preview( UInt16Image& image ) const
{
   ExponentialTransformationEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   ExponentialTransformationEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* ExponentialTransformationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheExponentialFunctionTypeParameter )
      return &type;
   if ( p == TheExponentialFunctionOrderParameter )
      return &order;
   if ( p == TheExponentialFunctionSmoothingParameter )
      return &sigma;
   if ( p == TheExponentialFunctionMaskParameter )
      return &useLightnessMask;
   return 0;
}

// ----------------------------------------------------------------------------

String ExponentialTransformationInstance::TypeAsString() const
{
   switch ( type )
   {
   case ExponentialFunctionType::PIP: return "PIP";
   case ExponentialFunctionType::SMI: return "SMI";
   }
   return String();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationInstance.cpp - Released 2015/11/26 16:00:13 UTC
