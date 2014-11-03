// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Image Process Module Version 01.02.08.0281
// ****************************************************************************
// ImageIdentifierInstance.cpp - Released 2014/10/29 07:35:22 UTC
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

#include "ImageIdentifierInstance.h"

#include <pcl/View.h>
#include <pcl/ImageWindow.h> // for UndoFlags
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIdentifierInstance::ImageIdentifierInstance( const MetaProcess* m ) : ProcessImplementation( m ), id()
{
}

ImageIdentifierInstance::ImageIdentifierInstance( const ImageIdentifierInstance& x ) : ProcessImplementation( x ), id( x.id )
{
}

bool ImageIdentifierInstance::Validate( pcl::String& info )
{
   id.Trim();

   // The identifier can either be empty, meaning that a default image
   // identifier will be automatically assigned by the PixInsight core
   // application, or a valid C identifier.
   //
   // We only have to check validity of our identifier here. Uniqueness in any
   // particular naming context is enforced by the core application.

   if ( !id.IsEmpty() && !id.IsValidIdentifier() ) // String::IsValidIdentifier() does the job
   {
      info = '\'' + id + "' is not a valid identifier.";
      return false;
   }

   info.Clear();
   return true;
}

void ImageIdentifierInstance::Assign( const ProcessImplementation& p )
{
   const ImageIdentifierInstance* x = dynamic_cast<const ImageIdentifierInstance*>( &p );
   if ( x != 0 )
      id = x->id;
}

bool ImageIdentifierInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( !v.IsMainView() )
   {
      whyNot = "ImageIdentifier can only be executed on main views, not on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool ImageIdentifierInstance::IsMaskable( const View&, const ImageWindow& /*mask*/ ) const
{
   return false;
}

UndoFlags ImageIdentifierInstance::UndoMode( const View& ) const
{
   return UndoFlag::ImageId;
}

bool ImageIdentifierInstance::ExecuteOn( View& view )
{
   Console().WriteLn( "id = " + id );
   view.Rename( id );
   return true;
}

void* ImageIdentifierInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheImageIdentifierParameter )
      return id.c_str();
   return 0;
}

bool ImageIdentifierInstance::AllocateParameter( size_type length, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheImageIdentifierParameter )
   {
      id.Clear();
      if ( length > 0 )
         id.Reserve( length );
   }
   else
      return false;

   return true;
}

size_type ImageIdentifierInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheImageIdentifierParameter )
      return id.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ImageIdentifierInstance.cpp - Released 2014/10/29 07:35:22 UTC
