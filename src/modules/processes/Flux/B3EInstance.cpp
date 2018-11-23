//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0200
// ----------------------------------------------------------------------------
// B3EInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#include "B3EInstance.h"
#include "B3EParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/Mutex.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/View.h>

#include <iostream>

#define TINY 1.2e-07

namespace pcl
{

// ----------------------------------------------------------------------------

B3EInstance::InputViewParameters::InputViewParameters() :
   id(),
   center( 0 ),
   subtractBackground( TheB3ESubstractBackground1Parameter->DefaultValue() ),
   backgroundReferenceViewId(),
   backgroundLow( TheB3EBackgroundLow1Parameter->DefaultValue() ),
   backgroundHigh( TheB3EBackgroundHigh1Parameter->DefaultValue() ),
   backgroundUseROI( TheB3EBackgroundUseROI1Parameter->DefaultValue() ),
   backgroundROI( 0 ),
   outputBackgroundReferenceMask( TheB3EOutputBackgroundReferenceMask1Parameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

B3EInstance::B3EInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_outputCenter( 0 ),
   p_intensityUnits( B3EIntensityUnits::Default ),
   p_syntheticImage( TheB3ESyntheticImageParameter->DefaultValue() ),
   p_thermalMap( TheB3EThermalMapParameter->DefaultValue() ),
   p_outOfRangeMask( TheB3EOutOfRangeMaskParameter->DefaultValue() ),
   o_syntheticImageViewId(),
   o_thermalMapViewId(),
   o_outOfRangeMaskViewId()
{
}

B3EInstance::B3EInstance( const B3EInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void B3EInstance::Assign( const ProcessImplementation& p )
{
   const B3EInstance* x = dynamic_cast<const B3EInstance*>( &p );
   if ( x != nullptr )
   {
      p_inputView[0]         = x->p_inputView[0];
      p_inputView[1]         = x->p_inputView[1];
      p_outputCenter         = x->p_outputCenter;
      p_intensityUnits       = x->p_intensityUnits;
      p_syntheticImage       = x->p_syntheticImage;
      p_thermalMap           = x->p_thermalMap;
      p_outOfRangeMask       = x->p_outOfRangeMask;
      o_syntheticImageViewId = x->o_syntheticImageViewId;
      o_thermalMapViewId     = x->o_thermalMapViewId;
      o_outOfRangeMaskViewId = x->o_outOfRangeMaskViewId;
   }
}

UndoFlags B3EInstance::UndoMode( const View& ) const
{
   return UndoFlag::Keywords | UndoFlag::PixelData;
}

bool B3EInstance::Validate( String& info )
{
   if ( p_inputView[0].id.IsEmpty() )
   {
      info = "Empty first view identifier.";
      return false;
   }
   if ( !View::IsValidViewId( p_inputView[0].id ) )
   {
      info = "Invalid first view identifier.";
      return false;
   }

   if ( p_inputView[1].id.IsEmpty() )
   {
      info = "Empty second view identifier.";
      return false;
   }
   if ( !View::IsValidViewId( p_inputView[1].id ) )
   {
      info = "Invalid second view identifier.";
      return false;
   }

   if ( 1 + p_inputView[0].center == 1 )
   {
      info = "Zero or insignificant first input center value.";
      return false;
   }

   if ( 1 + p_inputView[1].center == 1 )
   {
      info = "Zero or insignificant second input center value.";
      return false;
   }

   if ( 1 + p_outputCenter == 1 )
   {
      info = "Zero or insignificant output center value.";
      return false;
   }

   if ( 1 + Abs( p_inputView[0].center - p_inputView[1].center ) == 1 )
   {
      info = "Both input center values must be different (with respect to the machine epsilon).";
      return false;
   }

   if ( p_syntheticImage )
      if ( 1 + Abs( p_outputCenter - p_inputView[0].center ) == 1 ||
           1 + Abs( p_outputCenter - p_inputView[1].center ) == 1 )
      {
         info = "The output center value must be different (with respect to the machine epsilon) from both input center values.";
         return false;
      }

   return true;
}

bool B3EInstance::CanExecuteOn( const View&, String& whyNot ) const
{
   whyNot = "B3E can only be executed in the global context.";
   return false;
}

bool B3EInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true; // Validate() has already detected any invalid parameters.
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct B3ETuple
{
   double T; // temperature in K
   double f; // output flux
};

class B3EEngine
{
public:

   B3EEngine( const B3EInstance& instance ) : m_instance( instance )
   {
      switch ( m_instance.p_intensityUnits )
      {
      case B3EIntensityUnits::PhotonsFrequency:
         n = 2;
         break;
      case B3EIntensityUnits::EnergyFrequency:
         n = 3;
         break;
      case B3EIntensityUnits::PhotonsWavelength:
         n = 4;
         break;
      case B3EIntensityUnits::EnergyWavelength:
         n = 5;
         break;
      default:
         throw Error( "Internal error: unknown intensity units" );
      }

      switch ( m_instance.p_intensityUnits )
      {
      case B3EIntensityUnits::PhotonsFrequency:
      case B3EIntensityUnits::EnergyFrequency:
         xa = 48 * m_instance.p_inputView[0].center;
         xb = 48 * m_instance.p_inputView[1].center;
         xc = 48 * m_instance.p_outputCenter;
         break;
      case B3EIntensityUnits::PhotonsWavelength:
      case B3EIntensityUnits::EnergyWavelength:
         xa = 1.439e+07/m_instance.p_inputView[0].center;
         xb = 1.439e+07/m_instance.p_inputView[1].center;
         xc = 1.439e+07/m_instance.p_outputCenter;
         break;
      }

      xq = xb/xa;

      gamma = 5 - 9*Pow( 3 + Abs( Ln( xq ) ), -0.252 );

      xq1 = PowI( xq, n - 1 );
      xq2 = Pow( xq, n - gamma );

      xnca = PowI( xc/xa, n );
      xncb = PowI( xc/xb, n );

      Tprop = (xb - xa)/2;

      closerToA = Abs( xc - xa ) < Abs( xc - xb );
   }

   void operator()( UInt16Image* It, Image* Ic, UInt8Image* Im,
                    const ImageVariant& va, double ra, double ma, double ba,
                    const ImageVariant& vb, double rb, double mb, double bb, double rc ) const
   {
      B3E( It, Ic, Im, va, ra, ma, ba, vb, rb, mb, bb, rc );
   }

   B3ETuple operator()( double ra, double ma, double rb, double mb ) const
   {
      double a = ra + ma;
      double b = rb + mb;
      double Iab = Max( TINY, a )/Max( TINY, b );
      B3ETuple b3e;
      b3e.T = Tprop*(1/Ln( Iab*xq1 ) + 1/Ln( Iab*xq2 ));
      if ( Abs( b3e.T ) < TINY )
         b3e.f = 0;
      else
      {
         double xcT1 = Max( TINY, Exp( xc/b3e.T ) - 1 );
         if ( closerToA )
            b3e.f = Max( 0.0, a*xnca*(Exp( xa/b3e.T ) - 1)/xcT1 );
         else
            b3e.f = Max( 0.0, b*xncb*(Exp( xb/b3e.T ) - 1)/xcT1 );
      }
      return b3e;
   }

private:

   template <class P1, class P2>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const B3EEngine& a_engine,
                  UInt16Image* a_It, Image* a_Ic, UInt8Image* a_Im,
                  const GenericImage<P1>& a_Ia, double a_ra, double a_ma, double a_ba,
                  const GenericImage<P2>& a_Ib, double a_rb, double a_mb, double a_bb, double a_rc,
                  const StatusMonitor& status, size_type count ) :
         AbstractImage::ThreadData( status, count ),
         engine( a_engine ),
         It( a_It ), Ic( a_Ic ), Im( a_Im ),
         Ia( a_Ia ), ra( a_ra ), ma( a_ma ),  ba( a_ba ),
         Ib( a_Ib ), rb( a_rb ), mb( a_mb ),  bb( a_bb ),
         rc( a_rc )
      {
      }

      const B3EEngine&        engine;
      UInt16Image*            It;
      Image*                  Ic;
      UInt8Image*             Im;
      const GenericImage<P1>& Ia;
      double                  ra, ma, ba;
      const GenericImage<P2>& Ib;
      double                  rb, mb, bb;
      double                  rc;
   };

   template <class P1, class P2>
   class B3EThread : public Thread
   {
   public:

      B3EThread( ThreadData<P1,P2>& data, size_type start, size_type end ) :
         Thread(),
         m_data( data ), m_start( start ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         uint16* ft = m_data.It ? m_data.It->PixelData() + m_start : nullptr;
         float*  fc = m_data.Ic ? m_data.Ic->PixelData() + m_start : nullptr;
         uint8*  fm = m_data.Im ? m_data.Im->PixelData() + m_start : nullptr;

         const typename P1::sample* fa = m_data.Ia.PixelData() + m_start;
         const typename P2::sample* fb = m_data.Ib.PixelData() + m_start;

         for ( size_type i = m_start; i < m_end; ++i )
         {
            double fax = *fa;
            double fbx = *fb;

            double a;
            P1::FromSample( a, *fa++ );
            a = (a - m_data.ba)*m_data.ra + m_data.ma;

            double b;
            P2::FromSample( b, *fb++ );
            b = (b - m_data.bb)*m_data.rb + m_data.mb;

            double Iab = Max( TINY, a )/Max( TINY, b );
            double T = m_data.engine.Tprop*(1/Ln( Iab*m_data.engine.xq1 ) + 1/Ln( Iab*m_data.engine.xq2 ));

            if ( static_cast<uint16>( T ) == 28 )
               std::cout << String().Format( "a  = %.6f\n", a )
                         << String().Format( "b  = %.6f\n", b )
                         << String().Format( "fa = %.6f\n", fax )
                         << String().Format( "fb = %.6f\n", fbx )
                         << String().Format( "x  = %d\n",   i%m_data.Ia.Width() )
                         << String().Format( "y  = %d\n",   i/m_data.Ia.Width() )
                         << String().Format( "Tp = %.6f\n", m_data.engine.Tprop )
                         << String().Format( "xa = %.6f\n", m_data.engine.xa )
                         << String().Format( "xb = %.6f\n", m_data.engine.xb )
                         << String().Format( "g  = %.6f\n", m_data.engine.gamma ) << std::flush;

            if ( ft != nullptr )
               *ft++ = static_cast<uint16>( RoundI( Range( T, 0.0, double( uint16_max ) ) ) );

            if ( fc != nullptr )
               if ( T < 1 )
               {
                  *fc++ = 0;
                  if ( fm != nullptr )
                     *fm++ = 64;
               }
               else
               {
                  double xcT1 = Max( TINY, Exp( m_data.engine.xc/T ) - 1 );
                  double c;
                  if ( m_data.engine.closerToA )
                     c = a*m_data.engine.xnca*(Exp( m_data.engine.xa/T ) - 1)/xcT1;
                  else
                     c = b*m_data.engine.xncb*(Exp( m_data.engine.xb/T ) - 1)/xcT1;
                  c /= m_data.rc;
                  if ( fm != nullptr )
                     *fm++ = (c < 0) ? 0 : ((c > 1) ? 255 : 153);
                  *fc++ = Range( c, 0.0, 1.0 );
               }

            UPDATE_THREAD_MONITOR( 65536 )
         }
      }

   private:

      ThreadData<P1,P2>& m_data;
      size_type m_start, m_end;
   };

   template <class P1, class P2>
   void B3E( UInt16Image* It, Image* Ic, UInt8Image* Im,
             const GenericImage<P1>& Ia, double ra, double ma, double ba,
             const GenericImage<P2>& Ib, double rb, double mb, double bb, double rc ) const
   {
      size_type N = Ia.NumberOfPixels();

      int numberOfThreads = Thread::NumberOfThreads( N, 16 );
      size_type pixelsPerThread = N/numberOfThreads;

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "B3E", N );

      ThreadData<P1,P2> data( *this, It, Ic, Im, Ia, ra, ma, ba, Ib, rb, mb, bb, rc, monitor, N );
      ReferenceArray<B3EThread<P1,P2> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads << new B3EThread<P1,P2>( data, i*pixelsPerThread,
                                                (j < numberOfThreads) ? j*pixelsPerThread : N );
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
   }

   template <class P1>
   void B3E( UInt16Image* It, Image* Ic, UInt8Image* Im,
             const GenericImage<P1>& Ia, double ra, double ma, double ba,
             const ImageVariant& vb, double rb, double mb, double bb, double rc ) const
   {
      if ( vb.IsFloatSample() )
         switch ( vb.BitsPerSample() )
         {
         case 32:
            B3E( It, Ic, Im, Ia, ra, ma, ba, static_cast<const Image&>( *vb ), rb, mb, bb, rc );
            break;
         case 64:
            B3E( It, Ic, Im, Ia, ra, ma, ba, static_cast<const DImage&>( *vb ), rb, mb, bb, rc );
            break;
         }
      else
         switch ( vb.BitsPerSample() )
         {
         case  8:
            B3E( It, Ic, Im, Ia, ra, ma, ba, static_cast<const UInt8Image&>( *vb ), rb, mb, bb, rc );
            break;
         case 16:
            B3E( It, Ic, Im, Ia, ra, ma, ba, static_cast<const UInt16Image&>( *vb ), rb, mb, bb, rc );
            break;
         case 32:
            B3E( It, Ic, Im, Ia, ra, ma, ba, static_cast<const UInt32Image&>( *vb ), rb, mb, bb, rc );
            break;
         }
   }

   void B3E( UInt16Image* It, Image* Ic, UInt8Image* Im,
             const ImageVariant& va, double ra, double ma, double ba,
             const ImageVariant& vb, double rb, double mb, double bb, double rc ) const
   {
      if ( va.IsFloatSample() )
         switch ( va.BitsPerSample() )
         {
         case 32:
            B3E( It, Ic, Im, static_cast<const Image&>( *va ), ra, ma, ba, vb, rb, mb, bb, rc );
            break;
         case 64:
            B3E( It, Ic, Im, static_cast<const DImage&>( *va ), ra, ma, ba, vb, rb, mb, bb, rc );
            break;
         }
      else
         switch ( va.BitsPerSample() )
         {
         case  8:
            B3E( It, Ic, Im, static_cast<const UInt8Image&>( *va ), ra, ma, ba, vb, rb, mb, bb, rc );
            break;
         case 16:
            B3E( It, Ic, Im, static_cast<const UInt16Image&>( *va ), ra, ma, ba, vb, rb, mb, bb, rc );
            break;
         case 32:
            B3E( It, Ic, Im, static_cast<const UInt32Image&>( *va ), ra, ma, ba, vb, rb, mb, bb, rc );
            break;
         }
   }

   const B3EInstance& m_instance;

   /*
    * Calculation invariants
    */
   int    n;             // wavelength or frequency exponent
   double xa, xb, xc;    // in Fernandian units
   double xq;            // xb/xa
   double gamma;         // gamma exponent
   double xq1, xq2;      // used in temperature formula
   double xnca, xncb;    // proportionality factors
   double Tprop;         // proportinality factor for temperature calculation
   bool   closerToA : 1; // xc is closer to xa than to xb
};

static bool GetFluxConversionParameters( double& flxRange, double& flxMin, const View& v )
{
   flxMin = 0;
   flxRange = 1;
   bool foundFlxRange = false;
   bool foundFlxMin = false;
   FITSKeywordArray keywords;
   v.Window().GetKeywords( keywords );
   for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
      if ( i->name == "FLXRANGE" )
      {
         if ( i->value.TryToDouble( flxRange ) )
            foundFlxRange = true;
      }
      else if ( i->name == "FLXMIN" )
      {
         if ( i->value.TryToDouble( flxMin ) )
            foundFlxMin = true;
      }

   if ( !foundFlxRange )
      Console().WarningLn( "<end><cbr>** Warning: B3E: FLXRANGE keyword not found: " + v.FullId() );
   if ( !foundFlxMin )
      Console().WarningLn( "<end><cbr>** Warning: B3E: FLXMIN keyword not found: " + v.FullId() );

   return foundFlxMin && foundFlxRange;
}

static void SetKeyword( ImageWindow& w, const IsoString& name, double value, const IsoString& comment )
{
   FITSHeaderKeyword keyword( name,
                              IsoString().Format( "%+.8e", value ),
                              comment );
   FITSKeywordArray keywords;
   w.GetKeywords( keywords );
   keywords.Add( keyword );
   w.SetKeywords( keywords );
}

static IsoString ValidFullId( const IsoString& id )
{
   IsoString validId( id );
   validId.ReplaceString( "->", "_" );
   return validId;
}

template <class P>
static void MakeBackgroundMask( UInt8Image& mask, const GenericImage<P>& image, float low, float high )
{
   mask.Zero();

   Rect r = image.SelectedRectangle();

   for ( int c = 0; c < mask.NumberOfChannels(); ++c )
   {
      for ( int y = r.y0; y < r.y1; ++y )
      {
         const typename P::sample* p = image.ScanLine( y, c );
         uint8* m = mask.ScanLine( y, c );
         for ( int x = r.x0; x < r.x1; ++x )
         {
            float f; P::FromSample( f, p[x] );
            if ( f > low && f < high )
               m[x] = 0xff;
         }
      }
   }
}

static void MakeBackgroundMask( UInt8Image& mask, const ImageVariant& image, float low, float high )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         MakeBackgroundMask( mask, static_cast<const Image&>( *image ), low, high );
         break;
      case 64:
         MakeBackgroundMask( mask, static_cast<const DImage&>( *image ), low, high );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         MakeBackgroundMask( mask, static_cast<const UInt8Image&>( *image ), low, high );
         break;
      case 16:
         MakeBackgroundMask( mask, static_cast<const UInt16Image&>( *image ), low, high );
         break;
      case 32:
         MakeBackgroundMask( mask, static_cast<const UInt32Image&>( *image ), low, high );
         break;
      }
}

double B3EInstance::EvaluateBackground( const InputViewParameters& imgParams )
{
   String viewId = imgParams.backgroundReferenceViewId;
   if ( viewId.IsEmpty() )
      viewId = imgParams.id;

   View bkgView = View::ViewById( viewId );
   if ( bkgView.IsNull() )
      throw Error( "No such view (background reference): " + imgParams.backgroundReferenceViewId );
   if ( !bkgView.IsColor() )
      throw Error( "The background reference view must be a color image: " + imgParams.backgroundReferenceViewId );

   AutoViewLock lock( bkgView, false/*lock*/ );
   if ( bkgView.CanWrite() )
      bkgView.LockForWrite();

   ImageVariant image = bkgView.Image();

   Console console;
   console.EnableAbort();

   if ( imgParams.backgroundUseROI )
   {
      image->SelectRectangle( imgParams.backgroundROI );
      Rect r = image->SelectedRectangle();
      if ( !r.IsRect() )
         image->ResetSelection();
      console.WriteLn( String().Format( "<end><cbr>Background reference ROI : left=%d, top=%d, width=%d, height=%d",
                                        r.x0, r.y0, r.Width(), r.Height() ) );
   }

   StandardStatus status;
   image->SetStatusCallback( &status );
   image->Status().Initialize( "Evaluating background", image->NumberOfSelectedPixels() );
   image->Status().DisableInitialization();
   image->SelectChannel( 0 );
   double bg = image.Median();

   if ( imgParams.outputBackgroundReferenceMask )
   {
      IsoString maskId = ValidFullId( bkgView.FullId() ) + "_background";
      ImageWindow maskWindow = ImageWindow( bkgView.Width(), bkgView.Height(), 1, 8, false, false, true, maskId );
      if ( maskWindow.IsNull() )
         throw Error( "Unable to create image window: " + maskId );
      ImageVariant mask = maskWindow.MainView().Image();
      MakeBackgroundMask( static_cast<UInt8Image&>( *mask ), image, imgParams.backgroundLow, imgParams.backgroundHigh );
      maskWindow.Show();
      maskWindow.ZoomToFit();
   }

   console.WriteLn( String().Format( "<end><cbr>* Mean background: %.5g", bg ) );
   return bg;
}

bool B3EInstance::ExecuteGlobal()
{
   o_syntheticImageViewId.Clear();
   o_thermalMapViewId.Clear();
   o_outOfRangeMaskViewId.Clear();

   View inputView1 = View::ViewById( p_inputView[0].id );
   if ( inputView1.IsNull() )
      throw Error( "No such view: " + p_inputView[0].id );

   View inputView2 = View::ViewById( p_inputView[1].id );
   if ( inputView2.IsNull() )
      throw Error( "No such view: " + p_inputView[1].id );

   AutoViewLock lock1( inputView1 );
   AutoViewLock lock2( inputView2 );

   const ImageVariant inputImage1 = inputView1.Image();
   const ImageVariant inputImage2 = inputView2.Image();

   if ( inputImage1.IsComplexSample() || inputImage2.IsComplexSample() )
      throw Error( "B3E cannot work with complex images" );

   if ( inputImage1->Bounds() != inputImage2->Bounds() )
      throw Error( "Incompatible image geometries" );

   if ( inputImage1->NumberOfNominalChannels() != 1 )
      throw Error( "B3E requires grayscale (monochrome) images: " + p_inputView[0].id );

   if ( inputImage2->NumberOfNominalChannels() != 1 )
      throw Error( "B3E requires grayscale (monochrome) images: " + p_inputView[1].id );

   B3EEngine E( *this );

   double flxRange1, flxMin1;
   GetFluxConversionParameters( flxRange1, flxMin1, inputView1 );
   double flxRange2, flxMin2;
   GetFluxConversionParameters( flxRange2, flxMin2, inputView2 );

   B3ETuple limits = E( flxRange1, flxMin1, flxRange2, flxMin2 );

   ImageWindow syntheticImageWindow;
   ImageWindow outOfRangeMaskWindow;
   if ( p_syntheticImage )
   {
      syntheticImageWindow = ImageWindow( inputImage1->Width(),
                                          inputImage1->Height(),
                                          1,     /*numberOfChannels*/
                                          32,    /*bitsPerSample*/
                                          true,  /*floatSample*/
                                          false, /*Color*/
                                          true,  /*initialProcessing*/
                                          "B3E_synthetic_channel" );
      if ( syntheticImageWindow.IsNull() )
         throw Error( "Unable to create synthetic image window" );

      o_syntheticImageViewId = syntheticImageWindow.MainView().Id();

      SetKeyword( syntheticImageWindow,
                  "FLXRANGE", limits.f, "FLXRANGE*pixel_value + FLXMIN = erg/cm^2/s/nm" );
      SetKeyword( syntheticImageWindow,
                  "FLXMIN", 0.0, "" );

      if ( p_outOfRangeMask )
      {
         outOfRangeMaskWindow = ImageWindow( inputImage1->Width(),
                                             inputImage1->Height(),
                                             1,     /*numberOfChannels*/
                                             8,     /*bitsPerSample*/
                                             false, /*floatSample*/
                                             false, /*Color*/
                                             true,  /*initialProcessing*/
                                             "B3E_out_of_range_mask" );
         if ( outOfRangeMaskWindow.IsNull() )
            throw Error( "Unable to create out-of-range mask image window" );

         o_thermalMapViewId = outOfRangeMaskWindow.MainView().Id();
      }
   }

   ImageWindow thermalMapWindow;
   if ( p_thermalMap )
   {
      thermalMapWindow = ImageWindow(  inputImage1->Width(),
                                       inputImage1->Height(),
                                       1,       /*numberOfChannels*/
                                       16,      /*bitsPerSample*/
                                       false,   /*floatSample*/
                                       false,   /*Color*/
                                       true,    /*initialProcessing*/
                                       "B3E_thermal_map" );
      if ( thermalMapWindow.IsNull() )
         throw Error( "Unable to create thermal map image window" );

      o_outOfRangeMaskViewId = thermalMapWindow.MainView().Id();
   }

   ImageVariant syntheticImageVar;
   Image* syntheticImage = 0;
   ImageVariant outOfRangeMaskVar;
   UInt8Image* outOfRangeMask = 0;
   if ( p_syntheticImage )
   {
      syntheticImageVar = syntheticImageWindow.MainView().Image();
      syntheticImage = static_cast<Image*>( syntheticImageVar.ImagePtr() );
      if ( p_outOfRangeMask )
      {
         outOfRangeMaskVar = outOfRangeMaskWindow.MainView().Image();
         outOfRangeMask = static_cast<UInt8Image*>( outOfRangeMaskVar.ImagePtr() );
      }
   }

   ImageVariant thermalMapVar;
   UInt16Image* thermalMapImage = 0;
   if ( p_thermalMap )
   {
      thermalMapVar = thermalMapWindow.MainView().Image();
      thermalMapImage = static_cast<UInt16Image*>( thermalMapVar.ImagePtr() );
   }

   /*
    * Compute background references
    */
   double bkg1 = 0;
   if ( p_inputView[0].subtractBackground )
      bkg1 = EvaluateBackground( p_inputView[0] );

   double bkg2 = 0;
   if ( p_inputView[1].subtractBackground )
      bkg2 = EvaluateBackground( p_inputView[1] );

   E( thermalMapImage, syntheticImage, outOfRangeMask,
      inputImage1, flxRange1, flxMin1, bkg1,
      inputImage2, flxRange2, flxMin2, bkg2, limits.f );

   if ( p_thermalMap )
      SetKeyword( thermalMapWindow,
                  "THMAX",
                  thermalMapImage->MaximumPixelValue(),
                  "PixInsight (B3E): Thermal map maximum value" );

   if ( p_syntheticImage )
   {
      syntheticImageWindow.Show();
      syntheticImageWindow.ZoomToFit();
      if ( p_outOfRangeMask )
      {
         outOfRangeMaskWindow.Show();
         outOfRangeMaskWindow.ZoomToFit();
      }
   }

   if ( p_thermalMap )
   {
      thermalMapWindow.Show();
      thermalMapWindow.ZoomToFit();
   }

   return true;
}

void* B3EInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheB3EInputViewId1Parameter )
      return p_inputView[0].id.Begin();
   if ( p == TheB3EInputViewId2Parameter )
      return p_inputView[1].id.Begin();
   if ( p == TheB3EInputCenter1Parameter )
      return &p_inputView[0].center;
   if ( p == TheB3EInputCenter2Parameter )
      return &p_inputView[1].center;
   if ( p == TheB3EOutputCenterParameter )
      return &p_outputCenter;
   if ( p == TheB3EIntensityUnitsParameter )
      return &p_intensityUnits;
   if ( p == TheB3ESyntheticImageParameter )
      return &p_syntheticImage;
   if ( p == TheB3EThermalMapParameter )
      return &p_thermalMap;
   if ( p == TheB3EOutOfRangeMaskParameter )
      return &p_outOfRangeMask;
   if ( p == TheB3ESyntheticImageViewIdParameter )
      return o_syntheticImageViewId.Begin();
   if ( p == TheB3EThermalMapViewIdParameter )
      return o_thermalMapViewId.Begin();
   if ( p == TheB3EOutOfRangeMaskViewIdParameter )
      return o_outOfRangeMaskViewId.Begin();

   if ( p == TheB3ESubstractBackground1Parameter )
      return &p_inputView[0].subtractBackground;
   if ( p == TheB3EBackgroundReferenceViewId1Parameter )
      return p_inputView[0].backgroundReferenceViewId.Begin();
   if ( p == TheB3EBackgroundLow1Parameter )
      return &p_inputView[0].backgroundLow;
   if ( p == TheB3EBackgroundHigh1Parameter )
      return &p_inputView[0].backgroundHigh;
   if ( p == TheB3EBackgroundUseROI1Parameter )
      return &p_inputView[0].backgroundUseROI;
   if ( p == TheB3EBackgroundROIX01Parameter )
      return &p_inputView[0].backgroundROI.x0;
   if ( p == TheB3EBackgroundROIY01Parameter )
      return &p_inputView[0].backgroundROI.y0;
   if ( p == TheB3EBackgroundROIX11Parameter )
      return &p_inputView[0].backgroundROI.x1;
   if ( p == TheB3EBackgroundROIY11Parameter )
      return &p_inputView[0].backgroundROI.y1;
   if ( p == TheB3EOutputBackgroundReferenceMask1Parameter )
      return &p_inputView[0].outputBackgroundReferenceMask;

   if ( p == TheB3ESubstractBackground2Parameter )
      return &p_inputView[1].subtractBackground;
   if ( p == TheB3EBackgroundReferenceViewId2Parameter )
      return p_inputView[1].backgroundReferenceViewId.Begin();
   if ( p == TheB3EBackgroundLow2Parameter )
      return &p_inputView[1].backgroundLow;
   if ( p == TheB3EBackgroundHigh2Parameter )
      return &p_inputView[1].backgroundHigh;
   if ( p == TheB3EBackgroundUseROI2Parameter )
      return &p_inputView[1].backgroundUseROI;
   if ( p == TheB3EBackgroundROIX02Parameter )
      return &p_inputView[1].backgroundROI.x0;
   if ( p == TheB3EBackgroundROIY02Parameter )
      return &p_inputView[1].backgroundROI.y0;
   if ( p == TheB3EBackgroundROIX12Parameter )
      return &p_inputView[1].backgroundROI.x1;
   if ( p == TheB3EBackgroundROIY12Parameter )
      return &p_inputView[1].backgroundROI.y1;
   if ( p == TheB3EOutputBackgroundReferenceMask2Parameter )
      return &p_inputView[1].outputBackgroundReferenceMask;

   return 0;
}

bool B3EInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheB3EInputViewId1Parameter )
   {
      p_inputView[0].id.Clear();
      if ( sizeOrLength > 0 )
         p_inputView[0].id.SetLength( sizeOrLength );
   }
   else if ( p == TheB3EInputViewId2Parameter )
   {
      p_inputView[1].id.Clear();
      if ( sizeOrLength > 0 )
         p_inputView[1].id.SetLength( sizeOrLength );
   }
   else if ( p == TheB3ESyntheticImageViewIdParameter )
   {
      o_syntheticImageViewId.Clear();
      if ( sizeOrLength > 0 )
         o_syntheticImageViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheB3EThermalMapViewIdParameter )
   {
      o_thermalMapViewId.Clear();
      if ( sizeOrLength > 0 )
         o_thermalMapViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheB3EOutOfRangeMaskViewIdParameter )
   {
      o_outOfRangeMaskViewId.Clear();
      if ( sizeOrLength > 0 )
         o_outOfRangeMaskViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheB3EBackgroundReferenceViewId1Parameter )
   {
      p_inputView[0].backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         p_inputView[0].backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheB3EBackgroundReferenceViewId2Parameter )
   {
      p_inputView[1].backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         p_inputView[1].backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type B3EInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheB3EInputViewId1Parameter )
      return p_inputView[0].id.Length();
   if ( p == TheB3EInputViewId2Parameter )
      return p_inputView[1].id.Length();
   if ( p == TheB3ESyntheticImageViewIdParameter )
      return o_syntheticImageViewId.Length();
   if ( p == TheB3EThermalMapViewIdParameter )
      return o_thermalMapViewId.Length();
   if ( p == TheB3EOutOfRangeMaskViewIdParameter )
      return o_outOfRangeMaskViewId.Length();
   if ( p == TheB3EBackgroundReferenceViewId1Parameter )
      return p_inputView[0].backgroundReferenceViewId.Length();
   if ( p == TheB3EBackgroundReferenceViewId2Parameter )
      return p_inputView[1].backgroundReferenceViewId.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF B3EInstance.cpp - Released 2018-11-23T18:45:58Z
