//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.12.01.0359
// ----------------------------------------------------------------------------
// HDRCompositionParameters.h - Released 2016/12/30 01:41:29 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __HDRCompositionParameters_h
#define __HDRCompositionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class HCImages : public MetaTable
{
public:

   HCImages( MetaProcess* );

   virtual IsoString Id() const;
};

extern HCImages* TheHCImagesParameter;

// ----------------------------------------------------------------------------

class HCImageEnabled : public MetaBoolean
{
public:

   HCImageEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern HCImageEnabled* TheHCImageEnabledParameter;

// ----------------------------------------------------------------------------

class HCImagePath : public MetaString
{
public:

   HCImagePath( MetaTable* );

   virtual IsoString Id() const;
};

extern HCImagePath* TheHCImagePathParameter;

// ----------------------------------------------------------------------------

class HCInputHints : public MetaString
{
public:

   HCInputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern HCInputHints* TheHCInputHintsParameter;

// ----------------------------------------------------------------------------

class HCMaskBinarizingThreshold : public MetaFloat
{
public:

   HCMaskBinarizingThreshold( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCMaskBinarizingThreshold* TheHCMaskBinarizingThresholdParameter;

// ----------------------------------------------------------------------------

class HCMaskSmoothness : public MetaInt32
{
public:

   HCMaskSmoothness( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCMaskSmoothness* TheHCMaskSmoothnessParameter;

// ----------------------------------------------------------------------------

class HCMaskGrowth : public MetaInt32
{
public:

   HCMaskGrowth( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCMaskGrowth* TheHCMaskGrowthParameter;

// ----------------------------------------------------------------------------

class HCReplaceLargeScales : public MetaInt32
{
public:

   HCReplaceLargeScales( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCReplaceLargeScales* TheHCReplaceLargeScalesParameter;

// ----------------------------------------------------------------------------

class HCAutoExposures : public MetaBoolean
{
public:

   HCAutoExposures( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern HCAutoExposures* TheHCAutoExposuresParameter;

// ----------------------------------------------------------------------------

class HCRejectBlack : public MetaBoolean
{
public:

   HCRejectBlack( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern HCRejectBlack* TheHCRejectBlackParameter;

// ----------------------------------------------------------------------------

class HCUseFittingRegion : public MetaBoolean
{
public:

   HCUseFittingRegion( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern HCUseFittingRegion* TheHCUseFittingRegionParameter;

// ----------------------------------------------------------------------------

class HCFittingRectX0 : public MetaInt32
{
public:

   HCFittingRectX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCFittingRectX0* TheHCFittingRectX0Parameter;

// ----------------------------------------------------------------------------

class HCFittingRectY0 : public MetaInt32
{
public:

   HCFittingRectY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCFittingRectY0* TheHCFittingRectY0Parameter;

// ----------------------------------------------------------------------------

class HCFittingRectX1 : public MetaInt32
{
public:

   HCFittingRectX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCFittingRectX1* TheHCFittingRectX1Parameter;

// ----------------------------------------------------------------------------

class HCFittingRectY1 : public MetaInt32
{
public:

   HCFittingRectY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HCFittingRectY1* TheHCFittingRectY1Parameter;

// ----------------------------------------------------------------------------

class HCGenerate64BitResult : public MetaBoolean
{
public:

   HCGenerate64BitResult( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern HCGenerate64BitResult* TheHCGenerate64BitResultParameter;

// ----------------------------------------------------------------------------

class HCOutputMasks : public MetaBoolean
{
public:

   HCOutputMasks( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern HCOutputMasks* TheHCOutputMasksParameter;

// ----------------------------------------------------------------------------

class HCClosePreviousImages : public MetaBoolean
{
public:

   HCClosePreviousImages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern HCClosePreviousImages* TheHCClosePreviousImagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __HDRCompositionParameters_h

// ----------------------------------------------------------------------------
// EOF HDRCompositionParameters.h - Released 2016/12/30 01:41:29 UTC
