//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 01.00.00.0233
// ----------------------------------------------------------------------------
// AnnotationProcess.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2018 Zbynek Vrastil
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#include "AnnotationProcess.h"
#include "AnnotationParameters.h"
#include "AnnotationInstance.h"
#include "AnnotationInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "AnnotationIcon.xpm"

// ----------------------------------------------------------------------------

AnnotationProcess* TheAnnotationProcess = 0;

// ----------------------------------------------------------------------------

AnnotationProcess::AnnotationProcess() : MetaProcess()
{
   TheAnnotationProcess = this;

   // Instantiate process parameters
   new AnnotationText( this );
   new AnnotationFont( this );
   new AnnotationFontSize( this );
   new AnnotationFontBold( this );
   new AnnotationFontItalic( this );
   new AnnotationFontUnderline( this );
   new AnnotationFontShadow( this );
   new AnnotationColor( this );
   new AnnotationPositionX( this );
   new AnnotationPositionY( this );
   new AnnotationShowLeader( this );
   new AnnotationLeaderPositionX( this );
   new AnnotationLeaderPositionY( this );
   new AnnotationOpacity( this );
}

// ----------------------------------------------------------------------------

IsoString AnnotationProcess::Id() const
{
   return "Annotation";
}

// ----------------------------------------------------------------------------

IsoString AnnotationProcess::Category() const
{
   return "Painting";
}

// ----------------------------------------------------------------------------

uint32 AnnotationProcess::Version() const
{
   return 0x101; // required
}

// ----------------------------------------------------------------------------

String AnnotationProcess::Description() const
{
   return

      "<html>"
      "<p>Annotation is an interactive text rendering procedure implemented as a "
      "dynamic PixInsight process. Annotation can only be executed on images, not on previews. "
      "Annotation renders single line of text with selected font and color. Optionally, it also "
      "renders a leader line from text to object. It is designed mainly to add your name and copyright "
      "to the image or to annotate objects on the image.</p>"

      "<p>To start an Annotation session, double-click the corresponding entry on the "
      "Process Explorer; the Annotation interface window will be shown and activated. "
      "Then click on the target image to show the dynamic preview of the annotation. "
      "Now, you can edit the annotation text and properties. Using left mouse button, "
      "you can do fine positioning of the text and leader by dragging. If you hold Ctrl "
      "key during the dragging, text and leader are dragged simultaneously. Otherwise, "
      "only selected part of the annotation is dragged.</p>"

      "<p>By pressing Apply button on the Annotation window, annotation is rendered to the image "
      "pixels. Next click on the image creates new annotation preview in the image.</p>"
      "</html>";
}

// ----------------------------------------------------------------------------

const char** AnnotationProcess::IconImageXPM() const
{
   return AnnotationIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* AnnotationProcess::DefaultInterface() const
{
   return TheAnnotationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* AnnotationProcess::Create() const
{
   return new AnnotationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AnnotationProcess::Clone( const ProcessImplementation& p ) const
{
   const AnnotationInstance* instPtr = dynamic_cast<const AnnotationInstance*>( &p );
   return (instPtr != 0) ? new AnnotationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool AnnotationProcess::CanProcessCommandLines() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AnnotationProcess.cpp - Released 2018-12-12T09:25:25Z
