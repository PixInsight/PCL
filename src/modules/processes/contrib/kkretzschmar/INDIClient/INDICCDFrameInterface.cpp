//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.09.0153
// ----------------------------------------------------------------------------
// INDICCDFrameInterface.cpp - Released 2016/05/08 20:36:42 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#include "INDICCDFrameInstance.h"
#include "INDICCDFrameInterface.h"
#include "INDICCDFrameParameters.h"
#include "INDICCDFrameProcess.h"
#include "INDIDeviceControllerInstance.h"

#include "INDI/basedevice.h"
#include "INDI/indiapi.h"
#include "INDI/indibase.h"

#include <pcl/Console.h>
#include <pcl/Dialog.h>

#include <assert.h>

// time out in seconds
#define PCL_TIMEOUT 60

namespace pcl
{

// ----------------------------------------------------------------------------

INDICCDFrameInterface* TheINDICCDFrameInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "INDICCDFrameProcess.xpm" // ### TODO

// ----------------------------------------------------------------------------

// ### Make this a public PCL API?
class SimpleGetStringDialog : public Dialog
{
public:

   SimpleGetStringDialog( const String& label, const String& text = String(), const String& title = "INDICCDFrame" ) :
      Dialog()
   {
      Text_Label.SetText( label );
      Text_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      Text_Edit.SetText( text.Trimmed() );
      Text_Edit.SetMinWidth( 32*Font().Width( 'm' ) );

      Text_Sizer.SetSpacing( 6 );
      Text_Sizer.Add( Text_Label );
      Text_Sizer.Add( Text_Edit, 100 );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.OnClick( (Button::click_event_handler)&SimpleGetStringDialog::e_Click, *this );

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.OnClick( (Button::click_event_handler)&SimpleGetStringDialog::e_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( Text_Sizer );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      SetWindowTitle( title );

      OnShow( (Control::event_handler)&SimpleGetStringDialog::e_Show, *this );
   }

   String Text() const
   {
      return Text_Edit.Text().Trimmed();
   }

private:

   VerticalSizer     Global_Sizer;
      HorizontalSizer   Text_Sizer;
         Label             Text_Label;
         Edit              Text_Edit;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void e_Show( Control& )
   {
      AdjustToContents();
      SetFixedHeight();
      SetMinWidth();
   }

   void e_Click( Button& sender, bool checked )
   {
      if ( sender == OK_PushButton )
         Ok();
      else if ( sender == Cancel_PushButton )
         Cancel();
   }
};

// ----------------------------------------------------------------------------

INDICCDFrameInterface::INDICCDFrameInterface() :
   ProcessInterface(),
   m_device(),
   m_execution( nullptr ),
   GUI( nullptr )
{
   TheINDICCDFrameInterface = this;
}

INDICCDFrameInterface::~INDICCDFrameInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString INDICCDFrameInterface::Id() const
{
   return "INDICCDFrame";
}

MetaProcess* INDICCDFrameInterface::Process() const
{
   return TheINDICCDFrameProcess;
}

const char** INDICCDFrameInterface::IconImageXPM() const
{
   return 0; // INDICCDFrameProcess_XPM; // ### TODO
}

InterfaceFeatures INDICCDFrameInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void INDICCDFrameInterface::ResetInstance()
{
   INDIDeviceControllerInstance defaultInstance( TheINDICCDFrameProcess );
   ImportProcess( defaultInstance );
}

bool INDICCDFrameInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "INDI CCD Controller" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheINDICCDFrameProcess;
}

ProcessImplementation* INDICCDFrameInterface::NewProcess() const
{
   INDICCDFrameInstance* instance = new INDICCDFrameInstance( TheINDICCDFrameProcess );
   instance->p_deviceName = m_device;
   instance->p_uploadMode = GUI->UploadMode_Combo.CurrentItem();
   instance->p_serverUploadDirectory = GUI->UploadDir_Edit.Text().Trimmed();
   instance->p_serverFileNameTemplate = GUI->ServerFileNameTemplate_Edit.Text().Trimmed();
   instance->p_frameType = GUI->CCDFrameType_Combo.CurrentItem();
   instance->p_binningX = GUI->CCDBinX_Combo.CurrentItem() + 1;
   instance->p_binningY = GUI->CCDBinY_Combo.CurrentItem() + 1;
   instance->p_exposureTime = GUI->ExposureTime_NumericEdit.Value();
   instance->p_exposureDelay = GUI->ExposureDelay_NumericEdit.Value();
   instance->p_exposureCount = GUI->ExposureCount_SpinBox.Value();
   //instance->p_newImageIdTemplate = ; // ### TODO
   instance->p_reuseImageWindow = GUI->ReuseImageWindow_CheckBox.IsChecked();
   instance->p_autoStretch = GUI->AutoStretch_CheckBox.IsChecked();
   instance->p_linkedAutoStretch = GUI->LinkedAutoStretch_CheckBox.IsChecked();
   return instance;
}

bool INDICCDFrameInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const INDICCDFrameInstance* r = dynamic_cast<const INDICCDFrameInstance*>( &p );
   if ( r == nullptr )
   {
      whyNot = "Not an INDICCDFrame instance.";
      return false;
   }
   whyNot.Clear();
   return true;
}

bool INDICCDFrameInterface::RequiresInstanceValidation() const
{
   return true;
}

bool INDICCDFrameInterface::ImportProcess( const ProcessImplementation& p )
{
   const INDICCDFrameInstance* r = dynamic_cast<const INDICCDFrameInstance*>( &p );
   if ( r != nullptr )
   {
      if ( r->ValidateDevice( false/*throwErrors*/ ) )
      {
         m_device = r->p_deviceName;
         r->SendDeviceProperties();
         GUI->ServerFileNameTemplate_Edit.SetText( r->p_serverFileNameTemplate );
         GUI->ExposureTime_NumericEdit.SetValue( r->p_exposureTime );
         GUI->ExposureDelay_NumericEdit.SetValue( r->p_exposureDelay );
         GUI->ExposureCount_SpinBox.SetValue( r->p_exposureCount );
         GUI->ReuseImageWindow_CheckBox.SetChecked( r->p_reuseImageWindow );
         GUI->AutoStretch_CheckBox.SetChecked( r->p_autoStretch );
         GUI->LinkedAutoStretch_CheckBox.SetChecked( r->p_linkedAutoStretch );
         return true;
      }
      else
         m_device.Clear();
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

INDICCDFrameInterface::GUIData::GUIData( INDICCDFrameInterface& w )
{
   int emWidth = w.Font().Width( 'm' );
   int labelWidth1 = w.Font().Width( "Server file name template:" ) + emWidth;
   int editWidth1 = 5*emWidth;
   int editWidth2 = 8*emWidth;

   //

   CCDParam_SectionBar.SetTitle( "CCD Device" );
   CCDParam_SectionBar.SetSection( CCDParam_Control );
   CCDParam_Control.SetSizer( CCDParam_Sizer );

   CCDDevice_Label.SetText( "INDI CCD device:" );
   CCDDevice_Label.SetToolTip( "<p>Select an INDI CCD device.</p>" );
   CCDDevice_Label.SetMinWidth( labelWidth1 );
   CCDDevice_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   CCDDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   CCDDevice_Sizer.SetSpacing( 4 );
   CCDDevice_Sizer.Add( CCDDevice_Label );
   CCDDevice_Sizer.Add( CCDDevice_Combo, 100 );

   CCDTemp_NumericEdit.SetReal();
   CCDTemp_NumericEdit.SetPrecision( 2 );
   CCDTemp_NumericEdit.SetRange( -50, +50 );
   CCDTemp_NumericEdit.label.SetText( "Temperature:" );
   CCDTemp_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CCDTemp_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CCDTemp_NumericEdit.sizer.AddStretch();
   CCDTemp_NumericEdit.SetToolTip( "<p>Current chip temperature in degrees Celsius.</p>" );
   CCDTemp_NumericEdit.Disable();

   CCDTargetTemp_NumericEdit.SetReal();
   CCDTargetTemp_NumericEdit.SetPrecision( 2 );
   CCDTargetTemp_NumericEdit.SetRange( -30, +25 );
   CCDTargetTemp_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CCDTargetTemp_NumericEdit.slider.SetScaledMinWidth( 200 );
   CCDTargetTemp_NumericEdit.slider.SetRange( 0, 110 );
   CCDTargetTemp_NumericEdit.SetToolTip( "<p>Target chip temperature in degrees Celsius.</p>" );
   CCDTargetTemp_NumericEdit.Disable();

   CCDTargetTemp_PushButton.SetText( "Set" );
   CCDTargetTemp_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );
   CCDTargetTemp_PushButton.Disable();

   CCDTemp_HSizer.SetSpacing( 4 );
   CCDTemp_HSizer.Add( CCDTemp_NumericEdit );
   CCDTemp_HSizer.Add( CCDTargetTemp_NumericEdit, 100 );
   CCDTemp_HSizer.AddSpacing( 4 );
   CCDTemp_HSizer.Add( CCDTargetTemp_PushButton );

   const char* ccdBinXToolTipText =
      "<p>Horizontal (X-axis) pixel binning factor.</p>";

   CCDBinX_Label.SetText( "Binning X:" );
   CCDBinX_Label.SetFixedWidth( labelWidth1 );
   CCDBinX_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDBinX_Label.SetToolTip( ccdBinXToolTipText );
   CCDBinX_Label.Disable();

   CCDBinX_Combo.AddItem( "1" );
   CCDBinX_Combo.AddItem( "2" );
   CCDBinX_Combo.AddItem( "3" );
   CCDBinX_Combo.AddItem( "4" );
   CCDBinX_Combo.AddItem( "5" );
   CCDBinX_Combo.AddItem( "6" );
   CCDBinX_Combo.AddItem( "7" );
   CCDBinX_Combo.AddItem( "8" );
   CCDBinX_Combo.SetFixedWidth( editWidth1 );
   CCDBinX_Combo.SetToolTip( ccdBinXToolTipText );
   CCDBinX_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDBinX_Combo.Disable();

   CCDBinX_HSizer.SetSpacing( 4 );
   CCDBinX_HSizer.Add( CCDBinX_Label );
   CCDBinX_HSizer.Add( CCDBinX_Combo );
   CCDBinX_HSizer.AddStretch();

   const char* ccdBinYToolTipText =
      "<p>Vertical (Y-axis) pixel binning factor.</p>";

   CCDBinY_Label.SetText( "Binning Y:" );
   CCDBinY_Label.SetFixedWidth( labelWidth1 );
   CCDBinY_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDBinY_Label.SetToolTip( ccdBinYToolTipText );
   CCDBinY_Label.Disable();

   CCDBinY_Combo.AddItem( "1" );
   CCDBinY_Combo.AddItem( "2" );
   CCDBinY_Combo.AddItem( "3" );
   CCDBinY_Combo.AddItem( "4" );
   CCDBinY_Combo.AddItem( "5" );
   CCDBinY_Combo.AddItem( "6" );
   CCDBinY_Combo.AddItem( "7" );
   CCDBinY_Combo.AddItem( "8" );
   CCDBinY_Combo.SetFixedWidth( editWidth1 );
   CCDBinY_Combo.SetToolTip( ccdBinYToolTipText );
   CCDBinY_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDBinY_Combo.Disable();

   CCDBinY_HSizer.SetSpacing( 4 );
   CCDBinY_HSizer.Add( CCDBinY_Label );
   CCDBinY_HSizer.Add( CCDBinY_Combo );
   CCDBinY_HSizer.AddStretch();

   const char* ccdFrameTypeToolTipText =
      "<p>The frame type will be stored as a standard FITS header keyword in each acquired frame.</p>";

   CCDFrameType_Label.SetText( "Frame type:" );
   CCDFrameType_Label.SetFixedWidth( labelWidth1 );
   CCDFrameType_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDFrameType_Label.SetToolTip( ccdFrameTypeToolTipText );
   CCDFrameType_Label.Disable();

   CCDFrameType_Combo.AddItem( "Light frame" );
   CCDFrameType_Combo.AddItem( "Bias frame" );
   CCDFrameType_Combo.AddItem( "Dark frame" );
   CCDFrameType_Combo.AddItem( "Flat frame" );
   CCDFrameType_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDFrameType_Combo.SetToolTip( ccdFrameTypeToolTipText );
   CCDFrameType_Combo.Disable();

   CCDFrameType_HSizer.SetSpacing( 4 );
   CCDFrameType_HSizer.Add( CCDFrameType_Label );
   CCDFrameType_HSizer.Add( CCDFrameType_Combo );
   CCDFrameType_HSizer.AddStretch();

   const char* uploadModeToolTipText =
      "<p>Upload to client: The frame will be uploaded to the client and displayed on a new image window. "
      "The frame will not be stored on the client's file system.</p>"
      "<p>Upload to server: The frame will be stored on the file system of the INDI server. "
      "The server upload directory and file name template can be specified. "
      "The frame will not be uploaded to the client.</p>"
      "<p>Upload both: The frame will be stored on the server's file system, then uploaded to the client and displayed.</p>";

   UploadMode_Label.SetText( "Upload mode:" );
   UploadMode_Label.SetToolTip( uploadModeToolTipText );
   UploadMode_Label.SetMinWidth( labelWidth1 );
   UploadMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadMode_Label.Disable();

   UploadMode_Combo.AddItem( "Upload to client only" );
   UploadMode_Combo.AddItem( "Upload to INDI server only" );
   UploadMode_Combo.AddItem( "Upload both: Client and server" );
   UploadMode_Combo.AdjustToContents();
   UploadMode_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   UploadMode_Combo.SetToolTip( uploadModeToolTipText );
   UploadMode_Combo.Disable();

   UploadMode_HSizer.SetSpacing( 4 );
   UploadMode_HSizer.Add( UploadMode_Label );
   UploadMode_HSizer.Add( UploadMode_Combo );
   UploadMode_HSizer.AddStretch();

   const char* uploadDirToolTipText =
      "<p>The directory where acquired CCD frames will be stored on the INDI server.</p>";

   UploadDir_Label.SetText( "Server upload directory:" );
   UploadDir_Label.SetToolTip( uploadDirToolTipText );
   UploadDir_Label.SetMinWidth( labelWidth1 );
   UploadDir_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadDir_Label.Disable();

   UploadDir_Edit.SetReadOnly();
   UploadDir_Edit.SetToolTip( uploadDirToolTipText );

   UploadDir_PushButton.SetText( "Set" );
   UploadDir_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );
   UploadDir_PushButton.Disable();

   UploadDir_HSizer.SetSpacing( 4 );
   UploadDir_HSizer.Add( UploadDir_Label );
   UploadDir_HSizer.Add( UploadDir_Edit, 100 );
   UploadDir_HSizer.AddSpacing( 4 );
   UploadDir_HSizer.Add( UploadDir_PushButton );

   const char* serverFileNameTemplateToolTipText =
      "<p>A template to build the file name prefixes of CCD frames stored on the INDI server.</p>"
      "<p>The template can be any valid text suitable to specify file names on the server's filesystem, and may include "
      "one or more <i>template specifiers</i>. Template specifiers are replaced automatically with selected tokens when "
      "CCD frames are acquired. Supported template specifiers are the following:</p>"
      "<p><table border=\"1\" cellspacing=\"1\" cellpadding=\"4\">"
      "<tr>"
         "<td><i>Template specifier</i></td>"
         "<td width=\"90%\"><i>Will be replaced by</i></td>"
      "</tr>"
      "<tr>"
         "<td>%f</td>"
         "<td>Frame type (light, flat, dark, bias).</td>"
      "</tr>"
      "<tr>"
         "<td>%b</td>"
         "<td>CCD binning with the format HxV, where H and V are, respectively, the horizontal and vertical binning factors.</td>"
      "</tr>"
      "<tr>"
         "<td>%e</td>"
         "<td>Exposure time in seconds.</td>"
      "</tr>"
      "<tr>"
         "<td>%F</td>"
         "<td>Filter name</td>"
      "</tr>"
      "<tr>"
         "<td>%T</td>"
         "<td>CCD temperature in degrees Celsius.</td>"
      "</tr>"
      "<tr>"
         "<td>%t</td>"
         "<td>Acquisition date and time in the UTC time scale, ISO 8601 format.</td>"
      "</tr>"
      "<tr>"
         "<td>%d</td>"
         "<td>Acquisition date in the UTC time scale, yyyy-mm-dd format.</td>"
      "</tr>"
      "<tr>"
         "<td>%n</td>"
         "<td>The frame number starting from one, with three digits and left-padded with zeros.</td>"
      "</tr>"
      "<tr>"
         "<td>%u</td>"
         "<td>A universally unique identifier (UUID) in canonical form (36 characters).</td>"
      "</tr>"
      "</table></p>"
      "<p>For example, the default template %f_B%b_E%e_%n would produce the following server file name:</p>"
      "<p>LIGHT_B2x2_E300.00_002.fits</p>"
      "<p>for the second light frame of a series with exposure time of 300 seconds at binning 2x2.</p>";

   ServerFileNameTemplate_Label.SetText( "Server file name template:" );
   ServerFileNameTemplate_Label.SetToolTip( serverFileNameTemplateToolTipText );
   ServerFileNameTemplate_Label.SetMinWidth( labelWidth1 );
   ServerFileNameTemplate_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ServerFileNameTemplate_Label.Disable();

   ServerFileNameTemplate_Edit.SetToolTip( serverFileNameTemplateToolTipText );
   ServerFileNameTemplate_Edit.SetText( TheICFServerFileNameTemplateParameter->DefaultValue() );
   ServerFileNameTemplate_Edit.Disable();

   ServerFileNameTemplate_HSizer.SetSpacing( 4 );
   ServerFileNameTemplate_HSizer.Add( ServerFileNameTemplate_Label );
   ServerFileNameTemplate_HSizer.Add( ServerFileNameTemplate_Edit, 100 );

   CCDProperties_Sizer.SetSpacing( 4 );
   CCDProperties_Sizer.Add( CCDTemp_HSizer );
   CCDProperties_Sizer.Add( CCDBinX_HSizer );
   CCDProperties_Sizer.Add( CCDBinY_HSizer );
   CCDProperties_Sizer.Add( CCDFrameType_HSizer );
   CCDProperties_Sizer.Add( UploadMode_HSizer );
   CCDProperties_Sizer.Add( UploadDir_HSizer );
   CCDProperties_Sizer.Add( ServerFileNameTemplate_HSizer );

   CCDProperties_Control.SetSizer( CCDProperties_Sizer );

   CCDParam_Sizer.SetSpacing( 4 );
   CCDParam_Sizer.Add( CCDDevice_Sizer );
   CCDParam_Sizer.Add( CCDProperties_Control );

   //

   FrameAcquisition_SectionBar.SetTitle( "Frame Acquisition" );
   FrameAcquisition_SectionBar.SetSection( FrameAcquisition_Control );

   ExposureTime_NumericEdit.SetReal();
   ExposureTime_NumericEdit.SetPrecision( 3 );
   ExposureTime_NumericEdit.SetRange( 0.001, 60000 ); // 1000 minutes = 16.666... hours
   ExposureTime_NumericEdit.label.SetText( "Exposure time:" );
   ExposureTime_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExposureTime_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ExposureTime_NumericEdit.sizer.AddStretch();
   ExposureTime_NumericEdit.SetValue( 1 );
   ExposureTime_NumericEdit.SetToolTip( "<p>Exposure time in seconds.</p>" );

   ExposureDelay_NumericEdit.SetReal();
   ExposureDelay_NumericEdit.SetPrecision( 3 );
   ExposureDelay_NumericEdit.SetRange( 0, 600 ); // 10 minutes
   ExposureDelay_NumericEdit.label.SetText( "Exposure delay:" );
   ExposureDelay_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExposureDelay_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ExposureDelay_NumericEdit.sizer.AddStretch();
   ExposureDelay_NumericEdit.SetValue( 0 );
   ExposureDelay_NumericEdit.SetToolTip( "<p>Waiting time between exposures in seconds.</p>" );

   ExposureCount_Label.SetText( "Number of frames:" );
   ExposureCount_Label.SetToolTip( "<p>Number of frames to be acquired.</p>" );
   ExposureCount_Label.SetMinWidth( labelWidth1 );
   ExposureCount_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ExposureCount_SpinBox.SetRange( 1, 1000 );
   ExposureCount_SpinBox.SetFixedWidth( editWidth2 );
   ExposureCount_SpinBox.SetValue( 1 );

   ExposureCount_Sizer.SetSpacing( 4 );
   ExposureCount_Sizer.Add( ExposureCount_Label );
   ExposureCount_Sizer.Add( ExposureCount_SpinBox );
   ExposureCount_Sizer.AddStretch();

   ReuseImageWindow_CheckBox.SetText( "Reuse image window" );
   ReuseImageWindow_CheckBox.SetToolTip( "<p>Load newly acquired client frames on the same image window, if available.</p>" );
   ReuseImageWindow_Sizer.AddSpacing( labelWidth1 + 4 );
   ReuseImageWindow_Sizer.Add( ReuseImageWindow_CheckBox );
   ReuseImageWindow_Sizer.AddStretch();

   AutoStretch_CheckBox.SetText( "AutoStretch" );
   AutoStretch_CheckBox.SetToolTip( "<p>Compute and apply adaptive screen transfer functions (STF) for newly acquired client frames.</p>" );
   AutoStretch_Sizer.AddSpacing( labelWidth1 + 4 );
   AutoStretch_Sizer.Add( AutoStretch_CheckBox );
   AutoStretch_Sizer.AddStretch();

   LinkedAutoStretch_CheckBox.SetText( "Linked AutoStretch" );
   LinkedAutoStretch_CheckBox.SetToolTip( "<p>If enabled, compute and apply a single adaptive STF for all nominal channels of "
      "each acquired color image.</p>"
      "<p>If disabled, compute a separate adaptive STF for each nominal color channel.</p>" );
   LinkedAutoStretch_Sizer.AddSpacing( labelWidth1 + 4 );
   LinkedAutoStretch_Sizer.Add( LinkedAutoStretch_CheckBox );
   LinkedAutoStretch_Sizer.AddStretch();

   FrameAcquisitionLeft_Sizer.SetSpacing( 4 );
   FrameAcquisitionLeft_Sizer.Add( ExposureTime_NumericEdit );
   FrameAcquisitionLeft_Sizer.Add( ExposureDelay_NumericEdit );
   FrameAcquisitionLeft_Sizer.Add( ExposureCount_Sizer );
   FrameAcquisitionLeft_Sizer.Add( ReuseImageWindow_Sizer );
   FrameAcquisitionLeft_Sizer.Add( AutoStretch_Sizer );
   FrameAcquisitionLeft_Sizer.Add( LinkedAutoStretch_Sizer );

   StartExposure_PushButton.SetText( "Start" );
   StartExposure_PushButton.SetIcon( w.ScaledResource( ":/icons/play.png" ) );
   StartExposure_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );

   StartExposure_Sizer.Add( StartExposure_PushButton );
   StartExposure_Sizer.AddStretch();

   CancelExposure_PushButton.SetText( "Cancel" );
   CancelExposure_PushButton.SetIcon( w.ScaledResource( ":/icons/stop.png" ) );
   CancelExposure_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );
   CancelExposure_PushButton.Disable();

   CancelExposure_Sizer.Add( CancelExposure_PushButton );
   CancelExposure_Sizer.AddStretch();

   FrameAcquisitionRight_Sizer.SetSpacing( 4 );
   FrameAcquisitionRight_Sizer.Add( StartExposure_Sizer );
   FrameAcquisitionRight_Sizer.Add( CancelExposure_Sizer );
   FrameAcquisitionRight_Sizer.Add( ExposureInfo_Label, 100 );

   FrameAcquisition_Sizer.SetSpacing( 16 );
   FrameAcquisition_Sizer.Add( FrameAcquisitionLeft_Sizer );
   FrameAcquisition_Sizer.Add( FrameAcquisitionRight_Sizer, 100 );
   FrameAcquisition_Sizer.AddStretch();

   FrameAcquisition_Control.SetSizer( FrameAcquisition_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( CCDParam_SectionBar );
   Global_Sizer.Add( CCDParam_Control );
   Global_Sizer.Add( FrameAcquisition_SectionBar );
   Global_Sizer.Add( FrameAcquisition_Control );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();

   w.OnShow( (Control::event_handler)&INDICCDFrameInterface::e_Show, w );
   w.OnHide( (Control::event_handler)&INDICCDFrameInterface::e_Hide, w );

   //

   UpdateDeviceList_Timer.SetInterval( 1 );
   UpdateDeviceList_Timer.SetPeriodic();
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDICCDFrameInterface::e_Timer, w );

   UpdateDeviceProperties_Timer.SetInterval( 1 );
   UpdateDeviceProperties_Timer.SetPeriodic();
   UpdateDeviceProperties_Timer.OnTimer( (Timer::timer_event_handler)&INDICCDFrameInterface::e_Timer, w );
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::UpdateControls()
{
   if ( m_device.IsEmpty() )
   {
      GUI->UpdateDeviceProperties_Timer.Stop();
      GUI->CCDProperties_Control.Disable();
      GUI->FrameAcquisition_Control.Disable();
   }
   else
   {
      if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
         GUI->UpdateDeviceProperties_Timer.Start();

      GUI->CCDProperties_Control.Enable();
      GUI->FrameAcquisition_Control.Enable();
   }
}

void INDICCDFrameInterface::e_Show( Control& )
{
   if ( GUI != nullptr )
   {
      e_Timer( GUI->UpdateDeviceList_Timer );
      GUI->UpdateDeviceList_Timer.Start();
   }
}

void INDICCDFrameInterface::e_Hide( Control& )
{
   if ( GUI != nullptr )
   {
      GUI->UpdateDeviceList_Timer.Stop();
      GUI->UpdateDeviceProperties_Timer.Stop();
   }
}

void INDICCDFrameInterface::e_Timer( Timer& sender )
{
   if ( sender == GUI->UpdateDeviceList_Timer )
   {
      GUI->CCDDevice_Combo.Clear();

      if ( INDIClient::HasClient() )
      {
         INDIClient* indi = INDIClient::TheClient();
         ExclConstDeviceList x = indi->ConstDeviceList();
         const INDIDeviceListItemArray& devices( x );
         if ( devices.IsEmpty() )
            GUI->CCDDevice_Combo.AddItem( "<No Device Available>" );
         else
         {
            GUI->CCDDevice_Combo.AddItem( "<No Device Selected>" );

            for ( auto device : devices )
            {
               INDIPropertyListItem item;
               if ( indi->HasPropertyItem( device.DeviceName, "CCD_FRAME", "WIDTH" ) ) // is this a camera device?
                  GUI->CCDDevice_Combo.AddItem( device.DeviceName );
            }

            int i = Max( 0, GUI->CCDDevice_Combo.FindItem( m_device ) );
            GUI->CCDDevice_Combo.SetCurrentItem( i );
            if ( i > 0 )
               goto __device_found;
         }
      }
      else
         GUI->CCDDevice_Combo.AddItem( "<INDI Server Not Connected>" );

      m_device.Clear();

__device_found:

      UpdateControls();
   }
   else if ( sender == GUI->UpdateDeviceProperties_Timer )
   {
      if ( !INDIClient::HasClient() )
         return;

      INDIClient* indi = INDIClient::TheClient();
      INDIPropertyListItem item;

      if ( indi->GetPropertyItem( m_device, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", item ) )
      {
         GUI->CCDTargetTemp_NumericEdit.Enable();
         GUI->CCDTargetTemp_NumericEdit.label.Enable();
         GUI->CCDTargetTemp_PushButton.Enable();
         GUI->CCDTemp_NumericEdit.SetValue( item.PropertyValue.ToDouble() );
      }

      if ( indi->GetPropertyItem( m_device, "CCD_BINNING", "HOR_BIN", item ) )
      {
         GUI->CCDBinX_Combo.Enable();
         GUI->CCDBinX_Label.Enable();
         GUI->CCDBinX_Combo.SetCurrentItem( item.PropertyValue.ToInt() - 1 );
      }

      if ( indi->GetPropertyItem( m_device, "CCD_BINNING", "VER_BIN", item ) )
      {
         GUI->CCDBinY_Combo.Enable();
         GUI->CCDBinY_Label.Enable();
         GUI->CCDBinY_Combo.SetCurrentItem( item.PropertyValue.ToInt() - 1 );
      }

      int uploadModeIndex = -1;
      if ( indi->GetPropertyItem( m_device, "UPLOAD_MODE", "UPLOAD_CLIENT", item ) )
      {
         if ( item.PropertyValue == "OFF" )
         {
            if ( indi->GetPropertyItem( m_device, "UPLOAD_MODE", "UPLOAD_LOCAL", item ) )
               if ( item.PropertyValue == "OFF" )
                  uploadModeIndex = ICFUploadMode::UploadServerAndClient;
               else
                  uploadModeIndex = ICFUploadMode::UploadServer;
         }
         else
            uploadModeIndex = ICFUploadMode::UploadClient;
      }

      if ( uploadModeIndex >= 0 )
      {
         GUI->UploadMode_Combo.Enable();
         GUI->UploadMode_Label.Enable();
         GUI->UploadMode_Combo.SetCurrentItem( uploadModeIndex );

         if ( m_execution == nullptr )
            if ( uploadModeIndex == ICFUploadMode::UploadClient || uploadModeIndex == ICFUploadMode::UploadServerAndClient )
            {
               GUI->ReuseImageWindow_CheckBox.Enable();
               GUI->AutoStretch_CheckBox.Enable();
               GUI->LinkedAutoStretch_CheckBox.Enable( GUI->AutoStretch_CheckBox.IsChecked() );
            }
            else
            {
               GUI->ReuseImageWindow_CheckBox.Disable();
               GUI->AutoStretch_CheckBox.Disable();
               GUI->LinkedAutoStretch_CheckBox.Disable();
            }

         if ( uploadModeIndex == ICFUploadMode::UploadServer || uploadModeIndex == ICFUploadMode::UploadServerAndClient )
         {
            GUI->UploadDir_Label.Enable();
            GUI->UploadDir_PushButton.Enable();
            GUI->ServerFileNameTemplate_Label.Enable();
            GUI->ServerFileNameTemplate_Edit.Enable();
         }
         else
         {
            GUI->UploadDir_Label.Disable();
            GUI->UploadDir_PushButton.Disable();
            GUI->ServerFileNameTemplate_Label.Disable();
            GUI->ServerFileNameTemplate_Edit.Disable();
         }
      }
      else
      {
         GUI->UploadMode_Combo.Disable();
         GUI->UploadMode_Label.Disable();
      }

      {
         static const char* indiFrameTypes[] = { "FRAME_LIGHT", "FRAME_BIAS", "FRAME_DARK", "FRAME_FLAT" };
         for ( size_type i = 0; i < ItemsInArray( indiFrameTypes ); ++i )
            if ( indi->GetPropertyItem( m_device, "CCD_FRAME_TYPE", indiFrameTypes[i], item ) )
               if ( item.PropertyValue == "ON" )
               {
                  GUI->CCDFrameType_Label.Enable();
                  GUI->CCDFrameType_Combo.SetCurrentItem( i );
                  GUI->CCDFrameType_Combo.Enable();
                  break;
               }
      }

      if ( indi->GetPropertyItem( m_device, "UPLOAD_SETTINGS", "UPLOAD_DIR", item ) )
         GUI->UploadDir_Edit.SetText( item.PropertyValue );
   }
}

void INDICCDFrameInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( !INDIClient::HasClient() )
      return;

   INDIClient* indi = INDIClient::TheClient();


   if ( sender == GUI->CCDDevice_Combo )
   {
      // The first item in the combo box list has been reserved for a "no device" selection.
      m_device = (itemIndex > 0) ? sender.ItemText( itemIndex ).Trimmed() : String();
      UpdateControls();

      // check for cooler connection (e.g. Atik cameras)
      if ( !m_device.IsEmpty() )
      {
         INDIPropertyListItem item;
         if ( indi->GetPropertyItem( m_device, "COOLER_CONNECTION", "CONNECT_COOLER", item ) )
            if ( item.PropertyValue == "OFF" )
            {
               INDINewPropertyItem newItem;
               newItem.Device = m_device;
               newItem.Property = "COOLER_CONNECTION";
               newItem.PropertyType = "INDI_SWITCH";
               newItem.ElementValue.Add(ElementValuePair("CONNECT_COOLER","ON"));
               indi->SendNewPropertyItem( newItem, true/*async*/ );
            }
      }
   }
   else if ( sender == GUI->CCDBinX_Combo )
   {
      if ( indi->HasPropertyItem( m_device, "CCD_BINNING", "HOR_BIN" ) )
      {
    	 INDINewPropertyItem newItem;
    	 newItem.Device = m_device;
         newItem.Property = "CCD_BINNING";
         newItem.PropertyType = "INDI_NUMBER";
         newItem.ElementValue.Add(ElementValuePair("HOR_BIN",GUI->CCDBinX_Combo.ItemText( itemIndex ).Trimmed()));
         indi->SendNewPropertyItem( newItem, true/*async*/ );
      }
   }
   else if ( sender == GUI->CCDBinY_Combo )
   {
      if ( indi->HasPropertyItem( m_device, "CCD_BINNING", "VER_BIN" ) )
      {
    	 INDINewPropertyItem newItem;
    	 newItem.Device = m_device;
         newItem.Property = "CCD_BINNING";
         newItem.PropertyType = "INDI_NUMBER";
         newItem.ElementValue.Add(ElementValuePair("VER_BIN",GUI->CCDBinY_Combo.ItemText( itemIndex ).Trimmed()));
         indi->SendNewPropertyItem( newItem, true/*async*/ );
      }
   }
   else if ( sender == GUI->UploadMode_Combo )
   {
      String PropertyElement = INDICCDFrameInstance::UploadModePropertyString( itemIndex );
      if ( indi->HasPropertyItem( m_device, "UPLOAD_MODE", PropertyElement ) )
      {
    	 INDINewPropertyItem newItem;
    	 newItem.Device = m_device;
         newItem.Property = "UPLOAD_MODE";
         newItem.PropertyType = "INDI_SWITCH";
         newItem.ElementValue.Add(ElementValuePair(PropertyElement,"ON"));
         indi->SendNewPropertyItem( newItem, true/*async*/ );
      }
   }
   else if ( sender == GUI->CCDFrameType_Combo )
   {
      String PropertyElement = INDICCDFrameInstance::CCDFrameTypePropertyString( itemIndex );
      if ( indi->HasPropertyItem( m_device, "CCD_FRAME_TYPE", PropertyElement ) )
      {
    	 INDINewPropertyItem newItem;
    	 newItem.Device = m_device;
         newItem.Property = "CCD_FRAME_TYPE";
         newItem.PropertyType = "INDI_SWITCH";
         newItem.ElementValue.Add(ElementValuePair(PropertyElement,"ON"));
         indi->SendNewPropertyItem( newItem, true/*async*/ );
      }
   }
}

class INDICCDFrameInterfaceExecution : public AbstractINDICCDFrameExecution
{
public:

   INDICCDFrameInterfaceExecution( INDICCDFrameInterface* iface ) :
      AbstractINDICCDFrameExecution( *dynamic_cast<INDICCDFrameInstance*>( iface->NewProcess() ) ),
      m_iface( iface ),
      m_instanceAuto( &m_instance ),
      m_abortRequested( false )
   {
   }

   virtual void Abort()
   {
      m_abortRequested = true;
   }

private:

   INDICCDFrameInterface*            m_iface;
   AutoPointer<INDICCDFrameInstance> m_instanceAuto;
   bool                              m_abortRequested;

   virtual void StartAcquisitionEvent()
   {
      m_iface->m_execution = this;
      m_iface->GUI->ExposureTime_NumericEdit.Disable();
      m_iface->GUI->ExposureDelay_NumericEdit.Disable();
      m_iface->GUI->ExposureCount_Label.Disable();
      m_iface->GUI->ExposureCount_SpinBox.Disable();
      m_iface->GUI->ReuseImageWindow_CheckBox.Disable();
      m_iface->GUI->AutoStretch_CheckBox.Disable();
      m_iface->GUI->LinkedAutoStretch_CheckBox.Disable();
      m_iface->GUI->StartExposure_PushButton.Disable();
      m_iface->GUI->CancelExposure_PushButton.Enable();
      m_iface->GUI->ExposureInfo_Label.Clear();
   }

   virtual void NewExposureEvent( int expNum, int expCount )
   {
      m_iface->ProcessEvents();
   }

   virtual void StartExposureDelayEvent( double totalDelayTime )
   {
      m_iface->GUI->ExposureInfo_Label.SetText( String().Format( "Waiting for %.3gs", totalDelayTime ) );
      m_iface->ProcessEvents();
   }

   virtual void ExposureDelayEvent( double delayTime )
   {
      if ( m_abortRequested )
         AbstractINDICCDFrameExecution::Abort();

      m_iface->ProcessEvents();
   }

   virtual void EndExposureDelayEvent()
   {
      m_iface->GUI->ExposureInfo_Label.Clear();
      m_iface->ProcessEvents();
   }

   virtual void StartExposureEvent( int expNum, int expCount, double expTime )
   {
      m_iface->ProcessEvents();
   }

   virtual void ExposureEvent( int expNum, int expCount, double expTime )
   {
      if ( m_abortRequested )
         AbstractINDICCDFrameExecution::Abort();

      m_iface->GUI->ExposureInfo_Label.SetText( String().Format( "Exposure %d of %d: %.3gs", expNum+1, expCount, expTime ) );
      m_iface->ProcessEvents();
   }

   virtual void ExposureErrorEvent( const String& errMsg )
   {
      m_iface->GUI->ExposureInfo_Label.SetText( "*** Error: " + errMsg + '.' );
      m_iface->ProcessEvents();
   }

   virtual void EndExposureEvent( int expNum )
   {
      m_iface->GUI->ExposureInfo_Label.Clear();
      m_iface->ProcessEvents();
   }

   virtual void WaitingForServerEvent()
   {
      if ( m_abortRequested )
         AbstractINDICCDFrameExecution::Abort();

      m_iface->GUI->ExposureInfo_Label.SetText( "Waiting for INDI server" );
      m_iface->ProcessEvents();
   }

   virtual void NewFrameEvent( ImageWindow& window, bool reusedWindow )
   {
      if ( reusedWindow )
         window.Regenerate();
      else
      {
         window.BringToFront();
         window.Show();
         window.ZoomToFit( false/*allowZoom*/ );
      }
      m_iface->ProcessEvents();
   }

   virtual void EndAcquisitionEvent()
   {
      m_iface->m_execution = nullptr;
      m_iface->GUI->ExposureTime_NumericEdit.Enable();
      m_iface->GUI->ExposureDelay_NumericEdit.Enable();
      m_iface->GUI->ExposureCount_Label.Enable();
      m_iface->GUI->ExposureCount_SpinBox.Enable();
      m_iface->GUI->ReuseImageWindow_CheckBox.Enable();
      m_iface->GUI->AutoStretch_CheckBox.Enable();
      m_iface->GUI->LinkedAutoStretch_CheckBox.Enable();
      m_iface->GUI->StartExposure_PushButton.Enable();
      m_iface->GUI->CancelExposure_PushButton.Disable();
      m_iface->GUI->ExposureInfo_Label.Clear();
      m_iface->ProcessEvents();
   }

   virtual void AbortEvent()
   {
      EndAcquisitionEvent();
      m_iface->GUI->ExposureInfo_Label.SetText( "Aborted" );
   }
};

void INDICCDFrameInterface::e_Click( Button& sender, bool checked )
{
   if ( !INDIClient::HasClient() )
      return;

   INDIClient* indi = INDIClient::TheClient();
   INDINewPropertyItem newItem;
   newItem.Device = m_device;

   if ( sender == GUI->CCDTargetTemp_PushButton )
   {
      newItem.Property = "CCD_TEMPERATURE";
      newItem.PropertyType = "INDI_NUMBER";
      newItem.ElementValue.Add(ElementValuePair("CCD_TEMPERATURE_VALUE",String( GUI->CCDTargetTemp_NumericEdit.Value() )));
      indi->SendNewPropertyItem( newItem, true/*async*/ );
   }
   else if ( sender == GUI->UploadDir_PushButton )
   {
      SimpleGetStringDialog dialog( "Server upload directory:", GUI->UploadDir_Edit.Text() );
      if ( dialog.Execute() )
      {
         newItem.Property = "UPLOAD_SETTINGS";
         newItem.PropertyType = "INDI_TEXT";
         newItem.ElementValue.Add(ElementValuePair("UPLOAD_DIR",dialog.Text()));
         indi->SendNewPropertyItem( newItem, true/*async*/ );
      }
   }
   else if ( sender == GUI->StartExposure_PushButton )
   {
      INDICCDFrameInterfaceExecution( this ).Perform();
   }
   else if ( sender == GUI->CancelExposure_PushButton )
   {
      if ( m_execution != nullptr )
         m_execution->Abort();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInterface.cpp - Released 2016/05/08 20:36:42 UTC
