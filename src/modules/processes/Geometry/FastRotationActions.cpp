//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.02.0379
// ----------------------------------------------------------------------------
// FastRotationActions.cpp - Released 2017-10-16T10:07:46Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "FastRotationActions.h"
#include "FastRotationInstance.h"
#include "FastRotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#include "Rotate180ActionIcon.xpm"
#include "Rotate90CWActionIcon.xpm"
#include "Rotate90CCWActionIcon.xpm"
#include "HorizontalMirrorActionIcon.xpm"
#include "VerticalMirrorActionIcon.xpm"

// ----------------------------------------------------------------------------

Rotate180Action::Rotate180Action() :
   Action( "Image > Geometry > Rotate 180\xb0", Bitmap( Rotate180ActionIcon_XPM ), "Geometry" )
{
   SetToolTip( "Rotate 180\xb0" );
}

void Rotate180Action::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::Rotate180 ).LaunchOnCurrentWindow();
}

bool Rotate180Action::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

Rotate90CWAction::Rotate90CWAction() :
   Action( "Image > Geometry > Rotate 90\xb0 Clockwise", Bitmap( Rotate90CWActionIcon_XPM ), "Geometry" )
{
   SetToolTip( "Rotate 90\xb0 Clockwise" );
}

void Rotate90CWAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::Rotate90CW ).LaunchOnCurrentWindow();
}

bool Rotate90CWAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

Rotate90CCWAction::Rotate90CCWAction() :
   Action( "Image > Geometry > Rotate 90\xb0 Counter-clockwise", Bitmap( Rotate90CCWActionIcon_XPM ), "Geometry" )
{
   SetToolTip( "Rotate 90\xb0 Counter-clockwise" );
}

void Rotate90CCWAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::Rotate90CCW ).LaunchOnCurrentWindow();
}

bool Rotate90CCWAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

HorizontalMirrorAction::HorizontalMirrorAction() :
   Action( "Image > Geometry >> Horizontal Mirror", Bitmap( HorizontalMirrorActionIcon_XPM ), "Geometry" )
{
   SetToolTip( "Horizontal Mirror" );
}

void HorizontalMirrorAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::HorizontalMirror ).LaunchOnCurrentWindow();
}

bool HorizontalMirrorAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

VerticalMirrorAction::VerticalMirrorAction() :
   Action( "Image > Geometry > Vertical Mirror", Bitmap( VerticalMirrorActionIcon_XPM ), "Geometry" )
{
   SetToolTip( "Vertical Mirror" );
}

void VerticalMirrorAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::VerticalMirror ).LaunchOnCurrentWindow();
}

bool VerticalMirrorAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FastRotationActions.cpp - Released 2017-10-16T10:07:46Z
