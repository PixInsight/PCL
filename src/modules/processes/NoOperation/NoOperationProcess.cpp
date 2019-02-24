//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard NoOperation Process Module Version 01.00.00.0344
// ----------------------------------------------------------------------------
// NoOperationProcess.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoOperation PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "NoOperationProcess.h"
#include "NoOperationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NoOperationProcess* TheNoOperationProcess = nullptr;

#include "NoOperationIcon.xpm"

// ----------------------------------------------------------------------------

NoOperationProcess::NoOperationProcess() : MetaProcess()
{
   TheNoOperationProcess = this;
}

// ----------------------------------------------------------------------------

IsoString NoOperationProcess::Id() const
{
   return "NoOperation";
}

// ----------------------------------------------------------------------------

IsoString NoOperationProcess::Category() const
{
   return IsoString(); // No specific category
}

// ----------------------------------------------------------------------------

uint32 NoOperationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String NoOperationProcess::Description() const
{
   return

   "<html>"
   "<p>As its name suggests, NoOperation does exactly nothing. This is indeed the simplest possible "
   "process in the PixInsight platform :)</p>"

   "<p>The purpose of NoOperation, however, is not so simple. Process instances are the main actors in "
   "PixInsight, since they are responsible for all of the actual image processing work. However, "
   "there are special situations in which one does not want to modify any image, but despite that, "
   "a proces has to be used <i>in formal terms</i> for a given task to be accomplished.</p>"

   "<p>A notable example of such situations happens with ImageContainer. ImageContainer can be used to "
   "perform a batch format conversion for a set of image files. In this case, one simply specifies "
   "an output filename template like, for example:</p>"

   "<p>&amp;filename;.jpg</p>"

   "<p>to convert every input image file (in any supported file format) to the JPEG format.</p>"

   "<p>However, for ImageContainer to work, a process instance is always required. In this case, NoOperation "
   "can be used to execute ImageContainer without applying any actual process to input images, which are "
   "simply read from disk and written in the JPEG format (which corresponds to the .jpg file extension).</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** NoOperationProcess::IconImageXPM() const
{
   return NoOperationIcon_XPM;
}

// ----------------------------------------------------------------------------

bool NoOperationProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

ProcessImplementation* NoOperationProcess::Create() const
{
   return new NoOperationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* NoOperationProcess::Clone( const ProcessImplementation& p ) const
{
   const NoOperationInstance* instPtr = dynamic_cast<const NoOperationInstance*>( &p );
   return (instPtr != nullptr) ? new NoOperationInstance( this ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NoOperationProcess.cpp - Released 2019-01-21T12:06:42Z
