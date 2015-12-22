//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0294
// ----------------------------------------------------------------------------
// LRGBCombinationInstance.h - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#ifndef __LRGBCombinationInstance_h
#define __LRGBCombinationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Point.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// LRGBCombinationInstance
// ----------------------------------------------------------------------------

class LRGBCombinationInstance : public ProcessImplementation
{
public:

   LRGBCombinationInstance( const MetaProcess* );
   LRGBCombinationInstance( const LRGBCombinationInstance& );

   virtual bool Validate( pcl::String& info );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View& v, String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   // -------------------------------------------------------------------------

private:

   // Array indices: 0=R, 1=G, 2=B, 3=L

   pcl_bool channelEnabled[ 4 ];
   String   channelId[ 4 ];
   double   channelWeight[ 4 ];
   float    luminanceMTF;
   float    saturationMTF;
   pcl_bool clipHighlights;
   pcl_bool noiseReduction;
   uint32   numberOfRemovedWaveletLayers;
   uint32   numberOfProtectedWaveletLayers;

   friend class LRGBCombinationInterface;
   friend class LRGBChannelSourceSelectionDialog;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LRGBCombinationInstance_h

// ----------------------------------------------------------------------------
// EOF LRGBCombinationInstance.h - Released 2015/12/18 08:55:08 UTC
