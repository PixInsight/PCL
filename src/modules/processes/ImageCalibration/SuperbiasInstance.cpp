//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0323
// ----------------------------------------------------------------------------
// SuperbiasInstance.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "SuperbiasInstance.h"
#include "SuperbiasParameters.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/MultiscaleMedianTransform.h>
#include <pcl/StdStatus.h>
#include <pcl/Vector.h>
#include <pcl/Version.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SuperbiasInstance::SuperbiasInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_columns( TheSBColumnsParameter->DefaultValue() ),
   p_rows( TheSBRowsParameter->DefaultValue() ),
   p_medianTransform( TheSBMedianTransformParameter->DefaultValue() ),
   p_excludeLargeScale( TheSBExcludeLargeScaleParameter->DefaultValue() ),
   p_multiscaleLayers( TheSBMultiscaleLayersParameter->DefaultValue() ),
   p_trimmingFactor( TheSBTrimmingFactorParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

SuperbiasInstance::SuperbiasInstance( const SuperbiasInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void SuperbiasInstance::Assign( const ProcessImplementation& p )
{
   const SuperbiasInstance* x = dynamic_cast<const SuperbiasInstance*>( &p );
   if ( x != nullptr )
   {
      p_columns = x->p_columns;
      p_rows = x->p_rows;
      p_medianTransform = x->p_medianTransform;
      p_excludeLargeScale = x->p_excludeLargeScale;
      p_multiscaleLayers = x->p_multiscaleLayers;
      p_trimmingFactor = x->p_trimmingFactor;
      o_superbiasViewId = x->o_superbiasViewId;
      o_superbiasViewId1 = x->o_superbiasViewId1;
   }
}

// ----------------------------------------------------------------------------

bool SuperbiasInstance::IsHistoryUpdater( const View& v ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SuperbiasInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.GetImage().IsComplexSample() )
   {
      whyNot = "Superbias cannot be executed on complex images.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool SuperbiasInstance::ExecuteOn( View& view )
{
   o_superbiasViewId.Clear();
   o_superbiasViewId1.Clear();

   if ( !p_rows )
      if ( !p_columns )
         return true;

   ImageWindow resultWindow, resultWindow1;

   try
   {
      AutoViewLock lock( view, false/*lock*/ );
      lock.LockForWrite();

      Console console;
      StandardStatus status;

      console.EnableAbort();

      Image B;
      ImageVariant( &B ).CopyImage( view.Image() );

      resultWindow = ImageWindow( B.Width(), B.Height(), B.NumberOfChannels(),
                                  32/*bitsPerSample*/, true/*floatSample*/, B.IsColor(),
                                  true/*initialProcessing*/,
                                  (p_rows != p_columns) ? "superbias" : "superbias_v" );
      if ( resultWindow.IsNull() )
         throw Error( "Superbias: Unable to create output image window." );

      CreateKeywords( resultWindow, view.Window(), p_columns );

      o_superbiasViewId = resultWindow.MainView().Id();

      if ( p_rows && p_columns )
      {
         resultWindow1 = ImageWindow( B.Width(), B.Height(), B.NumberOfChannels(),
                                      32/*bitsPerSample*/, true/*floatSample*/, B.IsColor(),
                                      true/*initialProcessing*/,
                                      "superbias_h" );
         if ( resultWindow1.IsNull() )
            throw Error( "Superbias: Unable to create output image window (rows)." );

         CreateKeywords( resultWindow1, view.Window(), false/*columns*/ );

         o_superbiasViewId1 = resultWindow1.MainView().Id();
      }

      ImageVariant Sv = resultWindow.MainView().Image();
      Image& S = static_cast<Image&>( *Sv );
      {
         B.SetStatusCallback( &status );
         S.SetStatusCallback( &status );

         if ( p_medianTransform )
         {
            MultiscaleMedianTransform T( Max( 1, p_multiscaleLayers ) );
            for ( int i = 0; i < T.NumberOfLayers(); ++i )
               T.DisableLayer( i );
            T << B;
            T >> S;
         }
         else
         {
            static const float B3S_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
            StarletTransform::WaveletScalingFunction B3S( SeparableFilter( B3S_hv, B3S_hv, 5 ) );
            StarletTransform T( B3S, Max( 1, p_multiscaleLayers ) );
            for ( int i = 0; i < T.NumberOfLayers(); ++i )
               T.DisableLayer( i );
            T << B;
            T >> S;
         }

         B.SetStatusCallback( 0 );
         S.SetStatusCallback( 0 );

         if ( p_excludeLargeScale )
            B -= S;
      }

      if ( !resultWindow1.IsNull() )
         static_cast<Image&>( *resultWindow1.MainView().Image() ).Apply( S );

      if ( p_columns )
      {
         StatusMonitor monitor;
         monitor.SetCallback( &status );
         monitor.Initialize( "<end><cbr>Computing superbias columns", B.NumberOfChannels()*B.Width() );

         for ( int c = 0, n = B.Height(), t = TruncInt( p_trimmingFactor*n ); c < B.NumberOfChannels(); ++c )
            for ( int i = 0; i < B.Width(); ++i, ++monitor )
            {
               DVector v( n );
               B.GetColumn( v.Begin(), i, c );
               v.Sort();
               double m = pcl::Mean( v.At( t ), v.At( n-t ) );
               for ( int j = 0; j < n; ++j )
                  S( i, j, c ) += m;
            }
      }

      if ( p_rows )
      {
         ImageVariant S1v;
         Image* S1;
         if ( p_columns )
         {
            S1v = resultWindow1.MainView().Image();
            S1 = static_cast<Image*>( S1v.ImagePtr() );
         }
         else
         {
            S1 = &S;
         }

         StatusMonitor monitor;
         monitor.SetCallback( &status );
         monitor.Initialize( "<end><cbr>Computing superbias rows", B.NumberOfChannels()*B.Height() );

         for ( int c = 0, n = B.Width(), t = TruncInt( p_trimmingFactor*n ); c < B.NumberOfChannels(); ++c )
            for ( int i = 0; i < B.Height(); ++i, ++monitor )
            {
               DVector v( n );
               B.GetRow( v.Begin(), i, c );
               v.Sort();
               double m = pcl::Mean( v.At( t ), v.At( n-t ) );
               for ( int j = 0; j < n; ++j )
                  (*S1)( j, i, c ) += m;
            }

         if ( p_columns )
            S1->Truncate();
      }

      S.Truncate();

      if ( !resultWindow1.IsNull() )
      {
         resultWindow1.Show();
         resultWindow1.ZoomToFit( false/*allowZoomIn*/ );
      }

      resultWindow.Show();
      resultWindow.ZoomToFit( false/*allowZoomIn*/ );

      return true;
   }
   catch ( ... )
   {
      if ( !resultWindow.IsNull() )
         resultWindow.Close();
      if ( !resultWindow1.IsNull() )
         resultWindow1.Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

void SuperbiasInstance::CreateKeywords( ImageWindow& resultWindow, const ImageWindow& sourceWindow, bool columns )
{
   FITSKeywordArray keywords;
   sourceWindow.GetKeywords( keywords );

   for ( size_type i = 0; i < keywords.Length(); )
      if ( !keywords[i].name.CompareIC( "IMAGETYP" ) ||
           !keywords[i].name.CompareIC( "SBIASDIR" ) )
         keywords.Remove( keywords.At( i ) );
      else
         ++i;
   keywords.Add( FITSHeaderKeyword( "IMAGETYP", "Master Bias", "Type of image" ) );
   keywords.Add( FITSHeaderKeyword( "SBIASDIR", columns ? "Vertical" : "Horizontal", "Superbias orientation" ) );

   keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "Generation with " + PixInsightVersion::AsString() ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Generation with " + Module->ReadableVersion() ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Generation with Superbias process" ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Superbias.columns: " + IsoString( bool( p_columns ) ) ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Superbias.rows: " + IsoString( bool( p_rows ) ) ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Superbias.medianTransform: " + IsoString( bool( p_medianTransform ) ) ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), "Superbias.excludeLargeScale: " + IsoString( bool( p_excludeLargeScale ) ) ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Superbias.multiscaleLayers: %d", p_multiscaleLayers ) ) );
   keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), IsoString().Format( "Superbias.trimmingFactor: %.3f", p_trimmingFactor ) ) );

   resultWindow.SetKeywords( keywords );
}

// ----------------------------------------------------------------------------

void* SuperbiasInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSBColumnsParameter )
      return &p_columns;
   if ( p == TheSBRowsParameter )
      return &p_rows;
   if ( p == TheSBMedianTransformParameter )
      return &p_medianTransform;
   if ( p == TheSBExcludeLargeScaleParameter )
      return &p_excludeLargeScale;
   if ( p == TheSBMultiscaleLayersParameter )
      return &p_multiscaleLayers;
   if ( p == TheSBTrimmingFactorParameter )
      return &p_trimmingFactor;
   if ( p == TheSBSuperbiasViewIdParameter )
      return o_superbiasViewId.Begin();
   if ( p == TheSBSuperbiasViewId1Parameter )
      return o_superbiasViewId1.Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool SuperbiasInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type/*tableRow*/ )
{
   if ( p == TheSBSuperbiasViewIdParameter )
   {
      o_superbiasViewId.Clear();
      if ( sizeOrLength > 0 )
         o_superbiasViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheSBSuperbiasViewId1Parameter )
   {
      o_superbiasViewId1.Clear();
      if ( sizeOrLength > 0 )
         o_superbiasViewId1.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type SuperbiasInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSBSuperbiasViewIdParameter )
      return o_superbiasViewId.Length();
   if ( p == TheSBSuperbiasViewId1Parameter )
      return o_superbiasViewId1.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SuperbiasInstance.cpp - Released 2017-07-18T16:14:18Z
