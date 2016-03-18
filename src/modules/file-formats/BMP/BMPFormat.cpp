//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 01.00.03.0276
// ----------------------------------------------------------------------------
// BMPFormat.cpp - Released 2016/02/21 20:22:34 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
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
   return StringList() << ".bmp";
}

IsoStringList BMPFormat::MimeTypes() const
{
   return IsoStringList() << "image/bmp"
                          << "image/x-bmp"
                          << "image/x-bitmap"
                          << "image/x-xbitmap"
                          << "image/x-win-bitmap"
                          << "image/x-windows-bmp"
                          << "image/ms-bmp"
                          << "image/x-ms-bmp"
                          << "application/bmp"
                          << "application/x-bmp"
                          << "application/x-win-bitmap";
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

   "<p>Support for the Windows Bitmap format (BMP), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".gif";
}

IsoStringList GIFFormat::MimeTypes() const
{
   // RFC2045,RFC2046
   return IsoStringList() << "image/gif";
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

   "<p>Read-only support for the Graphics Interchange Format (GIF), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".ico";
}

IsoStringList ICOFormat::MimeTypes() const
{
   return IsoStringList() << "image/ico" << "image/x-icon" << "application/ico" << "application/x-ico";
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

   "<p>Support for the Windows Icon format (ICO), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".mng";
}

IsoStringList MNGFormat::MimeTypes() const
{
   return IsoStringList() << "image/mng" << "video/mng" << "video/x-mng";
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

   "<p>Read-only support for the Multiple Network Graphics format (MNG), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".pbm";
}

IsoStringList PBMFormat::MimeTypes() const
{
   return IsoStringList() << "image/pbm" << "image/x-pbm" << "image/portable bitmap" << "image/x-portable-bitmap";
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

   "<p>Read-only support for the Portable Bitmap format (PBM), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".pgm";
}

IsoStringList PGMFormat::MimeTypes() const
{
   return IsoStringList() << "image/pgm" << "image/x-pgm" << "image/portable graymap" << "image/x-portable-graymap";
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

   "<p>Read-only support for the Portable Graymap format (PGM), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".png";
}

IsoStringList PNGFormat::MimeTypes() const
{
   return IsoStringList() << "image/png" << "application/png" << "application/x-png";
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

   "<p>Basic support for the Portable Network Graphics format (PNG), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".ppm";
}

IsoStringList PPMFormat::MimeTypes() const
{
   return IsoStringList() << "image/ppm"
                          << "image/x-portable-pixmap"
                          << "image/x-p"
                          << "image/x-ppm"
                          << "application/ppm"
                          << "application/x-ppm";
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

   "<p>Support for the Portable Pixmap format (PPM), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".xbm";
}

IsoStringList XBMFormat::MimeTypes() const
{
   return IsoStringList() << "image/xbm" << "image/x-xbitmap";
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

   "<p>Support for the X11 Bitmap format (XBM), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".xpm";
}

IsoStringList XPMFormat::MimeTypes() const
{
   return IsoStringList() << "image/xpm" << "image/x-xpm" << "image/x-xpixmap";
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

   "<p>Support for the X11 Pixmap format (XPM), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".tga" << ".tpic";
}

IsoStringList TGAFormat::MimeTypes() const
{
   return IsoStringList() << "image/tga"
                          << "image/x-tga"
                          << "image/targa"
                          << "image/x-targa"
                          << "application/tga"
                          << "application/x-tga"
                          << "application/x-targa";
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

   "<p>Read-only support for the Truevision TGA format (TARGA), based on the Qt5 library.</p>"

   "<p>RLE compressed TGA files are not supported by this reader. Only uncompressed images can be read.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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
   return StringList() << ".svg";
}

IsoStringList SVGFormat::MimeTypes() const
{
   return IsoStringList() << "image/svg" << "image/svg+xml";
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

   "<p>Read-only support for the Scalable Vector Graphics format (SVG), based on the Qt5 library.</p>"

   "<p>Qt Library:<br/>"
   "Copyright (C) 2014-2015 Digia Plc and/or its subsidiary(-ies)</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2015 Pleiades Astrophoto</p>"
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

// ----------------------------------------------------------------------------
// EOF BMPFormat.cpp - Released 2016/02/21 20:22:34 UTC
