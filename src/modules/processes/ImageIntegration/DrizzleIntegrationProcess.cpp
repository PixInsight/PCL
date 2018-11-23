//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.16.01.0462
// ----------------------------------------------------------------------------
// DrizzleIntegrationProcess.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "DrizzleIntegrationParameters.h"
#include "DrizzleIntegrationProcess.h"
#include "DrizzleIntegrationInstance.h"
#include "DrizzleIntegrationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DrizzleIntegrationProcess* TheDrizzleIntegrationProcess = nullptr;

// ----------------------------------------------------------------------------

#include "DrizzleIntegrationIcon.xpm"

// ----------------------------------------------------------------------------

DrizzleIntegrationProcess::DrizzleIntegrationProcess() : MetaProcess()
{
   TheDrizzleIntegrationProcess = this;

   new DZInputData( this );
   new DZItemEnabled( TheDZInputDataParameter );
   new DZItemPath( TheDZInputDataParameter );
   new DZLocalNormalizationDataPath( TheDZInputDataParameter );
   new DZInputHints( this );
   new DZInputDirectory( this );
   new DZScale( this );
   new DZDropShrink( this );
   new DZKernelFunction( this );
   new DZKernelGridSize( this );
   new DZOriginX( this );
   new DZOriginY( this );
   new DZEnableCFA( this );
   new DZCFAPattern( this );
   new DZEnableRejection( this );
   new DZEnableImageWeighting( this );
   new DZEnableSurfaceSplines( this );
   new DZEnableLocalNormalization( this );
   new DZUseROI( this );
   new DZROIX0( this );
   new DZROIY0( this );
   new DZROIX1( this );
   new DZROIY1( this );
   new DZClosePreviousImages( this );
   new DZNoGUIMessages( this );
   new DZOnError( this );

   new DZIntegrationImageId( this );
   new DZWeightImageId( this );
   new DZNumberOfChannels( this );
   new DZOutputPixels( this );
   new DZIntegratedPixels( this );
   new DZTotalRejectedLowRK( this );
   new DZTotalRejectedLowG( this );
   new DZTotalRejectedLowB( this );
   new DZTotalRejectedHighRK( this );
   new DZTotalRejectedHighG( this );
   new DZTotalRejectedHighB( this );
   new DZOutputData( this );
   new DZImageData( this );
   new DZImageFilePath( TheDZImageDataParameter );
   new DZImageWeightRK( TheDZImageDataParameter );
   new DZImageWeightG( TheDZImageDataParameter );
   new DZImageWeightB( TheDZImageDataParameter );
   new DZImageLocationRK( TheDZImageDataParameter );
   new DZImageLocationG( TheDZImageDataParameter );
   new DZImageLocationB( TheDZImageDataParameter );
   new DZImageReferenceLocationRK( TheDZImageDataParameter );
   new DZImageReferenceLocationG( TheDZImageDataParameter );
   new DZImageReferenceLocationB( TheDZImageDataParameter );
   new DZImageScaleRK( TheDZImageDataParameter );
   new DZImageScaleG( TheDZImageDataParameter );
   new DZImageScaleB( TheDZImageDataParameter );
   new DZImageRejectedLowRK( TheDZImageDataParameter );
   new DZImageRejectedLowG( TheDZImageDataParameter );
   new DZImageRejectedLowB( TheDZImageDataParameter );
   new DZImageRejectedHighRK( TheDZImageDataParameter );
   new DZImageRejectedHighG( TheDZImageDataParameter );
   new DZImageRejectedHighB( TheDZImageDataParameter );
   new DZImageOutputData( TheDZImageDataParameter );
}

// ----------------------------------------------------------------------------

IsoString DrizzleIntegrationProcess::Id() const
{
   return "DrizzleIntegration";
}

// ----------------------------------------------------------------------------

IsoString DrizzleIntegrationProcess::Category() const
{
   return "ImageIntegration,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 DrizzleIntegrationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String DrizzleIntegrationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** DrizzleIntegrationProcess::IconImageXPM() const
{
   return DrizzleIntegrationIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* DrizzleIntegrationProcess::DefaultInterface() const
{
   return TheDrizzleIntegrationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* DrizzleIntegrationProcess::Create() const
{
   return new DrizzleIntegrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* DrizzleIntegrationProcess::Clone( const ProcessImplementation& p ) const
{
   const DrizzleIntegrationInstance* instPtr = dynamic_cast<const DrizzleIntegrationInstance*>( &p );
   return (instPtr != nullptr) ? new DrizzleIntegrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationProcess.cpp - Released 2018-11-23T18:45:58Z
