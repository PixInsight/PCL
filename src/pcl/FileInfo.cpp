//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/FileInfo.cpp - Released 2016/02/21 20:22:19 UTC
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

#include <pcl/FileInfo.h>

#ifdef __PCL_WINDOWS
#  include <io.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <sys/stat.h>
#else
#  include <unistd.h>
#  include <errno.h>
#  include <sys/stat.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

FileInfo::FileInfo()
{
   Clear();
}

FileInfo::FileInfo( const String& path )
{
   Refresh( path );
}

FileInfo& FileInfo::operator =( const FileInfo& x )
{
   m_path              = x.m_path;
   m_attributes        = x.m_attributes;
   m_size              = x.m_size;
   m_numberOfHardLinks = x.m_numberOfHardLinks;
   m_userId            = x.m_userId;
   m_groupId           = x.m_groupId;
   m_created           = x.m_created;
   m_lastAccessed      = x.m_lastAccessed;
   m_lastModified      = x.m_lastModified;
   m_exists            = x.m_exists;
   m_readable          = x.m_readable;
   m_writable          = x.m_writable;
   m_executable        = x.m_executable;
   return *this;
}

#ifdef __PCL_WINDOWS

static String WinErrorMessage( DWORD errorCode )
{
   LPVOID lpMsgBuf;

   ::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     0,
                     errorCode,
                     MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
                     (LPWSTR)&lpMsgBuf,
                     0,
                     0 );

   String msg( reinterpret_cast<char16_type*>( lpMsgBuf ) );

   ::LocalFree( lpMsgBuf );

   return String().Format( "Win32 error (%u): ", errorCode ) + msg;
}

static void WinAttrToPCL( pcl::FileAttributes& a, DWORD wa )
{
   a.SetFlag( FileAttribute::Regular,    (wa & FILE_ATTRIBUTE_DIRECTORY) == 0 );
   a.SetFlag( FileAttribute::Directory,  (wa & FILE_ATTRIBUTE_DIRECTORY) != 0 );
   a.SetFlag( FileAttribute::Archive,    (wa & FILE_ATTRIBUTE_ARCHIVE) != 0 );
   a.SetFlag( FileAttribute::Compressed, (wa & FILE_ATTRIBUTE_COMPRESSED) != 0 );
   a.SetFlag( FileAttribute::Encrypted,  (wa & FILE_ATTRIBUTE_ENCRYPTED) != 0 );
   a.SetFlag( FileAttribute::Hidden,     (wa & FILE_ATTRIBUTE_HIDDEN) != 0 );
   a.SetFlag( FileAttribute::ReadOnly,   (wa & FILE_ATTRIBUTE_READONLY) != 0 );
   a.SetFlag( FileAttribute::System,     (wa & FILE_ATTRIBUTE_SYSTEM) != 0 );
   a.SetFlag( FileAttribute::Temporary,  (wa & FILE_ATTRIBUTE_TEMPORARY) != 0 );
}

static void WinFileTimeToPCL( pcl::FileTime& t, FILETIME ft )
{
   SYSTEMTIME stUTC, stLocal;
   ::FileTimeToSystemTime( &ft, &stUTC );
   ::SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );
   t.year         = stLocal.wYear;
   t.month        = stLocal.wMonth;
   t.day          = stLocal.wDay;
   t.hour         = stLocal.wHour;
   t.minute       = stLocal.wMinute;
   t.second       = stLocal.wSecond;
   // Milliseconds are unreliable for file times on Windows
   t.milliseconds = 0; // stLocal.wMilliseconds;
}

#else // !__PCL_WINDOWS

static void POSIXAttrToPCL( pcl::FileAttributes& a, unsigned mode )
{
   unsigned type = mode & S_IFMT;
   a.SetFlag( FileAttribute::Block,        type == S_IFBLK );
   a.SetFlag( FileAttribute::Character,    type == S_IFCHR );
   a.SetFlag( FileAttribute::FIFO,         type == S_IFIFO );
   a.SetFlag( FileAttribute::Regular,      type == S_IFREG );
   a.SetFlag( FileAttribute::Directory,    type == S_IFDIR );
   a.SetFlag( FileAttribute::SymbolicLink, type == S_IFLNK );
   a.SetFlag( FileAttribute::Socket,       type == S_IFSOCK );

   a.SetFlag( FileAttribute::Read,          (mode & S_IRUSR) != 0 );
   a.SetFlag( FileAttribute::Write,         (mode & S_IWUSR) != 0 );
   a.SetFlag( FileAttribute::Execute,       (mode & S_IXUSR) != 0 );
   a.SetFlag( FileAttribute::ReadGroup,     (mode & S_IRGRP) != 0 );
   a.SetFlag( FileAttribute::WriteGroup,    (mode & S_IWGRP) != 0 );
   a.SetFlag( FileAttribute::ExecuteGroup,  (mode & S_IXGRP) != 0 );
   a.SetFlag( FileAttribute::ReadOthers,    (mode & S_IROTH) != 0 );
   a.SetFlag( FileAttribute::WriteOthers,   (mode & S_IWOTH) != 0 );
   a.SetFlag( FileAttribute::ExecuteOthers, (mode & S_IXOTH) != 0 );
}

static void POSIXFileTimeToPCL( pcl::FileTime& t, time_t ft )
{
   ::tm* lt = ::localtime( &ft );
   t.year         = lt->tm_year + 1900;
   t.month        = lt->tm_mon + 1;
   t.day          = lt->tm_mday;
   t.hour         = lt->tm_hour;
   t.minute       = lt->tm_min;
   t.second       = lt->tm_sec;
   t.milliseconds = 0;
}

#endif // __PCL_WINDOWS

void FileInfo::Refresh()
{
   ClearData();

   if ( m_path.IsEmpty() )
      return;

#ifdef __PCL_WINDOWS

   WIN32_FILE_ATTRIBUTE_DATA data;

   String winPath = File::UnixPathToWindows( m_path );
   if ( !::GetFileAttributesExW( (LPCTSTR)winPath.c_str(),
                                 GetFileExInfoStandard,
                                 (LPVOID)&data ) )
   {
      DWORD error = ::GetLastError();
      if ( error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND )
      {
         m_exists = false;
         return;
      }

      throw Error( "Access error: " + m_path + ": " + WinErrorMessage( error ) );
   }

   WinAttrToPCL( m_attributes, data.dwFileAttributes );

   // File size is unreliable for directories.
   // (See community information on MSDN entry for GetFileAttributesEx())
   if ( m_attributes.IsFlagSet( FileAttribute::Directory ) )
      m_size = 0;
   else
      m_size = fsize_type( data.nFileSizeLow ) | (fsize_type( data.nFileSizeHigh ) << 32);

   m_numberOfHardLinks = 0;
   m_userId            = 0;
   m_groupId           = 0;

   WinFileTimeToPCL( m_created, data.ftCreationTime );
   WinFileTimeToPCL( m_lastAccessed, data.ftLastAccessTime );
   WinFileTimeToPCL( m_lastModified, data.ftLastWriteTime );

   m_exists = true;

   m_readable = ::_waccess_s( (const wchar_t*)winPath.c_str(), 4 ) == 0;
   m_writable = ::_waccess_s( (const wchar_t*)winPath.c_str(), 2 ) == 0;
   m_executable = false; // not available on Windows

#else

   IsoString path8 = m_path.ToUTF8();
   struct stat s;
   errno = 0;
   if ( ::lstat( path8.c_str(), &s ) != 0 )
   {
      if ( errno == ENOENT )
      {
         m_exists = false;
         return;
      }

      if ( errno != 0 )
         throw Error( "Access error: " + m_path + ": " + String( ::strerror( errno ) ) );
      throw Error( "Access error: " + m_path );
   }

   POSIXAttrToPCL( m_attributes, s.st_mode );

   // emulated flags
   m_attributes.SetFlag( FileAttribute::Hidden,   File::ExtractName( m_path ).StartsWith( '.' ) );
   m_attributes.SetFlag( FileAttribute::ReadOnly, m_attributes.IsFlagSet( FileAttribute::Write ) );

   m_size              = s.st_size;
   m_numberOfHardLinks = s.st_nlink;
   m_userId            = s.st_uid;
   m_groupId           = s.st_gid;

   POSIXFileTimeToPCL( m_created,      s.st_ctime );
   POSIXFileTimeToPCL( m_lastAccessed, s.st_atime );
   POSIXFileTimeToPCL( m_lastModified, s.st_mtime );

   m_exists = true;

   m_readable   = ::access( path8.c_str(), R_OK ) == 0;
   m_writable   = ::access( path8.c_str(), W_OK ) == 0;
   m_executable = ::access( path8.c_str(), X_OK ) == 0;

#endif
}

String FileInfo::SymbolicLinkTarget() const
{
#ifdef __PCL_WINDOWS
   return String();
#else
   if ( !m_attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
      return String();

   IsoString path8 = m_path.ToUTF8();
   IsoString link8;
   link8.Reserve( 4096 );
   int len = ::readlink( path8.c_str(), link8.Begin(), 4096-1 );
   if ( len < 0 )
      throw Error( "Unable to read symbolic link contents: " + m_path );
   link8.SetLength( len );
   return link8.UTF8ToUTF16();
#endif
}

void FileInfo::Refresh( const String& path )
{
   m_path = File::FullPath( path );
   Refresh();
}

void FileInfo::Clear()
{
   m_path.Clear();
   ClearData();
}

void FileInfo::ClearData()
{
   m_attributes.Clear();
   m_size = 0;
   m_numberOfHardLinks = 0;
   m_userId = m_groupId = -2;
   m_exists = m_readable = m_writable = m_executable = false;

   union { FileTime time; uint64 u; } t;
   t.u = 0;
   m_created = m_lastAccessed = m_lastModified = t.time;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FileInfo.cpp - Released 2016/02/21 20:22:19 UTC
