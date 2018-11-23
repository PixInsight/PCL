//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0420
// ----------------------------------------------------------------------------
// RescaleActions.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "RescaleActions.h"
#include "RescaleInstance.h"
#include "RescaleProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#include "RescaleRGBActionIcon.xpm"
#include "RescaleLumActionIcon.xpm"

// ----------------------------------------------------------------------------

RescaleRGBAction::RescaleRGBAction() :
   Action( "Image > Rescale > Rescale RGB/K", Bitmap( RescaleRGBActionIcon_XPM ), "Intensity Transformations" )
{
   SetToolTip( "Rescale" );
}

void RescaleRGBAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::RGBK ).LaunchOnCurrentWindow();
}

bool RescaleRGBAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

RescaleRGBIndividualAction::RescaleRGBIndividualAction() :
   Action( "Image > Rescale > Rescale Individual RGB/K Channels", Bitmap( RescaleRGBActionIcon_XPM ) )
{
}

void RescaleRGBIndividualAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::RGBK ).LaunchOnCurrentWindow();
}

bool RescaleRGBIndividualAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

RescaleCIELAction::RescaleCIELAction() :
   Action( "Image > Rescale > Rescale CIE L* Component", Bitmap( RescaleLumActionIcon_XPM ) )
{
}

void RescaleCIELAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::CIEL ).LaunchOnCurrentWindow();
}

bool RescaleCIELAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

RescaleCIEYAction::RescaleCIEYAction() :
   Action( "Image > Rescale > Rescale CIE Y Component", Bitmap( RescaleLumActionIcon_XPM ) )
{
}

void RescaleCIEYAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::CIEY ).LaunchOnCurrentWindow();
}

bool RescaleCIEYAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RescaleActions.cpp - Released 2018-11-23T18:45:58Z
