//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/PyramidalWaveletTransform.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_PyramidalWaveletTransform_h
#define __PCL_PyramidalWaveletTransform_h

/// \file pcl/PyramidalWaveletTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageTransformation.h>
#include <pcl/String.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class WaveletFilter
 * \brief Abstract base class of all orthogonal wavelet filters.
 *
 * ### TODO: Write a detailed description for %WaveletFilter.
 *
 * \ingroup multiscale_transforms
 */
class PCL_CLASS WaveletFilter
{
public:

   /*!
    * Constructs a default %WaveletFilter object.
    */
   WaveletFilter() : Kc ( nullptr ), Kr(), N( 0 ), ioff( 0 ), joff( 0 )
   {
   }

   /*!
    * Destroys a %WaveletFilter object.
    */
   virtual ~WaveletFilter()
   {
   }

   /*!
    * Returns an identifying name for this wavelet filter.
    */
   virtual String Name() const = 0;

   /*!
    * Performs an in-place one-dimensional wavelet transform.
    *
    * \param[in,out] f  Starting address of the vector to be transformed.
    *
    * \param n          Length of the input vector.
    *
    * \param inverse    True to perform an inverse wavelet transform. False
    *                   (the default value) to perform a direct wavelet
    *                   transform.
    */
   template <typename T>
   void operator ()( T* f, size_type n, bool inverse = false ) const
   {
      Apply( f, n, inverse );
   }

protected:

   const double* Kc;   // Wavelet filter coefficients
   Vector        Kr;   // Transpose filter coefficients
   int           N;    // Number of coefficients
   int           ioff;
   int           joff;

   void Initialize()
   {
      PCL_CHECK( Kc != nullptr && N > 0 )
      Kr = Vector( N );
      for ( int k = 0, sig = -1; k < N; ++k )
      {
         Kr[N-1-k] = sig*Kc[k];
         sig = -sig;
      }
   // ioff = joff = -(N >> 1);
      ioff = -2;
      joff = -N + 2;
   }

private:

   void Apply( float*, size_type, bool ) const;
   void Apply( double*, size_type, bool ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies4Filter
 * \brief Daubechies-4 wavelet filter.
 * \ingroup multiscale_transforms
 */
class PCL_CLASS Daubechies4Filter : public WaveletFilter
{
public:

   /*!
    * Default constructor.
    */
   Daubechies4Filter();

   /*!
    */
   virtual String Name() const
   {
      return "Daubechies-4";
   }
};

extern PCL_DATA Daubechies4Filter Daubechies4;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies6Filter
 * \brief Daubechies-6 wavelet filter.
 * \ingroup multiscale_transforms
 */
class PCL_CLASS Daubechies6Filter : public WaveletFilter
{
public:

   /*!
    * Default constructor.
    */
   Daubechies6Filter();

   /*!
    */
   virtual String Name() const
   {
      return "Daubechies-6";
   }
};

extern PCL_DATA Daubechies6Filter Daubechies6;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies8Filter
 * \brief Daubechies-8 wavelet filter.
 * \ingroup multiscale_transforms
 */
class PCL_CLASS Daubechies8Filter : public WaveletFilter
{
public:

   /*!
    * Default constructor.
    */
   Daubechies8Filter();

   /*!
    */
   virtual String Name() const
   {
      return "Daubechies-8";
   }
};

extern PCL_DATA Daubechies8Filter Daubechies8;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies10Filter
 * \brief Daubechies-10 wavelet filter.
 * \ingroup multiscale_transforms
 */
class PCL_CLASS Daubechies10Filter : public WaveletFilter
{
public:

   /*!
    * Default constructor.
    */
   Daubechies10Filter();

   /*!
    */
   virtual String Name() const
   {
      return "Daubechies-10";
   }
};

extern PCL_DATA Daubechies10Filter Daubechies10;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies12Filter
 * \brief Daubechies-12 wavelet filter.
 * \ingroup multiscale_transforms
 */
class PCL_CLASS Daubechies12Filter : public WaveletFilter
{
public:

   /*!
    * Default constructor.
    */
   Daubechies12Filter();

   /*!
    */
   virtual String Name() const
   {
      return "Daubechies-12";
   }
};

extern PCL_DATA Daubechies12Filter Daubechies12;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies20Filter
 * \brief Daubechies-20 wavelet filter.
 * \ingroup multiscale_transforms
 */
class PCL_CLASS Daubechies20Filter : public WaveletFilter
{
public:

   /*!
    * Default constructor.
    */
   Daubechies20Filter();

   /*!
    */
   virtual String Name() const
   {
      return "Daubechies-20";
   }
};

extern PCL_DATA Daubechies20Filter Daubechies20;

// ----------------------------------------------------------------------------

/*!
 * \class PyramidalWaveletTransform
 * \brief Discrete two-dimensional wavelet transforms by the pyramidal
 * algorithms.
 *
 * \note The dimensions of transformed images must be integer powers of two.
 *
 * <b>References</b>
 *
 * Partially based on <em>Numerical Recipes in C</em>, 2nd Ed., by W. H. Press
 * et al.
 *
 * ### TODO: Write a detailed description for %PyramidalWaveletTransform.
 *
 * \ingroup multiscale_transforms
 */
class PCL_CLASS PyramidalWaveletTransform : public BidirectionalImageTransformation
{
public:

   /*!
    * Default constructor.
    *
    * \note This constructor yields an uninitialized instance that cannot be
    * used prior to initializing it with a reference to a WaveletFilter, which
    * will be used as the scaling function of the wavelet transform.
    */
   PyramidalWaveletTransform() :
      BidirectionalImageTransformation(),
      m_scalingFunction( nullptr ), m_nonstandard( true ), m_transform()
   {
   }

   /*!
    * Constructs a %PyramidalWaveletTransform instance with the specified
    * wavelet filter \a f.
    */
   PyramidalWaveletTransform( const WaveletFilter& f ) :
      BidirectionalImageTransformation(),
      m_scalingFunction( &f ), m_nonstandard( true ), m_transform()
   {
      PCL_CHECK( m_scalingFunction != nullptr )
   }

   /*!
    * Copy constructor.
    */
   PyramidalWaveletTransform( const PyramidalWaveletTransform& x ) :
      BidirectionalImageTransformation( x ),
      m_scalingFunction( x.m_scalingFunction ), m_nonstandard( x.m_nonstandard ), m_transform( x.m_transform )
   {
   }

   /*!
    * Move constructor.
    */
   PyramidalWaveletTransform( PyramidalWaveletTransform&& x ) :
      BidirectionalImageTransformation( x ),
      m_scalingFunction( x.m_scalingFunction ), m_nonstandard( x.m_nonstandard ), m_transform( std::move( x.m_transform ) )
   {
   }

   /*!
    * Destroys this %PyramidalWaveletTransform object. The existing wavelet
    * transform is destroyed and deallocated.
    */
   virtual ~PyramidalWaveletTransform()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PyramidalWaveletTransform& operator =( const PyramidalWaveletTransform& x )
   {
      (void)BidirectionalImageTransformation::operator =( x );
      m_scalingFunction = x.m_scalingFunction;
      m_nonstandard = x.m_nonstandard;
      m_transform = x.m_transform;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   PyramidalWaveletTransform& operator =( PyramidalWaveletTransform&& x )
   {
      (void)BidirectionalImageTransformation::operator =( x );
      m_scalingFunction = x.m_scalingFunction;
      m_nonstandard = x.m_nonstandard;
      m_transform = std::move( x.m_transform );
      return *this;
   }

   /*!
    * Returns a reference to the <em>scaling function</em> (or
    * <em>wavelet filter</em>) used by this wavelet transform.
    */
   const WaveletFilter& ScalingFunction() const
   {
      PCL_PRECONDITION( m_scalingFunction != nullptr )
      return *m_scalingFunction;
   }

   /*!
    * Sets the <em>scaling function</em> (or <em>wavelet filter</em>) used by
    * this wavelet transform.
    */
   void SetScalingFunction( const WaveletFilter& f )
   {
      Clear();
      m_scalingFunction = &f;
      PCL_CHECK( m_scalingFunction != nullptr )
   }

   /*!
    * Returns true iff this object applies <em>standard wavelet transforms</em>;
    * false if it applies <em>nonstandard wavelet transforms</em>.
    */
   bool IsStandard() const
   {
      return !m_nonstandard;
   }

   /*!
    * Returns true if this object applies <em>nonstandard wavelet
    * transforms</em>; false if it applies <em>nstandard wavelet
    * transforms</em>.
    *
    * This is a convenience member function, equivalent to !IsStandard().
    */
   bool IsNonstandard() const
   {
      return m_nonstandard;
   }

   /*!
    * Enables or disables <em>standard wavelet transforms</em> for this
    * %PyramidalWaveletTransform object.
    *
    * Calling this member function implicitly destroys the existing wavelet
    * transform.
    */
   void SetStandard( bool b = true )
   {
      Clear();
      m_nonstandard = !b;
   }

   /*!
    * Enables or disables <em>nonstandard wavelet transforms</em> for this
    * %PyramidalWaveletTransform object.
    *
    * Calling this member function implicitly destroys the existing wavelet
    * transform.
    *
    * This is a convenience member function, equivalent to SetStandard( !b ).
    */
   void SetNonstandard( bool b = true )
   {
      Clear();
      m_nonstandard = b;
   }

   /*!
    * Returns a reference to the wavelet transform in this
    * %PyramidalWaveletTransform object.
    *
    * The returned image is empty if no transform has been performed by this
    * %PyramidalWaveletTransform object.
    */
   const Image& Transform() const
   {
      return m_transform;
   }

   /*!
    * Returns a reference to the wavelet transform in this
    * %PyramidalWaveletTransform object.
    *
    * The returned image is empty if no transform has been performed by this
    * %PyramidalWaveletTransform object.
    */
   Image& Transform()
   {
      return m_transform;
   }

   /*!
    * Destroys and deallocates the existing wavelet transform.
    */
   void Clear()
   {
      m_transform.FreeData();
   }

protected:

   /*
    * Scaling function, or wavelet filter.
    */
   const WaveletFilter* m_scalingFunction;

   /*
    * Flag true if nonstandard transforms are being used.
    * Standard transforms are used otherwise (and by default).
    */
   bool m_nonstandard : 1;

   /*
    * Wavelet transform
    */
   pcl::Image m_transform;

   void Validate() const;
   void DoTransform( StatusMonitor& );

   // Transform (decomposition)
   virtual void Transform( const pcl::Image& );
   virtual void Transform( const pcl::DImage& );
   virtual void Transform( const pcl::ComplexImage& );
   virtual void Transform( const pcl::DComplexImage& );
   virtual void Transform( const pcl::UInt8Image& );
   virtual void Transform( const pcl::UInt16Image& );
   virtual void Transform( const pcl::UInt32Image& );

   // Inverse transform (reconstruction)
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::ComplexImage& ) const;
   virtual void Apply( pcl::DComplexImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PyramidalWaveletTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/PyramidalWaveletTransform.h - Released 2017-06-28T11:58:36Z
