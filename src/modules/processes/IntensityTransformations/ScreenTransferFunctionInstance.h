//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0351
// ----------------------------------------------------------------------------
// ScreenTransferFunctionInstance.h - Released 2015/12/18 08:55:08 UTC
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

#ifndef __ScreenTransferFunctionInstance_h
#define __ScreenTransferFunctionInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Array.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/MetaParameter.h> // for pcl_enum

#include "ScreenTransferFunctionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

struct STF
{
   float m;       // midtones balance
   float c0, c1;  // shadows and highlights clipping points
   float r0, r1;  // shadows and highlights dynamic range expansion

   STF()
   {
      Reset();
   }

   STF( const STF& x ) : m( x.m ), c0( x.c0 ), c1( x.c1 ), r0( x.r0 ), r1( x.r1 )
   {
   }

   void Reset()
   {
      m = 0.5F;
      c0 = r0 = 0;
      c1 = r1 = 1;
   }

   bool operator ==( const STF& x ) const
   {
      return m == x.m && c0 == x.c0 && c1 == x.c1 && r0 == x.r0 && r1 == x.r1;
   }
};

class ScreenTransferFunctionInstance : public ProcessImplementation
{
public:

   typedef Array<STF>   stf_list;

   ScreenTransferFunctionInstance( const MetaProcess* );
   ScreenTransferFunctionInstance( const ScreenTransferFunctionInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool IsHistoryUpdater( const View& ) const;
   virtual bool IsMaskable( const View&, const ImageWindow& ) const;

   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter*, size_type tableRow ) const;

   void GetViewSTF( const View& );

   void ApplyTo( View& view ) const;

   const STF& operator []( int i ) const
   {
      return stf[i];
   }

   STF& operator []( int i )
   {
      return stf[i];
   }

   pcl_enum Interaction() const
   {
      return interaction;
   }

private:

   stf_list stf;

   // The following parameter is updated by ScreenTransferFunctionInterface and
   // is used to import STF instances by HistogramTransformationInterface.
   pcl_enum interaction;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ScreenTransferFunctionInstance_h

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionInstance.h - Released 2015/12/18 08:55:08 UTC
