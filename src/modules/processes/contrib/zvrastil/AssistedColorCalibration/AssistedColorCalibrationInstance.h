// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard AssistedColorCalibration Process Module Version 01.00.00.0098
// ****************************************************************************
// AssistedColorCalibrationInstance.h - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2014 Zbynek Vrastil
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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

#ifndef __AssistedColorCalibrationInstance_h
#define __AssistedColorCalibrationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------
// AssistedColorCalibrationInstance
// ----------------------------------------------------------------------------

class AssistedColorCalibrationInstance : public ProcessImplementation
{
public:

   AssistedColorCalibrationInstance( const MetaProcess* );
   AssistedColorCalibrationInstance( const AssistedColorCalibrationInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   void GetWBCorrectionFactors( double &red, double &green, double &blue ) const;
   const String& GetBackgroundReference() const;
   void GetHistogramTransformation( double &shadows, double &highlights, double &midtones ) const;
   double GetSaturationBoost() const;

   void CalculateBackgroundReference( double &red, double &green, double &blue ) const;
   void CorrectBackgroundReference( double &red, double &green, double &blue ) const;

private:

   float    redCorrectionFactor;         // white balance RED correction factor
   float    greenCorrectionFactor;       // white balance GREEN correction factor
   float    blueCorrectionFactor;        // white balance BLUE correction factor

   String   backgroundReference;         // view with background reference (view's full id)

   float    histogramShadows;            // shadows clipping value of histogram transformation
   float    histogramHighlights;         // highlights clipping value of histogram transformation
   float    histogramMidtones;           // midtones balance of histogram transformation

   float    saturationBoost;             // color saturation enhancement factor

   friend class AssistedColorCalibrationProcess;
   friend class AssistedColorCalibrationInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __AssistedColorCalibrationInstance_h

// ****************************************************************************
// EOF AssistedColorCalibrationInstance.h - Released 2014/11/14 17:19:24 UTC
