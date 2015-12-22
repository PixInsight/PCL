//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0318
// ----------------------------------------------------------------------------
// DrizzleIntegrationInstance.h - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __DrizzleIntegrationInstance_h
#define __DrizzleIntegrationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>
#include <pcl/Matrix.h>

#include "DrizzleIntegrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class DrizzleIntegrationInstance : public ProcessImplementation
{
public:

   DrizzleIntegrationInstance( const MetaProcess* );
   DrizzleIntegrationInstance( const DrizzleIntegrationInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   struct DataItem
   {
      pcl_bool enabled;
      String   path; // drizzle data file

      DataItem( const String& p = String() ) : enabled( true ), path( p )
      {
      }

      DataItem( const DataItem& x ) : enabled( x.enabled ), path( x.path )
      {
      }

      bool IsDefined() const
      {
         return !path.IsEmpty();
      }
   };

   typedef Array<DataItem>  input_data_list;

   input_data_list p_inputData;            // input .drz files
   String          p_inputHints;           // input format hints
   String          p_inputDirectory;       // if nonempty, replaces dirs in .drz files
   float           p_scale;                // input_pixel_size/output_pixel_size
   float           p_dropShrink;           // pixel shrink factor
   pcl_bool        p_enableRejection;      // enable pixel rejection
   pcl_bool        p_enableImageWeighting; // enable image weights
   pcl_bool        p_enableSurfaceSplines; // enable registration surface splines
   pcl_bool        p_useROI;               // use a region of interest
   Rect            p_roi;                  // region of interest
   pcl_bool        p_closePreviousImages;  // close existing integration and weight images before running
   pcl_bool        p_noGUIMessages;        // only show errors on the console
   pcl_enum        p_onError;              // error policy

   /*
    * Read-only output properties
    */

   struct OutputData
   {
      // General integration data

      String     integrationImageId;       // identifier of the output drizzle integrated image
      String     weightImageId;            // identifier of the output drizzle weight image
      int32      numberOfChannels;         // number of pixel components
      uint64     outputPixels;             // area of the integrated image in square pixels
      uint64     integratedPixels;         // total integrated source pixels
      UI64Vector totalRejectedLow;         // per-channel total low rejected pixels
      UI64Vector totalRejectedHigh;        // per-channel total high rejected pixels
      float      outputData;                // total data gathered in input image units

      // Per-channel data for each integrated image

      struct ImageData
      {
         String     filePath;              // source image file
         FVector    weight;                // image weight (only if p_generateIntegratedImage)
         DVector    location;              // location estimates
         DVector    referenceLocation;     // reference location estimates
         DVector    scale;                 // scaling factors
         UI64Vector rejectedLow;           // number of low rejected pixels
         UI64Vector rejectedHigh;          // number of high rejected pixels
         float      outputData;             // total data gathered from this image in [0,1]

         ImageData( const String& a_filePath = String() ) :
            filePath( a_filePath ),
            weight( 0, 3 ),
            location( 0, 3 ),
            referenceLocation( 0, 3 ),
            scale( 0, 3 ),
            rejectedLow( 0, 3 ),
            rejectedHigh( 0, 3 ),
            outputData( 0 )
         {
         }

         ImageData( const ImageData& x )
         {
            (void)operator =( x );
         }

         ImageData& operator =( const ImageData& x )
         {
            filePath = x.filePath;
            weight = x.weight;
            location = x.location;
            referenceLocation = x.referenceLocation;
            scale = x.scale;
            rejectedLow = x.rejectedLow;
            rejectedHigh = x.rejectedHigh;
            outputData = x.outputData;
            return *this;
         }
      };

      Array<ImageData> imageData;

      OutputData() :
         integrationImageId(),
         weightImageId(),
         numberOfChannels( 0 ),
         outputPixels( 0 ),
         integratedPixels( 0 ),
         totalRejectedLow( 0, 3 ),
         totalRejectedHigh( 0, 3 ),
         outputData( 0 ),
         imageData()
      {
      }

      OutputData( const OutputData& x )
      {
         (void)operator =( x );
      }

      OutputData& operator =( const OutputData& x )
      {
         integrationImageId = x.integrationImageId;
         weightImageId = x.weightImageId;
         numberOfChannels = x.numberOfChannels;
         outputPixels = x.outputPixels;
         integratedPixels = x.integratedPixels;
         totalRejectedLow = x.totalRejectedLow;
         totalRejectedHigh = x.totalRejectedHigh;
         outputData = x.outputData;
         imageData = x.imageData;
         return *this;
      }
   };

   OutputData o_output;

   friend class DrizzleIntegrationEngine;
   friend class DrizzleIntegrationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DrizzleIntegrationInstance_h

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationInstance.h - Released 2015/12/18 08:55:08 UTC
