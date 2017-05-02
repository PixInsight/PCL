// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarMonitorInstance.h - Released 2014/01/30 00:00:00 UTC
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

#ifndef __StarMonitorInstance_h
#define __StarMonitorInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Matrix.h>

#include <pcl/FITSHeaderKeyword.h>

#include "Star.h"
#include "StarMonitorParameters.h"
#include "PSF.h"

namespace pcl {

//#define debug 1

// ----------------------------------------------------------------------------

class StarMonitorInstance : public ProcessImplementation {
public:

   StarMonitorInstance(const MetaProcess*);
   StarMonitorInstance(const StarMonitorInstance&);

   virtual void Assign(const ProcessImplementation&);

   virtual bool CanExecuteOn(const View&, String& whyNot) const;
   virtual bool IsHistoryUpdater(const View& v) const;

   virtual bool ExecuteOn(View&);

   virtual void* LockParameter(const MetaParameter*, size_type /*tableRow*/);
   virtual bool AllocateParameter(size_type sizeOrLength, const MetaParameter* p, size_type tableRow);
   virtual size_type ParameterLength(const MetaParameter* p, size_type tableRow) const;

   bool ProcessFile(String);


   // -------------------------------------------------------------------------

private:

   /*
    * PSF fitting functions
    */
   struct PSFOptions {
      pcl_bool circular;
      pcl_bool gaussian;
   };
   PSFOptions psfOptions;

   struct Item {
      String path;
      Array<Star> stars;
      size_type qty;
      double B, A, sx, sy, FWHMx, FWHMy, FWHM_Min, FWHM_Max, theta, mad;
      float xScale, yScale; // image scale in arcsec/px

      Item() : path(), stars(), qty(), B(), A(), sx(), sy(), FWHMx(), FWHMy(), FWHM_Min(), FWHM_Max(), theta(), mad(), xScale(), yScale() {
      }

      Item(const String& aPath, const Array<Star>& aStars,
              const size_type aqty,
              const double aB, const double aA,
              const double asx, const double asy,
              const double aFWHMx, const double aFWHMy,
              const double aFWHM_Min, const double aFWHM_Max,
              const double atheta, const double amad,
              const float axScale, const float ayScale
              )
      : path(aPath), stars(aStars),
      qty(aqty), B(aB), A(aA), sx(asx), sy(asy),
      FWHMx(aFWHMx), FWHMy(aFWHMy),
      FWHM_Min(aFWHM_Min), FWHM_Max(aFWHM_Max),
      theta(atheta), mad(amad),
      xScale(axScale), yScale(ayScale) {
      }

      Item(const Item & x)
      : path(x.path), stars(x.stars), qty(x.qty), B(x.B), A(x.A), sx(x.sx), sy(x.sy),
      FWHMx(x.FWHMx), FWHMy(x.FWHMy), FWHM_Min(x.FWHM_Min), FWHM_Max(x.FWHM_Max),
      theta(x.theta), mad(x.mad),
      xScale(x.xScale), yScale(x.yScale) {
      }

   };
   typedef Array<Item> item_list;

   // Star detector parameters
   int32 structureLayers;
   int32 noiseLayers;
   int32 hotPixelFilterRadius;
   float sensitivity;
   float peakResponse;
   float maxStarDistortion;
   pcl_bool invert;

   // Working images & folder
   item_list targets;
   String monitoredFolder;

   // Format hints
   String inputHints;

   // temporary variable
   float xScale, yScale; // image scale in arcsec/px

   //Image scale parameters
   pcl_enum scaleMode; // std keyword (FOCALLEN), custom keyword or literal value
   float scaleValue; // image scale in arc seconds per pixel

   // Alert parameters
   pcl_bool alertCheck; //Enable/Disable Alert
   pcl_bool alertFWHMxCheck;
   pcl_bool alertRoundnessCheck;
   pcl_bool alertBackgroundCheck;
   pcl_bool alertStarQuantityCheck;

   String alertExecute; //Path to ExecutableProgram
   String alertArguments; //Execute with this command parameters
   float alertFWHMx;
   float alertRoundness;
   double alertBackground;
   size_type alertStarQuantity;


   Array<Star> GetStars(const ImageVariant& image, const UInt8Image& mask);
   void UpdateFileData(const Array<Star>& S, const String& path);
   void LoadTargetImage(ImageVariant& image, const String& path, const String& inputHints);

   friend class StarMonitorInterface;
};


// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarMonitorInstance_h

// ****************************************************************************
// EOF StarMonitorInstance.h - Released 2014/01/30 00:00:00 UTC
