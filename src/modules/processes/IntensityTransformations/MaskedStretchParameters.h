//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0430
// ----------------------------------------------------------------------------
// MaskedStretchParameters.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __MaskedStretchParameters_h
#define __MaskedStretchParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class MSTargetBackground : public MetaDouble
{
public:

   MSTargetBackground( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern MSTargetBackground* TheMSTargetBackgroundParameter;

// ----------------------------------------------------------------------------

class MSNumberOfIterations : public MetaInt32
{
public:

   MSNumberOfIterations( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern MSNumberOfIterations* TheMSNumberOfIterationsParameter;

// ----------------------------------------------------------------------------

class MSClippingFraction : public MetaDouble
{
public:

   MSClippingFraction( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern MSClippingFraction* TheMSClippingFractionParameter;

// ----------------------------------------------------------------------------

class MSBackgroundReferenceViewId : public MetaString
{
public:

   MSBackgroundReferenceViewId( MetaProcess* );

   virtual IsoString Id() const;
};

extern MSBackgroundReferenceViewId* TheMSBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class MSBackgroundLow : public MetaDouble
{
public:

   MSBackgroundLow( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern MSBackgroundLow* TheMSBackgroundLowParameter;

// ----------------------------------------------------------------------------

class MSBackgroundHigh : public MetaDouble
{
public:

   MSBackgroundHigh( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern MSBackgroundHigh* TheMSBackgroundHighParameter;

// ----------------------------------------------------------------------------

class MSUseROI : public MetaBoolean
{
public:

   MSUseROI( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern MSUseROI* TheMSUseROIParameter;

// ----------------------------------------------------------------------------

class MSROIX0 : public MetaInt32
{
public:

   MSROIX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern MSROIX0* TheMSROIX0Parameter;

// ----------------------------------------------------------------------------

class MSROIY0 : public MetaInt32
{
public:

   MSROIY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern MSROIY0* TheMSROIY0Parameter;

// ----------------------------------------------------------------------------

class MSROIX1 : public MetaInt32
{
public:

   MSROIX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern MSROIX1* TheMSROIX1Parameter;

// ----------------------------------------------------------------------------

class MSROIY1 : public MetaInt32
{
public:

   MSROIY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern MSROIY1* TheMSROIY1Parameter;

// ----------------------------------------------------------------------------

class MSMaskType : public MetaEnumeration
{
public:

   enum { Intensity,
          Value,
          NumberOfItems,
          Default = Intensity };

   MSMaskType( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern MSMaskType* TheMSMaskTypeParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __MaskedStretchParameters_h

// ----------------------------------------------------------------------------
// EOF MaskedStretchParameters.h - Released 2019-01-21T12:06:41Z
