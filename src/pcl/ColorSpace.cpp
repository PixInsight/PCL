//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/ColorSpace.cpp - Released 2017-08-01T14:23:38Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/ImageColor.h>

namespace pcl
{
   namespace ColorSpace
   {
      String Name( int s )
      {
         static const char* name[] =
         { "Grayscale", "RGB", "CIE XYZ", "CIE L*a*b*", "CIE L*c*h*", "HSV", "HSI" };
         PCL_PRECONDITION( s >= 0 && s < NumberOfColorSpaces )
         if ( s >= 0 && s < NumberOfColorSpaces )
            return String( name[s] );
         return String();
      }

      String ChannelId( int s, int c )
      {
         static const char* id[3][7] = { { "K", "R", "X", "L*", "L*", "H", "H" },
                                         { "",  "G", "Y", "a*", "c*", "S", "S" },
                                         { "",  "B", "Z", "b*", "h*", "V", "I" } };
         PCL_PRECONDITION( s >= 0 && s < NumberOfColorSpaces )
         PCL_PRECONDITION( c >= 0 )
         if ( s >= 0 && s < NumberOfColorSpaces && c >= 0 )
         {
            if ( s == Gray && c > 0 )
               c += 2;
            if ( c < 3 )
               return String( id[c][s] );
            return String().Format( "alpha%02d", c-2 );
         }
         return String();
      }
   } // ColorSpace
} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ColorSpace.cpp - Released 2017-08-01T14:23:38Z
