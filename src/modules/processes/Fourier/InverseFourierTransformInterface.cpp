//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 01.00.04.0260
// ----------------------------------------------------------------------------
// InverseFourierTransformInterface.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "InverseFourierTransformInterface.h"
#include "InverseFourierTransformProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

InverseFourierTransformInterface* TheInverseFourierTransformInterface = nullptr;

// ----------------------------------------------------------------------------

#include "InverseFourierTransformIcon.xpm"

// ----------------------------------------------------------------------------

InverseFourierTransformInterface::InverseFourierTransformInterface() :
   instance( TheInverseFourierTransformProcess )
{
   TheInverseFourierTransformInterface = this;
}

InverseFourierTransformInterface::~InverseFourierTransformInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString InverseFourierTransformInterface::Id() const
{
   return "InverseFourierTransform";
}

MetaProcess* InverseFourierTransformInterface::Process() const
{
   return TheInverseFourierTransformProcess;
}

const char** InverseFourierTransformInterface::IconImageXPM() const
{
   return InverseFourierTransformIcon_XPM;
}

InterfaceFeatures InverseFourierTransformInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void InverseFourierTransformInterface::ApplyInstance() const
{
   instance.LaunchGlobal();
}

void InverseFourierTransformInterface::ResetInstance()
{
   InverseFourierTransformInstance defaultInstance( TheInverseFourierTransformProcess );
   ImportProcess( defaultInstance );
}

bool InverseFourierTransformInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "InverseFourierTransform" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheInverseFourierTransformProcess;
}

ProcessImplementation* InverseFourierTransformInterface::NewProcess() const
{
   return new InverseFourierTransformInstance( instance );
}

bool InverseFourierTransformInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const InverseFourierTransformInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an InverseFourierTransform instance.";
   return false;
}

bool InverseFourierTransformInterface::RequiresInstanceValidation() const
{
   return true;
}

bool InverseFourierTransformInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InverseFourierTransformInterface::UpdateControls()
{
   GUI->FirstComponent_Edit.SetText( instance.idOfFirstComponent );
   GUI->SecondComponent_Edit.SetText( instance.idOfSecondComponent );
   GUI->OnOutOfRangeResult_ComboBox.SetCurrentItem( instance.onOutOfRangeResult );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InverseFourierTransformInterface::__Click( Button& sender, bool checked )
{
   bool firstComponent = sender == GUI->FirstComponent_ToolButton;
   String* id = firstComponent ? &instance.idOfFirstComponent : &instance.idOfSecondComponent;
   ViewSelectionDialog d( *id, false/*allowPreviews*/ );
   d.SetWindowTitle( "Select " + String( firstComponent ? "First" : "Second" ) + " DFT Component" );
   if ( d.Execute() == StdDialogCode::Ok )
      *id = d.Id();

   UpdateControls();
}

void InverseFourierTransformInterface::__EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text().Trimmed();
      if ( !id.IsEmpty() && !id.IsValidIdentifier() )
         throw Error( "Invalid image identifier: " + id );

      if ( sender == GUI->FirstComponent_Edit )
        instance.idOfFirstComponent = id;
      else if ( sender == GUI->SecondComponent_Edit )
        instance.idOfSecondComponent = id;
   }
   ERROR_HANDLER

   UpdateControls();
}

void InverseFourierTransformInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->OnOutOfRangeResult_ComboBox )
      instance.onOutOfRangeResult = itemIndex;
}

void InverseFourierTransformInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->FirstComponent_Edit || sender == GUI->SecondComponent_Edit )
      wantsView = view.IsMainView();
}

void InverseFourierTransformInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( view.IsMainView() )
   {
      if ( sender == GUI->FirstComponent_Edit )
         GUI->FirstComponent_Edit.SetText( instance.idOfFirstComponent = view.Id() );
      else if ( sender == GUI->SecondComponent_Edit )
         GUI->SecondComponent_Edit.SetText( instance.idOfSecondComponent = view.Id() );

      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

InverseFourierTransformInterface::GUIData::GUIData( InverseFourierTransformInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "On out-of-range result:" ) + 'M' );

   const char* firstComponentToolTip = "<p>First component of the discrete Fourier transform.</p>"
      "<p>The inverse transform can be computed either from complex (real and imaginary) components, "
      "or from polar (magnitude and phase) components.</p>";

   FirstComponent_Label.SetText( "First DFT component:" );
   FirstComponent_Label.SetFixedWidth( labelWidth1 );
   FirstComponent_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FirstComponent_Label.SetToolTip( firstComponentToolTip );

   FirstComponent_Edit.SetScaledMinWidth( 400 );
   FirstComponent_Edit.SetToolTip( firstComponentToolTip );
   FirstComponent_Edit.OnEditCompleted( (Edit::edit_event_handler)&InverseFourierTransformInterface::__EditCompleted, w );
   FirstComponent_Edit.OnViewDrag( (Control::view_drag_event_handler)&InverseFourierTransformInterface::__ViewDrag, w );
   FirstComponent_Edit.OnViewDrop( (Control::view_drop_event_handler)&InverseFourierTransformInterface::__ViewDrop, w );

   FirstComponent_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   FirstComponent_ToolButton.SetScaledFixedSize( 20, 20 );
   FirstComponent_ToolButton.SetToolTip( "<p>Select the first DFT component</p>" );
   FirstComponent_ToolButton.OnClick( (Button::click_event_handler)&InverseFourierTransformInterface::__Click, w );

   FirstComponent_Sizer.SetSpacing( 4 );
   FirstComponent_Sizer.Add( FirstComponent_Label );
   FirstComponent_Sizer.Add( FirstComponent_Edit, 100 );
   FirstComponent_Sizer.Add( FirstComponent_ToolButton );

   const char* secondComponentToolTip = "<p>Second component of the discrete Fourier transform.</p>"
      "<p>The inverse transform can be computed either from complex (real and imaginary) components, "
      "or from polar (magnitude and phase) components.</p>";

   SecondComponent_Label.SetText( "Second DFT component:" );
   SecondComponent_Label.SetFixedWidth( labelWidth1 );
   SecondComponent_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SecondComponent_Label.SetToolTip( secondComponentToolTip );

   SecondComponent_Edit.SetScaledMinWidth( 400 );
   SecondComponent_Edit.SetToolTip( secondComponentToolTip );
   SecondComponent_Edit.OnEditCompleted( (Edit::edit_event_handler)&InverseFourierTransformInterface::__EditCompleted, w );
   SecondComponent_Edit.OnViewDrag( (Control::view_drag_event_handler)&InverseFourierTransformInterface::__ViewDrag, w );
   SecondComponent_Edit.OnViewDrop( (Control::view_drop_event_handler)&InverseFourierTransformInterface::__ViewDrop, w );

   SecondComponent_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   SecondComponent_ToolButton.SetScaledFixedSize( 20, 20 );
   SecondComponent_ToolButton.SetToolTip( "<p>Select the second DFT component</p>" );
   SecondComponent_ToolButton.OnClick( (Button::click_event_handler)&InverseFourierTransformInterface::__Click, w );

   SecondComponent_Sizer.SetSpacing( 4 );
   SecondComponent_Sizer.Add( SecondComponent_Label );
   SecondComponent_Sizer.Add( SecondComponent_Edit, 100 );
   SecondComponent_Sizer.Add( SecondComponent_ToolButton );

   const char* onOutOfRangeResultToolTip = "<p>Specify how to handle out-of-range results after "
      "the inverse Fourier transform.</p>"
      "<p>When the DFT components are modified (for example, to implement a filtering procedure "
      "in the frequency domain), sample values outside the normalized [0,1] range can be generated "
      "in the resulting image after the inverse Fourier transform. You can handle these out-of-range "
      "values in three ways:</p>"
      "<p><b>Don't care</b> will leave out-of-range values unmodified. In this mode, the resulting "
      "image may contain invalid pixel values. Do not use this mode unless you know what you are doing.</p>"
      "<p><b>Truncate</b> will truncate any out-of-range values to the [0,1] range. In this mode the "
      "original dynamics of the image are preserved. This is the default working mode.</p>"
      "<p><b>Rescale</b> will rescale the resulting image to the [0,1] range only if there are out-of-range "
      "values. In this mode all the output data are preserved, but the dynamics of the original image "
      "may change after the inverse Fourier transform.</p>";

   OnOutOfRangeResult_Label.SetText( "On out-of-range result:" );
   OnOutOfRangeResult_Label.SetFixedWidth( labelWidth1 );
   OnOutOfRangeResult_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OnOutOfRangeResult_Label.SetToolTip( onOutOfRangeResultToolTip );

   OnOutOfRangeResult_ComboBox.AddItem( "Don't care" );
   OnOutOfRangeResult_ComboBox.AddItem( "Truncate" );
   OnOutOfRangeResult_ComboBox.AddItem( "Rescale" );
   OnOutOfRangeResult_ComboBox.SetToolTip( onOutOfRangeResultToolTip );
   OnOutOfRangeResult_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&InverseFourierTransformInterface::__ItemSelected, w );

   OnOutOfRangeResult_Sizer.SetSpacing( 4 );
   OnOutOfRangeResult_Sizer.Add( OnOutOfRangeResult_Label );
   OnOutOfRangeResult_Sizer.Add( OnOutOfRangeResult_ComboBox );
   OnOutOfRangeResult_Sizer.AddStretch();

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( FirstComponent_Sizer );
   Global_Sizer.Add( SecondComponent_Sizer );
   Global_Sizer.Add( OnOutOfRangeResult_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformInterface.cpp - Released 2018-12-12T09:25:24Z
