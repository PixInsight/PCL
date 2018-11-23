//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0230
// ----------------------------------------------------------------------------
// GradientsMergeMosaicInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2018. Licensed under LGPL 2.1
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include "GradientsMergeMosaicInstance.h"
#include "GradientsMergeMosaicParameters.h"
#include "GradientsMergeMosaic.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsMergeMosaicInstance::GradientsMergeMosaicInstance( const MetaProcess* m )
  :ProcessImplementation( m )
  ,targetFrames()
  ,type(GradientsMergeMosaicType::Default)
  ,shrinkCount(TheGradientsMergeMosaicShrinkCountParameter->DefaultValue())
  ,featherRadius(TheGradientsMergeMosaicFeatherRadiusParameter->DefaultValue())
  ,blackPoint(TheGradientsMergeMosaicBlackPointParameter->DefaultValue())
  ,generateMask(TheGradientsMergeMosaicGenerateMaskParameter->DefaultValue())
{
}

GradientsMergeMosaicInstance::GradientsMergeMosaicInstance( const GradientsMergeMosaicInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void GradientsMergeMosaicInstance::Assign( const ProcessImplementation& p )
{
   const GradientsMergeMosaicInstance* x = dynamic_cast<const GradientsMergeMosaicInstance*>( &p );
   if ( x != 0 )
   {
      targetFrames = x->targetFrames;
      type = x->type;
      shrinkCount=x->shrinkCount;
      featherRadius=x->featherRadius;
      blackPoint = x->blackPoint;
      generateMask = x->generateMask;
   }
}

bool GradientsMergeMosaicInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "GradientsMergeMosaic can only be executed in the global context.";
   return false;
}

bool GradientsMergeMosaicInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

bool GradientsMergeMosaicInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( targetFrames.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

//static
ImageWindow GradientsMergeMosaicInstance::CreateImageWindow( const IsoString& id, GradientsMergeMosaic::imageType_t const &rImage_p)
{
  int const nRows=rImage_p.Height();
  int const nCols=rImage_p.Width();
  int const nChannels=rImage_p.NumberOfNominalChannels();
  int const nBitsPerSample=rImage_p.BitsPerSample();
  bool const bIsFloatSample=rImage_p.IsFloatSample();
  bool const bIsColor=rImage_p.IsColor();

  ImageWindow window( nCols, nRows, nChannels,
		      nBitsPerSample, bIsFloatSample, bIsColor, true, id );
  if ( window.IsNull() )
    throw Error( "Unable to create image window: " + id );
  window.MainView().Image().CopyImage(rImage_p);
  return window;
}

bool GradientsMergeMosaicInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
  GradientsMergeMosaic::imageListType_t imageList;
  {
    String why;
    if ( !CanExecuteGlobal( why ) )
      throw Error( why );
    for ( image_list::const_iterator i = targetFrames.Begin(); i != targetFrames.End(); ++i ){
      if ( i->enabled){
	if (!File::Exists( i->path ) )
	  throw( "No such file exists on the local filesystem: " + i->path );
	imageList.Add(i->path);
      }
    }
  }
  GradientsMergeMosaic::imageType_t result;
  GradientsMergeMosaic::sumMaskImageType_t sumMask;
  GradientsMergeMosaic gradientsMergeMosaic;

  gradientsMergeMosaic.mergeMosaic(imageList,blackPoint,type,shrinkCount,featherRadius,result,sumMask);

  ImageWindow window=CreateImageWindow("MergeMosaic",result);
  window.Show();
  if(generateMask){
    sumMask.Rescale();
    ImageWindow maskWindow=CreateImageWindow("MergeMosaicMask",sumMask);
    maskWindow.Show();
  }

  return true;
}

// ----------------------------------------------------------------------------

void* GradientsMergeMosaicInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGradientsMergeMosaicTargetFrameEnabledParameter )
      return &targetFrames[tableRow].enabled;
   if ( p == TheGradientsMergeMosaicTargetFramePathParameter )
      return targetFrames[tableRow].path.Begin();
   if ( p == TheGradientsMergeMosaicTypeParameter )
     return &type;
   if ( p == TheGradientsMergeMosaicShrinkCountParameter )
     return &shrinkCount;
   if ( p == TheGradientsMergeMosaicFeatherRadiusParameter )
     return &featherRadius;
   if ( p == TheGradientsMergeMosaicBlackPointParameter )
     return &blackPoint;
   if ( p == TheGradientsMergeMosaicGenerateMaskParameter )
     return &generateMask;


   return 0;
}

bool GradientsMergeMosaicInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGradientsMergeMosaicTargetFramesParameter )
   {
      targetFrames.Clear();
      if ( sizeOrLength > 0 )
         targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheGradientsMergeMosaicTargetFramePathParameter )
   {
      targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         targetFrames[tableRow].path.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type GradientsMergeMosaicInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheGradientsMergeMosaicTargetFramesParameter )
      return targetFrames.Length();
   if ( p == TheGradientsMergeMosaicTargetFramePathParameter )
      return targetFrames[tableRow].path.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicInstance.cpp - Released 2018-11-23T18:45:58Z
