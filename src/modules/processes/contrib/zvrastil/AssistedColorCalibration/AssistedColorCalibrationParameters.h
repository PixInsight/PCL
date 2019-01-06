//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 01.00.00.0234
// ----------------------------------------------------------------------------
// AssistedColorCalibrationParameters.h - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2018 Zbynek Vrastil
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#ifndef __AssistedColorCalibrationParameters_h
#define __AssistedColorCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ACCRedCorrectionFactor : public MetaFloat
{
public:

   ACCRedCorrectionFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ACCRedCorrectionFactor* TheACCRedCorrectionFactor;

// ----------------------------------------------------------------------------

class ACCGreenCorrectionFactor : public MetaFloat
{
public:

   ACCGreenCorrectionFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ACCGreenCorrectionFactor* TheACCGreenCorrectionFactor;

// ----------------------------------------------------------------------------

class ACCBlueCorrectionFactor : public MetaFloat
{
public:

   ACCBlueCorrectionFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ACCBlueCorrectionFactor* TheACCBlueCorrectionFactor;

// ----------------------------------------------------------------------------

class ACCBackgroundReference : public MetaString
{
public:

   ACCBackgroundReference( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String DefaultValue() const;
};

extern ACCBackgroundReference* TheACCBackgroundReference;

// ----------------------------------------------------------------------------

class ACCHistogramShadows : public MetaFloat
{
public:

   ACCHistogramShadows( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ACCHistogramShadows* TheACCHistogramShadows;

// ----------------------------------------------------------------------------

class ACCHistogramHighlights : public MetaFloat
{
public:

   ACCHistogramHighlights( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ACCHistogramHighlights* TheACCHistogramHighlights;

// ----------------------------------------------------------------------------

class ACCHistogramMidtones : public MetaFloat
{
public:

   ACCHistogramMidtones( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ACCHistogramMidtones* TheACCHistogramMidtones;

// ----------------------------------------------------------------------------

class ACCSaturationBoost : public MetaFloat
{
public:

   ACCSaturationBoost( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ACCSaturationBoost* TheACCSaturationBoost;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __AssistedColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationParameters.h - Released 2018-12-12T09:25:25Z
