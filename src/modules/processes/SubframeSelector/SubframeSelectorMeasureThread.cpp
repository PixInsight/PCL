//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureThread.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#include "SubframeSelectorMeasureThread.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorParameters.h"

#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorMeasureThread::SubframeSelectorMeasureThread( Image* subframe, MeasureData* outputData, const String& subframePath,
                                                              int subimageIndex, const MeasureThreadInputData& data ) :
   m_subframe( subframe ),
   m_outputData( outputData ),
   m_subframePath( subframePath ),
   m_subimageIndex( subimageIndex ),
   m_success( false ),
   m_data( data )
{
}

void SubframeSelectorMeasureThread::Run()
{
   try
   {
      m_success = false;
      m_subframe->Status().DisableInitialization();
      Console().WriteLn( String().Format( "<end><cbr><br>Image Data: %u", m_subframe->BitsPerSample() ) );
      m_success = true;
   }
   catch ( ... )
   {
      ClearConsoleOutputText();
      Console().WriteLn( "Thread error!" );
      try
      {
         throw;
      }
      ERROR_HANDLER
   }
}

const MeasureThreadInputData& SubframeSelectorMeasureThread::MeasuringData() const
{
   return m_data;
}

const Image* SubframeSelectorMeasureThread::SubframeImage() const
{
   return m_subframe.Pointer();
}

String SubframeSelectorMeasureThread::SubframePath() const
{
   return m_subframePath;
}

const MeasureData& SubframeSelectorMeasureThread::OutputData() const
{
   return *m_outputData;
}

int SubframeSelectorMeasureThread::SubimageIndex() const
{
   return m_subimageIndex;
}

bool SubframeSelectorMeasureThread::Success() const
{
   return m_success;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasureThread.cpp - Released 2017-08-01T14:26:58Z
