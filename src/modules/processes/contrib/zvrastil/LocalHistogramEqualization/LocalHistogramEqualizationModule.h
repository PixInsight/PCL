// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard LocalHistogramEqualization Process Module Version 01.00.00.0092
// ****************************************************************************
// LocalHistogramEqualizationModule.h - Released 2014/10/29 07:35:26 UTC
// ****************************************************************************
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2014 Zbynek Vrastil
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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
// ****************************************************************************

#ifndef __LocalHistogramEqualizationModule_h
#define __LocalHistogramEqualizationModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// LocalHistogramEqualizationModule
// ----------------------------------------------------------------------------

class LocalHistogramEqualizationModule : public MetaModule
{
public:

   LocalHistogramEqualizationModule();

   virtual const char* Version() const;
   virtual IsoString Name() const;
   virtual String Description() const;
   virtual String Company() const;
   virtual String Author() const;
   virtual String Copyright() const;
   virtual String TradeMarks() const;
   virtual String OriginalFileName() const;
   virtual void GetReleaseDate( int& year, int& month, int& day ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LocalHistogramEqualizationModule_h

// ****************************************************************************
// EOF LocalHistogramEqualizationModule.h - Released 2014/10/29 07:35:26 UTC
