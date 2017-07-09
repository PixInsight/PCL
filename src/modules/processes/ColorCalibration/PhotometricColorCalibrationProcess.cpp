//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.02.0297
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationProcess.cpp - Released 2017-07-09T18:07:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "PhotometricColorCalibrationProcess.h"
#include "PhotometricColorCalibrationParameters.h"
#include "PhotometricColorCalibrationInstance.h"
#include "PhotometricColorCalibrationInterface.h"

#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/MessageBox.h>
#include <pcl/XML.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "PhotometricColorCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

PhotometricColorCalibrationProcess* ThePhotometricColorCalibrationProcess = nullptr;

// ----------------------------------------------------------------------------

PhotometricColorCalibrationProcess::PhotometricColorCalibrationProcess() : MetaProcess()
{
   ThePhotometricColorCalibrationProcess = this;

   new PCCWhiteReferenceId( this );
   new PCCWhiteReferenceName( this );
   new PCCWhiteReferenceSr_JV( this );
   new PCCWhiteReferenceJB_JV( this );
   new PCCZeroPointSr_JV( this );
   new PCCZeroPointJB_JV( this );
   new PCCFocalLength( this );
   new PCCPixelSize( this );
   new PCCCenterRA( this );
   new PCCCenterDec( this );
   new PCCEpochJD( this );
   new PCCForcePlateSolve( this );
   new PCCIgnoreImagePositionAndScale( this );
   new PCCServerURL( this );
   new PCCSolverCatalogName( this );
   new PCCSolverAutoCatalog( this );
   new PCCSolverLimitMagnitude( this );
   new PCCSolverAutoLimitMagnitude( this );
   new PCCSolverAutoLimitMagnitudeFactor( this );
   new PCCSolverStarSensitivity( this );
   new PCCSolverNoiseLayers( this );
   new PCCSolverAlignmentDevice( this );
   new PCCSolverDistortionCorrection( this );
   new PCCSolverSplineSmoothing( this );
   new PCCSolverProjection( this );
   new PCCPhotCatalogName( this );
   new PCCPhotLimitMagnitude( this );
   new PCCPhotAutoLimitMagnitude( this );
   new PCCPhotAutoLimitMagnitudeFactor( this );
   new PCCPhotAutoAperture( this );
   new PCCPhotAperture( this );
   new PCCPhotUsePSF( this );
   new PCCPhotSaturationThreshold( this );
   new PCCPhotShowDetectedStars( this );
   new PCCPhotShowBackgroundModels( this );
   new PCCPhotGenerateGraphs( this );
   new PCCApplyCalibration( this );
   new PCCNeutralizeBackground( this );
   new PCCBackgroundReferenceViewId( this );
   new PCCBackgroundLow( this );
   new PCCBackgroundHigh( this );
   new PCCBackgroundUseROI( this );
   new PCCBackgroundROIX0( this );
   new PCCBackgroundROIY0( this );
   new PCCBackgroundROIX1( this );
   new PCCBackgroundROIY1( this );
}

// ----------------------------------------------------------------------------

IsoString PhotometricColorCalibrationProcess::Id() const
{
   return "PhotometricColorCalibration";
}

// ----------------------------------------------------------------------------

IsoString PhotometricColorCalibrationProcess::Category() const
{
   return "ColorCalibration,Photometry";
}

// ----------------------------------------------------------------------------

uint32 PhotometricColorCalibrationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String PhotometricColorCalibrationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** PhotometricColorCalibrationProcess::IconImageXPM() const
{
   return PhotometricColorCalibrationIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* PhotometricColorCalibrationProcess::DefaultInterface() const
{
   return ThePhotometricColorCalibrationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* PhotometricColorCalibrationProcess::Create() const
{
   return new PhotometricColorCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* PhotometricColorCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const PhotometricColorCalibrationInstance* instPtr = dynamic_cast<const PhotometricColorCalibrationInstance*>( &p );
   return (instPtr != nullptr) ? new PhotometricColorCalibrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static WhiteReferenceArray s_whiteReferences;
static WhiteReference      s_zeroPoint;
static bool                s_initialized = false;
static bool                s_valid = false;

void PhotometricColorCalibrationProcess::InitializeWhiteReferences( bool interactive )
{
   if ( !s_initialized )
   {
      s_initialized = true;

      if ( !interactive )
         Exception::DisableGUIOutput();

      String wrfFilePath = PixInsightSettings::GlobalString( "Application/LibraryDirectory" ) + "/default.xwrf";

      try
      {
         if ( !File::Exists( wrfFilePath ) )
            throw String( "The white reference database file does not exist" );

         XMLDocument xml;
         xml.SetParserOption( XMLParserOption::IgnoreComments );
         xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
         xml.Parse( File::ReadTextFile( wrfFilePath ).UTF8ToUTF16() );

         if ( xml.RootElement() == nullptr )
            throw String( "The XML document has no root element." );
         if ( xml.RootElement()->Name() != "xwrf" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
            throw String( "Not an XWRF version 1.0 document." );

         bool zeroPointDefined = false;
         int errorCount = 0;
         XMLElement::child_element_list elements = xml.RootElement()->ChildElements();
         for ( const XMLElement& element : elements )
         {
            bool isZeroPoint;
            if ( element.Name() == "WhiteRef" )
               isZeroPoint = false;
            else if ( element.Name() == "ZeroPoint" )
               isZeroPoint = true;
            else
               continue;

            String id = element.AttributeValue( "id" );
            String name = element.AttributeValue( "name" );
            String JB_JV = element.AttributeValue( "JB_JV" );
            String Sr_JV = element.AttributeValue( "Sr_JV" );
            String Sr_Sg = element.AttributeValue( "Sr_Sg" );
            String isDefault = element.AttributeValue( "default" );

            if ( id.IsEmpty() || name.IsEmpty() || JB_JV.IsEmpty() || Sr_JV.IsEmpty() || Sr_Sg.IsEmpty() )
            {
               ++errorCount;
               continue;
            }

            WhiteReference W;
            W.id = id;
            W.name = name;
            if ( !JB_JV.TryToDouble( W.JB_JV ) || !Sr_JV.TryToDouble( W.Sr_JV ) || !Sr_Sg.TryToDouble( W.Sr_Sg ) )
            {
               ++errorCount;
               continue;
            }

            if ( isZeroPoint )
            {
               if ( zeroPointDefined )
               {
                  ++errorCount;
                  continue;
               }

               s_zeroPoint = W;
               zeroPointDefined = true;
            }
            else
            {
               bool setDefault = false;
               if ( !isDefault.IsEmpty() )
                  if ( !isDefault.TryToBool( setDefault ) )
                  {
                     ++errorCount;
                     continue;
                  }

               if ( setDefault )
                  s_whiteReferences.Prepend( W );
               else
                  s_whiteReferences << W;
            }
         }

         if ( s_whiteReferences.IsEmpty() )
            throw String( "No valid white reference definitions are available" );
         if ( !zeroPointDefined )
            throw String( "No valid photometric zero point reference is available" );

         if ( errorCount > 0 )
         {
            if ( interactive )
               MessageBox( "<p>Decoded white reference database file with " + String( errorCount ) + "error(s).</p>"
                        "<p><b>Reinstalling the PixInsight platform should solve this problem.</b></p>",
                        "PhotometricColorCalibration",
                        StdIcon::Warning,
                        StdButton::Ok ).Execute();

            Console().CriticalLn( "<end><cbr><br>*** Error: PhotometricColorCalibration: "
                        "Decoded white reference database file with " + String( errorCount ) + "error(s): " + wrfFilePath );
         }

         s_valid = true;
      }
      catch ( const Exception& x )
      {
         try
         {
            throw x;
         }
         ERROR_HANDLER
      }
      catch ( const String& what )
      {
         if ( interactive )
            MessageBox( "<p>Decoding white reference database file: " + what +
                        ". Selectable white references won't be available, and PhotometricColorCalibration "
                        "will work with factory-default white and photometric zero point references.</p>"
                        "<p><b>Reinstalling the PixInsight platform should solve this problem.</b></p>",
                        "PhotometricColorCalibration",
                        StdIcon::Error,
                        StdButton::Ok ).Execute();

         Console().CriticalLn( "<end><cbr><br>*** Error: PhotometricColorCalibration: "
                        "Decoding white reference database file: " + what + ": " + wrfFilePath );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }

      if ( !interactive )
         Exception::EnableGUIOutput();
   }
}

bool PhotometricColorCalibrationProcess::HasValidWhiteReferences()
{
   return s_valid;
}

const WhiteReferenceArray& PhotometricColorCalibrationProcess::WhiteReferences()
{
   return s_whiteReferences;
}

const WhiteReference& PhotometricColorCalibrationProcess::DefaultWhiteReference()
{
   return s_whiteReferences[0];
}

const WhiteReference& PhotometricColorCalibrationProcess::ZeroPoint()
{
   return s_zeroPoint;
}

int PhotometricColorCalibrationProcess::FindWhiteReferenceById( const String& id )
{
   for ( size_type i = 0; i < s_whiteReferences.Length(); ++i )
      if ( s_whiteReferences[i].id == id )
         return int( i );
   return -1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationProcess.cpp - Released 2017-07-09T18:07:32Z
