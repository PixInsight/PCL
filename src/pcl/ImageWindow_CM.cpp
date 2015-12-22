//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/ImageWindow_CM.cpp - Released 2015/12/17 18:52:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/ImageWindow.h>
#include <pcl/ICCProfile.h>
#include <pcl/ErrorHandler.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

bool ImageWindow::IsColorManagementEnabled() const
{
   api_bool enableCM;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, &enableCM, 0, 0 );
   return enableCM != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableColorManagement( bool enable )
{
   api_bool proofing, gamutCheck;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, 0, &proofing, &gamutCheck );
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, enable, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsProofingEnabled() const
{
   api_bool proofing;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, 0, &proofing, 0 );
   return proofing != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableProofing( bool enable )
{
   api_bool enableCM, gamutCheck;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, &enableCM, 0, &gamutCheck );
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, enableCM, enable, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsGamutCheckEnabled() const
{
   api_bool gamutCheck;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, 0, 0, &gamutCheck );
   return gamutCheck != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableGamutCheck( bool enable )
{
   api_bool enableCM, proofing;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, &enableCM, &proofing, 0 );
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, enableCM, enable || proofing, enable );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetColorManagementFeatures( bool cmEnabled, bool proofing, bool gamutCheck )
{
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, cmEnabled, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageWindow::GetICCProfile( ICCProfile& profile ) const
{
   profile.Clear();

   size_type profileSize = (*API->ImageWindow->GetImageWindowICCProfileLength)( handle );
   if ( profileSize == 0 )
      return false;

   try
   {
      ByteArray profileData( profileSize );
      (*API->ImageWindow->GetImageWindowICCProfile)( handle, profileData.Begin() );
      profile.Set( profileData );
      return true;
   }
   ERROR_HANDLER

   return false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetICCProfile( const ICCProfile& profile )
{
   if ( profile.IsProfile() )
      (*API->ImageWindow->SetImageWindowICCProfile)( handle, profile.ProfileData().Begin() );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetICCProfile( const String& filePath )
{
   (*API->ImageWindow->LoadImageWindowICCProfile)( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

void ImageWindow::DeleteICCProfile()
{
   (*API->ImageWindow->DeleteImageWindowICCProfile)( handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageWindow_CM.cpp - Released 2015/12/17 18:52:18 UTC
