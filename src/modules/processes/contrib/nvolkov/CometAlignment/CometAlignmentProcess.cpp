//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 01.02.06.0181
// ----------------------------------------------------------------------------
// CometAlignmentProcess.cpp - Released 2017-07-18T16:14:19Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2017 Nikolay Volkov
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#include "CometAlignmentProcess.h"
#include "CometAlignmentParameters.h"
#include "CometAlignmentInstance.h"
#include "CometAlignmentInterface.h"
#include "CometAlignmentModule.h" // for CometAlignmentModule::ReadableVersion();

namespace pcl
{
#include "CometAlignmentIcon.xpm"

// ----------------------------------------------------------------------------

CometAlignmentProcess* TheCometAlignmentProcess = 0;

// ----------------------------------------------------------------------------

//#include "CometAlignmentIcon.xpm"

// ----------------------------------------------------------------------------

CometAlignmentProcess::CometAlignmentProcess () : MetaProcess ()
{
   TheCometAlignmentProcess = this;

   // Instantiate process parameters
   new CATargetFrames (this);

   new CATargetFrameEnabled (TheTargetFrames);
   new CATargetFramePath (TheTargetFrames);
   new CATargetFrameDate (TheTargetFrames);
   new CATargetFrameJDate (TheTargetFrames);
   new CATargetFrameX (TheTargetFrames);
   new CATargetFrameY (TheTargetFrames);
   new CADrizzlePath( TheTargetFrames );

   new CAInputHints( this );
   new CAOutputHints( this );
   new CAOutputDir (this);
   new CAOutputExtension( this );
   new CAPrefix (this);
   new CAPostfix (this);
   new CAOverwrite (this);

   new CAReference (this);

   new CASubtractFile (this);
   new CASubtractMode (this);
   new CAEnableLinearFit (this);
   new CARejectLow (this);
   new CARejectHigh (this);
   new CANormalize (this);
   new CADrzSaveSA (this);
   new CADrzSaveCA (this);
   new CAOperandIsDI (this);

   new CAPixelInterpolation (this);
   new CALinearClampingThreshold (this);
}

// ----------------------------------------------------------------------------

IsoString CometAlignmentProcess::Id () const
{
   return "CometAlignment";
}

// ----------------------------------------------------------------------------

IsoString CometAlignmentProcess::Category () const
{
   return "ImageRegistration";
}

// ----------------------------------------------------------------------------

uint32 CometAlignmentProcess::Version () const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CometAlignmentProcess::Description () const
{
   return CometAlignmentModule::ReadableVersion () +
           "<html><p>For more detailed information, please refer to the Release Information board on PixInsight Forum:<br/>"
           "http://pixinsight.com/forum/index.php?topic=3980.0</html>";

}

const char** CometAlignmentProcess::IconImageXPM() const
{
   return CometAlignmentIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* CometAlignmentProcess::DefaultInterface () const
{
   return TheCometAlignmentInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* CometAlignmentProcess::Create () const
{
   return new CometAlignmentInstance (this);
}

// ----------------------------------------------------------------------------

ProcessImplementation* CometAlignmentProcess::Clone (const ProcessImplementation& p) const
{
   const CometAlignmentInstance* instPtr = dynamic_cast<const CometAlignmentInstance*> (&p);
   return (instPtr != 0) ? new CometAlignmentInstance (*instPtr) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CometAlignmentProcess.cpp - Released 2017-07-18T16:14:19Z
