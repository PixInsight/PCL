//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0319
// ----------------------------------------------------------------------------
// Structure.cpp - Released 2017-07-09T18:07:33Z
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

#include "Structure.h"

#include <pcl/Math.h>
#include <pcl/File.h>
#include <pcl/Settings.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Structure::Structure( int nw, int sz, const String& nm ) :
name( nm ), size( Max( 3, sz|1 ) ), mask()
{
   nw = Max( 1, nw );

   mask.Add( ByteArray() );
   mask[0].Add( uint8( 1 ), size*size );

   for ( int i = 1; i < nw; ++i )
   {
      mask.Add( ByteArray() );
      mask[i].Add( uint8( 0 ), size*size );
   }
}

Structure::Structure( const Structure& s ) :
name( s.name ), size( s.size ), mask( s.mask )
{
}

Structure::Structure( File& f ) : name(), size( 0 ), mask()
{
   Read( f );
}

Structure::Structure( const IsoString& key ) : name(), size( 0 ), mask()
{
   Load( key );
}

bool Structure::SameStructure( const Structure& x ) const
{
   if ( NumberOfWays() != x.NumberOfWays() || Size() != x.Size() )
      return false;
   for ( int i = 0; i < NumberOfWays(); ++i )
      if ( ::memcmp( mask[i].Begin(), x.mask[i].Begin(), NumberOfElements() ) != 0 )
         return false;
   return true;
}

void Structure::Resize( int sz )
{
   sz = Max( 3, sz|1 );

   if ( sz != Size() )
   {
      for ( int w = 0; w < NumberOfWays(); ++w )
      {
         ByteArray b;
         b.Add( uint8( 0 ), sz*sz );

         if ( sz > Size() )
         {
            for ( int y = 0, d = (sz - Size()) >> 1; y < Size(); ++y )
               for ( int x = 0; x < Size(); ++x )
                  if ( Element( w, x, y ) )
                     b[(y+d)*sz + x+d] = 1;
         }
         else
         {
            for ( int y = 0, d = (Size() - sz) >> 1; y < sz; ++y )
               for ( int x = 0; x < sz; ++x )
                  if ( Element( w, x+d, y+d ) )
                     b[y*sz + x] = 1;
         }

         mask[w] = b;
      }

      size = sz;
   }
}

void Structure::Invert( int w )
{
   for ( int y = 0; y < Size(); ++y )
      for ( int x = 0; x < Size(); ++x )
         SetElement( w, x, y, !Element( w, x, y ) );
}

void Structure::Rotate( int w )
{
   ByteArray t( mask[w] );
   for ( int y = 0; y < Size(); ++y )
      for ( int x = 0; x < Size(); ++x )
         SetElement( w, y, Size()-1-x, t[y*Size() + x] != 0 );
}

void Structure::Circular( int w )
{
   float n2 = 0.5F*Size();
   float n22 = n2*n2;
   for ( int y = 0; y < Size(); ++y )
   {
      float dy = Abs( y+0.5 - n2 );

      for ( int x = 0; x < Size(); ++x )
      {
         float dx = Abs( x+0.5 - n2 );
         SetElement( w, x, y, dy*dy + dx*dx <= n22 );
      }
   }
}

void Structure::Diamond( int w )
{
   int n2 = Size() >> 1;
   for ( int y = 0; y < n2; ++y )
   {
      for ( int x = 0; x < n2-y; ++x )
      {
         SetElement( w, x, y, false );
         SetElement( w, x, Size()-y-1, false );
      }
      for ( int x = n2-y; x <= n2+y; ++x )
      {
         SetElement( w, x, y, true );
         SetElement( w, x, Size()-y-1, true );
      }
      for ( int x = n2+y; ++x < Size(); )
      {
         SetElement( w, x, y, false );
         SetElement( w, x, Size()-y-1, false );
      }
   }
   for ( int x = 0; x < Size(); ++x )
      SetElement( w, x, n2, true );
}

void Structure::Orthogonal( int w )
{
   int n2 = Size() >> 1;
   for ( int y = 0; y < Size(); ++y )
   {
      for ( int x = 0; x < n2; ++x )
         SetElement( w, x, y, false );
      SetElement( w, n2, y, true );
      for ( int x = n2; ++x < Size(); )
         SetElement( w, x, y, false );
   }
   for ( int x = 0; x < Size(); ++x )
      SetElement( w, x, n2, true );
}

void Structure::Diagonal( int w )
{
   int n2 = Size() >> 1;
   for ( int y = 0; y < Size(); ++y )
      for ( int x = 0; x < Size(); ++x )
         SetElement( w, x, y, Abs( x-n2 ) == Abs( y-n2 ) );
}

void Structure::Read( File& f )
{
   mask.Clear();

   f.Read( name );
   f.ReadUI32( size );

   uint32 numberOfWays;
   f.ReadUI32( numberOfWays );

   for ( size_type i = 0; i < numberOfWays; ++i )
   {
      mask.Add( ByteArray() );
      mask[i].Add( uint8( 0 ), size*size );
      f.ReadArray( mask[i].Begin(), mask[i].Length() );
   }
}

void Structure::Write( File& f ) const
{
   f.Write( name );
   f.Write( uint32( size ) );
   f.Write( uint32( mask.Length() ) );
   for ( size_type i = 0; i < mask.Length(); ++i )
      f.WriteArray( mask[i].Begin(), mask[i].Length() );
}

void Structure::Load( const IsoString& key )
{
   String newName;
   uint32 newSize;
   structure_mask_list newMask;

   if ( !Settings::Read( key + "Name", newName ) )
      return;

   if ( !Settings::Read( key + "Size", newSize ) || (newSize & 1) == 0 )
      return;

   unsigned numberOfWays = 0;
   if ( !Settings::Read( key + "NumberOfWays", numberOfWays ) || numberOfWays < 1 || numberOfWays > 100 )
      return;

   for ( size_type i = 0; i < numberOfWays; ++i )
   {
      newMask.Add( ByteArray() );
      if ( !Settings::Read( key + IsoString().Format( "Mask%03d", i ), newMask[i] ) )
         return;
   }

   name = newName;
   size = newSize;
   mask = newMask;
}

void Structure::Save( const IsoString& key ) const
{
   Settings::Write( key + "Name", name );
   Settings::Write( key + "Size", size );
   Settings::Write( key + "NumberOfWays", unsigned( mask.Length() ) );
   for ( size_type i = 0; i < mask.Length(); ++i )
      Settings::Write( key + IsoString().Format( "Mask%03d", i ), mask[i] );
}

// ----------------------------------------------------------------------------

#define SSM_MAGIC    0x464D5353  // 'SSMF'

#define SSM_DATAPOS  256

struct STRHeader
{
   uint32   magic;
   uint32   version;
   String   userInfo;
   uint32   numberOfStructures;
   uint32   dataPosition;  // relative to file beginning;

   STRHeader()
   {
      Initialize();
   }

   void Initialize();

   void Read( pcl::File& );
   void Write( pcl::File& );
};

void STRHeader::Initialize()
{
   magic = SSM_MAGIC;
   version = 0x00000100;
   userInfo.Clear();
   numberOfStructures = 0;
   dataPosition = SSM_DATAPOS;
}

void STRHeader::Read( pcl::File& f )
{
   f.Read( magic );
   f.Read( version );

   if ( magic != SSM_MAGIC || version < 0x0100 )
      throw Error( "Unrecognized SSM file format." );

   f.Read( userInfo );

   f.Read( numberOfStructures ); // ### TODO: Should impose a maximum number of items here?

   f.Read( dataPosition );

   if ( dataPosition < f.Position() || dataPosition > 1024 )
      throw Error( "Invalid SSM header data." );

   f.Seek( dataPosition, pcl::SeekMode::FromBegin );
}

void STRHeader::Write( pcl::File& f )
{
   f.Write( magic );
   f.Write( version );
   f.Write( userInfo );
   f.Write( numberOfStructures );
   f.Write( dataPosition );

   uint8* pad = 0;

   try
   {
      size_type padSz = dataPosition - f.Position();
      pad = new uint8[ padSz ];
      ::memset( pad, 0, padSz );
      f.Write( (const void*)pad, padSz );
      delete [] pad;
   }

   catch ( ... )
   {
      if ( pad != 0 )
         delete [] pad;
      throw;
   }
}

// ----------------------------------------------------------------------------

void StructureCollection::Read( const String& filePath )
{
   try
   {
      Destroy();
      path = File::FullPath( filePath );

      File f;
      f.OpenForReading( path );

      STRHeader h;
      h.Read( f );

      for ( size_type i = 0; i < h.numberOfStructures; ++i )
      {
         Structure* s = new Structure( f );
         collection.Add( s );
         index.Add( new IndexNode( s->Name(), collection.UpperBound() ) );
      }
   }

   ERROR_CLEANUP( Destroy() )
}

void StructureCollection::Write( const String& filePath )
{
   try
   {
      if ( !filePath.IsEmpty() )
         path = File::FullPath( filePath );

      File f;
      f.CreateForWriting( path );

      STRHeader h;
      h.numberOfStructures = uint32( Length() );
      h.Write( f );

      for ( size_type i = 0; i < Length(); ++i )
         (*this)[i].Write( f );
   }

   ERROR_HANDLER
}

void StructureCollection::Load( const IsoString& key )
{
   try
   {
      Destroy();
      unsigned numberOfStructures;
      Settings::Read( key + "NumberOfStructures", numberOfStructures );

      for ( unsigned i = 0; i < numberOfStructures; ++i )
      {
         Structure* s = new Structure( key + IsoString().Format( "Structure%05u/", i ) );
         collection.Add( s );
         index.Add( new IndexNode( s->Name(), collection.UpperBound() ) );
      }
   }

   ERROR_CLEANUP( Destroy() )
}

void StructureCollection::Save( const IsoString& key ) const
{
   try
   {
      Settings::Write( key + "NumberOfStructures", unsigned( Length() ) );
      for ( unsigned i = 0; i < Length(); ++i )
         (*this)[i].Save( key + IsoString().Format( "Structure%05u/", i ) );
   }

   ERROR_HANDLER
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Structure.cpp - Released 2017-07-09T18:07:33Z
