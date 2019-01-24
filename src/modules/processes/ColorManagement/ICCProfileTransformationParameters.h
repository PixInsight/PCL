//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0359
// ----------------------------------------------------------------------------
// ICCProfileTransformationParameters.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ICCProfileTransformationParameters_h
#define __ICCProfileTransformationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/ICCProfileTransformation.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ICCTTargetProfile : public MetaString
{
public:

   ICCTTargetProfile( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICCTTargetProfile* TheICCTTargetProfileParameter;

// ----------------------------------------------------------------------------

class ICCTToDefaultProfile : public MetaBoolean
{
public:

   ICCTToDefaultProfile( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ICCTToDefaultProfile* TheICCTToDefaultProfileParameter;

// ----------------------------------------------------------------------------

class ICCTRenderingIntent : public MetaEnumeration
{
public:

   enum { Perceptual = ICCRenderingIntent::Perceptual,
          Saturation = ICCRenderingIntent::Saturation,
          RelativeColorimetric = ICCRenderingIntent::RelativeColorimetric,
          AbsoluteColorimetric = ICCRenderingIntent::AbsoluteColorimetric,
          NumberOfRenderingIntents,
          Default = RelativeColorimetric };

   ICCTRenderingIntent( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ICCTRenderingIntent* TheICCTRenderingIntentParameter;

// ----------------------------------------------------------------------------

class ICCTUseBlackPointCompensation : public MetaBoolean
{
public:

   ICCTUseBlackPointCompensation( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ICCTUseBlackPointCompensation* TheICCTUseBlackPointCompensationParameter;

// ----------------------------------------------------------------------------

class ICCTUseFloatingPointTransformation : public MetaBoolean
{
public:

   ICCTUseFloatingPointTransformation( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern ICCTUseFloatingPointTransformation* TheICCTUseFloatingPointTransformationParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ICCProfileTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF ICCProfileTransformationParameters.h - Released 2019-01-21T12:06:41Z
