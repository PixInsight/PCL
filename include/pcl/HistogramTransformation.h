//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/HistogramTransformation.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_HistogramTransformation_h
#define __PCL_HistogramTransformation_h

/// \file pcl/HistogramTransformation.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/ImageTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS Histogram;

/*!
 * \class HistogramTransformation
 * \brief Multiple histogram transformation.
 *
 * ### TODO: Write a detailed description for %HistogramTransformation.
 */
class PCL_CLASS HistogramTransformation : public ImageTransformation
{
public:

   /*!
    * Represents a set of chained histogram transformations.
    */
   typedef Array<HistogramTransformation> transformation_list;

   /*!
    * Constructs a %HistogramTransformation object with the specified
    * parameters.
    *
    * \param mb   Midtones balance in the range [0,1].
    *
    * \param sc   Shadows clipping point in the range [0,1].
    *
    * \param hc   Highlights clipping point in the range [0,1].
    *
    * \param lr   Low dynamic range expansion bound <= 0.
    *
    * \param hr   High dynamic range expansion bound >= 1.
    *
    * The default parameter values define an identity transformation:
    *
    * mb = 0.5, sc = 0, hc = 1, lr = 0, hr = 1.
    */
   HistogramTransformation( double mb = 0.5,
                            double sc = 0, double hc = 1,
                            double lr = 0, double hr = 1 ) :
      ImageTransformation(),
      m_parallel( true ),
      m_maxProcessors( PCL_MAX_PROCESSORS )
   {
      PCL_PRECONDITION( mb >= 0 && mb <= 1 )
      PCL_PRECONDITION( sc >= 0 && sc <= 1 )
      PCL_PRECONDITION( hc >= 0 && hc <= 1 )
      PCL_PRECONDITION( lr <= 0 )
      PCL_PRECONDITION( hr >= 1 )
      m_midtonesBalance = pcl::Range( mb, 0.0, 1.0 );
      m_clipLow = pcl::Range( sc, 0.0, 1.0 );
      m_clipHigh = pcl::Range( hc, 0.0, 1.0 );
      if ( m_clipHigh < m_clipLow )
         pcl::Swap( m_clipLow, m_clipHigh );
      m_expandLow = Min( 0.0, lr );
      m_expandHigh = Max( 1.0, hr );
      UpdateFlags();
   }

   /*!
    * Copy constructor.
    */
   HistogramTransformation( const HistogramTransformation& )  = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   HistogramTransformation( HistogramTransformation&& ) = default;
#endif

   /*!
    * Destroys a %HistogramTransformation object.
    */
   virtual ~HistogramTransformation()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   HistogramTransformation& operator =( const HistogramTransformation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   HistogramTransformation& operator =( HistogramTransformation&& ) = default;
#endif

   /*!
    * Returns the number of histogram transforms in the transformation chain.
    */
   size_type Length() const
   {
      return 1 + m_transformChain.Length();
   }

   /*!
    * Returns a reference to the (immutable) transformation at the specified
    * \a index in the current transformation chain.
    */
   const HistogramTransformation& operator []( size_type index ) const
   {
      return (index == 0) ? *this : m_transformChain[index-1];
   }

   /*!
    * Returns a reference to the (mutable) transformation at the specified
    * \a index in this transformation chain.
    */
   HistogramTransformation& operator []( size_type index )
   {
      return (index == 0) ? *this : m_transformChain[index-1];
   }

   /*!
    * Returns a reference to the (immutable) first histogram transformation in
    * the current chain. The returned value is always a reference to this
    * object.
    */
   const HistogramTransformation& operator *() const
   {
      return *this;
   }

   /*!
    * Returns a reference to the (mutable) first histogram transformation in
    * the current chain. The returned value is always a reference to this
    * object.
    */
   HistogramTransformation& operator *()
   {
      return *this;
   }

   /*!
    * Adds a histogram transformation to the transformation chain of this
    * %HistogramTransformation object.
    */
   void Add( const HistogramTransformation& H )
   {
      m_transformChain.Add( H );
   }

   /*! #
    */
   double MidtonesBalance() const
   {
      return m_midtonesBalance;
   }

   /*! #
    */
   double ShadowsClipping() const
   {
      return m_clipLow;
   }

   /*! #
    */
   double HighlightsClipping() const
   {
      return m_clipHigh;
   }

   /*! #
    */
   double LowRange() const
   {
      return m_expandLow;
   }

   /*! #
    */
   double HighRange() const
   {
      return m_expandHigh;
   }

   /*! #
    */
   bool IsIdentityTransformation() const
   {
      return m_midtonesBalance == 0.5
          && m_clipLow == 0 && m_clipHigh == 1
          && m_expandLow == 0 && m_expandHigh == 1;
   }

   /*! #
    */
   bool IsIdentityTransformationSet() const;

   /*! #
    */
   void SetMidtonesBalance( double b )
   {
      PCL_PRECONDITION( b >= 0 && b <= 1 )
      m_midtonesBalance = pcl::Range( b, 0.0, 1.0 );
      UpdateFlags();
   }

   /*! #
    */
   void SetShadowsClipping( double c )
   {
      PCL_PRECONDITION( c >= 0 && c <= 1 )
      m_clipLow = pcl::Range( c, 0.0, 1.0 );
      if ( m_clipHigh < m_clipLow )
         pcl::Swap( m_clipLow, m_clipHigh );
      UpdateFlags();
   }

   /*! #
    */
   void SetHighlightsClipping( double c )
   {
      PCL_PRECONDITION( c >= 0 && c <= 1 )
      m_clipHigh = pcl::Range( c, 0.0, 1.0 );
      if ( m_clipHigh < m_clipLow )
         pcl::Swap( m_clipLow, m_clipHigh );
      UpdateFlags();
   }

   /*! #
    */
   void SetClipping( double sc, double hc )
   {
      PCL_PRECONDITION( sc >= 0 && sc <= 1 )
      PCL_PRECONDITION( hc >= 0 && hc <= 1 )
      m_clipLow = pcl::Range( sc, 0.0, 1.0 );
      m_clipHigh = pcl::Range( hc, 0.0, 1.0 );
      if ( m_clipHigh < m_clipLow )
         pcl::Swap( m_clipLow, m_clipHigh );
      UpdateFlags();
   }

   /*! #
    */
   void SetLowRange( double r )
   {
      PCL_PRECONDITION( r <= 0 )
      m_expandLow = Min( 0.0, r );
      UpdateFlags();
   }

   /*! #
    */
   void SetHighRange( double r )
   {
      PCL_PRECONDITION( r >= 1 )
      m_expandHigh = Max( 1.0, r );
      UpdateFlags();
   }

   /*! #
    */
   void SetRange( double lr, double hr )
   {
      PCL_PRECONDITION( lr <= 0 )
      PCL_PRECONDITION( hr >= 1 )
      m_expandLow = Min( 0.0, lr );
      m_expandHigh = Max( 1.0, hr );
      UpdateFlags();
   }

   /*!
    * Resets all transformation parameters to yield an identity histogram
    * transformation.
    */
   void Reset()
   {
      m_midtonesBalance = 0.5;
      m_clipLow = 0;
      m_clipHigh = 1;
      m_expandLow = 0;
      m_expandHigh = 1;
      m_transformChain.Clear();
      UpdateFlags();
   }

   // Avoid "hides virtual function in base" warnings by clang
   using ImageTransformation::Apply;

   /*! #
    */
   void Apply( double*, size_type, double x0 = 0, double x1 = 1 ) const;

   /*! #
    */
   void Apply( float*, size_type, float x0 = 0, float x1 = 1 ) const;

   /*! #
    */
   void Apply( Histogram& dstH, const Histogram& srcH ) const;

   /*! #
    */
   void Make8BitLUT( uint8* ) const;

   /*! #
    */
   void Make16BitLUT( uint8* ) const;

   /*! #
    */
   void Make16BitLUT( uint16* ) const;

   /*! #
    */
   void Make20BitLUT( uint8* ) const;

   /*! #
    */
   void Make20BitLUT( uint16* ) const;

   /*! #
    */
   void Make24BitLUT( uint8* ) const;

   /*! #
    */
   void Make24BitLUT( uint16* ) const;

   /*!
    * Returns the value of the <em>midtones transfer function</em> (MTF) for a
    * given midtones balance \a m and a sample point \a x. Both \a m and \a x
    * must be in the range [0,1].
    */
   static double MidtonesTransferFunction( double m, double x )
   {
      /*
       * Bulirsch-Stoer algorithm for a diagonal rational interpolation
       * function with three fixed data points: (0,0) (m,1/2) (1,1).
       *
       * This is the MTF function after direct substitution in the B-S
       * equations:
       *
       *    double r = 1 + 0.5/((x - m)/(x - 1)/2 - 1);
       *    return r + r/(x/(x - 1) * (1 - r/(r - 0.5)) - 1);
       *
       * We can simplify:
       *
       *    double r = (m - 1)/(x + m - 2);
       *
       * and then we can further simplify to:
       *
       *    return (m - 1)*x/((2*m - 1)*x - m);
       *
       * Finally, precalculating (m - 1) we can save a multiplication:
       */
      if ( x > 0 ) // guard us against degenerate cases
      {
         if ( x < 1 )
         {
            double m1 = m - 1;
            return m1*x/((m + m1)*x - m);
         }
         return 1;
      }
      return 0;
   }

   /*!
    * A convenience synonym for MidtonesTransferFunction( m, x ).
    */
   static double MTF( double m, double x )
   {
      return MidtonesTransferFunction( m, x );
   }

   /*!
    * Transforms a real \a value in the normalized [0,1] range.
    */
   void Transform( double& value ) const
   {
      if ( m_flags.hasClipping )
         value = m_flags.hasDelta ? ((value <= m_clipLow) ? 0.0 : ((value >= m_clipHigh) ? 1.0 : (value - m_clipLow)/m_flags.d)) : m_clipLow;
      if ( m_flags.hasMTF )
         value = HistogramTransformation::MTF( m_midtonesBalance, value );
      if ( m_flags.hasRange )
         value = (value - m_expandLow)/m_flags.dr;
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of
    * %HistogramTransformation.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %HistogramTransformation. If \a enable is
    *                false this parameter is ignored. A value <= 0 is ignored.
    *                The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of
    * %HistogramTransformation.
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
    * %HistogramTransformation.
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
    * %HistogramTransformation.
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

private:

   /*!
    * \internal
    * \struct pcl::HistogramTransformation::Flags
    * \brief Characterizes a histogram transformation pertaining to a
    * histogram transformation chain.
    */
   struct Flags
   {
      double d;               //!< Total width of the stretched dynamic range
      double dr;              //!< Total width of the expanded dynamic range
      bool   hasClipping : 1; //!< The transformation defines clipping parameters
      bool   hasMTF      : 1; //!< The transformation defines a midtones transfer function
      bool   hasRange    : 1; //!< The transformation defines dynamic range expansion parameters
      bool   hasDelta    : 1; //!< The transformation defines a stretched range of sample values
   };

   double              m_midtonesBalance;   // midtones balance
   double              m_clipLow;           // shadows clipping point
   double              m_clipHigh;          // highlights clipping point
   double              m_expandLow;         // shadows dynamic range expansion
   double              m_expandHigh;        // highlights dynamic range expansion
   Flags               m_flags;             // transformation flags
   bool                m_parallel      : 1; // use multiple execution threads
   unsigned            m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT; // Maximum number of processors allowed
   transformation_list m_transformChain;    // more transformations

   void UpdateFlags();

   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;

   friend class LUT2408Thread;
   friend class LUT2416Thread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_HistogramTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/HistogramTransformation.h - Released 2017-06-28T11:58:36Z
