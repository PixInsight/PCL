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

#include <pcl/Console.h>

#include "GraphWebView.h"
#include "SubframeSelectorMeasureData.h"

namespace pcl
{

// ----------------------------------------------------------------------------

void GraphWebView::SetDataset( const String& dataname, DataPointVector* dataset )
{
   String coreSrcDir = PixInsightSettings::GlobalString ( "Application/SrcDirectory" );

   // Find Median and MAD of the values
   Array<double> values( dataset->Length() );
   for ( int i = 0; i < dataset->Length(); ++i )
      values[i] = dataset->At( i )->data;
   double median, meanDev;
   MeasureUtils::MedianAndMeanDeviation( values, median, meanDev );

   // Create the Graphing arrays and backend objects
   String graphingArray = "[ ";
   String indexedApprovals = "{ ";
   String indexedLocks = "{ ";
   String indexedSigmas = "{ ";
   for ( DataPointVector::const_iterator i = dataset->Begin(); i != dataset->End(); ++i ) {
      double dataSigma = MeasureUtils::DeviationNormalize( i->data, median, meanDev );
      graphingArray += String().Format(
              "[ %i, [%.4f, %.4f, %.4f], [%.4f, %.4f, %.4f], [%.4f, %.4f, %.4f], [%.4f, %.4f, %.4f] ], ",
              i->x,
              i->weight, i->weight, i->weight,
              median - meanDev, median, median + meanDev,
              median - meanDev*2, median, median + meanDev*2,
              i->data, i->data, i->data
      );
      indexedLocks += String().Format( "%i:%s, ", i->x, i->locked ? "true" : "false" );
      indexedApprovals += String().Format( "%i:%s, ", i->x, i->approved ? "true" : "false" );
      indexedSigmas += String().Format( "%i:%.4f, ", i->x, dataSigma );
   }
   indexedSigmas += "}";
   indexedLocks += "}";
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
   /* No white background, makes it appear more like a normal control */
   html, body {
      background-color: rgba(0,0,0,0);
   }

   /* Fit the graph within the confines of the view */
   #graph {
      height: 92vh;
      weight: 80vw;
   }

   /* Override the ugly legend for something more like a PI Tooltip */
   .dygraph-legend {
      font-size: 13px;
      width: auto;
      padding: 1px;
      background-color: black;
      border: 1px solid gray;
      border-radius: 2px;
   }

   /* Fix the table too */
   table, tr, tr:nth-child(even), tr:nth-child(odd), td {
      padding: 2px;
      background-color: black;
      color: white;
      border: 0px solid black;
      border-collapse: collapse;
   }
   td:first-child {
      font-weight: bold;
      text-align: right;
   }

   /* Shrink the axis labels a little to prevent overlap */
   .dygraph-axis-label {
      font-size: 11px;
   }
</style>
</head>
<body>
<div id="graph"></div>
<script type="text/javascript">

   // Some items are stored in Objects (hashmap-like) for quick lookup
   var approvals = )DELIM" + indexedApprovals + R"DELIM(;
   var locks = )DELIM" + indexedLocks + R"DELIM(;
   var sigmas = )DELIM" + indexedSigmas + R"DELIM(;

   // This is the Array of data that will be graphed
   var dataset = )DELIM" + graphingArray + R"DELIM(;

   // Custom Legend Formatting to lookup attributes and hide some series
   function legendFormatter(data) {
      let html = '<table>';
      data.series.forEach(function(series) {
         if (series.labelHTML.indexOf('NOLEGEND') === -1)
            html += '<tr><td>' + series.labelHTML + ':</td><td>' + series.yHTML + '</td></tr>';
      });
      html += '<tr><td>Sigma:</td><td>' + sigmas[data.x] + '</td></tr>';
      html += '</table>';
      return html;
   }

   // The main series can be drawn to indicate approved and locked frames
   function drawApprovedPoint(g, series, ctx, cx, cy, color, radius, idx) {
      ctx.save();

      ctx.lineCap = "round";
      ctx.lineJoin = "round";

      if (approvals[dataset[idx][0]] === false) {
         ctx.strokeStyle = "red";
         ctx.lineWidth = 3;

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

      if (locks[dataset[idx][0]] === true) {
         ctx.strokeStyle = "black";
         ctx.lineWidth = 1;

         ctx.beginPath();
         ctx.arc(cx, cy, radius*2, 0, 2 * Math.PI, false);
         ctx.closePath();
         ctx.stroke();
      }

      ctx.restore()
   }

   var g = new Dygraph(
      document.getElementById("graph"), dataset,
      {
         ylabel: ")DELIM" + dataname + R"DELIM(",
         y2label: " Weight",

         labels: [
            "Index",
            " Weight",
            " Median", " Median NOLEGEND",
            ")DELIM" + dataname + R"DELIM(",
         ],
         labelsSeparateLines: true,


         series: {
            " Weight": {
               axis: "y2",
               strokeWidth: 1,
               strokePattern: Dygraph.DASHED_LINE,
               color: "#7A7A7A",
            },
            " Median": {
               axis: "y",
               strokeWidth: 1,
               color: "black",
            },
            " Median NOLEGEND": {
               axis: "y",
               strokeWidth: 0,
               color: "black",
            },
            ")DELIM" + dataname + R"DELIM(": {
               axis: "y",
               drawPoints: true,
               drawPointCallback: drawApprovedPoint,
               strokeWidth: 3,
               color: "#4394E5",
            },
         },

         // Separate the Axes' Grids
         axes: {
            y: {
               independentTicks: true,
            },
            y2: {
               independentTicks: true,
               drawGrid: false,
            },
         },

         digitsAfterDecimal: 4,

         // Legend acts more like a tooltip
         legend: "follow",
         legendFormatter: legendFormatter,

         // Error bars are used to show MAD across the Median
         customBars: true,
         errorBars: true,
         fillAlpha: 0.08,

         // Sizing and padding
         pointSize: 4,
         xRangePad: 10,
         yRangePad: 60,
         rightGap: 20,
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
