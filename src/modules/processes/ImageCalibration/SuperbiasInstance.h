//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.03.05.0262
// ----------------------------------------------------------------------------
// SuperbiasInstance.h - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __SuperbiasInstance_h
#define __SuperbiasInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SuperbiasInstance : public ProcessImplementation
{
public:

   SuperbiasInstance( const MetaProcess* );
   SuperbiasInstance( const SuperbiasInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool IsHistoryUpdater( const View& v ) const;

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter*, size_type tableRow ) const;

private:

   pcl_bool p_columns;           // average columns
   pcl_bool p_rows;              // average rows
   pcl_bool p_medianTransform;   // use MMT instead of ST
   pcl_bool p_excludeLargeScale; // remove large-scale layers before averaging
   int32    p_multiscaleLayers;  // number of multiscale analysis layers (excluding residual)
   float    p_trimmingFactor;    // trimming factor for mean calculation
   String   o_superbiasViewId;   // 1 orientation: columns or rows / 2 orientations: columns
   String   o_superbiasViewId1;  // 2 orientations: rows

   void CreateKeywords( ImageWindow&, const ImageWindow&, bool columns );

   friend class SuperbiasProcess;
   friend class SuperbiasInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SuperbiasInstance_h

// ----------------------------------------------------------------------------
// EOF SuperbiasInstance.h - Released 2015/11/26 16:00:13 UTC
