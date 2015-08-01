//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/File.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_File_h
#define __PCL_File_h

/// \file pcl/File.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Flags_h
#include <pcl/Flags.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_StringList_h
#include <pcl/StringList.h>
#endif

#ifndef __PCL_ByteArray_h
#include <pcl/ByteArray.h>
#endif

#ifndef __PCL_Exception_h
#include <pcl/Exception.h>
#endif

#ifndef __PCL_FILE_DONT_REMOVE_PREVIOUS_DECLARATION

// Removing conflicting identifiers from Win32 SDK headers.

#ifdef CreateFile
#undef CreateFile
#endif

#ifdef CopyFile
#undef CopyFile
#endif

#ifdef MoveFile
#undef MoveFile
#endif

#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef RemoveDirectory
#undef RemoveDirectory
#endif

#endif // !__PCL_FILE_DONT_REMOVE_PREVIOUS_DECLARATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace FileMode
 * \brief File access, share and opening/creation modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FileMode::Read</td>         <td>Read access enabled</td></tr>
 * <tr><td>FileMode::Write</td>        <td>Write access enabled</td></tr>
 * <tr><td>FileMode::AccessMode</td>   <td>Mask to isolate read/write flags</td></tr>
 * <tr><td>FileMode::ShareRead</td>    <td>Sharing for reading enabled</td></tr>
 * <tr><td>FileMode::ShareWrite</td>   <td>Sharing for writing enabled</td></tr>
 * <tr><td>FileMode::ShareMode</td>    <td>Mask to isolate share read/write flags</td></tr>
 * <tr><td>FileMode::Open</td>         <td>Open an existing file</td></tr>
 * <tr><td>FileMode::Create</td>       <td>Create a new file or truncate an existing file</td></tr>
 * <tr><td>FileMode::OpenMode</td>     <td>Mask to isolate open/creation flags</td></tr>
 * </table>
 */
namespace FileMode
{
   enum mask_type
   {
      Zero        = 0x00000000,

      /*
       * Access mode
       */
      Read        = 0x00000001,
      Write       = 0x00000002,
      AccessMode  = 0x0000000F,

      /*
       * Share mode
       */
      ShareRead   = 0x00000010,
      ShareWrite  = 0x00000020,
      ShareMode   = 0x000000F0,

      /*
       * Open/create mode
       */
      Open        = 0x00000100,
      Create      = 0x00000200,
      OpenMode    = 0x00000F00
   };
}

/*!
 * A combination of file access, share and opening/creation mode flags.
 */
typedef Flags<FileMode::mask_type>  FileModes;

// ----------------------------------------------------------------------------

/*!
 * \namespace SeekMode
 * \brief File seek modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>SeekMode::FromBegin</td>     <td>Move file pointer relative to the beginning of the file</td></tr>
 * <tr><td>SeekMode::FromCurrent</td>   <td>Move file pointer from the current position</td></tr>
 * <tr><td>SeekMode::FromEnd</td>       <td>Move file pointer relative to the end of the file</td></tr>
 * </table>
 */
namespace SeekMode
{
   enum value_type
   {
      FromBegin,     // Move relative to the beginning of the file
      FromCurrent,   // Move from current position
      FromEnd,       // Move relative to the end of the file
      NumberOfSeekModes
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace FileAttribute
 * \brief File attributes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td colspan="2"><b>File type</b></td></tr>
 * <tr><td>FileAttribute::Block</td>         <td>Block special</td></tr>
 * <tr><td>FileAttribute::Character</td>     <td>Character special</td></tr>
 * <tr><td>FileAttribute::FIFO</td>          <td>FIFO special</td></tr>
 * <tr><td>FileAttribute::Regular</td>       <td>Regular file</td></tr>
 * <tr><td>FileAttribute::Directory</td>     <td>Directory</td></tr>
 * <tr><td>FileAttribute::SymbolicLink</td>  <td>Symbolic link</td></tr>
 * <tr><td>FileAttribute::Socket</td>        <td>Socket</td></tr>
 * <tr><td>FileAttribute::FileType</td>      <td>Mask to isolate file type flags</td></tr>
 * <tr><td colspan="2"><b>File attributes</b>\n
 *                    <em>Windows-exclusive, except ReadOnly and Hidden, which we emulate on POSIX platforms.</em></td></tr>
 * <tr><td>FileAttribute::Archive</td>       <td>File is archived</td></tr>
 * <tr><td>FileAttribute::Compressed</td>    <td>File is compressed</td></tr>
 * <tr><td>FileAttribute::Encrypted</td>     <td>File is encrypted</td></tr>
 * <tr><td>FileAttribute::Hidden</td>        <td>File is hidden</td></tr>
 * <tr><td>FileAttribute::ReadOnly</td>      <td>File is read-only</td></tr>
 * <tr><td>FileAttribute::System</td>        <td>File is a system file</td></tr>
 * <tr><td>FileAttribute::Temporary</td>     <td>File is a temporary file</td></tr>
 * <tr><td>FileAttribute::FileAttributes</td><td>Mask to isolate file attributes flags</td></tr>
 * <tr><td colspan="2"><b>File permissions</b></td></tr>
 * <tr><td>FileAttribute::Read</td>          <td>Owner can read</td></tr>
 * <tr><td>FileAttribute::Write</td>         <td>Owner can write</td></tr>
 * <tr><td>FileAttribute::Execute</td>       <td>Owner can execute/search</td></tr>
 * <tr><td>FileAttribute::ReadGroup</td>     <td>Group can read</td></tr>
 * <tr><td>FileAttribute::WriteGroup</td>    <td>Group can write</td></tr>
 * <tr><td>FileAttribute::ExecuteGroup</td>  <td>Group can execute/search</td></tr>
 * <tr><td>FileAttribute::ReadOthers</td>    <td>Others can read</td></tr>
 * <tr><td>FileAttribute::WriteOthers</td>   <td>Others can write</td></tr>
 * <tr><td>FileAttribute::ExecuteOthers</td> <td>Others can execute/search</td></tr>
 * <tr><td>FileAttribute::FilePermissions</td><td>Mask to isolate file permission flags</td></tr>
 * </table>
 */
namespace FileAttribute
{
   enum mask_type
   {
      /*
       * File type
       */
      Block          = 0x00000001,  // Block special
      Character      = 0x00000002,  // Character special
      FIFO           = 0x00000004,  // FIFO special
      Regular        = 0x00000008,  // Regular file
      Directory      = 0x00000010,  // Directory
      SymbolicLink   = 0x00000020,  // Symbolic link
      Socket         = 0x00000040,  // Socket
      FileType       = 0x000000FF,

      /*
       * File attributes
       * Windows-exclusive, except ReadOnly and Hidden, which we emulate on X.
       */
      Archive        = 0x00001000,  // File is archived
      Compressed     = 0x00002000,  // File is compressed
      Encrypted      = 0x00004000,  // File is encrypted
      Hidden         = 0x00008000,  // File is hidden
      ReadOnly       = 0x00010000,  // File is read-only
      System         = 0x00020000,  // File is a system file
      Temporary      = 0x00040000,  // File is a temporary file
      FileAttributes = 0x000FF000,

      /*
       * File permissions
       */
      Read           = 0x00100000,  // Owner can read
      Write          = 0x00200000,  // Owner can write
      Execute        = 0x00400000,  // Owner can execute/search
      ReadGroup      = 0x01000000,  // Group can read
      WriteGroup     = 0x02000000,  // Group can write
      ExecuteGroup   = 0x04000000,  // Group can execute/search
      ReadOthers     = 0x10000000,  // Others can read
      WriteOthers    = 0x20000000,  // Others can write
      ExecuteOthers  = 0x40000000,  // Others can execute/search
      FilePermissions= 0xFFF00000
   };
}

/*!
 * A combination of file type, attribute and access mode flags.
 */
typedef Flags<FileAttribute::mask_type>   FileAttributes;

// ----------------------------------------------------------------------------

/*!
 * \struct FileTime
 * \brief File date and time.
 */
struct FileTime
{
   uint16   year         : 16; //!< Year
   uint8    month        :  4; //!< Month [1,12]
   uint8    day          :  5; //!< Day [1,31]
   uint8    hour         :  5; //!< Hour [0,23]
   int                   :  2;
   uint8    minute       :  6; //!< Minute [0,59]
   uint8    second       :  6; //!< Second [0,59]
   int                   :  4;
   uint16   milliseconds : 10; //!< Milliseconds [0,999]
   int                   :  6;

   /*!
    * Returns true if this file time is equal to another object \a t.
    */
   bool operator ==( const FileTime& t ) const
   {
      return second == t.second && minute == t.minute && hour == t.hour &&
             day == t.day && month == t.month && year == t.year &&
             milliseconds == t.milliseconds;
   }

   /*!
    * Returns true if this file time precedes another object \a t.
    */
   bool operator <( const FileTime& t ) const
   {
      if ( year != t.year ) return year < t.year;
      if ( month != t.month ) return month < t.month;
      if ( day != t.day ) return day < t.day;
      if ( hour != t.hour ) return hour < t.hour;
      if ( minute != t.minute ) return minute < t.minute;
      if ( second != t.second ) return second < t.second;
      if ( milliseconds != t.milliseconds ) return milliseconds < t.milliseconds;
      return false;
   }

   /*!
    * Returns this file date and time as a Julian day number in local time.
    */
   double ToJD() const;

   /*!
    * Returns this file date and time as a Julian day number in the Coordinated
    * Universal Time (UTC) time scale.
    *
    * This function takes into account the difference between local time and
    * UTC on systems that support time zones.
    */
   double ToJDUTC() const;

   /*!
    * Returns the elapsed time in days since 1970 January 1 00:00:00 UTC.
    *
    * This function takes into account the difference between local time and
    * Coordinated Universal Time (UTC) on systems that support time zones.
    */
   double DaysSinceEpoch() const
   {
      return ToJDUTC() - 2440587.5;
   }

   /*!
    * Returns the elapsed time in seconds since 1970 January 1 00:00:00 UTC.
    *
    * This function takes into account the difference between local time and
    * Coordinated Universal Time (UTC) on systems that support time zones.
    */
   double SecondsSinceEpoch() const
   {
      return 86400*DaysSinceEpoch();
   }
};

// ----------------------------------------------------------------------------

/*!
 * \struct FindFileInfo
 * \brief %File information structure used by File::Find()
 */
struct FindFileInfo
{
   String         name;          //!< File or directory name, including the file name extension.
   FileAttributes attributes;    //!< Item attributes.
   fsize_type     size;          //!< File size in bytes.
   int            numberOfLinks; //!< Number of existing hard links to this file.
   int            userId;        //!< User id of the file owner.
   int            groupId;       //!< Group id of the file owner.
   FileTime       created;       //!< Creation time.
   FileTime       lastAccessed;  //!< Time of last access.
   FileTime       lastModified;  //!< Time of last change.

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a directory.
    */
   bool IsDirectory() const
   {
      return attributes.IsFlagSet( FileAttribute::Directory );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a file with the
    * FileAttribute::Archive attribute set.
    */
   bool IsArchive() const
   {
      return attributes.IsFlagSet( FileAttribute::Archive );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a file with the
    * FileAttribute::Compressed attribute set.
    */
   bool IsCompressed() const
   {
      return attributes.IsFlagSet( FileAttribute::Compressed );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a file with the
    * FileAttribute::Encrypted attribute set.
    */
   bool IsEncrypted() const
   {
      return attributes.IsFlagSet( FileAttribute::Encrypted );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a file with the
    * FileAttribute::Hidden attribute set.
    */
   bool IsHidden() const
   {
      return attributes.IsFlagSet( FileAttribute::Hidden );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a file with the
    * FileAttribute::ReadOnly attribute set.
    */
   bool IsReadOnly() const
   {
      return attributes.IsFlagSet( FileAttribute::ReadOnly );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a file with the
    * FileAttribute::System attribute set.
    */
   bool IsSystem() const
   {
      return attributes.IsFlagSet( FileAttribute::System );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to a file with the
    * FileAttribute::Temporary attribute set.
    */
   bool IsTemporary() const
   {
      return attributes.IsFlagSet( FileAttribute::Temporary );
   }

   /*!
    * Returns true if this %FindFileInfo structure corresponds to an empty file.
    */
   bool IsEmpty() const
   {
      return size == 0;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \namespace ReadTextOption
 * \brief File text reading options.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>RemoveEmptyLines</td>   <td>Discard empty text lines (after trimming whitespace, when applicable).</td></tr>
 * <tr><td>TrimTrailingSpaces</td> <td>Delete trailing whitespace characters from input text lines.</td></tr>
 * <tr><td>TrimLeadingSpaces</td>  <td>Delete leading whitespace characters from input text lines.</td></tr>
 * <tr><td>TrimSpaces</td>         <td>Delete both leading and trailing whitespace characters from input text lines.</td></tr>
 * </table>
 *
 * \sa File::ReadLines(), File::ScanLines()
 */
namespace ReadTextOption
{
   enum mask_type
   {
      RemoveEmptyLines   = 0x0001,
      TrimTrailingSpaces = 0x0002,
      TrimLeadingSpaces  = 0x0004,
      TrimSpaces         = TrimTrailingSpaces | TrimLeadingSpaces
   };
}

/*!
 * A combination of file text reading mode flags.
 *
 * \sa File::ReadLines(), File::ScanLines()
 */
typedef Flags<ReadTextOption::mask_type>  ReadTextOptions;

// ----------------------------------------------------------------------------

/*!
 * \class File
 * \brief A portable interface to disk files
 *
 * ### TODO: Write a detailed description for %File.
 */
class PCL_CLASS File
{
public:

   /*!
    * Represents a low-level opaque handle to a file.
    */
   typedef void*  handle;

   /*!
    * Represents a file seek mode.
    */
   typedef SeekMode::value_type  seek_mode;

   // -------------------------------------------------------------------------

   /*!
    * \class Error
    * File I/O exception.
    */
   class Error : public pcl::Error
   {
   public:

      /*!
       * Constructs a %File::Error object with the specified full file
       * \a path and error \a message.
       */
      Error( const String& filePath, const String& message ) :
         pcl::Error( message ),
         m_filePath( filePath )
      {
      }

      /*!
       * Copy constructor.
       */
      Error( const Error& ) = default;

      /*!
       */
      virtual String ExceptionClass() const
      {
         return "File I/O Error";
      }

      /*!
       * Returns the full file path associated to this file error.
       */
      virtual String FilePath() const
      {
         return m_filePath;
      }

      /*!
       */
      virtual String ErrorMessage() const
      {
         return m_message;
      }

      /*!
       */
      virtual String Message() const
      {
         String filePath = FilePath();
         String message = m_message;
         if ( !filePath.IsEmpty() )
         {
            message += ": ";
            message += filePath;
         }
         return message;
      }

   private:

      String m_filePath;
   };

   // -------------------------------------------------------------------------

   /*!
    * \class Find
    * \brief Directory search operation.
    */
   class PCL_CLASS Find
   {
   public:

      /*!
       * Constructs an inactive %File::Find object.
       */
      Find() : m_handle( nullptr )
      {
      }

      /*!
       * Constructs a %File::Find object and starts a new directory search
       * operation for the specified \a path.
       *
       * The specified \a path can include wildcards to define a pattern to
       * search for a set of directory items.
       */
      Find( const String& path ) : m_handle( nullptr )
      {
         Begin( path );
      }

      /*!
       * Destroys a %File::Find object. If a directory search operation is
       * currently active, it is finalized.
       */
      ~Find()
      {
         End();
      }

      /*!
       * Starts a new directory search operation for the specified \a path.
       *
       * The specified \a path can include wildcards to define a pattern to
       * search for a set of directory items.
       */
      void Begin( const String& path );

      /*!
       * Retrieves the next item in the current file search operation. Returns
       * true if there are more items in the current file search; false if
       * there are no more items.
       *
       * The \a info structure receives the data corresponding to the next
       * search item.
       */
      bool NextItem( FindFileInfo& info );

      /*!
       * Finalizes the current file search operation, if there is one active.
       */
      void End();

      /*!
       * Returns the search path of the current file search operation.
       */
      String SearchPath() const
      {
         return m_searchPath;
      }

   private:

      FindFileInfo m_info;
      String       m_searchPath;
#ifndef __PCL_WINDOWS
      String       m_searchDir;
      String       m_searchName;
#endif
      void*        m_handle;

      Find( const Find& ) = delete;
      Find& operator =( const Find& ) = delete;
   };

   // -------------------------------------------------------------------------

   /*!
    * \class Progress
    * \brief Abstract base class of file progress objects
    *
    * The purpose of a file progress object is to provide feedback to the user
    * during potentially long file tasks, such as copying, moving and deleting
    * large files or file sets. The %File::Progress class provides a simple
    * template with a text line, an increasing counter, and the possibility to
    * abort a long operation.
    */
   class PCL_CLASS Progress
   {
   public:

      /*!
       * Constructs a %File::Progress object.
       *
       * \param total         Total progress count (for example, the total size
       *                      in bytes of a file being copied, or the total
       *                      number of files being deleted).
       *
       * \param initialText   Initial progress text. The default value is an
       *                      empty string.
       *
       * \param initialValue  Initial progress value. The default value is
       *                      zero.
       */
      Progress( fsize_type total, const String& initialText = String(), fsize_type initialValue = 0 ) :
         m_total( Max( fsize_type( 0 ), total ) ),
         m_current( Range( initialValue, fsize_type( 0 ), m_total ) ),
         m_text( initialText ),
         m_abort( false )
      {
      }

      /*!
       * Destroys a %File::Progress object.
       */
      virtual ~Progress()
      {
      }

      /*!
       * Returns the total progress count.
       */
      fsize_type Total() const
      {
         return m_total;
      }

      /*!
       * Returns the current progress count.
       */
      fsize_type Value() const
      {
         return m_current;
      }

      /*!
       * Returns the current progress text.
       */
      const String& Text() const
      {
         return m_text;
      }

      /*!
       * Sets the \a current progress count.
       *
       * This member function can abort the ongoing task. In such case, a
       * ProcessAborted exception is thrown and the IsAborted() member function
       * returns true.
       */
      void SetValue( fsize_type current )
      {
         if ( current >= m_current )
         {
            m_current = current;

            if ( !ValueChanged() )
            {
               m_abort = true;
               throw ProcessAborted();
            }
         }
      }

      /*!
       * Adds \a delta items to the current progress count.
       *
       * This member function can abort the ongoing task. In such case, a
       * ProcessAborted exception is thrown and the IsAborted() member function
       * returns true.
       */
      void Add( fsize_type delta )
      {
         if ( delta >= 0 )
         {
            m_current += delta;

            if ( !ValueChanged() )
            {
               m_abort = true;
               throw ProcessAborted();
            }
         }
      }

      /*!
       * Changes the current progress \a text.
       *
       * This member function can abort the ongoing task. In such case, a
       * ProcessAborted exception is thrown and the IsAborted() member function
       * returns true.
       */
      void SetText( const String& text )
      {
         m_text = text;

         if ( !TextChanged() )
         {
            m_abort = true;
            throw ProcessAborted();
         }
      }

      /*!
       * Returns true if this object aborted a running task.
       */
      bool IsAborted() const
      {
         return m_abort;
      }

   protected:

      fsize_type m_total;
      fsize_type m_current;
      String     m_text;
      bool       m_abort : 1;

      /*!
       * Function called when the current progress count has been changed.
       * Returns true to continue the running task; false to abort.
       *
       * This is a pure virtual function that must be reimplemented by derived
       * classes.
       */
      virtual bool ValueChanged() = 0;

      /*!
       * Function called when the current progress text has been changed.
       * Returns true to continue the running task; false to abort.
       *
       * This is a pure virtual function that must be reimplemented by derived
       * classes.
       */
      virtual bool TextChanged() = 0;
   };

   // -------------------------------------------------------------------------

   /*!
    * Constructs a %File object that does not represent an existing disk file.
    */
   File()
   {
      Initialize();
   }

   /*!
    * Constructs a %File object that opens a file at a given \a path with the
    * specified \a mode.
    */
   File( const String& path, FileModes mode )
   {
      PCL_PRECONDITION( !path.IsEmpty() )
      Initialize();
      Open( path, mode );
   }

   /*!
    * Move constructor.
    */
   File( File&& x ) :
      m_fileHandle( x.m_fileHandle ),
      m_filePath( std::move( x.m_filePath ) ),
      m_fileMode( x.m_fileMode )
   {
      x.m_fileHandle = s_invalidHandle;
      x.m_fileMode = FileMode::Zero;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   File& operator =( File&& x )
   {
      if ( &x != this )
      {
         if ( IsOpen() )
            Close();
         m_fileHandle = x.m_fileHandle;
         m_filePath = std::move( x.m_filePath );
         m_fileMode = x.m_fileMode;
         x.m_fileHandle = s_invalidHandle;
         x.m_fileMode = FileMode::Zero;
      }
      return *this;
   }

   /*!
    * Destroys a %File object. If this instance represents an open file, it is
    * closed upon destruction.
    */
   virtual ~File()
   {
      if ( IsOpen() )
         Close();
   }

   /*!
    * Returns true if this %File object represents an open disk file.
    */
   bool IsOpen() const
   {
      return IsValidHandle( m_fileHandle );
   }

   /*!
    * Returns the path of the file represented by this %File object.
    * Returned file paths are always absolute, full file paths.
    */
   const String& FilePath() const
   {
      return m_filePath;
   }

   /*!
    * Returns the path of the file represented by this %File object.
    *
    * \deprecated This member function has been deprecated. Use FilePath() in
    * newly produced code.
    */
   const String& FileName() const
   {
      return FilePath();
   }

   /*!
    * Returns the file mode for this %File object.
    */
   FileModes Mode() const
   {
      return m_fileMode;
   }

   /*!
    * Returns true if this file allows read access.
    */
   virtual bool CanRead() const
   {
      return m_fileMode.IsFlagSet( FileMode::Read );
   }

   /*!
    * Returns true if this file allows write access.
    */
   virtual bool CanWrite() const
   {
      return m_fileMode.IsFlagSet( FileMode::Write );
   }

   /*!
    * Returns the current file position, relative to the beginning of the file
    * represented by this %File object.
    */
   virtual fpos_type Position() const;

   /*!
    * Sets the file position relative to the file beginning.
    */
   virtual void SetPosition( fpos_type pos );

   /*!
    * Moves the file pointer. Returns the resulting pointer position from the
    * file beginning.
    *
    * \param dist    Distance to move in bytes.
    * \param mode    Seek mode. See the SeekMode namespace for valid modes.
    */
   fpos_type Seek( fpos_type dist, seek_mode mode = SeekMode::FromCurrent );

   /*!
    * Moves the file pointer after the end of the file.
    * Returns the size of this file, that is, the absolute position of the last
    * byte plus one.
    *
    * This is a convenience member function, equivalent to
    * Seek( 0, SeekMode::FromEnd )
    */
   fpos_type SeekEnd()
   {
      return Seek( 0, SeekMode::FromEnd );
   }

   /*!
    * Moves the file pointer to the beginning of this file.
    *
    * This is a convenience member function, equivalent to SetPosition( 0 )
    */
   void Rewind()
   {
      SetPosition( 0 );
   }

   /*!
    * Returns true if the file pointer has been moved beyond the end of the
    * file.
    */
   bool IsEOF() const
   {
      return Position() >= Size();
   }

   /*!
    * Returns the length in bytes of this file.
    */
   virtual fsize_type Size() const;

   /*!
    * Truncates or extends this file to the specified \a length.
    */
   virtual void Resize( fsize_type length );

   /*!
    * Reads a contiguous block of \a len bytes into the specified \a buffer.
    */
   virtual void Read( void* buffer, fsize_type len );

   /*!
    * Reads an object \a x of type T.
    */
   template <typename T>
   void Read( T& x )
   {
      Read( (void*)&x, sizeof( T ) );
   }

   /*!
    * Reads a set of \a n consecutive objects of type T into the array pointed
    * to by \a a.
    *
    * This is a convenience member function, equivalent to
    * Read( (void*)a, n*sizeof( T ) )
    */
   template <typename T>
   void ReadArray( T* a, size_type n )
   {
      Read( (void*)a, n*sizeof( T ) );
   }

   /*!
    * Reads a numeric variable as a 32-bit signed integer.
    */
   template <typename T>
   void ReadI32( T& x )
   {
      int32 i; Read( i ); x = T( i );
   }

   /*!
    * Reads a numeric variable as a 32-bit unsigned integer.
    */
   template <typename T>
   void ReadUI32( T& x )
   {
      uint32 i; Read( i ); x = T( i );
   }

   /*!
    * Reads a numeric variable as a 64-bit signed integer.
    */
   template <typename T>
   void ReadI64( T& x )
   {
      int64 i; Read( i ); x = T( i );
   }

   /*!
    * Reads a numeric variable as a 64-bit unsigned integer.
    */
   template <typename T>
   void ReadUI64( T& x )
   {
      uint64 i; Read( i ); x = T( i );
   }

   /*!
    * Writes a contiguous block of \a len bytes from the specified \a buffer.
    */
   virtual void Write( const void* buffer, fsize_type len );

   /*!
    * Writes an object \a x of type T.
    */
   template <typename T>
   void Write( const T& x )
   {
      Write( (const void*)&x, sizeof( T ) );
   }

   /*!
    * Writes a set of \a n consecutive objects of type T from the array pointed
    * to by \a a.
    *
    * This is a convenience member function, equivalent to
    * Write( (const void*)a, n*sizeof( T ) )
    */
   template <typename T>
   void WriteArray( const T* a, size_type n )
   {
      Write( (const void*)a, n*sizeof( T ) );
   }

   /*!
    * Writes a numeric variable as a 32-bit signed integer.
    */
   template <typename T>
   void WriteI32( const T& x )
   {
      Write( int32( x ) );
   }

   /*!
    * Writes a numeric variable as a 32-bit unsigned integer.
    */
   template <typename T>
   void WriteUI32( const T& x )
   {
      Write( uint32( x ) );
   }

   /*!
    * Writes a numeric variable as a 64-bit signed integer.
    */
   template <typename T>
   void WriteI64( const T& x )
   {
      Write( int64( x ) );
   }

   /*!
    * Writes a numeric variable as a 64-bit unsigned integer.
    */
   template <typename T>
   void WriteUI64( const T& x )
   {
      Write( uint64( x ) );
   }

   /*!
    * Reads a boolean variable.
    *
    * This function reads a 32-bit integer and assigns true to the variable
    * \a b if the read number is nonzero; false otherwise.
    */
   void Read( bool& b )
   {
      unsigned i; ReadUI32( i ); b = i != 0;
   }

   /*!
    * Writes a boolean variable.
    *
    * This function writes a 32-bit integer with a value of 1 (one) if the
    * variable \a b is true, zero otherwise.
    */
   void Write( const bool& b )
   {
      WriteUI32( b ? 1 : 0 );
   }

   /*!
    * Reads a dynamic 8-bit string stored as binary data. The data should have
    * been generated by a previous call to Write( const IsoString& ) on the
    * file being read.
    */
   void Read( IsoString& s );

   /*!
    * Reads a dynamic UTF-16 string stored as binary data. The data should have
    * been generated by a previous call to Write( const IsoString& ) on the
    * file being read.
    */
   void Read( String& s );

   /*!
    * Writes a sequence of 8-bit characters, defined by the range [i,j), as
    * binary data.
    */
   void Write( const char* i, const char* j );

   /*!
    * Writes a null-terminated 8-bit string as binary data.
    */
   void Write( const char* s )
   {
      Write( s, s + IsoCharTraits::Length( s ) );
   }

   /*!
    * Writes a dynamic 8-bit string as binary data.
    */
   void Write( const IsoString& s )
   {
      Write( s.Begin(), s.End() );
   }

   /*!
    * Writes a sequence of UTF-16 characters, defined by the range [i,j), as
    * binary data.
    */
   void Write( const char16_type* i, const char16_type* j );

   /*!
    * Writes a null-terminated string of UTF-16 characters as binary data.
    */
   void Write( const char16_type* s )
   {
      Write( s, s + CharTraits::Length( s ) );
   }

   /*!
    * Writes a dynamic UTF-16 string as binary data.
    */
   void Write( const String& s )
   {
      Write( s.Begin(), s.End() );
   }

#ifndef __PCL_NO_FLAGS_FILE_IO

   /*!
    * Reads a set of flags \a f.
    */
   template <typename E>
   static void Read( Flags<E>& f )
   {
      Read( f.m_flags );
   }

   /*!
    * Reads a set of flags \a f as an unsigned 32-bit integer.
    */
   template <typename E>
   static void ReadUI32( Flags<E>& f )
   {
      ReadUI32( f.m_flags );
   }

   /*!
    * Writes a set of flags \a f.
    */
   template <typename E>
   static void Write( Flags<E> f )
   {
      Write( f.m_flags );
   }

   /*!
    * Writes a set of flags \a f as an unsigned 32-bit integer.
    */
   template <typename E>
   static void WriteUI32( Flags<E> f )
   {
      WriteUI32( f.m_flags );
   }

#endif

   /*!
    * Writes a sequence of 8-bit characters, defined by the range [i,j), as
    * plain text.
    */
   void OutText( const char* i, const char* j )
   {
      if ( i < j )
         Write( (const void*)i, fsize_type( j - i ) );
   }

   /*!
    * Writes a null-terminated 8-bit string as plain text.
    */
   void OutText( const char* s )
   {
      OutText( s, s + IsoCharTraits::Length( s ) );
   }

   /*!
    * Writes a dynamic 8-bit string as plain text.
    */
   void OutText( const IsoString& s )
   {
      OutText( s.Begin(), s.End() );
   }

   /*!
    * Writes a sequence of UTF-16 characters, defined by the range [i,j), as
    * plain text.
    */
   void OutText( const char16_type* i, const char16_type* j )
   {
      if ( i < j )
         Write( (const void*)i, fsize_type( j - i ) << 1 );
   }

   /*!
    * Writes a null-terminated UTF-16 string as plain text.
    */
   void OutText( const char16_type* s )
   {
      OutText( s, s + CharTraits::Length( s ) );
   }

   /*!
    * Writes a dynamic UTF16 string as plain text.
    */
   void OutText( const String& s )
   {
      OutText( s.Begin(), s.End() );
   }

   /*!
    * Writes a sequence of 8-bit characters, defined by the range [i,j), as
    * plain text. Then writes a newline character.
    */
   void OutTextLn( const char* i, const char* j )
   {
      OutText( i, j ); Write( '\n' );
   }

   /*!
    * Writes a null-terminated 8-bit string as plain text and appends a newline
    * character.
    */
   void OutTextLn( const char* s )
   {
      OutText( s ); Write( '\n' );
   }

   /*!
    * Writes an 8-bit newline character ('\n').
    *
    * \warning Do not use this member function if you are generating plain text
    * encoded as UTF-16, for example in a sequence of calls to
    * OutTextLn( const String& s ). In such case you can call:\n
    * \n
    * \code OutTextLn( String() ); \endcode \n
    * \n
    * to generate a newline in UTF-16 format.
    */
   void OutTextLn()
   {
      Write( '\n' );
   }

   /*!
    * Writes a dynamic 8-bit string as plain text and appends a newline
    * character.
    */
   void OutTextLn( const IsoString& s )
   {
      OutText( s ); Write( '\n' );
   }

   /*!
    * Writes a sequence of UTF-16 characters, defined by the range [i,j), as
    * plain text. Then writes a newline character.
    */
   void OutTextLn( const char16_type* i, const char16_type* j )
   {
      OutText( i, j ); Write( char16_type( '\n' ) );
   }

   /*!
    * Writes a null-terminated UTF-16 string as plain text and appends a
    * newline character.
    */
   void OutTextLn( const char16_type* s )
   {
      OutText( s ); Write( char16_type( '\n' ) );
   }

   /*!
    * Writes a dynamic UTF16 string as plain text and appends a newline
    * character.
    */
   void OutTextLn( const String& s )
   {
      OutText( s ); Write( char16_type( '\n' ) );
   }

   /*!
    * Commits any pending write operations.
    */
   void Flush();

   /*!
    * Opens or creates a file.
    *
    * \param path    %File path.
    * \param mode    %File access, share and opening/creation mode.
    */
   virtual void Open( const String& path, FileModes mode = FileMode::Read|FileMode::Open );

   /*!
    * Opens a file at the specified \a path for read-only access.
    */
   virtual void OpenForReading( const String& path )
   {
      Open( path, FileMode::Read|FileMode::Open );
   }

   /*!
    * Returns an open file at the specified \a path, ready for read-only
    * access.
    */
   static File OpenFileForReading( const String& path )
   {
      File f;
      f.OpenForReading( path );
      return f;
   }

   /*!
    * Opens a file at \a path for read/write access.
    */
   virtual void OpenForReadWrite( const String& path )
   {
      Open( path, FileMode::Read|FileMode::Write|FileMode::Open );
   }

   /*!
    * Returns an open file at the specified \a path, ready for read/write
    * access.
    */
   static File OpenFileForReadWrite( const String& path )
   {
      File f;
      f.OpenForReadWrite( path );
      return f;
   }

   /*!
    * Creates a file at the specified \a path for read/write access. If a file
    * already exists with the same \a path, its contents will be truncated to
    * zero length.
    */
   virtual void Create( const String& path )
   {
      Open( path, FileMode::Read|FileMode::Write|FileMode::Create );
   }

   /*!
    * Returns a newly created file at the specified \a path, ready for
    * read/write access. If a file already exists with the same \a path, its
    * contents will be truncated to zero length.
    */
   static File CreateFile( const String& path )
   {
      File f;
      f.Create( path );
      return f;
   }

   /*!
    * Creates a file at the specified \a path for write-only access. If a file
    * already exists with the same \a path, its contents will be truncated to
    * zero length.
    */
   virtual void CreateForWriting( const String& path )
   {
      Open( path, FileMode::Write|FileMode::Create );
   }

   /*!
    * Returns a newly created file at the specified \a path, ready for
    * write-only access. If a file already exists with the same \a path, its
    * contents will be truncated to zero length.
    */
   static File CreateFileForWriting( const String& path )
   {
      File f;
      f.CreateForWriting( path );
      return f;
   }

   /*!
    * Opens a file at the specified \a path if it exists, or creates it
    * otherwise. The file will be opened in read/write mode.
    */
   void OpenOrCreate( const String& path )
   {
      Open( path, FileMode::Read|FileMode::Write|FileMode::Open|FileMode::Create );
   }

   /*!
    * Returns an open file at the specified \a path if it already exists, or a
    * newly created file otherwise, ready for read/write access.
    */
   static File OpenOrCreateFile( const String& path )
   {
      File f;
      f.OpenOrCreate( path );
      return f;
   }

   /*!
    * Closes an open file.
    */
   virtual void Close();

   /*!
    * \defgroup file_utilities File Utility Routines
    */

   /*!
    * Removes an existing file at the specified \a filePath.
    * \ingroup file_utilities
    */
   static void Remove( const String& filePath );

   /*!
    * Creates a new directory at the specified \a dirPath.
    *
    * If \a createIntermediateDirectories is true (the default value) and
    * \a dirPath specifies one or more parent directories that don't exist,
    * they are created automatically.
    *
    * \ingroup file_utilities
    */
   static void CreateDirectory( const String& dirPath, bool createIntermediateDirectories = true );

   /*!
    * Removes an existing \e empty directory at the specified \a dirPath.
    *
    * The specified directory must be empty, or this function will fail.
    *
    * \ingroup file_utilities
    */
   static void RemoveDirectory( const String& dirPath );

   /*!
    * Moves and/or renames a file.
    *
    * \param filePath      Current path of the file to move/rename
    *
    * \param newFilePath   New path. If the directory in \a newFilePath is the
    *             same as in \a filePath, or if no directory is specified, the
    *             file is just renamed. If a different directory is specified,
    *             the file is also moved.
    *
    * \warning This function can only move or rename a file to stay in the same
    * physical device. To move a file across devices, use the File::MoveFile()
    * static member function.
    *
    * \ingroup file_utilities
    */
   static void Move( const String& filePath, const String& newFilePath );

   /*!
    * Returns true if a file at the specified \a filePath exists.
    * \ingroup file_utilities
    */
   static bool Exists( const String& filePath );

   /*!
    * Returns true if a directory at the specified \a dirPath exists.
    * \ingroup file_utilities
    */
   static bool DirectoryExists( const String& dirPath );

   /*!
    * Returns true if the file at \a filePath exists and the current user has
    * read-only access to it.
    * \ingroup file_utilities
    */
   static bool IsReadOnly( const String& filePath );

   /*!
    * Enables or disables read-only access to a file at the specified
    * \a filePath.
    * \ingroup file_utilities
    */
   static void SetReadOnly( const String& filePath, bool rdOnly = true );

   /*!
    * Disables or enables read-only access to a file at the specified
    * \a filePath.
    *
    * This is a convenience member function, equivalent to
    * SetReadOnly( filePath, false )
    *
    * \ingroup file_utilities
    */
   static void SetReadWrite( const String& filePath )
   {
      SetReadOnly( filePath, false );
   }

   /*!
    * Reads the contents of a file at the specified \a filePath and returns
    * them as a ByteArray object.
    *
    * \ingroup file_utilities
    */
   static ByteArray ReadFile( const String& filePath );

   /*!
    * Creates a file with the specified contents.
    *
    * \param filePath   Path to the file that will be created.
    *
    * \param contents   Reference to a ByteArray object providing the contents
    *                   that will be written to the newly created file.
    *
    * Typically this function is used along with File::ReadFile() to implement
    * a file copying routine; for example:
    *
    * \code
    * void CopyFile( const String& targetFilePath, const String& sourceFilePath )
    * {
    *    File::WriteFile( targetFilePath, File::ReadFile( sourceFilePath ) );
    * }
    * \endcode
    *
    * \warning If a file already exists at the specified path, its previous
    * contents will be lost after calling this function.
    *
    * \ingroup file_utilities
    */
   static void WriteFile( const String& filePath, const ByteArray& contents );

   /*!
    * Creates a file with a subset of a ByteArray container.
    *
    * \param filePath   Path to the file that will be created.
    *
    * \param contents   Reference to a ByteArray object providing the contents
    *                   that will be written to the newly created file.
    *
    * \param start      Zero-based index of the first byte to be written.
    *
    * \param size       Number of bytes that will be written.
    *
    * \warning If a file already exists at the specified path, its previous
    * contents will be lost after calling this function.
    */
   static void WriteFile( const String& filePath, const ByteArray& contents, size_type start, size_type size );

   /*!
    * Reads the contents of a file at the specified \a filePath and returns
    * them as plain text stored in a dynamic 8-bit string.
    *
    * This function is useful to load document files encoded in UTF-8,
    * ISO/IEC-8859-1 (or Latin-1), and other 8-bit encodings.
    *
    * \ingroup file_utilities
    */
   static IsoString ReadTextFile( const String& filePath );

   /*!
    * Creates a file with the specified plain text content.
    *
    * \param filePath   Path to the file that will be created.
    *
    * \param text       Reference to an IsoString object providing the plain
    *                   text that will be written to the newly created file.
    *
    * This function is useful to generate document files encoded in UTF-8,
    * ISO/IEC-8859-1 (or Latin-1), and other 8-bit encodings.
    *
    * \warning If a file already exists at the specified path, its previous
    * contents will be lost after calling this function.
    *
    * \ingroup file_utilities
    */
   static void WriteTextFile( const String& filePath, const IsoString& text );

   /*!
    * Changes the access permissions of a file or directory.
    *
    * \param targetPath    Path to the existing file or directory whose
    *                      permissions will be modified.
    *
    * \param permissions   An ORed combination of file permission flags. See
    *                      the FilePermission namespace for contants.
    *
    * On UNIX and Linux platforms (FreeBSD, GNU Linux, Mac OS X), the read,
    * write and execution permissions can be specified separately for the owner
    * user, for the owner group, and for the rest of users. On Windows, only
    * generic read and write permissions can be set, and there are no execution
    * permissions.
    */
   static void SetPermissions( const String& targetPath, FileAttributes permissions );

   /*!
    * Copies access and modification times and access permissions from one
    * file or directory item to another.
    *
    * \param targetPath    Path to the target item whose times and permissions
    *                      will be changed.
    *
    * \param sourcePath    Path to the source item whose times and permissions
    *                      will be copied to the target item.
    *
    * \ingroup file_utilities
    */
   static void CopyTimesAndPermissions( const String& targetPath, const String& sourcePath );

#ifndef __PCL_WINDOWS
   /*!
    * Copies a symbolic link.
    *
    * \param targetLinkPath   Path to the target file. A new symbolic link will
    *                         be created at this path, which will point to the
    *                         same link target as the source link.
    *
    * \param sourceLinkPath   Path to the source link, whose target will be
    *                         copied to the target link.
    *
    * Note that this function creates a duplicate of the symbolic link itself,
    * \e not of its pointed file.
    *
    * \note This static member function is only defined on UNIX and Linux
    * platforms. It does not make sense on Windows, where POSIX symbolic links
    * don't exist.
    *
    * \ingroup file_utilities
    */
   static void CopyLink( const String& targetLinkPath, const String& sourceLinkPath );
#endif // !__PCL_WINDOWS

   /*!
    * Copies a single file.
    *
    * \param targetFilePath   Path to the target file, which must \e not exist.
    *                         If the specified file exists, an Error exception
    *                         will be thrown.
    *
    * \param sourceFilePath   Path to the source file. This file must exist and
    *                         must be different from the specified target file.
    *
    * \param progress         Pointer to an instance of (a derived class of)
    *                         File::Progress. If a non-null pointer is
    *                         specified, the object will be used to provide
    *                         progress information and the possibility of
    *                         aborting the file copy operation. The default
    *                         value is \c nullptr.
    *
    * The file copy operation is implemented as an iterative block copying
    * routine. This means that huge files can be copied with minimal
    * consumption of memory resources and no specific file size limit.
    *
    * Along with the file contents, file access and modification times, as well
    * as file access permissions, will also be copied by this function.
    *
    * This function does not follow symbolic links. It does not copy the file
    * pointed to by a symbolic link, but the link itself. This is only relevant
    * to platforms that support symbolic links (i.e., all of them but Windows).
    *
    * If a \a progress object is specified, it receives successive calls to
    * File::Progress::Add() with increments in bytes. If the progress object
    * aborts the file copy operation, this member function throws a
    * ProcessAborted exception.
    *
    * \ingroup file_utilities
    */
   static void CopyFile( const String& targetFilePath, const String& sourceFilePath, File::Progress* progress = nullptr );

   /*!
    * Moves a single file.
    *
    * \param targetFilePath   Path to the target file, which must \e not exist.
    *                         If the specified file exists, an Error exception
    *                         will be thrown.
    *
    * \param sourceFilePath   Path to the source file. This file must exist and
    *                         must be different from the specified target file.
    *
    * \param progress         Pointer to an instance of (a derived class of)
    *                         File::Progress. If a non-null pointer is
    *                         specified, the object will be used to provide
    *                         progress information and the possibility of
    *                         aborting the file copy operation. The default
    *                         value is \c nullptr.
    *
    * For file move operations within the same physical device, this function
    * implements a simple rename operation. In these cases, the \a progress
    * object (if specified) receives a single call to File::Progress::Add()
    * with the total file size in bytes.
    *
    * When a file is moved to a different device, this member function first
    * copies the source file to the target location using a routine equivalent
    * to Copy(). When the entire file contents have been transferred, the
    * source file is removed.
    *
    * Along with the file contents, file access and modification times, as well
    * as file access permissions, will be preserved by this function.
    *
    * This function does not follow symbolic links. It does not move the file
    * pointed to by a symbolic link, but the link itself. This is only relevant
    * to platforms that support symbolic links (i.e., all of them but Windows).
    *
    * If a \a progress object is specified and it aborts the file move
    * operation, this member function throws a ProcessAborted exception. In
    * such case the source file is guaranteed to remain intact.
    *
    * \ingroup file_utilities
    */
   static void MoveFile( const String& targetFilePath, const String& sourceFilePath, File::Progress* progress = nullptr );

   /*!
    * Returns true if two files or directories are mounted on the same physical
    * device.
    *
    * This is relevant to some critical file operations. For example, moving a
    * file on the same device is a simple and fast rename operation. However,
    * moving a file across devices involves copying the entire file contents.
    *
    * This function does not follow symbolic links. It does not consider
    * mounted devices for files pointed to by symbolic links, but for the links
    * themselves. This is only relevant to platforms that support symbolic
    * links (all of them but Windows).
    *
    * \ingroup file_utilities
    */
   static bool SameDevice( const String& path1, const String& path2 );

   /*!
    * Returns true if two file path specifications refer to the same file on
    * the system.
    *
    * On UNIX and Linux platforms, this function compares the device and inode
    * numbers reported by the st_dev and st_ino members of the stat structure,
    * respectively. This pair of numbers uniquely identifies every file on the
    * system, irrespective of file names.
    *
    * On Windows platforms, this function verifies the devices that support
    * both files and, if they are the same, performs a case-insensitive
    * comparison of the canonical absolute versions of the specified paths.
    *
    * This function does not follow symbolic links. It does not consider
    * mounted devices for files pointed to by symbolic links, but for the links
    * themselves. This is only relevant to platforms that support symbolic
    * links (all of them but Windows).
    *
    * \ingroup file_utilities
    */
   static bool SameFile( const String& path1, const String& path2 );

   /*!
    * Reads all existing text lines from a file as 8-bit strings (e.g.,
    * ISO 8859-1 or UTF-8 formats) and returns them as a string list.
    *
    * \param filePath      Path to the source file from which text lines will
    *                      be read.
    *
    * \param options       Optional set of flags (ORed combination) that can be
    *                      specified to control the way text lines are
    *                      extracted and stored. Available flags are defined in
    *                      the ReadTextOption namespace.
    *
    * Returns a string list (IsoStringList) with all existing lines in the
    * specified file. If the file is empty, an empty string list is returned.
    *
    * This routine automatically detects and processes line ending sequences
    * characteristic of all supported platforms: UNIX (LF = 0x0A), Windows
    * (CR+LF = 0x0D+0x0A) and legacy Mac (CR = 0x0D).
    *
    * \ingroup file_utilities
    */
   static IsoStringList ReadLines( const String& filePath,
                                   ReadTextOptions options = ReadTextOptions() );

   /*!
    * Iterates a function through all existing text lines in a file.
    *
    * \param filePath      Path to the source file from which text lines will
    *                      be read. This file is assumed to store text as a
    *                      sequence of 8-bit characters, e.g. ASCII, ISO 8859-1
    *                      or UTF-8.
    *
    * \param callback      Callback function. This function will be invoked
    *                      once for each text line. The function will receive
    *                      two arguments: a char* with the starting address of
    *                      an 8-bit null terminated string, and a void* which
    *                      is the specified \a data argument. The function will
    *                      return true if the process can continue for the next
    *                      text line; false to stop the iteration.
    *
    * \param data          A void* that will be passed to the callback function
    *                      as its second argument on each call. The default
    *                      value is \c nullptr.
    *
    * \param options       Optional set of flags (ORed combination) that can be
    *                      specified to control the way text lines are explored
    *                      and passed to the callback function. Available flags
    *                      are defined in the ReadTextOption namespace.
    *
    * Returns the number of text lines that have been scanned, i.e. the number
    * of times the callback function has been invoked and has returned true.
    *
    * This routine avoids all the work associated with creating and storing new
    * IsoString objects. When IsoString instances are not necessary to acquire
    * the data being read, this routine can be much faster than ReadLines(),
    * especially for vey large files.
    *
    * This routine automatically detects and processes line ending sequences
    * characteristic of all supported platforms: UNIX (LF = 0x0A), Windows
    * (CR+LF = 0x0D+0x0A) and legacy Mac (CR = 0x0D).
    *
    * \ingroup file_utilities
    */
   static size_type ScanLines( const String& filePath,
                               bool (*callback)( char*, void* ), void* data = nullptr,
                               ReadTextOptions options = ReadTextOptions() );

   /*!
    * Returns the full, absolute path to the specified \a path.
    * \ingroup file_utilities
    */
   static String FullPath( const String& path );

   /*!
    * Returns the system temporary storage directory.
    * \ingroup file_utilities
    */
   static String SystemTempDirectory();

   /*!
    * Converts a path from Windows to UNIX syntax. Replaces all occurrences of
    * the '\\' Windows separator with the '/' UNIX separator.
    * \ingroup file_utilities
    */
   static String WindowsPathToUnix( const String& path );

   /*!
    * Converts a path from UNIX to Windows. Replaces all occurrences of
    * the '/' UNIX separator with the '\\' Windows separator.
    * \ingroup file_utilities
    */
   static String UnixPathToWindows( const String& path );

   /*!
    * Returns a nonexistent, randomly generated file name.
    *
    * \param directory  Name of an existing directory where the generated file
    *                   name is guaranteed to be unique. If empty or not
    *                   specified, the current directory of the calling process
    *                   will be used.
    *
    * \param n          Number of random characters in the generated file name.
    *                   Must be at least five characters. The default value is
    *                   twelve characters.
    *
    * \param prefix     An optional prefix that will be prepended to the
    *                   generated file name.
    *
    * \param postfix    An optional postfix that will be appended to the
    *                   generated file name.
    *
    * This function returns the full absolute path of a nonexistent file name
    * generated by concatenation of \a n random characters. For compatibility
    * with case-insensitive file systems (FAT, NTFS), only uppercase characters
    * and decimal digits are used in the generated file name.
    *
    * The returned file name is guaranteed to be unique on the specified
    * directory (or on the current directory of the calling process if no
    * directory is specified) just after this function returns. Note that since
    * a high-quality random number generator is used to select file name
    * characters, there is no practical chance for a race condition by calling
    * this function from several threads concurrently.
    *
    * \ingroup file_utilities
    */
   static String UniqueFileName( const String& directory = String(), int n = 12,
                                 const String& prefix = String(), const String& postfix = String() );

   /*!
    * Retrieves the storage space available on the device that supports a
    * specified directory.
    *
    * \param dirPath    A path specification to an existing directory on the
    *                   local filesystem. On Windows, UNC paths are fully
    *                   supported. Symbolic links are supported on all
    *                   platforms. If a symbolic link is specified, the
    *                   returned values refer to the target device.
    *
    * \param[out] totalBytes  If non-null, the address of a variable that
    *                   receives the total capacity in bytes of the device that
    *                   supports the specified directory.
    *
    * \param[out] freeBytes   If non-null, the address of a variable that
    *                   receives the total number of free bytes on the device
    *                   that supports the specified directory.
    *
    * Returns the total number of bytes available on the device that supports
    * the specified directory. The returned values take into account any disk
    * quotas that might apply to the user associated with the calling process.
    *
    * \ingroup file_utilities
    */
   static uint64 GetAvailableSpace( const String& dirPath, uint64* totalBytes = nullptr, uint64* freeBytes = nullptr );

   /*!
    * Finds a Windows drive specification in the specified \a path.
    *
    * Returns the character index position of the drive found, or
    * String::notFound.
    *
    * This function only makes sense on Windows. On UNIX and Linux operating
    * systems, this function always returns String::notFound.
    *
    * \ingroup file_utilities
    */
   static size_type FindDrive( const String& path );

   /*!
    * Finds a file name in the specified \a path.
    *
    * Returns the character index position of the name found, or
    * String::notFound.
    *
    * For example, in '/foo/bar.tar.gz' the file name is 'bar'.
    *
    * \ingroup file_utilities
    */
   static size_type FindName( const String& path );

   /*!
    * Finds a file extension in the specified \a path.
    *
    * Returns the character index position of the extension found, or
    * String::notFound.
    *
    * The file extension, or file suffix, is the rightmost substring of \a path
    * starting with the last occurrence of a dot character. For example, in
    * 'foo.tar.gz' the file extension is '.gz'.
    *
    * \sa FindCompleteSuffix()
    * \ingroup file_utilities
    */
   static size_type FindExtension( const String& path );

   /*!
    * A synonym for FindExtension().
    * \ingroup file_utilities
    */
   static size_type FindSuffix( const String& path )
   {
      return FindExtension( path );
   }

   /*!
    * Finds the complete file suffix in the specified \a path.
    *
    * Returns the character index position of the complete suffix found, or
    * String::notFound.
    *
    * The complete suffix is the rightmost substring of the \a path starting
    * with the first occurrence of a dot character. For example, in
    * 'foo.tar.gz' the complete suffix is '.tar.gz'.
    *
    * \sa FindSuffix()
    * \ingroup file_utilities
    */
   static size_type FindCompleteSuffix( const String& path );

   /*!
    * Extracts a Windows drive specification from the specified \a path.
    *
    * Returns the extracted drive specification as a new string, or an empty
    * string if no drive was found.
    *
    * This function only makes sense on Windows. On UNIX and Linux operating
    * systems, this function always returns an empty string.
    *
    * \ingroup file_utilities
    */
   static String ExtractDrive( const String& path ); // always empty string on X

   /*!
    * Extracts a directory specification from the specified \a path.
    *
    * Returns the extracted directory specification, or an empty string if no
    * directory was found. On Windows, the drive element of the path is not
    * included.
    *
    * Examples:
    *
    * In '/foo.tar.gz' the directory is '/'.
    * In '/foo/bar.tar.gz' the directory is '/foo'.
    * In 'foo/bar.tar.gz' the directory is 'foo'.
    * In 'C:/Foo/Bar.txt' the directory is '/Foo'.
    *
    * \ingroup file_utilities
    */
   static String ExtractDirectory( const String& path );

   /*!
    * Extracts a file name from the specified \a path.
    *
    * Returns the extracted name, or an empty string if no file name was found.
    * The file extension or suffix is not included.
    *
    * Examples:
    *
    * In '/foo/bar.tar.gz' the name is 'bar.tar'.
    * In 'C:/Foo/Bar.txt' the name is 'Bar'.
    *
    * \ingroup file_utilities
    */
   static String ExtractName( const String& path );

   /*!
    * Extracts a file extension from the specified \a path.
    *
    * Returns the extracted extension, or an empty string if no file extension
    * was found. The returned extension \e includes the initial dot separator.
    *
    * Examples:
    *
    * In '/foo/bar.tar.gz' the extension is '.gz'.
    * In 'C:/Foo/Bar.txt' the extension is '.txt'.
    *
    * \sa ExtractCompleteSuffix()
    * \ingroup file_utilities
    */
   static String ExtractExtension( const String& path );

   /*!
    * A synonym for ExtractExtension().
    * \ingroup file_utilities
    */
   static String ExtractSuffix( const String& path )
   {
      return ExtractExtension( path );
   }

   /*!
    * Extracts the complete file suffix from the specified \a path.
    *
    * Returns the extracted complete suffix, or an empty string if no file
    * suffix was found. The returned string \e includes the initial dot
    * separator.
    *
    * The complete suffix is the ending substring of the file identifier in the
    * specified \a path, starting with the first occurrence of a dot character.
    * For example, in 'foo.tar.gz' the complete suffix is '.tar.gz'.
    *
    * \sa ExtractSuffix()
    * \ingroup file_utilities
    */
   static String ExtractCompleteSuffix( const String& path );

   /*!
    * Extracts the file name and extension from the specified \a path. Calling
    * this member function is functionally equivalent to:
    *
    *    File::ExtractName( path ) + File::ExtractExtension( path )
    *
    * \ingroup file_utilities
    */
   static String ExtractNameAndExtension( const String& path );

   /*!
    * A synonym for ExtractNameAndExtension().
    * \ingroup file_utilities
    */
   static String ExtractNameAndSuffix( const String& path )
   {
      return ExtractNameAndExtension( path );
   }

   /*!
    * Changes the current file extension in \a filePath to a new extension
    * \a ext. If it is not an empty string, the specified extension \e must
    * include the initial dot separator.
    *
    * If the specified new extension \a ext is an empty string, this function
    * returns \a filePath with its extension (if any) removed.
    *
    * Returns the resulting path after changing the file extension to the
    * specified value.
    *
    * \ingroup file_utilities
    */
   static String ChangeExtension( const String& filePath, const String& ext );

   /*!
    * Appends a \a postfix string to the current file name in the specified
    * \a filePath. Returns the resulting path.
    *
    * \ingroup file_utilities
    */
   static String AppendToName( const String& filePath, const String& postfix );

   /*!
    * Prepends a \a prefix string to the current file name in the specified
    * \a filePath. Returns the resulting path.
    *
    * \ingroup file_utilities
    */
   static String PrependToName( const String& filePath, const String& prefix );

   /*!
    * Returns a readable textual representation of a file size with automatic
    * units conversion.
    *
    * \param sizeInBytes   The file size in bytes.
    *
    * \param precision     The number of decimal digits used to represent
    *                      floating point numbers. The default value is 3.
    *
    * \param alsoShowBytes Whether to add the size in bytes to the string
    *                      representation, irrespective of the units used. The
    *                      default value is false.
    *
    * Examples:
    *
    * \code
    * String s1 = File::SizeAsString( 21771472 );
    *                         // s1 = "20.763 MiB"
    * String s2 = File::SizeAsString( 21771472, 2, true );
    *                         // s2 = "20.76 MiB (21771472 bytes)"
    * String s3 = File::SizeAsString( 4727, 4 );
    *                         // s3 = "4.6162 KiB"
    * \endcode
    */
   static String SizeAsString( fsize_type sizeInBytes, int precision = 3, bool alsoShowBytes = false );

   // -------------------------------------------------------------------------

protected:

   handle    m_fileHandle;
   String    m_filePath;
   FileModes m_fileMode;

   static const handle s_invalidHandle;

   void Initialize();
   virtual bool IsValidHandle( handle h ) const;

   // Cannot copy a File object.
   File( const File& ) = delete;
   File& operator =( const File& ) = delete;
};

// ----------------------------------------------------------------------------

}  // pcl

#endif   // __PCL_File_h

// ----------------------------------------------------------------------------
// EOF pcl/File.h - Released 2015/07/30 17:15:18 UTC
