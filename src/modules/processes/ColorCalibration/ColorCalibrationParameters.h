//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.02.00.0228
// ----------------------------------------------------------------------------
// ColorCalibrationParameters.h - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ColorCalibrationParameters_h
#define __ColorCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CCWhiteReferenceViewId : public MetaString
{
public:

   CCWhiteReferenceViewId( MetaProcess* );

   virtual IsoString Id() const;
};

extern CCWhiteReferenceViewId* TheCCWhiteReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class CCWhiteLow : public MetaFloat
{
public:

   CCWhiteLow( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern CCWhiteLow* TheCCWhiteLowParameter;

// ----------------------------------------------------------------------------

class CCWhiteHigh : public MetaFloat
{
public:

   CCWhiteHigh( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern CCWhiteHigh* TheCCWhiteHighParameter;

// ----------------------------------------------------------------------------

class CCWhiteUseROI : public MetaBoolean
{
public:

   CCWhiteUseROI( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern CCWhiteUseROI* TheCCWhiteUseROIParameter;

// ----------------------------------------------------------------------------

class CCWhiteROIX0 : public MetaInt32
{
public:

   CCWhiteROIX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCWhiteROIX0* TheCCWhiteROIX0Parameter;


// ----------------------------------------------------------------------------

class CCWhiteROIY0 : public MetaInt32
{
public:

   CCWhiteROIY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCWhiteROIY0* TheCCWhiteROIY0Parameter;

// ----------------------------------------------------------------------------

class CCWhiteROIX1 : public MetaInt32
{
public:

   CCWhiteROIX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCWhiteROIX1* TheCCWhiteROIX1Parameter;

// ----------------------------------------------------------------------------

class CCWhiteROIY1 : public MetaInt32
{
public:

   CCWhiteROIY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCWhiteROIY1* TheCCWhiteROIY1Parameter;

// ----------------------------------------------------------------------------

class CCStructureDetection : public MetaBoolean
{
public:

   CCStructureDetection( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CCStructureDetection* TheCCStructureDetectionParameter;

// ----------------------------------------------------------------------------

class CCStructureLayers : public MetaInt32
{
public:

   CCStructureLayers( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCStructureLayers* TheCCStructureLayersParameter;

// ----------------------------------------------------------------------------

class CCNoiseLayers : public MetaInt32
{
public:

   CCNoiseLayers( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCNoiseLayers* TheCCNoiseLayersParameter;

// ----------------------------------------------------------------------------

class CCManualWhiteBalance : public MetaBoolean
{
public:

   CCManualWhiteBalance( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CCManualWhiteBalance* TheCCManualWhiteBalanceParameter;

// ----------------------------------------------------------------------------

class CCManualRedFactor : public MetaFloat
{
public:

   CCManualRedFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern CCManualRedFactor* TheCCManualRedFactorParameter;

// ----------------------------------------------------------------------------

class CCManualGreenFactor : public MetaFloat
{
public:

   CCManualGreenFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern CCManualGreenFactor* TheCCManualGreenFactorParameter;

// ----------------------------------------------------------------------------

class CCManualBlueFactor : public MetaFloat
{
public:

   CCManualBlueFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern CCManualBlueFactor* TheCCManualBlueFactorParameter;

// ----------------------------------------------------------------------------

class CCBackgroundReferenceViewId : public MetaString
{
public:

   CCBackgroundReferenceViewId( MetaProcess* );

   virtual IsoString Id() const;
};

extern CCBackgroundReferenceViewId* TheCCBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class CCBackgroundLow : public MetaFloat
{
public:

   CCBackgroundLow( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern CCBackgroundLow* TheCCBackgroundLowParameter;

// ----------------------------------------------------------------------------

class CCBackgroundHigh : public MetaFloat
{
public:

   CCBackgroundHigh( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern CCBackgroundHigh* TheCCBackgroundHighParameter;

// ----------------------------------------------------------------------------

class CCBackgroundUseROI : public MetaBoolean
{
public:

   CCBackgroundUseROI( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern CCBackgroundUseROI* TheCCBackgroundUseROIParameter;

// ----------------------------------------------------------------------------

class CCBackgroundROIX0 : public MetaInt32
{
public:

   CCBackgroundROIX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCBackgroundROIX0* TheCCBackgroundROIX0Parameter;


// ----------------------------------------------------------------------------

class CCBackgroundROIY0 : public MetaInt32
{
public:

   CCBackgroundROIY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCBackgroundROIY0* TheCCBackgroundROIY0Parameter;

// ----------------------------------------------------------------------------

class CCBackgroundROIX1 : public MetaInt32
{
public:

   CCBackgroundROIX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCBackgroundROIX1* TheCCBackgroundROIX1Parameter;

// ----------------------------------------------------------------------------

class CCBackgroundROIY1 : public MetaInt32
{
public:

   CCBackgroundROIY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CCBackgroundROIY1* TheCCBackgroundROIY1Parameter;

// ----------------------------------------------------------------------------

class CCOutputWhiteReferenceMask : public MetaBoolean
{
public:

   CCOutputWhiteReferenceMask( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CCOutputWhiteReferenceMask* TheCCOutputWhiteReferenceMaskParameter;

// ----------------------------------------------------------------------------

class CCOutputBackgroundReferenceMask : public MetaBoolean
{
public:

   CCOutputBackgroundReferenceMask( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CCOutputBackgroundReferenceMask* TheCCOutputBackgroundReferenceMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF ColorCalibrationParameters.h - Released 2015/11/26 16:00:12 UTC
