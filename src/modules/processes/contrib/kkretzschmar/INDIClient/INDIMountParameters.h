//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0217
// ----------------------------------------------------------------------------
// INDIMountParameters.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#ifndef __INDIMountParameters_h
#define __INDIMountParameters_h

#include <pcl/MetaParameter.h>

#include "ModuleParameterRegistration.h"


namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

DECLARE_STRING_PARAMETER_CLASS(IMCDeviceName);

// ----------------------------------------------------------------------------

#define COMMAND_ENUM ENUM_ITEM(Unpark) ENUM_ITEM(Park)\
	            ENUM_ITEM(MoveNorthStart) ENUM_ITEM(MoveNorthStop)\
		        ENUM_ITEM(MoveSouthStart) ENUM_ITEM(MoveSouthStop)\
				ENUM_ITEM(MoveWestStart) ENUM_ITEM(MoveWestStop)\
				ENUM_ITEM(MoveEastStart) ENUM_ITEM(MoveEastStop)\
				ENUM_ITEM(GoTo) ENUM_ITEM(Sync) ENUM_ITEM(ParkDefault)\
				ENUM_ITEM(FitPointingModel) ENUM_ITEM(TestSync)



DECLARE_ENUM_PARAMETER(IMCCommand,COMMAND_ENUM,Unpark);

// ----------------------------------------------------------------------------

#define SLEW_RATE_ENUM ENUM_ITEM(Guide) ENUM_ITEM(Centering)\
	            ENUM_ITEM(Find) ENUM_ITEM(Max)

DECLARE_ENUM_PARAMETER(IMCSlewRate,SLEW_RATE_ENUM,Max);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_PARAMETER_CLASS(IMCTargetRA);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_PARAMETER_CLASS(IMCTargetDec);

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_PARAMETER_CLASS(IMCComputeApparentPosition);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCCurrentLST);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCCurrentRA);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCCurrentDec);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCApparentTargetRA);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCApparentTargetDec);

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_PARAMETER_CLASS(IMCGeographicLatitude);

// ----------------------------------------------------------------------------

#define PIERSIDE_ENUM  ENUM_ITEM(West) ENUM_ITEM(East) ENUM_ITEM(None)
DECLARE_ENUM_PARAMETER(IMCPierSide,PIERSIDE_ENUM,None);

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncLST);

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncCelestialRA);

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncCelestialDec);

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncTelescopeRA);

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncTelescopeDec);

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_PARAMETER_CLASS(IMCEnableAlignmentCorrection);

#define ALIGNMENT_ENUM ENUM_ITEM(None) ENUM_ITEM(AnalyticalModel) ENUM_ITEM(ServerModel)
DECLARE_ENUM_PARAMETER(IMCAlignmentMethod,ALIGNMENT_ENUM, None);

DECLARE_INT32_PARAMETER_CLASS(IMCAlignmentConfig);

// ----------------------------------------------------------------------------

DECLARE_STRING_PARAMETER_CLASS(IMCAlignmentFile);

// ----------------------------------------------------------------------------



PCL_END_LOCAL

} // pcl

#endif   // __INDIMountParameters_h

// ----------------------------------------------------------------------------
// EOF INDIMountParameters.h - Released 2017-08-01T14:26:58Z
