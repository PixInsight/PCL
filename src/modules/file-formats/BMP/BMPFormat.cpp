// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard BMP File Format Module Version 01.00.03.0209
// ****************************************************************************
// BMPFormat.cpp - Released 2014/10/29 07:34:47 UTC
// ****************************************************************************
// This file is part of the standard BMP PixInsight module.
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

#include "BMPFormat.h"
#include "BMPInstance.h"

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BMPFormat::BMPFormat() : MetaFileFormat()
{
}

IsoString BMPFormat::Name() const
{
   return "BMP";
}

StringList BMPFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".bmp" );
   return exts;
}

IsoStringList BMPFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/bmp" );
   mimes.Add( "image/x-bmp" );
   mimes.Add( "image/x-bitmap" );
   mimes.Add( "image/x-xbitmap" );
   mimes.Add( "image/x-win-bitmap" );
   mimes.Add( "image/x-windows-bmp" );
   mimes.Add( "image/ms-bmp" );
   mimes.Add( "image/x-ms-bmp" );
   mimes.Add( "application/bmp" );
   mimes.Add( "application/x-bmp" );
   mimes.Add( "application/x-win-bitmap" );
   return mimes;
}

uint32 BMPFormat::Version() const
{
   return 0x100;
}

String BMPFormat::Description() const
{
   return "Windows Bitmap format (BMP).";
}

String BMPFormat::Implementation() const
{
   return
   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Support for the Windows Bitmap format (BMP), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String BMPFormat::IconImageFile() const
{
   return ":/file-format/bmp-format-icon.png";
}

bool BMPFormat::CanRead() const
{
   return true;
}

bool BMPFormat::CanWrite() const
{
   return true;
}

bool BMPFormat::CanStore8Bit() const
{
   return true;
}

bool BMPFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* BMPFormat::Create() const
{
   return new BMPInstance( this );
}

// ----------------------------------------------------------------------------

GIFFormat::GIFFormat() : MetaFileFormat()
{
}

IsoString GIFFormat::Name() const
{
   return "GIF";
}

StringList GIFFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".gif" );
   return exts;
}

IsoStringList GIFFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/gif" );  // RFC2045,RFC2046
   return mimes;
}

uint32 GIFFormat::Version() const
{
   return 0x100;
}

String GIFFormat::Description() const
{
   return "Graphics Interchange Format (GIF) - read-only support.";
}

String GIFFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Read-only support for the Graphics Interchange Format (GIF), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String GIFFormat::IconImageFile() const
{
   return ":/file-format/gif-format-icon.png";
}

bool GIFFormat::CanRead() const
{
   return true;
}

bool GIFFormat::CanWrite() const
{
   return false;
}

bool GIFFormat::CanStore8Bit() const
{
   return true;
}

bool GIFFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* GIFFormat::Create() const
{
   return new GIFInstance( this );
}

// ----------------------------------------------------------------------------

ICOFormat::ICOFormat() : MetaFileFormat()
{
}

IsoString ICOFormat::Name() const
{
   return "ICO";
}

StringList ICOFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".ico" );
   return exts;
}

IsoStringList ICOFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/ico" );
   mimes.Add( "image/x-icon" );
   mimes.Add( "application/ico" );
   mimes.Add( "application/x-ico" );
   return mimes;
}

uint32 ICOFormat::Version() const
{
   return 0x100;
}

String ICOFormat::Description() const
{
   return "Windows Icon format (ICO).";
}

String ICOFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Support for the Windows Icon format (ICO), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String ICOFormat::IconImageFile() const
{
   return ":/file-format/ico-format-icon.png";
}

bool ICOFormat::CanRead() const
{
   return true;
}

bool ICOFormat::CanWrite() const
{
   return true;
}

bool ICOFormat::CanStore8Bit() const
{
   return true;
}

bool ICOFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* ICOFormat::Create() const
{
   return new ICOInstance( this );
}

// ----------------------------------------------------------------------------

MNGFormat::MNGFormat() : MetaFileFormat()
{
}

IsoString MNGFormat::Name() const
{
   return "MNG";
}

StringList MNGFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".mng" );
   return exts;
}

IsoStringList MNGFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/mng" );
   mimes.Add( "video/mng" );
   mimes.Add( "video/x-mng" );
   return mimes;
}

uint32 MNGFormat::Version() const
{
   return 0x100;
}

String MNGFormat::Description() const
{
   return "Multiple Network Graphics format (MNG) - read-only support.";
}

String MNGFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Read-only support for the Multiple Network Graphics format (MNG), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String MNGFormat::IconImageFile() const
{
   return ":/file-format/mng-format-icon.png";
}

bool MNGFormat::CanRead() const
{
   return true;
}

bool MNGFormat::CanWrite() const
{
   return false;
}

bool MNGFormat::CanStore8Bit() const
{
   return true;
}

bool MNGFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* MNGFormat::Create() const
{
   return new MNGInstance( this );
}

// ----------------------------------------------------------------------------

PBMFormat::PBMFormat() : MetaFileFormat()
{
}

IsoString PBMFormat::Name() const
{
   return "PBM";
}

StringList PBMFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".pbm" );
   return exts;
}

IsoStringList PBMFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/pbm" );
   mimes.Add( "image/x-pbm" );
   mimes.Add( "image/portable bitmap" );
   mimes.Add( "image/x-portable-bitmap" );
   return mimes;
}

uint32 PBMFormat::Version() const
{
   return 0x100;
}

String PBMFormat::Description() const
{
   return "Portable Bitmap format (PBM) - read-only support.";
}

String PBMFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Read-only support for the Portable Bitmap format (PBM), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String PBMFormat::IconImageFile() const
{
   return ":/file-format/pbm-format-icon.png";
}

bool PBMFormat::CanRead() const
{
   return true;
}

bool PBMFormat::CanWrite() const
{
   return false;
}

bool PBMFormat::CanStore8Bit() const
{
   return true;
}

bool PBMFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* PBMFormat::Create() const
{
   return new PBMInstance( this );
}

// ----------------------------------------------------------------------------

PGMFormat::PGMFormat() : MetaFileFormat()
{
}

IsoString PGMFormat::Name() const
{
   return "PGM";
}

StringList PGMFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".pgm" );
   return exts;
}

IsoStringList PGMFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/pgm" );
   mimes.Add( "image/x-pgm" );
   mimes.Add( "image/portable graymap" );
   mimes.Add( "image/x-portable-graymap" );
   return mimes;
}

uint32 PGMFormat::Version() const
{
   return 0x100;
}

String PGMFormat::Description() const
{
   return "Portable Graymap format (PGM) - read-only support.";
}

String PGMFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Read-only support for the Portable Graymap format (PGM), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String PGMFormat::IconImageFile() const
{
   return ":/file-format/pgm-format-icon.png";
}

bool PGMFormat::CanRead() const
{
   return true;
}

bool PGMFormat::CanWrite() const
{
   return false;
}

bool PGMFormat::CanStore8Bit() const
{
   return true;
}

bool PGMFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* PGMFormat::Create() const
{
   return new PGMInstance( this );
}

// ----------------------------------------------------------------------------

PNGFormat::PNGFormat() : MetaFileFormat()
{
}

IsoString PNGFormat::Name() const
{
   return "PNG";
}

StringList PNGFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".png" );
   return exts;
}

IsoStringList PNGFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/png" );
   mimes.Add( "application/png" );
   mimes.Add( "application/x-png" );
   return mimes;
}

uint32 PNGFormat::Version() const
{
   return 0x100;
}

String PNGFormat::Description() const
{
   return "Portable Network Graphics format (PNG).";
}

String PNGFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Basic support for the Portable Network Graphics format (PNG), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String PNGFormat::IconImageFile() const
{
   return ":/file-format/png-format-icon.png";
}

bool PNGFormat::CanRead() const
{
   return true;
}

bool PNGFormat::CanWrite() const
{
   return true;
}

bool PNGFormat::CanStore8Bit() const
{
   return true;
}

bool PNGFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* PNGFormat::Create() const
{
   return new PNGInstance( this );
}

// ----------------------------------------------------------------------------

PPMFormat::PPMFormat() : MetaFileFormat()
{
}

IsoString PPMFormat::Name() const
{
   return "PPM";
}

StringList PPMFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".ppm" );
   return exts;
}

IsoStringList PPMFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/ppm" );
   mimes.Add( "image/x-portable-pixmap" );
   mimes.Add( "image/x-p" );
   mimes.Add( "image/x-ppm" );
   mimes.Add( "application/ppm" );
   mimes.Add( "application/x-ppm" );
   return mimes;
}

uint32 PPMFormat::Version() const
{
   return 0x100;
}

String PPMFormat::Description() const
{
   return "Portable Pixmap format (PPM).";
}

String PPMFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Support for the Portable Pixmap format (PPM), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String PPMFormat::IconImageFile() const
{
   return ":/file-format/ppm-format-icon.png";
}

bool PPMFormat::CanRead() const
{
   return true;
}

bool PPMFormat::CanWrite() const
{
   return true;
}

bool PPMFormat::CanStore8Bit() const
{
   return true;
}

bool PPMFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* PPMFormat::Create() const
{
   return new PPMInstance( this );
}

// ----------------------------------------------------------------------------

XBMFormat::XBMFormat() : MetaFileFormat()
{
}

IsoString XBMFormat::Name() const
{
   return "XBM";
}

StringList XBMFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".xbm" );
   return exts;
}

IsoStringList XBMFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/xbm" );
   mimes.Add( "image/x-xbitmap" );
   return mimes;
}

uint32 XBMFormat::Version() const
{
   return 0x100;
}

String XBMFormat::Description() const
{
   return "X11 Bitmap format (XBM).";
}

String XBMFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Support for the X11 Bitmap format (XBM), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String XBMFormat::IconImageFile() const
{
   return ":/file-format/xbm-format-icon.png";
}

bool XBMFormat::CanRead() const
{
   return true;
}

bool XBMFormat::CanWrite() const
{
   return true;
}

bool XBMFormat::CanStore8Bit() const
{
   return true;
}

bool XBMFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* XBMFormat::Create() const
{
   return new XBMInstance( this );
}

// ----------------------------------------------------------------------------

XPMFormat::XPMFormat() : MetaFileFormat()
{
}

IsoString XPMFormat::Name() const
{
   return "XPM";
}

StringList XPMFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".xpm" );
   return exts;
}

IsoStringList XPMFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/xpm" );
   mimes.Add( "image/x-xpm" );
   mimes.Add( "image/x-xpixmap" );
   return mimes;
}

uint32 XPMFormat::Version() const
{
   return 0x100;
}

String XPMFormat::Description() const
{
   return "X11 Pixmap format (XPM).";
}

String XPMFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Support for the X11 Pixmap format (XPM), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String XPMFormat::IconImageFile() const
{
   return ":/file-format/xpm-format-icon.png";
}

bool XPMFormat::CanRead() const
{
   return true;
}

bool XPMFormat::CanWrite() const
{
   return true;
}

bool XPMFormat::CanStore8Bit() const
{
   return true;
}

bool XPMFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* XPMFormat::Create() const
{
   return new XPMInstance( this );
}

// ----------------------------------------------------------------------------

TGAFormat::TGAFormat() : MetaFileFormat()
{
}

IsoString TGAFormat::Name() const
{
   return "TGA";
}

StringList TGAFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".tga" );
   exts.Add( ".tpic" );
   return exts;
}

IsoStringList TGAFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/tga" );
   mimes.Add( "image/x-tga" );
   mimes.Add( "image/targa" );
   mimes.Add( "image/x-targa" );
   mimes.Add( "application/tga" );
   mimes.Add( "application/x-tga" );
   mimes.Add( "application/x-targa" );
   return mimes;
}

uint32 TGAFormat::Version() const
{
   return 0x100;
}

String TGAFormat::Description() const
{
   return "Truevision TGA format (TARGA).";
}

String TGAFormat::Implementation() const
{
   return
   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Read-only support for the Truevision TGA format (TARGA), based on the Qt4 library.</p>"

   "<p>RLE compressed TGA files are not supported by this reader. Only uncompressed images can be read.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String TGAFormat::IconImageFile() const
{
   return ":/file-format/tga-format-icon.png";
}

bool TGAFormat::CanRead() const
{
   return true;
}

bool TGAFormat::CanWrite() const
{
   return false;
}

bool TGAFormat::CanStore8Bit() const
{
   return true;
}

bool TGAFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* TGAFormat::Create() const
{
   return new TGAInstance( this );
}

// ----------------------------------------------------------------------------

SVGFormat::SVGFormat() : MetaFileFormat()
{
}

IsoString SVGFormat::Name() const
{
   return "SVG";
}

StringList SVGFormat::FileExtensions() const
{
   StringList exts;
   exts.Add( ".svg" );
   return exts;
}

IsoStringList SVGFormat::MimeTypes() const
{
   IsoStringList mimes;
   mimes.Add( "image/svg" );
   mimes.Add( "image/svg+xml" );
   return mimes;
}

uint32 SVGFormat::Version() const
{
   return 0x100;
}

String SVGFormat::Description() const
{
   return "Scalable Vector Graphics format (SVG) - read-only support.";
}

String SVGFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>Read-only support for the Scalable Vector Graphics format (SVG), based on the Qt4 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (c) 1992-2014 Digia Plc and/or its subsidiary(-ies).</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2014 Pleiades Astrophoto</p>"
   "</html>";
}

String SVGFormat::IconImageFile() const
{
   return ":/file-format/svg-format-icon.png";
}

bool SVGFormat::CanRead() const
{
   return true;
}

bool SVGFormat::CanWrite() const
{
   return false;
}

bool SVGFormat::CanStore8Bit() const
{
   return true;
}

bool SVGFormat::CanStore16Bit() const
{
   return false;
}

FileFormatImplementation* SVGFormat::Create() const
{
   return new SVGInstance( this );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF BMPFormat.cpp - Released 2014/10/29 07:34:47 UTC
