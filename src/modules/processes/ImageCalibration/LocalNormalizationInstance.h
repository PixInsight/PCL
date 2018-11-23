//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0352
// ----------------------------------------------------------------------------
// LocalNormalizationInstance.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __LocalNormalizationInstance_h
#define __LocalNormalizationInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class LocalNormalizationInstance : public ProcessImplementation
{
public:

   LocalNormalizationInstance( const MetaProcess* );
   LocalNormalizationInstance( const LocalNormalizationInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual bool IsHistoryUpdater( const View& ) const;
   virtual UndoFlags UndoMode( const View& ) const;
   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool ExecuteOn( View& );
   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();
   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter*, size_type tableRow ) const;

private:

   struct Item
   {
      pcl_bool enabled = true;
      String   path;

      Item() = default;
      Item( const Item& ) = default;

      Item( const String& path_ ) : path( path_ )
      {
      }

      bool IsDefined() const
      {
         return !path.IsEmpty();
      }
   };

   typedef Array<Item>  item_list;

   // Working parameters
   int32       p_scale;                      // working scale in pixels
   pcl_bool    p_noScale;                    // only compute offset component with scale = 1
   pcl_bool    p_rejection;
   float       p_backgroundRejectionLimit;
   float       p_referenceRejectionThreshold;
   float       p_targetRejectionThreshold;
   int32       p_hotPixelFilterRadius;
   int32       p_noiseReductionFilterRadius;

   // Working images
   String      p_referencePathOrViewId;
   pcl_bool    p_referenceIsView;
   item_list   p_targets;

   // Format hints
   String      p_inputHints;
   String      p_outputHints;

   // Working modes
   pcl_enum    p_generateNormalizedImages;   // apply to target images
   pcl_bool    p_generateNormalizationData;  // generate .xnml files
   pcl_bool    p_showBackgroundModels;
   pcl_bool    p_showRejectionMaps;
   pcl_enum    p_plotNormalizationFunctions;
   pcl_bool    p_noGUIMessages;

   // Output images
   String      p_outputDirectory;
   String      p_outputExtension;
   String      p_outputPrefix;
   String      p_outputPostfix;
   pcl_bool    p_overwriteExistingFiles;
   pcl_enum    p_onError;

   // High-level parallelism
   pcl_bool    p_useFileThreads;
   float       p_fileThreadOverload;
   int32       p_maxFileReadThreads;
   int32       p_maxFileWriteThreads;

   // Graph options
   int32       p_graphSize;
   int32       p_graphTextSize;
   int32       p_graphTitleSize;
   pcl_bool    p_graphTransparent;
   String      p_graphOutputDirectory;

   void ApplyErrorPolicy();

   friend class LocalNormalizationThread;
   friend class LocalNormalizationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LocalNormalizationInstance_h

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInstance.h - Released 2018-11-23T18:45:58Z
