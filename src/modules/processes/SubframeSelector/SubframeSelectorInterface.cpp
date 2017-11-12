//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.02.01.0002
// ----------------------------------------------------------------------------
// SubframeSelectorInterface.cpp - Released 2017-11-05T16:00:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#include <pcl/Dialog.h>
#include <pcl/FileDialog.h>
#include <pcl/ViewList.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/Console.h>
#include <pcl/FileFormat.h>
#include <pcl/StdStatus.h>

#include "SubframeSelectorInterface.h"
#include "SubframeSelectorProcess.h"
#include "Version.h"

#define IMAGELIST_MINHEIGHT( fnt )  (12*fnt.Height() + 2)

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorInterface* TheSubframeSelectorInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "SubframeSelectorIcon.xpm"

// ----------------------------------------------------------------------------

SubframeSelectorInterface::SubframeSelectorInterface() :
        instance( TheSubframeSelectorProcess )
{
   TheSubframeSelectorInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. TreeBox) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

SubframeSelectorInterface::~SubframeSelectorInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString SubframeSelectorInterface::Id() const
{
   return "SubframeSelector";
}

MetaProcess* SubframeSelectorInterface::Process() const
{
   return TheSubframeSelectorProcess;
}

const char** SubframeSelectorInterface::IconImageXPM() const
{
   return nullptr; // SubframeSelectorIcon_XPM; ---> put a nice icon here
}

InterfaceFeatures SubframeSelectorInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void SubframeSelectorInterface::ResetInstance()
{
   SubframeSelectorInstance defaultInstance( TheSubframeSelectorProcess );
   ImportProcess( defaultInstance );
}

bool SubframeSelectorInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic,
                                        unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "SubframeSelector" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheSubframeSelectorProcess;
}

ProcessImplementation* SubframeSelectorInterface::NewProcess() const
{
   return new SubframeSelectorInstance( instance );
}

bool SubframeSelectorInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const SubframeSelectorInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a SubframeSelector instance.";
   return false;
}

bool SubframeSelectorInterface::RequiresInstanceValidation() const
{
   return true;
}

bool SubframeSelectorInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::ClearMeasurements()
{
   instance.measures.Clear();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::AddMeasurements( Array<MeasureItem>::const_iterator begin,
                                                 Array<MeasureItem>::const_iterator end )
{
   for ( Array<MeasureItem>::const_iterator i = begin; i != end; ++i )
      instance.measures.Add( MeasureItem( *i ) );
}

// ----------------------------------------------------------------------------

MeasureItem* SubframeSelectorInterface::GetMeasurementItem( size_type i )
{
   TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
   if ( node == nullptr )
      return nullptr;

   String indexString = node->Text( 0 );
   long index;
   try
   {
      index = indexString.ToInt();
   }
   catch ( ... )
   {
      return nullptr;
   }

   // 1-based index, 0-based arrays
   if ( index > 0 )
      --index;

   return &instance.measures[index];
}

// ----------------------------------------------------------------------------

TreeBox::Node* SubframeSelectorInterface::GetMeasurementNode( MeasureItem* item )
{
   if ( item == nullptr )
      return nullptr;

   for ( int i = 0; i < GUI->MeasurementTable_TreeBox.NumberOfChildren(); ++i )
   {
      TreeBox::Node* node = GUI->MeasurementTable_TreeBox[i];
      if ( node == nullptr )
         return nullptr;

      String indexString = node->Text( 0 );
      long index;
      try
      {
         index = indexString.ToInt();
      }
      catch ( ... )
      {
         return nullptr;
      }

      if ( index == item->index )
         return node;
   }

   return nullptr;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateControls()
{
   GUI->Routine_Control.SetCurrentItem( instance.routine );
   UpdateSubframeImagesList();
   UpdateSystemParameters();
   UpdateStarDetectorParameters();
   UpdateOutputFilesControls();
   UpdateExpressionParameters();
   UpdateMeasurementImagesList();;
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImageItem( size_type i )
{
   TreeBox::Node* node = GUI->SubframeImages_TreeBox[i];
   if ( node == nullptr )
      return;

   const SubframeSelectorInstance::SubframeItem& item = instance.subframes[i];

   node->SetText( 0, String( i + 1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/browser/picture.png" ) ) );
   node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImagesList()
{
   int currentIdx = GUI->SubframeImages_TreeBox.ChildIndex( GUI->SubframeImages_TreeBox.CurrentNode() );

   GUI->SubframeImages_TreeBox.DisableUpdates();
   GUI->SubframeImages_TreeBox.Clear();

   for ( size_type i = 0; i < instance.subframes.Length(); ++i )
   {
      new TreeBox::Node( GUI->SubframeImages_TreeBox );
      UpdateSubframeImageItem( i );
   }

   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->SubframeImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !instance.subframes.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->SubframeImages_TreeBox.NumberOfChildren() )
         GUI->SubframeImages_TreeBox.SetCurrentNode( GUI->SubframeImages_TreeBox[currentIdx] );

   GUI->SubframeImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSubframeImageSelectionButtons()
{
   bool hasItems = GUI->SubframeImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->SubframeImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->SubframeImages_Invert_PushButton.Enable( hasItems );
   GUI->SubframeImages_Toggle_PushButton.Enable( hasSelection );
   GUI->SubframeImages_Remove_PushButton.Enable( hasSelection );
   GUI->SubframeImages_Clear_PushButton.Enable( hasItems );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateSystemParameters()
{
   GUI->SystemParameters_SubframeScale_Control.SetValue( instance.subframeScale );
   GUI->SystemParameters_CameraGain_Control.SetValue( instance.cameraGain );
   GUI->SystemParameters_CameraResolution_Control.SetCurrentItem( instance.cameraResolution );
   GUI->SystemParameters_SiteLocalMidnight_Control.SetValue( instance.siteLocalMidnight );
   GUI->SystemParameters_ScaleUnit_Control.SetCurrentItem( instance.scaleUnit );
   GUI->SystemParameters_DataUnit_Control.SetCurrentItem( instance.dataUnit );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateStarDetectorParameters()
{
   GUI->StarDetectorParameters_StructureLayers_Control.SetValue( instance.structureLayers );
   GUI->StarDetectorParameters_NoiseLayers_Control.SetValue( instance.noiseLayers );
   GUI->StarDetectorParameters_HotPixelFilterRadius_Control.SetValue( instance.hotPixelFilterRadius );
   GUI->StarDetectorParameters_ApplyHotPixelFilter_Control.SetChecked( instance.applyHotPixelFilterToDetectionImage );
   GUI->StarDetectorParameters_NoiseReductionFilterRadius_Control.SetValue( instance.noiseReductionFilterRadius );
   GUI->StarDetectorParameters_Sensitivity_Control.SetValue( instance.sensitivity );
   GUI->StarDetectorParameters_PeakResponse_Control.SetValue( instance.peakResponse );
   GUI->StarDetectorParameters_MaxDistortion_Control.SetValue( instance.maxDistortion );
   GUI->StarDetectorParameters_UpperLimit_Control.SetValue( instance.upperLimit );
   GUI->StarDetectorParameters_BackgroundExpansion_Control.SetValue( instance.backgroundExpansion );
   GUI->StarDetectorParameters_XYStretch_Control.SetValue( instance.xyStretch );
   GUI->StarDetectorParameters_PSFFit_Control.SetCurrentItem( instance.psfFit );
   GUI->StarDetectorParameters_PSFFitCircular_Control.SetChecked( instance.psfFitCircular );
   GUI->StarDetectorParameters_Pedestal_Control.SetValue( instance.pedestal );
   GUI->StarDetectorParameters_ROIX0_Control.SetValue( instance.roi.x0 );
   GUI->StarDetectorParameters_ROIY0_Control.SetValue( instance.roi.y0 );
   GUI->StarDetectorParameters_ROIWidth_Control.SetValue( instance.roi.Width() );
   GUI->StarDetectorParameters_ROIHeight_Control.SetValue( instance.roi.Height() );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateOutputFilesControls()
{
   GUI->OutputDirectory_Edit.SetText( instance.outputDirectory );
   GUI->OutputPrefix_Edit.SetText( instance.outputPrefix );
   GUI->OutputPostfix_Edit.SetText( instance.outputPostfix );
   GUI->OutputKeyword_Edit.SetText( instance.outputKeyword );
   GUI->OverwriteExistingFiles_CheckBox.SetChecked( instance.overwriteExistingFiles );
   GUI->OnError_ComboBox.SetCurrentItem( instance.onError );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateExpressionParameters()
{
   GUI->ExpressionParameters_Approval_Control.SetText( instance.approvalExpression );
   GUI->ExpressionParameters_Weighting_Control.SetText( instance.weightingExpression );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateMeasurementImageItem( size_type i, MeasureItem* item )
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

   node->SetIcon( 2,
                  Bitmap( ScaledResource( item->locked ? ":/file-explorer/read-only.png" : ":/icons/unlock.png" ) ) );
   node->SetAlignment( 2, TextAlign::Left );

   node->SetIcon( 3, Bitmap( ScaledResource( ":/browser/picture.png" ) ) );
   node->SetText( 3, File::ExtractNameAndSuffix( item->path ) );
   node->SetToolTip( 3, item->path );
   node->SetAlignment( 3, TextAlign::Left );

   node->SetText( 4, String().Format( "%.03f", item->weight ) );
   node->SetAlignment( 4, TextAlign::Center );

   node->SetText( 5, String().Format( "%.03f", item->FWHM( instance.subframeScale, instance.scaleUnit ) ) );
   node->SetAlignment( 5, TextAlign::Center );

   node->SetText( 6, String().Format( "%.03f", item->eccentricity ) );
   node->SetAlignment( 6, TextAlign::Center );

   node->SetText( 7, String().Format( "%.04f", item->snrWeight ) );
   node->SetAlignment( 7, TextAlign::Center );

   node->SetText( 8, String().Format( "%.03f", item->Median( instance.cameraGain,
                                                             TheSSCameraResolutionParameter->ElementData(
                                                                     instance.cameraResolution ),
                                                             instance.dataUnit ) ) );
   node->SetAlignment( 8, TextAlign::Center );

   node->SetText( 9, String().Format( "%.03f", item->MedianMeanDev( instance.cameraGain,
                                                                    TheSSCameraResolutionParameter->ElementData(
                                                                            instance.cameraResolution ),
                                                                    instance.dataUnit ) ) );
   node->SetAlignment( 9, TextAlign::Center );

   node->SetText( 10, String().Format( "%.03f", item->Noise( instance.cameraGain,
                                                             TheSSCameraResolutionParameter->ElementData(
                                                                     instance.cameraResolution ),
                                                             instance.dataUnit ) ) );
   node->SetAlignment( 10, TextAlign::Center );

   node->SetText( 11, String().Format( "%.04f", item->noiseRatio ) );
   node->SetAlignment( 11, TextAlign::Center );

   node->SetText( 12, String().Format( "%i", item->stars ) );
   node->SetAlignment( 12, TextAlign::Center );

   node->SetText( 13, String().Format( "%.03f", item->starResidual ) );
   node->SetAlignment( 13, TextAlign::Center );

   node->SetText( 14, String().Format( "%.04f", item->FWHMMeanDeviation( instance.subframeScale,
                                                                         instance.scaleUnit ) ) );
   node->SetAlignment( 14, TextAlign::Center );

   node->SetText( 15, String().Format( "%.04f", item->eccentricityMeanDev ) );
   node->SetAlignment( 15, TextAlign::Center );

   node->SetText( 16, String().Format( "%.04f", item->starResidualMeanDev ) );
   node->SetAlignment( 16, TextAlign::Center );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateMeasurementImagesList()
{
   GUI->MeasurementsTable_SortingProperty_Control.SetCurrentItem( instance.sortingProperty );

   bool shouldRecreate = instance.measures.Length() != GUI->MeasurementTable_TreeBox.NumberOfChildren();

   const IndirectArray<TreeBox::Node>& selections = GUI->MeasurementTable_TreeBox.SelectedNodes();
   Array<int> currentIds( selections.Length() );
   for ( size_type i = 0; i < selections.Length(); ++i )
      currentIds[i] = GUI->MeasurementTable_TreeBox.ChildIndex( selections[i] );

   GUI->MeasurementTable_TreeBox.DisableUpdates();
   if ( shouldRecreate )
      GUI->MeasurementTable_TreeBox.Clear();

   ApplyWeightingExpression();
   ApplyApprovalExpression();

   Array<MeasureItem> measuresSorted( instance.measures );
   measuresSorted.Sort( SubframeSortingBinaryPredicate( instance.sortingProperty,
                                                        GUI->MeasurementsTable_SortingMode_Control.CurrentItem() ) );

   for ( size_type i = 0; i < measuresSorted.Length(); ++i )
   {
      if ( shouldRecreate )
         new TreeBox::Node( GUI->MeasurementTable_TreeBox );
      UpdateMeasurementImageItem( i, &measuresSorted[i] );
   }

   GUI->MeasurementTable_TreeBox.AdjustColumnWidthToContents( 3 );
   GUI->MeasurementTable_TreeBox.AdjustColumnWidthToContents( 4 );
   GUI->MeasurementTable_TreeBox.AdjustColumnWidthToContents( 5 );

   if ( shouldRecreate && !instance.measures.IsEmpty() )
      for ( size_type i = 0; i < currentIds.Length(); ++i )
         if ( currentIds[i] >= 0 && currentIds[i] < GUI->MeasurementTable_TreeBox.NumberOfChildren() )
            GUI->MeasurementTable_TreeBox.Child( currentIds[i] )->Select();

   GUI->MeasurementTable_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::UpdateMeasurementGraph()
{
   GUI->MeasurementGraph_GraphProperty_Control.SetCurrentItem( instance.graphProperty );

   DataPointVector dataset( instance.measures.Length() );
   for ( size_type i = 0; i < instance.measures.Length(); ++i )
   {
      dataset[i].x = instance.measures[i].index;
      switch ( instance.graphProperty )
      {
      case SSGraphProperty::Weight: dataset[i].data = instance.measures[i].weight;
         break;
      case SSGraphProperty::FWHM:
         dataset[i].data = instance.measures[i].FWHM( instance.subframeScale, instance.scaleUnit );
         break;
      case SSGraphProperty::Eccentricity: dataset[i].data = instance.measures[i].eccentricity;
         break;
      case SSGraphProperty::SNRWeight: dataset[i].data = instance.measures[i].snrWeight;
         break;
      case SSGraphProperty::Median:
         dataset[i].data = instance.measures[i].Median( instance.cameraGain,
                                                     TheSSCameraResolutionParameter->ElementData(
                                                             instance.cameraResolution ),
                                                     instance.dataUnit );
         break;
      case SSGraphProperty::MedianMeanDev:
         dataset[i].data = instance.measures[i].MedianMeanDev( instance.cameraGain,
                                                            TheSSCameraResolutionParameter->ElementData(
                                                                    instance.cameraResolution ),
                                                            instance.dataUnit );
         break;
      case SSGraphProperty::Noise:
         dataset[i].data = instance.measures[i].Noise( instance.cameraGain,
                                                    TheSSCameraResolutionParameter->ElementData(
                                                            instance.cameraResolution ),
                                                    instance.dataUnit );
         break;
      case SSGraphProperty::NoiseRatio: dataset[i].data = instance.measures[i].noiseRatio;
         break;
      case SSGraphProperty::Stars: dataset[i].data = instance.measures[i].stars;
         break;
      case SSGraphProperty::StarResidual: dataset[i].data = instance.measures[i].starResidual;
         break;
      case SSGraphProperty::FWHMMeanDev:
         dataset[i].data = instance.measures[i].FWHMMeanDeviation( instance.subframeScale, instance.scaleUnit );
         break;
      case SSGraphProperty::EccentricityMeanDev: dataset[i].data = instance.measures[i].eccentricityMeanDev;
         break;
      case SSGraphProperty::StarResidualMeanDev: dataset[i].data = instance.measures[i].starResidualMeanDev;
         break;
      default: dataset[i].data = instance.measures[i].weight;
         break;
      }
      dataset[i].weight = instance.measures[i].weight;
      dataset[i].approved = instance.measures[i].enabled;
      dataset[i].locked = instance.measures[i].locked;
   }

   GUI->MeasurementGraph_Graph.SetDataset( TheSSGraphPropertyParameter->ElementLabel( instance.graphProperty ),
                                           &dataset );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::ApplyApprovalExpression()
{
   GUI->MeasurementTable_TreeBox.DisableUpdates();
   try
   {
      instance.ApproveMeasurements();
   }
   catch ( ... )
   {
      GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
      Console().Show();
   }
   GUI->MeasurementTable_TreeBox.EnableUpdates();
}

void SubframeSelectorInterface::ApplyWeightingExpression()
{
   GUI->MeasurementTable_TreeBox.DisableUpdates();
   try
   {
      instance.WeightMeasurements();
   }
   catch ( ... )
   {
      GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
      Console().Show();
   }
   GUI->MeasurementTable_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::ExportCSV() const
{
   SaveFileDialog d;
   d.SetCaption( "SubframeSelector: Save CSV File" );
   d.SetFilter( FileFilter( "CSV", ".csv" ) );
   d.EnableOverwritePrompt();
   if ( d.Execute() )
   {
      String filePath = d.FileName();
      File f = File::CreateFileForWriting( filePath );
      Console().WriteLn( "Generating output CSV file: " + filePath );

      StandardStatus status;
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Writing CSV file", instance.measures.Length() );


      f.OutTextLn( String().Format( "SubframeSelector Version: %s", SubframeSelectorVersion() ) );

      f.OutTextLn( String().Format( "Subframe Scale,%.5f", instance.subframeScale ) );
      f.OutTextLn( String().Format( "Camera Gain,%.5f", instance.cameraGain ) );
      f.OutTextLn( "Camera Resolution,\"" +
                   TheSSCameraResolutionParameter->ElementLabel( instance.cameraResolution ) + "\"" );
      f.OutTextLn( String().Format( "Site Local Midnight,%d", instance.siteLocalMidnight ) );
      f.OutTextLn( "Scale Unit,\"" + TheSSScaleUnitParameter->ElementLabel( instance.scaleUnit ) + "\"" );
      f.OutTextLn( "Data Unit,\"" + TheSSDataUnitParameter->ElementLabel( instance.dataUnit ) + "\"" );
      f.OutTextLn( String().Format( "Structure Layers,%d", instance.structureLayers ) );
      f.OutTextLn( String().Format( "Noise Layers,%d", instance.noiseLayers ) );
      f.OutTextLn( String().Format( "Hot Pixel Filter Radius,%d", instance.hotPixelFilterRadius ) );
      f.OutTextLn( String().Format( "Apply Hot Pixel Filter to Detection Image,%s",
                                    instance.applyHotPixelFilterToDetectionImage ? "true," : "false," ) );
      f.OutTextLn( String().Format( "Noise Reduction Filter Radius,%d", instance.noiseReductionFilterRadius ) );
      f.OutTextLn( String().Format( "Sensitivity,%.5f", instance.sensitivity ) );
      f.OutTextLn( String().Format( "Peak Response,%.3f", instance.peakResponse ) );
      f.OutTextLn( String().Format( "Maximum Star Distortion,%.3f", instance.maxDistortion ) );
      f.OutTextLn( String().Format( "Upper Limit,%.3f", instance.upperLimit ) );
      f.OutTextLn( String().Format( "Background Expansion,%d", instance.backgroundExpansion ) );
      f.OutTextLn( String().Format( "XY Stretch,%.3f", instance.xyStretch ) );
      f.OutTextLn( String().Format( "Pedestal,%d", instance.pedestal ) );
      f.OutTextLn( String().Format( "Subframe Region,%d,%d,%d,%d",
                                    instance.roi.x0, instance.roi.y0, instance.roi.Width(), instance.roi.Height() )
      );
      f.OutTextLn( "PSF Fit,\"" + TheSSPSFFitParameter->ElementLabel( instance.psfFit ) + "\"" );
      f.OutTextLn( String().Format( "Circular PSF,%s", instance.psfFitCircular ? "true," : "false," ) );
      f.OutTextLn( "Approval expression,\"" + instance.approvalExpression + "\"" );
      f.OutTextLn( "Weighting expression,\"" + instance.weightingExpression + "\"" );

      f.OutTextLn( "Index,Approved,Locked,File,Weight,FWHM,Eccentricity,SNR Weight,Median,"
                           "Median Mean Deviation,Noise,Noise Ratio,Stars,Star Residual,"
                           "FWHM Mean Deviation,Eccentricity Mean Deviation,"
                           "Star Residual Mean Deviation" );

      for ( Array<MeasureItem>::const_iterator i = instance.measures.Begin(); i != instance.measures.End(); ++i, ++monitor )
         f.OutTextLn( IsoString( i->index ) + ',' +
                      IsoString( i->enabled ? "true," : "false," ) +
                      IsoString( i->locked ? "true," : "false," ) +
                      IsoString( '"' + i->path + '"' + ',' ) +
                      String().Format(
                              "%.05f,%.05f,%.05f,%.05f,%.05f,%.05f,%.05f,%.05f,%i,%.05f,%.05f,%.05f,%.05f",
                              i->weight,
                              i->FWHM( instance.subframeScale, instance.scaleUnit ),
                              i->eccentricity,
                              i->snrWeight,
                              i->Median( instance.cameraGain,
                                         TheSSCameraResolutionParameter->ElementData( instance.cameraResolution ),
                                         instance.dataUnit ),
                              i->MedianMeanDev( instance.cameraGain,
                                                TheSSCameraResolutionParameter->ElementData(
                                                        instance.cameraResolution ),
                                                instance.dataUnit ),
                              i->Noise( instance.cameraGain,
                                        TheSSCameraResolutionParameter->ElementData( instance.cameraResolution ),
                                        instance.dataUnit ),
                              i->noiseRatio,
                              i->stars,
                              i->starResidual,
                              i->FWHMMeanDeviation( instance.subframeScale, instance.scaleUnit ),
                              i->eccentricityMeanDev,
                              i->starResidualMeanDev
                      ) );

      monitor.Complete();
      f.Close();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::ExportPDF() const
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

void SubframeSelectorInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( sender == GUI->SubframeImages_SectionBar )
   {
      if ( start )
         GUI->SubframeImages_TreeBox.SetFixedHeight();
      else
      {
         GUI->SubframeImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
         GUI->SubframeImages_TreeBox.SetMaxHeight( int_max );
      }
   }
   else if ( sender == GUI->MeasurementTable_SectionBar )
   {
      if ( start )
         GUI->MeasurementTable_TreeBox.SetFixedHeight();
      else
      {
         GUI->MeasurementTable_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
         GUI->MeasurementTable_TreeBox.SetMaxHeight( int_max );
      }
   }
}

// ----------------------------------------------------------------------------

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != nullptr) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_CurrentNodeUpdated( TreeBox& sender,
                                                                     TreeBox::Node& current,
                                                                     TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.subframes.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.subframes.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );

   SubframeSelectorInstance::SubframeItem& item = instance.subframes[index];

   switch ( col )
   {
   case 0:
      // Activate the item's index number: ignore.
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
      UpdateSubframeImageItem( index );
      break;
   case 2:
   {
      // Activate the item's path: open the image.
      Array<ImageWindow> windows = ImageWindow::Open( item.path, IsoString()/*id*/ );
      for ( ImageWindow& window : windows )
         window.Show();
   }
      break;
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateSubframeImageSelectionButtons();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__SubframeImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->SubframeImages_AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.EnableMultipleSelections();
      d.LoadImageFilters();
      d.SetCaption( "SubframeSelector: Select Subframes" );

      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->SubframeImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.subframes.Insert( instance.subframes.At( i0++ ), SubframeSelectorInstance::SubframeItem( *i ) );
         UpdateSubframeImagesList();
         UpdateSubframeImageSelectionButtons();
      }
   }
   else if ( sender == GUI->SubframeImages_Invert_PushButton )
   {
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->SubframeImages_TreeBox[i]->Select( !GUI->SubframeImages_TreeBox[i]->IsSelected() );
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_Toggle_PushButton )
   {
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->SubframeImages_TreeBox[i]->IsSelected() )
            instance.subframes[i].enabled = !instance.subframes[i].enabled;
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_Remove_PushButton )
   {
      SubframeSelectorInstance::subframe_list newTargets;
      for ( int i = 0, n = GUI->SubframeImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->SubframeImages_TreeBox[i]->IsSelected() )
            newTargets.Add( instance.subframes[i] );
      instance.subframes = newTargets;
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->SubframeImages_Clear_PushButton )
   {
      instance.subframes.Clear();
      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__MeasurementImages_CurrentNodeUpdated( TreeBox& sender,
                                                                        TreeBox::Node& current,
                                                                        TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.measures.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__MeasurementImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= instance.measures.Length() )
      throw Error( "SubframeSelectorInterface: *Warning* Corrupted interface structures" );

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
      UpdateMeasurementGraph();
      break;
   case 2:
      // Activate the item's checkmark: toggle item's locked state.
      item->locked = !item->locked;
      UpdateMeasurementImageItem( index, item );
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

void SubframeSelectorInterface::__MeasurementImages_Click( Button& sender, bool checked )
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
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
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
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
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
      for ( size_type i = 0; i < instance.measures.Length(); ++i )
      {
         MeasureItem* measure = instance.measures.At( i );
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
      instance.measures = newMeasures;
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->MeasurementsTable_Clear_PushButton )
   {
      instance.measures.Clear();
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->MeasurementsTable_CSV_PushButton )
   {
      ExportCSV();
   }
   else if ( sender == GUI->MeasurementGraph_Save_PushButton )
   {
      ExportPDF();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__MeasurementGraph_Approve( GraphWebView &sender, int& index )
{
   if ( index <= 0 )
      return;

   --index; // 1-based to 0-based

   if ( index >= instance.measures.Length() )
      return;

   MeasureItem* item = &instance.measures[index];
   item->enabled = !item->enabled;
   item->locked = true;

   TreeBox::Node* node = GetMeasurementNode( item );
   if ( node == nullptr )
      return;

   GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
   GUI->MeasurementTable_TreeBox.SetVerticalScrollPosition( GUI->MeasurementTable_TreeBox.NodeRect( node ).y0 );
   UpdateMeasurementImageItem( GUI->MeasurementTable_TreeBox.ChildIndex( node ), item );
   UpdateMeasurementGraph();
}

void SubframeSelectorInterface::__MeasurementGraph_Unlock( GraphWebView &sender, int& index )
{
   if ( index <= 0 )
      return;

   --index; // 1-based to 0-based

   if ( index >= instance.measures.Length() )
      return;

   MeasureItem* item = &instance.measures[index];
   item->locked = false;

   TreeBox::Node* node = GetMeasurementNode( item );
   if ( node == nullptr )
      return;

   GUI->MeasurementTable_TreeBox.SetCurrentNode( node );
   GUI->MeasurementTable_TreeBox.SetVerticalScrollPosition( GUI->MeasurementTable_TreeBox.NodeRect( node ).y0 );
   UpdateMeasurementImageItem( GUI->MeasurementTable_TreeBox.ChildIndex( node ), item );
   UpdateMeasurementGraph();
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__StarDetector_ViewDrag( Control& sender, const Point& pos, const View& view,
                                                         unsigned modifiers,
                                                         bool& wantsView )
{
   if ( sender == GUI->StarDetectorParameters_ROISelectPreview_Button )
      wantsView = view.IsPreview();
}

void SubframeSelectorInterface::__StarDetector_ViewDrop( Control& sender, const Point& pos, const View& view,
                                                         unsigned modifiers )
{
   if ( sender == GUI->StarDetectorParameters_ROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         instance.roi = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SystemParameters_SubframeScale_Control )
   {
      instance.subframeScale = value;
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->SystemParameters_CameraGain_Control )
   {
      instance.cameraGain = value;
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->StarDetectorParameters_Sensitivity_Control )
      instance.sensitivity = value;
   else if ( sender == GUI->StarDetectorParameters_PeakResponse_Control )
      instance.peakResponse = value;
   else if ( sender == GUI->StarDetectorParameters_MaxDistortion_Control )
      instance.maxDistortion = value;
   else if ( sender == GUI->StarDetectorParameters_UpperLimit_Control )
      instance.upperLimit = value;
   else if ( sender == GUI->StarDetectorParameters_XYStretch_Control )
      instance.xyStretch = value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->SystemParameters_SiteLocalMidnight_Control )
      instance.siteLocalMidnight = value;

   else if ( sender == GUI->StarDetectorParameters_StructureLayers_Control )
      instance.structureLayers = value;
   else if ( sender == GUI->StarDetectorParameters_NoiseLayers_Control )
      instance.noiseLayers = value;
   else if ( sender == GUI->StarDetectorParameters_HotPixelFilterRadius_Control )
      instance.hotPixelFilterRadius = value;
   else if ( sender == GUI->StarDetectorParameters_NoiseReductionFilterRadius_Control )
      instance.noiseReductionFilterRadius = value;
   else if ( sender == GUI->StarDetectorParameters_BackgroundExpansion_Control )
      instance.backgroundExpansion = value;
   else if ( sender == GUI->StarDetectorParameters_Pedestal_Control )
      instance.pedestal = value;
   else if ( sender == GUI->StarDetectorParameters_ROIX0_Control )
      instance.roi.x0 = value;
   else if ( sender == GUI->StarDetectorParameters_ROIY0_Control )
      instance.roi.y0 = value;
   else if ( sender == GUI->StarDetectorParameters_ROIWidth_Control )
      instance.roi.x1 = instance.roi.x0 + value;
   else if ( sender == GUI->StarDetectorParameters_ROIHeight_Control )
      instance.roi.y1 = instance.roi.y0 + value;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ComboSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Routine_Control )
      instance.routine = itemIndex;
   else if ( sender == GUI->SystemParameters_CameraResolution_Control )
   {
      instance.cameraResolution = itemIndex;
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->SystemParameters_ScaleUnit_Control )
   {
      instance.scaleUnit = itemIndex;
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->SystemParameters_DataUnit_Control )
   {
      instance.dataUnit = itemIndex;
      UpdateMeasurementImagesList();
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->StarDetectorParameters_PSFFit_Control )
      instance.psfFit = itemIndex;

   else if ( sender == GUI->MeasurementsTable_SortingProperty_Control )
   {
      instance.sortingProperty = itemIndex;
      UpdateMeasurementImagesList();
   }
   else if ( sender == GUI->MeasurementsTable_SortingMode_Control )
   {
      UpdateMeasurementImagesList();
   }
   else if ( sender == GUI->MeasurementGraph_GraphProperty_Control )
   {
      instance.graphProperty = itemIndex;
      UpdateMeasurementGraph();
   }
   else if ( sender == GUI->OnError_ComboBox )
      instance.onError = itemIndex;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__CheckboxUpdated( Button& sender, Button::check_state state )
{
   if ( sender == GUI->StarDetectorParameters_ApplyHotPixelFilter_Control )
      instance.applyHotPixelFilterToDetectionImage = state == CheckState::Checked;
   else if ( sender == GUI->StarDetectorParameters_PSFFitCircular_Control )
      instance.psfFitCircular = state == CheckState::Checked;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->StarDetectorParameters_ROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               instance.roi = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
   else if ( sender == GUI->OutputDirectory_ToolButton )
   {
      GetDirectoryDialog d;
      d.SetCaption( "SubframeSelector: Select Output Directory" );
      if ( d.Execute() )
         GUI->OutputDirectory_Edit.SetText( instance.outputDirectory = d.Directory() );
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
      instance.overwriteExistingFiles = checked;
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__TextUpdated( Edit& sender, const String& text )
{
   if ( sender == GUI->ExpressionParameters_Approval_Control )
   {
      if ( MeasureUtils::IsValidExpression( text ) )
      {
         GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/enabled.png" ) ) );
      }
      else
      {
         GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
      }
   }
   else if ( sender == GUI->ExpressionParameters_Weighting_Control )
   {
      if ( MeasureUtils::IsValidExpression( text ) )
      {
         GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/enabled.png" ) ) );
      }
      else
      {
         GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
      }
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__TextUpdateCompleted( Edit& sender )
{
   bool shouldUpdate = false;
   if ( sender == GUI->ExpressionParameters_Approval_Control )
   {
      String text = sender.Text();
      if ( MeasureUtils::IsValidExpression( text ) )
      {
         GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/enabled.png" ) ) );
         if ( instance.approvalExpression != text )
            shouldUpdate = true;
         instance.approvalExpression = text;
      }
      else
      {
         GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
         instance.approvalExpression = "";
      }
      if ( shouldUpdate )
      {
         UpdateMeasurementImagesList();
         UpdateMeasurementGraph();
      }
   }
   else if ( sender == GUI->ExpressionParameters_Weighting_Control )
   {
      String text = sender.Text();
      if ( MeasureUtils::IsValidExpression( text ) )
      {
         GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/enabled.png" ) ) );
         if ( instance.weightingExpression != text )
            shouldUpdate = true;
         instance.weightingExpression = text;
      }
      else
      {
         GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
         instance.weightingExpression = "";
      }
      if ( shouldUpdate )
      {
         UpdateMeasurementImagesList();
         UpdateMeasurementGraph();
      }
   }
   else if ( sender == GUI->OutputDirectory_Edit )
   {
      String text = sender.Text().Trimmed();
      instance.outputDirectory = text;
      sender.SetText( text );
   }
   else if ( sender == GUI->OutputPrefix_Edit )
   {
      String text = sender.Text().Trimmed();
      instance.outputPrefix = text;
      sender.SetText( text );
   }
   else if ( sender == GUI->OutputPostfix_Edit )
   {
      String text = sender.Text().Trimmed();
      instance.outputPostfix = text;
      sender.SetText( text );
   }
   else if ( sender == GUI->OutputKeyword_Edit )
   {
      String text = sender.Text().Trimmed();
      instance.outputKeyword = text;
      sender.SetText( text );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files,
                                            unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->SubframeImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDirectory_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
}

// ----------------------------------------------------------------------------

void SubframeSelectorInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files,
                                            unsigned modifiers )
{
   if ( sender == GUI->SubframeImages_TreeBox.Viewport() )
   {
      StringList inputFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
         if ( File::Exists( item ) )
            inputFiles << item;
         else if ( File::DirectoryExists( item ) )
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );

      inputFiles.Sort();
      size_type i0 = TreeInsertionIndex( GUI->SubframeImages_TreeBox );
      for ( const String& file : inputFiles )
         instance.subframes.Insert( instance.subframes.At( i0++ ), SubframeSelectorInstance::SubframeItem( file ) );

      UpdateSubframeImagesList();
      UpdateSubframeImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDirectory_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDirectory_Edit.SetText( instance.outputDirectory = files[0] );
   }
}

// ----------------------------------------------------------------------------

SubframeSelectorInterface::GUIData::GUIData( SubframeSelectorInterface& w ) : MeasurementGraph_Graph( w )
{
   pcl::Font fnt = w.Font();

   int currentLabelWidth = fnt.Width( String( "Routine:" ) ); // the longest label text

   //

   Routine_Label.SetText( "Routine:" );
   Routine_Label.SetMinWidth( currentLabelWidth );
   Routine_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSRoutineParameter->NumberOfElements(); ++i )
      Routine_Control.AddItem( TheSSRoutineParameter->ElementLabel( i ) );
   Routine_Control.SetToolTip( TheSSRoutineParameter->Tooltip() );
   Routine_Control.OnItemSelected( (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   Routine_Sizer.SetSpacing( 4 );
   Routine_Sizer.Add( Routine_Label );
   Routine_Sizer.Add( Routine_Control );

   //

   currentLabelWidth = fnt.Width( String( "Site Local Midnight:" ) ); // the longest label text

   SubframeImages_SectionBar.SetTitle( "Subframes" );
   SubframeImages_SectionBar.SetSection( SubframeImages_Control );
   SubframeImages_SectionBar.OnToggleSection(
           (SectionBar::section_event_handler) &SubframeSelectorInterface::__ToggleSection, w );

   SubframeImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   SubframeImages_TreeBox.SetScaledMinWidth( 200 );
   SubframeImages_TreeBox.SetNumberOfColumns( 3 );
   SubframeImages_TreeBox.HideHeader();
   SubframeImages_TreeBox.EnableMultipleSelections();
   SubframeImages_TreeBox.DisableRootDecoration();
   SubframeImages_TreeBox.EnableAlternateRowColor();
   SubframeImages_TreeBox.OnCurrentNodeUpdated(
           (TreeBox::node_navigation_event_handler) &SubframeSelectorInterface::__SubframeImages_CurrentNodeUpdated,
           w );
   SubframeImages_TreeBox.OnNodeActivated(
           (TreeBox::node_event_handler) &SubframeSelectorInterface::__SubframeImages_NodeActivated, w );
   SubframeImages_TreeBox.OnNodeSelectionUpdated(
           (TreeBox::tree_event_handler) &SubframeSelectorInterface::__SubframeImages_NodeSelectionUpdated, w );
   SubframeImages_TreeBox.Viewport().OnFileDrag(
           (Control::file_drag_event_handler) &SubframeSelectorInterface::__FileDrag, w );
   SubframeImages_TreeBox.Viewport().OnFileDrop(
           (Control::file_drop_event_handler) &SubframeSelectorInterface::__FileDrop, w );

   SubframeImages_AddFiles_PushButton.SetText( "Add Files" );
   SubframeImages_AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of subframes.</p>" );
   SubframeImages_AddFiles_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   SubframeImages_Invert_PushButton.SetText( "Invert" );
   SubframeImages_Invert_PushButton.SetToolTip( "<p>Invert the current selection of subframes.</p>" );
   SubframeImages_Invert_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click,
                                       w );

   SubframeImages_Toggle_PushButton.SetText( "Toggle" );
   SubframeImages_Toggle_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected subframes.</p>"
                                                 "<p>Disabled subframes will be ignored during the measuring and output processes.</p>" );
   SubframeImages_Toggle_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click,
                                      w );

   SubframeImages_Remove_PushButton.SetText( "Remove" );
   SubframeImages_Remove_PushButton.SetToolTip( "<p>Remove all currently selected subframes.</p>" );
   SubframeImages_Remove_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click,
                                      w );

   SubframeImages_Clear_PushButton.SetText( "Clear" );
   SubframeImages_Clear_PushButton.SetToolTip( "<p>Clear the list of subframes.</p>" );
   SubframeImages_Clear_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__SubframeImages_Click, w );

   SubframeButtons_Sizer.SetSpacing( 4 );
   SubframeButtons_Sizer.Add( SubframeImages_AddFiles_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Invert_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Toggle_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Remove_PushButton );
   SubframeButtons_Sizer.Add( SubframeImages_Clear_PushButton );
   SubframeButtons_Sizer.AddStretch();

   SubframeImages_Sizer.SetSpacing( 4 );
   SubframeImages_Sizer.Add( SubframeImages_TreeBox, 100 );
   SubframeImages_Sizer.Add( SubframeButtons_Sizer );

   SubframeImages_Control.SetSizer( SubframeImages_Sizer );
   SubframeImages_Control.AdjustToContents();

   //

   SystemParameters_SectionBar.SetTitle( "System Parameters" );
   SystemParameters_SectionBar.SetSection( SystemParameters_Control );
   SystemParameters_SectionBar.OnToggleSection(
           (SectionBar::section_event_handler) &SubframeSelectorInterface::__ToggleSection, w );

   SystemParameters_SubframeScale_Control.label.SetText( "Subframe Scale:" );
   SystemParameters_SubframeScale_Control.label.SetMinWidth( currentLabelWidth );
   SystemParameters_SubframeScale_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   SystemParameters_SubframeScale_Control.slider.Hide();
   SystemParameters_SubframeScale_Control.SetReal();
   SystemParameters_SubframeScale_Control.SetRange( TheSSSubframeScaleParameter->MinimumValue(),
                                                    TheSSSubframeScaleParameter->MaximumValue() );
   SystemParameters_SubframeScale_Control.SetPrecision( TheSSSubframeScaleParameter->Precision() );
   SystemParameters_SubframeScale_Control.SetToolTip( TheSSSubframeScaleParameter->Tooltip() );
   SystemParameters_SubframeScale_Control.OnValueUpdated(
           (NumericEdit::value_event_handler) &SubframeSelectorInterface::__RealValueUpdated, w );

   SystemParameters_SubframeScale_Unit.SetText( "arcseconds / pixel" );
   SystemParameters_SubframeScale_Unit.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   SystemParameters_SubframeScale_Sizer.SetSpacing( 4 );
   SystemParameters_SubframeScale_Sizer.Add( SystemParameters_SubframeScale_Control );
   SystemParameters_SubframeScale_Sizer.Add( SystemParameters_SubframeScale_Unit );
   SystemParameters_SubframeScale_Sizer.AddStretch();

   SystemParameters_CameraGain_Control.label.SetText( "Camera Gain:" );
   SystemParameters_CameraGain_Control.label.SetMinWidth( currentLabelWidth );
   SystemParameters_CameraGain_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   SystemParameters_CameraGain_Control.slider.Hide();
   SystemParameters_CameraGain_Control.SetReal();
   SystemParameters_CameraGain_Control.SetRange( TheSSCameraGainParameter->MinimumValue(),
                                                 TheSSCameraGainParameter->MaximumValue() );
   SystemParameters_CameraGain_Control.SetPrecision( TheSSCameraGainParameter->Precision() );
   SystemParameters_CameraGain_Control.SetToolTip( TheSSCameraGainParameter->Tooltip() );
   SystemParameters_CameraGain_Control.OnValueUpdated(
           (NumericEdit::value_event_handler) &SubframeSelectorInterface::__RealValueUpdated, w );

   SystemParameters_CameraGain_Unit.SetText( "electrons / Data Number" );
   SystemParameters_CameraGain_Unit.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   SystemParameters_CameraGain_Sizer.SetSpacing( 4 );
   SystemParameters_CameraGain_Sizer.Add( SystemParameters_CameraGain_Control );
   SystemParameters_CameraGain_Sizer.Add( SystemParameters_CameraGain_Unit );
   SystemParameters_CameraGain_Sizer.AddStretch();

   SystemParameters_CameraResolution_Label.SetText( "Camera Resolution:" );
   SystemParameters_CameraResolution_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_CameraResolution_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSCameraResolutionParameter->NumberOfElements(); ++i )
      SystemParameters_CameraResolution_Control.AddItem( TheSSCameraResolutionParameter->ElementLabel( i ) );
   SystemParameters_CameraResolution_Control.SetToolTip( TheSSCameraResolutionParameter->Tooltip() );
   SystemParameters_CameraResolution_Control.OnItemSelected(
           (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   SystemParameters_CameraResolution_Sizer.SetSpacing( 4 );
   SystemParameters_CameraResolution_Sizer.Add( SystemParameters_CameraResolution_Label );
   SystemParameters_CameraResolution_Sizer.Add( SystemParameters_CameraResolution_Control );
   SystemParameters_CameraResolution_Sizer.AddStretch();

   SystemParameters_SiteLocalMidnight_Label.SetText( "Site Local Midnight:" );
   SystemParameters_SiteLocalMidnight_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_SiteLocalMidnight_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   SystemParameters_SiteLocalMidnight_Control.SetRange( TheSSSiteLocalMidnightParameter->MinimumValue(),
                                                        TheSSSiteLocalMidnightParameter->MaximumValue() );
   SystemParameters_SiteLocalMidnight_Control.SetToolTip( TheSSSiteLocalMidnightParameter->Tooltip() );
   SystemParameters_SiteLocalMidnight_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   SystemParameters_SiteLocalMidnight_Unit.SetText( "hours (UTC)" );
   SystemParameters_SiteLocalMidnight_Unit.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   SystemParameters_SiteLocalMidnight_Sizer.SetSpacing( 4 );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Label );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Control );
   SystemParameters_SiteLocalMidnight_Sizer.Add( SystemParameters_SiteLocalMidnight_Unit );
   SystemParameters_SiteLocalMidnight_Sizer.AddStretch();

   SystemParameters_ScaleUnit_Label.SetText( "Scale Unit:" );
   SystemParameters_ScaleUnit_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_ScaleUnit_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSScaleUnitParameter->NumberOfElements(); ++i )
      SystemParameters_ScaleUnit_Control.AddItem( TheSSScaleUnitParameter->ElementLabel( i ) );
   SystemParameters_ScaleUnit_Control.SetToolTip( TheSSScaleUnitParameter->Tooltip() );
   SystemParameters_ScaleUnit_Control.OnItemSelected(
           (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   SystemParameters_ScaleUnit_Sizer.SetSpacing( 4 );
   SystemParameters_ScaleUnit_Sizer.Add( SystemParameters_ScaleUnit_Label );
   SystemParameters_ScaleUnit_Sizer.Add( SystemParameters_ScaleUnit_Control );
   SystemParameters_ScaleUnit_Sizer.AddStretch();

   SystemParameters_DataUnit_Label.SetText( "Data Unit:" );
   SystemParameters_DataUnit_Label.SetMinWidth( currentLabelWidth );
   SystemParameters_DataUnit_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSDataUnitParameter->NumberOfElements(); ++i )
      SystemParameters_DataUnit_Control.AddItem( TheSSDataUnitParameter->ElementLabel( i ) );
   SystemParameters_DataUnit_Control.SetToolTip( TheSSDataUnitParameter->Tooltip() );
   SystemParameters_DataUnit_Control.OnItemSelected(
           (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   SystemParameters_DataUnit_Sizer.SetSpacing( 4 );
   SystemParameters_DataUnit_Sizer.Add( SystemParameters_DataUnit_Label );
   SystemParameters_DataUnit_Sizer.Add( SystemParameters_DataUnit_Control );
   SystemParameters_DataUnit_Sizer.AddStretch();

   SystemParameters_Sizer.SetSpacing( 4 );
   SystemParameters_Sizer.Add( SystemParameters_SubframeScale_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_CameraGain_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_CameraResolution_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_SiteLocalMidnight_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_ScaleUnit_Sizer );
   SystemParameters_Sizer.Add( SystemParameters_DataUnit_Sizer );

   SystemParameters_Control.SetSizer( SystemParameters_Sizer );
   SystemParameters_Control.AdjustToContents();

   //

   currentLabelWidth = fnt.Width( String( "Background Expansion:" ) ); // the longest label text

   StarDetectorParameters_SectionBar.SetTitle( "Star Detector Parameters" );
   StarDetectorParameters_SectionBar.SetSection( StarDetectorParameters_Control );
   StarDetectorParameters_SectionBar.OnToggleSection(
           (SectionBar::section_event_handler) &SubframeSelectorInterface::__ToggleSection, w );

   StarDetectorParameters_StructureLayers_Label.SetText( "Structure Layers:" );
   StarDetectorParameters_StructureLayers_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_StructureLayers_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_StructureLayers_Control.SetRange( TheSSStructureLayersParameter->MinimumValue(),
                                                            TheSSStructureLayersParameter->MaximumValue() );
   StarDetectorParameters_StructureLayers_Control.SetToolTip( TheSSStructureLayersParameter->Tooltip() );
   StarDetectorParameters_StructureLayers_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_StructureLayers_Sizer.SetSpacing( 4 );
   StarDetectorParameters_StructureLayers_Sizer.Add( StarDetectorParameters_StructureLayers_Label );
   StarDetectorParameters_StructureLayers_Sizer.Add( StarDetectorParameters_StructureLayers_Control );
   StarDetectorParameters_StructureLayers_Sizer.AddStretch();

   StarDetectorParameters_NoiseLayers_Label.SetText( "Noise Layers:" );
   StarDetectorParameters_NoiseLayers_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_NoiseLayers_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_NoiseLayers_Control.SetRange( TheSSNoiseLayersParameter->MinimumValue(),
                                                        TheSSNoiseLayersParameter->MaximumValue() );
   StarDetectorParameters_NoiseLayers_Control.SetToolTip( TheSSNoiseLayersParameter->Tooltip() );
   StarDetectorParameters_NoiseLayers_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_NoiseLayers_Sizer.SetSpacing( 4 );
   StarDetectorParameters_NoiseLayers_Sizer.Add( StarDetectorParameters_NoiseLayers_Label );
   StarDetectorParameters_NoiseLayers_Sizer.Add( StarDetectorParameters_NoiseLayers_Control );
   StarDetectorParameters_NoiseLayers_Sizer.AddStretch();

   StarDetectorParameters_HotPixelFilterRadius_Label.SetText( "Hot Pixel Filter:" );
   StarDetectorParameters_HotPixelFilterRadius_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_HotPixelFilterRadius_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_HotPixelFilterRadius_Control.SetRange( TheSSHotPixelFilterRadiusParameter->MinimumValue(),
                                                                 TheSSHotPixelFilterRadiusParameter->MaximumValue() );
   StarDetectorParameters_HotPixelFilterRadius_Control.SetToolTip( TheSSHotPixelFilterRadiusParameter->Tooltip() );
   StarDetectorParameters_HotPixelFilterRadius_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_HotPixelFilterRadius_Sizer.SetSpacing( 4 );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Label );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Control );
   StarDetectorParameters_HotPixelFilterRadius_Sizer.AddStretch();

   StarDetectorParameters_ApplyHotPixelFilter_Label.SetText( "Apply Hot Pixel Filter to Detection Image:" );
   StarDetectorParameters_ApplyHotPixelFilter_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_ApplyHotPixelFilter_Control.SetToolTip( TheSSApplyHotPixelFilterParameter->Tooltip() );
   StarDetectorParameters_ApplyHotPixelFilter_Control.OnCheck(
           (Button::check_event_handler) &SubframeSelectorInterface::__CheckboxUpdated, w );

   StarDetectorParameters_ApplyHotPixelFilter_Sizer.SetSpacing( 4 );
   StarDetectorParameters_ApplyHotPixelFilter_Sizer.Add( StarDetectorParameters_ApplyHotPixelFilter_Label );
   StarDetectorParameters_ApplyHotPixelFilter_Sizer.Add( StarDetectorParameters_ApplyHotPixelFilter_Control );
   StarDetectorParameters_ApplyHotPixelFilter_Sizer.AddStretch();

   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetText( "Noise Reduction Filter:" );
   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_NoiseReductionFilterRadius_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_NoiseReductionFilterRadius_Control.SetRange(
           TheSSNoiseReductionFilterRadiusParameter->MinimumValue(),
           TheSSNoiseReductionFilterRadiusParameter->MaximumValue() );
   StarDetectorParameters_NoiseReductionFilterRadius_Control.SetToolTip(
           TheSSNoiseReductionFilterRadiusParameter->Tooltip() );
   StarDetectorParameters_NoiseReductionFilterRadius_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.SetSpacing( 4 );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.Add(
           StarDetectorParameters_NoiseReductionFilterRadius_Label );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.Add(
           StarDetectorParameters_NoiseReductionFilterRadius_Control );
   StarDetectorParameters_NoiseReductionFilterRadius_Sizer.AddStretch();

   StarDetectorParameters_Sensitivity_Control.label.SetText( "Sensitivity:" );
   StarDetectorParameters_Sensitivity_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_Sensitivity_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_Sensitivity_Control.SetReal();
   StarDetectorParameters_Sensitivity_Control.SetRange( TheSSSensitivityParameter->MinimumValue(),
                                                        TheSSSensitivityParameter->MaximumValue() );
   StarDetectorParameters_Sensitivity_Control.SetPrecision( TheSSSensitivityParameter->Precision() );
   StarDetectorParameters_Sensitivity_Control.SetToolTip( TheSSSensitivityParameter->Tooltip() );
   StarDetectorParameters_Sensitivity_Control.OnValueUpdated(
           (NumericEdit::value_event_handler) &SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_PeakResponse_Control.label.SetText( "Peak Response:" );
   StarDetectorParameters_PeakResponse_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_PeakResponse_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_PeakResponse_Control.SetReal();
   StarDetectorParameters_PeakResponse_Control.SetRange( TheSSPeakResponseParameter->MinimumValue(),
                                                         TheSSPeakResponseParameter->MaximumValue() );
   StarDetectorParameters_PeakResponse_Control.SetPrecision( TheSSPeakResponseParameter->Precision() );
   StarDetectorParameters_PeakResponse_Control.SetToolTip( TheSSPeakResponseParameter->Tooltip() );
   StarDetectorParameters_PeakResponse_Control.OnValueUpdated(
           (NumericEdit::value_event_handler) &SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_MaxDistortion_Control.label.SetText( "Max Distortion:" );
   StarDetectorParameters_MaxDistortion_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_MaxDistortion_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_MaxDistortion_Control.SetReal();
   StarDetectorParameters_MaxDistortion_Control.SetRange( TheSSMaxDistortionParameter->MinimumValue(),
                                                          TheSSMaxDistortionParameter->MaximumValue() );
   StarDetectorParameters_MaxDistortion_Control.SetPrecision( TheSSMaxDistortionParameter->Precision() );
   StarDetectorParameters_MaxDistortion_Control.SetToolTip( TheSSMaxDistortionParameter->Tooltip() );
   StarDetectorParameters_MaxDistortion_Control.OnValueUpdated(
           (NumericEdit::value_event_handler) &SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_UpperLimit_Control.label.SetText( "Upper Limit:" );
   StarDetectorParameters_UpperLimit_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_UpperLimit_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_UpperLimit_Control.SetReal();
   StarDetectorParameters_UpperLimit_Control.SetRange( TheSSUpperLimitParameter->MinimumValue(),
                                                       TheSSUpperLimitParameter->MaximumValue() );
   StarDetectorParameters_UpperLimit_Control.SetPrecision( TheSSUpperLimitParameter->Precision() );
   StarDetectorParameters_UpperLimit_Control.SetToolTip( TheSSUpperLimitParameter->Tooltip() );
   StarDetectorParameters_UpperLimit_Control.OnValueUpdated(
           (NumericEdit::value_event_handler) &SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_BackgroundExpansion_Label.SetText( "Background Expansion:" );
   StarDetectorParameters_BackgroundExpansion_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_BackgroundExpansion_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_BackgroundExpansion_Control.SetRange( TheSSBackgroundExpansionParameter->MinimumValue(),
                                                                TheSSBackgroundExpansionParameter->MaximumValue() );
   StarDetectorParameters_BackgroundExpansion_Control.SetToolTip( TheSSBackgroundExpansionParameter->Tooltip() );
   StarDetectorParameters_BackgroundExpansion_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_BackgroundExpansion_Sizer.SetSpacing( 4 );
   StarDetectorParameters_BackgroundExpansion_Sizer.Add( StarDetectorParameters_BackgroundExpansion_Label );
   StarDetectorParameters_BackgroundExpansion_Sizer.Add( StarDetectorParameters_BackgroundExpansion_Control );
   StarDetectorParameters_BackgroundExpansion_Sizer.AddStretch();

   StarDetectorParameters_XYStretch_Control.label.SetText( "XY Stretch:" );
   StarDetectorParameters_XYStretch_Control.label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_XYStretch_Control.label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_XYStretch_Control.SetReal();
   StarDetectorParameters_XYStretch_Control.SetRange( TheSSXYStretchParameter->MinimumValue(),
                                                      TheSSXYStretchParameter->MaximumValue() );
   StarDetectorParameters_XYStretch_Control.SetPrecision( TheSSXYStretchParameter->Precision() );
   StarDetectorParameters_XYStretch_Control.SetToolTip( TheSSXYStretchParameter->Tooltip() );
   StarDetectorParameters_XYStretch_Control.OnValueUpdated(
           (NumericEdit::value_event_handler) &SubframeSelectorInterface::__RealValueUpdated, w );

   StarDetectorParameters_PSFFit_Label.SetText( "PSF Fit:" );
   StarDetectorParameters_PSFFit_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_PSFFit_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSPSFFitParameter->NumberOfElements(); ++i )
      StarDetectorParameters_PSFFit_Control.AddItem( TheSSPSFFitParameter->ElementLabel( i ) );

   StarDetectorParameters_PSFFit_Control.SetToolTip( TheSSPSFFitParameter->Tooltip() );
   StarDetectorParameters_PSFFit_Control.OnItemSelected(
           (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   StarDetectorParameters_PSFFitCircular_Label.SetText( "Circular:" );
   StarDetectorParameters_PSFFitCircular_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_PSFFitCircular_Control.SetToolTip( TheSSPSFFitCircularParameter->Tooltip() );
   StarDetectorParameters_PSFFitCircular_Control.OnCheck(
           (Button::check_event_handler) &SubframeSelectorInterface::__CheckboxUpdated, w );

   StarDetectorParameters_PSFFit_Sizer.SetSpacing( 4 );
   StarDetectorParameters_PSFFit_Sizer.Add( StarDetectorParameters_PSFFit_Label );
   StarDetectorParameters_PSFFit_Sizer.Add( StarDetectorParameters_PSFFit_Control );
   StarDetectorParameters_PSFFit_Sizer.Add( StarDetectorParameters_PSFFitCircular_Label );
   StarDetectorParameters_PSFFit_Sizer.Add( StarDetectorParameters_PSFFitCircular_Control );
   StarDetectorParameters_PSFFit_Sizer.AddStretch();

   StarDetectorParameters_Pedestal_Label.SetText( "Pedestal:" );
   StarDetectorParameters_Pedestal_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_Pedestal_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   StarDetectorParameters_Pedestal_Control.SetRange( TheSSPedestalParameter->MinimumValue(),
                                                     TheSSPedestalParameter->MaximumValue() );
   StarDetectorParameters_Pedestal_Control.SetToolTip( TheSSPedestalParameter->Tooltip() );
   StarDetectorParameters_Pedestal_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_Pedestal_Unit.SetText( "Data Numbers" );
   StarDetectorParameters_Pedestal_Unit.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );

   StarDetectorParameters_Pedestal_Sizer.SetSpacing( 4 );
   StarDetectorParameters_Pedestal_Sizer.Add( StarDetectorParameters_Pedestal_Label );
   StarDetectorParameters_Pedestal_Sizer.Add( StarDetectorParameters_Pedestal_Control );
   StarDetectorParameters_Pedestal_Sizer.Add( StarDetectorParameters_Pedestal_Unit );
   StarDetectorParameters_Pedestal_Sizer.AddStretch();

   currentLabelWidth = fnt.Width( String( "Height:" ) ); // the longest label text

   StarDetectorParameters_ROIX0_Label.SetText( "Left:" );
   StarDetectorParameters_ROIX0_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIX0_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_ROIX0_Label.SetToolTip( "<p>X pixel coordinate of the upper-left corner of the ROI.</p>" );

   StarDetectorParameters_ROIX0_Control.SetRange( TheSSROIX0Parameter->MinimumValue(),
                                                  TheSSROIX0Parameter->MaximumValue() );
   StarDetectorParameters_ROIX0_Control.SetToolTip( "<p>X pixel coordinate of the upper-left corner of the ROI.</p>" );
   StarDetectorParameters_ROIX0_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROIY0_Label.SetText( "Top:" );
   StarDetectorParameters_ROIY0_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIY0_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_ROIY0_Label.SetToolTip( "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>" );

   StarDetectorParameters_ROIY0_Control.SetRange( TheSSROIY0Parameter->MinimumValue(),
                                                  TheSSROIY0Parameter->MaximumValue() );
   StarDetectorParameters_ROIY0_Control.SetToolTip( "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>" );
   StarDetectorParameters_ROIY0_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROIRow1_Sizer.SetSpacing( 4 );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIX0_Label );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIX0_Control );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIY0_Label );
   StarDetectorParameters_ROIRow1_Sizer.Add( StarDetectorParameters_ROIY0_Control );
   StarDetectorParameters_ROIRow1_Sizer.AddStretch();

   StarDetectorParameters_ROIWidth_Label.SetText( "Width:" );
   StarDetectorParameters_ROIWidth_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIWidth_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_ROIWidth_Label.SetToolTip( "<p>Width of the ROI in pixels.</p>" );

   StarDetectorParameters_ROIWidth_Control.SetRange( TheSSROIX1Parameter->MinimumValue(),
                                                     TheSSROIX1Parameter->MaximumValue() );
   StarDetectorParameters_ROIWidth_Control.SetToolTip( "<p>Width of the ROI in pixels.</p>" );
   StarDetectorParameters_ROIWidth_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROIHeight_Label.SetText( "Height:" );
   StarDetectorParameters_ROIHeight_Label.SetMinWidth( currentLabelWidth );
   StarDetectorParameters_ROIHeight_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   StarDetectorParameters_ROIHeight_Label.SetToolTip( "<p>Height of the ROI in pixels.</p>" );

   StarDetectorParameters_ROIHeight_Control.SetRange( TheSSROIY1Parameter->MinimumValue(),
                                                      TheSSROIY1Parameter->MaximumValue() );
   StarDetectorParameters_ROIHeight_Control.SetToolTip( "<p>Height of the ROI in pixels.</p>" );
   StarDetectorParameters_ROIHeight_Control.OnValueUpdated(
           (SpinBox::value_event_handler) &SubframeSelectorInterface::__IntegerValueUpdated, w );

   StarDetectorParameters_ROISelectPreview_Button.SetText( "From Preview" );
   StarDetectorParameters_ROISelectPreview_Button.SetToolTip(
           "<p>Import ROI coordinates from an existing preview.</p>" );
   StarDetectorParameters_ROISelectPreview_Button.OnClick(
           (Button::click_event_handler) &SubframeSelectorInterface::__ButtonClick, w );
   StarDetectorParameters_ROISelectPreview_Button.OnViewDrag(
           (Control::view_drag_event_handler) &SubframeSelectorInterface::__StarDetector_ViewDrag, w );
   StarDetectorParameters_ROISelectPreview_Button.OnViewDrop(
           (Control::view_drop_event_handler) &SubframeSelectorInterface::__StarDetector_ViewDrop, w );

   StarDetectorParameters_ROIRow2_Sizer.SetSpacing( 4 );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIWidth_Label );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIWidth_Control );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIHeight_Label );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROIHeight_Control );
   StarDetectorParameters_ROIRow2_Sizer.AddSpacing( 12 );
   StarDetectorParameters_ROIRow2_Sizer.Add( StarDetectorParameters_ROISelectPreview_Button );
   StarDetectorParameters_ROIRow2_Sizer.AddStretch();

   StarDetectorParameters_Sizer.SetSpacing( 4 );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_StructureLayers_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_NoiseLayers_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_HotPixelFilterRadius_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_ApplyHotPixelFilter_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_NoiseReductionFilterRadius_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_Sensitivity_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_PeakResponse_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_MaxDistortion_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_UpperLimit_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_BackgroundExpansion_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_XYStretch_Control );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_PSFFit_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_Pedestal_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_ROIRow1_Sizer );
   StarDetectorParameters_Sizer.Add( StarDetectorParameters_ROIRow2_Sizer );

   StarDetectorParameters_Control.SetSizer( StarDetectorParameters_Sizer );
   StarDetectorParameters_Control.AdjustToContents();

   //

   currentLabelWidth = fnt.Width( String( "Directory:" ) ); // the longest label text

   OutputFiles_SectionBar.SetTitle( "Output Files" );
   OutputFiles_SectionBar.SetSection( OutputFiles_Control );
   OutputFiles_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&SubframeSelectorInterface::__ToggleSection, w );

   OutputDirectory_Label.SetText( "Directory:" );
   OutputDirectory_Label.SetFixedWidth( currentLabelWidth );
   OutputDirectory_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputDirectory_Edit.SetToolTip( TheSSOutputDirectoryParameter->Tooltip() );
   OutputDirectory_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::__TextUpdateCompleted, w );
   OutputDirectory_Edit.OnFileDrag( (Control::file_drag_event_handler)&SubframeSelectorInterface::__FileDrag, w );
   OutputDirectory_Edit.OnFileDrop( (Control::file_drop_event_handler)&SubframeSelectorInterface::__FileDrop, w );

   OutputDirectory_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDirectory_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputDirectory_ToolButton.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__ButtonClick, w );

   OutputDirectory_Sizer.SetSpacing( 4 );
   OutputDirectory_Sizer.Add( OutputDirectory_Label );
   OutputDirectory_Sizer.Add( OutputDirectory_Edit, 100 );
   OutputDirectory_Sizer.Add( OutputDirectory_ToolButton );

   OutputPrefix_Label.SetText( "Prefix:" );
   OutputPrefix_Label.SetFixedWidth( currentLabelWidth );
   OutputPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPrefix_Edit.SetToolTip( TheSSOutputPrefixParameter->Tooltip() );
   OutputPrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::__TextUpdateCompleted, w );

   OutputPostfix_Label.SetText( "Postfix:" );
   OutputPostfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputPostfix_Edit.SetToolTip( TheSSOutputPostfixParameter->Tooltip() );
   OutputPostfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::__TextUpdateCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( OutputPrefix_Label );
   OutputChunks_Sizer.Add( OutputPrefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( OutputPostfix_Label );
   OutputChunks_Sizer.Add( OutputPostfix_Edit );
   OutputChunks_Sizer.AddStretch();

   OutputKeyword_Label.SetText( "Keyword:" );
   OutputKeyword_Label.SetFixedWidth( currentLabelWidth );
   OutputKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputKeyword_Edit.SetToolTip( TheSSOutputKeywordParameter->Tooltip() );
   OutputKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&SubframeSelectorInterface::__TextUpdateCompleted, w );

   OutputKeyword_Sizer.SetSpacing( 4 );
   OutputKeyword_Sizer.Add( OutputKeyword_Label );
   OutputKeyword_Sizer.Add( OutputKeyword_Edit );
   OutputKeyword_Sizer.AddStretch();

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( TheSSOverwriteExistingFilesParameter->Tooltip() );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&SubframeSelectorInterface::__ButtonClick, w );

   OnError_Label.SetText( "On error:" );
   OnError_Label.SetFixedWidth( currentLabelWidth );
   OnError_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   for ( size_type i = 0; i < TheSSOnErrorParameter->NumberOfElements(); ++i )
      OnError_ComboBox.AddItem( TheSSOnErrorParameter->ElementLabel( i ) );

   OnError_ComboBox.SetToolTip( TheSSOnErrorParameter->Tooltip() );
   OnError_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SubframeSelectorInterface::__ComboSelected, w );

   OutputMisc_Sizer.SetSpacing( 4 );
   OutputMisc_Sizer.Add( OverwriteExistingFiles_CheckBox );
   OutputMisc_Sizer.AddStretch();
   OutputMisc_Sizer.Add( OnError_Label );
   OutputMisc_Sizer.Add( OnError_ComboBox );

   OutputFiles_Sizer.SetSpacing( 4 );
   OutputFiles_Sizer.Add( OutputDirectory_Sizer );
   OutputFiles_Sizer.Add( OutputChunks_Sizer );
   OutputFiles_Sizer.Add( OutputKeyword_Sizer );
   OutputFiles_Sizer.Add( OutputMisc_Sizer );

   OutputFiles_Control.SetSizer( OutputFiles_Sizer );
   OutputFiles_Control.AdjustToContents();

   //

   currentLabelWidth = fnt.Width( String( "Weighting:" ) ); // the longest label text

   ExpressionParameters_SectionBar.SetTitle( "Expressions" );
   ExpressionParameters_SectionBar.SetSection( ExpressionParameters_Control );
   ExpressionParameters_SectionBar.OnToggleSection(
           (SectionBar::section_event_handler) &SubframeSelectorInterface::__ToggleSection, w );

   ExpressionParameters_Approval_Status.SetBitmap( Bitmap( w.ScaledResource( ":/browser/enabled.png" ) ) );

   ExpressionParameters_Approval_Label.SetText( "Approval:" );
   ExpressionParameters_Approval_Label.SetMinWidth( currentLabelWidth );
   ExpressionParameters_Approval_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   ExpressionParameters_Approval_Control.SetScaledMinWidth( 300 );
   ExpressionParameters_Approval_Control.SetToolTip( TheSSApprovalExpressionParameter->Tooltip() );
   ExpressionParameters_Approval_Control.OnTextUpdated(
           (Edit::text_event_handler) &SubframeSelectorInterface::__TextUpdated, w );
   ExpressionParameters_Approval_Control.OnEditCompleted(
           (Edit::edit_event_handler) &SubframeSelectorInterface::__TextUpdateCompleted, w );
   ExpressionParameters_Approval_Control.OnReturnPressed(
           (Edit::edit_event_handler) &SubframeSelectorInterface::__TextUpdateCompleted, w );

   ExpressionParameters_Approval_Sizer.SetSpacing( 4 );
   ExpressionParameters_Approval_Sizer.Add( ExpressionParameters_Approval_Status, 0 );
   ExpressionParameters_Approval_Sizer.Add( ExpressionParameters_Approval_Label, 0 );
   ExpressionParameters_Approval_Sizer.Add( ExpressionParameters_Approval_Control, 100 );

   ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( w.ScaledResource( ":/browser/enabled.png" ) ) );

   ExpressionParameters_Weighting_Label.SetText( "Weighting:" );
   ExpressionParameters_Weighting_Label.SetMinWidth( currentLabelWidth );
   ExpressionParameters_Weighting_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   ExpressionParameters_Weighting_Control.SetScaledMinWidth( 300 );
   ExpressionParameters_Weighting_Control.SetToolTip( TheSSWeightingExpressionParameter->Tooltip() );
   ExpressionParameters_Weighting_Control.OnTextUpdated(
           (Edit::text_event_handler) &SubframeSelectorInterface::__TextUpdated, w );
   ExpressionParameters_Weighting_Control.OnEditCompleted(
           (Edit::edit_event_handler) &SubframeSelectorInterface::__TextUpdateCompleted, w );
   ExpressionParameters_Weighting_Control.OnReturnPressed(
           (Edit::edit_event_handler) &SubframeSelectorInterface::__TextUpdateCompleted, w );

   ExpressionParameters_Weighting_Sizer.SetSpacing( 4 );
   ExpressionParameters_Weighting_Sizer.Add( ExpressionParameters_Weighting_Status, 0 );
   ExpressionParameters_Weighting_Sizer.Add( ExpressionParameters_Weighting_Label, 0 );
   ExpressionParameters_Weighting_Sizer.Add( ExpressionParameters_Weighting_Control, 100 );

   ExpressionParameters_Sizer.SetSpacing( 4 );
   ExpressionParameters_Sizer.Add( ExpressionParameters_Approval_Sizer );
   ExpressionParameters_Sizer.Add( ExpressionParameters_Weighting_Sizer );

   ExpressionParameters_Control.SetSizer( ExpressionParameters_Sizer );
   ExpressionParameters_Control.AdjustToContents();

   //

   MeasurementTable_SectionBar.SetTitle( "Measurements Table" );
   MeasurementTable_SectionBar.SetSection( MeasurementTable_Control );
   MeasurementTable_SectionBar.OnToggleSection(
           (SectionBar::section_event_handler) &SubframeSelectorInterface::__ToggleSection, w );

   for ( size_type i = 0; i < TheSSSortingPropertyParameter->NumberOfElements(); ++i )
      MeasurementsTable_SortingProperty_Control.AddItem( TheSSSortingPropertyParameter->ElementLabel( i ) );

   MeasurementsTable_SortingProperty_Control.OnItemSelected(
           (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   MeasurementsTable_SortingMode_Control.AddItem( "Asc." );
   MeasurementsTable_SortingMode_Control.AddItem( "Desc." );
   MeasurementsTable_SortingMode_Control.OnItemSelected(
           (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   MeasurementsTable_ToggleApproved_PushButton.SetText( "Toggle Approve" );
   MeasurementsTable_ToggleApproved_PushButton.SetToolTip( "<p>Toggle the approved state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleApproved_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__MeasurementImages_Click,
                                                     w );

   MeasurementsTable_ToggleLocked_PushButton.SetText( "Toggle Lock" );
   MeasurementsTable_ToggleLocked_PushButton.SetToolTip( "<p>Toggle the locked state of currently selected measurements.</p>" );
   MeasurementsTable_ToggleLocked_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__MeasurementImages_Click,
                                                       w );

   MeasurementsTable_Invert_PushButton.SetText( "Invert" );
   MeasurementsTable_Invert_PushButton.SetToolTip( "<p>Invert the current selection of measurements.</p>" );
   MeasurementsTable_Invert_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__MeasurementImages_Click,
                                                         w );

   MeasurementsTable_Remove_PushButton.SetText( "Remove" );
   MeasurementsTable_Remove_PushButton.SetToolTip( "<p>Remove the selected measurements.</p>" );
   MeasurementsTable_Remove_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__MeasurementImages_Click, w );

   MeasurementsTable_Clear_PushButton.SetText( "Clear" );
   MeasurementsTable_Clear_PushButton.SetToolTip( "<p>Clear the list of measurements.</p>" );
   MeasurementsTable_Clear_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__MeasurementImages_Click, w );

   MeasurementsTable_CSV_PushButton.SetText( "Save CSV" );
   MeasurementsTable_CSV_PushButton.SetToolTip( "<p>Export the table as a CSV file.</p>" );
   MeasurementsTable_CSV_PushButton.OnClick( (Button::click_event_handler) &SubframeSelectorInterface::__MeasurementImages_Click, w );

   MeasurementsTable_Top1_Sizer.SetSpacing( 4 );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_SortingProperty_Control );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_SortingMode_Control );
   MeasurementsTable_Top1_Sizer.AddStretch( 100 );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_ToggleApproved_PushButton );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_ToggleLocked_PushButton );
   MeasurementsTable_Top1_Sizer.Add( MeasurementsTable_Invert_PushButton );

   MeasurementsTable_Top2_Sizer.SetSpacing( 4 );
   MeasurementsTable_Top2_Sizer.AddStretch( 100 );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Remove_PushButton );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_Clear_PushButton );
   MeasurementsTable_Top2_Sizer.Add( MeasurementsTable_CSV_PushButton );

   MeasurementTable_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   MeasurementTable_TreeBox.SetScaledMinWidth( 400 );
   MeasurementTable_TreeBox.SetNumberOfColumns( 5 );
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
   MeasurementTable_TreeBox.EnableMultipleSelections();
   MeasurementTable_TreeBox.DisableRootDecoration();
   MeasurementTable_TreeBox.EnableAlternateRowColor();
   MeasurementTable_TreeBox.OnCurrentNodeUpdated(
           (TreeBox::node_navigation_event_handler) &SubframeSelectorInterface::__MeasurementImages_CurrentNodeUpdated,
           w );
   MeasurementTable_TreeBox.OnNodeActivated(
           (TreeBox::node_event_handler) &SubframeSelectorInterface::__MeasurementImages_NodeActivated, w );

   MeasurementTable_Sizer.SetSpacing( 4 );
   MeasurementTable_Sizer.Add( MeasurementsTable_Top1_Sizer );
   MeasurementTable_Sizer.Add( MeasurementsTable_Top2_Sizer );
   MeasurementTable_Sizer.Add( MeasurementTable_TreeBox, 100 );

   MeasurementTable_Control.SetSizer( MeasurementTable_Sizer );
   MeasurementTable_Control.AdjustToContents();

   //

   MeasurementGraph_SectionBar.SetTitle( "Measurements Graph" );
   MeasurementGraph_SectionBar.SetSection( MeasurementGraph_Control );
   MeasurementGraph_SectionBar.OnToggleSection(
           (SectionBar::section_event_handler) &SubframeSelectorInterface::__ToggleSection, w );

   for ( size_type i = 0; i < TheSSGraphPropertyParameter->NumberOfElements(); ++i )
      MeasurementGraph_GraphProperty_Control.AddItem( TheSSGraphPropertyParameter->ElementLabel( i ) );

   MeasurementGraph_GraphProperty_Control.OnItemSelected(
           (ComboBox::item_event_handler) &SubframeSelectorInterface::__ComboSelected, w );

   MeasurementGraph_Save_PushButton.SetText( "Save PDF" );
   MeasurementGraph_Save_PushButton.SetToolTip( "<p>Export the graph as a PDF file.</p>" );
   MeasurementGraph_Save_PushButton.OnClick(
           (Button::click_event_handler) &SubframeSelectorInterface::__MeasurementImages_Click, w );

   MeasurementGraph_Top_Sizer.SetSpacing( 4 );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_GraphProperty_Control );
   MeasurementGraph_Top_Sizer.Add( MeasurementGraph_Save_PushButton );

   MeasurementGraph_Graph.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   MeasurementGraph_Graph.SetScaledMinWidth( 400 );
   MeasurementGraph_Graph.SetZoomFactor( MeasurementGraph_Graph.DisplayPixelRatio() );
   MeasurementGraph_Graph.SetBackgroundColor( MeasurementGraph_Control.BackgroundColor() );
   MeasurementGraph_Graph.OnApprove(
           (GraphWebView::approve_event_handler) &SubframeSelectorInterface::__MeasurementGraph_Approve, w );
   MeasurementGraph_Graph.OnUnlock(
           (GraphWebView::unlock_event_handler) &SubframeSelectorInterface::__MeasurementGraph_Unlock, w );

   MeasurementGraph_Sizer.SetSpacing( 4 );
   MeasurementGraph_Sizer.Add( MeasurementGraph_Top_Sizer );
   MeasurementGraph_Sizer.Add( MeasurementGraph_Graph, 100 );
   MeasurementGraph_Sizer.SetAlignment( MeasurementGraph_Top_Sizer, Sizer::item_alignment::Center );

   MeasurementGraph_Control.SetSizer( MeasurementGraph_Sizer );
   MeasurementGraph_Control.AdjustToContents();

   //

   Left_Sizer.SetMargin( 8 );
   Left_Sizer.SetSpacing( 6 );

   Left_Sizer.Add( Routine_Sizer );
   Left_Sizer.SetAlignment( Routine_Sizer, Sizer::item_alignment::Center );
   Left_Sizer.Add( SubframeImages_SectionBar );
   Left_Sizer.Add( SubframeImages_Control );
   Left_Sizer.Add( SystemParameters_SectionBar );
   Left_Sizer.Add( SystemParameters_Control );
   Left_Sizer.Add( StarDetectorParameters_SectionBar );
   Left_Sizer.Add( StarDetectorParameters_Control );
   Left_Sizer.Add( OutputFiles_SectionBar );
   Left_Sizer.Add( OutputFiles_Control );
   Left_Sizer.AddStretch( 100 );

   Right_Sizer.SetMargin( 8 );
   Right_Sizer.SetSpacing( 6 );
   Right_Sizer.Add( ExpressionParameters_SectionBar );
   Right_Sizer.Add( ExpressionParameters_Control );
   Right_Sizer.Add( MeasurementTable_SectionBar );
   Right_Sizer.Add( MeasurementTable_Control, 50 );
   Right_Sizer.Add( MeasurementGraph_SectionBar );
   Right_Sizer.Add( MeasurementGraph_Control, 100 );

   Global_Sizer.Add( Left_Sizer, 20 );
   Global_Sizer.Add( Right_Sizer, 80 );

   w.SetSizer( Global_Sizer );

   SystemParameters_Control.Hide();
   StarDetectorParameters_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.cpp - Released 2017-11-05T16:00:00Z
