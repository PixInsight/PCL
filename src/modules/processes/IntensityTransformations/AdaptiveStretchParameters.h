//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0397
// ----------------------------------------------------------------------------
// AdaptiveStretchParameters.h - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __AdaptiveStretchParameters_h
#define __AdaptiveStretchParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ASNoiseThreshold : public MetaDouble
{
public:

   ASNoiseThreshold( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern ASNoiseThreshold* TheASNoiseThresholdParameter;

// ----------------------------------------------------------------------------

class ASProtection : public MetaDouble
{
public:

   ASProtection( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern ASProtection* TheASProtectionParameter;

// ----------------------------------------------------------------------------

class ASUseProtection : public MetaBoolean
{
public:

   ASUseProtection( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ASUseProtection* TheASUseProtectionParameter;

// ----------------------------------------------------------------------------

class ASMaxCurvePoints : public MetaInt32
{
public:

   ASMaxCurvePoints( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ASMaxCurvePoints* TheASMaxCurvePointsParameter;

// ----------------------------------------------------------------------------

class ASUseROI : public MetaBoolean
{
public:

   ASUseROI( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ASUseROI* TheASUseROIParameter;

// ----------------------------------------------------------------------------

class ASROIX0 : public MetaInt32
{
public:

   ASROIX0( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ASROIX0* TheASROIX0Parameter;

// ----------------------------------------------------------------------------

class ASROIY0 : public MetaInt32
{
public:

   ASROIY0( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ASROIY0* TheASROIY0Parameter;

// ----------------------------------------------------------------------------

class ASROIX1 : public MetaInt32
{
public:

   ASROIX1( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ASROIX1* TheASROIX1Parameter;

// ----------------------------------------------------------------------------

class ASROIY1 : public MetaInt32
{
public:

   ASROIY1( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ASROIY1* TheASROIY1Parameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __AdaptiveStretchParameters_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchParameters.h - Released 2017-07-18T16:14:18Z
