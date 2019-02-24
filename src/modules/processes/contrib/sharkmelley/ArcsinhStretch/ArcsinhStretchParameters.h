//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 01.00.00.0142
// ----------------------------------------------------------------------------
// ArcsinhStretchParameters.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2018 Mark Shelley
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

#ifndef __ArcsinhStretchs_h
#define __ArcsinhStretchs_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ArcsinhStretch : public MetaFloat
{
public:

   ArcsinhStretch( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ArcsinhStretch* TheArcsinhStretchParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchBlackPoint : public MetaFloat
{
public:

   ArcsinhStretchBlackPoint(MetaProcess*);

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ArcsinhStretchBlackPoint* TheArcsinhStretchBlackPointParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchProtectHighlights : public MetaBoolean
{
public:

   ArcsinhStretchProtectHighlights(MetaProcess*);

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ArcsinhStretchProtectHighlights* TheArcsinhStretchProtectHighlightsParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchUseRgbws : public MetaBoolean
{
public:

   ArcsinhStretchUseRgbws(MetaProcess*);

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ArcsinhStretchUseRgbws* TheArcsinhStretchUseRgbwsParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchPreviewClipped : public MetaBoolean
{
public:

   ArcsinhStretchPreviewClipped(MetaProcess*);

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ArcsinhStretchPreviewClipped* TheArcsinhStretchPreviewClippedParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ArcsinhStretchs_h

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchParameters.h - Released 2019-01-21T12:06:42Z
