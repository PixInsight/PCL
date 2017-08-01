//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.05.0411
// ----------------------------------------------------------------------------
// FITS.h - Released 2017-08-01T14:26:50Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#ifndef __PCL_FITS_h
#define __PCL_FITS_h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_AutoPointer_h
#include <pcl/AutoPointer.h>
#endif

#ifndef __PCL_File_h
#include <pcl/File.h>
#endif

#ifndef __PCL_FITSHeaderKeyword_h
#include <pcl/FITSHeaderKeyword.h>
#endif

#ifndef __PCL_ICCProfile_h
#include <pcl/ICCProfile.h>
#endif

#ifndef __PCL_Image_h
#include <pcl/Image.h>
#endif

#ifndef __PCL_ImageDescription_h
#include <pcl/ImageDescription.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

struct FITSFileData;
struct FITSReaderData;
struct FITSWriterData;
struct FITSExtensionData;

// ----------------------------------------------------------------------------

/*
 * FITS-specific image file options
 */
class FITSImageOptions
{
public:

   bool   unsignedIntegers                  :  1; // Write unsigned integer samples
   bool   writeFixedFloatKeywords           :  1; // Use a fixed-precision exponential format for real keyword values
   bool   writeScalingKeywordsForSignedData :  1; // Write BZERO=0 and BSCALE=1 for signed integer images
   bool   reloadKeywords                    :  1; // Reload header keywords upon file write completion
   bool   bottomUp                          :  1; // When this flag is true, the origin of coordinates is at the lower left corner of
                                                  //   the image. When false, the origin is at the upper left corner
   bool   signedIntegersArePhysical         :  1; // If true, truncate to [0,2^(n/2-1)] and do not rescale.
                                                  //   Otherwise the full range is [-2^(n/2),+2^(n/2-1)]
   bool   cleanupHeaders                    :  1; // Transform header keywords for FITS standard compliance before writing
   uint8  verbosity                         :  3; // Verbosity level: 0 = quiet, > 0 = write console state messages.
   uint32 __rsv__                           : 22; // Reserved for future extension -- must be zero

   double zeroOffset; // Zero offset value of read pixel values
   double scaleRange; // Scaling value of read pixel values

   FITSImageOptions()
   {
      Reset();
   }

   FITSImageOptions( const FITSImageOptions& ) = default;

   FITSImageOptions& operator =( const FITSImageOptions& ) = default;

   void Reset()
   {
      unsignedIntegers                  = true;
      writeFixedFloatKeywords           = false;
      writeScalingKeywordsForSignedData = true;
      reloadKeywords                    = true;
      bottomUp                          = false;
      signedIntegersArePhysical         = false;
      cleanupHeaders                    = false;
      verbosity                         = 1;
      __rsv__                           = 0;
      zeroOffset                        = 0;
      scaleRange                        = 1;
   }
};

// ----------------------------------------------------------------------------

/*
 * Base class for the FITSReader and FITSWriter classes.
 */
class FITS
{
public:

   /*
    * FITS-specific exception class
    */
   class Error : public File::Error
   {
   public:

      Error( const String& filePath, const String& message ) :
         File::Error( filePath, message )
      {
      }

      Error( const FITS::Error& x ) = default;

      virtual String ExceptionClass() const
      {
         return "PCL Legacy FITS Format Support";
      }

      virtual String Message() const; // reimplements File::Error
   };

#define PCL_DECLARE_FITS_ERROR( className, errorMessage )   \
   class className : public FITS::Error                     \
   {                                                        \
   public:                                                  \
      className( const String& filePath ) :                 \
         FITS::Error( filePath, errorMessage )              \
      {                                                     \
      }                                                     \
      className( const className& x ) = default;            \
   };

   PCL_DECLARE_FITS_ERROR( InvalidFilePath,
                           "Internal error: Invalid file path" )
   PCL_DECLARE_FITS_ERROR( InvalidReadOperation,
                           "Invalid read operation in FITS file" )
   PCL_DECLARE_FITS_ERROR( InvalidImage,
                           "Invalid image" )
   PCL_DECLARE_FITS_ERROR( NoImageAvailable,
                           "No images available in stream" )
   PCL_DECLARE_FITS_ERROR( InvalidNormalizationRange,
                           "Invalid pixel normalization range" )
   PCL_DECLARE_FITS_ERROR( UnableToAccessCurrentHDU,
                           "Unable to access the current FITS HDU" )
   PCL_DECLARE_FITS_ERROR( FileReadError,
                           "FITS file read error" )
   PCL_DECLARE_FITS_ERROR( ReadCoordinatesOutOfRange,
                           "Coordinates out of range in incremental read operation" )
   PCL_DECLARE_FITS_ERROR( UnableToOpenFile,
                           "Unable to open FITS file" )
   PCL_DECLARE_FITS_ERROR( UnableToAccessPrimaryHDU,
                           "Unable to access the primary FITS HDU" )
   PCL_DECLARE_FITS_ERROR( KeywordReadError,
                           "Error reading FITS header keyword" )
   PCL_DECLARE_FITS_ERROR( NoImageHDU,
                           "The FITS file contains no image HDU" )
   PCL_DECLARE_FITS_ERROR( NoReadableImage,
                           "The FITS file contains no readable image" )
   PCL_DECLARE_FITS_ERROR( UnsupportedThumbnailImageDimensions,
                           "Unsupported thumbnail image dimensions" )
   PCL_DECLARE_FITS_ERROR( InvalidThumbnailImage,
                           "Invalid thumbnail image" )
   PCL_DECLARE_FITS_ERROR( UnsupportedThumbnailSampleFormat,
                           "Unsupported thumbnail sample format" )
   PCL_DECLARE_FITS_ERROR( InvalidExtensionHDU,
                           "Invalid FITS extension HDU" )
   PCL_DECLARE_FITS_ERROR( FileWriteError,
                           "FITS file write error" )
   PCL_DECLARE_FITS_ERROR( InvalidWriteOperation,
                           "Invalid write operation in FITS file" )
   PCL_DECLARE_FITS_ERROR( WriteCoordinatesOutOfRange,
                           "Coordinates out of range in incremental write operation" )
   PCL_DECLARE_FITS_ERROR( UnableToCreateFile,
                           "Unable to create FITS file" )
   PCL_DECLARE_FITS_ERROR( UnableToCreatePrimaryHDU,
                           "Unable to create primary FITS HDU" )
   PCL_DECLARE_FITS_ERROR( KeywordWriteError,
                           "Error writing FITS header keyword" )
   PCL_DECLARE_FITS_ERROR( UnsupportedComplexFITS,
                           "This implementation does not support complex FITS images" )
   PCL_DECLARE_FITS_ERROR( Unsupported64BitFITS,
                           "This implementation does not support 64-bit integer FITS images" )
   PCL_DECLARE_FITS_ERROR( UnableToCreateImageHDU,
                           "Unable to create image FITS HDU" )
   PCL_DECLARE_FITS_ERROR( InvalidCloseImageOperation,
                           "Invalid close operation in FITS file" )
   PCL_DECLARE_FITS_ERROR( UnableToCreateThumbnailHDU,
                           "Unable to create thumbnail image HDU" )

   enum
   {
      MinThumbnailSize = 32,
      MaxThumbnailSize = 1024
   };

   FITS();

   FITS( const FITS& ) = delete;
   FITS& operator =( const FITS& ) = delete;

   virtual ~FITS() noexcept( false );

   String Path() const
   {
      return m_path;
   }

protected:

   String                    m_path;
   AutoPointer<FITSFileData> m_fileData;

   bool IsOpenStream() const;
   void CloseStream(); // ### derived must call base
};

// ----------------------------------------------------------------------------

/*
 * FITS Image File Reader
 */
class FITSReader : public FITS
{
public:

   FITSReader();

   FITSReader( const FITSReader& ) = delete;
   FITSReader& operator =( const FITSReader& ) = delete;

   virtual ~FITSReader() noexcept( false );

   /*!
    * Returns true iff this input stream is open.
    */
   bool IsOpen() const;

   /*!
    * Closes this input stream and destroys all internal working structures.
    */
   void Close();

   /*!
    * Opens an existing file at the specified \a path.
    */
   void Open( const String& path );

   /*!
    * Returns the number of images available in this input stream.
    */
   size_type NumberOfImages() const
   {
      return m_images.Length();
   }

   /*!
    * Sets the current image index in this input stream.
    */
   void SetIndex( int i )
   {
      m_index = m_images.IsEmpty() ? 0 : Range( i, 0, int( NumberOfImages() ) - 1 );
   }

   /*!
    * Returns the current image index in this input stream.
    */
   int Index() const
   {
      return m_index;
   }

   /*!
    * Returns a reference to the immutable basic image data structure for the
    * current image in this input stream.
    */
   const ImageInfo& Info() const
   {
      if ( m_images.IsEmpty() )
         throw NoImageAvailable( m_path );
      return m_images[m_index].info;
   }

   /*!
    * Returns a reference to an immutable format-independent options structure
    * corresponding to the current image in this input stream.
    */
   const ImageOptions& Options() const
   {
      if ( m_images.IsEmpty() )
         throw NoImageAvailable( m_path );
      return m_images[m_index].options;
   }

   /*!
    * Sets a new set of format-independent options for the current image in
    * this input stream.
    *
    * You should only call this function \e before any image read operation,
    * and only options that modify the reading behavior of the stream will be
    * taken into account. Not all image formats can use all reading options
    * available, so any call to this function can be simply ignored by an
    * instance of a derived class.
    */
   void SetOptions( const ImageOptions& options )
   {
      if ( m_images.IsEmpty() )
         throw NoImageAvailable( m_path );
      m_images[m_index].options = options;
   }

   /*!
    * Returns the identifier of the current image in this input stream.
    *
    * Not all image formats support image identifiers. If no identifier is
    * available, this function returns an empty string.
    */
   IsoString Id() const
   {
      if ( m_images.IsEmpty() )
         throw NoImageAvailable( m_path );
      return m_images[m_index].id;
   }

   /*
    * Returns a reference to the immutable FITS-specific options of the current
    * image in this FITS input stream.
    */
   const FITSImageOptions& FITSOptions() const;

   /*
    * Sets new FITS-specific options for the current image in this FITS input
    * stream. If you change options, you should do that *before* reading the
    * next image from this FITS file.
    */
   void SetFITSOptions( const FITSImageOptions& );

   /*
    * Embedded data.
    */
   ICCProfile ReadICCProfile();
   UInt8Image ReadThumbnail();
   FITSKeywordArray ReadFITSKeywords();

   /*
    * Extracts a FITS extension with the specified name.
    *
    * Returns true if the specified extension exists. In such case, a copy of
    * the extension's data is stored in the specified ByteArray container.
    */
   bool ReadBLOB( ByteArray& data, const IsoString& name );

   /*
    * Returns a list of data extensions in this FITS file.
    */
   IsoStringList DataExtensionNames() const;

   /*
    * Image reading
    */
   void ReadImage( FImage& img );
   void ReadImage( DImage& img );
   void ReadImage( UInt8Image& img );
   void ReadImage( UInt16Image& img );
   void ReadImage( UInt32Image& img );

   /*
    * Incremental reading
    */
   void ReadSamples( FImage::sample* buffer, int startRow, int rowCount, int channel );
   void ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel );
   void ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel );
   void ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel );
   void ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

private:

   ImageDescriptionArray       m_images;
   int                         m_index;  // current image index
   AutoPointer<FITSReaderData> m_data;

   friend class FITSReaderPrivate;
};

// ----------------------------------------------------------------------------

/*
 * FITS image file writer
 */
class FITSWriter : public FITS
{
public:

   FITSWriter();

   FITSWriter( const FITSWriter& ) = delete;
   FITSWriter& operator =( const FITSWriter& ) = delete;

   virtual ~FITSWriter() noexcept( false );

   /*!
    * Returns true iff this stream is open.
    */
   bool IsOpen() const;

   /*!
    * Closes this output stream and destroys all internal working structures.
    */
   void Close();

   /*!
    * Creates a new file for writing at the specified \a path, and prepares for
    * subsequent \a count image write and data embedding operations.
    */
   void Create( const String& path, int count );

   /*!
    * Returns a reference to a format-independent options structure
    * corresponding to the next image that will be written by this output
    * stream.
    */
   const ImageOptions& Options() const
   {
      return m_options;
   }

   /*!
    * Sets a new set of format-independent options for the next image written
    * by this output stream.
    *
    * Only options that modify the writing behavior of the stream will be taken
    * into account. Not all image formats can use all output options available,
    * so any call to this function can be simply ignored by an instance of a
    * derived class.
    */
   void SetOptions( const ImageOptions& options )
   {
      m_options = options;
   }

   /*!
    * Sets the identifier of the current image (that is, of the next image that
    * will be written) in this output stream.
    */
   void SetId( const IsoString& identifier )
   {
      m_id = identifier.Trimmed();
   }

   /*!
    * Returns the identifier of the current image (that is, of the next image
    * that will be written) in this output stream.
    */
   IsoString Id() const
   {
      return m_id;
   }

   /*
    * Returns a reference to the immutable FITS-specific options of the current
    * image in this FITS output stream.
    */
   const FITSImageOptions& FITSOptions() const;

   /*
    * Sets new FITS-specific options for the current image in this FITS output
    * stream. If you change options, you should do that \e before writing the
    * next image to this FITS file.
    */
   void SetFITSOptions( const FITSImageOptions& );

   /*
    * Embeddings
    */
   void WriteICCProfile( const ICCProfile& );
   void WriteThumbnail( const UInt8Image& );
   void WriteFITSKeywords( const FITSKeywordArray& );

   const FITSKeywordArray& FITSKeywords() const;

   /*
    * Writes a FITS extension with the specified name.
    */
   void WriteBLOB( const ByteArray& extData, const IsoString& extName );

   /*
    * Image writing
    */
   void WriteImage( const FImage& );
   void WriteImage( const DImage& );
   void WriteImage( const UInt8Image& );
   void WriteImage( const UInt16Image& );
   void WriteImage( const UInt32Image& );

   /*
    * Incremental writing
    */
   void CreateImage( const ImageInfo& );
   void WriteSamples( const FImage::sample* buffer, int startRow, int rowCount, int channel );
   void WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel );
   void WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel );
   void WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel );
   void WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel );
   void CloseImage();

   void Reset()
   {
      Close();
   }

private:

   ImageOptions                m_options; // format-independent options for the next image
   IsoString                   m_id;      // identifier for the next image
   AutoPointer<FITSWriterData> m_data;

   void WriteExtensionHDU( const FITSExtensionData& );

   friend class FITSWriterPrivate;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FITS_h

// ----------------------------------------------------------------------------
// EOF FITS.h - Released 2017-08-01T14:26:50Z
