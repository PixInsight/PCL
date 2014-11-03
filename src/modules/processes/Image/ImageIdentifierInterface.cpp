// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Image Process Module Version 01.02.08.0281
// ****************************************************************************
// ImageIdentifierInterface.cpp - Released 2014/10/29 07:35:22 UTC
// ****************************************************************************
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "ImageIdentifierInterface.h"
#include "ImageIdentifierProcess.h"

#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIdentifierInterface* TheImageIdentifierInterface = 0;

// ----------------------------------------------------------------------------

#include "ImageIdentifierIcon.xpm"

// ----------------------------------------------------------------------------

ImageIdentifierInterface::ImageIdentifierInterface() :
ProcessInterface(), instance( TheImageIdentifierProcess ), GUI( 0 )
{
   TheImageIdentifierInterface = this;
}

// ----------------------------------------------------------------------------

ImageIdentifierInterface::~ImageIdentifierInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString ImageIdentifierInterface::Id() const
{
   return "ImageIdentifier";
}

// ----------------------------------------------------------------------------

MetaProcess* ImageIdentifierInterface::Process() const
{
   return TheImageIdentifierProcess;
}

// ----------------------------------------------------------------------------

const char** ImageIdentifierInterface::IconImageXPM() const
{
   return ImageIdentifierIcon_XPM;
}

// ----------------------------------------------------------------------------

void ImageIdentifierInterface::Initialize()
{
   // ### Deferred initialization
   /*
   GUI = new GUIData( *this );
   SetWindowTitle( "ImageIdentifier" );
   UpdateControls();
   */
}

// ----------------------------------------------------------------------------

void ImageIdentifierInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

void ImageIdentifierInterface::ResetInstance()
{
   ImageIdentifierInstance defaultInstance( TheImageIdentifierProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ImageIdentifier" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheImageIdentifierProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ImageIdentifierInterface::NewProcess() const
{
   return new ImageIdentifierInstance( instance );
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const ImageIdentifierInstance* r = dynamic_cast<const ImageIdentifierInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not an ImageIdentifier instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ImageIdentifierInterface::UpdateControls()
{
   GUI->Identifier_Edit.SetText( instance.Id() );
}

// ----------------------------------------------------------------------------

void ImageIdentifierInterface::Identifier_EditCompleted( Edit& sender )
{
   try
   {
      String newId = sender.Text();
      newId.Trim();

      // The identifier can either be empty, meaning that a default image
      // identifier will be set by the PixInsight application, or a valid
      // C identifier.

      if ( !newId.IsEmpty() && !newId.IsValidIdentifier() )
         throw Error( '\'' + newId + "': Invalid identifier" );

      instance.SetId( newId );
      UpdateControls();
   }

   ERROR_CLEANUP(
      UpdateControls();
      )
}

// ----------------------------------------------------------------------------

ImageIdentifierInterface::GUIData::GUIData( ImageIdentifierInterface& w )
{
   Identifier_Label.SetText( "Identifier:" );

   Identifier_Edit.SetMinWidth( w.Font().Width( String( '0', 50 ) ) );
   Identifier_Edit.OnEditCompleted( (pcl::Edit::edit_event_handler)&ImageIdentifierInterface::Identifier_EditCompleted, w );

   Identifier_Sizer.SetSpacing( 6 );
   Identifier_Sizer.Add( Identifier_Label );
   Identifier_Sizer.Add( Identifier_Edit );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( Identifier_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ImageIdentifierInterface.cpp - Released 2014/10/29 07:35:22 UTC
