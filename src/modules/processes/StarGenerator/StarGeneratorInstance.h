//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0243
// ----------------------------------------------------------------------------
// StarGeneratorInstance.h - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __StarGeneratorInstance_h
#define __StarGeneratorInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------
// StarGeneratorInstance
// ----------------------------------------------------------------------------

class StarGeneratorInstance : public ProcessImplementation
{
public:

   StarGeneratorInstance( const MetaProcess* );
   StarGeneratorInstance( const StarGeneratorInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   String   starDatabasePath;    // path to the star database file
   double   centerRA;            // center right ascension, degrees
   double   centerDec;           // center declination, degrees
   double   epoch;               // picture epoch, JD
   pcl_enum projectionSystem;    // conformal/automatic or Gnomonic
   float    focalLength;         // effective focal length in mm
   float    pixelSize;           // pixel size, micrometers
   int32    sensorWidth;         // sensor width in pixels
   int32    sensorHeight;        // sensor height in pixels
   float    limitMagnitude;      // limiting magnitude
   float    starFWHM;            // star FWHM, arcseconds
   pcl_bool nonlinear;           // apply a nonlinear transformation (output image only)
   float    targetMinimumValue;  // target minimum sample value for nonlinear output (output image only)
   pcl_enum outputMode;          // generate an output image or a CSV text file
   String   outputFilePath;      // path to the output CSV file

   friend class StarGeneratorEngine;
   friend class StarGeneratorProcess;
   friend class StarGeneratorInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __StarGeneratorInstance_h

// ----------------------------------------------------------------------------
// EOF StarGeneratorInstance.h - Released 2015/12/18 08:55:08 UTC
