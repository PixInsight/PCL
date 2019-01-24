//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.02.0025
// ----------------------------------------------------------------------------
// SubframeSelectorMeasurementsInterface.cpp - Released 2019-01-21T12:06:42Z
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

#include <pcl/Console.h>
#include <pcl/FileDialog.h>
#include <pcl/MetaModule.h>
#include <pcl/StdStatus.h>

#include "SubframeSelectorExpressionsInterface.h"
#include "SubframeSelectorMeasurementsInterface.h"
#include "SubframeSelectorProcess.h"

#define IMAGELIST_MINHEIGHT( fnt )  (12*fnt.Height() + 2)

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "SubframeSelectorIcon.xpm"

// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface* TheSubframeSelectorMeasurementsInterface = nullptr;

// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface::SubframeSelectorMeasurementsInterface( SubframeSelectorInstance& instance ) :
   m_instance( instance )
{
   TheSubframeSelectorMeasurementsInterface = this;
}

// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface::~SubframeSelectorMeasurementsInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorMeasurementsInterface::Id() const
{
   return "SubframeSelectorMeasurements";
}

// ----------------------------------------------------------------------------

MetaProcess* SubframeSelectorMeasurementsInterface::Process() const
{
   return TheSubframeSelectorProcess;
}

// ----------------------------------------------------------------------------

const char** SubframeSelectorMeasurementsInterface::IconImageXPM() const
{
   return nullptr; // SubframeSelectorIcon_XPM; ---> put a nice icon here
}

// ----------------------------------------------------------------------------

InterfaceFeatures SubframeSelectorMeasurementsInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorMeasurementsInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorMeasurementsInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorMeasurementsInterface::Launch( const MetaProcess&,
                                                    const ProcessImplementation*,
                                                    bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "SubframeSelector | Measurements" );
      UpdateControls();
   }

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::Cleanup()
{
   if ( GUI != nullptr )
   {
      GUI->MeasurementGraph_Graph.Cleanup();
      GUI->MeasurementDistribution_Graph.Cleanup();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

MeasureItem* SubframeSelectorMeasurementsInterface::GetMeasurementItem( size_type i )
{
   TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
   if ( node == nullptr )
      return nullptr;

   int index;
   if ( !node->Text( 0 ).TryToInt( index ) )
      return nullptr;

   // 1-based index, 0-based arrays
   if ( index > 0 ) // ?!
      --index;
   return &m_instance.p_measures[index];
}

// ----------------------------------------------------------------------------

TreeBox::Node* SubframeSelectorMeasurementsInterface::GetMeasurementNode( MeasureItem* item )
{
   if ( item == nullptr )
      return nullptr;

   for ( int i = 0; i < GUI->MeasurementTable_TreeBox.NumberOfChildren(); ++i )
   {
      TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
      if ( node == nullptr )
         return nullptr;

      int index;
      if ( !node->Text( 0 ).TryToInt( index ) )
         return nullptr;

      if ( index == item->index )
         return node;
   }

   return nullptr;
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateControls()
{
   UpdateMeasurementImagesList();
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementQuantity()
{
   // Update the table and gather quantities
   int approved = 0;
   int locked = 0;
   size_type amount = m_instance.p_measures.Length();
   if ( amount > 0 )
   {
      for ( Array<MeasureItem>::const_iterator i = m_instance.p_measures.Begin(); i < m_instance.p_measures.End(); ++i )
      {
         if ( i->enabled )
            ++approved;
         if ( i->locked )
            ++locked;
      }

      GUI->MeasurementsTable_Quantities_Label.SetText(
         String().Format( "%i/%i Approved (%.2f%%), %i Locked (%.2f%%)",
                          approved, amount, 100.0*approved/amount,
                          locked, 100.0*locked/amount ) );
   }
   else
   {
      GUI->MeasurementsTable_Quantities_Label.SetText( "0/0 Approved (0%), 0 Locked (0%)" );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementImageItem( size_type i, MeasureItem* item )
{
   TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
   if ( node == nullptr )
      return;
   if ( item == nullptr )
      return;

   node->SetText( 0, String().Format( "%3i", item->index ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item->enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( item->locked ? ":/file-explorer/read-only.png" : ":/icons/unlock.png" ) ) );
   node->SetAlignment( 2, TextAlign::Left );

   node->SetIcon( 3, Bitmap( ScaledResource( ":/browser/picture.png" ) ) );
   node->SetText( 3, File::ExtractNameAndSuffix( item->path ) );
   node->SetToolTip( 3, item->path );
   node->SetAlignment( 3, TextAlign::Left );

   node->SetText( 4, String().Format( "%.03f", item->weight ) );
   node->SetAlignment( 4, TextAlign::Center );

   node->SetText( 5, String().Format( "%.03f", item->FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit ) ) );
   node->SetAlignment( 5, TextAlign::Center );

   node->SetText( 6, String().Format( "%.03f", item->eccentricity ) );
   node->SetAlignment( 6, TextAlign::Center );

   node->SetText( 7, String().Format( "%.04f", item->snrWeight ) );
   node->SetAlignment( 7, TextAlign::Center );

   node->SetText( 8, String().Format( "%.03f", item->Median( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ) ) );
   node->SetAlignment( 8, TextAlign::Center );

   node->SetText( 9, String().Format( "%.03f", item->MedianMeanDev( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ) ) );
   node->SetAlignment( 9, TextAlign::Center );

   node->SetText( 10, String().Format( "%.03f",
            item->Noise( m_instance.p_cameraGain,
                         TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                         m_instance.p_dataUnit ) ) );
   node->SetAlignment( 10, TextAlign::Center );

   node->SetText( 11, String().Format( "%.04f", item->noiseRatio ) );
   node->SetAlignment( 11, TextAlign::Center );

   node->SetText( 12, String().Format( "%i", item->stars ) );
   node->SetAlignment( 12, TextAlign::Center );

   node->SetText( 13, String().Format( "%.03f", item->starResidual ) );
   node->SetAlignment( 13, TextAlign::Center );

   node->SetText( 14, String().Format( "%.04f", item->FWHMMeanDeviation( m_instance.p_subframeScale,
                                                                         m_instance.p_scaleUnit ) ) );
   node->SetAlignment( 14, TextAlign::Center );

   node->SetText( 15, String().Format( "%.04f", item->eccentricityMeanDev ) );
   node->SetAlignment( 15, TextAlign::Center );

   node->SetText( 16, String().Format( "%.04f", item->starResidualMeanDev ) );
   node->SetAlignment( 16, TextAlign::Center );
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementImagesList()
{
   GUI->MeasurementsTable_SortingProperty_Control.SetCurrentItem( m_instance.p_sortingProperty );

   // Ensure Approval/Weighting is updated
   TheSubframeSelectorExpressionsInterface->ApplyWeightingExpression();
   TheSubframeSelectorExpressionsInterface->ApplyApprovalExpression();

   GUI->MeasurementTable_TreeBox.DisableUpdates();

   // Removing/Adding all the items can be slow; try to update each items text if possible
   bool shouldRecreate = m_instance.p_measures.Length()
                         != size_type( GUI->MeasurementTable_TreeBox.NumberOfChildren() );

   // Store all current selections to re-select later
   const IndirectArray<TreeBox::Node>& selections = GUI->MeasurementTable_TreeBox.SelectedNodes();
   Array<int> currentIds( selections.Length() );
   for ( size_type i = 0; i < selections.Length(); ++i )
      currentIds[i] = GUI->MeasurementTable_TreeBox.ChildIndex( selections[i] );

   // When the number of items changes, it's easier to add all items over again
   if ( shouldRecreate )
      GUI->MeasurementTable_TreeBox.Clear();

   // Ensure items are sorted properly
   Array<MeasureItem> measuresSorted( m_instance.p_measures );
   measuresSorted.Sort( SubframeSortingBinaryPredicate( m_instance.p_sortingProperty,
                                    GUI->MeasurementsTable_SortingMode_Control.CurrentItem() ) );

   // Update the table
   size_type amount = measuresSorted.Length();
   for ( size_type i = 0; i < amount; ++i )
   {
      if ( shouldRecreate )
         new TreeBox::Node( GUI->MeasurementTable_TreeBox );
      UpdateMeasurementImageItem( i, &measuresSorted[i] );
   }

   GUI->MeasurementTable_TreeBox.AdjustColumnWidthToContents( 3 ); // filename

   // If the table was cleared, setup previous selections
   if ( shouldRecreate && !m_instance.p_measures.IsEmpty() )
      for ( size_type i = 0; i < currentIds.Length(); ++i )
         if ( currentIds[i] >= 0 )
            if ( currentIds[i] < GUI->MeasurementTable_TreeBox.NumberOfChildren() )
               GUI->MeasurementTable_TreeBox.Child( currentIds[i] )->Select();

   GUI->MeasurementTable_TreeBox.EnableUpdates();
   UpdateMeasurementQuantity();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::UpdateMeasurementGraph()
{
   GUI->MeasurementGraph_GraphProperty_Control.SetCurrentItem( m_instance.p_graphProperty );

   DataPointVector dataset( m_instance.p_measures.Length() );
   for ( size_type i = 0; i < m_instance.p_measures.Length(); ++i )
   {
      dataset[i].x = m_instance.p_measures[i].index;
      switch ( m_instance.p_graphProperty )
      {
      case SSGraphProperty::Weight: dataset[i].data = m_instance.p_measures[i].weight;
         break;
      case SSGraphProperty::FWHM:
         dataset[i].data = m_instance.p_measures[i].FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit );
         break;
      case SSGraphProperty::Eccentricity: dataset[i].data = m_instance.p_measures[i].eccentricity;
         break;
      case SSGraphProperty::SNRWeight: dataset[i].data = m_instance.p_measures[i].snrWeight;
         break;
      case SSGraphProperty::Median:
         dataset[i].data = m_instance.p_measures[i].Median( m_instance.p_cameraGain,
                              TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                              m_instance.p_dataUnit );
         break;
      case SSGraphProperty::MedianMeanDev:
         dataset[i].data = m_instance.p_measures[i].MedianMeanDev( m_instance.p_cameraGain,
                              TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                              m_instance.p_dataUnit );
         break;
      case SSGraphProperty::Noise:
         dataset[i].data = m_instance.p_measures[i].Noise( m_instance.p_cameraGain,
                              TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                              m_instance.p_dataUnit );
         break;
      case SSGraphProperty::NoiseRatio: dataset[i].data = m_instance.p_measures[i].noiseRatio;
         break;
      case SSGraphProperty::Stars: dataset[i].data = m_instance.p_measures[i].stars;
         break;
      case SSGraphProperty::StarResidual: dataset[i].data = m_instance.p_measures[i].starResidual;
         break;
      case SSGraphProperty::FWHMMeanDev:
         dataset[i].data = m_instance.p_measures[i].FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit );
         break;
      case SSGraphProperty::EccentricityMeanDev: dataset[i].data = m_instance.p_measures[i].eccentricityMeanDev;
         break;
      case SSGraphProperty::StarResidualMeanDev: dataset[i].data = m_instance.p_measures[i].starResidualMeanDev;
         break;
      default: dataset[i].data = m_instance.p_measures[i].weight;
         break;
      }
      dataset[i].weight = m_instance.p_measures[i].weight;
      dataset[i].approved = m_instance.p_measures[i].enabled;
      dataset[i].locked = m_instance.p_measures[i].locked;
   }

   GUI->MeasurementGraph_Graph.SetDataset( TheSSGraphPropertyParameter->ElementLabel( m_instance.p_graphProperty ),
                                           &dataset );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::SetMeasurements( const Array<MeasureItem>& measures )
{
   m_instance.p_measures = measures;
   UpdateControls();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::ExportCSV() const
{
   SaveFileDialog d;
   d.SetCaption( "SubframeSelector: Save CSV File" );
   d.SetFilter( FileFilter( "CSV", ".csv" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      String filePath = d.FileName();
      Console().WriteLn( "Generating output CSV file: " + filePath );

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Writing CSV file", m_instance.p_measures.Length() );

      IsoStringList lines;
      lines << Module->ReadableVersion()
            << IsoString().Format( "Subframe Scale,%.5f", m_instance.p_subframeScale )
            << IsoString().Format( "Camera Gain,%.5f", m_instance.p_cameraGain )
            << "Camera Resolution,\"" + TheSSCameraResolutionParameter->ElementLabel( m_instance.p_cameraResolution ) + "\""
            << IsoString().Format( "Site Local Midnight,%d", m_instance.p_siteLocalMidnight )
            << "Scale Unit,\"" + TheSSScaleUnitParameter->ElementLabel( m_instance.p_scaleUnit ) + "\""
            << "Data Unit,\"" + TheSSDataUnitParameter->ElementLabel( m_instance.p_dataUnit ) + "\""
            << IsoString().Format( "Structure Layers,%d", m_instance.p_structureLayers )
            << IsoString().Format( "Noise Layers,%d", m_instance.p_noiseLayers )
            << IsoString().Format( "Hot Pixel Filter Radius,%d", m_instance.p_hotPixelFilterRadius )
            << IsoString().Format( "Apply Hot Pixel Filter to Detection Image,%s",
                                    m_instance.p_hotPixelFilter ? "true" : "false" )
            << IsoString().Format( "Noise Reduction Filter Radius,%d", m_instance.p_noiseReductionFilterRadius )
            << IsoString().Format( "Sensitivity,%.5f", m_instance.p_sensitivity )
            << IsoString().Format( "Peak Response,%.3f", m_instance.p_peakResponse )
            << IsoString().Format( "Maximum Star Distortion,%.3f", m_instance.p_maxDistortion )
            << IsoString().Format( "Upper Limit,%.3f", m_instance.p_upperLimit )
            << IsoString().Format( "Background Expansion,%d", m_instance.p_backgroundExpansion )
            << IsoString().Format( "XY Stretch,%.3f", m_instance.p_xyStretch )
            << IsoString().Format( "Pedestal,%d", m_instance.p_pedestal )
            << IsoString().Format( "Subframe Region,%d,%d,%d,%d",
                                    m_instance.p_roi.x0, m_instance.p_roi.y0, m_instance.p_roi.Width(), m_instance.p_roi.Height() )
            << "PSF Fit,\"" + TheSSPSFFitParameter->ElementLabel( m_instance.p_psfFit ) + "\""
            << IsoString().Format( "Circular PSF,%s", m_instance.p_psfFitCircular ? "true" : "false" )
            << "Approval expression,\"" + m_instance.p_approvalExpression + "\""
            << "Weighting expression,\"" + m_instance.p_weightingExpression + "\""
            << "Index,Approved,Locked,File,Weight,FWHM,Eccentricity,SNR Weight,Median,"
               "Median Mean Deviation,Noise,Noise Ratio,Stars,Star Residual,"
               "FWHM Mean Deviation,Eccentricity Mean Deviation,"
               "Star Residual Mean Deviation";

      for ( const MeasureItem& i : m_instance.p_measures )
      {
         lines << IsoString( i.index ) + ',' +
                  (i.enabled ? "true," : "false,") +
                  (i.locked ? "true," : "false,") +
                  '"' + i.path.ToUTF8() + '"' + ',' +
                  IsoString().Format(
                     "%.05f,%.05f,%.05f,%.05f,%.05f,%.05f,%.05f,%.05f,%i,%.05f,%.05f,%.05f,%.05f",
                     i.weight,
                     i.FWHM( m_instance.p_subframeScale, m_instance.p_scaleUnit ),
                     i.eccentricity,
                     i.snrWeight,
                     i.Median( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ),
                     i.MedianMeanDev( m_instance.p_cameraGain,
                                       TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                       m_instance.p_dataUnit ),
                     i.Noise( m_instance.p_cameraGain,
                                 TheSSCameraResolutionParameter->ElementData( m_instance.p_cameraResolution ),
                                 m_instance.p_dataUnit ),
                     i.noiseRatio,
                     i.stars,
                     i.starResidual,
                     i.FWHMMeanDeviation( m_instance.p_subframeScale, m_instance.p_scaleUnit ),
                     i.eccentricityMeanDev,
                     i.starResidualMeanDev );
         ++monitor;
      }

      lines << IsoString(); // make sure the file ends with a newline.

      File::WriteTextFile( filePath, IsoString().ToNewLineSeparated( lines ) );

      monitor.Complete();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::ExportPDF() const
{
   SaveFileDialog d;
   d.SetCaption( "SubframeSelector: Save PDF File" );
   d.SetFilter( FileFilter( "PDF", ".PDF" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      String filePath = d.FileName();
      File f = File::CreateFileForWriting( filePath );
      Console().WriteLn( "Generating output PDF file: " + filePath );

      double defaultWidth = 210;
      double ratio = (double) GUI->MeasurementGraph_Graph.Width() / (double) GUI->MeasurementGraph_Graph.Height();
      GUI->MeasurementGraph_Graph.SaveAsPDF( filePath,
                                             defaultWidth, pcl::Ceil( defaultWidth / ratio ),
                                             0, 0, 0, 0 );

      Console().WriteLn( "Generated PDF file: " + filePath );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
   {
      GUI->MeasurementTable_TreeBox.SetFixedHeight();
      GUI->MeasurementGraph_Control.SetFixedHeight();
   }
   else
   {
      GUI->MeasurementTable_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->MeasurementTable_TreeBox.SetMaxHeight( int_max );
      GUI->MeasurementGraph_Control.SetScaledMinHeight( 360 );
      GUI->MeasurementGraph_Control.SetMaxHeight( int_max );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_CurrentNodeUpdated( TreeBox& sender,
                                                   TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_measures.Length() )
      throw Error( "SubframeSelectorMeasurementsInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_measures.Length() )
      throw Error( "SubframeSelectorMeasurementsInterface: *Warning* Corrupted interface structures" );

   MeasureItem* item = GetMeasurementItem( index );
   if ( item == nullptr )
      return;

   switch ( col )
   {
   case 0:
      // Activate the item's index number: ignore.
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item->enabled = !item->enabled;
      item->locked = true;
      UpdateMeasurementImageItem( index, item );
      UpdateMeasurementQuantity();
      UpdateMeasurementGraph();
      break;
   case 2:
      // Activate the item's checkmark: toggle item's locked state.
      item->locked = !item->locked;
      UpdateMeasurementImageItem( index, item );
      UpdateMeasurementQuantity();
      UpdateMeasurementGraph();
      break;
   case 3:
      {
         // Activate the item's path: open the image.
         Array<ImageWindow> windows = ImageWindow::Open( item->path, IsoString()/*id*/ );
         for ( ImageWindow& window : windows )
            window.Show();
      }
      break;
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->MeasurementsTable_ToggleApproved_PushButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->MeasurementTable_TreeBox[i]->IsSelected() )
         {
            MeasureItem* item = GetMeasurementItem( i );
            if ( item == nullptr )
               continue;

            item->enabled = !item->enabled;
            item->locked = true;
         }
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_ToggleLocked_PushButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->MeasurementTable_TreeBox[i]->IsSelected() )
         {
            MeasureItem* item = GetMeasurementItem( i );
            if ( item == nullptr )
               continue;

            item->locked = !item->locked;
         }
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_Invert_PushButton )
   {
      for ( int i = 0, n = GUI->MeasurementTable_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->MeasurementTable_TreeBox[i]->Select( !GUI->MeasurementTable_TreeBox[i]->IsSelected() );
   }
   else if ( sender == GUI->MeasurementsTable_Remove_PushButton )
   {
      Array<MeasureItem> newMeasures;
      uint16 index = 1;
      for ( size_type i = 0; i < m_instance.p_measures.Length(); ++i )
      {
         MeasureItem* measure = m_instance.p_measures.At( i );
         TreeBox::Node* measureNode = GetMeasurementNode( measure );
         if ( measureNode == nullptr )
            continue;

         if ( !measureNode->IsSelected() )
         {
            measure->index = index;
            newMeasures.Add( *measure );
            ++index;
         }
      }
      m_instance.p_measures = newMeasures;
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_Clear_PushButton )
   {
      m_instance.p_measures.Clear();
      UpdateControls();
   }
   else if ( sender == GUI->MeasurementsTable_ExportCSV_PushButton )
   {
      ExportCSV();
   }
   else if ( sender == GUI->MeasurementGraph_ExportPDF_PushButton )
   {
      ExportPDF();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_GraphApprove( GraphWebView &sender, int& index )
{
   if ( index <= 0 )
      return;

   --index; // 1-based to 0-based

   if ( index >= int( m_instance.p_measures.Length() ) )
      return;

   MeasureItem* item = &m_instance.p_measures[index];
   item->enabled = !item->enabled;
   item->locked = true;

   TreeBox::Node* node = GetMeasurementNode( item );
   if ( node == nullptr )
      return;

   GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
   GUI->MeasurementTable_TreeBox.SetVerticalScrollPosition( GUI->MeasurementTable_TreeBox.NodeRect( node ).y0 );
   UpdateMeasurementImageItem( GUI->MeasurementTable_TreeBox.ChildIndex( node ), item );
   UpdateMeasurementQuantity();
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_GraphUnlock( GraphWebView &sender, int& index )
{
   if ( index <= 0 )
      return;

   --index; // 1-based to 0-based

   if ( index >= int( m_instance.p_measures.Length() ) )
      return;

   MeasureItem* item = &m_instance.p_measures[index];
   item->locked = false;

   TreeBox::Node* node = GetMeasurementNode( item );
   if ( node == nullptr )
      return;

   GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
   GUI->MeasurementTable_TreeBox.SetVerticalScrollPosition( GUI->MeasurementTable_TreeBox.NodeRect( node ).y0 );
   TheSubframeSelectorExpressionsInterface->ApplyApprovalExpression();
   UpdateMeasurementImageItem( GUI->MeasurementTable_TreeBox.ChildIndex( node ), item );
   UpdateMeasurementQuantity();
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorMeasurementsInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->MeasurementsTable_SortingProperty_Control )
   {
      m_instance.p_sortingProperty = itemIndex;
      UpdateMeasurementImagesList();
   }
   else if ( sender == GUI->MeasurementsTable_SortingMode_Control )
   {
      UpdateMeasurementImagesList();
   }
   else if ( sender == GUI->MeasurementGraph_GraphProperty_Control )
   {
      m_instance.p_graphProperty = itemIndex;
      UpdateMeasurementGraph();
   }
}

// ----------------------------------------------------------------------------

SubframeSelectorMeasurementsInterface::GUIData::GUIData( SubframeSelectorMeasurementsInterface& w ) :
   MeasurementGraph_Graph( w ),
   MeasurementDistribution_Graph( w )
{
   MeasurementTable_SectionBar.SetTitle( "Measurements Table" );
   MeasurementTable_SectionBar.SetSection( MeasurementTable_Control );
   MeasurementTable_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ToggleSection, w );

   for ( size_type i = 0; i < TheSSSortingPropertyParameter->NumberOfElements(); ++i )
      MeasurementsTable_SortingProperty_Control.AddItem( TheSSSortingPropertyParameter->ElementLabel( i ) );

   MeasurementsTable_SortingProperty_Control.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementsTable_SortingMode_Control.AddItem( "Asc." );
   MeasurementsTable_SortingMode_Control.AddItem( "Desc." );
   MeasurementsTable_SortingMode_Control.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementsTable_ToggleApproved_PushButton.SetText( "Toggle Approve" );
   MeasurementsTable_ToggleApproved_PushButton.SetToolTip( "<p>Toggle the approved state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleApproved_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_ToggleLocked_PushButton.SetText( "Toggle Lock" );
   MeasurementsTable_ToggleLocked_PushButton.SetToolTip( "<p>Toggle the locked state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleLocked_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Invert_PushButton.SetText( "Invert" );
   MeasurementsTable_Invert_PushButton.SetToolTip( "<p>Invert the current selection of measurements.</p>" );
   MeasurementsTable_Invert_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Remove_PushButton.SetText( "Remove" );
   MeasurementsTable_Remove_PushButton.SetToolTip( "<p>Remove the selected measurements.</p>" );
   MeasurementsTable_Remove_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Clear_PushButton.SetText( "Clear" );
   MeasurementsTable_Clear_PushButton.SetToolTip( "<p>Clear the list of measurements.</p>" );
   MeasurementsTable_Clear_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_ExportCSV_PushButton.SetText( "Save CSV" );
   MeasurementsTable_ExportCSV_PushButton.SetToolTip( "<p>Export the table as a CSV file.</p>" );
   MeasurementsTable_ExportCSV_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementsTable_Top1_Sizer.SetSpacing( 4 );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_SortingProperty_Control );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_SortingMode_Control );
   MeasurementsTable_Top1_Sizer.AddStretch( 100 );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_ToggleApproved_PushButton );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_ToggleLocked_PushButton );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_Invert_PushButton );

   MeasurementsTable_Top2_Sizer.SetSpacing( 4 );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Quantities_Label );
   MeasurementsTable_Top2_Sizer.AddStretch( 100 );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Remove_PushButton );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Clear_PushButton );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_ExportCSV_PushButton );

   MeasurementTable_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( w.Font() ) );
   MeasurementTable_TreeBox.SetScaledMinWidth( 400 );
   MeasurementTable_TreeBox.SetNumberOfColumns( 18 );
   MeasurementTable_TreeBox.SetHeaderText( 0, "Ind." );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 0, 40 );
   MeasurementTable_TreeBox.SetHeaderIcon( 1, Bitmap( w.ScaledResource( ":/icons/picture-ok.png" ) ) );
   MeasurementTable_TreeBox.SetHeaderText( 1, "" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 1, 30 );
   MeasurementTable_TreeBox.SetHeaderIcon( 2, Bitmap( w.ScaledResource( ":/icons/function-import.png" ) ) );
   MeasurementTable_TreeBox.SetHeaderText( 2, "" );
   MeasurementTable_TreeBox.SetScaledColumnWidth( 2, 30 );
   MeasurementTable_TreeBox.SetHeaderText( 3, "Name" );
   MeasurementTable_TreeBox.SetHeaderText( 4, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::Weight ) );
   MeasurementTable_TreeBox.SetHeaderText( 5, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::FWHM ) );
   MeasurementTable_TreeBox.SetHeaderText( 6, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::Eccentricity ) );
   MeasurementTable_TreeBox.SetHeaderText( 7, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::SNRWeight ) );
   MeasurementTable_TreeBox.SetHeaderText( 8, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::Median ) );
   MeasurementTable_TreeBox.SetHeaderText( 9, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::MedianMeanDev ) );
   MeasurementTable_TreeBox.SetHeaderText( 10, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::Noise ) );
   MeasurementTable_TreeBox.SetHeaderText( 11, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::NoiseRatio ) );
   MeasurementTable_TreeBox.SetHeaderText( 12, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::Stars ) );
   MeasurementTable_TreeBox.SetHeaderText( 13, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::StarResidual ) );
   MeasurementTable_TreeBox.SetHeaderText( 14, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::FWHMMeanDev ) );
   MeasurementTable_TreeBox.SetHeaderText( 15, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::EccentricityMeanDev ) );
   MeasurementTable_TreeBox.SetHeaderText( 16, TheSSSortingPropertyParameter->ElementLabel( SSSortingProperty::StarResidualMeanDev ) );
   MeasurementTable_TreeBox.SetHeaderText( 17, " " ); // blank 'spacer' column
   MeasurementTable_TreeBox.EnableMultipleSelections();
   MeasurementTable_TreeBox.DisableRootDecoration();
   MeasurementTable_TreeBox.EnableAlternateRowColor();
   MeasurementTable_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_CurrentNodeUpdated, w );
   MeasurementTable_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_NodeActivated, w );

   MeasurementTable_Sizer.SetSpacing( 4 );
   MeasurementTable_Sizer.Add( MeasurementsTable_Top1_Sizer );
   MeasurementTable_Sizer.Add( MeasurementsTable_Top2_Sizer );
   MeasurementTable_Sizer.Add( MeasurementTable_TreeBox, 100 );

   MeasurementTable_Control.SetSizer( MeasurementTable_Sizer );
   MeasurementTable_Control.AdjustToContents();

   //

   MeasurementGraph_SectionBar.SetTitle( "Measurements Graph" );
   MeasurementGraph_SectionBar.SetSection( MeasurementGraph_Control );
   MeasurementGraph_SectionBar.OnToggleSection( (SectionBar::section_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ToggleSection, w );

   for ( size_type i = 0; i < TheSSGraphPropertyParameter->NumberOfElements(); ++i )
      MeasurementGraph_GraphProperty_Control.AddItem( TheSSGraphPropertyParameter->ElementLabel( i ) );

   MeasurementGraph_GraphProperty_Control.OnItemSelected( (ComboBox::item_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ItemSelected, w );

   MeasurementGraph_ExportPDF_PushButton.SetText( "Save PDF" );
   MeasurementGraph_ExportPDF_PushButton.SetToolTip( "<p>Export the graph as a PDF file.</p>" );
   MeasurementGraph_ExportPDF_PushButton.OnClick( (Button::click_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_ButtonClick, w );

   MeasurementGraph_Top_Sizer.SetSpacing( 4 );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_GraphProperty_Control );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_ExportPDF_PushButton );

   MeasurementGraph_Graph.SetMinHeight( IMAGELIST_MINHEIGHT( w.Font() ) );
   MeasurementGraph_Graph.SetZoomFactor( MeasurementGraph_Graph.DisplayPixelRatio() );
   MeasurementGraph_Graph.SetBackgroundColor( MeasurementGraph_Control.BackgroundColor() );
   MeasurementGraph_Graph.OnApprove( (GraphWebView::approve_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_GraphApprove, w );
   MeasurementGraph_Graph.OnUnlock( (GraphWebView::unlock_event_handler)
                                    &SubframeSelectorMeasurementsInterface::e_GraphUnlock, w );

   MeasurementGraph_Sizer.SetSpacing( 4 );
   MeasurementGraph_Sizer.Add( MeasurementGraph_Top_Sizer );
   MeasurementGraph_Sizer.Add( MeasurementGraph_Graph, 100 );
   MeasurementGraph_Sizer.SetAlignment( MeasurementGraph_Top_Sizer, Sizer::item_alignment::Center );

   MeasurementGraph_Control.SetSizer( MeasurementGraph_Sizer );
   MeasurementGraph_Control.SetScaledMinSize( 640, 360 );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( MeasurementTable_SectionBar );
   Global_Sizer.Add( MeasurementTable_Control, 50 );
   Global_Sizer.Add( MeasurementGraph_SectionBar );
   Global_Sizer.Add( MeasurementGraph_Control, 100 );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasurementsInterface.cpp - Released 2019-01-21T12:06:42Z
