//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0345
// ----------------------------------------------------------------------------
// SuperbiasProcess.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SuperbiasParameters.h"
#include "SuperbiasProcess.h"
#include "SuperbiasInstance.h"
#include "SuperbiasInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#include "SuperbiasIcon.xpm"

// ----------------------------------------------------------------------------

SuperbiasProcess* TheSuperbiasProcess = nullptr;

// ----------------------------------------------------------------------------

SuperbiasProcess::SuperbiasProcess()
{
   TheSuperbiasProcess = this;

   // Instantiate process parameters
   new SBColumns( this );
   new SBRows( this );
   new SBMedianTransform( this );
   new SBExcludeLargeScale( this );
   new SBMultiscaleLayers( this );
   new SBTrimmingFactor( this );
   new SBSuperbiasViewId( this );
   new SBSuperbiasViewId1( this );
}

// ----------------------------------------------------------------------------

IsoString SuperbiasProcess::Id() const
{
   return "Superbias";
}

// ----------------------------------------------------------------------------

IsoString SuperbiasProcess::Category() const
{
   return "ImageCalibration,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 SuperbiasProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String SuperbiasProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** SuperbiasProcess::IconImageXPM() const
{
   return SuperbiasIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* SuperbiasProcess::DefaultInterface() const
{
   return TheSuperbiasInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SuperbiasProcess::Create() const
{
   return new SuperbiasInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SuperbiasProcess::Clone( const ProcessImplementation& p ) const
{
   const SuperbiasInstance* instPtr = dynamic_cast<const SuperbiasInstance*>( &p );
   return (instPtr != nullptr) ? new SuperbiasInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SuperbiasProcess.cpp - Released 2018-11-01T11:07:21Z
