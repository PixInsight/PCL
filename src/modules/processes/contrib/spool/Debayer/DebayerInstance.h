//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.05.00.0236
// ----------------------------------------------------------------------------
// DebayerInstance.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#ifndef __DebayerInstance_h
#define __DebayerInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class DebayerInstance : public ProcessImplementation
{
public:

   DebayerInstance( const MetaProcess* );
   DebayerInstance( const DebayerInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   /*
    * Process parameters
    */
   pcl_enum p_bayerPattern;
   pcl_enum p_debayerMethod;
   pcl_bool p_evaluateNoise;
   pcl_enum p_noiseEvaluationAlgorithm;
   pcl_bool p_showImages;
   String   p_cfaSourceFilePath;

   // working CFA pattern
   pcl_enum m_bayerPattern;

   /*
    * Read-only output properties
    */
   String     o_imageId;
   FVector    o_noiseEstimates;
   FVector    o_noiseFractions;
   StringList o_noiseAlgorithms;

   virtual void DoSuperPixel( Image& target, const ImageVariant& source );
   virtual void DoBilinear( Image& target, const ImageVariant& source );
   virtual void DoVNG( Image& target, const ImageVariant& source );

   friend class DebayerProcess;
   friend class DebayerInterface;
   friend class DebayerEngine;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __DebayerInstance_h

// ----------------------------------------------------------------------------
// EOF DebayerInstance.h - Released 2017-05-02T09:43:01Z
