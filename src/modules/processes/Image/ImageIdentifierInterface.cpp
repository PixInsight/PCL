//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0431
// ----------------------------------------------------------------------------
// ImageIdentifierInterface.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "ImageIdentifierInterface.h"
#include "ImageIdentifierProcess.h"

#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIdentifierInterface* TheImageIdentifierInterface = nullptr;

// ----------------------------------------------------------------------------

#include "ImageIdentifierIcon.xpm"

// ----------------------------------------------------------------------------

ImageIdentifierInterface::ImageIdentifierInterface() :
   instance( TheImageIdentifierProcess )
{
   TheImageIdentifierInterface = this;
}

// ----------------------------------------------------------------------------

ImageIdentifierInterface::~ImageIdentifierInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
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
   if ( GUI == nullptr )
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
   if ( dynamic_cast<const ImageIdentifierInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an ImageIdentifier instance.";
   return false;
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

void ImageIdentifierInterface::__EditCompleted( Edit& sender )
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

void ImageIdentifierInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->Identifier_Edit )
      wantsView = view.IsMainView();
}

void ImageIdentifierInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->Identifier_Edit )
      if ( view.IsMainView() )
      {
         instance.SetId( view.Id() );
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

ImageIdentifierInterface::GUIData::GUIData( ImageIdentifierInterface& w )
{
   Identifier_Label.SetText( "Identifier:" );

   Identifier_Edit.SetMinWidth( w.Font().Width( String( '0', 50 ) ) );
   Identifier_Edit.OnEditCompleted( (pcl::Edit::edit_event_handler)&ImageIdentifierInterface::__EditCompleted, w );
   Identifier_Edit.OnViewDrag( (Control::view_drag_event_handler)&ImageIdentifierInterface::__ViewDrag, w );
   Identifier_Edit.OnViewDrop( (Control::view_drop_event_handler)&ImageIdentifierInterface::__ViewDrop, w );

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

// ----------------------------------------------------------------------------
// EOF ImageIdentifierInterface.cpp - Released 2018-12-12T09:25:25Z
