//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0125
// ----------------------------------------------------------------------------
// SplitCFAInstance.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2017 Nikolay Volkov
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#include "SplitCFAInstance.h"
#include "SplitCFAModule.h" // for ReadableVersion()

#include <pcl/AutoViewLock.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/ICCProfile.h>
#include <pcl/MetaModule.h> // for ProcessEvents()
#include <pcl/StdStatus.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SplitCFAInstance::SplitCFAInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_targetFrames(),
   p_outputTree( TheOutputTreeParameter->DefaultValue() ),
   p_outputSubDirCFA( TheOutputSubDirCFAParameter->DefaultValue() ),
   p_outputDir( TheOutputDirParameter->DefaultValue() ),
   p_overwrite( TheOverwriteParameter->DefaultValue() ),
   p_prefix( ThePrefixParameter->DefaultValue() ),
   p_postfix( ThePostfixParameter->DefaultValue() )
{
}

SplitCFAInstance::SplitCFAInstance( const SplitCFAInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void SplitCFAInstance::Assign( const ProcessImplementation& p )
{
   const SplitCFAInstance* x = dynamic_cast<const SplitCFAInstance*>( &p );
   if ( x != nullptr )
   {
      p_targetFrames    = x->p_targetFrames;
      p_outputTree      = x->p_outputTree;
      p_outputSubDirCFA = x->p_outputSubDirCFA;
      p_outputDir       = x->p_outputDir;
      p_overwrite       = x->p_overwrite;
      p_prefix          = x->p_prefix;
      p_postfix         = x->p_postfix;
   }
}

bool SplitCFAInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "SplitCFA cannot be executed on complex images.";
      return false;
   }
   if ( view.Image().Width()&1 | view.Image().Height()&1 ) // is odd?
   {
      whyNot = String().Format( "Invalid odd image dimension(s): %dx%d", view.Width(), view.Height() );
      return false;
   }
   return true;
}

bool SplitCFAInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

static void CopySTF( View& target, const View& source )
{
   View::stf_list F;
   source.GetScreenTransferFunctions( F );
   target.SetScreenTransferFunctions( F );
   target.EnableScreenTransferFunctions();
}

template <class P>
void SplitCFAInstance::SplitCFAViewImage( const GenericImage<P>& inputImage, const View& inputView )
{
   for ( int cfaIndex = 0; cfaIndex < 4; ++cfaIndex )
   {
      ImageWindow outputWindow( inputImage.Width()/2, inputImage.Height()/2,
                                inputImage.NumberOfChannels(), P::BitsPerSample(), P::IsFloatSample(),
                                inputImage.NumberOfChannels() > 1 ? true : false, // RGB : Mono
                                true, inputView.Id() + p_postfix + String( cfaIndex ) );

      View outputView = outputWindow.MainView();
      ImageVariant outputImageVariant = outputView.GetImage();
      GenericImage<P>& outputImage = static_cast<GenericImage<P>&>( *outputImageVariant );

      for ( int c = 0; c < inputImage.NumberOfChannels(); ++c )
         for ( int sY = 0, tY = 0; sY < inputImage.Height(); sY += 2, ++tY )
         {
            for ( int sX = 0, tX = 0; sX < inputImage.Width(); sX += 2, ++tX )
            {
               switch( cfaIndex )
               {
               case 0: outputImage( tX, tY, c ) = inputImage( sX,   sY,   c ); break;
               case 1: outputImage( tX, tY, c ) = inputImage( sX,   sY+1, c ); break;
               case 2: outputImage( tX, tY, c ) = inputImage( sX+1, sY,   c ); break;
               case 3: outputImage( tX, tY, c ) = inputImage( sX+1, sY+1, c ); break;
               }
            }
            inputImage.Status() += inputImage.Width()/2;
         }

      o_outputViewId[cfaIndex] = outputView.Id();

      CopySTF( outputView, inputView );
      outputWindow.Show();
   }
}

bool SplitCFAInstance::ExecuteOn( View& view )
{
   for ( int i = 0; i < 4; ++i )
      o_outputViewId[i].Clear();

   volatile AutoViewWriteLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );
   image.Status().Initialize( "Splitting CFA components", image.NumberOfSamples() );

   Console().EnableAbort();

   if ( !image.IsComplexSample() )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: SplitCFAViewImage( static_cast<const Image&>( *image ), view ); break;
         case 64: SplitCFAViewImage( static_cast<const DImage&>( *image ), view ); break;
         }
      else
         switch (image.BitsPerSample())
         {
         case  8: SplitCFAViewImage( static_cast<const UInt8Image&>( *image ), view ); break;
         case 16: SplitCFAViewImage( static_cast<const UInt16Image&>( *image ), view ); break;
         case 32: SplitCFAViewImage( static_cast<const UInt32Image&>( *image ), view ); break;
         }

   return true;
}

// ----------------------------------------------------------------------------

bool SplitCFAInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_targetFrames.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }

   if ( !p_outputDir.IsEmpty() && !File::DirectoryExists( p_outputDir ) )
   {
      whyNot = "The specified output directory does not exist: " + p_outputDir;
      return false;
   }

   for ( auto frame : p_targetFrames )
      if ( frame.enabled && !File::Exists( frame.path ) )
      {
         whyNot = "File not found: " + frame.path;
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

struct FileData
{
   FileFormat*      format = nullptr;  // the file format of retrieved data
   const void*      fsData = nullptr;  // format-specific data
   ImageOptions     options;           // currently used for resolution only
   FITSKeywordArray keywords;          // FITS keywords
   ICCProfile       profile;           // ICC profile

   FileData() = default;

   FileData( FileFormatInstance& file, const ImageOptions& o ) : options( o )
   {
      format = new FileFormat( file.Format() );
      if ( format->UsesFormatSpecificData() )
         fsData = file.FormatSpecificData();
      if ( format->CanStoreKeywords() )
         file.ReadFITSKeywords( keywords );
      if ( format->CanStoreICCProfiles() )
         file.ReadICCProfile( profile );
   }

   ~FileData()
   {
      if ( format != nullptr )
      {
         if ( fsData != nullptr )
            format->DisposeFormatSpecificData( const_cast<void*>( fsData ) ), fsData = nullptr;
         delete format, format = nullptr;
      }
   }
};

class SplitCFAThread : public Thread
{
public:

   SplitCFAThread( ImageVariant* s, FileData* fd, const String& tp, const String& tf, int i ) :
      targetImages(),
      sourceImage( s ),
      targetData( fd ),
      targetPath( tp ),
      targetFolder( tf ),
      subimageIndex( i )
   {
   }

   virtual ~SplitCFAThread()
   {
      targetImages.Destroy();

      if ( targetData != nullptr )
         delete targetData, targetData = nullptr;

      if ( sourceImage != nullptr )
         delete sourceImage, sourceImage = nullptr;
   }

   virtual void Run()
   {
      try
      {
         for( int i = 0; i < 4; ++i ) // CFA patern index
         {
            ImageVariant* image = new ImageVariant();
            image->CreateImageAs( *sourceImage );
            image->AllocateData( sourceImage->Width()/2, sourceImage->Height()/2, sourceImage->NumberOfChannels(), sourceImage->ColorSpace() );
            Perform( *image, *sourceImage, i );
            targetImages << image;
         }
      }
      catch ( ... )
      {
         /*
          * NB: No exceptions can propagate out of a thread!
          * We *only* throw exceptions to stop this thread when data.abort has
          * been set to true by the root thread
          */
      }
   }

   const ImageVariant* TargetImage( int i ) const
   {
      return targetImages[i];
   }

   String TargetPath() const
   {
      return targetPath;
   }

   String TargetFolder() const
   {
      return targetFolder;
   }

   const FileData& TargetData() const
   {
      return *targetData;
   }

   int SubimageIndex() const
   {
      return subimageIndex;
   }

private:

   IndirectArray<ImageVariant> targetImages;  // Target planes
   ImageVariant*               sourceImage;
   FileData*                   targetData;    // Target image parameters and embedded data. It belongs to this thread.
   String                      targetPath;    // File path of this target image
   String                      targetFolder;  // File sub-folder in output dir for this target image
   int                         subimageIndex; // > 0 in case of a multiple image; = 0 otherwise

   template <class P1>
   static void Perform( GenericImage<P1>& targetImage, const GenericImage<P1>& sourceImage, int cfaIndex )
   {
      for ( int c = 0; c < sourceImage.NumberOfChannels(); ++c )
         for ( int sY = 0, tY = 0; sY < sourceImage.Height(); sY+=2, ++tY )
            for ( int sX = 0, tX = 0; sX < sourceImage.Width(); sX+=2, ++tX )
               switch ( cfaIndex )
               {
               case 0: targetImage( tX, tY, c ) = sourceImage( sX,   sY,   c); break;
               case 1: targetImage( tX, tY, c ) = sourceImage( sX,   sY+1, c); break;
               case 2: targetImage( tX, tY, c ) = sourceImage( sX+1, sY,   c); break;
               case 3: targetImage( tX, tY, c ) = sourceImage( sX+1, sY+1, c); break;
               }
   }

   static void Perform( ImageVariant& targetImage, ImageVariant& sourceImage, int cfaIndex )
   {
      if ( sourceImage.IsFloatSample() )
         switch ( sourceImage.BitsPerSample() )
         {
         case 32: Perform( static_cast<Image&>( *targetImage ), static_cast<const Image&>( *sourceImage ), cfaIndex ); break;
         case 64: Perform( static_cast<DImage&>( *targetImage ), static_cast<const DImage&>( *sourceImage ), cfaIndex ); break;
         }
      else
         switch ( sourceImage.BitsPerSample() )
         {
         case  8: Perform( static_cast<UInt8Image&>( *targetImage ), static_cast<const UInt8Image&>( *sourceImage ), cfaIndex ); break;
         case 16: Perform( static_cast<UInt16Image&>( *targetImage ), static_cast<const UInt16Image&>( *sourceImage ), cfaIndex ); break;
         case 32: Perform( static_cast<UInt32Image&>( *targetImage ), static_cast<const UInt32Image&>( *sourceImage ), cfaIndex ); break;
         }
   }
};

// ----------------------------------------------------------------------------

template <class P>
static void LoadImageFile( GenericImage<P>& image, FileFormatInstance& file )
{
   if ( !file.ReadImage( image ) )
      throw CatchedException();
}

static void LoadImageFile( ImageVariant& image, FileFormatInstance& file, const ImageDescriptionArray& images, int index )
{
   if ( !file.SelectImage( index ) )
      throw CatchedException();

   image.CreateSharedImage( images[index].options.ieeefpSampleFormat, false, images[index].options.bitsPerSample );
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: LoadImageFile( static_cast<Image&>( *image ), file ); break;
      case 64: LoadImageFile( static_cast<DImage&>( *image ), file ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: LoadImageFile( static_cast<UInt8Image&>( *image ), file ); break;
      case 16: LoadImageFile( static_cast<UInt16Image&>( *image ), file ); break;
      case 32: LoadImageFile( static_cast<UInt32Image&>( *image ), file ); break;
      }
}

thread_list SplitCFAInstance::LoadTargetFrame( const size_type fileIndex )
{
   Console console;
   const ImageItem& item = p_targetFrames[fileIndex];
   const String filePath = item.path;

   console.WriteLn( "Open " + filePath );

   FileFormat format( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );
   FileFormatInstance file( format );

   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, "raw cfa" ) ) // ### Use input hints to load raw CFA DSLR images
      throw CatchedException();
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty image file." );

   thread_list threads;

   try
   {
      for ( unsigned int index = 0; index < images.Length(); ++index )
      {
         Module->ProcessEvents();

         if ( images[index].info.height&1 | images[index].info.width&1 ) // height is odd
         {
            console.WriteLn( String().Format( "Invalid odd image dimension(s): %dx%d", images[index].info.width, images[index].info.height ) );
            continue;
         }

         if ( images.Length() > 1 )
            console.WriteLn( String().Format( "* Subimage %u of %u", index+1, images.Length() ) );
         if ( !file.SelectImage( index ) )
            throw CatchedException();

         ImageVariant* source = new ImageVariant();
         LoadImageFile( *source, file, images, index );
         threads << new SplitCFAThread( source, new FileData( file, images[index].options ), filePath, item.folder, index );
      }

      console.WriteLn( "Close " + filePath );
      file.Close();
      return threads;
   }
   catch ( ... )
   {
      threads.Destroy();
      throw;
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void SaveImageFile( const GenericImage<P>& image, FileFormatInstance& file )
{
   if ( !file.WriteImage( image ) )
      throw CatchedException();
}

static void SaveImageFile( const ImageVariant& image, FileFormatInstance& file )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: SaveImageFile( static_cast<const Image&>( *image ), file ); break;
      case 64: SaveImageFile( static_cast<const DImage&>( *image ), file ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: SaveImageFile( static_cast<const UInt8Image&>( *image ), file ); break;
      case 16: SaveImageFile( static_cast<const UInt16Image&>( *image ), file ); break;
      case 32: SaveImageFile( static_cast<const UInt32Image&>( *image ), file ); break;
      }
}

inline String UniqueFilePath( const String& filePath )
{
   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

inline String SplitCFAInstance::OutputFilePath( const String& filePath, const String& fileFolder, const size_type index, const int cfaIndex )
{
   String dir = p_outputDir.Trimmed();
   if ( dir.IsEmpty() ) //save to same source location
      dir = File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath );
   else //save to selected location
   {
      if ( p_outputTree )
      {
         //check or create folder and sub-folder if not exist
#if debug
         Console().WriteLn( "dir:" + dir );
         Console().WriteLn( "sub-folder:" + fileFolder );
#endif
         dir.Append( fileFolder );
      }
   }
   if ( dir.IsEmpty() )
      throw Error( dir + ": Unable to determine an output directory." );

   if ( p_outputSubDirCFA )
   {
      if ( !dir.EndsWith( '/' ) )
         dir.Append( '/' );
      dir.Append( "CFA" );
      dir.Append( String( cfaIndex ) );
   }
   if ( !File::DirectoryExists( dir ) )
      File::CreateDirectory( dir, true/*createTree*/ );

   if ( !dir.EndsWith( '/' ) )
      dir.Append( '/' );

   String fileName = File::ExtractName( filePath ).Trimmed();
   if ( !p_prefix.IsEmpty() )
      fileName.Prepend( p_prefix );
   if ( index > 0 )
      fileName.Append( String().Format( "_%02d", index+1 ) );
   if ( !p_postfix.IsEmpty() )
      fileName.Append( p_postfix );
   if ( fileName.IsEmpty() )
      throw Error( filePath + ": Unable to determine an output file name." );
   fileName.Append( String( cfaIndex ) );

   String outputFilePath = dir + fileName + outputExtension;
   Console().WriteLn( "<end><cbr><br>Writing output file: " + outputFilePath );

   if ( File::Exists( outputFilePath ) )
      if ( p_overwrite )
         Console().WarningLn( "** Warning: Overwriting already existing file." );
      else
      {
         outputFilePath = UniqueFilePath( outputFilePath );
         Console().NoteLn( "* File already exists, writing to: " + outputFilePath );
      }

   return outputFilePath;
}

void SplitCFAInstance::SaveImage( const SplitCFAThread* t )
{
   Console console;
   for( int i = 0; i < 4; ++i )
   {
      String outputFilePath = OutputFilePath( t->TargetPath(), t->TargetFolder(), t->SubimageIndex(), i );
      console.WriteLn( "Create " + outputFilePath );

      FileFormat outputFormat( outputExtension, false/*read*/, true/*write*/ );
      FileFormatInstance outputFile( outputFormat );
      if ( !outputFile.Create( outputFilePath ) )
         throw CatchedException();

      const FileData& data = t->TargetData();

      ImageOptions options = data.options; // get resolution, etc.
      outputFile.SetOptions( options );

      if ( data.fsData != nullptr )
         if ( outputFormat.ValidateFormatSpecificData( data.fsData ) )
            outputFile.SetFormatSpecificData( data.fsData );

      /*
       * Add FITS header keywords and preserve existing ones, if possible.
       * NB: A COMMENT or HISTORY keyword cannot have a value; these keywords
       * have only the name and comment components.
       */
      if ( outputFormat.CanStoreKeywords() )
      {
         FITSKeywordArray keywords = data.keywords;
         keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "SplitCFA with " + PixInsightVersion::AsString() ) );
         keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "SplitCFA with " + SplitCFAModule::ReadableVersion() ) );
         keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "SplitCFA" ) );
         outputFile.WriteFITSKeywords( keywords );
      }
      else if ( !data.keywords.IsEmpty() )
            console.WarningLn( "** Warning: The output format cannot store FITS keywords - original keywords not embedded." );

      if ( data.profile.IsProfile() )
         if ( outputFormat.CanStoreICCProfiles() ) outputFile.WriteICCProfile( data.profile );
         else console.WarningLn( "** Warning: The output format cannot store ICC profiles - original profile not embedded." );

      SaveImageFile( *t->TargetImage(i), outputFile );

      console.WriteLn( "Close file." );
      outputFile.Close();
   }
}

bool SplitCFAInstance::ExecuteGlobal()
{
   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );
   }

   Console console;

   try //try 1
   {
      Exception::EnableConsoleOutput();
      Exception::DisableGUIOutput();
      console.EnableAbort();

      int succeeded = 0;
      int skipped = 0;
      const int total = int( p_targetFrames.Length() );
      console.WriteLn( String().Format( "<br>Processing %d target frames:", total ) );

      Array<int> t; // array with file indexes
      for ( int i = 0; i < total; ++i )
         t << i;

      const int n = Min( Thread::NumberOfThreads( 1024, 1 ), total );
      thread_list runningThreads( n );                                     // n = how many threads will run simultaneously
      console.WriteLn( String().Format( "Using %u worker threads", runningThreads.Length() ) );

      thread_list waitingThreads;                                          // container for hold images from next image. One or more if file is multi image

      //Translation::DisableParallelProcessing();
      try //try 2
      {
         int running = 0;                                                   // running = Qty sub-images processing now = Qty CPU isActiv now.
         do
         {
            Module->ProcessEvents();                                       // Keep the GUI responsive
            if ( console.AbortRequested() )
               throw ProcessAborted();

            /*
             * Open File
             */
            if ( !t.IsEmpty() && waitingThreads.IsEmpty() )
            {
               size_type fileIndex = *t;                                      // take first index from begining of the list
               t.Remove( t.Begin() );                                      // remove the index from the list

               console.WriteLn( String().Format( "<br>File %u of %u", total - t.Length(), total ) );
               if ( p_targetFrames[fileIndex].enabled )
                  waitingThreads = LoadTargetFrame( fileIndex );           // put all sub-images from file to waitingThreads
               else
               {
                  ++skipped;
                  console.NoteLn( "* Skipping disabled target" );
               }
            }

            /*
             * Find idle or free CPU
             */
            thread_list::iterator i = nullptr;
            for ( thread_list::iterator j = runningThreads.Begin(); j != runningThreads.End(); ++j ) //Cycle in CPU units
            {
               if ( *j == nullptr )                                        // the CPU is free and empty.
               {
                  if ( !waitingThreads.IsEmpty() )                         // there are not processed images
                  {
                     i = j;
                     break;                                                // i pointed to CPU which is free now.
                  }
               }
               else                                                        // *j != 0 the CPU is non free and maybe idle or active
               {
                  if ( !(*j)->IsActive() )                                 // the CPU is idle = the CPU has finished processing
                  {
                     i = j;
                     break;                                                // i pointed to CPU thread which ready to save.
                  }
               }
            }

            if ( i == nullptr )                                            // all CPU IsActive or no new images
            {
               pcl::Sleep( 100 );
               continue;
            }

            /*
             * Write File
             */
            if ( *i != nullptr )                                           // the CPU is idle
            {
               --running;
               try
               {
                  console.WriteLn( String().Format( "<br>CPU#%u has finished processing. Saving file:", i - runningThreads.Begin() ) );
                  SaveImage( *i );
                  runningThreads.Delete( i );                              // prepare thread for next image. now (*i == nullptr) the CPU is free
               }
               catch ( ... )
               {
                  runningThreads.Delete( i );
                  throw;
               }
               ++succeeded;
            }                                                              // now (*i == nullptr) the CPU is free

            /*
             * Put image to empty runningThreads slot and Run()
             */
            if ( !waitingThreads.IsEmpty() )
            {
               *i = *waitingThreads;                                       // put one sub-image to runningThreads. so, now (*i != 0)
               waitingThreads.Remove( waitingThreads.Begin() );            // remove one sub-image from waitingThreads
               console.WriteLn( String().Format( "<br>CPU#%u processing file " , i - runningThreads.Begin() ) + (*i)->TargetPath() );
               (*i)->Start( ThreadPriority::DefaultMax, i - runningThreads.Begin() );
               ++running;
            }
         }
         while ( running > 0 || !t.IsEmpty() );
      } // try 2
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER;

         bool printed = false;
         for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            if ( *i != nullptr )
            {
               if ( !printed )
               {
                  console.Note( "<end><cbr><br>** Waiting for running tasks at CPU# " );
                  printed = true;
               }
               console.Note( String().Format( ":%u ", i - runningThreads.Begin() ) );
               console.Flush();
               (*i)->Wait();
            }
         console.NoteLn( "<br>* All running tasks are terminated." );
         runningThreads.Destroy();
         waitingThreads.Destroy();
      }

      console.NoteLn( String().Format( "<br>===== SplitCFA: %u succeeded, %u skipped, %u canceled =====",
                                       succeeded, skipped, total - succeeded ) );

      Exception::DisableConsoleOutput();
      Exception::EnableGUIOutput();

      return true;
   }
   catch ( ... )
   {
      Exception::DisableConsoleOutput();
      Exception::EnableGUIOutput();
      console.NoteLn( "<end><cbr><br>* SplitCFA terminated." );
      throw;
   }
}

// ----------------------------------------------------------------------------

void* SplitCFAInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheTargetFrameEnabledParameter )
      return &p_targetFrames[tableRow].enabled;
   if ( p == TheTargetFramePathParameter )
      return p_targetFrames[tableRow].path.Begin();
   if ( p == TheTargetFrameFolderParameter )
      return p_targetFrames[tableRow].folder.Begin();

   if ( p == TheOutputTreeParameter )
      return &p_outputTree;
   if ( p == TheOutputSubDirCFAParameter )
      return &p_outputSubDirCFA;
   if ( p == TheOutputDirParameter )
      return p_outputDir.Begin();
   if ( p == ThePrefixParameter )
      return p_prefix.Begin();
   if ( p == ThePostfixParameter )
      return p_postfix.Begin();
   if ( p == TheOverwriteParameter )
      return &p_overwrite;

   if ( p == TheSplitCFAOutputViewId0Parameter )
      return o_outputViewId[0].Begin();
   if ( p == TheSplitCFAOutputViewId1Parameter )
      return o_outputViewId[1].Begin();
   if ( p == TheSplitCFAOutputViewId2Parameter )
      return o_outputViewId[2].Begin();
   if ( p == TheSplitCFAOutputViewId3Parameter )
      return o_outputViewId[3].Begin();

   return nullptr;
}

bool SplitCFAInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheTargetFramesParameter )
   {
      p_targetFrames.Clear();
      if ( sizeOrLength > 0 )
         p_targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheTargetFramePathParameter )
   {
      p_targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_targetFrames[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheTargetFrameFolderParameter )
   {
      p_targetFrames[tableRow].folder.Clear();
      if ( sizeOrLength > 0 )
         p_targetFrames[tableRow].folder.SetLength( sizeOrLength );
   }
   else if ( p == TheOutputDirParameter )
   {
      p_outputDir.Clear();
      if ( sizeOrLength > 0 )
         p_outputDir.SetLength( sizeOrLength );
   }
   else if ( p == ThePrefixParameter )
   {
      p_prefix.Clear();
      if ( sizeOrLength > 0 )
         p_prefix.SetLength( sizeOrLength );
   }
   else if ( p == ThePostfixParameter )
   {
      p_postfix.Clear();
      if ( sizeOrLength > 0 )
         p_postfix.SetLength( sizeOrLength );
   }
   else if ( p == TheSplitCFAOutputViewId0Parameter )
   {
      o_outputViewId[0].Clear();
      if ( sizeOrLength > 0 )
         o_outputViewId[0].SetLength( sizeOrLength );
   }
   else if ( p == TheSplitCFAOutputViewId1Parameter )
   {
      o_outputViewId[1].Clear();
      if ( sizeOrLength > 0 )
         o_outputViewId[1].SetLength( sizeOrLength );
   }
   else if ( p == TheSplitCFAOutputViewId2Parameter )
   {
      o_outputViewId[2].Clear();
      if ( sizeOrLength > 0 )
         o_outputViewId[2].SetLength( sizeOrLength );
   }
   else if ( p == TheSplitCFAOutputViewId3Parameter )
   {
      o_outputViewId[3].Clear();
      if ( sizeOrLength > 0 )
         o_outputViewId[3].SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type SplitCFAInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheTargetFramesParameter )
      return p_targetFrames.Length();
   if ( p == TheTargetFramePathParameter )
      return p_targetFrames[tableRow].path.Length();
   if ( p == TheTargetFrameFolderParameter )
      return p_targetFrames[tableRow].folder.Length();
   if ( p == TheOutputDirParameter )
      return p_outputDir.Length();
   if ( p == ThePrefixParameter )
      return p_prefix.Length();
   if ( p == ThePostfixParameter )
      return p_postfix.Length();
   if ( p == TheSplitCFAOutputViewId0Parameter )
      return o_outputViewId[0].Length();
   if ( p == TheSplitCFAOutputViewId1Parameter )
      return o_outputViewId[1].Length();
   if ( p == TheSplitCFAOutputViewId2Parameter )
      return o_outputViewId[2].Length();
   if ( p == TheSplitCFAOutputViewId3Parameter )
      return o_outputViewId[3].Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SplitCFAInstance.cpp - Released 2017-04-14T23:07:12Z
