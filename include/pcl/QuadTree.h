//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/QuadTree.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_QuadTree_h
#define __PCL_QuadTree_h

/// \file pcl/QuadTree.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/Rectangle.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class QuadTree
 * \brief Bucket PR quadtree for two-dimensional point data.
 *
 * A quadtree is a specialized binary search tree for partitioning of a set of
 * points in two dimensions. Quadtrees have important applications in
 * computational geometry problems requiring efficient rectangular range
 * searching and nearest neighbor queries.
 *
 * This class implements a <em>bucket point region quadtree</em> structure
 * (see Reference 2).
 *
 * The template type argument T represents the type of a \e point object stored
 * in a %QuadTree structure. The type T must have the following properties:
 *
 * \li The standard default and copy constructors are required:\n
 * \n
 * T::T() \n
 * T::T( const T& )
 *
 * \li The \c T::component subtype must be defined. It represents a component
 * of an object of type T. For example, if T is a vector type, T::component
 * must be the type of a vector component.
 *
 * \li The array subscript operator must be defined as follows:\n
 * \n
 * component T::operator []( int i ) const \n
 * \n
 * This operator must return the value of the first or second component of an
 * object being stored in the quadtree. The subindex i will be either 0 or 1
 * for the first or second point component, respectively.
 *
 * \b References
 *
 * \li 1. Mark de Berg et al, <em>Computational Geometry: Algorithms and
 * Applications Third Edition,</em> Springer, 2010, Chapter 14.
 *
 * \li 2. Hanan Samet, <em>Foundations of Multidimensional and Metric Data
 * Structures,</em> Morgan Kaufmann, 2006, Section 1.4.
 *
 * \sa KDTree
 */
template <class T>
class QuadTree
{
public:

   /*!
    * Represents a two-dimensional point stored in this quadtree.
    */
   typedef T                           point;

   /*!
    * Represents a point component.
    */
   typedef typename point::component   component;

   /*!
    * A list of points. Used for tree build and search operations.
    */
   typedef Array<point>                point_list;

   /*!
    * A rectangular region. Used for rectangular range search operations.
    */
   typedef DRect                       rectangle;

   /*!
    * Constructs an empty quadtree.
    */
   QuadTree() :
      m_root( nullptr ), m_bucketCapacity( 0 ), m_length( 0 )
   {
   }

   /*!
    * Constructs a quadtree and builds it for the specified list of \a points.
    *
    * \param points           A list of points that will be stored in this
    *                         quadtree.
    *
    * \param bucketCapacity   The maximum number of points in a leaf tree node.
    *                         Must be >= 1. The default value is 40.
    *
    * If the specified list of \a points is empty, this constructor yields an
    * empty quadtree.
    */
   QuadTree( const point_list& points, int bucketCapacity = 40 ) :
      m_root( nullptr ), m_bucketCapacity( 0 ), m_length( 0 )
   {
      Build( points, bucketCapacity );
   }

   /*!
    * Move constructor.
    */
   QuadTree( QuadTree&& x ) :
      m_root( x.m_root ), m_bucketCapacity( x.m_bucketCapacity ), m_length( x.m_length )
   {
      x.m_root = nullptr;
      x.m_length = 0;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   QuadTree& operator =( QuadTree&& x )
   {
      DestroyTree( m_root );
      m_root = x.m_root;
      m_bucketCapacity = x.m_bucketCapacity;
      m_length = x.m_length;
      x.m_root = nullptr;
      x.m_length = 0;
      return *this;
   }

   /*!
    * Destroys a quadtree. All the stored point objects are deleted.
    */
   ~QuadTree()
   {
      Clear();
   }

   /*!
    * Removes all the stored point objects, yielding an empty quadtree.
    */
   void Clear()
   {
      DestroyTree( m_root );
      m_root = nullptr;
      m_length = 0;
   }

   /*!
    * Builds a new quadtree for the specified list of \a points.
    *
    * \param points           A list of points that will be stored in this
    *                         quadtree.
    *
    * \param bucketCapacity   The maximum number of points in a leaf tree node.
    *                         Must be >= 1. The default value is 40.
    *
    * If the tree stores point objects before calling this function, they are
    * destroyed and removed before building a new tree.
    *
    * If the specified list of \a points is empty, this member function yields
    * an empty quadtree.
    */
   void Build( const point_list& points, int bucketCapacity = 40 )
   {
      Clear();
      m_bucketCapacity = Max( 1, bucketCapacity );

      if ( !points.IsEmpty() )
      {
         component x = points[0][0];
         component y = points[0][1];
         rectangle rect( x, y, x, y );
         for ( typename point_list::const_iterator i = points.Begin(); ++i != points.End(); )
         {
            x = (*i)[0];
            y = (*i)[1];
            if ( x < rect.x0 )
               rect.x0 = x;
            if ( y < rect.y0 )
               rect.y0 = y;
            if ( x > rect.x1 )
               rect.x1 = x;
            if ( y > rect.y1 )
               rect.y1 = y;
         }
         m_root = BuildTree( rect, points );
      }
   }

   /*!
    * Performs a rectangular range search in this quadtree.
    *
    * \param rect    The rectangular search region.
    *
    * Returns a (possibly empty) list with all the points found in this tree
    * within the specified search range.
    */
   point_list Search( const rectangle& rect ) const
   {
      point_list found;
      SearchTree( found, rect, m_root );
      return found;
   }

   /*!
    * Performs a rectangular range search in this quadtree.
    *
    * \param rect       The rectangular search region.
    *
    * \param callback   Callback functional.
    *
    * \param data       Callback data.
    *
    * The callback function prototype should be:
    *
    * \code void callback( const point& pt, void* data ) \endcode
    *
    * The callback function will be called once for each point found in the
    * tree within the specified search range.
    */
   template <class F>
   void Search( const rectangle& rect, F callback, void* data ) const
   {
      SearchTree( rect, callback, data, m_root );
   }

   /*!
    * Inserts a new point in this quadtree.
    */
   void Insert( const point& pt )
   {
      InsertTree( pt, m_root );
   }

   /*!
    * Deletes all points in this quadtree equal to the specified point.
    */
   void Delete( const point& pt )
   {
      DeleteTree( pt, m_root );
   }

   /*!
    * Deletes all points in this quadtree included in the specified rectangular
    * region \a rect.
    */
   void Delete( const rectangle& rect )
   {
      DeleteTree( rect, m_root );
   }

   /*!
    * Returns the bucket capacity of this quadtree, or the maximum number of
    * points that can be stored in a leaf tree node. This parameter is
    * specified when a new tree is built.
    */
   int BucketCapacity() const
   {
      return m_bucketCapacity;
   }

   /*!
    * Returns the total number of points stored in this quadtree.
    */
   size_type Length() const
   {
      return m_length;
   }

   /*!
    * Returns true iff this quadtree is empty.
    */
   bool IsEmpty()
   {
      return m_root == nullptr;
   }

   /*!
    * Exchanges two %QuadTree objects \a x1 and \a x2.
    */
   friend void Swap( QuadTree& x1, QuadTree& x2 )
   {
      pcl::Swap( x1.m_root,           x2.m_root );
      pcl::Swap( x1.m_bucketCapacity, x2.m_bucketCapacity );
      pcl::Swap( x1.m_length,         x2.m_length );
   }

private:

   struct Node
   {
      rectangle rect; // rectangular region
      Node*     nw;   // north-west
      Node*     ne;   // north-east
      Node*     sw;   // south-west
      Node*     se;   // south-east

      Node( const rectangle& r = rectangle( 0.0 ) ) :
         rect( r ), nw( nullptr ), ne( nullptr ), sw( nullptr ), se( nullptr )
      {
      }

      bool IsLeaf() const
      {
         return nw == nullptr && ne == nullptr && sw == nullptr && se == nullptr;
      }

      bool Intersects( const rectangle& r ) const
      {
         return rect.x1 >= r.x0 && rect.x0 <= r.x1 &&
                rect.y1 >= r.y0 && rect.y0 <= r.y1;
      }

      bool Includes( const point& p ) const
      {
         component x = p[0];
         component y = p[1];
         return x >= rect.x0 && x <= rect.x1 &&
                y >= rect.y0 && y <= rect.y1;
      }
   };

   struct LeafNode : public Node
   {
      point_list points;

      LeafNode( const rectangle& r, const point_list& p ) : Node( r ), points( p )
      {
         points.Sort( Lexicographically );
      }

      int Length() const
      {
         return int( points.Length() );
      }

      int Find( component x ) const
      {
         int i = 0;
         for ( int j = Length(); i < j; )
         {
            int m = (i + j) >> 1;
            if ( x < points[m][0] )
               j = m;
            else
               i = m+1;
         }
         for ( int l = i; --l >= 0 && points[l][0] == x; --i ) {}
         return i;
      }

      static bool Lexicographically( const point& a, const point& b )
      {
         component ax = a[0], ay = a[1];
         component bx = b[0], by = b[1];
         return (ax != bx) ? ax < bx : ay < by;
      }
   };

   Node*     m_root;
   int       m_bucketCapacity;
   size_type m_length;

   Node* BuildTree( const rectangle& rect, const point_list& points )
   {
      if ( points.IsEmpty() )
         return nullptr;

      if ( points.Length() <= size_type( m_bucketCapacity ) )
      {
         m_length += points.Length();
         return new LeafNode( rect, points );
      }

      Node* node = new Node( rect );

      double x2 = (rect.x0 + rect.x1)/2;
      double y2 = (rect.y0 + rect.y1)/2;
      point_list nw, ne, sw, se;
      for ( const point& p : points )
      {
         component x = p[0];
         component y = p[1];
         if ( x <= x2 )
         {
            if ( y <= y2 )
               nw.Add( p );
            else
               sw.Add( p );
         }
         else
         {
            if ( y <= y2 )
               ne.Add( p );
            else
               se.Add( p );
         }
      }

      node->nw = BuildTree( rectangle( rect.x0, rect.y0,      x2,      y2 ), nw );
      node->ne = BuildTree( rectangle(      x2, rect.y0, rect.x1,      y2 ), ne );
      node->sw = BuildTree( rectangle( rect.x0,      y2,      x2, rect.y1 ), sw );
      node->se = BuildTree( rectangle(      x2,      y2, rect.x1, rect.y1 ), se );

      // Don't propagate degeneracies.
      if ( node->IsLeaf() )
      {
         delete node;
         return nullptr;
      }

      return node;
   }

   void SearchTree( point_list& found, const rectangle& rect, const Node* node ) const
   {
      if ( node != nullptr )
         if ( node->Intersects( rect ) )
            if ( node->IsLeaf() )
            {
               const LeafNode* leaf = static_cast<const LeafNode*>( node );
               for ( int i = leaf->Find( rect.x0 ), j = leaf->Find( rect.x1 ); i < j; ++i )
               {
                  const point& p = leaf->points[i];
                  component y = p[1];
                  if ( y >= rect.y0 && y <= rect.y1 )
                     found.Add( p );
               }
            }
            else
            {
               SearchTree( found, rect, node->nw );
               SearchTree( found, rect, node->ne );
               SearchTree( found, rect, node->sw );
               SearchTree( found, rect, node->se );
            }
   }

   template <class F>
   void SearchTree( const rectangle& rect, F callback, void* data, const Node* node ) const
   {
      if ( node != nullptr )
         if ( node->Intersects( rect ) )
            if ( node->IsLeaf() )
            {
               const LeafNode* leaf = static_cast<const LeafNode*>( node );
               for ( int i = leaf->Find( rect.x0 ), j = leaf->Find( rect.x1 ); i < j; ++i )
               {
                  const point& p = leaf->points[i];
                  component y = p[1];
                  if ( y >= rect.y0 && y <= rect.y1 )
                     callback( p, data );
               }
            }
            else
            {
               SearchTree( rect, callback, data, node->nw );
               SearchTree( rect, callback, data, node->ne );
               SearchTree( rect, callback, data, node->sw );
               SearchTree( rect, callback, data, node->se );
            }
   }

   void InsertTree( const point& pt, Node*& node )
   {
      if ( node != nullptr )
      {
         component x = pt[0];
         component y = pt[1];

         if ( x < node->rect.x0 )
            node->rect.x0 = x;
         else if ( x > node->rect.x1 )
            node->rect.x1 = x;

         if ( y < node->rect.y0 )
            node->rect.y0 = y;
         else if ( y > node->rect.y1 )
            node->rect.y1 = y;

         if ( node->IsLeaf() )
         {
            LeafNode* leaf = static_cast<LeafNode*>( node );
            if ( leaf->Length() < m_bucketCapacity )
            {
               typename point_list::iterator i =
                  pcl::InsertionPoint( leaf->points.Begin(), leaf->points.End(), pt, LeafNode::Lexicographically );
               leaf->points.Insert( i, pt );
            }
            else
            {
               rectangle rect = leaf->rect;
               double x2 = (rect.x0 + rect.x1)/2;
               double y2 = (rect.y0 + rect.y1)/2;
               point_list nw, ne, sw, se;
               for ( const point& p : leaf->points )
               {
                  component x = p[0];
                  component y = p[1];
                  if ( x <= x2 )
                  {
                     if ( y <= y2 )
                        nw.Add( p );
                     else
                        sw.Add( p );
                  }
                  else
                  {
                     if ( y <= y2 )
                        ne.Add( p );
                     else
                        se.Add( p );
                  }
               }

               if ( x <= x2 )
               {
                  if ( y <= y2 )
                     nw.Add( pt );
                  else
                     sw.Add( pt );
               }
               else
               {
                  if ( y <= y2 )
                     ne.Add( pt );
                  else
                     se.Add( pt );
               }

               delete leaf;

               node = new Node( rect );

               if ( !nw.IsEmpty() )
                  node->nw = new LeafNode( rectangle( rect.x0, rect.y0,      x2,      y2 ), nw );
               if ( !ne.IsEmpty() )
                  node->ne = new LeafNode( rectangle(      x2, rect.y0, rect.x1,      y2 ), ne );
               if ( !sw.IsEmpty() )
                  node->sw = new LeafNode( rectangle( rect.x0,      y2,      x2, rect.y1 ), sw );
               if ( !se.IsEmpty() )
                  node->se = new LeafNode( rectangle(      x2,      y2, rect.x1, rect.y1 ), se );
            }

            ++m_length;
         }
         else
         {
            double x2 = (node->rect.x0 + node->rect.x1)/2;
            double y2 = (node->rect.y0 + node->rect.y1)/2;
            if ( pt[0] <= x2 )
            {
               if ( pt[1] <= y2 )
                  InsertTree( pt, node->nw );
               else
                  InsertTree( pt, node->sw );
            }
            else
            {
               if ( pt[1] <= y2 )
                  InsertTree( pt, node->ne );
               else
                  InsertTree( pt, node->se );
            }
         }
      }
   }

   void DeleteTree( const rectangle& rect, Node*& node )
   {
      if ( node != nullptr )
         if ( node->Intersects( rect ) )
         {
            if ( node->IsLeaf() )
            {
               LeafNode* leaf = static_cast<LeafNode*>( node );
               for ( int i = leaf->Find( rect.x0 ), j = leaf->Find( rect.x1 ); --j >= i; )
               {
                  component y = leaf->points[i][1];
                  if ( y >= rect.y0 && y <= rect.y1 )
                  {
                     leaf->points.Remove( leaf->points.At( i ) );
                     --m_length;
                  }
               }

               if ( leaf->points.IsEmpty() )
               {
                  delete leaf;
                  node = nullptr;
               }
            }
            else
            {
               DeleteTree( rect, node->nw );
               DeleteTree( rect, node->ne );
               DeleteTree( rect, node->sw );
               DeleteTree( rect, node->se );

               if ( node->IsLeaf() )
               {
                  delete node;
                  node = nullptr;
               }
            }
         }
   }

   void DeleteTree( const point& pt, Node*& node )
   {
      if ( node != nullptr )
         if ( node->Includes( pt ) )
         {
            if ( node->IsLeaf() )
            {
               LeafNode* leaf = static_cast<LeafNode*>( node );
               component x = pt[0];
               component y = pt[1];
               for ( int i = leaf->Find( x ); i < leaf->Length() && leaf->points[i][0] == x; ++i )
                  if ( leaf->points[i][1] == y )
                  {
                     int j = i;
                     while ( ++j < leaf->Length() && leaf->points[j][0] == x && leaf->points[j][1] == y ) {}
                     leaf->points.Remove( leaf->points.At( i ), leaf->points.At( j ) );
                     m_length -= j - i;
                     break;
                  }

               if ( leaf->points.IsEmpty() )
               {
                  delete leaf;
                  node = nullptr;
               }
            }
            else
            {
               DeleteTree( pt, node->nw );
               DeleteTree( pt, node->ne );
               DeleteTree( pt, node->sw );
               DeleteTree( pt, node->se );

               if ( node->IsLeaf() )
               {
                  delete node;
                  node = nullptr;
               }
            }
         }
   }

   void DestroyTree( Node* node )
   {
      if ( node != nullptr )
         if ( node->IsLeaf() )
            delete static_cast<LeafNode*>( node );
         else
         {
            DestroyTree( node->nw );
            DestroyTree( node->ne );
            DestroyTree( node->sw );
            DestroyTree( node->se );
            delete node;
         }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_QuadTree_h

// ----------------------------------------------------------------------------
// EOF pcl/QuadTree.h - Released 2017-07-09T18:07:07Z
