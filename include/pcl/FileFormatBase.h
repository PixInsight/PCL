//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/FileFormatBase.h - Released 2017-06-09T08:12:42Z
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

#ifndef __PCL_FileFormatBase_h
#define __PCL_FileFormatBase_h

/// \file pcl/FileFormatBase.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Bitmap.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class FileFormatBase
 * \brief Abstract base class for image file format descriptions.
 *
 * %FileFormatBase defines a number of descriptive properties of a file format
 * in the PixInsight platform.
 *
 * %FileFormatBase is an abstract base class of the MetaFileFormat and
 * FileFormat instantiable classes. %MetaFileFormat describes an installable
 * file format, while the %FileFormat class provides direct access to an
 * installed file format through <em>intermodule communication</em>.
 *
 * In more practical terms, modules defining new file formats must implement
 * derived classes of %MetaFileFormat, while %FileFormat is used by modules
 * requiring direct access to image files through installed file formats.
 *
 * Note that the %FileFormatBase, %MetaFileFormat, %FileFormat,
 * FileFormatImplementation and FileFormatInstance classes are conceptually
 * parallel to the ProcessBase, MetaProcess, Process, ProcessImplementation and
 * ProcessInstance classes, respectively. This is just a reflection of the
 * strong object orientation and modularity that characterize the PixInsight
 * platform.
 *
 * \sa MetaFileFormat, FileFormat, FileFormatImplementation, FileFormatInstance
 */
class PCL_CLASS FileFormatBase
{
public:

   /*!
    * Constructs a %FileFormatBase object.
    */
   FileFormatBase()
   {
   }

   /*!
    * Destroys this %FileFormatBase object.
    */
   virtual ~FileFormatBase() noexcept( false )
   {
   }

   /*!
    * Returns the identifier of this file format (also known as the
    * <em>format name</em>).
    *
    * File format identifiers are unique, valid C identifiers.
    * Examples: "FITS", "TIFF", "JPEG2000".
    */
   virtual IsoString Name() const = 0;

   /*!
    * Returns the list of file extensions associated to this file format.
    *
    * The returned list must be a sequence of ".xxx...x" strings in priority
    * order. Examples: ".fit", ".fits", ".fts".
    */
   virtual StringList FileExtensions() const = 0;

   /*!
    * Returns a list of MIME types corresponding to the data supported by
    * this file format.
    *
    * The returned list must be a sequence of "media_type/content_type" items
    * approved by IANA (see http://www.iana.org/assignments/media-types/), for
    * example: "image/fits", "application/fits".
    *
    * Providing a list of MIME types is not mandatory, but highly recommended
    * for all format support modules implementing standard (i.e., recognized by
    * IANA) image formats.
    */
   virtual IsoStringList MimeTypes() const = 0;

   /*!
    * Returns a version number for this file format, encoded as a hexadecimal
    * number.
    *
    * For example, version 1.0.5 should be returned as 0x105, and version
    * 3.11.5 as 0x3B5. The default return value is 0x100, corresponding to
    * version 1.0.0.
    */
   virtual uint32 Version() const = 0;

   /*!
    * Returns a brief description text for this file format.
    *
    * This function should provide a simple, typically single-line, description
    * of this image file format for quick reference. Example: "Flexible Image
    * Transport System". The Implementation() member function has been designed
    * to provide a more complete description of a format's functionality and
    * capabilities.
    */
   virtual String Description() const = 0;

   /*!
    * Returns a descriptive text about this implementation of a particular
    * image file format.
    *
    * This function must provide a brief but sufficiently informative
    * description of this file format implementation. The returned description
    * will appear on the Format Explorer window, and should provide information
    * about how this format has been implemented in a module. Avoid too
    * exhaustive descriptions that are better reserved for a technical manual.
    * Avoid also describing a file format itself; the information given should
    * not intend to replace an official/formal definition of an image format.
    *
    * Descriptions of file format implementations are always printed on
    * PixInsight consoles. This means that the text output functionality of the
    * Console class can be used to format the string returned by this function.
    * Refer to that class and its documentation for further information.
    */
   virtual String Implementation() const = 0;

   /*!
    * Returns a description of the current status of this file format
    * implementation.
    *
    * This function should return an empty string for normal file format
    * implementations. Exceptions to this rule are obsolete or deprecated file
    * formats (see the IsDeprecated() member function), deficient
    * implementations, or other special cases where the user should be aware of
    * important potential problems or limitations.
    *
    * The output of this function should be essentially plain text with basic
    * HTML tags. No console tags should be used.
    */
   virtual String Status() const = 0;

   /*!
    * Returns a <em>large icon</em> image that identifies this format.
    *
    * The returned image is used to identify all instances of this format
    * (e.g., images and files) in the core application's GUI. It is used on the
    * Format Explorer window, on image icons of this format, and in general for
    * every graphical item related to this format or to an instance of this
    * format.
    */
   virtual Bitmap Icon() const = 0;

   /*!
    * Returns a <em>small icon</em> image that identifies this format.
    *
    * For details on format icon images, see the documentation for Icon().
    *
    * Small icons are used on interface elements where screen space must be
    * preserved. Two good examples are the Format Explorer window and the
    * ImageContainer interface.
    */
   virtual Bitmap SmallIcon() const = 0;

   /*!
    * Returns true only if this file format implementation can read an entire
    * image in a single operation.
    */
   virtual bool CanRead() const = 0;

   /*!
    * Returns true only if this file format implementation can write an entire
    * image in a single operation.
    */
   virtual bool CanWrite() const = 0;

   /*!
    * Returns true only if this file format implementation supports
    * <em>incremental read</em> operations on image files.
    *
    * Incremental read operations allow the PixInsight core application and
    * other modules to load images by successive row strips.
    */
   virtual bool CanReadIncrementally() const = 0;

   /*!
    * Returns true only if this file format implementation supports
    * <em>incremental write</em> operations on image files.
    *
    * Incremental write operations allow the PixInsight core application and
    * other modules to write images by successive row strips.
    */
   virtual bool CanWriteIncrementally() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write 8-bit
    * unsigned integer images
    */
   virtual bool CanStore8Bit() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * 16-bit unsigned integer images
    */
   virtual bool CanStore16Bit() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * 32-bit unsigned integer images
    */
   virtual bool CanStore32Bit() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * 64-bit unsigned integer images
    */
   virtual bool CanStore64Bit() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * 32-bit floating point real images (IEEE 754 32-bit <em>single
    * precision</em> format for pixel sample values).
    */
   virtual bool CanStoreFloat() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * 64-bit floating point real images (IEEE 754 64-bit <em>double
    * precision</em> format for pixel sample values).
    */
   virtual bool CanStoreDouble() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * 32-bit complex floating point images (IEEE 754 32-bit <em>single
    * precision</em> format for components of complex pixel sample values).
    */
   virtual bool CanStoreComplex() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * 64-bit complex floating point images (IEEE 754 64-bit <em>double
    * precision</em> format for components of complex pixel sample values).
    */
   virtual bool CanStoreDComplex() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write
    * grayscale pixel data.
    */
   virtual bool CanStoreGrayscale() const = 0;

   /*!
    * Returns true only if this file format implementation can read/write RGB
    * color pixel data.
    */
   virtual bool CanStoreRGBColor() const = 0;

   /*!
    * Returns true only if this file format implementation supports alpha
    * channels.
    */
   virtual bool CanStoreAlphaChannels() const = 0;

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * image resolution data.
    */
   virtual bool CanStoreResolution() const = 0;

   /*!
    * Returns true only if this file format implementation can embed/extract
    * FITS header keyword collections.
    */
   virtual bool CanStoreKeywords() const = 0;

   /*!
    * Returns true only if this file format implementation can embed/extract
    * ICC color profiles.
    */
   virtual bool CanStoreICCProfiles() const = 0;

   /*!
    * Returns true only if this file format implementation can embed/extract
    * thumbnail images.
    */
   virtual bool CanStoreThumbnails() const = 0;

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * data properties associated with format instances or image files.
    *
    * \note Don't confuse this member function with CanStoreImageProperties().
    * This function returns true if the implementation can store properties
    * associated with an entire file or format instance, while
    * CanStoreImageProperties() returns true if the implementation can store
    * properties associated with individual images.
    *
    * \sa CanStoreImageProperties(), SupportsViewProperties()
    */
   virtual bool CanStoreProperties() const = 0;

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * data properties associated with individual images.
    *
    * \sa CanStoreProperties(), SupportsViewProperties()
    */
   virtual bool CanStoreImageProperties() const = 0;

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * RGB working space data.
    */
   virtual bool CanStoreRGBWS() const = 0;

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * display function (aka screen transfer function, or STF) parameters.
    */
   virtual bool CanStoreDisplayFunctions() const = 0;

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * color filter array (CFA) descriptions.
    */
   virtual bool CanStoreColorFilterArrays() const = 0;

   /*!
    * Returns true only if this file format implementation supports compression
    * of pixel data.
    *
    * This refers to compression of \e source pixels, not to native compression
    * schemes used by some file formats.
    *
    * For example, the compression schemes employed in the JPEG and JPEG2000
    * formats must \e not cause this member function to return true. The
    * optional ZIP and LZW compressions used in TIFF are the exact kind of
    * compressions that must cause this member function to return true.
    */
   virtual bool SupportsCompression() const = 0;

   /*!
    * Returns true only if this file format implementation supports multiple
    * images stored in a single file.
    *
    * For example, multiple images (e.g., taken with different filters) can be
    * stored in FITS files by means of FITS image extensions, forming a
    * <em>data cube</em> of several images with the same dimensions, or even a
    * collection of independent images.
    */
   virtual bool SupportsMultipleImages() const = 0;

   /*!
    * Returns true only if this file format implementation supports data
    * properties of different data types such as Float64, UI32Vector, String,
    * Complex32, etc.
    *
    * If this member function returns true, a reimplementation of
    * CanStoreProperties() and/or CanStoreImageProperties() (depending on
    * format capabilities) must also return true, and the format must implement
    * all property data types supported by View objects. For information on
    * supported view property types, see the VTYPE_XXX predefined constants in
    * PCL API headers.
    *
    * This function should return false if this format only supports storage of
    * BLOB properties, represented as ByteArray objects, or a limited subset of
    * view property types.
    *
    * \sa CanStoreProperties(), CanStoreImageProperties(),
    * View::PropertyValue(), View::SetPropertyValue()
    */
   virtual bool SupportsViewProperties() const = 0;

   /*!
    * Returns true only if this file format implementation allows the user to
    * edit specific format preferences.
    *
    * If this function returns true, then the EditPreferences() procedure must
    * be reimplemented in a derived class of MetaFileFormat by the module that
    * implements this format.
    */
   virtual bool CanEditPreferences() const = 0;

   /*!
    * Returns true only if this file format implementation uses
    * <em>format-specific data</em>.
    *
    * Format-specific data are preserved on a per-instance (say per-file) basis
    * by the PixInsight application, who actually knows nothing about them.
    */
   virtual bool UsesFormatSpecificData() const = 0;

   /*!
    * Returns true only if this file format has been deprecated or declared
    * obsolete on the PixInsight platform.
    *
    * When this function returns true, the Status() member function should also
    * return information about the current status of this file format,
    * including an explanation of the reasons for deprecation.
    */
   virtual bool IsDeprecated() const = 0;

   /*!
    * Validates a <em>format-specific data block</em>.
    *
    * File formats that use format-specific data reimplement this function to
    * validate format-specific data structures. If this function returns true,
    * that means that the passed \a data block is a valid format-specific data
    * structure for this file format implementation.
    *
    * This function will be called by the PixInsight core application for
    * validation of the \a data block before calling
    * FileFormatImplementation::SetFormatSpecificData() and
    * DisposeFormatSpecificData().
    */
   virtual bool ValidateFormatSpecificData( const void* data ) const = 0;

   /*!
    * Disposes a <em>format-specific data block</em>.
    *
    * File formats that use format-specific data reimplement this function to
    * destroy and deallocate, as appropriate, their own format-specific data
    * structures.
    *
    * This function will be called by the PixInsight core application with the
    * \a data argument pointing to the beginning of a format-specific data
    * block. This function will only be called after validation of the data
    * block by ValidateFormatSpecificData().
    */
   virtual void DisposeFormatSpecificData( void* data ) const = 0;

   /*!
    * Handles a request to edit format preferences. Returns true if the
    * preferences were successfully edited.
    *
    * When implemented, this procedure should open a dialog box to let the
    * user edit format-specific preferences and operating options. This
    * function should only return true if the user accepts the new settings
    * (e.g. by clicking the dialog's OK button).
    *
    * \note This member function will never be called if the
    * CanEditPreferences() member function is not reimplemented to return true
    * in a derived class of MetaFileFormat by the module that implements this
    * file format.
    */
   virtual bool EditPreferences() const = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_FileFormatBase_h

// ----------------------------------------------------------------------------
// EOF pcl/FileFormatBase.h - Released 2017-06-09T08:12:42Z
