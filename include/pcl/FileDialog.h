//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/FileDialog.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_FileDialog_h
#define __PCL_FileDialog_h

/// \file pcl/FileDialog.h

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/File.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FileDialogPrivate;
class OpenFileDialogPrivate;
class SaveFileDialogPrivate;
class GetDirectoryDialogPrivate;

// ----------------------------------------------------------------------------

/*!
 * \class FileFilter
 * \brief A description of a file type and its associated file extensions
 *
 * %FileFilter is a simple class to describe a file type and enumerate its
 * associated file extensions. That information plays a key role in file
 * dialogs as OpenFileDialog and SaveFileDialog.
 *
 * \sa FileDialog, GetDirectoryDialog, OpenFileDialog, SaveFileDialog
 */
class PCL_CLASS FileFilter
{
public:

   /*!
    * Constructs an empty %FileFilter object.
    */
   FileFilter() = default;

   /*!
    * Constructs a %FileFilter with the specified \a description and list of
    * file \a extensions.
    */
   FileFilter( const String& description, const StringList extensions )
   {
      SetDescription( description );
      AddExtensions( extensions );
   }

   /*!
    * Constructs a %FileFilter with the specified \a description and a single
    * file \a extension.
    */
   FileFilter( const String& description, const String& extension )
   {
      SetDescription( description );
      AddExtension( extension );
   }

   /*!
    * Copy constructor.
    */
   FileFilter( const FileFilter& x ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~FileFilter()
   {
   }

   /*!
    * Returns the description of the file type represented by this %FileFilter
    * object. Example: "FITS Files".
    */
   String Description() const
   {
      return m_description;
   }

   /*!
    * Sets the \a description of the file type represented by this %FileFilter
    * object.
    */
   void SetDescription( const String& description )
   {
      m_description = description.Trimmed();
   }

   /*!
    * Returns a list of file extensions associated to the file type represented
    * by this %FileFilter object. Example: ".fit", ".fits", ".fts".
    */
   const StringList& Extensions() const
   {
      return m_extensions;
   }

   /*!
    * Adds a file \a extension associated with the file type represented by
    * this %FileFilter object.
    */
   void AddExtension( const String& extension );

   /*!
    * Adds an ordered list of file \a extensions.
    */
   void AddExtensions( const StringList& extensions )
   {
      for ( const String& extension : extensions )
         AddExtension( extension );
   }

   /*!
    * Clears the file type description and the list of file extensions.
    */
   void Clear();

private:

   String      m_description;
   StringList  m_extensions;

   String MakeAPIFilter() const;

   friend class FileDialogPrivate;
};

// ----------------------------------------------------------------------------

/*!
 * \class FileDialog
 * \brief Abstract base class of PCL file dialogs
 *
 * ### TODO: Write a detailed description for %FileDialog.
 *
 * \sa GetDirectoryDialog, OpenFileDialog, SaveFileDialog, FileFilter
 */
class PCL_CLASS FileDialog
{
public:

   /*!
    * A list of file filters describing file types and their associated file
    * extensions.
    */
   typedef Array<FileFilter>  filter_list;

   /*!
    * Constructs a %FileDialog object.
    */
   FileDialog();

   /*!
    * Destroys a %FileDialog object.
    */
   virtual ~FileDialog();

   /*!
    * Returns the caption of this file dialog.
    */
   String Caption() const;

   /*!
    * Sets a new \a caption for this file dialog. If an empty caption is
    * specified, the platform will assign a default caption depending on the
    * type of dialog: "Open File", "Save File As", and so on.
    */
   void SetCaption( const String& caption );

   /*!
    * Returns the <em>initial path</em> for this file dialog.
    *
    * If not empty, the initial directory is selected upon dialog execution. If
    * the initial path points to a file, that file is set as the initial dialog
    * selection, if appropriate.
    */
   String InitialPath() const;

   /*!
    * Sets the <em>initial path</em> for this file dialog. See InitialPath()
    * for a full description.
    */
   void SetInitialPath( const String& path );

   /*!
    * Returns a reference to the immutable list of file filters in this file
    * dialog.
    */
   const filter_list& Filters() const;

   /*!
    * Defines the list of file \a filters to be used by this file dialog.
    */
   void SetFilters( const filter_list& filters );

   /*!
    * Defines a unique file \a filter to be used by this file dialog.
    */
   void SetFilter( const FileFilter& filter )
   {
      SetFilters( filter_list() << filter );
   }

   /*!
    * Adds a set of file \a filters to the list of filters used by this file
    * dialog.
    */
   void AddFilters( const filter_list& filters );

   /*!
    * Adds a file \a filter to the list of filters used by this file dialog.
    */
   void AddFilter( const FileFilter& filter )
   {
      AddFilters( filter_list() << filter );
   }

   /*!
    * Returns a reference to the mutable list of file filters in this file
    * dialog.
    *
    * \deprecated This member function has been deprecated. It is kept just to
    * support existing code and must not be used in newly produced code. Use
    * SetFilters() instead of this function.
    */
   filter_list& Filters();

   /*!
    * Returns the <em>selected file extension</em> in this file dialog.
    *
    * If not empty, a file filter containing the specified file extension will
    * be selected upon dialog execution. After an accepted call to Execute(),
    * this property will store the extension of the first selected file.
    */
   String SelectedFileExtension() const;

   /*!
    * Sets the <em>selected file extension</em> for this file dialog.
    * See SelectedFileExtension() for a full description.
    */
   void SetSelectedFileExtension( const String& );

   /*!
    * Modal dialog execution.
    *
    * Returns true if the dialog has been accepted; false if the dialog has
    * been cancelled.
    */
   virtual bool Execute() = 0;

protected:

   AutoPointer<FileDialogPrivate> p;
};

// ----------------------------------------------------------------------------

/*!
 * \class OpenFileDialog
 * \brief A modal dialog box to select one or more existing files
 *
 * ### TODO: Write a detailed description for %FileDialog.
 *
 * \sa GetDirectoryDialog, FileDialog, SaveFileDialog, FileFilter
 */
class PCL_CLASS OpenFileDialog : public FileDialog
{
public:

   /*!
    * Constructs an %OpenFileDialog object.
    */
   OpenFileDialog();

   /*!
    * Destroys an %OpenFileDialog object.
    */
   virtual ~OpenFileDialog();

   /*!
    * Loads a set of file filters corresponding to all installed file formats
    * that are able to read image files.
    *
    * The set of file filters loaded by this function will depend on the file
    * format modules currently installed on the PixInsight platform. Typically
    * it will include formats like FITS, TIFF, JPEG, JPEG2000, and DSLR RAW.
    * In the unlikely event that no file format able to read files is installed
    * on the platform, the filter set will be empty.
    *
    * Before the set of format-specific file filters, an additional filter is
    * always included that comprises the whole set of reading-capable formats,
    * i.e. a first "All known formats" filter is always present.
    */
   void LoadImageFilters();

   /*!
    * Returns true iff this dialog accepts multiple selections.
    *
    * When multiple selections are enabled, the user can select a list of one
    * or more existing files. When this mode is disabled, only a single
    * existing file can be selected.
    *
    * \sa EnableMultipleSelections(), DisableMultipleSelections()
    */
   bool AllowsMultipleSelections() const;

   /*!
    * Enables or disables multiple file selections for this dialog.
    *
    * \sa DisableMultipleSelections(), AllowsMultipleSelections()
    */
   void EnableMultipleSelections( bool enable = true );

   /*!
    * Disables or enables multiple file selections for this dialog.
    *
    * This is a convenience member function, equivalent to
    * EnableMultipleSelections( !disable ).
    *
    * \sa DisableMultipleSelections(), AllowsMultipleSelections()
    */
   void DisableMultipleSelections( bool disable = true )
   {
      EnableMultipleSelections( !disable );
   }

   /*!
    */
   virtual bool Execute();

   /*!
    * Returns a reference to the list of selected files.
    *
    * Each element in the returned list is a full path to a selected file.
    *
    * When multiple selections are enabled, the returned list may contain one
    * or more file paths.
    */
   const StringList& FileNames() const;

   /*!
    * Returns the first selected file path.
    *
    * Use this member function to access a single file name, when multiple
    * selections are disabled, or to the first file name of a multiple
    * selection.
    */
   String FileName() const;

private:

   AutoPointer<OpenFileDialogPrivate> q;
};

// ----------------------------------------------------------------------------

/*!
 * \class SaveFileDialog
 * \brief A modal dialog box to select a single file name for output
 *
 * ### TODO: Write a detailed description for %SaveFileDialog.
 *
 * \sa GetDirectoryDialog, OpenFileDialog, FileDialog, FileFilter
 */
class PCL_CLASS SaveFileDialog : public FileDialog
{
public:

   /*!
    * Constructs a %SaveFileDialog object
    */
   SaveFileDialog();

   /*!
    * Destroys a %SaveFileDialog object
    */
   virtual ~SaveFileDialog();

   /*!
    * Loads a set of file filters corresponding to all installed file formats
    * that are able to write image files.
    *
    * The set of file filters loaded by this function will depend on the file
    * format modules currently installed on the PixInsight platform. Typically
    * it will include formats like FITS, TIFF, JPEG, and JPEG2000. In the
    * (improbable) event that no file format able to write files is installed
    * on the platform, the filter set will be empty.
    */
   void LoadImageFilters();

   /*!
    * Returns true iff <em>overwrite prompts</em> are enabled for this dialog.
    *
    * When overwrite prompts are enabled, the dialog will request confirmation
    * if an existing file is selected.
    *
    * \sa EnableOverwritePrompt(), DisableOverwritePrompt()
    */
   bool IsOverwritePromptEnabled() const;

   /*!
    * Enables or disables overwrite prompts for this dialog.
    *
    * \sa DisableOverwritePrompt(), IsOverwritePromptEnabled()
    */
   void EnableOverwritePrompt( bool enable = true );

   /*!
    * Disables or enables overwrite prompts for this dialog.
    *
    * This is a convenience member function, equivalent to
    * EnableOverwritePrompt( !disable )
    *
    * \sa EnableOverwritePrompt(), IsOverwritePromptEnabled()
    */
   void DisableOverwritePrompt( bool disable = true )
   {
      EnableOverwritePrompt( !disable );
   }

   /*!
    */
   virtual bool Execute();

   /*!
    * Returns the selected file path. A full file path is always returned.
    */
   String FileName() const;

private:

   AutoPointer<SaveFileDialogPrivate> q;
};

// ----------------------------------------------------------------------------

/*!
 * \class GetDirectoryDialog
 * \brief A modal dialog box to select an existing directory
 *
 * ### TODO: Write a detailed description for %GetDirectoryDialog.
 *
 * \sa FileDialog, OpenFileDialog, SaveFileDialog
 */
class PCL_CLASS GetDirectoryDialog : public FileDialog
{
public:

   /*!
    * Constructs a %GetDirectoryDialog object
    */
   GetDirectoryDialog();

   /*!
    * Destroys a %GetDirectoryDialog object
    */
   virtual ~GetDirectoryDialog();

   /*!
    */
   virtual bool Execute();

   /*!
    * Returns the selected directory path. A full path is always returned.
    */
   String Directory() const;

private:

   AutoPointer<GetDirectoryDialogPrivate> q;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FileDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/FileDialog.h - Released 2017-08-01T14:23:31Z
