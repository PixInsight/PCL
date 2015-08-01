//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 01.02.01.0166
// ----------------------------------------------------------------------------
// BlinkVideoDialog.cpp - Released 2015/07/31 11:49:49 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "BlinkVideoDialog.h"
#include "BlinkInterface.h"

#include <pcl/Console.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h> // for ProcessEvents()
#include <pcl/StdStatus.h>
#include <pcl/FileFormat.h>

#include <time.h> // for Timestamp() implementation

#define DIALOG_TITLE "Make Video"

namespace pcl
{

// ----------------------------------------------------------------------------

static IsoString Timestamp()
{
   time_t t0 = ::time( 0 );
   const tm* t = ::localtime( &t0 );
   return IsoString().Format( "[%d-%02d-%02d %02d:%02d:%02d] ",
                              t->tm_year+1900, t->tm_mon+1, t->tm_mday,
                              t->tm_hour, t->tm_min, t->tm_sec );
}

// ----------------------------------------------------------------------------

void BlinkVideoDialog::Init()
{
#if debug
   Console().WriteLn( String().Format( "<br>Init() Start" ) );
#endif

   Program_Edit.SetText( m_parent->m_program );
   Arguments_Edit.SetText( m_parent->m_arguments );
   OutputDir_Edit.SetText( m_parent->m_frameOutputDir );
   FrameExtension_Edit.SetText( m_parent->m_frameExtension );
   DeleteVideoFrames_CheckBox.SetChecked( m_parent->m_deleteVideoFrames );

   EnableGUI();

#if debug
   Console().WriteLn( String().Format( "Init() End" ) );
#endif
}

// ----------------------------------------------------------------------------
// Processing
// ----------------------------------------------------------------------------

inline void BlinkVideoDialog::DeleteFrames()
{
   if( !m_parent->m_deleteVideoFrames )
      return;

   m_framesDone.Remove( m_parent->m_frameExtension );

   Console().WriteLn( "<end><cbr><br>* Blink: Deleting video frames:" );
   while ( m_frameCount > 0 )
   {
      String f = m_parent->m_frameOutputDir
               + '/'
               + m_parent->m_outputFileName
               + String().Format( "%05d", m_frameCount-- )
               + m_parent->m_frameExtension;
      if ( File::Exists( f ) )
      {
         try
         {
            Console().WriteLn( "<end><cbr><raw>" + f + "</raw>" );
            File::Remove( f );
         }
         catch(...)
         {
            Console().CriticalLn( "<end><cbr><raw>*** Couldn't delete file: " + f + "</raw>" );
            throw CatchedException();
         }
      }

      Module->ProcessEvents();
   }
}

struct FrameData
{
   String filePath;
   int    fileNumber;
};

typedef Array<FrameData> frame_data_list;

void BlinkVideoDialog::CreateFrames()
{
#define v m_parent->m_blink.m_screen.CurrentView()

   FileFormat outputFormat( m_parent->m_frameExtension, false/*toRead*/, true/*toWrite*/ );

   m_frameCount = 0;   // total file counter in the sequence

   frame_data_list frames;
   bool allExist = true;
   for ( int row = 0; row < m_parent->GUI->Files_TreeBox.NumberOfChildren(); row++ )
      if ( m_parent->GUI->Files_TreeBox.Child( row )->IsChecked() )
      {
         FrameData frame;
         frame.filePath = m_parent->m_frameOutputDir
                        + '/'
                        + m_parent->m_outputFileName
                        + String().Format( "%05d", ++m_frameCount )
                        + m_parent->m_frameExtension;
         frame.fileNumber = m_parent->FileNumberGet( row );
         frames.Add( frame );

         allExist &= File::Exists( frame.filePath );
      }

   if ( allExist )
      if ( m_framesDone.Contains( m_parent->m_frameExtension ) )
         return;

   m_framesDone.Remove( m_parent->m_frameExtension );
   m_frameCount = 0;

   Rect r( m_parent->GetCropRect() );

   Console().Show();

   View::stf_list stf( 4 );
   bool isSTF = v.AreScreenTransferFunctionsEnabled();
   if ( isSTF ) // Get STF from blink screen to apply HT to every image before saving it
      v.GetScreenTransferFunctions( stf );

   Console().WriteLn( "<end><cbr><br>* Blink: Generating video frames:" );

   int numberOfChannels = v.Image()->NumberOfNominalChannels();
   for ( frame_data_list::const_iterator i = frames.Begin(); i != frames.End(); ++i )
   {
      Console().WriteLn( "<end><cbr><raw>" + i->filePath + "</raw>" );
      FileFormatInstance outputFile( outputFormat );
      if ( !outputFile.Create( i->filePath ) )
         throw CatchedException();

      blink_image image( *m_parent->m_blink.m_filesData[i->fileNumber].m_image ); // create temp image
      image.SelectRectangle( r ); // crop it
      if ( isSTF ) //if STF enabled apply HT to all channels
      {
         for ( int c = 0; c < numberOfChannels; c++ ) // for every channel
         {
            image.SelectChannel( c );
            stf[c] >> image; //apply HT
         }
         image.ResetChannelRange();
      }

      if ( !outputFile.WriteImage( image ) )
         throw CatchedException();

      outputFile.Close();

      ++m_frameCount;

      Module->ProcessEvents();
   }

   Console().WriteLn( String().Format( "<end><cbr><br>* %i video frames written to:", m_frameCount ) );
   Console().WriteLn( m_parent->m_frameOutputDir );

   m_framesDone.Add( m_parent->m_frameExtension );
}

void BlinkVideoDialog::DisableGUI( bool d )
{
   Load_PushButton.Disable( d );
   Save_PushButton.Disable( d );
   Run_PushButton.Disable( d );
   Program_Edit.Disable( d );
   Arguments_Edit.Disable( d );
   OutputDir_Edit.Disable( d );
   OutputDir_SelectButton.Disable( d );
   FrameExtension_Edit.Disable( d );
   DeleteVideoFrames_CheckBox.Disable( d );
   m_parent->Disable( d );
   Cancel_PushButton.SetText( d ? "Cancel" : "Close" );
   StdIn_Edit.Enable( d );
   StdOut_TextBox.Insert( "<end>" );
}

void BlinkVideoDialog::ExecuteVideoEncoder()
{
   if ( m_parent->m_frameOutputDir.IsEmpty() )
   {
      pcl::MessageBox( "You must specify an output directory.",
                       "Blink", StdIcon::Error ).Execute();
      return;
   }

   if ( !File::DirectoryExists( m_parent->m_frameOutputDir ) )
   {
      pcl::MessageBox( "No such directory:\n\n" + m_parent->m_frameOutputDir,
                       "Blink", StdIcon::Error ).Execute();
      return;
   }

   if ( m_parent->m_frameExtension.IsEmpty() )
   {
      pcl::MessageBox( "You must specify a file extension for generated video frames.",
                       "Blink", StdIcon::Error ).Execute();
      return;
   }

   if ( !m_parent->m_frameExtension.StartsWith( '.' ) )
   {
      m_parent->m_frameExtension.Prepend( '.' );
      FrameExtension_Edit.SetText( m_parent->m_frameExtension );
   }

   Console().Show();

   DisableGUI(); // Disable GUI during executing
   StdOut_TextBox.Clear();

   CreateFrames(); // prepare frame sequence

   m_videoEncoder.SetWorkingDirectory( m_parent->m_frameOutputDir );

   m_command = m_parent->m_program.Trimmed() + ' ' + m_parent->m_arguments.Trimmed();
   m_timestamp = Timestamp();
   Console().WriteLn( "<end><cbr><br><raw>* Blink: Running external process: " + m_command + "</raw>" );
   StdOut_TextBox.Focus();
   try
   {
      m_videoEncoder.Start( m_command );
   }
   catch( ... )
   {
      EnableGUI();
      throw;
   }
}

bool BlinkVideoDialog::TerminateVideoEncoder()
{
   if ( !m_videoEncoder.IsRunning() )
      return true;

   if ( pcl::MessageBox( "Terminate running video encoder program ?",
                         "Blink", StdIcon::Question, StdButton::No, StdButton::Yes ).Execute() == StdButton::No )
      return false;

   if ( !m_videoEncoder.IsRunning() )
      return true;

   m_videoEncoder.Terminate();

   for ( ;; )
   {
      for ( int i = 0; i < 15; ++i ) // wait for 1.5 seconds
         if ( m_videoEncoder.WaitForFinished( 100 ) )
            return true;

      int r = pcl::MessageBox( "The process is not responding. Do you want to force termination ?",
                               "Blink", StdIcon::Question, StdButton::No, StdButton::Yes, StdButton::Cancel ).Execute();

      if ( !m_videoEncoder.IsRunning() )
         return r != StdButton::Cancel;

      if ( r == StdButton::Yes )
      {
         m_videoEncoder.Kill();
         return true;
      }
   }
}

void BlinkVideoDialog::WriteLogFile()
{
   if ( m_command.IsEmpty() || m_timestamp.IsEmpty() )
   {
      pcl::MessageBox( "No process has been executed - cannot create an empty log file.",
                       "Blink", StdIcon::Error ).Execute();
      return;
   }

   String filePath = m_parent->m_frameOutputDir
                   + '/'
                   + m_parent->m_outputFileName
                   + ".log";
   IsoString command = m_command.ToUTF8();
   IsoString consoleText = StdOut_TextBox.Text().ToUTF8();
   File f;
   f.CreateForWriting( filePath );
   f.OutTextLn( m_timestamp );
   f.OutTextLn( command );
   if ( !consoleText.IsEmpty() )
   {
      f.OutTextLn( "" );
      f.OutText( consoleText );
      if ( !consoleText.EndsWith( '\n' ) )
         f.OutTextLn( "" );
   }
   f.Close();
   Console().WriteLn( "<end><cbr><br>* Blink: Log file created:" );
   Console().WriteLn( filePath );
}

// ----------------------------------------------------------------------------
// Event handlers
// ----------------------------------------------------------------------------

void BlinkVideoDialog::AddLog( const String& s )
{
   StdOut_TextBox.Insert( "<end><raw>" + s + "</raw><flush>" );
   StdOut_TextBox.Focus();
   Module->ProcessEvents();
}

void BlinkVideoDialog::__Process_Finished( ExternalProcess& p, int exitCode, bool exitOk )
{
   Console().WriteLn( "<end><cbr><br>* Blink: External process finished." );
   DeleteFrames();
   EnableGUI();
}

void BlinkVideoDialog::__Process_Started( ExternalProcess& p)
{
   //StdIn_Edit.Focus();
}

void BlinkVideoDialog::__Process_StandardOutputDataAvailable( ExternalProcess& p )
{
   ByteArray d( p.Read() );
   AddLog( String::UTF8ToUTF16( reinterpret_cast<const char*>( d.Begin() ), 0, d.Length() ) );
}

void BlinkVideoDialog::__Process_Error( ExternalProcess& p, ExternalProcess::error_code error )
{
   static const char *str[] = { "Failed to start", "Crashed", "Timed out", "Read error", "Write error", "Unknown error" };
   String s = "*** Failed to execute external process: " + String( str[error] );
   AddLog( s );
   Console().CriticalLn( s );
   EnableGUI();
}

void BlinkVideoDialog::__Edit_Completed( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == Program_Edit )
   {
      m_parent->m_program = text;
   }
   else if ( sender == Arguments_Edit )
   {
      m_parent->m_arguments = text;
   }
   else if ( sender == OutputDir_Edit )
   {
      m_parent->m_frameOutputDir = text;
   }
   else if ( sender == FrameExtension_Edit )
   {
      if ( !text.StartsWith( '.' ) )
         text.Prepend( '.' );
      m_parent->m_frameExtension = text;
   }
   else if ( sender == StdIn_Edit )
   {
      if ( !text.IsEmpty() )
         if ( m_videoEncoder.IsRunning() )
         {
            AddLog( text + '\n' );
            m_videoEncoder.Write( text );
            m_videoEncoder.CloseStandardInput();
            text.Clear();
         }
   }

   sender.SetText( text );
}

void BlinkVideoDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == Load_PushButton )
   {
      m_parent->LoadVideoSettings();
      Init();
   }
   if ( sender == Save_PushButton )
   {
      m_parent->SaveVideoSettings();
   }
   else if ( sender ==  WriteLog_PushButton )
   {
      WriteLogFile();
   }
   else if ( sender == Run_PushButton )
   {
      ExecuteVideoEncoder();
   }
   else if ( sender == Cancel_PushButton )
   {
      if ( TerminateVideoEncoder() )
         Cancel();
   }
   else if ( sender == OutputDir_SelectButton )
   {
      String dir( m_parent->SelectDirectory( "Blink/CropToVideo: Select Output Directory" ) );
      if ( !dir.IsEmpty() )
         m_parent->m_frameOutputDir = dir;
      Init();
   }
   else if ( sender == DeleteVideoFrames_CheckBox )
   {
      m_parent->m_deleteVideoFrames = checked;
   }
}

void BlinkVideoDialog::__Dialog_Close( Control& sender, bool& allowClose )
{
   allowClose = TerminateVideoEncoder();
}

void BlinkVideoDialog::__Dialog_Return( Dialog& sender, int retVal )
{
   m_parent->Enable();
}

BlinkVideoDialog::BlinkVideoDialog( BlinkInterface* parent ) :
   Dialog(),
   m_parent( parent ),
   m_frameCount( 0 ),
   m_framesDone(),
   m_command(),
   m_timestamp()
{
   pcl::Font fnt = this->Font();
   int labelWidth1 = fnt.Width( String( "Frame extension:" ) + 'T' );

   //

   StdOut_TextBox.SetScaledMinSize( 640, 400 );
   StdOut_TextBox.SetReadOnly();
   StdOut_TextBox.SetObjectId( "CWConsole" ); // inherit font and colors from PixInsight's terminal
   StdOut_TextBox.SetStyleSheet( ScaledStyleSheet(
      "pi--TextBox {"
         "font-family: DejaVu Sans Mono, Monospace;"
         "font-size: 8pt;"
      "}" ) );

   StdIn_Edit.OnEditCompleted( (Edit::edit_event_handler)&BlinkVideoDialog::__Edit_Completed, *this );

   const char* programToolTip = "<p>External video encoder application.</p>";

   Program_Label.SetText( "Program:" );
   Program_Label.SetFixedWidth( labelWidth1 );
   Program_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Program_Label.SetToolTip( programToolTip );

   Program_Edit.SetToolTip( programToolTip );
   Program_Edit.OnEditCompleted( (Edit::edit_event_handler)&BlinkVideoDialog::__Edit_Completed, *this );

   const char* argumentsToolTip = "<p>Command-line arguments passed to the video encoder program.</p>";

   Arguments_Label.SetText( "Arguments:" );
   Arguments_Label.SetFixedWidth( labelWidth1 );
   Arguments_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Arguments_Label.SetToolTip( argumentsToolTip );

   Arguments_Edit.SetToolTip( argumentsToolTip );
   Arguments_Edit.OnEditCompleted( (Edit::edit_event_handler)&BlinkVideoDialog::__Edit_Completed, *this );

   const char* outputDirToolTip = "<p>This is the directory (or folder) where temporary files and video files will be written.</p>";

   OutputDir_Label.SetText( "Output directory:" );
   OutputDir_Label.SetFixedWidth( labelWidth1 );
   OutputDir_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   OutputDir_Label.SetToolTip( outputDirToolTip );

   OutputDir_Edit.SetToolTip( outputDirToolTip );
   OutputDir_Edit.OnEditCompleted( (Edit::edit_event_handler)&BlinkVideoDialog::__Edit_Completed, *this );

   OutputDir_SelectButton.SetIcon( Bitmap( ScaledResource( ":/browser/select-file.png" ) ) );
   OutputDir_SelectButton.SetScaledFixedSize( 19, 19 );
   OutputDir_SelectButton.SetToolTip( "<p>Select the output directory.</p>" );
   OutputDir_SelectButton.OnClick( (Button::click_event_handler)&BlinkVideoDialog::__Button_Click, *this );

   const char* frameExtensionToolTip = "<p>This file extension determines the format used to generate video frame images</p>"
      "<p>The selected output format must be a <i>writable</i> one, that is, a format able to generate image files.</p>";

   FrameExtension_Label.SetText( "Frame extension:" );
   FrameExtension_Label.SetFixedWidth( labelWidth1 );
   FrameExtension_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   FrameExtension_Label.SetToolTip( frameExtensionToolTip );

   FrameExtension_Edit.SetToolTip( frameExtensionToolTip );
   FrameExtension_Edit.OnEditCompleted( (Edit::edit_event_handler)&BlinkVideoDialog::__Edit_Completed, *this );

   DeleteVideoFrames_CheckBox.SetText( "Delete video frames" );
   DeleteVideoFrames_CheckBox.SetToolTip( "<p>Delete all video frame files after running the video encoder program.</p>" );
   DeleteVideoFrames_CheckBox.OnClick( (Button::click_event_handler)&BlinkVideoDialog::__Button_Click, *this );

   Load_PushButton.SetText( "Load" );
   Load_PushButton.SetToolTip( "<p>Load default video generation parameters.</p>" );
   Load_PushButton.OnClick( (Button::click_event_handler)&BlinkVideoDialog::__Button_Click, *this );

   Save_PushButton.SetText( "Save" );
   Save_PushButton.SetToolTip( "<p>Save current video generation parameters as default.</p>" );
   Save_PushButton.OnClick( (Button::click_event_handler)&BlinkVideoDialog::__Button_Click, *this );

   WriteLog_PushButton.SetText( "Write Log" );
   WriteLog_PushButton.SetToolTip( "<p>Create a plain text file with the current parameters and console output.</p>" );
   WriteLog_PushButton.OnClick( (Button::click_event_handler)&BlinkVideoDialog::__Button_Click, *this );

   Run_PushButton.SetText( "Run" );
   Run_PushButton.SetDefault();
   Run_PushButton.SetCursor( StdCursor::Checkmark );
   Run_PushButton.OnClick( (Button::click_event_handler)&BlinkVideoDialog::__Button_Click, *this );

   //Cancel_PushButton.SetText( "Close" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&BlinkVideoDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddUnscaledSpacing( labelWidth1 + LogicalPixelsToPhysical( 4 ) );
   Buttons_Sizer.Add( Load_PushButton );
   Buttons_Sizer.Add( Save_PushButton );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( WriteLog_PushButton );
   Buttons_Sizer.Add( Run_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   PreferencesLine1_Sizer.SetSpacing( 4 );
   PreferencesLine1_Sizer.Add( Program_Label );
   PreferencesLine1_Sizer.Add( Program_Edit, 100 );

   PreferencesLine2_Sizer.SetSpacing( 4 );
   PreferencesLine2_Sizer.Add( Arguments_Label );
   PreferencesLine2_Sizer.Add( Arguments_Edit, 100 );

   PreferencesLine3_Sizer.SetSpacing( 4 );
   PreferencesLine3_Sizer.Add( OutputDir_Label );
   PreferencesLine3_Sizer.Add( OutputDir_Edit, 100 );
   PreferencesLine3_Sizer.Add( OutputDir_SelectButton );

   PreferencesLine4_Sizer.SetSpacing( 4 );
   PreferencesLine4_Sizer.Add( FrameExtension_Label );
   PreferencesLine4_Sizer.Add( FrameExtension_Edit );
   PreferencesLine4_Sizer.AddSpacing( 4 );
   PreferencesLine4_Sizer.Add( DeleteVideoFrames_CheckBox );
   PreferencesLine4_Sizer.AddStretch();

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( StdOut_TextBox, 100 );
   Global_Sizer.Add( StdIn_Edit );
   Global_Sizer.Add( PreferencesLine1_Sizer );
   Global_Sizer.Add( PreferencesLine2_Sizer );
   Global_Sizer.Add( PreferencesLine3_Sizer );
   Global_Sizer.Add( PreferencesLine4_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   SetWindowTitle( DIALOG_TITLE );

   m_videoEncoder.OnStarted( (ExternalProcess::process_event_handler)&BlinkVideoDialog::__Process_Started, *this );
   m_videoEncoder.OnFinished( (ExternalProcess::process_exit_event_handler)&BlinkVideoDialog::__Process_Finished, *this );
   m_videoEncoder.OnStandardOutputDataAvailable( (ExternalProcess::process_event_handler)&BlinkVideoDialog::__Process_StandardOutputDataAvailable, *this );
   m_videoEncoder.OnStandardErrorDataAvailable( ( ExternalProcess::process_event_handler)&BlinkVideoDialog::__Process_StandardOutputDataAvailable, *this );
   m_videoEncoder.OnError( ( ExternalProcess::process_error_event_handler)&BlinkVideoDialog::__Process_Error, *this );

   OnClose( (Control::close_event_handler)&BlinkVideoDialog::__Dialog_Close, *this );
   OnReturn( (Dialog::return_event_handler)&BlinkVideoDialog::__Dialog_Return, *this );

   Init();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BlinkVideoDialog.cpp - Released 2015/07/31 11:49:49 UTC
