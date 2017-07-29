//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0199
// ----------------------------------------------------------------------------
// INDIMountParameters.cpp - Released 2016/06/20 17:47:31 UTC
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

namespace pcl
{

// ----------------------------------------------------------------------------

IMCDeviceName*              TheIMCDeviceNameParameter = nullptr;
IMCCommand*                 TheIMCCommandParameter = nullptr;
IMCSlewRate*                TheIMCSlewRateParameter = nullptr;
IMCTargetRA*                TheIMCTargetRAParameter = nullptr;
IMCTargetDec*               TheIMCTargetDecParameter = nullptr;
IMCComputeApparentPosition* TheIMCComputeApparentPositionParameter = nullptr;
IMCCurrentLST*              TheIMCCurrentLSTParameter = nullptr;
IMCCurrentRA*               TheIMCCurrentRAParameter = nullptr;
IMCCurrentDec*              TheIMCCurrentDecParameter = nullptr;
IMCApparentTargetRA*        TheIMCApparentTargetRAParameter = nullptr;
IMCApparentTargetDec*       TheIMCApparentTargetDecParameter = nullptr;

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

IMCCommand::IMCCommand( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIMCCommandParameter = this;
}

IsoString IMCCommand::Id() const
{
   return "command";
}

size_type IMCCommand::NumberOfElements() const
{
   return NumberOfCommands;
}

IsoString IMCCommand::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Unpark:         return "Command_Unpark";
   case Park:           return "Command_Park";
   case MoveNorthStart: return "Command_MoveNorth_Start";
   case MoveNorthStop:  return "Command_MoveNorth_Stop";
   case MoveSouthStart: return "Command_MoveSouth_Start";
   case MoveSouthStop:  return "Command_MoveSouth_Stop";
   case MoveWestStart:  return "Command_MoveWest_Start";
   case MoveWestStop:   return "Command_MoveWest_Stop";
   case MoveEastStart:  return "Command_MoveEast_Start";
   case MoveEastStop:   return "Command_MoveEast_Stop";
   case GoTo:           return "Command_GoTo";
   case Sync:           return "Command_Sync";
   case ParkDefault:    return "Command_ParkDefault";
   }
}

int IMCCommand::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IMCCommand::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IMCSlewRate::IMCSlewRate( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIMCSlewRateParameter = this;
}

IsoString IMCSlewRate::Id() const
{
   return "slewRate";
}

size_type IMCSlewRate::NumberOfElements() const
{
   return NumberOfCommands;
}

IsoString IMCSlewRate::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Guide:     return "SlewRate_Guide";
   case Centering: return "SlewRate_Centering";
   case Find:      return "SlewRate_Find";
   case Max:       return "SlewRate_Max";
   }
}

int IMCSlewRate::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IMCSlewRate::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

IMCTargetRA::IMCTargetRA( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCTargetRAParameter = this;
}

IsoString IMCTargetRA::Id() const
{
   return "targetRA";
}

int IMCTargetRA::Precision() const
{
   return 8;
}

double IMCTargetRA::DefaultValue() const
{
   return 0;
}

double IMCTargetRA::MinimumValue() const
{
   return 0;
}

double IMCTargetRA::MaximumValue() const
{
   return 24;
}

// ----------------------------------------------------------------------------

IMCTargetDec::IMCTargetDec( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCTargetDecParameter = this;
}

IsoString IMCTargetDec::Id() const
{
   return "targetDec";
}

int IMCTargetDec::Precision() const
{
   return 7;
}

double IMCTargetDec::DefaultValue() const
{
   return 0;
}

double IMCTargetDec::MinimumValue() const
{
   return -90;
}

double IMCTargetDec::MaximumValue() const
{
   return +90;
}

// ----------------------------------------------------------------------------

IMCComputeApparentPosition::IMCComputeApparentPosition( MetaProcess* P ) : MetaBoolean( P )
{
   TheIMCComputeApparentPositionParameter = this;
}

IsoString IMCComputeApparentPosition::Id() const
{
   return "computeApparentPosition";
}

bool IMCComputeApparentPosition::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

IMCCurrentLST::IMCCurrentLST( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCCurrentLSTParameter = this;
}

IsoString IMCCurrentLST::Id() const
{
   return "currentLST";
}

int IMCCurrentLST::Precision() const
{
   return 8;
}

double IMCCurrentLST::DefaultValue() const
{
   return -1; // < 0 -> unknown LST
}

double IMCCurrentLST::MinimumValue() const
{
   return -1; // < 0 -> unknown LST
}

double IMCCurrentLST::MaximumValue() const
{
   return 24;
}

bool IMCCurrentLST::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IMCCurrentRA::IMCCurrentRA( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCCurrentRAParameter = this;
}

IsoString IMCCurrentRA::Id() const
{
   return "currentRA";
}

int IMCCurrentRA::Precision() const
{
   return 8;
}

double IMCCurrentRA::DefaultValue() const
{
   return -1; // < 0 -> unknown RA
}

double IMCCurrentRA::MinimumValue() const
{
   return -1; // < 0 -> unknown RA
}

double IMCCurrentRA::MaximumValue() const
{
   return 24;
}

bool IMCCurrentRA::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IMCCurrentDec::IMCCurrentDec( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCCurrentDecParameter = this;
}

IsoString IMCCurrentDec::Id() const
{
   return "currentDec";
}

int IMCCurrentDec::Precision() const
{
   return 7;
}

double IMCCurrentDec::DefaultValue() const
{
   return -91; // < -90 -> unknown Dec
}

double IMCCurrentDec::MinimumValue() const
{
   return -91; // < -90 -> unknown Dec
}

double IMCCurrentDec::MaximumValue() const
{
   return +90;
}

bool IMCCurrentDec::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IMCApparentTargetRA::IMCApparentTargetRA( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCApparentTargetRAParameter = this;
}

IsoString IMCApparentTargetRA::Id() const
{
   return "apparentTargetRA";
}

int IMCApparentTargetRA::Precision() const
{
   return 8;
}

double IMCApparentTargetRA::DefaultValue() const
{
   return -1; // < 0 -> unknown RA
}

double IMCApparentTargetRA::MinimumValue() const
{
   return -1; // < 0 -> unknown RA
}

double IMCApparentTargetRA::MaximumValue() const
{
   return 24;
}

bool IMCApparentTargetRA::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

IMCApparentTargetDec::IMCApparentTargetDec( MetaProcess* P ) : MetaDouble( P )
{
   TheIMCApparentTargetDecParameter = this;
}

IsoString IMCApparentTargetDec::Id() const
{
   return "apparentTargetDec";
}

int IMCApparentTargetDec::Precision() const
{
   return 7;
}

double IMCApparentTargetDec::DefaultValue() const
{
   return -91; // < -90 -> unknown Dec
}

double IMCApparentTargetDec::MinimumValue() const
{
   return -91; // < -90 -> unknown Dec
}

double IMCApparentTargetDec::MaximumValue() const
{
   return +90;
}

bool IMCApparentTargetDec::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountParameters.cpp - Released 2016/06/20 17:47:31 UTC
