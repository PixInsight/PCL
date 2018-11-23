//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0200
// ----------------------------------------------------------------------------
// FluxCalibrationInterface.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "FluxCalibrationInterface.h"
#include "FluxCalibrationParameters.h"
#include "FluxCalibrationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FluxCalibrationInterface* TheFluxCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "FluxCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

int FluxCalibrationInterface::CalibrationParameter::m_valueLabelWidth;
int FluxCalibrationInterface::CalibrationParameter::m_valueEditWidth;
int FluxCalibrationInterface::CalibrationParameter::m_keywordEditWidth;

// ----------------------------------------------------------------------------

FluxCalibrationInterface::FluxCalibrationInterface() :
   m_instance( TheFluxCalibrationProcess )
{
   TheFluxCalibrationInterface = this;
}

// ----------------------------------------------------------------------------

FluxCalibrationInterface::~FluxCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString FluxCalibrationInterface::Id() const
{
   return "FluxCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* FluxCalibrationInterface::Process() const
{
   return TheFluxCalibrationProcess;
}

// ----------------------------------------------------------------------------

const char** FluxCalibrationInterface::IconImageXPM() const
{
   return FluxCalibrationIcon_XPM;
}

// ----------------------------------------------------------------------------

void FluxCalibrationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void FluxCalibrationInterface::ResetInstance()
{
   FluxCalibrationInstance defaultInstance( TheFluxCalibrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FluxCalibration" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheFluxCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FluxCalibrationInterface::NewProcess() const
{
   return new FluxCalibrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const FluxCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a FluxCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FluxCalibrationInterface::UpdateControls()
{
   GUI->Wavelength_Parameter.Update();
   GUI->Transmissivity_Parameter.Update();
   GUI->FilterWidth_Parameter.Update();
   GUI->Aperture_Parameter.Update();
   GUI->CentralObstruction_Parameter.Update();
   GUI->ExposureTime_Parameter.Update();
   GUI->AtmosphericExtinction_Parameter.Update();
   GUI->SensorGain_Parameter.Update();
   GUI->QuantumEfficiency_Parameter.Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FluxCalibrationInterface::GUIData::GUIData( FluxCalibrationInterface& w )
{
   pcl::Font fnt = w.Font();

   FluxCalibrationInterface::CalibrationParameter::m_valueLabelWidth =
         fnt.Width( String( "Central obstruction (mm):" ) + 'M' ); // the longest label text;
   FluxCalibrationInterface::CalibrationParameter::m_valueEditWidth =
         fnt.Width( String( '0', 15 ) );
   FluxCalibrationInterface::CalibrationParameter::m_keywordEditWidth =
         fnt.Width( String( 'M', 10 ) );

   Wavelength_Parameter.Build(            w.m_instance.p_wavelength,            "Wavelength (nm):",
      "<p>Effective filter wavelenth in nm (mandatory parameter).</p>",    true /* required parameter */ );

   Transmissivity_Parameter.Build(        w.m_instance.p_transmissivity,        "Transmissivity:",
      "<p>Filter transmissivity in the range ]0,1].</p>"                                                 );

   FilterWidth_Parameter.Build(           w.m_instance.p_filterWidth,           "Filter width (nm):",
      "<p>Filter bandwith in nm (mandatory parameter).</p>",               true /* required parameter */ );

   Aperture_Parameter.Build(              w.m_instance.p_aperture,              "Aperture (mm):",
      "<p>Telescope aperture diameter in mm (mandatory parameter).</p>",   true /* required parameter */ );

   CentralObstruction_Parameter.Build(    w.m_instance.p_centralObstruction,    "Central obstruction (mm):",
      "<p>Telescope central obstruction diameter in mm.</p>"                                             );

   ExposureTime_Parameter.Build(          w.m_instance.p_exposureTime,          "Exposure time (s):",
      "<p>Exposure time in seconds (mandatory parameter).</p>",            true /* required parameter */ );

   AtmosphericExtinction_Parameter.Build( w.m_instance.p_atmosphericExtinction, "Atmospheric extinction:",
      "<p>Atmospheric extinction in the range [0,1].</p>"                                                );

   SensorGain_Parameter.Build(            w.m_instance.p_sensorGain,            "Sensor gain (e-/ADU):",
      "<p>Sensor gain (e-/ADU). The gain must be &gt;= 0.</p>"                                           );

   QuantumEfficiency_Parameter.Build(     w.m_instance.p_quantumEfficiency,     "Quantum efficiency:",
      "<p>Sensor quantum efficiency in the range [0,1[.</p>"                                             );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );

   Global_Sizer.Add( Wavelength_Parameter );
   Global_Sizer.Add( Transmissivity_Parameter );
   Global_Sizer.Add( FilterWidth_Parameter );
   Global_Sizer.Add( Aperture_Parameter );
   Global_Sizer.Add( CentralObstruction_Parameter );
   Global_Sizer.Add( ExposureTime_Parameter );
   Global_Sizer.Add( AtmosphericExtinction_Parameter );
   Global_Sizer.Add( SensorGain_Parameter );
   Global_Sizer.Add( QuantumEfficiency_Parameter );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FluxCalibrationInterface.cpp - Released 2018-11-23T18:45:58Z
