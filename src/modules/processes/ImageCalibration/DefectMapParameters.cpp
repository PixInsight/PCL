//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.03.05.0272
// ----------------------------------------------------------------------------
// DefectMapParameters.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "DefectMapParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DMPMapId*     TheDMPMapIdParameter = 0;
DMPOperation* TheDMPOperationParameter = 0;
DMPStructure* TheDMPStructureParameter = 0;
DMPIsCFA*     TheDMPIsCFAParameter = 0;

// ----------------------------------------------------------------------------

DMPMapId::DMPMapId( MetaProcess* p ) : MetaString( p )
{
   TheDMPMapIdParameter = this;
}

IsoString DMPMapId::Id() const
{
   return "mapId";
}

size_type DMPMapId::MinLength() const
{
   return 0; // can be empty
}

// ----------------------------------------------------------------------------

DMPOperation::DMPOperation( MetaProcess* p ) : MetaEnumeration( p )
{
   TheDMPOperationParameter = this;
}

IsoString DMPOperation::Id() const
{
   return "operation";
}

size_type DMPOperation::NumberOfElements() const
{
   return NumberOfOperations;
}

IsoString DMPOperation::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Mean:     return "Mean";
   case Gaussian: return "Gaussian";
   case Minimum:  return "Minimum";
   case Maximum:  return "Maximum";
   default:
   case Median:   return "Median";
   }
}

int DMPOperation::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DMPOperation::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DMPStructure::DMPStructure( MetaProcess* p ) : MetaEnumeration( p )
{
   TheDMPStructureParameter = this;
}

IsoString DMPStructure::Id() const
{
   return "structure";
}

size_type DMPStructure::NumberOfElements() const
{
   return NumberOfStructures;
}

IsoString DMPStructure::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Square:     return "Square";
   case Circular:   return "Circular";
   case Horizontal: return "Horizontal";
   case Vertical:   return "Vertical";
   }
}

int DMPStructure::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DMPStructure::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DMPIsCFA::DMPIsCFA( MetaProcess* P ) : MetaBoolean( P )
{
   TheDMPIsCFAParameter = this;
}

IsoString DMPIsCFA::Id() const
{
   return "isCFA";
}

bool DMPIsCFA::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DefectMapParameters.cpp - Released 2016/02/21 20:22:43 UTC
