//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0336
// ----------------------------------------------------------------------------
// DynamicCropInstance.h - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#ifndef __DynamicCropInstance_h
#define __DynamicCropInstance_h

#include <pcl/Point.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/Rectangle.h>

#include "DynamicCropParameters.h"

namespace pcl
{

class PixelInterpolation;

// ----------------------------------------------------------------------------

class DynamicCropInstance : public ProcessImplementation
{
public:

   DynamicCropInstance( const MetaProcess* );
   DynamicCropInstance( const DynamicCropInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool IsMaskable( const View&, const ImageWindow& ) const;

   virtual UndoFlags UndoMode( const View& ) const;

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool BeforeExecution( View& );
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

private:

   DPoint   p_center;            // Center of cropping rectangle
   double   p_width, p_height;   // Dimensions of cropping area
   double   p_angle;             // Rotation angle, radians
   double   p_scaleX, p_scaleY;  // Scaling factors
   pcl_bool p_optimizeFast;      // Optimize fast rotations: +/-90d, 180d
   pcl_enum p_interpolation;     // Interpolation algorithm
   float    p_clampingThreshold; // deringing clamping for bicubic spline and Lanczos
   float    p_smoothness;        // smoothness for cubic filter interpolations
   DPoint   p_resolution;        // horizontal and vertical resolution in pixels
   pcl_bool p_metric;            // metric resolution?
   pcl_bool p_forceResolution;   // set resolution of target image window?
   DVector  p_fillColor;         // Filling values for unused areas (R/K,G,B,A)
   pcl_bool p_noGUIMessages;    // only show warning messages on the console

   friend class DynamicCropEngine;
   friend class DynamicCropInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicCropInstance_h

// ----------------------------------------------------------------------------
// EOF DynamicCropInstance.h - Released 2017-04-14T23:07:12Z
