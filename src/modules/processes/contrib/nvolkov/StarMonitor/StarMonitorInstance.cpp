// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarMonitorInstance.cpp - Released 2016/01/14 19:32:59 UTC
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

#include "StarMonitorInstance.h"
#include "StarMonitorInterface.h"

#include "StarDetector.h"

#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/StdStatus.h>
#include <pcl/Graphics.h>
#include <pcl/ProcessInterface.h> // for ProcessEvents()

namespace pcl
{

   // ----------------------------------------------------------------------------

   StarMonitorInstance::StarMonitorInstance(const MetaProcess* m) :
   ProcessImplementation(m),
   structureLayers(int32(TheSMStructureLayersParameter->DefaultValue())),
   noiseLayers(int32(TheSMNoiseLayersParameter->DefaultValue())),
   hotPixelFilterRadius(int32(TheSMHotPixelFilterRadiusParameter->DefaultValue())),
   sensitivity(TheSMSensitivityParameter->DefaultValue()),
   peakResponse(TheSMPeakResponseParameter->DefaultValue()),
   maxStarDistortion(TheSMMaxStarDistortionParameter->DefaultValue()),
   invert(TheSMInvertParameter->DefaultValue()),
   targets(),
   monitoredFolder(String()),
   inputHints(),
   scaleMode(SMScaleMode::Default),
   scaleValue(TheSMScaleValueParameter->DefaultValue()),

   alertCheck(TheSMAlertCheckParameter->DefaultValue()),
   alertFWHMxCheck(TheSMAlertFWHMxCheckParameter->DefaultValue()),
   alertRoundnessCheck(TheSMAlertRoundnessCheckParameter->DefaultValue()),
   alertBackgroundCheck(TheSMAlertBackgroundCheckParameter->DefaultValue()),
   alertStarQuantityCheck(TheSMAlertStarQuantityCheckParameter->DefaultValue()),
   alertExecute(TheSMAlertExecuteParameter->DefaultValue()),
   alertArguments(TheSMAlertArgumentsParameter->DefaultValue()),
   alertFWHMx(TheSMAlertFWHMxParameter->DefaultValue()),
   alertRoundness(TheSMAlertRoundnessParameter->DefaultValue()),
   alertBackground(TheSMAlertBackgroundParameter->DefaultValue()),
   alertStarQuantity(TheSMAlertStarQuantityParameter->DefaultValue())
   {
   }

   StarMonitorInstance::StarMonitorInstance(const StarMonitorInstance& x) :
   ProcessImplementation(x)
   {
      Assign(x);
   }

   void StarMonitorInstance::Assign(const ProcessImplementation& p)
   {
      const StarMonitorInstance* x = dynamic_cast<const StarMonitorInstance*> (&p);
      if (x != 0)
      {
         structureLayers = x->structureLayers;
         noiseLayers = x->noiseLayers;
         hotPixelFilterRadius = x->hotPixelFilterRadius;
         sensitivity = x->sensitivity;
         peakResponse = x->peakResponse;
         maxStarDistortion = x->maxStarDistortion;
         invert = x->invert;
         monitoredFolder = x->monitoredFolder;
         targets = x->targets;

         inputHints = x->inputHints;

         scaleMode = x->scaleMode;
         scaleValue = x->scaleValue;

         alertCheck = x->alertCheck;
         alertFWHMxCheck = x->alertFWHMxCheck;
         alertRoundnessCheck = x->alertRoundnessCheck;
         alertBackgroundCheck = x->alertBackgroundCheck;
         alertStarQuantityCheck = x->alertStarQuantityCheck;
         alertExecute = x->alertExecute;
         alertArguments = x->alertArguments;
         alertFWHMx = x->alertFWHMx;
         alertRoundness = x->alertRoundness;
         alertBackground = x->alertBackground;
         alertStarQuantity = x->alertStarQuantity;
      }
   }

   bool StarMonitorInstance::CanExecuteOn(const View& view, String& whyNot) const
   {
      if (view.Image().IsComplexSample())
      {
         whyNot = "StarMonitor cannot be executed on complex images.";
         return false;
      }

      whyNot.Clear();
      return true;
   }

   bool StarMonitorInstance::IsHistoryUpdater(const View& view) const
   {
      return false;
   }


   // ----------------------------------------------------------------------------

   template <class P>
   static void LoadImageFile(GenericImage<P>& image, FileFormatInstance& file)
   {
      if (!file.ReadImage(image))
         throw CatchedException();
   }

   static void LoadImageFile(ImageVariant& v, FileFormatInstance& file, const ImageDescriptionArray& images, int index)
   {
      if (!file.SelectImage(index))
         throw CatchedException();

      v.CreateSharedImage(images[index].options.ieeefpSampleFormat, false, images[index].options.bitsPerSample);

      if (v.IsFloatSample())
         switch (v.BitsPerSample())
         {
            case 32: LoadImageFile(*static_cast<Image*> (v.ImagePtr()), file);
               break;
            case 64: LoadImageFile(*static_cast<DImage*> (v.ImagePtr()), file);
               break;
         }
      else
         switch (v.BitsPerSample())
         {
            case 8: LoadImageFile(*static_cast<UInt8Image*> (v.ImagePtr()), file);
               break;
            case 16: LoadImageFile(*static_cast<UInt16Image*> (v.ImagePtr()), file);
               break;
            case 32: LoadImageFile(*static_cast<UInt32Image*> (v.ImagePtr()), file);
               break;
         }
   }
   // ----------------------------------------------------------------------------

   static double KeywordValue(const FITSKeywordArray& keywords, const IsoString& keyName)
   {
      for (FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i)
         if (!i->name.CompareIC(keyName))
         {
            if (i->IsNumeric())
            {
               double v;
               if (i->GetNumericValue(v))
                  return v;
            }
            break;
         }
      return 0;
   }
   // ----------------------------------------------------------------------------

   void StarMonitorInstance::LoadTargetImage(ImageVariant& image, const String& path, const String& inputHints)
   {
      Console().WriteLn("<end><cbr>Loading target image:");
      Console().WriteLn(path);

      FileFormat format(File::ExtractExtension(path), true, false);
      FileFormatInstance file(format);

      ImageDescriptionArray images;

      if (!file.Open(images, path, inputHints))
         throw CatchedException();

      if (images.IsEmpty())
         throw Error(path + ": Empty target image.");
      if (images.Length() > 1)
         throw Error(path + ": Multiple images cannot be used as registration targets.");

      LoadImageFile(image, file, images, 0);

      xScale = yScale = 0;
      FITSKeywordArray keywords; // FITS keywords
      file.Extract(keywords);
      double focalLength = KeywordValue(keywords, "FOCALLEN"); // mm
      double xPixSize = KeywordValue(keywords, "XPIXSZ"); // um
      double yPixSize = KeywordValue(keywords, "YPIXSZ"); // um
      if (focalLength > 0)
      {
         if (yPixSize <= 0)
            yPixSize = xPixSize;
         if (xPixSize > 0)
         {
            xScale = 3.6 * Deg(2 * ArcTan(xPixSize, 2 * focalLength));
            yScale = 3.6 * Deg(2 * ArcTan(yPixSize, 2 * focalLength));
         }
      }

      file.Close();
   }

   // ----------------------------------------------------------------------------

   Array<Star> StarMonitorInstance::GetStars(const ImageVariant& image, const UInt8Image& mask = UInt8Image())
   {
      StarDetector D(image,
              structureLayers,
              noiseLayers,
              hotPixelFilterRadius,
              sensitivity,
              peakResponse,
              maxStarDistortion,
              invert,
              &mask);

      Console console;
      console.WriteLn(String().Format("<end><cbr>%u stars found.", D.S.Length()));

      if (D.S.Length() == 0)
         throw Error("No stars found");

      for (size_type i = 0; i < D.S.Length(); ++i)
      {
         PSFFit F(image, D.S[i].pos, D.S[i].rect, PSFFit::Gaussian, false);
         if (F) D.S[i].SetPSF(F.psf);
      }

      return D.S;
   }

   // ----------------------------------------------------------------------------

   static void PrintStarList(const Array<Star>& S, const size_t start, const int qty, const String& msg)
   {
      Console().WriteLn("<br>" + msg);
      Console().Write("x \ty \tflux \tpeak \tFWHMx \tFWHMy \ttheta \tMAD");
#ifdef debug
      Console().Write("\tpixels \tw \th");
#endif
      Console().WriteLn();

      size_t begin;
      size_t end;
      if (qty < 0)
      {
         end = start;
         begin = size_t(Max(int( S.Length()) + qty, 0));
      }
      else
      {
         begin = start;
         end = Min(S.Length(), start + size_t(qty));
      }

      for (size_type i = begin; i < end; ++i)
      {
         Console().Write(String().Format("%.2f\t%.2f\t%i\t%i\t", S[i].pos.x, S[i].pos.y, int(S[i].total * 65535), int(S[i].peak * 65535)));
         if (S[i].psf)
         {
            Console().Write(String().Format("%f\t%f\t", S[i].psf.FWHMx(), S[i].psf.FWHMy()));
            Console().Write(String().Format("%.2f\t%f\t", S[i].psf.theta, S[i].psf.mad));
         }
         else
         {
            Console().Write("NO CONVERGENCE");
         }
#ifdef debug
         Console().Write(String().Format("%u\t", S[i].count));
         Console().Write(String().Format("%u\t %u\t", S[i].rect.Width(), S[i].rect.Height()));
#endif
         Console().WriteLn();
      }
   }

   inline static void PrintData(const Array<Star>& S)
   {
      PrintStarList(S, 0, 10, "Top bright.");
      PrintStarList(S, S.Length(), -10, "Top dim.");

      double FWHMx = 0;
      double FWHMy = 0;
      size_type qty = 0;

      for (size_type i = 0; i < S.Length(); ++i)
      {
         if (S[i].psf)
         {
            FWHMx += S[i].psf.FWHMx();
            FWHMy += S[i].psf.FWHMy();
            qty++;
         }
      }
      FWHMx /= qty;
      FWHMy /= qty;
      Console().WriteLn(String().Format("<br>Average FWHMx:%fpx\t FWHMy:%fpx\t", FWHMx, FWHMy));
   }

   void StarMonitorInstance::UpdateFileData(const Array<Star>& S, const String& path)
   {
      double B, A, sx, sy, theta, mad, FWHMx, FWHMy, FWHM_Min, FWHM_Max;
      B = A = sx = sy = theta = mad = FWHMx = FWHMy = 0;
      FWHM_Min = DBL_MAX;
      FWHM_Max = DBL_MIN;
      int N = 0;
      for (size_type i = 0; i < S.Length(); ++i)
      {
         if (S[i].psf)
         {
            const PSFData* p = &S[i].psf;
            B += p->B;
            A += p->A;
            sx += p->sx;
            sy += p->sy;
            theta += p->theta;
            //beta += p->beta;
            mad += p->mad;
            FWHM_Min = Min(FWHM_Min, Min(p->FWHMx(), p->FWHMy()));
            FWHM_Max = Max(FWHM_Max, Max(p->FWHMx(), p->FWHMy()));
            FWHMx += p->FWHMx();
            FWHMy += p->FWHMy();
            ++N;
         }
      }
      if (N == 0)
         throw Error("Internal Error: Insufficient PSF data.");

      Item item(path, S, N, B / N, A / N, sx / N, sy / N, FWHMx / N, FWHMy / N, FWHM_Min, FWHM_Max, theta / N, mad / N, xScale, yScale);
      targets.Add(item);

      Console().WriteLn(String().Format("<br>Min FWHM:%fpx\t Max FWHM:%fpx\t", FWHM_Min, FWHM_Max));
   }

   bool StarMonitorInstance::ProcessFile(String path)
   {
      ImageVariant targetImage;

      try
      {
         Console().Show();
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         LoadTargetImage(targetImage, path, inputHints);

         StandardStatus status;
         targetImage.ImagePtr()->SetStatusCallback(&status);

         Array<Star> S = GetStars(targetImage);
         PrintData(S);
         UpdateFileData(S, path);

         ProcessInterface::ProcessEvents();

         targetImage.Free();

         Exception::DisableConsoleOutput();
         Exception::EnableGUIOutput();

         return true;
      } // try

      catch (...)
      {
         Exception::DisableConsoleOutput();
         Exception::EnableGUIOutput();

         targetImage.Free();

         return false;
      }
   }

   bool StarMonitorInstance::ExecuteOn(View& view)
   {
      {
         String why;
         if (!CanExecuteOn(view, why))
            throw Error(why);
      }

      try
      {
         Console().Show();
         view.LockForWrite();

         ImageVariant targetImage;
         targetImage = view.Image();
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();
         Console().EnableAbort();
         StandardStatus status;
         targetImage.ImagePtr()->SetStatusCallback(&status);

         Array<Star> S = GetStars(targetImage);
         PrintData(S);

         ProcessInterface::ProcessEvents();

         Exception::DisableConsoleOutput();
         Exception::EnableGUIOutput();

         view.Unlock();

         return true;
      }

      catch (...)
      {
         Exception::DisableConsoleOutput();
         Exception::EnableGUIOutput();

         view.Unlock(); // Never leave a view locked!
         throw;
      }
   }

   void* StarMonitorInstance::LockParameter(const MetaParameter* p, size_type tableRow)
   {
      if (p == TheSMStructureLayersParameter) return &structureLayers;
      if (p == TheSMNoiseLayersParameter) return &noiseLayers;
      if (p == TheSMHotPixelFilterRadiusParameter) return &hotPixelFilterRadius;
      if (p == TheSMSensitivityParameter) return &sensitivity;
      if (p == TheSMPeakResponseParameter) return &peakResponse;
      if (p == TheSMMaxStarDistortionParameter) return &maxStarDistortion;
      if (p == TheSMInvertParameter) return &invert;

      if (p == TheSMMonitoredFolderParameter) return monitoredFolder.Begin();
      if (p == TheSMTargetImageParameter) return targets[tableRow].path.Begin();
      if (p == TheSMStarQuantityParameter) return &targets[tableRow].qty;
      if (p == TheSMBackgroundParameter) return &targets[tableRow].B;
      if (p == TheSMAmplitudeParameter) return &targets[tableRow].A;
      if (p == TheSMRadiusXParameter) return &targets[tableRow].sx;
      if (p == TheSMRadiusYParameter) return &targets[tableRow].sy;
      if (p == TheSMFWHMxParameter) return &targets[tableRow].FWHMx;
      if (p == TheSMFWHMyParameter) return &targets[tableRow].FWHMy;
      if (p == TheSMRotationAngleParameter) return &targets[tableRow].theta;
      if (p == TheSMMADParameter) return &targets[tableRow].mad;
      if (p == TheSMxScaleParameter) return &targets[tableRow].xScale;
      if (p == TheSMyScaleParameter) return &targets[tableRow].yScale;

      if (p == TheSMInputHintsParameter) return inputHints.Begin();

      if (p == TheSMScaleModeParameter) return &scaleMode;
      if (p == TheSMScaleValueParameter) return &scaleValue;

      if (p == TheSMAlertCheckParameter) return &alertCheck;
      if (p == TheSMAlertFWHMxCheckParameter) return &alertFWHMxCheck;
      if (p == TheSMAlertRoundnessCheckParameter) return &alertRoundnessCheck;
      if (p == TheSMAlertBackgroundCheckParameter) return &alertBackgroundCheck;
      if (p == TheSMAlertStarQuantityCheckParameter)return &alertStarQuantityCheck;
      if (p == TheSMAlertExecuteParameter) return alertExecute.Begin();
      if (p == TheSMAlertArgumentsParameter) return alertArguments.Begin();
      if (p == TheSMAlertFWHMxParameter) return &alertFWHMx;
      if (p == TheSMAlertRoundnessParameter) return &alertRoundness;
      if (p == TheSMAlertBackgroundParameter) return &alertBackground;
      if (p == TheSMAlertStarQuantityParameter) return &alertStarQuantity;

      return 0;
   }

   bool StarMonitorInstance::AllocateParameter(size_type sizeOrLength, const MetaParameter* p, size_type tableRow)
   {
      if (p == TheSMMonitoredFolderParameter)
      {
         monitoredFolder.Clear();
         if (sizeOrLength > 0)
            monitoredFolder.Reserve(sizeOrLength);
      }
      else if (p == TheSMTargetItemsParameter)
      {
         targets.Clear();
         if (sizeOrLength > 0)
            targets.Add(Item(), sizeOrLength);
      }
      else if (p == TheSMTargetImageParameter)
      {
         targets[tableRow].path.Clear();
         if (sizeOrLength > 0)
            targets[tableRow].path.Reserve(sizeOrLength);
      }
      else if (p == TheSMInputHintsParameter)
      {
         inputHints.Clear();
         if (sizeOrLength > 0)
            inputHints.Reserve(sizeOrLength);
      }
      else if (p == TheSMAlertExecuteParameter)
      {
         alertExecute.Clear();
         if (sizeOrLength > 0)
            alertExecute.Reserve(sizeOrLength);
      }
      else if (p == TheSMAlertArgumentsParameter)
      {
         alertArguments.Clear();
         if (sizeOrLength > 0)
            alertArguments.Reserve(sizeOrLength);
      }
      else
         return false;

      return true;
   }

   size_type StarMonitorInstance::ParameterLength(const MetaParameter* p, size_type tableRow) const
   {
      if (p == TheSMMonitoredFolderParameter) return monitoredFolder.Length();
      if (p == TheSMTargetItemsParameter) return targets.Length();
      if (p == TheSMTargetImageParameter) return targets[tableRow].path.Length();
      if (p == TheSMInputHintsParameter) return inputHints.Length();
      if (p == TheSMAlertExecuteParameter)return alertExecute.Length();
      if (p == TheSMAlertArgumentsParameter) return alertArguments.Length();
      return 0;
   }

   // ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF StarMonitorInstance.cpp - Released 2016/01/14 19:32:59 UTC
