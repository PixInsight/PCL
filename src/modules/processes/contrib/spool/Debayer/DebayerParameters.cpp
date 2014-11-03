// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Debayer Process Module Version 01.04.03.0144
// ****************************************************************************
// DebayerParameters.cpp - Released 2014/10/29 07:35:26 UTC
// ****************************************************************************
// This file is part of the standard Debayer PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "DebayerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerBayerPatternParameter*	   TheDebayerBayerPatternParameter = 0;
DebayerMethodParameter*          TheDebayerMethodParameter = 0;
DebayerEvaluateNoise*            TheDebayerEvaluateNoiseParameter = 0;
DebayerNoiseEvaluationAlgorithm* TheDebayerNoiseEvaluationAlgorithmParameter = 0;
DebayerShowImages*               TheDebayerShowImagesParameter = 0;
DebayerOutputImage*              TheDebayerOutputImageParameter = 0;
DebayerNoiseEstimateR*           TheDebayerNoiseEstimateRParameter = 0;
DebayerNoiseEstimateG*           TheDebayerNoiseEstimateGParameter = 0;
DebayerNoiseEstimateB*           TheDebayerNoiseEstimateBParameter = 0;
DebayerNoiseFractionR*           TheDebayerNoiseFractionRParameter = 0;
DebayerNoiseFractionG*           TheDebayerNoiseFractionGParameter = 0;
DebayerNoiseFractionB*           TheDebayerNoiseFractionBParameter = 0;
DebayerNoiseAlgorithmR*          TheDebayerNoiseAlgorithmRParameter = 0;
DebayerNoiseAlgorithmG*          TheDebayerNoiseAlgorithmGParameter = 0;
DebayerNoiseAlgorithmB*          TheDebayerNoiseAlgorithmBParameter = 0;

// ----------------------------------------------------------------------------

DebayerBayerPatternParameter::DebayerBayerPatternParameter( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerBayerPatternParameter = this;
}

IsoString DebayerBayerPatternParameter::Id() const
{
   return "bayerPattern";
}

IsoString DebayerBayerPatternParameter::Aliases() const
{
   return "BayerPattern";
}

size_type DebayerBayerPatternParameter::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerBayerPatternParameter::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RGGB: return "RGGB";
   case BGGR: return "BGGR";
   case GBRG: return "GBRG";
   case GRBG: return "GRBG";
   }
}

int DebayerBayerPatternParameter::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerBayerPatternParameter::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

DebayerMethodParameter::DebayerMethodParameter( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerMethodParameter = this;
}

IsoString DebayerMethodParameter::Id() const
{
   return "debayerMethod";
}

IsoString DebayerMethodParameter::Aliases() const
{
   return "DebayerMethod";
}

size_type DebayerMethodParameter::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerMethodParameter::ElementId( size_type i ) const
{
   switch ( i )
   {
   case SuperPixel: return "SuperPixel";
   case Bilinear:   return "Bilinear";
   default:
   case VNG:        return "VNG";
   // case AHD:        return "AHD"; ### Uncomment when implemented
   }
}

int DebayerMethodParameter::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerMethodParameter::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

DebayerEvaluateNoise::DebayerEvaluateNoise( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerEvaluateNoiseParameter = this;
}

IsoString DebayerEvaluateNoise::Id() const
{
   return "evaluateNoise";
}

bool DebayerEvaluateNoise::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEvaluationAlgorithm::DebayerNoiseEvaluationAlgorithm( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerNoiseEvaluationAlgorithmParameter = this;
}

IsoString DebayerNoiseEvaluationAlgorithm::Id() const
{
   return "noiseEvaluationAlgorithm";
}

size_type DebayerNoiseEvaluationAlgorithm::NumberOfElements() const
{
   return NumberOfNoiseEvaluationAlgorithms;
}

IsoString DebayerNoiseEvaluationAlgorithm::ElementId( size_type i ) const
{
   switch ( i )
   {
   case KSigma: return "NoiseEvaluation_KSigma";
   default:
   case MRS:    return "NoiseEvaluation_MRS";
   }
}

int DebayerNoiseEvaluationAlgorithm::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerNoiseEvaluationAlgorithm::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DebayerShowImages::DebayerShowImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerShowImagesParameter = this;
}

IsoString DebayerShowImages::Id() const
{
   return "showImages";
}

bool DebayerShowImages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputImage::DebayerOutputImage( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputImageParameter = this;
}

IsoString DebayerOutputImage::Id() const
{
   return "outputImage";
}

bool DebayerOutputImage::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateR::DebayerNoiseEstimateR( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseEstimateRParameter = this;
}

IsoString DebayerNoiseEstimateR::Id() const
{
   return "noiseEstimateR";
}

int DebayerNoiseEstimateR::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateG::DebayerNoiseEstimateG( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseEstimateGParameter = this;
}

IsoString DebayerNoiseEstimateG::Id() const
{
   return "noiseEstimateG";
}

int DebayerNoiseEstimateG::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateB::DebayerNoiseEstimateB( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseEstimateBParameter = this;
}

IsoString DebayerNoiseEstimateB::Id() const
{
   return "noiseEstimateB";
}

int DebayerNoiseEstimateB::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionR::DebayerNoiseFractionR( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseFractionRParameter = this;
}

IsoString DebayerNoiseFractionR::Id() const
{
   return "noiseFractionR";
}

int DebayerNoiseFractionR::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionG::DebayerNoiseFractionG( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseFractionGParameter = this;
}

IsoString DebayerNoiseFractionG::Id() const
{
   return "noiseFractionG";
}

int DebayerNoiseFractionG::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionB::DebayerNoiseFractionB( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseFractionBParameter = this;
}

IsoString DebayerNoiseFractionB::Id() const
{
   return "noiseFractionB";
}

int DebayerNoiseFractionB::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmR::DebayerNoiseAlgorithmR( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmRParameter = this;
}

IsoString DebayerNoiseAlgorithmR::Id() const
{
   return "noiseAlgorithmR";
}

bool DebayerNoiseAlgorithmR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmG::DebayerNoiseAlgorithmG( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmGParameter = this;
}

IsoString DebayerNoiseAlgorithmG::Id() const
{
   return "noiseAlgorithmG";
}

bool DebayerNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmB::DebayerNoiseAlgorithmB( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmBParameter = this;
}

IsoString DebayerNoiseAlgorithmB::Id() const
{
   return "noiseAlgorithmB";
}

bool DebayerNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF DebayerParameters.cpp - Released 2014/10/29 07:35:26 UTC
