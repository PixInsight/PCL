//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0307
// ----------------------------------------------------------------------------
// GrayscaleRGBInstances.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "GrayscaleRGBInstances.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ConvertToGrayscaleInstance::ConvertToGrayscaleInstance( const MetaProcess* m ) :
ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

ConvertToGrayscaleInstance::ConvertToGrayscaleInstance( const ConvertToGrayscaleInstance& x ) :
ProcessImplementation( x )
{
}

// ----------------------------------------------------------------------------

bool ConvertToGrayscaleInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ConvertToGrayscale cannot be executed on complex images.";
      return false;
   }

   if ( view.Image().ColorSpace() == ColorSpace::Gray )
   {
      whyNot = "ConvertToGrayscale cannot be executed on grayscale images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool ConvertToGrayscaleInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   image.SetColorSpace( ColorSpace::Gray );

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConvertToRGBColorInstance::ConvertToRGBColorInstance( const MetaProcess* m ) :
ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

ConvertToRGBColorInstance::ConvertToRGBColorInstance( const ConvertToRGBColorInstance& x ) :
ProcessImplementation( x )
{
}

// ----------------------------------------------------------------------------

bool ConvertToRGBColorInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ConvertToRGBColor cannot be executed on complex images.";
      return false;
   }

   if ( view.Image().ColorSpace() == ColorSpace::RGB )
   {
      whyNot = "ConvertToRGBColor cannot be executed on color images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool ConvertToRGBColorInstance::ExecuteOn( View& view )
{
   ImageWindow window = view.Window();
   Array<ImageWindow> windows = ImageWindow::AllWindows();
   for ( size_type i = 0; i < windows.Length(); ++i )
      if ( windows[i].Mask() == window && !windows[i].MainView().IsColor() )
         windows[i].RemoveMask();

   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   image.SetColorSpace( ColorSpace::RGB );

   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBInstances.cpp - Released 2017-04-14T23:07:12Z
