//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 01.00.04.0266
// ----------------------------------------------------------------------------
// InverseFourierTransformInstance.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "InverseFourierTransformInstance.h"

#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/FourierTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

InverseFourierTransformInstance::InverseFourierTransformInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   onOutOfRangeResult( IFTOnOutOfRangeResult::Default )
{
}

// ----------------------------------------------------------------------------

InverseFourierTransformInstance::InverseFourierTransformInstance( const InverseFourierTransformInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void InverseFourierTransformInstance::Assign( const ProcessImplementation& p )
{
   const InverseFourierTransformInstance* x = dynamic_cast<const InverseFourierTransformInstance*>( &p );
   if ( x != nullptr )
   {
      idOfFirstComponent = x->idOfFirstComponent;
      idOfSecondComponent = x->idOfSecondComponent;
      onOutOfRangeResult = x->onOutOfRangeResult;
   }
}

// ----------------------------------------------------------------------------

bool InverseFourierTransformInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "InverseFourierTransform can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool InverseFourierTransformInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class InverseFourierTransformEngine
{
public:

   InverseFourierTransformEngine( const InverseFourierTransformInstance& i ) : instance( i )
   {
      if ( instance.idOfFirstComponent.IsEmpty() || instance.idOfSecondComponent.IsEmpty() )
         throw Error( "Missing DFT component identifier(s)" );

      if ( !instance.idOfFirstComponent.IsValidIdentifier() || !instance.idOfSecondComponent.IsValidIdentifier() )
         throw Error( "Invalid DFT component identifier(s)" );

      ImageWindow window1 = ImageWindow::WindowById( instance.idOfFirstComponent );
      if ( window1.IsNull() )
         throw Error( "No such view: \'" + instance.idOfFirstComponent + '\'' );

      IsoString component1 = GetDFTComponentMetadata( min1, max1, centered, width, height, window1 );

      ImageVariant image1 = window1.MainView().GetImage();

      if ( !image1.IsFloatSample() )
         throw Error( "A DFT component must be a floating point image: \'" + instance.idOfFirstComponent + '\'' );

      ImageWindow window2 = ImageWindow::WindowById( instance.idOfSecondComponent );
      if ( window2.IsNull() )
         throw Error( "No such view: \'" + instance.idOfSecondComponent + '\'' );

      bool centered2;
      int width2, height2;
      IsoString component2 = GetDFTComponentMetadata( min2, max2, centered2, width2, height2, window2 );
      if ( centered2 != centered )
         throw Error( "Mismatched coordinate origins between Fourier transform components" );
      if ( width2 != width || height2 != height )
         throw Error( "Mismatched image dimensions between Fourier transform components" );

      ImageVariant image2 = window2.MainView().GetImage();

      if ( !image2.IsFloatSample() )
         throw Error( "A DFT component must be a floating point image: \'" + instance.idOfSecondComponent + '\'' );

      if ( image2.BitsPerSample() != image1.BitsPerSample() )
         throw Error( "Mismatched sample types between Fourier transform components" );

      if ( image1->Bounds() != image2->Bounds() )
         throw Error( "Mismatched dimensions between Fourier transform components" );

      if ( image1->ColorSpace() != image2->ColorSpace() )
         throw Error( "Mismatched color spaces between Fourier transform components" );

      magnitudeAndPhase = false;
      bool componentsSwapped = false;

      if ( component1 == "real" )
      {
         if ( component2 != "imaginary" )
            throw Error( "Expected an imaginary DFT component: " + instance.idOfSecondComponent );
      }
      else if ( component1 == "imaginary" )
      {
         if ( component1 != "real" )
            throw Error( "Expected a real DFT component: " + instance.idOfFirstComponent );
         componentsSwapped = true;
      }
      else if ( component1 == "magnitude" )
      {
         if ( component2 != "phase" )
            throw Error( "Expected a phase DFT component: " + instance.idOfSecondComponent );
         magnitudeAndPhase = true;
      }
      else if ( component1 == "phase" )
      {
         if ( component1 != "magnitude" )
            throw Error( "Expected a magnitude DFT component: " + instance.idOfFirstComponent );
         magnitudeAndPhase = true;
         componentsSwapped = true;
      }
      else
         throw Error( "Unknown DFT component \'" + component1 + "\': " + instance.idOfFirstComponent );

      if ( componentsSwapped )
      {
         Swap( component1, component2 );
         Swap( window1, window2 );
         Swap( image1, image2 );
         Swap( min1, min2 );
         Swap( max1, max2 );
      }

      ImageWindow window( 1, 1, image1->NumberOfNominalChannels(), image1.BitsPerSample(),
                          true/*floatSample*/, image1->IsColor(),
                          true/*initialProcessing*/, IsoString( "inverse_DFT" ) );
      ImageVariant image = window.MainView().Image();
      switch ( image.BitsPerSample() )
      {
      case 32:
         DoInverseTransform( static_cast<Image&>( *image ),
                             static_cast<const Image&>( *image1 ),
                             static_cast<const Image&>( *image2 ),
                             (ComplexPixelTraits*)0 );
         break;
      case 64:
         DoInverseTransform( static_cast<DImage&>( *image ),
                             static_cast<const DImage&>( *image1 ),
                             static_cast<const DImage&>( *image2 ),
                             (DComplexPixelTraits*)0 );
         break;
      }

      window.Show();
      window.ZoomToFit( false/*allowMagnification*/ );
   }

private:

   const InverseFourierTransformInstance& instance;
   int width, height;
   double min1, max1;
   double min2, max2;
   bool centered;
   bool magnitudeAndPhase;

   template <class P1, class P2>
   void DoInverseTransform( GenericImage<P1>& image,
                            const GenericImage<P1>& cmp1,
                            const GenericImage<P1>& cmp2,
                            P2* )
   {
      GenericImage<P2> C( cmp1.Width(), cmp1.Height(), cmp1.ColorSpace() );
      if ( magnitudeAndPhase )
         DoFromMagnitudeAndPhaseComponents( image, cmp1, cmp2, C );
      else
         DoFromRealAndImaginaryComponents( image, cmp1, cmp2, C );

      int dx = image.Width() - width;
      int dy = image.Height() - height;
      image.CropBy( -dx/2, -dy/2, dx/2 - dx, dy/2 - dy );

      switch ( instance.onOutOfRangeResult )
      {
      case IFTOnOutOfRangeResult::DontCare:
         break;
      default:
      case IFTOnOutOfRangeResult::Truncate:
         image.Truncate();
         break;
      case IFTOnOutOfRangeResult::Rescale:
         image.Normalize();
         break;
      }
   }

   template <class P1, class P2>
   class DoFromComponentsThreadBase : public Thread
   {
   public:

      DoFromComponentsThreadBase( const InverseFourierTransformEngine& _engine,
                                  GenericImage<P1>& _img,
                                  const GenericImage<P1>& _cmp1,
                                  const GenericImage<P1>& _cmp2,
                                  GenericImage<P2>& _C,
                                  size_type _start, size_type _end ) :
         engine( _engine ),
         image( _img ),
         cmp1( _cmp1 ),
         cmp2( _cmp2 ),
         C( _C ),
         start( _start ),
         end( _end )
      {
         min1 = engine.min1; max1 = engine.max1;
         min2 = engine.min2; max2 = engine.max2;
      }

   protected:

      const InverseFourierTransformEngine& engine;
      GenericImage<P1>&                    image;
      const GenericImage<P1>&              cmp1;
      const GenericImage<P1>&              cmp2;
      GenericImage<P2>&                    C;
      double                               min1, max1, min2, max2;
      size_type                            start, end;
   };

   template <class P1, class P2>
   class DoFromRealAndImaginaryComponentsThread : public DoFromComponentsThreadBase<P1, P2>
   {
   public:

      DoFromRealAndImaginaryComponentsThread( const InverseFourierTransformEngine& engine,
                                              GenericImage<P1>& image,
                                              const GenericImage<P1>& cmp1,
                                              const GenericImage<P1>& cmp2,
                                              GenericImage<P2>& C,
                                              size_type start, size_type end ) :
         DoFromComponentsThreadBase<P1, P2>( engine, image, cmp1, cmp2, C, start, end )
      {
      }

      virtual void Run()
      {
         const double delta1 = this->max1 - this->min1;
         const double delta2 = this->max2 - this->min2;
         for ( int ch = 0; ch < this->C.NumberOfChannels(); ++ch )
         {
            const typename P1::sample* f1 = this->cmp1[ch] + this->start;
            const typename P1::sample* f2 = this->cmp2[ch] + this->start;
                  typename P2::sample* c  = this->C[ch] + this->start;
                  typename P2::sample* cN = this->C[ch] + this->end;
            do
            {
               c->Real() = *f1++ * delta1 + this->min1;
               c->Imag() = *f2++ * delta2 + this->min2;
            }
            while ( ++c < cN );
         }
      }
   };

   template <class P1, class P2>
   void DoFromRealAndImaginaryComponents( GenericImage<P1>& image,
                                          const GenericImage<P1>& cmp1,
                                          const GenericImage<P1>& cmp2,
                                          GenericImage<P2>& C )
   {
      DoInverseTransform( image, cmp1, cmp2, C, (DoFromRealAndImaginaryComponentsThread<P1, P2>*)0 );
   }

   template <class P1, class P2>
   class DoFromMagnitudeAndPhaseComponentsThread : public DoFromComponentsThreadBase<P1, P2>
   {
   public:

      DoFromMagnitudeAndPhaseComponentsThread( const InverseFourierTransformEngine& engine,
                                               GenericImage<P1>& image,
                                               const GenericImage<P1>& cmp1,
                                               const GenericImage<P1>& cmp2,
                                               GenericImage<P2>& C,
                                               size_type start, size_type end ) :
         DoFromComponentsThreadBase<P1, P2>( engine, image, cmp1, cmp2, C, start, end )
      {
      }

      virtual void Run()
      {
         const double delta1 = this->max1 - this->min1;
         const double delta2 = this->max2 - this->min2;
         for ( int ch = 0; ch < this->C.NumberOfChannels(); ++ch )
         {
            const typename P1::sample* f1 = this->cmp1[ch] + this->start;
            const typename P1::sample* f2 = this->cmp2[ch] + this->start;
                  typename P2::sample* c  = this->C[ch] + this->start;
                  typename P2::sample* cN = this->C[ch] + this->end;
            do
            {
               *c = Polar( *f1++ * delta1 + this->min1,
                           *f2++ * delta2 + this->min2 );
            }
            while ( ++c < cN );
         }
      }
   };

   template <class P1, class P2>
   void DoFromMagnitudeAndPhaseComponents( GenericImage<P1>& image,
                                           const GenericImage<P1>& cmp1,
                                           const GenericImage<P1>& cmp2,
                                           GenericImage<P2>& C )
   {
      DoInverseTransform( image, cmp1, cmp2, C, (DoFromMagnitudeAndPhaseComponentsThread<P1, P2>*)0 );
   }

   template <class P1, class P2, class T>
   void DoInverseTransform( GenericImage<P1>& image,
                            const GenericImage<P1>& cmp1,
                            const GenericImage<P1>& cmp2,
                            GenericImage<P2>& C,
                            T* )
   {
      size_type N = C.NumberOfPixels();
      size_type numberOfThreads = Thread::NumberOfThreads( N, 1024 );
      size_type pixelsPerThread = N/numberOfThreads;

      IndirectArray<T> threads;
      for ( size_type i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new T( *this, image, cmp1, cmp2, C,
                             i*pixelsPerThread, (j < numberOfThreads) ? j*pixelsPerThread : N ) );

      for ( size_type i = 0; i < numberOfThreads; ++i )
         threads[i]->Start( ThreadPriority::DefaultMax, i );
      for ( size_type i = 0; i < numberOfThreads; ++i )
         threads[i]->Wait();

      threads.Destroy();

      DoInverseTransform( image, C );
   }

   template <class P1, class P2>
   void DoInverseTransform( GenericImage<P1>& image, GenericImage<P2>& C )
   {
      StandardStatus status;
      C.SetStatusCallback( &status );

      InPlaceInverseFourierTransform() >> C;

      C.SetStatusCallback( nullptr );

      if ( centered )
         ShiftToCenter( C );

      image = C;
   }

   template <class P2>
   static void ShiftToCenter( GenericImage<P2>& C )
   {
      /*
       * Multiply by (-1)^(x+y) to translate the origin of the DFT to the center
       * of the transformation matrix.
       */
      for ( int ch = 0; ch < C.NumberOfChannels(); ++ch )
         for ( int i = 0; i < C.Height(); ++i )
         {
            typename P2::sample* c = C.ScanLine( i, ch ) + (i & 1);
            for ( int j = i & 1; j < C.Width(); j += 2, c += 2 )
               *c = -*c;
         }
   }

   static IsoString GetDFTComponentMetadata( double& minx, double& maxx, bool& centered,
                                             int& width, int& height, const ImageWindow& w )
   {
      IsoString component;
      bool gotMin = false, gotMax = false, gotCnt = false, gotXsz = false, gotYsz = false;
      FITSKeywordArray keywords;
      w.GetKeywords( keywords );
      for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
      {
         if ( i->name == "PIDFTCMP" )
            component = i->value.Trimmed();
         else if ( i->name == "PIDFTMIN" )
            minx = i->value.ToDouble(), gotMin = true;
         else if ( i->name == "PIDFTMAX" )
            maxx = i->value.ToDouble(), gotMax = true;
         else if ( i->name == "PIDFTCNT" )
            centered = i->value.ToBool(), gotCnt = true;
         else if ( i->name == "PIDFTXSZ" )
            width = i->value.ToInt(), gotXsz = true;
         else if ( i->name == "PIDFTYSZ" )
            height = i->value.ToInt(), gotYsz = true;
      }

      if ( component.IsEmpty() )
         throw Error( "The specified image is not a valid DFT component: " + w.MainView().Id() );
      if ( !gotMin || !gotMax || !gotCnt || !gotXsz || !gotYsz )
         throw Error( "Missing required DFT metadata: " + w.MainView().Id() );

      return component;
   }
};

// ----------------------------------------------------------------------------

bool InverseFourierTransformInstance::ExecuteGlobal()
{
   InverseFourierTransformEngine( *this );
   return true;
}

// ----------------------------------------------------------------------------

void* InverseFourierTransformInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheIFTIdOfFirstComponentParameter )
      return idOfFirstComponent.Begin();
   if ( p == TheIFTIdOfSecondComponentParameter )
      return idOfSecondComponent.Begin();
   if ( p == TheIFTOnOutOfRangeResultParameter )
      return &onOutOfRangeResult;

   return nullptr;
}

bool InverseFourierTransformInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheIFTIdOfFirstComponentParameter )
   {
      idOfFirstComponent.Clear();
      if ( sizeOrLength != 0 )
         idOfFirstComponent.SetLength( sizeOrLength );
   }
   else if ( p == TheIFTIdOfSecondComponentParameter )
   {
      idOfSecondComponent.Clear();
      if ( sizeOrLength != 0 )
         idOfSecondComponent.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type InverseFourierTransformInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheIFTIdOfFirstComponentParameter )
      return idOfFirstComponent.Length();
   if ( p == TheIFTIdOfSecondComponentParameter )
      return idOfSecondComponent.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformInstance.cpp - Released 2019-01-21T12:06:41Z
