//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/LocalNormalizationData.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_LocalNormalizationData_h
#define __PCL_LocalNormalizationData_h

/// \file pcl/LocalNormalizationData.h

#include <pcl/Defs.h>

#include <pcl/BicubicInterpolation.h>
#include <pcl/File.h>
#include <pcl/Image.h>
#include <pcl/TimePoint.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS XMLDocument;
class PCL_CLASS XMLElement;

/*!
 * \class LocalNormalizationData
 * \brief Normalization data parser and generator
 *
 * %LocalNormalizationData implements support for the %XML normalization data
 * format (XNML format, .xnml file suffix).
 *
 * An XNML file stores local normalization function data suitable for pixel
 * rejection and image integration tasks, including drizzle integration. The
 * %LocalNormalizationData class is a high-level interface for parsing and
 * generation of these special files.
 *
 * Local normalization functions are serialized as floating point real images,
 * where each channel stores scaling and zero offset coefficients, sampled at a
 * prescribed normalization scale, for a channel of a specific image.
 *
 * For a channel index c and position vector i on a given input image, the
 * local normalization function is:
 *
 * v'(c,i) = U(A(c,i))*v(c,i) + U(B(c,i))
 *
 * where A(c) is the matrix of scaling coefficients, B(c) is the matrix of zero
 * offset coefficients, U() is a scale-dependent interpolation functional, v is
 * an input pixel value, and v' is the normalized pixel value. Local
 * normalization matrices are conveniently stored as multichannel,
 * two-dimensional images, which facilitates two-dimensional interpolation with
 * smooth pixel interpolation algorithms.
 *
 * Normalization matrices are normally much smaller than the reference image
 * dimensions. This happens because we define local normalization functions at
 * a specific dimensional scale, which typically corresponds to a large dyadic
 * wavelet scale of 64, 128, or 256 pixels. For this reason, normalization
 * matrices require a smooth interpolation to compute normalization
 * coefficients at reference image coordinates.
 *
 * \sa DrizzleData
 */
class PCL_CLASS LocalNormalizationData
{
public:

   /*!
    * Represents a per-channel set of local image normalization matrices.
    */
   typedef DImage                            normalization_matrices;

   /*!
    * Represents a local normalization function coefficient.
    */
   typedef normalization_matrices::sample    normalization_coefficient;

   /*!
    * Represents a smooth interpolation used for calculation of local
    * normalization function values at image pixel coordinates.
    */
   typedef BicubicBSplineInterpolation<normalization_coefficient>
                                             matrix_interpolation;

   /*!
    * Represents an array of local normalization interpolators.
    */
   typedef Array<matrix_interpolation>       matrix_interpolations;

   /*!
    * Constructs an empty %LocalNormalizationData object.
    */
   LocalNormalizationData() = default;

   /*!
    * Copy constructor.
    */
   LocalNormalizationData( const LocalNormalizationData& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   LocalNormalizationData( LocalNormalizationData&& ) = default;
#else
   LocalNormalizationData( LocalNormalizationData&& x ) :
      m_referenceFilePath( std::move( x.m_referenceFilePath ) ),
      m_targetFilePath( std::move( x.m_targetFilePath ) ),
      m_scale( x.m_scale ),
      m_referenceWidth( x.m_referenceWidth ),
      m_referenceHeight( x.m_referenceHeight ),
      m_A( std::move( x.m_A ) ),
      m_B( std::move( x.m_B ) )
   {
   }
#endif

   /*!
    * Constructs a new %LocalNormalizationData instance by loading and parsing
    * a file in XNML format.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents must be in %XML normalization data format
    *                   (usually, a file with the .xnml suffix).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * This constructor validates the data retrieved from the specified file. It
    * throws the appropriate Error exceptions in the event of parsing errors or
    * invalid data.
    */
   LocalNormalizationData( const String& filePath, bool ignoreNormalizationData = false )
   {
      Parse( filePath, ignoreNormalizationData );
   }

   /*!
    * Constructs a new %LocalNormalizationData instance by parsing a
    * well-formed %XML document in XNML format.
    *
    * \param xml        Reference to the source %XML document. This constructor
    *                   expects an %XML document in valid %XML normalization
    *                   data format (.xnml).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * This constructor validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   LocalNormalizationData( const XMLDocument& xml, bool ignoreNormalizationData = false )
   {
      Parse( xml, ignoreNormalizationData );
   }

   /*!
    * Constructs a new %LocalNormalizationData instance by parsing an %XML
    * element in XNML format.
    *
    * \param element    Reference to the source %XML element. This constructor
    *                   expects an %XML document tree in valid %XML
    *                   normalization data format (.xnml) rooted at the
    *                   specified element.
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * This constructor validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   LocalNormalizationData( const XMLElement& element, bool ignoreNormalizationData = false )
   {
      Parse( element, ignoreNormalizationData );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~LocalNormalizationData()
   {
   }

   /*!
    * Returns the full path to the reference image file used for calculation of
    * the normalization data represented by this instance, or an empty string
    * if that file path is not available.
    *
    * \sa SetReferenceFilePath()
    */
   const String& ReferenceFilePath() const
   {
      return m_referenceFilePath;
   }

   /*!
    * Sets the path to the reference image file used for calculation of the
    * normalization data stored in this instance.
    *
    * \sa ReferenceFilePath()
    */
   void SetReferenceFilePath( const String& filePath )
   {
      m_referenceFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns the full path to the image file for which the normalization data
    * represented by this instance was calculated, or an empty string if such
    * file path is not available.
    *
    * The file identified by this function stores the input image of the image
    * normalization task that created this object. This file can be used as
    * input for a regular (non-drizzle) integration task, and the data
    * transported by this object can then be used for normalization of pixel
    * values in pixel rejection and/or integration output generation, including
    * drizzle integration.
    *
    * \sa SetTargetFilePath()
    */
   const String& TargetFilePath() const
   {
      return m_targetFilePath;
   }

   /*!
    * Sets the path to the image file for which the normalization data stored
    * in this instance was calculated.
    *
    * \sa TargetFilePath()
    */
   void SetTargetFilePath( const String& filePath )
   {
      m_targetFilePath = File::FullPath( filePath );
   }

   /*!
    * Returns the scale in pixels at which the local normalization functions
    * were sampled and calculated. For valid normalization data, the returned
    * value must be larger than zero, and should normally be larger than or
    * equal to 32 pixels.
    *
    * \sa SetNormalizationScale()
    */
   int NormalizationScale() const
   {
      return m_scale;
   }

   /*!
    * Sets the scale in pixels at which the local normalization functions have
    * been sampled and calculated.
    *
    * \sa NormalizationScale()
    */
   void SetNormalizationScale( int scale )
   {
      m_scale = scale;
   }

   /*!
    * Returns the width in pixels of the normalization reference and target
    * images, or -1 if this object has not been initialized with valid data.
    *
    * \sa ReferenceHeight(), SetReferenceDimensions()
    */
   int ReferenceWidth() const
   {
      return m_referenceWidth;
   }

   /*!
    * Returns the height in pixels of the normalization reference and target
    * images, or -1 if this object has not been initialized with valid data.
    *
    * \sa ReferenceWidth(), SetReferenceDimensions()
    */
   int ReferenceHeight() const
   {
      return m_referenceHeight;
   }

   /*!
    * Sets new normalization image dimensions in pixels, \a width and
    * \a height, respectively.
    *
    * \sa ReferenceWidth(), ReferenceHeight()
    */
   void SetReferenceDimensions( int width, int height )
   {
      m_referenceWidth = width;
      m_referenceHeight = height;
   }

   /*!
    * Returns the number of channels or pixel sample planes in the image
    * represented by this instance, or zero if this object has not been
    * initialized with valid data.
    */
   int NumberOfChannels() const
   {
      return m_A.NumberOfChannels();
   }

   /*!
    * Returns a reference to the set of matrices of zero offset coefficients in
    * the local normalization function.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * \sa Scale(), SetNormalizationMatrices(), NormalizationScale()
    */
   const normalization_matrices& ZeroOffset() const
   {
      return m_B;
   }

   /*!
    * Returns a reference to the set of matrices of scaling coefficients in the
    * local normalization function.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * \sa ZeroOffset(), SetNormalizationMatrices(), NormalizationScale()
    */
   const normalization_matrices& Scale() const
   {
      return m_B;
   }

   /*!
    * Sets new local normalization function matrices.
    *
    * \param A          Reference to an image storing the matrices of scaling
    *                   normalization coefficients.
    *
    * \param B          Reference to an image storing the matrices of zero
    *                   offset normalization coefficients.
    *
    * See the class description for detailed information on local normalization
    * functions.
    *
    * \sa ZeroOffset(), Scale()
    */
   void SetNormalizationMatrices( const normalization_matrices& A, const normalization_matrices& B )
   {
      if ( A.Bounds() != B.Bounds() || A.NumberOfChannels() != B.NumberOfChannels() )
         throw Error( "LocalNormalizationData::SetNormalizationMatrices(): Incompatible matrix dimensions." );
      m_A = A;
      m_B = B;
   }

   /*!
    * Initializes the internal local normalization function interpolators.
    * After this initialization, operator()() can be called repeatedly to apply
    * the local normalization function to pixel sample values in reference
    * image coordinates.
    *
    * Calling this member function is not necessary when valid local
    * normalization data have been loaded from an XNML file or %XML document,
    * since in this case the internal interpolators are always initialized
    * automatically. This function must be called, however, for newly defined
    * normalization data set by calling SetReferenceDimensions() and
    * SetNormalizationMatrices().
    *
    * If this object has not been initialized with valid local normalization
    * data, this member function throws an Error exception.
    *
    * \sa operator()()
    */
   void InitInterpolations();

   /*!
    * Returns true iff valid local normalization function data have been loaded
    * and the internal interpolators have been initialized.
    */
   bool HasInterpolations() const
   {
      return !m_UA.IsEmpty() && !m_UB.IsEmpty();
   }

   /*!
    * Returns a locally normalized pixel sample value.
    *
    * \param z          Input pixel sample value (unnormalized).
    * \param x          Horizontal image pixel coordinate.
    * \param y          Vertical image pixel coordinate.
    * \param c          Channel index.
    *
    * This operator interpolates local normalization functions and applies the
    * corresponding linear transformation to the input value \a z, which gives
    * the return value. Note that the normalized value might be out of the
    * input pixel sample range. In such case the normalized value should be
    * truncated or rescaled as appropriate.
    *
    * For performance reasons, this operator does not check that the required
    * normalization and interpolation data have been initialized; if they are
    * not, calling this operator will most likely lead to a crash.
    * Interpolation data are automatically initialized when a valid XNML file
    * or %XML element is loaded and parsed. For newly constructed objects, make
    * sure you define the reference dimensions and normalization matrices. Then
    * call InitInterpolations() before starting to call this operator.
    */
   double operator()( double z, int x, int y, int c = 0 ) const
   {
      double sx = m_sx*x;
      double sy = m_sy*y;
      return m_UA[c]( sx, sy )*z + m_UB[c]( sx, sy );
   }

   /*!
    * Returns the UTC time this normalization data was created, or an invalid
    * TimePoint instance if creation time information is not available.
    */
   TimePoint CreationTime() const
   {
      return m_creationTime;
   }

   /*!
    * Clears all of the image normalization data transported by this instance,
    * yielding an empty %LocalNormalizationData object.
    */
   void Clear();

   /*!
    * Loads and parses a normalization data file in XNML format.
    *
    * \param filePath   Path to an existing file that will be parsed. The file
    *                   contents must be in %XML normalization data format
    *                   (usually, a file with the .xnml suffix).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * All of the previous data transported by this instance will be replaced
    * with new data acquired from the specified file.
    *
    * This function validates the data retrieved from the specified file. It
    * throws an Error exception in the event of parsing errors or invalid data.
    */
   void Parse( const String& filePath, bool ignoreNormalizationData = false );

   /*!
    * Parses a well-formed %XML document in XNML format.
    *
    * \param xml        Reference to the source %XML document. This member
    *                   function expects an %XML document in valid %XML
    *                   normalization data format (.xnml).
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * All of the previous data transported by this instance will be replaced
    * with new data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * document. It throws an Error exception in the event of an invalid
    * document or invalid data.
    */
   void Parse( const XMLDocument& xml, bool ignoreNormalizationData = false );

   /*!
    * Parses an %XML element in XNML format.
    *
    * \param element    Reference to the source %XML element. This member
    *                   function expects an %XML document tree in valid %XML
    *                   normalization data format (.xnml) rooted at the
    *                   specified element.
    *
    * \param ignoreNormalizationData     If true, local normalization data will
    *                   be ignored and not parsed. This includes matrices of
    *                   normalization function coefficients. Setting this
    *                   parameter to true can be useful when the caller only
    *                   needs to retrieve information on file paths and
    *                   reference dimensions.
    *
    * All of the previous data transported by this instance will be replaced
    * with new data acquired from the specified %XML contents.
    *
    * This function validates the data retrieved from the specified %XML
    * element. It throws an Error exception in the event of invalid data.
    */
   void Parse( const XMLElement& element, bool ignoreNormalizationData = false );

   /*!
    * Returns true if data compression is enabled for serializations performed
    * with this object.
    *
    * Data compression is enabled by default for newly constructed
    * %LocalNormalizationData objects. Currently the LZ4 compression codec is
    * applied for serialization of local normalization matrices.
    *
    * \sa EnableCompression(), DisableCompression()
    */
   bool IsCompressionEnabled() const
   {
      return m_compressionEnabled;
   }

   /*!
    * Enables compression of serialized local normalization data.
    *
    * \sa IsCompressionEnabled(), DisableCompression()
    */
   void EnableCompression( bool enable = true )
   {
      m_compressionEnabled = enable;
   }

   /*!
    * Disables compression of serialized local normalization data.
    *
    * \sa IsCompressionEnabled(), EnableCompression()
    */
   void DisableCompression( bool disable = true )
   {
      EnableCompression( !disable );
   }

   /*!
    * Serializes the normalization data transported by this object as a new
    * %XML document in XNML format.
    *
    * The returned XMLDocument object has been allocated dynamically. The
    * caller is responsible for destroying and deallocating it as appropriate.
    */
   XMLDocument* Serialize() const;

   /*!
    * Serializes the normalization data transported by this object as a new
    * %XML document file in XNML format (.xnml). The file will be newly created
    * at the specified file \a path.
    *
    * \warning If a file already exists at the specified \a path, its previous
    * contents will be lost after calling this function.
    */
   void SerializeToFile( const String& path ) const;

private:

   String                 m_referenceFilePath;    // path to the normalization reference image
   String                 m_targetFilePath;       // path to the normalization target image
   int                    m_scale = -1;           // normalization scale in px
   int                    m_referenceWidth = -1;  // reference image width in px
   int                    m_referenceHeight = -1; // reference image height in px
   normalization_matrices m_A;                    // scaling coefficients
   normalization_matrices m_B;                    // zero offset coefficients
   matrix_interpolations  m_UA;                   // interpolators for m_A
   matrix_interpolations  m_UB;                   // interpolators for m_B
   double                 m_sx;                   // coordinate scaling factor, X axis
   double                 m_sy;                   // coordinate scaling factor, Y axis
   TimePoint              m_creationTime;
   bool                   m_compressionEnabled = true;

   void ParseNormalizationMatrices( normalization_matrices&, const XMLElement& ) const;
   void SerializeNormalizationMatrices( XMLElement*, const normalization_matrices& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LocalNormalizationData_h

// ----------------------------------------------------------------------------
// EOF pcl/LocalNormalizationData.h - Released 2017-07-09T18:07:07Z
