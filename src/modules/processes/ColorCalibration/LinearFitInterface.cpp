//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0319
// ----------------------------------------------------------------------------
// LinearFitInterface.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "LinearFitInterface.h"
#include "LinearFitProcess.h"
#include "LinearFitParameters.h"

#include <pcl/ErrorHandler.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LinearFitInterface* TheLinearFitInterface = nullptr;

// ----------------------------------------------------------------------------

#include "LinearFitIcon.xpm"

// ----------------------------------------------------------------------------

LinearFitInterface::LinearFitInterface() :
   m_instance( TheLinearFitProcess )
{
   TheLinearFitInterface = this;
}

// ----------------------------------------------------------------------------

LinearFitInterface::~LinearFitInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString LinearFitInterface::Id() const
{
   return "LinearFit";
}

// ----------------------------------------------------------------------------

MetaProcess* LinearFitInterface::Process() const
{
   return TheLinearFitProcess;
}

// ----------------------------------------------------------------------------

const char** LinearFitInterface::IconImageXPM() const
{
   return LinearFitIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures LinearFitInterface::Features() const
{
   return InterfaceFeature::Default;
}

// ----------------------------------------------------------------------------

void LinearFitInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void LinearFitInterface::ResetInstance()
{
   LinearFitInstance defaultInstance( TheLinearFitProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool LinearFitInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "LinearFit" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheLinearFitProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* LinearFitInterface::NewProcess() const
{
   return new LinearFitInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool LinearFitInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const LinearFitInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a LinearFit instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool LinearFitInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool LinearFitInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void LinearFitInterface::UpdateControls()
{
   GUI->ReferenceView_Edit.SetText( m_instance.p_referenceViewId );
   GUI->RejectLow_NumericControl.SetValue( m_instance.p_rejectLow );
   GUI->RejectHigh_NumericControl.SetValue( m_instance.p_rejectHigh );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LinearFitInterface::__EditCompleted( Edit& sender )
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

void LinearFitInterface::__EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RejectLow_NumericControl )
   {
      m_instance.p_rejectLow = value;
      if ( m_instance.p_rejectLow >= m_instance.p_rejectHigh )
      {
         m_instance.p_rejectHigh = m_instance.p_rejectLow + 0.05;
         if ( m_instance.p_rejectHigh > 1 )
         {
            m_instance.p_rejectLow = 0.95;
            m_instance.p_rejectHigh = 1;
         }
         UpdateControls();
      }
   }
   else if ( sender == GUI->RejectHigh_NumericControl )
   {
      m_instance.p_rejectHigh = value;
      if ( m_instance.p_rejectHigh <= m_instance.p_rejectLow )
      {
         m_instance.p_rejectLow = m_instance.p_rejectHigh - 0.05;
         if ( m_instance.p_rejectLow < 0 )
         {
            m_instance.p_rejectLow = 0;
            m_instance.p_rejectHigh = 0.05;
         }
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------

void LinearFitInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->ReferenceView_ToolButton )
   {
      ViewSelectionDialog d( m_instance.p_referenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
         GUI->ReferenceView_Edit.SetText( m_instance.p_referenceViewId = d.Id() );
   }
}

// ----------------------------------------------------------------------------

void LinearFitInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ReferenceView_Edit )
      wantsView = true;
}

// ----------------------------------------------------------------------------

void LinearFitInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ReferenceView_Edit )
      GUI->ReferenceView_Edit.SetText( m_instance.p_referenceViewId = view.FullId() );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LinearFitInterface::GUIData::GUIData( LinearFitInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Reference image:" ) + 'T' );
   int editWidth1 = fnt.Width( String( 'M', 32 ) );

   //

   const char* referenceViewToolTip = "<p>LinearFit computes linear fitting functions to match the mean "
   "background and signal levels of its target image to the corresponding values of the reference image. The "
   "reference image must be a main view or a complete preview (partial previews are not allowed as reference "
   "images).</p>";

   ReferenceView_Label.SetText( "Reference image:" );
   ReferenceView_Label.SetFixedWidth( labelWidth1 );
   ReferenceView_Label.SetToolTip( referenceViewToolTip );
   ReferenceView_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ReferenceView_Edit.SetMinWidth( editWidth1 );
   ReferenceView_Edit.SetToolTip( referenceViewToolTip );
   ReferenceView_Edit.OnEditCompleted( (Edit::edit_event_handler)&LinearFitInterface::__EditCompleted, w );
   ReferenceView_Edit.OnViewDrag( (Control::view_drag_event_handler)&LinearFitInterface::__ViewDrag, w );
   ReferenceView_Edit.OnViewDrop( (Control::view_drop_event_handler)&LinearFitInterface::__ViewDrop, w );

   ReferenceView_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   ReferenceView_ToolButton.SetScaledFixedSize( 20, 20 );
   ReferenceView_ToolButton.SetToolTip( "<p>Select the reference image.</p>" );
   ReferenceView_ToolButton.OnClick( (Button::click_event_handler)&LinearFitInterface::__Click, w );

   ReferenceView_Sizer.SetSpacing( 4 );
   ReferenceView_Sizer.Add( ReferenceView_Label );
   ReferenceView_Sizer.Add( ReferenceView_Edit, 100 );
   ReferenceView_Sizer.Add( ReferenceView_ToolButton );

   //

   RejectLow_NumericControl.label.SetText( "Reject low:" );
   RejectLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   RejectLow_NumericControl.slider.SetRange( 0, 100 );
   RejectLow_NumericControl.slider.SetScaledMinWidth( 200 );
   RejectLow_NumericControl.SetReal();
   RejectLow_NumericControl.SetRange( TheLFRejectLowParameter->MinimumValue(), TheLFRejectLowParameter->MaximumValue() );
   RejectLow_NumericControl.SetPrecision( TheLFRejectLowParameter->Precision() );
   RejectLow_NumericControl.SetToolTip( "<p>Low rejection limit. LinearFit will ignore all reference and target "
      "pixels whose values are less than or equal to this limit. Note that according to that definition, black "
      "pixels (zero pixel values) are always rejected.</p>" );
   RejectLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LinearFitInterface::__EditValueUpdated, w );

   //

   RejectHigh_NumericControl.label.SetText( "Reject high:" );
   RejectHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   RejectHigh_NumericControl.slider.SetRange( 0, 100 );
   RejectHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   RejectHigh_NumericControl.SetReal();
   RejectHigh_NumericControl.SetRange( TheLFRejectHighParameter->MinimumValue(), TheLFRejectHighParameter->MaximumValue() );
   RejectHigh_NumericControl.SetPrecision( TheLFRejectHighParameter->Precision() );
   RejectHigh_NumericControl.SetToolTip( "<p>High rejection limit. LinearFit will ignore all reference and target "
      "pixels whose values are greater than or equal to this limit. Note that according to that definition, white "
      "pixels (pixel values equal to one in the normalized [0,1] range) are always rejected.</p>");
   RejectHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LinearFitInterface::__EditValueUpdated, w );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );

   Global_Sizer.Add( ReferenceView_Sizer );
   Global_Sizer.Add( RejectLow_NumericControl );
   Global_Sizer.Add( RejectHigh_NumericControl );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LinearFitInterface.cpp - Released 2018-11-01T11:07:20Z
