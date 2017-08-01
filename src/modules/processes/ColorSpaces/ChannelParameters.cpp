//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0348
// ----------------------------------------------------------------------------
// ChannelParameters.cpp - Released 2017-08-01T14:26:57Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "ChannelParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSpaceId::ColorSpaceId( MetaProcess* P ) : MetaEnumeration( P )
{
}

IsoString ColorSpaceId::Id() const
{
   return "colorSpace";
}

size_type ColorSpaceId::NumberOfElements() const
{
   return NumberOfColorSpaces;
}

IsoString ColorSpaceId::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RGB:      return "RGB";
   case CIEXYZ:   return "CIEXYZ";
   case CIELab:   return "CIELab";
   case CIELch:   return "CIELch";
   case HSV:      return "HSV";
   case HSI:      return "HSI";
   }
}

int ColorSpaceId::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ColorSpaceId::DefaultValueIndex() const
{
   return Default;
}

IsoString ColorSpaceId::SpaceId( int cs )
{
   switch ( cs )
   {
   default:
   case RGB:      return "RGB";
   case CIEXYZ:   return "CIE XYZ";
   case CIELab:   return "CIE L*a*b*";
   case CIELch:   return "CIE L*c*h*";
   case HSV:      return "HSV";
   case HSI:      return "HSI";
   }
}

IsoString ColorSpaceId::ChannelId( int cs, int c )
{
   switch ( cs )
   {
   default:
   case RGB:      return (c == 0) ? "R"  : ((c == 1) ? "G"  : "B");
   case CIEXYZ:   return (c == 0) ? "X"  : ((c == 1) ? "Y"  : "Z");
   case CIELab:   return (c == 0) ? "L"  : ((c == 1) ? "a"  : "b");
   case CIELch:   return (c == 0) ? "L"  : ((c == 1) ? "c"  : "h");
   case HSV:      return (c == 0) ? "H"  : ((c == 1) ? "Sv" : "V");
   case HSI:      return (c == 0) ? "H"  : ((c == 1) ? "Si" : "I");
   }
}

// ----------------------------------------------------------------------------

ChannelTable::ChannelTable( MetaProcess* P ) : MetaTable( P )
{
}

IsoString ChannelTable::Id() const
{
   return "channels";
}

size_type ChannelTable::MinLength() const
{
   return 3;
}

size_type ChannelTable::MaxLength() const
{
   return 3;
}

// ----------------------------------------------------------------------------

ChannelEnabled::ChannelEnabled( MetaTable* T ) : MetaBoolean( T )
{
}

IsoString ChannelEnabled::Id() const
{
   return "enabled";
}

bool ChannelEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ChannelId::ChannelId( MetaTable* T ) : MetaString( T )
{
}

IsoString ChannelId::Id() const
{
   return "id";
}

size_type ChannelId::MinLength() const
{
   return 0; // can be void
}

String ChannelId::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

ChannelSampleFormat::ChannelSampleFormat( MetaProcess* p ) : MetaEnumeration( p )
{
}

IsoString ChannelSampleFormat::Id() const
{
   return "sampleFormat";
}

size_type ChannelSampleFormat::NumberOfElements() const
{
   return NumberOfSampleFormats;
}

IsoString ChannelSampleFormat::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case SameAsSource:   return "SameAsSource";
   case I8:             return "i8";
   case I16:            return "i16";
   case I32:            return "i32";
   case F32:            return "f32";
   case F64:            return "f64";
   }
}

int ChannelSampleFormat::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ChannelSampleFormat::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelParameters.cpp - Released 2017-08-01T14:26:57Z
