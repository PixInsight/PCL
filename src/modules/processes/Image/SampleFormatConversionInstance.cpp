//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0410
// ----------------------------------------------------------------------------
// SampleFormatConversionInstance.cpp - Released 2018-11-01T11:07:21Z
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

#include "SampleFormatConversionInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SampleFormatConversionInstance::SampleFormatConversionInstance( const MetaProcess* P, int t ) :
   ProcessImplementation( P ),
   sampleFormat( t )
{
}

// ----------------------------------------------------------------------------

SampleFormatConversionInstance::SampleFormatConversionInstance( const SampleFormatConversionInstance& x ) :
   ProcessImplementation( x ),
   sampleFormat( x.sampleFormat )
{
}

// -------------------------------------------------------------------------

void SampleFormatConversionInstance::Assign( const ProcessImplementation& p )
{
   const SampleFormatConversionInstance* x = dynamic_cast<const SampleFormatConversionInstance*>( &p );
   if ( x != nullptr )
      sampleFormat = x->sampleFormat;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags SampleFormatConversionInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "SampleFormatConversion cannot be executed on previews.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   ImageWindow window = view.Window();

   Console().EnableAbort();

   switch ( sampleFormat )
   {
   case SampleFormatConversion::To8Bit:
      window.SetSampleFormat( 8, false );
      break;
   case SampleFormatConversion::To16Bit:
      window.SetSampleFormat( 16, false );
      break;
   case SampleFormatConversion::To32Bit:
      window.SetSampleFormat( 32, false );
      break;
   case SampleFormatConversion::ToFloat:
      window.SetSampleFormat( 32, true );
      break;
   case SampleFormatConversion::ToDouble:
      window.SetSampleFormat( 64, true );
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* SampleFormatConversionInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSampleFormatConversionParameter )
      return &sampleFormat;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionInstance.cpp - Released 2018-11-01T11:07:21Z
