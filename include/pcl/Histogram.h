// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Histogram.h - Released 2014/11/14 17:16:34 UTC
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

#ifndef __PCL_Histogram_h
#define __PCL_Histogram_h

/// \file pcl/Histogram.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_ImageVariant_h
#include <pcl/ImageVariant.h>
#endif

#ifndef __PCL_Vector_h
#include <pcl/Vector.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Histogram
 * \brief Discrete image histogram function
 *
 * ### TODO: Write a detailed description for %Histogram.
 */
class PCL_CLASS Histogram
{
public:

   /*!
    * Represents a histogram bin count, or the value of the discrete histogram
    * function at a specific abscissa or range of abscissae.
    */
   typedef uint64       count_type;

   /*!
    * A vector of histogram bin counts, which is the type used internally to
    * store the values of the discrete histogram function.
    */
   typedef UI64Vector   histogram_type;

   /*!
    * Constructs an empty %Histogram object with the specified \a resolution.
    *
    * The resolution of a histogram function is the number of discrete
    * intervals (or \e bins) used to analyze image data. The minimum possible
    * resolution is 2. In PCL, the default resolution is 2^16 = 65536 intervals
    * (also known as a <em>16-bit histogram</em>).
    */
   Histogram( int resolution = 0x10000L ) :
      m_histogram(), m_resolution( pcl::Max( 2, resolution ) ), m_peakLevel( 0 ),
      m_rect( 0 ), m_channel( -1 ),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
      PCL_PRECONDITION( resolution > 1 )
   }

   /*!
    *
    */
   Histogram( const histogram_type& data ) :
      m_histogram(), m_resolution( 0x10000L ), m_peakLevel( 0 ),
      m_rect( 0 ), m_channel( -1 ),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
      SetHistogramData( data );
   }

   /*!
    * Copy constructor.
    */
   Histogram( const Histogram& x ) :
      m_histogram( x.m_histogram ), m_resolution( x.m_resolution ), m_peakLevel( x.m_peakLevel ),
      m_rect( x.m_rect ), m_channel( x.m_channel ),
      m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
   }

   /*!
    * Destroys a %Histogram object.
    */
   virtual ~Histogram()
   {
   }

   /*!
    * Assigns another %Histogram object \a x to this object.
    */
   void Assign( const Histogram& x )
   {
      if ( &x != this )
      {
         m_histogram = x.m_histogram;
         m_resolution = x.m_resolution;
         m_peakLevel = x.m_peakLevel;
         m_rect = x.m_rect;
         m_channel = x.m_channel;
         m_parallel = x.m_parallel;
         m_maxProcessors = x.m_maxProcessors;
      }
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   Histogram& operator =( const Histogram& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Deallocates the internal vector of histogram values, yielding an empty
    * %Histogram object.
    */
   void Clear()
   {
      m_histogram.Clear();
      m_peakLevel = 0;
   }

   /*!
    * Returns true if this %Histogram object is empty. An empty histogram has
    * no computed histogram values.
    */
   bool IsEmpty() const
   {
      return m_histogram.IsEmpty();
   }

   /*!
    * Returns the current resolution of this %Histogram object.
    *
    * The resolution of a histogram function is the number of discrete
    * intervals (or \e bins) used to analyze image data. The minimum possible
    * resolution is 2. In PCL, the default resolution is 2^16 = 65536 intervals
    * (also known as a <em>16-bit histogram</em>).
    */
   int Resolution() const
   {
      return m_resolution;
   }

   /*!
    * Returns the highest valid discrete histogram level, or Resolution()-1.
    */
   int LastLevel() const
   {
      return m_resolution-1;
   }

   /*!
    * Sets the resolution of this %Histogram object.
    *
    * The resolution of a histogram function is the number of discrete
    * intervals (or \e bins) used to analyze image data. The minimum possible
    * resolution is 2. In PCL, the default resolution is 2^16 = 65536 intervals
    * (also known as a <em>16-bit histogram</em>).
    *
    * After calling this member function, the histogram will be empty and the
    * internal vector of histogram function values will be deallocated.
    */
   void SetResolution( int resolution )
   {
      PCL_PRECONDITION( resolution > 1 )
      Clear();
      m_resolution = pcl::Max( 2, resolution );
   }

   /*!
    * If this histogram object is empty, this member function allocates the
    * internal vector of histogram function values. Newly allocated vectors are
    * not initialized, so the histogram will contain unpredictable values after
    * calling this function.
    *
    * If the histogram is not empty, calling this function has no effect.
    */
   void Allocate()
   {
      if ( m_histogram.IsEmpty() )
         m_histogram = histogram_type( m_resolution );
   }

   /*!
    * Returns the current peak level of the histogram. If this histogram is
    * empty, the peak level is zero conventionally.
    *
    * The peak level is simply the index (or integer abscissa) of the histogram
    * bin with the largest count, or the position on the X axis of the maximum
    * of the histogram function, in the discrete range [0,LastLevel()].
    */
   int PeakLevel() const
   {
      return m_peakLevel;
   }

   /*!
    * Returns the histogram peak level as a floating point number normalized to
    * the [0,1] range. See PeakLevel().
    */
   double NormalizedPeakLevel() const
   {
      return NormalizedLevel( m_peakLevel );
   }

   /*!
    * Recalculates the histogram peak level from current histogram function
    * values. If the histogram is empty, the peak level is reset to zero
    * conventionally.
    *
    * Returns the newly calculated histogram peak level.
    */
   int UpdatePeakLevel()
   {
      return m_peakLevel = m_histogram.IsEmpty() ? 0 :
                  int( pcl::MaxItem( m_histogram.Begin(), m_histogram.End() ) - m_histogram.Begin() );
   }

   /*!
    * Returns the index of the discrete level in this %Histogram object
    * corresponding to the specified normalized level \a x in the [0,1] range.
    */
   int HistogramLevel( double x ) const
   {
      PCL_PRECONDITION( x >= 0 && x <= 1 )
      return RoundI( pcl::Range( x, 0.0, 1.0 )*(m_resolution - 1) );
   }

   /*!
    * Returns the normalized histogram level in the [0,1] range corresponding
    * to the specified discrete level \a i in this %Histogram object.
    */
   double NormalizedLevel( int i ) const
   {
      PCL_PRECONDITION( i >= 0 && i < m_resolution )
      return double( pcl::Range( i, 0, m_resolution-1 ) )/(m_resolution - 1);
   }

   /*!
    * Converts the specified normalized levels \a a, \a b in the [0,1] range to
    * discrete histogram levels \a i, \a j, respectively.
    */
   void GetHistogramRange( int& i, int& j, double a, double b ) const
   {
      i = HistogramLevel( a );
      j = HistogramLevel( b );
      if ( j < i )
         pcl::Swap( i, j );
   }

   /*!
    * Converts the specified discrete histogram levels \a i, \a j to normalized
    * levels \a a, \a b, respectively, in the [0,1] range.
    */
   void GetNormalizedRange( double& a, double& b, int i, int j ) const
   {
      if ( j < i )
         pcl::Swap( i, j );
      a = NormalizedLevel( i );
      b = NormalizedLevel( j );
   }

   /*!
    * Returns the total sum of the counts in all histogram intervals, or the
    * sum of all discrete histogram function values. If this %Histogram object
    * is empty, this function returns zero.
    */
   count_type Count() const
   {
      return m_histogram.Sum();
   }

   /*!
    * Returns the histogram count, or the value of the histogram function, at
    * the specified discrete level \a i.
    */
   count_type Count( int i ) const
   {
      PCL_PRECONDITION( i >= 0 && i < m_histogram.Length() )
      if ( i < 0 || i >= m_histogram.Length() )
         return 0;
      return m_histogram[i];
   }

   /*!
    * Subscript operator. Equivalent to Count( int ).
    */
   count_type operator []( int i ) const
   {
      return Count( i );
   }

   /*!
    * Returns the sum of counts in the specified interval of discrete histogram
    * levels \a i, \a j.
    */
   count_type Count( int i, int j ) const
   {
      PCL_PRECONDITION( i >= 0 && i < m_histogram.Length() )
      PCL_PRECONDITION( j >= 0 && j < m_histogram.Length() )
      if ( m_histogram.IsEmpty() )
         return 0;
      i = pcl::Range( i, 0, m_histogram.Length()-1 );
      j = pcl::Range( j, 0, m_histogram.Length()-1 );
      if ( j < i )
         pcl::Swap( i, j );
      return pcl::Sum( m_histogram.At( i ), m_histogram.At( j+1 ) );
   }

   /*!
    * Returns the histogram function value for the bin at the current peak
    * level. Typically, the peak count is used to normalize the histogram to
    * a prescribed range. For example, by dividing all histogram function
    * values by the peak count the entire histogram will be normalized to the
    * [0,1] range.
    */
   count_type PeakCount() const
   {
      return Count( m_peakLevel );
   }

   /*!
    * Returns the peak count, or the maximum histogram function value, within
    * the specified range [i,j] of discrete histogram intervals.
    */
   count_type PeakCount( int i, int j ) const
   {
      PCL_PRECONDITION( i >= 0 && i < m_histogram.Length() )
      PCL_PRECONDITION( j >= 0 && j < m_histogram.Length() )
      if ( m_histogram.IsEmpty() )
         return 0;
      i = pcl::Range( i, 0, m_histogram.Length()-1 );
      j = pcl::Range( j, 0, m_histogram.Length()-1 );
      if ( j < i )
         pcl::Swap( i, j );
      return *pcl::MaxItem( m_histogram.At( i ), m_histogram.At( j+1 ) );
   }

   /*!
    * Returns the discrete histogram level where the sum of counts for its
    * preceding levels is greater than or equal to the specified amount \a n.
    *
    * This function is useful to compute an automatic histogram shadows
    * clipping point. The returned index is the position of the shadows
    * clipping point that clips (sets to black) the specified amount of image
    * pixel samples
    */
   int ClipLow( count_type n ) const
   {
      int i = 0;
      for ( count_type s = 0; i < m_histogram.Length()-1 && (s += m_histogram[i]) <= n; ++i ) {}
      return i;
   }

   /*!
    * Returns the normalized histogram level (in the [0,1] range) where the sum
    * of counts for its preceding levels is greater than or equal to the
    * specified amount \a n.
    */
   double NormalizedClipLow( count_type n ) const
   {
      return NormalizedLevel( ClipLow( n ) );
   }

   /*!
    * Returns the discrete histogram level where the sum of counts for its
    * succeding levels is greater than or equal to the specified amount \a n.
    *
    * This function is useful to compute an automatic histogram highlights
    * clipping point. The returned index is the position of the highlights
    * clipping point that clips (sets to white) the specified amount of image
    * pixel samples
    */
   int ClipHigh( count_type n ) const
   {
      int i = m_histogram.Length();
      for ( count_type s = 0; --i > 0 && (s += m_histogram[i]) <= n; ) {}
      return i;
   }

   /*!
    * Returns the normalized histogram level (in the [0,1] range) where the sum
    * of counts for its succeding levels is greater than or equal to the
    * specified amount \a n.
    */
   double NormalizedClipHigh( count_type n ) const
   {
      return NormalizedLevel( ClipHigh( n ) );
   }

   /*!
    * Returns a reference to the immutable internal vector of histogram
    * function values.
    */
   const histogram_type& HistogramData() const
   {
      return m_histogram;
   }

   /*!
    * Causes this %Histogram object to import a copy of the specified \a data
    * vector as its internal vector of histogram function values. Automatically
    * sets the histogram resolution equal to the length of the \a data vector,
    * and recalculates the peak level of the new histogram.
    *
    * If the specified \a data vector has less than two components, this member
    * function will yield an empty histogram without changing the current
    * histogram resolution.
    */
   void SetHistogramData( const histogram_type& data )
   {
      if ( data.Length() < 2 )
         Clear();
      else
      {
         m_histogram = data;
         m_resolution = m_histogram.Length();
         UpdatePeakLevel();
      }
   }

   /*!
    * \deprecated This member function has been deprecated. It is kept as part
    * of PCL for compatibility with existing modules; do not use it in newly
    * produced code.
    */
   void GetData( count_type* where, int fromLevel = 0, int toLevel = -1 ) const
   {
      PCL_PRECONDITION( where != 0 )
      PCL_PRECONDITION( fromLevel >= 0 )
      if ( where != 0 )
         if ( !m_histogram.IsEmpty() )
         {
            fromLevel = pcl::Range( fromLevel, 0, m_histogram.Length()-1 );
            toLevel = (toLevel < 0) ? m_histogram.Length()-1 : pcl::Range( toLevel, 0, m_histogram.Length()-1 );
            if ( toLevel < fromLevel )
               pcl::Swap( fromLevel, toLevel );
            ::memcpy( where, m_histogram.At( fromLevel ), (1+toLevel-fromLevel)*sizeof( count_type ) );
         }
   }

   /*!
    * Remaps the histogram function to fit the resolution of a target
    * histogram \a h.
    *
    * If the target histogram has the same resolution as this object, this
    * member function is equivalent to a plain assignment. If the resolutions
    * differ, if this histogram is empty then the target histogram is
    * deallocated and cleared, otherwise the target histogram is recomputed as
    * a resampled version of this histogram.
    *
    * This member function is useful to generate reduced versions of a
    * histogram. It can also be used to generate bootstrap samples from an
    * existing histogram function.
    */
   void Resample( Histogram& h ) const
   {
      if ( h.m_resolution == m_resolution )
         h = *this;
      else
      {
         if ( m_histogram.IsEmpty() )
            h.Clear();
         else
         {
            h.Allocate();
            h.m_histogram = 0;
            double k = double( h.m_histogram.Length() - 1 )/(m_histogram.Length() - 1);
            for ( int i = 0; i < m_histogram.Length(); ++i )
               h.m_histogram[pcl::RoundInt( i*k )] += m_histogram[i];
            h.m_peakLevel = int( pcl::MaxItem( h.m_histogram.Begin(), h.m_histogram.End() ) - h.m_histogram.Begin() );
         }
      }
   }

   /*!
    * Computes the discrete entropy of this histogram.
    *
    * The returned value is normalized to the total weight of the histogram,
    * that is, it is independent on the total number of counts stored in the
    * histogram data vector.
    */
   double Entropy() const
   {
      double H = 0;
      count_type n = Count();
      if ( n > 0 )
         for ( int i = 0; i < m_histogram.Length(); ++i )
            if ( m_histogram[i] > 0 )
            {
               double f = double( m_histogram[i] )/n;
               H -= f*Log( f );
            }
      return H;
   }

   /*!
    * Computes the histogram function of a 32-bit floating point real image.
    */
   const Image&        operator <<( const Image& );

   /*!
    * Computes the histogram function of a 64-bit floating point real image.
    */
   const DImage&       operator <<( const DImage& );

   /*!
    * Computes the histogram function of an 8-bit unsigned integer image.
    */
   const UInt8Image&   operator <<( const UInt8Image& );

   /*!
    * Computes the histogram function of a 16-bit unsigned integer image.
    */
   const UInt16Image&  operator <<( const UInt16Image& );

   /*!
    * Computes the histogram function of a 32-bit unsigned integer image.
    */
   const UInt32Image&  operator <<( const UInt32Image& );

   /*!
    * Computes the histogram function of the image transported by an
    * ImageVariant object.
    */
   const ImageVariant& operator <<( const ImageVariant& );

   /*!
    * Returns the current rectangular selection in this %Histogram object.
    *
    * By default, the rectangular selection is an empy rectangle, meaning that
    * the rectangular selection in the target image will be used.
    */
   const Rect& SelectedRectangle() const
   {
      return m_rect;
   }

   /*!
    * Sets a new rectangular selection for this %Histogram object.
    *
    * When a non-empty rectangular selection is specified, it is used as the
    * region of interest (ROI) for calculation of histograms. If an empty or
    * invalid rectangle is specified, the current selection in the target image
    * will be used.
    */
   void SelectRectangle( const Rect& r )
   {
      m_rect = r;
   }

   /*!
    * Clears the rectangular selection in this %Histogram object.
    *
    * After calling this function, histograms will be calculated for the
    * current rectangular selection of the target image.
    */
   void ClearRectangle()
   {
      m_rect = Rect( 0 );
   }

   /*!
    * Returns the current channel index selected in this %Histogram object.
    *
    * By default, the selected channel index is -1, meaning that the channel
    * selected in the target image will be used.
    */
   int SelectedChannel() const
   {
      return m_channel;
   }

   /*!
    * Selects a channel index for this %Histogram object.
    *
    * When a positive (>= 0) channel index is specified, it is used for
    * calculation of histograms. If a negative channel index is specified, the
    * current selected channel in the target image will be used.
    */
   void SelectChannel( int channel )
   {
      m_channel = pcl::Max( -1, channel );
   }

   /*!
    * Clears the channel index selection in this %Histogram object.
    *
    * After calling this function, histograms will be calculated for the
    * current selected channel of the target image.
    */
   void ClearSelectedChannel()
   {
      m_channel = -1;
   }

   /*!
    * Returns true if this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of %Histogram.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %Histogram. If \a enable is false
    *                this parameter is ignored. A value <= 0 is ignored. The
    *                default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of %Histogram.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %Histogram.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %Histogram.
    *
    * In the current version of PCL, a module can use a maximum of 1023
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

protected:

   histogram_type m_histogram;         // Discrete histogram levels
   int            m_resolution;        // Number of histogram levels
   int            m_peakLevel;         // Maximum level (index of maximum count)
   Rect           m_rect;              // ROI, Rect( 0 ) to use target image's selection
   int            m_channel;           // < 0 to use target image's selection
   bool           m_parallel      : 1; // Use multiple execution threads
   unsigned       m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT; // Maximum number of processors allowed

   friend class View;
   friend class HistogramTransformation;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Histogram_h

// ****************************************************************************
// EOF pcl/Histogram.h - Released 2014/11/14 17:16:34 UTC
