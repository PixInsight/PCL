// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Global Process Module Version 01.02.05.0260
// ****************************************************************************
// ReadoutOptionsInstance.cpp - Released 2014/11/14 17:18:47 UTC
// ****************************************************************************
// This file is part of the standard Global PixInsight module.
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

#include "ReadoutOptionsInstance.h"

#include <pcl/ReadoutOptions.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptionsInstance::ReadoutOptionsInstance( const MetaProcess* p ) :
ProcessImplementation( p )
{
   SetOptions( ReadoutOptions() );
}

// ----------------------------------------------------------------------------

ReadoutOptionsInstance::ReadoutOptionsInstance( const ReadoutOptionsInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInstance::Assign( const ProcessImplementation& p )
{
   const ReadoutOptionsInstance* x = dynamic_cast<const ReadoutOptionsInstance*>( &p );
   if ( x != 0 )
   {
      data          = x->data;
      mode          = x->mode;
      probeSize     = x->probeSize;
      previewSize   = x->previewSize;
      previewZoom   = x->previewZoom;
      precision     = x->precision;
      range         = x->range;
      showAlpha     = x->showAlpha;
      showMask      = x->showMask;
      showPreview   = x->showPreview;
      previewCenter = x->previewCenter;
      broadcast     = x->broadcast;
      real          = x->real;
   }
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "ReadoutOptions can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::ExecuteGlobal()
{
   ReadoutOptions::SetCurrentOptions( Options() );
   return true;
}

// ----------------------------------------------------------------------------

void* ReadoutOptionsInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheReadoutOptionsDataParameter )
      return &data;
   if ( p == TheReadoutOptionsModeParameter )
      return &mode;
   if ( p == TheReadoutOptionsProbeSizeParameter )
      return &probeSize;
   if ( p == TheReadoutOptionsPreviewSizeParameter )
      return &previewSize;
   if ( p == TheReadoutOptionsPreviewZoomFactorParameter )
      return &previewZoom;
   if ( p == TheReadoutRealPrecisionParameter )
      return &precision;
   if ( p == TheReadoutIntegerRangeParameter )
      return &range;
   if ( p == TheReadoutAlphaParameter )
      return &showAlpha;
   if ( p == TheReadoutMaskParameter )
      return &showMask;
   if ( p == TheReadoutPreviewParameter )
      return &showPreview;
   if ( p == TheReadoutPreviewCenterParameter )
      return &previewCenter;
   if ( p == TheReadoutBroadcastParameter )
      return &broadcast;
   if ( p == TheReadoutRealParameter )
      return &real;
   return 0;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::ValidateParameter( void* value, const MetaParameter* p, size_type tableRow ) const
{
#define i32 *reinterpret_cast<int32*>( value )
#define u32 *reinterpret_cast<uint32*>( value )
#define u8  *reinterpret_cast<uint8*>( value )

   if ( p == TheReadoutOptionsDataParameter )
   {
      if ( i32 < 0 )
      {
         i32 = 0;
         return false;
      }

      if ( i32 >= ReadoutData::NumberOfModes )
      {
         i32 = ReadoutData::NumberOfModes-1;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsModeParameter )
   {
      if ( i32 < 0 )
      {
         i32 = 0;
         return false;
      }

      if ( i32 >= ReadoutMode::NumberOfModes )
      {
         i32 = ReadoutMode::NumberOfModes-1;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsProbeSizeParameter )
   {
      if ( u8 < 1 )
      {
         u8 = 1;
         return false;
      }

      if ( u8 > ReadoutOptions::MaxProbeSize )
      {
         u8 = ReadoutOptions::MaxProbeSize;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsPreviewSizeParameter )
   {
      if ( u8 < ReadoutOptions::MinPreviewSize )
      {
         u8 = ReadoutOptions::MinPreviewSize;
         return false;
      }

      if ( u8 > ReadoutOptions::MaxPreviewSize )
      {
         u8 = ReadoutOptions::MaxPreviewSize;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsPreviewZoomFactorParameter )
   {
      if ( u8 < 1 )
      {
         u8 = 1;
         return false;
      }

      if ( u8 > ReadoutOptions::MaxPreviewZoomFactor )
      {
         u8 = ReadoutOptions::MaxPreviewZoomFactor;
         return false;
      }
   }
   else if ( p == TheReadoutRealPrecisionParameter )
   {
      if ( u8 > ReadoutOptions::MaxPrecision )
      {
         u8 = ReadoutOptions::MaxPrecision;
         return false;
      }
   }
   else if ( p == TheReadoutIntegerRangeParameter )
   {
      if ( u32 < 1 )
      {
         u32 = 1;
         return false;
      }
   }

   return true;

#undef i32
#undef u32
#undef u8
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInstance::LoadCurrentOptions()
{
   SetOptions( ReadoutOptions::GetCurrentOptions() );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ReadoutOptionsInstance.cpp - Released 2014/11/14 17:18:47 UTC
