//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.12.01.0368
// ----------------------------------------------------------------------------
// HDRCompositionInstance.h - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __HDRCompositionInstance_h
#define __HDRCompositionInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Array.h>

#include "HDRCompositionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class HDRCompositionInstance : public ProcessImplementation
{
public:

   HDRCompositionInstance( const MetaProcess* );
   HDRCompositionInstance( const HDRCompositionInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   struct ImageItem
   {
      pcl_bool  enabled;
      String    path;
      size_type exposure;

      ImageItem( const String& p = String() ) : enabled( true ), path( p ), exposure( 0 )
      {
      }

      ImageItem( const ImageItem& x ) : enabled( x.enabled ), path( x.path ), exposure( x.exposure )
      {
      }

      bool IsDefined() const
      {
         return !path.IsEmpty();
      }

      bool operator <( const ImageItem& x ) const
      {
         return enabled ? (x.enabled ? exposure > x.exposure : true) : false;
      }

      bool operator ==( const ImageItem& x ) const
      {
         return path == x.path;
      }
   };

   typedef Array<ImageItem>  image_list;

   image_list  p_images;                  // source images
   String      p_inputHints;              // input format hints
   float       p_maskBinarizingThreshold; // binarizing threshold for composition masks
   int32       p_maskSmoothness;          // radius of low-pass filter (>= 1)
   int32       p_maskGrowth;              // radius of dilation filter (>= 0)
   int32       p_replaceLargeScales;      // number of small-scale MMT layers replaced in combined images - 4
   pcl_bool    p_autoExposures;           // sort images by relative exposures automatically; manual order otherwise
   pcl_bool    p_rejectBlack;             // reject black pixels
   pcl_bool    p_useFittingRegion;        // use a rectangular region for image fitting; entire image otherwise
   Rect        p_fittingRect;             // image fitting rectangle
   pcl_bool    p_generate64BitResult;     // generate a 64-bit floating point HDR image instead of 32-bit
   pcl_bool    p_outputMasks;             // output the composition mask images
   pcl_bool    p_closePreviousImages;     // close existing mask and HDR images before running

   void SortImages(); // sort by relative exposures

   friend class HDRCompositionFile;
   friend class HDRCompositionEngine;
   friend class HDRCompositionInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HDRCompositionInstance_h

// ----------------------------------------------------------------------------
// EOF HDRCompositionInstance.h - Released 2017-04-14T23:07:12Z
