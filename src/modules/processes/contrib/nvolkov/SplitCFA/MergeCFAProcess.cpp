//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0116
// ----------------------------------------------------------------------------
// MergeCFAProcess.cpp - Released 2016/05/12 12:53:00 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2016 Nikolay Volkov
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L.
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

#include "MergeCFAProcess.h"
#include "MergeCFAParameters.h"
#include "MergeCFAInterface.h"
#include "SplitCFAModule.h" // for SplitCFAModule::ReadableVersion();

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "MergeCFAIcon.xpm"

// ----------------------------------------------------------------------------

MergeCFAProcess* TheMergeCFAProcess = nullptr;

// ----------------------------------------------------------------------------

MergeCFAProcess::MergeCFAProcess() : MetaProcess()
{
   TheMergeCFAProcess = this;

   // Instantiate process parameters
   new MergeCFASourceImage0( this );
   new MergeCFASourceImage1( this );
   new MergeCFASourceImage2( this );
   new MergeCFASourceImage3( this );
   new MergeCFAOutputViewId( this );
}

// ----------------------------------------------------------------------------

IsoString MergeCFAProcess::Id() const
{
   return "MergeCFA";
}

// ----------------------------------------------------------------------------

IsoString MergeCFAProcess::Category() const
{
   return "ColorSpaces,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 MergeCFAProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String MergeCFAProcess::Description() const
{
   return SplitCFAModule::ReadableVersion() +
           "<html><p>The module combine one CFA Bayer( [ R ] [ G ] [ G ] [ B ] ) from sub images ( names [ 0 ] [ 1 ] [ 2 ] [ 3 ] ).<br/>"
           "<html><p>For more detailed information, please refer to the Release Information board on PixInsight Forum:<br/>"
           "http://pixinsight.com/forum/index.php?topic=6249.0</html>";
}

// ----------------------------------------------------------------------------

const char** MergeCFAProcess::IconImageXPM() const
{
   return MergeCFAIcon_XPM; //---> put a nice icon here
}
// ----------------------------------------------------------------------------

ProcessInterface* MergeCFAProcess::DefaultInterface() const
{
   return TheMergeCFAInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* MergeCFAProcess::Create() const
{
   return new MergeCFAInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* MergeCFAProcess::Clone( const ProcessImplementation& p ) const
{
   const MergeCFAInstance* instPtr = dynamic_cast<const MergeCFAInstance*>( &p );
   return (instPtr != nullptr) ? new MergeCFAInstance( *instPtr ) : nullptr;
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MergeCFAProcess.cpp - Released 2016/05/12 12:53:00 UTC
