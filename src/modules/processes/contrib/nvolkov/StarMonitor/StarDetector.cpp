// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarDetector.cpp - Released 2014/01/30 00:00:00 UTC
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

#include "StarDetector.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/Selection.h>

#include <float.h>

namespace pcl
{

   // ----------------------------------------------------------------------------

#define SD_ITERATIONS   4
#define SD_COST         (((hotPixelFilterRadius > 0) ? 1 : 0) \
                         + SD_ITERATIONS*(4 + 2*structureLayers) \
                         + ((mask != 0) ? 2 : 1))
#define BKG_DELTA       4

   // ----------------------------------------------------------------------------

   static void GetStructureMap(Image& map, int structureLayers, int noiseLayers, int hotPixelFilterRadius,
           const UInt8Image* mask)
   {
      bool initializeStatus = map.Status().IsInitializationEnabled();
      if (initializeStatus)
      {
         map.Status().Initialize("Structure map", map.NumberOfPixels() * SD_COST);
         map.Status().DisableInitialization();
      }

      if (hotPixelFilterRadius > 0) // N
      {
         MedianFilter M;
         if (hotPixelFilterRadius > 1)
         {
            CircularStructure C(2 * hotPixelFilterRadius + 1);
            MorphologicalTransformation(M, C) >> map;
         }
         else
         {
            BoxStructure B(3);
            MorphologicalTransformation(M, B) >> map;
         }
      }

      static const float __3x3Linear_hv[] = {0.50F, 1.00F, 0.50F};
      SeparableFilter F(__3x3Linear_hv, __3x3Linear_hv, 3);

      ATrousWaveletTransform W(F);
      W.SetNumberOfLayers(structureLayers);
      for (int j = noiseLayers; j < structureLayers; ++j)
         W.DisableLayer(j);

      for (int i = 0; i < SD_ITERATIONS; ++i)
      {
         W << map; // structureLayers*N
         Image tmp;
         tmp.Status() = map.Status();
         W >> tmp; // structureLayers*N
         tmp.Truncate(); // N
         map.Status() = tmp.Status();
         map -= tmp; // N
         map.Truncate(); // N
         map.Rescale(); // N
      }

      map.Binarize(0.0005); // N

      if (mask != 0)
         map.Mul(*mask); // N

      if (initializeStatus)
         map.Status().EnableInitialization();
   }

   static void GetStructures(Image& img, int structureLayers, int noiseLayers, int hotPixelFilterRadius,
           const UInt8Image* mask)
   {
      bool initializeStatus = img.Status().IsInitializationEnabled();
      if (initializeStatus)
      {
         img.Status().Initialize("Extracting structures", img.NumberOfPixels()*(SD_COST + 1));
         img.Status().DisableInitialization();
      }

      Image map(img);
      GetStructureMap(map, structureLayers, noiseLayers, hotPixelFilterRadius, mask);
      img.Status() = map.Status();
      img.Mul(map); // N

      if (initializeStatus)
         img.Status().EnableInitialization();
   }

   // Compute star barycenter parameters, total flux and mean local background.
   // Returns star intensity corrected for peak response.

   inline static float StarValues(float& x, float& y, float& sz, float& z1, float& b,
           const Image& img, const Rect& r, const Array<Point>& sp, float peakResponse)
   {
      // Define star region including local background pixels
      int dx = Max(1, Min(BKG_DELTA, r.Width() >> 1));
      int dy = Max(1, Min(BKG_DELTA, r.Height() >> 1));
      Rect r1(r.x0 - dx, r.y0 - dy, r.x1 + dx, r.y1 + dy);
      r1.Intersect(img.Bounds());

      // Pixel set
      Array<float> Z;
      Z.Reserve(r1.Area());

      // Calculate the mean local background as the median of background pixels
      for (int i = r1.y0; i < r1.y1; ++i)
         for (int j = r1.x0; j < r1.x1; ++j)
            if (!sp.Contains (Point(j, i)))
               Z.Add(img.Pixel(j, i));
      Z.Sort();
      b = Z[Z.Length() >> 1];
      if ((Z.Length() & 1) == 0)
         b = (b + Z[(Z.Length() >> 1) - 1]) / 2;

      // Compute barycenter coordinates and peak value corrected for peak response.
      x = y = sz = z1 = 0;
      for (Array<Point>::const_iterator i = sp.Begin(); i != sp.End(); ++i)
      {
         float z = img.Pixel(*i);
         x += z * i->x;
         y += z * i->y;
         sz += z;
         if (z > z1)
            z1 = z;
      }
      x /= sz;
      y /= sz;
      return z1 - (1 - peakResponse) * sz / sp.Length();
   }

   static void GetStars(StarDetector::star_list& S, Image& img,
           int structureLayers, int noiseLayers, int hotPixelFilterRadius,
           float sensitivity, float peakResponse, float maxDistortion, bool invert,
           const UInt8Image* mask)
   {
      // If the invert flag is set, then we are looking for dark structures on a
      // bright background.
      if (invert)
         img.Invert();

      // Rescale to [0,1] (but don't bother the user with progress info)
      {
         StatusCallback* s = img.GetStatusCallback();
         img.SetStatusCallback(0);
         img.Rescale();
         img.SetStatusCallback(s);
      }

      // Structure map
      Image map(img);
      GetStructureMap(map, structureLayers, noiseLayers, hotPixelFilterRadius, 0);

      bool initializeStatus = img.Status().IsInitializationEnabled();
      if (initializeStatus)
      {
         img.Status().Initialize("Detecting stars",
                 size_type(img.Width() - 1) * size_type(img.Height() - 1));
         img.Status().DisableInitialization();
      }

      for (int y0 = 0, x1 = img.Width() - 1, y1 = img.Height() - 1; y0 < y1; ++y0)
         for (int x0 = 0; x0 < x1; ++x0, ++img.Status())
         {
            // Exclude background pixels and already visited pixels
            if (map.Pixel(x0, y0) == 0)
               continue;

            // Star bounding rectangle
            Rect r(x0, y0, x0 + 1, y0 + 1);

            // Current star points
            Array<Point> sp;

            // Grow star region downward
            for (int y = y0, x = x0, xa, xb;;)
            {
               // Add this pixel to the current star
               sp.Add(Point(x, y));

               // Explore left segment of this row
               for (xa = x; xa > 0;)
               {
                  if (map.Pixel(xa - 1, y) == 0)
                     break;
                  --xa;
                  sp.Add(Point(xa, y));
               }

               // Explore right segment of this row
               for (xb = x; xb < x1;)
               {
                  if (map.Pixel(xb + 1, y) == 0)
                     break;
                  ++xb;
                  sp.Add(Point(xb, y));
               }

               // xa and xb are now the left and right boundary limits,
               // respectively, of this row in the current star.

               if (xa < r.x0) // update left boundary
                  r.x0 = xa;

               if (xb >= r.x1) // update right boundary
                  r.x1 = xb + 1; // bottom-right corner excluded (PCL-specific)

               // Prepare for next row
               ++y;

               // Decide whether we are done with this star now, or if
               // there is at least one more row that must be explored.

               bool nextRow = false;

               // We explore the next row from left to right. We'll continue
               // gathering pixels if we find at least one nonzero pixel.
               for (x = xa; x <= xb; ++x)
                  if (map.Pixel(x, y) != 0)
                  {
                     nextRow = true;
                     break;
                  }

               if (!nextRow)
                  break;

               // Update bottom boundary
               r.y1 = y + 1; // Rectangle<int> *excludes* the bottom-right corner

               // Terminate if we reach the last row of the image
               if (y == y1)
                  break;
            }

            // If this is a reliable star, compute its barycenter coordinates and
            // add it to the star list.
            //
            // Rejection criteria:
            //
            // * If this star is touching a border of the image, reject it. We
            //   cannot compute an accurate position for a clipped star.
            //
            // * Too small stars are rejected. This mainly prevents inclusion of
            //   hot (or cold) pixels. This condition is enforced by the noise
            //   reduction performed during the structure detection phase.
            //
            // * Too large stars are rejected. This prevents inclusion of
            //   extended nonstellar objects and saturated bright stars. This is
            //   also part of the structure detection algorithm.
            //
            // * Too elongated stars are rejected. The maxDistortion parameter
            //   determines the maximum distortion allowed. A perfect square has
            //   distortion = 1. The distortion of a perfect circle is pi/4, or
            //   about 0.75.
            //
            // * We don't trust too faint stars, or stars whose centroids are too
            //   misplaced with respect to their peak positions.

            if (r.Width() > 2 && r.Height() > 2 && r.y0 > 0 && r.y1 <= y1 && r.x0 > 0 && r.x1 <= x1)
            {
               int d = Max(r.Width(), r.Height());
               if (float( sp.Length()) / d / d > maxDistortion)
               {
                  // Compute star parameters
                  float b; // local background
                  float x, y; // barycenter coordinates
                  float sz; // total flux
                  float z1; // peak value
                  float z = StarValues(x, y, sz, z1, b, img, r, sp, peakResponse); // intensity

                  // * Reject this structure if its star peak value is under
                  //   sensitivity with respect to the mean local background.
                  if (b == 0 || (z - b) / b > sensitivity)
                  {
                     int ix = RoundI(x);
                     int iy = RoundI(y);
                     // * Ignore masked stars, if a mask has been specified.
                     if (mask == 0 || mask->Pixel(ix, iy) != 0)
                        // * Reject star if the pixel at the barycenter position is
                        //   too misplaced with respect to the peak position.
                        if (img.Pixel(ix, iy) > 0.7 * z1)
                           // * Reject overburned stars
                           if (z1 < 1.0)
                              S.Add(Star(x, y, sp.Length(), sz - b, z1, r)); // subtract local background
                  }
               }
            }

            // Erase this structure
            for (Array<Point>::const_iterator i = sp.Begin(); i != sp.End(); ++i)
               map.Pixel(*i) = 0;
         }

      if (initializeStatus)
         img.Status().EnableInitialization();
   }

   /*
    * Get the HSI intensity component of the current selection in img.
    */
   template <class P>
   static void GetIntensity(Image& I, const GenericImage<P>& img)
   {
      Rect r = img.SelectedRectangle();
      if (!r.IsRect()) // this cannot happen
         throw Error("Internal error: Invalid image selection.");

      I.AllocateData(r.Width(), r.Height(), 1, ColorSpace::Gray);
      float* g = *I;

      int n = img.NumberOfSelectedChannels();
      if (n == 1 || !img.IsColor())
      {
         // Just one component selected or img is a grayscale image
         if (img.IsFullSelection())
            P::Get(g, *img, I.NumberOfPixels());
         else
         {
            const typename P::sample* f = img.PixelAddress(r.LeftTop(), img.FirstSelectedChannel());
            for (int i = 0; i < I.Height(); ++i, f += img.Width(), g += I.Width())
               P::Get(g, f, I.Width());
         }
      }
      else
      {
         // Intensity from the three nominal components of a color image img
         const typename P::sample* f0 = img.PixelAddress(r.LeftTop(), 0);
         const typename P::sample* f1 = img.PixelAddress(r.LeftTop(), 1);
         const typename P::sample* f2 = img.PixelAddress(r.LeftTop(), 2);
         if (img.IsFullSelection())
            for (float* gN = g + I.NumberOfPixels(); g < gN; ++f0, ++f1, ++f2, ++g)
            {
               float m, M;
               P::FromSample(m, Min(*f0, Min(*f1, *f2)));
               P::FromSample(M, Max(*f0, Max(*f1, *f2)));
               *g = (m + M) / 2;
            }
         else
            for (int i = 0, dw = img.Width() - I.Width(); i < I.Height(); ++i, f0 += dw, f1 += dw, f2 += dw)
               for (int j = 0; j < I.Width(); ++j, ++f0, ++f1, ++f2, ++g)
               {
                  float m, M;
                  P::FromSample(m, Min(*f0, Min(*f1, *f2)));
                  P::FromSample(M, Max(*f0, Max(*f1, *f2)));
                  *g = (m + M) / 2;
               }
      }
   }

   template <class P>
   static void GetStars(StarDetector::star_list& S, const GenericImage<P>& img,
   int structureLayers, int noiseLayers, int hotPixelFilterRadius,
   float sensitivity, float peakResponse, float maxDistortion, bool invert,
   const UInt8Image* mask)
   {
      img.PushSelections();
      img.ResetSelection();

      Image tmp;
      GetIntensity(tmp, img);
      tmp.Status() = img.Status();

      GetStars(S, tmp, structureLayers, noiseLayers, hotPixelFilterRadius,
              sensitivity, peakResponse, maxDistortion, invert, mask);

      img.PopSelections();
   }

   StarDetector::StarDetector(const ImageVariant& v,
           int structureLayers, int noiseLayers, int hotPixelFilterRadius,
           float sensitivity, float peakResponse, float maxDistortion, bool invert,
           const UInt8Image* mask)
   {
      if (mask != 0 && mask->IsEmpty())
         mask = 0;

      if (!v.IsComplexSample())
         if (v.IsFloatSample())
            switch (v.BitsPerSample())
            {
               case 32: GetStars(S, *static_cast<const Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius,
                          sensitivity, peakResponse, maxDistortion, invert, mask);
                  break;
               case 64: GetStars(S, *static_cast<const DImage*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius,
                          sensitivity, peakResponse, maxDistortion, invert, mask);
                  break;
            }
         else
            switch (v.BitsPerSample())
            {
               case 8: GetStars(S, *static_cast<const UInt8Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius,
                          sensitivity, peakResponse, maxDistortion, invert, mask);
                  break;
               case 16: GetStars(S, *static_cast<const UInt16Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius,
                          sensitivity, peakResponse, maxDistortion, invert, mask);
                  break;
               case 32: GetStars(S, *static_cast<const UInt32Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius,
                          sensitivity, peakResponse, maxDistortion, invert, mask);
                  break;
            }

      // Sort stars in decreasing brightness order
      S.Sort();
   }

   // ----------------------------------------------------------------------------

   static void __GetStructureMap(Image& img,
           int structureLayers, int noiseLayers, int hotPixelFilterRadius, bool invert,
           const UInt8Image* mask)
   {
      if (img.IsColor())
      {
         Image tmp;
         GetIntensity(tmp, img);
         tmp.Status() = img.Status();

         if (invert)
            tmp.Invert();

         GetStructureMap(tmp, structureLayers, noiseLayers, hotPixelFilterRadius, mask);

         img = tmp;
      }
      else
      {
         if (invert)
            img.Invert();

         GetStructureMap(img, structureLayers, noiseLayers, hotPixelFilterRadius, mask);
      }
   }

   template <class P>
   static void __GetStructureMap(GenericImage<P>& img,
   int structureLayers, int noiseLayers, int hotPixelFilterRadius, bool invert,
   const UInt8Image* mask)
   {
      Image tmp;
      if (!img.IsColor())
         img.SelectChannel(0);
      GetIntensity(tmp, img);
      tmp.Status() = img.Status();

      if (invert)
         tmp.Invert();

      GetStructureMap(tmp, structureLayers, noiseLayers, hotPixelFilterRadius, mask);

      img = tmp;
   }

   void StarDetector::GetStructureMap(ImageVariant& v,
           int structureLayers, int noiseLayers, int hotPixelFilterRadius, bool invert,
           const UInt8Image* mask)
   {
      if (mask != 0 && mask->IsEmpty())
         mask = 0;

      if (!v.IsComplexSample())
         if (v.IsFloatSample())
            switch (v.BitsPerSample())
            {
               case 32: __GetStructureMap(*static_cast<Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
               case 64: __GetStructureMap(*static_cast<DImage*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
            }
         else
            switch (v.BitsPerSample())
            {
               case 8: __GetStructureMap(*static_cast<UInt8Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
               case 16: __GetStructureMap(*static_cast<UInt16Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
               case 32: __GetStructureMap(*static_cast<UInt32Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
            }
   }

   // ----------------------------------------------------------------------------

   static void __GetStructures(Image& img,
           int structureLayers, int noiseLayers, int hotPixelFilterRadius, bool invert,
           const UInt8Image* mask)
   {
      if (img.IsColor())
      {
         Image tmp;
         GetIntensity(tmp, img);
         tmp.Status() = img.Status();

         if (invert)
            tmp.Invert();

         GetStructures(tmp, structureLayers, noiseLayers, hotPixelFilterRadius, mask);

         img = tmp;
      }
      else
      {
         if (invert)
            img.Invert();

         GetStructures(img, structureLayers, noiseLayers, hotPixelFilterRadius, mask);
      }
   }

   template <class P>
   static void __GetStructures(GenericImage<P>& img,
   int structureLayers, int noiseLayers, int hotPixelFilterRadius, bool invert,
   const UInt8Image* mask)
   {
      Image tmp;
      if (!img.IsColor())
         img.SelectChannel(0);
      GetIntensity(tmp, img);
      tmp.Status() = img.Status();

      if (invert)
         tmp.Invert();

      GetStructures(tmp, structureLayers, noiseLayers, hotPixelFilterRadius, mask);

      img = tmp;
   }

   void StarDetector::GetStructures(ImageVariant& v,
           int structureLayers, int noiseLayers, int hotPixelFilterRadius, bool invert,
           const UInt8Image* mask)
   {
      if (mask != 0 && mask->IsEmpty())
         mask = 0;

      if (!v.IsComplexSample())
         if (v.IsFloatSample())
            switch (v.BitsPerSample())
            {
               case 32: __GetStructures(*static_cast<Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
               case 64: __GetStructures(*static_cast<DImage*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
            }
         else
            switch (v.BitsPerSample())
            {
               case 8: __GetStructures(*static_cast<UInt8Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
               case 16: __GetStructures(*static_cast<UInt16Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
               case 32: __GetStructures(*static_cast<UInt32Image*> (v.ImagePtr()),
                          structureLayers, noiseLayers, hotPixelFilterRadius, invert, mask);
                  break;
            }
   }

   // ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF StarDetector.cpp - Released 2014/01/30 00:00:00 UTC
