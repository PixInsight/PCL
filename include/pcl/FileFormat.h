//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/FileFormat.h - Released 2016/02/21 20:22:12 UTC
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

/// \file pcl/FileFormat.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_AutoPointer_h
#include <pcl/AutoPointer.h>
#endif

#ifndef __PCL_FileFormatBase_h
#include <pcl/FileFormatBase.h>
#endif

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
    * \param nameExtOrMime  A format name, file extension, or MIME type. This
    *                   parameter determines how the PixInsight core
    *                   application looks for an installed file format to which
    *                   this %FileFormat instance will be an interface.
    *
    * \param toRead     When a file extension or MIME type is specified and
    *                   this parameter is true, a %FileFormat instance will be
    *                   created for an installed file format able to read files
    *                   with the specified file extension or corresponding to
    *                   the specified MIME type.
    *
    * \param toWrite    When a file extension or MIME type is specified and
    *                   this parameter is true, a %FileFormat instance will be
    *                   created for an installed file format able to write
    *                   files with the specified file extension or
    *                   corresponding to the specified MIME type.
    *
    * When a format name is used as the argument of this constructor,
    * %FileFormat will provide access to an installed file format with the
    * specified identifier, if there exists one. If the argument is a string
    * starting with a dot character, then it is interpreted as a file
    * extension. If the argument has a slash character ('/'), it is interpreted
    * as a MIME type specifier. In the latter two cases, the PixInsight core
    * application will search for an installed file format able to handle image
    * files with the specified file extension or for the specified MIME type.
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
    *    // Find a format able to read FITS files
    *    FileFormat fitsFormat( ".fits", true );
    *
    *    // Create a format instance
    *    FileFormatInstance myFITSFile( fitsFormat );
    *
    *    // Use the instance to open an existing file
    *    myFITSFile.Open( "/home/images/test.fit" );
    *
    *    // Read an image in 32-bit floating point format
    *    Image img;
    *    myFITSFile.Read( img );
    *    // ...
    * }
    *
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
   virtual IsoString Name() const;

   /*!
    */
   virtual StringList FileExtensions() const;

   /*!
    */
   virtual IsoStringList MimeTypes() const;

   /*!
    */
   virtual uint32 Version() const;

   /*!
    */
   virtual String Description() const;

   /*!
    */
   virtual String Implementation() const;

   /*!
    */
   virtual String Status() const;

   /*!
    */
   virtual Bitmap Icon() const;

   /*!
    */
   virtual Bitmap SmallIcon() const;

   /*!
    */
   virtual bool CanRead() const;

   /*!
    */
   virtual bool CanWrite() const;

   /*!
    */
   virtual bool CanReadIncrementally() const;

   /*!
    */
   virtual bool CanWriteIncrementally() const;

   /*!
    */
   virtual bool CanStore8Bit() const;

   /*!
    */
   virtual bool CanStore16Bit() const;

   /*!
    */
   virtual bool CanStore32Bit() const;

   /*!
    */
   virtual bool CanStore64Bit() const;

   /*!
    */
   virtual bool CanStoreFloat() const;

   /*!
    */
   virtual bool CanStoreDouble() const;

   /*!
    */
   virtual bool CanStoreComplex() const;

   /*!
    */
   virtual bool CanStoreDComplex() const;

   /*!
    */
   virtual bool CanStoreGrayscale() const;

   /*!
    */
   virtual bool CanStoreRGBColor() const;

   /*!
    */
   virtual bool CanStoreAlphaChannels() const;

   /*!
    */
   virtual bool CanStoreResolution() const;

   /*!
    */
   virtual bool CanStoreKeywords() const;

   /*!
    */
   virtual bool CanStoreICCProfiles() const;

   /*!
    */
   virtual bool CanStoreThumbnails() const;

   /*!
    */
   virtual bool CanStoreProperties() const;

   /*!
    */
   virtual bool CanStoreRGBWS() const;

   /*!
    */
   virtual bool CanStoreDisplayFunctions() const;

   /*!
    */
   virtual bool CanStoreColorFilterArrays() const;

   /*!
    */
   virtual bool SupportsCompression() const;

   /*!
    */
   virtual bool SupportsMultipleImages() const;

   /*!
    */
   virtual bool SupportsViewProperties() const;

   /*!
    */
   virtual bool CanEditPreferences() const;

   /*!
    */
   virtual bool UsesFormatSpecificData() const;

   /*!
    */
   virtual bool IsDeprecated() const;

   /*!
    */
   virtual bool ValidateFormatSpecificData( const void* data ) const;

   /*!
    */
   virtual void DisposeFormatSpecificData( void* data ) const;

   /*!
    */
   virtual bool EditPreferences() const;

   /*!
    * Returns a list with all installed file formats in the PixInsight core
    * application.
    */
   static Array<FileFormat> AllFormats();

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
// EOF pcl/FileFormat.h - Released 2016/02/21 20:22:12 UTC
