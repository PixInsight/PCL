//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0318
// ----------------------------------------------------------------------------
// ReadoutOptionsParameters.h - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ReadoutOptionsParameters_h
#define __ReadoutOptionsParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ReadoutOptionsData : public MetaEnumeration
{
public:

   // This enumeration corresponds to pcl::ReadoutData (see pcl/ReadoutOptions.h)

   ReadoutOptionsData( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ReadoutOptionsData* TheReadoutOptionsDataParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsMode : public MetaEnumeration
{
public:

   // This enumeration corresponds to pcl::ReadoutMode (see pcl/ReadoutOptions.h)

   ReadoutOptionsMode( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ReadoutOptionsMode* TheReadoutOptionsModeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsProbeSize : public MetaUInt8
{
public:

   ReadoutOptionsProbeSize( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool NeedsValidation() const; // to ensure an odd value

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ReadoutOptionsProbeSize* TheReadoutOptionsProbeSizeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsPreviewSize : public MetaUInt8
{
public:

   ReadoutOptionsPreviewSize( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool NeedsValidation() const; // to ensure an odd value

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ReadoutOptionsPreviewSize* TheReadoutOptionsPreviewSizeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsPreviewZoomFactor : public MetaUInt8
{
public:

   ReadoutOptionsPreviewZoomFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ReadoutOptionsPreviewZoomFactor* TheReadoutOptionsPreviewZoomFactorParameter;

// ----------------------------------------------------------------------------

class ReadoutRealPrecision : public MetaUInt8
{
public:

   ReadoutRealPrecision( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ReadoutRealPrecision* TheReadoutRealPrecisionParameter;

// ----------------------------------------------------------------------------

class ReadoutIntegerRange : public MetaUInt32
{
public:

   ReadoutIntegerRange( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ReadoutIntegerRange* TheReadoutIntegerRangeParameter;

// ----------------------------------------------------------------------------

class ReadoutAlpha : public MetaBoolean
{
public:

   ReadoutAlpha( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ReadoutAlpha* TheReadoutAlphaParameter;

// ----------------------------------------------------------------------------

class ReadoutMask : public MetaBoolean
{
public:

   ReadoutMask( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ReadoutMask* TheReadoutMaskParameter;

// ----------------------------------------------------------------------------

class ReadoutPreview : public MetaBoolean
{
public:

   ReadoutPreview( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ReadoutPreview* TheReadoutPreviewParameter;

// ----------------------------------------------------------------------------

class ReadoutPreviewCenter : public MetaBoolean
{
public:

   ReadoutPreviewCenter( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ReadoutPreviewCenter* TheReadoutPreviewCenterParameter;

// ----------------------------------------------------------------------------

class ReadoutBroadcast : public MetaBoolean
{
public:

   ReadoutBroadcast( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ReadoutBroadcast* TheReadoutBroadcastParameter;

// ----------------------------------------------------------------------------

class ReadoutReal : public MetaBoolean
{
public:

   ReadoutReal( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ReadoutReal* TheReadoutRealParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ReadoutOptionsParameters_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsParameters.h - Released 2015/11/26 16:00:12 UTC
