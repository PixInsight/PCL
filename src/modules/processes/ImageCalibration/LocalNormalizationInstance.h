//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0300
// ----------------------------------------------------------------------------
// LocalNormalizationInstance.h - Released 2017-05-17T17:41:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __LocalNormalizationInstance_h
#define __LocalNormalizationInstance_h

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

//    struct Item
//    {
//       pcl_bool enabled  = true;
//       pcl_bool isFile   = false;
//       String   idOrPath;
//
//       Item() = default;
//
//       Item( bool isFile_, const String& idOrPath_ ) : isFile( isFile_ ), idOrPath( idOrPath_ )
//       {
//       }
//
//       Item( const Item& x ) = default;
//
//       bool IsDefined() const
//       {
//          return !idOrPath.IsEmpty();
//       }
//    };
//
//    typedef Array<Item>     item_list;

   int32       p_scale;

   // Working images
   String      p_referenceViewId;
//    Item        p_reference;
//    item_list   p_targets;

   // Format hints
//    String      p_inputHints;
//    String      p_outputHints;

   friend class LocalNormalizationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LocalNormalizationInstance_h

// ----------------------------------------------------------------------------
// EOF LocalNormalizationInstance.h - Released 2017-05-17T17:41:56Z
