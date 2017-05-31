//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/ImageWindow.h - Released 2017-05-28T08:28:50Z
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

#ifndef __PCL_ImageWindow_h
#define __PCL_ImageWindow_h

/// \file pcl/ImageWindow.h

#include <pcl/Defs.h>

#include <pcl/Flags.h>
#include <pcl/ImageRenderingModes.h>

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Bitmap.h>
#include <pcl/ByteArray.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/ImageOptions.h>
#include <pcl/UIObject.h>
#include <pcl/View.h>

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

class RGBColorSystem;
class ICCProfile;
class ProcessInterface;

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ImageMode
 * \brief     GUI operation modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ImageMode::Invalid</td>          <td>Represents an invalid or unsupported mode.</td></tr>
 * <tr><td>ImageMode::Readout</td>          <td>Default mode, click to get real-time readouts.</td></tr>
 * <tr><td>ImageMode::ZoomIn</td>           <td>Click to zoom in the current view.</td></tr>
 * <tr><td>ImageMode::ZoomOut</td>          <td>Click to zoom out the current view.</td></tr>
 * <tr><td>ImageMode::Pan</td>              <td>Click and drag to pan the current view.</td></tr>
 * <tr><td>ImageMode::Center</td>           <td>Click to center the view at the mouse position.</td></tr>
 * <tr><td>ImageMode::NewPreview</td>       <td>Click and drag to define a new preview.</td></tr>
 * <tr><td>ImageMode::EditPreview</td>      <td>Click and drag to change an existing preview.</td></tr>
 * <tr><td>ImageMode::DynamicOperation</td> <td>Module-defined dynamic operation.</td></tr>
 * <tr><td>ImageMode::Default</td>          <td>Identifies the default GUI operation mode in the PixInsight core application (ImageMode::Readout).</td></tr>
 * </table>
 */
namespace ImageMode
{
   enum value_type
   {
      Invalid  = -1,    // Represents an invalid or unsupported mode
      Readout  = 0,     // Default mode, click to get real-time readouts
      ZoomIn,           // Click to zoom in the current view
      ZoomOut,          // Click to zoom out the current view
      Pan,              // Click and drag to pan the current view
      Center,           // Click to center the view at the mouse position
      NewPreview,       // Click and drag to define a new preview
      EditPreview,      // Click and drag to change an existing preview
      DynamicOperation, // Module-defined dynamic operation

      NumberOfModes,

      Default = Readout
   };

   /*!
    * Returns true iff the specified mode is a valid GUI mode when the active
    * view is a preview.
    */
   inline bool IsValidPreviewMode( value_type m )
   {
      return m != NewPreview && m != EditPreview;
   }

   /*!
    * Returns true iff the specified mode is a valid GUI mode when the active
    * view is a main view.
    */
   inline bool IsValidMainViewMode( value_type m )
   {
      return !IsValidPreviewMode( m );
   }
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::BackgroundBrush
 * \brief     Transparency background brushes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>BackgroundBrush::Small</td>              <td>Small chessboard pattern</td></tr>
 * <tr><td>BackgroundBrush::Medium</td>             <td>Medium chessboard pattern</td></tr>
 * <tr><td>BackgroundBrush::Large</td>              <td>Large chessboard pattern</td></tr>
 * <tr><td>BackgroundBrush::SmallCrossPattern</td>  <td>Small cross pattern</td></tr>
 * <tr><td>BackgroundBrush::MediumCrossPattern</td> <td>Medium cross pattern</td></tr>
 * <tr><td>BackgroundBrush::LargeCrossPattern</td>  <td>Large cross pattern</td></tr>
 * <tr><td>BackgroundBrush::SmallDiagPattern</td>   <td>Small diagonal pattern</td></tr>
 * <tr><td>BackgroundBrush::MediumDiagPattern</td>  <td>Medium diagonal pattern</td></tr>
 * <tr><td>BackgroundBrush::LargeDiagPattern</td>   <td>Large diagonal pattern</td></tr>
 * <tr><td>BackgroundBrush::Solid</td>              <td>Solid brush with the foreground color</td></tr>
 * <tr><td>BackgroundBrush::Default</td>            <td>Identifies the default transparency background brush used by the PixInsight core application. Currently this corresponds to BackgroundBrush::Small.</td></tr>
 * </table>
 */
namespace BackgroundBrush
{
   enum value_type
   {
      Small,               // Small chessboard pattern
      Medium,              // Medium chessboard pattern
      Large,               // Large chessboard pattern
      SmallCrossPattern,   // Small cross pattern
      MediumCrossPattern,  // Medium cross pattern
      LargeCrossPattern,   // Large cross pattern
      SmallDiagPattern,    // Small diagonal pattern
      MediumDiagPattern,   // Medium diagonal pattern
      LargeDiagPattern,    // Large diagonal pattern
      Solid,               // Solid brush with the foreground color

      NumberOfBrushes,

      Default = Small
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::TransparencyMode
 * \brief     Transparency rendering modes
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>TransparencyMode::Hide</td>            <td>Don't show transparencies (ignore alpha channels).</td></tr>
 * <tr><td>TransparencyMode::BackgroundBrush</td> <td>Use the transparency background brush.</td></tr>
 * <tr><td>TransparencyMode::Color</td>           <td>Use an opaque background color.</td></tr>
 * <tr><td>TransparencyMode::Default</td>         <td>Identifies the default transparency rendering mode used by the PixInsight core application. Currently this corresponds to TransparencyMode::BackgroundBrush.</td></tr>
 * </table>
 */
namespace TransparencyMode
{
   enum value_type
   {
      Hide,                // Don't show transparencies
      BackgroundBrush,     // Use the transparency background brush
      Color,               // Use an opaque background color

      NumberOfModes,

      Default = BackgroundBrush
   };
};

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::UndoFlag
 * \brief     History data item specifiers for ProcessImplementation::UndoMode().
 *
 * Use %UndoFlag constants to make up return values for
 * ProcessImplementation::UndoMode(). That member function specifies the data
 * that the PixInsight core application must save to swap files before
 * executing an instance of a given process. Saved undo data are used to
 * retrieve a history state of an image, e.g. for undo and redo operations.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>UndoFlag::DefaultMode</td>          <td>Save pixel data and previews</td></tr>
 * <tr><td>UndoFlag::PixelData</td>            <td>Save pixel data</td></tr>
 * <tr><td>UndoFlag::RGBWS</td>                <td>RGB Working Space data</td></tr>
 * <tr><td>UndoFlag::ICCProfile</td>           <td>ICC profile</td></tr>
 * <tr><td>UndoFlag::Keywords</td>             <td>%FITS keywords</td></tr>
 * <tr><td>UndoFlag::FormatData</td>           <td>Format-specific data</td></tr>
 * <tr><td>UndoFlag::ImageId</td>              <td>%Image identifier</td></tr>
 * <tr><td>UndoFlag::Resolution</td>           <td>%Image resolution</td></tr>
 * <tr><td>UndoFlag::All</td>                  <td>Save all data items</td></tr>
 * <tr><td>UndoFlag::ExcludePreviews</td>      <td>Don't save state of previews</td></tr>
 * <tr><td>UndoFlag::ExcludeMaskRelations</td> <td>Don't save masking dependencies</td></tr>
 * </table>
 */
namespace UndoFlag
{
   /*
    * ### TODO: PCL 2.x: Get rid of all project depencies on UndoFlags.
    */
   enum mask_type
   {
      DefaultMode          = 0x00000000,  // Save pixel data and previews
      PixelData            = 0x00000001,  // Save pixel data
      RGBWS                = 0x00000002,  // RGB Working Space data
      ICCProfile           = 0x00000004,  // ICC profile
      Keywords             = 0x00000008,  // %FITS keywords
      //Metadata             = 0x00000010,  // ### DEPRECATED - Keep unused for now, for compatibility with existing projects
      FormatData           = 0x00000020,  // Format-specific data
      ImageId              = 0x00000040,  // %Image identifier
      Resolution           = 0x00000080,  // %Image resolution
      All                  = 0x000FFFFF,  // Save all data items
      ExcludePreviews      = 0x80000000,  // Don't save state of previews
      ExcludeMaskRelations = 0x40000000   // Don't save masking dependencies
   };
}

/*!
 * \class pcl::UndoFlags
 * \brief A collection of history data item specifiers.
 */
typedef Flags<UndoFlag::mask_type>  UndoFlags;

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class ImageWindow
 * \brief High-level interface to an image window object in the PixInsight core
 *        application.
 *
 * ### TODO: Write a detailed description for %ImageWindow.
 *
 * \sa View
 */
class PCL_CLASS ImageWindow : public UIObject
{
public:

   /*!
    * Represents a GUI working mode. Valid modes are defined in the ImageMode
    * namespace.
    */
   typedef ImageMode::value_type          gui_mode;

   /*!
    * Represents a display image channel supported by the PixInsight core
    * application. Valid display channels and components are defined in the
    * DisplayChannel namespace.
    */
   typedef DisplayChannel::value_type     display_channel;

   /*!
    * Represents a mask rendering mode supported by the PixInsight core
    * application. Valid mask rendering modes are defined in the MaskMode
    * namespace.
    */
   typedef MaskMode::value_type           mask_mode;

   /*!
    * Represents a transparency background brush supported by the PixInsight
    * core application. Valid transparency brushes are defined in the
    * BackgroundBrush namespace.
    */
   typedef BackgroundBrush::value_type    background_brush;

   /*!
    * Represents a transparency rendering mode supported by the PixInsight core
    * application. Valid transparency rendering modes are defined in the
    * TransparencyMode namespace.
    */
   typedef TransparencyMode::value_type   transparency_mode;

   /*!
    * Constructs a null image window. A null %ImageWindow does not correspond
    * to an existing image window in the PixInsight core application.
    */
   ImageWindow() = default;

   /*!
    * Creates a new image window with the specified image parameters.
    *
    * \param width   %Image width in pixels. Must be >= 1. There is no specific
    *                upper limit.
    *
    * \param height  %Image height in pixels. Must be >= 1. There is no
    *                specific upper limit.
    *
    * \param numberOfChannels    Number of channels in the image. Should not be
    *             less than the number of nominal channels: 1 for grayscale
    *             images, 3 for color images. Channels in excess of nominal
    *             channels are generated as alpha channels. This parameter is
    *             one by default.
    *
    * \param bitsPerSample    Number of bits per sample. Supported values are
    *             8, 16 and 32 for integer images; 32 and 64 for floating point
    *             images. This parameter is 32 by default.
    *
    * \param floatSample      True to create a floating point image; false to
    *             create an integer image. This parameter is true by default.
    *
    * \param color   True to create a RGB color image; false to create a
    *             grayscale image. This parameter is false by default.
    *
    * \param initialProcessing   True if the core application should assign a
    *             ProcessContainer instance to hold the initial processing for
    *             this image. The initial processing items will be
    *             automatically generated by the core application as a function
    *             of the current processing thread context. This parameter is
    *             true by default.
    *
    * \param id   Identifier for the new image. If an empty string is
    *             specified, the core application will assign an automatically
    *             generated identifier. If the specified identifier is not
    *             unique, the core application will make it unique by appending
    *             an automatically generated suffix. This parameter is an empty
    *             string by default.
    *
    * The newly created image is initialized with zero pixel sample values
    * (black pixels). This includes all nominal and alpha channels.
    *
    * The new image window will be hidden. To make it visible on the core
    * application, you must call its Show() member function explicitly.
    */
   ImageWindow( int width, int height, int numberOfChannels = 1,
                int bitsPerSample = 32, bool floatSample = true, bool color = false,
                bool initialProcessing = true,
                const IsoString& id = IsoString() );

   template <class S>
   ImageWindow( int width, int height, int numberOfChannels,
                int bitsPerSample, bool floatSample, bool color,
                bool initialProcessing,
                const S& id ) :
      ImageWindow( width, height, numberOfChannels, bitsPerSample, floatSample, color, initialProcessing, IsoString( id ) )
   {
   }

   /*!
    * Copy constructor. Constructs an %ImageWindow object as an alias of an
    * existing %ImageWindow object.
    *
    * It cannot be overemphasized that this constructor <em>does not create a
    * new image window</em> in the PixInsight core application. It only creates
    * an \e alias object for an existing image window <em>in the calling
    * module</em>. In all respects, the alias and aliased objects are
    * completely interchangeable; their behaviors are exactly identical since
    * they refer to the same server-side object.
    */
   ImageWindow( const ImageWindow& w ) : UIObject( w )
   {
   }

   /*!
    * Move constructor.
    */
   ImageWindow( ImageWindow&& x ) : UIObject( std::move( x ) )
   {
   }

   /*!
    * Destroys this %ImageWindow object.
    *
    * This destructor does \e not destroy the actual image window object, which
    * is part of the PixInsight core application. Only the managed alias object
    * living in the calling module is destroyed.
    */
   virtual ~ImageWindow()
   {
   }

   /*!
    * Copy assignment operator. Detaches this object from its previously
    * referenced server-side image window and makes it an alias of the
    * specified \a window. Returns a reference to this object.
    *
    * When this operator is used, the calling object is detached from the
    * previously referenced window. Note that an image window cannot be
    * destroyed when it becomes unreferenced, as it is an internal UI object
    * that belongs to the core application, not to the calling module, even if
    * the window has been created by the calling module.
    */
   ImageWindow& operator =( const ImageWindow& window )
   {
      Assign( window );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ImageWindow& operator =( ImageWindow&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Returns a reference to a null %ImageWindow instance. A null %ImageWindow
    * does not correspond to an existing image window in the PixInsight core
    * application.
    */
   static ImageWindow& Null();

   /*!
    * Opens an image file and loads one or more images into new image windows.
    *
    * \param url  A path specification or URL that points to an image file to
    *             be loaded by the PixInsight core application. The file name
    *             \e must include a file extension, since it is the only way to
    *             select a file format, necessary to read and decode images.
    *
    * \param id   Identifier for the new image(s). If an empty string is
    *             specified, the application will assign an automatically
    *             generated identifier. If the specified identifier is not
    *             unique, the application will make it unique by appending an
    *             automatically generated suffix. This parameter is an empty
    *             string by default.
    *
    * \param formatHints   A string of format hints that will be sent to the
    *             file format instance created to load the image. Format hints
    *             are useful to modify the behavior of a file format suuport
    *             module lo load a specific image, overriding global
    *             preferences and format settings. See the
    *             FileFormatInstance::Open() member function for more
    *             information on format hints. This parameter is an empty
    *             string by default.
    *
    * \param asCopy  True to open the images as \e copies. The %File > Save
    *             command (and all related menu items) is always disabled for
    *             images loaded as copies in the PixInsight core application.
    *             This means that the only way to write an image copy is the
    *             %File > Save As command. This is a security enhancement to
    *             avoid %File > Save accidents, e.g. by pressing Ctrl+S
    *             inattentively. The same applies to the \c save internal
    *             command-line command, which requires an explicit file path
    *             specification for image copies. This parameter is false by
    *             default.
    *
    * \param allowMessages    True to allow the core application to show
    *             warning message boxes (e.g., to inform about inexact read
    *             operations) and color management dialog boxes (upon
    *             ICC profile mismatches or missing ICC profiles). If this
    *             parameter is false, no dialog box or warning message box will
    *             be shown, and default actions will be taken as appropriate
    *             for conflicting ICC profiles. However, all warning messages
    *             will be sent to the processing console when this parameter is
    *             false. Error messages will always be shown. This parameter is
    *             true by default.
    *
    * The PixInsight core application will find and select a file format to
    * read the specified image file, among the installed file format modules.
    * Format selection is exclusively a function of the specified file
    * extension (e.g., .fit will select the %FITS format, .tif the %TIFF
    * format, .jpg for %JPEG, and so on).
    *
    * %File and directory paths in the PixInsight environment employ UNIX
    * directory separators (the slash character, '/') on all supported
    * platforms. As an exception, on Windows PCL allows you to use also DOS
    * separators (backslash, '\'), but their use is discouraged since its
    * support might be discontinued in future PCL versions.
    *
    * URLs must be valid and conform to the URI specification from RFC 3986
    * (Uniform Resource Identifier: Generic Syntax). At least the file, HTTP,
    * HTTPS, FTP and FTPS protocols are supported. More protocols may be
    * available, depending on PixInsight core versions.
    *
    * Returns an array of %ImageWindow objects. Each object in the array
    * corresponds to an image loaded into a new image window from the specified
    * file. In general, the returned array will contain only one object, since
    * most image file formats can store just a single image. For formats that
    * can store multiple images, more than one %ImageWindow can be created as a
    * result of opening a single file.
    *
    * Newly created image windows will be hidden. To make them visible on the
    * core application's workspace, you must call their ImageWindow::Show()
    * member function explicitly.
    */
   static Array<ImageWindow> Open( const String& url,
                                   const IsoString& id = IsoString(),
                                   const IsoString& formatHints = IsoString(),
                                   bool asCopy = false,
                                   bool allowMessages = true );

   template <class S1, class S2>
   static Array<ImageWindow> Open( const String& url,
                                   const S1& id,
                                   const S2& formatHints,
                                   bool asCopy = false,
                                   bool allowMessages = true )
   {
      return Open( url, IsoString( id ), IsoString( formatHints ), asCopy, allowMessages );
   }

   /*!
    * Returns true iff the image in this window has not been loaded from a disk
    * file, but created as a new image in the core application (e.g., by the
    * NewImage standard process).
    */
   bool IsNew() const;

   /*!
    * Returns true iff the image in this window has been loaded from a file
    * <em>as a copy</em>.
    *
    * The %File > Save command (and all related menu items) is always disabled
    * for images loaded as copies in the PixInsight core application. This
    * means that the only way to write an image copy is the %File > Save As
    * command. This is a security enhancement to avoid %File > Save accidents,
    * e.g. by pressing Ctrl+S inattentively. The same applies to the \c save
    * command-line internal command, which requires an explicit file path
    * specification for image copies.
    */
   bool IsACopy() const;

   /*!
    * Returns the full file path to the image file loaded in this image
    * window, or an empty string if this window holds is a \e new image.
    *
    * \note When an image has been loaded from a remote location (by specifying
    * a valid URL in a call to ImageWindow::Open(), for example), the returned
    * path corresponds to the file that has been downloaded to the local
    * filesystem upon window creation, not to the original URL.
    */
   String FilePath() const;

   /*!
    * Returns the URL of the remote resource from which this image has been
    * downloaded, or an empty string if this window has not been downloaded
    * from a remote location.
    */
   String FileURL() const;

   /*!
    * Returns true iff this image has been downloaded from a remote location, in
    * which case the source URL can be obtained by calling FileURL().
    */
   bool HasURL() const
   {
      return !FileURL().IsEmpty();
   }

   /*!
    * Returns an ImageOptions structure with information about the image in
    * this image window.
    */
   ImageOptions FileInfo() const;

   /*!
    * Returns the <em>modification counter</em> for this image window. The
    * modification counter contains the amount of changes made to the image
    * since it was created, loaded from a disk file, or saved to a disk file.
    */
   size_type ModifyCount() const;

   /*!
    * Returns true iff the image in this image window has been modified and is
    * unsaved. An image is modified if its modification counter is not zero.
    */
   bool IsModified() const
   {
      return ModifyCount() > 0;
   }

   /*!
    * Requests closing this image window. Returns true if the window was
    * actually closed.
    *
    * When an image window is closed, its image and all of its numerous
    * associated resources are destroyed. The close operation is irreversible
    * in PixInsight.
    *
    * If the image in this window is not modified, the window will be closed
    * and true will be returned.
    *
    * If the image in this window is modified and has not been saved to a disk
    * file, the PixInsight core application will ask the user if it is ok to
    * throw away the changes by closing this window. If the user answers
    * affirmatively, the window will be closed and this function will return
    * true. If the user says no, the window will not be closed and the
    * function will return false.
    */
   bool Close();

   /*!
    * Returns the main view of this image window.
    *
    * The main view holds the entire image of this window. There is only one
    * main view in an image window.
    */
   View MainView() const;

   /*!
    * Returns the current view of this image window.
    *
    * The current view is the view currently selected for direct user
    * interaction in the core application GUI for this image window. The
    * current view receives cursor and pointing device (mouse) events.
    */
   View CurrentView() const;

   /*!
    * Selects the specified view as the current view in this image window.
    */
   void SelectView( View& );

   /*!
    * Selects the main view as the current view in this image window.
    */
   void SelectMainView()
   {
      View v( MainView() ); SelectView( v );
   }

   /*!
    * Destroys all existing properties for all views in this image window.
    */
   void PurgeProperties();

   /*!
    * Returns true iff the specified view is a view of this image window, that
    * is, either the main view of this image window, or one of its previews.
    */
   bool IsValidView( const View& ) const;

   /*!
    * Returns the number of previews currently defined in this image window.
    */
   int NumberOfPreviews() const;

   /*!
    * Returns true iff there are one or more previews currently defined in this
    * image window.
    */
   bool HasPreviews() const
   {
      return NumberOfPreviews() > 0;
   }

   /*!
    * Returns a dynamic array of View objects corresponding to the previews
    * currently defined in this image window. If this window has no previews,
    * an empty array is returned.
    */
   Array<View> Previews() const;

   /*!
    * Finds a preview by its identifier. If the specified \a previewId
    * identifier corresponds to an existing preview in this image window, it
    * is returned as a View object. If no preview in this image window has the
    * specified \a previewId identifier, or if this window has no previews,
    * this function returns an alias of View::Null().
    */
   View PreviewById( const IsoString& previewId ) const;

   template <class S>
   View PreviewById( const S& previewId ) const
   {
      return PreviewById( IsoString( previewId ) );
   }

   /*!
    * Returns true if a preview with the specified \a previewId identifier is
    * currently defined in this image window; false if no preview with such
    * identifier exists in this window, or if this window has no previews.
    */
   bool PreviewExists( const IsoString& previewId ) const
   {
      return !IsNull() && !PreviewById( previewId ).IsNull();
   }

   template <class S>
   bool PreviewExists( const S& previewId ) const
   {
      return PreviewExists( IsoString( previewId ) );
   }

   /*!
    * Returns the currently selected preview, or an alias of View::Null() if
    * no preview is currently selected in this image window.
    *
    * \note The <em>selected preview</em> in an image window has nothing to do
    * with the <em>current view</em> in the same image window. The selected
    * preview corresponds to a selection on the main view of the window, used
    * to resize and move preview rectangles, and also to perform preview
    * actions (undo/redo, reset, store, restore, etc) without needing to select
    * a preview as the window's current view.
    */
   View SelectedPreview() const;

   /*!
    * Causes this window to select the specified view as its <em>selected
    * preview</em>. Refer to the SelectedPreview() function for further
    * information on the concept of selected preview.
    */
   void SelectPreview( View& );

   /*!
    * Causes this window to select the view with the specified identifier as
    * its current <em>selected preview</em>. Refer to the SelectedPreview()
    * function for further information on the concept of selected preview.
    */
   void SelectPreview( const IsoString& previewId )
   {
      View v( PreviewById( previewId ) ), SelectPreview( v );
   }

   template <class S>
   void SelectPreview( const S& previewId )
   {
      SelectPreview( IsoString( previewId ) );
   }

   /*!
    * Creates a new preview in this image window. Returns the newly created
    * preview as a View object.
    *
    * Calling this function is equivalent to:
    *
    * \code CreatePreview( r.x0, r.y0, r.x1, r.y1, previewId ); \endcode
    */
   View CreatePreview( const Rect& r, const IsoString& previewId = IsoString() )
   {
      return CreatePreview( r.x0, r.y0, r.x1, r.y1, previewId );
   }

   template <class S>
   View CreatePreview( const Rect& r, const S& previewId )
   {
      return CreatePreview( r, IsoString( previewId ) );
   }

   /*!
    * Creates a new preview in this image window. Returns the newly created
    * preview as a View object.
    *
    * \param x0,y0   Position (horizontal, vertical) of the left-top corner of
    *             the preview rectangle in image coordinates.
    *
    * \param x1,y1   Position (horizontal, vertical) of the right-bottom corner
    *             of the preview rectangle in image coordinates.
    *
    * \param previewId     Preview identifier. If an empty string is specified,
    *             the core application will generate an automatic identifier
    *             for the newly created preview. If the specified identifier is
    *             already in use, the core application will make it unique by
    *             appending an automatically generated suffix.
    *
    * The preview rectangle must define a non-empty rectangular area
    * completely included in the boundaries of the image in this window.
    */
   View CreatePreview( int x0, int y0, int x1, int y1, const IsoString& previewId = IsoString() );

   template <class S>
   View CreatePreview( int x0, int y0, int x1, int y1, const S& previewId )
   {
      return CreatePreview( x0, y0, x1, y1, IsoString( previewId ) );
   }

   /*!
    * Modifies the identifier and rectangular area properties of an existing
    * preview.
    *
    * Calling this function is equivalent to:
    *
    * \code ModifyPreview( previewId, r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void ModifyPreview( const IsoString& previewId, const Rect& r, const IsoString& newId = IsoString() )
   {
      ModifyPreview( previewId, r.x0, r.y0, r.x1, r.y1, newId );
   }

   template <class S>
   void ModifyPreview( const S& previewId, const Rect& r )
   {
      ModifyPreview( IsoString( previewId ), r );
   }

   template <class S1, class S2>
   void ModifyPreview( const S1& previewId, const Rect& r, const S2& newId )
   {
      ModifyPreview( IsoString( previewId ), r, IsoString( newId ) );
   }

   /*!
    * Modifies the identifier and rectangular area properties of an existing
    * preview.
    *
    * \param previewId  Identifier of a preview whose properties will be
    *             modified.
    *
    * \param x0,y0   Position (horizontal, vertical) of the left-top corner of
    *             the new preview rectangle in image coordinates.
    *
    * \param x1,y1   Position (horizontal, vertical) of the right-bottom corner
    *             of the new preview rectangle in image coordinates.
    *
    * \param newId   New preview identifier. If an empty string is specified,
    *             this function will not modify the current preview identifier.
    *             If the specified identifier is already in use in this window,
    *             the core application will make it unique by appending an
    *             automatically generated suffix.
    *
    * The preview rectangle must define a rectangular area of at least one
    * pixel, completely included in the boundaries of the image in this window.
    */
   void ModifyPreview( const IsoString& previewId,
                       int x0, int y0, int x1, int y1, const IsoString& newId = IsoString() );

   template <class S>
   void ModifyPreview( const S& previewId, int x0, int y0, int x1, int y1 )
   {
      ModifyPreview( IsoString( previewId ), x0, y0, x1, y1 );
   }

   template <class S1, class S2>
   void ModifyPreview( const S1& previewId, int x0, int y0, int x1, int y1, const S2& newId )
   {
      ModifyPreview( IsoString( previewId ), x0, y0, x1, y1, IsoString( newId ) );
   }

   /*!
    * Returns the rectangular area in image coordinates of a preview with the
    * specified \a previewId identifier.
    *
    * If no preview is defined in this window with the specified identifier,
    * this function returns an empty rectangle.
    */
   Rect PreviewRect( const IsoString& previewId ) const;

   template <class S>
   Rect PreviewRect( const S& previewId ) const
   {
      return PreviewRect( IsoString( previewId ) );
   }

   /*!
    * Destroys an existing preview in this window and frees all of its
    * associated view resources.
    *
    * \param      previewId Identifier of the preview that will be destroyed.
    *
    * The PixInsight core application might ask the user for permission before
    * attempting to destroy a preview in an image window.
    */
   void DeletePreview( const IsoString& previewId );

   template <class S>
   void DeletePreview( const S& previewId )
   {
      DeletePreview( IsoString( previewId ) );
   }

   /*!
    * Destroys all previews in this image window.
    *
    * The PixInsight core application reserves the right to ask the user for
    * permission before attempting to destroy all previews in an image window.
    */
   void DeletePreviews();

   /*!
    * Retrieves the pixel sample data format used by the image in this window.
    *
    * \param[out] bitsPerSample     On output, will be equal to the number of
    *             bits per sample in the image in this window. Can be 8, 16 or
    *             32 for an integer image; 32 or 64 for a floating point image.
    *
    * \param[out] floatSample    On output, will be true if the image in this
    *             window uses floating point samples; false if it uses integer
    *             samples.
    */
   void GetSampleFormat( int& bitsPerSample, bool& floatSample ) const;

   /*!
    * Converts the image in this window to the specified sample data format.
    *
    * \param bitsPerSample    Number of bits per sample. Can be 8, 16 or 32 for
    *             an integer image; 32 or 64 for a floating point image.
    *
    * \param floatSample   True to transform this image to a floating point
    *             data format; false to transform this image to an integer data
    *             format.
    *
    * This function does nothing if the image in this window already has the
    * specified sample data format.
    */
   void SetSampleFormat( int bitsPerSample, bool floatSample );

   /*!
    * Returns the image window that is currently acting as a mask for this
    * window, or ImageWindow::Null() if this window has no assigned mask.
    */
   ImageWindow Mask() const;

   /*!
    * Selects an image as the current mask for this image window.
    *
    * \param w    Reference to an image window whose image will be selected as
    *             a mask for this window.
    *
    * \param inverted   True to activate mask inversion.
    */
   void SetMask( ImageWindow& w, bool inverted = false );

   /*!
    * Removes the current mask selection of this image window.
    */
   void RemoveMask()
   {
      SetMask( Null() );
   }

   /*!
    * Returns true iff mask inversion is currently active for this image window.
    */
   bool IsMaskInverted() const;

   /*!
    * Activates or deactivates mask inversion for this image window.
    */
   void InvertMask( bool invert = true )
   {
      ImageWindow mask = Mask();
      SetMask( mask, invert );
   }

   /*!
    * Returns true iff the mask is current enabled for this image window.
    */
   bool IsMaskEnabled() const;

   /*!
    * Enables the mask of this image window.
    */
   void EnableMask( bool = true );

   /*!
    * Disables the mask of this image window.
    */
   void DisableMask( bool disable = true )
   {
      EnableMask( !disable );
   }

   /*!
    * Returns true iff the mask is currently visible for this image window.
    */
   bool IsMaskVisible() const;

   /*!
    * Shows the mask of this image window, i.e. enables mask visibility.
    */
   void ShowMask( bool = true );

   /*!
    * Hides the mask of this image window, i.e. disables mask visibility.
    */
   void HideMask( bool hide = true )
   {
      ShowMask( !hide );
   }

   /*!
    * Returns true iff a specified image window could be selected as a mask for
    * this image window.
    *
    * For a given image \a M to be a valid mask for an image \a I:
    *
    * \li Both \a I and \a M must have identical dimensions.
    * \li If \a I is a RGB color image, \a M can be either a grayscale image
    *     or a RGB color image.
    * \li If \a I is a grayscale image, \a M must be also a grayscale image.
    */
   bool IsMaskCompatible( const ImageWindow& );

   /*!
    * Returns true iff this image window is acting as a mask for other image
    * window(s).
    */
   bool HasMaskReferences() const;

   /*!
    * Removes all mask references for this image window.
    *
    * All images that were masked by this window will be unmasked after calling
    * this function. Use it with great care, since removing masking relations
    * without direct user intervention is potentially dangerous.
    */
   void RemoveMaskReferences();

   /*!
    * Updates the screen renditions of all visible image windows that are
    * currently masked by this window. This includes only masked image windows
    * with enabled mask visibility.
    */
   void UpdateMaskReferences();

   /*!
    * Obtains a copy of the RGB working space (RGBWS) associated with this
    * image window.
    *
    * \param[out] rgbws    Reference to an object where the parameters of the
    *             current RGBWS in this window will be copied.
    */
   void GetRGBWS( RGBColorSystem& rgbws ) const;

   /*!
    * Sets the RGB working space (RGBWS) for this image window.
    *
    * \param rgbws   Reference to a RGBWS that will be associated with this
    *             image window.
    */
   void SetRGBWS( const RGBColorSystem& rgbws );

   /*!
    * Returns true iff this image window is associated with the global RGB
    * working space (RGBWS).
    *
    * See the documentation for GetGlobalRGBWS() for a description of the
    * global RGBWS in PixInsight.
    */
   bool UsingGlobalRGBWS() const;

   /*!
    * Associates this image window with the global RGB working space.
    *
    * See the documentation for GetGlobalRGBWS() for a description of the
    * global RGBWS in PixInsight.
    */
   void UseGlobalRGBWS();

   /*!
    * Obtains a copy of the current global RGB working space (RGBWS) in the
    * PixInsight core application.
    *
    * \param[out] rgbws    Reference to an object where the parameters of the
    *             current global RGBWS will be copied.
    *
    * The global RGBWS is the default RGBWS that is associated with newly
    * created image windows (both as new images and when images are loaded
    * from disk files). Global RGBWS parameters can be modified by the user
    * through the standard RGBWorkingSpace process in the core application.
    */
   static void GetGlobalRGBWS( RGBColorSystem& rgbws );

   /*!
    * Sets the parameters of the global RGB working space (RGBWS) in the core
    * PixInsight application.
    *
    * \param rgbws   Reference to an object whose parameters will be copied to
    *             the global RGBWS.
    *
    * See the documentation for GetGlobalRGBWS() for a description of the
    * global RGBWS in PixInsight.
    */
   static void SetGlobalRGBWS( const RGBColorSystem& rgbws );

   /*!
    * Returns true iff color management is active for this image window.
    */
   bool IsColorManagementEnabled() const;

   /*!
    * Enables color management for this image window.
    *
    * Color management uses ICC profiles for this image (either a profile
    * associated with this image or a global profile) and the monitor device to
    * control how this image is rendered on the screen. Color management
    * transformations are applied to yield a visual representation that takes
    * into account the actual meaning of numerical pixel sample values,
    * considering the chromatic responses of the screen and of the devices and
    * color spaces that have originated this image.
    *
    * If color management is disabled for an image, its pixel sample values are
    * sent directly to the screen, avoiding ICC profile transformations. This
    * improves output performance, but may give a false representation of the
    * image on the screen.
    *
    * If this image window is visible, its screen rendition is updated
    * immediately after calling this function.
    */
   void EnableColorManagement( bool = true );

   /*!
    * Disables color management for this image window.
    *
    * See the documentation for EnableColorManagement() for additional
    * information on color management in PixInsight.
    *
    * If this image window is visible, its screen rendition is updated
    * immediately after calling this function.
    */
   void DisableColorManagement( bool disable = true )
   {
      EnableColorManagement( !disable );
   }

   /*!
    * Returns true iff <em>soft-proofing</em> is currently enabled for this
    * image window.
    */
   bool IsProofingEnabled() const;

   /*!
    * Enables <em>soft-proofing</em> for this image window.
    *
    * Color proofing is used to emulate an output device (usually a printer) on
    * the screen. Proofing requires three ICC profiles: the image profile
    * (either an embedded profile or the default profile), the monitor profile,
    * and a <em>proofing profile</em> that describes the emulated device. Color
    * proofing is useful to preview the final printed result without rendering
    * images to physical media.
    *
    * If this image window is visible and has color management enabled, its
    * screen rendition is updated immediately after calling this function.
    */
   void EnableProofing( bool = true );

   /*!
    * Disables <em>soft-proofing</em> for this image window.
    *
    * See the documentation for EnableProofing() for additional information on
    * color proofing.
    *
    * If this image window is visible and has color management enabled, its
    * screen rendition is updated immediately after calling this function.
    */
   void DisableProofing( bool disable = true )
   {
      EnableProofing( !disable );
   }

   /*!
    * Returns true iff the <em>gamut check</em> soft-proofing feature is
    * currently enabled for this image window.
    */
   bool IsGamutCheckEnabled() const;

   /*!
    * Enables the <em>gamut check</em> proofing feature for this image window.
    *
    * The gamut check feature represents <em>out-of-gamut colors</em> (i.e.,
    * colors that are not defined in the color space of the image) with a
    * special color (selectable via global color management preferences) for
    * quick visual detection.
    *
    * If this image window is visible, has color management enabled, and has
    * proofing enabled, its screen rendition is updated immediately after
    * calling this function. If color proofing is disabled, calling this
    * function has no effect.
    */
   void EnableGamutCheck( bool = true );

   /*!
    * Disables the <em>gamut check</em> proofing feature for this image window.
    *
    * See the documentation for EnableGamutCheck() for additional information
    * on the gamut check color proofing feature.
    *
    * If this image window is visible, has color management enabled, and has
    * proofing enabled, its screen rendition is updated immediately after
    * calling this function. If color proofing is disabled, calling this
    * function has no effect.
    */
   void DisableGamutCheck( bool disable = true )
   {
      EnableGamutCheck( !disable );
   }

   /*!
    * Allows enabling or disabling all color management features at once.
    *
    * \param cmEnabled     Enables (if true) or disables (if false) color
    *                      management for this image window.
    *
    * \param proofing      Enables (if true) or disables (if false)
    *                      soft-proofing for this image window.
    *
    * \param gamutCheck    Enables (if true) or disables (if false) the
    *                      gamut check proofing feature for this image window.
    *                      The state of this argument has no meaning if the
    *                      \a proofing parameter is false.
    *
    * If this image window is visible, its screen rendition is updated
    * immediately after calling this function.
    */
   void SetColorManagementFeatures( bool cmEnabled, bool proofing, bool gamutCheck );

   /*!
    * Obtains a copy of the current ICC profile associated with this image.
    *
    * \param[out] icc   Reference to an object where a copy of the current ICC
    *             profile in this image will be stored.
    */
   bool GetICCProfile( ICCProfile& icc ) const;

   /*!
    * Sets the current ICC profile for this image window as a copy of an
    * existing ICC profile.
    *
    * \param icc  Reference to an object that will be used to set the current
    *             ICC profile for this image.
    *
    * If this image window is visible and has color management enabled, its
    * screen rendition is updated immediately after calling this function.
    */
   void SetICCProfile( const ICCProfile& icc );

   /*!
    * Sets the current ICC profile for this image window as a copy of an
    * ICC profile loaded from a disk file.
    *
    * \param filePath   A file path specification to an ICC profile that will
    *             be loaded and copied to the ICC profile of this image window.
    *
    * If this image window is visible and has color management enabled, its
    * screen rendition is updated immediately after calling this function.
    */
   void SetICCProfile( const String& filePath );

   /*!
    * Causes this image window to be associated with the default ICC profile
    * globally defined in the PixInsight core application.
    *
    * If this image window has its own associated ICC profile, this funtion
    * destroys it and frees its allocated space.
    *
    * If this image window is visible and has color management enabled, its
    * screen rendition is updated immediately after calling this function.
    */
   void DeleteICCProfile();

   /*!
    * Returns a copy of the %FITS header keywords currently defined for the
    * image in this window. Returns an empty array if this image has no %FITS
    * keywords.
    */
   FITSKeywordArray Keywords() const;

   /*!
    * Obtains a copy of the %FITS header keywords currently defined for the
    * image in this window. Returns true if this image has %FITS keywords.
    *
    * \param[out] keywords Reference to a container that will receive a copy of
    *                      the %FITS keywords defined in this image.
    *
    * \deprecated This member function has been deprecated. It is left only to
    * keep existing code working. Use Keywords() in newly produced code.
    */
   bool GetKeywords( FITSKeywordArray& keywords ) const
   {
      keywords = Keywords();
      return !keywords.IsEmpty();
   }

   /*!
    * Sets the %FITS header keywords for this image as a copy of the set of
    * keywords in the specified container.
    *
    * \param keywords   Reference to a %FITS keyword array whose contents will
    *                   be copied to the list of %FITS keywords of this image.
    *
    * The previous set of %FITS keywords in this image, if any, is lost and
    * replaced by the specified set.
    */
   void SetKeywords( const FITSKeywordArray& keywords );

   /*!
    * Deletes all existing %FITS header keywords in this image window.
    */
   void ResetKeywords();

   /*!
    * Gets the current image resolution parameters in this image window.
    *
    * \param[out] xRes  Reference to a variable where the horizontal resolution
    *             will be stored.
    *
    * \param[out] yRes  Reference to a variable where the vertical resolution
    *             will be stored.
    *
    * \param[out] metric   Reference to a variable whose stored value will
    *             indicate the current resolution units in this image window.
    *             If this variable is true, resolution is expressed in pixels
    *             per centimeter; if it is false, resolution is expressed in
    *             pixels per inch.
    *
    * Resolution is expressed in pixels per resolution unit (centimeter if
    * \a metric is true; inch if \a metric is false).
    */
   void GetResolution( double& xRes, double& yRes, bool& metric ) const;

   /*!
    * Sets image resolution parameters for this image window.
    *
    * \param xRes    Horizontal resolution in pixels per resolution unit.
    *                Must be > 0.
    *
    * \param yRes    Vertical resolution in pixels per resolution unit.
    *                Must be > 0.
    *
    * \param metric  Indicates the resolution unit in which resolution is
    *             expressed for both axes. If this parameter is true,
    *             resolution is expressed in pixels per centimeter; if it is
    *             false, resolution is in pixels per inch.
    */
   void SetResolution( double xRes, double yRes, bool metric = false );

   /*!
    * Sets equal image resolution parameters for both axes in this image
    * window.
    *
    * \param r    Resolution in pixels per resolution unit. This value is to be
    *             applied to both axes (horizontal, vertical) of the image in
    *             this window.
    *
    * \param metric  Indicates the resolution unit in which resolution is
    *             expressed. If this parameter is true, resolution is expressed
    *             in pixels per centimeter; if it is false, resolution is in
    *             pixels per inch.
    */
   void SetResolution( double r, bool metric = false )
   {
      SetResolution( r, r, metric );
   }

   /*!
    * Gets the current default image resolution parameters.
    *
    * \param[out] xRes     Reference to a variable where the default horizontal
    *             resolution will be stored.
    *
    * \param[out] yRes     Reference to a variable where the default vertical
    *             resolution will be stored.
    *
    * \param[out] metric   Reference to a variable whose stored value will
    *             indicate the current default resolution units. If this
    *             variable is true, default resolution is expressed in pixels
    *             per centimeter; if it is false, resolution is expressed in
    *             pixels per inch.
    *
    * Resolution is expressed in pixels per resolution unit (centimeter if
    * \a metric is true; inch if \a metric is false).
    *
    * Default resolution parameters are applied to all newly created images in
    * the PixInsight core application.
    */
   static void GetDefaultResolution( double& xRes, double& yRes, bool& metric );

   /*!
    * Returns true iff default ICC profile embedding is globally enabled for RGB
    * images in the PixInsight core application.
    *
    * Modules implementing file format support may call this function to
    * provide coherent default states for ICC profile embedding selectable
    * options in their interfaces; e.g. "Embed ICC Profile" check boxes on
    * format options dialogs presented to the user during file write
    * operations. However, the PixInsight core application always provides
    * coherent default states for all embedding flags to all modules, so this
    * function is not frequently used.
    */
   static bool IsDefaultICCProfileEmbeddingEnabledForRGBImages();

   /*!
    * Returns true iff default ICC profile embedding is globally enabled for
    * grayscale images in the PixInsight core application.
    *
    * Modules implementing file format support may call this function to
    * provide coherent default states for ICC profile embedding selectable
    * options in their interfaces; e.g. "Embed ICC Profile" check boxes on
    * format options dialogs presented to the user during file write
    * operations. However, the PixInsight core application always provides
    * coherent default states for all embedding flags to all modules, so this
    * function is not frequently used.
    */
   static bool IsDefaultICCProfileEmbeddingEnabledForGrayscaleImages();

   /*!
    * Returns true iff default thumbnail embedding is globally enabled in the
    * PixInsight core application.
    *
    * Modules implementing file formats should call this function to provide
    * coherent default states for thumbnail embedding selectable options in
    * their interfaces; e.g. "Embed Thumbnail" check boxes on dialogs presented
    * to the user to gather file and format-specific options during file write
    * operations.
    */
   static bool IsDefaultThumbnailEmbeddingEnabled();

   /*!
    * Returns true iff default properties embedding is globally enabled in the
    * PixInsight core application.
    *
    * Modules implementing file formats should call this function to provide
    * coherent default states for properties embedding selectable options in
    * their interfaces; e.g. "Embed Properties" check boxes on dialogs
    * presented to the user to gather file and format-specific options during
    * file write operations.
    */
   static bool IsDefaultPropertiesEmbeddingEnabled();

   /*!
    * Returns the current list of directories for storage of swap image files.
    *
    * Swap files are generated and maintained by the PixInsight core
    * application to store temporary image data. By default, the returned list
    * contains a single string with the system's temporary storage directory
    * (usually the value of the TEMP or TMP environmental variables).
    *
    * When the returned list contains two or more directories, PixInsight uses
    * parallel disk I/O operations to store and retrieve swap files on separate
    * physical devices, which greatly improves performance of history traversal
    * operations (as undo/redo operations).
    */
   static StringList SwapDirectories();

   /*!
    * Attempts to set a new list of directories for storage of swap files.
    *
    * To be able to set a new list of directories for swap file storage, a
    * module must have enough privileges to change global settings on the
    * PixInsight platform. If the calling module has insufficient privileges,
    * this function returns false and no changes are done.
    *
    * The specified list can contain zero or more path specifications to
    * existing directories. If more than one directory is specified, all of
    * them must be supported by different \e physical devices (usually hard
    * disks) on the system. Read the documentation for the
    * ImageVariant::WriteSwapFiles() member function for more information about
    * parallel disk I/O operations and their associated risks.
    *
    * If the specified list is empty, the PixInsight platform will perform a
    * reset of the swap directories list to its default settings. This usually
    * means that all swap files will be generated and maintained on the
    * system's temporary storage directory (the value of the TEMP or TMP
    * environmental variables by default) using single-threaded disk I/O
    * operations.
    */
   static bool SetSwapDirectories( const StringList& directories );

   /*!
    * Returns the current cursor tolerance in viewport coordinates, as
    * globally established in the PixInsight core application.
    *
    * This value is the threshold distance in pixels, in viewport coordinates,
    * used to activate actions based on object proximity.
    *
    * For example, if an interface implements an interactive procedure for
    * resizing a rectangle based on mouse movement events, when the mouse
    * cursor gets closer to a given side of the rectangle by less than the
    * current cursor tolerance, the cursor shape should be changed to give
    * feedback to the user; e.g. to a double arrow shape corresponding to the
    * side being resized.
    */
   static int CursorTolerance();

   /*!
    * Returns the current GUI working mode in the PixInsight core application.
    *
    * The returned value can be identified with the symbolic constants defined
    * in the ImageMode namespace.
    */
   static gui_mode CurrentMode();

   /*!
    * Sets the current GUI working mode in the PixInsight core application.
    *
    * \param mode    Specifies the new GUI mode. Supported modes are identified
    *             through symbolic constants defined in the ImageMode namespace.
    *
    * Not all GUI modes are valid in all contexts. For example, if the current
    * view is a preview, the New Preview mode cannot be selected. If the
    * specified mode is not currently valid, this function is simply ignored.
    */
   static void SelectMode( gui_mode mode );

   /*!
    * Returns the current display channel in the core application's GUI.
    *
    * The returned value can be identified with the symbolic constants defined
    * int the DisplayChannel namespace.
    */
   display_channel CurrentChannel() const;

   /*!
    * Returns true iff the current display channel (in the core application)
    * corresponds to an alpha channel of the image in the current view.
    */
   bool IsAlphaChannelDisplayed() const
   {
      return CurrentChannel() >= DisplayChannel::Alpha;
   }

   /*!
    * Returns the <em>alpha channel index</em> displayed for the image in the
    * current view. If the current display channel does not correspond to an
    * alpha channel, a negative integer value is returned.
    *
    * Alpha channel indices are relative to the first alpha channel in an
    * image. For example, the first alpha channel is always indexed as alpha
    * channel #0, and so on.
    */
   int CurrentAlphaChannel() const
   {
      return CurrentChannel() - DisplayChannel::Alpha;
   }

   /*!
    * Selects the current display channel in the core application's GUI.
    *
    * \param c    Specifies the new display channel. Supported display channels
    *             are identified through symbolic constants defined in the
    *             DisplayChannel namespace.
    *
    * Not all display channels are valid for all images in all contexts. For
    * example, if the image in the current view is a grayscale image, the only
    * valid display channels are the combined RGB/K channel
    * (DisplayChannel::RGBK) and, if the image has \a n alpha channels, values
    * in the range DisplayChannel::Alpha to DisplayChannel::Alpha+n-1. If an
    * invalid display channel is specified, this function is simply ignored.
    */
   void SelectChannel( display_channel c );

   /*!
    * Returns the current mask rendering mode for this image window.
    *
    * Supported mask rendering modes are defined in the MaskMode namespace.
    */
   mask_mode MaskMode() const;

   /*!
    * Sets the current mask rendering mode for this image window.
    *
    * \param mode    Specifies the new mode that will be used to render
    *                masks. Supported mask rendition modes are identified
    *                through symbolic constants defined in the
    *                MaskMode namespace.
    */
   void SetMaskMode( mask_mode mode );

   /*!
    * Returns the current transparency background brush and its foreground and
    * background colors in the core application's GUI.
    *
    * \param[out] fgColor  The foreground color of the background brush.
    * \param[out] bgColor  The background color of the background brush.
    *
    * The returned value can be identified with the symbolic constants defined
    * int the BackgroundBrush namespace.
    *
    * The obtained colors are AARRGGBB values. The alpha values are always
    * 0xff, since the background brush cannot be transparent.
    *
    * In PixInsight, pixel opacity is defined by the first alpha channel of the
    * image: a pixel is opaque if the corresponding alpha channel value is
    * white; a pixel is completely transparent if the corresponding alpha value
    * is black. Intermediate alpha values define semitransparent pixels.
    *
    * The background brush is a two-color brush used to render transparent
    * regions of images. It is a global object used for all images in the
    * PixInsight core application.
    */
   static background_brush GetBackgroundBrush( uint32& fgColor, uint32& bgColor );

   /*!
    * Sets the transparency background brush in the core application's GUI.
    *
    * \param brush   Specifies the new background brush that will be used
    *                to render transparent regions of images. Supported
    *                background brushes are identified through symbolic
    *                constants defined in the BackgroundBrush namespace.
    *
    * \param fgColor New foreground color for the background brush. If zero is
    *                specified (the default value), the current foreground
    *                color will not be changed.
    *
    * \param bgColor New background color for the background brush. If zero is
    *                specified (the default value), the current background
    *                color will not be changed.
    *
    * For a definition of background brushes and their meaning in PixInsight,
    * see the documentation of GetBackgroundBrush().
    *
    * \note    If no foreground and background colors are specified, this
    * function will not change the current background brush colors. This is
    * because the corresponding function arguments default to zero.
    */
   static void SetBackgroundBrush( background_brush brush, uint32 fgColor = 0, uint32 bgColor = 0 );

   /*!
    * Returns the current transparency rendering mode for this image window.
    *
    * Supported transparency modes are defined in the TransparencyMode
    * namespace.
    */
   transparency_mode TransparencyMode() const;

   /*!
    * Returns the color to be used in this image window to render transparent
    * image regions, when the transparency mode is TransparencyMode::Color.
    *
    * The returned color is an AARRGGBB value. The alpha value is always 0xff,
    * since the transparency background cannot be transparent.
    */
   RGBA TransparencyColor() const;

   /*!
    * Returns true iff transparent image areas are currently visible for this
    * image window.
    *
    * Transparencies are visible if the current rendering mode is not
    * TransparencyMode::Hide.
    */
   bool IsTransparencyVisible() const
   {
      return TransparencyMode() != pcl::TransparencyMode::Hide;
   }

   /*!
    * Sets the current transparency rendering mode for this image window.
    *
    * \param mode    Specifies the new mode that will be used to render
    *                transparent regions of images. Supported transparency
    *                modes are identified through symbolic constants defined in
    *                the TransparencyMode namespace.
    *
    * \param color   New color that will be used when the transparency mode is
    *                TransparencyMode::Color. If zero is specified (the default
    *                value), the current color will not be changed.
    */
   void SetTransparencyMode( transparency_mode mode, RGBA color = 0 );

   /*!
    * Disables transparency renditions for this image window.
    *
    * Calling this function is equivalent to:
    *
    * \code SetTransparencyMode( pcl::TransparencyMode::Hide ); \endcode
    */
   void HideTransparency()
   {
      SetTransparencyMode( pcl::TransparencyMode::Hide );
   }

   /*!
    * Sets new viewport central location coordinates and zoom factor for this
    * image window.
    *
    * \param cx,cy   Position (horizontal, vertical) of the new central
    *                viewport location in image coordinates.
    *
    * \param zoom    New viewport zoom factor. Positive zoom factors are
    *                interpreted as magnifying factors; negative zoom factors
    *                are reduction factors:
    *
    * <pre>
    * Zoom Factor    Screen Zoom (screen pixels : image pixels)
    * ===========    ==========================================
    *      0               Current zoom factor (no change)
    *   1 or -1            1:1 (actual pixels)
    *      2               2:1 (x2 magnification)
    *      3               3:1 (x3 magnification)
    *     ...
    *     -2               1:2 (1/2 reduction)
    *     -3               1:3 (1/3 reduction)
    *     ...
    * </pre>
    *
    *                A zoom factor of -1 should not be used explicitly; it is a
    *                reserved value for internal use, and there is no guarantee
    *                that future versions of PCL continue accepting it.
    *
    * If the specified viewport location cannot correspond to the viewport
    * central position with the specified zoom factor, the nearest image
    * coordinates are always selected automatically. For example, if you
    * pass image coordinates \a cx=0 and \a cy=0 as arguments to this function,
    * the GUI will set the viewport center to the necessary image location in
    * order to display the upper left corner of the image just at the upper
    * left corner of the viewport.
    *
    * If the \a zoom argument is not specified, the current zoom factor of the
    * active view is not changed since the default value of this parameter is
    * zero; see the table above.
    *
    * If this image window is visible, calling this function causes an
    * immediate regeneration of the screen rendition for the visible viewport
    * region. This includes regeneration of screen renditions depending on
    * active dynamic interfaces.
    */
   void SetViewport( double cx, double cy, int zoom = 0 );

   /*!
    * Sets new viewport central location and zoom factor for this image
    * window.
    *
    * Calling this function is equivalent to:
    *
    * \code SetViewport( center.x, center.y, int ); \endcode
    *
    * See the documentation for SetViewport( double, double, int ).
    */
   void SetViewport( const DPoint& center, int zoom = 0 )
   {
      SetViewport( center.x, center.y, zoom );
   }

   /*!
    * Adjusts this image window to the minimum size required to show the entire
    * image at the current zoom factor or, if that cannot be achieved, to show
    * the maximum possible portion of the image without exceeding the visible
    * area of its parent workspace in the core application's GUI.
    *
    * This function will move the window on its parent workspace only if it is
    * strictly necessary.
    */
   void FitWindow();

   /*!
    * Selects the largest viewport zoom factor that allows showing the entire
    * image in this image window, changing its size as necessary, but not its
    * position on its parent workspace.
    *
    * If the \a allowMagnification argument is false, this function will not
    * select a zoom factor greater than one, even if doing so would be possible
    * without exceeding the visible area of its parent workspace.
    */
   void ZoomToFit( bool allowMagnification = true );

   /*!
    * Returns the current zoom factor in the viewport of this image window.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   int ZoomFactor() const;

   /*!
    * Sets the zoom factor for the viewport in this image window to the
    * specified value \a z.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   void SetZoomFactor( int z );

   /*!
    * Increments the current zoom factor for the viewport in this image window.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   void ZoomIn()
   {
      int z = ZoomFactor() + 1;
      SetZoomFactor( (z > 0) ? z : +1 );
   }

   /*!
    * Decrements the current zoom factor for the viewport in this image window.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   void ZoomOut()
   {
      int z = ZoomFactor() - 1;
      SetZoomFactor( (z > 0 || z < -1) ? z : -2 );
   }

   /*!
    * Gets the current sizes of the viewport in this image window.
    *
    * \param[out] width    Reference to a variable where the viewport width in
    *             pixels will be stored.
    *
    * \param[out] height   Reference to a variable where the viewport height in
    *             pixels will be stored.
    *
    * The provided sizes represent the entire image in this image window, as
    * represented on its viewport after applying the current zoom factor.
    * Viewport sizes can also be thought of as the available navigation ranges
    * in an image window.
    */
   void GetViewportSize( int& width, int& height ) const;

   /*!
    * Gets the current width of the viewport in this image window.
    */
   int ViewportWidth() const
   {
      int w, dum;
      GetViewportSize( w, dum );
      return w;
   }

   /*!
    * Gets the current height of the viewport in this image window.
    */
   int ViewportHeight() const
   {
      int dum, h;
      GetViewportSize( dum, h );
      return h;
   }

   /*!
    * Returns the current viewport position in this image window.
    *
    * The viewport position corresponds to the viewport coordinates of the
    * upper left corner of the <em>visible viewport region</em>. These
    * coordinates can be negative if the represented image is smaller than the
    * size of the viewport control, that is, when the <em>extended viewport
    * space</em> is visible around the screen rendition of the image.
    */
   Point ViewportPosition() const;

   /*!
    * Sets the viewport position in this image window to the specified \a x
    * and \a y viewport coordinates.
    *
    * If this image window is visible, calling this function causes an
    * immediate regeneration of the screen rendition for the visible viewport
    * region. This includes regeneration of screen renditions depending on
    * active dynamic interfaces.
    */
   void SetViewportPosition( int x, int y );

   /*!
    * Sets the viewport position in this image window to the specified
    * position \a p in viewport coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code SetViewportPosition( p.x, p.y ); \endcode
    */
   void SetViewportPosition( const Point& p )
   {
      SetViewportPosition( p.x, p.y );
   }

   /*!
    * Gets the current visible viewport region in this image window.
    *
    * The returned rectangle corresponds to the visible region of the image in
    * viewport coordinates. It depends on the current viewport zoom factor and
    * on the positions of the viewport's scroll bars, if they are visible.
    */
   Rect VisibleViewportRect() const;

   /*!
    * Returns the width in pixels of the current visible viewport region in
    * this image window.
    */
   int VisibleViewportWidth() const
   {
      return VisibleViewportRect().Width();
   }

   /*!
    * Returns the height in pixels of the current visible viewport region in
    * this image window.
    */
   int VisibleViewportHeight() const
   {
      return VisibleViewportRect().Height();
   }

   /*!
    * Returns true iff this image window is currently visible.
    */
   bool IsVisible() const;

   /*!
    * Shows this image window, if it was previously hidden.
    */
   void Show();

   /*!
    * Hides this image window, if it was previously visible.
    */
   void Hide();

   /*!
    * Returns true iff this image window is iconic.
    */
   bool IsIconic() const;

   /*!
    * Iconizes this image window.
    */
   void Iconize();

   /*!
    * Deiconizes this image window, if it was previously iconized.
    */
   void Deiconize();

   /*!
    * Brings this image window to the top of the stack of image windows in its
    * parent workspace of the PixInsight core application's GUI.
    *
    * Calling this function implicitly shows this image window, if it was
    * previously hidden or iconized. If this window is a child of the current
    * workspace, calling this function also activates it and gives it the
    * keyboard focus.
    */
   void BringToFront();

   /*!
    * Pushes this image window to the bottom of the stack of image windows in
    * its parent workspace of the PixInsight core application's GUI.
    *
    * If this is the only visible image window in its parent workspace, calling
    * this function has no effect. If there are more visible image windows in
    * the parent workspace and it is the current workspace, calling this
    * function implicitly deactivates this image window and gives the keyboard
    * focus to the new topmost image window in the window stack.
    */
   void SendToBack();

   /*!
    * Returns true iff a dynamic session is currently active in the core
    * PixInsight application.
    *
    * A dynamic session is active if there is a processing interface working
    * in dynamic mode, and one or more views have been selected by the active
    * dynamic interface as dynamic targets.
    */
   static bool IsDynamicSessionActive();

   /*!
    * Returns a pointer to the current active dynamic interface.
    *
    * The active dynamic interface is the interface that has been activated in
    * dynamic mode.
    *
    * \warning Note that the returned pointer could point to an object that has
    * not been created by the calling module; this is what we call a
    * <em>foreign object</em> in the PixInsight/PCL framework. If a pointer to
    * a foreign interface is returned by this function, the calling module
    * cannot use that pointer to access the foreign object <em>in any way</em>,
    * since direct intermodule communication is not supported in the current
    * PixInsight/PCL framework.
    */
   static ProcessInterface* ActiveDynamicInterface();

   /*!
    * Terminates the current dynamic session, if there is one active, and
    * optionally closes the active dynamic interface.
    *
    * If the calling module is the owner of the active dynamic interface, the
    * dynamic session is terminated immediately.
    *
    * If the calling module is not the owner of the active dynamic interface
    * (i.e., if the active dynamic interface is a <em>foreign object</em> for
    * the calling module), then the PixInsight core application will ask the
    * user if it is ok to terminate the active dynamic session, providing
    * complete information about the calling module. This is a security
    * enforcement procedure.
    */
   static bool TerminateDynamicSession( bool closeInterface = true );

   /*!
    * Sets the shape of the pointing device's cursor for the active dynamic
    * session as an image in the XPM format.
    *
    * \param xpm  Address of an image in the XPM format, which will be used
    *             as the new cursor shape in Dynamic Operation GUI mode.
    *
    * \param hx,hy   Coordinates (horizontal, vertical) of the cursor hot spot
    *             relative to the upper left corner of the specified cursor
    *             shape image.
    *
    * See the documentation for SetDynamicCursor( const Bitmap&, int, int ) for
    * more information on dynamic mode cursors.
    *
    * \deprecated This function has been deprecated and will be removed in a
    * future version of PCL. Dynamic view cursors must always be specified as
    * Bitmap objects in newly produced code.
    */
   void SetDynamicCursor( const char** xpm, int hx = 0, int hy = 0 );

   /*!
    * Sets the shape of the pointing device's cursor for the active dynamic
    * session as an image in the XPM format.
    *
    * Calling this function is equivalent to:
    *
    * \code SetDynamicCursor( xpm, p.x, p.y ); \endcode
    *
    * \deprecated This function has been deprecated and will be removed in a
    * future version of PCL. Dynamic view cursors must always be specified as
    * Bitmap objects in newly produced code.
    */
   void SetDynamicCursor( const char** xpm, const Point& p )
   {
      SetDynamicCursor( xpm, p.x, p.y );
   }

   /*!
    * Sets the shape of the pointing device's cursor for the active dynamic
    * session as an ARGB bitmap.
    *
    * \param bmp  Reference to a bitmap that will be used as the new cursor
    *             shape for the Dynamic Operation GUI mode.
    *
    * \param hx,hy   Coordinates (horizontal, vertical) of the cursor hot spot
    *             relative to the upper left corner of the specified cursor
    *             shape bitmap.
    *
    * If there is no active dynamic session, this function is ignored.
    *
    * Modules call this function to adapt the cursor shape to specific
    * interactive operations during dynamic sessions. Custom cursor shapes
    * must be used by dynamic interfaces to provide accurate and meaningful
    * feedback to the user.
    *
    * The specified cursor shape will be used in the Dynamic Operation working
    * GUI mode. It will not affect standard cursor shapes used by the GUI in
    * other working modes (as Readout, Zoom In, etc).
    */
   void SetDynamicCursor( const Bitmap& bmp, int hx = 0, int hy = 0 );

   /*!
    * Sets the shape of the pointing device's cursor for the active dynamic
    * session as a RGBA bitmap.
    *
    * Calling this function is equivalent to:
    *
    * \code SetDynamicCursor( bmp, p.x, p.y ); \endcode
    */
   void SetDynamicCursor( const Bitmap& bmp, const Point& p )
   {
      SetDynamicCursor( bmp, p.x, p.y );
   }

   /*!
    * Restores the default shape of the pointing device's cursor for the active
    * dynamic session.
    */
   void ResetDynamicCursor()
   {
      SetDynamicCursor( Bitmap::Null() );
   }

   /*!
    * Returns a RGBA bitmap as a copy of the current shape of the pointing
    * device's cursor for the active dynamic session.
    */
   Bitmap DynamicCursorBitmap() const;

   /*!
    * Returns the cursor hot spot position for the active dynamic session.
    *
    * Cursor hot spot coordinates are relative to the left-top corner of the
    * cursor shape bitmap.
    */
   Point DynamicCursorHotSpot() const;

   /*!
    * Returns the ratio between physical screen pixels and device-independent
    * coordinate units for this image window (actually, for the workspace that
    * this window belongs to).
    *
    * The returned value is greater than or equal to one. Typical pixel ratios
    * are 2.0 for high-dpi displays such as Retina monitors, or 1.0 for normal
    * 96 dpi monitors.
    *
    * The display pixel ratio is never used by ImageWindow, since all viewport
    * coordinates are always specified in physical display pixels. However,
    * knowing this ratio is useful to properly scale interactive graphical
    * contents generated by dynamic processes. For example, drawing vector
    * graphics with one-pixel wide pens is fine for normal monitors, but may
    * cause visibility problems on a 4K monitor, and is clearly inappropriate
    * on a 5K monitor.
    *
    * The following example shows how to scale a Pen's width to adapt a drawing
    * to the physical display where the window is being shown to the user:
    *
    * \code
    * void MyStrokePolygon( VectorGraphics& G,
    *                       const Array<DPoint>& points, RGBA color,
    *                       const ImageWindow& window )
    * {
    *    G.StrokePolygon( points, Pen( color, window.DisplayPixelRatio() ) );
    * }
    * \endcode
    */
   double DisplayPixelRatio() const;

   /*!
    * Converts the specified \a x and \a y integer coordinates from the
    * viewport coordinate system to the image coordinate system.
    *
    * The converted image coordinates are rounded to the nearest integers.
    */
   void ViewportToImage( int& x, int& y ) const;

   /*!
    * Converts the specified \a x and \a y real floating point coordinates from
    * the viewport coordinate system to the image coordinate system.
    */
   void ViewportToImage( double& x, double& y ) const;

   /*!
    * Converts a point \a p in the viewport coordinate system to the image
    * coordinate system. Returns a point with the resulting floating point real
    * image coordinates.
    */
   template <typename T>
   DPoint ViewportToImage( const GenericPoint<T>& p ) const
   {
      DPoint p1 = p;
      ViewportToImage( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the viewport coordinate system to the image
    * coordinate system. Returns a rectangle with the resulting floating point
    * real image coordinates.
    */
   template <typename T>
   DRect ViewportToImage( const GenericRectangle<T>& r ) const
   {
      DRect r1 = r;
      ViewportToImage( r1.x0, r1.y0 );
      ViewportToImage( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Converts a set of integer points from the viewport coordinate system to
    * the image coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from viewport coordinates to image coordinates.
    *
    * \param n    Number of points in the \a p array.
    *
    * Converted point image coordinates are rounded to the nearest integers.
    */
   void ViewportToImage( Point* p, size_type n ) const;

   /*!
    * Converts a set of real floating-point points from the viewport
    * coordinate system to the image coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from viewport coordinates to image coordinates.
    *
    * \param n    Number of points in the \a p array.
    */
   void ViewportToImage( DPoint* p, size_type n ) const;

   /*!
    * Converts a set of points in a dynamic array from the viewport coordinate
    * system to the image coordinate system.
    *
    * \param a    Reference to a dynamic array of points that will be converted
    *             from viewport coordinates to image coordinates.
    */
   template <typename T>
   void ViewportToImage( Array<GenericPoint<T> >& a ) const
   {
      ViewportToImage( a.Begin(), a.Length() );
   }

   /*!
    * Converts a set of integer scalars from the viewport coordinate system to
    * the image coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from viewport coordinates to image
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    *
    * Converted scalars in image coordinates are rounded to the nearest
    * integers.
    */
   void ViewportScalarToImage( int* d, size_type n ) const;

   /*!
    * Converts a set of real floating-point scalars from the viewport
    * coordinate system to the image coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from viewport coordinates to image
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    */
   void ViewportScalarToImage( double* d, size_type n ) const;

   /*!
    * Converts a scalar \a d in the viewport coordinate system to the image
    * coordinate system. Returns the resulting floating point real scalar in
    * image coordinates.
    */
   double ViewportScalarToImage( double d ) const;

   /*!
    * Converts a scalar \a d in the viewport coordinate system to the image
    * coordinate system. Returns the resulting floating point real scalar in
    * image coordinates.
    */
   template <typename T>
   double ViewportScalarToImage( T d ) const
   {
      return ViewportScalarToImage( double( d ) );
   }

   /*!
    * Converts the specified \a x and \a y integer coordinates from the image
    * coordinate system to the viewport coordinate system.
    *
    * The converted viewport coordinates are rounded to the nearest integers.
    */
   void ImageToViewport( int& x, int& y ) const;

   /*!
    * Converts the specified \a x and \a y real floating-point coordinates from
    * the image coordinate system to the viewport coordinate system.
    */
   void ImageToViewport( double& x, double& y ) const;

   /*!
    * Converts a point \a p in the image coordinate system to the viewport
    * coordinate system. Returns a point with the resulting viewport
    * coordinates.
    *
    * If the template argument T corresponds to an integer type, the converted
    * viewport coordinates in the resulting point are rounded to the nearest
    * integers.
    */
   template <typename T>
   GenericPoint<T> ImageToViewport( const GenericPoint<T>& p ) const
   {
      GenericPoint<T> p1 = p;
      ImageToViewport( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the image coordinate system to the viewport
    * coordinate system. Returns a rectangle with the resulting viewport
    * coordinates.
    *
    * If the template argument T corresponds to an integer type, the converted
    * viewport coordinates in the resulting rectangle are rounded to the
    * nearest integers.
    */
   template <typename T>
   GenericRectangle<T> ImageToViewport( const GenericRectangle<T>& r ) const
   {
      GenericRectangle<T> r1 = r;
      ImageToViewport( r1.x0, r1.y0 );
      ImageToViewport( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Converts a set of integer points from the image coordinate system to
    * the viewport coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from image coordinates to viewport coordinates.
    *
    * \param n    Number of points in the \a p array.
    *
    * Converted point viewport coordinates are rounded to the nearest integers.
    */
   void ImageToViewport( Point* p, size_type n ) const;

   /*!
    * Converts a set of real floating-point points from the image coordinate
    * system to the viewport coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from image coordinates to viewport coordinates.
    *
    * \param n    Number of points in the \a p array.
    */
   void ImageToViewport( DPoint* p, size_type n ) const;

   /*!
    * Converts a set of points in a dynamic array from the image coordinate
    * system to the viewport coordinate system.
    *
    * \param a    Reference to a dynamic array of points that will be converted
    *             from image coordinates to viewport coordinates.
    */
   template <typename T>
   void ImageToViewport( Array<GenericPoint<T> >& a ) const
   {
      ImageToViewport( a.Begin(), a.Length() );
   }

   /*!
    * Converts a set of integer scalars from the image coordinate system to
    * the viewport coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from image coordinates to viewport
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    *
    * Converted scalars in viewport coordinates are rounded to the nearest
    * integers.
    */
   void ImageScalarToViewport( int* d, size_type n ) const;

   /*!
    * Converts a set of real floating-point scalars from the image coordinate
    * system to the viewport coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from image coordinates to viewport
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    */
   void ImageScalarToViewport( double* d, size_type n ) const;

   /*!
    * Converts an integer scalar \a d in the image coordinate system to the
    * viewport coordinate system. Returns the resulting scalar in viewport
    * coordinates.
    *
    * The resulting scalar in viewport coordinates is rounded to the nearest
    * integer.
    */
   int ImageScalarToViewport( int ) const;

   /*!
    * Converts a real floating-point scalar \a d in the image coordinate system
    * to the viewport coordinate system. Returns the resulting scalar in
    * viewport coordinates.
    */
   double ImageScalarToViewport( double ) const;

   /*!
    * Converts the specified \a x and \a y integer coordinates from the
    * viewport coordinate system to the global coordinate system.
    *
    * The converted global coordinates are rounded to the nearest integers.
    */
   void ViewportToGlobal( int& x, int& y ) const;

   /*!
    * Converts a point \a p in the viewport coordinate system to the global
    * coordinate system. Returns a point with the resulting global coordinates.
    */
   Point ViewportToGlobal( const Point& p ) const
   {
      Point p1 = p;
      ViewportToGlobal( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the viewport coordinate system to the global
    * coordinate system. Returns a rectangle with the resulting global
    * coordinates.
    */
   Rect ViewportToGlobal( const Rect& r ) const
   {
      Rect r1 = r;
      ViewportToGlobal( r1.x0, r1.y0 );
      ViewportToGlobal( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Converts the specified \a x and \a y integer coordinates from the global
    * coordinate system to the viewport coordinate system.
    *
    * The converted viewport coordinates are rounded to the nearest integers.
    */
   void GlobalToViewport( int& x, int& y ) const;

   /*!
    * Converts a point \a p in the global coordinate system to the viewport
    * coordinate system. Returns a point with the resulting viewport
    * coordinates.
    */
   Point GlobalToViewport( const Point& p ) const
   {
      Point p1 = p;
      GlobalToViewport( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the global coordinate system to the viewport
    * coordinate system. Returns a rectangle with the resulting viewport
    * coordinates.
    */
   Rect GlobalToViewport( const Rect& r ) const
   {
      Rect r1 = r;
      GlobalToViewport( r1.x0, r1.y0 );
      GlobalToViewport( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Causes a complete regeneration of the whole viewport and its associated
    * UI resources.
    *
    * Calling this function may be necessary to force a regeneration after
    * changing the geometry of the image in the current view of this image
    * window.
    */
   void Reset()
   {
      SetZoomFactor( ZoomFactor() );
   }

   /*!
    * Regenerates the screen rendition for the visible viewport region in this
    * image window. This includes regeneration of screen renditions depending
    * on active dynamic interfaces.
    */
   void Regenerate();

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in viewport coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code RegenerateViewportRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void RegenerateViewportRect( const Rect& r )
   {
      RegenerateViewportRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in viewport coordinates.
    *
    * \param x0,y0   %Viewport coordinates (horizontal, vertical) of the upper
    *                left corner of the regenerated region.
    *
    * \param x1,y1   %Viewport coordinates (horizontal, vertical) of the lower
    *                right corner of the regenerated region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be regenerated and updated, that is,
    * its intersection with the visible viewport region.
    */
   void RegenerateViewportRect( int x0, int y0, int x1, int y1 );

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in image coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code RegenerateImageRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void RegenerateImageRect( const pcl::DRect& r )
   {
      RegenerateImageRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in image coordinates.
    *
    * \param x0,y0   %Image coordinates (horizontal, vertical) of the upper
    *                left corner of the regenerated region.
    *
    * \param x1,y1   %Image coordinates (horizontal, vertical) of the lower
    *                right corner of the regenerated region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be regenerated and updated, that is,
    * its intersection with the visible viewport region.
    */
   void RegenerateImageRect( double x0, double y0, double x1, double y1 );

   /*!
    * Requests a screen update for the entire visible viewport region in this
    * image window.
    */
   void UpdateViewport();

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in viewport coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code UpdateViewportRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void UpdateViewportRect( const Rect& r )
   {
      UpdateViewportRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in viewport coordinates.
    *
    * \param x0,y0   %Viewport coordinates (horizontal, vertical) of the upper
    *                left corner of the update region.
    *
    * \param x1,y1   %Viewport coordinates (horizontal, vertical) of the lower
    *                right corner of the update region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be updated, that is, its
    * intersection with the visible viewport region.
    */
   void UpdateViewportRect( int x0, int y0, int x1, int y1 );

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in image coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code UpdateImageRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void UpdateImageRect( const pcl::DRect& r )
   {
      UpdateImageRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in image coordinates.
    *
    * \param x0,y0   %Image coordinates (horizontal, vertical) of the upper
    *                left corner of the update region.
    *
    * \param x1,y1   %Image coordinates (horizontal, vertical) of the lower
    *                right corner of the update region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be updated, that is, its
    * intersection with the visible viewport region in the image coordinate
    * system.
    */
   void UpdateImageRect( double x0, double y0, double x1, double y1 );

   /*!
    * Returns true iff there are pending viewport updates for this image window.
    */
   bool HasPendingUpdates() const;

   /*!
    * Executes all pending viewport update requests for this image window.
    *
    * Viewport update requests are not executed immediately by the core
    * PixInsight application. For the sake of performance, The GUI tries to
    * optimize screen updates by performing the minimum possible redrawing
    * work.
    *
    * This means that update requests are placed on a queue, waiting while the
    * main GUI thread is busy or until a sufficiently large update region
    * becomes available.
    *
    * With this function, an interface can force the immediate screen update
    * for all pending update viewport regions. If multiple update regions are
    * pending, the core application will still minimize the required drawing
    * work by grouping all of them into the minimum possible amount of larger
    * regions.
    */
   void CommitPendingUpdates();

   /*!
    * Returns a rectangle in viewport coordinates that includes all pending
    * viewport update regions for this image window.
    */
   Rect ViewportUpdateRect() const;

   /*!
    * Returns a RGBA bitmap with a screen rendition of a viewport region of
    * this image window.
    *
    * Calling this function is equivalent to:
    *
    * \code ViewportBitmap( r.x0, r.y0, r.x1, r.y1, flags ); \endcode
    */
   Bitmap ViewportBitmap( const Rect& r, uint32 flags = 0 ) const
   {
      return ViewportBitmap( r.x0, r.y0, r.x1, r.y1, flags );
   }

   /*!
    * Returns a RGBA bitmap with a screen rendition of a viewport region of
    * this image window.
    *
    * \param x0,y0   %Viewport coordinates (horizontal, vertical) of the upper
    *                left corner of the source viewport region.
    *
    * \param x1,y1   %Viewport coordinates (horizontal, vertical) of the lower
    *                right corner of the source viewport region.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to zero.
    *
    * The returned bitmap will contain a screen rendition for the specified
    * source region of the image in the current view of this image window. It
    * will be magnified or reduced according to the current zoom factor. It
    * will not contain auxiliary vectorial elements (as preview rectangles), or
    * other renditions of any kind that depend on active dynamic interfaces.
    */
   Bitmap ViewportBitmap( int x0, int y0, int x1, int y1, uint32 flags = 0 ) const;

   /*!
    * Begins a new interactive rectangular selection procedure on the viewport
    * of this image window.
    *
    * Calling this function is equivalent to:
    *
    * \code BeginSelection( p.x, p.y, flags ); \endcode
    */
   void BeginSelection( const Point& p, uint32 flags = 0 )
   {
      BeginSelection( p.x, p.y, flags );
   }

   /*!
    * Starts a new interactive rectangular selection procedure on the viewport
    * of this image window.
    *
    * \param x,y     %Image coordinates of the starting point of a new
    *                rectangular selection.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to zero.
    *
    * Dynamic interfaces use BeginSelection() in response to an OnMousePress
    * event to start a rectangular selection. ModifySelection() is then called
    * from an OnMouseMove event handler, and the final selection coordinates
    * are obtained by calling SelectionRect() from an OnMouseRelease event
    * handler.
    */
   void BeginSelection( int x, int y, uint32 flags = 0 );

   /*!
    * Updates an ongoing interactive rectangular selection procedure on the
    * viewport of this image window.
    *
    * Calling this function is equivalent to:
    *
    * \code ModifySelection( p.x, p.y, flags ); \endcode
    */
   void ModifySelection( const Point& p, uint32 flags = 0 )
   {
      ModifySelection( p.x, p.y, flags );
   }

   /*!
    * Updates an ongoing interactive rectangular selection procedure on the
    * viewport of this image window.
    *
    * \param x,y     %Image coordinates to update the current rectangular
    *                selection. The selection rectangle is defined by this
    *                position and the starting position defined in the call to
    *                BeginSelection() that originated the current selection
    *                procedure.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to zero.
    */
   void ModifySelection( int x, int y, uint32 flags = 0 );

   /*!
    * Requests a screen viewport update for the current rectangular selection.
    */
   void UpdateSelection();

   /*!
    * Aborts the current interactive selection procedure.
    */
   void CancelSelection();

   /*!
    * Terminates the current interactive selection procedure.
    */
   void EndSelection();

   /*!
    * Returns the current rectangular selection in image coordinates.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to nullptr.
    *
    * Before calling this function, a rectangular selection procedure should
    * have been started with BeginSelection().
    */
   Rect SelectionRect( uint32* flags = nullptr ) const;

   /*!
    * Returns true iff an interactive selection procedure is currently active on
    * the viewport of this image window.
    */
   bool IsSelection() const;

   /*!
    * Returns a image window with the specified image identifier. If no image
    * exists with the specified identifier, this function returns
    * ImageWindow::Null().
    */
   static ImageWindow WindowById( const IsoString& id );

   template <class S>
   static ImageWindow WindowById( const S& id )
   {
      return WindowById( IsoString( id ) );
   }

   /*!
    * Returns the image window where the specified file path has been loaded.
    * If no image has been loaded from the specified path, this function
    * returns ImageWindow::Null().
    */
   static ImageWindow WindowByFilePath( const String& filePath );

   /*!
    * Returns the current active image window. Returns ImageWindow::Null() if
    * there are no image windows, if all image windows are hidden, or if all
    * image windows have been iconized.
    *
    * The active image window is the topmost image window of the stack of image
    * windows in the <em>current workspace</em>. Most commands that apply to
    * views, images and previews, are executed on the active image window by
    * default, even if it doesn't have the keyboard focus.
    */
   static ImageWindow ActiveWindow();

   /*!
    * Returns a container with all existing image windows, including visible
    * and hidden windows, and optionally iconized windows.
    *
    * \param includeIconicWindows   If this parameter is false, iconic image
    *             windows will not be included in the returned container.
    */
   static Array<ImageWindow> AllWindows( bool includeIconicWindows = true );

private:

   ImageWindow( void* h ) : UIObject( h )
   {
   }

   friend class View;
   friend class ProcessInstance; // for IsMaskable()
   friend class ProcessContextDispatcher;
   friend class InternalWindowEnumerator;
};

// ----------------------------------------------------------------------------

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_ImageWindow_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageWindow.h - Released 2017-05-28T08:28:50Z
