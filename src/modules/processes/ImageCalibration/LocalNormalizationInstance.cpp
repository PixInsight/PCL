//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0356
// ----------------------------------------------------------------------------
// LocalNormalizationInstance.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "LocalNormalizationInstance.h"
#include "LocalNormalizationParameters.h"
#include "OutputFileData.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ExternalProcess.h>
#include <pcl/GaussianFilter.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ImageWindow.h>
#include <pcl/LocalNormalizationData.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/MultiscaleMedianTransform.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/Resample.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/ShepardInterpolation.h>
#include <pcl/StdStatus.h>
#include <pcl/SurfaceSpline.h>
#include <pcl/Version.h>
#include <pcl/View.h>

#define TINY_SAMPLE_VALUE  4.5e-5   // approx. 3/65535

#include <iostream>

namespace pcl
{

// ----------------------------------------------------------------------------

LocalNormalizationInstance::LocalNormalizationInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_scale( TheLNScaleParameter->DefaultValue() ),
   p_noScale( TheLNNoScaleParameter->DefaultValue() ),
   p_rejection( TheLNRejectionParameter->DefaultValue() ),
   p_backgroundRejectionLimit( TheLNBackgroundRejectionLimitParameter->DefaultValue() ),
   p_referenceRejectionThreshold( TheLNReferenceRejectionThresholdParameter->DefaultValue() ),
   p_targetRejectionThreshold( TheLNTargetRejectionThresholdParameter->DefaultValue() ),
   p_hotPixelFilterRadius( TheLNHotPixelFilterRadiusParameter->DefaultValue() ),
   p_noiseReductionFilterRadius( TheLNNoiseReductionFilterRadiusParameter->DefaultValue() ),
   p_referencePathOrViewId( TheLNReferencePathOrViewIdParameter->DefaultValue() ),
   p_referenceIsView( TheLNReferenceIsViewParameter->DefaultValue() ),
   p_inputHints( TheLNInputHintsParameter->DefaultValue() ),
   p_outputHints( TheLNOutputHintsParameter->DefaultValue() ),
   p_generateNormalizedImages( LNGenerateNormalizedImages::Default ),
   p_generateNormalizationData( TheLNGenerateNormalizationDataParameter->DefaultValue() ),
   p_showBackgroundModels( TheLNShowBackgroundModelsParameter->DefaultValue() ),
   p_showRejectionMaps( TheLNShowRejectionMapsParameter->DefaultValue() ),
   p_plotNormalizationFunctions( LNPlotNormalizationFunctions::Default ),
   p_noGUIMessages( TheLNNoGUIMessagesParameter->DefaultValue() ),
   p_outputDirectory( TheLNOutputDirectoryParameter->DefaultValue() ),
   p_outputExtension( TheLNOutputExtensionParameter->DefaultValue() ),
   p_outputPrefix( TheLNOutputPrefixParameter->DefaultValue() ),
   p_outputPostfix( TheLNOutputPostfixParameter->DefaultValue() ),
   p_overwriteExistingFiles( TheLNOverwriteExistingFilesParameter->DefaultValue() ),
   p_onError( LNOnError::Default ),
   p_useFileThreads( TheLNUseFileThreadsParameter->DefaultValue() ),
   p_fileThreadOverload( TheLNFileThreadOverloadParameter->DefaultValue() ),
   p_maxFileReadThreads( TheLNMaxFileReadThreadsParameter->DefaultValue() ),
   p_maxFileWriteThreads( TheLNMaxFileWriteThreadsParameter->DefaultValue() ),
   p_graphSize( TheLNGraphSizeParameter->DefaultValue() ),
   p_graphTextSize( TheLNGraphTextSizeParameter->DefaultValue() ),
   p_graphTitleSize( TheLNGraphTitleSizeParameter->DefaultValue() ),
   p_graphTransparent( TheLNGraphTransparentParameter->DefaultValue() ),
   p_graphOutputDirectory( TheLNGraphOutputDirectoryParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

LocalNormalizationInstance::LocalNormalizationInstance( const LocalNormalizationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void LocalNormalizationInstance::Assign( const ProcessImplementation& p )
{
   const LocalNormalizationInstance* x = dynamic_cast<const LocalNormalizationInstance*>( &p );
   if ( x != nullptr )
   {
      p_scale                       = x->p_scale;
      p_noScale                     = x->p_noScale;
      p_rejection                   = x->p_rejection;
      p_backgroundRejectionLimit    = x->p_backgroundRejectionLimit;
      p_referenceRejectionThreshold = x->p_referenceRejectionThreshold;
      p_targetRejectionThreshold    = x->p_targetRejectionThreshold;
      p_hotPixelFilterRadius        = x->p_hotPixelFilterRadius;
      p_noiseReductionFilterRadius  = x->p_noiseReductionFilterRadius;
      p_referencePathOrViewId       = x->p_referencePathOrViewId;
      p_referenceIsView             = x->p_referenceIsView;
      p_targets                     = x->p_targets;
      p_inputHints                  = x->p_inputHints;
      p_outputHints                 = x->p_outputHints;
      p_generateNormalizedImages    = x->p_generateNormalizedImages;
      p_generateNormalizationData   = x->p_generateNormalizationData;
      p_showBackgroundModels        = x->p_showBackgroundModels;
      p_showRejectionMaps           = x->p_showRejectionMaps;
      p_plotNormalizationFunctions  = x->p_plotNormalizationFunctions;
      p_noGUIMessages               = x->p_noGUIMessages;
      p_outputDirectory             = x->p_outputDirectory;
      p_outputExtension             = x->p_outputExtension;
      p_outputPrefix                = x->p_outputPrefix;
      p_outputPostfix               = x->p_outputPostfix;
      p_overwriteExistingFiles      = x->p_overwriteExistingFiles;
      p_onError                     = x->p_onError;
      p_useFileThreads              = x->p_useFileThreads;
      p_fileThreadOverload          = x->p_fileThreadOverload;
      p_maxFileReadThreads          = x->p_maxFileReadThreads;
      p_maxFileWriteThreads         = x->p_maxFileWriteThreads;
      p_graphSize                   = x->p_graphSize;
      p_graphTextSize               = x->p_graphTextSize;
      p_graphTitleSize              = x->p_graphTitleSize;
      p_graphTransparent            = x->p_graphTransparent;
      p_graphOutputDirectory        = x->p_graphOutputDirectory;
   }
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::IsHistoryUpdater( const View& view ) const
{
   return p_generateNormalizedImages == LNGenerateNormalizedImages::ViewExecutionOnly ||
          p_generateNormalizedImages == LNGenerateNormalizedImages::Always;
}

// ----------------------------------------------------------------------------

UndoFlags LocalNormalizationInstance::UndoMode( const View& view ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static void RunGnuplot( const String& gnuFilePath )
{
   Console console;

   String gnuplotPath = PixInsightSettings::GlobalString( "Application/BinDirectory" ) + "/gnuplot";
#ifdef __PCL_WINDOWS
   gnuplotPath << ".exe";
#endif
   if ( !File::Exists( gnuplotPath ) )
      throw Error( "The gnuplot executable does not exist: " + gnuplotPath );

   ExternalProcess P;
   P.Start( gnuplotPath, StringList() << gnuFilePath );
   if ( !P.WaitForStarted() )
      throw Error( "Unable to launch gnuplot executable: " + gnuplotPath );

   Module->ProcessEvents();

   console.Write( "<end><cbr>Running gnuplot:  " );
   for ( int n = 0; P.IsRunning() && !P.WaitForFinished( 250 ); ++n )
   {
      console.Write( String( "<end>\b" ) + "-/|\\"[n%4] );
      Module->ProcessEvents();
      if ( console.AbortRequested() )
      {
         console.NoteLn( "<end><cbr>* Abort requested - killing process." );
         P.Kill();
         throw ProcessAborted();
      }
   }
   console.WriteLn( "<end>\bdone" );

   if ( P.HasCrashed() )
      throw Error( "The gnuplot executable has crashed: " + gnuplotPath );
   if ( P.ExitCode() != 0 )
      throw Error( "Gnuplot rendering failed. Exit code = " + String( P.ExitCode() ) );
}

// ----------------------------------------------------------------------------

class LocalNormalizationThread : public Thread
{
public:

   typedef ShepardInterpolation<double>   background_interpolation;

   typedef GridShepardInterpolation       background_model;

   typedef Array<background_model>        background_models;

   LocalNormalizationThread( const LocalNormalizationInstance& instance,
                             const ImageVariant& referenceImage,
                             const String& targetFilePath ) :
      m_instance( instance ),
      m_referenceImage( referenceImage ),
      m_targetFilePath( targetFilePath )
   {
      if ( m_instance.p_referenceIsView )
         m_referenceFilePath = View::ViewById( m_instance.p_referencePathOrViewId ).Window().FilePath();
      else
         m_referenceFilePath = m_instance.p_referencePathOrViewId;
   }

   LocalNormalizationThread( const LocalNormalizationInstance& instance,
                             const ImageVariant& referenceImage,
                             ImageVariant& targetImage ) :
      m_instance( instance ),
      m_referenceImage( referenceImage ),
      m_targetImage( targetImage )
   {
      m_targetImage.SetOwnership( false );
   }

   virtual void Run()
   {
      Console console;

      try
      {
         Module->ProcessEvents();

         if ( !ReadInputData() )
            return;

         Build();

         if ( GeneratesNormalizedImages() )
            Apply();

         WriteOutputData();

         m_success = true;
      }
      catch ( ... )
      {
         if ( IsRootThread() )
            throw;

         String text = ConsoleOutputText();
         ClearConsoleOutputText();
         try
         {
            throw;
         }
         ERROR_HANDLER
         m_errorInfo = ConsoleOutputText();
         ClearConsoleOutputText();
         console.Write( text );
      }
   }

   bool ExecutedOnView() const
   {
      return m_targetFilePath.IsEmpty();
   }

   bool GeneratesNormalizedImages() const
   {
      return m_instance.p_generateNormalizedImages == LNGenerateNormalizedImages::Always ||
             m_instance.p_generateNormalizedImages == (ExecutedOnView() ?
                                    LNGenerateNormalizedImages::ViewExecutionOnly :
                                    LNGenerateNormalizedImages::GlobalExecutionOnly);
   }

   String TargetFilePath() const
   {
      return m_targetFilePath;
   }

   String OutputFilePath() const
   {
      return m_outputFilePath;
   }

   bool Succeeded() const
   {
      return m_success;
   }

   String ErrorInfo() const
   {
      return m_errorInfo;
   }

private:

   const LocalNormalizationInstance& m_instance;
   const ImageVariant&               m_referenceImage;
         String                      m_referenceFilePath;
         String                      m_targetFilePath;
         OutputFileData              m_fileData;
         ImageVariant                m_targetImage;
         Image                       m_RB;
         DImage                      m_A1;
         DImage                      m_A0;
         String                      m_outputFilePath;
         StatusMonitor               m_monitor;
         String                      m_errorInfo;
         bool                        m_success = false;

   // -------------------------------------------------------------------------

   template <class P>
   static void CreateImageWindow( const GenericImage<P>& image, const IsoString& id )
   {
      ImageWindow window( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false/*color*/, true/*processing*/, id );
      window.MainView().Image().CopyImage( image );
      window.Show();
      window.ZoomToFit( false/*zoomIn*/ );
   }

   // -------------------------------------------------------------------------

   class FixZeroThread : public Thread
   {
   public:

      FixZeroThread( Image& image, const background_model& G, int channel, int startRow, int endRow ) :
         m_image( image ),
         m_G( G ),
         m_channel( channel ),
         m_startRow( startRow ),
         m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         Image::sample_iterator r( m_image, m_channel );
         r.MoveBy( 0, m_startRow );
         for ( int y = m_startRow; y < m_endRow; ++y )
            for ( int x = 0; x < m_image.Width(); ++x, ++r )
               if ( *r == 0 )
                  *r = m_G( x, y );
      }

   private:

      Image&                  m_image;
      const background_model& m_G;
      int                     m_channel;
      int                     m_startRow;
      int                     m_endRow;
   };

   background_models FixZero( Image& image, int delta = 40 )   // N
   {
      const int w  = image.Width();
      const int h  = image.Height();
      const int dx = RoundInt( w/Ceil( double( w )/delta ) );
      const int dy = RoundInt( h/Ceil( double( h )/delta ) );
      const int dx2 = dx >> 1;
      const int dy2 = dy >> 1;

      image.SetRangeClipping( 0, 0.92 );

      background_models B;

      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         Array<double> X0, Y0, Z0;
         for ( int y = dy2; y < h; y += dy )
            for ( int x = dx2; x < w; x += dx )
               if ( image( x, y, c ) != 0 )
               {
                  double z = image.Median( Rect( x-dx2, y-dy2, x+dx2+1, y+dy2+1 ), c, c, 1 );
                  if ( 1 + z != 1 )
                  {
                     X0 << x;
                     Y0 << y;
                     Z0 << z;
                  }
               }

         double m = NondestructiveMedian( Z0.Begin(), Z0.End() );
         double s = 3*1.5*MAD( Z0.Begin(), Z0.End(), m );
         Array<double> X, Y, Z;
         for ( size_type i = 0; i < Z0.Length(); ++i )
            if ( Abs( Z0[i] - m ) < s )
            {
               X << X0[i];
               Y << Y0[i];
               Z << Z0[i];
            }

         if ( X.Length() < 16 )
            throw Error( "LocalNormalizationThread::FixZero(): Insufficient data to sample background image pixels, channel " + String( c ) );

         background_interpolation S;
         S.SetRadius( 0.1 );
         S.Initialize( X.Begin(), Y.Begin(), Z.Begin(), X.Length() );

         background_model G;
         G.Initialize( image.Bounds(), 16/*delta*/, S, false/*verbose*/ );

         int numberOfThreads = Thread::NumberOfThreads( image.Height(), 4 );
         int rowsPerThread = image.Height()/numberOfThreads;
         ReferenceArray<FixZeroThread> threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads << new FixZeroThread( image, G, c,
                                 i*rowsPerThread,
                                 (j < numberOfThreads) ? j*rowsPerThread : image.Height() );
         if ( numberOfThreads > 1 )
         {
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i].Start( ThreadPriority::DefaultMax, i );
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i].Wait();
         }
         else
            threads[0].Run();

         threads.Destroy();

         B << G;

         m_monitor += image.NumberOfPixels();
      }

      return B;
   }

   // -------------------------------------------------------------------------

   class RejectThread : public Thread
   {
   public:

      RejectThread( AbstractImage::ThreadData& data,
                    const LocalNormalizationInstance& instance,
                    UInt8Image& Rr, UInt8Image& Tr,
                    const Image& R, const Image& T,
                    const background_model& Rz, const background_model& Tz,
                    int channel, int startRow, int endRow ) :
         m_data( data ),
         m_instance( instance ),
         m_Rr( Rr ),
         m_Tr( Tr ),
         m_R( R ),
         m_T( T ),
         m_Rz( Rz ),
         m_Tz( Tz ),
         m_channel( channel ),
         m_startRow( startRow ),
         m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         Image::const_sample_iterator r( m_R, m_channel );
         Image::const_sample_iterator t( m_T, m_channel );
         UInt8Image::sample_iterator rr( m_Rr );
         UInt8Image::sample_iterator tr( m_Tr );
         r.MoveBy( 0, m_startRow );
         t.MoveBy( 0, m_startRow );
         rr.MoveBy( 0, m_startRow );
         tr.MoveBy( 0, m_startRow );
         for ( int y = m_startRow; y < m_endRow; ++y )
            for ( int x = 0; x < m_R.Width(); ++x, ++r, ++t, ++rr, ++tr )
            {
               double rb = m_Rz( x, y );
               double tb = m_Tz( x, y );
               double rk = Abs( *r - rb )/rb;
               double tk = Abs( *t - tb )/tb;
               *rr = tk < m_instance.p_backgroundRejectionLimit && rk > m_instance.p_referenceRejectionThreshold;
               *tr = rk < m_instance.p_backgroundRejectionLimit && tk > m_instance.p_targetRejectionThreshold;

               UPDATE_THREAD_MONITOR( 65536 )
            }
      }

   private:

            AbstractImage::ThreadData&  m_data;
      const LocalNormalizationInstance& m_instance;
            UInt8Image&                 m_Rr;
            UInt8Image&                 m_Tr;
      const Image&                      m_R;
      const Image&                      m_T;
      const background_model&           m_Rz;
      const background_model&           m_Tz;
            int                         m_channel;
            int                         m_startRow;
            int                         m_endRow;
   };

   void Reject( Image& T, Image& R )   // 9*N
   {
      /*
       * Setup rejection map images if requested.
       */
      UInt8Image Rmap, Tmap;
      bool haveMaps = false;
      if ( m_instance.p_rejection )
         if ( m_instance.p_showRejectionMaps )
            if ( ExecutedOnView() )
            {
               Rmap.AllocateData( R.Width(), R.Height(), R.NumberOfNominalChannels(), R.ColorSpace() ).Fill( uint8( 0 ) );
               Tmap.AllocateData( T.Width(), T.Height(), T.NumberOfNominalChannels(), T.ColorSpace() ).Fill( uint8( 0 ) );
               haveMaps = true;
            }

      /*
       * Initial exclusion of black or insignificant pixel samples.
       */
      for ( int c = 0; c < R.NumberOfChannels(); ++c )
      {
         UInt8Image::sample_iterator rm( Rmap, c );
         UInt8Image::sample_iterator tm( Tmap, c );
         for ( Image::sample_iterator r( R, c ), t( T, c ); r; ++r, ++t )
         {
            if ( *r < TINY_SAMPLE_VALUE || *t < TINY_SAMPLE_VALUE )
               *r = *t = 0;

            if ( haveMaps )
            {
               if ( *r == 0 )
                  *rm = *tm = uint8( 0xff );
               ++rm, ++tm;
            }
         }
      }

      /*
       * Initial approximate background models and replacement of black pixels.
       */
      background_models Rz = FixZero( R );   // N
      background_models Tz = FixZero( T );   // N
//       {
//          Image RZ, TZ;
//          RZ.AllocateData( R.Width(), R.Height(), 1 );
//          TZ.AllocateData( T.Width(), T.Height(), 1 );
//          Image::sample_iterator r( RZ ), t( TZ );
//          for ( int y = 0; y < R.Height(); ++y )
//             for ( int x = 0; x < R.Width(); ++x, ++r, ++t )
//             {
//                *r = Rz[0]( x, y );
//                *t = Tz[0]( x, y );
//             }
//
//          CreateImageWindow( RZ, "LN_RZ" );
//          CreateImageWindow( TZ, "LN_TZ" );
//       }

      /*
       * Optional hot/cold pixel removal.
       */
      if ( m_instance.p_hotPixelFilterRadius > 0 ) // 2*N
      {
         MorphologicalTransformation M;
         M.SetOperator( MedianFilter() );
         if ( m_instance.p_hotPixelFilterRadius > 1 )
            M.SetStructure( CircularStructure( 2*m_instance.p_hotPixelFilterRadius + 1 ) );
         else
            M.SetStructure( BoxStructure( 3 ) );

         M >> R; m_monitor += R.NumberOfSamples();
         M >> T; m_monitor += R.NumberOfSamples();
      }
      else
         m_monitor += 2*R.NumberOfSamples();

      /*
       * Optional noise reduction.
       */
      if ( m_instance.p_noiseReductionFilterRadius > 0 ) // 2*N
      {
         SeparableConvolution C( GaussianFilter( 2*m_instance.p_noiseReductionFilterRadius + 1 ).AsSeparableFilter() );
         C >> R; m_monitor += R.NumberOfSamples();
         C >> T; m_monitor += R.NumberOfSamples();
      }
      else
         m_monitor += 2*R.NumberOfSamples();

      /*
       * Outlier rejection.
       */
      if ( m_instance.p_rejection )
      {
         for ( int c = 0; c < T.NumberOfChannels(); ++c )   // N
         {
            UInt8Image Rr( R.Width(), R.Height() );
            UInt8Image Tr( T.Width(), T.Height() );

            int numberOfThreads = Thread::NumberOfThreads( R.Height(), 4 );
            int rowsPerThread = R.Height()/numberOfThreads;
            AbstractImage::ThreadData data( m_monitor, T.NumberOfPixels() );
            ReferenceArray<RejectThread> threads;
            for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
               threads << new RejectThread( data, m_instance, Rr, Tr, R, T, Rz[c], Tz[c], c,
                                    i*rowsPerThread,
                                    (j < numberOfThreads) ? j*rowsPerThread : R.Height() );
            AbstractImage::RunThreads( threads, data );
            threads.Destroy();

            MorphologicalTransformation M( DilationFilter(), BoxStructure( 3 ) );
            M >> Rr;
            M >> Tr;

            Image::sample_iterator r( R, c );
            Image::sample_iterator t( T, c );
            UInt8Image::sample_iterator rm( Rmap, c );
            UInt8Image::sample_iterator tm( Tmap, c );
            UInt8Image::const_sample_iterator rr( Rr );
            UInt8Image::const_sample_iterator tr( Tr );
            for ( ; r; ++r, ++t, ++rr, ++tr )
            {
               if ( *rr )
                  *r = 0;
               if ( *tr )
                  *t = 0;

               if ( haveMaps )
               {
                  if ( *rr )
                     *rm = uint8( 0xff );
                  if ( *tr )
                     *tm = uint8( 0xff );
                  ++rm, ++tm;
               }
            }
         }

         FixZero( R );  // N
         FixZero( T );  // N

         if ( haveMaps )
         {
            CreateImageWindow( Rmap, "LN_rmap_r" );
            CreateImageWindow( Tmap, "LN_rmap_t" );
         }
      }
      else
         m_monitor += 3*R.NumberOfSamples();
   }

   // -------------------------------------------------------------------------

   Image Background( const Image& image, int scalePx ) // N
   {
      // Accelerated multiscale median transform with linear scaling
      MultiscaleMedianTransform M( Max( 1, RoundInt( scalePx/32.0 ) ), 16 );
      //M.DisableMultiwayStructures();
      for ( int i = 0; i < M.NumberOfLayers(); ++i )
         M.DisableLayer( i );
      M << image;
      m_monitor += image.NumberOfSamples();
      return M[M.NumberOfLayers()];
   }

   // -------------------------------------------------------------------------

   static int TicsDelta( int size )
   {
      int x = size/8;
      return x - x%TruncInt( Pow10I<double>()( TruncInt( Log( double( x ) ) ) ) );
   }

   void Plot( int component )
   {
      const DImage& image = (component == 0) ? m_A0 : m_A1;
      const String fileSuffix = (component == 0) ? "_a0" : "_a1";
      const IsoString windowSuffix = (component == 0) ? "_offset" : "_scale";
      const IsoString titleComponent = (component == 0) ? "Offset" : "Scale";
      const String tmpDir = File::SystemTempDirectory();
      const double sx = double( image.Width() )/m_referenceImage.Width();
      const double sy = double( image.Height() )/m_referenceImage.Height();
      const double gridStep = Max( m_referenceImage.Width(), m_referenceImage.Height() )/64;
      const int xGridSize = RoundInt( m_referenceImage.Width()/gridStep );
      const int yGridSize = RoundInt( m_referenceImage.Height()/gridStep );

      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         Module->ProcessEvents();

         BicubicBSplineInterpolation<double> A;
         A.Initialize( image[c], image.Width(), image.Height() );
         IsoString text;
         for ( int x = 0, n = 0; x < xGridSize; ++x )
            for ( int y = 0; y < yGridSize; ++y, ++n )
            {
               double nx = x*gridStep;
               double ny = y*gridStep;
               text << IsoString( nx ) << ' ' << IsoString( ny ) << ' ' << IsoString( A( sx*nx, sy*ny ) ) << '\n';
            }
         String datFilePath = tmpDir + "/ln" + fileSuffix + ".dat";
         File::WriteTextFile( datFilePath, text );

         String outputDir = m_instance.p_graphOutputDirectory.Trimmed();
         if ( outputDir.IsEmpty() )
            outputDir = tmpDir;
         else
         {
            if ( !File::DirectoryExists( outputDir ) )
            {
               Console console;
               console.WarningLn( "** Warning: The specified graph output directory does not exist: <raw>" + outputDir + "</raw>" );
               console.WarningLn( "Will use the system temp directory instead: <raw>" + tmpDir + "</raw>" );
            }
         }
         if ( !outputDir.EndsWith( '/' ) )
            outputDir << '/';

         String gnuFilePath = tmpDir + '/' + "ln" + fileSuffix + ".gnu";
         String svgFilePath = outputDir + "ln" + fileSuffix + ".svg";

         text.Clear();
         text << "set terminal svg size ";
         if ( m_instance.p_plotNormalizationFunctions == LNPlotNormalizationFunctions::Map3D )
         {
            if ( m_referenceImage.Width() >= m_referenceImage.Height() )
               text << IsoString( m_instance.p_graphSize ) << ','
                    << IsoString( RoundInt( m_referenceImage.Height()*double( m_instance.p_graphSize )/m_referenceImage.Width() ) );
            else
               text << IsoString( RoundInt( m_referenceImage.Width()*double( m_instance.p_graphSize )/m_referenceImage.Height() ) ) << ','
                    << IsoString( m_instance.p_graphSize );
         }
         else
            text << IsoString( m_instance.p_graphSize ) << ',' << IsoString( m_instance.p_graphSize );
         text << " font \"helvetica," << IsoString( m_instance.p_graphTextSize ) << "\" enhanced";
         if ( !m_instance.p_graphTransparent )
            text << " background \"#FFFFFF\"";
         text << '\n';

         text << "set samples " << IsoString( xGridSize ) << "," << IsoString( yGridSize ) << '\n'
              << "set isosamples " << IsoString( xGridSize ) << "," << IsoString( yGridSize ) << '\n'
              << "set dgrid3d " << IsoString( xGridSize ) << "," << IsoString( yGridSize ) << '\n'
              << "set xyplane 0\n"
              << "set hidden3d\n"
              << "set margins 0,0,0,0\n"
              << "set yrange [] reverse\n"
              << "set xtics 0," << IsoString( TicsDelta( m_referenceImage.Width() ) ) << "," << IsoString( m_referenceImage.Width()-1 ) << '\n'
              << "set ytics 0," << IsoString( TicsDelta( m_referenceImage.Height() ) ) << "," << IsoString( m_referenceImage.Height()-1 ) << '\n';

         if ( m_instance.p_plotNormalizationFunctions != LNPlotNormalizationFunctions::Map3D )
            text << "set xtics offset character 0,-0.25\n"
                 << "set ytics offset character 1.75\n"
                 << "set border 895\n";

         if ( m_instance.p_plotNormalizationFunctions != LNPlotNormalizationFunctions::Line3D )
            text << "set pm3d interpolate 0,0\n";

         if ( m_instance.p_plotNormalizationFunctions == LNPlotNormalizationFunctions::Map3D )
            text << "set view map scale 1\n";

         text << "set grid xtics ytics ztics\n"
              << "set contour base\n"
              << "set title \"Local Normalization " << titleComponent << " Component, Channel " << IsoString( c ) << '\"'
                  << " font \"helvetica," << IsoString( m_instance.p_graphTitleSize ) << "\" offset character 0,0.5\n"
              << "unset key\n"
              << "set output \"" << svgFilePath.ToUTF8() << "\"\n"
              << "splot \"" << datFilePath.ToUTF8() << "\" using 1:2:3 with ";

         if ( m_instance.p_plotNormalizationFunctions == LNPlotNormalizationFunctions::Line3D )
            text << "lines lc rgbcolor \"#E00000\"\n";
         else
            text << "pm3d\n";

         File::WriteTextFile( gnuFilePath, text );

         RunGnuplot( gnuFilePath );

         {
            Bitmap bmp( svgFilePath );
            UInt8Image image;
            image.AllocateData( bmp.Width(), bmp.Height(), m_instance.p_graphTransparent ? 4 : 3, ColorSpace::RGB );
            image.Blend( bmp );
            CreateImageWindow( image, "LN" + windowSuffix );
         }
      }
   }

   // -------------------------------------------------------------------------

   class BuildThread : public Thread
   {
   public:

      BuildThread( AbstractImage::ThreadData& data,
                   const Image& R, const Image& T, const Image& RB, const Image& TB,
                   DImage& A0, DImage& A1, bool noScale, distance_type start, distance_type end ) :
         m_data( data ),
         m_R( R ), m_T( T ), m_RB( RB ), m_TB( TB ),
         m_A0( A0 ), m_A1( A1 ), m_noScale( noScale ),
         m_start( start ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         for ( int c = 0; c < m_R.NumberOfChannels(); ++c )
         {
            Image::const_sample_iterator r( m_R, c ), t( m_T, c ), rb( m_RB, c ), tb( m_TB, c );
            DImage::sample_iterator a0( m_A0, c ), a1( m_A1, c );
            r += m_start; t += m_start; rb += m_start; tb += m_start;
            a0 += m_start; a1 += m_start;
            for ( distance_type p = m_start; p < m_end; ++p, ++r, ++t, ++rb, ++tb, ++a0, ++a1 )
            {
               *a0 = *rb - *tb;
               *a1 = m_noScale ? 1.0 : (*r  - *a0) / *t;

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

            AbstractImage::ThreadData& m_data;
      const Image&                     m_R;
      const Image&                     m_T;
      const Image&                     m_RB;
      const Image&                     m_TB;
            DImage&                    m_A0;
            DImage&                    m_A1;
            bool                       m_noScale;
            distance_type              m_start;
            distance_type              m_end;
   };

   template <class P1, class P2>
   void Build( const GenericImage<P1>& target, const GenericImage<P2>& reference )
   {
      if ( target.Bounds() != reference.Bounds() || target.NumberOfNominalChannels() != reference.NumberOfNominalChannels() )
         throw Error( "LocalNormalizationThread::Build(): Internal error: Incompatible image geometries." );

      reference.SelectNominalChannels();
      target.SelectNominalChannels();

      Image R = reference;
      Image T = target;

      reference.ResetSelections();
      target.ResetSelections();

      StandardStatus status;
      m_monitor.SetCallback( &status );
      m_monitor.Initialize( "Building local normalization functions", 15*R.NumberOfSamples() );

      Reject( T, R ); // 9*N

      Image RB = Background( R, m_instance.p_scale ); // N
      Image TB = Background( T, m_instance.p_scale ); // N

      if ( m_instance.p_showBackgroundModels )
         if ( ExecutedOnView() )
         {
            CreateImageWindow( RB, "LN_background_r" );
            CreateImageWindow( TB, "LN_background_t" );
         }

      m_A0.AllocateData( R.Width(), R.Height(), R.NumberOfChannels() );
      m_A1.AllocateData( R.Width(), R.Height(), R.NumberOfChannels() );

      int numberOfThreads = Thread::NumberOfThreads( R.NumberOfPixels(), 256 );
      int pixelsPerThread = R.NumberOfPixels()/numberOfThreads;
      AbstractImage::ThreadData data( m_monitor, R.NumberOfSamples() );
      ReferenceArray<BuildThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads << new BuildThread( data, R, T, RB, TB, m_A0, m_A1, m_instance.p_noScale,
                                     i*pixelsPerThread,
                                     (j < numberOfThreads) ? j*pixelsPerThread : R.NumberOfPixels() );
      AbstractImage::RunThreads( threads, data ); // N
      threads.Destroy();

      {
         MorphologicalTransformation M;
         M.SetOperator( MedianFilter() );
         M.SetStructure( BoxStructure( 3 ) );
         M >> m_A1; m_monitor += m_A1.NumberOfSamples(); // N
      }

      {
         BicubicFilterPixelInterpolation BF( m_instance.p_scale, m_instance.p_scale, CubicBSplineFilter() );
         Resample R( BF, 2.0/m_instance.p_scale );
         R.EnableUnclippedInterpolation();
         R >> m_A0; m_monitor += m_A0.NumberOfSamples(); // N
         R >> m_A1; m_monitor += m_A1.NumberOfSamples(); // N
      }

      m_monitor.Complete();

      if ( m_instance.p_plotNormalizationFunctions != LNPlotNormalizationFunctions::DontPlot )
         if ( ExecutedOnView() )
         {
            Plot( 0 );
            if ( !m_instance.p_noScale )
               Plot( 1 );
         }
   }

   template <class P>
   void Build( const GenericImage<P>& reference )
   {
      if ( m_targetImage.IsFloatSample() )
         switch ( m_targetImage.BitsPerSample() )
         {
         case 32: Build( static_cast<const Image&>( *m_targetImage ), reference ); break;
         case 64: Build( static_cast<const DImage&>( *m_targetImage ), reference ); break;
         }
      else
         switch ( m_targetImage.BitsPerSample() )
         {
         case  8: Build( static_cast<const UInt8Image&>( *m_targetImage ), reference ); break;
         case 16: Build( static_cast<const UInt16Image&>( *m_targetImage ), reference ); break;
         case 32: Build( static_cast<const UInt32Image&>( *m_targetImage ), reference ); break;
         }
   }

   void Build()
   {
      if ( m_referenceImage.IsFloatSample() )
         switch ( m_referenceImage.BitsPerSample() )
         {
         case 32: Build( static_cast<const Image&>( *m_referenceImage ) ); break;
         case 64: Build( static_cast<const DImage&>( *m_referenceImage ) ); break;
         }
      else
         switch ( m_referenceImage.BitsPerSample() )
         {
         case  8: Build( static_cast<const UInt8Image&>( *m_referenceImage ) ); break;
         case 16: Build( static_cast<const UInt16Image&>( *m_referenceImage ) ); break;
         case 32: Build( static_cast<const UInt32Image&>( *m_referenceImage ) ); break;
         }
   }

   // -------------------------------------------------------------------------

   template <class P>
   class ApplyThread : public Thread
   {
   public:

      ApplyThread( AbstractImage::ThreadData& data,
                   GenericImage<P>& target, const DImage& A0, const DImage& A1,
                   int startRow, int endRow ) :
         m_data( data ),
         m_target( target ),
         m_A0( A0 ),
         m_A1( A1 ),
         m_startRow( startRow ),
         m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         double sx = double( m_A1.Width() )/m_target.Width();
         double sy = double( m_A1.Height() )/m_target.Height();

         for ( int c = 0; c < m_target.NumberOfNominalChannels(); ++c )
         {
            BicubicBSplineInterpolation<double> A0;
            A0.Initialize( m_A0[c], m_A0.Width(), m_A0.Height() );
            BicubicBSplineInterpolation<double> A1;
            A1.Initialize( m_A1[c], m_A1.Width(), m_A1.Height() );

            typename GenericImage<P>::sample_iterator t( m_target, c );
            t.MoveBy( 0, m_startRow );
            for ( int y = m_startRow; y < m_endRow; ++y )
            {
               double ys = sy*y;
               for ( int x = 0; x < m_target.Width(); ++x, ++t )
                  if ( *t != P::MinSampleValue() )
                  {
                     double f; P::FromSample( f, *t );
                     double xs = sx*x;
                     *t = P::ToSample( Range( A0( xs, ys ) + A1( xs, ys )*f, 0.0, 1.0 ) );

                     UPDATE_THREAD_MONITOR( 65536 )
                  }
            }
         }
      }

   private:

            AbstractImage::ThreadData& m_data;
            GenericImage<P>&           m_target;
      const DImage&                    m_A0;
      const DImage&                    m_A1;
            int                        m_startRow;
            int                        m_endRow;
   };

   template <class P>
   void Apply( GenericImage<P>& target )
   {
      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Applying local normalization", target.NumberOfSamples() );

      int numberOfThreads = Thread::NumberOfThreads( target.Height(), 4 );
      int rowsPerThread = target.Height()/numberOfThreads;
      AbstractImage::ThreadData data( monitor, target.NumberOfSamples() );
      ReferenceArray<ApplyThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads << new ApplyThread<P>( data, target, m_A0, m_A1,
                              i*rowsPerThread,
                              (j < numberOfThreads) ? j*rowsPerThread : target.Height() );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
   }

   void Apply()
   {
      if ( m_targetImage.IsFloatSample() )
         switch ( m_targetImage.BitsPerSample() )
         {
         case 32: Apply( static_cast<Image&>( *m_targetImage ) ); break;
         case 64: Apply( static_cast<DImage&>( *m_targetImage ) ); break;
         }
      else
         switch ( m_targetImage.BitsPerSample() )
         {
         case  8: Apply( static_cast<UInt8Image&>( *m_targetImage ) ); break;
         case 16: Apply( static_cast<UInt16Image&>( *m_targetImage ) ); break;
         case 32: Apply( static_cast<UInt32Image&>( *m_targetImage ) ); break;
         }
   }

   // -------------------------------------------------------------------------

   bool ReadInputData()
   {
      if ( m_targetImage )
         return true;

      static Mutex mutex;
      static AtomicInt count;

      Console console;
      console.WriteLn( "<end><cbr>Loading target file:" );
      console.WriteLn( "<raw>" + m_targetFilePath + "</raw>" );

      FileFormat format( File::ExtractExtension( m_targetFilePath ), true/*read*/, false/*write*/ );
      FileFormatInstance file( format );

      ImageDescriptionArray images;

      if ( !file.Open( images, m_targetFilePath, m_instance.p_inputHints ) )
         throw CaughtException();

      if ( images.IsEmpty() )
      {
         console.NoteLn( "* Skipping empty image file." );
         if ( !file.Close() )
            throw CaughtException();
         return false;
      }

      if ( images.Length() > 1 )
         console.NoteLn( String().Format( "* Ignoring %u additional image(s) in target file.", images.Length()-1 ) );

      m_targetImage.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                                       false/*isComplex*/,
                                       images[0].options.bitsPerSample );
      {
         volatile AutoLockCounter lock( mutex, count, m_instance.p_maxFileReadThreads );
         if ( !file.ReadImage( m_targetImage ) )
            throw CaughtException();
         m_fileData = OutputFileData( file, images[0].options );
         if ( !file.Close() )
            throw CaughtException();
      }

      return true;
   }

   // -------------------------------------------------------------------------

   void WriteOutputData()
   {
      static Mutex mutex;
      static AtomicInt count;

      if ( m_targetFilePath.IsEmpty() )
         return;

      String fileDir = m_instance.p_outputDirectory.Trimmed();
      if ( fileDir.IsEmpty() )
         fileDir = File::ExtractDrive( m_targetFilePath ) + File::ExtractDirectory( m_targetFilePath );
      if ( fileDir.IsEmpty() )
         throw Error( "Unable to determine an output directory: " + m_targetFilePath );
      if ( !fileDir.EndsWith( '/' ) )
         fileDir.Append( '/' );

      String fileName = File::ExtractName( m_targetFilePath ).Trimmed();
      if ( fileName.IsEmpty() )
         throw Error( "Unable to determine an output file name: " + m_targetFilePath );

      Console console;

      if ( m_instance.p_generateNormalizationData )
      {
         Module->ProcessEvents();

         String xnmlFilePath = fileDir + fileName + ".xnml";
         console.WriteLn( "<end><cbr>Writing output file: " + xnmlFilePath );
         if ( File::Exists( xnmlFilePath ) )
            if ( m_instance.p_overwriteExistingFiles )
               console.WarningLn( "** Warning: Overwriting existing file" );
            else
            {
               xnmlFilePath = UniqueFilePath( xnmlFilePath );
               console.NoteLn( "* File already exists, writing to: " + xnmlFilePath );
            }

         LocalNormalizationData data;
         data.SetReferenceFilePath( m_referenceFilePath );
         data.SetTargetFilePath( m_targetFilePath );
         data.SetNormalizationScale( m_instance.p_scale );
         data.SetReferenceDimensions( m_referenceImage.Width(), m_referenceImage.Height() );
         data.SetNormalizationMatrices( m_A1, m_A0 );
         {
            volatile AutoLockCounter lock( mutex, count, m_instance.p_maxFileWriteThreads );
            data.SerializeToFile( xnmlFilePath );
         }
      }

      if ( GeneratesNormalizedImages() )
      {
         String fileExtension = m_instance.p_outputExtension.Trimmed();
         if ( fileExtension.IsEmpty() )
            fileExtension = File::ExtractExtension( m_targetFilePath ).Trimmed();
         if ( fileExtension.IsEmpty() )
            throw Error( "Unable to determine an output file extension: " + m_targetFilePath );
         if ( !fileExtension.StartsWith( '.' ) )
            fileExtension.Prepend( '.' );

         m_outputFilePath = fileDir + m_instance.p_outputPrefix + fileName + m_instance.p_outputPostfix + fileExtension;

         console.WriteLn( "<end><cbr>Writing output file: " + m_outputFilePath );

         if ( File::Exists( m_outputFilePath ) )
            if ( m_instance.p_overwriteExistingFiles )
               console.WarningLn( "** Warning: Overwriting existing file" );
            else
            {
               m_outputFilePath = UniqueFilePath( m_outputFilePath );
               console.NoteLn( "* File already exists, writing to: " + m_outputFilePath );
            }

         FileFormat outputFormat( fileExtension, false/*read*/, true/*write*/ );
         FileFormatInstance outputFile( outputFormat );

         if ( !outputFile.Create( m_outputFilePath, m_instance.p_outputHints ) )
            throw CaughtException();

         ImageOptions options = m_fileData.options; // get resolution, etc.
         options.bitsPerSample = m_targetImage.BitsPerSample();
         options.ieeefpSampleFormat = m_targetImage.IsFloatSample();
         outputFile.SetOptions( options );

         bool canStore = true;
         if ( m_targetImage.IsFloatSample() )
            switch ( m_targetImage.BitsPerSample() )
            {
            case 32: canStore = outputFormat.CanStoreFloat(); break;
            case 64: canStore = outputFormat.CanStoreDouble(); break;
            }
         else
            switch ( m_targetImage.BitsPerSample() )
            {
            case  8: canStore = outputFormat.CanStore8Bit(); break;
            case 16: canStore = outputFormat.CanStore16Bit(); break;
            case 32: canStore = outputFormat.CanStore32Bit(); break;
            case 64: canStore = outputFormat.CanStore64Bit(); break;
            }

         if ( !canStore )
            console.WarningLn( "** Warning: The output format does not support the required sample data format." );

         if ( m_fileData.fsData != nullptr )
            if ( outputFormat.UsesFormatSpecificData() )
               if ( outputFormat.ValidateFormatSpecificData( m_fileData.fsData ) )
                  outputFile.SetFormatSpecificData( m_fileData.fsData );

         if ( !m_fileData.properties.IsEmpty() )
            if ( outputFormat.CanStoreImageProperties() )
               outputFile.WriteImageProperties( m_fileData.properties );
            else
               console.WarningLn( "** Warning: The output format cannot store image properties - existing properties not embedded." );

         if ( outputFormat.CanStoreKeywords() )
         {
            FITSKeywordArray keywords = m_fileData.keywords;
            keywords << FITSHeaderKeyword( "COMMENT",  IsoString(), "Normalization with " + PixInsightVersion::AsString() )
                     << FITSHeaderKeyword( "HISTORY",  IsoString(), "Normalization with " + Module->ReadableVersion() )
                     << FITSHeaderKeyword( "HISTORY",  IsoString(), "Normalization with LocalNormalization process" );
            outputFile.WriteFITSKeywords( keywords );
         }
         else
            console.WarningLn( "** Warning: The output format cannot store FITS keywords - keywords not embedded." );

         if ( m_fileData.profile.IsProfile() )
            if ( outputFormat.CanStoreICCProfiles() )
               outputFile.WriteICCProfile( m_fileData.profile );
            else
               console.WarningLn( "** Warning: The output format cannot store ICC profiles - existing profile not embedded." );

         Module->ProcessEvents();

         {
            volatile AutoLockCounter lock( mutex, count, m_instance.p_maxFileWriteThreads );
            if ( !outputFile.WriteImage( m_targetImage ) || !outputFile.Close() )
               throw CaughtException();
         }

         m_targetImage.Free();
      }
   }

   // -------------------------------------------------------------------------

   static String UniqueFilePath( const String& filePath )
   {
      for ( unsigned u = 1; ; ++u )
      {
         String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
         if ( !File::Exists( tryFilePath ) )
            return tryFilePath;
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( p_referencePathOrViewId.IsEmpty() )
   {
      whyNot = "No reference image has been defined.";
      return false;
   }

   if ( view.IsPreview() && view.Bounds() != view.Window().MainView().Bounds() )
   {
      whyNot = "LocalNormalization cannot be executed on partial previews.";
      return false;
   }

   if ( view.Image().IsComplexSample() )
   {
      whyNot = "LocalNormalization cannot be executed on complex images.";
      return false;
   }

   if ( p_generateNormalizedImages != LNGenerateNormalizedImages::Always )
      if ( p_generateNormalizedImages != LNGenerateNormalizedImages::ViewExecutionOnly )
         if ( !p_showBackgroundModels )
            if ( !p_showRejectionMaps )
               if ( p_plotNormalizationFunctions == LNPlotNormalizationFunctions::DontPlot )
               {
                  whyNot = "No process operation has been defined applicable to view execution.";
                  return false;
               }

   return true;
}

// ----------------------------------------------------------------------------

static void LoadReferenceImage( ImageVariant& image, const String& path, const String& inputHints )
{
   Console console;

   console.WriteLn( "<end><cbr>Loading reference image:" );
   console.WriteLn( "<raw>" + path + "</raw>" );

   FileFormat format( File::ExtractExtension( path ), true/*read*/, false/*write*/ );
   FileFormatInstance file( format );

   ImageDescriptionArray images;

   if ( !file.Open( images, path, inputHints ) )
      throw CaughtException();

   if ( images.IsEmpty() )
      throw Error( "Empty reference image: " + path );
   if ( images.Length() > 1 )
      console.NoteLn( String().Format( "<end><cbr>* Ignoring %u additional image(s) in reference file.", images.Length()-1 ) );

   image.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                            false/*isComplex*/,
                            images[0].options.bitsPerSample );

   if ( !file.ReadImage( image ) || !file.Close() )
      throw CaughtException();
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::ExecuteOn( View& view )
{
   {
      String why;
      if ( !CanExecuteOn( view, why ) )
         throw Error( why );
   }

   Console console;

   View referenceView = View::Null();
   if ( p_referenceIsView )
   {
      referenceView = View::ViewById( p_referencePathOrViewId );
      if ( referenceView.IsNull() )
         throw Error( "No such reference view: " + p_referencePathOrViewId );
      if ( referenceView.IsPreview() && referenceView.Bounds() != referenceView.Window().MainView().Bounds() )
         throw Error( "Cannot use a partial preview as normalization reference: " + p_referencePathOrViewId );
      if ( referenceView == view )
         throw Error( "The normalization reference and target images must be different" );
   }

   AutoViewLock referenceLock( referenceView, false/*lock*/ );
   if ( !referenceView.IsNull() )
      referenceLock.LockForWrite();

   console.EnableAbort();

   ImageVariant referenceImage;
   if ( p_referenceIsView )
   {
      console.WriteLn( "<end><cbr><br>Reference view: " + p_referencePathOrViewId );
      referenceImage = referenceView.Image();
   }
   else
      LoadReferenceImage( referenceImage, p_referencePathOrViewId, p_inputHints );

   if ( Min( referenceImage.Width(), referenceImage.Height() ) < 2*p_scale )
      throw Error( "Insufficient image dimensions for the selected normalization scale: " + p_referencePathOrViewId );

   if ( Min( referenceImage.Width(), referenceImage.Height() ) < 256 )
      throw Error( "Image too small for local normalization; at least 256 pixels are required." );

   AutoViewLock targetLock( view, false/*lock*/ );
   if ( !view.IsPreview() || referenceView.IsNull() || view.Window() != referenceView.Window() )
      if ( IsHistoryUpdater( view ) )
         targetLock.Lock();
      else
         targetLock.LockForWrite();

   ImageVariant targetImage = view.Image();

   if ( targetImage.Bounds() != referenceImage.Bounds() || targetImage.IsColor() != referenceImage.IsColor() )
      throw Error( "Incompatible image geometry: " + view.FullId() );

   LocalNormalizationThread( *this, referenceImage, targetImage ).Run();

   return true;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_referencePathOrViewId.IsEmpty() )
   {
      whyNot = "No reference image has been defined.";
      return false;
   }

   if ( p_targets.IsEmpty() )
   {
      whyNot = "No target images have been defined.";
      return false;
   }

   if ( p_generateNormalizedImages != LNGenerateNormalizedImages::Always )
      if ( p_generateNormalizedImages != LNGenerateNormalizedImages::GlobalExecutionOnly )
         if ( !p_generateNormalizationData )
         {
            whyNot = "No process operation has been defined applicable to global execution.";
            return false;
         }

   return true;
}

// ----------------------------------------------------------------------------

typedef IndirectArray<LocalNormalizationThread> thread_list;

bool LocalNormalizationInstance::ExecuteGlobal()
{
   Exception::DisableGUIOutput( p_noGUIMessages );

   {
      String why;
      if ( !CanExecuteGlobal( why ) )
         throw Error( why );
   }

//    o_output = Array<OutputData>( p_targets.Length() );

   Console console;

   View referenceView = View::Null();
   if ( p_referenceIsView )
   {
      referenceView = View::ViewById( p_referencePathOrViewId );
      if ( referenceView.IsNull() )
         throw Error( "No such reference view: " + p_referencePathOrViewId );
      if ( referenceView.IsPreview() && referenceView.Bounds() != referenceView.Window().MainView().Bounds() )
         throw Error( "Cannot use a partial preview as normalization reference: " + p_referencePathOrViewId );
   }

   AutoViewLock referenceLock( referenceView, false/*lock*/ );
   if ( !referenceView.IsNull() )
      referenceLock.LockForWrite();

   ImageVariant referenceImage;
   int succeeded = 0;
   int failed = 0;
   int skipped = 0;

   try
   {
      Exception::DisableGUIOutput();

      console.EnableAbort();

      if ( p_referenceIsView )
      {
         console.WriteLn( "<end><cbr><br>Reference view: " + p_referencePathOrViewId );
         referenceImage = referenceView.Image();
      }
      else
         LoadReferenceImage( referenceImage, p_referencePathOrViewId, p_inputHints );

      if ( Min( referenceImage.Width(), referenceImage.Height() ) < 2*p_scale )
         throw Error( "Insufficient image dimensions for the selected normalization scale: " + p_referencePathOrViewId );

      if ( Min( referenceImage.Width(), referenceImage.Height() ) < 256 )
         throw Error( "Image too small for local normalization; at least 256 pixels are required." );

      console.WriteLn( String().Format( "<end><cbr><br>Normalization of %u target files.", p_targets.Length() ) );

      Array<size_type> pendingItems;
      for ( size_type i = 0; i < p_targets.Length(); ++i )
         if ( p_targets[i].enabled )
            pendingItems << i;
         else
         {
            console.NoteLn( "* Skipping disabled target: " + p_targets[i].path );
            ++skipped;
         }

      int numberOfThreadsAvailable = RoundInt( Thread::NumberOfThreads( PCL_MAX_PROCESSORS, 1 ) * p_fileThreadOverload );

      if ( p_useFileThreads && pendingItems.Length() > 1 )
      {
         int numberOfThreads = Min( numberOfThreadsAvailable, int( pendingItems.Length() ) );
         thread_list runningThreads( numberOfThreads ); // N.B.: all pointers are set to nullptr by IndirectArray's ctor.
         console.NoteLn( String().Format( "* Using %d worker threads.", numberOfThreads ) );

         try
         {
            /*
             * Thread watching loop.
             */
            for ( ;; )
            {
               try
               {
                  int running = 0;
                  for ( thread_list::iterator i = runningThreads.Begin(); i != runningThreads.End(); ++i )
                  {
                     Module->ProcessEvents();
                     if ( console.AbortRequested() )
                        throw ProcessAborted();

                     if ( *i != nullptr )
                     {
                        if ( !(*i)->Wait( 150 ) )
                        {
                           ++running;
                           continue;
                        }

                        /*
                         * A thread has just finished.
                         */
                        (*i)->FlushConsoleOutputText();
                        console.WriteLn();
                        String errorInfo;
                        if ( !(*i)->Succeeded() )
                           errorInfo = (*i)->ErrorInfo();

                        /*
                         * N.B.: IndirectArray<>::Delete() sets to zero the
                         * pointer pointed to by the iterator, but does not
                         * remove the array element.
                         */
                        runningThreads.Delete( i );

                        if ( !errorInfo.IsEmpty() )
                           throw Error( errorInfo );

                        ++succeeded;
                     }

                     /*
                      * If there are items pending, create a new thread and
                      * fire the next one.
                      */
                     if ( !pendingItems.IsEmpty() )
                     {
                        *i = new LocalNormalizationThread( *this, referenceImage, p_targets[*pendingItems].path );
                        pendingItems.Remove( pendingItems.Begin() );
                        size_type threadIndex = i - runningThreads.Begin();
                        console.NoteLn( String().Format( "<end><cbr>[%03u] ", threadIndex ) + (*i)->TargetFilePath() );
                        (*i)->Start( ThreadPriority::DefaultMax/*, threadIndex*/ );
                        ++running;
                        if ( pendingItems.IsEmpty() )
                           console.NoteLn( "<br>* Waiting for running tasks to terminate...<br>" );
                        else if ( succeeded+failed > 0 )
                           console.WriteLn();
                     }
                  }

                  if ( !running )
                     break;
               }
               catch ( ProcessAborted& )
               {
                  throw;
               }
               catch ( ... )
               {
                  if ( console.AbortRequested() )
                     throw ProcessAborted();

                  ++failed;
                  try
                  {
                     throw;
                  }
                  ERROR_HANDLER

                  ApplyErrorPolicy();
               }
            }
         }
         catch ( ... )
         {
            console.NoteLn( "<end><cbr><br>* Waiting for running tasks to terminate..." );
            for ( LocalNormalizationThread* thread : runningThreads )
               if ( thread != nullptr )
                  thread->Abort();
            for ( LocalNormalizationThread* thread : runningThreads )
               if ( thread != nullptr )
                  thread->Wait();
            runningThreads.Destroy();
            throw;
         }
      }
      else // !p_useFileThreads || pendingItems.Length() < 2
      {
         for ( size_type itemIndex : pendingItems )
         {
            try
            {
               console.WriteLn( "<end><cbr><br>" );
               LocalNormalizationThread( *this, referenceImage, p_targets[itemIndex].path ).Run();
               ++succeeded;
            }
            catch ( ProcessAborted& )
            {
               throw;
            }
            catch ( ... )
            {
               if ( console.AbortRequested() )
                  throw ProcessAborted();

               ++failed;
               try
               {
                  throw;
               }
               ERROR_HANDLER

               ApplyErrorPolicy();
            }
         }
      }

      Module->ProcessEvents();

      if ( succeeded == 0 )
      {
         if ( failed == 0 )
            throw Error( "No images were normalized: Empty target frames list? No enabled target frames?" );
         throw Error( "No image could be normalized." );
      }

      if ( !p_referenceIsView )
         referenceImage.Free();

      console.NoteLn( String().Format( "<end><cbr><br>===== LocalNormalization: %u succeeded, %u failed, %u skipped =====",
                                       succeeded, failed, skipped ) );
      return true;
   }
   catch ( ... )
   {
      if ( !p_referenceIsView )
         referenceImage.Free();
      throw;
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInstance::ApplyErrorPolicy()
{
   Console console;
   console.ResetStatus();
   console.EnableAbort();

   console.Note( "<end><cbr><br>* Applying error policy: " );

   switch ( p_onError )
   {
   default: // ?
   case LNOnError::Continue:
      console.NoteLn( "Continue on error." );
      break;

   case LNOnError::Abort:
      console.NoteLn( "Abort on error." );
      throw ProcessAborted();

   case LNOnError::AskUser:
      {
         console.NoteLn( "Ask on error..." );

         int r = MessageBox( "<p style=\"white-space:pre;\">"
                             "An error occurred during LocalNormalization execution. What do you want to do?</p>",
                             "LocalNormalization",
                             StdIcon::Error,
                             StdButton::Ignore, StdButton::Abort ).Execute();

         if ( r == StdButton::Abort )
         {
            console.NoteLn( "* Aborting as per user request." );
            throw ProcessAborted();
         }

         console.NoteLn( "* Error ignored as per user request." );
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void* LocalNormalizationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLNScaleParameter )
      return &p_scale;
   if ( p == TheLNNoScaleParameter )
      return &p_noScale;
   if ( p == TheLNRejectionParameter )
      return &p_rejection;
   if ( p == TheLNBackgroundRejectionLimitParameter )
      return &p_backgroundRejectionLimit;
   if ( p == TheLNReferenceRejectionThresholdParameter )
      return &p_referenceRejectionThreshold;
   if ( p == TheLNTargetRejectionThresholdParameter )
      return &p_targetRejectionThreshold;
   if ( p == TheLNHotPixelFilterRadiusParameter )
      return &p_hotPixelFilterRadius;
   if ( p == TheLNNoiseReductionFilterRadiusParameter )
      return &p_noiseReductionFilterRadius;
   if ( p == TheLNReferencePathOrViewIdParameter )
      return p_referencePathOrViewId.Begin();
   if ( p == TheLNReferenceIsViewParameter )
      return &p_referenceIsView;
   if ( p == TheLNTargetEnabledParameter )
      return &p_targets[tableRow].enabled;
   if ( p == TheLNTargetImageParameter )
      return p_targets[tableRow].path.Begin();
   if ( p == TheLNInputHintsParameter )
      return p_inputHints.Begin();
   if ( p == TheLNOutputHintsParameter )
      return p_outputHints.Begin();
   if ( p == TheLNGenerateNormalizedImagesParameter )
      return &p_generateNormalizedImages;
   if ( p == TheLNGenerateNormalizationDataParameter )
      return &p_generateNormalizationData;
   if ( p == TheLNShowBackgroundModelsParameter )
      return &p_showBackgroundModels;
   if ( p == TheLNShowRejectionMapsParameter )
      return &p_showRejectionMaps;
   if ( p == TheLNPlotNormalizationFunctionsParameter )
      return &p_plotNormalizationFunctions;
   if ( p == TheLNNoGUIMessagesParameter )
      return &p_noGUIMessages;
   if ( p == TheLNOutputDirectoryParameter )
      return p_outputDirectory.Begin();
   if ( p == TheLNOutputExtensionParameter )
      return p_outputExtension.Begin();
   if ( p == TheLNOutputPrefixParameter )
      return p_outputPrefix.Begin();
   if ( p == TheLNOutputPostfixParameter )
      return p_outputPostfix.begin();
   if ( p == TheLNOverwriteExistingFilesParameter )
      return &p_overwriteExistingFiles;
   if ( p == TheLNOnErrorParameter )
      return &p_onError;
   if ( p == TheLNUseFileThreadsParameter )
      return &p_useFileThreads;
   if ( p == TheLNFileThreadOverloadParameter )
      return &p_fileThreadOverload;
   if ( p == TheLNMaxFileReadThreadsParameter )
      return &p_maxFileReadThreads;
   if ( p == TheLNMaxFileWriteThreadsParameter )
      return &p_maxFileWriteThreads;
   if ( p == TheLNGraphSizeParameter )
      return &p_graphSize;
   if ( p == TheLNGraphTextSizeParameter )
      return &p_graphTextSize;
   if ( p == TheLNGraphTitleSizeParameter )
      return &p_graphTitleSize;
   if ( p == TheLNGraphTransparentParameter )
      return &p_graphTransparent;
   if ( p == TheLNGraphOutputDirectoryParameter )
      return p_graphOutputDirectory.Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLNReferencePathOrViewIdParameter )
   {
      p_referencePathOrViewId.Clear();
      if ( sizeOrLength > 0 )
         p_referencePathOrViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheLNTargetItemsParameter )
   {
      p_targets.Clear();
      if ( sizeOrLength > 0 )
         p_targets.Add( Item(), sizeOrLength );
   }
   else if ( p == TheLNTargetImageParameter )
   {
      p_targets[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         p_targets[tableRow].path.SetLength( sizeOrLength );
   }
   else if ( p == TheLNInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputDirectoryParameter )
   {
      p_outputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_outputDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputPrefixParameter )
   {
      p_outputPrefix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPrefix.SetLength( sizeOrLength );
   }
   else if ( p == TheLNOutputPostfixParameter )
   {
      p_outputPostfix.Clear();
      if ( sizeOrLength > 0 )
         p_outputPostfix.SetLength( sizeOrLength );
   }
   else if ( p == TheLNGraphOutputDirectoryParameter )
   {
      p_graphOutputDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_graphOutputDirectory.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type LocalNormalizationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheLNReferencePathOrViewIdParameter )
      return p_referencePathOrViewId.Length();
   if ( p == TheLNTargetItemsParameter )
      return p_targets.Length();
   if ( p == TheLNTargetImageParameter )
      return p_targets[tableRow].path.Length();
   if ( p == TheLNInputHintsParameter )
      return p_inputHints.Length();
   if ( p == TheLNOutputHintsParameter )
      return p_outputHints.Length();
   if ( p == TheLNOutputDirectoryParameter )
      return p_outputDirectory.Length();
   if ( p == TheLNOutputExtensionParameter )
      return p_outputExtension.Length();
   if ( p == TheLNOutputPrefixParameter )
      return p_outputPrefix.Length();
   if ( p == TheLNOutputPostfixParameter )
      return p_outputPostfix.Length();
   if ( p == TheLNGraphOutputDirectoryParameter )
      return p_graphOutputDirectory.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInstance.cpp - Released 2018-12-12T09:25:25Z
