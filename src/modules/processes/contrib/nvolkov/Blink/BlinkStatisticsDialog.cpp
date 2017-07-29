//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 01.02.02.0244
// ----------------------------------------------------------------------------
// BlinkStatisticsDialog.cpp - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2017 Nikolay Volkov
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#include "BlinkStatisticsDialog.h"
#include "BlinkInterface.h"

#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StdStatus.h>

#define ftb m_parent->GUI->Files_TreeBox

#define DIALOG_TITLE "Statistics"

namespace pcl
{

// ----------------------------------------------------------------------------

BlinkStatisticsDialog::BlinkStatisticsDialog( BlinkInterface* parent ) : Dialog(), m_parent( parent ), m_outputDir()
{
   // File_ComboBox
   File_ComboBox.SetToolTip( "<p>Select Image to see FITS keywords.</p>" );
   File_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&BlinkStatisticsDialog::__TreeBox_ItemSelected, *this );

   // Keyword_TreeBox
   Keyword_TreeBox.SetNumberOfColumns( 3 );
   Keyword_TreeBox.DisableRootDecoration();
   Keyword_TreeBox.EnableAlternateRowColor();
   Keyword_TreeBox.SetToolTip( "<p>Enable to include the keyword in the report.</p>");
   Keyword_TreeBox.SetHeaderText( 0, "Name" );
   Keyword_TreeBox.SetHeaderText( 1, "Value" );
   Keyword_TreeBox.SetHeaderText( 2, "Comment" );
   Keyword_TreeBox.SetColumnWidth( 0, 100 );
   Keyword_TreeBox.SetColumnWidth( 1, 200 );

   // Preferences Line1
   Range0_RadioButton.SetText( "[0.0,1.0]" );
   Range0_RadioButton.SetToolTip( "Normalized real range" );
   Range0_RadioButton.OnClick( (Button::click_event_handler)&BlinkStatisticsDialog::__RangeMode_Click, *this );

   Range1_RadioButton.SetText( "[0,65535]" );
   Range1_RadioButton.SetToolTip( "16-bit integer range" );
   Range1_RadioButton.OnClick( (Button::click_event_handler)&BlinkStatisticsDialog::__RangeMode_Click, *this );

   Digits_Label.SetText( "Precision:" );
   Digits_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   Digits0_SpinBox.SetRange( 1, 15 );
   Digits0_SpinBox.SetToolTip( "Fractional digits for range [0.0,1.0]" );

   Digits1_SpinBox.SetRange( 0, 10 );
   Digits1_SpinBox.SetToolTip( "Fractional digits for range [0,65535]" );

   SortChannels_CheckBox.SetText( "Sort by channels" );

   StatCropMode_CheckBox.SetText( "Crop" );
   StatCropMode_CheckBox.SetToolTip( "<p>Generate statistics only for the pixels inside the green rectangle.</p>" );

   WriteStatsFile_CheckBox.SetText( "Write text file" );
   WriteStatsFile_CheckBox.SetToolTip( "<p>Write a text file with the computed image statistics.</p>" );
   WriteStatsFile_CheckBox.OnClick( (Button::click_event_handler)&BlinkStatisticsDialog::__Button_Click, *this );

   // Preferences Line2
   OutputDir_Label.SetText("Output folder:");
   OutputDir_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
   OutputDir_Edit.SetToolTip(
      "<p>This is the directory (aka folder) where the statistics report file will be written "
         "(when the <i>write text file</i> option is enabled).</p>"
      "<p>If this field is left blank, the output file will be written to the directory of the first file.</p>" );
   OutputDir_Edit.OnMouseDoubleClick( (Control::mouse_event_handler)&BlinkStatisticsDialog::__Control_DoubleClick, *this );
   OutputDir_Edit.OnEditCompleted( (Edit::edit_event_handler)&BlinkStatisticsDialog::__Edit_Completed, *this );
   OutputDir_Edit.SetText( m_outputDir );

   OutputDir_SelectButton.SetIcon( Bitmap( ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDir_SelectButton.SetToolTip( "<p>Select the output directory.</p>" );
   OutputDir_SelectButton.OnClick( (Button::click_event_handler)&BlinkStatisticsDialog::__Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&BlinkStatisticsDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&BlinkStatisticsDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   StatPreferencesLine1_Sizer.Add( Range0_RadioButton );
   StatPreferencesLine1_Sizer.AddSpacing( 16 );
   StatPreferencesLine1_Sizer.Add( Range1_RadioButton );
   StatPreferencesLine1_Sizer.AddSpacing( 16 );
   StatPreferencesLine1_Sizer.Add( Digits_Label );
   StatPreferencesLine1_Sizer.AddSpacing( 4 );
   StatPreferencesLine1_Sizer.Add( Digits0_SpinBox );
   StatPreferencesLine1_Sizer.Add( Digits1_SpinBox );
   StatPreferencesLine1_Sizer.AddSpacing( 16 );
   StatPreferencesLine1_Sizer.Add( SortChannels_CheckBox );
   StatPreferencesLine1_Sizer.AddSpacing( 16 );
   StatPreferencesLine1_Sizer.Add( StatCropMode_CheckBox );
   StatPreferencesLine1_Sizer.AddSpacing( 16 );
   StatPreferencesLine1_Sizer.Add( WriteStatsFile_CheckBox );
   StatPreferencesLine1_Sizer.AddSpacing( 64 );
   StatPreferencesLine1_Sizer.AddStretch();

   StatPreferencesLine2_Sizer.Add( OutputDir_Label );
   StatPreferencesLine2_Sizer.Add( OutputDir_Edit );
   StatPreferencesLine2_Sizer.Add( OutputDir_SelectButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( File_ComboBox );
   Global_Sizer.Add( Keyword_TreeBox );
   Global_Sizer.Add( StatPreferencesLine1_Sizer );
   Global_Sizer.Add( StatPreferencesLine2_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   SetScaledMinWidth( 600 );

   SetWindowTitle( DIALOG_TITLE );
   Init();

   OnReturn( (Dialog::return_event_handler)&BlinkStatisticsDialog::__Dialog_Return, *this );
}

void BlinkStatisticsDialog::Init()
{
#if debug
   Console().WriteLn(String().Format( "<br>Init() Start" ) );
   Console().WriteLn(String().Format( "<br>Total filesData records in memory: %u", m_parent->m_blink.m_filesData.Length() ) );
#endif

   // Default state
   SortChannels_CheckBox.SetChecked( m_parent->m_sortChannels );     // mode: sort by channel(true) or not(false)?
   StatCropMode_CheckBox.SetChecked( m_parent->m_cropMode );         // true = Statistics only for Green rectangle
   WriteStatsFile_CheckBox.SetChecked( m_parent->m_writeStatsFile ); // true = Write text file
   Digits0_SpinBox.SetValue( m_parent->m_digits0_1 );                // digits after dot for range 0-1.
   Digits1_SpinBox.SetValue( m_parent->m_digits0_65535 );            // digits after dot for range 0-65535.
   if ( m_parent->m_range0_65535 )
      Range1_RadioButton.Check();   // Checked  >> use range range 1-65535. UnCheked >> use range range 0-1.
   else
      Range0_RadioButton.Check();   // UnCheked >> use range range 1-65535. Checked  >> use range range 0-1.

   __RangeMode_Click( Range0_RadioButton, Range0_RadioButton.IsChecked() );  // initialize Digits0_SpinBox & Digits1_SpinBox

   OutputDir_Edit.Enable( m_parent->m_writeStatsFile );
   OutputDir_SelectButton.Enable( m_parent->m_writeStatsFile );

   for ( int i = 0 ; i < ftb.NumberOfChildren(); i++ ) // Add Files to File_ComboBox
   {
      const int fileNumber = BlinkInterface::FileNumberGet( i ); //extract file # from GUI TreeBox

#if debug
      Console().WriteLn( String().Format( "<br>Add to File_ComboBox file #%u", fileNumber ) );
      Console().WriteLn( m_parent->m_blink.m_filesData[fileNumber].m_filePath );
#endif

      File_ComboBox.AddItem( m_parent->m_blink.m_filesData[fileNumber].m_filePath );
   }

   File_ComboBox.SetCurrentItem( m_parent->m_blink.m_currentImage );             // point File_ComboBox to currentImage

   __TreeBox_ItemSelected( File_ComboBox, m_parent->m_blink.m_currentImage );   // Fill Keyword_TreeBox by keywords from currentImage

   for ( int i = 0; i < Keyword_TreeBox.NumberOfColumns()-1; i++ )   //Adjust width only Columns: Name and Value
      Keyword_TreeBox.AdjustColumnWidthToContents( i );

#if debug
   Console().WriteLn( String().Format( "Init() End" ) );
#endif
}

// ----------------------------------------------------------------------------
// Statistics Processing
// ----------------------------------------------------------------------------

StringList BlinkStatisticsDialog::FindKeywords( int fileIndex )
{
   const FITSKeywordArray& fileKeys = m_parent->m_blink.m_filesData[fileIndex].m_keywords;

   StringList keys;

   for ( int key = 0; key < Keyword_TreeBox.NumberOfChildren(); key++ )
      if ( Keyword_TreeBox.Child( key )->IsChecked() )
      {
         String st;
         String keyName = Keyword_TreeBox.Child( key )->Text( 0 );
         int k = -1;
         if ( key < int( fileKeys.Length() ) )
            if ( String( fileKeys[key].name ) == keyName )
               k = key;                                        // FITS header have same structure, no need to scan FITS Header
         if ( k < 0 )                                          // try find keyword by scaning FITS Header
            for ( k = 0; k < int( fileKeys.Length() ); k++ )
               if ( String( fileKeys[k].name ) == keyName )
                  break;
         if ( k < int( fileKeys.Length() ) )                   // keyword found
         {
            if ( fileKeys[k].IsNumeric() )
            {
               double value;
               fileKeys[k].GetNumericValue( value );
               keys.Add( String( value ) );                    // Store Numeric value
            }
            else
               keys.Add( fileKeys[k].StripValueDelimiters() ); // Store String value
         }
         else
            keys.Add( "none" );                                // keyword not found
      }

   return keys;
}

void BlinkStatisticsDialog::ConvertStatisticsToText()
{
#if debug
   Console().WriteLn( "ConvertStatisticsToText() Start" );
   Console().Flush();
#endif

   String fileName = "Statistics";

   bool Channel[] = { false, false, false, false };
   const char channelName[] = { 'R', 'G', 'B', 'L' };
   const int range( Range1_RadioButton.IsChecked() ? 65535 : 1 );
   const int precision( Range1_RadioButton.IsChecked() ? Digits1_SpinBox.Value() : Digits0_SpinBox.Value() );
   const int width = precision + 1 + (Range1_RadioButton.IsChecked() ? 5 : 1);

   /*
    * Prepare headers
    */

   const char* h0[] = { "FileName", "#", "#", "Ch", "Exposure", "Mean", "Median", "MAD", "AvgDev", "StdDev", "Minimum", "Maximum" };
   StringList h;
   h.Add( h0[0] );
   h.Add( h0[1] );
   h.Add( h0[2] );
   h.Add( h0[3] );
   h.Add( String().Format( "%9s", h0[4] ) );
   for ( int i = 5; i < int( ItemsInArray( h0 ) ); i++ )
      h.Add( String().Format( "%*s", width, h0[i] ) );

   for( int key = 0; key < Keyword_TreeBox.NumberOfChildren(); key++ )
      if ( Keyword_TreeBox.Child( key )->IsChecked() )
         h.Add( Keyword_TreeBox.Child( key )->Text( 0 ) );  // add FITS keyword Name to header

   ftb.ShowHeader();                                        // show header in main dialog treebox
   ftb.DisableHeaderSorting();
   ftb.SetNumberOfColumns( int( h.Length() ) + 1 );         // add more column to treeebox for show statistics, +1 for last blank column
   for ( int i = 0; i < int( h.Length() ); i++ )
      ftb.SetHeaderText( i , h[i] );                        // set column name of FileThreeBox. i+1 for file #
   ftb.SetHeaderText( int( h.Length() ), String() );        // last column for padding

   ftb.SetHeaderAlignment( 0, TextAlign::Left );
//	ftb.SetHeaderAlignment( 1, TextAlign::Left );
   for ( int i = 3; i < int( h.Length() ); i++ )
      ftb.SetHeaderAlignment( i, TextAlign::Center );

   m_stat.Add( h );                                         // write header to statistics table

   /*
    * Convert to text
    */

#if debug
   Console().WriteLn( "Convert to text" );
   Console().Flush();
#endif

   for ( int i = 0 ; i < ftb.NumberOfChildren(); i++ ) // circle by open files; prepare m_stat table
   {
      if ( !ftb.Child( i )->IsChecked() )
         continue; // skip unchecked file

      const int fileNumber = BlinkInterface::FileNumberGet( i ); //extract file # from GUI TreeBox

      BlinkInterface::FileData& fd = m_parent->m_blink.m_filesData[fileNumber];

      StringList keys = FindKeywords( fileNumber );

      for ( int c = 0; c < fd.m_info.numberOfChannels; c++ ) // for every color chanel
      {
         StringList st;

         // Add filename
         st.Add( File::ExtractName( fd.m_filePath ) );

         // Add fileNumber 1 and 2
         st.Add( String( fileNumber ) );
         st.Add( BlinkInterface::RowToStringFileNumber( i ) );

         // Add filter name
         if ( fd.m_info.numberOfChannels == 1 )
         {
            Channel[3] = true;
            st.Add( String( ' ' ) + channelName[3] );
         }
         else
         {
            Channel[c] = true;
            st.Add( String( ' ' ) + channelName[c] );
         }

         // add Exposure
         st.Add( String().Format( "%9.3f", fd.m_options.exposure ) );

         // Add Statistics
         const ImageStatistics& is = fd.m_statReal[c];
         st.Add( String().Format( "%*.*f", width, precision, is.Mean()    * range ) );
         st.Add( String().Format( "%*.*f", width, precision, is.Median()  * range ) );
         st.Add( String().Format( "%*.*f", width, precision, is.MAD()     * range * 1.4826 ) );
         st.Add( String().Format( "%*.*f", width, precision, is.AvgDev()  * range * 1.2533 ) );
         st.Add( String().Format( "%*.*f", width, precision, is.StdDev()  * range ) );
         st.Add( String().Format( "%*.*f", width, precision, is.Minimum() * range ) );
         st.Add( String().Format( "%*.*f", width, precision, is.Maximum() * range ) );

         // Add selected FITS keywords
         st.Add( keys );

         // Store to main table
         m_stat.Add( st );

         // Send to main GUI. Show only the first channel (gray or red)
         if ( c == 0 )
            for ( int column = 1; column < int( st.Length() ); column++ )
            {
               ftb.Child( i )->SetText( column, st[column].Trimmed() );
               //if ( column > 0 )
                  ftb.Child( i )->SetAlignment( column, TextAlign::Right );
            }
      }
   }

   if ( m_stat.Length() == 1 )
   {
      Console().WriteLn( "No images have been selected" );
      return;
   }

   /*
    * Correct column widths
    */
#if debug
   Console().WriteLn( "Correcting column widths" );
   Console().Flush();
#endif

   for ( int column = 0; column < ftb.NumberOfColumns()-1; column++ )
   {
      // calculate maximum width of the Row
      int width = 0;
      for ( int row = 0; row < int( m_stat.Length() ); row++ )
         width = Max( width, int( m_stat[row][column].Length() ) );

      // apply correction width
      for ( int row = 0; row < int( m_stat.Length() ); row++ )
#if debug
         m_stat[row][column].Append( '_', width - m_stat[row][column].Length() );
#else
         m_stat[row][column].Append( ' ', width - m_stat[row][column].Length() );
#endif

      // adjust main GUI
      ftb.AdjustColumnWidthToContents( column );
   }
   ftb.ShowColumn( 2 ); // column with String fileNumber
   ftb.EnableHeaderSorting();

   /*
    * Prepare output file
    */
#if debug
   Console().WriteLn( "Prepare output file" );
   Console().Flush();
#endif

   String fileDir = OutputDir_Edit.Text();
   if ( fileDir.IsEmpty() )   // output dir not selected = write to dir of Images[0]
   {
      const String p = m_parent->m_blink.m_filesData[0].m_filePath;
      fileDir = File::ExtractDrive( p ) + File::ExtractDirectory( p ) ;
   }

   if ( !File::DirectoryExists( fileDir ) )
      File::CreateDirectory( fileDir );

   const String p = fileDir + "/" + fileName + ".txt";
   String tp = p;
   for ( int u = 1; File::Exists( tp ) ; u++ )
   {
      String n = String( u );
      n.Prepend( '0', 4 - n.Length() );
      tp = File::AppendToName( p, '-' + n );
   }

   /*
    * Write to output file and/or console
    */
#if debug
   Console().WriteLn( "Write output file" );
#endif

   bool writeFile = WriteStatsFile_CheckBox.IsChecked();
   File f;
   if ( writeFile )
      f.CreateForWriting( tp );

   bool sort = SortChannels_CheckBox.IsChecked();
   for ( int c = sort ? 0 : 3; c < 4; c++ )
   {
#if debug
      Console().WriteLn( "Channel " + String ( c ) + ": " + String( Channel[c] ) );
#endif

      if ( !Channel[c] && sort )
         continue;

      Console().WriteLn();
      for ( int j = 0; j < int( m_stat.Length() ); j++ )
      {
         if ( ( m_stat[j][3].Trimmed() != channelName[c] ) && ( j > 0 ) && sort )
            continue;

         IsoString out;
         for ( int k = 0; k < int( m_stat[j].Length() ); k++ )
         {
            if ( k == 1 ) continue; // skip real fileNumber. String fileNumber enought
            if ( k > 0 )
               out.Append( ' ' );
            out.Append( m_stat[j][k].ToIsoString() );
            if ( writeFile )
               out.Append( '\t' ); // best for excel
         }

         Console().WriteLn( out );
         if ( writeFile )
            f.OutTextLn( out );
      }
   }
   if ( writeFile )
   {
      f.Close();
      Console().NoteLn( "* Saved to " + tp );
   }

#if debug
   Console().WriteLn( "ConvertStatisticsToText() Exit" );
   Console().Flush();
#endif
}

void BlinkStatisticsDialog::PrepareData()
{
#if debug
   Console().WriteLn( "PrepareData() Start" );
   Console().WriteLn( "StatCropMode_CheckBox: " + String( StatCropMode_CheckBox.IsChecked() ) );
#endif

   BlinkInterface::BlinkData& blink = m_parent->m_blink;

   if ( !blink.m_screen.IsNull() && StatCropMode_CheckBox.IsChecked() )  // set m_rect to CurrentView Rectangle
   {
      if ( blink.m_screen.SelectedPreview().IsNull() )
         m_rect = blink.m_screen.ViewportToImage( blink.m_screen.VisibleViewportRect() ).RoundedToInt();
      else
         m_rect = blink.m_screen.PreviewRect( blink.m_screen.SelectedPreview().Id() );
   }
   else
      m_rect = blink.m_screenRect;      // set m_rect full size

#if debug
   Console().WriteLn( String().Format( "m_rect %i, %i, %i, %i", m_rect.x0, m_rect.y0, m_rect.x1, m_rect.y1 ) );
   Console().WriteLn( "Crop mode: " + String( StatCropMode_CheckBox.IsChecked() ) );
#endif

   for ( int i = 0; i < ftb.NumberOfChildren(); i++ )
   {
      if ( !ftb.Child( i )->IsChecked() ) continue;    // skip unchecked file
#if debug
      Console().WriteLn();
#endif

      const int fileNumber = BlinkInterface::FileNumberGet( i ); //extract file # from GUI TreeBox

      BlinkInterface::FileData& fd = blink.m_filesData[fileNumber];

      if ( fd.m_statRealRect != m_rect && !fd.m_statReal.IsEmpty() )  // wrong geometry?
      {
#if debug
         Console().WriteLn( "delete old data in statReal" );
#endif

         fd.m_statReal.Clear();                             // Destroy old data
      }

      if ( !fd.m_statReal.IsEmpty() )
         continue;                                          // statReal size and data is good

      if ( !StatCropMode_CheckBox.IsChecked() || m_rect == blink.m_screenRect )   // Full size mode or Crop mode size == full size
         if ( fd.m_isSTFStatisticsEqualToReal )             // if statSTF hold good data
         {                                                  // Copy Statistics from statSTF
            Console().WriteLn( "<br>Using image statistics in memory: " + fd.m_filePath  );
            ProcessEvents();
            for ( int c = 0; c < int( fd.m_statSTF.Length() ); c++ )
               fd.m_statReal.Add( fd.m_statSTF[c] );

#if debug
            Console().WriteLn( "Copy Statistics from statSTF sucesful" );
#endif
            fd.m_statRealRect = m_rect;                     // save new geometry
            continue;
         }

      DImage img;

      if ( fd.m_isRealPixelData )                           // use blink_image PixelData
      {
         Console().WriteLn( "<br>Use Image in memory: " + fd.m_filePath );
         ProcessEvents();
         img.Assign( *fd.m_image );                         // Convert & Copy blink_image to DImage
      }
      else                                                  // load Real Image from file
      {
         ImageVariant v( &img );
         if ( !BlinkInterface::LoadImage( v, fd.m_filePath ) )
            throw CatchedException();
      }

      StandardStatus callback;
      StatusMonitor monitor;
      monitor.SetCallback( &callback );
      monitor.Initialize( "<end><cbr>Calculating image statistics", img.NumberOfNominalChannels() );
      ProcessEvents();

      if ( m_rect != blink.m_screenRect )                   // Crop mode
         img.SelectRectangle( m_rect );

      for ( int i = 0; i < img.NumberOfNominalChannels(); i++, ++monitor )
      {
         img.SelectChannel( i );
         ImageStatistics S;
         S.EnableRejection();
         S.SetRejectionLimits( 0.0, 1.0 );
         S.DisableSumOfSquares();
         S.DisableBWMV();
         S.DisablePBMV();
         S << img;
         fd.m_statReal.Add( S );
      }

#if debug
      Console().WriteLn( "Calculate Statistics successful" );
#endif

      // In full size mode copy real statistics to statSTF for future use
      if ( m_rect == blink.m_screenRect )
      {
         if ( !fd.m_statSTF.IsEmpty() )
         {
#if debug
            Console().WriteLn( "delete old data in statSTF" );
#endif
            fd.m_statSTF.Clear();                           // Destroy old data
         }
#if debug
         Console().WriteLn( "Copy Statistics from statReal to statSTF" );
#endif
         for ( int c = 0; c < int( fd.m_statReal.Length() ); c++ )
            fd.m_statSTF.Add( fd.m_statReal[c] );

         fd.m_isSTFStatisticsEqualToReal = true;
      }

      fd.m_statRealRect = m_rect;                           // save new geometry
   }

#if debug
   Console().WriteLn( "PrepareData() Exit" );
#endif
}

void BlinkStatisticsDialog::ExecuteStatistics()
{
   Console().Show();
   Console().WriteLn();

   // Disable GUI during calculation
   Disable();
   m_parent->Disable();
   SetCursor( StdCursor::ArrowWait );
   m_parent->SetCursor( StdCursor::ArrowWait );

   // This is the main processing routine
   PrepareData();
   ConvertStatisticsToText();

   // Enable GUI
   Enable();
   m_parent->Enable();
   SetCursor( StdCursor::Arrow );
   m_parent->SetCursor( StdCursor::Arrow );
}

void BlinkStatisticsDialog::SelectOutputDir()
{
   String dir = BlinkInterface::SelectDirectory( String( DIALOG_TITLE ) + ": Select Output Directory", OutputDir_Edit.Text() );
   if ( !dir.IsEmpty() )
      m_outputDir = dir;
   OutputDir_Edit.SetText( m_outputDir );
}

// ----------------------------------------------------------------------------
// Event handlers
// ----------------------------------------------------------------------------

void BlinkStatisticsDialog::__TreeBox_ItemSelected( ComboBox& sender, int itemIndex )
{
   Keyword_TreeBox.DisableUpdates();
   Keyword_TreeBox.Clear();

   FITSKeywordArray& keywords = m_parent->m_blink.m_filesData[itemIndex].m_keywords;

   for ( int i = 0; i < int( keywords.Length() ); i++ )
   {
      TreeBox::Node* node = new TreeBox::Node( Keyword_TreeBox ); // add new line in Keyword_TreeBox
      node->SetText( 0, keywords[i].name );
      node->SetText( 1, keywords[i].value );
#if debug
      Console().Write(String().Format( "keywords[i].comment.Length() %u", keywords[i].comment.Length() ) );
      Console().WriteLn(" :"+ keywords[i].comment );
#endif
      node->SetText( 2, keywords[i].comment );
      node->Uncheck();
   }

   if ( m_parent->m_blink.m_currentImage != itemIndex )
   {
      m_parent->m_blink.m_currentImage = itemIndex;
      m_parent->m_blink.Update();
   }

   Keyword_TreeBox.EnableUpdates();
}

void BlinkStatisticsDialog::__Control_DoubleClick( Control& sender, const Point& /*pos*/, unsigned /*buttons*/, unsigned /*modifiers*/ )
{
   if ( sender == OutputDir_Edit )
      SelectOutputDir();
}

void BlinkStatisticsDialog::__Edit_Completed( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == OutputDir_Edit )
   {
      if ( !text.IsEmpty() && !File::DirectoryExists( text ) )
      {
         MessageBox( "The specified output directory does not exist: " + text, DIALOG_TITLE ).Execute();
         text = m_outputDir;
      }
      else
      {
         m_outputDir = text; // Clear or store new path
      }
   }
   sender.SetText( text );
}

void BlinkStatisticsDialog::__RangeMode_Click( Button& /*sender*/, bool /*checked*/ )
{
   if ( Range0_RadioButton.IsChecked() )
   {
      Digits1_SpinBox.Hide();	// Hide Fractional digits for range [0 - 65535]
      Digits0_SpinBox.Show();
   }
   else
   {
      Digits0_SpinBox.Hide();	// Hide Fractional digits for range [0.0 - 1.0]
      Digits1_SpinBox.Show();
   }
}

void BlinkStatisticsDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
   {
      ExecuteStatistics();
      Ok();
   }
   else if ( sender == Cancel_PushButton )
      Cancel();
   else if ( sender == WriteStatsFile_CheckBox )
   {
      OutputDir_Edit.Enable( checked );
      OutputDir_SelectButton.Enable( checked );
   }
   else if ( sender == OutputDir_SelectButton )
      SelectOutputDir();
}

void BlinkStatisticsDialog::__Dialog_Return( Dialog& sender, int retVal )
{
   // Transfer settings to main GUI
   m_parent->m_sortChannels   = SortChannels_CheckBox.IsChecked();
   m_parent->m_cropMode       = StatCropMode_CheckBox.IsChecked();
   m_parent->m_digits0_1      = Digits0_SpinBox.Value();
   m_parent->m_digits0_65535  = Digits1_SpinBox.Value();
   m_parent->m_range0_65535   = Range1_RadioButton.IsChecked();
   m_parent->m_writeStatsFile = WriteStatsFile_CheckBox.IsChecked();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BlinkStatisticsDialog.cpp - Released 2017-05-02T09:43:01Z
