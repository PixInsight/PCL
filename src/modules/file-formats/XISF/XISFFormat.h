// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard XISF File Format Module Version 01.00.00.0023
// ****************************************************************************
// XISFFormat.h - Released 2014/11/30 10:38:10 UTC
// ****************************************************************************
// This file is part of the standard XISF PixInsight module.
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

#ifndef __XISFFormat_h
#define __XISFFormat_h

#include "XISF.h"

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFFormat : public MetaFileFormat
{
public:

   XISFFormat();

   virtual IsoString Name() const;

   virtual StringList FileExtensions() const;
   virtual IsoStringList MimeTypes() const;

   virtual uint32 Version() const;

   virtual String Description() const;
   virtual String Implementation() const;

   virtual String IconImageFile() const;

   virtual bool CanReadIncrementally() const;
   virtual bool CanWriteIncrementally() const;
   virtual bool CanStore32Bit() const;
   virtual bool CanStore64Bit() const;
   virtual bool CanStoreFloat() const;
   virtual bool CanStoreDouble() const;
   virtual bool CanStoreComplex() const;
   virtual bool CanStoreDComplex() const;
   virtual bool CanStoreResolution() const;
   virtual bool CanStoreKeywords() const;
   virtual bool CanStoreICCProfiles() const;
   virtual bool CanStoreMetadata() const;
   virtual bool CanStoreThumbnails() const;
   virtual bool CanStoreProperties() const;
   virtual bool SupportsCompression() const;
   virtual bool SupportsMultipleImages() const;
   virtual bool CanEditPreferences() const;
   virtual bool UsesFormatSpecificData() const;

   virtual FileFormatImplementation* Create() const;

   virtual bool EditPreferences() const;

   // XISF format-specific data

   virtual bool ValidateFormatSpecificData( const void* data ) const;
   virtual void DisposeFormatSpecificData( void* data ) const;

   struct FormatOptions
   {
      XISFOptions options;

      FormatOptions();
      FormatOptions( const FormatOptions& ) = default;

      static FormatOptions* FromGenericDataBlock( const void* );

   private:

      uint32 signature;
   };

   static XISFOptions DefaultOptions();

   // Overridden embedding options.
   struct EmbeddingOverrides
   {
      bool overrideICCProfileEmbedding;
      bool embedICCProfiles;
      bool overrideMetadataEmbedding;
      bool embedMetadata;
      bool overrideThumbnailEmbedding;
      bool embedThumbnails;
      bool overridePropertyEmbedding;
      bool embedProperties;

      EmbeddingOverrides() :
      overrideICCProfileEmbedding( false ), embedICCProfiles( false ),
      overrideMetadataEmbedding( false ), embedMetadata( false ),
      overrideThumbnailEmbedding( false ), embedThumbnails( false ),
      overridePropertyEmbedding( false ), embedProperties( true )
      {
      }
   };

   static EmbeddingOverrides DefaultEmbeddingOverrides();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __XISFFormat_h

// ****************************************************************************
// EOF XISFFormat.h - Released 2014/11/30 10:38:10 UTC
