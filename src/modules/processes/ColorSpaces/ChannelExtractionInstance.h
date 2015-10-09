//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0258
// ----------------------------------------------------------------------------
// ChannelExtractionInstance.h - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#ifndef __ChannelExtractionInstance_h
#define __ChannelExtractionInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// ChannelExtractionInstance
// ----------------------------------------------------------------------------

class ChannelExtractionInstance : public ProcessImplementation
{
public:

   ChannelExtractionInstance( const MetaProcess* );
   ChannelExtractionInstance( const ChannelExtractionInstance& );

   virtual bool Validate( pcl::String& info );

   virtual void Assign( const ProcessImplementation& );

   virtual bool IsHistoryUpdater( const View& ) const;
   virtual bool IsMaskable( const View&, const ImageWindow& ) const;

   virtual bool CanExecuteOn( const View& v, String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   // -------------------------------------------------------------------------

   int ColorSpace() const
   {
      return colorSpace;
   }

   bool IsChannelEnabled( int c ) const
   {
      return channelEnabled[c];
   }

   const String& ChannelId( int c ) const
   {
      return channelId[c];
   }

   int SampleFormat() const
   {
      return sampleFormat;
   }

   // -------------------------------------------------------------------------

private:

   pcl_enum colorSpace;
   pcl_bool channelEnabled[ 3 ]; // PCL MetaBoolean maps to uint32
   String   channelId[ 3 ];
   pcl_enum sampleFormat;

   friend class ChannelExtractionInterface;
   friend class ExtractCIELAction;
   friend class SplitRGBChannelsAction;
   friend class ExtractRedChannelAction;
   friend class ExtractGreenChannelAction;
   friend class ExtractBlueChannelAction;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelExtractionInstance_h

// ----------------------------------------------------------------------------
// EOF ChannelExtractionInstance.h - Released 2015/10/08 11:24:39 UTC
