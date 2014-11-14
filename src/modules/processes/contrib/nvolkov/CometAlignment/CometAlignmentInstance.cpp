// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard CometAlignment Process Module Version 01.00.06.0059
// ****************************************************************************
// CometAlignmentInstance.cpp - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2014 Nikolay Volkov
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

#include "CometAlignmentInstance.h"
#include "CometAlignmentModule.h" // for ReadableVersion()

#include <pcl/FileFormat.h>
#include <pcl/ProcessInterface.h> // for ProcessEvents()
#include <pcl/StdStatus.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Version.h>
#include <pcl/Translation.h>


namespace pcl
{

static PixelInterpolation* pixelInterpolation = 0;

CometAlignmentInstance::CometAlignmentInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_targetFrames   ( ),
p_outputDir      ( TheOutputDir->DefaultValue()),
p_overwrite      ( TheOverwrite->DefaultValue()),
p_prefix         ( ThePrefix->DefaultValue()   ),
p_postfix        ( ThePostfix->DefaultValue()  ),
p_reference      ( TheReference->DefaultValue()),
p_pixelInterpolation( ThePixelInterpolationParameter->DefaultValueIndex() ),
p_linearClampingThreshold( TheLinearClampingThresholdParameter->DefaultValue() )
{
}

CometAlignmentInstance::CometAlignmentInstance( const CometAlignmentInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void CometAlignmentInstance::Assign( const ProcessImplementation& p )
{
   const CometAlignmentInstance* x = dynamic_cast<const CometAlignmentInstance*>( &p );
   if ( x != 0 )
   {
      p_targetFrames   = x->p_targetFrames;
      p_outputDir      = x->p_outputDir;
      p_overwrite      = x->p_overwrite;
      p_prefix         = x->p_prefix;
      p_postfix        = x->p_postfix;
      p_reference      = x->p_reference;
      p_pixelInterpolation = x->p_pixelInterpolation;
      p_linearClampingThreshold = x->p_linearClampingThreshold;
   }
}

bool CometAlignmentInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "CometAlignment can only be executed in the global context.";
   return false;
}

bool CometAlignmentInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

bool CometAlignmentInstance::CanExecuteGlobal( String& whyNot ) const
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
/*
// ----------------------------------------------------------------------------
static void ImageVariant2ImageWindow( ImageVariant* v )
{
   if ( v == 0 )
   {
      Console().WriteLn( "Wrong target pointer" );
      return;
   }
   ImageWindow w( v->AnyImage()->Width(), v->AnyImage()->Height(), v->AnyImage()->NumberOfChannels(), v->BitsPerSample(), v->IsFloatSample(), v->AnyImage()->IsColor(), true, "debug" );
   w.MainView().Image().CopyImage( *v );
   w.Show();
}
 */
// ----------------------------------------------------------------------------

template <class P1>
void Engine( GenericImage<P1>& target, const DPoint& delta )
{
   Translation T( *pixelInterpolation , delta );
   T.DisableParallelProcessing();
   T >> target;
}

inline static void Engine( ImageVariant& target, const DPoint& delta )
{
   if ( target.IsFloatSample() )   switch ( target.BitsPerSample() )
   {
      case 32 : Engine( *static_cast<Image*>( target.AnyImage() ), delta ); break;
      case 64 : Engine( *static_cast<DImage*>( target.AnyImage() ), delta ); break;
   }
   else switch ( target.BitsPerSample() )
   {
      case  8 : Engine( *static_cast<UInt8Image*>( target.AnyImage() ), delta ); break;
      case 16 : Engine( *static_cast<UInt16Image*>( target.AnyImage() ), delta ); break;
      case 32 : Engine( *static_cast<UInt32Image*>( target.AnyImage() ), delta ); break;
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
class CAThread : public Thread
{

public:

   CAThread( ImageVariant* t, FileData* fd, const String& tp, int i, /*const CAThreadData& d,*/ const DPoint d ) :
   target( t ), fileData( fd ), targetPath( tp ), subimageIndex( i ), /*data( d ),*/ delta( d )
   {
   }

   virtual ~CAThread()
   {
      if ( target != 0 )
         delete target, target = 0;

      if ( fileData != 0 )
         delete fileData, fileData = 0;
   }

   virtual void Run()
   {
      try
      {
         Engine( *target, delta );
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

   const ImageVariant* TargetImage() const
   {
      return target;
   }

   String TargetPath() const
   {
      return targetPath;
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

   ImageVariant* target;
   FileData*   fileData;       // Target image parameters and embedded data. It belongs to this thread.
   String      targetPath;     // File path of this target image
   int         subimageIndex;  // > 0 in case of a multiple image; = 0 otherwise
   DPoint      delta;          // Delta x, y
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
      case 32 : LoadImageFile( *static_cast<Image*>( v.AnyImage() ), file ); break;
      case 64 : LoadImageFile( *static_cast<DImage*>( v.AnyImage() ), file ); break;
   }
   else switch ( v.BitsPerSample() )
   {
      case  8 : LoadImageFile( *static_cast<UInt8Image*>( v.AnyImage() ), file ); break;
      case 16 : LoadImageFile( *static_cast<UInt16Image*>( v.AnyImage() ), file ); break;
      case 32 : LoadImageFile( *static_cast<UInt32Image*>( v.AnyImage() ), file ); break;
   }
}

inline thread_list CometAlignmentInstance::LoadTargetFrame( const size_t fileIndex )
{
   Console console;
   const ImageItem& item = p_targetFrames[fileIndex];
   const ImageItem& r = p_targetFrames[p_reference];
   const DPoint delta( item.x - r.x , item.y - r.y );

   const String filePath = item.path;
   console.WriteLn( "Open " + filePath );

   FileFormat format( File::ExtractExtension( filePath ), true, false );
   FileFormatInstance file( format );

   ImageDescriptionArray images;
   if ( !file.Open( images, filePath ) ) throw CatchedException();
   if ( images.IsEmpty() ) throw Error( filePath + ": Empty image file." );

   thread_list threads;
   ImageVariant* target = new ImageVariant();
   try
   {
      for ( unsigned int index = 0; index < images.Length(); ++index )
      {
         if ( images.Length() > 1 ) console.WriteLn( String().Format( "* Subimage %u of %u", index+1, images.Length() ) );
         if ( !file.SelectImage( index ) ) throw CatchedException();
         LoadImageFile( *target, file, images, index );
         ProcessInterface::ProcessEvents();
         FileData* inputData = new FileData( file, images[index].options );
         threads.Add( new CAThread( target, inputData, filePath, index, delta ) );
      }
      console.WriteLn( "Close " + filePath );
      file.Close();
      console.WriteLn( String().Format( "Delta x:%f, y:%f", delta.x, delta.y ) );
      return threads;
   }
   catch ( ... )
   {
      if ( target != 0 ) delete target;
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
      case 32 : SaveImageFile( *static_cast<const Image*>( image.AnyImage() ), file ); break;
      case 64 : SaveImageFile( *static_cast<const DImage*>( image.AnyImage() ), file ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8 : SaveImageFile( *static_cast<const UInt8Image*>( image.AnyImage() ), file ); break;
      case 16 : SaveImageFile( *static_cast<const UInt16Image*>( image.AnyImage() ), file ); break;
      case 32 : SaveImageFile( *static_cast<const UInt32Image*>( image.AnyImage() ), file ); break;
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

inline String CometAlignmentInstance::OutputFilePath( const String& filePath, const size_t index )
{
   String dir = p_outputDir;
   dir.Trim();
   if ( dir.IsEmpty() )
      dir = File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath );
   if ( dir.IsEmpty() )
      throw Error( dir + ": Unable to determine an output p_outputDir." );
   if ( !dir.EndsWith( '/' ) )
      dir.Append( '/' );

   String fileName = File::ExtractName( filePath );
   fileName.Trim();
   if ( !p_prefix.IsEmpty()   ) fileName.Prepend( p_prefix );
   if ( index > 0            ) fileName.Append( String().Format( "_%02d", index+1 ) );
   if ( !p_postfix.IsEmpty()   ) fileName.Append( p_postfix );
   if ( fileName.IsEmpty()   ) throw Error( filePath + ": Unable to determine an output file name." );

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

void CometAlignmentInstance::SaveImage( const CAThread* t )
{
   Console console;
   String outputFilePath = OutputFilePath( t->TargetPath(), t->SubimageIndex() );
   console.WriteLn( "Create " + outputFilePath );

   FileFormat outputFormat( outputExtension, false, true );
   FileFormatInstance outputFile( outputFormat );
   if ( !outputFile.Create( outputFilePath ) ) throw CatchedException();

   const FileData& data = t->GetFileData();

   ImageOptions options = data.options; // get resolution, etc.
   //options.bitsPerSample = bitsPerSample;
   //options.ieeefpSampleFormat = floatSample;
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
      keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "CometAlignment with " + PixInsightVersion::AsString() ) );
      keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "CometAlignment with " + CometAlignmentModule::ReadableVersion() ) );
      keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "CometAlignment" ) );
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

   SaveImageFile( *t->TargetImage(), outputFile );

   console.WriteLn( "Close file." );
   outputFile.Close();
}

inline void CometAlignmentInstance::InitPixelInterpolation()
{
   Console().WriteLn( "PixelInterpolation: " + ThePixelInterpolationParameter->ElementId( p_pixelInterpolation ) );
   if ( p_pixelInterpolation == CAPixelInterpolation::BicubicSpline ) Console().WriteLn( "ClampingThreshold : " + String( p_linearClampingThreshold ) );

   switch ( p_pixelInterpolation )
   {
   case CAPixelInterpolation::NearestNeighbor:
      pixelInterpolation = new NearestNeighborPixelInterpolation;
      break;
   case CAPixelInterpolation::Bilinear:
      pixelInterpolation = new BilinearPixelInterpolation;
      break;
   default:
   case CAPixelInterpolation::BicubicSpline:
      pixelInterpolation = new BicubicSplinePixelInterpolation( p_linearClampingThreshold );
      break;
   case CAPixelInterpolation::BicubicBSpline:
      pixelInterpolation = new BicubicBSplinePixelInterpolation;
      break;
   case CAPixelInterpolation::Lanczos3:
   case CAPixelInterpolation::Lanczos4:
   case CAPixelInterpolation::Lanczos5:
      {
         // ### FIXME: Use LUT Lanczos interpolations for 8-bit and 16-bit images

         // Disable clamping when clampingThreshold == 1
         float clamp = (p_linearClampingThreshold < 1) ? p_linearClampingThreshold : -1;
         switch ( p_pixelInterpolation )
         {
         default:
         case CAPixelInterpolation::Lanczos3:
            pixelInterpolation = new LanczosPixelInterpolation( 3, clamp );
            break;
         case CAPixelInterpolation::Lanczos4:
            pixelInterpolation = new LanczosPixelInterpolation( 4, clamp );
            break;
         case CAPixelInterpolation::Lanczos5:
            pixelInterpolation = new LanczosPixelInterpolation( 5, clamp );
            break;
         }
      }
      break;
   case CAPixelInterpolation::MitchellNetravaliFilter:
      pixelInterpolation = new BicubicFilterPixelInterpolation( 2, 2, MitchellNetravaliCubicFilter() );
      break;
   case CAPixelInterpolation::CatmullRomSplineFilter:
      pixelInterpolation = new BicubicFilterPixelInterpolation( 2, 2, CatmullRomSplineFilter() );
      break;
   case CAPixelInterpolation::CubicBSplineFilter:
      pixelInterpolation = new BicubicFilterPixelInterpolation( 2, 2, CubicBSplineFilter() );
      break;
   }
}

// ----------------------------------------------------------------------------
bool CometAlignmentInstance::ExecuteGlobal()
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

      InitPixelInterpolation();

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
               pcl::Sleep( 0.1 );
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
               *i = *waitingThreads;                                       //put one sub-image to runningThreads. so, now (*i != 0)
               waitingThreads.Remove( waitingThreads.Begin() );            //remove one sub-image from waitingThreads
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
                  console.Note( "<br>* Waiting for running tasks at CPU# " );
                  printed = true;
               }
               console.Write( String().Format( ":%u ", i - runningThreads.Begin() ));
               console.Flush();
               (*i)->Wait();
            }
         console.NoteLn( "<br>* All running tasks are terminated." );
         runningThreads.Destroy();
         waitingThreads.Destroy();
      }
      //Translation::EnableParallelProcessing();

      console.NoteLn( String().Format( "<br>===== CometAlignment: %u succeeded, %u skipped, %u canceled =====",
                                       succeeded, skipped, total - succeeded ) );

      Exception::DisableConsoleOutput();
      Exception::EnableGUIOutput();

      return true;
   }
   catch ( ... )
   {
      //AbstractImage::EnableParallelProcessing();
      Exception::DisableConsoleOutput();
      Exception::EnableGUIOutput();

      console.NoteLn( "<end><cbr><br>* CometAlignment terminated." );
      throw;
   }
}

// ----------------------------------------------------------------------------
void* CometAlignmentInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheTargetFrameEnabled )   return &p_targetFrames[tableRow].enabled;
   if ( p == TheTargetFramePath )      return p_targetFrames[tableRow].path.c_str();
   if ( p == TheTargetFrameDate )      return p_targetFrames[tableRow].date.c_str();
   if ( p == TheTargetFrameJDate )     return &p_targetFrames[tableRow].Jdate;
   if ( p == TheTargetFrameX )         return &p_targetFrames[tableRow].x;
   if ( p == TheTargetFrameY )         return &p_targetFrames[tableRow].y;

   if ( p == TheOutputDir )            return p_outputDir.c_str();
   if ( p == ThePrefix )               return p_prefix.c_str();
   if ( p == ThePostfix )              return p_postfix.c_str();
   if ( p == TheOverwrite )            return &p_overwrite;

   if ( p == TheReference )            return &p_reference;
   if ( p == TheLinearClampingThresholdParameter ) return &p_linearClampingThreshold;
   if ( p == ThePixelInterpolationParameter ) return &p_pixelInterpolation;

   return 0;
}

bool CometAlignmentInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheTargetFrames )
   {
      p_targetFrames.Clear();
      if ( sizeOrLength > 0 ) p_targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheTargetFramePath )
   {
      p_targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 ) p_targetFrames[tableRow].path.Reserve( sizeOrLength );
   }
   else if ( p == TheTargetFrameDate )
   {
      p_targetFrames[tableRow].date.Clear();
      if ( sizeOrLength > 0 ) p_targetFrames[tableRow].date.Reserve( sizeOrLength );
   }
   else if ( p == TheOutputDir )
   {
      p_outputDir.Clear();
      if ( sizeOrLength > 0 ) p_outputDir.Reserve( sizeOrLength );
   }
   else if ( p == ThePrefix )
   {
      p_prefix.Clear();
      if ( sizeOrLength > 0 ) p_prefix.Reserve( sizeOrLength );
   }
   else if ( p == ThePostfix )
   {
      p_postfix.Clear();
      if ( sizeOrLength > 0 ) p_postfix.Reserve( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type CometAlignmentInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheTargetFrames )      return p_targetFrames.Length();
   if ( p == TheTargetFramePath )   return p_targetFrames[tableRow].path.Length();
   if ( p == TheTargetFrameDate )   return p_targetFrames[tableRow].date.Length();
   if ( p == TheOutputDir )         return p_outputDir.Length();
   if ( p == ThePrefix )            return p_prefix.Length();
   if ( p == ThePostfix )           return p_postfix.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CometAlignmentInstance.cpp - Released 2014/11/14 17:19:24 UTC
