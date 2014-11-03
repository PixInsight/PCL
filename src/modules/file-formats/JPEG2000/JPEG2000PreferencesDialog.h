// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard JPEG2000 File Format Module Version 01.00.01.0212
// ****************************************************************************
// JPEG2000PreferencesDialog.h - Released 2014/10/29 07:34:49 UTC
// ****************************************************************************
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __JPEG2000PreferencesDialog_h
#define __JPEG2000PreferencesDialog_h

#include "JPEG2000OptionsDialog.h"
#include "JPEG2000Format.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// JPEG2000PreferencesDialog
// ----------------------------------------------------------------------------

class JPEG2000PreferencesDialog: public JPEG2000OptionsDialog
{
public:

   JPEG2000PreferencesDialog( const JP2Format::EmbeddingOverrides&, const JPEG2000ImageOptions&, bool isJPC );

   virtual ~JPEG2000PreferencesDialog()
   {
   }

   JP2Format::EmbeddingOverrides overrides;

private:

   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEG2000PreferencesDialog_h

// ****************************************************************************
// EOF JPEG2000PreferencesDialog.h - Released 2014/10/29 07:34:49 UTC
