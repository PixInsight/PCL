//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/TextAlign.h - Released 2015/12/17 18:52:09 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_TextAlign_h
#define __PCL_TextAlign_h

/// \file pcl/TextAlign.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Flags_h
#include <pcl/Flags.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace TextAlign
 * \brief     Text alignment modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>TextAlign::Left</td>       <td>Left alignment</td></tr>
 * <tr><td>TextAlign::Right</td>      <td>Right alignment</td></tr>
 * <tr><td>TextAlign::HorzCenter</td> <td>Centered horizontally</td></tr>
 * <tr><td>TextAlign::Justify</td>    <td>Left and right justified</td></tr>
 * <tr><td>TextAlign::Top</td>        <td>Top alignment</td></tr>
 * <tr><td>TextAlign::Bottom</td>     <td>Bottom alignment</td></tr>
 * <tr><td>TextAlign::VertCenter</td> <td>Centered vertically</td></tr>
 * <tr><td>TextAlign::Center</td>     <td>Centered horizontally and vertically. Equal to HorzCenter|VertCenter.</td></tr>
 * <tr><td>TextAlign::Default</td>    <td>Default alignment. Equal to Left|Top.</td></tr>
 * <tr><td>TextAlign::Unknown</td>    <td>Unknown or unsupported alignment</td></tr>
 * </table>
 */
namespace TextAlign
{
   enum mask_type
   {
      Left        = 0x01,
      Right       = 0x02,
      HorzCenter  = 0x04,
      Justify     = 0x08,
      Top         = 0x20,
      Bottom      = 0x40,
      VertCenter  = 0x80,
#if defined( _MSC_VER ) && _MSC_VER >= 1700 // VC++ >= 2012
      Center      = 132,
      Default     = 33,
#else
      Center      = HorzCenter|VertCenter,
      Default     = Left|Top,
#endif
      Unknown     = 0x00
   };
}

/*!
 * A collection of text alignment mode flags.
 */
typedef Flags<TextAlign::mask_type> TextAlignmentFlags;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_TextAlign_h

// ----------------------------------------------------------------------------
// EOF pcl/TextAlign.h - Released 2015/12/17 18:52:09 UTC
