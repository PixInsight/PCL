//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0226
// ----------------------------------------------------------------------------
// UnsharpMaskParameters.h - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#ifndef __UnsharpMaskParameters_h
#define __UnsharpMaskParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class USMSigma : public MetaFloat
{
public:

   USMSigma( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern USMSigma* TheUSMSigmaParameter;

// ----------------------------------------------------------------------------

class USMAmount : public MetaFloat
{
public:

   USMAmount( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern USMAmount* TheUSMAmountParameter;

// ----------------------------------------------------------------------------

class USMUseLuminance : public MetaBoolean
{
public:

   USMUseLuminance( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern USMUseLuminance* TheUSMUseLuminanceParameter;

// ----------------------------------------------------------------------------

class USMLinear : public MetaBoolean
{
public:

   USMLinear( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern USMLinear* TheUSMLinearParameter;

// ----------------------------------------------------------------------------

class USMDeringing : public MetaBoolean
{
public:

   USMDeringing( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern USMDeringing* TheUSMDeringingParameter;

// ----------------------------------------------------------------------------

class USMDeringingDark : public MetaFloat
{
public:

   USMDeringingDark( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern USMDeringingDark* TheUSMDeringingDarkParameter;

// ----------------------------------------------------------------------------

class USMDeringingBright : public MetaFloat
{
public:

   USMDeringingBright( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern USMDeringingBright* TheUSMDeringingBrightParameter;

// ----------------------------------------------------------------------------

class USMOutputDeringingMaps : public MetaBoolean
{
public:

   USMOutputDeringingMaps( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern USMOutputDeringingMaps* TheUSMOutputDeringingMapsParameter;

// ----------------------------------------------------------------------------

class USMRangeLow : public MetaFloat
{
public:

   USMRangeLow( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern USMRangeLow* TheUSMRangeLowParameter;

// ----------------------------------------------------------------------------

class USMRangeHigh : public MetaFloat
{
public:

   USMRangeHigh( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern USMRangeHigh* TheUSMRangeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __UnsharpMaskParameters_h

// ----------------------------------------------------------------------------
// EOF UnsharpMaskParameters.h - Released 2017-05-02T09:43:00Z
