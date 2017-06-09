//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/Crop.cpp - Released 2017-06-09T08:12:54Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Crop.h>

// ----------------------------------------------------------------------------

namespace pcl
{

// ----------------------------------------------------------------------------

void Crop::GetNewSizes( int& width, int& height ) const
{
   switch ( m_mode )
   {
   default:
   case CropMode::RelativeMargins:
      width = Max( 0, RoundInt( width*(1 + m_margins.Left() + m_margins.Right()) ) );
      height = Max( 0, RoundInt( height*(1 + m_margins.Top() + m_margins.Bottom()) ) );
      break;

   case CropMode::AbsolutePixels:
   case CropMode::AbsoluteCentimeters:
   case CropMode::AbsoluteInches:
      {
         double dx = m_margins.Left() + m_margins.Right();
         double dy = m_margins.Top() + m_margins.Bottom();

         if ( m_mode != CropMode::AbsolutePixels )
         {
            dx *= xResolution;
            dy *= yResolution;

            if ( m_mode == CropMode::AbsoluteCentimeters )
            {
               if ( !metric ) // inch -> cm
               {
                  dx /= 2.54F;
                  dy /= 2.54F;
               }
            }
            else // inches
            {
               if ( metric ) // cm -> inch
               {
                  dx *= 2.54F;
                  dy *= 2.54F;
               }
            }
         }

         width = Max( 0, RoundInt( width + dx ) );
         height = Max( 0, RoundInt( height + dy ) );
      }
      break;
   }
}

// ----------------------------------------------------------------------------

class PCL_CropEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Crop& crop )
   {
      try
      {
         DRect margins = crop.Margins();
         int w0 = image.Width(), h0 = image.Height();
         int left, top, right, bottom;

         switch ( crop.Mode() )
         {
         default:
         case CropMode::RelativeMargins:
            left   = RoundInt( margins.Left()   * w0 );
            top    = RoundInt( margins.Top()    * h0 );
            right  = RoundInt( margins.Right()  * w0 );
            bottom = RoundInt( margins.Bottom() * h0 );
            break;

         case CropMode::AbsolutePixels:
         case CropMode::AbsoluteCentimeters:
         case CropMode::AbsoluteInches:
            {
               double dl = margins.Left();
               double dt = margins.Top();
               double dr = margins.Right();
               double db = margins.Bottom();

               if ( crop.Mode() != CropMode::AbsolutePixels )
               {
                  dl *= crop.HorizontalResolution();
                  dt *= crop.VerticalResolution();
                  dr *= crop.HorizontalResolution();
                  db *= crop.VerticalResolution();

                  if ( crop.Mode() == CropMode::AbsoluteCentimeters )
                  {
                     if ( !crop.IsMetricResolution() ) // inch -> cm
                     {
                        dl /= 2.54;
                        dt /= 2.54;
                        dr /= 2.54;
                        db /= 2.54;
                     }
                  }
                  else // inches
                  {
                     if ( crop.IsMetricResolution() ) // cm -> inch
                     {
                        dl *= 2.54;
                        dt *= 2.54;
                        dr *= 2.54;
                        db *= 2.54;
                     }
                  }
               }

               left   = RoundInt( dl );
               top    = RoundInt( dt );
               right  = RoundInt( dr );
               bottom = RoundInt( db );
            }
            break;
         }

         int width  = w0 + left + right;
         int height = h0 + top + bottom;

         if ( width <= 0 || height <= 0 )
         {
            image.FreeData();
            return;
         }

         image.CropBy( left, top, right, bottom, crop.FillValues() );
      }
      catch ( ... )
      {
         image.FreeData();
         throw;
      }
   }
};

// ----------------------------------------------------------------------------

void Crop::Apply( pcl::Image& image ) const
{
   PCL_CropEngine::Apply( image, *this );
}

void Crop::Apply( pcl::DImage& image ) const
{
   PCL_CropEngine::Apply( image, *this );
}

void Crop::Apply( pcl::ComplexImage& image ) const
{
   PCL_CropEngine::Apply( image, *this );
}

void Crop::Apply( pcl::DComplexImage& image ) const
{
   PCL_CropEngine::Apply( image, *this );
}

void Crop::Apply( pcl::UInt8Image& image ) const
{
   PCL_CropEngine::Apply( image, *this );
}

void Crop::Apply( pcl::UInt16Image& image ) const
{
   PCL_CropEngine::Apply( image, *this );
}

void Crop::Apply( pcl::UInt32Image& image ) const
{
   PCL_CropEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Crop.cpp - Released 2017-06-09T08:12:54Z
