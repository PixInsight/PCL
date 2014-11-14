// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/ICCProfileTransformation.cpp - Released 2014/11/14 17:17:00 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/ICCProfileTransformation.h>
#include <pcl/Thread.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ICCProfileTransformation::ICCProfileTransformation() :
ImageTransformation(),
m_transformation( 0 ),
m_profiles(),
m_intent( ICCRenderingIntent::Perceptual ),
m_proofingIntent( ICCRenderingIntent::AbsoluteColorimetric ),
m_blackPointCompensation( false ),
m_forceFloatingPoint( false ),
m_highResolutionCLUT( true ),
m_lowResolutionCLUT( false ),
m_proofingTransformation( false ),
m_gamutCheck( false ),
m_parallel( true ),
m_maxProcessors( PCL_MAX_PROCESSORS ),
m_srcRGB( false ),
m_dstRGB( false ),
m_floatingPoint( false )
{
}

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
      throw Error( String().Format( "Invalid ICC profile handle %X", profileHandle ) );
   AddOrReplaceAt( i, profileHandle );
}

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

   for ( profile_list::iterator i = m_profiles.Begin(); i != m_profiles.End(); ++i )
      if ( ICCProfile::IsValidHandle( *i ) )
         ICCProfile::Close( *i );

   m_profiles.Clear();
   m_srcRGB = m_dstRGB = false;
}

// ----------------------------------------------------------------------------

void ICCProfileTransformation::CreateTransformation( bool floatingPoint ) const
{
   CloseTransformation();

   if ( m_profiles.Length() < 2 )
      throw Error( "Insufficient profiles to create an ICC color transformation." );

   if ( m_proofingTransformation && m_profiles.Length() != 3 )
      throw Error( "A proofing ICC color transformation requires three ICC profiles." );

   for ( size_type i = 0; i < m_profiles.Length(); ++i )
      if ( m_profiles[i] == 0 )
         throw Error( "Cannot create an ICC color transformation due to invalid or undefined ICC profile(s)." );

   m_srcRGB = (*API->ColorManagement->GetProfileColorSpace)( *m_profiles.Begin() ) != int32( ICCColorSpace::Gray );
   m_dstRGB = (*API->ColorManagement->GetProfileColorSpace)( *m_profiles.ReverseBegin() ) != int32( ICCColorSpace::Gray );

   m_floatingPoint = floatingPoint;

   int32 srcFormat = m_srcRGB ?
      (m_floatingPoint ? ColorManagementContext::RGB_F64 : ColorManagementContext::RGB_I16) :
      (m_floatingPoint ? ColorManagementContext::Gray_F64 : ColorManagementContext::Gray_I16);

   int32 dstFormat = m_dstRGB ?
      (m_floatingPoint ? ColorManagementContext::RGB_F64 : ColorManagementContext::RGB_I16) :
      (m_floatingPoint ? ColorManagementContext::Gray_F64 : ColorManagementContext::Gray_I16);

   uint32 flags = 0;

   if ( m_blackPointCompensation )
      flags |= ColorManagementContext::BlackPointCompensation;

   if ( m_highResolutionCLUT )
      flags |= ColorManagementContext::HighResolutionCLUT;
   else if ( m_lowResolutionCLUT )
      flags |= ColorManagementContext::LowResolutionCLUT;

   if ( m_gamutCheck )
      flags |= ColorManagementContext::GamutCheck;

   if ( m_proofingTransformation )
      m_transformation = (*API->ColorManagement->CreateProofingTransformation)(
                                       m_profiles[0],
                                       m_profiles[2],
                                       m_profiles[1],
                                       srcFormat,
                                       dstFormat,
                                       m_intent,
                                       m_proofingIntent,
                                       flags );
   else if ( m_profiles.Length() == 2 )
      m_transformation = (*API->ColorManagement->CreateTransformation)(
                                       m_profiles[0],
                                       m_profiles[1],
                                       srcFormat,
                                       dstFormat,
                                       m_intent,
                                       flags );
   else
      m_transformation = (*API->ColorManagement->CreateMultiprofileTransformation)(
                                       (icc_profile_handle*)m_profiles.Begin(),
                                       uint32( m_profiles.Length() ),
                                       srcFormat,
                                       dstFormat,
                                       m_intent,
                                       flags );

   if ( m_transformation == 0 )
   {
      size_type len;
      (void)(*API->ColorManagement->GetLastErrorMessage)( 0, &len );

      String apiMessage;
      if ( len > 0 )
      {
         apiMessage.Reserve( len );
         if ( (*API->ColorManagement->GetLastErrorMessage)( apiMessage.c_str(), &len ) == api_false )
            apiMessage.Clear();
      }

      if ( apiMessage.IsEmpty() )
         apiMessage = '.';
      else
         apiMessage.Prepend( ": " );

      throw Error( "Error initializing ICC color profile transformation" + apiMessage );
   }
}

// ----------------------------------------------------------------------------

void ICCProfileTransformation::CloseTransformation() const
{
   if ( m_transformation != 0 )
   {
      transformation_handle t = m_transformation;
      m_transformation = 0;
      m_floatingPoint = false;
      if ( (*API->ColorManagement->DestroyTransformation)( t ) == api_false )
         throw APIFunctionError( "DestroyTransformation" );
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

      if ( image.ColorSpace() != ColorSpace::RGB && image.ColorSpace() != ColorSpace::Gray )
         throw Error( String().Format( "Unsupported color space %X in ICC color transformation.", image.ColorSpace() ) );

      image.SetUnique();

      Rect r = image.SelectedRectangle();
      int h = r.Height();

      int numberOfThreads = T.IsParallelProcessingEnabled() ? Min( T.MaxProcessors(), pcl::Thread::NumberOfThreads( h, 1 ) ) : 1;
      int rowsPerThread = h/numberOfThreads;

      size_type N = image.NumberOfSelectedPixels();
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "In-place ICC color profile transformation", N );

      ThreadData<P> data( image, T, transformation, N );

      PArray<Thread<P, S> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new Thread<P, S>( data,
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

            (void)(*API->ColorManagement->Transfrom)( m_data.transformation,
                                                      srcColors.Begin(),
                                                      dstColors.Begin(),
                                                      width );

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
   if ( m_transformation == 0 || !m_floatingPoint )
      CreateTransformation( true );

   PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::DImage& image ) const
{
   if ( m_transformation == 0 || !m_floatingPoint )
      CreateTransformation( true );

   PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::UInt8Image& image ) const
{
   if ( m_transformation == 0 )
      CreateTransformation( m_forceFloatingPoint );

   if ( m_floatingPoint )
      PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
   else
      PCL_ICCProfileTransformationEngine::Apply( image, (UInt16PixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::UInt16Image& image ) const
{
   if ( m_transformation == 0 )
      CreateTransformation( m_forceFloatingPoint );

   if ( m_floatingPoint )
      PCL_ICCProfileTransformationEngine::Apply( image, (DoublePixelTraits*)0, *this, m_transformation );
   else
      PCL_ICCProfileTransformationEngine::Apply( image, (UInt16PixelTraits*)0, *this, m_transformation );
}

void ICCProfileTransformation::Apply( pcl::UInt32Image& image ) const
{
   if ( m_transformation == 0 || !m_floatingPoint )
      CreateTransformation( true );

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
   float r, g, b;
   (*API->ColorManagement->GetGamutWarningColor)( &r, &g, &b );
   return RGBAColor( r, g, b );
}

void ICCProofingTransformation::SetGamutWarningColor( RGBA color )
{
   (*API->ColorManagement->SetGamutWarningColor)( Red( color )/255.0,
                                                  Green( color )/255.0,
                                                  Blue( color )/255.0 );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/ICCProfileTransformation.cpp - Released 2014/11/14 17:17:00 UTC
