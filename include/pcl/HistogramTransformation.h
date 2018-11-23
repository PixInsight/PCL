//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/HistogramTransformation.h - Released 2018-11-23T16:14:19Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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
#include <pcl/ParallelProcess.h>

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
class PCL_CLASS HistogramTransformation : public ImageTransformation, public ParallelProcess
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
    * \param lr   Low dynamic range expansion bound, \a lr &le; 0.
    *
    * \param hr   High dynamic range expansion bound, \a hr &ge; 1.
    *
    * The default parameter values define an identity transformation:
    *
    * \a mb = 0.5, \a sc = 0, \a hc = 1, \a lr = 0, \a hr = 1.
    */
   HistogramTransformation( double mb = 0.5,
                            double sc = 0, double hc = 1,
                            double lr = 0, double hr = 1 )
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
      m_expandLow = Min( lr, 0.0 );
      m_expandHigh = Max( 1.0, hr );
      UpdateFlags();
   }

   /*!
    * Copy constructor.
    */
   HistogramTransformation( const HistogramTransformation& ) = default;

   /*!
    * Move constructor.
    */
   HistogramTransformation( HistogramTransformation&& ) = default;

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
   HistogramTransformation& operator =( HistogramTransformation&& ) = default;

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
       * Finally, precalculating (m - 1) we can save a multiplication.
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
    * \struct pcl::HistogramTransformation::Flags
    * \brief Characterizes a histogram transformation pertaining to a
    * histogram transformation chain.
    */
   struct Flags
   {
      double d = 1.0;             //!< Total width of the stretched dynamic range
      double dr = 1.0;            //!< Total width of the expanded dynamic range
      bool   hasClipping = false; //!< The transformation defines clipping parameters
      bool   hasMTF = false;      //!< The transformation defines a midtones transfer function
      bool   hasRange = false;    //!< The transformation defines dynamic range expansion parameters
      bool   hasDelta = false;    //!< The transformation defines a nonzero stretched range of sample values

      /*!
       * Default constructor.
       */
      Flags() = default;

      /*!
       * Copy constructor.
       */
      Flags( const Flags& ) = default;

      /*!
       * Constructs a new %Flags object initialized for the parameters of the
       * specified HistogramTransformation \a H.
       */
      Flags( const HistogramTransformation& H )
      {
         hasClipping = H.m_clipLow != 0 || H.m_clipHigh != 1;
         hasMTF = H.m_midtonesBalance != 0.5;
         hasRange = H.m_expandLow != 0 || H.m_expandHigh != 1;
         hasDelta = false;
         if ( hasClipping )
         {
            d = H.m_clipHigh - H.m_clipLow;
            hasDelta = 1 + d != 1;
         }
         if ( hasRange )
            dr = H.m_expandHigh - H.m_expandLow;
      }
   };

   /*!
    * Returns the set of flags characterizing this histogram transformation.
    */
   Flags TransformationFlags() const
   {
      return m_flags;
   }

private:

   double              m_midtonesBalance = 0.5; // midtones balance
   double              m_clipLow = 0;           // shadows clipping point
   double              m_clipHigh = 1;          // highlights clipping point
   double              m_expandLow = 0;         // shadows dynamic range expansion
   double              m_expandHigh = 1;        // highlights dynamic range expansion
   Flags               m_flags;                 // transformation flags
   transformation_list m_transformChain;        // more transformations

   /*!
    * \internal
    */
   void UpdateFlags()
   {
      m_flags = Flags( *this );
   }

   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;

   friend class LUT2408Thread;
   friend class LUT2416Thread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_HistogramTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/HistogramTransformation.h - Released 2018-11-23T16:14:19Z
