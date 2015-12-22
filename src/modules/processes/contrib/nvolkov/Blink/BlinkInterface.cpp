//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 01.02.01.0211
// ----------------------------------------------------------------------------
// BlinkInterface.cpp - Released 2015/12/18 08:55:08 UTC
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

#include "BlinkInterface.h"
#include "BlinkProcess.h"
#include "BlinkStatisticsDialog.h"
#include "BlinkVideoDialog.h"

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/MessageBox.h>
#include <pcl/Random.h>
#include <pcl/Settings.h>
#include <pcl/StdStatus.h>  // for progress monitor
#include <pcl/Version.h>    // for PixInsightVersion
#include <pcl/View.h>

#define WHEEL_STEP_ANGLE   PixInsightSettings::GlobalInteger( "ImageWindow/WheelStepAngle" )

namespace pcl
{

// ----------------------------------------------------------------------------

#include "BlinkIcon.xpm"
#include "ScreenTransferFunctionIcon.xpm"
#include "HistogramTransformationIcon.xpm"

// ----------------------------------------------------------------------------

static int PreviewSize = 202;

// ----------------------------------------------------------------------------

#if debug
int BlinkInterface::BlinkData::m_total = 0;
int BlinkInterface::FileData::m_total = 0;
#endif

// ----------------------------------------------------------------------------

/*
 * Blinking speed in seconds
 */
static const float g_blinkingDelaySecs[] = { 0.0F, 0.05F, 0.1F, 0.2F, 0.3F, 0.5F, 0.7F , 1.0F, 2.0F , 5.0F };
//static const char *g_framePerSecond[] =    { "25",  "20", "10",  "5", "3" , "2" , "1.5", "1" , "0.5", "0.2" }; // FramePerSsecond for avi converter

/*
 * AutoSTF parameters
 */
static const double g_stfShadowsClipping = -2.80 * 1.4826; // in MAD units from the median
static const double g_stfTargetBackground = 0.25;

// ----------------------------------------------------------------------------

template <class P>
static bool LoadImage_P( GenericImage<P>& image, const String& filePath )
{
   Console().WriteLn( "<end><cbr><br>* Read file: " + filePath );
   FileFormat format( File::ExtractExtension( filePath ), true/*toRead*/, false/*toWrite*/ );
   FileFormatInstance file( format );

   try
   {
      ImageDescriptionArray images;

      if ( !file.Open( images, filePath ) )
         throw CatchedException();
      if ( images.IsEmpty() )
         throw Error( filePath + ": Empty image file." );
      if ( images.Length() > 1 )
         Console().NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in input file.", images.Length()-1 ) );
      if ( !file.SelectImage( 0 ) )
         throw CatchedException();
      if ( !file.ReadImage( image ) )
         throw CatchedException();
      if ( !file.Close() )
         throw CatchedException();

      ProcessInterface::ProcessEvents();

      return true;
   }
   catch ( ... )
   {
      if ( file.IsOpen() )
         file.Close();

      try
      {
         throw;
      }
      ERROR_HANDLER;

      return false;
   }
}

// ----------------------------------------------------------------------------
// BlinkInterface::FileData Implementation
// ----------------------------------------------------------------------------

BlinkInterface::FileData::FileData( FileFormatInstance& file,
                                    blink_image* image,
                                    const ImageDescription& description,
                                    const String& path,
                                    bool realPixelData ) :
   m_filePath( path ),
   m_image( image ),
   m_format( nullptr ),
   m_fsData( nullptr ),
   m_options( description.options ),
   m_info( description.info ),
   m_keywords(),
   m_profile(),
   m_statSTF(),
   m_statReal(),
   m_statRealRect( 0 ),
   m_isSTFStatisticsEqualToReal( false ),
   m_isRealPixelData( realPixelData )
{
#if debug
   m_id = m_total;
   Console().WriteLn( String().Format( "Create FileData Id %i", m_id ) );
   m_total++;
#endif
   m_format = new FileFormat( file.Format() );

   if ( m_format->UsesFormatSpecificData() )
      m_fsData = file.FormatSpecificData();

   if ( m_format->CanStoreKeywords() )
      file.Extract( m_keywords );

   if ( m_format->CanStoreICCProfiles() )
      file.Extract( m_profile );
#if debug
   Console().WriteLn( String().Format( "FileData End") );
#endif
}

BlinkInterface::FileData::~FileData()
{
#if debug
   Console().WriteLn( String().Format( "~FileData( Id = %i, total = %i )<br>{", m_id, m_total ) );
   Console().WriteLn( "format   = " + String( m_format != nullptr ) );
   Console().WriteLn( "image    = " + String( m_image != nullptr ) );
   Console().WriteLn( "statSTF  = " + String( !m_statSTF.IsEmpty() ) );
   Console().WriteLn( "statReal = " + String( !m_statReal.IsEmpty() ) );
   m_total--;
#endif
   if ( m_format != nullptr )
   {
      if ( m_fsData != nullptr )
         m_format->DisposeFormatSpecificData( const_cast<void*>( m_fsData ) ), m_fsData = nullptr;
      delete m_format, m_format = nullptr;
   }

   if ( m_image != nullptr )
      delete m_image, m_image = nullptr;
#if debug
   Console().WriteLn( "}~FileData" );
#endif
}

// ----------------------------------------------------------------------------
// BlinkInterface::BlinkData Implementation
// ----------------------------------------------------------------------------

BlinkInterface::BlinkData::BlinkData() :
   m_screenRect( 0 ),
   m_statRect( 0 ),
   m_filesData(),
   m_screen( ImageWindow::Null() ),
   m_info(),
   m_options(),
   m_currentImage( 0 ),
   m_blinkMaster( 0 ),
   m_isBlinkMaster( false )
{
#if debug
   m_id = m_total;
   m_total++;
#endif
}

BlinkInterface::BlinkData::~BlinkData()
{
#if debug
   Console().WriteLn( "<br>~BlinkData() Start" );
#endif

   Clear();

#if debug
   Console().WriteLn( String().Format( "Delete BlinkData Id %i", m_id ) );
   m_total--;
   Console().WriteLn( "BlinkData() Exit" );
#endif
}

bool BlinkInterface::BlinkData::Add( const String& filePath )
{
   Console().WriteLn( "<end><cbr>" + filePath );
   FileFormat format( File::ExtractExtension( filePath ), true/*toRead*/, false/*toWrite*/ );
   FileFormatInstance file( format );

   try
   {
      ImageDescriptionArray images;

      if ( !file.Open( images, filePath ) )
         throw CatchedException();
      if ( images.IsEmpty() )
         throw Error( filePath + ": Empty image file." );
      if ( images.Length() > 1 )
         throw Error( filePath + ": Multiple images cannot be used in the current version (push me if you need them)." );
      if ( !file.SelectImage( 0 ) )
         throw CatchedException();
      if ( !CheckGeomery( images[0] ) )
         throw Error( "Wrong image geometry" );

      AutoPointer<blink_image> image( new blink_image );

      if ( !file.ReadImage( *image ) )
         throw CatchedException();

      bool realPixelData = image->IsFloatSample() != images[0].options.ieeefpSampleFormat &&
                           image->BitsPerSample() != images[0].options.bitsPerSample;

      m_filesData.Add( new FileData( file, image.Release(), images[0], filePath, realPixelData ) );

      if ( !file.Close() )
         throw CatchedException();

      return true;
   }
   catch ( ... )
   {
      if ( file.IsOpen() )
         file.Close();

      try
      {
         throw;
      }
      ERROR_HANDLER;

      return false;
   }
}

void BlinkInterface::BlinkData::Remove( int row )
{
   const int fileNumber = FileNumberGet( row );

#if debug
   Console().WriteLn( String().Format( "Delete m_filesData record # %d", fileNumber ) );
#endif

   m_filesData.Destroy( m_filesData.At( fileNumber ) );

   if ( row == m_blinkMaster )
      m_isBlinkMaster = false;
   if ( row < m_blinkMaster )
      m_blinkMaster--;
   if ( row <= m_currentImage && m_currentImage > 0 )
      m_currentImage--;

   // Correcting links from Files_TreeBox to m_filesData
   for ( int i = 0; i < TheBlinkInterface->GUI->Files_TreeBox.NumberOfChildren(); i++ )
   {
      int n = FileNumberGet( i );
      if ( n > fileNumber ) // Correcting links only for fileNumbers great then curent Removed file #
      {
         TheBlinkInterface->GUI->Files_TreeBox.Child( i )->SetText( 1, String( --n ) );

#if debug
         Console().WriteLn( "Files_TreeBox: Link row: " + String( i ) + " to fileNumber: " + String( n ) );
#endif
      }
   }
   TheBlinkInterface->UpdateFileNumbers();

#if debug
   Console().WriteLn( String().Format( "m_filesData # %d was deleted", fileNumber ) );
#endif

   CheckScreen();
}

void BlinkInterface::BlinkData::Clear()
{
#if debug
   Console().WriteLn( "Clear() Start" );
   Console().WriteLn( String().Format( "Total m_filesData records in memory: %u", m_filesData.Length() ) );
#endif

   m_filesData.Destroy();
   m_isBlinkMaster = false;
   m_blinkMaster = 0;
   m_currentImage = 0;

   CheckScreen();

#if debug
   Console().WriteLn( "Clear() Exit" );
#endif
}

void BlinkInterface::BlinkData::GetStatsForSTF( int fileNumber )
{
#if debug
   Console().WriteLn( String().Format( "<br>Call GetStatsForSTF( fileNumber = %d )", fileNumber ) );
   m_total--;
#endif

   FileData& fd = m_filesData[fileNumber];
   if ( !fd.m_statSTF.IsEmpty() )
      return;

   Console().Show();
   StandardStatus callback;
   StatusMonitor m;
   m.SetCallback( &callback );
   m.Initialize( "<end><cbr><br>* Blink: Calculating statistics", fd.m_image->NumberOfNominalChannels() );
   ProcessEvents();

   for( int c = 0; c < fd.m_image->NumberOfNominalChannels(); c++, ++m )
   {
      fd.m_image->SelectChannel( c );
      ImageStatistics S;
      S.EnableRejection();
      S.SetRejectionLimits( 0.0, 1.0 );
      S.DisableSumOfSquares();
      S.DisableBWMV();
      S.DisablePBMV();
      S << *fd.m_image;
      fd.m_statSTF.Add( S );
   }
   fd.m_image->ResetChannelRange();
   fd.m_isSTFStatisticsEqualToReal = fd.m_isRealPixelData;
}

void BlinkInterface::BlinkData::AutoSTF()
{
   if ( !CheckScreen() )
      return;

   GetStatsForSTF();

   const int fileNumber = FileNumberGet( m_currentImage );
   const stats_list& S = m_filesData[fileNumber].m_statSTF;

   int n = m_filesData[fileNumber].m_image->NumberOfNominalChannels();
   double c0 = 0, m = 0;

   View::stf_list stf( 4 );

   if ( TheBlinkInterface->GUI->RGBLinked_Button.IsChecked() || n == 1 )
   {
      for ( int c = 0; c < n; c++ )
      {
         if ( 1 + S[c].MAD() != 1 )
            c0 += S[c].Median() + g_stfShadowsClipping * S[c].MAD();
         m += S[c].Median();
      }
      c0 = Range( c0/n, 0.0, 1.0 );
      m = HistogramTransformation::MTF( g_stfTargetBackground, m/n - c0 );
      for ( int i = 0; i < 3; i++ )
      {
         stf[i].SetMidtonesBalance( m );
         stf[i].SetClipping( c0, 1 );
         stf[i].SetRange( 0, 1 );
      }
   }
   else
   {
      for ( int c = 0; c < n; c++ )
      {
         c0 = (1 +  S[c].MAD() != 1) ? Range( S[c].Median() + g_stfShadowsClipping * S[c].MAD(), 0.0, 1.0 ) : 0.0;
         m = HistogramTransformation::MTF( g_stfTargetBackground, S[c].Median() - c0 );
         stf[c].SetMidtonesBalance( m );
         stf[c].SetClipping( c0, 1 );
         stf[c].SetRange( 0, 1 );
      }
   }

   stf[3].SetMidtonesBalance( 0.5 );
   stf[3].SetClipping( 0, 1 );
   stf[3].SetRange( 0, 1 );

   m_screen.MainView().SetScreenTransferFunctions( stf );

   EnableSTF();
}

void BlinkInterface::BlinkData::AutoHT()
{
   Console().Show();
   Console().WriteLn( "<end><cbr><br>* Blink: Applying automatic histogram transformations" );
   ProcessEvents();

   for ( int fileNumber = 0; fileNumber < int( m_filesData.Length() ); fileNumber++ )
   {
      GetStatsForSTF( fileNumber );
      FileData& fd = m_filesData[fileNumber];

      Console().WriteLn( fd.m_filePath );
      ProcessEvents();

      int n = fd.m_image->NumberOfNominalChannels();
      double c0 = 0, m = 0;
      for ( int c = 0; c < n; c++ )
      {
         const ImageStatistics& S = fd.m_statSTF[c];
         if ( 1 + S.MAD() != 1 )
            c0 += S.Median() + g_stfShadowsClipping * S.MAD();
         m += S.Median();
      }
      c0 = Range( c0/n, 0.0, 1.0 );
      m = HistogramTransformation::MTF( g_stfTargetBackground, m/n - c0 );
      for ( int c = 0; c < n; c++ )
      {
         HistogramTransformation H ( m, c0, 1, 0, 1 );
         fd.m_image->SelectChannel( c );
         H >> *fd.m_image;
         ProcessEvents();
      }
      fd.m_image->ResetChannelRange();
   }

   DisableSTF();
   UpdateScreen();

   Console().Hide();
}

void BlinkInterface::BlinkData::ResetHT()
{
   Console().Show();

   for ( int fileNumber = 0; fileNumber < int( m_filesData.Length() ); fileNumber++ )
      if ( !LoadImage_P( *m_filesData[fileNumber].m_image, m_filesData[fileNumber].m_filePath ) )
         throw CatchedException();

   //EnableSTF();
   DisableSTF();
   UpdateScreen();

   Console().Hide();
}

void BlinkInterface::BlinkData::EnableSTF()
{
   if ( !m_screen.IsNull() )
   {
      m_screen.MainView().EnableScreenTransferFunctions();
      Array<View> previews = m_screen.Previews();
      for ( Array<View>::iterator i = previews.Begin(); i != previews.End(); ++i )
         i->EnableScreenTransferFunctions();
   }
}

void BlinkInterface::BlinkData::DisableSTF()
{
   if ( !m_screen.IsNull() )
   {
      m_screen.MainView().DisableScreenTransferFunctions();
      Array<View> previews = m_screen.Previews();
      for ( Array<View>::iterator i = previews.Begin(); i != previews.End(); ++i )
         i->DisableScreenTransferFunctions();
   }
}

bool BlinkInterface::BlinkData::CheckScreen()
{
#if debug
   Console().WriteLn( "<br>Call CheckScreen() with files = " + String( m_filesData.Length() ) + ", screen = " + String( !m_screen.IsNull() ) );
#endif

   if ( m_screen.IsNull() ) // must check because user can close BlinkScreen by pushing red X on it.
      return false;

   if ( m_filesData.IsEmpty() ) // close Screen if no files
   {
#if debug
      Console().WriteLn( "Close BlinkScreen." );
#endif
      m_screen.Close();
      m_screen = ImageWindow::Null();
      return false;
   }

   return true;
}

bool BlinkInterface::BlinkData::CheckGeomery( const ImageDescription& description )
{
   const Rect imageBounds( description.info.width, description.info.height );

   if ( m_filesData.IsEmpty() )
   {
      m_statRect = m_screenRect = imageBounds;
      m_info = description.info;
      m_options = description.options;
   }
   else if ( imageBounds != m_screenRect )
   {
      Console().WarningLn( String().Format( "<end><cbr>*** Wrong image geometry: %dx%d pixels, expected %dx%d.",
                                            imageBounds.Width(), imageBounds.Height(),
                                            m_screenRect.Width(), m_screenRect.Height() ) );
      return false;
   }

   return true;
}

void BlinkInterface::BlinkData::Next()
{
   const int wasCurrent = m_currentImage;
   while ( true )
   {
      m_currentImage++;
      if ( m_currentImage >= int( m_filesData.Length() ) )
         m_currentImage = 0;
      if ( m_currentImage == wasCurrent ) // one round passed
         return;  // no new images
      if ( m_isBlinkMaster && m_currentImage == m_blinkMaster )
         continue; // skip blinkMaster
      if ( TheBlinkInterface->GUI->Files_TreeBox.Child( m_currentImage )->IsChecked() ) // skip unchecked
         break;
   }
   Update();
}

void BlinkInterface::BlinkData::Prev()
{
   const int wasCurrent = m_currentImage;
   while ( true )
   {
      if ( m_currentImage == 0 )
         m_currentImage = int( m_filesData.Length() );
      m_currentImage--;
      if ( m_currentImage == wasCurrent ) // one round passed
         break; // no new images
      if ( TheBlinkInterface->GUI->Files_TreeBox.Child( m_currentImage )->IsChecked() )
         break;
   }
   Update();
}

void BlinkInterface::BlinkData::UpdateScreen( int fileNumber )
{
#if debug
   Console().WriteLn( "UpdateScreen to fileNumber: " + String( fileNumber ) );
#endif

   if ( m_screen.IsNull() )
   {
      if ( m_filesData.IsEmpty() )
         return;

      m_screen = ImageWindow( m_info.width, m_info.height, m_info.numberOfChannels,
                              m_options.bitsPerSample, m_options.ieeefpSampleFormat,
                              IsColor(), true, "BlinkScreen" );

      if ( TheBlinkInterface->GUI->AutoSTF_Button.IsChecked() )
         AutoSTF();

      m_screen.BringToFront();
      m_screen.Show();
      m_screen.ZoomToFit();
   }

   FileData& fd = m_filesData[fileNumber];
   View view = m_screen.CurrentView();
   if( view.IsMainView() )
      view.Image().CopyImage( *fd.m_image );
   else
   {
      fd.m_image->SelectRectangle( m_screen.PreviewRect( view.Id() ) );
      view.Image().CopyImage( *fd.m_image );
      fd.m_image->ResetSelection();
   }
   m_screen.Regenerate();
}

void BlinkInterface::BlinkData::Update( int row )
{
   UpdateScreen( row );
   TheBlinkInterface->GUI->Files_TreeBox.SetCurrentNode( TheBlinkInterface->GUI->Files_TreeBox.Child( row ) );
}

void BlinkInterface::BlinkData::ShowNextImage()
{
   if ( m_isBlinkMaster )
   {
      static bool showBlinkMaster = false; // trigger
      showBlinkMaster = !showBlinkMaster;
      if ( showBlinkMaster )
      {
         //while PCL don't have events on TreeBox sorting, we must check blinkMaster row position every time
         if ( TheBlinkInterface->GUI->Files_TreeBox.Child( m_blinkMaster )->Icon( 0 ).IsNull() )
         {
            //row of blinkMaster chenged >> find row with blinkMaster
#if debug
            Console().WriteLn( "row of blinkMaster was:" + String( m_blinkMaster ) );
            Console().WriteLn( "row chenged >> find row with blinkMaster" );
#endif
            for ( int i = 0; i < TheBlinkInterface->GUI->Files_TreeBox.NumberOfChildren(); i++)
            {
               if ( !TheBlinkInterface->GUI->Files_TreeBox.Child( i )->Icon( 0 ).IsNull() )
               {
#if debug
                  Console().WriteLn( "blinkMaster found in row:" + String( i ) );
#endif
                  m_blinkMaster = i;
                  break;
               }
            }
         }
         Update( m_blinkMaster );
      }
      else
         Next();
   }
   else
      Next();
}

// ----------------------------------------------------------------------------
// BlinkInterface Implementation
// ----------------------------------------------------------------------------

BlinkInterface* TheBlinkInterface = nullptr;

BlinkInterface::BlinkInterface() :
   ProcessInterface(),
   GUI( nullptr ),
   m_blink(),
   m_previewBmp( Bitmap::Null() ),
   m_isRunning( false ),
   m_wheelSteps( 0 ),
   m_sortChannels( true ),    // mode: sort by channel(true) or not(false)?
   m_cropMode( false ),       // true = Statistics only for Green rectangle
   m_digits0_1( 6 ),          // digits after dot for range 0-1
   m_digits0_65535( 3 ),      // digits after dot for range 0-65535
   m_range0_65535( true ),    // true = use range 0-65535. false = use range 0-1
   m_writeStatsFile( false ), // true = write a text file with statistical data
#ifdef __PCL_WINDOWS
   m_program( "ffmpeg.exe" ), // video encoder program
#else
   m_program( "ffmpeg" ),
#endif
   m_frameOutputDir(),
   m_frameExtension( ".png" ),
   m_deleteVideoFrames( false )
{
   TheBlinkInterface = this;
   m_outputFileName = Id();
   // Default video encoder arguments
   m_arguments= "-y -r 25 -i " + m_outputFileName + "%05d" + m_frameExtension + ' ' + m_outputFileName + ".avi";
}

BlinkInterface::~BlinkInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
   m_blink.Clear();
}

IsoString BlinkInterface::Id() const
{
   return "Blink";
}

MetaProcess* BlinkInterface::Process() const
{
   return TheBlinkProcess;
}

const char** BlinkInterface::IconImageXPM() const
{
   return BlinkIcon_XPM;
}

InterfaceFeatures BlinkInterface::Features() const
{
   return InterfaceFeature::None;
}

bool BlinkInterface::IsInstanceGenerator() const
{
   return false;
}

bool BlinkInterface::CanImportInstances() const
{
   return false;
}

bool BlinkInterface::Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Blink" );
      PreviewSize = GUI->Preview_ScrollBox.Width(); // already scaled to logical units
   }

   Init();

   dynamic = false;
   return true;
}

void BlinkInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "_SortChannels",      m_sortChannels );
   Settings::Write( SettingsKey() + "_CropMode",          m_cropMode );
   Settings::Write( SettingsKey() + "_Digits0_1",         m_digits0_1 );
   Settings::Write( SettingsKey() + "_Digits0_65535",     m_digits0_65535 );
   Settings::Write( SettingsKey() + "_Range0_65535",      m_range0_65535 );
   Settings::Write( SettingsKey() + "_WriteStatsFile",    m_writeStatsFile );

   // Video generation parameters are not saved automatically. They are saved
   // manually from BlinkVideoDialog.
   //SaveVideoSettings();
}

void BlinkInterface::SaveVideoSettings() const
{
   Settings::Write( SettingsKey() + "_Program",           m_program );
   Settings::Write( SettingsKey() + "_Arguments",         m_arguments );
   Settings::Write( SettingsKey() + "_FrameOutputDir",    m_frameOutputDir );
   Settings::Write( SettingsKey() + "_FrameExtension",    m_frameExtension );
   Settings::Write( SettingsKey() + "_DeleteVideoFrames", m_deleteVideoFrames );
}

void BlinkInterface::LoadSettings()
{
   Settings::Read( SettingsKey() + "_SortChannels",       m_sortChannels );
   Settings::Read( SettingsKey() + "_CropMode",           m_cropMode );
   Settings::Read( SettingsKey() + "_Digits0_1",          m_digits0_1 );
   Settings::Read( SettingsKey() + "_Digits0_65535",      m_digits0_65535 );
   Settings::Read( SettingsKey() + "_Range0_65535",       m_range0_65535 );
   Settings::Read( SettingsKey() + "_ReadStatsFile",      m_writeStatsFile );

   LoadVideoSettings();

#if debug
   Console().WriteLn( "LoadSettings()" );
#endif
}

void BlinkInterface::LoadVideoSettings()
{
   Settings::Read( SettingsKey() + "_Program",            m_program );
   Settings::Read( SettingsKey() + "_Arguments",          m_arguments );
   Settings::Read( SettingsKey() + "_FrameOutputDir",     m_frameOutputDir );
   Settings::Read( SettingsKey() + "_FrameExtension",     m_frameExtension );
   Settings::Read( SettingsKey() + "_DeleteVideoFrames",  m_deleteVideoFrames );
}

bool BlinkInterface::WantsImageNotifications() const
{
   return true;
}

void BlinkInterface::ImageCreated( const View& view )
{
   /*
    * If a new preview has been created on our bliking screen window and we are
    * blinking, get possession of it by locking it for write operations.
    */
   if ( m_isRunning )
      if ( view.IsPreview() )
         if ( view.Window() == m_blink.m_screen )
            if ( !view.CanWrite() ) // other task owns a preview on our blinking screen window !? - stop blinking
               Stop();
            else
               view.LockForWrite();
}

void BlinkInterface::ImageDeleted( const View& view )
{
   /*
    * Our blinking screen window has been destroyed.
    */
   if ( !m_blink.m_screen.IsNull() )
      if ( view.Window() == m_blink.m_screen )
         m_blink.m_screen = ImageWindow::Null();
}

// ----------------------------------------------------------------------------

void BlinkInterface::Init()
{
#if debug
   Console().WriteLn( "Init() Start" );
   Console().WriteLn( String().Format( "files = %u", m_blink.m_filesData.Length() ) );
   Console().WriteLn( String().Format( "currentImage = %u", m_blink.m_currentImage ) );
   Console().WriteLn( String().Format( "blinkMaster = %i", m_blink.m_blinkMaster ) );
#endif

   Disable();  // disable GUI during initialization

   bool noFiles = m_blink.m_filesData.IsEmpty(); // noFiles = disabled = no files

   GUI->AutoHT_Button.Disable( noFiles );
   GUI->AutoSTF_Button.Disable( noFiles );
   GUI->FileClose_Button.Disable( noFiles );
   GUI->FileCloseAll_Button.Disable( noFiles );
   GUI->FileCopyTo_Button.Disable( noFiles );
   GUI->FileMoveTo_Button.Disable( noFiles );
   GUI->FileCropTo_Button.Disable( noFiles );
   GUI->Statistics_button.Disable( noFiles );
   GUI->CropToVideo_button.Disable( noFiles );

   if ( noFiles )             // file == 0 -> init default GUI
   {
      GUI->AutoHT_Button.Uncheck();
      GUI->AutoSTF_Button.SetChecked();
      GUI->RGBLinked_Button.Hide();
      GUI->RGBLinked_Button.SetChecked();
      GUI->Preview_ScrollBox.Update();
   }
   else
   {
      if ( m_blink.IsColor() && GUI->AutoSTF_Button.IsChecked() )
         GUI->RGBLinked_Button.Show();
      m_blink.Update();
      GUI->UpdatePreview_Timer.Start();
   }

   bool oneOrNone = m_blink.m_filesData.Length() < 2;  // disabled = zero or one file only
   GUI->PreviousImage_Button.Disable( oneOrNone );
   GUI->Play_Button.Disable( oneOrNone );
   GUI->NextImage_Button.Disable( oneOrNone );
   GUI->BlinkingDelay_ComboBox.Disable( oneOrNone );

   GeneratePreview();

   Enable();

#if debug
   Console().WriteLn( "Init() Exit" );
#endif
}

void BlinkInterface::TranslucentPlanets()
{
   const int  maxRadius = 30;               // Maximum planet radius
   const int  numberOfPlanets = 50;         // Number of translucent planets
   const int  networkFrequency = 10;        // Frequency of network lines
   const RGBA skyTopColor = 0xFF000000;     // Top background color (solid black by default)
   const RGBA skyBottomColor = 0xFF00008B;  // Bottom background color (dark blue by default)
   const RGBA networkColor = 0xFFFF8000;    // Network color (solid orange by default)
   const RGBA networkBkgColor = 0xFF000000; // Network background color (solid black by default)
   const int  planetTransparency = 0x80;    // Alpha value of all random planet colors

   m_previewBmp = Bitmap( PreviewSize, PreviewSize, BitmapFormat::RGB32 );

   // Begin painting
   Graphics g( m_previewBmp );

   // We want high-quality antialiased graphics
   g.EnableAntialiasing();

   // Fill the background with a linear gradient
   GradientBrush::stop_list stops;
   stops.Add( GradientBrush::Stop( 0.0, skyTopColor ) );
   stops.Add( GradientBrush::Stop( 1.0, skyBottomColor ) );
   g.FillRect( Rect( PreviewSize, PreviewSize ),
               LinearGradientBrush( Rect( PreviewSize, PreviewSize ), stops ) );

   // Draw random circles
   RandomNumberGenerator randomColor( 255 );
   RandomNumberGenerator R;
   for ( int i = 0; i < numberOfPlanets; ++i )
   {
      // Random colors in the range [0,255]
      int red = randomColor();
      int green = randomColor();
      int blue = randomColor();

      // Avoid too dark circles
      if ( red < 24 && green < 24 && blue < 24 ) { --i; continue; }

      // 32-bit RRGGBBAA color values
      RGBA color1 = RGBAColor( red, green, blue, planetTransparency );
      RGBA color2 = RGBAColor( red >> 1, green >> 1, blue >> 1, planetTransparency );

      // Random center and radius
      Point center( PreviewSize * R(), PreviewSize * R() );
      double radius = maxRadius * R();

      // Define working objects
      GradientBrush::stop_list stops;
      stops.Add( GradientBrush::Stop( 0.0, color1 ) );
      stops.Add( GradientBrush::Stop( 1.0, color2 ) );

      g.SetPen( color2 );
      g.SetBrush( RadialGradientBrush( center, radius, center, stops ) );

      // Draw this planet
      g.DrawCircle( center, radius );
   }

   // Erase the network region by drawing a dense network
   g.DisableAntialiasing();
   g.SetPen( networkBkgColor );
   for ( int i = 0; i < PreviewSize; ++i )
      g.DrawLine( i-1, PreviewSize, -1, i+1 );

   // Generate the network
   g.EnableAntialiasing();
   g.SetPen( networkColor );
   for ( int i = 0; i < PreviewSize; i += networkFrequency )
      g.DrawLine( i, PreviewSize-1, 0, i );
   g.DrawLine( PreviewSize-1, PreviewSize-1, 0, PreviewSize-1 );
}

void BlinkInterface::Image2Preview()
{
   blink_image image( *m_blink.m_filesData[0].m_image );

   if ( GUI->AutoSTF_Button.IsChecked() )
   {
      m_blink.GetStatsForSTF( 0 );

      int n = image.NumberOfNominalChannels();
      double c0 = 0, m = 0;
      for ( int c = 0; c < n; c++ )
      {
         const ImageStatistics& S = m_blink.m_filesData[0].m_statSTF[c];
         if ( 1 + S.MAD() != 1 )
            c0 += S.Median() + g_stfShadowsClipping * S.MAD();
         m += S.Median();
      }
      c0 = Range( c0/n, 0.0, 1.0 );
      m = HistogramTransformation::MTF( g_stfTargetBackground, m/n - c0 );
      for ( int c = 0; c < n; c++ )
      {
         HistogramTransformation H( m, c0, 1, 0, 1 );
         image.SelectChannel( c );
         H >> image;
      }
      image.ResetChannelRange();
   }

   int w = (image.Width() > image.Height()) ? PreviewSize : RoundInt( float( PreviewSize )*image.Width()/image.Height() );
   int h = (image.Height() > image.Width()) ? PreviewSize : RoundInt( float( PreviewSize )*image.Height()/image.Width() );
   m_previewBmp = Bitmap::Render( &image ).ScaledToSize( w, h ); // fit big Image to small Preview
}

void BlinkInterface::GeneratePreview()
{
   if ( m_blink.m_filesData.IsEmpty() )
      TranslucentPlanets();
   else
      Image2Preview();

   GUI->Preview_ScrollBox.SetFixedSize( m_previewBmp.Width(), m_previewBmp.Height() );
}

void BlinkInterface::UpdateFileNumbers()
{
#if debug
   Console().WriteLn( "UpdateFileNumbers()" );
#endif

   GUI->Files_TreeBox.DisableHeaderSorting();
   for ( int i = 0; i < GUI->Files_TreeBox.NumberOfChildren(); i++ )
   {
      RowToStringFileNumber( i );
      GUI->Files_TreeBox.Child( i )->SetAlignment( 1, TextAlign::Right );
      GUI->Files_TreeBox.Child( i )->SetAlignment( 2, TextAlign::Right );
   }
   GUI->Files_TreeBox.AdjustColumnWidthToContents( 2 );
   GUI->Files_TreeBox.EnableHeaderSorting();
}

void BlinkInterface::FileAdd()
{
   OpenFileDialog d;
   d.LoadImageFilters();
   d.EnableMultipleSelections();
   d.SetCaption( "Blink/FileOpen: Select Image Files" );
   if ( d.Execute() )
   {
      Console().Show();
      Console().WriteLn( String().Format( "<end><cbr><br>* Blink: Loading %u file(s)", d.FileNames().Length() ) );

      GUI->Files_TreeBox.DisableUpdates();

      ElapsedTime timer;  // to calculate execution time

      for ( size_type i = 0; i < d.FileNames().Length(); i++ )
      {
         if ( !m_blink.Add( d.FileNames()[i] ) )  // add the file to BlinkData
            continue;                             // skip the file on error

         TreeBox::Node* node = new TreeBox::Node( GUI->Files_TreeBox ); // add new item in Files_TreeBox
         node->Check();                                                 // check new file items
         node->SetText( 0, File::ExtractName( d.FileNames()[i] ) );     // show only the file name
         node->SetText( 1, String( GUI->Files_TreeBox.NumberOfChildren()-1 ) ); // Store file #

         ProcessEvents();
      }

      //GUI->Files_TreeBox.AdjustToContents(); ### don't do this, since it resizes the whole interface!
      GUI->Files_TreeBox.AdjustColumnWidthToContents( 0 );
      GUI->Files_TreeBox.AdjustColumnWidthToContents( 1 );
      UpdateFileNumbers();
      GUI->Files_TreeBox.EnableUpdates();

      Console().WriteLn( String().Format( "<end><cbr><br>Loaded in %.3f seconds.", timer() ) );
      Console().Hide();

      Init();
   }
}

void BlinkInterface::FileCopyTo()
{
   Pause();

   const String dir( SelectDirectory( "Blink/FileCopy: Select Output Directory" ) );
   if ( dir.IsEmpty() )
   {
      Continue();
      return;
   }

   try
   {
      Console().Show();

      Console().WriteLn( "<end><cbr><br>* Blink: " + String( GUI->Files_TreeBox.SelectedNodes().Length() ) + " files selected." );
      for ( int row = 0; row < GUI->Files_TreeBox.NumberOfChildren(); row++ )
         if ( GUI->Files_TreeBox.Child( row )->IsSelected() )
         {
            const int fileNumber = FileNumberGet( row );
            const String fromFilePath = m_blink.m_filesData[fileNumber].m_filePath;
            const String toFilePath = UniqueFilePath( fromFilePath, dir );
            Console().WriteLn( "Copy from " + fromFilePath + " to " + toFilePath );
            ProcessEvents();
            File::CopyFile( toFilePath, fromFilePath );
         }
      Console().WriteLn( "Done." );

      Console().Hide();
      Continue();
   }
   catch ( ... )
   {
      Console().Hide();
      Stop();
      throw;
   }
}

void BlinkInterface::FileMoveTo()
{
   Pause();

   const String dir( SelectDirectory( "Blink/FileMove: Select Output Directory" ) );
   if ( dir.IsEmpty() )
   {
      Continue();
      return;
   }

   try
   {
      Console().Show();

      Console().WriteLn( "<end><cbr><br>* Blink: " + String( GUI->Files_TreeBox.SelectedNodes().Length() ) + " files selected." );
      for ( int row = 0; row < GUI->Files_TreeBox.NumberOfChildren(); row++ )
         if ( GUI->Files_TreeBox.Child( row )->IsSelected() )
         {
            const int fileNumber = FileNumberGet( row );
            const String fromFilePath = m_blink.m_filesData[fileNumber].m_filePath;
            const String toFilePath = UniqueFilePath( fromFilePath, dir );
            Console().WriteLn( "Move from " + fromFilePath + " to " + toFilePath );
            ProcessEvents();
            File::MoveFile( toFilePath, fromFilePath );
            m_blink.m_filesData[fileNumber].m_filePath = toFilePath; // update file path
         }
      Console().WriteLn( "Done." );

      Console().Hide();
      Continue();
   }
   catch ( ... )
   {
      Console().Hide();
      Stop();
      throw;
   }
}

Rect BlinkInterface::GetCropRect()
{
   if ( m_blink.m_screen.SelectedPreview().IsNull() )
      return m_blink.m_screen.ViewportToImage( m_blink.m_screen.VisibleViewportRect() );
   else
   {
      const Point p = m_blink.m_screen.PreviewRect( m_blink.m_screen.SelectedPreview().Id() ).LeftTop();
      return m_blink.m_screen.SelectedPreview().Bounds().MovedTo( p );
   }
}

void BlinkInterface::CropToVideo()
{
   Stop();

   if ( m_blink.m_screen.IsNull() )
   {
      pcl::MessageBox( "<p>The BlinkScreen image window has been closed - the cropping region is undefined.</p>",
                        "Blink", StdIcon::Information ).Execute();
      return;
   }

   BlinkVideoDialog dialog( this );
   dialog.Execute();
}

void BlinkInterface::FileCropTo()
{
   if ( m_blink.m_screen.IsNull() )
   {
      pcl::MessageBox( "<p>The BlinkScreen image window has been closed - the cropping region is undefined.</p>",
                        "Blink",
                        StdIcon::Information ).Execute();
      return;
   }

   try
   {
      Pause();

      Rect r(GetCropRect());

      if ( r == m_blink.m_screenRect )
      {
         pcl::MessageBox( "<p>The entire image has been selected - "
                           "Create a partial preview in BlinkScreen, or zoom in the image to define a valid cropping region?</p>",
                           "Blink", StdIcon::Information ).Execute();
         return;
      }

      const String dir( SelectDirectory( "Blink/Crop: Select Output Directory" ) );
      if ( dir.IsEmpty() ) return;

      Console().Show();

      Console().WriteLn( "<end><cbr><br>Blink: " + String( GUI->Files_TreeBox.SelectedNodes().Length() ) + " files selected." );
      for ( int row = 0; row < GUI->Files_TreeBox.NumberOfChildren(); row++ )
      {
         if ( GUI->Files_TreeBox.Child( row )->IsSelected() )
         {
            const int fileNumber = FileNumberGet( row );
            FileFormatInstance outputFile( CreateImageFile( fileNumber, "CropTo", dir ) );

            FileData& fd = m_blink.m_filesData[fileNumber];

            if ( fd.m_isRealPixelData )
            {
               fd.m_image->SelectRectangle( r );
               if ( !outputFile.WriteImage( *fd.m_image ) )
                  throw CatchedException();
               fd.m_image->ResetSelection();
            }
            else
            {
               DImage tmp;
               if ( !LoadImage_P( tmp, fd.m_filePath ) )
                  throw CatchedException();

               tmp.SelectRectangle( r );
               if ( !outputFile.WriteImage( tmp ) )
                  throw CatchedException();
            }

            outputFile.Close();
            ProcessEvents();
         }
      }
      Console().WriteLn( "Done." );

      Console().Hide();
      Continue();
   }
   catch ( ... )
   {
      Console().Hide();
      Stop();
      throw;
   }
}

void BlinkInterface::FileCloseSelected()
{
   Pause();

   GUI->Files_TreeBox.DisableUpdates();
   for ( int i = int( m_blink.m_filesData.Length() ); --i >= 0; )
   {
#if debug
      Console().WriteLn( String().Format( "Checking for Close file # %i", i ) );
#endif
      if ( GUI->Files_TreeBox.Child( i )->IsSelected() )
      {
#if debug
         Console().WriteLn(String().Format( "Close file # %i", i ) );
#endif

         m_blink.Remove( i );
         GUI->Files_TreeBox.Remove( i );
      }
   }
   GUI->Files_TreeBox.EnableUpdates();

   if ( m_blink.m_filesData.Length() < 2 )
   {
      Stop();
      Init();
      if ( m_blink.m_filesData.IsEmpty() )
         ResetFilesTreeBox();
   }
   else
   {
      m_blink.Update();
      Continue();
   }
}

void BlinkInterface::FileCloseAll()
{
   Console().Write( "<end><cbr><br>* Blink: Releasing memory ... " );
   Stop();
   m_blink.Clear();
   ResetFilesTreeBox();
   Init();
   Console().WriteLn( "done." );
}

void BlinkInterface::FileStatistics()
{
   Pause();
   BlinkStatisticsDialog dialog( this );
   dialog.Execute();
   Continue();
}

void BlinkInterface::DisableButtonsIfRunning()
{
   GUI->AutoHT_Button.Disable( m_isRunning );
   GUI->AutoSTF_Button.Disable( m_isRunning );
   GUI->RGBLinked_Button.Disable( m_isRunning );
   GUI->FileAdd_Button.Disable( m_isRunning );
   GUI->FileClose_Button.Disable( m_isRunning );
   GUI->FileCloseAll_Button.Disable( m_isRunning );
   GUI->FileCopyTo_Button.Disable( m_isRunning );
   GUI->FileMoveTo_Button.Disable( m_isRunning );
   GUI->FileCropTo_Button.Disable( m_isRunning );
   GUI->Statistics_button.Disable( m_isRunning );
   GUI->CropToVideo_button.Disable( m_isRunning );
}

void BlinkInterface::Play()
{
   Stop();

   GUI->Play_Button.SetIcon( Bitmap( ScaledResource( ":/icons/pause.png" ) ) );
   GUI->Play_Button.SetToolTip( "Pause Animation" );
   DisableButtonsIfRunning();

   /*
    * Ensure the screen window exists and is visible, in case it was closed.
    */
   m_blink.UpdateScreen();

   /*
    * Lock the blink screen window's main view for writing. This implicitly
    * locks all existing previews.
    */
   m_blink.m_screen.MainView().LockForWrite();

   m_isRunning = true;
   GUI->UpdateAnimation_Timer.Start();
}

void BlinkInterface::Stop()
{
   GUI->UpdateAnimation_Timer.Stop();

   if ( m_isRunning )
   {
      /*
       * Unlock all locked blink screen views
       */
      if ( m_blink.CheckScreen() ) // this check not necessary in theory, but doesn't hurt either
      {
         Array<View> previews = m_blink.m_screen.Previews();
         for ( Array<View>::iterator i = previews.Begin(); i != previews.End(); ++i )
            if ( !i->CanWrite() )
               i->Unlock();
         if ( !m_blink.m_screen.MainView().CanWrite() )
            m_blink.m_screen.MainView().Unlock();
      }
      m_isRunning = false;
   }

   GUI->Play_Button.SetIcon( Bitmap( ScaledResource( ":/icons/play.png" ) ) );
   GUI->Play_Button.SetToolTip( "Play Animation" );
   DisableButtonsIfRunning();
}

void BlinkInterface::Pause()
{
   if ( !m_blink.m_filesData.IsEmpty() )
      GUI->UpdatePreview_Timer.Stop();
   if ( m_isRunning )
      GUI->UpdateAnimation_Timer.Stop();
}

void BlinkInterface::Continue()
{
   if ( !m_blink.m_filesData.IsEmpty() )
      GUI->UpdatePreview_Timer.Start();
   if ( m_isRunning )
      GUI->UpdateAnimation_Timer.Start();
}

// ----------------------------------------------------------------------------
// Event Handlers
// ----------------------------------------------------------------------------

void BlinkInterface::__Brightness_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AutoHT_Button )
   {
      Pause();

      if ( checked )
      {
         if ( GUI->AutoSTF_Button.IsChecked() )
         {
            GUI->AutoSTF_Button.Uncheck();
            GUI->RGBLinked_Button.Hide();
         }

         m_blink.AutoHT();
      }
      else
         m_blink.ResetHT();

      if ( m_blink.m_screen.IsNull() )
         m_blink.UpdateScreen();

      GeneratePreview();

      Continue();
   }
   else if ( sender == GUI->AutoSTF_Button )
   {
      Pause();

      if ( checked )
      {
         if ( GUI->AutoHT_Button.IsChecked() )
         {
            GUI->AutoHT_Button.Uncheck();
            m_blink.ResetHT();
         }

         if ( m_blink.IsColor() )
            GUI->RGBLinked_Button.Show();

         m_blink.AutoSTF();
      }
      else
      {
         GUI->RGBLinked_Button.Hide();
         m_blink.DisableSTF();
      }

      if ( m_blink.m_screen.IsNull() )
         m_blink.UpdateScreen();

      GeneratePreview();

      Continue();
   }
   else if ( sender == GUI->RGBLinked_Button )
   {
      Pause();

      m_blink.AutoSTF();

      GeneratePreview();

      Continue();
   }
}

void BlinkInterface::__Files_NodeSelectionUpdated( TreeBox& sender )
{
#if debug
   Console().WriteLn( "__Files_NodeSelectionUpdated<flush>" );
#endif

   m_blink.m_currentImage = sender.ChildIndex( sender.CurrentNode() );

#if debug
   Console().WriteLn( "Set currentImage to: " + String( m_blink.m_currentImage ) );
#endif

   m_blink.UpdateScreen();
}

void BlinkInterface::__Files_NodeUpdated( TreeBox& sender, TreeBox::Node& node, int col  )
{
   for ( size_type i = 0; i < sender.SelectedNodes().Length(); i++ ) // Invert CheckMark
   {
      if ( sender.SelectedNodes()[i]->IsSameObject( node ) )
         continue;   // skip curent clicked node, because it will be inverted automaticaly
      sender.SelectedNodes()[i]->Uncheck( sender.SelectedNodes()[i]->IsChecked() );
   }
}

void BlinkInterface::__Files_MouseWheel( Control& sender, const Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   if ( delta > 0 )
      m_blink.Prev();
   else
      m_blink.Next();
}

void BlinkInterface::__Files_NodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col  )
{
#if debug
   Console().WriteLn( "__Files_NodeDoubleClicked<flush>" );
#endif
   if ( m_blink.m_isBlinkMaster )
   {
      sender.Child( m_blink.m_blinkMaster )->SetIcon( 0, Bitmap() );
      if ( m_blink.m_blinkMaster == sender.ChildIndex( &node ) )
      {
#if debug
         Console().WriteLn( "Disable blinkMaster<flush>" );
#endif
         m_blink.m_isBlinkMaster = false;
         return;
      }
   }

#if debug
   Console().WriteLn( "Enable blinkMaster<flush>" );
#endif

   m_blink.m_isBlinkMaster = true;
   m_blink.m_blinkMaster = sender.ChildIndex( &node );
   node.SetIcon( 0, Bitmap( ScaledResource( ":/icons/repeat.png" ) ) );

#if debug
   Console().WriteLn( "blinkMaster set to row: " + String( m_blink.m_blinkMaster ) );
#endif
}

void BlinkInterface::__FileButton_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->FileAdd_Button )
      FileAdd();
   else if ( sender == GUI->FileClose_Button )
      FileCloseSelected();
   else if ( sender == GUI->FileCloseAll_Button )
      FileCloseAll();
   else if ( sender == GUI->FileCopyTo_Button )
      FileCopyTo();
   else if ( sender == GUI->FileMoveTo_Button )
      FileMoveTo();
   else if ( sender == GUI->FileCropTo_Button )
      FileCropTo();
   else if ( sender == GUI->Statistics_button )
      FileStatistics();
   else if ( sender == GUI->CropToVideo_button )
      CropToVideo();
}

void BlinkInterface::__Delay_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   GUI->UpdateAnimation_Timer.SetInterval( g_blinkingDelaySecs[itemIndex] );
   if ( m_isRunning )
      GUI->UpdateAnimation_Timer.Start();
}

void BlinkInterface::__ActionButton_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Play_Button && !m_isRunning )
   {
      Play();
      return;
   }

   if ( m_isRunning )
      Stop();

   if ( sender == GUI->NextImage_Button )
      m_blink.Next();
   else if ( sender == GUI->PreviousImage_Button )
      m_blink.Prev();
}

void BlinkInterface::__ScrollControl_Paint( Control& sender, const Rect& r )
{
   if ( m_previewBmp.IsNull() )
      return;

   Graphics g( sender );
   g.DisableAntialiasing();

   g.DrawBitmapRect( r.LeftTop(), m_previewBmp, r );

   if ( m_blink.m_screen.IsNull() || m_blink.m_filesData.IsEmpty() )
      return;

   DRect destRect = m_blink.m_screen.ViewportToImage( m_blink.m_screen.VisibleViewportRect() );
   if ( m_blink.m_screen.CurrentView().IsPreview() )
      destRect += m_blink.m_screen.PreviewRect( m_blink.m_screen.CurrentView().Id() ).LeftTop();

   if ( m_blink.m_screen.SelectedPreview().IsNull() )
      g.SetPen( 0xFF00FF00 ); // green
   else
      g.SetPen( 0xFFFFFFFF ); // white

   double k = (m_blink.m_info.width > m_blink.m_info.height) ?
                              double( sender.Width() )/m_blink.m_info.width :
                              double( sender.Height() )/m_blink.m_info.height;
   g.DrawRect( (k * destRect).RoundedToInt() );

   if ( m_blink.m_screen.HasPreviews() )
   {
      Array<View> previews = m_blink.m_screen.Previews();
      for ( size_type i = 0; i < previews.Length(); i++ )
      {
         if ( m_blink.m_screen.SelectedPreview() == previews[i] )
            g.SetPen( 0xFF00FF00 ); // green
         else
            g.SetPen( 0xFFFFFFFF ); // white

         Point p = m_blink.m_screen.PreviewRect( previews[i].Id() ).LeftTop();
         g.DrawRect( (k * DRect( previews[i].Bounds().MovedTo( p ) )).RoundedToInt() );
      }
   }
}

void BlinkInterface::__ScrollControl_MouseWheel( Control& sender, const Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   if ( m_blink.m_screen.IsNull() || m_blink.m_filesData.IsEmpty() )
      return;

   m_wheelSteps += delta; // delta is rotation angle in 1/8 degree steps
   if ( Abs( m_wheelSteps ) >= WHEEL_STEP_ANGLE*8 )
   {
      if ( delta > 0 )
      {
         int z = m_blink.m_screen.ZoomFactor() - 1;
         m_blink.m_screen.SetZoomFactor( (z != 0) ? z : -2 );
      }
      else
         m_blink.m_screen.ZoomIn();

      m_wheelSteps = 0;

      sender.Update();
   }
}

void BlinkInterface::__ScrollControl_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( m_blink.m_filesData.IsEmpty() )
   {
      TranslucentPlanets();
      sender.Update();
      return;
   }

   if ( buttons != MouseButton::Left )
      return;

   if ( m_blink.m_screen.IsNull() )
      return;

   const double k = double( m_blink.m_info.width )/sender.Width();
   const Point p = m_blink.m_screen.PreviewRect( m_blink.m_screen.CurrentView().Id() ).LeftTop();
   m_blink.m_screen.SetViewport( pos.x*k - p.x, pos.y*k - p.y );

   sender.Update();
}

void BlinkInterface::__ScrollControl_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_blink.m_screen.IsNull() || m_blink.m_filesData.IsEmpty() )
      return;
   if ( buttons != MouseButton::Left )
      return;

   const double k = double( m_blink.m_info.width )/sender.Width() ;
   const Point p = m_blink.m_screen.PreviewRect( m_blink.m_screen.CurrentView().Id() ).LeftTop();
   m_blink.m_screen.SetViewport( pos.x*k - p.x, pos.y*k - p.y );

   sender.Update();
}

void BlinkInterface::__FilePanelHideButton_Click( Button& sender, bool /*checked*/ )
{
   if ( GUI->RightPanel_Control.IsHidden() )
   {
      SetVariableSize();

      GUI->RightPanel_Control.Show();
      GUI->RightPanel_Control.SetVariableSize();

      GUI->ShowTreeBox_Button.SetIcon( Bitmap( ScaledResource( ":/process-interface/contract.png" ) ) );
      GUI->ShowTreeBox_Button.SetToolTip( "<p>Hide file panel</p>" );
   }
   else
   {
      GUI->RightPanel_Control.SetFixedSize();
      GUI->RightPanel_Control.Hide();

      AdjustToContents();
      SetFixedSize();

      GUI->ShowTreeBox_Button.SetIcon( Bitmap( ScaledResource( ":/process-interface/expand.png" ) ) );
      GUI->ShowTreeBox_Button.SetToolTip( "<p>Show file panel</p>" );
   }
}

void BlinkInterface::__Show( Control& /*sender*/ )
{
   // If necessary, generate the preview bitmap
   if ( m_previewBmp.IsNull() )
      GeneratePreview();
   // Enable periodic preview refresh
   GUI->UpdatePreview_Timer.Start();
}

void BlinkInterface::__Hide( Control& /*sender*/ )
{
   // Stop periodic preview refreshing
   GUI->UpdatePreview_Timer.Stop();
   // Stop animation
   Stop();
}

void BlinkInterface::__UpdateAnimation_Timer( Timer& timer )
{
#if debug
   ElapsedTime t;
#endif

   if ( m_blink.CheckScreen() )
   {
      m_blink.ShowNextImage();
      timer.Start();
   }
   else
   {
      Stop();
   }

#if debug
   GUI->DebugInfo_Label.SetText( String().Format("%.3f",time) + "+" + String().Format( "%.3f", t() ) );
#endif
}

void BlinkInterface::__UpdatePreview_Timer( Timer& timer )
{
   /*
    * Here we must be careful to prevent unnecessary timer events when the
    * interface is not visible, or when we have no files to animate.
    */
   if ( IsVisible() )
   {
      GUI->Preview_ScrollBox.Viewport().Update();
      if ( !m_blink.m_filesData.IsEmpty() )
         timer.Start();
   }
}

// ----------------------------------------------------------------------------
// Helper routines
// ----------------------------------------------------------------------------

bool BlinkInterface::LoadImage( ImageVariant& image, const String& filePath )
{
   if ( image )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: return LoadImage_P( static_cast<Image&>( *image ), filePath );
         case 64: return LoadImage_P( static_cast<DImage&>( *image ), filePath );
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: return LoadImage_P( static_cast<UInt8Image&>( *image ), filePath );
         case 16: return LoadImage_P( static_cast<UInt16Image&>( *image ), filePath );
         case 32: return LoadImage_P( static_cast<UInt32Image&>( *image ), filePath );
         }
   return false;
}

String BlinkInterface::SelectDirectory( const String& caption, const String& initialPath )
{
   GetDirectoryDialog d;
   d.SetCaption( caption );
   d.SetInitialPath( initialPath );
   if ( d.Execute() )
      return d.Directory();
   return String();
}

String BlinkInterface::UniqueFilePath( const String& fileName, const String& dir )
{
   String filePath = dir + "/" + File::ExtractName( fileName ) + File::ExtractExtension( fileName );
   if ( !File::Exists( filePath ) )
      return filePath;

   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

int BlinkInterface::FileNumberGet( const int row ) //extract file # from GUI TreeBox
{
   const int fileNumber = TheBlinkInterface->GUI->Files_TreeBox.Child( row )->Text( 1 ).ToInt( 10 );

#if debug
   Console().WriteLn( "FileNumberGet() Convert Row # " + String( row ) + ", to fileNumber: " + String( fileNumber ) );
#endif

   return fileNumber;
}

String BlinkInterface::RowToStringFileNumber( const int row ) //Convert fileNumber to String with additional "0"
{
   const int fileNumber = FileNumberGet( row );

   String s = String( fileNumber );

#if debug
   Console().WriteLn( "RowToStringFileNumber() TreeBox row: " + String( row ) + " link to fileNumber: " + s );
#endif

   s.Prepend( '0', String( TheBlinkInterface->GUI->Files_TreeBox.NumberOfChildren() ).Length() - s.Length() );
   TheBlinkInterface->GUI->Files_TreeBox.Child( row )->SetText( 2, s );
   return s;
}

#define PrintableVersion_1( a, b, c, d ) \
                              String( #a ) + '.' + #b + '.' + #c + '.' + #d

#define PrintableVersion()    PrintableVersion_1( MODULE_VERSION_MAJOR,       \
                                                  MODULE_VERSION_MINOR,       \
                                                  MODULE_VERSION_REVISION,    \
                                                  MODULE_VERSION_BUILD )

FileFormatInstance BlinkInterface::CreateImageFile( int index, const String& history, const String& dir )
{
   const FileData& fd = m_blink.m_filesData[index];

   String filePath = fd.m_filePath;

   String extension;
   if ( fd.m_format->CanWrite() )
      extension = File::ExtractExtension( filePath );
   else
      extension = ".xisf";

   filePath = UniqueFilePath( File::ChangeExtension( filePath, extension ), dir );
   FileFormat outputFormat( extension, false/*toRead*/, true/*toWrite*/ );
   FileFormatInstance outputFile( outputFormat );
   Console().WriteLn( "<end><cbr><raw>* Creating file: " + filePath + "</raw>" );
   if ( !outputFile.Create( filePath ) )
      throw CatchedException();

   outputFile.SetOptions( fd.m_options );

   if ( fd.m_format->UsesFormatSpecificData() )
      outputFile.SetFormatSpecificData( fd.m_fsData );

   if ( fd.m_format->CanStoreKeywords() )
   {
      FITSKeywordArray keywords( fd.m_keywords );
      if ( !history.IsEmpty() )
      {
         keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "Processed with " + PixInsightVersion::AsString() ) );
         keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "Processed with Blink module " + PrintableVersion() ) );
         keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), history ) );
      }
      outputFile.Embed( keywords );
   }

   if ( fd.m_format->CanStoreICCProfiles() )
      outputFile.Embed( fd.m_profile );

   return outputFile;
}

void BlinkInterface::ResetFilesTreeBox()
{
   GUI->Files_TreeBox.Clear();
   GUI->Files_TreeBox.SetNumberOfColumns( 3 );
   GUI->Files_TreeBox.HideHeader();
   GUI->Files_TreeBox.Sort( 2 );       // sort by fileNumber with preped '0'
#ifndef debug
   GUI->Files_TreeBox.HideColumn( 1 ); // hidden column to store link from GUI to fileData
   GUI->Files_TreeBox.HideColumn( 2 ); // hidden column to store string representation of link from GUI to fileData
#endif
}

// ----------------------------------------------------------------------------

BlinkInterface::GUIData::GUIData( BlinkInterface& w )
{
   /*
    * Central panel elements
    */

   const char* scrollControlToolTip =
   "<p>1. Use the mouse wheel to zoom in/out.<br/>"
      "2. Click or click & drag to move the viewing region to a new position.</p>"
   "<p>Note: you can get the same result via direct manipulation (zoom/resize/scroll) on the BilnkScreen image window.</p>";

   Preview_ScrollBox.SetToolTip( scrollControlToolTip );
   Preview_ScrollBox.SetStyle( FrameStyle::Flat ); // no frame
   Preview_ScrollBox.SetScaledFixedSize( PreviewSize, PreviewSize );
   Preview_ScrollBox.Viewport().SetCursor( StdCursor::CirclePlus );
   Preview_ScrollBox.Viewport().OnPaint( (Control::paint_event_handler)&BlinkInterface::__ScrollControl_Paint, w );
   Preview_ScrollBox.Viewport().OnMouseWheel( (Control::mouse_wheel_event_handler)&BlinkInterface::__ScrollControl_MouseWheel, w );
   Preview_ScrollBox.Viewport().OnMousePress( (Control::mouse_button_event_handler)&BlinkInterface::__ScrollControl_MousePress, w );
   Preview_ScrollBox.Viewport().OnMouseMove( (Control::mouse_event_handler)&BlinkInterface::__ScrollControl_MouseMove, w );

   RGBLinked_Button.SetCheckable();
   RGBLinked_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-color.png" ) ) );
   RGBLinked_Button.SetToolTip( "<p>Link RGB channels. Enabled only for RGB images.</p>" );
   RGBLinked_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__Brightness_Click, w );

   AutoSTF_Button.SetCheckable();
   AutoSTF_Button.SetIcon( Bitmap( ScreenTransferFunctionIcon_XPM ) );
   AutoSTF_Button.SetToolTip( "<p>Compute AutoSTF for the curent image, then apply the computed STF to all images.</p>" );
   AutoSTF_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__Brightness_Click, w );

   AutoHT_Button.SetCheckable();
   AutoHT_Button.SetIcon( Bitmap( HistogramTransformationIcon_XPM ) );
   AutoHT_Button.SetToolTip( "<p>Apply an automatic histogram transformation to all images</p>" );
   AutoHT_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__Brightness_Click, w );

#if debug
   DebugInfo_Label.SetText( "Debug" );
   DebugInfo_Label.Sizer().AddStretch();
#endif

   PreviousImage_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/left.png" ) ) );
   PreviousImage_Button.SetToolTip( "Previous image" );
   PreviousImage_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__ActionButton_Click, w );

   Play_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/play.png" ) ) );
   Play_Button.SetToolTip( "Play Animation" );
   Play_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__ActionButton_Click, w );

   NextImage_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/right.png" ) ) );
   NextImage_Button.SetToolTip( "Next image" );
   NextImage_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__ActionButton_Click, w );

   for ( int i = 0 ; i < int( ItemsInArray( g_blinkingDelaySecs ) ); i++ )
      BlinkingDelay_ComboBox.AddItem( String().Format( "%.2f sec", g_blinkingDelaySecs[i] ) );
   BlinkingDelay_ComboBox.SetToolTip( "Minimum delay between images" );
   BlinkingDelay_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&BlinkInterface::__Delay_ItemSelected, w );

   ShowTreeBox_Button.SetIcon( Bitmap( w.ScaledResource( ":/process-interface/contract.png" ) ) );
   ShowTreeBox_Button.SetToolTip( "Hide file panel" );
   ShowTreeBox_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FilePanelHideButton_Click, w );

   /*
    * Left panel elements
    */

   const char* filesTreeBoxToolTip =
   "<p>1. Only checked images are used for blinking.<br/>"
      "2. To invert the selection for a set of images, select the images and click a check box, or press Space.<br/>"
      "3. Double click to select the BlinkMaster image.</p>";

   const char* fileNameNoteToolTip =
      "<p>Note: If necessary, the source files being copied or moved will be renamed to make them unique in the target folder. "
         "Existing files with the same names as source files will always be preserved.</p>";

   const char* selectionNoteToolTip =
      "<p>Note: <i>Checked</i> is not the same as <i>selected</i>. "
         "To select more then one image use Shift or Ctrl + arow keys or click.</p>";

   Files_TreeBox.SetNumberOfColumns( 3 );
   Files_TreeBox.SetScaledMinWidth( 250 );
   Files_TreeBox.HideHeader();
   Files_TreeBox.Sort( 2 ); // sort by fileNumber with preped '0'

#ifndef debug
   Files_TreeBox.HideColumn( 1 ); // hidden column to store link from GUI to fileData
   Files_TreeBox.HideColumn( 2 ); // hidden column to store string representation of link from GUI to fileData
#endif

   Files_TreeBox.EnableMultipleSelections();
   Files_TreeBox.DisableRootDecoration();
   Files_TreeBox.EnableAlternateRowColor();
   Files_TreeBox.SetToolTip( String( filesTreeBoxToolTip ) + selectionNoteToolTip );
   Files_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&BlinkInterface::__Files_NodeSelectionUpdated, w );
   Files_TreeBox.OnNodeUpdated( (TreeBox::node_event_handler)&BlinkInterface::__Files_NodeUpdated, w );
   Files_TreeBox.OnMouseWheel( (Control::mouse_wheel_event_handler)&BlinkInterface::__Files_MouseWheel, w );
   Files_TreeBox.OnNodeDoubleClicked( (TreeBox::node_event_handler)&BlinkInterface::__Files_NodeDoubleClicked, w );

   FileAdd_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/folder-open.png" ) ) );
   FileAdd_Button.SetToolTip( "<p>Add image files.</p>" );
   FileAdd_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileClose_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/window-close.png" ) ) );
   FileClose_Button.SetToolTip( String( "<p>Close Selected images.</p>" ) + selectionNoteToolTip );
   FileClose_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileCloseAll_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/window-close-all.png" ) ) );
   FileCloseAll_Button.SetToolTip( "<p>Close all images.</p>" );
   FileCloseAll_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileCopyTo_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/save.png" ) ) );
   FileCopyTo_Button.SetToolTip( String( "<p>Copy selected files to new location.</p>" ) + fileNameNoteToolTip + selectionNoteToolTip );
   FileCopyTo_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileMoveTo_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/file-copy.png" ) ) );
   FileMoveTo_Button.SetToolTip( String( "<p>Move selected files to new location.</p>" ) + fileNameNoteToolTip + selectionNoteToolTip );
   FileMoveTo_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileCropTo_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/cut.png" ) ) );
   FileCropTo_Button.SetToolTip(
      String( "<p>Crop the selected files by the green rectangle and save them to a new location. "
              "You can define a preview to use its size and position.</p>" ) + fileNameNoteToolTip + selectionNoteToolTip );
   FileCropTo_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   Statistics_button.SetIcon( Bitmap( w.ScaledResource( ":/icons/statistics.png" ) ) );
   Statistics_button.SetToolTip( "<p>Series analysis report.</p>");
   Statistics_button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   CropToVideo_button.SetIcon( Bitmap( w.ScaledResource( ":/icons/clap.png" ) ) );
   CropToVideo_button.SetToolTip( "<p>Crop and create video.</p>");
   CropToVideo_button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   /*
    * Sizers central panel
    */

   STF_Sizer.SetSpacing( 2 );
   STF_Sizer.Add( AutoHT_Button );
   STF_Sizer.Add( AutoSTF_Button );
   STF_Sizer.Add( RGBLinked_Button );
   STF_Sizer.AddStretch();

   Preview_sizer.SetSpacing( 4 );
   Preview_sizer.Add( Preview_ScrollBox );
   Preview_sizer.Add( STF_Sizer );

#if debug
   ActionControl_Sizer.Add( DebugInfo_Label );
#else
   ActionControl_Sizer.AddStretch();
#endif
   //ActionControl_Sizer.AddSpacing( 4 );
   ActionControl_Sizer.Add( PreviousImage_Button );
   ActionControl_Sizer.Add( Play_Button );
   ActionControl_Sizer.Add( NextImage_Button ) ;
   ActionControl_Sizer.AddSpacing( 4 );
   ActionControl_Sizer.Add( BlinkingDelay_ComboBox );
   ActionControl_Sizer.AddSpacing( 4 );
   ActionControl_Sizer.Add( ShowTreeBox_Button );

   CentralPanel_sizer.SetSpacing( 4 );
   CentralPanel_sizer.Add( Preview_sizer );
   CentralPanel_sizer.Add( ActionControl_Sizer );
   CentralPanel_sizer.AddStretch();

   /*
    * Sizers right panel
    */

   FilesControl_Sizer.SetSpacing( 2 );
   FilesControl_Sizer.Add( FileAdd_Button );
   FilesControl_Sizer.Add( FileClose_Button );
   FilesControl_Sizer.Add( FileCloseAll_Button );
   FilesControl_Sizer.Add( FileCopyTo_Button );
   FilesControl_Sizer.Add( FileMoveTo_Button );
   FilesControl_Sizer.Add( FileCropTo_Button );
   FilesControl_Sizer.Add( Statistics_button );
   FilesControl_Sizer.Add( CropToVideo_button );
   FilesControl_Sizer.AddStretch();

   RightPanel_sizer.SetSpacing( 4 );
   RightPanel_sizer.Add( Files_TreeBox, 100 );
   RightPanel_sizer.Add( FilesControl_Sizer );

   RightPanel_Control.SetSizer( RightPanel_sizer );

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( CentralPanel_sizer );
   Global_Sizer.Add( RightPanel_Control, 100 );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();

   w.OnShow( (Control::event_handler)&BlinkInterface::__Show, w );
   w.OnHide( (Control::event_handler)&BlinkInterface::__Hide, w );

   UpdateAnimation_Timer.SetSingleShot();
   UpdateAnimation_Timer.SetInterval( 0.0 );
   UpdateAnimation_Timer.OnTimer( (Timer::timer_event_handler)&BlinkInterface::__UpdateAnimation_Timer, w );

   UpdatePreview_Timer.SetSingleShot();
   UpdatePreview_Timer.SetInterval( 0.2 );
   UpdatePreview_Timer.OnTimer( (Timer::timer_event_handler)&BlinkInterface::__UpdatePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BlinkInterface.cpp - Released 2015/12/18 08:55:08 UTC
