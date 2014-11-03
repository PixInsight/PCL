// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Image Process Module Version 01.02.08.0281
// ****************************************************************************
// SampleFormatConversionInstance.cpp - Released 2014/10/29 07:35:22 UTC
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

#include "SampleFormatConversionInstance.h"

#include <pcl/View.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/MessageBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SampleFormatConversionInstance::SampleFormatConversionInstance( const MetaProcess* P, int t ) :
ProcessImplementation( P ), sampleFormat( t )
{
}

// ----------------------------------------------------------------------------

SampleFormatConversionInstance::SampleFormatConversionInstance( const SampleFormatConversionInstance& x ) :
ProcessImplementation( x ), sampleFormat( x.sampleFormat )
{
}

// -------------------------------------------------------------------------

void SampleFormatConversionInstance::Assign( const ProcessImplementation& p )
{
   const SampleFormatConversionInstance* x = dynamic_cast<const SampleFormatConversionInstance*>( &p );
   if ( x != 0 )
      sampleFormat = x->sampleFormat;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "SampleFormatConversion cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::ExecuteOn( View& view )
{
   try
   {
      // We only can execute on main views
      if ( !view.IsMainView() )
         return false;  // should never reach this point!

      // Lock the view for read/write operations
      view.Lock();

      // Get access to the window where the view lives
      ImageWindow w = view.Window();

      // Allow users to abort this process
      Console().EnableAbort();

      // Apply the transform
      switch ( sampleFormat )
      {
      case SampleFormatConversion::To8Bit:
         w.SetSampleFormat( 8, false );
         break;
      case SampleFormatConversion::To16Bit:
         w.SetSampleFormat( 16, false );
         break;
      case SampleFormatConversion::To32Bit:
         w.SetSampleFormat( 32, false );
         break;
      case SampleFormatConversion::ToFloat:
         w.SetSampleFormat( 32, true );
         break;
      case SampleFormatConversion::ToDouble:
         w.SetSampleFormat( 64, true );
         break;
      }

      // Unlock the view for read/write
      view.Unlock();

      // Return true to signal successful process completion
      return true;
   }

   catch ( ... )
   {
      // Never leave a view locked!
      view.Unlock();

      // Pass exceptions back to the PCL
      throw;
   }
}

// ----------------------------------------------------------------------------

void* SampleFormatConversionInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSampleFormatConversionParameter )
      return &sampleFormat;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF SampleFormatConversionInstance.cpp - Released 2014/10/29 07:35:22 UTC
