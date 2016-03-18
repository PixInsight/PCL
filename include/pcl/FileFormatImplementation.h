//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/FileFormatImplementation.h - Released 2016/02/21 20:22:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_FileFormat_h
#define __PCL_FileFormat_h

/// \file pcl/FileFormatImplementation.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_AutoPointer_h
#include <pcl/AutoPointer.h>
#endif

#ifndef __PCL_MetaFileFormat_h
#include <pcl/MetaFileFormat.h>
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

#ifndef __PCL_RGBColorSystem_h
#include <pcl/RGBColorSystem.h>
#endif

#ifndef __PCL_DisplayFunction_h
#include <pcl/DisplayFunction.h>
#endif

#ifndef __PCL_ColorFilterArray_h
#include <pcl/ColorFilterArray.h>
#endif

namespace pcl
{

struct FileFormatImplementationPrivate;

// ----------------------------------------------------------------------------

/*!
 * \class FileFormatImplementation
 * \brief Implementation of a PixInsight file format instance
 *
 * In the PixInsight/PCL framework, an <em>image file format</em> is formally
 * defined as a descendant of the MetaFileFormat class.
 * %FileFormatImplementation defines the behavior and functionality of a file
 * format instance, which usually (although not necessarily) identifies an
 * <em>image file</em> stored in the corresponding file format.
 *
 * Note that %MetaFileFormat and %FileFormatImplementation describe and
 * implement, respectively, an \e installable image file format in a PixInsight
 * module. All installed file formats can be accessed and instantiated by means
 * of the FileFormat and FileFormatInstance classes, respectively, through
 * intermodule communication mechanisms.
 *
 * \sa MetaFileFormat, FileFormat, FileFormatInstance
 */
class PCL_CLASS FileFormatImplementation
{
public:

   /*!
    * Constructs a file format instance.
    *
    * \param m    Pointer to a metaformat that identifies the file format class
    *             that this file instance belongs to.
    */
   FileFormatImplementation( const MetaFileFormat* m );

   /*!
    * Destroys a file format instance.
    */
   virtual ~FileFormatImplementation();

   /*!
    * Returns a pointer to the \e metaformat of this file format instance.
    *
    * The metaformat defines the format class this instance belongs to.
    */
   const MetaFileFormat* Meta() const
   {
      return meta;
   }

   /*!
    * Closes an image file (after Open() or Create()).
    */
   virtual void Close();

   /*!
    * Returns true iff this file is currently open.
    */
   virtual bool IsOpen() const;

   /*!
    * Returns the current file path in this instance. If no file path has been
    * set yet (by calling Open() or Create()), this member function should
    * return an empty string.
    */
   virtual String FilePath() const;

   // Reader functionality

   /*!
    * Opens an image file for reading and/or information retrieval.
    *
    * \param filePath   File path to a file that will be opened. The core
    *          PixInsight application always passes full file paths to existing
    *          files. However, other modules that may call this member function
    *          (through intermodule communication) may not behave in the same
    *          way. A module must be prepared to receive relative file path
    *          specifications, and even paths to files that don't exist, in
    *          this argument.
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
    *          current DSLR RAW format settings.
    *
    * This function must return a dynamic array of ImageDescription structures
    * that describes the image(s) stored in the opened image file. Each
    * %ImageDescription structure contains ImageInfo and ImageOptions objects
    * that describe both basic image parameters (as geometry and color space)
    * and format-independent image options (pixel format, embedded data, etc).
    *
    * In the event of error, a reimplementation of this member function should
    * throw a PCL exception. Thrown exceptions will be caught and processed by
    * internal PCL code.
    */
   virtual ImageDescriptionArray Open( const String& filePath, const IsoString& hints );

   /*!
    * Selects the image at the specified zero-based \a index in this file.
    *
    * This member function will only be called for file formats that support
    * multiple images stored in a single file. It will never be called if the
    * MetaFileFormat::SupportsMultipleImages() member function has not been
    * reimplemented to return true in the metaformat class for this instance.
    */
   virtual void SelectImage( int index );

   /*!
    * Returns the current zero-based image index in this file.
    *
    * This member function will only be called for file formats that support
    * multiple images stored in a single file. It will never be called if the
    * MetaFileFormat::SupportsMultipleImages() member function has not been
    * reimplemented to return true in the metaformat class for this instance.
    */
   virtual int SelectedImageIndex() const;

   /*!
    * Returns a <em>format-specific data block</em> for the current image in
    * this file, or zero if no such data have been retrieved.
    *
    * See SetFormatSpecificData() for a description of format specific data
    * functionality in PCL.
    */
   virtual void* FormatSpecificData() const;

   /*!
    * Provides a human-readable summary of format-specific properties for the
    * current image in this file.
    *
    * The returned string should include format-specific information, \e not
    * generic image information. For example, don't include image dimensions,
    * color space, and other things that the PixInsight core application
    * already knows.
    *
    * \note The default implementation returns an empty string.
    */
   virtual String ImageProperties() const
   {
      return String();
   }

   /*!
    * Extraction of embedded %FITS keywords for the current image in this file.
    *
    * If the current image embeds FITS keywords, they must be appended to the
    * specified \a keywords list.
    */
   virtual void Extract( FITSKeywordArray& keywords );

   /*!
    * Extraction of an embedded ICC profile for the current image in this file.
    *
    * If the current image embeds an ICC profile, it should be assigned to the
    * specified \a icc object.
    */
   virtual void Extract( ICCProfile& icc );

   /*!
    * Extraction of an embedded thumbnail for the current image in this file.
    *
    * If the current image embeds a thumbnail image, it should be assigned to
    * the specified \a thumbnail 8-bit integer image.
    */
   virtual void Extract( pcl::UInt8Image& thumbnail );

   /*!
    * Returns a description of all data properties associated with the current
    * image in this file. For each data property, the returned array provides
    * information on the unique identifier of a property and its data type.
    *
    * Returns an empty array if there are no properties stored for the current
    * image in this file.
    *
    * This member function will never be called if the underlying file format
    * does not support data properties. See FileFormat::CanStoreProperties().
    */
   virtual ImagePropertyDescriptionArray Properties();

   /*!
    * Extraction of a data property with the specified unique identifier.
    *
    * If no property with the specified identifier exists associated with the
    * current image in this file, an invalid Variant object should be returned.
    *
    * This member function will never be called if the underlying file format
    * does not support data properties. See FileFormat::CanStoreProperties().
    */
   virtual Variant ReadProperty( const IsoString& property );

   /*!
    * Specifies a data property to be embedded in the next image written or
    * created in this file.
    *
    * \param property   Unique identifier of the data property.
    *
    * \param value      Property value.
    *
    * This member function will never be called if the underlying file format
    * does not support data properties. See FileFormat::CanStoreProperties().
    */
   virtual void WriteProperty( const IsoString& property, const Variant& value );

   /*!
    * Extraction of the RGB working space associated with the current image in
    * this file. If no RGBWS is defined for the current image, this function
    * should return a default RGBColorSystem object (see RGBColorSystem::sRGB).
    *
    * This member function will never be called if the underlying file format
    * does not support storage of RGB working spaces. See
    * FileFormat::CanStoreRGBWS().
    */
   virtual RGBColorSystem ReadRGBWS();

   /*!
    * Specifies the parameters of an RGB working space that will be embedded in
    * the next image written or created in this file.
    */
   virtual void WriteRGBWS( const RGBColorSystem& rgbws );

   /*!
    * Extraction of the display function associated with the current image in
    * this file. If no display function is defined for the current image, this
    * function should return an identity display function (see
    * DisplayFunction's default constructor).
    *
    * This member function will never be called if the underlying file format
    * does not support storage of display functions. See
    * FileFormat::CanStoreDisplayFunctions().
    */
   virtual DisplayFunction ReadDisplayFunction();

   /*!
    * Specifies a display function that will be embedded in the next image
    * written or created in this file.
    */
   virtual void WriteDisplayFunction( const DisplayFunction& df );

   /*!
    * Extraction of the color filter array (CFA) for the current image in this
    * file. If no CFA is defined for the current image, this function should
    * return an empty CFA (see ColorFilterArray's default constructor).
    *
    * This member function will never be called if the underlying file format
    * does not support storage of color filter arrays. See
    * FileFormat::CanStoreColorFilterArrays().
    */
   virtual ColorFilterArray ReadColorFilterArray();

   /*!
    * Specifies a color filter array (CFA) that will be embedded in the next
    * image written or created in this file.
    */
   virtual void WriteColorFilterArray( const ColorFilterArray& cfa );

   /*!
    * Reads the current image in 32-bit floating point format.
    */
   virtual void ReadImage( pcl::Image& image );

   /*!
    * Reads the current image in 64-bit floating point format.
    */
   virtual void ReadImage( pcl::DImage& image );

   /*!
    * Reads the current image in 8-bit unsigned integer format.
    */
   virtual void ReadImage( UInt8Image& image );

   /*!
    * Reads the current image in 16-bit unsigned integer format.
    */
   virtual void ReadImage( UInt16Image& image );

   /*!
    * Reads the current image in 32-bit unsigned integer format.
    */
   virtual void ReadImage( UInt32Image& image );

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
    * Incremental read operations allow the PixInsight core application and
    * other modules to load images by successive row strips.
    *
    * To implement incremental reading,
    * MetaFileFormat::CanReadIncrementally() must be reimplemented to return
    * true in the metaformat class for this file instance; otherwise this
    * member function will never be called.
    */
   virtual void Read( pcl::Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 64-bit floating point sample format.
    *
    * This is an overloaded member function for the DImage type; see
    * Read( Image::sample*, int, int, int, int ) for a full description.
    */
   virtual void Read( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 8-bit unsigned integer sample format.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * Read( Image::sample*, int, int, int, int ) for a full description.
    */
   virtual void Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 16-bit unsigned integer sample format.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * Read( Image::sample*, int, int, int, int ) for a full description.
    */
   virtual void Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental read in 32-bit unsigned integer sample format.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * Read( Image::sample*, int, int, int, int ) for a full description.
    */
   virtual void Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Returns true iff the last file read operation was \e inexact.
    *
    * The PixInsight application invokes this function just after successful
    * completion of a call to ReadImage() or Read(). If a file format instance
    * reads (or might read) source images inexactly, (e.g., by applying a
    * rounding function to source sample values), then this function must be
    * reimplemented to return true, as appropriate.
    *
    * \note The default implementation of this function returns false, so
    * exact read operations are assumed by default.
    */
   virtual bool WasInexactRead() const
   {
      return false;  // exact read operations assumed by default
   }

   // Writer functionality

   /*!
    * Query image options and format-specific data before creating a new image
    * file. Returns true if the file creation operation can continue; false if
    * it should be aborted.
    *
    * \param[out] options   Reference to a dynamic array of ImageOptions
    *          objects. On output, each of these objects must provide a general
    *          description of an image stored in a file instance of this file
    *          format. A file format module is responsible for setting the
    *          appropriate values in the passed structures, which will be used
    *          to generate a new file through a subsequent call to Create().
    *          Formats that can store multiple images per file can receive more
    *          than one %ImageOptions structure stored in this array. Formats
    *          that don't support multiple images can safely ignore all
    *          structures but the first one in this array.
    *
    * \param[out] formatOptions  Reference to a dynamic array of
    *          <em>format-specific data blocks</em>. Each void pointer in this
    *          array can be either zero or point to valid format-specific data
    *          for the format of this instance. This array can be empty if no
    *          format-specific data is being set. See the
    *          SetFormatSpecificData() member function for more information on
    *          format-specific data blocks.
    *
    * If the PixInsight application calls this function, it does so just before
    * calling Create(). Typically, this happens as part of a <tt>File > Save
    * As</tt> operation. Other modules can also invoke this function through
    * intermodule communication. Format modules should open a dialog box to let
    * the user modify some working options, as appropriate; then they should
    * return true unless the user cancels the dialog.
    *
    * \note The default implementation of this function simply returns true
    * without opening an options dialog.
    */
   virtual bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions )
   {
      return true;
   }

   /*!
    * Creates an image file for writing.
    *
    * \param filePath   Path to a file that will be created. If a file exists
    *          at the same path, it will be overwritten and its current
    *          contents will be lost. The PixInsight core application always
    *          passes full file paths to existing files. However, other modules
    *          that may call this member function (through intermodule
    *          communication) may not behave in the same way. A module must be
    *          prepared to receive relative file path specifications, and even
    *          invalid paths, in this argument.
    *
    * \param numberOfImages   This is the number of images that will be written
    *          to the file after creation. It can be zero if an empty file is
    *          being created, and also less than zero if the number of images
    *          is unknown or cannot be defined at the point of creation.
    *          Although the PixInsight core application will always pass the
    *          exact number of images that will be written, other modules that
    *          can call this member function (through intermodule
    *          communication) might not behave so accurately. Therefore, format
    *          modules should be flexible enough as to not depend on an
    *          accurate count of images passed here. For a format that does not
    *          support empty files or multiple images stored in a single file,
    *          a reimplementation of this member function should throw an Error
    *          exception if this value is zero or not equal to one,
    *          respectively.
    *
    * \param hints      A string containing a (possibly empty) list of \e hints
    *          intended to modify the way an image file is generated and/or the
    *          way image data are to be encoded and written. A format module
    *          can simply ignore all of these hints, or just look for one or
    *          more hints that it recognizes and supports, ignoring others.
    *          When two or more hints are specified, they must be separated by
    *          space characters (0x20). Many standard file formats support some
    *          hints. For example, the standard JPEG format recognizes the
    *          "quality" hint to force generation of a JPEG image with a given
    *          quality level in the range 1 to 100, irrespective of the current
    *          JPEG format settings.
    *
    * In the event of error, a reimplementation of this member function should
    * throw a PCL exception. Thrown exceptions will be caught and processed by
    * internal PCL code.
    */
   virtual void Create( const String& filePath, int numberOfImages, const IsoString& hints );

   /*!
    * Specifies an identifier for the next image that will be written or
    * created in this file.
    *
    * \note Reimplementation of this member function is optional. The default
    * implementation does nothing (ignores image identifiers).
    */
   virtual void SetId( const IsoString& id )
   {
   }

   /*!
    * Specifies a set of format-independent image \a options for the next image
    * that will be written or created in this file.
    *
    * \note Reimplementation of this member function is optional. The default
    * implementation does nothing (ignores format-independent image options).
    */
   virtual void SetOptions( const ImageOptions& options )
   {
   }

   /*!
    * Specifies a <em>format-specific data block</em> for the next image that
    * will be written or created in this file.
    *
    * Format-specific data are arbitrary data blocks; the PixInsight core
    * application knows nothing about them except that it passes them among
    * instances, even instances of different file formats.
    *
    * Format-specific data are used by file formats that need working
    * parameters that must persist across different file instances. In such
    * cases, derived classes should implement suitable mechanisms to identify
    * and validate their own data.
    *
    * For example, file formats that use variable compression schemes usually
    * reimplement this member function, along with FormatSpecificData(), to
    * keep track of a compression ratio, along with other private data items.
    *
    * To implement format-specific data support,
    * MetaFileFormat::UsesFormatSpecificData() must be reimplemented to return
    * true in the metaformat class for this file instance; otherwise this
    * member function will never be called.
    *
    * Before calling this function, the PixInsight core application will call
    * MetaFileFormat::ValidateFormatSpecificData() for the same \a data block.
    * If the validation routine returns false, this function will not be called
    * for the \a data block that failed the validation test.
    */
   virtual void SetFormatSpecificData( const void* data );

   /*!
    * Specifies a set of %FITS \a keywords to be embedded in the next image
    * written or created in this file.
    */
   virtual void Embed( const FITSKeywordArray& keywords );

   /*!
    * Specifies an ICC profile to be embedded in the next image written or
    * created in this file.
    */
   virtual void Embed( const ICCProfile& icc );

   /*!
    * Specifies a thumbnail \a image to be embedded in the next image written
    * or created in this file.
    */
   virtual void Embed( const pcl::UInt8Image& image );

   /*!
    * Writes a 32-bit floating point image to this file.
    */
   virtual void WriteImage( const pcl::Image& image );

   /*!
    * Writes a 64-bit floating point image to this file.
    */
   virtual void WriteImage( const pcl::DImage& image );

   /*!
    * Writes an 8-bit unsigned integer image to this file.
    */
   virtual void WriteImage( const UInt8Image& image );

   /*!
    * Writes a 16-bit unsigned integer image to this file.
    */
   virtual void WriteImage( const UInt16Image& image );

   /*!
    * Writes a 32-bit unsigned integer image to this file.
    */
   virtual void WriteImage( const UInt32Image& image );

   /*!
    * Creates a new image with the geometry and color space as specified by an
    * ImageInfo structure. The newly created image will be written by
    * subsequent incremental write operations.
    *
    * The sample data type and other format-independent and format-specific
    * image parameters have been specified by previous calls to SetOptions()
    * and SetFormatSpecificData().
    *
    * \note This member function must be reimplemented by all derived classes
    * supporting incremental write operations.
    */
   virtual void CreateImage( const ImageInfo& info );

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
    * Incremental write operations allow the PixInsight core application and
    * other modules to generate images by successive row strips.
    *
    * To implement incremental writing,
    * MetaFileFormat::CanWriteIncrementally() must be reimplemented to return
    * true in the metaformat class for this file instance; otherwise this
    * member function will never be called.
    */
   virtual void Write( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of a 64-bit floating point image.
    *
    * This is an overloaded member function for the DImage type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of an 8-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of a 16-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental write of a 32-bit unsigned integer image.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * Write( const Image::sample*, int, int, int ) for a full description.
    */
   virtual void Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Returns true iff the last file write operation in this file was \e lossy.
    *
    * The PixInsight core application invokes this function just after
    * successful completion of a call to WriteImage() or Write(). If a file
    * format instance writes (or might write) lossy image data (e.g., by
    * applying some lossy compression scheme), then this function must be
    * reimplemented to return true, as appropriate.
    *
    * \note The default implementation of this function returns false, so
    * lossless write operations are assumed by default.
    */
   virtual bool WasLossyWrite() const
   {
      return false;  // lossless write operations assumed by default
   }

protected:

   const MetaFileFormat* meta;

private:

   /*
    * FileFormatImplementation instances (e.g., image files) are unique.
    */
   FileFormatImplementation( const FileFormatImplementation& x ) = delete;
   void operator =( const FileFormatImplementation& ) = delete;

   /*
    * Internal stuff to automate low-level C API communication.
    */

   AutoPointer<FileFormatImplementationPrivate> m_data;

   ImageDescriptionArray m_description; // used exclusively by FileFormatDispatcher

   void BeginPrivate();

   void BeginKeywordExtraction();
   size_type NumberOfKeywords() const;
   bool GetNextKeyword( FITSHeaderKeyword& ) const;
   void EndKeywordExtraction();

   void BeginICCProfileExtraction();
   const ICCProfile& GetICCProfile() const;
   void EndICCProfileExtraction();

   void BeginThumbnailExtraction();
   const UInt8Image& GetThumbnail() const;
   void EndThumbnailExtraction();

   void BeginPropertyExtraction();
   const Variant& GetImageProperty( const IsoString& );
   void EndPropertyExtraction();

   void BeginRGBWSExtraction();
   const RGBColorSystem& GetRGBWS() const;
   void EndRGBWSExtraction();

   void BeginDisplayFunctionExtraction();
   const DisplayFunction& GetDisplayFunction() const;
   void EndDisplayFunctionExtraction();

   void BeginColorFilterArrayExtraction();
   const ColorFilterArray& GetColorFilterArray() const;
   void EndColorFilterArrayExtraction();

   void BeginKeywordEmbedding();
   void AddKeyword( const FITSHeaderKeyword& );
   void EndKeywordEmbedding();

   void BeginICCProfileEmbedding();
   void SetICCProfile( const ICCProfile& );
   void EndICCProfileEmbedding();

   void BeginThumbnailEmbedding();
   void SetThumbnail( const UInt8Image& );
   void EndThumbnailEmbedding();

   void BeginPropertyEmbedding();
   void SetImageProperty( const IsoString&, const Variant& );
   void EndPropertyEmbedding();

   void BeginRGBWSEmbedding();
   void SetRGBWS( const RGBColorSystem& );
   void EndRGBWSEmbedding();

   void BeginDisplayFunctionEmbedding();
   void SetDisplayFunction( const DisplayFunction& );
   void EndDisplayFunctionEmbedding();

   void BeginColorFilterArrayEmbedding();
   void SetColorFilterArray( const ColorFilterArray& );
   void EndColorFilterArrayEmbedding();

   friend class FileFormatDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_FileFormat_h

// ----------------------------------------------------------------------------
// EOF pcl/FileFormatImplementation.h - Released 2016/02/21 20:22:12 UTC
