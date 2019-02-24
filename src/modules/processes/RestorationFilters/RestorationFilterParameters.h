//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 01.00.05.0346
// ----------------------------------------------------------------------------
// RestorationFilterParameters.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __RestorationFilterParameters_h
#define __RestorationFilterParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class RFWienerK : public MetaDouble
{
public:

   RFWienerK( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFWienerK* TheRFWienerKParameter;

// ----------------------------------------------------------------------------

class RFLeastSquaresGamma : public MetaDouble
{
public:

   RFLeastSquaresGamma( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFLeastSquaresGamma* TheRFLeastSquaresGammaParameter;

// ----------------------------------------------------------------------------

class RFAlgorithm : public MetaEnumeration
{
public:

   enum { Wiener,
          ConstrainedLeastSquares,
          NumberOfAlgorithms,
          Default = ConstrainedLeastSquares };

   RFAlgorithm( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern RFAlgorithm* TheRFAlgorithmParameter;

// ----------------------------------------------------------------------------

class RFPSFMode : public MetaEnumeration
{
public:

   enum { Parametric,
          MotionBlur,
          External,
          NumberOfModes,
          Default = Parametric };

   RFPSFMode( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
   virtual IsoString ElementAliases() const;
};

extern RFPSFMode* TheRFPSFModeParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricSigma : public MetaFloat
{
public:

   RFPSFParametricSigma( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFPSFParametricSigma* TheRFPSFParametricSigmaParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricShape : public MetaFloat
{
public:

   RFPSFParametricShape( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFPSFParametricShape* TheRFPSFParametricShapeParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricAspectRatio : public MetaFloat
{
public:

   RFPSFParametricAspectRatio( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFPSFParametricAspectRatio* TheRFPSFParametricAspectRatioParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricRotationAngle : public MetaFloat
{
public:

   RFPSFParametricRotationAngle( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFPSFParametricRotationAngle* TheRFPSFParametricRotationAngleParameter;

// ----------------------------------------------------------------------------

class RFPSFMotionLength : public MetaFloat
{
public:

   RFPSFMotionLength( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFPSFMotionLength* TheRFPSFMotionLengthParameter;

// ----------------------------------------------------------------------------

class RFPSFMotionRotationAngle : public MetaFloat
{
public:

   RFPSFMotionRotationAngle( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFPSFMotionRotationAngle* TheRFPSFMotionRotationAngleParameter;

// ----------------------------------------------------------------------------

class RFPSFViewId : public MetaString
{
public:

   RFPSFViewId( MetaProcess* );

   virtual IsoString Id() const;
};

extern RFPSFViewId* TheRFPSFViewIdParameter;

// ----------------------------------------------------------------------------

class RFAmount : public MetaFloat
{
public:

   RFAmount( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFAmount* TheRFAmountParameter;

// ----------------------------------------------------------------------------

class RFToLuminance : public MetaBoolean
{
public:

   RFToLuminance( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual bool DefaultValue() const;
};

extern RFToLuminance* TheRFToLuminanceParameter;

// ----------------------------------------------------------------------------

class RFLinear : public MetaBoolean
{
public:

   RFLinear( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern RFLinear* TheRFLinearParameter;

// ----------------------------------------------------------------------------

class RFDeringing : public MetaBoolean
{
public:

   RFDeringing( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern RFDeringing* TheRFDeringingParameter;

// ----------------------------------------------------------------------------

class RFDeringingDark : public MetaFloat
{
public:

   RFDeringingDark( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern RFDeringingDark* TheRFDeringingDarkParameter;

// ----------------------------------------------------------------------------

class RFDeringingBright : public MetaFloat
{
public:

   RFDeringingBright( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern RFDeringingBright* TheRFDeringingBrightParameter;

// ----------------------------------------------------------------------------

class RFOutputDeringingMaps : public MetaBoolean
{
public:

   RFOutputDeringingMaps( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern RFOutputDeringingMaps* TheRFOutputDeringingMapsParameter;

// ----------------------------------------------------------------------------

class RFRangeLow : public MetaFloat
{
public:

   RFRangeLow( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFRangeLow* TheRFRangeLowParameter;

// ----------------------------------------------------------------------------

class RFRangeHigh : public MetaFloat
{
public:

   RFRangeHigh( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern RFRangeHigh* TheRFRangeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __RestorationFilterParameters_h

// ----------------------------------------------------------------------------
// EOF RestorationFilterParameters.h - Released 2019-01-21T12:06:42Z
