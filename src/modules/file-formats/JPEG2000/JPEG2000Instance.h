//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 01.00.01.0269
// ----------------------------------------------------------------------------
// JPEG2000Instance.h - Released 2015/11/26 15:59:58 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
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

#ifndef __JPEG2000Instance_h
#define __JPEG2000Instance_h

#include <pcl/FileFormatImplementation.h>

//PCL_BEGIN_LOCAL // Uncomment these *_LOCAL macros if using JasPer as a static library
#include <jasper/jasper.h>
//PCL_END_LOCAL

namespace pcl
{

// ----------------------------------------------------------------------------

class JPCFormat;
class JP2Format;

/*
 * JPEG2000 Format-Specific Data
 */
struct JPEG2000ProgressionOrder
{
   enum { LRCP, RLCP, RPCL, PCRL, CPRL }; // see JasPer documentation
};

struct JPEG2000ImageOptions
{
   bool  lossyCompression; // use lossy JPEG2000 compression
   float compressionRate;  // compressed/original: 1.0 = no compression
   bool  signedSample;     // signed sample values
   bool  resolutionData;   // include resolution information
   bool  tiledImage;       // partition the image into tiles
   int   tileWidth;        // tile width in pixels
   int   tileHeight;       // tile height in pixels
   int   numberOfLayers;   // > 1 -> progressive JPEG2000
   int   progressionOrder; // use JasPer code

   JPEG2000ImageOptions()
   {
      Reset();
   }

   JPEG2000ImageOptions( const JPEG2000ImageOptions& ) = default;

   void Reset()
   {
      lossyCompression = false;
      compressionRate  = 0.1F;
      signedSample     = false;
      resolutionData   = true;
      tiledImage       = false;
      tileWidth        = 256;
      tileHeight       = 256;
      numberOfLayers   = 1;
      progressionOrder = JPEG2000ProgressionOrder::LRCP;
   }
};

/*
 * JPEG-2000 Code Stream Format Instance.
 */
class JPCInstance : public FileFormatImplementation
{
public:

   JPCInstance( const JPCFormat* );
   virtual ~JPCInstance();

   virtual ImageDescriptionArray Open( const String& filePath, const IsoString& hints );
   virtual bool IsOpen() const;
   virtual String FilePath() const;
   virtual void Close();

   virtual void* FormatSpecificData() const;
   virtual String ImageProperties() const;

   virtual void ReadImage( Image& );
   virtual void ReadImage( DImage& );
   virtual void ReadImage( UInt8Image& );
   virtual void ReadImage( UInt16Image& );
   virtual void ReadImage( UInt32Image& );

   virtual bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions );
   virtual void Create( const String& filePath, int numberOfImages, const IsoString& hints );
   virtual void SetOptions( const ImageOptions& options );
   virtual void SetFormatSpecificData( const void* data );

   virtual void WriteImage( const Image& );
   virtual void WriteImage( const DImage& );
   virtual void WriteImage( const UInt8Image& );
   virtual void WriteImage( const UInt16Image& );
   virtual void WriteImage( const UInt32Image& );

   virtual bool WasLossyWrite() const;

protected:

   // Parameters of the image being read/written
   String               path;       // current file path
   ImageOptions         options;    // format-independent options
   JPEG2000ImageOptions jp2Options; // format-specific options

   // JasPer objects
   jas_stream_t*        jp2Stream;
   jas_image_t*         jp2Image;

   // Embedded ICC profile, JP2 format only.
   jas_cmprof_t*        jp2CMProfile; // JasPer's color management, incl. ICC profile

   // Did us query options to the user?
   bool                 queriedOptions;

   void CreateImage( const ImageInfo& );

   virtual bool IsCodeStream() const
   {
      return true;
   }
};

/*
 * JPEG-2000 Format Instance.
 */
class JP2Instance : public JPCInstance
{
public:

   JP2Instance( const JP2Format* );
   virtual ~JP2Instance();

   virtual void Extract( ICCProfile& );
   virtual void Embed( const ICCProfile& );

private:

   virtual bool IsCodeStream() const
   {
      return false;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEG2000Instance_h

// ----------------------------------------------------------------------------
// EOF JPEG2000Instance.h - Released 2015/11/26 15:59:58 UTC
