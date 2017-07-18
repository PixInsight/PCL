//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 01.00.02.0317
// ----------------------------------------------------------------------------
// SimplexNoiseParameters.h - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
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

#ifndef __SimplexNoiseParameters_h
#define __SimplexNoiseParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SNAmount : public MetaFloat
{
public:

   SNAmount( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern SNAmount* TheSNAmountParameter;

// ----------------------------------------------------------------------------

class SNScale : public MetaInt32
{
public:

   SNScale( MetaProcess* );

   virtual IsoString Id() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern SNScale* TheSNScaleParameter;

// ----------------------------------------------------------------------------

class SNOffsetX : public MetaInt32
{
public:

   SNOffsetX( MetaProcess* );

   virtual IsoString Id() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern SNOffsetX* TheSNOffsetXParameter;

// ----------------------------------------------------------------------------

class SNOffsetY : public MetaInt32
{
public:

   SNOffsetY( MetaProcess* );

   virtual IsoString Id() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern SNOffsetY* TheSNOffsetYParameter;

// ----------------------------------------------------------------------------

class SNOperator : public MetaEnumeration
{
public:

   enum { Copy,   // Normal mode, replace target
          Add,    // Add noise to target
          Sub,    // Subtract noise from target
          Mul,    // Multiply target by noise
          Div,    // Divide target by noise
          Pow,    // Target = Target ** noise (exponentiation)
          Dif,    // Target = |Target - noise|
          Screen, // Target = ~(~Target * ~noise)
          Or,     // Target |= noise
          And,    // Target &= noise
          Xor,    // Target ^= noise
          Nor,    // Target |= ~noise
          Nand,   // Target &= ~noise
          Xnor,   // Target ^= ~noise

          NumberOfOperators,
          Default = Copy };

   SNOperator( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
};

extern SNOperator* TheSNOperatorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __SimplexNoiseParameters_h

// ----------------------------------------------------------------------------
// EOF SimplexNoiseParameters.h - Released 2017-07-18T16:14:18Z
