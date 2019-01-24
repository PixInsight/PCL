//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0282
// ----------------------------------------------------------------------------
// ConvolutionDialog.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ConvolutionDialog_h
#define __ConvolutionDialog_h

#include "ConvolutionInstance.h"
#include "FilterLibrary.h"

#include <pcl/KernelFilter.h>
#include <pcl/SeparableFilter.h>
#include <pcl/Label.h>
#include <pcl/Dialog.h>
#include <pcl/Array.h>
#include <pcl/String.h>
#include <pcl/TextBox.h>
#include <pcl/PushButton.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvolutionFilterCodeDialog : public Dialog
{
public:

   ConvolutionFilterCodeDialog( String mode, const Filter& f );

   const Filter& BeingEdited() const
   {
      return filter;
   }

private:

   int line, column;
   Filter filter;

   VerticalSizer  Global_Sizer;
      TextBox           FilterCode_TextBox;
      HorizontalSizer   LineCol_Sizer;
         Label          LineCol_Label;
      HorizontalSizer   DialogButtons_Sizer;
         PushButton     Save_PushButton;
         PushButton     Cancel_PushButton;

   void __Button_Click( Button& sender, bool checked );
   void __TextBox_CaretPosition( TextBox& sender, int oldPos, int newPos );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ConvolutionDialog_h

// ----------------------------------------------------------------------------
// EOF ConvolutionDialog.h - Released 2019-01-21T12:06:41Z
