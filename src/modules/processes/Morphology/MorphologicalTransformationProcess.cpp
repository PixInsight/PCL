//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0241
// ----------------------------------------------------------------------------
// MorphologicalTransformationProcess.cpp - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
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

#include "MorphologicalTransformationProcess.h"
#include "MorphologicalTransformationParameters.h"
#include "MorphologicalTransformationInstance.h"
#include "MorphologicalTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

MorphologicalTransformationProcess* TheMorphologicalTransformationProcess = 0;

// ----------------------------------------------------------------------------

#include "MorphologicalTransformationIcon.xpm"

// ----------------------------------------------------------------------------

MorphologicalTransformationProcess::MorphologicalTransformationProcess() : MetaProcess()
{
   TheMorphologicalTransformationProcess = this;

   // Instantiate process parameters

   new MorphologicalOp( this );
   new InterlacingDistance( this );
   new LowThreshold( this );
   new HighThreshold( this );
   new NumberOfIterations( this );
   new Amount( this );
   new SelectionPoint( this );
   new StructureName( this );
   new StructureSize( this );
   new StructureWayTable( this );
   new StructureWayMask( TheStructureWayTableParameter );
}

// ----------------------------------------------------------------------------

IsoString MorphologicalTransformationProcess::Id() const
{
   return "MorphologicalTransformation";
}

// ----------------------------------------------------------------------------

IsoString MorphologicalTransformationProcess::Category() const
{
   return "Morphology";
}

// ----------------------------------------------------------------------------

uint32 MorphologicalTransformationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String MorphologicalTransformationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** MorphologicalTransformationProcess::IconImageXPM() const
{
   return MorphologicalTransformationIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* MorphologicalTransformationProcess::DefaultInterface() const
{
   return TheMorphologicalTransformationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* MorphologicalTransformationProcess::Create() const
{
   return new MorphologicalTransformationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* MorphologicalTransformationProcess::Clone( const ProcessImplementation& p ) const
{
   const MorphologicalTransformationInstance* instPtr = dynamic_cast<const MorphologicalTransformationInstance*>( &p );
   return (instPtr != 0) ? new MorphologicalTransformationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationProcess.cpp - Released 2015/10/08 11:24:40 UTC
