//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0374
// ----------------------------------------------------------------------------
// ExponentialTransformationParameters.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ExponentialTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialFunctionType*      TheExponentialFunctionTypeParameter = 0;
ExponentialFunctionOrder*     TheExponentialFunctionOrderParameter = 0;
ExponentialFunctionSmoothing* TheExponentialFunctionSmoothingParameter = 0;
ExponentialFunctionMask*      TheExponentialFunctionMaskParameter = 0;

// ----------------------------------------------------------------------------

ExponentialFunctionType::ExponentialFunctionType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheExponentialFunctionTypeParameter = this;
}

IsoString ExponentialFunctionType::Id() const
{
   return "functionType";
}

size_type ExponentialFunctionType::NumberOfElements() const
{
   return NumberOfExponentialFunctions;
}

IsoString ExponentialFunctionType::ElementId( size_type i ) const
{
   switch ( i )
   {
   case PIP: return "PIP";
   case SMI: return "SMI";
   }

   return IsoString();
}

int ExponentialFunctionType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ExponentialFunctionType::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

ExponentialFunctionOrder::ExponentialFunctionOrder( MetaProcess* P ) : MetaFloat( P )
{
   TheExponentialFunctionOrderParameter = this;
}

IsoString ExponentialFunctionOrder::Id() const
{
   return "order";
}

int ExponentialFunctionOrder::Precision() const
{
   return 1;
}

double ExponentialFunctionOrder::DefaultValue() const
{
   return 1;
}

double ExponentialFunctionOrder::MinimumValue() const
{
   return 0.1;
}

double ExponentialFunctionOrder::MaximumValue() const
{
   return 6;
}

// ----------------------------------------------------------------------------

ExponentialFunctionSmoothing::ExponentialFunctionSmoothing( MetaProcess* P ) : MetaFloat( P )
{
   TheExponentialFunctionSmoothingParameter = this;
}

IsoString ExponentialFunctionSmoothing::Id() const
{
   return "sigma";
}

int ExponentialFunctionSmoothing::Precision() const
{
   return 2;
}

double ExponentialFunctionSmoothing::DefaultValue() const
{
   return 0;
}

double ExponentialFunctionSmoothing::MinimumValue() const
{
   return 0;
}

double ExponentialFunctionSmoothing::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

ExponentialFunctionMask::ExponentialFunctionMask( MetaProcess* P ) : MetaBoolean( P )
{
   TheExponentialFunctionMaskParameter = this;
}

IsoString ExponentialFunctionMask::Id() const
{
   return "useLightnessMask";
}

IsoString ExponentialFunctionMask::Aliases() const
{
   return "luminanceMask";
}

bool ExponentialFunctionMask::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationParameters.cpp - Released 2017-05-02T09:43:00Z
