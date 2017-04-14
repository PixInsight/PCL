//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0281
// ----------------------------------------------------------------------------
// GREYCstorationProcess.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#include "GREYCstorationProcess.h"
#include "GREYCstorationParameters.h"
#include "GREYCstorationInstance.h"
#include "GREYCstorationInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "GREYCstorationIcon.xpm"

// ----------------------------------------------------------------------------

GREYCstorationProcess* TheGREYCstorationProcess = 0;

// ----------------------------------------------------------------------------

GREYCstorationProcess::GREYCstorationProcess() : MetaProcess()
{
   TheGREYCstorationProcess = this;

   // Instantiate process parameters
   new GREYCsAmplitude( this );
   new GREYCsIterations( this );
   new GREYCsSharpness( this );
   new GREYCsAnisotropy( this );
   new GREYCsAlpha( this );
   new GREYCsSigma( this );
   new GREYCsFastApproximation( this );
   new GREYCsPrecision( this );
   new GREYCsSpatialStepSize( this );
   new GREYCsAngularStepSize( this );
   new GREYCsInterpolation( this );
   new GREYCsCoupledChannels( this );
}

// ----------------------------------------------------------------------------

IsoString GREYCstorationProcess::Id() const
{
   return "GREYCstoration";
}

// ----------------------------------------------------------------------------

IsoString GREYCstorationProcess::Category() const
{
   return "Obsolete,NoiseReduction";
}

// ----------------------------------------------------------------------------

uint32 GREYCstorationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String GREYCstorationProcess::Description() const
{
   return

   "<html>"
   "<p>GREYCstoration is an open-source image regularization algorithm which is able to locally remove small variations "
   "of pixel intensities while preserving significant global image features, such as edges and corners. The most "
   "direct application of image regularization is noise reduction. By extension, it can also be used to inpaint or "
   "resize images.</p>"

   "<p>GREYCstoration is based on state-of-the-art image processing methods using <i>nonlinear multi-valued diffusion "
   "partial differential equations</i>. This kind of method generally outperforms basic image filtering techniques "
   "(such as convolution, median filtering, etc), classically encountered in image painting programs.</p>"

   "<p>The author of the GREYCstoration algorithm is <b>David Tschumperl&eacute;</b>, a CNRS researcher in the Image "
   "Team of the GREYC Lab (UMR CNRS 6072) in Caen/France.</p>"

   "<p>This implementation of the GREYCstoration PixInsight module uses a custom adaptation of CImg version 1.46 "
   "(released 2010/11/18) under the CeCILL-C license, with the kind permission of the author. The CImg library is "
   "available at http://cimg.sourceforge.net/</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** GREYCstorationProcess::IconImageXPM() const
{
   return GREYCstorationIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* GREYCstorationProcess::DefaultInterface() const
{
   return TheGREYCstorationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* GREYCstorationProcess::Create() const
{
   return new GREYCstorationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* GREYCstorationProcess::Clone( const ProcessImplementation& p ) const
{
   const GREYCstorationInstance* instPtr = dynamic_cast<const GREYCstorationInstance*>( &p );
   return (instPtr != 0) ? new GREYCstorationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool GREYCstorationProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: GREYCstoration [<arg_list>] [<view_list>]"
"\n"
"\n-dt=<n> | -amplitude=<n>"
"\n"
"\n      Regularization strength per iteration, n >= 0."
"\n"
"\n-n=<n> | -iter=<n> | -iterations=<n>"
"\n"
"\n      Number of iterations, n > 0."
"\n"
"\n-p=<n> | -sharpness=<n>"
"\n"
"\n      Contour preservation, n >= 0."
"\n"
"\n-a=<n> | -anisotropy=<n>"
"\n"
"\n      Smoothing anisotropy, 0 <= n <= 1."
"\n"
"\n-alpha=<n> | -noise-scale=<n>"
"\n"
"\n      Noise scale, n >= 0."
"\n"
"\n-sigma=<n> | -regularity=<n>"
"\n"
"\n      Geometry regularity, n >= 0."
"\n"
"\n-f[+|-] | -fast[+|-] | -fast-approximation[+|-]"
"\n"
"\n      Enables or disables fast approximation. (default=enabled)"
"\n"
"\n-c[+|-] | -coupled-channels[+|-]"
"\n"
"\n      Enables or disables the coupled-channel mode. (default=enabled)"
"\n"
"\n-prec=<n> | -precision=<n>"
"\n"
"\n      Computation precision, n > 0."
"\n"
"\n-dl=<n> | -spatial-step-size=<n>"
"\n"
"\n      Spatial integration step, 0 <= n <= 1."
"\n"
"\n-da=<n> | -angular-step-size=<n>"
"\n"
"\n      Angular integration step size in degrees, 0 <= n <= 90."
"\n"
"\n-i=<i> | -interp=<i> | -interpolation=<i>"
"\n"
"\n      Interpolation type, where <i> is one of:"
"\n"
"\n      nearest  Nearest neighbor interpolation. (default=nearest)"
"\n      linear   Linear interpolation."
"\n      rk2      2nd order Runge-Kutta interpolation."
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

#define CHECK_OUT_OF_RANGE( param ) \
   if ( arg.NumericValue() < param->MinimumValue() || arg.NumericValue() > param->MaximumValue() ) \
      throw pcl::Error( "Numeric argument out of range: " + arg.Token() )

int GREYCstorationProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   GREYCstorationInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "dt" || arg.Id() == "amplitude" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsAmplitudeParameter );
            instance.amplitude = arg.NumericValue();
         }
         else if ( arg.Id() == "n" || arg.Id() == "iter" || arg.Id() == "iterations" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsIterationsParameter );
            instance.numberOfIterations = int( arg.NumericValue() );
         }
         else if ( arg.Id() == "p" || arg.Id() == "sharpness" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsSharpnessParameter );
            instance.sharpness = arg.NumericValue();
         }
         else if ( arg.Id() == "a" || arg.Id() == "anisotropy" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsAnisotropyParameter );
            instance.anisotropy = arg.NumericValue();
         }
         else if ( arg.Id() == "alpha" || arg.Id() == "noise-scale" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsAlphaParameter );
            instance.alpha = arg.NumericValue();
         }
         else if ( arg.Id() == "sigma" || arg.Id() == "regularity" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsSigmaParameter );
            instance.sigma = arg.NumericValue();
         }
         else if ( arg.Id() == "prec" || arg.Id() == "precision" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsPrecisionParameter );
            instance.precision = arg.NumericValue();
         }
         else if ( arg.Id() == "dl" || arg.Id() == "spatial-step-size" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsSpatialStepSizeParameter );
            instance.spatialStepSize = arg.NumericValue();
         }
         else if ( arg.Id() == "da" || arg.Id() == "angular-step-size" )
         {
            CHECK_OUT_OF_RANGE( TheGREYCsAngularStepSizeParameter );
            instance.angularStepSize = arg.NumericValue();
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "i" || arg.Id() == "interp" || arg.Id() == "interpolation" )
         {
            if ( arg.StringValue() == "nearest" )
               instance.interpolation = GREYCsInterpolation::Nearest;
            else if ( arg.StringValue() == "linear" )
               instance.interpolation = GREYCsInterpolation::Bilinear;
            else if ( arg.StringValue() == "rk2" )
               instance.interpolation = GREYCsInterpolation::RungeKutta;
            else
               throw Error( "Unknown interpolation type: " + arg.Token() );
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "f" || arg.Id() == "fast" || arg.Id() == "fast-approximation" )
            instance.fastApproximation = arg.SwitchState();
         else if ( arg.Id() == "c" || arg.Id() == "coupled-channels" )
            instance.coupledChannels = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "f" || arg.Id() == "fast" || arg.Id() == "fast-approximation" )
            instance.fastApproximation = true;
         else if ( arg.Id() == "c" || arg.Id() == "coupled-channels" )
            instance.coupledChannels = true;
         else if ( arg.Id() == "-interface" )
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
// EOF GREYCstorationProcess.cpp - Released 2017-04-14T23:07:12Z
