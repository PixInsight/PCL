//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.08.0405
// ----------------------------------------------------------------------------
// PreferencesInterface.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PreferencesInterface_h
#define __PreferencesInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ColorComboBox.h>
#include <pcl/Edit.h>
#include <pcl/FontComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TreeBox.h>

#include "PreferencesInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class GlobalItemControl : public Control
{
public:

   GlobalItemControl();

   virtual void Synchronize() = 0;

   VerticalSizer sizer;
};

// ----------------------------------------------------------------------------

class GlobalFlagControl : public GlobalItemControl
{
public:

   GlobalFlagControl();

   void Synchronize() override;

   pcl_bool* item = nullptr;
   CheckBox  checkBox;
   Control*  enableControl = nullptr;
   Control*  disableControl = nullptr;

private:

   void __Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

class GlobalIntegerControl : public GlobalItemControl
{
public:

   GlobalIntegerControl();

   void Synchronize() override;

   int32*          item = nullptr;
   Label           label;
   HorizontalSizer spinSizer;
      SpinBox         spinBox;

private:

   void __ValueUpdated( SpinBox& sender, int value );
};

// ----------------------------------------------------------------------------

class GlobalUnsignedControl : public GlobalItemControl
{
public:

   GlobalUnsignedControl();

   void Synchronize() override;

   uint32*         item = nullptr;
   Label           label;
   HorizontalSizer spinSizer;
      SpinBox         spinBox;

private:

   void __ValueUpdated( SpinBox& sender, int value );
};

// ----------------------------------------------------------------------------

class GlobalSetControl : public GlobalItemControl
{
public:

   GlobalSetControl();

   void Synchronize() override;

   int32*          item = nullptr;
   int             minValue = 0;
   Label           label;
   HorizontalSizer comboSizer;
      ComboBox       comboBox;

private:

   void __ItemSelected( ComboBox& sender, int itemIndex );
};

// ----------------------------------------------------------------------------

class GlobalRealControl : public GlobalItemControl
{
public:

   GlobalRealControl();

   void Synchronize() override;

   double*         item = nullptr;
   Label           label;
   HorizontalSizer numericSizer;
      NumericEdit     numericEdit;

private:

   void __ValueUpdated( NumericEdit& sender, double value );
};

// ----------------------------------------------------------------------------

class GlobalStringControl : public GlobalItemControl
{
public:

   GlobalStringControl();

   void Synchronize() override;

   String*  item = nullptr;
   Label    label;
   Edit     edit;

protected:

   virtual String GetText( const String& s );

private:

   void __EditCompleted( Edit& sender );
};

// ----------------------------------------------------------------------------

class GlobalDirectoryControl : public GlobalStringControl
{
public:

   GlobalDirectoryControl();

   HorizontalSizer buttonSizer;
      PushButton selectDirButton;

private:

   String GetText( const String& s ) override;

   void __SelectDir( Button& sender, bool checked );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
};

// ----------------------------------------------------------------------------

class GlobalFileControl : public GlobalStringControl
{
public:

   GlobalFileControl();

   StringList fileExtensions;
   String     dialogTitle;
   HorizontalSizer buttonSizer;
      PushButton selectFileButton;

private:

   String GetText( const String& s ) override;

   void __SelectFile( Button& sender, bool checked );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
};

// ----------------------------------------------------------------------------

class GlobalFileExtensionControl : public GlobalStringControl
{
public:

   GlobalFileExtensionControl();

private:

   String GetText( const String& s ) override;
};

// ----------------------------------------------------------------------------

class GlobalIdentifierControl : public GlobalStringControl
{
public:

   GlobalIdentifierControl();

private:

   String GetText( const String& s ) override;
};

// ----------------------------------------------------------------------------

class GlobalColorControl : public GlobalItemControl
{
public:

   GlobalColorControl();

   void Synchronize() override;

   uint32*         item = nullptr;
   Label           label;
   HorizontalSizer colorSizer;
      ColorComboBox   colorComboBox;
      Control         colorSample;

private:

   void __ColorSelected( ColorComboBox& sender, RGBA color );
   void __ColorSample_Paint( Control& sender, const Rect& updateRect );
   void __ColorSample_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
};

// ----------------------------------------------------------------------------

class GlobalFontControl : public GlobalItemControl
{
public:

   GlobalFontControl();

   void Synchronize() override;

   String*         item         = nullptr;
   int32*          itemSize     = nullptr;
   pcl_bool*       itemSizeAuto = nullptr;
   Label           label;
   FontComboBox    fontComboBox;
   Edit            fontSample;
   HorizontalSizer sizeSizer;
      Label          sizeLabel;
      SpinBox        sizeSpinBox;
      CheckBox       sizeAutoCheckBox;

private:

   void __FontSelected( FontComboBox& sender, const String& fontFace );
   void __SizeUpdated( SpinBox& sender, int value );
   void __AutoSizeClick( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

class GlobalDirectoryListControl : public GlobalItemControl
{
public:

   GlobalDirectoryListControl();

   void Synchronize() override;

   StringList*     item = nullptr;
   String          dialogTitle;
   Label           label;
   TreeBox         directoriesTreeBox;
   HorizontalSizer buttonsSizer;
      PushButton     addPushButton;
      PushButton     removePushButton;
      PushButton     resetPushButton;

private:

   void __NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __Click( Button& sender, bool checked );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
};

// ----------------------------------------------------------------------------

class GlobalFileSetControl : public GlobalItemControl
{
public:

   GlobalFileSetControl();

   void Synchronize() override;

   Array<String*>  items;
   StringList      fileExtensions;
   String          dialogTitle;
   Label           label;
   TreeBox         filesTreeBox;
   HorizontalSizer buttonsSizer;
      PushButton     addPushButton;
      PushButton     removePushButton;
      PushButton     clearPushButton;

private:

   void __NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __Click( Button& sender, bool checked );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
};

// ----------------------------------------------------------------------------

class PreferencesCategoryPage : public Control
{
public:

   PreferencesCategoryPage() : Control()
   {
   }

   virtual ~PreferencesCategoryPage()
   {
   }

   virtual void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) = 0;

   virtual void PerformAdditionalUpdates()
   {
   }

   VerticalSizer  Page_Sizer;
};

// ----------------------------------------------------------------------------

class PreferencesCategory
{
public:

   PreferencesCategoryPage* page = nullptr;

   PreferencesCategory() = default;

   virtual ~PreferencesCategory()
   {
      if ( page != nullptr )
         if ( page->Parent().IsNull() )
            delete page, page = nullptr;
   }

   PreferencesCategoryPage* CreatePage( PreferencesInstance& instance )
   {
      if ( page == nullptr )
         page = NewPage( instance );
      return page;
   }

   virtual String Title() = 0;

   virtual Bitmap Icon( double resourceScalingFactor )
   {
      return Bitmap( UIScaledResource( resourceScalingFactor, ":/icons/gear.png" ) );
   }

   void HidePage()
   {
      if ( page != nullptr )
         page->Hide();
   }

   void PerformAdditionalUpdates()
   {
      if ( page != nullptr )
         page->PerformAdditionalUpdates();
   }

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from )
   {
      if ( page != nullptr )
         page->TransferSettings( to, from );
   }

private:

   virtual PreferencesCategoryPage* NewPage( PreferencesInstance& ) = 0;
};

#define DEFINE_PREFERENCES_CATEGORY( name, title )                            \
class name##PreferencesCategory : public PreferencesCategory                  \
{                                                                             \
public:                                                                       \
                                                                              \
   name##PreferencesCategory() : PreferencesCategory()                        \
   {                                                                          \
   }                                                                          \
                                                                              \
   String Title() override { return title; }                                  \
                                                                              \
private:                                                                      \
                                                                              \
   PreferencesCategoryPage* NewPage( PreferencesInstance& instance ) override \
   {                                                                          \
      return new name##PreferencesPage( instance );                           \
   }                                                                          \
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class MainWindowPreferencesPage : public PreferencesCategoryPage
{
public:

   MainWindowPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFlagControl          MaximizeAtStartup_Flag;
   GlobalFlagControl          FullScreenAtStartup_Flag;
   GlobalFlagControl          ShowSplashAtStartup_Flag;
   GlobalFlagControl          CheckForUpdatesAtStartup_Flag;
   GlobalFlagControl          ConfirmProgramTermination_Flag;
   GlobalFlagControl          NativeMenuBar_Flag;
   GlobalFlagControl          CapitalizedMenuBars_Flag;
   GlobalFlagControl          WindowButtonsOnTheLeft_Flag;
   GlobalFlagControl          AcceptDroppedFiles_Flag;
   GlobalFlagControl          DoubleClickLaunchesOpenDialog_Flag;
   GlobalIntegerControl       MaxRecentFiles_Integer;
   GlobalFlagControl          ShowRecentlyUsed_Flag;
   GlobalFlagControl          ShowMostUsed_Flag;
   GlobalFlagControl          ShowFavorites_Flag;
   GlobalIntegerControl       MaxUsageListLength_Integer;
   GlobalFlagControl          ExpandRecentlyUsedAtStartup_Flag;
   GlobalFlagControl          ExpandMostUsedAtStartup_Flag;
   GlobalFlagControl          ExpandFavoritesAtStartup_Flag;
   GlobalFlagControl          OpenURLsWithInternalBrowser_Flag;
};

DEFINE_PREFERENCES_CATEGORY( MainWindow, "Main Window / Startup" )

// ----------------------------------------------------------------------------

class ResourcesPreferencesPage : public PreferencesCategoryPage
{
public:

   ResourcesPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFileControl          StyleSheet_File;
   GlobalFileSetControl       Resources_FileSet;
   GlobalFlagControl          AutoUIScaling_Flag;
   HorizontalSizer            UIScaling_Sizer;
      GlobalRealControl          UIScalingFactor_Real;
      GlobalIntegerControl       FontResolution_Integer;
   HorizontalSizer            Font_Sizer;
      GlobalStringControl        LowResFont_String;
      GlobalStringControl        HighResFont_String;
   HorizontalSizer            MonoFont_Sizer;
      GlobalStringControl        LowResMonoFont_String;
      GlobalStringControl        HighResMonoFont_String;
};

DEFINE_PREFERENCES_CATEGORY( Resources, "Core UI Resources" )

// ----------------------------------------------------------------------------

class WallpapersPreferencesPage : public PreferencesCategoryPage
{
public:

   WallpapersPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFileSetControl       Wallpapers_FileSet;
   GlobalFlagControl          UseWallpapers_Flag;
};

DEFINE_PREFERENCES_CATEGORY( Wallpapers, "Core Wallpapers" )

// ----------------------------------------------------------------------------

class EphemeridesPreferencesPage : public PreferencesCategoryPage
{
public:

   EphemeridesPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFileControl          FundamentalEphemerides_File;
   GlobalFileControl          ShortTermFundamentalEphemerides_File;
   GlobalFileControl          AsteroidEphemerides_File;
   GlobalFileControl          ShortTermAsteroidEphemerides_File;
   GlobalFileControl          NutationModel_File;
   GlobalFileControl          ShortTermNutationModel_File;
   GlobalFileControl          DeltaTData_File;
   GlobalFileControl          DeltaATData_File;
   GlobalFileControl          CIPITRSData_File;
};

DEFINE_PREFERENCES_CATEGORY( Ephemerides, "Core Ephemerides" )

// ----------------------------------------------------------------------------

class GUIEffectsPreferencesPage : public PreferencesCategoryPage
{
public:

   GUIEffectsPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFlagControl          HoverableAutoHideWindows_Flag;
   GlobalFlagControl          DesktopSettingsAware_Flag;
   GlobalFlagControl          TranslucentWindows_Flag;
   GlobalFlagControl          TranslucentChildWindows_Flag;
   GlobalFlagControl          FadeWindows_Flag;
   GlobalFlagControl          FadeAutoHideWindows_Flag;
   GlobalFlagControl          TranslucentAutoHideWindows_Flag;
   GlobalFlagControl          FadeWorkspaces_Flag;
   GlobalFlagControl          FadeMenu_Flag;
   GlobalFlagControl          FadeToolTip_Flag;
   GlobalFlagControl          ExplodeIcons_Flag;
   GlobalFlagControl          ImplodeIcons_Flag;
   GlobalFlagControl          AnimateWindows_Flag;
   GlobalFlagControl          AnimateMenu_Flag;
   GlobalFlagControl          AnimateCombo_Flag;
   GlobalFlagControl          AnimateToolTip_Flag;
   GlobalFlagControl          AnimateToolBox_Flag;
};

DEFINE_PREFERENCES_CATEGORY( GUIEffects, "Special GUI Effects" )

// ----------------------------------------------------------------------------

class FileIOPreferencesPage : public PreferencesCategoryPage
{
public:

   FileIOPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFlagControl          BackupFiles_Flag;
   GlobalFileExtensionControl DefaultFileExtension_Ext;
   GlobalFlagControl          NativeFileDialogs_Flag;
   GlobalFlagControl          RememberFileOpenType_Flag;
   GlobalFlagControl          RememberFileSaveType_Flag;
   GlobalFlagControl          StrictFileSaveMode_Flag;
   GlobalFlagControl          FileFormatWarnings_Flag;
   GlobalFlagControl          DefaultEmbedThumbnails_Flag;
   GlobalFlagControl          DefaultEmbedProperties_Flag;
};

DEFINE_PREFERENCES_CATEGORY( FileIO, "File I/O Settings" )

// ----------------------------------------------------------------------------

class DirectoriesAndNetworkPreferencesPage : public PreferencesCategoryPage
{
public:

   DirectoriesAndNetworkPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalDirectoryListControl SwapDirectories_DirList;
   GlobalFlagControl          SwapCompression_Flag;
   GlobalDirectoryControl     DownloadsDirectory_Dir;
   GlobalFlagControl          FollowDownloadLocations_Flag;
   GlobalFlagControl          VerboseNetworkOperations_Flag;
};

DEFINE_PREFERENCES_CATEGORY( DirectoriesAndNetwork, "Directories and Network" )

// ----------------------------------------------------------------------------

class DefaultImageResolutionPreferencesPage : public PreferencesCategoryPage
{
public:

   DefaultImageResolutionPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalRealControl          DefaultHorizontalResolution_Real;
   GlobalRealControl          DefaultVerticalResolution_Real;
   GlobalFlagControl          DefaultMetricResolution_Flag;
};

DEFINE_PREFERENCES_CATEGORY( DefaultImageResolution, "Default Image Resolution" )

// ----------------------------------------------------------------------------

class DefaultMaskSettingsPreferencesPage : public PreferencesCategoryPage
{
public:

   DefaultMaskSettingsPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFlagControl          DefaultMasksShown_Flag;
   GlobalSetControl           DefaultMaskMode_Set;
};

DEFINE_PREFERENCES_CATEGORY( DefaultMaskSettings, "Default Mask Settings" )

// ----------------------------------------------------------------------------

class DefaultTransparencySettingsPreferencesPage : public PreferencesCategoryPage
{
public:

   DefaultTransparencySettingsPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalSetControl           DefaultTransparencyMode_Set;
   GlobalSetControl           TransparencyBrush_Set;
};

DEFINE_PREFERENCES_CATEGORY( DefaultTransparencySettings, "Default Transparency Settings" )

// ----------------------------------------------------------------------------

class MiscImageWindowSettingsPreferencesPage : public PreferencesCategoryPage
{
public:

   MiscImageWindowSettingsPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFlagControl          ShowCaptionCurrentChannels_Flag;
   GlobalFlagControl          ShowCaptionZoomRatios_Flag;
   GlobalFlagControl          ShowCaptionIdentifiers_Flag;
   GlobalFlagControl          ShowCaptionFullPaths_Flag;
   GlobalFlagControl          ShowActiveSTFIndicators_Flag;
   GlobalIntegerControl       CursorTolerance_Integer;
   GlobalUnsignedControl      WheelStepAngle_Unsigned;
   GlobalSetControl           WheelDirection_Set;
   GlobalFlagControl          TouchEvents_Flag;
   GlobalRealControl          PinchSensitivity_Real;
   GlobalFlagControl          FastScreenRenditions_Flag;
   GlobalIntegerControl       FastScreenRenditionThreshold_Integer;
   GlobalFlagControl          HighDPIRenditions_Flag;
   GlobalFlagControl          Default24BitScreenLUT_Flag;
   GlobalFlagControl          CreatePreviewsFromCoreProperties_Flag;
   GlobalFlagControl          LoadAstrometricSolutions_Flag;
};

DEFINE_PREFERENCES_CATEGORY( MiscImageWindowSettings, "Miscellaneous Image Window Settings" )

// ----------------------------------------------------------------------------

class IdentifiersPreferencesPage : public PreferencesCategoryPage
{
public:

   IdentifiersPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFlagControl          UseFileNamesAsImageIdentifiers_Flag;
   GlobalIdentifierControl    WorkspacePrefix_Id;
   GlobalIdentifierControl    ImagePrefix_Id;
   GlobalIdentifierControl    PreviewPrefix_Id;
   GlobalIdentifierControl    ProcessIconPrefix_Id;
   GlobalIdentifierControl    ImageContainerIconPrefix_Id;
   GlobalIdentifierControl    ClonePostfix_Id;
};

DEFINE_PREFERENCES_CATEGORY( Identifiers, "Global Object Identifiers" )

// ----------------------------------------------------------------------------

class StringsPreferencesPage : public PreferencesCategoryPage
{
public:

   StringsPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalStringControl        NewImageCaption_String;
   GlobalStringControl        NoViewsAvailableText_String;
   GlobalStringControl        NoViewSelectedText_String;
   GlobalStringControl        NoPreviewsAvailableText_String;
   GlobalStringControl        NoPreviewSelectedText_String;
   GlobalStringControl        BrokenLinkText_String;
};

DEFINE_PREFERENCES_CATEGORY( Strings, "Global Strings and Indicators" )

// ----------------------------------------------------------------------------

class ParallelProcessingPreferencesPage : public PreferencesCategoryPage
{
public:

   ParallelProcessingPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;
   void PerformAdditionalUpdates() override;

   GlobalFlagControl          EnableParallelProcessing_Flag;
   GlobalFlagControl          EnableParallelCoreRendering_Flag;
   GlobalFlagControl          EnableParallelCoreColorManagement_Flag;
   GlobalFlagControl          EnableParallelModuleProcessing_Flag;
   GlobalFlagControl          EnableThreadCPUAffinity_Flag;
   GlobalSetControl           MaxModuleThreadPriority_Set;
   GlobalIntegerControl       MaxProcessors_Integer;
   CheckBox                   UseAllAvailableProcessors_CheckBox;

   void __UseAllAvailableProcessors_ButtonClick( Button& sender, bool checked );
};

DEFINE_PREFERENCES_CATEGORY( ParallelProcessing, "Parallel Processing and Threads" )

// ----------------------------------------------------------------------------

class MiscProcessingPreferencesPage : public PreferencesCategoryPage
{
public:

   MiscProcessingPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalFlagControl          BackupPSMFiles_Flag;
   GlobalFlagControl          GenerateScriptComments_Flag;
   GlobalIntegerControl       MaxConsoleLines_Integer;
   GlobalIntegerControl       ConsoleDelay_Integer;
   GlobalIntegerControl       AutoSavePSMPeriod_Integer;
   GlobalFlagControl          AlertOnProcessCompleted_Flag;
   GlobalFlagControl          EnableExecutionStatistics_Flag;
   GlobalFlagControl          EnableLaunchStatistics_Flag;
};

DEFINE_PREFERENCES_CATEGORY( MiscProcessing, "Miscellaneous Process Settings" )

// ----------------------------------------------------------------------------

class TransparencyColorsPreferencesPage : public PreferencesCategoryPage
{
public:

   TransparencyColorsPreferencesPage( PreferencesInstance& );

   void TransferSettings( PreferencesInstance& to, const PreferencesInstance& from ) override;

   GlobalColorControl         TransparencyBrushForegroundColor_Color;
   GlobalColorControl         TransparencyBrushBackgroundColor_Color;
   GlobalColorControl         DefaultTransparencyColor_Color;
};

DEFINE_PREFERENCES_CATEGORY( TransparencyColors, "Transparency Colors" )

// ----------------------------------------------------------------------------

class PreferencesInterface : public ProcessInterface
{
public:

   PreferencesInterface();
   virtual ~PreferencesInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   const char** IconImageXPM() const override;

   InterfaceFeatures Features() const override;

   void ResetInstance() override;

   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;

   ProcessImplementation* NewProcess() const override;

   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   PreferencesInstance instance;

   class CategoryNode : public TreeBox::Node
   {
   public:

      CategoryNode( TreeBox& parentTree, int pageIndex ) :
         TreeBox::Node( parentTree ), m_pageIndex( pageIndex )
      {
      }

      int PageIndex() const
      {
         return m_pageIndex;
      }

   private:

      int m_pageIndex;
   };

   class GUIData
   {
   public:

      GUIData( PreferencesInterface& );
      virtual ~GUIData();

      VerticalSizer     Global_Sizer;
         HorizontalSizer   TopRow_Sizer;
            TreeBox           CategorySelection_TreeBox;
            Control           CategoryStack_Control;
            VerticalSizer     CategoryStack_Sizer;
               Control           Title_Control;
                  HorizontalSizer   Title_Sizer;
                  Label             Title_Label;
         HorizontalSizer   LoadSettings_Sizer;
            PushButton        LoadCurrentSettings_PushButton;
            PushButton        LoadCurrentPageSettings_PushButton;
            PushButton        LoadDefaultPageSettings_PushButton;

      int NumberOfPages() const
      {
         return int( categories.Length() );
      }

      PreferencesCategoryPage* PageByIndex( int index );

      void HideAllPages();
      void PerformAllPageAdditionalUpdates();

   private:

      PreferencesInterface& window;

      typedef ReferenceArray<PreferencesCategory>  category_list;
      category_list categories;

      void InitializeCategories();
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void SelectPageByTreeNode( TreeBox::Node* node );

   void __Category_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __LoadSettings_ButtonClick( Button& sender, bool checked );

   friend class GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern PreferencesInterface* ThePreferencesInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PreferencesInterface_h

// ----------------------------------------------------------------------------
// EOF PreferencesInterface.h - Released 2019-01-21T12:06:41Z
