// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard SplitCFA Process Module Version 01.00.05.0037
// ****************************************************************************
// SplitCFAParameters.cpp - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2014 Nikolay Volkov
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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

#include "SplitCFAParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SplitCFATargetFrames*       TheTargetFramesParameter = 0;
SplitCFATargetFrameEnabled* TheTargetFrameEnabledParameter = 0;
SplitCFATargetFramePath*    TheTargetFramePathParameter = 0;
SplitCFATargetFrameFolder*  TheTargetFrameFolderParameter = 0;

SplitCFAOutputTree*         TheOutputTreeParameter = 0;
SplitCFAOutputSubDirCFA*    TheOutputSubDirCFAParameter = 0;
SplitCFAOutputDir*          TheOutputDirParameter = 0;
SplitCFAPrefix*             ThePrefixParameter = 0;
SplitCFAPostfix*            ThePostfixParameter = 0;
SplitCFAOverwrite*          TheOverwriteParameter = 0;

// ----------------------------------------------------------------------------

SplitCFATargetFrames::SplitCFATargetFrames( MetaProcess* P ) : MetaTable( P )
{
   TheTargetFramesParameter = this;
}

IsoString SplitCFATargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

SplitCFATargetFrameEnabled::SplitCFATargetFrameEnabled( MetaTable* T ) : MetaBoolean( T ) { TheTargetFrameEnabledParameter = this; }
IsoString SplitCFATargetFrameEnabled::Id() const { return "enabled"; }
bool SplitCFATargetFrameEnabled::DefaultValue() const { return true; }

// ----------------------------------------------------------------------------

SplitCFATargetFramePath::SplitCFATargetFramePath( MetaTable* T ) : MetaString( T ) { TheTargetFramePathParameter = this; }
IsoString SplitCFATargetFramePath::Id() const { return "path"; }

// ----------------------------------------------------------------------------

SplitCFATargetFrameFolder::SplitCFATargetFrameFolder( MetaTable* T ) : MetaString( T ) { TheTargetFrameFolderParameter = this; }
IsoString SplitCFATargetFrameFolder::Id() const { return "folder"; }

// ----------------------------------------------------------------------------

SplitCFAOutputTree::SplitCFAOutputTree( MetaProcess* P ) : MetaBoolean( P ) { TheOutputTreeParameter = this; }
IsoString SplitCFAOutputTree::Id() const { return "outputTree"; }
bool SplitCFAOutputTree::DefaultValue() const { return true; }

// ----------------------------------------------------------------------------

SplitCFAOutputSubDirCFA::SplitCFAOutputSubDirCFA( MetaProcess* P ) : MetaBoolean( P ) { TheOutputSubDirCFAParameter = this; }
IsoString SplitCFAOutputSubDirCFA::Id() const { return "outputSubDirCFA"; }
bool SplitCFAOutputSubDirCFA::DefaultValue() const { return true; }
// ----------------------------------------------------------------------------

SplitCFAOutputDir::SplitCFAOutputDir( MetaProcess* P ) : MetaString( P ) { TheOutputDirParameter = this; }
IsoString SplitCFAOutputDir::Id() const { return "outputDir"; }
String SplitCFAOutputDir::DefaultValue() const { return ""; }

// ----------------------------------------------------------------------------

SplitCFAPrefix::SplitCFAPrefix( MetaProcess* P ) : MetaString( P ) { ThePrefixParameter = this; }
IsoString SplitCFAPrefix::Id() const { return "prefix"; }
String SplitCFAPrefix::DefaultValue() const { return ""; }

// ----------------------------------------------------------------------------

SplitCFAPostfix::SplitCFAPostfix( MetaProcess* P ) : MetaString( P ) { ThePostfixParameter = this; }
IsoString SplitCFAPostfix::Id() const { return "postfix"; }
String SplitCFAPostfix::DefaultValue() const { return "_CFA"; }

// ----------------------------------------------------------------------------

SplitCFAOverwrite::SplitCFAOverwrite( MetaProcess* P ) : MetaBoolean( P ) { TheOverwriteParameter = this; }
IsoString SplitCFAOverwrite::Id() const { return "overwrite"; }
bool SplitCFAOverwrite::DefaultValue() const { return false; }

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF SplitCFAParameters.cpp - Released 2014/11/14 17:19:24 UTC
