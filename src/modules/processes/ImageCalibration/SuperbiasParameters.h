//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0319
// ----------------------------------------------------------------------------
// SuperbiasParameters.h - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __SuperbiasParameters_h
#define __SuperbiasParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SBColumns : public MetaBoolean
{
public:

   SBColumns( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SBColumns* TheSBColumnsParameter;

// ----------------------------------------------------------------------------

class SBRows : public MetaBoolean
{
public:

   SBRows( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SBRows* TheSBRowsParameter;

// ----------------------------------------------------------------------------

class SBMedianTransform : public MetaBoolean
{
public:

   SBMedianTransform( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SBMedianTransform* TheSBMedianTransformParameter;

// ----------------------------------------------------------------------------

class SBExcludeLargeScale : public MetaBoolean
{
public:

   SBExcludeLargeScale( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SBExcludeLargeScale* TheSBExcludeLargeScaleParameter;

// ----------------------------------------------------------------------------

class SBMultiscaleLayers : public MetaInt32
{
public:

   SBMultiscaleLayers( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SBMultiscaleLayers* TheSBMultiscaleLayersParameter;

// ----------------------------------------------------------------------------

class SBTrimmingFactor : public MetaFloat
{
public:

   SBTrimmingFactor( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern SBTrimmingFactor* TheSBTrimmingFactorParameter;

// ----------------------------------------------------------------------------

class SBSuperbiasViewId : public MetaString
{
public:

   SBSuperbiasViewId( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern SBSuperbiasViewId* TheSBSuperbiasViewIdParameter;

// ----------------------------------------------------------------------------

class SBSuperbiasViewId1 : public MetaString
{
public:

   SBSuperbiasViewId1( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern SBSuperbiasViewId1* TheSBSuperbiasViewId1Parameter;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SuperbiasParameters_h

// ----------------------------------------------------------------------------
// EOF SuperbiasParameters.h - Released 2017-07-09T18:07:33Z
