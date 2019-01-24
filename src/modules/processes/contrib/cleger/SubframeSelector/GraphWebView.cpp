//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.02.0025
// ----------------------------------------------------------------------------
// GraphWebView.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

GraphWebView::GraphWebView( Control& parent ) :
   WebView( parent )
{
   m_eventCheckTimer.SetInterval( 0.1 );
   m_eventCheckTimer.SetPeriodic();
   m_eventCheckTimer.OnTimer( (Timer::timer_event_handler)&GraphWebView::__Timer, *this );

   OnEnter( (Control::event_handler)&GraphWebView::__MouseEnter, *this );
   OnLeave( (Control::event_handler)&GraphWebView::__MouseLeave, *this );

   OnScriptResultAvailable( (WebView::result_event_handler)&GraphWebView::__JSResult, *this );
}

// ----------------------------------------------------------------------------

void GraphWebView::__MouseEnter( Control& sender )
{
   m_keepChecking = true;
   if ( !m_eventCheckTimer.IsRunning() )
      if ( !m_eventHandlers.IsNull() )
         if ( m_eventHandlers->onApprove != nullptr || m_eventHandlers->onUnlock != nullptr )
            m_eventCheckTimer.Start();
}

// ----------------------------------------------------------------------------

void GraphWebView::__MouseLeave( Control& sender )
{
   m_keepChecking = false;
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

   bool approve = resultText.StartsWith( "A:" );
   bool unlock = resultText.StartsWith( "U:" );
   resultText.DeleteLeft( 2 );

   int index = resultText.ToInt();
   if ( index <= 0 )
   {
      if ( !m_keepChecking )
         m_eventCheckTimer.Stop();
   }
   else
   {
      if ( !m_eventHandlers.IsNull() )
      {
         if ( approve )
            if ( m_eventHandlers->onApprove != nullptr )
               (m_eventHandlers->onApproveReceiver->*m_eventHandlers->onApprove)( *this, index );
         if ( unlock )
            if ( m_eventHandlers->onUnlock != nullptr )
               (m_eventHandlers->onUnlockReceiver->*m_eventHandlers->onUnlock)( *this, index );
      }
   }
}

// ----------------------------------------------------------------------------

void GraphWebView::OnApprove( approve_event_handler handler, Control& receiver )
{
   if ( m_eventHandlers.IsNull() )
      m_eventHandlers = new EventHandlers;
   m_eventHandlers->onApprove = handler;
   m_eventHandlers->onApproveReceiver = &receiver;
}

void GraphWebView::OnUnlock( unlock_event_handler handler, Control& receiver )
{
   if ( m_eventHandlers.IsNull() )
      m_eventHandlers = new EventHandlers;
   m_eventHandlers->onUnlock = handler;
   m_eventHandlers->onUnlockReceiver = &receiver;
}

// ----------------------------------------------------------------------------

void GraphWebView::SetDataset( const String& dataname, const DataPointVector* dataset )
{
   int length = int( dataset->Length() );

   // Sort the dataset by X values to ensure a proper line
   DataPointVector datasetSortedX = dataset->Sorted( DataPointSortingBinaryPredicate( false/*y*/ ) );
   // Sort the dataset by Y values to make distribution graphs
   DataPointVector datasetSortedY = dataset->Sorted( DataPointSortingBinaryPredicate( true/*y*/ ) );

   // Find Median and MAD of the values
   Array<double> values( length );
   for ( int i = 0; i < length; ++i )
      values[i] = dataset->At( i )->data;
   double median, meanDev;
   MeasureUtils::MedianAndMeanDeviation( values, median, meanDev );

   // Find Min, Max, and Range of the values
   double min = 0;
   double max = 0;
   if ( !dataset->IsEmpty() )
   {
      min = datasetSortedY.Begin()->data;
      max = datasetSortedY.At( datasetSortedY.Length() - 1 )->data;
   }
   double range = max - min;

   // Determine the # of bins and their width
   int bins = 0;
   double binRange = 0;
   if ( !dataset->IsEmpty() )
   {
      bins = pcl::Sqrt( length );
      binRange = range / bins;
   }

   // Create initial bin X values
   DataPointVector datasetBinned( bins );
   for ( int b = 0; b < bins; ++b )
   {
      datasetBinned[b].x = min + binRange/2 + binRange * b; // Center the bin value
      datasetBinned[b].data = 0;
   }
   // Sort Y values into bins
   double maxBins = 0;
   for ( const DataPoint& p : *dataset )
   {
      int b = pcl::Min( bins - 1, pcl::RoundInt( pcl::Floor( (p.data - min) / binRange ) ) );
      datasetBinned[b].data += 1;
      maxBins = pcl::Max( maxBins, datasetBinned[b].data );
   }

   // Create initial Empirical Distribution Function vector
   DataPointArray datasetEDF = DataPointArray();
   double currentEDF = 0;
   for ( const DataPoint& p : datasetSortedY )
   {
      currentEDF += 1.0/length;

      if ( !datasetEDF.IsEmpty() && datasetEDF.At( datasetEDF.Length()-1 )->x == p.data )
      {
         // For extremely close values, add to the previous value
         datasetEDF.At( datasetEDF.Length()-1 )->data = currentEDF;
      }
      else
      {
         // Otherwise, add a new point at the value and with one step up
         DataPoint dataPoint = DataPoint();
         dataPoint.x = p.data;
         dataPoint.data = currentEDF;
         datasetEDF.Append( dataPoint );
      }
   }

   // Create the Graphing arrays and backend objects (Plots)
   String graphingArray = "[ ";
   String indexedApprovals = "{ ";
   String indexedLocks = "{ ";
   String indexedSigmas = "{ ";
   for ( const DataPoint& p : datasetSortedX )
   {
      double dataSigma = DeviationNormalize( p.data, median, meanDev );
      graphingArray += String().Format(
              "[ %.0f, [%.4f, %.4f, %.4f], [%.4f, %.4f, %.4f], [%.4f, %.4f, %.4f], [%.4f, %.4f, %.4f] ], ",
              p.x,
              p.weight, p.weight, p.weight,
              median - meanDev, median, median + meanDev,
              median - meanDev*2, median, median + meanDev*2,
              p.data, p.data, p.data
      );
      indexedLocks += String().Format( "%.0f:%s, ", p.x, p.locked ? "true" : "false" );
      indexedApprovals += String().Format( "%.0f:%s, ", p.x, p.approved ? "true" : "false" );
      indexedSigmas += String().Format( "%.0f:%.4f, ", p.x, dataSigma );
   }
   indexedSigmas += "}";
   indexedLocks += "}";
   indexedApprovals += "}";
   graphingArray += "]";

   // Create the Graphing arrays and backend objects (Histograph)
   String histographingArray = "[ ";
   // Master loop through the Histogram bins
   for ( const DataPoint& b : datasetBinned )
   {
      // First, look for EDF values that come before this bin
      int eAdded = 0;
      int eMatched = -1;
      for ( int e = 0; e != int( datasetEDF.Length() ); ++e )
      {
         const DataPoint* edf = datasetEDF.At( e );

         if ( edf->x < b.x ) // EDF < Bin, add the EDF and remove it later
         {
            histographingArray += String().Format( "[ %.8f, null, %.4f ], ", edf->x, edf->data );
            ++eAdded;
         }
         else if ( edf->x == b.x ) // EDF = Bin, mark the EDF to be merged and remove it later
         {
            eMatched = e;
            ++eAdded;
         }
         else // Sorted array means there's no more EDFs before this Bin
            break;
      }

      // If this point aligns with an EDF value, create that joint point
      if ( eMatched >= 0 )
         histographingArray += String().Format( "[ %.8f, %.0f, %.4f ], ", b.x, b.data, datasetEDF[eMatched].data );
      else // Add this bin alone
         histographingArray += String().Format( "[ %.8f, %.0f, null ], ", b.x, b.data );

      // Remove the EDF values that were already added
      if ( eAdded > 0 )
         datasetEDF.Remove( datasetEDF.Begin(), eAdded );
   }
   // Add any EDF values that weren't already
   for ( const DataPoint& e : datasetEDF )
      histographingArray += String().Format( "[ %.8f, null, %.4f ], ", e.x, e.data );
   histographingArray += "]";

   String html = Header() + R"DELIM(
<div id="graph"></div>
<div id="histograph"></div>
<script type="text/javascript">

   // Store the graphs later globally for access
   graph = null;
   histograph = null;

   // Some items are stored in Objects (hashmap-like) for quick lookup
   approvals = )DELIM" + indexedApprovals + R"DELIM(;
   locks = )DELIM" + indexedLocks + R"DELIM(;
   sigmas = )DELIM" + indexedSigmas + R"DELIM(;

   // This is the Array of data that will be graphed
   datasetValues = )DELIM" + graphingArray + R"DELIM(;
   datasetHistogram = )DELIM" + histographingArray + R"DELIM(;

   // Helpful information for the legend
   datasetMin = )DELIM" + String().Format( "%.8f", min ) + R"DELIM(;
   datasetMax = )DELIM" + String().Format( "%.8f", max ) + R"DELIM(;
   datasetRange = )DELIM" + String().Format( "%.8f", range ) + R"DELIM(;
   datasetBinRange = )DELIM" + String().Format( "%.8f", binRange ) + R"DELIM(;
   datasetMaxValue = )DELIM" + String().Format( "%.8f", maxBins ) + R"DELIM(;

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
   function legendFormatterHistograph(data) {
      let html = '<center>';
      let hasFirstLine = false;
      data.series.forEach(function(series) {
         if (series.labelHTML === "Count" && series.yHTML !== undefined) {
            html += '<strong>' + series.labelHTML + '</strong><br/>';

            html += (data.x - datasetBinRange/2.0).toFixed(4);
            html += ' - ';
            html += (data.x + datasetBinRange/2.0).toFixed(4);
            html += '<br/>';

            html += '<strong>' + series.yHTML + '</strong>';
            hasFirstLine = true;
         }
         if (series.labelHTML === "Probability" && series.yHTML !== undefined) {
            if (hasFirstLine)
               html += '<br/>';
            html += '<strong>' + series.labelHTML + '</strong><br/>';

            html += '< ' + data.x.toFixed(4) + '<br/>';
            html += '<strong>' + (series.y * 100.0).toFixed(1) + '%</strong>';
         }
      });
      html += '</center>';
      return html;
   }

   // The main series can be drawn to indicate approved and locked frames
   function drawApprovedPoint(g, series, ctx, cx, cy, color, radius, idx) {
      ctx.save();

      ctx.lineCap = "round";
      ctx.lineJoin = "round";

      if (approvals[datasetValues[idx][0]] === false) {
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

      if (locks[datasetValues[idx][0]] === true) {
         ctx.strokeStyle = "black";
         ctx.lineWidth = 1;

         ctx.beginPath();
         ctx.arc(cx, cy, radius, 0, 2 * Math.PI, false);
         ctx.closePath();
         ctx.stroke();
      }

      ctx.restore()
   }

   // Custom Plotter to show bars instead of a line
   function barChartPlotter(e) {
      var ctx = e.drawingContext;
      var points = e.points;
      var y_bottom = e.dygraph.toDomYCoord(0);  // see http://dygraphs.com/jsdoc/symbols/Dygraph.html#toDomYCoord

      // Find the bar width from only points that have values
      var point1 = undefined;
      var point2 = undefined;
      for (var i = 0; i < points.length; i++) {
         if (points[i].yval !== null && point1 === undefined) {
            point1 = points[i];
            continue;
         }
         if (points[i].yval !== null && point2 === undefined) {
            point2 = points[i];
            break;
         }
      }
      if (point1 == undefined || point2 == undefined)
         return;
      var bar_width = 0.9 * (point2.canvasx - point1.canvasx);

      ctx.fillStyle = e.color;

      // Do the actual plotting.
      for (var i = 0; i < points.length; i++) {
         var p = points[i];
         var center_x = p.canvasx;  // center of the bar

         ctx.fillRect(center_x - bar_width / 2.0, p.canvasy,
            bar_width, y_bottom - p.canvasy);
         ctx.strokeRect(center_x - bar_width / 2.0, p.canvasy,
            bar_width, y_bottom - p.canvasy);
      }
   }

   graph = new Dygraph(
      document.getElementById("graph"), datasetValues,
      {
         title: " ", // empty space above graph for our own labels
         xlabel: "Index",

         labels: [
            "Index",
            " Weight", // if Weight is graphed, we need a unique name for this too
            " Median", " Median NOLEGEND", // above, plus the 'hidden' 2x Sigma version
            ")DELIM" + dataname + R"DELIM(",
         ],

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

   histograph = new Dygraph(
      document.getElementById("histograph"), datasetHistogram,
      {
         title: " ", // empty space above graph for our own labels
         xlabel: ")DELIM" + dataname + R"DELIM(",

         labels: [
            ")DELIM" + dataname + R"DELIM(",
            "Count",
            "Probability",
         ],

         connectSeparatedPoints: true,
         series: {
            "Count": {
               axis: "y",
               color: "#4394E5",
               plotter: barChartPlotter,
            },
            "Probability": {
               axis: "y2",
               strokeWidth: 1,
               color: "#333333",
               stepPlot: true,
            },
         },

         // Separate the Axes' Grids
         axes: {
            y: {
               independentTicks: true,
               valueRange: [0, datasetMaxValue*1.1],
            },
            y2: {
               independentTicks: true,
               drawGrid: false,
               valueRange: [0, 1],
            },
         },

         digitsAfterDecimal: 6,

         // Legend acts more like a tooltip
         legend: "follow",
         legendFormatter: legendFormatterHistograph,

         // Sizing and padding
         xRangePad: 10,
         yRangePad: 0,
         dateWindow: [datasetMin - datasetBinRange/3, datasetMax + datasetBinRange/3],
      }
   );

   // Dygraphs replaces the div content, but we want our own labels, so add them afterwards
   var node = document.createElement("div");
   var nodeClass = document.createAttribute("class");
   nodeClass.value = "label-y1";
   node.attributes.setNamedItem(nodeClass);
   node.appendChild(document.createTextNode(")DELIM" + dataname + R"DELIM("));
   document.getElementById("graph").appendChild(node);

   node = document.createElement("div");
   nodeClass = document.createAttribute("class");
   nodeClass.value = "label-y2";
   node.attributes.setNamedItem(nodeClass);
   node.appendChild(document.createTextNode("Weight"));
   document.getElementById("graph").appendChild(node);

   node = document.createElement("div");
   nodeClass = document.createAttribute("class");
   nodeClass.value = "label-y1";
   node.attributes.setNamedItem(nodeClass);
   node.appendChild(document.createTextNode("Count"));
   document.getElementById("histograph").appendChild(node);

   node = document.createElement("div");
   nodeClass = document.createAttribute("class");
   nodeClass.value = "label-y2";
   node.attributes.setNamedItem(nodeClass);
   node.appendChild(document.createTextNode("Probability"));
   document.getElementById("histograph").appendChild(node);

</script>
   )DELIM" + Footer();

   /*
    * N.B. On Windows with relative frequency, and rarely on Linux and macOS,
    * setting contents directly by calling SetHTML() fails, causing the graphs
    * to be empty and leading to an 'invalid script execution' error after this
    * function. This is probably a bug in QTWebEngine, which we have seen in
    * other places with several Qt versions. As happens with most Qt bugs, we
    * cannot rely on the hope of someone fixing this problem within a
    * manageable amount of time (years, maybe), so here is a workaround: save
    * the document to a local file and force loading it. This works reliably on
    * all platforms, fortunately.
    */
   // SetHTML( html.ToUTF8() );
   Cleanup();
   m_htmlFilePath = File::UniqueFileName( File::SystemTempDirectory(), 12, "SFS_graphs_", ".html" );
   File::WriteTextFile( m_htmlFilePath, html.ToUTF8() );
   LoadContent( File::FileURI( m_htmlFilePath ) );
}

// ----------------------------------------------------------------------------

String GraphWebView::Header() const
{
   String coreSrcDir = PixInsightSettings::GlobalString( "Application/SrcDirectory" );

   return R"DELIM(
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
      overflow: hidden;
   }

   /* Fit the graph within the confines of the view */
   #graph {
      position: fixed;
      top: 0px;
      left: -15px;
      height: 100vh;
      width: 67vw;
   }
   #histograph {
      position: fixed;
      top: 0px;
      right: -15px;
      height: 100vh;
      width: 37vw;
   }

   /* Create axis labels at the top to save horizontal space */
   .label-y1, .label-y2 {
      position: absolute;
      top: 0px;
      font-size: 12px;
      font-weight: bold;
   }
   .label-y1 {
      left: 40px;
   }
   .label-y2 {
      right: 60px;
   }

   /* Override the ugly legend for something more like a PI Tooltip */
   .dygraph-legend {
      font-size: 13px;
      width: auto;
      padding: 1px;
      background-color: black;
      color: white;
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
   )DELIM";
}

// ----------------------------------------------------------------------------

String GraphWebView::Footer() const
{
   return R"DELIM(
</body>
</html>
   )DELIM";
}

// ----------------------------------------------------------------------------

void GraphWebView::Cleanup()
{
   if ( !m_htmlFilePath.IsEmpty() )
   {
      try
      {
         if ( File::Exists( m_htmlFilePath ) )
            File::Remove( m_htmlFilePath );
      }
      catch ( ... )
      {
      }
      m_htmlFilePath.Clear();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GraphWebView.cpp - Released 2019-01-21T12:06:42Z
