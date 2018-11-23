//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0012
// ----------------------------------------------------------------------------
// SubframeSelectorExpressionsInterface.cpp - Released 2018-11-23T18:45:58Z
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

#include "SubframeSelectorExpressionsInterface.h"
#include "SubframeSelectorMeasurementsInterface.h"
#include "SubframeSelectorProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "SubframeSelectorIcon.xpm"

// ----------------------------------------------------------------------------

SubframeSelectorExpressionsInterface* TheSubframeSelectorExpressionsInterface = nullptr;

// ----------------------------------------------------------------------------

SubframeSelectorExpressionsInterface::SubframeSelectorExpressionsInterface( SubframeSelectorInstance& instance ) :
   m_instance( instance )
{
   TheSubframeSelectorExpressionsInterface = this;
}

// ----------------------------------------------------------------------------

SubframeSelectorExpressionsInterface::~SubframeSelectorExpressionsInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorExpressionsInterface::Id() const
{
   return "SubframeSelectorExpressions";
}

// ----------------------------------------------------------------------------

MetaProcess* SubframeSelectorExpressionsInterface::Process() const
{
   return TheSubframeSelectorProcess;
}

// ----------------------------------------------------------------------------

const char** SubframeSelectorExpressionsInterface::IconImageXPM() const
{
   return nullptr; // SubframeSelectorIcon_XPM; ---> put a nice icon here
}

// ----------------------------------------------------------------------------

InterfaceFeatures SubframeSelectorExpressionsInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorExpressionsInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorExpressionsInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorExpressionsInterface::Launch( const MetaProcess&,
                                                   const ProcessImplementation*,
                                                   bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "SubframeSelector | Expressions" );
      UpdateControls();
   }

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

void SubframeSelectorExpressionsInterface::ApplyApprovalExpression()
{
   try
   {
      m_instance.ApproveMeasurements();
      GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/enabled.png" ) ) );
   }
   catch ( ... )
   {
      GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
      Console().Show();
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorExpressionsInterface::ApplyWeightingExpression()
{
   try
   {
      m_instance.WeightMeasurements();
      GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/enabled.png" ) ) );
   }
   catch ( ... )
   {
      GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( ":/browser/disabled.png" ) ) );
      Console().Show();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorExpressionsInterface::UpdateControls()
{
   GUI->ExpressionParameters_Approval_Editor.SetText( m_instance.p_approvalExpression );
   GUI->ExpressionParameters_Weighting_Editor.SetText( m_instance.p_weightingExpression );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SubframeSelectorExpressionsInterface::e_TextUpdated( CodeEditor& sender )
{
   String text = sender.Text();
   String bmp = ":/browser/disabled.png";
   if ( MeasureUtils::IsValidExpression( text ) )
   {
      bmp = ":/browser/enabled.png";
   }

   if ( sender == GUI->ExpressionParameters_Approval_Editor )
   {
      if ( m_instance.p_approvalExpression != text )
         GUI->ExpressionParameters_Approval_Status.SetBitmap( Bitmap( ScaledResource( bmp ) ) );
   }
   else if ( sender == GUI->ExpressionParameters_Weighting_Editor )
   {
      if ( m_instance.p_weightingExpression != text )
         GUI->ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( ScaledResource( bmp ) ) );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorExpressionsInterface::e_KeyPressed( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   if ( key == KeyCode::Enter )
      if ( modifiers & KeyCode::Control )
      {
         if ( sender == GUI->ExpressionParameters_Approval_Editor )
            e_ButtonClick( GUI->ExpressionParameters_Approval_ToolButton, false/*checked*/ );
         else if ( sender == GUI->ExpressionParameters_Weighting_Editor )
            e_ButtonClick( GUI->ExpressionParameters_Weighting_ToolButton, false/*checked*/ );
         wantsKey = true;
      }
}

// ----------------------------------------------------------------------------

void SubframeSelectorExpressionsInterface::e_ButtonClick( Button& sender, bool/*checked*/ )
{
   if ( sender == GUI->ExpressionParameters_Approval_ToolButton )
   {
      String text = GUI->ExpressionParameters_Approval_Editor.Text();
      if ( m_instance.p_approvalExpression != text )
      {
         m_instance.p_approvalExpression = text;
         if ( MeasureUtils::IsValidExpression( text ) )
            TheSubframeSelectorMeasurementsInterface->UpdateControls();
      }
   }
   else if ( sender == GUI->ExpressionParameters_Weighting_ToolButton )
   {
      String text = GUI->ExpressionParameters_Weighting_Editor.Text();
      if ( m_instance.p_weightingExpression != text )
      {
         m_instance.p_weightingExpression = text;
         if ( MeasureUtils::IsValidExpression( text ) )
            TheSubframeSelectorMeasurementsInterface->UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SubframeSelectorExpressionsInterface::GUIData::GUIData( SubframeSelectorExpressionsInterface& w )
{
   int labelWidth1 = w.Font().Width( String( "Weighting:" ) + 'T' );

   ExpressionParameters_Approval_Status.SetBitmap( Bitmap( w.ScaledResource( ":/browser/enabled.png" ) ) );

   ExpressionParameters_Approval_Label.SetText( "Approval:" );
   ExpressionParameters_Approval_Label.SetMinWidth( labelWidth1 );
   ExpressionParameters_Approval_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   ExpressionParameters_Approval_Label_Sizer2.SetSpacing( 4 );
   ExpressionParameters_Approval_Label_Sizer2.Add( ExpressionParameters_Approval_Status );
   ExpressionParameters_Approval_Label_Sizer2.Add( ExpressionParameters_Approval_Label );

   ExpressionParameters_Approval_Label_Sizer1.Add( ExpressionParameters_Approval_Label_Sizer2 );
   ExpressionParameters_Approval_Label_Sizer1.AddStretch();

   ExpressionParameters_Approval_Editor.SetFilePath( "Approval.js" ); // enable syntax highlighting
   ExpressionParameters_Approval_Editor.SetScaledMinSize( 320, 96 );
   ExpressionParameters_Approval_Editor.SetToolTip( TheSSApprovalExpressionParameter->Tooltip() );
   ExpressionParameters_Approval_Editor.OnTextUpdated( (CodeEditor::editor_event_handler)
                                    &SubframeSelectorExpressionsInterface::e_TextUpdated, w );
   ExpressionParameters_Approval_Editor.OnKeyPress( (Control::keyboard_event_handler)
                                    &SubframeSelectorExpressionsInterface::e_KeyPressed, w );

   ExpressionParameters_Approval_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/play.png" ) ) );
   ExpressionParameters_Approval_ToolButton.SetScaledFixedSize( 20, 20 );
   ExpressionParameters_Approval_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExpressionParameters_Approval_ToolButton.SetToolTip( "Execute" );
   ExpressionParameters_Approval_ToolButton.OnClick( (ToolButton::click_event_handler)
                                    &SubframeSelectorExpressionsInterface::e_ButtonClick, w );

   ExpressionParameters_Approval_Sizer.SetSpacing( 4 );
   ExpressionParameters_Approval_Sizer.Add( ExpressionParameters_Approval_Label_Sizer1 );
   ExpressionParameters_Approval_Sizer.Add( ExpressionParameters_Approval_Editor, 100 );
   ExpressionParameters_Approval_Sizer.Add( ExpressionParameters_Approval_ToolButton );

   //

   ExpressionParameters_Weighting_Status.SetBitmap( Bitmap( w.ScaledResource( ":/browser/enabled.png" ) ) );

   ExpressionParameters_Weighting_Label.SetText( "Weighting:" );
   ExpressionParameters_Weighting_Label.SetMinWidth( labelWidth1 );
   ExpressionParameters_Weighting_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );

   ExpressionParameters_Weighting_Label_Sizer2.SetSpacing( 4 );
   ExpressionParameters_Weighting_Label_Sizer2.Add( ExpressionParameters_Weighting_Status );
   ExpressionParameters_Weighting_Label_Sizer2.Add( ExpressionParameters_Weighting_Label );

   ExpressionParameters_Weighting_Label_Sizer1.Add( ExpressionParameters_Weighting_Label_Sizer2 );
   ExpressionParameters_Weighting_Label_Sizer1.AddStretch();

   ExpressionParameters_Weighting_Editor.SetFilePath( "Weighting.js" ); // enable syntax highlighting
   ExpressionParameters_Weighting_Editor.SetScaledMinSize( 320, 96 );
   ExpressionParameters_Weighting_Editor.SetToolTip( TheSSWeightingExpressionParameter->Tooltip() );
   ExpressionParameters_Weighting_Editor.OnTextUpdated( (CodeEditor::editor_event_handler)
                                    &SubframeSelectorExpressionsInterface::e_TextUpdated, w );
   ExpressionParameters_Weighting_Editor.OnKeyPress( (Control::keyboard_event_handler)
                                    &SubframeSelectorExpressionsInterface::e_KeyPressed, w );

   ExpressionParameters_Weighting_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/play.png" ) ) );
   ExpressionParameters_Weighting_ToolButton.SetScaledFixedSize( 20, 20 );
   ExpressionParameters_Weighting_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExpressionParameters_Weighting_ToolButton.SetToolTip( "Execute" );
   ExpressionParameters_Weighting_ToolButton.OnClick( (ToolButton::click_event_handler)
                                    &SubframeSelectorExpressionsInterface::e_ButtonClick, w );

   ExpressionParameters_Weighting_Sizer.SetSpacing( 4 );
   ExpressionParameters_Weighting_Sizer.Add( ExpressionParameters_Weighting_Label_Sizer1 );
   ExpressionParameters_Weighting_Sizer.Add( ExpressionParameters_Weighting_Editor, 100 );
   ExpressionParameters_Weighting_Sizer.Add( ExpressionParameters_Weighting_ToolButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( ExpressionParameters_Approval_Sizer );
   Global_Sizer.Add( ExpressionParameters_Weighting_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorExpressionsInterface.cpp - Released 2018-11-23T18:45:58Z
