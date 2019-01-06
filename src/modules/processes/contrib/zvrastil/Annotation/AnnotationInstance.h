//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 01.00.00.0233
// ----------------------------------------------------------------------------
// AnnotationInstance.h - Released 2018-12-12T09:25:25Z
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

#ifndef __AnnotationInstance_h
#define __AnnotationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------
// AnnotationInstance
// ----------------------------------------------------------------------------

class AnnotationInstance : public ProcessImplementation
{
public:

   AnnotationInstance( const MetaProcess* );
   AnnotationInstance( const AnnotationInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   String   annotationText;            // annotation text
   String   annotationFont;            // annotation font face
   uint8    annotationFontSize;        // annotation font size in points
   pcl_bool annotationFontBold;        // flag whether font is bold
   pcl_bool annotationFontItalic;      // flag whether font is italic
   pcl_bool annotationFontUnderline;   // flag whether font is underlined
   pcl_bool annotationFontShadow;      // flag whether font has shadow
   RGBA     annotationColor;           // annotation color
   int32    annotationPositionX;       // annotation text position
   int32    annotationPositionY;
   pcl_bool annotationShowLeader;      // flag whether to show leader line
   int32    annotationLeaderPositionX; // leader endpoint position
   int32    annotationLeaderPositionY;
   uint8    annotationOpacity;         // annotation opacity

   friend class AnnotationEngine;
   friend class AnnotationProcess;
   friend class AnnotationInterface;
   friend class AnnotationRenderer;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __AnnotationInstance_h

// ----------------------------------------------------------------------------
// EOF AnnotationInstance.h - Released 2018-12-12T09:25:25Z
