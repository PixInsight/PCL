//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.02.01.0002
// ----------------------------------------------------------------------------
// GraphWebView.cpp - Released 2017-11-05T16:00:00Z
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
#include "SubframeSelectorMeasureData.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GraphWebView::GraphWebView( Control& parent ) : WebView( parent ), eventCheckTimer(), keepChecking( true )
{
   eventCheckTimer.SetInterval( 0.1 );
   eventCheckTimer.SetPeriodic();

   eventCheckTimer.OnTimer( (Timer::timer_event_handler)&GraphWebView::__Timer, *this );

   OnEnter( (Control::event_handler)&GraphWebView::__MouseEnter, *this );
   OnLeave( (Control::event_handler)&GraphWebView::__MouseLeave, *this );

   OnScriptResultAvailable( (WebView::result_event_handler)&GraphWebView::__JSResult, *this );
}

// ----------------------------------------------------------------------------

void GraphWebView::__MouseEnter( Control& sender )
{
   keepChecking = true;
   if ( !eventCheckTimer.IsRunning() )
      eventCheckTimer.Start();
}

void GraphWebView::__MouseLeave( Control& sender )
{
   keepChecking = false;
}

// ----------------------------------------------------------------------------

void GraphWebView::__Timer( Timer& sender )
{
   EvaluateScript( "getApprovalIndex()", "JavaScript" );
   EvaluateScript( "getLockIndex()", "JavaScript" );
}

// ----------------------------------------------------------------------------

void GraphWebView::__JSResult( WebView& sender, const Variant& result )
{
   if ( !result.IsValid() )
      throw Error( "Graph Error: Invalid script execution" );

   String resultText = result.ToString();
   if ( resultText.Contains( "Error" ) )
      throw Error( resultText );

   bool approve = resultText.StartsWith("A:");
   bool unlock = resultText.StartsWith("U:");
   resultText.DeleteLeft( 2 );

   int index = resultText.ToInt();
   if ( index <= 0 )
   {
      if ( !keepChecking && eventCheckTimer.IsRunning() )
         eventCheckTimer.Stop();
      return;
   }

   if ( approve && !eventHandlers.IsNull() && eventHandlers->onApprove != nullptr )
      (eventHandlers->onApproveReceiver->*eventHandlers->onApprove)( *this, index );
   if ( unlock && !eventHandlers.IsNull() && eventHandlers->onUnlock != nullptr )
      (eventHandlers->onUnlockReceiver->*eventHandlers->onUnlock)( *this, index );
}

// ----------------------------------------------------------------------------

void GraphWebView::OnApprove( approve_event_handler handler, Control& receiver )
{
   if ( eventHandlers.IsNull() )
      eventHandlers = new EventHandlers;
   eventHandlers->onApprove = handler;
   eventHandlers->onApproveReceiver = &receiver;
}

void GraphWebView::OnUnlock( unlock_event_handler handler, Control& receiver )
{
   if ( eventHandlers.IsNull() )
      eventHandlers = new EventHandlers;
   eventHandlers->onUnlock = handler;
   eventHandlers->onUnlockReceiver = &receiver;
}

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

   // Store the graph later globaly for access
   graph = null;

   // Some items are stored in Objects (hashmap-like) for quick lookup
   approvals = )DELIM" + indexedApprovals + R"DELIM(;
   locks = )DELIM" + indexedLocks + R"DELIM(;
   sigmas = )DELIM" + indexedSigmas + R"DELIM(;

   // This is the Array of data that will be graphed
   dataset = )DELIM" + graphingArray + R"DELIM(;

   // For the parent controls to communicate with us, we'll
   // store the indices that have been selected with this interface
   // and return them with this function call
   approvalQueue = [];
   lockQueue = [];
   function getApprovalIndex() {
      if (approvalQueue.length <= 0) return 'A:' + -1;
      return 'A:' + approvalQueue.shift();
   }
   function getLockIndex() {
      if (lockQueue.length <= 0) return 'U:' + -1;
      return 'U:' + lockQueue.shift();
   }

   // When points are clicked, put them in the Queues
   // Update the graph too for more immediate feedback
   function pointClickCallback(event, point) {
      if (event.shiftKey) {
         if (locks.hasOwnProperty(point.xval) && locks[point.xval] === true) {
            lockQueue.push(point.xval);
            locks[point.xval] = false;
         }
      } else {
         approvalQueue.push(point.xval);
         if (approvals.hasOwnProperty(point.xval))
            approvals[point.xval] = !approvals[point.xval];
         if (locks.hasOwnProperty(point.xval))
            locks[point.xval] = true;
      }
      graph.updateOptions({}); // force redraw
   }

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
         ctx.moveTo(cx + radius/2, cy + radius/2);
         ctx.lineTo(cx - radius/2, cy - radius/2);
         ctx.closePath();
         ctx.stroke();

         ctx.beginPath();
         ctx.moveTo(cx + radius/2, cy - radius/2);
         ctx.lineTo(cx - radius/2, cy + radius/2);
         ctx.closePath();
         ctx.stroke();
      }

      if (locks[dataset[idx][0]] === true) {
         ctx.strokeStyle = "black";
         ctx.lineWidth = 1;

         ctx.beginPath();
         ctx.arc(cx, cy, radius, 0, 2 * Math.PI, false);
         ctx.closePath();
         ctx.stroke();
      }

      ctx.restore()
   }

   graph = new Dygraph(
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
               strokeWidth: 2,
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

         // Need to know which points were clicked
         pointClickCallback: pointClickCallback,

         // Legend acts more like a tooltip
         legend: "follow",
         legendFormatter: legendFormatter,

         // Error bars are used to show MAD across the Median
         customBars: true,
         errorBars: true,
         fillAlpha: 0.08,

         // Sizing and padding
         pointSize: 6,
         highlightCircleSize: 4,
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
// EOF GraphWebView.cpp - Released 2017-11-05T16:00:00Z
