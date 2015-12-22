//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/UCCaseFolding.cpp - Released 2015/12/17 18:52:18 UTC
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

/*
 * PCL Unicode case folding selection routine.
 * Automatically generated on Fri, 20 Mar 2015 16:18:43 GMT
 */

#include <pcl/Defs.h>

namespace pcl
{

char16_type PCL_FUNC PCL_ToCaseFolded( char16_type c )
{
   if ( c < 1022 )
   {
      if ( c < 461 )
      {
         if ( c < 400 )
         {
            if ( c < 377 )
            {
               if ( c < 256 )
               {
                  if ( c < 192 )
                  {
                     if ( c >= 65 && c <= 90 )
                        return c + 32;
                     if ( c == 181 )
                        return 956;
                  }
                  else
                  {
                     if ( c >= 192 && c <= 214 )
                        return c + 32;
                     if ( c >= 216 && c <= 222 )
                        return c + 32;
                  }
               }
               else
               {
                  if ( c < 313 )
                  {
                     if ( c >= 256 && c <= 302 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 306 || c == 308 || c == 310 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c >= 313 && c <= 327 && (c & 1) != 0 )
                        return c + 1;
                     if ( c >= 330 && c <= 374 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 376 )
                        return 255;
                  }
               }
            }
            else
            {
               if ( c < 390 )
               {
                  if ( c < 385 )
                  {
                     if ( c == 377 || c == 379 || c == 381 )
                        return c + 1;
                     if ( c == 383 )
                        return 115;
                  }
                  else
                  {
                     if ( c == 385 )
                        return 595;
                     if ( c == 386 || c == 388 )
                        return c + 1;
                  }
               }
               else
               {
                  if ( c < 393 )
                  {
                     if ( c == 390 )
                        return 596;
                     if ( c == 391 || c == 395 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 393 || c == 394 )
                        return c + 205;
                     if ( c == 398 )
                        return 477;
                     if ( c == 399 )
                        return 601;
                  }
               }
            }
         }
         else
         {
            if ( c < 422 )
            {
               if ( c < 406 )
               {
                  if ( c < 403 )
                  {
                     if ( c == 400 )
                        return 603;
                     if ( c == 401 || c == 408 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 403 )
                        return 608;
                     if ( c == 404 )
                        return 611;
                  }
               }
               else
               {
                  if ( c < 413 )
                  {
                     if ( c == 406 || c == 412 )
                        return c + 211;
                     if ( c == 407 )
                        return 616;
                  }
                  else
                  {
                     if ( c == 413 )
                        return 626;
                     if ( c == 415 )
                        return 629;
                     if ( c == 416 || c == 418 || c == 420 )
                        return c + 1;
                  }
               }
            }
            else
            {
               if ( c < 437 )
               {
                  if ( c < 430 )
                  {
                     if ( c == 422 || c == 425 )
                        return c + 218;
                     if ( c == 423 || c == 428 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 430 )
                        return 648;
                     if ( c == 431 || c == 435 )
                        return c + 1;
                     if ( c == 433 || c == 434 )
                        return c + 217;
                  }
               }
               else
               {
                  if ( c < 444 )
                  {
                     if ( c == 437 || c == 440 )
                        return c + 1;
                     if ( c == 439 )
                        return 658;
                  }
                  else
                  {
                     if ( c == 444 || c == 453 )
                        return c + 1;
                     if ( c == 452 || c == 455 || c == 458 )
                        return c + 2;
                     if ( c == 456 || c == 459 )
                        return c + 1;
                  }
               }
            }
         }
      }
      else
      {
         if ( c < 886 )
         {
            if ( c < 570 )
            {
               if ( c < 502 )
               {
                  if ( c < 497 )
                  {
                     if ( c >= 461 && c <= 475 && (c & 1) != 0 )
                        return c + 1;
                     if ( c >= 478 && c <= 494 && (c & 1) == 0 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 497 )
                        return 499;
                     if ( c == 498 || c == 500 )
                        return c + 1;
                  }
               }
               else
               {
                  if ( c < 504 )
                  {
                     if ( c == 502 )
                        return 405;
                     if ( c == 503 )
                        return 447;
                  }
                  else
                  {
                     if ( c >= 504 && c <= 542 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 544 || c == 1021 )
                        return c - 130;
                     if ( c >= 546 && c <= 562 && (c & 1) == 0 )
                        return c + 1;
                  }
               }
            }
            else
            {
               if ( c < 580 )
               {
                  if ( c < 573 )
                  {
                     if ( c == 570 )
                        return 11365;
                     if ( c == 571 || c == 577 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 573 )
                        return 410;
                     if ( c == 574 )
                        return 11366;
                     if ( c == 579 )
                        return 384;
                  }
               }
               else
               {
                  if ( c < 582 )
                  {
                     if ( c == 580 )
                        return 649;
                     if ( c == 581 )
                        return 652;
                  }
                  else
                  {
                     if ( c >= 582 && c <= 590 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 837 || c == 895 )
                        return c + 116;
                     if ( c == 880 || c == 882 )
                        return c + 1;
                  }
               }
            }
         }
         else
         {
            if ( c < 977 )
            {
               if ( c < 910 )
               {
                  if ( c < 904 )
                  {
                     if ( c == 886 || c == 962 )
                        return c + 1;
                     if ( c == 902 )
                        return 940;
                  }
                  else
                  {
                     if ( c == 904 || c == 905 || c == 906 )
                        return c + 37;
                     if ( c == 908 )
                        return 972;
                  }
               }
               else
               {
                  if ( c < 931 )
                  {
                     if ( c == 910 || c == 911 )
                        return c + 63;
                     if ( c >= 913 && c <= 929 )
                        return c + 32;
                  }
                  else
                  {
                     if ( c >= 931 && c <= 939 )
                        return c + 32;
                     if ( c == 975 )
                        return 983;
                     if ( c == 976 )
                        return 946;
                  }
               }
            }
            else
            {
               if ( c < 1009 )
               {
                  if ( c < 982 )
                  {
                     if ( c == 977 )
                        return 952;
                     if ( c == 981 )
                        return 966;
                  }
                  else
                  {
                     if ( c == 982 )
                        return 960;
                     if ( c >= 984 && c <= 1006 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 1008 )
                        return 954;
                  }
               }
               else
               {
                  if ( c < 1013 )
                  {
                     if ( c == 1009 )
                        return 961;
                     if ( c == 1012 )
                        return 952;
                  }
                  else
                  {
                     if ( c == 1013 )
                        return 949;
                     if ( c == 1015 || c == 1018 )
                        return c + 1;
                     if ( c == 1017 || c == 8172 )
                        return c - 7;
                  }
               }
            }
         }
      }
   }
   else
   {
      if ( c < 8490 )
      {
         if ( c < 8008 )
         {
            if ( c < 4256 )
            {
               if ( c < 1162 )
               {
                  if ( c < 1040 )
                  {
                     if ( c == 1022 || c == 1023 )
                        return c - 130;
                     if ( c >= 1024 && c <= 1039 )
                        return c + 80;
                  }
                  else
                  {
                     if ( c >= 1040 && c <= 1071 )
                        return c + 32;
                     if ( c >= 1120 && c <= 1152 && (c & 1) == 0 )
                        return c + 1;
                  }
               }
               else
               {
                  if ( c < 1217 )
                  {
                     if ( c >= 1162 && c <= 1214 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 1216 )
                        return 1231;
                  }
                  else
                  {
                     if ( c >= 1217 && c <= 1229 && (c & 1) != 0 )
                        return c + 1;
                     if ( c >= 1232 && c <= 1326 && (c & 1) == 0 )
                        return c + 1;
                     if ( c >= 1329 && c <= 1366 )
                        return c + 48;
                  }
               }
            }
            else
            {
               if ( c < 7840 )
               {
                  if ( c < 7680 )
                  {
                     if ( c >= 4256 && c <= 4293 )
                        return c + 7264;
                     if ( c == 4295 || c == 4301 )
                        return c + 7264;
                  }
                  else
                  {
                     if ( c >= 7680 && c <= 7828 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 7835 )
                        return 7777;
                     if ( c == 7838 )
                        return 223;
                  }
               }
               else
               {
                  if ( c < 7960 )
                  {
                     if ( c >= 7840 && c <= 7934 && (c & 1) == 0 )
                        return c + 1;
                     if ( c >= 7944 && c <= 7951 )
                        return c - 8;
                  }
                  else
                  {
                     if ( c >= 7960 && c <= 7965 )
                        return c - 8;
                     if ( c >= 7976 && c <= 7983 )
                        return c - 8;
                     if ( c >= 7992 && c <= 7999 )
                        return c - 8;
                  }
               }
            }
         }
         else
         {
            if ( c < 8126 )
            {
               if ( c < 8088 )
               {
                  if ( c < 8040 )
                  {
                     if ( c >= 8008 && c <= 8013 )
                        return c - 8;
                     if ( c >= 8025 && c <= 8031 && (c & 1) != 0 )
                        return c - 8;
                  }
                  else
                  {
                     if ( c >= 8040 && c <= 8047 )
                        return c - 8;
                     if ( c >= 8072 && c <= 8079 )
                        return c - 8;
                  }
               }
               else
               {
                  if ( c < 8120 )
                  {
                     if ( c >= 8088 && c <= 8095 )
                        return c - 8;
                     if ( c >= 8104 && c <= 8111 )
                        return c - 8;
                  }
                  else
                  {
                     if ( c == 8120 || c == 8121 )
                        return c - 8;
                     if ( c == 8122 || c == 8123 )
                        return c - 74;
                     if ( c == 8124 || c == 8140 )
                        return c - 9;
                  }
               }
            }
            else
            {
               if ( c < 8170 )
               {
                  if ( c < 8152 )
                  {
                     if ( c == 8126 )
                        return 953;
                     if ( c >= 8136 && c <= 8139 )
                        return c - 86;
                  }
                  else
                  {
                     if ( c == 8152 || c == 8153 )
                        return c - 8;
                     if ( c == 8154 || c == 8155 )
                        return c - 100;
                     if ( c == 8168 || c == 8169 )
                        return c - 8;
                  }
               }
               else
               {
                  if ( c < 8186 )
                  {
                     if ( c == 8170 || c == 8171 )
                        return c - 112;
                     if ( c == 8184 || c == 8185 )
                        return c - 128;
                  }
                  else
                  {
                     if ( c == 8186 || c == 8187 )
                        return c - 126;
                     if ( c == 8188 )
                        return 8179;
                     if ( c == 8486 )
                        return 969;
                  }
               }
            }
         }
      }
      else
      {
         if ( c < 11506 )
         {
            if ( c < 11364 )
            {
               if ( c < 8579 )
               {
                  if ( c < 8498 )
                  {
                     if ( c == 8490 )
                        return 107;
                     if ( c == 8491 )
                        return 229;
                  }
                  else
                  {
                     if ( c == 8498 )
                        return 8526;
                     if ( c >= 8544 && c <= 8559 )
                        return c + 16;
                  }
               }
               else
               {
                  if ( c < 11264 )
                  {
                     if ( c == 8579 || c == 11360 )
                        return c + 1;
                     if ( c >= 9398 && c <= 9423 )
                        return c + 26;
                  }
                  else
                  {
                     if ( c >= 11264 && c <= 11310 )
                        return c + 48;
                     if ( c == 11362 )
                        return 619;
                     if ( c == 11363 )
                        return 7549;
                  }
               }
            }
            else
            {
               if ( c < 11376 )
               {
                  if ( c < 11373 )
                  {
                     if ( c == 11364 )
                        return 637;
                     if ( c == 11367 || c == 11369 || c == 11371 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 11373 )
                        return 593;
                     if ( c == 11374 )
                        return 625;
                     if ( c == 11375 )
                        return 592;
                  }
               }
               else
               {
                  if ( c < 11390 )
                  {
                     if ( c == 11376 )
                        return 594;
                     if ( c == 11378 || c == 11381 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 11390 || c == 11391 )
                        return c - 10815;
                     if ( c >= 11392 && c <= 11490 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 11499 || c == 11501 )
                        return c + 1;
                  }
               }
            }
         }
         else
         {
            if ( c < 42893 )
            {
               if ( c < 42802 )
               {
                  if ( c < 42624 )
                  {
                     if ( c == 11506 || c == 42560 )
                        return c + 1;
                     if ( c >= 42562 && c <= 42604 && (c & 1) == 0 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c >= 42624 && c <= 42650 && (c & 1) == 0 )
                        return c + 1;
                     if ( c >= 42786 && c <= 42798 && (c & 1) == 0 )
                        return c + 1;
                  }
               }
               else
               {
                  if ( c < 42877 )
                  {
                     if ( c >= 42802 && c <= 42862 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 42873 || c == 42875 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c == 42877 )
                        return 7545;
                     if ( c >= 42878 && c <= 42886 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 42891 || c == 42896 )
                        return c + 1;
                  }
               }
            }
            else
            {
               if ( c < 42924 )
               {
                  if ( c < 42904 )
                  {
                     if ( c == 42893 )
                        return 613;
                     if ( c == 42898 || c == 42902 )
                        return c + 1;
                  }
                  else
                  {
                     if ( c >= 42904 && c <= 42920 && (c & 1) == 0 )
                        return c + 1;
                     if ( c == 42922 )
                        return 614;
                     if ( c == 42923 )
                        return 604;
                  }
               }
               else
               {
                  if ( c < 42928 )
                  {
                     if ( c == 42924 )
                        return 609;
                     if ( c == 42925 )
                        return 620;
                  }
                  else
                  {
                     if ( c == 42928 )
                        return 670;
                     if ( c == 42929 )
                        return 647;
                     if ( c >= 65313 && c <= 65338 )
                        return c + 32;
                  }
               }
            }
         }
      }
   }
   return c;
}

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/UCCaseFolding.cpp - Released 2015/12/17 18:52:18 UTC
