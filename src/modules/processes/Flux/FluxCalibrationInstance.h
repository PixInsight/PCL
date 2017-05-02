//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0154
// ----------------------------------------------------------------------------
// FluxCalibrationInstance.h - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#ifndef __FluxCalibrationInstance_h
#define __FluxCalibrationInstance_h

#include "FluxCalibrationParameters.h"

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------
// FluxCalibrationInstance
// ----------------------------------------------------------------------------

class FluxCalibrationInstance : public ProcessImplementation
{
public:

   FluxCalibrationInstance( const MetaProcess* );
   FluxCalibrationInstance( const FluxCalibrationInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual UndoFlags UndoMode( const View& ) const;

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   struct CalibrationParameter
   {
      float       value;
      pcl_enum    mode;
      String      keyword;
      //
      String stdKeyword;

      float GetValue( const FITSKeywordArray& keywords ) const
      {
         if ( mode == pcl_enum( FCParameterMode::Literal ) )
            return value;

         for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
            if ( mode == pcl_enum( FCParameterMode::StandardKeyword ) && String( i->name ) == stdKeyword ||
                 mode == pcl_enum( FCParameterMode::CustomKeyword ) && String( i->name ) == keyword )
            {
               float retVal;
               if ( i->value.TryToFloat( retVal ) )
                  return retVal;
            }

         return -1;
      }

      float operator =( float x )
      {
         return value = x;
      }
   };

   CalibrationParameter p_wavelength;            // nm
   CalibrationParameter p_transmissivity;        // [0,1]
   CalibrationParameter p_filterWidth;           // nm
   CalibrationParameter p_aperture;              // mm    *
   CalibrationParameter p_centralObstruction;    // mm    *
   CalibrationParameter p_exposureTime;          // s     *
   CalibrationParameter p_atmosphericExtinction; // [0,1]
   CalibrationParameter p_sensorGain;            //       *
   CalibrationParameter p_quantumEfficiency;     // [0,1]

   friend class FluxCalibrationEngine;
   friend class FluxCalibrationProcess;
   friend class FluxCalibrationInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __FluxCalibrationInstance_h

// ----------------------------------------------------------------------------
// EOF FluxCalibrationInstance.h - Released 2017-05-02T09:43:00Z
