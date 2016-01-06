//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 01.02.05.0145
// ----------------------------------------------------------------------------
// CosmeticCorrectionProcess.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "CosmeticCorrectionProcess.h"
#include "CosmeticCorrectionParameters.h"
#include "CosmeticCorrectionInstance.h"
#include "CosmeticCorrectionInterface.h"
#include "CosmeticCorrectionModule.h" // for CosmeticCorrectionModule::ReadableVersion()

namespace pcl
{

// ----------------------------------------------------------------------------

CosmeticCorrectionProcess* TheCosmeticCorrectionProcess = 0;

// ----------------------------------------------------------------------------

//#include "CosmeticCorrectionIcon.xpm"

// ----------------------------------------------------------------------------

CosmeticCorrectionProcess::CosmeticCorrectionProcess() : MetaProcess()
{
   TheCosmeticCorrectionProcess = this;

   // Instantiate process parameters
   new CCTargetFrames( this );
   new CCTargetFrameEnabled( TheTargetFrames );
   new CCTargetFramePath( TheTargetFrames );
   new CCMasterDarkPath( this );
   new CCOutputDir( this );
   new CCOutputExtension( this );
   new CCPrefix( this );
   new CCPostfix( this );
   new CCOverwrite( this );

   new CCAmount( this );
   new CCCFA( this );

   new CCUseMasterDark( this );
   new CCHotDarkCheck( this );
   new CCHotLevel( this );
   new CCColdDarkCheck( this );
   new CCColdLevel( this );

   new CCUseAutoDetect( this );
   new CCHotAutoCheck( this );
   new CCHotAutoValue( this );
   new CCColdAutoCheck( this );
   new CCColdAutoValue( this );

   new CCUseDefectList( this );
   new CCDefects( this );
   new CCDefectEnabled( TheDefects );
   new CCDefectIsRow( TheDefects );
   new CCDefectAddress( TheDefects );
   new CCDefectIsRange( TheDefects );
   new CCDefectBegin( TheDefects );
   new CCDefectEnd( TheDefects );

}

IsoString CosmeticCorrectionProcess::Id() const
{
   return "CosmeticCorrection";
}

IsoString CosmeticCorrectionProcess::Category() const
{
   return "ImageCalibration,Preprocessing";
}

uint32 CosmeticCorrectionProcess::Version() const
{
   return 0x100;
}

String CosmeticCorrectionProcess::Description() const
{
   return  CosmeticCorrectionModule::ReadableVersion() +
         "<html><br/>The CosmeticCorrection tool replaces defective pixels (hot and cold pixels)"
         "with averaged values from the appropriate neighbor pixels.<br/> "
         "The script requires a map of defective pixels or a master dark frame.</html>";
}

const char** CosmeticCorrectionProcess::IconImageXPM() const
{
   return 0; //CosmeticCorrectionIcon_XPM;
}

ProcessInterface* CosmeticCorrectionProcess::DefaultInterface() const
{
   return TheCosmeticCorrectionInterface;
}

ProcessImplementation* CosmeticCorrectionProcess::Create() const
{
   return new CosmeticCorrectionInstance( this );
}

ProcessImplementation* CosmeticCorrectionProcess::Clone( const ProcessImplementation& p ) const
{
   const CosmeticCorrectionInstance* instPtr = dynamic_cast<const CosmeticCorrectionInstance*>( &p );
   return (instPtr != 0) ? new CosmeticCorrectionInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionProcess.cpp - Released 2015/12/18 08:55:08 UTC
