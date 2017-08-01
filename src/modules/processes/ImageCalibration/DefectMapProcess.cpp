//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0332
// ----------------------------------------------------------------------------
// DefectMapProcess.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "DefectMapProcess.h"
#include "DefectMapParameters.h"
#include "DefectMapInstance.h"
#include "DefectMapInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "DefectMapIcon.xpm"

// ----------------------------------------------------------------------------

DefectMapProcess* TheDefectMapProcess = 0;

// ----------------------------------------------------------------------------

DefectMapProcess::DefectMapProcess() : MetaProcess()
{
   TheDefectMapProcess = this;

   // Instantiate process parameters
   new DMPMapId( this );
   new DMPOperation( this );
   new DMPStructure( this );
   new DMPIsCFA( this );
}

IsoString DefectMapProcess::Id() const
{
   return "DefectMap";
}

IsoString DefectMapProcess::Category() const
{
   return "ImageCalibration,Preprocessing";
}

uint32 DefectMapProcess::Version() const
{
   return 0x100;
}

String DefectMapProcess::Description() const
{
   return
   "<p>DefectMap uses a <i>defect map</i> image to find defective pixels, "
   "then it replaces all defective pixels with data computed from neighbor pixels.</p>"
   "<p>In a defect map image, defective pixels are signaled by zero pixel values.</p>";
}

const char** DefectMapProcess::IconImageXPM() const
{
   return DefectMapIcon_XPM;
}

ProcessInterface* DefectMapProcess::DefaultInterface() const
{
   return TheDefectMapInterface;
}

ProcessImplementation* DefectMapProcess::Create() const
{
   return new DefectMapInstance( this );
}

ProcessImplementation* DefectMapProcess::Clone( const ProcessImplementation& p ) const
{
   const DefectMapInstance* instPtr = dynamic_cast<const DefectMapInstance*>( &p );
   return (instPtr != 0) ? new DefectMapInstance( *instPtr ) : 0;
}

bool DefectMapProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: DefectMap [<arg_list>] [<img_list>]"
"\n"
"\n--map=<image_id>"
"\n"
"\n      <image_id> identifies the defect map image. In a defect map, black (zero)"
"\n      pixels correspond to bad or defective pixels. Bad pixels will be replaced"
"\n      with new values computed from neighbor pixels."
"\n"
"\n      When using convolution replacement operations (-gaussian, -mean),"
"\n      non-zero pixel values represent the pixel weights that will be used for"
"\n      convolution. Morphological replacement operations (-minimum, -maximum,"
"\n      -median) use binarized (0/1) map values."
"\n"
"\n--gaussian"
"\n--mean"
"\n"
"\n      Specify a defect replacement operation to be applied by convolution."
"\n"
"\n--minimum"
"\n--maximum"
"\n--median"
"\n"
"\n      Specify a defect replacement operation to be applied by a morphological"
"\n      transformation. (default = median)"
"\n"
"\n--square"
"\n--circular"
"\n--horizontal"
"\n--vertical"
"\n"
"\n      Specify a structuring element for the morphological defect replacement"
"\n      operations."
"\n"
"\n--CFA"
"\n"
"\n      Use this if you are operating over bayered images."
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

#define OUT_OF_RANGE \
   throw pcl::Error( "Numerical argument out of range: " + arg.Token() )

int DefectMapProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );
   DefectMapInstance instance( this );

   bool launchInterface = false;
   size_type count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "-map" )
            instance.mapId = arg.StringValue();
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "-minimum" )
            instance.operation = DMPOperation::Minimum;
         else if ( arg.Id() == "-median" )
            instance.operation = DMPOperation::Median;
         else if ( arg.Id() == "-mean" )
            instance.operation = DMPOperation::Mean;
         else if ( arg.Id() == "-maximum" )
            instance.operation = DMPOperation::Maximum;
         else if ( arg.Id() == "-gaussian" )
            instance.operation = DMPOperation::Gaussian;
         else if ( arg.Id() == "-square" )
            instance.structure = DMPStructure::Square;
         else if ( arg.Id() == "-circular" )
            instance.structure = DMPStructure::Circular;
         else if ( arg.Id() == "-horizontal" )
            instance.structure = DMPStructure::Horizontal;
         else if ( arg.Id() == "-vertical" )
            instance.structure = DMPStructure::Vertical;
         else if ( arg.Id() == "-CFA" )
            instance.isCFA = true;
         else if ( arg.Id() == "-i" || arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-h" || arg.Id() == "-help" )
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
            continue;
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
// EOF DefectMapProcess.cpp - Released 2017-08-01T14:26:58Z
