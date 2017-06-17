//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/FourierTransform.cpp - Released 2017-06-17T10:55:56Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/FourierTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define FFTC   GenericFFT2D<typename P::sample::component>
#define FFTR   GenericRealFFT2D<typename P::sample>

// ----------------------------------------------------------------------------

template <class P>
static void ApplyInPlaceFourierTransform( GenericImage<P>& image, FFTDirection::value_type dir, bool parallel, int maxProcessors )
{
   int w = FFTC::OptimizedLength( image.Width() );
   int h = FFTC::OptimizedLength( image.Height() );

   if ( w != image.Width() || h != image.Height() )
   {
      StatusCallback* s = image.GetStatusCallback(); // don't update status here
      image.SetStatusCallback( 0 );
      image.ShiftToCenter( w, h );
      image.SetStatusCallback( s );
   }

   bool statusInitialized = false;
   if ( image.Status().IsInitializationEnabled() )
   {
      image.Status().Initialize( (dir == FFTDirection::Backward) ? "Inverse FFT" : "FFT",
                                 image.NumberOfSelectedChannels()*size_type( w + h ) );
      image.Status().DisableInitialization();
      statusInitialized = true;
   }

   try
   {
      FFTC F( h, w, image.Status() );
      F.EnableParallelProcessing( parallel, maxProcessors );
      for ( int c = image.FirstSelectedChannel(); c <= image.LastSelectedChannel(); ++c )
         F( image[c], image[c], (dir == FFTDirection::Backward) ? PCL_FFT_BACKWARD : PCL_FFT_FORWARD );
      if ( statusInitialized )
         image.Status().EnableInitialization();
   }
   catch ( ... )
   {
      if ( statusInitialized )
         image.Status().EnableInitialization();
      throw;
   }
}

void InPlaceFourierTransform::Apply( ComplexImage& image ) const
{
   ApplyInPlaceFourierTransform( image, m_type, m_parallel, m_maxProcessors );
}

void InPlaceFourierTransform::Apply( DComplexImage& image ) const
{
   ApplyInPlaceFourierTransform( image, m_type, m_parallel, m_maxProcessors );
}

// ----------------------------------------------------------------------------

template <class P, class P1>
static void LoadAndTransformImage( GenericImage<P>& transform, const GenericImage<P1>& image, bool parallel, int maxProcessors )
{
   Rect r = image.SelectedRectangle();
   if ( !r.IsRect() )
      return;

   int w = FFTC::OptimizedLength( r.Width() );
   int h = FFTC::OptimizedLength( r.Height() );

   int dw2 = (w - r.Width()) >> 1;
   int dh2 = (h - r.Height()) >> 1;

   transform.AllocateData( w, h, image.NumberOfSelectedChannels(),
                           (image.NumberOfSelectedChannels() < 3 || image.FirstSelectedChannel() != 0) ?
                                 ColorSpace::Gray : image.ColorSpace() );
   transform.Zero().Move( image, Point( dw2, dh2 ) );

   ApplyInPlaceFourierTransform( transform, FFTDirection::Forward, parallel, maxProcessors );
}

void FourierTransform::Transform( const Image& image )
{
   m_dft.CreateComplexImage( 32 );
   LoadAndTransformImage( static_cast<FComplexImage&>( *m_dft ), image, m_parallel, m_maxProcessors );
}

void FourierTransform::Transform( const DImage& image )
{
   m_dft.CreateComplexImage( 64 );
   LoadAndTransformImage( static_cast<DComplexImage&>( *m_dft ), image, m_parallel, m_maxProcessors );
}

void FourierTransform::Transform( const ComplexImage& image )
{
   m_dft.CreateComplexImage( 32 );
   LoadAndTransformImage( static_cast<FComplexImage&>( *m_dft ), image, m_parallel, m_maxProcessors );
}

void FourierTransform::Transform( const DComplexImage& image )
{
   m_dft.CreateComplexImage( 64 );
   LoadAndTransformImage( static_cast<DComplexImage&>( *m_dft ), image, m_parallel, m_maxProcessors );
}

void FourierTransform::Transform( const UInt8Image& image )
{
   m_dft.CreateComplexImage( 32 );
   LoadAndTransformImage( static_cast<FComplexImage&>( *m_dft ), image, m_parallel, m_maxProcessors );
}

void FourierTransform::Transform( const UInt16Image& image )
{
   m_dft.CreateComplexImage( 32 );
   LoadAndTransformImage( static_cast<FComplexImage&>( *m_dft ), image, m_parallel, m_maxProcessors );
}

void FourierTransform::Transform( const UInt32Image& image )
{
   m_dft.CreateComplexImage( 64 );
   LoadAndTransformImage( static_cast<DComplexImage&>( *m_dft ), image, m_parallel, m_maxProcessors );
}

// ----------------------------------------------------------------------------

template <class P, class P1>
static void ApplyInverseRealFourierTransform_2( GenericImage<P>& image, const GenericImage<P1>& dft, bool parallel, int maxProcessors )
{
   if ( dft.IsEmpty() )
   {
      image.FreeData();
      return;
   }

   int w = dft.Width();
   int h = dft.Height();

   image.AllocateData( 2*(w - 1), h, dft.NumberOfChannels(), dft.ColorSpace() );

   bool statusInitialized = false;
   if ( image.Status().IsInitializationEnabled() )
   {
      image.Status().Initialize( "Inverse FFT", image.NumberOfChannels()*size_type( w + h ) );
      image.Status().DisableInitialization();
      statusInitialized = true;
   }

   try
   {
      FFTR F( h, w, image.Status() );
      F.EnableParallelProcessing( parallel, maxProcessors );
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
         F( image[c], dft[c] );
      if ( statusInitialized )
         image.Status().EnableInitialization();
   }
   catch ( ... )
   {
      if ( statusInitialized )
         image.Status().EnableInitialization();
      throw;
   }
}

template <class P>
static void ApplyInverseRealFourierTransform_1( GenericImage<P>& image, const FComplexImage& dft, bool parallel, int maxProcessors )
{
   FImage tmp;
   tmp.Status() = image.Status();
   image.FreeData();

   ApplyInverseRealFourierTransform_2( tmp, dft, parallel, maxProcessors );

   image.SetStatusCallback( 0 );
   image.Assign( tmp );
   image.Status() = tmp.Status();
}

static void ApplyInverseRealFourierTransform_1( FImage& image, const FComplexImage& dft, bool parallel, int maxProcessors )
{
   ApplyInverseRealFourierTransform_2( image, dft, parallel, maxProcessors );
}

template <class P>
static void ApplyInverseRealFourierTransform_1( GenericImage<P>& image, const DComplexImage& dft, bool parallel, int maxProcessors )
{
   DImage tmp;
   tmp.Status() = image.Status();
   image.FreeData();

   ApplyInverseRealFourierTransform_2( tmp, dft, parallel, maxProcessors );

   image.SetStatusCallback( 0 );
   image.Assign( tmp );
   image.Status() = tmp.Status();
}

static void ApplyInverseRealFourierTransform_1( DImage& image, const DComplexImage& dft, bool parallel, int maxProcessors )
{
   ApplyInverseRealFourierTransform_2( image, dft, parallel, maxProcessors );
}

template <class P>
static void ApplyInverseRealFourierTransform( GenericImage<P>& image, const ImageVariant& dft, bool parallel, int maxProcessors )
{
   if ( !dft || dft->IsEmpty() )
   {
      image.FreeData();
      return;
   }

   switch ( dft.BitsPerSample() )
   {
   case 32: ApplyInverseRealFourierTransform_1( image, static_cast<const FComplexImage&>( *dft ), parallel, maxProcessors ); break;
   case 64: ApplyInverseRealFourierTransform_1( image, static_cast<const DComplexImage&>( *dft ), parallel, maxProcessors ); break;
   }
}

void FourierTransform::Apply( Image& image ) const
{
   ApplyInverseRealFourierTransform( image, m_dft, m_parallel, m_maxProcessors );
}

void FourierTransform::Apply( DImage& image ) const
{
   ApplyInverseRealFourierTransform( image, m_dft, m_parallel, m_maxProcessors );
}

void FourierTransform::Apply( ComplexImage& image ) const
{
   if ( m_dft->IsEmpty() )
   {
      image.FreeData();
      return;
   }

   StatusMonitor monitor = image.Status();
   switch ( m_dft.BitsPerSample() )
   {
   case 32: image.Assign( static_cast<const FComplexImage&>( *m_dft ) ); break;
   case 64: image.Assign( static_cast<const DComplexImage&>( *m_dft ) ); break;
   }
   image.Status() = monitor;

   ApplyInPlaceFourierTransform( image, FFTDirection::Backward, m_parallel, m_maxProcessors );
}

void FourierTransform::Apply( DComplexImage& image ) const
{
   if ( m_dft->IsEmpty() )
   {
      image.FreeData();
      return;
   }

   StatusMonitor monitor = image.Status();
   switch ( m_dft.BitsPerSample() )
   {
   case 32: image.Assign( static_cast<const FComplexImage&>( *m_dft ) ); break;
   case 64: image.Assign( static_cast<const DComplexImage&>( *m_dft ) ); break;
   }
   image.Status() = monitor;

   ApplyInPlaceFourierTransform( image, FFTDirection::Backward, m_parallel, m_maxProcessors );
}

void FourierTransform::Apply( UInt8Image& image ) const
{
   ApplyInverseRealFourierTransform( image, m_dft, m_parallel, m_maxProcessors );
}

void FourierTransform::Apply( UInt16Image& image ) const
{
   ApplyInverseRealFourierTransform( image, m_dft, m_parallel, m_maxProcessors );
}

void FourierTransform::Apply( UInt32Image& image ) const
{
   ApplyInverseRealFourierTransform( image, m_dft, m_parallel, m_maxProcessors );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FourierTransform.cpp - Released 2017-06-17T10:55:56Z
