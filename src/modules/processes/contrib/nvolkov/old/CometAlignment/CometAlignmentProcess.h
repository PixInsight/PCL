// ****************************************************************************
// PixInsight Class Library - PCL 02.00.14.0695
// Standard CometAlignment Process Module Version 01.02.02.0065
// ****************************************************************************
// CometAlignmentProcess.h - Released 2015/02/06 19:50:08 UTC
// ****************************************************************************
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#ifndef __CometAlignmentProcess_h
#define __CometAlignmentProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

  // ----------------------------------------------------------------------------

  class CometAlignmentProcess : public MetaProcess
  {
  public:

    CometAlignmentProcess ();

    virtual IsoString Id () const;
    virtual IsoString Category () const;

    virtual uint32 Version () const;

    virtual String Description () const;

    virtual const char** IconImageXPM() const;

    virtual ProcessInterface* DefaultInterface () const;

    virtual ProcessImplementation* Create () const;
    virtual ProcessImplementation* Clone (const ProcessImplementation&) const;
  };

  PCL_BEGIN_LOCAL
          extern CometAlignmentProcess* TheCometAlignmentProcess;
  PCL_END_LOCAL

  // ----------------------------------------------------------------------------

} // pcl

#endif   // __CometAlignmentProcess_h

// ****************************************************************************
// EOF CometAlignmentProcess.h - Released 2015/02/06 19:50:08 UTC
