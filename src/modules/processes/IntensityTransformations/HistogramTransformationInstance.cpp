//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.00.0314
// ----------------------------------------------------------------------------
// HistogramTransformationInstance.cpp - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "HistogramTransformationInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ImageWindow.h>
#include <pcl/MuteStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramTransformationInstance::HistogramTransformationInstance( const MetaProcess* P ) :
ProcessImplementation( P )
{
   Reset();
}

HistogramTransformationInstance::HistogramTransformationInstance( const HistogramTransformationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void HistogramTransformationInstance::Assign( const ProcessImplementation& p )
{
   const HistogramTransformationInstance* x = dynamic_cast<const HistogramTransformationInstance*>( &p );
   if ( x != 0 )
      for ( int i = 0; i < 5; ++i )
      {
         m[i] = x->m[i];
         c0[i] = x->c0[i];
         c1[i] = x->c1[i];
         r0[i] = x->r0[i];
         r1[i] = x->r1[i];
      }
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   return CanExecuteOn( view.Image(), whyNot );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class HistogramEngine
{
public:

   static void Apply( ImageVariant& image, const HistogramTransformationInstance& instance, bool consoleOutput = true )
   {
      Console console;

      HistogramTransformation H3( instance.MidtonesBalance( 3 ),
                                  instance.ShadowsClipping( 3 ),
                                  instance.HighlightsClipping( 3 ),
                                  instance.LowRange( 3 ),
                                  instance.HighRange( 3 ) );

      if ( image.IsColor() )
      {
         for ( int c = 0; c < 3; ++c )
         {
            if ( consoleOutput )
               console << "<end><cbr>Processing channel #" << c << ": ";

            HistogramTransformation H( instance.MidtonesBalance( c ),
                                       instance.ShadowsClipping( c ),
                                       instance.HighlightsClipping( c ),
                                       instance.LowRange( c ),
                                       instance.HighRange( c ) );
            H.Add( H3 );

            if ( H.IsIdentityTransformationSet() )
            {
               if ( consoleOutput )
                  console << "&lt;* Identity *&gt;\n";
            }
            else
            {
               image.SelectChannel( c );
               H >> image;
            }
         }
      }
      else
      {
         if ( consoleOutput )
            console << "<end><cbr>Processing gray channel: ";

         if ( H3.IsIdentityTransformation() )
         {
            if ( consoleOutput )
               console << "&lt;* Identity *&gt;\n";
         }
         else
         {
            image.SelectChannel( 0 );
            H3 >> image;
         }
      }

      if ( image.HasAlphaChannels() )
      {
         if ( consoleOutput )
            console << "<end><cbr>Processing alpha channel: ";

         HistogramTransformation H( instance.MidtonesBalance( 4 ),
                                    instance.ShadowsClipping( 4 ),
                                    instance.HighlightsClipping( 4 ),
                                    instance.LowRange( 4 ),
                                    instance.HighRange( 4 ) );

         if ( H.IsIdentityTransformation() )
         {
            if ( consoleOutput )
               console << "&lt;* Identity *&gt;\n";
         }
         else
         {
            image.SelectChannel( image.NumberOfNominalChannels() );
            H >> image;
         }
      }
   }
};

// ----------------------------------------------------------------------------

void HistogramTransformationInstance::Preview( UInt16Image& image ) const
{
   ImageVariant v( &image );
   MuteStatus status;
   v.SetStatusCallback( &status );
   HistogramEngine::Apply( v, *this, false/*consoleOutput*/ );
   v.ResetSelections();
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   HistogramEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInstance::CanExecuteOn( const ImageVariant& image, String& whyNot ) const
{
   if ( image.IsComplexSample() )
   {
      whyNot = "HistogramTransformation cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool HistogramTransformationInstance::ExecuteOn( ImageVariant& image, const IsoString& hints )
{
   if ( image.IsComplexSample() )
      return false;

   bool enableConsole = true;
   bool enableAbort = true;

   IsoStringList theHints;
   hints.Break( theHints, ' ', true/*trim*/ );
   theHints.Remove( IsoString() );
   for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
   {
      if ( *i == "no-console-output" )
         enableConsole = false;
      else if ( *i == "console-output" )
         enableConsole = true;
      else if ( *i == "disable-abort" )
         enableAbort = false;
      else if ( *i == "enable-abort" )
         enableAbort = true;
   }

   if ( enableAbort )
      Console().EnableAbort();

   StandardStatus status;
   if ( enableConsole )
      image.SetStatusCallback( &status );

   HistogramEngine::Apply( image, *this, enableConsole );

   return true;
}

// ----------------------------------------------------------------------------

void* HistogramTransformationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheShadowsClippingParameter )
      return c0+tableRow;
   if ( p == TheHighlightsClippingParameter )
      return c1+tableRow;
   if ( p == TheMidtonesBalanceParameter )
      return m+tableRow;
   if ( p == TheLowRangeParameter )
      return r0+tableRow;
   if ( p == TheHighRangeParameter )
      return r1+tableRow;
   return 0;
}

bool HistogramTransformationInstance::AllocateParameter( size_type /*sizeOrLength*/, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheHistogramTransformationTableParameter )
   {
      for ( int c = 0; c < 5; ++c )
      {
         m[c] = 0.5;
         c0[c] = r0[c] = 0;
         c1[c] = r1[c] = 1;
      }

      return true;
   }

   return false;
}

size_type HistogramTransformationInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheHistogramTransformationTableParameter )
      return 5;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HistogramTransformationInstance.cpp - Released 2015/10/08 11:24:40 UTC
