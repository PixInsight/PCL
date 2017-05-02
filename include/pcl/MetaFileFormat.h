//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/MetaFileFormat.h - Released 2017-05-02T10:38:59Z
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

#ifndef __PCL_MetaFileFormat_h
#define __PCL_MetaFileFormat_h

/// \file pcl/MetaFileFormat.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/FileFormatBase.h>
#include <pcl/MetaObject.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileFormatImplementation;

// ----------------------------------------------------------------------------

/*!
 * \class MetaFileFormat
 * \brief A formal description of an image file format in PixInsight.
 *
 * %MetaFileFormat provides a formal description of the basic functionality and
 * properties of an <em>image file format</em> in a PixInsight module.
 *
 * A module implements an \e installable file format as a derived class of
 * %MetaFileFormat. Such subclass describes general properties of the file
 * format, and also provides the basic mechanisms to instantiate a format and
 * to generate duplicates of existing format instances.
 *
 * %MetaFileFormat and its subclasses don't implement any file handling
 * capabilities. Actual file access work must be implemented through a
 * <em>format instance implementation</em> class, which is always a subclass of
 * the FileFormatImplementation class in the PixInsight/PCL framework.
 *
 * In PixInsight's terminology, a <em>format instance</em> can be thought of as
 * an image stored as a disk file of the format in question, although the
 * concept of format instances allows for more flexible, generalized
 * interpretations.
 *
 * Note that the %MetaFileFormat and FileFormatImplementation classes are
 * conceptually parallel to MetaProcess and ProcessImplementation,
 * respectively. In the same way, a process is to a format like a process
 * instance is to a format instance in PixInsight. This is just a reflection of
 * the strong object orientation that characterizes the PixInsight platform.
 *
 * Each file format must have a unique name, or \e identifier. Identifiers are
 * crucial for many key features of the PixInsight environment, as scripts, the
 * command-line interface, image icons and image containers, etc.
 *
 * \sa MetaObject, FileFormatBase, FileFormat, FileFormatImplementation,
 * FileFormatInstance
 */
class PCL_CLASS MetaFileFormat : public MetaObject, public FileFormatBase
{
public:

   /*!
    * Constructs a %MetaFileFormat object.
    */
   MetaFileFormat();

   /*!
    * Destroys this %MetaFileFormat object.
    */
   virtual ~MetaFileFormat() noexcept( false )
   {
   }

   /*!
    * Copy constructor. This constructor is disabled because %MetaFileFormat
    * represents unique server-side objects.
    */
   MetaFileFormat( const MetaFileFormat& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %MetaFileFormat
    * represents unique server-side objects.
    */
   MetaFileFormat& operator =( const MetaFileFormat& ) = delete;

   /*!
    */
   virtual IsoString Name() const = 0;

   /*!
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
    *
    * \note    The default implementation of this function returns an empty
    * string list, so by default a format does not provide a set of supported
    * MIME types. This is legal, but considered bad practice when the format
    * implements a standard image format recognized by IANA.
    */
   virtual IsoStringList MimeTypes() const
   {
      return IsoStringList();
   }

   /*!
    */
   virtual uint32 Version() const;

   /*!
    * Returns a brief description text for this file format.
    *
    * This function must provide a simple, typically single-line, description
    * of this image file format for quick reference. Example: "Flexible Image
    * Transport System". The Implementation() member function has been designed
    * to provide a more complete description of a format's functionality and
    * capabilities.
    *
    * \note    The default implementation of this function returns an empty
    * string, so by default a format does not provide a description. This is
    * legal, but it is considered bad practice, so this function \e should be
    * reimplemented.
    */
   virtual String Description() const
   {
      return String();
   }

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
    *
    * \note    The default implementation of this function returns an empty
    * string, so by default a format does not provide an implementation
    * description. This is legal, but it is considered bad practice, so this
    * function \e should be reimplemented.
    */
   virtual String Implementation() const
   {
      return String();
   }

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
    *
    * \note    The default implementation of this function returns an empty
    * string, so by default a format does not provide any status information.
    */
   virtual String Status() const
   {
      return String();
   }

   /*!
    * Returns a <em>large icon</em> for this format as an image in the
    * standard XPM format.
    *
    * The specified image will be used to identify all instances of this
    * format (e.g., images and files) in the core application's GUI. It will be
    * used on the Format Explorer window, on image icons of this format, and in
    * general for every graphical item related to this format or to an instance
    * of this format.
    *
    * 32-bit RGBA color images (including an alpha channel) are fully
    * supported.
    *
    * If this function returns zero, a default icon will be assigned to this
    * file format automatically.
    *
    * \note    The default implementation of this function returns zero.
    *
    * \sa IconImageFile()
    */
   virtual const char** IconImageXPM() const
   {
      return 0;
   }

   /*!
    * Returns a <em>large icon</em> for this format as a path specification to
    * an existing image file.
    *
    * Supported image file formats include PNG, XPM, JPG and BMP.
    *
    * For details on format icon images, see the documentation for
    * IconImageXPM().
    *
    * \note    Using this function is discouraged, since it produces an
    * unnecessary dependency on an external file, which complicates the module
    * installation procedure. The recommended method is always reimplementing
    * the IconImageXPM() member function in a derived class, to provide the
    * address of a static image in the XPM format.
    *
    * \note    The default implementation of this function returns an empty
    *          string.
    *
    * \sa IconImageXPM()
    */
   virtual String IconImageFile() const
   {
      return String();
   }

   /*!
    * Returns a <em>small icon</em> for this format as an image in the
    * standard XPM format.
    *
    * For details on format icon images, see the documentation for
    * IconImageXPM().
    *
    * Small icons are used on interface elements where screen space must be
    * preserved. Two good examples are the Format Explorer window and the
    * ImageContainer interface.
    *
    * When this function is not reimplemented in a derived class, the core
    * PixInsight application automatically generates a small icon by resampling
    * down the large icon provided by IconImageXPM().
    *
    * You normally should not need to reimplement this function; the core
    * application does a fine work resampling large icons to obtain reduced
    * versions.
    *
    * \sa SmallIconImageFile()
    */
   virtual const char** SmallIconImageXPM() const
   {
      return 0;
   }

   /*!
    * Returns a <em>small icon</em> for this format as a path specification to
    * an existing image file.
    *
    * Supported image file formats include PNG, XPM, JPG and BMP.
    *
    * For details on small format icon images, see the documentation for
    * SmallIconImageXPM().
    *
    * \note    Using this function is discouraged, for the same reasons
    * explained on the documentation entry for IconImageFile().
    *
    * \sa SmallIconImageXPM()
    */
   virtual String SmallIconImageFile() const
   {
      return String();
   }

   /*!
    */
   virtual Bitmap Icon() const;

   /*!
    */
   virtual Bitmap SmallIcon() const;

   /*!
    * Returns true only if this file format implementation can read an entire
    * image in a single operation.
    *
    * The default implementation returns true.
    */
   virtual bool CanRead() const
   {
      return true;
   }

   /*!
    * Returns true only if this file format implementation can write an entire
    * image in a single operation.
    *
    * The default implementation returns true.
    */
   virtual bool CanWrite() const
   {
      return true;
   }

   /*!
    * Returns true only if this file format implementation supports
    * <em>incremental read</em> operations on image files.
    *
    * Incremental read operations allow the PixInsight core application and
    * other modules to load images by successive row strips.
    *
    * The default implementation returns false.
    */
   virtual bool CanReadIncrementally() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation supports
    * <em>incremental write</em> operations on image files.
    *
    * Incremental write operations allow the PixInsight core application and
    * other modules to write images by successive row strips.
    *
    * The default implementation returns false.
    */
   virtual bool CanWriteIncrementally() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can read/write 8-bit
    * unsigned integer images
    *
    * The default implementation returns true.
    */
   virtual bool CanStore8Bit() const
   {
      return true;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * 16-bit unsigned integer images
    *
    * The default implementation returns true.
    */
   virtual bool CanStore16Bit() const
   {
      return true;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * 32-bit unsigned integer images
    *
    * The default implementation returns false.
    */
   virtual bool CanStore32Bit() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * 64-bit unsigned integer images
    *
    * The default implementation returns false.
    */
   virtual bool CanStore64Bit() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * 32-bit floating point real images (IEEE 754 32-bit <em>single
    * precision</em> format for pixel sample values).
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreFloat() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * 64-bit floating point real images (IEEE 754 64-bit <em>double
    * precision</em> format for pixel sample values).
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreDouble() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * 32-bit floating point complex images (IEEE 754 32-bit <em>single
    * precision</em> format for components of complex pixel sample values).
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreComplex() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * 64-bit floating point complex images (IEEE 754 64-bit <em>double
    * precision</em> format for components of complex pixel sample values).
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreDComplex() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can read/write
    * grayscale pixel data.
    *
    * The default implementation returns true.
    */
   virtual bool CanStoreGrayscale() const
   {
      return true;
   }

   /*!
    * Returns true only if this file format implementation can read/write RGB
    * color pixel data.
    *
    * The default implementation returns true.
    */
   virtual bool CanStoreRGBColor() const
   {
      return true;
   }

   /*!
    * Returns true only if this file format implementation supports alpha
    * image channels.
    *
    * The default implementation returns true.
    */
   virtual bool CanStoreAlphaChannels() const
   {
      return true;
   }

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * image resolution data.
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreResolution() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can embed/extract
    * FITS header keyword collections.
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreKeywords() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can embed/extract
    * ICC color profiles.
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreICCProfiles() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can embed/extract
    * thumbnail images.
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreThumbnails() const
   {
      return false;
   }

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
    * The default implementation returns false.
    *
    * \sa CanStoreImageProperties(), SupportsViewProperties()
    */
   virtual bool CanStoreProperties() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * properties associated with individual images.
    *
    * The default implementation returns false.
    *
    * \sa CanStoreProperties(), SupportsViewProperties()
    */
   virtual bool CanStoreImageProperties() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * RGB working space data.
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreRGBWS() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * DisplayFunction objects.
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreDisplayFunctions() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation can store/retrieve
    * color filter array (CFA) descriptions.
    *
    * The default implementation returns false.
    */
   virtual bool CanStoreColorFilterArrays() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation supports compression
    * of pixel data.
    *
    * This refers to compression of \e source pixels, not to native compression
    * schemes used by some file formats.
    *
    * For examle, the compression schemes employed in the JPEG and JPEG2000
    * formats must \e not cause this member function to return true. The
    * optional ZIP and LZW compressions used in TIFF are the exact kind of
    * compressions that must cause this member function to return true.
    *
    * The default implementation returns false.
    */
   virtual bool SupportsCompression() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation supports multiple
    * images stored in a single file.
    *
    * For example, multiple images (e.g., taken with different filters) can be
    * stored in FITS files by means of FITS image extensions, forming a
    * <em>data cube</em> of several images with the same dimensions, or even a
    * collection of independent images.
    *
    * The default implementation returns false.
    */
   virtual bool SupportsMultipleImages() const
   {
      return false;
   }

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
    * The default implementation returns false.
    *
    * \sa CanStoreProperties(), CanStoreImageProperties(),
    * View::PropertyValue(), View::SetPropertyValue()
    */
   virtual bool SupportsViewProperties() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation allows the user to
    * edit specific format preferences.
    *
    * If this function returns true, then the EditPreferences() procedure must
    * be reimplemented in a derived class.
    *
    * The default implementation returns false.
    */
   virtual bool CanEditPreferences() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format implementation uses
    * <em>format-specific data</em>.
    *
    * Format-specific data are preserved on a per-instance (say per-file) basis
    * by the PixInsight application, who actually knows nothing about them.
    *
    * The default implementation returns false.
    *
    * \sa ValidateFormatSpecificData(), DisposeFormatSpecificData()
    */
   virtual bool UsesFormatSpecificData() const
   {
      return false;
   }

   /*!
    * Returns true only if this file format has been deprecated or declared
    * obsolete on the PixInsight platform.
    *
    * When this function returns true, the Status() member function should also
    * return information about the current status of this file format,
    * including an explanation of the reasons for deprecation.
    *
    * The default implementation returns false.
    */
   virtual bool IsDeprecated() const
   {
      return false;
   }

   /*!
    */
   virtual bool ValidateFormatSpecificData( const void* data ) const;

   /*!
    */
   virtual void DisposeFormatSpecificData( void* data ) const;

   /*!
    * Creates a new instance of this file format.
    *
    * Typically, we have a %MetaFileFormat descendant class describing an image
    * file format in generic terms, and a FileFormatImplementation descendant
    * class implementing format instantiation (i.e. doing actual file reading,
    * writing, embedding, etc.).
    *
    * \note This is a pure virtual function that must be reimplemented in all
    * derived classes.
    */
   virtual FileFormatImplementation* Create() const = 0;

   /*!
    * Handles a request to edit format preferences. Returns true iff the
    * preferences were successfully edited.
    *
    * When implemented, this procedure should open a dialog box to let the
    * user edit format-specific preferences and operating options. This
    * function should only return true if the user accepts the new settings
    * (e.g. by clicking the dialog's OK button).
    *
    * \note This member function will never be called if the
    * CanEditPreferences() member function is not reimplemented to return true.
    */
   virtual bool EditPreferences() const;

private:

   virtual void PerformAPIDefinitions() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_MetaFileFormat_h

// ----------------------------------------------------------------------------
// EOF pcl/MetaFileFormat.h - Released 2017-05-02T10:38:59Z
