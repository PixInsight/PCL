// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Flux Process Module Version 01.00.00.0066
// ****************************************************************************
// PreviewSelectionDialog.cpp - Released 2014/11/14 17:18:46 UTC
// ****************************************************************************
// This file is part of the standard Flux PixInsight module.
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

#include "PreviewSelectionDialog.h"

#include <pcl/Dialog.h>
#include <pcl/ViewList.h>
#include <pcl/PushButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PreviewSelectionDialog::PreviewSelectionDialog( const String& aId ) : Dialog(), id( aId )
{
   Images_ViewList.GetPreviews();
   Images_ViewList.SelectView( View::ViewById( id ) );
   Images_ViewList.SetMinWidth( Font().Width( String( 'M', 40 ) ) );
   Images_ViewList.OnViewSelected( (ViewList::view_event_handler)&PreviewSelectionDialog::__ViewSelected, *this );

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&PreviewSelectionDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&PreviewSelectionDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Images_ViewList );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedHeight();

   SetWindowTitle( "Select Preview" );
}

void PreviewSelectionDialog::__ViewSelected( ViewList& sender, View& view )
{
   id = view.IsNull() ? String() : String( view.FullId() );
}

void PreviewSelectionDialog::__Option_Click( Button& sender, bool checked )
{
   Images_ViewList.GetPreviews();
   Images_ViewList.SelectView( View::ViewById( id ) );
}

void PreviewSelectionDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF PreviewSelectionDialog.cpp - Released 2014/11/14 17:18:46 UTC
