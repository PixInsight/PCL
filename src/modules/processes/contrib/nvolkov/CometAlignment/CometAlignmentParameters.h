//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 01.02.06.0127
// ----------------------------------------------------------------------------
// CometAlignmentParameters.h - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------

#ifndef __CometAlignmentParameters_h
#define __CometAlignmentParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

  PCL_BEGIN_LOCAL
  // ----------------------------------------------------------------------------

  class CATargetFrames : public MetaTable
  {
  public:
    CATargetFrames (MetaProcess*);
    virtual IsoString Id () const;
  };

  class CATargetFrameEnabled : public MetaBoolean
  {
  public:
    CATargetFrameEnabled (MetaTable*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  class CATargetFramePath : public MetaString
  {
  public:
    CATargetFramePath (MetaTable*);
    virtual IsoString Id () const;
  };

  class CATargetFrameDate : public MetaString
  {
  public:
    CATargetFrameDate (MetaTable*);
    virtual IsoString Id () const;
  };

  class CATargetFrameJDate : public MetaDouble
  {
  public:
    CATargetFrameJDate (MetaTable*);
    virtual IsoString Id () const;
    virtual int Precision () const; //bag possible? wrong data in instance icon if Precision not set.
  };

  class CATargetFrameX : public MetaDouble
  {
  public:
    CATargetFrameX (MetaTable*);
    virtual IsoString Id () const;
    virtual int Precision () const;
    virtual double MinimumValue () const;
    virtual double MaximumValue () const;
  };

  class CATargetFrameY : public MetaDouble
  {
  public:
    CATargetFrameY (MetaTable*);
    virtual IsoString Id () const;
    virtual int Precision () const;
    virtual double MinimumValue () const;
    virtual double MaximumValue () const;
  };
  //--------------------------------------------------------------------------
  class CADrizzlePath : public MetaString
  {
  public:
    CADrizzlePath( MetaTable* );
    virtual IsoString Id() const;
  };

  // ----------------------------------------------------------------------------
   class CAInputHints : public MetaString
   {
   public:
      CAInputHints( MetaProcess* );
      virtual IsoString Id() const;
   };

   class CAOutputHints : public MetaString
   {
   public:
      CAOutputHints( MetaProcess* );
      virtual IsoString Id() const;
   };

  class CAOutputDir : public MetaString
  {
  public:
    CAOutputDir (MetaProcess*);
    virtual IsoString Id () const;
    virtual String DefaultValue () const;
  };

  class CAOutputExtension : public MetaString
  {
  public:
    CAOutputExtension (MetaProcess*);
    virtual IsoString Id () const;
    virtual String DefaultValue () const;
  };

  class CAPrefix : public MetaString
  {
  public:
    CAPrefix (MetaProcess*);
    virtual IsoString Id () const;
    virtual String DefaultValue () const;
  };

  class CAPostfix : public MetaString
  {
  public:
    CAPostfix (MetaProcess*);
    virtual IsoString Id () const;
    virtual String DefaultValue () const;
  };

  class CAOverwrite : public MetaBoolean
  {
  public:
    CAOverwrite (MetaProcess*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CAReference : public MetaUInt16
  {
  public:
    CAReference (MetaProcess*);
    virtual IsoString Id () const;
  };

  // ----------------------------------------------------------------------------

  class CASubtractFile : public MetaString
  {
  public:
    CASubtractFile (MetaProcess*);
    virtual IsoString Id () const;
    virtual String DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CASubtractMode : public MetaBoolean
  {
  public:
    CASubtractMode (MetaProcess*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CAEnableLinearFit : public MetaBoolean
  {
  public:
    CAEnableLinearFit (MetaProcess*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CARejectLow : public MetaFloat
  {
  public:
    CARejectLow (MetaProcess*);
    virtual IsoString Id () const;
    virtual int Precision () const;
    virtual double MinimumValue () const;
    virtual double MaximumValue () const;
    virtual double DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CARejectHigh : public MetaFloat
  {
  public:
    CARejectHigh (MetaProcess*);
    virtual IsoString Id () const;
    virtual int Precision () const;
    virtual double MinimumValue () const;
    virtual double MaximumValue () const;
    virtual double DefaultValue () const;
  };
  // ----------------------------------------------------------------------------

  class CANormalize : public MetaBoolean
  {
  public:
    CANormalize (MetaProcess*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CADrzSaveSA : public MetaBoolean
  {
  public:
    CADrzSaveSA (MetaProcess*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CADrzSaveCA : public MetaBoolean
  {
  public:
    CADrzSaveCA (MetaProcess*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CAOperandIsDI : public MetaBoolean
  {
  public:
    CAOperandIsDI (MetaProcess*);
    virtual IsoString Id () const;
    virtual bool DefaultValue () const;
  };

  // ----------------------------------------------------------------------------

  class CAPixelInterpolation : public MetaEnumeration
  {
  public:

    enum
    {
      NearestNeighbor,
      Bilinear,
      BicubicSpline,
      BicubicBSpline,
      Lanczos3,
      Lanczos4,
      Lanczos5,
      MitchellNetravaliFilter,
      CatmullRomSplineFilter,
      CubicBSplineFilter,
      Auto,
      NumberOfInterpolationAlgorithms,
      Default = BicubicSpline
    };

    CAPixelInterpolation (MetaProcess*);

    virtual IsoString Id () const;
    virtual size_type NumberOfElements () const;
    virtual IsoString ElementId (size_type) const;
    virtual int ElementValue (size_type) const;
    virtual size_type DefaultValueIndex () const;
  };

  class CALinearClampingThreshold : public MetaFloat
  {
  public:

    CALinearClampingThreshold (MetaProcess*);

    virtual IsoString Id () const;
    virtual int Precision () const;
    virtual double DefaultValue () const;
    virtual double MinimumValue () const;
    virtual double MaximumValue () const;
  };


  // ----------------------------------------------------------------------------

   extern CATargetFrames* TheTargetFrames;
   extern CATargetFrameEnabled* TheTargetFrameEnabled;
   extern CATargetFramePath* TheTargetFramePath;

   extern CATargetFrameDate* TheTargetFrameDate;
   extern CATargetFrameJDate* TheTargetFrameJDate;
   extern CATargetFrameX* TheTargetFrameX;
   extern CATargetFrameY* TheTargetFrameY;

   extern CADrizzlePath* TheDrizzlePath;

   extern CAInputHints* TheCAInputHintsParameter;
   extern CAOutputHints* TheCAOutputHintsParameter;
   extern CAOutputDir* TheOutputDir;
   extern CAOutputExtension* TheCAOutputExtensionParameter;
   extern CAPrefix* ThePrefix;
   extern CAPostfix* ThePostfix;
   extern CAOverwrite* TheOverwrite;

   extern CAReference* TheReference;

   extern CASubtractFile* TheSubtractFile;
   extern CASubtractMode* TheSubtractMode;
   extern CAOperandIsDI* TheOperandIsDI;
   extern CAEnableLinearFit* TheEnableLinearFit;
   extern CARejectLow* TheRejectLow;
   extern CARejectHigh* TheRejectHigh;
   extern CANormalize* TheNormalize;
   extern CADrzSaveSA* TheDrzSaveSA;
   extern CADrzSaveCA* TheDrzSaveCA;

   extern CAPixelInterpolation* ThePixelInterpolationParameter;
   extern CALinearClampingThreshold* TheLinearClampingThresholdParameter;

  // ----------------------------------------------------------------------------
  PCL_END_LOCAL

} // pcl

#endif   // __CometAlignmentParameters_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentParameters.h - Released 2015/11/26 16:00:13 UTC
