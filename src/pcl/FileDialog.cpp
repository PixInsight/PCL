//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/FileDialog.cpp - Released 2017-06-17T10:55:56Z
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

#include <pcl/FileDialog.h>
#include <pcl/GlobalSettings.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FileDialogPrivate
{
public:

   typedef FileDialog::filter_list filter_list;

   String MakeAPIFilters() const
   {
      String apiFilters;
      for ( filter_list::const_iterator i = filters.Begin(); i != filters.End(); ++i )
      {
         if ( i != filters.Begin() )
            apiFilters += ";;";
         apiFilters += (*i).MakeAPIFilter();
      }
      return apiFilters;
   }

   void LoadFiltersFromGlobalString( const IsoString& globalKey )
   {
      filters.Clear();

      String s = PixInsightSettings::GlobalString( globalKey  );
      if ( !s.IsEmpty() )
      {
         for ( size_type p = 0; ; )
         {
            size_type p1 = s.Find( '(', p );
            if ( p1 == String::notFound )
               break;

            size_type p2 = s.Find( ')', p1 );
            if ( p2 == String::notFound )
               break;

            String extStr = s.Substring( p1, p2-p1 );
            if ( !extStr.IsEmpty() )
            {
               StringList extLst;
               extStr.Break( extLst, ' ', true );
               if ( !extLst.IsEmpty() )
               {
                  FileFilter filter;

                  for ( const String& ext : extLst )
                  {
                     size_type d = ext.Find( '.' );
                     if ( d != String::notFound )
                        filter.AddExtension( ext.Substring( d ) );
                  }

                  if ( !filter.Extensions().IsEmpty() )
                  {
                     String desc = s.Substring( p, p1-p );
                     desc.Trim();
                     filter.SetDescription( desc );
                     filters.Add( filter );
                  }
               }
            }

            p = p2 + 1;
         }
      }
   }

   String      caption;
   String      initialPath;
   filter_list filters;
   String      fileExtension;
};

// ----------------------------------------------------------------------------

void FileFilter::AddExtension( const String& ext )
{
   String x = ext.Trimmed();
   if ( !x.StartsWith( '.' ) )
      if ( !x.StartsWith( '*' ) )
         x.Prepend( '*' );
   x.ToLowercase(); // case-insensitive file extensions
   if ( !m_extensions.Contains( x ) )
      m_extensions.Add( x );
}

void FileFilter::Clear()
{
   m_description.Clear();
   m_extensions.Clear();
}

String FileFilter::MakeAPIFilter() const
{
   String filter;

   if ( !m_extensions.IsEmpty() )
   {
      if ( !m_description.IsEmpty() )
      {
         filter += m_description;
         filter += " (";
      }

      for ( StringList::const_iterator i = m_extensions.Begin(); i != m_extensions.End(); ++i )
      {
         if ( i != m_extensions.Begin() )
            filter += ' '; // also legal are ';' and ','
         if ( i->StartsWith( '.' ) )
            filter += '*';
         filter += *i;
      }

      if ( !m_description.IsEmpty() )
         filter += ')';
   }

   return filter;
}

// ----------------------------------------------------------------------------

FileDialog::FileDialog()
{
   p = new FileDialogPrivate();
}

FileDialog::~FileDialog()
{
}

String FileDialog::Caption() const
{
   return p->caption;
}

void FileDialog::SetCaption( const String& caption )
{
   p->caption = caption;
}

String FileDialog::InitialPath() const
{
   return p->initialPath;
}

void FileDialog::SetInitialPath( const String& path )
{
   p->initialPath = path;
}

const FileDialog::filter_list& FileDialog::Filters() const
{
   return p->filters;
}

void FileDialog::SetFilters( const filter_list& filters )
{
   p->filters = filters;
}

FileDialog::filter_list& FileDialog::Filters() // ### DEPRECATED
{
   return p->filters;
}

String FileDialog::SelectedFileExtension() const
{
   return p->fileExtension;
}

void FileDialog::SetSelectedFileExtension( const String& ext )
{
   p->fileExtension = File::ExtractExtension( ext );
}

// ----------------------------------------------------------------------------

class OpenFileDialogPrivate
{
public:

   bool       multipleSelections = false;
   StringList fileNames;
};

OpenFileDialog::OpenFileDialog() : FileDialog()
{
   q = new OpenFileDialogPrivate();
   p->caption = "Open File";
}

OpenFileDialog::~OpenFileDialog()
{
}

void OpenFileDialog::LoadImageFilters()
{
   p->LoadFiltersFromGlobalString( "FileFormat/ReadFilters" );
}

bool OpenFileDialog::AllowsMultipleSelections() const
{
   return q->multipleSelections;
}

void OpenFileDialog::EnableMultipleSelections( bool enable )
{
   q->multipleSelections = enable;
}

// This is a file_enumeration_callback function as per APIDefs.h
static uint32 AddFileNameToList( const char16_type* fileName, void* list )
{
   reinterpret_cast<StringList*>( list )->Add( String( fileName ) );
   return api_true;
}

bool OpenFileDialog::Execute()
{
   q->fileNames.Clear();

   String apiFilters = p->MakeAPIFilters();

   String fileName;
   fileName.Reserve( 8192 );
   *fileName.Begin() = CharTraits::Null();

   if ( q->multipleSelections )
   {
      if ( (*API->Dialog->ExecuteOpenMultipleFilesDialog)( fileName.Begin(),
               AddFileNameToList, &q->fileNames,
               p->caption.c_str(),
               p->initialPath.c_str(), apiFilters.c_str(), p->fileExtension.c_str() ) == api_false )
      {
         return false;
      }
   }
   else
   {
      if ( (*API->Dialog->ExecuteOpenFileDialog)( fileName.Begin(),
               p->caption.c_str(),
               p->initialPath.c_str(), apiFilters.c_str(), p->fileExtension.c_str() ) == api_false )
      {
         return false;
      }

      fileName.ResizeToNullTerminated();
      if ( !fileName.IsEmpty() )
         q->fileNames.Add( fileName );
   }

   if ( !q->fileNames.IsEmpty() )
   {
      p->fileExtension = File::ExtractExtension( *q->fileNames );
      return true;
   }

   return false;
}

const StringList& OpenFileDialog::FileNames() const
{
   return q->fileNames;
}

String OpenFileDialog::FileName() const
{
   return q->fileNames.IsEmpty() ? String() : *q->fileNames;
}

// ----------------------------------------------------------------------------

class SaveFileDialogPrivate
{
public:

   bool   overwritePrompt = true;
   String fileName;
};

SaveFileDialog::SaveFileDialog() : FileDialog()
{
   q = new SaveFileDialogPrivate();
   p->caption = "Save File As";
}

SaveFileDialog::~SaveFileDialog()
{
}

void SaveFileDialog::LoadImageFilters()
{
   p->LoadFiltersFromGlobalString( "FileFormat/WriteFilters" );
}

bool SaveFileDialog::IsOverwritePromptEnabled() const
{
   return q->overwritePrompt;
}

void SaveFileDialog::EnableOverwritePrompt( bool enable )
{
   q->overwritePrompt = enable;
}

bool SaveFileDialog::Execute()
{
   String apiFilters = p->MakeAPIFilters();

   q->fileName.Reserve( 8192 );
   *q->fileName.Begin() = CharTraits::Null();

   if ( (*API->Dialog->ExecuteSaveFileDialog)( q->fileName.Begin(),
               p->caption.c_str(),
               p->initialPath.c_str(), apiFilters.c_str(), p->fileExtension.c_str(),
               q->overwritePrompt ) != api_false )
   {
      q->fileName.ResizeToNullTerminated();
      if ( !q->fileName.IsEmpty() )
      {
         p->fileExtension = File::ExtractExtension( q->fileName );
         return true;
      }
   }

   return false;
}

String SaveFileDialog::FileName() const
{
   return q->fileName;
}

// ----------------------------------------------------------------------------

class GetDirectoryDialogPrivate
{
public:

   String directory;
};

GetDirectoryDialog::GetDirectoryDialog() : FileDialog()
{
   q = new GetDirectoryDialogPrivate();
   p->caption = "Select Directory";
}

GetDirectoryDialog::~GetDirectoryDialog()
{
}

bool GetDirectoryDialog::Execute()
{
   q->directory.Reserve( 8192 );
   *q->directory.Begin() = CharTraits::Null();

   if ( (*API->Dialog->ExecuteGetDirectoryDialog)( q->directory.Begin(),
                        p->caption.c_str(), p->initialPath.c_str() ) != api_false )
   {
      q->directory.ResizeToNullTerminated();
      return !q->directory.IsEmpty();
   }

   return false;
}

String GetDirectoryDialog::Directory() const
{
   return q->directory;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FileDialog.cpp - Released 2017-06-17T10:55:56Z
