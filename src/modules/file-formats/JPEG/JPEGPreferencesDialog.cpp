// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard JPEG File Format Module Version 01.00.01.0228
// ****************************************************************************
// JPEGPreferencesDialog.cpp - Released 2014/11/14 17:18:35 UTC
// ****************************************************************************
// This file is part of the standard JPEG PixInsight module.
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

#include "JPEGPreferencesDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

static ImageOptions options;

JPEGPreferencesDialog::JPEGPreferencesDialog( const JPEGFormat::EmbeddingOverrides& e, const JPEGImageOptions& o ) :
JPEGOptionsDialog( options, o ), overrides( e )
{
   ICCProfile_CheckBox.SetTristateMode();
   ICCProfile_CheckBox.SetToolTip( "Override global core application settings for embedded ICC profiles." );
   ICCProfile_CheckBox.SetState( overrides.overrideICCProfileEmbedding ?
      (overrides.embedICCProfiles ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Metadata_CheckBox.SetTristateMode();
   Metadata_CheckBox.SetToolTip( "Override global core application settings for embedded XML metadata." );
   Metadata_CheckBox.SetState( overrides.overrideMetadataEmbedding ?
      (overrides.embedMetadata ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Thumbnail_CheckBox.SetTristateMode();
   Thumbnail_CheckBox.SetToolTip( "Override global core application settings for embedded thumbnails." );
   Thumbnail_CheckBox.SetState( overrides.overrideThumbnailEmbedding ?
      (overrides.embedThumbnails ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   EmbeddedData_GroupBox.SetTitle( "Override Embedding Settings" );

   SetWindowTitle( "JPEG Format Preferences" );

   OnReturn( (Dialog::return_event_handler)&JPEGPreferencesDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void JPEGPreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   JPEGOptionsDialog::Dialog_Return( sender, retVal );

   if ( retVal == StdDialogCode::Ok )
   {
      overrides.overrideICCProfileEmbedding = ICCProfile_CheckBox.State() != CheckState::ThirdState;
      overrides.embedICCProfiles = ICCProfile_CheckBox.IsChecked();

      overrides.overrideMetadataEmbedding = Metadata_CheckBox.State() != CheckState::ThirdState;
      overrides.embedMetadata = Metadata_CheckBox.IsChecked();

      overrides.overrideThumbnailEmbedding = Thumbnail_CheckBox.State() != CheckState::ThirdState;
      overrides.embedThumbnails = Thumbnail_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF JPEGPreferencesDialog.cpp - Released 2014/11/14 17:18:35 UTC
