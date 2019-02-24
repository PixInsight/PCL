//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 01.00.04.0266
// ----------------------------------------------------------------------------
// InverseFourierTransformParameters.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
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

#include "InverseFourierTransformParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IFTIdOfFirstComponent*  TheIFTIdOfFirstComponentParameter = 0;
IFTIdOfSecondComponent* TheIFTIdOfSecondComponentParameter = 0;
IFTOnOutOfRangeResult*  TheIFTOnOutOfRangeResultParameter = 0;

// ----------------------------------------------------------------------------

IFTIdOfFirstComponent::IFTIdOfFirstComponent( MetaProcess* p ) : MetaString( p )
{
   TheIFTIdOfFirstComponentParameter = this;
}

IsoString IFTIdOfFirstComponent::Id() const
{
   return "idOfFirstComponent";
}

size_type IFTIdOfFirstComponent::MinLength() const
{
   return 0; // can be void
}

String IFTIdOfFirstComponent::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

IFTIdOfSecondComponent::IFTIdOfSecondComponent( MetaProcess* p ) : MetaString( p )
{
   TheIFTIdOfSecondComponentParameter = this;
}

IsoString IFTIdOfSecondComponent::Id() const
{
   return "idOfSecondComponent";
}

size_type IFTIdOfSecondComponent::MinLength() const
{
   return 0; // can be void
}

String IFTIdOfSecondComponent::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

IFTOnOutOfRangeResult::IFTOnOutOfRangeResult( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIFTOnOutOfRangeResultParameter = this;
}

IsoString IFTOnOutOfRangeResult::Id() const
{
   return "onOutOfRangeResult";
}

size_type IFTOnOutOfRangeResult::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString IFTOnOutOfRangeResult::ElementId( size_type i ) const
{
   switch ( i )
   {
   case DontCare: return "DontCare";
   default:
   case Truncate: return "Truncate";
   case Rescale:  return "Rescale";
   }
}

int IFTOnOutOfRangeResult::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IFTOnOutOfRangeResult::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformParameters.cpp - Released 2019-01-21T12:06:41Z
