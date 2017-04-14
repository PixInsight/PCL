//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.12.01.0368
// ----------------------------------------------------------------------------
// HDRCompositionInstance.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "HDRCompositionInstance.h"

#include <pcl/AutoPointer.h>
#include <pcl/Convolution.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FFTConvolution.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/GaussianFilter.h>
#include <pcl/ImageWindow.h>
#include <pcl/LinearFit.h>
#include <pcl/MetaModule.h>
#include <pcl/MorphologicalOperator.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MultiscaleMedianTransform.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/StructuringElement.h>
#include <pcl/Version.h>
#include <pcl/View.h>

#define FIT_MAX               0.92
#define QUANTIZATION_LEVELS   15000000

namespace pcl
{

// ----------------------------------------------------------------------------

HDRCompositionInstance::HDRCompositionInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_images(),
   p_inputHints(),
   p_maskBinarizingThreshold( TheHCMaskBinarizingThresholdParameter->DefaultValue() ),
   p_maskSmoothness( TheHCMaskSmoothnessParameter->DefaultValue() ),
   p_maskGrowth( TheHCMaskGrowthParameter->DefaultValue() ),
   p_replaceLargeScales( TheHCReplaceLargeScalesParameter->DefaultValue() ),
   p_autoExposures( TheHCAutoExposuresParameter->DefaultValue() ),
   p_rejectBlack( TheHCRejectBlackParameter->DefaultValue() ),
   p_useFittingRegion( TheHCUseFittingRegionParameter->DefaultValue() ),
   p_fittingRect( 0 ),
   p_generate64BitResult( TheHCGenerate64BitResultParameter->DefaultValue() ),
   p_outputMasks( TheHCOutputMasksParameter->DefaultValue() ),
   p_closePreviousImages( TheHCClosePreviousImagesParameter->DefaultValue() )
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
   if ( x != nullptr )
   {
      p_images = x->p_images;
      p_inputHints = x->p_inputHints;
      p_maskBinarizingThreshold = x->p_maskBinarizingThreshold;
      p_maskSmoothness = x->p_maskSmoothness;
      p_maskGrowth = x->p_maskGrowth;
      p_replaceLargeScales = x->p_replaceLargeScales;
      p_autoExposures = x->p_autoExposures;
      p_rejectBlack = x->p_rejectBlack;
      p_useFittingRegion = x->p_useFittingRegion;
      p_fittingRect = x->p_fittingRect;
      p_generate64BitResult = x->p_generate64BitResult;
      p_outputMasks = x->p_outputMasks;
      p_closePreviousImages = x->p_closePreviousImages;
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
   if ( p_images.Length() < 2 )
   {
      whyNot = "This instance of HDRComposition defines less than two input images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class HDRCompositionFile
{
public:

   String Path() const
   {
      return file.IsNull() ? String() : file->FilePath();
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
      for ( auto i : files )
         i->ComputeExposure( instance );
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
      rejectionMask.Destroy();
   }

private:

   typedef IndirectArray<HDRCompositionFile>  file_list;

   AutoPointer<FileFormatInstance> file;
   size_type                       exposure;
   static file_list                files;
   static int                      width;
   static int                      height;
   static int                      numberOfChannels;
   static bool                     isColor;
   static AutoPointer<UInt8Image>  rejectionMask;

   HDRCompositionFile() : exposure( 0 )
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
AutoPointer<UInt8Image> HDRCompositionFile::rejectionMask;

void HDRCompositionFile::DoOpen( const String& path, const HDRCompositionInstance* instance )
{
   FileFormat format( File::ExtractExtension( path ), true, false );

   file = new FileFormatInstance( format );

   ImageDescriptionArray images;

   if ( !file->Open( images, path, instance->p_inputHints ) )
      throw CatchedException();

   if ( images.IsEmpty() )
      throw Error( file->FilePath() + ": Empty image file." );

   if ( images.Length() > 1 )
      Console().NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in input file.", images.Length()-1 ) );

   if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
      throw Error( file->FilePath() + ": Invalid or unsupported image." );

   if ( files.Length() <= 1 ) // if this is the first file
   {
      width = images[0].info.width;
      height = images[0].info.height;
      numberOfChannels = images[0].info.numberOfChannels;
      isColor = images[0].info.colorSpace != ColorSpace::Gray;

      if ( instance->p_useFittingRegion )
         if ( !Rect( width, height ).Includes( instance->p_fittingRect ) )
            throw Error( String().Format( "Fitting rectangle out of image boundaries: {%d,%d,%d,%d}",
                                          instance->p_fittingRect.x0, instance->p_fittingRect.y0,
                                          instance->p_fittingRect.x1, instance->p_fittingRect.y1 ) );
      if ( instance->p_rejectBlack )
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

   if ( instance->p_rejectBlack )
   {
      Image image( (void*)0, 0, 0 ); // shared image
      if ( !file->ReadImage( image ) )
         throw CatchedException();

      if ( isColor )
      {
         UInt8Image::sample_iterator m( *rejectionMask );
         Image::const_pixel_iterator f( image );
         for ( ; f; ++f, ++m )
            if ( f[0] == 0 || f[1] == 0 || f[2] == 0 )
               *m = 0;
      }
      else
      {
         UInt8Image::sample_iterator m( *rejectionMask );
         Image::const_sample_iterator f( image );
         for ( ; f; ++f, ++m )
            if ( *f == 0 )
               *m = 0;
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

   if ( isColor )
   {
      Image::const_pixel_iterator f( image );
      if ( rejectionMask.IsNull() )
      {
         for ( ; f; ++f )
            if ( Max( Max( f[0], f[1] ), f[2] ) >= instance->p_maskBinarizingThreshold )
               ++exposure;
      }
      else
      {
         UInt8Image::const_sample_iterator m( *rejectionMask );
         for ( ; f; ++f, ++m )
            if ( *m != 0 )
               if ( Max( Max( f[0], f[1] ), f[2] ) >= instance->p_maskBinarizingThreshold )
                  ++exposure;
      }
   }
   else
   {
      Image::const_sample_iterator f( image );
      if ( rejectionMask.IsNull() )
      {
         for ( ; f; ++f )
            if ( *f >= instance->p_maskBinarizingThreshold )
               ++exposure;
      }
      else
      {
         UInt8Image::const_sample_iterator m( *rejectionMask );
         for ( ; f; ++f, ++m )
            if ( *m != 0 )
               if ( *f >= instance->p_maskBinarizingThreshold )
                  ++exposure;
      }
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
         hdrWindow = CreateHDRWorkingImageWindow( "HDR", instance->p_generate64BitResult ? 64 : 32 );
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
            console.WriteLn( String().Format( "q%d = %.5e (%.2f bits)", c, k, Log2( k ) ) );
         }

         Array<GenericVector<LinearFit> > L;

         UInt8Image outlierMask( HDRCompositionFile::Width(), HDRCompositionFile::Height() );

         for ( int index = 1; index < HDRCompositionFile::NumberOfFiles(); ++index )
         {
            console.WriteLn( String().Format( "<end><cbr><br>* Integrating HDR image component %d of %d",
                                              index+1, HDRCompositionFile::NumberOfFiles() ) );

            Image image( (void*)0, 0, 0 ); // shared image
            HDRCompositionFile::FileByIndex( index ).ReadImage( image );

            image.Rescale();

            GenericVector<LinearFit> lfit( HDRCompositionFile::NumberOfChannels() );

            if ( !Combine( hdr, lfit, outlierMask, image, index ) )
            {
               console.WarningLn( "<end><cbr><br>** This HDR composition cannot integrate further images - aborting process." );
               break;
            }

            L << lfit;
         }

         hdr.Rescale();

         console.WriteLn( "<end><cbr><br>* Dynamic range estimates:" );

         bool outOfRange = false;
         for ( int c = 0; c < HDRCompositionFile::NumberOfChannels(); ++c )
         {
            double k = Q[c];
            for ( size_type i = 0; i < L.Length(); ++i )
               k /= L[i][c].b;
            console.WriteLn( String().Format( "q%d = %.5e (%.2f bits)", c, k, Log2( k ) ) );

            if ( !instance->p_generate64BitResult && k > 1.2e+07 )
               outOfRange = true;
         }

         if ( outOfRange )
            console.WarningLn( "<end><cbr><br>** Warning: The HDR result does not fit in a 32-bit floating point image." );

         FITSKeywordArray keywords;
         hdrWindow.GetKeywords( keywords );

         keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Integration with " + PixInsightVersion::AsString() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with " + Module->ReadableVersion() )
                  << FITSHeaderKeyword( "HISTORY", IsoString(), "Integration with HDRComposition process" )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "HDRComposition.maskBinarizingThreshold: %.4f", instance->p_maskBinarizingThreshold ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "HDRComposition.maskSmoothness: %d", instance->p_maskSmoothness ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "HDRComposition.maskGrowth: %d", instance->p_maskGrowth ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        IsoString().Format( "HDRComposition.replaceLargeScales: %d", instance->p_replaceLargeScales ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        "HDRComposition.autoExposures: " + IsoString( bool( instance->p_autoExposures ) ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        "HDRComposition.rejectBlack: " + IsoString( bool( instance->p_rejectBlack ) ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        "HDRComposition.generate64BitResult: " + IsoString( bool( instance->p_generate64BitResult ) ) )
                  << FITSHeaderKeyword( "HISTORY", IsoString(),
                                        "HDRComposition.useFittingRegion: " + IsoString( bool( instance->p_useFittingRegion ) ) );
         if ( instance->p_useFittingRegion )
            keywords << FITSHeaderKeyword( "HISTORY", IsoString(),
                                           IsoString().Format( "HDRComposition.fittingRect: left=%d, top=%d, width=%d, height=%d",
                                                            instance->p_fittingRect.x0, instance->p_fittingRect.y0,
                                                            instance->p_fittingRect.Width(), instance->p_fittingRect.Height() ) );

         hdrWindow.SetKeywords( keywords );

         hdrWindow.Show();
         hdrWindow.ZoomToFit( false/*allowZoomIn*/ );
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

   template <class P>
   IVector Quantize( const GenericImage<P>& image ) const
   {
      IVector q( 0, image.NumberOfNominalChannels() );
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         GenericVector<size_type> Q( size_type( 0 ), QUANTIZATION_LEVELS );
         for ( typename GenericImage<P>::const_sample_iterator f( image, c ); f; ++f )
            if ( *f > 0 )
               ++Q[RoundInt( *f * (QUANTIZATION_LEVELS-1) )];
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
      case 32: return Quantize( static_cast<const Image&>( *image ) ); break;
      case 64: return Quantize( static_cast<const DImage&>( *image ) ); break;
      }
      return IVector();
   }

   template <class P>
   bool Combine( GenericImage<P>& hdr, GenericVector<LinearFit>& L, UInt8Image& outlierMask, Image& image, int index ) const
   {
      size_type N = hdr.NumberOfPixels();
      Console console;
      StandardStatus status;

      ImageWindow maskWindow = CreateGrayscaleImageWindow( IsoString().Format( "HDR_mask%02d", index ) );
      ImageVariant vmask = maskWindow.MainView().Image();
      Image& mask = static_cast<Image&>( *vmask );

      //mask.Zero(); -- not necessary - already zeroed by ImageWindow

      mask.SetStatusCallback( &status );
      mask.Status().Initialize( "<end><cbr>Building HDR composition mask", 4*N );
      mask.Status().DisableInitialization();

      {
         size_type count = 0;
         Image::sample_iterator m( mask );
         if ( hdr.IsColor() )
         {
            typename GenericImage<P>::const_pixel_iterator f( hdr );
            for ( ; f; ++f, ++m, ++mask.Status() )  // N
               if ( Max( Max( f[0], f[1] ), f[2] ) >= instance->p_maskBinarizingThreshold )
                  *m = 1, ++count;
         }
         else
         {
            typename GenericImage<P>::const_sample_iterator f( hdr );
            for ( ; f; ++f, ++m, ++mask.Status() )  // N
               if ( *f >= instance->p_maskBinarizingThreshold )
                  *m = 1, ++count;
         }

         if ( count == 0 )
         {
            mask.Status().Complete();
            maskWindow.Close();
            return false;
         }
      }

      if ( instance->p_maskGrowth > 0 )
      {
         DilationFilter D;
         CircularStructure C( instance->p_maskGrowth*2 + 1 );
         MorphologicalTransformation M( D, C );
         M >> mask;
      }
      else
         mask.Status() += N;

      if ( instance->p_maskSmoothness > 0 )
      {
         GaussianFilter G( instance->p_maskSmoothness*2 + 1 );
         if ( G.Size() >= PCL_FFT_CONVOLUTION_IS_FASTER_THAN_SEPARABLE_FILTER_SIZE )
         {
            FFTConvolution C( G );
            C >> mask;
         }
         else
         {
            SeparableFilter S( G.AsSeparableFilter() );
            SeparableConvolution C( S );
            C >> mask;
         }
      }
      else
         mask.Status() += N;

      if ( instance->p_rejectBlack )
      {
         Image::sample_iterator m( mask );
         if ( hdr.IsColor() )
         {
            Image::const_pixel_iterator v( image );
            for ( ; v; ++v, ++m, ++mask.Status() )  // N
               if ( v[0] <= 0 || v[1] <= 0 || v[2] <= 0 )
                  *m = 0;
         }
         else
         {
            Image::const_sample_iterator v( image );
            for ( ; v; ++v, ++m, ++mask.Status() )  // N
               if ( *v <= 0 )
                  *m = 0;
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
            if ( instance->p_useFittingRegion )
            {
               F0.Reserve( size_type( instance->p_fittingRect.Area() ) );
               F1.Reserve( size_type( instance->p_fittingRect.Area() ) );
               typename GenericImage<P>::const_roi_sample_iterator f( hdr, instance->p_fittingRect, c );
               Image::const_roi_sample_iterator v( image, instance->p_fittingRect, c );
               UInt8Image::const_roi_sample_iterator o( outlierMask, instance->p_fittingRect );
               for ( ; f; ++f, ++v, ++o, ++monitor )
                  if ( *o )
                     if ( *f > 0 && *f < FIT_MAX )
                        if ( *v > 0 && *v < FIT_MAX )
                        {
                           F0.Add( float( *f ) );
                           F1.Add( *v );
                        }
            }
            else
            {
               F0.Reserve( N );
               F1.Reserve( N );
               typename GenericImage<P>::const_sample_iterator f( hdr, c );
               Image::const_sample_iterator v( image, c );
               UInt8Image::const_sample_iterator o( outlierMask );
               for ( ; f; ++f, ++v, ++o, ++monitor )
                  if ( *o )
                     if ( *f > 0 && *f < FIT_MAX )
                        if ( *v > 0 && *v < FIT_MAX )
                        {
                           F0.Add( float( *f ) );
                           F1.Add( *v );
                        }
            }

            if ( F0.Length() < 3 )
               throw Error( "Insufficient data (channel " + String( c ) + ')' );

            L[c] = LinearFit( F0, F1, &monitor );

            if ( L[c].b > 1 )
               throw Error( "Inconsistent HDR composition detected (bad linear fit, channel " + String( c ) + ')' );

            {
               typename GenericImage<P>::sample_iterator f( hdr, c );
               for ( ; f; ++f, ++monitor )   // N
                  *f = L[c].a + *f*L[c].b;
            }
         }

         monitor.Complete();

         console.WriteLn( "<end><cbr>Linear fit functions:" );
         for ( int c = 0; c < hdr.NumberOfNominalChannels(); ++c )
            console.WriteLn( String().Format( "y%d = %+.6f + %.6f * x%d", c, L[c].a, L[c].b, c ) );
      }

      if ( instance->p_replaceLargeScales > 0 )
      {
         int J = 4 + instance->p_replaceLargeScales;
         image.SetStatusCallback( &status );
         image.Status().Initialize( "<end><cbr>Performing large-scale layer substitution", (2*J + 3)*N*hdr.NumberOfNominalChannels() );
         image.Status().DisableInitialization();

         hdr.SelectNominalChannels();
         image.SelectNominalChannels();
         MultiscaleMedianTransform M( J );
         for ( int j = 0; j < J; ++j )
            M.DisableLayer( j );
         hdr.Status() = image.Status();
         M << hdr;
         Image ls = M[J];
         image.Status() = hdr.Status();
         M << image;
         image.Sub( M[J] );
         image.Add( ls );
         image.Truncate();
         hdr.Status().Clear();
         image.Status().Clear();

//          ImageWindow w1 = CreateImageWindow( "hdrLS", false );
//          ImageVariant v1 = w1.MainView().Image();
//          v1.Apply( ImageVariant( &ls ) );
//          ImageWindow w2 = CreateImageWindow( "image", false );
//          ImageVariant v2 = w2.MainView().Image();
//          v2.Apply( ImageVariant( &image ) );
//          w1.Show();
//          w2.Show();
      }

      {
         StatusMonitor monitor;
         monitor.SetCallback( &status );
         monitor.Initialize( "<end><cbr>Performing HDR composition", N*hdr.NumberOfNominalChannels() + N );

         for ( int c = 0; c < hdr.NumberOfNominalChannels(); ++c )
         {
            typename GenericImage<P>::sample_iterator f( hdr, c );
            Image::const_sample_iterator v( image, c );
            Image::const_sample_iterator m( mask );
            for ( ; f; ++f, ++v, ++m, ++monitor )
               *f = *f * (1 - *m) + *v * *m;
         }

         {
            UInt8Image::sample_iterator o( outlierMask );
            Image::const_sample_iterator m( mask );
            for ( ; o; ++o, ++m, ++monitor )
               *o = (*m < 0.00001 || *m > 0.99999) ? 1 : 0;
         }
      }

      if ( instance->p_outputMasks )
         maskWindow.Show();
      else
         maskWindow.Close();

      return true;
   }

   bool Combine( ImageVariant& hdr, GenericVector<LinearFit>& L, UInt8Image& outlierMask, Image& image, int index ) const
   {
      switch ( hdr.BitsPerSample() )
      {
      case 32: return Combine( static_cast<Image&>( *hdr ), L, outlierMask, image, index ); break;
      case 64: return Combine( static_cast<DImage&>( *hdr ), L, outlierMask, image, index ); break;
      }
      return false;
   }

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
      if ( instance->p_closePreviousImages )
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
      if ( p_useFittingRegion )
      {
         p_fittingRect.Order();
         if ( !p_fittingRect.IsRect() )
            throw Error( "Empty fitting region defined" );
      }

      Console console;
      console.EnableAbort();

      console.WriteLn( "<end><cbr><br>Opening files:" );

      for ( auto i : p_images )
         if ( i.enabled )
         {
            console.WriteLn( i.path );
            HDRCompositionFile::Open( i.path, this );
         }

      if ( HDRCompositionFile::NumberOfFiles() < 2 )
         throw Error( "HDRComposition requires at least two input images" );

      if ( p_autoExposures )
      {
         console.WriteLn( "<end><cbr><br>Computing relative exposures:" );
         HDRCompositionFile::ComputeExposures( this );
         HDRCompositionFile::Sort();
      }

      console.WriteLn( String().Format( "<end><cbr><br>HDR composition of %d images:", HDRCompositionFile::NumberOfFiles() ) );
      for ( int i = 0; i < HDRCompositionFile::NumberOfFiles(); ++i )
      {
         String s = String().Format( "%2d : ", i+1 );
         if ( p_autoExposures )
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
      return &p_images[tableRow].enabled;
   if ( p == TheHCImagePathParameter )
      return p_images[tableRow].path.Begin();
   if ( p == TheHCInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheHCMaskBinarizingThresholdParameter )
      return &p_maskBinarizingThreshold;
   if ( p == TheHCMaskSmoothnessParameter )
      return &p_maskSmoothness;
   if ( p == TheHCMaskGrowthParameter )
      return &p_maskGrowth;
   if ( p == TheHCReplaceLargeScalesParameter )
      return &p_replaceLargeScales;
   if ( p == TheHCAutoExposuresParameter )
      return &p_autoExposures;
   if ( p == TheHCRejectBlackParameter )
      return &p_rejectBlack;
   if ( p == TheHCUseFittingRegionParameter )
      return &p_useFittingRegion;
   if ( p == TheHCFittingRectX0Parameter )
      return &p_fittingRect.x0;
   if ( p == TheHCFittingRectY0Parameter )
      return &p_fittingRect.y0;
   if ( p == TheHCFittingRectX1Parameter )
      return &p_fittingRect.x1;
   if ( p == TheHCFittingRectY1Parameter )
      return &p_fittingRect.y1;
   if ( p == TheHCGenerate64BitResultParameter )
      return &p_generate64BitResult;
   if ( p == TheHCOutputMasksParameter )
      return &p_outputMasks;
   if ( p == TheHCClosePreviousImagesParameter )
      return &p_closePreviousImages;
   return nullptr;
}

bool HDRCompositionInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheHCImagesParameter )
   {
      p_images.Clear();
      if ( sizeOrLength > 0 )
         p_images.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheHCImagePathParameter )
   {
      p_images[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_images[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheHCInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type HDRCompositionInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheHCImagesParameter )
      return p_images.Length();
   if ( p == TheHCImagePathParameter )
      return p_images[tableRow].path.Length();
   if ( p == TheHCInputHintsParameter )
      return p_inputHints.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HDRCompositionInstance.cpp - Released 2017-04-14T23:07:12Z
