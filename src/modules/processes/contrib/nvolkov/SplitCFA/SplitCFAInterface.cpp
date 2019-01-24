//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0191
// ----------------------------------------------------------------------------
// SplitCFAInterface.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2018 Nikolay Volkov
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#include "SplitCFAInterface.h"
#include "SplitCFAProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/StdStatus.h>

#define IMAGELIST_MINHEIGHT( fnt )  RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

SplitCFAInterface* TheSplitCFAInterface = 0;

SplitCFAInterface::SplitCFAInterface() :
ProcessInterface(), m_instance( TheSplitCFAProcess ), GUI( 0 )
{
   TheSplitCFAInterface = this;
   DisableAutoSaveGeometry();
}

SplitCFAInterface::~SplitCFAInterface()
{
   if ( GUI != 0 ) delete GUI, GUI = 0;
}

IsoString SplitCFAInterface::Id() const
{
   return "SplitCFA";
}

MetaProcess* SplitCFAInterface::Process() const
{
   return TheSplitCFAProcess;
}

InterfaceFeatures SplitCFAInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;// | SplitCFAInterfaceFeature::RealTimeButton;
}

void SplitCFAInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}
// ----------------------------------------------------------------------------

void SplitCFAInterface::ResetInstance()
{
   SplitCFAInstance defaultInstance( TheSplitCFAProcess );
   ImportProcess( defaultInstance );
}

bool SplitCFAInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "SplitCFA" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheSplitCFAProcess;
}

ProcessImplementation* SplitCFAInterface::NewProcess() const
{
   return new SplitCFAInstance( m_instance );
}

bool SplitCFAInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const SplitCFAInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an SplitCFA instance.";
   return false;
}

bool SplitCFAInterface::RequiresInstanceValidation() const
{
   return true;
}

bool SplitCFAInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

//-------------------------------------------------------------------------

static
void FileShow( const String& path )   // load ( if not loaded before ) and bring to front the ImageWindow
{
   #if debug
   Console().WriteLn( "FileShow():" + path );
   #endif
   if ( ImageWindow::WindowByFilePath( path ).IsNull() )
   {
      Array<ImageWindow> windows = ImageWindow::Open( path, IsoString()/*id*/, "raw cfa"/*formatHints*/ );
      for ( ImageWindow& window : windows )
         window.Show();
   }
   else
      ImageWindow::WindowByFilePath( path ).BringToFront();
}

void SplitCFAInterface::UpdateControls()
{
   GUI->OutputTree_CheckBox.Disable(m_instance.p_outputDir.IsEmpty());
   GUI->OutputTree_CheckBox.SetChecked( m_instance.p_outputTree );
   GUI->OutputSubDirCFA_CheckBox.SetChecked( m_instance.p_outputSubDirCFA );
   GUI->OutputDir_Edit.SetText( m_instance.p_outputDir );
   GUI->Postfix_Edit.SetText( m_instance.p_postfix );
   GUI->Prefix_Edit.SetText( m_instance.p_prefix );

   GUI->Overwrite_CheckBox.SetChecked( m_instance.p_overwrite );

   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
}

void SplitCFAInterface::UpdateTargetImagesList()
{
   GUI->Files_TreeBox.Clear();
   GUI->Files_TreeBox.DisableUpdates();
   GUI->Files_TreeBox.DisableHeaderSorting();

   size_type n = m_instance.p_targetFrames.Length();

   GUI->Files_TreeBox.ShowHeader( n > 0 );

   Bitmap enabledIcon( ScaledResource( ":/browser/enabled.png" ) );
   Bitmap disabledIcon( ScaledResource( ":/browser/disabled.png" ) );

   for ( size_type i = 0; i < n; ++i )
   {
      String path = m_instance.p_targetFrames[i].path;

      TreeBox::Node* node = new TreeBox::Node( GUI->Files_TreeBox ); // add new item in Files_TreeBox

      node->SetIcon( 0, m_instance.p_targetFrames[i].enabled ? enabledIcon : disabledIcon );
      node->SetToolTip( 0, "Double Click to check/uncheck" );
      node->SetAlignment( 0, TextAlign::Left );

      node->SetText( 1, path );       // save full patch to hiden column for file idintification

      if( GUI->FullPaths_CheckBox.IsChecked())
         node->SetText( 2, path );   // show file full patch
      else
         node->SetText( 2, File::ExtractName( path ) );   //show only file name

      node->SetText( 3, File::ExtractExtension( path ) );
   }

   GUI->Files_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->Files_TreeBox.AdjustColumnWidthToContents( 2 );
   GUI->Files_TreeBox.AdjustColumnWidthToContents( 3 );
   GUI->Files_TreeBox.EnableHeaderSorting();
   GUI->Files_TreeBox.EnableUpdates();
}

void SplitCFAInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->Files_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->Files_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->Clear_PushButton.Enable( hasItems );

   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
}

//-------------------------------------------------------------------------------------

void SplitCFAInterface::SelectDir() //Select Output Directory
{
   GetDirectoryDialog d;
   d.SetCaption( "SplitCFA: Select Output Directory" );
   if ( d.Execute() )
   {
      GUI->OutputDir_Edit.SetText( m_instance.p_outputDir = d.Directory() );
      GUI->OutputTree_CheckBox.Disable(m_instance.p_outputDir.IsEmpty());
   }
}

void SplitCFAInterface::__TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_targetFrames.Length() )
      throw Error( "SplitCFAInterface: *Warning* Corrupted interface structures" );

   // ### If there's something else that depends on which image is selected in the list, do it here.
}

void SplitCFAInterface::__TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int i = sender.ChildIndex( &node );
   if ( i < 0 || size_type( i ) >= m_instance.p_targetFrames.Length() )
      throw Error( "SplitCFAInterface: *Warning* Corrupted interface structures" );

   // indexes in TreeBox and image_list is not equal, because TreeBox sorting is enabled.
   // so i find image index by image full patch
   SplitCFAInstance::ImageItem& item = m_instance.p_targetFrames[FileInList(GUI->Files_TreeBox[i]->Text(1))];

   switch ( col )
   {
   case 0:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
      node.SetIcon( 0, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
      break;
   case 2:
      // Activate the item's path: open the image.
      FileShow(item.path);
      break;
   }
}

void SplitCFAInterface::__TargetImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}


int SplitCFAInterface::FileInList(const String& path)
{
   // indexes in TreeBox and image_list is not equal, because TreeBox sorting is enabled.
   // so i find image index by image full patch
   int j = int(m_instance.p_targetFrames.Length()) -1;
   while ( j >= 0 )
   {
      if ( m_instance.p_targetFrames.At( j )->path == path  )
         break; //file found
      j--;
   }
   return j; //-1 = not found
}

void SplitCFAInterface::AddFile(const String& path, const String& folder )
{
   if ( FileInList(path) < 0 ) // new file
   {
      Console().WriteLn( "AddFile: " + path);

      ProcessEvents();
      // insert the file to list
      String f;
      if(!folder.IsEmpty())
      {
         f = File::ExtractDirectory(path);
         #if debug
            Console().WriteLn( "intut TopFolder:" + folder );
            Console().WriteLn( "input FileFolder:" + f );
            ProcessEvents();
         #endif
         f.DeleteLeft(folder.Length());
      }

      #if debug
         Console().WriteLn( "output subfolder:" + f );
         ProcessEvents();
      #endif

      m_instance.p_targetFrames.Add( SplitCFAInstance::ImageItem( path, f ) );
   }
}

void SplitCFAInterface::AddFiles()
{
   OpenFileDialog d;
   d.LoadImageFilters();
   d.EnableMultipleSelections();
   d.SetCaption( "SplitCFA: Select Target Frames" );
   if ( d.Execute() )
   {
      for ( size_type i = 0; i < d.FileNames().Length(); i++ )
         AddFile(d.FileNames()[i]);
      UpdateTargetImagesList();
   }
   Console().WriteLn( "Total images:" + String(m_instance.p_targetFrames.Length()));
}

/*
 * Secure recursive directory search routine.
 */
static
void SearchDirectory_Recursive( StringList& foundFiles, const String& whereToFind, const String& baseDir )
{
   if ( whereToFind.Contains( ".." ) )
      throw Error( "Attempt to climb up the filesystem in file search operation: " + whereToFind );
   if ( !whereToFind.StartsWith( baseDir ) )
      throw Error( "Attempt to walk on a parallel directory tree in file search operation: '" + whereToFind
                 + "'; expected to be rooted at '" + baseDir + "'" );
   if ( !File::DirectoryExists( whereToFind ) )
      throw Error( "Nonexistent directory in file search operation: " + whereToFind );

   String currentDir = whereToFind;
   if ( !currentDir.EndsWith( '/' ) )
      currentDir += '/';

   StringList directories;
   FindFileInfo info;
   for ( File::Find f( currentDir + "*" ); f.NextItem( info ); )
      if ( info.IsDirectory() )
      {
         if ( info.name != "." && info.name != ".." )
            directories.Add( info.name );
      }
      else
      {
         String ext = File::ExtractExtension( info.name ).Lowercase();
         if ( ext == ".cr2" || ext == ".xisf" || ext == ".fit" || ext == ".fits" || ext == ".fts" || ext == ".nef" || ext == ".dng" )
            foundFiles.Add( currentDir + info.name );
      }

   for ( StringList::const_iterator i = directories.Begin(); i != directories.End(); ++i )
      SearchDirectory_Recursive( foundFiles, currentDir + *i, baseDir );
}

static
StringList SearchDirectory( const String& whereToFind )
{
   try
   {
      StringList foundFiles;
      SearchDirectory_Recursive( foundFiles, whereToFind, whereToFind );
      return foundFiles;
   }
   ERROR_HANDLER
   return StringList();
}

void SplitCFAInterface::SearchFile( const String& path, const String& folder ) // Recursiv Search file in sub-folders
{
   StringList files = SearchDirectory( path );
   for ( StringList::const_iterator i = files.Begin(); i != files.End(); ++i )
      AddFile( *i, folder );
}

void SplitCFAInterface::AddFolders()
{
   GetDirectoryDialog d;
   d.SetCaption( "SplitCFA: Select Directory" );
   if ( d.Execute() )
   {
      SearchFile(d.Directory(), File::ExtractDirectory(d.Directory()));
      UpdateTargetImagesList();
   }
   Console().WriteLn( "Total images:" + String(m_instance.p_targetFrames.Length()));
}

void SplitCFAInterface::RemoveSelectedFiles()
{
   for ( int i = GUI->Files_TreeBox.NumberOfChildren(); --i >= 0; )
   {
      if ( GUI->Files_TreeBox.Child( i )->IsSelected() )
      {
         String path = GUI->Files_TreeBox.Child( i )->Text(1);
         int fileNumber = FileInList( path );
         m_instance.p_targetFrames.Remove( m_instance.p_targetFrames.At( fileNumber ) );
      }
   }
   UpdateTargetImagesList();
}

void SplitCFAInterface::__TargetImages_BottonClick( Button& sender, bool checked )
{
   if ( sender == GUI->AddFolder_PushButton )
      AddFolders(); // Recursiv Search file in sub-folders

   else if ( sender == GUI->AddFiles_PushButton )
      AddFiles();

   else if ( sender == GUI->SelectAll_PushButton )
      GUI->Files_TreeBox.SelectAllNodes();

   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->Files_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->Files_TreeBox[i]->Select( !GUI->Files_TreeBox[i]->IsSelected() );
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->Files_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         if ( GUI->Files_TreeBox[i]->IsSelected() )
         {
            int j = FileInList(GUI->Files_TreeBox[i]->Text(1));
            bool enabled = !m_instance.p_targetFrames[j].enabled;
            m_instance.p_targetFrames[j].enabled = enabled;
            GUI->Files_TreeBox[i]->SetIcon( 0, Bitmap( ScaledResource( enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
         }
      }
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      RemoveSelectedFiles();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      m_instance.p_targetFrames.Clear();
      GUI->Files_TreeBox.Clear();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateTargetImagesList();
   }
   UpdateImageSelectionButtons();
}

void SplitCFAInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
   {
      GUI->Files_TreeBox.SetFixedHeight();
   }
   else
   {
      GUI->Files_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->Files_TreeBox.SetMaxHeight( int_max );
   }
}

void SplitCFAInterface::__MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( sender == GUI->OutputDir_Edit )
      SelectDir();
}

void SplitCFAInterface::__EditCompleted( Edit& sender )
{
   if ( !sender.IsModified() ) return;
   sender.SetModified( false );

   #if debug
   Console().WriteLn( "__EditCompleted()" );
   #endif

   String text = sender.Text().Trimmed();

   if ( sender == GUI->Prefix_Edit )
   m_instance.p_prefix = text;
   else if ( sender == GUI->Postfix_Edit )
      m_instance.p_postfix = text;
   else if ( sender == GUI->OutputDir_Edit )
   {
      if ( File::DirectoryExists( text ) )
      {
         m_instance.p_outputDir = text;
         GUI->OutputTree_CheckBox.Disable(m_instance.p_outputDir.IsEmpty());
      }
      else
         text = m_instance.p_outputDir;
   }

   sender.SetText( text );
}

void SplitCFAInterface::__Button_Click( Button& sender, bool checked )
{
   #if debug
   Console().WriteLn( "__Button_Click():" + sender.Text() );
   Console().Flush();
   #endif

   if ( sender == GUI->OutputDir_SelectButton )
      SelectDir();

   else if ( sender == GUI->OutputDir_ClearButton )
   {
      GUI->OutputDir_Edit.SetText( m_instance.p_outputDir = TheOutputDirParameter->DefaultValue() );
      GUI->OutputTree_CheckBox.Disable(m_instance.p_outputDir.IsEmpty());
   }

   else if ( sender == GUI->Overwrite_CheckBox )
      m_instance.p_overwrite = checked;
   else if ( sender == GUI->OutputTree_CheckBox )
      m_instance.p_outputTree = checked;
   else if ( sender == GUI->OutputSubDirCFA_CheckBox )
      m_instance.p_outputSubDirCFA = checked;
   GUI->OutputTree_CheckBox.Disable(m_instance.p_outputDir.IsEmpty());
}

// ----------------------------------------------------------------------------

SplitCFAInterface::GUIData::GUIData( SplitCFAInterface& w )
{
   int editWidth1 = w.Font().Width( String( 'M', 6 ) );

   //

   Files_TreeBox.SetMinSize( w.LogicalPixelsToPhysical( 400 ), IMAGELIST_MINHEIGHT( w.Font() ) );
   Files_TreeBox.SetNumberOfColumns( 4 ); // plus 1 hidden column for GUI Stretch
   Files_TreeBox.SetHeaderText( 0, "?" );
   Files_TreeBox.SetHeaderText( 1, "Patch" ); //hiden column
   Files_TreeBox.HideColumn(1);
   Files_TreeBox.SetHeaderText( 2, "File" );
   Files_TreeBox.SetHeaderText( 3, "Ext" );
   Files_TreeBox.ShowHeader();
   Files_TreeBox.EnableHeaderSorting();
   Files_TreeBox.Sort( 2, true );
   //Files_TreeBox.SetHeaderText( 4, "" ); // hiden column for Stretch

   Files_TreeBox.EnableMultipleSelections();
   Files_TreeBox.DisableRootDecoration();
   Files_TreeBox.EnableAlternateRowColor();
   Files_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&SplitCFAInterface::__TargetImages_CurrentNodeUpdated, w );
   Files_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&SplitCFAInterface::__TargetImages_NodeActivated, w );
   Files_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&SplitCFAInterface::__TargetImages_NodeSelectionUpdated, w );

   AddFolder_PushButton.SetText( "Add Folder" );
   AddFolder_PushButton.SetToolTip( "<p>Search subfolders recursively and add existing image files to the list of target frames.</p>" );
   AddFolder_PushButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target frames.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target frames.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target frames.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target frames.</p>"
      "<p>Disabled target frames will be ignored during the SplitCFA process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target frames.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of target frames.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target frame files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&SplitCFAInterface::__TargetImages_BottonClick, w );

   TargetImages_Control.SetSizer( TargetImages_Sizer );

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SplitCFAInterface::__ToggleSection, w );

   TargetButtons_Sizer.SetSpacing( 4 );
   TargetButtons_Sizer.Add( AddFolder_PushButton );
   TargetButtons_Sizer.Add( AddFiles_PushButton );
   TargetButtons_Sizer.Add( SelectAll_PushButton );
   TargetButtons_Sizer.Add( InvertSelection_PushButton );
   TargetButtons_Sizer.Add( ToggleSelected_PushButton );
   TargetButtons_Sizer.Add( RemoveSelected_PushButton );
   TargetButtons_Sizer.Add( Clear_PushButton );
   TargetButtons_Sizer.Add( FullPaths_CheckBox );
   TargetButtons_Sizer.AddStretch();

   TargetImages_Sizer.SetSpacing( 4 );
   TargetImages_Sizer.Add( Files_TreeBox, 100 );
   TargetImages_Sizer.Add( TargetButtons_Sizer );

   //

   OutputTree_CheckBox.SetText( "Tree" );
   OutputTree_CheckBox.SetToolTip( "<p>If checked, SplitCFA will create a new directory tree structure to "
      "replicate the selected input folders. All subfolders will be created under the output directory, "
      "and output files will populate them at the same relative locations as their input counterparts.</p>"
      "<p>If unchecked, SplitCFA will write all output files on a single dirrectory.</p>");
   OutputTree_CheckBox.OnClick( (Button::click_event_handler)&SplitCFAInterface::__Button_Click, w );

   OutputSubDirCFA_CheckBox.SetText( "CFA sub-folder" );
   OutputSubDirCFA_CheckBox.SetToolTip( "<p>If checked, SplitCFA will create four sub-folders for each "
   "CFA channel, namely CFA0, CFA1, CFA2 and CFA3. Output files will be written according to this scheme.</p>");
   OutputSubDirCFA_CheckBox.OnClick( (Button::click_event_handler)&SplitCFAInterface::__Button_Click, w );

   const char* ToolTipOutputDir = "<p>This is the directory (or folder) where all output files "
      "will be written.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their "
      "corresponding target files. In this case, make sure that source directories are writable, or the "
      "SplitCFA process will fail.</p>";

   OutputDir_Edit.SetToolTip( ToolTipOutputDir );
   OutputDir_Edit.OnMouseDoubleClick( (Control::mouse_event_handler)&SplitCFAInterface::__MouseDoubleClick, w );
   OutputDir_Edit.OnEditCompleted( (Edit::edit_event_handler)&SplitCFAInterface::__EditCompleted, w );

   OutputDir_SelectButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   OutputDir_SelectButton.SetScaledFixedSize( 19, 19 );
   OutputDir_SelectButton.SetToolTip( "<p>Select output directory</p>" );
   OutputDir_SelectButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__Button_Click, w );

   OutputDir_ClearButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/clear.png" ) ) );
   OutputDir_ClearButton.SetScaledFixedSize( 19, 19 );
   OutputDir_ClearButton.SetToolTip( "<p>Reset output directory</p>" );
   OutputDir_ClearButton.OnClick( (Button::click_event_handler)&SplitCFAInterface::__Button_Click, w );

   OutputDir_Sizer.SetSpacing( 4 );
   OutputDir_Sizer.Add( OutputTree_CheckBox );
   OutputDir_Sizer.Add( OutputDir_Edit, 100 );
   OutputDir_Sizer.Add( OutputDir_SelectButton );
   OutputDir_Sizer.Add( OutputDir_ClearButton );

   const char* ToolTipPrefix =
      "<p>This is a prefix that will be prepended to the file name of each target image.</p>";
   Prefix_Label.SetText( "Prefix:" );
   Prefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Prefix_Label.SetToolTip( ToolTipPrefix );
   Prefix_Edit.SetFixedWidth( editWidth1 );
   Prefix_Edit.SetToolTip( ToolTipPrefix );
   Prefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&SplitCFAInterface::__EditCompleted, w );

   const char* ToolTipPostfix =
      "<p>This is a postfix that will be appended to the file name of each targer image.</p>";
   Postfix_Label.SetText( "Postfix:" );
   Postfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Postfix_Label.SetToolTip( ToolTipPostfix );
   Postfix_Edit.SetFixedWidth( editWidth1 );
   Postfix_Edit.SetToolTip( ToolTipPostfix );
   Postfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&SplitCFAInterface::__EditCompleted, w );

   Overwrite_CheckBox.SetText( "Overwrite" );
   Overwrite_CheckBox.SetToolTip( "<p>If this option is selected, SplitCFA will overwrite existing files with "
      "the same names as generated output files. This can be dangerous because the original contents of "
      "overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   Overwrite_CheckBox.OnClick( (Button::click_event_handler)&SplitCFAInterface::__Button_Click, w );

   OutputChunks_Sizer.Add( OutputSubDirCFA_CheckBox );
   OutputChunks_Sizer.AddSpacing( 20 );
   OutputChunks_Sizer.Add( Overwrite_CheckBox );
   OutputChunks_Sizer.AddSpacing( 20 );
   OutputChunks_Sizer.Add( Prefix_Label );
   OutputChunks_Sizer.AddSpacing( 4 );
   OutputChunks_Sizer.Add( Prefix_Edit );
   OutputChunks_Sizer.AddSpacing( 20 );
   OutputChunks_Sizer.Add( Postfix_Label );
   OutputChunks_Sizer.AddSpacing( 4 );
   OutputChunks_Sizer.Add( Postfix_Edit );
   OutputChunks_Sizer.AddStretch();

   //

   Output_Sizer.SetSpacing( 4 );
   Output_Sizer.Add( OutputDir_Sizer );
   Output_Sizer.Add( OutputChunks_Sizer );

   Output_Control.SetSizer( Output_Sizer );
   Output_SectionBar.SetTitle( "Output" );
   Output_SectionBar.SetSection( Output_Control );
   Output_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SplitCFAInterface::__ToggleSection, w );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( Output_SectionBar );
   Global_Sizer.Add( Output_Control );

   Output_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SplitCFAInterface.cpp - Released 2019-01-21T12:06:42Z
