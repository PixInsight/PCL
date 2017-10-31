//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorStarDetector.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#ifndef __SubframeSelectorStarDetector_h
#define __SubframeSelectorStarDetector_h

#include <pcl/Point.h>
#include <pcl/Image.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct Star
{
   Point position;
   int size;
   double flux;
   double peak;
   double normalized;
   double background;

   Star( const Point& p ) :
           position( p ),
           size( 0 ),
           flux( 0 ),
           peak( 0 ),
           normalized( 0 ),
           background( 0 )
   {
   }

   Star( const Star& x ) :
           position( x.position ),
           size( x.size ),
           flux( x.flux ),
           peak( x.peak ),
           normalized( x.normalized ),
           background( x.background )
   {
   }
};

// ----------------------------------------------------------------------------

class SubframeSelectorStarDetector
{
public:

   SubframeSelectorStarDetector();

   Array<Star> GetStars( ImageVariant* image );

   bool showStarDetectionMaps = false;

   /*
    * Number of wavelet layers for structure detection. (default=5)
    */
   int structureLayers = 5;

   /*
    * ### DEPRECATED
    * Number of small-scale wavelet layers for noise suppression. (default=0)
    */
   int noiseLayers = 0;

   /*
    * Half size in pixels of a morphological median filter, for hot pixel
    * removal. (default=1)
    */
   int hotPixelFilterRadius = 1;

   /*
    * Whether the hot pixel filter removal should be applied to the image used
    * for star detection, or only to the working image used to build the
    * structure map. (default=false)
    *
    * By setting this parameter to true, the detection algorithm is completely
    * robust to hot pixels (of sizes not larger than hotPixelFilterRadius), but
    * it is also less sensitive, so less stars will in general be detected.
    * With the default value of false, some hot pixels may be wrongly detected
    * as stars but the number of true stars detected will generally be larger.
    */
   bool applyHotPixelFilterToDetectionImage = false;

   /*
    * Half size in pixels of a Gaussian convolution filter applied for noise
    * reduction. Useful for star detection in low-SNR images. (default=0)
    *
    * Setting the value of this parameter > 0 implies
    * applyHotPixelFilterToDetectionImage=true.
    */
   int noiseReductionFilterRadius = 0;

   /*
    * Sensitivity of the star detector device - smaller values mean more
    * sensitivity. (default=0.1)
    */
   float sensitivity = 0.1;

   /*
    * Peak response of the star detector device - larger values are more
    * tolerant with relatively flat structures. (default=0.8)
    */
   float peakResponse = 0.8;

   /*
    * Maximum distortion allowed, relative to a perfect square. The distortion
    * of a perfect circle is pi/4. (default=0.5)
    */
   float maxDistortion = 0.5;

   /*
    * Stars with peak values greater than this value won't be detected.
    * (default=1)
    */
   float upperLimit = 1.0;

   /*
    * Local background is evaluated for each star on an inflated rectangular
    * region around the star detection structure. backgroundExpansion is the inflation
    * distance in pixels. (default=3)
    */
   int backgroundExpansion = 3;

   /*
    * Stretch factor for the barycenter search algorithm, in sigma units.
    * Increase it to make the algorithm more robust to nearby structures, such
    * as multiple/crowded stars and small nebular features. However, too large
    * of a stretch factor will make the algorithm less accurate. (default=1.5)
    */
   float xyStretch = 1.5;

private:

   void HotPixelFilter( ImageVariant& image );
   void StructureMap( ImageVariant& image );
   Star GetStarParameters( ImageVariant& image, const Rect& rect, const Array<Point>& stars );

};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __SubframeSelectorStarDetector_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorStarDetector.h - Released 2017-08-01T14:26:58Z
