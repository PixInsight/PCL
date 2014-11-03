// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/FileFormatInstance.h - Released 2014/10/29 07:34:13 UTC
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

#ifndef __PCL_FileFormatInstance_h
#define __PCL_FileFormatInstance_h

/// \file pcl/FileFormatInstance.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
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

#ifndef __PCL_ICCProfile_h
#include <pcl/ICCProfile.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileFormat;

/*!
 * \class FileFormatInstance
 * \brief High-level interface to an image file format instance.
 *
 * %FileFormatInstance, along with %FileFormat, allows full access to any
 * installed file format through intermodule communication.
 *
 * %FileFormatInstance represents a live instance of an installed image format;
 * usually (but not necessarily) a disk file.
 *
 * \sa FileFormat
 */
class PCL_CLASS FileFormatInstance : public UIObject
{
public:

   /*!
    * Constructs a file format instance of the specified file format \a fmt.
    */
   FileFormatInstance( const FileFormat& fmt );

   /*!
    * Destroys a file format instance.
    *
    * After destruction of this instance, the server-side object is also
    * destroyed if it is no longer referenced by other %FileFormatInstance
    * objects. When the server-side object is destroyed, any open files are
    * automatically flushed and closed, as necessary.
    */
   virtual ~FileFormatInstance()
   {
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since file format instances are unique objects by definition, calling
    * this member function has no effect.
    */
   virtual void SetUnique()
   {
   }

   /*!
    * Returns a reference to a <em>null format instance</em>. A null
    * %FileFormatInstance object does not correspond to an existing image file
    * format instance in the PixInsight core application.
    */
   static FileFormatInstance& Null();

   /*!
    * Returns a FileFormat object that represents the installed file format
    * that this instance belongs to.
    */
   FileFormat Format() const;

   /*!
    * Closes an image file (after Open() or Create()).
    *
    * Returns true if the file was successfully closed.
    */
   bool Close();

   /*!
    * Returns true if this file is currently open.
    */
   bool IsOpen() const;

   /*!
    * Returns the absolute file path of this file format instance. If no file
    * path has been set yet (by calling Open() or Create()), this member
    * function returns an empty string.
    */
   String FilePath() const;

   // Reader functionality

   /*!
    * Opens an image file for reading and/or information retrieval.
    *
    * \param images     Reference to a dynamic array of ImageDescription
    *          structures. On successful return, this array will describe the
    *          image(s) stored in this image file. Each %ImageDescription
    *          structure will contain ImageInfo and ImageOptions objects that
    *          describe both basic image parameters (as geometry and color
    *          space) and format-independent image options (pixel format,
    *          embedded data, etc). Descriptive information is provided by the
    *          underlying format module with data retrieved from the opened
    *          image file.
    *
    * \param filePath   File path to an existing file that will be opened.
    *
    * \param hints      A string containing a (possibly empty) list of \e hints
    *          intended to modify the way an image file is opened and/or the
    *          way image data are to be read and decoded. A format module can
    *          simply ignore all of these hints, or just look for one or more
    *          hints that it recognizes and supports, ignoring others. When two
    *          or more hints are specified, they must be separated by space
    *          characters (0x20). Most standard file formats support hints. For
    *          example, the standard DSLR_RAW format recognizes the "raw" hint
    *          to force reading pure raw data (no deBayerization, no black
    *          pedestal subtraction, no white balancing) irrespective of the
    *          current DSLR RAW format settings. The default value is an empty
    *          string.
    *
    * This member function can provide an empty \a images array. If this
    * happens, it means that the file is empty, that is, it contains no image.
    * Although there is no standard file format module that supports empty
    * image files as of writing this documentation, this is a possibility that
    * must be taken into account by calling modules.
    *
    * Returns true if the file was successfully opened; false in the event of
    * error.
    */
   bool Open( ImageDescriptionArray& images,
              const String& filePath, const IsoString& hints = IsoString() );

   /*!
    * Selects the image at the specified zero-based \a index in this file.
    *
    * Returns true if the specified image was successfully selected; false in
    * the event of error.
    *
    * This member function must only be called for file formats that support
    * multiple images stored in a single file. It should not be called if the
    * FileFormat::SupportsMultipleImages() member function returns false for
    * the file format this instance belongs to.
    */
   bool SelectImage( int index );

   /*!
    * Returns the current zero-based image index in this file.
    *
    * This member function must only be called for file formats that support
    * multiple images stored in a single file. It should not be called if the
    * FileFormat::SupportsMultipleImages() member function returns false for
    * the file format this instance belongs to.
    */
   int SelectedImageIndex() const;

   /*!
    * Returns a <em>format-specific data</em> block for the current image in
    * this file, or zero if no such data have been retrieved.
    *
    * See SetFormatSpecificData() for a description of format-specific data
    * functionality in the PCL.
    *
    * Format-specific data blocks do not pertain to the caller's heap, but to
    * the underlying module that implements the file format. Therefore, all
    * format-specific data blocks must be deallocated, when appropriate, by
    * calling FileFormat::DisposeFormatSpecificData().
    */
   const void* FormatSpecificData() const;

   /*!
    * Provides a human-readable summary of format-specific properties for the
    * current image in this file.
    *
    * The returned string should include format-specific information, \e not
    * generic image information. For example, it won't include image
    * dimensions, color space, and other things that can be retrieved with the
    * Open() member function.
    */
   String ImageProperties() const;

   /*!
    * Extraction of embedded keywords for the current image in this file.
    *
    * If the current image embeds FITS header keywords, they will be added
    * to the specified \a keywords list. Otherwise the \a keywords list will be
    * left empty. In any case, previous contents of the \a kwywords list will
    * be destroyed.
    *
    * Returns true if the file access operation was successful, even if no
    * keyword was extracted. Returns false in the event of error.
    */
   bool Extract( FITSKeywordArray& keywords );

   /*!
    * Extraction of an embedded ICC profile for the current image in this file.
    *
    * If the current image embeds an ICC profile, it will be assigned to the
    * specified \a icc object. Otherwise, \a icc will be set to a null profile.
    *
    * Returns true if the file access operation was successful, even if no
    * ICC profile was extracted. Returns false in the event of error.
    */
   bool Extract( ICCProfile& icc );

   /*!
    * Extraction of embedded metadata for the current image in this file.
    *
    * If the current image embeds metadata, the address of a newly allocated
    * copy of them will be assigned to the specified \a data pointer, and its
    * length in bytes will be assigned to the specified \a length variable. If
    * the current image embeds no metadata, zero is assigned to both \a data
    * and \a length.
    *
    * Returns true if the file access operation was successful, even if no
    * metadata were extracted. Returns false in the event of error.
    */
   bool Extract( void*& data, size_type& length );

   /*!
    * Extraction of an embedded thumbnail for the current image in this file.
    *
    * If the current image embeds a thumbnail image, it will be assigned to the
    * specified \a thumbnail 8-bit integer image. Otherwise, a null image will
    * be assigned to \a thumbnail.
    *
    * Returns true if the file access operation was successful, even if no
    * thumbnail image was extracted. Returns false in the event of error.
    */
   bool Extract( UInt8Image& thumbnail );

   /*!
    * Returns a description of all data properties associated with the current
    * image in this file. For each data property, the returned array provides
    * information on the unique identifier of a property and its data type.
    *
    * Returns an empty array if there are no properties stored for the current
    * image in this file.
    *
    * To prevent runtime errors, you should only call this member function for
    * instances of file formats supporting data properties. See
    * FileFormat::CanStoreProperties().
    */
   ImagePropertyDescriptionArray Properties();

   /*!
    * Returns true if the specified property exists associated with the current
    * image in  this file. Returns false if no such property exists.
    *
    * This is a convenience member function, equivalent to the following code:
    *
    * \code Properties().Has( property ); \endcode
    *
    * To prevent runtime errors, you should only call this member function for
    * instances of file formats supporting data properties. See
    * FileFormat::CanStoreProperties().
    */
   bool HasProperty( const IsoString& property )
   {
      return Properties().Has( property );
   }

   /*!
    * Returns a description (unique identifier and data type) of the specified
    * data property associated with the current image. If no such property
    * exists, the returned ImagePropertyDescription object specifies an
    * invalid data type and empty identifier.
    *
    * To prevent runtime errors, you should only call this member function for
    * instances of file formats supporting data properties. See
    * FileFormat::CanStoreProperties().
    */
   ImagePropertyDescription PropertyDescription( const IsoString& property )
   {
      ImagePropertyDescriptionArray properties = Properties();
      ImagePropertyDescriptionArray::const_iterator i = properties.Search( property );
      if ( i == properties.End() )
         return ImagePropertyDescription();
      return *i;
   }

   /*!
    * Extraction of a data property with the specified unique identifier.
    *
    * If no property with the specified identifier exists associated with the
    * current image in this file, an invalid Variant object will be returned.
    *
    * To prevent runtime errors, you should only call this member function for
    * instances of file formats supporting data properties. See
    * FileFormat::CanStoreProperties().
    */
   Variant ReadProperty( const IsoString& property );

   /*!
    * Specifies a data property to be embedded in the next image written or
    * created in this file.
    *
    * \param property   Unique identifier of the data property.
    *
    * \param value      Property value.
    *
    * Returns true only if the embedding operation was successful.
    *
    * To prevent runtime errors, you should only call this member function for
    * instances of file formats supporting data properties. See
    * FileFormat::CanStoreProperties().
    */
   bool WriteProperty( const IsoString& property, const Variant& value );

   /*!
    * Reads the current image in 32-bit floating point format. Returns true if
    * the image was successfully read.
    */
   bool ReadImage( FImage& image );

   /*!
    * Reads the current image in 64-bit floating point format. Returns true if
    * the image was successfully read.
    */
   bool ReadImage( DImage& image );

   /*!
    * Reads the current image in 8-bit unsigned integer format. Returns true if
    * the image was successfully read.
    */
   bool ReadImage( UInt8Image& image );

   /*!
    * Reads the current image in 16-bit unsigned integer format. Returns true if
    * the image was successfully read.
    */
   bool ReadImage( UInt16Image& image );

   /*!
    * Reads the current image in 32-bit unsigned integer format. Returns true if
    * the image was successfully read.
    */
   bool ReadImage( UInt32Image& image );

   /*!
    * Incremental read in 32-bit floating point sample format.
    *
    * \param[out] buffer      Address of the destination sample buffer.
    *
    * \param      startRow    First pixel row to read.
    *
    * \param      rowCount    Number of pixel rows to read.
    *
    * \param      channel     Channel index to read.
    *
    * Returns true if the specified pixel row(s) were successfully read; false
    * in the event of error, or if the operation was cancelled.
    *
    * Incremental read operations allow loading images by successive pixel row
    * strips. They are useful to manage very large files, or large sets of
    * image files that don't fit in the available RAM.
    *
    * When a file format implements incremental reading, the corresponding
    * FileFormat::CanReadIncrementally() returns true. Otherwise this function
    * should not be invoked - it will throw an exception if called for a file
    * format that doesn't support incremental reading.
    */
   bool Read( FImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 64-bit floating point sample format.
    *
    * This is an overloaded member function for the DImage type; see
    * Read( FImage::sample*, int, int, int, int ) for a full description.
    */
   bool Read( DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 8-bit unsigned integer sample format.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * Read( FImage::sample*, int, int, int, int ) for a full description.
    */
   bool Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 16-bit unsigned integer sample format.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * Read( FImage::sample*, int, int, int, int ) for a full description.
    */
   bool Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 32-bit unsigned integer sample format.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * Read( FImage::sample*, int, int, int, int ) for a full description.
    */
   bool Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Returns true if the last file read operation was \e inexact.
    *
    * This function can be called just after a successful call to ReadImage()
    * or Read(). If a file format instance reads source images inexactly,
    * (e.g., by applying a rounding function to source sample values), then
    * this function should return true.
    */
   bool WasInexactRead() const;

   // Writer functionality

   /*!
    * Queries image options and format-specific data before creating a new
    * image file. Returns true if the file creation operation can continue;
    * false if it should be aborted.
    *
    * \param[out] options   Reference to a dynamic array of ImageOptions
    *          objects. Each of these objects provides a general description of
    *          an image stored in a file instance of this file format. The
    *          underlying file format module is responsible for setting the
    *          appropriate values in the passed structures. The returned
    *          options can be used to generate a new file through a subsequent
    *          call to Create(). Formats that support multiple images per file
    *          can receive more than one %ImageOptions structure stored in this
    *          array. Passing more than one %ImageOptions object to a format
    *          that doesn't support multiple images will be caught and blocked
    *          by the PixInsight core application.
    *
    * \param[out] formatOptions  Reference to a dynamic array of
    *          <em>format-specific data blocks</em>. Each void pointer in this
    *          array can be either zero or point to valid format-specific data
    *          for the format of this instance. This array can be empty if no
    *          format-specific data is being set. See the
    *          SetFormatSpecificData() member function for more information on
    *          format-specific data blocks.
    *
    * A module should call this function just before calling Create(), as part
    * of a <tt>File > Save As</tt> operation, or equivalent. Typically, the
    * underlying format module will open a dialog box to let the user modify
    * some working options. This function will return true unless the user
    * cancels the format options dialog.
    */
   bool QueryOptions( Array<ImageOptions>& options, Array<const void*>& formatOptions );

   /*!
    * Creates an image file for writing.
    *
    * \param filePath   Path to a file that will be created. If a file exists
    *          at the same path, it will be overwritten and its current
    *          contents will be lost.
    *
    * \param hints      A string containing a (possibly empty) list of \e hints
    *          intended to modify the way an image file is generated and/or the
    *          way image data are to be encoded and written. A format module
    *          can simply ignore all of these hints, or just look for one or
    *          more hints that it recognizes and supports, ignoring others.
    *          When two or more hints are specified, they must be separated by
    *          space characters (0x20). Many standard file formats support some
    *          hints. For example, the standard JPEG format recognizes the
    *          "quality &lt;n&gt;" hint to force generation of a JPEG image
    *          with a given quality level &lt;n&gt; in the range 1 to 100,
    *          irrespective of the current JPEG format settings.
    *
    * \param numberOfImages   This is the number of images that will be written
    *          to the file after creation. It can be zero if an empty file is
    *          being created, and also less than zero if the number of images
    *          is unknown or cannot be defined at the point of creation. This
    *          parameter defaults to one.
    *
    * Returns true if the file was successfully created. Returns false in the
    * event of error.
    */
   bool Create( const String& filePath, const IsoString& hints = IsoString(), int numberOfImages = 1 );

   /*!
    * Specifies an identifier for the next image that will be written or
    * created in this file.
    *
    * Returns true if the identifier was successfully assigned. Returns false
    * in the event of error.
    *
    * \note Most file format implementations don't support image identifiers
    * and simply ignore a call to this function, reporting success.
    */
   bool SetId( const IsoString& id );

   /*!
    * Specifies a set of format-independent image \a options for the next image
    * that will be written or created in this file.
    *
    * Returns true if the options were successfully assigned. Returns false in
    * the event of error.
    *
    * \note A file format implementations may ignore any format-independent
    * options set with a call to this function, reporting success.
    */
   bool SetOptions( const ImageOptions& options );

   /*!
    * Specifies a <em>format-specific data block</em> for the next image that
    * will be written or created in this file.
    *
    * Format-specific data are arbitrary data blocks; the PixInsight core
    * application knows nothing about them except that it keeps them for all
    * open image files and passes them among instances, even instances of
    * different file formats. The core application deallocates format-specific
    * data blocks when appropriate, by invoking the corresponding module
    * deallocation routines.
    *
    * Format-specific data are often used by image file formats to store
    * working data on a per-instance basis (which usually means on a
    * <em>per-file</em> basis), such as compression modes and ratios, file
    * organization modes, and many other critical working parameters that can
    * persist across different file format instances.
    *
    * When a file format uses (or can use) format-specific data, the
    * corresponding FileFormat::UsesFormatSpecificData() member function
    * returns true.
    *
    * Returns true if the specified data block was accepted by this instance.
    * Returns false if the data block is invalid, or in the event of error.
    *
    * \a note A module should verify validity of a format-specific data block
    * with FileFormat::ValidateFormatSpecificData() before calling this
    * function to pass the block to an instance.
    */
   bool SetFormatSpecificData( const void* data );

   /*!
    * Specifies a set of %FITS \a keywords to be embedded in the next image
    * written or created in this file.
    *
    * Returns true if the specified \a keywords list was successfully embedded
    * in the image; false in the event of error.
    */
   bool Embed( const FITSKeywordArray& keywords );

   /*!
    * Specifies an ICC profile to be embedded in the next image written or
    * created in this file.
    *
    * Returns true if the specified ICC profile was successfully embedded in
    * the image; false in the event of error.
    */
   bool Embed( const ICCProfile& icc );

   /*!
    * Specifies a metadata block to be embedded in the next image written or
    * created in this file.
    *
    * \param data    Starting address of the source metadata block.
    *
    * \param length  Length in bytes of the source metadata block.
    *
    * Returns true if the specified metadata were successfully embedded in the
    * image; false in the event of error.
    */
   bool Embed( const void* data, size_type length );

   /*!
    * Specifies a thumbnail \a image to be embedded in the next image written
    * or created in this file.
    *
    * Returns true if the specified thumbnail \a image was successfully
    * embedded in the image; false in the event of error.
    */
   bool Embed( const UInt8Image& image );

   /*!
    * Writes a 32-bit floating point image to this file. Returns true if the
    * image was successfully written.
    */
   bool WriteImage( const FImage& image );

   /*!
    * Writes a 64-bit floating point image to this file. Returns true if the
    * image was successfully written.
    */
   bool WriteImage( const DImage& image );

   /*!
    * Writes an 8-bit unsigned integer image to this file. Returns true if the
    * image was successfully written.
    */
   bool WriteImage( const UInt8Image& image );

   /*!
    * Writes a 16-bit unsigned integer image to this file. Returns true if the
    * image was successfully written.
    */
   bool WriteImage( const UInt16Image& image );

   /*!
    * Writes a 32-bit unsigned integer image to this file. Returns true if the
    * image was successfully written.
    */
   bool WriteImage( const UInt32Image& image );

   /*!
    * Creates a new image with the geometry and color space as specified by an
    * ImageInfo structure. The newly created image will be written by
    * subsequent incremental write operations.
    *
    * The sample data type and other format-independent and format-specific
    * image parameters have been specified by previous calls to SetOptions()
    * and SetFormatSpecificData().
    *
    * Returns true if the image was successfully created.
    *
    * \note This member function should not be invoked for file formats that
    * don't support incremental write operations.
    */
   bool CreateImage( const ImageInfo& info );

   /*!
    * Incremental write of 32-bit floating point pixel samples.
    *
    * \param buffer     Address of the source sample buffer.
    *
    * \param startRow   First pixel row to write.
    *
    * \param rowCount   Number of pixel rows to write.
    *
    * \param channel    Channel index to write.
    *
    * Returns true if the specified pixel row(s) were successfully written;
    * false in the event of error, or if the operation was cancelled.
    *
    * Incremental write operations allow the PixInsight core application and
    * other modules to generate images by successive row strips. They are
    * useful to generate extremely large images, or large sets of images that
    * don't fit in the available RAM.
    *
    * When a file format implements incremental writing, the corresponding
    * FileFormat::CanWriteIncrementally() returns true. Otherwise this function
    * should not be invoked - it will throw an exception if called for a file
    * format that doesn't support incremental writing.
    */
   bool Write( const FImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of a 64-bit floating point image.
    *
    * This is an overloaded member function for the DImage type; see
    * Write( const FImage::sample*, int, int, int ) for a full description.
    */
   bool Write( const DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of an 8-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * Write( const FImage::sample*, int, int, int ) for a full description.
    */
   bool Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of a 16-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * Write( const FImage::sample*, int, int, int ) for a full description.
    */
   bool Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of a 32-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * Write( const FImage::sample*, int, int, int ) for a full description.
    */
   bool Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Returns true if the last file write operation in this file was \e lossy.
    *
    * This function can ve called just after successful completion of a call to
    * WriteImage() or Write(). If a file format instance writes lossy image
    * data (e.g., by applying some lossy compression scheme), then this
    * function will return true.
    */
   bool WasLossyWrite() const;

private:

   FileFormatInstance( void* );

   virtual void* CloneHandle() const;

   friend class FileFormatInstancePrivate;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_FileFormatInstance_h

// ****************************************************************************
// EOF pcl/FileFormatInstance.h - Released 2014/10/29 07:34:13 UTC
