//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0225
// ----------------------------------------------------------------------------
// INDIMountParameters.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

#include "INDIMountParameters.h"



namespace pcl
{

// ----------------------------------------------------------------------------


REGISTER_MODULE_PARAMETER(IMCDeviceName);
REGISTER_MODULE_PARAMETER(IMCCommand);
REGISTER_MODULE_PARAMETER(IMCSlewRate);
REGISTER_MODULE_PARAMETER(IMCTargetRA);
REGISTER_MODULE_PARAMETER(IMCTargetDec);
REGISTER_MODULE_PARAMETER(IMCComputeApparentPosition);
REGISTER_MODULE_PARAMETER(IMCCurrentLST);
REGISTER_MODULE_PARAMETER(IMCCurrentRA);
REGISTER_MODULE_PARAMETER(IMCCurrentDec);
REGISTER_MODULE_PARAMETER(IMCApparentTargetRA);
REGISTER_MODULE_PARAMETER(IMCApparentTargetDec);
REGISTER_MODULE_PARAMETER(IMCGeographicLatitude);
REGISTER_MODULE_PARAMETER(IMCPierSide);
REGISTER_MODULE_PARAMETER(IMCSyncLST);
REGISTER_MODULE_PARAMETER(IMCSyncCelestialRA);
REGISTER_MODULE_PARAMETER(IMCSyncCelestialDec);
REGISTER_MODULE_PARAMETER(IMCSyncTelescopeRA);
REGISTER_MODULE_PARAMETER(IMCSyncTelescopeDec);
REGISTER_MODULE_PARAMETER(IMCAlignmentMethod);
REGISTER_MODULE_PARAMETER(IMCEnableAlignmentCorrection);
REGISTER_MODULE_PARAMETER(IMCAlignmentFile);
REGISTER_MODULE_PARAMETER(IMCAlignmentConfig);


// ----------------------------------------------------------------------------

DEFINE_STRING_PARAMETER_CLASS(IMCDeviceName,"deviceName");

// ----------------------------------------------------------------------------
#undef ENUM_ITEM
#define ENUM_ITEM(X) case X: return "Command_" #X;
DEFINE_ENUM_PARAMETER_CLASS(IMCCommand,"Command",COMMAND_ENUM, Unpark);

// ----------------------------------------------------------------------------
#undef ENUM_ITEM
#define ENUM_ITEM(X) case X: return "SlewRate_" #X;
DEFINE_ENUM_PARAMETER_CLASS(IMCSlewRate,"SlewRate",SLEW_RATE_ENUM, Max);


// ----------------------------------------------------------------------------

DEFINE_DOUBLE_PARAMETER_CLASS(IMCTargetRA,"targetRA",8,0,0,24);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_PARAMETER_CLASS(IMCTargetDec,"targetDec",7,0,-90,90);

// ----------------------------------------------------------------------------

DEFINE_BOOLEAN_PARAMETER_CLASS(IMCComputeApparentPosition,"computeApparentPosition",false);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCCurrentLST,"currentLST",8,0,0,24);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCCurrentRA,"currentRA",8,-1,-1,24);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCCurrentDec,"currentDec",7,-91,-91,90);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCApparentTargetRA,"apparentTargetRA",8,-1,-1,24);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCApparentTargetDec,"apparentTargetDec",7,-91,-91,90);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_PARAMETER_CLASS(IMCGeographicLatitude,"geographicLatitude",7,-91,-91,90);

// ----------------------------------------------------------------------------

#undef ENUM_ITEM
#define ENUM_ITEM(X) case X: return "PierSide_" #X;
DEFINE_ENUM_PARAMETER_CLASS(IMCPierSide,"PierSide",PIERSIDE_ENUM, None);

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncLST,"syncLST",8,0,0,24);
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncCelestialRA,"syncCelestialRA",8,0,0,24);
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncCelestialDec,"syncCelestialDec",7,0,-90,90);
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncTelescopeRA,"syncTelescopeRA",8,0,0,24);
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(IMCSyncTelescopeDec,"syncTelescopeDec",7,0,-90,90);


// ----------------------------------------------------------------------------

DEFINE_BOOLEAN_PARAMETER_CLASS(IMCEnableAlignmentCorrection,"enableAlignmentCorrection",false);

#undef ENUM_ITEM
#define ENUM_ITEM(X) case X: return "AlignmentMethod_" #X;
DEFINE_ENUM_PARAMETER_CLASS(IMCAlignmentMethod,"AlignmentMethod",ALIGNMENT_ENUM, None);

DEFINE_STRING_PARAMETER_CLASS(IMCAlignmentFile,"alignmentModelFile");

DEFINE_UINT32_PARAMETER_CLASS(IMCAlignmentConfig,"alignmentConfig",0,1,2047);

// ----------------------------------------------------------------------------


} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountParameters.cpp - Released 2018-11-01T11:07:21Z
