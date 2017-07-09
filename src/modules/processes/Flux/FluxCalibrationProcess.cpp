//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0173
// ----------------------------------------------------------------------------
// FluxCalibrationProcess.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#include "FluxCalibrationProcess.h"
#include "FluxCalibrationParameters.h"
#include "FluxCalibrationInstance.h"
#include "FluxCalibrationInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "FluxCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

FluxCalibrationProcess* TheFluxCalibrationProcess = 0;

// ----------------------------------------------------------------------------

FluxCalibrationProcess::FluxCalibrationProcess() : MetaProcess()
{
   TheFluxCalibrationProcess = this;

   // Instantiate process parameters

   new FCWavelengthValue( this );
   new FCWavelengthMode( this );
   new FCWavelengthKeyword( this );

   new FCTransmissivityValue( this );
   new FCTransmissivityMode( this );
   new FCTransmissivityKeyword( this );

   new FCFilterWidthValue( this );
   new FCFilterWidthMode( this );
   new FCFilterWidthKeyword( this );

   new FCApertureValue( this );
   new FCApertureMode( this );
   new FCApertureKeyword( this );

   new FCCentralObstructionValue( this );
   new FCCentralObstructionMode( this );
   new FCCentralObstructionKeyword( this );

   new FCExposureTimeValue( this );
   new FCExposureTimeMode( this );
   new FCExposureTimeKeyword( this );

   new FCAtmosphericExtinctionValue( this );
   new FCAtmosphericExtinctionMode( this );
   new FCAtmosphericExtinctionKeyword( this );

   new FCSensorGainValue( this );
   new FCSensorGainMode( this );
   new FCSensorGainKeyword( this );

   new FCQuantumEfficiencyValue( this );
   new FCQuantumEfficiencyMode( this );
   new FCQuantumEfficiencyKeyword( this );
}

// ----------------------------------------------------------------------------

IsoString FluxCalibrationProcess::Id() const
{
   return "FluxCalibration";
}

// ----------------------------------------------------------------------------

IsoString FluxCalibrationProcess::Category() const
{
   return "Flux"; // No category
}

// ----------------------------------------------------------------------------

uint32 FluxCalibrationProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String FluxCalibrationProcess::Description() const
{
   return

   "<html>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** FluxCalibrationProcess::IconImageXPM() const
{
   return FluxCalibrationIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* FluxCalibrationProcess::DefaultInterface() const
{
   return TheFluxCalibrationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* FluxCalibrationProcess::Create() const
{
   return new FluxCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* FluxCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const FluxCalibrationInstance* instPtr = dynamic_cast<const FluxCalibrationInstance*>( &p );
   return (instPtr != 0) ? new FluxCalibrationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool FluxCalibrationProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: FluxCalibration [<arg_list>] [<view_list>]"
"\n"
"\n-l | --wavelength"
"\n"
"\n      Effective filter wavelenth in nm. This parameter is mandatory."
"\n"
"\n-tr | --transmissivity"
"\n"
"\n      Filter transmissivity in the range ]0,1]."
"\n"
"\n-w | --filter-width"
"\n"
"\n      Filter bandwith in nm. This parameter is mandatory."
"\n"
"\n-a | --aperture"
"\n"
"\n      Telescope aperture diameter in mm. This parameter is mandatory."
"\n"
"\n-o | --central-obstruction"
"\n"
"\n      Telescope central obstruction diameter in mm."
"\n"
"\n-t | --exposure-time"
"\n"
"\n      Exposure time in seconds. This parameter is mandatory."
"\n"
"\n-e | --atmospheric-extinction"
"\n"
"\n      Atmospheric extinction in the range [0,1]."
"\n"
"\n-G | --sensor-gain"
"\n"
"\n      Sensor gain (e-/ADU). The gain must be equal or greater than 0."
"\n"
"\n-qe | --quantum-efficiency"
"\n"
"\n      Sensor quantum efficiency in the range [0,1[."
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int FluxCalibrationProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   FluxCalibrationInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "l" || arg.Id() == "-wavelength" )
            instance.p_wavelength = arg.NumericValue();
         else if ( arg.Id() == "tr" || arg.Id() == "-transmissivity" )
            instance.p_transmissivity = arg.NumericValue();
         else if ( arg.Id() == "w" || arg.Id() == "-filter-width" )
            instance.p_filterWidth = arg.NumericValue();
         else if ( arg.Id() == "a" || arg.Id() == "-aperture" )
            instance.p_aperture = arg.NumericValue();
         else if ( arg.Id() == "o" || arg.Id() == "-central-obstruction" )
            instance.p_centralObstruction = arg.NumericValue();
         else if ( arg.Id() == "t" || arg.Id() == "-exposure-time" )
            instance.p_exposureTime = arg.NumericValue();
         else if ( arg.Id() == "e" || arg.Id() == "-atmospheric-extinction" )
            instance.p_atmosphericExtinction = arg.NumericValue();
         else if ( arg.Id() == "G" || arg.Id() == "-sensor-gain" )
            instance.p_sensorGain = arg.NumericValue();
         else if ( arg.Id() == "qe" || arg.Id() == "-quantum-efficiency" )
            instance.p_quantumEfficiency = arg.NumericValue();
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         // These are standard parameters that all processes should provide.
         if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            throw;
         }

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "No such view: " + *j );
            instance.LaunchOn( v );
         }
      }
   }

   if ( launchInterface )
      instance.LaunchInterface();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentView();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FluxCalibrationProcess.cpp - Released 2017-07-09T18:07:33Z
