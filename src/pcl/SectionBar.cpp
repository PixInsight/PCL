//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/SectionBar.cpp - Released 2017-06-17T10:55:56Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h> // MetaModule::ProcessEvents()
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>

#define contract_icon ":/process-interface/contract-vert.png"
#define expand_icon   ":/process-interface/expand-vert.png"

namespace pcl
{

// ----------------------------------------------------------------------------

SectionBar::SectionBar( Control& parent ) :
   Control( parent )
{
   SetObjectId( "IWSectionBar" );

   SetFocusStyle( FocusStyle::NoFocus );

   SetSizer( Global_Sizer );

   Global_Sizer.AddSpacing( 1 );
   Global_Sizer.Add( Title_Sizer );
   Global_Sizer.AddSpacing( 1 );

   Title_Sizer.AddSpacing( 4 );
   Title_Sizer.Add( Title_Label );
   Title_Sizer.AddStretch();
   Title_Sizer.Add( Title_ToolButton );
   Title_Sizer.AddSpacing( 4 );

   Title_Label.SetText( String( 'M' ) );
   Title_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   Title_Label.SetText( String() );

   Title_ToolButton.SetIcon( Bitmap( ScaledResource( contract_icon ) ) );
   Title_ToolButton.SetScaledFixedSize( 17, 17 );
   Title_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Title_ToolButton.OnClick( (Button::click_event_handler)&SectionBar::ButtonClick, *this );

   AdjustToContents();
   SetFixedHeight();

   OnMousePress( (Control::mouse_button_event_handler)&SectionBar::MousePress, *this );
   OnShow( (Control::event_handler)&SectionBar::ControlShow, *this );
}

// ----------------------------------------------------------------------------

SectionBar::~SectionBar()
{
   if ( m_section != nullptr )
   {
      m_section->OnShow( nullptr, Control::Null() );
      m_section->OnHide( nullptr, Control::Null() );
      m_section = nullptr;
   }
}

// ----------------------------------------------------------------------------

void SectionBar::SetSection( Control& section )
{
   if ( m_section != nullptr )
   {
      m_section->OnShow( nullptr, Control::Null() );
      m_section->OnHide( nullptr, Control::Null() );
      m_section = nullptr;
   }

   if ( section.IsNull() )
      return;

   m_section = &section;
   m_section->OnShow( (Control::event_handler)&SectionBar::ControlShow, *this );
   m_section->OnHide( (Control::event_handler)&SectionBar::ControlHide, *this );

   Title_ToolButton.SetIcon( Bitmap( ScaledResource( m_section->IsVisible() ? contract_icon : expand_icon ) ) );
}

// ----------------------------------------------------------------------------

void SectionBar::EnableTitleCheckBox( bool enable )
{
   if ( Title_CheckBox.IsNull() == enable )
   {
      if ( Title_CheckBox.IsNull() )
      {
         Title_CheckBox = new CheckBox;
         Title_CheckBox->SetFocusStyle( FocusStyle::NoFocus );
         Title_CheckBox->SetChecked();
         Title_CheckBox->OnClick( (Button::click_event_handler)&SectionBar::ButtonClick, *this );
         Title_Sizer.Insert( 1, *Title_CheckBox );
         Title_Sizer.InsertSpacing( 2, 2 );
      }
      else
      {
         if ( !m_handlers.IsNull() )
         {
            m_handlers->onCheck = nullptr;
            m_handlers->onCheckReceiver = nullptr;
         }
         Title_CheckBox.Destroy();
      }

      if ( m_section != nullptr )
         m_section->Enable();
   }
}

// ----------------------------------------------------------------------------

void SectionBar::SetChecked( bool checked )
{
   if ( !Title_CheckBox.IsNull() )
   {
      if ( m_section != nullptr )
         m_section->Enable( checked );
      Title_CheckBox->SetChecked( checked );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::Enable( bool enabled )
{
   Title_Label.Enable( enabled );

   if ( !Title_CheckBox.IsNull() )
      Title_CheckBox->Enable( enabled );

   if ( m_section != nullptr )
      m_section->Enable( enabled && (Title_CheckBox.IsNull() || Title_CheckBox->IsChecked()) );
}

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void SectionBar::OnToggleSection( section_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onToggleSection = nullptr;
         m_handlers->onToggleSectionReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onToggleSection = f;
      m_handlers->onToggleSectionReceiver = &c;
   }
}

void SectionBar::OnCheck( check_event_handler f, Control& c )
{
   if ( Title_CheckBox.IsNull() || f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onCheck = nullptr;
         m_handlers->onCheckReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onCheck = f;
      m_handlers->onCheckReceiver = &c;
   }
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

void SectionBar::SetSectionVisible( bool visible )
{
   if ( m_section != nullptr )
      if ( visible != m_section->IsVisible() )
      {
         Control* p = &m_section->Parent();
         if ( !p->IsNull() )
         {
            while ( !p->Parent().IsNull() )
               p = &p->Parent();
            p->DisableUpdates();
         }

         m_section->SetVisible( visible );

         /*
          * On macOS, forcing event processing here causes a lot of flickering.
          * On X11 and Windows, it's just the opposite... go figure!
          */
#ifndef __PCL_MACOSX
         Module->ProcessEvents();
#endif
         if ( !p->IsNull() )
         {
            bool fixedWidth = p->IsFixedWidth();
            if ( !fixedWidth )
               p->SetFixedWidth();

            bool fixedHeight = p->IsFixedHeight();
            if ( fixedHeight )
               p->SetVariableHeight();

            p->AdjustToContents();

            Module->ProcessEvents();

            if ( !fixedWidth )
               p->SetVariableWidth();
            if ( fixedHeight )
               p->SetFixedHeight();

            p->EnableUpdates();
         }
      }
}

// ----------------------------------------------------------------------------

void SectionBar::ButtonClick( Button& sender, bool checked )
{
   if ( m_section != nullptr && sender == Title_ToolButton )
   {
      if ( !m_handlers.IsNull() )
         if ( m_handlers->onToggleSection != nullptr )
            (m_handlers->onToggleSectionReceiver->*m_handlers->onToggleSection)( *this, *m_section, true );

      SetSectionVisible( !m_section->IsVisible() );

      if ( !m_handlers.IsNull() )
         if ( m_handlers->onToggleSection != nullptr )
            (m_handlers->onToggleSectionReceiver->*m_handlers->onToggleSection)( *this, *m_section, false );
   }
   else if ( !Title_CheckBox.IsNull() && sender == *Title_CheckBox )
   {
      if ( m_section != nullptr )
         m_section->Enable( checked );

      if ( !m_handlers.IsNull() )
         if ( m_handlers->onCheck != nullptr )
            (m_handlers->onCheckReceiver->*m_handlers->onCheck)( *this, checked );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::MousePress( Control& /*sender*/, const pcl::Point& /*pos*/,
                             int mouseButton, unsigned /*buttons*/, unsigned /*modifiers*/ )
{
   if ( mouseButton == MouseButton::Left )
      ButtonClick( Title_ToolButton, false );
}

// ----------------------------------------------------------------------------

void SectionBar::ControlShow( Control& sender )
{
   if ( m_section != nullptr )
   {
      const char* iconResource = nullptr;
      if ( sender == *m_section )
         iconResource = contract_icon;
      else if ( sender == *this )
      {
         if ( m_section->IsVisible() )
            iconResource = contract_icon;
         else
            iconResource = expand_icon;
      }
      if ( iconResource != nullptr )
         Title_ToolButton.SetIcon( Bitmap( ScaledResource( iconResource ) ) );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::ControlHide( Control& sender )
{
   if ( m_section != nullptr )
      if ( sender == *m_section )
         Title_ToolButton.SetIcon( Bitmap( ScaledResource( expand_icon ) ) );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SectionBar.cpp - Released 2017-06-17T10:55:56Z
