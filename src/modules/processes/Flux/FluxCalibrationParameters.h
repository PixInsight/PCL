//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0210
// ----------------------------------------------------------------------------
// FluxCalibrationParameters.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#ifndef __FluxCalibrationParameters_h
#define __FluxCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCParameterMode : public MetaEnumeration
{
public:

   enum { Literal,
          StandardKeyword,
          CustomKeyword,
          NumberOfItems,
          Default = Literal };

   FCParameterMode( MetaProcess* P ) : MetaEnumeration( P )
   {
   }

   virtual IsoString Id() const = 0;

   virtual size_type NumberOfElements() const
   {
      return NumberOfItems;
   }

   virtual IsoString ElementId( size_type index ) const
   {
      switch ( index )
      {
      default:
      case Literal:         return "Literal";
      case StandardKeyword: return "StandardKeyword";
      case CustomKeyword:   return "CustomKeyword";
      }
   }

   virtual int ElementValue( size_type index ) const
   {
      return int( index );
   }

   virtual size_type DefaultValueIndex() const
   {
      return Default;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCWavelengthValue : public MetaFloat
{
public:

   FCWavelengthValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double DefaultValue() const;
};

extern FCWavelengthValue* TheFCWavelengthValueParameter;

// ----------------------------------------------------------------------------

class FCWavelengthMode : public FCParameterMode
{
public:

   FCWavelengthMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCWavelengthMode* TheFCWavelengthModeParameter;

// ----------------------------------------------------------------------------

class FCWavelengthKeyword : public MetaString
{
public:

   FCWavelengthKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCWavelengthKeyword* TheFCWavelengthKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCTransmissivityValue : public MetaFloat
{
public:

   FCTransmissivityValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern FCTransmissivityValue* TheFCTransmissivityValueParameter;

// ----------------------------------------------------------------------------

class FCTransmissivityMode : public FCParameterMode
{
public:

   FCTransmissivityMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCTransmissivityMode* TheFCTransmissivityModeParameter;

// ----------------------------------------------------------------------------

class FCTransmissivityKeyword : public MetaString
{
public:

   FCTransmissivityKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCTransmissivityKeyword* TheFCTransmissivityKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCFilterWidthValue : public MetaFloat
{
public:

   FCFilterWidthValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double DefaultValue() const;
};

extern FCFilterWidthValue* TheFCFilterWidthValueParameter;

// ----------------------------------------------------------------------------

class FCFilterWidthMode : public FCParameterMode
{
public:

   FCFilterWidthMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCFilterWidthMode* TheFCFilterWidthModeParameter;

// ----------------------------------------------------------------------------

class FCFilterWidthKeyword : public MetaString
{
public:

   FCFilterWidthKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCFilterWidthKeyword* TheFCFilterWidthKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCApertureValue : public MetaFloat
{
public:

   FCApertureValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double DefaultValue() const;
};

extern FCApertureValue* TheFCApertureValueParameter;

// ----------------------------------------------------------------------------

class FCApertureMode : public FCParameterMode
{
public:

   FCApertureMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCApertureMode* TheFCApertureModeParameter;

// ----------------------------------------------------------------------------

class FCApertureKeyword : public MetaString
{
public:

   FCApertureKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCApertureKeyword* TheFCApertureKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCCentralObstructionValue : public MetaFloat
{
public:

   FCCentralObstructionValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double DefaultValue() const;
};

extern FCCentralObstructionValue* TheFCCentralObstructionValueParameter;

// ----------------------------------------------------------------------------

class FCCentralObstructionMode : public FCParameterMode
{
public:

   FCCentralObstructionMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCCentralObstructionMode* TheFCCentralObstructionModeParameter;

// ----------------------------------------------------------------------------

class FCCentralObstructionKeyword : public MetaString
{
public:

   FCCentralObstructionKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCCentralObstructionKeyword* TheFCCentralObstructionKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCExposureTimeValue : public MetaFloat
{
public:

   FCExposureTimeValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double DefaultValue() const;
};

extern FCExposureTimeValue* TheFCExposureTimeValueParameter;

// ----------------------------------------------------------------------------

class FCExposureTimeMode : public FCParameterMode
{
public:

   FCExposureTimeMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCExposureTimeMode* TheFCExposureTimeModeParameter;

// ----------------------------------------------------------------------------

class FCExposureTimeKeyword : public MetaString
{
public:

   FCExposureTimeKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCExposureTimeKeyword* TheFCExposureTimeKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCAtmosphericExtinctionValue : public MetaFloat
{
public:

   FCAtmosphericExtinctionValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern FCAtmosphericExtinctionValue* TheFCAtmosphericExtinctionValueParameter;

// ----------------------------------------------------------------------------

class FCAtmosphericExtinctionMode : public FCParameterMode
{
public:

   FCAtmosphericExtinctionMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCAtmosphericExtinctionMode* TheFCAtmosphericExtinctionModeParameter;

// ----------------------------------------------------------------------------

class FCAtmosphericExtinctionKeyword : public MetaString
{
public:

   FCAtmosphericExtinctionKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCAtmosphericExtinctionKeyword* TheFCAtmosphericExtinctionKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCSensorGainValue : public MetaFloat
{
public:

   FCSensorGainValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double DefaultValue() const;
};

extern FCSensorGainValue* TheFCSensorGainValueParameter;

// ----------------------------------------------------------------------------

class FCSensorGainMode : public FCParameterMode
{
public:

   FCSensorGainMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCSensorGainMode* TheFCSensorGainModeParameter;

// ----------------------------------------------------------------------------

class FCSensorGainKeyword : public MetaString
{
public:

   FCSensorGainKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCSensorGainKeyword* TheFCSensorGainKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCQuantumEfficiencyValue : public MetaFloat
{
public:

   FCQuantumEfficiencyValue( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern FCQuantumEfficiencyValue* TheFCQuantumEfficiencyValueParameter;

// ----------------------------------------------------------------------------

class FCQuantumEfficiencyMode : public FCParameterMode
{
public:

   FCQuantumEfficiencyMode( MetaProcess* P );
   virtual IsoString Id() const;
};

extern FCQuantumEfficiencyMode* TheFCQuantumEfficiencyModeParameter;

// ----------------------------------------------------------------------------

class FCQuantumEfficiencyKeyword : public MetaString
{
public:

   FCQuantumEfficiencyKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern FCQuantumEfficiencyKeyword* TheFCQuantumEfficiencyKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __FluxCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF FluxCalibrationParameters.h - Released 2019-01-21T12:06:41Z
