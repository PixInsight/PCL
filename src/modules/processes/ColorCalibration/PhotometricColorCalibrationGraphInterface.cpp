//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.02.0297
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationGraphInterface.cpp - Released 2017-07-09T18:07:32Z
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

#include "PhotometricColorCalibrationGraphInterface.h"
#include "PhotometricColorCalibrationProcess.h"

#include <pcl/LinearFit.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PhotometricColorCalibrationGraphInterface* ThePhotometricColorCalibrationGraphInterface = nullptr;

// ----------------------------------------------------------------------------

#include "PhotometricColorCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

PhotometricColorCalibrationGraphInterface::PhotometricColorCalibrationGraphInterface()
{
   ThePhotometricColorCalibrationGraphInterface = this;
}

// ----------------------------------------------------------------------------

PhotometricColorCalibrationGraphInterface::~PhotometricColorCalibrationGraphInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString PhotometricColorCalibrationGraphInterface::Id() const
{
   return "PhotometricColorCalibrationGraph";
}

// ----------------------------------------------------------------------------

MetaProcess* PhotometricColorCalibrationGraphInterface::Process() const
{
   return ThePhotometricColorCalibrationProcess;
}

// ----------------------------------------------------------------------------

const char** PhotometricColorCalibrationGraphInterface::IconImageXPM() const
{
   return PhotometricColorCalibrationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures PhotometricColorCalibrationGraphInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationGraphInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationGraphInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationGraphInterface::Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "PhotometricColorCalibration" );
   }

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationGraphInterface::SaveSettings() const
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationGraphInterface::LoadSettings()
{
   // Placeholder
}

// ----------------------------------------------------------------------------

struct DataPoint
{
   double x, y, z;
};

typedef GenericVector<DataPoint> DataPointVector;

inline bool operator ==( const DataPoint& p1, const DataPoint& p2 )
{
   return p1.x == p2.x;
}

inline bool operator <( const DataPoint& p1, const DataPoint& p2 )
{
   return p1.x < p2.x;
}

void PhotometricColorCalibrationGraphInterface::UpdateGraphs(
                           const String& viewId, const String& catalogName,
                           const String& filterNameR, const String& filterNameG, const String& filterNameB,
                           const Vector& catRG, const Vector& imgRG, const LinearFit& fitRG,
                           const Vector& catBG, const Vector& imgBG, const LinearFit& fitBG )
{
   if ( GUI == nullptr )
      return;

   DataPointVector PRG( catRG.Length() );
   for ( int i = 0; i < catRG.Length(); ++i )
   {
      PRG[i].x = catRG[i];
      PRG[i].y = imgRG[i];
      PRG[i].z = fitRG( catRG[i] );
   }
   PRG.Sort();

   DataPointVector PBG( catBG.Length() );
   for ( int i = 0; i < catBG.Length(); ++i )
   {
      PBG[i].x = catBG[i];
      PBG[i].y = imgBG[i];
      PBG[i].z = fitBG( catBG[i] );
   }
   PBG.Sort();

   String RGFilters = filterNameR + '-' + filterNameG;
   String RGLabels = "labels: [ \"" + RGFilters + "\", \"R-G\", \"Fit\" ]";
   String RGData = "[\n";
   for ( int i = 0; ; )
   {
      RGData << '[' << String().Format( "%+.6g", PRG[i].x )
             << ',' << String().Format( "%+.6g", PRG[i].y )
             << ',' << String().Format( "%+.6g", PRG[i].z ) << ']';
      if ( ++i == catRG.Length() )
         break;
      RGData << ",\n";
   }
   RGData << "\n]";

   String BGFilters = filterNameB + '-' + filterNameG;
   String BGLabels = "labels: [ \"" + BGFilters + "\", \"B-G\", \"Fit\" ]";
   String BGData = "[\n";
   for ( int i = 0; ; )
   {
      BGData << '[' << String().Format( "%+.6g", PBG[i].x )
             << ',' << String().Format( "%+.6g", PBG[i].y )
             << ',' << String().Format( "%+.6g", PBG[i].z ) << ']';
      if ( ++i == catBG.Length() )
         break;
      BGData << ",\n";
   }
   BGData << "\n]";

   String coreSrcDir = PixInsightSettings::GlobalString ( "Application/SrcDirectory" );

   String html =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<meta charset=\"utf-8\">\n"
"<script type=\"text/javascript\" src=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.js" ) + "\"></script>\n"
"<script type=\"text/javascript\" src=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-extra.js" ) + "\"></script>\n"
"<link rel=\"stylesheet\" href=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-doc.css" ) + "\"/>\n"
"<link rel=\"stylesheet\" href=\"" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.css" ) + "\"/>\n"
"<script type=\"text/javascript\">\n"
"function toggleDisplay( id )\n"
"{\n"
"   var obj = document.getElementById( id );\n"
"   if ( obj )\n"
"      obj.style.display = obj.style.display ? \"\" : \"none\";\n"
"}\n"
"</script>\n"
"</head>\n"
"<body>\n"
"<h3 style=\"margin-top:0;\">White Balance Functions"
   "<img style=\"float:right;\""
   " onclick=\"toggleDisplay('Help');\""
   " src=\"qrc:/icons/help.png\""
   " title=\"Click to show/hide the help text.\"/></h3>\n"
"<p>Image: " + viewId + "<br/>\n"
   "Catalog: " + catalogName + "<br/>\n"
   "Processed: " + TimePoint::Now().ToString() + " UTC</p>\n"
"<div id=\"Help\" style=\"display:none;\">\n"
"<hr/>\n"
"<p>These graphs show the white balance functions fitted by the PhotometricColorCalibration process for the "
   "specified image and photometric catalog.</p>\n"
"<p>On each graph, the X axis corresponds to color indices found in the catalog, while the Y axis shows color "
   "indices calculated from image pixel values. Each dot corresponds to a measured star, and the straight line "
   "is the white balance function fitted by the PCC process.</p>\n"
"<p>Outlier stars can be easily identified as dots that depart considerably from the fitted linear function. "
   "The more grouped all dots are around the fitted line the better, but as you'll see, PCC's linear fitting "
   "routines have been designed to be very robust and resilient to outliers, so they should not degrade the "
   "quality of the color calibration process under normal working conditions.</p>\n"
"<p>The graphs are interactive:</p>\n"
"<ul>\n"
"<li>Mouse over to highlight individual values for measured stars.</li>\n"
"<li>Click and drag to zoom. You can zoom both vertically and horizontally.</li>\n"
"<li>Double-click to zoom out.</li>\n"
"<li>On a zoomed graph, shift-drag to pan.</li>\n"
"</ul>\n"
"<hr/>\n"
"</div>\n"
"<p><strong>X:" + RGFilters + " &nbsp; Y:R-G &nbsp; N:" + String( catRG.Length() ) + "</strong></p>\n"
"<div id=\"graphRG\"></div>\n"
"<p><strong>X:" + BGFilters + " &nbsp; Y:B-G &nbsp; N:" + String( catBG.Length() ) + "</strong></p>\n"
"<div id=\"graphBG\"></div>\n"
"<script type=\"text/javascript\">\n"
"   var gRG = new Dygraph(\n"
"      document.getElementById( \"graphRG\" ),\n" +
RGData + ",\n"
"      {\n"
"         " + RGLabels + ",\n"
"         axisLabelFontSize: 12,\n"
"         series: { 'R-G': { strokeWidth: 0, drawPoints: true, pointSize: 3, highlightCircleSize: 4 }, 'Fit': { strokeWidth: 1.0, drawPoints: false } }\n"
"      }\n"
"   );\n"
"   var gBG = new Dygraph(\n"
"      document.getElementById( \"graphBG\" ),\n" +
BGData + ",\n" +
"      {\n"
"         " + BGLabels + ",\n"
"         axisLabelFontSize: 12,\n"
"         series: { 'B-G': { strokeWidth: 0, drawPoints: true, pointSize: 3, highlightCircleSize: 4 }, 'Fit': { strokeWidth: 1.0, drawPoints: false } }\n"
"      }\n"
"   );\n"
"</script>\n"
"</body>\n"
"</html>\n";

   String filePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "PCC_graphs_", ".html" );
   File::WriteTextFile( filePath, html.ToUTF8() );
   m_htmlFiles << filePath;

   GUI->Graph_WebView.LoadContent( File::FileURI( filePath ) );
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationGraphInterface::CleanUp()
{
   for ( const String& filePath : m_htmlFiles )
      if ( !filePath.IsEmpty() )
      {
         try
         {
            if ( File::Exists( filePath ) )
               File::Remove( filePath );
         }
         catch ( ... )
         {
         }
      }
   m_htmlFiles.Clear();
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationGraphInterface::e_LoadFinished( WebView& sender, bool state )
{
   GUI->Graph_WebView.SetZoomFactor( DisplayPixelRatio() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PhotometricColorCalibrationGraphInterface::GUIData::GUIData( PhotometricColorCalibrationGraphInterface& w )
{
   Graph_WebView.OnLoadFinished( (WebView::state_event_handler)&PhotometricColorCalibrationGraphInterface::e_LoadFinished, w );

   Global_Sizer.Add( Graph_WebView );

   w.SetSizer( Global_Sizer );
   w.SetScaledMinSize( 400, 400 );
   w.Resize( w.PhysicalPixelsToLogical( 520 ), w.PhysicalPixelsToLogical( 850 ) );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationGraphInterface.cpp - Released 2017-07-09T18:07:32Z
