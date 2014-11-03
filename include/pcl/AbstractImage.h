// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/AbstractImage.h - Released 2014/10/29 07:34:13 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_AbstractImage_h
#define __PCL_AbstractImage_h

/// \file pcl/AbstractImage.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_ImageGeometry_h
#include <pcl/ImageGeometry.h>
#endif

#ifndef __PCL_ImageColor_h
#include <pcl/ImageColor.h>
#endif

#ifndef __PCL_ImageSelections_h
#include <pcl/ImageSelections.h>
#endif

#ifndef __PCL_Array_h
#include <pcl/Array.h>
#endif

#ifndef __PCL_StatusMonitor_h
#include <pcl/StatusMonitor.h>
#endif

#ifndef __PCL_Thread_h
#include <pcl/Thread.h>
#endif

#ifndef __PCL_Mutex_h
#include <pcl/Mutex.h>
#endif

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
class SharedImage;
}
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_width            m_geometry->width
#define m_height           m_geometry->height
#define m_numberOfChannels m_geometry->numberOfChannels

#define m_colorSpace       m_color->colorSpace
#define m_RGBWS            m_color->RGBWS

// ----------------------------------------------------------------------------

/*!
 * \class AbstractImage
 * \brief Base class of all two-dimensional images in PCL.
 *
 * %AbstractImage encapsulates ImageGeometry and ImageColor into a single class
 * employed as the root base class for all two-dimensional images in PCL.
 *
 * This class provides fundamental properties and functionality that are
 * independent on the particular data types used to store and manage pixel
 * samples.
 *
 * %AbstractImage also provides a simple selection mechanism consisting of a
 * rectangular selection (also known as <em>region of interest</em>, or ROI), a
 * channel range, and an anchor point. Image selections can be stored in a
 * local stack for quick retrieval (see PushSelections() and PopSelections()).
 *
 * Note that image selections are implemented as \c mutable data members, so
 * modifying selections is possible for constant %AbstractImage instances.
 *
 * Finally, %AbstractImage provides member functions and properties to manage
 * status monitoring of images. The status monitoring mechanism can be used to
 * provide feedback to the user about the progress of a running process. Status
 * monitoring is implemented through the StatusMonitor and StatusCallback
 * classes. See the Status(), StatusCallback() and SetStatusCallback() member
 * functions for more information.
 *
 * \sa ImageGeometry, ImageColor, GenericImage, ImageVariant
 */
class PCL_CLASS AbstractImage : public ImageGeometry, public ImageColor
{
public:

   /*!
    * A type used to implement a stack of stored image selections.
    */
   typedef Array<ImageSelections>   selection_stack;

   /*!
    * An enumerated type that represents the set of supported color spaces.
    * Valid constants for this enumeration are defined in the ColorSpace
    * namespace.
    */
   typedef ImageColor::color_space  color_space;

   /*!
    * Virtual destructor.
    */
   virtual ~AbstractImage()
   {
   }

   /*!
    * Returns the number of nominal channels corresponding to the current color
    * space of this image.
    */
   int NumberOfNominalChannels() const
   {
      return ColorSpace::NumberOfNominalChannels( m_colorSpace );
   }

   /*!
    * Returns the number of nominal samples in this image. This is equal to the
    * area in square pixels multiplied by the number of nominal channels.
    */
   size_type NumberOfNominalSamples() const
   {
      return NumberOfPixels() * NumberOfNominalChannels();
   }

   /*!
    * Returns true if this image has one or more alpha channels.
    *
    * Alpha channels are those in excess of nominal channels, e.g. a fourth
    * channel in a RGB image, or a second channel in a grayscale image.
    */
   bool HasAlphaChannels() const
   {
      PCL_PRECONDITION( NumberOfChannels() != 0 )
      return NumberOfChannels() > NumberOfNominalChannels();
   }

   /*!
    * Returns the number of existing alpha channels in this image.
    */
   int NumberOfAlphaChannels() const
   {
      PCL_PRECONDITION( NumberOfChannels() != 0 )
      return NumberOfChannels() - NumberOfNominalChannels();
   }

   /*!
    * Returns the number of existing alpha samples in this image. The returned
    * value is equal to the number of alpha channels multiplied by the area of
    * the image in square pixels.
    */
   size_type NumberOfAlphaSamples() const
   {
      return NumberOfPixels() * NumberOfAlphaChannels();
   }

   // -------------------------------------------------------------------------

   /*!
    * Selects a single channel.
    *
    * \param c    Channel index, 0 <= \a c < \a n, where \a n is the total
    *             number of channels in this image, including alpha channels.
    */
   void SelectChannel( int c ) const
   {
      PCL_PRECONDITION( 0 <= c && c < m_numberOfChannels )
      m_selected.channel = m_selected.lastChannel = c;
      ValidateChannelRange();
   }

   /*!
    * Returns the index of the currently selected channel.
    *
    * If the current channel selection includes more than one channel, this
    * function returns the index of the first selected channel.
    *
    * This function is a convenience synonym for FirstSelectedChannel().
    */
   int SelectedChannel() const
   {
      return m_selected.channel;
   }

   /*!
    * Selects a range of channels by their channel indices. The selected range
    * \e includes the two channels specified.
    *
    * \param c0   Index of the first channel to select.
    * \param c1   Index of the last channel to select.
    */
   void SelectChannelRange( int c0, int c1 ) const
   {
      PCL_PRECONDITION( 0 <= c0 && c0 < m_numberOfChannels )
      PCL_PRECONDITION( 0 <= c1 && c1 < m_numberOfChannels )
      m_selected.channel = c0;
      m_selected.lastChannel = c1;
      ValidateChannelRange();
   }

   /*!
    * Sets the current channel range selection to include all nominal channels
    * exclusively, excluding alpha channels.
    */
   void SelectNominalChannels() const
   {
      m_selected.channel = 0;
      m_selected.lastChannel = NumberOfNominalChannels()-1;
      ValidateChannelRange();
   }

   /*!
    * Sets the current channel range selection to include the existing alpha
    * channels only, excluding the nominal channels.
    *
    * \note If this image has no alpha channels, this function selects the last
    * nominal channel. The channel range selection cannot be empty by design.
    */
   void SelectAlphaChannels() const
   {
      m_selected.channel = NumberOfNominalChannels();
      m_selected.lastChannel = m_numberOfChannels-1;
      ValidateChannelRange();
   }

   /*!
    * Resets the channel range selection to include all existing channels (all
    * nominal and alpha channels) in this image.
    */
   void ResetChannelRange() const
   {
      m_selected.channel = 0;
      m_selected.lastChannel = pcl::Max( 0, m_numberOfChannels-1 );
   }

   /*!
    * Returns the number of selected channels.
    */
   int NumberOfSelectedChannels() const
   {
      return 1 + m_selected.lastChannel - m_selected.channel;
   }

   /*!
    * Returns the channel index of the first selected channel.
    */
   int FirstSelectedChannel() const
   {
      return m_selected.channel;
   }

   /*!
    * Returns the channel index of the last selected channel.
    */
   int LastSelectedChannel() const
   {
      return m_selected.lastChannel;
   }

   /*!
    * Copies the first and last channel indices of the current channel
    * selection to the specified variables.
    *
    * \param[out] c0 Index of the first selected channel.
    * \param[out] c1 Index of the last selected channel.
    */
   void GetSelectedChannelRange( int& c0, int& c1 ) const
   {
      c0 = m_selected.channel;
      c1 = m_selected.lastChannel;
   }

   /*!
    * Selects an anchor point by its separate image coordinates.
    *
    * \param x Horizontal coordinate of the new anchor point.
    * \param y Vertical coordinate of the new anchor point.
    */
   void SelectPoint( int x, int y ) const
   {
      m_selected.point.MoveTo( x, y );
   }

   /*!
    * Selects a new anchor point \a p in image coordinates.
    */
   void SelectPoint( const Point& p ) const
   {
      m_selected.point = p;
   }

   /*!
    * Resets the anchor point to the origin of image coordinates, i.e to x=y=0.
    */
   void ResetPoint() const
   {
      m_selected.point = 0;
   }

   /*!
    * Returns the current anchor point.
    */
   const Point& SelectedPoint() const
   {
      return m_selected.point;
   }

   /*!
    * Defines the current rectangular selection by its separate image
    * coordinates.
    *
    * \param x0,y0   Upper left corner coordinates (horizontal, vertical) of
    *                the new rectangular selection.
    *
    * \param x1,y1   Lower right corner coordinates (horizontal, vertical) of
    *                the new rectangular selection.
    *
    * The resulting selection is constrained to stay within the image
    * boundaries.
    */
   void SelectRectangle( int x0, int y0, int x1, int y1 ) const
   {
      m_selected.rectangle.Set( x0, y0, x1, y1 );
      Clip( m_selected.rectangle );
   }

   /*!
    * Defines the current rectangular selection by its separate corner
    * points in image coordinates.
    *
    * \param p0   Position of the upper left corner of the new selection.
    *
    * \param p1   Position of the lower right corner of the new selection.
    */
   void SelectRectangle( const Point& p0, const Point& p1 ) const
   {
      SelectRectangle( p0.x, p0.y, p1.x, p1.y );
   }

   /*!
    * Defines the current rectangular selection as the specified rectangle \a r
    * in image coordinates.
    */
   void SelectRectangle( const Rect& r ) const
   {
      SelectRectangle( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Resets the rectangular selection to include the entire image boundaries.
    */
   void ResetSelection() const
   {
      m_selected.rectangle.Set( 0, 0, m_width, m_height );
   }

   /*!
    * Returns true if the current selection is empty, i.e. if its area is zero.
    */
   bool IsEmptySelection() const
   {
      return m_selected.rectangle.IsPointOrLine();
   }

   /*!
    * Returns true if the current rectangular selection comprises the entire
    * image.
    */
   bool IsFullSelection() const
   {
      return m_selected.rectangle.x0 <= 0 &&
             m_selected.rectangle.y0 <= 0 &&
             m_selected.rectangle.x1 >= m_width &&
             m_selected.rectangle.y1 >= m_height;
   }

   /*!
    * Returns the current rectangular selection.
    */
   const Rect& SelectedRectangle() const
   {
      return m_selected.rectangle;
   }

   /*!
    * Returns true if this image is completely selected; false if it is
    * only partially selected.
    *
    * In a completely selected image, the current rectangular selection
    * includes the whole image, and the current channel range selection
    * comprises all existing channels, including nominal and alpha channels.
    */
   bool IsCompletelySelected() const
   {
      return m_selected.channel == 0 &&
             m_selected.lastChannel >= m_numberOfChannels-1 &&
             m_selected.rectangle.x0 <= 0 &&
             m_selected.rectangle.y0 <= 0 &&
             m_selected.rectangle.x1 >= m_width &&
             m_selected.rectangle.y1 >= m_height;
   }

   /*!
    * Returns the number of selected pixels. This is the area in square pixels
    * of the current selection rectangle.
    */
   size_type NumberOfSelectedPixels() const
   {
      return size_type( m_selected.rectangle.Width() ) * size_type( m_selected.rectangle.Height() );
      // ### NB: Rect::Area() cannot be used here because it performs a
      //         *signed* multiplication of two 32-bit signed integers.
      //return m_selected.rectangle.Area();
   }

   /*!
    * Returns the number of selected samples. This is the area in square pixels
    * of the current selection rectangle multiplied by the number of selected
    * channels.
    */
   size_type NumberOfSelectedSamples() const
   {
      return NumberOfSelectedPixels()*size_type( NumberOfSelectedChannels() );
   }

   /*!
    * Returns true if range clipping is currently enabled for this image.
    *
    * When range clipping is enabled, pixel samples outside the current
    * clipping range:
    *
    * ]RangeClipLow(),RangeClipHigh()[
    *
    * are ignored by statistics calculation routines. The clipping range is
    * always defined in the normalized [0,1] range for all pixel sample data
    * types; the necessary conversions are performed transparently.
    *
    * When range clipping is disabled, the clipping range is ignored and all
    * pixel samples are considered for statistics calculations.
    *
    * The default clipping range is the normalized [0,1] range. Range clipping
    * is disabled by default.
    */
   bool IsRangeClippingEnabled() const
   {
      return m_selected.clipped;
   }

   /*!
    * Enables range clipping for statistical calculations.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void EnableRangeClipping( bool enable = true ) const
   {
      m_selected.clipped = enable;
   }

   /*!
    * Disables range clipping for statistical calculations.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void DisableRangeClipping( bool disable = true ) const
   {
      m_selected.clipped = !disable;
   }

   /*!
    * Returns the lower bound of the current clipping range.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   double RangeClipLow() const
   {
      return m_selected.clipLow;
   }

   /*!
    * Returns the upper bound of the current clipping range.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   double RangeClipHigh() const
   {
      return m_selected.clipHigh;
   }

   /*!
    * Sets the lower bound of the clipping range.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void SetRangeClipLow( double clipLow ) const
   {
      m_selected.clipLow = clipLow;
      if ( m_selected.clipHigh < m_selected.clipLow )
         pcl::Swap( m_selected.clipLow, m_selected.clipHigh );
   }

   /*!
    * Sets the upper bound of the clipping range.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void SetRangeClipHigh( double clipHigh ) const
   {
      m_selected.clipHigh = clipHigh;
      if ( m_selected.clipHigh < m_selected.clipLow )
         pcl::Swap( m_selected.clipLow, m_selected.clipHigh );
   }

   /*!
    * Sets the lower and upper bounds of the clipping range and enables range
    * clipping, in a single function call.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void SetRangeClipping( double clipLow, double clipHigh ) const
   {
      if ( clipHigh < clipLow )
         pcl::Swap( clipLow, clipHigh );
      m_selected.clipLow = clipLow;
      m_selected.clipHigh = clipHigh;
      m_selected.clipped = true;
   }

   /*!
    * Resets the range clipping parameters:
    *
    * Clipping range lower bound = 0.0
    * Clipping range upper bound = 1.0
    * Range clipping disabled
    */
   void ResetRangeClipping() const
   {
      m_selected.clipLow = 0;
      m_selected.clipHigh = 1;
      m_selected.clipped = false;
   }

   /*!
    * Resets all image selections to default values:
    *
    * \li All channels are selected, including nominal and alpha channels.
    * \li The anchor point is located at {0,0}, that is the upper left corner.
    * \li The rectangular selection is set to comprise the entire image.
    * \li The clipping range is set to [0,1].
    * \li Range clipping is disabled.
    *
    * Calling this member function is equivalent to:
    *
    * \code
    * ResetChannelRange();
    * ResetPoint();
    * ResetSelection();
    * ResetRangeClipping();
    * \endcode
    */
   void ResetSelections() const
   {
      ResetChannelRange();
      ResetPoint();
      ResetSelection();
      ResetRangeClipping();
   }

   /*!
    * Returns a reference to the internal ImageSelections object in this image.
    */
   ImageSelections& Selections() const
   {
      return m_selected;
   }

   /*!
    * Saves the current selections (rectangular area, channel range, anchor
    * point and range clipping), pushing them to the internal selection stack.
    */
   void PushSelections() const
   {
      m_savedSelections.Append( m_selected );
   }

   /*!
    * Restores and pops (removes) the current selections (rectangular area,
    * channel range, anchor point and range clipping) from the internal
    * selection stack.
    *
    * If no selections have been previously pushed to the internal selection
    * stack, this function is ignored.
    */
   void PopSelections() const
   {
      if ( CanPopSelections() )
      {
         selection_stack::iterator i = m_savedSelections.ReverseBegin();
         m_selected = *i;
         m_savedSelections.Remove( i );
      }
   }

   /*!
    * Returns true if one or more selections have been pushed to the internal
    * selection stack, that is, if the PopSelections() function can be called
    * to restore them.
    */
   bool CanPopSelections() const
   {
      return !m_savedSelections.IsEmpty();
   }

   // -------------------------------------------------------------------------

   /*!
    * Returns a reference to the status monitoring object associated with this
    * image.
    */
   StatusMonitor& Status() const
   {
      return m_status;
   }

   /*!
    * Returns the address of the status monitoring callback object currently
    * selected for this image.
    */
   pcl::StatusCallback* StatusCallback() const
   {
      return m_status.Callback();
   }

   /*!
    * \deprecated This function has been deprecated. It is included in this
    * version of PCL to keep existing code functional. Use StatusCallback() in
    * newly produced code.
    */
   pcl::StatusCallback* GetStatusCallback() const
   {
      return StatusCallback();
   }

   /*!
    * Specifies the address of an object that will be used to perform status
    * monitoring callbacks for this image.
    */
   void SetStatusCallback( pcl::StatusCallback* callback ) const
   {
      m_status.SetCallback( callback );
   }

   // -------------------------------------------------------------------------

   /*!
    * Returns true if this image is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available) for member
    * functions that support parallel execution.
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this image.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                image. If \a enable is false this parameter is ignored. A
    *                value <= 0 is ignored. The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      if ( (m_parallel = enable) && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this image.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this image.
    *
    * The returned value is the maximum number of processors that this image
    * can use in member functions that support parallel execution.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return int( m_maxProcessors );
   }

   /*!
    * Sets the maximum number of processors allowed for this image.
    *
    * The specified \a maxProcessors parameter is the maximum number of
    * processors that this image can use in member functions that support
    * parallel execution.
    *
    * In the current versions of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

   /*!
    * Returns the maximum number of threads that this image can use
    * concurrently to process a set of items, taking into account if parallel
    * processing is currently enabled for this image, as well as the maximum
    * number of processors allowed.
    *
    * \param count            Number of <em>processing units</em>. A processing
    *             unit can be a single pixel, a row of pixels, or any suitable
    *             item, according to the task being performed by the caller.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *             the maximum number of processors allowed, which takes
    *             precedence over the current limit set for this image (see the
    *             MaxProcessors() and SetMaxProcessors() member functions). If
    *             zero or a negative value is specified, it is ignored and the
    *             current MaxProcessors() limit is applied.
    *
    * \param overheadLimit    Thread overhead limit in processing units. The
    *             function returns a maximum number of threads such that no
    *             thread would have to process less processing units than this
    *             value. The default overhead limit is 16 processing units.
    *
    * Refer the Thread::NumberOfThreads() for detailed and important
    * information on the values returned by this member function.
    */
   int NumberOfThreads( size_type count, int maxProcessors = 0, size_type overheadLimit = 16u ) const
   {
      return m_parallel ? pcl::Min( (maxProcessors > 0) ? maxProcessors : int( m_maxProcessors ),
                                    Thread::NumberOfThreads( count, overheadLimit ) ) : 1;
   }

   /*!
    * Returns the maximum number of threads that this image can use
    * concurrently to process a set of pixel rows, taking into account if
    * parallel processing is currently enabled for this image, as well as the
    * maximum number of processors allowed.
    *
    * \param rowCount         Number of pixel rows to be processed. If zero or
    *             a negative value is specified, the height of the image in
    *             pixels will be used. The default value is zero.
    *
    * \param rowWidth         Width in pixels of the ROI being processed. If
    *             zero or a negative value is specified, the width of the image
    *             in pixels is used. The default value is zero.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *             the maximum number of processors allowed, which takes
    *             precedence over the current limit set for this image (see the
    *             MaxProcessors() and SetMaxProcessors() member functions). If
    *             zero or a negative value is specified, it is ignored and the
    *             current MaxProcessors() limit is applied. The default value
    *             is zero.
    *
    * \param overheadLimitPx  Thread overhead limit in pixels. The function
    *             will calculate the minimum number of pixel rows that a single
    *             thread can process, based on this value and on the specified
    *             \a rowWidth (or the image's width if zero is passed for that
    *             parameter). The default overhead limit is 1024 pixels.
    */
   int NumberOfThreadsForRows( int rowCount = 0, int rowWidth = 0, int maxProcessors = 0, size_type overheadLimitPx = 1024u ) const
   {
      return NumberOfThreads( (rowCount > 0) ? rowCount : Height(),
                              maxProcessors,
                              pcl::Max( size_type( 1 ), size_type( overheadLimitPx/((rowWidth > 0) ? rowWidth : Width()) ) ) );
   }

   // -------------------------------------------------------------------------

   /*!
    * \struct pcl::AbstractImage::ThreadData
    * \brief Thread synchronization data for status monitoring of parallel
    * image processing tasks.
    *
    * The %ThreadData structure provides the required objects to synchronize
    * the status monitoring task for a set of running threads. An instance of
    * %ThreadData (or of a derived class) can be used along with the
    * AbstractImage::RunThreads() function to run a set of threads with
    * synchronized monitoring and task abortion.
    */
   struct ThreadData
   {
              StatusMonitor status; //!< %Status monitoring object.
      mutable Mutex         mutex;  //!< Mutual exclusion for synchronized thread access.
      mutable size_type     count;  //!< current monitoring count.
              size_type     total;  //!< Total monitoring count.

      /*!
       * Constructs a default %ThreadData object.
       */
      ThreadData() : count( 0 ), total( 0 )
      {
      }

      /*!
       * Constructs a %ThreadData object with a copy of the current status
       * monitor of the specified \a image, and the specified total monitoring
       * count \a N.
       *
       * If a zero count \a N is specified, the monitor will be initialized as
       * an \e unbounded monitor. See the StatusMonitor::Initialize() member
       * function for more information.
       */
      ThreadData( const AbstractImage& image, size_type N ) :
      status( image.Status() ), count( 0 ), total( N )
      {
      }

      /*!
       * Constructs a %ThreadData object with a copy of the specified status
       * monitor, and the specified total monitoring count \a N.
       *
       * If a zero count \a N is specified, the monitor will be initialized as
       * an \e unbounded monitor. See the StatusMonitor::Initialize() member
       * function for more information.
       */
      ThreadData( const StatusMonitor& a_status, size_type N ) :
      status( a_status ), count( 0 ), total( N )
      {
      }
   };

   /*!
    * Runs a set of threads with synchronized status monitoring and task
    * abortion.
    *
    * \param threads    Reference to a PArray container of threads. %PArray
    *                   contains pointers to objects and allows direct
    *                   iteration and access by reference. It cannot contain
    *                   invalid pointers (e.g. null pointers). Each %PArray
    *                   element must be an instance of a derived class of
    *                   Thread.
    *
    * \param data       Reference to a ThreadData object for synchronization.
    *
    * \param useAffinity   If (1) this parameter is true, (2) the \a threads
    *                   array contains two or more threads, and (3) this
    *                   function is being called from the root thread (see
    *                   Thread::IsRootThread()), then each thread will be run
    *                   with its affinity set to a single processor (on systems
    *                   that support thread processor affinity). If one or more
    *                   of the three conditions above is false, the thread(s)
    *                   will be run without forcing their processor affinities.
    *
    * This static member function launches the threads in sequence and waits
    * until all threads have finished execution. While the threads are running,
    * the \c status member of ThreadData is incremented to perform the process
    * monitoring task. This also ensures that the graphical interface remains
    * responsive during the whole process.
    *
    * For normal thread execution with maximum performance, the \a useAffinity
    * parameter should be true, in order to minimize cache invalidations due to
    * processor reassignments of running threads. However, there are cases
    * where forcing processor affinities can be counterproductive. An example
    * is real-time previewing of intensive processes requiring continuous GUI
    * updates. In these cases, disabling processor affinity can help to keep
    * the GUI responsive with the required granularity.
    *
    * The threads can be aborted asynchronously with the standard
    * Thread::Abort() mechanism, or through StatusMonitor/StatusCallback. If
    * one or more threads are aborted, this function destroys all the threads
    * by calling PArray::Destroy(), and then throws a ProcessAborted exception.
    * Otherwise, if all threads complete execution normally, the \a threads
    * array is left intact and the function returns. The caller is then
    * responsible for destroying the threads, when appropriate.
    *
    * \warning Do not call this function from a high-priority thread. Doing so
    * can lead to a significant performance loss because this function will
    * consume too much processing time just for process monitoring. In general,
    * you should call this function from normal priority threads.
    */
   template <class thread>
   static void RunThreads( PArray<thread>& threads, ThreadData& data, bool useAffinity = true )
   {
      if ( threads.IsEmpty() )
         return;

      if ( useAffinity )
         if ( threads.Length() == 1 || !Thread::IsRootThread() )
            useAffinity = false;

      for ( typename PArray<thread>::iterator i = threads.Begin(); i != threads.End(); ++i )
         i->Start( ThreadPriority::DefaultMax, useAffinity ? Distance( threads.Begin(), i ) : -1 );

      double waitTime = 0.6*StatusMonitor::RefreshRate();

      for ( size_type lastCount = 0; ; )
      {
         for ( typename PArray<thread>::iterator i = threads.Begin(); ; )
         {
            if ( !i->Wait( waitTime ) )
               break;

            if ( ++i == threads.End() )
            {
               if ( data.total > 0 )
                  data.status += data.total - lastCount;
               return;
            }
         }

         if ( data.mutex.TryLock() )
         {
            try
            {
               if ( data.total > 0 )
               {
                  data.status += data.count - lastCount;
                  lastCount = data.count;
               }
               else
                  ++data.status;

               data.mutex.Unlock();
            }
            catch ( ... )
            {
               data.mutex.Unlock();
               for ( typename PArray<thread>::iterator i = threads.Begin(); i != threads.End(); ++i )
                  i->Abort();
               for ( typename PArray<thread>::iterator i = threads.Begin(); i != threads.End(); ++i )
                  i->Wait();
               threads.Destroy();
               throw ProcessAborted();
            }
         }
      }
   }

protected:

   mutable ImageSelections m_selected;
   mutable selection_stack m_savedSelections;
   mutable StatusMonitor   m_status;
           bool            m_parallel      : 1;
           unsigned        m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT;

   AbstractImage() :
      ImageGeometry(), ImageColor(),
      m_selected(), m_savedSelections(), m_status(),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   AbstractImage( const AbstractImage& x ) :
      ImageGeometry( x ), ImageColor( x ),
      m_selected( x.m_selected ), m_savedSelections( x.m_savedSelections ), m_status( x.m_status ),
      m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
   }

   AbstractImage& operator =( const AbstractImage& x )
   {
      (void)ImageGeometry::operator =( x );
      (void)ImageColor::operator =( x );
      m_selected = x.m_selected;
      m_savedSelections = x.m_savedSelections;
      m_status = x.m_status;
      m_parallel = x.m_parallel;
      m_maxProcessors = x.m_maxProcessors;
      return *this;
   }

   void Swap( AbstractImage& image )
   {
      ImageGeometry::Swap( image );
      ImageColor::Swap( image );
      pcl::Swap( m_selected, image.m_selected );
      pcl::Swap( m_savedSelections, image.m_savedSelections );
      pcl::Swap( m_status, image.m_status );
      bool b = m_parallel; m_parallel = image.m_parallel; image.m_parallel = b;
      unsigned u = m_maxProcessors; m_maxProcessors = image.m_maxProcessors; image.m_maxProcessors = u;
   }

   void ValidateChannelRange() const
   {
      if ( m_numberOfChannels > 0 )
      {
         if ( m_selected.channel < 0 )
            m_selected.channel = 0;
         else if ( m_selected.channel >= m_numberOfChannels )
            m_selected.channel = m_numberOfChannels-1;

         if ( m_selected.lastChannel < 0 )
            m_selected.lastChannel = 0;
         else if ( m_selected.lastChannel >= m_numberOfChannels )
            m_selected.lastChannel = m_numberOfChannels-1;

         if ( m_selected.lastChannel < m_selected.channel )
            pcl::Swap( m_selected.channel, m_selected.lastChannel );
      }
      else
      {
         m_selected.channel = m_selected.lastChannel = 0;
      }
   }

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   friend class pi::SharedImage;
#endif
};

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_numberOfChannels
#undef m_colorSpace
#undef m_RGBWS

// ----------------------------------------------------------------------------

/*!
 * \defgroup thread_monitoring_macros Helper Macros for Synchronized Status
 * Monitoring of Image Processing Threads
 */

/*!
 * \def INIT_THREAD_MONITOR()
 * \brief Declares and initializes local variables used for synchronization of
 * thread status monitoring.
 * \ingroup thread_monitoring_macros
 *
 * This macro is intended to be used at the beginning of a reimplemented
 * Thread::Run() member function. It declares and initializes some counters
 * required to update a status monitoring count with thread synchronization in
 * the UPDATE_THREAD_MONITOR macro.
 *
 * For an example of code using these macros, see UPDATE_THREAD_MONITOR().
 */
#define INIT_THREAD_MONITOR()             \
   size_type ___n___ = 0, ___n1___ = 0;

/*!
 * \def UPDATE_THREAD_MONITOR()
 * \brief Synchronized status monitoring of a set of image processing threads.
 * \ingroup thread_monitoring_macros
 *
 * \param N    Number of accumulated monitoring counts before performing a
 *             a synchronized update of the ThreadData monitoring count. A
 *             larger value will reduce the frequency of monitor updates. A
 *             value of 64K (65536), equivalent to the number of pixels in a
 *             square of 256x256 pixels, is quite appropriate for threads that
 *             process individual pixels.
 *
 * This macro increments a status monitoring count in a ThreadData member of an
 * image processing thread, with thread synchronization. It must be used within
 * a reimplemented Thread::Run() member function.
 *
 * The thread class where this macro is used must have a data member declared
 * as follows:
 *
 * \code AbstractImage::ThreadData& m_data; \endcode
 *
 * where AbstractImage::ThreadData can be replaced with a suitable derived
 * class, e.g. when the thread requires additional data items.
 *
 * The \c m_data member is a reference to a structure providing the necessary
 * objects to perform the synchronized status monitoring task for a set of
 * concurrent threads. For more information on synchronized thread monitoring,
 * see the AbstractImage::RunThreads() member function.
 *
 * To use this macro, the INIT_THREAD_MONITOR macro must also be used at the
 * begining of the reimplemented Thread::Run() function.
 *
 * Here is a brief example pseudocode:
 *
 * \code
 * class FooThreadData : public AbstractImage::ThreadData
 * {
 *    ... constructor and additional data members here
 * };
 *
 * class FooThread : public Thread
 * {
 * public:
 *
 *    ... constructor and other member functions here
 *
 *    virtual void Run()
 *    {
 *       INIT_THREAD_MONITOR()
 *
 *       for ( int i = m_startRow, i < m_endRow; ++i )
 *       {
 *          for ( int j = 0; j < width; ++j )
 *          {
 *             ... process a single pixel here
 *
 *             // Update monitor every 64K processed pixels.
 *             UPDATE_THREAD_MONITOR( 65536 )
 *          }
 *       }
 *    }
 *
 * private:
 *
 *    FooThreadData& m_data;
 *    int            m_startRow;
 *    int            m_endRow;
 *    ... other data members here
 * };
 *
 * ... the code that initializes and runs the threads:
 *
 * FooThreadData data;
 * ... initialize thread data here
 *
 * PArray<FooThread> threads;
 * ... create and construct the threads here
 *
 * AbstractImage::RunThreads( threads, data );
 * threads.Destroy();
 * \endcode
 *
 * For a similar macro that allows incrementing the status monitor by steps
 * larger than one unit, see UPDATE_THREAD_MONITOR_CHUNK().
 */
#define UPDATE_THREAD_MONITOR( N )                    \
   if ( ++___n1___ == (N) )                           \
   {                                                  \
      if ( this->TryIsAborted() )                     \
         return;                                      \
      ___n___ += ___n1___;                            \
      ___n1___ = 0;                                   \
      if ( this->m_data.total > 0 )                   \
         if ( this->m_data.mutex.TryLock() )          \
         {                                            \
            this->m_data.count += ___n___;            \
            this->m_data.mutex.Unlock();              \
            ___n___ = 0;                              \
         }                                            \
   }

/*!
 * \def UPDATE_THREAD_MONITOR_CHUNK()
 * \brief Synchronized status monitoring of a set of image processing threads.
 * \ingroup thread_monitoring_macros
 *
 * This macro is identical to UPDATE_THREAD_MONITOR(), but it updates the
 * status monitoring count by successive steps of the specified \a chunkSize.
 *
 * Typically, this macro is used in situations where the status monitor cannot
 * be incremented by single units, due to the complexity of the monitored code
 * or to excessive overhead of monitoring count. The difference between this
 * macro and %UPDATE_THREAD_MONITOR() can be easily understood with the
 * following two examples. In these examples, we show a reimplemented
 * Thread::Run() member function in an image processing thread:
 *
 * Example of code using %UPDATE_THREAD_MONITOR_CHUNK():
 *
 * \code
 *    virtual void Run()
 *    {
 *       INIT_THREAD_MONITOR()
 *
 *       for ( int i = m_startRow, i < m_endRow; ++i )
 *       {
 *          for ( int j = 0; j < width; ++j )
 *          {
 *             ... process a row of pixels here.
 *          }
 *
 *          // Update every 64K processed pixels, once for each processed row.
 *          UPDATE_THREAD_MONITOR_CHUNK( 65536, width )
 *       }
 *    }
 * \endcode
 *
 * Example of code using %UPDATE_THREAD_MONITOR():
 *
 * \code
 *    virtual void Run()
 *    {
 *       INIT_THREAD_MONITOR()
 *
 *       for ( int i = m_startRow, i < m_endRow; ++i )
 *       {
 *          for ( int j = 0; j < width; ++j )
 *          {
 *             ... process a single pixel here.
 *
 *             // Update monitor every 64K processed pixels.
 *             UPDATE_THREAD_MONITOR( 65536 )
 *          }
 *       }
 *    }
 * \endcode
 */
#define UPDATE_THREAD_MONITOR_CHUNK( N, chunkSize )   \
   if ( (___n1___ += (chunkSize)) == (N) )            \
   {                                                  \
      if ( this->TryIsAborted() )                     \
         return;                                      \
      ___n___ += ___n1___;                            \
      ___n1___ = 0;                                   \
      if ( this->m_data.total > 0 )                   \
         if ( this->m_data.mutex.TryLock() )          \
         {                                            \
            this->m_data.count += ___n___;            \
            this->m_data.mutex.Unlock();              \
            ___n___ = 0;                              \
         }                                            \
   }

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AbstractImage_h

// ****************************************************************************
// EOF pcl/AbstractImage.h - Released 2014/10/29 07:34:13 UTC
