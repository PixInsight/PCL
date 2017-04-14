//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.04.03.0222
// ----------------------------------------------------------------------------
// DebayerParameters.h - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#ifndef __DebayerParameters_h
#define __DebayerParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DebayerBayerPatternParameter : public MetaEnumeration
{
public:

   enum { RGGB,
          BGGR,
          GBRG,
          GRBG,
          NumberOfItems,
          Default = RGGB };

   DebayerBayerPatternParameter( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DebayerBayerPatternParameter* TheDebayerBayerPatternParameter;

// ----------------------------------------------------------------------------

class DebayerMethodParameter : public MetaEnumeration
{
public:

   enum { SuperPixel,
          Bilinear,
          VNG,
          /*AHD,*/ // ### Uncomment when implemented
          NumberOfItems,
          Default = VNG };

   DebayerMethodParameter( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DebayerMethodParameter* TheDebayerMethodParameter;

// ----------------------------------------------------------------------------

class DebayerEvaluateNoise : public MetaBoolean
{
public:

   DebayerEvaluateNoise( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DebayerEvaluateNoise* TheDebayerEvaluateNoiseParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEvaluationAlgorithm : public MetaEnumeration
{
public:

   enum { KSigma,
          MRS,
          NumberOfNoiseEvaluationAlgorithms,
          Default = MRS };

   DebayerNoiseEvaluationAlgorithm( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DebayerNoiseEvaluationAlgorithm* TheDebayerNoiseEvaluationAlgorithmParameter;

// ----------------------------------------------------------------------------

class DebayerShowImages : public MetaBoolean
{
public:

   DebayerShowImages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DebayerShowImages* TheDebayerShowImagesParameter;

// ----------------------------------------------------------------------------

class DebayerOutputImage : public MetaString
{
public:

   DebayerOutputImage( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputImage* TheDebayerOutputImageParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateR : public MetaFloat
{
public:

   DebayerNoiseEstimateR( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseEstimateR* TheDebayerNoiseEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateG : public MetaFloat
{
public:

   DebayerNoiseEstimateG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseEstimateG* TheDebayerNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateB : public MetaFloat
{
public:

   DebayerNoiseEstimateB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseEstimateB* TheDebayerNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionR : public MetaFloat
{
public:

   DebayerNoiseFractionR( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseFractionR* TheDebayerNoiseFractionRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionG : public MetaFloat
{
public:

   DebayerNoiseFractionG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseFractionG* TheDebayerNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionB : public MetaFloat
{
public:

   DebayerNoiseFractionB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseFractionB* TheDebayerNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmR : public MetaString
{
public:

   DebayerNoiseAlgorithmR( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseAlgorithmR* TheDebayerNoiseAlgorithmRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmG : public MetaString
{
public:

   DebayerNoiseAlgorithmG( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseAlgorithmG* TheDebayerNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmB : public MetaString
{
public:

   DebayerNoiseAlgorithmB( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseAlgorithmB* TheDebayerNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DebayerParameters_h

// ----------------------------------------------------------------------------
// EOF DebayerParameters.h - Released 2017-04-14T23:07:12Z
