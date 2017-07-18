//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 01.00.04.0317
// ----------------------------------------------------------------------------
// BMPFormat.h - Released 2017-07-18T16:14:10Z
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __BMPFormat_h
#define __BMPFormat_h

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BMPFormat : public MetaFileFormat
{
public:

   BMPFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class GIFFormat : public MetaFileFormat
{
public:

   GIFFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class ICOFormat : public MetaFileFormat
{
public:

   ICOFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class MNGFormat : public MetaFileFormat
{
public:

   MNGFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class PBMFormat : public MetaFileFormat
{
public:

   PBMFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class PGMFormat : public MetaFileFormat
{
public:

   PGMFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class PNGFormat : public MetaFileFormat
{
public:

   PNGFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class PPMFormat : public MetaFileFormat
{
public:

   PPMFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class XBMFormat : public MetaFileFormat
{
public:

   XBMFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class XPMFormat : public MetaFileFormat
{
public:

   XPMFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class TGAFormat : public MetaFileFormat
{
public:

   TGAFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class WEBPFormat : public MetaFileFormat
{
public:

   WEBPFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

class SVGFormat : public MetaFileFormat
{
public:

   SVGFormat() = default;

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanRead() const;
   virtual bool CanWrite() const;
   virtual bool CanStore8Bit() const;
   virtual bool CanStore16Bit() const;

   virtual FileFormatImplementation* Create() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BMPFormat_h

// ----------------------------------------------------------------------------
// EOF BMPFormat.h - Released 2017-07-18T16:14:10Z
