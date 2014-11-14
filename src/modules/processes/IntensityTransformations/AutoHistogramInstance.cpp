// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard IntensityTransformations Process Module Version 01.07.00.0287
// ****************************************************************************
// AutoHistogramInstance.cpp - Released 2014/11/14 17:19:23 UTC
// ****************************************************************************
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "AutoHistogramInstance.h"
#include "AutoHistogramParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Histogram.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AutoHistogramInstance::AutoHistogramInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_clip( TheAHClipParameter->DefaultValue() ),
p_clipTogether( TheAHClipTogetherParameter->DefaultValue() ),
p_stretch( TheAHStretchParameter->DefaultValue() ),
p_stretchTogether( TheAHStretchTogetherParameter->DefaultValue() ),
p_stretchMethod( AHStretchMethod::Default )
{
   p_clipLow[0] = TheAHClipLowRParameter->DefaultValue();
   p_clipLow[1] = TheAHClipLowGParameter->DefaultValue();
   p_clipLow[2] = TheAHClipLowBParameter->DefaultValue();

   p_clipHigh[0] = TheAHClipHighRParameter->DefaultValue();
   p_clipHigh[1] = TheAHClipHighGParameter->DefaultValue();
   p_clipHigh[2] = TheAHClipHighBParameter->DefaultValue();

   p_targetMedian[0] = TheAHTargetMedianRParameter->DefaultValue();
   p_targetMedian[1] = TheAHTargetMedianGParameter->DefaultValue();
   p_targetMedian[2] = TheAHTargetMedianBParameter->DefaultValue();
}

// ----------------------------------------------------------------------------

AutoHistogramInstance::AutoHistogramInstance( const AutoHistogramInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void AutoHistogramInstance::Assign( const ProcessImplementation& p )
{
   const AutoHistogramInstance* x = dynamic_cast<const AutoHistogramInstance*>( &p );
   if ( x != 0 )
   {
      p_clip            = x->p_clip;
      p_clipTogether    = x->p_clipTogether;
      p_stretch         = x->p_stretch;
      p_stretchTogether = x->p_stretchTogether;
      p_stretchMethod   = x->p_stretchMethod;

      for ( int i = 0; i < 3; ++i )
      {
         p_clipLow[i]      = x->p_clipLow[i];
         p_clipHigh[i]     = x->p_clipHigh[i];
         p_targetMedian[i] = x->p_targetMedian[i];
      }
   }
}

// ----------------------------------------------------------------------------

bool AutoHistogramInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "AutoHistogram cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class AutoHistogramEngine
{
public:

   static void Apply( ImageVariant& image,
                      size_type N, const View::histogram_list& H, const View::statistics_list& S,
                      const AutoHistogramInstance& instance,
                      bool consoleOutput = true )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<pcl::Image&>( *image ), N, H, S, instance, consoleOutput ); break;
         case 64: Apply( static_cast<pcl::DImage&>( *image ), N, H, S, instance, consoleOutput ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: Apply( static_cast<pcl::UInt8Image&>( *image ), N, H, S, instance, consoleOutput ); break;
         case 16: Apply( static_cast<pcl::UInt16Image&>( *image ), N, H, S, instance, consoleOutput ); break;
         case 32: Apply( static_cast<pcl::UInt32Image&>( *image ), N, H, S, instance, consoleOutput ); break;
         }
   }

   template <class P>
   static void Apply( GenericImage<P>& image,
                      size_type N, const View::histogram_list& H, const View::statistics_list& S,
                      const AutoHistogramInstance& instance,
                      bool consoleOutput = true )
   {
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         image.SelectChannel( c );

         // Median of the original image
         double median = 0;
         if ( instance.p_stretch )
            median = S[c]->Median();

         /*
          * Auto clip
          */
         if ( instance.p_clip )
         {
            float c0 = H[c]->NormalizedClipLow( size_type( Round( N * instance.p_clipLow[c]/100 ) ) );
            float c1 = H[c]->NormalizedClipHigh( size_type( Round( N * instance.p_clipHigh[c]/100 ) ) );
            if ( c0 > 0 || c1 < 1 )
            {
               image.Truncate( P::ToSample( c0 ), P::ToSample( c1 ) );
               image.Rescale();

               /*
                * NB: If clipped the histogram, compute the resulting median
                * after clipping + rescaling.
                */
               if ( instance.p_stretch )
                  if ( c0 >= median )
                     median = 0;
                  else if ( c1 <= median )
                     median = 1;
                  else
                     median = (median - c0)/(c1 - c0);
            }
         }

         /*
          * Auto stretch
          */
         if ( instance.p_stretch )
         {
            if ( median <= 0 )
            {
               if ( consoleOutput )
                  Console().WarningLn( "<end><cbr>** Warning: Too dark image - unable to autostretch." );
               continue;
            }

            if ( median >= 1 )
            {
               if ( consoleOutput )
                  Console().WarningLn( "<end><cbr>** Warning: Too bright image - unable to autostretch." );
               continue;
            }

            switch ( instance.p_stretchMethod )
            {
            default:
            case AHStretchMethod::Gamma:
               image.Pow( Log( instance.p_targetMedian[c] )/Log( median ) );
               break;

            case AHStretchMethod::Logarithm:
               {
                  double range = FindLogRange( median, instance.p_targetMedian[c], consoleOutput );
                  double m = range - 1;
                  double ln = Ln( range );
                  typename P::sample* p = image[c];
                  typename P::sample* pN = p + image.NumberOfPixels();
                  for ( ; p < pN; ++p, ++image.Status() )
                  {
                     double f; P::FromSample( f, *p );
                     *p = P::ToSample( Ln( 1 + m*f )/ln );
                  }
               }
               break;

            case AHStretchMethod::MTF:
               HistogramTransformation( HistogramTransformation::MTF( instance.p_targetMedian[c], median ) ) >> image;
               break;
            }
         }
      }

      image.ResetSelections();
   }

private:

   static double FindLogRange( double median, double targetMedian, bool consoleOutput )
   {
      double range = Pow( median, 1.0/(targetMedian - 1) );
      range = Pow( median + 1/(range - 1), 1.0/(targetMedian - Ln( range - 1 )/Ln( range )) );
      for ( int i = 0; ; )
      {
         double old = range;
         range = Pow( median + 1/(range - 1), 1/(targetMedian - Ln( range - 1 )/Ln( range )) );
         if ( Abs( range - old ) < 0.01 )
            break; // converged
         if ( ++i == 100 )
         {
            if ( consoleOutput )
               Console().WarningLn( "<end><cbr>** Warning: AutoHistogram / LogStretch: No convergence after 100 iterations." );
            break;
         }
      }
      return range;
   }
};

bool AutoHistogramInstance::ExecuteOn( View& view )
{
   View::histogram_list H;
   View::statistics_list S;

   AutoViewLock lock( view );

   try
   {
      ImageVariant image = view.Image();

      if ( image.IsComplexSample() )
         return false;

      /*
       * NB: If our target view is a preview, we want to make sure we get
       * statistics and histograms for its mother image. Otherwise this
       * process would not provide consistent results for previews.
       */
      View mainView = view.Window().MainView();

      if ( p_clip )
      {
         if ( !mainView.AreHistogramsAvailable() )
            mainView.CalculateHistograms();
         mainView.GetHistograms( H );
      }

      if ( p_stretch )
      {
         if ( !mainView.AreStatisticsAvailable() )
            mainView.CalculateStatistics();
         mainView.GetStatistics( S );
      }

      StandardStatus status;
      image->SetStatusCallback( &status );

      Console().EnableAbort();

      AutoHistogramEngine::Apply( image, mainView.Image()->NumberOfPixels(), H, S, *this );

      H.Destroy();
      S.Destroy();
      return true;
   }
   catch ( ... )
   {
      H.Destroy();
      S.Destroy();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* AutoHistogramInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheAHClipParameter )
      return &p_clip;
   if ( p == TheAHClipTogetherParameter )
      return &p_clipTogether;
   if ( p == TheAHClipLowRParameter )
      return p_clipLow + 0;
   if ( p == TheAHClipLowGParameter )
      return p_clipLow + 1;
   if ( p == TheAHClipLowBParameter )
      return p_clipLow + 2;
   if ( p == TheAHClipHighRParameter )
      return p_clipHigh + 0;
   if ( p == TheAHClipHighGParameter )
      return p_clipHigh + 1;
   if ( p == TheAHClipHighBParameter )
      return p_clipHigh + 2;
   if ( p == TheAHStretchParameter )
      return &p_stretch;
   if ( p == TheAHStretchTogetherParameter )
      return &p_stretchTogether;
   if ( p == TheAHStretchMethodParameter )
      return &p_stretchMethod;
   if ( p == TheAHTargetMedianRParameter )
      return p_targetMedian + 0;
   if ( p == TheAHTargetMedianGParameter )
      return p_targetMedian + 1;
   if ( p == TheAHTargetMedianBParameter )
      return p_targetMedian + 2;
  return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AutoHistogramInstance.cpp - Released 2014/11/14 17:19:23 UTC
