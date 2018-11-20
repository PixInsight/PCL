//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0319
// ----------------------------------------------------------------------------
// BackgroundNeutralizationParameters.h - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __BackgroundNeutralizationParameters_h
#define __BackgroundNeutralizationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class BNBackgroundReferenceViewId : public MetaString
{
public:

   BNBackgroundReferenceViewId( MetaProcess* );

   virtual IsoString Id() const;
};

extern BNBackgroundReferenceViewId* TheBNBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class BNBackgroundLow : public MetaFloat
{
public:

   BNBackgroundLow( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern BNBackgroundLow* TheBNBackgroundLowParameter;

// ----------------------------------------------------------------------------

class BNBackgroundHigh : public MetaFloat
{
public:

   BNBackgroundHigh( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern BNBackgroundHigh* TheBNBackgroundHighParameter;

// ----------------------------------------------------------------------------

class BNUseROI : public MetaBoolean
{
public:

   BNUseROI( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern BNUseROI* TheBNUseROIParameter;

// ----------------------------------------------------------------------------

class BNROIX0 : public MetaInt32
{
public:

   BNROIX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern BNROIX0* TheBNROIX0Parameter;


// ----------------------------------------------------------------------------

class BNROIY0 : public MetaInt32
{
public:

   BNROIY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern BNROIY0* TheBNROIY0Parameter;

// ----------------------------------------------------------------------------

class BNROIX1 : public MetaInt32
{
public:

   BNROIX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern BNROIX1* TheBNROIX1Parameter;

// ----------------------------------------------------------------------------

class BNROIY1 : public MetaInt32
{
public:

   BNROIY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern BNROIY1* TheBNROIY1Parameter;

// ----------------------------------------------------------------------------

class BNMode : public MetaEnumeration
{
public:

   enum { TargetBackground,
          Rescale,
          RescaleAsNeeded,
          Truncate,
          NumberOfModes,
          Default = RescaleAsNeeded };

   BNMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern BNMode* TheBNModeParameter;

// ----------------------------------------------------------------------------

class BNTargetBackground : public MetaFloat
{
public:

   BNTargetBackground( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern BNTargetBackground* TheBNTargetBackgroundParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __BackgroundNeutralizationParameters_h

// ----------------------------------------------------------------------------
// EOF BackgroundNeutralizationParameters.h - Released 2018-11-01T11:07:20Z
