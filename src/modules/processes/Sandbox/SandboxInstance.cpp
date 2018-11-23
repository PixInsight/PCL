//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 01.00.02.0276
// ----------------------------------------------------------------------------
// SandboxInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
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

#include "SandboxInstance.h"
#include "SandboxParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SandboxInstance::SandboxInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_one( TheSandboxParameterOneParameter->DefaultValue() ),
   p_two( int32( TheSandboxParameterTwoParameter->DefaultValue() ) ),
   p_three( TheSandboxParameterThreeParameter->DefaultValue() ),
   p_four( SandboxParameterFour::Default ),
   p_five( TheSandboxParameterFiveParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

SandboxInstance::SandboxInstance( const SandboxInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void SandboxInstance::Assign( const ProcessImplementation& p )
{
   const SandboxInstance* x = dynamic_cast<const SandboxInstance*>( &p );
   if ( x != nullptr )
   {
      p_one   = x->p_one;
      p_two   = x->p_two;
      p_three = x->p_three;
      p_four  = x->p_four;
      p_five  = x->p_five;
   }
}

// ----------------------------------------------------------------------------

UndoFlags SandboxInstance::UndoMode( const View& ) const
{
   /*
    * The following flag assumes that your process modifies pixel sample values
    * *exclusively*. If you are going to change anything else, such as image
    * geometry, keywords, etc., or maybe nothing at all (in case you are
    * implementing an image observer process), see the UpdateFlag enumeration
    * in pcl/ImageWindow.h for complete information.
    */
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool SandboxInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Sandbox cannot be executed on complex images.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class SandboxEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const SandboxInstance& instance )
   {
      /*
       * Your magic comes here...
       */
      Console().WriteLn( "<end><cbr>Ah, did I mention that I do just <em>nothing</em> at all? ;D" );
   }
};

// ----------------------------------------------------------------------------

bool SandboxInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: SandboxEngine::Apply( static_cast<Image&>( *image ), *this ); break;
      case 64: SandboxEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: SandboxEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
      case 16: SandboxEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
      case 32: SandboxEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
      }

   return true;
}

// ----------------------------------------------------------------------------

void* SandboxInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSandboxParameterOneParameter )
      return &p_one;
   if ( p == TheSandboxParameterTwoParameter )
      return &p_two;
   if ( p == TheSandboxParameterThreeParameter )
      return &p_three;
   if ( p == TheSandboxParameterFourParameter )
      return &p_four;
   if ( p == TheSandboxParameterFiveParameter )
      return p_five.Begin();

   return nullptr;
}

// ----------------------------------------------------------------------------

bool SandboxInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSandboxParameterFiveParameter )
   {
      p_five.Clear();
      if ( sizeOrLength > 0 )
         p_five.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type SandboxInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSandboxParameterFiveParameter )
      return p_five.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxInstance.cpp - Released 2018-11-23T18:45:58Z
