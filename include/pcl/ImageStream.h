//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/ImageStream.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_ImageStream_h
#define __PCL_ImageStream_h

/// \file pcl/ImageStream.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_File_h
#include <pcl/File.h>
#endif

#ifndef __PCL_ByteArray_h
#include <pcl/ByteArray.h>
#endif

#ifndef __PCL_ImageVariant_h
#include <pcl/ImageVariant.h>
#endif

#ifndef __PCL_ImageDescription_h
#include <pcl/ImageDescription.h>
#endif

#ifndef __PCL_FITSHeaderKeyword_h
#include <pcl/FITSHeaderKeyword.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ICCProfile;
class PCL_CLASS FITSHeaderKeyword;
class PCL_CLASS Variant;

// ----------------------------------------------------------------------------

/*!
 * \class ImageStream
 * \brief Abstract base class for internal image file format implementations
 *
 * %ImageStream provides a generic interface for format-independent
 * input/output operations on image data streams.
 *
 * %ImageStream is an abstract base class for ImageReader and ImageWriter.
 * These classes are used internally by standard file format support modules to
 * simplify implementation of common I/O operations. In general, unless you are
 * implementing a standard file format module, you should not need to use these
 * classes.
 *
 * \sa ImageReader, ImageWriter
 */
class PCL_CLASS ImageStream
{
public:

   /*!
    * Constructs an %ImageStream object.
    */
   ImageStream() = default;

   /*!
    * Destroys an %ImageStream object. If this object represents an open
    * stream, it is closed upon destruction.
    */
   virtual ~ImageStream()
   {
      if ( IsOpen() )
         Close();
   }

   // The following two member functions *should* be pure virtual...
   // But bloody MSVC does not like them pure and called from the destructor!

   /*!
    * Returns true if this image stream is open.
    */
   virtual bool IsOpen() const
   {
      return false;
   }

   /*!
    * Closes an open image stream.
    */
   virtual void Close()
   {
   }

   /*!
    * Returns true if this stream can read image data.
    */
   virtual bool IsReader() const
   {
      return false;
   }

   /*!
    * Returns true if this stream can write image data.
    */
   virtual bool IsWriter() const
   {
      return false;
   }

   /*!
    * Returns the full file path of this image stream.
    */
   String Path() const
   {
      return path;
   }

protected:

   String path; // Full path to an image file

private:

   // Image streams are unique objects.
   ImageStream( const ImageStream& ) = delete;
   ImageStream& operator =( const ImageStream& ) = delete;
};

// ----------------------------------------------------------------------------

/*!
 * \class ImageReader
 * \brief Abstract base class for internal input image stream implementations
 *
 * %ImageReader provides a generic interface for format-independent input
 * operations on image data streams.
 *
 * The %ImageReader and ImageWriter classes are used internally by standard
 * file format support modules to simplify implementation of common I/O
 * operations. In general, unless you are implementing a standard file format
 * module, you should not need to use these classes.
 *
 * \sa ImageStream, ImageWriter
 */
class PCL_CLASS ImageReader : public virtual ImageStream
{
public:

   /*!
    * Constructs an %ImageReader object.
    */
   ImageReader() : ImageStream(), images(), index( 0 )
   {
   }

   /*!
    * Destroys an %ImageReader object.
    */
   virtual ~ImageReader()
   {
   }

   // N.B.: virtual IsOpen() and virtual Close() redeclared as pure virtual.

   /*!
    */
   virtual bool IsOpen() const = 0;

   /*!
    */
   virtual void Close() = 0;

   /*!
    * Opens an image file for reading at the specified \a filePath.
    */
   virtual void Open( const String& filePath ) = 0;

   /*!
    */
   virtual bool IsReader() const
   {
      return true;
   }

   /*!
    * Returns the number of images available in this input stream.
    */
   size_type NumberOfImages() const
   {
      return images.Length();
   }

   /*!
    * Sets the current image index in this input stream.
    */
   virtual void SetIndex( int i )
   {
      index = images.IsEmpty() ? 0 : Range( i, 0, int( NumberOfImages() ) - 1 );
   }

   /*!
    * Returns the current image index in this input stream.
    */
   int Index() const
   {
      return index;
   }

   /*!
    * Returns a reference to the immutable basic image data structure for the
    * current image in this input stream.
    */
   const ImageInfo& Info() const
   {
      if ( images.IsEmpty() )
         throw Error( String( "ImageReader:Info()" ) + ": No image in input stream" );
      return images[index].info;
   }

   /*!
    * Returns a reference to an immutable format-independent options structure
    * corresponding to the current image in this input stream.
    */
   const ImageOptions& Options() const
   {
      if ( images.IsEmpty() )
         throw Error( String( "ImageReader:Options()" ) + ": No image in input stream" );
      return images[index].options;
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
      if ( images.IsEmpty() )
         throw Error( String( "ImageReader:SetOptions()" ) + ": No image in input stream" );
      images[index].options = options;
   }

   /*!
    * Returns a reference to a format-independent options structure
    * corresponding to the current image in this input stream.
    *
    * \deprecated This member function has been deprecated and should not be
    * used in newly produced code. Use ImageReader::SetOptions() instead of
    * this member function.
    */
   ImageOptions& Options()
   {
      if ( images.IsEmpty() )
         throw Error( String( "ImageReader:Options()" ) + ": No image in input stream" );
      return images[index].options;
   }

   /*!
    * Returns the identifier of the current image in this input stream.
    *
    * Not all image formats support image identifiers. If no identifier is
    * available, this function returns an empty string.
    */
   IsoString Id() const
   {
      if ( images.IsEmpty() )
         throw Error( String( "ImageReader:Id()" ) + ": No image in input stream" );
      return images[index].id;
   }

   /*!
    * Extracts a list of FITS header keywords from the current image in this
    * input stream. Returns true if the extraction operation was successful.
    */
   virtual bool Extract( FITSKeywordArray& keywords )
   {
      throw NotImplemented( *this, "Extract embedded FITS keywords" );
   }

   /*!
    * Extracts an ICC profile from the current image in this input stream.
    * Returns true if the extraction operation was successful.
    */
   virtual bool Extract( ICCProfile& iccProfile )
   {
      throw NotImplemented( *this, "Extract embedded ICC profiles" );
   }

   /*!
    * Extracts a thumbnail image from the current image in this input stream.
    * Returns true if the extraction operation was successful.
    */
   virtual bool Extract( UInt8Image& thumbnail )
   {
      throw NotImplemented( *this, "Extract thumbnail images" );
   }

   /*!
    * Extracts an image \a property with the specified \a identifier. Returns
    * true if the extraction operation was successful.
    */
   virtual bool Extract( Variant& property, const IsoString& identifier )
   {
      throw NotImplemented( *this, "Extract image properties" );
   }

   /*!
    * Reads a 32-bit floating point image.
    */
   virtual void ReadImage( FImage& image )
   {
      throw NotImplemented( *this, "Read 32-bit floating-point images" );
   }

   /*!
    * Reads a 64-bit floating point image.
    */
   virtual void ReadImage( DImage& image )
   {
      throw NotImplemented( *this, "Read 64-bit floating-point images" );
   }

   /*!
    * Reads a 32-bit floating point complex image.
    */
   virtual void ReadImage( ComplexImage& image )
   {
      throw NotImplemented( *this, "Read 32-bit complex images" );
   }

   /*!
    * Reads a 64-bit floating point complex image.
    */
   virtual void ReadImage( DComplexImage& image )
   {
      throw NotImplemented( *this, "Read 64-bit complex images" );
   }

   /*!
    * Reads an 8-bit unsigned integer image.
    */
   virtual void ReadImage( UInt8Image& image )
   {
      throw NotImplemented( *this, "Read 8-bit integer images" );
   }

   /*!
    * Reads a 16-bit unsigned integer image.
    */
   virtual void ReadImage( UInt16Image& image )
   {
      throw NotImplemented( *this, "Read 16-bit integer images" );
   }

   /*!
    * Reads a 32-bit unsigned integer image.
    */
   virtual void ReadImage( UInt32Image& image )
   {
      throw NotImplemented( *this, "Read 32-bit integer images" );
   }

   /*!
    * Reads an \a image transported by an ImageVariant object.
    */
   virtual void ReadImage( ImageVariant& image )
   {
      if ( !image )
         image.CreateImage( images[index].options.ieeefpSampleFormat,
                            images[index].options.complexSample,
                            images[index].options.bitsPerSample );
   if ( image )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: ReadImage( static_cast<FImage&>( *image ) ); break;
         case 64: ReadImage( static_cast<DImage&>( *image ) ); break;
         }
      else if ( image.IsComplexSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: ReadImage( static_cast<ComplexImage&>( *image ) ); break;
         case 64: ReadImage( static_cast<DComplexImage&>( *image ) ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: ReadImage( static_cast<UInt8Image&>( *image ) ); break;
         case 16: ReadImage( static_cast<UInt16Image&>( *image ) ); break;
         case 32: ReadImage( static_cast<UInt32Image&>( *image ) ); break;
         }
   }

   /*!
    * Incremental read of a 32-bit floating point image.
    *
    * \param[out] buffer      Address of the destination sample buffer.
    * \param      startRow    First pixel row to read.
    * \param      rowCount    Number of pixel rows to read.
    * \param      channel     Channel index to read.
    */
   virtual void Read( FImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental read, 32-bit floating-point samples" );
   }

   /*!
    * Incremental read of a 64-bit floating point image.
    *
    * This is an overloaded member function for the DImage type; see
    * Read( Image::sample*, int, int, int ) for a full description.
    */
   virtual void Read( DImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental read, 64-bit floating-point samples" );
   }

   /*!
    * Incremental read of a 32-bit floating point complex image.
    *
    * This is an overloaded member function for the ComplexImage type; see
    * Read( Image::sample*, int, int, int ) for a full description.
    */
   virtual void Read( ComplexImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental read, 32-bit complex samples" );
   }

   /*!
    * Incremental read of a 64-bit floating point complex image.
    *
    * This is an overloaded member function for the DComplexImage type; see
    * Read( Image::sample*, int, int, int ) for a full description.
    */
   virtual void Read( DComplexImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental read, 64-bit complex samples" );
   }

   /*!
    * Incremental read of an 8-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * Read( Image::sample*, int, int, int ) for a full description.
    */
   virtual void Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental read, 8-bit integer samples" );
   }

   /*!
    * Incremental read of a 16-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * Read( Image::sample*, int, int, int ) for a full description.
    */
   virtual void Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental read, 16-bit integer samples" );
   }

   /*!
    * Incremental read of a 32-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * Read( Image::sample*, int, int, int ) for a full description.
    */
   virtual void Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental read, 32-bit integer samples" );
   }

protected:

   ImageDescriptionArray images; // Descriptions of the images in this stream
   int                   index;  // current image index

private:

   // Image streams are unique objects.
   ImageReader( const ImageReader& ) = delete;
   ImageReader& operator =( const ImageReader& ) = delete;
};

// ----------------------------------------------------------------------------

/*!
 * \class ImageWriter
 * \brief Abstract base class for internal output image stream implementations
 *
 * %ImageWriter provides a generic interface for format-independent output
 * operations on image data streams.
 *
 * The ImageReader and %ImageWriter classes are used internally by standard
 * file format support modules to simplify implementation of common I/O
 * operations. In general, unless you are implementing a standard file format
 * module, you should not need to use these classes.
 *
 * \sa ImageStream, ImageReader
 */
class PCL_CLASS ImageWriter : public virtual ImageStream
{
public:

   /*!
    * Constructs an %ImageWriter object.
    */
   ImageWriter() : ImageStream(), options(), id()
   {
   }

   /*!
    * Destroys an %ImageWriter object.
    */
   virtual ~ImageWriter()
   {
   }

   // N.B.: IsOpen() and Close() redeclared as pure virtual.

   /*!
    */
   virtual bool IsOpen() const = 0;

   /*!
    */
   virtual void Close() = 0;

   /*!
    * Creates a new file for writing at the specified \a path, and prepares for
    * subsequent \a count image write and data embedding operations.
    */
   virtual void Create( const String& path, int count ) = 0;

   /*!
    * Creates a new file for writing at the specified \a path, and prepares for
    * a single image write operation.
    *
    * This is an overloaded member function, equivalent to:
    *
    * \code Create( path, 1 ); \endcode
    */
   virtual void Create( const String& path )
   {
      Create( path, 1 );
   }

   /*!
    */
   virtual bool IsWriter() const
   {
      return true;
   }

   /*!
    * Returns a constant reference to a format-independent options structure
    * corresponding to the next image written by this output stream.
    *
    * \note This member function is virtual because derived classes can
    * restrict access to the format-independent image options structure.
    */
   virtual const ImageOptions& Options() const
   {
      return options;
   }

   /*!
    * Sets a new set of format-independent options for the next image written
    * by this output stream.
    *
    * Only options that modify the writing behavior of the stream will be taken
    * into account. Not all image formats can use all output options available,
    * so any call to this function can be simply ignored by an instance of a
    * derived class.
    *
    * \note This member function is virtual because derived classes can
    * restrict access to the format-independent image options structure.
    */
   virtual void SetOptions( const ImageOptions& a_options )
   {
      options = a_options;
   }

   /*!
    * Returns a reference to a format-independent options structure for the
    * next image written by this output stream.
    *
    * \deprecated This member function has been deprecated and should not be
    * used in newly produced code. Use ImageWriter::SetOptions() instead of
    * this member function.
    */
   virtual ImageOptions& Options()
   {
      return options;
   }

   /*!
    * Sets the identifier of the current image (that is, of the next image that
    * will be written) in this input stream.
    *
    * Not all image formats support image identifiers. If identifiers are not
    * supported, the specified identifier will be ignored on writing the next
    * image.
    */
   void SetId( const IsoString& identifier )
   {
      id = identifier.Trimmed();
   }

   /*!
    * Returns the identifier of the current image (that is, of the next image
    * that will be written) in this output stream.
    *
    * Not all image formats support image identifiers. If no identifier has
    * been set, this function returns an empty string.
    */
   IsoString Id() const
   {
      return id;
   }

   /*!
    * Embeds a set of %FITS header keywords in the current image of this image
    * writer.
    */
   virtual void Embed( const FITSKeywordArray& keywords )
   {
      throw NotImplemented( *this, "Embed FITS keywords" );
   }

   /*!
    * Embeds an ICC profile in the current image of this image writer.
    */
   virtual void Embed( const ICCProfile& iccProfile )
   {
      throw NotImplemented( *this, "Embed ICC profiles" );
   }

   /*!
    * Embeds an 8-bit thumbnail image in the current image of this image
    * writer.
    */
   virtual void Embed( const UInt8Image& thumbnail )
   {
      throw NotImplemented( *this, "Embed thumbnail images" );
   }

   /*!
    * Embeds an image property with the specified \a value and \a identifier.
    */
   virtual void Embed( const Variant& value, const IsoString& identifier )
   {
      throw NotImplemented( *this, "Embed image property" );
   }

   /*!
    * Writes a 32-bit floating point image.
    */
   virtual void WriteImage( const Image& image )
   {
      throw NotImplemented( *this, "Write 32-bit floating-point images" );
   }

   /*!
    * Writes a 64-bit floating point image.
    */
   virtual void WriteImage( const DImage& image )
   {
      throw NotImplemented( *this, "Write 64-bit floating-point images" );
   }

   /*!
    * Writes a 32-bit floating point complex image.
    */
   virtual void WriteImage( const ComplexImage& image )
   {
      throw NotImplemented( *this, "Write 32-bit complex images" );
   }

   /*!
    * Writes a 64-bit floating point complex image.
    */
   virtual void WriteImage( const DComplexImage& image )
   {
      throw NotImplemented( *this, "Write 64-bit complex images" );
   }

   /*!
    * Writes an 8-bit unsigned integer image.
    */
   virtual void WriteImage( const UInt8Image& image )
   {
      throw NotImplemented( *this, "Write 8-bit integer images" );
   }

   /*!
    * Writes a 16-bit unsigned integer image.
    */
   virtual void WriteImage( const UInt16Image& image )
   {
      throw NotImplemented( *this, "Write 16-bit integer images" );
   }

   /*!
    * Writes a 32-bit unsigned integer image.
    */
   virtual void WriteImage( const UInt32Image& image )
   {
      throw NotImplemented( *this, "Write 32-bit integer images" );
   }

   /*!
    * Writes an \a image transported by an ImageVariant object.
    */
   virtual void WriteImage( const ImageVariant& image )
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: WriteImage( static_cast<const FImage&>( *image ) ); break;
            case 64: WriteImage( static_cast<const DImage&>( *image ) ); break;
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: WriteImage( static_cast<const ComplexImage&>( *image ) ); break;
            case 64: WriteImage( static_cast<const DComplexImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: WriteImage( static_cast<const UInt8Image&>( *image ) ); break;
            case 16: WriteImage( static_cast<const UInt16Image&>( *image ) ); break;
            case 32: WriteImage( static_cast<const UInt32Image&>( *image ) ); break;
            }
   }

   /*!
    * Creates a new image with the specified geometry and color space. The data
    * type and other image parameters are defined by current format-independent
    * options (see the Options() member function).
    *
    * This member function can be reimplemented by derived classes supporting
    * incremental write operations.
    */
   virtual void CreateImage( const ImageInfo& info )
   {
      throw NotImplemented( *this, "Create images by geometry" );
   }

   /*!
    * Incremental write of a 32-bit floating point image.
    *
    * \param buffer     Address of the source sample buffer.
    * \param startRow   First pixel row to write.
    * \param rowCount   Number of pixel rows to write.
    * \param channel    Channel index to write.
    */
   virtual void Write( const FImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental write, 32-bit floating-point samples" );
   }

   /*!
    * Incremental write of a 64-bit floating point image.
    *
    * This is an overloaded member function for the DImage type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const DImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental write, 64-bit floating-point samples" );
   }

   /*!
    * Incremental write of a 32-bit floating point complex image.
    *
    * This is an overloaded member function for the ComplexImage type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const ComplexImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental write, 32-bit complex samples" );
   }

   /*!
    * Incremental write of a 64-bit floating point complex image.
    *
    * This is an overloaded member function for the DComplexImage type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const DComplexImage::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental write, 64-bit complex samples" );
   }

   /*!
    * Incremental write of an 8-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental write, 8-bit integer samples" );
   }

   /*!
    * Incremental write of a 16-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental write, 16-bit integer samples" );
   }

   /*!
    * Incremental write of a 32-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
   {
      throw NotImplemented( *this, "Incremental write, 32-bit integer samples" );
   }

protected:

   ImageOptions options; // format-independent options for the next image
   IsoString    id;      // identifier for the next image

private:

   // Image streams are unique objects.
   ImageWriter( const ImageWriter& ) = delete;
   ImageWriter& operator =( const ImageWriter& ) = delete;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageStream_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageStream.h - Released 2015/07/30 17:15:18 UTC
