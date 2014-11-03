// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Image Process Module Version 01.02.08.0281
// ****************************************************************************
// ExtractAlphaChannelsInterface.cpp - Released 2014/10/29 07:35:22 UTC
// ****************************************************************************
// This file is part of the standard Image PixInsight module.
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

#include "ExtractAlphaChannelsInterface.h"
#include "ExtractAlphaChannelsParameters.h"
#include "ExtractAlphaChannelsProcess.h"

#include <pcl/Parser.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ExtractAlphaChannelsInterface* TheExtractAlphaChannelsInterface = 0;

// ----------------------------------------------------------------------------

#include "ExtractAlphaChannelsIcon.xpm"

// ----------------------------------------------------------------------------

ExtractAlphaChannelsInterface::ExtractAlphaChannelsInterface() :
ProcessInterface(), instance( TheExtractAlphaChannelsProcess ), GUI( 0 )
{
   TheExtractAlphaChannelsInterface = this;
}

ExtractAlphaChannelsInterface::~ExtractAlphaChannelsInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString ExtractAlphaChannelsInterface::Id() const
{
   return "ExtractAlphaChannels";
}

MetaProcess* ExtractAlphaChannelsInterface::Process() const
{
   return TheExtractAlphaChannelsProcess;
}

const char** ExtractAlphaChannelsInterface::IconImageXPM() const
{
   return ExtractAlphaChannelsIcon_XPM;
}

void ExtractAlphaChannelsInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void ExtractAlphaChannelsInterface::ResetInstance()
{
   ExtractAlphaChannelsInstance defaultInstance( TheExtractAlphaChannelsProcess );
   ImportProcess( defaultInstance );
}

bool ExtractAlphaChannelsInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ExtractAlphaChannels" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheExtractAlphaChannelsProcess;
}

ProcessImplementation* ExtractAlphaChannelsInterface::NewProcess() const
{
   return new ExtractAlphaChannelsInstance( instance );
}

bool ExtractAlphaChannelsInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const ExtractAlphaChannelsInstance* r = dynamic_cast<const ExtractAlphaChannelsInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not an ExtractAlphaChannels instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool ExtractAlphaChannelsInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ExtractAlphaChannelsInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ExtractAlphaChannelsInterface::UpdateControls()
{
   GUI->AllAlphaChannels_RadioButton.SetChecked( instance.channels == EAChannels::AllAlphaChannels );
   GUI->ActiveAlphaChannel_RadioButton.SetChecked( instance.channels == EAChannels::ActiveAlphaChannel );
   GUI->ChannelList_RadioButton.SetChecked( instance.channels == EAChannels::ChannelList );

   GUI->ChannelList_Edit.SetText( instance.channelList );
   GUI->ChannelList_Edit.Enable( instance.channels == EAChannels::ChannelList );

   GUI->ExtractChannels_CheckBox.SetChecked( instance.extractChannels );
   GUI->DeleteChannels_CheckBox.SetChecked( instance.deleteChannels );
}

// ----------------------------------------------------------------------------

void ExtractAlphaChannelsInterface::__Channels_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AllAlphaChannels_RadioButton )
      instance.channels = EAChannels::AllAlphaChannels;
   else if ( sender == GUI->ActiveAlphaChannel_RadioButton )
      instance.channels = EAChannels::ActiveAlphaChannel;
   else if ( sender == GUI->ChannelList_RadioButton )
      instance.channels = EAChannels::ChannelList;
   UpdateControls();
}

void ExtractAlphaChannelsInterface::__ChannelList_EditCompleted( Edit& sender )
{
   try
   {
      String s = sender.Text();
      s.Trim();

      SortedArray<int> list;
      ExtractAlphaChannelsInstance::ParseChannelList( list, s );

      s.Clear();
      for ( size_type i = 0; ; )
      {
         s.Append( String( list[i] ) );
         if ( ++i == list.Length() )
            break;
         s.Append( ',' );
      }

      instance.channelList = s;
      sender.SetText( s );
   }

   ERROR_CLEANUP(
      sender.SetText( instance.channelList );
      sender.SelectAll();
      sender.Focus()
   )
}

void ExtractAlphaChannelsInterface::__Mode_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ExtractChannels_CheckBox )
      instance.extractChannels = checked;
   else if ( sender == GUI->DeleteChannels_CheckBox )
      instance.deleteChannels = checked;
   if ( !instance.extractChannels && !instance.deleteChannels )
      instance.extractChannels = true;
   UpdateControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ExtractAlphaChannelsInterface::GUIData::GUIData( ExtractAlphaChannelsInterface& w )
{
   pcl::Font fnt = w.Font();
   int editWidth = fnt.Width( String( 'M', 40 ) );

   //

   AllAlphaChannels_RadioButton.SetText( "All alpha channels" );
   AllAlphaChannels_RadioButton.SetToolTip( "Extract/delete all existing alpha channels." );
   AllAlphaChannels_RadioButton.OnClick( (Button::click_event_handler)&ExtractAlphaChannelsInterface::__Channels_Click, w );

   ActiveAlphaChannel_RadioButton.SetText( "Active alpha channel" );
   ActiveAlphaChannel_RadioButton.SetToolTip( "Extract/delete the active (first) alpha channel only." );
   ActiveAlphaChannel_RadioButton.OnClick( (Button::click_event_handler)&ExtractAlphaChannelsInterface::__Channels_Click, w );

   ChannelList_RadioButton.SetText( "Channel list" );
   ChannelList_RadioButton.SetToolTip( "Specify a comma-separated list of alpha channel indexes to extract/delete." );
   ChannelList_RadioButton.OnClick( (Button::click_event_handler)&ExtractAlphaChannelsInterface::__Channels_Click, w );

   ChannelList_Edit.SetMinWidth( editWidth );
   ChannelList_Edit.OnEditCompleted( (Edit::edit_event_handler)&ExtractAlphaChannelsInterface::__ChannelList_EditCompleted, w );

   Channels_Sizer.SetMargin( 6 );
   Channels_Sizer.SetSpacing( 4 );
   Channels_Sizer.Add( AllAlphaChannels_RadioButton );
   Channels_Sizer.Add( ActiveAlphaChannel_RadioButton );
   Channels_Sizer.Add( ChannelList_RadioButton );
   Channels_Sizer.Add( ChannelList_Edit );

   Channels_GroupBox.SetTitle( "Channels" );
   Channels_GroupBox.SetSizer( Channels_Sizer );

   //

   ExtractChannels_CheckBox.SetText( "Extract alpha channels" );
   ExtractChannels_CheckBox.SetToolTip( "Extract alpha channels as new images." );
   ExtractChannels_CheckBox.OnClick( (Button::click_event_handler)&ExtractAlphaChannelsInterface::__Mode_Click, w );

   DeleteChannels_CheckBox.SetText( "Delete alpha channels" );
   DeleteChannels_CheckBox.SetToolTip( "Delete specified alpha channels from the target image(s)." );
   DeleteChannels_CheckBox.OnClick( (Button::click_event_handler)&ExtractAlphaChannelsInterface::__Mode_Click, w );

   Mode_Sizer.SetMargin( 6 );
   Mode_Sizer.SetSpacing( 4 );
   Mode_Sizer.Add( ExtractChannels_CheckBox );
   Mode_Sizer.Add( DeleteChannels_CheckBox );

   Mode_GroupBox.SetTitle( "Mode" );
   Mode_GroupBox.SetSizer( Mode_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Channels_GroupBox );
   Global_Sizer.Add( Mode_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ExtractAlphaChannelsInterface.cpp - Released 2014/10/29 07:35:22 UTC
