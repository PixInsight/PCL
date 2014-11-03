// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard GradientDomain Process Module Version 00.06.04.0096
// ****************************************************************************
// GradientsHdrCompositionInstance.cpp - Released 2014/10/29 07:35:25 UTC
// ****************************************************************************
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2014. Licensed under LGPL 2.1
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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
// ****************************************************************************

#include "GradientsHdrCompositionInstance.h"
#include "GradientsHdrCompositionParameters.h"
#include "GradientsHdrComposition.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsHdrCompositionInstance::GradientsHdrCompositionInstance( const MetaProcess* m )
  :ProcessImplementation( m )
  ,targetFrames()
  ,dLogBias(TheGradientsHdrCompositionLogBiasParameter->DefaultValue())
  ,bKeepLog(TheGradientsHdrCompositionKeepLogParameter->DefaultValue())
  ,bNegativeBias(TheGradientsHdrCompositionNegativeBiasParameter->DefaultValue())
  ,generateMask(TheGradientsHdrCompositionGenerateMaskParameter->DefaultValue())
{
}

GradientsHdrCompositionInstance::GradientsHdrCompositionInstance( const GradientsHdrCompositionInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void GradientsHdrCompositionInstance::Assign( const ProcessImplementation& p )
{
   const GradientsHdrCompositionInstance* x = dynamic_cast<const GradientsHdrCompositionInstance*>( &p );
   if ( x != 0 )
   {
      targetFrames = x->targetFrames;
      dLogBias = x->dLogBias;
      bKeepLog = x->bKeepLog;
      bNegativeBias = x->bNegativeBias;
      generateMask = x->generateMask;
   }
}

bool GradientsHdrCompositionInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   whyNot = "GradientsHdrComposition can only be executed in the global context.";
   if (dLogBias>1.0) {
     whyNot = "GradientsHdrComposition needs in range 0..1.";
     return false;
   }

   return false;
}

bool GradientsHdrCompositionInstance::IsHistoryUpdater( const View& view ) const
{
   return false;
}

bool GradientsHdrCompositionInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( targetFrames.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

//static
ImageWindow GradientsHdrCompositionInstance::CreateImageWindow( const IsoString& id, GradientsHdrComposition::imageType_t const &rImage_p)
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

//static
ImageWindow GradientsHdrCompositionInstance::CreateImageWindow( const IsoString& id, GradientsHdrComposition::numImageType_t const &rImage_p)
{
  // FIXME change to template style
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


bool GradientsHdrCompositionInstance::ExecuteGlobal()
{
   /*
    * Start with a general validation of working parameters.
    */
  GradientsHdrComposition::imageListType_t imageList;
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
  double bias=0.0;
  const double biasRange=TheGradientsHdrCompositionLogBiasParameter->MaximumValue()-TheGradientsHdrCompositionLogBiasParameter->MinimumValue();
  if(dLogBias>TheGradientsHdrCompositionLogBiasParameter->MinimumValue()+0.05*biasRange){
    bias=std::pow(10.0,dLogBias);
  }
  if(bNegativeBias){
    bias= -bias;
  }

  GradientsHdrComposition::imageType_t result;
  GradientsHdrComposition::numImageType_t dxNumImage,dyNumImage;
  GradientsHdrComposition gradientsHdrComposition;

  // std::cerr<<"Bias="<<bias<<std::endl;
  gradientsHdrComposition.hdrComposition(imageList,bKeepLog,bias,result,dxNumImage,dyNumImage);

  ImageWindow window=CreateImageWindow("HdrComposition",result);
  window.Show();
  if(generateMask){
    dxNumImage.Rescale();
    ImageWindow maskWindow=CreateImageWindow("HdrCompositionDxMask",dxNumImage);
    maskWindow.Show();
    dyNumImage.Rescale();
    maskWindow=CreateImageWindow("HdrCompositionDyMask",dyNumImage);
    maskWindow.Show();
  }

  return true;
}

// ----------------------------------------------------------------------------

void* GradientsHdrCompositionInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGradientsHdrCompositionTargetFrameEnabledParameter )
      return &targetFrames[tableRow].enabled;
   if ( p == TheGradientsHdrCompositionTargetFramePathParameter )
      return targetFrames[tableRow].path.c_str();
   if ( p == TheGradientsHdrCompositionKeepLogParameter )
     return &bKeepLog;
   if ( p == TheGradientsHdrCompositionLogBiasParameter )
     return &dLogBias;
   if ( p == TheGradientsHdrCompositionNegativeBiasParameter )
     return &bNegativeBias;
   if ( p == TheGradientsHdrCompositionGenerateMaskParameter )
     return &generateMask;

   return 0;
}

bool GradientsHdrCompositionInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheGradientsHdrCompositionTargetFramesParameter )
   {
      targetFrames.Clear();
      if ( sizeOrLength > 0 )
         targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheGradientsHdrCompositionTargetFramePathParameter )
   {
      targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         targetFrames[tableRow].path.Reserve( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type GradientsHdrCompositionInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheGradientsHdrCompositionTargetFramesParameter )
      return targetFrames.Length();
   if ( p == TheGradientsHdrCompositionTargetFramePathParameter )
      return targetFrames[tableRow].path.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF GradientsHdrCompositionInstance.cpp - Released 2014/10/29 07:35:25 UTC
