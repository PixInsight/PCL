//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 01.00.02.0295
// ----------------------------------------------------------------------------
// CloneStampInstance.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#include "CloneStampInstance.h"
#include "CloneStampParameters.h"
#include "CloneStampInterface.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/VariableShapeFilter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CloneStampInstance::CloneStampInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   actions(),
   cloner(),
   width( 0 ), height( 0 ), // don't rescale coordinates
   color( uint32( TheCSColorParameter->DefaultValue() ) ),
   boundsColor( uint32( TheCSBoundsColorParameter->DefaultValue() ) ),
   isInterfaceInstance( false )
{
}

CloneStampInstance::CloneStampInstance( const CloneStampInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void CloneStampInstance::Assign( const ProcessImplementation& p )
{
   const CloneStampInstance* x = dynamic_cast<const CloneStampInstance*>( &p );
   if ( x != nullptr )
   {
      actions = x->actions;
      cloner = x->cloner;
      width = x->width;
      height = x->height;
      color = x->color;
      boundsColor = x->boundsColor;
   }

   isInterfaceInstance = false;
}

void CloneStampInstance::TestAssign( const ProcessImplementation& p )
{
   const CloneStampInstance* x = dynamic_cast<const CloneStampInstance*>( &p );
   if ( x != nullptr )
   {
      actions.Clear();
      cloner.Clear();
      width = x->width;
      height = x->height;
      color = x->color;
      boundsColor = x->boundsColor;
   }

   isInterfaceInstance = false;
}

bool CloneStampInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

KernelFilter CloneStampInstance::BrushData::CreateBrush() const
{
   if ( radius == 0 )
      return KernelFilter::coefficient_matrix( 0.F, 0.F,     0.F,
                                               0.F, opacity, 0.F,
                                               0.F, 0.F,     0.F );
   const float KMax = 16.0f;
   const float KMin = 1.0f;

/*
   float k = (softness <= 0.5) ?
      KMax - (2*KMax - 4)*softness : 2 - (4 - 2*KMin)*(softness - 0.5);
*/
   int n = 1 + (Max( 1, radius ) << 1);

   VariableShapeFilter G( n, (KMax - KMin)*(1 - softness) + KMin, 0.025F );

   KernelFilter::coefficient_matrix C = G.Coefficients();

   if ( opacity != 1 )
      C *= opacity;

   for ( int i = 0; i < n; ++i )
      for ( int j = 0; j < n; ++j )
         if ( C[i][j] <= 0.025F )
            C[i][j] = 0;

   return KernelFilter( C );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CloneStampEngine
{
public:

   typedef CloneStampInstance::action_sequence::const_iterator action_iterator;
   typedef CloneStampInstance::cloner_sequence::const_iterator cloner_iterator;
   typedef CloneStampInstance::BrushData                       brush_data;

   static void Apply( ImageVariant& image, const ImageVariant& mask, bool maskInverted, const CloneStampInstance& S )
   {
      if ( image.IsComplexSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<ComplexImage&>( *image ), mask, maskInverted, S ); break;
         case 64: Apply( static_cast<DComplexImage&>( *image ), mask, maskInverted, S ); break;
         }
      else if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<Image&>( *image ), mask, maskInverted, S ); break;
         case 64: Apply( static_cast<DImage&>( *image ), mask, maskInverted, S ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: Apply( static_cast<UInt8Image&>( *image ), mask, maskInverted, S ); break;
         case 16: Apply( static_cast<UInt16Image&>( *image ), mask, maskInverted, S ); break;
         case 32: Apply( static_cast<UInt32Image&>( *image ), mask, maskInverted, S ); break;
         }
   }

   static void Apply( ImageVariant& image, const ImageVariant& src, const ImageVariant& mask, bool maskInverted,
                      const brush_data& b, cloner_iterator i, const Point& delta )
   {
      if ( image.IsComplexSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<ComplexImage&>( *image ), src, mask, maskInverted, b, i, delta ); break;
         case 64: Apply( static_cast<DComplexImage&>( *image ), src, mask, maskInverted, b, i, delta ); break;
         }
      else if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<Image&>( *image ), src, mask, maskInverted, b, i, delta ); break;
         case 64: Apply( static_cast<DImage&>( *image ), src, mask, maskInverted, b, i, delta ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: Apply( static_cast<UInt8Image&>( *image ), src, mask, maskInverted, b, i, delta ); break;
         case 16: Apply( static_cast<UInt16Image&>( *image ), src, mask, maskInverted, b, i, delta ); break;
         case 32: Apply( static_cast<UInt32Image&>( *image ), src, mask, maskInverted, b, i, delta ); break;
         }
   }

private:

   static brush_data   brushData;
   static KernelFilter brush;

   template <class P>
   static void Apply( GenericImage<P>& image, const ImageVariant& mask, bool maskInverted, const CloneStampInstance& S )
   {
      if ( mask )
      {
         if ( mask.IsComplexSample() )
            switch ( mask.BitsPerSample() )
            {
            case 32: Apply( image, static_cast<const ComplexImage&>( *mask ), maskInverted, S ); break;
            case 64: Apply( image, static_cast<const DComplexImage&>( *mask ), maskInverted, S ); break;
            }
         else if ( mask.IsFloatSample() )
            switch ( mask.BitsPerSample() )
            {
            case 32: Apply( image, static_cast<const Image&>( *mask ), maskInverted, S ); break;
            case 64: Apply( image, static_cast<const DImage&>( *mask ), maskInverted, S ); break;
            }
         else
            switch ( mask.BitsPerSample() )
            {
            case  8: Apply( image, static_cast<const UInt8Image&>( *mask ), maskInverted, S ); break;
            case 16: Apply( image, static_cast<const UInt16Image&>( *mask ), maskInverted, S ); break;
            case 32: Apply( image, static_cast<const UInt32Image&>( *mask ), maskInverted, S ); break;
            }
      }
      else
      {
         ImageVariant m;
         m.CreateImageAs( image );
         Apply( image, static_cast<const GenericImage<P>&>( *m ), maskInverted, S );
      }
   }

   template <class P, class P1>
   static void Apply( GenericImage<P>& image, const GenericImage<P1>& mask, bool maskInverted, const CloneStampInstance& S )
   {
      double tsx, tsy;

      if ( S.width > 0 && S.height > 0 )
      {
         tsx = double( image.Width() )/S.width;
         tsy = double( image.Height() )/S.height;
      }
      else
         tsx = tsy = 1;

      for ( cloner_iterator i = S.cloner.Begin(); i != S.cloner.End(); )
      {
         if ( i->actionIdx >= S.actions.Length() )
            throw Error( "Invalid action index" );

         action_iterator a = S.actions.At( i->actionIdx );

         Point delta( i->targetPos - a->sourcePos );

         if ( a->sourceId.IsEmpty() )
            i = Apply( image, image, mask, maskInverted, i, delta, tsx, tsy, tsx, tsy, S );
         else
         {
            ImageWindow sourceWindow = ImageWindow::WindowById( a->sourceId );
            if ( sourceWindow.IsNull() )
               throw Error( "Source image not found: " + a->sourceId );

            ImageVariant source = sourceWindow.MainView().Image();

            double ssx, ssy;
            if ( a->sourceWidth > 0 && a->sourceHeight > 0 )
            {
               ssx = double( source.Width() )/a->sourceWidth;
               ssy = double( source.Height() )/a->sourceHeight;
            }
            else
               ssx = ssy = 1;

            if ( source.IsComplexSample() )
               switch ( source.BitsPerSample() )
               {
               case 32: i = Apply( image, static_cast<pcl::ComplexImage&>( *source ), mask, maskInverted, i, delta, tsx, tsy, ssx, ssy, S ); break;
               case 64: i = Apply( image, static_cast<pcl::DComplexImage&>( *source ), mask, maskInverted, i, delta, tsx, tsy, ssx, ssy, S ); break;
               }
            else if ( source.IsFloatSample() )
               switch ( source.BitsPerSample() )
               {
               case 32: i = Apply( image, static_cast<pcl::Image&>( *source ), mask, maskInverted, i, delta, tsx, tsy, ssx, ssy, S ); break;
               case 64: i = Apply( image, static_cast<pcl::DImage&>( *source ), mask, maskInverted, i, delta, tsx, tsy, ssx, ssy, S ); break;
               }
            else
               switch ( source.BitsPerSample() )
               {
               case  8: i = Apply( image, static_cast<pcl::UInt8Image&>( *source ), mask, maskInverted, i, delta, tsx, tsy, ssx, ssy, S ); break;
               case 16: i = Apply( image, static_cast<pcl::UInt16Image&>( *source ), mask, maskInverted, i, delta, tsx, tsy, ssx, ssy, S ); break;
               case 32: i = Apply( image, static_cast<pcl::UInt32Image&>( *source ), mask, maskInverted, i, delta, tsx, tsy, ssx, ssy, S ); break;
               }
         }
      }
   }

   template <class P, class P1, class P2>
   static cloner_iterator Apply( GenericImage<P>& image, const GenericImage<P1>& src, const GenericImage<P2>& mask, bool maskInverted,
                                 cloner_iterator i, const Point& delta,
                                 double tsx, double tsy, double ssx, double ssy, const CloneStampInstance& S )
   {
      for ( uint32 actionIdx = i->actionIdx; ; )
      {
         Apply( image, src, mask, maskInverted, S.actions[actionIdx].brush, i, delta, tsx, tsy, ssx, ssy );
         if ( ++i == S.cloner.End() || i->actionIdx != actionIdx )
            break;
      }
      return i;
   }

   template <class P>
   static void Apply( GenericImage<P>& image, const ImageVariant& src, const ImageVariant& mask, bool maskInverted,
                      const brush_data& b, cloner_iterator i, const Point& delta )
   {
      if ( src.IsComplexSample() )
         switch ( src.BitsPerSample() )
         {
         case 32: Apply( image, static_cast<const ComplexImage&>( *src ), mask, maskInverted, b, i, delta ); break;
         case 64: Apply( image, static_cast<const DComplexImage&>( *src ), mask, maskInverted, b, i, delta ); break;
         }
      else if ( src.IsFloatSample() )
         switch ( src.BitsPerSample() )
         {
         case 32: Apply( image, static_cast<const Image&>( *src ), mask, maskInverted, b, i, delta ); break;
         case 64: Apply( image, static_cast<const DImage&>( *src ), mask, maskInverted, b, i, delta ); break;
         }
      else
         switch ( src.BitsPerSample() )
         {
         case  8: Apply( image, static_cast<const UInt8Image&>( *src ), mask, maskInverted, b, i, delta ); break;
         case 16: Apply( image, static_cast<const UInt16Image&>( *src ), mask, maskInverted, b, i, delta ); break;
         case 32: Apply( image, static_cast<const UInt32Image&>( *src ), mask, maskInverted, b, i, delta ); break;
         }
   }

   template <class P, class P1>
   static void Apply( GenericImage<P>& image, const GenericImage<P1>& src, const ImageVariant& mask, bool maskInverted,
                      const brush_data& b, cloner_iterator i, const Point& delta )
   {
      if ( mask )
      {
         if ( mask.IsComplexSample() )
            switch ( mask.BitsPerSample() )
            {
            case 32: Apply( image, src, static_cast<const ComplexImage&>( *mask ), maskInverted, b, i, delta ); break;
            case 64: Apply( image, src, static_cast<const DComplexImage&>( *mask ), maskInverted, b, i, delta ); break;
            }
         else if ( mask.IsFloatSample() )
            switch ( mask.BitsPerSample() )
            {
            case 32: Apply( image, src, static_cast<const Image&>( *mask ), maskInverted, b, i, delta ); break;
            case 64: Apply( image, src, static_cast<const DImage&>( *mask ), maskInverted, b, i, delta ); break;
            }
         else
            switch ( mask.BitsPerSample() )
            {
            case  8: Apply( image, src, static_cast<const UInt8Image&>( *mask ), maskInverted, b, i, delta ); break;
            case 16: Apply( image, src, static_cast<const UInt16Image&>( *mask ), maskInverted, b, i, delta ); break;
            case 32: Apply( image, src, static_cast<const UInt32Image&>( *mask ), maskInverted, b, i, delta ); break;
            }
      }
      else
      {
         ImageVariant m;
         m.CreateImageAs( image );
         Apply( image, src, static_cast<const GenericImage<P>&>( *m ), maskInverted, b, i, delta );
      }
   }

   template <class P, class P1, class P2>
   static void Apply( GenericImage<P>& image, const GenericImage<P1>& src, const GenericImage<P2>& mask, bool maskInverted,
                      const brush_data& b, cloner_iterator i, const Point& delta,
                      double tsx = 1, double tsy = 1, double ssx = 1, double ssy = 1 )
   {
      bool istsx = tsx != 1;
      bool istsy = tsy != 1;
      bool isssx = ssx != 1;
      bool isssy = ssy != 1;

      bool masked = !mask.IsEmpty();

      if ( !istsx && !istsy && !isssx && !isssy && !masked )
      {
         Apply_Fast( image, src, b, i, delta );
         return;
      }

      int ns = src.NumberOfNominalChannels();
      int nt = image.NumberOfNominalChannels();
      int nm = mask.NumberOfNominalChannels();

      brush_data br( b );
      if ( istsx || istsy )
         br.radius = RoundI( Max( tsx, tsy ) * b.radius );

      if ( brush.IsEmpty() || brushData != br )
      {
         brush = br.CreateBrush();
         brushData = br;
      }

      const float* m = brush.Begin();

      double fm = 0; // calm compiler

      for ( int dy = -int( brushData.radius ); dy <= +int( brushData.radius ); ++dy )
      {
         int ys = (isssy ? RoundI( ssy*(i->targetPos.y - delta.y) ) : (i->targetPos.y - delta.y)) + dy;
         int yt = (istsy ? RoundI( tsy*i->targetPos.y ) : i->targetPos.y) + dy;

         if ( ys >= 0 && ys < src.Height() && yt >= 0 && yt < image.Height() )
         {
            for ( int dx = -int( brushData.radius ); dx <= +int( brushData.radius ); ++dx, ++m )
               if ( *m != 0 )
               {
                  int xs = (isssx ? RoundI( ssx*(i->targetPos.x - delta.x) ) : (i->targetPos.x - delta.x)) + dx;
                  int xt = (istsx ? RoundI( tsx*i->targetPos.x ) : i->targetPos.x) + dx;

                  if ( xs >= 0 && xs < src.Width() && xt >= 0 && xt < image.Width() )
                  {
                     double m0 = *m;
                     double m1 = 1 - m0;

                     for ( int c = 0; c < nt; ++c )
                     {
                        //double fm = 0; // calm compiler
                        if ( masked )
                        {
                           typename P2::sample vm = mask.Pixel( xt, yt, Min( c, nm-1 ) );
                           if ( vm == (maskInverted ? P2::MaxSampleValue() : P2::MinSampleValue()) )
                              continue;

                           P2::FromSample( fm, vm );

                           if ( maskInverted )
                              fm = 1 - fm;
                        }

                        typename P::sample& t = image.Pixel( xt, yt, c );

                        double ft;
                        P::FromSample( ft, t );

                        double fs;
                        P1::FromSample( fs, src.Pixel( xs, ys, Min( c, ns-1 ) ) );

                        double f = m1*ft + m0*fs;

                        if ( masked )
                           f = fm*f + (1 - fm)*ft;

                        t = P::ToSample( f );
                     }
                  }
               }
         }
         else
            m += 1 + (brushData.radius << 1);
      }

      ++image.Status();
   }

   template <class P, class P1>
   static void Apply_Fast( GenericImage<P>& image, const GenericImage<P1>& src,
                           const brush_data& b, cloner_iterator it, const Point& delta )
   {
      int nns = src.NumberOfNominalChannels();
      int nnt = image.NumberOfNominalChannels();
      int nt = nnt + Min( image.NumberOfAlphaChannels(), src.NumberOfAlphaChannels() );

      if ( brush.IsEmpty() || brushData != b )
      {
         brush = b.CreateBrush();
         brushData = b;
      }

      int db = 1 + (brushData.radius << 1);
      int dt = image.Width() - db;
      int ds = src.Width() - db;

      const float* m = brush.Begin();

      int yt = it->targetPos.y - brushData.radius;
      int xt0 = it->targetPos.x - brushData.radius;

      typename P::sample** pt = new typename P::sample*[ nt ];
      for ( int i = 0; i < nt; ++i )
         pt[i] = image.ScanLine( yt, i ) + xt0;

      int ys = yt - delta.y;
      int xs0 = xt0 - delta.x;

      const typename P1::sample** ps = new const typename P1::sample*[ nt ];
      *ps = src.ScanLine( ys, 0 ) + xs0;

      if ( nnt > 1 )
      {
         if ( nns > 1 )
         {
            ps[1] = src.ScanLine( ys, 1 ) + xs0;
            ps[2] = src.ScanLine( ys, 2 ) + xs0;
         }
         else
            ps[1] = ps[2] = *ps;
      }

      for ( int a = 0, n = nt - nnt; a < n; ++a )
         ps[nnt+a] = src.ScanLine( ys, nns+a ) + xs0;

      for ( int i = 0; i < db; ++i )
      {
         if ( ys >= 0 && ys < src.Height() && yt >= 0 && yt < image.Height() )
         {
            int xt = xt0;
            int xs = xs0;

            for ( int j = 0; j < db; ++j )
            {
               double m0 = *m++;

               if ( m0 != 0 && xs >= 0 && xs < src.Width() && xt >= 0 && xt < image.Width() )
               {
                  if ( m0 > 0.985 )
                  {
                     for ( int c = 0; c < nt; ++c )
                        *pt[c]++ = P::ToSample( *ps[c]++ );
                  }
                  else
                  {
                     double m1 = 1 - m0;

                     for ( int c = 0; c < nt; ++c )
                     {
                        double ft;
                        P::FromSample( ft, *pt[c] );

                        double fs;
                        P1::FromSample( fs, *ps[c]++ );

                        *pt[c]++ = P::ToSample( m1*ft + m0*fs );
                     }
                  }
               }
               else
               {
                  for ( int c = 0; c < nt; ++c )
                     ++pt[c], ++ps[c];
               }

               ++xt;
               ++xs;
            }
         }
         else
         {
            for ( int c = 0; c < nt; ++c )
               pt[c] += db, ps[c] += db;

            m += db;
         }

         ++yt;
         ++ys;

         for ( int c = 0; c < nt; ++c )
            pt[c] += dt, ps[c] += ds;
      }

      ++image.Status();
   }
};

CloneStampEngine::brush_data CloneStampEngine::brushData;
KernelFilter CloneStampEngine::brush;

// ----------------------------------------------------------------------------

bool CloneStampInstance::ExecuteOn( View& view )
{
   if ( view.IsPreview() )
      return false;

   if ( isInterfaceInstance )
   {
      TheCloneStampInterface->RestoreView();
      return true;
   }

   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );
   image.Status().Initialize( "Performing clone stamp actions", cloner.Length() );

   Console().EnableAbort();

   ImageVariant mask;

   ImageWindow window = view.Window();

   if ( window.IsMaskEnabled() )
   {
      ImageWindow maskWindow = window.Mask();
      if ( !maskWindow.IsNull() )
         mask = maskWindow.MainView().Image();
   }

   CloneStampEngine::Apply( image, mask, window.IsMaskInverted(), *this );

   return true;
}

// ----------------------------------------------------------------------------

bool CloneStampInstance::IsMaskable( const View& v, const ImageWindow& mask ) const
{
   return false;  // We handle masks directly
}

// ----------------------------------------------------------------------------

void CloneStampInstance::Apply( ImageVariant& image, const ImageVariant& src, const ImageVariant& mask, bool maskInverted,
                                const BrushData& brush, cloner_sequence::const_iterator i, const Point& delta )
{
   CloneStampEngine::Apply( image, src, mask, maskInverted, brush, i, delta );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void* CloneStampInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheCSActionIndexParameter )
      return &cloner[tableRow].actionIdx;
   if ( p == TheCSTargetXParameter )
      return &cloner[tableRow].targetPos.x;
   if ( p == TheCSTargetYParameter )
      return &cloner[tableRow].targetPos.y;

   if ( p == TheCSSourceIdentifierParameter )
      return actions[tableRow].sourceId.Begin();
   if ( p == TheCSSourceWidthParameter )
      return &actions[tableRow].sourceWidth;
   if ( p == TheCSSourceHeightParameter )
      return &actions[tableRow].sourceHeight;
   if ( p == TheCSSourceXParameter )
      return &actions[tableRow].sourcePos.x;
   if ( p == TheCSSourceYParameter )
      return &actions[tableRow].sourcePos.y;
   if ( p == TheCSRadiusParameter )
      return &actions[tableRow].brush.radius;
   if ( p == TheCSSoftnessParameter )
      return &actions[tableRow].brush.softness;
   if ( p == TheCSOpacityParameter )
      return &actions[tableRow].brush.opacity;

   if ( p == TheCSWidthParameter )
      return &width;
   if ( p == TheCSHeightParameter )
      return &height;
   if ( p == TheCSColorParameter )
      return &color;
   if ( p == TheCSBoundsColorParameter )
      return &boundsColor;

   return 0;
}

bool CloneStampInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheCSClonerTableParameter )
   {
      cloner.Clear();
      if ( sizeOrLength > 0 )
         cloner.Add( ClonerData(), sizeOrLength );
   }
   else if ( p == TheCSActionTableParameter )
   {
      actions.Clear();
      if ( sizeOrLength > 0 )
         actions.Add( ActionData(), sizeOrLength );
   }
   else if ( p == TheCSSourceIdentifierParameter )
   {
      actions[tableRow].sourceId.Clear();
      if ( sizeOrLength > 0 )
         actions[tableRow].sourceId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type CloneStampInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheCSClonerTableParameter )
      return cloner.Length();
   else if ( p == TheCSActionTableParameter )
      return actions.Length();
   else if ( p == TheCSSourceIdentifierParameter )
      return actions[tableRow].sourceId.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CloneStampInstance.cpp - Released 2017-04-14T23:07:12Z
