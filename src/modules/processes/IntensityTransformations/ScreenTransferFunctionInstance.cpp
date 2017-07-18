//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0397
// ----------------------------------------------------------------------------
// ScreenTransferFunctionInstance.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ScreenTransferFunctionInstance.h"

#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ScreenTransferFunctionInstance::ScreenTransferFunctionInstance( const MetaProcess* m ) :
ProcessImplementation( m ), stf( 4 ), interaction( STFInteraction::Default )
{
}

ScreenTransferFunctionInstance::ScreenTransferFunctionInstance( const ScreenTransferFunctionInstance& x ) :
ProcessImplementation( x ), stf( 4 ), interaction( STFInteraction::Default )
{
   Assign( x );
}

void ScreenTransferFunctionInstance::Assign( const ProcessImplementation& p )
{
   const ScreenTransferFunctionInstance* x = dynamic_cast<const ScreenTransferFunctionInstance*>( &p );
   if ( x != 0 )
   {
      stf.Assign( x->stf );
      interaction = x->interaction;
   }
}

bool ScreenTransferFunctionInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

bool ScreenTransferFunctionInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

bool ScreenTransferFunctionInstance::ExecuteOn( View& view )
{
   ApplyTo( view );
   return true;
}

void* ScreenTransferFunctionInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSTFInteractionParameter )
      return &interaction;
   if ( tableRow < 4 )
      if ( p == TheSTFShadowsClippingParameter )
         return &stf[tableRow].c0;
      else if ( p == TheSTFHighlightsClippingParameter )
         return &stf[tableRow].c1;
      else if ( p == TheSTFMidtonesBalanceParameter )
         return &stf[tableRow].m;
      else if ( p == TheSTFLowRangeParameter )
         return &stf[tableRow].r0;
      else if ( p == TheSTFHighRangeParameter )
         return &stf[tableRow].r1;
   return 0;
}

bool ScreenTransferFunctionInstance::AllocateParameter( size_type length, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSTFSetParameter )
   {
      stf.Clear();
      if ( length > 0 )
         stf.Add( STF(), length );
      return true;
   }

   return false;
}

size_type ScreenTransferFunctionInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheSTFSetParameter )
      return 4;
   return 0;
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInstance::GetViewSTF( const View& view )
{
   if ( !view.IsNull() )
   {
      View::stf_list F;
      view.GetScreenTransferFunctions( F );
      for ( int c = 0; c < 4; ++c )
      {
         STF& f = stf[c];
         const HistogramTransformation& H = F[c];
         f.m = H.MidtonesBalance();
         f.c0 = H.ShadowsClipping();
         f.c1 = H.HighlightsClipping();
         f.r0 = H.LowRange();
         f.r1 = H.HighRange();
      }

      interaction = view.IsColor() ? STFInteraction::SeparateChannels : STFInteraction::Grayscale;
   }
}

void ScreenTransferFunctionInstance::ApplyTo( View& view ) const
{
   View::stf_list F;
   for ( int c = 0; c < 4; ++c )
   {
      const STF& f = stf[c];
      F.Add( HistogramTransformation( f.m, f.c0, f.c1, f.r0, f.r1 ) );
   }

   view.SetScreenTransferFunctions( F );
   if ( !view.AreScreenTransferFunctionsEnabled() )
      view.EnableScreenTransferFunctions();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionInstance.cpp - Released 2017-07-18T16:14:18Z
