//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR STargetFERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#ifndef __SubframeSelectorParameters_h
#define __SubframeSelectorParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SSSubframes : public MetaTable
{
public:

   SSSubframes( MetaProcess* );

   virtual IsoString Id() const;
};

extern SSSubframes* TheSSSubframesParameter;

// ----------------------------------------------------------------------------

class SSSubframeEnabled : public MetaBoolean
{
public:

   SSSubframeEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SSSubframeEnabled* TheSSSubframeEnabledParameter;

// ----------------------------------------------------------------------------

class SSSubframePath : public MetaString
{
public:

   SSSubframePath( MetaTable* );

   virtual IsoString Id() const;
};

extern SSSubframePath* TheSSSubframePathParameter;

// ----------------------------------------------------------------------------

class SSSubframeScale : public MetaFloat
{
public:

   SSSubframeScale( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSSubframeScale* TheSSSubframeScaleParameter;

// ----------------------------------------------------------------------------

class SSCameraGain : public MetaFloat
{
public:

   SSCameraGain( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSCameraGain* TheSSCameraGainParameter;

// ----------------------------------------------------------------------------

class SSCameraResolution : public MetaEnumeration
{
   public:

   enum { Bits8,
      Bits10,
      Bits12,
      Bits14,
      Bits16,
      NumberOfItems,
      Default = Bits8 };

   SSCameraResolution( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
   IsoString ElementLabel( size_type) const;
   int ElementData( size_type) const;
};

extern SSCameraResolution* TheSSCameraResolutionParameter;

// ----------------------------------------------------------------------------

class SSSiteLocalMidnight : public MetaInt32
{
   public:

   SSSiteLocalMidnight( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSSiteLocalMidnight* TheSSSiteLocalMidnightParameter;

// ----------------------------------------------------------------------------

class SSScaleUnit : public MetaEnumeration
{
   public:

   enum { ArcSeconds,
      Pixel,
      NumberOfItems,
      Default = ArcSeconds };

   SSScaleUnit( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
   IsoString ElementLabel( size_type) const;
   IsoString ElementData( size_type) const;
};

extern SSScaleUnit* TheSSScaleUnitParameter;

// ----------------------------------------------------------------------------

class SSDataUnit : public MetaEnumeration
{
   public:

   enum { Electron,
      DataNumber,
      NumberOfItems,
      Default = Electron };

   SSDataUnit( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
   IsoString ElementLabel( size_type) const;
   IsoString ElementData( size_type) const;
};

extern SSDataUnit* TheSSDataUnitParameter;

// ----------------------------------------------------------------------------

class SSMeasurements : public MetaTable
{
   public:

   SSMeasurements( MetaProcess* );

   virtual IsoString Id() const;
};

extern SSMeasurements* TheSSMeasurementsParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEnabled : public MetaBoolean
{
   public:

   SSMeasurementEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SSMeasurementEnabled* TheSSMeasurementEnabledParameter;

// ----------------------------------------------------------------------------

class SSMeasurementLocked : public MetaBoolean
{
   public:

   SSMeasurementLocked( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SSMeasurementLocked* TheSSMeasurementLockedParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPath : public MetaString
{
   public:

   SSMeasurementPath( MetaTable* );

   virtual IsoString Id() const;
};

extern SSMeasurementPath* TheSSMeasurementPathParameter;

// ----------------------------------------------------------------------------

class SSMeasurementFWHM : public MetaFloat
{
   public:

   SSMeasurementFWHM( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementFWHM* TheSSMeasurementFWHMParameter;

// ----------------------------------------------------------------------------


   PCL_END_LOCAL

} // pcl

#endif   // __SubframeSelectorParameters_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.h - Released 2017-08-01T14:26:58Z
