//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0345
// ----------------------------------------------------------------------------
// OutputFileData.h - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __OutputFileData_h
#define __OutputFileData_h

#include <pcl/AutoPointer.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ICCProfile.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct OutputFileData
{
   AutoPointer<FileFormat> format;           // the file format of retrieved data
   const void*             fsData = nullptr; // format-specific data
   ImageOptions            options;          // currently used for resolution only
   PropertyArray           properties;       // image properties
   FITSKeywordArray        keywords;         // FITS keywords
   ICCProfile              profile;          // ICC profile

   OutputFileData() = default;

   OutputFileData( const OutputFileData& ) = delete;

   OutputFileData( OutputFileData&& x ) :
      format( std::move( x.format ) ),
      fsData( x.fsData ),
      options( x.options ),
      properties( std::move( x.properties ) ),
      keywords( std::move( x.keywords ) ),
      profile( std::move( x.profile ) )
   {
      x.fsData = nullptr;
   }

   OutputFileData( FileFormatInstance& file, const ImageOptions& o ) : options( o )
   {
      format = new FileFormat( file.Format() );

      if ( format->UsesFormatSpecificData() )
         fsData = file.FormatSpecificData();

      if ( format->CanStoreImageProperties() )
         properties = file.ReadImageProperties();

      if ( format->CanStoreKeywords() )
         file.ReadFITSKeywords( keywords );

      if ( format->CanStoreICCProfiles() )
         file.ReadICCProfile( profile );
   }

   ~OutputFileData()
   {
      if ( format )
      {
         if ( fsData != nullptr )
         {
            format->DisposeFormatSpecificData( const_cast<void*>( fsData ) );
            fsData = nullptr;
         }
      }
   }

   OutputFileData& operator =( const OutputFileData& ) = delete;

   OutputFileData& operator =( OutputFileData&& x )
   {
      format = std::move( x.format );
      fsData = x.fsData; x.fsData = nullptr;
      options = x.options;
      properties = std::move( x.properties );
      keywords = std::move( x.keywords );
      profile = std::move( x.profile );
      return *this;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __OutputFileData_h

// ----------------------------------------------------------------------------
// EOF OutputFileData.h - Released 2018-11-01T11:07:21Z
