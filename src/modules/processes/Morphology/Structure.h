//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0300
// ----------------------------------------------------------------------------
// Structure.h - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
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

#ifndef __Structure_h
#define __Structure_h

#include <pcl/ByteArray.h>
#include <pcl/IndirectSortedArray.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class File;

class Structure
{
public:

   typedef Array<ByteArray>   structure_mask_list;

   Structure( int nw = 1, int sz = 3, const String& nm = String() );
   Structure( const Structure& );
   Structure( File& );
   Structure( const IsoString& key );

   String Name() const
   {
      return name;
   }

   void Rename( const String& n )
   {
      name = n;
      name.Trim();
   }

   bool operator ==( const Structure& x ) const
   {
      return name == x.name && SameStructure( x );
   }

   bool operator <( const Structure& x ) const
   {
      return name.CompareIC( x.name ) < 0;
   }

   bool SameStructure( const Structure& ) const;

   int Size() const
   {
      return int( size );
   }

   void Resize( int sz );

   int NumberOfWays() const
   {
      return int( mask.Length() );
   }

   int NumberOfElements() const
   {
      return int( size*size );
   }

   bool IsEmptyWay( int w )
   {
      return *MaxItem( mask[w].Begin(), mask[w].End() ) == 0;
   }

   bool Element( int w, int x, int y ) const
   {
      return mask[w][y*size + x] != 0;
   }

   void SetElement( int w, int x, int y, bool set = true )
   {
      mask[w][y*size + x] = set ? 1 : 0;
   }

   void ClearElement( int w, int x, int y, bool clr = true )
   {
      SetElement( w, x, y, !clr );
   }

   void ClearWay( int w )
   {
      Fill( mask[w].Begin(), mask[w].End(), uint8( 0 ) );
   }

   void SetWay( int w )
   {
      Fill( mask[w].Begin(), mask[w].End(), uint8( 1 ) );
   }

   void GetWay( int w, ByteArray& b ) const
   {
      b = mask[w];
   }

   void SetWay( int w, const ByteArray& b )
   {
      mask[w] = b;
   }

   void RemoveWay( int w, ByteArray& b )
   {
      GetWay( w, b );
      RemoveWay( w );
   }

   void RemoveWay( int w )
   {
      mask.Remove( mask.At( w ) );
   }

   void AddWay( const ByteArray& b )
   {
      mask.Add( b );
   }

   void AddWay( bool set = false )
   {
      mask.Add( ByteArray() );
      (*mask.ReverseBegin()).Add( uint8( set ? 1 : 0 ), size*size );
   }

   void InsertWay( int w, const ByteArray& b )
   {
      mask.Insert( mask.At( w ), b );
   }

   void InsertWay( int w, bool set = false )
   {
      mask.Insert( mask.At( w ), ByteArray() );
      mask[w].Add( uint8( set ? 1 : 0 ), size*size );
   }

   void Clear()
   {
      for ( int i = 0; i < NumberOfWays(); ++i )
         ClearWay( i );
   }

   void Set()
   {
      for ( int i = 0; i < NumberOfWays(); ++i )
         SetWay( i );
   }

   void Invert( int w );
   void Rotate( int w );
   void Circular( int w );
   void Diamond( int w );
   void Orthogonal( int w );
   void Diagonal( int w );

   void Read( File& );
   void Write( File& ) const;

   void Load( const IsoString& key );
   void Save( const IsoString& key ) const;

private:

   String              name;
   uint32              size;
   structure_mask_list mask;

   friend class MorphologicalTransformationInstance;
   friend class MorphologicalTransformationInterface;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class StructureCollection
{
public:

   struct IndexNode
   {
      String    name;
      size_type index;

      IndexNode( const String& s, size_type i = 0 ) : name( s ), index( i )
      {
      }

      IndexNode( const IndexNode& x ) : name( x.name ), index( x.index )
      {
      }

      bool operator ==( const IndexNode& n ) const
      {
         return name == n.name;
      }

      bool operator <( const IndexNode& n ) const
      {
         return name < n.name;
      }
   };

   typedef IndirectArray<Structure>       structure_list;
   typedef IndirectSortedArray<IndexNode> index_list;

   static const size_type notFound = ~size_type( 0 );

   // -------------------------------------------------------------------------

   StructureCollection() : path(), collection(), index()
   {
   }

   virtual ~StructureCollection()
   {
      Destroy();
   }

   void Read( const String& filePath );
   void Write( const String& filePath = String() );

   void Load( const IsoString& key );
   void Save( const IsoString& key ) const;

   String Path() const
   {
      return path;
   }

   size_type Length() const
   {
      return index.Length();
   }

   bool IsEmpty() const
   {
      return index.IsEmpty();
   }

   const Structure& operator[]( size_type i ) const
   {
      return *collection[index[i]->index];
   }

   Structure& operator[]( size_type i )
   {
      return *collection[index[i]->index];
   }

   void AddStructure( const Structure& s )
   {
      collection.Add( new Structure( s ) );
      index.Add( new IndexNode( s.Name(), collection.UpperBound() ) );
   }

   void DeleteStructure( size_type i )
   {
      collection.Delete( collection.At( index[i]->index ) );
      index.Destroy( index.MutableAt( i ) );
   }

   void Reindex()
   {
      index.Destroy();
      collection.Pack();
      for ( size_type i = 0; i < collection.Length(); ++i )
         index.Add( new IndexNode( collection[i]->Name(), i ) );
   }

   void Destroy()
   {
      path.Clear();
      collection.Destroy();
      index.Destroy();
   }

   size_type FindStructure( const String& name ) const
   {
      index_list::const_iterator i = index.Search( IndexNode( name ) );
      return (i != index.End()) ? i-index.Begin() : notFound;
   }

   size_type FindStructure( const Structure& s ) const
   {
      return FindStructure( s.Name() );
   }

   bool HasStructure( const String& name ) const
   {
      return FindStructure( name ) != notFound;
   }

   bool HasStructure( const Structure& s ) const
   {
      return FindStructure( s ) != notFound;
   }

private:

   String         path;
   structure_list collection;
   index_list     index;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Structure_h

// ----------------------------------------------------------------------------
// EOF Structure.h - Released 2017-05-02T09:43:00Z
