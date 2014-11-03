// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/SectionBar.cpp - Released 2014/10/29 07:34:20 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include <pcl/SectionBar.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ProcessInterface.h>

#define contract_icon   Bitmap( ":/process-interface/contract-vert.png" )
#define expand_icon     Bitmap( ":/process-interface/expand-vert.png" )

namespace pcl
{

// ----------------------------------------------------------------------------

SectionBar::SectionBar( Control& parent ) : Control( parent ),
section( 0 ), check( 0 ),
onToggleSection( 0 ), onToggleSectionReceiver( 0 ),
onCheck( 0 ), onCheckReceiver( 0 )
{
   SetObjectId( "IWSectionBar" );

   SetFocusStyle( FocusStyle::NoFocus );

   label.SetText( String( 'M' ) );
   label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   label.SetText( String() );

   button.SetIcon( contract_icon );
   button.SetFixedSize( 17, 17 );
   button.SetFocusStyle( FocusStyle::NoFocus );
   button.OnClick( (Button::click_event_handler)&SectionBar::__Click, *this );

   hSizer.AddSpacing( 4 );
   hSizer.Add( label );
   hSizer.AddStretch();
   hSizer.Add( button );
   hSizer.AddSpacing( 4 );

   vSizer.AddSpacing( 1 );
   vSizer.Add( hSizer );
   vSizer.AddSpacing( 1 );

   SetSizer( vSizer );
   AdjustToContents();
   SetFixedHeight();

   OnMousePress( (Control::mouse_button_event_handler)&SectionBar::__MousePress, *this );
   OnShow( (Control::event_handler)&SectionBar::__Show, *this );
}

// ----------------------------------------------------------------------------

SectionBar::~SectionBar()
{
   if ( section != 0 )
   {
      section->OnShow( 0, Control::Null() );
      section->OnHide( 0, Control::Null() );
      section = 0;

      if ( check != 0 )
      {
         check->OnClick( 0, Control::Null() );
         delete check;
         check = 0;
      }
   }
}

// ----------------------------------------------------------------------------

void SectionBar::SetSection( Control& c )
{
   if ( section != 0 )
   {
      section->OnShow( 0, Control::Null() );
      section->OnHide( 0, Control::Null() );
      section = 0;
   }

   if ( c.IsNull() )
      return;

   section = &c;

   if ( section->IsVisible() )
      button.SetIcon( contract_icon );
   else
      button.SetIcon( expand_icon );

   section->OnShow( (Control::event_handler)&SectionBar::__Show, *this );
   section->OnHide( (Control::event_handler)&SectionBar::__Hide, *this );
}

// ----------------------------------------------------------------------------

void SectionBar::EnableTitleCheckBox( bool enable )
{
   if ( (check == 0) == enable )
   {
      if ( check != 0 )
      {
         check->OnClick( 0, Control::Null() );
         onCheck = 0;
         onCheckReceiver = 0;
         delete check;
         check = 0;
      }
      else
      {
         check = new CheckBox;
         check->SetFocusStyle( FocusStyle::NoFocus );
         check->SetChecked();
         check->OnClick( (Button::click_event_handler)&SectionBar::__Click, *this );
         hSizer.Insert( 1, *check );
         hSizer.InsertSpacing( 2, 2 );
      }

      if ( section != 0 )
         section->Enable();
   }
}

// ----------------------------------------------------------------------------

void SectionBar::SetChecked( bool checked )
{
   if ( check != 0 )
   {
      if ( section != 0 )
         section->Enable( checked );
      check->SetChecked( checked );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::Enable( bool enabled )
{
   label.Enable( enabled );

   if ( check != 0 )
      check->Enable( enabled );

   if ( section != 0 )
      section->Enable( enabled && (check == 0 || check->IsChecked()) );
}

// ----------------------------------------------------------------------------

void SectionBar::OnToggleSection( section_event_handler f, Control& c )
{
   __PCL_NO_ALIAS_HANDLER;
   if ( f == 0 || c.IsNull() )
   {
      onToggleSection = 0;
      onToggleSectionReceiver = 0;
   }
   else
   {
      onToggleSection = f;
      onToggleSectionReceiver = &c;
   }
}

// ----------------------------------------------------------------------------

void SectionBar::OnCheck( check_event_handler f, Control& c )
{
   __PCL_NO_ALIAS_HANDLER;
   if ( check == 0 || f == 0 || c.IsNull() )
   {
      onCheck = 0;
      onCheckReceiver = 0;
   }
   else
   {
      onCheck = f;
      onCheckReceiver = &c;
   }
}

// ----------------------------------------------------------------------------

void SectionBar::__Click( Button& sender, bool checked )
{
   if ( section != 0 && sender == button )
   {
      if ( onToggleSection != 0 )
         (onToggleSectionReceiver->*onToggleSection)( *this, *section, true );

      bool visible = section->IsVisible();

      Control p = section->Parent();
      if ( !p.IsNull() )
      {
         while ( !p.Parent().IsNull() )
            p = p.Parent();

         p.DisableUpdates();
      }

      if ( visible )
         section->Hide();
      else
         section->Show();

      // On Mac OS X, forcing event processing here causes a lot of flickering.
      // On X11 and Windows, it's just the opposite...
#ifndef __PCL_MACOSX
      ProcessInterface::ProcessEvents();
#endif
      if ( !p.IsNull() )
      {
         bool fixedWidth = p.IsFixedWidth();
         if ( !fixedWidth )
            p.SetFixedWidth();

         bool fixedHeight = p.IsFixedHeight();
         if ( fixedHeight )
            p.SetVariableHeight();

         p.AdjustToContents();

//#ifndef __PCL_MACOSX
         ProcessInterface::ProcessEvents();
//#endif
         if ( !fixedWidth )
            p.SetVariableWidth();
         if ( fixedHeight )
            p.SetFixedHeight();

         p.EnableUpdates();
      }

      if ( onToggleSection != 0 )
         (onToggleSectionReceiver->*onToggleSection)( *this, *section, false );
   }
   else if ( check != 0 && sender == *check )
   {
      if ( section != 0 )
         section->Enable( checked );

      if ( onCheck != 0 )
         (onCheckReceiver->*onCheck)( *this, checked );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::__MousePress( Control& /*sender*/, const pcl::Point& /*pos*/,
               int mouseButton, unsigned /*buttons*/, unsigned /*modifiers*/ )
{
   if ( mouseButton == MouseButton::Left )
      __Click( button, false );
}

// ----------------------------------------------------------------------------

void SectionBar::__Show( Control& sender )
{
   if ( section != 0 )
   {
      if ( sender == *section )
         button.SetIcon( contract_icon );
      else if ( sender == *this )
      {
         if ( section->IsVisible() )
            button.SetIcon( contract_icon );
         else
            button.SetIcon( expand_icon );
      }
   }
}

// ----------------------------------------------------------------------------

void SectionBar::__Hide( Control& sender )
{
   if ( section != 0 && sender == *section )
      button.SetIcon( expand_icon );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/SectionBar.cpp - Released 2014/10/29 07:34:20 UTC
