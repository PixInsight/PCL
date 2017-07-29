//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 01.00.05.0290
// ----------------------------------------------------------------------------
// RestorationFilterInstance.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
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

#ifndef __RestorationFilterInstance_h
#define __RestorationFilterInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h>

#include "RestorationFilterParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RestorationFilterInstance : public ProcessImplementation
{
public:

   RestorationFilterInstance( const MetaProcess* );
   RestorationFilterInstance( const RestorationFilterInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   bool CreatePSF( Image& ) const;

   // -------------------------------------------------------------------------

private:

   // PSF
   pcl_enum psfMode;                // PSF type: Parametric, motion blur, or external image
   float    psfSigma;               // standard deviation of Parametric PSF
   float    psfShape;               // kurtosys of Parametric-family PSF
   float    psfAspectRatio;         // axis ratio of Parametric PSF
   float    psfRotationAngle;       // rotation angle of Parametric PSF, degrees
   float    psfMotionLength;        // length of motion blur PSF
   float    psfMotionRotationAngle; // angle of motion blur PSF
   String   psfViewId;              // PSF view id (for external PSF only)

   // Noise estimates
   double   K;                      // noise estimation, Wiener
   double   gamma;                  // noise estimation, Constrained Least Squares

   // Filter parameters
   pcl_enum algorithm;
   float    amount;                 // processed:original ratio
   pcl_bool toLuminance;            // apply to luminance for color images
   pcl_bool linear;                 // apply to CIE Y instead of CIE L*

   // Deringing
   pcl_bool deringing;              // enable deringing
   float    deringingDark;          // deringing amount, dark rings
   float    deringingBright;        // deringing amount, bright rings
   pcl_bool outputDeringingMaps;    // generate deringing map images

   // Dynamic Range Extension
   float    rangeLow;               // dynamic range extension, low range
   float    rangeHigh;              // dynamic range extension, high range

   friend class RestorationFilterEngine;
   friend class RestorationFilterInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RestorationFilterInstance_h

// ----------------------------------------------------------------------------
// EOF RestorationFilterInstance.h - Released 2017-05-02T09:43:01Z
