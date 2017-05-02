//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/Compression.h - Released 2017-05-02T10:38:59Z
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

#ifndef __PCL_Compression_h
#define __PCL_Compression_h

/// \file pcl/Compression.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ByteArray.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup compression_classes Data Compression Classes
 */

/*!
 * \class Compression
 * \brief Abstract base class of data compression algorithm implementations.
 *
 * %Compression defines a common interface for compression algorithms such as
 * Zlib/deflate, LZ4, LZMA, etc.
 *
 * \ingroup compression_classes
 * \sa ZLibCompression, LZ4Compression, LZ4HCCompression, BloscLZCompression
 */
class PCL_CLASS Compression
{
public:

   /*!
    * A structure to hold the compressed data and uncompressed length of
    * compression subblock.
    */
   struct Subblock
   {
      ByteArray compressedData;       //!< Sub-block compressed data.
      size_type uncompressedSize = 0; //!< size in bytes of the uncompressed subblock.
      uint64    checksum         = 0; //!< If non-zero, 64-bit non-cryptographic checksum of the compressed data.
   };

   /*!
    * A structure to hold compression/decompression performance measurements.
    */
   struct Performance
   {
      float  sizeReduction   = 0; //!< Relative size reduction, e.g.: 0.25 for a 25% size reduction.
      double throughput      = 0; //!< Compression/decompression rate in MiB per second.
      int    numberOfThreads = 0; //!< Number of parallel threads used.
   };

   /*!
    * An ordered list of compression subblocks.
    */
   typedef Array<Subblock>    subblock_list;

   /*!
    * Default constructor. This object will be initialized with the following
    * parameters:
    *
    * \li Compression level: Use an algorithm-dependent default level to
    * achieve a good compromise between running speed and compression ratio.
    *
    * \li Byte shuffling: Enabled.
    *
    * \li Item size: One byte (which inhibits shuffling).
    *
    * \li Sub-block checksums: Enabled.
    *
    * \li Sub-block size: Use the maximum possible subblock size for each
    * compression algorithm.
    *
    * \li Parallel processing: Enabled to use the maximum possible number of
    * processors.
    */
   Compression() :
      m_compressionLevel( 0 ), // use default
      m_subblockSize( 0 ), // use largest possible subblocks
      m_itemSize( 1 ),
      m_byteShuffle( true ),
      m_checksums( true ),
      m_parallel( true ),
      m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   /*!
    * Copy constructor.
    */
   Compression( const Compression& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Compression& operator =( const Compression& ) = default;

   /*!
    * Destroys a %Compression object.
    */
   virtual ~Compression()
   {
   }

   /*!
    * Returns the name of this data compression algorithm.
    */
   virtual String AlgorithmName() const = 0;

   /*!
    * Returns the maximum compression optimization level supported by this
    * algorithm. Higher levels tend to provide higher compression ratios at the
    * cost of more computational work.
    */
   virtual int MaxCompressionLevel() const = 0;

   /*!
    * Returns the default compression optimization level for this algorithm.
    * The returned value represents a good compromise between execution speed
    * and compression ratio.
    */
   virtual int DefaultCompressionLevel() const = 0;

   /*!
    * Returns the current compression optimization level.
    *
    * See SetCompressionLevel() for more information.
    */
   int CompressionLevel() const
   {
      return m_compressionLevel;
   }

   /*!
    * Sets an algorithm-dependent compression optimization level.
    *
    * Higher levels tend to provide higher compression ratios at the cost of
    * more computational work. If zero or a negative value is specified, the
    * compression routines will select a default level automatically to achieve
    * a good compromise between speed and compression ratio for each
    * compression algorithm.
    */
   void SetCompressionLevel( int level )
   {
      m_compressionLevel = Range( level, 0, MaxCompressionLevel() );
   }

   /*!
    * Sets the default compression optimization level for this compression
    * algorithm.
    *
    * This function is a shortcut for SetCompressionLevel( 0 ). The default
    * compression level attempts to achieve a good compromise between speed and
    * compression ratio for each compression algorithm.
    */
   void SetDefaultCompressionLevel()
   {
      SetCompressionLevel( 0 );
   }

   /*!
    * Returns true iff byte shuffling is currently enabled for this
    * %Compression object.
    *
    * See EnableByteShuffling() for more information.
    */
   bool ByteShufflingEnabled() const
   {
      return m_byteShuffle;
   }

   /*!
    * Enables byte shuffling for this %Compression object.
    *
    * Enable this option to pre-process the data with a byte shuffling routine
    * before compression. The byte shuffle algorithm reorganizes the data in a
    * clever way that can greatly improve compression ratios, especially for
    * data with high \e locality (or data containing similar values close
    * together), such as most scientific and observational data. This option
    * can be particularly efficient for compression of data structured as
    * contiguous sequences of 16-bit, 32-bit and 64-bit numbers, such as most
    * images processed with PixInsight. For 8-bit data, byte shuffling is a
    * no-op and is hence ignored. Byte shuffling is enabled by default.
    */
   void EnableByteShuffling( bool enable = true )
   {
      m_byteShuffle = enable;
   }

   /*!
    * Disables byte shuffling for this %Compression object.
    *
    * Equivalent to EnableByteShuffling( !disable ).
    */
   void DisableByteShuffling( bool disable = true )
   {
      EnableByteShuffling( !disable );
   }

   /*!
    * Returns the current item size for shuffling.
    *
    * This is the length in bytes of an element in the uncompressed data, used
    * by the byte shuffling algorithm. See EnableByteShuffling() for more
    * information.
    */
   size_type ItemSize() const
   {
      return m_itemSize;
   }

   /*!
    * Sets the item size for shuffling.
    *
    * \a itemSize is the length in bytes of a data item for the byte shuffling
    * algorithm. See EnableByteShuffling() for more information.
    */
   void SetItemSize( size_type itemSize )
   {
      m_itemSize = uint8( Range( itemSize, size_type( 1 ), size_type( 128 ) ) );
   }

   /*!
    * Returns the current compression subblock size in bytes.
    *
    * See SetSubblockSize() for more information.
    */
   size_type SubblockSize() const
   {
      return m_subblockSize;
   }

   /*!
    * Sets the compression subblock size.
    *
    * The specified \a size is the length in bytes of a compression subblock.
    * If a value of zero is specified, the effective block size will be the
    * maximum possible for this compression algorithm (for example, slightly
    * less than 4 GiB for zlib/deflate and slightly less than 2 GiB for LZ4 and
    * BloscLZ). The number of elements in the arrays returned by the Compress()
    * family of functions will depend on this value.
    */
   void SetSubblockSize( size_type size )
   {
      m_subblockSize = Range( size, size_type( 0 ), MaxBlockSize() );
   }

   /*!
    * Returns true iff subblock checksums are currently enabled for this
    * %Compression object.
    *
    * See EnableChecksums() for more information.
    */
   bool ChecksumsEnabled() const
   {
      return m_checksums;
   }

   /*!
    * Enables subblock checksums for this %Compression object.
    *
    * When this option is enabled, a non-cryptographic checksum is computed for
    * each compressed subblock and stored along with the subblock data. These
    * checksums are computed again before decompression and compared with the
    * stored values. This guarantees that the compressed data have not been
    * altered.
    *
    * In current versions of PixInsight, the xxHash algorithm (64-bit version)
    * is used for compressed subblock checksums. xxHash is an extremely fast
    * hashing function, so its impact on compression/decompression speed is
    * negligible. See the pcl::Hash64() function for details.
    */
   void EnableChecksums( bool enable = true )
   {
      m_checksums = enable;
   }

   /*!
    * Disables subblock checksums for this %Compression object.
    *
    * Equivalent to EnableChecksums( !disable ).
    */
   void DisableChecksums( bool disable = true )
   {
      EnableChecksums( !disable );
   }

   /*!
    * Compresses a data block.
    *
    * \param data    Starting address of the data block to be compressed.
    *
    * \param size    Length in bytes of the data block to be compressed.
    *
    * \param perf    If non-null, pointer to a Performance structure where
    *                performance data will be provided.
    *
    * Returns a dynamic array of compressed subblocks. Each array element is a
    * Subblock structure with the compressed data, the uncompressed length in
    * bytes, and an optional checksum. If compression succeeds, the returned
    * array will have at least one element.
    *
    * If the specified \a size is zero, or if one or more subblocks are not
    * compressible with this algorithm, this function returns an empty array.
    *
    * This function can only throw an exception if the specified \a data
    * pointer is null, or in the event of an out-of-memory condition.
    */
   subblock_list Compress( const void* data, size_type size, Performance* perf = nullptr ) const;

   /*!
    * Compression of a direct container with contiguous storage.
    *
    * \param data    Reference to a direct container with contiguous storage
    *                and PCL container semantics. Meaningful Begin() and Size()
    *                member functions are required. Typically, the ByteArray
    *                class is used, although any array-like direct container
    *                class is appropriate, including Array, String, Vector and
    *                Matrix, among others.
    *
    * \param perf    If non-null, pointer to a Performance structure where
    *                performance data will be provided.
    *
    * Returns a dynamic array of compressed subblocks. Each array element is a
    * Subblock structure with the compressed data, the uncompressed length in
    * bytes, and an optional checksum. If compression succeeds, the returned
    * array will have at least one element.
    *
    * If the specified container is empty, or if one or more subblocks are not
    * compressible with this algorithm, this function returns an empty array.
    *
    * This function can only throw an exception in the event of an
    * out-of-memory condition.
    */
   template <class C>
   subblock_list Compress( const C& data, Performance* perf = nullptr ) const
   {
      return Compress( data.Begin(), data.Size(), perf );
   }

   /*!
    * Decompression of a set of compressed subblocks.
    *
    * \param data       Starting address of the output uncompressed data block.
    *
    * \param maxSize    Maximum space in bytes available at \a data. Must be
    *                   equal to or larger than the total uncompressed size.
    *
    * \param subblocks  Reference to a dynamic array of compressed subblocks.
    *                   The specified array should be the result of a previous
    *                   call to one of the Compress() functions for this
    *                   algorithm. This object must have coherent ItemSize()
    *                   and ByteShufflingEnabled() properties to match the ones
    *                   used when the data was compressed.
    *
    * \param perf       If non-null, pointer to a Performance structure where
    *                   performance data will be provided.
    *
    * Upon successful completion, the uncompressed data will be available in
    * the specified output \a data block, and the function will return the
    * length in bytes of the uncompressed data (necessarily smaller than or
    * equal to \a maxSize).
    *
    * In the event of errors, such as invalid or corrupted data, uncompressed
    * block or subblock size mismatch, or an out-of-memory condition, this
    * function throws an Error exception. In such case the data possibly stored
    * in the output \a data buffer will be invalid.
    */
   size_type Uncompress( void* data, size_type maxSize,
                         const subblock_list& subblocks, Performance* perf = nullptr ) const;

   /*!
    * Decompression of a set of compressed subblocks.
    *
    * \param subblocks  Reference to a dynamic array of compressed subblocks.
    *                   The specified array should be the result of a previous
    *                   call to one of the Compress() functions for this
    *                   algorithm. This object must have coherent ItemSize()
    *                   and ByteShufflingEnabled() properties to match the ones
    *                   used when the data was compressed.
    *
    * \param perf       If non-null, pointer to a Performance structure where
    *                   performance data will be provided.
    *
    * Returns the uncompressed data as a ByteArray object.
    *
    * In the event of errors, such as invalid or corrupted data, uncompressed
    * block or subblock size mismatch, or an out-of-memory condition, this
    * function throws an Error exception.
    */
   ByteArray Uncompress( const subblock_list& subblocks, Performance* perf = nullptr ) const
   {
      size_type uncompressedSize = 0;
      for ( const Subblock& subblock : subblocks )
         uncompressedSize += subblock.uncompressedSize;
      ByteArray data( uncompressedSize );
      (void)Uncompress( data.Begin(), uncompressedSize, subblocks, perf );
      return data;
   }

   /*!
    * Decompression of a compressed data block stored in a ByteArray object.
    * Returns the uncompressed data as a ByteArray.
    *
    * See the Uncompress( const subblock_list&, Performance* ) const member
    * function for more information.
    */
   ByteArray Uncompress( const ByteArray& compressedData,
                         size_type uncompressedSize, Performance* perf = nullptr ) const
   {
      Subblock subblock;
      subblock.compressedData = compressedData;
      subblock.uncompressedSize = uncompressedSize;
      return Uncompress( subblock_list() << subblock, perf );
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel
    * execution threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this object.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance. If \a enable is false this parameter is ignored.
    *                A value <= 0 is ignored. The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this object.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this object.
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
    * Sets the maximum number of processors allowed for this object.
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

   int       m_compressionLevel;
   size_type m_subblockSize;
   uint8     m_itemSize;
   bool      m_byteShuffle   : 1;
   bool      m_checksums     : 1;
   bool      m_parallel      : 1;
   unsigned  m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT;

protected:

   /*!
    * Returns the length in bytes of the smallest contiguous data block that
    * can be compressed with this algorithm.
    */
   virtual size_type MinBlockSize() const = 0;

   /*!
    * Returns the length in bytes of the largest contiguous data block that can
    * be compressed with this algorithm.
    */
   virtual size_type MaxBlockSize() const = 0;

   /*!
    * Returns the maximum length in bytes of the contiguous block necessary to
    * compress a contiguous block of the specified \a size in bytes.
    */
   virtual size_type MaxCompressedBlockSize( size_type size ) const = 0;

   /*!
    * Compression of a contiguous block starting at \a inputData with
    * \a inputSize length in bytes, using the specified compression \a level.
    * The compressed data will be written as a contiguous block starting at
    * \a outputData, with length smaller than or equal to \a outputSize.
    * Returns the number of bytes written to \a outputData, or zero if the data
    * are not compressible.
    */
   virtual size_type CompressBlock( void* outputData, size_type outputSize,
                                    const void* inputData, size_type inputSize, int level ) const = 0;

   /*!
    * Decompression of a contiguous compressed block starting at \a inputData
    * with \a inputSize length in bytes. The uncompressed data will be written
    * as a contiguous block starting at \a outputData, with length equal to
    * \a outputSize. Returns the number of bytes written to \a outputData,
    * which should be equal to \a outputSize, or smaller in the event of error.
    */
   virtual size_type UncompressBlock( void* outputData, size_type outputSize,
                                      const void* inputData, size_type inputSize ) const = 0;

   /*!
    * Byte shuffling algorithm applied to \a size bytes starting at \a data,
    * with element length \a itemSize in bytes. Returns the shuffled data as a
    * ByteArray object.
    */
   static ByteArray Shuffle( const uint8* data, size_type size, size_type itemSize )
   {
      ByteArray shuffled( size );
      if ( size > 0 && data != nullptr )
      {
         size_type numberOfItems = size / itemSize;
         ByteArray::iterator s = shuffled.Begin();
         for ( size_type j = 0; j < itemSize; ++j )
         {
            ByteArray::const_iterator u = data + j;
            for ( size_type i = 0; i < numberOfItems; ++i, ++s, u += itemSize )
               *s = *u;
         }
         ::memcpy( s, data + numberOfItems*itemSize, size % itemSize );
      }
      return shuffled;
   }

   /*!
    * Reverse byte shuffling algorithm (or \e unshuffling) applied to \a size
    * bytes starting at \a data, with element length \a itemSize in bytes.
    * Returns the unshuffled data as a ByteArray object.
    */
   static ByteArray Unshuffle( const uint8* data, size_type size, size_type itemSize )
   {
      ByteArray unshuffled( size );
      if ( size > 0 && data != nullptr )
      {
         size_type numberOfItems = size / itemSize;
         ByteArray::const_iterator s = data;
         for ( size_type j = 0; j < itemSize; ++j )
         {
            ByteArray::iterator u = unshuffled.At( j );
            for ( size_type i = 0; i < numberOfItems; ++i, ++s, u += itemSize )
               *u = *s;
         }
         ::memcpy( unshuffled.At( numberOfItems*itemSize ), s, size % itemSize );
      }
      return unshuffled;
   }

   /*!
    * In-place reverse byte shuffling algorithm (or \e unshuffling) applied to
    * \a size bytes starting at \a data, with element length \a itemSize in
    * bytes.
    */
   static void InPlaceUnshuffle( uint8* data, size_type size, size_type itemSize )
   {
      if ( size > 0 && data != nullptr )
      {
         const ByteArray shuffled( data, data+size );
         size_type numberOfItems = size / itemSize;
         ByteArray::const_iterator s = shuffled.Begin();
         for ( size_type j = 0; j < itemSize; ++j )
         {
            ByteArray::iterator u = data + j;
            for ( size_type i = 0; i < numberOfItems; ++i, ++s, u += itemSize )
               *u = *s;
         }
      }
   }

   /*!
    * Helper function to throw an error message with inclusion of the algorithm
    * name.
    */
   void Throw( const String& errorMessage ) const;

   friend class PCL_CompressionEngine;
   friend class PCL_DecompressionEngine;
};

// ----------------------------------------------------------------------------

/*!
 * \class ZLibCompression
 * \brief Implementation of the ZLib/Deflate compression algorithm.
 *
 * This class implements the deflate lossless compression algorithm in the
 * standard zlib format. This is the default PixInsight/XISF compression codec.
 *
 * The underlying implementation in the PixInsight Core application is the
 * well-known zlib library written by Jean-Loup Gailly and Mark Adler.
 *
 * \b References
 *
 * \li Jean-Loup Gailly, L. Peter Deutsch (1996), RFC 1950: <em>ZLIB Compressed
 * Data Format Specification version 3.3</em>
 *
 * \li Jean-Loup Gailly, L. Peter Deutsch (1996), RFC 1951: <em>DEFLATE
 * Compressed Data Format Specification version 1.3</em>
 *
 * \li Greg Roelofs, Mark Adler, Zlib Home Site: http://www.zlib.net/
 *
 * \ingroup compression_classes
 * \sa Compression, LZ4Compression, LZ4HCCompression, BloscLZCompression
 */
class PCL_CLASS ZLibCompression : public Compression
{
public:

   /*!
    * Returns the name of this data compression algorithm (ZLib).
    */
   virtual String AlgorithmName() const
   {
      return "ZLib";
   }

   /*!
    */
   virtual int MaxCompressionLevel() const;

   /*!
    */
   virtual int DefaultCompressionLevel() const;


private:

   /*!
    */
   virtual size_type MinBlockSize() const;

   /*!
    */
   virtual size_type MaxBlockSize() const;

   /*!
    */
   virtual size_type MaxCompressedBlockSize( size_type size ) const;

   /*!
    */
   virtual size_type CompressBlock( void* outputData, size_type outputSize,
                                    const void* inputData, size_type inputSize, int level ) const;

   /*!
    */
   virtual size_type UncompressBlock( void* outputData, size_type outputSize,
                                      const void* inputData, size_type inputSize ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class LZ4Compression
 * \brief Implementation of the LZ4 compression algorithm.
 *
 * This class implements the LZ4 compression algorithm by Yann Collet.
 *
 * LZ4 is a lossless data compression algorithm focused on compression and
 * decompression speed.
 *
 * The underlying implementation in the PixInsight Core application is the
 * reference implementation by Yann Collet, which has been released under a New
 * BSD license.
 *
 * \b References
 *
 * \li LZ4 source code repository: https://github.com/Cyan4973/lz4
 *
 * \li LZ4 website: http://cyan4973.github.io/lz4/
 *
 * \ingroup compression_classes
 * \sa Compression, LZ4HCCompression, ZLibCompression, BloscLZCompression
 */
class PCL_CLASS LZ4Compression : public Compression
{
public:

   /*!
    * Returns the name of this data compression algorithm (LZ4-HC).
    */
   virtual String AlgorithmName() const
   {
      return "LZ4";
   }

   /*!
    */
   virtual int MaxCompressionLevel() const;

   /*!
    */
   virtual int DefaultCompressionLevel() const;


private:

   /*!
    */
   virtual size_type MinBlockSize() const;

   /*!
    */
   virtual size_type MaxBlockSize() const;

   /*!
    */
   virtual size_type MaxCompressedBlockSize( size_type size ) const;

   /*!
    */
   virtual size_type CompressBlock( void* outputData, size_type outputSize,
                                    const void* inputData, size_type inputSize, int level ) const;

   /*!
    */
   virtual size_type UncompressBlock( void* outputData, size_type outputSize,
                                      const void* inputData, size_type inputSize ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class LZ4HCCompression
 * \brief Implementation of the LZ4-HC compression algorithm.
 *
 * This class implements the LZ4 compression algorithm (high compression
 * variant) by Yann Collet.
 *
 * LZ4 is a lossless data compression algorithm focused on compression and
 * decompression speed.
 *
 * The underlying implementation in the PixInsight Core application is the
 * reference implementation by Yann Collet, which has been released under a New
 * BSD license.
 *
 * \b References
 *
 * \li LZ4 source code repository: https://github.com/Cyan4973/lz4
 *
 * \li LZ4 website: http://cyan4973.github.io/lz4/
 *
 * \ingroup compression_classes
 * \sa Compression, LZ4Compression, ZLibCompression, BloscLZCompression
 */
class PCL_CLASS LZ4HCCompression : public Compression
{
public:

   /*!
    * Returns the name of this data compression algorithm (LZ4-HC).
    */
   virtual String AlgorithmName() const
   {
      return "LZ4-HC";
   }

   /*!
    */
   virtual int MaxCompressionLevel() const;

   /*!
    */
   virtual int DefaultCompressionLevel() const;


private:

   /*!
    */
   virtual size_type MinBlockSize() const;

   /*!
    */
   virtual size_type MaxBlockSize() const;

   /*!
    */
   virtual size_type MaxCompressedBlockSize( size_type size ) const;

   /*!
    */
   virtual size_type CompressBlock( void* outputData, size_type outputSize,
                                    const void* inputData, size_type inputSize, int level ) const;

   /*!
    */
   virtual size_type UncompressBlock( void* outputData, size_type outputSize,
                                      const void* inputData, size_type inputSize ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Compression_h

// ----------------------------------------------------------------------------
// EOF pcl/Compression.h - Released 2017-05-02T10:38:59Z
