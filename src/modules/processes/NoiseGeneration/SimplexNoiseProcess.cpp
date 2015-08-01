//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 01.00.02.0227
// ----------------------------------------------------------------------------
// SimplexNoiseProcess.cpp - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SimplexNoiseProcess.h"
#include "SimplexNoiseParameters.h"
#include "SimplexNoiseInstance.h"
#include "SimplexNoiseInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SimplexNoiseProcess* TheSimplexNoiseProcess = 0;

// ----------------------------------------------------------------------------

#include "SimplexNoiseIcon.xpm"

// ----------------------------------------------------------------------------

SimplexNoiseProcess::SimplexNoiseProcess() : MetaProcess()
{
   TheSimplexNoiseProcess = this;

   // Instantiate process parameters
   new SNAmount( this );
   new SNScale( this );
   new SNOffsetX( this );
   new SNOffsetY( this );
   new SNOperator( this );
}

// ----------------------------------------------------------------------------

IsoString SimplexNoiseProcess::Id() const
{
   return "SimplexNoise";
}

// ----------------------------------------------------------------------------

IsoString SimplexNoiseProcess::Category() const
{
   return "NoiseGeneration";
}

// ----------------------------------------------------------------------------

uint32 SimplexNoiseProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String SimplexNoiseProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** SimplexNoiseProcess::IconImageXPM() const
{
   return SimplexNoiseIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* SimplexNoiseProcess::DefaultInterface() const
{
   return TheSimplexNoiseInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* SimplexNoiseProcess::Create() const
{
   return new SimplexNoiseInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SimplexNoiseProcess::Clone( const ProcessImplementation& p ) const
{
   const SimplexNoiseInstance* instPtr = dynamic_cast<const SimplexNoiseInstance*>( &p );
   return (instPtr != 0) ? new SimplexNoiseInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SimplexNoiseProcess.cpp - Released 2015/07/31 11:49:48 UTC
