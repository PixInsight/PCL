//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/LinearFilter.h - Released 2015/07/30 17:15:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_LinearFilter_h
#define __PCL_LinearFilter_h

/// \file pcl/LinearFilter.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_KernelFilter_h
#include <pcl/KernelFilter.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class LinearFilter
 * \brief A kernel filter that follows a discrete linear distribution with
 *        radial symmetry in two dimensions.
 *
 * A %LinearFilter object is a KernelFilter whose elements are calculated
 * following a radial linear gradient function controlled by the following
 * parameters:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>\e size</td>         <td>Odd filter size in pixels (size >= 3).</td></tr>
 * <tr><td>\e centralValue</td> <td>Value of the central filter element.</td></tr>
 * <tr><td>\e outerValue</td>   <td>Value of outer (radial) elements.</td></tr>
 * </table>
 *
 * \sa KernelFilter, GaussianFilter, MoffatFilter, VariableShapeFilter
 */
class PCL_CLASS LinearFilter : public KernelFilter
{
public:

   /*!
    * Constructs an empty %LinearFilter object with default functional
    * parameters: size=5, centralValue=1, outerValue=0.01.
    */
   LinearFilter() :
      KernelFilter(), m_centralValue( 1.0f ), m_outerValue( 0.01f )
   {
   }

   /*!
    * Constructs a %LinearFilter object given the odd filter size \a n >= 3,
    * central value \a v0, and outer value \a v1. Assigns an optional \a name
    * to the new filter object.
    */
   LinearFilter( int n, float v0 = 1.0f, float v1 = 0.01f, const String& name = String() ) :
      KernelFilter( n, name )
   {
      Initialize( v0, v1 );
   }

   /*!
    * Copy constructor.
    */
   LinearFilter( const LinearFilter& x ) :
      KernelFilter( x ), m_centralValue( x.m_centralValue ), m_outerValue( x.m_outerValue )
   {
   }

   /*!
    * Move constructor.
    */
   LinearFilter( LinearFilter&& x ) :
      KernelFilter( std::move( x ) ), m_centralValue( x.m_centralValue ), m_outerValue( x.m_outerValue )
   {
   }

   /*!
    */
   virtual KernelFilter* Clone() const
   {
      return new LinearFilter( *this );
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   LinearFilter& operator =( const LinearFilter& x )
   {
      (void)KernelFilter::operator =( x );
      m_centralValue = x.m_centralValue;
      m_outerValue = x.m_outerValue;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   LinearFilter& operator =( LinearFilter&& x )
   {
      (void)KernelFilter::operator =( std::move( x ) );
      m_centralValue = x.m_centralValue;
      m_outerValue = x.m_outerValue;
      return *this;
   }

   /*!
    * Returns the central filter coefficient value.
    */
   float CentralValue() const
   {
      return m_centralValue;
   }

   /*!
    * Returns the outer filter coefficient value.
    */
   float OuterValue() const
   {
      return m_outerValue;
   }

   /*!
    * Recalculates filter coefficients for the specified central and outer
    * coefficient values, \a v0 and \a v1, respectively.
    */
   void Set( float v0, float v1 )
   {
      Initialize( v0, v1 );
   }

   /*!
    * Recalculates filter coefficients for the specified outer coefficient
    * value \a v1. The current central coefficient value is not varied.
    */
   void Set( float v1 )
   {
      Initialize( m_centralValue, v1 );
   }

   /*!
    * Recalculates filter coefficients for the given kernel size \a n >= 3. The
    * current central and outer coefficient values are not varied.
    */
   virtual void Resize( int n )
   {
      KernelFilter::Resize( n );
      Initialize( m_centralValue, m_outerValue );
   }

   /*!
    * Exchanges two linear filters \a x1 and \a x2.
    */
   friend void Swap( LinearFilter& x1, LinearFilter& x2 )
   {
      pcl::Swap( static_cast<KernelFilter&>( x1 ), static_cast<KernelFilter&>( x2 ) );
      pcl::Swap( x1.m_centralValue, x2.m_centralValue );
      pcl::Swap( x1.m_outerValue,   x2.m_outerValue );
   }

private:

   float m_centralValue;
   float m_outerValue;

   void Initialize( float v0, float v1 )
   {
      m_centralValue = v0;
      m_outerValue = v1;
      float* h = *coefficients;
      if ( h != 0 )
      {
         int n2 = Size() >> 1;
         float dvn2 = (v1 - v0)/n2;
         for ( int y = -n2; y <= n2; ++y )
            for ( int x = -n2; x <= n2; ++x )
               *h++ = v0 + Sqrt( float( x*x + y*y ) ) * dvn2;
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LinearFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/LinearFilter.h - Released 2015/07/30 17:15:18 UTC
