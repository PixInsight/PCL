//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0332
// ----------------------------------------------------------------------------
// LocalNormalizationProcess.cpp - Released 2017-08-01T14:26:58Z
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

#include "LocalNormalizationProcess.h"
#include "LocalNormalizationParameters.h"
#include "LocalNormalizationInstance.h"
#include "LocalNormalizationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#include "LocalNormalizationIcon.xpm"

// ----------------------------------------------------------------------------

LocalNormalizationProcess* TheLocalNormalizationProcess = nullptr;

// ----------------------------------------------------------------------------

LocalNormalizationProcess::LocalNormalizationProcess() : MetaProcess()
{
   TheLocalNormalizationProcess = this;

   new LNScale( this );
   new LNRejection( this );
   new LNBackgroundRejectionLimit( this );
   new LNReferenceRejectionThreshold( this );
   new LNTargetRejectionThreshold( this );
   new LNHotPixelFilterRadius( this );
   new LNNoiseReductionFilterRadius( this );
   new LNReferencePathOrViewId( this );
   new LNReferenceIsView( this );
   new LNTargetItems( this );
   new LNTargetEnabled( TheLNTargetItemsParameter );
   new LNTargetImage( TheLNTargetItemsParameter );
   new LNInputHints( this );
   new LNOutputHints( this );
   new LNGenerateNormalizedImages( this );
   new LNGenerateNormalizationData( this );
   new LNShowBackgroundModels( this );
   new LNShowRejectionMaps( this );
   new LNPlotNormalizationFunctions( this );
   new LNNoGUIMessages( this );
   new LNOutputDirectory( this );
   new LNOutputExtension( this );
   new LNOutputPrefix( this );
   new LNOutputPostfix( this );
   new LNOverwriteExistingFiles( this );
   new LNOnError( this );
   new LNUseFileThreads( this );
   new LNFileThreadOverload( this );
   new LNMaxFileReadThreads( this );
   new LNMaxFileWriteThreads( this );
   new LNGraphSize( this );
   new LNGraphTextSize( this );
   new LNGraphTitleSize( this );
   new LNGraphTransparent( this );
   new LNGraphOutputDirectory( this );
}

// ----------------------------------------------------------------------------

IsoString LocalNormalizationProcess::Id() const
{
   return "LocalNormalization";
}

// ----------------------------------------------------------------------------

IsoString LocalNormalizationProcess::Category() const
{
   return "ImageCalibration,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 LocalNormalizationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String LocalNormalizationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** LocalNormalizationProcess::IconImageXPM() const
{
   return LocalNormalizationIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* LocalNormalizationProcess::DefaultInterface() const
{
   return TheLocalNormalizationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* LocalNormalizationProcess::Create() const
{
   return new LocalNormalizationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* LocalNormalizationProcess::Clone( const ProcessImplementation& p ) const
{
   const LocalNormalizationInstance* instPtr = dynamic_cast<const LocalNormalizationInstance*>( &p );
   return (instPtr != nullptr) ? new LocalNormalizationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationProcess.cpp - Released 2017-08-01T14:26:58Z
