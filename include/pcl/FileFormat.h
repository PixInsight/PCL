//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/FileFormat.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_FileFormat_h
#define __PCL_FileFormat_h

/// \file pcl/FileFormat.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/FileFormatBase.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FileFormatPrivate;

/*!
 * \class FileFormat
 * \brief High-level interface to an installed image file format.
 *
 * %FileFormat instances are high-level, managed objects that represent
 * installed image file formats in the PixInsight platform. A module creates an
 * instance of %FileFormat to gain access to an installed file format through
 * <em>intermodule communication</em>.
 *
 * <b>%FileFormat and %MetaFileFormat</b>
 *
 * %FileFormat provides a description of the functionality and properties of an
 * <em>already installed</em> image file format. Contrarily, by subclassing the
 * MetaFileFormat class a module can define and implement a new image file
 * format that can be installed in the PixInsight platform. %MetaFileFormat is
 * a formal description of a file format, while %FileFormat describes an
 * existing (and installed) file format.
 *
 * As a %MetaFileFormat subclass describes how a format can be instantiated,
 * %FileFormat allows a module to create new instances of a file format that
 * can be used to access actual image files. %FileFormat doesn't provide any
 * file handling functionality; access to image files is provided by the
 * FileFormatInstance class.
 *
 * \sa FileFormatBase, MetaFileFormat, FileFormatInstance
 */
class PCL_CLASS FileFormat : public FileFormatBase
{
public:

   /*!
    * Constructs a %FileFormat object.
    *
    * \param nameExtOrMime  A format name, file suffix, or MIME type. This
    *                   parameter determines how the PixInsight core
    *                   application looks for an installed file format to which
    *                   this %FileFormat instance will be an interface.
    *
    * \param toRead     When a file suffix or MIME type is specified and this
    *                   parameter is true, a %FileFormat instance will be
    *                   created for an installed file format able to read files
    *                   with the specified file suffix or corresponding to the
    *                   specified MIME type.
    *
    * \param toWrite    When a file suffix or MIME type is specified and this
    *                   parameter is true, a %FileFormat instance will be
    *                   created for an installed file format able to write
    *                   files with the specified file suffix or corresponding
    *                   to the specified MIME type.
    *
    * When a format name is used as the argument of this constructor,
    * %FileFormat will provide access to an installed file format with the
    * specified identifier, if there exists one. If the argument is a string
    * starting with a dot character, then it is interpreted as a file suffix.
    * If the argument has a slash character ('/'), it is interpreted as a MIME
    * type specifier. In the latter two cases, the PixInsight core application
    * will search for an installed file format able to handle image files with
    * the specified file suffix or for the specified MIME type.
    *
    * In all cases, if no installed file format fits to the specified
    * argument(s), this constructor throws an Error exception with the
    * corresponding error message. Your code should guarantee that these
    * exceptions will be caught and handled appropriately.
    *
    * When \a nameExtOrMime specifies a format name, the \a toRead and
    * \a toWrite parameters are ignored.
    *
    * Example:
    *
    * \code
    * try
    * {
    *    // Find a format able to read XISF files
    *    FileFormat xisfFormat( ".xisf", true );
    *
    *    // Create a format instance
    *    FileFormatInstance myXISFFile( xisfFormat );
    *
    *    // Use the instance to open an existing file
    *    myXISFFile.Open( "/path/to/test.xisf" );
    *
    *    // Read an image in 32-bit floating point format
    *    Image img;
    *    myXISFFile.ReadImage( img );
    *    // ...
    * }
    * catch ( Exception& x )
    * {
    *    // Handle errors ...
    * }
    * \endcode
    *
    * \note The \a toRead and \a toWrite parameters are false by default, which
    * means that no access restrictions are applied by default when creating
    * %FileFormat instances.
    */
   FileFormat( const String& nameExtOrMime, bool toRead = false, bool toWrite = false );

   /*!
    * Copy constructor. Constructs an \e alias %FileFormat object that refers
    * to the same image file format as the specified object \a fmt.
    */
   FileFormat( const FileFormat& fmt );

   /*!
    * Destroys this %FileFormat object.
    *
    * \note This destructor does not destroy or uninstall the actual image file
    * format it refers to, which is part of the PixInsight core application.
    * Only the managed alias object living in the caller module is destroyed.
    */
   virtual ~FileFormat();

   /*!
    */
   IsoString Name() const override;

   /*!
    */
   StringList FileExtensions() const override;

   /*!
    */
   IsoStringList MimeTypes() const override;

   /*!
    */
   uint32 Version() const override;

   /*!
    */
   String Description() const override;

   /*!
    */
   String Implementation() const override;

   /*!
    */
   String Status() const override;

   /*!
    */
   Bitmap Icon() const override;

   /*!
    */
   Bitmap SmallIcon() const override;

   /*!
    */
   bool CanRead() const override;

   /*!
    */
   bool CanWrite() const override;

   /*!
    */
   bool CanReadIncrementally() const override;

   /*!
    */
   bool CanWriteIncrementally() const override;

   /*!
    */
   bool CanStore8Bit() const override;

   /*!
    */
   bool CanStore16Bit() const override;

   /*!
    */
   bool CanStore32Bit() const override;

   /*!
    */
   bool CanStore64Bit() const override;

   /*!
    */
   bool CanStoreFloat() const override;

   /*!
    */
   bool CanStoreDouble() const override;

   /*!
    */
   bool CanStoreComplex() const override;

   /*!
    */
   bool CanStoreDComplex() const override;

   /*!
    */
   bool CanStoreGrayscale() const override;

   /*!
    */
   bool CanStoreRGBColor() const override;

   /*!
    */
   bool CanStoreAlphaChannels() const override;

   /*!
    */
   bool CanStoreResolution() const override;

   /*!
    */
   bool CanStoreKeywords() const override;

   /*!
    */
   bool CanStoreICCProfiles() const override;

   /*!
    */
   bool CanStoreThumbnails() const override;

   /*!
    */
   bool CanStoreProperties() const override;

   /*!
    */
   bool CanStoreImageProperties() const override;

   /*!
    */
   bool CanStoreRGBWS() const override;

   /*!
    */
   bool CanStoreDisplayFunctions() const override;

   /*!
    */
   bool CanStoreColorFilterArrays() const override;

   /*!
    */
   bool SupportsCompression() const override;

   /*!
    */
   bool SupportsMultipleImages() const override;

   /*!
    */
   bool SupportsViewProperties() const override;

   /*!
    */
   bool CanEditPreferences() const override;

   /*!
    */
   bool UsesFormatSpecificData() const override;

   /*!
    */
   bool IsDeprecated() const override;

   /*!
    */
   bool ValidateFormatSpecificData( const void* data ) const override;

   /*!
    */
   void DisposeFormatSpecificData( void* data ) const override;

   /*!
    */
   bool EditPreferences() const override;

   /*!
    * Returns a list with all installed file formats in the PixInsight core
    * application.
    */
   static Array<FileFormat> AllFormats();

   /*!
    * Returns true iff the specified file suffix corresponds to an installed
    * file format.
    *
    * \param path       A file path including a file suffix identifying an
    *                   image format, such as ".xisf", ".jpg", etc. This
    *                   parameter is case-insensitive. Existence of an actual
    *                   file at the specified path is not verified.
    *
    * \param toRead     True to identify an installed file format module able
    *                   to read files.
    *
    * \param toWrite    True to identify an installed file format module able
    *                   to write files.
    *
    * \note The \a toRead and \a toWrite parameters are false by default, which
    * means that no access restrictions are applied by default.
    */
   static bool IsSupportedFileFormatBySuffix( const String& path, bool toRead = false, bool toWrite = false );

   /*!
    * Returns a list with the full file paths of all supported image files in
    * a given directory of the local filesystem.
    *
    * \param dirPath    Path to an existing directory in the local filesystem,
    *                   where supported image files will be looked for.
    *
    * \param toRead     True to select files supported for read operations.
    *
    * \param toWrite    True to select files supported for write operations.
    *
    * \param recursive  True to search for files recursively throughout the
    *                   entire subtree rooted at \a dirPath. False to restrict
    *                   the file search operation to existing files on
    *                   \a dirPath. This parameter is false by default.
    *
    * \param followLinks   True to follow symbolic links to directories and
    *                   files, on platforms supporting symbolic links. This is
    *                   true by default. This parameter is ignored on Windows.
    *
    * \note The \a toRead and \a toWrite parameters are false by default, which
    * means that no access restrictions are applied by default.
    */
   static StringList SupportedImageFiles( const String& dirPath, bool toRead = false, bool toWrite = false,
                                          bool recursive = false, bool followLinks = true );

   /*!
    * Returns a list with the full file paths of all local normalization data
    * files in a given directory of the local filesystem.
    *
    * \param dirPath    Path to an existing directory in the local filesystem,
    *                   where local normalization data files will be looked
    *                   for.
    *
    * \param recursive  True to search for files recursively throughout the
    *                   entire subtree rooted at \a dirPath. False to restrict
    *                   the file search operation to existing files on
    *                   \a dirPath. This parameter is false by default.
    *
    * \param followLinks   True to follow symbolic links to directories and
    *                   files, on platforms supporting symbolic links. This is
    *                   true by default. This parameter is ignored on Windows.
    *
    * Currently this function looks for files with the .xnml suffix (XML local
    * normalization data format, XNML).
    *
    * \sa LocalNormalizationData
    */
   static StringList LocalNormalizationFiles( const String& dirPath, bool recursive = false, bool followLinks = true );

   /*!
    * Returns a list with the full file paths of all drizzle data files in a
    * given directory of the local filesystem.
    *
    * \param dirPath    Path to an existing directory in the local filesystem,
    *                   where drizzle data files will be looked for.
    *
    * \param recursive  True to search for files recursively throughout the
    *                   entire subtree rooted at \a dirPath. False to restrict
    *                   the file search operation to existing files on
    *                   \a dirPath. This parameter is false by default.
    *
    * \param followLinks   True to follow symbolic links to directories and
    *                   files, on platforms supporting symbolic links. This is
    *                   true by default. This parameter is ignored on Windows.
    *
    * Currently this function looks for files with the .drz (compatibility text
    * drizzle format) and .xdrz (XML drizzle data format, XDRZ) suffixes.
    *
    * \sa DrizzleData
    */
   static StringList DrizzleFiles( const String& dirPath, bool recursive = false, bool followLinks = true );

   /*!
    * Returns a list with the full file paths of all ephemeris data files in a
    * given directory of the local filesystem.
    *
    * \param dirPath    Path to an existing directory in the local filesystem,
    *                   where ephemeris data files will be looked for.
    *
    * \param recursive  True to search for files recursively throughout the
    *                   entire subtree rooted at \a dirPath. False to restrict
    *                   the file search operation to existing files on
    *                   \a dirPath. This parameter is false by default.
    *
    * \param followLinks   True to follow symbolic links to directories and
    *                   files, on platforms supporting symbolic links. This is
    *                   true by default. This parameter is ignored on Windows.
    *
    * Currently this function looks for files with the .xeph suffix (Extensible
    * Ephemeris Data format, XEPH).
    *
    * \sa EphemerisFile
    */
   static StringList EphemerisFiles( const String& dirPath, bool recursive = false, bool followLinks = true );

private:

   AutoPointer<FileFormatPrivate> m_data;

   FileFormat( const void* );

   const void* Handle() const;

   friend class FileFormatPrivate;
   friend class FileFormatInstance;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_FileFormat_h

// ----------------------------------------------------------------------------
// EOF pcl/FileFormat.h - Released 2018-12-12T09:24:21Z
