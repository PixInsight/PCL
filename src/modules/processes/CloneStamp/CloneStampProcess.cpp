//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 01.00.02.0328
// ----------------------------------------------------------------------------
// CloneStampProcess.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#include "CloneStampProcess.h"
#include "CloneStampParameters.h"
#include "CloneStampInstance.h"
#include "CloneStampInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CloneStampProcess* TheCloneStampProcess = 0;

// ----------------------------------------------------------------------------

#include "CloneStampIcon.xpm"

// ----------------------------------------------------------------------------

CloneStampProcess::CloneStampProcess() : MetaProcess()
{
   TheCloneStampProcess = this;

   // Instantiate process parameters

   new CSClonerTable( this );
   new CSActionIndex( TheCSClonerTableParameter );
   new CSTargetX( TheCSClonerTableParameter );
   new CSTargetY( TheCSClonerTableParameter );

   new CSActionTable( this );
   new CSSourceIdentifier( TheCSActionTableParameter );
   new CSSourceWidth( TheCSActionTableParameter );
   new CSSourceHeight( TheCSActionTableParameter );
   new CSSourceX( TheCSActionTableParameter );
   new CSSourceY( TheCSActionTableParameter );
   new CSRadius( TheCSActionTableParameter );
   new CSSoftness( TheCSActionTableParameter );
   new CSOpacity( TheCSActionTableParameter );

   new CSWidth( this );
   new CSHeight( this );
   new CSColor( this );
   new CSBoundsColor( this );
}

IsoString CloneStampProcess::Id() const
{
   return "CloneStamp";
}

IsoString CloneStampProcess::Category() const
{
   return "Painting";
}

uint32 CloneStampProcess::Version() const
{
   return 0x100;
}

String CloneStampProcess::Description() const
{
   return "";
}

const char** CloneStampProcess::IconImageXPM() const
{
   return CloneStampIcon_XPM;
}

ProcessInterface* CloneStampProcess::DefaultInterface() const
{
   return TheCloneStampInterface;
}

ProcessImplementation* CloneStampProcess::Create() const
{
   return new CloneStampInstance( this );
}

ProcessImplementation* CloneStampProcess::Clone( const ProcessImplementation& p ) const
{
   const CloneStampInstance* instPtr = dynamic_cast<const CloneStampInstance*>( &p );
   return (instPtr != 0) ? new CloneStampInstance( *instPtr ) : 0;
}

ProcessImplementation* CloneStampProcess::TestClone( const ProcessImplementation& p ) const
{
   const CloneStampInstance* instPtr = dynamic_cast<const CloneStampInstance*>( &p );
   if ( instPtr != 0 )
   {
      CloneStampInstance* i = new CloneStampInstance( TheCloneStampProcess );
      i->TestAssign( *instPtr );
      return i;
   }
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CloneStampProcess.cpp - Released 2017-07-18T16:14:18Z
