//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0364
// ----------------------------------------------------------------------------
// AutoHistogramParameters.h - Released 2017-04-14T23:07:12Z
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

#ifndef __AutoHistogramParameters_h
#define __AutoHistogramParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class AHClip : public MetaBoolean
{
public:

   AHClip( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual bool DefaultValue() const;
};

extern AHClip* TheAHClipParameter;

// ----------------------------------------------------------------------------

class AHClipTogether : public MetaBoolean
{
public:

   AHClipTogether( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual bool DefaultValue() const;
};

extern AHClipTogether* TheAHClipTogetherParameter;

// ----------------------------------------------------------------------------

class AHClipLowR : public MetaFloat
{
public:

   AHClipLowR( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHClipLowR* TheAHClipLowRParameter;

// ----------------------------------------------------------------------------

class AHClipLowG : public MetaFloat
{
public:

   AHClipLowG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHClipLowG* TheAHClipLowGParameter;

// ----------------------------------------------------------------------------

class AHClipLowB : public MetaFloat
{
public:

   AHClipLowB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHClipLowB* TheAHClipLowBParameter;

// ----------------------------------------------------------------------------

class AHClipHighR : public MetaFloat
{
public:

   AHClipHighR( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

// ----------------------------------------------------------------------------

extern AHClipHighR* TheAHClipHighRParameter;

class AHClipHighG : public MetaFloat
{
public:

   AHClipHighG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHClipHighG* TheAHClipHighGParameter;

// ----------------------------------------------------------------------------

class AHClipHighB : public MetaFloat
{
public:

   AHClipHighB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHClipHighB* TheAHClipHighBParameter;

// ----------------------------------------------------------------------------

class AHStretch : public MetaBoolean
{
public:

   AHStretch( MetaProcess* );
   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual bool DefaultValue() const;
};

extern AHStretch* TheAHStretchParameter;

// ----------------------------------------------------------------------------

class AHStretchTogether : public MetaBoolean
{
public:

   AHStretchTogether( MetaProcess* );
   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual bool DefaultValue() const;
};

extern AHStretchTogether* TheAHStretchTogetherParameter;

// ----------------------------------------------------------------------------

class AHStretchMethod : public MetaEnumeration
{
public:

   enum { Gamma,
          Logarithm,
          MTF,
          NumberOfItems,
          Default = Gamma };

   AHStretchMethod( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern AHStretchMethod* TheAHStretchMethodParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianR : public MetaDouble
{
public:

   AHTargetMedianR( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHTargetMedianR* TheAHTargetMedianRParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianG : public MetaDouble
{
public:

   AHTargetMedianG( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHTargetMedianG* TheAHTargetMedianGParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianB : public MetaDouble
{
public:

   AHTargetMedianB( MetaProcess* );

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern AHTargetMedianB* TheAHTargetMedianBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __AutoHistogramParameters_h

// ----------------------------------------------------------------------------
// EOF AutoHistogramParameters.h - Released 2017-04-14T23:07:12Z
