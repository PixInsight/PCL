//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0352
// ----------------------------------------------------------------------------
// DynamicPSFInstance.h - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __DynamicPSFInstance_h
#define __DynamicPSFInstance_h

#include <pcl/ProcessImplementation.h>

#include "StarDetector.h"
#include "PSF.h"
#include "DynamicPSFParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// DynamicPSFInstance
// ----------------------------------------------------------------------------

typedef GenericPoint<int32>   Point32;

class DynamicPSFInstance : public ProcessImplementation
{
public:

   DynamicPSFInstance( const MetaProcess* );
   DynamicPSFInstance( const DynamicPSFInstance& );

   virtual ~DynamicPSFInstance();

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View& v, String& whyNot ) const;

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   void AssignOptions( const DynamicPSFInstance& );

   // -------------------------------------------------------------------------

private:

   /*
    * View identifiers
    */
   StringList  views;

   /*
    * Stars
    */
   struct Star : public StarData
   {
      uint32      view;          // index of the view to which this star pertains

      Star() : StarData(), view( 0 )
      {
      }

      Star( const StarData& data, uint32 v ) : StarData( data ), view( v )
      {
      }

      Star( const Star& x ) : StarData( x ), view( x.view )
      {
      }
   };
   Array<Star>    stars;

   /*
    * PSF fittings
    */
   struct PSF : public PSFData
   {
      uint32      star;          // index of the fitted star

      PSF() : PSFData(), star( 0 )
      {
      }

      PSF( const PSFData& data, uint32 s ) : PSFData( data ), star( s )
      {
      }

      PSF( const PSF& x ) : PSFData( x ), star( x.star )
      {
      }
   };
   Array<PSF>     psfs;

   /*
    * PSF fitting functions
    */
   struct PSFOptions
   {
      pcl_bool  autoPSF;
      pcl_bool  circular;
      pcl_bool  gaussian;
      pcl_bool  moffat;
      pcl_bool  moffatA;
      pcl_bool  moffat8;
      pcl_bool  moffat6;
      pcl_bool  moffat4;
      pcl_bool  moffat25;
      pcl_bool  moffat15;
      pcl_bool  lorentzian;
   };
   PSFOptions     psfOptions;

   pcl_bool       signedAngles;  // show rotation angles in [0,180[ or ]-90,+90]

   pcl_bool       regenerate;    // do a regeneration or a recalculation in ExecuteGlobal()?

   /*
    * Star detection parameters
    */
   int32          searchRadius;  // star search radius in pixels
   float          threshold;     // background threshold, sigma units
   pcl_bool       autoAperture;  // automatic sampling aperture

   /*
    * Image scale parameters
    */
   pcl_enum       scaleMode;     // std keyword (FOCALLEN), custom keyword or literal value
   float          scaleValue;    // image scale in arc seconds per pixel
   String         scaleKeyword;  // custom image scale keyword

   /*
    * Interface options
    */
   uint32         starColor;              // drawing color, normal stars
   uint32         selectedStarColor;      // drawing color, selected star
   uint32         selectedStarFillColor;  // fill color, selected star
   uint32         badStarColor;           // drawing color, failed fit
   uint32         badStarFillColor;       // filling color, failed fit

   friend class DynamicPSFInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicPSFInstance_h

// ----------------------------------------------------------------------------
// EOF DynamicPSFInstance.h - Released 2016/02/21 20:22:43 UTC
