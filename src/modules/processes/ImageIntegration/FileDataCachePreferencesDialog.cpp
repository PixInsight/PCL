//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.11.00.0344
// ----------------------------------------------------------------------------
// FileDataCachePreferencesDialog.cpp - Released 2016/11/13 17:30:54 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "FileDataCachePreferencesDialog.h"
#include "FileDataCache.h"

#include <pcl/MessageBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FileDataCachePreferencesDialog::FileDataCachePreferencesDialog( FileDataCache* theCache ) :
   Dialog(),
   cache( theCache )
{
   int labelWidth1 = Font().Width( String( "Cache duration (days):" ) + 'T' );
   int ui4 = LogicalPixelsToPhysical( 4 );

   //

   const char* persistentCacheToolTip =
   "<p>Use a persistent file cache to store statistical data and noise "
   "estimates of all integrated images. A persistent cache is kept across "
   "PixInsight sessions. If you disable this option, the file cache will "
   "still be used, but only during the current session; as soon as you exit "
   "the PixInsight Core application, all the cached information will be lost. "
   "With the persistent cache option enabled, all cache items will be stored "
   "and will be available the next time you run PixInsight.</p>"
   "<p>The cache greatly improves performance when the same images are being "
   "integrated several times; for example to find optimal pixel rejection "
   "parameters, or to check different HDR composition thresholds.</p>";

   PersistentCache_CheckBox.SetText( "Persistent file cache" );
   PersistentCache_CheckBox.SetToolTip( persistentCacheToolTip );
   PersistentCache_CheckBox.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::Button_Click, *this );

   PersistentCache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PersistentCache_Sizer.Add( PersistentCache_CheckBox );
   PersistentCache_Sizer.AddStretch();

   //

   const char* cacheDurationToolTip =
   "<p>Persistent file cache items can be automatically removed after a "
   "specified period without accessing the corresponding files. Enter the "
   "desired period in days, or specify zero to disable this <i>automatic purge</i> "
   "feature, so that existing file cache items will never be removed "
   "automatically.</p>";

   CacheDuration_Label.SetText( "Cache duration (days):" );
   CacheDuration_Label.SetMinWidth( labelWidth1 );
   CacheDuration_Label.SetToolTip( cacheDurationToolTip );
   CacheDuration_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   CacheDuration_SpinBox.SetRange( 0, 90 );
   CacheDuration_SpinBox.SetMinimumValueText( "<Forever>" );
   CacheDuration_SpinBox.SetToolTip( cacheDurationToolTip );
   CacheDuration_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&FileDataCachePreferencesDialog::SpinBox_ValueUpdated, *this );

   CacheDuration_Sizer.SetSpacing( 4 );
   CacheDuration_Sizer.Add( CacheDuration_Label );
   CacheDuration_Sizer.Add( CacheDuration_SpinBox );
   CacheDuration_Sizer.AddStretch();

   //

   ClearCache_PushButton.SetText( "Clear Memory Cache Now" );
   ClearCache_PushButton.SetToolTip(
      "Click this button to remove all cache items currently stored in volatile RAM." );
   ClearCache_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::Button_Click, *this );

   ClearCache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ClearCache_Sizer.Add( ClearCache_PushButton, 100 );

   //

   PurgeCache_PushButton.SetText( "Purge Persistent Cache Now" );
   PurgeCache_PushButton.SetToolTip(
      "Click this button to remove all stored persistent cache items." );
   PurgeCache_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::Button_Click, *this );

   PurgeCache_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PurgeCache_Sizer.Add( PurgeCache_PushButton, 100 );

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&FileDataCachePreferencesDialog::Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( PersistentCache_Sizer );
   Global_Sizer.Add( CacheDuration_Sizer );
   Global_Sizer.Add( ClearCache_Sizer );
   Global_Sizer.Add( PurgeCache_Sizer );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   //

   SetWindowTitle( cache->CacheName() + " Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&FileDataCachePreferencesDialog::Dialog_Return, *this );

   cacheEnabled = cache->IsEnabled();
   cacheDuration = cache->Duration();

   Update();
}

void FileDataCachePreferencesDialog::Update()
{
   PersistentCache_CheckBox.SetChecked( cacheEnabled );
   CacheDuration_Label.Enable( cacheEnabled );
   CacheDuration_SpinBox.Enable( cacheEnabled );
   CacheDuration_SpinBox.SetValue( cacheDuration );
}

void FileDataCachePreferencesDialog::SpinBox_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == CacheDuration_SpinBox )
   {
      cacheDuration = value;
      Update();
   }
}

void FileDataCachePreferencesDialog::Button_Click( Button& sender, bool checked )
{
   if ( sender == ClearCache_PushButton )
   {
      if ( MessageBox( "<p>This will delete all cache items currently stored in volatile memory.</p>"
                       "<p><b>This action is irreversible. Proceed?</b></p>",
                       cache->CacheName() + " - Clear Memory Cache", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
      {
         size_type n = cache->NumberOfItems();
         cache->Clear();
         MessageBox( "<p>" + String( n ) + " cache item(s) were deleted.</p>",
                     cache->CacheName() + " - Clear Memory Cache", // caption
                     StdIcon::Information,
                     StdButton::Ok ).Execute();
      }
   }
   else if ( sender == PurgeCache_PushButton )
   {
      if ( MessageBox( "<p>This will delete all stored persistent file cache items.</p>"
                       "<p><b>This action is irreversible. Proceed?</b></p>",
                       cache->CacheName() + " - Purge Persistent Cache", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
      {
         cache->Purge();
         MessageBox( "<p>All persistent cache items were deleted.</p>",
                     cache->CacheName() + " - Purge Persistent Cache", // caption
                     StdIcon::Information,
                     StdButton::Ok ).Execute();
      }
   }
   else if ( sender == PersistentCache_CheckBox )
   {
      cacheEnabled = checked;
      Update();
   }
   else if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void FileDataCachePreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      cache->Enable( cacheEnabled );
      cache->SetDuration( cacheDuration );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FileDataCachePreferencesDialog.cpp - Released 2016/11/13 17:30:54 UTC
