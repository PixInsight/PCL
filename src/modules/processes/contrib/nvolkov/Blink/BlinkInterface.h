//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 01.02.02.0294
// ----------------------------------------------------------------------------
// BlinkInterface.h - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2018 Nikolay Volkov
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

#ifndef __BlinkInterface_h
#define __BlinkInterface_h

#include <pcl/ComboBox.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/ICCProfile.h>
#include <pcl/Image.h>
#include <pcl/ImageDescription.h>
#include <pcl/ImageInfo.h>
#include <pcl/ImageOptions.h>
#include <pcl/ImageStatistics.h>
#include <pcl/ImageWindow.h>
#include <pcl/Label.h>
#include <pcl/ReferenceArray.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ScrollBox.h>
#include <pcl/Sizer.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

//#define debug 1

namespace pcl
{

// ----------------------------------------------------------------------------

class FileFormat;
class FileFormatInstance;
class ImageWindow;

typedef UInt16Image  blink_image;

// ----------------------------------------------------------------------------

class BlinkInterface : public ProcessInterface
{
public:

   BlinkInterface();
   virtual ~BlinkInterface();

   virtual IsoString Id() const;

   virtual MetaProcess* Process() const;

   const char** IconImageXPM() const;
   virtual InterfaceFeatures Features() const;

   virtual bool IsInstanceGenerator() const;
   virtual bool CanImportInstances() const;

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual void SaveSettings() const;
   void SaveVideoSettings() const;

   virtual void LoadSettings();
   void LoadVideoSettings();

   virtual bool WantsImageNotifications() const;
   virtual void ImageCreated( const View& );
   virtual void ImageDeleted( const View& );

private:


   typedef Array<ImageStatistics>   stats_list;

   // -------------------------------------------------------------------------

   struct FileData
   {
#if debug
      static int        m_total;
      int               m_id;
#endif

      String            m_filePath;     // file path of main image
      blink_image*      m_image;        // main image
      FileFormat*       m_format;       // the file format of retrieved data
      const void*       m_fsData;       // format-specific data

      ImageOptions      m_options;
      ImageInfo         m_info;
      FITSKeywordArray  m_keywords;     // FITS keywords
      ICCProfile        m_profile;      // ICC profile

      stats_list        m_statSTF;      // image statistics for STF calculation
      stats_list        m_statReal;     // image statistics in actual pixel values
      Rect              m_statRealRect; // geometry of statReal

      bool              m_isSTFStatisticsEqualToReal; // statSTF generated from RealImage
      bool              m_isRealPixelData; // PixelData in image == RealImage

      FileData( FileFormatInstance&     file,
                blink_image*            image,
                const ImageDescription& description,
                const String&           filePath,
                bool                    realPixelData );

      virtual ~FileData();
   };

   // -------------------------------------------------------------------------

   struct BlinkData
   {
      BlinkData();
      virtual ~BlinkData();

      bool Add( const String& filePath ); // Load new image from File
      void Remove( int row );             // Remove one FileData record
      void Clear();                       // Remove all FileData records

      void UpdateScreen( int fileNumber );// Regenerate BlinkScreen image window
      void UpdateScreen()
      {
         UpdateScreen( FileNumberGet( m_currentImage ) );
      }

      void Update( int row );             // Update BlinkScreen + GUI
      void Update()
      {
         Update( m_currentImage );
      }

      void Next();                        // Show Next image on BlinkScreen
      void Prev();                        // Show Prev image on BlinkScreen
      void ShowNextImage();

      void GetStatsForSTF( int fileNumber ); // Calculate image statistics
      void GetStatsForSTF()
      {
         GetStatsForSTF( FileNumberGet( m_currentImage ) );
      }

      void AutoSTF();                     // Calculate and set auto screen stretch
      void AutoHT();                      // Calculate and apply auto histogram stretch
      void ResetHT();                     // Recover original pixel data after AutoHT()

      void EnableSTF();
      void DisableSTF();

      bool CheckScreen();                 // Close BlinkScreen if no files, return true if window valid

      bool IsColor() const
      {
         return m_info.colorSpace != ColorSpace::Gray;
      }

      Rect              m_screenRect;     // BlinkScreen geometry
      Rect              m_statRect;       // statReal geometry

      ReferenceArray<FileData>  m_filesData;      // Path, Images, FITS keywords, ICC profile, ...

      ImageWindow       m_screen;         // BlinkScreen image window
      ImageInfo         m_info;           // BlinkScreen image info
      ImageOptions      m_options;        // BlinkScreen image options

      int               m_currentImage;   // current image index in m_filesData

      int               m_blinkMaster;    // Index of blinkMaster
      bool              m_isBlinkMaster;  // Set or not?

   private:

#if debug
      static int m_total;
      int        m_id;
#endif

      bool CheckGeomery( const ImageDescription& description ); // Compare size of new image with Screen size
   };

   // -------------------------------------------------------------------------

   struct GUIData
   {
      GUIData( BlinkInterface& );

      HorizontalSizer   Global_Sizer;

      VerticalSizer        CentralPanel_sizer;
         HorizontalSizer      Preview_sizer;
            ScrollBox            Preview_ScrollBox;
            VerticalSizer        STF_Sizer;
               ToolButton           AutoSTF_Button;
               ToolButton           AutoHT_Button;
               ToolButton           RGBLinked_Button;

         HorizontalSizer         ActionControl_Sizer;
#if debug
            Label                DebugInfo_Label;
#endif
            ToolButton           PreviousImage_Button;
            ToolButton           Play_Button;
            ToolButton           NextImage_Button;
            ComboBox             BlinkingDelay_ComboBox;
            ToolButton           ShowTreeBox_Button;

      Control              RightPanel_Control;
      VerticalSizer        RightPanel_sizer;
         TreeBox              Files_TreeBox;
         HorizontalSizer      FilesControl_Sizer;
            ToolButton           FileAdd_Button;
            ToolButton           FileClose_Button;
            ToolButton           FileCloseAll_Button;
            ToolButton           FileCopyTo_Button;
            ToolButton           FileMoveTo_Button;
            ToolButton           FileCropTo_Button;
            ToolButton           Statistics_button;
            ToolButton           CropToVideo_button;

      Timer UpdateAnimation_Timer;
      Timer UpdatePreview_Timer;
   };

   // -------------------------------------------------------------------------

   GUIData*    GUI;
   BlinkData   m_blink;
   Bitmap      m_previewBmp;     // preview image
   bool        m_isRunning;      // true = show is going
   int         m_wheelSteps;     // accumulated 1/8-degree wheel steps

   /*
    * Statistics default settings
    */
   bool        m_sortChannels;   // mode: sort by channel(true) or not(false)?
   bool        m_cropMode;       // true = Statistics only for Green rectangle
   int         m_digits0_1;      // digits after dot for range 0-1.
   int         m_digits0_65535;  // digits after dot for range 0-65535.
   bool        m_range0_65535;   // true = use range range 0-65535. false = use range range 0-1.
   bool        m_writeStatsFile; // true = write a text file with statistical data

   /*
    * Video generation settings
    */
   String      m_outputFileName; // file name for output video files
   String      m_program;        // execute the program
   String      m_arguments;      // execute with parameters
   String      m_frameOutputDir; // output directory for frames and video
   String      m_frameExtension; // file extension for video frames
   bool        m_deleteVideoFrames; // delete or not temporary video frame files

   /*
    * Worker routines
    */

   void Init();                  // enable/disable buttons
   void TranslucentPlanets();    // nice pic to Preview if 0 file opened
   void Image2Preview();
   void GeneratePreview();

   void Play();
   void Stop();
   void Pause();
   void Continue();
   void DisableButtonsIfRunning();

   void FileAdd();
   void FileCloseSelected();
   void FileCloseAll();
   void FileCopyTo();
   void FileMoveTo();
   void CropToVideo();
   void FileCropTo();
   void FileStatistics();

   void UpdateFileNumbers();
   void FileNumberSet( const int row, const int fileNumber );  // set fileNumber in GUI TreeBox row

   /*
    * Event handlers
    */

   void __Brightness_Click( Button&, bool );

   void __Files_NodeSelectionUpdated( TreeBox& );
   void __Files_NodeUpdated( TreeBox&, TreeBox::Node&, int );
   void __Files_MouseWheel( Control&, const Point&, int, unsigned, unsigned );
   void __Files_NodeDoubleClicked( TreeBox&, TreeBox::Node&, int );

   void __FileButton_Click( Button&, bool );

   void __Delay_ItemSelected( ComboBox&, int );
   void __ActionButton_Click( Button&, bool );

   void __ScrollControl_Paint( Control&, const Rect& );
   void __ScrollControl_MouseWheel( Control&, const Point&, int, unsigned, unsigned );
   void __ScrollControl_MousePress( Control&, const Point&, int, unsigned, unsigned );
   void __ScrollControl_MouseMove( Control&, const Point&, unsigned, unsigned );

   void __FilePanelHideButton_Click( Button&, bool );

   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   void __Show( Control& );
   void __Hide( Control& );

   void __UpdateAnimation_Timer( Timer& );
   void __UpdatePreview_Timer( Timer& );

   /*
    * Helper routines
    */

   static bool LoadImage( ImageVariant&, const String& filePath );
   static String SelectDirectory( const String& caption, const String& initialPath = String() );
   static String UniqueFilePath( const String& fileName, const String& dir );
   static int FileNumberGet( const int row );   // extract fileNumber from GUI TreeBox
   static String RowToStringFileNumber( const int row ); // convert fileNumber to String with additional "0"

   FileFormatInstance CreateImageFile( int index, const String& history, const String& dir );

   void AddFiles( const StringList& );
   void ResetFilesTreeBox();
   Rect GetCropRect(); //return geometry of sub window

   //

   friend struct BlinkInterface::GUIData;
   friend struct BlinkInterface::BlinkData;
   friend class  BlinkStatisticsDialog;
   friend class  BlinkVideoDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern BlinkInterface* TheBlinkInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------
} // pcl

#endif   // __BlinkInterface_h

// ----------------------------------------------------------------------------
// EOF BlinkInterface.h - Released 2018-12-12T09:25:25Z
