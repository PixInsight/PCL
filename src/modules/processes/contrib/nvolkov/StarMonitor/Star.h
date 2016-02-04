// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// Star.h - Released 2016/01/14 19:32:59 UTC
// ****************************************************************************
// This file is part of the standard StarMonitor PixInsight module.
//
// Copyright (c) 2003-2016, Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Star_h
#define __PCL_Star_h

#include <pcl/Point.h>
#include "PSF.h"

namespace pcl {

   // ----------------------------------------------------------------------------

   class Star {
   public:

      FPoint pos; // Centroid position in pixels, image coordinates
      unsigned count; // Number of pixels
      float total; // Integrated flux (pixel values normalized to the [0,1] range)
      float peak; // peak value
      Rect rect; // rectangle around the star
      PSFData psf;

      Star(float x, float y, unsigned n, float t, float p, Rect r) : pos(x, y), count(n), total(t), peak(p), rect(r), psf() {
      }

      Star(const Star& s) : pos(s.pos), count(s.count), total(s.total), peak(s.peak), rect(s.rect) {
      }

      bool operator ==(const Star& s) const {
         return pos == s.pos;
      }

      // Stars are sorted in descending order (brighter comes first)

      bool operator<(const Star& s) const {
         return total > s.total;
      }

      void SetPSF(PSFData f) {
         psf = f;
      }
   };

   // ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Star_h

// ****************************************************************************
// EOF Star.h - Released 2016/01/14 19:32:59 UTC
