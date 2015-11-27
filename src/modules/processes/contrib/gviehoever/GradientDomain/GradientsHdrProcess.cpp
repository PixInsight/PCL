//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// GradientsHdrProcess.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2015. Licensed under LGPL 2.1
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include "GradientsHdrProcess.h"
#include "GradientsHdrParameters.h"
#include "GradientsHdrInstance.h"
#include "GradientsHdrInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "GradientsHdrCompressionIcon.xpm"

// ----------------------------------------------------------------------------

GradientsHdrProcess* TheGradientsHdrProcess = 0;

// ----------------------------------------------------------------------------

GradientsHdrProcess::GradientsHdrProcess() : MetaProcess()
{
   TheGradientsHdrProcess = this;

   new GradientsHdrParameterLogMaxGradient( this );
   new GradientsHdrParameterLogMinGradient( this );
   new GradientsHdrParameterExpGradient( this );
   new GradientsHdrParameterRescale01( this);
   new GradientsHdrParameterPreserveColor( this);
}

// ----------------------------------------------------------------------------

IsoString GradientsHdrProcess::Id() const
{
   return "GradientHDRCompression";
}

IsoString GradientsHdrProcess::Aliases() const
{
   return "GradientsHdrCompression";
}

// ----------------------------------------------------------------------------

IsoString GradientsHdrProcess::Category() const
{
   return "GradientDomain";
}

// ----------------------------------------------------------------------------

uint32 GradientsHdrProcess::Version() const
{
   return 0x102;
}

// ----------------------------------------------------------------------------

String GradientsHdrProcess::Description() const
{
   return

   "<html>"
   "<p>GradientsHDRCompression supports creation of HDR images, such as M42, M33 etc. "
   "It reduces steep gradients, thus revealing faint detail. Should be applied to linear image. "
   "May consume considerable amounts of CPU time</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** GradientsHdrProcess::IconImageXPM() const
{
   return 0; // FIXME GradientsHdrCompressionIcon_XPM; ---> put a nice icon here
}
// ----------------------------------------------------------------------------

ProcessInterface* GradientsHdrProcess::DefaultInterface() const
{
   return TheGradientsHdrInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* GradientsHdrProcess::Create() const
{
   return new GradientsHdrInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* GradientsHdrProcess::Clone( const ProcessImplementation& p ) const
{
   const GradientsHdrInstance* instPtr = dynamic_cast<const GradientsHdrInstance*>( &p );
   return (instPtr != 0) ? new GradientsHdrInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool GradientsHdrProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
         "<raw>"
         "Usage: GradientsHdrCompression exp_gradient [log_max_gradient  [log_min_gradient [bRescale01 [bPreserveColor]]]"
         "\n"
         "\n exp_gradient is a double in [0,1.1]. Gradient is transformed via sign(gradient)*pow(gradient,exp_gradient). Small values enhance small gradients. Neutral is 1.0"
         "\n log_max_gradient is a double in [-7,0] abs(gradients)>pow(10,value) are clipped to sign(gradient)*pow(10,value), thus reducing the influence of large gradients. Default 0.0 is neutral."
         "\n log_min_gradient is a double in ]-7,0]. abs(gradients)<pow(10,value) are clipped to zero. -7 is neutral."
         "\n bRescale01 is a boolean. If true (default), the result is rescaled to range [0,1], otherwise to the range of the original image"
         "\n bPreserveColor is a boolean. If true (default), the relative proportions of R:G:B are preserved for RGB images"
         "\n"
         "\n--interface"
         "\n      Launches the interface of this process."
         "\n--help"
         "\n      Displays this help and exits."
         "</raw>" );
}

int GradientsHdrProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::Ignore);

   GradientsHdrInstance instance( this );

   bool launchInterface = false;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i ){
   const Argument& arg = *i;

   if (arg.IsItemList()){
StringList const & rItemsList=arg.Items();
if (rItemsList.Length() == 0 || rItemsList.Length()>5){
   throw Error( "expecting exp_gradient [log_max_gradient  [log_min_gradient [bRescale01 [bPreserveColor]]]] as argument");
}
if (rItemsList.Length() >=1){
   instance.expGradient=rItemsList[0].ToDouble();
   if (rItemsList.Length() >=2){
      instance.logMaxGradient=rItemsList[1].ToDouble();
   }
   if (rItemsList.Length() >=3){
      instance.logMinGradient=rItemsList[2].ToDouble();
   }
   if (rItemsList.Length() >=4){
      instance.bRescale01=rItemsList[3].ToBool();
   }
   if (rItemsList.Length() >=5){
      instance.bPreserveColor=rItemsList[4].ToBool();
   }
}
   } else if ( arg.IsLiteral() ) {
// These are standard parameters that all processes should provide.
if ( arg.Id() == "-interface" )
   launchInterface = true;
else if ( arg.Id() == "-help" )
   {
      ShowHelp();
      return 0;
   }
else
   throw Error( "Unknown literal argument: " + arg.Token() );
   } else {
throw Error( "Unknown type of argument: " + arg.Token() );
   }
   }

   if ( launchInterface )
   instance.LaunchInterface();
   else if ( ImageWindow::ActiveWindow().IsNull() )
   throw Error( "There is no active image window." );
   instance.LaunchOnCurrentView();

   return 0;
}

} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsHdrProcess.cpp - Released 2015/11/26 16:00:13 UTC
