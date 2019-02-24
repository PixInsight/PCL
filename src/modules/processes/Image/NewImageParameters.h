//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0437
// ----------------------------------------------------------------------------
// NewImageParameters.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __NewImageParameters_h
#define __NewImageParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class NewImageIdentifier : public MetaString
{
public:

   NewImageIdentifier( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern NewImageIdentifier* TheNewImageIdentifierParameter;

// ----------------------------------------------------------------------------

class NewImageWidth : public MetaInt32
{
public:

   NewImageWidth( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern NewImageWidth* TheNewImageWidthParameter;

// ----------------------------------------------------------------------------

class NewImageHeight : public MetaInt32
{
public:

   NewImageHeight( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern NewImageHeight* TheNewImageHeightParameter;

// ----------------------------------------------------------------------------

class NewImageNumberOfChannels : public MetaInt32
{
public:

   NewImageNumberOfChannels( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern NewImageNumberOfChannels* TheNewImageNumberOfChannelsParameter;

// ----------------------------------------------------------------------------

class NewImageColorSpace : public MetaEnumeration
{
public:

   enum { RGB,     // RGB Color
          Gray };  // Grayscale

   NewImageColorSpace( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern NewImageColorSpace* TheNewImageColorSpaceParameter;

// ----------------------------------------------------------------------------

class NewImageSampleFormat : public MetaEnumeration
{
public:

   enum { I8,     // unsigned 8-bit integer
          I16,    // unsigned 16-bit integer
          I32,    // unsigned 32-bit integer
          F32,    // IEEE 32-bit floating point (simple precision)
          F64 };  // IEEE 64-bit floating point (double precision)

   NewImageSampleFormat( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern NewImageSampleFormat* TheNewImageSampleFormatParameter;

// ----------------------------------------------------------------------------

class NewImageV0 : public MetaDouble
{
public:

   NewImageV0( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern NewImageV0* TheNewImageV0Parameter;

// ----------------------------------------------------------------------------

class NewImageV1 : public MetaDouble
{
public:

   NewImageV1( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern NewImageV1* TheNewImageV1Parameter;

// ----------------------------------------------------------------------------

class NewImageV2 : public MetaDouble
{
public:

   NewImageV2( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern NewImageV2* TheNewImageV2Parameter;

// ----------------------------------------------------------------------------

class NewImageVA : public MetaDouble
{
public:

   NewImageVA( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern NewImageVA* TheNewImageVAParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __NewImageParameters_h

// ----------------------------------------------------------------------------
// EOF NewImageParameters.h - Released 2019-01-21T12:06:41Z
