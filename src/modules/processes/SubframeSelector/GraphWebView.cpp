//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0402
// ----------------------------------------------------------------------------
// GraphWebView.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "GraphWebView.h"

namespace pcl
{

// ----------------------------------------------------------------------------

void GraphWebView::SetDataset( const String& dataname, DataPointVector* dataset )
{
   String coreSrcDir = PixInsightSettings::GlobalString ( "Application/SrcDirectory" );

   // Find Median and MAD of the values
   Array<double> values( dataset->Length() );
   for ( int i = 0; i < dataset->Length(); ++i )
      values[i] = dataset->At( i )->y;
   double median = pcl::Median( values.Begin(), values.End() );
   double mad = pcl::MAD( values.Begin(), values.End() );

   // Create the Graphing arrays and backend objects
   String graphingArray = "[ ";
   String indexedApprovals = "{ ";
   for ( DataPointVector::const_iterator i = dataset->Begin(); i != dataset->End(); ++i ) {
      graphingArray += String().Format( "[ %i, [%.4f, %.4f, %.4f] ], ", i->x, median - mad, i->y, median + mad );
      indexedApprovals += String().Format( "%i:%s, ", i->x, i->approved ? "true" : "false" );
   }
   indexedApprovals += "}";
   graphingArray += "]";

   String html = R"DELIM(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<script type="text/javascript" src=")DELIM" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.js" ) + R"DELIM("></script>
<script type="text/javascript" src=")DELIM" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-extra.js" ) + R"DELIM("></script>
<link rel="stylesheet" href=")DELIM" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph-doc.css" ) + R"DELIM("/>
<link rel="stylesheet" href=")DELIM" + File::FileURI( coreSrcDir + "/scripts/Dygraph/dygraph.css" ) + R"DELIM("/>
<style>
   html, body {
      background-color: rgba(0,0,0,0);
   }

   #graph {
      height: 90vh;
      weight: 80vw;
   }

   .dygraph-legend {
      width: auto;
      padding: 2px;
      color: white;
      background-color: rgba(0,0,0,0.9);
      border: 4px solid rgba(0,0,0,0.5);
      border-radius: 2px;
   }
</style>
</head>
<body>
<div id="graph"></div>
<script type="text/javascript">
   var approvals = )DELIM" + indexedApprovals + R"DELIM(;
   var dataset = )DELIM" + graphingArray + R"DELIM(;

   function legendFormatter(data) {
      let first = true;
      let html = '';
      data.series.forEach(function(series) {
         if (!series.isVisible) return;
         if (!first) html += '<br/>';
         html += '<b>' + series.labelHTML + '</b>: ' + series.yHTML;
         first = false;
      });
      return html;
   }

   function drawApprovedPoint(g, series, ctx, cx, cy, color, radius, idx) {
      ctx.save();

      ctx.lineCap = "round";
      ctx.lineJoin = "round";

      if (approvals[dataset[idx][0]] === true) {
         ctx.strokeStyle = "green";
         ctx.lineWidth = 3;

         ctx.beginPath();
         ctx.moveTo(cx - radius,     cy);
         ctx.lineTo(cx - radius/2.0, cy + radius);
         ctx.closePath();
         ctx.stroke();

         ctx.beginPath();
         ctx.moveTo(cx - radius/2.0, cy + radius);
         ctx.lineTo(cx + radius,     cy - radius);
         ctx.closePath();
         ctx.stroke();
      } else {
         ctx.strokeStyle = "red";
         ctx.lineWidth = 4;

         ctx.beginPath();
         ctx.moveTo(cx + radius, cy + radius);
         ctx.lineTo(cx - radius, cy - radius);
         ctx.closePath();
         ctx.stroke();

         ctx.beginPath();
         ctx.moveTo(cx + radius, cy - radius);
         ctx.lineTo(cx - radius, cy + radius);
         ctx.closePath();
         ctx.stroke();
      }

      ctx.restore()
   }

   var g = new Dygraph(
      document.getElementById("graph"), dataset,
      {
         ylabel: ")DELIM" + dataname + R"DELIM(",
         labels: ["Index", ")DELIM" + dataname + R"DELIM("],
         labelsSeparateLines: true,
         digitsAfterDecimal: 4,
         legend: "follow",
         legendFormatter: legendFormatter,
         customBars: true,
         errorBars: true,
         strokeWidth: 2,
         drawPoints: true,
         drawPointCallback: drawApprovedPoint,
         pointSize: 6,
         xRangePad: 10,
         yRangePad: 20,
      }
   );
</script>
</body>
</html>
   )DELIM";

   SetHTML( html.ToUTF8() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GraphWebView.cpp - Released 2017-08-01T14:26:58Z
