//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0402
// ----------------------------------------------------------------------------
// NewImageParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "NewImageParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NewImageIdentifier*        TheNewImageIdentifierParameter = 0;
NewImageWidth*             TheNewImageWidthParameter = 0;
NewImageHeight*            TheNewImageHeightParameter = 0;
NewImageNumberOfChannels*  TheNewImageNumberOfChannelsParameter = 0;
NewImageColorSpace*        TheNewImageColorSpaceParameter = 0;
NewImageSampleFormat*      TheNewImageSampleFormatParameter = 0;
NewImageV0*                TheNewImageV0Parameter = 0;
NewImageV1*                TheNewImageV1Parameter = 0;
NewImageV2*                TheNewImageV2Parameter = 0;
NewImageVA*                TheNewImageVAParameter = 0;

// ----------------------------------------------------------------------------

NewImageIdentifier::NewImageIdentifier( MetaProcess* p ) : MetaString( p )
{
   TheNewImageIdentifierParameter = this;
}

IsoString NewImageIdentifier::Id() const
{
   return "id";
}

size_type NewImageIdentifier::MinLength() const
{
   return 0; // can be void
}

String NewImageIdentifier::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

NewImageWidth::NewImageWidth( MetaProcess* p ) : MetaInt32( p )
{
   TheNewImageWidthParameter = this;
}

IsoString NewImageWidth::Id() const
{
   return "width";
}

double NewImageWidth::DefaultValue() const
{
   return 256;
}

double NewImageWidth::MinimumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

NewImageHeight::NewImageHeight( MetaProcess* p ) : MetaInt32( p )
{
   TheNewImageHeightParameter = this;
}

IsoString NewImageHeight::Id() const
{
   return "height";
}

double NewImageHeight::DefaultValue() const
{
   return 256;
}

double NewImageHeight::MinimumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

NewImageNumberOfChannels::NewImageNumberOfChannels( MetaProcess* p ) : MetaInt32( p )
{
   TheNewImageNumberOfChannelsParameter = this;
}

IsoString NewImageNumberOfChannels::Id() const
{
   return "numberOfChannels";
}

double NewImageNumberOfChannels::DefaultValue() const
{
   return 3;
}

double NewImageNumberOfChannels::MinimumValue() const
{
   return 1;
}

double NewImageNumberOfChannels::MaximumValue() const
{
   return 256;
}

// ----------------------------------------------------------------------------

NewImageColorSpace::NewImageColorSpace( MetaProcess* p ) : MetaEnumeration( p )
{
   TheNewImageColorSpaceParameter = this;
}

IsoString NewImageColorSpace::Id() const
{
   return "colorSpace";
}

size_type NewImageColorSpace::NumberOfElements() const
{
   return 2;
}

IsoString NewImageColorSpace::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RGB:   return "RGB";
   case Gray:  return "Grayscale";
   }
}

int NewImageColorSpace::ElementValue( size_type i ) const
{
   return int( i );
}

size_type NewImageColorSpace::DefaultValueIndex() const
{
   return size_type( RGB );
}

// ----------------------------------------------------------------------------

NewImageSampleFormat::NewImageSampleFormat( MetaProcess* p ) : MetaEnumeration( p )
{
   TheNewImageSampleFormatParameter = this;
}

IsoString NewImageSampleFormat::Id() const
{
   return "sampleFormat";
}

size_type NewImageSampleFormat::NumberOfElements() const
{
   return 5;
}

IsoString NewImageSampleFormat::ElementId( size_type i ) const
{
   switch ( i )
   {
   case I8:    return "i8";
   case I16:   return "i16";
   case I32:   return "i32";
   default:
   case F32:   return "f32";
   case F64:   return "f64";
   }
}

int NewImageSampleFormat::ElementValue( size_type i ) const
{
   return int( i );
}

size_type NewImageSampleFormat::DefaultValueIndex() const
{
   return size_type( F32 );
}

// ----------------------------------------------------------------------------

NewImageV0::NewImageV0( MetaProcess* p ) : MetaDouble( p )
{
   TheNewImageV0Parameter = this;
}

IsoString NewImageV0::Id() const
{
   return "v0";
}

int NewImageV0::Precision() const
{
   return 8;
}

double NewImageV0::DefaultValue() const
{
   return 0;
}

double NewImageV0::MinimumValue() const
{
   return 0;
}

double NewImageV0::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

NewImageV1::NewImageV1( MetaProcess* p ) : MetaDouble( p )
{
   TheNewImageV1Parameter = this;
}

IsoString NewImageV1::Id() const
{
   return "v1";
}

int NewImageV1::Precision() const
{
   return 8;
}

double NewImageV1::DefaultValue() const
{
   return 0;
}

double NewImageV1::MinimumValue() const
{
   return 0;
}

double NewImageV1::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

NewImageV2::NewImageV2( MetaProcess* p ) : MetaDouble( p )
{
   TheNewImageV2Parameter = this;
}

IsoString NewImageV2::Id() const
{
   return "v2";
}

int NewImageV2::Precision() const
{
   return 8;
}

double NewImageV2::DefaultValue() const
{
   return 0;
}

double NewImageV2::MinimumValue() const
{
   return 0;
}

double NewImageV2::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

NewImageVA::NewImageVA( MetaProcess* p ) : MetaDouble( p )
{
   TheNewImageVAParameter = this;
}

IsoString NewImageVA::Id() const
{
   return "va";
}

int NewImageVA::Precision() const
{
   return 8;
}

double NewImageVA::DefaultValue() const
{
   return 1;
}

double NewImageVA::MinimumValue() const
{
   return 0;
}

double NewImageVA::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NewImageParameters.cpp - Released 2017-08-01T14:26:58Z
