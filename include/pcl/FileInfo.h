//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/FileInfo.h - Released 2017-05-28T08:28:50Z
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

#ifndef __PCL_FileInfo_h
#define __PCL_FileInfo_h

/// \file pcl/FileInfo.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/File.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class FileInfo
 * \brief Platform-independent information on file and directory items.
 *
 * ### TODO: Write a detailed description for %FileInfo.
 */
class PCL_CLASS FileInfo
{
public:

   /*!
    * Default constructor. Constructs an empty %FileInfo object without any
    * information on existing file or directory items.
    */
   FileInfo();

   /*!
    * Constructs a %FileInfo object with information retrieved for the
    * specified file or directory \a path.
    */
   FileInfo( const String& path );

   /*!
    * Copy constructor.
    */
   FileInfo( const FileInfo& x )
   {
      (void)operator =( x );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~FileInfo()
   {
      Clear();
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   FileInfo& operator =( const FileInfo& x );

   /*!
    * Returns the absolute path of this file or directory.
    */
   const String& Path() const
   {
      return m_path;
   }

   /*!
    * Returns the Windows drive component of the path in this object, or an
    * empty string if the path does not include a Windows drive specification.
    *
    * This function only makes sense on Windows. On UNIX and Linux operating
    * systems, this function always returns an empty string.
    */
   String Drive() const
   {
      return File::ExtractDrive( m_path );
   }

   /*!
    * Returns the parent directory component of the path in this object, or an
    * empty string if the path does not include a parent directory.
    *
    * On Windows, the drive component of the path is \e not included.
    *
    * Examples:
    *
    * In '/foo.tar.gz' the directory is '/'.
    * In '/foo/bar.tar.gz' the directory is '/foo'.
    * In 'foo/bar.tar.gz' the directory is 'foo'.
    * In 'C:/Foo/Bar.txt' the directory is '/Foo'.
    */
   String Directory() const
   {
      return File::ExtractDirectory( m_path );
   }

   /*!
    * Returns the file name component of the path in this object, or an empty
    * string if the path does not include a name.
    *
    * The file extension or suffix is \e not included.
    *
    * Examples:
    *
    * In '/foo/bar.tar.gz' the name is 'bar'.
    * In 'C:/Foo/Bar.txt' the name is 'Bar'.
    */
   String Name() const
   {
      return File::ExtractName( m_path );
   }

   /*!
    * Returns the file extension component of the path in this object, or an
    * empty string if the path does not include an extension.
    *
    * The returned extension \e includes the initial dot separator.
    *
    * Examples:
    *
    * In '/foo/bar.tar.gz' the extension is '.gz'.
    * In 'C:/Foo/Bar.txt' the extension is '.txt'.
    */
   String Extension() const
   {
      return File::ExtractExtension( m_path );
   }

   /*!
    * A synonym for Extension().
    */
   String Suffix() const
   {
      return Extension();
   }

   /*!
    * Returns the complete suffix of the path in this object, or an empty
    * string if the path does not include a suffix.
    *
    * The returned string \e includes the initial dot separator.
    *
    * The complete suffix is the rightmost substring of the path, starting with
    * the first occurrence of a dot character. For example, in 'foo.tar.gz' the
    * complete suffix is '.tar.gz'.
    */
   String CompleteSuffix() const
   {
      return File::ExtractCompleteSuffix( m_path );
   }

   /*!
    * Returns the name and extension of the path in this object. Calling this
    * member function is functionally equivalent to: Name() + Extension().
    */
   String NameAndExtension() const
   {
      return File::ExtractNameAndExtension( m_path );
   }

   /*!
    * A synonym for NameAndExtension().
    */
   String NameAndSuffix() const
   {
      return NameAndExtension();
   }

   /*!
    * Returns true iff this object represents an existing file or directory.
    */
   bool Exists() const
   {
      return m_exists;
   }

   /*!
    * Returns a constant reference to the FileAttributes flags for this file or
    * directory.
    */
   const FileAttributes& Attributes() const
   {
      return m_attributes;
   }

   /*!
    * Returns true iff this object represents an existing directory.
    */
   bool IsDirectory() const
   {
      return m_attributes.IsFlagSet( FileAttribute::Directory );
   }

   /*!
    * Returns true iff this object represents an existing file.
    */
   bool IsFile() const
   {
      return m_attributes.IsFlagSet( FileAttribute::Regular );
   }

   /*!
    * Returns true iff this object represents a symbolic link.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns false.
    */
   bool IsSymbolicLink() const
   {
      return m_attributes.IsFlagSet( FileAttribute::SymbolicLink );
   }

   /*!
    * Returns the target path of this symbolic link, or an empty string if this
    * object does not represent a symbolic link.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns an empty string.
    */
   String SymbolicLinkTarget() const;

   /*!
    * Returns true iff this object represents a hidden file or directory.
    *
    * On Windows, this member function returns true if the file or directory
    * represented by this object has the hidden attribute set. On UNIX and
    * Linux operating systems, this function returns true when the file or
    * directory name begins with a dot character.
    */
   bool IsHidden() const
   {
      return m_attributes.IsFlagSet( FileAttribute::Hidden );
   }

   /*!
    * Returns true iff the user id and group id of the caller process have read
    * access permission for the item represented by this object.
    */
   bool IsReadable() const
   {
      return m_readable;
   }

   /*!
    * Returns true iff the user id and group id of the caller process have write
    * access permission for the item represented by this object.
    */
   bool IsWritable() const
   {
      return m_writable;
   }

   /*!
    * Returns true iff the user id and group id of the caller process have
    * execution access permission for the item represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns false.
    *
    * On POSIX systems, this function returns true only if the item represented
    * has the S_IXUSR file mode bit set. This means that this function will
    * normally return true for directories, since S_IXUSR/S_IXGRP/S_IXOTH
    * represent standard search permissions for a directory.
    */
   bool IsExecutable() const
   {
      return m_executable;
   }

   /*!
    * Returns the size in bytes of the file or directory item represented by
    * this object.
    *
    * If this object represents a directory, a fixed size (usually 4096 bytes)
    * is always returned on UNIX and Linux systems. On Windows, zero is always
    * returned for directories.
    */
   fsize_type Size() const
   {
      return m_size;
   }

   /*!
    * Returns the number of existing hard links to the file or directory item
    * represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns zero.
    */
   int NumberOfHardLinks() const
   {
      return m_numberOfHardLinks;
   }

   /*!
    * Returns the user id of the owner of the file or directory item
    * represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns zero.
    */
   int UserId() const
   {
      return m_userId;
   }

   /*!
    * Returns the group id of the owner of the file or directory item
    * represented by this object.
    *
    * This member function only makes sense on UNIX/Linux platforms. On Windows
    * it always returns zero.
    */
   int GroupId() const
   {
      return m_groupId;
   }

   /*!
    * Returns a constant reference to a FileTime structure corresponding to the
    * creation time of this file or directory.
    */
   const FileTime& TimeCreated() const
   {
      return m_created;
   }

   /*!
    * Returns a constant reference to a FileTime structure corresponding to the
    * last access time of this file or directory.
    */
   const FileTime& LastAccessed() const
   {
      return m_lastAccessed;
   }

   /*!
    * Returns a constant reference to a FileTime structure corresponding to the
    * last modification time of this file or directory.
    */
   const FileTime& LastModified() const
   {
      return m_lastModified;
   }

   /*!
    * Retrieves up-to-date information for the item represented by this object.
    */
   void Refresh();

   /*!
    * Retrieves information for the specified \a path.
    */
   void Refresh( const String& path );

   /*!
    * Clears all the information in this object, leaving it in an invalid state
    * corresponding to a nonexistent item.
    */
   void Clear();

private:

   String         m_path;              // full file or directory path
   FileAttributes m_attributes;        // item attributes
   fsize_type     m_size;              // file size in bytes
   int            m_numberOfHardLinks; // number of existing hard links to this file
   int            m_userId;            // user id of the file owner
   int            m_groupId;           // group id of the file owner
   FileTime       m_created;           // creation time
   FileTime       m_lastAccessed;      // time of last access
   FileTime       m_lastModified;      // time of last change
   bool           m_exists        : 1; // item exists
   bool           m_readable      : 1; // caller has read permission
   bool           m_writable      : 1; // caller has write permission
   bool           m_executable    : 1; // caller has execute permission

   // Clears everything but *preserves* m_path
   void ClearData();
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_FileInfo_h

// ----------------------------------------------------------------------------
// EOF pcl/FileInfo.h - Released 2017-05-28T08:28:50Z
