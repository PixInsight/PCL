//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 01.00.03.0266
// ----------------------------------------------------------------------------
// BMPInstance.h - Released 2015/11/26 15:59:58 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
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

#ifndef __BMPInstance_h
#define __BMPInstance_h

#include <pcl/FileFormatImplementation.h>

#include "BMPFormat.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Bitmap;

class BitmapInstance : public FileFormatImplementation
{
public:

   BitmapInstance( const MetaFileFormat* );
   virtual ~BitmapInstance();

   virtual ImageDescriptionArray Open( const String& filePath, const IsoString& hints );
   virtual bool IsOpen() const;
   virtual String FilePath() const;
   virtual void Create( const String& filePath, int numberOfImages, const IsoString& hints );
   virtual void Close();

   // Read image.
   virtual void ReadImage( Image& );
   virtual void ReadImage( DImage& );
   virtual void ReadImage( UInt8Image& );
   virtual void ReadImage( UInt16Image& );
   virtual void ReadImage( UInt32Image& );

   // Write image.
   virtual void WriteImage( const Image& );
   virtual void WriteImage( const DImage& );
   virtual void WriteImage( const UInt8Image& );
   virtual void WriteImage( const UInt16Image& );
   virtual void WriteImage( const UInt32Image& );

protected:

   Bitmap*   bitmap;
   String    path;
   ImageInfo info;
};

// ----------------------------------------------------------------------------

class BMPInstance : public BitmapInstance
{
public:

   BMPInstance( const BMPFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~BMPInstance()
   {
   }
};

class GIFInstance : public BitmapInstance
{
public:

   GIFInstance( const GIFFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~GIFInstance()
   {
   }
};

class ICOInstance : public BitmapInstance
{
public:

   ICOInstance( const ICOFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~ICOInstance()
   {
   }
};

class MNGInstance : public BitmapInstance
{
public:

   MNGInstance( const MNGFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~MNGInstance()
   {
   }
};

class PBMInstance : public BitmapInstance
{
public:

   PBMInstance( const PBMFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~PBMInstance()
   {
   }
};

class PGMInstance : public BitmapInstance
{
public:

   PGMInstance( const PGMFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~PGMInstance()
   {
   }
};

class PNGInstance : public BitmapInstance
{
public:

   PNGInstance( const PNGFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~PNGInstance()
   {
   }
};

class PPMInstance : public BitmapInstance
{
public:

   PPMInstance( const PPMFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~PPMInstance()
   {
   }
};

class XBMInstance : public BitmapInstance
{
public:

   XBMInstance( const XBMFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~XBMInstance()
   {
   }
};

class XPMInstance : public BitmapInstance
{
public:

   XPMInstance( const XPMFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~XPMInstance()
   {
   }
};

class TGAInstance : public BitmapInstance
{
public:

   TGAInstance( const TGAFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~TGAInstance()
   {
   }
};

class SVGInstance : public BitmapInstance
{
public:

   SVGInstance( const SVGFormat* F ) : BitmapInstance( F )
   {
   }

   virtual ~SVGInstance()
   {
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BMPInstance_h

// ----------------------------------------------------------------------------
// EOF BMPInstance.h - Released 2015/11/26 15:59:58 UTC
