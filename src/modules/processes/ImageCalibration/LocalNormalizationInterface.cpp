//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0300
// ----------------------------------------------------------------------------
// LocalNormalizationInterface.cpp - Released 2017-05-17T17:41:56Z
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

#include "LocalNormalizationInterface.h"
#include "LocalNormalizationProcess.h"
#include "LocalNormalizationParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LocalNormalizationInterface* TheLocalNormalizationInterface = nullptr;

// ----------------------------------------------------------------------------

// #include "LocalNormalizationIcon.xpm"

// ----------------------------------------------------------------------------

LocalNormalizationInterface::LocalNormalizationInterface() :
   m_instance( TheLocalNormalizationProcess )
{
   TheLocalNormalizationInterface = this;
}

// ----------------------------------------------------------------------------

LocalNormalizationInterface::~LocalNormalizationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString LocalNormalizationInterface::Id() const
{
   return "LocalNormalization";
}

// ----------------------------------------------------------------------------

MetaProcess* LocalNormalizationInterface::Process() const
{
   return TheLocalNormalizationProcess;
}

// ----------------------------------------------------------------------------

const char** LocalNormalizationInterface::IconImageXPM() const
{
   return nullptr; //LocalNormalizationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures LocalNormalizationInterface::Features() const
{
   return InterfaceFeature::Default;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::ResetInstance()
{
   LocalNormalizationInstance defaultInstance( TheLocalNormalizationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "LocalNormalization" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheLocalNormalizationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* LocalNormalizationInterface::NewProcess() const
{
   return new LocalNormalizationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const LocalNormalizationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a LocalNormalization instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool LocalNormalizationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::UpdateControls()
{
   GUI->ReferenceView_Edit.SetText( m_instance.p_referenceViewId );
   GUI->Scale_SpinBox.SetValue( m_instance.p_scale );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LocalNormalizationInterface::__EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text().Trimmed();
      if ( !id.IsEmpty() )
      {
         bool valid;
         size_type p = id.Find( "->" );
         if ( p == String::notFound )
            valid = id.IsValidIdentifier();
         else
            valid = id.Left( p ).IsValidIdentifier() && id.Substring( p+2 ).IsValidIdentifier();
         if ( !valid )
            throw Error( "Invalid identifier: " + id );
      }

      if ( sender == GUI->ReferenceView_Edit )
         sender.SetText( m_instance.p_referenceViewId = id );

      return;
   }
   catch ( ... )
   {
      if ( sender == GUI->ReferenceView_Edit )
         sender.SetText( m_instance.p_referenceViewId );

      try
      {
         throw;
      }
      ERROR_HANDLER

      sender.SelectAll();
      sender.Focus();
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::__SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Scale_SpinBox )
      m_instance.p_scale = value;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->ReferenceView_ToolButton )
   {
      ViewSelectionDialog d( m_instance.p_referenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
         GUI->ReferenceView_Edit.SetText( m_instance.p_referenceViewId = d.Id() );
   }
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ReferenceView_Edit )
      wantsView = true;
}

// ----------------------------------------------------------------------------

void LocalNormalizationInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ReferenceView_Edit )
      GUI->ReferenceView_Edit.SetText( m_instance.p_referenceViewId = view.FullId() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LocalNormalizationInterface::GUIData::GUIData( LocalNormalizationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Reference image:" ) + 'T' );
   int editWidth1 = fnt.Width( String( 'M', 32 ) );
   int editWidth2 = fnt.Width( String( '0', 12 ) );

   //

   const char* referenceViewToolTip = "<p>LocalNormalization computes local image normalization functions to match the "
   "background and signal levels of its target image to the corresponding values of the reference image. "
   "The reference image must be a main view or a complete preview (partial previews are not allowed as reference or "
   "target images).</p>";

   ReferenceView_Label.SetText( "Reference image:" );
   ReferenceView_Label.SetFixedWidth( labelWidth1 );
   ReferenceView_Label.SetToolTip( referenceViewToolTip );
   ReferenceView_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ReferenceView_Edit.SetMinWidth( editWidth1 );
   ReferenceView_Edit.SetToolTip( referenceViewToolTip );
   ReferenceView_Edit.OnEditCompleted( (Edit::edit_event_handler)&LocalNormalizationInterface::__EditCompleted, w );
   ReferenceView_Edit.OnViewDrag( (Control::view_drag_event_handler)&LocalNormalizationInterface::__ViewDrag, w );
   ReferenceView_Edit.OnViewDrop( (Control::view_drop_event_handler)&LocalNormalizationInterface::__ViewDrop, w );

   ReferenceView_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   ReferenceView_ToolButton.SetScaledFixedSize( 20, 20 );
   ReferenceView_ToolButton.SetToolTip( "<p>Select the reference image.</p>" );
   ReferenceView_ToolButton.OnClick( (Button::click_event_handler)&LocalNormalizationInterface::__Click, w );

   ReferenceView_Sizer.SetSpacing( 4 );
   ReferenceView_Sizer.Add( ReferenceView_Label );
   ReferenceView_Sizer.Add( ReferenceView_Edit, 100 );
   ReferenceView_Sizer.Add( ReferenceView_ToolButton );

   //

   const char* scaleTip =
      "<p>Size in pixels of the sampling region for local image normalization. The larger this parameter, the less locally "
      "adaptive will be the frame adaptation task. Smaller values tend to reproduce variations among small-scale structures "
      "in the reference image. Larger values tend to reproduce variations among large-scale structures.</p>";

   Scale_Label.SetText( "Scale:" );
   Scale_Label.SetFixedWidth( labelWidth1 );
   Scale_Label.SetToolTip( scaleTip );
   Scale_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Scale_SpinBox.SetRange( int( TheLNScaleParameter->MinimumValue() ), int( TheLNScaleParameter->MaximumValue() ) );
   Scale_SpinBox.SetMinWidth( editWidth2 );
   Scale_SpinBox.SetToolTip( scaleTip );
   Scale_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&LocalNormalizationInterface::__SpinValueUpdated, w );

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( Scale_Label );
   Scale_Sizer.Add( Scale_SpinBox );
   Scale_Sizer.AddStretch();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );

   Global_Sizer.Add( ReferenceView_Sizer );
   Global_Sizer.Add( Scale_Sizer );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInterface.cpp - Released 2017-05-17T17:41:56Z
