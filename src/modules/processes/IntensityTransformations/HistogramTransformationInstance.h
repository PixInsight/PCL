// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard IntensityTransformations Process Module Version 01.07.00.0287
// ****************************************************************************
// HistogramTransformationInstance.h - Released 2014/11/14 17:19:22 UTC
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

#ifndef __HistogramTransformationInstance_h
#define __HistogramTransformationInstance_h

#include <pcl/ProcessImplementation.h>

#include "HistogramTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class HistogramTransformationInstance : public ProcessImplementation
{
public:

   HistogramTransformationInstance( const MetaProcess* );
   HistogramTransformationInstance( const HistogramTransformationInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View& v, String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual bool CanExecuteOn( const ImageVariant& image, String& whyNot ) const;
   virtual bool ExecuteOn( ImageVariant& image, const IsoString& hints );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   // -------------------------------------------------------------------------

   double MidtonesBalance( int i ) const
   {
      return m[i];
   }

   void SetMidtonesBalance( int i, double b )
   {
      m[i] = pcl::Range( b, 0.0, 1.0 );
   }

   double ShadowsClipping( int i ) const
   {
      return c0[i];
   }

   double HighlightsClipping( int i ) const
   {
      return c1[i];
   }

   void SetShadowsClipping( int i, double c )
   {
      c0[i] = pcl::Range( c, 0.0, 1.0 );
      if ( c1[i] < c0[i] )
         pcl::Swap( c0[i], c1[i] );
   }

   void SetHighlightsClipping( int i, double c )
   {
      c1[i] = pcl::Range( c, 0.0, 1.0 );
      if ( c1[i] < c0[i] )
         pcl::Swap( c0[i], c1[i] );
   }

   double LowRange( int i ) const
   {
      return r0[i];
   }

   double HighRange( int i ) const
   {
      return r1[i];
   }

   void SetLowRange( int i, double r )
   {
      r0[i] = Min( 0.0, r );
   }

   void SetHighRange( int i, double r )
   {
      r1[i] = Max( 1.0, r );
   }

   bool IsIdentity( int i ) const
   {
      return m[i] == 0.5 && c0[i] == 0 && c1[i] == 1 && r0[i] == 0 && r1[i] == 1;
   }

   void Reset()
   {
      for ( int i = 0; i < 5; ++i )
      {
         m[i] = 0.5;
         c0[i] = r0[i] = 0;
         c1[i] = r1[i] = 1;
      }
   }

   void Preview( UInt16Image& ) const;

private:

   // Channel indexes: 0=R, 1=G, 2=B, 3=RGB/K, 4=Alpha
   double m[ 5 ];           // midtones balance
   double c0[ 5 ], c1[ 5 ]; // shadows and highlights clipping points
   double r0[ 5 ], r1[ 5 ]; // shadows and highlights dynamic range expansion
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramTransformationInstance_h

// ****************************************************************************
// EOF HistogramTransformationInstance.h - Released 2014/11/14 17:19:22 UTC
