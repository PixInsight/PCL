//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/StructuringElement.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_StructuringElement_h
#define __PCL_StructuringElement_h

/// \file pcl/StructuringElement.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/Math.h>
#include <pcl/Rotate.h>     // Reverse()
#include <pcl/StringList.h> // BitmapStructure
#include <pcl/Utility.h>
#include <pcl/Vector.h>

#include <memory.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class StructuringElement
 * \brief Abstract base class of all PCL structuring elements.
 *
 * %StructuringElement defines a square multiway structure that can be used,
 * along with a MorphologicalOperator, to build a MorphologicalTransformation.
 *
 * A structure is a square matrix of Boolean elements. When an element is true
 * we say that that element \e exists. Nonexistent (false) structure elements
 * are ignored when a morphological operator is applied to an image.
 *
 * A <em>structuring element</em> works for morphological transformations in a
 * similar way to a <em>kernel filter</em> for spatial convolutions: the
 * structure is placed centered on a pixel of the image being processed, and
 * only those neighbor pixels falling behind existing structure elements are
 * taken into account to compute the result of a morphological operator. When
 * this operation is repeated for each pixel in an image, it implements a
 * <em>morphological transformation</em>.
 *
 * %StructuringElement holds a <em>multiway structure</em>. Each way is
 * actually a separate structure, so a multiway structure actually groups a
 * set of structures with the same dimensions. When a multiway structure is
 * used to apply a morphological operator, the operator is first applied
 * through each way in the structure, and the result is the same operator
 * applied to the partial results.
 *
 * \sa MorphologicalOperator, MorphologicalTransformation
 */
class PCL_CLASS StructuringElement
{
public:

   /*!
    * Represents an element of a structure existence mask.
    */
   typedef uint32                                  existence_mask_element;

   /*!
    * Represents a structure existence mask.
    */
   typedef GenericVector<existence_mask_element>   existence_mask;

   /*!
    * Represents a set of structure existence masks.
    */
   typedef GenericVector<existence_mask>           existence_mask_set;

   /*!
    * A vector type used to store the number of existing structure elements.
    */
   typedef GenericVector<int>                      existence_mask_count;

   /*!
    * Constructs a default \e box %StructuringElement object of the specified
    * \a size in pixels and number of ways \a n.
    */
   StructuringElement( int size = 3, int n = 1 ) :
      m_size( Max( 3, size|1 ) ), m_mask( Max( 1, n ) ), m_count( 0, Max( 1, n ) ), m_reflected( false ), m_initialized()
   {
      PCL_PRECONDITION( size >= 3 )
      PCL_PRECONDITION( (size & 1) != 0 )
      PCL_PRECONDITION( n >= 1 )
      for ( int k = 0; k < NumberOfWays(); ++k )
         m_mask[k] = existence_mask( existence_mask_element( 0 ), NumberOfElements() );
   }

   /*!
    * Copy constructor.
    */
   StructuringElement( const StructuringElement& x ) :
      m_size( x.m_size ), m_mask( x.NumberOfWays() ), m_count( 0, x.NumberOfWays() ), m_reflected( false ), m_initialized()
   {
      for ( int k = 0; k < NumberOfWays(); ++k )
         m_mask[k] = existence_mask( existence_mask_element( 0 ), NumberOfElements() );
   }

   /*!
    * Destroys a %StructuringElement object.
    */
   virtual ~StructuringElement()
   {
   }

   /*!
    * Returns a pointer to a dynamically allocated duplicate of this
    * structuring element.
    */
   virtual StructuringElement* Clone() const = 0;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   StructuringElement& operator =( const StructuringElement& x )
   {
      if ( &x != this )
      {
         volatile AutoLock lock( m_mutex );
         m_size = x.m_size;
         m_mask = existence_mask_set( x.NumberOfWays() );
         m_count = existence_mask_count( 0, x.NumberOfWays() );
         m_reflected = false;
         m_initialized = 0;
         for ( int k = 0; k < NumberOfWays(); ++k )
            m_mask[k] = existence_mask( existence_mask_element( 0 ), NumberOfElements() );
      }
      return *this;
   }

   /*!
    * Returns the number of ways in this structure.
    */
   int NumberOfWays() const
   {
      return int( m_mask.Length() );
   }

   /*!
    * Returns the size of this structure in pixels.
    */
   int Size() const
   {
      return m_size;
   }

   /*!
    * Returns the total number of structure elements, or Size()*Size(). Note
    * that this includes both existing and nonexistent structure elements.
    */
   int NumberOfElements() const
   {
      return m_size*m_size;
   }

   /*!
    * Returns true iff this is a <em>box structure</em>. All elements in a box
    * structure are <em>existing elements</em>.
    *
    * The default implementation of this member function returns true if all
    * elements in the k-th way are reported as existing elements by the
    * ElementExists() member function.
    *
    * \note This is a performance-critical routine. When possible, derived
    * classes should reimplement it in more efficient ways (for example, a
    * circular structure is never a box structure).
    */
   virtual bool IsBox( int k ) const
   {
      PCL_PRECONDITION( k >= 0 && k < NumberOfWays() )
      Initialize();
      return m_count[k] == NumberOfElements();
   }

   /*!
    * Returns true iff a given element exists in this structure.
    *
    * \param i    column position (X-coordinate) of the requested structure
    *             element. Must be 0 <= i < n, where n is the structure size.
    *
    * \param j    row position (Y-coordinate) of the requested structure
    *             element. Must be 0 <= j < n, where n is the structure size.
    *
    * \param k    Way index. Must be 0 <= k < m, where m is the number of ways
    *             defined in this structure.
    */
   virtual bool ElementExists( int i, int j, int k ) const
   {
      PCL_PRECONDITION( i >= 0 && i < m_size )
      PCL_PRECONDITION( j >= 0 && j < m_size )
      PCL_PRECONDITION( k >= 0 && k < NumberOfWays() )
      return true;
   }

   /*!
    * Gets the subset of pixel sample values that correspond to existing
    * elements in a way of this structure.
    *
    * \param[out] h1    %Vector of samples from the input \a h array that
    *                   correspond to existing structure elements.
    *
    * \param[out] nh1   The number of existing elements in the k-th way of
    *                   this structure. This is also the number of samples
    *                   stored in the \a h1 vector.
    *
    * \param h    Input array of samples. Must be the starting address of a
    *             square matrix of \a n samples stored in row order, where \a n
    *             is the number of elements in this structure (equal to
    *             NumberOfElements()).
    *
    * \param k    Way index. Must be 0 <= k < m, where m is the number of ways
    *             defined in this structure.
    *
    * \note This function is thread-safe.
    */
   template <typename T>
   void PeekElements( T* h1, int& nh1, const T* h, int k ) const
   {
      PCL_PRECONDITION( h1 != 0 )
      PCL_PRECONDITION( h != 0 )
      PCL_PRECONDITION( k >= 0 && k < NumberOfWays() )
      Initialize();
      nh1 = 0;
      for ( existence_mask::const_iterator m = m_mask[k].Begin(), m1 = m_mask[k].End(); m < m1; ++m, ++h )
         if ( *m )
         {
            *h1++ = *h;
            ++nh1;
         }
   }

   /*!
    * Reflects this structure.
    *
    * Structure reflection is equivalent to a 180 degrees rotation of the
    * underlying structure mask for each way. This member function is called
    * internally by MorphologicalTransformation to apply dilation operators.
    */
   void Reflect()
   {
      Initialize();
      for ( existence_mask_set::iterator i = m_mask.Begin(); i != m_mask.End(); ++i )
         pcl::Reverse( i->Begin(), i->End() );
      m_reflected = !m_reflected;
   }

   /*!
    * Returns true iff this structure has been reflected. Note that after an
    * even number of successive reflections (which is a no-op) this member
    * function will return false.
    */
   bool IsReflected() const
   {
      return m_reflected;
   }

   /*!
    * Initializes the internal <em>existence tables</em>.
    *
    * A structure is a square matrix of Boolean elements. When an element is
    * true we say that that element \e exists. Nonexistent (false) structure
    * elements are ignored when a morphological operator is applied to
    * transform an image.
    *
    * StructuringElement (and derived classes) uses a set of precomputed
    * <em>existence tables</em> that greatly improves performance of
    * morphological transformations, especially for complex structures. Calling
    * this member function forces the immediate calculation of existence
    * tables, if they haven't already been calculated previously. If the
    * existence tables already exist, this function does nothing.
    *
    * You normally should not need to call this member function directly, as
    * existence tables are automatically calculated when required.
    *
    * \note This is a thread-safe routine. It can be safely called from
    * multiple running threads.
    */
   void Initialize() const
   {
      if ( !m_initialized )
      {
         volatile AutoLock lock( m_mutex );
         if ( m_initialized.Load() == 0 )
         {
            int N = NumberOfWays();
            for ( int k = 0; k < N; ++k )
            {
               m_count[k] = 0; // redundant, but doesn't hurt
               existence_mask::iterator m = m_mask[k].Begin();
               for ( int i = 0; i < m_size; ++i )
                  for ( int j = 0; j < m_size; ++j, ++m )
                     if ( ElementExists( i, j, k ) )
                     {
                        *m = 1;
                        ++m_count[k];
                     }
                     else
                        *m = 0;
            }

            m_initialized.Store( 1 );
         }
      }
   }

private:

   /*
    * Structure size.
    */
   int                           m_size;

   /*
    * Element existence masks (one mask per way).
    * An element exists if its corresponding mask element is nonzero.
    */
   mutable existence_mask_set    m_mask;

   /*
    * Number of existing elements for each way.
    */
   mutable existence_mask_count  m_count;

   /*
    * Flag true when the structure has been reflected.
    */
   bool                          m_reflected : 1;

   /*
    * Flag true when existence masks have been calculated.
    */
   mutable AtomicInt             m_initialized;

   /*
    * Thread synchronization.
    */
   mutable Mutex                 m_mutex;
};

// ----------------------------------------------------------------------------

/*!
 * \class BoxStructure
 * \brief Standard box (square) structure.
 *
 * A <em>block structure</em> defines all of its elements. For example, a box
 * structure of size 5 is:
 *
 * <pre>
 * x x x x x
 * x x x x x
 * x x x x x
 * x x x x x
 * x x x x x
 * </pre>
 *
 * where existing elements are marked as 'x'.
 *
 * \sa CircularStructure, OrthogonalStructure, DiagonalStructure, StarStructure
 */
class PCL_CLASS BoxStructure : public StructuringElement
{
public:

   /*!
    * Constructs a standard block structure of the specified \a size.
    */
   BoxStructure( int size ) : StructuringElement( size, 1 )
   {
   }

   /*!
    * Copy constructor.
    */
   BoxStructure( const BoxStructure& ) = default;

   /*!
    */
   virtual StructuringElement* Clone() const
   {
      return new BoxStructure( *this );
   }

   /*!
    * Returns true iff this is a <em>box structure</em>. All elements in a box
    * structure are <em>existing elements</em>.
    *
    * As reimplemented by %BoxStructure, this member function always
    * returns true.
    */
   virtual bool IsBox( int k ) const
   {
      PCL_PRECONDITION( k == 0 )
      return true;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class CircularStructure
 * \brief Standard circular structure.
 *
 * The <em>standard circular structure</em> of size 7 is defined as follows:
 *
 * <pre>
 * · · x x x · ·
 * · x x x x x ·
 * x x x x x x x
 * x x x x x x x
 * x x x x x x x
 * · x x x x x ·
 * · · x x x · ·
 * </pre>
 *
 * where existing elements are marked as 'x' and nonexisting elements are
 * denoted as '·'.
 *
 * Circular structures can help in preserving small structures, especially
 * rounded ones, like stars.
 *
 * \sa ThreeWayStructure, OrthogonalStructure, DiagonalStructure, StarStructure
 */
class PCL_CLASS CircularStructure : public StructuringElement
{
public:

   /*!
    * Constructs a standard circular structure of the specified \a diameter.
    */
   CircularStructure( int diameter ) : StructuringElement( diameter, 1 )
   {
   }

   /*!
    * Copy constructor.
    */
   CircularStructure( const CircularStructure& ) = default;

   /*!
    */
   virtual StructuringElement* Clone() const
   {
      return new CircularStructure( *this );
   }

   /*!
    * Returns true iff this is a <em>box structure</em>. All elements in a box
    * structure are <em>existing elements</em>.
    *
    * As reimplemented by %CircularStructure, this member function always
    * returns false.
    */
   virtual bool IsBox( int k ) const
   {
      PCL_PRECONDITION( k == 0 )
      return false;
   }

   /*!
    */
   virtual bool ElementExists( int i, int j, int k ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Size() )
      PCL_PRECONDITION( j >= 0 && j < Size() )
      PCL_PRECONDITION( k == 0 )
      float n2 = 0.5F*Size();
      float di = i+0.5F - n2;
      float dj = j+0.5F - n2;
      return di*di + dj*dj <= n2*n2;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class OrthogonalStructure
 * \brief Standard orthogonal structure.
 *
 * An <em>orthogonal structure</em> of size 5 is defined as follows:
 *
 * <pre>
 * · · x · ·
 * · · x · ·
 * x x x x x
 * · · x · ·
 * · · x · ·
 * </pre>
 *
 * where existing elements are marked as 'x' and nonexisting elements are
 * denoted as '·'.
 *
 * \sa ThreeWayStructure, CircularStructure, DiagonalStructure, StarStructure
 */
class PCL_CLASS OrthogonalStructure : public StructuringElement
{
public:

   /*!
    * Constructs an orthogonal structure of the specified \a size.
    */
   OrthogonalStructure( int size ) : StructuringElement( size, 1 )
   {
   }

   /*!
    * Copy constructor.
    */
   OrthogonalStructure( const OrthogonalStructure& ) = default;

   /*!
    */
   virtual StructuringElement* Clone() const
   {
      return new OrthogonalStructure( *this );
   }

   /*!
    * Returns true iff this is a <em>box structure</em>. All elements in a box
    * structure are <em>existing elements</em>.
    *
    * As reimplemented by %OrthogonalStructure, this member function always
    * returns false.
    */
   virtual bool IsBox( int k ) const
   {
      PCL_PRECONDITION( k == 0 )
      return false;
   }

   /*!
    */
   virtual bool ElementExists( int i, int j, int k ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Size() )
      PCL_PRECONDITION( j >= 0 && j < Size() )
      PCL_PRECONDITION( k == 0 )
      int n2 = Size() >> 1;
      return i == n2 || j == n2;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class DiagonalStructure
 * \brief Standard diagonal structure.
 *
 * A <em>diagonal structure</em> of size 5 is defined as follows:
 *
 * <pre>
 * x · · · x
 * · x · x ·
 * · · x · ·
 * · x · x ·
 * x · · · x
 * </pre>
 *
 * where existing elements are marked as 'x' and nonexisting elements are
 * denoted as '·'.
 *
 * \sa ThreeWayStructure, CircularStructure, OrthogonalStructure, StarStructure
 */
class PCL_CLASS DiagonalStructure : public StructuringElement
{
public:

   /*!
    * Constructs a diagonal structure of the specified \a size.
    */
   DiagonalStructure( int size ) : StructuringElement( size, 1 )
   {
   }

   /*!
    * Copy constructor.
    */
   DiagonalStructure( const DiagonalStructure& ) = default;

   /*!
    */
   virtual StructuringElement* Clone() const
   {
      return new DiagonalStructure( *this );
   }

   /*!
    * Returns true iff this is a <em>box structure</em>. All elements in a box
    * structure are <em>existing elements</em>.
    *
    * As reimplemented by %DiagonalStructure, this member function always
    * returns false.
    */
   virtual bool IsBox( int k ) const
   {
      PCL_PRECONDITION( k == 0 )
      return false;
   }

   /*!
    */
   virtual bool ElementExists( int i, int j, int k ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Size() )
      PCL_PRECONDITION( j >= 0 && j < Size() )
      PCL_PRECONDITION( k == 0 )
      return j == i || j == Size()-i-1;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class StarStructure
 * \brief Standard star structure.
 *
 * A <em>star structure</em> of size 7 is defined as follows:
 *
 * <pre>
 * · · · x · · ·
 * · x · x · x ·
 * · · x x x · ·
 * x x x x x x x
 * · · x x x · ·
 * · x · x · x ·
 * · · · x · · ·
 * </pre>
 *
 * where existing elements are marked as 'x' and nonexisting elements are
 * denoted as '·'.
 *
 * \sa ThreeWayStructure, CircularStructure, OrthogonalStructure,
 * DiagonalStructure
 */
class PCL_CLASS StarStructure : public StructuringElement
{
public:

   /*!
    * Constructs a star structure of the specified \a size.
    */
   StarStructure( int size ) : StructuringElement( size, 1 )
   {
   }

   /*!
    * Copy constructor.
    */
   StarStructure( const StarStructure& ) = default;

   /*!
    */
   virtual StructuringElement* Clone() const
   {
      return new StarStructure( *this );
   }

   /*!
    * Returns true iff this is a <em>box structure</em>. All elements in a box
    * structure are <em>existing elements</em>.
    *
    * As reimplemented by %StarStructure, this member function always
    * returns false.
    */
   virtual bool IsBox( int k ) const
   {
      PCL_PRECONDITION( k == 0 )
      return false;
   }

   /*!
    */
   virtual bool ElementExists( int i, int j, int k ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Size() )
      PCL_PRECONDITION( j >= 0 && j < Size() )
      PCL_PRECONDITION( k == 0 )
      int n2 = Size() >> 1;
      if ( i == n2 || j == n2 )
         return true;
      if ( j == i || j == Size()-i-1 )
      {
         float n2 = 0.5F*Size();
         float di = i+0.5F - n2;
         float dj = j+0.5F - n2;
         return di*di + dj*dj <= n2*n2;
      }
      return false;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class ThreeWayStructure
 * \brief Standard three-way structure.
 *
 * The <em>standard three-way structure</em> of size 5 is defined as follows:
 *
 * <pre>
 *   Way #0       Way #1       Way #2
 *
 * · · x · ·    x · · · x    · · · · ·
 * · · x · ·    · x · x ·    · · · · ·
 * x x · x x    · · · · ·    · · x · ·
 * · · x · ·    · x · x ·    · · · · ·
 * · · x · ·    x · · · x    · · · · ·
 * </pre>
 *
 * where existing elements are marked as 'x' and nonexisting elements are
 * denoted as '·'.
 *
 * The standard three-way structure leads to ranking operations where data
 * from different spatial directions are ranked separately. When used to apply
 * morphological operators, standard three-way structures can preserve edges
 * better than box structures, especially when applied recursively.
 *
 * \sa CircularStructure, OrthogonalStructure, DiagonalStructure, StarStructure
 */
class PCL_CLASS ThreeWayStructure : public StructuringElement
{
public:

   /*!
    * Constructs a standard three-way structure of the specified \a size.
    */
   ThreeWayStructure( int size ) : StructuringElement( size, 3 )
   {
   }

   /*!
    * Copy constructor.
    */
   ThreeWayStructure( const ThreeWayStructure& ) = default;

   /*!
    */
   virtual StructuringElement* Clone() const
   {
      return new ThreeWayStructure( *this );
   }

   /*!
    * Returns true iff this is a <em>box structure</em>. All elements in a box
    * structure are <em>existing elements</em>.
    *
    * As reimplemented by %ThreeWayStructure, this member function always
    * returns false.
    */
   virtual bool IsBox( int k ) const
   {
      PCL_PRECONDITION( k >= 0 && k < 3 )
      return false;
   }

   /*!
    */
   virtual bool ElementExists( int i, int j, int k ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Size() )
      PCL_PRECONDITION( j >= 0 && j < Size() )
      PCL_PRECONDITION( k >= 0 && k < 3 )
      int n2 = Size() >> 1;
      switch ( k )
      {
      default:
      case 0:  // central element
         return i == n2 && j == n2;
      case 1:  // horizontal and vertical elements
         return i == n2 || j == n2;
      case 2:  // diagonal elements
         return (j == i || j == Size()-i-1) && i != n2;
      }
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class BitmapStructure
 * \brief A structuring element where static strings are used to define custom
 * existence matrices.
 *
 * %BitmapStructure allows you to build a structuring element using character
 * strings to define the existence of structure elements. Each string defines a
 * structure way, where 'x' characters correspond to existing structure
 * elements. For example, the following array of static strings:
 *
 * <pre>
 * const char* B[] = { "--x--"
 *                     "-xxx-"
 *                     "xxxxx"
 *                     "-xxx-"
 *                     "--x--",
 *                     //
 *                     "-xxx-"
 *                     "xxxxx"
 *                     "xxxxx"
 *                     "xxxxx"
 *                     "-xxx-" };
 * </pre>
 *
 * would define a 5x5 structure with two ways. 'x' characters define existing
 * elements; other character values ('-' in this case) are interpreted as
 * nonexistent elements. The corresponding %BitmapStructure object would be
 * constructed as follows:
 *
 * <tt>BitmapStructure S( B, 5, 2 );</tt>
 *
 * This class is a convenient, straightforward approach to defining multi-way
 * custom structuring elements that can be used directly with the
 * MorphologicalTransformation class.
 */
class PCL_CLASS BitmapStructure : public StructuringElement
{
public:

   /*!
    * Represents a structure bitmap. Each bitmap defines the existence matrix
    * for a \e way of the structuring element.
    */
   typedef IsoString       bitmap;

   /*!
    * Represents a set of bitmaps used to define the structure ways in a
    * %BitmapStructure object.
    */
   typedef IsoStringList   bitmap_set;

   /*!
    * Constructs a %BitmapStructure object of the specified \a size and \a n
    * number of ways.
    *
    * \param bitmaps    An array of null-terminated strings representing the
    *                   set of ways in this structuring element. There must be
    *                   \a n contiguous null-terminated strings of length
    *                   \a size*size characters at the address specified by
    *                   this argument. In these strings, 'x' characters
    *                   correspond to existing structure elements; other
    *                   character values will be interpreted as nonexistent
    *                   structure elements.
    *
    * \param size       Size of this structure. Must be an odd integer >= 3.
    *
    * \param n          Number of ways in this structure. Must be >= 1.
    */
   BitmapStructure( const char** bitmaps, int size, int n = 1 ) :
      StructuringElement( size, n ), m_bitmaps()
   {
      PCL_PRECONDITION( bitmaps != nullptr )
      PCL_PRECONDITION( *bitmaps != '\0' )
      for ( int i = 0; i < NumberOfWays(); ++i )
         m_bitmaps.Add( bitmap( bitmaps[i] ) );
   }

   /*!
    * Copy constructor.
    */
   BitmapStructure( const BitmapStructure& ) = default;

   /*!
    */
   virtual StructuringElement* Clone() const
   {
      return new BitmapStructure( *this );
   }

   /*!
    */
   virtual bool ElementExists( int i, int j, int k ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Size() )
      PCL_PRECONDITION( j >= 0 && j < Size() )
      PCL_PRECONDITION( k >= 0 && k < NumberOfWays() )
      return m_bitmaps[k][i + j*Size()] == 'x';
   }

protected:

   bitmap_set m_bitmaps;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_StructuringElement_h

// ----------------------------------------------------------------------------
// EOF pcl/StructuringElement.h - Released 2017-07-09T18:07:07Z
