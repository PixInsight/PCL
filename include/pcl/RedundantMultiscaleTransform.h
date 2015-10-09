//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/RedundantMultiscaleTransform.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_RedundantMultiscaleTransform_h
#define __PCL_RedundantMultiscaleTransform_h

/// \file pcl/RedundantMultiscaleTransform.h

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
 * \class RedundantMultiscaleTransform
 * \brief Base class of all redundant multiscale transforms.
 *
 * A redundant multiscale transform produces a set {w1,w2,...,wN,cN}, where
 * each wj is a set of coefficients at scale j, which we call <em>detail
 * layer</em>, and cN is a large-scale smoothed residual, which we call
 * <em>residual layer</em>. Each layer has the same dimensions as the input
 * image, so the generated multiscale transform is called \e redundant. Two
 * well-known examples of redundant multiscale transform algorithms are the
 * <em>à trous</em> (with holes) wavelet transform, also known as <em>starlet
 * transform</em>, and the multiscale median transform.
 *
 * In all cases the reconstruction algorithm consists of the sum of all wj
 * multiscale layers for 1 <= j <= N, plus the residual layer cN.
 *
 * In our implementation, each layer in a redundant multiscale transform is a
 * floating-point image with the same dimensions as the transformed image.
 * Layers are indexed from 0 to N. Layers at indexes from 0 to N-1 are
 * detail layers, whose elements are actually difference coefficients. Pixel
 * samples in a detail layer can be negative, zero or positive real values.
 *
 * The last layer, at index N, is the large-scale residual layer. Pixel samples
 * in the residual layer image can only be positive or zero real values.
 *
 * \sa ATrousWaveletTransform, StarletTransform, MultiscaleMedianTransform,
 * MultiscaleLinearTransform
 */
class PCL_CLASS RedundantMultiscaleTransform : public BidirectionalImageTransformation
{
public:

   /*!
    * Represents a multiscale layer.
    */
   typedef Image                 layer;

   /*!
    * Represents a set of multiscale layers, or multiscale transform.
    */
   typedef Array<layer>          transform;

   /*!
    * Represents a set of layer enabled/disabled states.
    */
   typedef GenericVector<bool>   layer_state_set;

   /*!
    * Constructs a redundant multiscale transform.
    *
    * \param n    Number of detail layers. The transform will consist of \a n
    *             detail layers plus a residual layer, that is n+1 total
    *             layers. The default value is 4.
    *
    * \param d    Scaling sequence. If \a d <= 0, the transform will use the
    *             dyadic sequence: 1, 2, 4, ... 2^i. If \a d > 0, its value is
    *             the distance in pixels between two successive scales.
    *
    * The default values for \a n and \a d are 4 and 0, respectively (four
    * layers and the dyadic scaling sequence).
    *
    * Successive layers are computed by applying linear or nonlinear operations
    * with kernels of size 2*s + 1. The scaling sequence parameter \a d is
    * interpreted as follows:
    *
    * - If the specified sequence parameter \a d is zero 0, then the transform
    *   uses the dyadic sequence: s = 1, 2, 4, ..., 2^j for 0 <= j < n.
    *
    * - If \a d > 0, then \a d is the constant increment in pixels between two
    *   successive scales (linear scaling sequence): s = d*j for 1 <= j < n.
    */
   RedundantMultiscaleTransform( int n = 4, int d = 0 ) :
      BidirectionalImageTransformation(),
      m_delta( Max( 0, d ) ),
      m_numberOfLayers( Max( 1, n ) ),
      m_parallel( true ),
      m_maxProcessors( PCL_MAX_PROCESSORS ),
      m_transform(),
      m_layerEnabled()
   {
      PCL_PRECONDITION( n >= 1 )
      PCL_PRECONDITION( d >= 0 )
      InitializeLayersAndStates();
   }

   /*!
    * Copy constructor.
    */
   RedundantMultiscaleTransform( const RedundantMultiscaleTransform& x ) :
      BidirectionalImageTransformation( x ),
      m_delta( x.m_delta ),
      m_numberOfLayers( x.m_numberOfLayers ),
      m_parallel( x.m_parallel ),
      m_maxProcessors( x.m_maxProcessors ),
      m_transform( x.m_transform ),
      m_layerEnabled( x.m_layerEnabled )
   {
   }

   /*!
    * Move constructor.
    */
   RedundantMultiscaleTransform( RedundantMultiscaleTransform&& x ) :
      BidirectionalImageTransformation( x ),
      m_delta( x.m_delta ),
      m_numberOfLayers( x.m_numberOfLayers ),
      m_parallel( x.m_parallel ),
      m_maxProcessors( x.m_maxProcessors ),
      m_transform( std::move( x.m_transform ) ),
      m_layerEnabled( std::move( x.m_layerEnabled ) )
   {
   }

   /*!
    * Destroys this %RedundantMultiscaleTransform object. All existing layers
    * are destroyed and deallocated.
    */
   virtual ~RedundantMultiscaleTransform()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   RedundantMultiscaleTransform& operator =( const RedundantMultiscaleTransform& x )
   {
      (void)BidirectionalImageTransformation::operator =( x );
      m_delta          = x.m_delta;
      m_numberOfLayers = x.m_numberOfLayers;
      m_parallel       = x.m_parallel;
      m_maxProcessors  = x.m_maxProcessors;
      m_transform      = x.m_transform;
      m_layerEnabled   = x.m_layerEnabled;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   RedundantMultiscaleTransform& operator =( RedundantMultiscaleTransform&& x )
   {
      (void)BidirectionalImageTransformation::operator =( x );
      m_delta          = x.m_delta;
      m_numberOfLayers = x.m_numberOfLayers;
      m_parallel       = x.m_parallel;
      m_maxProcessors  = x.m_maxProcessors;
      m_transform      = std::move( x.m_transform );
      m_layerEnabled   = std::move( x.m_layerEnabled );
      return *this;
   }

   /*!
    * Returns the scaling sequence used by this multiscale transform.
    *
    * Successive layers are computed by applying linear on nonlinear operations
    * with kernels of size 2*s + 1.
    *
    * If the value returned by this function is zero 0, then the transform uses
    * the dyadic sequence: s = 1, 2, 4, ..., 2^j for 0 <= j < n.
    *
    * If the returned value is > 0, it is the constant increment in pixels
    * between two successive scales (linear scaling sequence): s = d*j for
    * 1 <= j < n.
    */
   int ScalingSequence() const
   {
      return m_delta;
   }

   /*!
    * Sets the scaling sequence \a d used by this multiscale transform.
    *
    * For the meaning of the \a d scaling sequence parameter, see the
    * documentation for the ScalingSequence() member function.
    *
    * \note As a consequence of calling this member function, all existing
    * layers in this transform are destroyed.
    */
   void SetScalingSequence( int d )
   {
      PCL_PRECONDITION( d >= 0 )
      DestroyLayers();
      m_delta = Max( 0, d );
   }

   /*!
    * Selects the dyadic scaling sequence for this multiscale transform.
    *
    * This is a convenience function, equivalent to:
    * SetScalingSequence( 0 );
    *
    * \note As a consequence of calling this member function, all existing
    * layers in this transform are destroyed.
    */
   void SetDyadicScalingSequence()
   {
      SetScalingSequence( 0 );
   }

   /*!
    * Selects a linear scaling sequence with distance \a d in pixels.
    *
    * This is a convenience function, equivalent to:
    * SetScalingSequence( d );
    *
    * \note As a consequence of calling this member function, all existing
    * layers in this transform are destroyed.
    */
   void SetLinearScalingSequence( int d = 1 )
   {
      PCL_PRECONDITION( d >= 1 )
      SetScalingSequence( Max( 1, d ) );
   }

   /*!
    * Returns the filter size for the specified layer \a j. Returns an odd
    * integer larger than or equal to three. For the dyadic scaling sequence,
    * the returned value is equal to 1 + 2*2^j (3, 5, 9, 17, 33, ...). For a
    * linear scaling sequence with distance d, returns 1 + 2*d*(1 + j).
    */
   int FilterSize( int j ) const
   {
      return 1 + (((m_delta < 1) ? 1 << j : (1 + j)*m_delta) << 1);
   }

   /*!
    * Returns the number of detail layers that will be (or have been) generated
    * by this transform.
    *
    * The number returned <em>does not</em> include the residual layer. In
    * other words, the total number of layers generated is always equal to one
    * plus the value returned by this member function, since the large-scale
    * residual layer is always generated by all multiscale transforms.
    */
   int NumberOfLayers() const
   {
      return m_numberOfLayers;
   }

   /*!
    * Sets the number of detail layers that will be generated by this
    * transform.
    *
    * \param n    Number of detail layers, <em>not including</em> the last,
    *             large-scale residual layer. Must be \a n >= 1.
    *
    * \note Calling this function implicitly performs a complete reset of this
    * object, including the deletion of all existing layers.
    */
   void SetNumberOfLayers( int n )
   {
      PCL_PRECONDITION( n >= 1 )
      m_numberOfLayers = Max( 1, n );
      InitializeLayersAndStates();
   }

   /*!
    * Returns a reference to the immutable layer at scale index \a i,
    * 0 <= \a i <= \a n, where \a n is the number of generated detail layers.
    * If \a i == \a n, this member function returns a reference to the
    * large-scale residual layer.
    *
    * \note Before trying to access layers, the multiscale transform must be
    * performed on an image. In addition, the specified layer must exist (must
    * not have been deleted). Otherwise this function (as well as others that
    * provide access to layer images) throws an Error exception.
    */
   const layer& Layer( int i ) const
   {
      ValidateLayerAccess( i );
      return m_transform[i];
   }

   /*!
    * Returns a reference to the (mutable) layer at scale index \a i. This is
    * an overloaded member function, provided for convenience.
    *
    * See Layer( int ) const for more information.
    */
   layer& Layer( int i )
   {
      ValidateLayerAccess( i );
      return m_transform[i];
   }

   /*!
    * Returns a reference to the (immutable) layer at scale index \a i. This is
    * a convenience operator, equivalent to:
    *
    * \code Layer( i ) const; \endcode
    *
    * The array subscript operators can produce more elegant code than the
    * %Layer functions.
    */
   const layer& operator []( int i ) const
   {
      return Layer( i );
   }

   /*!
    * Returns a reference to the (mutable) layer at scale index \a i. This is a
    * convenience operator, equivalent to:
    *
    * \code Layer( i ); \endcode
    *
    * The array subscript operators can produce more elegant code than the
    * %Layer functions.
    */
   layer& operator []( int i )
   {
      return Layer( i );
   }

   /*!
    * Deletes the layer at layer index \a i, 0 <= \a i <= \a n, where \a n is
    * the number of generated layers. If \a i == \a n this member function
    * deletes the large-scale residual layer.
    *
    * Deleted layers are excluded from image reconstructions performed by
    * subsequent inverse multiscale transforms.
    *
    * For example, if you delete the first two layers of a transform, a
    * subsequent inverse transform will remove all small-scale image structures
    * smaller than four pixels, assuming a dyadic scaling sequence.
    */
   void DeleteLayer( int i )
   {
      ValidateLayerAccess( i );
      m_transform[i].FreeData();
   }

   /*!
    * Returns true iff the layer at layer index \a i exists (0 <= \a i <= \a n,
    * where \a n is the number of generated layers), that is, if the multiscale
    * transform has been generated and the specified layer has been calculated
    * (because it was not disabled) and has not been deleted.
    */
   bool IsLayer( int i ) const
   {
      ValidateLayerIndex( i );
      return !m_transform[i].IsEmpty();
   }

   /*!
    * Enables or disables the layer at index \a i, 0 <= \a i <= \a n, where
    * \a n is the number of generated layers. If \a i == \a n, this member
    * function enables or disables the large-scale residual layer.
    *
    * Disabled layers are not generated during multiscale transforms, so they
    * are obviously excluded from image reconstructions (inverse transforms).
    * This has the advantage that a lot of memory can be saved if one or more
    * layers aren't required or have to be excluded from the inverse transform.
    *
    * For the reason explained, disabling layers is more efficient, in terms of
    * memory consumption, if done \e before the multiscale transform. When the
    * transform has already been performed, disabling one or more layers
    * excludes them from reconstructions, but does not delete them, so the
    * after-transformation layer disable operation, unlike layer deletion, is
    * reversible.
    */
   void EnableLayer( int i, bool enable = true )
   {
      ValidateLayerIndex( i );
      m_layerEnabled[i] = enable;
   }

   /*!
    * Disables or enables the layer at index \a i, 0 <= \a i <= \a n, where
    * \a n is the number of generated layers. If \a i == \a n, this member
    * function disables or enables the large-scale residual layer.
    *
    * This is a convenience function, equivalent to:
    * EnableLayer( i, !disable );
    */
   void DisableLayer( int i, bool disable = true )
   {
      EnableLayer( i, !disable );
   }

   /*!
    * Returns true iff the layer at index \a i is enabled, false if it is
    * disabled. See the documentation for EnableLayer( int, bool ) for detailed
    * information about disabled layers.
    */
   bool IsLayerEnabled( int i ) const
   {
      ValidateLayerIndex( i );
      return m_layerEnabled[i];
   }

   /*!
    * Biases a multiscale transform layer.
    *
    * \param i    Layer index, 0 <= \a i <= \a n, where \a n is the number of
    *             generated multiscale layers. If \a i == \a n, the residual
    *             layer is biased.
    *
    * \param k    Bias factor. Positive biases increase the relative weight of
    *             biased layers in image reconstructions (inverse transforms).
    *             Negative biases decrease relative layer weights. A zero bias
    *             factor does not change a layer.
    *
    * This member function can be used to enhance or attenuate image structures
    * at selected scales. Consider this code:
    *
    * \code
    * Image img;
    * // ...
    * RedundantMultiscaleTransform* M;
    * // ...
    * *M << img; // decomposition (transform)
    * M->BiasLayer( 1, +2.5 );
    * M->BiasLayer( 3, -1 );
    * *M >> img; // reconstruction (inverse transform)
    * \endcode
    *
    * In the example above, the second and fourth layers of a multiscale
    * transform have been biased. The layer at index 1 (the second layer) has
    * been enhanced because a positive layer bias has been applied (2.5). The
    * layer at index 3 (fourth layer) has been attenuated because a negative
    * bias (-1) has been applied.
    *
    * \note Layer biasing is an \e irreversible operation because it works by
    * multiplying all coefficients in the specified layer by a constant derived
    * from the specified bias factor.
    */
   void BiasLayer( int i, float k )
   {
      ValidateLayerAccess( i );
      if ( k != 0 )
         m_transform[i] *= (k > 0) ? (1 + k) : 1/(1 - k);
   }

   /*!
    * Returns the set of layers in this transform, after clearing the transform
    * without destroying its layers, whose ownership is then transferred to the
    * caller.
    *
    * If no multiscale transform has been performed, this function returns an
    * empty set.
    *
    * The caller is responsible for deallocation of the returned layers. After
    * calling this function, this object will be empty, just as if no transform
    * had been performed.
    */
   virtual transform ReleaseTransform()
   {
      transform r = m_transform;
      DestroyLayers();
      return r;
   }

   /*!
    * Destroys and deallocates all existing layers and resets all layer states
    * so that all layers are enabled.
    */
   virtual void Reset()
   {
      InitializeLayersAndStates();
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this multiscale transform.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance. If \a enable is false this parameter is ignored.
    *                A value <= 0 is ignored. The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this multiscale transform.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this multiscale
    * transform.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this multiscale
    * transform.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   /*
    * delta >= 1 :
    *    Linear scaling sequence. delta is the constant scale difference
    *    between two consecutive layers.
    *
    * delta  < 1 :
    *    Dyadic scaling sequence (1, 2, 4, 8, 16, ...).
    */
   int m_delta;

   /*
    * Number of detail layers, *not including* the residual smoothed layer,
    * which is always generated in a multiscale transform.
    */
   int m_numberOfLayers;

   /*
    * Use multiple threads
    */
   bool m_parallel : 1;

   /*
    * Maximum number of processors allowed
    */
   unsigned m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT;

   /*
    * Array of transform layers, including the residual layer, so the length
    * of this array is numberOfLayers+1.
    */
   transform m_transform;

   /*
    * Vector of layer enable/disable states.
    */
   layer_state_set m_layerEnabled;

   /*
    * Inverse transform (reconstruction)
    */
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::ComplexImage& ) const;
   virtual void Apply( pcl::DComplexImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;

   void InitializeLayersAndStates()
   {
      DestroyLayers();
      m_layerEnabled = layer_state_set( true, m_numberOfLayers+1 );
   }

   void DestroyLayers()
   {
      m_transform = transform( size_type( m_numberOfLayers+1 ) );
   }

   void ValidateLayerIndex( int j ) const;
   void ValidateLayerAccess( int j ) const;

   friend class MTReconstruction;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_RedundantMultiscaleTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/RedundantMultiscaleTransform.h - Released 2015/10/08 11:24:12 UTC
