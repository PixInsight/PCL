//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0191
// ----------------------------------------------------------------------------
// SplitCFAInstance.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2018 Nikolay Volkov
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#ifndef __SplitCFAInstance_h
#define __SplitCFAInstance_h

#include <pcl/FileFormatInstance.h>
#include <pcl/ProcessImplementation.h>

#include "SplitCFAParameters.h"

//#define debug 1

namespace pcl
{

// ----------------------------------------------------------------------------

class SplitCFAThread;
struct SplitCFAThreadData;

typedef IndirectArray<SplitCFAThread> thread_list;

class SplitCFAInstance : public ProcessImplementation
{
public:

   SplitCFAInstance( const MetaProcess* );
   SplitCFAInstance( const SplitCFAInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool ExecuteOn(View&);
   virtual bool IsHistoryUpdater( const View& v ) const;

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   struct ImageItem
   {
      String   path;    // absolute file path
      String   folder;  // absolute file folder
      pcl_bool enabled; // if disabled, skip (ignore) this image

      ImageItem( const String& p = String(), const String& f = String() ) :
         path( p ),
         folder( f ),
         enabled( true )
      {
      }

      ImageItem( const ImageItem& ) = default;
   };

#define outputExtension ".xisf"

   typedef Array<ImageItem> image_list;

   image_list p_targetFrames;
   pcl_bool   p_outputTree;
   pcl_bool   p_outputSubDirCFA;
   String     p_outputDir;
   pcl_bool   p_overwrite;
   String     p_prefix;
   String     p_postfix;

   String     o_outputViewId[ 4 ];

   template <class P>
   void SplitCFAViewImage( const GenericImage<P>& source, const View& view ); // for ExecuteOn()

   thread_list LoadTargetFrame( size_t fileIndex );
   String      OutputFilePath( const String&, const String&, const size_t, const int );
   void        SaveImage( const SplitCFAThread* );

   friend class SplitCFAThread;
   friend class SplitCFAInterface;
};

// ----------------------------------------------------------------------------
} // pcl

#endif   // __SplitCFAInstance_h

// ----------------------------------------------------------------------------
// EOF SplitCFAInstance.h - Released 2019-01-21T12:06:42Z
