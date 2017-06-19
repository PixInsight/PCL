//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0300
// ----------------------------------------------------------------------------
// DefectMapInterface.cpp - Released 2017-05-17T17:41:55Z
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

#include "DefectMapInterface.h"
#include "DefectMapProcess.h"

#include <pcl/ComboBox.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ImageWindow.h>
#include <pcl/PushButton.h>
#include <pcl/ViewList.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

DefectMapInterface* TheDefectMapInterface = nullptr;

// ----------------------------------------------------------------------------

#include "DefectMapIcon.xpm"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DefectMapInterface::DefectMapInterface() :
   instance( TheDefectMapProcess )
{
   TheDefectMapInterface = this;
}

DefectMapInterface::~DefectMapInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString DefectMapInterface::Id() const
{
   return "DefectMap";
}

MetaProcess* DefectMapInterface::Process() const
{
   return TheDefectMapProcess;
}

const char** DefectMapInterface::IconImageXPM() const
{
   return DefectMapIcon_XPM;
}

void DefectMapInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void DefectMapInterface::ResetInstance()
{
   DefectMapInstance defaultInstance( TheDefectMapProcess );
   ImportProcess( defaultInstance );
}

bool DefectMapInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "DefectMap" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheDefectMapProcess;
}

ProcessImplementation* DefectMapInterface::NewProcess() const
{
   return new DefectMapInstance( instance );
}

bool DefectMapInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const DefectMapInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a DefectMap instance.";
   return false;
}

bool DefectMapInterface::RequiresInstanceValidation() const
{
   return true;
}

bool DefectMapInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void DefectMapInterface::UpdateControls()
{
   GUI->MapImage_Edit.SetText( instance.mapId );
   GUI->Operation_ComboBox.SetCurrentItem( instance.operation );
   GUI->Structure_ComboBox.SetCurrentItem( instance.structure );
   GUI->IsCFA_CheckBox.SetChecked( instance.isCFA );
}

// ----------------------------------------------------------------------------

void DefectMapInterface::__MapImage_EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text();
      id.Trim();
      if ( !id.IsEmpty() && !id.IsValidIdentifier() )
         throw Error( "Invalid image identifier: " + id );
      instance.mapId = id;
   }
   ERROR_HANDLER
   UpdateControls();
}

void DefectMapInterface::__MapImage_Click( Button& sender, bool checked )
{
   ViewSelectionDialog d( instance.mapId, false/*allowPreviews*/ );
   d.SetWindowTitle( "Select Defect Map Image" );
   if ( d.Execute() )
   {
      instance.mapId = d.Id();
      UpdateControls();
   }
}

void DefectMapInterface::__Operation_ItemSelected( ComboBox& sender, int itemIndex )
{
   instance.operation = itemIndex;
}

void DefectMapInterface::__Structure_ItemSelected( ComboBox& sender, int itemIndex )
{
   instance.structure = itemIndex;
}

void DefectMapInterface::__ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->IsCFA_CheckBox )
      instance.isCFA = checked;
}

void DefectMapInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->MapImage_Edit )
      wantsView = !view.IsPreview();
}

void DefectMapInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->MapImage_Edit )
      if ( view.IsMainView() )
      {
         instance.mapId = view.Id();
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

DefectMapInterface::GUIData::GUIData( DefectMapInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Defect map:" ) + 'M' );
   int editWidth1 = fnt.Width( String( 'M', 35 ) );
   int comboWidth1 = fnt.Width( String( 'M', 15 ) );

   //

   const char* mapIdToolTip =
   "<p>Defect map image</p>"
   "<p>DefectMap uses a <i>defect map</i> image to find defective pixels. In a defect map, "
   "black (zero) pixels correspond to bad or defective pixels. Bad pixels will be replaced "
   "with new values computed from neighbor pixels.</p>"
   "<p>When using convolution replacement operations (Gaussian, mean), non-zero pixel values "
   "represent the pixel weights that will be used for convolution. Morphological replacement "
   "operations (minimum, maximum, median) use binarized (0/1) map values.</p>";

   MapImage_Label.SetText( "Defect map:" );
   MapImage_Label.SetFixedWidth( labelWidth1 );
   MapImage_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MapImage_Label.SetToolTip( mapIdToolTip );

   MapImage_Edit.SetToolTip( mapIdToolTip );
   MapImage_Edit.SetMinWidth( editWidth1 );
   MapImage_Edit.OnEditCompleted( (Edit::edit_event_handler)&DefectMapInterface::__MapImage_EditCompleted, w );
   MapImage_Edit.OnViewDrag( (Control::view_drag_event_handler)&DefectMapInterface::__ViewDrag, w );
   MapImage_Edit.OnViewDrop( (Control::view_drop_event_handler)&DefectMapInterface::__ViewDrop, w );

   MapImage_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   MapImage_ToolButton.SetScaledFixedSize( 19, 19 );
   MapImage_ToolButton.SetToolTip( "<p>Select the defect map image</p>" );
   MapImage_ToolButton.OnClick( (Button::click_event_handler)&DefectMapInterface::__MapImage_Click, w );

   MapImage_Sizer.SetSpacing( 4 );
   MapImage_Sizer.Add( MapImage_Label );
   MapImage_Sizer.Add( MapImage_Edit, 100 );
   MapImage_Sizer.Add( MapImage_ToolButton );

   //

   const char* operationToolTip =
   "<p>Bad pixel replacement operation</p>"
   "<p>Specify the convolution or morphological operation used to replace bad pixels with "
   "new pixels computed from neighbor values. Mean and Gaussian will be applied by convolution; "
   "minimum, maximum and median will be applied as morphological operators.</p>";

   Operation_Label.SetText( "Operation:" );
   Operation_Label.SetFixedWidth( labelWidth1 );
   Operation_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Operation_Label.SetToolTip( operationToolTip );

   Operation_ComboBox.AddItem( "Mean" );
   Operation_ComboBox.AddItem( "Gaussian" );
   Operation_ComboBox.AddItem( "Minimum" );
   Operation_ComboBox.AddItem( "Maximum" );
   Operation_ComboBox.AddItem( "Median" );
   Operation_ComboBox.SetMinWidth( comboWidth1 );
   Operation_ComboBox.SetToolTip( operationToolTip );
   Operation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DefectMapInterface::__Operation_ItemSelected, w );

   Operation_Sizer.SetSpacing( 4 );
   Operation_Sizer.Add( Operation_Label );
   Operation_Sizer.Add( Operation_ComboBox );
   Operation_Sizer.AddStretch();

   //

   const char* structureToolTip =
   "<p>Structural element</p>"
   "<p>Specify the shape of a structural element used to apply a convolution or morphological "
   "transformation for bad pixel replacement. The structural element defines which neighbor "
   "pixels will be used to compute replacement values.</p>";

   Structure_Label.SetText( "Structure:" );
   Structure_Label.SetFixedWidth( labelWidth1 );
   Structure_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Structure_Label.SetToolTip( structureToolTip );

   Structure_ComboBox.AddItem( "Square" );
   Structure_ComboBox.AddItem( "Circular" );
   Structure_ComboBox.AddItem( "Horizontal" );
   Structure_ComboBox.AddItem( "Vertical" );
   Structure_ComboBox.SetMinWidth( comboWidth1 );
   Structure_ComboBox.SetToolTip( structureToolTip );
   Structure_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DefectMapInterface::__Structure_ItemSelected, w );

   IsCFA_CheckBox.SetText( "CFA images" );
   IsCFA_CheckBox.SetToolTip( "<p>Check this if you are loading Bayered one-shot color or DSLR raw images.</p>" );
   IsCFA_CheckBox.OnClick( (pcl::Button::click_event_handler)&DefectMapInterface::__ItemClicked, w );

   Structure_Sizer.SetSpacing( 4 );
   Structure_Sizer.Add( Structure_Label );
   Structure_Sizer.Add( Structure_ComboBox );
   Structure_Sizer.AddStretch();
   Structure_Sizer.Add( IsCFA_CheckBox );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( MapImage_Sizer );
   Global_Sizer.Add( Operation_Sizer );
   Global_Sizer.Add( Structure_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DefectMapInterface.cpp - Released 2017-05-17T17:41:55Z
