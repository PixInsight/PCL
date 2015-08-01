//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0266
// ----------------------------------------------------------------------------
// DynamicCropProcess.cpp - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "DynamicCropProcess.h"
#include "DynamicCropParameters.h"
#include "DynamicCropInstance.h"
#include "DynamicCropInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DynamicCropProcess* TheDynamicCropProcess = 0;

// ----------------------------------------------------------------------------

#include "DynamicCropIcon.xpm"

// ----------------------------------------------------------------------------

DynamicCropProcess::DynamicCropProcess() : MetaProcess()
{
   TheDynamicCropProcess = this;

   // Instantiate process parameters
   // Common parameters require explicit assignment to global instances.
   new CropCenterX( this );
   new CropCenterY( this );
   new CropWidth( this );
   new CropHeight( this );
   TheRotationAngleDynamicCropParameter = new RotationAngle( this );
   new ScaleX( this );
   new ScaleY( this );
   TheOptimizeFastRotationsDynamicCropParameter = new OptimizeFastRotations( this );
   TheInterpolationAlgorithmDynamicCropParameter = new InterpolationAlgorithm( this );
   TheClampingThresholdDynamicCropParameter = new ClampingThreshold( this );
   TheSmoothnessDynamicCropParameter = new Smoothness( this );;
   TheFillRedDynamicCropParameter = new FillRed( this );
   TheFillGreenDynamicCropParameter = new FillGreen( this );
   TheFillBlueDynamicCropParameter = new FillBlue( this );
   TheFillAlphaDynamicCropParameter = new FillAlpha( this );
}

// ----------------------------------------------------------------------------

IsoString DynamicCropProcess::Id() const
{
   return "DynamicCrop";
}

// ----------------------------------------------------------------------------

IsoString DynamicCropProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 DynamicCropProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String DynamicCropProcess::Description() const
{
   return

   "<html>"
   "<p>DynamicCrop is an interactive cropping/rotation/scaling procedure implemented as a "
   "dynamic PixInsight process. As most geometric processes, DynamicCrop can only be "
   "executed on images, not on previews.</p>"

   "<p>To start a DynamicCrop session, double-click the corresponding entry on the "
   "Process Explorer; the DynamicCrop interface window will be shown and activated. "
   "Then click on the target image and drag the mouse cursor to define an initial "
   "cropping rectangle. Once you release the mouse, the DynamicCrop window becomes "
   "active and fully operational.</p>"

   "<p>You can edit DynamicCrop parameters by entering their numeric values literally, "
   "but most of the time you'll be defining a cropping area on the image with the mouse. "
   "To use DynamicCrop interactively, you can perform, among many other actions:</p>"

   "<ul>"
   "<li>Click <i>inside</i> the cropping rectangle and drag to <b>move</b> it.<br/></li>"

   "<li>Click <i>outside</i> the cropping rectangle and drag to <b>rotate</b> it.<br/></li>"

   "<li>Click <i>on one side or corner</i> of the rectangle to change its <b>dimensions</b>.<br/></li>"

   "<li>Click on the <i>center icon</i> (a small circle initially located at the rectangle's center) "
   "and drag to <b>move the center of rotation</b>. You can double-click the center icon to "
   "relocate it at its default central position.<br/></li>"

   "<li>Double-click inside the cropping rectangle to <b>commit</b> the defined cropping/rotation/scaling "
   "operation.<br/></li>"

   "<li>Pressing the Return key or clicking the Execute button on the DynamicCrop window "
   "also commit the currently defined process. The Escape key cancels the DynamicCrop session.<br/></li>"
   "</ul>"

   "<p>Watch the mouse cursor; its shape changes to give you feedback about the actions "
   "available as a function of the current cursor position over the image.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** DynamicCropProcess::IconImageXPM() const
{
   return DynamicCropIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* DynamicCropProcess::DefaultInterface() const
{
   return TheDynamicCropInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* DynamicCropProcess::Create() const
{
   return new DynamicCropInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* DynamicCropProcess::Clone( const ProcessImplementation& p ) const
{
   const DynamicCropInstance* instPtr = dynamic_cast<const DynamicCropInstance*>( &p );
   return (instPtr != 0) ? new DynamicCropInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicCropProcess.cpp - Released 2015/07/31 11:49:48 UTC
