//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/RedundantMultiscaleTransform.cpp - Released 2015/10/08 11:24:19 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Exception.h>
#include <pcl/RedundantMultiscaleTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Inverse transform (reconstruction)
 */

class MTReconstruction
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const RedundantMultiscaleTransform& T )
   {
      for ( int j = T.m_numberOfLayers; ; --j )
      {
         if ( T.m_transform[j] && T.m_layerEnabled[j] )
         {
            StatusMonitor status = image.Status();
            bool statusInitialized = false;

            image.SetStatusCallback( 0 );

            try
            {
               if ( image.IsEmpty() || image.IsEmptySelection() )
                  image.Assign( T.m_transform[j] );
               else
                  image.Apply( T.m_transform[j] );

               size_type N = image.NumberOfSelectedSamples();

               image.Status() = status;
               if ( image.Status().IsInitializationEnabled() )
               {
                  image.Status().Initialize( "Multiscale reconstruction", T.m_numberOfLayers*N );
                  image.Status().DisableInitialization();
                  statusInitialized = true;
               }

               image.Status() += (T.m_numberOfLayers - j)*N;

               for ( ; --j >= 0; )
                  if ( T.m_transform[j] && T.m_layerEnabled[j] )
                     image += T.m_transform[j];
                  else
                     image.Status() += N;

               if ( statusInitialized )
                  image.Status().EnableInitialization();
            }
            catch ( ... )
            {
               if ( statusInitialized )
                  image.Status().EnableInitialization();
               throw;
            }

            break;
         }

         if ( j == 0 )
         {
            image.Zero();
            break;
         }
      }
   }
}; // MTReconstruction

void RedundantMultiscaleTransform::Apply( Image& image ) const
{
   MTReconstruction::Apply( image, *this );
}

void RedundantMultiscaleTransform::Apply( DImage& image ) const
{
   MTReconstruction::Apply( image, *this );
}

#define IMPLEMENT_RECONSTRUCTION                      \
   F.Status() = image.Status();                       \
   MTReconstruction::Apply( F, *this );               \
   if ( image.IsEmpty() || image.IsEmptySelection() ) \
      image.Assign( F );                              \
   else                                               \
      image.Apply( F ).Status() = F.Status();

void RedundantMultiscaleTransform::Apply( pcl::ComplexImage& image ) const
{
   pcl::Image F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::DComplexImage& image ) const
{
   pcl::DImage F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::UInt8Image& image ) const
{
   pcl::Image F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::UInt16Image& image ) const
{
   pcl::Image F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::UInt32Image& image ) const
{
   pcl::DImage F;
   IMPLEMENT_RECONSTRUCTION
}

#undef IMPLEMENT_RECONSTRUCTION

// ----------------------------------------------------------------------------

void RedundantMultiscaleTransform::ValidateLayerIndex( int j ) const
{
   if ( j < 0 || j > m_numberOfLayers )
      throw Error( "Invalid access to undefined multiscale transform layer." );
}

void RedundantMultiscaleTransform::ValidateLayerAccess( int j ) const
{
   ValidateLayerIndex( j );
   if ( m_transform.IsEmpty() || !m_transform[j] )
      throw Error( "Invalid access to nonexistent multiscale transform layer." );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/RedundantMultiscaleTransform.cpp - Released 2015/10/08 11:24:19 UTC
