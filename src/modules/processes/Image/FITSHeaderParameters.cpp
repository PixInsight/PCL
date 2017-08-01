//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0402
// ----------------------------------------------------------------------------
// FITSHeaderParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "FITSHeaderParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FITSKeywordTable*   TheFITSKeywordTableParameter = 0;
FITSKeywordName*    TheFITSKeywordNameParameter = 0;
FITSKeywordValue*   TheFITSKeywordValueParameter = 0;
FITSKeywordComment* TheFITSKeywordCommentParameter = 0;

// ----------------------------------------------------------------------------

FITSKeywordTable::FITSKeywordTable( MetaProcess* P ) : MetaTable( P )
{
   TheFITSKeywordTableParameter = this;
}

IsoString FITSKeywordTable::Id() const
{
   return "keywords";
}

// ----------------------------------------------------------------------------

FITSKeywordName::FITSKeywordName( MetaTable* T ) : MetaString( T )
{
   TheFITSKeywordNameParameter = this;
}

IsoString FITSKeywordName::Id() const
{
   return "name";
}

// ----------------------------------------------------------------------------

FITSKeywordValue::FITSKeywordValue( MetaTable* T ) : MetaString( T )
{
   TheFITSKeywordValueParameter = this;
}

IsoString FITSKeywordValue::Id() const
{
   return "value";
}

// ----------------------------------------------------------------------------

FITSKeywordComment::FITSKeywordComment( MetaTable* T ) : MetaString( T )
{
   TheFITSKeywordCommentParameter = this;
}

IsoString FITSKeywordComment::Id() const
{
   return "comment";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSHeaderParameters.cpp - Released 2017-08-01T14:26:58Z
