//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0181
// ----------------------------------------------------------------------------
// SplitCFAParameters.h - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2018 Nikolay Volkov
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#ifndef __SplitCFAParameters_h
#define __SplitCFAParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SplitCFATargetFrames : public MetaTable
{
public:
   SplitCFATargetFrames( MetaProcess* );
   virtual IsoString Id() const;
};

class SplitCFATargetFrameEnabled : public MetaBoolean
{
public:
   SplitCFATargetFrameEnabled( MetaTable* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class SplitCFATargetFramePath : public MetaString
{
public:
   SplitCFATargetFramePath( MetaTable* );
   virtual IsoString Id() const;
};

class SplitCFATargetFrameFolder : public MetaString
{
public:
   SplitCFATargetFrameFolder( MetaTable* );
   virtual IsoString Id() const;
};

class SplitCFAOutputDir : public MetaString
{
public:
   SplitCFAOutputDir( MetaProcess* );
   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

class SplitCFAOutputTree : public MetaBoolean
{
public:
   SplitCFAOutputTree( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class SplitCFAOutputSubDirCFA : public MetaBoolean
{
public:
   SplitCFAOutputSubDirCFA( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class SplitCFAPrefix : public MetaString
{
public:
   SplitCFAPrefix( MetaProcess* );
   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

class SplitCFAPostfix : public MetaString
{
public:
   SplitCFAPostfix( MetaProcess* );
   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

class SplitCFAOverwrite : public MetaBoolean
{
public:
   SplitCFAOverwrite( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class SplitCFAOutputViewId0 : public MetaString
{
public:
   SplitCFAOutputViewId0( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

class SplitCFAOutputViewId1 : public MetaString
{
public:
   SplitCFAOutputViewId1( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

class SplitCFAOutputViewId2 : public MetaString
{
public:
   SplitCFAOutputViewId2( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

class SplitCFAOutputViewId3 : public MetaString
{
public:
   SplitCFAOutputViewId3( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};


extern SplitCFATargetFrames*       TheTargetFramesParameter;
extern SplitCFATargetFrameEnabled* TheTargetFrameEnabledParameter;
extern SplitCFATargetFramePath*    TheTargetFramePathParameter;
extern SplitCFATargetFrameFolder*  TheTargetFrameFolderParameter;

extern SplitCFAOutputDir*          TheOutputDirParameter;
extern SplitCFAOutputTree*         TheOutputTreeParameter;
extern SplitCFAOutputSubDirCFA*    TheOutputSubDirCFAParameter;
extern SplitCFAPrefix*             ThePrefixParameter;
extern SplitCFAPostfix*            ThePostfixParameter;
extern SplitCFAOverwrite*          TheOverwriteParameter;

extern SplitCFAOutputViewId0*      TheSplitCFAOutputViewId0Parameter;
extern SplitCFAOutputViewId1*      TheSplitCFAOutputViewId1Parameter;
extern SplitCFAOutputViewId2*      TheSplitCFAOutputViewId2Parameter;
extern SplitCFAOutputViewId3*      TheSplitCFAOutputViewId3Parameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SplitCFAParameters_h

// ----------------------------------------------------------------------------
// EOF SplitCFAParameters.h - Released 2018-11-23T18:45:59Z
