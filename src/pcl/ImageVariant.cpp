// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ImageVariant.cpp - Released 2014/10/29 07:34:21 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include <pcl/AutoLock.h>
#include <pcl/ImageVariant.h>
#include <pcl/MetaModule.h> // MetaModule::ProcessEvents()
#include <pcl/PArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

// Masking buffer length in bytes.
// 8 MB seems a reasonable size to fit into a hard disk internal buffer.
const size_type maskBufSize = 0x800000u;

#define SWAP_FILE_MAGIC    0x726177696D616765ull   // 'rawimage'
#define SWAP_FILE_VERSION  0x00010000ul            // version 1.00

// ----------------------------------------------------------------------------

struct SwapFileHeader
{
   uint64   magic;   // SWAP_FILE_MAGIC
   uint32   version; // SWAP_FILE_VERSION

   uint8    isImage;
   uint8    isFloatSample;
   uint8    isComplexSample;
   uint8    bitsPerSample;
   uint8    isSRGB;
   uint8    isSRGBGamma;
   uint8    __r1__[ 10 ]; // reserved

   int32    width;
   int32    height;
   int32    numberOfChannels;
   int32    colorSpace;
   float    gamma;
   float    Y[ 3 ];
   float    x[ 3 ];
   float    y[ 3 ];
   uint8    __r2__[ 20 ]; // reserved
   int32    numberOfThreads;
   uint8    __r3__[ 20 ]; // reserved

   SwapFileHeader()
   {
      Initialize();
   }

   SwapFileHeader( const ImageVariant& image )
   {
      Initialize();

      isImage = image ? 1 : 0;
      isFloatSample = image.IsFloatSample() ? 1 : 0;
      isComplexSample = image.IsComplexSample() ? 1 : 0;
      bitsPerSample = uint8( image.BitsPerSample() );

      if ( isImage )
      {
         width = image->Width();
         height = image->Height();
         numberOfChannels = image->NumberOfChannels();
         colorSpace = image->ColorSpace();

         const RGBColorSystem& rgbws = image->RGBWorkingSpace();

         isSRGB = (rgbws == RGBColorSystem::sRGB) ? 1 : 0;
         if ( !isSRGB )
         {
            isSRGBGamma = rgbws.IsSRGB() ? 1 : 0;
            gamma = rgbws.Gamma();
            memcpy( x, *rgbws.ChromaticityXCoordinates(), sizeof( x ) );
            memcpy( y, *rgbws.ChromaticityYCoordinates(), sizeof( y ) );
            memcpy( Y, *rgbws.LuminanceCoefficients(), sizeof( Y ) );
         }
      }
   }

   void Initialize()
   {
      magic = SWAP_FILE_MAGIC;
      version = SWAP_FILE_VERSION;
      isImage = isFloatSample = isComplexSample = isSRGB = isSRGBGamma = false;
      bitsPerSample = 0;
      width = height = numberOfChannels = colorSpace = 0;
      gamma = 0;
      Y[0] = Y[1] = Y[2] = x[0] = x[1] = x[2] = y[0] = y[1] = y[2] = 0;
      numberOfThreads = 0;
      memset( __r1__, 0, sizeof( __r1__ ) );
      memset( __r2__, 0, sizeof( __r2__ ) );
      memset( __r3__, 0, sizeof( __r3__ ) );
   }
};

// ----------------------------------------------------------------------------

class SwapFileThread : public Thread
{
public:

   typedef PArray<SwapFileThread>   thread_list;

   SwapFileThread( const String& path, size_type offsetBegin, size_type offsetEnd, bool hasHeader ) :
   m_path( path ), m_offsetBegin( offsetBegin ), m_offsetEnd( offsetEnd ), m_hasHeader( hasHeader )
   {
      if ( m_error != 0 )
         delete m_error, m_error = 0;
   }

   static void ConsumeThreads( thread_list& threads, bool processEvents )
   {
      if ( threads.Length() == 1 )
      {
         if ( processEvents )
         {
            threads[0].Start( ThreadPriority::DefaultMax );
            while ( !threads[0].Wait( 0.25 ) )
               Module->ProcessEvents();
         }
         else
            threads[0].Run();
      }
      else
      {
         for ( thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
            i->Start( ThreadPriority::DefaultMax, Distance( threads.Begin(), i ) );
         if ( processEvents )
            for ( thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
               while ( !i->Wait( 0.25 ) )
                  Module->ProcessEvents();
         else
            for ( thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
               i->Wait();
      }

      threads.Destroy();

      if ( m_error )
      {
         Error e( *m_error );
         delete m_error, m_error = 0;
         throw e;
      }
   }

protected:

          String    m_path;
          size_type m_offsetBegin;
          size_type m_offsetEnd;
          bool      m_hasHeader;
   static Mutex     m_mutex;
   static Error*    m_error;
};

Mutex  SwapFileThread::m_mutex;
Error* SwapFileThread::m_error = 0;

// ----------------------------------------------------------------------------

template <class P>
class SwapWriterThread : public SwapFileThread
{
public:

   SwapWriterThread( const GenericImage<P>& image, const String& path,
                     size_type offsetBegin, size_type offsetEnd, const SwapFileHeader* header ) :
   SwapFileThread( path, offsetBegin, offsetEnd, header != 0 ), m_image( image ), m_header( header )
   {
   }

   virtual void Run()
   {
      try
      {
         File f;
         f.CreateForWriting( m_path );

         if ( m_header != 0 )
            f.Write( *m_header );

         size_type byteCount = (m_offsetEnd - m_offsetBegin)*P::BytesPerSample();
         for ( int c = 0; c < m_image.NumberOfChannels(); ++c )
            f.Write( reinterpret_cast<const void*>( m_image[c] + m_offsetBegin ), byteCount );

         f.Close();
      }
      catch ( ... )
      {
         volatile AutoLock lock( m_mutex );

         if ( m_error == 0 )
         {
            try
            {
               throw;
            }
            catch ( File::Exception& x )
            {
               m_error = new Error( "File write error: " + m_path + ": " + x.ErrorMessage() );
            }
            catch ( ... )
            {
               m_error = new Error( "File write error: " + m_path );
            }
         }
      }
   }

private:

   const GenericImage<P>& m_image;
   const SwapFileHeader*  m_header;
};

// ----------------------------------------------------------------------------

template <class P>
class SwapReaderThread : public SwapFileThread
{
public:

   SwapReaderThread( GenericImage<P>& image, const String& path,
                     size_type offsetBegin, size_type offsetEnd, bool hasHeader ) :
   SwapFileThread( path, offsetBegin, offsetEnd, hasHeader ), m_image( image )
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

         size_type byteCount = (m_offsetEnd - m_offsetBegin)*P::BytesPerSample();
         for ( int c = 0; c < m_image.NumberOfChannels(); ++c )
            f.Read( reinterpret_cast<void*>( m_image[c] + m_offsetBegin ), byteCount );

         f.Close();
      }
      catch ( ... )
      {
         volatile AutoLock lock( m_mutex );

         if ( m_error == 0 )
         {
            try
            {
               throw;
            }
            catch ( File::Exception& x )
            {
               m_error = new Error( "File read error: " + m_path + ": " + x.ErrorMessage() );
            }
            catch ( ... )
            {
               m_error = new Error( "File read error: " + m_path );
            }
         }
      }
   }

private:

   GenericImage<P>& m_image;
};

// ----------------------------------------------------------------------------

template <class P, class M>
class SwapMaskerThread : public SwapFileThread
{
public:

   SwapMaskerThread( GenericImage<P>& image, const String& srcPath, const GenericImage<M>& mask, bool invert,
                     size_type offsetBegin, size_type offsetEnd, bool hasHeader ) :
   SwapFileThread( srcPath, offsetBegin, offsetEnd, hasHeader ), m_image( image ), m_mask( mask ), m_invert( invert )
   {
   }

   virtual void Run()
   {
      try
      {
         size_type totalPixels = m_offsetEnd - m_offsetBegin;
         size_type blockPixels = maskBufSize/P::BytesPerSample();
         size_type numberOfBlocks = totalPixels/blockPixels;
         size_type remainderPixels = totalPixels%blockPixels;

         Array<typename P::sample> buffer( blockPixels + remainderPixels );

         File f;
         f.OpenForReading( m_path );

         if ( m_hasHeader )
            f.SetPosition( sizeof( SwapFileHeader ) );

         for ( int c = 0; c < m_image.NumberOfNominalChannels(); ++c )
         {
                  typename P::sample* b = m_image.PixelData( c ) + m_offsetBegin;
            const typename M::sample* m = m_mask.PixelData( m_mask.IsColor() ? c : 0 ) + m_offsetBegin;

            for ( size_type i = 0; i <= numberOfBlocks; ++i )
            {
               size_type thisBlockPixels = (i < numberOfBlocks) ? blockPixels : remainderPixels;
               if ( thisBlockPixels == 0 ) // last block and no remainder pixels?
                  break;

               f.Read( reinterpret_cast<void*>( buffer.Begin() ), thisBlockPixels*P::BytesPerSample() );

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

         if ( m_error == 0 )
         {
            try
            {
               throw;
            }
            catch ( File::Exception& x )
            {
               m_error = new Error( "File read error (masking): " + m_path + ": " + x.ErrorMessage() );
            }
            catch ( ... )
            {
               m_error = new Error( "File read error (masking): " + m_path );
            }
         }
      }
   }

private:

         GenericImage<P>& m_image;
   const GenericImage<M>& m_mask;
         bool             m_invert;
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
   for ( StringList::iterator i = sortedDirectories.Begin(); i != sortedDirectories.End(); ++i )
      i->ToUpperCase();
#endif
   sortedDirectories.Sort();
   for ( int i = 0, j = 1; i < header.numberOfThreads; i = j, ++j )
   {
      while ( j < header.numberOfThreads && sortedDirectories[j].BeginsWith( sortedDirectories[i] ) )
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
      threads.Add( new SwapWriterThread<P>( image, path, begin, end, (i == 0) ? &header : 0 ) );
   }

   return threads;
}

template <class P>
static SwapFileThread::thread_list
CreateSwapReaderThreads( GenericImage<P>& image,
                         int numberOfThreads,
                         const String& fileName, const StringList& directories )
{
   size_type numberOfPixels = image.NumberOfPixels();
   size_type pixelsPerThread = numberOfPixels/numberOfThreads;

   SwapFileThread::thread_list threads;

   for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
      threads.Add( new SwapReaderThread<P>( image, SwapThreadFilePath( fileName, directories, i ),
                                            i*pixelsPerThread,
                                            (j < numberOfThreads) ? j*pixelsPerThread : numberOfPixels, i == 0 ) );
   return threads;
}

template <class P, class M>
static SwapFileThread::thread_list
CreateSwapMaskerThreads( GenericImage<P>& image, const GenericImage<M>& mask, bool invert,
                         int numberOfThreads,
                         const String& fileName, const StringList& directories )
{
   size_type numberOfPixels = image.NumberOfPixels();
   size_type pixelsPerThread = numberOfPixels/numberOfThreads;

   SwapFileThread::thread_list threads;

   for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
      threads.Add( new SwapMaskerThread<P,M>( image, SwapThreadFilePath( fileName, directories, i ),
                                              mask, invert,
                                              i*pixelsPerThread,
                                              (j < numberOfThreads) ? j*pixelsPerThread : numberOfPixels, i == 0 ) );
   return threads;
}

template <class P>
static SwapFileThread::thread_list
CreateSwapMaskerThreads( GenericImage<P>& image, const ImageVariant& mask, bool invert,
                         int numberOfThreads,
                         const String& fileName, const StringList& directories )
{
   SwapFileThread::thread_list threads;

   if ( mask.IsFloatSample() )
      switch ( mask.BitsPerSample() )
      {
      case 32 : threads = CreateSwapMaskerThreads( image, static_cast<const Image&>( *mask ),
                                                   invert, numberOfThreads, fileName, directories );
         break;
      case 64 : threads = CreateSwapMaskerThreads( image, static_cast<const DImage&>( *mask ),
                                                   invert, numberOfThreads, fileName, directories );
         break;
      }
   else if ( mask.IsComplexSample() )
      switch ( mask.BitsPerSample() )
      {
      case 32 : threads = CreateSwapMaskerThreads( image, static_cast<const ComplexImage&>( *mask ),
                                                   invert, numberOfThreads, fileName, directories );
         break;
      case 64 : threads = CreateSwapMaskerThreads( image, static_cast<const DComplexImage&>( *mask ),
                                                   invert, numberOfThreads, fileName, directories );
         break;
      }
   else
      switch ( mask.BitsPerSample() )
      {
      case  8 : threads = CreateSwapMaskerThreads( image, static_cast<const UInt8Image&>( *mask ),
                                                   invert, numberOfThreads, fileName, directories );
         break;
      case 16 : threads = CreateSwapMaskerThreads( image, static_cast<const UInt16Image&>( *mask ),
                                                   invert, numberOfThreads, fileName, directories );
         break;
      case 32 : threads = CreateSwapMaskerThreads( image, static_cast<const UInt32Image&>( *mask ),
                                                   invert, numberOfThreads, fileName, directories );
         break;
      }

   return threads;
}

// ----------------------------------------------------------------------------

void ImageVariant::WriteSwapFiles( const String& fileName, const StringList& directories, bool processEvents ) const
{
   ValidateSwapFileParameters( "WriteSwapFiles", fileName, directories );

   if ( !*this )
      return;

   SwapFileHeader h( *this );

   SwapFileThread::thread_list threads;

   if ( IsFloatSample() )
      switch ( BitsPerSample() )
      {
      case 32 : threads = CreateSwapWriterThreads( static_cast<const Image&>( **this ),
                                                   fileName, directories, h );
         break;
      case 64 : threads = CreateSwapWriterThreads( static_cast<const DImage&>( **this ),
                                                   fileName, directories, h );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32 : threads = CreateSwapWriterThreads( static_cast<const ComplexImage&>( **this ),
                                                   fileName, directories, h );
         break;
      case 64 : threads = CreateSwapWriterThreads( static_cast<const DComplexImage&>( **this ),
                                                   fileName, directories, h );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8 : threads = CreateSwapWriterThreads( static_cast<const UInt8Image&>( **this ),
                                                   fileName, directories, h );
         break;
      case 16 : threads = CreateSwapWriterThreads( static_cast<const UInt16Image&>( **this ),
                                                   fileName, directories, h );
         break;
      case 32 : threads = CreateSwapWriterThreads( static_cast<const UInt32Image&>( **this ),
                                                   fileName, directories, h );
         break;
      }

   if ( threads.IsEmpty() )
      throw Error( "WriteSwapFiles(): Invalid image!" );

   SwapFileThread::ConsumeThreads( threads, processEvents );
}

void ImageVariant::WriteSwapFile( const String& filePath, bool processEvents ) const
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   WriteSwapFiles( File::ExtractName( filePath ) + File::ExtractExtension( filePath ), directories, processEvents );
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

   if ( h.isImage )
   {
      // Check sample data type

      if ( h.isFloatSample || h.isComplexSample )
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

void ImageVariant::ReadSwapFiles( const String& fileName, const StringList& directories, bool processEvents )
{
   ValidateSwapFileParameters( "ReadSwapFiles", fileName, directories );

   Free();

   SwapFileHeader h;
   ReadSwapFileHeader( h, fileName, directories );

   if ( !h.isImage ) // this should not happen!
      return;

   // Create and allocate the image

   CreateImage( h.isFloatSample != 0, h.isComplexSample != 0, h.bitsPerSample );
   AllocateImage( h.width, h.height, h.numberOfChannels, ColorSpace::value_type( h.colorSpace ) );

   // Set RGB Working Space

   if ( h.isSRGB )
   {
      if ( RGBWorkingSpace() != RGBColorSystem::sRGB )
         SetRGBWorkingSpace( RGBColorSystem::sRGB );
   }
   else
   {
      RGBColorSystem rgbws( h.gamma, h.isSRGBGamma != 0, h.x, h.y, h.Y );
      SetRGBWorkingSpace( rgbws );
   }

   // Pixel data

   SwapFileThread::thread_list threads;

   if ( IsFloatSample() )
      switch ( BitsPerSample() )
      {
      case 32 : threads = CreateSwapReaderThreads( static_cast<Image&>( **this ),
                                                   h.numberOfThreads, fileName, directories );
         break;
      case 64 : threads = CreateSwapReaderThreads( static_cast<DImage&>( **this ),
                                                   h.numberOfThreads, fileName, directories );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32 : threads = CreateSwapReaderThreads( static_cast<ComplexImage&>( **this ),
                                                   h.numberOfThreads, fileName, directories );
         break;
      case 64 : threads = CreateSwapReaderThreads( static_cast<DComplexImage&>( **this ),
                                                   h.numberOfThreads, fileName, directories );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8 : threads = CreateSwapReaderThreads( static_cast<UInt8Image&>( **this ),
                                                   h.numberOfThreads, fileName, directories );
         break;
      case 16 : threads = CreateSwapReaderThreads( static_cast<UInt16Image&>( **this ),
                                                   h.numberOfThreads, fileName, directories );
         break;
      case 32 : threads = CreateSwapReaderThreads( static_cast<UInt32Image&>( **this ),
                                                   h.numberOfThreads, fileName, directories );
         break;
      }

   if ( threads.IsEmpty() )
      throw Error( "ReadSwapFiles(): Internal error!" );

   SwapFileThread::ConsumeThreads( threads, processEvents );
}

void ImageVariant::ReadSwapFile( const String& filePath, bool processEvents )
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   ReadSwapFiles( File::ExtractName( filePath ) + File::ExtractExtension( filePath ), directories, processEvents );
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

   if ( !h.isImage ) // this should not happen!
      throw Error( String( "MaskFromSwapFiles(): " ) + "Empty source image: " + fileName );

   if ( (h.isFloatSample != 0) != IsFloatSample() ||
        (h.isComplexSample != 0) != IsComplexSample() || h.bitsPerSample != BitsPerSample() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Incompatible source data type: " + fileName );

   if ( h.width != Width() || h.height != Height() || h.numberOfChannels < NumberOfNominalChannels() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Incompatible source geometry: " + fileName );

   SwapFileThread::thread_list threads;

   if ( IsFloatSample() )
      switch ( BitsPerSample() )
      {
      case 32 : threads = CreateSwapMaskerThreads( static_cast<Image&>( **this ),
                                                   mask, invert, h.numberOfThreads, fileName, directories );
         break;
      case 64 : threads = CreateSwapMaskerThreads( static_cast<DImage&>( **this ),
                                                   mask, invert, h.numberOfThreads, fileName, directories );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32 : threads = CreateSwapMaskerThreads( static_cast<ComplexImage&>( **this ),
                                                   mask, invert, h.numberOfThreads, fileName, directories );
         break;
      case 64 : threads = CreateSwapMaskerThreads( static_cast<DComplexImage&>( **this ),
                                                   mask, invert, h.numberOfThreads, fileName, directories );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8 : threads = CreateSwapMaskerThreads( static_cast<UInt8Image&>( **this ),
                                                   mask, invert, h.numberOfThreads, fileName, directories );
         break;
      case 16 : threads = CreateSwapMaskerThreads( static_cast<UInt16Image&>( **this ),
                                                   mask, invert, h.numberOfThreads, fileName, directories );
         break;
      case 32 : threads = CreateSwapMaskerThreads( static_cast<UInt32Image&>( **this ),
                                                   mask, invert, h.numberOfThreads, fileName, directories );
         break;
      }

   if ( threads.IsEmpty() )
      throw Error( String( "MaskFromSwapFiles(): " ) + "Internal error!" );

   SwapFileThread::ConsumeThreads( threads, processEvents );
}

void ImageVariant::MaskFromSwapFile( const String& filePath, const ImageVariant& mask, bool invert, bool processEvents )
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   MaskFromSwapFiles( File::ExtractName( filePath ) + File::ExtractExtension( filePath ), directories, mask, invert, processEvents );
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

   String name = File::ExtractName( filePath ) + File::ExtractExtension( filePath );
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

   if ( h.isImage )
   {
      totalSize += uint64( h.width ) * h.height * h.numberOfChannels * (h.bitsPerSample >> 3);
      if ( h.isComplexSample )
         totalSize *= 2;
   }

   return totalSize;
}

uint64 ImageVariant::SwapFileSize( const String& filePath )
{
   StringList directories;
   directories.Add( File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath ) );
   return SwapFilesSize( File::ExtractName( filePath ) + File::ExtractExtension( filePath ), directories );
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
      case 32: MaskImage2( image, src, static_cast<const Image&>( *mask ), invert );
         break;
      case 64: MaskImage2( image, src, static_cast<const DImage&>( *mask ), invert );
         break;
      }
   else if ( mask.IsComplexSample() )
      switch ( mask.BitsPerSample() )
      {
      case 32: MaskImage2( image, src, static_cast<const ComplexImage&>( *mask ), invert );
         break;
      case 64: MaskImage2( image, src, static_cast<const DComplexImage&>( *mask ), invert );
         break;
      }
   else
      switch ( mask.BitsPerSample() )
      {
      case  8: MaskImage2( image, src, static_cast<const UInt8Image&>( *mask ), invert );
         break;
      case 16: MaskImage2( image, src, static_cast<const UInt16Image&>( *mask ), invert );
         break;
      case 32: MaskImage2( image, src, static_cast<const UInt32Image&>( *mask ), invert );
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
      case 32: MaskImage1( static_cast<Image&>( **this ),
                           static_cast<const Image&>( *src ), mask, invert );
         break;
      case 64: MaskImage1( static_cast<DImage&>( **this ),
                           static_cast<const DImage&>( *src ), mask, invert );
         break;
      }
   else if ( IsComplexSample() )
      switch ( BitsPerSample() )
      {
      case 32: MaskImage1( static_cast<ComplexImage&>( **this ),
                           static_cast<const ComplexImage&>( *src ), mask, invert );
         break;
      case 64: MaskImage1( static_cast<DComplexImage&>( **this ),
                           static_cast<const DComplexImage&>( *src ), mask, invert );
         break;
      }
   else
      switch ( BitsPerSample() )
      {
      case  8: MaskImage1( static_cast<UInt8Image&>( **this ),
                           static_cast<const UInt8Image&>( *src ), mask, invert );
         break;
      case 16: MaskImage1( static_cast<UInt16Image&>( **this ),
                           static_cast<const UInt16Image&>( *src ), mask, invert );
         break;
      case 32: MaskImage1( static_cast<UInt32Image&>( **this ),
                           static_cast<const UInt32Image&>( *src ), mask, invert );
         break;
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/ImageVariant.cpp - Released 2014/10/29 07:34:21 UTC
