//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0207
// ----------------------------------------------------------------------------
// StarGeneratorInstance.cpp - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "Projection.h"
#include "StarDatabase.h"
#include "StarGeneratorInstance.h"
#include "StarGeneratorParameters.h"

#include <pcl/AutoLock.h>
#include <pcl/Console.h>
#include <pcl/GaussianFilter.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/Translation.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarGeneratorInstance::StarGeneratorInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
starDatabasePath(),
centerRA( TheSGCenterRAParameter->DefaultValue() ),
centerDec( TheSGCenterDecParameter->DefaultValue() ),
epoch( TheSGEpochParameter->DefaultValue() ),
projectionSystem( SGProjectionSystem::Default ),
focalLength( TheSGFocalLengthParameter->DefaultValue() ),
pixelSize( TheSGPixelSizeParameter->DefaultValue() ),
sensorWidth( TheSGSensorWidthParameter->DefaultValue() ),
sensorHeight( TheSGSensorHeightParameter->DefaultValue() ),
limitMagnitude( TheSGLimitMagnitudeParameter->DefaultValue() ),
starFWHM( TheSGStarFWHMParameter->DefaultValue() ),
nonlinear( TheSGNonlinearParameter->DefaultValue() ),
targetMinimumValue( TheSGTargetMinimumValueParameter->DefaultValue() ),
outputMode( SGOutputMode::Default ),
outputFilePath()
{
}

StarGeneratorInstance::StarGeneratorInstance( const StarGeneratorInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void StarGeneratorInstance::Assign( const ProcessImplementation& p )
{
   const StarGeneratorInstance* x = dynamic_cast<const StarGeneratorInstance*>( &p );
   if ( x != 0 )
   {
      starDatabasePath = x->starDatabasePath;
      centerRA = x->centerRA;
      centerDec = x->centerDec;
      epoch = x->epoch;
      projectionSystem = x->projectionSystem;
      focalLength = x->focalLength;
      pixelSize = x->pixelSize;
      sensorWidth = x->sensorWidth;
      sensorHeight = x->sensorHeight;
      limitMagnitude = x->limitMagnitude;
      starFWHM = x->starFWHM;
      nonlinear = x->nonlinear;
      targetMinimumValue = x->targetMinimumValue;
      outputMode = x->outputMode;
      outputFilePath = x->outputFilePath;
   }
}

bool StarGeneratorInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   whyNot = "StarGenerator can only be executed in the global context.";
   return false;
}

bool StarGeneratorInstance::IsHistoryUpdater( const View& v ) const
{
   return false;
}

bool StarGeneratorInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

class StarGeneratorEngine
{
public:

   StarGeneratorEngine( const StarGeneratorInstance& aInstance ) :
   instance( aInstance )
   {
      if ( S == 0 )
      {
         if ( instance.starDatabasePath.IsEmpty() )
            throw Error( "No star database file has been specified" );
         if ( !File::Exists( instance.starDatabasePath ) )
            throw Error( "No such file: " + instance.starDatabasePath );
         S = new StarDatabase( instance.starDatabasePath );
      }
      else
      {
         if ( !instance.starDatabasePath.IsEmpty() && S->FileName() != instance.starDatabasePath )
            S->Open( instance.starDatabasePath );
      }

      if ( instance.outputMode == SGOutputMode::CSVFile )
         if ( instance.outputFilePath.Trimmed().IsEmpty() )
            throw Error( "No output CSV file has been specified" );

      double sensorWidthMM = instance.sensorWidth*instance.pixelSize*1e-03;
      double sensorHeightMM = instance.sensorHeight*instance.pixelSize*1e-03;

      double fovWidth = 2*ArcTan( sensorWidthMM/2/instance.focalLength );
      double fovHeight = 2*ArcTan( sensorHeightMM/2/instance.focalLength );

      projection.Initialize( Rad( instance.centerRA ), Rad( instance.centerDec ),
                             fovHeight,
                             instance.sensorWidth, instance.sensorHeight,
                             instance.projectionSystem == SGProjectionSystem::Conformal );

      Console console;
      console.WriteLn( "<end><cbr>" );
      console.WriteLn( projection.System().Name() );
      console.WriteLn( String().Format( "Projection center ....... &alpha;=%.5f &delta;=%+.5f", Deg( projection.CenterLongitude() ), Deg( projection.CenterLatitude() ) ) );
      console.WriteLn( String().Format( "Field of view ........... H=%.5f V=%.5f", Deg( fovWidth ), Deg( fovHeight ) ) );
      console.WriteLn( String().Format( "Right ascension range ... &alpha;<sub>E</sub>=%.5f &alpha;<sub>W</sub>=%.5f", Deg( projection.EastLongitude() ), Deg( projection.WestLongitude() ) ) );
      console.WriteLn( String().Format( "Declination range ....... &delta;<sub>S</sub>=%+.5f &delta;<sub>N</sub>=%+.5f", Deg( projection.SouthLatitude() ), Deg( projection.NorthLatitude() ) ) );

      Console().WriteLn( "<end><cbr><br>Searching stars..." );

      StarDatabase::star_list stars;

      for ( int i = int( Floor( Deg( projection.SouthLatitude() ) ) ),
               i1 = int( Floor( Deg( projection.NorthLatitude() ) ) ); i <= i1; ++i )
      {
         GetStars( stars, i, int( Deg( In2PiRange( projection.EastLongitude() ) ) ),
                             int( Deg( In2PiRange( projection.WestLongitude() ) ) ) );
      }

      if ( stars.IsEmpty() )
         throw Error( "No stars found." );

      console.WriteLn( String().Format( "%u stars found", stars.Length() ) );

      ApplyProperMotions( stars );

      if ( instance.outputMode == SGOutputMode::CSVFile )
      {
         File f;
         f.CreateForWriting( instance.outputFilePath.Trimmed() );

         console.WriteLn( "Generating output CSV file: " + instance.outputFilePath );

         try
         {
            WriteCSVStars( f, stars );
            f.Close();
         }
         catch( ... )
         {
            f.Close();
            throw;
         }
      }
      else // SGOutputMode::Image
      {
         double starTheta = Rad( instance.starFWHM/3600.0 );
         double starFWHM = projection.AngularDistanceToPlane( starTheta );
         double starSigma = starFWHM/2.35482;

         console.WriteLn( String().Format( "Star FWHM: %.2f arcsec, %.3f px", Deg( starTheta )*3600, starFWHM ) );

         ImageWindow window = CreateImageWindow( "stars", 32, false ); // 32-bit integer image

         try
         {
            ImageVariant img = window.MainView().Image();

            PlotStars( img, stars, starSigma );

            if ( instance.nonlinear )
               ApplyMTF( img );

            window.Show();
            window.ZoomToFit();
         }
         catch ( ... )
         {
            window.Close();
            throw;
         }
      }
   }

private:

   static StarDatabase* S;

   const StarGeneratorInstance& instance;
   Projection projection;

   void GetStars( StarDatabase::star_list& stars, int i, int j1, int j2 ) const
   {
      if ( j1 <= j2 )
      {
         for ( ; j1 <= j2; ++j1 )
            if ( projection.IntersectsSector( i, j1 ) )
               break;
         for ( ; j1 <= j2; --j2 )
            if ( projection.IntersectsSector( i, j2 ) )
               break;
         if ( j1 <= j2 )
            stars.Add( S->ReadSectors( i, j1, j2, instance.limitMagnitude ) );
      }
      else
      {
         GetStars( stars, i,  0,  j2 );
         GetStars( stars, i, j1, 359 );
      }
   }

   void ApplyProperMotions( StarDatabase::star_list& stars )
   {
      // ### TODO: Compute apparent star positions

      if ( instance.epoch != J2000 )
      {
         StandardStatus status;
         StatusMonitor monitor;
         monitor.SetCallback( &status );
         monitor.Initialize( String().Format( "Applying proper motions, epoch = %.1f", instance.epoch ), stars.Length() );

         for ( StarDatabase::star_list::iterator i = stars.Begin(); i != stars.End(); ++i, ++monitor )
            i->SetEpoch( instance.epoch );
      }
   }

   void WriteCSVStars( File& f, const StarDatabase::star_list& stars )
   {
      int w = instance.sensorWidth;
      int h = instance.sensorHeight;

      double x0 = projection.Width()/2;
      double y0 = projection.Height()/2;

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Projecting stars", stars.Length() );

      f.OutTextLn( IsoString().Format( "%d,%d", w, h ) );

      size_type N = 0;

      // ### TODO: Parallelize

      for ( StarDatabase::star_list::const_iterator i = stars.Begin(); i != stars.End(); ++i, ++monitor )
      {
         double x, y;
         projection.SphericalToRectangular( x, y, Rad( i->ra ), Rad( i->dec ) );
         x = x0 - x;
         y = y0 - y;
         if ( x >= 0 && x < w && y >= 0 && y < h )
         {
            f.OutTextLn( IsoString().Format( "%.2f,%.2f,%.4e", x, y, Pow( 2.512, MAG_MIN - i->mag ) ) );
            ++N;
         }
      }

      Console().WriteLn( String().Format( "<end><cbr>%u stars in projection", N ) );
   }

   void PlotStars( ImageVariant& image, const StarDatabase::star_list& stars, float starSigma )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32 : PlotStars( static_cast<Image&>( *image ), stars, starSigma ); break;
         case 64 : PlotStars( static_cast<DImage&>( *image ), stars, starSigma ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8 : PlotStars( static_cast<UInt8Image&>( *image ), stars, starSigma ); break;
         case 16 : PlotStars( static_cast<UInt16Image&>( *image ), stars, starSigma ); break;
         case 32 : PlotStars( static_cast<UInt32Image&>( *image ), stars, starSigma ); break;
         }
   }

   template <class P>
   void PlotStars( GenericImage<P>& image, const StarDatabase::star_list& stars, float starSigma )
   {
      Image star;
      GaussianFilter( starSigma, 0.0001F ).ToImage( star );
      star.CropBy( +1, +1, +1, +1 );

      image.ResetSelections();
      image.Zero();

      size_type N = stars.Length();

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Rendering stars", N );

      int numberOfThreads = Thread::NumberOfThreads( N, 16 );
      size_type starsPerThread = N/numberOfThreads;

      ThreadData<P> data( image, star, projection, stars, monitor );

      ReferenceArray<PlotThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new PlotThread<P>( data,
                                         i*starsPerThread,
                                         (j < numberOfThreads) ? j*starsPerThread : N ) );

      AbstractImage::RunThreads( threads, data );

      N = 0;
      for ( int i = 0; i < numberOfThreads; ++i )
         N += threads[i].N;

      threads.Destroy();

      Console().WriteLn( String().Format( "<end><cbr>%u stars in projection", N ) );
   }

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image,
                  const Image& a_star,
                  const Projection& a_projection,
                  const StarDatabase::star_list& a_stars,
                  StatusMonitor& a_monitor ) :
      AbstractImage::ThreadData( a_monitor, a_stars.Length() ),
      image( a_image ),
      star( a_star ),
      projection( a_projection ),
      stars( a_stars )
      {
      }

            GenericImage<P>&         image;
      const Image&                   star;
      const Projection&              projection;
      const StarDatabase::star_list& stars;
   };

   template <class P>
   class PlotThread : public Thread
   {
   public:

      size_type N;

      PlotThread( ThreadData<P>& data, size_type first, size_type end ) :
      Thread(),
      N( 0 ), m_data( data ), m_first( first ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const double x0 = m_data.projection.Width()/2;
         const double y0 = m_data.projection.Height()/2;
         const int    r2 = m_data.star.Width() >> 1;

         // Here we want smoothness and cannot tolerate ringing, so bicubic
         // B-spline is a good option.
         BicubicBSplinePixelInterpolation I;
         Translation T( I );
         T.DisableParallelProcessing();

         Image star( m_data.star.Width(), m_data.star.Height() );
         star.Status().DisableInitialization();

         for ( StarDatabase::star_list::const_iterator i = m_data.stars.At( m_first ), j = m_data.stars.At( m_end ); i < j; ++i )
         {
            double x, y;
            m_data.projection.SphericalToRectangular( x, y, Rad( i->ra ), Rad( i->dec ) );
            x = x0 - x;
            y = y0 - y;
            if ( m_data.image.Includes( x, y ) )
            {
               star.Mov( m_data.star );
               star.Mul( Pow( 2.512, MAG_MIN - i->mag ) );
               Point p( TruncI( x ), TruncI( y ) );
               T.SetDelta( x-p.x, y-p.y );
               T >> star;
               p -= r2;
               {
                  AutoLock lock( m_data.mutex );
                  m_data.image.Apply( star, ImageOp::Add, p );
               }
               ++N;
            }
            UPDATE_THREAD_MONITOR( 50 )
         }
      }

   private:

      ThreadData<P>& m_data;
      size_type      m_first;
      size_type      m_end;
   };

   void ApplyMTF( ImageVariant& image )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32 : ApplyMTF( static_cast<Image&>( *image ) ); break;
         case 64 : ApplyMTF( static_cast<DImage&>( *image ) ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8 : ApplyMTF( static_cast<UInt8Image&>( *image ) ); break;
         case 16 : ApplyMTF( static_cast<UInt16Image&>( *image ) ); break;
         case 32 : ApplyMTF( static_cast<UInt32Image&>( *image ) ); break;
         }
   }

   template <class P>
   void ApplyMTF( GenericImage<P>& img )
   {
      double m0 = Pow( 2.512, MAG_MIN - Min( MAG_MAX, double( instance.limitMagnitude ) ) );
      double m1 = HistogramTransformation::MTF( instance.targetMinimumValue, m0 );
      HistogramTransformation( m1 ) >> img;
   }

   ImageWindow CreateImageWindow( const IsoString& id, int bitsPerSample, bool floatSample )
   {
      ImageWindow window( instance.sensorWidth, instance.sensorHeight, 1,
                          bitsPerSample, floatSample, false, true, id );
      if ( window.IsNull() )
         throw Error( "Unable to create image window: " + id );
      return window;
   }
};

StarDatabase* StarGeneratorEngine::S = 0;

bool StarGeneratorInstance::ExecuteGlobal()
{
   Console().EnableAbort();
   StarGeneratorEngine( *this );
   return true;
}

void* StarGeneratorInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSGStarDatabasePathParameter )
      return starDatabasePath.c_str();
   if ( p == TheSGCenterRAParameter )
      return &centerRA;
   if ( p == TheSGCenterDecParameter )
      return &centerDec;
   if ( p == TheSGEpochParameter )
      return &epoch;
   if ( p == TheSGProjectionSystemParameter )
      return &projectionSystem;
   if ( p == TheSGFocalLengthParameter )
      return &focalLength;
   if ( p == TheSGPixelSizeParameter )
      return &pixelSize;
   if ( p == TheSGSensorWidthParameter )
      return &sensorWidth;
   if ( p == TheSGSensorHeightParameter )
      return &sensorHeight;
   if ( p == TheSGLimitMagnitudeParameter )
      return &limitMagnitude;
   if ( p == TheSGStarFWHMParameter )
      return &starFWHM;
   if ( p == TheSGNonlinearParameter )
      return &nonlinear;
   if ( p == TheSGTargetMinimumValueParameter )
      return &targetMinimumValue;
   if ( p == TheSGOutputModeParameter )
      return &outputMode;
   if ( p == TheSGOutputFilePathParameter )
      return outputFilePath.c_str();
   return 0;
}

bool StarGeneratorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSGStarDatabasePathParameter )
   {
      starDatabasePath.Clear();
      if ( sizeOrLength > 0 )
         starDatabasePath.SetLength( sizeOrLength );
      return true;
   }
   if ( p == TheSGOutputFilePathParameter )
   {
      outputFilePath.Clear();
      if ( sizeOrLength > 0 )
         outputFilePath.SetLength( sizeOrLength );
      return true;
   }
   return false;
}

size_type StarGeneratorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSGStarDatabasePathParameter )
      return starDatabasePath.Length();
   if ( p == TheSGOutputFilePathParameter )
      return outputFilePath.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarGeneratorInstance.cpp - Released 2015/10/08 11:24:40 UTC
