//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0355
// ----------------------------------------------------------------------------
// ScreenTransferFunctionParameters.h - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#ifndef __ScreenTransferFunctionParameters_h
#define __ScreenTransferFunctionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------
// STFSet parameter.
//
// This is a table parameter including four rows. Each table row provides a STF
// corresponding to a display channel, namely:
//
//    row   channel
//    ===   =========
//     0    Red/Gray
//     1    Green
//     2    Blue
//     3    CIE L* and CIE Y
// ----------------------------------------------------------------------------

class STFSet : public MetaTable
{
public:

   STFSet( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type MinLength() const;
   virtual size_type MaxLength() const;
};

extern STFSet* TheSTFSetParameter;

// ----------------------------------------------------------------------------

class STFShadowsClipping : public MetaFloat
{
public:

   STFShadowsClipping( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern STFShadowsClipping* TheSTFShadowsClippingParameter;

// ----------------------------------------------------------------------------

class STFHighlightsClipping : public MetaFloat
{
public:

   STFHighlightsClipping( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern STFHighlightsClipping* TheSTFHighlightsClippingParameter;

// ----------------------------------------------------------------------------

class STFMidtonesBalance : public MetaFloat
{
public:

   STFMidtonesBalance( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern STFMidtonesBalance* TheSTFMidtonesBalanceParameter;

// ----------------------------------------------------------------------------

class STFLowRange : public MetaFloat
{
public:

   STFLowRange( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern STFLowRange* TheSTFLowRangeParameter;

// ----------------------------------------------------------------------------

class STFHighRange : public MetaFloat
{
public:

   STFHighRange( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern STFHighRange* TheSTFHighRangeParameter;

// ----------------------------------------------------------------------------

class STFInteraction : public MetaEnumeration
{
public:

   enum { SeparateChannels,
          Grayscale,
          NumberOfInteractionModes,
          Default = SeparateChannels };

   STFInteraction( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern STFInteraction* TheSTFInteractionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ScreenTransferFunctionParameters_h

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionParameters.h - Released 2016/02/21 20:22:43 UTC
