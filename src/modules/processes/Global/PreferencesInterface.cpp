//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.06.0288
// ----------------------------------------------------------------------------
// PreferencesInterface.cpp - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "PreferencesInterface.h"
#include "PreferencesParameters.h" // for DefaultFontInfo
#include "PreferencesProcess.h"

#include <pcl/CheckBox.h>
#include <pcl/ColorDialog.h>
#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

typedef IndirectArray<GlobalItemControl>  global_item_ref_list;
static global_item_ref_list globalItemControls;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

GlobalItemControl::GlobalItemControl() :
   Control()
{
   Restyle();

   sizer.SetMargin( 4 );
   sizer.SetSpacing( 2 );
   SetSizer( sizer );

   RGBA color = BackgroundColor();
   int r = Red( color );
   int g = Green( color );
   int b = Blue( color );
   if ( r >= 16 ) r -= 16; else r += 16;
   if ( g >= 16 ) g -= 16; else g += 16;
   if ( b >= 16 ) b -= 16; else b += 16;
   SetBackgroundColor( RGBAColor( r, g, b ) );

   globalItemControls.Add( this );
}

// ----------------------------------------------------------------------------

GlobalFlagControl::GlobalFlagControl() :
   GlobalItemControl(),
   item( 0 ),
   enableControl( 0 ),
   disableControl( 0 )
{
   checkBox.OnClick( (Button::click_event_handler)&GlobalFlagControl::__Click, *this );
   checkBox.SetButtonColor( BackgroundColor() );

   sizer.Add( checkBox );
}

void GlobalFlagControl::Synchronize()
{
   if ( item != 0 )
   {
      checkBox.SetChecked( *item );
      if ( enableControl != 0 )
         enableControl->Enable( *item );
      if ( disableControl != 0 )
         disableControl->Disable( *item );
   }
}

void GlobalFlagControl::__Click( Button&/*sender*/, bool checked )
{
   if ( item != 0 )
   {
      *item = checked;
      if ( enableControl != 0 )
         enableControl->Enable( *item );
      if ( disableControl != 0 )
         disableControl->Disable( *item );
   }
}

// ----------------------------------------------------------------------------

GlobalIntegerControl::GlobalIntegerControl() :
   GlobalItemControl(),
   item( 0 )
{
   spinBox.OnValueUpdated( (SpinBox::value_event_handler)&GlobalIntegerControl::__ValueUpdated, *this );
   spinSizer.Add( spinBox );
   spinSizer.AddStretch();

   sizer.Add( label );
   sizer.Add( spinSizer );
}

void GlobalIntegerControl::Synchronize()
{
   if ( item != 0 )
      spinBox.SetValue( *item );
}

void GlobalIntegerControl::__ValueUpdated( SpinBox& /*sender*/, int value )
{
   if ( item != 0 )
      *item = int32( value );
}

// ----------------------------------------------------------------------------

GlobalSetControl::GlobalSetControl() : GlobalItemControl(), item( 0 ), minValue( 0 )
{
   comboBox.OnItemSelected( (ComboBox::item_event_handler)&GlobalSetControl::__ItemSelected, *this );
   comboSizer.Add( comboBox );
   comboSizer.AddStretch();

   sizer.Add( label );
   sizer.Add( comboSizer );
}

void GlobalSetControl::Synchronize()
{
   if ( item != 0 )
      comboBox.SetCurrentItem( *item - minValue );
}

void GlobalSetControl::__ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   if ( item != 0 )
      *item = int32( itemIndex + minValue );
}

// ----------------------------------------------------------------------------

GlobalRealControl::GlobalRealControl() : GlobalItemControl(), item( 0 )
{
   numericEdit.label.Hide();
   numericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&GlobalRealControl::__ValueUpdated, *this );
   numericSizer.Add( numericEdit );
   numericSizer.AddStretch();

   sizer.Add( label );
   sizer.Add( numericSizer );
}

void GlobalRealControl::Synchronize()
{
   if ( item != 0 )
      numericEdit.SetValue( *item );
}

void GlobalRealControl::__ValueUpdated( NumericEdit& /*sender*/, double value )
{
   if ( item != 0 )
      *item = value;
}

// ----------------------------------------------------------------------------

GlobalStringControl::GlobalStringControl() : GlobalItemControl(), item( 0 )
{
   edit.OnEditCompleted( (Edit::edit_event_handler)&GlobalStringControl::__EditCompleted, *this );

   sizer.Add( label );
   sizer.Add( edit );
}

void GlobalStringControl::Synchronize()
{
   if ( item != 0 )
      edit.SetText( *item );
}

String GlobalStringControl::GetText( const String& s )
{
   return s;
}

void GlobalStringControl::__EditCompleted( Edit& sender )
{
   if ( item != 0 )
   {
      try
      {
         String s = sender.Text();
         s.Trim();
         sender.SetText( *item = GetText( s ) );
         return;
      }

      ERROR_HANDLER
      sender.SetText( *item );
      sender.Focus();
   }
}

// ----------------------------------------------------------------------------

GlobalDirectoryControl::GlobalDirectoryControl() : GlobalStringControl()
{
   selectDirButton.SetText( "Select directory..." );
   selectDirButton.OnClick( (Button::click_event_handler)&GlobalDirectoryControl::__SelectDir, *this );
   sizer.Add( selectDirButton );
}

String GlobalDirectoryControl::GetText( const String& s )
{
   String dir =
#ifdef __PCL_WINDOWS
      File::WindowsPathToUnix( s );
#else
      s;
#endif
   if ( !File::DirectoryExists( dir ) )
      throw Error( "The specified directory does not exist: " + dir );
   return dir;
}

void GlobalDirectoryControl::__SelectDir( Button& /*sender*/, bool /*checked*/ )
{
   if ( item != 0 )
   {
      GetDirectoryDialog dlg;
      dlg.SetCaption( "Select Directory" );
      dlg.SetInitialPath( *item );

      if ( dlg.Execute() )
      {
         *item = dlg.Directory();
         Synchronize();
      }
   }
}

// ----------------------------------------------------------------------------

GlobalFileControl::GlobalFileControl() : GlobalStringControl()
{
   selectFileButton.SetText( "Select file..." );
   selectFileButton.OnClick( (Button::click_event_handler)&GlobalFileControl::__SelectFile, *this );
   sizer.Add( selectFileButton );
}

String GlobalFileControl::GetText( const String& s )
{
   String path =
#ifdef __PCL_WINDOWS
      File::WindowsPathToUnix( s );
#else
      s;
#endif
/* ### Allow specifying nonexistent files
   ### Consider controlling this behavior through a flag passed to GlobalFileControl's ctor.
   if ( !File::FileExists( path ) )
      throw Error( "The specified file does not exist: " + path );
*/
   return path;
}

void GlobalFileControl::__SelectFile( Button& /*sender*/, bool /*checked*/ )
{
   if ( item != 0 )
   {
      OpenFileDialog dlg;
      dlg.SetCaption( "Select File" );
      dlg.SetInitialPath( *item );

      if ( dlg.Execute() )
      {
         *item = dlg.FileName();
         Synchronize();
      }
   }
}

// ----------------------------------------------------------------------------

GlobalFileExtensionControl::GlobalFileExtensionControl() : GlobalStringControl()
{
}

String GlobalFileExtensionControl::GetText( const String& s )
{
   if ( !s.IsEmpty() && !s.StartsWith( '.' ) )
      return '.' + s;
   return s;
}

// ----------------------------------------------------------------------------

GlobalIdentifierControl::GlobalIdentifierControl() : GlobalStringControl()
{
}

String GlobalIdentifierControl::GetText( const String& s )
{
   if ( !s.IsValidIdentifier() )
      throw Error( "Invalid identifier: " + s );
   return s;
}

// ----------------------------------------------------------------------------

GlobalColorControl::GlobalColorControl() : GlobalItemControl(), item( 0 )
{
   colorComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&GlobalColorControl::__ColorSelected, *this );

   colorSample.SetScaledMinWidth( 30 );
   colorSample.SetCursor( StdCursor::UpArrow );
   colorSample.OnPaint( (Control::paint_event_handler)&GlobalColorControl::__ColorSample_Paint, *this );
   colorSample.OnMouseRelease( (Control::mouse_button_event_handler)&GlobalColorControl::__ColorSample_MouseRelease, *this );

   colorSizer.SetSpacing( 4 );
   colorSizer.Add( colorComboBox, 100 );
   colorSizer.Add( colorSample );

   sizer.Add( label );
   sizer.Add( colorSizer );
}

void GlobalColorControl::Synchronize()
{
   if ( item != 0 )
   {
      colorComboBox.SetCurrentColor( *item );
      colorSample.Update();
   }
}

void GlobalColorControl::__ColorSelected( ColorComboBox& /*sender*/, RGBA color )
{
   if ( item != 0 )
   {
      *item = uint32( color );
      colorSample.Update();
   }
}

void GlobalColorControl::__ColorSample_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   if ( item != 0 )
   {
      Graphics g( sender );
      g.SetBrush( *item );
      g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
      g.DrawRect( sender.BoundsRect() );
   }
}

void GlobalColorControl::__ColorSample_MouseRelease(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( item != 0 && sender.IsUnderMouse() )
   {
      SimpleColorDialog dlg( item );
      if ( dlg.Execute() == StdDialogCode::Ok )
      {
         colorComboBox.SetCurrentColor( *item );
         colorSample.Update();
      }
   }
}

// ----------------------------------------------------------------------------

GlobalFontControl::GlobalFontControl() : GlobalItemControl(), item( 0 ), itemSize( 0 ), itemSizeAuto( 0 )
{
   fontComboBox.OnFontSelected( (FontComboBox::font_event_handler)&GlobalFontControl::__FontSelected, *this );

   fontSample.SetText( "The quick brown fox jumps over the lazy dog 0123456789" );
   fontSample.SetScaledMinWidth( 350 );

   sizeLabel.SetText( "Font size (pt):" );
   sizeLabel.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   sizeSpinBox.SetRange( 4, 16 );
   sizeSpinBox.OnValueUpdated( (SpinBox::value_event_handler)&GlobalFontControl::__SizeUpdated, *this );

   sizeAutoCheckBox.SetText( "Automatic" );
   sizeAutoCheckBox.OnClick( (Button::click_event_handler)&GlobalFontControl::__AutoSizeClick, *this );
   sizeAutoCheckBox.SetButtonColor( BackgroundColor() );
   sizeAutoCheckBox.SetToolTip( "<p>Enable automatic font size selection.</p>"
         "<p>If this option is enabled, the actual font size used will be computed dynamically as a "
         "function of the current display resolution.</p>" );

   sizeSizer.SetSpacing( 4 );
   sizeSizer.Add( sizeLabel );
   sizeSizer.Add( sizeSpinBox );
   sizeSizer.AddSpacing( 12 );
   sizeSizer.Add( sizeAutoCheckBox );
   sizeSizer.AddStretch();

   sizer.Add( label );
   sizer.Add( fontComboBox );
   sizer.Add( fontSample );
   sizer.Add( sizeSizer );
}

void GlobalFontControl::Synchronize()
{
   if ( item != 0 )
   {
      fontComboBox.SetCurrentFont( *item );
      if ( item->IsEmpty() )
         *item = fontComboBox.CurrentFontFace();

      if ( itemSize != 0 && itemSizeAuto != 0 )
      {
         sizeAutoCheckBox.SetChecked( *itemSizeAuto );
         sizeAutoCheckBox.Enable();
         sizeAutoCheckBox.Show();

         if ( *itemSizeAuto )
         {
            *itemSize = 9;
            sizeLabel.Disable();
            sizeSpinBox.Disable();
         }
         else
         {
            sizeLabel.Enable();
            sizeSpinBox.Enable();
         }
      }
      else
      {
         sizeAutoCheckBox.Disable();
         sizeAutoCheckBox.Hide();
      }

      if ( itemSize != 0 )
         sizeSpinBox.SetValue( *itemSize );
      else
      {
         sizeLabel.Disable();
         sizeSpinBox.Disable();
      }

      fontSample.SetFont( pcl::Font( *item, (itemSize != 0) ? *itemSize : 9) );
   }
}

void GlobalFontControl::__FontSelected( FontComboBox& /*sender*/, const String& fontFace )
{
   if ( item != 0 )
   {
      *item = fontFace;
      Synchronize();
   }
}

void GlobalFontControl::__SizeUpdated( SpinBox& /*sender*/, int value )
{
   if ( itemSize != 0 )
   {
      *itemSize = value;
      Synchronize();
   }
}

void GlobalFontControl::__AutoSizeClick( Button& /*sender*/, bool checked )
{
   if ( itemSizeAuto != 0 )
   {
      *itemSizeAuto = checked;
      Synchronize();
   }
}

// ----------------------------------------------------------------------------

GlobalDirectoryListControl::GlobalDirectoryListControl() : GlobalItemControl(), item( 0 )
{
   directoriesTreeBox.SetNumberOfColumns( 1 );
   directoriesTreeBox.HideHeader();
   directoriesTreeBox.DisableMultipleSelections();
   directoriesTreeBox.DisableRootDecoration();
   directoriesTreeBox.EnableAlternateRowColor();
   directoriesTreeBox.SetScaledMinHeight( 80 );
   directoriesTreeBox.OnNodeActivated( (TreeBox::node_event_handler)&GlobalDirectoryListControl::__NodeActivated, *this );

   addPushButton.SetText( "Add" );
   addPushButton.SetToolTip( "<p>Add an existing directory to the list.</p>" );
   addPushButton.OnClick( (Button::click_event_handler)&GlobalDirectoryListControl::__Click, *this );

   removePushButton.SetText( "Remove" );
   removePushButton.SetToolTip( "<p>Remove the selected directory from the list.</p>" );
   removePushButton.OnClick( (Button::click_event_handler)&GlobalDirectoryListControl::__Click, *this );

   resetPushButton.SetText( "Reset" );
   resetPushButton.SetToolTip( "<p>Reset the list of directories.</p>" );
   resetPushButton.OnClick( (Button::click_event_handler)&GlobalDirectoryListControl::__Click, *this );

   buttonsSizer.SetSpacing( 4 );
   buttonsSizer.Add( addPushButton );
   buttonsSizer.Add( removePushButton );
   buttonsSizer.AddStretch();
   buttonsSizer.Add( resetPushButton );

   sizer.Add( label );
   sizer.Add( directoriesTreeBox );
   sizer.Add( buttonsSizer );
}

void GlobalDirectoryListControl::Synchronize()
{
   if ( item != 0 )
   {
      int currentIdx = directoriesTreeBox.ChildIndex( directoriesTreeBox.CurrentNode() );

      directoriesTreeBox.DisableUpdates();
      directoriesTreeBox.Clear();

      for ( size_type i = 0; i < item->Length(); ++i )
         (new TreeBox::Node( directoriesTreeBox ))->SetText( 0, (*item)[i] );

      if ( !item->IsEmpty() )
         if ( currentIdx >= 0 && currentIdx < directoriesTreeBox.NumberOfChildren() )
            directoriesTreeBox.SetCurrentNode( directoriesTreeBox[currentIdx] );

      directoriesTreeBox.EnableUpdates();
   }
}

void GlobalDirectoryListControl::__NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   if ( item != 0 )
   {
      GetDirectoryDialog d;
      d.SetCaption( dialogTitle );

      if ( d.Execute() )
      {
         (*item)[sender.ChildIndex( &node )] = d.Directory();
         Synchronize();
      }
   }
}

void GlobalDirectoryListControl::__Click( Button& sender, bool checked )
{
   if ( item != 0 )
   {
      const TreeBox::Node* n = directoriesTreeBox.CurrentNode();

      if ( sender == addPushButton )
      {
         GetDirectoryDialog d;
         d.SetCaption( dialogTitle );

         if ( d.Execute() )
         {
            int i = (n != 0) ? directoriesTreeBox.ChildIndex( n ) + 1 : directoriesTreeBox.NumberOfChildren();
            item->Insert( item->At( i ), d.Directory() );
            Synchronize();
         }
      }
      else if ( sender == removePushButton )
      {
         if ( n != 0 )
         {
            item->Remove( item->At( directoriesTreeBox.ChildIndex( n ) ) );
            Synchronize();
         }
      }
      else if ( sender == resetPushButton )
      {
         item->Clear();
         Synchronize();
      }
   }
}

// ----------------------------------------------------------------------------

GlobalFileSetControl::GlobalFileSetControl() : GlobalItemControl(), items()
{
   filesTreeBox.SetNumberOfColumns( 1 );
   filesTreeBox.HideHeader();
   filesTreeBox.DisableMultipleSelections();
   filesTreeBox.DisableRootDecoration();
   filesTreeBox.EnableAlternateRowColor();
   filesTreeBox.SetScaledMinHeight( 80 );
   filesTreeBox.OnNodeActivated( (TreeBox::node_event_handler)&GlobalFileSetControl::__NodeActivated, *this );

   addPushButton.SetText( "Add" );
   addPushButton.SetToolTip( "<p>Add a file to the list.</p>" );
   addPushButton.OnClick( (Button::click_event_handler)&GlobalFileSetControl::__Click, *this );

   removePushButton.SetText( "Remove" );
   removePushButton.SetToolTip( "<p>Remove the selected file from the list.</p>" );
   removePushButton.OnClick( (Button::click_event_handler)&GlobalFileSetControl::__Click, *this );

   clearPushButton.SetText( "Clear" );
   clearPushButton.SetToolTip( "<p>Clear the list of files.</p>" );
   clearPushButton.OnClick( (Button::click_event_handler)&GlobalFileSetControl::__Click, *this );

   buttonsSizer.SetSpacing( 4 );
   buttonsSizer.Add( addPushButton );
   buttonsSizer.Add( removePushButton );
   buttonsSizer.AddStretch();
   buttonsSizer.Add( clearPushButton );

   sizer.Add( label );
   sizer.Add( filesTreeBox );
   sizer.Add( buttonsSizer );
}

void GlobalFileSetControl::Synchronize()
{
   int currentIdx = filesTreeBox.ChildIndex( filesTreeBox.CurrentNode() );

   StringList fileNames;
   for ( size_type i = 0; i < items.Length(); ++i )
      fileNames.Add( items[i]->Trimmed() );
   fileNames.Remove( String() );
   for ( size_type i = 0; i < items.Length(); ++i )
      items[i]->Clear();
   for ( size_type i = 0; i < fileNames.Length(); ++i )
      *items[i] = fileNames[i];

   filesTreeBox.DisableUpdates();
   filesTreeBox.Clear();

   for ( size_type i = 0; i < fileNames.Length(); ++i )
      (new TreeBox::Node( filesTreeBox ))->SetText( 0, fileNames[i] );

   if ( currentIdx >= 0 && currentIdx < int( fileNames.Length() ) )
      filesTreeBox.SetCurrentNode( filesTreeBox[currentIdx] );

   filesTreeBox.EnableUpdates();

   addPushButton.Enable( filesTreeBox.NumberOfChildren() < int( items.Length() ) );
   removePushButton.Enable( filesTreeBox.NumberOfChildren() > 0 );
}

void GlobalFileSetControl::__NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   SaveFileDialog d;
   d.SetCaption( dialogTitle );
   d.SetInitialPath( *items[sender.ChildIndex( &node )] );
   d.DisableOverwritePrompt();

   if ( d.Execute() )
   {
      *items[sender.ChildIndex( &node )] = d.FileName();
      Synchronize();
   }
}

void GlobalFileSetControl::__Click( Button& sender, bool checked )
{
   if ( sender == addPushButton )
   {
      SaveFileDialog d;
      d.SetCaption( dialogTitle );
      d.DisableOverwritePrompt();

      if ( d.Execute() )
      {
         *items[filesTreeBox.NumberOfChildren()] = d.FileName();
         Synchronize();
      }
   }
   else if ( sender == removePushButton )
   {
      const TreeBox::Node* n = filesTreeBox.CurrentNode();
      if ( n != 0 )
      {
         items[filesTreeBox.ChildIndex( n )]->Clear();
         Synchronize();
      }
   }
   else if ( sender == clearPushButton )
   {
      for ( size_type i = 0; i < items.Length(); ++i )
         items[i]->Clear();
      Synchronize();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PreferencesInterface* ThePreferencesInterface = 0;

#include "PreferencesIcon.xpm"

// ----------------------------------------------------------------------------

PreferencesInterface::PreferencesInterface() :
ProcessInterface(), instance( ThePreferencesProcess ), GUI( 0 )
{
   ThePreferencesInterface = this;
}

PreferencesInterface::~PreferencesInterface()
{
   globalItemControls.Clear();
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString PreferencesInterface::Id() const
{
   return "Preferences";
}

MetaProcess* PreferencesInterface::Process() const
{
   return ThePreferencesProcess;
}

const char** PreferencesInterface::IconImageXPM() const
{
   return PreferencesIcon_XPM;
}

InterfaceFeatures PreferencesInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void PreferencesInterface::ApplyInstance() const
{
   instance.LaunchGlobal();
}

void PreferencesInterface::ResetInstance()
{
   PreferencesInstance defaultInstance( ThePreferencesProcess );
   ImportProcess( defaultInstance );
}

bool PreferencesInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Preferences" );
      UpdateControls();
   }

   dynamic = false;
   return &P == ThePreferencesProcess;
}

ProcessImplementation* PreferencesInterface::NewProcess() const
{
   return new PreferencesInstance( instance );
}

bool PreferencesInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const PreferencesInstance* r = dynamic_cast<const PreferencesInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a Preferences instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool PreferencesInterface::RequiresInstanceValidation() const
{
   return true;
}

bool PreferencesInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void PreferencesInterface::UpdateControls()
{
   for ( global_item_ref_list::iterator i = globalItemControls.Begin();
         i != globalItemControls.End();
         ++i )
   {
      (*i)->Synchronize();
   }

   GUI->PerformAllPageAdditionalUpdates();

   TreeBox::Node* node = GUI->CategorySelection_TreeBox.CurrentNode();
   if ( node == 0 )
   {
      node = GUI->CategorySelection_TreeBox[0];
      GUI->CategorySelection_TreeBox.SetCurrentNode( node );
   }

   SelectPageByTreeNode( node );
}

void PreferencesInterface::SelectPageByTreeNode( TreeBox::Node* node )
{
   CategoryNode* cnode = dynamic_cast<CategoryNode*>( node );
   if ( cnode == nullptr )
      return;
   PreferencesCategoryPage* page = GUI->PageByIndex( cnode->PageIndex() );
   if ( page == nullptr || page->IsVisible() )
      return;

   GUI->HideAllPages();

   page->BringToFront();
   page->Show();

   String title = node->Text( 0 );
   if ( node->Parent() != 0 )
      title.Prepend( node->Parent()->Text( 0 ) + ": " );

   GUI->Title_Label.SetText( title );
}

// ----------------------------------------------------------------------------

void PreferencesInterface::__Category_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& /*oldCurrent*/ )
{
   TreeBox::Node* node = &current;
   if ( node->NumberOfChildren() > 0 )
   {
      node = node->Child( 0 );
      sender.SetCurrentNode( node );
   }
   SelectPageByTreeNode( node );
}

void PreferencesInterface::__LoadSettings_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->LoadCurrentSettings_PushButton )
   {
      instance.LoadCurrentSettings();
   }
   else if ( sender == GUI->LoadCurrentPageSettings_PushButton ||
             sender == GUI->LoadDefaultPageSettings_PushButton )
   {
      TreeBox::Node* node = GUI->CategorySelection_TreeBox.CurrentNode();
      if ( node != nullptr )
      {
         CategoryNode* cnode = dynamic_cast<CategoryNode*>( node );
         if ( cnode != nullptr )
         {
            PreferencesCategoryPage* page = GUI->PageByIndex( cnode->PageIndex() );
            if ( page != nullptr )
            {
               PreferencesInstance defaultInstance( ThePreferencesProcess ); // yeah, it loads defaults by default :)
               if ( sender == GUI->LoadCurrentPageSettings_PushButton )
                  defaultInstance.LoadCurrentSettings();
               page->TransferSettings( instance, defaultInstance );
            }
         }
      }
   }

   UpdateControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

MainWindowPreferencesPage::MainWindowPreferencesPage( PreferencesInstance& instance )
{
   MaximizeAtStartup_Flag.checkBox.SetText( "Maximize at startup" );
   MaximizeAtStartup_Flag.item = &instance.mainWindow.maximizeAtStartup;
   MaximizeAtStartup_Flag.SetToolTip(
      "<p>If enabled, PixInsight's main window will always be maximized "
      "upon startup. If disabled, window size and position will be restored from the last session.</p>" );

   FullScreenAtStartup_Flag.checkBox.SetText( "Full screen at startup" );
   FullScreenAtStartup_Flag.item = &instance.mainWindow.fullScreenAtStartup;
   FullScreenAtStartup_Flag.SetToolTip(
      "<p>If enabled, PixInsight's main window will always be expanded to cover the entire available "
      "screen space upon startup. When enabled, this option takes precedence over maximization "
      "(previous option). If disabled, window size and position will be restored from the last session.</p>" );

   NativeMenuBar_Flag.checkBox.SetText( "Native menu bar" );
   NativeMenuBar_Flag.item = &instance.mainWindow.nativeMenuBar;
   NativeMenuBar_Flag.SetToolTip(
      "<p>When enabled, PixInsight will use a native menu bar on Mac OS X. When disabled, a multiplatform "
      "menu bar will be used. This option only works on Mac OS X; it is ignored on X11 (FreeBSD, Linux) "
      "and Windows platforms. Changing this option requires a restart of the PixInsight core application "
      "to take effect.</p>" );

   CapitalizedMenuBars_Flag.checkBox.SetText( "Capitalized menu bars" );
   CapitalizedMenuBars_Flag.item = &instance.mainWindow.capitalizedMenuBars;
   CapitalizedMenuBars_Flag.SetToolTip(
      "<p>When this option is enabled, menu bar items will be written in uppercase characters "
      "(FILE, EDIT, ...). When disabled, only the first letter of each menu bar item will be uppercase "
      "(File, Edit, ...), following more traditional UI criteria. Note that this option does not apply to "
      "native menu bars on Mac OS X. Changing this option requires an application restart to take effect.</p>" );

   ShowSplashAtStartup_Flag.checkBox.SetText( "Show splash at startup" );
   ShowSplashAtStartup_Flag.item = &instance.mainWindow.showSplashAtStartup;
   ShowSplashAtStartup_Flag.SetToolTip(
      "<p>Enable this option to see a nice splash screen at startup.</p>" );

   CheckForUpdatesAtStartup_Flag.checkBox.SetText( "Check for updates at startup" );
   CheckForUpdatesAtStartup_Flag.item = &instance.mainWindow.checkForUpdatesAtStartup;
   CheckForUpdatesAtStartup_Flag.SetToolTip(
      "<p>Enable this option to download repository information on software updates at startup.</p>" );

   ConfirmProgramTermination_Flag.checkBox.SetText( "Confirm program termination" );
   ConfirmProgramTermination_Flag.item = &instance.mainWindow.confirmProgramTermination;
   ConfirmProgramTermination_Flag.SetToolTip(
      "<p>If enabled, PixInsight will ask if you really want to terminate execution when you "
      "select the File > Exit main menu option.</p>" );

   AcceptDroppedFiles_Flag.checkBox.SetText( "Accept dropped files/URLs" );
   AcceptDroppedFiles_Flag.item = &instance.mainWindow.acceptDroppedFiles;
   AcceptDroppedFiles_Flag.SetToolTip(
      "<p>If enabled, PixInsight will attempt to open images, PSM files and script files "
      "when one or more file or URL items are dragged to the workspace.</p>" );

   DoubleClickLaunchesOpenDialog_Flag.checkBox.SetText( "Double-click launches Open File dialog" );
   DoubleClickLaunchesOpenDialog_Flag.item = &instance.mainWindow.doubleClickLaunchesOpenDialog;
   DoubleClickLaunchesOpenDialog_Flag.SetToolTip(
      "<p>Enable this option to launch the File Open dialog box by double-clicking on the workspace.</p>" );

   MaxRecentFiles_Integer.label.SetText( "Maximum length of recent file lists" );
   MaxRecentFiles_Integer.item = &instance.mainWindow.maxRecentFiles;
   MaxRecentFiles_Integer.spinBox.SetRange( 2, 128 );
   MaxRecentFiles_Integer.SetToolTip(
      "<p>This is the maximum number of recently opened files that PixInsight can remember "
      "for quick access from the File > Open Recent menu.</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( MaximizeAtStartup_Flag );
   Page_Sizer.Add( FullScreenAtStartup_Flag );
   Page_Sizer.Add( NativeMenuBar_Flag );
   Page_Sizer.Add( CapitalizedMenuBars_Flag );
   Page_Sizer.Add( ShowSplashAtStartup_Flag );
   Page_Sizer.Add( CheckForUpdatesAtStartup_Flag );
   Page_Sizer.Add( ConfirmProgramTermination_Flag );
   Page_Sizer.Add( AcceptDroppedFiles_Flag );
   Page_Sizer.Add( DoubleClickLaunchesOpenDialog_Flag );
   Page_Sizer.Add( MaxRecentFiles_Integer );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void MainWindowPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.mainWindow.maximizeAtStartup             = from.mainWindow.maximizeAtStartup;
   to.mainWindow.fullScreenAtStartup           = from.mainWindow.fullScreenAtStartup;
   to.mainWindow.showSplashAtStartup           = from.mainWindow.showSplashAtStartup;
   to.mainWindow.checkForUpdatesAtStartup      = from.mainWindow.checkForUpdatesAtStartup;
   to.mainWindow.confirmProgramTermination     = from.mainWindow.confirmProgramTermination;
   to.mainWindow.acceptDroppedFiles            = from.mainWindow.acceptDroppedFiles;
   to.mainWindow.doubleClickLaunchesOpenDialog = from.mainWindow.doubleClickLaunchesOpenDialog;
   to.mainWindow.maxRecentFiles                = from.mainWindow.maxRecentFiles;
}

// ----------------------------------------------------------------------------

ResourcesPreferencesPage::ResourcesPreferencesPage( PreferencesInstance& instance )
{
   StyleSheet_File.label.SetText( "Core Style Sheet File" );
   StyleSheet_File.item = &instance.application.styleSheetFile;
   StyleSheet_File.SetToolTip(
      "<p>This is the path to a local <i>Qt style sheet</i> file (.qss) that controls the appearance of the "
      "entire PixInsight platform.</p>"
      "<p>Qt style sheets provide exhaustive customization of all controls in Qt-based applications. Qt style "
      "sheets are similar in concept and syntax to HTML Cascading Style Sheets (CSS), but adapted to the world "
      "of graphical user interfaces.</p>"
      "<p>With style sheets, you can literally transform the whole PixInsight platform to adapt it to your "
      "needs and taste, giving it a personal look and feel of your choice. For full information on Qt style "
      "sheets and their syntax, we recommend the following section of the official Qt reference documentation:</p>"
      "<p>http://doc.qt.io/qt-5/stylesheet.html</p>"
      "<p>The core style sheet file is only loaded upon application startup. Changes to this parameter will take "
      "place the next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying an invalid style sheet file may yield an unusable core application. "
      "DO NOT change this parameter if you are not sure of what you are doing.</b></p>" );

   Resources_FileSet.items.Add( &instance.application.resourceFile01 );
   Resources_FileSet.items.Add( &instance.application.resourceFile02 );
   Resources_FileSet.items.Add( &instance.application.resourceFile03 );
   Resources_FileSet.items.Add( &instance.application.resourceFile04 );
   Resources_FileSet.items.Add( &instance.application.resourceFile05 );
   Resources_FileSet.items.Add( &instance.application.resourceFile06 );
   Resources_FileSet.items.Add( &instance.application.resourceFile07 );
   Resources_FileSet.items.Add( &instance.application.resourceFile08 );
   Resources_FileSet.items.Add( &instance.application.resourceFile09 );
   Resources_FileSet.items.Add( &instance.application.resourceFile10 );
   Resources_FileSet.label.SetText( "Core Resource Files" );
   Resources_FileSet.dialogTitle = "Select Resource File";
   Resources_FileSet.SetToolTip(
      "<p>This is a list of up to 10 local Qt resource files (.rcc) that will be loaded by the PixInsight core "
      "application. The 'core-icons.rcc' file is the default resource that defines all the icons and images used by "
      "PixInsight's graphical interface in its default style. Additional resources can be specified to provide "
      "support for special modules, scripts or custom GUI styles.</p>"
      "<p>For information on the Qt resource system, please refer to the following document:</p>"
      "<p>http://doc.qt.io/qt-5/resources.html</p>"
      "<p>Core resource files are only loaded upon application startup. Changes to this parameter will take place "
      "the next time PixInsight is executed.</p>"
      "<p><b>* Warning * Replacing standard core resources with invalid ones may yield an unusable core application, "
      "and/or cause problems with distributed modules and scripts. DO NOT change this parameter if you are not sure "
      "of what you are doing.</b></p>" );

   AutoUIScaling_Flag.checkBox.SetText( "Automatic UI scaling" );
   AutoUIScaling_Flag.item = &instance.application.autoUIScaling;
   AutoUIScaling_Flag.disableControl = &UIScalingFactor_Real;
   AutoUIScaling_Flag.SetToolTip(
      "<p>When enabled, the PixInsight Core application will try to detect high-dpi monitors automatically, and will "
      "compute appropriate scaling factors for all GUI elements and resources. For automatic DPI-based scaling, the "
      "reference is a 27-inch monitor at QHD resolution (2560x1440 physical display pixels, or 109 dpi). This "
      "pixel density corresponds to a scaling factor of 1.0.</p>"
      "<p>Keep in mind that automatic display characterization is not always feasible or accurate. For example, some "
      "X11 desktop managers don't support high-dpi monitors adequately, especially with multiple monitors working at "
      "different resolutions. Virtualized operating systems can also have problems for automatic detection of monitor "
      "dimensions. In these cases, you can disable this option to specify the correct scaling factor manually (see the "
      "UI scaling factor parameter below).</p>"
      "<p>Core UI scaling parameters are only fetched upon application startup. Changes to this parameter will take "
      "place the next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying wrong UI scaling settings may yield an unusable core application. DO NOT change "
      "this parameter if you are not sure of what you are doing.</b></p>" );

   UIScalingFactor_Real.label.SetText( "UI scaling factor" );
   UIScalingFactor_Real.numericEdit.SetReal();
   UIScalingFactor_Real.numericEdit.SetRange( 1, 4 );
   UIScalingFactor_Real.numericEdit.SetPrecision( 2 );
   UIScalingFactor_Real.item = &instance.application.uiScalingFactor;
   UIScalingFactor_Real.SetToolTip(
      "<p>When the automatic UI scaling option is disabled, you can specify a manual scaling factor in the range "
      "[1,4].</p>"
      "<p>Core UI scaling parameters are only fetched upon application startup. Changes to this parameter will take "
      "place the next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying a wrong scaling factor may yield an unusable core application. DO NOT change "
      "this parameter if you are not sure of what you are doing.</b></p>" );

   FontResolution_Integer.label.SetText( "Font resolution (dpi)" );
   FontResolution_Integer.spinBox.SetRange( 1, 1000 );
   FontResolution_Integer.item = &instance.application.fontResolution;
   FontResolution_Integer.SetToolTip(
      "<p>Resolution in dots per inch (dpi) for automatic conversion of point dimensions to scaled pixel dimensions. "
      "This parameter will be applied for automatic scaling of core font resources in a platform-independent way. This "
      "parameter can be useful to fine tune core font rendering. The default value is 100 dpi.</p>"
      "<p>Core UI scaling parameters are only fetched upon application startup. Changes to this parameter will take "
      "place the next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying wrong UI scaling settings may yield an unusable core application. DO NOT change "
      "this parameter if you are not sure of what you are doing.</b></p>" );

   UIScaling_Sizer.SetSpacing( 4 );
   UIScaling_Sizer.Add( UIScalingFactor_Real, 50 );
   UIScaling_Sizer.Add( FontResolution_Integer, 50 );

   LowResFont_String.label.SetText( "Low-resolution font" );
   LowResFont_String.item = &instance.application.lowResFont;
   LowResFont_String.SetToolTip(
      "<p>Core UI font family used for normal (low-dpi) screen resolutions.</p>"
      "<p>Core UI settings are only fetched upon application startup. Changes to this parameter will take place the "
      "next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying wrong core fonts may yield an unusable core application. DO NOT change this "
      "parameter if you are not sure of what you are doing.</b></p>" );

   HighResFont_String.label.SetText( "High-resolution font" );
   HighResFont_String.item = &instance.application.highResFont;
   HighResFont_String.SetToolTip(
      "<p>Core UI font family used for high-dpi screen resolutions.</p>"
      "<p>Core UI settings are only fetched upon application startup. Changes to this parameter will take place the "
      "next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying wrong core fonts may yield an unusable core application. DO NOT change this "
      "parameter if you are not sure of what you are doing.</b></p>" );

   Font_Sizer.SetSpacing( 4 );
   Font_Sizer.Add( LowResFont_String, 50 );
   Font_Sizer.Add( HighResFont_String, 50 );

   LowResMonoFont_String.label.SetText( "Low-resolution monospaced font" );
   LowResMonoFont_String.item = &instance.application.lowResMonoFont;
   LowResMonoFont_String.SetToolTip(
      "<p>Core UI monospaced font family used for normal (low-dpi) screen resolutions.</p>"
      "<p>Core UI settings are only fetched upon application startup. Changes to this parameter will take place the "
      "next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying wrong core fonts may yield an unusable core application. DO NOT change this "
      "parameter if you are not sure of what you are doing.</b></p>" );

   HighResMonoFont_String.label.SetText( "High-resolution monospaced font" );
   HighResMonoFont_String.item = &instance.application.highResMonoFont;
   HighResMonoFont_String.SetToolTip(
      "<p>Core UI monospaced font family used for high-dpi screen resolutions.</p>"
      "<p>Core UI settings are only fetched upon application startup. Changes to this parameter will take place the "
      "next time PixInsight is executed.</p>"
      "<p><b>* Warning * Specifying wrong core fonts may yield an unusable core application. DO NOT change this "
      "parameter if you are not sure of what you are doing.</b></p>" );

   MonoFont_Sizer.SetSpacing( 4 );
   MonoFont_Sizer.Add( LowResMonoFont_String, 50 );
   MonoFont_Sizer.Add( HighResMonoFont_String, 50 );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( StyleSheet_File );
   Page_Sizer.Add( Resources_FileSet );
   Page_Sizer.Add( AutoUIScaling_Flag );
   Page_Sizer.Add( UIScaling_Sizer );
   Page_Sizer.Add( Font_Sizer );
   Page_Sizer.Add( MonoFont_Sizer );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void ResourcesPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.application.styleSheetFile = from.application.styleSheetFile;
   to.application.resourceFile01 = from.application.resourceFile01;
   to.application.resourceFile02 = from.application.resourceFile02;
   to.application.resourceFile03 = from.application.resourceFile03;
   to.application.resourceFile04 = from.application.resourceFile04;
   to.application.resourceFile05 = from.application.resourceFile05;
   to.application.resourceFile06 = from.application.resourceFile06;
   to.application.resourceFile07 = from.application.resourceFile07;
   to.application.resourceFile08 = from.application.resourceFile08;
   to.application.resourceFile09 = from.application.resourceFile09;
   to.application.resourceFile10 = from.application.resourceFile10;
   to.application.autoUIScaling = from.application.autoUIScaling;
   to.application.uiScalingFactor = from.application.uiScalingFactor;
   to.application.fontResolution = from.application.fontResolution;
   to.application.lowResFont = from.application.lowResFont;
   to.application.highResFont = from.application.highResFont;
   to.application.lowResMonoFont = from.application.lowResMonoFont;
   to.application.highResMonoFont = from.application.highResMonoFont;
}

// ----------------------------------------------------------------------------

GUIEffectsPreferencesPage::GUIEffectsPreferencesPage( PreferencesInstance& instance )
{
   HoverableAutoHideWindows_Flag.checkBox.SetText( "Hoverable AutoHide window selectors" );
   HoverableAutoHideWindows_Flag.item = &instance.mainWindow.hoverableAutoHideWindows;
   HoverableAutoHideWindows_Flag.SetToolTip(
      "<p>When enabled, docked AutoHide windows can be shown by just hovering the mouse cursor over vertical "
      "selector tabs for about 0.4 seconds. This is the default state. When this option is disabled, you must "
      "click on a selector tab to open the corresponding AutoHide window.</p>" );

   DesktopSettingsAware_Flag.checkBox.SetText( "Desktop settings aware" );
   DesktopSettingsAware_Flag.item = &instance.mainWindow.desktopSettingsAware;
   DesktopSettingsAware_Flag.SetToolTip(
      "<p>When enabled, PixInsight will use some colors and fonts as per global system desktop settings.</p>" );

   TranslucentWindows_Flag.checkBox.SetText( "Translucent workspace top-level windows" );
   TranslucentWindows_Flag.item = &instance.mainWindow.translucentWindows;
   TranslucentWindows_Flag.SetToolTip(
      "<p>Enable to allow translucency (varying opacity) effects for top-level workspace windows (e.g., tool windows).</p>" );

   TranslucentChildWindows_Flag.checkBox.SetText( "Translucent workspace child windows" );
   TranslucentChildWindows_Flag.item = &instance.mainWindow.translucentChildWindows;
   TranslucentChildWindows_Flag.SetToolTip(
      "<p>Enable to allow translucency (varying opacity) effects for child workspace windows (e.g., image windows).</p>"
      "<p><b>Note</b> Due to several Qt bugs, currently child window translucency only works reliably on X11 (FreeBSD and Linux). "
      "This option is disabled by default on Mac OS X and Windows; if you enable it on these platforms, make sure that it doesn't "
      "cause screen rendering artifacts or instabilities.</p>"
      "<p><b>Enable this option on Windows and Mac OS X on your own risk.</b></p>" );

   FadeWindows_Flag.checkBox.SetText( "Fade workspace windows" );
   FadeWindows_Flag.item = &instance.mainWindow.fadeWindows;
   FadeWindows_Flag.SetToolTip(
      "<p>Enable to allow fading in/out effects for workspace child windows.</p>" );

   FadeAutoHideWindows_Flag.checkBox.SetText( "Animate AutoHide windows" );
   FadeAutoHideWindows_Flag.item = &instance.mainWindow.fadeAutoHideWindows;
   FadeAutoHideWindows_Flag.SetToolTip(
      "<p>Enable to allow sliding animation effects for docked AutoHide windows.</p>" );

   FadeWorkspaces_Flag.checkBox.SetText( "Fade workspaces" );
   FadeWorkspaces_Flag.item = &instance.mainWindow.fadeWorkspaces;
   FadeWorkspaces_Flag.SetToolTip(
      "<p>Enable to allow fading in/out effects for workspaces.</p>" );

   FadeMenu_Flag.checkBox.SetText( "Fade menus" );
   FadeMenu_Flag.item = &instance.mainWindow.fadeMenu;
   FadeMenu_Flag.SetToolTip(
      "<p>Enable to show faded menus.</p>" );

   FadeToolTip_Flag.checkBox.SetText( "Fade tool tips" );
   FadeToolTip_Flag.item = &instance.mainWindow.fadeToolTip;
   FadeToolTip_Flag.SetToolTip(
      "<p>Enable to show faded tool tip panels.</p>" );

   ExplodeIcons_Flag.checkBox.SetText( "Explode icons" );
   ExplodeIcons_Flag.item = &instance.mainWindow.explodeIcons;
   ExplodeIcons_Flag.SetToolTip(
      "<p>Enable to allow exploding icon effects (e.g., when an icon is double clicked).</p>" );

   ImplodeIcons_Flag.checkBox.SetText( "Implode icons" );
   ImplodeIcons_Flag.item = &instance.mainWindow.implodeIcons;
   ImplodeIcons_Flag.SetToolTip(
      "<p>Enable to allow imploding icon effects (e.g., when an icon is copied to another).</p>" );

   AnimateWindows_Flag.checkBox.SetText( "Animate workspace windows" );
   AnimateWindows_Flag.item = &instance.mainWindow.animateWindows;
   AnimateWindows_Flag.SetToolTip(
      "<p>Enable to allow moving/resizing effects for workspace windows "
      "(e.g., when a window is iconized/deiconized or shaded/unshaded).</p>" );

   AnimateMenu_Flag.checkBox.SetText( "Animate menus" );
   AnimateMenu_Flag.item = &instance.mainWindow.animateMenu;
   AnimateMenu_Flag.SetToolTip(
      "<p>Enable to show animated pop-up menus.</p>" );

   AnimateCombo_Flag.checkBox.SetText( "Animate combo boxes" );
   AnimateCombo_Flag.item = &instance.mainWindow.animateCombo;
   AnimateCombo_Flag.SetToolTip(
      "<p>Enable to show animated combo boxes.</p>" );

   AnimateToolTip_Flag.checkBox.SetText( "Animate tool tips" );
   AnimateToolTip_Flag.item = &instance.mainWindow.animateToolTip;
   AnimateToolTip_Flag.SetToolTip(
      "<p>Enable to show animated tool tip panels.</p>" );

   AnimateToolBox_Flag.checkBox.SetText( "Animate tool boxes" );
   AnimateToolBox_Flag.item = &instance.mainWindow.animateToolBox;
   AnimateToolBox_Flag.SetToolTip(
      "<p>Enable to show animated tool boxes (currently not used; reserved for future extension).</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( HoverableAutoHideWindows_Flag );
   Page_Sizer.Add( DesktopSettingsAware_Flag );
   Page_Sizer.Add( TranslucentWindows_Flag );
   Page_Sizer.Add( TranslucentChildWindows_Flag );
   Page_Sizer.Add( FadeWindows_Flag );
   Page_Sizer.Add( FadeAutoHideWindows_Flag );
   Page_Sizer.Add( FadeWorkspaces_Flag );
   Page_Sizer.Add( FadeMenu_Flag );
   Page_Sizer.Add( FadeToolTip_Flag );
   Page_Sizer.Add( ExplodeIcons_Flag );
   Page_Sizer.Add( ImplodeIcons_Flag );
   Page_Sizer.Add( AnimateWindows_Flag );
   Page_Sizer.Add( AnimateMenu_Flag );
   Page_Sizer.Add( AnimateCombo_Flag );
   Page_Sizer.Add( AnimateToolTip_Flag );
   Page_Sizer.Add( AnimateToolBox_Flag );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void GUIEffectsPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.mainWindow.hoverableAutoHideWindows = from.mainWindow.hoverableAutoHideWindows;
   to.mainWindow.desktopSettingsAware     = from.mainWindow.desktopSettingsAware;
   to.mainWindow.nativeMenuBar            = from.mainWindow.nativeMenuBar;
   to.mainWindow.capitalizedMenuBars      = from.mainWindow.capitalizedMenuBars;
   to.mainWindow.translucentWindows       = from.mainWindow.translucentWindows;
   to.mainWindow.translucentChildWindows  = from.mainWindow.translucentChildWindows;
   to.mainWindow.fadeWindows              = from.mainWindow.fadeWindows;
   to.mainWindow.fadeAutoHideWindows      = from.mainWindow.fadeAutoHideWindows;
   to.mainWindow.fadeWorkspaces           = from.mainWindow.fadeWorkspaces;
   to.mainWindow.fadeMenu                 = from.mainWindow.fadeMenu;
   to.mainWindow.fadeToolTip              = from.mainWindow.fadeToolTip;
   to.mainWindow.explodeIcons             = from.mainWindow.explodeIcons;
   to.mainWindow.implodeIcons             = from.mainWindow.implodeIcons;
   to.mainWindow.animateWindows           = from.mainWindow.animateWindows;
   to.mainWindow.animateMenu              = from.mainWindow.animateMenu;
   to.mainWindow.animateCombo             = from.mainWindow.animateCombo;
   to.mainWindow.animateToolTip           = from.mainWindow.animateToolTip;
   to.mainWindow.animateToolBox           = from.mainWindow.animateToolBox;
}

// ----------------------------------------------------------------------------

FileIOPreferencesPage::FileIOPreferencesPage( PreferencesInstance& instance )
{
   BackupFiles_Flag.checkBox.SetText( "Backup image files" );
   BackupFiles_Flag.item = &instance.imageWindow.backupFiles;
   BackupFiles_Flag.SetToolTip(
      "<p>Make backups of existing disk image files when saving images.</p>"
      "<p><b>* Warning *</b> This may increase disk space requirements considerably.</p>" );

   DefaultFileExtension_Ext.label.SetText( "Default image file extension" );
   DefaultFileExtension_Ext.item = &instance.imageWindow.defaultFileExtension;
   DefaultFileExtension_Ext.SetToolTip(
      "<p>This is the default file extension proposed by File Save As dialogs for new images, when the "
      "<i>remember File Save file type</i> option (see below) is disabled.</p>" );

   NativeFileDialogs_Flag.checkBox.SetText( "Use native file dialogs" );
   NativeFileDialogs_Flag.item = &instance.imageWindow.nativeFileDialogs;
   NativeFileDialogs_Flag.SetToolTip(
      "<p>Use native file dialogs on FreeBSD, Linux, Mac OS X and Windows platforms.</p>"
      "<p>When this option is disabled, PixInsight uses its own, platform-independent file dialogs for "
      "all <i>file open</i> and <i>file save</i> operations. Platform-independent file dialogs are "
      "extremely reliable and behave consistently on all supported operating systems.</p>"
      "<p>If this option is enabled, PixInsight uses the native dialogs provided by the host operating "
      "system. On Windows and Mac OS X, PixInsight will use the <i>common dialogs</i> provided by these "
      "operating systems. On FreeBSD/X11 and Linux/X11 platforms, PixInsight supports native file dialogs "
      "on KDE4 and GNOME/GTK+. However, this option is currently disabled by default on X11 due to some "
      "problematic interactions between KDE file dialogs and the PixInsight Core application.</p>" );

   RememberFileOpenType_Flag.checkBox.SetText( "Remember File Open file type" );
   RememberFileOpenType_Flag.item = &instance.imageWindow.rememberFileOpenType;
   RememberFileOpenType_Flag.SetToolTip(
      "<p>Automatically select the last file type used on File Open dialogs.</p>" );

   RememberFileSaveType_Flag.checkBox.SetText( "Remember File Save file type" );
   RememberFileSaveType_Flag.item = &instance.imageWindow.rememberFileSaveType;
   RememberFileSaveType_Flag.SetToolTip(
      "<p>Automatically select the last file type used on File Save As dialogs.</p>" );

   StrictFileSaveMode_Flag.checkBox.SetText( "Strict file save mode" );
   StrictFileSaveMode_Flag.item = &instance.imageWindow.strictFileSaveMode;
   StrictFileSaveMode_Flag.SetToolTip(
      "<p>In strict mode, when a modified image is saved to disk using a lossy generation "
      "scheme (e.g., JPEG compression), its modified status is not cleared, and its "
      "associated file path is not changed to the new location. This is because what "
      "has been written to disk is not an accurate representation of existing image data.</p>" );

   DefaultEmbedThumbnails_Flag.checkBox.SetText( "Enable thumbnail embedding by default" );
   DefaultEmbedThumbnails_Flag.item = &instance.imageWindow.defaultEmbedThumbnails;
   DefaultEmbedThumbnails_Flag.SetToolTip(
      "<p>Enable <i>Embed Thumbnail</i> check boxes on File Options dialogs by default.</p>" );

   DefaultEmbedProperties_Flag.checkBox.SetText( "Enable embedding of image properties by default" );
   DefaultEmbedProperties_Flag.item = &instance.imageWindow.defaultEmbedProperties;
   DefaultEmbedProperties_Flag.SetToolTip(
      "<p>Enable <i>Embed Properties</i> check boxes on File Options dialogs by default.</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( BackupFiles_Flag );
   Page_Sizer.Add( DefaultFileExtension_Ext );
   Page_Sizer.Add( NativeFileDialogs_Flag );
   Page_Sizer.Add( RememberFileOpenType_Flag );
   Page_Sizer.Add( RememberFileSaveType_Flag );
   Page_Sizer.Add( StrictFileSaveMode_Flag );
   Page_Sizer.Add( DefaultEmbedThumbnails_Flag );
   Page_Sizer.Add( DefaultEmbedProperties_Flag );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void FileIOPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.backupFiles            = from.imageWindow.backupFiles;
   to.imageWindow.defaultFileExtension   = from.imageWindow.defaultFileExtension;
   to.imageWindow.nativeFileDialogs      = from.imageWindow.nativeFileDialogs;
   to.imageWindow.rememberFileOpenType   = from.imageWindow.rememberFileOpenType;
   to.imageWindow.rememberFileSaveType   = from.imageWindow.rememberFileSaveType;
   to.imageWindow.strictFileSaveMode     = from.imageWindow.strictFileSaveMode;
   to.imageWindow.defaultEmbedThumbnails = from.imageWindow.defaultEmbedThumbnails;
}

// ----------------------------------------------------------------------------

DirectoriesAndNetworkPreferencesPage::DirectoriesAndNetworkPreferencesPage( PreferencesInstance& instance )
{
   SwapDirectories_DirList.label.SetText( "Swap storage directories" );
   SwapDirectories_DirList.dialogTitle = "Select Swap Directory";
   SwapDirectories_DirList.item = &instance.imageWindow.swapDirectories;
   SwapDirectories_DirList.SetToolTip(
      "<p>This is the list of directories (or <i>folders</i>) where PixInsight generates temporary swap files.</p>"
      "<p>Each directory in this list must be a full path specification to an existing directory. "
      "With two or more swap directories, PixInsight uses concurrent execution threads to perform <i>parallel disk I/O "
      "operations</i> when reading and writing swap files. Parallel swap file access can provide significant performance "
      "improvements, especially working with very large images that require gigabytes of swap disk storage.</p>"
      "<p><b>* Important *</b> The swap storage routines have been optimized for parallel I/O operations on fast SSD "
      "disks and virtual RAM drives. If you use one of these drives for swap file storage, the best performance will be "
      "achieved when the same physical or virtual device (or a directory on it) is specified multiple times in this list. "
      "The optimal number of concurrent threads for a SSD or RAM disk often lies between 4 and 8, but the best value must "
      "be determined experimentally in each case through benchmarks.</p>"
      "<p><b>* Warning *</b> If you use rotational hard disks for swap file storage, do not specify two or more swap "
      "directories that are supported by the same physical device. Along with causing a severe performance degradation, "
      "<b>multiple parallel write operations performed on a <u>rotational</u> hard disk may be dangerous to the integrity "
      "of the disk drive.</b></p>"
      "<p><b>* Warning *</b> Ensure that there is enough free space on the storage device(s) that support these "
      "directories on the local filesystem. Insufficient swap disk space may compromise system stability. For normal "
      "CCD imaging production, a minimum of 60 GB is strongly recommended.</p>"
      "<p><b>* Important *</b> Many critical operations depend on fast sequential access to very large files on the "
      "PixInsight platform. For this reason, a highly fragmented filesystem may degrade performance. Under MS Windows "
      "(both FAT and NTFS filesystems), it is very important to avoid heavy disk fragmentation by running the "
      "<i>defrag</i> utility (or equivalent) on a regular basis. Under UNIX/Linux and Mac OS X, there are no "
      "fragmentation problems because the native filesystems on these platforms prevent fragmentation automatically.</p>" );

   SwapCompression_Flag.checkBox.SetText( "Swap file compression" );
   SwapCompression_Flag.item = &instance.imageWindow.swapCompression;
   SwapCompression_Flag.SetToolTip(
      "<p>Enable this option to compress image swap files. In the current versions of PixInsight, swap files are compressed "
      "using a multithreaded implementation of the LZ4 fast lossless compression algorithm with byte shuffling. This allows "
      "for real-time compression/decompression of swap data for moderately sized images. Swap file compression can provide "
      "significant disk space savings with minimal impact on I/O throughoutput in most cases.</p>" );

   DownloadsDirectory_Dir.label.SetText( "Downloads directory" );
   DownloadsDirectory_Dir.item = &instance.imageWindow.downloadsDirectory;
   DownloadsDirectory_Dir.SetToolTip(
      "<p>This is the directory where PixInsight will store all downloaded files.</p>"
      "<p><b>* Warning *</b> Ensure that there is enough free space on the storage device that supports this directory "
      "on the local filesystem, or download operations may fail.</p>" );

   FollowDownloadLocations_Flag.checkBox.SetText( "Follow download locations" );
   FollowDownloadLocations_Flag.item = &instance.imageWindow.followDownloadLocations;
   FollowDownloadLocations_Flag.SetToolTip(
      "<p>Enable this option to allow PixInsight to follow HTTP redirections, even if they point to a different "
      "server than the one specified in the original URL.</p>"
      "<p><b>* Warning * Enabling this option is potentially dangerous. DO NOT enable it if you "
      "are not absolutely sure of what you are doing.</b></p>" );

   VerboseNetworkOperations_Flag.checkBox.SetText( "Verbose network file I/O operations" );
   VerboseNetworkOperations_Flag.item = &instance.imageWindow.verboseNetworkOperations;
   VerboseNetworkOperations_Flag.SetToolTip(
      "<p>Enable this option to obtain information about network operations on PixInsight's stdout. Currently "
      "this only works under UNIX/Linux and Mac OS X.</p>"
      "<p><b>* Warning * Be aware that sensitive information, including user passwords and IP addresses, "
      "can be written to stdout as part of the generated reports.</b></p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( SwapDirectories_DirList );
   Page_Sizer.Add( SwapCompression_Flag );
   Page_Sizer.Add( DownloadsDirectory_Dir );
   Page_Sizer.Add( FollowDownloadLocations_Flag );
   Page_Sizer.Add( VerboseNetworkOperations_Flag );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void DirectoriesAndNetworkPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.swapDirectories          = from.imageWindow.swapDirectories;
   to.imageWindow.swapCompression          = from.imageWindow.swapCompression;
   to.imageWindow.downloadsDirectory       = from.imageWindow.downloadsDirectory;
   to.imageWindow.followDownloadLocations  = from.imageWindow.followDownloadLocations;
   to.imageWindow.verboseNetworkOperations = from.imageWindow.verboseNetworkOperations;
}

// ----------------------------------------------------------------------------

DefaultImageResolutionPreferencesPage::DefaultImageResolutionPreferencesPage( PreferencesInstance& instance )
{
   DefaultHorizontalResolution_Real.label.SetText( "Default horizontal resolution" );
   DefaultHorizontalResolution_Real.numericEdit.SetReal();
   DefaultHorizontalResolution_Real.numericEdit.SetRange( 1, 10000 );
   DefaultHorizontalResolution_Real.numericEdit.SetPrecision( 4 );
   DefaultHorizontalResolution_Real.item = &instance.imageWindow.defaultHorizontalResolution;
   DefaultHorizontalResolution_Real.SetToolTip(
      "<p>This is the default image resolution on the X-axis, in pixels per resolution unit.</p>" );

   DefaultVerticalResolution_Real.label.SetText( "Default vertical resolution" );
   DefaultVerticalResolution_Real.numericEdit.SetReal();
   DefaultVerticalResolution_Real.numericEdit.SetRange( 1, 10000 );
   DefaultVerticalResolution_Real.numericEdit.SetPrecision( 4 );
   DefaultVerticalResolution_Real.item = &instance.imageWindow.defaultVerticalResolution;
   DefaultVerticalResolution_Real.SetToolTip(
      "<p>This is the default image resolution on the Y-axis, in pixels per resolution unit.</p>" );

   DefaultMetricResolution_Flag.checkBox.SetText( "Default metric resolution" );
   DefaultMetricResolution_Flag.item = &instance.imageWindow.defaultMetricResolution;
   DefaultMetricResolution_Flag.SetToolTip(
      "<p>Enable to use centimeters as the default resolution unit. Disable to use inches.</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( DefaultHorizontalResolution_Real );
   Page_Sizer.Add( DefaultVerticalResolution_Real );
   Page_Sizer.Add( DefaultMetricResolution_Flag );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void DefaultImageResolutionPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.defaultHorizontalResolution = from.imageWindow.defaultHorizontalResolution;
   to.imageWindow.defaultVerticalResolution   = from.imageWindow.defaultVerticalResolution;
   to.imageWindow.defaultMetricResolution     = from.imageWindow.defaultMetricResolution;
}

// ----------------------------------------------------------------------------

DefaultMaskSettingsPreferencesPage::DefaultMaskSettingsPreferencesPage( PreferencesInstance& instance )
{
   DefaultMasksShown_Flag.checkBox.SetText( "Show image masks by default" );
   DefaultMasksShown_Flag.item = &instance.imageWindow.defaultMasksShown;
   DefaultMasksShown_Flag.SetToolTip(
      "<p>Enable to show mask images represented over masked images when new masks are selected.</p>" );

   DefaultMaskMode_Set.label.SetText( "Default mask rendering mode" );
   DefaultMaskMode_Set.comboBox.AddItem( "Replace" );
   DefaultMaskMode_Set.comboBox.AddItem( "Multiply" );
   DefaultMaskMode_Set.comboBox.AddItem( "Red Overlay" );
   DefaultMaskMode_Set.comboBox.AddItem( "Green Overlay" );
   DefaultMaskMode_Set.comboBox.AddItem( "Blue Overlay" );
   DefaultMaskMode_Set.comboBox.AddItem( "Yellow Overlay" );
   DefaultMaskMode_Set.comboBox.AddItem( "Magenta Overlay" );
   DefaultMaskMode_Set.comboBox.AddItem( "Cyan Overlay" );
   DefaultMaskMode_Set.comboBox.AddItem( "Orange Overlay" );
   DefaultMaskMode_Set.comboBox.AddItem( "Violet Overlay" );
   DefaultMaskMode_Set.item = &instance.imageWindow.defaultMaskMode;
   DefaultMaskMode_Set.SetToolTip(
      "<p>Mask rendering modes define how masked pixels are represented on bitmap renditions of images. "
      "The <i>traditional</i> mode used by PixInsight has always been <i>Multiply</i>, which simply multiplies "
      "each pixel of a screen rendition by its mask image counterpart.</p>"
      "<p>Since PixInsight version 1.5.5, an additional set of <i>overlay</i> mask rendering modes is available, "
      "providing more accurate representations in several user-selectable colors. A new <i>Replace</i> mode has "
      "also been implemented to facilitate blinking comparisons between images and masks.</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( DefaultMasksShown_Flag );
   Page_Sizer.Add( DefaultMaskMode_Set );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void DefaultMaskSettingsPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.defaultMasksShown = from.imageWindow.defaultMasksShown;
   to.imageWindow.defaultMaskMode   = from.imageWindow.defaultMaskMode;
}

// ----------------------------------------------------------------------------

DefaultTransparencySettingsPreferencesPage::DefaultTransparencySettingsPreferencesPage( PreferencesInstance& instance )
{
   DefaultTransparencyMode_Set.label.SetText( "Default transparency rendering mode" );
   DefaultTransparencyMode_Set.comboBox.AddItem( "Hide transparencies" );
   DefaultTransparencyMode_Set.comboBox.AddItem( "Use the background brush" );
   DefaultTransparencyMode_Set.comboBox.AddItem( "Use an opaque color" );
   DefaultTransparencyMode_Set.item = &instance.imageWindow.defaultTransparencyMode;
   DefaultTransparencyMode_Set.SetToolTip(
      "<p>Transparency rendering modes define how transparent pixels are represented on screen image "
      "renditions. Pixel opacity is determined by the first alpha channel of the image (if one exists): "
      "black alpha values define completely transparent pixels; white alpha values define opaque pixels.</p>" );

   TransparencyBrush_Set.label.SetText( "Background brush" );
   TransparencyBrush_Set.comboBox.AddItem( "Small Chessboard Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Medium Chessboard Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Large Chessboard Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Small Cross Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Medium Cross Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Large Cross Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Small Diagonal Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Medium Diagonal Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Large Diagonal Pattern" );
   TransparencyBrush_Set.comboBox.AddItem( "Solid Brush" );
   TransparencyBrush_Set.item = &instance.imageWindow.transparencyBrush;

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( DefaultTransparencyMode_Set );
   Page_Sizer.Add( TransparencyBrush_Set );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void DefaultTransparencySettingsPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.defaultTransparencyMode = from.imageWindow.defaultTransparencyMode;
   to.imageWindow.transparencyBrush    = from.imageWindow.transparencyBrush;
}

// ----------------------------------------------------------------------------

MiscImageWindowSettingsPreferencesPage::MiscImageWindowSettingsPreferencesPage( PreferencesInstance& instance )
{
   ShowCaptionCurrentChannels_Flag.checkBox.SetText( "Show current channels on image window captions" );
   ShowCaptionCurrentChannels_Flag.item = &instance.imageWindow.showCaptionCurrentChannels;
   ShowCaptionCurrentChannels_Flag.SetToolTip(
      "<p>Write the names of currently selected display channels on image window captions.</p>" );

   ShowCaptionZoomRatios_Flag.checkBox.SetText( "Show zoom ratios on image window captions" );
   ShowCaptionZoomRatios_Flag.item = &instance.imageWindow.showCaptionZoomRatios;
   ShowCaptionZoomRatios_Flag.SetToolTip(
      "<p>Write current zoom ratios on image window captions.</p>" );

   ShowCaptionIdentifiers_Flag.checkBox.SetText( "Show view identifiers on image window captions" );
   ShowCaptionIdentifiers_Flag.item = &instance.imageWindow.showCaptionIdentifiers;
   ShowCaptionIdentifiers_Flag.SetToolTip(
      "<p>Write current view identifiers on image window captions.</p>" );

   ShowCaptionFullPaths_Flag.checkBox.SetText( "Show full file paths on image window captions" );
   ShowCaptionFullPaths_Flag.item = &instance.imageWindow.showCaptionFullPaths;
   ShowCaptionFullPaths_Flag.SetToolTip(
      "<p>Write full image file paths on image window captions.</p>" );

   ShowActiveSTFIndicators_Flag.checkBox.SetText( "Show active STF indicators" );
   ShowActiveSTFIndicators_Flag.item = &instance.imageWindow.showActiveSTFIndicators;
   ShowActiveSTFIndicators_Flag.SetToolTip(
      "<p>Show special indicators on view selectors for views with active screen transfer functions.</p>" );

   CursorTolerance_Integer.label.SetText( "Cursor tolerance (px)" );
   CursorTolerance_Integer.spinBox.SetRange( 1, 16 );
   CursorTolerance_Integer.item = &instance.imageWindow.cursorTolerance;
   CursorTolerance_Integer.SetToolTip(
      "<p>This is the maximum distance in pixels for the mouse cursor to trigger an action when it is clicked "
      "close to a sensitive point. For example, this is used to change cursor shapes when you place "
      "the mouse near preview rectangle edges.</p>" );

   TouchEvents_Flag.checkBox.SetText( "Touch events" );
   TouchEvents_Flag.item = &instance.imageWindow.touchEvents;
   TouchEvents_Flag.SetToolTip(
      "<p>Enable touch events from track pads and touch screens on image views.</p>" );

   PinchSensitivity_Real.label.SetText( "Pinch zoom sensitivity" );
   PinchSensitivity_Real.numericEdit.SetReal();
   PinchSensitivity_Real.numericEdit.SetRange( 0, 200 );
   PinchSensitivity_Real.numericEdit.SetPrecision( 3 );
   PinchSensitivity_Real.item = &instance.imageWindow.pinchSensitivity;
   PinchSensitivity_Real.SetToolTip(
      "<p>Minimum distance variation between two touch points to trigger a pinch zoom event on an image view. "
      "This value is expressed in normalized touch device coordinates, where one corresponds to the largest "
      "device dimension.</p>" );

   FastScreenRenditions_Flag.checkBox.SetText( "Use fast screen renditions" );
   FastScreenRenditions_Flag.item = &instance.imageWindow.fastScreenRenditions;
   FastScreenRenditions_Flag.enableControl = &FastScreenRenditionThreshold_Integer;
   FastScreenRenditions_Flag.SetToolTip(
      "<p>Use a fast pixel interpolation algorithm to generate reduced screen renditions of images for zoom "
      "ratios below 1:2. With this option enabled, screen renditions can be significantly faster, although "
      "slightly less accurate. Fast renditions are enabled by default. Use the <i>fast rendition threshold</i> "
      "option to control the minimum size of an image that triggers fast interpolation.</p>" );

   FastScreenRenditionThreshold_Integer.label.SetText( "Fast rendition threshold (Mpx)" );
   FastScreenRenditionThreshold_Integer.spinBox.SetRange( 0, 64 );
   FastScreenRenditionThreshold_Integer.spinBox.SetMinimumValueText( "<always>" );
   FastScreenRenditionThreshold_Integer.item = &instance.imageWindow.fastScreenRenditionThreshold;
   FastScreenRenditionThreshold_Integer.SetToolTip(
      "<p>Minimum size in megapixels to trigger usage of the fast interpolation algorithm for screen image "
      "renditions, when fast renditions are enabled (See the <i>Use fast screen renditions</i> option). "
      "The default value is 6 megapixels.</p>" );

   Default24BitScreenLUT_Flag.checkBox.SetText( "Use 24-bit Screen Transfer Function LUTs" );
   Default24BitScreenLUT_Flag.item = &instance.imageWindow.default24BitScreenLUT;
   Default24BitScreenLUT_Flag.SetToolTip(
      "<p>Use 24-bit lookup tables (LUT) to apply screen transfer functions (STF) by default, instead of "
      "16-bit LUTs. Linear HDR images usually require larger LUTs to discretize nonlinear STFs without "
      "posterization, especially on dark areas, where the STF applies more aggressive transformations.</p>"
      "<p>For most linear images and all stretched (nonlinear) images, 16-bit LUTs are normally sufficient. "
      "Enable the 24-bit LUT feature for deep linear data that cause posterization artifacts on screen "
      "renditions, such as HDR compositions. Note that you can enable or disable this feature on a per view "
      "basis; this setting refers to the default state for newly created images.</p>" );

   /*
   PaintOnScreen_Flag.checkBox.SetText( "Paint image renditions directly on the screen" );
   PaintOnScreen_Flag.item = &instance.imageWindow.paintOnScreen;
   PaintOnScreen_Flag.SetToolTip(
      "<p>This option is available exclusively under X11-based window managers (UNIX/Linux). It avoids the "
      "doubly-buffered graphics output normally used by PixInsight's GUI, which may accelerate image renditions "
      "slightly, so this feature is enabled by default.</p>"
      "<p>However, direct screen painting may conflict with some advanced X11 composition managers. Try "
      "disabling this option if you experience problems with window transparencies and other special desktop "
      "effects.</p>"
      "<p>On MS Windows and Mac OS X platforms, the state of this option is simply ignored.</p>" );
   */

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( ShowCaptionCurrentChannels_Flag );
   Page_Sizer.Add( ShowCaptionZoomRatios_Flag );
   Page_Sizer.Add( ShowCaptionIdentifiers_Flag );
   Page_Sizer.Add( ShowCaptionFullPaths_Flag );
   Page_Sizer.Add( ShowActiveSTFIndicators_Flag );
   Page_Sizer.Add( CursorTolerance_Integer );
   Page_Sizer.Add( TouchEvents_Flag );
   Page_Sizer.Add( PinchSensitivity_Real );
   Page_Sizer.Add( FastScreenRenditions_Flag );
   Page_Sizer.Add( FastScreenRenditionThreshold_Integer );
   Page_Sizer.Add( Default24BitScreenLUT_Flag );
   //Page_Sizer.Add( PaintOnScreen_Flag );  // ### Deprecated
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void MiscImageWindowSettingsPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.showCaptionCurrentChannels = from.imageWindow.showCaptionCurrentChannels;
   to.imageWindow.showCaptionZoomRatios      = from.imageWindow.showCaptionZoomRatios;
   to.imageWindow.showCaptionIdentifiers     = from.imageWindow.showCaptionIdentifiers;
   to.imageWindow.showCaptionFullPaths       = from.imageWindow.showCaptionFullPaths;
   to.imageWindow.showActiveSTFIndicators     = from.imageWindow.showActiveSTFIndicators;
   to.imageWindow.cursorTolerance            = from.imageWindow.cursorTolerance;
}

// ----------------------------------------------------------------------------

IdentifiersPreferencesPage::IdentifiersPreferencesPage( PreferencesInstance& instance )
{
   UseFileNamesAsImageIdentifiers_Flag.checkBox.SetText( "Use file names as image identifiers" );
   UseFileNamesAsImageIdentifiers_Flag.item = &instance.imageWindow.useFileNamesAsImageIdentifiers;
   UseFileNamesAsImageIdentifiers_Flag.SetToolTip(
      "<p>For images loaded from local or remote files, use file names to build meaningful image identifiers.<p>"
      "<p>Illegal identifier characters in file names (such as white spaces) are automatically replaced with "
      "underscores. When this option is disabled, the default image prefix is always used to generate image "
      "identifiers.</p>" );

   WorkspacePrefix_Id.label.SetText( "Workspace prefix" );
   WorkspacePrefix_Id.item = &instance.identifiers.workspacePrefix;
   WorkspacePrefix_Id.SetToolTip(
      "<p>This is the default identifier assigned to newly created workspaces.</p>" );

   ImagePrefix_Id.label.SetText( "Image prefix" );
   ImagePrefix_Id.item = &instance.identifiers.imagePrefix;
   ImagePrefix_Id.SetToolTip(
      "<p>This is the default identifier assigned to newly created images.</p>" );

   PreviewPrefix_Id.label.SetText( "Preview prefix" );
   PreviewPrefix_Id.item = &instance.identifiers.previewPrefix;
   PreviewPrefix_Id.SetToolTip(
      "<p>This is the default identifier assigned to newly created previews.</p>" );

   ProcessIconPrefix_Id.label.SetText( "Process icon prefix" );
   ProcessIconPrefix_Id.item = &instance.identifiers.processIconPrefix;
   ProcessIconPrefix_Id.SetToolTip(
      "<p>This is the default identifier assigned to newly created process icons.</p>" );

   ImageContainerIconPrefix_Id.label.SetText( "ImageContainer icon prefix" );
   ImageContainerIconPrefix_Id.item = &instance.identifiers.imageContainerIconPrefix;
   ImageContainerIconPrefix_Id.SetToolTip(
      "<p>This is the default identifier assigned to newly created ImageContainer icons.</p>" );

   ClonePostfix_Id.label.SetText( "Cloned image postfix" );
   ClonePostfix_Id.item = &instance.identifiers.clonePostfix;
   ClonePostfix_Id.SetToolTip(
      "<p>This suffix is automatically appended to the identifiers of duplicated images.</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( UseFileNamesAsImageIdentifiers_Flag );
   Page_Sizer.Add( WorkspacePrefix_Id );
   Page_Sizer.Add( ImagePrefix_Id );
   Page_Sizer.Add( PreviewPrefix_Id );
   Page_Sizer.Add( ProcessIconPrefix_Id );
   Page_Sizer.Add( ImageContainerIconPrefix_Id );
   Page_Sizer.Add( ClonePostfix_Id );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void IdentifiersPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.useFileNamesAsImageIdentifiers = from.imageWindow.useFileNamesAsImageIdentifiers;
   to.identifiers.workspacePrefix                = from.identifiers.workspacePrefix;
   to.identifiers.imagePrefix                    = from.identifiers.imagePrefix;
   to.identifiers.previewPrefix                  = from.identifiers.previewPrefix;
   to.identifiers.processIconPrefix              = from.identifiers.processIconPrefix;
   to.identifiers.imageContainerIconPrefix       = from.identifiers.imageContainerIconPrefix;
   to.identifiers.clonePostfix                   = from.identifiers.clonePostfix;
}

// ----------------------------------------------------------------------------

StringsPreferencesPage::StringsPreferencesPage( PreferencesInstance& instance )
{
   NewImageCaption_String.label.SetText( "New image caption" );
   NewImageCaption_String.item = &instance.identifiers.newImageCaption;
   NewImageCaption_String.SetToolTip(
      "<p>This text is used to facilitate identifying newly created images.</p>" );

   NoViewsAvailableText_String.label.SetText( "No views available text" );
   NoViewsAvailableText_String.item = &instance.identifiers.noViewsAvailableText;
   NoViewsAvailableText_String.SetToolTip(
      "<p>This text is used to indicate that there are no views available in a given context.</p>" );

   NoViewSelectedText_String.label.SetText( "No view selected text" );
   NoViewSelectedText_String.item = &instance.identifiers.noViewSelectedText;
   NoViewSelectedText_String.SetToolTip(
      "<p>This text is used to indicate that no view has been selected in a given context.</p>" );

   NoPreviewsAvailableText_String.label.SetText( "No previews available text" );
   NoPreviewsAvailableText_String.item = &instance.identifiers.noPreviewsAvailableText;
   NoPreviewsAvailableText_String.SetToolTip(
      "<p>This text is used to indicate that there are no previews available in a given context.</p>" );

   NoPreviewSelectedText_String.label.SetText( "No preview selected text" );
   NoPreviewSelectedText_String.item = &instance.identifiers.noPreviewSelectedText;
   NoPreviewSelectedText_String.SetToolTip(
      "<p>This text is used to indicate that no preview has been selected in a given context.</p>" );

   BrokenLinkText_String.label.SetText( "Broken link text" );
   BrokenLinkText_String.item = &instance.identifiers.brokenLinkText;
   BrokenLinkText_String.SetToolTip(
      "<p>This text is used to indicate that a stored masking relation is no longer valid.</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( NewImageCaption_String );
   Page_Sizer.Add( NoViewsAvailableText_String );
   Page_Sizer.Add( NoViewSelectedText_String );
   Page_Sizer.Add( NoPreviewsAvailableText_String );
   Page_Sizer.Add( NoPreviewSelectedText_String );
   Page_Sizer.Add( BrokenLinkText_String );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void StringsPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.identifiers.newImageCaption         = from.identifiers.newImageCaption;
   to.identifiers.noViewsAvailableText    = from.identifiers.noViewsAvailableText;
   to.identifiers.noViewSelectedText      = from.identifiers.noViewSelectedText;
   to.identifiers.noPreviewsAvailableText = from.identifiers.noPreviewsAvailableText;
   to.identifiers.noPreviewSelectedText   = from.identifiers.noPreviewSelectedText;
   to.identifiers.brokenLinkText          = from.identifiers.brokenLinkText;
}

// ----------------------------------------------------------------------------

ParallelProcessingPreferencesPage::ParallelProcessingPreferencesPage( PreferencesInstance& instance )
{
   EnableParallelProcessing_Flag.checkBox.SetText( "Enable parallel processing" );
   EnableParallelProcessing_Flag.item = &instance.process.enableParallelProcessing;
   EnableParallelProcessing_Flag.SetToolTip(
      "<p>Enable this option to allow PixInsight to use multiple threads and processors for all tasks. This is "
      "a global setting; to enable/disable multiple threads for specific tasks, see the following options.</p>"
      "<p>If this option is disabled, PixInsight will use only a single processor for all tasks, regardless "
      "of the states of other specific settings.</p>"
      "<p><b>* Warning *</b> Disabling this option may degrade overall performance dramatically.</p>" );

   EnableParallelCoreRendering_Flag.checkBox.SetText( "Enable parallel core image rendering" );
   EnableParallelCoreRendering_Flag.item = &instance.process.enableParallelCoreRendering;
   EnableParallelCoreRendering_Flag.SetToolTip(
      "<p>Enable multiple threads for image renditions performed by the core PixInsight application.</p>"
      "<p><b>* Warning *</b> If you disable this option, you may experience a significant performance "
      "degradation in PixInsight's graphical user interface.</p>" );

   EnableParallelCoreColorManagement_Flag.checkBox.SetText( "Enable parallel core color management" );
   EnableParallelCoreColorManagement_Flag.item = &instance.process.enableParallelCoreColorManagement;
   EnableParallelCoreColorManagement_Flag.SetToolTip(
      "<p>Enable multiple threads for color management transformations performed by the core PixInsight "
      "application.</p>"
      "<p><b>* Warning *</b> If you disable this option, you may experience a significant performance "
      "degradation in PixInsight's graphical user interface.</p>" );

   EnableParallelModuleProcessing_Flag.checkBox.SetText( "Enable parallel module processing" );
   EnableParallelModuleProcessing_Flag.item = &instance.process.enableParallelModuleProcessing;
   EnableParallelModuleProcessing_Flag.SetToolTip(
      "<p>Enable multiple threads for image processing tasks performed by external modules.</p>"
      "<p><b>* Warning *</b> If you disable this option, you may experience a significant performance "
      "degradation in all installed processes.</p>" );

   EnableThreadCPUAffinity_Flag.checkBox.SetText( "Enable thread CPU affinity" );
   EnableThreadCPUAffinity_Flag.item = &instance.process.enableThreadCPUAffinity;
   EnableThreadCPUAffinity_Flag.SetToolTip(
      "<p>Allow setting the CPU affinity of running threads. This is a global flag that affects "
      "threads created by the core application as well as all external modules.</p>"
      "<p>The affinity of a thread defines the set of logical processors on which the thread is "
      "eligible to run. Thread affinity allows to improve execution speed by restricting each thread "
      "to run on a separate processor. This prevents the performance cost caused by the cache "
      "invalidation that occurs when a process ceases to execute on a processor and then restarts "
      "execution on a different one.</p>"
      "<p><b>Note</b> The state of this setting is only fetched upon application startup. Changes "
      "to this setting require a restart of the PixInsight core application to take effect.</p>"
      "<p><b>Note</b> Currently this option only works on Linux and Windows platforms. It does not "
      "work on Mac OS X and FreeBSD. Support for all platforms will be implemented in a future "
      "version of PixInsight.</p>" );

   MaxModuleThreadPriority_Set.label.SetText( "Maximum module thread priority" );
   MaxModuleThreadPriority_Set.minValue = ThreadPriority::Lowest; // ### Warning: DO NOT change these numbers
   MaxModuleThreadPriority_Set.comboBox.AddItem( "Lowest" );
   MaxModuleThreadPriority_Set.comboBox.AddItem( "Low" );
   MaxModuleThreadPriority_Set.comboBox.AddItem( "Normal" );
   MaxModuleThreadPriority_Set.comboBox.AddItem( "High" );
   MaxModuleThreadPriority_Set.comboBox.AddItem( "Highest" );
   MaxModuleThreadPriority_Set.comboBox.AddItem( "Time Critical" );
   MaxModuleThreadPriority_Set.item = &instance.process.maxModuleThreadPriority;
   MaxModuleThreadPriority_Set.SetToolTip(
      "<p>This is the maximum thread priority allowed for threads created by external modules. <i>Time critical</i> "
      "allows modules to create real-time priority threads, which are seldomly used, but sometimes necessary "
      "to carry out performance-critical operations. Lowest priority will force all installed modules to use very "
      "low priority threads, which may have a strong impact over processing performance.</p>"
      "<p><b>* Warning *</b> Forcing low thread priorities will degrade processing performance. You should only "
      "change this value if you are sure of what you are doing.</p>" );

   MaxProcessors_Integer.label.SetText( "Maximum number of processors used" );
   MaxProcessors_Integer.spinBox.SetRange( 1, int_max );
   MaxProcessors_Integer.item = &instance.process.maxProcessors;
   MaxProcessors_Integer.SetToolTip(
      "<p>This is the maximum number of processors (physical and logical) that PixInsight is allowed "
      "to use. Selecting just one processor will effectively disable parallel processing globally.</p>"
      "<p><b>* Warning *</b> Reducing the maximum number of allowed processors too much will degrade processing "
      "and interface performance dramatically. Change this value only if you are sure of what you are doing.</p>" );

   UseAllAvailableProcessors_CheckBox.SetText( "Allow using all available processors" );
   UseAllAvailableProcessors_CheckBox.OnClick( (Button::click_event_handler)&ParallelProcessingPreferencesPage::__UseAllAvailableProcessors_ButtonClick, *this );
   UseAllAvailableProcessors_CheckBox.SetToolTip(
      "<p>Check this option to allow PixInsight to use all available processors (physical and logical).</p>" );

   UseAllAvailableProcessors_CheckBox.SetButtonColor( MaxProcessors_Integer.BackgroundColor() );
   MaxProcessors_Integer.sizer.Add( UseAllAvailableProcessors_CheckBox );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( EnableParallelProcessing_Flag );
   Page_Sizer.Add( EnableParallelCoreRendering_Flag );
   Page_Sizer.Add( EnableParallelCoreColorManagement_Flag );
   Page_Sizer.Add( EnableParallelModuleProcessing_Flag );
   Page_Sizer.Add( EnableThreadCPUAffinity_Flag );
   Page_Sizer.Add( MaxModuleThreadPriority_Set );
   Page_Sizer.Add( MaxProcessors_Integer );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void ParallelProcessingPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.process.enableParallelProcessing          = from.process.enableParallelProcessing;
   to.process.enableParallelCoreRendering       = from.process.enableParallelCoreRendering;
   to.process.enableParallelCoreColorManagement = from.process.enableParallelCoreColorManagement;
   to.process.enableParallelModuleProcessing    = from.process.enableParallelModuleProcessing;
   to.process.maxModuleThreadPriority           = from.process.maxModuleThreadPriority;
   to.process.maxProcessors                     = from.process.maxProcessors;
}

void ParallelProcessingPreferencesPage::PerformAdditionalUpdates()
{
   int maxProcessors = *MaxProcessors_Integer.item;
   MaxProcessors_Integer.label.Enable( maxProcessors != int_max );
   MaxProcessors_Integer.spinBox.Enable( maxProcessors != int_max );
   UseAllAvailableProcessors_CheckBox.SetChecked( maxProcessors == int_max );
}

void ParallelProcessingPreferencesPage::__UseAllAvailableProcessors_ButtonClick( Button& /*sender*/, bool checked )
{
   *MaxProcessors_Integer.item = checked ?
            int_max :
            PixInsightSettings::GlobalInteger( "System/NumberOfProcessors" );
   MaxProcessors_Integer.Synchronize();
   MaxProcessors_Integer.label.Enable( !checked );
   MaxProcessors_Integer.spinBox.Enable( !checked );
}

// ----------------------------------------------------------------------------

MiscProcessingPreferencesPage::MiscProcessingPreferencesPage( PreferencesInstance& instance )
{
   BackupPSMFiles_Flag.checkBox.SetText( "Backup PSM/XPSM files" );
   BackupPSMFiles_Flag.item = &instance.process.backupFiles;
   BackupPSMFiles_Flag.SetToolTip(
      "<p>Make backups of existing disk image files when saving XPSM and PSM (Process Serialization Module) files.</p>" );

   GenerateScriptComments_Flag.checkBox.SetText( "Generate instance script comments" );
   GenerateScriptComments_Flag.item = &instance.process.generateScriptComments;
   GenerateScriptComments_Flag.SetToolTip(
      "<p>Enable this option to allow inclusion of module-defined comments in automatically generated scripts.</p>" );

   MaxConsoleLines_Integer.label.SetText( "Maximum number of console lines" );
   MaxConsoleLines_Integer.spinBox.SetRange( 0, int_max );
   MaxConsoleLines_Integer.spinBox.SetMinimumValueText( "<unlimited>" );
   MaxConsoleLines_Integer.item = &instance.process.maxConsoleLines;
   MaxConsoleLines_Integer.SetToolTip(
      "<p>Maximum number of text lines that can be stored by the process console. When the number of text "
      "lines in the console exceeds from this limit, old lines are removed from the beginning of the console "
      "before writing new ones. This greatly improves efficiency of console text output, especially during "
      "long batch processes, and also improves normal processes after a long time without clearing the console.</p>"
      "<p>Specify a value of zero to disable this limit and allow an unlimited number of text lines in the console. "
      "The default value is 1000 lines.</p>" );

   ConsoleDelay_Integer.label.SetText( "Console window delay (ms)" );
   ConsoleDelay_Integer.spinBox.SetRange( 0, 60000 );
   ConsoleDelay_Integer.spinBox.SetMinimumValueText( "<disabled>" );
   ConsoleDelay_Integer.item = &instance.process.consoleDelay;
   ConsoleDelay_Integer.SetToolTip(
      "<p>This is the time elapsed (in milliseconds) before the Process Console window is shown when a process "
      "is executed and the console is hidden. Set this value to zero to disable the delayed console feature. "
      "Note that this setting has no effect if the console has been floated as a top-level window; it only "
      "affects to the console when it is docked as an AutoHide window.</p>" );

   AutoSavePSMPeriod_Integer.label.SetText( "AutoSave PSM period (s)" );
   AutoSavePSMPeriod_Integer.spinBox.SetRange( 0, 600 );
   AutoSavePSMPeriod_Integer.item = &instance.process.autoSavePSMPeriod;
   AutoSavePSMPeriod_Integer.SetToolTip(
      "<p>This is the time interval in seconds between PSM auto-save events. Set it to zero to disable the "
      "PSM AutoSave feature.</p>"
      "<p><b>* Warning *</b> Ensure that you have a good reason to disable PSM AutoSave, or to enter a large "
      "interval here, because PSM AutoSave has virtually zero impact on performance and can save your "
      "work in the event of a system crash.</p>" );

   AlertOnProcessCompleted_Flag.checkBox.SetText( "Alert on process completion" );
   AlertOnProcessCompleted_Flag.item = &instance.process.alertOnProcessCompleted;
   AlertOnProcessCompleted_Flag.SetToolTip(
      "<p>When this option is enabled, the PixInsight core application generates a visible <i>alert</i> when "
      "a running process completes its execution. On Windows and most X11 (UNIX/Linux) desktops, an alert causes "
      "the application icon to flash intermittently on the task bar. On Mac OS X, an alert causes the application "
      "icon to start bouncing on the dock. This option is disabled by default.</p>" );

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( BackupPSMFiles_Flag );
   Page_Sizer.Add( GenerateScriptComments_Flag );
   Page_Sizer.Add( MaxConsoleLines_Integer );
   Page_Sizer.Add( ConsoleDelay_Integer );
   Page_Sizer.Add( AutoSavePSMPeriod_Integer );
   Page_Sizer.Add( AlertOnProcessCompleted_Flag );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void MiscProcessingPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.process.backupFiles             = from.process.backupFiles;
   to.process.generateScriptComments  = from.process.generateScriptComments;
   to.process.consoleDelay            = from.process.consoleDelay;
   to.process.autoSavePSMPeriod       = from.process.autoSavePSMPeriod;
   to.process.alertOnProcessCompleted = from.process.alertOnProcessCompleted;
}

// ----------------------------------------------------------------------------

TransparencyColorsPreferencesPage::TransparencyColorsPreferencesPage( PreferencesInstance& instance )
{
   TransparencyBrushForegroundColor_Color.label.SetText( "Transparency brush foreground color" );
   TransparencyBrushForegroundColor_Color.item = &instance.imageWindow.transparencyBrushForegroundColor;

   TransparencyBrushBackgroundColor_Color.label.SetText( "Transparency brush background color" );
   TransparencyBrushBackgroundColor_Color.item = &instance.imageWindow.transparencyBrushBackgroundColor;

   DefaultTransparencyColor_Color.label.SetText( "Default color for solid transparency brush" );
   DefaultTransparencyColor_Color.item = &instance.imageWindow.defaultTransparencyColor;

   Page_Sizer.SetSpacing( 4 );
   Page_Sizer.Add( TransparencyBrushForegroundColor_Color );
   Page_Sizer.Add( TransparencyBrushBackgroundColor_Color );
   Page_Sizer.Add( DefaultTransparencyColor_Color );
   Page_Sizer.AddStretch();

   SetSizer( Page_Sizer );
}

void TransparencyColorsPreferencesPage::TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
{
   to.imageWindow.transparencyBrushForegroundColor = from.imageWindow.transparencyBrushForegroundColor;
   to.imageWindow.transparencyBrushBackgroundColor = from.imageWindow.transparencyBrushBackgroundColor;
   to.imageWindow.defaultTransparencyColor         = from.imageWindow.defaultTransparencyColor;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PreferencesInterface::GUIData::GUIData( PreferencesInterface& w ) : window( w )
{
   InitializeCategories();

   CategorySelection_TreeBox.HideHeader();
   CategorySelection_TreeBox.SetNumberOfColumns( 1 );
   CategorySelection_TreeBox.EnableRootDecoration();
   CategorySelection_TreeBox.EnableAlternateRowColor();
   CategorySelection_TreeBox.SetFixedWidth( w.Font().Width( String( 'M', 32 ) ) );
   CategorySelection_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&PreferencesInterface::__Category_CurrentNodeUpdated, w );

   int index = 0;
   for ( category_list::iterator i = categories.Begin(); i != categories.End(); ++i )
   {
      CategoryNode* node = new CategoryNode( CategorySelection_TreeBox, index++ );
      node->SetText( 0, i->Title() );
      node->SetIcon( 0, i->Icon( w.DisplayPixelRatio() ) );
   }

   pcl::Font titleFont = w.Font();
   titleFont.SetBold();
   Title_Label.SetFont( titleFont );

   Title_Sizer.SetMargin( 4 );
   Title_Sizer.Add( Title_Label );

   Title_Control.SetObjectId( "IWSectionBar" );
   Title_Control.SetSizer( Title_Sizer );
   Title_Control.SetScaledMinWidth( 390 );

   CategoryStack_Sizer.SetMargin( 4 );
   CategoryStack_Sizer.Add( Title_Control );
   CategoryStack_Sizer.AddSpacing( 4 );
   CategoryStack_Sizer.AddStretch();

   CategoryStack_Control.SetSizer( CategoryStack_Sizer );
   CategoryStack_Control.SetScaledMinHeight( 500 );

   TopRow_Sizer.SetSpacing( 12 );
   TopRow_Sizer.Add( CategorySelection_TreeBox );
   TopRow_Sizer.Add( CategoryStack_Control, 100 );

   LoadCurrentSettings_PushButton.SetText( "Load current settings" );
   LoadCurrentSettings_PushButton.OnClick( (Button::click_event_handler)&PreferencesInterface::__LoadSettings_ButtonClick, w );

   LoadCurrentPageSettings_PushButton.SetText( "Load current page settings" );
   LoadCurrentPageSettings_PushButton.OnClick( (Button::click_event_handler)&PreferencesInterface::__LoadSettings_ButtonClick, w );

   LoadDefaultPageSettings_PushButton.SetText( "Load default page settings" );
   LoadDefaultPageSettings_PushButton.OnClick( (Button::click_event_handler)&PreferencesInterface::__LoadSettings_ButtonClick, w );

   LoadSettings_Sizer.SetSpacing( 6 );
   LoadSettings_Sizer.Add( LoadCurrentSettings_PushButton );
   LoadSettings_Sizer.AddStretch();
   LoadSettings_Sizer.Add( LoadCurrentPageSettings_PushButton );
   LoadSettings_Sizer.Add( LoadDefaultPageSettings_PushButton );

   //

   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.SetMargin( 6 );
   Global_Sizer.Add( TopRow_Sizer );
   Global_Sizer.Add( LoadSettings_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetMinSize();
}

PreferencesInterface::GUIData::~GUIData()
{
   categories.Destroy();
}

// ----------------------------------------------------------------------------

PreferencesCategoryPage* PreferencesInterface::GUIData::PageByIndex( int index )
{
   if ( index < 0 || index >= NumberOfPages() )
      return 0;

   PreferencesCategoryPage* page = categories[index].page;
   if ( page == 0 )
   {
      page = categories[index].CreatePage( window.instance );

      CategoryStack_Sizer.Insert( 2, *page );

      page->Hide();

      for ( global_item_ref_list::iterator i = globalItemControls.Begin();
            i != globalItemControls.End();
            ++i )
      {
         if ( (*i)->Parent() == *page )
            (*i)->Synchronize();
      }

      page->PerformAdditionalUpdates();
   }

   return page;
}

void PreferencesInterface::GUIData::HideAllPages()
{
   for ( category_list::iterator i = categories.Begin(); i != categories.End(); ++i )
      i->HidePage();
}

void PreferencesInterface::GUIData::PerformAllPageAdditionalUpdates()
{
   for ( category_list::iterator i = categories.Begin(); i != categories.End(); ++i )
      i->PerformAdditionalUpdates();
}

void PreferencesInterface::GUIData::InitializeCategories()
{
   categories.Destroy();
   categories.Add( new MainWindowPreferencesCategory );
   categories.Add( new ResourcesPreferencesCategory );
   categories.Add( new GUIEffectsPreferencesCategory );
   categories.Add( new FileIOPreferencesCategory );
   categories.Add( new DirectoriesAndNetworkPreferencesCategory );
   categories.Add( new DefaultImageResolutionPreferencesCategory );
   categories.Add( new DefaultMaskSettingsPreferencesCategory );
   categories.Add( new DefaultTransparencySettingsPreferencesCategory );
   categories.Add( new MiscImageWindowSettingsPreferencesCategory );
   categories.Add( new IdentifiersPreferencesCategory );
   categories.Add( new StringsPreferencesCategory );
   categories.Add( new ParallelProcessingPreferencesCategory );
   categories.Add( new MiscProcessingPreferencesCategory );
   categories.Add( new TransparencyColorsPreferencesCategory );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PreferencesInterface.cpp - Released 2015/10/08 11:24:39 UTC
