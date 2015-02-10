// ****************************************************************************
// PixInsight Class Library - PCL 02.00.14.0695
// Standard CometAlignment Process Module Version 01.02.03.0001
// ****************************************************************************
// CometAlignmentInstance.cpp - Released 2015/02/10 19:50:08 UTC
// ****************************************************************************
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "CometAlignmentInstance.h"
#include "CometAlignmentModule.h" // for ReadableVersion()

#include <pcl/ErrorHandler.h>
#include <pcl/FileFormat.h>
#include <pcl/LinearFit.h>
#include <pcl/ProcessInterface.h> // for ProcessEvents()
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Translation.h>
#include <pcl/Vector.h>
#include <pcl/Version.h>

namespace pcl
{
static PixelInterpolation* pixelInterpolation = 0;

CometAlignmentInstance::CometAlignmentInstance (const MetaProcess* m) :
ProcessImplementation (m),
p_targetFrames (),
p_inputHints( TheCAInputHintsParameter->DefaultValue() ),
p_outputHints( TheCAOutputHintsParameter->DefaultValue() ),
p_outputDir (TheOutputDir->DefaultValue ()),
p_outputExtension( TheCAOutputExtensionParameter->DefaultValue() ),
p_overwrite (TheOverwrite->DefaultValue ()),
p_prefix (ThePrefix->DefaultValue ()),
p_postfix (ThePostfix->DefaultValue ()),
p_reference (TheReference->DefaultValue ()),
p_subtractFile (TheSubtractFile->DefaultValue ()),
p_subtractMode (TheSubtractMode->DefaultValue ()),
p_normalize (TheNormalize->DefaultValue ()),
p_enableLinearFit (TheEnableLinearFit->DefaultValue ()),
p_rejectLow (TheRejectLow->DefaultValue ()),
p_rejectHigh (TheRejectHigh->DefaultValue ()),
p_pixelInterpolation (ThePixelInterpolationParameter->DefaultValueIndex ()),
p_linearClampingThreshold (TheLinearClampingThresholdParameter->DefaultValue ()) { }

CometAlignmentInstance::CometAlignmentInstance (const CometAlignmentInstance& x) :
ProcessImplementation (x)
{
   Assign (x);
}

void
CometAlignmentInstance::Assign (const ProcessImplementation& p)
{
   const CometAlignmentInstance* x = dynamic_cast<const CometAlignmentInstance*> (&p);
   if (x != 0)
   {
      p_targetFrames = x->p_targetFrames;
      p_inputHints = x->p_inputHints;
      p_outputHints = x->p_outputHints;
      p_outputDir = x->p_outputDir;
      p_outputExtension = x->p_outputExtension;
      p_overwrite = x->p_overwrite;
      p_prefix = x->p_prefix;
      p_postfix = x->p_postfix;
      p_reference = x->p_reference;
      p_subtractFile = x->p_subtractFile;
      p_enableLinearFit = x->p_enableLinearFit;
      p_rejectLow = x->p_rejectLow;
      p_rejectHigh = x->p_rejectHigh;
      p_normalize = x->p_normalize;
      p_subtractMode = x->p_subtractMode;
      p_pixelInterpolation = x->p_pixelInterpolation;
      p_linearClampingThreshold = x->p_linearClampingThreshold;
   }
}

bool
CometAlignmentInstance::CanExecuteOn (const View& view, String& whyNot) const
{
   whyNot = "CometAlignment can only be executed in the global context.";
   return false;
}

bool
CometAlignmentInstance::IsHistoryUpdater (const View& view) const
{
   return false;
}

bool
CometAlignmentInstance::CanExecuteGlobal (String& whyNot) const
{
   if (p_targetFrames.IsEmpty ())
      whyNot = "No target frames have been specified.";

   else if (!p_outputDir.IsEmpty () && !File::DirectoryExists (p_outputDir))
      whyNot = "The specified output directory does not exist: " + p_outputDir;
   else if (!p_subtractFile.IsEmpty () && !File::Exists (p_subtractFile))
      whyNot = "The specified file for LinearFit does not exist: " + p_subtractFile;

   else
   {
      for (image_list::const_iterator i = p_targetFrames.Begin (); i != p_targetFrames.End (); ++i)
         if (i->enabled && !File::Exists (i->path))
         {
            whyNot = "File not found: " + i->path;
            return false;
         }
      whyNot.Clear ();
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------
/*
static void ImageVariant2ImageWindow( ImageVariant* v )

{
    if ( v == 0 )

    {
        Console().WriteLn( "Wrong target pointer" );
        return;
    }
    ImageWindow w( v->AnyImage()->Width(), v->AnyImage()->Height(), v->AnyImage()->NumberOfChannels(), v->BitsPerSample(), v->IsFloatSample(), v->AnyImage()->IsColor(), true, "debug" );
    w.MainView().Image().CopyImage( *v );
    w.Show();
}
 */
/*
static void ImageVariant2ImageWindow( ImageVariant v )

{
    ImageWindow w( v.AnyImage()->Width(), v.AnyImage()->Height(), v.AnyImage()->NumberOfChannels(), v.BitsPerSample(), v.IsFloatSample(), v.AnyImage()->IsColor(), true, "debug" );
    w.MainView().Image().CopyImage( v );
    w.Show();

}
 */
/*
// ----------------------------------------------------------------------------
static void DImage2ImageWindow( const DImage* img )

{
    ImageWindow w( img->Width(), img->Height(), img->NumberOfChannels(), img->BitsPerSample(), img->IsFloatSample(), img->IsColor(), true, "debug" );
    w.MainView().Image().CopyImage( *img );
    w.Show();
}

 */
// ----------------------------------------------------------------------------

class LinearFitEngine
{
public:

   typedef GenericVector<LinearFit> linear_fit_set;

   LinearFitEngine (const float _rejectLow, const float _rejectHigh) : rejectLow (_rejectLow), rejectHigh (_rejectHigh) { }

   linear_fit_set
   Fit (const ImageVariant& image, const ImageVariant& reference)
   {
      if (!image.IsComplexSample ())
         if (image.IsFloatSample ())
            switch (image.BitsPerSample ())
            {
            case 32: return Fit (static_cast<const Image&> (*image), reference);
               break;
            case 64: return Fit (static_cast<const DImage&> (*image), reference);
               break;
            }
         else
            switch (image.BitsPerSample ())
            {
            case 8: return Fit (static_cast<const UInt8Image&> (*image), reference);
               break;
            case 16: return Fit (static_cast<const UInt16Image&> (*image), reference);
               break;
            case 32: return Fit (static_cast<const UInt32Image&> (*image), reference);
               break;
            }
      return linear_fit_set ();
   }

   void
   Apply (ImageVariant& image, const linear_fit_set& L)
   {
      if (!image.IsComplexSample ())
         if (image.IsFloatSample ())
            switch (image.BitsPerSample ())
            {
            case 32: Apply (static_cast<Image&> (*image), L);
               break;
            case 64: Apply (static_cast<DImage&> (*image), L);
               break;
            }
         else
            switch (image.BitsPerSample ())
            {
            case 8:
            {
               UInt8Image& wrk = static_cast<UInt8Image&> (*image);
               Image tmp (wrk);
               Apply (tmp, L);
               wrk.Apply (tmp);
            }
               break;
            case 16:
            {
               UInt16Image& wrk = static_cast<UInt16Image&> (*image);
               Image tmp (wrk);
               Apply (tmp, L);
               wrk.Apply (tmp);
            }
               break;
            case 32:
            {
               UInt32Image& wrk = static_cast<UInt32Image&> (*image);
               DImage tmp (wrk);
               Apply (tmp, L);
               wrk.Apply (tmp);
            }
               break;
            }
   }

private:

   const float rejectLow;
   const float rejectHigh;

   template <class P1, class P2>
   linear_fit_set
   Fit (const GenericImage<P1>& image, const GenericImage<P2>& reference)
   {
      linear_fit_set L (image.NumberOfNominalChannels ());
      GenericVector<size_type> count (image.NumberOfNominalChannels ());
      size_type N = image.NumberOfPixels ();

      for (int c = 0; c < image.NumberOfNominalChannels (); ++c)
      {
         Array<float> F1, F2;
         F1.Reserve (N);
         F2.Reserve (N);
         const typename P1::sample* v1 = image.PixelData (c);
         const typename P1::sample* vN = v1 + N;
         const typename P2::sample* v2 = reference.PixelData (c);
         for (; v1 < vN; ++v1, ++v2/*, ++monitor*/)
         {
            float f1;
            P1::FromSample (f1, *v1);
            if (f1 > rejectLow && f1 < rejectHigh)
            {
               float f2;
               P2::FromSample (f2, *v2);
               if (f2 > rejectLow && f2 < rejectHigh)
               {
                  F1.Add (f1);
                  F2.Add (f2);
               }
            }
         }

         if (F1.Length () < 3)
            throw Error ("Insufficient data (channel " + String (c) + ')');

         count[c] = F1.Length ();

         L[c] = LinearFit (F1, F2/*, &monitor*/);

         if (!L[c].IsValid ())
            throw Error ("Invalid linear fit (channel " + String (c) + ')');
      }
      return L;
   }

   template <class P>
   linear_fit_set
   Fit (const GenericImage<P>& image, const ImageVariant& reference)
   {
      if (!reference.IsComplexSample ())
         if (reference.IsFloatSample ())
            switch (reference.BitsPerSample ())
            {
            case 32: return Fit (image, static_cast<const Image&> (*reference));
               break;
            case 64: return Fit (image, static_cast<const DImage&> (*reference));
               break;
            }
         else
            switch (reference.BitsPerSample ())
            {
            case 8: return Fit (image, static_cast<const UInt8Image&> (*reference));
               break;
            case 16: return Fit (image, static_cast<const UInt16Image&> (*reference));
               break;
            case 32: return Fit (image, static_cast<const UInt32Image&> (*reference));
               break;
            }
      return linear_fit_set ();
   }

   template <class P>
   void
   Apply (GenericImage<P>& image, const linear_fit_set& L)
   {
      for (int c = 0; c < image.NumberOfNominalChannels (); ++c)
      {
         typename P::sample* v = image.PixelData (c);
         typename P::sample* vN = v + image.NumberOfPixels ();
         for (; v < vN; ++v)

         {
            double f;
            if (*v > 0) //ignore black pixels

            {
               P::FromSample (f, *v);
               *v = P::ToSample (L[c](f));
            }
         }
      }

      image.Truncate ();
   }
};

// ----------------------------------------------------------------------------

template <class P>
static void
Normalize (GenericImage<P>& img) //subtract Median from Comet image
{
   for (int c = 0; c < img.NumberOfNominalChannels (); ++c)
   {
      typename P::sample m = img.Median (0, c, c);
      typename P::sample* v = img.PixelData (c);
      typename P::sample* vN = v + img.NumberOfPixels ();
      for (; v < vN; ++v)
         if (*v > 0) //ignore black pixels
            *v -= m;
   }
}

static void
Normalize (ImageVariant& cimg)
{
   if (!cimg.IsComplexSample ())
      if (cimg.IsFloatSample ())
         switch (cimg.BitsPerSample ())
         {
         case 32: Normalize (static_cast<Image&> (*cimg));
            break;
         case 64: Normalize (static_cast<DImage&> (*cimg));
            break;
         }
      else
         switch (cimg.BitsPerSample ())

         {
         case 8: Normalize (static_cast<UInt8Image&> (*cimg));
            break;
         case 16: Normalize (static_cast<UInt16Image&> (*cimg));
            break;
         case 32: Normalize (static_cast<UInt32Image&> (*cimg));
            break;
         }
}

inline static void
Engine (ImageVariant& target, LinearFitEngine::linear_fit_set& LFSet, const DPoint& delta,
        const ImageVariant* operand, const bool mode, const bool enableLinearFit, const float rejectLow, const float rejectHigh, const bool normalize)

{
   Translation T (*pixelInterpolation);
   T.EnableParallelProcessing (); //.DisableParallelProcessing();
   if (operand)

   {
      ImageVariant o;
      o.CopyImage (*operand); //Create local copy of Operand image
      if (mode) //move Operand and subtract
      {
         T.SetDelta (-delta); //Invert direction and set delta
         T >> o; //align Operand to comet position
         if (enableLinearFit)
         {
            LinearFitEngine E (rejectLow, rejectHigh);
            LFSet = E.Fit (o, target);
            E.Apply (o, LFSet); //LinearFit Operand to Target
         }

         if (normalize == true)
            Normalize (o);

         target -= o; //Subtract Operand from Target Image
      }
      else //subtract Operand and move
      {
         if (enableLinearFit)
         {
            LinearFitEngine E (rejectLow, rejectHigh);
            LFSet = E.Fit (o, target);
            E.Apply (o, LFSet); //LinearFit Operand to Target
         }

         if (normalize == true)
            Normalize (o);

         target -= o; //Subtract Operand from Target Image

         T.SetDelta (delta);
         T >> target; //Target to Comet position
      }
      target.Truncate (); // Truncate to [0,1]
   }
   else
   {
      T.SetDelta (delta);
      T >> target;
   }
}

// ----------------------------------------------------------------------------

struct FileData
{
   FileFormat* format; // the file format of retrieved data
   const void* fsData; // format-specific data
   ImageOptions options; // currently used for resolution only
   FITSKeywordArray keywords; // FITS keywords
   ICCProfile profile; // ICC profile
   ByteArray metadata; // XML metadata

   FileData () :
   format (0), fsData (0), options (), keywords (), profile (), metadata () { }

   FileData (FileFormatInstance& file, const ImageOptions & o) :
   format (0), fsData (0), options (o), keywords (), profile (), metadata ()
   {
      format = new FileFormat (file.Format ());

      if (format->UsesFormatSpecificData ())
         fsData = file.FormatSpecificData ();

      if (format->CanStoreKeywords ())
         file.Extract (keywords);

      if (format->CanStoreICCProfiles ())
         file.Extract (profile);

      if (format->CanStoreMetadata ())
      {

         void* p = 0;
         size_t n = 0;
         file.Extract (p, n);
         if (p != 0)
         {
            metadata = ByteArray (ByteArray::iterator (p), ByteArray::iterator (p) + n);
            delete (uint8*) p;
         }
      }
   }

   ~FileData ()
   {
      if (format != 0)
      {
         if (fsData != 0)
            format->DisposeFormatSpecificData (const_cast<void*> (fsData)), fsData = 0;
         delete format, format = 0;
      }
   }
};

// ----------------------------------------------------------------------------

class CAThread : public Thread
{
public:

   CAThread (ImageVariant* t, FileData* fd, const String& tp, int i, const DPoint d,
             const ImageVariant* o, const bool m, const bool enableFit, const float rLow, const float rHigh, const bool norm) :
   target (t), fileData (fd), targetPath (tp), subimageIndex (i), delta (d),
   operand (o), mode (m), normalize (norm), enableLinearFit (enableFit), rejectLow (rLow), rejectHigh (rHigh) { }

   virtual
   ~CAThread ()
   {
      if (target != 0)
         delete target, target = 0;

      if (fileData != 0)
         delete fileData, fileData = 0;
   }

   virtual void
   Run ()
   {
      try
      {
         Engine (*target, LFSet, delta, operand, mode, enableLinearFit, rejectLow, rejectHigh, normalize);
      }
      catch (...)
      {
         /*
          * NB: No exceptions can propagate out of a thread!
          * We *only* throw exceptions to stop this thread when data.abort has
          * been set to true by the root thread
          */
      }
   }

   const ImageVariant*
   TargetImage () const
   {
      return target;
   }

   String
   TargetPath () const
   {
      return targetPath;
   }

   const FileData&
   GetFileData () const
   {
      return *fileData;
   }

   int
   SubimageIndex () const
   {
      return subimageIndex;
   }
   
   DPoint 
   Delta () const
   {
      return delta;
   }
   
   LinearFitEngine::linear_fit_set 
   GetLinearFitSet() const
   {
      return LFSet;
   }

private:

   ImageVariant* target;
   FileData* fileData; // Target image parameters and embedded data. It belongs to this thread.
   String targetPath; // File path of this target image
   int subimageIndex; // > 0 in case of a multiple image; = 0 otherwise
   DPoint delta; // Delta x, y
   const ImageVariant* operand; //Image for subtraction from target
   const bool mode; // true = move operand and subtract from target, false = subtract operand from target and move
   const bool normalize; // restore median in subtracted area
   const bool enableLinearFit; // fit operand before subtracting
   float rejectLow; //LinearFit parameter
   float rejectHigh; //LinearFit parameter
   LinearFitEngine::linear_fit_set LFSet;

};

// ----------------------------------------------------------------------------

template <class P>
static void
LoadImageFile (GenericImage<P>& image, FileFormatInstance& file)
{
   if (!file.ReadImage (image))
      throw CatchedException ();
}

static void
LoadImageFile (ImageVariant& v, FileFormatInstance& file, const ImageDescriptionArray& images, int index)
{
   if (!file.SelectImage (index))
      throw CatchedException ();

   v.CreateSharedImage (images[index].options.ieeefpSampleFormat, false, images[index].options.bitsPerSample);

   if (v.IsFloatSample ()) switch (v.BitsPerSample ())
      {
      case 32: LoadImageFile (*static_cast<Image*> (v.AnyImage ()), file);
         break;
      case 64: LoadImageFile (*static_cast<DImage*> (v.AnyImage ()), file);
         break;
      }
   else switch (v.BitsPerSample ())
      {
      case 8: LoadImageFile (*static_cast<UInt8Image*> (v.AnyImage ()), file);
         break;
      case 16: LoadImageFile (*static_cast<UInt16Image*> (v.AnyImage ()), file);
         break;
      case 32: LoadImageFile (*static_cast<UInt32Image*> (v.AnyImage ()), file);
         break;
      }
}

inline thread_list
CometAlignmentInstance::LoadTargetFrame (const size_t fileIndex)
{
   Console console;
   const ImageItem& item = p_targetFrames[fileIndex];
   const ImageItem& r = p_targetFrames[p_reference];
   const DPoint delta (item.x - r.x, item.y - r.y);

   const String filePath = item.path;
   console.WriteLn ("Open " + filePath);

   FileFormat format (File::ExtractExtension (filePath), true, false);
   FileFormatInstance file (format);

   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, p_inputHints ) ) throw CatchedException ();
   if (images.IsEmpty ()) throw Error (filePath + ": Empty image file.");

   thread_list threads;
   ImageVariant* target = new ImageVariant ();
   try
   {
      for (unsigned int index = 0; index < images.Length (); ++index)
      {
         if (images.Length () > 1) console.WriteLn (String ().Format ("* Subimage %u of %u", index + 1, images.Length ()));
         if (!file.SelectImage (index)) throw CatchedException ();
         LoadImageFile (*target, file, images, index);
         ProcessInterface::ProcessEvents ();
         if (m_geometry.IsRect ())
         {
            if (target->Bounds () != m_geometry)
               throw Error ("The Image geometry is not equal.");
         }
         else
            m_geometry = target->Bounds ();

         FileData* inputData = new FileData (file, images[index].options);
         threads.Add (new CAThread (target, inputData, filePath, index, delta, m_CometImage, p_subtractMode,
                                    p_enableLinearFit, p_rejectLow, p_rejectHigh, p_normalize));
      }

      console.WriteLn ("Close " + filePath);
      file.Close ();
      console.WriteLn (String ().Format ("Delta x:%f, y:%f", delta.x, delta.y));

      return threads;
   }
   catch (...)
   {
      if (target != 0) delete target;
      throw;
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void
SaveImageFile (const GenericImage<P>& image, FileFormatInstance& file)
{
   if (!file.WriteImage (image))
      throw CatchedException ();
}

static void
SaveImageFile (const ImageVariant& image, FileFormatInstance& file)
{
   if (image.IsFloatSample ()) switch (image.BitsPerSample ())
      {
      case 32: SaveImageFile (*static_cast<const Image*> (image.AnyImage ()), file);
         break;
      case 64: SaveImageFile (*static_cast<const DImage*> (image.AnyImage ()), file);
         break;
      }
   else switch (image.BitsPerSample ())
      {
      case 8: SaveImageFile (*static_cast<const UInt8Image*> (image.AnyImage ()), file);
         break;
      case 16: SaveImageFile (*static_cast<const UInt16Image*> (image.AnyImage ()), file);
         break;
      case 32: SaveImageFile (*static_cast<const UInt32Image*> (image.AnyImage ()), file);
         break;
      }
}

inline String
UniqueFilePath (const String& filePath)
{
   for (unsigned u = 1;; ++u)
   {
      String tryFilePath = File::AppendToName (filePath, '_' + String (u));
      if (!File::Exists (tryFilePath))
         return tryFilePath;
   }
}

inline String
CometAlignmentInstance::OutputFilePath (const String& filePath, const size_t index)
{
   String dir = p_outputDir;
   dir.Trim ();
   if (dir.IsEmpty ())
      dir = File::ExtractDrive (filePath) + File::ExtractDirectory (filePath);
   if (dir.IsEmpty ())
      throw Error (dir + ": Unable to determine an output p_outputDir.");
   if (!dir.EndsWith ('/'))
      dir.Append ('/');

   String fileName = File::ExtractName (filePath);
   fileName.Trim ();
   if (!p_prefix.IsEmpty ()) fileName.Prepend (p_prefix);
   if (index > 0) fileName.Append (String ().Format ("_%02d", index + 1));
   if (!p_postfix.IsEmpty ()) fileName.Append (p_postfix);
   if (fileName.IsEmpty ()) throw Error (filePath + ": Unable to determine an output file name.");

   String outputFilePath = dir + fileName + p_outputExtension;
   Console ().WriteLn ("<end><cbr><br>Writing output file: " + outputFilePath);

   if (File::Exists (outputFilePath))
      if (p_overwrite)
         Console ().WarningLn ("** Warning: Overwriting already existing file.");
      else
      {
         outputFilePath = UniqueFilePath (outputFilePath);
         Console ().NoteLn ("* File already exists, writing to: " + outputFilePath);
      }

   return outputFilePath;
}

void
CometAlignmentInstance::SaveImage (const CAThread* t)
{
   Console console;
   LinearFitEngine::linear_fit_set L;
   if (p_enableLinearFit)
   {
      //const LinearFitEngine::linear_fit_set L(t->GetLinearFitSet());
      L = t->GetLinearFitSet();
      console.WriteLn( "<end><cbr>Linear fit functions:" );
      for ( int c = 0; c <  t->TargetImage()->NumberOfNominalChannels(); ++c )
      {
         console.WriteLn( String().Format( "y<sub>%d</sub> = %+.6f %c %.6f&middot;x<sub>%d</sub>", c, L[c].a, (L[c].b < 0) ? '-' : '+', Abs( L[c].b ), c ) );
         console.WriteLn( String().Format( "&sigma;<sub>%d</sub> = %+.6f", c, L[c].adev ) );
      }
   }
   
   String outputFilePath = OutputFilePath (t->TargetPath (), t->SubimageIndex ());
   console.WriteLn ("Create " + outputFilePath);

   FileFormat outputFormat (p_outputExtension, false, true);
   FileFormatInstance outputFile (outputFormat);
   if ( !outputFile.Create( outputFilePath, p_outputHints ) ) throw CatchedException ();

   const FileData& data = t->GetFileData ();   

   ImageOptions options = data.options; // get resolution, etc.
   outputFile.SetOptions (options);

   if (data.fsData != 0)
      if (outputFormat.ValidateFormatSpecificData (data.fsData)) outputFile.SetFormatSpecificData (data.fsData);

   /*
    * Add FITS header keywords and preserve existing ones, if possible.
    * NB: A COMMENT or HISTORY keyword cannot have a value; these keywords have
    * only the name and comment components.
    */
   if (outputFormat.CanStoreKeywords ())
   {
      FITSKeywordArray keywords = data.keywords;
      keywords.Add (FITSHeaderKeyword ("COMMENT", IsoString (), "CometAlignment with " + PixInsightVersion::AsString ()));
      keywords.Add (FITSHeaderKeyword ("COMMENT", IsoString (), "CometAlignment with " + CometAlignmentModule::ReadableVersion ()));
      keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.X: " + IsoString(t->Delta().x)));
      keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.Y:" +IsoString(t->Delta().y)));
      if (!p_subtractFile.IsEmpty ())
      {
         keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.Subtract: " + IsoString( p_subtractFile ) ) );
         keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.Mode: " + IsoString( p_subtractMode ? "true" : "false" ) ) );
         keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.LinearFit: " + IsoString( p_enableLinearFit ? "true" : "false" ) ) );
         keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.RejectLow: " + IsoString( p_rejectLow ) ) );
         keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.RejectHigh: " + IsoString( p_rejectHigh ) ) );
         if (p_enableLinearFit)
         {
            keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.Linear fit functions:" ) );
            for ( int c = 0; c <  t->TargetImage()->NumberOfNominalChannels(); ++c )
            {               
               keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), IsoString().Format( "y%d = %+.6f %c %.6f * x%d", c, L[c].a, (L[c].b < 0) ? '-' : '+', Abs( L[c].b ), c ) ) );
               keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), IsoString().Format( "sigma%d = %+.6f", c, L[c].adev ) ) );
            }
         }
         keywords.Add (FITSHeaderKeyword ("HISTORY", IsoString (), "CometAlignment.Normalize: " + IsoString( p_normalize ? "true" : "false") ) );
      }
      outputFile.Embed (keywords);
   }
   else if (!data.keywords.IsEmpty ())
      console.WarningLn ("** Warning: The output format cannot store FITS keywords - original keywords not embedded.");

   if (data.profile.IsProfile ())
      if (outputFormat.CanStoreICCProfiles ()) outputFile.Embed (data.profile);
      else console.WarningLn ("** Warning: The output format cannot store ICC profiles - original profile not embedded.");

   if (!data.metadata.IsEmpty ())
      if (outputFormat.CanStoreMetadata ()) outputFile.Embed (data.metadata.Begin (), data.metadata.Length ());
      else console.WarningLn ("** Warning: The output format cannot store metadata - original metadata not embedded.");

   SaveImageFile (*t->TargetImage (), outputFile);

   console.WriteLn ("Close file.");
   outputFile.Close ();
}

inline void
CometAlignmentInstance::InitPixelInterpolation ()
{
   Console ().WriteLn ("PixelInterpolation: " + ThePixelInterpolationParameter->ElementId (p_pixelInterpolation));
   if (p_pixelInterpolation == CAPixelInterpolation::BicubicSpline) Console ().WriteLn ("ClampingThreshold : " + String (p_linearClampingThreshold));

   switch (p_pixelInterpolation)
   {
   case CAPixelInterpolation::NearestNeighbor:
      pixelInterpolation = new NearestNeighborPixelInterpolation;
      break;
   case CAPixelInterpolation::Bilinear:
      pixelInterpolation = new BilinearPixelInterpolation;
      break;
   default:
   case CAPixelInterpolation::BicubicSpline:
      pixelInterpolation = new BicubicSplinePixelInterpolation (p_linearClampingThreshold);
      break;
   case CAPixelInterpolation::BicubicBSpline:
      pixelInterpolation = new BicubicBSplinePixelInterpolation;
      break;
   case CAPixelInterpolation::Lanczos3:
   case CAPixelInterpolation::Lanczos4:
   case CAPixelInterpolation::Lanczos5:

   {
      // ### FIXME: Use LUT Lanczos interpolations for 8-bit and 16-bit images

      // Disable clamping when clampingThreshold == 1
      float clamp = (p_linearClampingThreshold < 1) ? p_linearClampingThreshold : -1;
      switch (p_pixelInterpolation)
      {
      default:
      case CAPixelInterpolation::Lanczos3:
         pixelInterpolation = new LanczosPixelInterpolation (3, clamp);
         break;
      case CAPixelInterpolation::Lanczos4:
         pixelInterpolation = new LanczosPixelInterpolation (4, clamp);
         break;
      case CAPixelInterpolation::Lanczos5:
         pixelInterpolation = new LanczosPixelInterpolation (5, clamp);
         break;
      }
   }
      break;
   case CAPixelInterpolation::MitchellNetravaliFilter:
      pixelInterpolation = new BicubicFilterPixelInterpolation (2, 2, MitchellNetravaliCubicFilter ());
      break;
   case CAPixelInterpolation::CatmullRomSplineFilter:
      pixelInterpolation = new BicubicFilterPixelInterpolation (2, 2, CatmullRomSplineFilter ());
      break;
   case CAPixelInterpolation::CubicBSplineFilter:
      pixelInterpolation = new BicubicFilterPixelInterpolation (2, 2, CubicBSplineFilter ());
      break;
   }
}

inline DImage
CometAlignmentInstance::GetCometImage (const String& filePath)
{
   Console ().WriteLn ("<end><cbr>Loading MathImage:<flash>");
   Console ().WriteLn (filePath);

   FileFormat format (File::ExtractExtension (filePath), true, false);
   FileFormatInstance file (format);
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, p_inputHints ) ) throw CatchedException ();
   if (images.IsEmpty ()) throw Error (filePath + ": Empty MathImage.");
   if (!file.SelectImage (0))
      throw CatchedException ();
   DImage img;
   if (!file.ReadImage (img)) throw CatchedException ();
   m_geometry = img.Bounds (); //set geometry from
   file.Close ();
   return img;
}

inline ImageVariant*
CometAlignmentInstance::LoadOperandImage (const String& filePath)

{
   Console console;
   if (filePath.IsEmpty ())
      return 0;
   console.WriteLn ("Open " + filePath);
   FileFormat format (File::ExtractExtension (filePath), true, false);
   FileFormatInstance file (format);
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath, p_inputHints ) ) throw CatchedException ();
   if (images.IsEmpty ()) throw Error (filePath + ": Empty image file.");
   ImageVariant* img = new ImageVariant ();
   if (!file.SelectImage (0)) throw CatchedException ();
   LoadImageFile (*img, file, images, 0);
   ProcessInterface::ProcessEvents ();
   console.WriteLn ("Close " + filePath);
   file.Close ();
   m_geometry = img->Bounds ();
   return img;
}

// ----------------------------------------------------------------------------

bool
CometAlignmentInstance::ExecuteGlobal ()
{
   Console console;
   console.Show ();

   String why;
   if (!CanExecuteGlobal (why)) throw Error (why);

   m_geometry = 0;
   m_CometImage = 0;

   try //try 1
   {
      Exception::EnableConsoleOutput ();
      Exception::DisableGUIOutput ();
      console.EnableAbort ();

      m_CometImage = LoadOperandImage (p_subtractFile);
      if (m_CometImage)
      {
         if (p_subtractMode)
            console.WriteLn ("Mode: Align Operand image and subtract from Targets.");
         else
            console.WriteLn ("Mode: Subtract operand from Targets and align.");

         console.WriteLn ("LinearFit " + String (p_enableLinearFit ? "Enabled" : "Disabled")
                          + String ().Format (", rejection Low:%f, High:%f", p_rejectLow, p_rejectHigh));
         console.WriteLn ("Normalization " + String (p_normalize ? "Enabled" : "Disabled"));
      }
      else
         console.WriteLn ("Mode: Only align Target images.");

      InitPixelInterpolation ();

      size_t succeeded = 0;
      size_t skipped = 0;
      const size_t total = p_targetFrames.Length ();
      console.WriteLn (String ().Format ("<br>Processing %u target frames:", total));

      Array<size_t> t;
      for (size_t i = 0; i < total; t.Add (i++)); // Array with file indexes

      const int totalCPU = Thread::NumberOfThreads (1024, 1);
      Console ().Write (String ().Format ("Detected %u CPU. ", totalCPU));

      const size_t n = Min (size_t (totalCPU), total);
      thread_list runningThreads (n); // n = how many threads will run simultaneously
      console.WriteLn (String ().Format ("Using %u worker threads", runningThreads.Length ()));

      thread_list waitingThreads; //container for hold images from next image. One or more if file is multi image

      try //try 2
      {
         int runing = 0; // runing = Qty sub-images processing now = Qty CPU isActiv now.
         do
         {
            ProcessInterface::ProcessEvents (); // Keep the GUI responsive
            if (console.AbortRequested ()) throw ProcessAborted ();

            // ------------------------------------------------------------
            // Open File
            if (!t.IsEmpty () && waitingThreads.IsEmpty ())
            {
               size_t fileIndex = *t; // take first index from begining of the list
               t.Remove (t.Begin ()); // remove the index from the list

               console.WriteLn (String ().Format ("<br>File %u of %u", total - t.Length (), total));
               if (p_targetFrames[fileIndex].enabled)
                  waitingThreads = LoadTargetFrame (fileIndex); // put all sub-images from file to waitingThreads
               else
               {
                  ++skipped;
                  console.NoteLn ("* Skipping disabled target");
               }
            }

            // ------------------------------------------------------------
            // Find idle or free CPU
            thread_list::iterator i = 0;
            for (thread_list::iterator j = runningThreads.Begin (); j != runningThreads.End (); ++j) //Cycle in CPU units
            {
               if (*j == 0) // the CPU is free and empty.
               {
                  if (!waitingThreads.IsEmpty ()) // there are not processed images
                  {
                     i = j;
                     break; // i pointed to CPU which is free now.
                  }
               }// *j != 0 the CPU is non free and maybe idle or active
               else if (!(*j)->IsActive ()) // the CPU is idle = the CPU has finished processing
               {
                  i = j;
                  break; // i pointed to CPU thread which ready to save.
               }
            }

            if (i == 0) // all CPU IsActive or no new images
            {
               pcl::Sleep (0.1);
               continue;
            }

            // ------------------------------------------------------------
            // Write File
            if (*i != 0) //the CPU is idle
            {
               runing--;
               try
               {
                  console.WriteLn (String ().Format ("<br>CPU#%u has finished processing.", i - runningThreads.Begin ()));
                  SaveImage (*i);
                  runningThreads.Delete (i); //prepare thread for next image. now (*i == 0) the CPU is free
               }
               catch (...)
               {
                  runningThreads.Delete (i);
                  throw;
               }
               ++succeeded;
            } //now (*i == 0) the CPU is free

            // ------------------------------------------------------------
            // Put image to empty runningThreads slot and Run()
            if (!waitingThreads.IsEmpty ())
            {
               *i = *waitingThreads; //put one sub-image to runningThreads. so, now (*i != 0)
               waitingThreads.Remove (waitingThreads.Begin ()); //remove one sub-image from waitingThreads
               console.WriteLn (String ().Format ("<br>CPU#%u processing file ", i - runningThreads.Begin ()) + (*i)->TargetPath ());
               (*i)->Start (ThreadPriority::DefaultMax, i - runningThreads.Begin ());
               runing++;
            }
         }
         while (runing > 0 || !t.IsEmpty ());
      }// try 2
      catch (...)
      {
         try
         {
            throw;
         }
         ERROR_HANDLER;

         bool printed = false;
         for (thread_list::iterator i = runningThreads.Begin (); i != runningThreads.End (); ++i)
            if (*i != 0)
            {
               if (!printed)
               {
                  console.Note ("<br>* Waiting for running tasks at CPU# ");
                  printed = true;
               }
               console.Write (String ().Format (":%u ", i - runningThreads.Begin ()));
               console.Flush ();
               (*i)->Wait ();
            }
         console.NoteLn ("<br>* All running tasks are terminated.");
         runningThreads.Destroy ();
         waitingThreads.Destroy ();
      }
      //Translation::EnableParallelProcessing();

      console.NoteLn (String ().Format ("<br>===== CometAlignment: %u succeeded, %u skipped, %u canceled =====",
                                        succeeded, skipped, total - succeeded));

      Exception::DisableConsoleOutput ();
      Exception::EnableGUIOutput ();

      if (m_CometImage != 0)
         delete m_CometImage, m_CometImage = 0;

      return true;
   }
   catch (...)
   {
      //AbstractImage::EnableParallelProcessing();
      Exception::DisableConsoleOutput ();
      Exception::EnableGUIOutput ();
      if (m_CometImage != 0) delete m_CometImage, m_CometImage = 0;

      console.NoteLn ("<end><cbr><br>* CometAlignment terminated.");
      throw;
   }
}

// ----------------------------------------------------------------------------

void*
CometAlignmentInstance::LockParameter (const MetaParameter* p, size_type tableRow)
{
   if (p == TheTargetFrameEnabled) return &p_targetFrames[tableRow].enabled;
   if (p == TheTargetFramePath) return p_targetFrames[tableRow].path.c_str ();
   if (p == TheTargetFrameDate) return p_targetFrames[tableRow].date.c_str ();
   if (p == TheTargetFrameJDate)return &p_targetFrames[tableRow].Jdate;
   if (p == TheTargetFrameX) return &p_targetFrames[tableRow].x;
   if (p == TheTargetFrameY) return &p_targetFrames[tableRow].y;

   if ( p == TheCAInputHintsParameter ) return p_inputHints.c_str();
   if ( p == TheCAOutputHintsParameter ) return p_outputHints.c_str();
   if (p == TheOutputDir) return p_outputDir.c_str ();
   if ( p == TheCAOutputExtensionParameter ) return p_outputExtension.c_str();
   if (p == ThePrefix) return p_prefix.c_str ();
   if (p == ThePostfix) return p_postfix.c_str ();
   if (p == TheOverwrite) return &p_overwrite;

   if (p == TheReference) return &p_reference;

   if (p == TheSubtractFile) return p_subtractFile.c_str ();
   if (p == TheSubtractMode) return &p_subtractMode;
   if (p == TheNormalize) return &p_normalize;
   if (p == TheEnableLinearFit) return &p_enableLinearFit;
   if (p == TheRejectLow) return &p_rejectLow;
   if (p == TheRejectHigh) return &p_rejectHigh;


   if (p == TheLinearClampingThresholdParameter) return &p_linearClampingThreshold;
   if (p == ThePixelInterpolationParameter) return &p_pixelInterpolation;
   return 0;
}

bool
CometAlignmentInstance::AllocateParameter (size_type sizeOrLength, const MetaParameter* p, size_type tableRow)

{
   if (p == TheTargetFrames)
   {
      p_targetFrames.Clear ();
      if (sizeOrLength > 0) p_targetFrames.Add (ImageItem (), sizeOrLength);
   }
   else if (p == TheTargetFramePath)
   {
      p_targetFrames[tableRow].path.Clear ();
      if (sizeOrLength > 0) p_targetFrames[tableRow].path.Reserve (sizeOrLength);
   }
   else if (p == TheTargetFrameDate)
   {
      p_targetFrames[tableRow].date.Clear ();
      if (sizeOrLength > 0) p_targetFrames[tableRow].date.Reserve (sizeOrLength);
   }
   else if ( p == TheCAInputHintsParameter )
   {
      p_inputHints.Clear();
      if ( sizeOrLength > 0 )
         p_inputHints.Reserve( sizeOrLength );
   }
   else if ( p == TheCAOutputHintsParameter )
   {
      p_outputHints.Clear();
      if ( sizeOrLength > 0 )
         p_outputHints.Reserve( sizeOrLength );
   }
   else if (p == TheOutputDir)
   {
      p_outputDir.Clear ();
      if (sizeOrLength > 0) p_outputDir.Reserve (sizeOrLength);
   }
   else if ( p == TheCAOutputExtensionParameter )
   {
      p_outputExtension.Clear();
      if ( sizeOrLength > 0 )
         p_outputExtension.Reserve( sizeOrLength );
   }
   else if (p == ThePrefix)
   {
      p_prefix.Clear ();
      if (sizeOrLength > 0) p_prefix.Reserve (sizeOrLength);
   }
   else if (p == ThePostfix)
   {
      p_postfix.Clear ();
      if (sizeOrLength > 0) p_postfix.Reserve (sizeOrLength);
   }
   else if (p == TheSubtractFile)
   {
      p_subtractFile.Clear ();
      if (sizeOrLength > 0) p_subtractFile.Reserve (sizeOrLength);
   }
   else
      return false;

   return true;
}

size_type
CometAlignmentInstance::ParameterLength (const MetaParameter* p, size_type tableRow) const
{
   if (p == TheTargetFrames) return p_targetFrames.Length ();
   if (p == TheTargetFramePath) return p_targetFrames[tableRow].path.Length ();
   if (p == TheTargetFrameDate) return p_targetFrames[tableRow].date.Length ();
   if ( p == TheCAInputHintsParameter ) return p_inputHints.Length();
   if ( p == TheCAOutputHintsParameter ) return p_outputHints.Length();
   if (p == TheOutputDir) return p_outputDir.Length ();
   if ( p == TheCAOutputExtensionParameter ) return p_outputExtension.Length();
   if (p == ThePrefix) return p_prefix.Length ();
   if (p == ThePostfix) return p_postfix.Length ();
   if (p == TheSubtractFile) return p_subtractFile.Length ();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CometAlignmentInstance.cpp - Released 2015/02/10 19:50:08 UTC
