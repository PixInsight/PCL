//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 01.00.02.0305
// ----------------------------------------------------------------------------
// CloneStampParameters.h - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#ifndef __CloneStampParameters_h
#define __CloneStampParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CSClonerTable : public MetaTable
{
public:

   CSClonerTable( MetaProcess* );

   virtual IsoString Id() const;
};

extern CSClonerTable* TheCSClonerTableParameter;

// ----------------------------------------------------------------------------

class CSActionIndex : public MetaUInt32
{
public:

   CSActionIndex( MetaTable* );

   virtual IsoString Id() const;
};

extern CSActionIndex* TheCSActionIndexParameter;

// ----------------------------------------------------------------------------

class CSTargetX : public MetaInt32
{
public:

   CSTargetX( MetaTable* );

   virtual IsoString Id() const;
};

extern CSTargetX* TheCSTargetXParameter;

// ----------------------------------------------------------------------------

class CSTargetY : public MetaInt32
{
public:

   CSTargetY( MetaTable* );

   virtual IsoString Id() const;
};

extern CSTargetY* TheCSTargetYParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CSActionTable : public MetaTable
{
public:

   CSActionTable( MetaProcess* );

   virtual IsoString Id() const;
};

extern CSActionTable* TheCSActionTableParameter;

// ----------------------------------------------------------------------------

class CSSourceIdentifier : public MetaString
{
public:

   CSSourceIdentifier( MetaTable* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern CSSourceIdentifier* TheCSSourceIdentifierParameter;

// ----------------------------------------------------------------------------

class CSSourceWidth : public MetaInt32
{
public:

   CSSourceWidth( MetaTable* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
};

extern CSSourceWidth* TheCSSourceWidthParameter;

// ----------------------------------------------------------------------------

class CSSourceHeight : public MetaInt32
{
public:

   CSSourceHeight( MetaTable* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
};

extern CSSourceHeight* TheCSSourceHeightParameter;

// ----------------------------------------------------------------------------

class CSSourceX : public MetaInt32
{
public:

   CSSourceX( MetaTable* );

   virtual IsoString Id() const;
};

extern CSSourceX* TheCSSourceXParameter;

// ----------------------------------------------------------------------------

class CSSourceY : public MetaInt32
{
public:

   CSSourceY( MetaTable* );

   virtual IsoString Id() const;
};

extern CSSourceY* TheCSSourceYParameter;

// ----------------------------------------------------------------------------

class CSRadius : public MetaInt32
{
public:

   CSRadius( MetaTable* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CSRadius* TheCSRadiusParameter;

// ----------------------------------------------------------------------------

class CSSoftness : public MetaFloat
{
public:

   CSSoftness( MetaTable* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CSSoftness* TheCSSoftnessParameter;

// ----------------------------------------------------------------------------

class CSOpacity : public MetaFloat
{
public:

   CSOpacity( MetaTable* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CSOpacity* TheCSOpacityParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CSWidth : public MetaInt32
{
public:

   CSWidth( MetaProcess* );

   virtual IsoString Id() const;
};

extern CSWidth* TheCSWidthParameter;

// ----------------------------------------------------------------------------

class CSHeight : public MetaInt32
{
public:

   CSHeight( MetaProcess* );

   virtual IsoString Id() const;
};

extern CSHeight* TheCSHeightParameter;

// ----------------------------------------------------------------------------

class CSColor : public MetaUInt32
{
public:

   CSColor( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern CSColor* TheCSColorParameter;

// ----------------------------------------------------------------------------

class CSBoundsColor : public MetaUInt32
{
public:

   CSBoundsColor( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern CSBoundsColor* TheCSBoundsColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CloneStampParameters_h

// ----------------------------------------------------------------------------
// EOF CloneStampParameters.h - Released 2017-05-02T09:43:00Z
