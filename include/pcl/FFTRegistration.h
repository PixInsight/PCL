//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/FFTRegistration.h - Released 2018-11-01T11:06:36Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_FFTRegistration_h
#define __PCL_FFTRegistration_h

/// \file pcl/FFTRegistration.h

#include <pcl/Defs.h>

#include <pcl/ImageVariant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class FFTRegistrationEngine
 * \brief Base class of %FFT-based image registration algorithms
 *
 * Image registration based on phase correlation via fast Fourier transforms.
 *
 * <b>References</b>
 *
 * \li Araiza, R., Xie, H. & al., 2002. Automatic referencing of multi-spectral
 * images. Proceedings of 15th IEEE Southwest Symposium on Image Analysis and
 * Interpretation, Santa Fe, USA, 21-25.
 *
 * \li Xie, H. & al., 2000. Automatic image registration based on a %FFT
 * algorithm and IDL/ENVI. Proceedings of the ICORG-2000 International
 * Conference on Remote Sensing and GIS/GPS, Hyderabad, India, I-397~I-402.
 *
 * \li Harold S., Stone T. & al., 2003. Analysis of image registration noise
 * due to rotationally dependent aliasing. Journal of Visual Communication and
 * Image Representation 14, 114-135.
 *
 * \li DeCastro, E., Morandi, C., 1987. Registration of translated and rotated
 * images using finite Fourier transforms. IEEE Transactions on Pattern
 * Analysis and Machine Intelligence 95, 700-703.
 */
class PCL_CLASS FFTRegistrationEngine
{
public:

   /*!
    * Constructs an %FFTRegistrationEngine object.
    */
   FFTRegistrationEngine() = default;

   /*!
    * Copy constructor.
    */
   FFTRegistrationEngine( const FFTRegistrationEngine& ) = default;

   /*!
    * Move constructor.
    */
   FFTRegistrationEngine( FFTRegistrationEngine&& x ) :
      m_fftReference( std::move( x.m_fftReference ) )
   {
   }

   /*!
    * Destroys an %FFTRegistrationEngine object.
    */
   virtual ~FFTRegistrationEngine()
   {
   }

   /*!
    * Returns true iff this engine has been initialized.
    */
   bool IsInitialized() const
   {
      return !m_fftReference.IsEmpty();
   }

   /*!
    * Initializes this registration engine for the specified reference image
    * \a image.
    *
    * Once initialized, subsequent calls to Evaluate() will compute
    * registration parameters for target images referred to the same reference
    * image, until a new initialization is performed, or until the Reset()
    * member function is called.
    */
   template <class P>
   void Initialize( const pcl::GenericImage<P>& image )
   {
      Reset();
      m_fftReference = DoInitialize( image );
   }

   /*!
    * Initializes this registration engine for the specified reference
    * \a image.
    *
    * This member function behaves like Initialize( const GenericImage<P>& ),
    * except that it receives a reference to ImageVariant. The engine is
    * initialized for the image transported by the ImageVariant object.
    */
   void Initialize( const ImageVariant& image )
   {
      Reset();
      if ( image )
         if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: m_fftReference = DoInitialize( static_cast<const ComplexImage&>( *image ) ); break;
            case 64: m_fftReference = DoInitialize( static_cast<const DComplexImage&>( *image ) ); break;
            }
         else if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: m_fftReference = DoInitialize( static_cast<const Image&>( *image ) ); break;
            case 64: m_fftReference = DoInitialize( static_cast<const DImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: m_fftReference = DoInitialize( static_cast<const UInt8Image&>( *image ) ); break;
            case 16: m_fftReference = DoInitialize( static_cast<const UInt16Image&>( *image ) ); break;
            case 32: m_fftReference = DoInitialize( static_cast<const UInt32Image&>( *image ) ); break;
            }
   }

   /*!
    * Returns a reference to the discrete Fourier transform of the reference
    * image.
    *
    * The reference image is the image specified in the last call to
    * Initialize(). This function returns an empty image if this registration
    * engine has not been initialized.
    */
   const ComplexImage& DFTOfReferenceImage() const
   {
      return m_fftReference;
   }

   /*!
    * Resets this registration engine and deallocates all internal data
    * structures. The engine will have to be initialized before performing new
    * evaluations of registration parameters.
    */
   void Reset()
   {
      m_fftReference.FreeData();
   }

   /*!
    * Evaluates a target image \a image to compute registration parameters.
    *
    * Before calling this function, the engina must be initialized by calling
    * Initialize() with a reference image.
    */
   template <class P>
   void Evaluate( const pcl::GenericImage<P>& image )
   {
      PCL_PRECONDITION( IsInitialized() )
      if ( IsInitialized() )
         DoEvaluate( image );
   }

   /*!
    * Evaluates a target image \a vimg to compute registration parameters.
    *
    * This member function behaves like Evaluate( const GenericImage<P>& ),
    * except that it receives a reference to ImageVariant. The engine evaluates
    * for the image transported by the ImageVariant object.
    */
   void Evaluate( const ImageVariant& image )
   {
      PCL_PRECONDITION( IsInitialized() )
      if ( IsInitialized() )
         if ( image )
            if ( image.IsComplexSample() )
               switch ( image.BitsPerSample() )
               {
               case 32: DoEvaluate( static_cast<const pcl::ComplexImage&>( *image ) ); break;
               case 64: DoEvaluate( static_cast<const pcl::DComplexImage&>( *image ) ); break;
               }
            else if ( image.IsFloatSample() )
               switch ( image.BitsPerSample() )
               {
               case 32: DoEvaluate( static_cast<const pcl::Image&>( *image ) ); break;
               case 64: DoEvaluate( static_cast<const pcl::DImage&>( *image ) ); break;
               }
            else
               switch ( image.BitsPerSample() )
               {
               case  8: DoEvaluate( static_cast<const pcl::UInt8Image&>( *image ) ); break;
               case 16: DoEvaluate( static_cast<const pcl::UInt16Image&>( *image ) ); break;
               case 32: DoEvaluate( static_cast<const pcl::UInt32Image&>( *image ) ); break;
               }
   }

protected:

   // DFT of the reference image.
   ComplexImage m_fftReference;

   virtual ComplexImage DoInitialize( const pcl::Image& ) = 0;
   virtual ComplexImage DoInitialize( const pcl::DImage& ) = 0;
   virtual ComplexImage DoInitialize( const pcl::ComplexImage& ) = 0;
   virtual ComplexImage DoInitialize( const pcl::DComplexImage& ) = 0;
   virtual ComplexImage DoInitialize( const pcl::UInt8Image& ) = 0;
   virtual ComplexImage DoInitialize( const pcl::UInt16Image& ) = 0;
   virtual ComplexImage DoInitialize( const pcl::UInt32Image& ) = 0;

   virtual void DoEvaluate( const pcl::Image& ) = 0;
   virtual void DoEvaluate( const pcl::DImage& ) = 0;
   virtual void DoEvaluate( const pcl::ComplexImage& ) = 0;
   virtual void DoEvaluate( const pcl::DComplexImage& ) = 0;
   virtual void DoEvaluate( const pcl::UInt8Image& ) = 0;
   virtual void DoEvaluate( const pcl::UInt16Image& ) = 0;
   virtual void DoEvaluate( const pcl::UInt32Image& ) = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup fft_registration_functions FFT-Based Registration Operators
 */

/*!
 * Initializes a %FFT registration engine or evaluates a target image.
 *
 * \param R      %FFT registration engine.
 * \param image  An image or ImageVariant for initialization or evaluation.
 *
 * If the engine has not still been initialized, it is initialized for the
 * \e reference image \a image. Once the engine has been initialized, it
 * evaluates registration parameters for the specified \e target image \a image.
 *
 * Returns a reference to the %FFT registration engine \a R.
 *
 * \sa FFTRegistrationEngine
 *
 * \ingroup fft_registration_functions
 */
template <class O> inline
FFTRegistrationEngine& operator <<( FFTRegistrationEngine& R, const O& image )
{
   if ( !R.IsInitialized() )
      R.Initialize( image );
   else
      R.Evaluate( image );
   return R;
}

// ----------------------------------------------------------------------------

/*!
 * \class FFTTranslation
 * \brief %FFT registration engine: translation
 *
 * %FFTTranslation evaluates translation image registration parameters. It
 * computes the offsets \e dx and \e dy required to register a target image on
 * a reference image, using an %FFT-based phase correlation algorithm.
 *
 * \sa FFTRotationAndScaling, FFTRegistrationEngine
 */
class PCL_CLASS FFTTranslation : public FFTRegistrationEngine
{
public:

   /*!
    * Constructs an %FFTTranslation object.
    */
   FFTTranslation() = default;

   /*!
    * Destroys an %FFTTranslation object.
    */
   virtual ~FFTTranslation()
   {
   }

   /*!
    * Returns true iff this registration engine can evaluate translations
    * greater than or equal to one half of the largest dimension of the
    * reference image.
    *
    * \sa EnableLargeTranslations(), DisableLargeTranslations()
    */
   bool AreLargeTranslationsEnabled() const
   {
      return m_largeTranslations;
   }

   /*!
    * Enables or disables evaluation of large translations (>= one half of the
    * reference image dimension).
    *
    * \warning When large translations are enabled, the engine needs to double
    * the sizes of internal working matrices, which \e quadruplicates the
    * amount of required memory.
    *
    * \sa DisableLargeTranslations(), AreLargeTranslationsEnabled()
    */
   void EnableLargeTranslations( bool enable = true )
   {
      if ( enable != m_largeTranslations )
      {
         Reset();
         m_largeTranslations = enable;
      }
   }

   /*!
    * Disables or enables evaluation of large translations (>= one half of the
    * reference image dimension).
    *
    * This is a convenience member function, equivalent to
    * EnableLargeTranslations( !disable )
    *
    * \warning When large translations are enabled, the engine needs to double
    * the sizes of internal working matrices, which \e quadruplicates the
    * amount of required memory.
    *
    * \sa EnableLargeTranslations(), AreLargeTranslationsEnabled()
    */
   void DisableLargeTranslations( bool disable = true )
   {
      EnableLargeTranslations( !disable );
   }

   /*!
    * Returns the evaluated translation increments in pixels.
    *
    * The returned Point object contains the required offsets in the X and Y
    * directions to register the last evaluated target image on the reference
    * image set upon initialization.
    *
    * \sa DeltaX(), DeltaY()
    */
   const FPoint& Delta() const
   {
      return m_delta;
   }

   /*!
    * Returns the evaluated translation increment in the X direction, in
    * pixels.
    *
    * \sa DeltaY(), Delta()
    */
   float DeltaX() const
   {
      return m_delta.x;
   }

   /*!
    * Returns the evaluated translation increment in the Y direction, in
    * pixels.
    *
    * \sa DeltaX(), Delta()
    */
   float DeltaY() const
   {
      return m_delta.y;
   }

   /*!
    * Returns the peak value read from the phase matrix for the last evaluated
    * target image. The peak value isn't normalized, that is, it doesn't
    * necessarily have to be in the [0,1] range.
    */
   float Peak() const
   {
      return m_peak;
   }

protected:

   // Allow translations > size/2.
   // Warning: a lot of memory may be necessary --four times more.
   bool     m_largeTranslations = false;

   // Evaluation result.
   FPoint   m_delta = 0.0F;

   // Peak value detected in the phase matrix.
   float    m_peak = 0.0F;

   ComplexImage DoInitialize( const pcl::Image& ) override;
   ComplexImage DoInitialize( const pcl::DImage& ) override;
   ComplexImage DoInitialize( const pcl::ComplexImage& ) override;
   ComplexImage DoInitialize( const pcl::DComplexImage& ) override;
   ComplexImage DoInitialize( const pcl::UInt8Image& ) override;
   ComplexImage DoInitialize( const pcl::UInt16Image& ) override;
   ComplexImage DoInitialize( const pcl::UInt32Image& ) override;

   void DoEvaluate( const pcl::Image& ) override;
   void DoEvaluate( const pcl::DImage& ) override;
   void DoEvaluate( const pcl::ComplexImage& ) override;
   void DoEvaluate( const pcl::DComplexImage& ) override;
   void DoEvaluate( const pcl::UInt8Image& ) override;
   void DoEvaluate( const pcl::UInt16Image& ) override;
   void DoEvaluate( const pcl::UInt32Image& ) override;
};

// ----------------------------------------------------------------------------

/*!
 * \class FFTRotationAndScaling
 * \brief %FFT registration engine: rotation and scaling
 *
 * %FFTRotationAndScaling evaluates rotation and scaling image registration
 * parameters. It computes the rotation angle and scaling ratio required to
 * register a target image on a reference image, using an algorithm based on
 * the Fourier-Mellin transform.
 *
 * \warning This class is currently experimental. It is not guaranteed to
 * provide reliable image registration parameters.
 *
 * \sa FFTTranslation, FFTRegistrationEngine
 */
class PCL_CLASS FFTRotationAndScaling : public FFTRegistrationEngine
{
public:

   /*!
    * Constructs a %FFTRotationAndScaling object.
    *
    * Scaling ratio evaluation (see EvaluatesScaling()) is disabled by default.
    */
   FFTRotationAndScaling() = default;

   /*!
    * Destroys a %FFTRotationAndScaling object.
    */
   virtual ~FFTRotationAndScaling()
   {
   }

   /*!
    * Returns true iff this engine evaluates scaling ratios. By default, scale
    * evaluation is disabled.
    */
   bool EvaluatesScaling() const
   {
      return m_evaluateScaling;
   }

   /*!
    * Enables evaluation of scaling ratios.
    */
   void EnableScalingEvaluation( bool enable = true )
   {
      if ( enable != m_evaluateScaling )
      {
         Reset();
         m_evaluateScaling = enable;
      }
   }

   /*!
    * Disables evaluation of scaling ratios.
    *
    * This is a convenience member function, equivalent to
    * EnableScalingEvaluation( !disable )
    */
   void DisableScalingEvaluation( bool disable = true )
   {
      EnableScalingEvaluation( !disable );
   }

   /*!
    * Returns the current low frequency cutoff for this registration engine.
    *
    * See the documentation for SetLowFrequencyCutoff() for a detailed
    * description of low-frequency cutoffs.
    */
   float LowFrequencyCutoff() const
   {
      return m_lowFrequencyCutoff;
   }

   /*!
    * Returns true iff this engine uses a low-frequency cutoff to reduce the
    * effects of rotational aliasing.
    */
   bool HasLowFrequencyCutoff() const
   {
      return LowFrequencyCutoff() > 0;
   }

   /*!
    * Sets the radius of the low-frequency cutoff.
    *
    * The evaluation of rotation and scaling registration parameters can
    * benefit from a simple low-frequency cutoff operation to reduce rotational
    * aliasing on the computed DFTs. The value set by this function is the
    * radius of the cutoff \e hole relative to the largest dimension of the
    * reference image. The default value is 1/200. A value of zero disables the
    * low-frequency cutoff feature.
    */
   void SetLowFrequencyCutoff( float r )
   {
      m_lowFrequencyCutoff = Range( r, 0.0F, 0.5F );
   }

   /*!
    * Returns the evaluated rotation angle in radians.
    *
    * The returned value is the required rotation angle to register the last
    * evaluated target image on the reference image set upon initialization.
    */
   float RotationAngle() const
   {
      return m_rotationAngle;
   }

   /*!
    * Returns the evaluated scaling ratio.
    *
    * The returned value is the required scaling ratio to register the last
    * evaluated target image on the reference image set upon initialization.
    */
   float ScalingRatio() const
   {
      return m_scalingRatio;
   }

protected:

   // Evaluate rotation+scaling, or just rotation?
   bool     m_evaluateScaling = false;

   // Low-frequency cutoff, as a fraction of the DFT radius.
   float    m_lowFrequencyCutoff = 1.0F/200;

   // Evaluation result
   float    m_rotationAngle = 0.0F; // radians
   float    m_scalingRatio = 1.0F;  // pixels

   ComplexImage DoInitialize( const pcl::Image& ) override;
   ComplexImage DoInitialize( const pcl::DImage& ) override;
   ComplexImage DoInitialize( const pcl::ComplexImage& ) override;
   ComplexImage DoInitialize( const pcl::DComplexImage& ) override;
   ComplexImage DoInitialize( const pcl::UInt8Image& ) override;
   ComplexImage DoInitialize( const pcl::UInt16Image& ) override;
   ComplexImage DoInitialize( const pcl::UInt32Image& ) override;

   void DoEvaluate( const pcl::Image& ) override;
   void DoEvaluate( const pcl::DImage& ) override;
   void DoEvaluate( const pcl::ComplexImage& ) override;
   void DoEvaluate( const pcl::DComplexImage& ) override;
   void DoEvaluate( const pcl::UInt8Image& ) override;
   void DoEvaluate( const pcl::UInt16Image& ) override;
   void DoEvaluate( const pcl::UInt32Image& ) override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FFTRegistration_h

// ----------------------------------------------------------------------------
// EOF pcl/FFTRegistration.h - Released 2018-11-01T11:06:36Z
