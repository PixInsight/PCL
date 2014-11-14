// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard ImageIntegration Process Module Version 01.09.04.0253
// ****************************************************************************
// HDRCompositionInstance.cpp - Released 2014/11/14 17:19:21 UTC
// ****************************************************************************
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "HDRCompositionInstance.h"

#include <pcl/Convolution.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FFTConvolution.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/GaussianFilter.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/MorphologicalOperator.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/StructuringElement.h>
#include <pcl/View.h>

#define QUANTIZATION_LEVELS   15000000

namespace pcl
{

// ----------------------------------------------------------------------------

HDRCompositionInstance::HDRCompositionInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
images(),
inputHints(),
maskBinarizingThreshold( TheHCMaskBinarizingThresholdParameter->DefaultValue() ),
maskSmoothness( TheHCMaskSmoothnessParameter->DefaultValue() ),
maskGrowth( TheHCMaskGrowthParameter->DefaultValue() ),
autoExposures( TheHCAutoExposuresParameter->DefaultValue() ),
rejectBlack( TheHCRejectBlackParameter->DefaultValue() ),
useFittingRegion( TheHCUseFittingRegionParameter->DefaultValue() ),
fittingRect( 0 ),
generate64BitResult( TheHCGenerate64BitResultParameter->DefaultValue() ),
outputMasks( TheHCOutputMasksParameter->DefaultValue() ),
closePreviousImages( TheHCClosePreviousImagesParameter->DefaultValue() )
{
}

HDRCompositionInstance::HDRCompositionInstance( const HDRCompositionInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void HDRCompositionInstance::Assign( const ProcessImplementation& p )
{
   const HDRCompositionInstance* x = dynamic_cast<const HDRCompositionInstance*>( &p );
   if ( x != 0 )
   {
      images = x->images;
      inputHints = x->inputHints;
      maskBinarizingThreshold = x->maskBinarizingThreshold;
      maskSmoothness = x->maskSmoothness;
      maskGrowth = x->maskGrowth;
      autoExposures = x->autoExposures;
      rejectBlack = x->rejectBlack;
      useFittingRegion = x->useFittingRegion;
      fittingRect = x->fittingRect;
      generate64BitResult = x->generate64BitResult;
      outputMasks = x->outputMasks;
      closePreviousImages = x->closePreviousImages;
   }
}

bool HDRCompositionInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "HDRComposition can only be executed in the global context.";
   return false;
}

bool HDRCompositionInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

bool HDRCompositionInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( images.Length() < 2 )
   {
      whyNot = "This instance of HDRComposition defines less than two source images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class HDRCompositionFile
{
public:

   virtual ~HDRCompositionFile()
   {
      if ( file != 0 )
         delete file, file = 0;
   }

   String Path() const
   {
      return (file != 0) ? file->FilePath() : String();
   }

   template <class P>
   void ReadImage( GenericImage<P>& image )
   {
      if ( !file->ReadImage( image ) )
         throw CatchedException();
   }

   void ReadImage( ImageVariant& image )
   {
      if ( image )
         if ( !image.IsComplexSample() )
            if ( image.IsFloatSample() )
               switch ( image.BitsPerSample() )
               {
               case 32: ReadImage( static_cast<Image&>( *image ) ); break;
               case 64: ReadImage( static_cast<DImage&>( *image ) ); break;
               }
            else
               switch ( image.BitsPerSample() )
               {
               case  8: ReadImage( static_cast<UInt8Image&>( *image ) ); break;
               case 16: ReadImage( static_cast<UInt16Image&>( *image ) ); break;
               case 32: ReadImage( static_cast<UInt32Image&>( *image ) ); break;
               }
   }

   size_type Exposure() const
   {
      return exposure;
   }

   double RelativeExposure() const
   {
      return double( exposure )/files[0]->exposure;
   }

   bool operator <( const HDRCompositionFile& f ) const
   {
      return Exposure() > f.Exposure(); // largest exposure is first image
   }

   bool operator ==( const HDRCompositionFile& f ) const
   {
      return Path() == f.Path();
   }

   static void Open( const String& path, const HDRCompositionInstance* instance )
   {
      HDRCompositionFile* file = new HDRCompositionFile;
      files.Add( file );
      file->DoOpen( path, instance );
   }

   static void ComputeExposures( const HDRCompositionInstance* instance )
   {
      for ( file_list::iterator i = files.Begin(); i != files.End(); ++i )
         (*i)->ComputeExposure( instance );
   }

   static void Sort()
   {
      files.Sort();
   }

   static int NumberOfFiles()
   {
      return int( files.Length() );
   }

   static HDRCompositionFile& FileByIndex( int i )
   {
      return *files[i];
   }

   static int Width()
   {
      return width;
   }

   static int Height()
   {
      return height;
   }

   static int NumberOfChannels()
   {
      return numberOfChannels;
   }

   static bool IsColor()
   {
      return isColor;
   }

   static const UInt8Image* RejectionMask()
   {
      return rejectionMask;
   }

   static void CloseAll()
   {
      files.Destroy();

      if ( rejectionMask != 0 )
         delete rejectionMask, rejectionMask = 0;
   }

private:

   FileFormatInstance* file;
   size_type           exposure;

   typedef IndirectArray<HDRCompositionFile>  file_list;

   static file_list    files;
   static int          width;
   static int          height;
   static int          numberOfChannels;
   static bool         isColor;
   static UInt8Image*  rejectionMask;

   HDRCompositionFile() : file( 0 ), exposure( 0 )
   {
   }

   void DoOpen( const String&, const HDRCompositionInstance* );
   void ComputeExposure( const HDRCompositionInstance* );
};

HDRCompositionFile::file_list HDRCompositionFile::files;
int HDRCompositionFile::width = 0;
int HDRCompositionFile::height = 0;
int HDRCompositionFile::numberOfChannels = 0;
bool HDRCompositionFile::isColor = false;
UInt8Image* HDRCompositionFile::rejectionMask = 0;

void HDRCompositionFile::DoOpen( const String& path, const HDRCompositionInstance* instance )
{
   Console console;

   FileFormat format( File::ExtractExtension( path ), true, false );

   file = new FileFormatInstance( format );

   ImageDescriptionArray images;

   if ( !file->Open( images, path, instance->inputHints ) )
      throw CatchedException();

   if ( images.IsEmpty() )
      throw Error( file->FilePath() + ": Empty image file." );

   if ( images.Length() > 1 )
      throw Error( file->FilePath() + ": HDR composition of multiple image files is not supported." );

   if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
      throw Error( file->FilePath() + ": Invalid or unsupported image." );

   if ( files.Length() <= 1 ) // if this is the first file
   {
      width = images[0].info.width;
      height = images[0].info.height;
      numberOfChannels = images[0].info.numberOfChannels;
      isColor = images[0].info.colorSpace != ColorSpace::Gray;

      if ( instance->useFittingRegion )
         if ( !Rect( width, height ).Includes( instance->fittingRect ) )
            throw Error( String().Format( "Fitting rectangle out of image boundaries: {%d,%d,%d,%d}",
                                          instance->fittingRect.x0, instance->fittingRect.y0,
                                          instance->fittingRect.x1, instance->fittingRect.y1 ) );
      if ( instance->rejectBlack )
      {
         rejectionMask = new UInt8Image( width, height );
         rejectionMask->One();
      }
   }
   else
   {
      if ( width != images[0].info.width ||
           height != images[0].info.height ||
           numberOfChannels != images[0].info.numberOfChannels )
         throw Error( file->FilePath() + ": Incompatible image geometry." );
   }

   if ( instance->rejectBlack )
   {
      Image image( (void*)0, 0, 0 ); // shared image
      if ( !file->ReadImage( image ) )
         throw CatchedException();

            uint8* fM = rejectionMask->PixelData();
      const float* f0 = image.PixelData( 0 );
      const float* fN = f0 + image.NumberOfPixels();
      if ( isColor )
      {
         const float* f1 = image.PixelData( 1 );
         const float* f2 = image.PixelData( 2 );
         for ( ; f0 < fN; ++f0, ++f1, ++f2, ++fM )
            if ( *f0 == 0 || *f1 == 0 || *f2 == 0 )
               *fM = 0;
      }
      else
      {
         for ( ; f0 < fN; ++f0, ++fM )
            if ( *f0 == 0 )
               *fM = 0;
      }
   }
}

void HDRCompositionFile::ComputeExposure( const HDRCompositionInstance* instance )
{
   exposure = 0;

   Image image( (void*)0, 0, 0 ); // shared image
   if ( !file->ReadImage( image ) )
      throw CatchedException();

   image.Rescale();

   const uint8* fM = rejectionMask ? rejectionMask->PixelData() : 0;
   const float* f0 = image.PixelData( 0 );
   const float* fN = f0 + image.NumberOfPixels();
   if ( isColor )
   {
      const float* f1 = image.PixelData( 1 );
      const float* f2 = image.PixelData( 2 );
      for ( ; f0 < fN; ++f0, ++f1, ++f2 )
         if ( (fM == 0 || *fM++ != 0) && Max( Max( *f0, *f1 ), *f2 ) >= instance->maskBinarizingThreshold )
            ++exposure;
   }
   else
   {
      for ( ; f0 < fN; ++f0 )
         if ( (fM == 0 || *fM++ != 0) && *f0 >= instance->maskBinarizingThreshold )
            ++exposure;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class HDRCompositionEngine
{
public:

   HDRCompositionEngine( const HDRCompositionInstance* H ) : instance( H )
   {
      ImageWindow hdrWindow;
      Console console;

      try
      {
         ImageVariant hdr;
         hdrWindow = CreateHDRWorkingImageWindow( "HDR", instance->generate64BitResult ? 64 : 32 );
         hdr = hdrWindow.MainView().Image();

         console.WriteLn( String().Format( "<end><cbr><br>* HDR image component 1 of %d",
                                           HDRCompositionFile::NumberOfFiles() ) );
         HDRCompositionFile::FileByIndex( 0 ).ReadImage( hdr );

         hdr.Rescale();

         IVector Q = Quantize( hdr );

         console.WriteLn( "<end><cbr><br>* Initial quantization:" );

         for ( int c = 0; c < HDRCompositionFile::NumberOfChannels(); ++c )
         {
            double k = Q[c];
            console.WriteLn( String().Format( "q<sub>%d</sub> = %.5e (%.2f bits)", c, k, Log2( k ) ) );
         }

         Array<GenericVector<LinearFit> > L;

         UInt8Image outlierMask( HDRCompositionFile::Width(), HDRCompositionFile::Height() );

         for ( int i = 1; i < HDRCompositionFile::NumberOfFiles(); ++i )
         {
            console.WriteLn( String().Format( "<end><cbr><br>* Integrating HDR image component %d of %d",
                                              i+1, HDRCompositionFile::NumberOfFiles() ) );

            Image image( (void*)0, 0, 0 ); // shared image
            HDRCompositionFile::FileByIndex( i ).ReadImage( image );

            image.Rescale();

            GenericVector<LinearFit> fit( HDRCompositionFile::NumberOfChannels() );

            if ( !Combine( hdr, fit, outlierMask, image, i ) )
            {
               console.WarningLn( "<end><cbr><br>** This HDR composition cannot integrate further images - aborting process." );
               break;
            }

            L.Add( fit );
         }

         hdr.Rescale();

         console.WriteLn( "<end><cbr><br>* Dynamic range estimates:" );

         bool outOfRange = false;
         for ( int c = 0; c < HDRCompositionFile::NumberOfChannels(); ++c )
         {
            double k = Q[c];
            for ( size_type i = 0; i < L.Length(); ++i )
               k /= L[i][c].b;
            console.WriteLn( String().Format( "q<sub>%d</sub> = %.5e (%.2f bits)", c, k, Log2( k ) ) );

            if ( !instance->generate64BitResult && k > 1.2e+07 )
               outOfRange = true;
         }

         if ( outOfRange )
            console.WarningLn( "<end><cbr><br>** Warning: The HDR result does not fit in a 32-bit floating point image." );

         hdrWindow.Show();
      }
      catch ( ... )
      {
         if ( !hdrWindow.IsNull() )
            hdrWindow.Close();
         throw;
      }
   }

private:

   const HDRCompositionInstance* instance;

   //

   template <class P>
   IVector Quantize( const GenericImage<P>& image ) const
   {
      IVector q( 0, image.NumberOfNominalChannels() );

      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         GenericVector<size_type> Q( size_type( 0 ), QUANTIZATION_LEVELS );
         const typename P::sample* f  = image.PixelData( c );
         const typename P::sample* fN = f + image.NumberOfPixels();
         for ( int n = QUANTIZATION_LEVELS - 1; f < fN; ++f )
            if ( *f > 0 )
               ++Q[RoundI( *f * n )];
         for ( int i = 0; i < QUANTIZATION_LEVELS; ++i )
            if ( Q[i] > 0 )
               ++q[c];
      }

      return q;
   }

   IVector Quantize( const ImageVariant& image ) const
   {
      switch ( image.BitsPerSample() )
      {
      case 32 : return Quantize( static_cast<const Image&>( *image ) ); break;
      case 64 : return Quantize( static_cast<const DImage&>( *image ) ); break;
      }
      return IVector();
   }

   //

   template <class P>
   bool Combine( GenericImage<P>& hdr, GenericVector<LinearFit>& L, UInt8Image& outlierMask,
                 const Image& image, int index ) const
   {
      size_type N = hdr.NumberOfPixels();
      Console console;
      StandardStatus status;

      //hdr.Rescale();

      ImageWindow maskWindow = CreateGrayscaleImageWindow( IsoString().Format( "HDR_mask%02d", index ) );
      ImageVariant vmask = maskWindow.MainView().Image();
      Image& mask = static_cast<Image&>( *vmask );

      //mask.Zero(); -- not necessary - already zeroed by ImageWindow

      mask.SetStatusCallback( &status );
      mask.Status().Initialize( "<end><cbr>Building HDR composition mask", 4*N );
      mask.Status().DisableInitialization();

      {
         size_type count = 0;
         const typename P::sample* f0 = hdr.PixelData( 0 );
         const typename P::sample* fN = f0 + N;
                      const float* v0 = image.PixelData( 0 ); // for zero rejection
                            float* fm = mask.PixelData();
         if ( hdr.IsColor() )
         {
            const typename P::sample* f1 = hdr.PixelData( 1 );
            const typename P::sample* f2 = hdr.PixelData( 2 );
                         const float* v1 = image.PixelData( 1 );
                         const float* v2 = image.PixelData( 2 );
            for ( ; f0 < fN; ++f0, ++f1, ++f2, ++v0, ++v1, ++v2, ++fm, ++mask.Status() )  // N
               if ( !instance->rejectBlack || *v0 > 0 && *v1 > 0 && *v2 > 0 )
                  if ( Max( Max( *f0, *f1 ), *f2 ) >= instance->maskBinarizingThreshold )
                     *fm = 1, ++count;
         }
         else
         {
            for ( ; f0 < fN; ++f0, ++v0, ++fm, ++mask.Status() )
               if ( !instance->rejectBlack || *v0 > 0 )
                  if ( *f0 >= instance->maskBinarizingThreshold )
                     *fm = 1, ++count;
         }

         if ( count == 0 )
         {
            mask.Status() += 3*N;
            maskWindow.Close();
            return false;
         }
      }

      if ( instance->maskGrowth > 0 )
      {
         DilationFilter D;
         CircularStructure C( instance->maskGrowth*2 + 1 );
         MorphologicalTransformation M( D, C );
         M >> mask;
      }
      else
         mask.Status() += N;

      if ( instance->maskSmoothness > 0 )
      {
         GaussianFilter G( instance->maskSmoothness*2 + 1 );
         if ( G.Size() > 15 )
         {
            FFTConvolution C( G );
            C >> mask;
         }
         else
         {
            Convolution C( G );
            C >> mask;
         }
      }
      else
         mask.Status() += N;

      if ( instance->rejectBlack )
      {
         const float* v0 = image.PixelData( 0 );
         const float* vN = v0 + N;
               float* fm = mask.PixelData();
         if ( hdr.IsColor() )
         {
            const float* v1 = image.PixelData( 1 );
            const float* v2 = image.PixelData( 2 );
            for ( ; v0 < vN; ++v0, ++v1, ++v2, ++fm, ++mask.Status() )  // N
               if ( *v0 <= 0 || *v1 <= 0 || *v2 <= 0 )
                  *fm = 0;
         }
         else
         {
            for ( ; v0 < vN; ++v0, ++fm, ++mask.Status() )
               if ( *v0 <= 0 )
                  *fm = 0;
         }
      }

      if ( index == 1 )
         outlierMask.One();

      {
         SpinStatus spin;
         StatusMonitor monitor;
         monitor.SetCallback( &spin );
         monitor.Initialize( "<end><cbr>Fitting linear HDR image" );

         for ( int c = 0; c < hdr.NumberOfNominalChannels(); ++c )
         {
            Array<float> F0, F1;
            if ( instance->useFittingRegion )
            {
               F0.Reserve( size_type( instance->fittingRect.Area() ) );
               F1.Reserve( size_type( instance->fittingRect.Area() ) );
               for ( int y = instance->fittingRect.y0; y < instance->fittingRect.y1; ++y )
               {
                  const typename P::sample* f0 = hdr.PixelAddress( instance->fittingRect.x0, y, c );
                  const typename P::sample* fN = f0 + instance->fittingRect.Width();
                               const float* f1 = image.PixelAddress( instance->fittingRect.x0, y, c );
                               const uint8* fo = outlierMask.PixelAddress( instance->fittingRect.x0, y );
                  for ( ; f0 < fN; ++f0, ++f1, ++fo, ++monitor )
                     if ( *fo )
                        if ( *f0 > 0 && *f0 < 0.92 )
                           if ( *f1 > 0 && *f1 < 0.92 )
                           {
                              F0.Add( float( *f0 ) );
                              F1.Add( *f1 );
                           }
               }
            }
            else
            {
               F0.Reserve( N );
               F1.Reserve( N );
               const typename P::sample* f0 = hdr.PixelData( c );
               const typename P::sample* fN = f0 + N;
                            const float* f1 = image.PixelData( c );
                            const uint8* fo = outlierMask.PixelData();
               for ( ; f0 < fN; ++f0, ++f1, ++fo, ++monitor )
                  if ( *fo )
                     if ( *f0 > 0 && *f0 < 0.92 )

                        if ( *f1 > 0 && *f1 < 0.92 )
                        {
                           F0.Add( float( *f0 ) );
                           F1.Add( *f1 );
                        }
            }

            if ( F0.Length() < 3 )
               throw Error( "Insufficient data (channel " + String( c ) + ')' );

            L[c] = LinearFit( F0, F1, &monitor );

            if ( L[c].b > 1 )
               throw Error( "Inconsistent HDR composition detected (bad linear fit, channel " + String( c ) + ')' );

            {
               typename P::sample* f0 = hdr.PixelData( c );
               typename P::sample* fN = f0 + N;
               for ( ; f0 < fN; ++f0, ++monitor )   // N
                  *f0 = L[c].a + *f0*L[c].b;
            }
         }

         monitor.Complete();

         console.WriteLn( "<end><cbr>Linear fit functions:" );
         for ( int c = 0; c < hdr.NumberOfNominalChannels(); ++c )
            console.WriteLn( String().Format( "y<sub>%d</sub> = %+.6f + %.6f&middot;x<sub>%d</sub>",
                                              c, L[c].a, L[c].b, c ) );
      }

      {
         StatusMonitor monitor;
         monitor.SetCallback( &status );
         monitor.Initialize( "<end><cbr>Performing HDR composition", N*hdr.NumberOfNominalChannels() + N );

         for ( int c = 0; c < hdr.NumberOfNominalChannels(); ++c )
         {
            typename P::sample* f0 = hdr.PixelData( c );
                   const float* f1 = image.PixelData( c );
                   const float* fm = mask.PixelData();
            for ( size_type i = 0; i < N; ++i, ++f0, ++f1, ++fm, ++monitor )
               *f0 = *f0 * (1 - *fm) + *f1 * *fm;
         }

         {
                  uint8* fo = outlierMask.PixelData();
            const float* fm = mask.PixelData();
            for ( size_type i = 0; i < N; ++i, ++fo, ++fm, ++monitor )
               *fo = (*fm < 0.00001 || *fm > 0.99999) ? 1 : 0;
         }
      }

      if ( instance->outputMasks )
         maskWindow.Show();
      else
         maskWindow.Close();

      return true;
   }

   bool Combine( ImageVariant& hdr, GenericVector<LinearFit>& L, UInt8Image& outlierMask,
                 const Image& image, int index ) const
   {
      switch ( hdr.BitsPerSample() )
      {
      case 32 : return Combine( static_cast<Image&>( *hdr ), L, outlierMask, image, index ); break;
      case 64 : return Combine( static_cast<DImage&>( *hdr ), L, outlierMask, image, index ); break;
      }
      return false;
   }

   //

   ImageWindow CreateGrayscaleImageWindow( const IsoString& id,
                                           int bitsPerSample = 32, bool floatingPoint = true ) const
   {
      return CreateImageWindow( id, true, bitsPerSample, floatingPoint );
   }

   ImageWindow CreateHDRWorkingImageWindow( const IsoString& id,
                                            int bitsPerSample = 32, bool floatingPoint = true ) const
   {
      return CreateImageWindow( id, false, bitsPerSample, floatingPoint );
   }

   ImageWindow CreateImageWindow( const IsoString& id,
                                  bool grayscale = true,
                                  int bitsPerSample = 32, bool floatingPoint = true ) const
   {
      if ( instance->closePreviousImages )
      {
         ImageWindow window = ImageWindow::WindowById( id );
         if ( !window.IsNull() )
            window.Close();
      }

      ImageWindow window( HDRCompositionFile::Width(), HDRCompositionFile::Height(),
                          grayscale ? 1 : HDRCompositionFile::NumberOfChannels(),
                          bitsPerSample, floatingPoint,
                          grayscale ? false : HDRCompositionFile::IsColor(),
                          true, id );
      if ( window.IsNull() )
         throw Error( "Unable to create image window: " + id );

      return window;
   }
};

// ----------------------------------------------------------------------------

bool HDRCompositionInstance::ExecuteGlobal()
{
   ImageWindow hdrWindow, maskWindow;

   try
   {
      if ( useFittingRegion )
      {
         fittingRect.Order();
         if ( !fittingRect.IsRect() )
            throw Error( "Empty fitting region defined" );
      }

      Console console;
      console.EnableAbort();

      console.WriteLn( "<end><cbr><br>Opening files:" );

      for ( image_list::const_iterator i = images.Begin(); i != images.End(); ++i )
         if ( i->enabled )
         {
            console.WriteLn( i->path );
            HDRCompositionFile::Open( i->path, this );
         }

      if ( HDRCompositionFile::NumberOfFiles() < 2 )
         throw Error( "HDRComposition requires at least two images; this instance defines " +
            String( HDRCompositionFile::NumberOfFiles() ) + " image(s)." );

      if ( autoExposures )
      {
         console.WriteLn( "<end><cbr><br>Computing relative exposures:" );
         HDRCompositionFile::ComputeExposures( this );
         HDRCompositionFile::Sort();
      }

      console.WriteLn( String().Format( "<end><cbr><br>HDR composition of %d images:", HDRCompositionFile::NumberOfFiles() ) );
      for ( int i = 0; i < HDRCompositionFile::NumberOfFiles(); ++i )
      {
         String s = String().Format( "%2d : ", i+1 );
         if ( autoExposures )
            s.AppendFormat( "%.4e : ", HDRCompositionFile::FileByIndex( i ).RelativeExposure() );
         console.WriteLn( s + HDRCompositionFile::FileByIndex( i ).Path() );
      }

      HDRCompositionEngine( this );

      HDRCompositionFile::CloseAll();
      return true;
   }
   catch ( ... )
   {
      HDRCompositionFile::CloseAll();
      throw;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void* HDRCompositionInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheHCImageEnabledParameter )
      return &images[tableRow].enabled;
   if ( p == TheHCImagePathParameter )
      return images[tableRow].path.c_str();
   if ( p == TheHCInputHintsParameter )
      return inputHints.c_str();
   if ( p == TheHCMaskBinarizingThresholdParameter )
      return &maskBinarizingThreshold;
   if ( p == TheHCMaskSmoothnessParameter )
      return &maskSmoothness;
   if ( p == TheHCMaskGrowthParameter )
      return &maskGrowth;
   if ( p == TheHCAutoExposuresParameter )
      return &autoExposures;
   if ( p == TheHCRejectBlackParameter )
      return &rejectBlack;
   if ( p == TheHCUseFittingRegionParameter )
      return &useFittingRegion;
   if ( p == TheHCFittingRectX0Parameter )
      return &fittingRect.x0;
   if ( p == TheHCFittingRectY0Parameter )
      return &fittingRect.y0;
   if ( p == TheHCFittingRectX1Parameter )
      return &fittingRect.x1;
   if ( p == TheHCFittingRectY1Parameter )
      return &fittingRect.y1;
   if ( p == TheHCGenerate64BitResultParameter )
      return &generate64BitResult;
   if ( p == TheHCOutputMasksParameter )
      return &outputMasks;
   if ( p == TheHCClosePreviousImagesParameter )
      return &closePreviousImages;
   return 0;
}

bool HDRCompositionInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheHCImagesParameter )
   {
      images.Clear();
      if ( sizeOrLength > 0 )
         images.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheHCImagePathParameter )
   {
      images[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         images[tableRow].path.Reserve( sizeOrLength );
   }
   else if ( p == TheHCInputHintsParameter )
   {
      inputHints.Clear();
      if ( sizeOrLength > 0 )
         inputHints.Reserve( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type HDRCompositionInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheHCImagesParameter )
      return images.Length();
   if ( p == TheHCImagePathParameter )
      return images[tableRow].path.Length();
   if ( p == TheHCInputHintsParameter )
      return inputHints.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF HDRCompositionInstance.cpp - Released 2014/11/14 17:19:21 UTC
