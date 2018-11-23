//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0427
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsInstance.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __ExtractAlphaChannelsInstance_h
#define __ExtractAlphaChannelsInstance_h

#include <pcl/ProcessImplementation.h>

#include "ExtractAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ExtractAlphaChannelsInstance : public ProcessImplementation
{
public:

   ExtractAlphaChannelsInstance( const MetaProcess* );
   ExtractAlphaChannelsInstance( const ExtractAlphaChannelsInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual bool Validate( String& info );
   virtual bool IsMaskable( const View&, const ImageWindow& ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;
   virtual UndoFlags UndoMode( const View& ) const;
   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );
   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   static void ParseChannelList( SortedArray<int>&, const String& );

private:

   pcl_enum channels;
   String   channelList;
   pcl_bool extractChannels;
   pcl_bool deleteChannels;

   friend class ExtractAlphaChannelsProcess;
   friend class ExtractAlphaChannelsInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExtractAlphaChannelsInstance_h

// ----------------------------------------------------------------------------
// EOF ExtractAlphaChannelsInstance.h - Released 2018-11-23T18:45:58Z
