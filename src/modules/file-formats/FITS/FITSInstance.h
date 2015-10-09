//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.02.0314
// ----------------------------------------------------------------------------
// FITSInstance.h - Released 2015/10/08 11:24:33 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#ifndef __FITSInstance_h
#define __FITSInstance_h

#include "FITS.h"

#include <pcl/FileFormatImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSFormat;
class FITSReadHints;

class FITSInstance : public FileFormatImplementation
{
public:

   FITSInstance( const FITSFormat* );

   virtual ~FITSInstance();

   virtual ImageDescriptionArray Open( const String& filePath, const IsoString& hints );
   virtual bool IsOpen() const;
   virtual String FilePath() const;
   virtual void Close();

   virtual void SelectImage( int index );
   virtual int SelectedImageIndex() const;

   virtual void* FormatSpecificData() const;

   virtual void Extract( FITSKeywordArray& keywords );
   virtual void Extract( ICCProfile& icc );
   virtual void Extract( UInt8Image& thumbnail );
   virtual ImagePropertyDescriptionArray Properties();
   virtual Variant ReadProperty( const IsoString& property );
   virtual void WriteProperty( const IsoString& property, const Variant& value );

   virtual void ReadImage( Image& );
   virtual void ReadImage( DImage& );
   virtual void ReadImage( UInt8Image& );
   virtual void ReadImage( UInt16Image& );
   virtual void ReadImage( UInt32Image& );

   virtual void Read( Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( DImage::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   virtual bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions );
   virtual void Create( const String& filePath, int numberOfImages, const IsoString& hints );
   virtual void SetId( const IsoString& id );
   virtual void SetOptions( const ImageOptions& options );
   virtual void SetFormatSpecificData( const void* data );

   virtual void Embed( const FITSKeywordArray& keywords );
   virtual void Embed( const ICCProfile& icc );
   virtual void Embed( const UInt8Image& image );

   virtual void WriteImage( const Image& );
   virtual void WriteImage( const DImage& );
   virtual void WriteImage( const UInt8Image& );
   virtual void WriteImage( const UInt16Image& );
   virtual void WriteImage( const UInt32Image& );

   virtual void CreateImage( const ImageInfo& info );

   virtual void Write( const Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const DImage::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

private:

   FITSReader*    reader;
   FITSWriter*    writer;
   FITSReadHints* readHints;

   // Embedded data that requires persistent objects
   ICCProfile* embeddedICCProfile;
   UInt8Image* embeddedThumbnail;

   bool queriedOptions : 1; // did us query options to the user?
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSInstance_h

// ----------------------------------------------------------------------------
// EOF FITSInstance.h - Released 2015/10/08 11:24:33 UTC
