// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard IntensityTransformations Process Module Version 01.07.00.0285
// ****************************************************************************
// BinarizeInstance.cpp - Released 2014/10/29 07:35:24 UTC
// ****************************************************************************
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "BinarizeInstance.h"
#include "BinarizeParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Histogram.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeInstance::BinarizeInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
isGlobal( TheBinarizeIsGlobalParameter->DefaultValue() )
{
   level[0] = TheBinarizeLevelRParameter->DefaultValue();
   level[1] = TheBinarizeLevelGParameter->DefaultValue();
   level[2] = TheBinarizeLevelBParameter->DefaultValue();
}

BinarizeInstance::BinarizeInstance( const BinarizeInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void BinarizeInstance::Assign( const ProcessImplementation& p )
{
   const BinarizeInstance* x = dynamic_cast<const BinarizeInstance*>( &p );
   if ( x != 0 )
   {
      for ( int i = 0; i < 3; ++i )
         level[i] = x->level[i];
      isGlobal = x->isGlobal;
   }
}

// ----------------------------------------------------------------------------

bool BinarizeInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Binarize cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

//----------------------------------------------------------------------------

class BinarizeEngine
{
public:

   static void Apply( ImageVariant& image, const BinarizeInstance& instance )
   {
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         image.SelectChannel( c );
         if ( instance.isGlobal )
            image.Binarize( instance.level[0] );
         else
            image.Binarize( instance.level[c] );
      }
   }
};

// ----------------------------------------------------------------------------

void BinarizeInstance::Preview( UInt16Image& image ) const
{
   try
   {
      ImageVariant var( &image );
      BinarizeEngine::Apply( var, *this );
   }
   catch ( ... )
   {
   }
}

// ----------------------------------------------------------------------------

bool BinarizeInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   BinarizeEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* BinarizeInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheBinarizeLevelRParameter )
      return level+0;
   if ( p == TheBinarizeLevelGParameter )
      return level+1;
   if ( p == TheBinarizeLevelBParameter )
      return level+2;
   if ( p == TheBinarizeIsGlobalParameter )
      return &isGlobal;
  return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF BinarizeInstance.cpp - Released 2014/10/29 07:35:24 UTC
