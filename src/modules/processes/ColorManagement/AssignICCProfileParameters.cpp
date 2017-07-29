//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0326
// ----------------------------------------------------------------------------
// AssignICCProfileParameters.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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

#include "AssignICCProfileParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

TargetProfile* TheTargetProfileParameter = 0;
AssignMode*    TheAssignModeParameter = 0;

// ----------------------------------------------------------------------------

TargetProfile::TargetProfile( MetaProcess* P ) : MetaString( P )
{
   TheTargetProfileParameter = this;
}

IsoString TargetProfile::Id() const
{
   return "targetProfile";
}

// ----------------------------------------------------------------------------

AssignMode::AssignMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheAssignModeParameter = this;
}

IsoString AssignMode::Id() const
{
   return "mode";
}

size_type AssignMode::NumberOfElements() const
{
   return NumberOfAssignModes;
}

IsoString AssignMode::ElementId( size_type id ) const
{
   switch ( id )
   {
   default:
   case AssignNewProfile:     return "AssignNewProfile";
   case AssignDefaultProfile: return "AssignDefaultProfile";
   case LeaveUntagged:        return "LeaveUntagged";
   }
}

int AssignMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type AssignMode::DefaultValueIndex() const
{
   return int( Default );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssignICCProfileParameters.cpp - Released 2017-07-18T16:14:18Z
