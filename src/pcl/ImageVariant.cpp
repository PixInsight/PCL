//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/ImageVariant.cpp - Released 2016/02/21 20:22:19 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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
#include <pcl/AutoPointer.h>
#include <pcl/Compression.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ImageVariant.h>
#include <pcl/MetaModule.h> // MetaModule::ProcessEvents()
#include <pcl/ReferenceArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Masking buffer length in bytes for uncompressed raw pixel data.
 * 8 MB seems a reasonable size to fit into a hard disk internal buffer.
 */
const size_type maskBufSize = 0x800000u;

#define SWAP_FILE_MAGIC    0x726177696D616765ull   // 'rawimage'
#define SWAP_FILE_VERSION  0x00010000ul            // version 1.00

// ----------------------------------------------------------------------------

struct SwapFileHeader
{
   uint64 magic            = SWAP_FILE_MAGIC;
   uint32 version          = SWAP_FILE_VERSION;
   uint8  validImage       = 0;
   uint8  floatSample      = 0;
   uint8  complexSample    = 0;
   uint8  bitsPerSample    = 0;
   uint8  sRGB             = 0;
   uint8  sRGBGamma        = 0;
#ifndef _MSC_VER
   uint8  rsv1[ 10 ]       = {}; // reserved
#else
   uint8  rsv1[ 10 ];
#endif
   int32  width            = 0;
   int32  height           = 0;
   int32  numberOfChannels = 0;
   int32  colorSpace       = 0;
   float  gamma            = 0;
#ifndef _MSC_VER
   float  Y[ 3 ]           = {};
   float  x[ 3 ]           = {};
   float  y[ 3 ]           = {};
   uint8  rsv2[ 20 ]       = {}; // reserved
#else
   float  Y[ 3 ];
   float  x[ 3 ];
   float  y[ 3 ];
   uint8  rsv2[ 20 ];
#endif
   int32  numberOfThreads  = 0;
   uint8  compression      = SwapCompression::None;
#ifndef _MSC_VER
   uint8  rsv3[ 19 ]       = {}; // reserved
#else
   uint8  rsv3[ 19 ];
#endif

#ifndef _MSC_VER
   SwapFileHeader() = default;
#else
   SwapFileHeader()
   {
      ::memset( rsv1, 0, sizeof( rsv1 ) );
      ::memset( rsv2, 0, sizeof( rsv2 ) );
      ::memset( rsv3, 0, sizeof( rsv3 ) );
      Y[0] = Y[1] = Y[2] =
      x[0] = x[1] = x[2] =
      y[0] = y[1] = y[2] = 0;
   }
#endif

   SwapFileHeader( const SwapFileHeader& ) = default;

   SwapFileHeader( const ImageVariant& image )
   {
      *this = SwapFileHeader();

      validImage = image ? 1 : 0;
      floatSample = image.IsFloatSample() ? 1 : 0;
      complexSample = image.IsComplexSample() ? 1 : 0;
      bitsPerSample = uint8( image.BitsPerSample() );

      if ( validImage )
      {
         width = image->Width();
         height = image->Height();
         numberOfChannels = image->NumberOfChannels();
         colorSpace = image->ColorSpace();

         const RGBColorSystem& rgbws = image->RGBWorkingSpace();
         sRGB = (rgbws == RGBColorSystem::sRGB) ? 1 : 0;
         if ( !sRGB )
         {
            sRGBGamma = rgbws.IsSRGB() ? 1 : 0;
            gamma = rgbws.Gamma();
            memcpy( x, *rgbws.ChromaticityXCoordinates(), sizeof( x ) );
            memcpy( y, *rgbws.ChromaticityYCoordinates(), sizeof( y ) );
            memcpy( Y, *rgbws.LuminanceCoefficients(), sizeof( Y ) );
         }
      }
   }
};

// ----------------------------------------------------------------------------

class SwapFileThread : public Thread
{
public:

   typedef ReferenceArray<SwapFileThread>   thread_list;

   SwapFileThread( const String& path, size_type offsetBegin, size_type offsetEnd, bool hasHeader ) :
      m_path( path ),
      m_offsetBegin( offsetBegin ),
      m_offsetEnd( offsetEnd ),
      m_hasHeader( hasHeader )
   {
   }

   static void ConsumeThreads( thread_list& threads, bool processEvents,
                               const ImageVariant& image, Compression::Performance* perf = nullptr )
   {
      m_errors.Clear();

      if ( threads.Length() > 1 )
      {
         for ( thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
            i->Start( ThreadPriority::DefaultMax, Distance( threads.Begin(), i ) );
         if ( processEvents )
            for ( thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
               while ( !i->Wait( 250 ) )
                  Module->ProcessEvents( true/*excludeUserInputEvents*/ );
         else
            for ( thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
               i->Wait();
      }
      else
      {
         if ( processEvents )
         {
            threads[0].Start( ThreadPriority::DefaultMax );
            while ( !threads[0].Wait( 250 ) )
               Module->ProcessEvents( true/*excludeUserInputEvents*/ );
         }
         else
            threads[0].Run();
      }

      if ( perf != nullptr )
      {
         size_type transferSize = 0;
         double compressorTime = 0;
         for ( thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
         {
            transferSize += i->TransferSize();
            compressorTime += i->CompressorTime();
         }

         size_type size = image.ImageSize();
         perf->sizeReduction = double( size - transferSize )/size;
         perf->throughput = size/compressorTime/1024/1024; // MiB/s
         perf->numberOfThreads = int( threads.Length() );
      }

      threads.Destroy();

      if ( !m_errors.IsEmpty() )
         throw String().ToSeparated( m_errors, '\n' );
   }

   virtual size_type TransferSize() const
   {
      return 0;
   }

   virtual double CompressorTime() const
   {
      return 0;
   }

protected:

          String     m_path;
          size_type  m_offsetBegin;
          size_type  m_offsetEnd;
          bool       m_hasHeader;
   static Mutex      m_mutex;
   static StringList m_errors;

   static size_type WriteSubblock( File& f, Compression::subblock_list::const_iterator i )
   {
      f.WriteUI64( i->compressedData.Length() );
      f.WriteUI64( i->uncompressedSize );
      f.WriteUI64( i->checksum );
      f.WriteUI64( 0u ); // reserved
      f.Write( reinterpret_cast<const void*>( i->compressedData.Begin() ), fsize_type( i->compressedData.Length() ) );
      return i->compressedData.Length() + 4 * sizeof( uint64 );
   }

   static size_type ReadSubblock( File& f, Compression::subblock_list::iterator i )
   {
      size_type compressedSize = 0;
      f.ReadUI64( compressedSize );
      i->compressedData = ByteArray( compressedSize );
      f.ReadUI64( i->uncompressedSize );
      f.ReadUI64( i->checksum );
#ifndef _MSC_VER
      volatile uint64 rsv;
#else
      uint64 rsv;
#endif
      f.ReadUI64( rsv ); // reserved
      f.Read( reinterpret_cast<void*>( i->compressedData.Begin() ), fsize_type( compressedSize ) );
      return compressedSize + 4 * sizeof( uint64 );
   }

   static size_type WriteData( File& f,
                               const Compression* compressor, double& compressionTimeAcc,
                               const void* data, size_type size )
   {
      if ( compressor != nullptr )
      {
         ElapsedTime T;
         Compression::subblock_list subblocks = compressor->Compress( data, size );
         compressionTimeAcc += T();
         f.WriteUI32( subblocks.Length() );
         if ( !subblocks.IsEmpty() )
         {
            // Data compressed.
            size_type compressedSize = 0;
            for ( Compression::subblock_list::const_iterator i = subblocks.Begin(); i != subblocks.End(); ++i )
               compressedSize += WriteSubblock( f, i );
            return compressedSize;
         }
      }

      // Either no compression, or the data is not compressible.
      f.Write( data, fsize_type( size ) );
      return size;
   }

   static size_type ReadData( File& f,
                              const Compression* compressor, double& decompressionTimeAcc,
                              void* data, size_type size )
   {
      if ( compressor != nullptr )
      {
         size_type subblockCount = 0;
         f.ReadUI32( subblockCount );
         if ( subblockCount > 0 )
         {
            // Compressed data.
            Compression::subblock_list subblocks( subblockCount );
            size_type compressedSize = 0;
            for ( Compression::subblock_list::iterator i = subblocks.Begin(); i != subblocks.End(); ++i )
               compressedSize += ReadSubblock( f, i );
            ElapsedTime T;
            (void)compressor->Uncompress( data, size, subblocks );
            decompressionTimeAcc += T();
            return compressedSize;
         }
      }

      // Either no compression, or data was not compressible.
      f.Read( data, fsize_type( size ) );
      return size;
   }

   class IncrementalReader
   {
   public:

      IncrementalReader( File& f, const Compression* compressor, size_type size ) : m_file( f )
      {
         if ( compressor != nullptr )
         {
            size_type subblockCount = 0;
            m_file.ReadUI32( subblockCount );
            if ( subblockCount > 0 )
            {
               // Compressed data.
               Compression::subblock_list subblocks( subblockCount );
               for ( Compression::subblock_list::iterator i = subblocks.Begin(); i != subblocks.End(); ++i )
                  ReadSubblock( f, i );
               m_data = ByteArray( size );
               compressor->Uncompress( m_data.Begin(), size, subblocks );
            }
         }
      }

      void operator ()( void* data, size_type size )
      {
         if ( m_data.IsEmpty() )
         {
            // Either no compression, or data was not compressible.
            m_file.Read( data, fsize_type( size ) );
         }
         else
         {
            // Data was compressed.
            ::memcpy( data, m_data.At( m_bytesRead ), size );
         }
         m_bytesRead += size;
      }

   private:

      File&     m_file;
      ByteArray m_data;
      size_type m_bytesRead = 0;
   };
};

Mutex      SwapFileThread::m_mutex;
StringList SwapFileThread::m_errors;

// ----------------------------------------------------------------------------

template <class P>
class SwapWriterThread : public SwapFileThread
{
public:

   SwapWriterThread( const GenericImage<P>& image,
                     const String& path,
                     size_type offsetBegin, size_type offsetEnd,
                     const Compression* compressor,
                     const SwapFileHeader* header ) :

      SwapFileThread( path, offsetBegin, offsetEnd, header != nullptr ),
      m_image( image ),
      m_compressor( compressor ),
      m_header( header ),
      m_outputSize( 0 ),
      m_compressionTime( 0 )
   {
   }

   virtual void Run()
   {
      try
      {
         File f;
         f.CreateForWriting( m_path );

         if ( m_header != nullptr )
            f.Write( *m_header );

         m_outputSize = 0;
         m_compressionTime = 0;
         size_type blockSize = (m_offsetEnd - m_offsetBegin)*P::BytesPerSample();
         for ( int c = 0; c < m_image.NumberOfChannels(); ++c )
            m_outputSize += WriteData( f, m_compressor, m_compressionTime,
                                       reinterpret_cast<const void*>( m_image[c] + m_offsetBegin ), blockSize );

         f.Close();
      }
      catch ( ... )
      {
         volatile AutoLock lock( m_mutex );

         try
         {
            throw;
         }
         catch ( File::Error& x )
         {
            m_errors << x.ErrorMessage() + ": " + m_path;
         }
         catch ( Exception& x )
         {
            m_errors << "File write error: " + m_path + ": " + x.Message();
         }
         catch ( ... )
         {
            m_errors << "File write error: " + m_path;
         }
      }
   }

   virtual size_type TransferSize() const
   {
      return m_outputSize;
   }

   virtual double CompressorTime() const
   {
      return m_compressionTime;
   }

private:

   const GenericImage<P>& m_image;
   const Compression*     m_compressor;
   const SwapFileHeader*  m_header;
         size_type        m_outputSize;
         double           m_compressionTime;
};

// ----------------------------------------------------------------------------

template <class P>
class SwapReaderThread : public SwapFileThread
{
public:

   SwapReaderThread( GenericImage<P>& image,
                     const String& path,
                     size_type offsetBegin, size_type offsetEnd,
                     const Compression* compressor,
                     bool hasHeader ) :

      SwapFileThread( path, offsetBegin, offsetEnd, hasHeader ),
      m_image( image ),
      m_compressor( compressor ),
      m_inputSize( 0 ),
      m_decompressionTime( 0 )
   {
   }

   virtual void Run()
   {
      try
      {
         File f;
         f.OpenForReading( m_path );

         if ( m_hasHeader )
            f.SetPosition( sizeof( SwapFileHeader ) );

         m_inputSize = 0;
         m_decompressionTime = 0;
         size_type blockSize = (m_offsetEnd - m_offsetBegin)*P::BytesPerSample();
         for ( int c = 0; c < m_image.NumberOfChannels(); ++c )
            m_inputSize += ReadData( f, m_compressor, m_decompressionTime,
                                     reinterpret_cast<void*>( m_image[c] + m_offsetBegin ), blockSize );

         f.Close();
      }
      catch ( ... )
      {
         volatile AutoLock lock( m_mutex );

         try
         {
            throw;
         }
         catch ( File::Error& x )
         {
            m_errors << x.ErrorMessage() + ": " + m_path;
         }
         catch ( Exception& x )
         {
            m_errors << "File read error: " + m_path + ": " + x.Message();
         }
         catch ( ... )
         {
            m_errors << "File read error: " + m_path;
         }
      }
   }

   virtual size_type TransferSize() const
   {
      return m_inputSize;
   }

   virtual double CompressorTime() const
   {
      return m_decompressionTime;
   }

private:

         GenericImage<P>& m_image;
   const Compression*     m_compressor;
         size_type        m_inputSize;
         double           m_decompressionTime;
};

// ----------------------------------------------------------------------------

template <class P, class M>
class SwapMaskerThread : public SwapFileThread
{
public:

   SwapMaskerThread( GenericImage<P>& image,
                     const String& srcPath,
                     const GenericImage<M>& mask, bool invert,
                     size_type offsetBegin, size_type offsetEnd,
                     const Compression* compressor,
                     bool hasHeader ) :

      SwapFileThread( srcPath, offsetBegin, offsetEnd, hasHeader ),
      m_image( image ),
      m_mask( mask ),
      m_invert( invert ),
      m_compressor( compressor )
   {
   }

   virtual void Run()
   {
      try
      {
         File f;
         f.OpenForReading( m_path );

         if ( m_hasHeader )
            f.SetPosition( sizeof( SwapFileHeader ) );

         size_type totalPixels = m_offsetEnd - m_offsetBegin;
         size_type blockPixels = maskBufSize/P::BytesPerSample();
         size_type numberOfBlocks = totalPixels/blockPixels;
         size_type remainderPixels = totalPixels%blockPixels;

         Array<typename P::sample> buffer( Max( blockPixels, remainderPixels ) );

         for ( int c = 0; c < m_image.NumberOfNominalChannels(); ++c )
         {
                  typename P::sample* b = m_image[c] + m_offsetBegin;
            const typename M::sample* m = m_mask[m_mask.IsColor() ? c : 0] + m_offsetBegin;

            IncrementalReader read( f, m_compressor, totalPixels*P::BytesPerSample() );

            for ( size_type i = 0; i <= numberOfBlocks; ++i )
            {
               size_type thisBlockPixels = (i < numberOfBlocks) ? blockPixels : remainderPixels;
               if ( thisBlockPixels == 0 ) // last block and no remainder pixels?
                  break;

               read( buffer.Begin(), thisBlockPixels*P::BytesPerSample() );

               for ( const typename P::sample* a  = buffer.Begin(),
                                             * a1 = a + thisBlockPixels;
                     a != a1;
                     ++a, ++b, ++m )
               {
                  double fa, fb, fm;
                  P::FromSample( fa, *a );
                  P::FromSample( fb, *b );
                  M::FromSample( fm, *m );
                  *b = P::ToSample( m_invert ? fb*(1 - fm) + fa*fm : fb*fm + fa*(1 - fm) );
               }
            }
         }

         f.Close();
      }
      catch ( ... )
      {
         volatile AutoLock lock( m_mutex );

         try
         {
            throw;
         }
         catch ( File::Error& x )
         {
            m_errors << x.ErrorMessage() + ": " + m_path;
         }
         catch ( Exception& x )
         {
            m_errors << "File read error: " + m_path + ": " + x.Message();
         }
         catch ( ... )
         {
            m_errors << "File read error: " + m_path;
         }
      }
   }

private:

         GenericImage<P>& m_image;
   const GenericImage<M>& m_mask;
         bool             m_invert;
   const Compression*     m_compressor;
};

// ----------------------------------------------------------------------------

static String SwapThreadFilePath( const String& fileName, const StringList& directories, int index )
{
   String path = directories[index];
   if ( !path.EndsWith( '/' ) )
      path += '/';
   path += fileName;
   if ( index > 0 )
      path = File::AppendToName( path, String().Format( "-part%03d", index ) );
   return path;
}

// ----------------------------------------------------------------------------

static void ValidateSwapFileParameters( const String& routine, const String& fileName, const StringList& directories )
{
   if ( directories.Length() > 1 )
      if ( !Thread::IsRootThread() )
         throw Error( routine + ": Only the root thread can perform parallel swap file operations." );

   if ( fileName.Trimmed().IsEmpty() )
      throw Error( routine + "(): Empty file name." );

   if ( directories.IsEmpty() )
      throw Error( routine + "(): Empty directory list." );

   for ( StringList::const_iterator i = directories.Begin(); i != directories.End(); ++i )
   {
      String dir = i->Trimmed();
      if ( dir.IsEmpty() )
         throw Error( routine + "(): Empty directory." );
      if ( !File::DirectoryExists( dir ) )
         throw Error( routine + "(): No such directory: " + dir );
   }
}

// ----------------------------------------------------------------------------

static int NumberOfThreads( const AbstractImage& image, const StringList& directories )
{
   const size_type overheadLimit = 4096;
   size_type L = directories.Length();
   if ( L > 1 )
   {
      size_type N = image.NumberOfPixels();
      if ( N > overheadLimit )
         return Max( 1, int( Min( L, N/Max( overheadLimit, N/L ) ) ) );
   }
   return 1;
}

// ----------------------------------------------------------------------------

template <class P>
static SwapFileThread::thread_list
CreateSwapWriterThreads( const GenericImage<P>& image,
                         const String& fileName, const StringList& directories,
                         const Compression* compressor,
                         SwapFileHeader& header )
{
   header.numberOfThreads = NumberOfThreads( image, directories );

   size_type numberOfPixels = image.NumberOfPixels();
   size_type pixelsPerThread = numberOfPixels/header.numberOfThreads;

   /*
    * Verify available space on target device(s).
    */
   String rootDirectory = directories[0];
   StringList sortedDirectories = directories;
#ifdef __PCL_WINDOWS
   rootDirectory.ToUppercase();
   for ( StringList::iterator i = sortedDirectories.Begin(); i != sortedDirectories.End(); ++i )
      i->ToUppercase();
#endif
   sortedDirectories.Sort();
   for ( int i = 0, j = 1; i < header.numberOfThreads; i = j, ++j )
   {
      while ( j < header.numberOfThreads && sortedDirectories[j].StartsWith( sortedDirectories[i] ) )
         ++j;

      size_type begin = i*pixelsPerThread;
      size_type end = (j < header.numberOfThreads) ? j*pixelsPerThread : numberOfPixels;

      uint64 bytesToWrite = uint64( end - begin )*image.NumberOfChannels()*P::BytesPerSample();
      if ( sortedDirectories[i] == rootDirectory )
         bytesToWrite += sizeof( SwapFileHeader );
      uint64 bytesAvailable = File::GetAvailableSpace( sortedDirectories[i] );

      if ( bytesAvailable < bytesToWrite )
         throw Error( "Insufficient disk space available to write image swap file:\n"
                      + SwapThreadFilePath( fileName, sortedDirectories, i ) + '\n'
                      + String( bytesToWrite ) + " bytes must be written; only "
                      + String( bytesAvailable ) + " bytes are available on the target device." );
   }

   /*
    * Create writer threads
    */
   SwapFileThread::thread_list threads;
   for ( int i = 0, j = 1; i < header.numberOfThreads; ++i, ++j )
   {
      String path = SwapThreadFilePath( fileName, directories, i );
      size_type begin = i*pixelsPerThread;
      size_type end = (j < header.numberOfThreads) ? j*pixelsPerThread : numberOfPixels;
      threads.Add( new SwapWriterThread<P>( image, path,
                                            begin, end,
                                            compressor,
                                            (i == 0) ? &header : nullptr ) );
   }
   return threads;
}

template <class P>
static SwapFileThread::thread_list
CreateSwapReaderThreads( GenericImage<P>& image,
                         int numberOfThreads,
                         const String& fileName, const StringList& directories,
                         const Compression* compressor )
{
   size_type numberOfPixels = image.NumberOfPixels();
   size_type pixelsPerThread = numberOfPixels/numberOfThreads;

   SwapFileThread::thread_list threads;
   for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
      threads.Add( new SwapReaderThread<P>( image, SwapThreadFilePath( fileName, directories, i ),
                                            i*pixelsPerThread,
                                            (j < numberOfThreads) ? j*pixelsPerThread : numberOfPixels,
                                            compressor,
                                            i == 0 ) );
   return threads;
}

template <class P, class M>
static SwapFileThread::thread_list
CreateSwapMaskerThreads( GenericImage<P>& image, const GenericImage<M>& mask, bool invert,
                         int numberOfThreads,
                         const String& fileName, const StringList& directories,
                         const Compression* compressor )
{
   size_type numberOfPixels = image.NumberOfPixels();
   size_type pixelsPerThread = numberOfPixels/numberOfThreads;

   SwapFileThread::thread_list threads;
   for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
      threads.Add( new SwapMaskerThread<P,M>( image, SwapThreadFilePath( fileName, directories, i ),
                                              mask, invert,
                                              i*pixelsPerThread,
                                              (j < numberOfThreads) ? j*pixelsPerThread : numberOfPixels,
                                              compressor,
                                              i == 0 ) );
   return threads;
}

template <class P>
static SwapFileThread::thread_list
CreateSwapMaskerThreads( GenericImage<P>& image, const ImageVariant& mask, bool invert,
                         int numberOfThreads,
                         const String& fileName, const StringList& directories,
                         const Compression* compressor )
{
   SwapFileThread::thread_list threads;

   if ( mask.IsFloatSample() )
      switch ( mask.BitsPerSample() )
      {
      case 32:
         threads = CreateSwapMaskerThreads( image, static_cast<const Image&>( *mask ),
                                            invert, numberOfThreads, fileName, directories, compressor );
         break;
      case 64:
         threads = CreateSwapMaskerThreads( image, static_cast<const DImage&>( *mask ),
                                            invert, numberOfThreads, fileName, directories, compressor );
         break;
      }
   else if ( mask.IsComplexSample() )
      switch ( mask.BitsPerSample() )
      {
      case 32:
         threads = CreateSwapMaskerThreads( image, static_cast<const ComplexImage&>( *mask ),
                                            invert, numberOfThreads, fileName, directories, compressor );
         break;
      case 64:
         threads = CreateSwapMaskerThreads( image, static_cast<const DComplexImage&>( *mask ),
                                            invert, numberOfThreads, fileName, directories, compressor );
         break;
      }
   else
      switch ( mask.BitsPerSample() )
      {
      case  8:
         threads = CreateSwapMaskerThreads( image, static_cast<const UInt8Image&>( *mask ),
                                            invert, numberOfThreads, fileName, directories, compressor );
         break;
      case 16:
         threads = CreateSwapMaskerThreads( image, static_cast<const UInt16Image&>( *mask ),
                                            invert, numberOfThreads, fileName, directories, compressor );
         break;
      case 32:
         threads = CreateSwapMaskerThreads( image, static_cast<const UInt32Image&>( *mask ),
                                            invert, numberOfThreads, fileName, directories, compressor );
         break;
      }

   return threads;
}

// ----------------------------------------------------------------------------

Compression* ImageVariant::NewCompression( swap_compression algorithm, int itemSize )
{
   Compression* compressor = nullptr;
   switch ( algorithm )
   {
   case SwapCompression::None:
      break;
   case SwapCompression::ZLib:
   case SwapCompression::ZLib_SH:
      compressor = new ZLibCompression;
      break;
   case SwapCompression::LZ4:
   case SwapCompression::LZ4_SH:
      compressor = new LZ4Compression;
      break;
   case SwapCompression::LZ4HC:
   case SwapCompression::LZ4HC_SH:
      compressor = new LZ4HCCompression;
      break;
   default:
      throw Error( String().Format( "Unsupported raw image storage compression %x", int( algorithm ) ) );
   }
   switch ( algorithm )
   {
   case SwapCompression::ZLib_SH:
   case SwapCompression::LZ4_SH:
   case SwapCompression::LZ4HC_SH:
      compressor->EnableByteShuffling();
      compressor->SetItemSize( itemSize );
      break;
   default:
      if ( compressor )
         compressor->DisableByteShuffling();
      break;
   }
   return compressor;
}

// ----------------------------------------------------------------------------

void ImageVariant::WriteSwapFiles( const String& fileName, const StringList& directories,
                                   swap_compression compression, Compression::Performance* perf,
                                   bool processEvents ) const
{
   ValidateSwapFileParameters( "WriteSwapFiles", fileName, directories );

   if ( !*this )
      return;

   SwapFileHeader h( *this );

   AutoPointer<Compression> compressor( NewCompression( compression, BytesPerSample() ) );
   if ( compressor )
      h.compression = uint8( compression );

   SwapFileThread::thread_list threads;

   if ( IsFloatSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         threads = CreateSwapWriterThreads( static_cast<const Image&>( **this ),
                                            fileName, directories, compressor, h );
         break;
      case 64:
         threads = CreateSwapWriterThreads( static_cast<const DImage&>( **this ),
                                            fileName, directories, compressor, h );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         threads = CreateSwapWriterThreads( static_cast<const ComplexImage&>( **this ),
                                            fileName, directories, compressor, h );
         break;
      case 64:
         threads = CreateSwapWriterThreads( static_cast<const DComplexImage&>( **this ),
                                            fileName, directories, compressor, h );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8:
         threads = CreateSwapWriterThreads( static_cast<const UInt8Image&>( **this ),
                                            fileName, directories, compressor, h );
         break;
      case 16:
         threads = CreateSwapWriterThreads( static_cast<const UInt16Image&>( **this ),
                                            fileName, directories, compressor, h );
         break;
      case 32:
         threads = CreateSwapWriterThreads( static_cast<const UInt32Image&>( **this ),
                                            fileName, directories, compressor, h );
         break;
      }

   if ( threads.IsEmpty() )
      throw Error( "WriteSwapFiles(): Invalid image!" );

   if ( compressor )
   {
      int numberOfSubthreads = Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 );
      compressor->SetSubblockSize( Range( ImageSize()/numberOfSubthreads,
                                          size_type( 0x100000u ), size_type( 0x80000000u ) ) );
      compressor->SetMaxProcessors( pcl::Max( 1, numberOfSubthreads/int( threads.Length() ) ) );
   }

   SwapFileThread::ConsumeThreads( threads, processEvents, *this, perf );
}

void ImageVariant::WriteSwapFile( const String& filePath,
                                  swap_compression compression, Compression::Performance* perf,
                                  bool processEvents ) const
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   WriteSwapFiles( File::ExtractName( filePath ) + File::ExtractExtension( filePath ), directories,
                   compression, perf, processEvents );
}

// ----------------------------------------------------------------------------

static void ReadSwapFileHeader( SwapFileHeader& h, const String& fileName, const StringList& directories )
{
   String swapFilePath = SwapThreadFilePath( fileName, directories, 0 );
   File f;
   f.OpenForReading( swapFilePath );
   f.Read( h );
   f.Close();

   if ( h.magic != SWAP_FILE_MAGIC )
      throw Error( "The file is not recognized as a valid PCL swap image file: " + swapFilePath );

   if ( h.version != SWAP_FILE_VERSION )
      throw Error( "Unsupported PCL swap image file version: " + swapFilePath );

   if ( h.validImage )
   {
      // Check sample data type

      if ( h.floatSample || h.complexSample )
      {
         if ( h.bitsPerSample != 32 && h.bitsPerSample != 64 )
            throw Error( String( "Invalid swap image file data: " ) + "Unsupported floating-point sample type: " + swapFilePath );
      }
      else
      {
         if ( h.bitsPerSample != 8 && h.bitsPerSample != 16 && h.bitsPerSample != 32 )
            throw Error( String( "Invalid swap image file data: " ) + "Unsupported integer sample type: " + swapFilePath );
      }

      // Check geometry and color space

      if ( h.width <= 0 || h.height <= 0 || h.numberOfChannels <= 0 || h.colorSpace != ColorSpace::Gray && h.numberOfChannels < 3 )
         throw Error( String( "Invalid swap image file data: " ) + "Incorrect image dimensions: " + swapFilePath );

      if ( h.colorSpace <= ColorSpace::Unknown || h.colorSpace >= ColorSpace::NumberOfColorSpaces )
         throw Error( String( "Invalid swap image file data: " ) + "Unknown color space: " + swapFilePath );

      if ( h.numberOfThreads > int( directories.Length() ) )
         throw Error( String( "Invalid swap image file data: " ) + "Incongruent number of threads: " + swapFilePath );

      // Ensure compatibility with some old swap files
      if ( h.numberOfThreads == 0 )
         h.numberOfThreads = 1;
   }
}

static Compression* NewCompressionFromHeader( const SwapFileHeader& h )
{
   return ImageVariant::NewCompression( ImageVariant::swap_compression( h.compression ), h.bitsPerSample >> 3 );
}

void ImageVariant::ReadSwapFiles( const String& fileName, const StringList& directories,
                                  Compression::Performance* perf,
                                  bool processEvents )
{
   ValidateSwapFileParameters( "ReadSwapFiles", fileName, directories );

   Free();

   SwapFileHeader h;
   ReadSwapFileHeader( h, fileName, directories );

   if ( !h.validImage ) // this should not happen!
      return;

   // Data compression

   AutoPointer<Compression> compressor( NewCompressionFromHeader( h ) );

   // Create and allocate the image

   CreateImage( h.floatSample != 0, h.complexSample != 0, h.bitsPerSample );
   AllocateImage( h.width, h.height, h.numberOfChannels, ColorSpace::value_type( h.colorSpace ) );

   // Set RGB Working Space

   if ( h.sRGB )
   {
      if ( RGBWorkingSpace() != RGBColorSystem::sRGB )
         SetRGBWorkingSpace( RGBColorSystem::sRGB );
   }
   else
   {
      RGBColorSystem rgbws( h.gamma, h.sRGBGamma != 0, h.x, h.y, h.Y );
      SetRGBWorkingSpace( rgbws );
   }

   // Pixel data

   SwapFileThread::thread_list threads;

   if ( IsFloatSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         threads = CreateSwapReaderThreads( static_cast<Image&>( **this ),
                                            h.numberOfThreads, fileName, directories, compressor );
         break;
      case 64:
         threads = CreateSwapReaderThreads( static_cast<DImage&>( **this ),
                                            h.numberOfThreads, fileName, directories, compressor );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         threads = CreateSwapReaderThreads( static_cast<ComplexImage&>( **this ),
                                            h.numberOfThreads, fileName, directories, compressor );
         break;
      case 64:
         threads = CreateSwapReaderThreads( static_cast<DComplexImage&>( **this ),
                                            h.numberOfThreads, fileName, directories, compressor );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8:
         threads = CreateSwapReaderThreads( static_cast<UInt8Image&>( **this ),
                                            h.numberOfThreads, fileName, directories, compressor );
         break;
      case 16:
         threads = CreateSwapReaderThreads( static_cast<UInt16Image&>( **this ),
                                            h.numberOfThreads, fileName, directories, compressor );
         break;
      case 32:
         threads = CreateSwapReaderThreads( static_cast<UInt32Image&>( **this ),
                                            h.numberOfThreads, fileName, directories, compressor );
         break;
      }

   if ( threads.IsEmpty() )
      throw Error( "ReadSwapFiles(): Internal error!" );

   SwapFileThread::ConsumeThreads( threads, processEvents, *this, perf );
}

void ImageVariant::ReadSwapFile( const String& filePath, Compression::Performance* perf, bool processEvents )
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   ReadSwapFiles( File::ExtractNameAndSuffix( filePath ), directories, perf, processEvents );
}

// ----------------------------------------------------------------------------

void ImageVariant::MaskFromSwapFiles( const String& fileName, const StringList& directories,
                                      const ImageVariant& mask, bool invert, bool processEvents )
{
   ValidateSwapFileParameters( "MaskFromSwapFiles", fileName, directories );

   if ( !*this )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Empty masked image." );

   if ( !mask )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Empty mask image." );

   if ( mask.Width() != Width() || mask.Height() != Height() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Incompatible mask geometry." );

   if ( mask.ColorSpace() != ColorSpace::Gray && mask.ColorSpace() != ColorSpace() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Incompatible mask color space." );

   SwapFileHeader h;
   ReadSwapFileHeader( h, fileName, directories );

   if ( !h.validImage ) // this should not happen!
      throw Error( String( "MaskFromSwapFiles(): " ) + "Empty source image: " + fileName );

   if ( (h.floatSample != 0) != IsFloatSample() ||
        (h.complexSample != 0) != IsComplexSample() || h.bitsPerSample != BitsPerSample() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Incompatible source data type: " + fileName );

   if ( h.width != Width() || h.height != Height() || h.numberOfChannels < NumberOfNominalChannels() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Incompatible source geometry: " + fileName );

   AutoPointer<Compression> compressor( NewCompressionFromHeader( h ) );

   SwapFileThread::thread_list threads;

   if ( IsFloatSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         threads = CreateSwapMaskerThreads( static_cast<Image&>( **this ),
                                            mask, invert, h.numberOfThreads, fileName, directories, compressor );
         break;
      case 64:
         threads = CreateSwapMaskerThreads( static_cast<DImage&>( **this ),
                                            mask, invert, h.numberOfThreads, fileName, directories, compressor );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         threads = CreateSwapMaskerThreads( static_cast<ComplexImage&>( **this ),
                                            mask, invert, h.numberOfThreads, fileName, directories, compressor );
         break;
      case 64:
         threads = CreateSwapMaskerThreads( static_cast<DComplexImage&>( **this ),
                                            mask, invert, h.numberOfThreads, fileName, directories, compressor );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8:
         threads = CreateSwapMaskerThreads( static_cast<UInt8Image&>( **this ),
                                            mask, invert, h.numberOfThreads, fileName, directories, compressor );
         break;
      case 16:
         threads = CreateSwapMaskerThreads( static_cast<UInt16Image&>( **this ),
                                            mask, invert, h.numberOfThreads, fileName, directories, compressor );
         break;
      case 32:
         threads = CreateSwapMaskerThreads( static_cast<UInt32Image&>( **this ),
                                            mask, invert, h.numberOfThreads, fileName, directories, compressor );
         break;
      }

   if ( threads.IsEmpty() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Internal error!" );

   SwapFileThread::ConsumeThreads( threads, processEvents, *this );
}

void ImageVariant::MaskFromSwapFile( const String& filePath, const ImageVariant& mask, bool invert, bool processEvents )
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   MaskFromSwapFiles( File::ExtractNameAndSuffix( filePath ), directories, mask, invert, processEvents );
}

// ----------------------------------------------------------------------------

void ImageVariant::DeleteSwapFiles( const String& fileName, const StringList& directories )
{
   ValidateSwapFileParameters( "DeleteSwapFiles", fileName, directories );

   SwapFileHeader h;
   ReadSwapFileHeader( h, fileName, directories );

   for ( int i = 0; i < h.numberOfThreads; ++i )
   {
      String filePath = SwapThreadFilePath( fileName, directories, i );
      if ( File::Exists( filePath ) )
      {
         File::SetReadOnly( filePath, false );
         File::Remove( filePath );
      }
   }
}

void ImageVariant::DeleteSwapFile( const String& filePath )
{
   String path = filePath;
   path.Trim();
   if ( path.IsEmpty() )
      throw Error( String( "DeleteSwapFile(): " ) + "Empty file path." );

   String dir = File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath );
   if ( dir.IsEmpty() )
      throw Error( String( "DeleteSwapFile(): " ) + "No directory specified." );

   String name = File::ExtractNameAndSuffix( filePath );
   if ( dir.IsEmpty() )
      throw Error( String( "DeleteSwapFile(): " ) + "No file name specified." );

   StringList directories;
   directories.Add( dir );
   DeleteSwapFiles( name, directories );
}

// ----------------------------------------------------------------------------

uint64 ImageVariant::SwapFilesSize( const String& fileName, const StringList& directories )
{
   ValidateSwapFileParameters( "SwapFilesSize", fileName, directories );

   SwapFileHeader h;
   ReadSwapFileHeader( h, fileName, directories );

   uint64 totalSize = sizeof( SwapFileHeader );
   if ( h.validImage )
   {
      totalSize += uint64( h.width ) * h.height * h.numberOfChannels * (h.bitsPerSample >> 3);
      if ( h.complexSample )
         totalSize *= 2;
   }
   return totalSize;
}

uint64 ImageVariant::SwapFileSize( const String& filePath )
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   return SwapFilesSize( File::ExtractNameAndSuffix( filePath ), directories );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template <class P, class M> static
void MaskImage2( GenericImage<P>& image, const GenericImage<P>& src, const GenericImage<M>& mask, bool invert )
{
   if ( image.Status().IsInitializationEnabled() )
      image.Status().Initialize( "Applying mask", image.NumberOfNominalSamples() );

   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
   {
      const typename P::sample* a = src.PixelData( c );
            typename P::sample* b = image.PixelData( c );
      const typename M::sample* m = mask.PixelData( mask.IsColor() ? c : 0 );

      for ( const typename P::sample* a1 = a + src.NumberOfPixels();
            a != a1;
            ++a, ++b, ++m, ++image.Status() )
      {
         double fa, fb, fm;
         P::FromSample( fa, *a );
         P::FromSample( fb, *b );
         M::FromSample( fm, *m );
         *b = P::ToSample( invert ? fb*(1 - fm) + fa*fm : fb*fm + fa*(1 - fm) );
      }
   }
}

template <class P> static
void MaskImage1( GenericImage<P>& image, const GenericImage<P>& src, const ImageVariant& mask, bool invert )
{
   if ( mask.IsFloatSample() )
      switch ( mask.BitsPerSample() )
      {
      case 32:
         MaskImage2( image, src, static_cast<const Image&>( *mask ), invert );
         break;
      case 64:
         MaskImage2( image, src, static_cast<const DImage&>( *mask ), invert );
         break;
      }
   else if ( mask.IsComplexSample() )
      switch ( mask.BitsPerSample() )
      {
      case 32:
         MaskImage2( image, src, static_cast<const ComplexImage&>( *mask ), invert );
         break;
      case 64:
         MaskImage2( image, src, static_cast<const DComplexImage&>( *mask ), invert );
         break;
      }
   else
      switch ( mask.BitsPerSample() )
      {
      case  8:
         MaskImage2( image, src, static_cast<const UInt8Image&>( *mask ), invert );
         break;
      case 16:
         MaskImage2( image, src, static_cast<const UInt16Image&>( *mask ), invert );
         break;
      case 32:
         MaskImage2( image, src, static_cast<const UInt32Image&>( *mask ), invert );
         break;
      }
}

void ImageVariant::MaskImage( const ImageVariant& src, const ImageVariant& mask, bool invert )
{
   if ( !*this )
      throw Error( "MaskImage(): Empty masked image" );

   if ( !mask )
      throw Error( "MaskImage(): Empty mask image" );

   if ( mask.Width() != Width() || mask.Height() != Height() )
      throw Error( "MaskImage(): Incompatible mask geometry" );

   if ( mask.ColorSpace() != ColorSpace::Gray && mask.ColorSpace() != ColorSpace() )
      throw Error( "MaskImage(): Incompatible mask color space" );

   if ( !src )
      throw Error( "MaskImage(): Empty source image" );

   if ( src.IsFloatSample() != IsFloatSample() || src.IsComplexSample() != IsComplexSample() ||
        src.BitsPerSample() != BitsPerSample() )
      throw Error( "MaskImage(): Incompatible source data type" );

   if ( src.Width() != Width() || src.Height() != Height() )
      throw Error( "MaskImage(): Incompatible source geometry" );

   if ( src.ColorSpace() != ColorSpace::Gray && src.ColorSpace() != ColorSpace() )
      throw Error( "MaskImage(): Incompatible source color space" );

   if ( IsFloatSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         MaskImage1( static_cast<Image&>( **this ), static_cast<const Image&>( *src ), mask, invert );
         break;
      case 64:
         MaskImage1( static_cast<DImage&>( **this ), static_cast<const DImage&>( *src ), mask, invert );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32:
         MaskImage1( static_cast<ComplexImage&>( **this ), static_cast<const ComplexImage&>( *src ), mask, invert );
         break;
      case 64:
         MaskImage1( static_cast<DComplexImage&>( **this ), static_cast<const DComplexImage&>( *src ), mask, invert );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8:
         MaskImage1( static_cast<UInt8Image&>( **this ), static_cast<const UInt8Image&>( *src ), mask, invert );
         break;
      case 16:
         MaskImage1( static_cast<UInt16Image&>( **this ), static_cast<const UInt16Image&>( *src ), mask, invert );
         break;
      case 32:
         MaskImage1( static_cast<UInt32Image&>( **this ), static_cast<const UInt32Image&>( *src ), mask, invert );
         break;
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageVariant.cpp - Released 2016/02/21 20:22:19 UTC
