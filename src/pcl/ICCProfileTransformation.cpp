//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/ICCProfileTransformation.cpp - Released 2017-06-28T11:58:42Z
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

#include <pcl/ICCProfileTransformation.h>
#include <pcl/Thread.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#include <lcms/lcms2.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void ICCProfileTransformation::AddAt( size_type i, const String& profilePath )
{
   CloseTransformation();
   AddOrReplaceAt( i, ICCProfile::Open( profilePath ) );
}

void ICCProfileTransformation::AddAt( size_type i, const ICCProfile& profile )
{
   CloseTransformation();
   AddOrReplaceAt( i, profile.Open() );
}

void ICCProfileTransformation::AddAt( size_type i, const ICCProfile::handle profileHandle )
{
   CloseTransformation();
   if ( !ICCProfile::IsValidHandle( profileHandle ) )
      throw Error( String().Format( "Invalid ICC profile handle %p", profileHandle ) );
   AddOrReplaceAt( i, profileHandle );
}

// Internal
void ICCProfileTransformation::AddOrReplaceAt( size_type i, const ICCProfile::handle h )
{
   for ( size_type n = m_profiles.Length(); n <= i; ++n )
      m_profiles.Add( 0 );
   m_profiles[i] = h;
}

// ----------------------------------------------------------------------------

void ICCProfileTransformation::Add( const String& profilePath )
{
   AddAt( m_profiles.Length(), profilePath );
}

void ICCProfileTransformation::Add( const ICCProfile& profile )
{
   AddAt( m_profiles.Length(), profile );
}

void ICCProfileTransformation::Add( const ICCProfile::handle profileHandle )
{
   AddAt( m_profiles.Length(), profileHandle );
}

// ----------------------------------------------------------------------------

void ICCProfileTransformation::Clear()
{
   CloseTransformation();

   for ( ICCProfile::handle h : m_profiles )
      if ( ICCProfile::IsValidHandle( h ) )
         ICCProfile::Close( h );

   m_profiles.Clear();
   m_srcRGB = m_dstRGB = false;
}

// ----------------------------------------------------------------------------

static uint32 PCLRenderingIntentToLCMS( ICCProfileTransformation::rendering_intent intent )
{
   switch ( intent )
   {
   case ICCRenderingIntent::Perceptual:           return INTENT_PERCEPTUAL;
   default: // ?!
   case ICCRenderingIntent::RelativeColorimetric: return INTENT_RELATIVE_COLORIMETRIC;
   case ICCRenderingIntent::Saturation:           return INTENT_SATURATION;
   case ICCRenderingIntent::AbsoluteColorimetric: return INTENT_ABSOLUTE_COLORIMETRIC;
   }
}

void ICCProfileTransformation::CreateTransformation( bool floatingPoint ) const
{
   CloseTransformation();

   if ( m_profiles.Length() < 2 )
      throw Error( "Two or more profiles are required to create an ICC color transformation." );

   if ( m_proofingTransformation && m_profiles.Length() != 3 )
      throw Error( "A proofing ICC color transformation requires exactly three ICC profiles." );

   for ( ICCProfile::handle h : m_profiles )
      if ( !ICCProfile::IsValidHandle( h ) )
         throw Error( String().Format( "Cannot create an ICC color transformation: Invalid ICC profile handle %p.", h ) );

   m_srcRGB = ICCProfile::ColorSpace( *m_profiles.Begin() ) != ICCColorSpace::Gray;
   m_dstRGB = ICCProfile::ColorSpace( *m_profiles.ReverseBegin() ) != ICCColorSpace::Gray;
   m_floatingPoint = floatingPoint;

   uint32 srcFormat = m_srcRGB ?
      (m_floatingPoint ? TYPE_RGB_DBL : TYPE_RGB_16) : (m_floatingPoint ? TYPE_GRAY_DBL : TYPE_GRAY_16);
   uint32 dstFormat = m_dstRGB ?
      (m_floatingPoint ? TYPE_RGB_DBL : TYPE_RGB_16) : (m_floatingPoint ? TYPE_GRAY_DBL : TYPE_GRAY_16);

   uint32 flags = 0;

   if ( m_blackPointCompensation )
      flags |= cmsFLAGS_BLACKPOINTCOMPENSATION;

   if ( m_highResolutionCLUT )
      flags |= cmsFLAGS_HIGHRESPRECALC;
   else if ( m_lowResolutionCLUT )
      flags |= cmsFLAGS_LOWRESPRECALC;

   if ( m_gamutCheck )
      flags |= cmsFLAGS_GAMUTCHECK;

   if ( m_proofingTransformation )
      m_transformation = ::cmsCreateProofingTransform(
                                       m_profiles[0],
                                       srcFormat,
                                       m_profiles[2],
                                       dstFormat,
                                       m_profiles[1],
                                       PCLRenderingIntentToLCMS( m_intent ),
                                       PCLRenderingIntentToLCMS( m_proofingIntent ),
                                       flags|cmsFLAGS_SOFTPROOFING );
   else if ( m_profiles.Length() == 2 )
      m_transformation = ::cmsCreateTransform(
                                       m_profiles[0],
                                       srcFormat,
                                       m_profiles[1],
                                       dstFormat,
                                       PCLRenderingIntentToLCMS( m_intent ),
                                       flags );
   else
      m_transformation = ::cmsCreateMultiprofileTransform(
                                       const_cast<void**>( m_profiles.Begin() ),
                                       uint32( m_profiles.Length() ),
                                       srcFormat,
                                       dstFormat,
                                       PCLRenderingIntentToLCMS( m_intent ),
                                       flags );

   if ( m_transformation == nullptr )
      ICCProfile::ThrowErrorWithCMSInfo( "Failure to initialize ICC color profile transformation." );
}

// ----------------------------------------------------------------------------

void ICCProfileTransformation::CloseTransformation() const
{
   if ( m_transformation != nullptr )
   {
      ::cmsDeleteTransform( m_transformation );
      m_transformation = nullptr;
      m_floatingPoint = false;
   }
}

// ----------------------------------------------------------------------------

class PCL_ICCProfileTransformationEngine
{
public:

   template <class P, class S> static
   void Apply( GenericImage<P>& image, S*,
               const ICCProfileTransformation& T,
               ICCProfileTransformation::transformation_handle transformation )
   {
      if ( image.IsEmptySelection() || T.Profiles().IsEmpty() )
         return;

      if ( image.ColorSpace() != ColorSpace::RGB )
         if ( image.ColorSpace() != ColorSpace::Gray )
            throw Error( String().Format( "Unsupported color space %X in ICC color transformation.", image.ColorSpace() ) );

      image.EnsureUnique();

      Rect r = image.SelectedRectangle();
      int h = r.Height();

      int numberOfThreads = T.IsParallelProcessingEnabled() ? Min( T.MaxProcessors(), pcl::Thread::NumberOfThreads( h, 1 ) ) : 1;
      int rowsPerThread = h/numberOfThreads;

      size_type N = image.NumberOfSelectedPixels();
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "In-place ICC color profile transformation", N );

      ThreadData<P> data( image, T, transformation, N );

      ReferenceArray<Thread<P,S> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new Thread<P,S>( data,
                                       i*rowsPerThread,
                                       (j < numberOfThreads) ? j*rowsPerThread : h ) );

      AbstractImage::RunThreads( threads, data );

      threads.Destroy();

      image.Status() = data.status;
   }

private:

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image,
                  const ICCProfileTransformation& instance,
                  ICCProfileTransformation::transformation_handle t, size_type a_count ) :
         AbstractImage::ThreadData( a_image, a_count ),
         image( a_image ), T( instance ), transformation( t )
      {
      }

            GenericImage<P>&                                image;
      const ICCProfileTransformation&                       T;
            ICCProfileTransformation::transformation_handle transformation;
   };

   template <class P, class S>
   struct Thread : public pcl::Thread
   {
      Thread( ThreadData<P>& data, int firstRow, int endRow ) :
         pcl::Thread(), m_data( data ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         Rect r = m_data.image.SelectedRectangle();
         int width = r.Width();

         Array<typename S::sample> srcColors( m_data.T.IsSourceRGBProfile() ? 3*width : width );
         Array<typename S::sample> dstColors( m_data.T.IsTargetRGBProfile() ? 3*width : width );

         typename P::sample* R = m_data.image.PixelAddress( r.x0, m_firstRow, 0 );
         typename P::sample* G = m_data.image.PixelAddress( r.x0, m_firstRow, m_data.image.IsColor() ? 1 : 0 );
         typename P::sample* B = m_data.image.PixelAddress( r.x0, m_firstRow, m_data.image.IsColor() ? 2 : 0 );

         for ( int y = m_firstRow, dw = m_data.image.Width()-width; y < m_endRow; ++y, R += dw, G += dw, B += dw )
         {
            typename P::sample* R1 = R;
            typename P::sample* G1 = G;
            typename P::sample* B1 = B;

            for ( int x = 0, p = 0; x < width; ++x )
               if ( m_data.T.IsSourceRGBProfile() )
               {
                  if ( m_data.image.IsColor() )
                  {
                     P::FromSample( srcColors[p++], *R1++ );
                     P::FromSample( srcColors[p++], *G1++ );
                     P::FromSample( srcColors[p++], *B1++ );
                  }
                  else
                  {
                     P::FromSample( srcColors[p], *R1++ );
                     srcColors[p+1] = srcColors[p+2] = srcColors[p];
                     p += 3;
                  }
               }
               else
               {
                  if ( m_data.image.IsColor() )
                  {
                     typename RGBColorSystem::sample r, g, b;
                     P::FromSample( r, *R1++ );
                     P::FromSample( g, *G1++ );
                     P::FromSample( b, *B1++ );
                     srcColors[p++] = S::ToSample( m_data.image.RGBWorkingSpace().Lightness( r, g, b ) );
                  }
                  else
                     P::FromSample( srcColors[p++], *R1++ );
               }

            ::cmsDoTransform( m_data.transformation, srcColors.Begin(), dstColors.Begin(), width );

            for ( int x = 0, p = 0; x < width; ++x, ++R, ++G, ++B )
               if ( m_data.T.IsTargetRGBProfile() )
               {
                  if ( m_data.image.IsColor() )
                  {
                     *R = P::ToSample( dstColors[p++] );
                     *G = P::ToSample( dstColors[p++] );
                     *B = P::ToSample( dstColors[p++] );
                  }
                  else
                  {
                     typename RGBColorSystem::sample r, g, b;
                     S::FromSample( r, dstColors[p++] );
                     S::FromSample( g, dstColors[p++] );
                     S::FromSample( b, dstColors[p++] );
                     *R = P::ToSample( m_data.image.RGBWorkingSpace().Lightness( r, g, b ) );
                  }
               }
               else
               {
                  *R = P::ToSample( dstColors[p++] );
                  if ( m_data.image.IsColor() )
                     *G = *B = *R;
               }

            UPDATE_THREAD_MONITOR_CHUNK( 65536, width )
         }
      }

   private:

      ThreadData<P>& m_data;
      int            m_firstRow;
      int            m_endRow;
   };
};

// ----------------------------------------------------------------------------

void ICCProfileTransformation::Apply( pcl::Image& image ) const
{
   if ( m_transformation == nullptr || !m_floatingPoint )
      CreateTransformation( true/*floatingPoint*/ );

   PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::DImage& image ) const
{
   if ( m_transformation == nullptr || !m_floatingPoint )
      CreateTransformation( true/*floatingPoint*/ );

   PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::UInt8Image& image ) const
{
   if ( m_transformation == nullptr )
      CreateTransformation( m_forceFloatingPoint );

   if ( m_floatingPoint )
      PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
   else
      PCL_ICCProfileTransformationEngine::Apply( image, (UInt16PixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::UInt16Image& image ) const
{
   if ( m_transformation == nullptr )
      CreateTransformation( m_forceFloatingPoint );

   if ( m_floatingPoint )
      PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
   else
      PCL_ICCProfileTransformationEngine::Apply( image, (UInt16PixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::UInt32Image& image ) const
{
   if ( m_transformation == nullptr || !m_floatingPoint )
      CreateTransformation( true/*floatingPoint*/ );

   PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ICCProofingTransformation::SetSourceProfile( const String& profilePath )
{
   AddAt( 0, profilePath );
}

void ICCProofingTransformation::SetSourceProfile( const ICCProfile& profile )
{
   AddAt( 0, profile );
}

void ICCProofingTransformation::SetSourceProfile( const ICCProfile::handle profileHandle )
{
   AddAt( 0, profileHandle );
}

void ICCProofingTransformation::SetProofingProfile( const String& profilePath )
{
   AddAt( 1, profilePath );
}

void ICCProofingTransformation::SetProofingProfile( const ICCProfile& profile )
{
   AddAt( 1, profile );
}

void ICCProofingTransformation::SetProofingProfile( const ICCProfile::handle profileHandle )
{
   AddAt( 1, profileHandle );
}

void ICCProofingTransformation::SetTargetProfile( const String& profilePath )
{
   AddAt( 2, profilePath );
}

void ICCProofingTransformation::SetTargetProfile( const ICCProfile& profile )
{
   AddAt( 2, profile );
}

void ICCProofingTransformation::SetTargetProfile( const ICCProfile::handle profileHandle )
{
   AddAt( 2, profileHandle );
}

RGBA ICCProofingTransformation::GamutWarningColor()
{
   Array<uint16> gw( size_type( cmsMAXCHANNELS ) );
   ::cmsGetAlarmCodes( gw.Begin() );
   return RGBAColor( gw[0]/65535.0, gw[1]/65535.0, gw[2]/65535.0 );
}

void ICCProofingTransformation::SetGamutWarningColor( RGBA color )
{
   Array<uint16> gw( size_type( cmsMAXCHANNELS ), uint16( 0 ) );
   gw[0] = uint16( RoundInt( Red( color )/255.0*65535 ) );
   gw[1] = uint16( RoundInt( Green( color )/255.0*65535 ) );
   gw[2] = uint16( RoundInt( Blue( color )/255.0*65535 ) );
   ::cmsSetAlarmCodes( gw.Begin() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ICCProfileTransformation.cpp - Released 2017-06-28T11:58:42Z
