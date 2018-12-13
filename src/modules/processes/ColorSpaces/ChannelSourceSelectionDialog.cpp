//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0368
// ----------------------------------------------------------------------------
// ChannelSourceSelectionDialog.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "ChannelSourceSelectionDialog.h"

#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelSourceSelectionDialog::ChannelSourceSelectionDialog( const String& channelSuffix,
                                                            const String& channelDesc )
{
   pcl::Font fnt = Font();
   int comboWidth = fnt.Width( String( 'M', 40 ) );
   int buttonWidth = fnt.Width( String( "Cancel" ) + String( 'M', 4 ) );

   Array<ImageWindow> windows = ImageWindow::AllWindows();
   Array<String> targetImages;
   for ( size_type i = 0; i < windows.Length(); ++i )
   {
      View v = windows[i].MainView();
      if ( !v.IsColor() )
         targetImages.Add( v.Id() );
   }

   if ( targetImages.IsEmpty() )
      SourceImages_ComboBox.AddItem( " <* No compatible images *> " );
   else
   {
      size_type currentItem = String::notFound;

      for ( size_type i = 0; i < targetImages.Length(); ++i )
      {
         SourceImages_ComboBox.AddItem( ' ' + targetImages[i] + ' ' );

         if ( currentItem == String::notFound && targetImages[i].EndsWithIC( channelSuffix ) )
            currentItem = i;
      }

      SourceImages_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ChannelSourceSelectionDialog::__SourceImages_ItemSelected, *this );

      if ( currentItem != String::notFound )
         SourceImages_ComboBox.SetCurrentItem( int( currentItem ) );

      sourceId = targetImages[SourceImages_ComboBox.CurrentItem()];
   }

   SourceImages_ComboBox.SetMinWidth( comboWidth );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetFixedWidth( buttonWidth );
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&ChannelSourceSelectionDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetFixedWidth( buttonWidth );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&ChannelSourceSelectionDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( SourceImages_ComboBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Select " + channelDesc + " Source Image" );
}

void ChannelSourceSelectionDialog::__SourceImages_ItemSelected( ComboBox& sender, int itemIndex )
{
   sourceId = sender.ItemText( itemIndex );
   sourceId.Trim();
}

void ChannelSourceSelectionDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelSourceSelectionDialog.cpp - Released 2018-12-12T09:25:24Z
