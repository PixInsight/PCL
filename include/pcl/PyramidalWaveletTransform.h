// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/PyramidalWaveletTransform.h - Released 2014/10/29 07:34:13 UTC
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

#ifndef __PCL_PyramidalWaveletTransform_h
#define __PCL_PyramidalWaveletTransform_h

/// \file pcl/PyramidalWaveletTransform.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_ImageTransformation_h
#include <pcl/ImageTransformation.h>
#endif

#ifndef __PCL_Vector_h
#include <pcl/Vector.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class WaveletFilter
 * \brief Abstract base class of all orthogonal wavelet filters.
 *
 * ### TODO: Write a detailed description for %WaveletFilter.
 */
class PCL_CLASS WaveletFilter
{
public:

   /*!
    * Constructs a default %WaveletFilter object.
    */
   WaveletFilter() : Kc ( 0 ), Kr(), N( 0 ), ioff( 0 ), joff( 0 )
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
   virtual const char* Name() const = 0;

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

   void Initialize();

private:

   void Apply( float*, size_type, bool ) const;
   void Apply( double*, size_type, bool ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies4Filter
 * \brief Daubechies-4 wavelet filter.
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
   virtual const char* Name() const
   {
      return "Daubechies-4";
   }
};

extern PCL_DATA Daubechies4Filter Daubechies4;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies6Filter
 * \brief Daubechies-6 wavelet filter.
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
   virtual const char* Name() const
   {
      return "Daubechies-6";
   }
};

extern PCL_DATA Daubechies6Filter Daubechies6;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies8Filter
 * \brief Daubechies-8 wavelet filter.
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
   virtual const char* Name() const
   {
      return "Daubechies-8";
   }
};

extern PCL_DATA Daubechies8Filter Daubechies8;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies10Filter
 * \brief Daubechies-10 wavelet filter.
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
   virtual const char* Name() const
   {
      return "Daubechies-10";
   }
};

extern PCL_DATA Daubechies10Filter Daubechies10;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies12Filter
 * \brief Daubechies-12 wavelet filter.
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
   virtual const char* Name() const
   {
      return "Daubechies-12";
   }
};

extern PCL_DATA Daubechies12Filter Daubechies12;

// ----------------------------------------------------------------------------

/*!
 * \class Daubechies20Filter
 * \brief Daubechies-20 wavelet filter.
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
   virtual const char* Name() const
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
   BidirectionalImageTransformation(), scalingFunction( 0 ), nonstandard( true ), w( 0 )
   {
   }

   /*!
    * Constructs a %PyramidalWaveletTransform instance with the specified
    * wavelet filter \a f.
    */
   PyramidalWaveletTransform( const WaveletFilter& f ) :
   BidirectionalImageTransformation(), scalingFunction( &f ), nonstandard( true ), w( 0 )
   {
      PCL_CHECK( scalingFunction != 0 )
   }

   /*!
    * Copy constructor.
    *
    * \note This constructor <em>does not copy the existing wavelet
    * transform</em> in the source object \a x. It just creates a duplicate
    * of current operating parameters in \a x: wavelet filter and transform
    * type (standard or nonstandard transform).
    */
   PyramidalWaveletTransform( const PyramidalWaveletTransform& x ) :
   BidirectionalImageTransformation( x ),
   scalingFunction( x.scalingFunction ), nonstandard( x.nonstandard ), w( 0 )
   {
   }

   /*!
    * Destroys this %PyramidalWaveletTransform object. The existing wavelet
    * transform is destroyed and deallocated.
    */
   virtual ~PyramidalWaveletTransform()
   {
      Clear();
   }

   /*!
    * Assignment operator.
    *
    * \note The existing wavelet transform in the source object \a x is
    * <em>not assigned</em>; only operating parameters are assigned:
    * wavelet filter and transform type (standard/nonstandard).
    *
    * \note As a consequence of this assignment operator, the existing wavelet
    * transform in this instance is destroyed.
    */
   PyramidalWaveletTransform& operator =( const PyramidalWaveletTransform& x )
   {
      if ( &x != this )
      {
         Clear();
         scalingFunction = x.scalingFunction;
         nonstandard = x.nonstandard;
      }
      return *this;
   }

   /*!
    * Returns a reference to the <em>scaling function</em> (or
    * <em>wavelet filter</em>) used by this wavelet transform.
    */
   const WaveletFilter& ScalingFunction() const
   {
      PCL_PRECONDITION( scalingFunction != 0 )
      return *scalingFunction;
   }

   /*!
    * Sets the <em>scaling function</em> (or <em>wavelet filter</em>) used by
    * this wavelet transform.
    */
   void SetScalingFunction( const WaveletFilter& f )
   {
      Clear();
      scalingFunction = &f;
      PCL_CHECK( scalingFunction != 0 )
   }

   /*!
    * Returns true if this object applies <em>standard wavelet transforms</em>;
    * false if it applies <em>nonstandard wavelet transforms</em>.
    */
   bool IsStandard() const
   {
      return !nonstandard;
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
      return nonstandard;
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
      nonstandard = !b;
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
      nonstandard = b;
   }

   /*!
    * Returns a constant reference to the wavelet transform in this
    * %PyramidalWaveletTransform object.
    *
    * The returned image is empty if no transform has been performed by this
    * %PyramidalWaveletTransform object.
    */
   const Image& Transform() const
   {
      PCL_PRECONDITION( w != 0 )
      return *w;
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
      PCL_PRECONDITION( w != 0 )
      return *w;
   }

   /*!
    * A synonym for Transform() const;
    */
   const Image& operator *() const
   {
      PCL_PRECONDITION( w != 0 )
      return Transform();
   }

   /*!
    * A synonym for Transform();
    */
   Image& operator *()
   {
      PCL_PRECONDITION( w != 0 )
      return Transform();
   }

   /*!
    * Returns a pointer to the wavelet transform in this
    * %PyramidalWaveletTransform object, whose ownership is transferred to the
    * caller.
    *
    * If no wavelet transform has been performed, this function returns zero.
    *
    * The caller is responsible for deallocation of the returned wavelet
    * transform image. After calling this function, this object will be empty
    * (no transform).
    */
   Image* ReleaseTransform()
   {
      Image* w1 = w;
      w = 0;
      return w1;
   }

   /*!
    * Destroys and deallocates the existing wavelet transform.
    */
   void Clear()
   {
      if ( w != 0 )
         delete w, w = 0;
   }

protected:

   /*
    * Scaling function, or wavelet filter.
    */
   const WaveletFilter* scalingFunction;

   /*
    * Flag true if nonstandard transforms are being used.
    * Standard transforms are used otherwise (and by default).
    */
   bool nonstandard : 1;

   /*
    * Wavelet transform
    */
   pcl::Image* w;

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

// ****************************************************************************
// EOF pcl/PyramidalWaveletTransform.h - Released 2014/10/29 07:34:13 UTC
