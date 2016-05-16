//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.04.0108
// ----------------------------------------------------------------------------
// INDICCDFrameParameters.h - Released 2016/04/15 15:37:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

namespace pcl {

IMCDeviceName*                 TheIMCDeviceNameParameter             = nullptr;
IMCCommandType*                TheIMCCommandTypeParameter            = nullptr;
IMCTargetRightascension*       TheIMCTargetRightAscensionParameter   = nullptr;
IMCTargetDeclination*          TheIMCTargetDeclinationParameter      = nullptr;
IMCCurrentRightascension*      TheIMCCurrentRightAscensionParameter  = nullptr;
IMCCurrentDeclination*         TheIMCCurrentDeclinationParameter     = nullptr;

// ----------------------------------------------------------------------------

IMCDeviceName::IMCDeviceName( MetaProcess* P ) : MetaString( P )
{
   TheIMCDeviceNameParameter = this;
}

IsoString IMCDeviceName::Id() const
{
   return "deviceName";
}

// ----------------------------------------------------------------------------

IMCCommandType::IMCCommandType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIMCCommandTypeParameter = this;
}

IsoString IMCCommandType::Id() const
{
   return "commandType";
}

size_type IMCCommandType::NumberOfElements() const
{
   return NumberOfCommands;
}

IsoString IMCCommandType::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Goto:          return "CommandType_Goto";
   case Synch:         return "CommandType_Synch";
   case Park:          return "CommandType_Park";
   }
}

int IMCCommandType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IMCCommandType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IMCTargetRightascension::IMCTargetRightascension( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCTargetRightAscensionParameter = this;
}

IsoString IMCTargetRightascension::Id() const
{
   return "targetRightascension";
}

int IMCTargetRightascension::Precision() const
{
   return 6;
}

double IMCTargetRightascension::DefaultValue() const
{
   return 0;
}

double IMCTargetRightascension::MinimumValue() const
{
   return 0;
}

double IMCTargetRightascension::MaximumValue() const
{
   return 23.999999;
}

// ----------------------------------------------------------------------------

IMCTargetDeclination::IMCTargetDeclination( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCTargetDeclinationParameter = this;
}

IsoString IMCTargetDeclination::Id() const
{
   return "targetDeclination";
}

int IMCTargetDeclination::Precision() const
{
   return 6;
}

double IMCTargetDeclination::DefaultValue() const
{
   return 0;
}

double IMCTargetDeclination::MinimumValue() const
{
   return 0;
}

double IMCTargetDeclination::MaximumValue() const
{
   return 23.999999;
}


// ----------------------------------------------------------------------------

IMCCurrentRightascension::IMCCurrentRightascension( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCCurrentRightAscensionParameter = this;
}

IsoString IMCCurrentRightascension::Id() const
{
   return "currentRightascension";
}

int IMCCurrentRightascension::Precision() const
{
   return 6;
}

double IMCCurrentRightascension::DefaultValue() const
{
   return 0;
}

double IMCCurrentRightascension::MinimumValue() const
{
   return 0;
}

double IMCCurrentRightascension::MaximumValue() const
{
   return 23.999999;
}

bool IMCCurrentRightascension::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IMCCurrentDeclination::IMCCurrentDeclination( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCCurrentDeclinationParameter = this;
}

IsoString IMCCurrentDeclination::Id() const
{
   return "currentDeclination";
}

int IMCCurrentDeclination::Precision() const
{
   return 6;
}

double IMCCurrentDeclination::DefaultValue() const
{
   return 0;
}

double IMCCurrentDeclination::MinimumValue() const
{
   return 0;
}

double IMCCurrentDeclination::MaximumValue() const
{
   return 23.999999;
}

bool IMCCurrentDeclination::IsReadOnly() const
{
   return true;
}






}


