//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0352
// ----------------------------------------------------------------------------
// CreateAlphaChannelsParameters.h - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __CreateAlphaChannelsParameters_h
#define __CreateAlphaChannelsParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CAFromImage : public MetaBoolean
{
public:

   CAFromImage( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern CAFromImage* TheCAFromImageParameter;

// ----------------------------------------------------------------------------

class CASourceImageIdentifier : public MetaString
{
public:

   CASourceImageIdentifier( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;   // 0 = automatic
   virtual String AllowedCharacters() const;
};

extern CASourceImageIdentifier* TheCASourceImageIdentifierParameter;

// ----------------------------------------------------------------------------

class CAInvertSourceImage : public MetaBoolean
{
public:

   CAInvertSourceImage( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern CAInvertSourceImage* TheCAInvertSourceImageParameter;

// ----------------------------------------------------------------------------

class CACloseSourceImage : public MetaBoolean
{
public:

   CACloseSourceImage( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern CACloseSourceImage* TheCACloseSourceImageParameter;

// ----------------------------------------------------------------------------

class CATransparency : public MetaDouble
{
public:

   CATransparency( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern CATransparency* TheCATransparencyParameter;

// ----------------------------------------------------------------------------

class CAReplaceExistingChannels : public MetaBoolean
{
public:

   CAReplaceExistingChannels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern CAReplaceExistingChannels* TheCAReplaceExistingChannelsParameter;

// ----------------------------------------------------------------------------

class CAChannelCount : public MetaInt32
{
public:

   CAChannelCount( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern CAChannelCount* TheCAChannelCountParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CreateAlphaChannelsParameters_h

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsParameters.h - Released 2016/02/21 20:22:43 UTC
