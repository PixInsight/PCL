//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/Compression.cpp - Released 2017-05-28T08:29:05Z
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

#include <pcl/AutoLock.h>
#include <pcl/Compression.h>
#include <pcl/ElapsedTime.h>
#include <pcl/Exception.h>
#include <pcl/ReferenceArray.h>
#include <pcl/StringList.h>
#include <pcl/Thread.h>

#include <lz4/lz4.h>
#include <lz4/lz4hc.h>
#include <zlib/zlib.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CompressionEngine
{
public:

   PCL_CompressionEngine( const Compression& compression ) :
      m_compression( compression )
   {
   }

   Compression::subblock_list operator ()( const void* data, size_type size, Compression::Performance* perf )
   {
      if ( data == nullptr || size == 0 )
         return Compression::subblock_list();

      m_data = reinterpret_cast<ByteArray::const_iterator>( data );

      m_compressionLevel = m_compression.CompressionLevel();
      if ( m_compressionLevel <= 0 )
         m_compressionLevel = m_compression.DefaultCompressionLevel();
      else
         m_compressionLevel = Range( m_compressionLevel, 1, m_compression.MaxCompressionLevel() );

      m_subblockSize = m_compression.SubblockSize();
      if ( m_subblockSize < m_compression.MinBlockSize() || m_subblockSize > m_compression.MaxBlockSize() )
         m_subblockSize = m_compression.MaxBlockSize();

      m_numberOfSubblocks = size / m_subblockSize;
      m_remainingSize = size % m_subblockSize;

      ElapsedTime T;
      double dt = 0;

      ByteArray S;
      if ( m_compression.ByteShufflingEnabled() )
         if ( m_compression.ItemSize() > 1 )
         {
            T.Reset();
            S = Compression::Shuffle( m_data, size, m_compression.ItemSize() );
            dt += T();
            m_data = S.Begin();
         }

      int numberOfThreads = m_compression.IsParallelProcessingEnabled() ?
               Min( m_compression.MaxProcessors(), pcl::Thread::NumberOfThreads( m_numberOfSubblocks + 1, 1 ) ) : 1;
      int subblocksPerThread = int( m_numberOfSubblocks + 1 )/numberOfThreads;

      ReferenceArray<CompressionThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new CompressionThread( *this,
                                             i*subblocksPerThread,
                                             (j < numberOfThreads) ? j*subblocksPerThread : m_numberOfSubblocks + 1 ) );
      m_errors.Clear();

      T.Reset();

      if ( numberOfThreads > 1 )
      {
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Wait();
      }
      else
         threads[0].Run();

      if ( !m_errors.IsEmpty() )
      {
         threads.Destroy();
         m_compression.Throw( String().ToSeparated( m_errors, '\n' ) );
      }

      dt += T();

      Compression::subblock_list subblocks;
      for ( int i = 0; i < numberOfThreads; ++i )
         subblocks.Append( threads[i].subblocks );

      threads.Destroy();

      size_type compressedSize = 0;
      for ( const Compression::Subblock& subblock : subblocks )
         compressedSize += subblock.compressedData.Length();
      compressedSize += subblocks.Size() + sizeof( Compression::subblock_list );

      if ( perf != nullptr )
      {
         perf->sizeReduction = double( size - compressedSize )/size;
         perf->throughput = size/dt/1024/1024; // MiB/s
         perf->numberOfThreads = numberOfThreads;
      }

      if ( compressedSize >= size )
         subblocks.Clear();

      return subblocks;
   }

private:

     const Compression&              m_compression;
           int                       m_compressionLevel;
           size_type                 m_numberOfSubblocks;
           size_type                 m_subblockSize;
           size_type                 m_remainingSize;
           ByteArray::const_iterator m_data;
   mutable Mutex                     m_mutex;
   mutable StringList                m_errors;

   class CompressionThread : public Thread
   {
   public:

      Compression::subblock_list subblocks;

      CompressionThread( const PCL_CompressionEngine& engine, size_type beginSubblock, size_type endSubblock ) :
         E( engine ),
         m_beginSubblock( beginSubblock ),
         m_endSubblock( endSubblock )
      {
      }

      virtual void Run()
      {
         try
         {
            for ( size_type i = m_beginSubblock; i < m_endSubblock; ++i )
            {
               Compression::Subblock subblock;
               subblock.uncompressedSize = (i < E.m_numberOfSubblocks) ? E.m_subblockSize : E.m_remainingSize;
               if ( subblock.uncompressedSize > 0 )
               {
                  ByteArray::const_iterator uncompressedBegin = E.m_data + i*E.m_subblockSize;
                  if ( subblock.uncompressedSize >= E.m_compression.MinBlockSize() )
                  {
                     ByteArray compressedData( E.m_compression.MaxCompressedBlockSize( subblock.uncompressedSize ) );
                     size_type compressedSize = E.m_compression.CompressBlock( compressedData.Begin(), compressedData.Length(),
                                                      uncompressedBegin, subblock.uncompressedSize, E.m_compressionLevel );
                     if ( compressedSize > 0 && compressedSize < subblock.uncompressedSize )
                     {
                        // Compressed subblock.
                        subblock.compressedData = ByteArray( compressedData.Begin(), compressedData.At( compressedSize ) );
                        goto __nextBlock;
                     }
                  }

                  // Subblock too small to be compressed, or data not compressible.
                  subblock.compressedData = ByteArray( uncompressedBegin, uncompressedBegin + subblock.uncompressedSize );

         __nextBlock:

                  if ( E.m_compression.ChecksumsEnabled() )
                     subblock.checksum = subblock.compressedData.Hash64();

                  subblocks.Append( subblock );
               }
            }
         }
         catch ( ... )
         {
            volatile AutoLock lock( E.m_mutex );

            try
            {
               throw;
            }
            catch ( Exception& x )
            {
               E.m_errors << x.Message();
            }
            catch ( std::bad_alloc& )
            {
               E.m_errors << "Out of memory";
            }
            catch ( ... )
            {
               E.m_errors << "Unknown error";
            }
         }
      }

   private:

      const PCL_CompressionEngine& E;
            size_type              m_beginSubblock;
            size_type              m_endSubblock;
   };
};

Compression::subblock_list Compression::Compress( const void* data, size_type size, Compression::Performance* perf ) const
{
   return PCL_CompressionEngine( *this )( data, size, perf );
}

// ----------------------------------------------------------------------------

class PCL_DecompressionEngine
{
public:

   PCL_DecompressionEngine( const Compression& compression ) :
      m_compression( compression )
   {
   }

   size_type operator ()( void* data, size_type maxSize,
                          const Compression::subblock_list& subblocks, Compression::Performance* perf )
   {
      if ( subblocks.IsEmpty() )
         return 0;

      size_type uncompressedSize = 0;
      for ( const Compression::Subblock& subblock : subblocks )
      {
         if ( subblock.compressedData.IsEmpty() || subblock.uncompressedSize == 0 )
            m_compression.Throw( "Invalid compressed subblock data." );
         uncompressedSize += subblock.uncompressedSize;
      }
      if ( maxSize < uncompressedSize )
         m_compression.Throw( String().Format( "Insufficient uncompression buffer length (required %llu, available %llu)",
                                               uncompressedSize, maxSize ) );

      m_uncompressedData = reinterpret_cast<ByteArray::iterator>( data );

      int numberOfThreads = m_compression.IsParallelProcessingEnabled() ?
               Min( m_compression.MaxProcessors(), pcl::Thread::NumberOfThreads( subblocks.Length(), 1 ) ) : 1;
      int subblocksPerThread = int( subblocks.Length() )/numberOfThreads;

      ReferenceArray<DecompressionThread> threads;
      size_type offset = 0;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
      {
         Compression::subblock_list::const_iterator begin = subblocks.At( i*subblocksPerThread );
         Compression::subblock_list::const_iterator end = (j < numberOfThreads) ? subblocks.At( j*subblocksPerThread ) : subblocks.End();

         threads << new DecompressionThread( *this, offset, begin, end );

         if ( j < numberOfThreads )
            for ( Compression::subblock_list::const_iterator i = begin; i != end; ++i )
               offset += i->uncompressedSize;
      }

      m_errors.Clear();

      ElapsedTime T;

      if ( numberOfThreads > 1 )
      {
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Wait();
      }
      else
         threads[0].Run();

      double dt = T();

      threads.Destroy();

      if ( !m_errors.IsEmpty() )
         m_compression.Throw( String().ToSeparated( m_errors, '\n' ) );

      if ( m_compression.ByteShufflingEnabled() )
         if ( m_compression.ItemSize() > 1 )
         {
            T.Reset();
            Compression::InPlaceUnshuffle( m_uncompressedData, uncompressedSize, m_compression.ItemSize() );
            dt += T();
         }

      if ( perf != nullptr )
      {
         size_type compressedSize = 0;
         for ( const Compression::Subblock& subblock : subblocks )
            compressedSize += subblock.compressedData.Length();
         compressedSize += subblocks.Size() + sizeof( Compression::subblock_list );

         perf->sizeReduction = double( uncompressedSize - compressedSize )/uncompressedSize;
         perf->throughput = uncompressedSize/dt/1024/1024; // MiB/s
         perf->numberOfThreads = numberOfThreads;
      }

      return uncompressedSize;
   }

private:

     const Compression&        m_compression;
           ByteArray::iterator m_uncompressedData;
   mutable Mutex               m_mutex;
   mutable StringList          m_errors;

   class DecompressionThread : public Thread
   {
   public:

      DecompressionThread( const PCL_DecompressionEngine& engine,
                           size_type offset,
                           Compression::subblock_list::const_iterator begin,
                           Compression::subblock_list::const_iterator end ) :
         E( engine ),
         m_offset( offset ),
         m_begin( begin ),
         m_end( end )
      {
      }

      virtual void Run()
      {
         try
         {
            size_type uncompressedSize = 0;
            for ( Compression::subblock_list::const_iterator i = m_begin; i != m_end; ++i )
               uncompressedSize += i->uncompressedSize;

            size_type totalSize = 0;
            for ( Compression::subblock_list::const_iterator i = m_begin; i != m_end; ++i )
            {
               if ( i->checksum != 0 )
               {
                  uint64 checksum = i->compressedData.Hash64();
                  if ( i->checksum != checksum )
                     throw String().Format( "Subblock checksum mismatch (offset=%llu, expected %llx, got %llx)",
                                            m_offset+totalSize, i->checksum, checksum );
               }

               if ( i->compressedData.Length() < i->uncompressedSize )
               {
                  // Compressed subblock.
                  size_type subblockSize = E.m_compression.UncompressBlock( E.m_uncompressedData+m_offset+totalSize,
                                                                            uncompressedSize-totalSize,
                                                                            i->compressedData.Begin(), i->compressedData.Length() );
                  if ( subblockSize == 0 )
                     throw String().Format( "Failed to uncompress subblock data (offset=%llu usize=%llu csize=%llu)",
                                            m_offset+totalSize, i->uncompressedSize, i->compressedData.Length() );
                  if ( subblockSize != i->uncompressedSize )
                     throw String().Format( "Uncompressed subblock size mismatch (offset=%llu, expected %llu, got %llu)",
                                            m_offset+totalSize, i->uncompressedSize, subblockSize );
               }
               else
               {
                  // Subblock too small to be compressed, or data not compressible.
                  ::memcpy( E.m_uncompressedData+m_offset+totalSize, i->compressedData.Begin(), i->uncompressedSize );
               }

               totalSize += i->uncompressedSize;
            }
            if ( totalSize != uncompressedSize )
               throw String().Format( "Uncompressed data size mismatch (offset=%llu, expected %llu, got %llu)",
                                      m_offset, uncompressedSize, totalSize );
         }
         catch ( ... )
         {
            volatile AutoLock lock( E.m_mutex );

            try
            {
               throw;
            }
            catch ( String& s )
            {
               E.m_errors << s;
            }
            catch ( Exception& x )
            {
               E.m_errors << x.Message();
            }
            catch ( std::bad_alloc& )
            {
               E.m_errors << "Out of memory";
            }
            catch ( ... )
            {
               E.m_errors << "Unknown error";
            }
         }
      }

   private:

      const PCL_DecompressionEngine&                   E;
            size_type                                  m_offset;
            Compression::subblock_list::const_iterator m_begin;
            Compression::subblock_list::const_iterator m_end;
   };
};

size_type Compression::Uncompress( void* data, size_type maxSize,
                                   const Compression::subblock_list& subblocks, Compression::Performance* perf ) const
{
   return PCL_DecompressionEngine( *this )( data, maxSize, subblocks, perf );
}

// ----------------------------------------------------------------------------

void Compression::Throw( const String& errorMessage ) const
{
   throw Error( AlgorithmName() + " compression: " + errorMessage );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifdef _MSC_VER // shut up!
PCL_WARNINGS_DISABLE_SIZE_T_TO_INT_LOSS
#endif

// ----------------------------------------------------------------------------

int ZLibCompression::MaxCompressionLevel() const
{
   return Z_BEST_COMPRESSION;
}

// ----------------------------------------------------------------------------

int ZLibCompression::DefaultCompressionLevel() const
{
   return 6;
}

// ----------------------------------------------------------------------------

size_type ZLibCompression::MinBlockSize() const
{
   return 64u;
}

// ----------------------------------------------------------------------------

size_type ZLibCompression::MaxBlockSize() const
{
   return uint32_max - 1;
}

// ----------------------------------------------------------------------------

size_type ZLibCompression::MaxCompressedBlockSize( size_type size ) const
{
   return ::compressBound( uint32( size ) );
}

// ----------------------------------------------------------------------------

size_type ZLibCompression::CompressBlock( void* outputData, size_type maxOutputSize,
                                          const void* inputData, size_type inputSize, int level ) const
{
   try
   {
      if ( inputData == nullptr )
         throw Error( "Null input buffer." );
      if ( outputData == nullptr )
         throw Error( "Null output buffer." );
      if ( inputSize >= uint32_max )
         throw Error( "Invalid input size." );
      if ( maxOutputSize > uint32_max )
         throw Error( "Invalid maximum output size." );

      unsigned long outputSize = uint32( maxOutputSize );
      int result = ::compress2( (uint8*)outputData, &outputSize,
                                (const uint8*)inputData, uint32( inputSize ),
                                Range( level, Z_BEST_SPEED, Z_BEST_COMPRESSION ) );
      return (result == Z_OK) ? outputSize : 0;
   }
   catch ( const Exception& x )
   {
      throw Error( "ZLibCompression::CompressBlock(): " + x.Message() );
   }
}

// ----------------------------------------------------------------------------

size_type ZLibCompression::UncompressBlock( void* outputData, size_type maxOutputSize,
                                            const void* inputData, size_type inputSize ) const
{
   try
   {
      if ( inputData == nullptr )
         throw Error( "Null input buffer." );
      if ( outputData == nullptr )
         throw Error( "Null output buffer." );
      if ( inputSize > uint32_max )
         throw Error( "Invalid input size." );
      if ( maxOutputSize > uint32_max )
         throw Error( "Invalid maximum output size." );

      unsigned long outputSize = uint32( maxOutputSize );
      int result = ::uncompress( (uint8*)outputData, &outputSize,
                                 (const uint8*)inputData, uint32( inputSize ) );
      return (result == Z_OK) ? outputSize : 0;
   }
   catch ( const Exception& x )
   {
      throw Error( "ZLibCompression::UncompressBlock(): " + x.Message() );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int LZ4Compression::MaxCompressionLevel() const
{
   return 64;
}

// ----------------------------------------------------------------------------

int LZ4Compression::DefaultCompressionLevel() const
{
   return 64;
}

// ----------------------------------------------------------------------------

size_type LZ4Compression::MinBlockSize() const
{
   return 64u;
}

// ----------------------------------------------------------------------------

size_type LZ4Compression::MaxBlockSize() const
{
   return LZ4_MAX_INPUT_SIZE;
}

// ----------------------------------------------------------------------------

size_type LZ4Compression::MaxCompressedBlockSize( size_type size ) const
{
   return (size <= LZ4_MAX_INPUT_SIZE) ? ::LZ4_compressBound( int( size ) ) : 0;
}

// ----------------------------------------------------------------------------

size_type LZ4Compression::CompressBlock( void* outputData, size_type maxOutputSize,
                                         const void* inputData, size_type inputSize, int level ) const
{
   try
   {
      if ( inputData == nullptr )
         throw Error( "Null input buffer." );
      if ( outputData == nullptr )
         throw Error( "Null output buffer." );
      if ( inputSize > LZ4_MAX_INPUT_SIZE )
         throw Error( "Invalid input size." );
      if ( maxOutputSize > int32_max )
         throw Error( "Invalid maximum output size." );

      int result = ::LZ4_compress_fast( (const char*)inputData, (char*)outputData,
                                        int( inputSize ), int( maxOutputSize ),
                                        65 - Range( level, 1, 64 )/*acceleration*/ );
      return (result > 0) ? uint32( result ) : 0;
   }
   catch ( const Exception& x )
   {
      throw Error( "LZ4Compression::CompressBlock(): " + x.Message() );
   }
}

// ----------------------------------------------------------------------------

size_type LZ4Compression::UncompressBlock( void* outputData, size_type maxOutputSize,
                                           const void* inputData, size_type inputSize ) const
{
   try
   {
      if ( inputData == nullptr )
         throw Error( "Null input buffer." );
      if ( outputData == nullptr )
         throw Error( "Null output buffer." );
      if ( inputSize > int32_max )
         throw Error( "Invalid input size." );
      if ( maxOutputSize > int32_max )
         throw Error( "Invalid maximum output size." );

      int result = ::LZ4_decompress_safe( (const char*)inputData, (char*)outputData,
                                          int( inputSize ), int( maxOutputSize ) );
      return (result > 0) ? uint32( result ) : 0;
   }
   catch ( const Exception& x )
   {
      throw Error( "LZ4Compression::UncompressBlock(): " + x.Message() );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int LZ4HCCompression::MaxCompressionLevel() const
{
   return 16;
}

// ----------------------------------------------------------------------------

int LZ4HCCompression::DefaultCompressionLevel() const
{
   return 9;
}

// ----------------------------------------------------------------------------

size_type LZ4HCCompression::MinBlockSize() const
{
   return 64u;
}

// ----------------------------------------------------------------------------

size_type LZ4HCCompression::MaxBlockSize() const
{
   return LZ4_MAX_INPUT_SIZE;
}

// ----------------------------------------------------------------------------

size_type LZ4HCCompression::MaxCompressedBlockSize( size_type size ) const
{
   return (size <= LZ4_MAX_INPUT_SIZE) ? ::LZ4_compressBound( int( size ) ) : 0;
}

// ----------------------------------------------------------------------------

size_type LZ4HCCompression::CompressBlock( void* outputData, size_type maxOutputSize,
                                           const void* inputData, size_type inputSize, int level ) const
{
   try
   {
      if ( inputData == nullptr )
         throw Error( "Null input buffer." );
      if ( outputData == nullptr )
         throw Error( "Null output buffer." );
      if ( inputSize > LZ4_MAX_INPUT_SIZE )
         throw Error( "Invalid input size." );
      if ( maxOutputSize > int32_max )
         throw Error( "Invalid maximum output size." );

      int result = ::LZ4_compress_HC( (const char*)inputData, (char*)outputData,
                                      int( inputSize ), int( maxOutputSize ),
                                      Range( level, 1, 16 ) );
      return (result > 0) ? uint32( result ) : 0;
   }
   catch ( const Exception& x )
   {
      throw Error( "LZ4HCCompression::CompressBlock(): " + x.Message() );
   }
}

// ----------------------------------------------------------------------------

size_type LZ4HCCompression::UncompressBlock( void* outputData, size_type maxOutputSize,
                                             const void* inputData, size_type inputSize ) const
{
   try
   {
      if ( inputData == nullptr )
         throw Error( "Null input buffer." );
      if ( outputData == nullptr )
         throw Error( "Null output buffer." );
      if ( inputSize > int32_max )
         throw Error( "Invalid input size." );
      if ( maxOutputSize > int32_max )
         throw Error( "Invalid maximum output size." );

      int result = ::LZ4_decompress_safe( (const char*)inputData, (char*)outputData,
                                          int( inputSize ), int( maxOutputSize ) );
      return (result > 0) ? uint32( result ) : 0;
   }
   catch ( const Exception& x )
   {
      throw Error( "LZ4HCCompression::UncompressBlock(): " + x.Message() );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Compression.cpp - Released 2017-05-28T08:29:05Z
