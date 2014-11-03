// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Convolution Process Module Version 01.01.03.0138
// ****************************************************************************
// ConvolutionInstance.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Convolution PixInsight module.
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

#include "ConvolutionInstance.h"
#include "ConvolutionParameters.h"
#include "FilterLibrary.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/Convolution.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FFTConvolution.h>
#include <pcl/GlobalSettings.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/Resample.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/StdStatus.h>
#include <pcl/VariableShapeFilter.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ConvolutionInstance::ConvolutionInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
mode( ConMode::Default ),
sigma( TheConSigmaParameter->DefaultValue() ),
shape( TheConShapeParameter->DefaultValue() ),
aspectRatio( TheConAspectRatioParameter->DefaultValue() ),
rotationAngle( TheConRotationAngleParameter->DefaultValue() ),
sourceCode(),
rescaleHighPass( TheConRescaleHighPassParameter->DefaultValue() ),
viewId()
{
}

ConvolutionInstance::ConvolutionInstance( const ConvolutionInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ConvolutionInstance::Assign( const ProcessImplementation& p )
{
   const ConvolutionInstance* x = dynamic_cast<const ConvolutionInstance*>( &p );
   if ( x != 0 )
   {
      mode = x->mode;
      sigma = x->sigma;
      shape = x->shape;
      aspectRatio = x->aspectRatio;
      rotationAngle = x->rotationAngle;
      sourceCode = x->sourceCode;
      rescaleHighPass = x->rescaleHighPass;
      viewId = x->viewId;
   }
}

// ----------------------------------------------------------------------------

bool ConvolutionInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Convolution cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

class ConvolutionEngine
{
public:

   static void Apply( ImageVariant& image, const ConvolutionInstance& instance )
   {
      switch ( instance.mode )
      {
      default: // ?
      case ConMode::Parametric:
         {
            VariableShapeFilter H( instance.sigma, instance.shape, 0.01F,
                                   instance.aspectRatio, Rad( instance.rotationAngle ) );

            if ( H.Size() >= 49 ||
                 image.Width() < H.Size() || image.Height() < H.Size() ||
                 !H.IsSeparable() && H.Size() >= 15 )
            {
               FFTConvolution Z( H );
               Z >> image;
            }
            else if ( H.IsSeparable() )
            {
               SeparableFilter S( H.AsSeparableFilter() );
               SeparableConvolution C( S );
               C >> image;
            }
            else
            {
               Convolution C( H );
               C >> image;
            }
         }
         break;

      case ConMode::Library:
         {
            Filter F = Filter::FromSource( instance.sourceCode );
            if ( !F.IsValid() )
               throw Error( "Invalid filter" );

            if ( F.IsSeparable() )
            {
               SeparableConvolution C( F.Separable() );
               C.EnableHighPassRescaling( instance.rescaleHighPass );
               C >> image;
            }
            else
            {
               if ( F.Kernel().Size() < 15 || F.Kernel().IsHighPassFilter() )
               {
                  Convolution C( F.Kernel() );
                  C.EnableHighPassRescaling( instance.rescaleHighPass );
                  C >> image;
               }
               else
               {
                  FFTConvolution Z( F.Kernel() );
                  Z >> image;
                  if ( instance.rescaleHighPass )
                     image.Normalize();
               }
            }
         }
         break;

      case ConMode::Image:
         {
            View view = View::ViewById( instance.viewId );
            if ( view.IsNull() )
               throw Error( "No such view: " + instance.viewId );

            ImageVariant v( view.Image() );
            FFTConvolution Z( v );
            Z >> image;
         }
         break;
      }
   }
};

// ----------------------------------------------------------------------------

void ConvolutionInstance::Convolve( Image& image, int zoomLevel ) const
{
   if ( zoomLevel >= -1 )
   {
      ImageVariant var( &image );
      ConvolutionEngine::Apply( var, *this );
      return;
   }

   int z = -zoomLevel;

   if ( mode == ConMode::Parametric )
   {
      ConvolutionInstance i( *this );
      i.sigma /= z;
      ImageVariant var( &image );
      ConvolutionEngine::Apply( var, i );
      return;
   }

   Image h;

   switch ( mode )
   {
   case ConMode::Library:
      {
         Filter f = Filter::FromSource( sourceCode );
         if ( !f.IsValid() )
            break;

         if ( f.IsHighPass() )
         {
            BicubicSplinePixelInterpolation I;
            Resample S( I, z, z );
            S.SetMode( ResizeMode::RelativeDimensions );
            S >> image;

            if ( f.IsSeparable() )
            {
               SeparableConvolution C( f.Separable() );
               C.EnableHighPassRescaling( rescaleHighPass );
               C >> image;
            }
            else
            {
               Convolution C( f.Kernel() );
               C.EnableHighPassRescaling( rescaleHighPass );
               C >> image;
            }

            S.SetScalingFactors( 1.0/z, 1.0/z );
            S >> image;
            return;
         }
         else
         {
            if ( f.IsSeparable() )
               KernelFilter( KernelFilter::coefficient_matrix::FromColumnVector( f.Separable().ColFilter() ) *
                             KernelFilter::coefficient_matrix::FromRowVector( f.Separable().RowFilter() ) ).ToImage( h );
            else
               f.Kernel().ToImage( h );
         }
      }
      break;

   case ConMode::Image:
      {
         View view = View::ViewById( viewId );
         if ( view.IsNull() )
            return;
         ImageVariant( &h ).CopyImage( view.Image() );
      }
      break;
   }

   int hw = RoundI( double( h.Width() )/z );
   int hh = RoundI( double( h.Height() )/z );
   if ( hw < 2 || hh < 2 )
      return;

   BicubicSplinePixelInterpolation I;
   Resample S( I );
   S.SetSizes( hw, hh );
   S.SetMode( ResizeMode::AbsolutePixels );
   S.SetAbsMode( AbsoluteResizeMode::ForceWidthAndHeight );
   S >> h;

   FFTConvolution( ImageVariant( &h ) ) >> image;
}

// ----------------------------------------------------------------------------

bool ConvolutionInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   ConvolutionEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* ConvolutionInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheConModeParameter )
      return &mode;
   if ( p == TheConSigmaParameter )
      return &sigma;
   if ( p == TheConShapeParameter )
      return &shape;
   if ( p == TheConAspectRatioParameter )
      return &aspectRatio;
   if ( p == TheConRotationAngleParameter )
      return &rotationAngle;
   if ( p == TheConSourceCodeParameter )
      return sourceCode.c_str();
   if ( p == TheConRescaleHighPassParameter )
      return &rescaleHighPass;
   if ( p == TheConViewIdParameter )
      return viewId.c_str();
   return 0;
}

// ----------------------------------------------------------------------------

bool ConvolutionInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheConViewIdParameter )
   {
      viewId.Clear();
      if ( sizeOrLength > 0 )
         viewId.Reserve( sizeOrLength );
   }
   else if ( p == TheConSourceCodeParameter )
   {
      sourceCode.Clear();
      if ( sizeOrLength > 0 )
         sourceCode.Reserve( sizeOrLength );
   }
   else
      return false;
   return true;
}

// ----------------------------------------------------------------------------

size_type ConvolutionInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheConViewIdParameter )
      return viewId.Length();
   if ( p == TheConSourceCodeParameter )
      return sourceCode.Length();
   return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Render a VariableShape filter as a 32-bit floating point image.
 */
static void CreateVariableShapeFilterImage( Image& image, float sigma, float shape, float aspectRatio, float rotationAngleDeg )
{
   VariableShapeFilter G( sigma, shape, 0.01F, aspectRatio, Rad( rotationAngleDeg ) );
   G.ToImage( image );
}

/*
 * Render a kernel filter as a 32-bit floating point image
 */
static void CreateKernelFilterImage( Image& image, const KernelFilter& F )
{
   KernelFilter::coefficient_matrix A = F.Coefficients();
   image.AllocateData( A.Cols(), A.Rows(), 3, ColorSpace::RGB );
   float* r = image[0];
   float* g = image[1];
   float* b = image[2];
   const KernelFilter::coefficient* a = A.Begin();
   const KernelFilter::coefficient* e = A.End();
   for ( ; a < e; ++a, ++r, ++g, ++b )
   {
      if ( *a < 0 )
      {
         // Negative coefficients are rendered red
         *r = -*a;
         *g = *b = 0;
      }
      else
      {
         // Positive coeffcients are rendered gray
         *r = *g = *b = *a;
      }
   }
}

/*
 * Render a separable filter as a 32-bit floating point image.
 *
 * For separable filters, we compute the equivalent kernel matrix by vector
 * multiplication and render it.
 */
static void CreateKernelFilterImage( Image& image, const SeparableFilter& F )
{
   CreateKernelFilterImage( image, KernelFilter(
      KernelFilter::coefficient_matrix::FromColumnVector( F.ColFilter() ) *
      KernelFilter::coefficient_matrix::FromRowVector( F.RowFilter() ) ) );
}

/*
 * Render a Filter object as a 32-bit floating point image.
 */
static void CreateKernelFilterImage( Image& image, const Filter& F )
{
   if ( F.IsSeparable() )
      CreateKernelFilterImage( image, F.Separable() );
   else
      CreateKernelFilterImage( image, F.Kernel() );
}

/*
 * Render the current filter in a Convolution instance as a 32-bit floating
 * point image.
 */
bool ConvolutionInstance::CreateFilterImage( Image& filter ) const
{
   switch ( mode )
   {
   default:
   case ConMode::Parametric:
      CreateVariableShapeFilterImage( filter, sigma, shape, aspectRatio, rotationAngle );
      break;

   case ConMode::Library:
      {
         try
         {
            Filter f = Filter::FromSource( sourceCode );
            if ( !f.IsValid() )
               return false;
            CreateKernelFilterImage( filter, f );
         }
         catch ( ... )
         {
            return false;
         }
      }
      break;

   case ConMode::Image:
      {
         View view = View::ViewById( viewId );
         if ( view.IsNull() )
            return false;
         ImageVariant( &filter ).CopyImage( view.Image() );
      }
      break;
   }

   filter.Rescale();

   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ConvolutionInstance.cpp - Released 2014/10/29 07:34:55 UTC
