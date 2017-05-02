// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard SplitCFA Process Module Version 01.00.05.0037
// ****************************************************************************
// SplitCFAInstance.cpp - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2014 Nikolay Volkov
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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

#include "SplitCFAInstance.h"
#include "SplitCFAModule.h" // for ReadableVersion()

#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/ProcessInterface.h> // for ProcessEvents()
#include <pcl/StdStatus.h>
//#include <pcl/Translation.h>
#include <pcl/Version.h>


namespace pcl
{

SplitCFAInstance::SplitCFAInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_targetFrames   ( ),
p_outputTree     ( TheOutputTreeParameter->DefaultValue()),
p_outputSubDirCFA( TheOutputSubDirCFAParameter->DefaultValue()),
p_outputDir      ( TheOutputDirParameter->DefaultValue()),
p_overwrite      ( TheOverwriteParameter->DefaultValue()),
p_prefix         ( ThePrefixParameter->DefaultValue()   ),
p_postfix        ( ThePostfixParameter->DefaultValue()  )
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
   if ( x != 0 )
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
   if (view.Image().IsComplexSample())
   {
      whyNot = "SplitCFA cannot be executed on complex images.";
      return false;
   }
   if ( view.Image().Width()&1 | view.Image().Height()&1 ) // is odd
   {
      whyNot = "Wrong image dimension: odd width ["+String(view.Image().Width())+"] or height ["+String(view.Image().Height())+"].";
      return false;
   }
   whyNot.Clear();
   return true;
}

bool SplitCFAInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

inline void CopySTF(View& target, const View& source)
{
   View::stf_list F;
   source.GetScreenTransferFunctions(F);
   target.SetScreenTransferFunctions(F);
   target.EnableScreenTransferFunctions();
}

template <class P>
void SplitCFAInstance::SplitCFA_Engine( const GenericImage<P>& s,
                                        const SplitCFAInstance& instance, const View& sView )
{
   int sWidth = s.Width();
   int sHeight = s.Height();

   for ( int i = 0; i < 4; ++i )
   {
      ImageWindow tWindow( sWidth/2, sHeight/2,
                           s.NumberOfChannels(), P::BitsPerSample(), P::IsFloatSample(),
                           s.NumberOfChannels() > 1 ? true : false, // RGB : Mono
                           true, sView.Id() + instance.p_postfix + String( i ) );

      View tView = tWindow.MainView();
      ImageVariant tVariant = tView.GetImage();
      GenericImage<P>& t = static_cast<GenericImage<P>&>( *tVariant );

      for ( int c = 0; c < s.NumberOfChannels(); ++c )
         for ( int sY = 0, tY = 0; sY < sHeight; sY+=2, ++tY )
            for ( int sX = 0, tX = 0; sX < sWidth; sX+=2, ++tX )
               switch( i )
               {
               case 0: t( tX, tY, c ) = s( sX,   sY,   c); break;
               case 1: t( tX, tY, c ) = s( sX,   sY+1, c); break;
               case 2: t( tX, tY, c ) = s( sX+1, sY,   c); break;
               case 3: t( tX, tY, c ) = s( sX+1, sY+1, c); break;
               }
      CopySTF( tView, sView ); //transfet STF from source to target.
      tWindow.Show();
   }
}

bool SplitCFAInstance::ExecuteOn(View& view)
{
   try
   {
      view.LockForWrite();

      ImageVariant image;
      image = view.Image();

      if (!image.IsComplexSample())
      {
         if (image.IsFloatSample())
            switch (image.BitsPerSample())
            {
            case 32: SplitCFA_Engine( static_cast<const Image&>( *image ), *this, view); break;
            case 64: SplitCFA_Engine( static_cast<const DImage&>( *image ), *this, view); break;
            }
         else
            switch (image.BitsPerSample())
            {
            case  8: SplitCFA_Engine( static_cast<const UInt8Image&>( *image ), *this, view); break;
            case 16: SplitCFA_Engine( static_cast<const UInt16Image&>( *image ), *this, view); break;
            case 32: SplitCFA_Engine( static_cast<const UInt32Image&>( *image ), *this, view); break;
            }
      }
      view.Unlock();
      return true;
   }

   catch (...)
   {
      view.Unlock(); // Never leave a view locked!
      throw;
   }
}

// ----------------------------------------------------------------------------

bool SplitCFAInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_targetFrames.IsEmpty() )
      whyNot = "No target frames have been specified.";

   else if ( !p_outputDir.IsEmpty() && !File::DirectoryExists( p_outputDir ) )
      whyNot = "The specified output directory does not exist: " + p_outputDir;

   else
   {
      for ( image_list::const_iterator i = p_targetFrames.Begin(); i != p_targetFrames.End(); ++i )
         if ( i->enabled && !File::Exists( i->path ) )
         {
            whyNot = "File not found: " + i->path;
            return false;
         }
      whyNot.Clear(); return true;
   }
   return false;
}


// ----------------------------------------------------------------------------
/*static void ImageVariant2ImageWindow( ImageVariant* v )
{
   if ( v == 0 )
   {
      Console().WriteLn( "Wrong target pointer" );
      return;
   }
   ImageWindow w( v->Width(), v->Height(), v->NumberOfChannels(), v->BitsPerSample(), v->IsFloatSample(), v->IsColor(), true, "debug" );
   w.MainView().Image().CopyImage( *v );
   w.Show();
}
*/
// ----------------------------------------------------------------------------

template <class P1>
void Engine( GenericImage<P1>& t, GenericImage<P1>& s, int i) //Engine for ExecuteGlobal
{
   int sWidth = s.Width();
   int sHeight = s.Height();
   for ( int c = 0; c < s.NumberOfChannels(); ++c )
      for ( int sY = 0, tY = 0; sY < sHeight; sY+=2, ++tY )
         for ( int sX = 0, tX = 0; sX < sWidth; sX+=2, ++tX )
            switch (i) //CFA patern index
            {
            case 0: t( tX, tY, c ) = s( sX,   sY,   c); break;
            case 1: t( tX, tY, c ) = s( sX,   sY+1, c); break;
            case 2: t( tX, tY, c ) = s( sX+1, sY,   c); break;
            case 3: t( tX, tY, c ) = s( sX+1, sY+1, c); break;
            }
}

inline static void Engine( ImageVariant& t, ImageVariant& s, int i)
{
   if ( s.IsFloatSample() )   switch ( s.BitsPerSample() )
   {
      case 32 : Engine( static_cast<Image&>( *t ), static_cast<Image&>( *s ), i); break;
      case 64 : Engine( static_cast<DImage&>( *t ), static_cast<DImage&>( *s ), i); break;
   }
   else switch ( s.BitsPerSample() )
   {
      case  8 : Engine( static_cast<UInt8Image&>( *t ), static_cast<UInt8Image&>( *s ), i); break;
      case 16 : Engine( static_cast<UInt16Image&>( *t ), static_cast<UInt16Image&>( *s ), i); break;
      case 32 : Engine( static_cast<UInt32Image&>( *t ), static_cast<UInt32Image&>( *s ), i); break;
   }
}

// ----------------------------------------------------------------------------
struct FileData
{
   FileFormat*      format;   // the file format of retrieved data
   const void*      fsData;   // format-specific data
   ImageOptions     options;  // currently used for resolution only
   FITSKeywordArray keywords; // FITS keywords
   ICCProfile       profile;  // ICC profile
   ByteArray        metadata; // XML metadata

   FileData() :
   format( 0 ), fsData( 0 ), options(), keywords(), profile(), metadata()
   {
   }

   FileData( FileFormatInstance& file, const ImageOptions& o ) :
   format( 0 ), fsData( 0 ), options( o ), keywords(), profile(), metadata()
   {
      format = new FileFormat( file.Format() );

      if ( format->UsesFormatSpecificData() )
         fsData = file.FormatSpecificData();

      if ( format->CanStoreKeywords() )
         file.Extract( keywords );

      if ( format->CanStoreICCProfiles() )
         file.Extract( profile );

      if ( format->CanStoreMetadata() )
      {
         void* p = 0;
         size_t n = 0;
         file.Extract( p, n );
         if ( p != 0 )
         {
            metadata = ByteArray( ByteArray::iterator( p ), ByteArray::iterator( p )+n );
            delete (uint8*)p;
         }
      }
   }

   ~FileData()
   {
      if ( format != 0 )
      {
         if ( fsData != 0 )
            format->DisposeFormatSpecificData( const_cast<void*>( fsData ) ), fsData = 0;
         delete format, format = 0;
      }
   }
};

// ----------------------------------------------------------------------------
class SplitCFAThread : public Thread
{

public:

   SplitCFAThread(ImageVariant* s, FileData* fd, const String& tp, const String& tf, int i) :
   t(), source( s ), fileData( fd ), targetPath( tp ), targetFolder( tf ), subimageIndex( i )
   {
   }

   virtual ~SplitCFAThread()
   {
      if( !t.IsEmpty() )
         for( int i = 3; i >= 0; --i )
            if ( t[i] != 0 )
               delete t[i], t[i] = 0;

      if ( fileData != 0 )
         delete fileData, fileData = 0;

      if ( source != 0 )
         delete source, source = 0; //->~ImageVariant();
   }

   virtual void Run()
   {
      try
      {
         for(int i = 0; i < 4; ++i) //CFA patern index
         {
            ImageVariant* img = new ImageVariant();
            img->CreateImageAs(*source);
            img->AllocateData(source->Width()/2, source->Height()/2, source->NumberOfChannels(), source->ColorSpace());

            Engine( *img, *source, i);
            t.Add(img);
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

   const ImageVariant* TargetImage(int i) const
   {
      return t[i];
   }

   String TargetPath() const
   {
      return targetPath;
   }

   String TargetFolder() const
   {
      return targetFolder;
   }

   const FileData& GetFileData() const
   {
      return *fileData;
   }

   int SubimageIndex() const
   {
      return subimageIndex;
   }

private:

   Array<ImageVariant*> t;             //Target planes
   ImageVariant*        source;
   FileData*            fileData;      // Target image parameters and embedded data. It belongs to this thread.
   String               targetPath;    // File path of this target image
   String               targetFolder;  // File sub-folder in output dir for this target image
   int                  subimageIndex; // > 0 in case of a multiple image; = 0 otherwise
};

// ----------------------------------------------------------------------------
template <class P>
static void LoadImageFile( GenericImage<P>& image, FileFormatInstance& file )
{
   if ( !file.ReadImage( image ) )
      throw CatchedException();
}

static void LoadImageFile( ImageVariant& v, FileFormatInstance& file, const ImageDescriptionArray& images, int index )
{
   if ( !file.SelectImage( index ) )
      throw CatchedException();

   v.CreateSharedImage( images[index].options.ieeefpSampleFormat, false, images[index].options.bitsPerSample );

   if ( v.IsFloatSample() ) switch ( v.BitsPerSample() )
   {
      case 32 : LoadImageFile( static_cast<Image&>( *v ), file ); break;
      case 64 : LoadImageFile( static_cast<DImage&>( *v ), file ); break;
   }
   else switch ( v.BitsPerSample() )
   {
      case  8 : LoadImageFile( static_cast<UInt8Image&>( *v ), file ); break;
      case 16 : LoadImageFile( static_cast<UInt16Image&>( *v ), file ); break;
      case 32 : LoadImageFile( static_cast<UInt32Image&>( *v ), file ); break;
   }
}

inline thread_list SplitCFAInstance::LoadTargetFrame( const size_t fileIndex )
{
   Console console;
   const ImageItem& item = p_targetFrames[fileIndex];

   const String filePath = item.path;

   console.WriteLn( "Open " + filePath );

   FileFormat format( File::ExtractExtension( filePath ), true, false );
   FileFormatInstance file( format );

   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, "raw cfa" ) ) throw CatchedException(); // ### Use input hints to load raw CFA DSLR images
   if ( images.IsEmpty() ) throw Error( filePath + ": Empty image file." );

   thread_list threads;
   ImageVariant* source = new ImageVariant();

   try
   {
      for ( unsigned int index = 0; index < images.Length(); ++index )
      {
         if ( images[index].info.height&1 | images[index].info.width&1 ) // height is odd
         {
            Console().WriteLn( "Wrong image dimension: odd width ["+String(images[index].info.width)+"] or height ["+String(images[index].info.height)+"]." );
            continue;
         }

         if ( images.Length() > 1 ) console.WriteLn( String().Format( "* Subimage %u of %u", index+1, images.Length() ) );
         if ( !file.SelectImage( index ) ) throw CatchedException();
         LoadImageFile( *source, file, images, index );
         ProcessInterface::ProcessEvents();
         FileData* inputData = new FileData( file, images[index].options );


         threads.Add( new SplitCFAThread(source, inputData, filePath, item.folder, index) );
      }
      console.WriteLn( "Close " + filePath );
      file.Close();
      return threads;
   }
   catch ( ... )
   {
      if ( source != 0 ) delete source;
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
      case 32 : SaveImageFile( static_cast<const Image&>( *image ), file ); break;
      case 64 : SaveImageFile( static_cast<const DImage&>( *image ), file ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8 : SaveImageFile( static_cast<const UInt8Image&>( *image ), file ); break;
      case 16 : SaveImageFile( static_cast<const UInt16Image&>( *image ), file ); break;
      case 32 : SaveImageFile( static_cast<const UInt32Image&>( *image ), file ); break;
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

inline String SplitCFAInstance::OutputFilePath( const String& filePath, const String& fileFolder, const size_t index, const int cfaIndex )
{
   String dir = p_outputDir;
   dir.Trim();
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
         dir.Append(fileFolder);
      }
   }
   if ( dir.IsEmpty() )
      throw Error( dir + ": Unable to determine an output p_outputDir." );

   if ( p_outputSubDirCFA )
   {
      if ( !dir.EndsWith( '/' ) )
         dir.Append( '/' );
      dir.Append( "CFA" );
      dir.Append( String(cfaIndex) );
   }
   if ( !File::DirectoryExists(dir) ) // if dir not exist
      File::CreateDirectory(dir,true); //cleate folder with intermediate folders

   if ( !dir.EndsWith( '/' ) )
      dir.Append( '/' );

   String fileName = File::ExtractName( filePath );
   fileName.Trim();
   if ( !p_prefix.IsEmpty()   ) fileName.Prepend( p_prefix );
   if ( index > 0            ) fileName.Append( String().Format( "_%02d", index+1 ) );
   if ( !p_postfix.IsEmpty()   ) fileName.Append( p_postfix );
   if ( fileName.IsEmpty()   ) throw Error( filePath + ": Unable to determine an output file name." );
   fileName.Append(String(cfaIndex)); // add CFA patern index

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
   for(int i = 0; i < 4; ++i)
   {

   String outputFilePath = OutputFilePath( t->TargetPath(), t->TargetFolder(), t->SubimageIndex(), i );
   console.WriteLn( "Create " + outputFilePath );

   FileFormat outputFormat( outputExtension, false, true );
   FileFormatInstance outputFile( outputFormat );
   if ( !outputFile.Create( outputFilePath ) ) throw CatchedException();

   const FileData& data = t->GetFileData();

   ImageOptions options = data.options; // get resolution, etc.
   outputFile.SetOptions( options );

   if ( data.fsData != 0 )
      if ( outputFormat.ValidateFormatSpecificData( data.fsData ) ) outputFile.SetFormatSpecificData( data.fsData );

   /*
   * Add FITS header keywords and preserve existing ones, if possible.
   * NB: A COMMENT or HISTORY keyword cannot have a value; these keywords have
   * only the name and comment components.
   */
   if ( outputFormat.CanStoreKeywords() )
   {
      FITSKeywordArray keywords = data.keywords;
      keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "SplitCFA with " + PixInsightVersion::AsString() ) );
      keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "SplitCFA with " + SplitCFAModule::ReadableVersion() ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "SplitCFA" ) );
      outputFile.Embed( keywords );
   }
   else if ( !data.keywords.IsEmpty() )
         console.WarningLn( "** Warning: The output format cannot store FITS keywords - original keywords not embedded." );

   if ( data.profile.IsProfile() )
      if ( outputFormat.CanStoreICCProfiles() ) outputFile.Embed( data.profile );
      else console.WarningLn( "** Warning: The output format cannot store ICC profiles - original profile not embedded." );

   if ( !data.metadata.IsEmpty() )
      if ( outputFormat.CanStoreMetadata() ) outputFile.Embed( data.metadata.Begin(), data.metadata.Length() );
      else console.WarningLn( "** Warning: The output format cannot store metadata - original metadata not embedded." );

   SaveImageFile( *t->TargetImage(i), outputFile );


   console.WriteLn( "Close file." );
   outputFile.Close();
   }
}

bool SplitCFAInstance::ExecuteGlobal()
{
   Console console;
   console.Show();

   String why;
   if ( !CanExecuteGlobal( why ) ) throw Error( why );

   try //try 1
   {
      Exception::EnableConsoleOutput();
      Exception::DisableGUIOutput();
      console.EnableAbort();

      size_t succeeded = 0;
      size_t skipped = 0;
      const size_t total = p_targetFrames.Length();
      console.WriteLn( String().Format( "<br>Processing %u target frames:", total ) );

      Array<size_t> t;
      for ( size_t i = 0; i < total; t.Add( i++ ) );                        // Array with file indexes

      const int totalCPU = Thread::NumberOfThreads( 1024, 1 );
      Console().Write( String().Format("Detected %u CPU. ", totalCPU ) );

      const size_t n = Min( size_t( totalCPU ), total );
      thread_list runningThreads( n );                                       // n = how many threads will run simultaneously
      console.WriteLn( String().Format( "Using %u worker threads", runningThreads.Length() ) );

      thread_list waitingThreads;                                          //container for hold images from next image. One or more if file is multi image

      //Translation::DisableParallelProcessing();
      try //try 2
      {
         int runing = 0;                                                   // runing = Qty sub-images processing now = Qty CPU isActiv now.
         do
         {
            ProcessInterface::ProcessEvents();                              // Keep the GUI responsive
            if ( console.AbortRequested() ) throw ProcessAborted();

            // ------------------------------------------------------------
            // Open File
            if ( !t.IsEmpty() && waitingThreads.IsEmpty() )
            {
               size_t fileIndex = *t;                                       // take first index from begining of the list
               t.Remove( t.Begin() );                                       // remove the index from the list

               console.WriteLn( String().Format( "<br>File %u of %u", total - t.Length(), total ) );
               if ( p_targetFrames[fileIndex].enabled )
                  waitingThreads = LoadTargetFrame( fileIndex );            // put all sub-images from file to waitingThreads
               else
               {
                  ++skipped; console.NoteLn( "* Skipping disabled target" );
               }
            }

            // ------------------------------------------------------------
            // Find idle or free CPU
            thread_list::iterator i = 0;
            for ( thread_list::iterator j = runningThreads.Begin(); j != runningThreads.End(); ++j ) //Cycle in CPU units
            {
               if ( *j == 0 )                                               // the CPU is free and empty.
               {
                  if ( !waitingThreads.IsEmpty() )                          // there are not processed images
                  {
                     i = j; break;                                          // i pointed to CPU which is free now.
                  }
               }
                                                                            // *j != 0 the CPU is non free and maybe idle or active
               else if ( !(*j)->IsActive() )                                // the CPU is idle = the CPU has finished processing
               {
                  i = j; break;                                             // i pointed to CPU thread which ready to save.
               }
            }

            if ( i == 0 )                                                   // all CPU IsActive or no new images
            {
               pcl::Sleep( 100 );
               continue;
            }

            // ------------------------------------------------------------
            // Write File
            if ( *i != 0 )                                                   //the CPU is idle
            {
               runing--;
               try
               {
                  console.WriteLn( String().Format( "<br>CPU#%u has finished processing. Saving file:", i - runningThreads.Begin() ));
                  SaveImage( *i );
                  runningThreads.Delete( i );                                //prepare thread for next image. now (*i == 0) the CPU is free
               }
               catch ( ... )
               {
                  runningThreads.Delete( i );
                  throw;
               }
               ++succeeded;
            }                                                                 //now (*i == 0) the CPU is free

            // ------------------------------------------------------------
            // Put image to empty runningThreads slot and Run()
            if ( !waitingThreads.IsEmpty() )
            {
               *i = *waitingThreads;                                          //put one sub-image to runningThreads. so, now (*i != 0)
               waitingThreads.Remove( waitingThreads.Begin() );               //remove one sub-image from waitingThreads
               console.WriteLn( String().Format( "<br>CPU#%u processing file " , i - runningThreads.Begin() ) + (*i)->TargetPath() );
               (*i)->Start( ThreadPriority::DefaultMax, i - runningThreads.Begin() );
               runing++;
            }
         }
         while ( runing > 0 || !t.IsEmpty() );
      } // try 2
      catch ( ... )
      {
         try {   throw; }   ERROR_HANDLER;

         bool printed = false;
         for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
            if ( *i != 0 )
            {
               if ( !printed )
               {
                  console.Note( "<br>** Waiting for running tasks at CPU# " );
                  printed = true;
               }
               console.Note( String().Format( ":%u ", i - runningThreads.Begin() ));
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
   if ( p == TheTargetFrameEnabledParameter )   return &p_targetFrames[tableRow].enabled;
   if ( p == TheTargetFramePathParameter )      return p_targetFrames[tableRow].path.c_str();
   if ( p == TheTargetFrameFolderParameter )    return p_targetFrames[tableRow].folder.c_str();

   if ( p == TheOutputTreeParameter )           return &p_outputTree;
   if ( p == TheOutputSubDirCFAParameter )      return &p_outputSubDirCFA;
   if ( p == TheOutputDirParameter )            return p_outputDir.c_str();
   if ( p == ThePrefixParameter )               return p_prefix.c_str();
   if ( p == ThePostfixParameter )              return p_postfix.c_str();
   if ( p == TheOverwriteParameter )            return &p_overwrite;

   return 0;
}

bool SplitCFAInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheTargetFramesParameter )
   {
      p_targetFrames.Clear();
      if ( sizeOrLength > 0 ) p_targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheTargetFramePathParameter )
   {
      p_targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 ) p_targetFrames[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheTargetFrameFolderParameter )
   {
      p_targetFrames[tableRow].folder.Clear();
      if ( sizeOrLength > 0 ) p_targetFrames[tableRow].folder.SetLength( sizeOrLength );
   }
   else if ( p == TheOutputDirParameter )
   {
      p_outputDir.Clear();
      if ( sizeOrLength > 0 ) p_outputDir.SetLength( sizeOrLength );
   }
   else if ( p == ThePrefixParameter )
   {
      p_prefix.Clear();
      if ( sizeOrLength > 0 ) p_prefix.SetLength( sizeOrLength );
   }
   else if ( p == ThePostfixParameter )
   {
      p_postfix.Clear();
      if ( sizeOrLength > 0 ) p_postfix.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type SplitCFAInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheTargetFramesParameter )      return p_targetFrames.Length();
   if ( p == TheTargetFramePathParameter )   return p_targetFrames[tableRow].path.Length();
   if ( p == TheTargetFrameFolderParameter ) return p_targetFrames[tableRow].folder.Length();
   if ( p == TheOutputDirParameter )         return p_outputDir.Length();
   if ( p == ThePrefixParameter )            return p_prefix.Length();
   if ( p == ThePostfixParameter )           return p_postfix.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF SplitCFAInstance.cpp - Released 2014/11/14 17:19:24 UTC
