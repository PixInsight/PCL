//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.06.00.0273
// ----------------------------------------------------------------------------
// DebayerInstance.h - Released 2017-07-18T16:14:19Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#ifndef __DebayerInstance_h
#define __DebayerInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileFormatInstance;

class DebayerInstance : public ProcessImplementation
{
public:

   DebayerInstance( const MetaProcess* );
   DebayerInstance( const DebayerInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool IsHistoryUpdater( const View& ) const;
   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

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

   // Process parameters
   pcl_enum   p_bayerPattern;
   pcl_enum   p_debayerMethod;
   pcl_bool   p_evaluateNoise;
   pcl_enum   p_noiseEvaluationAlgorithm;
   pcl_bool   p_showImages;        // optional for view execution only
   String     p_cfaSourceFilePath; // ...
   item_list  p_targets;
   pcl_bool   p_noGUIMessages;

   // Format hints
   String      p_inputHints;
   String      p_outputHints;

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

   // Read-only output properties, view execution.
   String     o_imageId;
   FVector    o_noiseEstimates;
   FVector    o_noiseFractions;
   StringList o_noiseAlgorithms;

   // Read-only output properties, batch execution.
   struct OutputFileData
   {
      String     filePath;
      FVector    noiseEstimates = FVector( 0.0F, 3 );
      FVector    noiseFractions = FVector( 0.0F, 3 );
      StringList noiseAlgorithms = StringList( size_type( 3 ) );
   };
   Array<OutputFileData> o_outputFileData;

   pcl_enum CFAPatternFromTarget( const View& ) const;
   pcl_enum CFAPatternFromTarget( FileFormatInstance& ) const;
   static pcl_enum CFAPatternFromTargetProperty( const Variant& );

   void ApplyErrorPolicy();

   friend class DebayerProcess;
   friend class DebayerInterface;
   friend class DebayerEngine;
   friend class DebayerFileThread;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __DebayerInstance_h

// ----------------------------------------------------------------------------
// EOF DebayerInstance.h - Released 2017-07-18T16:14:19Z
